/**
 * \ingroup liteqmi
 *
 * \file    voice.h
 *
 * \section     Table10 S1
 * \li          3GPP Mobile Radio Interface Layer 3 Specification : 
 *              Core Network Protocols; Stage 3 (Release 7)\n
 * \li          3GPP TS 24.008 V7.0.0 (2005-06)
**/
#ifndef __LIBPACK_VOICE_H__
#define __LIBPACK_VOICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define MAXVOICEUSSDLENGTH          182
#define MAX_VOICE_CALL_NO_LEN        81
#define MAX_VOICE_DESCRIPTION_LENGTH 255
#define VOICE_MAX_NO_OF_CALLS        20
#define BARRING_PASSWORD_LENGTH      4

/**
 *  This structure contains USS Information
 *
 *  @param  ussDCS
 *          - 1 - ASCII coding scheme
 *          - 2 - 8-BIT coding scheme
 *          - 3 - UCS2
 *
 *  @param  ussLen
 *          - Range 1 to 182
 *
 *  @param  ussData
 *          - Data encoded as per the DCS
 *
 */
struct voice_USSInfo
{
    uint8_t ussDCS;
    uint8_t ussLen;
    uint8_t ussData[MAXVOICEUSSDLENGTH];
};

/**
 *  This structure contains pack orginate USSD parameter.
 *  @param  pInfo
 *          - USS information
 *          - See @ref voice_USSInfo for more details
 */
typedef struct {
    uint8_t *pInfo ;
} pack_voice_OriginateUSSD_t;

/**
 * Initiates a USSD operation pack
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
int pack_voice_OriginateUSSD(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_OriginateUSSD_t *reqArg
        );

typedef unpack_result_t  unpack_voice_OriginateUSSD_t;

/**
 * Initiates a USSD operation unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_OriginateUSSD(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_OriginateUSSD_t *pOutput
        );

/**
 *  This structure contains pack anser USSD parameter.
 *  @param  pInfo
 *          - USS information
 *          - See @ref voice_USSInfo for more details
 */
typedef struct {
    uint8_t *pInfo ;
} pack_voice_AnswerUSSD_t;

/**
 * Responds to a USSD request from the network pack
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
int pack_voice_AnswerUSSD(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_AnswerUSSD_t *reqArg
        );

typedef unpack_result_t  unpack_voice_AnswerUSSD_t;

/**
 * Responds to a USSD request from the network unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_AnswerUSSD(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_AnswerUSSD_t *pOutput
        );

/**
 * Cancels an in-progress USSD operation pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *
*/
int pack_voice_CancelUSSD(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen);

typedef unpack_result_t  unpack_voice_CancelUSSD_t;

/**
 * Cancels an in-progress USSD operation unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_CancelUSSD(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_CancelUSSD_t *pOutput
        );

/**
 *  This structure contains User to User Signaling Service Information.
 *
 *  @param  UUSType
 *          - UUS type values are:
 *              - 0x00 - UUS_DATA
 *              - 0x01 - UUS_TYPE1_IMPLICIT
 *              - 0x02 - UUS_TYPE1_REQUIRED
 *              - 0x03 - UUS_TYPE1_NOT_REQUIRED
 *              - 0x04 - UUS_TYPE2_REQUIRED
 *              - 0x05 - UUS_TYPE2_NOT_REQUIRED
 *              - 0x06 - UUS_TYPE3_REQUIRED
 *              - 0x07 - UUS_TYPE3_NOT_REQUIRED
 *              - 0xFF - Not Available
 *
 *  @param  UUSDcs
 *          - UUS data coding scheme values are:
 *              - 0x01 - UUS_DCS_USP
 *              - 0x02 - UUS_DCS_OHLP
 *              - 0x03 - UUS_DCS_X244
 *              - 0x04 - UUS_DCS_SMCF
 *              - 0x05 - UUS_DCS_IA5
 *              - 0x06 - UUS_DCS_RV12RD
 *              - 0x07 - UUS_DCS_Q931UNCCM
 *              - 0xFF - Not Available
 *
 *  @param  UUSDatalen
 *          - Number of sets of the following elements.
 *              - UUSData
 *          - If zero(0) then no further information exists.
 *
 *  @param  UUSData[MAX_VOICE_DESCRIPTION_LENGTH]
 *          - UUS data encoded as per coding scheme
 *
 */
typedef struct
{
    uint8_t UUSType;
    uint8_t UUSDcs;
    uint8_t UUSDatalen;
    uint8_t UUSData[MAX_VOICE_DESCRIPTION_LENGTH];
} voice_UUSInfo;

/**
 *  This structure contains Closed User Group Information
 *
 *  @param  CUGIndex
 *          - Range 0x00... 0x7FFF
 *
 *  @param  SuppPrefCUG
 *          - Suppress preferential CUG
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 *  @param  SuppOA
 *          - Suppress OA subscription option
 *              - 0x00 - FALSE
 *              - 0x01 - TRUE
 *
 */
typedef struct
{
    uint16_t CUGIndex;
    uint8_t  SuppPrefCUG;
    uint8_t  SuppOA;
} voice_CUGInfo;

/**
 *  This structure contains information about the Called Sub Party Addresses.
 *
 *  @param  extBit
 *          - Extension bit.
 *
 *  @param  subAddrType
 *          - Subaddress type.
 *              - 0x00 - NSAP
 *              - 0x01 - USER
 *
 *  @param  oddEvenInd
 *          - Even/odd indicator.
 *              - 0x00 - Even number of address signals
 *              - 0x01 - Odd number of address signals
 *
 *  @param  subAddrLen
 *          - Number of sets of the following elements:
 *              - SubAddress
 *
 *  @param  subAddr[MAX_VOICE_DESCRIPTION_LENGTH]
 *          - Array of the SubAddress in BCD number format.
 *
 */
typedef struct
{
    uint8_t extBit;
    uint8_t subAddrType;
    uint8_t oddEvenInd;
    uint8_t subAddrLen;
    uint8_t subAddr[MAX_VOICE_DESCRIPTION_LENGTH];
} voice_calledPartySubAdd;

/**
 *  This structure contains Voice Call Request Parameters
 *
 *  @param  callNumber[81]
 *          - Number to be dialed in ASCII string, NULL terminated.
 *          - Length Range [1 to 81]
 *
 *  @param  pCallType(optional)
 *          - the type of call to be dialed. CALL_TYPE_VOICE is automatically
 *            selected if this parameter is not provided.
 *            When CALL_TYPE_NON_STD_OTASP is selected, the call is sent as a
 *            nonstandard OTASP call regardless of the digit string
 *            Call type values are:
 *              - 0x00 - CALL_TYPE_VOICE - Voice (automatic selection)
 *              - 0x01 - CALL_TYPE_VOICE_FORCED -
 *                  Avoid modem call classification
 *              - 0x08 - CALL_TYPE_NON_STD_OTASP - Nonstandard OTASP*
 *              - 0x09 - CALL_TYPE_EMERGENCY - Emergency
 *
 *  @param  pCLIRType(optional)
 *          - CLIR type values are:
 *              - 0x01 - CLIR_SUPPRESSION - Suppression
 *              - 0x02 - CLIR_INVOCATION - Invocation
 *
 *  @param  pUUSInFo(optional)
 *          - Pointer to structure of UUSInfo
 *              - See @ref voice_UUSInfo for more information
 *
 *  @param  pCUGInfo(optional)
 *          - Pointer to structure of CUGInfo
 *              - See @ref voice_CUGInfo for more information
 *
 *  @param  pEmergencyCategory(optional)
 *          - Bit mask of emergency number categories. This is only applicable
 *            when the call type is set to Emergency.
 *               - Bit 0 - VOICE_EMER_CAT_POLICE_BIT - Police
 *               - Bit 1 - VOICE_EMER_CAT_AMBULANCE_BIT - Ambulance
 *               - Bit 2 - VOICE_EMER_CAT_FIRE_BRIGADE_BIT- Fire brigade
 *               - Bit 3 - VOICE_EMER_CAT_MARINE_GUARD_BIT - Marine guard
 *               - Bit 4 - VOICE_EMER_CAT_MOUNTAIN_RESCUE_BIT - Mountain rescue
 *               - Bit 5 - VOICE_EMER_CAT_MANUAL_ECALL_BIT -
 *                   Manual emergency call
 *               - Bit 6 - VOICE_EMER_CAT_AUTO_ECALL_BIT -
 *                   Automatic emergency call
 *               - Bit 7 - VOICE_EMER_CAT_SPARE_BIT - Spare bit
 *
 *  @param  pCallPartySubAdd(optional)
 *          - Pointer to structure of calledPartySubAdd
 *              - See @ref voice_calledPartySubAdd for more information
 *
 *  @param  pSvcType(optional)
 *          - Service Type.
 *              - 0x01 - VOICE_DIAL_CALL_SRV_TYPE_AUTOMATIC - Automatic
 *              - 0x02 - VOICE_DIAL_CALL_SRV_TYPE_GSM - GSM
 *              - 0x03 - VOICE_DIAL_CALL_SRV_TYPE_WCDMA - WCDMA
 *              - 0x04 - VOICE_DIAL_CALL_SRV_TYPE_CDMA_AUTOMATIC -
 *                  CDMA automatic
 *              - 0x05 - VOICE_DIAL_CALL_SRV_TYPE_GSM_WCDMA - GSM or WCDMA
 *              - 0x06 - VOICE_DIAL_CALL_SRV_TYPE_LTE -LTE
 */
typedef struct {
    uint8_t                 callNumber[MAX_VOICE_CALL_NO_LEN];
    uint8_t                 *pCallType;
    uint8_t                 *pCLIRType;
    voice_UUSInfo           *pUUSInFo;
    voice_CUGInfo           *pCUGInfo;
    uint8_t                 *pEmergencyCategory;
    voice_calledPartySubAdd *pCallPartySubAdd;
    uint8_t                 *pSvcType;
} pack_voice_SLQSVoiceDialCall_t;

/**
 * Originates a voice call (MO call) pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceDialCall(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceDialCall_t *pReq);

 /**
  *  This structure contains information about the Alpha Identifier.
  *
  *  @param  alphaDcs
  *          - Alpha coding scheme
  *              - 0x01 - GSM Default_Char
  *              - 0x02 - UCS2
  *              - 0xFF - Not Available
  *
  *  @param  alphaLen
  *          - Number of sets of the following elements:
  *              - pAlpha_text
  *          - If zero(0) then no further information exists.
  *
  *  @param  alphaText[MAX_VOICE_DESCRIPTION_LENGTH]
  *          - Data encoded as per the alpha_dcs
  *
  */
 typedef struct
 {
    uint8_t alphaDcs;
    uint8_t alphaLen;
    uint8_t alphaText[MAX_VOICE_DESCRIPTION_LENGTH];
 } voice_alphaIDInfo;

 /**
  *  This structure contains information about the Call Control Supplementary
  *  Service Types
  *
  *  @param  svcType
  *          - Service type.
  *              - 0x01 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_ACTIVATE -
  *                  Activate
  *              - 0x02 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_DEACTIVATE -
  *                  Deactivate
  *              - 0x03 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_REGISTER -
  *                  Register
  *              - 0x04 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_ERASE - Erase
  *              - 0x05 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_INTERROGATE -
  *                  Interrogate
  *              - 0x06 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_REGISTER_PASSWORD -
  *                  Register password
  *              - 0x07 - VOICE_CC_SUPS_RESULT_SERVICE_TYPE_USSD - USSD
  *              - 0xFF - Not Available
  *
  *  @param  reason
  *          - Call control supplementary service result reason
  *          - Values:
  *              - See qaGobiApiTableCallControlReturnReasons.h for return reasons.
  *
  */
 typedef struct
 {
    uint8_t svcType;
    uint8_t reason;
 } voice_ccSUPSType;

/**
 *  This structure contains Voice Call Response Parameters
 *
 *  @param  pCallID(optional)
 *          - Unique call identifier for the dialed call
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAlphaIDInfo(optional)
 *          - Pointer to structure of alphaIDInfo
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCCResultType(optional)
 *          - Call Control Result Type.
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCSUPSType(optional)
 *          - Pointer to structure of ccSUPSType
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t           *pCallID;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResultType;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceDialCall_t;

 /**
 * Originates a voice call (MO call) unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceDialCall(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_voice_SLQSVoiceDialCall_t *pOutput
        );

/**
 *  This structure contain pack voice end call parameter.
 *  @param  pCallId
 *          - Unique call identifier for the call that must be ended
 *
 */
typedef struct {
    uint8_t     *pCallId;
} pack_voice_SLQSVoiceEndCall_t;

/**
 * Ends a voice call pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceEndCall(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceEndCall_t *pReq);

/**
 *  This structure contains unpack voice end call parameter.
 *  @param  pCallId
 *          - Unique call identifier for the call that must be ended
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t     *pCallId;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceEndCall_t;

 /**
 * Ends a voice call unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceEndCall(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_voice_SLQSVoiceEndCall_t *pOutput
        );
/**
 *  This structure contains Supplementary Service request parameters related to
 *  different features and their activation, deactivation, registration
 *  and erasure (applicable only for 3GPP)
 *
 *  @param  numberType
 *          - Call forwarding number type
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN
 *                  Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL
 *                  International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL
 *                  National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC
 *                  Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER
 *                  Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED
 *                  Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED
 *                  Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION
 *                  Reserved extension
 *
 *  @param  numberPlan
 *          - Call forwarding number plan
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN
 *                  Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN
 *                  ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA
 *                  Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX
 *                  Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL
 *                  National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE
 *                  Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION
 *                  Reserved extension
 */
typedef struct
{
    uint8_t numberType;
    uint8_t numberPlan;
} voice_callFwdTypeAndPlan;

/**
 *  Service Class information
 */
enum liteServiceClassInformation
{
    LITE_VOICE_SUPS_SRV_CLASS_NONE             = 0x00,
    LITE_VOICE_SUPS_SRV_CLASS_VOICE            = 0x01,
    LITE_VOICE_SUPS_SRV_CLASS_DATA             = 0x02,
    LITE_VOICE_SUPS_SRV_CLASS_FAX              = 0x04,
    LITE_VOICE_SUPS_SRV_CLASS_SMS              = 0x08,
    LITE_VOICE_SUPS_SRV_CLASS_DATACIRCUITSYNC  = 0x10,
    LITE_VOICE_SUPS_SRV_CLASS_DATACIRCUITASYNC = 0x20,
    LITE_VOICE_SUPS_SRV_CLASS_PACKETACCESS     = 0x40,
    LITE_VOICE_SUPS_SRV_CLASS_PADACCESS        = 0x80,
};

/**
 *  This structure contains Supplementary Service request parameters related to
 *  different features and their activation, deactivation, registration
 *  and erasure (applicable only for 3GPP)
 *
 *  @param  voiceSvc
 *          - Manages all call-independent supplementary services, such as
 *            activation, deactivation, registration, and erasure (mandatory)
 *              - 0x01 - VOICE_SERVICE_ACTIVATE
 *              - 0x02 - VOICE_SERVICE_DEACTIVATE
 *              - 0x03 - VOICE_SERVICE_REGISTER
 *              - 0x04 - VOICE_SERVICE_ERASE
 *
 *  @param  reason
 *          - supplementary service reason values (mandatory)
 *              - 0x01 - QMI_VOICE_REASON_FWD_UNCONDITIONAL\n
 *                   Unconditional call forwarding
 *              - 0x02 - QMI_VOICE_REASON_FWD_MOBILEBUSY\n
 *                  Forward when the mobile is busy
 *              - 0x03 - QMI_VOICE_REASON_FWD_NOREPLY\n
 *                  Forward when there is no reply
 *              - 0x04 - QMI_VOICE_REASON_FWD_UNREACHABLE\n
 *                  Forward when the call is unreachable
 *              - 0x05 - QMI_VOICE_REASON_FWD_ALLFORWARDING\n
 *                  All forwarding
 *              - 0x06 - QMI_VOICE_REASON_FWD_ALLCONDITIONAL\n
 *                  All conditional forwarding
 *              - 0x07 - QMI_VOICE_REASON_BARR_ALLOUTGOING\n
 *                  All outgoing calls are barred
 *              - 0x08 - QMI_VOICE_REASON_BARR_OUTGOINGINT\n
 *                  Outgoing internal calls are barred
 *              - 0x09 - QMI_VOICE_REASON_BARR_OUTGOINGINTEXTOHOME\n
 *                  Outgoing calls external to home are barred
 *              - 0x0A - QMI_VOICE_REASON_BARR_ALLINCOMING\n
 *                  All incoming calls are barred
 *              - 0x0B - QMI_VOICE_REASON_BARR_INCOMINGROAMING\n
 *                  Roaming incoming calls are barred
 *              - 0x0C - QMI_VOICE_REASON_BARR_ALLBARRING\n
 *                  All calls are barred
 *              - 0x0D - QMI_VOICE_REASON_BARR_ALLOUTGOINGBARRING\n
 *                  All outgoing calls are barred
 *              - 0x0E - QMI_VOICE_REASON_BARR_ALLINCOMINGBARRING\n
 *                  All incoming calls are barred
 *              - 0x0F - QMI_VOICE_REASON_CALLWAITING\n
 *                  Call waiting
 *
 *  @param  pServiceClass
 *          - Service class is a combination (sum) of information
 *            class constants (optional)
 *              - See @ref liteServiceClassInformation for more information
 *
 *  @param  pCallBarringPasswd
 *          - Password is required if call barring is provisioned using a
 *            password. Password consists of 4 ASCII digits.
 *            Range: 0000 to 9999 (optional)
 *
 *  @param  pCallForwardingNumber
 *          - Call forwarding number to be registered with the network. This has
 *            to be included in the request only when the service is set to
 *            VOICE_SERVICE_REGISTER. NULL terminated ASCII string. (optional)
 *
 *  @param  pTimerVal
 *          - Call forwarding no reply timer value in seconds. This has to be
 *            included in the request only when the service is set to
 *            VOICE_SERVICE_REGISTER and the reason is
 *            QMI_VOICE_REASON_FWD_NOREPLY. (optional)
 *              - Range: 5 to 30 in steps of 5
 *
 *  @param  pCallFwdTypeAndPlan
 *          - Information about call forwarding type and plan. This parameter is
 *            ignored when the Call Forwarding Number is not included (optional)
 *              - See @ref voice_callFwdTypeAndPlan for more information
 *
 */
typedef struct {
    uint8_t voiceSvc;
    uint8_t reason;
    uint8_t *pServiceClass;
    uint8_t *pCallBarringPasswd;
    uint8_t *pCallForwardingNumber;
    uint8_t *pTimerVal;
    voice_callFwdTypeAndPlan *pCallFwdTypeAndPlan;
} pack_voice_SLQSVoiceSetSUPSService_t;

/**
 * call-independent supplementary services pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceSetSUPSService(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceSetSUPSService_t *pReq);

/**
 *  This structure contains Supplementary Service response parameters related to
 *  different features and their activation, deactivation, registration
 *  and erasure (applicable only for 3GPP)
 *
 *  @param  pFailCause
 *          - Supplementary service failure causes (optional, supply NULL if
 *            not required).
 *              - 0xFFFF is the value when the information is not received from
 *                device
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo. The parameter used to pass
 *            the alpha (if any) given by the SIM/R-UIM after call control
 *            (optional, supply NULL if not required)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCCResultType
 *          - Call control result types (optional, supply NULL if not required)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD - Unstructured supplementary service
 *              - 0xFF - if the device does not provide this information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Unique call identifier for the dialed call (optional, supply NULL
 *            if not required)
 *              - 0x00 - if the device does not provide this information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCSUPSType
 *          - Data is present when pCCResultType is present and is other
 *            than Voice. (optional, supply NULL if not required)
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t          *pFailCause;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResultType;
    uint8_t           *pCallID;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceSetSUPSService_t;

/**
* call-independent supplementary services unpack
* @param[in]   pResp       qmi response from modem
* @param[in]   respLen     qmi response length
* @param[out]  pOutput      response unpacked
*
* @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
*
* @sa     See qmerrno.h for eQCWWAN_xxx error values
*/
int unpack_voice_SLQSVoiceSetSUPSService(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceSetSUPSService_t *pOutput
       );

/**
 *  This structure contains information about the Air Timer.
 *
 *  @param  namID
 *          - Index of the NAM(Number Assignment Module) to be configured.
 *          - Range 0 to 3.
 *          - Some modems support only 1 or 2 NAMs.
 *          - 0xFF,if not available.
 *
 *  @param  airTimerValue
 *          - Time in minutes.
 *          - Cumulative air time is slammed.
 *          - 0xFFFFFFFF,if not available.
 *
 */
typedef struct
{
    uint8_t  namID;
    uint32_t airTimerValue;
} voice_airTimer;

/**
 *  This structure contains information about the Roam Timer.
 *
 *  @param  namID
 *          - Index of the NAM(Number Assignment Module) to be configured.
 *          - Range 0 to 3.
 *          - Some modems support only 1 or 2 NAMs.
 *          - 0xFF,if not available.
 *
 *  @param  roamTimerValue
 *          - Time in minutes.
 *          - Cumulative air time is slammed.
 *          - 0xFFFFFFFF,if not available.
 *
 */
typedef struct
{
    uint8_t  namID;
    uint32_t roamTimerValue;
} voice_roamTimer;

/**
 *  This structure contains information about the Preferred Voice Service Options.
 *
 *  @param  namID
 *          - Index of the NAM(Number Assignment Module) to be configured.
 *          - Range 0 to 3.
 *          - Some modems support only 1 or 2 NAMs.
 *          - 0xFF,if not available.
 *
 *  @param  evrcCapability
 *          - EVRC capability.
 *          - Values:
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *              - 0xFF - Not Available
 *
 *  @param  homePageVoiceSO
 *          - Home page voice SO; most preferred CDMA SO to be requested from
 *            the network when receiving an incoming (MT) voice call within
 *            the home network.
 *          - Values:
 *              - 0x0000 - VOICE_SO_WILD - Any service option
 *              - 0x0001 - VOICE_SO_IS_96A - IS-96A
 *              - 0x0003 - VOICE_SO_EVRC - EVRC
 *              - 0x0011 - VOICE_SO_13K_IS733 - 13K_IS733
 *              - 0x0038 - VOICE_SO_SELECTABLE_MODE_VOCODER -
 *                  Selectable mode vocoder
 *              - 0x0044 - VOICE_SO_4GV_NARR0W_BAND - 4GV narrowband
 *              - 0x0046 - VOICE_SO_4GV_WIDE_BAND - 4GV wideband
 *              - 0x8000 - VOICE_SO_13K - 13K
 *              - 0x8001 - VOICE_SO_IS_96 - IS-96
 *              - 0x8023 - VOICE_SO_WVRC - WVRC
 *              - 0xFFFF - Not Available
 *
 *  @param  homeOrigVoiceSO
 *          - Home origination voice SO; most preferred CDMA SO to be requested
 *            from the network when receiving an incoming (MT) voice call within
 *            the home network.
 *          - Values:
 *              - 0x0000 - VOICE_SO_WILD - Any service option
 *              - 0x0001 - VOICE_SO_IS_96A - IS-96A
 *              - 0x0003 - VOICE_SO_EVRC - EVRC
 *              - 0x0011 - VOICE_SO_13K_IS733 - 13K_IS733
 *              - 0x0038 - VOICE_SO_SELECTABLE_MODE_VOCODER -
 *                  Selectable mode vocoder
 *              - 0x0044 - VOICE_SO_4GV_NARR0W_BAND - 4GV narrowband
 *              - 0x0046 - VOICE_SO_4GV_WIDE_BAND - 4GV wideband
 *              - 0x8000 - VOICE_SO_13K - 13K
 *              - 0x8001 - VOICE_SO_IS_96 - IS-96
 *              - 0x8023 - VOICE_SO_WVRC - WVRC
 *              - 0xFFFF - Not Available
 *
 *  @param  roamOrigVoiceSO
 *          - Roaming origination voice SO; most preferred CDMA SO to be
 *            requested from the network when receiving an incoming (MT)
 *            voice call within the home network.
 *          - Values:
 *              - 0x0000 - VOICE_SO_WILD - Any service option
 *              - 0x0001 - VOICE_SO_IS_96A - IS-96A
 *              - 0x0003 - VOICE_SO_EVRC - EVRC
 *              - 0x0011 - VOICE_SO_13K_IS733 - 13K_IS733
 *              - 0x0038 - VOICE_SO_SELECTABLE_MODE_VOCODER -
 *                  Selectable mode vocoder
 *              - 0x0044 - VOICE_SO_4GV_NARR0W_BAND - 4GV narrowband
 *              - 0x0046 - VOICE_SO_4GV_WIDE_BAND - 4GV wideband
 *              - 0x8000 - VOICE_SO_13K - 13K
 *              - 0x8001 - VOICE_SO_IS_96 - IS-96
 *              - 0x8023 - VOICE_SO_WVRC - WVRC
 *              - 0xFFFF - Not Available
 *
 */
typedef struct
{
    uint8_t  namID;
    uint8_t  evrcCapability;
    uint16_t homePageVoiceSO;
    uint16_t homeOrigVoiceSO;
    uint16_t roamOrigVoiceSO;
} voice_prefVoiceSO;

/**
 *  This structure contains information about the Set Configuration
 *  Request Parameters.
 *
 *  @param  pAutoAnswer
 *          - Value specified is written to NV_AUTO_ANSWER_I. (optional)
 *          - Values:
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pAirTimerConfig
 *          - Value specified is written to NV_AIR_CNT_I. (optional)
 *          - See @ref voice_airTimer for more information
 *
 *  @param  pRoamTimerConfig
 *          - Value specified is written to NV_ROAM_CNT_I. (optional)
 *          - See @ref voice_roamTimer for more information
 *
 *  @param  pTTYMode
 *          - Value specified is written to NV_TTY_I. (optional)
 *          - Values:
 *              - 0x00 - TTY_MODE_FULL - Full
 *              - 0x01 - TTY_MODE_VCO - Voice carry over
 *              - 0x02 - TTY_MODE_HCO - Hearing carry over
 *              - 0x03 - TTY_MODE_OFF - Off
 *
 *  @param  pPrefVoiceSO
 *          - Value specified is written to NV_PREF_VOICE_SO_I. (optional)
 *          - See @ref voice_prefVoiceSO for more information
 *
 *  @param  pPrefVoiceDomain
 *          - Preferred Voice-Domain. (optional)
 *          - Values:
 *              - 0x00 - VOICE_DOMAIN_PREF_CS_ONLY -
 *                  Circuit-switched (CS) only
 *              - 0x01 - VOICE_DOMAIN_PREF_PS_ONLY -
 *                  Packet-switched (PS) only
 *              - 0x02 - VOICE_DOMAIN_PREF_CS_PREF -
 *                  CS is preferred, PS is secondary
 *              - 0x03 - VOICE_DOMAIN_PREF_PS_PREF -
 *                  PS is preferred, CS is secondary
 *
 *  @note  One of the optional parameters must be present in the request.
 *
 */
typedef struct
{
    uint8_t           *pAutoAnswer;
    voice_airTimer    *pAirTimerConfig;
    voice_roamTimer   *pRoamTimerConfig;
    uint8_t           *pTTYMode;
    voice_prefVoiceSO *pPrefVoiceSO;
    uint8_t           *pPrefVoiceDomain;
} pack_voice_SLQSVoiceSetConfig_t;

/**
 * configuration parameters that control the modem behavior pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceSetConfig(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceSetConfig_t *pReq);

/**
 *  This structure contains information about the Set Configuration
 *  Response Parameters.
 *
 *  @param  pAutoAnsStatus
 *          - Auto Answer Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAirTimerStatus
 *          - Air Timer Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pRoamTimerStatus
 *          - Roam Timer Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pTTYConfigStatus
 *          - TTY Config Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pPrefVoiceSOStatus
 *          - Preferred Voice SO Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pVoiceDomainPrefStatus
 *          - Voice-Domain Preference Status. (optional)
 *          - Values:
 *              - 0x00 - Information was written successfully
 *              - 0x01 - Information write failed
 *              - 0xFF - Not Available.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t *pAutoAnsStatus;
    uint8_t *pAirTimerStatus;
    uint8_t *pRoamTimerStatus;
    uint8_t *pTTYConfigStatus;
    uint8_t *pPrefVoiceSOStatus;
    uint8_t *pVoiceDomainPrefStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceSetConfig_t;

/**
 * configuration parameters that control the modem behavior unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceSetConfig(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceSetConfig_t *pOutput
       );

/**
 *  Contains the parameters passed for pack voice Answer Call.
 *
 *  \param  pCallId
 *          - Unique call identifier for the call that must be answered.
 *
 */
typedef struct
{
    uint8_t *pCallId;
} pack_voice_SLQSVoiceAnswerCall_t;

/**
 * Answers an incoming voice call pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceAnswerCall(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceAnswerCall_t *pReq);

/**
 *  Contains the parameters passed for pack voice Answer Call.
 *
 *  \param  pCallId
 *          - Unique call identifier for the call that must be answered.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t *pCallId;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceAnswerCall_t;

/**
 * Answers an incoming voice call unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceAnswerCall(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceAnswerCall_t *pOutput
      );

/**
 * status of the Calling Line Identification Restriction (CLIR) pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceGetCLIR(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

 /**
 *  This structure contains information about the Calling Line Identification
 *  Restriction (CLIR) supplementary service responses.
 *
 *  @param  ActiveStatus
 *          - Active status.
 *          - Values:
 *              - 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
 *              - 0x01 - ACTIVE_STATUS_ACTIVE - Active
 *              - 0xFF - Not Available
 *
 *  @param  ProvisionStatus
 *          - Provisioned status.
 *          - Values:
 *              - 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
 *              - 0x01 - PROVISION_STATUS_PROVISIONED_PERMANENT -
 *                  Permanently provisioned
 *              - 0x02 - PROVISION_STATUS_PRESENTATION_RESTRICTED -
 *                  Restricted presentation
 *              - 0x03 - PROVISION_STATUS_PRESENTATION_ALLOWED -
 *                  Allowed presentation
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t ActiveStatus;
    uint8_t ProvisionStatus;
} voice_CLIRResp;


/**
 *  This structure contains Voice Get Calling Line Identification
 *  Restriction (CLIR) Response Parameters
 *
 *  @param  pCLIRResp
 *          - Pointer to structure of CLIRResp (optional)
 *              - See @ref voice_CLIRResp for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
   voice_CLIRResp    *pCLIRResp;
   uint16_t          *pFailCause;
   voice_alphaIDInfo *pAlphaIDInfo;
   uint8_t           *pCCResType;
   uint8_t           *pCallID;
   voice_ccSUPSType  *pCCSUPSType;
   swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCLIR_t;

/**
 * status of the Calling Line Identification Restriction (CLIR) unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCLIR(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCLIR_t *pOutput
      );

 /**
  * status of the Calling Line Identification Presentation (CLIP) pack
  * @param[in,out] pCtx qmi request context
  * @param[out] pReqBuf qmi request buffer
  * @param[out] pLen qmi request length
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  */
  int pack_voice_SLQSVoiceGetCLIP(
         pack_qmi_t  *pCtx,
         uint8_t     *pReqBuf,
         uint16_t    *pLen
         );

/**
 *  This structure contains information about the Calling Line Identification
 *  Presentation (CLIP) supplementary service responses.
 *
 *  \param  ActiveStatus
 *          - Active status.
 *          - Values:
 *              - 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
 *              - 0x01 - ACTIVE_STATUS_ACTIVE - Active
 *              - 0xFF - Not Available
 *
 *  \param  ProvisionStatus
 *          - Provisioned status.
 *          - Values:
 *              - 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
 *              - 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
 *              - 0xFF - Not Available
 *
 */
 typedef struct
 {
     uint8_t ActiveStatus;
     uint8_t ProvisionStatus;
 } voice_CLIPResp;

/**
 *  This structure contains Voice Get Calling Line Identification
 *  Presentation(CLIP) Response Parameters
 *
 *  @param  pCLIPResp
 *          - Pointer to structure of CLIPResp (optional)
 *              - See @ref voice_CLIPResp for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
 typedef struct
 {
    voice_CLIPResp    *pCLIPResp;
    uint16_t          *pFailCause;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResType;
    uint8_t           *pCallID;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
 } unpack_voice_SLQSVoiceGetCLIP_t;

/**
 * status of the Calling Line Identification Presentation (CLIP) unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCLIP(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCLIP_t *pOutput
      );

/**
 *  This structure contains Voice Get Call Waiting Response Parameters
 *
 *  @param  pSvcClass
 *          - Service class is a combination (sum) of information class
 *            constants (optional)
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *          - Service Class is set to 0 if call waiting is not active for any
 *            of the information classes.
 *          - 0xFF,if Not Available
 */
typedef struct
{
    uint8_t        *pSvcClass;
}pack_voice_SLQSVoiceGetCallWaiting_t;

/**
 * Status of Call Waiting Supplementary Service pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceGetCallWaiting(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceGetCallWaiting_t *pReq);

/**
 *  This structure contains Voice Get Call Waiting Unpack Parameters
 *
 *  @param  pSvcClass
 *          - Service class is a combination (sum) of information class
 *            constants (optional)
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *          - Service Class is set to 0 if call waiting is not active for any
 *            of the information classes.
 *          - 0xFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
   uint8_t           *pSvcClass;
   uint16_t          *pFailCause;
   voice_alphaIDInfo *pAlphaIDInfo;
   uint8_t           *pCCResType;
   uint8_t           *pCallID;
   voice_ccSUPSType  *pCCSUPSType;
   swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCallWaiting_t;

/**
 * Status of Call Waiting Supplementary Service unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCallWaiting(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCallWaiting_t *pOutput
      );

/**
 *  This structure contains Voice Get Call Barring Pack Parameters
 *
 *  @param  reason
 *          - Call Barring Reason
 *          - Values:
 *              - 0x07 - QMI_VOICE_REASON_BARR_ALLOUTGOING - All outgoing
 *              - 0x08 - QMI_VOICE_REASON_BARR_OUTGOINGINT - Outgoing internal
 *              - 0x09 - QMI_VOICE_REASON_BARR_OUTGOINGINTEXTOHOME -
 *                  Outgoing external to home
 *              - 0x0A - QMI_VOICE_REASON_BARR_ALLINCOMING - All incoming
 *              - 0x0B - QMI_VOICE_REASON_BARR_INCOMINGROAMING -
 *                  Roaming incoming
 *              - 0x0C - QMI_VOICE_REASON_BARR_ALLBARRING -
 *                  All calls are barred
 *              - 0x0D - QMI_VOICE_REASON_BARR_ALLOUTGOINGBARRING -
 *                  All outgoing calls are barred
 *              - 0x0E - QMI_VOICE_REASON_BARR_ALLINCOMINGBARRING -
 *                  All incoming calls are barred
 *
 *  @param  pSvcClass
 *          - Service class is a combination (sum) of information class
 *            constants (optional)
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *          - Service Class is set to 0 if call waiting is not active for any
 *            of the information classes.
 *          - 0xFF,if Not Available
 *
 */
typedef struct
{
   uint8_t reason;
   uint8_t *pSvcClass;
} pack_voice_SLQSVoiceGetCallBarring_t;

/**
 * Status of Call Barring Supplementary Service pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceGetCallBarring(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceGetCallBarring_t *pReq);

/**
 *  This structure contains Voice Get Call Barring Response Parameters
 *
 *  @param  pSvcClass
 *          - Service class is a combination (sum) of information class
 *            constants (optional)
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *          - Service Class is set to 0 if call waiting is not active for any of
 *            the information classes.
 *          - 0xFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type  (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    uint8_t           *pSvcClass;
    uint16_t          *pFailCause;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResType;
    uint8_t           *pCallID;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCallBarring_t;

/**
 * Status of Call Barring Supplementary Service unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCallBarring(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCallBarring_t *pOutput
      );

/**
 *  This structure contains Voice Get Call Forwarding Status Pack Parameters
 *
 *  @param  Reason
 *          - Call Forwarding Reason
 *          - Values:
 *              - 0x01 - QMI_VOICE_REASON_FWDREASON_UNCONDITIONAL -
 *                  Unconditional call forwarding
 *              - 0x02 - QMI_VOICE_REASON_FWDREASON_MOBILEBUSY -
 *                  Forward when the mobile is busy
 *              - 0x03 - QMI_VOICE_REASON_FWDREASON_NOREPLY -
 *                  Forward when there is no reply
 *              - 0x04 - QMI_VOICE_REASON_FWDREASON_UNREACHABLE -
 *                  Forward when the call is unreachable
 *              - 0x05 - QMI_VOICE_REASON_FWDREASON_ALLFORWARDING -
 *                  All forwarding
 *              - 0x06 - QMI_VOICE_REASON_FWDREASON_ALLCONDITIONAL -
 *                  All conditional forwarding
 *
 *  @param  pSvcClass(optional)
 *          - Service Class is a combination (sum) of information class constants
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *
 */
typedef struct
{
    uint8_t Reason;
    uint8_t *pSvcClass;
} pack_voice_SLQSVoiceGetCallForwardingStatus_t;

/**
 * Status of Call Forwarding Supplementary Service pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceGetCallForwardingStatus(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceGetCallForwardingStatus_t *pReq);

 /**
 *  This structure contains information for Get Call Forwarding Information.
 *
 *  @param  SvcStatus
 *          - Service status. Values:
 *              - 0x00 - SERVICE_STATUS_INACTIVE - Inactive
 *              - 0x01 - SERVICE_STATUS_ACTIVE - Active
 *
 *  @param  SvcClass
 *          - Service Class is a combination (sum) of information class
 *            constants
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[255]
 *          - number of numLen length, NULL terminated.
 *
 *  @param  noReplyTimer
 *          - No reply timer value in seconds
 *          - A value of 0 indicates that noReplyTimer is ignored.
 */
typedef struct
{
    uint8_t SvcStatus;
    uint8_t SvcClass;
    uint8_t numLen;
    uint8_t number[255];
    uint8_t noReplyTimer;
} voice_callFWInfo;

 /**
 *  This structure contains information for Get Call Forwarding
 *  Extended Information.
 *
 *  @param  SvcStatus
 *          - Service status. Values:
 *              - 0x00 - SERVICE_STATUS_INACTIVE - Inactive
 *              - 0x01 - SERVICE_STATUS_ACTIVE - Active
 *
 *  @param  SvcClass
 *          - Service Class is a combination (sum) of information class
 *            constants
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *
 *  @param  noReplyTimer
 *          - No reply timer value in seconds
 *          - A value of 0 indicates that no_reply_timer is ignored.
 *
 *  @param  PI
 *          - Presentation indicator; refer to [S1, Table 2.7.4.4-1]
 *            for valid values.
 *
 *  @param  SI
 *          - Number screening indicator.
 *          - Values:
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                  Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                  Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                  Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED - Provided network
 *
 *  @param  numType
 *          - Number type.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numPlan
 *          - Number plan.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[255]
 *          - number of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t SvcStatus;
    uint8_t SvcClass;
    uint8_t noReplyTimer;
    uint8_t PI;
    uint8_t SI;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t numLen;
    uint8_t number[255];
} voice_callFWExtInfo;

/**
 *  This structure contains an array of Call Forwarded Information.
 *
 *  @param  numInstances
 *          - Number of callFWInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  CallFWInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of callFWInfo.
 *              - See @ref voice_callFWInfo for more information.
 *
 */
typedef struct
{
    uint8_t          numInstances;
    voice_callFWInfo CallFWInfo[VOICE_MAX_NO_OF_CALLS];
} voice_getCallFWInfo;

/**
 *  This structure contains an array of Call Forwarded Extended Information.
 *
 *  @param  numInstances
 *          - Number of callFWExtInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  CallFWExtInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of CallFWExtInfo.
 *              - See @ref voice_callFWExtInfo for more information.
 *
 */
typedef struct
{
    uint8_t             numInstances;
    voice_callFWExtInfo CallFWExtInfo[VOICE_MAX_NO_OF_CALLS];
} voice_getCallFWExtInfo;

/**
 *  This structure contains Voice Get Call Forwarding Status Unpack
 *  Parameters
 *
 *  @param  pGetCallFWInfo
 *          - Pointer to structure of getCallFWInfo (optional)
 *              - See @ref voice_getCallFWInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pGetCallFWExtInfo
 *          - Pointer to structure of getCallFWExtInfo (optional)
 *              - See @ref voice_getCallFWExtInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    voice_getCallFWInfo    *pGetCallFWInfo;
    uint16_t               *pFailCause;
    voice_alphaIDInfo      *pAlphaIDInfo;
    uint8_t                *pCCResType;
    uint8_t                *pCallID;
    voice_ccSUPSType       *pCCSUPSType;
    voice_getCallFWExtInfo *pGetCallFWExtInfo;
    swi_uint256_t          ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCallForwardingStatus_t;

/**
 * Status of Call Forwarding Supplementary Service unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCallForwardingStatus(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCallForwardingStatus_t *pOutput
      );

/**
 *  This structure contains Voice Set Call Barring Password Pack Parameters
 *
 *  @param  Reason
 *          - Call Barring Reason
 *          - Values:
 *              - 0x07 - QMI_VOICE_REASON_BARR_ALLOUTGOING - All outgoing
 *              - 0x08 - QMI_VOICE_REASON_BARR_OUTGOINGINT - Outgoing internal
 *              - 0x09 - QMI_VOICE_REASON_BARR_OUTGOINGINTEXTOHOME -
 *                  Outgoing external to home
 *              - 0x0A - QMI_VOICE_REASON_BARR_ALLINCOMING - All incoming
 *              - 0x0B - QMI_VOICE_REASON_BARR_INCOMINGROAMING -
 *                  Roaming incoming
 *              - 0x0C - QMI_VOICE_REASON_BARR_ALLBARRING -
 *                  All calls are barred
 *              - 0x0D - QMI_VOICE_REASON_BARR_ALLOUTGOINGBARRING -
 *                  All outgoing calls are barred
 *              - 0x0E - QMI_VOICE_REASON_BARR_ALLINCOMINGBARRING -
 *                  All incoming calls are barred
 *
 *  @param  oldPasswd[BARRING_PASSWORD_LENGTH]
 *          - Old password.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *
 *  @param  newPasswd[BARRING_PASSWORD_LENGTH]
 *          - New password.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *
 *  @param  newPasswdAgain[BARRING_PASSWORD_LENGTH]
 *          - New password Again.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *
 */
typedef struct
{
    uint8_t Reason;
    uint8_t oldPasswd[BARRING_PASSWORD_LENGTH];
    uint8_t newPasswd[BARRING_PASSWORD_LENGTH];
    uint8_t newPasswdAgain[BARRING_PASSWORD_LENGTH];
} pack_voice_SLQSVoiceSetCallBarringPassword_t;

/**
 * Sets a Call Barring Password pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceSetCallBarringPassword(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceSetCallBarringPassword_t *pReq);

/**
 *  This structure contains Voice Set Call Barring Password Unpack Parameters
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    uint16_t          *pFailCause;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResType;
    uint8_t           *pCallID;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceSetCallBarringPassword_t;

/**
 * Sets a Call Barring Password unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceSetCallBarringPassword(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceSetCallBarringPassword_t *pOutput
      );

/**
 *  This structure contains information of the request parameters associated
 *  with a call.
 *
 *  @param  callID
 *          - Call identifier for the call queried for information.
 *
 */
typedef struct
{
    uint8_t callID;
} pack_voice_SLQSVoiceGetCallInfo_t;

/**
 * information associated with a call pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceGetCallInfo(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceGetCallInfo_t *pReq);

/**
 *  This structure contains Information about call state changes. For example,
 *  when an incoming call is received, this structure is populated and indicate
 *  the incoming call information. When this incoming call is answered, the
 *  call status changes from INCOMING to CONVERSATION, which means a change
 *  in the call information and this structure is populated again with the
 *  changes and notified to/retrived by the user.
 *
 *  @param  callID
 *          - Call identifier for the call queried for information.
 *          - If zero(0) then invalid.
 *
 *  @param  callState
 *          - Call state.
 *              - 0x01 - CALL_STATE_ORIGINATION - Origination
 *              - 0x02 - CALL_STATE_INCOMING - Incoming
 *              - 0x03 - CALL_STATE_CONVERSATION - Conversation
 *              - 0x04 - CALL_STATE_CC_IN_PROGRESS -
 *                 Call is originating but waiting for call control to complete
 *              - 0x05 - CALL_STATE_ALERTING - Alerting
 *              - 0x06 - CALL_STATE_HOLD - Hold
 *              - 0x07 - CALL_STATE_WAITING - Waiting
 *              - 0x08 - CALL_STATE_DISCONNECTING - Disconnecting
 *              - 0x09 - CALL_STATE_END - End
 *              - 0x0A - CALL_STATE_SETUP - MT call is in Setup state in 3GPP
 *              - 0xFF - Not Available
 *
 *  @param  callType
 *          - Call type.
 *              - 0x00 - CALL_TYPE_VOICE - Voice
 *              - 0x02 - CALL_TYPE_VOICE_IP - Voice over IP
 *              - 0x06 - CALL_TYPE_OTAPA - OTAPA
 *              - 0x07 - CALL_TYPE_STD_OTASP - Standard OTASP
 *              - 0x08 - CALL_TYPE_NON_STD_OTASP - Nonstandard OTASP
 *              - 0x09 - CALL_TYPE_EMERGENCY - Emergency
 *              - 0xFF - Not Available
 *
 *  @param  direction
 *          - Direction.
 *              - 0x01 - CALL_DIRECTION_MO - MO call
 *              - 0x02 - CALL_DIRECTION_MT - MT call
 *              - 0xFF - Not Available
 *
 *  @param  mode
 *          - Mode.
 *          - If the mode field is "0x01 - CDMA",the optional Service Option,
 *            Voice Privacy, and OTASP Status (only for OTASP calls) TLVs are
 *            included in the response.
 *              - 0x01 - CALL_MODE_CDMA - CDMA
 *              - 0x02 - CALL_MODE_GSM - GSM
 *              - 0x03 - CALL_MODE_UMTS - UMTS
 *              - 0x04 - CALL_MODE_LTE - LTE
 *              - 0x05 - CALL_MODE_TDS - TD-SCDMA
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t callID;
    uint8_t callState;
    uint8_t callType;
    uint8_t direction;
    uint8_t mode;
} voice_callInfo;

/**
 *  This structure contains information about the numbers that are dialed from
 *  the device or from which a call is received on the device.
 *
 *  @param  presentationInd
 *          - Presentation indicator.
 *              - 0x00 - PRESENTATION_ALLOWED - Allowed presentation
 *              - 0x01 - PRESENTATION_RESTRICTED - Restricted presentation
 *              - 0x02 - PRESENTATION_NUM_UNAVAILABLE -
 *                 Unavailable presentation
 *              - 0x04 - PRESENTATION_PAYPHONE -
 *                 Payphone presentation (GSM/UMTS specific)
 *              - 0xFF - Not Available
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  remPartyNumber[MAX_CALL_NO_LEN]
 *          - Array of numbers in ASCII, NULL ending.
 *
 */
typedef struct
{
    uint8_t presentationInd;
    uint8_t numLen;
    uint8_t remPartyNumber[MAX_VOICE_CALL_NO_LEN];
} voice_remotePartyNum;

 /**
 *  This structure contains information about the names that are dialed from
 *  the device or from which a call is received on the device.
 *
 *  @param  namePI
 *          - Name presentation indicator.
 *              - 0x00 - PRESENTATION_NAME_PRESENTATION_ALLOWED -
 *                 Allowed presentation
 *              - 0x01 - PRESENTATION_NAME_PRESENTATION_RESTRICTED -
 *                 Restricted presentation
 *              - 0x02 - PRESENTATION_NAME_UNAVAILABLE -
 *                 Unavailable presentation
 *              - 0x03 - PRESENTATION_NAME_NAME_PRESENTATION_RESTRICTED -
 *                 Restricted name presentation
 *              - 0xFF - Not Available
 *
 *  @param  codingScheme
 *          - Refer to Table10 qaGobiApiTableCodingScheme.h for coding schemes
 *          - 0xFF - Not Available
 *
 *  @param  nameLen
 *          - Provides the length of name which follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callerName[MAX_VOICE_DESCRIPTION_LENGTH]
 *          - Name in ASCII, NULL ending.
 *
 */
typedef struct
{
    uint8_t namePI;
    uint8_t codingScheme;
    uint8_t nameLen;
    uint8_t callerName[MAX_VOICE_DESCRIPTION_LENGTH];
} voice_remotePartyName;

 /**
 *  This structure contains information about the numbers connected to a device.
 *  It contains information such as number type, eg International or Local.
 *
 *  @param  numPresInd
 *          - Presentation indicator
 *              - 0x00 - PRESENTATION_ALLOWED - Allowed presentation
 *              - 0x01 - PRESENTATION_RESTRICTED - Restricted presentation
 *              - 0x02 - PRESENTATION_NUM_UNAVAILABLE -
 *                 Unavailable presentation
 *              - 0x04 - PRESENTATION_PAYPHONE -
 *                 Payphone presentation (GSM/UMTS specific)
 *              - 0xFF - Not Available
 *
 *  @param  screeningInd
 *          - Screening indicator.
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                 Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                 Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                 Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED -
 *                 Provided network
 *              - 0xFF - Not Available
 *
 *  @param  numType
 *          - Number type.
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                 Reserved extension
 *              - 0xFF - Not Available
 *
 *  @param  numPlan
 *          - Number plan.
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                 Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                 Reserved extension
 *              - 0xFF - Not Available
 *
 *  @param  callerIDLen
 *          - Provides the length of caller ID which follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callerID[MAX_VOICE_CALL_NO_LEN]
 *          - callerID of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t numPresInd;
    uint8_t screeningInd;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t callerIDLen;
    uint8_t callerID[MAX_VOICE_CALL_NO_LEN];
} voice_connectNumInfo;

/**
 *  This structure contains Diagnostic Information
 *
 *  @param  diagInfoLen
 *          - Provides the length of information which follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  diagnosticInfo[MAX_VOICE_DESCRIPTION_LENGTH]
 *          - Diagnostic information.
 *
 */
typedef struct
{
   uint8_t diagInfoLen;
   uint8_t diagnosticInfo[MAX_VOICE_DESCRIPTION_LENGTH];
} voice_diagInfo;

/**
 *  This structure contains information of the unpack parameters associated
 *  with a call.
 *
 *  @param  pCallInfo(optional)
 *          - See @ref voice_callInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pRemotePartyNum(optional)
 *          - See @ref voice_remotePartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pSrvOpt
 *          - Service option(optional)
 *          - Applicable only for 3GPP2 devices.
 *          - See Table8 qaGobiApiTableServiceOptions.h for standard service
 *            option number assignments.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pVoicePrivacy
 *          - Voice Privacy.(optional)
 *          - Applicable only for 3GPP2 devices.
 *          - Values.
 *              - 0x00 - VOICE_PRIVACY_STANDARD - Standard privacy
 *              - 0x01 - VOICE_PRIVACY_ENHANCED - Enhanced privacy
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pOTASPStatus
 *          - OTASP status for the OTASP call.(optional)
 *          - Applicable only for 3GPP2 devices.
 *              - 0x00 - OTASP_STATUS_SPL_UNLOCKED -
 *              SPL unlocked; only for user-initiated OTASP
 *              - 0x01 - OTASP_STATUS_SPRC_RETRIES_EXCEEDED -
 *                 SPC retries exceeded; only for user-initiated OTASP
 *              - 0x02 - OTASP_STATUS_AKEY_EXCHANGED -
 *                 A-key exchanged; only for user-initiated OTASP
 *              - 0x03 - OTASP_STATUS_SSD_UPDATED - SSD updated; for both
 *                 user-initiated OTASP and network-initiated OTASP (OTAPA)
 *              - 0x04 - OTASP_STATUS_NAM_DOWNLOADED - NAM downloaded;
 *                 only for user-initiated OTASP
 *              - 0x05 - OTASP_STATUS_MDN_DOWNLOADED - MDN downloaded;
 *                 only for user-initiated OTASP
 *              - 0x06 - OTASP_STATUS_IMSI_DOWNLOADED - IMSI downloaded;
 *                 only for user-initiated OTASP
 *              - 0x07 - OTASP_STATUS_PRL_DOWNLOADED - PRL downloaded;
 *                 only for user-initiated OTASP
 *              - 0x08 - OTASP_STATUS_COMMITTED - Commit successful;
 *                 only for user-initiated OTASP
 *              - 0x09 - OTASP_STATUS_OTAPA_STARTED - OTAPA started;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0A - OTASP_STATUS_OTAPA_STOPPED - OTAPA stopped;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0B - OTASP_STATUS_OTAPA_ABORTED - OTAPA aborted;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0C - OTASP_STATUS_OTAPA_COMMITTED - OTAPA committed;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pRemotePartyName(optional)
 *          - Applicable only for 3GPP devices.
 *          - See @ref voice_remotePartyName for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pUUSInfo(optional)
 *          - Applicable only for 3GPP devices.
 *          - See @ref voice_UUSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pAlertType(optional)
 *          - Alerting type.
 *          - Applicable only for 3GPP devices.
 *              - 0x00 - ALERTING_LOCAL - Local
 *              - 0x01 - ALERTING_REMOTE - Remote
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pAlphaIDInfo(optional)
 *          - Applicable only for 3GPP devices.
 *          - See @ref voice_alphaIDInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pConnectNumInfo(optional)
 *          - See @ref voice_connectNumInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pDiagInfo(optional)
 *          - See @ref voice_diagInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pAlertingPattern
 *          - Alerting pattern.(optional)
 *              - 0x00 - QMI_VOICE_ALERTING_PATTERN_1 - Pattern 1
 *              - 0x01 - QMI_VOICE_ALERTING_PATTERN_2 - Pattern 2
 *              - 0x02 - QMI_VOICE_ALERTING_PATTERN_3 - Pattern 3
 *              - 0x04 - QMI_VOICE_ALERTING_PATTERN_5 - Pattern 5
 *              - 0x05 - QMI_VOICE_ALERTING_PATTERN_6 - Pattern 6
 *              - 0x06 - QMI_VOICE_ALERTING_PATTERN_7 - Pattern 7
 *              - 0x07 - QMI_VOICE_ALERTING_PATTERN_8 - Pattern 8
 *              - 0x08 - QMI_VOICE_ALERTING_PATTERN_9 - Pattern 9
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    voice_callInfo        *pCallInfo;
    voice_remotePartyNum  *pRemotePartyNum;
    uint16_t              *pSrvOpt;
    uint8_t               *pVoicePrivacy;
    uint8_t               *pOTASPStatus;
    voice_remotePartyName *pRemotePartyName;
    voice_UUSInfo         *pUUSInfo;
    uint8_t               *pAlertType;
    voice_alphaIDInfo     *pAlphaIDInfo;
    voice_connectNumInfo  *pConnectNumInfo;
    voice_diagInfo        *pDiagInfo;
    uint32_t              *pAlertingPattern;
    swi_uint256_t         ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCallInfo_t;

/**
 * information associated with a call unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetCallInfo(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetCallInfo_t *pOutput
      );

/**
 * information associated with all the calls originating or terminating pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_voice_SLQSVoiceGetAllCallInfo(
       pack_qmi_t  *pCtx,
       uint8_t     *pReqBuf,
       uint16_t    *pLen
       );

/**
 *  This structure contains information related to call state change.
 *
 *  @param  Callinfo
 *          - See @ref voice_callInfo for more information.
 *
 *  @param  isEmpty
 *          - Multiparty indicator.
 *              - 0x00 - False
 *              - 0x01 - True
 *
 *  @param  ALS
 *          - Alternate Line Service line indicator.
 *          - Feature for supporting two different phone numbers on the same
 *            mobile device.
 *              - 0x00 - ALS_LINE1 - Line 1 (default)
 *              - 0x01 - ALS_LINE2 - Line 2
 *
 */
typedef struct
{
     voice_callInfo Callinfo;
     uint8_t     isEmpty;
     uint8_t     ALS;
} voice_getAllCallInformation;

/**
 *  This structure contains information for All Call Remote Party Numbers
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  RemotePartyNum
 *          - See @ref voice_remotePartyNum for more information.
 *
 */
typedef struct
{
    uint8_t              callID;
    voice_remotePartyNum RemotePartyNum;
} voice_getAllCallRmtPtyNum;

/**
 *  This structure contains information for All Call Remote Party Names
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  RemotePartyName
 *          - See @ref voice_remotePartyName for more information.
 *
 */
typedef struct
{
    uint8_t               callID;
    voice_remotePartyName RemotePartyName;
} voice_getAllCallRmtPtyName;

 /**
 *  This structure contains information for User to User Signaling Service
 *  for All Calls.
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  uusInfo
 *          - See @ref voice_UUSInfo for more information.
 *
 */
typedef struct
{
    uint8_t       callID;
    voice_UUSInfo uusInfo;
} voice_allCallsUUSInfo;

 /**
 *  This structure contains information for Alpha Identifier for All Calls
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  AlphaIDInfo
 *          - See @ref voice_alphaIDInfo for more information.
 *
 */
typedef struct
{
    uint8_t           callID;
    voice_alphaIDInfo AlphaIDInfo;
} voice_allCallsAlphaIDInfo;

/**
 *  This structure contains Diagnostic Information for All Calls
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  DiagInfo
 *          - See @ref voice_diagInfo for more information.
 *
 */
typedef struct
{
   uint8_t        callID;
   voice_diagInfo DiagInfo;
} voice_allCallsDiagInfo;

/**
 *  This structure contains information for Connected Peer Numbers.
 *
 *  @param  callID
 *          - Unique call identifier for the call.
 *
 *  @param  numPI
 *          - Number presentation indicator.
 *              - 0x00 - PRESENTATION_ALLOWED - Allowed presentation
 *              - 0x01 - PRESENTATION_RESTRICTED - Restricted presentation
 *              - 0x02 - PRESENTATION_NUM_UNAVAILABLE -
 *                 Unavailable presentation
 *              - 0x04 - PRESENTATION_PAYPHONE -
 *                 Payphone presentation (GSM/UMTS specific)
 *              - 0xFF - Not Available
 *
 *  @param  numSI
 *          - Number screening indicator.
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                  Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                  Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                  Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED - Provided network
 *
 *  @param  numType
 *          - Number type.
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numPlan
 *          - Number plan.
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[MAX_VOICE_CALL_NO_LEN]
 *          - number of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t callID;
    uint8_t numPI;
    uint8_t numSI;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t numLen;
    uint8_t number[MAX_VOICE_CALL_NO_LEN];
} voice_peerNumberInfo;

/**
 *  This structure contains an array of Call Info
 *
 *  @param  numInstances
 *          - Number of getAllCallInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  getAllCallInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of CallInfo.
 *          - See @ref voice_getAllCallInformation for more information.
 *
 */
typedef struct
{
    uint8_t                        numInstances;
    voice_getAllCallInformation getAllCallInfo[VOICE_MAX_NO_OF_CALLS];
} voice_arrCallInfo;

/**
 *  This structure contains an array of Remote Party Numbers
 *
 *  @param  numInstances
 *          - Number of remotePartyNum that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  RmtPtyNum[VOICE_MAX_NO_OF_CALLS]
 *          - Array of remotePartyNum.
 *          - See @ref voice_getAllCallRmtPtyNum for more information.
 *
 */
typedef struct
{
    uint8_t                   numInstances;
    voice_getAllCallRmtPtyNum RmtPtyNum[VOICE_MAX_NO_OF_CALLS];
} voice_arrRemotePartyNum;

/**
 *  This structure contains an array of Remote Party Names
 *
 *  @param  numInstances
 *          - Number of remotePartyName that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  GetAllCallRmtPtyName[MAX_NO_OF_CALLS]
 *          - Array of remotePartyName.
 *          - See @ref voice_getAllCallRmtPtyName for more information.
 *
 */
typedef struct
{
    uint8_t                    numInstances;
    voice_getAllCallRmtPtyName GetAllCallRmtPtyName[VOICE_MAX_NO_OF_CALLS];
} voice_arrRemotePartyName;

/**
 *  This structure contains an array of Alerting Type.
 *
 *  @param  numInstances
 *          - Number of callID, AlertingType that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callID[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Unique call identifier for the call.
 *
 *  @param  AlertingType[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Alerting type.
 *              - 0x00 - ALERTING_LOCAL - Local
 *              - 0x01 - ALERTING_REMOTE - Remote
 *
 */
typedef struct
{
    uint8_t numInstances;
    uint8_t callID[VOICE_MAX_NO_OF_CALLS];
    uint8_t AlertingType[VOICE_MAX_NO_OF_CALLS];
} voice_arrAlertingType;

/**
 *  This structure contains an array of User to User Signaling Service
 *  Information
 *
 *  @param  numInstances
 *          - Number of allCallsUUSInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  AllCallsUUSInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of allCallsUUSInfo.
 *          - See @ref voice_allCallsUUSInfo for more information.
 *
 */
typedef struct
{
    uint8_t               numInstances;
    voice_allCallsUUSInfo AllCallsUUSInfo[VOICE_MAX_NO_OF_CALLS];
} voice_arrUUSInfo;

 /**
 *  This structure contains array an of Servicing option.
 *
 *  @param  numInstances
 *          - Number of callID, srvOption that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callID[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Unique call identifier for the call.
 *
 *  @param  srvOption[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Service option.
 *          - See Table9 qaGobiApiTableServiceOptions.h for standard service
 *            option number assignments.
 *
 */
typedef struct
{
  uint8_t  numInstances;
  uint8_t  callID[VOICE_MAX_NO_OF_CALLS];
  uint16_t srvOption[VOICE_MAX_NO_OF_CALLS];
} voice_arrSvcOption;

/**
 *  This structure contains an array of Call End Reasons.
 *
 *  @param  numInstances
 *          - Number of callID, callEndReason that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callID[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Unique call identifier for the call.
 *
 *  @param  callEndReason[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Call End Reason .
 *          - See Table9 qaGobiApiTableVoiceCallEndReasons.h for a list of valid
 *            voice-related call end reasons
 *
 */
typedef struct
{
   uint8_t  numInstances;
   uint8_t  callID[VOICE_MAX_NO_OF_CALLS];
   uint16_t callEndReason[VOICE_MAX_NO_OF_CALLS];
} voice_arrCallEndReason;

/**
 *  This structure contains an array of Alpha ID Info
 *
 *  @param  numInstances
 *          - Number of allCallsAlphaIDInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  allCallsAlphaIDInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of allCallsAlphaIDInfo.
 *          - See @ref voice_allCallsAlphaIDInfo for more information.
 *
 */
typedef struct
{
    uint8_t                   numInstances;
    voice_allCallsAlphaIDInfo allCallsAlphaIDInfoArr[VOICE_MAX_NO_OF_CALLS];
} voice_arrAlphaID;

 /**
 *  This structure contains an array of Connected Party Numbers consisting of
 *  information regarding all the devices connected.
 *
 *  @param  numInstances
 *          - Number of ConnectedPartyNum that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  ConnectedPartyNum[VOICE_MAX_NO_OF_CALLS]
 *          - Array of ConnectedPartyNum.
 *          - See @ref voice_peerNumberInfo for more information.
 *
 */
typedef struct
{
     uint8_t              numInstances;
     voice_peerNumberInfo ConnectedPartyNum[VOICE_MAX_NO_OF_CALLS];
} voice_arrConnectPartyNum;

 /**
 *  This structure contains an array of Diagnostic Information.
 *
 *  @param  numInstances
 *          - Number of DiagInfo that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  DiagInfo[VOICE_MAX_NO_OF_CALLS]
 *          - Array of DiagInfo.
 *          - See @ref voice_allCallsDiagInfo for more information.
 *
 */
typedef struct
{
    uint8_t                numInstances;
    voice_allCallsDiagInfo DiagInfo[VOICE_MAX_NO_OF_CALLS];
} voice_arrDiagInfo;

/**
 *  This structure contains an array of Called Party Numbers consisting of
 *  information of all the numbers which have been called from the device.
 *
 *  @param  numInstances
 *          - Number of calledPartyNum that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  CalledPartyNum[VOICE_MAX_NO_OF_CALLS]
 *          - Array of CalledPartyNum.
 *          - See @ref voice_peerNumberInfo for more information.
 *
 */
typedef struct
{
     uint8_t              numInstances;
     voice_peerNumberInfo CalledPartyNum[VOICE_MAX_NO_OF_CALLS];
} voice_arrCalledPartyNum;

/**
 *  This structure contains an array of Redirecting Party Numbers consisting of
 *  information of all the numbers which have been redirected from the device.
 *
 *  @param  numInstances
 *          - Number of redirPartyNum that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  RedirPartyNum[VOICE_MAX_NO_OF_CALLS]
 *          - Array of RedirPartyNum.
 *          - See @ref voice_peerNumberInfo for more information.
 *
 */
typedef struct
{
    uint8_t              numInstances;
    voice_peerNumberInfo RedirPartyNum[VOICE_MAX_NO_OF_CALLS];
} voice_arrRedirPartyNum;

/**
 *  This structure contains an array of Alerting Pattern.
 *
 *  @param  numInstances
 *          - Number of callID, alertingPattern that follow.
 *          - If zero(0) then no further information exists.
 *
 *  @param  callID[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Unique call identifier for the call.
 *
 *  @param  alertingPattern[VOICE_MAX_NO_OF_CALLS]
 *          - Array of Alerting pattern.
 *              - 0x00 - QMI_VOICE_ALERTING_PATTERN_1 - Pattern 1
 *              - 0x01 - QMI_VOICE_ALERTING_PATTERN_2 - Pattern 2
 *              - 0x02 - QMI_VOICE_ALERTING_PATTERN_3 - Pattern 3
 *              - 0x04 - QMI_VOICE_ALERTING_PATTERN_5 - Pattern 5
 *              - 0x05 - QMI_VOICE_ALERTING_PATTERN_6 - Pattern 6
 *              - 0x06 - QMI_VOICE_ALERTING_PATTERN_7 - Pattern 7
 *              - 0x07 - QMI_VOICE_ALERTING_PATTERN_8 - Pattern 8
 *              - 0x08 - QMI_VOICE_ALERTING_PATTERN_9 - Pattern 9
 *
 */
typedef struct
{
     uint8_t  numInstances;
     uint8_t  callID[VOICE_MAX_NO_OF_CALLS];
     uint32_t alertingPattern[VOICE_MAX_NO_OF_CALLS];
} voice_arrAlertingPattern;

 /**
 *  This structure contains information about the response parameters
 *  with all the calls originating or terminating from a particular device.
 *
 *  @param  pArrCallInfo(optional)
 *          - See @ref voice_arrCallInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pArrRemotePartyNum(optional)
 *          - See @ref voice_arrRemotePartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pArrRemotePartyName(optional)
 *          - See @ref voice_arrRemotePartyName for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pArrAlertingType(optional)
 *          - See @ref voice_arrAlertingType for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pArrUUSInfo(optional)
 *          - See @ref voice_arrUUSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pArrSvcOption(optional)
 *          - See @ref voice_arrSvcOption for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pOTASPStatus(optional)
 *          - OTASP status for the OTASP call.
 *          - Applicable only for 3GPP2 devices.
 *              - 0x00 - OTASP_STATUS_SPL_UNLOCKED -
 *              SPL unlocked; only for user-initiated OTASP
 *              - 0x01 - OTASP_STATUS_SPRC_RETRIES_EXCEEDED -
 *                 SPC retries exceeded; only for user-initiated OTASP
 *              - 0x02 - OTASP_STATUS_AKEY_EXCHANGED -
 *                 A-key exchanged; only for user-initiated OTASP
 *              - 0x03 - OTASP_STATUS_SSD_UPDATED - SSD updated; for both
 *                 user-initiated OTASP and network-initiated OTASP (OTAPA)
 *              - 0x04 - OTASP_STATUS_NAM_DOWNLOADED - NAM downloaded;
 *                 only for user-initiated OTASP
 *              - 0x05 - OTASP_STATUS_MDN_DOWNLOADED - MDN downloaded;
 *                 only for user-initiated OTASP
 *              - 0x06 - OTASP_STATUS_IMSI_DOWNLOADED - IMSI downloaded;
 *                 only for user-initiated OTASP
 *              - 0x07 - OTASP_STATUS_PRL_DOWNLOADED - PRL downloaded;
 *                 only for user-initiated OTASP
 *              - 0x08 - OTASP_STATUS_COMMITTED - Commit successful;
 *                 only for user-initiated OTASP
 *              - 0x09 - OTASP_STATUS_OTAPA_STARTED - OTAPA started;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0A - OTASP_STATUS_OTAPA_STOPPED - OTAPA stopped;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0B - OTASP_STATUS_OTAPA_ABORTED - OTAPA aborted;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0x0C - OTASP_STATUS_OTAPA_COMMITTED - OTAPA committed;
 *                 only for network-initiated OTASP (OTAPA)
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pVoicePrivacy(optional)
 *          - Voice Privacy.
 *          - Values.
 *              - 0x00 - VOICE_PRIVACY_STANDARD - Standard privacy
 *              - 0x01 - VOICE_PRIVACY_ENHANCED - Enhanced privacy
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pArrCallEndReason(optional)
 *          - See @ref voice_arrCallEndReason for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pArrAlphaID(optional)
 *          - See @ref voice_arrAlphaID for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pArrConnectPartyNum(optional)
 *          - See @ref voice_arrConnectPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pArrDiagInfo(optional)
 *          - See @ref voice_arrDiagInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pArrCalledPartyNum(optional)
 *          - See @ref voice_arrCalledPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pArrRedirPartyNum(optional)
 *          - See @ref voice_arrRedirPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pArrAlertingPattern(optional)
 *          - See @ref voice_arrAlertingPattern for more information.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    voice_arrCallInfo        *pArrCallInfo;
    voice_arrRemotePartyNum  *pArrRemotePartyNum;
    voice_arrRemotePartyName *pArrRemotePartyName;
    voice_arrAlertingType    *pArrAlertingType;
    voice_arrUUSInfo         *pArrUUSInfo;
    voice_arrSvcOption       *pArrSvcOption;
    uint8_t                  *pOTASPStatus;
    uint8_t                  *pVoicePrivacy;
    voice_arrCallEndReason   *pArrCallEndReason;
    voice_arrAlphaID         *pArrAlphaID;
    voice_arrConnectPartyNum *pArrConnectPartyNum;
    voice_arrDiagInfo        *pArrDiagInfo;
    voice_arrCalledPartyNum  *pArrCalledPartyNum;
    voice_arrRedirPartyNum   *pArrRedirPartyNum;
    voice_arrAlertingPattern *pArrAlertingPattern;
    swi_uint256_t            ParamPresenceMask;
} unpack_voice_SLQSVoiceGetAllCallInfo_t;

/**
 * information associated with all the calls originating or terminating unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceGetAllCallInfo(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceGetAllCallInfo_t *pOutput
      );

/**
 *  This structure contains Manage Calls Information.
 *
 *  @param  SUPSType
 *          - Supplementary service type during the call.
 *            - 0x01 - SUPS_TYPE_RELEASE_HELD_OR_WAITING
 *                     - Release is held or waiting
 *            - 0x02 - SUPS_TYPE_RELEASE_ACTIVE_ACCEPT_HELD_OR_WAITING
 *                     - Release is active and accepting held or waiting
 *            - 0x03 - SUPS_TYPE_HOLD_ACTIVE_ACCEPT_WAITING_OR_HELD
 *                     - Hold is active and accepting waiting or held
 *            - 0x04 - SUPS_TYPE_HOLD_ALL_EXCEPT_SPECIFIED_CALL
 *                     - Hold all calls except a specified one
 *            - 0x05 - SUPS_TYPE_MAKE_CONFERENCE_CALL
 *                     - Make a conference call
 *            - 0x06 - SUPS_TYPE_EXPLICIT_CALL_TRANSFER
 *                     - Explicit call transfer
 *            - 0x07 - SUPS_TYPE_CCBS_ACTIVATION
 *                     - Activate completion of calls to busy subscriber
 *            - 0x08 - SUPS_TYPE_END_ALL_CALLS
 *                     - End all calls
 *            - 0x09 - SUPS_TYPE_RELEASE_SPECIFIED_CALL
 *                     - Release a specified call
 *
 *  @param  pCallID[Optional]
 *          - Applicable only for SUPSType 0x04, 0x07, and 0x09
 *          - NULL pointer - Invalid data.
 *
 */
typedef struct
{
    uint8_t SUPSType;
    uint8_t *pCallID;
}pack_voice_SLQSVoiceManageCalls_t;

/**
 * Manages the calls by using the supplementary service pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceManageCalls(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceManageCalls_t *pReq);

/**
 *  This structure contains Failure cause Information.
 *  Populated when API Fails.
 *
 *  @param  pFailCause
 *          - Supplementary service failure causes (optional, supply NULL if
 *            not required).
 *          - See Table8 qaGobiApiTableVoiceCallEndReasons.h for supplementary
 *            services failure cause
 *              - 0xFFFF is the value when the information is not received from
 *                device
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t *pFailCause;
    swi_uint256_t  ParamPresenceMask;
}unpack_voice_SLQSVoiceManageCalls_t;

/**
 * Manages the calls by using the supplementary service unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceManageCalls(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceManageCalls_t *pOutput
      );

 /**
  *  This structure contains Voice Burst DTMF Information
  *
  *  @param  pCallID
  *          - Call ID associated with call on which the DTMF information
  *            has to be sent.\n A burst DTMF request is sent to the current
  *            active/alerting call when pCallId is set to 0xFF.
  *          - This is IN/OUT parameter, value passed by user will be packed in
  *            request and value received from the device would be returned to
  *            the user.
  *          - If the call ID value received is 0, no value has been
  *            returned by the device
  *          - NULL pointer - Invalid data.
  *
  *  @param  digitCnt
  *          - Length of DTMF digit buffer which follows
  *
  *  @param pDigitBuff[MAX_VOICE_DESCRIPTION_LENGTH]
  *         - DTMF digit buffer in ASCII, NULL terminated
  *
  */
 typedef struct
 {
     uint8_t *pCallID;
     uint8_t digitCnt;
     uint8_t pDigitBuff[MAX_VOICE_DESCRIPTION_LENGTH];
 }voice_burstDTMFInfo;

 /**
  *  This structure contains Voice Burst DTMF pulse length information
  *
  *  @param  DTMFPulseWidth
  *          - DTMF pulse width. Values:
  *            - 0x00 - DTMF_ONLENGTH_95MS - 95 ms
  *            - 0x01 - DTMF_ONLENGTH_150MS - 150 ms
  *            - 0x02 - DTMF_ONLENGTH_200MS - 200 ms
  *            - 0x03 - DTMF_ONLENGTH_250MS - 250 ms
  *            - 0x04 - DTMF_ONLENGTH_300MS - 300 ms
  *            - 0x05 - DTMF_ONLENGTH_350MS - 350 ms
  *            - 0x06 - DTMF_ONLENGTH_SMS  SMS Tx special pulse width
  *
  *  @param  DTMFInterdigitInterval
  *          - DTMF interdigit interval Values:
  *            - 0x00 - DTMF_OFFLENGTH_60MS  - 60 ms
  *            - 0x01 - DTMF_OFFLENGTH_100MS - 100 ms
  *            - 0x02 - DTMF_OFFLENGTH_150MS - 150 ms
  *            - 0x03 - DTMF_OFFLENGTH_200MS - 200 ms
  *
  */
 typedef struct
 {
     uint8_t DTMFPulseWidth;
     uint8_t DTMFInterdigitInterval;
 }voice_DTMFLengths;

/**
 *  This structure contains parameters of burst Dual-Tone Multifrequency (DTMF)
 *
 *  @param  BurstDTMFInfo
 *          - Burst DTMF Information
 *            - See @ref voice_burstDTMFInfo for more information
 *  @param  pBurstDTMFLengths [optional]
 *          - DTMF Lengths
 *            - See @ref voice_DTMFLengths for more information
 *          - NULL pointer - Invalid data.
 *
 */
typedef struct
{
    voice_burstDTMFInfo BurstDTMFInfo;
    voice_DTMFLengths   *pBurstDTMFLengths;
}pack_voice_SLQSVoiceBurstDTMF_t;

/**
 * Sends a burst Dual-Tone Multi frequency (DTMF) pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceBurstDTMF(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceBurstDTMF_t *pReq);

 /**
 *  This structure contains Voice Burst DTMF Information for unpack.
 *
 *  @param  pCallID
 *          - Call ID associated with call on which the DTMF information
 *            has to be sent.\n A burst DTMF request is sent to the current
 *            active/alerting call when pCallId is set to 0xFF.
 *          - This is IN/OUT parameter, value passed by user will be packed in
 *            request and value received from the device would be returned to
 *            the user.
 *          - If the call ID value received is 0, no value has been
 *            returned by the device
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t *pCallID;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceBurstDTMF_t;

/**
 * Sends a burst Dual-Tone Multi frequency (DTMF) unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceBurstDTMF(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceBurstDTMF_t *pOutput
      );

/**
 *  This structure contains parameters of continuous DTMF
 *
 *  @param  pCallID
 *          - Call ID associated with call on which the DTMF information
 *            has to be sent.\n Start continuous DTMF request is sent to the
 *            current active/alerting call when pCallId is set to 0xFF.
 *          - This is IN/OUT parameter, value passed by user will be packed in
 *            request and value received from the device would be returned to
 *            the user.
 *          - If the call ID value received is 0, no value has been
 *            returned by the device
 *
 *  @param  DTMFdigit
 *          - DTMF digit in ASCII.
 *
 */
typedef struct
{
    uint8_t *pCallID;
    uint8_t DTMFdigit;
} pack_voice_SLQSVoiceStartContDTMF_t;

/**
 * Starts a continuous DTMF pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceStartContDTMF(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceStartContDTMF_t *pReq);

/**
 *  This structure contains parameters of continuous DTMF
 *
 *  @param  pCallID
 *          - Call ID associated with call on which the DTMF information
 *            has to be sent.\n Start continuous DTMF request is sent to the
 *            current active/alerting call when pCallId is set to 0xFF.
 *          - This is IN/OUT parameter, value passed by user will be packed in
 *            request and value received from the device would be returned to
 *            the user.
 *          - If the call ID value received is 0, no value has been
 *            returned by the device
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t *pCallID;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SLQSVoiceStartContDTMF_t;

/**
 * Starts a continuous DTMF unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceStartContDTMF(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceStartContDTMF_t *pOutput
      );

/**
 *  This structure contains parameters of stop continuous DTMF
 *
 *  @param  pCallID
 *          - Call ID associated with call on which the DTMF information
 *            has to be sent.\n Stop continuous DTMF request is sent to the
 *            current active/alerting call when pCallId is set to 0xFF.
 *          - This is IN/OUT parameter, value passed by user will be packed in
 *            request and value received from the device would be returned to
 *            the user.
 *          - If the call ID value received is 0, no value has been
 *            returned by the device
 *
 */
typedef struct
{
    uint8_t callID;
}pack_voice_SLQSVoiceStopContDTMF_t;

/**
 * Stops a continuous DTMF pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceStopContDTMF(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceStopContDTMF_t *pReq);

/**
 *  This structure contains parameters of stop continuous DTMF
 *
 *  @param  pCallID
 *          - Call ID associated with call on which the DTMF information
 *            has to be sent.\n Stop continuous DTMF request is sent to the
 *            current active/alerting call when pCallId is set to 0xFF.
 *          - This is IN/OUT parameter, value passed by user will be packed in
 *            request and value received from the device would be returned to
 *            the user.
 *          - If the call ID value received is 0, no value has been
 *            returned by the device
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t callID;
    swi_uint256_t  ParamPresenceMask;
}unpack_voice_SLQSVoiceStopContDTMF_t;

/**
 * Stops a continuous DTMF unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note return eQCWWAN_ERR_NULL_TLV then callID value is invalid.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_voice_SLQSVoiceStopContDTMF(
       uint8_t   *pResp,
       uint16_t  respLen,
       unpack_voice_SLQSVoiceStopContDTMF_t *pOutput
      );

/**
 *  This structure contains the flash information associated with a call.
 *
 *  @param  pCallID
 *          - Unique call identifier associated with the current call.
 *
 *  @param  pFlashPayLd(optional)
 *          - Payload in ASCII to be sent in Flash.
 *          - Variable Length, NULL terminated.
 *
 *  @param  pFlashType(optional)
 *          - Flash type.
 *              - 0 - Simple Flash (default)
 *              - 1 - Activate answer hold
 *              - 2 - Deactivate answer hold
 *
 */
typedef struct
{
    uint8_t *pCallID;
    uint8_t *pFlashPayLd;
    uint8_t *pFlashType;
} pack_voice_SLQSVoiceSendFlash_t;

/**
 * sends a simple flash message pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_voice_SLQSVoiceSendFlash(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_voice_SLQSVoiceSendFlash_t *pReq);

/**
 *  This structure contains the flash information associated with a call.
 *
 *  @param  pCallID
 *          - Unique call identifier associated with the current call.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
 typedef struct
 {
     uint8_t *pCallID;
     swi_uint256_t  ParamPresenceMask;
 } unpack_voice_SLQSVoiceSendFlash_t;

/**
 * sends a simple flash message unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceSendFlash(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSVoiceSendFlash_t *pOutput
     );

/**
 *  This structure contains the preferred voice privacy values.
 *
 *  @param  privacyPref
 *          - Voice Privacy Preference
 *              - 0x00 - VOICE_PRIVACY_STANDARD - Standard privacy
 *              - 0x01 - VOICE_PRIVACY_ENHANCED - Enhanced privacy
 *
 */
typedef struct
{
    uint8_t privacyPref;
} pack_voice_SLQSVoiceSetPreferredPrivacy_t;

/**
 * sets the voice privacy preference pack
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
int pack_voice_SLQSVoiceSetPreferredPrivacy(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceSetPreferredPrivacy_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceSetPreferredPrivacy_t;

/**
 * sets the voice privacy preference unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceSetPreferredPrivacy(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceSetPreferredPrivacy_t *pOutput
        );

/**
 *  This structure contains parameters of Indication Register Information
 *
 *  @param  pRegDTMFEvents(optional)
 *          - Registration Indication For DTMF Events.
 *          - When this registration is enabled, the device learns of DTMF
 *            events via the QMI_VOICE_DTMF_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pRegVoicePrivacyEvents(optional)
 *          - Registration Indication For Voice Privacy Events.
 *          - When this registration is enabled, the device learns of DTMF
 *            events via the QMI_VOICE_PRIVACY_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSuppsNotifEvents(optional)
 *          - Registration Indication For Supplementary Service Notification
 *            Events.
 *          - When this registration is enabled, the device learns of DTMF
 *            events via the QMI_VOICE_SUPS_NOTIFICATION_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @note   One of the optional parameter is mandatory to be present in the
 *          request.
 */
typedef struct
{
    uint8_t *pRegDTMFEvents;
    uint8_t *pRegVoicePrivacyEvents;
    uint8_t *pSuppsNotifEvents;
} pack_voice_SLQSVoiceIndicationRegister_t;

/**
 * Sets the registration state for different QMI_VOICE indications pack
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
int pack_voice_SLQSVoiceIndicationRegister(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceIndicationRegister_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceIndicationRegister_t;

/**
 * Sets the registration state for different QMI_VOICE indications unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceIndicationRegister(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceIndicationRegister_t *pOutput
        );

/**
 *  This structure contains Voice Get Configuration Request Parameters
 *
 *  @param  pAutoAnswer(optional)
 *          - Indicator to retrieve the Auto Answer Information.
 *              - 0x01 - Include auto answer information
 *
 *  @param  pAirTimer(optional)
 *          - Indicator to retrieve the Air Timer Information.
 *              - 0x01 - Include air calls timer count information
 *          - Currently Not Supported.
 *
 *  @param  pRoamTimer(optional)
 *          - Indicator to retrieve the Roam Timer Information.
 *              - 0x01 - Include roam calls timer information
 *          - Currently Not Supported.
 *
 *  @param  pTTYMode(optional)
 *          - Indicator to retrieve the TTY Mode Information.
 *              - 0x01 - Include TTY configuration status information
 *
 *  @param  pPrefVoiceSO(optional)
 *          - Indicator to retrieve the Preferred Voice SO Information.
 *              - 0x01 - Include preferred voice configuration status
 *                       information
 *          - Currently Not Supported.
 *
 *  @param  pAMRStatus(optional)
 *          - Indicator to retrieve the AMR Status Information.
 *              - 0x01 - Include AMR status information
 *
 *  @param  pPrefVoicePrivacy(optional)
 *          - Indicator to retrieve the Preferred Voice Privacy Information.
 *              - 0x01 - Include preferred voice privacy status information
 *
 *  @param  pNamID(optional)
 *          - Index of the Number Assignment Module Index (CDMA subscription)
 *            to be configured
 *          - Range: 0 to 3.
 *          - Some modems support only 1 or 2 NAMs.
 *          - The NAM Index is valid only when the request contains at least
 *            one of Air Timer, Roam Timer, and Preferred Voice SO.
 *          - If no nam_id value is specified in the request,the default value
 *            is 0.
 *
 *  @param  pVoiceDomainPref(optional)
 *          - Indicator to retrieve the Preferred Voice Domain Information.
 *              - 0x01 - Include voice domain preference information
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not returned.
 *
 */
typedef struct
{
    uint8_t *pAutoAnswer;
    uint8_t *pAirTimer;
    uint8_t *pRoamTimer;
    uint8_t *pTTYMode;
    uint8_t *pPrefVoiceSO;
    uint8_t *pAMRStatus;
    uint8_t *pPrefVoicePrivacy;
    uint8_t *pNamID;
    uint8_t *pVoiceDomainPref;
} pack_voice_SLQSVoiceGetConfig_t;

/**
 * retrieves various configuration parameters that control the modem behavior pack
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
int pack_voice_SLQSVoiceGetConfig(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceGetConfig_t *reqArg
        );

/**
 *  This structure contains the Current Adaptive Multi Rate Configuration
 *  Information.
 *
 *  @param  gsmAmrStat
 *          - GSM AMR Status
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *              - 0xFF - Not Available
 *
 *  @param  wcdmaAmrStat
 *          - WCDMA AMR Status
 *          - One or a combination of the following bitmask values:
 *              - Bit 0 - AMR codec advertised is not supported
 *              - Bit 1 - Controls WCDMA AMR wideband
 *              - Bit 2 - Controls GSM half rate AMR
 *              - Bit 3 - Controls GSM AMR wideband
 *              - Bit 4 - Controls GSM AMR narrowband
 *          - 0xFF, if not available
 *
 */
typedef struct
{
    uint8_t gsmAmrStat;
    uint8_t wcdmaAmrStat;
} voice_curAMRConfig;

/**
 *  This structure contains Voice Get Configuration Response Parameters.
 *
 *  @param  pAutoAnswerStat(optional)
 *          - Auto Answer Status
 *          - Value returned is read from NV_AUTO_ANSWER_I.
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAirTimerCnt(optional)
 *          - Air Timer Count
 *          - Value returned is read from NV_AIR_CNT_I.
 *          - See @ref voice_airTimer for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pRoamTimerCnt(optional)
 *          - Roam Timer Count
 *          - Value returned is read from NV_ROAM_CNT_I.
 *          - See @ref voice_roamTimer for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCurrTTYMode(optional)
 *          - Current TTY Mode
 *          - Value returned is read from NV_TTY_I.
 *              - 0x00 - TTY_MODE_FULL - Full
 *              - 0x01 - TTY_MODE_VCO - Voice carry over
 *              - 0x02 - TTY_MODE_HCO - Hearing carry over
 *              - 0x03 - TTY_MODE_OFF - Off
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCurPrefVoiceSO(optional)
 *          - Current Preferred Voice SO
 *          - Value returned is read from NV_PREF_VOICE_SO_I.
 *          - See @ref voice_prefVoiceSO for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCurAMRConfig(optional)
 *          - Current Adaptive Multi-Rate Configuration.
 *          - Values returned are read from NV_GSM_ARM_CALL_CONFIG_I and
 *            NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I.
 *          - See @ref voice_curAMRConfig for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pCurVoicePrivacyPref(optional)
 *          - Current Voice Privacy Preference
 *          - Value returned is read from NV_VOICE_PRIV_I.
 *              - 0x00 - Standard privacy
 *              - 0x01 - Enhanced privacy
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pCurVoiceDomainPref(optional)
 *          - Current Voice Domain Preference.
 *              - 0x00 - Circuit-switched (CS) only
 *              - 0x01 - Packet-switched (PS) only
 *              - 0x02 - CS is preferred; PS is secondary
 *              - 0x03 - PS is preferred; CS is secondary
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    uint8_t            *pAutoAnswerStat;
    voice_airTimer     *pAirTimerCnt;
    voice_roamTimer    *pRoamTimerCnt;
    uint8_t            *pCurrTTYMode;
    voice_prefVoiceSO  *pCurPrefVoiceSO;
    voice_curAMRConfig *pCurAMRConfig;
    uint8_t            *pCurVoicePrivacyPref;
    uint8_t            *pCurVoiceDomainPref;
    swi_uint256_t      ParamPresenceMask;
} unpack_voice_SLQSVoiceGetConfig_t;

/**
 * retrieves various configuration parameters that control the modem behavior unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceGetConfig(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSVoiceGetConfig_t *pOutput
     );

/**
 *  This structure contains Orig USSD No Wait Information Parameters.
 *
 *  @param  USSInformation
 *          - See @ref voice_USSInfo for more information.
 *
 */
typedef struct
{
    struct voice_USSInfo USSInformation;
} pack_voice_SLQSVoiceOrigUSSDNoWait_t;

/**
 * initiates a USSD operation No Wait pack
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
int pack_voice_SLQSVoiceOrigUSSDNoWait(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceOrigUSSDNoWait_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceOrigUSSDNoWait_t;

/**
 * initiates a USSD operation No Wait unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceOrigUSSDNoWait(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceOrigUSSDNoWait_t *pOutput
        );

/**
 *  This structure contains Bind Subscription Information Parameters.
 *
 *  @param  subsType
 *          - Subscription Type.
 *              - 0x00 - VOICE_SUBS_TYPE_PRIMARY - Primary
 *              - 0x01 - VOICE_SUBS_TYPE_SECONDARY - Secondary
 *
 */
typedef struct
{
    uint8_t subsType;
} pack_voice_SLQSVoiceBindSubscription_t;

/**
 * binds a subscription type to a specific voice client ID pack
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
int pack_voice_SLQSVoiceBindSubscription(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceBindSubscription_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceBindSubscription_t;

/**
 * binds a subscription type to a specific voice client ID unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceBindSubscription(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceBindSubscription_t *pOutput
        );

/**
 *  This structure contains ALS Set Line Switching Information Parameters.
 *
 *  @param  switchOption
 *          - Switch Option.
 *              - 0x00 - VOICE_LINE_SWITCHING_NOT_ALLOWED -
 *                 Line switching is not allowed
 *              - 0x01 - VOICE_LINE_SWITCHING_ALLOWED -
 *                 Line switching is allowed
 *
 */
typedef struct
{
    uint8_t switchOption;
} pack_voice_SLQSVoiceALSSetLineSwitching_t;

/**
 * sets the line switch setting on the card pack
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
int pack_voice_SLQSVoiceALSSetLineSwitching(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceALSSetLineSwitching_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceALSSetLineSwitching_t;

/**
 * sets the line switch setting on the card unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceALSSetLineSwitching(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceALSSetLineSwitching_t *pOutput
        );

/**
 *  This structure contains ALS Select Line Information Parameters.
 *
 *  @param  lineValue
 *          - ALS Line Value.
 *              - 0x00 - ALS_LINE1 - Line 1 (default)
 *              - 0x01 - ALS_LINE2 - Line 2
 *
 */
typedef struct
{
    uint8_t lineValue;
} pack_voice_SLQSVoiceALSSelectLine_t;

/**
 * allows the user to select the preferred line pack
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
int pack_voice_SLQSVoiceALSSelectLine(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSVoiceALSSelectLine_t *reqArg
        );

typedef unpack_result_t  unpack_voice_SLQSVoiceALSSelectLine_t;

/**
 * allows the user to select the preferred line unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceALSSelectLine(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceALSSelectLine_t *pOutput
        );

/**
 * status of the Connected Line Identification Presentation pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_voice_SLQSVoiceGetCOLP(
       pack_qmi_t  *pCtx,
       uint8_t     *pReqBuf,
       uint16_t    *pLen
       );

/**
 *  This structure contains information about the Connected Line Identification
 *  Presentation (COLP) supplementary service responses.
 *
 *  @param  ActiveStatus
 *          - Active status.
 *          - Values:
 *              - 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
 *              - 0x01 - ACTIVE_STATUS_ACTIVE - Active
 *              - 0xFF - Not Available
 *
 *  @param  ProvisionStatus
 *          - Provisioned status.
 *          - Values:
 *              - 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
 *              - 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t ActiveStatus;
    uint8_t ProvisionStatus;
} voice_COLPResp;

/**
 *  This structure contains Voice Get Connected Line Identification
 *  Presentation(COLP) Response Parameters
 *
 *  @param  pCOLPResp
 *          - Pointer to structure of COLPResp (optional)
 *              - See @ref voice_COLPResp for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    voice_COLPResp     *pCOLPResp;
    uint16_t           *pFailCause;
    voice_alphaIDInfo  *pAlphaIDInfo;
    uint8_t            *pCCResType;
    uint8_t            *pCallID;
    voice_ccSUPSType   *pCCSUPSType;
    swi_uint256_t      ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCOLP_t;

/**
 * status of the Connected Line Identification Presentation unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceGetCOLP(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSVoiceGetCOLP_t *pOutput
     );

/**
 * status of the Connected Line Identification Restriction pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_voice_SLQSVoiceGetCOLR(
       pack_qmi_t  *pCtx,
       uint8_t     *pReqBuf,
       uint16_t    *pLen
       );

/**
 *  This structure contains information about the Connected Line Identification
 *  Restriction (COLR) supplementary service responses.
 *
 *  @param  ActiveStatus
 *          - Active status.
 *          - Values:
 *              - 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
 *              - 0x01 - ACTIVE_STATUS_ACTIVE - Active
 *              - 0xFF - Not Available
 *
 *  @param  ProvisionStatus
 *          - Provisioned status.
 *          - Values:
 *              - 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
 *              - 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t ActiveStatus;
    uint8_t ProvisionStatus;
} voice_COLRResp;

/**
 *  This structure contains Voice Get Connected Line Identification
 *  Restriction(COLR) Response Parameters
 *
 *  @param  pCOLRResp
 *          - Pointer to structure of COLRResp (optional)
 *              - See @ref voice_COLRResp for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    voice_COLRResp    *pCOLRResp;
    uint16_t          *pFailCause;
    voice_alphaIDInfo *pAlphaIDInfo;
    uint8_t           *pCCResType;
    uint8_t           *pCallID;
    voice_ccSUPSType  *pCCSUPSType;
    swi_uint256_t     ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCOLR_t;

/**
 * status of the Connected Line Identification Restriction unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceGetCOLR(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSVoiceGetCOLR_t *pOutput
     );

/**
 * status of the Calling Name Presentation(CNAP) pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_voice_SLQSVoiceGetCNAP(
       pack_qmi_t  *pCtx,
       uint8_t     *pReqBuf,
       uint16_t    *pLen
       );

/**
 *  This structure contains information about the Calling Name Presentation
 *  (CNAP) supplementary service responses.
 *
 *  @param  ActiveStatus
 *          - Active status.
 *          - Values:
 *              - 0x00 - ACTIVE_STATUS_INACTIVE - Inactive
 *              - 0x01 - ACTIVE_STATUS_ACTIVE - Active
 *              - 0xFF - Not Available
 *
 *  @param  ProvisionStatus
 *          - Provisioned status.
 *          - Values:
 *              - 0x00 - PROVISION_STATUS_NOT_PROVISIONED - Not provisioned
 *              - 0x01 - PROVISION_STATUS_PROVISIONED - Provisioned
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t ActiveStatus;
    uint8_t ProvisionStatus;
} voice_CNAPResp;

/**
 *  This structure contains Voice Get Calling Name
 *  Presentation(CNAP) Response Parameters
 *
 *  @param  pCNAPResp
 *          - Pointer to structure of CNAPResp (optional)
 *              - See @ref voice_CNAPResp for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFailCause
 *          - Supplementary services failure cause (optional)
 *          - see qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - 0xFFFF,if Not Available
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAlphaIDInfo
 *          - Pointer to structure of alphaIDInfo (optional)
 *              - See @ref voice_alphaIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCCResType
 *          - Call Control Result Type (optional)
 *              - 0x00 - CC_RESULT_TYPE_VOICE - Voice
 *              - 0x01 - CC_RESULT_TYPE_SUPS - Supplementary service
 *              - 0x02 - CC_RESULT_TYPE_USSD -
 *                  Unstructured supplementary service
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pCallID
 *          - Call ID of the voice call that resulted from call
 *            control. (optional)
 *          - It is present when pCCResType is present and is Voice.
 *          - If zero(0) then invalid.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCCSUPSType
 *          - Supplementary service data that resulted from call
 *            control (optional)
 *          - Data is present when pCCResultType is present and
 *            is other than Voice.
 *              - See @ref voice_ccSUPSType for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    voice_CNAPResp     *pCNAPResp;
    uint16_t           *pFailCause;
    voice_alphaIDInfo  *pAlphaIDInfo;
    uint8_t            *pCCResType;
    uint8_t            *pCallID;
    voice_ccSUPSType   *pCCSUPSType;
    swi_uint256_t      ParamPresenceMask;
} unpack_voice_SLQSVoiceGetCNAP_t;

/**
 * status of the Calling Name Presentation(CNAP) unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceGetCNAP(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSVoiceGetCNAP_t *pOutput
     );

/**
 *  This structure contains USS Information
 *
 *  @param  ussDCS
 *          - 1 - ASCII coding scheme
 *          - 2 - 8-BIT coding scheme
 *          - 3 - UCS2
 *
 *  @param  ussLen
 *          - Range 1 to 182
 *
 *  @param  ussData
 *          - Data encoded as per the DCS
 *
 */
typedef struct
{
    uint8_t ussDCS;
    uint8_t ussLen;
    uint8_t ussData[MAXVOICEUSSDLENGTH];
} pack_voice_SLQSOriginateUSSD_t;

/**
 * Initiates a USSD session pack
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
int pack_voice_SLQSOriginateUSSD(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_voice_SLQSOriginateUSSD_t *reqArg
        );

/**
 * This structure contains the parameters of USS response.
 * @param pfailureCause
 *          - Failure Cause.
 *          - 0 - QMI_FAILURE_CAUSE_OFFLINE - Phone is offline 
 *          - 20 - QMI_FAILURE_CAUSE_CDMA_LOCK - Phone is CDMA locked until a power cycle; CDMA only 
 *          - 21 - QMI_FAILURE_CAUSE_NO_SRV - Phone has no service 
 *          - 22 - QMI_FAILURE_CAUSE_FADE - Call has ended abnormally 
 *          - 23 - QMI_FAILURE_CAUSE_INTERCEPT - Received intercept from the base station; originating only; CDMA only 
 *          - 24 - QMI_FAILURE_CAUSE_REORDER - Received reorder from the base station; originating only; CDMA only 
 *          - 25 - QMI_FAILURE_CAUSE_REL_NORMAL - Received release from the base station; no reason was given 
 *          - 26 - QMI_FAILURE_CAUSE_REL_SO_REJ - Received release from the base station; SO reject; CDMA only 
 *          - 27 - QMI_FAILURE_CAUSE_INCOM_CALL - Received incoming call from the base station 
 *          - 28 - QMI_FAILURE_CAUSE_ALERT_STOP - Received alert stop from the base station; incoming only; CDMA only 
 *          - 29 - QMI_FAILURE_CAUSE_CLIENT_END - Client ended the call 
 *          - 30 - QMI_FAILURE_CAUSE_ACTIVATION - Received end activation; OTASP call only; CDMA only 
 *          - 31 - QMI_FAILURE_CAUSE_MC_ABORT - MC aborted the origination/conversation; CDMA only 
 *          - 32 - QMI_FAILURE_CAUSE_MAX_ACCESS_PROBE - Maximum access probes were transmitted; CDMA only 
 *          - 33 - QMI_FAILURE_CAUSE_PSIST_N - Persistence test failure; FEATURE_JCDMA only; CDMA only 
 *          - 34 - QMI_FAILURE_CAUSE_UIM_NOT_PRESENT - R-UIM is not present 
 *          - 35 - QMI_FAILURE_CAUSE_ACC_IN_PROG - Access attempt is already in progress 
 *          - 36 - QMI_FAILURE_CAUSE_ACC_FAIL - Access failure for a reason other than the above 
 *          - 37 - QMI_FAILURE_CAUSE_RETRY_ORDER - Received retry order; originating only; IS 2000; CDMA only 
 *          - 38 - QMI_FAILURE_CAUSE_CCS_NOT_SUPPORTED_BYBS - Concurrent service is not supported by the base station 
 *          - 39 - QMI_FAILURE_CAUSE_NO_RESPONSE_FROM_BS - No response was received from the base station 
 *          - 40 - QMI_FAILURE_CAUSE_REJECTED_BY_BS - Call was rejected by the base station; CDMA only 
 *          - 41 - QMI_FAILURE_CAUSE_INCOMPATIBLE - Concurrent services requested were not compatible; CDMA only 
 *          - 42 - QMI_FAILURE_CAUSE_ACCESS_BLOCK - Access is blocked by the base station; CDMA only 
 *          - 43 - QMI_FAILURE_CAUSE_ALREADY_IN_TC - Corresponds to CM_CALL_ORIGERR_ALREADY_IN_TC 
 *          - 44 - QMI_FAILURE_CAUSE_EMERGENCY_FLASHED - Call is ended because an emergency call was flashed over this call; CDMA only 
 *          - 45 - QMI_FAILURE_CAUSE_USER_CALL_ORIGDURING_GPS - Used if CM is ending a GPS call in preference of a user call 
 *          - 46 - QMI_FAILURE_CAUSE_USER_CALL_ORIGDURING_SMS - Used if CM is ending an SMS call in preference of a user call 
 *          - 47 - QMI_FAILURE_CAUSE_USER_CALL_ORIGDURING_DATA - Used if CM is ending a data call in preference of an emergency call 
 *          - 48 - QMI_FAILURE_CAUSE_REDIR_OR_HANDOFF - Call was rejected because of a redirection or handoff 
 *          - 49 - QMI_FAILURE_CAUSE_ACCESS_BLOCK_ALL - Access is blocked by the base station for all mobiles; KDDI-specific; CDMA only 
 *          - 50 - QMI_FAILURE_CAUSE_OTASP_SPC_ERR - To support OTASP SPC Error indication  
 *          - 51 - QMI_FAILURE_CAUSE_IS707B_MAX_ACC - Maximum access probes for an IS-707B call; CDMA only 
 *          - 52 - QMI_FAILURE_CAUSE_ACC_FAIL_REJ_ORD - Base station reject order 
 *          - 53 - QMI_FAILURE_CAUSE_ACC_FAIL_RETRY_ORD - Base station retry order 
 *          - 54 - QMI_FAILURE_CAUSE_TIMEOUT_T42 - Timer T42 is expired 
 *          - 55 - QMI_FAILURE_CAUSE_TIMEOUT_T40 - Timer T40 is expired 
 *          - 56 - QMI_FAILURE_CAUSE_SRV_INIT_FAIL - Service initialization failure 
 *          - 57 - QMI_FAILURE_CAUSE_T50_EXP - Timer T50m is expired 
 *          - 58 - QMI_FAILURE_CAUSE_T51_EXP - Timer T51m is expired 
 *          - 59 - QMI_FAILURE_CAUSE_RL_ACK_TIMEOUT - Acknowledgement timeout due to 12 retransmissions 
 *          - 60 - QMI_FAILURE_CAUSE_BAD_FL - Bad forward link or timer T5M is expired 
 *          - 61 - QMI_FAILURE_CAUSE_TRM_REQ_FAIL - Transceiver Resource Manager request failed 
 *          - 62 - QMI_FAILURE_CAUSE_TIMEOUT_T41 - Timer T41 is expired 
 *          - 102 - QMI_FAILURE_CAUSE_INCOM_REJ - WCDMA/GSM only; client rejected an incoming call 
 *          - 103 - QMI_FAILURE_CAUSE_SETUP_REJ - WCDMA/GSM only; client rejected a setup indication 
 *          - 104 - QMI_FAILURE_CAUSE_NETWORK_END - WCDMA/GSM only; network ended the call 
 *          - 105 - QMI_FAILURE_CAUSE_NO_FUNDS - WCDMA/GSM only 
 *          - 106 - QMI_FAILURE_CAUSE_NO_GW_SRV - GSM/WCDMA only; phone has no service 
 *          - 107 - QMI_FAILURE_CAUSE_NO_CDMA_SRV - 1X only; phone has no service 
 *          - 108 - QMI_FAILURE_CAUSE_NO_FULL_SRV - Full service is unavailable 
 *          - 109 - QMI_FAILURE_CAUSE_MAX_PS_CALLS - Indicates resources are not available to handle a new MO/MT PS call Supplementary service errors 
 *          - 110 - QMI_FAILURE_CAUSE_UNKNOWN_SUBSCRIBER - Refer to \ref Table10 Section 4.5 
 *          - 111 - QMI_FAILURE_CAUSE_ILLEGAL_SUBSCRIBER - Refer to \ref Table10 Section 4.5 
 *          - 112 - QMI_FAILURE_CAUSE_BEARER_SERVICE_NOTPROVISIONED - Refer to \ref Table10 Section 4.5 
 *          - 113 - QMI_FAILURE_CAUSE_TELE_SERVICE_NOTPROVISIONED - Refer to \ref Table10 Section 4.5 
 *          - 114 - QMI_FAILURE_CAUSE_ILLEGAL_EQUIPMENT - Refer to \ref Table10 Section 4.5 
 *          - 115 - QMI_FAILURE_CAUSE_CALL_BARRED - Refer to \ref Table10 Section 4.5 
 *          - 116 - QMI_FAILURE_CAUSE_ILLEGAL_SS_OPERATION - Refer to \ref Table10 Section 4.5 
 *          - 117 - QMI_FAILURE_CAUSE_SS_ERROR_STATUS - Refer to \ref Table10 Section 4.5 
 *          - 118 - QMI_FAILURE_CAUSE_SS_NOT_AVAILABLE - Refer to \ref Table10 Section 4.5 
 *          - 119 - QMI_FAILURE_CAUSE_SS_SUBSCRIPTIONVIOLATION - Refer to \ref Table10 Section 4.5 
 *          - 120 - QMI_FAILURE_CAUSE_SS_INCOMPATIBILITY - Refer to \ref Table10 Section 4.5 
 *          - 121 - QMI_FAILURE_CAUSE_FACILITY_NOTSUPPORTED - Refer to \ref Table10 Section 4.5 
 *          - 122 - QMI_FAILURE_CAUSE_ABSENT_SUBSCRIBER - Refer to \ref Table10 Section 4.5 
 *          - 123 - QMI_FAILURE_CAUSE_SHORT_TERM_DENIAL - Refer to \ref Table10 Section 4.5 
 *          - 124 - QMI_FAILURE_CAUSE_LONG_TERM_DENIAL - Refer to \ref Table10 Section 4.5 
 *          - 125 - QMI_FAILURE_CAUSE_SYSTEM_FAILURE - Refer to \ref Table10 Section 4.5 
 *          - 126 - QMI_FAILURE_CAUSE_DATA_MISSING - Refer to \ref Table10 Section 4.5 
 *          - 127 - QMI_FAILURE_CAUSE_UNEXPECTED_DATAVALUE - Refer to \ref Table10 Section 4.5 
 *          - 128 - QMI_FAILURE_CAUSE_PWD_REGISTRATIONFAILURE - Refer to \ref Table10 Section 4.5 
 *          - 129 - QMI_FAILURE_CAUSE_NEGATIVE_PWD_CHECK - Refer to \ref Table10 Section 4.5 
 *          - 130 - QMI_FAILURE_CAUSE_NUM_OF_PWDATTEMPTS_VIOLATION - Refer to \ref Table10 Section 4.5 
 *          - 131 - QMI_FAILURE_CAUSE_POSITION_METHODFAILURE - Refer to \ref Table10 Section 4.5 
 *          - 132 - QMI_FAILURE_CAUSE_UNKNOWN_ALPHABET - Refer to \ref Table10 Section 4.5 
 *          - 133 - QMI_FAILURE_CAUSE_USSD_BUSY - Refer to \ref Table10 Section 4.5 
 *          - 134 - QMI_FAILURE_CAUSE_REJECTED_BY_USER - Refer to \ref Table10 Section 4.5  
 *          - 135 - QMI_FAILURE_CAUSE_REJECTED_BY_NETWORK - Refer to \ref Table10 Section 4.5 
 *          - 136 - QMI_FAILURE_CAUSE_DEFLECTION_TOSERVED_SUBSCRIBER - Refer to \ref Table10 Section 4.5 
 *          - 137 - QMI_FAILURE_CAUSE_SPECIAL_SERVICE_CODE - Refer to \ref Table10 Section 4.5 
 *          - 138 - QMI_FAILURE_CAUSE_INVALID_DEFLECTEDTO_NUMBER - Refer to \ref Table10 Section 4.5 
 *          - 139 - QMI_FAILURE_CAUSE_MPTY_PARTICIPANTSEXCEEDED - Refer to \ref Table10 Section 4.5 
 *          - 140 - QMI_FAILURE_CAUSE_RESOURCES_NOTAVAILABLE - Refer to \ref Table10 Section 4.5 Call control cause values 
 *          - 141 - QMI_FAILURE_CAUSE_UNASSIGNED_NUMBER - Refer to \ref Table10 Annex H 
 *          - 142 - QMI_FAILURE_CAUSE_NO_ROUTE_TODESTINATION - Refer to \ref Table10 Annex H 
 *          - 143 - QMI_FAILURE_CAUSE_CHANNELUNACCEPTABLE - Refer to \ref Table10 Annex H 
 *          - 144 - QMI_FAILURE_CAUSE_OPERATORDETERMINED_BARRING - Refer to \ref Table10 Annex H 
 *          - 145 - QMI_FAILURE_CAUSE_NORMAL_CALLCLEARING - Refer to \ref Table10 Annex H 
 *          - 146 - QMI_FAILURE_CAUSE_USER_BUSY - Refer to \ref Table10 Annex H 
 *          - 147 - QMI_FAILURE_CAUSE_NO_USER_RESPONDING - Refer to \ref Table10 Annex H 
 *          - 148 - QMI_FAILURE_CAUSE_USER_ALERTING_NOANSWER - Refer to \ref Table10 Annex H 
 *          - 149 - QMI_FAILURE_CAUSE_CALL_REJECTED - Refer to \ref Table10 Annex H 
 *          - 150 - QMI_FAILURE_CAUSE_NUMBER_CHANGED - Refer to \ref Table10 Annex H 
 *          - 151 - QMI_FAILURE_CAUSE_PREEMPTION - Refer to \ref Table10 Annex H 
 *          - 152 - QMI_FAILURE_CAUSE_DESTINATION_OUT_OFORDER - Refer to \ref Table10 Annex H 
 *          - 153 - QMI_FAILURE_CAUSE_INVALID_NUMBERFORMAT - Refer to \ref Table10 Annex H 
 *          - 154 - QMI_FAILURE_CAUSE_FACILITY_REJECTED - Refer to \ref Table10 Annex H 
 *          - 155 - QMI_FAILURE_CAUSE_RESP_TO_STATUSENQUIRY - Refer to \ref Table10 Annex H 
 *          - 156 - QMI_FAILURE_CAUSE_NORMAL_UNSPECIFIED - Refer to \ref Table10 Annex H 
 *          - 157 - QMI_FAILURE_CAUSE_NO_CIRCUIT_ORCHANNEL_AVAILABLE - Refer to \ref Table10 Annex H 
 *          - 158 - QMI_FAILURE_CAUSE_NETWORK_OUT_OFORDER - Refer to \ref Table10 Annex H 
 *          - 159 - QMI_FAILURE_CAUSE_TEMPORARY_FAILURE - Refer to \ref Table10 Annex H 
 *          - 160 - QMI_FAILURE_CAUSE_SWITCHING_EQUIPMENTCONGESTION - Refer to \ref Table10 Annex H 
 *          - 161 - QMI_FAILURE_CAUSE_ACCESS_INFORMATIONDISCARDED - Refer to \ref Table10 Annex H 
 *          - 162 - QMI_FAILURE_CAUSE_REQUESTED_CIRCUITOR_CHANNEL_NOT_AVAILABLE - Refer to \ref Table10 Annex H 
 *          - 163 - QMI_FAILURE_CAUSE_RESOURCESUNAVAILABLE_OR_UNSPECIFIED - Refer to \ref Table10 Annex H 
 *          - 164 - QMI_FAILURE_CAUSE_QOS_UNAVAILABLE - Refer to \ref Table10 Annex H 
 *          - 165 - QMI_FAILURE_CAUSE_REQUESTED_FACILITYNOT_SUBSCRIBED - Refer to \ref Table10 Annex H 
 *          - 166 - QMI_FAILURE_CAUSE_INCOMING_CALLSBARRED_WITHIN_CUG - Refer to \ref Table10 Annex H 
 *          - 167 - QMI_FAILURE_CAUSE_BEARER_CAPABILITYNOT_AUTH - Refer to \ref Table10 Annex H 
 *          - 168 - QMI_FAILURE_CAUSE_BEARER_CAPABILITYUNAVAILABLE - Refer to \ref Table10 Annex H 
 *          - 169 - QMI_FAILURE_CAUSE_SERVICE_OPTIONNOT_AVAILABLE - Refer to \ref Table10 Annex H 
 *          - 170 - QMI_FAILURE_CAUSE_ACM_LIMIT_EXCEEDED - Refer to \ref Table10 Annex H 
 *          - 171 - QMI_FAILURE_CAUSE_BEARER_SERVICE_NOTIMPLEMENTED - Refer to \ref Table10 Annex H 
 *          - 172 - QMI_FAILURE_CAUSE_REQUESTED_FACILITYNOT_IMPLEMENTED - Refer to \ref Table10 Annex H 
 *          - 173 - QMI_FAILURE_CAUSE_ONLY_DIGITALINFORMATION_BEARER_AVAILABLE - Refer to \ref Table10 Annex H 
 *          - 174 - QMI_FAILURE_CAUSE_SERVICE_OR_OPTIONNOT_IMPLEMENTED - Refer to \ref Table10 Annex H 
 *          - 175 - QMI_FAILURE_CAUSE_INVALID_TRANSACTIONIDENTIFIER - Refer to \ref Table10 Annex H 
 *          - 176 - QMI_FAILURE_CAUSE_USER_NOT_MEMBEROF_CUG - Refer to \ref Table10 Annex H 
 *          - 177 - QMI_FAILURE_CAUSE_INCOMPATIBLEDESTINATION - Refer to \ref Table10 Annex H 
 *          - 178 - QMI_FAILURE_CAUSE_INVALID_TRANSIT_NWSELECTION - Refer to \ref Table10 Annex H 
 *          - 179 - QMI_FAILURE_CAUSE_SEMANTICALLYINCORRECT_MESSAGE - Refer to \ref Table10 Annex H 
 *          - 180 - QMI_FAILURE_CAUSE_INVALID_MANDATORYINFORMATION - Refer to \ref Table10 Annex H 
 *          - 181 - QMI_FAILURE_CAUSE_MESSAGE_TYPE_NONIMPLEMENTED - Refer to \ref Table10 Annex H
 *          - 182 - QMI_FAILURE_CAUSE_MESSAGE_TYPE_NOTCOMPATIBLE_WITH_PROTOCOL_STATERefer - to [S3] Annex H
 *          - 183 - QMI_FAILURE_CAUSE_INFORMATION_ELEMENTNON_EXISTENTRefer - to [S3] Annex H
 *          - 184 - QMI_FAILURE_CAUSE_CONDITONAL_IE_ERROR - Refer to \ref Table10 Annex H
 *          - 185 - QMI_FAILURE_CAUSE_MESSAGE_NOTCOMPATIBLE_WITH_PROTOCOL_STATERefer - to [S3] Annex H
 *          - 186 - QMI_FAILURE_CAUSE_RECOVERY_ON_TIMEREXPIREDRefer - to [S3] Annex H
 *          - 187 - QMI_FAILURE_CAUSE_PROTOCOL_ERRORUNSPECIFIEDRefer - to [S3] Annex H
 *          - 188 - QMI_FAILURE_CAUSE_INTERWORKINGUNSPECIFIEDRefer - to [S3] Annex H
 *          - 189 - QMI_FAILURE_CAUSE_OUTGOING_CALLSBARRED_WITHIN_CUGRefer - to [S3] Annex H
 *          - 190 - QMI_FAILURE_CAUSE_NO_CUG_SELECTION - Refer to \ref Table10 Annex H
 *          - 191 - QMI_FAILURE_CAUSE_UNKNOWN_CUG_INDEX - Refer to \ref Table10 Annex H
 *          - 192 - QMI_FAILURE_CAUSE_CUG_INDEXINCOMPATIBLERefer - to [S3] Annex H
 *          - 193 - QMI_FAILURE_CAUSE_CUG_CALL_FAILUREUNSPECIFIEDRefer - to [S3] Annex H
 *          - 194 - QMI_FAILURE_CAUSE_CLIR_NOT_SUBSCRIBED - Refer to \ref Table10 Annex H
 *          - 195 - QMI_FAILURE_CAUSE_CCBS_POSSIBLE - Refer to \ref Table10 Annex H
 *          - 196 - QMI_FAILURE_CAUSE_CCBS_NOT_POSSIBLE - Refer to \ref Table10 Annex HMM/GMM reject causes
 *          - 197 - QMI_FAILURE_CAUSE_IMSI_UNKNOWN_IN_HLR - Refer to \ref Table10 Section 10.5.3.6
 *          - 198 - QMI_FAILURE_CAUSE_ILLEGAL_MS - Refer to \ref Table10 Section 10.5.3.6
 *          - 199 - QMI_FAILURE_CAUSE_IMSI_UNKNOWN_IN_VLR - Refer to \ref Table10 Section 10.5.3.6
 *          - 200 - QMI_FAILURE_CAUSE_IMEI_NOT_ACCEPTED - Refer to \ref Table10 Section 10.5.3.6
 *          - 201 - QMI_FAILURE_CAUSE_ILLEGAL_ME - Refer to \ref Table10 Section 10.5.3.6
 *          - 202 - QMI_FAILURE_CAUSE_PLMN_NOT_ALLOWED - Refer to \ref Table10 Section 10.5.3.6
 *          - 203 - QMI_FAILURE_CAUSE_LOCATION_AREA_NOTALLOWEDRefer - to [S3] Section 10.5.3.6
 *          - 204 - QMI_FAILURE_CAUSE_ROAMING_NOTALLOWED_IN_THIS_LOCATION_AREARefer - to [S3] Section 10.5.3.6
 *          - 205 - QMI_FAILURE_CAUSE_NO_SUITABLE_CELLSIN_LOCATION_AREARefer - to [S3] Section 10.5.3.6
 *          - 206 - QMI_FAILURE_CAUSE_NETWORK_FAILURE - Refer to \ref Table10 Section 10.5.3.6
 *          - 207 - QMI_FAILURE_CAUSE_MAC_FAILURE - Refer to \ref Table10 Section 10.5.3.6
 *          - 208 - QMI_FAILURE_CAUSE_SYNCH_FAILURE - Refer to \ref Table10 Section 10.5.3.6
 *          - 209 - QMI_FAILURE_CAUSE_NETWORK_CONGESTION - Refer to \ref Table10 Section 10.5.3.6
 *          - 210 - QMI_FAILURE_CAUSE_GSM_AUTHENTICATIONUNACCEPTABLERefer - to [S3] Section 10.5.3.6
 *          - 211 - QMI_FAILURE_CAUSE_SERVICE_NOTSUBSCRIBEDRefer - to [S3] Section 10.5.3.6
 *          - 212 - QMI_FAILURE_CAUSE_SERVICE_TEMPORARILYOUT_OF_ORDERRefer - to [S3] Section 10.5.3.6
 *          - 213 - QMI_FAILURE_CAUSE_CALL_CANNOT_BEIDENTIFIEDRefer - to [S3] Section 10.5.3.6
 *          - 214 - QMI_FAILURE_CAUSE_INCORRECT_SEMANTICSIN_MESSAGERefer - to [S3] Section 10.5.3.6
 *          - 215 - QMI_FAILURE_CAUSE_MANDATORYINFORMATION_INVALIDRefer - to [S3] Section 10.5.3.6
 *          - 216 - QMI_FAILURE_CAUSE_ACCESS_STRATUMFAILURECall - failed due to other accessstratum failures
 *          - 217 - QMI_FAILURE_CAUSE_INVALID_SIM - SIM is invalid
 *          - 218 - QMI_FAILURE_CAUSE_WRONG_STATE - Invalid call state
 *          - 229 - QMI_FAILURE_CAUSE_ACCESS_CLASS_BLOCKED - Access class is blocked
 *          - 220 - QMI_FAILURE_CAUSE_NO_RESOURCES - No resources are in the protocolstack to allow the call
 *          - 221 - QMI_FAILURE_CAUSE_INVALID_USER_DATA - Invalid user data was receivedMM reject causes
 *          - 222 - QMI_FAILURE_CAUSE_TIMER_T3230_EXPIRED - Timer T3230 is expired
 *          - 223 - QMI_FAILURE_CAUSE_NO_CELL_AVAILABLE - No cell is available
 *          - 224 - QMI_FAILURE_CAUSE_ABORT_MSG_RECEIVED - Abort message was received
 *          - 225 - QMI_FAILURE_CAUSE_RADIO_LINK_LOST - Radio link was lost due to otherlower layer causesCNM reject causes
 *          - 226 - QMI_FAILURE_CAUSE_TIMER_T303_EXPIRED - Timer T303 is expired
 *          - 227 - QMI_FAILURE_CAUSE_CNM_MM_REL_PENDING - CNM MM release is pendingAccess stratum reject causes
 *          - 228 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJRR_REL_INDAccess - stratum RR releaseindication
 *          - 229 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJRR_RANDOM_ACCESS_FAILUREAccess - stratum random accessfailure
 *          - 230 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJRRC_REL_INDAccess - stratum RRC releaseindication
 *          - 231 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJRRC_CLOSE_SESSION_INDAccess - stratum close sessionindication
 *          - 232 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJRRC_OPEN_SESSION_FAILUREAccess - stratum open sessionfailure
 *          - 233 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJLOW_LEVEL_FAILAccess - stratum low level failure
 *          - 234 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJLOW_LEVEL_FAIL_REDIAL_NOT_ALLOWEDAccess - stratum low level failureredial is not allowed
 *          - 235 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJLOW_LEVEL_IMMED_RETRYAccess - stratum low levelimmediate retry
 *          - 236 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJABORT_RADIO_UNAVAILABLEAccess - stratum abort radio isunavailableOTA reject causes
 *          - 237 - QMI_FAILURE_CAUSE_SERVICE_OPTION_NOTSUPPORTEDService - option is not supportedAdditional access stratum reject causes
 *          - 238 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJCONN_EST_FAILURE_ACCESS_BARREDAccess - stratum connectionestablishment failure access isbarred
 *          - 239 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJCONN_REL_NORMALAccess - stratum connectionrelease is normal
 *          - 240 - QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJUL_DATA_CNF_FAILURE_CONN_RELAccess - stratum UL dataconfirmation failure connectionwas released
 *          - 300 - QMI_FAILURE_CAUSE_BAD_REQ_WAIT_INVITE - Received SIP 400 bad request;waiting for INVITE response
 *          - 301 - QMI_FAILURE_CAUSE_BAD_REQ_WAITREINVITEReceived - SIP 400 bad request;waiting for INVITE response
 *          - 302 - QMI_FAILURE_CAUSE_INVALID_REMOTE_URI - Received SIP 404 not found; callfailed; called party does not exist
 *          - 303 - QMI_FAILURE_CAUSE_REMOTE_UNSUPP_MEDIA_TYPEReceived - SIP 415 unsupportedmedia type; call failed; calledparty does not support media
 *          - 304 - QMI_FAILURE_CAUSE_PEER_NOT_REACHABLE - Received SIP 480 temporarilyunavailable; call failed; calledparty is not in the LTE area
 *          - 305 - QMI_FAILURE_CAUSE_NETWORK_NO_RESP_TIME_OUTNo - network response; call failed
 *          - 306 - QMI_FAILURE_CAUSE_NETWORK_NO_RESP_HOLD_FAILNo - network response; unable toput call on hold
 *          - 307 - QMI_FAILURE_CAUSE_DATA_CONNECTION_LOST - Moved to eHRPD; call failed ordropped; not in the LTE area
 *          - 308 - QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_REJUpgrade/downgrade - rejected(200 OK with the current callSDP)
 *          - 309 - QMI_FAILURE_CAUSE_SIP_403_FORBIDDEN - Received 403 call forbidden;waiting for INVITE response
 *          - 310 - QMI_FAILURE_CAUSE_NO_NETWORK_RESP - Generic timeout; did not receivea response from the server orother end
 *          - 311 - QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_FAILEDReported - on the MO side forgeneric internal software errors;user can try again if the call stillexists
 *          - 312 - QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_CANCELLEDReported - on the MT side if theupgrade timer has beencancelled or cannot complete therequest for some reason afternotifying the user of a reinviterequest
 *          - 313 - QMI_FAILURE_CAUSE_SSAC_REJECT - Call origination is rejected dueto a Service-Specific AccessControl (SSAC) barring
 *          - 314 - QMI_FAILURE_CAUSE_THERMAL_EMERGENCY - Phone was put in thermalemergency
 *          - 315 - QMI_FAILURE_CAUSE_1XCSFB_SOFT_FAILURE - 1XCSFB call ended because of asoft failure
 *          - 316 - QMI_FAILURE_CAUSE_1XCSFB_HARD_FAILURE - 1XCSFB call ended because of ahard failure
 *          - 317 - QMI_FAILURE_CAUSE_CONNECTION_EST_FAILURERR/RRC - connectionestablishment procedure was notsuccessful
 *          - 318 - QMI_FAILURE_CAUSE_CONNECTION_FAILURE - After the connection wasestablished and a Page responsewas sent to the network, theconnection was dropped due toRLF
 *          - 319 - QMI_FAILURE_CAUSE_RRC_CONN_REL_NO_MT_SETUPRRC - connection was released bythe network without sending anMT Setup message
 *          - 320 - QMI_FAILURE_CAUSE_ESR_FAILURE - ESR failure; applicable only forLTE
 *          - 321 - QMI_FAILURE_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NWMT - circuit-switched fallbackfailure due to a release from thenetwork
 *          - 322 - QMI_FAILURE_CAUSE_BUSY_EVERYWHERE - MT call has ended due to arelease from the network(SIP 600)
 *          - 323 - QMI_FAILURE_CAUSE_ANSWERED_ELSEWHERE - MT call has ended due to arelease from the networkbecause the call was answeredelsewhere (SIP 200)
 *          - 324 - QMI_FAILURE_CAUSE_RLF_DURING_CC_DISCONNECTRadio - link failure wasencountered during theDisconnect state of the call
 *          - 325 - QMI_FAILURE_CAUSE_TEMP_REDIAL_ALLOWED - Call was ended and the user mayredial
 *          - 326 - QMI_FAILURE_CAUSE_PERM_REDIAL_NOT_NEEDEDCall - was ended with a permanentfailure and a redial is not needed
 *          - 327 - QMI_FAILURE_CAUSE_MERGED_TO_CONFERENCECall - was ended because it wasmerged to a conference call
 *          - 328 - QMI_FAILURE_CAUSE_LOW_BATTERY - Call was rejected by a peer dueto a low battery
 *          - 329 - QMI_FAILURE_CAUSE_CALL_DEFLECTED - Call was ended because the MTcall was deflected
 *          - 330 - QMI_FAILURE_CAUSE_RTP_RTCP_TIMEOUT - Call was terminated due to anRTP/RTCP timeout
 *          - 331 - QMI_FAILURE_CAUSE_RINGING_RINGBACK_TIMEOUTCall - was terminated due to aringing or ringback timeout
 *          - 332 - QMI_FAILURE_CAUSE_REG_RESTORATION - Call was terminated due to aregistration restoration
 *          - 333 - QMI_FAILURE_CAUSE_CODEC_ERROR - Call was terminated due to acodec error
 *          - 334 - QMI_FAILURE_CAUSE_UNSUPPORTED_SDP - Call terminated due to an SDPparsing failure
 *          - 335 - QMI_FAILURE_CAUSE_RTP_FAILURE - Call was terminated due to anRTP configuration failure
 *          - 336 - QMI_FAILURE_CAUSE_QoS_FAILURE - Call was terminated due to aQoS failure
 *          - 337 - QMI_FAILURE_CAUSE_MULTIPLE_CHOICES - Request was resolved in severalchoices, each with its ownspecific location
 *          - 338 - QMI_FAILURE_CAUSE_MOVED_PERMANENTLY - User is no longer at therequested address and client is toretry at new address specified
 *          - 339 - QMI_FAILURE_CAUSE_MOVED_TEMPORARILY - Requesting client is to retry therequest at the new addressspecified; expires header fieldgives the expiration time
 *          - 340 - QMI_FAILURE_CAUSE_USE_PROXY - Requested resource must beaccessed through a proxyspecified by the contact field
 *          - 341 - QMI_FAILURE_CAUSE_ALTERNATE_SERVICE - Call was not successful, butalternate services are possible
 *          - 342 - QMI_FAILURE_CAUSE_ALTERNATE_EMERGENCY_CALLCall - must be reoriginated as anemergency call
 *          - 343 - QMI_FAILURE_CAUSE_UNAUTHORIZED - Request requires userauthentication
 *          - 344 - QMI_FAILURE_CAUSE_PAYMENT_REQUIRED - Payment is required
 *          - 345 - QMI_FAILURE_CAUSE_METHOD_NOT_ALLOWED - Method requested in the addressline was not allowed for theaddress identified by therequest-URI
 *          - 346 - QMI_FAILURE_CAUSE_NOT_ACCEPTABLE - Resource identified by therequest can only generate aresponse with content that is notacceptable
 *          - 347 - QMI_FAILURE_CAUSE_PROXY_AUTHENTICATION_REQUIREDClient - must first authenticatewith a proxy
 *          - 348 - QMI_FAILURE_CAUSE_GONE - Requested resource is no longeravailable at the server and thereis no forwarding address
 *          - 349 - QMI_FAILURE_CAUSE_REQUEST_ENTITY_TOO_LARGERequest - entity body is largerthan what the server is willing toprocess
 *          - 350 - QMI_FAILURE_CAUSE_REQUEST_URI_TOO_LARGEServer - is refusing to servicebecause the request-URI islonger than the server willing tointerpret
 *          - 351 - QMI_FAILURE_CAUSE_UNSUPPORTED_URI_SCHEMEUnsupported - URI scheme
 *          - 352 - QMI_FAILURE_CAUSE_BAD_EXTENSION - Server did not understand theprotocol extension specified inthe proxy-required or requireheader field
 *          - 353 - QMI_FAILURE_CAUSE_EXTENSION_REQUIRED - Extension to process a request isnot listed in the supportedheader field in the request
 *          - 354 - QMI_FAILURE_CAUSE_INTERVAL_TOO_BRIEF - Expiration time of the resourcerefreshed by the request is tooshort
 *          - 355 - QMI_FAILURE_CAUSE_CALL_OR_TRANS_DOES_NOT_EXISTRequest - received by a UAS doesnot match any existing dialog ortransaction
 *          - 356 - QMI_FAILURE_CAUSE_LOOP_DETECTED - Server detected a loop
 *          - 357 - QMI_FAILURE_CAUSE_TOO_MANY_HOPS - Request received hasMax-Forwards header field at 0
 *          - 358 - QMI_FAILURE_CAUSE_ADDRESS_INCOMPLETE - Request had an incomplete URI
 *          - 359 - QMI_FAILURE_CAUSE_AMBIGUOUS - Requested URI was ambiguous
 *          - 360 - QMI_FAILURE_CAUSE_REQUEST_TERMINATED - Request was terminated by aBYE/Cancel
 *          - 361 - QMI_FAILURE_CAUSE_NOT_ACCEPTABLE_HERE - Resource requested by therequest-URI is not acceptable
 *          - 362 - QMI_FAILURE_CAUSE_REQUEST_PENDING - Request was received by a UASthat had a pending requestwithin the same dialog
 *          - 363 - QMI_FAILURE_CAUSE_UNDECIPHERABLE - Request has an encrypted MIMEbody for which the recipientdoes not possess an appropriatedecryption key
 *          - 364 - QMI_FAILURE_CAUSE_SERVER_INTERNAL_ERRORServer - internal error
 *          - 365 - QMI_FAILURE_CAUSE_NOT_IMPLEMENTED - Server does not support thefunctionality to fulfill the request
 *          - 366 - QMI_FAILURE_CAUSE_BAD_GATEWAY - Server received an invalidresponse from the downstreamgateway
 *          - 367 - QMI_FAILURE_CAUSE_SERVER_TIME_OUT - Server did not receive a timelyresponse from the externalserver it accessed
 *          - 368 - QMI_FAILURE_CAUSE_VERSION_NOT_SUPPORTEDServer - does not support the SIPprotocol version used in therequest
 *          - 369 - QMI_FAILURE_CAUSE_MESSAGE_TOO_LARGE - Server was unable to process therequest because the messagelength exceeded its capabilities
 *          - 370 - QMI_FAILURE_CAUSE_DOES_NOT_EXIST_ANYWHERE - Server has information that thepeer (pointed to by therequest-URI) does not existanywhere.
 *          - 371 - QMI_FAILURE_CAUSE_SESS_DESCR_NOT_ACCEPTABLE - User's agent was contacted butsome aspects of the sessiondescription were not acceptable
 *          - 372 - QMI_FAILURE_CAUSE_SRVCC_END_CALL - Call has ended due to an SRVCChandover from LTE toWCDMA; used for held calls oralerting calls
 *          - 373 - QMI_FAILURE_CAUSE_INTERNAL_ERROR - QMI internal error
 *          - 374 - QMI_FAILURE_CAUSE_SERVER_UNAVAILABLE - Request failed because theserver was unavailable
 *          - 375 - QMI_FAILURE_CAUSE_PRECONDITION_FAILURE - Request failed due to a precondition failure
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param AlphaIdentifier
 *          - see voice_alphaIDInfo definition
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param pUSSDInfo
 *          - USS Data from Network (See structure voice_USSInfo)
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @param pCcResultType
 *          - CC result code
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * @param pCallId
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *          - NULL pointer - Invalid data.
 *
 * @param pCCSuppsType
 *          - See structure 'voice_ccSUPSType' definition
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t                         *pfailureCause;
    voice_alphaIDInfo                *pAlphaIDInfo;
    struct voice_USSInfo             *pUSSDInfo;
    uint8_t                          *pCcResultType;
    uint8_t                          *pCallId;
    voice_ccSUPSType                 *pCCSuppsType;
    swi_uint256_t                    ParamPresenceMask;
} unpack_voice_SLQSOriginateUSSD_t;

/**
 * Initiates a USSD session unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSOriginateUSSD(
      uint8_t   *pResp,
      uint16_t  respLen,
      unpack_voice_SLQSOriginateUSSD_t *pOutput
     );

/**
 *  Structure for storing the USS info present in USSDNotification callback.
 *
 * @param tlvPresent
 *        Values:
 *        - 0 - Not present
 *        - 1 - Tlv is present 
 *
 * @param  NetworkInfo
 *        - USS Data from Network (See structure voice_USSInfo)
 *        - NULL pointer - Invalid data.
 *
 */
typedef struct
{
    uint8_t            tlvPresent;
    struct voice_USSInfo    networkInfo;
} voice_USSDNotificationNetworkInfo;
    

/**
 *  Structure for storing the USSD notification indication parameters.
 *
 * @param notification_Type(mandatory)
 *         Values:
 *          -0x01-FURTHER_USER_ACTION_NOT_REQUIRED
 *          -0x02-FURTHER_USER_ACTION_REQUIRED
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param  USSDNotificationNetworkInfo(optional)
 *         - USSD network info (See @ref voice_USSDNotificationNetworkInfo)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t                             notification_Type;
    voice_USSDNotificationNetworkInfo   USSDNotificationNetworkInfo;
    swi_uint256_t                       ParamPresenceMask;
} unpack_voice_USSDNotificationCallback_ind_t;

/**
 * Unpack USSD notification callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     USSD notification unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_USSDNotificationCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_USSDNotificationCallback_ind_t *pOutput
        );

/**
 *  This structure contains information about the Supplementary Services.
 *
 *  @param  svcType
 *          - Service type.
 *              - 0x01 - SERVICE_TYPE_ACTIVATE - Activate
 *              - 0x02 - SERVICE_TYPE_DEACTIVATE - Deactivate
 *              - 0x03 - SERVICE_TYPE_REGISTER - Register
 *              - 0x04 - SERVICE_TYPE_ERASE - Erase
 *              - 0x05 - SERVICE_TYPE_INTERROGATE - Interrogate
 *              - 0x06 - SERVICE_TYPE_REGISTER_PASSWORD - Register password
 *              - 0x07 - SERVICE_TYPE_USSD - USSD
 *
 *  @param  isModByCC
 *          - Indicates whether the supplementary service data is modified by
 *            the card (SIM/USIM) as part of the call control:
 *              - 0 - False
 *              - 1 - True
 *
 */
typedef struct
{
    uint8_t svcType;
    uint8_t isModByCC;
} voice_SUPSInfo;

/**
 *  This structure contains New Password Data.
 *
 *  @param  newPwd[BARRING_PASSWORD_LENGTH]
 *          - New password.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *
 *  @param  newPwdAgain[BARRING_PASSWORD_LENGTH]
 *          - New password again.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *
 */
typedef struct
{
    uint8_t newPwd[BARRING_PASSWORD_LENGTH];
    uint8_t newPwdAgain[BARRING_PASSWORD_LENGTH];
} voice_newPwdData;

/**
 *  This structure contains the parameters passed for SUPS info indication
 *  by the device.
 *
 *  @param  SUPSInformation(mandatory)
 *          - See @ref voice_SUPSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSvcClass(optional)
 *          - Service class is a combination (sum) of information class
 *            constants (optional)
 *          - See qaGobiApiTableSupServiceInfoClasses.h for service classes.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pReason(optional)
 *          - See qaGobiApiTableCallControlReturnReasons.h for return reasons.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCallFWNum(optional)
 *          - Call forwarding number to be registered with the network.
 *          - ASCII String, NULL terminated.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCallFWTimerVal(optional)
 *          - Call Forwarding No Reply Timer.
 *              - Range: 5 to 30 in steps of 5.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pUSSInfo(optional)
 *          - See @ref voice_USSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCallID(optional)
 *          - Call identifier of the voice call that has been modified to a
 *            supplementary service as a result of call control.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pAlphaIDInfo(optional)
 *          - See @ref voice_alphaIDInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pCallBarPasswd(optional)
 *          - Password is required if call barring is provisioned using a
 *            password.
 *              - Password consists of 4 ASCII digits.
 *              - Range: 0000 to 9999.
 *          - This also serves as the old password in the register password
 *            scenario.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pNewPwdData(optional)
 *          - See @ref voice_newPwdData for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pDataSrc(optional)
 *          - Sups Data Source.
 *          - Used to distinguish between the supplementary service data sent
 *            to the network and the response received from the network.
 *          - If absent, the supplementary service data in this indication can
 *            be assumed as a request sent to the network.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pFailCause(optional)
 *          - Supplementary services failure cause.
 *          - See @ref qaGobiApiTableVoiceCallEndReasons.h for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pCallFwdInfo(optional)
 *          - See @ref voice_getCallFWInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pCLIRstatus(optional)
 *          - See @ref voice_CLIRResp for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pCLIPstatus(optional)
 *          - See @ref voice_CLIPResp for more information.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pCOLPstatus(optional)
 *          - See @ref voice_COLPResp for more information.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pCOLRstatus(optional)
 *          - See @ref voice_COLRResp for more information.
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pCNAPstatus(optional)
 *          - See @ref voice_CNAPResp for more information.
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  @note    None
 *
 */
typedef struct
{
    voice_SUPSInfo          SUPSInformation;
    uint8_t                   *pSvcClass;
    uint8_t                   *pReason;
    uint8_t                   *pCallFWNum;
    uint8_t                   *pCallFWTimerVal;
    struct voice_USSInfo      *pUSSInfo;
    uint8_t                   *pCallID;
    voice_alphaIDInfo         *pAlphaIDInfo;
    uint8_t                   *pCallBarPasswd;
    voice_newPwdData          *pNewPwdData;
    uint8_t                   *pDataSrc;
    uint16_t                  *pFailCause;
    voice_getCallFWInfo       *pCallFwdInfo;
    voice_CLIRResp            *pCLIRstatus;
    voice_CLIPResp       *pCLIPstatus;
    voice_COLPResp       *pCOLPstatus;
    voice_COLRResp       *pCOLRstatus;
    voice_CNAPResp       *pCNAPstatus;
    swi_uint256_t        ParamPresenceMask;
} unpack_voice_SLQSVoiceSUPSCallback_ind_t;

/**
 * Unpack voice sups callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     voice susp callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SLQSVoiceSUPSCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SLQSVoiceSUPSCallback_ind_t *pOutput
        );

 /**
 *  This structure contains information about the indication parameters
 *  for all the calls originating or terminating from a particular device.
 *
 *  @param  arrCallInfo(mandatory)
 *          - See @ref voice_arrCallInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pArrRemotePartyNum(optional)
 *          - See @ref voice_arrRemotePartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pArrRemotePartyName(optional)
 *          - See @ref voice_arrRemotePartyName for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pArrAlertingType(optional)
 *          - See @ref voice_arrAlertingType for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pArrSvcOption(optional)
 *          - See @ref voice_arrSvcOption for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pArrCallEndReason(optional)
 *          - See @ref voice_arrCallEndReason for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pArrAlphaID(optional)
 *          - See @ref voice_arrAlphaID for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pArrConnectPartyNum(optional)
 *          - See @ref voice_arrConnectPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pArrDiagInfo(optional)
 *          - See @ref voice_arrDiagInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pArrCalledPartyNum(optional)
 *          - See @ref voice_arrCalledPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pArrRedirPartyNum(optional)
 *          - See @ref voice_arrRedirPartyNum for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pArrAlertingPattern(optional)
 *          - See @ref voice_arrAlertingPattern for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    voice_arrCallInfo        arrCallInfomation;
    voice_arrRemotePartyNum  *pArrRemotePartyNum;
    voice_arrRemotePartyName *pArrRemotePartyName;
    voice_arrAlertingType    *pArrAlertingType;
    voice_arrSvcOption       *pArrSvcOption;
    voice_arrCallEndReason   *pArrCallEndReason;
    voice_arrAlphaID         *pArrAlphaID;
    voice_arrConnectPartyNum *pArrConnectPartyNum;
    voice_arrDiagInfo        *pArrDiagInfo;
    voice_arrCalledPartyNum  *pArrCalledPartyNum;
    voice_arrRedirPartyNum   *pArrRedirPartyNum;
    voice_arrAlertingPattern *pArrAlertingPattern;
    swi_uint256_t            ParamPresenceMask;
} unpack_voice_allCallStatusCallback_ind_t;

/**
 * Unpack all call status callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     all call status callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_allCallStatusCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_allCallStatusCallback_ind_t *pOutput
        );

/**
 *  Contains the parameters passed for voice privacy change indication
 *
 *  @param  callID
 *          - Unique identifier of the call for which the voice privacy is
 *            applicable. (mandatory)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  voicePrivacy
 *          - Voice Privacy (mandatory)
 *              - 0x00 - VOICE_PRIVACY_STANDARD - Standard privacy
 *              - 0x01 - VOICE_PRIVACY_ENHANCED - Enhanced privacy
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint8_t callID;
    uint8_t voicePrivacy;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_voicePrivacyChangeCallback_ind_t;

/**
 * Unpack voice privacy change callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     voice privacy change callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_voicePrivacyChangeCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_voicePrivacyChangeCallback_ind_t *pOutput
        );

/**
 *  This structure contains information about the DTMF (Dual Tone
 *  Multi-Frequency).
 *
 *  @param  callID
 *          - Call identifier for the current call.
 *
 *  @param  DTMFEvent
 *          - DTMF event
 *              - 0x00 - DTMF_EVENT_REV_BURST - Sends a CDMA-burst DTMF
 *              - 0x01 - DTMF_EVENT_REV_START_CONT -
 *                 Starts a continuous DTMF tone
 *              - 0x03 - DTMF_EVENT_REV_STOP_CONT -
 *                 Stops a continuous DTMF tone
 *              - 0x05 - DTMF_EVENT_FWD_BURST -
 *                 Received a CDMA-burst DTMF message
 *              - 0x06 - DTMF_EVENT_FWD_START_CONT -
 *                 Received a start-continuous DTMF tone order
 *              - 0x07 - DTMF_EVENT_FWD_STOP_CONT -
 *                 Received a stop-continuous DTMF tone order
 *
 *  @param  digitCnt
 *          - Number of set of following element i.e. digitBuff.
 *
 *  @param  digitBuff[MAX_VOICE_DESCRIPTION_LENGTH]
 *          - DTMF digit buffer in ASCII string which is NULL terminated
 *
 */
typedef struct
{
    uint8_t          callID;
    uint8_t          DTMFEvent;
    uint8_t          digitCnt;
    uint8_t          digitBuff[MAX_VOICE_DESCRIPTION_LENGTH];
} voice_DTMFInfo;

/**
 *  This structure contains the parameters passed for
 *  DTMF event indication by the device.
 *
 *  @param  DTMFInformation(mandatory)
 *          - See @ref voice_DTMFInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pOnLength(optional)
 *          - DTMF Pulse Width
 *              - 0x00 - DTMF_ONLENGTH_95MS - 95 ms
 *              - 0x01 - DTMF_ONLENGTH_150MS - 150 ms
 *              - 0x02 - DTMF_ONLENGTH_200MS - 200 ms
 *              - 0x03 - DTMF_ONLENGTH_250MS - 250 ms
 *              - 0x04 - DTMF_ONLENGTH_300MS - 300 ms
 *              - 0x05 - DTMF_ONLENGTH_350MS - 350 ms
 *              - 0x06 - DTMF_ONLENGTH_SMS - SMS Tx special pulse width
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pOffLength(optional)
 *          - DTMF Interdigit Interval
 *              - 0x00 - DTMF_OFFLENGTH_60MS - 60 ms
 *              - 0x01 - DTMF_OFFLENGTH_100MS - 100 ms
 *              - 0x02 - DTMF_OFFLENGTH_150MS - 150 ms
 *              - 0x03 - DTMF_OFFLENGTH_200MS - 200 ms
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 * @note    None
 *
 */
typedef struct
{
    voice_DTMFInfo DTMFInformation;
    uint8_t     *pOnLength;
    uint8_t     *pOffLength;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_DTMFEventCallback_ind_t;

/**
 * Unpack DTMF event callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     DTMF event callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_DTMFEventCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_DTMFEventCallback_ind_t *pOutput
        );

/**
 *  Contains the parameters passed for Explicit Communication Transfer by
 *  the device.
 *
 *  @param  ECTCallState
 *          - ECT call state:
 *              - 0x00 - ECT_CALL_STATE_NONE - None
 *              - 0x01 - ECT_CALL_STATE_ALERTING - Alerting
 *              - 0x02 - ECT_CALL_STATE_ACTIVE - Active
 *
 *  @param  presentationInd
 *          - Presentation indicator
 *              - 0x00 - presentationAllowedAddress
 *              - 0x01 - presentationRestricted
 *              - 0x02 - numberNotAvailable
 *              - 0x04 - presentationRestrictedAddress
 *
 *  @param  number
 *          - Number in ASCII characters terminated by NULL
 *
 */
typedef struct
{
    uint8_t ECTCallState;
    uint8_t presentationInd;
    uint8_t number[MAX_VOICE_CALL_NO_LEN];
} voice_ECTNum;

/**
 *  Contains the parameters passed for SUPS notification indication by
 *  the device.
 *
 *  @param  callID
 *          - Unique identifier of the call for which the notification is
 *            applicable. (mandatory)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  notifType
 *          - Notification type parameter (mandatory)
 *              - 0x01 - NOTIFICATION_TYPE_OUTGOING_CALL_IS_FORWARDED\n
 *                  Originated MO call is being forwarded to another user
 *              - 0x02 - NOTIFICATION_TYPE_OUTGOING_CALL_IS_WAITING\n
 *                  Originated MO call is waiting at the called user
 *              - 0x03 - NOTIFICATION_TYPE_OUTGOING_CUG_CALL\n
 *                  Outgoing call is a CUG call
 *              - 0x04 - NOTIFICATION_TYPE_OUTGOING_CALLS_BARRED\n
 *                  Outgoing calls are barred
 *              - 0x05 - NOTIFICATION_TYPE_OUTGOING_CALL_IS_DEFLECTED\n
 *                  Outgoing call is deflected
 *              - 0x06 - NOTIFICATION_TYPE_INCOMING_CUG_CALL\n
 *                  Incoming call is a CUG call
 *              - 0x07 - NOTIFICATION_TYPE_INCOMING_CALLS_BARRED\n
 *                  Incoming calls are barred
 *              - 0x08 - NOTIFICATION_TYPE_INCOMING_FORWARDED_CALL\n
 *                  Incoming call received is a forwarded call
 *              - 0x09 - NOTIFICATION_TYPE_INCOMING_DEFLECTED_CALL\n
 *                  Incoming call is a deflected call
 *              - 0x0A - NOTIFICATION_TYPE_INCOMING_CALL_IS_FORWARDED\n
 *                  Incoming call is forwarded to another user
 *              - 0x0B - NOTIFICATION_TYPE_UNCOND_CALL_FORWARD_ACTIVE\n
 *                  Unconditional call forwarding is active
 *              - 0x0C - NOTIFICATION_TYPE_COND_CALL_FORWARD_ACTIVE\n
 *                  Conditional call forwarding is active
 *              - 0x0D - NOTIFICATION_TYPE_CLIR_SUPPRESSION_REJECTED\n
 *                  CLIR suppression is rejected
 *              - 0x0E - NOTIFICATION_TYPE_CALL_IS_ON_HOLD\n
 *                  Call is put on hold at the remote party
 *              - 0x0F - NOTIFICATION_TYPE_CALL_IS_RETRIEVED\n
 *                  Call is retrieved at the remote party from the hold state
 *              - 0x10 - NOTIFICATION_TYPE_CALL_IS_IN_MPTY\n
 *                  Call is in a conference
 *              - 0x11 - NOTIFICATION_TYPE_INCOMING_CALL_IS_ECT\n
 *                  Incoming call is an explicit call transfer
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pCUGIndex
 *          - The CUG Index used to indicate that the incoming/outgoing
 *            call is a CUG call. (optional, NULL when not present)\n
 *            Range: 0x00 to 0x7FFF.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pECTNum
 *          - The ECT Number is used to indicate that the incoming call is an
 *            explicitly transferred call. (optional, NULL when not present)\n
 *            Refer ECTNum for details.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 * @note    None
 *
 */
typedef struct
{
    uint8_t      callID;
    uint8_t      notifType;
    uint16_t     *pCUGIndex;
    voice_ECTNum *pECTNum;
    swi_uint256_t  ParamPresenceMask;
} unpack_voice_SUPSNotificationCallback_ind_t;

/**
 * Unpack SUPS notification callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     SUPS notification callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_SUPSNotificationCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_SUPSNotificationCallback_ind_t *pOutput
        );

/**
 * This structure consist of OTASP or OTAPA event params
 *
 *  @param  callID
 *          - Call identifier for the call.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  OTASPStatus
 *          - OTASP status for the OTASP call.
 *            Values:
 *            - 0x00 - OTASP_STATUS_SPL_UNLOCKED.SPL unlocked; only for
 *                     user-initiated OTASP
 *            - 0x01 - OTASP_STATUS_SPRC_RETRIES_EXCEEDED. SPC retries exceeded;
 *                     only for user-initiated OTASP
 *            - 0x02 - OTASP_STATUS_AKEY_EXCHANGED.A-key exchanged;
 *                     only for user-initiated OTASP
 *            - 0x03 - OTASP_STATUS_SSD_UPDATED. SSD updated; for both\n
 *                     user-initiated OTASP and network-initiated OTASP (OTAPA)
 *            - 0x04 - OTASP_STATUS_NAM_DOWNLOADED - NAM downloaded;
 *                     only for user-initiated OTASP
 *            - 0x05 - OTASP_STATUS_MDN_DOWNLOADED - MDN downloaded;
 *                     only for user-initiated OTASP
 *            - 0x06 - OTASP_STATUS_IMSI_DOWNLOADED - IMSI downloaded;
 *                     only for user-initiated OTASP
 *            - 0x07 - OTASP_STATUS_PRL_DOWNLOADED - PRL downloaded;
 *                     only for user-initiated OTASP
 *            - 0x08 - OTASP_STATUS_COMMITTED - Commit successful;
 *                     only for user-initiated OTASP
 *            - 0x09 - OTASP_STATUS_OTAPA_STARTED - OTAPA started;
 *                     only for network-initiated OTASP(OTAPA)
 *            - 0x0A - OTASP_STATUS_OTAPA_STOPPED - OTAPA stopped;
 *                     only for network-initiated OTASP(OTAPA)
 *            - 0x0B - OTASP_STATUS_OTAPA_ABORTED - OTAPA aborted;
 *                     only for network-initiated OTASP(OTAPA)
 *            - 0x0C - OTASP_STATUS_OTAPA_COMMITTED - OTAPA committed;
 *                     only for network-initiated OTASP(OTAPA)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t callID;
    uint8_t OTASPStatus;
    swi_uint256_t  ParamPresenceMask;
}unpack_voice_OTASPStatusCallback_ind_t;

/**
 * Unpack voice OTASP status callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     voice OTASP status callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_OTASPStatusCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_OTASPStatusCallback_ind_t *pOutput
        );

/**
 *  This structure contains Signal Information
 *
 *  @param  signalType
 *          - Call identifier for the call.
 *
 *  @param  alertPitch
 *          - Signal Information
 *
 *  @param  signal
 *          - Caller ID Information
 *
 */
typedef struct
{
    uint8_t signalType;
    uint8_t alertPitch;
    uint8_t signal;
} voice_signalInfo;

/**
 *  This structure contains Caller ID Information
 *
 *  @param  PI
 *          - Presentation indicator; refer to [S1, Table 2.7.4.4-1]
 *            for valid values.
 *
 *  @param  callerIDLen
 *          - Number of sets of following elements
 *            - Caller Id
 *
 *  @param  pCallerID
 *          - Caller ID in ASCII string.
 *
 */
typedef struct
{
    uint8_t PI;
    uint8_t callerIDLen;
    uint8_t callerID[255];
} voice_callerIDInfo;

/**
 *  This structure contains Calling party Number Information
 *
 *  @param  PI
 *          - Presentation indicator; refer to [S1, Table 2.7.4.4-1]
 *            for valid values.
 *
 *  @param  SI
 *          - Number of sets of following elements
 *            - Caller Id
 *
 *  @param  SI
 *          - Number screening indicator.
 *          - Values:
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                  Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                  Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                  Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED - Provided network
 *
 *  @param  numType
 *          - Number type.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numPlan
 *          - Number plan.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[255]
 *          - number of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t PI;
    uint8_t SI;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t numLen;
    uint8_t number[255];
} voice_callingPartyInfo;

/**
 *  This structure contains Called party Number Information
 *
 *  @param  PI
 *          - Presentation indicator; refer to [S1, Table 2.7.4.4-1]
 *            for valid values.
 *
 *  @param  SI
 *          - Number of sets of following elements
 *            - Caller Id
 *
 *  @param  SI
 *          - Number screening indicator.
 *          - Values:
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                  Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                  Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                  Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED - Provided network
 *
 *  @param  numType
 *          - Number type.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numPlan
 *          - Number plan.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[255]
 *          - number of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t PI;
    uint8_t SI;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t numLen;
    uint8_t number[255];
} voice_calledPartyInfo;

/**
 *  This structure contains Redirecting Number Information
 *
 *  @param  PI
 *          - Presentation indicator; refer to [S1, Table 2.7.4.4-1]
 *            for valid values.
 *
 *  @param  SI
 *          - Number of sets of following elements
 *            - Caller Id
 *
 *  @param  SI
 *          - Number screening indicator.
 *          - Values:
 *              - 0x00 - QMI_VOICE_SI_USER_PROVIDED_NOT_SCREENED -
 *                  Provided user is not screened
 *              - 0x01 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_PASSED -
 *                  Provided user passed verification
 *              - 0x02 - QMI_VOICE_SI_USER_PROVIDED_VERIFIED_FAILED -
 *                  Provided user failed verification
 *              - 0x03 - QMI_VOICE_SI_NETWORK_PROVIDED - Provided network
 *
 *  @param  numType
 *          - Number type.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_TYPE_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_TYPE_INTERNATIONAL - International
 *              - 0x02 - QMI_VOICE_NUM_TYPE_NATIONAL - National
 *              - 0x03 - QMI_VOICE_NUM_TYPE_NETWORK_SPECIFIC - Network-specific
 *              - 0x04 - QMI_VOICE_NUM_TYPE_SUBSCRIBER - Subscriber
 *              - 0x05 - QMI_VOICE_NUM_TYPE_RESERVED - Reserved
 *              - 0x06 - QMI_VOICE_NUM_TYPE_ABBREVIATED - Abbreviated
 *              - 0x07 - QMI_VOICE_NUM_TYPE_RESERVED_EXTENSION -
 *                  Reserved extension
 *
 *  @param  numPlan
 *          - Number plan.
 *          - Values:
 *              - 0x00 - QMI_VOICE_NUM_PLAN_UNKNOWN - Unknown
 *              - 0x01 - QMI_VOICE_NUM_PLAN_ISDN - ISDN
 *              - 0x03 - QMI_VOICE_NUM_PLAN_DATA - Data
 *              - 0x04 - QMI_VOICE_NUM_PLAN_TELEX - Telex
 *              - 0x08 - QMI_VOICE_NUM_PLAN_NATIONAL - National
 *              - 0x09 - QMI_VOICE_NUM_PLAN_PRIVATE - Private
 *              - 0x0B - QMI_VOICE_NUM_PLAN_RESERVED_CTS -
 *                  Reserved cordless telephony system
 *              - 0x0F - QMI_VOICE_NUM_PLAN_RESERVED_EXTENSION -
 *                  Reserved extension
 *  @param  reason
 *          -Redirecting reason; refer to [S1, Table 3.7.5.11-1] for\n
 *           valid values
 *
 *  @param  numLen
 *          - Provides the length of number which follow.
 *
 *  @param  number[255]
 *          - number of numLen length, NULL terminated.
 *
 */
typedef struct
{
    uint8_t PI;
    uint8_t SI;
    uint8_t numType;
    uint8_t numPlan;
    uint8_t reason;
    uint8_t numLen;
    uint8_t number[255];
} voice_redirNumInfo;

/**
 *  This structure contains National Supplementary Services - Audio Control
 *  Information
 *
 *  @param  upLink
 *          - Values as per[ S24, 4.10 Reservation response].
 *
 *  @param  downLink
 *          - Values as per[ S24, 4.10 Reservation response].
 */
typedef struct
{
    uint8_t upLink;
    uint8_t downLink;
} voice_NSSAudioCtrl;

/**
 *  This structure contains Line Control Information
 *
 *  @param  polarityIncluded
 *          - Included Polarity; Boolean Value
 *
 *  @param  toggleMode
 *          - Toggle mode; Boolean Value
 *
 *  @param  revPolarity
 *          - Reverse Polarity; Boolean Value
 *
 *  @param  pwrDenialTime
 *          - Power denial time; refer to [S1, Section 3.7.5.15 Line Control]
 *            for valid values
 */
typedef struct
{
    uint8_t polarityIncluded;
    uint8_t toggleMode;
    uint8_t revPolarity;
    uint8_t pwrDenialTime;
} voice_lineCtrlInfo;

/**
 *  This structure contains Line Control Information
 *
 *  @param  dispType
 *          - Values are per [S1, Table 3.7.5.16-1].
 *
 *  @param  extDispInfoLen
 *          - Number of sets of the following elements:
 *            - ext_display_info
 *
 *  @param  extDispInfo
 *          - Extended display information buffer containing the display
 *            record; refer to [S1, Section 3.7.5.16] for the format
 *            information of the buffer contents.
 */
typedef struct
{
    uint8_t dispType;
    uint8_t extDispInfoLen;
    uint8_t extDispInfo[255];
} voice_extDispRecInfo;

/**
 *  This structure contains Voice record Information
 *
 *  @param  callID [Mandatory]
 *          - Call identifier for the call.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSignalInfo[Optional]
 *          - Signal Information
 *          - See @ref voice_signalInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pCallerIDInfo[Optional]
 *          - Caller ID Information
 *          - See @ref voice_callerIDInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pDispInfo[Optional]
 *          - Display Information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pExtDispInfo[Optional]
 *          - Extended Display Information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *
 *  @param  pCallerNameInfo[Optional]
 *          - Caller Name Information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCallWaitInd[Optional]
 *          - Call Waiting Indicator
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pConnectNumInfo[Optional]
 *          - Connected Number Information
 *          - see @ref voice_connectNumInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pCallingPartyInfo[Optional]
 *          - Calling Party Number Information
 *          - This structure is having exactly same elements as connectNumInfo
 *          - see @ref voice_connectNumInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pCalledPartyInfo[Optional]
 *          - Called Party Number Information
 *          - see @ref voice_calledPartyInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param   pRedirNumInfo[Optional]
 *          - Redirecting Number Information
 *          - see @ref voice_redirNumInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pCLIRCause[Optional]
 *          - National Supplementary Services - CLIR
 *          - see @ref voice_NSSAudioCtrl for more information
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pNSSAudioCtrl[Optional]
 *          - National Supplementary Services - Audio Control
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pNSSRelease[Optional]
 *          - National Supplementary Services - Release
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pLineCtrlInfo[Optional]
 *          - Line Control Information
 *          - see @ref voice_lineCtrlInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pExtDispRecInfo[Optional]
 *          - Extended Display Record Information
 *          - see @ref voice_extDispRecInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t               callID;
    voice_signalInfo      *pSignalInfo;
    voice_callerIDInfo    *pCallerIDInfo;
    uint8_t               *pDispInfo;
    uint8_t               *pExtDispInfo;
    uint8_t               *pCallerNameInfo ;
    uint8_t               *pCallWaitInd;
    voice_connectNumInfo  *pConnectNumInfo;
    voice_connectNumInfo  *pCallingPartyInfo;
    voice_calledPartyInfo *pCalledPartyInfo;
    voice_redirNumInfo    *pRedirNumInfo;
    uint8_t               *pCLIRCause;
    voice_NSSAudioCtrl    *pNSSAudioCtrl;
    uint8_t               *pNSSRelease;
    voice_lineCtrlInfo    *pLineCtrlInfo;
    voice_extDispRecInfo  *pExtDispRecInfo;
    swi_uint256_t         ParamPresenceMask;
} unpack_voice_VoiceInfoRecCallback_ind_t;

/**
 * Unpack SUPS notification callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     SUPS notification callback unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_voice_VoiceInfoRecCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_voice_VoiceInfoRecCallback_ind_t *pOutput
        );





#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif//#ifndef  __LIBPACK_VOICE_H__

