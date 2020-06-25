#ifndef _MSGID_H_
#define _MSGID_H_
#pragma once
/* QMI Service Enumeration */
enum eQMIService
{
   eQMI_SVC_ENUM_BEGIN = -1,
   eQMI_SVC_CONTROL,          // 000 Control service
   eQMI_SVC_RANGE0_START,
   eQMI_SVC_WDS =             // 001 Wireless data service
        eQMI_SVC_RANGE0_START,
   eQMI_SVC_DMS,              // 002 Device management service
   eQMI_SVC_NAS,              // 003 Network access service
   eQMI_SVC_QOS,              // 004 Quality of service, err, service
   eQMI_SVC_WMS,              // 005 Wireless messaging service
   eQMI_SVC_PDS,              // 006 Position determination service
   eQMI_SVC_AUTH,             // 007 Authentication service
   eQMI_SVC_RANGE0_STOP =
        eQMI_SVC_AUTH,
   eQMI_SVC_VOICE = 9,
   eQMI_SVC_CAT = 0x0A,        // 0x0A Card application toolkit service
   eQMI_SVC_UIM = 0x0B,
   eQMI_SVC_PBM = 0x0C,       // Phone Book Manager service
   eQMI_SVC_LOC = 0x10,       // LOC service
   eQMI_SVC_SAR = 17,
   eQMI_SVC_IMS = 0x12,       // 012 IMS Service
   eQMI_SVC_TMD = 0x18,       // 0x18 TMD Service
   eQMI_SVC_IMSA = 0x21,      // 0x21 IMSA service

   eQMI_SVC_RANGE1_START   = 224,
   eQMI_SVC_RMS,              // 225 Remote management service
   eQMI_SVC_OMA,              // 226 Open mobile alliance dev mgmt service
   /* MC77xx, MC73xx and EM74xx use this service for OMADM */
   eQMI_SVC_SWIOMA_MDM9600 = 240,     // 240 SWI Open mobile alliance dev mgmt service
   eQMI_SVC_AUDIO,                    // 241 Audio service
   eQMI_SVC_SWI_M2M_CMD    = 243,     // 0xF3 SWI M2M general commands swiavms
   eQMI_SVC_SWIOMA_MDM6600 = 244,     //OAM Service for MC/SL9090
   eQMI_SVC_SWIAUDIO       = 245,     // 245 Swi Audio service
   eQMI_SVC_SWI_LOC        = 246,     // 0xf6
   eQMI_SVC_RANGE1_STOP =
        eQMI_SVC_SWI_LOC,
   /* Services below are internal to the SDK. Hence, the corresponding
    * QMI requests are processed locally by the SDK and therefore are not
    * sent to the device.
    */
   eQMI_SVC_DCS,                      //0xF8
   eQMI_SVC_FMS,                      //0xF9
   
   /* 0xFD SWI M2M general commands swiavms2 */
   eQMI_SVC_SWI_M2M_CMD_2  = 253,
   eQMI_SVC_SWI_DMS        = 254,      //0xFE   
   eQMI_SVC_ENUM_END
};

/* QMI Control Service Type Message ID Enumeration */
enum eQMIMessageCTL
{
   eQMI_CTL_ENUM_BEGIN = -1,
   eQMI_CTL_SET_INSTANCE_ID = 32,   // 32 Set the unique link instance ID
   eQMI_CTL_GET_VERSION_INFO,       // 33 Get supported service version info
   eQMI_CTL_GET_CLIENT_ID,          // 34 Get a unique client ID
   eQMI_CTL_RELEASE_CLIENT_ID,      // 35 Release the unique client ID
   eQMI_CTL_REVOKE_CLIENT_ID_IND,   // 36 Indication of client ID revocation
   eQMI_CTL_INVALID_CLIENT_ID,      // 37 Indication of invalid client ID
   eQMI_CTL_SET_DATA_FORMAT,        // 38 Set host driver data format
   eQMI_CTL_SYNC,                   // 39 Synchronize client/server
   eQMI_CTL_SYNC_IND = 39,          // 39 Synchronize indication
   eQMI_CTL_SET_EVENT,              // 40 Set event report conditions
   eQMI_CTL_EVENT_IND = 40,         // 40 Event report indication
   eQMI_CTL_SET_POWER_SAVE_CFG,     // 41 Set power save config
   eQMI_CTL_SET_POWER_SAVE_MODE,    // 42 Set power save mode
   eQMI_CTL_GET_POWER_SAVE_MODE,    // 43 Get power save mode
   eQMI_CTL_ENUM_END
};

/* QMI WDS Service Type Message ID Enumeration */
enum eQMIMessageWDS
{
   eQMI_WDS_ENUM_BEGIN = -1,

   eQMI_WDS_RESET,                // 00 Reset WDS service state variables
   eQMI_WDS_SET_EVENT,            // 01 Set connection state report conditions
   eQMI_WDS_EVENT_IND = 1,        // 01 Connection state report indication
   eQMI_WDS_ABORT,                // 02 Abort previously issued WDS command

   eQMI_WDS_START_NET = 32,       // 32 Start WDS network interface
   eQMI_WDS_STOP_NET,             // 33 Stop WDS network interface
   eQMI_WDS_GET_PKT_STATUS,       // 34 Get packet data connection status
   eQMI_WDS_PKT_STATUS_IND = 34,  // 34 Packet data connection status indication
   eQMI_WDS_GET_RATES,            // 35 Get current bit rates of the connection
   eQMI_WDS_GET_STATISTICS,       // 36 Get the packet data transfer statistics
   eQMI_WDS_G0_DORMANT,           // 37 Go dormant
   eQMI_WDS_G0_ACTIVE,            // 38 Go active
   eQMI_WDS_CREATE_PROFILE,       // 39 Create profile with specified settings
   eQMI_WDS_MODIFY_PROFILE,       // 40 Modify profile with specified settings
   eQMI_WDS_DELETE_PROFILE,       // 41 Delete the specified profile
   eQMI_WDS_GET_PROFILE_LIST,     // 42 Get all profiles
   eQMI_WDS_GET_PROFILE,          // 43 Get the specified profile
   eQMI_WDS_GET_DEFAULTS,         // 44 Get the default data session settings
   eQMI_WDS_GET_SETTINGS,         // 45 Get the runtime data session settings
   eQMI_WDS_SET_MIP,              // 46 Get the mobile IP setting
   eQMI_WDS_GET_MIP,              // 47 Set the mobile IP setting
   eQMI_WDS_GET_DORMANCY,         // 48 Get the dormancy status

   eQMI_WDS_GET_AUTOCONNECT = 52, // 52 Get the NDIS autoconnect setting
   eQMI_WDS_GET_DURATION,         // 53 Get the duration of data session
   eQMI_WDS_GET_MODEM_STATUS,     // 54 Get the modem status
   eQMI_WDS_MODEM_IND = 54,       // 54 Modem status indication
   eQMI_WDS_GET_DATA_BEARER,      // 55 Get the data bearer type
   eQMI_WDS_GET_DUN_CALL_INFO,    // 56 Get the modem info
   eQMI_WDS_GET_DUN_CALL_INFO_IND = 56,  // 56 Modem info indication

   eQMI_WDS_GET_ACTIVE_MIP = 60,  // 60 Get the active mobile IP profile
   eQMI_WDS_SET_ACTIVE_MIP,       // 61 Set the active mobile IP profile
   eQMI_WDS_GET_MIP_PROFILE,      // 62 Get mobile IP profile settings
   eQMI_WDS_SET_MIP_PROFILE,      // 63 Set mobile IP profile settings
   eQMI_WDS_GET_MIP_PARAMS,       // 64 Get mobile IP parameters
   eQMI_WDS_SET_MIP_PARAMS,       // 65 Set mobile IP parameters
   eQMI_WDS_GET_LAST_MIP_STATUS,  // 66 Get last mobile IP status
   eQMI_WDS_GET_AAA_AUTH_STATUS,  // 67 Get AN-AAA authentication status
   eQMI_WDS_GET_CUR_DATA_BEARER,  // 68 Get current data bearer
   eQMI_WDS_GET_CALL_LIST,        // 69 Get the call history list
   eQMI_WDS_GET_CALL_ENTRY,       // 70 Get an entry from the call history list
   eQMI_WDS_CLEAR_CALL_LIST,      // 71 Clear the call history list
   eQMI_WDS_GET_CALL_LIST_MAX,    // 72 Get maximum size of call history list
   eQMI_WDS_GET_DEFAULT_PROFILE_NUM, // 73(0x49) Get default profile number
   eQMI_WDS_SET_DEFAULT_PROFILE_NUM, // 74(0x50) Set default profile number

   eQMI_WDS_SET_IP_FAMILY = 77,   // 77 Set the client IP family preference

   eQMI_WDS_SET_AUTOCONNECT = 81, // 81 Set the NDIS autoconnect setting
   eQMI_WDS_GET_DNS,              // 82 Get the DNS setting
   eQMI_WDS_SET_DNS,              // 83 Set the DNS setting
   eQMI_WDS_GET_CURR_DATA_SYS_STAT = 107, // 107 Get Current Data System Status
   eQMI_WDS_GET_DATA_BEARER_TECHNOLOGY_EXT = 145, // 145 Get Data Bearer Technology Ext
   eQMI_WDS_SET_QMUX_ID                    = 162,// 0x00A2,
   eQMI_WDS_RESET_PKT_STATISTICS  = 0x0086, // 134 Reset Packet Statistics
   eQMI_WDS_SWI_CREATE_PROFILE = 0x5558,    // Create profile 3GPP2 Profile
   eQMI_WDS_SWI_GET_3GPP_CFG_ITEM = 0x5559, // Get 3GPP Config Item
   eQMI_WDS_SWI_SET_3GPP_CFG_ITEM = 0x555A, // Set 3GPP Config Item
   eQMI_WDS_SWI_PDP_RUNTIME_SETTINGS = 0x555B, // Get PDP Context
   eQMI_WDS_SWI_PROFILE_CHANGE = 0x5567, // Config and receive notifications for profile updates
   eQMI_WDS_SWI_PROFILE_CHANGE_IND = 0x5567,
   eQMI_WDS_SWI_RM_TRANSFER_STATISTICS = 0x5568,//Set RM Trasnsfter statistics
   eQMI_WDS_SWI_GET_DATA_LOOPBACK  = 0x5569,
   eQMI_WDS_SWI_SET_DATA_LOOPBACK  = 0x556A,
   eQMI_WDS_SWI_GET_CURRENT_CHANNEL_RATE = 0x556B,//Queries the current bit rate of the packet data connection.
   eQMI_WDS_RM_TRANSFER_STATISTICS_IND = eQMI_WDS_SWI_RM_TRANSFER_STATISTICS, // Get Current RM Transfer Statistics
   eQMI_WDS_SWI_GET_DHCPV4_CLIENT_CONFIG = 0x556F,
   eQMI_WDS_SWI_SET_DHCPV4_CLIENT_CONFIG = 0x5570,
   eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE = 0x5571,
   eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND = 0x5571,
   eQMI_WDS_ENUM_END
};

/* QMI WDS Internal Service Type Message ID Enumeration */
enum eQMIMessageWDSInternal
{
   eQMI_WDS_INT_ENUM_BEGIN = 0x100,
   eQMI_WDS_INT_SET_SESSIONID = eQMI_WDS_INT_ENUM_BEGIN, // Internally map sessn ID to instance
   eQMI_WDS_INT_GET_INSTANCEID, // Get instance ID from session ID
   eQMI_WDS_INT_GET_SESSIONID, // Get session ID
   eQMI_WDS_INT_ENUM_END
};

/* QMI DMS Service Type Message ID Enumeration */
enum eQMIMessageDMS
{
   eQMI_DMS_ENUM_BEGIN = -1,

   eQMI_DMS_RESET,               // 00 Reset DMS service state variables
   eQMI_DMS_SET_EVENT,           // 01 Set connection state report conditions
   eQMI_DMS_EVENT_IND = 1,       // 01 Connection state report indication
   eQMI_DMS_INDICATION_REGISTER = 3, //03 Register Indication
   eQMI_DMS_GET_CAPS = 32,       // 32 Get the device capabilities
   eQMI_DMS_GET_MANUFACTURER,    // 33 Get the device manfacturer
   eQMI_DMS_GET_MODEL_ID,        // 34 Get the device model ID
   eQMI_DMS_GET_REV_ID,          // 35 Get the device revision ID
   /* Change eQMI_DMS_GET_NUMBER to eQMI_DMS_GET_MSISDN */
   //eQMI_DMS_GET_NUMBER,          // 36 Get the assigned voice number
   eQMI_DMS_GET_MSISDN,          // 36 Get the assigned voice number
   eQMI_DMS_GET_IDS,             // 37 Get the ESN/IMEI/MEID
   eQMI_DMS_GET_POWER_STATE,     // 38 Get the get power state
   eQMI_DMS_UIM_SET_PIN_PROT,    // 39 UIM - Set PIN protection
   eQMI_DMS_UIM_PIN_VERIFY,      // 40 UIM - Verify PIN
   eQMI_DMS_UIM_PIN_UNBLOCK,     // 41 UIM - Unblock PIN
   eQMI_DMS_UIM_PIN_CHANGE,      // 42 UIM - Change PIN
   eQMI_DMS_UIM_GET_PIN_STATUS,  // 43 UIM - Get PIN status
   /* Change eQMI_DMS_MSM_ID to eQMI_DMS_GET_HARDWARE_REV */
   //eQMI_DMS_GET_MSM_ID = 44,     // 44 Get MSM ID
   eQMI_DMS_GET_HARDWARE_REV = 44, // 44 Get MSM ID
   eQMI_DMS_GET_OPERATING_MODE,  // 45 Get the operating mode
   eQMI_DMS_SET_OPERATING_MODE,  // 46 Set the operating mode
   eQMI_DMS_GET_TIME,            // 47 Get timestamp from the device
   eQMI_DMS_GET_PRL_VERSION,     // 48 Get the PRL version
   eQMI_DMS_GET_ACTIVATED_STATE, // 49 Get the activation state
   eQMI_DMS_ACTIVATE_AUTOMATIC,  // 50 Perform an automatic activation
   eQMI_DMS_ACTIVATE_MANUAL,     // 51 Perform a manual activation
   eQMI_DMS_GET_USER_LOCK_STATE, // 52 Get the lock state
   eQMI_DMS_SET_USER_LOCK_STATE, // 53 Set the lock state
   eQMI_DMS_SET_USER_LOCK_CODE,  // 54 Set the lock PIN
   eQMI_DMS_READ_USER_DATA,      // 55 Read user data
   eQMI_DMS_WRITE_USER_DATA,     // 56 Write user data
   eQMI_DMS_READ_ERI_FILE,       // 57 Read the enhanced roaming indicator file
   eQMI_DMS_FACTORY_DEFAULTS,    // 58 Reset to factory defaults
   eQMI_DMS_VALIDATE_SPC,        // 59 Validate service programming code
   eQMI_DMS_UIM_GET_ICCID,       // 60 Get UIM ICCID
   eQMI_DMS_GET_FIRWARE_ID,      // 61 Get firmware ID
   eQMI_DMS_SET_FIRMWARE_ID,     // 62 Set firmware ID
   eQMI_DMS_GET_HOST_LOCK_ID,    // 63 Get host lock ID
   eQMI_DMS_UIM_GET_CK_STATUS,   // 64 UIM - Get control key status
   eQMI_DMS_UIM_SET_CK_PROT,     // 65 UIM - Set control key protection
   eQMI_DMS_UIM_UNBLOCK_CK,      // 66 UIM - Unblock facility control key
   eQMI_DMS_GET_IMSI,            // 67 Get the IMSI
   eQMI_DMS_UIM_GET_STATE,       // 68 UIM - Get the UIM state
   eQMI_DMS_GET_BAND_CAPS,       // 69 Get the device band capabilities
   eQMI_DMS_GET_FACTORY_ID,      // 70 Get the device factory ID
   eQMI_DMS_GET_FIRMWARE_PREF,   // 71 Get firmware preference
   eQMI_DMS_SET_FIRMWARE_PREF,   // 72 Set firmware preference
   eQMI_DMS_LIST_FIRMWARE,       // 73 List all stored firmware
   eQMI_DMS_DELETE_FIRMWARE,     // 74 Delete specified stored firmware
   eQMI_DMS_SET_TIME,            // 75 Set device time
   eQMI_DMS_GET_FIRMWARE_INFO,   // 76 Get stored firmware info
   eQMI_DMS_GET_ALT_NET_CFG,     // 77 Get alternate network config
   eQMI_DMS_SET_ALT_NET_CFG,     // 78 Set alternate network config
   eQMI_DMS_GET_IMG_DLOAD_MODE,  // 79 Get next image download mode
   eQMI_DMS_SET_IMG_DLOAD_MODE,  // 80 Set next image download mode
   eQMI_DMS_GET_CURRENT_PRL_INFO  = 83,// 83 Get Current PRL Info
   eQMI_DMS_PSM_GET_CFG_PARAMS    = 96,// 96 Get Power Save Mode COnfig Params
   eQMI_DMS_PSM_SET_CFG_PARAMS    = 102,//102 Set Power Save Mode Config Params
   eQMI_DMS_PSM_CFG_PARAMS_CHANGE_IND = 103, //103 PSM config parametrs change ind
   eQMI_DMS_GET_CWE_SPKGS_INFO    = 0x5556, // CWE Sierra Package String info
   eQMI_DMS_SWI_SET_EVENT_REPORT  = 0x5557, // DMS SWI Set Event Report
   eQMI_DMS_SWI_EVENT_IND         = 0x5557, // DMS SWI Event Indication
   eQMI_DMS_SWI_GET_USB_COMP      = 0x555B, // SWI Get USB Composition
   eQMI_DMS_SWI_SET_USB_COMP      = 0x555C, // SWI Set USB Composition
   eQMI_DMS_GET_FSN               = 0x5567, // Get Factory Sequence Number
   eQMI_DMS_SWI_GET_HOST_DEV_INFO = 0x556A, // Get Host Device info configure on the Modem
   eQMI_DMS_SWI_SET_HOST_DEV_INFO = 0x556B, // Set Host Device info configured on the Modem
   eQMI_DMS_SWI_GET_OS_INFO = 0x556C, // Get Host Device info configure on the Modem
   eQMI_DMS_SWI_SET_OS_INFO = 0x556D, // Set Host Device info configured on the Modem
   eQMI_DMS_SWI_GET_PC_INFO = 0x556E, // Get device power control status information
   eQMI_DMS_SWI_GET_CUST_FEATURES = 0x6557, // Query custom feature settings
   eQMI_DMS_SWI_SET_CUST_FEATURES = 0x6558, // Change custom feature settings
   eQMI_DMS_SWI_GET_FIRMWARE_CURR = 0x5563, // Get firmware currently running
   eQMI_DMS_SWI_GET_FW_UPDATE_STAT = 0x5564, // Get firmware update status
   eQMI_DMS_SWI_GET_CRASH_INFO     = 0x5565, // SWI Get Crash Information
   eQMI_DMS_SWI_GET_CRASH_ACTION   = 0x5568, // SWI Get Crash Action
   eQMI_DMS_SWI_SET_CRASH_ACTION   = 0x5569, // SWI Set Crash Action
   eQMI_DMS_SWI_GET_SERIAL_NO_EXT  = 0x5575, // SWI Get Device Serial Number extended
   eQMI_DMS_SWI_GET_DYING_GASP_CFG = 0x557B, // SWI Get current configuration settings for the Dying Gasp feature
   eQMI_DMS_SWI_SET_DYING_GASP_CFG = 0x557C, // SWI Set current configuration settings for the Dying Gasp feature
   eQMI_DMS_SWI_GET_DYING_GASP_STAT = 0x557D, // SWI Get Last Time Stamp and Status for the Dying Gasp feature
   eQMI_DMS_SWI_CLR_DYING_GASP_STAT = 0x557E, // SWI Clear Time Stamp and Status for the Dying Gasp feature
   eQMI_DMS_SWI_INDICATION_REGISTER = 0x557F, // Sets the registration state for different QMI_DMS SWI indications
   eQMI_DMS_SWI_GET_RESET_INFO      = 0x5580, // Queries the reason for the most recent device reset or powerdown
   eQMI_DMS_SWI_GET_RESET_INFO_IND  = 0x5580, // Indicates the reason for the most recent device reset or powerdown
   eQMI_DMS_SWI_UIM_SELECT          = 0x655a, // SLQSSwiDmsUimSelect API QMI message
   eQMI_DMS_SWI_GET_UIM_SELECTION   = 0x6569, // SLQSDmsSwiGetUimSelection API QMI message
   eQMI_DMS_ENUM_END
};

/* QMI NAS Service Type Message ID Enumeration */
enum eQMIMessageNAS
{
   eQMI_NAS_ENUM_BEGIN = -1,

   eQMI_NAS_RESET,               // 00 Reset NAS service state variables
   eQMI_NAS_ABORT,               // 01 Abort previously issued NAS command
   eQMI_NAS_SET_EVENT,           // 02 Set NAS state report conditions
   eQMI_NAS_EVENT_IND = 2,       // 02 Connection state report indication
   eQMI_NAS_SET_REG_EVENT,       // 03 Set NAS registration report conditions

   eQMI_NAS_GET_RSSI = 32,       // 32 Get the signal strength
   eQMI_NAS_SCAN_NETS,           // 33 Scan for visible network
   eQMI_NAS_REGISTER_NET,        // 34 Initiate a network registration
   eQMI_NAS_ATTACH_DETACH,       // 35 Initiate an attach or detach action
   eQMI_NAS_GET_SS_INFO,         // 36 Get info about current serving system
   eQMI_NAS_SS_INFO_IND = 36,    // 36 Current serving system info indication
   eQMI_NAS_GET_HOME_INFO,       // 37 Get info about home network
   eQMI_NAS_GET_NET_PREF_LIST,   // 38 Get the list of preferred networks
   eQMI_NAS_SET_NET_PREF_LIST,   // 39 Set the list of preferred networks
   eQMI_NAS_GET_NET_BAN_LIST,    // 40 Get the list of forbidden networks
   eQMI_NAS_SET_NET_BAN_LIST,    // 41 Set the list of forbidden networks
   eQMI_NAS_SET_TECH_PREF,       // 42 Set the technology preference
   eQMI_NAS_GET_TECH_PREF,       // 43 Get the technology preference
   eQMI_NAS_GET_ACCOLC,          // 44 Get the Access Overload Class
   eQMI_NAS_SET_ACCOLC,          // 45 Set the Access Overload Class
   eQMI_NAS_GET_SYSPREF,         // 46 Get the CDMA system preference
   eQMI_NAS_GET_NET_PARAMS,      // 47 Get various network parameters
   eQMI_NAS_SET_NET_PARAMS,      // 48 Set various network parameters
   eQMI_NAS_GET_RF_INFO,         // 49 Get the SS radio/band channel info
   eQMI_NAS_GET_AAA_AUTH_STATUS, // 50 Get AN-AAA authentication status
   eQMI_NAS_SET_SYS_SELECT_PREF, // 51 Set system selection preference
   eQMI_NAS_GET_SYS_SELECT_PREF, // 52 Get system selection preference
   eQMI_NAS_SYS_SELECT_IND = 52, // 52 System selection pref indication

   eQMI_NAS_SET_DDTM_PREF = 55,  // 55 Set DDTM preference
   eQMI_NAS_GET_DDTM_PREF,       // 56 Get DDTM preference
   eQMI_NAS_DDTM_IND = 56,       // 56 DDTM preference indication
   eQMI_NAS_GET_OPERATOR_NAME = 57, // 57 Get Operator Name Data
   eQMI_NAS_GET_PLMN_MODE = 59,  // 59 Get PLMN mode bit from CSP
   eQMI_NAS_PLMN_MODE_IND,       // 60 CSP PLMN mode bit indication
   eQMI_NAS_GET_3GPP2_SUBSCRIPTION = 62, // Get 3GPP2 subscription info
   eQMI_NAS_GET_CELL_LOCATION_INFO = 67, // 67 Get Cell Location Info
   eQMI_NAS_GET_PLMN_NAME      = 68, // 68 Get operator name for specified network
   eQMI_NAS_NETWORK_TIME_IND   = 76, // 76 Network Time Indication.
   eQMI_NAS_GET_SYS_INFO       = 77, // 77 Get System Information.
   eQMI_NAS_SYS_INFO_IND       = 78, // 78 System Information Indication.
   eQMI_NAS_GET_SIG_INFO       = 79, // 79 Get information regarding signal strength
   eQMI_NAS_CONFIG_SIG_INFO    = 80, // 80 Config Sig information
   eQMI_NAS_SIG_INFO_IND       = 81, // 81 Signal Information Indication
   eQMI_NAS_GET_ERR_RATE       = 82, // 82 Get Err Rate
   eQMI_NAS_GET_HDR_COLOR_CODE = 87, // 87 Get HDR Color Code
   eQMI_NAS_GET_TX_RX_INFO     = 90, // 90 Get detailed Tx/Rx information
   eQMI_NAS_RF_BAND_INFO_IND   = 102, // 102 Reports current RF band information
   eQMI_NAS_NETWORK_REJECT_IND = 104,// 104 Reports network reject information
   eQMI_NAS_CONFIG_SIG_INFO2   = 108, // 108 Config Sig information 2
   eQMI_NAS_GET_NETWORK_TIME   = 125, // 0x007D Get Network Time
   eQMI_NAS_LTE_CPHY_CA_IND    = 130, // 0x0082 Indicates a crarrier aggregation event has occurred.
   eQMI_NAS_GET_LTE_CPHY_CA_INFO = 172,//0x00AC get Previous carrier aggregation event infromation
   eQMI_NAS_SET_EDRX           = 186, //0x00BA Sets the extended DRX for the device
   eQMI_NAS_GET_EDRX           = 187,  //0x00BB Retrieves the eDRX for the device
   eQMI_NAS_EDRX_CHANGE_INFO_IND = 191, // 0x00BF Indicates a change in the eDRX value.
   eQMI_NAS_SET_EDRX_PARAMS    = 192, // 0x00C0 Sets the eDRX parameters for the device
   eQMI_NAS_GET_EDRX_PARAMS    = 193, // 0x00C1 Get the eDRX parameters for the device
   eQMI_NAS_SWI_MODEM_STATUS   = 0x5556, // Get current status of Modem
   eQMI_NAS_SWI_GET_HRPD_STATS = 0x5561, // Get HRPD stats
   eQMI_NAS_SWI_INDICATION_REGISTER = 0x5566, // Set Swi Nas Indication Register
   eQMI_NAS_SWI_OTA_MESSAGE_INDICATION = 0x5567, /* OTA Message Indication */
   eQMI_NAS_SWI_GET_CHANNEL_LOCK    = 0x5568, // Get Channel lock information
   eQMI_NAS_SWI_SET_CHANNEL_LOCK    = 0x5569, // Set Channel lock information
   eQMI_NAS_SWI_RANK_INDICATOR_IND  = 0x556A, /* Rank Incicator Indication */
   eQMI_NAS_SWI_GET_LTE_SCC_RX_INFO = 0x556B, // Get LTE SCC Rx Info
   eQMI_NAS_SWI_HDR_PERSONALITY     = 0x7558, // Get HDR Personality Information
   eQMI_NAS_SWI_HDR_PERSONALITY_IND = 0x7558, // Get HDR Personality Information Indication
   eQMI_NAS_SWI_HDR_PROT_SUBTYPE    = 0x7559, // Get HDR Proto Subtype Information
   eQMI_NAS_SWI_GET_CQI             = 0x7561, // Get CQI for LTE.
   eQMI_NAS_SWI_NETWORK_DEBUG       = 0x7562, // Swi Network Debug
   eQMI_NAS_SWI_NETWORK_TIMERS_IND  = 0x7563,
   eQMI_NAS_SWI_PS_DETACH           = 0x655D, //Initiate PS Detach
   eQMI_NAS_ENUM_END
};

/* QMI QOS Service Type Message ID Enumeration */
enum eQMIMessageQOS
{
   eQMI_QOS_ENUM_BEGIN = -1,

   eQMI_QOS_RESET = 0x00,
   eQMI_QOS_SET_EVENT = 1,            // 01 Set qos report conditions
   eQMI_QOS_EVENT_IND = 1,        // 01 qos report indication
   eQMI_QOS_REQ = 0x20,
   eQMI_QOS_REL = 0x21,
   eQMI_QOS_SUSPEND = 0x22,
   eQMI_QOS_RESUME = 0x23,
   eQMI_QOS_GET_GRANTED = 0x25,
   eQMI_QOS_GET_FLOW_STATUS = 0x26,
   eQMI_QOS_FLOW_STATUS_IND = 0x26,
   eQMI_QOS_GET_NW_STATUS = 0x27,
   eQMI_QOS_NETWORK_STATUS_IND = 0x27,  // Network Qos Support Status indication
   eQMI_QOS_GET_NW_PROFILES = 0x28,
   eQMI_QOS_PRIMARY_QOS_EVENT_IND = 0x29,
   eQMI_QOS_BIND_DATA_PORT = 0x2B,
   eQMI_QOS_SWI_READ_DATA_STATS = 0x5556,
   eQMI_QOS_SWI_READ_APN_PARAMS = 0x5557,

   eQMI_QOS_ENUM_END
};

/* QMI WMS Service Type Message ID Enumeration */
enum eQMIMessageWMS
{
   eQMI_WMS_ENUM_BEGIN = -1,

   eQMI_WMS_RESET,                  // 00 Reset WMS service state variables
   eQMI_WMS_SET_EVENT,              // 01 Set new message report conditions
   eQMI_WMS_EVENT_IND = 1,          // 01 New message report indication

   eQMI_WMS_RAW_SEND = 32,          // 32 Send a raw message
   eQMI_WMS_RAW_WRITE,              // 33 Write a raw message to the device
   eQMI_WMS_RAW_READ,               // 34 Read a raw message from the device
   eQMI_WMS_MODIFY_TAG,             // 35 Modify message tag on the device
   eQMI_WMS_DELETE,                 // 36 Delete message by index/tag/memory

   eQMI_WMS_GET_MSG_PROTOCOL = 48,  // 48 Get the current message protocol
   eQMI_WMS_GET_MSG_LIST,           // 49 Get list of messages from the device
   eQMI_WMS_SET_ROUTES,             // 50 Set routes for message memory storage
   eQMI_WMS_GET_ROUTES,             // 51 Get routes for message memory storage
   eQMI_WMS_GET_SMSC_ADDR,          // 52 Get SMSC address
   eQMI_WMS_SET_SMSC_ADDR,          // 53 Set SMSC address
   eQMI_WMS_GET_MSG_LIST_MAX,       // 54 Get maximum size of SMS storage
   eQMI_WMS_SEND_ACK,               // 55 Send ACK
   eQMI_WMS_SET_RETRY_PERIOD,       // 56 Set retry period
   eQMI_WMS_SET_RETRY_INTERVAL,     // 57 Set retry interval
   eQMI_WMS_SET_DC_DISCO_TIMER,     // 58 Set DC auto-disconnect timer
   eQMI_WMS_SET_MEMORY_STATUS,      // 59 Set memory storage status
   eQMI_WMS_SET_BC_ACTIVATION,      // 60 Set broadcast activation
   eQMI_WMS_SET_BC_CONFIG,          // 61 Set broadcast config
   eQMI_WMS_GET_BC_CONFIG,          // 62 Get broadcast config
   eQMI_WMS_MEMORY_FULL_IND,        // 63 Memory full indication
   eQMI_WMS_GET_DOMAIN_PREF,        // 64 Get domain preference
   eQMI_WMS_SET_DOMAIN_PREF,        // 65 Set domain preference
   eQMI_WMS_MEMORY_SEND,            // 66 Send message from memory store
   eQMI_WMS_GET_MSG_WAITING,        // 67 Get message waiting info
   eQMI_WMS_MSG_WAITING_IND,        // 68 Message waiting indication
   eQMI_WMS_SET_PRIMARY_CLIENT,     // 69 Set client as primary client
   eQMI_WMS_SMSC_ADDR_IND,          // 70 SMSC address indication

   eQMI_WMS_SET_IND_REGISTER      = 71,  // 71 Set indication register
   eQMI_WMS_GET_TRANS_LAYER_INFO  = 72,  // 72 Get Transport Layer Information
   eQMI_WMS_TRANS_LAYER_INFO_IND  = 73,  // 73 Transport Layer Info Indication
   eQMI_WMS_GET_TRANS_NW_REG_INFO = 74,  // 74 Get Transport NW Reg Info
   eQMI_WMS_TRANS_NW_REG_INFO_IND = 75,  // 75 Transport NW Reg Info Indication
   eQMI_WMS_GET_IND_REGISTER      = 77,  // 77 Get indication register
   eQMI_WMS_ASYNC_RAW_SEND        = 89,  // 89 Send an Async raw message
   eQMI_WMS_ASYNC_RAW_SEND_IND    = 89,
   eQMI_WMS_SWI_GET_SMS_STORAGE   = 0x5556,
   eQMI_WMS_SWI_SET_SMS_STORAGE   = 0x5557,
   eQMI_WMS_ENUM_END
};

/* QMI PDS Service Type Message ID Enumeration */
enum eQMIMessagePDS
{
   eQMI_PDS_ENUM_BEGIN = -1,

   eQMI_PDS_RESET,                // 00 Reset PDS service state variables
   eQMI_PDS_SET_EVENT,            // 01 Set PDS report conditions
   eQMI_PDS_EVENT_IND = 1,        // 01 PDS report indication

   eQMI_PDS_GET_STATE = 32,       // 32 Return PDS service state
   eQMI_PDS_STATE_IND = 32,       // 32 PDS service state indication
   eQMI_PDS_SET_STATE,            // 33 Set PDS service state
   eQMI_PDS_START_SESSION,        // 34 Start a PDS tracking session
   eQMI_PDS_GET_SESSION_INFO,     // 35 Get PDS tracking session info
   eQMI_PDS_FIX_POSITION,         // 36 Manual tracking session position
   eQMI_PDS_END_SESSION,          // 37 End a PDS tracking session
   eQMI_PDS_GET_NMEA_CFG,         // 38 Get NMEA sentence config
   eQMI_PDS_SET_NMEA_CFG,         // 39 Set NMEA sentence config
   eQMI_PDS_INJECT_TIME,          // 40 Inject a time reference
   eQMI_PDS_GET_DEFAULTS,         // 41 Get default tracking session config
   eQMI_PDS_SET_DEFAULTS,         // 42 Set default tracking session config
   eQMI_PDS_GET_XTRA_PARAMS,      // 43 Get the GPS XTRA parameters
   eQMI_PDS_SET_XTRA_PARAMS,      // 44 Set the GPS XTRA parameters
   eQMI_PDS_FORCE_XTRA_DL,        // 45 Force a GPS XTRA database download
   eQMI_PDS_GET_AGPS_CONFIG,      // 46 Get the AGPS mode configuration
   eQMI_PDS_SET_AGPS_CONFIG,      // 47 Set the AGPS mode configuration

   eQMI_PDS_GET_SVC_AUTOTRACK,    // 48 Get the service auto-tracking state
   eQMI_PDS_SET_SVC_AUTOTRACK,    // 49 Set the service auto-tracking state
   eQMI_PDS_GET_COM_AUTOTRACK,    // 50 Get COM port auto-tracking config
   eQMI_PDS_SET_COM_AUTOTRACK,    // 51 Set COM port auto-tracking config
   eQMI_PDS_RESET_DATA,           // 52 Reset PDS service data
   eQMI_PDS_SINGLE_FIX,           // 53 Request single position fix
   eQMI_PDS_GET_VERSION,          // 54 Get PDS service version
   eQMI_PDS_INJECT_XTRA,          // 55 Inject XTRA data
   eQMI_PDS_INJECT_POSITION,      // 56 Inject position data
   eQMI_PDS_INJECT_WIFI,          // 57 Inject Wi-Fi obtained data
   eQMI_PDS_GET_SBAS_CONFIG,      // 58 Get SBAS config
   eQMI_PDS_SET_SBAS_CONFIG,      // 59 Set SBAS config
   eQMI_PDS_SEND_NI_RESPONSE,     // 60 Send network initiated response
   eQMI_PDS_INJECT_ABS_TIME,      // 61 Inject absolute time
   eQMI_PDS_INJECT_EFS,           // 62 Inject EFS data
   eQMI_PDS_GET_DPO_CONFIG,       // 63 Get DPO config
   eQMI_PDS_SET_DPO_CONFIG,       // 64 Set DPO config
   eQMI_PDS_GET_ODP_CONFIG,       // 65 Get ODP config
   eQMI_PDS_SET_ODP_CONFIG,       // 66 Set ODP config
   eQMI_PDS_CANCEL_SINGLE_FIX,    // 67 Cancel single position fix
   eQMI_PDS_GET_GPS_STATE,        // 68 Get GPS state

   eQMI_PDS_GET_METHODS = 80,     // 80 Get GPS position methods state
   eQMI_PDS_SET_METHODS,          // 81 Set GPS position methods state

   eQMI_PDS_ENUM_END
};

/* QMI AUTH Service Type Message ID Enumeration */
enum eQMIMessageAUTH
{
   eQMI_AUTH_ENUM_BEGIN = -1,

   eQMI_AUTH_START_EAP = 32,        // 32 Start the EAP session
   eQMI_AUTH_SEND_EAP,              // 33 Send and receive EAP packets
   eQMI_AUTH_EAP_RESULT_IND,        // 34 EAP session result indication
   eQMI_AUTH_GET_EAP_KEYS,          // 35 Get the EAP session keys
   eQMI_AUTH_END_EAP,               // 36 End the EAP session

   eQMI_AUTH_ENUM_END
};
/* QMI CAT Service Type Message ID Enumeration */
enum eQMIMessageCAT
{
   eQMI_CAT_ENUM_BEGIN = -1,

   eQMI_CAT_RESET,                  // 00 Reset CAT service state variables
   eQMI_CAT_SET_EVENT,              // 01 Set new message report conditions
   eQMI_CAT_EVENT_IND = 1,          // 01 New message report indication

   eQMI_CAT_GET_STATE = 32,         // 32 Get service state information
   eQMI_CAT_SEND_TERMINAL,          // 33 Send a terminal response
   eQMI_CAT_SEND_ENVELOPE,          // 34 Send an envelope command

   eQMI_CAT_ENUM_END
};

/* QMI RMS Service Type Message ID Enumeration */
enum eQMIMessageRMS
{
   eQMI_RMS_ENUM_BEGIN = -1,

   eQMI_RMS_RESET,                  // 00 Reset RMS service state variables

   eQMI_RMS_GET_SMS_WAKE = 32,      // 32 Get SMS wake settings
   eQMI_RMS_SET_SMS_WAKE,           // 33 Set SMS wake settings

   eQMI_RMS_ENUM_END
};

/* QMI OMA-DM Service Type Message ID Enumeration */
enum eQMIMessageOMA
{
   eQMI_OMA_ENUM_BEGIN = -1,

   eQMI_OMA_RESET,                  // 00 Reset OMA service state variables
   eQMI_OMA_SET_EVENT,              // 01 Set OMA report conditions
   eQMI_OMA_EVENT_IND = 1,          // 01 OMA report indication

   eQMI_OMA_START_SESSION = 32,     // 32 Start client inititated session
   eQMI_OMA_CANCEL_SESSION,         // 33 Cancel session
   eQMI_OMA_GET_SESSION_INFO,       // 34 Get session information
   eQMI_OMA_SEND_SELECTION,         // 35 Send selection for net inititated msg
   eQMI_OMA_GET_FEATURES,           // 36 Get feature settings
   eQMI_OMA_SET_FEATURES,           // 37 Set feature settings

   eQMI_OMA_ENUM_END
};

/* SWI QMI OMA-DM Service Type Message ID Enumeration */
enum eQMIMessageSWIOMA
{
   eQMI_SWIOMA_ENUM_BEGIN = -1,

   eQMI_SWIOMA_START_SESSION = 1,     // 1 Start client inititated session
   eQMI_SWIOMA_CANCEL_SESSION,        // 2 Cancel session
   eQMI_SWIOMA_GET_SESSION_INFO,      // 3 Get session information
   eQMI_SWIOMA_EVENT_IND,             // 4 OMA report indication
   eQMI_SWIOMA_SEND_SELECTION,        // 5 Send selection for net inititated msg
   eQMI_SWIOMA_GET_SETTINGS,          // 6 Get feature settings
   eQMI_SWIOMA_SET_SETTINGS,          // 7 Set feature settings
   eQMI_SWIOMA_SET_EVENT,             // 8 Set OMA report conditions

   eQMI_SWIOMA_ENUM_END
};

/* QMI Device Connectivity Service (DCS) Type Message ID Enumeration
 * should matches dcshandlertbl @ qmdcs.c
 */
enum eQMIMessageDCS
{
   eQMI_DCS_ENUM_BEGIN = -1,
   eQMI_DCS_ENUMERATE_DEVICES,          // QCWWANEnumerateDevices API QMI message
   eQMI_DCS_CONNECT,                    // QCWWANConnect                -"-
   eQMI_DCS_DISCONNECT,                 // QCWWANDisconnect             -"-
   eQMI_DCS_GET_CONNECTED_DEVICE_ID,    // QCWWANGetConnectedDeviceIP   -"-
   eQMI_DCS_CANCEL,                     // QCWWANCancel                 -"-
   eQMI_DCS_GET_USB_PORT_NAMES,         // SLQSGetUsbPortNames          -"-
   eQMI_DCS_GET_DEVICE_MODE,            // SLQSGetDeviceMode            -"-
   eQMI_DCS_START_SRV,                  // SLQSStartSrv                 -"-
   eQMI_DCS_QOS_MAP,                    // SLQSQosMap                   -"-
   eQMI_DCS_QOS_UNMAP,                  // SLQSQosUnmap                 -"-
   eQMI_DCS_QOS_EDIT_MAP,               // SLQSQosEditMap               -"-
   eQMI_DCS_QOS_READ_MAP,               // SLQSQosReadMap               -"-
   eQMI_DCS_QOS_CLEAR_MAP,              // SLQSQosClearMap              -"-
   eQMI_DCS_QOS_DUMP_MAP,               // SLQSQosDumpMap               -"-
   eQMI_DCS_SET_NET_CHANGE,             // SetNetChangeCbk              -"-
   eQMI_DCS_SET_LOGGING_MASK,
   eQMI_DCS_GET_NET_STATS,
   eQMI_DCS_GET_QMAP_SUPPORT,
   eQMI_DCS_SET_QMAP_IP_TABLE,
   eQMI_DCS_SET_QMAP_IPv6_TABLE,
   eQMI_DCS_EVENT_IND,
   eQMI_DCS_NET_EVENT_IND,
   eQMI_DCS_ENUM_END
};

/* QMI SAR Service Type Message ID Enumeration */
enum eQMIMessageSAR
{
   eQMI_SAR_ENUM_BEGIN = -1,
   eQMI_SAR_RF_SET_STATE = 1,
   eQMI_SAR_RF_GET_STATE,
   eQMI_SAR_ENUM_END
};

/* QMI VOICE Service Type Message ID Enumeration */
enum eQMIMessageVOICE
{
   eQMI_VOICE_ENUM_BEGIN = -1,
   eQMI_VOICE_INDICATION_REGISTER       =  3, //Voice Indication register QMI message
   eQMI_VOICE_DIAL_CALL                 = 32, //SLQSVoiceDialCall API QMI message
   eQMI_VOICE_END_CALL                  = 33, //SLQSVoiceEndCall API QMI message
   eQMI_VOICE_ANSWER_CALL               = 34, //SLQSAnswerCall API
   eQMI_VOICE_GET_CALL_INFO             = 36, //SLQSVoiceGetCallInfo API QMI message
   eQMI_VOICE_OTASP_STATUS_IND          = 37, //SLQSVoiceSetOTASPStatusCallback
   eQMI_VOICE_INFO_REC_IND              = 38, //SLQSVoiceInfoRecCallback
   eQMI_VOICE_SEND_FLASH                = 39, //SLQSVoiceSendFlash API QMI message
   eQMI_VOICE_BURST_DTMF                = 40, //SLQSVoiceBurstDTMF
   eQMI_VOICE_START_CONT_DTMF           = 41, //SLQSVoiceStartContDTMF
   eQMI_VOICE_STOP_CONT_DTMF            = 42, //SLQSVoiceStopContDTMF
   eQMI_VOICE_DTMF_IND                  = 43, //SLQSVoiceSetDTMFEventCallBack QMI message
   eQMI_VOICE_SET_PREFERRED_PRIVACY     = 44, //SLQSVoiceSetPreferredPrivacy API QMI message
   eQMI_VOICE_PRIVACY_IND               = 45, //SLQSVoiceSetPrivacyChangeCallBack
   eQMI_VOICE_ALL_CALL_STATUS_IND       = 46, //SLQSVoiceSetAllCallStatusCallBack
   eQMI_VOICE_GET_ALL_CALL_INFO         = 47, //SLQSVoiceGetAllCallInfo API QMI message
   eQMI_VOICE_MANAGE_CALLS              = 49, //SLQSVoiceManageCalls
   eQMI_VOICE_SUPS_NOTIFICATION_IND     = 50, //SLQSVoiceSetSUPSNotificationCallBack API QMI message
   eQMI_VOICE_SET_SUPS_SERVICE          = 51, //SLQSVoiceSetSUPSService API QMI message
   eQMI_VOICE_GET_CALL_WAITING          = 52, //SLQSVoiceGetCallWaiting API QMI message
   eQMI_VOICE_GET_CALL_BARRING          = 53, //SLQSVoiceGetCallBarring API QMI message
   eQMI_VOICE_GET_CLIP                  = 54, //SLQSVoiceGetCLIP API QMI message
   eQMI_VOICE_GET_CLIR                  = 55, //SLQSVoiceGetCLIR API QMI message
   eQMI_VOICE_GET_CALL_FORWARDING       = 56, //SLQSVoiceGetCallForwarding API QMI message
   eQMI_VOICE_SET_CALL_BARRING_PASSWORD = 57, //SLQSVoiceSetCallBarringPassword API QMI message
   eQMI_VOICE_ORIGINATE_USSD            = 58, //OrignateUSSD API QMI message
   eQMI_VOICE_ANSWER_USSD               = 59, //AnswerUSSD API QMI message
   eQMI_VOICE_CANCEL_USSD               = 60, //CancelUSSD API QMI message
   eQMI_VOICE_USSD_RELEASE_IND          = 61, //Release Indication
   eQMI_VOICE_USSD_IND                  = 62, //USSD Indication
   eQMI_VOICE_SET_CONFIG                = 64, //SLQSVoiceSetConfig API QMI message
   eQMI_VOICE_GET_CONFIG                = 65, //SLQSVoiceGetConfig API QMI message
   eQMI_VOICE_SUPS_IND                  = 66, //SLQSVoiceSetSUPSCallBack QMI message
   eQMI_VOICE_ORIG_USSD_NO_WAIT         = 67, //SLQSVoiceOrigUSSDNoWait  API QMI message
   eQMI_VOICE_ORIG_USSD_NO_WAIT_IND     = 67, //USSD No wait Indication
   eQMI_VOICE_BIND_SUBSCRIPTION         = 68, //SLQSVoiceBindSubscription API QMI message
   eQMI_VOICE_ALS_SET_LINE_SWITCHING    = 69, //SLQSVoiceALSSetLineSwitching API QMI message
   eQMI_VOICE_ALS_SELECT_LINE           = 70, //SLQSVoiceALSSelectLine API QMI message
   eQMI_VOICE_GET_COLP                  = 75, //SLQSVoiceGetCOLP API QMI message
   eQMI_VOICE_GET_COLR                  = 76, //SLQSVoiceGetCOLR API QMI message
   eQMI_VOICE_GET_CNAP                  = 77, //SLQSVoiceGetCNAP API QMI message
   eQMI_VOICE_ENUM_END
};

/* QMI Audio Service Type Message ID Enumeration */
enum eQMIMessageAUDIO
{
   eQMI_AUDIO_ENUM_BEGIN               = -1,
   eQMI_AUDIO_GET_PROFILE              = 96, //SLQSGetAudioProfile
   eQMI_AUDIO_SET_PROFILE              = 97, //SLQSSetAudioProfile
   eQMI_AUDIO_GET_AUDIO_PATH_CONFIG    = 100, //SLQSGetAudioPathConfig
   eQMI_AUDIO_SET_AUDIO_PATH_CONFIG    = 101, //SLQSSetAudioPathConfig
   eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG = 102, //SLQSGetAudioVolTLBConfig
   eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG = 103, //SLQSSetAudioVolTLBConfig
   eQMI_AUDIO_ENUM_END
};

/* QMI IMS Service Type Message ID Enumeration */
enum eQMIMessageIMS
{
   eQMI_IMS_ENUM_BEGIN                  = -1,
   eQMI_IMS_SETTINGS_SET_SIP_CONFIG     = 32, //SLQSSetSIPConfig
   eQMI_IMS_SETTINGS_SET_REG_MGR_CFG    = 33, //SLQSSetRegMgrConfig
   eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG    = 34, //SLQSSetIMSSMSConfig
   eQMI_IMS_SETTINGS_SET_IMS_USER_CFG   = 35, //SLQSSetIMSUserConfig
   eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG   = 36, //SLQSSetIMSVoIPConfig
   eQMI_IMS_SETTINGS_GET_SIP_CONFIG     = 37, //SLQSGetSIPConfig
   eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG = 38, //SLQSGetRegMgrConfig
   eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG = 39, //SLQSGetIMSSMSConfig
   eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG = 40, //SLQSGetIMSUserConfig
   eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG   = 41, //SLQSGetIMSVoIPConfig
   eQMI_IMS_CONFIG_INDICATION_REGISTER  = 42, //SLQSImsConfigIndicationRegister
   eQMI_IMS_SIP_CONFIG_IND              = 43, //SLQSSetSIPCfgInfoCallback
   eQMI_IMS_REG_MGR_CONFIG_IND          = 44, //SLQSSetRegMgrConfigCallback
   eQMI_IMS_SMS_CONFIG_IND              = 45, //SLQSSetIMSSMSConfigCallback
   eQMI_IMS_USER_CONFIG_IND             = 46, //SLQSSetIMSUserConfigCallback
   eQMI_IMS_VOIP_CONFIG_IND             = 47, //SLQSSetIMSVoIPConfigCallback
   eQMI_IMS_ENUM_END
};

/* QMI SWI Audio Service Type Message ID Enumeration */
enum eQMIMessageSWIAUDIO
{
   eQMI_SWIAUDIO_ENUM_BEGIN   = -1,
   eQMI_SWIAUDIO_GET_PROFILE  = 0x01, //SLQSGetM2MAudioProfile
   eQMI_SWIAUDIO_SET_PROFILE  = 0x02, //SLQSSetM2MAudioProfile
   eQMI_SWIAUDIO_GET_VOLUME   = 0x03, //SLQSGetM2MAudioVolume
   eQMI_SWIAUDIO_SET_VOLUME   = 0x04, //SLQSSetM2MAudioVolume
   eQMI_SWIAUDIO_SET_LPBK     = 0x09, //SLQSSetM2MAudioLPBK
   eQMI_SWIAUDIO_SET_NV_DEF   = 0x0C, //SLQSSetM2MAudioNVDef
   eQMI_SWIAUDIO_GET_SPKRGAIN = 0x12, //SLQSSetM2MAudioVolume
   eQMI_SWIAUDIO_SET_SPKRGAIN = 0x13, //SLQSSetM2MAudioVolume
   eQMI_SWIAUDIO_SET_AVCFG    = 0x1B, //SLQSSetM2MAudioAVGC
   eQMI_SWIAUDIO_GET_AVMUTE   = 0x1C, //SLQSGetM2MAVMute
   eQMI_SWIAUDIO_SET_AVMUTE   = 0x1D, //SLQSSetM2MAVMute
   eQMI_SWIAUDIO_ENUM_END
};

/* QMI LOC Service Type Message ID Enumeration */
enum eQMIMessageLOC
{
   eQMI_LOC_ENUM_BEGIN                  = -1,
   eQMI_LOC_REG_EVENTS                  = 0x21,
   eQMI_LOC_START                       = 0x22,
   eQMI_LOC_STOP                        = 0x23,
   eQMI_LOC_EVENT_POSITION_REPORT_IND   = 0x24,
   eQMI_LOC_EVENT_GNSS_SV_INFO_IND      = 0x25,
   eQMI_LOC_EVENT_NMEA_IND              = 0x26,
   eQMI_LOC_EVENT_ENGINE_STATE_IND      = 0x2B,
   eQMI_LOC_SENSOR_STREAMING_STATUS_IND = 0x2E,
   eQMI_LOC_EVENT_TIME_SYNC_REQ_IND     = 0x2F,
   eQMI_LOC_GET_FIX_CRITERIA            = 0x33,
   eQMI_LOC_GET_FIX_CRITERIA_IND        = 0x33,
   eQMI_LOC_INJECT_UTC_TIME             = 0x38,
   eQMI_LOC_INJECT_POSITION             = 0x39,
   eQMI_LOC_SET_SERVER                  = 0x42,
   eQMI_LOC_SET_SERVER_IND              = 0x42,
   eQMI_LOC_GET_SERVER                  = 0x43,
   eQMI_LOC_GET_SERVER_IND              = 0x43,
   eQMI_LOC_DELETE_ASSIST_DATA          = 0x44,
   eQMI_LOC_DELETE_ASSIST_DATA_IND      = 0x44,
   eQMI_LOC_SET_OPERATION_MODE          = 0x4A,
   eQMI_LOC_SET_OPERATION_MODE_IND      = 0x4A,
   eQMI_LOC_GET_OPERATION_MODE          = 0x4B,
   eQMI_LOC_GET_OPERATION_MODE_IND      = 0x4B,
   eQMI_LOC_INJECT_SENSOR_DATA          = 0x4D,
   eQMI_LOC_INJECT_SENSOR_DATA_IND      = 0x4D,
   eQMI_LOC_INJECT_TIME_SYNC_DATA_IND   = 0x4E,
   eQMI_LOC_SET_CRADLE_MOUNT_CONFIG     = 0x4F,
   eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND = 0x4F,
   eQMI_LOC_SET_EXTERNAL_POWER_CONFIG   = 0x51,
   eQMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND = 0x51,
   eQMI_LOC_GET_BEST_AVAIL_POS          = 0x67,
   eQMI_LOC_GET_BEST_AVAIL_POS_IND      = 0x67,
   eQMI_LOC_ENUM_END
};

/* Swi QMI LOC Service Type Message ID Enumeration */
enum eQMIMessageSwiLOC
{
   eQMI_SWI_LOC_ENUM_BEGIN                  = -1,
   eQMI_SWI_LOC_GET_AUTO_START              = 0x01,
   eQMI_SWI_LOC_SET_AUTO_START              = 0x02,
   eQMI_SWI_LOC_ENUM_END
};

/* QMI IMSA Service Type Message ID Enumeration */
enum eQMIMessageIMSA
{
   eQMI_IMSA_ENUM_BEGIN                 = -1,
   eQMI_IMSA_GET_SUPPORTED_MSGS         = 0x1E,
   eQMI_IMSA_GET_SUPPORTED_FIELDS       = 0x1F,
   eQMI_IMSA_GET_REGISTRATION_STATUS    = 0x20,
   eQMI_IMSA_GET_SERVICE_STATUS         = 0x21,
   eQMI_IMSA_INDICATION_REGISTER        = 0x22, //SLQSRegisterIMSAIndication
   eQMI_IMSA_REGISTRATION_STATUS_IND    = 0x23,
   eQMI_IMSA_SERVICE_STATUS_IND         = 0x24,
   eQMI_IMSA_RAT_HANDOVER_STATUS_IND    = 0x25,
   eQMI_IMSA_PDP_STATUS_IND             = 0x26,
   eQMI_IMSA_ENUM_END
};

/* QMI TMD Service Type Message ID Enumeration */
enum eQMIMessageTMD
{
   eQMI_TMD_ENUM_BEGIN                  = -1,
   eQMI_TMD_GET_MITIGATION_DEV_LIST     = 0x20,
   eQMI_TMD_GET_MITIGATION_LEVEL        = 0x22,
   eQMI_TMD_REG_NOT_MITIGATION_LEVEL    = 0x23,
   eQMI_TMD_DEREG_NOT_MITIGATION_LEVEL  = 0x24,
   eQMI_TMD_MITIGATION_LEVEL_REPORT_IND = 0x25,
   eQMI_TMD_ENUM_END
};

enum eQMI_DCS_ENUMERATE_TLVs
{
    eTLV_DCS_ENUMERATE_DEVICE_NODE,
    eTLV_DCS_ENUMERATE_DEVICE_KEY
};

enum eQMI_DCS_CONNECT_TLVs
{
    eTLV_DCS_CONNECT_DEVICE_NODE,
    eTLV_DCS_CONNECT_DEVICE_KEY,
    eTLV_DCS_SDK_PID=3
};

enum eQMI_DCS_START_SRV_REQ_TLVs
{
    eTLV_ACTION,
    eTLV_MASK
};

enum eQMI_DCS_SET_LOGGING_MASK_REQ_TLVs
{
    eTLV_LEVEL
};

enum eQMI_DCS_GET_NET_STATS_TLVs
{
    eTLV_DCS_GET_NET_STATS = 0x10
};

enum eQMI_DCS_GET_QMAP_SUPPORT_TLVs
{
    eTLV_DCS_GET_QMAP_SUPPORT = 0x10
};


enum
{
    eTLV_DSCP,
    eTLV_QOS_ID
};

enum eQMI_DCS_DISCONNECT_TLVs
{
    eTLV__DCS_DISCONNECT_TLVs_PLACEHOLDER
};

enum eQMI_DCS_CONNECTED_DEVICEID_TLVs
{
    eTLV_DCS_CONNECTED_DEVICEID_DEVICE_NODE,
    eTLV_DCS_CONNECTED_DEVICEID_DEVICE_KEY
};

enum eQMI_DCS_CANCEL_TLVs
{
    eTLV_DCS_CANCEL_TLVs_PLACEHOLDER
};

enum eQMI_DCS_GET_USB_PORT_NAME_TLVs
{
    eTLV_DCS_GET_USB_PORT_NAME_ATCMDPORT =0x10,
    eTLV_DCS_GET_USB_PORT_NAME_NMEAPORT,
    eTLV_DCS_GET_USB_PORT_NAME_DMPORT
};

enum
{
    eTLV_DCS_READ_MAP = 0x10
};

enum
{
    eTLV_DCS_DUMP_MAP = 0x10
};

enum eQMI_DCS_GET_DEVICE_MODE_TLVs
{
    eTLV_DCS_GET_DEVICE_MODE =0x10
};

enum
{
    eTLV_DCS_START_SRV = 0x10
};

enum eQMI_DCS_EVENT_INDICATION_TLVs
{
    eTLV_DCS_IND_DEVICE_STATE   = 0x10,
    eTLV_DCS_IND_SDK_TERMINATED = 0x11
};

enum eQMI_DCS_NET_EVENT_INDICATION_TLVs
{
    eTLV_DCS_IND_NET   = 0x10
};

enum
{
    eTLV_WATERMARKS = 0x01,
    eTLV_PERIOD
};

enum
{
    eTLV_DCS_QMAP_SET_IP_TABLE = 0x10
};


enum
{
    eTLV_DCS_QMAP_SET_IPv6_MUXID = 0x10,
    eTLV_DCS_QMAP_SET_IPv6_IP_ADDR = 0x11,
    eTLV_DCS_QMAP_SET_IPv6_PREFIX_LEN = 0x12
};


/* QMI Firmware Management Service (FMS) Type Message ID Enumeration */
enum eQMIMessageFMS
{
   eQMI_FMS_ENUM_BEGIN = -1,
   eQMI_FMS_SET_IMAGE_PATH,     /* support for UpgradeFirmware2k() API */
   eQMI_FMS_GET_CWE_SPKGS_INFO, /* support for SLQSGetImageInfo() API */
   eQMI_FMS_GET_MBN_INFO,       /* support for GetImageInfo() API */
   eQMI_FMS_GET_CWE_PART_NO,    /* support for SLQSGetImagePartNo() API */
   eQMI_FMS_EVENT_IND,          /* FMS notifications to host */
   eQMI_FMS_ENUM_END
};

enum eQMI_FMS_SET_IMAGE_PATH_TLVs
{
    eTLV_FMS_IMAGE_PATH,
    eTLV_IMG_TYPE,
    eTLV_FMS_MDM_TYPE,
    eTLV_FMS_IMG_MASK,
    eTLV_FMS_MDM_PROTOCOL
};

enum eQMI_FMS_EVENT_INDICATION_TLVs
{
    eTLV_FMS_IND_FW_DWLD_COMPLETE   = 0x10
};

enum eQMI_FMS_GET_CWE_SPKGS_INFO_TLVs
{
    /* request TLVs */
    eTLV_FMS_GET_CWE_SPKGS_INFO_IMGDIR_PATH,
    eTLV_FMS_GET_CWE_SPKGS_INFO_IMGTYPE,
    /* response TLVs */
    eTLV_FMS_GET_CWE_SPKGS_INFO_SKUID_STR = 0x10,
    eTLV_FMS_GET_CWE_SPKGS_INFO_MODELID_STR,
    eTLV_FMS_GET_CWE_SPKGS_INFO_FWVERSION_STR,
    eTLV_FMS_GET_CWE_SPKGS_INFO_CARRIER_STR,
    eTLV_FMS_GET_CWE_SPKGS_INFO_PRIVERSION_STR,
    eTLV_FMS_GET_CWE_SPKGS_INFO_PKGVERSION_STR
};

enum eQMI_FMS_GET_CWE_PART_NO_TLVs
{
    /* request TLVs */
    eTLV_FMS_GET_CWE_PART_NO_IMGDIR_PATH,
    eTLV_FMS_GET_CWE_PART_NO_IMGTYPE,
    /* response TLVs */
    eTLV_FMS_GET_CWE_PART_NO_STR = 0x10,
};

enum eQMI_FMS_GET_MBN_INFO_TLVs
{
    /* request TLVs */
    eTLV_FMS_GET_MBN_INFO_IMGDIR_PATH,
    eTLV_FMS_GET_MBN_INFO_IMGTYPE,
    /* response TLVs */
    eTLV_FMS_GET_MBN_INFO_FIRMWAREID = 0x10,
    eTLV_FMS_GET_MBN_INFO_FWVERSION_STR,
    eTLV_FMS_GET_MBN_INFO_FIRMWAREID_STR
};

enum eQMI_WDS_INT_SET_SESSION_ID_TLVs
{
    eTLV_WDS_INTERNAL_SET_SESSIONID = 0x10,
    eTLV_WDS_INTERNAL_SET_INSTANCEID,
};

enum eQMI_WDS_INT_GET_INSTANCE_ID_TLVs
{
    eTLV_WDS_INTERNAL_GETINSTANCE_SESSIONID = 0x10,
    eTLV_WDS_INTERNAL_GETINSTANCE_INSTID,
    eTLV_WDS_INTERNAL_GETINSTANCE_IPFAM,
};

enum eQMI_WDS_INT_GET_SESSION_ID_TLVs
{
    eTLV_WDS_INTERNAL_GETSESSIONID_INST = 0x10,
    eTLV_WDS_INTERNAL_GETSESSIONID_SESSIDv4,
    eTLV_WDS_INTERNAL_GETSESSIONID_SESSIDv6
};

/* QMI UIM Service Type Message ID Enumeration */
enum eQMIMessageUIM
{
   eQMI_UIM_ENUM_BEGIN             = -1,
   eQMI_UIM_RESET                  =  0, //SLQSUIMReset API QMI message
   eQMI_UIM_READ_TRANSPARENT       = 32, //SLQSUIMReadTransparent API QMI message
   eQMI_UIM_READ_RECORD            = 33, //SLQSUIMReadRecord API QMI Message
   eQMI_UIM_WRITE_TRANSPARENT      = 34, //SLQSUIMWriteTransparent API QMI Message
   eQMI_UIM_WRITE_RECORD           = 35, //SLQSUIMWriteRecord API QMI Message
   eQMI_UIM_GET_FILE_ATTRIBUTES    = 36, //SLQSUIMGetFileAttributes API QMI message
   eQMI_UIM_SET_PIN_PROTECTION     = 37, //SLQSUIMSetPinProtection API QMI message
   eQMI_UIM_SET_PIN_PROTECTION_IND = 37, //Set Pin Protection Ind
   eQMI_UIM_VERIFY_PIN             = 38, //SLQSUIMVerifyPin API QMI message
   eQMI_UIM_VERIFY_PIN_IND         = 38, //UIM verify PIN Indication
   eQMI_UIM_UNBLOCK_PIN            = 39, //SLQSUIMUnblockPin API QMI message
   eQMI_UIM_CHANGE_PIN             = 40, //SLQSUIMChangePin API QMI message
   eQMI_UIM_CHANGE_PIN_IND         = 40, //UIM change PIN Ind
   eQMI_UIM_DEPERSONALIZATION      = 41, //SLQSUIMDepersonalization API QMI message
   eQMI_UIM_REFRESH_REGISTER       = 42, //SLQSUIMRefreshRegister
   eQMI_UIM_REFRESH_OK             = 43, //SLQSUIMRefreshOK
   eQMI_UIM_REFRESH_COMPLETE       = 44, //SLQSUIMRefreshComplete
   eQMI_UIM_REFRESH_GET_LAST_EVENT = 45, //SLQSUIMRefreshRegister
   eQMI_UIM_EVENT_REG              = 46, //SLQSUIMEventRegister API QMI message
   eQMI_UIM_GET_CARD_STATUS        = 47, //SLQSUIMGetCardStatus API QMI message
   eQMI_UIM_POWER_DOWN             = 48, //SLQSUIMPowerDown API QMI message
   eQMI_UIM_POWER_UP               = 49, //SLQSUIMPowerUp API QMI message
   eQMI_UIM_STATUS_CHANGE_IND      = 50, //SLQSUIMSetStatusChangeCallBack
   eQMI_UIM_REFRESH_IND            = 51, //SLQSUIMSetRefreshCallBack
   eQMI_UIM_AUTHENTICATE           = 52, //SLQSUIMAuthenticate API QMI message
   eQMI_UIM_GET_SERVICE_STATUS     = 54, //SLQSUIMGetServiceStatus API QMI message
   eQMI_UIM_SET_SERVICE_STATUS     = 55, //SLQSUIMSetServiceStatus API QMI message
   eQMI_UIM_GET_CONFIGURATION      = 58, //SLQSUIMGetConfiguration API QMI message
   eQMI_UIM_SWITCH_SLOT            = 70, //SLQSUIMSwitchSlot API QMI message
   eQMI_UIM_GET_SLOTS_STATUS       = 71, //SLQSUIMGetSlotsStatus
   eQMI_UIM_SLOT_STATUS_IND        = 72 //SetUimSlotStatusChangeCallback
};

/* QMI SWI M2M Service Type Message ID Enumeration */
enum eQMIMessageSWIAVMS
{
    eQMI_SWI_M2M_FDT_OPEN                   = 0x0050, // Request to have exclusive access to SWIFOTA partition
    eQMI_SWI_M2M_FDT_WRITE                  = 0x0051, // Request to write to SWIFOTA partition
    eQMI_SWI_M2M_FDT_CLOSE                  = 0x0052, // Request to release access to SWIFOTA partition
    eQMI_SWI_M2M_FDT_IND                    = 0x0053, // Indication of FDT events
    eQMI_SWI_M2M_FDT_READ_DOWNLOAD_MARKER   = 0x0054, // Request to get the offset from the start of the SWIFOTA partition where the FDT WRITE should resume
    eQMI_SWI_M2M_FDT_SEEK                   = 0x0055, // Request to position the start of the first FDT WRITE
    eQMI_SWI_M2M_FDT_MAX_SIZE               = 0x0056, // Request to query the MAX size reserved for FOTA partition.
    eQMI_SWI_M2M_FDT_READ                   = 0x0057, // Request to read from SWIFOTA partition
    eQMI_SWI_M2M_AVMS_SESSION_START         = 0x00A0, // Perform an OMA-DM AVMS session with the AVMS server
    eQMI_SWI_M2M_AVMS_SESSION_STOP          = 0x00A1, // This message requests the device to cancel an ongoing OMA-DM AVMS session
    eQMI_SWI_M2M_AVMS_SESSION_GETINFO       = 0x00A2, // This message requests the device to cancel an ongoing OMA-DM AVMS session
    eQMI_SWI_M2M_AVMS_EVENT_IND             = 0x00A3, // This unsolicited message is sent by the service to a control point that registers for eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT when any information in the TLV
    eQMI_SWI_M2M_AVMS_SELECTION             = 0x00A4, // This message is used to make a selection after the host has received eQMI_SWI_M2M_AVMS_EVENT_IND.
    eQMI_SWI_M2M_AVMS_SET_SETTINGS          = 0x00A5, // This message sets configuration related to OMA DM AVMS
    eQMI_SWI_M2M_AVMS_GET_SETTINGS          = 0x00A6, // This message gets settings related to OMA DM AVMS
    eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT      = 0x00A7, // This message is used to register for the eQMI_SWI_M2M_AVMS_EVENT_IND
    eQMI_SWI_M2M_AVMS_GET_WAMS              = 0x00A8, // This message is retrieves only those that are used by the application processor
    eQMI_SWI_M2M_AVMS_OPEN_NONCE            = 0x00A9, // This message informs the modem that the application side is about to enter the authentication phase
    eQMI_SWI_M2M_AVMS_CLOSE_NONCE           = 0x00AA, // This message informs the modem that the application side is about to close the authentication phase
    
};

/* QMI SWIDMS Service Type Message ID Enumeration */
enum eQMIMessageSWIDMS
{
   eQMI_SWIDMS_ENUM_BEGIN             = -1,
   eQMI_SWIDMS_GET_USB_INTER_COMP     = 1,
   eQMI_SWIDMS_SET_USB_INTER_COMP     = 2,
   eQMI_SWIDMS_GET_USB_NET_NUM        = 3, //SLQSSwiDmsGetUsbNetNum API QMI message
   eQMI_SWIDMS_SET_USB_NET_NUM        = 4, //SLQSSwiDmsSetUsbNetNum API QMI message
   eQMI_SWIDMS_GET_MTU                = 5, // SWIDMS Get MTU
   eQMI_SWIDMS_SET_MTU                = 6, // SWIDMS Set MTU
   eQMI_SWIDMS_GET_SEC_INFO           = 7, // SWIDMS Get SecureInfo
   eQMI_SWIDMS_GET_HW_WATCH_DOG       = 8, // SWIDMS Get Watch Dog
   eQMI_SWIDMS_SET_HW_WATCH_DOG       = 9, // SWIDMS Set Watch Dog
   eQMI_SWIDMS_ENUM_END
};

#endif
