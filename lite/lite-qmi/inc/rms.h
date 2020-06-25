/**
 * \ingroup liteqmi
 *
 * \file rms.h
 */
#include "common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LITEQMI_RMS_H__
#define __LITEQMI_RMS_H__

/**
 *  Unpack get SMS wake parameters
 *  @param  enabled
 *          - SMS wake functionality enabled
 *              - 0 - Disabled
 *              - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *  @param  wake_mask
 *          - SMS wake mask to search for incoming messages
 *            (only relevant when enabled)
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t enabled;
    uint32_t wake_mask;
    swi_uint256_t  ParamPresenceMask;
} unpack_rms_GetSMSWake_t;

/**
 *  Pack set SMS wake parameters
 *  @param  enabled
 *          - SMS wake functionality enabled
 *              - 0 - Disabled
 *              - 1 - Enabled
 *  @param  wake_mask
 *          - SMS wake mask to search for incoming messages
 *            (only relevant when enabled)
 */
typedef struct{
    uint32_t enabled;
    uint32_t wake_mask;
} pack_rms_SetSMSWake_t;

/**
 * Get SMS Wake pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_rms_GetSMSWake(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * Get SMS Wake unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_rms_GetSMSWake(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_rms_GetSMSWake_t *pOutput
        );

/**
 * Set SMS Wake pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_rms_SetSMSWake(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_rms_SetSMSWake_t *reqArg
        );

/**
 *   This structure contains unpack Set SMS Wake.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_rms_SetSMSWake_t;

/**
 * Set SMS Wake unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_rms_SetSMSWake(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_rms_SetSMSWake_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
