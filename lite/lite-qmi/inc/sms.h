/**
 * \ingroup liteqmi
 *
 * \file sms.h
 */
#ifndef __LITEQMI_SMS_H__
#define __LITEQMI_SMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define MAX_SMS_MESSAGE_SIZE 2048
#define MAX_SMS_LIST_SIZE 255
#define MAX_MS_TRANSFER_ROUTE_MSG 256
#define MAX_MSE_TWS_MSG 1254
#define MAX_MSC_ADDRESS_SIZE 256
#define MAX_CDMA_ENC_MO_TXT_MSG_SIZE 255
#define SMSC_TYPE_LEN 0x03
#define SMS_CONFIG_LEN         0x05
#define SMS_MAX_SMS_ROUTES     0x0A
#define SMS_NUM_OF_SET         0xFF


/**
 * This sturcture contains pack get SMS parameters.
 * @param storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 * @param messageIndex
 *          - Message index
 *
 * @param pMessageMode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 */
typedef struct{
    uint32_t storageType;
    uint32_t messageIndex;
    uint8_t *pMessageMode;
} pack_sms_SLQSGetSMS_t;

/**
 * This sturcture contains unpack get SMS parameters.
 * @param messageTag
 *          - Message tag
 *              - 0 - Read
 *              - 1 - Not read
 *              - 2 - Mobile originated and sent
 *              - 3 - Mobile originated but not yet sent
 *           - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param messageFormat
 *          - Message format
 *              - 0 - CDMA (IS-637B)
 *              - 1 - 5 (Reserved)
 *              - 6 - GSM/WCDMA PP
 *           - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param messageSize
 *          - Upon input the maximum number of bytes that can be written to the
 *            message array.
 *          - Upon successful output the actual number of bytes written to the
 *            message array.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param message
 *          - The message contents array
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t  messageTag;
    uint32_t  messageFormat;
    uint32_t  messageSize;
    uint8_t   message[MAX_SMS_MESSAGE_SIZE];
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetSMS_t;

/**
 * get sms pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSGetSMS(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSGetSMS_t *reqParam
        );

/**
 * get sms unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSGetSMS(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetSMS_t *pOutput
        );

/**
 * This structure contains pack get SMS list parameters.
 * @param storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 * @param requestedTag
 *          - (Optional) Message tag
 *              - 0 - Read
 *              - 1 - Not read
 *              - 2 - Mobile originated and sent
 *              - 3 - Mobile originated but not yet sent
 *
 * @param messageMode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 */
typedef struct{
    uint32_t storageType;
    uint32_t *pRequestedTag;
    uint8_t  *pMessageMode;
} pack_sms_SLQSGetSMSList_t;

/**
 * This structure contains SMS message list.
 * @param messageIndex
 *          - Message index of each matched message
 * @param messageTag
 *          - Messagetag
 */
typedef struct 
{
    uint32_t messageIndex;
    uint32_t messageTag;
}qmiSmsMessageList;

/**
 * This structure contains unpack get SMS list parameters.
 * @param messageListSize
 *          - Upon input the maximum number of elements that the message list
 *            array can contain.
 *          - Upon successful output the actual number of elements in the
 *            message list array.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param messageList
 *          - Message List
 *          - See @ref qmiSmsMessageList for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t    messageListSize;
    qmiSmsMessageList messageList[MAX_SMS_LIST_SIZE];
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetSMSList_t;

/**
 * get sms list pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSGetSMSList(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSGetSMSList_t *reqParam
        );

/**
 * get sms list unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSGetSMSList(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetSMSList_t *pOutput
        );

/**
 * This structure contains pack modify SMS status parameters.
 * @param storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 * @param messageIndex
 *          - Message index
 *
 * @param messageTag
 *          - Message tag
 *              - 0 - Read
 *              - 1 - Not read
 *
 * @param pMessageMode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 */
typedef struct{
    uint32_t storageType;
    uint32_t messageIndex;
    uint32_t messageTag;
    uint8_t *pMessageMode;
} pack_sms_SLQSModifySMSStatus_t;

/**
 * modify sms status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSModifySMSStatus(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSModifySMSStatus_t *reqParam
        );

/**
 * This structure contains unpack modify SMS status parameters.
 * 
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSModifySMSStatus_t;

/**
 * modify sms status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSModifySMSStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSModifySMSStatus_t *pOutput
        );

/**
 * This structure contains pack delete SMS parameters.
 * @param storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 * @param pMessageIndex
 *          - (Optional) message index
 *
 * @param pMessageTag
 *          - (Optional) message tag
 *              - 0 - Read
 *              - 1 - Not read
 *              - 2 - Mobile originated and sent
 *              - 3 - Mobile originated but not yet sent
 *
 * @param pMessageMode
 *        - (Optional) message mode
 *        - this must be included if the device is capable of supporting more than one protocol
 *        - e.g. CDMA and GW
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 */
typedef struct{
    uint32_t storageType;
    uint32_t *pMessageIndex;
    uint32_t *pMessageTag;
    uint8_t  *pMessageMode;
} pack_sms_SLQSDeleteSMS_t;

/**
 * delete sms pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSDeleteSMS(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSDeleteSMS_t *reqParam
        );

/**
 * This structure contains unpack delete SMS parameters.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSDeleteSMS_t;

/**
 * delete sms unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSDeleteSMS(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSDeleteSMS_t *pOutput
        );

/**
 * This structure contains pack send SMS parameters.
 * @param messageFormat
 *          - Message format
 *              - 0 - CDMA (IS-637B)
 *              - 1 - 5 (Reserved)
 *              - 6 - GSM/WCDMA PP
 *
 * @param messageSize
 *          - The length of the message contents in bytes
 *
 * @param pLinktimer
 *          - GW SMS link open for the specified number of second
 *
 * @param pMessage
 *          - The message contents in PDU format contains SMS header and payload message
 *
 */
typedef struct{
    uint32_t messageFormat;
    uint32_t messageSize;
    uint8_t  *pMessage;
    uint8_t *pLinktimer;
} pack_sms_SendSMS_t;

/**
 * This structure contains unpack send SMS parameters.
 * @param messageID
 *          - WMS message ID
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param messageFailureCode
 *          - pointer to message failure code.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t    messageID;
    uint32_t    messageFailureCode;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SendSMS_t;

/**
 * send sms list pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SendSMS(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SendSMS_t *reqParam
        );

/**
 * send sms  unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SendSMS(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SendSMS_t *pOutput
        );

enum eqmiCbkSetStatus{
    LITEQMI_QMI_CBK_PARAM_RESET = 0,
    LITEQMI_QMI_CBK_PARAM_SET   = 1,
    LITEQMI_QMI_CBK_PARAM_NOCHANGE
};

/**
 * This strucure contains pack set new SMS callback parameters.
 * @param status callback parameter
 */
typedef struct{
    enum eqmiCbkSetStatus status;
} pack_sms_SetNewSMSCallback_t;

/**
 * set new sms callback pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SetNewSMSCallback(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SetNewSMSCallback_t reqParam
        );

/**
 * This structure contains unpack set new SMS callback.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SetNewSMSCallback_t;

/**
 * set new sms callback unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] Output unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SetNewSMSCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SetNewSMSCallback_t *Output
        );
/**
 * This structure contains MT message information.
 * @param storageType memory storage 0x00-UIM  0x01-NV
 * @param messageIndex MT Message index
 */
typedef struct
{
    uint32_t storageType;
    uint32_t messageIndex;
} sMSMTMessageInfo;

/**
 * This structure contains MT message TLV information.
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param MTMessageInfo
 *          - MT Message
 *          - See @ref sMSMTMessageInfo for more information
 *
 */
typedef struct
{
    uint8_t    TlvPresent;
    sMSMTMessageInfo MTMessageInfo;
}newMTMessageTlv;
/**
 * This structure contains Transfer route MT message information.
 * @param ackIndicator
 *          - Parameter to indicate if ACK must be sent by the control point
 *              0x00 - Send ACK
 *              0x01 - Do not send ACK
 *
 * @param transactionID
 *          - Transaction ID of the message
 *
 * @param format
 *          - Message format
 *              0x00 - CDMA
 *              0x02 - 0x05 - Reserved
 *              0x06 - GW_PP
 *              0x07 - GW_BC
 *
 * @param length
 *          - Length of the raw message. This length should not exceed the
 *            maximum WMS payload length of 256 bytes
 *
 * @param data
 *          - Raw message data
 */
typedef struct
{
    uint8_t  ackIndicator;
    uint32_t transactionID;
    uint8_t  format;
    uint16_t  length;
    uint8_t  data[MAX_MS_TRANSFER_ROUTE_MSG];
} sMSTransferRouteMTMessageInfo;

/**
 * This structure contains Transfer route MT message TLV information.
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param TransferRouteMTMessageInfo
 *          - Transfer Route MT Message
 *          - See @ref sMSTransferRouteMTMessageInfo for more information
 */
typedef struct
{
    uint8_t TlvPresent;
    sMSTransferRouteMTMessageInfo TransferRouteMTMessageInfo;
}transferRouteMessageTlv;

/**
 * This structure contains message mode information.
 * @param messageMode
 *          - Message Mode
 *           - 0x00 - CDMA
 *           - 0x01 - GW
 */
typedef struct
{
    uint8_t  messageMode;
} sMSMessageModeInfo;

/**
 * This structure contains message mode TLV information.
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param MessageModeInfo
 *          - Message Mode
 *          - See @ref sMSMessageModeInfo for more information
 */
typedef struct
{
    uint8_t TlvPresent;
    sMSMessageModeInfo MessageModeInfo;
}messageModeTlv;

/**
 * This structure contains ETWS information
 * @param notificationType
 *          - Message mode
 *              0x00 - Primary
 *              0x01 - Secondary GSM
 *              0x02 - Secondary UMTS
 *
 * @param length
 *          - Number of sets of following elements
 *
 * @param data
 *          - Raw message data
 */

typedef struct
{
    uint8_t notificationType;
    uint16_t length;
    uint8_t data[MAX_MSE_TWS_MSG];
} sMSEtwsMessageInfo;

/**
 * This structure contains ETWS TLV information
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param EtwsMessageInfo
 *          - ETWS Message
 *          - See @ref sMSEtwsMessageInfo for more information
 */
typedef struct
{
    uint8_t    TlvPresent;
    sMSEtwsMessageInfo EtwsMessageInfo;
}sMSEtwsMessageTlv;

/**
 * This structure contains ETWS PLMN information
 * @param mobileCountryCode
 *          - 16 bit representation of MCC
 *              value range : 0 -999
 *
 * @param mobileNetworkCode
 *          - 16 bit representation of MNC
 *              value range : 0 -999
 *
 */
typedef struct
{
    uint16_t mobileCountryCode;
    uint16_t mobileNetworkCode;
} sMSEtwsPlmnInfo;

/**
 * This structure contains ETWS PLMN TLV information
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param ETWSPLMNInfo
 *          - ETWS PLMN Information
 *          - See @ref sMSEtwsPlmnInfo for more information
 */
typedef struct
{
    uint8_t         TlvPresent;
    sMSEtwsPlmnInfo ETWSPLMNInfo;
}eTWSPLMNInfoTlv;

/**
 * This structure contains SMSC information
 * @param length
 *          - Number of sets of following element
 *
 * @param data
 *          - SMSC address
 */

typedef struct
{
    uint8_t length;
    uint8_t data[MAX_MSC_ADDRESS_SIZE];
} sMSCAddressInfo;

/**
 * This structure contains SMSC TLV information
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param SMSCInfo
 *          - SMSC Address
 *          - See @ref sMSCAddressInfo for more information
 */
typedef struct
{
    uint8_t         TlvPresent;
    sMSCAddressInfo SMSCInfo;
}sMSCAddressTlv;

/**
 * This structure contains SMS on IMS information
 * @param smsOnIMS SMS on IMS
 */
typedef struct
{
    uint8_t smsOnIMS;
}sMSOnIMSInfo;

/**
 * This structure contains SMS on IMS TLV information.
 * @param TlvPresent
 *          - Boolean indicating the presence of the TLV
 *             in the QMI response
 *
 * @param IMSInfo
 *          - SMS on IMS
 *          - See @ref sMSOnIMSInfo for more information
 */
typedef struct
{
    uint8_t      TlvPresent;
    sMSOnIMSInfo IMSInfo;
}sMSOnIMSTlv;

/**
 * This structure contains unpack new SMS callback indication.
 * @param NewMMTlv
 *          - MT message
 *          - See @ref newMTMessageTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param TRMessageTlv
 *          - Transfer Route MT Message
 *          - See @ref transferRouteMessageTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param MMTlv
 *          - Message mode
 *          - See @ref messageModeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @param ETWSTlv
 *          - ETWS Message
 *          - See @ref sMSEtwsMessageTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * @param ETWSPLMNTlv
 *          - ETWS PLMN Information
 *          - See @ref eTWSPLMNInfoTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * @param SMSCTlv
 *          - SMSC Address
 *          - See @ref sMSCAddressTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 * @param IMSTlv
 *          - SMS on IMS
 *          - See @ref sMSOnIMSTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    newMTMessageTlv         NewMMTlv;
    transferRouteMessageTlv TRMessageTlv;
    messageModeTlv          MMTlv;
    sMSEtwsMessageTlv       ETWSTlv;
    eTWSPLMNInfoTlv         ETWSPLMNTlv;
    sMSCAddressTlv          SMSCTlv;
    sMSOnIMSTlv             IMSTlv;
    swi_uint256_t           ParamPresenceMask;
}unpack_sms_SetNewSMSCallback_ind_t;

/**
 * set new sms callback indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SetNewSMSCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SetNewSMSCallback_ind_t *pOutput
        );

/**
 * This structure contains unpack SMS memory full callback indication.
 * @param storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param messageMode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t storageType;
    uint8_t messageMode;
    swi_uint256_t  ParamPresenceMask;
}unpack_sms_SLQSWmsMemoryFullCallBack_ind_t;

/**
 * sms full callback indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSWmsMemoryFullCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSWmsMemoryFullCallBack_ind_t *pOutput
        );

/**
 *  Structure contains Gets the SMS center address parameters.
 *
 *  @param  addressSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the SMS center address array can contain.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSMSCAddress
 *          - The SMS center address represented as a NULL terminated string.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  typeSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the SMS center address type array can contain.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSMSCType
 *          - The SMS center address type represented as a NULL terminated
 *            string.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t addressSize;
    uint8_t *pSMSCAddress;
    uint8_t typeSize;
    uint8_t *pSMSCType;
    swi_uint256_t  ParamPresenceMask;
}unpack_sms_GetSMSCAddress_t;

/**
 * Gets the SMS center address pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_GetSMSCAddress(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Gets the SMS center address unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_GetSMSCAddress(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_GetSMSCAddress_t *pOutput
        );

/**
 *  Structure to Set the SMS center address.
 *
 *  @param  SMSCAddress
 *          - The SMS center address represented as a NULL terminated string
 *
 *  @param  SMSCType
 *          - The SMS center address type represented as a NULL terminated
 *          string (optional).
 *
 */
typedef struct{
    uint8_t  *pSMSCAddress;
    uint8_t  *pSMSCType;
} pack_sms_SetSMSCAddress_t;

/**
 * Set the SMS center address pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SetSMSCAddress(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SetSMSCAddress_t *reqParam
        );

/**
 *  This structure contains unpack Set SMS Address parameter.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SetSMSCAddress_t;

/**
 * Set the SMS center address unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SetSMSCAddress(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SetSMSCAddress_t *pOutput
        );

/**
 *  Sutructure containing parameters to Save an SMS message to device memory
 *
 *  @param  storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 *  @param  messageFormat
 *          - Message format
 *              - 0 - CDMA (IS-637B)
 *              - 1 - 5 (Reserved)
 *              - 6 - GSM/WCDMA PP
 *
 *  @param  messageSize
 *          - The length of the message contents in bytes
 *
 *  @param  pMessage
 *          - The message contents
 *
 *
 */
typedef struct{
    uint32_t storageType;
    uint32_t messageFormat;
    uint32_t messageSize;
    uint8_t  *pMessage;
} pack_sms_SaveSMS_t;

/**
 *  This structure contains unpack save SMS parameter.
 *  @param  pMessageIndex
 *          - The message index assigned by the device
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct{
    uint32_t *pMessageIndex;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SaveSMS_t;

/**
 * Saves an SMS message to device memory pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SaveSMS(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SaveSMS_t *reqParam
        );

/**
 * Saves an SMS message to device memory unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SaveSMS(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SaveSMS_t *pOutput
        );

/**
 *  This structure contains BroadcastConfig parameters
 *
 *  @param  fromServiceId
 *          - Starting point of range of CBM message identifiers
 *
 *  @param  toServiceId
 *          - Ending point of range of CBM message identifiers
 *
 *  @param  selected
 *          - Range of CBM message identifiers indicated by
 *            from_service_id and to_service_id
 *               - 0x00 - Not selected
 *               - 0x01 - Selected
 *
 */
typedef struct
{
    uint16_t fromServiceId;
    uint16_t toServiceId;
    uint8_t  selected;
} sms_BroadcastConfig;

/**
 *  This structure contains the 3GPP Broadcast Configuration Information
 *  parameters
 *
 *  @param  activated_ind
 *          - Broadcast SMS
 *              - 0x00 - Deactivated
 *              - 0x01 - Activated
 *
 *  @param  num_instances
 *          - Number of sets (N) of parameters
 *            Following each set describes one entry in the broadcast
 *            configuration table.
 *              - fromServiceId
 *              - toServiceId
 *              - selected
 *
 *  @param  broadcastConfig
 *          - A BroadcastConfig structure array.
 *          - Further defined by the structure sms_BroadcastConfig
 *
 */
typedef struct {
    uint8_t             activated_ind;
    uint16_t            num_instances;
    sms_BroadcastConfig broadcastConfig[SMS_CONFIG_LEN];
} sms_qaQmi3GPPBroadcastCfgInfo;

/**
 *  This structure contains CDMABroadcastConfig parameters
 *
 *  @param  serviceCategory
 *          - Service category
 *
 *  @param  language
 *          - Language
 *
 *  @param  selected
 *          - Specified service_category and language
 *               - 0x00 - Not selected
 *               - 0x01 - Selected
 *
 */
typedef struct{
    uint16_t serviceCategory;
    uint16_t language;
    uint8_t  selected;
} sms_CDMABroadcastConfig;

/**
 *  This structure contains the 3GPP2 Broadcast Configuration Information
 *  parameters
 *
 *  @param  activated_ind
 *          - Broadcast SMS
 *              - 0x00 - Deactivated
 *              - 0x01 - Activated
 *
 *  @param  num_instances
 *          - Number of sets (N) of parameters
 *            Following each set describes one entry in the broadcast
 *            configuration table.
 *              - serviceCategory
 *              - language
 *              - selected
 *
 *  @param  broadcastConfig
 *          - A CDMABroadcastConfig structure array.
 *          - Further defined by the structure sms_CDMABroadcastConfig
 *
 */
typedef struct{
    uint8_t                 activated_ind;
    uint16_t                num_instances;
    sms_CDMABroadcastConfig CDMABroadcastConfig[SMS_CONFIG_LEN];
} sms_qaQmi3GPP2BroadcastCfgInfo;

/**
 *  Structure contain parameters that Provides Information about the SMS BroadcastConfiguration
 *
 * @param  pBroadcastConfig
 *         - The data for 3GPP Broadcast Information(Optional).
 *         - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param  pCDMABroadcastConfig
 *         - The data for 3GPP2 Broadcast Information(Optional).
 *         - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    sms_qaQmi3GPPBroadcastCfgInfo  *pBroadcastConfig;
    sms_qaQmi3GPP2BroadcastCfgInfo *pCDMABroadcastConfig;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetSmsBroadcastConfig_t;

/**
 * This structure contains get SMS boardcast configure parameter.
 * @param  mode
 *         - Mode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
*/
typedef struct{
    uint8_t mode;
} pack_sms_SLQSGetSmsBroadcastConfig_t;

/**
 * Provides Information about the SMS BroadcastConfiguration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSGetSmsBroadcastConfig(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_sms_SLQSGetSmsBroadcastConfig_t *reqParam
        );

/**
 * Provides Information about the SMS BroadcastConfiguration unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSGetSmsBroadcastConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetSmsBroadcastConfig_t *pOutput
        );

/**
 * Structure containing parameters that provides the information about the SMS BroadcastConfiguration
 *
 * @param  mode
 *         - Mode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 * @param  pBroadcastConfig
 *         - The data for 3GPP Broadcast Information(Optional).
 *
 * @param  pCDMABroadcastConfig
 *         - The data for 3GPP2 Broadcast Information(Optional).
 *
 */
typedef struct{
    uint8_t                        mode;
    sms_qaQmi3GPPBroadcastCfgInfo  *pBroadcastConfig;
    sms_qaQmi3GPP2BroadcastCfgInfo *pCDMABroadcastConfig;
} pack_sms_SLQSSetSmsBroadcastConfig_t;

/**
 * Sets the information about the SMS BroadcastConfiguration pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSSetSmsBroadcastConfig(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_sms_SLQSSetSmsBroadcastConfig_t *reqParam
        );

/**
 * This structure contains unpack SLQSSetSmsBroadcastConfig parameters.
 *
 * @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSetSmsBroadcastConfig_t;

/**
 * Sets the information about the SMS BroadcastConfiguration unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSSetSmsBroadcastConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSSetSmsBroadcastConfig_t *pOutput
        );

/**
 *  Structure contain parameters that Enables or disables the reception of broadcast SMS messages.
 *
 *  @param  Mode
 *          - Mode
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 *  @param  broadcastActivate
 *          - 0x00 - Disable broadcast
 *          - 0x01 - Activate broadcast
 *
 */
typedef struct{
    uint8_t mode;
    uint8_t broadcastActivate;
} pack_sms_SLQSSetSmsBroadcastActivation_t;

/**
 * Enables or disables the reception of broadcast SMS messages pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSSetSmsBroadcastActivation(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSSetSmsBroadcastActivation_t *reqParam
        );

/**
 *   This structure contains unpack SLQSSetSmsBroadcastActivation parameter.
 *   @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/

typedef struct{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSetSmsBroadcastActivation_t;

/**
 * Enables or disables the reception of broadcast SMS messages unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSSetSmsBroadcastActivation(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSSetSmsBroadcastActivation_t *pOutput
        );

/**
 *  This structure contains Transport Layer Information
 *
 *  @param  TransType
 *          - Transport Type
 *              - 0x00 - IMS
 *
 *  @param  TransCap
 *          - Transport Capability
 *          - Values:
 *              - 0x00 - CDMA
 *              - 0x01 - GW
 */
typedef struct
{
    uint8_t TransType;
    uint8_t TransCap;
} sms_transLayerInfo;

/**
 *  This structure contains Get Transport Layer Info Response parameters
 *
 *  @param  pRegInd -
 *          - Optional parameter indicating if transport layer is registered
 *          - Values:
 *              - 0x00 - Transport layer is not registered
 *              - 0x01 - Transport layer is registered
 *          - returns a default value 0xFF
 *            if no response is received from the device.
 *
 *  @param  pTransLayerInfo
 *          - Pointer to structure of sms_transLayerInfo.
 *              - Optional parameter
 *              - See @ref sms_transLayerInfo for more information
 *          -  returns a default value 0xFF
 *            for parameter values if no response is received from the device.
 *
 */
typedef struct
{
    uint8_t            *pRegInd;
    sms_transLayerInfo *pTransLayerInfo;
} sms_getTransLayerInfo;

/**
 *  Structure contain parameters that gives information about the transport layer.
 *
 *  @param  pGetTransLayerInfo
 *          - Pointer to structure of getTransLayerInfo
 *              - See @ref sms_getTransLayerInfo for more information
 *              - pGetTransLayerInfo->pRegInd
 *                  - Bit to check in ParamPresenceMask - <B>16</B>
 *              - pGetTransLayerInfo->pTransLayerInfo
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *
 */
typedef struct
{
    sms_getTransLayerInfo  *pGetTransLayerInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetTransLayerInfo_t;

/**
 * Gets information about the transport layer pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSGetTransLayerInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Gets information about the transport layer unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSGetTransLayerInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetTransLayerInfo_t *pOutput
        );

/**
 *  This structure contains transport network registration info
 *  parameter
 *
 *  @param  pRegStatus -
 *          - Optional parameter indicating transport layer network
 *            registration status
 *          - Values:
 *              - 0x00 - No service
 *              - 0x01 - In progress
 *              - 0x02 - Failed
 *              - 0x03 - Limited Service
 *              - 0x04 - Full Service
 *          - returns a default value 0xFF
 *            if no response is received from the device.

 */
typedef struct
{
    uint8_t *pRegStatus;
} sms_getTransNWRegInfo;

/**
 *  Structure containing parameters that provides transport layer network registration info.
 *
 *  @param  pGetTransNWRegInfo
 *          - Pointer to structure of getTransNWRegInfo
 *              - See @ref sms_getTransNWRegInfo for more information
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    sms_getTransNWRegInfo *pGetTransNWRegInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetTransNWRegInfo_t;

 /**
  * Gets transport layer network registration info pack.
  * @param[in,out] pCtx qmi request context
  * @param[out] pReqBuf qmi request buffer
  * @param[out] pLen qmi request length
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int pack_sms_SLQSGetTransNWRegInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

 /**
  * Gets transport layer network registration info unpack.
  * @param[in] pResp qmi response
  * @param[in] respLen qmi response length
  * @param[out] pOutput unpacked response
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int unpack_sms_SLQSGetTransNWRegInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetTransNWRegInfo_t *pOutput
        );

/**
 *  This structure contains Get Indication Register Response parameters
 *
 *  @param  pRegTransLayerInfoEvt -
 *          - Optional parameter indicating registration status of
 *            transport layer information events
 *          - Values:
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *
 *  @param  pRegTransNWRegInfoEvt -
 *          - Optional parameter indicating registration status of
 *            transport network registration information events
 *          - Values:
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *
 *  @param  pRegCallStatInfoEvt -
 *          - Optional parameter indicating registration status of call
 *            status information events
 *          - Values:
 *              - 0x00 - Disabled
 *              - 0x01 - Enabled
 *
 */
typedef struct
{
   uint8_t *pRegTransLayerInfoEvt;
   uint8_t *pRegTransNWRegInfoEvt;
   uint8_t *pRegCallStatInfoEvt;
} sms_getIndicationReg;

/**
 *  Structure containing Parameters that provides registration state of different WMS indications.
 *
 *  @param  pGetIndicationRegInfo
 *          - Pointer to structure of getIndicationReg
 *              - See @ref sms_getIndicationReg for more information
 *              - pRegTransLayerInfoEvt
 *                  - Bit to check in ParamPresenceMask - <B>16</B>
 *              - pRegTransNWRegInfoEvt
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 *              - pRegCallStatInfoEvt
 *                  - Bit to check in ParamPresenceMask - <B>18</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
   sms_getIndicationReg *pGetIndicationRegInfo;
   swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetIndicationRegister_t;

/**
 * Gets registration state of different WMS indications pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSGetIndicationRegister(
       pack_qmi_t  *pCtx,
       uint8_t *pReqBuf,
       uint16_t *pLen
       );

/**
 * Gets registration state of different WMS indications unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSGetIndicationRegister(
       uint8_t *pResp,
       uint16_t respLen,
       unpack_sms_SLQSGetIndicationRegister_t *pOutput
       );


/**
*  This structure contains Indication Register request parameters
*
*  @param  pRegTransLayerInfoEvt -
*          - Optional parameter indicating registration status of
*            transport layer information events
*          - Values:
*              - 0x00 - Disabled
*              - 0x01 - Enabled
*              - NULL - No change - specifying NULL indicates that the device
*                will continue to use the existing setting (disable/enable)
*                which has been previously set for the device
*
*  @param  pRegTransNWRegInfoEvt -
*          - Optional parameter indicating registration status of
*            transport network registration information events
*          - Values:
*              - 0x00 - Disabled
*              - 0x01 - Enabled
*              - NULL - No change - specifying NULL indicates that
*                the device will continue to use the existing setting
*                (disable/enable) which has been previously set for the device
*
*  @param  pRegCallStatInfoEvt -
*          - Optional parameter indicating registration status of call
*            status information events
*          - Values:
*              - 0x00 - Disabled
*              - 0x01 - Enabled
*              - NULL - No change - specifying NULL indicates that the device
*                will continue to use the existing setting (disable/enable)
*                which has been previously set for the device
*
*/
typedef struct
{
   uint8_t *pRegTransLayerInfoEvt;
   uint8_t *pRegTransNWRegInfoEvt;
   uint8_t *pRegCallStatInfoEvt;
} sms_setIndicationReg;

/**
*  Structure contain Parameter that sets the registration state of different WMS indications.
*
*  @param  pSetIndicationReg
*          - Pointer to structure of indicationRegReqParams
*              - See @ref sms_setIndicationReg for more information
*
*/
typedef struct
{
   sms_setIndicationReg *pSetIndicationRegReq;
} pack_sms_SLQSSetIndicationRegister_t;

/**
 * Sets the registration state of different WMS indications pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSSetIndicationRegister(
       pack_qmi_t  *pCtx,
       uint8_t *pReqBuf,
       uint16_t *pLen,
       pack_sms_SLQSSetIndicationRegister_t *reqParam
       );

/**
 *   This structure contains unpack SLQSSetIndicationRegister parameter.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
*/
typedef struct
{
   swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSetIndicationRegister_t;

/**
 * Sets the registration state of different WMS indications unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSSetIndicationRegister(
       uint8_t *pResp,
       uint16_t respLen,
       unpack_sms_SLQSSetIndicationRegister_t *pOutput
       );

/**
 *  This structure contains SMS route entry details
 *
 *  @param  messageType -
 *          - Message type matching this route
 *          - Values:
 *              - 0x00 - MESSAGE_TYPE_POINT_TO_POINT
 *
 *  @param  messageClass -
 *          - Message Class
 *          - Values:
 *              - 0x00 - MESSAGE_CLASS_0
 *              - 0x01 - MESSAGE_CLASS_1
 *              - 0x02 - MESSAGE_CLASS_2
 *              - 0x03 - MESSAGE_CLASS_3
 *              - 0x04 - MESSAGE_CLASS_NONE
 *              - 0x05 - MESSAGE_CLASS_CDMA
 *
 *  @param  routeStorage -
 *          - If the receiptAction is store where to store the message
 *          - Values:
 *              - 0x00 - STORAGE_TYPE_UIM
 *              - 0x01 - STORAGE_TYPE_NV
 *              - 0xFF - STORAGE_TYPE_NONE
 *
 *  @param  receiptAction -
 *          - Action to be taken on receipt of a message matching the specified
 *            type and class for this route
 *          - Values:
 *              - 0x00 - DISCARD (discarded without notification)
 *              - 0x01 - STORE AND NOTIFY (stored and notified to the
 *                registered clients)
 *              - 0x02 - TRANSFER ONLY (transferred to the client, client
 *                expected to send the ACK)
 *              - 0x03 - TRANSFER AND ACK (transferred to the client, device
 *                expected to send the ACK)
 */
typedef struct
{
   uint8_t messageType;
   uint8_t messageClass;
   uint8_t routeStorage;
   uint8_t receiptAction;
} sms_routeEntry;

/**
 *  This structure contains SMS route request parameters
 *
 *  @param  numOfRoutes -
 *          - Number of sets of the following element
 *
 *  @param  routeList -
 *          - Array containing the set of @ref sms_routeEntry
 *
 *  @param  pTransferStatusReport -
 *          - 0x01 - Status report are transferred to the client (optional)
 */
typedef struct
{
   uint16_t       numOfRoutes;
   sms_routeEntry routeList[SMS_MAX_SMS_ROUTES];
   uint8_t        *pTransferStatusReport;
} sms_setRoutesReq;

/**
 *  Structure contain parameter that Sets the action performed on SMS message receipt for specified
 *  message routes. It also specifies the action performed on SMS receipt of
 *  status reports.
 *
 *  @param  pSetRoutesReq
 *          - Pointer to structure of smsSetRoutesReq
 *              - See @ref sms_setRoutesReq for more information
 *
 */
typedef struct
{
   sms_setRoutesReq *pSetRoutesReq;
} pack_sms_SLQSSmsSetRoutes_t;

/**
* Sets the action performed on SMS message receipt for specified
*  message routes. It also specifies the action performed on SMS receipt of
*  status reports pack.
* @param[in,out] pCtx qmi request context
* @param[out] pReqBuf qmi request buffer
* @param[out] pLen qmi request length
* @param reqParam packed request
*
* @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
*
* @sa     See qmerrno.h for eQCWWAN_xxx error values
*/
int pack_sms_SLQSSmsSetRoutes(
      pack_qmi_t  *pCtx,
      uint8_t *pReqBuf,
      uint16_t *pLen,
      pack_sms_SLQSSmsSetRoutes_t *reqParam
      );

/**
 *   This structure contains unpack SLQSSmsSetRoutes parameter.
 *   @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
 * 
**/
typedef struct 
{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSmsSetRoutes_t;
/**
* Sets the action performed on SMS message receipt for specified
*  message routes. It also specifies the action performed on SMS receipt of
*  status reports unpack.
* @param[in] pResp qmi response
* @param[in] respLen qmi response length
* @param[out] pOutput unpacked response
* @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
*
* @sa     See qmerrno.h for eQCWWAN_xxx error values
*/
int unpack_sms_SLQSSmsSetRoutes(
      uint8_t *pResp,
      uint16_t respLen,
      unpack_sms_SLQSSmsSetRoutes_t *pOutput
      );

/**
 *  This structure contains get message protocol response parameters
 *
 *  @param  msgProtocol -
 *          - Message Protocol
 *          - Values:
 *              - 0x00 - MESSAGE_PROTOCOL_CDMA
 *              - 0x01 - MESSAGE_PROTOCOL_WCDMA
 */
typedef struct
{
    uint8_t msgProtocol;
} sms_msgProtocolResp;

/**
 *  Structure contain Parameters that get the message protocol currently in use for the WMS client.
 *
 *  @param  pMessageProtocol
 *          - Pointer to smsMsgprotocolResp
 *              - See @ref sms_msgProtocolResp for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    sms_msgProtocolResp *pMessageProtocol;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSmsGetMessageProtocol_t;

/**
  * Gets the message protocol currently in use for the WMS client pack.
  * @param[in,out] pCtx qmi request context
  * @param[out] pReqBuf qmi request buffer
  * @param[out] pLen qmi request length
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int pack_sms_SLQSSmsGetMessageProtocol(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

 /**
  * Gets the message protocol currently in use for the WMS client unpack.
  * @param[in] pResp qmi response
  * @param[in] respLen qmi response length
  * @param[out] pOutput unpacked response
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int unpack_sms_SLQSSmsGetMessageProtocol(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSSmsGetMessageProtocol_t *pOutput
        );

/**
 *  This structure contains get store max size request parameters
 *
 *  @param  storageType
 *          - SMS message storage type
 *              - 0 - UIM - Invalid in case of CDMA device that does not
 *                          require SIM
 *              - 1 - NV
 *
 *  @param  pMessageMode(optional parameter)
 *          - 0x00 - CDMA, LTE (if network type is CDMA)
 *          - 0x01 - GW, LTE (if network type is UMTS)
 *
 *  @note   The Message Mode TLV must be included if the device is capable of
 *          supporting more than one protocol
 */
typedef struct
{
    uint8_t storageType;
    uint8_t *pMessageMode;
} sms_maxStorageSizeReq;


/**
 *  This structure contains get store max size response parameters
 *
 *  @param  maxStorageSize -
 *          - Memory Store Size
 *
 *  @param  freeSlots -
 *          - Optional parameter indicating how much Memory is available
 *          -  returns a default value 0xFFFFFFFF for
 *             parameter values if no response is received from
 *            the device.
 */
typedef struct
{
    uint32_t maxStorageSize;
    uint32_t freeSlots;
} sms_maxStorageSizeResp;

/**
 *  This structure contains get maximum storage size.
 *  @param  pMaxStorageSizeReq
 *          - Request parameters for SmsSLQSGetMaxStorageSize
 *              - See @ref sms_maxStorageSizeReq for more information
 */
typedef struct
{
    sms_maxStorageSizeReq *pMaxStorageSizeReq;
} pack_sms_SLQSSmsGetMaxStorageSize_t;

/**
 *  Structure contain parameters that provides the maximum number of messages that can be stored in the
 *  specified memory storage. Also it provides the number of slots currently
 *  available
 *
 *  @param  pMaxStorageSizeResp
 *          - Response parameters for SmsSLQSGetMaxStorageSize
 *              - See @ref sms_maxStorageSizeResp for more information
 *          - pMaxStorageSizeResp->maxStorageSize
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *          - pMaxStorageSizeResp->freeSlots
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    sms_maxStorageSizeResp *pMaxStorageSizeResp;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSmsGetMaxStorageSize_t;

/**
 * Get the maximum number of messages that can be stored in the
 * specified memory storage. Also it provides the number of slots currently
 * available pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSSmsGetMaxStorageSize(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_sms_SLQSSmsGetMaxStorageSize_t *reqParam
        );

/**
 * Get the maximum number of messages that can be stored in the
 * specified memory storage. Also it provides the number of slots currently
 * available unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSSmsGetMaxStorageSize(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSSmsGetMaxStorageSize_t *pOutput
        );

/**
 *  This structure contains message waiting information per instance
 *
 *  @param  msgType
 *          - Message type
 *              - 0x00 - MWI_MESSAGE_TYPE_VOICMAIL - Voicemail
 *              - 0x01 - MWI_MESSAGE_TYPE_FAX - Fax
 *              - 0X02 - MWI_MESSAGE_TYPE_EMAIL - Email
 *              - 0x03 - MWI_MESSAGE_TYPE_OTHER - Other
 *              - 0x04 - MWI_MESSAGE_TYPE_VIDEOMAIL - Videomail
 *
 *  @param  activeInd
 *          - Indicates whether the indication is active
 *              - 0x00 - Inactive
 *              - 0x01 - Active
 *
 *  @param  msgCount
 *          - Number of messages
 */
typedef struct
{
    uint8_t msgType;
    uint8_t activeInd;
    uint8_t msgCount;
} sms_messageWaitingInfoContent;

/**
 *  This structure contains Get Message Waiting Info Response parameters
 *
 *  @param  numInstances
 *          - Number of sets of the elements in structure sms_messageWaitingInfoContent
 *
 *  @param  pMsgWaitInfo
 *          - Pointer to structure of sms_messageWaitingInfoContent.
 *              - See @ref sms_messageWaitingInfoContent for more information.
 *
 */
typedef struct
{
    uint8_t                       numInstances;
    sms_messageWaitingInfoContent msgWaitInfo[SMS_NUM_OF_SET];
} sms_getMsgWaitingInfo;

/**
 *  Structure contain Parameter that provide information about the message waiting information.
 *
 *  @param  pGetMsgWaitingInfoResp
 *          - Pointer to structure of getMsgWaitingInfoResp
 *              - See @ref sms_getMsgWaitingInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    sms_getMsgWaitingInfo  *pGetMsgWaitingInfoResp;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSGetMessageWaiting_t;

/**
  * Gets the message waiting information pack.
  * @param[in,out] pCtx qmi request context
  * @param[out] pReqBuf qmi request buffer
  * @param[out] pLen qmi request length
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int pack_sms_SLQSGetMessageWaiting(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

 /**
  * Gets the message waiting information unpack.
  * @param[in] pResp qmi response
  * @param[in] respLen qmi response length
  * @param[out] pOutput unpacked response
  *
  * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
  *
  * @sa     See qmerrno.h for eQCWWAN_xxx error values
  *
  */
int unpack_sms_SLQSGetMessageWaiting(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSGetMessageWaiting_t *pOutput
        );

/**
 *  This structure contains SMS parameters
 *
 *  @param  messageFormat
 *          - Message format
 *          - Values:
 *              - 0 - CDMA (IS-637B)
 *              - 1 - 5 (Reserved)
 *              - 6 - GSM/WCDMA PP
 *
 *  @param  messageSize
 *          - The length of the message contents in bytes
 *
 *  @param  pMessage
 *          - The message contents
 *
 *  @param  pForceOnDC
 *          - Force the message to be sent on the CDMA dedicated channel.
 *          - Values:
 *              - 0x00 - Do not care about the channel on which the message is sent
 *              - 0x01 - Request to send the message over the dedicated channel
 *
 *  @param  pServiceOption
 *          - Service option:
 *          - Values:
 *              - 0x00 - SO_AUTO - AUTO (choose the best service option)
 *              - 0x06 - SO_6 - Service option 6
 *              - 0x0E - SO_14 - Service option 14
 *
 *  @param  pFollowOnDC
 *          - Flag to request not to disconnect the CDMA dedicated channel
 *            after the send operation is complete.
 *          - This TLV can be included if more messages are expected to follow.
 *          - Values:
 *              - 0x01 - FOLLOW_ON_DC_ON - On (don't disconnect after send operation)
 *                Any value other than 0x01 is treated as an absence of this TLV.
 *
 *  @param  pLinktimer
 *          - Keeps the GW SMS link open for the specified number of seconds;
 *            can be enabled if more messages are expected to follow
 *
 *  @param  pSmsOnIms
 *          - Indicates whether the message is to be sent on IMS.
 *          - Values:
 *              - 0x00 - Message is not to be sent on IMS
 *              - 0x01 - Message is to be sent on IMS
 *              - 0x02 to 0xFF - Reserved
 *
 *  @param  pRetryMessage
 *          - Indicates this message is a retry message.
 *          - Values:
 *              - 0x01 - WMS_MESSAGE_IS_A_RETRY - Message is a retry message
 *              Note: Any value other than 0x01 in this field is treated
 *              as an absence of this TLV.
 *
 *
 *  @param  pRetryMessageId
 *          - Message ID to be used in the retry message.
 *          - The message ID specified here is used instead of the messsage ID
 *            encoded in the raw message.
 *
 *  @param  pUserData
 *          - Enables the control point to associate the request with the
 *            corresponding indication.
 *          - The control point might send numerous requests.
 *          - This TLV will help the control point to identify the request
 *            for which the received indication belongs.
 */
typedef struct
{
    uint32_t messageFormat;
    uint32_t messageSize;
    uint8_t  *pMessage;
    uint8_t  *pForceOnDC;
    uint8_t  *pServiceOption;
    uint8_t  *pFollowOnDC;
    uint8_t  *pLinktimer;
    uint8_t  *pSmsOnIms;
    uint8_t  *pRetryMessage;
    uint32_t *pRetryMessageId;
    uint32_t *pUserData;
} sms_sendAsyncsmsParams;

/**
 *  Structure contain Parameter to Send an SMS message for immediate over-the-air transmission
 *
 *  @param  pSendSmsParams
 *          - structure containing the SMS parameters. Refer sms_sendasyncsmsparams
 *
 */
typedef struct
{
    sms_sendAsyncsmsParams *pSendSmsParams;
} pack_sms_SLQSSendAsyncSMS_t;

/**
 * Sends an SMS message for immediate over-the-air transmission pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSSendAsyncSMS(
       pack_qmi_t  *pCtx,
       uint8_t *pReqBuf,
       uint16_t *pLen,
       pack_sms_SLQSSendAsyncSMS_t *reqParam
       );

/**
 *   This structure contains unpack SLQSSendAsyncSMS parameter.
 *   @param ParamPresenceMask
 *      - bitmask representation to indicate valid parameters.
**/
typedef struct
{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSendAsyncSMS_t;

/**
 * Sends an SMS message for immediate over-the-air transmission unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSSendAsyncSMS(
       uint8_t *pResp,
       uint16_t respLen,
       unpack_sms_SLQSSendAsyncSMS_t *pOutput
       );

/**
 *  Structure contain Parameter to set the SMS Storage on the device
 *
 * @param  smsStorage
 *         - SMS Storage
 *          - 0x01 - device’s permanent memory
 *          - 0x02 - UICC
 *
 */
typedef struct
{
    uint8_t smsStorage;
} pack_sms_SLQSSetSmsStorage_t;

/**
 * Sets the SMS Storage on the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param reqParam packed request
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_sms_SLQSSetSmsStorage(
       pack_qmi_t  *pCtx,
       uint8_t *pReqBuf,
       uint16_t *pLen,
       pack_sms_SLQSSetSmsStorage_t *reqParam
       );

/**
 *   This structure contains unpack SLQSSetSmsStorage parameter.
 *   @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct
{
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSetSmsStorage_t;

/**
 * Sets the SMS Storage on the device unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_sms_SLQSSetSmsStorage(
       uint8_t *pResp,
       uint16_t respLen,
       unpack_sms_SLQSSetSmsStorage_t *pOutput
       );

/**
 *  Structure contain Parameters that  return current
 *  SMS configuration that is applied to all incoming and outgoing messages.
 *
 *  @param  pSmsStorage
 *          - Values:
 *              - 0x01 - device's permanent memory
 *              - 0x02 - UICC
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint32_t *pSmsStorage;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSSwiGetSMSStorage_t;

/**
 * Gets the current SMS configuration that is applied
 * to all incoming and outgoing messages pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_sms_SLQSSwiGetSMSStorage(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

 /**
 * Gets the current SMS configuration that is applied
 * to all incoming and outgoing messages unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSSwiGetSMSStorage(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSSwiGetSMSStorage_t *pOutput
        );

/**
 *  Contains the parameters passed for the indication about change in 
 *  transport layer information 
 *
 *  @param  regInd
 *          - Indicates whether the transport layer is registered or not
 *          - Values:
 *              - 0x00 - Transport layer is not registered
 *              - 0x01 - Transport layer is registered
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pTransLayerInfo
 *          - Optional parameter
 *          - See @ref sms_transLayerInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 * @note    None
 *
 */
typedef struct
{
    uint8_t           regInd;
    sms_transLayerInfo *pTransLayerInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSTransLayerInfoCallback_ind_t;

 /**
 * Unpack indication about change in transport layer info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSTransLayerInfoCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSTransLayerInfoCallback_ind_t *pOutput
        );

/**
 *  Contains the parameters passed for indication about transport network 
 *  registration change by the device.
 *
 *  @param  NWRegStat
 *          - provides the transport network registration information
 *          - Values:
 *              - 0x00 - No Service
 *              - 0x01 - In Progress
 *              - 0x02 - Failed
 *              - 0x03 - Limited Service
 *              - 0x04 - Full Service
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 * @note    None
 *
 */
typedef struct
{
    uint8_t NWRegStat;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSNWRegInfoCallback_ind_t;

 /**
 * Unpack indication about change in transport layer info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSNWRegInfoCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSNWRegInfoCallback_ind_t *pOutput
        );

/**
 *  This structure holds information related to message waiting information indication
 *
 *  @param  numInstances
 *          - Number of sets of the elements in structure sms_messageWaitingInfoContent
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  msgWaitInfo
 *          - Pointer to structure of sms_messageWaitingInfoContent.
 *              - See @ref sms_messageWaitingInfoContent for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint8_t                      numInstances;
    sms_messageWaitingInfoContent msgWaitInfo[SMS_NUM_OF_SET];
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t;

 /**
 * Unpack indication for message waiting information
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSWmsMessageWaitingCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t *pOutput
        );

/**
 *  This structure contains SMS parameters for indication of RAW ASYNC SEND
 *
 *  @param  sendStatus
 *          - Send Status
 *          - Values:
 *              - QMI_ERR_NONE - No error in the request
 *              - QMI_ERR_CAUSE_CODE - SMS cause code
 *              - QMI_ERR_MESSAGE_DELIVERY_FAILURE - Message could not be delivered
 *              - QMI_ERR_NO_MEMORY - Device could not allocate memory to formulate
 *                a response
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  messageID
 *          - Unique ID assigned by WMS for non-retry messages.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  causeCode
 *          - WMS cause code
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  errorClass
 *          - Error Class
 *          - Values:
 *              - 0x00 - ERROR_CLASS_TEMPORARY
 *              - 0x01 - ERROR_CLASS_PERMANENT
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  RPCause
 *          - GW RP cause
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  TPCause
 *          - GW TP Cause
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  msgDelFailureType
 *          - Message delivery failure type
 *          - Values:
 *              - 0x00 - WMS_MESSAGE_DELIVERY_FAILURE_TEMPORARY
 *              - 0x01 - WMS_MESSAGE_DELIVERY_FAILURE_PERMANENT
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  msgDelFailureCause
 *          - Message delivery failure cause
 *          - Values:
 *              - 0x00 - WMS_MESSAGE_BLOCKED_DUE_TO_CALL_CONTROL
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  alphaIDLen
 *          - Number of sets of the pAlphaID
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pAlphaID
 *          - Alpha ID
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  userData
 *          - Identifies the request associated with this indication.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint16_t  sendStatus;
    uint16_t  messageID;
    uint16_t  causeCode;
    uint8_t  errorClass;
    uint16_t  RPCause;
    uint8_t  TPCause;
    uint8_t  msgDelFailureType;
    uint8_t  msgDelFailureCause;
    uint8_t  alphaIDLen;
    uint8_t  *pAlphaID;
    uint32_t userData;
    swi_uint256_t  ParamPresenceMask;
} unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t;

 /**
 * Unpack indication for sms async raw send
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_sms_SLQSWmsAsyncRawSendCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t *pOutput
        );


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif

