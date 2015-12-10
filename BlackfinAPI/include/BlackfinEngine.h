#ifndef __Blackfin_Engine__
#define __Blackfin_Engine__

#include <vector>

#include "IBlackfinErrorHandler.h"

#ifdef linux
typedef int SOCKET;
#endif

#ifdef WIN32
typedef UINT_PTR SOCKET;
#endif

namespace Irisys{

  
  // Forward declarations
  class Blackfin;
  namespace Threading
  {
    class Locking;
  }
  
  
  /*!
  * \class BlackfinEngine
  * \brief This class connects Blackfin objects to SOCKET handles.
  *
  *
  */
  class BlackfinEngine{
  public:

    /*!
    * Gets the version number of the executing API assembly
    * \return A string value representing a version number
    */
    static std::string GetAPIVersion();

    /*!
    * Constructs a new BlackfinEngine object
    */
    BlackfinEngine();
    ~BlackfinEngine();

    /*!
    * Intializes the BlackfinEngine object.
    * This method is required to be called before AddBlackfinEndPoint() is called
    */
    void StartEngine();
    
    /*!
    * Releases allocated resources and closes.
    * This should not be called until all Blackfin objects have been destroyed.
    * No further method calls should be invoked.
    */
    void ShutdownEngine();

    /*!
    * Connects the specified blackfin object to the specified connected SOCKET handler.
    * Errors in the connection will be reported to the specified error handler.
    * Do not attempt to connect the same Blackfin object more than once.
    *
    * \param[in] endPoint The Blackfin object to connect. 
    * \param[in] socketHandle An established socket connection descriptor
    * \param[in] errorHandler An object to callback in case of errors
    */
    bool AddBlackfinEndPoint(Blackfin *endPoint, SOCKET socketHandle, IBlackfinErrorHandler *errorHandler);
    
    /*!
    * Removes the Blackfin object from the engine and disconnects it. 
    * The Blackfin object will not be deleted, this is the responsibility of the caller.
    * The SOCKET handle will be closed by this method.
    *
    * \param[in] endPoint The Blackfin object to disconnect
    */
    void RemoveBlackfinEndPoint(Blackfin *endPoint);

  private:
    std::vector<Blackfin *> m_blackfinEndPoints;
    Irisys::Threading::Locking* m_pDataLock;

  };

}


#endif
