
#ifndef __IRISYS_TZ__
#define __IRISYS_TZ__
namespace Irisys
{
    /*!
    * \struct IrisysTimeZone
    * \brief Immutable class representing one of a set of supported time zones
    */
    struct IrisysTimeZone
    {
      /*!
      * The base UTC offset for the time zone, before any daylight savings are take into account
      */
      const std::string m_strBaseUTCOffset;

      /*!
      * Whether or not this timezone has daylight savings rules which could be applied if desired
      */
      const bool m_bDSTSupported;

      /*!
      * The string descriptor of the timezone. This corresponds to a Windows time zone ID key in the registry.
      */
      const std::string m_strDescription;
  
      /*!
      * Creates a new immutable object based on the parameters specified.
      * Note that on a IrisysTimeZone object with a m_strDescription value which is equivalent of a value from the
      * IRISYS_TIMEZONES can be used to set the value in the device. 
      * We do not support custom time zones. For this use the Irisys::Blackfin::LocaleString() field.
      */
      IrisysTimeZone(std::string strBaseUTCOffset, 
                     bool bDSTSupported,
                     std::string strDescription)
                     :m_strBaseUTCOffset(strBaseUTCOffset),
                     m_bDSTSupported(bDSTSupported),
                     m_strDescription(strDescription)
      {
      }

    };

  /*!
    * \defgroup timezones Irisys Timezone Definitions
    * @{
    */

    /*!
    * Total number of Irisys Time Zones currently defined
  */
  const static unsigned int IRISYS_TIMEZONES_SIZE = 90;

  /*!
  * A list of all of the time zones which are supported for IRC3000 devices.
  * The order and size of this array is unspecified (may change). 
  * However, the descriptor strings are guaranteed to remain constant. These should be used
  * if serializing time zone values to a database.
  */
  const static IrisysTimeZone IRISYS_TIMEZONES[] = {
  IrisysTimeZone("00:00:00",false,"UTC"),
  IrisysTimeZone("-12:00:00",false,"Dateline Standard Time"),
  IrisysTimeZone("-11:00:00",false,"Samoa Standard Time"),
  IrisysTimeZone("-10:00:00",false,"Hawaiian Standard Time"),
  IrisysTimeZone("-09:00:00",true,"Alaskan Standard Time"),
  IrisysTimeZone("-08:00:00",true,"Pacific Standard Time"),
  IrisysTimeZone("-08:00:00",true,"Pacific Standard Time (Mexico)"),
  IrisysTimeZone("-07:00:00",false,"US Mountain Standard Time"),
  IrisysTimeZone("-07:00:00",true,"Mountain Standard Time (Mexico)"),
  IrisysTimeZone("-07:00:00",true,"Mountain Standard Time"),
  IrisysTimeZone("-06:00:00",false,"Central America Standard Time"),
  IrisysTimeZone("-06:00:00",true,"Central Standard Time"),
  IrisysTimeZone("-06:00:00",true,"Central Standard Time (Mexico)"),
  IrisysTimeZone("-06:00:00",true,"Central Standard Time (Mexico)"),
  IrisysTimeZone("-06:00:00",false,"Canada Central Standard Time"),
  IrisysTimeZone("-05:00:00",false,"SA Pacific Standard Time"),
  IrisysTimeZone("-05:00:00",true,"Eastern Standard Time"),
  IrisysTimeZone("-05:00:00",false,"US Eastern Standard Time"),
  IrisysTimeZone("-04:30:00",false,"Venezuela Standard Time"),
  IrisysTimeZone("-04:00:00",true,"Paraguay Standard Time"),
  IrisysTimeZone("-04:00:00",true,"Atlantic Standard Time"),
  IrisysTimeZone("-04:00:00",false,"SA Western Standard Time"),
  IrisysTimeZone("-04:00:00",true,"Central Brazilian Standard Time"),
  IrisysTimeZone("-04:00:00",true,"Pacific SA Standard Time"),
  IrisysTimeZone("-03:30:00",true,"Newfoundland Standard Time"),
  IrisysTimeZone("-03:00:00",true,"E. South America Standard Time"),
  IrisysTimeZone("-03:00:00",true,"Argentina Standard Time"),
  IrisysTimeZone("-03:00:00",false,"SA Eastern Standard Time"),
  IrisysTimeZone("-03:00:00",true,"Greenland Standard Time"),
  IrisysTimeZone("-02:00:00",true,"Mid-Atlantic Standard Time"),
  IrisysTimeZone("-01:00:00",true,"Azores Standard Time"),
  IrisysTimeZone("-01:00:00",false,"Cape Verde Standard Time"),
  IrisysTimeZone("00:00:00",true,"Morocco Standard Time"),
  IrisysTimeZone("00:00:00",true,"GMT Standard Time"),
  IrisysTimeZone("01:00:00",true,"W. Europe Standard Time"),
  IrisysTimeZone("01:00:00",true,"Central Europe Standard Time"),
  IrisysTimeZone("01:00:00",true,"Romance Standard Time"),
  IrisysTimeZone("01:00:00",true,"Central European Standard Time"),
  IrisysTimeZone("01:00:00",false,"W. Central Africa Standard Time"),
  IrisysTimeZone("02:00:00",true,"Jordan Standard Time"),
  IrisysTimeZone("02:00:00",true,"GTB Standard Time"),
  IrisysTimeZone("02:00:00",true,"Middle East Standard Time"),
  IrisysTimeZone("02:00:00",true,"E. Europe Standard Time"),
  IrisysTimeZone("02:00:00",true,"Egypt Standard Time"),
  IrisysTimeZone("02:00:00",false,"South Africa Standard Time"),
  IrisysTimeZone("02:00:00",true,"FLE Standard Time"),
  IrisysTimeZone("02:00:00",true,"Israel Standard Time"),
  IrisysTimeZone("02:00:00",true,"Namibia Standard Time"),
  IrisysTimeZone("03:00:00",true,"Arabic Standard Time"),
  IrisysTimeZone("03:00:00",false,"Arab Standard Time"),
  IrisysTimeZone("03:00:00",true,"Russian Standard Time"),
  IrisysTimeZone("03:00:00",false,"E. Africa Standard Time"),
  IrisysTimeZone("03:30:00",true,"Iran Standard Time"),
  IrisysTimeZone("04:00:00",false,"Arabian Standard Time"),
  IrisysTimeZone("04:00:00",true,"Azerbaijan Standard Time"),
  IrisysTimeZone("04:00:00",true,"Caucasus Standard Time"),
  IrisysTimeZone("04:00:00",true,"Mauritius Standard Time"),
  IrisysTimeZone("04:00:00",false,"Georgian Standard Time"),
  IrisysTimeZone("04:00:00",true,"Caucasus Standard Time"),
  IrisysTimeZone("04:30:00",false,"Afghanistan Standard Time"),
  IrisysTimeZone("05:00:00",true,"Ekaterinburg Standard Time"),
  IrisysTimeZone("05:00:00",true,"Pakistan Standard Time"),
  IrisysTimeZone("05:00:00",false,"West Asia Standard Time"),
  IrisysTimeZone("05:30:00",false,"India Standard Time"),
  IrisysTimeZone("05:45:00",false,"Nepal Standard Time"),
  IrisysTimeZone("06:00:00",true,"N. Central Asia Standard Time"),
  IrisysTimeZone("06:00:00",false,"Central Asia Standard Time"),
  IrisysTimeZone("06:00:00",false,"Sri Lanka Standard Time"),
  IrisysTimeZone("06:30:00",false,"Myanmar Standard Time"),
  IrisysTimeZone("07:00:00",false,"SE Asia Standard Time"),
  IrisysTimeZone("07:00:00",true,"North Asia Standard Time"),
  IrisysTimeZone("08:00:00",false,"China Standard Time"),
  IrisysTimeZone("08:00:00",true,"North Asia East Standard Time"),
  IrisysTimeZone("08:00:00",false,"Singapore Standard Time"),
  IrisysTimeZone("08:00:00",true,"W. Australia Standard Time"),
  IrisysTimeZone("08:00:00",false,"Taipei Standard Time"),
  IrisysTimeZone("09:00:00",false,"Tokyo Standard Time"),
  IrisysTimeZone("09:00:00",false,"Korea Standard Time"),
  IrisysTimeZone("09:00:00",true,"Yakutsk Standard Time"),
  IrisysTimeZone("09:30:00",true,"Cen. Australia Standard Time"),
  IrisysTimeZone("09:30:00",false,"AUS Central Standard Time"),
  IrisysTimeZone("10:00:00",false,"E. Australia Standard Time"),
  IrisysTimeZone("10:00:00",true,"AUS Eastern Standard Time"),
  IrisysTimeZone("10:00:00",false,"West Pacific Standard Time"),
  IrisysTimeZone("10:00:00",true,"Tasmania Standard Time"),
  IrisysTimeZone("10:00:00",true,"Vladivostok Standard Time"),
  IrisysTimeZone("11:00:00",false,"Central Pacific Standard Time"),
  IrisysTimeZone("12:00:00",true,"New Zealand Standard Time"),
  IrisysTimeZone("12:00:00",false,"Fiji Standard Time"),
  IrisysTimeZone("13:00:00",false,"Tonga Standard Time") 
  };
}

#endif
