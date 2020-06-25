/**
 * \ingroup liteqmi
 *
 * \file swiomaext.h
 */

#ifndef __LITEQMI_SWIOMAEXT_H__
#define __LITEQMI_SWIOMAEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#include "msgid.h"

#define LITEQMI_MAX_SWIOMA_STR_LEN 255
#define LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN 512

/**
 *  Structure that contains the session type for OMA start session command
 *  \param  sessionType [IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0x02 - DM, to check availability of DM Update
 *              - 0x03 - PRL,  to check availability of PRL Update 
 */
typedef struct {
    uint32_t sessionType;
} pack_swioma_SLQSOMADMStartSessionExt_t;

typedef unpack_result_t  unpack_swioma_SLQSOMADMStartSessionExt_t;

/**
 *  Function to pack Start OMA-DM session command
 *  This maps to SLQSOMADMStartSessionExt
 *
 *  \param  pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param  pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param  pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *        
 *  \param  reqParam [IN]
 *          - See \ref pack_swioma_SLQSOMADMStartSessionExt_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMStartSessionExt (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMStartSessionExt_t  reqParam 
        );

/**
 *  Function to unpack Start OMA-DM session response from modem
 *  This maps to SLQSOMADMStartSessionExt
 *
 *  \param  pResp [IN]
 *            - Response from modem
 *
 *  \param  respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param  pOutput [OUT]
 *            - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMStartSessionExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMStartSessionExt_t *pOutput
        );
        
/**
 *  Structure that contains the session type for OMA cancel session command
 *  \param  sessionType [IN]
 *          - Session type
 *              - 0x01 - FOTA, to check availability of FW Update
 *              - 0xFF - Cancel any active OMADM session
 */
typedef struct {
    uint32_t sessionType;
} pack_swioma_SLQSOMADMCancelSessionExt_t;

typedef unpack_result_t  unpack_swioma_SLQSOMADMCancelSessionExt_t;

/**
 *  Function to pack cancel OMA-DM session command
 *  This maps to SLQSOMADMCancelSessionExt
 *
 *  \param  pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param  pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param  pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *        
 *  \param  reqParam [IN]
 *          - See \ref pack_swioma_SLQSOMADMCancelSessionExt_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
 int pack_swioma_SLQSOMADMCancelSessionExt (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMCancelSessionExt_t  reqParam 
        );

/**
 *  Function to pack cancel OMA-DM session command
 *  This maps to SLQSOMADMCancelSessionExt
 *
 *  \param  pResp [IN]
 *            - Response from modem
 *
 *  \param  respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param  pOutput [OUT]
 *            - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swioma_SLQSOMADMCancelSessionExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMCancelSessionExt_t *pOutput
        );

/**
 *  Structure containing the OMA DM settings to be set on the device
 *  This maps to structure SLQSOMADMSetSettingsExt
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
 *  \param  OMADMEnable [IN]
 *          -  1 byte parameter indicating OMA DM
 *              - 0x00 - OMA DM Disabled
 *              - 0x01 - OMA DM Enabled
 *
 *  \param  OMADMLogEnable [IN]
 *          -  1 byte parameter indicating OMA DM Logs
 *              - 0x00 - OMA DM Logs Disabled
 *              - 0x01 - OMA DM Logs Enabled
 *
 *  \param  FUMOEnable [IN]
 *          -  1 byte parameter indicating FUMO enabled
 *              - 0x00 - FUMO Disabled
 *              - 0x01 - FUMO enabled
 *
 *  \param  PRLEnable [IN]
 *          -  1 byte parameter indicating PRL Enabled
 *              - 0x00 - PRL Disabled
 *              - 0x01 - PRL Enabled
 *              - 0x02 - Launch a CI PRL
 *              - 0x03 - PRL Enabled and PRL automatic update every 45 days
 *              - 0x04 - PRL Enabled and PRL update ever 90 days
 * 
 */
typedef struct {
    uint8_t  FOTAdownload;
    uint8_t  FOTAUpdate;
    uint8_t  OMADMEnable;
    uint8_t  OMADMLogEnable;
    uint8_t  FUMOEnable;
    uint8_t  PRLEnable;
} pack_swioma_SLQSOMADMSetSettingsExt_t;
        
typedef unpack_result_t  unpack_swioma_SLQSOMADMSetSettingsExt_t;

/**
 *  Function to pack OMA-DM set settings command
 *  This maps to SLQSOMADMSetSettingsExt
 *
 *  \param  pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param  pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param  pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *        
 *  \param  reqParam [IN]
 *          - See \ref pack_swioma_SLQSOMADMSetSettings_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
int pack_swioma_SLQSOMADMSetSettingsExt (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMSetSettingsExt_t  reqParam 
        );

/**
 *  Function to unpack OMA-DM set settings command
 *  This maps to SLQSOMADMSetSettingsExt
 *
 *  \param  pResp [IN]
 *            - Response from modem
 *
 *  \param  respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param  pOutput [OUT]
 *            - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swioma_SLQSOMADMSetSettingsExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMSetSettingsExt_t *pOutput
        );

/**
 *  Structure containing the OMA DM selection
 *  
 *  \param  selection [IN]
 *          - OMA-DM NIA Selection
 *              - 0x01 - Accept
 *              - 0x02 - Reject
 *              - 0x03 - Defer
 */
typedef struct {
    uint32_t  selection;
} pack_swioma_SLQSOMADMSendSelectionExt_t;

typedef unpack_result_t  unpack_swioma_SLQSOMADMSendSelectionExt_t;

/**
 *  Function to pack OMA-DM send selection command
 *  This maps to SLQSOMADMSendSelectionExt
 *
 *  \param  pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param  pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param  pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *        
 *  \param  reqParam [IN]
 *          - See \ref pack_swioma_SLQSOMADMSendSelectionExt_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
int pack_swioma_SLQSOMADMSendSelectionExt (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swioma_SLQSOMADMSendSelectionExt_t  reqParam 
        );

/**
 *  Function to unpack OMA-DM send selection command
 *  This maps to SLQSOMADMSendSelectionExt
 *
 *  \param  pResp [IN]
 *            - Response from modem
 *
 *  \param  respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param  pOutput [OUT]
 *            - response unpacked
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */        
int unpack_swioma_SLQSOMADMSendSelectionExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMSendSelectionExt_t *pOutput
        );


/**
 *  Function to pack OMA-DM session info command
 *  This maps to SLQSOMADMGetSessionInfoExt
 *
 *  \param  pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param  pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param  pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 20 seconds
 */
int pack_swioma_SLQSOMADMGetSessionInfoExt (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  Structure containing info for OMADM session
 *  Bit to check in ParamPresenceMask - <B>16</B>
 *  \param  status
 *          - 1 Byte parameter indicating status of OMADM initialization
 *               0 – OMA module initiation not completed.
 *               1 – OMA module initiation completed but no UI is registered.
 *               128 – OMA initiation completed.
 *               255 – OMA initiation error.
 *
 *  \param  sessionState
 *          - 1 byte parameter indicating OMA session state
 *              0 - OMA module in CIDC session.
 *              1 - OMA module in CIFUMO session
 *              2 - OMA module in device initiated FUMO session
 *              3 - OMA module in FUMO resume session
 *              4 - OMA module is sending FUMO report
 *              5 - OMA module in device initiated DM session
 *              6 - OMA module in HFA DC session
 *              7 - OMA module in CI PRL session
 *              8 - OMA module in device initiated PRL session
 *              9 - OMA module in HFA PRL session
 *             10 - OMA module in HFA FUMO session
 *             11 - OMA module in NI PRL session
 *             12 - OMA is storing configuration after DC/PRL session
 *             13 - OMA module is blocked by UI
 *             14 - there is a pending session
 *             15 - OMA module in idle state
 *
 *  \param  hfaStatus
 *          - 1 byte parameter indicating session state
 *             0 – HFA completed
 *             1 – HFA not start
 *             2 – HFA DC completed, but PRL is not completed
 *             3 – HFA PRL completed, but FUMO is not completed
 *
 *  \param  hfaMaxRetry
 *          - 2 byte parameter indicating max retry for HFA
 *             0 – HFA completed
 *
 *  \param  hfaRetryInterval
 *          - 2 byte parameter indicating retry interval in seconds for HFA
 *             0 – HFA completed
 *
 *  \param  hfaRetryIndex
 *          - 2 byte parameter indicating number of retry currently ongoing
 *              0 – no retrying
 *              0xFFFF- Invalid value
 *
 *  \param  fumoState
 *          - 1 byte parameter indicating fumo state
 *             0 – FUMO state READY
 *             1 – FUMO state DISCOVERY
 *             2 –  FUMO state DISCOCONF
 *             3 –  FUMO state DOWNLOAD
 *             4 – FUMO state RECEIVED
 *             5 – FUMO state CONFIRM
 *             6 – FUMO state CONFIRMED
 *             7 – FUMO state UPDATE
 *             8 – FUMO state UPDATING
 *             9 – FUMO state UNUSABLE
 *             10 – FUMO state REJECTED
 *             11 – FUMO state REJECTCONF
 *             12 – FUMO state FAILED
 *             13 – FUMO state DENIED
 *             14 – FUMO state UPDATED
 *             15 – FUMO state FINCONF
 *             16 – FUMO state FINISHED
 *
 *  \param  fumoResultCode
 *          - 4 byte parameter indicating FUMO update image installation result.
 *              200 - Request succeeded
 *              250 – the start of vendor specified success result code
 *              299 – the end of vendor specified success result code
 *              400 - Management client error
 *              401 - User rejected operation
 *              402 - Corrupted update package
 *              403 - Wrong package for device
 *              404 - Invalid package signature
 *              405 - Update package not acceptable
 *              406 - DL auth failure
 *              407 - DL download timeout
 *              408 - Unsupported operation
 *              409 - Err not defined by other code
 *              410 - Firmware update failed
 *              411 - Malformed or bad DL URL
 *              412 - DL server unavailable
 *              450 - vendor specified client error start
 *              499 - vendor specified client error end
 *              500 - DL server error
 *              501 - DL fails due to out of memory
 *              502 - Update fails, out of memory
 *              503 - DL fails due to net issues
 *              550 - vendor defined DL server error start
 *              599 - vendor defined DL server error end
 *              0xFFFFFFFF – invalid value.
 *
 *  \param  pkgVendorNameLength
 *          - 2 byte parameter indicating Length of package vendor String
 *            in WORDs
 *
 *  \param  pkgVendorName
 *          - Variable length parameter indicating Package Name in UCS2
 *          - size in bytes is 2*pkgVendorNameLength
 *
 *  \param  pkgSize
 *          - 4 byte parameter indicating pkg size
 *
 *  \param  pkgVersionNameLength
 *          - 2 byte parameter indicating Length of Package version Name String
 *            in WORDs
 *
 *  \param  pkgVersionName
 *          - Variable length parameter indicating Package version Name in UCS2
 *          - size in bytes is 2*pkgVersionNameLength
 *
 *  \param  pkgNameLength
 *          - 2 byte parameter indicating Length of Package Name String
 *            in WORDs
 *
 *  \param  pkgName
 *          - Variable length parameter indicating Package Name in UCS2
 *          - size in bytes is 2*pkgNameLength
 *
 *  \param  pkgDescLength
 *          - 2 byte parameter indicating Length of Package description String
 *            in WORDs
 *
 *  \param  pkgDesc
 *          - Variable length parameter indicating Package description in UCS2
 *          - size in bytes is 2*pkgDescLength
 *
 *  \param  pkgDateLength
 *          - 2 byte parameter indicating Length of Package date String
 *            in WORDs
 *
 *  \param  pkgDate
 *          - Variable length parameter indicating Package date in UCS2
 *          - size in bytes is 2*pkgDateLength
 *
 *  \param  pkgInstallTimeLength
 *          - 2 byte parameter indicating Length of Package install time String
 *            in WORDs
 *
 *  \param  pkgInstallTime
 *          - Variable length parameter indicating Package install time in UCS2
 *          - size in bytes is 2*pkgInstallTimeLength
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t   status;
    uint8_t   sessionState;
    uint8_t   hfaStatus;
    uint16_t  hfaMaxRetry;
    uint16_t  hfaRetryInterval;
    uint16_t  hfaRetryIndex;
    uint8_t   fumoState;
    uint32_t  fumoResultCode;
    uint16_t  pkgVendorNameLength;
    uint8_t   pkgVendorName[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    uint32_t  pkgSize;
    uint16_t  pkgVersionNameLength;
    uint8_t   pkgVersionName[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    uint16_t  pkgNameLength;
    uint8_t   pkgName[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    uint16_t  pkgDescLength;
    uint8_t   pkgDesc[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    uint16_t  pkgDateLength;
    uint8_t   pkgDate[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    uint16_t  pkgInstallTimeLength;
    uint8_t   pkgInstallTime[LITE_SWIOMAEXT_MAX_UCS2_DATA_LEN];
    swi_uint256_t  ParamPresenceMask;
} unpack_swioma_SLQSOMADMGetSessionInfoExt_t;

/**
 *  Function to unpack information related to the current 
 * (or previous if no session is active) OMA-DM session.
 *  This maps to SLQSOMADMGetSessionInfoExt 
 *
 *  \param pResp [in]
 *              - Response from modem
 *
 *  \param respLen [in]
 *              - Length of pResp from modem
 *
 *  \param pOutput [out]
 *               - See \ref unpack_swioma_SLQSOMADMGetSessionInfoExt_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_swioma_SLQSOMADMGetSessionInfoExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swioma_SLQSOMADMGetSessionInfoExt_t *pOutput
        );


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif //__LITEQMI_SWIOMAEXT_H__
