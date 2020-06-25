/**
 * \ingroup liteqmi
 *
 * \file swiloc.h
 */
#ifndef __LITEQMI_SWILOC_H__
#define __LITEQMI_SWILOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

/**
 * This structure contains SWI LOC Get Auto Start setting
 *
 * \param function
 *        - Setting to indicate when modem should start an automatic GNSS fix
 *          - 0 - disabled
 *          - 1 - At bootup
 *          - 2 - When NMEA port is opened
 *        - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * \param function_reported
 *        - 0 - not reported by modem
 *        - l - reported by modem
 *
 * \param fix_type
 *        - Type of GNSS fix:
 *          - 1 - Default Engine mode
 *          - 2 - MS-Based
 *          - 3 - MS-Assisted
 *          - 4 - Standalone
 *        - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * \param fix_type_reported
 *        - 0 - not reported by modem
 *        - l - reported by modem
 *
 * \param max_time
 *        - Maximum time allowed for the receiver to get a fix in seconds
 *        - Valid range: 1-255
 *        - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * \param max_time_reported
 *        - 0 - not reported by modem
 *        - l - reported by modem
 *
 * \param max_dist
 *        - Maximum uncertainty of a fix measured by distance in meters
 *        - Valid range: 1 - 4294967280
 *        - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * \param max_dist_reported
 *        - 0 - not reported by modem
 *        - l - reported by modem
 *
 * \param fix_rate
 *        - Time between fixes in seconds
 *        - Valid range: 1--65535
 *        - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * \param fix_rate_reported
 *        - 0 - not reported by modem
 *        - l - reported by modem
 */
typedef struct {
    uint8_t       function;
    int           function_reported;
    uint8_t       fix_type;
    int           fix_type_reported;
    uint8_t       max_time;
    int           max_time_reported;
    uint32_t      max_dist;
    int           max_dist_reported;
    uint32_t      fix_rate;
    int           fix_rate_reported;
    swi_uint256_t ParamPresenceMask;
} unpack_swiloc_SwiLocGetAutoStart_t;

/**
 * Get Auto Start pack
 * @param[in] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swiloc_SwiLocGetAutoStart(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * Get Auto Start unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiloc_SwiLocGetAutoStart(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swiloc_SwiLocGetAutoStart_t *pOutput);

/**
 * This structure contains SWI LOC Get Auto Start setting
 *
 * \param function
 *        - Setting to indicate when modem should start an automatic GNSS fix
 *          - 0 - disabled
 *          - 1 - At bootup
 *          - 2 - When NMEA port is opened
 *
 * \param set_function
 *        - 0 - do not set to modem
 *        - l - set to modem
 *
 * \param fix_type
 *        - Type of GNSS fix:
 *          - 1 - Default Engine mode
 *          - 2 - MS-Based
 *          - 3 - MS-Assisted
 *          - 4 - Standalone
 *
 * \param set_fix_type
 *        - 0 - do not set to modem
 *        - l - set to modem
 *
 * \param max_time
 *        - Maximum time allowed for the receiver to get a fix in seconds
 *        - Valid range: 1-255
 *
 * \param set_max_time
 *        - 0 - do not set to modem
 *        - l - set to modem
 *
 * \param max_dist
 *        - Maximum uncertainty of a fix measured by distance in meters
 *        - Valid range: 1 - 4294967280
 *
 * \param set_max_dist
 *        - 0 - do not set to modem
 *        - l - set to modem
 *
 * \param fix_rate
 *        - Time between fixes in seconds
 *        - Valid range: 1--65535
 *
 * \param set_fix_rate
 *        - 0 - do not set to modem
 *        - l - set to modem
 */
typedef struct {
   uint8_t function;
   int set_function;
   uint8_t fix_type;
   int set_fix_type;
   uint8_t max_time;
   int set_max_time;
   uint32_t max_dist;
   int set_max_dist;
   uint32_t fix_rate;
   int set_fix_rate;
} pack_swiloc_SwiLocSetAutoStart_t;

typedef unpack_result_t  unpack_swiloc_SwiLocSetAutoStart_t;

/**
 * Set Auto Start pack
 * @param[in] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swiloc_SwiLocSetAutoStart(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_swiloc_SwiLocSetAutoStart_t *reqArg);

/**
 * Set Auto Start unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiloc_SwiLocSetAutoStart(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swiloc_SwiLocSetAutoStart_t *pOutput);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
