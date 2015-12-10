#ifndef __IBlackfinErrorHandler__
#define __IBlackfinErrorHandler__

namespace Irisys{
 class Blackfin;
 
 class IBlackfinErrorHandler{
 public:
    virtual void BlackfinError(Blackfin *device, const std::string &errorString) = 0;
  };
}


#endif

