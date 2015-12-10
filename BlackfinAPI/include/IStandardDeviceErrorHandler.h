#ifndef __IStandardDeviceHandler__
#define __IStandardDeviceHandler__

#include <string>

namespace Irisys{

  class IStandardDeviceErrorHandler{
  public:
    virtual void ReportError(const std::string &errorMessage) = 0;
  };

}


#endif

