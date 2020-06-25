#define __STDC_FORMAT_MACROS
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <errno.h>
#include <locale.h>
#include <langinfo.h>
#include "packingdemo.h"

#include "sms.h"
#include "helper_sms.h"

#define IFPRINTF(s,p)   if( NULL != p ){ printf(s,*p); }

#define PHONE_NUM "96818255"
#define maxSMSCAddrLen  0x15
#define maxSMSCTypeLen  0x04
#define MESSAGE_FORMAT_CDMA  0
#define MESSAGE_FORMAT_GW_PP 6
#define STORAGE_UIM          0
#define STORAGE_NV           1

volatile int enSmsThread;
int sms = -1;
pthread_attr_t sms_attr;
pthread_t sms_tid = 0;
uint32_t storageType = 0; /* default UIM */
uint8_t messageMode = 1; /* default GW */
uint32_t messageFormat = MESSAGE_FORMAT_GW_PP; /* default GSM 8 */
char phoneNum[20]={0};
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;


#define MAX_COPY_BUF_SIZE 255
static  char *copy_buf[MAX_COPY_BUF_SIZE] = {NULL};
static uint8_t buf_allocated = FALSE;
static uint8_t concatenated_sms_counter = 0;
static uint8_t concate_prev_ref_number = 0;
static uint8_t concate_prev_total_number = 0;
void sms_test_ind_exit();
int sms_validate_dummy_unpack();

/****** Modem Settings*******/
uint8_t DefaultSMSCAddress[255] = "";
uint8_t DefaultSMSCType[255] = "";
pack_sms_SetSMSCAddress_t DefaultSMSSCAddressSettings = { DefaultSMSCAddress, DefaultSMSCType};
sms_qaQmi3GPPBroadcastCfgInfo  DefaultlBroadcastConfig = { 0x00, 0x02, {{0x0102, 0x0304, 0x01}} };
sms_qaQmi3GPP2BroadcastCfgInfo DefaultlCDMABroadcastConfig = { 0x00, 0x02, {{ 0x0102, 0x0304, 0x01}} };
pack_sms_SLQSSetSmsBroadcastConfig_t DefaultSmsBroadcastConfigSettings = { 0x01, &DefaultlBroadcastConfig, &DefaultlCDMABroadcastConfig};
uint8_t DefaultregTransLayerInfoEvt = 0xFF;
uint8_t DefaultregTransNWRegInfoEvt = 0xFF;
uint8_t DefaultregCallStatInfoEvt = 0xFF;
sms_getIndicationReg DefaultindicationRegInfo = { &DefaultregTransLayerInfoEvt, &DefaultregTransNWRegInfoEvt, &DefaultregCallStatInfoEvt};
unpack_sms_SLQSGetIndicationRegister_t DefaultIndicationRegisterSettings = { &DefaultindicationRegInfo,SWI_UINT256_INT_VALUE};
pack_sms_SLQSSetSmsStorage_t DefaultSmsStorageSettings = { 0x01};/*Device's Permanent Memory */
/****************************/
void sms_dummy_unpack();
void sms_helper_test();

#if 1
#define DEBUG_STDOUT(f_, ...) fprintf(stdout,(f_), ##__VA_ARGS__);\
        fprintf(stdout,"%s:%d\n",__FUNCTION__,__LINE__);
#define DEBUG_STDERR(f_, ...) fprintf(stderr,(f_), ##__VA_ARGS__);\
        fprintf(stderr,"%s:%d\n",__FUNCTION__,__LINE__);
#else
#define DEBUG_STDOUT(f_, ...) UNUSEDPARAM(f_)
#define DEBUG_STDERR(f_, ...) UNUSEDPARAM(f_)
#endif

/* conversion of a UCS-2 char to UTF multibyte character */
uint8_t smsdemo_UCS2Char_To_UTF8Char(uint16_t ucs2, char* utf8, uint16_t *pTokenLen, uint16_t UTFLen)
{
    if ((utf8 == NULL) || (pTokenLen == NULL))
        return 0;

    if (ucs2 < 0x80) 
    {
        /* 1 tokenlen + 1 NULL character */
        if (UTFLen < 2)
            return 0;
        utf8[0] = ucs2;
        *pTokenLen = 1;
        return 1;
    }
    else if (ucs2 < 0x800) 
    {
        /* 2 tokenlen + 1 NULL character */
        if (UTFLen < 3)
            return 0;
        utf8[0] = (ucs2 >> 6)   | 0xC0;
        utf8[1] = (ucs2 & 0x3F) | 0x80;
        *pTokenLen = 2;
        return 1;
    }
    else if (ucs2 < 0xFFFF) 
    {
        /* 3 tokenlen + 1 NULL character */
        if (UTFLen < 4)
            return 0;
        utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
        utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
        utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
        *pTokenLen = 3;
        return 1;
    }
    else
    {
        return 0;
    }
}

/* conversion SMS from UCS-2 to UTF-8 format */
uint8_t smsdemo_conversionUCS2msgToUTF(
    char *pSms, 
    uint16_t inputStrLen, 
    char  *pUtf,
    uint16_t UTFBufLen
    )
{
    uint16_t UCSChar = 0;
    uint16_t outputStrTokenLen = 0;
    uint16_t inputStrIndex = 0;
    uint16_t outStrLen = 0;
    uint8_t   bRet = 0;

    if ((pSms == NULL) || (pUtf == NULL))
        return 0;

    if (inputStrLen > UTFBufLen)
        return 0;

    /* Length is not even, so no UCS2 */
    if ((inputStrLen % 2) != 0)
        return 0;
     
    while (inputStrIndex < inputStrLen) 
    {
        UCSChar= (uint16_t)(((pSms[0] << 8) & 0xFF00) | ((pSms[1]) & 0x00FF));
        outputStrTokenLen = 0;
        
        bRet = smsdemo_UCS2Char_To_UTF8Char(UCSChar, pUtf, &outputStrTokenLen, UTFBufLen);
        if (bRet) 
        {
            inputStrIndex = inputStrIndex + 2;
            pSms = pSms + 2;
            outStrLen =  outStrLen + outputStrTokenLen;
            UTFBufLen = UTFBufLen - outputStrTokenLen;
            pUtf = pUtf + outputStrTokenLen;
            
        } 
        else 
        {
           fprintf(stderr, "UCS-2 char can not be converted to UCS-2 format\n");
           return 0;
        }          
    }
    pUtf[outStrLen] = '\0';
    return 1;
}


/* conversion a UTF multibyte character to UCS-2 */
uint8_t smsdemo_UTF8Char_To_UCS2Char(char* utf8TokUs, uint16_t* ucs2Char, uint16_t* utf8TokLen)
{
    if ((utf8TokUs == NULL) || (utf8TokLen == NULL) || (ucs2Char == NULL))
        return 0;

    if (0x80 > ((unsigned char)utf8TokUs[0]))
    {
        /* 1 byte UTF-8 char */
        *ucs2Char = (uint16_t)(utf8TokUs[0]);
        *utf8TokLen = 1;
    }
    else if (0xC0 ==((unsigned char) (utf8TokUs[0] & 0xE0)))
    {
        /* 2 bytes UTF-8 char */
        if ( 0x80 != (utf8TokUs[1] & 0xC0))
            return 0;
        else {
        *utf8TokLen = 2;
        *ucs2Char = (uint16_t)((utf8TokUs[0] & 0x1F) << 6
                | (utf8TokUs[1] & 0x3F));
        }
    }
    else if (0xE0 == (utf8TokUs[0] & 0xF0))
    {
        /* 3 bytes UTF-8 char */
        if (( 0x80 != ((unsigned char) (utf8TokUs[1] & 0xC0)))
            || (0x80 != ((unsigned char) (utf8TokUs[2] & 0xC0))))
            return 0;
        else {
        *utf8TokLen = 3;
        *ucs2Char = (uint16_t)((utf8TokUs[0] & 0x0F) << 12
                | (utf8TokUs[1] & 0x3F) << 6
                | (utf8TokUs[2] & 0x3F));
        }
    }
    else if (0xF0 == ((unsigned char) (utf8TokUs[0] & 0xF8)))
    {
       /* More than 3 bytes UTF-8 char, exceeds UCS-2 range */
        return 0;
    }
    else 
    {
       return 0;
    }    
    return 1;
}

/* conversion complete UTF-8 format SMS to UCS-2 */
uint8_t smsdemo_conversionUTFmsgToUCS2(
    char* pSms, 
    uint16_t inputStrLen, 
    char* pSmsUCS,
    uint16_t ucsBufferLen,
    uint16_t *pUcsMsgLen)
{
    uint16_t UCSChar = 0;
    uint16_t inputStrTokenLen = 0;
    uint16_t inputStrIndex = 0;
    uint16_t ucsStrIndex = 0;
    uint8_t   bRet = 0;

    if ((pSms == NULL) || (pSmsUCS == NULL) || (pUcsMsgLen == NULL))
        return 0;

    while (inputStrIndex < inputStrLen) {
        bRet = smsdemo_UTF8Char_To_UCS2Char(pSms, &UCSChar, &inputStrTokenLen);
        if (bRet) {
            inputStrIndex += inputStrTokenLen;
            pSms = pSms + inputStrTokenLen;

            if (ucsStrIndex < (ucsBufferLen -2)) {
                pSmsUCS[ucsStrIndex] = (char)(UCSChar >> 8);
                pSmsUCS[ucsStrIndex + 1] = (char)UCSChar;
                    
                ucsStrIndex = ucsStrIndex + 2;
            } else {
                fprintf(stderr, "UCS-2 message buffer falls short\n");
                return 0;
            }  
            
        } else {
           fprintf(stderr, "UTF char can not be converted to UCS-2 format\n");
           return 0;
        }          
    }
    pSmsUCS[ucsStrIndex] = '\0';
    *pUcsMsgLen = ucsStrIndex;
    return 1;
}


void dump_SLQSGetSMS(void* ptr)
{
    unpack_sms_SLQSGetSMS_t  *result =
        (unpack_sms_SLQSGetSMS_t*) ptr;

    uint32_t count = 0;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("SLQS Get SMS Successful\n");
        printf("Message Tag : %u\n",result->messageTag);
        printf("Message Format : %u\n",result->messageFormat);
        printf("Message Size : %u\n",\
                result->messageSize);
        printf("Message : ");
        for ( count=0;
              count < result->messageSize;
              count++ )
            {
                if(count >= MAX_SMS_MESSAGE_SIZE)
                    break;
                printf(" %x ",(result->message[count]));
            }
    }
    printf("\n%s Done\n",__FUNCTION__);
}

void dump_SLQSGetSMSList(void* ptr)
{
    unpack_sms_SLQSGetSMSList_t  *result =
        (unpack_sms_SLQSGetSMSList_t*) ptr;
    uint8_t  count = 0;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result) && 
        swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("MessageListSize  : %x\n", result->messageListSize);
        for(count = 0; count < result->messageListSize; count++)
        {
            printf( "messageIndex[%d] : %x\n",
                    count, result->messageList[count].messageIndex);
            printf( "messageTag[%d] : %x\n",
                    count, result->messageList[count].messageTag);
        }
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSModifySMSStatus(void* ptr)
{
    unpack_sms_SLQSModifySMSStatus_t  *result =
        (unpack_sms_SLQSModifySMSStatus_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result)
    {


    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSDeleteSMS(void* ptr)
{
    unpack_sms_SLQSDeleteSMS_t  *result =
        (unpack_sms_SLQSDeleteSMS_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result)
    {


    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSendSMS(void* ptr)
{
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}


void dump_SetNewSMSCallback(void* ptr)
{
    if(ptr==NULL)
    {
        
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_GetSMSCAddress(void* ptr)
{
    unpack_sms_GetSMSCAddress_t  *result =
        (unpack_sms_GetSMSCAddress_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf( "\nSMSC Address : ");
        if(result->pSMSCAddress)
        printf("%s",result->pSMSCAddress);
        printf( "\nSMSC Type : ");
        if(result->pSMSCType)
        printf( "%s\n", result->pSMSCType);
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SetSMSCAddress(void* ptr)
{
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SaveSMS(void* ptr)
{
    unpack_sms_SaveSMS_t  *result =
        (unpack_sms_SaveSMS_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pMessageIndex) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf( "Message Index is : %u\n", *result->pMessageIndex );
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetSmsBroadcastConfig(void* ptr)
{
    unpack_sms_SLQSGetSmsBroadcastConfig_t  *result =
        (unpack_sms_SLQSGetSmsBroadcastConfig_t*) ptr;
    uint16_t lTemp = 0;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pBroadcastConfig!=NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "3GPP Broadcast Configuration Info\n");
        printf( "Activated Index  :%x\n",
            result->pBroadcastConfig->activated_ind);
        printf( "Number of Sets   :%x\n",
            result->pBroadcastConfig->num_instances);
        for (lTemp=0;lTemp<result->pBroadcastConfig->num_instances;lTemp++)
        {
            printf( "From Service Id  [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].fromServiceId);
            printf( "To Service Id    [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].toServiceId);
            printf( "Selected         [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].selected);
        }
    }
    if((result->pCDMABroadcastConfig!=NULL)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf( "3GPP2 Broadcast Configuration Info\n");
        printf( "Activated Index  :%x\n",
            result->pCDMABroadcastConfig->activated_ind);
        printf( "Number of Sets   :%x\n",
            result->pCDMABroadcastConfig->num_instances);
        for (lTemp=0;lTemp<result->pCDMABroadcastConfig->num_instances;lTemp++)
        {
            printf( "Service Category [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].serviceCategory);
            printf( "Language         [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].language);
            printf( "Selected         [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].selected);
        }
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSetSmsBroadcastConfig(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSetSmsBroadcastActivation(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetTransLayerInfo(void* ptr)
{
    unpack_sms_SLQSGetTransLayerInfo_t  *result =
        (unpack_sms_SLQSGetTransLayerInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pGetTransLayerInfo->pRegInd!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    printf( "Registered Indication        : 0x%x \n", *result->pGetTransLayerInfo->pRegInd );

    if((result->pGetTransLayerInfo->pTransLayerInfo!=NULL)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf( "Transport Type        : 0x%x \n",
                            result->pGetTransLayerInfo->pTransLayerInfo->TransType);
        printf( "Transport Capabilities: 0x%x \n",
                            result->pGetTransLayerInfo->pTransLayerInfo->TransCap);
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetTransNWRegInfo(void* ptr)
{
    unpack_sms_SLQSGetTransNWRegInfo_t  *result =
        (unpack_sms_SLQSGetTransNWRegInfo_t*) ptr;
    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pGetTransNWRegInfo->pRegStatus!=NULL)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    printf( "Registration Status        : 0x%x \n", *result->pGetTransNWRegInfo->pRegStatus );
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetIndicationRegister(void* ptr)
{
    unpack_sms_SLQSGetIndicationRegister_t  *result =
        (unpack_sms_SLQSGetIndicationRegister_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pGetIndicationRegInfo->pRegTransLayerInfoEvt!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    printf( "Transport layer Info events registration status: 0x%x \n",
            *result->pGetIndicationRegInfo->pRegTransLayerInfoEvt );
    if((result->pGetIndicationRegInfo->pRegTransNWRegInfoEvt!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    printf( "Transport Network Reg events registration Status: 0x%x \n",
            *result->pGetIndicationRegInfo->pRegTransNWRegInfoEvt );
    if((result->pGetIndicationRegInfo->pRegCallStatInfoEvt!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    printf( "Call Status Info events: 0x%x \n",
            *result->pGetIndicationRegInfo->pRegCallStatInfoEvt );
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSetIndicationRegister(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSmsSetRoutes(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSmsGetMessageProtocol(void* ptr)
{
    unpack_sms_SLQSSmsGetMessageProtocol_t  *result =
        (unpack_sms_SLQSSmsGetMessageProtocol_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pMessageProtocol!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf( "Message Protocol: %x\n", result->pMessageProtocol->msgProtocol );
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSmsGetMaxStorageSize(void* ptr)
{
    unpack_sms_SLQSSmsGetMaxStorageSize_t  *result =
        (unpack_sms_SLQSSmsGetMaxStorageSize_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result->pMaxStorageSizeResp!=NULL)
    {
        if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Max Storage Size: %u\n", result->pMaxStorageSizeResp->maxStorageSize);
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf( "Storage Free Slots: %u \n", result->pMaxStorageSizeResp->freeSlots );
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetMessageWaiting(void* ptr)
{
    unpack_sms_SLQSGetMessageWaiting_t  *result =
        (unpack_sms_SLQSGetMessageWaiting_t*) ptr;
    uint8_t i;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pGetMsgWaitingInfoResp!=NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    for ( i = 0; i < result->pGetMsgWaitingInfoResp->numInstances; i++)
    {
        printf( "message type: %d\n", result->pGetMsgWaitingInfoResp->msgWaitInfo[i].msgType);
        printf( "active indication: %d\n", result->pGetMsgWaitingInfoResp->msgWaitInfo[i].activeInd);
        printf( "message count: %d\n", result->pGetMsgWaitingInfoResp->msgWaitInfo[i].msgCount);
        printf( "\n");
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSendAsyncSMS(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSetSmsStorageDevice(void* ptr)
{
   if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSwiGetSMSStorage(void* ptr)
{
    unpack_sms_SLQSSwiGetSMSStorage_t  *result =
        (unpack_sms_SLQSSwiGetSMSStorage_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pSmsStorage!=NULL)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf( "SMS Storage: %x\n", *result->pSmsStorage );
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetSmsBroadcastConfigSettings(void* ptr)
{
    unpack_sms_SLQSGetSmsBroadcastConfig_t  *result =
        (unpack_sms_SLQSGetSmsBroadcastConfig_t*) ptr;
    uint16_t lTemp = 0;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->pBroadcastConfig) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "3GPP Broadcast Configuration Info\n");
        printf( "Activated Index  :%x\n",
            result->pBroadcastConfig->activated_ind);
        printf( "Number of Sets   :%x\n",
            result->pBroadcastConfig->num_instances);
        for (lTemp=0;lTemp<result->pBroadcastConfig->num_instances;lTemp++)
        {
            printf( "From Service Id  [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].fromServiceId);
            printf( "To Service Id    [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].toServiceId);
            printf( "Selected         [%d]:%x\n",
                lTemp,result->pBroadcastConfig->broadcastConfig[lTemp].selected);
        }
        DefaultSmsBroadcastConfigSettings.pBroadcastConfig->num_instances = result->pBroadcastConfig->num_instances;
        memcpy(&DefaultSmsBroadcastConfigSettings.pBroadcastConfig->broadcastConfig,&result->pBroadcastConfig->broadcastConfig,sizeof(sms_BroadcastConfig));
    }

    if((result->pCDMABroadcastConfig) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf( "3GPP2 Broadcast Configuration Info\n");
        printf( "Activated Index  :%x\n",
            result->pCDMABroadcastConfig->activated_ind);
        printf( "Number of Sets   :%x\n",
            result->pCDMABroadcastConfig->num_instances);
        for (lTemp=0;lTemp<result->pCDMABroadcastConfig->num_instances;lTemp++)
        {
            printf( "Service Category [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].serviceCategory);
            printf( "Language         [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].language);
            printf( "Selected         [%d]:%x\n",
                lTemp,result->pCDMABroadcastConfig->CDMABroadcastConfig[lTemp].selected);
        }
        DefaultSmsBroadcastConfigSettings.pCDMABroadcastConfig->num_instances = result->pCDMABroadcastConfig->num_instances;
        memcpy(&DefaultSmsBroadcastConfigSettings.pCDMABroadcastConfig->CDMABroadcastConfig,&result->pCDMABroadcastConfig->CDMABroadcastConfig,sizeof(sms_CDMABroadcastConfig));
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_GetSMSCAddressSettings(void* ptr)
{
    unpack_sms_GetSMSCAddress_t  *result =
        (unpack_sms_GetSMSCAddress_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        if(result->pSMSCAddress)
        {
            printf( "\nSMSC Address : ");
            printf("%s",result->pSMSCAddress);
            memcpy(DefaultSMSSCAddressSettings.pSMSCAddress,result->pSMSCAddress,result->addressSize);
        }
        if(result->pSMSCType)
        {
            printf( "\nSMSC Type : ");
            printf( "%s\n", result->pSMSCType);
            memcpy(DefaultSMSSCAddressSettings.pSMSCType,result->pSMSCType,result->typeSize);
        }
    }    
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSSwiGetSMSStorageSettings(void* ptr)
{
    unpack_sms_SLQSSwiGetSMSStorage_t  *result =
        (unpack_sms_SLQSSwiGetSMSStorage_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result->pSmsStorage!=NULL)
    {
        printf( "SMS Storage: %x\n", *result->pSmsStorage);
        DefaultSmsStorageSettings.smsStorage = *result->pSmsStorage;
    }
    printf("%s Done\n",__FUNCTION__);
}

void dump_SLQSGetIndicationRegisterSettings(void* ptr)
{
    unpack_sms_SLQSGetIndicationRegister_t  *result =
        (unpack_sms_SLQSGetIndicationRegister_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result->pGetIndicationRegInfo)
    {
        printf( "Transport layer Info events registration status: 0x%x \n",
            *result->pGetIndicationRegInfo->pRegTransLayerInfoEvt );
        printf( "Transport Network Reg events registration Status: 0x%x \n",
                *result->pGetIndicationRegInfo->pRegTransNWRegInfoEvt );
        printf( "Call Status Info events: 0x%x \n",
                *result->pGetIndicationRegInfo->pRegCallStatInfoEvt );
        printf("%s Done\n",__FUNCTION__);
        *(DefaultIndicationRegisterSettings.pGetIndicationRegInfo->pRegCallStatInfoEvt) = *result->pGetIndicationRegInfo->pRegTransLayerInfoEvt;
        *(DefaultIndicationRegisterSettings.pGetIndicationRegInfo->pRegTransNWRegInfoEvt) = *result->pGetIndicationRegInfo->pRegTransNWRegInfoEvt;
        *(DefaultIndicationRegisterSettings.pGetIndicationRegInfo->pRegCallStatInfoEvt) = *result->pGetIndicationRegInfo->pRegCallStatInfoEvt;
    }
}

uint8_t tpack_sms_SLQSGetSMSmessageMode =0x01;/* GW mode */

pack_sms_SLQSGetSMS_t tpack_sms_SLQSGetSMS={
    0x00, /* UIM */
    0x00,
    &tpack_sms_SLQSGetSMSmessageMode
};
unpack_sms_SLQSGetSMS_t tunpack_sms_SLQSGetSMS={
    0xFFFFFFFF,
    0xFFFFFFFF,
    MAX_SMS_MESSAGE_SIZE,
    {0},
    SWI_UINT256_INT_VALUE
};
uint8_t tpack_sms_SLQSGetSMSListmessageModelist = 0x01;/* GW mode */
uint32_t tpack_sms_SLQSGetSMSListmessagerequestedTag = 0x00;/* MT Read */

#if 1

pack_sms_SLQSGetSMSList_t tpack_sms_SLQSGetSMSList={
    0x00, /*UIM */
    &tpack_sms_SLQSGetSMSListmessagerequestedTag,
    &tpack_sms_SLQSGetSMSListmessageModelist
};

unpack_sms_SLQSGetSMSList_t tunpack_sms_SLQSGetSMSList={
    MAX_SMS_LIST_SIZE,
    {{0,0}},
    SWI_UINT256_INT_VALUE
};
#endif
uint8_t tpack_sms_SLQSModifySMSStatusmessageModeStat = 0x01;//GW


pack_sms_SLQSModifySMSStatus_t tpack_sms_SLQSModifySMSStatus={
    0x00, /* UIM */
    (uint32_t)1,/* Index */
    0x01, /* MT Not Read */
    &tpack_sms_SLQSModifySMSStatusmessageModeStat
};

unpack_sms_SLQSModifySMSStatus_t tunpack_sms_SLQSModifySMSStatus={
    SWI_UINT256_INT_VALUE
};

uint32_t  tpack_sms_SLQSDeleteSMSmessageIndex= 0x00;
uint8_t tpack_sms_SLQSDeleteSMSmessageModeDel  = 0x01;/* GW */

pack_sms_SLQSDeleteSMS_t tpack_sms_SLQSDeleteSMS={
    0x00, /* UIM */ 
    &tpack_sms_SLQSDeleteSMSmessageIndex,
    NULL,
    &tpack_sms_SLQSDeleteSMSmessageModeDel
};

unpack_sms_SLQSDeleteSMS_t tunpack_sms_SLQSDeleteSMS={
    SWI_UINT256_INT_VALUE
};

uint8_t tpack_sms_SendSMSmessage[255] = {0x07, 0l, 0x19, 0x89, 0x88, 0x00, 0x99, 0x89, 0x11,
                         0x00, 0x0C, 0x91, 0x19, 0x48, 0x72, 0x02, 0x85, 0x07,
                         0x00, 0x00, 0xAA, 0x1A, 0xD7, 0x32, 0x7B, 0xFC, 0x6E,
                         0x97, 0x41, 0xF4, 0x37, 0x68, 0x9A, 0x2E, 0xCB, 0xE5,
                         0x61, 0xD0, 0x35, 0x2D, 0x2F, 0xB3, 0xCB, 0xF3, 0x39};

uint8_t msg_cfong[] = {
    0x00,0x11,0x00,0x08,0x81,0x79,0x62,0x43,0x14,0x00,0x00,0x00,0x02,0xF4,0x18
};
uint8_t msg_pdu_number[25] = {0};
uint8_t msg_pdu_number_size = 25;
uint8_t tpack_sms_SendSMSlinktimer  = 0x03;


uint8_t msg_96818255[] = {
    0x00,0x11,0x00,0x08,0x81,0x69,0x18,0x28,0x55,0x00,0x00,0x00,0x02,0xF4,0x18
};



pack_sms_SendSMS_t tpack_sms_SendSMS={
     0x06, /* GW-PP */
     0xFF,
     msg_pdu_number,
     &tpack_sms_SendSMSlinktimer
};

unpack_sms_SendSMS_t tunpack_sms_SendSMS={
    0,
    0,
    SWI_UINT256_INT_VALUE
};

typedef union
{
    uint8_t u8Msg[MAX_CDMA_ENC_MO_TXT_MSG_SIZE];
    uint16_t u16Msg[MAX_CDMA_ENC_MO_TXT_MSG_SIZE/2];
}SLQSCDMAEncodeMOMessage;

SLQSCDMAEncodeMOMessage sSLQSCDMAEncodeMOTextMsg={"Welcome to Sierra Wireless"};

int8_t tpack_sms_SLQSCDMAEncodeMOTextMsgDestAddr[] = PHONE_NUM;
uint8_t tpack_sms_SLQSCDMAEncodeMOTextMsgmessage[MAX_CDMA_ENC_MO_TXT_MSG_SIZE]={0};


sms_SLQSCDMAEncodeMOTextMsg_t tpack_sms_SLQSCDMAEncodeMOTextMsg={
    250,/* Msg Size */
   (uint8_t*)tpack_sms_SLQSCDMAEncodeMOTextMsgmessage,/* tpack_sms_SLQSCDMAEncodeMOTextMsgmessage */
   0x00,/*Msg ID */
   (int8_t*)tpack_sms_SLQSCDMAEncodeMOTextMsgDestAddr,/* Dest Address */
   NULL,/* pCallBackAddrr */
   26,/* textMsgLength */
   sSLQSCDMAEncodeMOTextMsg.u16Msg,/* pTextMsg */
   NULL,/* pPariority */
   2,/* Encode Alpha */
   NULL/* pRelvalidity */
};


int8_t tpack_sms_SLQSWCDMAEncodeMOTextMsgDestAddr[]= PHONE_NUM;
int8_t tpack_sms_SLQSWCDMAEncodeMOTextMsgTextMsg[]="Welcome to Sierra Wireless";


sms_SLQSWCDMAEncodeMOTextMsg_t tpack_sms_SLQSWCDMAEncodeMOTextMsg={
    0,
    (int8_t*)tpack_sms_SLQSWCDMAEncodeMOTextMsgDestAddr,
    (int8_t*)tpack_sms_SLQSWCDMAEncodeMOTextMsgTextMsg,
    {0},
    0

};
uint8_t tpack_sms_SLQSCDMADecodeMTTextMsgcdmaMessage[] = {0x00, 0x00, 0x02, 0x10, 0x02, 0x04, 0x08, 0x03, 0x24, 0x61, 0x09, 0xca,
                             0x96, 0x1e, 0x80, 0x08, 0x26, 0x00, 0x03, 0x20, 0x00, 0x00, 0x01, 0x19,
                             0x10, 0xd5, 0x7c, 0xbb, 0x31, 0xef, 0xdb, 0x95, 0x07, 0x4d, 0xe8, 0x29,
                             0xe9, 0xcb, 0xcb, 0x96, 0x14, 0x15, 0xf4, 0xf2, 0xcb, 0xb3, 0x2f, 0x3e,
                             0x60, 0x05, 0x01, 0x0b, 0x08, 0x01, 0x00};

sms_SLQSCDMADecodeMTTextMsg_t tpack_sms_SLQSCDMADecodeMTTextMsg={
    (uint32_t)sizeof(tpack_sms_SLQSCDMADecodeMTTextMsgcdmaMessage),
    (uint8_t*)tpack_sms_SLQSCDMADecodeMTTextMsgcdmaMessage,
    0,
    16,
    {0},
    162,
    {0},
    0,
    0,
    0,
    {{0}},
    {{0}},
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

pack_sms_SetNewSMSCallback_t tpack_sms_SetNewSMSCallback[3]={
    {
        LITEQMI_QMI_CBK_PARAM_RESET
    },
    {
        LITEQMI_QMI_CBK_PARAM_SET
    },
    {
        LITEQMI_QMI_CBK_PARAM_NOCHANGE
    }
    
};

unpack_sms_SetNewSMSCallback_t tunpack_sms_SetNewSMSCallback={
    SWI_UINT256_INT_VALUE
};

uint8_t smscAddress[maxSMSCAddrLen + 2];
uint8_t smscType[maxSMSCTypeLen + 1];
unpack_sms_GetSMSCAddress_t tunpack_sms_GetSMSCAddress = { maxSMSCAddrLen, smscAddress, maxSMSCTypeLen, smscType, SWI_UINT256_INT_VALUE};

uint8_t set_SMSCAddress[] = "+919020000500";
uint8_t set_SMSCType[] = "145";
pack_sms_SetSMSCAddress_t tpack_sms_SetSMSCAddress = { set_SMSCAddress, set_SMSCType};
unpack_sms_SetSMSCAddress_t tunpack_sms_SetSMSCAddress = { SWI_UINT256_INT_VALUE};

uint8_t msgBuffer[] = { 0x00, 0x01, 0x00, 0x0C, 0x91, 0x19, 0x79, 0x97, 0x11,
                  0x21, 0x82, 0x00, 0x00, 0x05, 0xE8, 0x32, 0x9B, 0xFD,
                  0x06 };
uint32_t messageIndex;
pack_sms_SaveSMS_t tpack_sms_SaveSMS = { 0x00, 0x06, sizeof(msgBuffer), msgBuffer};
unpack_sms_SaveSMS_t tunpack_sms_SaveSMS = {&messageIndex,SWI_UINT256_INT_VALUE};

sms_qaQmi3GPPBroadcastCfgInfo  q3GPPBroadcastCfgInfo;
sms_qaQmi3GPP2BroadcastCfgInfo q3GPP2BroadcastCfgInfo;
pack_sms_SLQSGetSmsBroadcastConfig_t tpack_sms_SLQSGetSmsBroadcastConfig = {0x01};//GW mode
unpack_sms_SLQSGetSmsBroadcastConfig_t tunpack_sms_SLQSGetSmsBroadcastConfig = {&q3GPPBroadcastCfgInfo, &q3GPP2BroadcastCfgInfo,SWI_UINT256_INT_VALUE};

sms_qaQmi3GPPBroadcastCfgInfo  lBroadcastConfig = { 0x00, 0x02, {{0x0102, 0x0304, 0x01}} };
sms_qaQmi3GPP2BroadcastCfgInfo lCDMABroadcastConfig = { 0x00, 0x02, {{ 0x0102, 0x0304, 0x01}} };
pack_sms_SLQSSetSmsBroadcastConfig_t tpack_sms_SLQSSetSmsBroadcastConfig = { 0x01, &lBroadcastConfig, &lCDMABroadcastConfig};
unpack_sms_SLQSSetSmsBroadcastConfig_t tunpack_sms_SLQSSetSmsBroadcastConfig = {SWI_UINT256_INT_VALUE};

pack_sms_SLQSSetSmsBroadcastActivation_t tpack_sms_SLQSSetSmsBroadcastActivation =
     { 0x01, 0x01};/* GW Mode Activate */
unpack_sms_SLQSSetSmsBroadcastActivation_t tunpack_sms_SLQSSetSmsBroadcastActivation = {
    SWI_UINT256_INT_VALUE
};

uint8_t regInd = 0xFF;
sms_transLayerInfo transLayerInfo = { 0xFF, 0xFF};
sms_getTransLayerInfo transLayerInfoResp = { &regInd, &transLayerInfo};
unpack_sms_SLQSGetTransLayerInfo_t tunpack_sms_SLQSGetTransLayerInfo = { &transLayerInfoResp,SWI_UINT256_INT_VALUE};

uint8_t regStatus = 0xFF;
sms_getTransNWRegInfo transNWRegInfoResp = { &regStatus};
unpack_sms_SLQSGetTransNWRegInfo_t tunpack_sms_SLQSGetTransNWRegInfo = { &transNWRegInfoResp,SWI_UINT256_INT_VALUE};

uint8_t regTransLayerInfoEvt = 0xFF;
uint8_t regTransNWRegInfoEvt = 0xFF;
uint8_t regCallStatInfoEvt = 0xFF;
sms_getIndicationReg indicationRegInfo = { &regTransLayerInfoEvt, &regTransNWRegInfoEvt, &regCallStatInfoEvt};
unpack_sms_SLQSGetIndicationRegister_t tunpack_sms_SLQSGetIndicationRegister = { &indicationRegInfo,SWI_UINT256_INT_VALUE};

uint8_t transLayerInfoEvtStat = 1;
uint8_t transNWRegInfoEvtStat = 1;
uint8_t callInfoEvtStat = 1;
sms_setIndicationReg setIndicationRegReq = { &transLayerInfoEvtStat, &transNWRegInfoEvtStat, &callInfoEvtStat};
pack_sms_SLQSSetIndicationRegister_t tpack_sms_SLQSSetIndicationRegister = { &setIndicationRegReq};
unpack_sms_SLQSSetIndicationRegister_t tunpack_sms_SLQSSetIndicationRegister = { SWI_UINT256_INT_VALUE};

uint8_t transferStatusReport = 0x01;
sms_setRoutesReq setRoutesReq = {
                4,
                {
                    {0x00, 0x00, 0xFF, 0x03},
                    {0x00, 0x01, 0xFF, 0x03},
                    {0x00, 0x02, 0xFF, 0x03},
                    {0x00, 0x03, 0xFF, 0x03}
                } ,
                &transferStatusReport};
pack_sms_SLQSSmsSetRoutes_t tpack_sms_SLQSSmsSetRoutes = {&setRoutesReq};
unpack_sms_SLQSSmsSetRoutes_t tunpack_sms_SLQSSmsSetRoutes = {SWI_UINT256_INT_VALUE};
sms_msgProtocolResp msgProtResp = {0xFF};
unpack_sms_SLQSSmsGetMessageProtocol_t tunpack_sms_SLQSSmsGetMessageProtocol = {&msgProtResp,SWI_UINT256_INT_VALUE};

sms_maxStorageSizeResp maxStorageSizeresp = {0xFFFFFFFF, 0xFFFFFFFF};
uint8_t msgModeGW      = 0x01;
sms_maxStorageSizeReq maxStorageSizereq = {
        0x00,/* UIM STORAGE TYPE */
        &msgModeGW/* GW MESSAGE MODE */
};
pack_sms_SLQSSmsGetMaxStorageSize_t tpack_sms_SLQSSmsGetMaxStorageSize = {&maxStorageSizereq};
unpack_sms_SLQSSmsGetMaxStorageSize_t tunpack_sms_SLQSSmsGetMaxStorageSize = {&maxStorageSizeresp,SWI_UINT256_INT_VALUE};

sms_getMsgWaitingInfo getMsgWaitInfo;
unpack_sms_SLQSGetMessageWaiting_t tunpack_sms_SLQSGetMessageWaiting = { &getMsgWaitInfo,SWI_UINT256_INT_VALUE};

uint8_t smsBuffer[] = { 0x00, 0x01, 0x00, 0x0C, 0x91, 0x19, 0x79, 0x97, 0x11,
                     0x21, 0x82, 0x00, 0x00, 0x05, 0xE8, 0x32, 0x9B, 0xFD,
                     0x06 };
uint8_t   linktimer = 3;
sms_sendAsyncsmsParams sendAsyncsmsparams = { 0x06, sizeof(smsBuffer), smsBuffer, NULL, NULL, NULL, &linktimer, NULL, NULL, NULL, NULL};
pack_sms_SLQSSendAsyncSMS_t tpack_sms_SLQSSendAsyncSMS = { &sendAsyncsmsparams};
unpack_sms_SLQSSendAsyncSMS_t tunpack_sms_SLQSSendAsyncSMS = { SWI_UINT256_INT_VALUE };

pack_sms_SLQSSetSmsStorage_t tpack_sms_SLQSSetSmsStorage = { 0x01};/* Device's Permanent Memory */
unpack_sms_SLQSSetSmsStorage_t tunpack_sms_SLQSSetSmsStorage = { SWI_UINT256_INT_VALUE };

uint32_t smsStorage;
unpack_sms_SLQSSwiGetSMSStorage_t tunpack_sms_SLQSGetSmsStorage ={ &smsStorage,SWI_UINT256_INT_VALUE};

testitem_t smstotest[] = {
#if 0
    //// Get Modem Settings
    {
        (pack_func_item) &pack_sms_GetSMSCAddress, "pack_sms_GetSMSCAddress",
        NULL,
        (unpack_func_item) &unpack_sms_GetSMSCAddress, "unpack_sms_GetSMSCAddress",
        &tunpack_sms_GetSMSCAddress, dump_GetSMSCAddressSettings
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSmsBroadcastConfig, "pack_sms_SLQSGetSmsBroadcastConfig",
        &tpack_sms_SLQSGetSmsBroadcastConfig,
        (unpack_func_item) &unpack_sms_SLQSGetSmsBroadcastConfig, "unpack_sms_SLQSGetSmsBroadcastConfig",
        &tunpack_sms_SLQSGetSmsBroadcastConfig, dump_SLQSGetSmsBroadcastConfigSettings
    },
    {
        (pack_func_item) &pack_sms_SLQSGetIndicationRegister, "pack_sms_SLQSGetIndicationRegister",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetIndicationRegister, "unpack_sms_SLQSGetIndicationRegister",
        &tunpack_sms_SLQSGetIndicationRegister, dump_SLQSGetIndicationRegisterSettings
    },
        {
        (pack_func_item) &pack_sms_SLQSSwiGetSMSStorage, "pack_sms_SLQSSwiGetSMSStorage",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSwiGetSMSStorage, "unpack_sms_SLQSSwiGetSMSStorage",
        &tunpack_sms_SLQSGetSmsStorage, dump_SLQSSwiGetSMSStorageSettings
    },
    /////////////////
    {
        (pack_func_item) &pack_sms_SLQSDeleteSMS, "pack_sms_SLQSDeleteSMS",
        &tpack_sms_SLQSDeleteSMS, 
        (unpack_func_item) &unpack_sms_SLQSDeleteSMS, "unpack_sms_SLQSDeleteSMS",
        &tunpack_sms_SLQSDeleteSMS, dump_SLQSDeleteSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSMS, "pack_sms_SLQSGetSMS",
        &tpack_sms_SLQSGetSMS, 
        (unpack_func_item) &unpack_sms_SLQSGetSMS, "unpack_sms_SLQSGetSMS",
        &tunpack_sms_SLQSGetSMS, dump_SLQSGetSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSMSList, "pack_sms_SLQSGetSMSList",
        &tpack_sms_SLQSGetSMSList, 
        (unpack_func_item) &unpack_sms_SLQSGetSMSList, "unpack_sms_SLQSGetSMSList",
        &tunpack_sms_SLQSGetSMSList, dump_SLQSGetSMSList
    },
    {
        (pack_func_item) &pack_sms_SLQSModifySMSStatus, "pack_sms_SLQSModifySMSStatus",
        &tpack_sms_SLQSModifySMSStatus, 
        (unpack_func_item) &unpack_sms_SLQSModifySMSStatus, "unpack_sms_SLQSModifySMSStatus",
        &tunpack_sms_SLQSModifySMSStatus, dump_SLQSModifySMSStatus
    },
    {
        (pack_func_item) &pack_sms_SLQSDeleteSMS, "pack_sms_SLQSDeleteSMS",
        &tpack_sms_SLQSDeleteSMS, 
        (unpack_func_item) &unpack_sms_SLQSDeleteSMS, "unpack_sms_SLQSDeleteSMS",
        &tunpack_sms_SLQSDeleteSMS, dump_SLQSDeleteSMS
    },
#endif
    {
        (pack_func_item) &pack_sms_SendSMS, "pack_sms_SendSMS",
        &tpack_sms_SendSMS, 
        (unpack_func_item) &unpack_sms_SendSMS, "unpack_sms_SendSMS",
        &tunpack_sms_SendSMS, dump_SLQSSendSMS
    },
#if 0
    {
        //FIXME pack function expect pass by value instead of reference
        (pack_func_item) &pack_sms_SetNewSMSCallback, "pack_sms_SetNewSMSCallback",
        &tpack_sms_SetNewSMSCallback[LITEQMI_QMI_CBK_PARAM_SET], 
        (unpack_func_item) &unpack_sms_SetNewSMSCallback, "unpack_sms_SetNewSMSCallback",
        &tunpack_sms_SetNewSMSCallback, dump_SetNewSMSCallback
    },
    {
        (pack_func_item) &pack_sms_SetSMSCAddress, "pack_sms_SetSMSCAddress",
        &tpack_sms_SetSMSCAddress,
        (unpack_func_item) &unpack_sms_SetSMSCAddress, "unpack_sms_SetSMSCAddress",
        &tunpack_sms_SetSMSCAddress, dump_SetSMSCAddress
    },
    {
        (pack_func_item) &pack_sms_GetSMSCAddress, "pack_sms_GetSMSCAddress",
        NULL,
        (unpack_func_item) &unpack_sms_GetSMSCAddress, "unpack_sms_GetSMSCAddress",
        &tunpack_sms_GetSMSCAddress, dump_GetSMSCAddress
    },
    {
        (pack_func_item) &pack_sms_SaveSMS, "pack_sms_SaveSMS",
        &tpack_sms_SaveSMS,
        (unpack_func_item) &unpack_sms_SaveSMS, "unpack_sms_SaveSMS",
        &tunpack_sms_SaveSMS, dump_SaveSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSSetSmsBroadcastConfig, "pack_sms_SLQSSetSmsBroadcastConfig",
        &tpack_sms_SLQSSetSmsBroadcastConfig,
        (unpack_func_item) &unpack_sms_SLQSSetSmsBroadcastConfig, "unpack_sms_SLQSSetSmsBroadcastConfig",
        &tunpack_sms_SLQSSetSmsBroadcastConfig, dump_SLQSSetSmsBroadcastConfig
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSmsBroadcastConfig, "pack_sms_SLQSGetSmsBroadcastConfig",
        &tpack_sms_SLQSGetSmsBroadcastConfig,
        (unpack_func_item) &unpack_sms_SLQSGetSmsBroadcastConfig, "unpack_sms_SLQSGetSmsBroadcastConfig",
        &tunpack_sms_SLQSGetSmsBroadcastConfig, dump_SLQSGetSmsBroadcastConfig
    },
    {
        (pack_func_item) &pack_sms_SLQSSetSmsBroadcastActivation, "pack_sms_SLQSSetSmsBroadcastActivation",
        &tpack_sms_SLQSSetSmsBroadcastActivation,
        (unpack_func_item) &unpack_sms_SLQSSetSmsBroadcastActivation, "unpack_sms_SLQSSetSmsBroadcastActivation",
        &tunpack_sms_SLQSSetSmsBroadcastActivation, dump_SLQSSetSmsBroadcastActivation
    },
    {
        (pack_func_item) &pack_sms_SLQSGetTransLayerInfo, "pack_sms_SLQSGetTransLayerInfo",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetTransLayerInfo, "unpack_sms_SLQSGetTransLayerInfo",
        &tunpack_sms_SLQSGetTransLayerInfo, dump_SLQSGetTransLayerInfo
    },
    {
        (pack_func_item) &pack_sms_SLQSGetTransNWRegInfo, "pack_sms_SLQSGetTransNWRegInfo",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetTransNWRegInfo, "unpack_sms_SLQSGetTransNWRegInfo",
        &tunpack_sms_SLQSGetTransNWRegInfo, dump_SLQSGetTransNWRegInfo
    },
    {
        (pack_func_item) &pack_sms_SLQSSetIndicationRegister, "pack_sms_SLQSSetIndicationRegister",
        &tpack_sms_SLQSSetIndicationRegister,
        (unpack_func_item) &unpack_sms_SLQSSetIndicationRegister, "unpack_sms_SLQSSetIndicationRegister",
        &tunpack_sms_SLQSSetIndicationRegister, dump_SLQSSetIndicationRegister
    },
    {
        (pack_func_item) &pack_sms_SLQSGetIndicationRegister, "pack_sms_SLQSGetIndicationRegister",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetIndicationRegister, "unpack_sms_SLQSGetIndicationRegister",
        &tunpack_sms_SLQSGetIndicationRegister, dump_SLQSGetIndicationRegister
    },
    {
        (pack_func_item) &pack_sms_SLQSSmsSetRoutes, "pack_sms_SLQSSmsSetRoutes",
        &tpack_sms_SLQSSmsSetRoutes,
        (unpack_func_item) &unpack_sms_SLQSSmsSetRoutes, "unpack_sms_SLQSSmsSetRoutes",
        &tunpack_sms_SLQSSmsSetRoutes, dump_SLQSSmsSetRoutes
    },
    {
        (pack_func_item) &pack_sms_SLQSSmsGetMessageProtocol, "pack_sms_SLQSSmsGetMessageProtocol",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSmsGetMessageProtocol, "unpack_sms_SLQSSmsGetMessageProtocol",
        &tunpack_sms_SLQSSmsGetMessageProtocol, dump_SLQSSmsGetMessageProtocol
    },
    {
        (pack_func_item) &pack_sms_SLQSSmsGetMaxStorageSize, "pack_sms_SLQSSmsGetMaxStorageSize",
        &tpack_sms_SLQSSmsGetMaxStorageSize,
        (unpack_func_item) &unpack_sms_SLQSSmsGetMaxStorageSize, "unpack_sms_SLQSSmsGetMaxStorageSize",
        &tunpack_sms_SLQSSmsGetMaxStorageSize, dump_SLQSSmsGetMaxStorageSize
    },
    {
        (pack_func_item) &pack_sms_SLQSGetMessageWaiting, "pack_sms_SLQSGetMessageWaiting",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetMessageWaiting, "unpack_sms_SLQSGetMessageWaiting",
        &tunpack_sms_SLQSGetMessageWaiting, dump_SLQSGetMessageWaiting
    },
    {
        (pack_func_item) &pack_sms_SLQSSendAsyncSMS, "pack_sms_SLQSSendAsyncSMS",
        &tpack_sms_SLQSSendAsyncSMS,
        (unpack_func_item) &unpack_sms_SLQSSendAsyncSMS, "unpack_sms_SLQSSendAsyncSMS",
        &tunpack_sms_SLQSSendAsyncSMS, dump_SLQSSendAsyncSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSSetSmsStorage, "pack_sms_SLQSSetSmsStorage",
        &tpack_sms_SLQSSetSmsStorage,
        (unpack_func_item) &unpack_sms_SLQSSetSmsStorage, "unpack_sms_SLQSSetSmsStorage",
        &tunpack_sms_SLQSSetSmsStorage, dump_SLQSSetSmsStorageDevice
    },
    {
        (pack_func_item) &pack_sms_SLQSSwiGetSMSStorage, "pack_sms_SLQSSwiGetSMSStorage",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSwiGetSMSStorage, "unpack_sms_SLQSSwiGetSMSStorage",
        &tunpack_sms_SLQSGetSmsStorage, dump_SLQSSwiGetSMSStorage
    },
    ////Restore Modem Settings.
    {
        (pack_func_item) &pack_sms_SetSMSCAddress, "pack_sms_SetSMSCAddress",
        &DefaultSMSSCAddressSettings,
        (unpack_func_item) &unpack_sms_SetSMSCAddress, "unpack_sms_SetSMSCAddress",
        &tunpack_sms_SetSMSCAddress, dump_SetSMSCAddress
    },
    {
        (pack_func_item) &pack_sms_SLQSSetSmsBroadcastConfig, "pack_sms_SLQSSetSmsBroadcastConfig",
        &DefaultSmsBroadcastConfigSettings,
        (unpack_func_item) &unpack_sms_SLQSSetSmsBroadcastConfig, "unpack_sms_SLQSSetSmsBroadcastConfig",
        &tunpack_sms_SLQSSetSmsBroadcastConfig, dump_SLQSSetSmsBroadcastConfig
    },
    {
        (pack_func_item) &pack_sms_SLQSSetIndicationRegister, "pack_sms_SLQSSetIndicationRegister",
        &DefaultIndicationRegisterSettings,
        (unpack_func_item) &unpack_sms_SLQSSetIndicationRegister, "unpack_sms_SLQSSetIndicationRegister",
        NULL, dump_SLQSSetIndicationRegister
    },
    {
        (pack_func_item) &pack_sms_SLQSSetSmsStorage, "pack_sms_SLQSSetSmsStorage",
        &DefaultSmsStorageSettings,
        (unpack_func_item) &unpack_sms_SLQSSetSmsStorage, "unpack_sms_SLQSSetSmsStorage",
        &tunpack_sms_SLQSSetSmsStorage, dump_SLQSSetSmsStorageDevice
    },
#endif
    ///////////////////////////////
};

unsigned int smsarraylen = (unsigned int)((sizeof(smstotest))/(sizeof(smstotest[0])));

testitem_t smstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_sms_GetSMSCAddress, "pack_sms_GetSMSCAddress",
        NULL,
        (unpack_func_item) &unpack_sms_GetSMSCAddress, "unpack_sms_GetSMSCAddress",
        NULL, dump_GetSMSCAddressSettings
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSmsBroadcastConfig, "pack_sms_SLQSGetSmsBroadcastConfig",
        &tpack_sms_SLQSGetSmsBroadcastConfig,
        (unpack_func_item) &unpack_sms_SLQSGetSmsBroadcastConfig, "unpack_sms_SLQSGetSmsBroadcastConfig",
        NULL, dump_SLQSGetSmsBroadcastConfigSettings
    },
    {
        (pack_func_item) &pack_sms_SLQSGetIndicationRegister, "pack_sms_SLQSGetIndicationRegister",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetIndicationRegister, "unpack_sms_SLQSGetIndicationRegister",
        NULL, dump_SLQSGetIndicationRegisterSettings
    },
        {
        (pack_func_item) &pack_sms_SLQSSwiGetSMSStorage, "pack_sms_SLQSSwiGetSMSStorage",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSwiGetSMSStorage, "unpack_sms_SLQSSwiGetSMSStorage",
        NULL, dump_SLQSSwiGetSMSStorageSettings
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSMS, "pack_sms_SLQSGetSMS",
        &tpack_sms_SLQSGetSMS, 
        (unpack_func_item) &unpack_sms_SLQSGetSMS, "unpack_sms_SLQSGetSMS",
        NULL, dump_SLQSGetSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSMSList, "pack_sms_SLQSGetSMSList",
        &tpack_sms_SLQSGetSMSList, 
        (unpack_func_item) &unpack_sms_SLQSGetSMSList, "unpack_sms_SLQSGetSMSList",
        NULL, dump_SLQSGetSMSList
    },
    {
        (pack_func_item) &pack_sms_SendSMS, "pack_sms_SendSMS",
        &tpack_sms_SendSMS, 
        (unpack_func_item) &unpack_sms_SendSMS, "unpack_sms_SendSMS",
        NULL, dump_SLQSSendSMS
    },

    {
        (pack_func_item) &pack_sms_GetSMSCAddress, "pack_sms_GetSMSCAddress",
        NULL,
        (unpack_func_item) &unpack_sms_GetSMSCAddress, "unpack_sms_GetSMSCAddress",
        NULL, dump_GetSMSCAddress
    },
    {
        (pack_func_item) &pack_sms_SaveSMS, "pack_sms_SaveSMS",
        &tpack_sms_SaveSMS,
        (unpack_func_item) &unpack_sms_SaveSMS, "unpack_sms_SaveSMS",
        NULL, dump_SaveSMS
    },
    {
        (pack_func_item) &pack_sms_SLQSGetSmsBroadcastConfig, "pack_sms_SLQSGetSmsBroadcastConfig",
        &tpack_sms_SLQSGetSmsBroadcastConfig,
        (unpack_func_item) &unpack_sms_SLQSGetSmsBroadcastConfig, "unpack_sms_SLQSGetSmsBroadcastConfig",
        NULL, dump_SLQSGetSmsBroadcastConfig
    },
    {

        (pack_func_item) &pack_sms_SLQSGetTransLayerInfo, "pack_sms_SLQSGetTransLayerInfo",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetTransLayerInfo, "unpack_sms_SLQSGetTransLayerInfo",
        NULL, dump_SLQSGetTransLayerInfo
    },
    {
        (pack_func_item) &pack_sms_SLQSGetTransNWRegInfo, "pack_sms_SLQSGetTransNWRegInfo",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetTransNWRegInfo, "unpack_sms_SLQSGetTransNWRegInfo",
        NULL, dump_SLQSGetTransNWRegInfo
    },
    {
        (pack_func_item) &pack_sms_SLQSGetIndicationRegister, "pack_sms_SLQSGetIndicationRegister",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetIndicationRegister, "unpack_sms_SLQSGetIndicationRegister",
        NULL, dump_SLQSGetIndicationRegister
    },
    {
        (pack_func_item) &pack_sms_SLQSSmsGetMessageProtocol, "pack_sms_SLQSSmsGetMessageProtocol",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSmsGetMessageProtocol, "unpack_sms_SLQSSmsGetMessageProtocol",
        NULL, dump_SLQSSmsGetMessageProtocol
    },
    {
        (pack_func_item) &pack_sms_SLQSSmsGetMaxStorageSize, "pack_sms_SLQSSmsGetMaxStorageSize",
        &tpack_sms_SLQSSmsGetMaxStorageSize,
        (unpack_func_item) &unpack_sms_SLQSSmsGetMaxStorageSize, "unpack_sms_SLQSSmsGetMaxStorageSize",
        NULL, dump_SLQSSmsGetMaxStorageSize
    },
    {
        (pack_func_item) &pack_sms_SLQSGetMessageWaiting, "pack_sms_SLQSGetMessageWaiting",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSGetMessageWaiting, "unpack_sms_SLQSGetMessageWaiting",
        NULL, dump_SLQSGetMessageWaiting
    },
    {
        (pack_func_item) &pack_sms_SLQSSwiGetSMSStorage, "pack_sms_SLQSSwiGetSMSStorage",
        NULL,
        (unpack_func_item) &unpack_sms_SLQSSwiGetSMSStorage, "unpack_sms_SLQSSwiGetSMSStorage",
        NULL, dump_SLQSSwiGetSMSStorage
    },
};

void sms_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======SMS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    const char *qmi_msg;
    for(i=0; i<sizeof(smstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(smstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,smstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(sms<0)
            sms = client_fd(eSMS);
        if(sms<0)
        {
            fprintf(stderr,"SMS Service Not Supported!\n");
            return ;
        }
        rtn = write(sms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(sms>=0)
                close(sms);
            sms=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }
        fflush(stdout);

        rtn = read(sms, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eSMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);
            fflush(stdout);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(smstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                smstotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           smstotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    smstotest_invalidunpack[i].dump(smstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(sms>=0)
                close(sms);
            sms=-1;
        }
        sleep(1);
        xid++;
    }
    if(sms>=0)
        close(sms);
    sms=-1;
}

void sms_test_pack_unpack_loop()
{
    unsigned i;
    unsigned xid =1;
    printf("======SMS dummy unpack test===========\n");
    iLocalLog = 0;
    sms_validate_dummy_unpack();
    iLocalLog = 1;
    printf("======SMS dummy unpack test End ===========\n");
    printf("========START sms_dummy_unpack========\n");
    sms_dummy_unpack();
    printf("========END sms_dummy_unpack========\n");
    for(i=0; i<sizeof(smstotest)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        pack_qmi_t   req_ctx;
        int rtn;
        uint8_t rsp[QMI_MSG_MAX]={0};
        uint8_t req[QMI_MSG_MAX]={0};
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        printf("pack %d\n", i);
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(smstotest[i].pack)(&req_ctx, req, &reqLen,smstotest[i].pack_ptr);
        printf("# pack function returns %d\n", rtn);
        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        if(sms<0)
            sms = client_fd(eSMS);
        if(sms<0)
        {
            fprintf(stderr,"SMS Service Not Supported!\n");
            return ;
        }
        rtn = write(sms, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        if(smstotest[i].unpack==NULL)
        {
            if( (smstotest[i].dump !=NULL) && 
                (smstotest[i].unpack_ptr!=NULL))
            {
                smstotest[i].dump(smstotest[i].unpack_ptr);
            }
        }
        else
        {
            rtn = read(sms, rsp, QMI_MSG_MAX);
            if( (rtn<QMI_MSG_MAX))
            {
                rspLen = (uint16_t)rtn;
                qmi_msg = helper_get_resp_ctx(eSMS, rsp, rspLen, &rsp_ctx);
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                printf("unpack %d\n", i);
                if (rsp_ctx.xid == xid)
                {
                    if(smstotest[i].unpack != NULL)
                    {
                        unpackRetCode = run_unpack_item(smstotest[i].unpack)(rsp, rspLen, smstotest[i].unpack_ptr);
                        printf("# unpack function returns %d\n", unpackRetCode);
                    }
                    if(smstotest[i].dump !=NULL)
                    {
                        smstotest[i].dump(smstotest[i].unpack_ptr);
                    }
                }
            }
            else
            {
                printf("Error read %d > %d \n", rtn,QMI_MSG_MAX);
            }
        }
        
        sleep(1);
        xid++;
    }
    if(sms>=0)
       close(sms);
    sms = -1;;
}

void send_sms_read_req (uint32_t storageType, uint32_t msgIndex, uint8_t msgMode)
{
    pack_qmi_t req_ctx;
    int rtn;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len = QMI_MSG_MAX;
    const char *qmi_msg;

    memset(&sms_attr, 0, sizeof(sms_attr));
    memset(qmi_req, 0, QMI_MSG_MAX);
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = 10;
    
    pack_sms_SLQSGetSMS_t getsms_reqParam;
    getsms_reqParam.storageType  = storageType; 
    getsms_reqParam.messageIndex = msgIndex;
    uint8_t messageMode = msgMode; 
    getsms_reqParam.pMessageMode  = &messageMode;    
    rtn = pack_sms_SLQSGetSMS(&req_ctx, qmi_req, &qmi_req_len, &getsms_reqParam);
    printf("%s: pack rtn %d\n", __func__, rtn);    
    rtn = write(sms, qmi_req, qmi_req_len);
    printf("write %d wrote %d\n", qmi_req_len, rtn);
    if(rtn == qmi_req_len)
    {
        qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

}

int smsdemo_processucs2msg(char *pUcsMsg, uint16_t UcsMsgLen, char *copybuf)
{
    char *strbuf;
    uint8_t res;
    uint16_t UTFBufLen = 0;

    if (!pUcsMsg)
        return eQCWWAN_ERR_INVALID_ARG;

    if (UcsMsgLen == 0)
        return eQCWWAN_ERR_GENERAL;

    UTFBufLen = (UcsMsgLen)*2;
    if (UTFBufLen <= 0)
    {
        printf("invalid message size!\n");
        return eQCWWAN_ERR_BUFFER_SZ;
    }
    strbuf = malloc( UTFBufLen * sizeof(char) );
    if (strbuf == NULL) 
    {
        printf("memory allocation failure in smsdemo  for UCS2 to UTF conversion for wcdma sms\n");
        return eQCWWAN_ERR_MEMORY;
    }

    memset(strbuf, 0, UTFBufLen);
    res = smsdemo_conversionUCS2msgToUTF(pUcsMsg,
                           UcsMsgLen,
                           strbuf, UTFBufLen);
    if (!res)
    {
        free(strbuf);
        printf("UCS2 to UTF conversion failed for wcdma sms\n");
        return eQCWWAN_ERR_GENERAL;
    }

    /* copybuf is NULL for short message */
    if (copybuf)
        strcpy(copybuf, strbuf);
    else
        fprintf( stderr, "\nMessage: %s\n\n",strbuf);
    free(strbuf);
    return eQCWWAN_ERR_NONE;
}

void wcdma_decode_text_msg (unpack_sms_SLQSGetSMS_t getrawsms)
{
    sms_SLQSWCDMADecodeMTTextMsg_t wcdmadctxmsg_reqParam;
    uint8_t encoding = 0xFF;
    uint16_t userdataLen = 0;
    memset(&wcdmadctxmsg_reqParam,0,sizeof(wcdmadctxmsg_reqParam));
    
    wcdmadctxmsg_reqParam.pMessage = getrawsms.message;
    wcdmadctxmsg_reqParam.MessageLen = getrawsms.messageSize;
    wcdmadctxmsg_reqParam.scAddrLength = 16;
    wcdmadctxmsg_reqParam.senderAddrLength = 16;
    wcdmadctxmsg_reqParam.textMsgLength = MAX_SMS_MSG_LEN;

    int rtn = sms_SLQSWCDMADecodeMTTextMsg(&wcdmadctxmsg_reqParam);
    if ( !rtn )
    {
        printf("WCDMA Decoding Successful\n");
        printf("\nService Center Number = %s",wcdmadctxmsg_reqParam.scAddr);
        printf("\nSender Number = %s",wcdmadctxmsg_reqParam.senderAddr);
        printf("\nDate = %s",wcdmadctxmsg_reqParam.date.data);
        printf("\nTime = %s\n",wcdmadctxmsg_reqParam.time.data);

        rtn = sms_SLQSWCDMADecodeMTEncoding(wcdmadctxmsg_reqParam.pMessage,
                                    wcdmadctxmsg_reqParam.MessageLen,
                                    &encoding,
                                    &userdataLen);

        if ((!rtn) && (encoding == UCSENCODING))
        {
            
            smsdemo_processucs2msg((char*)wcdmadctxmsg_reqParam.textMsg,
                                         userdataLen,
                                         NULL);

        }
        else
        {
            printf("\nMessage: %s\n\n",wcdmadctxmsg_reqParam.textMsg );
        }
    }
}

static uint8_t octet_2_bin(char* octet)
{
    uint8_t result = 0;
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

void display_long_sms(sms_SLQSWCDMADecodeLongTextMsg_t *pWcdmaLongMsgDecodingParams)
{
    uint8_t i;
    uint8_t  AddressType = 0;
    if(pWcdmaLongMsgDecodingParams==NULL)
        return ;
    AddressType = octet_2_bin(pWcdmaLongMsgDecodingParams->pScAddr);
    /* It is an international number add '+' in pScAddr */
    if (AddressType == 0x91)
        fprintf( stderr, "\nService Center Number = +%s\n",
                 pWcdmaLongMsgDecodingParams->pScAddr+2);
    else
        fprintf( stderr, "\nService Center Number = %s\n",
                 pWcdmaLongMsgDecodingParams->pScAddr+2 );

    AddressType = octet_2_bin(pWcdmaLongMsgDecodingParams->pSenderAddr);

    /* It is an international number add '+' in pSendAddr */
    if (AddressType == 0x91)
        fprintf( stderr, "\nSender's Number = +%s\n",
                 pWcdmaLongMsgDecodingParams->pSenderAddr+2);
    else
        fprintf( stderr, "\nSender's Number = %s\n",
                 pWcdmaLongMsgDecodingParams->pSenderAddr+2 );

    fprintf( stderr, "Date = %s\n",
             pWcdmaLongMsgDecodingParams->Date.data );
    fprintf( stderr, "Time = %s\n",
             pWcdmaLongMsgDecodingParams->Time.data );

    fprintf( stderr,"Message:" );
    for ( i = 0; i < *pWcdmaLongMsgDecodingParams->pTotalNum; i++)
    {
        if (copy_buf[i])
        {
            fprintf( stderr,"%s",copy_buf[i]);
        }
    }
    fprintf( stderr,"\n" );
    concatenated_sms_counter = 0;
    buf_allocated = FALSE;
    for ( i = 0; i < *pWcdmaLongMsgDecodingParams->pTotalNum; i++)
    {
        if (copy_buf[i] != NULL) {
            free(copy_buf[i]);
            copy_buf[i] = NULL;
        }
    }
}

void display_cdma_long_sms(struct sms_cdmaMsgDecodingParamsExt_t *pcdmaMsgDecodingParamsExt)
{
    uint8_t i;
    if(pcdmaMsgDecodingParamsExt==NULL)
        return ;
    printf( "Total Numbers of Split Messages: %d\n",*pcdmaMsgDecodingParamsExt->pTotalNum);
    
    printf( "Sender Number: %s\n",pcdmaMsgDecodingParamsExt->pSenderAddr);
    printf( "Time Stamp: %d:%d:%d:%d:%d:%d\n",
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_YEAR],
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_MONTH],
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_DAY],
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_HOUR],
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_MINUTE],
            pcdmaMsgDecodingParamsExt->mcTimeStamp.data[eTIMESTEMP_SECOND]);


    printf( "Concatenated Long Message:");
    for ( i = 0; i < *pcdmaMsgDecodingParamsExt->pTotalNum; i++)
    {
        if (copy_buf[i])
        {
            printf( "%s",copy_buf[i]);
        }
    }
    printf( "\n");

    concatenated_sms_counter = 0;
    concate_prev_total_number = 0;
    concate_prev_ref_number = 0;
    buf_allocated = FALSE;

    for ( i = 0; i < *pcdmaMsgDecodingParamsExt->pTotalNum; i++)
    {
        if (copy_buf[i] != NULL) {
            free(copy_buf[i]);
            copy_buf[i] = NULL;
        }
    }
}

static void swapDigits(char **msg)
{
    uint8_t i, temp;

    for ( i = 0; i < strlen(*msg); i+=2)
    {
        temp = (*msg)[i];
        (*msg)[i] = (*msg)[i+1];
        (*msg)[i+1] = temp;
    }
}

static void convertAscToHex(char *number, char *pdu )
{
    uint8_t i;

    for ( i = 0; i < strlen(number); i+=2 )
    {
        pdu[i/2] = ((number[i]-0x30)<<4) | (number[i+1]-0x30);
    }
}

void sms_setOemApiTestConfig(smsConfig *smsConfigList)
{
    char pdu[10]={0};
    uint8_t len=0;
    char *ptr =NULL;

    if ( smsConfigList != NULL)
    {
        storageType = smsConfigList->storageType;
        tpack_sms_SLQSGetSMSmessageMode = smsConfigList->msgMode;
        messageFormat = smsConfigList->msgFormat;
        strcpy(phoneNum, smsConfigList->number);
        /* construct the pdu by modifying the phone number */
        ptr = &phoneNum[0];
        swapDigits(&ptr);
        convertAscToHex(ptr, pdu);
        len = strlen(pdu);
        msg_pdu_number[0] = 0x00;
        msg_pdu_number[1] = 0x11;
        msg_pdu_number[2] = 0x00;
        msg_pdu_number[3] = strlen(ptr);
        msg_pdu_number[4] = 0x81;
        memcpy(&msg_pdu_number[5], pdu, len);
        msg_pdu_number[5+len] = 0x00;
        msg_pdu_number[6+len] = 0x00;
        msg_pdu_number[7+len] = 0x00;
        msg_pdu_number[8+len] = 0x02;
        msg_pdu_number[9+len] = 0xF4;
        msg_pdu_number[10+len] = 0x18;
        msg_pdu_number_size = 10+len+1;
        
    }
}

static void checkErrorAndFormErrorString(uint8_t *msg)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    if ( unpackRetCode!=eQCWWAN_ERR_NONE )
    {
        eRCode = helper_get_error_code(msg);
        reason = helper_get_error_reason(eRCode);
        sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
    }
}

uint8_t dummy_sms_resp_msg[][QMI_MSG_MAX] = { 
        /*************Response***********/
        /* eQMI_WMS_SET_IND_REGISTER */
        {0x02,0x0a,0x00,0x47,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* WMS_RAW_READ */
        {0x02,0x01,0x00,0x22,0x00,0x2e,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x24
            ,0x00,0x01,0x06,0x20,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x33,0xf4,0x04,0x08,0x84
            ,0x55,0x00,0x53,0x83,0x00,0x00,0x81,0x80,0x61,0x71,0x51,0x81,0x23,0x08,0x6b,0xb3},
        /* eQMI_WMS_GET_MSG_LIST */
        {0x02,0x01,0x00,0x31,0x00,0x1d,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x13
            ,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x03},
        /* eQMI_WMS_MODIFY_TAG */
        {0x02,0x01,0x00,0x23,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_DELETE */
        {0x02,0x01,0x00,0x24,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_SET_EVENT */
        {0x02,0x01,0x00,0x01,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_RAW_SEND */
        {0x02,0x01,0x00,0x20,0x00,0x0c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02
            ,0x00,0x33,0x00},
        /* eQMI_WMS_GET_SMSC_ADDR */
        {0x02,0x01,0x00,0x34,0x00,0x1a,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x10,
            0x00,0x31,0x34,0x35,0x0c,0x2b,0x38,0x35,0x32,0x39,0x30,0x31,0x30,0x30,0x30,0x30,
            0x30},
        /* eQMI_WMS_GET_BC_CONFIG */
        {0x02,0x02,0x00,0x3e,0x00,0x1c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x12,
            0x00,0x01,0x03,0x00,0x00,0x11,0x07,0x11,0x01,0x12,0x11,0x2f,0x11,0x01,0x00,0x00,
            0xff,0xff,0x01},
        /* eQMI_WMS_GET_IND_REGISTER */
        {0x02,0x03,0x00,0x4d,0x00,0x23,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
            0x00,0x00,0x11,0x01,0x00,0x00,0x12,0x01,0x00,0x00,0x13,0x01,0x00,0x00,0x14,0x01,
            0x00,0x00,0x15,0x01,0x00,0x00,0x16,0x01,0x00,0x00},
        /* eQMI_WMS_SWI_GET_SMS_STORAGE */
        {0x02,0x04,0x00,0x56,0x55,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x47,0x00},
        /* eQMI_WMS_RAW_READ */
        {0x02,0x05,0x00,0x22,0x00,0x2b,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x21
            ,0x00,0x03,0x06,0x1d,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x00,0xf0,0x11,0x64,0x0b
            ,0x91,0x58,0x92,0x27,0x36,0x44,0xf1,0x00,0x00,0xa7,0x07,0xe8,0x32,0x9b,0xfd,0x56
            ,0x28,0x00},
        /* eQMI_WMS_GET_MSG_LIST */
        {0x02,0x06,0x00,0x31,0x00,0x0e,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04
            ,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_RAW_SEND */
        {0x02,0x07,0x00,0x20,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x3a,0x00},
        /* eQMI_WMS_GET_SMSC_ADDR */
        {0x02,0x08,0x00,0x34,0x00,0x1a,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x10
            ,0x00,0x31,0x34,0x35,0x0c,0x2b,0x38,0x35,0x32,0x39,0x30,0x31,0x30,0x30,0x30,0x30
            ,0x30},
        /* eQMI_WMS_RAW_WRITE */
        {0x02,0x09,0x00,0x21,0x00,0x0e,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04
            ,0x00,0x02,0x00,0x00,0x00},
        /* eQMI_WMS_GET_BC_CONFIG */
        {0x02,0x0a,0x00,0x3e,0x00,0x1c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x12
            ,0x00,0x01,0x03,0x00,0x00,0x11,0x07,0x11,0x01,0x12,0x11,0x2f,0x11,0x01,0x00,0x00
            ,0xff,0xff,0x01},
        /* eQMI_WMS_GET_TRANS_LAYER_INFO */
        {0x02,0x0b,0x00,0x48,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x34,0x00},
        /* WMS_GET_TRANS_NW_REG_INFO */
        {0x02,0x0c,0x00,0x4a,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x34,0x00},
        /* eQMI_WMS_GET_IND_REGISTER */
        {0x02,0x0d,0x00,0x4d,0x00,0x23,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01
            ,0x00,0x00,0x11,0x01,0x00,0x00,0x12,0x01,0x00,0x00,0x13,0x01,0x00,0x00,0x14,0x01
            ,0x00,0x00,0x15,0x01,0x00,0x00,0x16,0x01,0x00,0x00},
        /* eQMI_WMS_GET_MSG_PROTOCOL */
        {0x02,0x0e,0x00,0x30,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x19,0x00},
        /* eQMI_WMS_GET_MSG_LIST_MAX */
        {0x02,0x0f,0x00,0x36,0x00,0x15,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04
            ,0x00,0x50,0x00,0x00,0x00,0x10,0x04,0x00,0x4d,0x00,0x00,0x00},
        /* WMS_GET_MSG_WAITING */
        {0x02,0x10,0x00,0x43,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x2f,0x00},
        /* eQMI_WMS_SWI_GET_SMS_STORAGE */
        {0x02,0x11,0x00,0x56,0x55,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x47,0x00},
        /* eQMI_WMS_SET_SMSC_ADDR */
        {0x02,0x0c,0x00,0x35,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_SET_BC_CONFIG */
        {0x02,0x0f,0x00,0x3d,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_GET_BC_CONFIG */
        {0x02,0x10,0x00,0x3e,0x00,0x21,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x17
            ,0x00,0x01,0x04,0x00,0x1f,0x11,0x1f,0x11,0x01,0x02,0x01,0x04,0x03,0x01,0x12,0x11
            ,0x12,0x11,0x01,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_SET_BC_ACTIVATION */
        {0x02,0x11,0x00,0x3c,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_SET_ROUTES */
        {0x02,0x16,0x00,0x32,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_ASYNC_RAW_SEND */
        {0x02,0x1a,0x00,0x59,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
        /* eQMI_WMS_SWI_SET_SMS_STORAGE */
        {0x02,0x1b,0x00,0x57,0x55,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x47,0x00},
        /* eQMI_WMS_SET_IND_REGISTER */
        {0x02,0x1f,0x00,0x47,0x00,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x30,0x00},
        /* eQMI_WMS_RAW_READ */
        {0x02,0x01,0x00,0x22,0x00,0x60,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x56
            ,0x00,0x01,0x06,0x52,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xf2,0x24,0x0b,0xd0
            ,0xb1,0x19,0xce,0xda,0x9c,0x02,0x41,0x00,0x81,0x80,0x92,0x31,0x40,0x22,0x23,0x3e
            ,0xd9,0x77,0x1d,0x84,0x0e,0xdb,0xcb,0xa0,0x30,0xc8,0x5d,0xbe,0x83,0xec,0xef,0xf4
            ,0xb8,0xdc,0x0e,0xa7,0xd9,0xa0,0x76,0x79,0x3e,0x0f,0x9f,0xcb,0x3b,0x10,0x9c,0x5d
            ,0x0e,0xcf,0xcb,0xa0,0x71,0x98,0xcd,0x06,0xc5,0x66,0x38,0x10,0xfd,0x0d,0x92,0x97
            ,0xe9,0xf2,0x74,0xd9,0x5e,0x76,0x01},
        /* eQMI_WMS_RAW_SEND */
        {0x02,0x0a,0x00,0x20,0x00,0x0c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02
            ,0x00,0x7e,0x00},        
        /* eQMI_WMS_RAW_READ */
        {0x02,0x0a,0x00,0x22,0x00,0xb3,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xa9
            ,0x00,0x01,0x06,0xa5,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x33,0xf4,0x44,0x08,0x84
            ,0x69,0x18,0x28,0x55,0x00,0x00,0x81,0x80,0x92,0x41,0x50,0x25,0x23,0xa0,0x05,0x00
            ,0x03,0x00,0x02,0x01,0x36,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3
            ,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe2,0x91,0x38,0x35,0x1e
            ,0x8d,0xc4,0xa3,0x91,0xb8,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b
            ,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0x6b,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb
            ,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x22
            ,0x1e,0x89,0x53,0xe3,0xd1,0x48,0x3c,0x1a,0x89,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e
            ,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x36,0x2f,0xe7
            ,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9
            ,0x79,0x39,0x37,0x2f,0xe2,0x91,0x38,0x35,0x1e,0x8d},
        /* eQMI_WMS_RAW_SEND */
        {0x02,0x0a,0x00,0x20,0x00,0x0c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02
            ,0x00,0x7f,0x00},
        /* eQMI_WMS_RAW_READ */
        {0x02,0x0a,0x00,0x22,0x00,0xae,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xa4
            ,0x00,0x01,0x06,0xa0,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x33,0xf4,0x44,0x08,0x84
            ,0x69,0x18,0x28,0x55,0x00,0x00,0x81,0x80,0x92,0x41,0x50,0x45,0x23,0x9a,0x05,0x00
            ,0x03,0x00,0x02,0x02,0x88,0x47,0x23,0x71,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9
            ,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0xd7,0xe6,0xe5,0xdc,0xbc
            ,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f
            ,0xe7,0xe6,0x45,0x3c,0x12,0xa7,0xc6,0xa3,0x91,0x78,0x34,0x12,0x37,0x2f,0xe7,0xe6
            ,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79
            ,0x6d,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c
            ,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xc4,0x23,0x71,0x6a,0x3c,0x1a,0x89,0x47,0x23
            ,0x71,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5
            ,0xdc,0xbc,0x9c,0x9b,0x17},
        /***********Indication********************/
        /* eQMI_WMS_EVENT_IND */
        {0x04,0x01,0x00,0x01,0x00,0x10,0x00,0x16,0x01,0x00,0x00,0x10,0x05,0x00,0x00,0x01
            ,0x00,0x00,0x00,0x12,0x01,0x00,0x01},        
        /* eQMI_WMS_EVENT_IND */
        {0x04,0x02,0x00,0x01,0x00,0x10,0x00,0x16,0x01,0x00,0x00,0x10,0x05,0x00,0x00,0x03
            ,0x00,0x00,0x00,0x12,0x01,0x00,0x01},
        
};

void sms_dummy_unpack()
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    unpack_sms_SLQSGetSMSList_t smslist;
    unpack_sms_SendSMS_t sendsms;
    unpack_sms_SLQSGetSMS_t getrawsms;
    unpack_sms_SLQSWmsMemoryFullCallBack_ind_t full_stat;
    unpack_sms_SetNewSMSCallback_ind_t newsmscbind;
    unpack_sms_SLQSTransLayerInfoCallback_ind_t transLayerInfo;
    unpack_sms_SLQSNWRegInfoCallback_ind_t nwRegInfo;
    unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t msgWait;
    unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t rawAsync;
    unpack_sms_SLQSModifySMSStatus_t ModifySMSStatus;
    unpack_sms_SLQSDeleteSMS_t DeleteSMS;
    unpack_sms_SLQSSetIndicationRegister_t SLQSSetIndicationRegister;
    msgbuf msg;
    int rtn;
    ssize_t rlen;
    int Length = 0;
    int lIndex = 0;
    pack_qmi_t req_ctx;
    int loopCount = 0;
    int index = 0;
    memset(&req_ctx, 0, sizeof(req_ctx));
    loopCount = sizeof(dummy_sms_resp_msg)/sizeof(dummy_sms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index>=loopCount)
            return ;
        /* TODO select multiple file and read them */
        memcpy(&msg.buf,&dummy_sms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSMS, msg.buf, rlen, &rsp_ctx);

            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("SMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            uint8_t count;

            switch(rsp_ctx.msgid)
            {
                case eQMI_WMS_SET_IND_REGISTER:
                    printf("eQMI_WMS_SET_IND_REGISTER ...\n");
                    unpackRetCode = unpack_sms_SLQSSetIndicationRegister( msg.buf, rlen,&SLQSSetIndicationRegister);
                    printf("[%s][line:%d]rtn: %d\n", __func__, __LINE__, unpackRetCode);

#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSSetIndicationRegister",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");
                    }
#endif
                break;



                case eQMI_WMS_RAW_READ:
                    printf("get sms raw read ...\n");
                    getrawsms.messageSize = sizeof(getrawsms.message);
                    unpackRetCode = unpack_sms_SLQSGetSMS( msg.buf, rlen,&getrawsms);
                    printf("[%s][line:%d]rtn: %d\n", __func__, __LINE__, unpackRetCode);
                    swi_uint256_print_mask (getrawsms.ParamPresenceMask);
                    
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSGetSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");
                    }
#endif                    
                    if ((!unpackRetCode && 6 == getrawsms.messageFormat) &&//UMTS-1
                        (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
                    {
                        char  ascii[2048]= {0};
                        uint8_t  SenderAddrLength = MAX_SMSC_LENGTH;
                        uint8_t TextMsgLength = MAX_SMS_MSG_LEN;
                        uint8_t  SMSCAddrLength   = MAX_SMSC_LENGTH;
                        char  SenderAddr[MAX_SMSC_LENGTH+1]   = {'\0'};
                        char  SMSCAddr[MAX_SMSC_LENGTH+1]     = {'\0'};
                        uint8_t  Refnum[5] = {'\0'};
                        uint8_t  Totalnum[5] = {'\0'};
                        uint8_t  Seqnum[5] = {'\0'};
                        int8_t  IsUDHPresent;
                        uint8_t encoding = 0xFF;
                        uint8_t  i = 0;
                        sms_SLQSWCDMADecodeLongTextMsg_t wcdmaLongMsgDecodingParams;
                        memset(&wcdmaLongMsgDecodingParams,0,sizeof(wcdmaLongMsgDecodingParams));
                        wcdmaLongMsgDecodingParams.pMessage          = getrawsms.message;
                        wcdmaLongMsgDecodingParams.MessageLen        = getrawsms.messageSize;
                        wcdmaLongMsgDecodingParams.pSenderAddrLength = &SenderAddrLength;
                        wcdmaLongMsgDecodingParams.pTextMsgLength    = &TextMsgLength;
                        wcdmaLongMsgDecodingParams.pScAddrLength     = &SMSCAddrLength;
                        wcdmaLongMsgDecodingParams.pSenderAddr = SenderAddr;
                        wcdmaLongMsgDecodingParams.pTextMsg    = ascii;
                        wcdmaLongMsgDecodingParams.pScAddr     = SMSCAddr;
                        wcdmaLongMsgDecodingParams.pReferenceNum = Refnum;
                        wcdmaLongMsgDecodingParams.pTotalNum   = Totalnum;
                        wcdmaLongMsgDecodingParams.pPartNum    = Seqnum;
                        wcdmaLongMsgDecodingParams.pIsUDHPresent = &IsUDHPresent;

                        unpackRetCode = sms_SLQSWCDMADecodeLongTextMsg( &wcdmaLongMsgDecodingParams );

#if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "sms_SLQSWCDMADecodeLongTextMsg",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                            checkErrorAndFormErrorString(msg.buf);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,",  "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  "Correct");
                        }
#endif

                        if( eQCWWAN_ERR_NONE != unpackRetCode )
                        {
                            fprintf( stderr,"Failed to Read SMS\n");
                            fprintf( stderr,"Failure Code: %d\n", unpackRetCode );
                            break;
                        }
                        if ( *wcdmaLongMsgDecodingParams.pIsUDHPresent)
                        {
                            concatenated_sms_counter++;
                            if (!buf_allocated && (wcdmaLongMsgDecodingParams.pTotalNum!= NULL))
                            {
                                for ( i = 0; i < *wcdmaLongMsgDecodingParams.pTotalNum; i++)
                                {
                                    if (wcdmaLongMsgDecodingParams.pTextMsgLength == NULL)
                                    {
                                        printf("invalid message lenth pointer!\n");
                                        return;
                                    }
                                    else
                                    {
                                        if (*wcdmaLongMsgDecodingParams.pTextMsgLength <= 0)
                                        {
                                            printf("invalid message size!\n");
                                            return;
                                        }
                                    }
                                    copy_buf[i] = malloc( (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char) );
                                    if (copy_buf[i] == NULL) {
                                        printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                        return;
                                    }  
                                    memset(copy_buf[i], 0, (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char));
                                    buf_allocated = TRUE;
                                }
                            }
                            if (wcdmaLongMsgDecodingParams.pPartNum != NULL)
                            {
                                if (*wcdmaLongMsgDecodingParams.pPartNum > 0)
                                {
                                    if(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]!=NULL)
                                    {
                                        uint16_t userdataLen = 0;
                                        unpackRetCode = sms_SLQSWCDMADecodeMTEncoding(wcdmaLongMsgDecodingParams.pMessage,
                                                          wcdmaLongMsgDecodingParams.MessageLen,
                                                          &encoding,
                                                          &userdataLen);

                                         if ((!unpackRetCode) && (encoding == UCSENCODING))
                                         {
                                             unpackRetCode = smsdemo_processucs2msg((char*)wcdmaLongMsgDecodingParams.pTextMsg,
                                                                    userdataLen,
                                                                    copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]);

                                             if (unpackRetCode == eQCWWAN_ERR_MEMORY)
                                                 return;
                                         }
                                         else
                                         {
                                             strcpy(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1], wcdmaLongMsgDecodingParams.pTextMsg);
                                         }
                                    }
                                }
                            }
                            if(wcdmaLongMsgDecodingParams.pTotalNum!=NULL)
                            {           
                                if (concatenated_sms_counter == (*wcdmaLongMsgDecodingParams.pTotalNum))
                                {
                                    display_long_sms(&wcdmaLongMsgDecodingParams);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            printf("SLQS Get SMS Successful\n");
                            printf("Message Tag: %d\n",getrawsms.messageTag);
                            printf("Message Format: %d\n",getrawsms.messageFormat);
                            printf("Message Size: %d\n",getrawsms.messageSize);
                            printf("Message: \n");
                            for ( count=0;count < getrawsms.messageSize;count++ )
                            {
                                printf("%02X ",(getrawsms.message[count]));
                                if (count % 16 == 15)
                                    printf("\n");
                            }
                            printf("\n");
                                
                            /* Decode as GSM/WCDMA PP */
                            wcdma_decode_text_msg (getrawsms);
                        }
                    }
                    else if ((!unpackRetCode && 0 == getrawsms.messageFormat) &&/* CDMA */
                            (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
                    {
                        char     ascii[2048]= {0};
                        uint8_t  SenderAddrLength = MAX_SMSC_LENGTH;
                        uint8_t  TextMsgLength    = 161;
                        char     SenderAddr[MAX_SMSC_LENGTH+1]   = {'\0'};
                        int8_t   IsUDHPresent;
                        uint8_t  i;
                        uint16_t  CdmaRefnum = 0;
                        uint8_t  CdmaTotalnum = 0;
                        uint8_t  CdmaSeqnum = 0;
                        struct sms_cdmaMsgDecodingParamsExt_t CdmaMsgDecodingParamsExt;

                        memset(&CdmaMsgDecodingParamsExt, 0, sizeof(CdmaMsgDecodingParamsExt));
                        CdmaMsgDecodingParamsExt.pMessage = getrawsms.message;
                        CdmaMsgDecodingParamsExt.messageLength = getrawsms.messageSize;
                        CdmaMsgDecodingParamsExt.pSenderAddr = SenderAddr;
                        CdmaMsgDecodingParamsExt.pSenderAddrLength = &SenderAddrLength;
                        CdmaMsgDecodingParamsExt.pTextMsg = (uint16_t*)ascii;
                        CdmaMsgDecodingParamsExt.pTextMsgLength = &TextMsgLength;
                        CdmaMsgDecodingParamsExt.pReferenceNum  = &CdmaRefnum;
                        CdmaMsgDecodingParamsExt.pTotalNum      = &CdmaTotalnum;
                        CdmaMsgDecodingParamsExt.pPartNum       = &CdmaSeqnum;
                        CdmaMsgDecodingParamsExt.pIsUDHPresent  = &IsUDHPresent;

                        unpackRetCode = sms_SLQSCDMADecodeMTTextMsgExt(&CdmaMsgDecodingParamsExt);

                        if( sMS_HELPER_OK != unpackRetCode )
                        {
                            printf( "Failed to decode SMS\n");
                            printf( "Failure Code: %u\n", unpackRetCode );
                        }
                        switch( unpackRetCode )
                        {
                            case sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED:
                                printf( "Message encoding or Teleservice type not supported\n");
                                break; 

                            case sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED:
                                printf( "Message is not available or is corrupted\n" );
                                break;

                            case sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED:
                                printf( " SMSC/Mobile number is not correct\n" );
                                break;

                            case sMS_HELPER_ERR_BUFFER_SZ:
                                printf( " Internal Error - Can not read SMS \n" );
                                break;

                            case sMS_HELPER_ERR_MEMORY:
                                printf( " Memory allocation failure - Can not read SMS \n" );
                                break;

                            case sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG:
                                printf( " Message exceeds 160 characters\n" );
                                break;

                            case sMS_HELPER_OK:
                                printf ("Decoding CDMA SMS\n");
                                if ( *CdmaMsgDecodingParamsExt.pIsUDHPresent)
                                {
                                    if(concate_prev_ref_number != (*CdmaMsgDecodingParamsExt.pReferenceNum))
                                    {
                                        if(concate_prev_ref_number != 0)
                                        {
                                            if(buf_allocated)
                                            {
                                                for ( i = 0; i < concate_prev_total_number; i++)
                                                {
                                                     if (copy_buf[i] != NULL)
                                                     {
                                                         free(copy_buf[i]);
                                                         copy_buf[i] = NULL;
                                                     }
                                                 }
                                                 buf_allocated = FALSE;
                                            }
                                            concatenated_sms_counter = 0;
                                        }
                                    }

                                    concatenated_sms_counter++;

                                    if (!buf_allocated && (CdmaMsgDecodingParamsExt.pTotalNum!= NULL))
                                    {
                                        for ( i = 0; i < *CdmaMsgDecodingParamsExt.pTotalNum; i++)
                                        {
                                            if (CdmaMsgDecodingParamsExt.pTextMsgLength == NULL)
                                            {
                                                printf("invalid message length pointer!\n");
                                                return;
                                            }
                                            else
                                            {
                                                if (*CdmaMsgDecodingParamsExt.pTextMsgLength <= 0)
                                                {
                                                    printf("invalid message size!\n");
                                                    return;
                                                }
                                            }
                                            copy_buf[i] = malloc( (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char) );
                                            if (copy_buf[i] == NULL) {
                                                printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                                return;
                                            }     
                                            memset(copy_buf[i], 0, (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char));
                                            buf_allocated = TRUE;
                                        }
                                        concate_prev_total_number = *CdmaMsgDecodingParamsExt.pTotalNum;
                                    }
                                    concate_prev_ref_number = *CdmaMsgDecodingParamsExt.pReferenceNum;
                                    if(CdmaMsgDecodingParamsExt.pPartNum!=NULL)
                                    {
                                        if (*CdmaMsgDecodingParamsExt.pPartNum > 0) 
                                        {
                                            if(copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1]!=NULL)
                                            {
                                                strcpy(copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1], (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                                printf("Part %d of Long Message:%s\n",
                                                    (*CdmaMsgDecodingParamsExt.pPartNum),
                                                    (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                            }
                                        }
                                    }
                                    if(CdmaMsgDecodingParamsExt.pTotalNum!=NULL)
                                    {
                                        if (concatenated_sms_counter == (*CdmaMsgDecodingParamsExt.pTotalNum))
                                        {
                                            display_cdma_long_sms(&CdmaMsgDecodingParamsExt);
                                        }
                                    }                                    
                                }

                                else
                                {
                                    /* Print SMS details */
                                    fprintf(stderr,"Sender Number: %s\n",CdmaMsgDecodingParamsExt.pSenderAddr);
                                    fprintf(stderr,"SMS Text: %s\n",(char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                    fprintf(stderr,"Time Stamp: %d:%d:%d:%d:%d:%d\n",
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_YEAR],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MONTH],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_DAY],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_HOUR],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MINUTE],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_SECOND]);
                                }
                                break;
                             default:
                                break;
                        }
                    }

                    break;

                case eQMI_WMS_GET_MSG_LIST:
                    smslist.messageListSize = sizeof(smslist.messageList)/sizeof(qmiSmsMessageList);
                    unpackRetCode = unpack_sms_SLQSGetSMSList( msg.buf, rlen,&smslist);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSGetSMSList",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
#endif                    
                    if(swi_uint256_get_bit (smslist.ParamPresenceMask, 1))
                    {
                        printf("MessageListSize  : %x\n", smslist.messageListSize);
                        for(count = 0; count < smslist.messageListSize; count++)
                        {
                            printf("messageIndex[%d] : %x\n",
                                    count, smslist.messageList[count].messageIndex);
                            printf("messageTag[%d] : %x\n",
                                    count, smslist.messageList[count].messageTag);
                        }
                    }
#if DEBUG_LOG_TO_FILE
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  (smslist.messageListSize > 0) ? "Correct": "Wrong");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  (smslist.messageListSize > 0) ? "Correct": "Wrong");
                    }
#endif

                    break;

                case eQMI_WMS_MODIFY_TAG:
                    unpackRetCode = unpack_sms_SLQSModifySMSStatus( msg.buf, rlen,&ModifySMSStatus);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSModifySMSStatus",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);

                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");
                    }
#endif

                    if(!unpackRetCode)
                        printf("SLQS Modify SMS Status Successful\n");
                    break;

                case eQMI_WMS_DELETE:
                    unpackRetCode = unpack_sms_SLQSDeleteSMS( msg.buf, rlen,&DeleteSMS);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSDeleteSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");
                    }
#endif                    

                    if(!unpackRetCode)
                        printf("SLQS Delete SMS Successful\n");
                    break;

                case eQMI_WMS_RAW_SEND:
                    unpackRetCode = unpack_sms_SendSMS( msg.buf, rlen, &sendsms);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SendSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  (sendsms.messageID > 0) ? "Correct": "Wrong");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  (sendsms.messageID > 0) ? "Correct": "Wrong");
                    }
#endif     

                    if(!unpackRetCode)
                        printf("Send SMS Successful\n");
                    break;
                case eQMI_WMS_SET_EVENT:
                    if(rsp_ctx.type == eIND)
                    {
                        unpackRetCode = unpack_sms_SetNewSMSCallback_ind( msg.buf, rlen, &newsmscbind);
#if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_sms_SetNewSMSCallback_ind",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                            checkErrorAndFormErrorString(msg.buf);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,",  "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  "Correct");
                        }
#endif

                        printf ("SMS IND unpacked: %d\n", unpackRetCode);
                        if ((newsmscbind.NewMMTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 16)))
                        {
                            printf ("NewMMTlv.MTMessageInfo.storageType : %d\n", newsmscbind.NewMMTlv.MTMessageInfo.storageType);
                            printf ("NewMMTlv.MTMessageInfo.messageIndex: %d\n", newsmscbind.NewMMTlv.MTMessageInfo.messageIndex);

                        }
                        if ((newsmscbind.MMTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 18)))
                        {
                            printf ("MMTlv.MessageModeInfo.messageMode : %d\n", newsmscbind.MMTlv.MessageModeInfo.messageMode);
                        }
                        if ((newsmscbind.ETWSTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 19)))
                        {
                            printf ("ETWSTlv.EtwsMessageInfo.notificationType : %d\n", newsmscbind.ETWSTlv.EtwsMessageInfo.notificationType);
                            printf ("ETWSTlv.EtwsMessageInfo.length : %d\n", newsmscbind.ETWSTlv.EtwsMessageInfo.length);
                            Length = newsmscbind.ETWSTlv.EtwsMessageInfo.length;
                            lIndex = 0;
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.ETWSTlv.EtwsMessageInfo.data[lIndex++] );
                                    if (lIndex % 16 ==0)
                                    {
                                            fprintf ( stderr, "\n");
                                    }
                            }        
                            fprintf ( stderr, "\r\n\n" );
                        }
                        if ((newsmscbind.TRMessageTlv.TlvPresent)&&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 17)))
                        {
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.transactionID : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.transactionID);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.format : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.format);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.length : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.length);
                            Length = newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.length;
                            lIndex = 0;
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.data[lIndex++] );
                                    if (lIndex % 16 ==0)
                                    {
                                            fprintf ( stderr, "\n");
                                    }
                            }
                            fprintf ( stderr, "\r\n\n" );
                        }
                        if ((newsmscbind.ETWSPLMNTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 20)))
                        {
                            printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode : %d\n", newsmscbind.ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode);
                            printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode : %d\n", newsmscbind.ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode);
                        }
                        if ((newsmscbind.SMSCTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 21)))
                        {
                            printf ("SMSCTlv.SMSCInfo.length : %d\n", newsmscbind.SMSCTlv.SMSCInfo.length);
                            Length = newsmscbind.SMSCTlv.SMSCInfo.length;
                            lIndex = 0;                            
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.SMSCTlv.SMSCInfo.data[lIndex++] );
                            }
                            fprintf ( stderr, "\r\n\n" );                            
                        }
                        if ((newsmscbind.IMSTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 22)))
                        {
                            printf ("IMSTlv.IMSInfo.smsOnIMS : %d\n", newsmscbind.IMSTlv.IMSInfo.smsOnIMS);
                        }
                        /* send request to read the message content */
                        send_sms_read_req (newsmscbind.NewMMTlv.MTMessageInfo.storageType, newsmscbind.NewMMTlv.MTMessageInfo.messageIndex, newsmscbind.MMTlv.MessageModeInfo.messageMode );
                    }
                    else
                    {
                        unpack_sms_SetNewSMSCallback_t SetNetSMSCallback;
                        unpackRetCode = unpack_sms_SetNewSMSCallback( msg.buf, rlen,&SetNetSMSCallback);
                        if(!unpackRetCode)
                            printf(" Enabled SMS event callback\n");
                    }
                    break;

                case eQMI_WMS_MEMORY_FULL_IND:
                    unpack_sms_SLQSWmsMemoryFullCallBack_ind( msg.buf, rlen, &full_stat);
                    if(swi_uint256_get_bit (full_stat.ParamPresenceMask, 1))
                        printf("sms store full, storage type/message mode : %d/%d\n",
                            full_stat.storageType, full_stat.messageMode);
                    break;
                case eQMI_WMS_TRANS_LAYER_INFO_IND:
                    unpackRetCode = unpack_sms_SLQSTransLayerInfoCallback_ind( msg.buf, rlen, &transLayerInfo);
                    printf("unpack QMI_WMS_TRANS_LAYER_INFO_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                         if(swi_uint256_get_bit (transLayerInfo.ParamPresenceMask, 1))
                            printf("Registration Indication 0x%x\n",transLayerInfo.regInd);
                         if ( (transLayerInfo.pTransLayerInfo) &&
                              (swi_uint256_get_bit (transLayerInfo.ParamPresenceMask, 16)))
                         {
                             printf("Transport Layer Information:\n" );
                             printf("Transport Type: 0x%x\n", 
                                     transLayerInfo.pTransLayerInfo->TransType );
                             printf("Transport Capabilities 0x%x\n", 
                                     transLayerInfo.pTransLayerInfo->TransCap );
                         }
                    }
                    break;
                case eQMI_WMS_TRANS_NW_REG_INFO_IND:
                    unpackRetCode = unpack_sms_SLQSNWRegInfoCallback_ind( msg.buf, rlen, &nwRegInfo);
                    printf("unpack QMI_WMS_TRANS_NW_REG_INFO_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                         if(swi_uint256_get_bit (nwRegInfo.ParamPresenceMask, 1))
                         printf("Network Registration Status 0x%x\n",nwRegInfo.NWRegStat);
                    }
                    break;
                case eQMI_WMS_MSG_WAITING_IND:
                    unpackRetCode = unpack_sms_SLQSWmsMessageWaitingCallBack_ind( msg.buf, rlen, &msgWait);
                    printf("unpack QMI_WMS_MSG_WAITING_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                        if(swi_uint256_get_bit (msgWait.ParamPresenceMask, 1))
                        {
                            uint8_t i;
                            printf("\nreceive Message Waiting Info Indication\n");
                            for ( i = 0; i < msgWait.numInstances; i++)
                            {
                                printf("message type: %d\n", msgWait.msgWaitInfo[i].msgType);
                                printf("active indication: %d\n", msgWait.msgWaitInfo[i].activeInd);
                                printf("message count: %d\n", msgWait.msgWaitInfo[i].msgCount);
                                printf("\n");
                            }
                        }
                        
                    }
                    break;
                case eQMI_WMS_ASYNC_RAW_SEND_IND:
                    unpackRetCode = unpack_sms_SLQSWmsAsyncRawSendCallBack_ind( msg.buf, rlen, &rawAsync);
                    printf("unpack QMI_WMS_ASYNC_RAW_SEND_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 1))
                        printf("Send Status     : %d\n", rawAsync.sendStatus);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 16))
                        printf("Message Id      : %d\n", rawAsync.messageID);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 17))
                        printf("Cause Code      : %d\n", rawAsync.causeCode);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 18))
                        printf("Error Class     : %d\n", rawAsync.errorClass);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 19))
                        printf("RP Cause        : %d\n", rawAsync.RPCause);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 19))
                        printf("TP Cause        : %d\n", rawAsync.TPCause);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 22))
                        {
                            printf("Alpha Id Length : %d\n", rawAsync.alphaIDLen);
                            if (rawAsync.alphaIDLen)
                                printf("Alpha Id: %s\n", rawAsync.pAlphaID);
                        }
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 23))
                        printf("User Data       : %u\n", rawAsync.userData);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 20))
                        printf("Message Delivery Failure Type  : %d\n", rawAsync.msgDelFailureType);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 21))
                        printf("Message Delivery Failure Cause : %d\n", rawAsync.msgDelFailureCause);
                        printf("\n");
                    }
                    break;
                case eQMI_WMS_GET_SMSC_ADDR:
                    {
                        unpack_sms_GetSMSCAddress_t Resp = { maxSMSCAddrLen, smscAddress, maxSMSCTypeLen, smscType, SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_GetSMSCAddress,
                            dump_GetSMSCAddress,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_BC_CONFIG:
                    {
                        unpack_sms_SLQSGetSmsBroadcastConfig_t Resp = {&q3GPPBroadcastCfgInfo, &q3GPP2BroadcastCfgInfo,SWI_UINT256_INT_VALUE};;
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSGetSmsBroadcastConfig,
                            dump_SLQSGetSmsBroadcastConfig,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_IND_REGISTER:
                    {
                        unpack_sms_SLQSGetIndicationRegister_t Resp = { &indicationRegInfo,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSGetIndicationRegister,
                            dump_SLQSGetIndicationRegister,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SWI_GET_SMS_STORAGE:
                    {
                        unpack_sms_SLQSSwiGetSMSStorage_t Resp = { &smsStorage,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSwiGetSMSStorage,
                            dump_SLQSSwiGetSMSStorage,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SWI_SET_SMS_STORAGE:
                    {
                        unpack_sms_SLQSSetSmsStorage_t Resp = { SWI_UINT256_INT_VALUE };
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSetSmsStorage,
                            dump_SLQSSetSmsStorageDevice,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_RAW_WRITE:
                    {
                        unpack_sms_SaveSMS_t Resp = {&messageIndex,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SaveSMS,
                            dump_SaveSMS,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_TRANS_LAYER_INFO:
                    {
                        unpack_sms_SLQSGetTransLayerInfo_t Resp = { &transLayerInfoResp,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSGetTransLayerInfo,
                            dump_SLQSGetTransLayerInfo,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_TRANS_NW_REG_INFO:
                    {
                        unpack_sms_SLQSGetTransNWRegInfo_t Resp = { &transNWRegInfoResp,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSGetTransNWRegInfo,
                            dump_SLQSGetTransNWRegInfo,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_MSG_PROTOCOL:
                    {
                        unpack_sms_SLQSSmsGetMessageProtocol_t Resp = {&msgProtResp,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSmsGetMessageProtocol,
                            dump_SLQSSmsGetMessageProtocol,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_MSG_LIST_MAX:
                    {
                        unpack_sms_SLQSSmsGetMaxStorageSize_t Resp = {&maxStorageSizeresp,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSmsGetMaxStorageSize,
                            dump_SLQSSmsGetMaxStorageSize,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SET_SMSC_ADDR:
                    {
                        unpack_sms_SetSMSCAddress_t Resp = { SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SetSMSCAddress,
                            dump_SetSMSCAddress,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SET_ROUTES:
                    {
                        unpack_sms_SLQSSmsSetRoutes_t Resp = { SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSmsSetRoutes,
                            dump_SLQSSmsSetRoutes,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SET_BC_ACTIVATION:
                    {
                        unpack_sms_SLQSSetSmsBroadcastActivation_t Resp = { SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSetSmsBroadcastActivation,
                            dump_SLQSSetSmsBroadcastActivation,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_SET_BC_CONFIG:
                    {
                        unpack_sms_SLQSSetSmsBroadcastConfig_t Resp = { SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSSetSmsBroadcastConfig,
                            dump_SLQSSetSmsBroadcastConfig,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                case eQMI_WMS_GET_MSG_WAITING:
                    {
                        unpack_sms_SLQSGetMessageWaiting_t Resp = { &getMsgWaitInfo,SWI_UINT256_INT_VALUE};
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_sms_SLQSGetMessageWaiting,
                            dump_SLQSGetMessageWaiting,
                            msg.buf, 
                            rlen, 
                            &Resp);
                    }
                    break;
                default:
                    printf("missing msgid 0x%x handler\n", rsp_ctx.msgid);
                    break;
            }
        }        
    }
   return ;
}

    /************************** dummy SMS messages  ***********************/
    /* GSM */
    /* Valid message */
    uint8_t sms_gsm_msg[]=
    {0x02,0x01,0x00,0x22,0x00,0x60,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x56
     ,0x00,0x01,0x06,0x52,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xf2,0x24,0x0b,0xd0
     ,0xb1,0x19,0xce,0xda,0x9c,0x02,0x41,0x00,0x81,0x80,0x92,0x31,0x40,0x22,0x23,0x3e
     ,0xd9,0x77,0x1d,0x84,0x0e,0xdb,0xcb,0xa0,0x30,0xc8,0x5d,0xbe,0x83,0xec,0xef,0xf4
     ,0xb8,0xdc,0x0e,0xa7,0xd9,0xa0,0x76,0x79,0x3e,0x0f,0x9f,0xcb,0x3b,0x10,0x9c,0x5d
     ,0x0e,0xcf,0xcb,0xa0,0x71,0x98,0xcd,0x06,0xc5,0x66,0x38,0x10,0xfd,0x0d,0x92,0x97
     ,0xe9,0xf2,0x74,0xd9,0x5e,0x76,0x01};

    /* valid 8 bit encoded GSM message */
    uint8_t sms_gsm_msg8bit[]={
      0x02,0x04,0x00,0x22,0x00,0x2C,0x00,0x02,0x04,0x00,0x00,0x00,
      0x00,0x00,0x01,0x22,0x00,0x01,0x06,0x1E,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xF2,0x04,
      0x08,0x84,0x55,0x00,0x53,0x83,0x00,0x04,0x91,0x50,0x51,0x01,0x03,0x04,0x23,0x05,0x00,0x41,
      0x42,0x43,0x00};

    /* valid message with UCS2 encoding */
    uint8_t sms_gsm_msg_ucs2[]=
    {0x02, 0x04, 0x00, 0x22, 0x00, 0x3F, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x35,
     0x00, 0x01, 0x06, 0x31, 0x00, 0x07, 0x91, 0x19, 0x07, 0x90, 0x70, 0x05, 0x90, 0x04, 0x0C, 0x91,
     0x19, 0x26, 0x38, 0x20, 0x98, 0x16, 0x00, 0x08, 0x91, 0x10, 0x03, 0x31, 0x14, 0x02, 0x22, 0x16,
     0x4F, 0x55, 0x30, 0x4C, 0x4E, 0xF2, 0x95, 0x93, 0x30, 0x6B, 0x8D, 0x77, 0x30, 0x53, 0x30, 0x63,
     0x30, 0x66, 0x30, 0x44, 0x30, 0x8B};

    /* unsupported message type (status report) */
    uint8_t sms_gsm_msg_status_report[]=
    {0x02,0x01,0x00,0x22,0x00,0x60,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x56
     ,0x00,0x01,0x06,0x52,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xf2,0x26,0x0b,0xd0
     ,0xb1,0x19,0xce,0xda,0x9c,0x02,0x41,0x00,0x81,0x80,0x92,0x31,0x40,0x22,0x23,0x3e
     ,0xd9,0x77,0x1d,0x84,0x0e,0xdb,0xcb,0xa0,0x30,0xc8,0x5d,0xbe,0x83,0xec,0xef,0xf4
     ,0xb8,0xdc,0x0e,0xa7,0xd9,0xa0,0x76,0x79,0x3e,0x0f,0x9f,0xcb,0x3b,0x10,0x9c,0x5d
     ,0x0e,0xcf,0xcb,0xa0,0x71,0x98,0xcd,0x06,0xc5,0x66,0x38,0x10,0xfd,0x0d,0x92,0x97
     ,0xe9,0xf2,0x74,0xd9,0x5e,0x76,0x01};

    /* unsupported message encoding */
    uint8_t sms_gsm_msg_unsupported_encoding[]=
    {0x02,0x01,0x00,0x22,0x00,0x60,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x56
     ,0x00,0x01,0x06,0x52,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xf2,0x24,0x0b,0xd0
     ,0xb1,0x19,0xce,0xda,0x9c,0x02,0x41,0x09,0x81,0x80,0x92,0x31,0x40,0x22,0x23,0x3e
     ,0xd9,0x77,0x1d,0x84,0x0e,0xdb,0xcb,0xa0,0x30,0xc8,0x5d,0xbe,0x83,0xec,0xef,0xf4
     ,0xb8,0xdc,0x0e,0xa7,0xd9,0xa0,0x76,0x79,0x3e,0x0f,0x9f,0xcb,0x3b,0x10,0x9c,0x5d
     ,0x0e,0xcf,0xcb,0xa0,0x71,0x98,0xcd,0x06,0xc5,0x66,0x38,0x10,0xfd,0x0d,0x92,0x97
     ,0xe9,0xf2,0x74,0xd9,0x5e,0x76,0x01};

    /* GSM long message */
    uint8_t sms_gsm_long_msg1[]=
    {0x02,0x0a,0x00,0x22,0x00,0xb3,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xa9
      ,0x00,0x01,0x06,0xa5,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x33,0xf4,0x44,0x08,0x84
      ,0x69,0x18,0x28,0x55,0x00,0x00,0x81,0x80,0x92,0x41,0x50,0x25,0x23,0xa0,0x05,0x00
      ,0x03,0x00,0x02,0x01,0x36,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3
      ,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe2,0x91,0x38,0x35,0x1e
      ,0x8d,0xc4,0xa3,0x91,0xb8,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b
      ,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0x6b,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb
      ,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x22
      ,0x1e,0x89,0x53,0xe3,0xd1,0x48,0x3c,0x1a,0x89,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e
      ,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x36,0x2f,0xe7
      ,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9
      ,0x79,0x39,0x37,0x2f,0xe2,0x91,0x38,0x35,0x1e,0x8d};

    uint8_t sms_gsm_long_msg2[]=
    {0x02,0x0a,0x00,0x22,0x00,0xae,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xa4
      ,0x00,0x01,0x06,0xa0,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x33,0xf4,0x44,0x08,0x84
      ,0x69,0x18,0x28,0x55,0x00,0x00,0x81,0x80,0x92,0x41,0x50,0x45,0x23,0x9a,0x05,0x00
      ,0x03,0x00,0x02,0x02,0x88,0x47,0x23,0x71,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9
      ,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c,0x9b,0xd7,0xe6,0xe5,0xdc,0xbc
      ,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f
      ,0xe7,0xe6,0x45,0x3c,0x12,0xa7,0xc6,0xa3,0x91,0x78,0x34,0x12,0x37,0x2f,0xe7,0xe6
      ,0xe5,0xdc,0xbc,0x9c,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79
      ,0x6d,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5,0xdc,0xbc,0x9c
      ,0x9b,0x97,0x73,0xf3,0x72,0x6e,0x5e,0xc4,0x23,0x71,0x6a,0x3c,0x1a,0x89,0x47,0x23
      ,0x71,0xf3,0x72,0x6e,0x5e,0xce,0xcd,0xcb,0xb9,0x79,0x39,0x37,0x2f,0xe7,0xe6,0xe5
      ,0xdc,0xbc,0x9c,0x9b,0x17};

    /* CDMA */
    /* CDMA normal message */
    uint8_t sms_cdma_msg[]=
    {0x02,0x91,0x00,0x22,0x00,0x45,0x00,0x02,0x04,0x00,0x00,0x00,
     0x00,0x00,0x01,0x3B,0x00,0x00,0x00,0x37,0x00,0x00,0x00,0x02,
     0x10,0x02,0x02,0x08,0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
     0x08,0x26,0x00,0x03,0x10,0x00,0x00,0x01,0x19,0x10,0xd5,0x7c,
     0xbb,0x31,0xef,0xdb,0x95,0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,
     0x96,0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,0x60,0x05,0x01,
     0x0b,0x08,0x01,0x00};

    /* encoding not supported */
    uint8_t sms_cdma_msg_encoding[]=
    {0x02,0x91,0x00,0x22,0x00,0x45,0x00,0x02,0x04,0x00,0x00,0x00,
    0x00,0x00,0x01,0x3B,0x00,0x00,0x00,0x37,0x00,0x00,0x00,0x02,
    0x10,0x02,0x02,0x08,0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
    0x08,0x26,0x00,0x03,0x10,0x00,0x00,0x01,0x19,0x20,0xd5,0x7c,
    0xbb,0x31,0xef,0xdb,0x95,0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,
    0x96,0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,0x60,0x05,0x01,
    0x0b,0x08,0x01,0x00};

    /* teleservice id  not supported */
    uint8_t sms_cdma_msg_teleid[]=
    {0x02,0x91,0x00,0x22,0x00,0x45,0x00,0x02,0x04,0x00,0x00,0x00,
    0x00,0x00,0x01,0x3B,0x00,0x00,0x00,0x37,0x00,0x00,0x00,0x02,
    0x10,0x07,0x02,0x08,0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
    0x08,0x26,0x00,0x03,0x10,0x00,0x00,0x01,0x19,0x20,0xd5,0x7c,
    0xbb,0x31,0xef,0xdb,0x95,0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,
    0x96,0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,0x60,0x05,0x01,
    0x0b,0x08,0x01,0x00};

    /* message type not supported */
    uint8_t sms_cdma_msg_msgtype[]=
    {0x02,0x91,0x00,0x22,0x00,0x45,0x00,0x02,0x04,0x00,0x00,0x00,
    0x00,0x00,0x01,0x3B,0x00,0x00,0x00,0x37,0x00,0x00,0x00,0x02,
    0x10,0x07,0x02,0x08,0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
    0x08,0x26,0x00,0x03,0x80,0x00,0x00,0x01,0x19,0x20,0xd5,0x7c,
    0xbb,0x31,0xef,0xdb,0x95,0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,
    0x96,0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,0x60,0x05,0x01,
    0x0b,0x08,0x01,0x00};

    /* valid long CDMA message */
    uint8_t sms_cdma_long_msg1[] = {
    2, 10, 0, 34, 0,187,0,2,
    4,0,0,0,0,0,1,177,0,0,0,173 ,0,
    0, 0, 2, 16, 5, 2, 7, 2,
    152, 97, 86, 5, 137, 128, 8, 157,
    0, 3, 17, 26, 136, 1, 142, 77,
    0, 40, 0, 29, 136, 24, 13, 70,
    65, 19, 66, 80, 106, 85, 73, 1,
    81, 209, 232, 61, 47, 61, 1, 132,
    66, 244, 246, 154, 8, 222, 161, 48,
    40, 84, 94, 63, 37, 151, 204, 98,
    117, 94, 191, 101, 183, 216, 108, 126,
    95, 63, 165, 215, 236, 118, 127, 90,
    10, 163, 163, 208, 114, 86, 122, 11,
    8, 133, 233, 229, 60, 25, 181, 228,
    104, 120, 36, 94, 75, 53, 164, 215,
    108, 54, 94, 11, 21, 144, 205, 107,
    53, 223, 11, 149, 132, 195, 98, 53,
    94, 203, 117, 192, 228, 68, 82, 118,
    122, 15, 73, 201, 64, 101, 20, 191,
    59, 160, 134, 120, 56, 80, 84, 62,
    37, 23, 140, 66, 97, 84, 190, 101,
    55, 152, 76, 106, 85, 62, 165, 87,
    172, 86, 107, 82, 8, 3, 6, 23,
    6, 48, 16, 72, 81};

    uint8_t sms_cdma_long_msg2[] = {
     2, 11, 0, 34, 0,187,0,2,
     4,0,0,0,0,0,1,177,0,0,0,173 ,0,
     0, 0, 2, 16, 5, 2, 7, 2,
     152, 97, 86, 5, 137, 128, 8, 157,
     0, 3, 17, 26, 152, 1, 142, 77,
     0, 40, 0, 29, 136, 24, 18, 1,
     142, 204, 98, 181, 158, 235, 129, 202,
     194, 193, 225, 52, 190, 107, 60, 44,
     20, 82, 118, 122, 15, 73, 201, 64,
     101, 20, 191, 59, 166, 128, 53, 170,
     76, 14, 23, 17, 141, 203, 97, 245,
     30, 159, 81, 173, 223, 107, 246, 31,
     31, 145, 205, 235, 117, 255, 31, 155,
     208, 128, 172, 236, 244, 30, 147, 154,
     128, 194, 33, 126, 127, 77, 0, 107,
     127, 24, 30, 11, 21, 144, 205, 107,
     53, 223, 11, 149, 132, 195, 98, 53,
     94, 203, 117, 192, 225, 97, 52, 222,
     139, 85, 176, 217, 112, 61, 21, 22,
     155, 154, 131, 208, 112, 84, 31, 71,
     41, 200, 238, 39, 156, 12, 22, 17,
     13, 139, 65, 225, 20, 158, 81, 45,
     159, 75, 226, 21, 24, 3, 6, 23,
     6, 48, 16, 72, 82
     };

    uint8_t sms_cdma_long_msg3[] = {
    2, 12, 0, 34, 0,88,0,2,
    4,0,0,0,0,0,1,78,0,0,0,74 ,0,
    0, 0, 2, 16, 5, 2, 7, 2,
    152, 97, 86, 5, 137, 128, 8, 58,
    0, 3, 17, 26, 168, 1, 40, 73,
    88, 40, 0, 29, 136, 24, 29, 34,
    155, 86, 171, 214, 43, 53, 169, 2,
    130, 97, 181, 30, 171, 97, 186, 224,
    112, 176, 180, 126, 75, 47, 152, 202,
    34, 40, 178, 10, 41, 56, 136, 3,
    6, 23, 6, 48, 16, 72, 82, 10,
    1, 64};

    /* Long sms, 2 messages, second message encoding not supported */
    uint8_t sms_cdma_long_ivmsg1[]=           
    {0x02,0x93,0x00,0x22,0x00,0x8B,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x81,
     0x00,0x00,0x00,0x7D,0x00,0x00,0x00,0x02,0x10,0x05,0x02,0x05,0x01,0x9E,0x21,0xA0,
     0xC0,0x08,0x6F,0x00,0x03,0x1C,0xA5,0x88,0x01,0x5D,0x4B,0x40,0x28,0x00,0x1A,0xA0,
     0x10,0x17,0x47,0x4D,0xB8,0xD9,0x21,0x74,0xDE,0xFF,0x91,0x94,0x44,0x71,0x74,0x1E,
     0x47,0x29,0x84,0xCC,0xEB,0xB4,0xBB,0xA8,0x54,0x2C,0xE3,0x73,0xFE,0x9E,0x5D,0x77,
     0x8D,0xDF,0xEB,0x96,0xDF,0x2F,0x26,0xC2,0x6D,0x0B,0x44,0x0D,0x27,0x7F,0x88,0xD2,
     0x22,0x3C,0xBA,0x0E,0xB1,0xBD,0xED,0x62,0x82,0x46,0x5B,0x83,0xB0,0xF1,0xA5,0x29,
     0xF5,0x09,0x07,0xD0,0xD8,0xA5,0x7D,0x3F,0x49,0x60,0x85,0x96,0x44,0x24,0x16,0x63,
     0x79,0xC9,0x47,0x69,0xB5,0xF8,0xD8,0x03,0x06,0x17,0x09,0x26,0x12,0x23,0x02,0x08,
     0x01,0x00};

    uint8_t sms_cdma_long_ivmsg2[]= 
    {0x02,0x97,0x00,0x22,0x00,0xBB,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0xB1,
     0x00,0x00,0x00,0xAD,0x00,0x00,0x00,0x02,0x10,0x05,0x02,0x07,0x02,0xA2,0xA0,0xD5,
     0x15,0x20,0xC0,0x08,0x9D,0x00,0x03,0x10,0x01,0xE8,0x01,0x8E,0x22,0x30,0x28,0x00,
     0x19,0x80,0x10,0x08,0x02,0x40,0x03,0x08,0x03,0xB0,0x03,0x28,0x01,0x00,0x03,0xC8,
     0x03,0x78,0x03,0xA8,0x01,0x00,0x03,0x98,0x03,0x28,0x03,0x28,0x03,0x70,0x01,0x00,
     0x03,0xA0,0x03,0x40,0x03,0x28,0x01,0x00,0x02,0x18,0x03,0x08,0x03,0x98,0x03,0x40,
     0x01,0x00,0x03,0x08,0x03,0x80,0x03,0x80,0x01,0xF8,0x01,0x00,0x02,0xA0,0x03,0x90,
     0x03,0xC8,0x01,0x00,0x03,0x48,0x03,0xA0,0x01,0x00,0x03,0xA8,0x03,0x98,0x03,0x48,
     0x03,0x70,0x03,0x38,0x01,0x00,0x03,0x68,0x03,0xC8,0x01,0x00,0x03,0x18,0x03,0x78,
     0x03,0x20,0x03,0x28,0x01,0x00,0x03,0x08,0x03,0x70,0x03,0x20,0x01,0x00,0x03,0xB8,
     0x03,0x29,0x00,0xC8,0x03,0x60,0x03,0x60,0x01,0x00,0x03,0x28,0x03,0x08,0x03,0x18,
     0x03,0x40,0x01,0x00,0x03,0x38,0x03,0x28,0x03,0xA0,0x03,0x06,0x17,0x11,0x01,0x23,
     0x24,0x23};
     const uint8_t sms_wcdms_test[]={
      0x02,0x03,0x00,0x22,0x00,0x32,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x28,
      0x00,0x01,0x06,0x24,0x00,0x08,0x91,0x98,0x89,0x25,0x36,0x43,0x09,0x23,0x04,
      0x0B,0x91,0x58,0x92,0x21,0x75,0x86,0xF3,0x00,0x00,0x81,0x01,0x52,0x61,0x52,
      0x75,0x23,0x09,0x80,0xA0,0x70,0x08,0x1A,0x05,0x85,0x43
    };
void sms_test_encode_decode_dummy_unpack(msgbuf msg)
{
    unpack_sms_SLQSGetSMS_t getrawsms;
    getrawsms.messageSize = sizeof(getrawsms.message);
    ssize_t rlen=msg.type;
    int unpackRetCode = unpack_sms_SLQSGetSMS( msg.buf, rlen,&getrawsms);
    printf("[%s]: unpack_sms_SLQSGetSMS returned %d\n", __func__, unpackRetCode);
    swi_uint256_print_mask (getrawsms.ParamPresenceMask);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,", "unpack_sms_SLQSGetSMS",\
          ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
    checkErrorAndFormErrorString(msg.buf);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,",  "Correct");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n",  "Correct");
    }
#endif

    if ((!unpackRetCode && (0 == getrawsms.messageFormat)) &&/*CDMA */
        (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
    {
        char     ascii[2048]= {0};
        uint8_t  SenderAddrLength = MAX_SMSC_LENGTH;
        uint8_t  TextMsgLength    = 161;
        char     SenderAddr[MAX_SMSC_LENGTH+1]   = {'\0'};
        int8_t   IsUDHPresent;
        uint8_t  i;
        uint16_t  CdmaRefnum = 0;
        uint8_t  CdmaTotalnum = 0;
        uint8_t  CdmaSeqnum = 0;
        struct sms_cdmaMsgDecodingParamsExt_t CdmaMsgDecodingParamsExt;

        memset(&CdmaMsgDecodingParamsExt, 0, sizeof(CdmaMsgDecodingParamsExt));
        CdmaMsgDecodingParamsExt.pMessage = getrawsms.message;
        CdmaMsgDecodingParamsExt.messageLength = getrawsms.messageSize;
        CdmaMsgDecodingParamsExt.pSenderAddr = SenderAddr;
        CdmaMsgDecodingParamsExt.pSenderAddrLength = &SenderAddrLength;
        CdmaMsgDecodingParamsExt.pTextMsg = (uint16_t*)ascii;
        CdmaMsgDecodingParamsExt.pTextMsgLength = &TextMsgLength;
        CdmaMsgDecodingParamsExt.pReferenceNum  = &CdmaRefnum;
        CdmaMsgDecodingParamsExt.pTotalNum      = &CdmaTotalnum;
        CdmaMsgDecodingParamsExt.pPartNum       = &CdmaSeqnum;
        CdmaMsgDecodingParamsExt.pIsUDHPresent  = &IsUDHPresent;

        unpackRetCode = sms_SLQSCDMADecodeMTTextMsgExt(&CdmaMsgDecodingParamsExt);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "sms_SLQSCDMADecodeMTTextMsgExt",\
        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg.buf);
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",  "Correct");
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n",  "Correct");
        }
    #endif

        if( sMS_HELPER_OK != unpackRetCode )
        {
            printf( "Failed to decode SMS properly\n");
            printf( "Failure Code: %u\n", unpackRetCode );

            if ((CdmaMsgDecodingParamsExt.pSenderAddr) &&
                (CdmaMsgDecodingParamsExt.pSenderAddrLength)) {
                if (*CdmaMsgDecodingParamsExt.pSenderAddrLength)
                    printf( "Sender Number: %s\n",CdmaMsgDecodingParamsExt.pSenderAddr);
            }
            printf( "Time Stamp: %d:%d:%d:%d:%d:%d\n",
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_YEAR],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MONTH],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_DAY],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_HOUR],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MINUTE],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_SECOND]);
            
        }
        switch( unpackRetCode )
        {
            case sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED:
                printf( "Message encoding or Teleservice type not supported\n");
                break; 

            case sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED:
                printf( "Message is not available or is corrupted\n" );
                break;

            case sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED:
                printf( " SMSC/Mobile number is not correct\n" );
                break;

            case sMS_HELPER_ERR_BUFFER_SZ:
                printf( " Internal Error - Can not read SMS \n" );
                break;

            case sMS_HELPER_ERR_MEMORY:
                printf( " Memory allocation failure - Can not read SMS \n" );
                break;

            case sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG:
                printf( " Message exceeds 160 characters\n" );
                break;

            case sMS_HELPER_OK:
                printf ("Decoding CDMA SMS\n");
                if ( *CdmaMsgDecodingParamsExt.pIsUDHPresent)
                {
                    printf ("Display CDMA Long SMS\n");

                    /* display_cdma_long_sms */
                    if(concate_prev_ref_number != (*CdmaMsgDecodingParamsExt.pReferenceNum))
                    {
                        if(concate_prev_ref_number != 0)
                        {
                            if(buf_allocated)
                            {
                                for ( i = 0; i < concate_prev_total_number; i++)
                                {
                                    if (copy_buf[i] != NULL) {
                                        free(copy_buf[i]);
                                        copy_buf[i] = NULL;
                                    }
                                }
                                buf_allocated = FALSE;
                            }
                            concatenated_sms_counter = 0;
                        }
                    }
                    concatenated_sms_counter++;

                    if (!buf_allocated && (CdmaMsgDecodingParamsExt.pTotalNum!= NULL))
                    {
                        for ( i = 0; i < *CdmaMsgDecodingParamsExt.pTotalNum; i++)
                        {
                            if (CdmaMsgDecodingParamsExt.pTextMsgLength == NULL)
                            {
                                printf("invalid message length pointer!\n");
                                return;
                            }
                            else
                            {
                                if (*CdmaMsgDecodingParamsExt.pTextMsgLength <= 0)
                                {
                                    printf("invalid message size!\n");
                                    return;
                                }
                            }
                            copy_buf[i] = malloc( (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char) );
                            if (copy_buf[i] == NULL) {
                                printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                return;
                            }     
                            memset(copy_buf[i], 0, (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char));
                            buf_allocated = TRUE;
                        }
                        concate_prev_total_number = *CdmaMsgDecodingParamsExt.pTotalNum;
                    }
                    concate_prev_ref_number = *CdmaMsgDecodingParamsExt.pReferenceNum;
                    if(CdmaMsgDecodingParamsExt.pPartNum != NULL)
                    {
                        if (*CdmaMsgDecodingParamsExt.pPartNum > 0) 
                        {
                            if(copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1]!=NULL)
                            {
                                strcpy(copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1], (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                printf("Part %d of Long Message:%s\n",
                                   (*CdmaMsgDecodingParamsExt.pPartNum),
                                   (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                            }
                        }
                    }
                    if(CdmaMsgDecodingParamsExt.pTotalNum!=NULL)
                    {
                        if (concatenated_sms_counter == (*CdmaMsgDecodingParamsExt.pTotalNum))
                        {
                            display_cdma_long_sms(&CdmaMsgDecodingParamsExt);
                        }
                    }                    
                }

                else
                {
                    /* Print SMS details */
                    fprintf(stderr,"Sender Number: %s\n",CdmaMsgDecodingParamsExt.pSenderAddr);
                    fprintf(stderr,"SMS Text: %s\n",(char*)CdmaMsgDecodingParamsExt.pTextMsg);
                    fprintf(stderr,"Time Stamp: %d:%d:%d:%d:%d:%d\n",
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_YEAR],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MONTH],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_DAY],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_HOUR],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MINUTE],
                            CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_SECOND]);
                }
                break;
            default:
                printf( "Unknown error code\n");
                break;
        }
    }
    else if ((!unpackRetCode && (6 == getrawsms.messageFormat)) &&/*UMTS-1 */
            (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
    {
        char  ascii[2048]= {0};
        uint8_t  SenderAddrLength = MAX_SMSC_LENGTH;
        uint8_t TextMsgLength = MAX_SMS_MSG_LEN;
        uint8_t  SMSCAddrLength   = MAX_SMSC_LENGTH;
        char  SenderAddr[MAX_SMSC_LENGTH+1]   = {'\0'};
        char  SMSCAddr[MAX_SMSC_LENGTH+1]     = {'\0'};
        uint8_t  Refnum[5] = {'\0'};
        uint8_t  Totalnum[5] = {'\0'};
        uint8_t  Seqnum[5] = {'\0'};
        int8_t  IsUDHPresent;
        uint8_t encoding = 0xFF;
        uint8_t  i = 0;
        sms_SLQSWCDMADecodeLongTextMsg_t wcdmaLongMsgDecodingParams;

        memset(&wcdmaLongMsgDecodingParams, 0, sizeof(wcdmaLongMsgDecodingParams));
        wcdmaLongMsgDecodingParams.pMessage          = getrawsms.message;
        wcdmaLongMsgDecodingParams.MessageLen        = getrawsms.messageSize;
        wcdmaLongMsgDecodingParams.pSenderAddrLength = &SenderAddrLength;
        wcdmaLongMsgDecodingParams.pTextMsgLength    = &TextMsgLength;
        wcdmaLongMsgDecodingParams.pScAddrLength     = &SMSCAddrLength;
        wcdmaLongMsgDecodingParams.pSenderAddr = SenderAddr;
        wcdmaLongMsgDecodingParams.pTextMsg    = ascii;
        wcdmaLongMsgDecodingParams.pScAddr     = SMSCAddr;
        wcdmaLongMsgDecodingParams.pReferenceNum = Refnum;
        wcdmaLongMsgDecodingParams.pTotalNum   = Totalnum;
        wcdmaLongMsgDecodingParams.pPartNum    = Seqnum;
        wcdmaLongMsgDecodingParams.pIsUDHPresent = &IsUDHPresent;

        unpackRetCode = sms_SLQSWCDMADecodeLongTextMsg( &wcdmaLongMsgDecodingParams );

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "sms_SLQSWCDMADecodeLongTextMsg",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg.buf);
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",  "Correct");
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n",  "Correct");
        }
    #endif

        if( sMS_HELPER_OK != unpackRetCode )
        {
            fprintf( stderr,"Failed to decode SMS\n");
            fprintf( stderr,"Failure Code: %d\n", unpackRetCode );
            if ((wcdmaLongMsgDecodingParams.pSenderAddr) &&
                (wcdmaLongMsgDecodingParams.pSenderAddrLength))
                printf( "Sender Number: %s\n",wcdmaLongMsgDecodingParams.pSenderAddr);
        }

        switch( unpackRetCode )
        {
            case sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED:
                printf("Message encoding or message type not supported or invalid\n");
                printf("Error Message:%s \n",wcdmaLongMsgDecodingParams.pTextMsg);
                break;

            case sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED:
                printf( "Message is not available or is corrupted\n" );
                break;

            case sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED:
                printf( " SMSC/Mobile number is not correct\n" );
                break;

            case sMS_HELPER_ERR_BUFFER_SZ:
                printf( " Invalid buffer size \n" );
                break;

            case sMS_HELPER_ERR_MEMORY:
                printf( "Memory allocation failure \n" );
                break;

            case sMS_HELPER_OK:
                if ( *wcdmaLongMsgDecodingParams.pIsUDHPresent)
                {
                    concatenated_sms_counter++;
                    if (!buf_allocated && (wcdmaLongMsgDecodingParams.pTotalNum!= NULL))
                    {
                        for ( i = 0; i < *wcdmaLongMsgDecodingParams.pTotalNum; i++)
                        {
                            if (wcdmaLongMsgDecodingParams.pTextMsgLength == NULL)
                            {
                                printf("invalid message length pointer!\n");
                                return;
                            }
                            else
                            {
                                if (*wcdmaLongMsgDecodingParams.pTextMsgLength <= 0)
                                {
                                    printf("invalid message size!\n");
                                    return;
                                }
                            }
                            copy_buf[i] = malloc( (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char) );
                            if (copy_buf[i] == NULL) {
                                printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                return;
                            }                            
                            memset(copy_buf[i], 0, (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char));
                            buf_allocated = TRUE;
                        }
                    }
                    if(wcdmaLongMsgDecodingParams.pPartNum!=NULL)
                    {
                        if (*wcdmaLongMsgDecodingParams.pPartNum > 0)
                        {
                            if(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]!=NULL)
                            {
                                uint16_t userdataLen = 0;
                                unpackRetCode = sms_SLQSWCDMADecodeMTEncoding(wcdmaLongMsgDecodingParams.pMessage,
                                                  wcdmaLongMsgDecodingParams.MessageLen,
                                                  &encoding,
                                                  &userdataLen);

                                 if ((!unpackRetCode) && (encoding == UCSENCODING))
                                 {

                                     unpackRetCode = smsdemo_processucs2msg((char*)wcdmaLongMsgDecodingParams.pTextMsg,
                                                            userdataLen,
                                                            copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]);

                                     if (unpackRetCode == eQCWWAN_ERR_MEMORY)
                                         return;
                                 }
                                 else
                                 {
                                     strcpy(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1], wcdmaLongMsgDecodingParams.pTextMsg);
                                 }

                            }
                        }   
                    }                    
                    if(wcdmaLongMsgDecodingParams.pTotalNum!=NULL)
                    {
                        if (concatenated_sms_counter == (*wcdmaLongMsgDecodingParams.pTotalNum))
                        {
                            display_long_sms(&wcdmaLongMsgDecodingParams);
                        }
                    }                    
                }
                else
                {
                    uint8_t count;
                    printf("SLQS Get SMS Successful\n");
                    printf("Message Tag: %d\n",getrawsms.messageTag);
                    printf("Message Format: %d\n",getrawsms.messageFormat);
                    printf("Message Size: %d\n",getrawsms.messageSize);
                    printf("Message: \n");
                    for ( count=0;count < getrawsms.messageSize;count++ )
                    {
                    printf("%02X ",(getrawsms.message[count]));
                    if (count % 16 == 15)
                        printf("\n");
                    }
                    printf("\n");

                    /* Decode as GSM/WCDMA PP */
                    wcdma_decode_text_msg (getrawsms);
                }
                break;
            default:
                break;
        }
    }
}

void sms_test_msg_encode_decode()
{

    msgbuf buffer={0,{0}};
    printf("========sms dummy unpack to test encoding decoding========\n");

    #if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "SMS Encoding Decoding Test", "Status", "Payload Parsing");
    #endif    

    printf("----------Running quick tests for sms decoding--------------\n");
    printf("\nTest: decoding normal length and valid GSM/WCDMA message\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_wcdms_test,sizeof(sms_wcdms_test));
    buffer.type = sizeof(sms_wcdms_test);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding normal length and valid GSM/WCDMA message\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_msg,sizeof(sms_gsm_msg));
    buffer.type = sizeof(sms_gsm_msg);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding normal length and valid GSM/WCDMA message with 8bit encoding\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_msg8bit,sizeof(sms_gsm_msg8bit));
    buffer.type = sizeof(sms_gsm_msg8bit);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding normal length and valid GSM/WCDMA message with UCS2 encoding\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_msg_ucs2,sizeof(sms_gsm_msg_ucs2));
    buffer.type = sizeof(sms_gsm_msg_ucs2);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("Test: decoding GSM/WCDMA message with unsupported/invalid message type\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_msg_status_report,sizeof(sms_gsm_msg_status_report));
    buffer.type = sizeof(sms_gsm_msg_status_report);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding GSM/WCDMA message with unsupported/invalid encoding scheme\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_msg_unsupported_encoding,sizeof(sms_gsm_msg_unsupported_encoding));
    buffer.type = sizeof(sms_gsm_msg_unsupported_encoding);
    sms_test_encode_decode_dummy_unpack(buffer);

    sleep(1);

    printf("\nTest: decoding long GSM/WCDMA message\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_long_msg1,sizeof(sms_gsm_long_msg1));
    buffer.type = sizeof(sms_gsm_long_msg1);
    sms_test_encode_decode_dummy_unpack(buffer);

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_gsm_long_msg2,sizeof(sms_gsm_long_msg2));
    buffer.type = sizeof(sms_gsm_long_msg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    sleep(1);

    printf("\n");
    printf("\nTest: decoding normal length and valid CDMA message\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_msg,sizeof(sms_cdma_msg));
    buffer.type = sizeof(sms_cdma_msg);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding CDMA message with unsupported/invalid encoding\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_msg_encoding,sizeof(sms_cdma_msg_encoding));
    buffer.type = sizeof(sms_cdma_msg_encoding);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding CDMA message with unsupported/invalid teleservice id\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_msg_teleid,sizeof(sms_cdma_msg_teleid));
    buffer.type = sizeof(sms_cdma_msg_teleid);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\nTest: decoding CDMA message with unsupported/invalid message type\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_msg_msgtype,sizeof(sms_cdma_msg_msgtype));
    buffer.type = sizeof(sms_cdma_msg_msgtype);
    sms_test_encode_decode_dummy_unpack(buffer);

    sleep(1);
    printf("\nTest: decoding valid long CDMA message\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_long_msg1,sizeof(sms_cdma_long_msg1));
    buffer.type = sizeof(sms_cdma_long_msg1);
    sms_test_encode_decode_dummy_unpack(buffer);

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_long_msg2,sizeof(sms_cdma_long_msg2));
    buffer.type = sizeof(sms_cdma_long_msg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_long_msg3,sizeof(sms_cdma_long_msg3));
    buffer.type = sizeof(sms_cdma_long_msg3);
    sms_test_encode_decode_dummy_unpack(buffer);

    sleep(1);
    printf("\nTest: decoding long CDMA message with one valid and one invalid part\n");

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_long_ivmsg1,sizeof(sms_cdma_long_ivmsg1));
    buffer.type = sizeof(sms_cdma_long_ivmsg1);
    sms_test_encode_decode_dummy_unpack(buffer);

    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,sms_cdma_long_ivmsg2,sizeof(sms_cdma_long_ivmsg2));
    buffer.type = sizeof(sms_cdma_long_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    printf("\n");

    printf("----------Running extensive tests for sms decoding--------------\n");
    sms_helper_test();
    printf("\n");
}


int generate_plus_1_message(uint8_t *buf, 
    unsigned int buf_length )
{
    if(buf[0]==0xff)
    {
        buf[0] = 0;
        if(buf_length>1)
        {
            unsigned int index = 1;
            for(index=1;index < buf_length;index++)
            {

                if(buf[index]==0xff)
                {
                    buf[index] = 0;
                }
                else
                {
                    buf[index] += 1;
                    break;
                }
            }
            if(index < buf_length)
                return 1;
            else
                return 0;
        }
        else
            return 0;
    }
    else 
    {
        buf[0] +=1;
    }
    return 1;
}

uint32_t ShiftBytesByBits(uint8_t *src,
    uint32_t srclength, 
    uint8_t *dest, 
    uint32_t destlength, 
    uint8_t noofbitshift)
{
    uint32_t i =0;
    uint8_t left;
    uint8_t right;
    if( (src==NULL) ||
        (dest==NULL) ||
        (destlength<2) ||
        (srclength==0) )
    {
        return 0;
    }
    if(noofbitshift>=8)
        return i;
    
    for(i=0;i<srclength;i++)
    {
        left = src[i] >> noofbitshift;
        right = src[i] << (8-noofbitshift);
        dest[i]|=left;
        dest[i+1]|=right;
        if(destlength <= i+1)
        {
            return i;
        }
    }
    return i+1;
}

    /***dummy SMS message for extensive SMS tests***/
    /* valid long CDMA message */
    const uint8_t lsms_cdma_long_msg1[] = {
    2, 10, 0, 34, 0,
        187,0,
            2,4,
                0,0,0,0,0,
            1,177,0,
                0,0,173 ,0,
                    0, 0, 2, 16, 5, 2, 7, 2,
                    152, 97, 86, 5, 137, 128, 8, 157,
                    0, 3, 17, 26, 136, 1, 142, 77,
                    0, 40, 0, 29, 136, 24, 
    13, 70,
    65, 19, 66, 80, 106, 85, 73, 1,
    81, 209, 232, 61, 47, 61, 1, 132,
    66, 244, 246, 154, 8, 222, 161, 48,
    40, 84, 94, 63, 37, 151, 204, 98,
    117, 94, 191, 101, 183, 216, 108, 126,
    95, 63, 165, 215, 236, 118, 127, 90,
    10, 163, 163, 208, 114, 86, 122, 11,
    8, 133, 233, 229, 60, 25, 181, 228,
    104, 120, 36, 94, 75, 53, 164, 215,
    108, 54, 94, 11, 21, 144, 205, 107,
    53, 223, 11, 149, 132, 195, 98, 53,
    94, 203, 117, 192, 228, 68, 82, 118,
    122, 15, 73, 201, 64, 101, 20, 191,
    59, 160, 134, 120, 56, 80, 84, 62,
    37, 23, 140, 66, 97, 84, 190, 101,
    55, 152, 76, 106, 85, 62, 165, 87,
    172, 86, 107, 82, 8, 
    3, 6, 23, 6, 48, 16, 72, 81};

    const uint8_t lsms_cdma_long_msg2[] = {
     2, 11, 0, 34, 0,
     187,0,
        2,4,
            0,0,0,0,0,
        1,177,0,
            0, 0,173 ,0, 
                0, 0, 2, 16, 5, 2, 7, 2,
                 152, 97, 86, 5, 137, 128, 8, 157,
                 0, 3, 17, 26, 152, 1, 142, 77,
                 0, 40, 0, 29, 136, 24, 
     18, 1,
     142, 204, 98, 181, 158, 235, 129, 202,
     194, 193, 225, 52, 190, 107, 60, 44,
     20, 82, 118, 122, 15, 73, 201, 64,
     101, 20, 191, 59, 166, 128, 53, 170,
     76, 14, 23, 17, 141, 203, 97, 245,
     30, 159, 81, 173, 223, 107, 246, 31,
     31, 145, 205, 235, 117, 255, 31, 155,
     208, 128, 172, 236, 244, 30, 147, 154,
     128, 194, 33, 126, 127, 77, 0, 107,
     127, 24, 30, 11, 21, 144, 205, 107,
     53, 223, 11, 149, 132, 195, 98, 53,
     94, 203, 117, 192, 225, 97, 52, 222,
     139, 85, 176, 217, 112, 61, 21, 22,
     155, 154, 131, 208, 112, 84, 31, 71,
     41, 200, 238, 39, 156, 12, 22, 17,
     13, 139, 65, 225, 20, 158, 81, 45,
     159, 75, 226, 21, 24,
     3, 6, 23, 6, 48, 16, 72, 82
     };

    const uint8_t lsms_cdma_long_msg3[] = {
    2, 12, 0, 34, 0,
    88,0,
        2,4,
            0,0,0,0,0,
        1,78,0,
            0,0,74 ,0,
                0, 0, 2, 16, 5, 2, 7, 2,
            152, 97, 86, 5, 137, 128, 8, 58,
            0, 3, 17, 26, 168, 1, 40, 73,
            88, 40, 0, 29, 136, 24, 29, 34,
            155, 86, 171, 214, 43, 53, 169, 2,
            130, 97, 181, 30, 171, 97, 186, 224,
            112, 176, 180, 126, 75, 47, 152, 202,
            34, 40, 178, 10, 41, 56, 136, 
            3, 6, 23, 6, 48, 16, 72, 82, 10, 1, 64
    };
    const uint8_t sms_gsm_ivmsg[]={
      0x02,0x01,0x00,0x22,0x00,0x60,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x56
     ,0x00,0x01,0x06,0x52,0x00,0x07,0x91,0x58,0x92,0x10,0x00,0x34,0xf2,0x24,0x0b,0xd0
     ,0xb1,0x19,0xce,0xda,0x9c,0x02,0x41,0x00,0x81,0x80,0x92,0x31,0x40,0x22,0x23,0x3e
     ,0xd9,0x77,0x1d,0x84,0x0e,0xdb,0xcb,0xa0,0x30,0xc8,0x5d,0xbe,0x83,0xec,0xef,0xf4
     ,0xb8,0xdc,0x0e,0xa7,0xd9,0xa0,0x76,0x79,0x3e,0x0f,0x9f,0xcb,0x3b,0x10,0x9c,0x5d
     ,0x0e,0xcf,0xcb,0xa0,0x71,0x98,0xcd,0x06,0xc5,0x66,0x38,0x10,0xfd,0x0d,0x92,0x97
     ,0xe9,0xf2,0x74,0xd9,0x5e,0x76,0x01};

    const uint8_t sms_cdma_ivmsg[]= {
     0x02,0x91,0x00,0x22,0x00,0x5a,0x00,0x02,0x04,0x00,0x00,0x00,
     0x00,0x00,0x01,0x50,0x00,0x00,0x00,0x4c,0x00,0x00,
     0x00,0x02,
        0x10,0x02,
     0x02,0x08,
        0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
     0x08,0x26,
        0x00,0x03,
            0x10,0x00,0x00,
        0x01,0x19,
             0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
             0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
             0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
             0x60,
        0x05,0x01,
             0x0b,
        0x08,0x01,0x00,
     0x06,0x01,
        0x01,/*BearerReplySeq*/
     0x04,0x07,
         0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/
     0x01,0x02,
        0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
     0x03,0x03,
        0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };

    const uint8_t sms_cdma_ivmsg2[]={
     0x02,0x91,0x00,0x22,0x00,
     0x5a+21,0x00,/*Length +10*/
     0x02,0x04,0x00,0x00,0x00,
     0x00,0x00,0x01,
     0x50+21,0x00,
     0x00,
     0x00,
     0x4c+21,
     0x00,
     0x00,
     0x00,0x02,
        0x10,0x02,
     0x02,0x08,
        0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
     0x08,0x26+21,
        0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
            0x10,0x00,0x00,
        0x01,0x19,/*USER_DATA_SUB_IDENTIFIER*/
             0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
             0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
             0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
             0x60,
        0x05,0x01,/*REL_VAL_PERIOD_SUB_IDENTIFIER*/
             0x0b,
        0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
        0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
            0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
        0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
            0xaa,
        0x03,0x06,/*MSG_TIMESTAMP_SUB_INDETIFIER*/
            0x10,0x20,0x30,0x40,0x50,0x60,
     0x06,0x01,
        0x01,/*BearerReplySeq*/
     0x04,0x07,
         0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
     0x01,0x02,
        0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
     0x03,0x03,
        0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };

    const uint8_t sms_cdma_ivmsg3[]=
    {0x02,0x91,0x00,0x22,0x00,
     0x5a+21,0x00,/*Length +10 */ 
     0x02,0x04,0x00,0x00,0x00,
     0x00,0x00,0x01,
     0x50+21,0x00,
     0x00,
     0x00,
     0x4c+21,
     0x00,
     0x00,
     0x00,0x02,
        0x10,0x02,
     0x02,0x08,
        0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
     0x08,0x26+21,
        0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
            0x10,0x00,0x00,
        0x01,0x19,/*USER_DATA_SUB_IDENTIFIER
        encoding(x) 0:xxxx xyyy;
        MsgLen(y)   1:yyyy yzzz;
        Msg Content 2:zzzzz ...;*/
             0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
             0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
             0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
             0x60,
        0x05,0x01,/* REL_VAL_PERIOD_SUB_IDENTIFIER */
             0x0b,
        0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
        0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
            0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
        0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
            0xaa,
        0x03,0x06,/*MSG_TIMESTAMP_SUB_INDETIFIER*/
            0x10,0x20,0x30,0x40,0x50,0x60,
     0x06,0x01,
        0x01,/*BearerReplySeq*/
     0x04,0x07,
         0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
     0x01,0x02,
        0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
     0x03,0x03,
        0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };

    const uint8_t sms_cdma_ivmsg_double_usr_data_sub_id[]={
        0x02,0x91,0x00,0x22,0x00,
         0x5a+21,0x00,
         0x02,0x04,0x00,0x00,0x00,
         0x00,0x00,0x01,
         0x50+21,0x00,
         0x00,
         0x00,
         0x4c+21,
         0x00,
         0x00,
         0x00,0x02,
            0x10,0x02,
         0x02,0x08,
            0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
         0x08,0x26+21,
            0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
                0x10,0x00,0x00,
            0x01,0x08,/*USER_DATA_SUB_IDENTIFIER 8/0x19*/
                 0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
            0x01,0x0f,/*USER_DATA_SUB_IDENTIFIER 15/0x19*/
                 0xe8,0x29,0xe9,0xcb,0xcb,0x96,
                 0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
                 0x60,
            0x05,0x01,/*REL_VAL_PERIOD_SUB_IDENTIFIER*/
                 0x0b,
            0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
            0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
            0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
                0xaa,
            0x03,0x06,/*MSG_TIMESTAMP_SUB_INDETIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,
         0x06,0x01,
            0x01,/*BearerReplySeq*/
         0x04,0x07,
             0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
         0x01,0x02,
            0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
         0x03,0x03,
            0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };

    const uint8_t sms_cdma_ivmsg_double_dearer_data_id[]=
        {0x02,0x91,0x00,0x22,0x00,
         0x5a+21,0x00,/*Length*/
         0x02,0x04,0x00,0x00,0x00,
         0x00,0x00,0x01,
         0x50+21,0x00,
         0x00,
         0x00,
         0x4c+21,
         0x00,
         0x00,
         0x00,0x02,
            0x10,0x02,
         0x02,0x08,
            0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
         /****************************/
         0x08,0x26+21-3-8,
            0x00,0x03,/*MSG_SUB_IDENTIFIER_ID */
                0x10,0x00,0x00,
            0x01,0x19,/*USER_DATA_SUB_IDENTIFIER*/
                 0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
                 0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
                 0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
                 0x60,
            
            0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
            0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
            0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
                0xaa,        
         0x08,0x09,
            0x05,0x01,0x01,
            0x05,0x01,0x02,
            0x05,0x01,0x03,        
         /****************************/
         0x06,0x01,
            0x01,/*BearerReplySeq*/
         0x04,0x07,
             0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
         0x01,0x02,
            0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
         0x03,0x03,
            0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };

    const uint8_t sms_cdma_ivmsg_double_dest_addr_id[]=
        {0x02,0x91,0x00,0x22,0x00,
         0x5a+21,0x00,/*Length*/
         0x02,0x04,0x00,0x00,0x00,
         0x00,0x00,0x01,
         0x50+21,0x00,
         0x00,
         0x00,
         0x4c+21,
         0x00,
         0x00,
         0x00,0x02,
            0x10,0x02,
         0x02,0x08,
            0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
         0x08,0x26+21-3-8,
            0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
                0x10,0x00,0x00,
            0x01,0x19,/*USER_DATA_SUB_IDENTIFIER*/
                 0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
                 0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
                 0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
                 0x60,
            
            0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
            0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
            0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
                0xaa,        
         0x04,0x07,
                0x04,0x01,0x01,
                0x04,0x01,0x02,
                0x04,
         0x06,0x01,
            0x01,/*BearerReplySeq*/
         0x04,0x07,
             0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
         0x01,0x02,
            0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
         0x03,0x05,
            0x3a,0x2a,0x1a,/*ORIGSUBADDR_IDENTIFIER*/
            0x01,0x03
    };

    const uint8_t sms_cdma_ivmsg_double_orginal_addr_id[]={
         0x02,0x91,0x00,0x22,0x00,
         0x5a+21,0x00,
         0x02,0x04,0x00,0x00,0x00,
         0x00,0x00,0x01,
         0x50+21,0x00,
         0x00,
         0x00,
         0x4c+21,
         0x00,
         0x00,
         0x00,0x02,
            0x10,0x02,
         0x02,0x08,
            0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
         0x08,0x26+21-3-8,
            0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
                0x10,0x00,0x00,
            0x01,0x19,/*USER_DATA_SUB_IDENTIFIER*/
                 0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
                 0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
                 0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
                 0x60,
            
            0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
            0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
            0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
                0xaa,        
         0x02,0x07,
                0x04,0x01,0x01,
                0x04,0x01,0x02,
                0x04,
         0x06,0x01,
            0x01,/*BearerReplySeq*/
         0x02,0x07,
             0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
         0x01,0x02,
            0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
         0x03,0x05,
            0x3a,0x2a,0x1a,/*ORIGSUBADDR_IDENTIFIER*/
            0x01,0x03
    };
    const uint8_t sms_cdma_ivmsg_double_call_num_sub_id[]=
    {    0x02,0x91,0x00,0x22,0x00,
         0x5a+21,0x00,
         0x02,0x04,0x00,0x00,0x00,
         0x00,0x00,0x01,
         0x50+21,0x00,
         0x00,
         0x00,
         0x4c+21,
         0x00,
         0x00,
         0x00,0x02,
            0x10,0x02,
         0x02,0x08,
            0x03,0x24,0x61,0x09,0xca,0x96,0x1e,0x80,
         0x08,0x26+21,
            0x00,0x03,/*MSG_SUB_IDENTIFIER_ID*/
                0x10,0x00,0x00,
            0x01,0x19,/*USER_DATA_SUB_IDENTIFIER*/
                 0x10,0xd5,0x7c,0xbb,0x31,0xef,0xdb,0x95,
                 0x07,0x4d,0xe8,0x29,0xe9,0xcb,0xcb,0x96,
                 0x14,0x15,0xf4,0xf2,0xcb,0xb3,0x2f,0x3e,
                 0x60,
            0x05,0x01,/*REL_VAL_PERIOD_SUB_IDENTIFIER*/
                 0x0b,
            0x08,0x01,0x00,/*PRIORIY_SUB_IDENTIFIER*/
            0x0e,0x08,/*CALLBACK_NUM_SUB_IDENTIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
            0x0d,0x01,/*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
                0xaa,
            0x0e,0x06,/*MSG_TIMESTAMP_SUB_INDETIFIER*/
                0x10,0x20,0x30,0x40,0x50,0x60,
         0x06,0x01,
            0x01,/*BearerReplySeq*/
         0x04,0x07,
             0x03,0x96,0x1e,0x24,0x61,0x09,0xca,/*pDestAddrBuf*/ 
         0x01,0x02,
            0x1a,0x2a,/*SERVICE_CATEGORY_IDENIFIER*/
         0x03,0x03,
            0x3a,0x2a,0x1a/*ORIGSUBADDR_IDENTIFIER*/
    };
    const uint8_t ind_hdr[] =
    {
        0x00,0x03,0x01,0x02,0x03,
        0x08,0x04,0x01,0x02,0x03,0x04,
        0x02,0x02,0x01,0x02
    };

    const uint8_t ind_hdr_inv[] =
    {
        0x00,0x05,0x01,0x02,0x03,0x04,0x05,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };

    const uint8_t ind_hdr_invalid_length[] =
    {
        0x00,0x03,0x01,0x02,0x03,
        0x08,0x14,0x01,0x02,0x03,0x04,
        0x02,0x02,0x01,0x02
    };

void sms_helper_test()
{
    msgbuf buffer={0,{0}};
    int i = 0;
    uint16_t k = 0;
    uint8_t u8Temp;
    uint8_t u8Temp2;
    uint8_t bufShift[QMI_MSG_MAX];
    uint32_t lengthshift = 0;
    uint32_t u32i=0;

    DEBUG_STDOUT("\nStarting GSM/WCDMA test cases for sms decoding\n");

    /*****Type 0 format GSM*****/
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* Type 0 */
    DEBUG_STDOUT("Type:0\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;//Type 0
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* Type:0 sms_ExtractSMSDeliver invalid SMSC length */
    DEBUG_STDOUT("Type:0 sms_ExtractSMSDeliver invalid SMSC Length\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[21] = 0x15;/*sms_ExtractSMSDeliver invalid sc address Length */
    buffer.buf[29] = 0x24;/*Type 0 */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* Type:0 sms_ExtractSMSDeliver invalid source address length */
    DEBUG_STDOUT("Type:0 sms_ExtractSMSDeliver invalid source address Length\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;/*Type 0 */
    buffer.buf[30] = 0x21;/*sms_ExtractSMSDeliver invalid source address Length */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type:0 sms_ExtractSMSDeliver Length less than expected */
    DEBUG_STDOUT("Type:0 sms_ExtractSMSDeliver Length less than expected\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;/*Type 0 */
    buffer.buf[30] = 0x21;/*sms_ExtractSMSDeliver Length less than expected */
    buffer.type = 30 + 0x16;/*sizeof(sms_gsm_ivmsg) */
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type:0 sms_ExtractSMSDeliver Length greater than expected */
    DEBUG_STDOUT("Type:0 sms_ExtractSMSDeliver Length greater than expected\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;/*Type 0*/
    buffer.buf[30] = 161;/*sms_ExtractSMSDeliver Length greater than expected*/
    buffer.type = 30 + 161;/*sizeof(sms_gsm_ivmsg);*/
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 encodeing default */
    DEBUG_STDOUT("Type:0 Default encoding\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[39] = 0x00;/* Type 0 encodeing default */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 encodeing EIGHTBITENCODING */
    DEBUG_STDOUT("Type:0 EIGHTBITENCODING\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[39] = 0x04;/*Type 0 EIGHTBITENCODING*/
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 encodeing UCSENCODING */
    DEBUG_STDOUT("Type:0 UCSENCODING\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[39] = 0x08;/* Type 0 UCSENCODING */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 encodeing unknown */
    DEBUG_STDOUT("Type:0 unknown encoding\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[39] = 0x0F;/* Type 0 unknown encoding */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 Compressed data not supported */
    DEBUG_STDOUT("Type:0 Compressed data\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[39] = 0x28;/* Compressed data with encoding */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 0 PDU Length Invalid */
    DEBUG_STDOUT("Type 0:PDU Lenth invalid\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x24;
    buffer.buf[47] = 0xb9;/*IType 0 invalid PDU Lenth VALID_PDU_MESSAGE_SIZE*/
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    /***********Type 0 format GSM********/

    /* Type 1 format GSM*/
    /* Type 1*/
    DEBUG_STDOUT("Type 1:PDU Lenth source address \n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = 0xF0;/* invalid source address length */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Unknown PDU type*/
    DEBUG_STDOUT("Type unknown: Unknown PDU Type\n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x23;
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 1 encodeing default*/
    DEBUG_STDOUT("Type 1: encoding default \n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = MAX_SMSC_LENGTH-2;/* valid source address length*/
    buffer.buf[45] = 0x00;/*IType 0 encodeing default*/
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 1 encodeing EIGHTBITENCODING*/
    DEBUG_STDOUT("Type 1: encoding EIGHTBITENCODING \n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = MAX_SMSC_LENGTH-2;/*valid source address length*/
    buffer.buf[45] = 0x04;/*Type 1 encodeing EIGHTBITENCODING*/
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 1 encodeing UCSENCODING*/
    DEBUG_STDOUT("Type 1: encoding UCSENCODING \n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = MAX_SMSC_LENGTH-2;/* valid source address length */
    buffer.buf[45] = 0x08;/* Type 1 encodeing UCSENCODING */
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 1 encodeing UCSENCODING */
    DEBUG_STDOUT("Type 1: encoding not supported \n");
    concatenated_sms_counter = 0;
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = MAX_SMSC_LENGTH-2;/* valid source address length */
    buffer.buf[45] = 0x09;
    buffer.type = sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* Type 1 encodeing UCSENCODING */
    DEBUG_STDOUT("Type 1: encoding UCSENCODING \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_gsm_ivmsg,sizeof(sms_gsm_ivmsg));
    buffer.buf[29] = 0x25;
    buffer.buf[31] = MAX_SMSC_LENGTH-2;
    buffer.buf[45] = 0x08;/* Type 1 encoding UCSENCODING*/
    buffer.buf[47] = 0xb9;/* IType 0 invalid PDU Lenth VALID_PDU_MESSAGE_SIZE */
    buffer.type =  sizeof(sms_gsm_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    /***********Type 1 format GSM****************/

    /************CDMA test case***********/
    DEBUG_STDOUT("\nStarting CDMA test cases for sms decoding\n");

    /* cdma Invaild TELESERVICE_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild TELESERVICE_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[23] = 0; /*Invaild TELESERVICE_HDR_LEN*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild ORIGADDR_IDENTIFIER_HDR_LENTH */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_REPLY_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[27] = 0xa1; /*Invaild BEARER_REPLY_HDR_LEN*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild ORIGADDR_IDENTIFIER_HDR_LENTH */
    DEBUG_STDOUT("CDMA:\nInvaild ORIGADDR_IDENTIFIER_HDR_LENTH zero\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[27] = 0; /* Invaild BEARER_REPLY_HDR_LEN zero */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild BEARER_DATA_IDENTIFIER_HDR_LENGTH */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_DATA_IDENTIFIER_HDR_LENGTH zero\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[37] = 0; /* Invaild BEARER_DATA_IDENTIFIER_HDR_LENGTH zero */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild BEARER_DATA_IDENTIFIER_HDR_LENGTH */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_DATA_IDENTIFIER_HDR_LENGTH \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[37] = 0xa1; /* Invaild BEARER_DATA_IDENTIFIER_HDR_LENGTH */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild ORIGSUBADDR_IDENTIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild ORIGSUBADDR_IDENTIFIER \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[93] = 0xa1; /* Invaild ORIGSUBADDR_IDENTIFIER */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild SERVICE_CATEGORY_IDENIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild SERVICE_CATEGORY_IDENIFIER \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[89] = 0xa1; /* Invaild SERVICE_CATEGORY_IDENIFIER */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild DESTADDR_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild DESTADDR_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[80] = 0xa1; /* Invaild SERVICE_CATEGORY_IDENIFIER */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild DESTADDR_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild DESTADDR_HDR_LEN zero \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[80] = 0; /* Invaild DESTADDR_HDR_LEN */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild BEARER_REPLY_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_REPLY_HDR_LEN zero \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[77] = 0; /* Invaild BEARER_REPLY_HDR_LEN */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild BEARER_INV_SEQ_VALUE */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_INV_SEQ_VALUE \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[78] = 0xFF; /* Invaild BEARER_REPLY_HDR_LEN */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* cdma Invaild PDU IDENTIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild Invaild PDU IDENTIFIER 0xff \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[76] = 0xff; /* Invaild PDU IDENTIFIER */ 
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* cdma origin Address mode DigitMode */
    DEBUG_STDOUT("CDMA:\nInvaild origin Address mode DigitMode \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[26] = 0x02; /* Invaild ORIGADDR_IDENTIFIER */
    buffer.buf[28] = 0x80;/* DigitMode */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma origin Address mode NumMode */
    DEBUG_STDOUT("CDMA:\nInvaild origin Address mode NumMode \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[26] = 0x02; /* Invaild ORIGADDR_IDENTIFIER */
    buffer.buf[28] = 0x40;/* NumMode */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma origin Address mode Not NumMode and DigitMode */
    DEBUG_STDOUT("CDMA:\n origin Address Not DigitMode/NumMode \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[28] &= 0x3F;/* !(NumMode|DigitMode) */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* cdma invalid OriginAddr addr length */
    DEBUG_STDOUT("CDMA:\nInvaild sms_DecodeOriginAddr addr length \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[26] = 0x02; 
    buffer.buf[28] = 0x0F;/*sms_DecodeOriginAddr addr length */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma no ORIGADDR_IDENTIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild BEARER_REPLY_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[26] = 0x03; /* Skip no ORIGADDR_IDENTIFIER */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Dest Address mode DigitMode */
    DEBUG_STDOUT("CDMA:\nInvaild Dest Address mode DigitMode \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[26] = 0x03; /* ORIGSUBADDR_IDENTIFIER */
    buffer.buf[81] = 0x80; /* DigitMode */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Dest Address mode DigitMode */
    DEBUG_STDOUT("CDMA:\nInvaild Dest Address mode NumMode \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[26] = 0x03; /* ORIGSUBADDR_IDENTIFIER */
    buffer.buf[81] = 0x40; /* NumMode */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild Dest Address length */
    DEBUG_STDOUT("CDMA:\nInvaild Dest Address length \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[26] = 0x03; /* ORIGSUBADDR_IDENTIFIER */
    buffer.buf[81] = 0x0f; /* Invaild Dest Address length */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Invaild no ORIGADDR_IDENTIFIER and DESTADDR_IDENTIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild No Org & Dest identifer \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[26] = 0x03; /*ORIGSUBADDR_IDENTIFIER*/
    buffer.buf[79] = 0x03; /*no DESTADDR_IDENTIFIER*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma no BEARER_DATA_IDENTIFIER*/
    DEBUG_STDOUT("CDMA:\nInvaild No BEARER_DATA_IDENTIFIER \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[36] = 0x03; /*ORIGSUBADDR_IDENTIFIER*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma MSG_SUB_IDENTIFIER_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild MSG_SUB_IDENTIFIER_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[39] = 0x0F; /* no BEARER_DATA_IDENTIFIER */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma sub MsgType(s) */
    for(i=0;i<16;i++)
    {
        u8Temp = (uint8_t)i<<4;
        switch(i)
        {
            case 1:
            case 4:
            case 5:
            case 6:
                DEBUG_STDOUT("CDMA:\nMsgType (0x%02x) 0x%02x\n",i,u8Temp);
                break;
            default:
                DEBUG_STDOUT("CDMA:\nInvaild No MsgType (0x%02x) 0x%02x\n",i,u8Temp);
                break;
        }
        concatenated_sms_counter = 0;
        memset(buffer.buf,0,QMI_MSG_MAX);
        memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
        buffer.buf[40] = u8Temp; /* no BEARER_DATA_IDENTIFIER */
        buffer.type = sizeof(sms_cdma_ivmsg);
        sms_test_encode_decode_dummy_unpack(buffer);
    }
    
    /* cdma PRIORIY_SUB_IDENTIFIER_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild PRIORIY_SUB_IDENTIFIER_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[74] = 0x0F; /* Invaild PRIORIY_SUB_IDENTIFIER_HDR_LEN */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma USER_DATA_SUB_IDENTIFIER Data Length */
    DEBUG_STDOUT("CDMA:\nInvaild USER_DATA_SUB_IDENTIFIER Data Length \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[44] = 0xFF; /* Invaild USER_DATA_SUB_IDENTIFIER Data Length */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma MSG_TIMESTAMP_SUB_INDETIFIER_HDR_LEN Data Length */
    DEBUG_STDOUT("CDMA:\nInvaild MSG_TIMESTAMP_SUB_INDETIFIER_HDR_LEN Data Length \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    i = 43;
    buffer.buf[i++] = 0x03; /* Change to TimeStemp */
    buffer.buf[i++] = 0x08; /* Change to TimeStemp */
    buffer.buf[i++] = 0x01; /* Change to TimeStemp */
    buffer.buf[i++] = 0x02; /* Change to TimeStemp */
    buffer.buf[i++] = 0x03; /* Change to TimeStemp */
    buffer.buf[i++] = 0x04; /* Change to TimeStemp */
    buffer.buf[i++] = 0x05; /* Change to TimeStemp */
    buffer.buf[i++] = 0x06; /* Change to TimeStemp */
    buffer.buf[i++] = 0x07; /* Change to TimeStemp */

    buffer.buf[i++] = 0x05; /* skip */
    buffer.buf[i++] = 0x10; /* Change to TimeStemp */
    
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* cdma LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN */
    DEBUG_STDOUT("CDMA:\nInvaild LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[73] = 13; /*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
    buffer.buf[74] = 0x0F; /*Invaild LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma CALLBACK_NUM_SUB_IDENTIFIER Data Length */
    DEBUG_STDOUT("CDMA:\nInvaild CALLBACK_NUM_SUB_IDENTIFIER Data Length \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[43] = 14; /*CALLBACK_NUM_SUB_IDENTIFIER*/
    buffer.buf[44] = 0xFF; /*Invaild USER_DATA_SUB_IDENTIFIER Data Length*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma SUB INDETIFIER SKIP LENGTH */
    DEBUG_STDOUT("CDMA:\nInvaild SUB INDETIFIER SKIP LENGTH \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));
    buffer.buf[70] = 5; /*LANGUAGE_INDICATOR_SUB_INDETIFIER*/
    buffer.buf[71] = 0xFF; /*Invaild LANGUAGE_INDICATOR_SUB_INDETIFIER_HDR_LEN*/
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma UNKNOWN SUB IDENTIFIER */
    DEBUG_STDOUT("CDMA:\nInvaild UNKNOWN SUB IDENTIFIER \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg,sizeof(sms_cdma_ivmsg));    
    buffer.buf[43] = 0xff; /* UNKNOWN SUB IDENTIFIER */
    buffer.type = sizeof(sms_cdma_ivmsg);
    sms_test_encode_decode_dummy_unpack(buffer);

    /* cdma Teleservice 0x1002 encoding */
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1002 encoding \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x02;
    buffer.buf[45]=0xF0;
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    /* cdma Teleservice 0x1002 encoding */
    for(i=0;i<6;i++)
    {
        u8Temp2 = 0xff;
        switch(i)
        {
            case 1:
                u8Temp2 = 0x00;
                u8Temp = 0x10; /* USER_DATA_MSG_ENCODING_7BIT_ASCII */
                break;
            case 4:
                u8Temp = 0x10; /* USER_DATA_MSG_ENCODING_7BIT_ASCII */
                break;
            case 2:
                u8Temp2 = 0x00;
                u8Temp = 0x40; /* USER_DATA_MSG_ENCODING_8BIT_LATIN */
                break;
            case 5:
                u8Temp = 0x40; /* USER_DATA_MSG_ENCODING_8BIT_LATIN */
                break;
            case 0:
                u8Temp2 = 0x00;
                u8Temp = 0x00; /* USER_DATA_MSG_ENCODING_8BIT_OCTET */
                break;
            case 3:
                u8Temp = 0x00; /* USER_DATA_MSG_ENCODING_8BIT_OCTET */
                break;
            default:
                u8Temp = 0x00; /* USER_DATA_MSG_ENCODING_8BIT_OCTET */
                break;
        }
        if(i<3)
        {
            DEBUG_STDOUT("CDMA:\n Teleservice Msg Length \n");
        }
        else
        {
            DEBUG_STDOUT("CDMA:\nInvaild Teleservice Msg Length \n");
        }
        concatenated_sms_counter = 0;
        memset(buffer.buf,0,QMI_MSG_MAX);
        memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
        buffer.buf[24]=0x10;
        buffer.buf[25]=0x02;
        buffer.buf[45]=u8Temp;/* encoding */ 
        buffer.buf[46]=u8Temp2;/* invalid size */
        buffer.buf[47]=u8Temp2;/* invalid size */
        buffer.type = sizeof(sms_cdma_ivmsg2);
        sms_test_encode_decode_dummy_unpack(buffer);
    }

    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[45]=0x0F;/* USER_DATA_MSG_ENCODING_8BIT_OCTET */ 
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    DEBUG_STDOUT("CDMA:\n Teleservice 0x1005 encoding zero data length\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */
    buffer.buf[46]=0x00;/* zero size */
    buffer.buf[47]=0x00;/* zero size */
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding invalid size \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.buf[46]=0xFF;/* invalid size */
    buffer.buf[47]=0xFF;/* invalid size */
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd header size too large\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */
    buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    DEBUG_STDOUT("CDMA:\nInvaild double call num sub \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg_double_call_num_sub_id,sizeof(sms_cdma_ivmsg_double_call_num_sub_id));
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0x00;/* HdrInd = 0 */ 
    k = 45;
    buffer.buf[k++]=0x48;/* 0x48; USER_DATA_MSG_ENCODING_7BIT_GSM */
    buffer.type = sizeof(sms_cdma_ivmsg_double_call_num_sub_id);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nInvaild double org addr data \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg_double_orginal_addr_id,sizeof(sms_cdma_ivmsg_double_orginal_addr_id));
    buffer.type = sizeof(sms_cdma_ivmsg_double_orginal_addr_id);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    
    DEBUG_STDOUT("CDMA:\nInvaild double dest addr data \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg_double_dest_addr_id,sizeof(sms_cdma_ivmsg_double_dest_addr_id));
    buffer.type = sizeof(sms_cdma_ivmsg_double_dest_addr_id);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nInvaild double dearer data \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg_double_dearer_data_id,sizeof(sms_cdma_ivmsg_double_dearer_data_id));
    buffer.type = sizeof(sms_cdma_ivmsg_double_dearer_data_id);
    sms_test_encode_decode_dummy_unpack(buffer);

    
    DEBUG_STDOUT("CDMA:\nInvaild double user data \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg_double_usr_data_sub_id,sizeof(sms_cdma_ivmsg_double_usr_data_sub_id));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0x00;/* HdrInd = 0 */ 
    k = 45;
    buffer.buf[k++]=0x48;/* 0x48, USER_DATA_MSG_ENCODING_7BIT_GSM */ 

    buffer.type = sizeof(sms_cdma_ivmsg_double_usr_data_sub_id);
    sms_test_encode_decode_dummy_unpack(buffer);
        
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd User Data length small\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.buf[46]=0x08;/* header size too small */
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);

    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd header size too small\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nTeleservice 0x1005 msg body 0\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg3,sizeof(sms_cdma_ivmsg3));
    buffer.buf[46] &= 0x07;
    buffer.type = sizeof(sms_cdma_ivmsg3);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 ElementIdLen sms_DecodeUserDataHdr\n");
    memset(bufShift,0,QMI_MSG_MAX);
    lengthshift = ShiftBytesByBits((uint8_t*)&ind_hdr_invalid_length[0],
        sizeof(ind_hdr_invalid_length),
        bufShift,
        QMI_MSG_MAX,
        5);
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    k = 45;
    buffer.buf[k++]=0x48;/* 0x48;// USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.buf[k++]=0x98;
    buffer.buf[k++]=0x88 + 0x00; // last 2 bits
    for(u32i = 1;u32i<lengthshift;u32i++)
    {
        buffer.buf[k++] = bufShift[u32i];
    }
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd header size too small\n");
    memset(bufShift,0,QMI_MSG_MAX);
    lengthshift = ShiftBytesByBits((uint8_t*)&ind_hdr_inv[0],
        sizeof(ind_hdr_inv),
        bufShift,
        QMI_MSG_MAX,
        5);
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    k = 45;
    buffer.buf[k++]=0x48;
    buffer.buf[k++]=0x98;
    buffer.buf[k++]=0x88 + 0x00; /* last 2 bits */
    for(u32i = 1;u32i<lengthshift;u32i++)
    {
        buffer.buf[k++] = bufShift[u32i];
    }
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);   
    
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd header size too small\n");
    memset(bufShift,0,QMI_MSG_MAX);
    lengthshift = ShiftBytesByBits((uint8_t*)&ind_hdr[0],
        sizeof(ind_hdr),
        bufShift,
        QMI_MSG_MAX,
        5);
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    k = 45;
    buffer.buf[k++]=0x48;
    buffer.buf[k++]=0x98;
    buffer.buf[k++]=0x88 + 0x00; // last 2 bits
    for(u32i = 1;u32i<lengthshift;u32i++)
    {
        buffer.buf[k++] = bufShift[u32i];
    }
    
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd \n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */
    k = 45;
    buffer.buf[k++]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.buf[k++]=0x98;
    buffer.buf[k++]=0x88 + 0x00; /* last 2 bits */
    buffer.buf[k++]=0x08; /* 0x01,num */
    buffer.buf[k++]=0x10; /* 0x02,size */ 
    buffer.buf[k++]=0x00;
    buffer.buf[k++]=0x00;
    buffer.buf[k++]=0x10; /* 0x02,num */
    buffer.buf[k++]=0x08; /* 0x01,size */
    buffer.buf[k++]=0x00;
    buffer.buf[k++]=0x18;/* 0x03,num */
    buffer.buf[k++]=0x10;/* 0x02,size */
    buffer.buf[k++]=0x00;/* 3data:14-8 */
    buffer.buf[k++]=0x08;/* 3data:6 4num:7-6 */
    buffer.buf[k++]=0x20;/* 0x04, num 4num:5-1, 4size:7-6: */
    buffer.buf[k++]=0x10;/* 0x02,size 4size:5-1: 4data:14-13: */
    buffer.buf[k++]=0x00;/* 4data:11-4: */
    buffer.buf[k++]=0x00;/* 4data:3-0: */
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);


    DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
    buffer.buf[24]=0x10;
    buffer.buf[25]=0x05;
    buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
    k = 45;
    buffer.buf[k++]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
    buffer.buf[k++]=0x98;
    buffer.buf[k++]=0x88 + 0x00; /* last 2 bits */
    buffer.buf[k++]=0x00; /* 1 num */
    buffer.buf[k++]=0x18; /* 1_3,size:7-3 */
    buffer.buf[k++]=0x00; 
    buffer.buf[k++]=0x00; 
    buffer.buf[k++]=0x12; 
    buffer.buf[k++]=0x00; 
    buffer.buf[k++]=0x00;
    buffer.buf[k++]=0x40;/* 8 num:5-1, 8_4size:7-5 */
    buffer.buf[k++]=0x20;/* 8_3size:4-1 data: 28 - 3 */
    buffer.buf[k++]=0x00;/* data: 25 - 8 */
    buffer.buf[k++]=0x08;/* data: 17 - 8 */
    buffer.buf[k++]=0x20;/* data: 9 - 8 */
    buffer.buf[k++]=0x10;/* data: 1, num 2 */ 
    buffer.buf[k++]=0x80;/* size 2 */
    buffer.buf[k++]=0x10; 
    buffer.type = sizeof(sms_cdma_ivmsg2);
    sms_test_encode_decode_dummy_unpack(buffer);
    
    u8Temp = 0;
    do
    {
        DEBUG_STDOUT("CDMA:\nInvaild Teleservice 0x1005 encoding pHdrInd header size too small\n");
        DEBUG_STDOUT("u8Temp:0x%02x\n",u8Temp);
        /*concatenated_sms_counter = 0; */
        memset(buffer.buf,0,QMI_MSG_MAX);
        memcpy(buffer.buf,sms_cdma_ivmsg2,sizeof(sms_cdma_ivmsg2));    
        buffer.buf[24]=0x10;
        buffer.buf[25]=0x05;
        buffer.buf[42]=0xFF;/* HdrInd = 1 */ 
        buffer.buf[45]=0x48;/* USER_DATA_MSG_ENCODING_7BIT_GSM */ 
        buffer.buf[46]=0x90; 
        buffer.buf[47]=0x40; 
        buffer.buf[48]=u8Temp; 
        buffer.type = sizeof(sms_cdma_ivmsg2);
        sms_test_encode_decode_dummy_unpack(buffer);
    }while (++u8Temp!=0);
    
    DEBUG_STDOUT("CDMA:\n Teleservice 0x1005 msg body length(0x0000-0xFFFF)\n");
    concatenated_sms_counter = 0;
    memset(buffer.buf,0,QMI_MSG_MAX);
    memcpy(buffer.buf,sms_cdma_ivmsg3,sizeof(sms_cdma_ivmsg3));
    buffer.type = sizeof(sms_cdma_ivmsg3);
    buffer.buf[45] = 0x00;
    buffer.buf[46] = 0x00;
    while(generate_plus_1_message(&buffer.buf[45],2)==1)
    {
        sms_test_encode_decode_dummy_unpack(buffer);
    }
    
    DEBUG_STDOUT("Different size of lsms_cdma_long_msg1\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,lsms_cdma_long_msg1,sizeof(lsms_cdma_long_msg1));
    buffer.type = sizeof(lsms_cdma_long_msg1);
    for(i=187;i>0;i--)
    {
        buffer.buf[15] = i+20;
        buffer.buf[19] = i+16;
        sms_test_encode_decode_dummy_unpack(buffer);
    }

    DEBUG_STDOUT("Different size of lsms_cdma_long_msg2\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,lsms_cdma_long_msg2,sizeof(lsms_cdma_long_msg2));
    buffer.type = sizeof(lsms_cdma_long_msg2);
    for(i=187;i>0;i--)
    {
        buffer.buf[15] = i+20;
        buffer.buf[19] = i+16;
        sms_test_encode_decode_dummy_unpack(buffer);
    }

    DEBUG_STDOUT("Different size of lsms_cdma_long_msg3\n");
    memset(buffer.buf,0,QMI_MSG_MAX);
    buffer.type=0;
    memcpy(buffer.buf,lsms_cdma_long_msg3,sizeof(lsms_cdma_long_msg3));
    buffer.type = sizeof(lsms_cdma_long_msg3);
    for(i=58;i>0;i--)
    {
        memset(buffer.buf,0,QMI_MSG_MAX);
        buffer.type=0;
        memcpy(buffer.buf,lsms_cdma_long_msg3,sizeof(lsms_cdma_long_msg3));
        buffer.type = sizeof(lsms_cdma_long_msg3);
        buffer.buf[15] = i+20;
        buffer.buf[19] = i+16;
        sms_test_encode_decode_dummy_unpack(buffer);
    }    

    #if 0 /* generate 0-QMI_MSG_MAX-17 bytes string */
    memcpy(buffer.buf,lsms_cdma_long_msg3,sizeof(lsms_cdma_long_msg3));
    struct timespec tstart={0,0}, tend={0,0};
    for(k=1;k<QMI_MSG_MAX-17;k++)
    {
        uint16_t length = 0;
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        DEBUG_STDOUT("==%d/%d.==\n",k,QMI_MSG_MAX);
        memset(&buffer.buf[17],0,k);        
        buffer.type = length;
        length = k;
        buffer.buf[16] = (uint8_t)(length >> 8);//(uint8_t) (length/256);//15,16//((length>>8) & 0xFF)
        buffer.buf[15] = (uint8_t) (length & 0xFF);//15,16
        length += 10;
        buffer.buf[6] = (uint8_t)(length >> 8);//15,16
        buffer.buf[5] = (uint8_t) (length & 0xFF);//15,16
        while(generate_plus_1_message(&buffer.buf[17],k)==1)
        {
            sms_test_encode_decode_dummy_unpack(buffer);
        }
        clock_gettime(CLOCK_MONOTONIC, &tend);
        DEBUG_STDOUT("==%d/%d.==%.5f\n",k,QMI_MSG_MAX,
            ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
    }
    #endif
    
}

swi_uint256_t unpack_sms_SLQSGetSMSParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_sms_SLQSGetSMSParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_sms_SLQSGetSMSListParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_sms_SLQSGetSMSListParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_sms_SLQSModifySMSStatusParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_sms_SLQSModifySMSStatusParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_sms_SLQSDeleteSMSParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_sms_SLQSDeleteSMSParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_sms_SendSMSParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)
}};

swi_uint256_t unpack_sms_SendSMSParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_sms_SetNewSMSCallback_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_7_BITS,16,17,18,19,20,21,22)
}};

swi_uint256_t unpack_sms_SetNewSMSCallbackParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_sms_SetNewSMSCallbackParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

void *sms_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    unpack_sms_SLQSGetSMSList_t smslist;
    unpack_sms_SendSMS_t sendsms;
    unpack_sms_SLQSGetSMS_t getrawsms;
    unpack_sms_SLQSWmsMemoryFullCallBack_ind_t full_stat;
    unpack_sms_SetNewSMSCallback_ind_t newsmscbind;
    unpack_sms_SLQSTransLayerInfoCallback_ind_t transLayerInfo;
    unpack_sms_SLQSNWRegInfoCallback_ind_t nwRegInfo;
    unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t msgWait;
    unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t rawAsync;
    unpack_sms_SLQSModifySMSStatus_t ModifySMSStatus;
    unpack_sms_SLQSDeleteSMS_t DeleteSMS;
    unpack_sms_SLQSSetIndicationRegister_t SLQSSetIndicationRegister;
    msgbuf msg;
    int rtn;
    ssize_t rlen;
    int Length = 0;
    int lIndex = 0;
    pack_qmi_t req_ctx;

    memset(&req_ctx, 0, sizeof(req_ctx));

    printf("%s param %p\n", __func__, ptr);
    sleep(1);
    while(enSmsThread)
    {
        memset(&msg, 0, sizeof(msg));

        /* TODO select multiple file and read them */
        rtn = read(sms, msg.buf, QMI_MSG_MAX);
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSMS, msg.buf, rlen, &rsp_ctx);

            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("SMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            uint8_t count;

            switch(rsp_ctx.msgid)
            {
                case eQMI_WMS_SET_IND_REGISTER:
                    printf("eQMI_WMS_SET_IND_REGISTER ...\n");
                    unpackRetCode = unpack_sms_SLQSSetIndicationRegister( msg.buf, rlen,&SLQSSetIndicationRegister);
                    printf("[%s][line:%d]rtn: %d\n", __func__, __LINE__, unpackRetCode);

#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSSetIndicationRegister",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");
                    }
#endif
                break;



                case eQMI_WMS_RAW_READ:
                    printf("get sms raw read ...\n");
                    getrawsms.messageSize = sizeof(getrawsms.message);
                    unpackRetCode = unpack_sms_SLQSGetSMS( msg.buf, rlen,&getrawsms);
                    printf("[%s][line:%d]rtn: %d\n", __func__, __LINE__, unpackRetCode);
                    swi_uint256_print_mask (getrawsms.ParamPresenceMask);
                    
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSGetSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);

                    CHECK_WHITELIST_MASK(
                        unpack_sms_SLQSGetSMSParamPresenceMaskWhiteList,
                        getrawsms.ParamPresenceMask);
                    CHECK_MANDATORYLIST_MASK(
                        unpack_sms_SLQSGetSMSParamPresenceMaskMandatoryList,
                        getrawsms.ParamPresenceMask);

                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,", SUCCESS_MSG);
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n", SUCCESS_MSG);
                    }
#endif                    
                    if ((!unpackRetCode && MESSAGE_FORMAT_GW_PP == getrawsms.messageFormat) &&/*UMTS-1*/
                        (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
                    {
                        char  ascii[2048]= {0};
                        uint8_t  SenderAddrLength = MAX_SMSC_LENGTH;
                        uint8_t TextMsgLength = MAX_SMS_MSG_LEN;
                        uint8_t  SMSCAddrLength   = MAX_SMSC_LENGTH;
                        char  SenderAddr[MAX_SMSC_LENGTH+1]   = {'\0'};
                        char  SMSCAddr[MAX_SMSC_LENGTH+1]     = {'\0'};
                        uint8_t  Refnum[5] = {'\0'};
                        uint8_t  Totalnum[5] = {'\0'};
                        uint8_t  Seqnum[5] = {'\0'};
                        int8_t  IsUDHPresent;
                        uint8_t encoding = 0xFF;
                        uint8_t  i = 0;
                        sms_SLQSWCDMADecodeLongTextMsg_t wcdmaLongMsgDecodingParams;
                        memset(&wcdmaLongMsgDecodingParams,0,sizeof(wcdmaLongMsgDecodingParams));

                        wcdmaLongMsgDecodingParams.pMessage          = getrawsms.message;
                        wcdmaLongMsgDecodingParams.MessageLen        = getrawsms.messageSize;
                        wcdmaLongMsgDecodingParams.pSenderAddrLength = &SenderAddrLength;
                        wcdmaLongMsgDecodingParams.pTextMsgLength    = &TextMsgLength;
                        wcdmaLongMsgDecodingParams.pScAddrLength     = &SMSCAddrLength;
                        wcdmaLongMsgDecodingParams.pSenderAddr = SenderAddr;
                        wcdmaLongMsgDecodingParams.pTextMsg    = ascii;
                        wcdmaLongMsgDecodingParams.pScAddr     = SMSCAddr;
                        wcdmaLongMsgDecodingParams.pReferenceNum = Refnum;
                        wcdmaLongMsgDecodingParams.pTotalNum   = Totalnum;
                        wcdmaLongMsgDecodingParams.pPartNum    = Seqnum;
                        wcdmaLongMsgDecodingParams.pIsUDHPresent = &IsUDHPresent;

                        unpackRetCode = sms_SLQSWCDMADecodeLongTextMsg( &wcdmaLongMsgDecodingParams );

#if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "sms_SLQSWCDMADecodeLongTextMsg",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                            checkErrorAndFormErrorString(msg.buf);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,",  SUCCESS_MSG);
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  SUCCESS_MSG);
                        }
#endif

                        if( eQCWWAN_ERR_NONE != unpackRetCode )
                        {
                            fprintf( stderr,"Failed to decode SMS\n");
                            fprintf( stderr,"Failure Code: %d\n", unpackRetCode );
                            break;
                        }
                        if ( *wcdmaLongMsgDecodingParams.pIsUDHPresent)
                        {
                            concatenated_sms_counter++;
                            if (!buf_allocated && (wcdmaLongMsgDecodingParams.pTotalNum!= NULL))
                            {
                                for ( i = 0; i < *wcdmaLongMsgDecodingParams.pTotalNum; i++)
                                {
                                    if (wcdmaLongMsgDecodingParams.pTextMsgLength == NULL)
                                    {
                                        printf("invalid message length pointer!\n");
                                        return NULL;
                                    }
                                    else
                                    {
                                        if (*wcdmaLongMsgDecodingParams.pTextMsgLength <= 0)
                                        {
                                            printf("invalid message size!\n");
                                            return NULL;
                                        }
                                    }
                                    copy_buf[i] = malloc( (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char) );
                                    if (copy_buf[i] == NULL) {
                                        printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                        return NULL;
                                    }  
                                    memset(copy_buf[i], 0, (*wcdmaLongMsgDecodingParams.pTextMsgLength)*sizeof(char));
                                    buf_allocated = TRUE;
                                }
                            }
                            if(wcdmaLongMsgDecodingParams.pPartNum != NULL)
                            {
                                if (*wcdmaLongMsgDecodingParams.pPartNum > 0)
                                {
                                    if(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]!=NULL)
                                    {
                                        uint16_t userdataLen = 0;
                                        unpackRetCode = sms_SLQSWCDMADecodeMTEncoding(wcdmaLongMsgDecodingParams.pMessage,
                                                          wcdmaLongMsgDecodingParams.MessageLen,
                                                          &encoding,
                                                          &userdataLen);

                                         if ((!unpackRetCode) && (encoding == UCSENCODING))
                                         {

                                             unpackRetCode = smsdemo_processucs2msg((char*)wcdmaLongMsgDecodingParams.pTextMsg,
                                                                    userdataLen,
                                                                    copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1]);

                                             if (unpackRetCode == eQCWWAN_ERR_MEMORY)
                                                 return NULL;
                                         }
                                         else
                                         {
                                             strcpy(copy_buf[*wcdmaLongMsgDecodingParams.pPartNum-1], wcdmaLongMsgDecodingParams.pTextMsg);
                                         }
                                    }
                                }
                            }
                            if(wcdmaLongMsgDecodingParams.pTotalNum!=NULL)
                            {
                                if (concatenated_sms_counter == (*wcdmaLongMsgDecodingParams.pTotalNum))
                                {
                                    display_long_sms(&wcdmaLongMsgDecodingParams);
                                    enSmsThread = 0;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            printf("SLQS Get SMS Successful\n");
                            printf("Message Tag: %d\n",getrawsms.messageTag);
                            printf("Message Format: %d\n",getrawsms.messageFormat);
                            printf("Message Size: %d\n",getrawsms.messageSize);
                            printf("Message: \n");
                            for ( count=0;count < getrawsms.messageSize;count++ )
                            {
                                printf("%02X ",(getrawsms.message[count]));
                                if (count % 16 == 15)
                                    printf("\n");
                            }
                            printf("\n");

                            /* Decode as GSM/WCDMA PP */
                            wcdma_decode_text_msg (getrawsms);
                        }
                    }
                    else if ((!unpackRetCode && MESSAGE_FORMAT_CDMA == getrawsms.messageFormat) &&/* CDMA */
                            (swi_uint256_get_bit (getrawsms.ParamPresenceMask, 1)))
                    {
                        char     ascii[2048]= {0};
                        uint8_t  SenderAddrLength = 16;
                        uint8_t  TextMsgLength    = MAX_SMS_MSG_LEN;
                        char     SenderAddr[16]   = {'\0'};
                        int8_t   IsUDHPresent;
                        uint8_t  i;
                        uint16_t  CdmaRefnum = 0;
                        uint8_t  CdmaTotalnum = 0;
                        uint8_t  CdmaSeqnum = 0;
                        struct sms_cdmaMsgDecodingParamsExt_t CdmaMsgDecodingParamsExt;

                        memset(&CdmaMsgDecodingParamsExt, 0, sizeof(CdmaMsgDecodingParamsExt));
                        CdmaMsgDecodingParamsExt.pMessage = getrawsms.message;
                        CdmaMsgDecodingParamsExt.messageLength = getrawsms.messageSize;
                        CdmaMsgDecodingParamsExt.pSenderAddr = SenderAddr;
                        CdmaMsgDecodingParamsExt.pSenderAddrLength = &SenderAddrLength;
                        CdmaMsgDecodingParamsExt.pTextMsg = (uint16_t*)ascii;
                        CdmaMsgDecodingParamsExt.pTextMsgLength = &TextMsgLength;
                        CdmaMsgDecodingParamsExt.pReferenceNum  = &CdmaRefnum;
                        CdmaMsgDecodingParamsExt.pTotalNum      = &CdmaTotalnum;
                        CdmaMsgDecodingParamsExt.pPartNum       = &CdmaSeqnum;
                        CdmaMsgDecodingParamsExt.pIsUDHPresent  = &IsUDHPresent;

                        unpackRetCode = sms_SLQSCDMADecodeMTTextMsgExt(&CdmaMsgDecodingParamsExt);

                        if( sMS_HELPER_OK != unpackRetCode )
                        {
                            printf( "Failed to decode SMS\n");
                            printf( "Failure Code: %u\n", unpackRetCode );
                        }
                        switch( unpackRetCode )
                        {
                            case sMS_HELPER_ENCODING_OR_MSGTYPE_NOT_SUPPORTED:
                                printf( "Message encoding or Teleservice type not supported\n");
                                break; 

                            case sMS_HELPER_ERR_SWISMS_MSG_CORRUPTED:
                                printf( "Message is not available or is corrupted\n" );
                                break;

                            case sMS_HELPER_ERR_SWISMS_SMSC_NUM_CORRUPTED:
                                printf( " SMSC/Mobile number is not correct\n" );
                                break;

                            case sMS_HELPER_ERR_BUFFER_SZ:
                                printf( " Internal Error - Can not read SMS \n" );
                                break;

                            case sMS_HELPER_ERR_MEMORY:
                                printf( " Memory allocation failure - Can not read SMS \n" );
                                break;

                            case sMS_HELPER_ERR_SWISMS_MSG_LEN_TOO_LONG:
                                printf( " Message exceeds 160 characters\n" );
                                break;

                            case sMS_HELPER_OK:
                                printf ("Decoding CDMA SMS\n");
                                if ( *CdmaMsgDecodingParamsExt.pIsUDHPresent)
                                {
                                    if(concate_prev_ref_number != (*CdmaMsgDecodingParamsExt.pReferenceNum))
                                    {
                                        if(concate_prev_ref_number != 0)
                                        {
                                            if(buf_allocated)
                                            {
                                                for ( i = 0; i < concate_prev_total_number; i++)
                                                {
                                                     if (copy_buf[i] != NULL) {
                                                        free(copy_buf[i]);
                                                        copy_buf[i] = NULL;
                                                     }
                                                 }
                                                 buf_allocated = FALSE;
                                            }
                                            concatenated_sms_counter = 0;
                                        }
                                    }

                                    concatenated_sms_counter++;

                                    if (!buf_allocated && (CdmaMsgDecodingParamsExt.pTotalNum!= NULL))
                                    {
                                        for ( i = 0; i < *CdmaMsgDecodingParamsExt.pTotalNum; i++)
                                        {
                                            if (CdmaMsgDecodingParamsExt.pTextMsgLength == NULL)
                                            {
                                                printf("invalid message length pointer!\n");
                                                return NULL;
                                            }
                                            else
                                            {
                                                if (*CdmaMsgDecodingParamsExt.pTextMsgLength <= 0)
                                                {
                                                    printf("invalid message size!\n");
                                                    return NULL;
                                                }
                                            }
                                            copy_buf[i] = malloc( (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char) );
                                            if (copy_buf[i] == NULL) {
                                                printf("memory allocation failure in smsdemo concatenating long SMS\n");
                                                break;
                                            } 
                                            memset(copy_buf[i], 0, (*CdmaMsgDecodingParamsExt.pTextMsgLength)*sizeof(char));
                                            buf_allocated = TRUE;
                                        }
                                        concate_prev_total_number = *CdmaMsgDecodingParamsExt.pTotalNum;
                                    }
                                    concate_prev_ref_number = *CdmaMsgDecodingParamsExt.pReferenceNum;
                                    if(CdmaMsgDecodingParamsExt.pPartNum!=NULL)
                                    {
                                        if (*CdmaMsgDecodingParamsExt.pPartNum > 0) 
                                        {
                                            if (copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1] != NULL) 
                                            {
                                                strcpy(copy_buf[*CdmaMsgDecodingParamsExt.pPartNum-1], (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                                printf("Part %d of Long Message:%s\n",
                                                    (*CdmaMsgDecodingParamsExt.pPartNum),
                                                    (char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                            }
                                        }
                                    }

                                    if(CdmaMsgDecodingParamsExt.pTotalNum!=NULL)
                                    {
                                        if (concatenated_sms_counter == (*CdmaMsgDecodingParamsExt.pTotalNum))
                                        {
                                            display_cdma_long_sms(&CdmaMsgDecodingParamsExt);
                                        }
                                    }                                    
                                }

                                else
                                {
                                    /* Print SMS details */
                                    fprintf(stderr,"Sender Number: %s\n",CdmaMsgDecodingParamsExt.pSenderAddr);
                                    fprintf(stderr,"SMS Text: %s\n",(char*)CdmaMsgDecodingParamsExt.pTextMsg);
                                    fprintf(stderr,"Time Stamp: %d:%d:%d:%d:%d:%d\n",
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_YEAR],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MONTH],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_DAY],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_HOUR],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_MINUTE],
                                                CdmaMsgDecodingParamsExt.mcTimeStamp.data[eTIMESTEMP_SECOND]);
                                }
                                break;
                             default:
                                break;
                        }
                    }

                    break;

                case eQMI_WMS_GET_MSG_LIST:
                    smslist.messageListSize = sizeof(smslist.messageList)/sizeof(qmiSmsMessageList);
                    unpackRetCode = unpack_sms_SLQSGetSMSList( msg.buf, rlen,&smslist);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSGetSMSList",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);
#endif                    
                    if(swi_uint256_get_bit (smslist.ParamPresenceMask, 1))
                    {
                        printf("MessageListSize  : %x\n", smslist.messageListSize);
                        for(count = 0; count < smslist.messageListSize; count++)
                        {
                            printf("messageIndex[%d] : %x\n",
                                    count, smslist.messageList[count].messageIndex);
                            printf("messageTag[%d] : %x\n",
                                    count, smslist.messageList[count].messageTag);
                        }
                    }
#if DEBUG_LOG_TO_FILE
                    CHECK_WHITELIST_MASK(
                        unpack_sms_SLQSGetSMSListParamPresenceMaskWhiteList,
                        smslist.ParamPresenceMask);
                    CHECK_MANDATORYLIST_MASK(
                        unpack_sms_SLQSGetSMSListParamPresenceMaskMandatoryList,
                        smslist.ParamPresenceMask);
                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  (smslist.messageListSize > 0) ? SUCCESS_MSG : FAILED_MSG);
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  (smslist.messageListSize > 0) ? SUCCESS_MSG : FAILED_MSG);
                    }
#endif

                    break;

                case eQMI_WMS_MODIFY_TAG:
                    unpackRetCode = unpack_sms_SLQSModifySMSStatus( msg.buf, rlen,&ModifySMSStatus);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SLQSModifySMSStatus",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);

                    CHECK_WHITELIST_MASK(
                        unpack_sms_SLQSModifySMSStatusParamPresenceMaskWhiteList,
                        ModifySMSStatus.ParamPresenceMask);
                    CHECK_MANDATORYLIST_MASK(
                        unpack_sms_SLQSModifySMSStatusParamPresenceMaskMandatoryList,
                        ModifySMSStatus.ParamPresenceMask);

                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,", SUCCESS_MSG);
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n", SUCCESS_MSG);
                    }
#endif

                    if(!unpackRetCode)
                        printf("SLQS Modify SMS Status Successful\n");
                    break;

                case eQMI_WMS_DELETE:
                    unpackRetCode = unpack_sms_SLQSDeleteSMS( msg.buf, rlen,&DeleteSMS);
#if DEBUG_LOG_TO_FILE

                    local_fprintf("%s,%s,", "unpack_sms_SLQSDeleteSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);

                    CHECK_WHITELIST_MASK(
                        unpack_sms_SLQSDeleteSMSParamPresenceMaskWhiteList,
                        DeleteSMS.ParamPresenceMask);
                    CHECK_MANDATORYLIST_MASK(
                        unpack_sms_SLQSDeleteSMSParamPresenceMaskMandatoryList,
                        DeleteSMS.ParamPresenceMask);

                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,", SUCCESS_MSG);
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n", SUCCESS_MSG);
                    }
#endif                    

                    if(!unpackRetCode)
                        printf("SLQS Delete SMS Successful\n");
                    break;

                case eQMI_WMS_RAW_SEND:
                    unpackRetCode = unpack_sms_SendSMS( msg.buf, rlen, &sendsms);
#if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_sms_SendSMS",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(msg.buf);

                    CHECK_WHITELIST_MASK(
                        unpack_sms_SendSMSParamPresenceMaskWhiteList,
                        sendsms.ParamPresenceMask);
                    CHECK_MANDATORYLIST_MASK(
                        unpack_sms_SendSMSParamPresenceMaskMandatoryList,
                        sendsms.ParamPresenceMask);

                    if ( unpackRetCode != 0 )
                    {
                        local_fprintf("%s,",  (sendsms.messageID > 0) ? SUCCESS_MSG : FAILED_MSG);
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  (sendsms.messageID > 0) ? SUCCESS_MSG : FAILED_MSG);
                    }
#endif     

                    if(!unpackRetCode)
                        printf("Send SMS Successful\n");
                    break;
                case eQMI_WMS_SET_EVENT:
                    if(rsp_ctx.type == eIND)
                    {
                        unpackRetCode = unpack_sms_SetNewSMSCallback_ind( msg.buf, rlen, &newsmscbind);
#if DEBUG_LOG_TO_FILE

                        local_fprintf("%s,%s,", "unpack_sms_SetNewSMSCallback_ind",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                            checkErrorAndFormErrorString(msg.buf);

                        CHECK_WHITELIST_MASK(
                            unpack_sms_SetNewSMSCallback_indParamPresenceMaskWhiteList,
                            newsmscbind.ParamPresenceMask);

                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,",  SUCCESS_MSG);
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  SUCCESS_MSG);
                        }
#endif

                        printf ("SMS IND unpacked: %d\n", unpackRetCode);
                        if ((newsmscbind.NewMMTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 16)))
                        {
                            printf ("NewMMTlv.MTMessageInfo.storageType : %d\n", newsmscbind.NewMMTlv.MTMessageInfo.storageType);
                            printf ("NewMMTlv.MTMessageInfo.messageIndex: %d\n", newsmscbind.NewMMTlv.MTMessageInfo.messageIndex);

                        }
                        if ((newsmscbind.MMTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 18)))
                        {
                            printf ("MMTlv.MessageModeInfo.messageMode : %d\n", newsmscbind.MMTlv.MessageModeInfo.messageMode);
                        }
                        if ((newsmscbind.ETWSTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 19)))
                        {
                            printf ("ETWSTlv.EtwsMessageInfo.notificationType : %d\n", newsmscbind.ETWSTlv.EtwsMessageInfo.notificationType);
                            printf ("ETWSTlv.EtwsMessageInfo.length : %d\n", newsmscbind.ETWSTlv.EtwsMessageInfo.length);
                            Length = newsmscbind.ETWSTlv.EtwsMessageInfo.length;
                            lIndex = 0;
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.ETWSTlv.EtwsMessageInfo.data[lIndex++] );
                                    if (lIndex % 16 ==0)
                                    {
                                            fprintf ( stderr, "\n");
                                    }
                            }        
                            fprintf ( stderr, "\r\n\n" );
                        }
                        if ((newsmscbind.TRMessageTlv.TlvPresent)&&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 17)))
                        {
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.transactionID : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.transactionID);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.format : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.format);
                            printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.length : %d\n", newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.length);
                            Length = newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.length;
                            lIndex = 0;
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.TRMessageTlv.TransferRouteMTMessageInfo.data[lIndex++] );
                                    if (lIndex % 16 ==0)
                                    {
                                            fprintf ( stderr, "\n");
                                    }
                            }
                            fprintf ( stderr, "\r\n\n" );
                        }
                        if ((newsmscbind.ETWSPLMNTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 20)))
                        {
                            printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode : %d\n", newsmscbind.ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode);
                            printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode : %d\n", newsmscbind.ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode);
                        }
                        if ((newsmscbind.SMSCTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 21)))
                        {
                            printf ("SMSCTlv.SMSCInfo.length : %d\n", newsmscbind.SMSCTlv.SMSCInfo.length);
                            Length = newsmscbind.SMSCTlv.SMSCInfo.length;
                            lIndex = 0;                            
                            while(Length--)
                            {
                                fprintf ( stderr,
                                    "%2xH",
                                    newsmscbind.SMSCTlv.SMSCInfo.data[lIndex++] );
                            }
                            fprintf ( stderr, "\r\n\n" );                            
                        }
                        if ((newsmscbind.IMSTlv.TlvPresent) &&
                            (swi_uint256_get_bit (newsmscbind.ParamPresenceMask, 22)))
                        {
                            printf ("IMSTlv.IMSInfo.smsOnIMS : %d\n", newsmscbind.IMSTlv.IMSInfo.smsOnIMS);
                        }
                        /* send request to read the message content */
                        send_sms_read_req (newsmscbind.NewMMTlv.MTMessageInfo.storageType, newsmscbind.NewMMTlv.MTMessageInfo.messageIndex, newsmscbind.MMTlv.MessageModeInfo.messageMode );
                    }
                    else
                    {
                        unpack_sms_SetNewSMSCallback_t SetNetSMSCallback;
                        unpackRetCode = unpack_sms_SetNewSMSCallback( msg.buf, rlen,&SetNetSMSCallback);
                        if(!unpackRetCode)
                            printf(" Enabled SMS event callback\n");
#if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_sms_SetNewSMSCallback",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                            checkErrorAndFormErrorString(msg.buf);

                        CHECK_WHITELIST_MASK(
                            unpack_sms_SetNewSMSCallbackParamPresenceMaskWhiteList,
                            SetNetSMSCallback.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_sms_SetNewSMSCallbackParamPresenceMaskMandatoryList,
                            SetNetSMSCallback.ParamPresenceMask);

                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", SUCCESS_MSG);
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", SUCCESS_MSG);
                        }
#endif
                    }
                    break;

                case eQMI_WMS_MEMORY_FULL_IND:
                    unpack_sms_SLQSWmsMemoryFullCallBack_ind( msg.buf, rlen, &full_stat);
                    if(swi_uint256_get_bit (full_stat.ParamPresenceMask, 1))
                        printf("sms store full, storage type/message mode : %d/%d\n",
                            full_stat.storageType, full_stat.messageMode);
                    break;
                case eQMI_WMS_TRANS_LAYER_INFO_IND:
                    unpackRetCode = unpack_sms_SLQSTransLayerInfoCallback_ind( msg.buf, rlen, &transLayerInfo);
                    printf("unpack QMI_WMS_TRANS_LAYER_INFO_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                         if(swi_uint256_get_bit (transLayerInfo.ParamPresenceMask, 1))
                            printf("Registration Indication 0x%x\n",transLayerInfo.regInd);
                         if ( (transLayerInfo.pTransLayerInfo) &&
                              (swi_uint256_get_bit (transLayerInfo.ParamPresenceMask, 16)))
                         {
                             printf("Transport Layer Information:\n" );
                             printf("Transport Type: 0x%x\n", 
                                     transLayerInfo.pTransLayerInfo->TransType );
                             printf("Transport Capabilities 0x%x\n", 
                                     transLayerInfo.pTransLayerInfo->TransCap );
                         }
                    }
                    break;
                case eQMI_WMS_TRANS_NW_REG_INFO_IND:
                    unpackRetCode = unpack_sms_SLQSNWRegInfoCallback_ind( msg.buf, rlen, &nwRegInfo);
                    printf("unpack QMI_WMS_TRANS_NW_REG_INFO_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                         if(swi_uint256_get_bit (nwRegInfo.ParamPresenceMask, 1))
                         printf("Network Registration Status 0x%x\n",nwRegInfo.NWRegStat);
                    }
                    break;
                case eQMI_WMS_MSG_WAITING_IND:
                    unpackRetCode = unpack_sms_SLQSWmsMessageWaitingCallBack_ind( msg.buf, rlen, &msgWait);
                    printf("unpack QMI_WMS_MSG_WAITING_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                        if(swi_uint256_get_bit (msgWait.ParamPresenceMask, 1))
                        {
                            uint8_t i;
                            printf("\nreceive Message Waiting Info Indication\n");
                            for ( i = 0; i < msgWait.numInstances; i++)
                            {
                                printf("message type: %d\n", msgWait.msgWaitInfo[i].msgType);
                                printf("active indication: %d\n", msgWait.msgWaitInfo[i].activeInd);
                                printf("message count: %d\n", msgWait.msgWaitInfo[i].msgCount);
                                printf("\n");
                            }
                        }
                        
                    }
                    break;
                case eQMI_WMS_ASYNC_RAW_SEND_IND:
                    unpackRetCode = unpack_sms_SLQSWmsAsyncRawSendCallBack_ind( msg.buf, rlen, &rawAsync);
                    printf("unpack QMI_WMS_ASYNC_RAW_SEND_IND result :%d\n",unpackRetCode);
                    if (unpackRetCode == eQCWWAN_ERR_NONE) {
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 1))
                        printf("Send Status     : %d\n", rawAsync.sendStatus);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 16))
                        printf("Message Id      : %d\n", rawAsync.messageID);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 17))
                        printf("Cause Code      : %d\n", rawAsync.causeCode);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 18))
                        printf("Error Class     : %d\n", rawAsync.errorClass);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 19))
                        printf("RP Cause        : %d\n", rawAsync.RPCause);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 19))
                        printf("TP Cause        : %d\n", rawAsync.TPCause);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 22))
                        {
                            printf("Alpha Id Length : %d\n", rawAsync.alphaIDLen);
                            if (rawAsync.alphaIDLen)
                                printf("Alpha Id: %s\n", rawAsync.pAlphaID);
                        }
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 23))
                        printf("User Data       : %u\n", rawAsync.userData);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 20))
                        printf("Message Delivery Failure Type  : %d\n", rawAsync.msgDelFailureType);
                        if(swi_uint256_get_bit (rawAsync.ParamPresenceMask, 21))
                        printf("Message Delivery Failure Cause : %d\n", rawAsync.msgDelFailureCause);
                        printf("\n");
                    }
                    break;
            }
        }
        else
        {
            enSmsThread = 0;
            sms = -1;
            return NULL;
        }
    }
   return NULL;
}
void write_qmi_message(int sms, uint8_t *qmiReq, uint16_t qmiReqLen)
{
    int rtn = -1;
    int j;
    const char *qmi_msg;

    if(qmiReqLen <= 0)
    {
        printf("invalid qmi request length\n");
        return;
    }

    for (j = 0; j < qmiReqLen; j++)
    {
        printf("%02x ", qmiReq[j]);
        if ((j+1)%16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    rtn = write(sms, qmiReq, qmiReqLen);

    if (rtn != qmiReqLen)
        printf("write %d wrote %d\n", qmiReqLen, rtn);
    else
    {
        qmi_msg = helper_get_req_str(eSMS, qmiReq, qmiReqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmiReqLen, qmiReq);
    }
}

void sms_test_ind()
{
    pack_qmi_t req_ctx;
    int rtn;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len = QMI_MSG_MAX;
    char *message = "Welcome to Sierra Wireless";
    char *text = "hello word";

    if(sms<0)
        sms = client_fd(eSMS);
    if(sms<0)
    {
        printf("SMS Service Not Supported!\n");
        return ;
    }

    printf("======SMS pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "SMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    memset(&sms_attr, 0, sizeof(sms_attr));
    enSmsThread = 1;
    pthread_create(&sms_tid, &sms_attr, sms_read_thread, NULL);
    sleep(1);
    
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = 10;

    printf("set indication register ...\n");
    rtn = pack_sms_SLQSSetIndicationRegister(&req_ctx, qmi_req, 
                                     &qmi_req_len, &tpack_sms_SLQSSetIndicationRegister);
    if(rtn<0)
    {
        printf("pack_sms_SLQSSetIndicationRegister Fail ...\n");
        sms_test_ind_exit();
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SLQSSetIndicationRegister", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
    printf("pack_sms_SLQSSetIndicationRegister returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);

    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SLQSGetSMS_t getsms_reqParam;
    getsms_reqParam.storageType  = STORAGE_UIM;
    getsms_reqParam.messageIndex = 0x01;
    getsms_reqParam.pMessageMode  = &tpack_sms_SLQSGetSMSmessageMode;

    printf("get sms raw read for UIM...\n");
    rtn = pack_sms_SLQSGetSMS(&req_ctx, qmi_req, &qmi_req_len, &getsms_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSGetSMS(UIM) Fail ...\n");
        sms_test_ind_exit();
    }
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_sms_SLQSGetSMS(UIM)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSGetSMS(UIM) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    printf("get sms raw read for NV...\n");
    getsms_reqParam.storageType = STORAGE_NV; //NV
    rtn = pack_sms_SLQSGetSMS(&req_ctx, qmi_req, &qmi_req_len, &getsms_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSGetSMS(NV) Fail ...\n");
        sms_test_ind_exit();
    }
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_sms_SLQSGetSMS(NV)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSGetSMS(NV) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SLQSGetSMSList_t smslist_reqParam;
    smslist_reqParam.storageType = storageType;
    uint8_t messageModelist = tpack_sms_SLQSGetSMSmessageMode;
    uint32_t requestedTag = 0x00; //MT Read
    smslist_reqParam.pMessageMode = &messageModelist;
    smslist_reqParam.pRequestedTag = &requestedTag;

    printf("get sms list ...\n");
    rtn = pack_sms_SLQSGetSMSList(&req_ctx, qmi_req, &qmi_req_len, &smslist_reqParam);
    printf("pack_sms_SLQSGetSMSList returns %d\n", rtn);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SLQSGetSMSList", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
    if(rtn<0)
    {
        printf("pack_sms_SLQSGetSMSList Fail ...\n");
        sms_test_ind_exit();
    }
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SLQSModifySMSStatus_t smsstatus_reqParam;
    smsstatus_reqParam.storageType = STORAGE_UIM; //UIM
    smsstatus_reqParam.messageIndex = 0x01;
    smsstatus_reqParam.messageTag =  0x01; /*MT Not Read*/
    uint8_t messageModeStat = tpack_sms_SLQSGetSMSmessageMode;
    smsstatus_reqParam.pMessageMode = &messageModeStat;

    printf("modify sms tag(storage UIM) ...\n");
    rtn = pack_sms_SLQSModifySMSStatus(&req_ctx, qmi_req, &qmi_req_len, &smsstatus_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSModifySMSStatus(storage UIM) Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_sms_SLQSModifySMSStatus(storage UIM)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSModifySMSStatus(storage UIM) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    printf("modify sms tag(storage NV) ...\n");
    smsstatus_reqParam.storageType = STORAGE_NV; //NV
    rtn = pack_sms_SLQSModifySMSStatus(&req_ctx, qmi_req, &qmi_req_len, &smsstatus_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSModifySMSStatus(storage NV) Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_sms_SLQSModifySMSStatus(storage NV)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSModifySMSStatus(storage NV) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SLQSDeleteSMS_t deletesms_reqParam;
    memset(&deletesms_reqParam, 0, sizeof(deletesms_reqParam));
    deletesms_reqParam.storageType = STORAGE_UIM; //UIM
    uint32_t messageIndex = 0x01;
    uint8_t messageModeDel  = tpack_sms_SLQSGetSMSmessageMode;
    deletesms_reqParam.pMessageIndex = &messageIndex;
    deletesms_reqParam.pMessageMode = &messageModeDel;

    printf("delete sms(storage UIM)  ...\n");
    rtn = pack_sms_SLQSDeleteSMS(&req_ctx, qmi_req, &qmi_req_len, &deletesms_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSDeleteSMS(storage UIM) Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SLQSDeleteSMS(storage UIM)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSDeleteSMS(storage UIM) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    printf("delete sms(storage NV)  ...\n");
    deletesms_reqParam.storageType = STORAGE_NV; //NV
    rtn = pack_sms_SLQSDeleteSMS(&req_ctx, qmi_req, &qmi_req_len, &deletesms_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SLQSDeleteSMS(storage NV) Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SLQSDeleteSMS(storage NV)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SLQSDeleteSMS(storage NV) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SetNewSMSCallback_t  newsmscb_reqParam;
    newsmscb_reqParam.status  = LITEQMI_QMI_CBK_PARAM_SET;

    printf("set new sMS callback ...\n");
    rtn = pack_sms_SetNewSMSCallback(&req_ctx, qmi_req, &qmi_req_len, newsmscb_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SetNewSMSCallback Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SetNewSMSCallback", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SetNewSMSCallback returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;


    printf("encode message with CDMA format  ...\n");
    sms_SLQSCDMAEncodeMOTextMsg_t smsMsg;
    uint32_t messagelength = 0;
    SLQSCDMAEncodeMOMessage TestMessage;

    messagelength = strlen(message);
    uint8_t messageId  = 0;
    memcpy (&TestMessage, message, messagelength);
    uint8_t alphabet   = USER_DATA_MSG_ENCODING_7BIT_ASCII;
    uint8_t pdu_message[255] = {0};

    memset(&smsMsg, 0, sizeof(sms_SLQSCDMAEncodeMOTextMsg_t));
    smsMsg.messageSize      = 255;
    smsMsg.pMessage         = (uint8_t *)pdu_message;
    smsMsg.messageId        = messageId;
    smsMsg.pDestAddr        = (int8_t *)phoneNum;
    smsMsg.pTextMsg         = TestMessage.u16Msg;
    smsMsg.textMsgLength    = messagelength;
    smsMsg.encodingAlphabet = alphabet;

    int ret = sms_SLQSCDMAEncodeMOTextMsg(&smsMsg);
    printf("sms_SLQSCDMAEncodeMOTextMsg Return value : %d\n", ret);
    if (sMS_HELPER_OK != ret )
    {
        printf("sms_SLQSCDMAEncodeMOTextMsg Return value : %d\n", ret);
    }
    printf("sms_SLQSCDMAEncodeMOTextMsg Encoded Message Size : %d\n", smsMsg.messageSize);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "sms_SLQSCDMAEncodeMOTextMsg", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    pack_sms_SendSMS_t pack_SendSMS;
    memset(&pack_SendSMS, 0, sizeof(pack_SendSMS));
    pack_SendSMS.messageFormat = MESSAGE_FORMAT_CDMA;
    pack_SendSMS.messageSize   = smsMsg.messageSize;
    pack_SendSMS.pMessage      = pdu_message;

    ret = pack_sms_SendSMS (&req_ctx, qmi_req, &qmi_req_len, &pack_SendSMS);
    if (eQCWWAN_ERR_NONE != ret )
    {
        printf("pack_sms_SendSMS(CDMA) error : %d\n", ret);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SendSMS(CDMA)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SendSMS(CDMA) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;


    printf("encode message with UMTS format  ...\n");
    sms_SLQSWCDMAEncodeMOTextMsg_t sMsg;
    uint32_t msglength = 0;
    alphabet = DEFAULTENCODING;
    msglength = strlen(text);

    memset(&sMsg, 0, sizeof(sms_SLQSWCDMAEncodeMOTextMsg_t));
    sMsg.messageSize = msglength;
    sMsg.pDestAddr   = (int8_t *)phoneNum;
    sMsg.pTextMsg    = (int8_t *)text;
    sMsg.alphabet    = alphabet;

    ret = sms_SLQSWCDMAEncodeMOTextMsg(&sMsg);
    printf("sms_SLQSWCDMAEncodeMOTextMsg Return value : %d\n", ret);
    if (sMS_HELPER_OK != ret )
    {
        printf("sms_SLQSWCDMAEncodeMOTextMsg Return value : %d\n", ret);
    }
    printf("sms_SLQSWCDMAEncodeMOTextMsg Encoded Message Size : %d\n", sMsg.messageSize);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "sms_SLQSWCDMAEncodeMOTextMsg", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    memset(&pack_SendSMS, 0, sizeof(pack_sms_SendSMS_t));
    pack_SendSMS.messageFormat = MESSAGE_FORMAT_GW_PP; //UMTS
    pack_SendSMS.messageSize   = sMsg.messageSize;
    pack_SendSMS.pMessage      = (uint8_t *)sMsg.PDUMessage;

    ret = pack_sms_SendSMS (&req_ctx, qmi_req, &qmi_req_len, &pack_SendSMS);
    if (eQCWWAN_ERR_NONE != ret )
    {
        printf("pack_sms_SendSMS(UMTS) error : %d\n", ret);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SendSMS(UMTS)", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SendSMS(UMTS) returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(2);

    req_ctx.msgid = 0;
    req_ctx.timeout = 0;
    req_ctx.xid++;

    pack_sms_SendSMS_t sendsms_reqParam;
    sendsms_reqParam.messageFormat = MESSAGE_FORMAT_GW_PP; //UMTS
    sendsms_reqParam.messageSize = 0xFF;
    sendsms_reqParam.messageSize = msg_pdu_number_size;
    sendsms_reqParam.pMessage = msg_pdu_number;
    uint8_t linktimer  = 0x03;
    sendsms_reqParam.pLinktimer = &linktimer;

    printf("send sms  ...\n");
    rtn = pack_sms_SendSMS(&req_ctx, qmi_req, &qmi_req_len, &sendsms_reqParam);
    if(rtn<0)
    {
        printf("pack_sms_SendSMS Fail ...\n");
        sms_test_ind_exit();
        return ;
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_sms_SendSMS", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    printf("pack_sms_SendSMS returns %d\n", rtn);
    write_qmi_message(sms, qmi_req, qmi_req_len);
    sleep(5);

#if 0
    sms_SLQSWCDMAEncodeMOTextMsg_t wcdmaentxmsg_reqParam;
    memset(&wcdmaentxmsg_reqParam,0,sizeof(sms_SLQSWCDMAEncodeMOTextMsg_t));
    wcdmaentxmsg_reqParam.pDestAddr = (int8_t*)PHONE_NUM;
    wcdmaentxmsg_reqParam.pTextMsg  = (int8_t*)"Welcome to Sierra Wireless";
    
    rtn = sms_SLQSWCDMAEncodeMOTextMsg(&wcdmaentxmsg_reqParam);
    if ( !rtn )
    {
        printf("Encoding of Text is Successful\n");
        printf("Encoded message : ");
        uint8_t count =0;
        for (count=0; count<wcdmaentxmsg_reqParam.messageSize; count++)
            printf(" %02x",(uint8_t)wcdmaentxmsg_reqParam.PDUMessage[count]);
     }
    printf("\n");
    sleep(4);

    sms_SLQSWCDMADecodeMTTextMsg_t wcdmadctxmsg_reqParam;
    uint8_t wcdmaMessage[] = {0x07, 0x91, 0x19, 0x89, 0x51, 0x50, 0x91, 0x73, 0x04, 0x0C,
                 0x91, 0x19, 0x48, 0x72, 0x02, 0x85, 0x07, 0x00, 0x00, 0x51,
                 0x01, 0x41, 0x81, 0x12, 0x33, 0x22, 0x2A, 0xC8, 0x32, 0x9B,
                 0xFD, 0x66, 0x81, 0xEE, 0x65, 0xF6, 0xF8, 0xDD, 0x2E, 0x83,
                 0xE8, 0x6F, 0xD0, 0x34, 0x5D, 0x96, 0xCB, 0xC3, 0xA0, 0x6B,
                 0x5A, 0x5E, 0x66, 0x97, 0xE7, 0xF3, 0x10, 0xE8, 0x58, 0x97,
                 0xB7, 0xCB, 0x65, 0x3A};

    memset(&wcdmadctxmsg_reqParam,0,sizeof(sms_SLQSWCDMADecodeMTTextMsg_t));
    wcdmadctxmsg_reqParam.pMessage = wcdmaMessage;
    wcdmadctxmsg_reqParam.scAddrLength = MAX_SMSC_LENGTH;
    wcdmadctxmsg_reqParam.senderAddrLength = MAX_SMSC_LENGTH;
    wcdmadctxmsg_reqParam.textMsgLength = MAX_SMS_MSG_LEN;

    rtn = sms_SLQSWCDMADecodeMTTextMsg(&wcdmadctxmsg_reqParam);
    if ( !rtn )
    {
        printf("WCDMA Decoding Successful\n");
        printf("\nService Center Number = %s",wcdmadctxmsg_reqParam.scAddr);
        printf("\nSender Number = %s",wcdmadctxmsg_reqParam.senderAddr);
        printf("\nDate = %s",wcdmadctxmsg_reqParam.date);
        printf("\nTime = %s",wcdmadctxmsg_reqParam.time);
        printf("\nMessage: %s\n\n",wcdmadctxmsg_reqParam.textMsg );
    }

    sms_SLQSCDMAEncodeMOTextMsg_t cdmaentxmsg_reqParam;
    uint8_t sms_SLQSCDMAEncodeMOTextMsg_t[255]={0};
    cdmaentxmsg_reqParam.messageSize = 250;
    cdmaentxmsg_reqParam.messageId = 0;
    cdmaentxmsg_reqParam.pDestAddr = (int8_t*)PHONE_NUM;
    cdmaentxmsg_reqParam.pCallbackAddr = NULL;
    cdmaentxmsg_reqParam.pTextMsg  = (uint16_t*)"Welcome to Sierra Wireless";
    cdmaentxmsg_reqParam.textMsgLength = strlen("Welcome to Sierra Wireless");
    cdmaentxmsg_reqParam.pPriority = NULL;
    cdmaentxmsg_reqParam.encodingAlphabet = 2;
    cdmaentxmsg_reqParam.pRelValidity = NULL;
    cdmaentxmsg_reqParam.pMessage = &sms_SLQSCDMAEncodeMOTextMsg_t[0];
    rtn = sms_SLQSCDMAEncodeMOTextMsg(&cdmaentxmsg_reqParam);

    if ( !rtn )
    {
        printf("Encoding of Text is Successful\n");
        printf("Encoded message : ");
        uint8_t count =0;
        for (count=0; count<cdmaentxmsg_reqParam.messageSize; count++)
            printf(" %02x ",sms_SLQSCDMAEncodeMOTextMsg_t[count]);
     }
    printf("\n");
    sleep(4);

    uint8_t cdmaMessage[] = {0x00, 0x00, 0x02, 0x10, 0x02, 0x04, 0x08, 0x03, 0x24, 0x61, 0x09, 0xca,
                             0x96, 0x1e, 0x80, 0x08, 0x26, 0x00, 0x03, 0x20, 0x00, 0x00, 0x01, 0x19,
                             0x10, 0xd5, 0x7c, 0xbb, 0x31, 0xef, 0xdb, 0x95, 0x07, 0x4d, 0xe8, 0x29,
                             0xe9, 0xcb, 0xcb, 0x96, 0x14, 0x15, 0xf4, 0xf2, 0xcb, 0xb3, 0x2f, 0x3e,
                             0x60, 0x05, 0x01, 0x0b, 0x08, 0x01, 0x00};

    char     ascii[2048]= {0};
    uint8_t  SenderAddrLength = 16;
    uint8_t  TextMsgLength    = 162;
    char     SenderAddr[16]   = {'\0'};
    int8_t   IsUDHPresent;
    uint8_t  CdmaRefnum = 0;
    uint8_t  CdmaTotalnum = 0;
    uint8_t  CdmaSeqnum = 0;

    struct sms_cdmaMsgDecodingParamsExt_t cdmadctxmsg_reqParam;

    memset(&cdmadctxmsg_reqParam, 0, sizeof(struct sms_cdmaMsgDecodingParamsExt_t));
    cdmadctxmsg_reqParam.pMessage = cdmaMessage;
    cdmadctxmsg_reqParam.messageLength = sizeof(cdmaMessage);
    cdmadctxmsg_reqParam.pSenderAddr = SenderAddr;
    cdmadctxmsg_reqParam.pSenderAddrLength = &SenderAddrLength;
    cdmadctxmsg_reqParam.pTextMsg = (uint16_t*)ascii;
    cdmadctxmsg_reqParam.pTextMsgLength = &TextMsgLength;
    cdmadctxmsg_reqParam.pReferenceNum  = &CdmaRefnum;
    cdmadctxmsg_reqParam.pTotalNum      = &CdmaTotalnum;
    cdmadctxmsg_reqParam.pPartNum       = &CdmaSeqnum;
    cdmadctxmsg_reqParam.pIsUDHPresent  = &IsUDHPresent;

    rtn = sms_SLQSCDMADecodeMTTextMsgExt(&cdmadctxmsg_reqParam);
    if ( !rtn )
    {
        printf(" CDMA Decoding Successful\n");
        printf("Message : ");
        printf("Sender Number: %s\n",cdmadctxmsg_reqParam.pSenderAddr);
        printf("SMS Text: %s\n",(char*)cdmadctxmsg_reqParam.pTextMsg);
        IFPRINTF("priority: %x\n",cdmadctxmsg_reqParam.pPriority);
        IFPRINTF("privacy: %x\n",cdmadctxmsg_reqParam.pPrivacy);
        IFPRINTF("language: %x\n",cdmadctxmsg_reqParam.pLanguage);
        IFPRINTF("relativeValidity: %x\n",
                cdmadctxmsg_reqParam.pRelativeValidity);
        IFPRINTF("displayMode: %x\n",
                cdmadctxmsg_reqParam.pDisplayMode);
        IFPRINTF("userAcknowledgementReq: %x\n",
                cdmadctxmsg_reqParam.pUserAcknowledgementReq);
        IFPRINTF("readAcknowledgementReq: %x\n",
                cdmadctxmsg_reqParam.pReadAcknowledgementReq);
        IFPRINTF("alertPriority: %x\n",cdmadctxmsg_reqParam.pAlertPriority);
        IFPRINTF("callbkAddrLength: %x\n",cdmadctxmsg_reqParam.pCallbkAddrLength);
        IFPRINTF("callbkAddr: %x\n",cdmadctxmsg_reqParam.pCallbkAddr);
    }
    printf("\nwait for 30 seconds to receive SMS indications, if any...\n");
    sleep(3);
#endif
}

int send_long_sms()
{
    int  rtn = eQCWWAN_ERR_NONE;
    uint8_t   i, multiple_sms_number = 0;
    uint8_t   reference_number, total_number, sequence_number;
    uint8_t   hexa[200]={0};
    char   *pdu=NULL;
    char *escaped_msg = NULL;
    uint16_t escaped_len;
    uint8_t encodingScheme = 0; //7 bit encoding
    uint16_t chrunk_size = 153; //when encoding scheme is 7 bit ecoding, the maximum number of chracters will be 153
    char msgText_backslash[] = "\\N\\N\\N\\N\\N\\N\\N\\N\\DGDSFGFDGFD\\N\\N\\N\\N\\N\\N\\N\\\\N\\N\\N\\N\\N\\N\\N\\N"
                               "\\DGDSFGFDGFD\\N\\N\\N\\N\\N\\N\\N\\\\N\\N\\N\\N\\N\\N\\N\\N\\DGDSFGFDGFD\\N\\N\\N\\N"
                               "\\N\\N\\N\\\\N\\N\\N\\N\\N\\N\\N\\N\\DGDSFGFDGFD\\N\\N\\N\\N\\N\\N\\N\\\\N\\N\\N\\N\\N"
                               "\\N\\N\\N\\DGDSFGFDGFD\\N\\N\\N\\N\\N\\N\\N\\";
    pack_qmi_t req_ctx;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len = QMI_MSG_MAX;
    const char *qmi_msg;

    if(sms<0)
        sms = client_fd(eSMS);
    if(sms<0)
    {
        fprintf(stderr,"SMS Service Not Supported!\n");
        return eQCWWAN_ERR_QMI_NOT_SUPPORTED;
    }
    memset(&sms_attr, 0, sizeof(sms_attr));
    memset(qmi_req, 0, QMI_MSG_MAX);
    enSmsThread = 1;
    pthread_create(&sms_tid, &sms_attr, sms_read_thread, NULL);
    sleep(1);
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = 10;


    pack_sms_SetNewSMSCallback_t  newsmscb_reqParam;
    newsmscb_reqParam.status  = LITEQMI_QMI_CBK_PARAM_SET;

    printf("set new sMS callback ...\n");
    rtn = pack_sms_SetNewSMSCallback(&req_ctx, qmi_req, &qmi_req_len, newsmscb_reqParam);
    rtn = write(sms, qmi_req, qmi_req_len);
    if (rtn != qmi_req_len)
        printf("write %d wrote %d\n", qmi_req_len, rtn);
    else
    {
        qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(2);

    sms_SLQSWCDMAEncodeMOTextMsg_t wcdmaentxmsg_reqParam;
    memset(&wcdmaentxmsg_reqParam,0,sizeof(sms_SLQSWCDMAEncodeMOTextMsg_t));
    wcdmaentxmsg_reqParam.pDestAddr = (int8_t*)PHONE_NUM;
    wcdmaentxmsg_reqParam.pTextMsg  = (int8_t*)&msgText_backslash[0];
    wcdmaentxmsg_reqParam.messageSize = strlen(msgText_backslash);

    if ( (wcdmaentxmsg_reqParam.pTextMsg == NULL) ||(wcdmaentxmsg_reqParam.pDestAddr == NULL) )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    if (wcdmaentxmsg_reqParam.messageSize <= 0)
    {
        printf("invalid message size!\n");
        return eQCWWAN_ERR_BUFFER_SZ;
    }
    escaped_msg = (char*)malloc(2 * wcdmaentxmsg_reqParam.messageSize);
    if( escaped_msg == NULL )
    {
        printf("%s: failed to allocated %d escape buffer\n", __func__,
                2*wcdmaentxmsg_reqParam.messageSize);
        return eQCWWAN_ERR_MEMORY ;
    }
    escaped_len = 2*wcdmaentxmsg_reqParam.messageSize;

    sms_ConvertAsciiToGSMCharacter((char*)wcdmaentxmsg_reqParam.pTextMsg, 
                               wcdmaentxmsg_reqParam.messageSize,
                               escaped_msg, &escaped_len);

    multiple_sms_number = (wcdmaentxmsg_reqParam.messageSize + chrunk_size - 1) / chrunk_size;

    /* Allocate pdu buffer memory */
    pdu = (char*)malloc(SMS_ASCII_STRING_SIZE*sizeof(char));
    if( pdu == NULL )
    {
        free(escaped_msg);
        return eQCWWAN_ERR_MEMORY ;
    }

    if (multiple_sms_number > 1)
    {
        /* long sms */
        total_number = multiple_sms_number;
        reference_number = 0;
        for ( i = 0; i < multiple_sms_number; i++)
        {
            memset(pdu, 0, SMS_ASCII_STRING_SIZE);
            sequence_number = i+1;
            if ( i == (multiple_sms_number-1) )
            {
    
                rtn = sms_SwiMakeConcatenatedSmsPdu( (char*)wcdmaentxmsg_reqParam.pDestAddr,
                                                    &escaped_msg[i*chrunk_size],
                                                    escaped_len - i*chrunk_size,
                                                    pdu,
                                                    SMS_ASCII_STRING_SIZE,
                                                    encodingScheme,
                                                    reference_number,
                                                    total_number,
                                                    sequence_number );
            }
            else
            {
                rtn = sms_SwiMakeConcatenatedSmsPdu( (char*)wcdmaentxmsg_reqParam.pDestAddr,
                                                    &escaped_msg[i*chrunk_size],
                                                    chrunk_size,
                                                    pdu,
                                                    SMS_ASCII_STRING_SIZE,
                                                    encodingScheme,
                                                    reference_number,
                                                    total_number,
                                                    sequence_number );
            }
            if( eQCWWAN_ERR_NONE != rtn )
            {
                free(pdu);
                free(escaped_msg);
                return eQCWWAN_ERR_PDU_GENERATION;
            }

            sms_SwiAsc2Hex((uint8_t*)hexa,sizeof(hexa), 
                (int8_t*)pdu,strlen((char*)pdu));

            pack_sms_SendSMS_t sendsms_reqParam;
            sendsms_reqParam.messageFormat = 0x06; /*GW-PP*/
            sendsms_reqParam.messageSize = strlen(pdu)/2;
            sendsms_reqParam.pMessage = hexa;
            uint8_t linktimer  = 0x03;
            sendsms_reqParam.pLinktimer = &linktimer;
        
            printf("send sms  ...\n");
            rtn = pack_sms_SendSMS(&req_ctx, qmi_req, &qmi_req_len, &sendsms_reqParam);
            if (rtn )
            {
                printf("packing error for send sms %d\n", rtn);
            }
            rtn = write(sms, qmi_req, qmi_req_len);
            if (rtn != qmi_req_len)
                printf("write %d wrote %d\n", qmi_req_len, rtn);
            else
            {
                qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
                printf(">> sending %s\n", qmi_msg);
                dump_hex(qmi_req_len, qmi_req);
                /* reset return value to eQCWWAN_ERR_NONE  */
                rtn = 0;
            }
            sleep(2);

        }
    }
    else
    {
        /* simple short sms */
        rtn = sms_SwiMakePdu( wcdmaentxmsg_reqParam.pDestAddr,
                              (int8_t*)escaped_msg,
                              escaped_len,
                              (int8_t*)pdu,
                              SMS_ASCII_STRING_SIZE-1,
                              0,
                              encodingScheme);
        if( eQCWWAN_ERR_NONE != rtn )
        {
            free(pdu);
            free(escaped_msg);
            return eQCWWAN_ERR_PDU_GENERATION;
        }
        sms_SwiAsc2Hex((uint8_t*)hexa,sizeof(hexa),
            (int8_t*)pdu,strlen((char*)pdu));

        /* Send SMS */
            pack_sms_SendSMS_t sendsms_reqParam;
            sendsms_reqParam.messageFormat = 0x06; //GW-PP
            sendsms_reqParam.messageSize = strlen(pdu)/2;
            sendsms_reqParam.pMessage = hexa;
            uint8_t linktimer  = 0x03;
            sendsms_reqParam.pLinktimer = &linktimer;
        
            printf("send sms  ...\n");
            rtn = pack_sms_SendSMS(&req_ctx, qmi_req, &qmi_req_len, &sendsms_reqParam);
            if (rtn)
                printf("packing error for send sms %d\n", rtn);
            rtn = write(sms, qmi_req, qmi_req_len);
            if (rtn != qmi_req_len)
                printf("write %d wrote %d\n", qmi_req_len, rtn);
            else
            {
                qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
                printf(">> sending %s\n", qmi_msg);
                dump_hex(qmi_req_len, qmi_req);
                /* reset return value to eQCWWAN_ERR_NONE  */
                rtn = 0;
            }
            sleep(2);
    }

    /* release pdu buffer memory */
    if(pdu)
        free(pdu);

    free(escaped_msg);
    return rtn;
}

int send_wcdma_sms_different_encodings()
{
    int  rtn = eQCWWAN_ERR_NONE;
    int i;
    uint8_t   hexa[SMS_ASCII_STRING_SIZE/2 + 1]={0};
    char   *pdu=NULL;
    char *escaped_msg = NULL;
    uint16_t escaped_len;
    uint8_t encodingScheme[2] = {8 /*UCS2 */, 4 /* 8BIT */};
    char msgtext[] = "qwrqtq!@@#$#$%%^%^&**asssossns";

    /* system will store unicode string in default UTF-8 format 
     * hex bytes in UTF-8 for a Japanese string */
    char utf8msg[] ={ 0xe4,0xbd,0x95,0xe3,0x81,0x8c,0xe4,0xbb,0xb2,0xe9,0x96,0x93,0xe3,
                     0x81,0xab,0xe8,0xb5,0xb7,0xe3,0x81,0x93,0xe3,0x81,0xa3,0xe3,0x81,
                     0xa6,0xe3,0x81,0x84,0xe3,0x82,0x8b};

    pack_qmi_t req_ctx;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len = QMI_MSG_MAX;
    const char *qmi_msg;

    if(sms<0)
        sms = client_fd(eSMS);
    if(sms<0)
    {
        fprintf(stderr,"SMS Service Not Supported!\n");
        return eQCWWAN_ERR_QMI_NOT_SUPPORTED;
    }
    memset(&sms_attr, 0, sizeof(sms_attr));
    memset(qmi_req, 0, QMI_MSG_MAX);
    enSmsThread = 1;
    pthread_create(&sms_tid, &sms_attr, sms_read_thread, NULL);
    sleep(1);
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = 10;


    pack_sms_SetNewSMSCallback_t  newsmscb_reqParam;
    newsmscb_reqParam.status  = LITEQMI_QMI_CBK_PARAM_SET;

    printf("set new sMS callback ...\n");
    rtn = pack_sms_SetNewSMSCallback(&req_ctx, qmi_req, &qmi_req_len, newsmscb_reqParam);
    rtn = write(sms, qmi_req, qmi_req_len);
    if (rtn != qmi_req_len)
    {
        printf("write %d wrote %d\n", qmi_req_len, rtn);
        return eQCWWAN_ERR_GENERAL;
    }
    else
    {
        qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(2);

    /* Allocate pdu buffer memory */
    pdu = (char*)malloc(SMS_ASCII_STRING_SIZE*sizeof(char));
    if( pdu == NULL )
    {
        return eQCWWAN_ERR_MEMORY ;
    }

    for (i= 0; i < ((int)sizeof(encodingScheme)); i++)
    {
        uint8_t encoding = 0xFF;
        sms_SLQSWCDMAEncodeMOTextMsg_t wcdmaentxmsg_reqParam;

        encoding = encodingScheme[i];
        memset(&wcdmaentxmsg_reqParam,0,sizeof(sms_SLQSWCDMAEncodeMOTextMsg_t));
        memset(pdu,0,(SMS_ASCII_STRING_SIZE*sizeof(char)));
        wcdmaentxmsg_reqParam.pDestAddr = (int8_t*)PHONE_NUM;

        if (encoding == UCSENCODING)
        {
            char  UCS2Msg[2048] = {'\0'};
            uint16_t inputStrLen = sizeof(utf8msg);
            uint16_t ucsBufferLen = sizeof(UCS2Msg);
            uint16_t ucsMsgLen = 0;
            uint8_t bRet = 0;
            if (!setlocale(LC_CTYPE, "")) 
            {
                fprintf(stderr, "Locale not specified for checking default encoding");
                free(pdu);
                return eQCWWAN_ERR_GENERAL;
            }
            if( strcmp(nl_langinfo(CODESET), "UTF-8"))
            {
                fprintf(stderr, "System default encoding scheme is not set as UTF-8\n");
                free(pdu);
                return eQCWWAN_ERR_GENERAL;
            }
            bRet = smsdemo_conversionUTFmsgToUCS2(utf8msg, inputStrLen, 
                                       UCS2Msg, ucsBufferLen, &ucsMsgLen);

            if(!bRet )
            {
                free(pdu);
                fprintf(stderr, "UTF-8 to UCS2 conversion failure\n");
                return eQCWWAN_ERR_GENERAL;
            }

            /* simple short sms */
            rtn = sms_SwiMakePdu( wcdmaentxmsg_reqParam.pDestAddr,
                              (int8_t *)UCS2Msg,
                              ucsMsgLen,
                              (int8_t*)pdu,
                              SMS_ASCII_STRING_SIZE-1,
                              0,
                              encoding);

            if( sMS_HELPER_OK != rtn )
            {
                free(pdu);
                return eQCWWAN_ERR_PDU_GENERATION;
            }
        }
        else if (encoding == EIGHTBITENCODING)
        {
            wcdmaentxmsg_reqParam.pTextMsg  = (int8_t *)msgtext;
            wcdmaentxmsg_reqParam.messageSize = strlen(msgtext);

            if (wcdmaentxmsg_reqParam.messageSize <= 0)
            {
                printf("invalid message size!\n");
                free(pdu);
                return eQCWWAN_ERR_BUFFER_SZ;
            }
            escaped_msg = (char*)malloc(2 * wcdmaentxmsg_reqParam.messageSize);
            if( escaped_msg == NULL )
            {
                printf("%s: failed to allocated %d escape buffer\n", __func__,
                    2*wcdmaentxmsg_reqParam.messageSize);
                free(pdu);
                return eQCWWAN_ERR_MEMORY ;
            }
            escaped_len = 2*wcdmaentxmsg_reqParam.messageSize;

            sms_ConvertAsciiToGSMCharacter((char*)wcdmaentxmsg_reqParam.pTextMsg, 
                               wcdmaentxmsg_reqParam.messageSize,
                               escaped_msg, &escaped_len);

            /* simple short sms */
            rtn = sms_SwiMakePdu( wcdmaentxmsg_reqParam.pDestAddr,
                              (int8_t*)escaped_msg,
                              escaped_len,
                              (int8_t*)pdu,
                              SMS_ASCII_STRING_SIZE-1,
                              0,
                              encoding);
            free(escaped_msg);
            escaped_msg = NULL;
            if( sMS_HELPER_OK != rtn )
            {
                free(pdu);
                return eQCWWAN_ERR_PDU_GENERATION;
            }
        } 
        sms_SwiAsc2Hex((uint8_t*)hexa,sizeof(hexa), 
            (int8_t*)pdu,strlen((char*)pdu));

        pack_sms_SendSMS_t sendsms_reqParam;
        sendsms_reqParam.messageFormat = 0x06; /*GW-PP*/
        sendsms_reqParam.messageSize = strlen(pdu)/2;
        sendsms_reqParam.pMessage = hexa;
        uint8_t linktimer  = 0x03;
        sendsms_reqParam.pLinktimer = &linktimer;
        
        printf("send sms  ...\n");
        rtn = pack_sms_SendSMS(&req_ctx, qmi_req, &qmi_req_len, &sendsms_reqParam);
        if (rtn )
        {
            printf("packing error for send sms %d\n", rtn);
        }
        rtn = write(sms, qmi_req, qmi_req_len);
        if (rtn != qmi_req_len)
            printf("write %d wrote %d\n", qmi_req_len, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSMS, qmi_req, qmi_req_len);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(qmi_req_len, qmi_req);
            /* reset return value to eQCWWAN_ERR_NONE  */
            rtn = 0;
        }
        sleep(2);
    }

    /* release pdu buffer memory */
    if(pdu)
        free(pdu);
    return rtn;
}

void sms_test_ind_exit()
{
    if(enSmsThread==0)
    {
        if(sms>=0)
           close(sms);
        sms = -1;
        return ;
    }
    enSmsThread = 0;
    printf("\nkilling SMS read thread...\n");
#ifdef __ARM_EABI__    
    if(sms>=0)
           close(sms);
        sms = -1;
    void *pthread_rtn_value;
    if(sms_tid!=0)
    pthread_join(sms_tid, &pthread_rtn_value);
#endif
    if(sms_tid!=0)
    pthread_cancel(sms_tid);
    sms_tid = 0;
    if(sms>=0)
       close(sms);
    sms = -1;
}

void dump_SetNewSMSCallback_ind(void* ptr)
{
    unpack_sms_SetNewSMSCallback_ind_t  *result =
        (unpack_sms_SetNewSMSCallback_ind_t*) ptr;
    int Length = 0;
    int lIndex = 0;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf ("SMS IND unpacked: %d\n", unpackRetCode);
    if ((result->NewMMTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf ("NewMMTlv.MTMessageInfo.storageType : %d\n", result->NewMMTlv.MTMessageInfo.storageType);
        printf ("NewMMTlv.MTMessageInfo.messageIndex: %d\n", result->NewMMTlv.MTMessageInfo.messageIndex);

    }
    if ((result->MMTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf ("MMTlv.MessageModeInfo.messageMode : %d\n", result->MMTlv.MessageModeInfo.messageMode);
    }
    if ((result->ETWSTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf ("ETWSTlv.EtwsMessageInfo.notificationType : %d\n", result->ETWSTlv.EtwsMessageInfo.notificationType);
        printf ("ETWSTlv.EtwsMessageInfo.length : %d\n", result->ETWSTlv.EtwsMessageInfo.length);
        Length = result->ETWSTlv.EtwsMessageInfo.length;
        lIndex = 0;
        while(Length--)
        {
            fprintf ( stderr,
                "%2xH",
                result->ETWSTlv.EtwsMessageInfo.data[lIndex++] );
                if (lIndex % 16 ==0)
                {
                        fprintf ( stderr, "\n");
                }
        }
        fprintf ( stderr, "\r\n\n" );
    }
    if ((result->TRMessageTlv.TlvPresent)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator : %d\n", result->TRMessageTlv.TransferRouteMTMessageInfo.ackIndicator);
        printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.transactionID : %d\n", result->TRMessageTlv.TransferRouteMTMessageInfo.transactionID);
        printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.format : %d\n", result->TRMessageTlv.TransferRouteMTMessageInfo.format);
        printf ("TRMessageTlv.TRMessageTlv.TransferRouteMTMessageInfo.length : %d\n", result->TRMessageTlv.TransferRouteMTMessageInfo.length);
        Length = result->TRMessageTlv.TransferRouteMTMessageInfo.length;
        lIndex = 0;
        while(Length--)
        {
            fprintf ( stderr,
                "%2xH",
                result->TRMessageTlv.TransferRouteMTMessageInfo.data[lIndex++] );
                if (lIndex % 16 ==0)
                {
                        fprintf ( stderr, "\n");
                }
        }
        fprintf ( stderr, "\r\n\n" );
    }
    if ((result->ETWSPLMNTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode : %d\n", result->ETWSPLMNTlv.ETWSPLMNInfo.mobileCountryCode);
        printf ("ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode : %d\n", result->ETWSPLMNTlv.ETWSPLMNInfo.mobileNetworkCode);
    }
    if ((result->SMSCTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf ("SMSCTlv.SMSCInfo.length : %d\n", result->SMSCTlv.SMSCInfo.length);
        Length = result->SMSCTlv.SMSCInfo.length;
        lIndex = 0;
        while(Length--)
        {
            fprintf ( stderr,
                "%2xH",
                result->SMSCTlv.SMSCInfo.data[lIndex++] );
        }
        fprintf ( stderr, "\r\n\n" );
    }
    if ((result->IMSTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 22)))
    {
        printf ("IMSTlv.IMSInfo.smsOnIMS : %d\n", result->IMSTlv.IMSInfo.smsOnIMS);
    }
}

void dump_SLQSWmsMemoryFullCallBack_ind(void* ptr)
{
    unpack_sms_SLQSWmsMemoryFullCallBack_ind_t  *result =
        (unpack_sms_SLQSWmsMemoryFullCallBack_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("sms store full, storage type/message mode : %d/%d\n",
                result->storageType, result->messageMode);
}

void dump_SLQSTransLayerInfoCallback_ind(void* ptr)
{
    unpack_sms_SLQSTransLayerInfoCallback_ind_t  *result =
        (unpack_sms_SLQSTransLayerInfoCallback_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
       printf("Registration Indication 0x%x\n",result->regInd);
    if ( (result->pTransLayerInfo) &&
         (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Transport Layer Information:\n" );
        printf("Transport Type: 0x%x\n",
                result->pTransLayerInfo->TransType );
        printf("Transport Capabilities 0x%x\n",
                result->pTransLayerInfo->TransCap );
    }
}

void dump_SLQSNWRegInfoCallback_ind(void* ptr)
{
    unpack_sms_SLQSNWRegInfoCallback_ind_t  *result =
        (unpack_sms_SLQSNWRegInfoCallback_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Network Registration Status 0x%x\n",result->NWRegStat);
}


void dump_SLQSWmsMessageWaitingCallBack_ind(void* ptr)
{
    unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t  *result =
        (unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        uint8_t i;
        printf("\nreceive Message Waiting Info Indication\n");
        for ( i = 0; i < result->numInstances; i++)
        {
            printf("message type: %d\n", result->msgWaitInfo[i].msgType);
            printf("active indication: %d\n", result->msgWaitInfo[i].activeInd);
            printf("message count: %d\n", result->msgWaitInfo[i].msgCount);
            printf("\n");
        }
    }
}

void dump_SLQSWmsAsyncRawSendCallBack_ind(void* ptr)
{
    unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t  *result =
        (unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("Send Status     : %d\n", result->sendStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Message Id      : %d\n", result->messageID);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("Cause Code      : %d\n", result->causeCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("Error Class     : %d\n", result->errorClass);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("RP Cause        : %d\n", result->RPCause);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("TP Cause        : %d\n", result->TPCause);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    {
        printf("Alpha Id Length : %d\n", result->alphaIDLen);
        if (result->alphaIDLen)
            printf("Alpha Id: %s\n", result->pAlphaID);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    printf("User Data       : %u\n", result->userData);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    printf("Message Delivery Failure Type  : %d\n", result->msgDelFailureType);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    printf("Message Delivery Failure Cause : %d\n", result->msgDelFailureCause);
    printf("\n");
}
/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_sms_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_WMS_RAW_READ */
    {0x02,0x01,0x00,0x22,0x00,0x18,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x0E,0x00,0x01,0x06,0x0A,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A},

    /* eQMI_WMS_GET_MSG_LIST */
    {0x02,0x02,0x00,0x31,0x00,0x13,0x00,
    0x02,0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x09,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,},

    /* eQMI_WMS_MODIFY_TAG */
    {0x02,0x03,0x00,0x23,0x00,0x7,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_DELETE */
    {0x02,0x04,0x00,0x24,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_RAW_SEND */
    {0x02,0x05,0x00,0x20,0x00,0x11,0x00,
        0x02,0x04,0x00,0x00,0x00,0x00,0x00,
        0x01,0x02,0x00,0x01,0x00,
        0x10,0x02,0x00,0x03,0x00},

    /* eQMI_WMS_SET_EVENT */
    {0x02,0x06,0x00,0x01,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_EVENT_IND */
    {0x04,0x07,0x00,0x01,0x00,0x10,0x00,
            0x10,0x05,0x00,0x00,0x01,0x00,0x00,0x00,
            0x12,0x01,0x00,0x01,
            0x16,0x01,0x00,0x00},

    /* eQMI_WMS_MEMORY_FULL_IND */
    {0x04,0x08,0x00,0x3F,0x00,0x10,0x00,
            0x01,0x02,0x10,0x01},

    /* eQMI_WMS_GET_SMSC_ADDR */
    {0x02,0x09,0x00,0x34,0x00,0x1B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x11,0x00,0x31,0x34,0x35,0x0D,0x2B,0x39,
                0x31,0x39,0x38,0x38,0x38,0x30,0x30,0x39,0x39,0x39,0x38},

    /* eQMI_WMS_SET_SMSC_ADDR */
    {0x02,0x0A,0x00,0x35,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_RAW_WRITE */
    {0x02,0x0B,0x00,0x21,0x00,0x0E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_WMS_GET_BC_CONFIG */
    {0x02,0x0C,0x00,0x3e,0x00,0x1c,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x12,0x00,0x01,0x03,0x00,0x00,0x11,0x07,0x11,0x01,
                0x12,0x11,0x2f,0x11,0x01,0x00,0x00,0xff,0xff,0x01},

    /* eQMI_WMS_SET_BC_CONFIG */
    {0x02,0x0D,0x00,0x3D,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_SET_BC_ACTIVATION */
    {0x02,0x0E,0x00,0x3C,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_GET_TRANS_LAYER_INFO */
    {0x02,0x0F,0x00,0x48,0x00,0x10,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x01,
            0x11,0x02,0x00,0x00,0x01},

    /* eQMI_WMS_GET_TRANS_NW_REG_INFO */
    {0x02,0x10,0x00,0x4A,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x01},

    /* eQMI_WMS_GET_IND_REGISTER */
    {0x02,0x11,0x00,0x4D,0x00,0x13,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x01,
            0x11,0x01,0x00,0x01,
            0x12,0x01,0x00,0x01},

    /* eQMI_WMS_SET_IND_REGISTER */
    {0x02,0x12,0x00,0x47,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_SET_ROUTES */
    {0x02,0x13,0x00,0x32,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_GET_MSG_PROTOCOL */
    {0x02,0x14,0x00,0x30,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x01,0x00,0x01},

    /* eQMI_WMS_GET_MSG_LIST_MAX */
    {0x02,0x15,0x00,0x36,0x00,0x15,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x10,0x00,0x00,0x00,
            0x10,0x04,0x00,0x05,0x00,0x00,0x00},

    /* eQMI_WMS_GET_MSG_WAITING */
    {0x02,0x16,0x00,0x43,0x00,0x0E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x01,0x02,0x01,0x05},

    /* eQMI_WMS_ASYNC_RAW_SEND */
    {0x02,0x17,0x00,0x59,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_SWI_SET_SMS_STORAGE */
    {0x02,0x18,0x00,0x57,0x55,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WMS_SWI_GET_SMS_STORAGE */
    {0x02,0x19,0x00,0x56,0x55,0x0E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_WMS_TRANS_LAYER_INFO_IND */
    {0x04,0x1A,0x00,0x49,0x00,0x09,0x00,
            0x01,0x01,0x00,0x01,
            0x10,0x02,0x00,0x00,0x01},

    /* eQMI_WMS_TRANS_NW_REG_INFO_IND */
    {0x04,0x1B,0x00,0x4B,0x00,0x04,0x00,
            0x01,0x01,0x00,0x01},

    /* eQMI_WMS_MSG_WAITING_IND */
    {0x04,0x1C,0x00,0x44,0x00,0x07,0x00,
            0x01,0x04,0x00,0x01,0x02,0x01,0x06},

    /* eQMI_WMS_ASYNC_RAW_SEND_IND */
    {0x04,0x1D,0x00,0x59,0x00,0x2F,0x00,
            0x01,0x02,0x00,0x00,0x00,
            0x10,0x02,0x00,0x01,0x00,
            0x11,0x02,0x00,0x22,0x00,
            0x12,0x01,0x00,0x00,
            0x13,0x03,0x00,0x0B,0x00,0x92,
            0x14,0x01,0x00,0x00,
            0x15,0x01,0x00,0x00,
            0x16,0x04,0x00,0x03,0x31,0x32,0x33,
            0x17,0x04,0x00,0x11,0x22,0x33,0x44},

};

    /* eQMI_WMS_RAW_READ */
const unpack_sms_SLQSGetSMS_t const_unpack_sms_SLQSGetSMS_t = {
        0x01,0x06,0x000A,{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A},
            {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

    /* eQMI_WMS_RAW_READ */
unpack_sms_SLQSGetSMS_t var_unpack_sms_SLQSGetSMS_t = {
    0xFF,0xFF,0x00FF,{0},{{0}} };

    /* eQMI_WMS_GET_MSG_LIST */
const unpack_sms_SLQSGetSMSList_t const_unpack_sms_SLQSGetSMSList_t = {
    0x01,{{0x01,0x01}}, {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };
    /* eQMI_WMS_GET_MSG_LIST */
unpack_sms_SLQSGetSMSList_t var_unpack_sms_SLQSGetSMSList_t = {
0xFF,{{0,0}}, {{0}} };

    /* eQMI_WMS_MODIFY_TAG */
const unpack_sms_SLQSModifySMSStatus_t const_unpack_sms_SLQSModifySMSStatus_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_DELETE */
const unpack_sms_SLQSDeleteSMS_t const_unpack_sms_SLQSDeleteSMS_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_RAW_SEND */
const unpack_sms_SendSMS_t const_unpack_sms_SendSMS_t = {0x01,0x03,
{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,1,16)}} };

    /* eQMI_WMS_SET_EVENT */
const unpack_sms_SetNewSMSCallback_t const_unpack_sms_SetNewSMSCallback_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_EVENT_IND */
const unpack_sms_SetNewSMSCallback_ind_t const_unpack_sms_SetNewSMSCallback_ind_t = {
{1,{0x00,0x01}},{0,{0,0,0,0,{0}}},{1,{0x01}},{0,{0,0,{0}}},{0,{0,0}},{0,{0,{0}}},{1,{0}},
    {{SWI_UINT256_BIT_VALUE(SET_3_BITS,16,18,22)}} };

    /* eQMI_WMS_MEMORY_FULL_IND */
const unpack_sms_SLQSWmsMemoryFullCallBack_ind_t const_unpack_sms_SLQSWmsMemoryFullCallBack_ind_t = {
0x01,0x01,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

uint8_t cst_SMSCAddress[] = {0x2B,0x39,0x31,0x39,0x38,0x38,0x38,0x30,0x30,0x39,0x39,0x39,0x38};
uint8_t cst_SMSCType[] = {0x31,0x34,0x35};
    /* eQMI_WMS_GET_SMSC_ADDR */
const unpack_sms_GetSMSCAddress_t const_unpack_sms_GetSMSCAddress_t = {0x0D,cst_SMSCAddress,
0x03,cst_SMSCType,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint8_t var_SMSCAddress[20];
uint8_t var_SMSCType[5];
    /* eQMI_WMS_GET_SMSC_ADDR */
unpack_sms_GetSMSCAddress_t var_unpack_sms_GetSMSCAddress_t = {0xFF,var_SMSCAddress,
0xFF,var_SMSCType,{{0}} };

    /* eQMI_WMS_SET_SMSC_ADDR */
const unpack_sms_SetSMSCAddress_t const_unpack_sms_SetSMSCAddress_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_saveMessageIndex = 0x00000001;
    /* eQMI_WMS_RAW_WRITE */
const unpack_sms_SaveSMS_t const_unpack_sms_SaveSMS_t = {&cst_saveMessageIndex,
{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

uint32_t var_saveMessageIndex;
    /* eQMI_WMS_RAW_WRITE */
unpack_sms_SaveSMS_t var_unpack_sms_SaveSMS_t = {&var_saveMessageIndex,{{0}} };

sms_qaQmi3GPPBroadcastCfgInfo  cst_BroadcastConfig = {0x01,0x0003,
        {{0x1100,0x1107,0x01},{0x1112,0x112F,0x01},{0x0000,0xFFFF,0x01}}};
     /* eQMI_WMS_GET_BC_CONFIG */
const unpack_sms_SLQSGetSmsBroadcastConfig_t const_unpack_sms_SLQSGetSmsBroadcastConfig_t = {&cst_BroadcastConfig,
        NULL,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

sms_qaQmi3GPPBroadcastCfgInfo  var_BroadcastConfig;
    /* eQMI_WMS_GET_BC_CONFIG */
unpack_sms_SLQSGetSmsBroadcastConfig_t var_unpack_sms_SLQSGetSmsBroadcastConfig_t = {&var_BroadcastConfig,NULL,
{{0}} };

    /* eQMI_WMS_SET_BC_CONFIG */
const unpack_sms_SLQSSetSmsBroadcastConfig_t const_unpack_sms_SLQSSetSmsBroadcastConfig_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_SET_BC_ACTIVATION */
const unpack_sms_SLQSSetSmsBroadcastActivation_t const_unpack_sms_SLQSSetSmsBroadcastActivation_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint8_t            cst_RegInd = 0x01 ;
sms_transLayerInfo cst_TransLayerInfo = {0x00,0x01};
sms_getTransLayerInfo  cst_GetTransLayerInfo ={&cst_RegInd,&cst_TransLayerInfo};
    /* eQMI_WMS_GET_TRANS_LAYER_INFO */
const unpack_sms_SLQSGetTransLayerInfo_t const_unpack_sms_SLQSGetTransLayerInfo_t = {
&cst_GetTransLayerInfo,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

uint8_t            var_RegInd;
sms_transLayerInfo var_TransLayerInfo;
sms_getTransLayerInfo  var_GetTransLayerInfo ={&var_RegInd,&var_TransLayerInfo};
    /* eQMI_WMS_GET_TRANS_LAYER_INFO */
unpack_sms_SLQSGetTransLayerInfo_t var_unpack_sms_SLQSGetTransLayerInfo_t = {
&var_GetTransLayerInfo,{{0}} };

uint8_t            cst_transNwRegInd = 0x01 ;
sms_getTransNWRegInfo cst_GetTransNWRegInfo = {&cst_transNwRegInd};
    /* eQMI_WMS_GET_TRANS_NW_REG_INFO */
const unpack_sms_SLQSGetTransNWRegInfo_t const_unpack_sms_SLQSGetTransNWRegInfo_t = {
&cst_GetTransNWRegInfo,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t            var_transNwRegInd;
sms_getTransNWRegInfo var_GetTransNWRegInfo = {&var_transNwRegInd};
    /* eQMI_WMS_GET_TRANS_NW_REG_INFO */
unpack_sms_SLQSGetTransNWRegInfo_t var_unpack_sms_SLQSGetTransNWRegInfo_t = {
&var_GetTransNWRegInfo,{{0}} };

uint8_t cst_RegTransLayerInfoEvt = 0x01;
uint8_t cst_RegTransNWRegInfoEvt = 0x01;
uint8_t cst_RegCallStatInfoEvt = 0x01;
sms_getIndicationReg cst_GetIndicationRegInfo = {&cst_RegTransLayerInfoEvt,
        &cst_RegTransNWRegInfoEvt,&cst_RegCallStatInfoEvt};

    /* eQMI_WMS_GET_IND_REGISTER */
const unpack_sms_SLQSGetIndicationRegister_t const_unpack_sms_SLQSGetIndicationRegister_t = {
&cst_GetIndicationRegInfo,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

uint8_t var_RegTransLayerInfoEvt;
uint8_t var_RegTransNWRegInfoEvt;
uint8_t var_RegCallStatInfoEvt;
sms_getIndicationReg var_GetIndicationRegInfo = {&var_RegTransLayerInfoEvt,
        &var_RegTransNWRegInfoEvt,&var_RegCallStatInfoEvt};

    /* eQMI_WMS_GET_IND_REGISTER */
unpack_sms_SLQSGetIndicationRegister_t var_unpack_sms_SLQSGetIndicationRegister_t = {
&var_GetIndicationRegInfo,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

    /* eQMI_WMS_SET_IND_REGISTER */
const unpack_sms_SLQSSetIndicationRegister_t const_unpack_sms_SLQSSetIndicationRegister_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_SET_ROUTES */
const unpack_sms_SLQSSmsSetRoutes_t const_unpack_sms_SLQSSmsSetRoutes_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

sms_msgProtocolResp cst_MessageProtocol = {0x01};
    /* eQMI_WMS_GET_MSG_PROTOCOL */
const unpack_sms_SLQSSmsGetMessageProtocol_t const_unpack_sms_SLQSSmsGetMessageProtocol_t = {
&cst_MessageProtocol,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

sms_msgProtocolResp var_MessageProtocol;
    /* eQMI_WMS_GET_MSG_PROTOCOL */
unpack_sms_SLQSSmsGetMessageProtocol_t var_unpack_sms_SLQSSmsGetMessageProtocol_t = {
&var_MessageProtocol,{{0}} };

sms_maxStorageSizeResp cst_MaxStorageSizeResp = {0x10,0x05};
    /* eQMI_WMS_GET_MSG_LIST_MAX */
const unpack_sms_SLQSSmsGetMaxStorageSize_t const_unpack_sms_SLQSSmsGetMaxStorageSize_t = {
&cst_MaxStorageSizeResp,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,1,16)}} };

sms_maxStorageSizeResp var_MaxStorageSizeResp;
    /* eQMI_WMS_GET_MSG_LIST_MAX */
unpack_sms_SLQSSmsGetMaxStorageSize_t var_unpack_sms_SLQSSmsGetMaxStorageSize_t = {
&var_MaxStorageSizeResp,{{0}} };

sms_getMsgWaitingInfo  cst_GetMsgWaitingInfoResp = {0x01,{{0x02,0x01,0x05}}};
    /* eQMI_WMS_GET_MSG_WAITING */
const unpack_sms_SLQSGetMessageWaiting_t const_unpack_sms_SLQSGetMessageWaiting_t = {
&cst_GetMsgWaitingInfoResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

sms_getMsgWaitingInfo  var_GetMsgWaitingInfoResp;
    /* eQMI_WMS_GET_MSG_WAITING */
unpack_sms_SLQSGetMessageWaiting_t var_unpack_sms_SLQSGetMessageWaiting_t = {
&var_GetMsgWaitingInfoResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

    /* eQMI_WMS_ASYNC_RAW_SEND */
const unpack_sms_SLQSSendAsyncSMS_t const_unpack_sms_SLQSSendAsyncSMS_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_WMS_SWI_SET_SMS_STORAGE */
const unpack_sms_SLQSSetSmsStorage_t const_unpack_sms_SLQSSetSmsStorage_t = {
{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_SmsStorage = 0x01;
    /* eQMI_WMS_SWI_GET_SMS_STORAGE */
const unpack_sms_SLQSSwiGetSMSStorage_t const_unpack_sms_SLQSSwiGetSMSStorage_t = {
&cst_SmsStorage,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint32_t var_SmsStorage;
    /* eQMI_WMS_SWI_GET_SMS_STORAGE */
unpack_sms_SLQSSwiGetSMSStorage_t var_unpack_sms_SLQSSwiGetSMSStorage_t = {
&var_SmsStorage,{{0}} };

sms_transLayerInfo cst_TransLayerInfoInd = {0x00,0x01};
    /* eQMI_WMS_TRANS_LAYER_INFO_IND */
const unpack_sms_SLQSTransLayerInfoCallback_ind_t const_unpack_sms_SLQSTransLayerInfoCallback_ind_t = {
0x01,&cst_TransLayerInfoInd,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

sms_transLayerInfo var_TransLayerInfoInd;
    /* eQMI_WMS_TRANS_LAYER_INFO_IND */
unpack_sms_SLQSTransLayerInfoCallback_ind_t var_unpack_sms_SLQSTransLayerInfoCallback_ind_t = {
0xFF,&var_TransLayerInfoInd,{{0}} };

    /* eQMI_WMS_TRANS_NW_REG_INFO_IND */
const unpack_sms_SLQSNWRegInfoCallback_ind_t const_unpack_sms_SLQSNWRegInfoCallback_ind_t = {
0x01,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

    /* eQMI_WMS_MSG_WAITING_IND */
const unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t const_unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t = {
0x01,{{0x02,0x01,0x06}},{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

uint8_t cst_AlphaID[] = {0x31,0x32,0x33};
    /* eQMI_WMS_ASYNC_RAW_SEND_IND */
const unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t const_unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t = {
0x00,0x01,0x22,0x00,0x0B,0x92,0x00,0x00,0x03,cst_AlphaID,0x44332211,
{{SWI_UINT256_BIT_VALUE(SET_9_BITS,1,16,17,18,19,20,21,22,23)}} };

uint8_t var_AlphaID[5];
    /* eQMI_WMS_ASYNC_RAW_SEND_IND */
unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t var_unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t = {
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,var_AlphaID,0xFF,{{0}} };

int sms_validate_dummy_unpack()
{
    msgbuf msg;
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    int rtn;
    ssize_t rlen;
    pack_qmi_t req_ctx;
    int loopCount = 0;
    int index = 0;
    int unpackRetCode;

    memset(&req_ctx, 0, sizeof(req_ctx));
    loopCount = sizeof(validate_sms_resp_msg)/sizeof(validate_sms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_sms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_WMS_RAW_READ:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_sms_SLQSGetSMS_t *varp = &var_unpack_sms_SLQSGetSMS_t;
                    const unpack_sms_SLQSGetSMS_t *cstp = &const_unpack_sms_SLQSGetSMS_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSGetSMS,
                    dump_SLQSGetSMS,
                    msg.buf,
                    rlen,
                    &var_unpack_sms_SLQSGetSMS_t,
                    5,
                    CMP_PTR_TYPE, &varp->messageTag, &cstp->messageTag,
                    CMP_PTR_TYPE, &varp->messageFormat, &cstp->messageFormat,
                    CMP_PTR_TYPE, &varp->messageSize, &cstp->messageSize,
                    CMP_PTR_TYPE, varp->message, cstp->message,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WMS_GET_MSG_LIST:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_sms_SLQSGetSMSList_t *varp = &var_unpack_sms_SLQSGetSMSList_t;
                    const unpack_sms_SLQSGetSMSList_t *cstp = &const_unpack_sms_SLQSGetSMSList_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSGetSMSList,
                    dump_SLQSGetSMSList,
                    msg.buf,
                    rlen,
                    &var_unpack_sms_SLQSGetSMSList_t,
                    3,
                    CMP_PTR_TYPE, &varp->messageListSize, &cstp->messageListSize,
                    CMP_PTR_TYPE, varp->messageList, cstp->messageList,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WMS_MODIFY_TAG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSModifySMSStatus,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSModifySMSStatus_t);
                }
                break;
            case eQMI_WMS_DELETE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSDeleteSMS,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSDeleteSMS_t);
                }
                break;
            case eQMI_WMS_RAW_SEND:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SendSMS,
                    dump_SLQSSendSMS,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SendSMS_t);
                }
                break;
            case eQMI_WMS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SetNewSMSCallback,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SetNewSMSCallback_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SetNewSMSCallback_ind,
                    dump_SetNewSMSCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SetNewSMSCallback_ind_t);
                }
                break;
            case eQMI_WMS_MEMORY_FULL_IND:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSWmsMemoryFullCallBack_ind,
                    dump_SLQSWmsMemoryFullCallBack_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSWmsMemoryFullCallBack_ind_t);
                }
                break;
            case eQMI_WMS_GET_SMSC_ADDR:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_GetSMSCAddress_t *varp = &var_unpack_sms_GetSMSCAddress_t;
                     const unpack_sms_GetSMSCAddress_t *cstp = &const_unpack_sms_GetSMSCAddress_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_GetSMSCAddress,
                     dump_GetSMSCAddress,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_GetSMSCAddress_t,
                     3,
                     CMP_PTR_TYPE, varp->pSMSCAddress, cstp->pSMSCAddress,
                     CMP_PTR_TYPE, varp->pSMSCType, cstp->pSMSCType,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_SET_SMSC_ADDR:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SetSMSCAddress,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SetSMSCAddress_t);
                }
                break;
            case eQMI_WMS_RAW_WRITE:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SaveSMS_t *varp = &var_unpack_sms_SaveSMS_t;
                     const unpack_sms_SaveSMS_t *cstp = &const_unpack_sms_SaveSMS_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SaveSMS,
                     dump_SaveSMS,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SaveSMS_t,
                     2,
                     CMP_PTR_TYPE, varp->pMessageIndex, cstp->pMessageIndex,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_GET_BC_CONFIG:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSGetSmsBroadcastConfig_t *varp = &var_unpack_sms_SLQSGetSmsBroadcastConfig_t;
                     const unpack_sms_SLQSGetSmsBroadcastConfig_t *cstp = &const_unpack_sms_SLQSGetSmsBroadcastConfig_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSGetSmsBroadcastConfig,
                     dump_SLQSGetSmsBroadcastConfig,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSGetSmsBroadcastConfig_t,
                     2,
                     CMP_PTR_TYPE, varp->pBroadcastConfig, cstp->pBroadcastConfig,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_SET_BC_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSetSmsBroadcastConfig,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSetSmsBroadcastConfig_t);
                }
                break;
            case eQMI_WMS_SET_BC_ACTIVATION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSetSmsBroadcastActivation,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSetSmsBroadcastActivation_t);
                }
                break;
            case eQMI_WMS_GET_TRANS_LAYER_INFO:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSGetTransLayerInfo_t *varp = &var_unpack_sms_SLQSGetTransLayerInfo_t;
                     const unpack_sms_SLQSGetTransLayerInfo_t *cstp = &const_unpack_sms_SLQSGetTransLayerInfo_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSGetTransLayerInfo,
                     dump_SLQSGetTransLayerInfo,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSGetTransLayerInfo_t,
                     3,
                     CMP_PTR_TYPE, varp->pGetTransLayerInfo->pRegInd, cstp->pGetTransLayerInfo->pRegInd,
                     CMP_PTR_TYPE, varp->pGetTransLayerInfo->pTransLayerInfo, cstp->pGetTransLayerInfo->pTransLayerInfo,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_GET_TRANS_NW_REG_INFO:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSGetTransNWRegInfo_t *varp = &var_unpack_sms_SLQSGetTransNWRegInfo_t;
                     const unpack_sms_SLQSGetTransNWRegInfo_t *cstp = &const_unpack_sms_SLQSGetTransNWRegInfo_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSGetTransNWRegInfo,
                     dump_SLQSGetTransNWRegInfo,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSGetTransNWRegInfo_t,
                     2,
                     CMP_PTR_TYPE, varp->pGetTransNWRegInfo->pRegStatus, cstp->pGetTransNWRegInfo->pRegStatus,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_GET_IND_REGISTER:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSGetIndicationRegister_t *varp = &var_unpack_sms_SLQSGetIndicationRegister_t;
                     const unpack_sms_SLQSGetIndicationRegister_t *cstp = &const_unpack_sms_SLQSGetIndicationRegister_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSGetIndicationRegister,
                     dump_SLQSGetIndicationRegister,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSGetIndicationRegister_t,
                     4,
                     CMP_PTR_TYPE, varp->pGetIndicationRegInfo->pRegTransLayerInfoEvt, cstp->pGetIndicationRegInfo->pRegTransLayerInfoEvt,
                     CMP_PTR_TYPE, varp->pGetIndicationRegInfo->pRegTransNWRegInfoEvt, cstp->pGetIndicationRegInfo->pRegTransNWRegInfoEvt,
                     CMP_PTR_TYPE, varp->pGetIndicationRegInfo->pRegCallStatInfoEvt, cstp->pGetIndicationRegInfo->pRegCallStatInfoEvt,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_SET_IND_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSetIndicationRegister,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSetIndicationRegister_t);
                }
                break;
            case eQMI_WMS_SET_ROUTES:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSmsSetRoutes,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSmsSetRoutes_t);
                }
                break;
            case eQMI_WMS_GET_MSG_PROTOCOL:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSSmsGetMessageProtocol_t *varp = &var_unpack_sms_SLQSSmsGetMessageProtocol_t;
                     const unpack_sms_SLQSSmsGetMessageProtocol_t *cstp = &const_unpack_sms_SLQSSmsGetMessageProtocol_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSSmsGetMessageProtocol,
                     dump_SLQSSmsGetMessageProtocol,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSSmsGetMessageProtocol_t,
                     2,
                     CMP_PTR_TYPE, varp->pMessageProtocol, cstp->pMessageProtocol,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_GET_MSG_LIST_MAX:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSSmsGetMaxStorageSize_t *varp = &var_unpack_sms_SLQSSmsGetMaxStorageSize_t;
                     const unpack_sms_SLQSSmsGetMaxStorageSize_t *cstp = &const_unpack_sms_SLQSSmsGetMaxStorageSize_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSSmsGetMaxStorageSize,
                     dump_SLQSSmsGetMaxStorageSize,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSSmsGetMaxStorageSize_t,
                     2,
                     CMP_PTR_TYPE, varp->pMaxStorageSizeResp, cstp->pMaxStorageSizeResp,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_GET_MSG_WAITING:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSGetMessageWaiting_t *varp = &var_unpack_sms_SLQSGetMessageWaiting_t;
                     const unpack_sms_SLQSGetMessageWaiting_t *cstp = &const_unpack_sms_SLQSGetMessageWaiting_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSGetMessageWaiting,
                     dump_SLQSGetMessageWaiting,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSGetMessageWaiting_t,
                     2,
                     CMP_PTR_TYPE, varp->pGetMsgWaitingInfoResp, cstp->pGetMsgWaitingInfoResp,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_ASYNC_RAW_SEND:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSendAsyncSMS,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSendAsyncSMS_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t *varp = &var_unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t;
                    const unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t *cstp = &const_unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSWmsAsyncRawSendCallBack_ind,
                    dump_SLQSWmsAsyncRawSendCallBack_ind,
                    msg.buf,
                    rlen,
                    &var_unpack_sms_SLQSWmsAsyncRawSendCallBack_ind_t,
                    12,
                    CMP_PTR_TYPE, &varp->sendStatus, &cstp->sendStatus,
                    CMP_PTR_TYPE, &varp->messageID, &cstp->messageID,
                    CMP_PTR_TYPE, &varp->causeCode, &cstp->causeCode,
                    CMP_PTR_TYPE, &varp->errorClass, &cstp->errorClass,
                    CMP_PTR_TYPE, &varp->RPCause, &cstp->RPCause,
                    CMP_PTR_TYPE, &varp->TPCause, &cstp->TPCause,
                    CMP_PTR_TYPE, &varp->msgDelFailureType, &cstp->msgDelFailureType,
                    CMP_PTR_TYPE, &varp->msgDelFailureCause, &cstp->msgDelFailureCause,
                    CMP_PTR_TYPE, &varp->alphaIDLen, &cstp->alphaIDLen,
                    CMP_PTR_TYPE, varp->pAlphaID, cstp->pAlphaID,
                    CMP_PTR_TYPE, &varp->userData, &cstp->userData,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WMS_SWI_SET_SMS_STORAGE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSSetSmsStorage,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSSetSmsStorage_t);
                }
                break;
            case eQMI_WMS_SWI_GET_SMS_STORAGE:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_sms_SLQSSwiGetSMSStorage_t *varp = &var_unpack_sms_SLQSSwiGetSMSStorage_t;
                     const unpack_sms_SLQSSwiGetSMSStorage_t *cstp = &const_unpack_sms_SLQSSwiGetSMSStorage_t;
                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSSwiGetSMSStorage,
                     dump_SLQSSwiGetSMSStorage,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSSwiGetSMSStorage_t,
                     2,
                     CMP_PTR_TYPE, varp->pSmsStorage, cstp->pSmsStorage,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_TRANS_LAYER_INFO_IND:
                 if (eIND == rsp_ctx.type)
                 {
                     unpack_sms_SLQSTransLayerInfoCallback_ind_t *varp = &var_unpack_sms_SLQSTransLayerInfoCallback_ind_t;
                     const unpack_sms_SLQSTransLayerInfoCallback_ind_t *cstp = &const_unpack_sms_SLQSTransLayerInfoCallback_ind_t;
                     UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                     unpack_sms_SLQSTransLayerInfoCallback_ind,
                     dump_SLQSTransLayerInfoCallback_ind,
                     msg.buf,
                     rlen,
                     &var_unpack_sms_SLQSTransLayerInfoCallback_ind_t,
                     2,
                     CMP_PTR_TYPE, &varp->regInd, &cstp->regInd,
                     CMP_PTR_TYPE, varp->pTransLayerInfo, cstp->pTransLayerInfo,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_WMS_TRANS_NW_REG_INFO_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSNWRegInfoCallback_ind,
                    dump_SLQSNWRegInfoCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSNWRegInfoCallback_ind_t);
                }
                break;
            case eQMI_WMS_MSG_WAITING_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_sms_SLQSWmsMessageWaitingCallBack_ind,
                    dump_SLQSWmsMessageWaitingCallBack_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_sms_SLQSWmsMessageWaitingCallBack_ind_t);
                }
                break;
            default:
                printf("msgid 0x%x is unknown or not handled\n", rsp_ctx.msgid);
                break;
            }
        }
    }
    return 0;
}
