/**
 * \ingroup liteqmi
 *
 * \file imsa.h
 */

#ifndef __LITEQMI_IMSA_H__
#define __LITEQMI_IMSA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#include "msgid.h"

#define MAX_ERROR_CODE_LEN 256

/**
 *  This structure contains parameters of IMSA Config Indication Register
 *
 *  \param  RegStatusConfig(optional)
 *          - Register Indication For Registration status.
 *          - When this registration is enabled, the device learns of Registration
 *            status via the QMI_IMSA_REGISTRATION_STATUS_IND
 *            indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  ServiceStatusConfig(optional)
 *          - Register Indication For Service status
 *            Events.
 *          - When this registration is enabled, the device learns of Service
 *            status via the QMI_IMSA_SERVICE_STATUS_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  RatHandoverStatusConfig(optional)
 *          - Registration Indication For RAT handover status.
 *          - When this registration is enabled, the device learns of RAT handover
 *            status via the QMI_IMSA_RAT_HANDOVER_STATUS_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  PdpStatusConfig(optional)
 *          - PDP Status Configuration.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \note   One of the optional parameter is mandatory to be present in the
 *          request.
 */
typedef struct{
    int has_RegStatusConfig;
    uint8_t RegStatusConfig;
    int has_ServiceStatusConfig;
    uint8_t ServiceStatusConfig;
    int has_RatHandoverStatusConfig;
    uint8_t RatHandoverStatusConfig;
    int has_PdpStatusConfig;
    uint8_t PdpStatusConfig;
} pack_imsa_SLQSRegisterIMSAIndication_t;

typedef unpack_result_t unpack_imsa_SLQSRegisterIMSAIndication_t;

/**
 *  Function to pack Register IMSA Indication command
 *
 *  \param pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *
 *  \param reqParam [IN]
 *          - See \ref pack_imsa_SLQSRegisterIMSAIndication_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_imsa_SLQSRegisterIMSAIndication (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_imsa_SLQSRegisterIMSAIndication_t  *reqParam
        );

/**
 *  Function to unpack Register IMSA Indication response from modem
 *
 *  \param pResp [IN]
 *            - Response from modem
 *
 *  \param respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param pOutput [OUT]
 *            - See \ref unpack_imsa_SLQSRegisterIMSAIndication_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSRegisterIMSAIndication(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSRegisterIMSAIndication_t *pOutput
);

/**
 *  Function to pack Get IMSA Registration Status command.
 *  \param pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_imsa_SLQSGetIMSARegStatus(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  This structure contains response parameters of registration status.
 *
 *  \param  ImsRegStatus
 *          - IMS Registration Status (Deprecated).
 *          - Values
 *             - TRUE  - UE is registered on the IMS network
 *             - FALSE - UE is not registered on the IMS network
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  ImsRegErrCode
 *          - IMS Registration Error Code.
 *          - An error code is returned when the IMS registration
 *            status is IMSA_STATUS_NOT_REGISTERED.
 *          -Values
 *            - 3xx - Redirection responses
 *            - 4xx - Client failure responses
 *            - 5xx - Server failure responses
 *            - 6xx - Global failure responses
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  NewImsRegStatus
 *          - New IMS Registration Status
 *          - Values
 *             - 0 - Not registered for IMS
 *             - 1 - Registering for IMS
 *             - 2 - Registered for IMS
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 */
typedef struct {
    uint8_t        ImsRegStatus;
    uint16_t       ImsRegErrCode;
    uint32_t       NewImsRegStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_imsa_SLQSGetIMSARegStatus_t;

/**
 *  Function to unpack Get IMSA Registration Status response from modem. *
 *  \param pResp [IN]
 *            - Response from modem
 *
 *  \param respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param pOutput [OUT]
 *            - See \ref unpack_imsa_SLQSGetIMSARegStatus_t for more information
 *
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSGetIMSARegStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSGetIMSARegStatus_t *pOutput
        );

/**
 *  Function to pack Get IMSA Service Status command
 *
 *  \param pCtx [OUT]
 *            - See \ref pack_qmi_t for more information
 *
 *  \param pReqBuf [IN/OUT]
 *            - Buffer for packed QMI command to be provided by the host application
 *            - Minimum expected size is 2048 bytes
 *
 *  \param pLen [IN/OUT]
 *          - On input, size of pReqBuf
 *          - On ouptut, number of bytes actually packed
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_imsa_SLQSGetIMSAServiceStatus(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  This structure contains response parameters of service status for various IMS services.
 *
 *  \param  SmsServiceStatus
 *          - SMS Service Status.
 *          - Values
 *             - 0 - IMS SMS service is not available
 *             - 1 - IMS SMS is in limited service
 *             - 2 - IMS SMS is in full service
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  VoipSeriveStatus
 *          - VoIP Service Status.
 *          - Values
 *            - 0 - IMS VoIP service is not available
 *            - 2 - IMS VoIP is in full service
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  VtServiceStatus
 *          - VT Service Status
 *          - Values
 *             - 0 - IMS VT service is not available
 *             - 2 - IMS VT is in full service
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  SmsServiceRat
 *          - SMS service RAT
 *          - Values
 *             - 0 - IMS service is registered on WLAN
 *             - 1 - IMS service is registered on WWAN
 *             - 2 - IMS service is registered on interworking WLAN
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  VoipServiceRat
 *          - VoIP service RAT.
 *          - Values
 *             - 0 - IMS service is registered on WLAN
 *             - 1 - IMS service is registered on WWAN
 *             - 2 - IMS service is registered on interworking WLAN
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  VtServiceRat
 *          - VT service RAT.
 *          - Values
 *             - 0 - IMS service is registered on WLAN
 *             - 1 - IMS service is registered on WWAN
 *             - 2 - IMS service is registered on interworking WLAN
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  UtServiceStatus
 *          - UT service Status.
 *          - Values
 *             - 0 - IMS UT service is not available
 *             - 2 - IMS UT is in full service
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  UtServiceRat
 *          - UT service RAT.
 *          - Values
 *             - 0 - IMS service is registered on WLAN
 *             - 1 - IMS service is registered on WWAN
 *             - 2 - IMS service is registered on interworking WLAN
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  VsServiceStatus
 *          - VS service Status.
 *          - Values
 *             - 0 - IMS UT service is not available
 *             - 2 - IMS UT is in full service
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  VsServiceRat
 *          - VS service RAT.
 *          - Values
 *             - 0 - IMS service is registered on WLAN
 *             - 1 - IMS service is registered on WWAN
 *             - 2 - IMS service is registered on interworking WLAN
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 */
typedef struct {
    uint32_t         SmsServiceStatus;
    uint32_t         VoipServiceStatus;
    uint32_t         VtServiceStatus;
    uint32_t         SmsServiceRat;
    uint32_t         VoipServiceRat;
    uint32_t         VtServiceRat;
    uint32_t         UtServiceStatus;
    uint32_t         UtServiceRat;
    uint32_t         VsServiceStatus;
    uint32_t         VsServiceRat;
    swi_uint256_t    ParamPresenceMask;
} unpack_imsa_SLQSGetIMSAServiceStatus_t;

/**
 *  Function to unpack Get IMSA Service Status response from modem
 *
 *  \param  pResp [IN]
 *            - Response from modem
 *
 *  \param  respLen [IN]
 *            - Length of pResp from modem
 *
 *  \param  pOutput [OUT]
 *            - See \ref unpack_imsa_SLQSGetIMSAServiceStatus_t for more information
 *
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSGetIMSAServiceStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSGetIMSAServiceStatus_t *pOutput
        );

/**
 *  This structure hold parameters about SMS service info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param SmsSvcStatus
 *          - Values:
 *          -0 - IMS SMS service is not available
 *          -1 - IMS SMS is in limited service
 *          -2 - IMS SMS is in full service
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t SmsSvcStatus;
} imsa_SmsSvcStatusInfo;

/**
 *  This structure hold parameters about SMS RAT info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param SmsRatVal
 *          - Values:
 *          - 0-IMSA_WLAN - IMS service is registered on WLAN
 *          - 1-IMSA_WWAN - IMS service is registered on WWAN
 *          - 2-IMSA_IWLAN - IMS service is registered on interworking WLAN
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t SmsRatVal;
} imsa_SmsRatInfo;

/**
 *  This structure hold parameters about VOIP service info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param VoipSvcStatus
 *          - Values:
 *          -0 - IMS VOIP service is not available
 *          -2 - IMS VOIP is in full service
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t VoipSvcStatus;
} imsa_VoipSvcStatusInfo;

/**
 *  This structure hold parameters about VOIP RAT info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param VoipRatVal
 *          - Values:
 *          - 0-IMSA_WLAN - IMS service is registered on WLAN
 *          - 1-IMSA_WWAN - IMS service is registered on WWAN
 *          - 2-IMSA_IWLAN - IMS service is registered on interworking WLAN
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t VoipRatVal;
} imsa_VoipRatInfo;

/**
 *  This structure hold parameters about VT service info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param VtSvcStatus
 *          - Values:
 *          -0 - IMS VT service is not available
 *          -2 - IMS VT is in full service
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t VtSvcStatus;
} imsa_VtSvcStatusInfo;

/**
 *  This structure hold parameters about VT RAT info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param VtRatVal
 *          - Values:
 *          - 0-IMSA_WLAN - IMS service is registered on WLAN
 *          - 1-IMSA_WWAN - IMS service is registered on WWAN
 *          - 2-IMSA_IWLAN - IMS service is registered on interworking WLAN
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t VtRatVal;
} imsa_VtRatInfo;


/**
 *  This structure hold parameters about UT service info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param UtSvcStatus
 *          - Values:
 *          -0 - IMS UT service is not available
 *          -2 - IMS UT is in full service
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t UtSvcStatus;
} imsa_UtSvcStatusInfo;

/**
 *  This structure hold parameters about UT RAT info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param UtRatVal
 *          - Values:
 *          - 0-IMSA_WLAN - IMS service is registered on WLAN
 *          - 1-IMSA_WWAN - IMS service is registered on WWAN
 *          - 2-IMSA_IWLAN - IMS service is registered on interworking WLAN
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t UtRatVal;
} imsa_UtRatInfo;


/**
 *  Structure used to store IMSA service status indication Parameters. 
 *  @param  SmsService
 *          - SMS service info
 *          - See @ref imsa_SmsSvcStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  VoipService
 *          - VOIP service info 
 *          - see @ref imsa_VoipSvcStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param VtService
 *          - VT service info
 *          - see @ref imsa_VtSvcStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param SmsRat
 *          - SMS RAT info
 *          - see @ref imsa_SmsRatInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param VoipRat
 *          - VOIP RAT info
 *          - see @ref imsa_VoipRatInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param VtRat
 *          - VT RAT info
 *          - see @ref imsa_VtRatInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param UtService
 *          - UT service info
 *          - see @ref imsa_UtSvcStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param UtRat
 *          - UT RAT info
 *          - see @ref imsa_UtRatInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 * @note:    None
 */
typedef struct{
    imsa_SmsSvcStatusInfo    SmsService;
    imsa_VoipSvcStatusInfo   VoipService;
    imsa_VtSvcStatusInfo     VtService;
    imsa_SmsRatInfo          SmsRat;
    imsa_VoipRatInfo         VoipRat;
    imsa_VtRatInfo           VtRat;
    imsa_UtSvcStatusInfo     UtService;
    imsa_UtRatInfo           UtRat;
    swi_uint256_t            ParamPresenceMask;
} unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t;

 /**
 * Unpack indication about IMSA service status info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSImsaSvcStatusCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t *pOutput
        );

/**
 *  This structure hold parameters about IMS registration info (decprecated tlv). 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param ImsRegistered
 *          - Values:
 *          - 0 -Not registered
 *          - 1- Registered
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t  ImsRegistered;
} imsa_IMSRegStatusInfo;

/**
 *  This structure hold parameters about IMS registration error info. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param ErrorCode
 *          Error codes when registration status is IMSA_STATUS_NOT_REGISTERED
 *          - Values:
 *          - 3xx - Redirection responses
 *          - 4xx - Client failure responses
 *          - 5xx - Server failure responses
 *          - 6xx - Global failure responses
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t  ErrorCode;
} imsa_IMSRegStatusErrorCodeInfo;

/**
 *  This structure hold parameters about IMS registration info (new tlv). 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param ImsRegStatus
 *          - Values:
 *          - 0 -IMSA_STATUS_NOT_REGISTERED- Not registered for IMS
 *          - 1- IMSA_STATUS_REGISTERING - Registering for IMS
 *          - 2- IMSA_STATUS_REGISTERED - Registered for IMS
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t  ImsRegStatus;
} imsa_NewIMSRegStatusInfo;

/**
 *  Structure used to store IMSA registration status indication Parameters. 
 *  @param  IMSRegistration
 *          - IMS registration info
 *          - See @ref imsa_IMSRegStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  IMSRegistrationError
 *          - IMS registration error 
 *          - see @ref imsa_IMSRegStatusErrorCodeInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param NewIMSRegistration
 *          - New IMS registration info
 *          - see @ref imsa_NewIMSRegStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @note:    None
 */
typedef struct{
    imsa_IMSRegStatusInfo          IMSRegistration;
    imsa_IMSRegStatusErrorCodeInfo IMSRegistrationError;
    imsa_NewIMSRegStatusInfo       NewIMSRegistration;
    swi_uint256_t                  ParamPresenceMask;
} unpack_imsa_SLQSImsaRegStatusCallBack_ind_t;

 /**
 * Unpack indication about IMSA registration status info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSImsaRegStatusCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSImsaRegStatusCallBack_ind_t *pOutput
        );

/**
 *  This structure hold parameters about RAT handover status. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param RatHandoverStatus
 *          - RAT handover status
 *  @param SourceRAT
 *          - source RAT info: IWLAN/WWAN
 *  @param TargetRAT
 *          - target TAT info: IWLAN/WWAN
 *  @param ErrorCodeLen
 *          - error code length
 *  @param ErrorCodeData
 *          - handover failure code string when status is IMSA_STATUS_RAT_HO_FAILURE  
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t RatHandoverStatus;
    uint32_t SourceRAT;
    uint32_t TargetRAT;
    uint8_t  ErrorCodeLen;
    uint8_t  ErrorCodeData[MAX_ERROR_CODE_LEN];
} imsa_RatHandoverStatusInfo;

/**
 *  Structure used to store IMSA RAT status indication Parameters. 
 *  @param  RatHandover
 *          - RAT handover status info
 *          - See @ref imsa_RatHandoverStatusInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @note:    None
 */
typedef struct{
    imsa_RatHandoverStatusInfo  RatHandover;
    swi_uint256_t               ParamPresenceMask;
} unpack_imsa_SLQSImsaRatStatusCallBack_ind_t;

 /**
 * Unpack indication about IMSA RAT status info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSImsaRatStatusCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSImsaRatStatusCallBack_ind_t *pOutput
        );

/**
 *  This structure hold parameters about IMS failure code for PDP connection. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param ImsFailErrCode
 *          - 0 -IMSA_PDP_STATUS_OTHER_FAILURE - Generic failure reason
 *          - 1 -IMSA_PDP_STATUS_OPTION_UNSUBSCRIBED - Option is unsubscribed
 *          - 2 -IMSA_PDP_STATUS_UNKNOWN_PDP - PDP was unknown 
 */
typedef struct
{
    uint8_t TlvPresent;
    uint32_t ImsFailErrCode;
} imsa_IMSFailErrCodeTlv;

/**
 *  Structure used to store IMSA PDP status indication Parameters. 
 *  @param  PdpConnState
 *          - Values:
 *            - 0- not connected
 *            - 1- connected
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  FailErrCode
 *          - Fail error code when PDP is not connected
 *          - See @ref imsa_IMSFailErrCodeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @note:    None
 */
typedef struct{
    uint8_t   PdpConnState;
    imsa_IMSFailErrCodeTlv  FailErrCode;
    swi_uint256_t           ParamPresenceMask;
} unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t;

 /**
 * Unpack indication about IMSA PDP status info
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_imsa_SLQSImsaPdpStatusCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t *pOutput
        );


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif //__LITEQMI_IMSA_H__
