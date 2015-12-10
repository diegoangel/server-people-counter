/*!
 * \file
 * 
 *  MovilgateServerApp.cpp : Defines the entry point for the console application.
 * -----------------------------------------------------------------------------
 *
 * \warning     This program wasn't compiled or tested in Windows and may not be fully compatible. 
 *              in fact, I'm really sure that it's not... sincerely yours, El Ziscador.
 * \copyright   Movilgate 2015
 *
 * This application uses a two thread approach to handle client ip connections.
 * The main thread polls the socket for incoming connection requests, and when one
 * arrives, it creates a Blackfin object and adds it to a list to be polled for data
 * at a regular interval by the server thread. This is especially suitable for embedded
 * systems, although it should be noted that each Blackfin object is based upon a threaded model
 * and so a new thread will be spawned for every current connection.
 *
 * An alternative to this is to spawn a new "action" thread every time a client connection
 * is made in order to handle the specific needs of counter data retrieval.
 *
 */

/*!
 * \note Order of includes: Local includes, Third party includes and STL.
 */
#include <easylogging++.h>
#include <json.hpp>
#include <BlackfinServer.h>
#include <iostream>
#include <fstream>  
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <syslog.h>
#include <libgen.h>

// "JSON for modern C++"" directives

using json = nlohmann::json;

// Easylogging++ directives

#define ELPP_THREAD_SAFE
#define ELPP_NO_DEFAULT_LOG_FILE

INITIALIZE_EASYLOGGINGPP

// VAR DECLARATIONS /////////////////////////////////////

/*! 
 * \var     BlackfinServer
 * \brief   Automatic creation of a new Blackfin instance.
 */
BlackfinServer server;
/*!
 * \var     static volatile sig_atomic_t
 * \brief   Flag for the main loop. 
 */
static volatile sig_atomic_t dieTotenHosenRules = 1;
/*! 
 * \var     std::fstream
 * \brief   Filename passed by command line in which the pid will be stored
 */
std::fstream pidfile;
/*! 
 * \var     std::fstream
 * \brief   Filename passed by command line in which the logs will be stored
 */
std::fstream logfile;
/*!
 * \var std::string
 * \brief Path of easylogging++ config file
 */
const std::string easyloggingConfigFile = "/../conf/easylogging++.conf";

// FUNCTIONS DECLARATIONS ///////////////////////////////

#ifdef linux
void signalHandler(int type);
#else
BOOL WINAPI signalHandler(DWORD type);
#endif

void initLogger(const std::string &currentRunningProgramPath, const std::string &easyloggingConfigFile);

std::string getSelfPath();

// AND HERE IS WHERE ALL BEGINS... //////////////////////

int main(int argc, char* argv[]) 
{
    openlog (argv[0], LOG_PID, LOG_DAEMON);
    if (argc < 2) {
        //! Expected parameters: argv[1] pidfile, argv[2] logfile
       syslog(LOG_WARNING, "No parameters were received from command line. Taking default values from config file.");
    } else {
        pidfile.open(argv[1], std::fstream::trunc | std::fstream::in | std::fstream::out);
        if (!pidfile.is_open()) {
            syslog(LOG_ERR, "Could not open pidfile received trough command line");
            exit(EXIT_FAILURE);
        }
    }

    /*! 
     * \note    IMPORTANT! all futures paths in the program will be built relative to this one.
     *          Return the dirname where the executable program resides.
     *          e.g. This function should return something like /program/executable/absolute/path/server/bin                
     */  
    const std::string currentRunningProgramPath = getSelfPath();

    //! \todo Read config settings from file here.

    // BEGIN DAEMONIZING OF THE PROCESS

    pid_t pid, sid;

    // Fork the parent
    pid = fork();

    if (pid < 0) {      
        exit(EXIT_FAILURE);
    }

    // Exit the parent
    if (pid > 0) {           
        exit(EXIT_SUCCESS);
    }

    sid = setsid();

    if (sid < 0) {
        syslog(LOG_ERR, "Failure on creating new session for the process");
        exit(EXIT_FAILURE);
    }

    // Fork off for the second time.
    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    // Success: Let the parent terminate.
    if (pid > 0) {
        //! Write the pid into pidfile and exit.       
        pidfile << std::to_string(pid) << std::endl;
        pidfile.close();        
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask.
    umask(0);

    // Initializing logger facility
    initLogger(currentRunningProgramPath, easyloggingConfigFile);

    // Setting up the signal handler
#ifdef linux
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signalHandler;
    // Registering Linux handlers
    sigaction(SIGTSTP, &sa, NULL);        
    sigaction(SIGHUP, &sa, NULL); 
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#else
    // Registering Windows handler
    SetConsoleCtrlHandler(signalHandler, true);
#endif

    // Change the current working directory
    if ((chdir("/")) < 0) {
        LOG(ERROR) << "Can't change the current process working directory";
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // FINISH DAEMONIZING OF THE PROCESS

    if (server.StartServer(currentRunningProgramPath)) {
        LOG(INFO) << "Server started";
        // Main loop
        while (dieTotenHosenRules) {
            server.HandleIncomingConnections(); // Handle, retrieve an store data from units
            sleep(10); // sleep 10 segundos
        }
    } else {
        LOG(ERROR) << "Server failed on start";
    }
    LOG(INFO) << "Server terminated";
    closelog();
    exit(EXIT_SUCCESS);
}

// FUNCTIONS DEFINITIONS ////////////////////////////////

#ifdef linux
/*!
 * \fn          signalHandler(int type)  
 * \brief       Linux signals handler
 * \description Funcion para gestion de señales utilizando la utilidad sigaction().
 *              Las señales suspension de ejecucion (Ctrl+Z) y el cierre de la ventana de la terminal
 *              son ignoradas.
 * \return      void
 */
void signalHandler(int type) 
{
    switch (type) {
        case SIGHUP:
            LOG(DEBUG) << "Terminal was closed, BlackfinServer keep working in background...";  
            break;
        case SIGTSTP:
            LOG(DEBUG) << "Ignoring the attempt to suspend the execution, BlackfinServer keep running...";            
            break;
        case SIGINT:
            if (server.isRunning()) {            
                LOG(DEBUG) << "CTRL+C signal received, quitting BlackfinServer...";
                server.ShutdownServer();
            } 
            dieTotenHosenRules = 0;
            exit(EXIT_SUCCESS);
        case SIGTERM:
            if (server.isRunning()) {
                LOG(DEBUG) << "Kill pid signal received, quitting BlackfinServer...";
                server.ShutdownServer();
            }  
            dieTotenHosenRules = 0;
            exit(EXIT_SUCCESS);            
        default:
            LOG(DEBUG) << "Caught wrong signal";
            return;
    }
}
#else
/*!
 * \fn          signalHandler(DWORD type)
 * \brief       Windows signals handler
 * \description Funcion para gestion de señales.
 *              Las señales suspension de ejecucion (Ctrl+Z) y el cierre de la ventana de la terminal
 *              son ignoradas.
 *              Para que la funcion se ejecute aun, si ningun usuario esta logueado, se debe ejecutar  
 *              utilizando  Windows Task Scheduler.
 * \sa          https://msdn.microsoft.com/en-us/library/windows/desktop/ms686016%28v=vs.85%29.aspx
 *              http://asawicki.info/news_1465_handling_ctrlc_in_windows_console_application.html
 * \warning     Esta funcion no esta testeada.
 * \return      bool
 */
BOOL WINAPI signalHandler(DWORD type)
{
    bool ret;

    switch (type)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT: 
            if (server.isRunning()) {
                dieTotenHosenRules = 0;
                LOG(INFO) << "Ctrl+C or Ctrl+Break signal received, quitting BlackfinServer...";
                server.ShutdownServer();
            }
            ret = true;
            break;
        case CTRL_CLOSE_EVENT:
            LOG(DEBUG) << "Terminal was closed, BlackfinServer keep working in background...";          
            ret false;
            break;
        case CTRL_LOGOFF_EVENT:
            LOG(INFO) << "User loging off signal received, I hope you've run this service using Windows Task Scheduler....";    
            ret false;
            break;
        case CTRL_SHUTDOWN_EVENT:
            if (server.isRunning()) {    
                dieTotenHosenRules = 0;        
                LOG(INFO) << "Shutting down signal received, quitting BlackfinServer...";
                server.ShutdownServer();
                ret false;
            }
            break;
    }
    return ret;
}
#endif

/*!
 * \fn initLogger(const std::string &currentRunningProgramPath, const std::string &easyloggingConfigFile)
 * \brief Set some config options and initialize the logger facility
 * 
 * \param string configFile. Path of the Easylogging++ configuration file
 * \return void
 */ 
void initLogger(const std::string &currentRunningProgramPath, const std::string &easyloggingConfigFile) 
{
    std::string absConfigFilePath = currentRunningProgramPath + easyloggingConfigFile;
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Configurations confFromFile(absConfigFilePath);
    el::Loggers::reconfigureAllLoggers(confFromFile);
}

/*!
 * \fn getSelfPath()
 * \brief Function to get the directory where the executable program resides
 *
 * \return string
 */
std::string getSelfPath() 
{
    char buff[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
        buff[len] = '\0';
        std::string programDirname = std::string(dirname(const_cast<char*>(buff)));
        return programDirname;
    }
    return std::string(); 
}