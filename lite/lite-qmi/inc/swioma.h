/**
 * \ingroup liteqmi
 *
 * \file swioma.h
 */

#ifndef __LITEQMI_SWIOMA_H__
#define __LITEQMI_SWIOMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#include "msgid.h"

#define LITEQMI_MAX_SWIOMA_STR_LEN 255

/**
 *  Structure that contains the session type for OMA start session command
 *  \param  sessionType[IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0x02 - DM, to check availability of DM Update
 *              - 0x03 - PRL,  to check availability of PRL Update 
 */
typedef struct {
    uint32_t sessionType;
} pack_swioma_SLQSOMADMStartSession_t;

/**
 *  Function to pack Start OMA-DM session command
 *  This maps to SLQSOMADMStartSession2
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param reqParam [in]
 *              - See \ref pack_swioma_SLQSOMADMStartSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMStartSession (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMStartSession_t  reqParam 
        );

/**
 *  Structure that contains the responses for OMA start session command
 *  \param  FwAvailability [out]
 *          - OMA-DM CHECK FW Available
 *          - Values
 *            - 0x00000001 - FW Available. For CIDC and CIPRL, this value
 *                           will be returned by the modem. CIDC and CIPRL
 *                           are asynchronous OMADM sessions.
 *            - 0x00000002 - FW Not Available
 *            - 0x00000003 - FW Check Timed Out
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t FwAvailability;
    swi_uint256_t  ParamPresenceMask;
} unpack_swioma_SLQSOMADMStartSession_t;

/**
 *  Function to unpack Start OMA-DM session response from modem
 *  This maps to SLQSOMADMStartSession2
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *               - See \ref unpack_swioma_SLQSOMADMStartSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMStartSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMStartSession_t *pOutput
        );

/**
 *  Structure that contains the session type for OMA cancel session command
 *  \param sessionType [in]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0xFF - Cancel any active OMADM session
 */
typedef struct {
    uint32_t sessionType;
} pack_swioma_SLQSOMADMCancelSession_t;

/**
 *  Function to pack cancel OMA-DM session command
 *  This maps to SLQSOMADMCancelSession
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On output, number of bytes actually packed
 *
 *  \param reqParam [in]
 *              - See \ref pack_swioma_SLQSOMADMCancelSession_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMCancelSession (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMCancelSession_t  reqParam 
        );

 typedef unpack_result_t  unpack_swioma_SLQSOMADMCancelSession_t;

/**
 *  Function to pack cancel OMA-DM session command
 *  This maps to SLQSOMADMCancelSession
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param  pOutput [out]
 *              - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMCancelSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMCancelSession_t *pOutput
        );

/**
 *  Function to pack command to retrieve the OMA-DM settings from the device.
 *  This maps to SLQSOMADMGetSettings2
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On output, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */		
int pack_swioma_SLQSOMADMGetSettings (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        ); 

/**
 *  Structure containing the OMA DM settings retrieved from the device
 *
 *  \param OMADMEnabled [out]
 *               - Optional 4 byte parameter indicating OMADM service enabled
 *                  - 0x00000001 - Client-initiated device configuration
 *                  - 0x00000002 - Network-initiated device configuration
 *                  - 0x00000010 - Client-initiated FUMO
 *                  - 0x00000020 - Network-initiated FUMO
 *               - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param FOTAdownload [out]
 *               - Optional 1 Byte parameter indicating support for FOTA Automatic
 *                 download
 *                  - 0x00 - Host permission required before downloading
 *                  - 0x01 - Automatically start downloading, no host permission required
 *                  - 0x02 - Automatically start downloading, while not roaming
 *                  - 0x03 - Automatically reject download
 *                  - 0x04 - Automatically reject download with “Enterprise Reject Policy
 *               - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param FOTAUpdate [out]
 *               - Optional 1 byte parameter indicating FOTA Automatic update
 *                  - 0x00 - User permission required before updating firmware
 *                  - 0x01 - No user permission required before updating firmware
 *                  - 0x02 - User permission required, auto update on power up
 *               - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param Autosdm [out]
 *               - Optional 1 byte parameter indicating OMA Automatic UI Alert
 *                 Response
 *                  - 0x00 - Disabled
 *                  - 0x01 - Enabled Accept
 *                  - 0x02 - Enabled Reject
 *               - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param FwAutoCheck [out]
 *               - Optional 1 byte parameter indicating OMA Automatic Check for
 *                 Firmware Update on Power-Up
 *                 Response
 *                  - 0x00 - Disabled
 *                  - 0x01 - Enabled
 *               - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t OMADMEnabled;
    uint8_t  FOTAdownload;
    uint8_t  FOTAUpdate;
    uint8_t  Autosdm;
    uint8_t  FwAutoCheck;
    swi_uint256_t  ParamPresenceMask;
} unpack_swioma_SLQSOMADMGetSettings_t;

/**
 *  Function to unpack OMA-DM get settings response from modem
 *  This maps to SLQSOMADMGetSettings2
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *               - See \ref unpack_swioma_SLQSOMADMGetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMGetSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMGetSettings_t *pOutput
        );

/**
 *  Structure containing the OMA DM settings to be set on the device
 *  This maps to structure SLQSOMADMSettingsReqParams3
 *
 *  \param  FOTAdownload
 *          - 1 Byte parameter indicating support for FOTA Automatic download
 *              - 0x00 - Firmware autodownload FALSE
 *              - 0x01 - Firmware autodownload TRUE
 *
 *  \param  FOTAUpdate
 *          - 1 byte parameter indicating FOTA Automatic update
 *              - 0x00 - Firmware autoupdate FALSE
 *              - 0x01 - Firmware autoupdate TRUE
 *
 *  \param  pAutosdm
 *          - Optional 1 byte parameter indicating OMA Automatic UI
 *            Alert Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled Accept
 *              - 0x02 - Enabled Reject
 *
 *  \param  pFwAutoCheck
 *          - Optional 1 byte parameter indicating OMA Automatic Check for
 *            Firmware Update on Power-Up
 *            Response
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 * 
 */
typedef struct {
    uint8_t  FOTAdownload;
    uint8_t  FOTAUpdate;
    uint8_t  *pAutosdm;
    uint8_t  *pFwAutoCheck;
} pack_swioma_SLQSOMADMSetSettings_t;	

/**
 *  Function to pack OMA-DM set settings command
 *  This maps to SLQSOMADMSetSettings3
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param reqParam [in]
 *              - See \ref pack_swioma_SLQSOMADMSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
int pack_swioma_SLQSOMADMSetSettings (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMSetSettings_t  reqParam 
        );

typedef unpack_result_t  unpack_swioma_SLQSOMADMSetSettings_t;

/**
 *  Function to unpack OMA-DM set settings command
 *  This maps to SLQSOMADMSetSettings3
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *              - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMSetSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMSetSettings_t *pOutput
        );

/**
 *  Structure containing the OMA DM selection
 *  
 *  \param selection [in]
 *              - OMA-DM NIA Selection
 *                 - 0x01 - Accept
 *                 - 0x02 - Reject
 *                 - 0x03 - Defer
 *
 *  \param pDeferTime [in]
 *              - Defer time in minutes. A value of 0 will cause the prompt
 *                to be resent immediately.
 *              - This TLV is mandatory if selection is set to 0x03.
 *
 *  \param pRejectReason [in]
 *              - Reject Reason
 *              - This TLV is processed if selection is set to 0x02.
 *                If it is not present, the reject reason 0 is used as default.
 */
typedef struct {
    uint32_t  selection;
    uint32_t  *pDeferTime;
    uint32_t  *pRejectReason;
} pack_swioma_SLQSOMADMSendSelection_t;

/**
 *  Function to pack OMA-DM send selection command
 *  This maps to SLQSOMADMSendSelection2
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param reqParam [in]
 *              - See \ref pack_swioma_SLQSOMADMSendSelection_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
int pack_swioma_SLQSOMADMSendSelection (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMSendSelection_t  reqParam 
        );

typedef unpack_result_t  unpack_swioma_SLQSOMADMSendSelection_t;

/**
 *  Function to unpack OMA-DM send selection command
 *  This maps to SLQSOMADMSendSelection2
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *              - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMSendSelection(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMSendSelection_t *pOutput
        );

/**
 *  Structure that contains the session type for OMA get session info command
 *  \param SessionType [in]
 *              - Session type
 *                  - 0x01 - FOTA
 *                  - 0xFF - Any active OMADM session. If no active sessions are available, then
 *                           previous OMADM session info is returned
 */
typedef struct {
    uint32_t SessionType;
} pack_swioma_SLQSOMADMGetSessionInfo_t;

/**
 *  Function to pack QMI command to return information related to the current 
 * (or previous if no session is active) OMA-DM session.
 *  This maps to SLQSOMADMGetSessionInfo
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param reqParam [in]
 *              - See \ref pack_swioma_SLQSOMADMGetSessionInfo_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMGetSessionInfo (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMGetSessionInfo_t  reqParam 
        );

/**
 *  Structure that contains the session type for OMA get session info unpack command
 *  Also used as input parameter to specify the size of variable parameters.
 *  (ref. notes)
 *
 *  \param  Status
 *          - 1 Byte parameter indicating status
 *              - 0x01 - No Firmware available
 *              - 0x02 - Query Firmware Download
 *              - 0x03 - Firmware Downloading
 *              - 0x04 - Firmware Downloaded
 *              - 0x05 - Query Firmware Update
 *              - 0x06 - Firmware Updating
 *              - 0x07 - Firmware Updated
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  UpdateCompleteStatus
 *          - 2 byte parameter indicating Update Complete Status
 *              - See qaGobiApiTableSwiOMADMUpdateCompleteStatus.h
 *                Update Complete Status
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  Severity
 *          - 1 byte parameter indicating severity
 *              - 0x01 - Mandatory
 *              - 0x02 - Optional
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  SourceLength
 *          - 2 byte parameter indicating Length of Vendor Name String
 *            in Bytes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  Source
 *          - Variable length parameter indicating Vendor Name in ASCII
 *          - See \ref LITEQMI_MAX_SWIOMA_STR_LEN for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  PkgNameLength
 *          - 2 byte parameter indicating Length of Package Name String
 *            in Bytes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  PkgName
 *          - Variable length parameter indicating Package Name in ASCII
 *          - See \ref LITEQMI_MAX_SWIOMA_STR_LEN for more information 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  PkgDescLength
 *          - 2 byte parameter indicating Length of Package Description String
 *            in Bytes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  PkgDescription
 *          - Variable length parameter indicating Package Description in ASCII
 *          - See \ref LITEQMI_MAX_SWIOMA_STR_LEN for more information 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  DateLength
 *          - 2 byte parameter indicating Length of Package Description String
 *            in Bytes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  Date
 *          - Variable length parameter indicating Package Description in ASCII
 *          - See \ref LITEQMI_MAX_SWIOMA_STR_LEN for more information 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  TimeLength
 *          - 2 byte parameter indicating Length of Time String in Bytes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  Time
 *          - Variable length parameter indicating Time String in ASCII
 *          - See \ref LITEQMI_MAX_SWIOMA_STR_LEN for more information 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  SessionType
 *          - 1 byte parameter reflects the last session started for Sprint
 *              - 0x00 - No session since boot
 *              - 0x01 - Sprint CI-DC Session
 *              - 0x02 - Sprint CI-PRL Session
 *              - 0x03 - Sprint CI-FUMO Session
 *              - 0x04 - Sprint HFA-DC Session
 *              - 0x05 - Sprint HFA-PRL Session
 *              - 0x06 - Sprint HFA-FUMO Session
 *              - 0x07 - Sprint NI Session
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  SessionState
 *          - 1 byte parameter indicating session state
 *              - 0x01 - idle
 *              - 0x02 - active
 *              - 0x03 - pending
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  RetryCount
 *          - 1 byte parameter indicating retries left count
 *              - valid values 0 to 6
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t   Status;
    uint16_t  UpdateCompleteStatus;
    uint8_t   Severity;
    uint16_t  SourceLength;
    uint8_t   Source[LITEQMI_MAX_SWIOMA_STR_LEN];
    uint16_t  PkgNameLength;
    uint8_t   PkgName[LITEQMI_MAX_SWIOMA_STR_LEN];
    uint16_t  PkgDescLength;
    uint8_t   PkgDescription[LITEQMI_MAX_SWIOMA_STR_LEN];
    uint16_t  DateLength;
    uint8_t   Date[LITEQMI_MAX_SWIOMA_STR_LEN];
    uint16_t  TimeLength;
    uint8_t   Time[LITEQMI_MAX_SWIOMA_STR_LEN];
    uint8_t   SessionType;
    uint8_t   SessionState;
    uint16_t   RetryCount;
    swi_uint256_t  ParamPresenceMask;
} unpack_swioma_SLQSOMADMGetSessionInfo_t;

/**
 *  Function to unpack information related to the current 
 * (or previous if no session is active) OMA-DM session.
 *  This maps to SLQSOMADMGetSessionInfo 
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *               - See \ref unpack_swioma_SLQSOMADMGetSessionInfo_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMGetSessionInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMGetSessionInfo_t *pOutput
        );

/**
 *  Function to pack QMI command to enable the SWIOMADM network-initiated alert callback function. 
 *  This maps to SetSLQSOMADMAlertCallback
 *
 *  \param pCtx [out]
 *               - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [in,out]
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param pLen [in,out]
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMAlertCallback (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

typedef unpack_result_t  unpack_swioma_SLQSOMADMAlertCallback_t;

/**
 *  Function to unpack response of QMI command to enable the SWIOMADM network-initiated alert callback function. 
 *  This maps to SetSLQSOMADMAlertCallback
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *              - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   
 *          - Please use eQMI_SWIOMA_EVENT_IND indication to identify this event from SWIOMA service read function
 */
int unpack_swioma_SLQSOMADMAlertCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMAlertCallback_t *pOutput
        );
/**
 * This structure will hold the SwiOmaDmFota session parameters information.
 *
 *  \param  state
 *          - 0x01 - No Firmware available
 *          - 0x02 - Query Firmware Download
 *          - 0x03 - Firmware Downloading
 *          - 0x04 - Firmware downloaded
 *          - 0x05 - Query Firmware Update
 *          - 0x06 - Firmware updating
 *          - 0x07 - Firmware updated
 *
 *  \param  userInputReq - Bit mask of available user inputs
 *          - 0x00 - No user input required.Informational indication
 *          - 0x01 - Accept
 *          - 0x02 - Reject
 *
 *  \param  userInputTimeout
 *          - Timeout for user input in minutes.
 *             A value of 0 means no time-out
 *
 *  \param  fwdloadsize
 *          - The size (in bytes) of the firmware update package
 *
 *  \param  fwloadComplete
 *          - The number of bytes downloaded. Need to determine how
 *            often to send this message for progress bar notification.
 *            Every 500ms or 5% increment.
 *
 *  \param  updateCompleteStatus
 *          - This field should be looked at only when the OMADM session is complete.
 *          - See \ref qaGobiApiTableSwiOMADMUpdateCompleteStatus.h for update complete status.
 *
 *  \param  severity
 *          - 0x01 - Mandatory
 *          - 0x02 - Optional
 *
 *  \param  versionlength
 *          - Length of FW Version string in bytes
 *
 *  \param  version
 *          - FW Version string in ASCII (Max 256 characters)
 *
 *  \param  namelength
 *          - Length Package Name string in bytes
 *
 *  \param  package_name
 *          - Package Name in UCS2 (Max 256 characters)
 *
 *  \param  descriptionlength
 *          - Length of description in bytes
 *
 *  \param  description
 *          - Description of Update Package in USC2 (Max 256 characters)
 *
 *  \param  sessionType
 *          - 0x00 - Client initiated
 *          - 0x01 - Network initiated
 *
 */
typedef struct 
{
    uint8_t   state;
    uint8_t   userInputReq;
    uint16_t  userInputTimeout;
    uint32_t  fwdloadsize;
    uint32_t  fwloadComplete;
    uint16_t  updateCompleteStatus;
    uint8_t   severity;
    uint16_t  versionlength;
    uint8_t   version[256];
    uint16_t  namelength;
    uint8_t   package_name[256];
    uint16_t  descriptionlength;
    uint8_t   description[256];
    uint8_t   sessionType;
}unpack_omaDmFotaTlv_t;

/**
 * This structure will hold the SwiOmaDmConfig session parameters information.
 *
 *  \param  state
 *          - 0x01 - OMA-DM Read Request
 *          - 0x02 - OMA-DM Change Request
 *          - 0x03 - OMA-DM Config Complete
 *
 *  \param  userInputReq - Bit mask of available user inputs
 *          - 0x00 - No user input required.Informational indication
 *          - 0x01 - Accept
 *          - 0x02 - Reject
 *
 *  \param  userInputTimeout
 *          - Timeout for user input in minutes.
 *            A value of 0 means no time-out
 *
 *  \param  alertmsglength
 *          - Length of Alert message string in bytes
 *
 *  \param  alertmsg
 *          - Alert message in UCS2  (Max 256 characters)
 */
typedef struct 
{
    uint8_t  state;
    uint8_t  userInputReq;
    uint16_t userInputTimeout;
    uint16_t alertmsglength;
    uint8_t  alertmsg[256];
}unpack_omaDmConfigTlv_t;

/**
 * This structure will hold the SwiOmaDmConfig session notification parameters information.
 *
 *  \param  notification
 *          - 0x01 - GPS settings change
 *          - 0x02 - Device reset
 *          - 0x03 - Device factory reset
 *          - 0x04 - CI-DC Session start
 *          - 0x05 - CI-DC Session end
 *          - 0x06 - CI-PRL Session start
 *          - 0x07 - CI-PRL Session end
 *          - 0x08 - CI-FUMO Session start
 *          - 0x09 - CI-FUMO session end
 *          - 0x0A - HFA-DC Session start
 *          - 0x0B - HFA-DC Session end
 *          - 0x0C - HFA-PRL Session start
 *          - 0x0D - HFA-PRL Session end
 *          - 0x0E - HFA-FUMO Session start
 *          - 0x0F - HFA-FUMO session end
 *          - 0x10 - NI Session start
 *          - 0x11 - NI session end
 *
 *  \param  sessionStatus
 *          - This field will set to the session status for notifications that
 *            occur at the end of a session, zero for all other notifications
 *          - See \ref qaGobiApiTableSwiOMADMSessionStatus.h for session completion code.
 *
 */
typedef struct 
{
    uint8_t   notification;
    uint16_t sessionStatus;
}unpack_omaDmNotificationsTlv_t;

/**
 *  Structure that contains OMA indication information based on eventType
 *  Strucutres for which the event is not valid will have values set to 0
 *
 *  \param  eventType
 *          - 0x00 - SWIOMA-DM FOTA
 *          - 0x01 - SWIOMA-DM Config
 *          - 0x02 - SWIOMA-DM Notification
 *          - 0xff - indication missing event information.
 *
 *  \param  SessionInfoFota[OUT]
 *          - See \ref unpack_omaDmFotaTlv_t for more information 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  SessionInfoConfig[OUT]
 *          - See \ref unpack_omaDmConfigTlv_t for more information 
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  SessionInfoNotification[OUT]
 *          - See \ref unpack_omaDmNotificationsTlv_t for more information 
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 * 
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t eventType;
    unpack_omaDmFotaTlv_t          SessionInfoFota;
    unpack_omaDmConfigTlv_t        SessionInfoConfig;
    unpack_omaDmNotificationsTlv_t SessionInfoNotification;	
    swi_uint256_t                  ParamPresenceMask;
} unpack_swioma_SLQSOMADMAlertCallback_ind_t;

/**
 *  Function to unpack SWIOMADM alert indications
 *  This maps to SetSLQSOMADMAlertCallback
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *               - See \ref unpack_swioma_SLQSOMADMAlertCallback_ind_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */		
int unpack_swioma_SLQSOMADMAlertCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMAlertCallback_ind_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif //__SWIOMA_H__
