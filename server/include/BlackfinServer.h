#pragma once

#ifdef linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>

typedef int SOCKET;

#endif

#include <iostream>

#include <Blackfin.h>
#include <BlackfinEngine.h>
#include <IrisysTimeZone.h>
#include <IBlackfinErrorHandler.h>
#include <PlatformDataTypes.h>

/*! 
 * Uncomment this define to enable disconnect mode - this causes the server to close the
 * connection to a Blackfin unit once it has gotten data from it once, rather than holding
 * the connection open indefinitely
 */

 // #define DISCONNECT_MODE

class BlackfinServer : public Irisys::IBlackfinErrorHandler
{
    public:
        BlackfinServer(void);
        ~BlackfinServer(void);

        bool StartServer(const std::string &currentRunningProgramPath);
        void ShutdownServer();
        void HandleIncomingConnections();
        bool Save(std::string key, std::string value);
        bool HasResetCurrentCount(const std::string &key);
        i_uint32 run();

        //! Check if the server is running
        inline bool isRunning() { return m_threadRunning; }

    protected:

        struct BlackfinData
        {
            Irisys::Blackfin *unit;
            bool needsRemoving;
            time_t startTime;
            std::string address;
            SOCKET sockd;

            BlackfinData ( Irisys::Blackfin* pBf, std::string addr, SOCKET s )
            {
                unit          = pBf;
                needsRemoving = false;
                startTime     = time(NULL);
                address       = addr;
                sockd         = s;
            }
        };

        //! Closes the connection to a Blackfin device and cleans up allocated memory
        void RemoveConnection ( BlackfinData* pData );

        void BlackfinError(Irisys::Blackfin *device, const std::string &errorString);

        Irisys::BlackfinEngine m_engine;
        std::vector<BlackfinData *> m_blackfinList;

        bool m_threadRunning;
        bool m_threadDone;

        SOCKET m_serverSocket;
        fd_set m_read_socket_set;
        fd_set m_write_socket_set;
        fd_set m_error_socket_set;

        pthread_mutex_t m_lock;
    public:
        pthread_t m_threadHandle;
};
