#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "helper_sms.h"

#define VALID_PDU_MESSAGE_SIZE 184
#define MAX_BINARY_ARRAY_LEN 500
#define UDH_SIZE_WITH_CONCATENATED_ELEMENT 5
#define PDU_FAULT_ENC_2_TXT_UDH_HDR_INDEX 2
#define PDU_FAULT_ENC_2_TXT_UDH_DATA2_INDEX PDU_FAULT_ENC_2_TXT_UDH_HDR_INDEX + 2
#define PDU_FAULT_ENC_2_TXT_REF_NUM_HDR_INDEX 8
#define PDU_FAULT_ENC_2_TXT_REF_NUM_DATA2_INDEX PDU_FAULT_ENC_2_TXT_REF_NUM_HDR_INDEX + 2
#define PDU_FAULT_ENC_2_TXT_TOT_NUM_HDR_INDEX 10
#define PDU_FAULT_ENC_2_TXT_TOT_NUM_DATA2_INDEX PDU_FAULT_ENC_2_TXT_TOT_NUM_HDR_INDEX + 2
#define PDU_FAULT_ENC_2_TXT_SEQ_NUM_HDR_INDEX 12
#define PDU_FAULT_ENC_2_TXT_SEQ_NUM_DATA2_INDEX PDU_FAULT_ENC_2_TXT_SEQ_NUM_HDR_INDEX + 2

#define PDU_ENC_2_TXT_UDH_HDR_INDEX 2
#define PDU_ENC_2_TXT_UDH_DATA2_INDEX PDU_ENC_2_TXT_UDH_HDR_INDEX + 2
#define PDU_ENC_2_TXT_REF_NUM_HDR_INDEX 8
#define PDU_ENC_2_TXT_REF_NUM_DATA2_INDEX PDU_ENC_2_TXT_REF_NUM_HDR_INDEX + 2
#define PDU_ENC_2_TXT_TOT_NUM_HDR_INDEX 10
#define PDU_ENC_2_TXT_TOT_NUM_DATA2_INDEX PDU_ENC_2_TXT_TOT_NUM_HDR_INDEX + 2
#define PDU_ENC_2_TXT_SEQ_NUM_HDR_INDEX 12
#define PDU_ENC_2_TXT_SEQ_NUM_DATA2_INDEX PDU_ENC_2_TXT_SEQ_NUM_HDR_INDEX + 2



#define DEBUG_DECODE_MSG 1
#define FREE_AND_CLEAR_PTR(x) if(x!=NULL) \
               free(x); \
               x = NULL;
#if DEBUG_DECODE_MSG
#define DEBUG_ERROR_PRINT fprintf(stderr,"\n-%s:%d-\n",__FUNCTION__,__LINE__);
#define DEBUG_ERROR_PRINT_INT(x) fprintf(stderr,"\n-%s:%d-"#x":%d\n",__FUNCTION__,__LINE__,x);
#define DEBUG_ERROR_PRINT_STRING(x) fprintf(stderr,"\n-%s:%d-"#x":%s\n",__FUNCTION__,__LINE__,x);
#define DEBUG_ERROR_PRINT_HEX(x,y) \
if(x!=NULL && y > 0) \
{ \
    unsigned _i =0;\
    fprintf(stderr,"\n-%s:%d-"#x"(%d)\n",__FUNCTION__,__LINE__,y); \
    for(_i=0;_i<y;_i++) \
    { \
        fprintf(stderr,"%02x",x[_i]); \
    } \
    fprintf(stderr,"\n"); \
} 
#define DEBUG_ERROR_PRINT_PTR(x) fprintf(stderr,"\n-%s:%d-"#x":%p\n",__FUNCTION__,__LINE__,x);
 
#else
#define DEBUG_ERROR_PRINT 
#define DEBUG_ERROR_PRINT_INT(x) 
#define DEBUG_ERROR_PRINT_STRING(x)
#define DEBUG_ERROR_PRINT_HEX(x,y)
#define DEBUG_ERROR_PRINT_PTR(x)
#endif

/* This structure associates a gsm 7bit default character with its UCS2
   equivalent.*/

typedef struct {

  /*! GSM escape character */
  uint32_t  gsmcode;

  /*! UCS2 character */
  uint32_t unicode;

} sms_UcsGsmCode;

 /* This structure associates with CDMA User Header Information
  * Elements info*/
 typedef struct {
    uint8_t ElementId;
    uint8_t ElementIdLen;
    uint8_t *pElementIdData;
 } sms_UserHdrIEInfo;

 /* This structure associates with CDMA User Data Header Information.*/
  typedef struct {
    uint8_t hdrTotalLen;
    uint8_t numIE;
    sms_UserHdrIEInfo usrHdrIE[255];
 } sms_UserDataHdrInfo;

const sms_UcsGsmCode sms_ucsGsmCode[GSM_CHARACTER_SET_SIZE + GSM_ESC_CHAR_SET_SIZE -1] =
{
  /*gsm    ucs */
  {0x0A, 0x000A},
  {0x010A, 0x000C},
  {0x0D, 0x000D},
  {0x20, 0x0020}, /* SPACE */
  {0x21, 0x0021}, /* EXCLAMATION MARK */
  {0x22, 0x0022}, /* QUOTATION MARK */
  {0x23, 0x0023}, /* NUMBER SIGN */
  {0x02, 0x0024},
  {0x25, 0x0025}, /* PERCENT SIGN */
  {0x26, 0x0026}, /* AMPERSAND */

  {0x27, 0x0027}, /* APOSTROPHE */
  {0x28, 0x0028}, /* LEFT PARENTHESIS */
  {0x29, 0x0029}, /* RIGHT PARENTHESIS */
  {0x2A, 0x002A}, /* ASTERISK */
  {0x2B, 0x002B}, /* PLUS SIGN */
  {0x2C, 0x002C}, /* COMMA */
  {0x2D, 0x002D}, /* HYPHEN-MINUS */
  {0x2E, 0x002E}, /* FULL STOP */
  {0x2F, 0x002F}, /* SOLIDUS */
  {0x30, 0x0030}, /* DIGIT ZERO */

  {0x31, 0x0031}, /* DIGIT ONE */
  {0x32, 0x0032}, /* DIGIT TWO */
  {0x33, 0x0033}, /* DIGIT THREE */
  {0x34, 0x0034}, /* DIGIT FOUR */
  {0x35, 0x0035}, /* DIGIT FIVE */
  {0x36, 0x0036}, /* DIGIT SIX */
  {0x37, 0x0037}, /* DIGIT SEVEN */
  {0x38, 0x0038}, /* DIGIT EIGHT */
  {0x39, 0x0039}, /* DIGIT NINE */
  {0x3A, 0x003A}, /* COLON */

  {0x3B, 0x003B}, /* SEMICOLON */
  {0x3C, 0x003C}, /* LESS-THAN SIGN */
  {0x3D, 0x003D}, /* EQUALS SIGN */
  {0x3E, 0x003E}, /* GREATER-THAN SIGN */
  {0x3F, 0x003F}, /* QUESTION MARK */
  {0x00, 0x0040},
  {0x41, 0x0041}, /* Latin CAPITAL LETTER A */
  {0x42, 0x0042}, /* Latin CAPITAL LETTER B */
  {0x43, 0x0043}, /* Latin CAPITAL LETTER C */
  {0x44, 0x0044}, /* Latin CAPITAL LETTER D */

  {0x45, 0x0045}, /* Latin CAPITAL LETTER E */
  {0x46, 0x0046}, /* Latin CAPITAL LETTER F */
  {0x47, 0x0047}, /* Latin CAPITAL LETTER G */
  {0x48, 0x0048}, /* Latin CAPITAL LETTER H */
  {0x49, 0x0049}, /* Latin CAPITAL LETTER I */
  {0x4A, 0x004A}, /* Latin CAPITAL LETTER J */
  {0x4B, 0x004B}, /* Latin CAPITAL LETTER K */
  {0x4C, 0x004C}, /* Latin CAPITAL LETTER L */
  {0x4D, 0x004D}, /* Latin CAPITAL LETTER M */
  {0x4E, 0x004E}, /* Latin CAPITAL LETTER N */

  {0x4F, 0x004F}, /* Latin CAPITAL LETTER O */
  {0x50, 0x0050}, /* Latin CAPITAL LETTER P */
  {0x51, 0x0051}, /* Latin CAPITAL LETTER Q */
  {0x52, 0x0052}, /* Latin CAPITAL LETTER R */
  {0x53, 0x0053}, /* Latin CAPITAL LETTER S */
  {0x54, 0x0054}, /* Latin CAPITAL LETTER T */
  {0x55, 0x0055}, /* Latin CAPITAL LETTER U */
  {0x56, 0x0056}, /* Latin CAPITAL LETTER V */
  {0x57, 0x0057}, /* Latin CAPITAL LETTER W */
  {0x58, 0x0058}, /* Latin CAPITAL LETTER X */

  {0x59, 0x0059}, /* Latin CAPITAL LETTER Y */
  {0x5A, 0x005A}, /* Latin CAPITAL LETTER Z */
  {0x013C, 0x005B},
  {0x012F, 0x005C},
  {0x013E, 0x005D},
  {0x0114, 0x005E},
  {0x11, 0x005F},
  {0x61, 0x0061}, /* Latin A */
  {0x62, 0x0062}, /* Latin B */
  {0x63, 0x0063}, /* Latin C */

  {0x64, 0x0064}, /* Latin D */
  {0x65, 0x0065}, /* Latin E */
  {0x66, 0x0066}, /* Latin F */
  {0x67, 0x0067}, /* Latin G */
  {0x68, 0x0068}, /* Latin H */
  {0x69, 0x0069}, /* Latin I */
  {0x6A, 0x006A}, /* Latin J */
  {0x6B, 0x006B}, /* Latin K */
  {0x6C, 0x006C}, /* Latin L */
  {0x6D, 0x006D}, /* Latin M */

  {0x6E, 0x006E}, /* Latin N */
  {0x6F, 0x006F}, /* Latin O */
  {0x70, 0x0070}, /* Latin P */
  {0x71, 0x0071}, /* Latin Q */
  {0x72, 0x0072}, /* Latin R */
  {0x73, 0x0073}, /* Latin S */
  {0x74, 0x0074}, /* Latin T */
  {0x75, 0x0075}, /* Latin U */
  {0x76, 0x0076}, /* Latin V */
  {0x77, 0x0077}, /* Latin W */

  {0x78, 0x0078}, /* Latin X */
  {0x79, 0x0079}, /* Latin Y */
  {0x7A, 0x007A}, /* Latin Z */
  {0x0128, 0x007B},
  {0x0140, 0x007C},
  {0x0129, 0x007D},
  {0x013D, 0x007E},
  {0x40, 0x00A1}, /* INVERTED EXCLAMATION MARK */
  {0x01, 0x00A3},
  {0x24, 0x00A4},

  {0x03, 0x00A5},
  {0x5F, 0x00A7},
  {0x60, 0x00BF},
  {0x5B, 0x00C4},
  {0x0E, 0x00C5},
  {0x1C, 0x00C6},
  {0x1F, 0x00C9},
  {0x5D, 0x00D1},
  {0x5C, 0x00D6},
  {0x0B, 0x00D8},

  {0x5E, 0x00DC},
  {0x1E, 0x00DF},
  {0x7F, 0x00E0},
  {0x7B, 0x00E4},
  {0x0F, 0x00E5},
  {0x1D, 0x00E6},
  {0x09, 0x00E7},
  {0x04, 0x00E8},
  {0x05, 0x00E9},
  {0x07, 0x00EC},

  {0x7D, 0x00F1},
  {0x08, 0x00F2},
  {0x7C, 0x00F6},
  {0x0C, 0x00F8},
  {0x06, 0x00F9},
  {0x7E, 0x00FC},
  {0x13, 0x0393},
  {0x10, 0x0394},
  {0x19, 0x0398},
  {0x14, 0x039B},

  {0x1A, 0x039E},
  {0x16, 0x03A0},
  {0x18, 0x03A3},
  {0x12, 0x03A6},
  {0x17, 0x03A8},
  {0x15, 0x03A9},
  {0x0165, 0x20AC},
};
const uint32_t sms_GsmToUcs2[GSM_CHARACTER_SET_SIZE] =
{         /*+0x0        +0x1        +0x2        +0x3        +0x4        +0x5        +0x6        +0x7*/
/*0x00*/    0x40,       0xa3,       0x24,       0xa5,       0xe8,       0xe9,       0xf9,       0xec,
/*0x08*/    0xf2,       0xc7,       0x0a,       0xd8,       0xf8,       0x0d,       0xc5,       0xe5,
/*0x10*/    0x394,      0x5f,       0x3a6,      0x393,      0x39b,      0x3a9,      0x3a0,      0x3a8,
/*0x18*/    0x3a3,      0x398,      0x39e,      0x20,       0xc6,       0xe6,       0xdf,       0xc9,
/*0x20*/    0x20,       0x21,       0x22,       0x23,       0xA4,       0x25,       0x26,       0x27,
/*0x28*/    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
/*0x30*/    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
/*0x37*/    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
/*0x40*/    0xa1,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
/*0x48*/    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
/*0x50*/    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
/*0x58*/    0x58,       0x59,       0x5a,       0xc4,       0xd6,       0xd1,       0xdc,       0xa7,
/*0x60*/    0xbf,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
/*0x68*/    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
/*0x70*/    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
/*0x78*/    0x78,       0x79,       0x7a,       0xe4,       0xf6,       0xf1,       0xfc,       0xe0
};
typedef struct {

  /*! GSM escape character */
  uint32_t  gsmcode;

  /*! UCS2 character */
  uint32_t unicode;

} UcsGsmCode;
const UcsGsmCode sms_EscapedGsm[GSM_ESC_CHAR_SET_SIZE] =
{
  /*gsm    ucs */
  {0x0A, 0x000C},

  {0x14, 0x005E},

  {0x28, 0x007B},
  {0x29, 0x007D},
  {0x2F, 0x005C},

  {0x3C, 0x005B},
  {0x3D, 0x007E},
  {0x3E, 0x005D},

  {0x40, 0x007C},

  {0x65, 0x20AC},
} ;

/*
 * Convert ascii char to digits
 *
 * \param  symbol[IN] - Character which need to be converted in number
 *
 * \return converted number
 *
 * \note   none
 */
static uint8_t sms_Char2Num( int8_t symbol )
{
    uint8_t Num;
    Num = (uint8_t)symbol;

    Num = Num - 48;

    return Num;
}

/*
 * Packs the given byte into the destination at the given offset
 * for the given number of length bits.
 *
 * \param  src[IN]  - Source string.
 *         dst[OUT] - Address of buffer, hold final value.
 *         dstlen[IN] - final value size.
 *         pos[IN]  - Offset in trms of numer of bits.
 *         len[IN]  - Number of length bits to be packed.
 *
 * \return None.
 *
 * \notes  none.
 */
static void sms_PackByteToBits(uint8_t src, 
    uint8_t dst[],
    uint32_t dstlen,
    uint16_t pos, 
    uint16_t len )
{
    uint16_t   t_pos = pos % 8;
    uint16_t   bits  = 8 - t_pos;    
    if( (0 == len) || 
        (0 == dstlen))
    {
        return ;
    }
    if(dstlen*8 <= (uint32_t)(pos+len-1))
    {
        return ;
    }
    dst += (pos+len-1)/8;
        
    if ( bits >= len )
    {
        *dst &= (uint8_t) ~MASK_B(t_pos, len);
        *dst |= (uint8_t) (MASK_B(t_pos, len) & (src << (bits - len)));
    }
    else /* len > bits */
    {
        dst--;
        *dst &= (uint8_t) ~MASK_B(t_pos, bits);
        *dst |= (uint8_t) (MASK_B(t_pos, bits) & (src >> (len - bits)));

        dst++;
        *dst &= (uint8_t) ~MASK_B(0, (len - bits));
        *dst |= (uint8_t) (MASK_B(0, (len - bits)) & (src << (8 - (len - bits))));
    }
}

/*
 * Packs bearerdata to CDMA PDU format.
 *
 * \params  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                               holds final value.
 *          DataLen            - length of bearer data.
 *          Len[IN/OUT]        - On input pointer to the length of allocated
 *                               memory, on output indicate buffer length.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */
static int sms_EncodeBearerData(
    uint8_t  *pPackedBuffer,
    uint8_t  DataLen,
    uint32_t *Len )
{
    uint8_t* pBuf;
    uint32_t bytesPacked = 0;

    if( pPackedBuffer == NULL || Len == NULL  )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if( *Len < 2 )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    pBuf = pPackedBuffer;
    *pBuf++ = BEARER_DATA_IDENTIFIER;
    bytesPacked++;

    *pBuf++ = DataLen;
    bytesPacked++;

    /* Update Len to indicate output buffer length */
    *Len = bytesPacked;

    return sMS_HELPER_OK;
}

/*
 * Make PDU string with input mobile number and message.
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer
 *                              which holds packed header.
 *         relValue[IN]       - pRelValidity (optional pointer)
 *                              Gives the relative validity period
 *                              of the outgoing message
 *                              per per 4.5.6-1 of spec
 *                              0 - Set Relative validity to 11.
 *                              1 - Set Relative validity to 71.
 *                              2 - Set Relative validity to 167.
 *                              3 - Set Relative validity to 169.
 *                              4 - Set Relative validity to 171.
 *                              Values have the following meanings:
 *                              0 to 143: validity period = (value + 1)*
 *                              5 minutes.
 *                              144 to 167: validity period = 12 hours
 *                              + (value - 143)*30 minutes.
 *                              168 to 196: validity period =
 *                              (value - 166) * 1 day.
 *                              197 to 244: validity period =
 *                              (value - 192) * 1 week.
 *                              245: validity period = indefinite.
 *         Len[IN]            - Pointer to Length of Validity header buffer.
 *
 *
 * \return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \note    Use of this function is optional in CDMA message encoding.
 */
static int sms_EncodeRelValidityHdr(
    uint8_t  *pPackedBuffer,
    uint8_t  *relValue,
    uint32_t *Len )
{
    uint8_t* pBuf;
    uint32_t bytesPacked = 0;
    uint8_t  RelValidity = 0;

    if( pPackedBuffer == NULL || Len == NULL )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if( *Len < 3 )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    pBuf = pPackedBuffer;
    *pBuf++ = REL_VAL_PERIOD_SUB_IDENTIFIER;
    bytesPacked++;
    *pBuf++ = REL_VAL_PERIOD_SUB_IDENTIFIER_HDR_LEN;
    bytesPacked++;

    if( relValue )
    {
       RelValidity = *relValue;
    }

    if( RelValidity > 4 )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    switch(RelValidity)
    {
        case 0:
            *pBuf = 11;  /* decimal value as per 4.5.6-1 of spec */
            break;
        case 1:
            *pBuf = 71;  /* decimal value as per 4.5.6-1 of spec */
            break;
        case 2:
            *pBuf = 167; /* decimal value as per 4.5.6-1 of spec */
            break;
        case 3:
            *pBuf = 169; /* decimal value as per 4.5.6-1 of spec */
            break;
        case 4:
            *pBuf = 171; /* decimal value as per 4.5.6-1 of spec */
            break;
    }
    bytesPacked++;

    /* Update Len to indicate output buffer length */
    *Len = bytesPacked;

    return sMS_HELPER_OK;
}

/*
 * Make PDU string with input mobile number and message.
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated memory,
 *                              hold encoded priority.
 *         Priority [IN]      - Gives the priority of the outgoing message:
 *                              0   - normal (default if pointer is NULL).
 *                              1   - interactive.
 *                              2   - urgent.
 *                              3   - emergency.
 *                              64  - 64 is decoded value for URGENT VZAM,
 *                                    Support interactive.
 *                              128 - 128 is decoded value for URGENT VZAM,
 *                                    Support urgent.
 *                              192 - 128 is decoded value for URGENT VZAM
 *                                    Support emergency.
 *         Len [IN] - Pointer to Length of Priority header buffer.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  Default value 3 for length parameter is used,
 *         Use of this function is optional in CDMA message encoding.
 */
static int sms_EncodePriorityHdr(
    uint8_t  *pPackedBuffer,
    uint8_t  Priority,
    uint32_t *Len )
{
    uint8_t *pBuf;
    uint32_t bytesPacked = 0;

    if( pPackedBuffer == NULL || Len == NULL )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if( *Len < 3 )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if( Priority > 3 )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    pBuf = pPackedBuffer;
    *pBuf++ = PRIORIY_SUB_IDENTIFIER;
    bytesPacked++;
    *pBuf++ = PRIORIY_SUB_IDENTIFIER_HDR_LEN;
    bytesPacked++;

    switch(Priority)
    {
        case 0:
            *pBuf = 0;
            break;
        case 1:
            *pBuf = 64;  /* Binary format 01-000000 */
            break;
        case 2:
            *pBuf = 128; /* Binary format 10-000000 */
            break;
        case 3:
            *pBuf = 192; /* Binary format 11-000000 */
            break;
         default:
            *pBuf = 0;
            break;
    }
    bytesPacked++;

    /* Update Len to indicate output buffer len;*/
    *Len = bytesPacked;

    return sMS_HELPER_OK;
}

/*
 * Packs userdata to CDMA PDU format
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                              holds final value.
 *         pszSmsMsg[IN]      - Pointer to sms message.
 *         MsgLen[IN]         - Length of sms message.
 *         Len[IN/OUT]        - On input pointer to the length of allocated
 *                              memory, on output indicate buffer length.
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */
static int sms_EncodeUserDataHdr(
    uint8_t *pPackedBuffer,
    int8_t *pszSmsMsg,
    uint8_t  MsgLen,
    uint32_t *Len )
{
    uint32_t totalbits = 0, temp = 0;
    uint32_t totalbytes = 0;
    int8_t  NumofReserveBits = 0;

    int8_t *pszTempMsg;
    int8_t UserDataId, HdrLen, MsgEncoding, Reserved;
    uint16_t bit_pos = 0;

    if( pPackedBuffer == NULL ||
        Len == NULL ||
        pszSmsMsg == NULL ||
        MsgLen > MAX_SMS_EXTRACT_CONTENT_LENGTH )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* First calculate the number of BYTES need to pack complete information */
    totalbits = MsgLen*7;
    totalbits += 8;
    totalbits += 5;

    temp =  totalbits % 8;

    if( temp == 0)
    {
        /* No padding needed */
        totalbytes = (totalbits/8)+2;
        NumofReserveBits = 0;
    }
    else
    {
        NumofReserveBits = (uint8_t)(8 - (totalbits % 8) );
        totalbits = totalbits + NumofReserveBits;
        /* 1 Byte identifier + 1 Byte Length + Rest of the bytes
           + Reserved bits */
        totalbytes = (totalbits/8)+2;
    }

    if( *Len < totalbytes )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    UserDataId = USER_DATA_SUB_IDENTIFIER;
    sms_PackByteToBits( UserDataId,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;

    HdrLen = (uint8_t)totalbytes - 2;
    sms_PackByteToBits( HdrLen,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;

    MsgEncoding = USER_DATA_MSG_ENCODING_7BIT_ASCII;
    sms_PackByteToBits( MsgEncoding,pPackedBuffer,*Len, bit_pos,5);
    bit_pos += 5;

    sms_PackByteToBits( MsgLen,pPackedBuffer,*Len, bit_pos,8);
    bit_pos += 8;

    pszTempMsg = pszSmsMsg;
    int ii;
    for( ii = 0; ii < MsgLen ; ii++ )
    {
        sms_PackByteToBits( *(pszTempMsg+ii),pPackedBuffer,*Len, bit_pos,7 );
        bit_pos += 7;
    }

    if( NumofReserveBits )
    {
        Reserved = 0;
        sms_PackByteToBits( Reserved,pPackedBuffer,*Len, bit_pos,NumofReserveBits );
    }

    /* Update Len to indicate output buffer length */
    *Len = totalbytes;

    return sMS_HELPER_OK;
}

/*
 * Packs message identifier to CDMA PDU format
 *
 * \params  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                               holds final value.
 *          MsgType            - MsgType.
 *          MsgId              - The message reference number for this.
 *                               message.This value should be incremented.
 *                               for every message the host application sends.
 *          Len[IN/OUT]        - On input pointer to the length of allocated.
 *                               memory, on output indicate buffer length.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */
static int sms_EncodeMsgIdenfier(
    uint8_t   *pPackedBuffer,
    uint8_t   MsgType,
    uint16_t MsgId,
    uint32_t  *Len )
{
    uint8_t  MsgIdentifier, HdrLen, temp;
    uint16_t  bit_pos = 0;
    uint32_t bytesPacked = 0;

    if( pPackedBuffer == NULL || Len == NULL || MsgType > 6  )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* MSG_SUB_IDENTIFIER_HDR_LEN + data length */
    if( *Len < MSG_SUB_IDENTIFIER_HDR_LEN + 2 )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    MsgIdentifier = MSG_SUB_IDENTIFIER_ID;
    sms_PackByteToBits( MsgIdentifier,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    HdrLen = MSG_SUB_IDENTIFIER_HDR_LEN;
    sms_PackByteToBits( HdrLen,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    /* MsgType = 2 is MO-SUBMIT */
    sms_PackByteToBits( MsgType,pPackedBuffer,*Len, bit_pos,4 );
    bit_pos += 4;

    temp  = MsgId >> 8;
    sms_PackByteToBits( temp,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;

    temp = MsgId;
    sms_PackByteToBits( temp,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;

    /* Last 4 bits zero -- reserved */
    sms_PackByteToBits( 0,pPackedBuffer,*Len, bit_pos,4 );

    bytesPacked = bytesPacked + 3;

    /* Update Len to indicate output buffer length */
    *Len = bytesPacked;

    return sMS_HELPER_OK;
}

/*
 * Packs callback number to CDMA PDU format.
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                              holds final value.
 *         pszCallbackAddr[IN]- Pointer to callback address.
 *         CallbackLen        - Length of callback number.
 *         Len[IN/OUT]        - On input pointer to the length of allocated
 *                              memory, on output indicate buffer length.
 *         ReserveBits        - Indicates reserve bits.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */
static int sms_EncodeCallbackNumber(
    uint8_t  *pPackedBuffer,
    int8_t  *pszCallbackAddr,
    uint8_t  CallbackLen,
    uint32_t *PackedBufferLen,
    uint8_t  ReserveBits )
{
    uint8_t CallbackId, HdrLen, DigitMode,NumsOfDigits,Reserved;
    uint16_t  bit_pos = 0;
    uint8_t  *numbers;
    int8_t  *pszNum;
    uint32_t bytesPacked = 0;
    uint8_t  RetNum = 0;
    uint32_t totalbytelength = 1 + 1 + 1 + ( 1 + CallbackLen*4 + ReserveBits)/8 ;

    if( pPackedBuffer == NULL || PackedBufferLen == NULL || pszCallbackAddr == NULL )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(CallbackLen==0)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(( 1 + CallbackLen*4 + ReserveBits)%8!=0)
    {
        totalbytelength++;
    }

    /*check PackedBufferLen */
    if(*PackedBufferLen < totalbytelength )
    {
        /* 8bits + 8 bits + 1 bits + 8bits + NumsOfDigits*4 + ReserveBits */
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    NumsOfDigits = CallbackLen;
    pszNum = pszCallbackAddr;
    numbers = malloc(NumsOfDigits);

    if (numbers != NULL)
    {
        /* Convert ascii char to digits */
        int i;
        for( i = 0; i < NumsOfDigits; i++ )
        {
             RetNum = sms_Char2Num( *(pszNum+i) );

            /*We are defaulting to 4 bit DTMF coding for
            callback phone number. In that coding
            0 is represented as 1010 in binary format.*/

            if( RetNum == 0 )
            {
                *(numbers+i) = 10;
            }
            else
            {
                *(numbers+i) = RetNum;
            }
        }
    }

    CallbackId = CALLBACK_NUM_SUB_IDENTIFIER;
    sms_PackByteToBits( CallbackId,pPackedBuffer,*PackedBufferLen , bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    HdrLen = *PackedBufferLen - 2;
    sms_PackByteToBits( HdrLen,pPackedBuffer,*PackedBufferLen , bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    DigitMode = 0;
    sms_PackByteToBits( DigitMode,pPackedBuffer,*PackedBufferLen , bit_pos,1 );
    bit_pos += 1;

    sms_PackByteToBits( NumsOfDigits,pPackedBuffer,*PackedBufferLen , bit_pos,8 );
    bit_pos += 8;

    if (numbers != NULL)
    {
        /* Pack each digit in 4 bit DTMF format */
        int ii;
        for( ii = 0; ii < NumsOfDigits ; ii++ )
        {
            sms_PackByteToBits( *(numbers+ii),pPackedBuffer,*PackedBufferLen , bit_pos,4 );
            bit_pos += 4;
        }
    }

    if( ReserveBits )
    {
        Reserved = 0;
        sms_PackByteToBits( Reserved,pPackedBuffer,*PackedBufferLen , bit_pos,ReserveBits );
        bit_pos += ReserveBits;
    }

    bytesPacked = (bit_pos/8);

    /* Update Len to indicate output buffer length */
    *PackedBufferLen = bytesPacked;

    if( numbers )
    {
        free(numbers);
    }

    return sMS_HELPER_OK;
}

/*
 * Packs destination address to CDMA PDU format header.
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                              holds final value.
 *         pszDestAddr[IN]    - Pointer to Destination number(Mobile number).
 *         DestAddrLen        - Length of destination number(Mobile number).
 *         Len[IN/OUT]        - On input pointer to the length of allocated
 *                              memory, on output indicate buffer length.
 *         ReserveBits        - Indicates reserve bits.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */
static int sms_EncodeDestAddrHdr(
    uint8_t  *pPackedBuffer,
    int8_t  *pszDestAddr,
    uint8_t  DestAddrLen,
    uint32_t *Len,
    uint8_t  ReserveBits )
{
    uint8_t DestAddrId, HdrLen, NumAndDigitMode, NumsOfDigits, Reserved;
    uint16_t   bit_pos = 0;
    uint8_t  *numbers = NULL;
    int8_t  *pszNum;
    uint32_t bytesPacked = 0;
    uint8_t  RetNum = 0;

    if( pPackedBuffer == NULL ||
        Len == NULL || pszDestAddr == NULL )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    
    NumsOfDigits = DestAddrLen;
    pszNum = pszDestAddr;
    if( (2 > *Len) ||
        ( (uint32_t)(4 + DestAddrLen/2) > *Len))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    
    numbers = malloc(NumsOfDigits);

    if (numbers != NULL)
    {
        /* Convert ascii char to digits */
        int i;
        for(i = 0; i < NumsOfDigits; i++ )
        {
             RetNum = sms_Char2Num( *(pszNum+i) );

            /* We are defaulting to 4 bit DTMF coding for
            Destination phone number. In that coding
            0 is represented as 1010 in binary format */

            if( RetNum == 0 )
            {
                *(numbers+i) = 10;
            }
            else
            {
                *(numbers+i) = RetNum;
            }
        }
    }

    DestAddrId = DESTADDR_IDENTIFIER;  /* For MO-SMS the id = 4; */
    sms_PackByteToBits( DestAddrId,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    /*Len is total memory allocated for Destination Addr buffer */
    HdrLen = (uint8_t)*Len - 2;
    sms_PackByteToBits( HdrLen,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;
    bytesPacked++;

    /* Num Mode and Digit Mode are 1 bit in size and
      set to zero for regular cell phone numbers
      hence copy 2 bits */

    NumAndDigitMode = 0;
    sms_PackByteToBits( NumAndDigitMode,pPackedBuffer,*Len, bit_pos,2 );
    bit_pos += 2;

    sms_PackByteToBits( NumsOfDigits,pPackedBuffer,*Len, bit_pos,8 );
    bit_pos += 8;

    if (numbers != NULL)
    {
        /* Pack each digit in 4 bit DTMF format */
        int ii;
        for(ii = 0; ii < NumsOfDigits ; ii++ )
        {
            sms_PackByteToBits( *(numbers+ii),pPackedBuffer,*Len, bit_pos,4 );
            bit_pos += 4;
        }
    }

    if( ReserveBits )
    {
        Reserved = 0;
        sms_PackByteToBits( Reserved,pPackedBuffer,*Len, bit_pos,ReserveBits );
        bit_pos += ReserveBits;
    }

    bytesPacked = (bit_pos/8);

    /* Update Len to indicate output buffer length */
    *Len = bytesPacked;

    if( numbers )
    {
        free(numbers);
    }

    return sMS_HELPER_OK;
}

/*
 * Packs TeleServiceHdr to CDMA PDU format.
 *
 * \param  pPackedBuffer[OUT] - Pointer to dynamically allocated buffer,
 *                              holds final value.
 *         pLen[IN/OUT]       - On input pointer to the length of allocated
 *                              memory, on output indicate buffer length.
 *
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \note   None.
 */
static int sms_EncodeTeleServiceHdr( uint8_t  *pPackedBuffer,
                                  uint32_t *Len)
{
    uint16_t teleidvalue;
    uint8_t* pBuf;
    uint32_t bytesPacked = 0;
    uint8_t temp = 0;

    if( pPackedBuffer == NULL || Len == NULL )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if( *Len < 5 )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    teleidvalue = TELESERVICE_ID_VALUE;
    pBuf = pPackedBuffer;

    *pBuf++ = POINT_TO_POINT_MSG;
    bytesPacked++;
    *pBuf++ = TELESERVICE_IDENTIFIER;
    bytesPacked++;
    *pBuf++ = TELESERVICE_HDR_LEN;
    bytesPacked++;

    temp = teleidvalue >> 8;
    *pBuf++ = temp;
    bytesPacked++;

    temp = (uint8_t)teleidvalue;
    *pBuf++ = temp;
    bytesPacked++;

    /* Update Len to indicate output buffer length */
    *Len = bytesPacked;

    return sMS_HELPER_OK;
}

/*
 * Encode text message to CDMA PDU fromat.
 *
 * \param  pOut[OUT]
 *         - The constructed raw message.
 * \param  pOutLen [OUT]
 *         - Upon input, specifies the total number of bytes
 *           that the given pOut buffer can hold
 *           (a buffer of length 240 is recommended)Upon
 *           successful output, specifies the length of the
 *           constructed.
 * \param  pDestNum [IN]
 *         - NULL-terminated ASCII String containing a
 *           destination address( mobile number ).
 *
 * \param  DestNumLen [OUT]
 *         - destination address length.
 *
 * \param  pCallbackNum[IN]
 *         - NULL-terminated ASCII String containing a
 *           callback address International number.
 *
 * \param  CallbackNumLen[IN]
 *         - callback address length.
 *
 * \param  pMsgBody[IN]
 *         - Pointer to text sms message.
 *
 * \param  MsgLen[IN]
 *         - Text sms message length.
 *
 * \param  MsgType[IN]
 *         - Indicate type of message.
 *
 * \param  MsgId[IN]
 *         - The message reference number for this message
 *           This value should be incremented for every
 *           message the host application sends.
 *
 * \param  Priority[IN]
 *         - Gives the priority of the outgoing message:
 *           0 - normal (default if NULL pointer is given).
 *           1 - interactive.
 *           2 - urgent.
 *           3 - emergency.
 *           64 - 64 is decoded value for URGENT VZAM Support interactive.
 *           128 - 128 is decoded value for URGENT VZAM Support urgent.
 *           192 - 128 is decoded value for URGENT VZAM Support emergency.
 * \param  RelVal
 *         - Gives the relative validity period of the
 *           outgoing message per 4.5.6-1 of spec
 *           0 - Set Relative validity to 11.
 *           1 - Set Relative validity to 71.
 *           2 - Set Relative validity to 167.
 *           3 - Set Relative validity to 169.
 *           4 - Set Relative validity to 171.
 *               Values have the following meanings:
 *           0 to 143: validity period = (value + 1)* 5
 *                     minutes.
 *           144 to 167: validity period = 12 hours +
 *                      (value - 143)*30 minutes
 *                              168 to 196: validity period = (value - 166) *
 *                               1 day.
 *                              197 to 244: validity period = (value - 192) *
 *                              1 week.
 *                              245: validity period = indefinite.
 *
 * \return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \note    None.
 *
 */
static int sms_EncodeCompleteMoSms(
    uint8_t*  pOut,
    uint8_t* pOutLen,
    int8_t* pDestNum,
    uint8_t DestNumLen,
    int8_t *pCallbackNum,
    uint8_t CallbackNumLen,
    int8_t* pMsgBody,
    uint8_t MsgLen,
    uint8_t MsgType,
    uint16_t MsgId,
    uint8_t Priority,
    uint8_t* RelVal )
{
#define FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS \
    FREE_AND_CLEAR_PTR(TeleServiceBuf); \
    FREE_AND_CLEAR_PTR(DestAddrBuf); \
    FREE_AND_CLEAR_PTR(CallbackAddrBuf); \
    FREE_AND_CLEAR_PTR(MsgIDBuf); \
    FREE_AND_CLEAR_PTR(UserDataBuf); \
    FREE_AND_CLEAR_PTR(PriorityBuf); \
    FREE_AND_CLEAR_PTR(RelValidityBuf); \
    FREE_AND_CLEAR_PTR(BearerDataBuf);

    uint32_t TeleserviceLen = 0, DestAddrLen = 0;
    uint32_t CallbackAddrLen = 0, BearerDataLen = 0;
    uint32_t MsgIDLen = 0, PriorityLen = 0;
    uint32_t UserDataLen = 0, RelValidityLen = 0;
    uint32_t TotalLen = 0;
    uint8_t  BearerMsgLen = 0;

    uint16_t bits = 0, remainder = 0;
    uint8_t reserved = 0;

    uint8_t* pbTemp;
    
    uint8_t* TeleServiceBuf = NULL;
    uint8_t* DestAddrBuf = NULL;
    uint8_t* CallbackAddrBuf = NULL;
    uint8_t* MsgIDBuf = NULL;
    uint8_t* UserDataBuf = NULL;
    uint8_t* PriorityBuf = NULL;
    uint8_t* RelValidityBuf = NULL;
    uint8_t* BearerDataBuf = NULL;
    
    sMSHelperError rCode = sMS_HELPER_OK;

    if( pDestNum == NULL ||
        pMsgBody == NULL ||
        MsgLen > MAX_SMS_MSG_LEN ||
        Priority > 3 )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* check if first digit of destination address needs special handling, application
     * may provide the destination with "+" in front of the dest number, but DTMF Digit
     * does not recognize "+", hence it needs to be deleted in the destination address */
    if (DestNumLen == 0)
    {
        rCode = sMS_HELPER_ERR_INVALID_ARG;
        return rCode;
    }

    // Handle First Digit
    if (pDestNum[0]=='+')
    {
        pDestNum++;
        DestNumLen--;
    }

    /* Teleservice Id  default LEN = 5 */
    TeleserviceLen = 5;
    TeleServiceBuf = (uint8_t*)malloc(TeleserviceLen);
    if( !TeleServiceBuf )
    {
        return sMS_HELPER_ERR_MEMORY;
    }
    memset( TeleServiceBuf, 0, TeleserviceLen );

    rCode = sms_EncodeTeleServiceHdr(TeleServiceBuf, &TeleserviceLen );
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    /* Calculate Destination Addr Len */
    /* ParamID(8bits)+ParamLen(8bits)+NumMode(1bit)+DigitMode(1bit)
       +NumofDigits(8bits) */

    bits = 8+8+1+1+8+(4*DestNumLen);
    remainder = bits%8;
    if( remainder )
    {
        reserved =  8 - remainder;
    }

    bits = bits + reserved;
    DestAddrLen = bits/8;
    DestAddrBuf = (uint8_t*)malloc(DestAddrLen);

    if( !DestAddrBuf )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return sMS_HELPER_ERR_MEMORY;
    }

    memset( DestAddrBuf, 0, DestAddrLen );

    rCode = sms_EncodeDestAddrHdr( DestAddrBuf,
                               pDestNum,
                               DestNumLen,
                               &DestAddrLen,
                               reserved);
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }
    /* Calculate Callback Addr Len */
    if( CallbackNumLen > 0 )
    {
        /* ParamID(8bits)+ParamLen(8bits)+DigitMode(1bit)+NumofDigits(8bits)
           +(NumOfDigits*DTMF format len(4 bits) + reserve bits(if needed)*/
        bits = 8+8+1+8+(4*CallbackNumLen);
        remainder = bits%8;
        if( remainder )
        {
            reserved =  8 - remainder;
        }

        bits = bits + reserved;

        CallbackAddrLen = bits/8;
        CallbackAddrBuf = (uint8_t*)malloc(CallbackAddrLen);

        if( !CallbackAddrBuf )
        {
            FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
            return sMS_HELPER_ERR_MEMORY;
        }

        memset(CallbackAddrBuf,0,CallbackAddrLen);

        rCode = sms_EncodeCallbackNumber(CallbackAddrBuf,
                                     pCallbackNum,
                                     CallbackNumLen,
                                     &CallbackAddrLen,
                                     reserved);
        if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
        {
            FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
            return rCode;
        }
    }
    /* MSG IDENTIFIER default LEN = 5 */
    MsgIDLen = 5;
    MsgIDBuf = (uint8_t*)malloc(MsgIDLen);

    if( !MsgIDBuf )
    {
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset(MsgIDBuf,0,MsgIDLen);

    rCode = sms_EncodeMsgIdenfier(MsgIDBuf,MsgType,MsgId,&MsgIDLen );
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    UserDataLen = 200;
    UserDataBuf = (uint8_t*)malloc(UserDataLen);

    if( !UserDataBuf )
    {
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset( UserDataBuf, 0, UserDataLen );
    rCode = sms_EncodeUserDataHdr(UserDataBuf,pMsgBody,MsgLen, &UserDataLen);
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    /* Priority default LEN = 3 */
    PriorityLen = 3;
    PriorityBuf = (uint8_t*)malloc(PriorityLen);

    if( !PriorityBuf )
    {
        /* LogCritical(_T("Failed to allocate memory") ); */
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset(PriorityBuf,0,PriorityLen);

    rCode = sms_EncodePriorityHdr(PriorityBuf,Priority,&PriorityLen);
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    /* Relative validity default LEN = 3 */

    RelValidityLen = 3;
    RelValidityBuf = (uint8_t*)malloc(RelValidityLen);

    if( !RelValidityBuf )
    {
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset(RelValidityBuf,0,RelValidityLen);

    rCode = sms_EncodeRelValidityHdr(RelValidityBuf,RelVal,&RelValidityLen);
    if( (uint32_t)rCode != (uint32_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }
    /* BearerData default LEN = 2 */
    BearerDataLen = 2;
    BearerDataBuf = (uint8_t*)malloc(BearerDataLen);

    if( !BearerDataBuf )
    {
        rCode = sMS_HELPER_ERR_MEMORY;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset(BearerDataBuf,0,BearerDataLen);

    BearerMsgLen = (uint8_t)(MsgIDLen + UserDataLen + RelValidityLen +
                                            PriorityLen + CallbackAddrLen);
    rCode = sms_EncodeBearerData(BearerDataBuf,BearerMsgLen,&BearerDataLen);
    if( (uint8_t)rCode != (uint8_t)sMS_HELPER_OK )
    {
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    TotalLen = TeleserviceLen + DestAddrLen + MsgIDLen +
               UserDataLen + RelValidityLen + PriorityLen + BearerDataLen +
               CallbackAddrLen;

    if( (TotalLen > (*pOutLen)) ||
        (TotalLen > 255) )
    {
        rCode = sMS_HELPER_ERR_BUFFER_SZ;
        FREE_MSM_ENCODE_COMPLETE_MO_SMS_POINTERS;
        return rCode;
    }

    memset(pOut,0,TotalLen);

    /* Copy layer by layer */
    pbTemp = pOut;

    memcpy(pbTemp, TeleServiceBuf, TeleserviceLen );
    pbTemp += TeleserviceLen;
    free(TeleServiceBuf);

    memcpy(pbTemp, DestAddrBuf, DestAddrLen );
    pbTemp += DestAddrLen;
    free(DestAddrBuf);

    memcpy(pbTemp, BearerDataBuf, BearerDataLen );
    pbTemp += BearerDataLen;
    free(BearerDataBuf);

    memcpy(pbTemp, MsgIDBuf, MsgIDLen );
    pbTemp += MsgIDLen;
    free(MsgIDBuf);

    memcpy(pbTemp, UserDataBuf, UserDataLen );
    pbTemp += UserDataLen;
    free(UserDataBuf);

    memcpy(pbTemp, RelValidityBuf, RelValidityLen );
    pbTemp += RelValidityLen;
    free(RelValidityBuf);
    memcpy(pbTemp, PriorityBuf, PriorityLen );
    pbTemp += PriorityLen;
    free(PriorityBuf);

    if( CallbackAddrLen > 0 )
    {
        memcpy(pbTemp,CallbackAddrBuf,CallbackAddrLen);
        pbTemp += CallbackAddrLen;
    }

    if( NULL != CallbackAddrBuf)
        free(CallbackAddrBuf);

    *pOutLen = TotalLen;
    pbTemp = pOut ;
    return rCode;
}

/*
 * This function can be used to encode an outgoing CDMA message.
 * the resulting message can be either stored on the device using
 * SaveSMS() or sent to the service center using SendSMS().
 * The caller must provide an text message to be encoded, a message
 * ID, a callback address and a destination address.  All other
 * information is optional for the caller to specify.
 * Default values will be used in the encoded when option parameter
 * are not given.
 *
 * \param  pMessageSize[IN/OUT]
 *         -Upon input, specifies the total number of bytes that the
 *          given pMessage buffer can hold (a buffer of length 240
 *          is recommended)Upon successful output, specifies the
 *          length of the constructed message placed in the pMessage
 *          buffer (in bytes)
 *
 * \param  pMessage[OUT]
 *         - The constructed raw message
 *
 * \param  messageId[IN]
 *         - The message reference number for this message.
 *           This value should be incremented for every message the host
 *           application sends
 *
 * \param  pDestAddr[IN]
 *         - Gives NULL-terminated ASCII String containing a destination
 *           address International number will be prepended with a '+'
 *           character
 *
 * \param  pCallbackAddr[IN]
 *         - Gives NULL-terminated ASCII String containing a callback
 *           address International number will be prepended with a '+'
 *           character
 *
 * \param  textMsgLength
 *         - Number of characters in the text message(excluding NULL)
 *
 * \param  pTextMsg[IN]
 *         - text message to be encoded
 *
 * \param  pPriority[IN](optional pointer)
 *         - Gives the priority of the outgoing message:
 *           0 - normal (default if NULL pointer is given)
 *           1 - interactive
 *           2 - urgent
 *           3 - emergency
 *           64 - 64 is decoded value for URGENT VZAM Support interactive
 *           128 - 128 is decoded value for URGENT VZAM Support urgent
 *           192 - 128 is decoded value for URGENT VZAM Support emergency
 *
 * \param  pEncodingAlphabet[IN/OUT](optional pointer)
 *         - Upon input, specifies the alphabet the text message
 *           should be encoded in
 *           0 - 8bit ASCII (not supported at this time)
 *           1 - IS91EP (not supported at this time)
 *           2 - 7bit ASCII (default if NULL pointer is given)
 *           3 - IA5 (not supported at this time)
 *           4 - unicode (not supported at this time)
 *           5 - shift JIS (not supported at this time)
 *           6 - korean (not supported at this time)
 *           7 - latin hebrew (not supported at this time)
 *           8 - latin (not supported at this time)
 *           9 - GSM 7 bit default
 *           Upon successful output, specifies the alphabet
 *           used to encode the message.
 *
 * \param  pRelValidity[IN](optional pointer)
 *         - Gives the relative validity period of the outgoing message
 *           per per 4.5.6-1 of spec
 *           0 - Set Relative validity to 11
 *           1 - Set Relative validity to 71
 *           2 - Set Relative validity to 167
 *           3 - Set Relative validity to 169
 *           4 - Set Relative validity to 171
 *               Values have the following meanings:
 *           0 to 143: validity period = (value + 1)* 5 minutes
 *           144 to 167: validity period = 12 hours + (value - 143)*30
 *                       minutes
 *           168 to 196: validity period = (value - 166) * 1 day
 *           197 to 244: validity period = (value - 192) * 1 week
 *           245: validity period = indefinite
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \note   Currently this API can only encode 7bit ASCII messages.
 *
 */
static int sms_SwiCDMAEncodeMOTextMsg(
    uint8_t    *pMessageSize,
    uint8_t    *pMessage,
    uint8_t    messageId,
    int8_t    *pDestAddr,
    int8_t    *pCallbackAddr,
    uint32_t   textMsgLength,
    uint16_t    *pTextMsg,
    uint8_t    *pPriority,
    uint8_t    *pEncodingAlphabet,
    uint8_t    *pRelValidity )
{
    uint8_t destAddrLen = 0;
    uint8_t callbackAddrLen = 0;
    uint8_t priority = 0;
    int8_t asciiTextMessage[240];
    uint32_t ret;

    if ((pMessage == NULL) || (pMessageSize == NULL)) {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if (sizeof(asciiTextMessage) < textMsgLength)
    {
         return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if(pEncodingAlphabet)
    {
        if( *pEncodingAlphabet != LITE_SMSHELPER_SMS_DATAENCODE_7bitAscii )
        {
            return sMS_HELPER_ERR_GENERAL;
        }
    }

    if(pDestAddr)
    {
        destAddrLen = strlen((char*)pDestAddr);
    }
    if(pCallbackAddr)
    {
        callbackAddrLen = strlen((char*)pCallbackAddr);
    }
    if(pPriority)
    {
        priority = *pPriority;
    }
    memcpy((void*)asciiTextMessage, (void*)pTextMsg, textMsgLength);

    ret  = sms_EncodeCompleteMoSms( pMessage,
                                pMessageSize,
                                pDestAddr,
                                destAddrLen,
                                pCallbackAddr,
                                callbackAddrLen,
                                asciiTextMessage,
                                textMsgLength,
                                2, /* submit */
                                messageId,
                                priority,
                                pRelValidity);
    return ret;
}

int sms_SLQSCDMAEncodeMOTextMsg(
        sms_SLQSCDMAEncodeMOTextMsg_t *pReqParam
        )
{
    int rtn = sMS_HELPER_ERR_ENUM_BEGIN;

    if(pReqParam==NULL)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    rtn =   sms_SwiCDMAEncodeMOTextMsg(
            &pReqParam->messageSize,
            pReqParam->pMessage,
            pReqParam->messageId,
            pReqParam->pDestAddr,
            pReqParam->pCallbackAddr,
            pReqParam->textMsgLength,
            pReqParam->pTextMsg,
            pReqParam->pPriority,
            &pReqParam->encodingAlphabet,
            pReqParam->pRelValidity );

    return rtn;
}

/*
 * Swaps charater of sting passed
 *
 * \param  pStr[IN] - Pointer to string which need to be swapped
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \notes   none
 */
static int sms_Swapchars( int8_t* pStr )
{
    int8_t Length;
    int8_t position;
    int8_t c;
    Length = strlen((char*)pStr);

    /* Length is 0,there are not enough character for swapping */
    if( Length <= 1 )
    {
        return sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED;
    }

    for ( position = 0; position < Length-1; position+=2 )
    {
        c                = pStr[position];
        pStr[position]   = pStr[position+1];
        pStr[position+1] = c;
    }
    return sMS_HELPER_OK;
}

void sms_ConvertAsciiToGSMCharacter(char *pMsg, 
    int msgLen, 
    char *pGsmChar, 
    uint16_t *pGsmCharLen)
{
    uint16_t i;
    uint16_t counted_characters = 0;
    uint16_t character;
    const uint8_t   maxTabLen = GSM_CHARACTER_SET_SIZE + GSM_ESC_CHAR_SET_SIZE -1;
    const int8_t   GSMEscChar = 0x1b;
    if ( (NULL == pMsg) || 
         (NULL == pGsmChar) ||
         (NULL == pGsmCharLen) ||
         (NULL== pGsmCharLen) )
    {
        return ;
    }
    if ( (0 == *pGsmCharLen) || 
         (0 == msgLen) )
    {
        return ;
    }
    memset(pGsmChar, 0, *pGsmCharLen);

    for ( character = 0; character < msgLen; character++ )
    {
        for ( i = 0; i < maxTabLen; i++ )
        {
            if( (uint32_t)pMsg[character] == (sms_ucsGsmCode[i].unicode) )
            {
                if( (sms_ucsGsmCode[i].gsmcode) > 0xFF )
                {
                    /* Insert a GSM escape character( 0x1b ) before the actual byte */
                    pGsmChar[counted_characters] = GSMEscChar;
                    /* Increment numChars by 1 as we added an escape char */
                    counted_characters++;
                }
                pGsmChar[counted_characters] = (char) sms_ucsGsmCode[i].gsmcode;
                counted_characters++;
                break;
            }
        }
        if((*pGsmCharLen-1) <= counted_characters)
        {
            break;
        }
    }
    /* NULL temminated */
    pGsmChar[counted_characters] = 0;
    *pGsmCharLen = counted_characters;
}

/* Converts binary to PDU string, this is basically a hex dump. */
static uint8_t sms_Eightbitpdu(char* binary, int binarylength, 
    char* pdu, uint16_t PduLength, 
    uint8_t *pUdh, uint16_t UdhLength )
{
    int character;
    char octet[10];
    uint8_t i;
    if(PduLength == 0)
    {
        return 0;
    }
    if( (NULL != pUdh) &&
        (LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER != UdhLength) )
    {
        return 0;
    }
    /* Fill the pdu memory buffer with zero */
    memset( pdu, 0, PduLength );

    if (PduLength < (binarylength)*2)
    {
        return 0;
    }
    if (pUdh != NULL)
    {
        if (PduLength < (LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER + binarylength)*2)
        {
            return 0;
        }
        for ( i = 0; i < LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER; i++)
        {
            /* convert the fields of UDH to 2 byte character */
            sprintf(octet,"%02X",pUdh[i]);
            strcat(pdu, octet);
        }
    }

    for( character=0; character<binarylength; character++ )
    {
        sprintf(octet,"%02X",(unsigned char) binary[character]);
        strcat(pdu,octet);
    }

    return (pUdh==NULL? binarylength:(binarylength+LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER));
}

/*
 * Converts ascii text to pdu string
 *
 * \param   pText[IN]   - SMS to send.
 *          length[IN]  - SMS message length.
 *          pPdu[OUT]   - Dynamically allocated memory pointer.
 *                        which will hold final pdu message hex dump.
 *          PduLength[IN] - PDU size.
 *          pUdh[IN]    - user data header to specify the concatenated sms.
 *          UdhLength[IN] - Udh size.
 *
 * \return  pdu string length
 *
 * \notes   none
 */
static uint16_t sms_Text2Pdu(
    int8_t   *pText,
    uint16_t length,
    int8_t   *pPdu,
    uint16_t PduLength,
    uint8_t   *pUdh,
    uint16_t   UdhLength)
{
    int8_t octet[10];
    uint8_t encoded_value;
    uint8_t bits = 0;
    uint16_t i = 0;
    uint8_t paddingBits = 0;
    if ( (pPdu == NULL) || 
         (PduLength==0) )
    {
        return 0;
    }
    if( (NULL != pUdh) &&
        (LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER != UdhLength) )
    {
        return 0;
    }
    /* Fill the pdu memory buffer with zero */
    memset( pPdu, 0, PduLength );
    if(PduLength < (length*2) )
    {
       return 0;
    }
    if (pUdh !=NULL)
    {
        if( (LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER+length)*2 > PduLength)
        {
            return 0;
        }
        for ( i = 0; i < LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER; i++)
        {
            /* convert the fields of UDH to 2 byte character */
            sprintf((char*)octet,"%02X",pUdh[i]);
            strcat((char*)pPdu, (char*)octet);
        }
        /* if PDH is present, we may have to insert the paddings bit to make sure the GSM-7 septets
           are written on a septet boundary  */
        paddingBits = ((pUdh[LITE_SMSHELPER_UDH_LENGTH] + 1) * 8) % 7;
        if (paddingBits)
        {
           paddingBits = 7 - paddingBits;
           bits = 7 - paddingBits;
           encoded_value = pText[0] << (7 - bits);
           sprintf((char*)octet,"%02X",encoded_value);
           strcat((char*)pPdu,(char*)octet);
           bits++;
        }
        
        for( i = 0; i < length; i++ )
        {
           if ( i == length -1)
           {
               encoded_value = pText[i] & 0x7f;
           }
           if( bits == 7 )
           {
               bits = 0;
               continue;
           }
           encoded_value = (pText[i] & 0x7f) >> bits;
           if( i < length - 1 )
           {
               encoded_value |= pText[i + 1] << (7 - bits);
           }
           sprintf((char*)octet,"%02X",encoded_value);
           strcat((char*)pPdu, (char*)octet);
           bits++;
        }
        return (length+1+LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER); /* include the padding and UDH */

    }
    else
    {
        for( i = 0; i < length; i++ )
        {
           if( bits == 7 )
           {
               bits = 0;
               continue;
           }
           encoded_value = (pText[i] & 0x7f) >> bits;
           if( i < length - 1 )
           {
               encoded_value |= pText[i + 1] << (7 - bits);
           }
           sprintf((char*)octet,"%02X",encoded_value);
           strcat((char*)pPdu, (char*)octet);
           bits++;
        }
        return length;
    }

}

/* Converts binary to PDU string, this is basically a hex dump. */
static uint8_t sms_Ucs2pdu(char* binary, int binaryLength,
    char* pdu, uint16_t pduLength, 
    uint8_t *pUdh, uint16_t UdhLength)
{
    int character;
    int length = binaryLength;
    char octet[10];
    uint8_t i;
    if(pduLength==0)
    {
        return 0;
    }
    if( (NULL != pUdh) &&
        (LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER != UdhLength) )
    {
        return 0;
    }
    /* Fill the pdu memory buffer with zero */
    memset( pdu, 0, pduLength );
    if( binaryLength > pduLength )
    {
        length = pduLength;
    }

    if(pduLength < length*2)
    {
        return 0;
    }
    if (pUdh != NULL)
    {
        if(pduLength < (length+LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER)*2)
        {
            return 0;
        }
        for ( i = 0; i < LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER; i++)
        {
            /* convert the fields of UDH to 2 byte character */
            sprintf(octet,"%02X",pUdh[i]);
            strcat(pdu, octet);
        }       
    }
    
    for( character=0; character<length; character++ )
    {
        sprintf(octet,"%02X",(unsigned char) binary[character]);
        strcat(pdu,octet);
    }

    return (pUdh==NULL? length:(length+LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER));
}

/*
 * Make PDU string with input mobile number and message
 *
 * \param  pNumber[IN]     - Mobile number entered by user.
 *         pMessage[IN]   - Sms to send.
 *         messagelen[IN] - Length of sms.
 *         pPdu[OUT]      - Dynamically allocated memory pointer
 *                          which will hold final pdu message hex dump.
 *         PduLength[IN]  - PDU message length.
 *         validity [IN]  - PDU protocol described field.
 *         alphabet [IN]  - Encoding .
 *                          - 0 -Default 7 bit encoding
 *                          - 4 -Eight bit encoding
 *                          - 8 -UCS-2 encoding
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \notes  none
 */
int sms_SwiMakePdu(
    int8_t   *pNumber,
    int8_t   *pMessage,
    uint16_t messagelen,
    int8_t   *pPdu,
    uint16_t PduLength,
    uint16_t validity,
    uint8_t   alphabet )
{
    uint16_t coding = 0; /* for 7bit coding */
    uint16_t flags;
    int8_t   tmp[MAX_SMS_NUMBER_LENGTH+2];
    int8_t   tmppdu[MAXIMUM_PDU];
    uint16_t numberformat;
    uint16_t numberlength;
    uint32_t nRet;
    int8_t buf[MAX_BINARY_ARRAY_LEN];
    uint16_t bufLen = sizeof(buf);
    uint16_t temppduLen = sizeof(tmppdu);

    /* Clear the tmp buffer */
    memset( tmp, 0,sizeof(tmp));

    if ( UCSENCODING == alphabet )
    {
        coding = UCSENCODING; /* UCS bit coding */
    }
    else if ( EIGHTBITENCODING == alphabet )
    {
        coding = EIGHTBITENCODING;
    }
    else
    {
        coding = DEFAULTENCODING;
    }

    if ( !pNumber   ||
         !pMessage ||
         !pPdu )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if ((messagelen > bufLen) && 
        ( ( coding == EIGHTBITENCODING ) || 
          ( coding == UCSENCODING ) ) )
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    if ((coding == UCSENCODING) && 
        (messagelen > MAX_SHORT_SMS_MSG_TOTAL_LEN_UCS))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    else if ((coding == EIGHTBITENCODING) && 
             (messagelen > MAX_SHORT_SMS_MSG_CHAR_LEN_EIGHTBIT))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    else if ((coding == DEFAULTENCODING) && 
             (messagelen > MAX_SHORT_SMS_MSG_CHAR_LEN_DEFAULT))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }

    numberlength = strlen((char*)pNumber);

    if ((numberlength > MAX_SMS_NUMBER_LENGTH) || (numberlength == 0))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    strcpy((char*)tmp,(char*)pNumber);

    if (tmp[0] == '+' || (numberlength > 20))
    {
        numberformat = NUMBERFORMAT;
        numberlength = numberlength -1;
        strcpy((char*)tmp,(char*)&pNumber[1]);
    }
    else
    {
        numberformat = NATIONALFORMAT;
    }

    if(( NULL != (strchr( (char*)tmp, ENTER_KEY )) ) &&
        (numberlength> 1) )
    {
        numberlength = numberlength -1;
        tmp[numberlength] = 0;
    }

    /* terminate the number with F if the length is odd */
    if ( numberlength%2 )
    {
        strcat((char*)tmp,"F");
    }

    /* Swap every second character to make PDU Message */
    nRet = sms_Swapchars(tmp);

    /* SMS-Sumbit MS to SMSC */
    flags = 1;

    /* Validity field */
    flags += 16;

    /* Clear the tmp buffer */
    memset( tmppdu, 0, sizeof(tmppdu));

    /* if it is 7bit or 8bit encoding, check if any GSM extended character presents in the message */
    if ( coding != UCSENCODING )
    {
        if (coding == EIGHTBITENCODING)
        {
            messagelen = sms_Eightbitpdu((char*)pMessage,messagelen,(char*)tmppdu,temppduLen, NULL,0);
        }
        else
        {
            messagelen = sms_Text2Pdu( pMessage,messagelen,tmppdu,temppduLen, NULL ,0);
        }
    }
    else
    {
        messagelen = sms_Ucs2pdu((char*)pMessage,messagelen,(char*)tmppdu,temppduLen, NULL, 0);
    }

    snprintf( (char*)pPdu,PduLength,
             "00%02X00%02X%02X%s00%02X%02X%02X",
             flags,
             numberlength,
             numberformat,
             tmp,
             coding,
             validity,
             messagelen );
    if(PduLength < (strlen((char*)tmppdu)+strlen((char*)pPdu)))
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    /* concatenate the text to the PDU string */
    strcat((char*)pPdu,(char*)tmppdu);
    return nRet;
}

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
 *          - pStr buffer Length.
 *
 * \notes  none
 */
void sms_SwiAsc2Hex( uint8_t *pHexa,
    uint16_t HexaLen,
    int8_t *pStr,
    uint16_t StrLen )
{
    uint16_t asciiCount;
    if ((pHexa == NULL) ||
       (pStr == NULL))
        return;

    for ( asciiCount = 0 ; asciiCount < StrLen ; pStr++, asciiCount++ )
    {
        if(asciiCount<(HexaLen*2))
        {
            pHexa[asciiCount/2] = ((asciiCount & 1) ? pHexa[asciiCount/2] : 0)
            | ( ASC2HEX(*pStr) << ((asciiCount & 1) ? 0 : 4));
        }
        else
        {
            break;
        }
    }
}

int sms_SLQSWCDMAEncodeMOTextMsg(
        sms_SLQSWCDMAEncodeMOTextMsg_t *pReqParam
        )
{
    int rtn = sMS_HELPER_ERR_ENUM_BEGIN;
    int8_t  pdu[SMS_ASCII_STRING_SIZE + 1] = {0};
    char *escaped_msg = NULL;
    uint16_t escaped_len;

    if(!pReqParam ||
       !pReqParam->pDestAddr ||
       !pReqParam->pTextMsg)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if ( strlen((char*)pReqParam->pTextMsg) > MAXSMS_BINARY )
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }

    escaped_msg = (char*)malloc(2 * pReqParam->messageSize);
    if( escaped_msg == NULL )
    {
        printf("%s: failed to allocated %d escape buffer\n", __func__,
                2*pReqParam->messageSize);
        return sMS_HELPER_ERR_MEMORY ;
    }
    escaped_len = 2*pReqParam->messageSize;

    /* If 7 bit default encoding or 8 bit encoding */
    if ((pReqParam->alphabet == DEFAULTENCODING) ||
        (pReqParam->alphabet == EIGHTBITENCODING))
    {
        sms_ConvertAsciiToGSMCharacter((char*)pReqParam->pTextMsg,
                               pReqParam->messageSize,
                               escaped_msg, &escaped_len);
    }
    else if (pReqParam->alphabet == UCSENCODING)
    {
        strcpy(escaped_msg, (char*)pReqParam->pTextMsg);
        escaped_len = pReqParam->messageSize;
    }
    else
    {
        free(escaped_msg);
        printf("%s: Encoding scheme not supported\n", __func__);
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }

    rtn =  sms_SwiMakePdu(pReqParam->pDestAddr,
            (int8_t*)escaped_msg,
            escaped_len,
            pdu,
            SMS_ASCII_STRING_SIZE,
            0,
            pReqParam->alphabet );

    if (rtn == sMS_HELPER_OK) 
    {
        /* Convert string in hex dump */
        sms_SwiAsc2Hex((unsigned char*)pReqParam->PDUMessage,
            sizeof(pReqParam->PDUMessage),
            pdu,strlen((char*)pdu));
        pReqParam->messageSize = strlen((char*)pdu)/2;
    }
    if (escaped_msg)
    {
        free(escaped_msg);
    }
    return rtn;
}

/*
 * Splits CDMA PDU message.
 *
 * \param  pMTSms[IN]              - Pointer bearer Data bufer.
 *
 * \param  MTSmsLen[IN]            - Bearer data bufferLen.
 *
 * \param  TeleServiceIdValue[OUT] - Indicates teleservice ID.
 *
 * \param  pOrigAddrBuf[OUT]       - Points to origin address buffer.
 *
 * \param  pOrigAddrBufLen[OUT]    - Points to origin address data length.
 *
 * \param  pDestAddrBuf[OUT]       - Points to Destination address buffer.
 *
 * \param  pDestAddrBufLen[OUT]    - Points to Destination address data length.
 *
 * \param  BearerReplySeq[OUT]     - Points to bearer reply sequence.
 *
 * \param  pBearerDataBuf[OUT]     - Points to bearer data buffer.
 *
 * \param  BearerDataLen[OUT]      - Points to bearer data length.
 *
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise
 *
 * \note   None
 *
 */
static int sms_SplitParamMTSms(
    uint8_t *pMTSms,
    uint32_t MTSmsLen,
    uint16_t *TeleServiceIdValue,
    uint8_t **pOrigAddrBuf,
    uint8_t *pOrigAddrBufLen,
    uint8_t **pDestAddrBuf,
    uint8_t *pDestAddrBufLen,
    uint8_t *BearerReplySeq,
    uint8_t **pBearerDataBuf,
    uint8_t *BearerDataLen )
{
    if( (pDestAddrBuf==NULL)||
        (pOrigAddrBuf==NULL)||
        (pBearerDataBuf==NULL) || 
        (pMTSms==NULL) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    uint8_t* pMsg = pMTSms;
    uint8_t* pEndMsg = pMsg + MTSmsLen;
    sMSHelperError rc = sMS_HELPER_OK;    

    *pDestAddrBuf = NULL;
    *pOrigAddrBuf = NULL;
    *pBearerDataBuf = NULL;

    uint8_t OriginAddrLen;
    uint8_t DestAddrLen;
    uint16_t SkipLen = 0;

    if( *pMsg != POINT_TO_POINT_MSG )
    {
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }

    pMsg++;

    while ( pMsg < pEndMsg )
    {
        switch(*pMsg)
        {
        case TELESERVICE_IDENTIFIER:
            if((pMsg + TELESERVICE_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            if( *(++pMsg) != TELESERVICE_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            if(TeleServiceIdValue==NULL)
            {
                rc = sMS_HELPER_ERR_INVALID_ARG;
                DEBUG_ERROR_PRINT;
                break;
            }
            *TeleServiceIdValue = *(++pMsg);
            *TeleServiceIdValue <<= 8;
            *TeleServiceIdValue += *(++pMsg);
            break;

        case BEARER_REPLY_IDENTIFIER:
            if((pMsg + BEARER_REPLY_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            if( *(++pMsg) != BEARER_REPLY_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            if(BearerReplySeq==NULL)
            {
                rc = sMS_HELPER_ERR_INVALID_ARG;
                DEBUG_ERROR_PRINT;
                break;
            }
            *BearerReplySeq = *(++pMsg);
            if( *BearerReplySeq == BEARER_INV_SEQ_VALUE)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
            }
            break;

        case ORIGADDR_IDENTIFIER:
            if(*pOrigAddrBuf!=NULL)
            {/* already allocated */
                DEBUG_ERROR_PRINT;
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            OriginAddrLen = *(++pMsg);

            /* OriginAddrLen is not fixed */
            if( OriginAddrLen != ORIGADDR_HDR_LEN )
            {
                
            }
            if(((pMsg + OriginAddrLen ) > pEndMsg) ||
               (OriginAddrLen == 0))
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT
                break;
            }
            if(NULL == pOrigAddrBufLen)
            {
                rc = sMS_HELPER_ERR_INVALID_ARG;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pOrigAddrBufLen = OriginAddrLen;
            *pOrigAddrBuf =  malloc(OriginAddrLen);
            if(*pOrigAddrBuf == NULL )
            {
                rc = sMS_HELPER_ERR_MEMORY;
                DEBUG_ERROR_PRINT;
                break;
            }
            else
            {
                /* increment ptr to point to data */
                ++pMsg;
                memcpy((void*)*pOrigAddrBuf,(void*)pMsg,OriginAddrLen);
                pMsg = pMsg + (OriginAddrLen - 1);
            }
            break;

        case DESTADDR_IDENTIFIER:
            if(*pDestAddrBuf!=NULL)
            {/* already allocated */
                DEBUG_ERROR_PRINT;
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            DestAddrLen = *(++pMsg);

            // DestAddrLen is not fixed
            if( DestAddrLen != DESTADDR_HDR_LEN )
            {
                
            }

            if(((pMsg + DestAddrLen ) > pEndMsg) ||
               (DestAddrLen == 0))
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            if(NULL == pDestAddrBufLen)
            {
                rc = sMS_HELPER_ERR_INVALID_ARG;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pDestAddrBufLen = DestAddrLen;
            *pDestAddrBuf = malloc(DestAddrLen);
            if(*pDestAddrBuf == NULL )
            {
                rc = sMS_HELPER_ERR_MEMORY;
                break;
            }
            else
            {
                /* increment ptr to point to data */
                ++pMsg;
                memcpy((void*)*pDestAddrBuf,(void*)pMsg,DestAddrLen);
                pMsg = pMsg + (DestAddrLen - 1);
            }
            break;

        case BEARER_DATA_IDENTIFIER:
            if(*pBearerDataBuf!=NULL)
            {/* already allocated */
                DEBUG_ERROR_PRINT;
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }
            if(BearerDataLen==NULL)
            {
                rc = sMS_HELPER_ERR_INVALID_ARG;
                break;
            }
            *BearerDataLen = *(++pMsg);

            /* No bearer data */
            if (*BearerDataLen == 0) {
                break;
            }
            else if(*BearerDataLen > (pEndMsg-pMsg) )
            {
                DEBUG_ERROR_PRINT;
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                break;
            }

            *pBearerDataBuf = malloc(*BearerDataLen);
            if(*pBearerDataBuf == NULL )
            {
                rc = sMS_HELPER_ERR_MEMORY;
            }
            else
            {
                /* increment ptr to point to data */
                ++pMsg;
                memcpy((void*)*pBearerDataBuf,(void*)pMsg,(*BearerDataLen));
                pMsg = pMsg + ((*BearerDataLen) - 1);
            }
            break;

        case SERVICE_CATEGORY_IDENIFIER:
        case ORIGSUBADDR_IDENTIFIER:
            if( CDMA_PDU_IDENIFIER_HDR_LENGTH > (pEndMsg-pMsg) )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            SkipLen = *(++pMsg);
            if((pMsg+SkipLen) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            pMsg = pMsg + SkipLen;
            break;

        default:
            rc = sMS_HELPER_ERR_GENERAL;
            break;
        }
        if(rc != sMS_HELPER_OK )
        {
            break;
        }
        else
        {
            pMsg++;
        }
    }
    if(rc != sMS_HELPER_OK )
    {
        DEBUG_ERROR_PRINT
        /* Clean up allocated buffers */
        if(*pDestAddrBuf!=NULL)
        {
            free(*pDestAddrBuf);
        }
        *pDestAddrBuf = NULL;
        if(*pOrigAddrBuf!=NULL)
        {
            free(*pOrigAddrBuf);
        }
        *pOrigAddrBuf = NULL;
        if(*pBearerDataBuf!=NULL)
        {
            free(*pBearerDataBuf);
        }
        *pBearerDataBuf = NULL;
    }
    return rc;
}

/*
 * Unpacks bits to Byte.
 *
 * \param  src[OUT] - Pointer to source string
 *         srcLen[IN] - source string length.
 *         pos[IN]   - Bit position.
 *         len[IN]   - Length in which bits will be grouped.
 *
 *
 * \return Result of unpacking.
 *
 * \note  None.
 *
 */
static uint8_t sms_UnpackBitsToAByte( uint8_t *src, 
        uint8_t srcLen, 
        int pos, 
        int len)
{
    uint8_t result = 0;
    int rshift = 0;
    if(src==NULL)
    {
        return 0;
    }
    if(pos<0)
    {
        return 0;
    }
    if((srcLen*8) < (pos+len))
    {
        return 0;
    }
    src += pos/8;
    pos %= 8;

    rshift = MAX( 8 - (pos + len), 0);

    if ( rshift > 0 )
    {
        result = (uint8_t)MASK_AND_SHIFT(len, pos, rshift, *src);
    }
    else
    {
        result = (uint8_t)MASK(8-pos, pos, *src);
        src++;
        len -= 8 - pos;

        if ( len > 0 )
        {
          result = ( result<<len ) | (*src >> (8-len));
        }
    }

   return result;
}

/*
 * Convert digits to ascii char.
 *
 * \params  Num [IN] - Number which need to be converted in character.
 *
 * \return  Converted character.
 *
 * \notes   none.
 */
static int8_t sms_Num2Char( uint8_t Num )
{
    int8_t c;

    c = (int8_t)Num + 48;

    return c;
}

/*
 * Unpacks origin Address from provided buffer.
 *
 * \param  pOrigAddrBuf[IN]
 *         - Pointer to origin address buffer.
 *
 * \param pOrigAddrBufLen[IN]
 *         - Origin address buffer length.
 *
 * \param  pDigitMode[OUT]
 *         - Pointer to digit mode data.
 *
 * \param  pNumMode[OUT]
 *         - Pointer to number mode data.
 *
 * \param  szOriginAddr[OUT]
 *         - points to origin address.
 *
 * \param  AddrLen[OUT]
 *         - Points to origin address length.
 *
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \note   None.
 *
 */
static int sms_DecodeOriginAddr(
    uint8_t *pOrigAddrBuf,
    uint8_t OrigAddrBufLen,
    uint8_t *pDigitMode,
    uint8_t *pNumMode,
    uint16_t *szOriginAddr,
    uint8_t *AddrLen )
{
    int bit_pos = 0;
    uint8_t Numbers[MAX_SMSI_TL_ADDRESS];
    int8_t szNumbers[MAX_SMSI_TL_ADDRESS] = {0};
    uint8_t rcvdAddrLen;
    if( (pOrigAddrBuf==NULL) ||
        (pDigitMode==NULL) ||
        (pNumMode==NULL) ||
        (szOriginAddr==NULL) ||
        (AddrLen==NULL) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    memset(&Numbers,0,MAX_SMSI_TL_ADDRESS);
    memset(&szNumbers,0,MAX_SMSI_TL_ADDRESS);
    if(OrigAddrBufLen < 2)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    *pDigitMode = sms_UnpackBitsToAByte( pOrigAddrBuf,OrigAddrBufLen,  bit_pos, 1);
    bit_pos += 1;

    *pNumMode = sms_UnpackBitsToAByte( pOrigAddrBuf,OrigAddrBufLen,  bit_pos, 1);
    bit_pos += 1;

    if( *pDigitMode != 0 || *pNumMode != 0 )
    {
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }
    
    rcvdAddrLen = sms_UnpackBitsToAByte( pOrigAddrBuf,OrigAddrBufLen,  bit_pos, 8);
    bit_pos += 8;
    if((OrigAddrBufLen *8) < (1+1+8 + (4*rcvdAddrLen)) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(rcvdAddrLen>MAX_SMSI_TL_ADDRESS)
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if(rcvdAddrLen > *AddrLen)
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    *AddrLen = rcvdAddrLen;  /* Assign the length */
    int i;
    for( i = 0; i < *AddrLen; i++ )
    {
        Numbers[i] = sms_UnpackBitsToAByte( pOrigAddrBuf,OrigAddrBufLen,  bit_pos, 4);
        if( Numbers[i] == 10 )
        {
            Numbers[i] = 0;
        }

        szNumbers[i] = sms_Num2Char( Numbers[i] );
        bit_pos += 4;
    }

    memcpy((void*)szOriginAddr,(void*)szNumbers,*AddrLen);

    return sMS_HELPER_OK;
}

/*
 * Unpacks BearerData
 *
 * @param  pBearerDataBuf[IN]
 *         - Pointer bearer Data bufer.
 *
 * @param  BearerDataLen[IN]
 *         - Bearer Data Length.
 *
 * @param  pMsgType[OUT]
 *         - Pointer to fetched message type.
 *
 * @param  pMsgId[OUT]
 *         - Pointer to Fecthed message ID.
 *
 *  @param  pHdrInd[OUT]
 *          - Pointer to Header Indication, if value is 1 it means it is a
 *          - concatenated SMS.
 *
 * @param  pYr[OUT]
 *         - Pointer to year field in fetched timestamp.
 *
 * @param  pMon[OUT]
 *         - Pointer to month field in fetched timestamp.
 *
 * @param  pDay[OUT]
 *         - Pointer to day field in fetched timestamp.
 *
 * @param  pHr[OUT]
 *         - Pointer to hours field in fetched timestamp.
 *
 * @param  pMin[OUT]
 *         - Pointer to minute field in fetched timestamp.
 *
 * @param  pSec[OUT]
 *         - Pointer to second field in fetched timestam.
 *
 * @param  pUserDataBuf[OUT]
 *         - Pointer user Data bufer.
 *
 * @param  pUserDataLen[OUT]
 *         - Pointer to length of user data buffer.
 *
 * @param  pPriority[OUT]
 *         - Pointer to unpacked priority of message.
 *
 * @param  pCallbackBuf[OUT]
 *         - pointer to callback buffer.
 *
 * @param  pCallbackLen[OUT]
 *         - Pointer to callback length.
 *
 * @return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * @note  None.
 *
 */
static int sms_DecodeBearerData(
    uint8_t  *pBearerDataBuf,
    uint8_t  BearerDataLen,
    uint8_t  *pMsgType,
    uint16_t *pMsgId,
    int8_t   *pHdrInd,
    uint8_t  *pYr,
    uint8_t  *pMon,
    uint8_t  *pDay,
    uint8_t  *pHr,
    uint8_t  *pMin,
    uint8_t  *pSec,
    uint8_t  **pUserDataBuf,
    uint8_t  *pUserDataLen,
    uint8_t  *pPriority,
    uint8_t  **pCallbackBuf,
    uint8_t  *pCallbackLen )
{
    if ( ( pBearerDataBuf == NULL) || 
         ( pMsgType == NULL) || 
         ( pMsgId == NULL) || 
         ( pHdrInd == NULL) || 
         ( pYr == NULL) || 
         ( pMon == NULL) || 
         ( pDay == NULL) || 
         ( pHr == NULL) || 
         ( pMin == NULL) || 
         ( pSec == NULL) ||
         ( pUserDataBuf == NULL) ||
         ( pUserDataLen == NULL) || 
         ( pPriority == NULL) ||
         ( pCallbackBuf == NULL) ||
         ( pCallbackLen == NULL) )
    {
        DEBUG_ERROR_PRINT
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    uint8_t* pMsg = pBearerDataBuf;
    uint8_t* pEndMsg = pMsg + BearerDataLen;
    sMSHelperError rc = sMS_HELPER_OK;

    uint8_t b1, b11, b2, b21, b22, b3, b31;
    uint16_t SkipLen = 0;

    *pUserDataBuf = NULL;
    *pCallbackBuf = NULL;

    if( (pMsg < pEndMsg) && (0) )
    {
        DEBUG_ERROR_PRINT_INT((int)(pEndMsg-pMsg));
        DEBUG_ERROR_PRINT_HEX(pMsg,(unsigned)(pEndMsg-pMsg));
    }
    

    while ( pMsg < pEndMsg )
    {
        switch(*pMsg)
        {
        case MSG_SUB_IDENTIFIER_ID:
            if((pMsg + MSG_SUB_IDENTIFIER_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            if( *(++pMsg) != MSG_SUB_IDENTIFIER_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            uint8_t MsgType, temp;
            b1 = *(++pMsg);
            b2 = *(++pMsg);
            b3 = *(++pMsg);

            /* Mask first 4 bits of b1 variable,first 4 bits is Message type */
            temp = b1 & 0xF0;

            /* Right shift to convert 4 MSBs to LSBs ==>
               11110000 becomes 00001111 */
            MsgType = temp >> 4;
            *pMsgType = MsgType;

            /* Validate the MsgType, set error for "not supported/not mobile terminated" */
            if( MsgType == 7 ||
                MsgType == 8)
            {
                rc = sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            else if( MsgType != 1 &&
                MsgType != 4 &&
                MsgType != 5 &&
                MsgType != 6 )
            {
                rc = sMS_HELPER_ERR_GENERAL;
                DEBUG_ERROR_PRINT;
                break;
            }

            b11 = b1 & 0x0F;  /* Mask Last 4 bits of first byte */
            b11 = b11<<4;     /* Left shift to convert 4 LSB to MSBs
                                 ==> 00001111 becomes 11110000 */

            b31 = b3 & 0xF0;  /* Mask first 4 bits of 3rd byte */
            b31 = b31 >> 4;   /* Right shift to convert 4 MSBs to LSBs
                                 ==> 33330000 becomes 00003333 */

            b21 = b2 & 0xF0;  /* Mask first 4 bits of 2nd byte */
            b21 = b21 >> 4;   /* Right shift to convert 4 MSBs to LSBs
                                 ==> 22220000 becomes 00002222 */

            b22 = b2 & 0x0F;  /* Mask Last 4 bits of 2nd byte */
            b22 = b22 << 4;   /* Left shift to convert 4 LSB to MSBs
                                 ==> 00002222 becomes 22220000 */

            *pMsgId = b11 + b21;
            *pMsgId <<= 8;
            *pMsgId += (b22 + b31);

           /*Extract value of Header Indication*/
            *pHdrInd = (b3 & 0x08) >> 3;
            break;

        case PRIORIY_SUB_IDENTIFIER:
            if((pMsg + PRIORIY_SUB_IDENTIFIER_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }

            if( *(++pMsg) != PRIORIY_SUB_IDENTIFIER_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pPriority = *(++pMsg);

            break;

        case USER_DATA_SUB_IDENTIFIER:
            if (*pUserDataBuf !=NULL)
            {/* already allocated */
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            if((pMsg + 2) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pUserDataLen = *(++pMsg);

            if((pMsg + (*pUserDataLen)) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }

            /* No user data */
            if (*pUserDataLen == 0)
            {
                break;
            }
            else if((pMsg + (*pUserDataLen)) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pUserDataBuf = (uint8_t*) malloc(*pUserDataLen);
            if(*pUserDataBuf == NULL )
            {
                rc = sMS_HELPER_ERR_MEMORY;
                DEBUG_ERROR_PRINT;
                break;
            }
            else
            {
                /* increment ptr to point to data */
                ++pMsg;
                memcpy((void*)*pUserDataBuf,(void*)pMsg,(*pUserDataLen));
                pMsg = pMsg + ((*pUserDataLen) - 1);
            }
            break;

        case MSG_TIMESTAMP_SUB_INDETIFIER:
            if((pMsg + MSG_TIMESTAMP_SUB_INDETIFIER_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }

            if( *(++pMsg) != MSG_TIMESTAMP_SUB_INDETIFIER_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            /* Upper layer expect year to be in decimal format */
            *pYr  = *(++pMsg);
            *pMon = *(++pMsg);
            *pDay = *(++pMsg);
            *pHr  = *(++pMsg);
            *pMin = *(++pMsg);
            *pSec = *(++pMsg);
            break;

        case LANGUAGE_INDICATOR_SUB_INDETIFIER:
            if((pMsg + LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN + 1) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }

            if( *(++pMsg) != LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            pMsg = pMsg + LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN;
            break;

        case CALLBACK_NUM_SUB_IDENTIFIER:
            if (*pCallbackBuf !=NULL)
            {/* already allocated */
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            if((pMsg + 2) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pCallbackLen = *(++pMsg);
            if ( *pCallbackLen == 0)
            {
                break;
            }
            else if((pMsg + (*pCallbackLen)) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            *pCallbackBuf = (uint8_t*) malloc(*pCallbackLen);
            if(*pCallbackBuf == NULL )
            {
                rc = sMS_HELPER_ERR_MEMORY;
                DEBUG_ERROR_PRINT;
            }
            else
            {
                /* increment ptr to point to data */
                ++pMsg;
                memcpy((void*)*pCallbackBuf,(void*)pMsg,*pCallbackLen);
                pMsg = pMsg + (*pCallbackLen - 1);
            }
            break;

        
        case USER_RESP_CODE_SUB_IDENTIFIER:
        case ABS_VAL_PERIOD_SUB_IDENTIFIER:
        case REL_VAL_PERIOD_SUB_IDENTIFIER:
        case DEF_DELIVERY_TIME_ABS_SUB_IDENTIFIER:
        case DEF_DELIVERY_TIME_REL_SUB_IDENTIFIER:
        case PRIVACY_SUB_IDENTIFIER:
        case REPLY_OPTION_SUB_IDENTIFIER:
        case NUM_OF_MSGS_SUB_IDENTIFIER:
        case ALERT_MSG_DELIVERY_SUB_IDENTIFIER:
        case MSG_DISP_MODE_SUB_IDENTIFIER:
        case MULTI_ENCODING_USER_DATA_SUB_IDENTIFIER:
        case MSG_DEPOSIT_INDEX_SUB_IDENTIFIER:
        case SERVICE_CAT_PROGRAM_DATA_SUB_IDENTIFIER:
        case SERVICE_CAT_PROGRAM_RESULT_SUB_IDENTIFIER:
        case MSG_STATUS_SUB_IDENTIFIER:
            if( CDMA_PDU_IDENIFIER_HDR_LENGTH > (pEndMsg-pMsg) )
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            SkipLen = *(++pMsg);
            if((pMsg+SkipLen) > pEndMsg)
            {
                rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                DEBUG_ERROR_PRINT;
                break;
            }
            pMsg = pMsg + SkipLen;
            break;
        default:
            rc = sMS_HELPER_ERR_GENERAL;
            DEBUG_ERROR_PRINT;
            break;
        }

        if(rc != sMS_HELPER_OK )
        {
            break;
        }
        else
        {
            pMsg++;
        }
    }
    if(rc != sMS_HELPER_OK )
    {
        /*Clean up allocated buffers */
        DEBUG_ERROR_PRINT;
        if(*pUserDataBuf != NULL)
        {
            free(*pUserDataBuf);
        }
        *pUserDataBuf = NULL;
        if(*pCallbackBuf != NULL)
        {
            free(*pCallbackBuf);
        }
        *pCallbackBuf = NULL;
    }
    
    return rc;
}

/*
 * Unpacks User Data Header buffer.
 *
 * @param  pUserData[IN]
 *         - Pointer to origin address buffer.
 *
 * @param  pUsrHdrInfo[OUT]
 *         - Pointer to User Header Information.
 *              - See @ref UserDataHdrInfo for more information.
 *
 * @return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * @note   None.
 *
 */
static uint32_t sms_DecodeUserDataHdr(
    uint8_t *pUserData,
    uint8_t UserDataLen,
    sms_UserDataHdrInfo *pUsrHdrInfo)
{
    uint8_t lHdrLen = 0 ;
    uint8_t lcount = 0;

    uint32_t rc = sMS_HELPER_OK;
    if ( (pUserData == NULL) || 
         (pUsrHdrInfo == NULL) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /*Extract total header length*/
    pUsrHdrInfo->hdrTotalLen = *pUserData;
    lHdrLen = pUsrHdrInfo->hdrTotalLen;

    if( UserDataLen < 2 )
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }
    else if ( UserDataLen < lHdrLen )
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    pUserData++;
    if(pUsrHdrInfo->hdrTotalLen)
    {
        while(lHdrLen > SMS_USER_DATA_HDR_LEN)
        {
            /*Extract Element Id*/
             pUsrHdrInfo->usrHdrIE[lcount].ElementId = *pUserData;
             pUserData++;

             /*Extract Element Id Length*/
             pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen = *pUserData;
             pUserData++;
             /*Check Element id length */
             if (pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen > (lHdrLen))
             {
                 DEBUG_ERROR_PRINT_INT(pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen);
                 rc = sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
                 break;
             }

             if (pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen == 0)
             {
                 lHdrLen = lHdrLen - 2;
                 continue;
             }

             pUsrHdrInfo->usrHdrIE[lcount].pElementIdData = (uint8_t*)malloc(pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen);
             if( pUsrHdrInfo->usrHdrIE[lcount].pElementIdData == NULL )
             {
                 rc = sMS_HELPER_ERR_MEMORY;
                 break;
             }

             /*Extract Element Id Data*/
             memcpy(pUsrHdrInfo->usrHdrIE[lcount].pElementIdData,pUserData,pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen);
             pUserData = pUserData + pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen;
             /*Update Header = Element ID (1Byte) + Element ID Length(1Byte)+ Element Data*/
             lHdrLen = lHdrLen - (2 + pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen);
             lcount++;
        }
        pUsrHdrInfo->numIE = lcount;
    }

    if( rc != sMS_HELPER_OK)
    {
        /* clean up allocated data */
        do
        {
            DEBUG_ERROR_PRINT;
            if(pUsrHdrInfo->usrHdrIE[lcount].pElementIdData!=NULL)
            {
                free(pUsrHdrInfo->usrHdrIE[lcount].pElementIdData);
            }
            pUsrHdrInfo->usrHdrIE[lcount].pElementIdData = NULL;
            pUsrHdrInfo->usrHdrIE[lcount].ElementIdLen = 0;
        }while(--lcount != 255);
    }
    return rc;
}

/*
 * Used to convert 7-Bit GSM to ASCII.
 *
 * @param  pInput[IN].
 *         - Pointer to Input buffer.
 *
 * @param  inputLen[IN].
 *         - Input buffer Length.
 *
 * @param  pOut[OUT]
 *         - Pointer to Output buffer.
 *              - See \ref UserDataHdrInfo for more information.
 *
 * @param  OutLen[IN]
 *          - Output buffer Length.
 *
 * \return None.
 *
 * \note   None.
 *
 */
static void sms_convert7bitGsmtoAscii(uint8_t *pInput, 
    uint8_t inputLen, 
    uint8_t *pOut, 
    uint8_t OutLen)
{
    uint32_t i=0;
    uint8_t mask = 0x7F;
    uint8_t mask1;
    uint8_t mask2 = 0;
    uint8_t length = 0, index =0;
    const uint8_t escGsmChar[]  ={0x28, 0x29, 0x14, 0x0A, 0x2F, 0x3C, 0x3D, 0x3E, 0x40};
    const uint8_t escAscii[]=    {0x7B, 0x7D, 0x5E, 0x0C, 0x5C, 0x5B, 0x7E, 0x5D, 0x7C};
    uint8_t GSMEscChar = 0x1b;
    int8_t EscCharFlag = 0;
    uint8_t lout[255];
    uint8_t k = 0;
    uint16_t mismatchFlag = 0;
    const uint8_t SevenBitChar[]  ={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0B, 0x0C, 0x0E, 0x0F, 0x11, 0x1C, 0x1D, 0x1E, 0x1F, 0x24, 0x40,
            0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};
    const uint8_t AsciiChar[]=    {0x40, 0xA3, 0x24, 0xA5, 0xE8, 0xE9, 0xF9, 0xEC, 0xF2,
            0xC7, 0xD8, 0xF8, 0xC5, 0xE5, 0x5F, 0xC6, 0xE6, 0xDF, 0xC9, 0xA4, 0xA1,
            0xC4, 0xD6, 0xD1, 0xDC, 0xA7, 0xBF, 0xE4, 0xF6, 0xF1, 0xFC, 0xE0};

    if ( (NULL == pInput) ||
         (0 == inputLen) ||
         (NULL == pOut) ||
         (0 == OutLen) )
    {
        return;
    }

    for(i=0; i<inputLen; i++)
    {

        mask1 = pInput[i] >> (7-(i%7));   /*extracted MSBs to add as LSBs in next received Byte*/
        mask  = pInput[i] << (i%7);       /*left shift data according to number of byte (0th byte */
        mask = mask | mask2;         /*logical Oring of data with previous byte reserved bits*/
        mask = mask & 0x7F;          /* mask MSB */
        lout[length++] = mask;
        mask2 = mask1;                /*Preserve data for the next byte in another variable*/
        if((i > 0) && ((i+1)%7)==0)     /*If 7 bits are extracted from the previous byte*/
        {
            lout[length++] = mask1;
            mask2 = 0;
        }
        if(length >= OutLen)
        {
            return ;
        }
     }

    lout[length]=0;
    for (i=0 ; i<length; i++)
    {
        if(lout[i] == GSMEscChar)
        {
            EscCharFlag = 1;
        }
        else
        {
            if(EscCharFlag)
            {
                for(index=0;index<sizeof(escGsmChar);index++)
                {
                    if(escGsmChar[index] == lout[i])
                        break;
                }
                if (index == sizeof(escGsmChar))   /* if escape char not found*/
                    pOut[k] = lout[i];
                else
                    pOut[k] = escAscii[index];

                EscCharFlag  = 0;
            }
            else
            {
                for(index=0;index<sizeof(SevenBitChar);index++)
                {
                    if(SevenBitChar[index] == lout[i])
                    {
                         mismatchFlag = 1;
                         break;
                     }
                }
                if(mismatchFlag)
                {
                    pOut[k] = AsciiChar[index];
                    mismatchFlag = 0;
                }
                else
                    pOut[k] = lout[i];
            }
            k++;
            if(k >= OutLen)
            {
                return ;
            }
        }
    }
}

/*
 * Unpacks user data from provided buffer.
 *
 * @param  pUserDataBuf[IN]
 *         - Pointer to origin address buffer.
 *
 * @param  UserDataLen[IN]
 *         - Pointer to digit mode data.
 *
 * @param  TeleServiceIdValue[IN]
 *         - Indicates teleservice ID.
 *
 * @param  pRefNum[OUT]
 *          - Reference number of the sms.
 *
 * @param  pTotalNum[OUT]
 *          - Total number of the concatenated message.
 *
 * @param  pSeqNum[OUT]
 *          - Sequence number of the current message.
 *
 * @param  pHdrInd[IN]
 *          - Is User Data Header Present in the PDU? If yes, it means it is a
 *          - concatenated SMS.
 *
 * @param  pMsgEncoding[OUT]
 *         - Pointer to message encoding type.
 *
 * @param  pMsgLen[OUT]
 *         - points to unpacked message length.
 *
 * @param  pMsgBody[OUT]
 *         - points to unpacked message.
 *
 * @return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * @note   None.
 *
 */
static int sms_DecodeUserData(
    uint8_t  *pUserDataBuf,
    uint8_t  UserDataLen,
    uint16_t TeleServiceIdValue,
    uint16_t* pRefNum,
    uint8_t* pTotalNum,
    uint8_t* pSeqNum,
    int8_t*  pHdrInd,
    uint8_t  *pMsgEncoding,
    uint8_t  *pMsgLen,
    uint8_t  **pMsgBody )
{
#define FREE_ALL_SMS_DECODE_USER_DATA_LOCAL_POINTERS \
                FREE_AND_CLEAR_PTR(l1Refptr); \
                FREE_AND_CLEAR_PTR(l2Refptr); 

#define FREE_ALL_SMS_DECODE_USER_DATA_POINTERS \
            FREE_ALL_SMS_DECODE_USER_DATA_LOCAL_POINTERS; \
            FREE_AND_CLEAR_PTR(*pMsgBody);

    int bit_pos = 0;
    uint8_t* ptr= NULL;
    int abyte_len;
    uint8_t *l1ptr= NULL, *l2ptr = NULL;
    uint8_t *l1Refptr = NULL;
    uint8_t *l2Refptr = NULL;
    uint8_t paddingBitsLen = 0;

    sMSHelperError rc;
    sms_UserDataHdrInfo UsrHdrInfo;
    memset(&UsrHdrInfo,0,sizeof(sms_UserDataHdrInfo));
    UNUSEDPARAM( UserDataLen );

    if ( (pUserDataBuf == NULL) ||
         (pRefNum == NULL) ||
         (pTotalNum == NULL) ||
         (pSeqNum == NULL) ||
         (pHdrInd == NULL) ||
         (pMsgEncoding == NULL) ||
         (pMsgLen == NULL) ||
         (pMsgBody == NULL) )
    {
        DEBUG_ERROR_PRINT;
         return sMS_HELPER_ERR_INVALID_ARG;
    }

    *pMsgBody = NULL;

    if(UserDataLen < 2)
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }
    DEBUG_ERROR_PRINT;
    *pMsgEncoding = sms_UnpackBitsToAByte( pUserDataBuf, UserDataLen,  bit_pos, 5);
    bit_pos += 5;

    *pMsgLen = sms_UnpackBitsToAByte( pUserDataBuf, UserDataLen,  bit_pos, 8);
    bit_pos += 8;

    if(TeleServiceIdValue == 0x1002)
    {
        /*support 7-bit ascii (default) & 8-bit ascii only*/
        if((USER_DATA_MSG_ENCODING_8BIT_OCTET == *pMsgEncoding) ||
           (USER_DATA_MSG_ENCODING_8BIT_LATIN == *pMsgEncoding))
        {
            abyte_len = 8;
        }
        else if (USER_DATA_MSG_ENCODING_7BIT_ASCII == *pMsgEncoding)
        {
            abyte_len = 7;
        }
        else 
        {
            /* encoding not supported */
            printf("Message encoding %d not supported for teleservice id %d\n",
                   *pMsgEncoding, TeleServiceIdValue);
            return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
        }

        if((((*pMsgLen) * abyte_len)/8) > UserDataLen)
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }
        else if(*pMsgLen > 0)
        {
            *pMsgBody = (uint8_t*)malloc(*pMsgLen);

            if( *pMsgBody == NULL )
            {
                return sMS_HELPER_ERR_MEMORY;
            }
            ptr = *pMsgBody;
            int i;
            for( i = 0; i < *pMsgLen; i++ )
            {
                *(ptr+i) = sms_UnpackBitsToAByte( pUserDataBuf, UserDataLen,  bit_pos, abyte_len);
                bit_pos += abyte_len;
            }
        }
    }
    else if(TeleServiceIdValue == 0x1005)
    {
        uint8_t encodedLen = 0 ;

        if(USER_DATA_MSG_ENCODING_7BIT_GSM == *pMsgEncoding )
        {
           /*How many octets needed to get equivalent seplets length*/
            encodedLen = ((*pMsgLen)*7)/8;
            if(((*pMsgLen * 7)%8))
                encodedLen = encodedLen + 1;
            abyte_len = 8;
        }
        else
        {
            /* encoding not supported */
            printf("Message encoding %d not supported for teleservice id %d\n",
                   *pMsgEncoding, TeleServiceIdValue );
            return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
        }

        if( (UserDataLen*8) < (encodedLen*abyte_len))
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        if(*pMsgLen > 0)
        {
            l1ptr = (uint8_t*)malloc(encodedLen);
            if( l1ptr == NULL )
            {
                return sMS_HELPER_ERR_MEMORY;
            }
            l1Refptr = l1ptr;
            int j;
            for( j = 0; j < encodedLen; j++ )
            {
                *(l1ptr+j) = sms_UnpackBitsToAByte( pUserDataBuf, UserDataLen,  bit_pos, abyte_len);
                bit_pos += abyte_len;
            }
            if(*pHdrInd == 1)
            {
                rc = sms_DecodeUserDataHdr(l1ptr, encodedLen, &UsrHdrInfo);
                DEBUG_ERROR_PRINT_INT(rc);
                if(rc == sMS_HELPER_OK )
                {
                    DEBUG_ERROR_PRINT_INT(UsrHdrInfo.numIE);
                    for(j=0; j<UsrHdrInfo.numIE; j++)
                    {
                        /* 8 -bit reference number */
                        if (UsrHdrInfo.usrHdrIE[j].ElementId == 0) {
                            if (UsrHdrInfo.usrHdrIE[j].ElementIdLen >= 3) {
                                uint8_t *lUsrHrdData = NULL;
                                lUsrHrdData = UsrHdrInfo.usrHdrIE[j].pElementIdData;
                                if(lUsrHrdData) 
                                {
                                    *pRefNum   = *lUsrHrdData++;
                                    *pTotalNum = *lUsrHrdData++;
                                    *pSeqNum   = *lUsrHrdData;
                                }
                            }
                        }
                        /* 16 -bit reference number */
                        else if (UsrHdrInfo.usrHdrIE[j].ElementId == 8) {
                            if (UsrHdrInfo.usrHdrIE[j].ElementIdLen >= 4) {
                                uint8_t *lUsrHrdData = NULL;
                                lUsrHrdData = UsrHdrInfo.usrHdrIE[j].pElementIdData;
                                if(lUsrHrdData) 
                                {
                                    *pRefNum   =  ((lUsrHrdData[0] << 8) & 0xFF00) | (lUsrHrdData[1] & 0x00FF);
                                    lUsrHrdData = lUsrHrdData + 2;
                                    *pTotalNum = *lUsrHrdData++;
                                    *pSeqNum   = *lUsrHrdData;
                                }
                            }
                        }
                        if(UsrHdrInfo.usrHdrIE[j].pElementIdData) {
                            free(UsrHdrInfo.usrHdrIE[j].pElementIdData);
                            UsrHdrInfo.usrHdrIE[j].pElementIdData = NULL;
                        }
                    }
                /*TODO : Can process other IE elements of User Data Header */
                }
                else 
                {
                    DEBUG_ERROR_PRINT;
                    FREE_ALL_SMS_DECODE_USER_DATA_POINTERS;
                    return rc;
                }    
                l1ptr = l1ptr + UsrHdrInfo.hdrTotalLen + 1;   /*1 HdrLen */
            }
            uint8_t l1, l2, l3, lcount;
            DEBUG_ERROR_PRINT
            if(UsrHdrInfo.hdrTotalLen >= encodedLen)
            {
                FREE_ALL_SMS_DECODE_USER_DATA_POINTERS;
                return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
            }
            encodedLen = encodedLen - UsrHdrInfo.hdrTotalLen - 1;
            paddingBitsLen = 7 -(((UsrHdrInfo.hdrTotalLen +1) * 8) % 7);
            DEBUG_ERROR_PRINT_INT(UsrHdrInfo.hdrTotalLen);
            if(encodedLen > 0)
                l2ptr = (uint8_t*)malloc(encodedLen);

            if( l2ptr == NULL )
            {
                FREE_ALL_SMS_DECODE_USER_DATA_POINTERS;
                return sMS_HELPER_ERR_MEMORY;
            }
            l2Refptr = l2ptr;
            for (lcount=0; lcount<encodedLen; lcount++)
            {
                l1 = *l1ptr >> paddingBitsLen;
                if(lcount != (encodedLen-1))
                {
                    l2 = *(l1ptr+1) << (8-paddingBitsLen);
                }
                else
                   l2 = 0;
                l3 = l1 | l2;
                *l2ptr = l3;
                l1ptr++;
                l2ptr++;
            }
            *pMsgBody = (uint8_t*)malloc(*pMsgLen);        
            if( *pMsgBody == NULL )
            {
                FREE_ALL_SMS_DECODE_USER_DATA_POINTERS;
                return sMS_HELPER_ERR_MEMORY;
            }
            memset(*pMsgBody,0 ,*pMsgLen);
            ptr = *pMsgBody;
            sms_convert7bitGsmtoAscii(l2Refptr,encodedLen,ptr,*pMsgLen);
            if(l1Refptr)
            {
                free(l1Refptr);
                l1Refptr = NULL;
            }
            if(l2Refptr)
            {
                free(l2Refptr);
                l2Refptr = NULL;
            }
        }
    }
    else 
    {
        FREE_ALL_SMS_DECODE_USER_DATA_POINTERS;
        /* teleservice id not supported */
        printf("teleservice id 0x%x not supported\n",TeleServiceIdValue);
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }
    FREE_ALL_SMS_DECODE_USER_DATA_LOCAL_POINTERS;
    return sMS_HELPER_OK;
}

/*
 * Unpacks CDMA PDU whcih is read using SLQSGetSMS API.
 *
 * \param  pEncodedSms[IN]
 *         - Pointer Message read by SLQSGetSMS.
 *
 * \param  UserDataLen[IN]
 *         - Length of message.
 *
 * \param  pSms[OUT]
 *         - Structure to be filled with unpacked data.
 *
 *  \param  pRefNum[OUT]
 *          - Reference number of the sms.
 *
 *  \param  pTotalNum[OUT]
 *          - Total number of the concatenated message.
 *
 *  \param  pSeqNum[OUT]
 *          - Sequence number of the current message.
 *
 *  \param  pUdhInd[OUT]
 *          - Is User Data Header Present in the PDU? If yes, it means it is a
 *          - concatenated SMS.
 *
 * \return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \note   None
 *
 */ 
static uint32_t sms_DecodeCompleteMTSms(
    uint8_t  *pEncodedSms,
    uint32_t MsgLen,
    LITE_SMSHELPER_STRUCT_CDMA_SMS_RetrieveSms *pSms,
    uint16_t*  pRefNum,
    uint8_t*  pTotalNum,
    uint8_t*  pSeqNum,
    int8_t*  pUdhInd
)
{
#define FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_LOCAL_POINTERS \
            FREE_AND_CLEAR_PTR(pDestAddrBuf); \
            FREE_AND_CLEAR_PTR(pOriginAddrBuf); \
            FREE_AND_CLEAR_PTR(pBearerDataBuf); \
            FREE_AND_CLEAR_PTR(pUserDataBuf); \
            FREE_AND_CLEAR_PTR(pCallbackBuf); 
#define FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS \
            FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_LOCAL_POINTERS; \
            FREE_AND_CLEAR_PTR(pMsgBody);

    uint16_t TeleServiceIdValue = 0;
    uint8_t* pDestAddrBuf = NULL;
    uint8_t* pOriginAddrBuf =NULL;

    /*Bearer Reply field is 6 bits so it cannot be more than 63 */
    uint8_t BearerReplySequence = 255;
    uint8_t* pBearerDataBuf = NULL;
    uint8_t BearerDataLen=0;

    uint8_t DigitMode, NumMode;

    uint16_t MsgId = 0;
    uint8_t MsgType = 0;
    uint8_t Yr = 200, Mon = 200, Day = 200, Hr = 200, Min = 200, Sec = 200;
    uint8_t *pUserDataBuf=NULL,UserDataLen, *pCallbackBuf=NULL, CallbackLen,Priority = 200;
    uint8_t OriginAddrBufLen = 0, DestAddrBufLen = 0;
    uint8_t MsgEncoding, DataMsgLen;
    uint8_t* pMsgBody = NULL;
    int8_t  HdrInd = 0;

    if ( (pEncodedSms == NULL) ||
         (pSms == NULL) ||
         (pTotalNum == NULL) ||
         (pSeqNum == NULL) ||
         (pUdhInd == NULL) )
    {
         return sMS_HELPER_ERR_INVALID_ARG;
    }

    sMSHelperError rc = sms_SplitParamMTSms(pEncodedSms,
                         MsgLen,
                         &TeleServiceIdValue,
                         &pOriginAddrBuf,
                         &OriginAddrBufLen,
                         &pDestAddrBuf,
                         &DestAddrBufLen,
                         &BearerReplySequence,
                         &pBearerDataBuf,
                         &BearerDataLen );

    if(rc != sMS_HELPER_OK )
    {
        FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
        return rc;
    }

    pSms->sHeader.sizeStruct = sizeof(LITE_SMSHELPER_STRUCT_SMS_CdmaHeader);

    /* Lets say there are no params at the beginning */
    pSms->sHeader.eParamMask = LITE_SMSHELPER_SMS_PARAMASK_NO_PARAMS;

    /* If Berer reply was received its value would be less than 255 */
    if( BearerReplySequence != BEARER_INV_SEQ_VALUE )
    {
        pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
               (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_BEARER_REPLY );
        pSms->sHeader.tlAckReq = BearerReplySequence >> 2;
    }

    /* We only support Point to Point messages for now
       If its not P2P this function would have returned
       error before reaching here */
    pSms->sHeader.eMsgCategory = LITE_SMSHELPER_SMS_MSGCAT_POINT_TO_POINT;

    /* Not used anywhere */
    pSms->sHeader.eTeleservice = TeleServiceIdValue;

    /*Don't know anything about this fields yet
    pSms->sHeader.serviceCategory*/

    /* Only proceed if PTR is NOT NULL */
    if(pOriginAddrBuf)
    {

        rc = sms_DecodeOriginAddr( pOriginAddrBuf,
                               OriginAddrBufLen,
                               &DigitMode,
                               &NumMode,
                               pSms->sHeader.szAddress,
                               &(pSms->sHeader.addrLen) );

        if( (uint32_t)rc != (uint32_t)sMS_HELPER_OK )
        {
            FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
            return rc;
        }

        if( pOriginAddrBuf )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                  (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_ADDRESS);
        }

        pSms->sHeader.addrDigitMode = DigitMode;
        pSms->sHeader.addrNumMode = NumMode;
        pSms->sHeader.addrNumType = 0;
        pSms->sHeader.addrNumPlan = 0;

        if(pOriginAddrBuf != NULL )
        {
            free(pOriginAddrBuf);
            pOriginAddrBuf = NULL;
        }
        if(pDestAddrBuf)
        {
            free(pDestAddrBuf);
            pDestAddrBuf = NULL;
        }
    }
    else if(pDestAddrBuf)/* if we want to retrieve a 'Draft' msg */
    {

        rc = sms_DecodeOriginAddr( pDestAddrBuf,
                              DestAddrBufLen,
                              &DigitMode,
                              &NumMode,
                              pSms->sHeader.szAddress,
                              &(pSms->sHeader.addrLen) );

        if( (uint32_t)rc != (uint32_t)sMS_HELPER_OK )
        {
            FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
            return rc;
        }

        if( pDestAddrBuf )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                 (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_ADDRESS);
        }

        pSms->sHeader.addrDigitMode = DigitMode;
        pSms->sHeader.addrNumMode = NumMode;
        pSms->sHeader.addrNumType = 0;
        pSms->sHeader.addrNumPlan = 0;

        if( pDestAddrBuf != NULL )
        {
            free(pDestAddrBuf);
            pDestAddrBuf = NULL;
        }
    }
    else
    {
        if(pBearerDataBuf)
            free(pBearerDataBuf);
        pBearerDataBuf = NULL;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    /* Only proceed if PTR is NOT NULL */
    if( pBearerDataBuf )
    {
        pUserDataBuf = NULL;
        pCallbackBuf = NULL;

        rc = sms_DecodeBearerData( pBearerDataBuf,
                               BearerDataLen,
                               &MsgType,
                               &MsgId,
                               &HdrInd,
                               &Yr,
                               &Mon,
                               &Day,
                               &Hr,
                               &Min,
                               &Sec,
                               &pUserDataBuf,
                               &UserDataLen,
                               &Priority,
                               &pCallbackBuf,
                               &CallbackLen );

        if( (uint32_t)rc != (uint32_t)sMS_HELPER_OK )
        {
            FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
            return rc;
        }
        if( pCallbackBuf )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                           (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_CALL_BACK);
            free(pCallbackBuf);
            pCallbackBuf = NULL;   
        }
        DEBUG_ERROR_PRINT;
        if( Yr != 200 && Mon != 200 )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_MC_TIME_STAMP);
        }

        if( Priority != 200 )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                   (pSms->sHeader.eParamMask | LITE_SMSHELPER_SMS_PARAMASK_PRIORITY );
        }

        if( pUserDataBuf )
        {
            pSms->sHeader.eParamMask = (LITE_SMSHELPER_TYPE_SMS_ParamMask )
                  (pSms->sHeader.eParamMask |LITE_SMSHELPER_SMS_PARAMASK_USER_DATA );
        }

        pSms->sHeader.ePriority = (LITE_SMSHELPER_TYPE_SMS_Priority)Priority;
        pSms->sHeader.eMsgIdType = MsgType;
        pSms->nSMSId = MsgId;
        pSms->sHeader.ts_yr = Yr;
        pSms->sHeader.ts_mon = Mon;
        pSms->sHeader.ts_day = Day;
        pSms->sHeader.ts_hrs = Hr;
        pSms->sHeader.ts_min = Min;
        pSms->sHeader.ts_sec = Sec;

        if(pBearerDataBuf != NULL )
        {
            free(pBearerDataBuf);
            pBearerDataBuf = NULL;
        }
        *pUdhInd = HdrInd;

        /* Only proceed if PTR is NOT NULL */
        if( pUserDataBuf )
        {
            pMsgBody = NULL;
            rc = sms_DecodeUserData( pUserDataBuf,
                                 UserDataLen,
                                 TeleServiceIdValue,
                                 pRefNum,
                                 pTotalNum,
                                 pSeqNum,
                                 pUdhInd,
                                 &MsgEncoding,
                                 &DataMsgLen,
                                 &pMsgBody );

            if( (uint32_t)rc != (uint32_t)sMS_HELPER_OK )
            {
                FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
                return rc;
            }
            pSms->sHeader.dataEncoding = MsgEncoding;
            pSms->sHeader.eMsgDispMode = LITE_SMSHELPER_SMS_MSGDISPMODE_DEFAULT;

            /* Copy the pointer to message body, application
               will the free the memory */
            pSms->pMessage = pMsgBody;
            pSms->sizeBuffer = DataMsgLen;

            if(pUserDataBuf != NULL )
            {
                free(pUserDataBuf);
                pUserDataBuf = NULL;
            }
        }
    }
    else
    {
        FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_POINTERS;
        return sMS_HELPER_ERR_SWISMS_BEARER_DATA_NOT_FOUND;
    }
    FREE_ALL_SMS_DECODE_COMPLETE_MT_SMS_LOCAL_POINTERS;
    return sMS_HELPER_OK;
}


/*
 * This function can be used to decode a received CDMA SMS message
 * read off the device using the GetSMS() API. The text message will
 * be returned in text along with related message information.
 *
 * @param  messageLength[IN]
 *         - Length of the message to be decoded in bytes.
 *
 * @param  pMessage[IN]
 *         - Message read off the device via GetSMS.
 *
 * @param  pSenderAddrLength[IN/OUT]
 *         - Upon input, indicates the maximum number of ASCII
 *           characters (including NULL termination) that the
 *           pSenderAddr buffer can accommodate.  Note that a
 *           length of 14 is reasonable.Upon successful output,
 *           returns the length of originating address string
 *          (including the NULL termination).
 *
 * @param  pSenderAddr[OUT]
 *         - Returns NULL-terminated ASCII String containing the
 *           originating address International number will be
 *           prepended with a '+'character.
 *
 * @param  pTextMsgLength[IN/OUT]
 *         - Upon input, specifies the number of UCS2 characters the
 *           given text message buffer can accommodate.Upon successful
 *           output, returns the number of UCS2 characters returns in
 *           the given text messagebuffer(including NULL-terminator).
 *
 * @param  pTextMsg[OUT]
 *         - Returns the text message as  NULL-terminated UCS2 string.
 *
 * @param  pPriority[OUT] (optional pointer)
 *         - Returns the priority setting of the message.
 *           0x00 - normal.
 *           0x01 - interactive.
 *           0x02 - urgent.
 *           0x03 - emergency.
 *           0xFF - unavailable setting.
 *
 * @param  pPrivacy[OUT](optional pointer)
 *         - Returns the privacy setting of the message.
 *           0x00 - not restricted.
 *           0x01 - restricted.
 *           0x02 - confidential.
 *           0x03 - secret.
 *           0xFF - unavailable setting.
 *
 * @param  pLanguage[OUT] (optional pointer)
 *         - Returns the language setting of the message
 *           0x00 - unspecified.
 *           0x01 - english.
 *           0x02 - french.
 *           0x03 - spanish.
 *           0x04 - japanese.
 *           0x05 - korean.
 *           0x06 - chinese.
 *           0x07 - hebrew.
 *           0xFF - unavailable setting.
 *
 * @param  pMcTimeStamp[OUT] (optional pointer)
 *         - Returns the message center timestamp which takes the form:
 *           YYMMDDHHMMSSTZ where
 *           YY - year.
 *           MM - month.
 *           DD - day.
 *           HH - hour.
 *           MM - minute.
 *           SS - second.
 *           TZ - timezone.
 *           All values are in decimal. Timezone is in relation to GMT,
 *           one unit is equal to 15 minutes and MSB indicates a
 *           negative value.If this information is unavailable for
 *           message then this field will be filled with 0xFF.
 *
 * @param  pAbsoluteValidity[OUT] (optional pointer)
 *         - Returns the absolute validity period setting for this
 *           message.Thisfield takes the same form as mcTimeStamp.
 *
 * @param  pRelativeValidity[OUT] (optional pointer)
 *         - Returns the relative validity period.Values have the
 *           following meanings:
 *           0 to 143: validity period =(value + 1)* 5 minutes.
 *           144 to 167: validity period =12 hours+(value - 143)*30
 *           minutes.
 *           168 to 196: validity period = (value - 166) * 1 day.
 *           197 to 244: validity period = (value - 192) * 1 week.
 *           245: validity period = indefinite.
 *           246: validity period = immediate.
 *           247: validity period = valid until mobile becomes inactive
 *           248: validity period = valid until registration area
 *                changes.
 *           249 to 254: reserved.
 *           255: unavailable information.
 *
 * @param  pDisplayMode[OUT] (optional pointer)
 *         - Returns the display mode parameter.
 *           0x00 - immediate display.
 *           0x01 - mobile default setting.
 *           0x02 - user invoked.
 *           0x03 - reserved.
 *           0xFF - unavailable parameter.
 *
 * @param  pUserAcknowledgementReq[OUT] (optional pointer)
 *         - Returns the user (manual) acknowledgment request parameter
 *           TRUE means the user is requested to manually acknowledge
 *           the delivery of the message FALSE means no such user
 *           acknowledgement is requested.
 *
 * @param  pReadAcknowledgementReq[OUT] (optional pointer)
 *         - Returns the read acknowledgement request parameter
 *           TRUE means acknowledgment of the message being viewed is
 *           requested FALSE means no such read acknowledgement is
 *           requested.
 *
 * @param pAlertPriority[OUT] (optional pointer)
 *        - Returns the alerting parameter setting.
 *          0x00 - use default alert.
 *          0x01 - use low priority alert.
 *          0x02 - use medium priority alert.
 *          0x03 - use high priority alert.
 *          0xFF - unavailable parameter.
 *
 * @param  pCallbkAddrLength[OUT] (optional pointer)
 *         - returns the length of Callback
 *           address string (including the NULL termination).
 *
 * @param  pCallbkAddr[OUT]
 *         - returns NULL-terminated ASCII String containing callback
 *           address String containing the Call Back number with a 32
 *           maximum characters.
 *
 * @param  pRefNum[OUT]
 *          - Reference number of the sms.
 *
 * @param  pTotalNum[OUT]
 *          - Total number of the concatenated message.
 *
 * @param  pSeqNum[OUT]
 *          - Sequence number of the current message.
 *
 * @param  pUdhInd[OUT]
 *          - Is User Data Header Present in the PDU? If yes, it means it is a
 *          - concatenated SMS.
 *
 * @return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * @note  None.
 *
 */
 static uint32_t sms_SwiCDMADecodeMTTextMsg(
    uint32_t  messageLength,
    uint8_t*  pMessage,
    uint32_t* pMessageID,
    uint8_t*  pSenderAddrLength,
    char*     pSenderAddr,
    uint8_t*  pTextMsgLength,
    uint16_t* pTextMsg,
    uint8_t*  pPriority,
    uint8_t*  pPrivacy,
    uint8_t*  pLanguage,
    sms_timestemp*  pMcTimeStamp,
    sms_timestemp*  pAbsoluteValidity,
    uint8_t*  pRelativeValidity,
    uint8_t*  pDisplayMode,
    int8_t*   pUserAcknowledgementReq,
    int8_t*   pReadAcknowledgementReq,
    uint8_t*  pAlertPriority,
    uint8_t*  pCallbkAddrLength,
    char*     pCallbkAddr,
    uint16_t*  pRefNum,
    uint8_t*  pTotalNum,
    uint8_t*  pSeqNum,
    int8_t*   pUdhInd)
{
    uint32_t ret;
    int i = 0;
    LITE_SMSHELPER_STRUCT_CDMA_SMS_RetrieveSms Sms;

    memset(&Sms, 0, sizeof(LITE_SMSHELPER_STRUCT_CDMA_SMS_RetrieveSms));
    Sms.pMessage = NULL;
    Sms.sHeader.addrLen = MAX_SMSI_TL_ADDRESS;

    ret = sms_DecodeCompleteMTSms(pMessage, messageLength, &Sms,
            pRefNum,
            pTotalNum,
            pSeqNum,
            pUdhInd);
    if(ret == 0)
    {
        if(pMessageID)
        {
            *pMessageID = Sms.nSMSId;
        }

        if(pTextMsgLength && pTextMsg)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_USER_DATA)
            {
                if(*pTextMsgLength < (Sms.sizeBuffer + 1))
                {
                    *pTextMsgLength = (uint8_t)(Sms.sizeBuffer + 1);
                    if (Sms.pMessage!=NULL) {
                        free(Sms.pMessage);
                        Sms.pMessage = NULL;
                    }
                    return(sMS_HELPER_ERR_BUFFER_SZ);
                }
                memset(pTextMsg, 0, (*pTextMsgLength) * sizeof(uint16_t) );
                *pTextMsgLength = (uint8_t)(Sms.sizeBuffer + 1);
                strncpy( (char*)pTextMsg, (char*)Sms.pMessage, Sms.sizeBuffer);
            
                if (Sms.pMessage!=NULL) {
                    free(Sms.pMessage);
                    Sms.pMessage = NULL;
                }
            }
            else
            {
                *pTextMsgLength = 0;
                *pTextMsg = 0;
            }
        }

        if(pSenderAddrLength && pSenderAddr)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_ADDRESS)
            {
                if(*pSenderAddrLength < (Sms.sHeader.addrLen + 1))
                {
                     *pSenderAddrLength = Sms.sHeader.addrLen + 1;
                     return(sMS_HELPER_ERR_BUFFER_SZ);
                }
                memset(pSenderAddr, 0, *pSenderAddrLength);
                *pSenderAddrLength = Sms.sHeader.addrLen + 1;
                strncpy( pSenderAddr, (char*)Sms.sHeader.szAddress,
                         Sms.sHeader.addrLen);
            }
            else
            {
                *pSenderAddrLength = 0;
                *pSenderAddr = 0;
            }
        }
        if(pCallbkAddrLength && pCallbkAddr)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_CALL_BACK)
            {
                if(*pCallbkAddrLength < (Sms.sHeader.callBackLen + 1))
                {
                    *pCallbkAddrLength = Sms.sHeader.callBackLen + 1;
                    return(sMS_HELPER_ERR_BUFFER_SZ);
                }
                memset(pCallbkAddr, 0, *pCallbkAddrLength);
                *pCallbkAddrLength = Sms.sHeader.callBackLen + 1;
                /*This loop will do the same task as wctomb()*/
                int i=0;
                for( i= 0; i < Sms.sHeader.callBackLen;i++ )
                pCallbkAddr[i] = (char) Sms.sHeader.szCallBack[i];
                /*wctomb(pCallbkAddr, (wchar_t)Sms.sHeader.szCallBack );*/
            }
            else
            {
                *pCallbkAddrLength = 0;
                *pCallbkAddr = 0;
            }
        }
        if(pPriority)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_PRIORITY)
            {
                 *pPriority = Sms.sHeader.ePriority;
                  /* 64 is decoded value for URGENT VZAM support
                     NORMAL - 0 or URGENT - 64 */
                  if (*pPriority == 64)
                  {
                      /* for VZAM  interactive (64), set to Normal
                         as VZAM does not use */
                      *pPriority = 0;
                  }
                  /* 128 is decoded value for URGENT VZAM
                     support NORMAL - 0 or URGENT - 128 */
                  else if (*pPriority == 128)
                  {
                      *pPriority = 1;
                  }
                  /* for WMC, this is set to urgent  as VZAM does
                     not currently support emergency (192)*/
                  else if (*pPriority == 192)
                  {
                     *pPriority = 1;
                  }
                  else
                  {
                  /* default - NORMAL, if we get here, then modem
                  must have returned a 0 or their were problems */
                      *pPriority = 0;
                  }
            }
            else
            {
                *pPriority = 0xFF;
            }
        }
        if(pPrivacy)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_PRIVACY)
            {
                *pPrivacy = Sms.sHeader.ePrivacy;
            }
            else
            {
                *pPrivacy = 0xFF;
            }
        }
        if(pLanguage)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_LANGUAGE)
            {
                *pLanguage = Sms.sHeader.eLanguage;
            }
            else
            {
                *pLanguage = 0xFF;
            }
        }
        if(pMcTimeStamp)
        {
            for(i=0;i<eTIMESTEMP_END;i++)
            {
                pMcTimeStamp->data[i] = SMS_INVALID_TIME_STAMP_VALUE;
            }
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_MC_TIME_STAMP)
            {
                pMcTimeStamp->data[eTIMESTEMP_YEAR] = BCD_DECODE(Sms.sHeader.ts_yr);
                pMcTimeStamp->data[eTIMESTEMP_MONTH] = BCD_DECODE(Sms.sHeader.ts_mon);
                pMcTimeStamp->data[eTIMESTEMP_DAY] = BCD_DECODE(Sms.sHeader.ts_day);
                pMcTimeStamp->data[eTIMESTEMP_HOUR] = BCD_DECODE(Sms.sHeader.ts_hrs);
                pMcTimeStamp->data[eTIMESTEMP_MINUTE] = BCD_DECODE(Sms.sHeader.ts_min);
                pMcTimeStamp->data[eTIMESTEMP_SECOND] = BCD_DECODE(Sms.sHeader.ts_sec);
                pMcTimeStamp->data[eTIMESTEMP_TIMEZONE] = SMS_DEFAULT_TIME_ZONE;
            }
            
        }
        if(pAbsoluteValidity)
        {
            for(i=0;i<eTIMESTEMP_END;i++)
            {
                pAbsoluteValidity->data[i] = SMS_INVALID_TIME_STAMP_VALUE;
            }
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_VALID_ABS)
            {
                pAbsoluteValidity->data[eTIMESTEMP_YEAR] = BCD_DECODE(Sms.sHeader.abVal_yr);
                pAbsoluteValidity->data[eTIMESTEMP_MONTH] = BCD_DECODE(Sms.sHeader.abVal_mon);
                pAbsoluteValidity->data[eTIMESTEMP_DAY] = BCD_DECODE(Sms.sHeader.abVal_day);
                pAbsoluteValidity->data[eTIMESTEMP_HOUR] = BCD_DECODE(Sms.sHeader.abVal_hrs);
                pAbsoluteValidity->data[eTIMESTEMP_MINUTE] = BCD_DECODE(Sms.sHeader.abVal_min);
                pAbsoluteValidity->data[eTIMESTEMP_SECOND] = BCD_DECODE(Sms.sHeader.abVal_sec);
                pAbsoluteValidity->data[eTIMESTEMP_TIMEZONE] = SMS_DEFAULT_TIME_ZONE;
            }
        }
        if(pRelativeValidity)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_VALID_REL)
            {
                *pRelativeValidity = Sms.sHeader.relVal_time;
            }
            else
            {
                 *pRelativeValidity = 0xFF;
            }
        }
        if(pDisplayMode)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_DISP_MODE)
            {
                *pDisplayMode = Sms.sHeader.eMsgDispMode;
            }
            else
            {
                *pDisplayMode = 0xFF;
            }
        }
        if(pUserAcknowledgementReq)
        {
            /* not supported by this code base yet */
            *pUserAcknowledgementReq = FALSE;
        }
        if(pReadAcknowledgementReq)
        {
            /* not supported by this code base yet */
            *pReadAcknowledgementReq = FALSE;
        }
        if(pAlertPriority)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_DISP_MODE)
            {
                *pAlertPriority = Sms.sHeader.deliveryAlert;
            }
            else
            {
                *pAlertPriority = 0xFF;
            }
        }
    }
    else {
        /* In case of error, set sender addr and time stamp, if available */
        if(pSenderAddrLength && pSenderAddr)
        {
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_ADDRESS)
            {
                if(*pSenderAddrLength >= (Sms.sHeader.addrLen + 1))
                {                   
                    memset(pSenderAddr, 0, *pSenderAddrLength);
                    *pSenderAddrLength = Sms.sHeader.addrLen + 1;
                    strncpy( pSenderAddr, (char*)Sms.sHeader.szAddress,
                             Sms.sHeader.addrLen);
                }
            }
            else
            {
                *pSenderAddrLength = 0;
                *pSenderAddr = 0;
            }
        }
        if(pMcTimeStamp)
        {
            for(i=0;i<eTIMESTEMP_END;i++)
            {
                pMcTimeStamp->data[i] = SMS_INVALID_TIME_STAMP_VALUE;
            }
            if(Sms.sHeader.eParamMask & LITE_SMSHELPER_SMS_PARAMASK_MC_TIME_STAMP)
            {
                pMcTimeStamp->data[eTIMESTEMP_YEAR] = BCD_DECODE(Sms.sHeader.ts_yr);
                pMcTimeStamp->data[eTIMESTEMP_MONTH] = BCD_DECODE(Sms.sHeader.ts_mon);
                pMcTimeStamp->data[eTIMESTEMP_DAY] = BCD_DECODE(Sms.sHeader.ts_day);
                pMcTimeStamp->data[eTIMESTEMP_HOUR] = BCD_DECODE(Sms.sHeader.ts_hrs);
                pMcTimeStamp->data[eTIMESTEMP_MINUTE] = BCD_DECODE(Sms.sHeader.ts_min);
                pMcTimeStamp->data[eTIMESTEMP_SECOND] = BCD_DECODE(Sms.sHeader.ts_sec);
                pMcTimeStamp->data[eTIMESTEMP_TIMEZONE] = SMS_DEFAULT_TIME_ZONE;
            }
        }        
    }
    return ret;
}

/*
 * Converts Hex format of data to ASCII format.
 *
 * \params  pStr[OUT] - Hold final converted ASCII string.
 *          Strlen[IN] - length of Str data.
 *          pHexa[IN] - Hex data to convert in ASCII.
 *          Hexalen[IN]   - length of hex data.
 *
 * \return  None.
 *
 * \notes   None.
 *
 */
static void sms_SwiHex2Asc( uint8_t *pStr, 
    uint16_t Strlen,
    uint8_t *pHexa,
    uint16_t Hexalen )
{
    uint16_t idx;

   if( (pStr==NULL) ||
       (pHexa==NULL) || 
        (Hexalen == 0) )
    {
        return ;
    }
    memset(pStr, 0, Strlen);
    for ( idx = 0 ; idx < Strlen ; idx ++ )
    {
        if(Hexalen*2 > idx)
        {
           /* 0..9 + '0'   or   10..15 + '0' + 7 */
           pStr[idx] = HEX( pHexa, idx ) + 0x30 +\
                          ( (HEX( pHexa, idx) <= 9) ? 0 : 7 );
        }
        else
        {
            break;
        }
    }
   
}

/*
 * Converts an octet(ASCII) to a 8-Bit value.
 *
 * \params  octet[IN] - octet which user wants to converts.
 *
 * \return  8 bit value.
 *
 * \notes   none.
 */
static uint8_t sms_Octet2bin(int8_t* octet)
{
    uint8_t result = 0;
    if(octet==NULL)
        return 0;
    if((octet[0]<48)||(octet[1]<48))
        return 0;

    if ( octet[0]>57 )
        result += octet[0]-55;
    else
        result += octet[0]-48;
    result = result<<4;
    if ( octet[1]>57 )
        result += octet[1]-55;
    else
        result += octet[1]-48;
    return result;
}

/*
 * Converts 7- bit PDU to text.
 *
 * \params  pPdu[IN]  - Pdu message which user wants to converts in text.
 *          PduLength[OUT] - Pdu Length.
 *          text[OUT] - Text message.
 *          with_udh  - If UDH want to fetch UDH header.
 *          PUDMsgLength - Message Length in PDU.
 *          pReferenceNum- Reference Number field.
 *          pTotalNum    - Total Number field.
 *          pSequenceNum - Sequence Number field.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes   None.
 */
static int sms_PduDefaultEncoding2text(
    int8_t *pPdu,
    uint16_t PduLength,
    uint8_t *pTextMsgLength,
    int8_t *text,
    uint32_t with_udh,
    uint32_t PUDMsgLength,
    uint8_t  *pReferenceNum,
    uint8_t  *pTotalNum,
    uint8_t  *pSequenceNum )
{
    uint32_t bitposition=0;
    uint32_t byteposition;
    uint8_t hdrElementType = 0xFF;
    uint32_t totalOctets = 0;
    uint32_t byteoffset;
    uint32_t charcounter;
    uint32_t bitcounter;
    uint32_t octets;
    uint8_t  udhsize;
    uint32_t octetcounter;
    uint32_t skip_characters;
    int8_t  c;
    int8_t  binary[MAX_BINARY_ARRAY_LEN]={'\0'};
    uint8_t  ctr = 0, EscTabCtr = 0;
    uint8_t  EscFlag = FALSE, EScTabMatch = FALSE;
    uint32_t NumEscChars = 0;
    uint8_t  Index       = 0;
    uint32_t EscTabSize  = GSM_ESC_CHAR_SET_SIZE;
    int8_t  GSMEscChar  = 0x1b;
    if(pTextMsgLength==NULL)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if( *pTextMsgLength < PUDMsgLength )
    {
        *pTextMsgLength = PUDMsgLength;
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    totalOctets = (PUDMsgLength*7+7)/8;
    
    /* PduLength includes the Userdatalen size field ( 2 bytes in ASCII) also */
    if (PduLength != ((totalOctets + 1) * 2))
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    if ( with_udh )
    {
        if ( (NULL == pReferenceNum)  ||
             (NULL == pTotalNum)  || 
             (NULL == pSequenceNum) )
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }

        /* 2 bytes for UDlen + 2 bytes for UDH */
        if(PduLength < (PDU_FAULT_ENC_2_TXT_SEQ_NUM_DATA2_INDEX+1))
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        /* copy the data header to udh and convert to hex dump */
        udhsize = sms_Octet2bin(pPdu+PDU_FAULT_ENC_2_TXT_UDH_HDR_INDEX);

        if ((udhsize * 2) > (PduLength - PDU_FAULT_ENC_2_TXT_UDH_DATA2_INDEX))
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        if (udhsize >= 2)
        {
            hdrElementType = sms_Octet2bin(pPdu+PDU_FAULT_ENC_2_TXT_UDH_DATA2_INDEX);

            /* If not concatenated short message type */
            if (hdrElementType != 0)
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
            }
            if (udhsize != UDH_SIZE_WITH_CONCATENATED_ELEMENT)
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
            }
        }
        else
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        /* copy the reference number of sms and convert to hex dump */
        *pReferenceNum = sms_Octet2bin(pPdu+PDU_FAULT_ENC_2_TXT_REF_NUM_HDR_INDEX);
        /* copy the total number of sms and convert to hex dump */
        *pTotalNum = sms_Octet2bin(pPdu+PDU_FAULT_ENC_2_TXT_TOT_NUM_HDR_INDEX);
        /* copy the total number of sms and convert to hex dump */
        *pSequenceNum = sms_Octet2bin(pPdu+PDU_FAULT_ENC_2_TXT_SEQ_NUM_HDR_INDEX);

        /* Calculate how many text characters include the UDH.*/
        /* After the UDH there may follow filling bits to reach a 7bit boundary.*/
        skip_characters=(((udhsize+1)*8)+6)/7;
    }
    else
    {
        skip_characters = 0;
    }

    /* Copy user data of pPdu to a buffer */
    octets = (PUDMsgLength*7+7)/8;
    for (octetcounter=0; octetcounter<octets; octetcounter++)
         binary[octetcounter]=sms_Octet2bin(pPdu+(octetcounter<<1)+2);

    /* Then convert from 8-Bit to 7-Bit encapsulated in 8 bit */
    /* skipping storing of some characters used by UDH. */
    for (charcounter=0; charcounter<PUDMsgLength; charcounter++)
    {
        c=0;
        for (bitcounter=0; bitcounter<7; bitcounter++)
        {
            byteposition = bitposition/8;
            byteoffset   = bitposition%8;
            if (binary[byteposition]&(1<<byteoffset))
                c = c|128;
            bitposition++;
            c=(c>>1)&127; /* The shift fills with 1, but I want 0 */
        }

        if (charcounter>=skip_characters)
        {
            /* Decode characters but skip any GSM escape character. In such
             * a scenario Subsequent character needs to be decoded via escape
             * table */
            if( GSMEscChar != c )
            {
                if(*pTextMsgLength < Index)
                {
                    return sMS_HELPER_ERR_INVALID_ARG;
                }
                /* Convert GSM character set to UCS either via GSM to UCS
                 * or escape table */
                if( EscFlag == FALSE )
                {
                    ctr = (uint8_t)c;
                    /* Retrieve from GSM to UCS 2 table*/
                    if ( ((sms_GsmToUcs2[ctr] >> 8) & 0xff) > 0 )
                    {
                        text[Index] = sms_GsmToUcs2[ctr] >> 8;
                        Index++;
                        text[Index] = sms_GsmToUcs2[ctr] & 0xff;
                    }
                    else
                    {
                        text[Index] = (char)sms_GsmToUcs2[ctr];
                    }
                    Index++;
                }
                else
                {
                    /* Previous char was a escape character. Try matching */
                    for( EscTabCtr = 0; EscTabCtr < EscTabSize; EscTabCtr++ )
                    {
                        /* Retrieve from Escape table*/
                        if( c == (char) sms_EscapedGsm[EscTabCtr].gsmcode )
                        {
                            if ( ((sms_EscapedGsm[EscTabCtr].unicode >> 8) & 0xff) > 0 )
                            {
                                text[Index] = sms_EscapedGsm[EscTabCtr].unicode >> 8;
                                Index++;
                                text[Index] = sms_EscapedGsm[EscTabCtr].unicode & 0xff;
                            }
                            else
                            {
                                text[Index] = (char)sms_EscapedGsm[EscTabCtr].unicode;
                            }
                            EScTabMatch = TRUE;
                            Index++;
                        }
                    }
                    /* Just in case, we could not match any character from
                     * escape table, copy the received character */
                    if ( EScTabMatch == FALSE )
                    {
                        text[Index] = c;
                        Index++;
                    }

                    /* Clear Flags */
                    EScTabMatch = FALSE;
                    EscFlag = FALSE;
                }
            }
            else
            {
                EscFlag = TRUE;
                NumEscChars++;
            }
        }
    }
    if(*pTextMsgLength < (PUDMsgLength-skip_characters-NumEscChars))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    text[Index]='\0';
    *pTextMsgLength = Index;

    return sMS_HELPER_OK;
}

/*
 * Converts 8-bit/UCS2 PDU to text.
 *
 * \params  pPdu[IN]  - Pdu message which user wants to converts in text.
 *          PduLength[INT] - Pdu Length.
 *          pTextMsgLength - Text message length.
 *          text[OUT] - Text message.
 *          with_udh  - If UDH want to fetch UDH header.
 *          PUDMsgLength  - Message Length in PDU.
 *          encoding  - SMS encoding scheme ( 0 - 7 bit(default),
 *                                            4 - 8 bit,
 *                                            8 - UCS2 ).
 *          pReferenceNum - Reference Number field.
 *          pTotalNum - Total Number field.
 *          pSequenceNum - Sequence Number field.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes   None.
 */
static uint32_t sms_PduEncoded2text(
    int8_t *pPdu,
    uint16_t PduLength,
    uint8_t *pTextMsgLength,
    int8_t *text,
    uint32_t with_udh,
    uint32_t PUDMsgLength,
    uint32_t encoding,
    uint8_t  *pReferenceNum,
    uint8_t  *pTotalNum,
    uint8_t  *pSequenceNum )
{
    uint32_t octets;
    uint8_t  udhsize;
    uint8_t hdrElementType = 0xFF;
    uint32_t octetcounter;
    uint32_t skip_octets;
    uint8_t  ctr;

    uint8_t EscTabCtr = 0;
    uint8_t EscFlag = FALSE, EScTabMatch = FALSE;
    uint32_t NumEscChars = 0;
    uint8_t Index = 0;
    int8_t c;
    uint8_t cb;
    uint32_t EscTabSize = GSM_ESC_CHAR_SET_SIZE;
    int8_t GSMEscChar = 0x1b;

    int8_t  binary[MAX_BINARY_ARRAY_LEN]={'\0'};
    if( (NULL == pPdu) ||
        (NULL == pTextMsgLength) ||
        (NULL == text) ) 
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* datStrlen includes the Userdatalen size field ( 2 bytesin ASCII) also */
    if (PduLength != ((PUDMsgLength + 1) * 2))
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    if ( with_udh )
    {
        if ( (NULL == pReferenceNum)  ||
              (NULL == pTotalNum)  || 
              (NULL == pSequenceNum) )
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }
        /* check size before decode */
        if(PduLength < (PDU_ENC_2_TXT_SEQ_NUM_DATA2_INDEX+1))
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        /* copy the data header to udh and convert to hex dump */
        udhsize = sms_Octet2bin(pPdu+PDU_ENC_2_TXT_UDH_HDR_INDEX);

        if ((udhsize * 2) > (PduLength - PDU_ENC_2_TXT_UDH_DATA2_INDEX))
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        if (udhsize >= 2)
        {
            hdrElementType = sms_Octet2bin(pPdu+PDU_ENC_2_TXT_UDH_DATA2_INDEX);

            /* If not concatenated short message type */
            if (hdrElementType != 0)
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
            }
            if (udhsize != UDH_SIZE_WITH_CONCATENATED_ELEMENT)
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
            }
        }
        else
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        /* copy the reference number of sms and convert to hex dump */
        *pReferenceNum = sms_Octet2bin(pPdu+PDU_ENC_2_TXT_REF_NUM_HDR_INDEX);
        /* copy the total number of sms and convert to hex dump */
        *pTotalNum = sms_Octet2bin(pPdu+PDU_ENC_2_TXT_TOT_NUM_HDR_INDEX);
        /* copy the total number of sms and convert to hex dump */
        *pSequenceNum = sms_Octet2bin(pPdu+PDU_ENC_2_TXT_SEQ_NUM_HDR_INDEX);

        /* For 8-bit and UCS2 encoding UDH consists of 1 byte UDH length
         * followed by no of octets in integer format( no padding )
         * ref. 3GPP TS03.40 version 7.5.0 sec 9.2.3.16
         */
        skip_octets = ( udhsize + 1 );
    }
    else
    {
        skip_octets = 0;
    }
    if(MAX_BINARY_ARRAY_LEN < PUDMsgLength)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(PduLength < PUDMsgLength)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    /* Copy user data of pPdu to a buffer by combining characters
     * and skipping UDH header */
    octets = PUDMsgLength;
    for( octetcounter = 0; octetcounter < octets; octetcounter++ )
    {
        if( octetcounter >= skip_octets )
        {
            if((octetcounter<<1)+2 < PduLength)
            {
                binary[octetcounter - skip_octets]= (char)
                    sms_Octet2bin(pPdu+(octetcounter<<1)+2);
            }        
        }
    }

    /* Create Text message based on encoding format.
     * 1) For 8-bit copy all characters as it is
     * 2) For UCS2 ignore first byte( To be changed in case
     *    multi-lingual support is required in SDK ) */
    if ( EIGHTBITENCODING == encoding )
    {
        /* Check for buffer overflow */
        if( *pTextMsgLength < PUDMsgLength )
        {
            *pTextMsgLength = PUDMsgLength;
            return sMS_HELPER_ERR_BUFFER_SZ;
        }
        if(octetcounter < skip_octets)
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }
        if(*pTextMsgLength < (octetcounter - skip_octets) )
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }
        /* Copy text byte by byte */
        for( ctr = 0; ctr < (octetcounter - skip_octets); ctr++ )
        {
            c= binary[ctr];

            /* Decode characters but skip any GSM escape character. In such
             * a scenario Subsequent character needs to be decoded via escape
             * table
             */
            if( GSMEscChar != c )
            {
                if(*pTextMsgLength < Index)
                {
                    return sMS_HELPER_ERR_INVALID_ARG;
                }

                /* Convert GSM character set to UCS either via GSM to UCS
                 * or escape table */
                if( EscFlag == FALSE )
                {
                    cb = (uint8_t)c;
                    /* Retrieve from GSM to UCS 2 table*/
                    if ( ((sms_GsmToUcs2[cb] >> 8) & 0xff) > 0 )
                    {
                        text[Index] = sms_GsmToUcs2[cb] >> 8;
                        Index++;
                        text[Index] = sms_GsmToUcs2[cb] & 0xff;
                    }
                    else
                    {
                        text[Index] = (char)sms_GsmToUcs2[cb];
                    }
                    Index++;
                }
                else
                {
                    /* Previous char was a escape character. Try matching */
                    for( EscTabCtr = 0; EscTabCtr < EscTabSize; EscTabCtr++ )
                    {
                        /* Retrieve from Escape table*/
                        if( c == (char) sms_EscapedGsm[EscTabCtr].gsmcode )
                        {
                            if ( ((sms_EscapedGsm[EscTabCtr].unicode >> 8) & 0xff) > 0 )
                            {
                                text[Index] = sms_EscapedGsm[EscTabCtr].unicode >> 8;
                                Index++;
                                text[Index] = sms_EscapedGsm[EscTabCtr].unicode & 0xff;
                            }
                            else
                            {
                                text[Index] = (char)sms_EscapedGsm[EscTabCtr].unicode;
                            }
                            EScTabMatch = TRUE;
                            Index++;
                        }
                    }
                    /* Just in case, we could not match any character from
                     * escape table, copy the received character */
                    if ( EScTabMatch == FALSE )
                    {
                        text[Index] = c;
                        Index++;
                    }

                    /* Clear Flags */
                    EScTabMatch = FALSE;
                    EscFlag = FALSE;
                }
            }
            else
            {
                EscFlag = TRUE;
                NumEscChars++;
            }
        }
        text[Index]='\0';
        *pTextMsgLength = Index;
    }
    else
    {
        /* Check for buffer overflow */
        if( *pTextMsgLength < PUDMsgLength )
        {
            *pTextMsgLength = PUDMsgLength;
            return sMS_HELPER_ERR_BUFFER_SZ;
        }

        if(octetcounter < skip_octets)
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }

        if(*pTextMsgLength < (octetcounter - skip_octets) )
        {
            return sMS_HELPER_ERR_INVALID_ARG;
        }

        /* UCS-2 decoding is not done here, copy and pass the entire data to user application */
        memcpy((void*)text,(void*)binary,octetcounter - skip_octets);
        *pTextMsgLength = octetcounter - skip_octets;
    }
    return sMS_HELPER_OK;
}

/*
 * Converts PDU to text.
 *
 * \params  pPdu[IN]  - Pdu message which user wants to converts in text.
 *          PduLength[IN] - Pdu size.
 *          pTextMsgLength[IN] - text messsage length.
 *          text[OUT] - Text message.
 *          with_udh  - If UDH want to fectch UDH header.
 *          encoding  - SMS encoding scheme ( 0 - 7 bit(default),
 *                                            4 - 8 bit,
 *                                            8 - UCS2 ).
 *          pReferenceNum - Reference Number field.
 *          pTotalNum     - Total Number field.
 *          pSequenceNum  - Sequence Number field.
 *
 * \return  Length of text.
 *
 * \notes   None.
 */
static uint32_t sms_Pdu2text(
    int8_t *pPdu,
    uint16_t PduLength,
    uint8_t *pTextMsgLength,
    int8_t *text,
    uint32_t with_udh,
    uint8_t  encoding,
    uint8_t  *pReferenceNum,
    uint8_t  *pTotalNum,
    uint8_t  *pSequenceNum )
{
    uint32_t PUDMsgLength;
    uint32_t nRet;

    if ( (NULL == pPdu) ||
         (NULL == pTextMsgLength) ||
         (NULL == text) ||
         ( 0 == *pTextMsgLength))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(PduLength< 1)
    {
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }
    PUDMsgLength = (uint32_t)sms_Octet2bin(pPdu);

    /* Validate the PDU message */
    if (( DEFAULTENCODING == encoding ) &&
        ( PUDMsgLength > MAX_SMS_UD_SEPTET_LEN ))
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    else if ((( EIGHTBITENCODING == encoding ) ||( EIGHTBITENCODING == encoding )) &&
        ( PUDMsgLength > MAX_SMS_UD_OCTET_LEN ))
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }

    if( DEFAULTENCODING == encoding )
    {
         nRet = sms_PduDefaultEncoding2text( pPdu,
                                     PduLength,
                                     pTextMsgLength,
                                     text,
                                     with_udh,
                                     PUDMsgLength,
                                     pReferenceNum,
                                     pTotalNum,
                                     pSequenceNum );
    }
    else if ((EIGHTBITENCODING == encoding) || 
             (UCSENCODING == encoding))
    {
         nRet = sms_PduEncoded2text( pPdu,
                                 PduLength,
                                 pTextMsgLength,
                                 text,
                                 with_udh,
                                 PUDMsgLength,
                                 encoding,
                                 pReferenceNum,
                                 pTotalNum,
                                 pSequenceNum );
    }
    else
    {
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }

    return nRet;
}

/*
 * Extract PDU for messages type 0 (SMS-Deliver).
 *
 * \param   pSrcPointer[IN] - Pointer to source string.
 *          SrcPointerLength[In] - source string length.
 *          pSenderLength[IN] - pSendr length.
 *          pSendr[OUT]     - sender address.
 *          pDate[OUT]      - date field.
 *          pTime[OUT]      - time field.
 *          pMsgLength[IN/OUT] - message length.
 *          pmessage[OUT]   - Pointer to message fetched.
 *          with_udh[OUT]   - If UDH header present in message.
 *          pReferenceNum[IN] - Refrence Number field.
 *          pTotalNum[IN]   - Total Number field.
 *          pSequenceNum[IN] - Sequence Number field.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  None.
 */
static int sms_ExtractSMSDeliver(
    int8_t  *pSrcPointer,
    uint16_t  SrcPointerLength,
    uint8_t  *pSenderLength,
    int8_t  *pSendr,
    sms_DateTime  *pDate,
    sms_DateTime  *pTime,
    uint8_t  *pMsgLength,
    int8_t  *pMessage,
    uint32_t with_udh,
    uint8_t  *pReferenceNum,
    uint8_t  *pTotalNum,
    uint8_t  *pSequenceNum )
{
    uint8_t  Length;
    uint32_t padding;
    uint32_t nRet;
    uint8_t  encoding = DEFAULTENCODING;
    int8_t *pRefSrcPointer = NULL;
    uint16_t u16NewSrcPointerLength = 0;
    if ( (NULL == pSrcPointer) || (0 == strlen((char*)pSrcPointer)) || 
          (NULL == pSenderLength) || (NULL == pSendr) ||
          (NULL == pMessage) || (NULL == pDate) || 
          (NULL == pTime) || (NULL == pMsgLength) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    /* Get sender address */
    if (SrcPointerLength < 2)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }
    pRefSrcPointer = pSrcPointer;
    Length      = sms_Octet2bin(pSrcPointer);
    padding     = Length % 2;
    if ((SrcPointerLength - 2) < ((int)(Length + padding + 2)))
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    /* Check If size of buffer is sufficient to hold message */
    if( *pSenderLength < Length )
    {
        DEBUG_ERROR_PRINT;
        *pSenderLength = Length;
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if (Length > MAX_SENDER_ADDRESS_LENGTH)
    {
        snprintf((char*)pMessage,*pMsgLength," Invalid sender address length\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    Length      = Length+2;

    /* Snd Address field size (2) + Length + padding Protocol Identifer(2) + 
       data encoding scheme (2) + date (6) + time stamp (8) + user data length(2) */
    if( (2 + Length + padding + 2 + 2 + 6 + 8 + 2) > SrcPointerLength)
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    pSrcPointer += 2;

    /* remain type of address at the begining */
    strncpy((char*)pSendr,(char*)pSrcPointer,Length+padding);
    nRet = sms_Swapchars(pSendr+2);
    pSendr[Length]=0;
    if (pSendr[Length-1]=='F')
        pSendr[Length-1]=0;

    /* skip 2 octet in Protodol Identifier */
    pSrcPointer = pSrcPointer + Length + padding + 2;

    encoding = sms_Octet2bin(pSrcPointer);

    /* Compressed data */
    if (encoding & 0x20)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pMsgLength,"Compressed data\n");
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }

    /* Bits 2 and 3 of TP-DCS indicate the alphabet being used, please refer to 
       3G TS 23.038 version 2.0.0, section 4 for more details of TP-DCS */
    if(encoding & UCSENCODING)
    {
        encoding = UCSENCODING;
    }
    else if(encoding & EIGHTBITENCODING)
    {
        encoding = EIGHTBITENCODING;
    }
    else if((encoding & 0x0C) == DEFAULTENCODING)
    {
        encoding = DEFAULTENCODING;
    }
    else
    {
        /* Msg not decoded, encoding scheme not supported */
        snprintf((char*)pMessage,*pMsgLength,"Message encoding scheme is not supported."
                        " Cannot decode\n");
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }

    /* Increment pointer to read date */
    pSrcPointer += 2;
    
    /* prepare date format to print */
    sprintf((char*)pDate->data,"%c%c-%c%c-%c%c",(char)pSrcPointer[5],(char)pSrcPointer[4],\
            (char)pSrcPointer[3],(char)pSrcPointer[2],(char)pSrcPointer[1],(char)pSrcPointer[0]);

    /* Increment pointer to read time stamp */
    pSrcPointer = pSrcPointer + 6;

    /* Prepare time format to print */
    sprintf((char*)pTime->data,"%c%c:%c%c:%c%c",(char)pSrcPointer[1],(char)pSrcPointer[0],\
            (char)pSrcPointer[3],(char)pSrcPointer[2],(char)pSrcPointer[5],(char)pSrcPointer[4]);

    /* Increment pointer to point at message */
    pSrcPointer = pSrcPointer + 8;

    if (strlen((char*)pSrcPointer)>1)
    {
        if((uint32_t)sms_Octet2bin(pSrcPointer)>MAX_SMS_MSG_LEN)
        {
           DEBUG_ERROR_PRINT;
           return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }
    }
    u16NewSrcPointerLength = SrcPointerLength - (pSrcPointer - pRefSrcPointer);
    nRet = sms_Pdu2text(pSrcPointer, \
        u16NewSrcPointerLength,\
        pMsgLength,pMessage,\
        with_udh, encoding,\
             pReferenceNum,  pTotalNum, pSequenceNum);
    return nRet;
}

/*
 * Extract PDU for messages type 1 (Sent SMS).
 * \param  pSrcPointer[IN] - Pointer to source string.
 *         SrcPointerLen[IN] - pSrcPointer Length.
 *         pSenderLength[IN] - pSendr size.
 *         pSendr[OUT]     - date field.
 *         pTime[OUT]      - time field.
 *         pDate[OUT]      - date field.
 *         pMessageLength[IN/OUT] - message length.
 *         pMessage[OUT]   - Pointer to message fetched.
 *         with_udh        - PDU field.
 *         pReferenceNum[IN]   - Reference Number field.
 *         pTotalNum[IN]       - Total Number field.
 *         pSequenceNum[IN]    - Sequence Number field.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes   none.
 */
static int sms_ExtractSMSSent(
    int8_t  *pSrcPointer,
    uint16_t SrcPointerLen,
    uint8_t  *pSenderLength,
    int8_t  *pSendr,
    sms_DateTime  *pDate,
    sms_DateTime  *pTime,
    uint8_t  *pMessageLength,
    int8_t  *pMessage,
    uint32_t with_udh,
    uint8_t  *pReferenceNum,
    uint8_t  *pTotalNum,
    uint8_t  *pSequenceNum )
{
    uint8_t  length;
    uint32_t padding;
    uint32_t nRet;
    uint8_t encoding = DEFAULTENCODING;
    int8_t  *pRefSrcPointer = NULL;

    UNUSEDPARAM( pDate );
    UNUSEDPARAM( pTime );

    if( ( NULL == pSrcPointer ) || 
        (pSenderLength==NULL))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(SrcPointerLen<1)
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    pRefSrcPointer = pSrcPointer;
    length  = sms_Octet2bin(pSrcPointer);
    
    padding = length%2;
    length = length+2;
    if(SrcPointerLen < (length + padding+ 2 +3))
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    /* Check If size of buffer is sufficient to hold Sender Address */
    if( *pSenderLength < length )
    {
        *pSenderLength = length;
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if(MIN_SMS_SENT_LENGTH > SrcPointerLen)
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }
    pSrcPointer += 2;
    strncpy((char*)pSendr,(char*)pSrcPointer,length+padding);
    nRet = sms_Swapchars(pSendr+2);
    pSendr[length]=0;
    if (pSendr[length-1]=='F')
        pSendr[length-1]=0;

    pSrcPointer = pSrcPointer + length + padding+3;
    if( UCSENCODING_CHAR == pSrcPointer[0] )
    {
        encoding = UCSENCODING;
    }
    else if( EIGHTBITENCODING_CHAR == pSrcPointer[0] )
    {
        encoding = EIGHTBITENCODING;
    }
    else if( DEFAULTENCODING_CHAR == pSrcPointer[0] )
    {
        encoding = DEFAULTENCODING;
    }
    else
    {
        /* Msg not decoded, encoding scheme not supported */
        snprintf((char*)pMessage,*pMessageLength,"Message encoding scheme is not supported."
                        " Cannot decode\n");
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }
    pSrcPointer += 3;

    sms_Pdu2text(pSrcPointer, \
             SrcPointerLen - (pSrcPointer - pRefSrcPointer), \
             pMessageLength,pMessage,with_udh,encoding, \
             pReferenceNum,  pTotalNum, pSequenceNum);
    return nRet;
}

/*
 * Extract PDU read from device to readable form of message
 *
 * \params  pPdu[IN]       - pointer to GSM PDU string.
 *          PduLength[IN]  - Pdu Length.
 *          pSendr[OUT]    - sender address.
 *          pSenderLength[In] - Sender Address Length.
 *          pDate[OUT]     - date field.
 *          pTime[OUT]     - time field.
 *          pMessage[OUT]  - pointer to message fetched.
 *          MessageLen[In] - message buffer length.
 *          pReferenceNum[OUT]- reference number of the sms.
 *          pTotalNum[OUT]    - total number of the concatenated sms.
 *          pSequenceNum[OUT] - sequence number of the concatenated sms.
 *          pWithUDH[OUT]     - is user data header present in the PDU.
 *
 * \return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes   None.
 */
static int sms_SwiExtractpdu(
    char *pPdu,
    uint16_t PduLength,
    int8_t *pSendr,
    uint8_t *pSenderLength,
    uint8_t *pTextMsgLength,
    uint8_t *pScAddrLength,
    sms_DateTime *pDate,
    sms_DateTime *pTime,
    int8_t *pMessage,
    int8_t *pSmsc,
    uint8_t *pReferenceNum,
    uint8_t *pTotalNum,
    uint8_t *pSequenceNum,
    int8_t *pWithUDH )
{
    uint8_t  Length    = 0;
    uint8_t  Type      = 0 ;
    int8_t  *Pointer  = NULL;
    int8_t  *RefPointer  = NULL;
    int with_udh  = 0;
    uint16_t u16NewPduLength = 0;
        
    if (pWithUDH != NULL)
    {
        *pWithUDH = 0;
    }

    if( (NULL == pPdu) || (0 == strlen((char*)pPdu)) || 
        (NULL == pSendr) || (NULL == pDate) || 
        (NULL == pTime) || (NULL == pMessage) ||
        (NULL == pSenderLength) || (NULL == pScAddrLength) ||
        (NULL == pSmsc) || (NULL == pTextMsgLength) ||
        (0 == *pTextMsgLength) )
    {
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    pSendr[0]       = 0;
    pDate->data[0]        = 0;
    pTime->data[0]        = 0;
    pMessage[0]     = 0;
    pSmsc[0]        = 0;
    *pMessage       = 0;
    
    if( *pTextMsgLength < MAX_SMS_MSG_LEN )
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    if((VALID_PDU_MESSAGE_SIZE * 2) < PduLength)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pTextMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    /* Get senders pSmsc */
    if (PduLength < 2)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pTextMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    Pointer = (int8_t *)pPdu;
    RefPointer = (int8_t *)pPdu;

    /* Get senders pSmsc */
    Length = sms_Octet2bin(Pointer)*2;

    if ((PduLength - 2) < Length)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pTextMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    if ( *pScAddrLength < Length )
    {
        DEBUG_ERROR_PRINT;
        *pScAddrLength = Length;
        snprintf((char*)pMessage,*pTextMsgLength,"PDU Src Addr Size :%d.\n",Length);
        return sMS_HELPER_ERR_BUFFER_SZ;
    }

    if ( Length > 2 && Length <= MAX_SMSC_LENGTH )
    {
        Pointer = Pointer+2;

        /* remain type of address at the begining */
        strncpy((char*)pSmsc,(char*)Pointer,Length);
        pSmsc[Length]='\0';
        sms_Swapchars(pSmsc+2);
        if (pSmsc[Length -1]=='F')
            pSmsc[Length -1]=0;
        Pointer = Pointer + Length ;
    }
    else
    {
        *pScAddrLength = Length;
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* If length is less to extract  UDH and message type */
    if ((PduLength - (Pointer - RefPointer)) < 2)
    {
        DEBUG_ERROR_PRINT;
        snprintf((char*)pMessage,*pTextMsgLength,"Invalid PDU Size\n");
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    /* Is UDH bit set?*/
    if ( sms_Octet2bin(Pointer) & 64 )
    {
        with_udh = 1;
        if (pWithUDH != NULL)
        {
            *pWithUDH = 1;
        }
    }
    Type = sms_Octet2bin(Pointer) & 3;
    Pointer += 2;

    /* SMS Deliver */
    if ( Type == ePDU_TYPE_0 )
    {
        u16NewPduLength = PduLength -(Pointer - RefPointer);
        return sms_ExtractSMSDeliver( Pointer,
                                  u16NewPduLength,
                                  pSenderLength,\
                                  pSendr,pDate,pTime,pTextMsgLength,\
                                  pMessage,with_udh, pReferenceNum,\
                                  pTotalNum, pSequenceNum);

    }
    /*  Sent message */
    else if ( Type == ePDU_TYPE_1 )
    {
        if(PDU_TYPE1_HEADER_LENGTH > PduLength)//18
        {
            snprintf((char*)pMessage,*pTextMsgLength,"Unexptected PDU Size Type:%d.\n",Type);
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }
        Pointer+=2;
        u16NewPduLength = PduLength -(Pointer - RefPointer);
        return sms_ExtractSMSSent( Pointer , 
                               u16NewPduLength,
                               pSenderLength,\
                               pSendr,pDate,pTime,pTextMsgLength,\
                               pMessage,with_udh, pReferenceNum,\
                               pTotalNum, pSequenceNum);
    }
    else
    /* Unsupported type */
    {
        snprintf((char*)pMessage,*pTextMsgLength,"Message type (%x) is not supported."
                        " Cannot decode.\n",Type);
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }
}

int sms_SLQSWCDMADecodeMTTextMsg(
        sms_SLQSWCDMADecodeMTTextMsg_t *pReqParam
        )
{
    int rtn;

    uint8_t  smsString[SMS_ASCII_STRING_ARRAY_SIZE + 1] = {'\0'};
    if( (NULL == pReqParam) ||
        (NULL == pReqParam->pMessage) ||
        (0 == pReqParam->MessageLen))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if (((pReqParam->MessageLen) * 2) > SMS_ASCII_STRING_SIZE)
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }

    /* Covert message into dump of hex string */
    sms_SwiHex2Asc(smsString,SMS_ASCII_STRING_SIZE,\
        pReqParam->pMessage,\
        pReqParam->MessageLen );

    /* Extract message details */
    rtn = sms_SwiExtractpdu((char*)smsString,
                                strlen((char*)smsString),
                                pReqParam->senderAddr,
                                &pReqParam->senderAddrLength,
                                &pReqParam->textMsgLength,
                                &pReqParam->scAddrLength,
                                &pReqParam->date,
                                &pReqParam->time,
                                pReqParam->textMsg,
                                (int8_t*)pReqParam->scAddr,
                                NULL,
                                NULL,
                                NULL,
                                NULL );

    return rtn;
}

/*
 * Extract encoding type from WCDMA SMS PDU
 *
 * \params  pPdu[IN]          - pointer to GSM PDU string.
 *          PduLength[IN]     - Pdu Length.
 *          pEncoding[OUT]    - encoding scheme of PDU
 *          pUserDataLen[OUT] - length of user data
 *
 * \return  sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes   None.
 */

static int sms_SwiExtractEncodingWcdma(
    char *pPdu,
    uint16_t PduLength,
    uint8_t *pEncoding,
    uint16_t *pUserDataLen)
{
    uint8_t  Length    = 0;
    uint8_t  Type      = 0 ;
    int8_t  *Pointer  = NULL;
    int with_udh  = 0;
    uint16_t padding;
    uint16_t udh_size = 0;
    uint16_t userLen = 0;
    uint8_t encoding = 0xFF;

    if ( (!pPdu) ||  (!pEncoding) || (!pUserDataLen) )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if(((VALID_PDU_MESSAGE_SIZE * 2) < PduLength) ||
        (PduLength < 2))
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    Pointer = (int8_t *)pPdu;

    /* Get senders Smsc length */
    Length = sms_Octet2bin(Pointer)*2;
    PduLength = PduLength - 2;
    Pointer = Pointer + 2;

    if ((PduLength < Length) || 
        (Length > MAX_SMSC_LENGTH) ||
        (Length == 0))
    {
        DEBUG_ERROR_PRINT; 
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    PduLength = PduLength - Length;
    Pointer = Pointer + Length;

    /* If length is less to extract message type */
    if (PduLength < 2)
    {
        DEBUG_ERROR_PRINT;
        return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
    }

    Type = sms_Octet2bin(Pointer) & 3;
    if ( sms_Octet2bin(Pointer) & 64 )
        with_udh  = 1;
    PduLength = PduLength - 2;
    Pointer = Pointer + 2;

    /* SMS Deliver */
    if ( (Type == ePDU_TYPE_0) && (PduLength > 2))
    {
        Length      = sms_Octet2bin(Pointer);
        padding     = Length % 2;
        PduLength = PduLength - 2;
        Pointer = Pointer + 2;

        if ((Length > MAX_SENDER_ADDRESS_LENGTH) ||
            (PduLength < (Length + padding + 2)))
        {
            DEBUG_ERROR_PRINT;        
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }

        PduLength = PduLength - (Length + padding + 2);
        Pointer = Pointer + (Length + padding + 2);

        /* skip two octets of protocol identifier */
        if (PduLength < 2)
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }
 
        PduLength = PduLength - 2;
        Pointer = Pointer + 2;
 
        if (PduLength < 2)
        {
            DEBUG_ERROR_PRINT;
            return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
        }
        encoding = sms_Octet2bin(Pointer);
        *pEncoding = encoding & 0x0F;

        if (encoding == UCSENCODING)
        {
            PduLength = PduLength - 2;
            Pointer = Pointer + 2;

            /* 6(date)+ 8 (timestamp) + 2 (uderdatalength) */
            if ( PduLength < 16)
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
            }
            Pointer = Pointer + 14;
            PduLength = PduLength - 14;

            userLen = sms_Octet2bin(Pointer);
            PduLength = PduLength - 2;
            Pointer = Pointer + 2;

            if (PduLength != (userLen * 2))
            {
                DEBUG_ERROR_PRINT;
                return sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED;
            }
            if (with_udh)
                udh_size = sms_Octet2bin(Pointer);
            *pUserDataLen = (userLen - udh_size);
        }
    }
    else
    /* Unsupported type */
    {
        return sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED;
    }
    return sMS_HELPER_OK;
}

int sms_SLQSWCDMADecodeMTEncoding(
    uint8_t *pMessage,
    uint16_t MessageLen,
    uint8_t  *pEncoding,
    uint16_t *pUserDataLen
)
{
    int resultCode;
    uint8_t  smsString[SMS_ASCII_STRING_SIZE + 1] = {'\0'};

    if ( (!pMessage) ||  (!pEncoding) || (!pUserDataLen ))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* Covert message into dump of hex string */
    sms_SwiHex2Asc(smsString,SMS_ASCII_STRING_SIZE, \
        pMessage,\
        MessageLen);

    /* Extract encoding */
    resultCode = sms_SwiExtractEncodingWcdma((char *)smsString,
                            strlen((char *)smsString),
                            pEncoding,
                            pUserDataLen);
    return resultCode;
}

/*
 * Make PDU string for concatenated sms
 *
 * \param  pNumber[IN]     - Mobile number entered by user.
 *         pMessage[IN]   - Sms to send.
 *         messagelen[IN] - Length of sms.
 *         pPdu[OUT]      - Dynamically allocated memory pointer
 *                          which will hold final pdu message hex dump.
 *         Pdulen[IN]     - pdu buffer size.
 *         alphabet [IN]  - message encoding.
 *                          - 0 -Default 7 bit encoding
 *                          - 4 -Eight bit encoding
 *                          - 8 -UCS-2 encoding
 *         refnumber [IN] - reference number of sms.
 *         totalnumb [IN] - total number of sms.
 *         seqnumber [IN] - sequence number of sms.
 *
 * \return sMS_HELPER_OK on success, sMS_HELPER_ERR_xxx error value otherwise.
 *
 * \notes  none.
 */

int sms_SwiMakeConcatenatedSmsPdu(
    char   *pNumber,
    char   *pMessage,
    uint16_t messagelen,
    char   *pPdu,
    uint16_t Pdulen,
    uint8_t   alphabet,
    uint8_t   refnumber,
    uint8_t   totalnumb,
    uint8_t   seqnumber )
{
    uint16_t coding = 0; /* for 7bit coding */
    uint16_t sms_summit_flags;
    char   tmp[MAX_SMS_NUMBER_ARRAY_LENGTH]={0};
    char   pdutmp[MAXIMUM_PDU]={0};
    uint16_t numberformat;
    uint16_t numberlength;
    int nRet;
    uint8_t udh[UDH_TOTAL_FIELD_NUMBER_ARRAY_SIZE]={0};
    uint16_t pdutempLength = sizeof(pdutmp);
    uint16_t udhLength = LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER;

    if ( UCSENCODING == alphabet )
    {
        coding = UCSENCODING; /* UCS bit coding */
    }
    else if ( EIGHTBITENCODING == alphabet )
    {
        coding = EIGHTBITENCODING;
    }
    else
    {
        coding = DEFAULTENCODING;
    }

    if ( !pNumber   ||
         !pMessage ||
         !pPdu )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    if ((coding == UCSENCODING) && 
        (messagelen > MAX_LONG_SMS_MSG_TOTAL_LEN_UCS))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    else if ((coding == EIGHTBITENCODING) && 
             (messagelen > MAX_LONG_SMS_MSG_CHAR_LEN_EIGHTBIT))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    else if ((coding == DEFAULTENCODING) && 
             (messagelen > MAX_LONG_SMS_MSG_CHAR_LEN_DEFAULT))
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }

    numberlength = strlen(pNumber);

    if ((numberlength > MAX_SMS_NUMBER_LENGTH) || (numberlength == 0))
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    strcpy(tmp,pNumber);
    if (tmp[0] == '+' || (numberlength > 20))
    {
        numberformat = NUMBERFORMAT;
        numberlength = numberlength -1;
        strcpy(tmp,&pNumber[1]);
    }
    else
    {
        numberformat = NATIONALFORMAT;
    }

    if( NULL != (strchr( tmp, ENTER_KEY )) )
    {
        numberlength = numberlength -1;
        tmp[numberlength] = 0;
    }

    /* terminate the number with F if the length is odd */
    if ( numberlength%2 )
    {
        strcat(tmp,"F");
    }

    /* Swap every second character to make PDU Message */
    nRet = sms_Swapchars((int8_t*)tmp);

    /* Set the UDH bit in the first octet of the PDU. For an SMS-SUBMIT PDU,
       the value is normally 0x01. To indicate that a UDH is present we need
       to set bit 6 (0x40). So for an SMS-SUBMIT with UDH present we set the
       PDU type to 0x41 */
    sms_summit_flags = 0x41;

    /* Set user data header information */
    /* length of UDH */
    udh[LITE_SMSHELPER_UDH_LENGTH] = 0x05;
    /* Information element identifier for a concatenated short message */
    udh[LITE_SMSHELPER_INFORMATION_ELEMENT_ID] = INFORMATION_ELEMENT_IDENTIFIER;
    /* Information element data length */
    udh[LITE_SMSHELPER_INFORMATION_ELEMENT_ID_LENGTH] = 0x03;
    /* A reference number (must be the same for all parts of the same larger messages) */
    udh[LITE_SMSHELPER_SMS_REFERENCE_NUMBER] = refnumber;
    /* Total number of the concatenated message */
    udh[LITE_SMSHELPER_SMS_TOTAL_NUMBER] = totalnumb;
    /* Sequence number of the current message */
    udh[LITE_SMSHELPER_SMS_SEQUENCE_NUMBER] = seqnumber;
    udh[LITE_SMSHELPER_UDH_TOTAL_FIELD_NUMBER] = '\0';

    /* if it is 7bit or 8bit encoding, check if any GSM extended character presents in the message */
    if ( alphabet != UCSENCODING_CHAR )
    {
        if (EIGHTBITENCODING_CHAR == alphabet)
        {
            messagelen = sms_Eightbitpdu(pMessage,messagelen,
                pdutmp,pdutempLength, 
                udh,udhLength);
        }
        else
        {
            messagelen = sms_Text2Pdu( (int8_t*)pMessage, messagelen,
                (int8_t*)pdutmp,pdutempLength, 
                udh ,udhLength);
        }
    }
    else
    {
        messagelen = sms_Ucs2pdu(pMessage,messagelen,
            pdutmp,pdutempLength,
            udh,udhLength);
    }

    /* The first 00 means length of SMSC information, the second 00 is TP-Message-Reference field.
       The "00" value here lets the phone set the message reference number itself,
       the third 00 is TP-PID field, Protocol identifier */
    snprintf( pPdu,
             Pdulen,
             "00%02X00%02X%02X%s00%02X%02X",
             sms_summit_flags,
             numberlength,
             numberformat,
             tmp,
             coding,
             messagelen );
    if(Pdulen < (strlen((char*)pdutmp)+strlen((char*)pPdu)))
    {
        return sMS_HELPER_ERR_BUFFER_SZ;
    }
    /* concatenate the text to the PDU string */
    strcat(pPdu,pdutmp);
    return nRet;
}

int sms_SLQSWCDMADecodeLongTextMsg(
    sms_SLQSWCDMADecodeLongTextMsg_t *pWcdmaLongMsgDecodingParams )
{
    if ( !pWcdmaLongMsgDecodingParams                    ||
         !pWcdmaLongMsgDecodingParams->pMessage          ||
         !pWcdmaLongMsgDecodingParams->pSenderAddrLength ||
         !pWcdmaLongMsgDecodingParams->pSenderAddr       ||
         !pWcdmaLongMsgDecodingParams->pTextMsgLength    ||
         !pWcdmaLongMsgDecodingParams->pTextMsg          ||
         !pWcdmaLongMsgDecodingParams->pScAddrLength     ||
         !pWcdmaLongMsgDecodingParams->pScAddr           ||
         !pWcdmaLongMsgDecodingParams->pReferenceNum     ||
         !pWcdmaLongMsgDecodingParams->pTotalNum         ||
         !pWcdmaLongMsgDecodingParams->pPartNum          ||
         !pWcdmaLongMsgDecodingParams->pIsUDHPresent )
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    int rtn;
    uint8_t  smsString[SMS_ASCII_STRING_ARRAY_SIZE] = {'\0'};
    if (((pWcdmaLongMsgDecodingParams->MessageLen) * 2) > SMS_ASCII_STRING_SIZE)
    {
        return sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG;
    }
    if(MAX_SMS_MSG_LEN < *pWcdmaLongMsgDecodingParams->pTextMsgLength)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }

    /* Covert message into dump of hex string */
    sms_SwiHex2Asc(smsString,SMS_ASCII_STRING_SIZE, \
        pWcdmaLongMsgDecodingParams->pMessage,\
        pWcdmaLongMsgDecodingParams->MessageLen);

    /* Extract message details */
    rtn = sms_SwiExtractpdu((char *)smsString,
                                strlen((char*)smsString),
                                (int8_t*)pWcdmaLongMsgDecodingParams->pSenderAddr,
                                pWcdmaLongMsgDecodingParams->pSenderAddrLength,
                                pWcdmaLongMsgDecodingParams->pTextMsgLength,
                                pWcdmaLongMsgDecodingParams->pScAddrLength,
                                &pWcdmaLongMsgDecodingParams->Date,
                                &pWcdmaLongMsgDecodingParams->Time,
                                (int8_t*)pWcdmaLongMsgDecodingParams->pTextMsg,
                                (int8_t*)pWcdmaLongMsgDecodingParams->pScAddr,
                                pWcdmaLongMsgDecodingParams->pReferenceNum,
                                pWcdmaLongMsgDecodingParams->pTotalNum,
                                pWcdmaLongMsgDecodingParams->pPartNum,
                                pWcdmaLongMsgDecodingParams->pIsUDHPresent );

    return rtn;
}

uint32_t sms_SLQSCDMADecodeMTTextMsgExt(
    struct sms_cdmaMsgDecodingParamsExt_t *pCdmaMsgDecodingParamsExt)
{
    uint32_t resultCode;

    /* Check if IN/OUT structure is NULL */
    if ( !pCdmaMsgDecodingParamsExt                    ||
         !pCdmaMsgDecodingParamsExt->pMessage          ||
         !pCdmaMsgDecodingParamsExt->pSenderAddrLength ||
         !pCdmaMsgDecodingParamsExt->pSenderAddr       ||
         !pCdmaMsgDecodingParamsExt->pTextMsgLength    ||
         !pCdmaMsgDecodingParamsExt->pTextMsg          ||
         !pCdmaMsgDecodingParamsExt->pReferenceNum     ||
         !pCdmaMsgDecodingParamsExt->pTotalNum         ||
         !pCdmaMsgDecodingParamsExt->pPartNum          ||
         !pCdmaMsgDecodingParamsExt->pIsUDHPresent
         )
    {
         return sMS_HELPER_ERR_INVALID_ARG;
    }
    if(MAX_SMS_MSG_LEN < *pCdmaMsgDecodingParamsExt->pTextMsgLength)
    {
        return sMS_HELPER_ERR_INVALID_ARG;
    }
    /* Execute function to decode CDMA SMS */
    resultCode = sms_SwiCDMADecodeMTTextMsg(
                         pCdmaMsgDecodingParamsExt->messageLength,
                         pCdmaMsgDecodingParamsExt->pMessage,
                         pCdmaMsgDecodingParamsExt->pMessageID,
                         pCdmaMsgDecodingParamsExt->pSenderAddrLength,
                         pCdmaMsgDecodingParamsExt->pSenderAddr,
                         pCdmaMsgDecodingParamsExt->pTextMsgLength,
                         pCdmaMsgDecodingParamsExt->pTextMsg,
                         pCdmaMsgDecodingParamsExt->pPriority,
                         pCdmaMsgDecodingParamsExt->pPrivacy,
                         pCdmaMsgDecodingParamsExt->pLanguage,
                         &pCdmaMsgDecodingParamsExt->mcTimeStamp,
                         &pCdmaMsgDecodingParamsExt->absoluteValidity,
                         pCdmaMsgDecodingParamsExt->pRelativeValidity,
                         pCdmaMsgDecodingParamsExt->pDisplayMode,
                         pCdmaMsgDecodingParamsExt->pUserAcknowledgementReq,
                         pCdmaMsgDecodingParamsExt->pReadAcknowledgementReq,
                         pCdmaMsgDecodingParamsExt->pAlertPriority,
                         pCdmaMsgDecodingParamsExt->pCallbkAddrLength,
                         pCdmaMsgDecodingParamsExt->pCallbkAddr,
                         pCdmaMsgDecodingParamsExt->pReferenceNum,
                         pCdmaMsgDecodingParamsExt->pTotalNum,
                         pCdmaMsgDecodingParamsExt->pPartNum,
                         pCdmaMsgDecodingParamsExt->pIsUDHPresent);

    return resultCode;
}
