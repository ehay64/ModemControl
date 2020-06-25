/**
 * \ingroup liteqmi
 *
 * \file pds.h
 */
#ifndef __LITEQMI_PDS_H__
#define __LITEQMI_PDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

/**
 *  Structure contain parameters of current PDS state.
 *
 *  @param  pEnabledStatus
 *          - Current PDS state
 *              - 0 - disable
 *              - 1 - enable
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pTrackingStatus
 *          - Current PDS tracking session state
 *          - Values:
 *              - 0x00 - Unknown
 *              - 0x01 - Inactive
 *              - 0x02 - Active
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t *pEnabledStatus;
    uint32_t *pTrackingStatus ;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetPDSState_t;

/**
 * Get current PDS state pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetPDSState(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get current PDS state unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetPDSState(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetPDSState_t *pOutput
        );

/**
 *  Structure contain PDS state parameters.
 *
 *  @param  enable
 *          - Desired PDS state
 *              - Zero     - disable
 *              - Non-Zero - enable
 */
typedef struct{
    uint32_t enable;
} pack_pds_SetPDSState_t;

typedef unpack_result_t  unpack_pds_SetPDSState_t;

/**
 * Sets the PDS state pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetPDSState(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetPDSState_t *pReqParam
        );

/**
 * Sets the PDS state unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetPDSState(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetPDSState_t *pOutput
        );

/**
 *  Structure contain session configuration parameters.
 *
 *  @param  pOperation
 *          - Current session operating mode
 *              - 0 - Standalone
 *              - 1 - MS based
 *              - 2 - MS assisted
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pTimeout
 *          - Maximum amount of time (seconds) to work on each fix, maximum is 255
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pInterval
 *          - Interval (seconds) between fix requests
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pAccuracy
 *          - Preferred accuracy threshold (meters)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t *pOperation;
    uint8_t  *pTimeout;
    uint32_t *pInterval;
    uint32_t *pAccuracy;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetPDSDefaults_t;

/**
 * Get the default tracking session configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetPDSDefaults(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get the default tracking session configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetPDSDefaults(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetPDSDefaults_t *pOutput
        );

/**
 *  Structure containing session configuration parameters.
 *
 *  @param  operation
 *          - Current session operating mode
 *              - 0 - Standalone
 *              - 1 - MS based
 *              - 2 - MS assisted
 *
 *  @param  timeout
 *          - Maximum amount of time (seconds) to work on each fix, maximum is 255
 *
 *  @param  interval
 *          - Interval (seconds) between fix requests
 *
 *  @param  accuracy
 *          - Preferred accuracy threshold (meters)
 */
typedef struct{
    uint32_t operation;
    uint8_t  timeout;
    uint32_t interval;
    uint32_t accuracy;
} pack_pds_SetPDSDefaults_t;

typedef unpack_result_t unpack_pds_SetPDSDefaults_t;

/**
 * Sets the default tracking session configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetPDSDefaults(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetPDSDefaults_t *pReqParam
        );

/**
 * Sets the default tracking session configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetPDSDefaults(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetPDSDefaults_t *pOutput
        );

/**
 *  Structure contain the parameter of automatic tracking configuration for the NMEA COM port.
 *
 *  @param  pbAuto
 *          - Automatic tracking enabled for NMEA COM port
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
*/
typedef struct{
    uint32_t *pbAuto;
    swi_uint256_t    ParamPresenceMask;
} unpack_pds_GetPortAutomaticTracking_t;

/**
 * Returns the automatic tracking configuration for the NMEA COM port pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetPortAutomaticTracking(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Returns the automatic tracking configuration for the NMEA COM port unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetPortAutomaticTracking(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetPortAutomaticTracking_t *pOutput
        );


/**
 *  Structure contain the parameter for the automatic tracking configuration for the NMEA COM port.
 *
 *  @param  bAuto
 *          - Enable automatic tracking for NMEA COM port
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 */
typedef struct{
    uint32_t bAuto;
} pack_pds_SetPortAutomaticTracking_t;

typedef unpack_result_t unpack_pds_SetPortAutomaticTracking_t;

/**
 * Sets the automatic tracking configuration for the NMEA COM port pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetPortAutomaticTracking(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetPortAutomaticTracking_t *pReqParam
        );

/**
 * Sets the automatic tracking configuration for the NMEA COM port unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetPortAutomaticTracking(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetPortAutomaticTracking_t *pOutput
        );

/**
 *  Structure contain the parameter for PDS tracking session.
 *
 *  @param  sessionControl
 *          - Control method:
 *              - 0x0 - Manual
 *
 *  @param  sessionType
 *          - Type:
 *              - 0x0 - New
 *
 *  @param  sessionOperation
 *          - Operating mode:
 *              - 0x00 - Standalone
 *              - 0x01 - MS-based
 *
 *  @param  sessionServerOption
 *          - Location server option:
 *              - 0x0 - Default
 *
 *  @param  fixTimeout
 *          - Maximum time to work on each fix (in seconds, max 255)
 *
 *  @param  fixCount
 *          - Count of position fix requests for this session
 *            (must be at least 1)
 *
 *  @param  fixInterval
 *          - interval between position fix requests (in seconds)
 *
 *  @param  fixAccuracy
 *          - Preferred accuracy threshold(in meters)
 *
 */
typedef struct{
    uint8_t  sessionControl;
    uint8_t  sessionType;
    uint8_t  sessionOperation;
    uint8_t  sessionServerOption;
    uint8_t  fixTimeout;
    uint32_t fixInterval;
    uint32_t fixCount;
    uint32_t fixAccuracy;
} pack_pds_StartPDSTrackingSessionExt_t;

typedef unpack_result_t unpack_pds_StartPDSTrackingSessionExt_t;

/**
 * Starts a PDS tracking session pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_StartPDSTrackingSessionExt(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_StartPDSTrackingSessionExt_t *pReqParam
        );

/**
 * Starts a PDS tracking session unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_StartPDSTrackingSessionExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_StartPDSTrackingSessionExt_t *pOutput
        );

typedef unpack_result_t unpack_pds_StopPDSTrackingSession_t;

/**
 * Stops a PDS tracking session pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_StopPDSTrackingSession(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Stops a PDS tracking session unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_StopPDSTrackingSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_StopPDSTrackingSession_t *pOutput
        );

/**
 *  Structure contain the parameter for PDS Inject time.
 *
 *  @param  systemTime
 *          - System time( milliseconds )
 *
 *  @param  systemDiscontinuities
 *          - Number of system time discontinuities
 *
 */
typedef struct{
    uint64_t    systemTime;
    uint16_t    systemDiscontinuities;
} pack_pds_PDSInjectTimeReference_t;

typedef unpack_result_t unpack_pds_PDSInjectTimeReference_t;

/**
 * Inject system time parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_PDSInjectTimeReference(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_PDSInjectTimeReference_t *pReqParam
        );

/**
 * Inject system time parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_PDSInjectTimeReference(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_PDSInjectTimeReference_t *pOutput
        );

/**
 *  Structure contain the parameter of automatic tracking configuration for the NMEA COM port.
 *
 *  @param  pbEnabled
 *          - Automatic XTRA download status
 *              - 0 - Disabled
 *              - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pInterval
 *          - Interval (hours) between XTRA downloads
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t  *pbEnabled;
    uint16_t  *pInterval;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetXTRAAutomaticDownload_t;

/**
 * XTRA automatic database download configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetXTRAAutomaticDownload(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * XTRA automatic database download configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetXTRAAutomaticDownload(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetXTRAAutomaticDownload_t *pOutput
        );

/**
 *  Structure contain the parameter to set the XTRA automatic database
 *  download configuration.
 *
 *  @param  bEnabled
 *          - Automatic XTRA download status
 *              - 0 - Disabled
 *              - 1 - Enabled
 *
 *  @param  interval
 *          - Interval (hours) between XTRA downloads
 *
 */
typedef struct{
    uint32_t  bEnabled;
    uint16_t  interval;
} pack_pds_SetXTRAAutomaticDownload_t;

typedef unpack_result_t  unpack_pds_SetXTRAAutomaticDownload_t;

/**
 * Sets the XTRA automatic database download configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetXTRAAutomaticDownload(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetXTRAAutomaticDownload_t *pReqParam
        );

/**
 * Sets the XTRA automatic database download configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetXTRAAutomaticDownload(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetXTRAAutomaticDownload_t *pOutput
        );

/**
 *  Structure contain the parameter for XTRA WWAN network preference
 *
 *  @param  pPreference
 *          - XTRA WWAN network preference
 *              - 0x00 - None (any available network)
 *              - 0x01 - Home-only, only when on home systems
 *              - 0x02 - Roam-only, only when on non-home systems
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t *pPreference;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetXTRANetwork_t;

/**
 * XTRA WWAN network preference parameter pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetXTRANetwork(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * XTRA WWAN network preference parameter unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetXTRANetwork(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetXTRANetwork_t *pOutput
        );

/**
 *  Structure contain the parameter to Sets the XTRA WWAN network preference
 *
 *  @param  preference
 *          - XTRA WWAN network preference
 *              - 0x00 - None (any available network)
 *              - 0x01 - Home-only, only when on home systems
 *              - 0x02 - Roam-only, only when on non-home systems
 *
 */
typedef struct{
    uint32_t preference;
} pack_pds_SetXTRANetwork_t;

typedef unpack_result_t  unpack_pds_SetXTRANetwork_t;

/**
 * Sets the XTRA WWAN network preference pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetXTRANetwork(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetXTRANetwork_t *pReqParam
        );

/**
 * Sets the XTRA WWAN network preference unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetXTRANetwork(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetXTRANetwork_t *pOutput
        );

/**
 *  Structure contain the parameter for XTRA database validity period
 *
 *  @param  pGPSWeek
 *          - Starting GPS week of validity period
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pGPSWeekOffset
 *          - Starting GPS week offset (minutes) of validity period
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pDuration
 *          - Length of validity period (hours)
 *          - NULL pointer - Invalid data.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t *pGPSWeek;
    uint16_t *pGPSWeekOffset;
    uint16_t *pDuration;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetXTRAValidity_t;

/**
 * XTRA database validity period parameter pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetXTRAValidity(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * XTRA database validity period parameter unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetXTRAValidity(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetXTRAValidity_t *pOutput
        );

typedef unpack_result_t unpack_pds_ForceXTRADownload_t;

/**
 * Forces the XTRA database to be downloaded to the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_ForceXTRADownload(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Forces the XTRA database to be downloaded to the device unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_ForceXTRADownload(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_ForceXTRADownload_t *pOutput
        );

/**
 *  Structure contain the parameter for automatic tracking state.
 *
 *  @param  pbAuto
 *          - Automatic tracking session started for service
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t *pbAuto;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_GetServiceAutomaticTracking_t;

/**
 * automatic tracking state for the service pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_GetServiceAutomaticTracking(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * automatic tracking state for the service unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_GetServiceAutomaticTracking(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_GetServiceAutomaticTracking_t *pOutput
        );

/**
 *  Structure contain the parameter to Sets the automatic tracking state.
 *
 *  @param  bAuto
 *          - Automatic tracking session started for service
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *
 */
typedef struct{
    uint32_t bAuto;
} pack_pds_SetServiceAutomaticTracking_t;

typedef unpack_result_t unpack_pds_SetServiceAutomaticTracking_t;

/**
 * Sets the automatic tracking state for the service pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetServiceAutomaticTracking(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SetServiceAutomaticTracking_t *pReqParam
        );

/**
 * Sets the automatic tracking state for the service unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetServiceAutomaticTracking(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetServiceAutomaticTracking_t *pOutput
        );

/**
 *  Structure contain the parameter to Resets the specified PDS data.
 *
 *  @param  pGPSDataMask
 *          - Bitmask of GPS data to clear (optional)
 *              - 0x00000001 - EPH
 *              - 0x00000002 - ALM
 *              - 0x00000004 - POS
 *              - 0x00000008 - TIME
 *              - 0x00000010 - IONO
 *              - 0x00000020 - UTC
 *              - 0x00000040 - HEALTH
 *              - 0x00000080 - SVDIR
 *              - 0x00000100 - SVSTEER
 *              - 0x00000200 - SADATA
 *              - 0x00000400 - RTI
 *              - 0x00000800 - ALM_CORR
 *              - 0x00001000 - FREQ_BIAS_EST
 *
 *  @param  pCellDataMask
 *          - Bitmask of cell data to clear (optional)
 *              - 0x00000001 - POS
 *              - 0x00000002 - LATEST_GPS_POS
 *              - 0x00000004 - OTA_POS
 *              - 0x00000008 - EXT_REF_POS
 *              - 0x00000010 - TIMETAG
 *              - 0x00000020 - CELLID
 *              - 0x00000040 - CACHED_CELLID
 *              - 0x00000080 - LAST_SRV_CELL
 *              - 0x00000100 - CUR_SRV_CELL
 *              - 0x00000200 - NEIGHBOR_INFO
 *
 */
typedef struct{
    uint32_t *pGPSDataMask;
    uint32_t *pCellDataMask;
} pack_pds_ResetPDSData_t;

typedef unpack_result_t  unpack_pds_ResetPDSData_t;

/**
 * Resets the specified PDS data pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_ResetPDSData(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_ResetPDSData_t *pReqParam
        );

/**
 * Resets the specified PDS data unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_ResetPDSData(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_ResetPDSData_t *pOutput
        );

/**
 *  Structure contain the parameter to Sets the PDS AGPS (MS-based) configuration.
 *
 *  @param  pServerAddress
 *          - IPv4 address of AGPS server [optional]
 *
 *  @param  pServerPort
 *          - Port number of AGPS server [optional - should be present
 *            when pServerAddress is present]
 *
 *  @param  pServerURL
 *          - URL of the AGPS server [optional]
 *
 *  @param  pServerURLLength
 *          - URL length of AGPS server [optional - should be present
 *            when pServerURL is present]
 *
 *  @param  pNetworkMode
 *          - Network Mode of AGPS Server [optional - should be present
 *            in Multimode Systems]
 *              - 0x00 - UMTS
 *              - 0x01 - CDMA
 *
 */
typedef struct{
    uint32_t *pServerAddress;
    uint32_t *pServerPort;
    uint8_t  *pServerURL;
    uint8_t  *pServerURLLength;
    uint8_t  *pNetworkMode;
} pack_pds_SLQSSetAGPSConfig_t;

typedef unpack_result_t unpack_pds_SLQSSetAGPSConfig_t;

/**
 * Sets the PDS AGPS (MS-based) configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSSetAGPSConfig(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SLQSSetAGPSConfig_t *pReqParam
        );

/**
 * Sets the PDS AGPS (MS-based) configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSSetAGPSConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSSetAGPSConfig_t *pOutput
        );

/**
 *  Structure contain the parameter to Injects a absolute time reference.
 *
 *  @param  timeMsec
 *          - Represents the number of milliseconds elapsed since either
 *            a GPS or UTC time base. If the time base is UTC, this value
 *            should NOT include leap seconds
 *
 *  @param  timeUncMsec
 *          - Time uncertainty in milliseconds
 *
 *  @param  timeBase
 *          - Time base
 *              - 0x00 - GPS (midnight, Jan 6, 1980)
 *              - 0x01 - UTC (midnight, Jan 1, 1970)
 *
 *  @param  forceFlag
 *          - Force acceptance of data
 *
 */
typedef struct{
    uint64_t     timeMsec;
    uint32_t     timeUncMsec;
    uint8_t      timeBase;
    uint8_t      forceFlag;
} pack_pds_SLQSPDSInjectAbsoluteTimeReference_t;

typedef unpack_result_t  unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t;

/**
 * Injects a absolute time reference into the PDS engine pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSPDSInjectAbsoluteTimeReference(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SLQSPDSInjectAbsoluteTimeReference_t *pReqParam
        );

/**
 * Injects a absolute time reference into the PDS engine unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSPDSInjectAbsoluteTimeReference(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t *pOutput
        );

/**
 *  Structure contain the parameter to PDS AGPS (MS-based) configuration pack.
 *
 *  @param  pNetworkMode
 *          - Network Mode of AGPS Server [optional - should be present
 *            in Multimode Systems]
 *              - 0x00 - UMTS
 *              - 0x01 - CDMA
 *
 */
typedef struct{
    uint8_t  *pNetworkMode;
} pack_pds_SLQSGetAGPSConfig_t;

/**
 *  Structure contain the parameter for PDS AGPS (MS-based) configuration unpack.
 *
 *  @param  pServerAddress
 *          - IPv4 address of AGPS server. "0" if not set
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pServerPort
 *          - Port number of AGPS server. "0" if not set
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pServerURL
 *          - URL of the AGPS server. "0" if not set
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pServerURLLength
 *          - URL length of AGPS server. "0" if not set
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t *pServerAddress;
    uint32_t *pServerPort;
    uint8_t  *pServerURL;
    uint8_t  *pServerURLLength;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_SLQSGetAGPSConfig_t;

/**
 * Gets the PDS AGPS (MS-based) configuration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSGetAGPSConfig(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SLQSGetAGPSConfig_t *pReqParam
        );

/**
 * Gets the PDS AGPS (MS-based) configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSGetAGPSConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSGetAGPSConfig_t *pOutput
        );

/**
 *  Position Data Parameters from the external source to be injected to PDS
 *  engine.
 *
 *  @param  pTimeStamp
 *          - Timestamp of the injected position in msec. The time can be of
 *            type UTC, GPS, or Age and is defined in the pTimeType parameter.
 *            If the pTimeType is not present, the timestamp shall be
 *            assumed to be UTC time
 *
 *  @param  pLatitude
 *          - Latitude position referenced to the WGS-84 reference ellipsoid,
 *            counting positive angles north of the equator and negative angles
 *            south of the equator. Value (in decimal degrees) in the range
 *            from -90 degrees to +90 degrees.Value in double float format
 *            (refer toIEEE Std 754-1985)
 *
 *  @param  pLongitude
 *          - Longitude position referenced to the WGS-84 reference ellipsoid,
 *            counting positive angles east of the Greenwich Meridian and
 *            negative angles west of Greenwich meridian. Value (in decimal
 *            degrees) in the range from -180 degrees to +180 degrees.
 *
 *  @param  pAltitudeWrtEllipsoid
 *          - Height above the WGS-84 reference ellipsoid. Value conveys
 *          height (in meters). When injecting altitude information, the
 *          control point should include either this parameter or the
 *          pAltitudeWrtSealevel parameter. Value in single float format
 *          (refer to IEEE Std 754-1985)
 *
 *  @param  pAltitudeWrtSealevel
 *          - Height of MS above the mean sea level in units (in meters).
 *          When injecting altitude information, the control point should
 *          include either this parameter or the pAltitudeWrtEllipsoid
 *          parameter. Value in single float format (refer to IEEE Std 754-1985)
 *
 *  @param  pHorizontalUncCircular
 *          - Circular horizontal uncertainty (in meters). This parameter must
 *          be included if the latitude and longitude parameters are specified.
 *          Value in single float format (refer to IEEE Std 754-1985)
 *
 *  @param  pVerticalUnc
 *          - Vertical uncertainty (in meters). This parameter must be included
 *          if one of the altitude parameter are specified.Value in single float
 *          format (refer to IEEE Std 754-1985)
 *
 *  @param  pHorizontalConfidence
 *          - Confidence value of the location horizontal uncertainty,
 *          specified as percentage, 1 to 100. This parameter must be included
 *          if the latitude and longitude parameters are specified.
 *
 *  @param  pVerticalConfidence
 *          - Confidence value of the location vertical uncertainty, specified
 *          as percentage, 1 to 100. This parameter must be included if one of
 *          the altitude paramters are specified.
 *
 *  @param  pPositionSource
 *          - Source of injected position:
 *              - 0x00 - Unknown
 *              - 0x01 - GPS
 *              - 0x02 - Cell ID
 *              - 0x03 - Enhanced cell ID
 *              - 0x04 - WiFi
 *              - 0x05 - Terrestrial
 *              - 0x06 - Terrestrial hybrid
 *              - 0x07 - Other
 *
 *  @param  pTimeType
 *          - Defines the time value set in the pTimeStamp parameter.
 *              - 0x00 - UTC Time: starting Jan 1, 1970
 *              - 0x01 - GPS Time: starting Jan 6, 1980
 *              - 0x02 - Age: Age of position information
 *
 */
typedef struct{
    uint64_t     *pTimeStamp;
    uint64_t     *pLatitude;
    uint64_t     *pLongitude;
    uint32_t     *pAltitudeWrtEllipsoid;
    uint32_t     *pAltitudeWrtSealevel;
    uint32_t     *pHorizontalUncCircular;
    uint32_t     *pVerticalUnc;
    uint8_t      *pHorizontalConfidence;
    uint8_t      *pVerticalConfidence;
    uint8_t      *pPositionSource;
    uint8_t      *pTimeType;
} pack_pds_SLQSPDSInjectPositionData_t;

typedef unpack_result_t unpack_pds_SLQSPDSInjectPositionData_t;

/**
 * Injects position data into the PDS engine pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSPDSInjectPositionData(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SLQSPDSInjectPositionData_t *pReqParam
        );

/**
 * Injects position data into the PDS engine unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSPDSInjectPositionData(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSPDSInjectPositionData_t *pOutput
        );

typedef unpack_result_t unpack_pds_SLQSPDSDeterminePosition_t;

/**
 * Requests the MSM GPS service to obtain the current position for manually
 * controlled tracking sessions pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSPDSDeterminePosition(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Requests the MSM GPS service to obtain the current position for manually
 * controlled tracking sessions unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSPDSDeterminePosition(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSPDSDeterminePosition_t *pOutput
        );

/**
 *  Structure contain the parameter for GPS state Info.
 *
 *  @param  EngineState
 *          - Values:
 *              - 0 - OFF
 *              - 1 - ON
 *          - This field is always valid
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ValidMask
 *          - Mask of valid state information data.
 *          - Values:
 *              - 0x00000001 - Position(latitude/longitude/horizontal uncertainty)
 *              - 0x00000002 - Altitude and vertical uncertainty
 *              - 0x00000004 - Time ms
 *              - 0x00000008 - Time week number
 *              - 0x00000010 - Time uncertainty
 *              - 0x00000020 - Iono validity
 *              - 0x00000040 - GPS ephemeris
 *              - 0x00000080 - GPS almanac
 *              - 0x00000100 - GPS health
 *              - 0x00000200 - GPS visible SVs
 *              - 0x00000400 - GLONASS ephemeris
 *              - 0x00000800 - GLONASS almanac
 *              - 0x00001000 - GLONASS health
 *              - 0x00002000 - GLONASS visible SVs
 *              - 0x00004000 - SBAS ephemeris
 *              - 0x00008000 - SBAS almanac
 *              - 0x00010000 - SBAS health
 *              - 0x00020000 - SBAS visible SVs
 *              - 0x00040000 - XTRA information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Latitude
 *          - Latitude position referenced to the WGS-84 reference ellipsoid,
 *            counting positive angles north of the equator and negative angles
 *            south of the equator.
 *          - Units: Decimal degrees
 *          - Range: -90 to +90 degrees.
 *          - Value is in double float format (refer to IEEE Std 754-1985)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Longitude
 *          - Longitude position referenced to the WGS-84 reference ellipsoid,
 *            counting positive angles east of the Greenwich Meridian and
 *            negative angles west of Greenwich meridian.
 *          - Units: Decimal degrees
 *          - Range: -180 to +180 degrees
 *          - Value is in double float format (refer to IEEE Std 754-1985)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  HorizontalUncertainty
 *          - Circular horizontal uncertainty (in meters). The uncertainty is
 *            provided at 63 percent confidence.
 *          - Value is in single float format (refer to IEEE Std 754-1985)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Altitude
 *          - Height above the WGS-84 reference ellipsoid. Value conveys
 *            height (in meters) plus 500 m
 *          - Range -500 to 15883
 *          - Value in single float format (refer to IEEE Std 754-1985)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  VerticalUncertainty
 *          - Vertical uncertainty (in meters). The uncertainty is
 *            provided at 68 percent confidence.
 *          - Value in single float format (refer to IEEE Std 754-1985)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  TimeStmp_tow_ms
 *          - Time stamp in GPS time of week( in milliseconds)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  TimeStmp_gps_week
 *          - GPS week number
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Time_uncert_ms
 *          - Time uncertainty (in milliseconds). The uncertainty
 *            is provided at 99 percent confidence.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Iono_valid
 *          - Iono validity.
 *          - Values:
 *              - 0 - Invalid
 *              - 1 - Valid
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  gps_ephemeris_sv_msk
 *          - GPS SV mask for ephemeris; if the bit is set, ephemeris for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  gps_almanac_sv_msk
 *          - GPS SV mask for almanac; if the bit is set, almanac for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  gps_health_sv_msk
 *          - GPS SV mask for health; if the bit is set, health for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  gps_visible_sv_msk
 *          - GPS SV mask for visible Svs; if the bit is set, the SV is
 *            available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  glo_ephemeris_sv_msk
 *          - GLONASS SV mask for ephemeris; if the bit is set, ephemeris
 *            for that SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  glo_almanac_sv_msk
 *          - GLONASS SV mask for almanac; if the bit is set, almanac for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  glo_health_sv_msk
 *          - GLONASS SV mask for health; if the bit is set, health for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  glo_visible_sv_msk
 *          - GLONASS SV mask for visible SVs; if the bit is set, the SV is
 *            available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  sbas_ephemeris_sv_msk
 *          - SBAS SV mask for ephemeris; if the bit is set, ephemeris
 *            for that SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  sbas_almanac_sv_msk
 *          - SBAS SV mask for almanac; if the bit is set, almanac for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  sbas_health_sv_msk
 *          - SBAS SV mask for health; if the bit is set, health for that
 *            SV is available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  sbas_visible_sv_msk
 *          - SBAS SV mask for visible SVs; if the bit is set, the SV is
 *            available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  xtra_start_gps_week
 *          - Current XTRA information is valid starting from this GPS week
 *            number
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  xtra_start_gps_minutes
 *          - Current XTRA information is valid starting from the GPS minutes
 *            with the GPS week
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  xtra_valid_duration_hours
 *          - XTRA information is valid for this many hours starting from the
 *            specified GPS week/minutes
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t      EngineState;
    uint32_t     ValidMask;
    uint64_t     Latitude;
    uint64_t     Longitude;
    uint32_t     HorizontalUncertainty;
    uint32_t     Altitude;
    uint32_t     VerticalUncertainty;
    uint32_t     TimeStmp_tow_ms;
    uint16_t     TimeStmp_gps_week;
    uint32_t     Time_uncert_ms;
    uint8_t      Iono_valid;
    uint32_t     gps_ephemeris_sv_msk;
    uint32_t     gps_almanac_sv_msk;
    uint32_t     gps_health_sv_msk;
    uint32_t     gps_visible_sv_msk;
    uint32_t     glo_ephemeris_sv_msk;
    uint32_t     glo_almanac_sv_msk;
    uint32_t     glo_health_sv_msk;
    uint32_t     glo_visible_sv_msk;
    uint32_t     sbas_ephemeris_sv_msk;
    uint32_t     sbas_almanac_sv_msk;
    uint32_t     sbas_health_sv_msk;
    uint32_t     sbas_visible_sv_msk;
    uint16_t     xtra_start_gps_week;
    uint16_t     xtra_start_gps_minutes;
    uint16_t     xtra_valid_duration_hours;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_SLQSGetGPSStateInfo_t;

/**
 * Queries the MSM GPS server for receiver state information pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSGetGPSStateInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Queries the MSM GPS server for receiver state information unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSGetGPSStateInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSGetGPSStateInfo_t *pOutput
        );

/**
 *  Parameters to Set state of positioning method for a device.
 *
 *  @param  pXtraTimeState
 *          - XTRA Time Position Method State.
 *          - Values:
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pXtraDataState
 *          - XTRA Data Position Method State.
 *          - Values:
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  Latitude
 *          - WiFi Position Method State
 *          - Values:
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 */
typedef struct
{
    uint8_t      *pXtraTimeState;
    uint8_t      *pXtraDataState ;
    uint8_t      *pWifiState;
} pack_pds_SLQSSetPositionMethodState_t;

typedef unpack_result_t unpack_pds_SLQSSetPositionMethodState_t;

/**
 * Sets the state of positioning methods for the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SLQSSetPositionMethodState(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_pds_SLQSSetPositionMethodState_t *pReqParam
        );

/**
 * Sets the state of positioning methods for the device unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SLQSSetPositionMethodState(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SLQSSetPositionMethodState_t *pOutput
        );

/**
 *  Structure contain the parameter for the automatic tracking configuration for the NMEA COM port.
 *
 *  @param  posDataNmea
 *          - Report new position data in NMEA format.
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 *
 *  @param  rptPosData
 *          - Report new position data in raw format.
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 */
typedef struct{
    uint8_t posDataNmea;
    uint8_t rptPosData;
} pack_pds_SetEventReportCallback_t;

typedef unpack_result_t unpack_pds_SetEventReportCallback_t;

/**
 * Enables/disables the PDS Event report callback function pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReq qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_pds_SetEventReportCallback(
        pack_qmi_t *pCtx,
        uint8_t *pReq,
        uint16_t *pLen,
        pack_pds_SetEventReportCallback_t *pReqParam
        );

/**
 * Enables/disables the PDS Event report callback function unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetEventReportCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_pds_SetEventReportCallback_t *pOutput
        );

/**
 *  Structure contain the parameter for the Set event report Indication.
 *  Please check has_<Param_Name> field for presence of optional parameters
 *
 *  @param  PositionDataNMEA
 *          - Position Data NMEA.
 *          - String containing the position data in NMEA sentence format (maximum 200 bytes).
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  dLongitude
 *          - Longitude position referenced to the WGS-84 reference ellipsoid, counting positive
 *            angles east of the Greenwich Meridian and negative angles west of the Greenwich Meridian.
 *          - Units - Decimal degrees
 *          - Range - -180 to +180
 *          - Note - Value is in double float format
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  dLatitude
 *          - Latitude position referenced to the WGS-84 reference ellipsoid,
 *            counting positive angles north of the equator and negative angles
 *            south of the equator.
 *          - Units: Decimal degrees
 *          - Range: -90 to +90
 *          - Note - Value is in double float format
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  session_status
 *        - Position Session Status
 *        - Status of current session.
 *        - Values
 *           - 0x00 - Success
 *           - 0x01 - In progress
 *           - 0x02 - General failure
 *           - 0x03 - Timeout
 *           - 0x04 - User ended the session
 *           - 0x05 - Bad parameter
 *           - 0x06 - Phone is offline
 *           - 0x07 - Engine is locked
 *           - 0x08 - E911 session in progress
 *        - Bit to check in ParamPresenceMask - <B>18</B>
 *        - Note - This TLV is always sent while a fix is being generated or if it halts due to an error.
 *                 The Parsed Position Data TLV (0x13) is included if the session status is Success (0x00) or In Progress (0x01);
 *                 otherwise this TLV is passed alone.
 *
 *  @param  posSrc
 *          - Position Source
 *          - This TLV is always sent with the Parsed Position Data TLV (0x13) when the
 *            latitude/longitude is marked as valid. Source of the position (bitmask).
 *          - Values
 *            - 0x00000001 - GPS
 *            - 0x00000002 - Cell ID
 *            - 0x00000004 - GLONASS
 *            - 0x00000008 - Network
 *            - 0x00000010 - External position injection
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t  has_PositionDataNMEA;
    char     PositionDataNMEA[200];
    uint8_t  has_dLongitude;
    double   dLongitude;
    uint8_t  has_dLatitude;
    double   dLatitude;
    uint8_t  has_SessionStatus;
    uint8_t  SessionStatus;
    uint8_t  has_posSrc;
    uint32_t posSrc;
    swi_uint256_t  ParamPresenceMask;
} unpack_pds_SetEventReport_Ind_t;

/**
 * PDS Event Report Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetEventReport_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_pds_SetEventReport_Ind_t *pOutput
);

/**
 *  Structure contain the parameter for the Set PDS State Indication.
 *
 *  @param  EnabledStatus
 *          - GPS service state.
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  TrackingStatus
 *          - Tracking session state.
 *          - Values
 *            - 0x00 - Unknown
 *            - 0x01 - Inactive
 *            - 0x02 - Active
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint16_t  TlvPresent;
    uint32_t EnabledStatus;
    uint32_t TrackingStatus;
    swi_uint256_t  ParamPresenceMask;
}unpack_pds_SetPdsState_Ind_t;

/**
 * PDS State Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_pds_SetPdsState_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_pds_SetPdsState_Ind_t *pOutput
);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
