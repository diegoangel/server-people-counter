/*!
 * \file
 *
 * Blackfin Server
 * ----------------
 *
 * \copyright   Movilgate 2015
 */

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

#include <BlackfinServer.h>
#include <easylogging++.h>
#include <json.hpp>
#include <libmemcached/memcached.h>
#include <math.h>
#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <sstream>
#include <algorithm>
#include <syslog.h>
#include <time.h>
#include <stdlib.h>

// "JSON for modern C++"" directives

using json = nlohmann::json;

// Easylogging++ directives

#define ELPP_NO_DEFAULT_LOG_FILE

//! \note sysctl -w net.core.somaxconn=1024   ADD net.core.somaxconn=1024 to /etc/sysctl.conf for it to become permanent 

inline std::string RetrieveCounterData(Irisys::Blackfin* bf);

/*!
 * \brief   Utility function for calling nanosleep
 */
void threadsleep(i_uint32 dwMilliseconds)
{
    timespec spec;
    spec.tv_sec = dwMilliseconds / 1000;
    spec.tv_nsec = (dwMilliseconds - (spec.tv_sec*1000)) * 1000000;
    nanosleep(&spec,0);
}

/*!
 * \brief   Entry point for the thread
 */
void *threadStart(void *param)
{
    BlackfinServer* pServer = reinterpret_cast<BlackfinServer*>(param);
    if ( pServer )
    {
        pServer->run();
        pthread_detach(pServer->m_threadHandle);
        pServer->m_threadHandle = 0;
    }

    return 0;
}

/*!
 * \brief   Constructor
 */
BlackfinServer::BlackfinServer(void)
{
    m_threadRunning = false;
    m_threadDone = true;
    m_threadHandle = 0;
    pthread_mutex_init(&m_lock,0);
}

/*!
 * \brief   Destructor
 */
BlackfinServer::~BlackfinServer(void)
{
    while ( m_threadHandle )
        threadsleep(1);
    pthread_mutex_unlock(&m_lock);
    pthread_mutex_destroy(&m_lock);
}

/*!
 * \brief   Binds the server socket and starts the thread to process the connected devices
 */
bool BlackfinServer::StartServer(const std::string &currentRunningProgramPath)
{
    const std::string abConfigFilePath = currentRunningProgramPath + std::string("/../../conf/servicioConteoPersonas.conf");
    std::ifstream conf(abConfigFilePath);
    std::string content(
        (std::istreambuf_iterator<char>(conf)),
        (std::istreambuf_iterator<char>())
    );
    json config = json::parse(content);

    // Want to start off the socket server here
    std::string api = m_engine.GetAPIVersion();
    LOG(INFO) << "Using API version " << api;

    m_serverSocket = -1;
    int status = -1;
    int reuse_addr = 1;

    addrinfo hint;
    addrinfo *list = 0;

    // Need to determine if we are using an ip address or a hostname
    memset(&hint, 0, sizeof(hint));

    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;

    std::string confServerIP = config["server"]["ip"];
    std::string confServerPort = config["server"]["port"];
    int retval = getaddrinfo(confServerIP.c_str(), confServerPort.c_str(), &hint, &list);

    if (retval != 0)
    {
        LOG(ERROR) << "Could not resolve host address ";
        return false;
    }

    // Create a socket
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == -1)
    {
        LOG(ERROR) << "Failed to create socket";
        freeaddrinfo(list);
        return false;
    }

    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<const char*> (&reuse_addr), sizeof(reuse_addr));

    // Bind socket to the address
    status = bind(m_serverSocket, list->ai_addr,
            static_cast<int> (list->ai_addrlen));

    freeaddrinfo(list);
    if (status != -1)
    {
        // Set it as listen socket, 128 connection limit
        status = listen(m_serverSocket, SOMAXCONN);
        if (status == -1)
        {
            LOG(ERROR) << "Could not start listening server ";
            return false;
        }
        LOG(INFO) << "Server will allow up to a maximum of " << SOMAXCONN
            << " concurrent connections";
    }
    else
    {
        LOG(ERROR) << "Could not bind Server Socket ";
        return false;
    }

    m_engine.StartEngine();

    // Start thread
    if (pthread_create(&m_threadHandle, 0, threadStart, (void*)this)!= 0)
    {
        LOG(ERROR) <<  "Failed to start server thread";
        exit(0);
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

/*!
 * \brief   Shutdown the server
 */
void BlackfinServer::ShutdownServer()
{
    m_threadRunning = false;

    while (!m_threadDone)
        threadsleep(10);

    m_engine.ShutdownEngine();

    pthread_mutex_lock(&(m_lock));
    for ( std::vector<BlackfinData*>::iterator it = m_blackfinList.begin(); it != m_blackfinList.end(); )
        RemoveConnection(*it);
    m_blackfinList.clear();
    pthread_mutex_unlock(&(m_lock));
}

// -------------------------------------------------------------------------------------------------

/*!
 * \brief   This thread run method polls each counter in the BlackfinData list
 *          every 10 seconds for data
 * \return  int
 */
i_uint32 BlackfinServer::run()
{
    m_threadRunning = true;
    m_threadDone = false;
    time_t start_time = time(NULL);
    threadsleep(1000);

    while (m_threadRunning)
    {
        // check if any blackfin units need removing from service list
        // must lock to prevent any concurrent access to blackfin list
        // in case we have to edit it
        pthread_mutex_lock(&(m_lock));
        for ( std::vector<BlackfinData*>::iterator it = m_blackfinList.begin(); it != m_blackfinList.end(); )
        {
            if ( (*it)->needsRemoving )
            {
                RemoveConnection(*it);
                it = m_blackfinList.erase(it);
                continue;
            }

            // Increment the iterator when we didn't remove an element
            ++it;
        }
        pthread_mutex_unlock(&(m_lock));

        // print a status line every 10 seconds
        if (difftime(time(NULL), start_time) > 10.0f) {
            LOG(DEBUG) << m_blackfinList.size() << " units connected";

            // This vector could be added to by the main thread, but stepping through by index should be safe
            for (unsigned short i = 0; i < m_blackfinList.size(); i++) {
                std::string ip;
                m_blackfinList[i]->unit->GetIPAddress(ip);
                LOG(DEBUG) << ip;
            }
            start_time = time(NULL);
        }
        // retrieve info for each blackfin unit every 30 seconds
        pthread_mutex_lock(&(m_lock));
        std::vector<BlackfinData *>::iterator iter = m_blackfinList.begin();
        while (iter != m_blackfinList.end()) {
            if (difftime(time(NULL), (*iter)->startTime) > 30.0f) { // every thirty seconds
                Irisys::Blackfin *bf = (*iter)->unit;
                // Retrieve data serialized in JSON format
                std::string serializedCounterData = RetrieveCounterData(bf);
                std::string IPAddress;
                bf->GetIPAddress(IPAddress);
                // Storing retrieved data in memcached
                BlackfinServer::Save(IPAddress, serializedCounterData);
                LOG(DEBUG) << serializedCounterData;
                if (BlackfinServer::HasResetCurrentCount("ResetCurrentCount")) 
                {
                    if(!bf->ResetCurrentCount()) {
                        LOG(INFO) << "ResetCurrentCount call failed";
                    }
                    else {
                        LOG(INFO) << "Count successfully reset";
                    }
                }


#ifdef DISCONNECT_MODE
                (*iter)->needsRemoving = true;
#endif
                (*iter)->startTime = time(NULL);
            }
            iter++;
        }
        pthread_mutex_unlock(&(m_lock));
        threadsleep(10);
    }
    m_threadDone = true;
    return 0;
}

/*! 
 * \brief   Method where all the information is gathered from the unit
 * \return string
 */
inline std::string RetrieveCounterData(Irisys::Blackfin* bf) {
    json j;
    /// GetDeviceID
    std::string deviceID;
    if (bf->GetDeviceID(deviceID)) {
        j[deviceID]["deviceID"] = deviceID;
    } else {
        LOG(DEBUG) << "Failed to get device ID ";

    }
    /// GetDeviceName
    std::string deviceName;
    if (bf->GetDeviceName(deviceName)) {
        j[deviceID]["deviceName"] = deviceName;
    } else {
        LOG(DEBUG) << "Failed to get Device Name";
    } 
    /// GetUserString
    std::string userString;
    if (bf->GetUserString(userString)) {
        j[deviceID]["userString"] = userString;
    } else {
        LOG(DEBUG) << "Failed to get User String";
    }
    /// GetMACAddress
    std::string macAddress;
    if (bf->GetMACAddress(macAddress)) {
        j[deviceID]["macAddress"] = macAddress;
    } else {
        LOG(DEBUG) << "Failed to get MAC address";

    }
    /// GetIPAddress
    std::string IPAddress;
    if (bf->GetIPAddress(IPAddress)) {
        j[deviceID]["IPADdress"] = IPAddress;
    } else {
        LOG(DEBUG) << "Failed to get IP Address";
    }
    /// GetUnitTimeZone
    int tz_id;
    bool applyDST;
    if (bf->GetUnitTimeZone(tz_id, applyDST)) {
        j[deviceID]["timeZone"] = Irisys::IRISYS_TIMEZONES[tz_id].m_strDescription;
        j[deviceID]["DST"] = applyDST;
    } else {
        LOG(DEBUG) << "GetUnitTimeZone call failed";
    }
    /// GetUpTime
    i_uint64 upTime;
    if (bf->GetUpTime(&upTime)) {
        j[deviceID]["upTime"] = upTime;    
    } else {
        LOG(DEBUG) << "Failed to get up time";
    }
    /// GetSiteName
    std::string siteName;
    if (bf->GetSiteName(siteName)) {
        j[deviceID]["siteName"] = siteName;
    } else {
        LOG(DEBUG) << "Failed to get site name";
    }
    /// GetCountLogPeriod
    unsigned int countLogPeriod = 0;
    if (bf->GetCountLogPeriod(countLogPeriod)) {
        j[deviceID]["countLogPeriod"] = countLogPeriod;
    } else {
        LOG(DEBUG) << "Failed to get countlog period";
    }

    // GetCurrentCount
    Irisys::Blackfin::Count currentCount;
    if (bf->GetCurrentCount(currentCount)) {

        j[deviceID]["Timestamp"] = currentCount.countTime;

        std::vector<i_uint32>::iterator countIt;

        /// GetRegisterLabels
        std::vector<std::string> labels;

        if(!bf->GetRegisterLabels(labels)) {
            LOG(DEBUG) << "Failed to get registered labels";
        } else {
            std::vector<std::string>::iterator registerLabelsIt;
            registerLabelsIt = labels.begin();
            for (countIt = currentCount.countLines.begin(); countIt != currentCount.countLines.end(); ++countIt) {
                int index = (countIt - currentCount.countLines.begin());
                j[deviceID]["count"][registerLabelsIt[index]] = *countIt;
            }
        }
    } else {
        LOG(DEBUG) << "Failed to get current count";
    }

    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------


    // GetLastNCounts
    std::vector<Irisys::Blackfin::Count> countData;
    if (bf->GetLastNCounts(10, countData)) {
        //std::cout << "Got " << countData.size() << " counts " << std::endl;

        std::vector<Irisys::Blackfin::Count>::iterator it;

        int ii = 0;

        for (it = countData.begin(); it != countData.end(); ++it) {
            threadsleep(20);
            //std::cout << "Timestamp " << (*it).countTime << " C: ";
            //j[deviceID]["lastNCounts"][ii]["timestamp"] = (*it).countTime;

            std::vector<i_uint32>::iterator countIt;
            int i = 1;
            for (countIt = (*it).countLines.begin(); countIt
                    != (*it).countLines.end(); ++countIt) {
                //std::cout << (*countIt) << " ";
                if (i == 1) {
                    //j[deviceID]["lastNCounts"][ii]["in"] = (*countIt);
                } else {
                    //j[deviceID]["lastNCounts"][ii]["out"] = (*countIt);
                }
                i++;
            }
            //std::cout << std::endl;
            ii++;
        }
    } else {
        std::cout << "Failed to get counts " << std::endl;

    }
    // GetDeviceStatus
    Irisys::Blackfin::DeviceStatus deviceStatus;
    if (bf->GetDeviceStatus(deviceStatus)) {
        std::vector<Irisys::Blackfin::DeviceLogEntry>::iterator deviceIt;

        j[deviceID]["NumErrors"] = deviceStatus.m_errorList.size();
        //std::cout << "Got device status " << std::endl;

        //std::cout << "Num Errors " << deviceStatus.m_errorList.size() << std::endl;
        //std::cout << "Num Warnings " << deviceStatus.m_warnList.size() << std::endl;
        //std::cout << "Errors " << std::endl;

        j[deviceID]["errorList"] = nullptr;

        for (deviceIt = deviceStatus.m_errorList.begin(); deviceIt
                != deviceStatus.m_errorList.end(); ++deviceIt) {
            //std::cout << "TS: " << (*deviceIt).timestamp << " Desc: "
            //<< (*deviceIt).errorDescription << std::endl;

        }

        //std::cout << "Warnings " << std::endl;

        for (deviceIt = deviceStatus.m_warnList.begin(); deviceIt
                != deviceStatus.m_warnList.end(); ++deviceIt) {
            //std::cout << "TS: " << (*deviceIt).timestamp << " Desc: "
            //<< (*deviceIt).errorDescription << std::endl;
        }

    } else {
        //std::cout << "Failed to get device status " << std::endl;

    }

    std::ostringstream deviceIDtoSet;

    deviceIDtoSet << "549638130622280"; // << rand;

    if (bf->SetDeviceID(deviceIDtoSet.str())) {
        //std::cout << "Set device id " << std::endl;
    } else {
        //std::cout << "Failed to set device id " << std::endl;

    }

    time_t unitTime = 0;

    if (bf->GetUnitTime(unitTime)) {
        //std::cout << "Got unit time " << unitTime << std::endl;

        //std::cout << "Attempting to get counts " << std::endl;

        j[deviceID]["unitTime"] = unitTime;

        std::vector<Irisys::Blackfin::Count> timedCounts;

        if (bf->GetCounts((unitTime - 120), unitTime, timedCounts)) {
            //std::cout << "Got timed counts " << std::endl;

            //std::cout << "Got " << timedCounts.size() << " counts " << std::endl;

            std::vector<Irisys::Blackfin::Count>::iterator it;

            for (it = timedCounts.begin(); it != timedCounts.end(); ++it) {
                threadsleep(10);
                //std::cout << "Timestamp " << (*it).countTime << " C: ";



                std::vector<i_uint32>::iterator countIt;

                for (countIt = (*it).countLines.begin(); countIt
                        != (*it).countLines.end(); ++countIt) {
                    //std::cout << (*countIt) << " ";
                }

                //std::cout << std::endl;
            }

        } else {
            std::cout << "failed to get timed counts " << std::endl;

        }
    } else {
        std::cout << "Failed to get unit time " << std::endl;

    }
    // JSON
    //json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    //std::cout << j.dump(4) << std::endl;

    return j.dump();
}

// -------------------------------------------------------------------------------------------------

/*!
 * \brief   Remove connection
 * \return  void
 */
void BlackfinServer::RemoveConnection ( BlackfinData* pData )
{
    std::cout << "Removing connection from " << pData->address << std::endl;
    m_engine.RemoveBlackfinEndPoint(pData->unit);
    delete pData->unit;
    close(pData->sockd);
    delete pData;
}

// -------------------------------------------------------------------------------------------------

/*!
 * \brief   Blackfin error handler
 * \return  void
 */
void BlackfinServer::BlackfinError(Irisys::Blackfin *device, const std::string &errorString) 
{

    std::cout << errorString << std::endl;

    //remove from all the lists and clean up
    std::vector<BlackfinData *>::iterator it;

    pthread_mutex_lock(&(m_lock));

    for (it = m_blackfinList.begin(); it != m_blackfinList.end(); ++it) {
        if ((*it)->unit == device) {
            (*it)->needsRemoving = true;
            std::cout << "marked device for removal" << std::endl;
            break;
        }
    }

    pthread_mutex_unlock(&(m_lock));
}

// -------------------------------------------------------------------------------------------------

/*!
 * \brief   Called by the main thread to check for and process new connections
 * \return  void
 */
void BlackfinServer::HandleIncomingConnections()
{
    FD_ZERO(&m_read_socket_set);
    FD_ZERO(&m_error_socket_set);

    if (m_serverSocket != -1)
    {
        FD_SET(m_serverSocket, &m_read_socket_set);
        FD_SET(m_serverSocket, &m_error_socket_set);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int returnVal = select(FD_SETSIZE, &m_read_socket_set, 0,
                &m_error_socket_set, &timeout);
        if (returnVal == -1) {
            LOG(ERROR) << "Select reported an error";
            return;
        } else if (returnVal > 0) {
            // check for error set
            if (FD_ISSET(m_serverSocket, &m_error_socket_set)) {
                LOG(ERROR) << "Triggered select error state";
                return;
            }

            // check if readable socket
            if (FD_ISSET(m_serverSocket, &m_read_socket_set)) {
                LOG(DEBUG) << "Connection detected......";

                //We have a connection do some stuff with it
                struct sockaddr_in saddr;
                SOCKET sockd = -1;
                int size = 0;

                memset(&saddr, 0, sizeof(saddr));

                size = sizeof(saddr);
                sockd = accept(m_serverSocket, (struct sockaddr*) &saddr,
                        (socklen_t*) &size);

                if (sockd != -1)
                {
                    char addrBuf[25];
                    sprintf ( addrBuf, "%s:%d", inet_ntoa(saddr.sin_addr), saddr.sin_port );

                    // Create our blackfin object hook it up to the engine
                    Irisys::Blackfin* pBf = new Irisys::Blackfin();
                    if (m_engine.AddBlackfinEndPoint(pBf, sockd, this))
                    {
                        // Add the new connection to the list so it gets processed by the thread
                        pthread_mutex_lock(&(m_lock));
                        m_blackfinList.push_back(new BlackfinData(pBf, addrBuf, sockd));
                        LOG(DEBUG) << "Blackfin unit connected: " << addrBuf;
                        pthread_mutex_unlock(&(m_lock));
                    }
                    else
                    {
                        delete pBf;
                        close(sockd);
                        LOG(DEBUG) << "Blackfin unit from " << addrBuf << " failed to connect";
                    }
                }
            }
        }
    }
}

/*!
 * \brief   Save data to memcached
 * \return  bool
 */
bool BlackfinServer::Save(std::string key, std::string value) 
{
    //connect server

    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *memserver;
    time_t expiration;
    uint32_t  flags;

    memc = memcached_create(NULL);
    memserver = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, memserver);
    memcached_server_list_free(memserver);

    //std::string key = "user";
    //std::string value = "diego diego";
    size_t value_length = value.length();
    size_t key_length = key.length();

    //std::cout << "KEY: " << key << std::endl;

    //Save data  
    rc = memcached_set(memc, key.c_str(), key.length(), value.c_str(), value.length(), 800, flags);
    if(rc == MEMCACHED_SUCCESS)  
    {
        std::cout << "Save data:" << value <<" sucessful!"<< std::endl;  
    }

    //Get data  
    char* result = memcached_get(memc, key.c_str(), key_length, &value_length, &flags, &rc);
    if(rc == MEMCACHED_SUCCESS)  
    {
        std::cout << "Get value:" << result << " sucessful!" << std::endl;
    }

    memcached_free(memc);      

    return true;
}

/*!
 * \brief   Delet data from memcached
 * \return  bool
 */
bool BlackfinServer::HasResetCurrentCount(const std::string &key) 
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *memserver;

    memc = memcached_create(NULL);
    memserver = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, memserver);
    memcached_server_list_free(memserver);

    size_t key_length = key.length();
    size_t return_value_length;
    uint32_t flags = 0;
    time_t expiration;

    //Get data  
    char* result = memcached_get(memc, key.c_str(), key_length, &return_value_length, &flags, &rc);
    if(rc == MEMCACHED_SUCCESS)  
    {
        std::cout << "HasResetCurrentCount" << result << " sucessful!" << std::endl;


        //Delete data  
        rc = memcached_delete(memc,key.c_str(),key_length,expiration);  
        if(rc == MEMCACHED_SUCCESS) 
        {  
            std::cout<<"Delete key:"<<key<<" sucessful!"<< std::endl;  
        } 

        memcached_free(memc);   

        return true;   

    }

    memcached_free(memc); 

    return false;
}


enum class UnitModifiers : uint32_t
{
    SetDeviceID = 1,
    ResetCountLogs = 2,
    SetPacketTimeout = 3,
    ResetCurrentCount = 4,
    SetCountLogPeriod = 5,
    SetDeviceName = 6,
};
