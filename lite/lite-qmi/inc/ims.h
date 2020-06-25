/**
 * \ingroup liteqmi
 *
 * \file ims.h
 */
#ifndef __LITEQMI_IMS_H__
#define __LITEQMI_IMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define MAX_NAME_LEN 255

/**
 *  This structure contains the SLQSSetSIPConfig pack parameters.
 *
 *  @param  pSIPLocalPort
 *          - Primary call session control function SIP port number
 *
 *  @param  pTimerSIPReg
 *          - Initial SIP registration duration from the User equipment,
 *            in seconds
 *
 *  @param  pSubscribeTimer
 *          - Duration of the subscription by the UE for IMS registration
 *            notifications, in seconds
 *
 *  @param  pTimerT1
 *          - RTT estimate, in milliseconds
 *
 *  @param  pTimerT2
 *          - The maximum retransmit interval for non-invite requests and
 *            invite responses, in milliseconds
 *
 *  @param  pTimerTf
 *          - Non-invite transaction timeout timer, in milliseconds
 *
 *  @param  pSigCompEnabled
 *          - Sig Comp Status
 *              - TRUE - Enable
 *              - FALSE - Disable
 *
 */
typedef struct
{
    uint16_t *pSIPLocalPort;
    uint32_t *pTimerSIPReg;
    uint32_t *pSubscribeTimer;
    uint32_t *pTimerT1;
    uint32_t *pTimerT2;
    uint32_t *pTimerTf;
    uint8_t  *pSigCompEnabled;
} pack_ims_SLQSSetSIPConfig_t;

/**
 *  This structure contains the SLQSSetSIPConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 */
typedef struct
{
    uint8_t          *pSettingResp;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSSetSIPConfig_t;

/**
 * Sets the IMS Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSSetSIPConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSSetSIPConfig_t *pReqParam
        );

/**
 * Sets the IMS Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSSetSIPConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSetSIPConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSSetRegMgrConfig pack parameters.
 *
 *  @param  pPriCSCFPort
 *          - Primary call session control function port
 *
 *  @param  pCSCFPortNameLen
 *          - Length of the CSCF Port name parameter to follow
 *
 *  @param  pCSCFPortName
 *          - Call Session control port, fully qualified domain name
 *          - Length of this string must be specified in
 *            pCSCFPortNameLen parameter
 *
 *  @param  pIMSTestMode
 *          - IMS Test mode Enabled.
 *              - TRUE - Enable, no IMS registration
 *              - FALSE - Disable, IMS registration is initiated
 *
 */
typedef struct
{
    uint16_t *pPriCSCFPort;
    uint8_t  *pCSCFPortNameLen;
    uint8_t  *pCSCFPortName;
    uint8_t  *pIMSTestMode;
} pack_ims_SLQSSetRegMgrConfig_t;

/**
 *  This structure contains the SLQSSetRegMgrConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 */
typedef struct
{
    uint8_t          *pSettingResp;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSSetRegMgrConfig_t;

/**
 * Sets the IMS registration manager configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSSetRegMgrConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSSetRegMgrConfig_t *pReqParam
        );

/**
 * Sets the IMS registration manager configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSSetRegMgrConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSetRegMgrConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSSetIMSSMSConfig pack parameters.
 *
 *  @param  pSMSFormat
 *          - SMS format
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  @param  pSMSOverIPNwInd
 *          - SMS over IP Network Indication Flag
 *              - TRUE - Turn on mobile-originated SMS
 *              - FALSE - Turn off mobile-originated SMS
 *
 *  @param  pPhoneCtxtURILen
 *          - Length of Phone context Universal Resource Identifier to follow
 *
 *  @param  pPhoneCtxtURI
 *          - Phone context universal resource identifier
 *          - Length of this string must be specified in
 *            pPhoneCtxtURILen parameter
 *
 */
typedef struct
{
    uint8_t *pSMSFormat;
    uint8_t *pSMSOverIPNwInd;
    uint8_t *pPhoneCtxtURILen;
    uint8_t *pPhoneCtxtURI;
} pack_ims_SLQSSetIMSSMSConfig_t;

/**
 *  This structure contains the SLQSSetIMSSMSConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 */
typedef struct
{
    uint8_t          *pSettingResp;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSSetIMSSMSConfig_t;

/**
 * Sets the IMS SMS configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSSetIMSSMSConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSSetIMSSMSConfig_t *pReqParam
        );

/**
 * Sets the IMS SMS configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSSetIMSSMSConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSetIMSSMSConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSSetIMSUserConfig pack parameters.
 *
 *  @param  pIMSDomainLen
 *          - Length of IMS Domain Name to follow
 *
 *  @param  pIMSDomain
 *          - IMS domain name
 *
 */
typedef struct
{
    uint8_t *pIMSDomainLen;
    uint8_t *pIMSDomain;
} pack_ims_SLQSSetIMSUserConfig_t;

/**
 *  This structure contains the SLQSSetIMSUserConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 */
typedef struct
{
    uint8_t          *pSettingResp;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSSetIMSUserConfig_t;

/**
 * Sets the IMS user configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSSetIMSUserConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSSetIMSUserConfig_t *pReqParam
        );

/**
 * Sets the IMS user configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSSetIMSUserConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSetIMSUserConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSSetIMSVoIPConfig pack parameters.
 *
 *  @param  pSessionExpiryTimer
 *          - Session duration, in seconds
 *
 *  @param  pMinSessionExpiryTimer
 *          - Minimum allowed value for session expiry timer,
 *            in seconds
 *
 *  @param  pAmrWbEnable
 *          - Flag to enable/disable Adaptive Multirate Codec(AMR)
 *            WideBand(WB) audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  @param  pScrAmrEnable
 *          - Flag to enable/disable Source Control Rate(SCR)
 *            for AMR NarrowBand (NB)
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  @param  pScrAmrWbEnable
 *          - Flag to enable/disable SCR for AMR WB Audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *
 *  @param  pAmrMode
 *          - BitMask for AMR NB modes allowed
 *          - Values:
 *              - 0x1 - 4.75 kbps
 *              - 0x2 - 5.15 kbps
 *              - 0x4 - 5.9 kbps
 *              - 0x8 - 6.17 kbps
 *              - 0x10 - 7.4 kbps
 *              - 0x20 - 7.95 kbps
 *              - 0x40 - 10.2 kbps
 *              - 0x80 - 12.2 kbps
 *
 *  @param  pAmrWBMode
 *          - BitMask for AMR WB modes allowed
 *          - Values:
 *              - 0x1 - 6.60 kbps
 *              - 0x2 - 8.85 kbps
 *              - 0x4 - 12.65 kbps
 *              - 0x8 - 14.25 kbps
 *              - 0x10 - 15.85 kbps
 *              - 0x20 - 18.25 kbps
 *              - 0x40 - 19.85 kbps
 *              - 0x80 - 23.05 kbps
 *              - 0x100 - 23.85 kbps
 *
 *  @param  pAmrOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR NB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  @param  pAmrWBOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR WB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *
 *  @param  pRingingTimer
 *          - Duration of ringing timer, in seconds. The ringing timer starts
 *            on the ringing event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  @param  pRingBackTimer
 *          - Duration of ringback timer, in seconds. The ringback timer starts
 *            on the ringback event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *
 *  @param  pRTPRTCPInactTimer
 *          - Duration of RTP/RTCP inactivity timer, in seconds. If no
 *            RTP/RTCP packet is received prior to the expiry
 *            of this timer, the call is disconnected.
 */
typedef struct
{
    uint16_t *pSessionExpiryTimer;
    uint16_t *pMinSessionExpiryTimer;
    uint8_t  *pAmrWbEnable;
    uint8_t  *pScrAmrEnable;
    uint8_t  *pScrAmrWbEnable;
    uint8_t  *pAmrMode;
    uint16_t *pAmrWBMode;
    uint8_t  *pAmrOctetAligned;
    uint8_t  *pAmrWBOctetAligned;
    uint16_t *pRingingTimer;
    uint16_t *pRingBackTimer;
    uint16_t *pRTPRTCPInactTimer;
} pack_ims_SLQSSetIMSVoIPConfig_t;

/**
 *  This structure contains the SLQSSetIMSVoIPConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings standard response type. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 */
typedef struct
{
    uint8_t          *pSettingResp;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSSetIMSVoIPConfig_t;

/**
 * Sets the IMS Voice over Internet Protocol (VoIP) configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSSetIMSVoIPConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSSetIMSVoIPConfig_t *pReqParam
        );

/**
 * Sets the IMS Voice over Internet Protocol (VoIP) configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSSetIMSVoIPConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSetIMSVoIPConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSGetSIPConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings Response
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pSIPLocalPort
 *          - Primary call session control function SIP port number
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pTimerSIPReg
 *          - Initial SIP registration duration from the User equipment,
 *            in seconds
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pSubscribeTimer
 *          - Duration of the subscription by the UE for IMS registration
 *            notifications, in seconds
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pTimerT1
 *          - RTT estimate, in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pTimerT2
 *          - The maximum retransmit interval for non-invite requests and
 *            invite responses, in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pTimerTf
 *          - Non-invite transaction timeout timer, in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pSigCompEnabled
 *          - Sig Comp Status
 *              - TRUE - Sig Comp Enabled
 *              - FALSE - Sig Comp Disabled
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 */
typedef struct
{
    uint8_t   *pSettingResp;
    uint16_t  *pSIPLocalPort;
    uint32_t  *pTimerSIPReg;
    uint32_t  *pSubscribeTimer;
    uint32_t  *pTimerT1;
    uint32_t  *pTimerT2;
    uint32_t  *pTimerTf;
    uint8_t   *pSigCompEnabled;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSGetSIPConfig_t;

/**
 * Retrieves the Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSGetSIPConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen
        );

/**
 * Retrieves the Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSGetSIPConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSGetSIPConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSGetRegMgrConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings Response
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPCSCFPort
 *          - Proxy call session control function port
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pPriCSCFPortNameLen(IN/OUT)
 *          - Size in bytes assigned to the primary CSCF Port name
 *            parameter to follow
 *
 *  @param  pPriCSCFPortName
 *          - Call Session control port, fully qualified domain name
 *          - Length of this string must be specified in
 *            pPriCSCFPortNameLen parameter
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pIMSTestMode
 *          - IMS Test mode Enabled.
 *              - TRUE - Enabled
 *              - FALSE - Disabled
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @note   pPriCSCFPortNameLen must be set to a valid value during API call
 *          to retrieve pPriCSCFPortName.
 */
typedef struct
{
    uint8_t  *pSettingResp;
    uint16_t *pPCSCFPort;
    uint8_t  *pPriCSCFPortNameLen;
    uint8_t  *pPriCSCFPortName;
    uint8_t  *pIMSTestMode;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSGetRegMgrConfig_t;

/**
 * Retrieves the Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSGetRegMgrConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen
        );

/**
 * Retrieves the Session Initiation Protocol(SIP) configuration
 * parameters for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSGetRegMgrConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSGetRegMgrConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSGetIMSSMSConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings Response
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pSMSFormat
 *          - SMS format
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pSMSOverIPNwInd
 *          - SMS over IP Network Indication Flag
 *              - TRUE - Turn on mobile-originated SMS
 *              - FALSE - Turn off mobile-originated SMS
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pPhoneCtxtURILen[IN/OUT]
 *          - Size in bytes assigned to the Phone context Universal Resource
 *            Identifier to follow
 *
 *  @param  pPhoneCtxtURI
 *          - Phone context universal resource identifier
 *          - Length of this string must be specified in
 *            pPhoneCtxtURILen parameter
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 */
typedef struct
{
    uint8_t *pSettingResp;
    uint8_t *pSMSFormat;
    uint8_t *pSMSOverIPNwInd;
    uint8_t *pPhoneCtxtURILen;
    uint8_t *pPhoneCtxtURI;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSGetIMSSMSConfig_t;

/**
 * Retrieves the SMS configuration parameters
 * for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSGetIMSSMSConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen
        );

/**
 * Retrieves the SMS configuration parameters
 * for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSGetIMSSMSConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSGetIMSSMSConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSGetIMSUserConfig pack parameters.
 *
 *  @param  pSettingResp
 *          - Settings Response
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pIMSDomainLen[IN/OUT]
 *          - Length of IMS Domain Name to follow
 *
 *  @param  pIMSDomain
 *          - IMS domain name
 *          - Length of this string must be specified in
 *            pIMSDomainLen parameter
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 */
typedef struct
{
    uint8_t *pSettingResp;
    uint8_t *pIMSDomainLen;
    uint8_t *pIMSDomain;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSGetIMSUserConfig_t;

/**
 * Retrieves the IMS User configuration parameters
 * for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSGetIMSUserConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen
        );

/**
 * Retrieves the IMS User configuration parameters
 * for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSGetIMSUserConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSGetIMSUserConfig_t *pOutput
        );

/**
 *  This structure contains the SLQSGetIMSVoIPConfig unpack parameters.
 *
 *  @param  pSettingResp
 *          - Settings Response. A settings specific error
 *            code is returned when the standard response error type is
 *            QMI_ERR_CAUSE_CODE
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pSessionExpiryTimer
 *          - Session duration, in seconds
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pMinSessionExpiryTimer
 *          - Minimum allowed value for session expiry timer,
 *            in seconds
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pAmrWbEnable
 *          - Flag to enable/disable Adaptive Multirate Codec(AMR)
 *            WideBand(WB) audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pScrAmrEnable
 *          - Flag to enable/disable Source Control Rate(SCR)
 *            for AMR NarrowBand (NB)
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pScrAmrWbEnable
 *          - Flag to enable/disable SCR for AMR WB Audio
 *          - Values:
 *              - True  - Enable
 *              - False - Disable
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pAmrMode
 *          - BitMask for AMR NB modes allowed
 *          - Values:
 *              - 0x1 - 4.75 kbps
 *              - 0x2 - 5.15 kbps
 *              - 0x4 - 5.9 kbps
 *              - 0x8 - 6.17 kbps
 *              - 0x10 - 7.4 kbps
 *              - 0x20 - 7.95 kbps
 *              - 0x40 - 10.2 kbps
 *              - 0x80 - 12.2 kbps
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pAmrWBMode
 *          - BitMask for AMR WB modes allowed
 *          - Values:
 *              - 0x1 - 6.60 kbps
 *              - 0x2 - 8.85 kbps
 *              - 0x4 - 12.65 kbps
 *              - 0x8 - 14.25 kbps
 *              - 0x10 - 15.85 kbps
 *              - 0x20 - 18.25 kbps
 *              - 0x40 - 19.85 kbps
 *              - 0x80 - 23.05 kbps
 *              - 0x100 - 23.85 kbps
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pAmrOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR NB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pAmrWBOctetAligned
 *          - Flag to indicate if the octet is aligned for AMR WB Audio
 *          - Values:
 *              - True  - Aligned
 *              - False - Not aligned, Bandwidth Efficient mode
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pRingingTimer
 *          - Duration of ringing timer, in seconds. The ringing timer starts
 *            on the ringing event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pRingBackTimer
 *          - Duration of ringback timer, in seconds. The ringback timer starts
 *            on the ringback event. If the call is not answered within the
 *            duration of this timer, the call is disconnected.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pRTPRTCPInactTimer
 *          - Duration of RTP/RTCP inactivity timer, in seconds. If no
 *            RTP/RTCP packet is received prior to the expiry
 *            of this timer, the call is disconnected.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 */
typedef struct
{
    uint8_t  *pSettingResp;
    uint16_t *pSessionExpiryTimer;
    uint16_t *pMinSessionExpiryTimer;
    uint8_t  *pAmrWbEnable;
    uint8_t  *pScrAmrEnable;
    uint8_t  *pScrAmrWbEnable;
    uint8_t  *pAmrMode;
    uint16_t *pAmrWBMode;
    uint8_t  *pAmrOctetAligned;
    uint8_t  *pAmrWBOctetAligned;
    uint16_t *pRingingTimer;
    uint16_t *pRingBackTimer;
    uint16_t *pRTPRTCPInactTimer;
    swi_uint256_t    ParamPresenceMask;
} unpack_ims_SLQSGetIMSVoIPConfig_t;

/**
 * Retrieves the IMS VoIP configuration parameters
 * for the requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 */
int pack_ims_SLQSGetIMSVoIPConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen
        );

/**
 * Retrieves the IMS VoIP configuration parameters
 * for the requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSGetIMSVoIPConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSGetIMSVoIPConfig_t *pOutput
        );

/**
 *  This structure contains parameters of IMS Config Indication Register
 *
 *  @param  pSIPConfigEvents(optional)
 *          - Registration Indication For SIP Configuration Events.
 *          - When this registration is enabled, the device learns of SIP
 *            config events via the QMI_IMS_SIP_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pRegMgrConfigEvents(optional)
 *          - Registration Indication For Registration Manager Configuration
 *            Events.
 *          - When this registration is enabled, the device learns of Reg Mgr
 *            config events via the QMI_IMS_REG_MGR_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSMSConfigEvents(optional)
 *          - Registration Indication For SMS Configuration Events.
 *          - When this registration is enabled, the device learns of SMS
 *            config events via the QMI_IMS_SMS_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pUserConfigEvents(optional)
 *          - Registration Indication For User Configuration Events.
 *          - When this registration is enabled, the device learns of user
 *            config events via the QMI_IMS_USER_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pVoIPConfigEvents(optional)
 *          - Registration Indication For VoIP Configuration Events.
 *          - When this registration is enabled, the device learns of VOIP
 *            config events via the QMI_IMS_VOIP_CONFIG_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @note   One of the optional parameter is mandatory to be present in the
 *          request.
 */
typedef struct
{
    uint8_t *pSIPConfigEvents;
    uint8_t *pRegMgrConfigEvents;
    uint8_t *pSMSConfigEvents;
    uint8_t *pUserConfigEvents;
    uint8_t *pVoIPConfigEvents;
} pack_ims_SLQSImsConfigIndicationRegister_t;

typedef unpack_result_t unpack_ims_SLQSImsConfigIndicationRegister_t;
/**
 * Sets the registration state for different QMI_IMS indications for the
 * requesting control point pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC73xx, MC74xx and EM74xx
 *
 *         - This function is used by a device to register/deregister for different
 *           QMI IMS indications.
 *         - The device's registration state variables that control registration
 *           for indications will be modified to reflect the settings indicated in the
 *           request message.
 *         - At least one optional parameter must be present in the request.
 *
 */
int pack_ims_SLQSImsConfigIndicationRegister(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_ims_SLQSImsConfigIndicationRegister_t *pReqParam
        );

/**
 * Sets the registration state for different QMI_IMS indications for the
 * requesting control point unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_ims_SLQSImsConfigIndicationRegister(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_ims_SLQSImsConfigIndicationRegister_t *pOutput
        );

/**
 *  This structure hold parameters about  primary CSCF port info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param priCSCFPort
 *          - primary CSCF port number
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t priCSCFPort;
} ims_PCSCFPortInfo;

/**
 *  This structure hold parameters about CSCF port info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param cscfPortName
 *          -CSCF port name string
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t cscfPortName[MAX_NAME_LEN];
} ims_CSCFPortNameInfo;

/**
 *  This structure hold parameters about ims test mode info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param imsTestMode
 *          -1- if IMS test mode is enabled
 *          -0- if IMS test mode is disabled
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t imsTestMode;
} ims_IMSTestModeInfo;

/**
 *  Structure used to store Reg Mgr Config Indication Parameters. 
 *  @param  PCTlv
 *          - Primary PCSCF Port info
 *          - See @ref ims_PCSCFPortInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  PNTlv
 *          - CSCF Port info
 *          - see @ref ims_CSCFPortNameInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param TMTlv
 *          - IMS test mode info
 *          - see @ref ims_IMSTestModeInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @note:    None
 */
typedef struct{
    ims_PCSCFPortInfo    PCTlv;
    ims_CSCFPortNameInfo PNTlv;
    ims_IMSTestModeInfo  TMTlv;
    swi_uint256_t  ParamPresenceMask;
} unpack_ims_SLQSRegMgrCfgCallBack_ind_t;

 /**
 * Unpack indication about IMS registration manager configuration
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_ims_SLQSRegMgrCfgCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_ims_SLQSRegMgrCfgCallBack_ind_t *pOutput
        );

/**
 *  This structure stores information about SIP port info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param SIPLocalPort
 *          -SIP local port
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t SIPLocalPort;
} ims_SIPPortInfo;

/**
 *  This structure stores information about SIP registration timer. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param tmrSIPRegn
 *          -Initial SIP registration duration, in seconds
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t tmrSIPRegn;
} ims_SIPRegnTmrInfo;

/**
 *  This structure stores information about subscriber timer. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param subscrTmr
 *          -Duration, in seconds, of the subscription by the UE for 
 *           IMS registration notifications.
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t subscrTmr;
} ims_SubscrTmrInfo;

/**
 *  This structure stores information about timer T1. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param tmrT1
 *          -RTT estimate, in milliseconds.
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t tmrT1;
} ims_TmrT1Info;

/**
 *  This structure stores information about timer T2. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param tmrT2
 *          -Maximum retransmit interval, in msec, for non-invite 
 *           requests and invite responses.
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t tmrT2;
} ims_TmrT2Info;


/**
 *  This structure store information about timer TF. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param tmrTf
 *          -Non-invite transaction timeout timer, in msec
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t tmrTf;
} ims_TmrTfInfo;

/**
 *  This structure hold parameters about SigComp status. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param SigCompEn
 *          -1- if SigComp is enabled
 *          -0- if IMS SigComp is disabled
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t SigCompEn;
} ims_SigCompEnInfo;

/**
 *  Structure used to store Reg Mgr Config Indication Parameters. 
 *  @param  SPTlv
 *          - SIP port tlv
 *          - See @ref ims_SIPPortInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  SRTlv
 *          - SIP Registration Timer tlv
 *          - see @ref ims_SIPRegnTmrInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param STTlv
 *          - Subscriber timer tlv
 *          - see @ref ims_SubscrTmrInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param TT1Tlv
 *          - Timer T1 tlv
 *          - see @ref ims_TmrT1Info for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param TT2Tlv
 *          - Timer T2 tlv
 *          - see @ref ims_TmrT2Info for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param TTfTlv
 *          - Timer Tf tlv
 *          - see @ref ims_TmrTfInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param SCTlv
 *          - SigComp Status Tlv
 *          - see @ref ims_SigCompEnInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 * @note:    None
 */
typedef struct{
    ims_SIPPortInfo    SPTlv;
    ims_SIPRegnTmrInfo SRTlv;
    ims_SubscrTmrInfo  STTlv;
    ims_TmrT1Info     TT1Tlv;
    ims_TmrT2Info     TT2Tlv;
    ims_TmrTfInfo      TTfTlv;
    ims_SigCompEnInfo  SCTlv;
    swi_uint256_t      ParamPresenceMask;
} unpack_ims_SLQSSIPCfgCallBack_ind_t;

 /**
 * Unpack indication about SIP configuration info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_ims_SLQSSIPCfgCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSIPCfgCallBack_ind_t *pOutput
        );

/**
 *  This structure hold parameters about  SMS format info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param smsFormat 
 *          - 0- IMS_SETTINGS_SMS_FORMAT_3GPP2
 *          - 1- IMS_SETTINGS_SMS_FORMAT_3GPP
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t smsFormat;
} ims_SMSFmtInfo;

/**
 *  This structure hold parameters about SMS over IP network. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param smsoIPNW
 *          -1- if MO SMS turned on
 *          -0- if MO SMS turned off
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t smsoIPNW;
} ims_SMSoIPNwInfo;


/**
 *  This structure hold parameters about Phone context URI info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param PhCtxtURI
 *          -Phone context URI string
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t PhCtxtURI[MAX_NAME_LEN];
} ims_PhCtxtURIInfo;

/**
 *  Structure used to store SMS Config Indication Parameters. 
 *  @param  SFTlv
 *          - SMS format info
 *          - See @ref ims_SMSFmtInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  SINTlv
 *          - SMS over IP network info
 *          - see @ref ims_SMSoIPNwInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param PCURTlv
 *          - Phone context URI info
 *          - see @ref ims_PhCtxtURIInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @note:    None
 */
typedef struct{
    ims_SMSFmtInfo     SFTlv;
    ims_SMSoIPNwInfo   SINTlv;
    ims_PhCtxtURIInfo  PCURTlv;
    swi_uint256_t      ParamPresenceMask;
} unpack_ims_SLQSSMSCfgCallBack_ind_t;

 /**
 * Unpack indication about SMS configuration info for IMS
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_ims_SLQSSMSCfgCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_ims_SLQSSMSCfgCallBack_ind_t *pOutput
        );

/**
 *  This structure hold parameters about IMS domain info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param imsDomainName
 *          -IMS domain name string
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t imsDomainName[MAX_NAME_LEN];
} ims_IMSDomainInfo;

/**
 *  Structure used to store SMS Config Indication Parameters. 
 *  @param  IDTlv
 *          - IMS domain info
 *          - See @ref ims_IMSDomainInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @note:    None
 */
typedef struct{
    ims_IMSDomainInfo    IDTlv;
    swi_uint256_t        ParamPresenceMask;
} unpack_ims_SLQSUserCfgCallBack_ind_t;

 /**
 * Unpack indication about User configuration info for IMS
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_ims_SLQSUserCfgCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_ims_SLQSUserCfgCallBack_ind_t *pOutput
        );


/**
 *  This structure store information about session duration info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param sessExp
 *          - session duration in seconds
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t sessExp;
} ims_SessDurInfo;


/**
 *  This structure store information about minimum session expiry info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param minSessExp
 *          - minimum session expiry in seconds
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t minSessExp;
} ims_MinSessExpInfo;

/**
 *  This structure store information about Enable AMR WB. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param amrWBEnable
 *          -1- Enable
 *          -0- Disable
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t amrWBEnable;
} ims_EnabAMRWBInfo;

/**
 *  This structure store information about Enable SCR AMR . 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param scrAmrEnable
 *          -1- Enable
 *          -0- Disable
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t scrAmrEnable;
} ims_EnabSCRAMRInfo;

/**
 *  This structure store information about Enable SCR AMR WB. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param scrAmrWBEnable
 *          -1- Enable
 *          -0- Disable
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t scrAmrWBEnable;
} ims_EnabSCRAMRWBInfo;


/**
 *  This structure store information about AMR NB mode. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param amrMode
 *          Bitmask indicating AMR modes.
 *           Values:
 *           - 0x1 - 4.75 kbps
 *           - 0x2 - 5.15 kbps
 *           - 0x4 - 5.9 kbps
 *           - 0x8 - 6.17 kbps
 *           - 0x10 - 7.4 kbps
 *           - 0x20 - 7.95 kbps
 *           - 0x40 - 10.2 kbps
 *           - 0x80 - 12.2 kbps
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t amrMode;
} ims_AMRModeInfo;


/**
 *  This structure store information about AMR WB mode. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param amrWBMode
 *          Bitmask indicating AMR WB modes.
 *           Values:
 *           - 0x1 - 6.60 kbps
 *           - 0x2 - 8.85 kbps
 *           - 0x4 - 12.65 kbps
 *           - 0x8 - 14.25 kbps
 *           - 0x10 - 15.85 kbps
 *           - 0x20 - 18.25 kbps
 *           - 0x40 - 19.85 kbps
 *           - 0x80 - 23.05 kbps
 *           - 0x100 - 23.85 kbps
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t amrWBMode;
} ims_AMRWBModeInfo;


/**
 *  This structure store information about AMR NB octet aligned. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param amrOctAlgn
 *          -1- if octet aligned
 *          -0- if octed not aligned
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t amrOctAlgn;
} ims_AMROctAlgnInfo;


/**
 *  This structure store information about AMR WB octet aligned. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param amrWBOctAlgn
 *          -1- if octet aligned
 *          -0- if octed not aligned
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t amrWBOctAlgn;
} ims_AMRWBOctAlgnInfo;

/**
 *  This structure hold parameters about Ring timer. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param RingTmr
 *          - Duration, in seconds, of the Ring timer
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t RingTmr;
} ims_RngTmrInfo;

/**
 *  This structure hold parameters about Ring back timer. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param RingBkTmr
 *          - Duration, in seconds, of the Ringback timer
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t RingBkTmr;
} ims_RngBkTmrInfo;

/**
 *  This structure hold parameters about RTP/RTCP timer. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param InactTmr
 *          - Duration, in seconds, of the RTP/RTCP inactivity timer
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t InactTmr;
} ims_RTPRTCPInactTmrDurInfo;

/**
 *  Structure used to store VOIP Config Indication Parameters. 
 *  @param  SDTlv
 *          - Session Duration tlv
 *          - See @ref ims_SessDurInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  MSETlv
 *          - Minimum Session Timer tlv
 *          - see @ref ims_MinSessExpInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param EAWTlv
 *          - Enable AMR WB tlv
 *          - see @ref ims_EnabAMRWBInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param ESATlv
 *          - Enable SCR AMR NB tlv
 *          - see @ref ims_EnabSCRAMRInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param ESAWTlv
 *          - Enable SCR AMR WB tlv
 *          - see @ref ims_EnabSCRAMRWBInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param AMTlv
 *          - AMR NB Mode tlv
 *          - see @ref ims_AMRModeInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param AWMTlv
 *          - AMR WB Mode Tlv
 *          - see @ref ims_AMRWBModeInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param AOATlv
 *          - AMR NB Octet Aligned tlv
 *          - see @ref ims_AMROctAlgnInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param AWOATlv
 *          - AMR WB Octet Aligned tlv
 *          - see @ref ims_AMRWBOctAlgnInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param RTTlv
 *          - Ringing Timer Duration tlv
 *          - see @ref ims_RngTmrInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param RBTTlv
 *          - Ringback Duration tlv
 *          - see @ref ims_RngBkTmrInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param RTIDTlv
 *          - RTP/RTCP Inactivity Timer Duration Tlv
 *          - see @ref ims_RTPRTCPInactTmrDurInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 * @note:    None
 */
typedef struct{
    ims_SessDurInfo            SDTlv;
    ims_MinSessExpInfo         MSETlv;
    ims_EnabAMRWBInfo          EAWTlv;
    ims_EnabSCRAMRInfo         ESATlv;
    ims_EnabSCRAMRWBInfo       ESAWTlv;
    ims_AMRModeInfo            AMTlv;
    ims_AMRWBModeInfo          AWMTlv;
    ims_AMROctAlgnInfo         AOATlv;
    ims_AMRWBOctAlgnInfo       AWOATlv;
    ims_RngTmrInfo             RTTlv;
    ims_RngBkTmrInfo           RBTTlv;
    ims_RTPRTCPInactTmrDurInfo RTIDTlv;
    swi_uint256_t              ParamPresenceMask;
} unpack_ims_SLQSVoIPCfgCallBack_ind_t;

 /**
 * Unpack indication about VOIP configuration info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_ims_SLQSVoIPCfgCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_ims_SLQSVoIPCfgCallBack_ind_t *pOutput
        );


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
