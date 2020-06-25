/**
 * \ingroup liteqmi
 *
 * \file uim.h
 */
#ifndef __LITEQMI_UIM_H__
#define __LITEQMI_UIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define UIM_UINT8_MAX_STRING_SZ 255
#define UIM_MAX_DESCRIPTION_LENGTH 255
#define UIM_MAX_NO_OF_SLOTS 5
#define UIM_MAX_NO_OF_APPLICATIONS 10
#define MAX_NO_OF_SLOTS        5
#define MAX_NO_OF_APPLICATIONS 10
#define MAX_DESCRIPTION_LENGTH 255
#define MAX_SLOTS_STATUS       255
#define MAX_ICCID_LENGTH       255
#define UIM_MAX_CONTENT_LENGTH 1024
#define UIM_MAX_ACTIVE_PERS_FEATURES   12
#define MAX_ATR_LENGTH         255
#define MAX_PHY_SLOTS_INFO     255


/**
 *  This structure contains Application Status Information loaded on the card.
 *
 *  \param  appType
 *          - Indicates the type of the application.
 *              - 0 - Unknown
 *              - 1 - SIM card
 *              - 2 - USIM application
 *              - 3 - RUIM card
 *              - 4 - CSIM application
 *              - 5 - ISIM application
 *          - Other values are reserved for the future and are
 *            to be handled as "Unknown".
 *
 *  \param  appState
 *          - Indicates the state of the application.
 *              - 0 - Unknown
 *              - 1 - Detected
 *              - 2 - PIN1 or UPIN is required
 *              - 3 - PUK1 or PUK for UPIN is required
 *              - 4 - Personalization state must be checked
 *              - 5 - PIN1 is blocked
 *              - 6 - Illegal
 *              - 7 - Ready
 *
 *  \param  persoState
 *          - Indicates the state of the personalization for the application.
 *              - 0 - Unknown
 *              - 1 - Personalization operation is in progress
 *              - 2 - Ready
 *              - 3 - Personalization code is required
 *              - 4 - PUK for personalization code is required
 *              - 5 - Permanently blocked
 *
 *  \param  persoFeature
 *          - Indicates the personalization feature.
 *          - This applies only when a personalization code is required to
 *            deactivate or unblock personalization.
 *              - 0 - GW network personalization
 *              - 1 - GW network subset personalization
 *              - 2 - GW service provider personalization
 *              - 3 - GW corporate personalization
 *              - 4 - GW UIM personalization
 *              - 5 - 1X network type 1 personalization
 *              - 6 - 1X network type 2 personalization
 *              - 7 - 1X HRPD personalization
 *              - 8 - 1X service provider personalization
 *              - 9 - 1X corporate personalization
 *              - 10 - 1X RUIM personalization
 *              - 11 - Unknown
 *
 *  \param  persoRetries
 *          - Indicates the number of retries remaining to disable the
 *            personalization.
 *
 *  \param  persoUnblockRetries
 *          - Indicates the number of retries remaining to unblock the
 *            personalization.
 *
 *  \param  aidLength
 *          - Number of sets of the following elements. i.e. aidVal
 *          - If zero(0) then no aidVal information exists.
 *
 *  \param  aidVal[MAX_DESCRIPTION_LENGTH]
 *          - Application identifier value.
 *
 *  \param  univPin
 *          - Indicates whether UPIN replaces PIN1.
 *              - 0 - PIN1 is used
 *              - 1 - UPIN replaces PIN1
 *
 *  \param  pin1State
 *          - Indicates the state of PIN1.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *
 *  \param  pin1Retries
 *          - Indicates the number of retries remaining to verify PIN1.
 *
 *  \param  puk1Retries
 *          - Indicates the number of retries remaining to unblock PIN1.
 *
 *  \param  pin2State
 *          - Indicates the state of PIN2.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *
 *  \param  pin2Retries
 *          - Indicates the number of retries remaining to verify PIN2.
 *
 *  \param  puk2Retries
 *          - Indicates the number of retries remaining to unblock PIN2.
 *
 */
typedef struct
{
    uint8_t appType;
    uint8_t appState;
    uint8_t persoState;
    uint8_t persoFeature;
    uint8_t persoRetries;
    uint8_t persoUnblockRetries;
    uint8_t aidLength;
    uint8_t aidVal[UIM_MAX_DESCRIPTION_LENGTH];
    uint8_t univPin;
    uint8_t pin1State;
    uint8_t pin1Retries;
    uint8_t puk1Retries;
    uint8_t pin2State;
    uint8_t pin2Retries;
    uint8_t puk2Retries;
} uim_appStatus;


/**
 *  This structure contains information about the SLOTS present.
 *
 *  \param  cardState
 *          - Indicates the state of the card for each slot.
 *              - 0 - Absent
 *              - 1 - Present
 *              - 2 - Error
 *
 *  \param  upinState
 *          - Indicates the state of UPIN.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *              - 0xFF - Not Available
 *
 *  \param  upinRetries
 *          - Indicates the number of retries remaining to verify the UPIN.
 *          - If 0xFF, information not available.
 *
 *  \param  upukRetries
 *          - Indicates the number of retries remaining to unblock the UPIN.
 *          - If 0xFF, information not available.
 *
 *  \param  errorState
 *          - Indicates the reason for the card error, and is valid only when
 *            the card state is Error
 *              - 0 - Unknown
 *              - 1 - Power down
 *              - 2 - Poll error
 *              - 3 - No ATR received
 *              - 4 - Volt mismatch
 *              - 5 - Parity error
 *              - 6 - Unknown; possibly removed
 *              - 7 - Card returned technical problems
 *              - 0xFF - Not Available
 *          - Other values are possible and reserved for future use.
 *          - When an unknown value is received, it is to be handled as "Unknown".
 *
 *  \param  numApp
 *          - Indicates the number of applications available on the card.
 *          - The following block is repeated for each application. i.e. AppStatus.
 *          - If zero(0) then no AppStatus information exists.
 *
 *  \param  AppStatus
 *          - See \ref uim_appStatus for more information.
 *
 */
typedef struct
{
    uint8_t      cardState;
    uint8_t      upinState;
    uint8_t      upinRetries;
    uint8_t      upukRetries;
    uint8_t      errorState;
    uint8_t      numApp;
    uim_appStatus AppStatus[UIM_MAX_NO_OF_APPLICATIONS];
} uim_slotInfo;


/**
 *  This structure contains Card Status Information.
 *
 *  \param  indexGwPri
 *          - Index of the primary GW provisioning application.
 *          - The most significant byte indicates the slot (starting from 0),
 *            while the least significant byte indicates the application for
 *            that slot (starting from 0).
 *          - The value 0xFFFF identifies when the session does not exist.
 *
 *  \param  index1xPri
 *          - Index of the primary 1X provisioning application.
 *          - The most significant byte indicates the slot (starting from 0),
 *            while the least significant byte indicates the application for
 *            that slot (starting from 0).
 *          - The value 0xFFFF identifies when the session does not exist.
 *
 *  \param  indexGwSec
 *          - Index of the secondary GW provisioning application.
 *          - The most significant byte indicates the slot (starting from 0),
 *            while the least significant byte indicates the application for
 *            that slot (starting from 0).
 *          - The value 0xFFFF identifies when the session does not exist.
 *
 *  \param  index1xSec
 *          - Index of the secondary GW provisioning application.
 *          - The most significant byte indicates the slot (starting from 0),
 *            while the least significant byte indicates the application for
 *            that slot (starting from 0).
 *          - The value 0xFFFF identifies when the session does not exist.
 *
 *  \param  numSlot
 *          - Indicates the number of slots available on the device.
 *          - The following block is repeated for each slot. i.e. cardState
 *          - If zero(0) then no cardState information exists.
 *
 *  \param  SlotInfo
 *          - See \ref uim_slotInfo for more information.
 *
 */
typedef struct
{
    uint16_t     indexGwPri;
    uint16_t     index1xPri;
    uint16_t     indexGwSec;
    uint16_t     index1xSec;
    uint8_t     numSlot;
    uim_slotInfo SlotInfo[UIM_MAX_NO_OF_SLOTS];
} uim_cardStatus;

/**
 *  This structure contains Hot Swap Status Information.
 *
 *  \param  hotSwapLength
 *          - Number of sets of the following elements. i.e. hot_swap
 *
 *  \param  hotSwap
 *          - Indicates the status of the hot-swap switch.
 *              - 0 - Hot-swap is not supported
 *              - 1 - Hot-swap is supported, but the status of the switch
 *                    is not supported
 *              - 2 - Switch indicates that the card is present
 *              - 3 - Switch indicates that the card is not present
 *
 */
typedef struct
{
    uint8_t hotSwapLength;
    uint8_t hotSwap[UIM_MAX_DESCRIPTION_LENGTH];
} uim_hotSwapStatus;

/**
 *  This structure contains Valid Card Status Information.
 *
 *  @param  validCardLength
 *          - Number of sets of the following elements. i.e. validCard
 *
 *  @param  validCard[MAX_DESCRIPTION_LENGTH]
 *          - Indicates the status of the valid card.
 *              - 0 - Status of the card is unknown
 *              - 1 - Status of the card is valid
 *
 */
typedef struct
{
    uint8_t validCardLength;
    uint8_t validCard[MAX_DESCRIPTION_LENGTH];
} uim_validCardStatus;

/**
 *  This structure contains Sim Busy Status Information.
 *
 *  @param  simBusyLength
 *          - Number of sets of the following elements. i.e. simBusy
 *
 *  @param  simBusy[MAX_DESCRIPTION_LENGTH]
 *          - Indicates the status of the valid card.
 *              - 0 - Sim card is not busy
 *              - 1 - Sim card is busy
 *
 */
typedef struct
{
    uint8_t simBusyLength;
    uint8_t simBusy[MAX_DESCRIPTION_LENGTH];
} uim_simBusyStatus;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Card Status API.
 *
 *  @param  pCardStatus(optional)
 *          - See \ref uim_cardStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHotSwapStatus(optional)
 *          - See \ref uim_hotSwapStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_cardStatus *pCardStatus;
    uim_hotSwapStatus *pHotSwapStatus;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_GetCardStatus_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Card Status API.
 *
 *  @param  pCardStatus(optional)
 *          - See \ref uim_cardStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHotSwapStatus(optional)
 *          - See \ref uim_hotSwapStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pValidCardStatus(optional)
 *          - See \ref uim_validCardStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pSimBusyStatus(optional)
 *          - See \ref uim_simBusyStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_cardStatus *pCardStatus;
    uim_hotSwapStatus *pHotSwapStatus;
    uim_validCardStatus *pValidCardStatus;
    uim_simBusyStatus *pSimBusyStatus;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_GetCardStatusV2_t;


/**
 *  This structure contains the encrypted PIN1 Information.
 *
 *  @param  pin1Len
 *          - Number of sets of the following elements ie encrypted PIN1 value.
 *          - If zero(0), no information follows.
 *
 *  @param  pin1Val
 *          - Encrypted PIN1 value.
 *
 *  @note   This value is returned only when PIN1 is enabled successfully
 *          and the feature is supported.
 *
 */
typedef struct
{
    uint8_t pin1Len;
    uint8_t pin1Val[UIM_MAX_DESCRIPTION_LENGTH];
} uim_encryptedPIN1;

/**
 *  This structure contains the information about the retries remaining.
 *
 *  @param  verifyLeft
 *          - Number of remaining attempts to verify the PIN.
 *          - 0xFF, if unavailable.
 *
 *  @param  unblockLeft
 *          - Number of remaining attempts to unblock the PIN.
 *          - 0xFF, if unavailable.
 *
 *  @note   This value is returned only when the enable/disable operation has
 *          failed.
 *          This information is not sent for a hidden key PIN type.
 *
 */
typedef struct
{
    uint8_t verifyLeft;
    uint8_t unblockLeft;
} uim_remainingRetries;

/**
 *  This structure contains the Session Information.
 *
 *  @param  sessionType
 *          - Indicates the session type.
 *              - 0 - Primary GW provisioning
 *              - 1 - Primary 1X provisioning
 *              - 2 - Secondary GW provisioning
 *              - 3 - Secondary 1X provisioning
 *              - 4 - Non-provisioning on slot 1
 *              - 5 - Non-provisioning on slot 2
 *              - 6 - Card on slot 1
 *              - 7 - Card on slot 2
 *              - 8 - Logical channel on slot 1
 *              - 9 - Logical channel on slot 2
 *
 *  @param  aidLength
 *          - Length of the following elements i.e. Application Identifier.
 *
 *  @param  aid
 *          - Application identifier value or channel ID.
 *          - This value is required for non-provisioning and for logical
 *            channel session types. It is ignored in all other cases.
 *
 */
typedef struct
{
    uint8_t   sessionType;
    uint8_t   aidLength;
    uint8_t   aid[UIM_MAX_DESCRIPTION_LENGTH];
} uim_sessionInformation ;

/**
 *  This structure contains the information about the pin parameters that need
 *  to be verified.
 *
 *  \param  pinID
 *          - Indicates the PIN ID to be verified.
 *              - 1 - PIN1 (also called PIN)
 *              - 2 - PIN2
 *              - 3 - Universal PIN
 *              - 4 - Hidden key
 *
 *  \param  pinLen
 *          - Length of the following elements i.e. pin value.
 *
 *  \param  pinVal[MAX_DESCRIPTION_LENGTH]
 *          - PIN value.
 *          - This value is a sequence of ASCII characters.
 *
 */
typedef struct{
    uint8_t pinID;
    uint8_t pinLen;
    uint8_t pinVal[UIM_MAX_DESCRIPTION_LENGTH];
} uim_verifyUIMPIN;

/**
 *  This structure contains the information about the unblock pin parameters.
 *
 *  @param  pinID
 *          - Indicates the PIN ID to be changed.
 *              - 1 - PIN1 (also called PIN)
 *              - 2 - PIN2
 *              - 3 - Universal PIN
 *
 *  @param  pukLen
 *          - Length of the following elements i.e. puk value.
 *
 *  @param  pukVal[UIM_MAX_DESCRIPTION_LENGTH]
 *          - PIN Unlock Key value.
 *          - This value is a sequence of ASCII characters.
 *
 *  @param  newPINLen
 *          - Length of the following elements i.e. new pin value.
 *
 *  @param  newPINVal[UIM_MAX_DESCRIPTION_LENGTH]
 *          - New PIN value.
 *          - This value is a sequence of ASCII characters.
 *
 */
typedef struct{
    uint8_t pinID;
    uint8_t pukLen;
    uint8_t pukVal[UIM_MAX_DESCRIPTION_LENGTH];
    uint8_t newPINLen;
    uint8_t newPINVal[UIM_MAX_DESCRIPTION_LENGTH];
} uim_unblockUIMPIN;

/**
 *  This structure contains the information about the card result.
 *
 *  @param  sw1
 *          - SW1 received from the card.
 *
 *  @param  sw2
 *          - SW2 received from the card.
 *
 */
typedef struct
{
    uint8_t sw1;
    uint8_t sw2;
} uim_cardResult;

/**
 *  This structure contains the information about the pin protection parameters
 *  that need to be set.
 *
 *  @param  pinID
 *          - Indicates the PIN ID to be enabled or disabled.
 *              - 1 - PIN1 (also called PIN)
 *              - 2 - PIN2
 *              - 3 - Universal PIN
 *              - 4 - Hidden key
 *
 *  @param  pinOperation
 *          - Indicates whether the PIN is enabled or disabled.
 *              - 0 - Disable the PIN
 *              - 1 - Enable the PIN
 *
 *  @param  pinLength
 *          - Length of the following elements i.e. pin value.
 *
 *  @param  pinValue[MAX_DESCRIPTION_LENGTH]
 *          - PIN value.
 *          - This value is a sequence of ASCII characters.
 *
 */
typedef struct
{
    uint8_t pinID;
    uint8_t pinOperation;
    uint8_t pinLength;
    uint8_t pinValue[UIM_MAX_DESCRIPTION_LENGTH];
} uim_setPINProtection;

/**
 *  This structure contains the information about the pin parameters that need
 *  to be verified.
 *
 *  \param  pinID
 *          - Indicates the PIN ID to be changed.
 *              - 1 - PIN1 (also called PIN)
 *              - 2 - PIN2
 *              - 3 - Universal PIN
 *              - 4 - Hidden key
 *
 *  \param  oldPINLen
 *          - Length of the following elements i.e. old pin value.
 *
 *  \param  oldPINVal[MAX_DESCRIPTION_LENGTH]
 *          - Old PIN value.
 *          - This value is a sequence of ASCII characters.
 *
 *  \param  pinLen
 *          - Length of the following elements i.e. new pin value.
 *
 *  \param  pinVal[MAX_DESCRIPTION_LENGTH]
 *          - New PIN value.
 *          - This value is a sequence of ASCII characters.
 *
 */
typedef struct{
    uint8_t pinID;
    uint8_t oldPINLen;
    uint8_t oldPINVal[UIM_MAX_DESCRIPTION_LENGTH];
    uint8_t pinLen;
    uint8_t pinVal[UIM_MAX_DESCRIPTION_LENGTH];
} uim_changeUIMPIN;

/**
 *  This structure contains paramaters for file Information
 *
 *  @param  fileID
 *          - This is Identifier to SIM files;
 *            e.g. in UIM "6F07" is Identifier of IMSI File
 *
 *  @param  pathLen
 *          - Length of file Path
 *
 *  @param  path
 *          - Path value. This value must be the complete
 *            path of the file, which is a sequence block of 2
 *            bytes (e.g., 0x3F00 0x7FFF) is for LTE
 *            (0x3F00 ,0x7F20) is for GSM.
 */
typedef struct {
    uint16_t  fileID;
    uint8_t  pathLen;
    uint16_t  path[UIM_UINT8_MAX_STRING_SZ];
} uim_fileInfo;


/**
 *  This structure contains the Session Information.
 *
 *  @param  sessionType
 *          - Indicates the session type.
 *              - 0 - Primary GW provisioning
 *              - 1 - Primary 1X provisioning
 *              - 2 - Secondary GW provisioning
 *              - 3 - Secondary 1X provisioning
 *              - 4 - Non-provisioning on slot 1
 *              - 5 - Non-provisioning on slot 2
 *              - 6 - Card on slot 1
 *              - 7 - Card on slot 2
 *              - 8 - Logical channel on slot 1
 *              - 9 - Logical channel on slot 2
 *
 *  @param  aidLength
 *          - Length of the following elements i.e. Application Identifier.
 *
 *  @param  aid
 *          - Application identifier value or channel ID.
 *          - This value is required for non-provisioning and for logical
 *            channel session types. It is ignored in all other cases.
 *
 */
typedef struct
{
    uint8_t   sessionType;
    uint8_t   aidLength;
    uint8_t   aid[255];
} uim_UIMSessionInformation ;

/**
 *  This structure contains the information for read operation.
 *
 *  @param  offset
 *          - Offset for the read operation.
 *
 *  @param  length
 *          - Length of the content to be read.
 *          - The value 0 is used to read the complete file.
 *
 */
typedef struct
{
    uint16_t offset;
    uint16_t length;
} uim_readTransparentInfo;


/**
 *  This structure contains the information for write operation.
 *
 *  @param  contentLen
 *          - Number of sets of content.
 *
 *  @param  content[255]
 *          - Read content.
 *          - The content is the sequence of bytes as read from the card.
 */
typedef struct
{
    uint16_t contentLen;
    uint8_t content[UIM_UINT8_MAX_STRING_SZ];
} uim_readResult;


/**
 * Get Card Status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_GetCardStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Card Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_GetCardStatus(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_GetCardStatus_t *pOutput
);

/**
 * Get Card Status unpack V2
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_GetCardStatusV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_GetCardStatusV2_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a verify PIN API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_sessionInformation for more information.
 *
 *  @param  verifyPIN
 *          - See \ref uim_verifyUIMPIN for more information.
 *
 *  @param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *
 *  @param  pKeyReferenceID(optional)
 *          - Indicates the PIN key reference ID.
 *          - Indicates the PIN key reference ID. Valid values are from 1 to 8,
 *            respectively, for application 1 to application 8.
 *          - This TLV is used only for PIN1 and PIN2 and is ignored in all
 *            other cases.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Valid Values
 *            - 0 - Result of operation in response. Indication will not be generated by the modem
 *            - Any other positive number - Result of operation in indication.
 *              Indication will have same token value set by this function
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct{
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uint8_t *pKeyReferenceID;
    uim_sessionInformation sessionInfo;
    uim_verifyUIMPIN      verifyPIN;
    uint16_t Tlvresult;
}pack_uim_VerifyPin_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a set of PIN related API's.
 *
 *  \param  pRemainingRetries(optional)
 *          - See \ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result is
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_remainingRetries *pRemainingRetries;
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_VerifyPin_t;


/**
 * Verify Pin Status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_VerifyPin(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_VerifyPin_t *reqArg
);

/**
 * Verify Pin unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_VerifyPin(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_VerifyPin_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Unblock PIN API.
 *
 *  @param  EncryptedPIN1
 *          - See \ref uim_encryptedPIN1 for more information.
 *
 *  @param  sessionInfo
 *          - See \ref uim_sessionInformation for more information.
 *
 *  @param  pinProtection
 *          - See \ref uim_unblockUIMPIN for more information.
 *
 *  @param  pKeyReferenceID(optional)
 *          - Indicates the PIN key reference ID.
 *          - Indicates the PIN key reference ID. Valid values are from 1 to 8,
 *            respectively, for application 1 to application 8.
 *          - This TLV is used only for PIN1 and PIN2 and is ignored in all
 *            other cases.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Valid Values
 *            - 0 - Result of operation in response. Indication will not be generated by the modem
 *            - Any other positive number - Result of operation in indication.
 *              Indication will have same token value set by this function
 *
 */
typedef struct{
    uim_encryptedPIN1    EncryptedPIN1;
    uint32_t *pIndicationToken;
    uint8_t *pKeyReferenceID;
    uim_sessionInformation sessionInfo;
    uim_unblockUIMPIN pinProtection;
    uint16_t Tlvresult;
}pack_uim_UnblockPin_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a set of PIN related API's.
 *
 *  @param  pRemainingRetries(optional)
 *          - See \ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result is
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_remainingRetries *pRemainingRetries;
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_UnblockPin_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a set of PIN related API's.
 *
 *  @param  pRemainingRetries(optional)
 *          - See \ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result is
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pCardResult(optional)
 *          - See \ref uim_cardResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_remainingRetries *pRemainingRetries;
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uim_cardResult       *pCardResult;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_UnblockPinV2_t;


/**
 * Unblock Pin pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_UnblockPin(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_UnblockPin_t *reqArg
);

/**
 * Unblock Pin unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_UnblockPin(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_UnblockPin_t *pOutput
);

/**
 * Unblock Pin unpack V2
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_UnblockPinV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_UnblockPinV2_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a set pin protection API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_sessionInformation for more information.
 *
 *  @param  pinProtection
 *          - See \ref uim_setPINProtection for more information.
 *
 *  @param  pKeyReferenceID(optional)
 *          - Indicates the PIN key reference ID.
 *          - Indicates the PIN key reference ID. Valid values are from 1 to 8,
 *            respectively, for application 1 to application 8.
 *          - This TLV is used only for PIN1 and PIN2 and is ignored in all
 *            other cases.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Valid Values
 *            - 0 - Result of operation in response. Indication will not be generated by the modem
 *            - Any other positive number - Result of operation in indication.
 *              Indication will have same token value set by this function
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct{
    uim_encryptedPIN1    EncryptedPIN1;
    uint32_t *pIndicationToken;
    uint8_t *pKeyReferenceID;
    uim_sessionInformation sessionInfo;
    uim_setPINProtection      pinProtection;
    uint16_t Tlvresult;
}pack_uim_SetPinProtection_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a set of PIN related API's.
 *
 *  @param  pRemainingRetries(optional)
 *          - See \ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result is
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_remainingRetries *pRemainingRetries;
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_SetPinProtection_t;


/**
 * Set Pin Protection pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SetPinProtection(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SetPinProtection_t *reqArg
);

/**
 * Set Pin Protection unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SetPinProtection(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SetPinProtection_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Change PIN API.
 *
 *  \param  sessionInfo
 *          - See \ref uim_sessionInformation for more information.
 *
 *  \param  changePIN
 *          - See \ref uim_changeUIMPIN for more information.
 *
 *  \param  pKeyReferenceID(optional)
 *          - Indicates the PIN key reference ID.
 *          - Indicates the PIN key reference ID. Valid values are from 1 to 8,
 *            respectively, for application 1 to application 8.
 *          - This TLV is used only for PIN1 and PIN2 and is ignored in all
 *            other cases.
 *
 *  \param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Valid Values
 *            - 0 - Result of operation in response. Indication will not be generated by the modem
 *            - Any other positive number - Result of operation in indication.
 *              Indication will have same token value set by this function
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct{
    uim_encryptedPIN1    EncryptedPIN1;
    uint32_t *pIndicationToken;
    uint8_t *pKeyReferenceID;
    uim_sessionInformation sessionInfo;
    uim_changeUIMPIN      changePIN;
    uint16_t Tlvresult;
}pack_uim_ChangePin_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a set of PIN related API's.
 *
 *  \param  pRemainingRetries(optional)
 *          - See \ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pEncryptedPIN1(optional)
 *          - See \ref uim_encryptedPIN1 for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result is
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_remainingRetries *pRemainingRetries;
    uim_encryptedPIN1    *pEncryptedPIN1;
    uint32_t *pIndicationToken;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_ChangePin_t;


/**
 * Change Pin pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_ChangePin(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_ChangePin_t *reqArg
);

/**
 * Change Pin unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_ChangePin(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_ChangePin_t *pOutput
);


/**
 *  This structure contains information of the request parameters associated
 *  with a Read Transparent API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_sessionInformation for more information.
 *
 *  @param  fileIndex
 *          - See \ref uim_fileInfo for more information.
 *
 *  @param  readTransparent
 *          - See \ref uim_readTransparentInfo for more information.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Valid Values
 *            - 0 - Result of operation in response. Indication will not be generated by the modem
 *            - Any other positive number - Result of operation in indication.
 *              Indication will have same token value set by this function
 *
 *  @param  pEncryptData(optional)
 *          - Encrypt Data.
 *          - Indicates whether the data read from the card is to be encrypted.
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct{
    uim_sessionInformation sessionInfo;
    uim_fileInfo              fileIndex;
    uim_readTransparentInfo   readTransparent;
    uint32_t                 *pIndicationToken;
    uint8_t                  *pEncryptData;
    uint16_t Tlvresult;
}pack_uim_ReadTransparent_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Read Transparent API.
 *
 *  \param  pCardResult
 *          - See \ref uim_cardResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pReadResult
 *          - See \ref uim_readResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pEncryptedData(optional)
 *          - Encrypted Data.
 *          - Indicates whether the data from the card passed in read_result is encrypted.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_cardResult  *pCardResult;
    uim_readResult  *pReadResult;
    uint32_t        *pIndicationToken;
    uint8_t         *pEncryptedData;
    uint16_t        Tlvresult;
    swi_uint256_t   ParamPresenceMask;
}unpack_uim_ReadTransparent_t;


/**
 * SLQS ReadTransparent pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_ReadTransparent(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_ReadTransparent_t *reqArg
);

/**
 * SLQS ReadTransparent unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_ReadTransparent(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_ReadTransparent_t *pOutput
);

/**
 * This structure contains pack event register parameter.
 * @param eventMask - bit 1 - card status
 *                  - bit 4 - physical slot status
 */
typedef struct{
    uint32_t eventMask;
}pack_uim_SLQSUIMEventRegister_t;

/**
 * UIM Status Change callback enable pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMEventRegister (
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMEventRegister_t *reqArg
);

/**
 * This structure contains unpack event register parameter.
 * @param eventMask - bit 0 - card status
 *                  - bit 1 - SAP connection
 *                  - bit 4 - physical slot status
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *        - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t eventMask;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_SLQSUIMEventRegister_t;

/**
 * UIM Status Change callback enable unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMEventRegister(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMEventRegister_t *pOutput
);

/**
 *  This structure contains Application Status Information loaded on the card.
 *
 *  \param  appType
 *          - Indicates the type of the application.
 *              - 0 - Unknown
 *              - 1 - SIM card
 *              - 2 - USIM application
 *              - 3 - RUIM card
 *              - 4 - CSIM application
 *              - 5 - ISIM application
 *          - Other values are reserved for the future and are
 *            to be handled as "Unknown".
 *
 *  \param  appState
 *          - Indicates the state of the application.
 *              - 0 - Unknown
 *              - 1 - Detected
 *              - 2 - PIN1 or UPIN is required
 *              - 3 - PUK1 or PUK for UPIN is required
 *              - 4 - Personalization state must be checked
 *              - 5 - PIN1 is blocked
 *              - 6 - Illegal
 *              - 7 - Ready
 *
 *  \param  persoState
 *          - Indicates the state of the personalization for the application.
 *              - 0 - Unknown
 *              - 1 - Personalization operation is in progress
 *              - 2 - Ready
 *              - 3 - Personalization code is required
 *              - 4 - PUK for personalization code is required
 *              - 5 - Permanently blocked
 *
 *  \param  persoFeature
 *          - Indicates the personalization feature.
 *          - This applies only when a personalization code is required to
 *            deactivate or unblock personalization.
 *              - 0 - GW network personalization
 *              - 1 - GW network subset personalization
 *              - 2 - GW service provider personalization
 *              - 3 - GW corporate personalization
 *              - 4 - GW UIM personalization
 *              - 5 - 1X network type 1 personalization
 *              - 6 - 1X network type 2 personalization
 *              - 7 - 1X HRPD personalization
 *              - 8 - 1X service provider personalization
 *              - 9 - 1X corporate personalization
 *              - 10 - 1X RUIM personalization
 *              - 11 - Unknown
 *
 *  \param  persoRetries
 *          - Indicates the number of retries remaining to disable the
 *            personalization.
 *
 *  \param  persoUnblockRetries
 *          - Indicates the number of retries remaining to unblock the
 *            personalization.
 *
 *  \param  aidLength
 *          - Number of sets of the following elements. i.e. aidVal
 *          - If zero(0) then no aidVal information exists.
 *
 *  \param  aidVal[MAX_DESCRIPTION_LENGTH]
 *          - Application identifier value.
 *
 *  \param  univPin
 *          - Indicates whether UPIN replaces PIN1.
 *              - 0 - PIN1 is used
 *              - 1 - UPIN replaces PIN1
 *
 *  \param  pin1State
 *          - Indicates the state of PIN1.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *
 *  \param  pin1Retries
 *          - Indicates the number of retries remaining to verify PIN1.
 *
 *  \param  puk1Retries
 *          - Indicates the number of retries remaining to unblock PIN1.
 *
 *  \param  pin2State
 *          - Indicates the state of PIN2.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *
 *  \param  pin2Retries
 *          - Indicates the number of retries remaining to verify PIN2.
 *
 *  \param  puk2Retries
 *          - Indicates the number of retries remaining to unblock PIN2.
 *
 */
typedef struct
{
    uint8_t appType;
    uint8_t appState;
    uint8_t persoState;
    uint8_t persoFeature;
    uint8_t persoRetries;
    uint8_t persoUnblockRetries;
    uint8_t aidLength;
    uint8_t aidVal[MAX_DESCRIPTION_LENGTH];
    uint8_t univPin;
    uint8_t pin1State;
    uint8_t pin1Retries;
    uint8_t puk1Retries;
    uint8_t pin2State;
    uint8_t pin2Retries;
    uint8_t puk2Retries;
} appStats;

/**
 *  This structure contains information about the SLOTS present.
 *
 *  \param  cardState
 *          - Indicates the state of the card for each slot.
 *              - 0 - Absent
 *              - 1 - Present
 *              - 2 - Error
 *
 *  \param  upinState
 *          - Indicates the state of UPIN.
 *              - 0 - Unknown
 *              - 1 - Enabled and not verified
 *              - 2 - Enabled and verified
 *              - 3 - Disabled
 *              - 4 - Blocked
 *              - 5 - Permanently blocked
 *              - 0xFF - Not Available
 *
 *  \param  upinRetries
 *          - Indicates the number of retries remaining to verify the UPIN.
 *          - If 0xFF, information not available.
 *
 *  \param  upukRetries
 *          - Indicates the number of retries remaining to unblock the UPIN.
 *          - If 0xFF, information not available.
 *
 *  \param  errorState
 *          - Indicates the reason for the card error, and is valid only when
 *            the card state is Error
 *              - 0 - Unknown
 *              - 1 - Power down
 *              - 2 - Poll error
 *              - 3 - No ATR received
 *              - 4 - Volt mismatch
 *              - 5 - Parity error
 *              - 6 - Unknown; possibly removed
 *              - 7 - Card returned technical problems
 *              - 0xFF - Not Available
 *          - Other values are possible and reserved for future use.
 *          - When an unknown value is received, it is to be handled as "Unknown".
 *
 *  \param  numApp
 *          - Indicates the number of applications available on the card.
 *          - The following block is repeated for each application. i.e. AppStatus.
 *          - If zero(0) then no AppStatus information exists.
 *
 *  \param  AppStatus[MAX_NO_OF_APPLICATIONS]
 *          - See \ref appStats for more information.
 *
 */
typedef struct
{
    uint8_t      cardState;
    uint8_t      upinState;
    uint8_t      upinRetries;
    uint8_t      upukRetries;
    uint8_t      errorState;
    uint8_t      numApp;
    appStats    AppStatus[MAX_NO_OF_APPLICATIONS];
} slotInf;

/**
 * This structure contains information about Status change callback.
 *
 *  @param  pCardStatus Card Status
 *          - See \ref uim_cardStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_cardStatus *pCardStatus;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t;

/**
 * UIM Status Change indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 * @note use pack_uim_SLQSUIMEventRegister to subscribe 
 */
int unpack_uim_SLQSUIMSetStatusChangeCallBack_ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t *pOutput
);

/**
 * Pack get slots status.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMGetSlotsStatus (
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status API.
 *
 *  \param  uPhyCardStatus
 *          - State of the card in the Pyhsical Slot Status.
 *              - 0x00 - Unknown.
 *              - 0x01 - Absent.
 *              - 0x02 - Present.
 *
 *  \param  uPhySlotStatus
 *          - State of the Physical Slot status.
 *              - 0x00 Inactive.
 *              - 0x01 Activate.
 *
 *  \param  bLogicalSlot
 *          - Logical Slot associated with this physical slot. THis is valid if the physical slot is active.
 *              - 1 - Slot 1.
 *              - 2 - Slot 2.
 *              - 3 - Slot 3.
 *              - 4 - Slot 4.
 *              - 5 - Slot 5.
 *
 *  \param  bLogicalSlot
 *          - Number of sets the sets of ICCCID
 *
 *  \param  bICCID[MAX_ICCID_LENGTH]
 *          - Contains the ICCID of the card in the physical slot.
 *
 */
typedef struct
{
    uint32_t   uPhyCardStatus;
    uint32_t   uPhySlotStatus;
    uint8_t    bLogicalSlot;
    uint8_t    bICCIDLength;
    uint8_t    bICCID[MAX_ICCID_LENGTH];
}slot_t;

/**
 *  This structure contains slots status.
 *  \param  uimSlotStatus
 *          - See \ref slot_t
 */

typedef struct
{
    slot_t uimSlotStatus[MAX_SLOTS_STATUS];
}slots_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status API.
 *
 *  \param  cardProtocol
 *          - Protocol of the card.
 *          - Valid values:
 *            - 0x00 - Unknown
 *            - 0x01 - ICC Protocol
 *            - 0x02 - UICC Protocol
 *
 *  \param  numApp
 *          - Number of valid applications present in the EF-DIR of card.
 *
 *  \param  atrValueLen
 *          - Number of sets of the atrValue
 *
 *  \param  atrValue
 *          - Raw value of the ATR sent by the card during the initialization.
 *
 *  \param  iseUICC
 *          - Indicates whether the card is an eUICC card based on the ATR.
 *          -  Values
 *            - 0 - Not an eUICC card
 *            - 1 - eUICC card
 *
 */

typedef struct
{
    uint32_t   cardProtocol;
    uint8_t    numApp;
    uint8_t    atrValueLen;
    uint8_t    atrValue[MAX_ATR_LENGTH];
    uint8_t    iseUICC;
}uim_physlotInfo;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status API.
 *
 *  \param  uimSlotInfo[MAX_PHY_SLOTS_INFO]
 *          - Contain all slots Information.
 */

typedef struct
{
    uim_physlotInfo uimSlotInfo[MAX_PHY_SLOTS_INFO];
}uim_physlotsInfo;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status API.
 *
 *  \param  pNumberOfPhySlot
 *          - Number of sets of the Slot Status.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pUimSlotsStatus
 *          - Slots Status  See \ref slots_t for more information..
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t         *pNumberOfPhySlot;
    slots_t         *pUimSlotsStatus;
    swi_uint256_t   ParamPresenceMask;
}unpack_uim_SLQSUIMGetSlotsStatus_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status TLV.
 *
 *  \param  NumberOfPhySlot
 *          - Number of sets of the Physical Slot Status.
 *
 *  \param  uimSlotStatus
 *          - Slots Status See \ref UIMSlotStatus for more information.
 *
 *  \param  TlvPresent
 *          - Physical slot status TLV present.
 */
typedef struct
{
    uint8_t             NumberOfPhySlot;
    slot_t              uimSlotStatus[MAX_SLOTS_STATUS];
    uint8_t             TlvPresent;
} uim_GetSlotsStatusTlv;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Information TLV.
 *
 *  \param  NumberOfPhySlotInfo
 *          - Number of sets of the Slot Information.
 *
 *  \param  uimSlotInfo
 *          - Slots information See \ref UIMSlotInfo for more information.
 *
 *  \param  TlvPresent
 *          - Physical slot information TLV present.
 */
typedef struct
{
    uint8_t            NumberOfPhySlotInfo;
    uim_physlotInfo    uimSlotInfo[MAX_PHY_SLOTS_INFO];
    uint8_t            TlvPresent;
} uim_GetSlotsInfoTlv;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get Slots Status API.
 *
 *  \param  pNumberOfPhySlot
 *          - Number of sets of the Slot Status.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pUimSlotsStatus
 *          - Slots Status  See \ref slots_t for more information..
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pNumberOfPhySlotInfo
 *          - Number of Physical slot information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pUimSlotsStatus
 *          - Slots Status  See \ref uim_physlotsInfo for more information..
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uim_GetSlotsStatusTlv       *pGetSlotsStatusTlv;
    uim_GetSlotsInfoTlv         *pGetSlotsInfoTlv;
    swi_uint256_t               ParamPresenceMask;
}unpack_uim_SLQSUIMGetSlotsStatusV2_t;

/**
 * get slot status unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMGetSlotsStatus(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMGetSlotsStatus_t *pOutput
);

/**
 * get slot status unpack V2
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMGetSlotsStatusV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMGetSlotsStatusV2_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Switch Slot.
 *
 *  \param  bLogicalSlot
 *          - Indicates the slot to be used.
 *              - 1 - Slot 1
 *              - 2 - Slot 2
 *              - 3 - Slot 3
 *              - 4 - Slot 4
 *              - 5 - Slot 5
 *
 *  \param  ulPhysicalSlot
 *              - 1 - Slot 1
 *              - 2 - Slot 2
 *              - 3 - Slot 3
 *              - 4 - Slot 4
 *              - 5 - Slot 5
 *
 */
typedef struct{
    uint8_t bLogicalSlot;
    uint32_t ulPhysicalSlot;
}pack_uim_SLQSUIMSwitchSlot_t;

/**
 * switch slot pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMSwitchSlot (
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMSwitchSlot_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMSwitchSlot_t;

/**
 * switch slot unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMSwitchSlot(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMSwitchSlot_t *pOutput
);

/**
 *  Structure consist of card status params
 *
 *  \param  slotsstatusChange
 *          - See \ref slot_t for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  bNumberOfPhySlots
 *          - Number of Physical Slot(s)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    slots_t slotsstatusChange;
    uint8_t bNumberOfPhySlots;
    swi_uint256_t  ParamPresenceMask;
}unpack_uim_SetUimSlotStatusChangeCallback_ind_t;

/**
 * UIM Slot Status Change indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 * @note use pack_uim_SLQSUIMEventRegister to subscribe 
 */
int unpack_uim_SetUimSlotStatusChangeCallback_ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SetUimSlotStatusChangeCallback_ind_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Power Down.
 *
 *  @param  slot
 *          - Indicates the slot to be used.
 *              - 1 - Slot 1
 *              - 2 - Slot 2
 *
 *  @param  pIgnoreHotSwapSwitch(optional)
 *          - Hot-swap switch status.
 *              - 0 - Checks the hot-swap switch status
 *              - 1 - Ignores the hot-swap switch status
 *
 */
typedef struct{
    uint8_t slot;
    uint8_t *pIgnoreHotSwapSwitch;
}pack_uim_SLQSUIMPowerUp_t;

/**
 * Powers up the card  pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMPowerUp (
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMPowerUp_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMPowerUp_t;

/**
 * Powers up the card unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMPowerUp(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMPowerUp_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Power Down.
 *
 *  @param  slot
 *          - Indicates the slot to be used.
 *              - 1 - Slot 1
 *              - 2 - Slot 2
 *
 */
typedef struct{
    uint8_t slot;
}pack_uim_SLQSUIMPowerDown_t;

/**
 * Powers down the card  pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMPowerDown (
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMPowerDown_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMPowerDown_t;

/**
 * Powers down the card unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMPowerDown(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMPowerDown_t *pOutput
);

/**
 * Resets the issuing control points state kept by the service pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMReset(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen
);

typedef unpack_result_t  unpack_uim_SLQSUIMReset_t;

/**
 * Resets the issuing control points state kept by the service unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMReset(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMReset_t *pOutput
);

/**
 *  This structure contains Parameters of the Session Information
 *
 *  @param  sessionInfo
 *          - Session Information
 *          - See @ref uim_sessionInformation for more information
 *
 *  @param  OKtoRefresh
 *          - Indicates whether a refresh is OK. Valid values:
 *            - 0 - Not OK to refresh
 *            - 1 - OK to refresh
 */
typedef struct
{
    uim_sessionInformation sessionInfo;
    uint8_t                OKtoRefresh;
} pack_uim_SLQSUIMRefreshOK_t;

/**
 * Enables the client to indicate whether it is OK to start the
 * Refresh procedure pack.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMRefreshOK (
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMRefreshOK_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMRefreshOK_t;

/**
 * Enables the client to indicate whether it is OK to start the
 * Refresh procedure unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMRefreshOK(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMRefreshOK_t *pOutput
);

/**
 *  This structure contains parameters of refresh Information
 *
 *  @param  registerFlag
 *          - Flag that indicates whether to register or
 *            deregister for refresh indications. Valid values:
 *            - 0 - Deregister
 *            - 1 - Register
 *
 *  @param  voteForInit
 *          - Flag that indicates whether to vote for the init
 *            when there is a refresh. Valid values:
 *            - 0 - Client does not vote for initialization
 *            - 1 - Client votes for initialization
 *
 *  @param  numFiles
 *          - Number of sets of the following elements:
 *            - file_id
 *            - path_len
 *            - path
 *
 *  @param  arrfileInfo
 *          - Array of file Information structure.
 *          - See @ref uim_fileInfo for more information
 */
typedef struct
{
    uint8_t     registerFlag;
    uint8_t     voteForInit;
    uint16_t    numFiles;
    uim_fileInfo arrfileInfo[MAX_DESCRIPTION_LENGTH];
} uim_registerRefresh;

/**
 *  This structure contains information of the request parameters associated
 *  with a Refresh Register.
 *
 *  @param  sessionInfo
 *          - Session Information params
 *          - See @ref uim_sessionInformation for more information
 *
 *  @param  regRefresh
 *          - Register Refresh parameters
 *          - See @ref uim_registerRefresh for more information
 *
 */
typedef struct
{
    uim_sessionInformation sessionInfo;
    uim_registerRefresh    regRefresh;
} pack_uim_SLQSUIMRefreshRegister_t;

/**
 * Registers for file change notifications triggered by the card pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMRefreshRegister(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMRefreshRegister_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMRefreshRegister_t;

/**
 * Registers for file change notifications triggered by the card unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMRefreshRegister(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMRefreshRegister_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Refresh Complete event.
 *
 *  @param  sessionInfo(Mandatory)
 *          - See @ref uim_sessionInformation for more information.
 *
 *  @param  refreshComplete(Mandatory)
 *          - Indicates whether the refresh was successful.
 *            Valid values:
 *            - 0 - Refresh was not completed successfully
 *            - 1 - Refresh was completed successfully
 *
 */
typedef struct
{
    uim_sessionInformation  sessionInfo;
    uint8_t                 refreshComplete;
} pack_uim_SLQSUIMRefreshComplete_t;

/**
 * Enables the terminal response to be sent to the card
 * When the client finished Refresh procedure pack
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMRefreshComplete(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMRefreshComplete_t *reqArg
);

typedef unpack_result_t  unpack_uim_SLQSUIMRefreshComplete_t;

/**
 * Enables the terminal response to be sent to the card
 * When the client finished Refresh procedure unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMRefreshComplete(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMRefreshComplete_t *pOutput
);

/**
 *  This structure contains information of parameters associated with
 *  the Refresh Event.
 *
 *  @param  stage
 *          - Indicates the stage of the Refresh procedure.
 *            - 0 - Waiting for OK to refresh
 *            - 1 - Refresh started
 *            - 2 - Refresh ended successfully
 *            - 3 - Refresh failed
 *
 *  @param  mode
 *          - Indicates the Refresh mode.
 *            - 0 - Reset
 *            - 1 - Init
 *            - 2 - Init and FCN
 *            - 3 - FCN
 *            - 4 - Init and Full FCN
 *            - 5 - Application reset
 *            - 6 - 3G session reset
 *
 *  @param  sessionType
 *          - Indicates the session type.
 *            - 0 - Primary GW provisioning
 *            - 1 - Primary 1X provisioning
 *            - 2 - Secondary GW provisioning
 *            - 3 - Secondary 1X provisioning
 *            - 4 - Nonprovisioning on slot 1
 *            - 5 - Nonprovisioning on slot 2
 *            - 6 - Card on slot 1
 *            - 7 - Card on slot 2
 *            - 8 - Logical channel on slot 1
 *            - 9 - Logical channel on slot 2
 *
 *  @param  aidLength
 *          - Number of sets of the following elements
 *            - Application Identifier
 *
 *  @param  aid
 *          - Application identifier value or channel ID. This
 *            value is required for non-provisioning and for
 *            logical channel session types. It is ignored in all
 *            other cases
 *
 *  @param  numFiles
 *          - Number of sets of the following elements:
 *            - file_id
 *            - path_len
 *            - path
 *
 *  @param  arrfileInfo
 *          - Array of file Information struct
 */
typedef struct
{
    uint8_t      stage;
    uint8_t      mode;
    uint8_t      sessionType;
    uint8_t      aidLength;
    uint8_t      aid[MAX_DESCRIPTION_LENGTH];
    uint16_t     numOfFiles;
    uim_fileInfo arrfileInfo[MAX_DESCRIPTION_LENGTH];
} uim_refreshevent;

/**
 *  This structure contains information of the request parameters associated
 *  with a Refresh Get Last Event.
 *
 *  @param  sessionInfo(Mandatory)
 *          - See @ref uim_sessionInformation for more information.
 *
 */
typedef struct
{
    uim_sessionInformation sessionInfo;
} pack_uim_SLQSUIMRefreshGetLastEvent_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Refresh Get Last Event.
 *
 *  @param  refreshEvent(Optional)
 *          - See @ref uim_refreshevent for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uim_refreshevent *pRefreshEvent;
    swi_uint256_t  ParamPresenceMask;
} unpack_uim_SLQSUIMRefreshGetLastEvent_t;

/**
 * Retrieve the last refresh event pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
  *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMRefreshGetLastEvent(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMRefreshGetLastEvent_t *reqArg
);

/**
 * Retrieve the last refresh event unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMRefreshGetLastEvent(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMRefreshGetLastEvent_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Get File Attributes
 *
 *  @param  sessionInfo
 *          - See @ref uim_sessionInformation for more information.
 *
 *  @param  fileIndex
 *          - See @ref uim_fileInfo for more information.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_sessionInformation sessionInfo;
    uim_fileInfo           fileIndex;
    uint32_t               *pIndicationToken;
} pack_uim_SLQSUIMGetFileAttributes_t;

/**
 *  This structure contains the information about the File Attributes.
 *
 *  @param  fileSize
 *          - Indicates the size of the file.
 *
 *  @param  fileID
 *          - Indicates the ID of the file.
 *
 *  @param  fileType
 *          - Indicates the type of the file.
 *              - 0 - Transparent
 *              - 1 - Cyclic
 *              - 2 - Linear fixed
 *              - 3 - Dedicated file
 *              - 4 - Master file
 *
 *  @param  recordSize
 *          - Indicates the size of the records.
 *          - Only for cyclic and linear fixed files
 *
 *  @param  recordCount
 *          - Indicates the total no. of the records.
 *          - Only for linear fixed files
 *
 *  @param  secRead
 *          - Read security attributes.
 *              - 0 - Always
 *              - 1 - Never
 *              - 2 - AND condition
 *              - 3 - OR condition
 *              - 4 - Single condition
 *
 *  @param  secReadMask
 *          - Mask with read security attributes.
 *          - This field is valid only when required by security attributes.
 *              - Bit 0 - PIN1
 *              - Bit 1 - PIN2
 *              - Bit 2 - UPIN
 *              - Bit 3 - ADM
 *
 *  @param  secWrite
 *          - Write security attributes.
 *              - 0 - Always
 *              - 1 - Never
 *              - 2 - AND condition
 *              - 3 - OR condition
 *              - 4 - Single condition
 *
 *  @param  secWriteMask
 *          - Mask with write security attributes.
 *          - This field is valid only when required by security attributes.
 *              - Bit 0 - PIN1
 *              - Bit 1 - PIN2
 *              - Bit 2 - UPIN
 *              - Bit 3 - ADM
 *
 *  @param  secIncrease
 *          - Increase security attributes.
 *              - 0 - Always
 *              - 1 - Never
 *              - 2 - AND condition
 *              - 3 - OR condition
 *              - 4 - Single condition
 *
 *  @param  secIncreaseMask
 *          - Mask with increase security attributes.
 *          - This field is valid only when required by security attributes.
 *              - Bit 0 - PIN1
 *              - Bit 1 - PIN2
 *              - Bit 2 - UPIN
 *              - Bit 3 - ADM
 *
 *  @param  secDeactivate
 *          - Deactivate security attributes.
 *              - 0 - Always
 *              - 1 - Never
 *              - 2 - AND condition
 *              - 3 - OR condition
 *              - 4 - Single condition
 *
 *  @param  secDeactivateMask
 *          - Mask with deactivate security attributes.
 *          - This field is valid only when required by security attributes.
 *              - Bit 0 - PIN1
 *              - Bit 1 - PIN2
 *              - Bit 2 - UPIN
 *              - Bit 3 - ADM
 *
  *  @param  secActivate
 *          - Activate security attributes.
 *              - 0 - Always
 *              - 1 - Never
 *              - 2 - AND condition
 *              - 3 - OR condition
 *              - 4 - Single condition
 *
 *  @param  secActivateMask
 *          - Mask with activate security attributes.
 *          - This field is valid only when required by security attributes.
 *              - Bit 0 - PIN1
 *              - Bit 1 - PIN2
 *              - Bit 2 - UPIN
 *              - Bit 3 - ADM
 *
 *  @param  rawLen
 *          - Length of the following elements i.e. raw value.
 *
 *  @param  rawValue[MAX_DESCRIPTION_LENGTH]
 *          - Raw value of file attributes.
 *
 */
typedef struct
{
    uint16_t  fileSize;
    uint16_t  fileID;
    uint8_t   fileType;
    uint16_t  recordSize;
    uint16_t  recordCount;
    uint8_t   secRead;
    uint16_t  secReadMask;
    uint8_t   secWrite;
    uint16_t  secWriteMask;
    uint8_t   secIncrease;
    uint16_t  secIncreaseMask;
    uint8_t   secDeactivate;
    uint16_t  secDeactivateMask;
    uint8_t   secActivate;
    uint16_t  secActivateMask;
    uint16_t  rawLen;
    uint8_t   rawValue[MAX_DESCRIPTION_LENGTH];
} uim_fileAttributes;

/**
 *  This structure contains information of the response parameters associated
 *  with a Get File Attributes
 *
 *  @param  pCardResult(optional)
 *          - See @ref uim_cardResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pFileAttributes(optional)
 *          - See @ref uim_fileAttributes for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uim_cardResult     *pCardResult;
    uim_fileAttributes *pFileAttributes;
    uint32_t           *pIndicationToken;
    swi_uint256_t      ParamPresenceMask;
} unpack_uim_SLQSUIMGetFileAttributes_t;

/**
 * Retrieves the file attributes for any EF(Elementary File) or DF(Dedicated File) in the card and provides access by the path pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMGetFileAttributes(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMGetFileAttributes_t *reqArg
);

/**
 * Retrieves the file attributes for any EF(Elementary File) or DF(Dedicated File) in the card and provides access by the path unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMGetFileAttributes(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMGetFileAttributes_t *pOutput
);

/**
 *  This structure contains the Depersonalization Information.
 *
 *  @param  feature
 *          - Indicates the personalization feature to de-activate or unblock.
 *              - 0 - GW network personalization
 *              - 1 - GW network subset personalization
 *              - 2 - GW service provider personalization
 *              - 3 - GW corporate personalization
 *              - 4 - GW UIM personalization
 *              - 5 - 1X network type 1 personalization
 *              - 6 - 1X network type 2 personalization
 *              - 7 - 1X HRPD personalization
 *              - 8 - 1X service provider personalization
 *              - 9 - 1X corporate personalization
 *              - 10 - 1X RUIM personalization
 *
 *  @param  operation
 *          - Indicates the operation to perform.
 *              - 0 - Deactivate personalization.
 *              - 1 - Unblock personalization.
 *
 *  @param  ckLen
 *          - Length of the following elements i.e. control key value.
 *
 *  @param  ckVal[MAX_DESCRIPTION_LENGTH]
 *          - Control key value.
 *          - This value is a sequence of ASCII characters.
 *
 */
typedef struct{
    uint8_t  feature;
    uint8_t  operation;
    uint8_t  ckLen;
    uint8_t  ckVal[MAX_DESCRIPTION_LENGTH];
} uim_depersonalizationInformation;

/**
 *  This structure contains information of the request parameters associated
 *  with a Depersonalization.
 *
 *  @param  depersonilisationInfo
 *          - See @ref uim_depersonalizationInformation for more information.
 *
 */
typedef struct
{
    uim_depersonalizationInformation depersonilisationInfo;
} pack_uim_SLQSUIMDepersonalization_t;

/**
 *  This structure contains information of the response parameters associated
 *  with a Depersonalization.
 *
 *  @param  pRemainingRetries(optional)
 *          - See @ref uim_remainingRetries for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uim_remainingRetries *pRemainingRetries;
    swi_uint256_t        ParamPresenceMask;
} unpack_uim_SLQSUIMDepersonalization_t;

/**
 * De-activates or unblocks the personalization on the phone pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMDepersonalization(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMDepersonalization_t *reqArg
);

/**
 * De-activates or unblocks the personalization on the phone unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMDepersonalization(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMDepersonalization_t *pOutput
);

/**
 *  This structure contains the Session Information.
 *
 *  @param  context
 *          - Authenticate context.
 *              - 0 - Runs the GSM algorithm (valid only on a 2GSIM card)
 *              - 1 - Runs the CAVE algorithm (valid only on a RUIM card)
 *              - 2 - GSM security context (valid only on a USIM application)
 *              - 3 - 3G security context (valid only on a USIM application)
 *              - 4 - VGCS/VBS security context
 *                  (valid only on aUSIM application)
 *              - 5 - GBA security context, Bootstrapping mode
 *                  (valid only on a USIM or ISIM application)
 *              - 6 - GBA security context, NAF Derivation mode
 *                  (valid only on a USIM or ISIM application)
 *              - 7 - MBMS security context, MSK Update mode
 *                  (valid only on a USIM application)
 *              - 8 - MBMS security context, MTK Generation mode
 *                  (valid only on a USIM application)
 *              - 9 - MBMS security context, MSK Deletion mode
 *                  (valid only on a USIM application)
 *              - 10 - MBMS security context, MUK Deletion mode
 *                  (valid only on a USIM application)
 *              - 11 - IMS AKA security context
 *                  (valid only on aISIM application)
 *              - 12 - HTTP-digest security context
 *                  (valid only onan ISIM application)
 *              - 13 - Compute IP authentication, CHAP
 *                  (valid onlyon RUIM or CSIM)
 *              - 14 - Compute IP authentication, MN-HA authenticator
 *                  (valid only on RUIM or CSIM)
 *              - 15 - Compute IP authentication, MIP-RRQ hash
 *                  (valid only on RUIM or CSIM)
 *              - 16 - Compute IP authentication, MN-AAA authenticator
 *                  (valid only on RUIM or CSIM)
 *              - 17 - Compute IP authentication, HRPD access authenticator
 *                  (valid only on RUIM or CSIM)
 *          - Other values are possible and reserved for future use.
 *
 *  @param  dataLen
 *          - Length of the following elements i.e. data.
 *
 *  @param  data[UIM_MAX_DESCRIPTION_LENGTH]
 *          - Authenticate Data.
 *
 */
typedef struct
{
    uint8_t  context;
    uint16_t dataLen;
    uint8_t  data[UIM_MAX_CONTENT_LENGTH];
} uim_authenticationData;

/**
 *  This structure contains information of the request parameters associated
 *  with a SLQSUIMAuthenticate.
 *
 *  @param  sessionInfo
 *          - See @ref uim_sessionInformation for more information.
 *
 *  @param  authData
 *          - See @ref uim_authenticationData for more information.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_sessionInformation sessionInfo;
    uim_authenticationData authData;
    uint32_t               *pIndicationToken;
} pack_uim_SLQSUIMAuthenticate_t;

/**
 *  This structure contains the information about the authenticate result.
 *
 *  @param  contentLen
 *          - Length of the following elements i.e. content.
 *
 *  @param  content[UIM_MAX_CONTENT_LENGTH]
 *          - Authenticate data.
 *          - This value is a sequence of bytes returned from the card.
 *
 */
typedef struct
{
    uint16_t contentLen;
    uint8_t  content[UIM_MAX_CONTENT_LENGTH];
} uim_authenticateResult;

/**
 *  This structure contains information of the response parameters associated
 *  with a SLQSUIMAuthenticate.
 *
 *  @param  pCardResult(optional)
 *          - See @ref uim_cardResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAuthenticateResult(optional)
 *          - See @ref uim_authenticateResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uim_cardResult            *pCardResult;
    uim_authenticateResult    *pAuthenticateResult;
    uint32_t                  *pIndicationToken;
    swi_uint256_t             ParamPresenceMask;
} unpack_uim_SLQSUIMAuthenticate_t;

/**
 * Send a security command to the card pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMAuthenticate(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMAuthenticate_t *reqArg
);

/**
 * Unpack the status code received from the card when card responded to the read request
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMAuthenticate(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMAuthenticate_t *pOutput
);

/**
 *  This structure contains the information about the card result.
 *
 *  @param  numFeatures
 *          - Number of active personalization features.
 *            The following block is repeated for each feature.
 *
 *  @param  feature
 *          - Indicates the personalization feature to deactivate or unblock. Valid values:
 *            - 0 - GW network personalization
 *            - 1 - GW network subset personalization
 *            - 2 - GW service provider personalization
 *            - 3 - GW corporate personalization
 *            - 4 - GW UIM personalization
 *            - 5 - 1X network type 1 personalization
 *            - 6 - 1X network type 2 personalization
 *            - 7 - 1X HRPD personalization
 *            - 8 - 1X service provider personalization
 *            - 9 - 1X corporate personalization
 *            - 10 - 1X RUIM personalization
 *
 *  @param  verifyLeft
 *          - Number of the remaining attempts to verify
 *            the personalization feature.
 *
 *  @param  unblockLeft
 *          - Number of the remaining attempts to unblock
 *            the personalization feature.
 *
 */
typedef struct
{
    uint8_t numFeatures;
    uint8_t feature[UIM_MAX_ACTIVE_PERS_FEATURES];
    uint8_t verifyLeft[UIM_MAX_ACTIVE_PERS_FEATURES];
    uint8_t unblockLeft[UIM_MAX_ACTIVE_PERS_FEATURES];
} uim_personalizationStatus;

/**
 *  This structure contains information of the request parameters associated
 *  with Get Configuration to get the modem configuration for the UIM module
 *
 *  @param  pConfigurationMask(optional)
 *          - Requested configurations
 *            - Bit 0 - Automatic selection
 *            - Bit 1 - Personalization status
 *            - Bit 2 - Halt subscription
 *            - All other bits are reserved for future use
 *
 *  @note     - if the TLV is missing, the service returns all
 *              configuration items in the response.
 */
typedef struct
{
    uint32_t *pConfigurationMask;
} pack_uim_SLQSUIMGetConfiguration_t;

/**
 *  This structure contains information of the response parameters associated
 *  with Get Configuration.
 *
 *  @param  pAutoSelection(optional)
 *          - Indicates whether the modem is configured to
 *            automatically select the provisioning sessions
 *            at powerup.
 *          - Valid values
 *           - 0 - Automatic provisioning is off
 *           - 1 - Automatic provisioning is on
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPersonalizationStatus(optional)
 *          - See @ref uim_personalizationStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pHaltSubscription(optional)
 *          - Indicates if the modem is configured to publish
 *            the subscription after successful initialization.
 *          - Valid values
 *           - 0 - Modem proceeds with publishing the subscription
 *           - 1 - Modem does not publish the subscription
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t                   *pAutoSelection;
    uim_personalizationStatus *pPersonalizationStatus;
    uint8_t                   *pHaltSubscription;
    swi_uint256_t             ParamPresenceMask;
} unpack_uim_SLQSUIMGetConfiguration_t;

/**
 * Gets the modem configuration for the UIM module pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMGetConfiguration(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMGetConfiguration_t *reqArg
);

/**
 * Gets the modem configuration for the UIM module unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMGetConfiguration(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMGetConfiguration_t *pOutput
);

/**
 *  This structure hold parameters about  UIM refresh event indication. 
 *  @param  TlvPresent
 *          - value is 1 if refresh event TLV is present in indication, otherwise 0
 *
 *  @param refreshEvent[OPTIONAL]
 *          - see @ref uim_refreshevent
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t          TlvPresent;
    uim_refreshevent refreshEvent;
    swi_uint256_t  ParamPresenceMask;
} unpack_uim_SLQSUIMRefreshCallback_Ind_t;

/**
 * Unpack UIM refresh event indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMRefreshCallback_Ind(
    uint8_t  *pResp,
    uint16_t respLen,
    unpack_uim_SLQSUIMRefreshCallback_Ind_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Set Service Status API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_UIMSessionInformation for more information.
 *
 *  @param  pFDNStatus (Optional)
 *          - 0: Disables the FDN
 *          - 1: Enables the FDN
 *
 *  @note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_UIMSessionInformation sessionInfo;
    uint8_t                   *pFDNStatus;
} pack_uim_SLQSUIMSetServiceStatus_t;

/**
 * Pack Set Service Status.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMSetServiceStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMSetServiceStatus_t *reqArg
);

typedef unpack_result_t unpack_uim_SLQSUIMSetServiceStatus_t;

/**
 * Set Service Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMSetServiceStatus(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_uim_SLQSUIMSetServiceStatus_t *pOutput
);

/**
 *  This structure contains information of the request parameters associated
 *  with a Get Service Status API.
 *
 *  \param  sessionInfo
 *          - See \ref uim_UIMSessionInformation for more information.
 *
 *  \param  capMask
 *          Bitmask of the capabilities the client retrieves from the card
 *              - Bit 0 - FDN status
 *              - Bit 1 - Hidden key PIN status (only for USIM cards)
 *              - Bit 2 – Index in the EF-DIR (only for UICC cards)
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_UIMSessionInformation  sessionInfo;
    uint32_t                   capMask;
} pack_uim_SLQSUIMGetServiceStatus_t;

/**
 * Pack Get Service Status.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMGetServiceStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMGetServiceStatus_t *reqArg
);

/**
 *  This structure contains the information for FDN Status.
 *
 *  \param  FDNStatus
 *          0 – FDN is not available
 *          1 – FDN is available and disabled
 *          2 – FDN is available and enabled
 *
 *  \param  TlvPresent
 *              - Tlv Present.
 *
 */
typedef struct
{
    uint8_t  FDNStatus;
    uint8_t  TlvPresent;
}uim_UIMGetFDNStatus;

/**
 *  This structure contains the information for Hidden Key Status.
 *
 *  @param  hiddenKey
 *          - Status of hidden key PIN
 *          0 – Not supported
 *          1 – Enabled and not verified
 *          2 – Enabled and verified
 *          3 – Disabled
 *
 *  @param  TlvPresent
 *              - Tlv Present.
 *
 */
typedef struct
{
    uint8_t  hiddenKey;
    uint8_t  TlvPresent;
}uim_UIMGetHiddenKeyStatus;

/**
 *  This structure contains the information for getting Index.
 *
 *  @param  index
 *          - Index of the application in EF_DIR file, starting from 1.
 *
 *  @param  TlvPresent
 *              - Tlv Present.
 *
 */
typedef struct
{
    uint8_t  index;
    uint8_t  TlvPresent;
}uim_UIMGetIndex;

/**
 *  This structure contains information of the response parameters associated
 *  with Get Services Status API.
 *
 *  @param  pFDNStatus(optional)
 *          - See \ref uim_UIMGetFDNStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHiddenKeyStatus(optional)
 *          - See \ref uim_UIMGetHiddenKeyStatus for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pIndex(optional)
 *          - See \ref uim_UIMGetIndex for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not returned.
 */
typedef struct
{
    uim_UIMGetFDNStatus       *pFDNStatus;
    uim_UIMGetHiddenKeyStatus *pHiddenKeyStatus;
    uim_UIMGetIndex           *pIndex;
    swi_uint256_t             ParamPresenceMask;
} unpack_uim_SLQSUIMGetServiceStatus_t;

/**
 * Get Service Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMGetServiceStatus(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_uim_SLQSUIMGetServiceStatus_t *pOutput
);

/**
 *  This structure contains the information for read record information.
 *
 *  @param  record
 *          - Record number (starting from 1).
 *
 *  @param  length
 *          - Length of the content to be read.
 *          - The value 0 is used to read the complete record.
 *
 */
typedef struct
{
    uint16_t record;
    uint16_t length;
} uim_readRecordInfo;

/**
 *  This structure contains information of the request parameters associated
 *  with a Read Record API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_UIMSessionInformation for more information.
 *
 *  @param  fileIndex
 *          - See \ref uim_fileInfo for more information.
 *
 *  @param  readRecord
 *          - See \ref uim_recordInfo for more information.
 *
 *  @param  pLastRecord
 *          - Last record.
 *            This value is used to read multiple records at the same time.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_UIMSessionInformation sessionInfo;
    uim_fileInfo              fileIndex;
    uim_readRecordInfo        readRecord;
    uint16_t                  *pLastRecord;
    uint32_t                  *pIndicationToken;
} pack_uim_SLQSUIMReadRecord_t;

/**
 * Pack Read Record Status
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMReadRecord(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMReadRecord_t *reqArg
);
/**
 *  This structure contains the information about the card result.
 *
 *  @param  sw1
 *          - SW1 received from the card.
 *
 *  @param  sw2
 *          - SW2 received from the card.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 */
typedef struct
{
    uint8_t sw1;
    uint8_t sw2;
    uint8_t TlvPresent;
} uim_cardResultInfo;

/**
 *  This structure contains the information for read operation.
 *
 *  @param  contentLen
 *          - Number of sets of content.
 *
 *  @param  content[MAX_DESCRIPTION_LENGTH]
 *          - Read content.
 *          - The content is the sequence of bytes as read from the card.
 *
 *  @param  TlvPresent
 *              - Tlv Present.
 *
 */
typedef struct
{
    uint16_t contentLen;
    uint8_t content[MAX_DESCRIPTION_LENGTH];
    uint8_t  TlvPresent;
} uim_readResultInfo;

/**
 *  This structure contains the information for additional result.
 *
 *  @param  additionalRecordLen
 *          - Number of sets of additionalRecord.
 *
 *  @param  additionalRecord[MAX_DESCRIPTION_LENGTH]
 *          - Read content of all the additional records. Each record has
 *            the same size as the first record.
 *
 *  @param  TlvPresent
 *              - Tlv Present.
 *
 */
typedef struct
{
    uint16_t additionalRecordLen;
    uint8_t  additionalRecord[MAX_DESCRIPTION_LENGTH];
    uint8_t  TlvPresent;
} uim_additionalReadResult;

/**
 *  This structure contains the information for indication.
 *
 *  \param  pIndicationToken
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 *
 */
typedef struct
{
    uint32_t       token;
    uint8_t        TlvPresent;
}uim_indToken;

/**
 *  This structure contains information of the response parameters associated
 *  with a Read Record API.
 *
 *  @param  pCardResult
 *          - See \ref uim_cardResultInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pReadResult
 *          - See \ref uim_readResultInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAdditionalReadResult
 *          - See \ref uim_additionalReadResult for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pIndicationToken(optional)
 *          - See \ref uim_indToken for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_cardResultInfo        *pCardResult;
    uim_readResultInfo        *pReadResult;
    uim_additionalReadResult  *pAdditionalReadResult;
    uim_indToken              *pIndicationToken;
    swi_uint256_t             ParamPresenceMask;
} unpack_uim_SLQSUIMReadRecord_t;

/**
 * Read Record Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMReadRecord(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_uim_SLQSUIMReadRecord_t *pOutput
);

/**
 *  This structure contains the information for write operation.
 *
 *  @param  record
 *          - Record number (starting from 1).
 *          - This field is ignored for cyclic files.
 *
 *  @param  length
 *          - Number of sets of content.
 *
 *  @param  content[MAX_DESCRIPTION_LENGTH]
 *          - Content to write.
 *
 */
typedef struct
{
    uint16_t record;
    uint16_t dataLen;
    uint8_t data[MAX_DESCRIPTION_LENGTH];
} uim_writeRecordInfo;

/**
 *  This structure contains information of the request parameters associated
 *  with a Write Record API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_UIMSessionInformation for more information.
 *
 *  @param  fileIndex
 *          - See \ref uim_fileInfo for more information.
 *
 *  @param  writeRecord
 *          - See \ref uim_writeRecordInfo for more information.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_UIMSessionInformation sessionInfo;
    uim_fileInfo              fileIndex;
    uim_writeRecordInfo       writeRecord;
    uint32_t              *pIndicationToken;
} pack_uim_SLQSUIMWriteRecord_t;

/**
 * Pack Write Record data
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMWriteRecord(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_uim_SLQSUIMWriteRecord_t *reqArg
);

/**
 *  This structure contains information of the response parameters associated
 *  with a Write Record API.
 *
 *  \param  pCardResult
 *          - See \ref uim_cardResultInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pIndicationToken(optional)
 *          - See \ref uim_indToken for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_cardResultInfo   *pCardResult;
    uim_indToken         *pIndicationToken;
    swi_uint256_t        ParamPresenceMask;
} unpack_uim_SLQSUIMWriteRecord_t;

/**
 * Write Record Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMWriteRecord(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_uim_SLQSUIMWriteRecord_t *pOutput
);

/**
 *  This structure contains the information for write operation.
 *
 *  @param  offset
 *          - Offset for the write operation.
 *
 *  @param  dataLen
 *          - Length of the following elements i.e. data.
 *
 *  @param  data[MAX_CONTENT_LENGTH]
 *          - Content to write.
 *
 */
typedef struct
{
    uint16_t offset;
    uint16_t dataLen;
    uint8_t  data[UIM_MAX_CONTENT_LENGTH];
} uim_writeTransparentInfo;

/**
 *  This structure contains information of the request parameters associated
 *  with a Write Transparent API.
 *
 *  @param  sessionInfo
 *          - See \ref uim_UIMSessionInformation for more information.
 *
 *  @param  fileId
 *          - See \ref uim_fileInfo for more information.
 *
 *  @param  writeTransparent
 *          - See \ref uim_writeTransparentInfo for more information.
 *
 *  @param  pIndicationToken(optional)
 *          - Response in Indication.
 *          - When this TLV is present, it indicates that the result must be
 *            provided in a subsequent indication.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not added to the request.
 */
typedef struct
{
    uim_UIMSessionInformation  sessionInfo;
    uim_fileInfo               fileIndex;
    uim_writeTransparentInfo   writeTransparent;
    uint32_t                   *pIndicationToken;
} pack_uim_SLQSUIMWriteTransparent_t;

/**
 * Pack Write Transparent
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_uim_SLQSUIMWriteTransparent(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_uim_SLQSUIMWriteTransparent_t *reqArg
);

/**
 *  This structure contains information of the response parameters associated
 *  with a Write Transparent API.
 *
 *  @param  pCardResult(optional)
 *          - See \ref uim_cardResultInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pIndicationToken(optional)
 *          - See \ref uim_indToken for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  \note   Using NULL for the pointers would make sure that the parameter is
 *          not returned.
 */
typedef struct
{
    uim_cardResultInfo   *pCardResult;
    uim_indToken         *pIndicationToken;
    swi_uint256_t        ParamPresenceMask;
} unpack_uim_SLQSUIMWriteTransparent_t;

/**
 * Write Transparent unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_uim_SLQSUIMWriteTransparent(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_uim_SLQSUIMWriteTransparent_t *pOutput
);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif

