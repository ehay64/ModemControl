#ifndef _HELPER_SMS_H_
#define _HELPER_SMS_H_

#define MAX_SMS_MSG_LEN                             161
#define MAX_SHORT_SMS_MSG_TOTAL_LEN_UCS             140
#define MAX_SHORT_SMS_MSG_CHAR_LEN_EIGHTBIT         140
#define MAX_SHORT_SMS_MSG_CHAR_LEN_DEFAULT          160
#define MAX_LONG_SMS_MSG_TOTAL_LEN_UCS              134 /*(67 *2 + 6 (UDH header size) = 140) */
#define MAX_LONG_SMS_MSG_CHAR_LEN_EIGHTBIT          134 /*(134 + 6) */
#define MAX_LONG_SMS_MSG_CHAR_LEN_DEFAULT           153 /*(153 septets + 7 septets UDH = 160 sep = 140 */
#define MAX_SMS_UD_SEPTET_LEN                       160
#define MAX_SMS_UD_OCTET_LEN                        140

#define TELESERVICE_ID_VALUE                        4098
#define POINT_TO_POINT_MSG                          0

#define TELESERVICE_IDENTIFIER                      0
#define TELESERVICE_HDR_LEN                         2

#define DESTADDR_IDENTIFIER                         4
#define DESTADDR_HDR_LEN                            7

#define CDMA_PDU_IDENIFIER_HDR_LENGTH               1

#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))
#define CALLBACK_NUM_SUB_IDENTIFIER                 14

#define MSG_SUB_IDENTIFIER_ID                       0
#define MSG_SUB_IDENTIFIER_HDR_LEN                  3

#define USER_DATA_MSG_ENCODING_7BIT_ASCII           2

#define PRIORIY_SUB_IDENTIFIER                      8
#define PRIORIY_SUB_IDENTIFIER_HDR_LEN              1

#define REL_VAL_PERIOD_SUB_IDENTIFIER               5
#define REL_VAL_PERIOD_SUB_IDENTIFIER_HDR_LEN       1

#define BEARER_DATA_IDENTIFIER                      8

#define SMS_ASCII_STRING_SIZE 400
#define SMS_ASCII_STRING_ARRAY_SIZE SMS_ASCII_STRING_SIZE + 1


#define UCSENCODING                              8
#define UCSENCODING_CHAR                         '8'

#define EIGHTBITENCODING                         4
#define EIGHTBITENCODING_CHAR                    '4'

#define DEFAULTENCODING                          0
#define DEFAULTENCODING_CHAR                     '0'

#define NUMBERFORMAT      145
#define NATIONALFORMAT    129
#define ENTER_KEY         0x0A

#define ASC2HEX(x) ( ((x >= '0') && (x <= '9')) ? \
                                      (x - '0') : \
                     ((x >= 'A') && (x <= 'F')) ? \
                                 (x - 'A' + 10) : \
                     ((x >= 'a') && (x <= 'f')) ? \
                             (x - 'a' + 10) : 0 )

#define GSM_CHARACTER_SET_SIZE                   0x80
#define GSM_ESC_CHAR_SET_SIZE                    0x0A
#if (GSM_CHARACTER_SET_SIZE+GSM_ESC_CHAR_SET_SIZE-1) > 256
#error OVERFLOW
#endif

#define MAXSMS_BINARY                            MAX_SMS_UD_SEPTET_LEN
#define MAXIMUM_PDU                              (MAXSMS_BINARY + LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER)*2


#define MAX_SMSI_TL_ADDRESS  50 /* Maximum SMS address length */
#define MAX_SMSI_CALL_BACK   32 /* Maximum SMS callback length */

#define BEARER_REPLY_IDENTIFIER                     6
#define BEARER_REPLY_HDR_LEN                        1
#define BEARER_INV_SEQ_VALUE                        0xff

#define ORIGADDR_IDENTIFIER                         2
#define ORIGADDR_HDR_LEN                            7

#define SERVICE_CATEGORY_IDENIFIER                  1
#define SERVICE_CATEGORY_HDR_LEN                    2

#define ORIGSUBADDR_IDENTIFIER                      3

/* CDMA SMS decoding defines */
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define HEX(x, i) (uint8_t)(((x[i/2] >> ((i & 1) ? 0 : 4)) & 0x0F))

#define bitsize(type) (sizeof(type) * 8)
#define copymask(type) ((0xffffffff) >> (32 - bitsize(type)))

#define MSG_TIMESTAMP_SUB_INDETIFIER                3
#define MSG_TIMESTAMP_SUB_INDETIFIER_HDR_LEN        6

#define LANGUAGE_INDICATOR_SUB_INDETIFIER           13
#define LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN   1

#define USER_RESP_CODE_SUB_IDENTIFIER               2

#define ABS_VAL_PERIOD_SUB_IDENTIFIER               4
#define ABS_VAL_PERIOD_SUB_IDENTIFIER_HDR_LEN       6

#define DEF_DELIVERY_TIME_ABS_SUB_IDENTIFIER        6

#define DEF_DELIVERY_TIME_REL_SUB_IDENTIFIER        7

#define PRIVACY_SUB_IDENTIFIER                      9

#define REPLY_OPTION_SUB_IDENTIFIER                 10
#define REPLY_OPTION_SUB_IDENTIFIERHDR_LEN          1

#define NUM_OF_MSGS_SUB_IDENTIFIER                  11

#define ALERT_MSG_DELIVERY_SUB_IDENTIFIER           12

#define MSG_DISP_MODE_SUB_IDENTIFIER                15

#define MULTI_ENCODING_USER_DATA_SUB_IDENTIFIER     16

#define MSG_DEPOSIT_INDEX_SUB_IDENTIFIER            17

#define SERVICE_CAT_PROGRAM_DATA_SUB_IDENTIFIER     18

#define SERVICE_CAT_PROGRAM_RESULT_SUB_IDENTIFIER   19

#define MSG_STATUS_SUB_IDENTIFIER                   20

#define USER_DATA_SUB_IDENTIFIER                    1
#define USER_DATA_MSG_ENCODING_8BIT_OCTET           0
#define USER_DATA_MSG_ENCODING_7BIT_ASCII           2
#define USER_DATA_MSG_ENCODING_8BIT_LATIN           8
#define USER_DATA_MSG_ENCODING_7BIT_GSM             9

#define INFORMATION_ELEMENT_IDENTIFIER              0x00
#define TIME_DATE_BUF      0x09

#define BCD_DECODE(x)   (x >> 4) * 10 + (x & 0x0F)

#define TRUE  1
#define FALSE 0

#define MAX_SMSC_LENGTH                          24
#define MAX_SENDER_ADDRESS_LENGTH                12

#define PDU_HEADER_LENGTH 18
#define PDU_TYPE0_HEADER_LENGTH PDU_HEADER_LENGTH
#define PDU_TYPE1_HEADER_LENGTH PDU_HEADER_LENGTH+2
#define MIN_SMS_SENT_LENGTH 32
#define MAX_SMS_EXTRACT_CONTENT_LENGTH 160
#define MIN_SMS_EXTRACT_CONTENT_LENGTH 34
#define SMS_MESSAGE_TIMESTEMP_SIZE 8
#define SMS_USER_DATA_HDR_LEN 2

#define MAX_SMS_NUMBER_LENGTH 50
#define MAX_SMS_NUMBER_ARRAY_LENGTH MAX_SMS_NUMBER_LENGTH+1

typedef enum{
    ePDU_TYPE_UNKNOWN=-1,
    ePDU_TYPE_0=0,
    ePDU_TYPE_1=1
}ePDU_TYPE;

#ifndef UNUSEDPARAM
#define UNUSEDPARAM( x ) (void)x
#endif

/******************************************************************************
MACRO MASK

DESCRIPTION
   Masks the bits in data at the given offset for given number of width bits.
******************************************************************************/
#define MASK(width, offset, data) \
    /*lint -e701 shift left  of signed quantity  */  \
    /*lint -e702 shift right of signed quantity  */  \
    /*lint -e572 Excessive shift value           */  \
    /*lint -e573 Signed-unsigned mix with divide */  \
    /*lint -e506 Constant value boolean          */  \
    /*lint -e649 Sign fill during constant shift */  \
                                                     \
   (((width) == bitsize(data)) ? (data) :   \
   ((((copymask(data) << (bitsize(data) - ((width) % bitsize(data)))) \
    & copymask(data)) >>  (offset)) & (data))) \
                     \
    /*lint +e701 */  \
    /*lint +e702 */  \
    /*lint +e572 */  \
    /*lint +e573 */  \
    /*lint +e506 */  \
    /*lint +e649 */

/******************************************************************************
MACRO MASK_AND_SHIFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result by the
   given number of shift bits.
*******************************************************************************/
#define MASK_AND_SHIFT(width, offset, shift, data)  \
    /*lint -e504 Unusual shifter value */  \
                  ((((signed) (shift)) < 0) ?       \
                    MASK((width), (offset), (data)) << -(shift) :  \
                    MASK((width), (offset), (data)) >>  (((unsigned) (shift)))) \
    /*lint +e504 */
/*
 * Name:     LITE_SMSHELPER_TYPE_SMS_DataEncodeType - SMS Data Encoding enumeration
 *
 * Purpose:  This enumeration identifies the possible data encoding methods
 *           used on the SMS message body.
 *
 * Members:  LITE_SMSHELPER_SMS_DATAENCODE_8bitAscii     - Octet, unspecified (i.e. Binary)
 *           LITE_SMSHELPER_SMS_DATAENCODE_IS91EP        -  varies
 *           LITE_SMSHELPER_SMS_DATAENCODE_7bitAscii     - Standard ascii, 7-bit
 *           LITE_SMSHELPER_SMS_DATAENCODE_IA5           - 7-bit  encoding (  International Alphabet No. 5 same as ASCII )
 *           LITE_SMSHELPER_SMS_DATAENCODE_Unicode       - Unicode
 *           LITE_SMSHELPER_SMS_DATAENCODE_Shift_Jis     - 8 or 16-bit-It's also known as SJIS or MS Kanji for Japanese support
 *           LITE_SMSHELPER_SMS_DATAENCODE_Korean        - 8 or 16-bit - Korean support
 *           LITE_SMSHELPER_SMS_DATAENCODE_LatinHebrew   - 8 specific to the Hebrew script ISO-8859-8 and Windows-1255
 *           LITE_SMSHELPER_SMS_DATAENCODE_Latin         - Latin
 *           LITE_SMSHELPER_SMS_DATAENCODE_GSM7BitDefault- 7-bit - defined in ETSI GSM 03.38.
 *           LITE_SMSHELPER_SMS_DATAENCODE_MAX32
 *
 *
 * Notes:    For a more complete list, see SMS standard TSB-58C.
 *
 */
typedef enum
{
        LITE_SMSHELPER_SMS_DATAENCODE_8bitAscii = 0,
        LITE_SMSHELPER_SMS_DATAENCODE_IS91EP,
        LITE_SMSHELPER_SMS_DATAENCODE_7bitAscii,
        LITE_SMSHELPER_SMS_DATAENCODE_IA5,
        LITE_SMSHELPER_SMS_DATAENCODE_Unicode,
        LITE_SMSHELPER_SMS_DATAENCODE_Shift_Jis,
        LITE_SMSHELPER_SMS_DATAENCODE_Korean,
        LITE_SMSHELPER_SMS_DATAENCODE_LatinHebrew,
        LITE_SMSHELPER_SMS_DATAENCODE_Latin,
        LITE_SMSHELPER_SMS_DATAENCODE_GSM7BitDefault,
  LITE_SMSHELPER_SMS_DATAENCODE_MAX32 = 0x10000000
}LITE_SMSHELPER_TYPE_SMS_DataEncodeType;

typedef enum {
    LITE_SMSHELPER_UDH_LENGTH, /* UDH length */
    LITE_SMSHELPER_INFORMATION_ELEMENT_ID, /* Information element identifier */
    LITE_SMSHELPER_INFORMATION_ELEMENT_ID_LENGTH, /* length of Information element identifier */
    LITE_SMSHELPER_SMS_REFERENCE_NUMBER, /* reference number of the sms */
    LITE_SMSHELPER_SMS_TOTAL_NUMBER, /* Total number of parts */
    LITE_SMSHELPER_SMS_SEQUENCE_NUMBER, /* Number of current part */
    LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER
} eLITE_SMSHELPER_UDH_FIELD;

#define UDH_TOTAL_FIELD_NUMBER_ARRAY_SIZE LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER+1
/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_ParamMask.
 * This identifies the meanings of the bits in the
 * LITE_SMSHELPER_STRUCT_SMS_CdmaHeader parameter mask of available fields.
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_PARAMASK_NO_PARAMS,
    LITE_SMSHELPER_SMS_PARAMASK_BEARER_REPLY,
    LITE_SMSHELPER_SMS_PARAMASK_VALID_ABS,
    LITE_SMSHELPER_SMS_PARAMASK_VALID_REL     = 0x00000004,
    LITE_SMSHELPER_SMS_PARAMASK_DEFER_ABS     = 0x00000008,
    LITE_SMSHELPER_SMS_PARAMASK_DEFER_REL     = 0x00000010,
    LITE_SMSHELPER_SMS_PARAMASK_USER_RESP     = 0x00000020,
    LITE_SMSHELPER_SMS_PARAMASK_ADDRESS       = 0x00000040,
    LITE_SMSHELPER_SMS_PARAMASK_SUBADDR       = 0x00000080,
    LITE_SMSHELPER_SMS_PARAMASK_USER_DATA     = 0x00000100,
    LITE_SMSHELPER_SMS_PARAMASK_PRIORITY      = 0x00000200,
    LITE_SMSHELPER_SMS_PARAMASK_PRIVACY       = 0x00000400,
    LITE_SMSHELPER_SMS_PARAMASK_REPLY_OPT     = 0x00000800,
    LITE_SMSHELPER_SMS_PARAMASK_NUM_OF_MSG    = 0x00001000,
    LITE_SMSHELPER_SMS_PARAMASK_ALERT         = 0x00002000,
    LITE_SMSHELPER_SMS_PARAMASK_LANGUAGE      = 0x00004000,
    LITE_SMSHELPER_SMS_PARAMASK_CALL_BACK     = 0x00008000,
    LITE_SMSHELPER_SMS_PARAMASK_MC_TIME_STAMP = 0x00010000,
    LITE_SMSHELPER_SMS_PARAMASK_DISP_MODE     = 0x00020000
}LITE_SMSHELPER_TYPE_SMS_ParamMask;

/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_MsgCategory.
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_MSGCAT_POINT_TO_POINT,
    LITE_SMSHELPER_SMS_MSGCAT_BROADCAST_MSG,
    LITE_SMSHELPER_SMS_MSGCAT_ACK_MSG
}LITE_SMSHELPER_TYPE_SMS_MsgCategory;
/*
 *
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_Teleservice
 * - SMS Message Teleservice Type enumeration. Defines the SMS teleservice type.
 *   Refer to TIA/EIA-41-D Table 175 for further information.
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_TELESRV_CELL_PAGING,
    LITE_SMSHELPER_SMS_TELESRV_CELL_MESSAGING,
    LITE_SMSHELPER_SMS_TELESRV_VOICE_MAIL_NOT,
    LITE_SMSHELPER_SMS_TELESRV_CLI_ORDER,
    LITE_SMSHELPER_SMS_TELESRV_VOICE_IND,
    LITE_SMSHELPER_SMS_TELESRV_SHORT_MSG,
    LITE_SMSHELPER_SMS_TELESRV_ALERT_INFO,
    LITE_SMSHELPER_SMS_TELESRV_WAP
}LITE_SMSHELPER_TYPE_SMS_Teleservice;

/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_MsgIdType
 * - SMS Message ID Type enumeration.Defines the SMS Message ID Type.
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_MSGIDTYPE_NOTUSED,
    LITE_SMSHELPER_SMS_MSGIDTYPE_DELIVER,
    LITE_SMSHELPER_SMS_MSGIDTYPE_SUBMIT,
    LITE_SMSHELPER_SMS_MSGIDTYPE_CANCEL,
    LITE_SMSHELPER_SMS_MSGIDTYPE_DELIVERY_ACK,
    LITE_SMSHELPER_SMS_MSGIDTYPE_USER_ACK
}LITE_SMSHELPER_TYPE_SMS_MsgIdType;

/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_Priority - SMS Message Priority enumeration
 * Defines the priority of the SMS Message.
 *
 */
typedef enum
{
        LITE_SMSHELPER_SMS_PRIORITY_NORMAL,
        LITE_SMSHELPER_SMS_PRIORITY_INTERACTIVE,
        LITE_SMSHELPER_SMS_PRIORITY_URGENT,
        LITE_SMSHELPER_SMS_PRIORITY_EMERGENCY
} LITE_SMSHELPER_TYPE_SMS_Priority;

/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_Privacy - SMS Message Privacy enumeration
 * Defines the privacy level of the SMS message
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_PRIVACY_NOT_RESTR,
    LITE_SMSHELPER_SMS_PRIVACY_RESTRICTED,
    LITE_SMSHELPER_SMS_PRIVACY_CONFIDENTIAL,
    LITE_SMSHELPER_SMS_PRIVACY_SECRET
}LITE_SMSHELPER_TYPE_SMS_Privacy;

/*
 *
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_Language - SMS Language type enumeration.
 * Defines the language used in the message body
 *
 */
typedef enum
{
    LITE_SMSHELPER_SMS_LANGUAGE_UNSPECIFIED,
    LITE_SMSHELPER_SMS_LANGUAGE_ENGLISH,
    LITE_SMSHELPER_SMS_LANGUAGE_FRENCH,
    LITE_SMSHELPER_SMS_LANGUAGE_SPANISH,
    LITE_SMSHELPER_SMS_LANGUAGE_JAPANESE,
    LITE_SMSHELPER_SMS_LANGUAGE_KOREAN,
    LITE_SMSHELPER_SMS_LANGUAGE_CHINESE,
    LITE_SMSHELPER_SMS_LANGUAGE_HEBREW
}LITE_SMSHELPER_TYPE_SMS_Language;

/*
 * An enumeration of LITE_SMSHELPER_TYPE_SMS_MsgDispMode - SMS Message
 * Display Mode enumeration. Specifies how the SMS message should be displayed
 *
 */
typedef enum
{
        LITE_SMSHELPER_SMS_MSGDISPMODE_IMMEDIATE,
        LITE_SMSHELPER_SMS_MSGDISPMODE_DEFAULT,
        LITE_SMSHELPER_SMS_MSGDISPMODE_USER_INVOKE
}LITE_SMSHELPER_TYPE_SMS_MsgDispMode;


/**
 * <b>SMS helper Error Code Enumeration</b>
 */
typedef enum sMSHelperError
{
    sMS_HELPER_ERR_ENUM_BEGIN = -1,
    /** 00 - Success */
    sMS_HELPER_OK,
    /** 01 - General error */
    sMS_HELPER_ERR_GENERAL,
    /** 02 - Internal error */
    sMS_HELPER_ERR_INTERNAL,
    /** 03 - Memory error */
    sMS_HELPER_ERR_MEMORY,
    /** 04 - Invalid argument */
    sMS_HELPER_ERR_INVALID_ARG,
    /** 05 - Buffer too small */
    sMS_HELPER_ERR_BUFFER_SZ,
    /** 1094 - Not Supported */
    sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED =1094,
    /** 0xE101 - SMS message length is long */
    sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG = 0xE101,
    /** 0xE102 - The SMS message is corrupted (encoding wrong) */
    sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED = 0xE102,
    /** 0xE103 - The SMS number is corrupted (incorrect number) */
    sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED = 0xE103,
    /** 0xE104 - The SMS bearer data is not available */
    sMS_HELPER_ERR_SWISMS_BEARER_DATA_NOT_FOUND = 0xE104,
} sMSHelperError;

typedef struct
{
    int8_t  data[TIME_DATE_BUF];
}sms_DateTime;

#define SMS_DEFAULT_TIME_ZONE 0
#define SMS_INVALID_TIME_STAMP_VALUE 0xff
typedef enum 
{
  eTIMESTEMP_START = -1,
  eTIMESTEMP_YEAR = 0,
  eTIMESTEMP_MONTH = 1,
  eTIMESTEMP_DAY = 2,
  eTIMESTEMP_HOUR = 3,
  eTIMESTEMP_MINUTE = 4,
  eTIMESTEMP_SECOND = 5,
  eTIMESTEMP_TIMEZONE = 6,
  eTIMESTEMP_END = 7
} sEnumTimeStampenum;

typedef struct
{
    uint8_t data[SMS_MESSAGE_TIMESTEMP_SIZE];
}sms_timestemp;
/*
 * LITE_SMSHELPER_STRUCT_SMS_CdmaHeader - CDMA SMS Header
 *
 * this structure defines the header of incoming
 * SMS messages. Most of the data items are
 * defined in the CDMA specification and are not
 * explained here. The enumerated types are
 * described following this structure.
 *
 * sizeStruct    - size of this structure
 *
 * eParamMask    - Parameter Mask indicating which of the below fields
 *                are valid.  See LITE_SMSHELPER_TYPE_SMS_ParamMask.
 *
 * eMsgCategory  - Message Category
 *                 See LITE_SMSHELPER_TYPE_SMS_MsgCategory
 *
 * eTeleservice  - Message Teleservice
 *                 See LITE_SMSHELPER_TYPE_SMS_Teleservice
 *
 * serviceCategory - Service Category (News, Restaraunts, etc.)
 *                   Refer to TIA/EIA-637 Section 3.4.3.2 for more detail.
 *
 * tlAckReq        - Indicates if the client needs to confirm
 *                   whether the message is received successfully or not.
 *
 * eMsgIdType      - Message ID Type
 *                   See LITE_SMSHELPER_TYPE_SMS_MsgIdType
 *
 * addrDigitMode   - Address Digit Mode
 *
 * addrNumMode     - Address number mode
 *
 * addrNumType     - Address number type:
 *                   GSM SMS: addr value is GSM 7-bit chars
 *                   if NumMode == 0
 *                   WMS_NUMBER_UNKNOWN        = 0,
 *                   WMS_NUMBER_INTERNATIONAL  = 1,
 *                   WMS_NUMBER_NATIONAL       = 2,
 *                   WMS_NUMBER_NETWORK        = 3,
 *                   WMS_NUMBER_SUBSCRIBER     = 4,
 *                   WMS_NUMBER_ALPHANUMERIC   = 5,
 *                   WMS_NUMBER_ABBREVIATED    = 6,
 *                   WMS_NUMBER_RESERVED_7     = 7,
 *                   if NumMode == 1
 *                   WMS_NUMBER_DATA_IP        = 1
 *                   WMS_NUMBER_INTERNET_EMAIL = 2
 *
 * addrNumPlan      - Address number plan
 *                    CCITT E.164 and E.163, including ISDN plan
 *                    WMS_NUMBER_PLAN_UNKNOWN     = 0,
 *                    WMS_NUMBER_PLAN_TELEPHONY   = 1,
 *                    WMS_NUMBER_PLAN_RESERVED_2  = 2,
 *                    WMS_NUMBER_PLAN_DATA        = 3, :CCITT X.121
 *                    WMS_NUMBER_PLAN_TELEX       = 4, :CCITT F.69
 *                    WMS_NUMBER_PLAN_RESERVED_5  = 5,
 *                    WMS_NUMBER_PLAN_RESERVED_6  = 6,
 *                    WMS_NUMBER_PLAN_RESERVED_7  = 7,
 *                    WMS_NUMBER_PLAN_RESERVED_8  = 8,
 *                    WMS_NUMBER_PLAN_PRIVATE     = 9,
 *
 * addrLen          - Length of Address
 *
 * szAddress        - Destination address if we are sending out a MO SMS
 *                    Origination address if we are retrieving a MT SMS
 *
 * subType          - Sub address type
 *
 * subOdd           - Sub Address Odd byte
 *
 * subLen           - Length of the sub-address
 *
 * szSubAddress     - Destination sub-address if we are sending out a MO SMS
 *                    Origination sub-address if we are retrieving a MT SMS
 *
 * callBackLen      - Length of the Callback number
 *
 * szCallBack       - String containing the Call Back number with a 32 maximum characters
 *
 * ePriority        - Message Priority
 *                    See LITE_SMSHELPER_TYPE_SMS_Priority
 *
 * ePrivacy         - Message Privacy Level
 *                    See LITE_SMSHELPER_TYPE_SMS_Privacy
 *
 * eLanguage        - Message Language
 *                    See LITE_SMSHELPER_TYPE_SMS_Language
 *
 * user_ack_req     - Specify whether user Acknowledgement is required
 *                    True/False
 *
 * dak_req          - Specify whether delivery acknowledge is required.
 *
 * userResp         - User Response Code
 *
 * szNumMsg         - Number of Voice messages (SMSI_VMN_95) on the network
 *
 * deliveryAlert    - Alert on Delivery
 *
 * dataEncoding     - Message Body Data Encoding.
 *                   Refer to LITE_SMSHELPER_TYPE_SMS_DataEncodeType.
 *
 * ts_yr            - Time Stamp Year - the value is presented
 *                    in decimal format.
 *
 * ts_mon           - Time Stamp Month - the value is presented
 *                    in hexadecimal format.
 *                    E.g. December will be interpreted as 0x12
 *
 * ts_day           - Time Stamp Day- the value is presented in
 *                    hexadecimal format.
 *                    E.g. 30th will be interpreted as 0x30.
 *
 * ts_hrs           - Time Stamp Hour - the value is presented in
 *                    hexadecimal format.
 *                    E.g. eleven o'clock is presented as 0x0x11.
 *                    Represented in 24-hour time.
 *
 * ts_min           - Time Stamp Minute - the value is presented
 *                    in hexadecimal format.
 *                    E.g. thirty minute is presented as 0x30.
 *
 * ts_sec           - Time Stamp Second - the value is presented
 *                    in hexadecimal format.
 *                    E.g. thirty minute is presented as 0x30.
 *
 * abVal_yr         - Absolute Validity Year - the value is
 *                    presented in decimal format.
 *
 * abVal_mon        - Absolute Validity Month - the value is
 *                    presented in hexadecimal format.
 *                    E.g. December will be interpreted as 0x12.
 *
 * abVal_day        - Absolute Validity Day- the value is presented
 *                    in hexadecimal format.
 *                    E.g. 30th will be interpreted as 0x30.
 *
 * abVal_hrs        - Absolute Validity Hour - the value is
 *                    presented in hexadecimal format.
 *                    E.g. eleven o'clock is presented as 0x0x11.
 *                    Represented in 24-hour time.
 *
 * abVal_min        - Absolute Validity Minute - the value is
 *                    presented in hexadecimal format.
 *                    Thus, thirty minute is presented as 0x30.
 *
 * abVal_sec        - Absolute Validity Second - the value is
 *                    presented in hexadecimal format.
 *                    E.g. thirty minute is presented as 0x30.
 *
 * relVal_time      - Relative Validity Time of a SMS transmission.
 *                    See TIA/EIA 637-A Section 4.5.6 for more detail.
 *
 * abDef_yr         - Absolute Delivery Deferral Year - the value is
 *                    presented in decimal format.
 *
 * abDef_mon        - Absolute Delivery Deferral Month - the value is
 *                    presented in hexadecimal format.
 *                    E.g. December will be interpreted as 0x12.
 *
 * abDef_day        - Absolute Delivery Deferral Day - the value is presented
 *                    in hexadecimal format.
 *                     E.g. 30th will be interpreted as 0x30.
 *
 * abDef_hrs        - Absolute Delivery Deferral Hour - the value is
 *                    presented in hexadecimal format.
 *                    E.g. eleven o'clock is presented as 0x0x11.
 *                    Represented in 24-hour time.
 *
 * abDef_min        - Absolute Delivery Deferral Minute - the value is
 *                    presented in hexadecimal format.
 *                    E.g. thirty minute is presented as 0x30.
 *
 * abDef_sec        - Absolute Delivery Deferral Second - the value is
 *                    presented in hexadecimal format.
 *                    E.g. thirty minute is presented as 0x30.
 *
 * relDef_time      - Relative Delivery Deferral Time of a SMS transmission.
 *                    See Reference 1 Section 4.5.8 for more detail.
 *
 * eMsgDispMode     - Message Display Mode:
 *                    See LITE_SMSHELPER_TYPE_SMS_MsgDispMode
 *
 */
typedef struct
{
    uint32_t                        sizeStruct;
    LITE_SMSHELPER_TYPE_SMS_ParamMask    eParamMask;
    LITE_SMSHELPER_TYPE_SMS_MsgCategory  eMsgCategory;
    LITE_SMSHELPER_TYPE_SMS_Teleservice  eTeleservice;
    uint16_t                         serviceCategory;
    uint8_t                         tlAckReq;
    LITE_SMSHELPER_TYPE_SMS_MsgIdType    eMsgIdType;
    uint8_t                         addrDigitMode;
    uint8_t                         addrNumMode;
    uint8_t                         addrNumType;
    uint8_t                         addrNumPlan;
    uint8_t                         addrLen;
    uint16_t                         szAddress[MAX_SMSI_TL_ADDRESS];
    uint8_t                         subType;
    uint8_t                         subOdd;
    uint8_t                         subLen;
    uint16_t                         szSubAddress[MAX_SMSI_TL_ADDRESS];
    uint8_t                         callBackLen;
    uint16_t                         szCallBack[MAX_SMSI_CALL_BACK];
    LITE_SMSHELPER_TYPE_SMS_Priority     ePriority;
    LITE_SMSHELPER_TYPE_SMS_Privacy      ePrivacy;
    LITE_SMSHELPER_TYPE_SMS_Language     eLanguage;
    uint8_t                         user_ack_req;
    uint8_t                         dak_req;
    uint8_t                         userResp;
    uint16_t                         szNumMsg[2];
    uint8_t                         deliveryAlert;
    LITE_SMSHELPER_TYPE_SMS_DataEncodeType  dataEncoding;
    uint8_t                         ts_yr;
    uint8_t                         ts_mon;
    uint8_t                         ts_day;
    uint8_t                         ts_hrs;
    uint8_t                         ts_min;
    uint8_t                         ts_sec;
    uint8_t                         abVal_yr;
    uint8_t                         abVal_mon;
    uint8_t                         abVal_day;
    uint8_t                         abVal_hrs;
    uint8_t                         abVal_min;
    uint8_t                         abVal_sec;
    uint8_t                         relVal_time;
    uint8_t                         abDef_yr;
    uint8_t                         abDef_mon;
    uint8_t                         abDef_day;
    uint8_t                         abDef_hrs;
    uint8_t                         abDef_min;
    uint8_t                         abDef_sec;
    uint8_t                         relDef_time;
    LITE_SMSHELPER_TYPE_SMS_MsgDispMode  eMsgDispMode;
}LITE_SMSHELPER_STRUCT_SMS_CdmaHeader;


/*
 * LITE_SMSHELPER_STRUCT_CDMA_SMS_RetrieveSms - SMS Retrieve structure
 * This structure is used to provide input to, and
 * receive output from the SwiRetrieveSMSMessage function.
 *
 * sizeStruct       - size of this structure
 *
 * sHeader          - The SMS header for this message.
 *
 * nSMSId           - The message ID number assigned by
 *                      the network.
 *
 * cntRemainingSMS  - The number of SMS
 *                      messages remaining in the modem (all three
 *                      queues combined).
 *
 * pMessage         - A pointer provided by the application
 *                      in which to place the body of the SMS
 *                      message.
 *
 * sizeBuffer       - The length in bytes of the buffer
 *                    supplied to receive the body of the SMS message.
 *                    This should be based on the maximum size of
 *                    message supported by the network. The API
 *                    currently supports up to 240 bytes, although
 *                    most networks limit this to a lower value.
 *                    retrievedStatus For future use; ignore.
 *                    On return , this parameter will be set to the
 *                    no. of bytes copied into the pMessage buffer
 *
 */
typedef struct
{
    uint32_t                              sizeStruct;
    uint32_t                              bUnreadSMS;      /* Not used */
    LITE_SMSHELPER_STRUCT_SMS_CdmaHeader  sHeader;
    uint16_t                              nSMSId;
    uint32_t                              cntRemainingSMS;
    uint8_t                               *pMessage;
    uint32_t                              sizeBuffer;
    uint32_t                              retrievedStatus;  /* Not used */
}LITE_SMSHELPER_STRUCT_CDMA_SMS_RetrieveSms;

/**
 * @param messageSize
 *          - Upon input, specifies the total number of bytes that the
 *            given pMessage buffer can hold (a buffer of length 240
 *            is recommended).Upon successful output, specifies the
 *            length of the constructed message placed in the pMessage
 *            buffer (in bytes)
 *
 * @param pMessage
 *          - The constructed raw message
 *
 * @param messageId
 *          - The message reference number for this message.
 *            This value should be incremented for every message the host
 *            application sends
 *
 * @param pDestAddr
 *         - Gives NULL-terminated ASCII String containing a destination
 *           address.International number will be prepended with a '+'
 *           character
 *
 * @param pCallbackAddr
 *          - Gives NULL-terminated ASCII String containing a callback
 *            address.International number will be prepended with a '+'
 *            character
 *
 * @param textMsgLength
 *          - Number of UCS2 characters in the text message(excluding NULL)
 *
 * @param pTextMsg
 *          - Text message to be encoded
 *
 * @param pPriority
 *          - Gives the priority of the outgoing message:
 *            0 - normal (default if NULL pointer is given)
 *            1 - interactive
 *            2 - urgent
 *            3 - emergency
 *            64  - 64 is decoded value for URGENT VZAM Support interactive.
 *            128 - 128 is decoded value for URGENT VZAM Support urgent.
 *            192 - 128 is decoded value for URGENT VZAM Support emergency.
 *
 * @param encodingAlphabet
 *          - Upon input, specifies the alphabet the text message
 *            should be encoded in
 *            0 - 8bit ASCII (not supported at this time)
 *            1 - IS91EP (not supported at this time)
 *            2 - 7bit ASCII (default if NULL pointer is given)
 *            3 - IA5 (not supported at this time)
 *            4 - unicode (not supported at this time)
 *            5 - shift JIS (not supported at this time)
 *            6 - korean (not supported at this time)
 *            7 - latin hebrew (not supported at this time)
 *            8 - latin (not supported at this time)
 *            9 - GSM 7 bit default
 *            Upon successful output, specifies the alphabet
 *            used to encode the message.
 *
 * @param pRelValidity
 *          - Gives the relative validity period of the outgoing message
 *            0 - Set Relative validity to 11
 *            1 - Set Relative validity to 71
 *            2 - Set Relative validity to 167
 *            3 - Set Relative validity to 169
 *            4 - Set Relative validity to 171
 *            Values have the following meanings:
 *            0 to 143: validity period = (value + 1)* 5 minutes
 *            144 to 167: validity period = 12 hours + (value - 143)*30
 *            minutes
 *            168 to 196: validity period = (value - 166) * 1 day
 *            197 to 244: validity period = (value - 192) * 1 week
 *            245: validity period = indefinite
 *
 * @Notes   Currently only encoding of 7bit ASCII messages is supported.
 *
 */
typedef struct{
    uint8_t   messageSize;
    uint8_t   *pMessage;
    uint8_t   messageId;
    int8_t    *pDestAddr;
    int8_t    *pCallbackAddr;
    uint32_t  textMsgLength;
    uint16_t  *pTextMsg;
    uint8_t   *pPriority;
    uint8_t   encodingAlphabet;
    uint8_t   *pRelValidity;
} sms_SLQSCDMAEncodeMOTextMsg_t;

/**
 * This function can be used to encode a CDMA SMS message
 * \param  pReqParam
 *          - See @ref sms_SLQSCDMAEncodeMOTextMsg_t for more information
 */
int sms_SLQSCDMAEncodeMOTextMsg(sms_SLQSCDMAEncodeMOTextMsg_t *pReqParam);

/**
 * @param messageSize
 *          - Upon input, specifies the total number of bytes of pTextMsg,
 *            Upon successful output, it is length of the constructed 
 *            message placed in the PDUMessage buffer (in bytes)
 *
 * @param pDestAddr
 *          - Gives NULL-terminated ASCII String containing destination address
 *
 * @param pTextMsg
 *          - Text message to be encoded, maximum limit is 160 charaters
 *
 * @param PDUMessage
 *          - Encoded PDU message
 *
 * @param alphabet
 *          - Encoding method to generate the PDU
 *              - 0 - 7 bit encoding
 *              - 4 - 8 bit encoding
 *              - 8 - 16 bit UCS2 encoding
 *              - others value will be treated as default 7 bit encoding
 *
 */
typedef struct{
    uint32_t   messageSize;
    int8_t     *pDestAddr;
    int8_t     *pTextMsg;
    int8_t     PDUMessage[400];
    uint8_t    alphabet;
} sms_SLQSWCDMAEncodeMOTextMsg_t;

/**
 * This function can be used to encode a WCDMA SMS message
 * \param  pReqParam
 *          - See @ref sms_SLQSWCDMAEncodeMOTextMsg_t for more information
 */
 int sms_SLQSWCDMAEncodeMOTextMsg(sms_SLQSWCDMAEncodeMOTextMsg_t *pReqParam);

/**
  * @param messageLength
  *          - Length of the message to be decoded in bytes
  *
  * @param pMessage
  *          - Message read off the device via GetSMS
  *
  * @param messageID
  *          - Message reference number for this message
  *
  * @param senderAddrLength
  *          - Upon input, indicates the maximum number of ASCII
  *            characters (including NULL termination) that the
  *            pSenderAddr buffer can accommodate.  Note that a
  *            length of 14 is reasonable.Upon successful output,
  *            returns the length of originating address string
  *           (including the NULL termination)
  *
  * @param senderAddr
  *          - Returns NULL-terminated ASCII String containing the
  *            originating address.International number will be
  *            prepended with a '+'character
  *
  * @param textMsgLength
  *          - Upon input, specifies the number of UCS2 characters the
  *            given text message buffer can accommodate.Upon successful
  *            output, returns the number of UCS2 characters returns in
  *            the given text messagebuffer(including NULL-terminator)
  *
  * @param textMsg
  *          - Returns the text message as  NULL-terminated UCS2 string
  *
  * @param priority
  *          - Returns the priority setting of the message
  *            0x00 - normal
  *            0x01 - interactive
  *            0x02 - urgent
  *            0x03 - emergency
  *            0xFF - unavailable setting
  *
  * @param privacy
  *          - Returns the privacy setting of the message
  *            0x00 - not restricted
  *            0x01 - restricted
  *            0x02 - confidential
  *            0x03 - secret
  *            0xFF - unavailable setting
  *
  * @param language
  *          - Returns the language setting of the message
  *            0x00 - unspecified
  *            0x01 - english
  *            0x02 - french
  *            0x03 - spanish
  *            0x04 - japanese
  *            0x05 - korean
  *            0x06 - chinese
  *            0x07 - hebrew
  *            0xFF - unavailable setting
  *
  * @param mcTimeStamp
  *          - Returns the message center timestamp which takes the form:
  *            YYMMDDHHMMSSTZ where
  *            YY - year
  *            MM - month
  *            DD - day
  *            HH - hour
  *            MM - minute
  *            SS - second
  *            TZ - timezone
  *            All values are in decimal. Timezone is in relation to GMT,
  *            one unit is equal to 15 minutes and MSB indicates a
  *            negative value.If this information is unavailable for
  *            message then this field will be filled with 0xFF
  *
  * @param absoluteValidity
  *          - Returns the absolute validity period setting for this
  *            message.This field takes the same form as mcTimeStamp
  *
  * @param relativeValidity
  *          - Returns the relative validity period.Values have the
  *            following meanings:
  *            0 to 143: validity period =(value + 1)* 5 minutes
  *            144 to 167: validity period =12 hours+(value - 143)*30
  *            minutes
  *            168 to 196: validity period = (value - 166) * 1 day
  *            197 to 244: validity period = (value - 192) * 1 week
  *            245: validity period = indefinite
  *            246: validity period = immediate
  *            247: validity period = valid until mobile becomes inactive
  *            248: validity period = valid until registration area
  *            changes
  *            249 to 254: reserved
  *            255: unavailable information
  *
  * @param displayMode
  *          - Returns the display mode parameter
  *            0x00 - immediate display
  *            0x01 - mobile default setting
  *            0x02 - user invoked
  *            0x03 - reserved
  *            0xFF - unavailable parameter
  *
  * @param userAcknowledgementReq
  *          - Returns the user (manual) acknowledgment request parameter
  *            TRUE - means the user is requested to manually acknowledge
  *                   the delivery of the message.
  *            FALSE - means no such user acknowledgement is requested
  *
  * @param readAcknowledgementReq
  *          - Returns the read acknowledgement request parameter
  *            TRUE - means acknowledgment of the message being viewed is
  *                   requested.
  *            FALSE - means no such read acknowledgement is requested
  *
  * @param alertPriority
  *          - Returns the alerting parameter setting
  *            0x00 - use default alert
  *            0x01 - use low priority alert
  *            0x02 - use medium priority alert
  *            0x03 - use high priority alert
  *            0xFF - unavailable parameter
  *
  * @param callbkAddrLength
  *          - returns the length of Callback
  *            address string (including the NULL termination)
  *
  * @param callbkAddr
  *          - returns NULL-terminated ASCII String containing callback
  *            address String containing the Call Back number with a 32
  *            maximum characters.
  *
  */
typedef struct{
     uint32_t  messageLength;
     uint8_t   *pMessage;
     uint32_t  messageID;
     uint8_t   senderAddrLength;
     int8_t    senderAddr[MAX_SMSC_LENGTH+1];
     uint8_t   textMsgLength;
     uint16_t  textMsg[162];
     uint8_t   priority;
     uint8_t   privacy;
     uint8_t   language;
     sms_timestemp mcTimeStamp;
     sms_timestemp absoluteValidity;
     uint8_t   relativeValidity;
     uint8_t   displayMode;
     uint8_t   userAcknowledgementReq;
     uint8_t   readAcknowledgementReq;
     uint8_t   alertPriority;
     uint8_t   callbkAddrLength;
     int8_t    callbkAddr;
 } sms_SLQSCDMADecodeMTTextMsg_t;

/**
 * This function can be used to decode a received CDMA SMS message
 * \param  pReqParam
 *          - See @ref sms_SLQSCDMADecodeMTTextMsg_t for more information
 */
 int sms_SLQSCDMADecodeMTTextMsg(sms_SLQSCDMADecodeMTTextMsg_t *pReqParam);

 /**
  * @param pMessage
  *          - Message read off the device via SLQSGetSMS
  *
  * @param MessageLen
  *          - Message buffer Length
  *
  * @param senderAddrLength
  *          - Upon input, indicates the maximum number of ASCII characters
  *            (including NULL termination) that the pSenderAddr buffer can
  *            accommodate.Note that a length of 14 is reasonable.
  *            Upon successful output, returns the length of destination
  *            address string.
  *
  * @param senderAddr
   *          - Returns NULL-terminated ASCII String containing destination
  *            address
  *
  * @param textMsgLength
  *          - Upon input, specifies the number of characters the given text
  *            message buffer can accommodate. Upon successful output, returns
  *            the number of characters returns in the given text message
  *            buffer.
  *
  * @param textMsg
  *          - Encoded PDU message
  *
  * @param scAddrLength
  *          -Returns NULL-terminated ASCII String containing destination address
  *
  * @param scAddr
  *          - NULL-terminated ASCII String containing service center address
  *
  * @param time
  *          - Time fetched from message
  *
  * @param date
  *          - Date fetched from message
  *
  */
typedef struct{
     uint8_t   *pMessage;
     uint16_t  MessageLen;
     uint8_t   senderAddrLength;
     int8_t    senderAddr[MAX_SMSC_LENGTH+1];
     uint8_t   textMsgLength;
     int8_t    textMsg[MAX_SMS_MSG_LEN+2];
     uint8_t   scAddrLength;
     int8_t    scAddr[MAX_SMSC_LENGTH+1];
     sms_DateTime   time;
     sms_DateTime   date;
 } sms_SLQSWCDMADecodeMTTextMsg_t;

 /**
  * This function can be used to decode a received WCDMA SMS message
  * \param  pReqParam
  *          - See @ref sms_SLQSWCDMADecodeMTTextMsg_t for more information
  */
 int sms_SLQSWCDMADecodeMTTextMsg(sms_SLQSWCDMADecodeMTTextMsg_t *pReqParam);

 /**
  * This function can be used to decode the encoding type from received WCDMA SMS message
  * @param pMessage
  *          - Message data
  * @param MessageLen
  *          - length of received message
  * @param pEncoding
  *          - to store the retrieved encoding type
  * @param pUserDataLen
  *          - length of user data for UCS2 encoding
  *          - this field is valid only if encoding type is retrieved as UCSENCODING in pEncoding
  */
int sms_SLQSWCDMADecodeMTEncoding(
    uint8_t *pMessage,
    uint16_t MessageLen,
    uint8_t  *pEncoding,
    uint16_t *pUserDataLen);

/*
 * This function makes PDU string with input mobile number and message
 *
 * @param number
 *          - destination number
 * @param pMessage
 *          - gsm characters
 * @param messagelen
 *          - length of gsm characters
 * @param pPdu
 *          - output buffer which stored the string in PDU format
 * @param Pdulen
 *          - Length of PDU buffer
 * @param validity
 *          - PDU protocol described field
 * @param alphabet
 *          - encoding scheme
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \notes  none
 */
int sms_SwiMakePdu(
    int8_t   *number,
    int8_t   *pMessage,
    uint16_t messagelen,
    int8_t   *pPdu,
    uint16_t Pdulen,
    uint16_t validity,
    uint8_t   alphabet );

/*
 * This function converts ascii value to hexidecimal value
 *
 * @param pHexa
 *          - destination buffer stored data in hexidecimal format
 * @param HexaLen
 *          - pHexa buffer Length
 * @param pStr
 *          - source buffer stored the ASCII value
 * @param StrLen
 *          - pStr buffer Length
 *
 * \notes  none
 */
void sms_SwiAsc2Hex(uint8_t *pHexa, uint16_t HexaLen ,
    int8_t *pStr, uint16_t StrLen);

/*
 * This function converts ascii character to GSM character
 *
 * @param pMsg
 *          - original message buffer
 * @param msgLen
 *          - length of orginal message
 * @param pGsmChar
 *          - message buffer after converting
 * @param pGsmCharLen
 *          - length of message buffer after converting
 *
 * \notes  none
 */
void sms_ConvertAsciiToGSMCharacter(char *pMsg, int msgLen, char *pGsmChar, uint16_t *pGsmCharLen);

/*
 * This function make PDU string for concatenated sms
 *
 * @param number
 *          - Mobile number entered by user
 * @param pMessage
 *          - message to send
 * @param messagelen
 *          - ength of sms
 * @param pPdu
 *          - Dynamically allocated memory pointer
 *            which will hold final pdu message hex dump
 * @param Pdulen
 *          - Pdu buffer length
 * @param pUdh
 *          - User data header which specifiy the concatenated sms
 * @param alphabet
 *          - message format
 * @param refnumber
 *          - reference number of sms
 * @param totalnumb
 *          - total number of sms
 * @param seqnumber 
 *          - sequence number of sms 
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \notes  none
 */
int sms_SwiMakeConcatenatedSmsPdu(
    char   *number,
    char   *pMessage,
    uint16_t messagelen,
    char   *pPdu,
    uint16_t Pdulen,
    uint8_t   alphabet,
    uint8_t   refnumber,
    uint8_t   totalnumb,
    uint8_t   seqnumber );

/**
 *  Structure contains parameters which need to be decoded from message
 *
 * @param  pMessage[IN]
 *           - Message read off the device via SLQSGetSMS
 *
 * @param  MessageLen[IN]
 *           - Message buffer length
 *
 * @param  pSenderAddrLength[IN/OUT]
 *           - Upon input, indicates the maximum number of ASCII characters
 *             (including NULL termination) that the pSenderAddr buffer can
 *             accommodate. A length with 24 will be much safe
 *             since this address filed can be up to 12 octets (24 bytes)
 *             Upon successful output, returns the length of destination
 *             address string.
 *
 * @param  pSenderAddr[OUT]
 *           - Note that a length with 24 bytes will be much safe. Returns
 *             NULL-terminated ASCII String containing destination address
 *
 *
 * @param  pTextMsgLength[IN/OUT]
 *           - Upon input, specifies the number of characters the given text
 *             message buffer can accommodate. Upon successful output, returns
 *             the number of characters returns in the given text message
 *             buffer.
 *
 * @param  pTextMsg[OUT]
 *           - Encoded PDU message
 *
 * @param  pScAddrLength[IN/OUT]
 *           - A length with 24 will be much safe since this address filed can
 *             be up to 12 octets (24 bytes) Returns NULL-terminated ASCII
 *             String containing destination address
 *
 * @param  pScAddr[OUT]
 *           - Note that a length with 24 bytes will be much safe. Returns
 *             NULL-terminated ASCII String containing service center address.
 *             This SMSC field contains the Type of Address. To get the exact
 *             SMSC address, skip the first two bytes. e.g, 9085290100334, 90
 *             is the Type of Address, indicates international format of phone number
 *
 * @param  pTime[OUT]
 *           - Time fecthed from message
 *
  * @param  pReferenceNum[OUT]
 *           - Reference number of the sms
 *
 * @param  pTotalNum[OUT]
 *           - Total number of the concatenated message
 *
 * @param  pPartNum[OUT]
 *           - Sequence number of the current message
 *
 * @param  pIsUDHPresent
 *           - Is User Data Header Prensent in the PDU? If yes, it means it is a
 *           - concatenated SMS.
 *
 */
typedef struct
{
    uint8_t  *pMessage;
    uint16_t  MessageLen;
    uint8_t  *pSenderAddrLength;
    char  *pSenderAddr;
    uint8_t  *pTextMsgLength;
    char  *pTextMsg;
    uint8_t  *pScAddrLength;
    char  *pScAddr;
    sms_DateTime  Time;
    sms_DateTime  Date;
    uint8_t  *pReferenceNum;
    uint8_t  *pTotalNum;
    uint8_t  *pPartNum;
    int8_t  *pIsUDHPresent;
} sms_SLQSWCDMADecodeLongTextMsg_t;

 /**
  * This function can be used to decode a received WCDMA long SMS message
  * \param  pReqParam
  *          - See @ref sms_SLQSWCDMADecodeLongTextMsg_t for more information
  */
int sms_SLQSWCDMADecodeLongTextMsg(
    sms_SLQSWCDMADecodeLongTextMsg_t *pWcdmaLongMsgDecodingParams );

/**
 * Structure contains parameters which need to be decoded from
 * message
 *
 *  @param  messageLength[IN]
 *          - Length of the message to be decoded in bytes
 *
 *  @param  pMessage[IN]
 *          - Message read off the device via GetSMS
 *
 *  @param  pSenderAddrLength[IN/OUT]
 *          - Upon input, indicates the maximum number of ASCII
 *            characters (including NULL termination) that the
 *            pSenderAddr buffer can accommodate.  Note that a
 *            length of 14 is reasonable.Upon successful output,
 *            returns the length of originating address string
 *           (including the NULL termination)
 *
 *  @param  pSenderAddr[OUT]
 *          - Returns NULL-terminated ASCII String containing the
 *            originating address.International number will be
 *            prepended with a '+'character
 *
 *  @param  pTextMsgLength[IN/OUT]
 *          - Upon input, specifies the number of UCS2 characters the
 *            given text message buffer can accommodate.Upon successful
 *            output, returns the number of UCS2 characters returns in
 *            the given text messagebuffer(including NULL-terminator)
 *
 *  @param  pTextMsg[OUT]
 *          - Returns the text message as  NULL-terminated UCS2 string
 *
 *  @param  pPriority[OUT] (optional parameter)
 *          - Returns the priority setting of the message
 *            0x00 - normal
 *            0x01 - interactive
 *            0x02 - urgent
 *            0x03 - emergency
 *            0xFF - unavailable setting
 *
 *  @param  pPrivacy[OUT](optional parameter)
 *          - Returns the privacy setting of the message
 *            0x00 - not restricted
 *            0x01 - restricted
 *            0x02 - confidential
 *            0x03 - secret
 *            0xFF - unavailable setting
 *
 *  @param  pLanguage[OUT] (optional parameter )
 *          - Returns the language setting of the message
 *            0x00 - unspecified
 *            0x01 - english
 *            0x02 - french
 *            0x03 - spanish
 *            0x04 - japanese
 *            0x05 - korean
 *            0x06 - chinese
 *            0x07 - hebrew
 *            0xFF - unavailable setting
 *
 *  @param  mcTimeStamp[8][OUT] (optional parameter)
 *          - Returns the message center timestamp which takes the form:
 *            YYMMDDHHMMSSTZ where
 *            YY - year
 *            MM - month
 *            DD - day
 *            HH - hour
 *            MM - minute
 *            SS - second
 *            TZ - timezone
 *            All values are in decimal. Timezone is in relation to GMT,
 *            one unit is equal to 15 minutes and MSB indicates a
 *            negative value.If this information is unavailable for
 *            message then this field will be filled with 0xFF
 *
 *  @param  absoluteValidity[8][OUT] (optional parameter)
 *          - Returns the absolute validity period setting for this
 *            message.This field takes the same form as mcTimeStamp
 *
 *  @param  pRelativeValidity[OUT] (optional parameter)
 *          - Returns the relative validity period.Values have the
 *            following meanings:
 *            0 to 143: validity period =(value + 1)* 5 minutes
 *            144 to 167: validity period =12 hours+(value - 143)*30
 *            minutes
 *            168 to 196: validity period = (value - 166) * 1 day
 *            197 to 244: validity period = (value - 192) * 1 week
 *            245: validity period = indefinite
 *            246: validity period = immediate
 *            247: validity period = valid until mobile becomes inactive
 *            248: validity period = valid until registration area
 *            changes
 *            249 to 254: reserved
 *            255: unavailable information
 *
 *  @param  pDisplayMode[OUT] (optional parameter)
 *          - Returns the display mode parameter
 *            0x00 - immediate display
 *            0x01 - mobile default setting
 *            0x02 - user invoked
 *            0x03 - reserved
 *            0xFF - unavailable parameter
 *
 *  @param  pUserAcknowledgementReq[OUT] (optional parameter)
 *          - Returns the user (manual) acknowledgment request parameter
 *            TRUE - means the user is requested to manually acknowledge
 *                   the delivery of the message.
 *            FALSE - means no such user acknowledgement is requested
 *
 *  @param  pReadAcknowledgementReq[OUT] (optional parameter)
 *          - Returns the read acknowledgement request parameter
 *            TRUE - means acknowledgment of the message being viewed is
 *                   requested.
 *            FALSE - means no such read acknowledgement is requested
 *
 *  @param  pAlertPriority[OUT] (optional parameter)
 *          - Returns the alerting parameter setting
 *            0x00 - use default alert
 *            0x01 - use low priority alert
 *            0x02 - use medium priority alert
 *            0x03 - use high priority alert
 *            0xFF - unavailable parameter
 *
 *  @param  pCallbkAddrLength[OUT] (optional parameter)
 *          - returns the length of Callback
 *            address string (including the NULL termination)
 *
 *  @param  pCallbkAddr[OUT] (optional parameter)
 *          - returns NULL-terminated ASCII String containing callback
 *            address String containing the Call Back number with a 32
 *            maximum characters.
 *
 *  @param  pReferenceNum[OUT]
 *          - Reference number of the sms
 *
 *  @param  pTotalNum[OUT]
 *          - Total number of the concatenated message
 *
 *  @param  pPartNum[OUT]
 *          - Sequence number of the current message
 *
 *  @param  pIsUDHPresent
 *          - Is User Data Header Present in the PDU? If yes, it means it is a
 *          - concatenated SMS.
 */
struct sms_cdmaMsgDecodingParamsExt_t
{
    uint32_t messageLength;
    uint8_t  *pMessage;
    uint32_t *pMessageID;
    uint8_t  *pSenderAddrLength;
    char     *pSenderAddr;
    uint8_t  *pTextMsgLength;
    uint16_t *pTextMsg;
    uint8_t  *pPriority;
    uint8_t  *pPrivacy;
    uint8_t  *pLanguage;
    sms_timestemp  mcTimeStamp;
    sms_timestemp  absoluteValidity;
    uint8_t  *pRelativeValidity;
    uint8_t  *pDisplayMode;
    int8_t   *pUserAcknowledgementReq;
    int8_t   *pReadAcknowledgementReq;
    uint8_t  *pAlertPriority;
    uint8_t  *pCallbkAddrLength;
    char     *pCallbkAddr;
    uint16_t  *pReferenceNum;
    uint8_t  *pTotalNum;
    uint8_t  *pPartNum;
    int8_t   *pIsUDHPresent;
};

/**
 * Decodes text message to CDMA PDU message Ext
 *
 *  @param  pCdmaMsgDecodingParamsExt[IN/OUT]
 *          - Pointer to structure containing parameters needed for decoding
 *
 *  @return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 *  @note   Technology Supported: CDMA\n
 *          Timeout: None
 */
 uint32_t sms_SLQSCDMADecodeMTTextMsgExt(
     struct sms_cdmaMsgDecodingParamsExt_t *pCdmaMsgDecodingParamsExt);

#endif
