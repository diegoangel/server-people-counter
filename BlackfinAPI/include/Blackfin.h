// -------------------------------------------------------------------------------------------------
// Copyright (c) 2012 Infrared Integrated Systems Ltd
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// -------------------------------------------------------------------------------------------------

/*!
* \brief User header for Irisys Blackfin API
*
* This file contains entry points for using the Irisys Blackfin API to connect to
* IRC3000 series devices for retrieving count data and basic unit maintenance.
*
* \author Infrared Integrated Systems Ltd.
* \version 3.0.0.0
*/

#ifndef __Blackfin__
#define __Blackfin__

#include <string>
#include <vector>
#include <ctime>
#include <stdint.h>

#include "IStandardDeviceErrorHandler.h"
#include "IBlackfinErrorHandler.h"
#include "PlatformDataTypes.h"
#include "IrisysTimeZone.h"

//! max string length for version strings (including a null terminating character)
#define MAX_VERSION_STRING   120
//! max string length for IP related strings (including a null terminating character)
#define MAX_IPADDRESS_STRING 20
//! max string length for SetClientConfigHostname (including a null terminating character)
#define MAX_HOSTNAME_STRING  120
//! max string length for flash property set functions (including a null terminating character)
#define MAX_PROPERTY_STRING  64
//! max string length for device ID string set function (including a null terminating character)
#define MAX_DEVICEID_STRING  160
//! min count log period (seconds) allowed by SetCountLogPeriod function
#define MIN_COUNT_LOG_PERIOD 60
//! max count log period (seconds) allowed by SetCountLogPeriod function
#define MAX_COUNT_LOG_PERIOD 3600
//! min allowed value for SetClientConfigPort
#define MIN_CLIENT_PORT      1
//! max allowed value for SetClientConfigPort
#define MAX_CLIENT_PORT      10000
//! min allowed value for SetClientConfigTimeout
#define MIN_CLIENT_TIMEOUT   1
//! max allowed value for SetClientConfigTimeout (2 days in seconds)
#define MAX_CLIENT_TIMEOUT   172800

class DV_BlackfinLite;
namespace FourthGen { class Device; }

namespace Irisys{

 /**!
  * \class Blackfin
  * \brief A representation of a IRC3000 series counting network.
  *
  * This class represents an Irisys IRC3000 series counting unit (a master device) and optionally a set of node
  * devices. It is used for all communication calls to the device including connection and disconnection.
  */
  class Blackfin : public IStandardDeviceErrorHandler{
  public:

    /*!
    * \struct Count
    * A structure containing a single count log entry downloaded from a device, or the live count
    * values for the count lines when used with Blackfin::GetCurrentCounts().
    */
    struct Count
    {
      Count() : countTime(0) {}

      /*!
      * A vector containing the count values for each count line configured on the device at the
      * time this count log entry was created.
      */
      std::vector<i_uint32> countLines;

      /*! The UTC device time when this count log entry was created */
      time_t countTime;
    };

    /*!
    * \struct DeviceLogEntry
    * \brief A single device status log entry
    *
    * This structure contains a string which describes a condition that the device was in
    * at a specific time stamp.
    */
    struct DeviceLogEntry
    {
      DeviceLogEntry() : errorDescription(""), timestamp(0) {}

      /*! A string describing the status of the device */
      std::string errorDescription;

      /*! The UTC device time when this device log entry was created */
      time_t timestamp;
    };
 
 /*!
    * \struct DeviceStatus
    * A structure containing all of the device status log entries downloaded from a device, which are grouped
    * into one of three categories which are, in order of increasing severity: information, warnings and errors.
    * Information entries provide general messages about the devices operation, such as reset events. Warning
    * messages indicate a potential problem which may need to be addresses whilst error messages indicate a
    * fault with the device or its configuration which must be corrected.
    */
    struct DeviceStatus
    {
      /*! Vector of device status log entries which fall into the information category */
      std::vector<DeviceLogEntry> m_infoList;
	  
      /*! Vector of device status logs entries which fall into the warnings category */
      std::vector<DeviceLogEntry> m_warnList;
	  
      /*! Vector of device status logs entries which fall into the errors category */
      std::vector<DeviceLogEntry> m_errorList;
    };

    Blackfin();
    virtual ~Blackfin();

    
    /**
    * \name Getters
    * @{
    */

    /*!
    * Get the client connection state configured on the device, which must feature an IP board. This setting specifies
    * whether the device will attempt to establish an outgoing connection to the IP address or hostname configured on
    * the device.
    *
    * \param[out] resultValue The client connection enabled state retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetClientConfigEnable(bool &resultValue);

    /*!
    * Get the client connection hostname configured on the device, which must feature an IP board. This setting
    * specifies the hostname the device will attempt to connect to when client connection mode is enabled.
    *
    * \note If both an IP address and hostname are specified for client connection mode the IP address is used
    *
    * \param[out] resultValue The client connection hostname retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetClientConfigHostname(std::string &resultValue);

    /*!
    * Get the client connection IP address configured on the device, which must feature an IP board. This setting
    * specifies the IP address the device will attempt to connect to when client connection mode is enabled.
    *
    * \note If both an IP address and hostname are specified for client connection mode the IP address is used
    *
    * \param[out] resultValue The client connection IP address retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetClientConfigIP(std::string &resultValue);

    /*!
    * Get the client connection port configured on the device, which must feature an IP board. This setting
    * specifies the port the device will attempt to connect to when client connection mode is enabled.
    *
    * \param[out] resultValue The client connection port retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetClientConfigPort(unsigned short &resultValue);

    /*!
    * Get the client connection timeout configured on the device, which must feature an IP board. This setting specifies
    * the timeout, in seconds, between client connection attempts by the device when client connection mode is enabled.
    *
    * \param[out] resultValue The client connection timeout retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetClientConfigTimeout(unsigned int &resultValue);

    /*!
    * Get the count log period currently configured on the device, which specifies the interval, in seconds,
    * between count log entries being written to the devices memory.
    *
    * \param[out] resultValue The configured count log period retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetCountLogPeriod(unsigned int &resultValue);


    /*!
    * Gets the count log entries from the device which fall within the specified time range.
    *
    * \note This call may take longer than the configured packet timeout
    *
    * \param[in] start UTC time of the earliest count log entry to retrieve
    * \param[in] end UTC time of the latest count log entry to retrieve
    * \param[out] resultValue
    *   A vector in which the count log entries retrieved from the device will be stored. Any existing data
    *   in the vector will be erased before new entries are added.
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetCounts(time_t startTime, time_t endTime, std::vector<Count> &resultValue);

    /*!
    * Get the live count line values from the device. These values are not written to the count log and
    * therefore may not be the same as the most recent entry returned by the GetCounts() function.
    *
    * \param[out] resultValue A count object containing the current count values for each line
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetCurrentCount(Count &resultValue);

    /*!
    * Get the latest N count log entries from the device. Upon successful completion the count log entries
    * will be available via the Counts() accessor function.
    *
    * \note This call may take longer than the configured packet timeout
    * \note
    *   The number of count log entries retrieved may be less than N if there are less than N entries in
    *   the device count log
    *
    * \param[in] n Number of count log entries to retrieve
    * \param[out] resultValue
    *   A vector in which the count log entries retrieved from the device will be stored. Any existing data
    *   in the vector will be erased before new entries are added.
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetLastNCounts(unsigned int n, std::vector<Count> &resultValue);

    /*!
    * Get the device ID string from the device.
    *
    * \param[out] resultValue The device ID string retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetDeviceID(std::string &resultValue);

    /*!
    * Get the device name string from the device.
    *
    * \param[out] resultValue The device name string retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetDeviceName(std::string &resultValue);

    /*!
    * Get the site ID string from the device.
    *
    * \param[out] resultValue The site ID string retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetSiteID(std::string &resultValue);

    /*!
    * Get the site name string from the device.
    *
    * \param[out] resultValue The site name string retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetSiteName(std::string &resultValue);

    /*!
    * Get the user string from the device.
    *
    * \apiversion{2.0}
    * \param[out] resultValue The user string retrieved from the device
    * \return True on success, false if the devices firmware is too old or a timeout or other comms error occurred
    */
    bool GetUserString(std::string &resultValue);

    /*!
    * Retrieve the labels for the enabled count registers from the device. Upon successful
    * completion the retrieved labels will be available via the RegisterLabels() accessor function
    *
    * \apiversion{3.0}
    * \dspfirmware{454}
    * \param[out] labels
    *   Vector of labels for each of the enabled registers on the device. These labels will be in
    *   the same order as the count values returned by the \irisyscode{\bfclass::GetCounts()},
    *   \irisyscode{\bfclass::GetLastNCounts()} and \irisyscode{\bfclass::GetCurrentCounts()}
    *   functions, unless the number of enabled register definitions has been changed on the device
    *   since the time at which the count values were recorded by the device.
    * \return True on success, false if the devices firmware is too old or a comms failure occurs
    */
    bool GetRegisterLabels(std::vector<std::string> &labels);

    /*!
    * Get the device status logs since the last boot up or device log reset.
    *
    * \note This call may take longer than the configured packet timeout
    *
    * \param[out] resultValue The device status logs retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetDeviceStatus(DeviceStatus &resultValue);

    /*!
    * Get the locale string from the device.
    *
    * \note This value is not linked to the unit timezone and has no functional impact on the device
    *
    * \param[out] resultValue The locale string retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetLocaleString(std::string &resultValue);

    /*!
    * Get the MAC address of the device, which must feature an IP board.
    *
    * \param[out] resultValue The MAC address retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetMACAddress(std::string &resultValue);

    /*!
    * Get the unique serial number from the device.
    *
    * \param[out] resultValue The unique serial number retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetSerialNumber(uint32_t &resultValue);

    /*!
    * Get the current UTC time of the devices internal clock, which is not affected by the configured timezone.
    *
    * \param[out] resultValue The UTC time retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetUnitTime(time_t &resultValue);

    /*!
    * Get the elapsed time, in seconds, since the device was booted. Upon successful completion the uptime will be
    * available via the UpTime() accessor function.
    *
    * \param[out] upTime The up time retrieved from the device, in seconds
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetUpTime(i_uint64 *upTime);

    /*!
    * Get the DHCP state of the device, which must feature an IP board. This setting determines if the device
    * obtains an IP address from a DHCP server or uses a statically configured address.
    *
    * \param[out] resultValue The DHCP state retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetDHCPEnabled(bool &resultValue);

    /*!
    * Get the statically configured IP address of the device, which must feature an IP board. This setting
    * determines the IP address of a device when it is not using DHCP.
    *
    * \param[out] resultValue The statically configured IP address retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetIPAddress(std::string &resultValue);

    /*!
    * Get the statically configured subnet mask of the device, which must feature an IP board. This setting
    * determines the subnet mask of a device when it is not using DHCP.
    *
    * \param[out] resultValue The statically configured subnet mask retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetSubnetMask(std::string &resultValue);

    /*!
    * Get the statically configured gateway of the device, which must feature an IP board. This setting determines
    * the gateway of a device when it is not using DHCP.
    *
    * \param[out] resultValue The statically configured gateway retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetGateway(std::string &resultValue);

    /*!
    * Get the version of IP firmware installed on the device, which must feature an IP board.
    *
    * \param[out] resultValue The IP firmware version retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetIPFirmwareVersion(std::string &resultValue);

    /*!
    * Get the version of firmware installed on the device.
    *
    * \param[out] resultValue The firmware version retrieved from the device
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool GetMonitorFirmwareVersion(std::string &resultValue);

    /*!
    * Get the primary DNS server for the device, which must feature an IP board.
    *
    * \param[out] resultValue The primary DNS server retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetDNS1(std::string &resultValue);

    /*!
    * Get the secondary DNS server for the device, which must feature an IP board.
    *
    * \param[out] resultValue The secondary DNS server retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetDNS2(std::string &resultValue);

    /*!
    * Get the tertiary DNS server for the device, which must feature an IP board.
    *
    * \param[out] resultValue The tertiary DNS server retrieved from the device
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool GetDNS3(std::string &resultValue);

    /*!
    * Get the timezone configured on the device. Note that this has no functional impact on the device itself, which
    * will always use UTC time.
    *
    * \note The timezone can be configured using the People Counter Setup Tool
    *
    * \param[out] tz_id The index of the Irisys timezone configured on the device in the IRISYS_TIMEZONES[] array
    * \param[out] useDST The DST enabled state configured on the device
    * \return True on success, false if the devices firmware is too old or a timeout or other comms error occurred
    */
    bool GetUnitTimeZone(int &tz_id, bool &useDST);

    // ---------------------------------------------------------------------------------------------

    /*!
    * Generates a checksum of the current configuration settings of all devices on the CAN network of
    * the connected device, which can be used to detect whether any configuration changes have taken
    * place on the network between two subsequent calls to this function. Upon successful completion
    * the network checksum will be available via the NetworkChecksum() accessor function.
    *
    * \note
    *   This checksum may also be affected by firmware upgrades to any device on the CAN network
    *
    * \apiversion{3.0}
    * \param[out] checksum The computed checksum of the settings for all devices on the network
    * \return True on success, false if a timeout or other comms error occurred 
    */
    bool GetNetworkChecksum ( std::string &checksum );

    /**
    * @}
    */

    /*!
    * Empty the count log stored on the device. This action cannot be undone and will not affect the live
    * count line values.
    *
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool ResetCountLogs();

    /*!
    * Reset the live count line values on the device. This action cannot be undone and does not affect existing
    * count log entries stored on the device.
    *
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool ResetCurrentCount();

    /*!
    * Clear all entries in the device status logs. This action cannot be undone.
    *
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool ResetDeviceStatus();

    /**
    * \name Setters
    * @{
    */

    /*!
    * Enable or disable client connection mode on the device, which must feature an IP board. If this setting is enabled
    * the device will regularly attempt to establish an outgoing client connection to the IP address or hostname specified
    * in the client connection settings. Please refer to the notes on \ref ipConfigSet before using this function with a
    * TCP/IP connection.
    *
    * \param[in] clientConfigEnable
    *   Client connection enabled state to set on the device
    *
    * \see SetClientConfigIP()
    * \see SetClientConfigHostname()
    * \see SetClientConfigPort()
    * \see SetClientConfigTimeout()
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetClientConfigEnable(bool clientConfigEnable);

    /*!
    * Set the client connection hostname on the device, which must feature an IP board. This setting specifies the
    * hostname the device will attempt to connect to when client connection mode is enabled. Please refer to the notes
    * on \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \note If a client connection IP address other than 0.0.0.0 is specified this setting has no effect
    *
    * \param[in] clientConfigHostname
    *   The client connection hostname to set on the device, whose length must not exceed the value specified
    *   by the MAX_HOSTNAME_STRING constant
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetClientConfigHostname(const std::string &clientConfigHostname);

    /*!
    * Set the client connection IP address on the device, which must feature an IP board. This setting specifies the IP
    * address the device will attempt to connect to when client connection mode is enabled. Please refer to the notes on
    * \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \note Set this value to 0.0.0.0 if you want to specify a hostname to connect to instead
    *
    * \param[in] clientConfigIP
    *   The client connection IP address to set on the device, in dotted notation, whose length must not exceed
    *   the value specified by the MAX_IPADDRESS_STRING constant
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetClientConfigIP(const std::string &clientConfigIP);

    /*!
    * Set the client connection port number on the device, which must feature an IP board. This setting specifies the
    * port the device will attempt to connect to when client connection mode is enabled. Please refer to the notes on
    * \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \param[in] clientConfigPort
    *   The client connection port number to set on the device, which must be a value between the MIN_CLIENT_PORT
    *   and MAX_CLIENT_PORT constants.
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetClientConfigPort(unsigned short clientConfigPort);

    /*!
    * Set the client connection timeout on the device, which must feature an IP board. This setting specifies the timeout,
    * in seconds, between client connection attempts by the device when client connection mode is enabled. Please refer to
    * the notes on \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \param[in] clientConfigTimeout
    *   The client connection timeout to set on the device, in seconds, which must be a value between the MIN_CLIENT_TIMEOUT
    *   and MAX_CLIENT_TIMEOUT constants.
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetClientConfigTimeout(unsigned int clientConfigTimeout);

    /*!
    * Set the count log period on the device which specifies the interval, in seconds, between count log entries being
    * written to the devices memory.
    *
    * \param[in] nCountLogPeriod
    *   The count log period to set on the device, in seconds, which must be a value between the MIN_COUNT_LOG_PERIOD
    *   and MAX_COUNT_LOG_PERIOD constants
    *
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetCountLogPeriod(unsigned int nCountLogPeriod);

    /*!
    * Set the device ID string on the device to the specified value
    * 
    * \param[in] deviceID
    *   Device ID string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetDeviceID(const std::string &deviceID);

    /*!
    * Set the device name string on the device to the specified value
    * 
    * \param[in] deviceName
    *   Device name string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetDeviceName(const std::string &deviceName);

    /*!
    * Set the site ID string on the device to the specified value
    * 
    * \param[in] siteID
    *   Site ID string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetSiteID(const std::string &siteID);

    /*!
    * Set the site name string on the device to the specified value
    * 
    * \param[in] siteName
    *   Site name string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetSiteName(const std::string &siteName);

    /*!
    * Set the locale string on the device to the specified value
    * 
    * \param[in] localeString
    *   Locale string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetLocaleString(const std::string &localeString);

    /*!
    * Set the user string on the device to the specified value
    * 
    * \param[in] userString
    *   User string to set on the device
    * 
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetUserString(const std::string userString);

    /*!
    * Sets the internal clock on the device to the specified time, which should be in UTC
    *
    * \param[in] unitTime
    *   The UTC time to set on the device
    *
    * \return Returns true on success, false on failure - timeout or comms error
    */
    bool SetUnitTime(time_t timestamp);

    /*!
    * Set the primary DNS server for the device to use when resolving hostnames, which must feature an IP board. Please
    * refer to the notes on \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \param[in] dns1
    *   Primary DNS server IP address, in dotted notation, to set on this device, whose length must not exceed the#
    *   value specified by the MAX_IPADDRESS_STRING constant
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetDNS1(const std::string &dns1);

    /*!
    * Set the secondary DNS server for the device to use when resolving hostnames, which must feature an IP board. Please
    * refer to the notes on \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \param[in] dns2
    *   Secondary DNS server IP address, in dotted notation, to set on this device, whose length must not exceed the#
    *   value specified by the MAX_IPADDRESS_STRING constant
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetDNS2(const std::string &dns2);

    /*!
    * Set the tertiary DNS server for the device to use when resolving hostnames, which must feature an IP board. Please
    * refer to the notes on \ref ipConfigSet before using this function with a TCP/IP connection.
    *
    * \param[in] dns3
    *   Tertiary DNS server IP address, in dotted notation, to set on this device, whose length must not exceed the#
    *   value specified by the MAX_IPADDRESS_STRING constant
    *
    * \return True on success, false if the device does not have an IP board or a timeout or other comms error occurred
    */
    bool SetDNS3(const std::string &dns3);

    /*!
    * Set the time zone and DST state on the device. Note that this has no functional impact on the device itself, which
    * will always use UTC time internally.
    *
    * \note The time zone can also be configured using the People Counter Setup Tool
    *
    * \param[in] itz
    *   Index of the %Irisys time zone within the IRISYS_TIMEZONES[] array to set on the device 
    * \param[in] bEnableDST
    *   DST state to set on the device, specifies whether a DST offset should be applied where appropriate
    *
    * \return True on success, false if the devices firmware is too old or a timeout or other comms error occurred
    */
    bool SetUnitTimeZone(const int tz_id, const bool bEnableDST);

    /*!
    * Set the packet timeout value for the API to use when communicating with the connected device, which specifies
    * how long it will wait for a response before returning a failure. Longer values can help reduce failures on
    * high latency connections but will also cause Get* and Set* function calls to block for longer whilst they
    * wait for a response when the connection has been lost.
    *
    * \param[in] timeoutMS
    *   Timeout value, in milliseconds, to use during communications with the device
    *
    * \see PacketTimeout()
    *
    * \return
    *   True on success, false on failure
    */
    bool SetPacketTimeout(unsigned int timeoutMS);

    /**
    * @}
    */

  private:
    bool GetCountsByIndex(unsigned int start, unsigned int end, std::vector<Count> &resultValue);
    bool GetEnabledCounters ( std::vector<unsigned char>& );

    virtual void ReportError(const std::string &errorMessage);

    friend class BlackfinEngine;
    //Used internally
    void Hookup(DV_BlackfinLite *hook, IBlackfinErrorHandler *handler);
    void RemoveHookup();
    DV_BlackfinLite *GetHookup()const;

    bool ValidateHostname(const std::string &hostname) const;

    FourthGen::Device* GetTargetDevice();

    DV_BlackfinLite *m_blackfinLite;
    IBlackfinErrorHandler *m_errorHandler;
  };

}

#endif
