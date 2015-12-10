#ifndef __Blackfin__
#include "Blackfin.h"
#include "BlackfinEngine.h"
#include "IrisysTimeZone.h"

namespace Irisys
{
  /*!
  * Converts a numerical serial number obtained from a device into the corresponding alphanumeric
  * serial number as used in Irisys applications and printed on Blackfin devices.
  *
  * \param[in] serialNumber
  *   A numerical serial number obtained from a Blackfin device
  * \param[out] buf
  *   A character array to write the alphanumeric serial number string into
  * \param[in] bufSize
  *   The size of the character array
  *
  * \see Blackfin::GetSerialNumber()
  */
  void ConvertBlackfinSerialNumber(uint32_t serialNumber, char* buf, int bufSize);

}
#endif