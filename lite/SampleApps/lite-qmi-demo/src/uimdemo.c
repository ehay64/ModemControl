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
#include "packingdemo.h"
#include "uim.h"


int uim_validate_dummy_unpack();
int msqid = -1;
int uim=-1;
unsigned running = 1;
static char remark[255]={0};
static int unpackRetCode = 0;
static char model_id[DMS_SLQSFWINFO_MODELID_SZ]="WP7603";
static int iLocalLog = 1;


#define BIT_CARD_STATUS (1)
#define BIT_PHY_SLOT_STATUS (1<<4)

////Modem Settings
pack_uim_SLQSUIMSwitchSlot_t DefaultSlotSettings = {1, 1};

pack_uim_SLQSUIMEventRegister_t st_ch = { BIT_CARD_STATUS | BIT_PHY_SLOT_STATUS };
unpack_uim_SLQSUIMEventRegister_t st_ch_ed;

slots_t UimSlotsStatus[3];
uint8_t    NumberOfPhySlot = sizeof(UimSlotsStatus)/sizeof(slots_t);

unpack_uim_SLQSUIMGetSlotsStatus_t tslot_stats = {&NumberOfPhySlot, UimSlotsStatus,
        SWI_UINT256_INT_VALUE} ;

uim_GetSlotsStatusTlv       GetSlotsStatusTlv;
uim_GetSlotsInfoTlv         GetSlotsInfoTlv;

unpack_uim_SLQSUIMGetSlotsStatusV2_t tslot_statsV2 = {&GetSlotsStatusTlv, &GetSlotsInfoTlv,
        SWI_UINT256_INT_VALUE} ;

pack_uim_SLQSUIMSwitchSlot_t tswitch_slot = {1, 1};
unpack_uim_SLQSUIMSwitchSlot_t tunpack_uim_SLQSUIMSwitchSlot = {0, SWI_UINT256_INT_VALUE};

uim_cardStatus CardStatus;
uim_hotSwapStatus HotSwapStatus;
uim_validCardStatus ValidCardStatus;
uim_simBusyStatus SimBusyStatus;
unpack_uim_GetCardStatus_t GetCardStatus = {&CardStatus,&HotSwapStatus,0, SWI_UINT256_INT_VALUE};
unpack_uim_GetCardStatusV2_t GetCardStatusV2 = {&CardStatus,&HotSwapStatus,
                               &ValidCardStatus,&SimBusyStatus,0, SWI_UINT256_INT_VALUE};

uim_remainingRetries RemainingRetries;
uim_encryptedPIN1    EncryptedPIN1;
uint32_t             IndicationToken;
uim_cardResult       CardResult;
unpack_uim_VerifyPin_t VerifyPin ={&RemainingRetries,&EncryptedPIN1,&IndicationToken,0, SWI_UINT256_INT_VALUE};

unpack_uim_UnblockPin_t UnblockPin= {&RemainingRetries,&EncryptedPIN1,&IndicationToken,0, SWI_UINT256_INT_VALUE};
unpack_uim_UnblockPinV2_t UnblockPinV2= {&RemainingRetries,&EncryptedPIN1,&IndicationToken,&CardResult,0, SWI_UINT256_INT_VALUE};

unpack_uim_SetPinProtection_t SetPinProtection = {&RemainingRetries,&EncryptedPIN1,&IndicationToken,0, SWI_UINT256_INT_VALUE};

uim_encryptedPIN1 test_pack_uim_VerifyPin_uim_encryptedPIN1={0,{0}};
uint8_t test_pack_uim_VerifyPin_KeyReferenceID = 1;
uint32_t test_pack_uim_IndicationToken=5;


pack_uim_VerifyPin_t VerifyPinReq[]={
    {
        NULL,//{0,{0}}, //uim_encryptedPIN1    EncryptedPIN1;
        NULL,//  &test_pack_uim_IndicationToken,//uint32_t IndicationToken;
        NULL,//1,//uint8_t KeyReferenceID;
        { 0x00, 0x01, "1" },//uim_sessionInformation sessionInfo;
        { 0x01, 0x04, "3699"},//uim_verifyUIMPIN      verifyPIN;
        0//uint16_t Tlvresult;
    },
    {
        (uim_encryptedPIN1*)&test_pack_uim_VerifyPin_uim_encryptedPIN1,//{0,{0}}, //uim_encryptedPIN1    EncryptedPIN1;
        &test_pack_uim_IndicationToken,//uint32_t IndicationToken;
        &test_pack_uim_VerifyPin_KeyReferenceID,//1,//uint8_t KeyReferenceID;
        { 0x00, 0x01, "1" },//uim_sessionInformation sessionInfo;
        { 0x01, 0x04, "3699" },//uim_verifyUIMPIN      verifyPIN;
        0//uint16_t Tlvresult;
    },
};
uint32_t IndicationToken = 5;
uint8_t KeyReferenceID  = 1;

pack_uim_UnblockPin_t UnblockPinReq={
    {0,{0}}, //uim_encryptedPIN1    EncryptedPIN1;
    NULL,//  &IndicationToken,//uint32_t IndicationToken;
    NULL,
    { 0x00, 0x01, "1" },//uim_sessionInformation sessionInfo;
    { 0x01, 0x08, "52075361", 0x04, "1234" },//Smartone LTE SIM (RED)
    //{ 0x01, 0x08, "64159849", 0x04, "1234" },//Smartone 3G SIM (WHITE)
    //{ 0x01, 0x08, "20624355", 0x04, "1234" },//Three LTE SIM
    //{ 0x01, 0x08, "79915593", 0x04, "1234" },//Three 3G SIM
    0//uint16_t Tlvresult;
};

pack_uim_SetPinProtection_t SetPinProtectionReq[]={
    {
        //Enable Pin Protect
        {0,{0}}, //uim_encryptedPIN1    EncryptedPIN1;
        &IndicationToken,//uint32_t IndicationToken;
        &KeyReferenceID,
        { 0x00, 0x01, "1" },//uim_sessionInformation sessionInfo;
        { 0x01, 0x01, 0x04, "1234" },//setPINProtection      pinProtection;
        0,//uint16_t Tlvresult;
    },
    {
        //Disable Pin Protect
        {0,{0}}, //uim_encryptedPIN1    EncryptedPIN1;
        &IndicationToken,//uint32_t IndicationToken;
        &KeyReferenceID,
        { 0x00, 0x01, "1" },//uim_sessionInformation sessionInfo;
        { 0x01, 0x00, 0x04, "1234" },//setPINProtection      pinProtection;
        0,//uint16_t Tlvresult;
    },
    
};

pack_uim_SLQSUIMPowerDown_t SLQSUIMPowerDownReq ={1};
unpack_uim_SLQSUIMPowerDown_t tunpack_uim_SLQSUIMPowerDown = {0, SWI_UINT256_INT_VALUE};

uint8_t bCheckHotSwapSwitch = 0;
pack_uim_SLQSUIMPowerUp_t   SLQSUIMPowerUpReq = {1,&bCheckHotSwapSwitch};
unpack_uim_SLQSUIMPowerUp_t tunpack_uim_SLQSUIMPowerUp = {0, SWI_UINT256_INT_VALUE};

unpack_uim_SLQSUIMReset_t tunpack_uim_SLQSUIMReset = {0, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMRefreshOK_t tSLQSUIMRefreshOK_t =  {
    { 1, 1, "1"},//uim_sessionInformation sessionInfo
    1,//OK to Refresh
};
unpack_uim_SLQSUIMRefreshOK_t tunpack_uim_SLQSUIMRefreshOK = {0, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMRefreshRegister_t tSLQSUIMRefreshRegister_t ={
    {
        0,//GW provisioning
        0,//Application identifier length
        {},//Application identifier value or channel ID. This value is required for non provisioning and for logical channel session types. It is ignored in all other cases
    },
    {
        1,//Flag that indicates to register
        1,//Flag that indicates vote for init when there is refresh
        1,//Number of set of fileinfo element
        {{0x3F00,0x02,{0x3F00,0x7FFF}}},//fileInfo information
    }
};
unpack_uim_SLQSUIMRefreshRegister_t tunpack_uim_SLQSUIMRefreshRegister = {0, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMRefreshComplete_t tSLQSUIMRefreshComplete_t ={
    { 2, 2, "1"},//Session Type: GW provisioning
    1,//Refresh was Successful
};
unpack_uim_SLQSUIMRefreshComplete_t tunpack_uim_SLQSUIMRefreshComplete = {0, SWI_UINT256_INT_VALUE};

uim_refreshevent refreshEvent;
pack_uim_SLQSUIMRefreshGetLastEvent_t tSLQSUIMRefreshGetLastEvent_req ={{ 1, 1, "1"}};//Session Type: 1X provisioning
unpack_uim_SLQSUIMRefreshGetLastEvent_t tSLQSUIMRefreshGetLastEvent_resp={&refreshEvent, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMGetFileAttributes_t tSLQSUIMGetFileAttributes_req={
    { 0x00, 0x00, "" },// Primary GW provisioning
    { 0x7F20, 0x02, {0x3F00,0x7F20}},// Contains the file information for GSM Dedicated File
    NULL,
};

uim_cardResult     cardresult;
uim_fileAttributes fileattributes;
uint32_t           indicationtoken;
unpack_uim_SLQSUIMGetFileAttributes_t tSLQSUIMGetFileAttributes_resp = { &cardresult, &fileattributes, &indicationtoken, SWI_UINT256_INT_VALUE};

uim_remainingRetries remainingRtries;
pack_uim_SLQSUIMDepersonalization_t tSLQSUIMDepersonalization_req = {{ 0x00, 0x00, 0x06, "000000" }};//This parameter de-activates the depersonalization information
unpack_uim_SLQSUIMDepersonalization_t tSLQSUIMDepersonalization_resp = {&remainingRtries, SWI_UINT256_INT_VALUE};

uim_cardResult         cardrsult;
uim_authenticateResult authenticateresult;
uint32_t           indicationToken;
uint32_t indicationTokenReq = 5;
pack_uim_SLQSUIMAuthenticate_t tSLQSUIMAuthenticate_req ={
    { 0x00, 0x00, "" }, //session type - Primary GW provisioning
    { 0x00, 0x04, "1234" }, //UIM Authenticate parameters
    &indicationTokenReq,
};
unpack_uim_SLQSUIMAuthenticate_t tSLQSUIMAuthenticate_resp = {&cardrsult, &authenticateresult, &indicationToken, SWI_UINT256_INT_VALUE};

uint32_t configurationMask = 0x01;
uint8_t autoSelection = 0xFF;
pack_uim_SLQSUIMGetConfiguration_t tSLQSUIMGetConfiguration_req = {&configurationMask};
unpack_uim_SLQSUIMGetConfiguration_t tSLQSUIMGetConfiguration_resp = {
     &autoSelection,
     NULL,
     NULL,
     SWI_UINT256_INT_VALUE
};//Automatic Selection"

pack_uim_ChangePin_t tChangePin_req = {{4,"1234"},&IndicationToken, &KeyReferenceID, { 0x00, 0x01, "1" },
        { 0x01, 0x04, "1234", 0x04, "0000" }, 1};

pack_uim_ChangePin_t tChangePin_req2 = {{4,"0000"},&IndicationToken, &KeyReferenceID, { 0x00, 0x01, "1" },
        { 0x01, 0x04, "0000", 0x04, "1234" }, 1};


uim_remainingRetries RemainingRetries;
uim_encryptedPIN1    EncryptedPIN1;
unpack_uim_ChangePin_t tChangePin_resp = {&RemainingRetries, &EncryptedPIN1, &IndicationToken,1, SWI_UINT256_INT_VALUE };

uint8_t    setFDNStatus = 0x01;
pack_uim_SLQSUIMSetServiceStatus_t tSLQSUIMSetServiceStatus_req = { { 0x01, 0x01, "1"}, &setFDNStatus };

uint8_t    setFDNStatusSettings;
pack_uim_SLQSUIMSetServiceStatus_t UimSetServiceStatus_Settings = { { 0x01, 0x01, "1"}, &setFDNStatusSettings };
unpack_uim_SLQSUIMSetServiceStatus_t tunpack_uim_SetServiceStatus = SWI_INIT_UNPACK_RESULT_VALUE;

pack_uim_SLQSUIMGetServiceStatus_t tSLQSUIMGetServiceStatus_req = { { 0x01, 0x01, "1"}, 1 };
uim_UIMGetFDNStatus       getFDNStatus;
uim_UIMGetHiddenKeyStatus HiddenKeyStatus;
uim_UIMGetIndex           Index;
unpack_uim_SLQSUIMGetServiceStatus_t tunpack_uim_GetServiceStatus = {
        &getFDNStatus,&HiddenKeyStatus,&Index, SWI_UINT256_INT_VALUE};

uint16_t                 lastRecord;
uint32_t                 indTokenreq;
pack_uim_SLQSUIMReadRecord_t tSLQSUIMReadRecord_req = { { 0, 1, "1"} ,
        {0x2FE2, 0x02, {0x3F00}}, {0x01, 0x00}, NULL, NULL  };
uim_cardResultInfo           cardResult;
uim_readResultInfo           readResult;
uim_additionalReadResult     addReadResult;
uim_indToken                 indicationTokenresp;
unpack_uim_SLQSUIMReadRecord_t tunpack_uim_ReadRecord = {&cardResult, &readResult,
        &addReadResult, &indicationTokenresp, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMReadRecord_t tSLQSUIMReadRecordSettings_req = { { 0, 1, "1"} ,
        { 0x6F3B, 0x04, {0x3F00,0x7F10} }, {0x01, 0x00}, NULL, NULL  };

pack_uim_SLQSUIMWriteRecord_t UIMWriteRecord_Settings = { { 0, 1, "1"} ,
        { 0x6F3B, 0x04, {0x3F00,0x7F10} }, {1,0,{}}, &indTokenreq  };

pack_uim_SLQSUIMWriteRecord_t tSLQSUIMWriteRecord_req = { { 0, 1, "1"} ,
        { 0x6F3B, 0x04, {0x3F00,0x7F10} }, {0x01,0x03,"ABC"}, &indTokenreq  };

uim_cardResultInfo            writecardResult;
uim_indToken                  writeIndTokenresp;
unpack_uim_SLQSUIMWriteRecord_t tunpack_uim_WriteRecord = {&writecardResult,
        &writeIndTokenresp, SWI_UINT256_INT_VALUE};

pack_uim_ReadTransparent_t tUIMReadTransparent_req =  {{0,1,"1",},
        { 0x6F07, 0x04, {0x3F00,0x7FFF} },{0,0}, NULL, NULL, 0};

uim_cardResult   CardResult;
uim_readResult   ReadResult;
uint32_t        rIndicationToken = 0xffffffff;
uint8_t         rEncryptedData = 0xff;
unpack_uim_ReadTransparent_t tunpack_uim_ReadTransparent = {
   &CardResult, &ReadResult, &rIndicationToken, &rEncryptedData,0, SWI_UINT256_INT_VALUE};

pack_uim_SLQSUIMWriteTransparent_t tSLQSUIMWriteTransparent_req = {{0,1,"1",},
        { 0x6F07, 0x04, {0x3F00,0x7FFF} },{0,3,"ABC"}, &indTokenreq};

pack_uim_SLQSUIMWriteTransparent_t UIMWriteTransparent_Settings = {{0,1,"1",},
        { 0x6F07, 0x04, {0x3F00,0x7FFF} },{0,0,{}}, &indTokenreq};

uim_cardResultInfo            writecardResultTrans;
uim_indToken                  writeIndTokenTrans;
unpack_uim_SLQSUIMWriteTransparent_t tunpack_uim_WriteTransparent = {&writecardResultTrans,
        &writeIndTokenTrans, SWI_UINT256_INT_VALUE};

swi_uint256_t unpack_uim_SLQSUIMEventRegisterPrefParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)
}};

swi_uint256_t unpack_uim_SLQSUIMEventRegisterParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_VerifyPinParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)
}};

swi_uint256_t unpack_uim_VerifyPinParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_SetPinProtectionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)
}};

swi_uint256_t unpack_uim_SetPinProtectionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_UnblockPinParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)
}};

swi_uint256_t unpack_uim_UnblockPinParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_ChangePinParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)
}};

swi_uint256_t unpack_uim_ChangePinParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_SLQSUIMGetSlotsStatusParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)
}};

swi_uint256_t unpack_uim_SLQSUIMGetSlotsStatusParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_SLQSUIMSwitchSlotParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_SLQSUIMSwitchSlotParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_uim_GetCardStatusParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,21)
}};

swi_uint256_t unpack_uim_GetCardStatusParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};
/////////////////////////////////////////////////////////////
void dump_eventReg(void * ptr)
{
    unpack_uim_SLQSUIMEventRegister_t *result =
            (unpack_uim_SLQSUIMEventRegister_t *) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("event mask returned %x\n", result->eventMask);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_uim_SLQSUIMEventRegisterPrefParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_SLQSUIMEventRegisterParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->eventMask > 0) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((result->eventMask > 0) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSUIMSwitchSlot(void * ptr)
{
    unpack_uim_SLQSUIMSwitchSlot_t *result =
            (unpack_uim_SLQSUIMSwitchSlot_t *) ptr;
    int is_matching = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_uim_SLQSUIMSwitchSlotParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_SLQSUIMSwitchSlotParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if (strstr( (char*)model_id, "EM75") ||
        strstr( (char*)model_id, "EM74"))
    {
        is_matching = 1;
        if ( !is_matching )
        {
            sprintf(remark,"This API is not supported for %s", model_id );
        }
    }
    local_fprintf("%s,",  "N/A");
    local_fprintf("%s\n", remark);
#endif
}

void dump_SLQSUIMGetSlotsStatusVerify(void * ptr)
{
    unpack_uim_SLQSUIMGetSlotsStatus_t *result =
            (unpack_uim_SLQSUIMGetSlotsStatus_t*) ptr;
    uint8_t i = 0, j = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pNumberOfPhySlot) && (result->pUimSlotsStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Physical Slot Status Length:%d\n",*(result->pNumberOfPhySlot));
        for( i = 0; i < *(result->pNumberOfPhySlot); i++)
        {
            printf("\t%d. Physical Card Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhyCardStatus);
            printf("\t%d. Physical Slot Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhySlotStatus);
            printf("\t%d. Logical Slot :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bLogicalSlot);
            printf("\t%d. ICCID Length :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength);
            printf("\t%d. ICCID :",i+1);
            for(j=0;j<result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength;j++)
                printf("0x%02X ",result->pUimSlotsStatus->uimSlotStatus[i].bICCID[j]);
            printf("\n");
        }
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if((result->pNumberOfPhySlot) && (result->pUimSlotsStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        for( i = 0; i < *(result->pNumberOfPhySlot); i++)
        {
            if ((uint8_t)tswitch_slot.ulPhysicalSlot == (i+1))
            {
                if(result->pUimSlotsStatus->uimSlotStatus[i].uPhySlotStatus != 1)
                    is_matching = 0;
            }
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
    }
#endif
}

void dump_SLQSUIMGetSlotsStatus(void * ptr)
{
    unpack_uim_SLQSUIMGetSlotsStatus_t *result =
            (unpack_uim_SLQSUIMGetSlotsStatus_t*) ptr;
    uint8_t i = 0, j = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pNumberOfPhySlot) && (result->pUimSlotsStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Physical Slot Status Length:%d\n",*(result->pNumberOfPhySlot));
        for( i = 0; i < *(result->pNumberOfPhySlot); i++)
        {
            printf("\t%d. Physical Card Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhyCardStatus);
            printf("\t%d. Physical Slot Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhySlotStatus);
            printf("\t%d. Logical Slot :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bLogicalSlot);
            printf("\t%d. ICCID Length :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength);
            printf("\t%d. ICCID :",i+1);
            for(j=0;j<result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength;j++)
                printf("0x%02X ",result->pUimSlotsStatus->uimSlotStatus[i].bICCID[j]);
            printf("\n");
        }
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetSlotsStatusV2(void * ptr)
{
    unpack_uim_SLQSUIMGetSlotsStatusV2_t *result =
            (unpack_uim_SLQSUIMGetSlotsStatusV2_t*) ptr;
    uint8_t i = 0, j = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pGetSlotsStatusTlv) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Physical Slot Status Length:%d\n",result->pGetSlotsStatusTlv->NumberOfPhySlot);
        for( i = 0; i < result->pGetSlotsStatusTlv->NumberOfPhySlot; i++)
        {
            printf("\t%d. Physical Card Status :%u\n",i+1,result->pGetSlotsStatusTlv->uimSlotStatus[i].uPhyCardStatus);
            printf("\t%d. Physical Slot Status :%u\n",i+1,result->pGetSlotsStatusTlv->uimSlotStatus[i].uPhySlotStatus);
            printf("\t%d. Logical Slot :%d\n",i+1,result->pGetSlotsStatusTlv->uimSlotStatus[i].bLogicalSlot);
            printf("\t%d. ICCID Length :%d\n",i+1,result->pGetSlotsStatusTlv->uimSlotStatus[i].bICCIDLength);
            printf("\t%d. ICCID :",i+1);
            for(j=0;j<result->pGetSlotsStatusTlv->uimSlotStatus[i].bICCIDLength;j++)
                printf("0x%02X ",result->pGetSlotsStatusTlv->uimSlotStatus[i].bICCID[j]);
            printf("\n");
        }
    }
    if((result->pGetSlotsInfoTlv) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Physical Slot Information Length:%d\n",result->pGetSlotsInfoTlv->NumberOfPhySlotInfo);
        for( i = 0; i < result->pGetSlotsInfoTlv->NumberOfPhySlotInfo; i++)
        {
            printf("\t%d. Card Protocol :%u\n",i+1,result->pGetSlotsInfoTlv->uimSlotInfo[i].cardProtocol);
            printf("\t%d. Num of App in EF-DIR of card. :%u\n",i+1,result->pGetSlotsInfoTlv->uimSlotInfo[i].numApp);
            printf("\t%d. ATR Value Length :%d\n",i+1,result->pGetSlotsInfoTlv->uimSlotInfo[i].atrValueLen);
            printf("\t%d. ATR Value :",i+1);
            for(j=0;j<result->pGetSlotsInfoTlv->uimSlotInfo[i].atrValueLen;j++)
            {
                printf("0x%02X ",result->pGetSlotsInfoTlv->uimSlotInfo[i].atrValue[j]);
            }
                printf("\n");
            printf("\t%d. Is eUICC :%d\n",i+1,result->pGetSlotsInfoTlv->uimSlotInfo[i].iseUICC);
            printf("\n");
        }
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetCardStatus(void * ptr)
{
    unpack_uim_GetCardStatus_t *result =
            (unpack_uim_GetCardStatus_t*) ptr;
    int lcount,lIcount1,lIcount;
    if(ptr==NULL)
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("%s\n",  "N/A");
        #endif
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->pCardStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Index of the primary GW   : %x\n",
                         result->pCardStatus->indexGwPri);
        printf("Index of the primary 1X   : %x\n",
                             result->pCardStatus->index1xPri);
        printf("Index of the secondary GW : %x\n",
                             result->pCardStatus->indexGwSec);
        printf("Index of the secondary 1X : %x\n",
                             result->pCardStatus->index1xSec);
        printf("Slots Available           : %x\n",
                             result->pCardStatus->numSlot);
        for ( lcount=0 ; lcount < result->pCardStatus->numSlot; lcount++ )
        {
            uim_slotInfo *temp = &result->pCardStatus->SlotInfo[lcount];
            printf( "\tInformation for SLOT%d\n ",lcount+1);
            printf( "\tState of the Card         : %x\n", temp->cardState);
            printf( "\tState of the UPIN         : %x\n", temp->upinState);
            printf( "\tRetries Remaining(UPIN)   : %d\n", temp->upinRetries);
            printf( "\tRetries Remaining(UPUK)   : %d\n", temp->upukRetries);
            printf( "\tReason For Error          : %x\n", temp->errorState);
            printf( "\tNo. of Apps Allowed       : %d\n", temp->numApp);
            for ( lIcount=0 ; lIcount < temp->numApp; lIcount++ )
            {
                uim_appStatus *lresp = &temp->AppStatus[lIcount];
                printf( "\t\tApplication Status Information for App%d\n ",lIcount+1);
                printf( "\t\tType of Application       : %x\n", lresp->appType);
                printf( "\t\tState of Application      : %x\n", lresp->appState);
                printf( "\t\tState of perso for App    : %x\n", lresp->persoState);
                printf( "\t\tIndicates perso feature   : %x\n", lresp->persoFeature);
                printf( "\t\tRetries Remaining(Perso BL): %d\n",
                                     lresp->persoRetries);
                printf( "\t\tRetries Remaining(Perso UB): %d\n",
                                     lresp->persoUnblockRetries);
                printf( "\t\tApplication Identifier Len: %d\n", lresp->aidLength);
                printf( "\t\tApplication Identifier Value : ");
                for ( lIcount1=0 ; lIcount1 < lresp->aidLength; lIcount1++ )
                {
                    printf( "%02x ", lresp->aidVal[lIcount1]);
                }
                printf("\n");
                printf( "\t\tIndication for UPIN       : %x\n", lresp->univPin);
                printf( "\t\tIndicates State of Pin1   : %x\n", lresp->pin1State);
                printf( "\t\tRetries Remaining(PIN1)   : %d\n", lresp->pin1Retries);
                printf( "\t\tRetries Remaining(PUK1)   : %d\n", lresp->puk1Retries);
                printf( "\t\tIndicates State of Pin2   : %x\n", lresp->pin2State);
                printf( "\t\tRetries Remaining(PIN2)   : %d\n", lresp->pin2Retries);
                printf( "\t\tRetries Remaining(PUK2)   : %d\n", lresp->puk2Retries);
            }
        }
    }
    if (( result->pHotSwapStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Hot Swap Length           : %d\n",
                result->pHotSwapStatus->hotSwapLength);
        printf("Status of HotSwap Switch  : ");
        for ( lcount=0 ; lcount < result->pHotSwapStatus->hotSwapLength; lcount++ )
        {
            printf("%x", result->pHotSwapStatus->hotSwap[lcount]);
        }
        printf("\n");
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_uim_GetCardStatusParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_GetCardStatusParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->pCardStatus != NULL) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((result->pCardStatus != NULL) ? "Correct": "Wrong"));
    }
#endif

}


void dump_GetCardStatusV2(void * ptr)
{
    unpack_uim_GetCardStatusV2_t *result =
            (unpack_uim_GetCardStatusV2_t*) ptr;
    int lcount,lIcount1,lIcount;
    if(ptr==NULL)
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("%s\n",  "N/A");
        #endif
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->pCardStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Index of the primary GW   : %x\n",
                         result->pCardStatus->indexGwPri);
        printf("Index of the primary 1X   : %x\n",
                             result->pCardStatus->index1xPri);
        printf("Index of the secondary GW : %x\n",
                             result->pCardStatus->indexGwSec);
        printf("Index of the secondary 1X : %x\n",
                             result->pCardStatus->index1xSec);
        printf("Slots Available           : %x\n",
                             result->pCardStatus->numSlot);
        for ( lcount=0 ; lcount < result->pCardStatus->numSlot; lcount++ )
        {
            uim_slotInfo *temp = &result->pCardStatus->SlotInfo[lcount];
            printf( "\tInformation for SLOT%d\n ",lcount+1);
            printf( "\tState of the Card         : %x\n", temp->cardState);
            printf( "\tState of the UPIN         : %x\n", temp->upinState);
            printf( "\tRetries Remaining(UPIN)   : %d\n", temp->upinRetries);
            printf( "\tRetries Remaining(UPUK)   : %d\n", temp->upukRetries);
            printf( "\tReason For Error          : %x\n", temp->errorState);
            printf( "\tNo. of Apps Allowed       : %d\n", temp->numApp);
            for ( lIcount=0 ; lIcount < temp->numApp; lIcount++ )
            {
                uim_appStatus *lresp = &temp->AppStatus[lIcount];
                printf( "\t\tApplication Status Information for App%d\n ",lIcount+1);
                printf( "\t\tType of Application       : %x\n", lresp->appType);
                printf( "\t\tState of Application      : %x\n", lresp->appState);
                printf( "\t\tState of perso for App    : %x\n", lresp->persoState);
                printf( "\t\tIndicates perso feature   : %x\n", lresp->persoFeature);
                printf( "\t\tRetries Remaining(Perso BL): %d\n",
                                     lresp->persoRetries);
                printf( "\t\tRetries Remaining(Perso UB): %d\n",
                                     lresp->persoUnblockRetries);
                printf( "\t\tApplication Identifier Len: %d\n", lresp->aidLength);
                printf( "\t\tApplication Identifier Value : ");
                for ( lIcount1=0 ; lIcount1 < lresp->aidLength; lIcount1++ )
                {
                    printf( "%02x ", lresp->aidVal[lIcount1]);
                }
                printf("\n");
                printf( "\t\tIndication for UPIN       : %x\n", lresp->univPin);
                printf( "\t\tIndicates State of Pin1   : %x\n", lresp->pin1State);
                printf( "\t\tRetries Remaining(PIN1)   : %d\n", lresp->pin1Retries);
                printf( "\t\tRetries Remaining(PUK1)   : %d\n", lresp->puk1Retries);
                printf( "\t\tIndicates State of Pin2   : %x\n", lresp->pin2State);
                printf( "\t\tRetries Remaining(PIN2)   : %d\n", lresp->pin2Retries);
                printf( "\t\tRetries Remaining(PUK2)   : %d\n", lresp->puk2Retries);
            }
        }
    }
    if (( result->pHotSwapStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Hot Swap Length           : %d\n",
                result->pHotSwapStatus->hotSwapLength);
        printf("Status of HotSwap Switch  : ");
        for ( lcount=0 ; lcount < result->pHotSwapStatus->hotSwapLength; lcount++ )
        {
            printf("%x", result->pHotSwapStatus->hotSwap[lcount]);
        }
        printf("\n");
    }
    if ( (result->pValidCardStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Valid Card Status Length           : %d\n",
                             result->pValidCardStatus->validCardLength);
        printf("Status of Valid Card  : ");
        for ( lcount=0 ; lcount < result->pValidCardStatus->validCardLength; lcount++ )
        {
            printf("%x", result->pValidCardStatus->validCard[lcount]);
        }
        printf("\n");
    }
    if ( (result->pSimBusyStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Sim Busy Status Length           : %d\n",
                             result->pSimBusyStatus->simBusyLength);
        printf("Status of Sim Busy  : ");
        for ( lcount=0 ; lcount < result->pSimBusyStatus->simBusyLength; lcount++ )
        {
            printf("%x", result->pSimBusyStatus->simBusy[lcount]);
        }
        printf("\n");
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_uim_GetCardStatusParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_GetCardStatusParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->pCardStatus != NULL) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((result->pCardStatus != NULL) ? "Correct": "Wrong"));
    }
#endif

}

void dump_uim_ReadTransparent(void *ptr)
{
    int lcount;
    unpack_uim_ReadTransparent_t *result =
            (unpack_uim_ReadTransparent_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

     if (( NULL !=  result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
     {
         printf("sw1(card result) : %x \n",
                           result->pCardResult->sw1);
         printf("sw2(card result): %x \n",
                           result->pCardResult->sw2);
     }
     if (( NULL !=  result->pReadResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
     {
         printf("Read Data Len : %d \n",
                           result->pReadResult->contentLen);
         printf("Read Data : ");
         for (lcount = 0;lcount < result->pReadResult->contentLen ;lcount++)
         {
             printf("%c", result->pReadResult->content[lcount]);
         }
         printf("\n");
     }
     if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
         printf("Indication Token : %d \n",*result->pIndicationToken);

     if (( NULL != result->pEncryptedData ) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
         printf(" Encrypted Data Flag : %d \n",(int)*result->pEncryptedData);
}

void dump_VerifyPin(void * ptr)
{
    int lcount;
    unpack_uim_VerifyPin_t *result =
            (unpack_uim_VerifyPin_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

     if (( NULL !=  result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
     {
         printf("Retry Remaining(Verify PIN) : %x \n",
                           result->pRemainingRetries->verifyLeft);
         printf("Retry Remaining(Unblock PIN): %x \n",
                           result->pRemainingRetries->unblockLeft);
     }
     if (( NULL !=  result->pEncryptedPIN1 ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
     {
         printf("Encrypted PIN1 Len : %d \n",
                           result->pEncryptedPIN1->pin1Len);
         printf("Encrypted PIN1 Value : ");
         for (lcount = 0;lcount < result->pEncryptedPIN1->pin1Len ;lcount++)
         {
             printf("%c", result->pEncryptedPIN1->pin1Val[lcount]);
         }
         printf("\n");
     }
     if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
         printf("Indication Token : %d \n",*result->pIndicationToken);
 
     if(iLocalLog==0)
       return ;
#if DEBUG_LOG_TO_FILE
     uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_uim_VerifyPinParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_VerifyPinParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

     if ((result->pRemainingRetries != NULL) || \
         (result->pEncryptedPIN1 != NULL) ||(result->pIndicationToken != NULL) )
         is_matching = 1;
     if ( unpackRetCode != 0 )
     {
         local_fprintf("%s,", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
         local_fprintf("%s\n", remark);
     }
     else
     {
         local_fprintf("%s\n", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
     }
#endif


}

void dump_UnblockPin(void * ptr)
{
    int lcount;
    unpack_uim_UnblockPin_t *result =
            (unpack_uim_UnblockPin_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    
     if (( NULL !=  result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
     {
         printf("Retry Remaining(Verify PIN) : %x \n",
                           result->pRemainingRetries->verifyLeft);
         printf("Retry Remaining(Unblock PIN): %x \n",
                           result->pRemainingRetries->unblockLeft);
     }
     if (( NULL !=  result->pEncryptedPIN1 ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
     {
         printf("Encrypted PIN1 Len : %d \n",
                           result->pEncryptedPIN1->pin1Len);
         printf("Encrypted PIN1 Value : ");
         for (lcount = 0;lcount < result->pEncryptedPIN1->pin1Len ;lcount++)
         {
             printf("%c", result->pEncryptedPIN1->pin1Val[lcount]);
         }
         printf("\n");
     }
     if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
         printf("Indication Token : %d \n",*result->pIndicationToken);
    
     if(iLocalLog==0)
       return ;
#if DEBUG_LOG_TO_FILE
     uint8_t is_matching = 0;
    
    CHECK_WHITELIST_MASK(
        unpack_uim_UnblockPinParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_UnblockPinParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

     if ((result->pRemainingRetries != NULL) || \
         (result->pEncryptedPIN1 != NULL) ||(result->pIndicationToken != NULL) )
         is_matching = 1;
     if ( unpackRetCode != 0 )
     {
         local_fprintf("%s,", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
         local_fprintf("%s\n", remark);
     }
     else
     {
         local_fprintf("%s\n", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
     }
#endif



}

void dump_UnblockPinV2(void * ptr)
{
    int lcount;
    unpack_uim_UnblockPinV2_t *result =
            (unpack_uim_UnblockPinV2_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    
    if (( NULL !=  result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Retry Remaining(Verify PIN) : %x \n",
                           result->pRemainingRetries->verifyLeft);
        printf("Retry Remaining(Unblock PIN): %x \n",
                           result->pRemainingRetries->unblockLeft);
    }
    if (( NULL !=  result->pEncryptedPIN1 ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Encrypted PIN1 Len : %d \n",
                           result->pEncryptedPIN1->pin1Len);
        printf("Encrypted PIN1 Value : ");
        for (lcount = 0;lcount < result->pEncryptedPIN1->pin1Len ;lcount++)
        {
            printf("%c", result->pEncryptedPIN1->pin1Val[lcount]);
        }
        printf("\n");
    }
    if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf("Indication Token : %d \n",*result->pIndicationToken);

    if (( NULL != result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf( "SW1 received from card :%x\n", result->pCardResult->sw1 );
        printf( "SW2 received from card :%x\n", result->pCardResult->sw2 );
    }
    
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
     uint8_t is_matching = 0;
    
    CHECK_WHITELIST_MASK(
        unpack_uim_UnblockPinParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_UnblockPinParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

     if ((result->pRemainingRetries != NULL) || \
         (result->pEncryptedPIN1 != NULL) ||(result->pIndicationToken != NULL) )
         is_matching = 1;
     if ( unpackRetCode != 0 )
     {
         local_fprintf("%s,", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
         local_fprintf("%s\n", remark);
     }
     else
     {
         local_fprintf("%s\n", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
     }
#endif
}

void dump_SetPinProtection(void * ptr)
{
    int lcount;
    unpack_uim_SetPinProtection_t *result =
            (unpack_uim_SetPinProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if (( NULL !=  result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Retry Remaining(Verify PIN) : %x \n",
                          result->pRemainingRetries->verifyLeft);
        printf("Retry Remaining(Unblock PIN): %x \n",
                           result->pRemainingRetries->unblockLeft);
    }
    if (( NULL !=  result->pEncryptedPIN1 ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Encrypted PIN1 Len : %d \n",
                    result->pEncryptedPIN1->pin1Len);
        printf("Encrypted PIN1 Value : ");
        for (lcount = 0;lcount < result->pEncryptedPIN1->pin1Len ;lcount++)
        {
            printf("%c", result->pEncryptedPIN1->pin1Val[lcount]);
        }
        printf("\n");
    }
    if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf("Indication Token : %d \n",*result->pIndicationToken);
    
#if DEBUG_LOG_TO_FILE
     uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_uim_SetPinProtectionParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_SetPinProtectionParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

     if ((result->pRemainingRetries != NULL) || \
         (result->pEncryptedPIN1 != NULL) ||(result->pIndicationToken != NULL) )
         is_matching = 1;

     if ( unpackRetCode != 0 )
     {
         local_fprintf("%s,", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
         local_fprintf("%s\n", remark);
     }
     else
     {
         local_fprintf("%s\n", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
     }
#endif

}

void dump_SLQSUIMPowerDown(void * ptr)
{
    unpack_uim_SLQSUIMPowerDown_t *result =
            (unpack_uim_SLQSUIMPowerDown_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,",  "Correct");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "Correct");
    }
#endif 
}

void dump_SLQSUIMPowerUp(void * ptr)
{
    unpack_uim_SLQSUIMPowerUp_t *result =
            (unpack_uim_SLQSUIMPowerUp_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    //need extra delay before next command can be sent
    sleep(10);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,",  "Correct");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "Correct");
    }
#endif   

}

void dump_SLQSUIMRefreshOK(void * ptr)
{
    unpack_uim_SLQSUIMRefreshOK_t *result =
            (unpack_uim_SLQSUIMRefreshOK_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMRefreshRegister(void * ptr)
{
    unpack_uim_SLQSUIMRefreshRegister_t *result =
            (unpack_uim_SLQSUIMRefreshRegister_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMRefreshComplete(void * ptr)
{
    unpack_uim_SLQSUIMRefreshComplete_t *result =
            (unpack_uim_SLQSUIMRefreshComplete_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMRefreshGetLastEvent(void * ptr)
{
    unpack_uim_SLQSUIMRefreshGetLastEvent_t *result =
            (unpack_uim_SLQSUIMRefreshGetLastEvent_t*) ptr;
    uint16_t lCount;
    uint16_t lIcount;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pRefreshEvent!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Parameter Indication refresh\n");
        printf("Stage:      %d\n", result->pRefreshEvent->stage );
        printf("Mode:       %d\n", result->pRefreshEvent->mode );
        printf("sessionType:%d\n", result->pRefreshEvent->sessionType);
        printf("aidLength: %d\n", result->pRefreshEvent->aidLength);

        uim_refreshevent *pTemp = result->pRefreshEvent;
        for ( lCount = 0; lCount < pTemp->aidLength ; lCount++ )
        {
            printf("Mode of Refresh = %d\n",pTemp->aid[lCount]);
        }

        printf(" Number of files = %d\n", pTemp->numOfFiles);
        for ( lCount = 0; lCount < pTemp->numOfFiles; lCount++ )
        {
            printf("FileID= %d\n",pTemp->arrfileInfo[lCount].fileID);
            printf("Path Length = %d\n",pTemp->arrfileInfo[lCount].pathLen);
            for ( lIcount = 0;lIcount < pTemp->arrfileInfo[lCount].pathLen; lIcount++ )
                {
                    printf(" Path = %x\n",pTemp->arrfileInfo[lCount].path[lIcount]);
                }
        }
    }
    printf("%s done\n", __func__);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetFileAttributes(void * ptr)
{
    unpack_uim_SLQSUIMGetFileAttributes_t *result =
            (unpack_uim_SLQSUIMGetFileAttributes_t*) ptr;
    uint16_t lcount;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if (( NULL != result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        {
            printf( "SW1 received from card :%x\n", result->pCardResult->sw1 );
            printf( "SW2 received from card :%x\n", result->pCardResult->sw2 );
        }
    if (( NULL != result->pFileAttributes ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        {
            printf( "File Size        : %d\n",result->pFileAttributes->fileSize);
            printf( "File ID          : %x\n",result->pFileAttributes->fileID);
            printf( "File Type        : %x\n",result->pFileAttributes->fileType);
            printf( "Record Size      : %d\n",result->pFileAttributes->recordSize);
            printf( "Record Count     : %d\n",result->pFileAttributes->recordCount);
            printf( "Security Read    : %x\n",result->pFileAttributes->secRead);
            printf( "Security Read M  : %x\n",result->pFileAttributes->secReadMask);
            printf( "Security Write   : %x\n",result->pFileAttributes->secWrite);
            printf( "Security Write M : %x\n",result->pFileAttributes->secWriteMask);
            printf( "Security Inc     : %x\n",result->pFileAttributes->secIncrease);
            printf( "Security Inc M   : %x\n",result->pFileAttributes->secIncreaseMask);
            printf( "Security De-Act  : %x\n",result->pFileAttributes->secDeactivate);
            printf( "Security De-Act M: %x\n",result->pFileAttributes->secDeactivateMask);
            printf( "Security Act     : %x\n",result->pFileAttributes->secActivate);
            printf( "Security Act M   : %x\n",result->pFileAttributes->secActivateMask);
            printf( "Raw Len          : %d\n",result->pFileAttributes->rawLen);
            printf( "Raw Value        : ");
            for ( lcount=0 ; lcount < result->pFileAttributes->rawLen ; lcount++ )
                {
                    printf( "%x ",result->pFileAttributes->rawValue[lcount]);
                }
        }
    printf( "\n");
    if((result->pIndicationToken != NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "Indication Token : %x\n",*result->pIndicationToken );
    printf("%s done\n", __func__);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMDepersonalization(void * ptr)
{
    unpack_uim_SLQSUIMDepersonalization_t *result =
            (unpack_uim_SLQSUIMDepersonalization_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if (( NULL != result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "SLQSUIMDepersonalization Successful\n");
        printf( "Verify Left  :%d\n", result->pRemainingRetries->verifyLeft );
        printf( "Unblock Left :%d\n", result->pRemainingRetries->unblockLeft );
    }
    else
        printf( "SLQSUIMDepersonalization Successful\n");
    printf("%s done\n", __func__);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMAuthenticate(void * ptr)
{
    unpack_uim_SLQSUIMAuthenticate_t *result =
            (unpack_uim_SLQSUIMAuthenticate_t*) ptr;
    uint16_t lcount;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if (( NULL != result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "SW1 received from card :%x\n", result->pCardResult->sw1 );
        printf( "SW2 received from card :%x\n", result->pCardResult->sw2 );
    }
    if (( NULL != result->pAuthenticateResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf( "Content Len  : %d\n",result->pAuthenticateResult->contentLen );
        printf( "Content      : ");
        for ( lcount=0 ; lcount < result->pAuthenticateResult->contentLen ; lcount++ )
        {
            printf( "%c",result->pAuthenticateResult->content[lcount]);
        }
        printf( "\n");
    }
    if((result->pIndicationToken != NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "Indication Token : %x\n",*result->pIndicationToken );

    printf("%s done\n", __func__);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetConfiguration(void * ptr)
{
    unpack_uim_SLQSUIMGetConfiguration_t *result =
            (unpack_uim_SLQSUIMGetConfiguration_t*) ptr;
    uint16_t count;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((NULL != result->pAutoSelection) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf( "Automatic Selection : 0x%X \n",*result->pAutoSelection);
    if (( NULL != result->pPersonalizationStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        {
            printf( "Personalization Status: Num features = %d\n",result->pPersonalizationStatus->numFeatures );
            for(count=0; count <result->pPersonalizationStatus->numFeatures;count++)
                {
                    printf( "0x%X \n",result->pPersonalizationStatus->feature[count] );
                    printf( "0x%X \n",result->pPersonalizationStatus->verifyLeft[count]);
                    printf( "0x%X \n",result->pPersonalizationStatus->unblockLeft[count] );
                    }
                }
    if((NULL != result->pHaltSubscription) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "HALT Subscription : 0x%X \n",*result->pHaltSubscription);
    printf("%s done\n", __func__);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMReset(void * ptr)
{
    unpack_uim_SLQSUIMReset_t *result =
            (unpack_uim_SLQSUIMReset_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf( "SLQSUIMReset Successful\n");
    printf("%s done\n", __func__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ChangePin(void * ptr)
{
    int lcount;
    unpack_uim_ChangePin_t *result =
            (unpack_uim_ChangePin_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

     if (( NULL !=  result->pRemainingRetries ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
     {
         printf("Retry Remaining(Verify PIN) : %x \n",
                           result->pRemainingRetries->verifyLeft);
         printf("Retry Remaining(Unblock PIN): %x \n",
                           result->pRemainingRetries->unblockLeft);
     }
     if (( NULL !=  result->pEncryptedPIN1 ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
     {
         printf("Encrypted PIN1 Len : %d \n",
                           result->pEncryptedPIN1->pin1Len);
         printf("Encrypted PIN1 Value : ");
         for (lcount = 0;lcount < result->pEncryptedPIN1->pin1Len ;lcount++)
         {
             printf("%c", result->pEncryptedPIN1->pin1Val[lcount]);
         }
         printf("\n");
     }
     if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
         printf("Indication Token : %d \n",*result->pIndicationToken);
    
     if(iLocalLog==0)
       return ;
#if DEBUG_LOG_TO_FILE
     uint8_t is_matching = 0;
    
    CHECK_WHITELIST_MASK(
        unpack_uim_ChangePinParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_ChangePinParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

     if ((result->pRemainingRetries != NULL) || \
         (result->pEncryptedPIN1 != NULL) ||(result->pIndicationToken != NULL) )
         is_matching = 1;
     if ( unpackRetCode != 0 )
     {
         local_fprintf("%s,", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
         local_fprintf("%s\n", remark);
     }
     else
     {
         local_fprintf("%s\n", ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
     }
#endif

}

void dump_SLQSUIMGetSlotsStatusSettings(void * ptr)
{
    unpack_uim_SLQSUIMGetSlotsStatus_t *result =
            (unpack_uim_SLQSUIMGetSlotsStatus_t*) ptr;

    uint8_t i = 0, j = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pNumberOfPhySlot) && (result->pUimSlotsStatus)  && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Physical Slot Status Length:%d\n",*(result->pNumberOfPhySlot));
        for( i = 0; i < *(result->pNumberOfPhySlot); i++)
        {
            if(result->pUimSlotsStatus->uimSlotStatus[i].uPhySlotStatus==1)
            {
                DefaultSlotSettings.ulPhysicalSlot = i+1;
                DefaultSlotSettings.bLogicalSlot = result->pUimSlotsStatus->uimSlotStatus[i].bLogicalSlot;
            }
            printf("\t%d. Physical Card Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhyCardStatus);
            printf("\t%d. Physical Slot Status :%u\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].uPhySlotStatus);
            printf("\t%d. Logical Slot :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bLogicalSlot);
            printf("\t%d. ICCID Length :%d\n",i+1,result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength);
            printf("\t%d. ICCID :",i+1);
            for(j=0;j<result->pUimSlotsStatus->uimSlotStatus[i].bICCIDLength;j++)
                printf("0x%02X ",result->pUimSlotsStatus->uimSlotStatus[i].bICCID[j]);
            printf("\n");
        }

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_uim_SLQSUIMGetSlotsStatusParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_uim_SLQSUIMGetSlotsStatusParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

        if ( result->pNumberOfPhySlot != NULL)
        {
            if ( *(result->pNumberOfPhySlot) > 0 )
                is_matching = 1;
        }
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        }
#endif
    }

}

void dump_SLQSUIMSetServiceStatus(void* ptr)
{
    unpack_uim_SLQSUIMSetServiceStatus_t *result =
        (unpack_uim_SLQSUIMSetServiceStatus_t*)ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf( "%s\n",__FUNCTION__ );
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetServiceStatusSettings(void * ptr)
{
    unpack_uim_SLQSUIMGetServiceStatus_t *result =
            (unpack_uim_SLQSUIMGetServiceStatus_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(( NULL !=  result->pFDNStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        *UimSetServiceStatus_Settings.pFDNStatus = result->pFDNStatus->FDNStatus;
    if (( NULL !=  result->pHiddenKeyStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf( "Hidden Key Status: %d\n",result->pHiddenKeyStatus->hiddenKey );
    if(( NULL !=  result->pIndex ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "Index            : %d\n",result->pIndex->index);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetServiceStatus(void * ptr)
{
    unpack_uim_SLQSUIMGetServiceStatus_t *result =
            (unpack_uim_SLQSUIMGetServiceStatus_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(( NULL !=  result->pFDNStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf( "FDN Status       : %d\n",result->pFDNStatus->FDNStatus);
    if (( NULL !=  result->pHiddenKeyStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf( "Hidden Key Status: %d\n",result->pHiddenKeyStatus->hiddenKey );
    if(( NULL !=  result->pIndex ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "Index            : %d\n",result->pIndex->index);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMReadRecordSettings(void * ptr)
{
    unpack_uim_SLQSUIMReadRecord_t *result =
            (unpack_uim_SLQSUIMReadRecord_t*) ptr;

    int lCount;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(( NULL !=  result->pCardResult ) &&(swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "sw1(card result)   :%x\n",result->pCardResult->sw1);
        printf( "sw1(card result)   :%x\n",result->pCardResult->sw2);
    }
    if ((NULL != result->pCardResult) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        UIMWriteRecord_Settings.writeRecord.dataLen = result->pReadResult->contentLen;
        memcpy(UIMWriteRecord_Settings.writeRecord.data,
              result->pReadResult->content,
              result->pReadResult->contentLen);

        printf( "Read ContentLen    :%d\n",result->pReadResult->contentLen);
        printf( "Card Content       :");
        for ( lCount = 0; lCount < result->pReadResult->contentLen; lCount++ )
        {
            printf( "%d",result->pReadResult->content[lCount]);
        }
        printf("\n");
    }
    if ((NULL!= result->pAdditionalReadResult) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("AdditionalRecordLen :%d\n",result->pAdditionalReadResult->additionalRecordLen);
        printf("Additional Content  :");
        for ( lCount = 0; lCount < result->pAdditionalReadResult->additionalRecordLen ; lCount++ )
        {
            printf("%d",result->pAdditionalReadResult->additionalRecord[lCount]);
        }
        printf("\n");
    }
    if((NULL!= result->pIndicationToken) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Indication token    :%d\n",result->pIndicationToken->token);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMReadRecord(void * ptr)
{
    unpack_uim_SLQSUIMReadRecord_t *result =
            (unpack_uim_SLQSUIMReadRecord_t*) ptr;

    int lCount;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(( NULL !=  result->pCardResult ) &&(swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf( "sw1(card result)   :%x\n",result->pCardResult->sw1);
        printf( "sw1(card result)   :%x\n",result->pCardResult->sw2);
    }
    if ((NULL != result->pCardResult) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf( "Read ContentLen    :%d\n",result->pReadResult->contentLen);
        printf( "Card Content       :");
        for ( lCount = 0; lCount < result->pReadResult->contentLen; lCount++ )
        {
            printf( "%d",result->pReadResult->content[lCount]);
        }
        printf("\n");
    }
    if ((NULL!= result->pAdditionalReadResult) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("AdditionalRecordLen :%d\n",result->pAdditionalReadResult->additionalRecordLen);
        printf("Additional Content  :");
        for ( lCount = 0; lCount < result->pAdditionalReadResult->additionalRecordLen ; lCount++ )
        {
            printf("%d",result->pAdditionalReadResult->additionalRecord[lCount]);
        }
        printf("\n");
    }
    if((NULL!= result->pIndicationToken) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Indication token    :%d\n",result->pIndicationToken->token);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMWriteRecord(void * ptr)
{
    unpack_uim_SLQSUIMWriteRecord_t *result =
            (unpack_uim_SLQSUIMWriteRecord_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(( NULL !=  result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("sw1(card result)    :%x \n", result->pCardResult->sw1);
        printf("sw2(card result)    :%x \n", result->pCardResult->sw2);
    }
    if(( NULL !=  result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Indication token    :%d\n",result->pIndicationToken->token);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ReadTransparentSetting(void *ptr)
{
    unpack_uim_ReadTransparent_t *result =
            (unpack_uim_ReadTransparent_t*) ptr;
    int count;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);

    if (( NULL !=  result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("sw1(card result)   :%x \n",result->pCardResult->sw1);
        printf("sw2(card result)   :%x \n",result->pCardResult->sw2);
    }
    if (( NULL != result->pReadResult) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        UIMWriteTransparent_Settings.writeTransparent.dataLen = result->pReadResult->contentLen;
        memcpy(UIMWriteTransparent_Settings.writeTransparent.data,
              result->pReadResult->content,
              result->pReadResult->contentLen);

        printf("Read Data Len : %d \n",
                           result->pReadResult->contentLen);
        printf("Read Data : ");

        for(count=0; count < result->pReadResult->contentLen; count++)
        {
            printf("%x",result->pReadResult->content[count]);
        }
        printf("\n");
    }

     printf("%s done\n", __func__);
     if(iLocalLog==0)
       return ;
 #if DEBUG_LOG_TO_FILE
     local_fprintf("%s\n",  "N/A");
 #endif
}

void dump_SLQSUIMWriteTransparent(void *ptr)
{
    unpack_uim_SLQSUIMWriteTransparent_t *result =
            (unpack_uim_SLQSUIMWriteTransparent_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if (( NULL !=  result->pCardResult ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("sw1(card result)   :%x \n",result->pCardResult->sw1);
        printf("sw2(card result)   :%x \n",result->pCardResult->sw2);
    }
    if (( NULL != result->pIndicationToken ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Indication Token   :%d \n",result->pIndicationToken->token);

    printf("%s done\n", __func__);
    if(iLocalLog==0)
      return ;
 #if DEBUG_LOG_TO_FILE
     local_fprintf("%s\n",  "N/A");
 #endif
}

testitem_t oemapitestuim[] = {

    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus Modem",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatus, "unpack_uim_SLQSUIMGetSlotsStatus Modem",
        &tslot_stats, dump_SLQSUIMGetSlotsStatusSettings
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMSwitchSlot, "pack_uim_SLQSUIMSwitchSlot Change",
        &tswitch_slot,
        (unpack_func_item) &unpack_uim_SLQSUIMSwitchSlot, "unpack_uim_SLQSUIMSwitchSlot Change",
        &tunpack_uim_SLQSUIMSwitchSlot, dump_SLQSUIMSwitchSlot
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus Result",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatus, "unpack_uim_SLQSUIMGetSlotsStatus Result",
        &tslot_stats, dump_SLQSUIMGetSlotsStatusVerify
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMSwitchSlot, "pack_uim_SLQSUIMSwitchSlot Restore",
        &DefaultSlotSettings,
        (unpack_func_item) &unpack_uim_SLQSUIMSwitchSlot, "unpack_uim_SLQSUIMSwitchSlot Restore",
        &tunpack_uim_SLQSUIMSwitchSlot, dump_SLQSUIMSwitchSlot
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatusV2, "unpack_uim_SLQSUIMGetSlotsStatusV2",
        &tslot_statsV2, dump_SLQSUIMGetSlotsStatusV2
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMEventRegister, "pack_uim_SLQSUIMEventRegister",
        &st_ch, 
        (unpack_func_item) &unpack_uim_SLQSUIMEventRegister, "unpack_uim_SLQSUIMEventRegister",
        &st_ch_ed, dump_eventReg
    },
    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL,
        (unpack_func_item) &unpack_uim_GetCardStatus, "unpack_uim_GetCardStatus",
        &GetCardStatus, dump_GetCardStatus
    },
    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL, 
        (unpack_func_item) &unpack_uim_GetCardStatusV2, "unpack_uim_GetCardStatusV2",
        &GetCardStatusV2, dump_GetCardStatusV2
    },
    {
        (pack_func_item) &pack_uim_VerifyPin, "pack_uim_VerifyPin",
        &VerifyPinReq[1], 
        (unpack_func_item) &unpack_uim_VerifyPin, "unpack_uim_VerifyPin",
        &VerifyPin, dump_VerifyPin
    },
    {
        (pack_func_item) &pack_uim_SetPinProtection, "pack_uim_SetPinProtection",
        &SetPinProtectionReq[0], 
        (unpack_func_item) &unpack_uim_SetPinProtection, "unpack_uim_SetPinProtection",
        &SetPinProtection, dump_SetPinProtection
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerDown, "pack_uim_SLQSUIMPowerDown",
        &SLQSUIMPowerDownReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerDown, "unpack_uim_SLQSUIMPowerDown",
        &tunpack_uim_SLQSUIMPowerDown, dump_SLQSUIMPowerDown
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerUp, "pack_uim_SLQSUIMPowerUp",
        &SLQSUIMPowerUpReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerUp, "unpack_uim_SLQSUIMPowerUp",
        &tunpack_uim_SLQSUIMPowerUp, dump_SLQSUIMPowerUp
    },
    {
        (pack_func_item) &pack_uim_UnblockPin, "pack_uim_UnblockPin",
        &UnblockPinReq, 
        (unpack_func_item) &unpack_uim_UnblockPin, "unpack_uim_UnblockPin",
        &UnblockPin, dump_UnblockPin
    },
    {
        (pack_func_item) &pack_uim_UnblockPin, "pack_uim_UnblockPin",
        &UnblockPinReq, 
        (unpack_func_item) &unpack_uim_UnblockPinV2, "unpack_uim_UnblockPinV2",
        &UnblockPinV2, dump_UnblockPinV2
    },
    {
        (pack_func_item) &pack_uim_ChangePin, "pack_uim_ChangePin",
        &tChangePin_req,
        (unpack_func_item) &unpack_uim_ChangePin, "unpack_uim_ChangePin",
        &tChangePin_resp, dump_ChangePin
    },
    {
        (pack_func_item) &pack_uim_ChangePin, "pack_uim_ChangePin",
        &tChangePin_req2,
        (unpack_func_item) &unpack_uim_ChangePin, "unpack_uim_ChangePin",
        &tChangePin_resp, dump_ChangePin
    },
    {
        (pack_func_item) &pack_uim_SetPinProtection, "pack_uim_SetPinProtection",
        &SetPinProtectionReq[1], 
        (unpack_func_item) &unpack_uim_SetPinProtection, "unpack_uim_SetPinProtection",
        &SetPinProtection, dump_SetPinProtection
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMSwitchSlot, "pack_uim_SLQSUIMSwitchSlot",
        &tswitch_slot,
        (unpack_func_item) &unpack_uim_SLQSUIMSwitchSlot, "unpack_uim_SLQSUIMSwitchSlot",
        &tunpack_uim_SLQSUIMSwitchSlot, dump_SLQSUIMSwitchSlot
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerDown, "pack_uim_SLQSUIMPowerDown",
        &SLQSUIMPowerDownReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerDown, "unpack_uim_SLQSUIMPowerDown",
        &tunpack_uim_SLQSUIMPowerDown, dump_SLQSUIMPowerDown
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerUp, "pack_uim_SLQSUIMPowerUp",
        &SLQSUIMPowerUpReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerUp, "unpack_uim_SLQSUIMPowerUp",
        &tunpack_uim_SLQSUIMPowerUp, dump_SLQSUIMPowerUp
    }

};

testitem_t totestuim[] = {
    ////Get Modem Settings
    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatus, "unpack_uim_SLQSUIMGetSlotsStatus",
        &tslot_stats, dump_SLQSUIMGetSlotsStatusSettings
    },
    /////////////////////////////////////
    {
        (pack_func_item) &pack_uim_SLQSUIMSwitchSlot, "pack_uim_SLQSUIMSwitchSlot",
        &tswitch_slot,
        (unpack_func_item) &unpack_uim_SLQSUIMSwitchSlot, "unpack_uim_SLQSUIMSwitchSlot",
        &tunpack_uim_SLQSUIMSwitchSlot, dump_SLQSUIMSwitchSlot
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatus, "unpack_uim_SLQSUIMGetSlotsStatus",
        &tslot_stats, dump_SLQSUIMGetSlotsStatus
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMEventRegister, "pack_uim_SLQSUIMEventRegister",
        &st_ch, 
        (unpack_func_item) &unpack_uim_SLQSUIMEventRegister, "unpack_uim_SLQSUIMEventRegister",
        &st_ch_ed, dump_eventReg
    },
    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL, 
        (unpack_func_item) &unpack_uim_GetCardStatus, "unpack_uim_GetCardStatus",
        NULL, dump_GetCardStatus
    },

    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL, 
        (unpack_func_item) &unpack_uim_GetCardStatus, "unpack_uim_GetCardStatus",
        &GetCardStatus, dump_GetCardStatus
    },
    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL, 
        (unpack_func_item) &unpack_uim_GetCardStatusV2, "unpack_uim_GetCardStatusV2",
        &GetCardStatusV2, dump_GetCardStatusV2
    },
#if 0
    {
        (pack_func_item) &pack_uim_VerifyPin, "pack_uim_VerifyPin",
        NULL, 
        (unpack_func_item) &unpack_uim_VerifyPin, "unpack_uim_VerifyPin",
        &VerifyPin, dump_VerifyPin
    },
#endif
    {
        (pack_func_item) &pack_uim_VerifyPin, "pack_uim_VerifyPin",
        &VerifyPinReq[1], 
        (unpack_func_item) &unpack_uim_VerifyPin, "unpack_uim_VerifyPin",
        &VerifyPin, dump_VerifyPin
    },
#if 0
    {
        (pack_func_item) &pack_uim_VerifyPin, "pack_uim_VerifyPin",
        &VerifyPinReq[1], 
        (unpack_func_item) &unpack_uim_VerifyPin, "unpack_uim_VerifyPin",
        &VerifyPin, dump_VerifyPin
    },
#endif
    {
        (pack_func_item) &pack_uim_SetPinProtection, "pack_uim_SetPinProtection",
        &SetPinProtectionReq[0], 
        (unpack_func_item) &unpack_uim_SetPinProtection, "unpack_uim_SetPinProtection",
        &SetPinProtection, dump_SetPinProtection
    },
    {
        (pack_func_item) &pack_uim_SetPinProtection, "pack_uim_SetPinProtection",
        &SetPinProtectionReq[1], 
        (unpack_func_item) &unpack_uim_SetPinProtection, "unpack_uim_SetPinProtection",
        &SetPinProtection, dump_SetPinProtection
    },
    {
        (pack_func_item) &pack_uim_UnblockPin, "pack_uim_UnblockPin",
        &UnblockPinReq, 
        (unpack_func_item) &unpack_uim_UnblockPin, "unpack_uim_UnblockPin",
        &UnblockPin, dump_UnblockPin
    },
    {
        (pack_func_item) &pack_uim_UnblockPin, "pack_uim_UnblockPin",
        &UnblockPinReq, 
        (unpack_func_item) &unpack_uim_UnblockPinV2, "unpack_uim_UnblockPin",
        &UnblockPinV2, dump_UnblockPinV2
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerDown, "pack_uim_SLQSUIMPowerDown",
        &SLQSUIMPowerDownReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerDown, "unpack_uim_SLQSUIMPowerDown",
        &tunpack_uim_SLQSUIMPowerDown, dump_SLQSUIMPowerDown
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMPowerUp, "pack_uim_SLQSUIMPowerUp",
        &SLQSUIMPowerUpReq,
        (unpack_func_item) &unpack_uim_SLQSUIMPowerUp, "unpack_uim_SLQSUIMPowerUp",
        &tunpack_uim_SLQSUIMPowerUp, dump_SLQSUIMPowerUp
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMReset, "pack_uim_SLQSUIMReset",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMReset, "unpack_uim_SLQSUIMReset",
        &tunpack_uim_SLQSUIMReset, dump_SLQSUIMReset
    },
    {
       (pack_func_item) &pack_uim_SLQSUIMRefreshOK, "pack_uim_SLQSUIMRefreshOK",
        &tSLQSUIMRefreshOK_t,
        (unpack_func_item) &unpack_uim_SLQSUIMRefreshOK, "unpack_uim_SLQSUIMRefreshOK",
        &tunpack_uim_SLQSUIMRefreshOK, dump_SLQSUIMRefreshOK
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMRefreshRegister, "pack_uim_SLQSUIMRefreshRegister",
        &tSLQSUIMRefreshRegister_t,
        (unpack_func_item) &unpack_uim_SLQSUIMRefreshRegister, "unpack_uim_SLQSUIMRefreshRegister",
        &tunpack_uim_SLQSUIMRefreshRegister, dump_SLQSUIMRefreshRegister
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMRefreshComplete, "pack_uim_SLQSUIMRefreshComplete",
        &tSLQSUIMRefreshComplete_t,
        (unpack_func_item) &unpack_uim_SLQSUIMRefreshComplete, "unpack_uim_SLQSUIMRefreshComplete",
        &tunpack_uim_SLQSUIMRefreshComplete, dump_SLQSUIMRefreshComplete
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMRefreshGetLastEvent, "pack_uim_SLQSUIMRefreshGetLastEvent",
        &tSLQSUIMRefreshGetLastEvent_req,
        (unpack_func_item) &unpack_uim_SLQSUIMRefreshGetLastEvent, "unpack_uim_SLQSUIMRefreshGetLastEvent",
        &tSLQSUIMRefreshGetLastEvent_resp, dump_SLQSUIMRefreshGetLastEvent
    },

    {
        (pack_func_item) &pack_uim_SLQSUIMGetFileAttributes, "pack_uim_SLQSUIMGetFileAttributes",
        &tSLQSUIMGetFileAttributes_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetFileAttributes, "unpack_uim_SLQSUIMGetFileAttributes",
        &tSLQSUIMGetFileAttributes_resp, dump_SLQSUIMGetFileAttributes
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMDepersonalization, "pack_uim_SLQSUIMDepersonalization",
        &tSLQSUIMDepersonalization_req,
        (unpack_func_item) &unpack_uim_SLQSUIMDepersonalization, "unpack_uim_SLQSUIMDepersonalization",
        &tSLQSUIMDepersonalization_resp, dump_SLQSUIMDepersonalization
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMAuthenticate, "pack_uim_SLQSUIMAuthenticate",
        &tSLQSUIMAuthenticate_req,
        (unpack_func_item) &unpack_uim_SLQSUIMAuthenticate, "unpack_uim_SLQSUIMAuthenticate",
        &tSLQSUIMAuthenticate_resp, dump_SLQSUIMAuthenticate
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetConfiguration, "pack_uim_SLQSUIMGetConfiguration",
        &tSLQSUIMGetConfiguration_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetConfiguration, "unpack_uim_SLQSUIMGetConfiguration",
        &tSLQSUIMGetConfiguration_resp, dump_SLQSUIMGetConfiguration
    },
    {
        (pack_func_item) &pack_uim_ChangePin, "pack_uim_ChangePin",
        &tChangePin_req,
        (unpack_func_item) &unpack_uim_ChangePin, "unpack_uim_ChangePin",
        &tChangePin_resp, dump_ChangePin
    },
    {
        (pack_func_item) &pack_uim_UnblockPin, "pack_uim_UnblockPin",
        &UnblockPinReq, 
        (unpack_func_item) &unpack_uim_UnblockPin, "unpack_uim_UnblockPin",
        &UnblockPin, dump_UnblockPin
    },

    ////Restore Modem Settings
    {
        (pack_func_item) &pack_uim_SLQSUIMSwitchSlot, "pack_uim_SLQSUIMSwitchSlot",
        &DefaultSlotSettings,
        (unpack_func_item) &unpack_uim_SLQSUIMSwitchSlot, "unpack_uim_SLQSUIMSwitchSlot",
        &tunpack_uim_SLQSUIMSwitchSlot, dump_SLQSUIMSwitchSlot
    },
    ////////////////////////////
    {
        (pack_func_item) &pack_uim_SLQSUIMGetServiceStatus, "pack_uim_SLQSUIMGetServiceStatus Settings",
        &tSLQSUIMGetServiceStatus_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetServiceStatus, "unpack_uim_SLQSUIMGetServiceStatus",
        &tunpack_uim_GetServiceStatus, dump_SLQSUIMGetServiceStatusSettings
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetServiceStatus, "pack_uim_SLQSUIMGetServiceStatus",
        &tSLQSUIMGetServiceStatus_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetServiceStatus, "unpack_uim_SLQSUIMGetServiceStatus",
        &tunpack_uim_GetServiceStatus, dump_SLQSUIMGetServiceStatus
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMSetServiceStatus, "pack_uim_SLQSUIMSetServiceStatus",
        &tSLQSUIMSetServiceStatus_req,
        (unpack_func_item) &unpack_uim_SLQSUIMSetServiceStatus, "unpack_uim_SLQSUIMSetServiceStatus",
        &tunpack_uim_SetServiceStatus, dump_SLQSUIMSetServiceStatus
    },
    //Restore settings
    {
        (pack_func_item) &pack_uim_SLQSUIMSetServiceStatus, "pack_uim_SLQSUIMSetServiceStatus RestoreSettings",
        &UimSetServiceStatus_Settings,
        (unpack_func_item) &unpack_uim_SLQSUIMSetServiceStatus, "unpack_uim_SLQSUIMSetServiceStatus",
        &tunpack_uim_SetServiceStatus, dump_SLQSUIMSetServiceStatus
    },

    {
        (pack_func_item) &pack_uim_SLQSUIMReadRecord, "pack_uim_SLQSUIMReadRecordSettings",
        &tSLQSUIMReadRecordSettings_req,
        (unpack_func_item) &unpack_uim_SLQSUIMReadRecord, "unpack_uim_SLQSUIMReadRecord",
        &tunpack_uim_ReadRecord, dump_SLQSUIMReadRecordSettings
    }, 
    {
        (pack_func_item) &pack_uim_SLQSUIMReadRecord, "pack_uim_SLQSUIMReadRecord",
        &tSLQSUIMReadRecord_req,
        (unpack_func_item) &unpack_uim_SLQSUIMReadRecord, "unpack_uim_SLQSUIMReadRecord",
        &tunpack_uim_ReadRecord, dump_SLQSUIMReadRecord
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMWriteRecord, "pack_uim_SLQSUIMWriteRecord",
        &tSLQSUIMWriteRecord_req,
        (unpack_func_item) &unpack_uim_SLQSUIMWriteRecord, "unpack_uim_SLQSUIMWriteRecord",
        &tunpack_uim_WriteRecord, dump_SLQSUIMWriteRecord
    },
    //Restore settings
    {
        (pack_func_item) &pack_uim_SLQSUIMWriteRecord, "pack_uim_SLQSUIMWriteRecord RestoreSettings",
        &UIMWriteRecord_Settings,
        (unpack_func_item) &unpack_uim_SLQSUIMWriteRecord, "unpack_uim_SLQSUIMWriteRecord",
        &tunpack_uim_WriteRecord, dump_SLQSUIMWriteRecord
    },
    {
        (pack_func_item) &pack_uim_ReadTransparent, "pack_uim_ReadTransparentSettings",
        &tUIMReadTransparent_req,
        (unpack_func_item) &unpack_uim_ReadTransparent, "unpack_uim_ReadTransparentSettings",
        &tunpack_uim_ReadTransparent, dump_ReadTransparentSetting
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMWriteTransparent, "pack_uim_SLQSUIMWriteTransparent",
        &tSLQSUIMWriteTransparent_req,
        (unpack_func_item) &unpack_uim_SLQSUIMWriteTransparent, "unpack_uim_SLQSUIMWriteTransparent",
        &tunpack_uim_WriteTransparent, dump_SLQSUIMWriteTransparent
    },
    //Restore Settings
    {
        (pack_func_item) &pack_uim_SLQSUIMWriteTransparent, "pack_uim_SLQSUIMWriteTransparent Restore",
        &UIMWriteTransparent_Settings,
        (unpack_func_item) &unpack_uim_SLQSUIMWriteTransparent, "unpack_uim_SLQSUIMWriteTransparent Restore",
        &tunpack_uim_WriteTransparent, dump_SLQSUIMWriteTransparent
    },
};

unsigned int uimarraylen = (unsigned int)((sizeof(totestuim))/(sizeof(totestuim[0])));

testitem_t uimtotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_uim_SLQSUIMGetSlotsStatus, "pack_uim_SLQSUIMGetSlotsStatus",
        NULL,
        (unpack_func_item) &unpack_uim_SLQSUIMGetSlotsStatus, "unpack_uim_SLQSUIMGetSlotsStatus",
        NULL, dump_SLQSUIMGetSlotsStatusSettings
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMEventRegister, "pack_uim_SLQSUIMEventRegister",
        &st_ch, 
        (unpack_func_item) &unpack_uim_SLQSUIMEventRegister, "unpack_uim_SLQSUIMEventRegister",
        NULL, dump_eventReg
    },
    {
        (pack_func_item) &pack_uim_GetCardStatus, "pack_uim_GetCardStatus",
        NULL, 
        (unpack_func_item) &unpack_uim_GetCardStatus, "unpack_uim_GetCardStatus",
        NULL, dump_GetCardStatus
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMRefreshGetLastEvent, "pack_uim_SLQSUIMRefreshGetLastEvent",
        &tSLQSUIMRefreshGetLastEvent_req,
        (unpack_func_item) &unpack_uim_SLQSUIMRefreshGetLastEvent, "unpack_uim_SLQSUIMRefreshGetLastEvent",
        NULL, dump_SLQSUIMRefreshGetLastEvent
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetFileAttributes, "pack_uim_SLQSUIMGetFileAttributes",
        &tSLQSUIMGetFileAttributes_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetFileAttributes, "unpack_uim_SLQSUIMGetFileAttributes",
        NULL, dump_SLQSUIMGetFileAttributes
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetConfiguration, "pack_uim_SLQSUIMGetConfiguration",
        &tSLQSUIMGetConfiguration_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetConfiguration, "unpack_uim_SLQSUIMGetConfiguration",
        NULL, dump_SLQSUIMGetConfiguration
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMGetServiceStatus, "pack_uim_SLQSUIMGetServiceStatus",
        &tSLQSUIMGetServiceStatus_req,
        (unpack_func_item) &unpack_uim_SLQSUIMGetServiceStatus, "unpack_uim_SLQSUIMGetServiceStatus",
        NULL, dump_SLQSUIMGetServiceStatus
    },
    {
        (pack_func_item) &pack_uim_SLQSUIMReadRecord, "pack_uim_SLQSUIMReadRecord",
        &tSLQSUIMReadRecord_req,
        (unpack_func_item) &unpack_uim_SLQSUIMReadRecord, "unpack_uim_SLQSUIMReadRecord",
        NULL, dump_SLQSUIMReadRecord
    },
};

/////////////////////////////////////////////////////////////

void
hexdump(uint8_t *rsp, uint16_t len)
{
    int j;
    for(j=0 ;j<len; j++)
        printf("%02x ", rsp[j]);
    printf("\n");
}

void
dump_slot_stats( unpack_uim_SetUimSlotStatusChangeCallback_ind_t* pslot_stat )
{
    int i = 0;
    int slotcount = 0;
    printf(">>%s\n", __func__);
    if(NULL == pslot_stat)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (pslot_stat->ParamPresenceMask);
    if(swi_uint256_get_bit (pslot_stat->ParamPresenceMask, 16))
    {
        printf("NumberOfPhySlots:%d\n",pslot_stat->bNumberOfPhySlots);

        for(slotcount=0;slotcount<pslot_stat->bNumberOfPhySlots;slotcount++)
        {
            printf("Slot:%d\n",slotcount+1);
            printf("\tPhysical Card Status:%u\n",pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].uPhyCardStatus);
            printf("\tPhysical Slot Status:%u\n",pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].uPhySlotStatus);
            printf("\tLogicalSlot:%d\n",pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].bLogicalSlot);
            printf("\tICCIDLength:%d\n",pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].bICCIDLength);
            printf("\tICCID: ");
            for(i=0;i<pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].bICCIDLength;i++)
                printf("0x%02X ",pslot_stat->slotsstatusChange.uimSlotStatus[slotcount].bICCID[i]);
            printf("\n" );
        }
    }
}

void
dump_stats( unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t* pstat )
{
    int i,j,k;
    printf(">>%s\n", __func__);
    swi_uint256_print_mask (pstat->ParamPresenceMask);

    if ((pstat->pCardStatus!=NULL) && (swi_uint256_get_bit (pstat->ParamPresenceMask, 16)))
    {
        printf("\tindexGwPri 0x%04x\n", pstat->pCardStatus->indexGwPri);
        printf("\tindex1xPri 0x%04x\n", pstat->pCardStatus->index1xPri);
        printf("\tindexGwSec 0x%04x\n", pstat->pCardStatus->indexGwSec);
        printf("\tindex1xSec 0x%04x\n", pstat->pCardStatus->index1xSec);
        printf("\tnumSlot %d\n", pstat->pCardStatus->numSlot);
        for(i=0; i<pstat->pCardStatus->numSlot; i++)
        {
            printf("\t\tslot[%d]: card state %d\n", i, pstat->pCardStatus->SlotInfo[i].cardState);
            printf("\t\tslot[%d]: upin state %d\n", i, pstat->pCardStatus->SlotInfo[i].upinState);
            printf("\t\tslot[%d]: upin retries %d\n", i, pstat->pCardStatus->SlotInfo[i].upinRetries);
            printf("\t\tslot[%d]: upuk retries %d\n", i, pstat->pCardStatus->SlotInfo[i].upukRetries);
            printf("\t\tslot[%d]: error state %d\n", i, pstat->pCardStatus->SlotInfo[i].errorState);
            printf("\t\tslot[%d]: num app %d\n", i, pstat->pCardStatus->SlotInfo[i].numApp);
            for(j=0; j<pstat->pCardStatus->SlotInfo[i].numApp; j++)
            {
                printf("\t\t\tapp[%d]: type %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].appType);
                printf("\t\t\tapp[%d]: state %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].appState);
                printf("\t\t\tapp[%d]: perso state %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].persoState);
                printf("\t\t\tapp[%d]: perso feateure %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].persoFeature);
                printf("\t\t\tapp[%d]: perso retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].persoRetries);
                printf("\t\t\tapp[%d]: perso unblock retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].persoUnblockRetries);
                printf("\t\t\tapp[%d]: aid len %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].aidLength);
                printf("\t\t\tapp[%d]: aid vals:\n\t\t\t\t", j);
                for(k=0; k<pstat->pCardStatus->SlotInfo[i].AppStatus[j].aidLength; k++)
                    printf("%d ", pstat->pCardStatus->SlotInfo[i].AppStatus[j].aidVal[k]);
                printf("\n");
                printf("\t\t\tapp[%d]: univ pin %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].univPin);
                printf("\t\t\tapp[%d]: pin1 state %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].pin1State);
                printf("\t\t\tapp[%d]: pin1 retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].pin1Retries);
                printf("\t\t\tapp[%d]: puk1 retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].puk1Retries);
                printf("\t\t\tapp[%d]: pin2 state %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].pin2State);
                printf("\t\t\tapp[%d]: pin2 retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].pin2Retries);
                printf("\t\t\tapp[%d]: puk1 retries %d\n", j, pstat->pCardStatus->SlotInfo[i].AppStatus[j].puk1Retries);
            }
        }
    }
}

void dump_refresh_ind_stats(unpack_uim_SLQSUIMRefreshCallback_Ind_t* result)
{
    uint16_t lCount;
    uint16_t lIcount;
    if(result == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        uim_refreshevent *pTemp = &result->refreshEvent;

        printf("Parameter Indication refresh\n");
        printf("Stage:      %d\n", result->refreshEvent.stage );
        printf("Mode:       %d\n", result->refreshEvent.mode );
        printf("sessionType:%d\n", result->refreshEvent.sessionType);
        printf("aidLength: %d\n", result->refreshEvent.aidLength);
        
        for ( lCount = 0; lCount < pTemp->aidLength ; lCount++ )
        {
            printf("Mode of Refresh = %d\n",pTemp->aid[lCount]);
        }

        printf(" Number of files = %d\n", pTemp->numOfFiles);
        for ( lCount = 0; lCount < pTemp->numOfFiles; lCount++ )
        {
            printf("FileID= %d\n",pTemp->arrfileInfo[lCount].fileID);
            printf("Path Length = %d\n",pTemp->arrfileInfo[lCount].pathLen);
            for ( lIcount = 0;lIcount < pTemp->arrfileInfo[lCount].pathLen; lIcount++ )
            {
                printf(" Path = %x",pTemp->arrfileInfo[lCount].path[lIcount]);
            }
            printf("\n" );
        }
    }
    printf("%s done\n", __func__);
}


void* readthread(void* ptr)
{
    uint8_t rsp[QMI_MSG_MAX];
    uint16_t rspLen;
    int rtn;
    unpack_qmi_t rsp_ctx;
    const char *qmi_msg;
    uim_cardStatus changeStatus;
    unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t stats= {&changeStatus, SWI_UINT256_INT_VALUE};
    unpack_uim_SetUimSlotStatusChangeCallback_ind_t slot_stats;
    unpack_uim_SLQSUIMRefreshCallback_Ind_t refreshInd;
    UNUSEDPARAM(ptr);
    //sleep(1);
    while(running)
    {
        rtn = read(uim, rsp, QMI_MSG_MAX);
        if ((rtn > 0) && (rtn<QMI_MSG_MAX))
        {
            rspLen = (uint16_t)rtn;
            qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp[0] == eRSP)//non empty response
            {
                msgsnd(msqid, rsp, rspLen, 0);
            }
            else if (rsp[0] == eIND)
            {
                switch (rsp[3]) //FIXME high byte ignore
                {
                    case eQMI_UIM_STATUS_CHANGE_IND:
                        rtn = unpack_uim_SLQSUIMSetStatusChangeCallBack_ind(
                                rsp, rspLen, &stats);
                        if (!rtn)
                            dump_stats(&stats);
                        break;

                    case eQMI_UIM_SLOT_STATUS_IND:
                        rtn = unpack_uim_SetUimSlotStatusChangeCallback_ind(
                                rsp, rspLen, &slot_stats);
                        if (!rtn)
                            dump_slot_stats(&slot_stats);
                        break;

                    case eQMI_UIM_REFRESH_IND:
                        memset(&refreshInd, 0, sizeof(unpack_uim_SLQSUIMRefreshCallback_Ind_t));
                        rtn = unpack_uim_SLQSUIMRefreshCallback_Ind(
                                rsp, rspLen, &refreshInd);
                        if (!rtn)
                            dump_refresh_ind_stats(&refreshInd);
                        break;
                }
            }
        }
    }
    return NULL;
}

void uim_setOemApiTestConfig(uimConfig *uimConfigList)
{
    if ( uimConfigList != NULL)
    {
        strcpy((char*)VerifyPinReq[0].verifyPIN.pinVal, uimConfigList->pin);
        strcpy((char*)VerifyPinReq[1].verifyPIN.pinVal, uimConfigList->pin);
        strcpy((char*)UnblockPinReq.pinProtection.newPINVal, uimConfigList->pin);
        strcpy((char*)UnblockPinReq.pinProtection.pukVal, uimConfigList->puk);
        strcpy((char*)SetPinProtectionReq[0].pinProtection.pinValue, uimConfigList->pin);
        strcpy((char*)SetPinProtectionReq[1].pinProtection.pinValue, uimConfigList->pin);
        strcpy((char*)tSLQSUIMAuthenticate_req.authData.data, uimConfigList->pin);
        strcpy((char*)tChangePin_req.EncryptedPIN1.pin1Val, uimConfigList->pin);
        strcpy((char*)tChangePin_req.changePIN.oldPINVal, uimConfigList->pin);
        strcpy((char*)tChangePin_req2.changePIN.pinVal, uimConfigList->pin);
        strcpy((char*)model_id, uimConfigList->modelId);
    }
}

pthread_t tid = 0;

void uim_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======UIM pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(uimtotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(uimtotest_invalidunpack[i].dump!=NULL)
        {
            uimtotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(totestuim)/sizeof(testitem_t); i++)
    {
        if(totestuim[i].dump!=NULL)
        {
            totestuim[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(uimtotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(uimtotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,uimtotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(uim<0)
            uim = client_fd(eUIM);
        if(uim<0)
        {
            fprintf(stderr,"UIM Service Not Supported!\n");
            return ;
        }
        rtn = write(uim, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(uim>=0)
                close(uim);
            uim=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eUIM, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(uim, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(uimtotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                uimtotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           uimtotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    uimtotest_invalidunpack[i].dump(uimtotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(uim>=0)
                close(uim);
            uim=-1;
        }
        sleep(1);
        xid++;
    }
    if(uim>=0)
        close(uim);
    uim=-1;
}

void uim_test_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;
    unsigned xid =1;
    pthread_attr_t attr;
    const char *qmi_msg;   
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    printf("======UIM dummy unpack test===========\n");
    iLocalLog = 0;
    uim_validate_dummy_unpack();
    iLocalLog = 1;
    printf("\n======UIM unpack test===========\n");

    if(uim<0)
        uim = client_fd(eUIM);    
    key_t key;
    if(uim<0)
    {
        fprintf(stderr,"UIM Service Not Supported!\n");
        return ;
    }
    key = ftok("/dev/random", 'c');
    msqid = msgget(key, 0666 | IPC_CREAT);
    memset(&attr, 0, sizeof(attr));
    pthread_create(&tid, &attr, readthread, NULL);
    usleep(500);

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "UIM Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitestuim)/sizeof(testitem_t);
    }
    else
    {
        numoftestcase = sizeof(totestuim)/sizeof(testitem_t);
    }
    for(i=0; i<numoftestcase; i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen, byte_read;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        if(g_runoem_demo==1)
        {
            rtn = run_pack_item(oemapitestuim[i].pack)(&req_ctx, req, &reqLen,oemapitestuim[i].pack_ptr);
        }
        else
        {
            rtn = run_pack_item(totestuim[i].pack)(&req_ctx, req, &reqLen,totestuim[i].pack_ptr);
        }
#if DEBUG_LOG_TO_FILE
        if(g_runoem_demo==1)
        {
            local_fprintf("%s,%s,%s\n", oemapitestuim[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
        else
        {
            local_fprintf("%s,%s,%s\n", totestuim[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
#endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        rtn = write(uim, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eUIM, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rspLen = QMI_MSG_MAX; //TODO not really use further, set to max for now
        byte_read = msgrcv(msqid, &rsp, sizeof(rsp), 0, 0);
        qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);

        printf("<< receiving %s\n", qmi_msg);
        dump_hex(byte_read, rsp);

        if (rsp_ctx.xid == xid)
        {
            if(g_runoem_demo==1)
            {
                unpackRetCode = run_unpack_item(oemapitestuim[i].unpack)(rsp, rspLen, oemapitestuim[i].unpack_ptr);
                memset(remark, 0, sizeof(remark));
            }
            else
            {
                unpackRetCode = run_unpack_item(totestuim[i].unpack)(rsp, rspLen, totestuim[i].unpack_ptr);
            }
#if DEBUG_LOG_TO_FILE
            if(g_runoem_demo==1)
            {
                local_fprintf("%s,%s,", oemapitestuim[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
            }
            else
            {
                local_fprintf("%s,%s,", totestuim[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
            }
            if ( unpackRetCode!=eQCWWAN_ERR_NONE )
            {
                eRCode = helper_get_error_code(rsp);
                reason = helper_get_error_reason(eRCode);
                sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
            }
#endif
            if(g_runoem_demo==1)
            {
                oemapitestuim[i].dump(oemapitestuim[i].unpack_ptr);
            }
            else
            {
                totestuim[i].dump(totestuim[i].unpack_ptr);
            }
        }
        sleep(1);
        xid++;
    }
    printf("wait 10s for uim indication, try insert/remove sim...\n");
    sleep(10);
    if(running==0)
    {
        if(uim>=0)
            close(uim);
        uim = -1;
        msgctl(msqid, IPC_RMID, NULL);
    }
    running = 0;
    msgctl(msqid, IPC_RMID, NULL);
#ifdef __ARM_EABI__
    if(uim>=0)
        close(uim);
    uim = -1;
    //segfault occur without pthread_join on ARM
    //however, pthread_join hang indefinitely on x86_64
    void* pthread_rtn_value;
    if(tid!=0)
    pthread_join(tid, &pthread_rtn_value);
#endif
    if(tid!=0)
    pthread_cancel(tid);
    tid = 0;
    if(uim>=0)
        close(uim);
    uim = -1;
    
}

typedef struct {
 char szICCID[UIM_UINT8_MAX_STRING_SZ];
 uint8_t szPUK[UIM_UINT8_MAX_STRING_SZ];
}iccid_puk;

void change_default_puk(char *ICCIDString, uint16_t uLength)
{
    iccid_puk sims[]={
       {"","88456232"},//Smartone LTE SIM (RED)
       {"8985203000049613960f","64159849"},//Smartone 3G SIM (WHITE)
       {"8985203000057115684f","20624355"},//Three LTE SIM
       {"8985203000049613960f","79915593"},//Three 3G SIM
       {"89852061012131090341","64159849"}
    };
    int i = 0;
    printf("ICCID:%s\n",ICCIDString);
    for(i=0;i<(int)(sizeof(sims)/sizeof(iccid_puk));i++)
    {
        if((int)strlen(sims[i].szICCID)==(int)uLength)
        {
            int j =0;
            for(j=0;j<(int)strlen(sims[i].szICCID);j++)
            {
               if(sims[i].szICCID[j]!=ICCIDString[j])
               {
                  break;
               }
            }
            if (j==(int)strlen(sims[i].szICCID))
            {
               for(j=0;j<8;j++)
               UnblockPinReq.pinProtection.pukVal[j]=sims[i].szPUK[j];
               printf("ICCID:%s,",sims[i].szICCID);
               printf("PUK:%s\n",sims[i].szPUK);
               return ;
            }
        }
    }
}

void uim_test()
{    
    pack_qmi_t req_ctx;    
    int rtn;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len;
    uint8_t rsp[QMI_MSG_MAX];
    uint16_t rspLen;
    unpack_qmi_t rsp_ctx;
    const char *qmi_msg;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    if(uim<0)
        uim = client_fd(eUIM);
    if(uim<0)
    {
        fprintf(stderr,"UIM Service Not Supported!\n");
        return ;
    }
    /* UIM_READ_TRANSPARENT :This parameter contains the file information for ICCID Elementary File */
    uim_UIMSessionInformation sessionInfo_ICCID = { 0x00, 0x00, "" };
    uim_fileInfo             fileEF_ICCID  = { 0x2FE2, 0x02, {0x3F00} };
    uim_readTransparentInfo  readtp_ICCID   = { 0, 0 };

    uim_cardResult   CardResult;
    uim_readResult   ReadResult;
    uint32_t        IndicationToken = 0xffffffff;
    uint8_t         EncryptedData = 0xff;

    pack_uim_ReadTransparent_t req;
    unpack_uim_ReadTransparent_t output;
    uint16_t count;

    memset(&req,0,sizeof(req));
    memset(&output,0,sizeof(output));

    memset(&CardResult,0,sizeof(CardResult));
    memset(&ReadResult,0,sizeof(ReadResult));

    memcpy(&(req.sessionInfo),&sessionInfo_ICCID,sizeof(uim_UIMSessionInformation));
    memcpy(&(req.fileIndex),&fileEF_ICCID,sizeof(uim_fileInfo));
    memcpy(&(req.readTransparent),&readtp_ICCID,sizeof(uim_readTransparentInfo));   
    req.pIndicationToken = NULL;
    req.pEncryptData     = NULL;

    output.pCardResult      = &CardResult;
    output.pEncryptedData   = &EncryptedData;
    output.pIndicationToken = &IndicationToken;
    output.pReadResult      = &ReadResult;

    memset(qmi_req, 0, QMI_MSG_MAX);
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = 300;

    
    printf("starting read uim transparent ...\n");
    rtn = pack_uim_ReadTransparent(&req_ctx, qmi_req, &qmi_req_len, (void*)&req);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_uim_ReadTransparent", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    rtn = write(uim, qmi_req, qmi_req_len);
    if (rtn != qmi_req_len)
    {
        printf("write %d wrote %d\n", qmi_req_len, rtn);
    }
    else
    {
        qmi_msg = helper_get_req_str(eUIM, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    rtn = rspLen = read(uim, rsp, QMI_MSG_MAX);

    if(rtn>0)
    {
        qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);
        printf("<< receiving %s, Len: %u\n", qmi_msg, rspLen);
        dump_hex(rspLen, rsp);
    
        if (rsp_ctx.xid == req_ctx.xid) {
            unpackRetCode = unpack_uim_ReadTransparent( rsp, rspLen,&output);
            printf("rtn %d\n", unpackRetCode);
#if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_uim_ReadTransparent",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            if ( unpackRetCode!=eQCWWAN_ERR_NONE )
            {
                eRCode = helper_get_error_code(rsp);
                reason = helper_get_error_reason(eRCode);
                sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                local_fprintf("%s,",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
                local_fprintf("%s\n", remark);
            }
            else
            {
                local_fprintf("%s\n",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
            }
#endif

            swi_uint256_print_mask (output.ParamPresenceMask);
            if (( NULL != output.pCardResult ) && (swi_uint256_get_bit (output.ParamPresenceMask, 16)))
            {
                printf("SW1 received from card :%x\n", output.pCardResult->sw1 );
                printf("SW2 received from card :%x\n", output.pCardResult->sw2 );
            }
    
            if (( NULL != output.pReadResult) && (swi_uint256_get_bit (output.ParamPresenceMask, 17)))
            {
                char szcontent[UIM_UINT8_MAX_STRING_SZ]={0};
                printf("ICCID No: ");
                for(count=0; count < output.pReadResult->contentLen; count++)
                {
                    printf("%x",output.pReadResult->content[count] & 0x0f);
                    printf("%x", (output.pReadResult->content[count]>>4) & 0x0f);
                    sprintf(szcontent+count*2,"%x%x",output.pReadResult->content[count] & 0x0f,(output.pReadResult->content[count]>>4) & 0x0f);
                }
                printf("\n");
                change_default_puk(szcontent,output.pReadResult->contentLen*2);
            }
        }
    }
    if(uim>=0)
        close(uim);
    uim=-1;
}

void uim_read_IMSI()
{
    pack_qmi_t req_ctx;
    int rtn,uim=-1;
    uint8_t qmi_req[2048];
    uint16_t qmi_req_len;
    uint8_t rsp[2048];
    uint16_t rspLen;
    unpack_qmi_t rsp_ctx;
    const char *qmi_msg;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;


    if(uim<0)
        uim = client_fd(eUIM);
    if(uim<0)
    {
        fprintf(stderr,"UIM Service Not Supported!\n");
        return ;
    }
    /* UIM_READ_TRANSPARENT :This parameter contains the file information for IMSI Elementary File */
    uim_UIMSessionInformation sessionInfo_IMSI = { 0x00, 0x01, "1" };
    // EF-IMSI file from from 3GPP TS 31.102, section 4.2.2
    uim_fileInfo fileEF_IMSI = {
        0x6F07, // EF-IMSI File ID
        0x04, // Path len, 4 nibbles
        {0x3F00 ,0x7F20}
        //EF-IMSI path
    }; //IMSI

    uim_readTransparentInfo readtp_IMSI = { 0, 0 };
    uim_cardResult CardResult;
    uim_readResult ReadResult;
    uint32_t IndicationToken = 0xffffffff;
    uint8_t EncryptedData = 0xff;
    pack_uim_ReadTransparent_t req;
    unpack_uim_ReadTransparent_t output;
    uint16_t count;

    memset(&req,0,sizeof(req));
    memset(&output,0,sizeof(output));
    memset(&CardResult,0,sizeof(CardResult));
    memset(&ReadResult,0,sizeof(ReadResult));

    memcpy(&(req.sessionInfo),&sessionInfo_IMSI,sizeof(uim_UIMSessionInformation));
    memcpy(&(req.fileIndex),&fileEF_IMSI,sizeof(uim_fileInfo));
    memcpy(&(req.readTransparent),&readtp_IMSI,sizeof(uim_readTransparentInfo));

    req.pIndicationToken = NULL;
    req.pEncryptData = NULL;
    output.pCardResult = &CardResult;
    output.pEncryptedData = &EncryptedData;
    output.pIndicationToken = &IndicationToken;
    output.pReadResult = &ReadResult;
    memset(qmi_req, 0, 2048);
    memset(&req_ctx, 0, sizeof(req_ctx));

    req_ctx.xid = 300;
    printf("starting read uim transparent ...\n");

    rtn = pack_uim_ReadTransparent(&req_ctx, qmi_req, &qmi_req_len, (void*)&req);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_uim_ReadTransparent", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    rtn = write(uim, qmi_req, qmi_req_len);
    if (rtn != qmi_req_len)
        printf("write %d wrote %d\n", qmi_req_len, rtn);
    else
    {
        qmi_msg = helper_get_req_str(eUIM, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    rtn = rspLen = read(uim, rsp, 2048);
    if(rtn>0)
    {
        qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);
        printf("<< receiving %s, Len: %u\n", qmi_msg, rspLen);
        dump_hex(rspLen, rsp);

        if (rsp_ctx.xid == req_ctx.xid) {
            unpackRetCode = unpack_uim_ReadTransparent( rsp, rspLen,&output);
#if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_uim_ReadTransparent",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            if ( unpackRetCode!=eQCWWAN_ERR_NONE )
            {
                eRCode = helper_get_error_code(rsp);
                reason = helper_get_error_reason(eRCode);
                sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                local_fprintf("%s,",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
                local_fprintf("%s\n", remark);
            }
            else
            {
                local_fprintf("%s\n",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
            }
#endif

            if (0!= output.Tlvresult)
            {
                printf("TLV return %d\n", output.Tlvresult);
            }
            else
            {
                if ( NULL != output.pCardResult )
                {
                    printf("SW1 received from card :%x\n", output.pCardResult->sw1 );
                    printf("SW2 received from card :%x\n", output.pCardResult->sw2 );
                }
                if ( NULL != output.pReadResult)
                {
                    printf ("Count len: %d\n", output.pReadResult->contentLen);
                    printf("IMSI No: ");
                    for(count= 1; count < output.pReadResult->contentLen; count++)
                    {
                        if(count!=1)/*Skip the first nibble */
                            printf("%x",output.pReadResult->content[count] & 0x0f);
                        printf("%x", (output.pReadResult->content[count]>>4) & 0x0f);
                    }
                    printf("\n");
                }
            }
        }
    }

    if(uim>=0)
        close(uim);
    uim=-1;
}

void uim_read_IMSI_LTE()
{
    pack_qmi_t req_ctx;
    int rtn,uim=-1;
    uint8_t qmi_req[2048];
    uint16_t qmi_req_len;
    uint8_t rsp[2048];
    uint16_t rspLen;
    unpack_qmi_t rsp_ctx;
    const char *qmi_msg;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;


    if(uim<0)
        uim = client_fd(eUIM);
    if(uim<0)
    {
        fprintf(stderr,"UIM Service Not Supported!\n");
        return ;
    }
    /* UIM_READ_TRANSPARENT :This parameter contains the file information for IMSI Elementary File */
    uim_UIMSessionInformation sessionInfo_IMSI = { 0x00, 0x01, "1" };
    // EF-IMSI file from from 3GPP TS 31.102, section 4.2.2
    uim_fileInfo fileEF_IMSI = {
        0x6F07, // EF-IMSI File ID
        0x04, // Path len, 4 nibbles
        {0x3F00 ,0x7FFF}
        //EF-IMSI path
    }; //IMSI

    uim_readTransparentInfo readtp_IMSI = { 0, 0 };
    uim_cardResult CardResult;
    uim_readResult ReadResult;
    uint32_t IndicationToken = 0xffffffff;
    uint8_t EncryptedData = 0xff;
    pack_uim_ReadTransparent_t req;
    unpack_uim_ReadTransparent_t output;
    uint16_t count;

    memset(&req,0,sizeof(req));
    memset(&output,0,sizeof(output));
    memset(&CardResult,0,sizeof(CardResult));
    memset(&ReadResult,0,sizeof(ReadResult));

    memcpy(&(req.sessionInfo),&sessionInfo_IMSI,sizeof(uim_UIMSessionInformation));
    memcpy(&(req.fileIndex),&fileEF_IMSI,sizeof(uim_fileInfo));
    memcpy(&(req.readTransparent),&readtp_IMSI,sizeof(uim_readTransparentInfo));

    req.pIndicationToken = NULL;
    req.pEncryptData = NULL;
    output.pCardResult = &CardResult;
    output.pEncryptedData = &EncryptedData;
    output.pIndicationToken = &IndicationToken;
    output.pReadResult = &ReadResult;
    memset(qmi_req, 0, 2048);
    memset(&req_ctx, 0, sizeof(req_ctx));

    req_ctx.xid = 300;
    printf("starting read uim transparent ...\n");

    rtn = pack_uim_ReadTransparent(&req_ctx, qmi_req, &qmi_req_len, (void*)&req);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_uim_ReadTransparent", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    rtn = write(uim, qmi_req, qmi_req_len);
    if (rtn != qmi_req_len)
        printf("write %d wrote %d\n", qmi_req_len, rtn);
    else
    {
        qmi_msg = helper_get_req_str(eUIM, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    rtn = rspLen = read(uim, rsp, 2048);
    if(rtn>0)
    {
        qmi_msg = helper_get_resp_ctx(eUIM, rsp, rspLen, &rsp_ctx);
        printf("<< receiving %s, Len: %u\n", qmi_msg, rspLen);
        dump_hex(rspLen, rsp);

        if (rsp_ctx.xid == req_ctx.xid) {
            unpackRetCode = unpack_uim_ReadTransparent( rsp, rspLen,&output);
#if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_uim_ReadTransparent",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            if ( unpackRetCode!=eQCWWAN_ERR_NONE )
            {
                eRCode = helper_get_error_code(rsp);
                reason = helper_get_error_reason(eRCode);
                sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                local_fprintf("%s,",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
                local_fprintf("%s\n", remark);
            }
            else
            {
                local_fprintf("%s\n",  (output.pCardResult != NULL || output.pReadResult != NULL) ? "Correct": "Wrong");
            }
#endif

            if (0!= output.Tlvresult)
            {
                printf("TLV return %d\n", output.Tlvresult);
            }
            else
            {
                if (( NULL != output.pCardResult ) && (swi_uint256_get_bit (output.ParamPresenceMask, 16)))
                {
                    printf("SW1 received from card :%x\n", output.pCardResult->sw1 );
                    printf("SW2 received from card :%x\n", output.pCardResult->sw2 );
                }
                if (( NULL != output.pReadResult) && (swi_uint256_get_bit (output.ParamPresenceMask, 17)))
                {
                    printf ("Count len: %d\n", output.pReadResult->contentLen);
                    printf("IMSI No: ");
                    for(count=1; count < output.pReadResult->contentLen; count++)
                    { 
                        if(count!=1)/*Skip the first nibble */
                            printf("%x",output.pReadResult->content[count] & 0x0f);
                        printf("%x", (output.pReadResult->content[count]>>4) & 0x0f);
                    }
                    printf("\n");
                }
            }
        }
    }
    
    if(uim>=0)
        close(uim);
    uim=-1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
 uint8_t validate_uim_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_UIM_STATUS_CHANGE_IND */
    {0x04,0x01,0x00,0x32,0x00,0x25,0x00,
     0x10,0x22,0x00,
     0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x01,0x02,0x03,0x03,0x00,0x01,
     0x01,0x01,0x02,0x04,0x02,0x02,0x05,0x01,0x02,0x03,0x04,0x05,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    },

    /* eQMI_UIM_SLOT_STATUS_IND */
    {0x04,0x02,0x00,0x48,0x00,0x1c,0x00,
     0x10,0x19,0x00,
     0x01,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x0E,0x31,0x32,0x33,0x34,
     0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35
    },

    /* eQMI_UIM_REFRESH_IND */
    {0x04,0x03,0x00,0x33,0x00,0x15,0x00,
     0x10,0x12,0x00,
     0x01,0x01,0x02,0x05,0x41,0x42,0x43,0x44,0x45,0x01,0x00,0x00,0x3F,0x02,0x00,0x2B,0x00,0x3a
    },

    /* eQMI_UIM_GET_CARD_STATUS */
    {0x02,0x04,0x00,0x2F,0x00,0x40,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x22,0x00,
     0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x01,0x02,0x03,0x03,0x00,0x01,
     0x01,0x01,0x02,0x04,0x02,0x02,0x05,0x01,0x02,0x03,0x04,0x05,0x01,0x02,0x03,0x01,0x02,0x03,0x01,

     0x11,0x03,0x00,0x02,0x01,0x02,
     0x12,0x04,0x00,0x03,0x00,0x01,0x01,
     0x15,0x04,0x00,0x03,0x01,0x00,0x01
     },

     /* eQMI_UIM_VERIFY_PIN */
     {0x02,0x05,0x00,0x26,0x00,0x1b,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x02,0x02,
     0x11,0x05,0x00,0x04,0x41,0x42,0x43,0x44,
     0x12,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_UNBLOCK_PIN */
     {0x02,0x06,0x00,0x27,0x00,0x20,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x02,0x02,
     0x11,0x05,0x00,0x04,0x41,0x42,0x43,0x44,
     0x12,0x04,0x00,0x01,0x00,0x00,0x00,
     0x13,0x02,0x00,0x01,0x02,
     },

     /* eQMI_UIM_SET_PIN_PROTECTION */
     {0x02,0x07,0x00,0x25,0x00,0x1b,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x02,0x02,
     0x11,0x05,0x00,0x04,0x41,0x42,0x43,0x44,
     0x12,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_CHANGE_PIN */
     {0x02,0x08,0x00,0x28,0x00,0x14,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x02,0x02,
     0x11,0x05,0x00,0x04,0x41,0x42,0x43,0x44,
     },

     /* eQMI_UIM_READ_TRANSPARENT */
     {0x02,0x09,0x00,0x20,0x00,0x25,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x01,0x02,
     0x11,0x0b,0x00,0x09,0x00,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
     0x12,0x04,0x00,0x01,0x00,0x00,0x00,
     0x13,0x01,0x00,0x01
     },

     /* eQMI_UIM_EVENT_REG */
     {0x02,0x0a,0x00,0x2E,0x00,0x0E,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x4,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_GET_SLOTS_STATUS */
     {0x02,0x0b,0x00,0x47,0x00,0x23,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x19,0x00,
      0x01,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x0E,0x31,0x32,0x33,0x34,
      0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35
     },

     /* eQMI_UIM_SWITCH_SLOT */
     {0x02,0x0c,0x00,0x46,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_POWER_UP */
     {0x02,0x0d,0x00,0x31,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_POWER_DOWN */
     {0x02,0x0e,0x00,0x30,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_RESET */
     {0x02,0x0f,0x00,0x00,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_REFRESH_OK */
     {0x02,0x10,0x00,0x2b,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_REFRESH_REGISTER */
     {0x02,0x11,0x00,0x2a,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_REFRESH_COMPLETE */
     {0x02,0x12,0x00,0x2c,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

     /* eQMI_UIM_REFRESH_GET_LAST_EVENT */
     {0x02,0x13,0x00,0x2d,0x00,0x1c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x12,0x00,
      0x01,0x01,0x02,0x05,0x41,0x42,0x43,0x44,0x45,0x01,0x00,0x00,0x3F,0x02,0x00,0x2B,0x00,0x3a
     },

     /* eQMI_UIM_GET_FILE_ATTRIBUTES */
     {0x02,0x14,0x00,0x24,0x00,0x38,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,

     0x10,0x02,0x00,0x01,0x02,
     0x11,0x22,0x00,
     0x01,0x02,0x21,0x3F,0x02,0x02,0x01,0x10,0x00,0x02,0x0F,0x00,0x01,0x02,0x00,0x02,
     0x04,0x00,0x03,0x03,0x00,0x00,0x01,0x00,0x08,0x00,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,

     0x12,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_DEPERSONALIZATION */
     {0x02,0x15,0x00,0x29,0x00,0x0c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x02,0x00,0x02,0x02,
     },

     /* eQMI_UIM_AUTHENTICATE */
     {0x02,0x16,0x00,0x34,0x00,0x29,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x02,0x00,0x01,0x02,
      0x11,0x13,0x00,
      0x11,0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,

      0x12,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_GET_CONFIGURATION */
     {0x02,0x17,0x00,0x3A,0x00,0x22,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x01,0x00,0x01,
      0x11,0x10,0x00,
      0x05,0x01,0x03,0x03,0x02,0x03,0x03,0x03,0x03,0x03,0x04,0x03,0x03,0x05,0x03,0x03,
      0x12,0x01,0x00,0x01
     },

     /* eQMI_UIM_GET_SLOTS_STATUS */
     {0x02,0x18,0x00,0x47,0x00,0x44,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x19,0x00,
      0x01,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x0E,0x31,0x32,0x33,0x34,
      0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35,
      0x11,0x1E,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x16,0x3B,0x9F,0x96,0x80,0x1F,0xC7,
      0x80,0x31,0xE0,0x73,0xFE,0x21,0x13,0x67,0x98,0x07,0x02,0x10,0x01,0x01,0x01,0x54,
      0x00
     },

     /*eQMI_UIM_SET_SERVICE_STATUS */
     {0x02,0x19,0x00,0x37,0x00,0x07,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00},

     /*eQMI_UIM_GET_SERVICE_STATUS */
     {0x02,0x1a,0x00,0x36,0x00,0x13,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00,
         0x10,0x01,0x00,0x01,
         0x11,0x01,0x00,0x01,
         0x12,0x01,0x00,0x01
     },

     /*eQMI_UIM_READ_RECORD */
     {0x02,0x1b,0x00,0x21,0x00,0x1f,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00,
         0x10,0x02,0x00,0x01,0x01,
         0x11,0x03,0x00,0x01,0x00,0x01,
         0x12,0x03,0x00,0x01,0x00,0x01,
         0x13,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /*eQMI_UIM_WRITE_RECORD */
     {0x02,0x1c,0x00,0x23,0x00,0x13,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00,
         0x10,0x02,0x00,0x01,0x01,
         0x11,0x04,0x00,0x01,0x00,0x00,0x00
     },

     /* eQMI_UIM_WRITE_TRANSPARENT */
     {0x02,0x1d,0x00,0x22,0x00,0x13,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00,
         0x10,0x02,0x00,0x01,0x01,
         0x11,0x04,0x00,0x01,0x00,0x00,0x00,
     },
   };

/* eQMI_UIM_STATUS_CHANGE_IND */
uim_cardStatus cst_cardStatus = {0x0201,0x0403,0x0605,0x0807,0x01,
        {{0x01,0x02,0x03,0x03,0x00,0x01,{{0x01,0x01,0x02,0x04,0x02,0x02,0x05,{0x01,0x02,0x03,0x04,0x05},
        0x01,0x02,0x03,0x01,0x02,0x03,0x01}}}}};

uim_cardStatus var_cardStatus;

const unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t const_unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t = {
        &cst_cardStatus,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t var_unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t = {
        &var_cardStatus,{{0}} };

/* eQMI_UIM_SLOT_STATUS_IND */
const unpack_uim_SetUimSlotStatusChangeCallback_ind_t const_unpack_uim_SetUimSlotStatusChangeCallback_ind_t = {
        {{{2,1,1,14,{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35}}}},
        1,
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

/* eQMI_UIM_REFRESH_IND */
const unpack_uim_SLQSUIMRefreshCallback_Ind_t const_unpack_uim_SLQSUIMRefreshCallback_Ind_t = {
        1,
        {1,1,2,5,{0x41,0x42,0x43,0x44,0x45},1,
           {{0x3F00,2,{0x2B00,0x3a00}}}},
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

/* eQMI_UIM_GET_CARD_STATUS */
uim_hotSwapStatus cst_hotswapStatus = {0x02,{0x01,0x02}};
uim_validCardStatus cst_validCardStatus = {0x03,{0x00,0x01,0x01}};
uim_simBusyStatus cst_simBusyStatus = {0x03,{0x01,0x00,0x01}};
        
const unpack_uim_GetCardStatus_t const_unpack_uim_GetCardStatus_t = {
        &cst_cardStatus,&cst_hotswapStatus,0,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,21)}} };
const unpack_uim_GetCardStatusV2_t const_unpack_uim_GetCardStatusV2_t = {
        &cst_cardStatus,&cst_hotswapStatus,&cst_validCardStatus, &cst_simBusyStatus,0,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,21)}} };

uim_cardStatus var_cardStatus;
uim_hotSwapStatus var_hotswapStatus;
uim_validCardStatus var_validCardStatus;
uim_simBusyStatus var_simBusyStatus;

unpack_uim_GetCardStatus_t var_unpack_uim_GetCardStatus_t = {
        &var_cardStatus,&var_hotswapStatus,0,{{0}} };

unpack_uim_GetCardStatusV2_t var_unpack_uim_GetCardStatusV2_t = {
        &var_cardStatus,&var_hotswapStatus, &var_validCardStatus, &var_simBusyStatus,0,{{0}} };

/* eQMI_UIM_VERIFY_PIN */
uim_remainingRetries cst_remRetry = {0x02,0x02};
uim_encryptedPIN1 cst_encryptPIN1 = {0x04,{0x41,0x42,0x43,0x44}};
uint32_t cst_indToken = 0x01;
uim_cardResult       cst_pinCardResult = {0x01,0x02};
const unpack_uim_VerifyPin_t const_unpack_uim_VerifyPin_t = {
        &cst_remRetry,&cst_encryptPIN1,&cst_indToken, 0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

uim_remainingRetries var_remRetry;
uim_encryptedPIN1 var_encryptPIN1;
uint32_t var_indToken;
uim_cardResult       var_pinCardResult;
unpack_uim_VerifyPin_t var_unpack_uim_VerifyPin_t = {
        &var_remRetry,&var_encryptPIN1,&var_indToken, 0,{{0}} };

/* eQMI_UIM_UNBLOCK_PIN */;
const unpack_uim_UnblockPin_t const_unpack_uim_UnblockPin_t = {
        &cst_remRetry,&cst_encryptPIN1,&cst_indToken, 0,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

unpack_uim_UnblockPin_t var_unpack_uim_UnblockPin_t = {
        &var_remRetry,&var_encryptPIN1,&var_indToken, 0,{{0}} };

const unpack_uim_UnblockPinV2_t const_unpack_uim_UnblockPinV2_t = {
        &cst_remRetry,&cst_encryptPIN1,&cst_indToken, &cst_pinCardResult,0,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

unpack_uim_UnblockPinV2_t var_unpack_uim_UnblockPinV2_t = {
        &var_remRetry,&var_encryptPIN1,&var_indToken,&var_pinCardResult, 0,{{0}} };

/* eQMI_UIM_SET_PIN_PROTECTION */
const unpack_uim_SetPinProtection_t const_unpack_uim_SetPinProtection_t = {
        &cst_remRetry,&cst_encryptPIN1,&cst_indToken, 0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

unpack_uim_SetPinProtection_t var_unpack_uim_SetPinProtection_t = {
        &var_remRetry,&var_encryptPIN1,&var_indToken, 0,{{0}} };

/* eQMI_UIM_CHANGE_PIN */
const unpack_uim_ChangePin_t const_unpack_uim_ChangePin_t = {
        &cst_remRetry,&cst_encryptPIN1, NULL, 0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

unpack_uim_ChangePin_t var_unpack_uim_ChangePin_t = {
        &var_remRetry,&var_encryptPIN1, &var_indToken, 0,{{0}} };

/* eQMI_UIM_READ_TRANSPARENT */
uim_cardResult  cst_cardResult = {0x01,0x02};
uim_readResult  cst_readResult = {0x09,{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39}};
uint32_t cst_readIndToken = 1;
uint8_t cst_encrypt = 1;

const unpack_uim_ReadTransparent_t const_unpack_uim_ReadTransparent_t = {
        &cst_cardResult,&cst_readResult,&cst_readIndToken,&cst_encrypt, 0,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

uim_cardResult  var_cardResult;
uim_readResult  var_readResult;
uint32_t var_readIndToken;
uint8_t var_encrypt;

unpack_uim_ReadTransparent_t var_unpack_uim_ReadTransparent_t = {
        &var_cardResult,&var_readResult, &var_readIndToken, &var_encrypt,0,{{0}} };

/* eQMI_UIM_EVENT_REG */
const unpack_uim_SLQSUIMEventRegister_t const_unpack_uim_SLQSUIMEventRegister_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_UIM_GET_SLOTS_STATUS */
slots_t cst_slot = {{{2,1,1,14,{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35}}}};
uint8_t  cst_numSlot = 0x01;
uint8_t         cst_numPhySlotInfo = 0x01;
uim_physlotsInfo   cst_uimSlotsInfo = {{{0x00000002,0x00,0x16,{0x3B,0x9F,0x96,0x80,0x1F,0xC7,
        0x80,0x31,0xE0,0x73,0xFE,0x21,0x13,0x67,0x98,0x07,0x02,0x10,0x01,0x01,0x01,0x54},0x00}}};

uim_GetSlotsStatusTlv       cst_GetSlotsStatusTlv = {0x01,{{2,1,1,14,{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35}}},1};
uim_GetSlotsInfoTlv         cst_GetSlotsInfoTlv   = {0x01,{{0x00000002,0x00,0x16,{0x3B,0x9F,0x96,0x80,0x1F,0xC7,
        0x80,0x31,0xE0,0x73,0xFE,0x21,0x13,0x67,0x98,0x07,0x02,0x10,0x01,0x01,0x01,0x54},0x00}},1};

const unpack_uim_SLQSUIMGetSlotsStatus_t const_unpack_uim_SLQSUIMGetSlotsStatus_t = {
        &cst_numSlot,&cst_slot,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

const unpack_uim_SLQSUIMGetSlotsStatusV2_t const_unpack_uim_SLQSUIMGetSlotsStatusV2_t = {
        &cst_GetSlotsStatusTlv,&cst_GetSlotsInfoTlv,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

slots_t var_slot;
uint8_t  var_numSlot;
uim_GetSlotsStatusTlv       var_GetSlotsStatusTlv;
uim_GetSlotsInfoTlv         var_GetSlotsInfoTlv;

unpack_uim_SLQSUIMGetSlotsStatus_t var_unpack_uim_SLQSUIMGetSlotsStatus_t = {
        &var_numSlot,&var_slot,{{0}} };

unpack_uim_SLQSUIMGetSlotsStatusV2_t var_unpack_uim_SLQSUIMGetSlotsStatusV2_t = {
        &var_GetSlotsStatusTlv,&var_GetSlotsInfoTlv,{{0}} };

/* eQMI_UIM_SWITCH_SLOT */
const unpack_uim_SLQSUIMSwitchSlot_t const_unpack_uim_SLQSUIMSwitchSlot_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_POWER_UP */
const unpack_uim_SLQSUIMPowerUp_t const_unpack_uim_SLQSUIMPowerUp_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_POWER_DOWN */
const unpack_uim_SLQSUIMPowerDown_t const_unpack_uim_SLQSUIMPowerDown_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_RESET */
const unpack_uim_SLQSUIMReset_t const_unpack_uim_SLQSUIMReset_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_REFRESH_OK */
const unpack_uim_SLQSUIMRefreshOK_t const_unpack_uim_SLQSUIMRefreshOK_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_REFRESH_REGISTER */
const unpack_uim_SLQSUIMRefreshRegister_t const_unpack_uim_SLQSUIMRefreshRegister_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_REFRESH_COMPLETE */
const unpack_uim_SLQSUIMRefreshComplete_t const_unpack_uim_SLQSUIMRefreshComplete_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_REFRESH_GET_LAST_EVENT */
uim_refreshevent  cst_refreshEvent = {1,1,2,5,{0x41,0x42,0x43,0x44,0x45},1,
                         {{0x3F00,2,{0x2B00,0x3a00}}}};

const unpack_uim_SLQSUIMRefreshGetLastEvent_t const_unpack_uim_SLQSUIMRefreshGetLastEvent_t = {
        &cst_refreshEvent,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uim_refreshevent  var_refreshEvent;
unpack_uim_SLQSUIMRefreshGetLastEvent_t var_unpack_uim_SLQSUIMRefreshGetLastEvent_t = {
        &var_refreshEvent,{{0}} };

/* eQMI_UIM_GET_FILE_ATTRIBUTES */
uim_fileAttributes cst_fileAttr= {0x0201,0x3F21,0x02,0x0102,0x10,0x02,0x000F,0x01,0x0002,0x02,0x0004,
                      0x03,0x0003,0x00,0x0001,0x0008,{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38}};

const unpack_uim_SLQSUIMGetFileAttributes_t  const_unpack_uim_SLQSUIMGetFileAttributes_t= {
               &cst_cardResult,&cst_fileAttr,&cst_indToken, {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

uim_fileAttributes var_fileAttr;
unpack_uim_SLQSUIMGetFileAttributes_t  var_unpack_uim_SLQSUIMGetFileAttributes_t= {
               &var_cardResult,&var_fileAttr,&var_indToken,{{0}} };

/* eQMI_UIM_DEPERSONALIZATION */
const unpack_uim_SLQSUIMDepersonalization_t const_unpack_uim_SLQSUIMDepersonalization_t = {
               &cst_remRetry,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

unpack_uim_SLQSUIMDepersonalization_t var_unpack_uim_SLQSUIMDepersonalization_t = {
               &var_remRetry,{{0}} };

/* eQMI_UIM_AUTHENTICATE */
uim_authenticateResult    cst_authenticateResult = {0x0011,
                        {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57}};
const unpack_uim_SLQSUIMAuthenticate_t const_unpack_uim_SLQSUIMAuthenticate_t = {
               &cst_cardResult,&cst_authenticateResult,&cst_indToken,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

uim_authenticateResult    var_authenticateResult;
unpack_uim_SLQSUIMAuthenticate_t var_unpack_uim_SLQSUIMAuthenticate_t = {
               &var_cardResult,&var_authenticateResult,&var_indToken,{{0}} };

/* eQMI_UIM_GET_CONFIGURATION */
uint8_t cst_autoSel = 0x01;
uint8_t cst_haltSub = 0x01;
uim_personalizationStatus cst_PersonStatus = {0x05,{0x01,0x02,0x03,0x04,0x05},
                        {0x03,0x03,0x03,0x03,0x03},{0x03,0x03,0x03,0x03,0x03}};

const unpack_uim_SLQSUIMGetConfiguration_t const_unpack_uim_SLQSUIMGetConfiguration_t = {
               &cst_autoSel,&cst_PersonStatus,&cst_haltSub,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

uint8_t var_autoSel = 0x01;
uint8_t var_haltSub = 0x01;

uim_personalizationStatus var_PersonStatus;
unpack_uim_SLQSUIMGetConfiguration_t var_unpack_uim_SLQSUIMGetConfiguration_t = {
               &var_autoSel,&var_PersonStatus,&var_haltSub,{{0}} };

/* eQMI_UIM_SET_SERVICE_STATUS */
const unpack_uim_SLQSUIMSetServiceStatus_t const_unpack_uim_SetServiceStatus_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_UIM_GET_SERVICE_STATUS */
uim_UIMGetFDNStatus       cst_getfdnStatus = {0x01, 0x01};
uim_UIMGetHiddenKeyStatus cst_getHiddenkey = {0x01, 0x01};
uim_UIMGetIndex           cst_getIndex     = {0x01, 0x01};
const unpack_uim_SLQSUIMGetServiceStatus_t const_unpack_uim_SLQSUIMGetServiceStatus_t = {
        &cst_getfdnStatus, &cst_getHiddenkey, &cst_getIndex, {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };
uim_UIMGetFDNStatus       var_fdnstatus;
uim_UIMGetHiddenKeyStatus var_hiddenkey;
uim_UIMGetIndex           var_Token;
unpack_uim_SLQSUIMGetServiceStatus_t var_unpack_uim_SLQSUIMGetServiceStatus_t = {
               &var_fdnstatus, &var_hiddenkey, &var_Token,{{0}} };

/* eQMI_UIM_READ_RECORD */
uim_cardResultInfo         cst_readCardResult = {0x01,0x01,0x01};
uim_readResultInfo         cst_readResultInfo = {0x01,{0x01},0x01};
uim_additionalReadResult   cst_additionalReadResult = {0x01,{0x01},0x01};
uim_indToken               cst_readindToken = {0x01,0x01};
const unpack_uim_SLQSUIMReadRecord_t const_unpack_uim_SLQSUIMReadRecord_t = {
        &cst_readCardResult, &cst_readResultInfo, &cst_additionalReadResult, &cst_readindToken,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };
uim_cardResultInfo       var_cardResultInfo;
uim_readResultInfo       var_readResultInfo;
uim_additionalReadResult var_addReadResult;
uim_indToken             var_indTokenInfo;
unpack_uim_SLQSUIMReadRecord_t var_unpack_uim_SLQSUIMReadRecord_t = {
        &var_cardResultInfo, &var_readResultInfo, &var_addReadResult, &var_indTokenInfo, {{0}} };

/* eQMI_UIM_WRITE_RECORD */
uim_cardResultInfo       cst_writecardResult = {0x01,0x01,0x01};
uim_indToken             cst_writeindToken = {0x01,0x01};
const unpack_uim_SLQSUIMWriteRecord_t const_unpack_uim_SLQSUIMWriteRecord_t = {
        &cst_writecardResult, &cst_writeindToken, {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };
unpack_uim_SLQSUIMWriteRecord_t var_unpack_uim_SLQSUIMWriteRecord_t = {
        &var_cardResultInfo, &var_indTokenInfo, {{0}} };

/* eQMI_UIM_WRITE_TRANSPARENT */
const unpack_uim_SLQSUIMWriteTransparent_t const_unpack_uim_SLQSUIMWriteTransparent_t = {
        &cst_writecardResult, &cst_writeindToken, {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };
unpack_uim_SLQSUIMWriteTransparent_t var_unpack_uim_SLQSUIMWriteTransparent_t = {
        &var_cardResultInfo, &var_indTokenInfo, {{0}} };

int uim_validate_dummy_unpack()
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
    loopCount = sizeof(validate_uim_resp_msg)/sizeof(validate_uim_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_uim_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_UIM, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("UIM IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("UIM RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_UIM_STATUS_CHANGE_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t *varp
                           = &var_unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t;
                    const unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t *cstp 
                           = &const_unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t;

                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMSetStatusChangeCallBack_ind,
                    dump_stats,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMSetStatusChangeCallBack_ind_t,
                    2,
                    CMP_PTR_TYPE, varp->pCardStatus, cstp->pCardStatus,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_REFRESH_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMRefreshCallback_Ind,
                    dump_refresh_ind_stats,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMRefreshCallback_Ind_t);
                }
                break;
            case eQMI_UIM_GET_CARD_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_GetCardStatus_t *varp = &var_unpack_uim_GetCardStatus_t;
                    const unpack_uim_GetCardStatus_t *cstp = &const_unpack_uim_GetCardStatus_t;

                    unpack_uim_GetCardStatusV2_t *varp1 = &var_unpack_uim_GetCardStatusV2_t;
                    const unpack_uim_GetCardStatusV2_t *cstp1 = &const_unpack_uim_GetCardStatusV2_t;


                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_GetCardStatus,
                    dump_GetCardStatus,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_GetCardStatus_t,
                    4,
                    CMP_PTR_TYPE, varp->pCardStatus, cstp->pCardStatus,
                    CMP_PTR_TYPE, varp->pHotSwapStatus, cstp->pHotSwapStatus,
                    CMP_DATA_TYPE, (void *)(intptr_t)varp->Tlvresult, (void *)(intptr_t)cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_GetCardStatusV2,
                    dump_GetCardStatusV2,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_GetCardStatusV2_t,
                    6,
                    CMP_PTR_TYPE, varp1->pCardStatus, cstp1->pCardStatus,
                    CMP_PTR_TYPE, varp1->pHotSwapStatus, cstp1->pHotSwapStatus,
                    CMP_PTR_TYPE, varp1->pValidCardStatus, cstp1->pValidCardStatus,
                    CMP_PTR_TYPE, varp1->pSimBusyStatus, cstp1->pSimBusyStatus,
                    CMP_PTR_TYPE, &varp1->Tlvresult, &cstp1->Tlvresult,
                    CMP_PTR_TYPE, &varp1->ParamPresenceMask, &cstp1->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_VERIFY_PIN:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_VerifyPin_t *varp = &var_unpack_uim_VerifyPin_t;
                    const unpack_uim_VerifyPin_t *cstp = &const_unpack_uim_VerifyPin_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_VerifyPin,
                    dump_VerifyPin,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_VerifyPin_t,
                    5,
                    CMP_PTR_TYPE, varp->pRemainingRetries, cstp->pRemainingRetries,
                    CMP_PTR_TYPE, varp->pEncryptedPIN1, cstp->pEncryptedPIN1,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_UNBLOCK_PIN:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_UnblockPin_t *varp = &var_unpack_uim_UnblockPin_t;
                    const unpack_uim_UnblockPin_t *cstp = &const_unpack_uim_UnblockPin_t;

                    unpack_uim_UnblockPinV2_t *varp1 = &var_unpack_uim_UnblockPinV2_t;
                    const unpack_uim_UnblockPinV2_t *cstp1 = &const_unpack_uim_UnblockPinV2_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_UnblockPin,
                    dump_UnblockPin,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_UnblockPin_t,
                    5,
                    CMP_PTR_TYPE, varp->pRemainingRetries, cstp->pRemainingRetries,
                    CMP_PTR_TYPE, varp->pEncryptedPIN1, cstp->pEncryptedPIN1,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_UnblockPinV2,
                    dump_UnblockPinV2,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_UnblockPinV2_t,
                    6,
                    CMP_PTR_TYPE, varp1->pRemainingRetries, cstp1->pRemainingRetries,
                    CMP_PTR_TYPE, varp1->pEncryptedPIN1, cstp1->pEncryptedPIN1,
                    CMP_PTR_TYPE, varp1->pIndicationToken, cstp1->pIndicationToken,
                    CMP_PTR_TYPE, varp1->pCardResult, cstp1->pCardResult,
                    CMP_PTR_TYPE, &varp1->Tlvresult, &cstp1->Tlvresult,
                    CMP_PTR_TYPE, &varp1->ParamPresenceMask, &cstp1->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_SET_PIN_PROTECTION:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SetPinProtection_t *varp = &var_unpack_uim_SetPinProtection_t;
                    const unpack_uim_SetPinProtection_t *cstp = &const_unpack_uim_SetPinProtection_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SetPinProtection,
                    dump_SetPinProtection,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SetPinProtection_t,
                    5,
                    CMP_PTR_TYPE, varp->pRemainingRetries, cstp->pRemainingRetries,
                    CMP_PTR_TYPE, varp->pEncryptedPIN1, cstp->pEncryptedPIN1,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_CHANGE_PIN:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_ChangePin_t *varp = &var_unpack_uim_ChangePin_t;
                    const unpack_uim_ChangePin_t *cstp = &const_unpack_uim_ChangePin_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_ChangePin,
                    dump_ChangePin,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_ChangePin_t,
                    4,
                    CMP_PTR_TYPE, varp->pRemainingRetries, cstp->pRemainingRetries,
                    CMP_PTR_TYPE, varp->pEncryptedPIN1, cstp->pEncryptedPIN1,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_READ_TRANSPARENT:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_ReadTransparent_t *varp = &var_unpack_uim_ReadTransparent_t;
                    const unpack_uim_ReadTransparent_t *cstp = &const_unpack_uim_ReadTransparent_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_ReadTransparent,
                    dump_uim_ReadTransparent,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_ReadTransparent_t,
                    6,
                    CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                    CMP_PTR_TYPE, varp->pReadResult, cstp->pReadResult,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, varp->pEncryptedData, cstp->pEncryptedData,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_EVENT_REG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMEventRegister,
                    dump_eventReg,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMEventRegister_t);
                }
                break;
            case eQMI_UIM_GET_SLOTS_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    if(rsp_ctx.xid == 0x0B)
                    {
                        unpack_uim_SLQSUIMGetSlotsStatus_t *varp = &var_unpack_uim_SLQSUIMGetSlotsStatus_t;
                        const unpack_uim_SLQSUIMGetSlotsStatus_t *cstp = &const_unpack_uim_SLQSUIMGetSlotsStatus_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_uim_SLQSUIMGetSlotsStatus,
                        dump_SLQSUIMGetSlotsStatus,
                        msg.buf,
                        rlen,
                        &var_unpack_uim_SLQSUIMGetSlotsStatus_t,
                        3,
                        CMP_PTR_TYPE, varp->pNumberOfPhySlot, cstp->pNumberOfPhySlot,
                        CMP_PTR_TYPE, varp->pUimSlotsStatus, cstp->pUimSlotsStatus,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    if(rsp_ctx.xid == 0x18)
                    {
                        unpack_uim_SLQSUIMGetSlotsStatusV2_t *varpV2 = &var_unpack_uim_SLQSUIMGetSlotsStatusV2_t;
                        const unpack_uim_SLQSUIMGetSlotsStatusV2_t *cstpV2 = &const_unpack_uim_SLQSUIMGetSlotsStatusV2_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_uim_SLQSUIMGetSlotsStatusV2,
                        dump_SLQSUIMGetSlotsStatusV2,
                        msg.buf,
                        rlen,
                        &var_unpack_uim_SLQSUIMGetSlotsStatusV2_t,
                        3,
                        CMP_PTR_TYPE, varpV2->pGetSlotsStatusTlv, cstpV2->pGetSlotsStatusTlv,
                        CMP_PTR_TYPE, varpV2->pGetSlotsInfoTlv, cstpV2->pGetSlotsInfoTlv,
                        CMP_PTR_TYPE, &varpV2->ParamPresenceMask, &cstpV2->ParamPresenceMask,
                        );
                    }
                }
                break;
            case eQMI_UIM_SWITCH_SLOT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMSwitchSlot,
                    dump_SLQSUIMSwitchSlot,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMSwitchSlot_t);
                }
                break;
            case eQMI_UIM_SLOT_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SetUimSlotStatusChangeCallback_ind,
                    dump_slot_stats,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SetUimSlotStatusChangeCallback_ind_t);
                }
                break;
            case eQMI_UIM_POWER_UP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMPowerUp,
                    dump_SLQSUIMPowerUp,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMPowerUp_t);
                }
                break;
            case eQMI_UIM_POWER_DOWN:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMPowerDown,
                    dump_SLQSUIMPowerDown,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMPowerDown_t);
                }
                break;
            case eQMI_UIM_RESET:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMReset,
                    dump_SLQSUIMReset,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMReset_t);
                }
                break;
            case eQMI_UIM_REFRESH_OK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMRefreshOK,
                    dump_SLQSUIMRefreshOK,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMRefreshOK_t);
                }
                break;
            case eQMI_UIM_REFRESH_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMRefreshRegister,
                    dump_SLQSUIMRefreshRegister,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMRefreshRegister_t);
                }
                break;
            case eQMI_UIM_REFRESH_COMPLETE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMRefreshComplete,
                    dump_SLQSUIMRefreshComplete,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SLQSUIMRefreshComplete_t);
                }
                break;
            case eQMI_UIM_REFRESH_GET_LAST_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMRefreshGetLastEvent_t *varp = &var_unpack_uim_SLQSUIMRefreshGetLastEvent_t;
                    const unpack_uim_SLQSUIMRefreshGetLastEvent_t *cstp = &const_unpack_uim_SLQSUIMRefreshGetLastEvent_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMRefreshGetLastEvent,
                    dump_SLQSUIMRefreshGetLastEvent,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMRefreshGetLastEvent_t,
                    2,
                    CMP_PTR_TYPE, varp->pRefreshEvent, cstp->pRefreshEvent,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_GET_FILE_ATTRIBUTES:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMGetFileAttributes_t *varp = &var_unpack_uim_SLQSUIMGetFileAttributes_t;
                    const unpack_uim_SLQSUIMGetFileAttributes_t *cstp = &const_unpack_uim_SLQSUIMGetFileAttributes_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMGetFileAttributes,
                    dump_SLQSUIMGetFileAttributes,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMGetFileAttributes_t,
                    4,
                    CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                    CMP_PTR_TYPE, varp->pFileAttributes, cstp->pFileAttributes,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_DEPERSONALIZATION:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMDepersonalization_t *varp = &var_unpack_uim_SLQSUIMDepersonalization_t;
                    const unpack_uim_SLQSUIMDepersonalization_t *cstp = &const_unpack_uim_SLQSUIMDepersonalization_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMDepersonalization,
                    dump_SLQSUIMDepersonalization,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMDepersonalization_t,
                    2,
                    CMP_PTR_TYPE, varp->pRemainingRetries, cstp->pRemainingRetries,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_AUTHENTICATE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMAuthenticate_t *varp = &var_unpack_uim_SLQSUIMAuthenticate_t;
                    const unpack_uim_SLQSUIMAuthenticate_t *cstp = &const_unpack_uim_SLQSUIMAuthenticate_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMAuthenticate,
                    dump_SLQSUIMAuthenticate,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMAuthenticate_t,
                    4,
                    CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                    CMP_PTR_TYPE, varp->pAuthenticateResult, cstp->pAuthenticateResult,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_GET_CONFIGURATION:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMGetConfiguration_t *varp = &var_unpack_uim_SLQSUIMGetConfiguration_t;
                    const unpack_uim_SLQSUIMGetConfiguration_t *cstp = &const_unpack_uim_SLQSUIMGetConfiguration_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMGetConfiguration,
                    dump_SLQSUIMGetConfiguration,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMGetConfiguration_t,
                    4,
                    CMP_PTR_TYPE, varp->pAutoSelection, cstp->pAutoSelection,
                    CMP_PTR_TYPE, varp->pPersonalizationStatus, cstp->pPersonalizationStatus,
                    CMP_PTR_TYPE, varp->pHaltSubscription, cstp->pHaltSubscription,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_SET_SERVICE_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMSetServiceStatus,
                    dump_SLQSUIMSetServiceStatus,
                    msg.buf,
                    rlen,
                    &const_unpack_uim_SetServiceStatus_t);
                }
                break;
            case eQMI_UIM_GET_SERVICE_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMGetServiceStatus_t *varp = &var_unpack_uim_SLQSUIMGetServiceStatus_t;
                    const unpack_uim_SLQSUIMGetServiceStatus_t *cstp = &const_unpack_uim_SLQSUIMGetServiceStatus_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMGetServiceStatus,
                    dump_SLQSUIMGetServiceStatus,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMGetServiceStatus_t,
                    4,
                    CMP_PTR_TYPE, varp->pFDNStatus, cstp->pFDNStatus,
                    CMP_PTR_TYPE, varp->pHiddenKeyStatus, cstp->pHiddenKeyStatus,
                    CMP_PTR_TYPE, varp->pIndex, cstp->pIndex,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_READ_RECORD:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMReadRecord_t *varp = &var_unpack_uim_SLQSUIMReadRecord_t;
                    const unpack_uim_SLQSUIMReadRecord_t *cstp = &const_unpack_uim_SLQSUIMReadRecord_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMReadRecord,
                    dump_SLQSUIMReadRecord,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMReadRecord_t,
                    5,
                    CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                    CMP_PTR_TYPE, varp->pReadResult, cstp->pReadResult,
                    CMP_PTR_TYPE, varp->pAdditionalReadResult, cstp->pAdditionalReadResult,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_WRITE_RECORD:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_uim_SLQSUIMWriteRecord_t *varp = &var_unpack_uim_SLQSUIMWriteRecord_t;
                    const unpack_uim_SLQSUIMWriteRecord_t *cstp = &const_unpack_uim_SLQSUIMWriteRecord_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_uim_SLQSUIMWriteRecord,
                    dump_SLQSUIMWriteRecord,
                    msg.buf,
                    rlen,
                    &var_unpack_uim_SLQSUIMWriteRecord_t,
                    3,
                    CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                    CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_UIM_WRITE_TRANSPARENT:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_uim_SLQSUIMWriteTransparent_t *varp = &var_unpack_uim_SLQSUIMWriteTransparent_t;
                     const unpack_uim_SLQSUIMWriteTransparent_t *cstp = &const_unpack_uim_SLQSUIMWriteTransparent_t;

                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_uim_SLQSUIMWriteTransparent,
                     dump_SLQSUIMWriteTransparent,
                     msg.buf,
                     rlen,
                     &var_unpack_uim_SLQSUIMWriteTransparent_t,
                     3,
                     CMP_PTR_TYPE, varp->pCardResult, cstp->pCardResult,
                     CMP_PTR_TYPE, varp->pIndicationToken, cstp->pIndicationToken,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            default:
                printf("msgid 0x%x Unknown/NotHandled\n", rsp_ctx.msgid);
                break;
            }
        }
    }
    return 0;
}

