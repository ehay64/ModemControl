/**
 * \ingroup liteqmi
 *
 * \file sar.h
 */
#ifndef __LITEQMI_SAR_H__
#define __LITEQMI_SAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

/**
 * Gets the specified RF SAR state pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sar_SLQSGetRfSarState(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );
/**
 *  This structure contains unpack get RF SAR state parameter.
 *  @param  pSarRFState
 *          - SAR RF State
 *            - QMI_SAR_RF_STATE_DEFAULT
 *            - QMI_SAR_RF_STATE_1
 *            - QMI_SAR_RF_STATE_2
 *            - QMI_SAR_RF_STATE_3
 *            - QMI_SAR_RF_STATE_4
 *            - QMI_SAR_RF_STATE_5
 *            - QMI_SAR_RF_STATE_6
 *            - QMI_SAR_RF_STATE_7
 *            - QMI_SAR_RF_STATE_8
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t *pSarRFState;
    swi_uint256_t  ParamPresenceMask;
} unpack_sar_SLQSGetRfSarState_t;

/**
 * Gets the specified RF SAR state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sar_SLQSGetRfSarState(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_sar_SLQSGetRfSarState_t *pOutput
        );

/**
 *  This sturcture contain sets the specified RF SAR state parameter.
 *  @param  RfSarState
 *          - SAR RF State
 *            - QMI_SAR_RF_STATE_DEFAULT
 *            - QMI_SAR_RF_STATE_1
 *            - QMI_SAR_RF_STATE_2
 *            - QMI_SAR_RF_STATE_3
 *            - QMI_SAR_RF_STATE_4
 *            - QMI_SAR_RF_STATE_5
 *            - QMI_SAR_RF_STATE_6
 *            - QMI_SAR_RF_STATE_7
 *            - QMI_SAR_RF_STATE_8
 */
typedef struct {
    uint32_t RfSarState;
} pack_sar_SLQSSetRfSarState_t;

typedef unpack_result_t  unpack_sar_SLQSSetRfSarState_t;

/**
 * Sets the specified RF SAR state pack
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
int pack_sar_SLQSSetRfSarState(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sar_SLQSSetRfSarState_t *reqArg
        );

/**
 * Sets the specified RF SAR state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sar_SLQSSetRfSarState(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sar_SLQSSetRfSarState_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif//#ifndef  __LITEQMI_SAR_H__

