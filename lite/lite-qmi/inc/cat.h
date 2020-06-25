/**
 * \ingroup liteqmi
 *
 * \file cat.h
 */
#ifndef __LITEQMI_CAT_H__
#define __LITEQMI_CAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define CAT_EVENT_DATA_MAX_LENGTH   255
#define CAN_COMMON_EVENT_TLV_NUMBER 11

/**
 *  This structure contains sEnables/disables the CAT event callback pack variable.
 *
 *  @param  eventMask
 *          - bitmask of CAT events to register for
 *              - 0x00000001 - Display Text
 *              - 0x00000002 - Get In-Key
 *              - 0x00000004 - Get Input
 *              - 0x00000008 - Setup Menu
 *              - 0x00000010 - Select Item
 *              - 0x00000020 - Send SMS - Alpha Identifier
 *              - 0x00000040 - Setup Event: User Activity
 *              - 0x00000080 - Setup Event: Idle Screen Notify
 *              - 0x00000100 - Setup Event: Language Sel Notify
 *              - 0x00000200 - Setup Idle Mode Text
 *              - 0x00000400 - Language Notification
 *              - 0x00000800 - Refresh
 *              - 0x00001000 - End Proactive Session
 */

typedef struct {
    uint32_t       eventMask;
}pack_cat_SetCATEventCallback_t;

/**
 * Enables/disables the CAT event callback pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_cat_SetCATEventCallback(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_cat_SetCATEventCallback_t *reqArg
);
/**
 *  This structure contains sEnables/disables the CAT event callback unpack variable.
 *
 *  @param  errorMask
 *          - error bitmask. Each bit set indicates the proactive command that
 *            caused the error
 *              - 0x00000001 - Display Text
 *              - 0x00000002 - Get In-Key
 *              - 0x00000004 - Get Input
 *              - 0x00000008 - Setup Menu
 *              - 0x00000010 - Select Item
 *              - 0x00000020 - Send SMS - Alpha Identifier
 *              - 0x00000040 - Setup Event: User Activity
 *              - 0x00000080 - Setup Event: Idle Screen Notify
 *              - 0x00000100 - Setup Event: Language Sel Notify
 *              - 0x00000200 - Setup Idle Mode Text
 *              - 0x00000400 - Language Notification
 *              - 0x00000800 - Refresh
 *              - 0x00001000 - End Proactive Session
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 */
typedef struct {
    uint32_t       errorMask;
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_cat_SetCATEventCallback_t;

/**
 * Enables/disables the CAT event callback unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_cat_SetCATEventCallback(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_cat_SetCATEventCallback_t *pOutput
);

/**
 * structure used to store all Common CAT Event parameters.
 *
 * @param   ReferenceID - proactive command reference ID.
 * @param   DataLength  - length of pData ( in Bytes )
 * @param   Data        - command specific to the CAT event ID, encoded as in
 *                        ETSI TS 102 223 [Section 6.6.X]
 */
struct cat_EventIDDataTlv
{
    uint32_t  ReferenceID;
    uint16_t  DataLength;
    uint8_t   Data[CAT_EVENT_DATA_MAX_LENGTH];
};

/**
 * structure used to store all Alpha Identifier parameters.
 *
 * @param   ReferenceID   - proactive command type that included the alpha
 *                          identifier - 0x01; sends SMS proactive command
 * @param   AlphaIDLength - length of AlphaID ( in bytes )
 * @param   AlphaID       - alpha identifier, encoded as in
 *                          ETSI TS 102 223 [Section 8.2]
 */
struct cat_AlPhaIdentifierTlv
{
    uint8_t    ReferenceID;
    uint16_t   AlphaIDLength;
    uint8_t    AlphaID[CAT_EVENT_DATA_MAX_LENGTH];
};

/**
 * structure used to store all Event List parameters.
 *
 * @param SetupEventList - Setup event list bit mask
 *                          - 0x00000001 - User Activity Notify
 *                          - 0x00000002 - Idle Screen Available
 *                          - 0x00000004 - Lang Selection Notify
 *                         Each set bit indicates the availability of the
 *                         corresponding event in Setup Event list proactive
 *                         command; all unlisted bits are reserved for future
 *                         use and will be ignored
 *
 */
struct cat_EventListTlv
{
    uint32_t SetupEventList;
};

/**
 * structure used to store all Refresh Event parameters.
 *
 * @param  RefreshMode  - The Refresh Event as in ETSI TS 102 223 [Section 8.6]
 * @param  RefreshStage - Stage of a refresh procedure
 *                          - 0x01 - Refresh start
 *                          - 0x02 - Refresh success
 *                          - 0x03 - Refresh failed
 */
struct cat_RefreshTlv
{
    uint16_t  RefreshMode;
    uint8_t   RefreshStage;
};

/**
 * structure used to store End Proactive Session event parameters.
 *
 * @param   EndProactiveSession - The proactive session end type values are:
 *              - 0x01 - End proactive session command type
 *                       received from the card
 *              - 0x02 - End proactive session internal to ME
 *
 */
struct cat_EndProactiveSessionTlv
{
    uint8_t EndProactiveSession;
};

/**
 *  Union used to represent the current CAT Event Data. Choose the structure
 *  based on the EventID received.
 *      - Use @ref cat_EventIDDataTlv if the Event ID is any of the below.
 *          - 16
 *          - 17
 *          - 18
 *          - 19
 *          - 20
 *          - 23
 *          - 24\n
 *      - Use @ref cat_AlPhaIdentifierTlv if the Event ID is 21\n
 *      - Use @ref cat_EventListTlv if the Event ID is 22\n
 *      - Use @ref cat_RefreshTlv if the Event ID is 25\n
 *      - Use @ref cat_EndProactiveSessionTlv if the Event ID is 26\n
 *
 */
union cat_currentCatEvent
{
    struct cat_EventIDDataTlv         CatEvIDData;
    struct cat_AlPhaIdentifierTlv     CatAlphaIdtfr;
    struct cat_EventListTlv           CatEventLst;
    struct cat_RefreshTlv             CatRefresh;
    struct cat_EndProactiveSessionTlv CatEndPS;
};
/**
 * Structure used to store all Common CAT Event TLV Value.
 *
 *  @param TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *  @param EventID     - Event ID. Can be any of the following\n
 *                          16 -  Display Text
 *                          17 -  Get In-Key
 *                          18 -  Get Input
 *                          19 -  Setup Menu
 *                          20 -  Select Item
 *                          21 -  Send SMS - Alpha Identifier
 *                          22 -  Setup Event List
 *                          23 -  Setup Idle Mode Text
 *                          24 -  Language Notification
 *                          25 -  Refresh
 *                          26 -  End Proactive Session
 *
 *  @param EventLength - Length of pData ( in Bytes )
 *
 *  @param CatEvent    - Structure to the Data specific to the CAT event ID
 */
struct cat_commonEventTlv
{
    uint8_t                      TlvPresent;
    uint8_t                      EventID;
    uint16_t                     EventLength;
    union cat_currentCatEvent CatEvent;
};

/**
 * Structure used to store all CAT Notification Parameters.
 *
 *  @param event_Index     - Event Index
 *
 *  @param CCETlv          - CAT common event Tlv
 *                           @ref cat_commonEventTlv for more info.
 *                         - [0] - Bit to check in ParamPresenceMask for Display Text - <B>16</B>
 *                         - [1] - Bit to check in ParamPresenceMask for Get In-Key   - <B>17</B>
 *                         - [2] - Bit to check in ParamPresenceMask for Get Input - <B>18</B>
 *                         - [3] - Bit to check in ParamPresenceMask for Setup Menu - <B>19</B>
 *                         - [4] - Bit to check in ParamPresenceMask for Select Item - <B>20</B>
 *                         - [5] - Bit to check in ParamPresenceMask for Alpha Identifier - <B>21</B>
 *                         - [6] - Bit to check in ParamPresenceMask for Setup Event List - <B>22</B>
 *                         - [7] - Bit to check in ParamPresenceMask for Setup Idle Mode Text - <B>23</B>
 *                         - [8] - Bit to check in ParamPresenceMask for Language Notification - <B>24</B>
 *                         - [9] - Bit to check in ParamPresenceMask for Refresh - <B>25</B>
 *                         - [10] - Bit to check in ParamPresenceMask for End Proactive Session - <B>26</B>
 *
 */
typedef struct {
    uint8_t   event_Index;
    struct cat_commonEventTlv CCETlv[CAN_COMMON_EVENT_TLV_NUMBER];
    swi_uint256_t      ParamPresenceMask;
}unpack_cat_SetCatEventCallback_ind_t;

/**
 * CAT event indication unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_cat_SetCatEventCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_cat_SetCatEventCallback_ind_t *pOutput
        );

/**
 * Structure used to store envelope command data.
 *
 *  @param  cmdID
 *          - Envelope command type
 *              - 0x01 - Menu Selection
 *              - 0x02 - Event DL User activity
 *              - 0x03 - Event DL Idle Screen Available
 *              - 0x04 - Event DL Language Selection
 *
 *  @param  dataLen
 *          - Length of pData in bytes
 *
 *  @param  pData
 *          - Encoded envelope data as defined in ETSI TS 102 223, section 7\n
 *            [Smart Cards: Card Application Toolkit (CAT) - Release 4]
 */
typedef struct {
    uint32_t cmdID;
    uint32_t dataLen;
    uint8_t  *pData;
}pack_cat_CATSendEnvelopeCommand_t;

typedef unpack_result_t  unpack_cat_CATSendEnvelopeCommand_t;
/**
 * Sends the envelope command to the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_cat_CATSendEnvelopeCommand(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_cat_CATSendEnvelopeCommand_t *reqArg
);

/**
 * Sends the envelope command to the device unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_cat_CATSendEnvelopeCommand(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_cat_CATSendEnvelopeCommand_t *pOutput
        );

/**
 * Structure used to terminal response data.
 *
 *  @param  refID
 *          - Proactive command reference ID. The value should be the same as
 *            indicated in the CAT event callback data for the relevant
 *            proactive command.
 *
 *  @param  dataLen
 *          - Terminal response data length
 *
 *  @param  pData
 *          - Terminal response for the relevant proactive command encoded as
 *            per ETSI TS 102 223, section 6.8\n
 *            [Smart Cards: Card Application Toolkit (CAT) - Release 4]
 *
 */
typedef struct {
    uint32_t refID;
    uint32_t dataLen;
    uint8_t  *pData;
}pack_cat_CATSendTerminalResponse_t;

typedef unpack_result_t unpack_cat_CATSendTerminalResponse_t;

/**
 * Sends the terminal response to the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *  *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_cat_CATSendTerminalResponse(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_cat_CATSendTerminalResponse_t *reqArg
);

/**
 * Sends the terminal response to the device unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_cat_CATSendTerminalResponse(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_cat_CATSendTerminalResponse_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif//#ifndef  __LITEQMI_CAT_H__
