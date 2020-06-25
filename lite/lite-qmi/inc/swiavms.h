/**
 * \ingroup liteqmi
 *
 * \file swiavms.h
 */

#ifndef __LITEQMI_SWIAVMS_H__
#define __LITEQMI_SWIAVMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#include "msgid.h"

#define LITEQMI_MAX_SWIOMA_STR_LEN 255
#define LITEQMI_MAX_GET_SETTINGS_AVMS_APN_STRING_LENGTH 49
#define LITEQMI_MAX_GET_SETTINGS_AVMS_UNAME_STRING_LENGTH 29
#define LITEQMI_MAX_GET_SETTINGS_AVMS_PWD_STRING_LENGTH 29

/**
 *  Structure that contains the session type request for AVMS start session command
 *  \param  sessionType[IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update. This field is 
 *                       mandatory in OMA-DM case but not necessary in LWM2M one. 
 *                       To keep compatibility, this shall be used in LWM2M without 
 *                       any specific treatment.
 */
typedef struct {
    uint8_t sessionType;
} pack_swiavms_SLQSAVMSStartSession_t;

/**
 *  Function to pack Start AVMS session command
 *  This maps to SLQSAVMSStartSession
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSStartSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
 int pack_swiavms_SLQSAVMSStartSession (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSStartSession_t  reqParam 
        );
/**
 *  Structure that contains the session type response for AVMS start session command
 * \param  SessionResponse - OMA-DM availability CHECK  for the requested item.
 *                      - 0x00000001 - Available.
 *                      - 0x00000002 - Not Available.
 *                      - 0x00000003 - Check Timed Out.
 *                      - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * \param  resultcode  - Tlv Result Code.
 */
typedef struct {
    uint32_t sessionResponse;
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSStartSession_t;

/**
 *  Function to unpack Start AVMS session response from modem
 *  This maps to SLQSAVMSStartSession
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSStartSession_t for more information
 *
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swiavms_SLQSAVMSStartSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSStartSession_t *pResponse
        );
        
/**
 *  Structure that contains the session type for AVMS Stop session command
 *  \param  sessionType
 *          - Session type
 *              - 0x01 - FOTA, to suspend FOTA session.
 *              - 0xFF - Suspend ongoing FOTA session or stop any other active  AVMS session.
 *  \param  resultcode
 *          - Tlv Result Code
 */
typedef struct {
    uint8_t sessionType;
} pack_swiavms_SLQSAVMSStopSession_t;

/**
 *  Function to pack cancel AVMS session command
 *  This maps to SLQSAVMSStopSession
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSStopSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
 int pack_swiavms_SLQSAVMSStopSession (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSStopSession_t  reqParam 
        );


/**
 *  Structure that contains the session type response for AVMS Stop session command
 *
 *  \param  resultcode  - Tlv Result Code.
 *                      - Bit to check in ParamPresenceMask - <B>2</B>
 */
typedef struct {
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSStopSession_t;

typedef unpack_swiavms_SLQSAVMSStopSession_t unpack_swiavms_SLQSAVMSStopSession_avc2_t;

/**
 *  Function to pack stop AVMS session command
 *  This maps to SLQSAVMSStopSession
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSStopSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSStopSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSStopSession_t *pResponse
        );

/**
 *  Function to pack stop AVMS session command
 *  This maps to SLQSAVMSStopSession
 *  Note: this targets WP760x as the stop response doesn't contains
 *  mandatory session_type(0x01) TLV
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSStopSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSStopSession_avc2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSStopSession_t *pResponse
        );

/**
 * This structure contain pack set settings period information.
 * @param ulMin  Min period in seconds.
 * @param ulMax Max period in seconds.
 */
typedef struct
{
    uint32_t ulMin;
    uint32_t ulMax;
}PackSwiAvmsSetSettingsPeriodInfo;


#define MAX_PACK_SWI_AVMS_SET_SETTING_APN_LENGTH 49
#define MAX_PACK_SWI_AVMS_SET_SETTING_UNAME_LENGTH 29
#define MAX_PACK_SWI_AVMS_SET_SETTING_PWD_LENGTH 29
/**
 * This structure contains pack set settings APN information. 
 * @param bAPNLength APN Length.
 * @param szAPN APN String.
 * @param bUnameLength Username Length.
 * @param szUname User Name String.
 * @param bPWDLength Password Length.
 * @param szPWD Password String.
 */
typedef struct
{
    uint8_t bAPNLength;
    uint8_t szAPN[MAX_PACK_SWI_AVMS_SET_SETTING_APN_LENGTH];
    uint8_t bUnameLength;
    uint8_t szUname[MAX_PACK_SWI_AVMS_SET_SETTING_UNAME_LENGTH];
    uint8_t bPWDLength;
    uint8_t szPWD[MAX_PACK_SWI_AVMS_SET_SETTING_PWD_LENGTH];
}PackSwiAvmsSetSettingsAPNInfo;


#define MAX_PACK_SWI_AVMS_SET_SETTING_CONNECTION_RETRY_TIMMERS 8
/**
 * This structure contains the SLQSAVMSSetSettings Polling timer to connect to AVMS server.
 *
 * @param Timers[0] - Timer 1.
 *                      - 0-20160 (min)
 *                      - 0:retry disabled
 * @param Timers[1] - Timer 2.
 *                      - 1-20160 (min)
 * @param Timers[2] - Timer 3.
 *                      - 1-20160 (min)
 * @param Timers[3] - Timer 4.
 *                      - 1-20160 (min)
 * @param Timers[4] - Timer 5.
 *                      - 1-20160 (min)
 * @param Timers[5] - Timer 6.
 *                      - 1-20160 (min)
 * @param Timers[6] - Timer 7.
 *                      - 1-20160 (min)
 * @param Timers[7] - Timer 8.
 *                      - 1-20160 (min)
 *
 */

typedef struct
{
    uint16_t  Timers[MAX_PACK_SWI_AVMS_SET_SETTING_CONNECTION_RETRY_TIMMERS];
    
}PackSwiAvmsSetSettingsConnectionRetryTimers;


/**
 *  Structure containing the AVMS settings to be set on the device
 *  This maps to structure SLQSAVMSSetSettings 
 *
 *  \param  AutoConnect[IN]
 *          - Auto Connect to AirVantage server
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *  \param  AutoReboot[IN]
 *          -  Automatic device reboot when the request is 
 *             received from AirVantage server
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *
 *  \param  PromptFwDownload[IN]
 *          - Firmware Auto Download
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *  \param  PromptFwUpdate[IN]
 *          - 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *
 *  \param  pFwAutoSDM[IN/OPTIONAL]
 *          - OMA Automatic UI Alert Response
 *                      - 0x00 - DISABLED
 *                      - 0x01 - ENABLED ACCEPT
 *                      - 0x02 - ENABLED REJECT
 *
 *  \param  pPollingTimer[IN/OPTIONAL]
 *          - Polling timer to connect to AVMS server
 *                      - 0-525600 (min)
 *                      - 0:disabled
 *
 *  \param  pConnectionRetryTimers[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsConnectionRetryTimers for more information
 *
 *  \param  pAPNInfo[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 *  \param  pNotifStore[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 *  \param  pPeriodInfo[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 * 
 */
typedef struct {    
    uint8_t  AutoConnect;
    uint8_t  AutoReboot;
    uint8_t  PromptFwDownload;
    uint8_t  PromptFwUpdate;
    uint8_t  *pFwAutoSDM;
    uint32_t *pPollingTimer;
    PackSwiAvmsSetSettingsConnectionRetryTimers *pConnectionRetryTimers;
    PackSwiAvmsSetSettingsAPNInfo *pAPNInfo;
    uint8_t *pNotifStore;
    PackSwiAvmsSetSettingsPeriodInfo *pPeriodInfo;
} pack_swiavms_SLQSAVMSSetSettings_t;
        
/**
 *  Function to pack AVMS Set settings command
 *  This maps to SLQSAVMSSetSettings
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSSetSettings (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSSetSettings_t  reqParam 
        );


/**
 *  Structure that contains the session type response for AVMS Set Settings command
 *
 *  \param  resultcode  - Tlv Result Code.
 *                      - Bit to check in ParamPresenceMask - <B>2</B>
 */
typedef struct {
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSSetSettings_t;

/**
 *  Function to unpack AVMS Set settings command
 *  This maps to SLQSAVMSSetSettings
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSSetSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSSetSettings_t *pResponse
        );


/**
 *  Structure containing the AVMS settings to be set on the device
 *  This maps to structure SLQSAVMSSetSettings_v2 (For AVC2 service)
 *
 *  \param  AutoConnect[IN]
 *          - Auto Connect to AirVantage server
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *  \param  PromptFwDownload[IN]
 *          - Firmware Auto Download
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *  \param  PromptFwUpdate[IN]
 *          - 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *
 *  \param  pFwAutoSDM[IN/OPTIONAL]
 *          - OMA Automatic UI Alert Response
 *                      - 0x00 - DISABLED
 *                      - 0x01 - ENABLED ACCEPT
 *                      - 0x02 - ENABLED REJECT
 *
 *  \param  pPollingTimer[IN/OPTIONAL]
 *          - Polling timer to connect to AVMS server
 *                      - 0-525600 (min)
 *                      - 0:disabled
 *
 *  \param  pConnectionRetryTimers[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsConnectionRetryTimers for more information
 *
 *  \param  pAPNInfo[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 *  \param  pNotifStore[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 *  \param  pPeriodInfo[IN/OPTIONAL]
 *          -  See \ref PackSwiAvmsSetSettingsAPNInfo for more information
 *
 *  \param  pAutoReboot[IN/OPTIONAL]
 *          -  Automatic device reboot when the request is 
 *             received from AirVantage server
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *  \note Setting PromptFwDownload/PromptFwUpdate as TRUE implies AutoConnect to be also TRUE
 *        even if AutoConnect was set FALSE initially. Automatic firmware download/update 
 *        selection trumps the connection setting flag.
 *       
 */
typedef struct {    
    uint8_t  AutoConnect;
    uint8_t  PromptFwDownload;
    uint8_t  PromptFwUpdate;
    uint8_t  *pFwAutoSDM;
    uint32_t *pPollingTimer;
    PackSwiAvmsSetSettingsConnectionRetryTimers *pConnectionRetryTimers;
    PackSwiAvmsSetSettingsAPNInfo *pAPNInfo;
    uint8_t *pNotifStore;
    PackSwiAvmsSetSettingsPeriodInfo *pPeriodInfo;
    uint8_t  *pAutoReboot;
} pack_swiavms_SLQSAVMSSetSettings_v2_t;
        
/**
 *  Function to pack AVMS Set settings command
 *  This maps to SLQSAVMSSetSettings_v2 (For AVC2 service)
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSSetSettings_v2_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSSetSettings_v2 (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSSetSettings_v2_t  reqParam 
        );


/**
 *  Structure that contains the session type response for AVMS Set Settings command
 *
 *  \param  resultcode  - Tlv Result Code.
 *                      - Bit to check in ParamPresenceMask - <B>2</B>
 */
typedef struct {
    uint32_t resultcode;
    swi_uint256_t    ParamPresenceMask;
} unpack_swiavms_SLQSAVMSSetSettings_v2_t;

/**
 *  Function to unpack AVMS Set settings command
 *  This maps to SLQSAVMSSetSettings_v2 (For AVC2 service)
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSetSettings_v2_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSSetSettings_v2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSSetSettings_v2_t *pResponse
        );

/**
 *  Function to pack AVMS Set settings without Auto Reobot field command
 *  This maps to SLQSAVMSSetSettingsIgnoreAutoReboot
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSSetSettings_t  reqParam 
 );

/**
 *  Function to pack AVMS Get settings command
 *  This maps to SLQSAVMSSetSettings
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSGetSettings (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
);

/**
  * This structure contains the SLQSAVMSGetSettings APN Info to connect to AVMS server.
  *
  * \param  bAPNLength - APN Length(Max 49).
  *
  * \param  szAPN - APN.
  *
  * \param  bUnameLength - User Name Length (Max 29).
  *
  * \param  szUname - User Name.
  *
  * \param  bPWDLength - Password Length (Max 29).
  *
  * \param  szPWD - Password.
  *
  */
typedef struct
{
    uint8_t    bAPNLength;
    uint8_t    *pAPN;
    uint8_t    bUnameLength;
    uint8_t    *pUname;
    uint8_t    bPWDLength;
    uint8_t    *pPWD;
}PackSwiAVMSSettingsAPNInfo;
#define MAX_AVMS_SETTINGS_RETRY_TIMER_NUMBER    8
/**
 * This structure contains the SLQSAVMSGetSettings Polling timer to connect to AVMS server.
 *
 * \param  Timers[0] - Timer 1.
 *                      - 0-20160 (min)
 *                      - 0:retry disabled
 * \param  Timers[1] - Timer 2.
 *                      - 1-20160 (min)
 * \param  Timers[2] - Timer 3.
 *                      - 1-20160 (min)
 * \param  Timers[3] - Timer 4.
 *                      - 1-20160 (min)
 * \param  Timers[4] - Timer 5.
 *                      - 1-20160 (min)
 * \param  Timers[5] - Timer 6.
 *                      - 1-20160 (min)
 * \param  Timers[6] - Timer 7.
 *                      - 1-20160 (min)
 * \param  Timers[7] - Timer 8.
 *                      - 1-20160 (min)
 *
 */
typedef struct
{
    uint16_t Timers[MAX_AVMS_SETTINGS_RETRY_TIMER_NUMBER];
}PackSwiAVMSSettingsConnectionRetryTimers;

/**
 * This structure contains the SLQSAVMSSettings Min/Max Period of an Observation.
 *
 * \param  min - Min period in seconds.
 *
 * \param  max - Max period in seconds.
 *
 */
typedef struct
{
    uint32_t min;
    uint32_t max;
}PackSwiAVMSSettingsPeriodsInfo;

/**
 *  Structure that contains the session type response for AVMS Set Settings command
 * \param  OMADMEnabled - OMA DM Enabled.
 *                          - 0 - Disabled.
 *                          - 1 - Enabled.
 *                      - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * \param AutoConnect - Auto Connect.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                    - Bit to check in ParamPresenceMask - <B>6</B>
 *
 * \param AutoReboot - Auto Reboot.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                   - Bit to check in ParamPresenceMask - <B>21</B>
 *
 * \param FwAutodownload - Firmware auto download.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                       - Bit to check in ParamPresenceMask - <B>3</B>
 *
 * \param FwAutoUpdate - Firmware auto update.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                     - Bit to check in ParamPresenceMask - <B>4</B>
 *
 * \param FwAutoSDM - OMA Automatic UI Alert Response.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                  - Bit to check in ParamPresenceMask - <B>5</B>
 *
 * \param pPollingTimer - Polling timer to connect to AVMS server.
 *                         - 0-525600 (min)
 *                         - 0:disabled 
 *                      - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * \param pConnectionRetryTimers - Connection Retry timers.
 *                         - See \ref PackSwiAVMSSettingsConnectionRetryTimers for more information.
 *                         - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * \param pAPNInfo - APN Information.
 *                         - See \ref PackSwiAVMSSettingsAPNInfo for more information.
 *                         - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * \param pNotificationStore - Notification Storing When Disabled or Offline.
 *                               - 0 - Disabled.
 *                               - 1 - Enabled.
 *                           - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * \param pPeroidsInfo - Min and Max Period of an Observation.
 *                         - See \ref PackSwiAVMSSettingsPeriodsInfo for more information.
 *                         - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * \param  resultcode  - Tlv Result Code.
 */
typedef struct {
    uint32_t          OMADMEnabled;
    uint8_t           AutoConnect;
    uint8_t           AutoReboot;
    uint8_t           FwPromptdownload;
    uint8_t           FwPromptUpdate;
    uint8_t           FwAutoSDM;
    uint32_t             *pPollingTimer;
    PackSwiAVMSSettingsConnectionRetryTimers *pConnectionRetryTimers;
    PackSwiAVMSSettingsAPNInfo *pAPNInfo;
    uint8_t              *pNotificationStore;
    PackSwiAVMSSettingsPeriodsInfo  *pPeroidsInfo;
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSGetSettings_t;

/**
 *  Function to unpack AVMS set settings command
 *  This maps to SLQSAVMSGetSettings
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSGetSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSGetSettings_t *pResponse
        );

/**
 *  Structure that contains the response for AVMS Get Settings command
 *  It maps with SLQSAVMSSetSettings_v2 (For AVC2 service)
 *
 * \param  OMADMEnabled - OMA DM Enabled.
 *                          - 0 - Disabled.
 *                          - 1 - Enabled.
 *                      - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * \param AutoConnect - Auto Connect.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                    - Bit to check in ParamPresenceMask - <B>6</B>
 *
 * \param FwPromptdownload - Firmware Prompt download.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                       - Bit to check in ParamPresenceMask - <B>3</B>
 *
 * \param FwPromptUpdate - Firmware Prompt update.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                     - Bit to check in ParamPresenceMask - <B>4</B>
 *
 * \param FwAutoSDM - OMA Automatic UI Alert Response.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                  - Bit to check in ParamPresenceMask - <B>5</B>
 *
 * \param pPollingTimer[OPTIONAL] - Polling timer to connect to AVMS server.
 *                         - 0-525600 (min)
 *                         - 0:disabled 
 *                               - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * \param pConnectionRetryTimers[OPTIONAL] - Connection Retry timers.
 *                         - See \ref PackSwiAVMSSettingsConnectionRetryTimers for more information.
 *                         - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * \param pAPNInfo[OPTIONAL] - APN Information.
 *                         - See \ref PackSwiAVMSSettingsAPNInfo for more information.
 *                         - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * \param pNotificationStore[OPTIONAL] - Notification Storing When Disabled or Offline.
 *                               - 0 - Disabled.
 *                               - 1 - Enabled.
 *                                   - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * \param pPeroidsInfo[OPTIONAL] - Min and Max Period of an Observation.
 *                         - See \ref PackSwiAVMSSettingsPeriodsInfo for more information.
 *                         - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * \param pAutoReboot[OPTIONAL] - Auto Reboot.
 *                      - 0 - Disabled.
 *                      - 1 - Enabled.
 *                             - Bit to check in ParamPresenceMask - <B>21</B>
 *
 * \param  resultcode  - Tlv Result Code.
 */
typedef struct {
    uint32_t          OMADMEnabled;
    uint8_t           AutoConnect;
    uint8_t           FwPromptdownload;
    uint8_t           FwPromptUpdate;
    uint8_t           FwAutoSDM;
    uint32_t          *pPollingTimer;
    PackSwiAVMSSettingsConnectionRetryTimers *pConnectionRetryTimers;
    PackSwiAVMSSettingsAPNInfo *pAPNInfo;
    uint8_t                     *pNotificationStore;
    PackSwiAVMSSettingsPeriodsInfo  *pPeroidsInfo;    
    uint8_t         *pAutoReboot;
    uint32_t        resultcode;
    swi_uint256_t   ParamPresenceMask;
} unpack_swiavms_SLQSAVMSGetSettings_v2_t;

/**
 *  Function to pack AVMS Get settings command
 *  This maps to SLQSAVMSSetSettings_v2 (For AVC2 service)
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSGetSettings_v2 (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
);

/**
 *  Function to unpack AVMS set settings command
 *  This maps to SLQSAVMSGetSettings_v2 (For AVC2 service)
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAVMSGetSettings_v2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSGetSettings_v2_t *pResponse
        );

/**
 *  Structure containing the AVMS selection
 *  
 *  \param  selection[IN]
 *          - User Selection
 *              - 0x01 - Accept
 *              - 0x02 - Reject
 *              - 0x03 - Defer
 *
 *  \param  pDeferTime[IN/OPTIONAL]
 *          - Defer time in minutes. A value of 0 will 
 *            cause the prompt to be resent immediately.
 *
 *  \param  pClientPerformOperationFlag[IN/OPTIONAL]
 *          - Client operation flag after accept.
 *              - 0: if modem performs the operation (download or update)
 *              - 1: if client performs the operation (download or update)
 *
 *  \param  pPackageID[IN/OPTIONAL]
 *          - Package ID.
 *
 *  \param  pRejectReason[IN/OPTIONAL]
 *          - Reject Reason.
 *
 */
typedef struct {
    uint8_t  selection;
    uint32_t *pDeferTime;
    uint8_t *pClientPerformOperationFlag;
    uint8_t *pPackageID;
    uint16_t *pRejectReason;
} pack_swiavms_SLQSAVMSSendSelection_t;
        
/**
 *  Function to pack AVMS send selection command
 *  This maps to SLQSAVMSSendSelection
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *        
 *  \param[in]  reqParam
 *              - See \ref pack_swiavms_SLQSAVMSSendSelection_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSSendSelection (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiavms_SLQSAVMSSendSelection_t  reqParam 
        );


/**
 *  Structure that contains the session type response for AVMS Selection command
 *
 *  \param  resultcode  - Tlv Result Code.
 *                      - Bit to check in ParamPresenceMask - <B>2</B>
 */
typedef struct {
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSSendSelection_t;

/**
 *  Function to unpack AVMS send selection command
 *  This maps to SLQSAVMSSendSelection
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSendSelection_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swiavms_SLQSAVMSSendSelection(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAVMSSendSelection_t *pResponse
        );
       
/**
 *  Function to pack AVMS set event report command
 *  This maps to SLQSAVMSSetEventReport
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAvmsSetEventReport (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );


/**
 *  Structure that contains the session type response for AVMS set event report command
 *
 *  \param  resultcode  - Tlv Result Code.
 *                      - Bit to check in ParamPresenceMask - <B>2</B>
 */
typedef struct {
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAvmsSetEventReport_t;

/**
 *  Function to unpack AVMS set event report command
 *  This maps to SLQSAVMSSetEventReport
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAvmsSetEventReport_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swiavms_SLQSAvmsSetEventReport(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiavms_SLQSAvmsSetEventReport_t *pResponse
        );

/**
 *  Function to pack AVMS get session info command
 *  This maps to SLQSAVMSSessionGetInfo
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 2 seconds
 */
int pack_swiavms_SLQSAVMSSessionGetInfo (
    pack_qmi_t *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

#define MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_VERSION_LENGTH 128
#define MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_NAME_LENGTH 128
#define MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_DESC_LENGTH 1024

/**
 * This structure contains unpack event report binary update session information parameters.
 * @param bBinaryType : Type
 *                      - 1 - Firmware.
 *                      - 2 - User App.
 *                      - 3 - Legato Framework.
 * @param bStat : State 
 *                      - 0x01 - No binary update available.
 *                      - 0x02 - Query binary Download.
 *                      - 0x03 - Binary Downloading.
 *                      - 0x04 - Binary downloaded.
 *                      - 0x05 -Query Binary Update.
 *                      - 0x06 - Binary updating.
 *                      - 0x07 - Binary updated.
 * @param bUserInputRequest:  Bit mask of available user inputs.
 *                      - 0x00 - No user input required. Informational indication.
 *                      - 0x01 - Accept.
 *                      - 0x02 - Reject.
 * @param wUserInputTimeout: Timeout for user input in minutes. A value of 0 means no time-out.
 * @param ulPkgDownloadSize: The size (in bytes) of the update package
 * @param ulPkgDownloadComplete:  The number of bytes being downloaded.
 *                                For downloading state, this value shall be > 0 and 
 *                                incremented toward the pkg_dload_size. 
 *                                For other states, the value shall be 0 as it is meaningless.
 * @param wUpdateCompeteStatus: Result code.This field should be looked at only 
                                when the AVMS session is complete.
 * @param bSerity: Serity.
 *                      - 0x01 - Mandatory.
 *                      - 0x02 - Optional.
 * @param wVersionLength: Length of FW Version string in bytes.
 * @param szVersion: FW Version string in ASCII.
 * @param wNameLength: Length Package Name string in bytes.
 * @param szName: Package Name in UCS2.
 * @param wDescriptionLength: Length of description in bytes.
 * @param szDescription: Description of Update Package in USC2.
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bBinaryType;
    uint8_t bState;
    uint8_t bUserInputRequest;
    uint16_t wUserInputTimeout;
    uint32_t ulPkgDownloadSize;
    uint32_t ulPkgDownloadComplete;
    uint16_t wUpdateCompeteStatus;
    uint8_t bSerity;
    uint16_t wVersionLength;
    uint8_t szVersion[MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_VERSION_LENGTH];
    uint16_t wNameLength;
    uint8_t szName[MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_NAME_LENGTH];
    uint16_t wDescriptionLength;
    uint8_t szDescription[MAX_PACK_SWI_AVMS_SESSIONGETINFO_BINARYUPDATESESSIONINFO_DESC_LENGTH];
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportBinaryUpdateSessionInfo;

#define MAX_PACK_SWI_AVMS_SESSIONGETINFO_CONFIG_ALERT_MSG_LENGTH 200

/**
 * This structure contains unpack event report configure parameters.
 * @param bState: State.                 
 *                  - 0x01 - AVMS Read Request.
 *                  - 0x02 - AVMS Change Request.
 *                  - 0x03 - AVMS Config Complete.
 * @param bUserInputRequest: Bit mask of available user inputs.
 *                  - 0x00 - No user input required. Informational indication.
 *                  - 0x01 - Accept.
 *                  - 0x02 - Reject.
 * @param wUserInputTimeout: Timeout for user input in seconds. A value of 0 means no time-out
 * @param wAlertMsgLength: Length of Alert message string in bytes.
 * @param szAlertMsg: Alert message in UCS2.
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bState;
    uint8_t bUserInputRequest;
    uint16_t wUserInputTimeout;
    uint16_t wAlertMsgLength;
    uint8_t szAlertMsg[MAX_PACK_SWI_AVMS_SESSIONGETINFO_CONFIG_ALERT_MSG_LENGTH];
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportConfig;

/**
 * This structure contains unpack event report notification parameters.
 * @param bNotification: Notification.
 *                  - 0x14 - Module starts sending data to server.
 *                  - 0x15 - Authentication with the server.
 *                  - 0x16 - session with the server is ended.
 * @param wSessionStatus: This field will set to the session status for notifications 
 *                        that occur at the end of a session, zero for all other notifications.
 *                          - 0x0000: Successful: Session succeeded
 *                          - 0x0001: Break: Session succeeded
 *                          - 0x0002: Large Object Handled: Session succeeded
 *                          - 0x0003: No status: Session succeeded
 *                          - 0x0004: No more commands: Session succeeded
 *                          - 0x0005: User cancel: Session cancelled
 *                          - 0x0100-0x01FF: General errors
 *                          - 0x0200-0x02FF: Syncml errors
 *                          - 0x0300-0x03FF: Authentication errors
 *                          - 0x0400-0x04FF: Protocol errors
 *                          - 0x0500-0x05FF: Tree errors (DM Only)
 *                          - 0x0600-0x06FF: Not applicable
 *                          - 0x0700-0x07FF: Trigger errors
 *                          - 0x0800-0x08FF: FUMO errors
 *                          - 0x0900-0x09FF: Communication errors
 *                          - 0x0A00-0x0AFF: Parsing errors
 *                          - 0x0B00-0x0CFF: Not applicable
 *                          - 0x7F00-0x7F12:Insignia errors
 *                          - 0x7F13: Illegal text: Text received contains illegal characters
 *                          - 0x7F14: Download failure: Failed to download FOTA image
 *                          - 0x7F15: Empty session: Session ran successfully, but no information wasa updated
 *                          - 0x7F16: Factory reset successful: Factory reset succeeded
 *                          - 0x7F17: Factory reset fail: Factory reset failed
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bNotification;
    uint16_t wSessionStatus;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportNotification;

/**
 * This structure contains unpack event report connection request parameters.
 * @param bUserInputRequest: Bit mask of available user inputs.
 *                              - 0x00 - No user input required. Informational indication.
 *                              - 0x01 - Accept.
 *                              - 0x02 - Reject.
 * @param bUserInputRequest: Timeout for user input in minutes. A value of 0 means no time-out.
 *
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 */
typedef struct
{
    uint8_t bUserInputRequest;
    uint16_t wUserInputTimeout;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportConnectionRequest;

/**
 * This structure contains unpack event report WAMS parameter change parameters.
 * @param wWamsChangeMask: Mask of WAMS parameters changed. 
 *                          - By default set to 0xFF for all changes.
 *                          - 0x01 - device_login
 *                          - 0x02 - device_MD5_key
 *                          - 0x04 - server_login
 *                          - 0x08 - server_MD5_key
 *                          - 0x10 - server_URL
 *                          - 0x20 - Nonce
 *                          - 0x40 - Application key
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint16_t wWamsChangeMask;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportWAMSParamChange;

/**
 * This structure contains unpack event report package ID parameters.
 * @param bPackageID: Package ID of the application binary that this 
 *                    AVMS_EVENT_ID notification is for.
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bPackageID;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportPackageID;

/**
 * This structure contains unpack event report registration status parameters.
 * @param bRegStatus: LWM2M Registration status.
 *                      - 0: Need Bootstrap.
 *                      - 1: Bootstrap made.
 *                      - 2: Register made.
 *                       -3: Update made.
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bRegStatus;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportRegStatus;

/**
 * This structure contains unpack evnet report data session status parameters.
 * @param bType: Notification type.
 *                      - 0: Data session closed.
 *                      - 1: Data session activated.
 *                      - 2: Register made.
 *                       -3: Data session error.
 * @param wErrorCode: LWM2M Session error code.
 *                      - 0x0000: none
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bType;
    uint16_t wErrorCode;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportDataSessionStatus;

/**
 * This structure contains unpack event report session type parameters.
 * @param bType: Session Type.
 *                      - 0: Bootstrap session.
 *                      - 1. DM session
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint8_t bType;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportSessionType;

/**
 * This structure contains unpack event report HTTP status parameters.
 * @param wHTTPStatus: See RFC 7231.
 *                      - 100  - Continue
 *                      - 101  - Switching Protocols
 *                      - 200  - OK
 *                      - 201  - Created
 *                      - 202  - Accepted
 *                      - 203  - Non-Authoritative Information
 *                      - 204  - No Content
 *                      - 205  - Reset Content
 *                      - 206  - Partial Content
 *                      - 300  - Multiple Choices
 *                      - 301  - Moved Permanently
 *                      - 302  - Found
 *                      - 303  - See Other
 *                      - 304  - Not Modified
 *                      - 305  - Use Proxy
 *                      - 307  - Temporary Redirect
 *                      - 400  - Bad Request
 *                      - 401  - Unauthorized
 *                      - 402  - Payment Required
 *                      - 403  - Forbidden
 *                      - 404  - Not Found
 *                      - 405  - Method Not Allowed
 *                      - 406  - Not Acceptable
 *                      - 407  - Proxy Authentication Required
 *                      - 408  - Request Timeout
 *                      - 409  - Conflict
 *                      - 410  - Gone
 *                      - 411  - Length Required
 *                      - 412  - Precondition Failed
 *                      - 413  - Payload Too Large
 *                      - 414  - URI Too Long
 *                      - 415  - Unsupported Media Type
 *                      - 416  - Range Not Satisfiable
 *                      - 417  - Expectation Failed
 *                      - 426  - Upgrade Required
 *                      - 500  - Internal Server Error
 *                      - 501  - Not Implemented
 *                      - 502  - Bad Gateway
 *                      - 503  - Service Unavailable
 *                      - 504  - Gateway Timeout
 *                      - 505  - HTTP Version Not Supported
 * @param TlvPresent: Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 */
typedef struct
{
    uint16_t wHTTPStatus;
    uint8_t TlvPresent;
}UnpackSwiAvmsEventReportHTTPStatus;

/**
 *  Structure that contains the session type response for AVMS get session info command
 *
 *  \param  pBinaryUpdateSessionInfo[OUT]
 *            - See \ref UnpackSwiAvmsEventReportBinaryUpdateSessionInfo for more information
 *            - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pConfig[OUT]
 *            - See \ref UnpackSwiAvmsEventReportConfig for more information
 *            - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pNotification[OUT]
 *            - See \ref UnpackSwiAvmsEventReportNotification for more information
 *            - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pPackageID[OUT]
 *            - See \ref UnpackSwiAvmsEventReportPackageID for more information
 *            - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  resultcode  - Tlv Result Code.
 */
typedef struct
{
    /* UnpackSwiAvmsEventReportBinaryUpdateSessionInfo response parameters. NULL Invalid data.*/
     UnpackSwiAvmsEventReportBinaryUpdateSessionInfo *pBinaryUpdateSessionInfo;
    
    /* UnpackSwiAvmsEventReportConfig response parameters. NULL Invalid data.*/
    UnpackSwiAvmsEventReportConfig *pConfig;

    /* UnpackSwiAvmsEventReportNotification response parameters. NULL Invalid data. */
    UnpackSwiAvmsEventReportNotification *pNotification;

    /* UnpackSwiAvmsEventReportPackageID response parameters. NULL Invalid data. */
    UnpackSwiAvmsEventReportPackageID *pPackageID;
    
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSSessionGetInfo_t;

/**
 *  Function to unpack AVMS event report Indication command
 *  This maps to SLQSAVMSSetEventReport
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSSessionGetInfo_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */       
int unpack_swiavms_SLQSAVMSSessionGetInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swiavms_SLQSAVMSSessionGetInfo_t *pResponse
);



/**
 *  Structure that contains the session type response for AVMS event report indication command
 *
 *  \param  pBinaryUpdateSessionInfo[OUT]
 *            - See \ref UnpackSwiAvmsEventReportBinaryUpdateSessionInfo for more information
 *            - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pConfig[OUT]
 *            - See \ref UnpackSwiAvmsEventReportConfig for more information
 *            - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pNotification[OUT]
 *            - See \ref UnpackSwiAvmsEventReportNotification for more information
 *            - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pPackageID[OUT]
 *            - See \ref UnpackSwiAvmsEventReportPackageID for more information
 *            - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  pConnectionRequest[OUT]
 *            - See \ref UnpackSwiAvmsEventReportConnectionRequest for more information
 *            - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pWAMSParaChanged[OUT]
 *            - See \ref UnpackSwiAvmsEventReportWAMSParamChange for more information
 *            - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  pRegStatus[OUT]
 *            - See \ref UnpackSwiAvmsEventReportRegStatus for more information
 *            - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  pDataSessionStatus[OUT]
 *            - See \ref UnpackSwiAvmsEventReportDataSessionStatus for more information
 *            - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  pSessionType[OUT]
 *            - See \ref UnpackSwiAvmsEventReportSessionType for more information
 *            - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  pHTTPStatus[OUT]
 *            - See \ref UnpackSwiAvmsEventReportHTTPStatus for more information
 *            - Bit to check in ParamPresenceMask - <B>25</B>
 *
 * \param  resultcode  - Tlv Result Code.
 */
typedef struct
{
    /* UnpackSwiAvmsEventReportBinaryUpdateSessionInfo response parameters */
     UnpackSwiAvmsEventReportBinaryUpdateSessionInfo *pBinaryUpdateSessionInfo;
    
    /* UnpackSwiAvmsEventReportConfig response parameters */
    UnpackSwiAvmsEventReportConfig *pConfig;

    /* UnpackSwiAvmsEventReportNotification response parameters */
    UnpackSwiAvmsEventReportNotification *pNotification;

    /* UnpackSwiAvmsEventReportPackageID response parameters */
    UnpackSwiAvmsEventReportPackageID *pPackageID;

    /* UnpackSwiAvmsEventReportConnectionRequest response parameters */
    UnpackSwiAvmsEventReportConnectionRequest *pConnectionRequest;

    /* UnpackSwiAvmsEventReportWAMSParamChange response parameters */
    UnpackSwiAvmsEventReportWAMSParamChange *pWAMSParaChanged;

    /* UnpackSwiAvmsEventReportRegStatus response parameters */
    UnpackSwiAvmsEventReportRegStatus *pRegStatus;

    /* UnpackSwiAvmsEventReportDataSessionStatus response parameters */
    UnpackSwiAvmsEventReportDataSessionStatus *pDataSessionStatus;

    /* UnpackSwiAvmsEventReportSessionType response parameters */
    UnpackSwiAvmsEventReportSessionType *pSessionType;

    /* UnpackSwiAvmsEventReportHTTPStatus response parameters */
    UnpackSwiAvmsEventReportHTTPStatus *pHTTPStatus;
    
    uint32_t resultcode;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiavms_SLQSAVMSEventReportInd_t;

/**
 *  Function to unpack AVMS event report Indication command
 *  This maps to SLQSAVMSSetEventReport
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pResponse
 *               - See \ref unpack_swiavms_SLQSAVMSEventReportInd_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */       
int unpack_swiavms_SLQSAVMSEventReportInd(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swiavms_SLQSAVMSEventReportInd_t *pResponse
);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif //__LITEQMI_SWIAVMS_H__
