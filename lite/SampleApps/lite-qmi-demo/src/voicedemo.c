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
#include "voice.h"

int voice_validate_dummy_unpack();

static int iLocalLog = 1;

////Modem Settings////
uint8_t    Defaultautoanswer = 0x00;
voice_airTimer    DefaultAirTimerConfig;
voice_roamTimer   DefaultRoamTimerConfig;
uint8_t           DefaultTTYMode=0;
voice_prefVoiceSO DefaultPrefVoiceSO;
uint8_t           DefaultPrefVoiceDomain=0;

pack_voice_SLQSVoiceSetConfig_t DefaultVoiceSetConfigSettings = { 
        &Defaultautoanswer, 
        &DefaultAirTimerConfig, 
        &DefaultRoamTimerConfig,
        &DefaultTTYMode,
        &DefaultPrefVoiceSO, 
        &DefaultPrefVoiceDomain
};

///////////////////////
int voice = -1;

volatile int enVOICEThread =0;
pthread_t voice_tid = 0;
pthread_attr_t voice_attr;

uint8_t OriginateUSSDData[] = {
        1,5, 0x2A, 0x32, 0x32, 0x35, 0x23};
pack_voice_OriginateUSSD_t tpack_voice_OriginateUSSD = {OriginateUSSDData
        };
unpack_voice_OriginateUSSD_t tunpack_voice_OriginateUSSD = {0, SWI_UINT256_INT_VALUE};

uint8_t AnswerUSSDData[] = {
        1,5, 0x2A, 0x32, 0x32, 0x35, 0x23};
pack_voice_AnswerUSSD_t tpack_voice_AnswerUSSD = {AnswerUSSDData
        };
unpack_voice_AnswerUSSD_t tunpack_voice_AnswerUSSD = {0, SWI_UINT256_INT_VALUE};

unpack_voice_CancelUSSD_t tunpack_voice_CancelUSSD = {0, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceDialCall_t tpack_voice_SLQSVoiceDialCall = { "8427205870",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL};

uint8_t             callId = 1;
voice_alphaIDInfo   alphaidinfo;
uint8_t             ccresulttype;
voice_ccSUPSType    ccsupstype;

unpack_voice_SLQSVoiceDialCall_t tunpack_voice_SLQSVoiceDialCall = { &callId, &alphaidinfo,
        &ccresulttype, &ccsupstype, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceEndCall_t tpack_voice_SLQSVoiceEndCall = { &callId };
unpack_voice_SLQSVoiceEndCall_t tunpack_voice_SLQSVoiceEndCall = { &callId, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceSetSUPSService_t tpack_voice_SLQSVoiceSetSUPSService = { 0x01, 0x0F, NULL, NULL,
        NULL, NULL, NULL};
uint16_t          failCause;
unpack_voice_SLQSVoiceSetSUPSService_t  tunpack_voice_SLQSVoiceSetSUPSService = { &failCause, &alphaidinfo,
        &ccresulttype, &callId, &ccsupstype, SWI_UINT256_INT_VALUE};

uint8_t    autoanswer = 0x00;
pack_voice_SLQSVoiceSetConfig_t tpack_voice_SLQSVoiceSetConfig = { &autoanswer, NULL, NULL, NULL,
        NULL, NULL};

uint8_t autoansstatus;
uint8_t airtimerstatus;
uint8_t roamtimerstatus;
uint8_t TTYconfigstatus;
uint8_t prefvoicesostatus;
uint8_t voicedomainprefstatus;

unpack_voice_SLQSVoiceSetConfig_t tunpack_voice_SLQSVoiceSetConfig = { &autoansstatus, &airtimerstatus,
        &roamtimerstatus, &TTYconfigstatus, &prefvoicesostatus, &voicedomainprefstatus, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceAnswerCall_t tpack_voice_SLQSVoiceAnswerCall = { &callId };
unpack_voice_SLQSVoiceAnswerCall_t tunpack_voice_SLQSVoiceAnswerCall = { &callId, SWI_UINT256_INT_VALUE };

voice_CLIRResp    clirresp;
uint16_t        failcause;
uint8_t        respcallid;
unpack_voice_SLQSVoiceGetCLIR_t tunpack_voice_SLQSVoiceGetCLIR = { &clirresp, &failcause, &alphaidinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE };

voice_CLIPResp    clipresp;
unpack_voice_SLQSVoiceGetCLIP_t tunpack_voice_SLQSVoiceGetCLIP = {&clipresp, &failcause, &alphaidinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE };

uint8_t svcclass = 0;
pack_voice_SLQSVoiceGetCallWaiting_t tpack_voice_SLQSVoiceGetCallWaiting = { &svcclass };
unpack_voice_SLQSVoiceGetCallWaiting_t tunpack_voice_SLQSVoiceGetCallWaiting = {
        &svcclass, &failcause, &alphaidinfo, &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceGetCallBarring_t tpack_voice_SLQSVoiceGetCallBarring = { 0x07 , NULL };
unpack_voice_SLQSVoiceGetCallBarring_t tunpack_voice_SLQSVoiceGetCallBarring = {
        &svcclass, &failcause, &alphaidinfo, &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceGetCallForwardingStatus_t tpack_voice_SLQSVoiceGetCallForwardingStatus = { 0x06, NULL };

voice_getCallFWInfo    getcallfwinfo;
voice_getCallFWExtInfo getcallfwextinfo;
unpack_voice_SLQSVoiceGetCallForwardingStatus_t tunpack_voice_SLQSVoiceGetCallForwardingStatus = {
    &getcallfwinfo, &failcause, &alphaidinfo, &ccresulttype, &respcallid, &ccsupstype, &getcallfwextinfo, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceSetCallBarringPassword_t tpack_voice_SLQSVoiceSetCallBarringPassword = {0x0B ,
        "0000", "1234", "1234"};
unpack_voice_SLQSVoiceSetCallBarringPassword_t tunpack_voice_SLQSVoiceSetCallBarringPassword = {
        &failcause, &alphaidinfo, &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceGetCallInfo_t tpack_voice_SLQSVoiceGetCallInfo = { 0x01 };
voice_callInfo callinfo;
unpack_voice_SLQSVoiceGetCallInfo_t tunpack_voice_SLQSVoiceGetCallInfo = {
        &callinfo, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,SWI_UINT256_INT_VALUE
};

voice_arrCallInfo        arrcallinfo;
unpack_voice_SLQSVoiceGetAllCallInfo_t tunpack_voice_SLQSVoiceGetAllCallInfo = {
        &arrcallinfo, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, SWI_UINT256_INT_VALUE,
};

pack_voice_SLQSVoiceManageCalls_t tpack_voice_SLQSVoiceManageCalls = { 0x09, &callId};
unpack_voice_SLQSVoiceManageCalls_t tunpack_voice_SLQSVoiceManageCalls = { &failcause, SWI_UINT256_INT_VALUE };

voice_DTMFLengths       DTMFLengthsTest1 = {0x02, 0x03};
pack_voice_SLQSVoiceBurstDTMF_t tpack_voice_SLQSVoiceBurstDTMF = { {&callId, 3, "123"}, &DTMFLengthsTest1};
unpack_voice_SLQSVoiceBurstDTMF_t tunpack_voice_SLQSVoiceBurstDTMF = { &callId, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceStartContDTMF_t tpack_voice_SLQSVoiceStartContDTMF = { &callId, '2' };
unpack_voice_SLQSVoiceStartContDTMF_t tunpack_voice_SLQSVoiceStartContDTMF = { &callId, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceStopContDTMF_t tpack_voice_SLQSVoiceStopContDTMF = { 1 };
unpack_voice_SLQSVoiceStopContDTMF_t tunpack_voice_SLQSVoiceStopContDTMF;

uint8_t flashCallID = 1;
uint8_t FlashPayLd[] = "FlashPayLoad";
uint8_t flashType     = 0;

pack_voice_SLQSVoiceSendFlash_t tpack_voice_SLQSVoiceSendFlash = {
        &flashCallID, FlashPayLd, &flashType};

unpack_voice_SLQSVoiceSendFlash_t tunpack_voice_SLQSVoiceSendFlash = { &callId, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceSetPreferredPrivacy_t tpack_voice_SLQSVoiceSetPreferredPrivacy = { 0x00 };
unpack_voice_SLQSVoiceSetPreferredPrivacy_t tunpack_voice_SLQSVoiceSetPreferredPrivacy = {0, SWI_UINT256_INT_VALUE};

/* Indication Register Parameters */
uint8_t dtmfevent         = 0x01;
uint8_t voiceprivacyevent = 0x01;
uint8_t supsnotifyevent   = 0x01;
pack_voice_SLQSVoiceIndicationRegister_t tpack_voice_SLQSVoiceIndicationRegister = {
        &dtmfevent, &voiceprivacyevent, &supsnotifyevent };
unpack_voice_SLQSVoiceIndicationRegister_t tunpack_voice_SLQSVoiceIndicationRegister = {0, SWI_UINT256_INT_VALUE};

uint8_t setgetconf = 0x01;
pack_voice_SLQSVoiceGetConfig_t tpack_voice_SLQSVoiceGetConfig = {NULL, NULL, NULL, &setgetconf,
        NULL, NULL, NULL, NULL, NULL};

uint8_t         autoansstat;
voice_airTimer     airtimercnt;
voice_roamTimer    roamtimercnt;
uint8_t         currttymode;
voice_prefVoiceSO  currprefvoiceso;
voice_curAMRConfig curramrconfig;
uint8_t         currvoiceprivacy;
uint8_t         currvoicedomain;

unpack_voice_SLQSVoiceGetConfig_t tunpack_voice_SLQSVoiceGetConfig = {
        &autoansstat, &airtimercnt, &roamtimercnt, &currttymode, &currprefvoiceso,
        &curramrconfig, &currvoiceprivacy, &currvoicedomain, SWI_UINT256_INT_VALUE };

pack_voice_SLQSVoiceOrigUSSDNoWait_t tpack_voice_SLQSVoiceOrigUSSDNoWait = {{ 1, 5, { 0x2A, 0x32, 0x32, 0x35, 0x23 }}};
unpack_voice_SLQSVoiceOrigUSSDNoWait_t tunpack_voice_SLQSVoiceOrigUSSDNoWait = {0, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceBindSubscription_t tpack_voice_SLQSVoiceBindSubscription = { 0x00 };
unpack_voice_SLQSVoiceBindSubscription_t tunpack_voice_SLQSVoiceBindSubscription = {0, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceALSSetLineSwitching_t tpack_voice_SLQSVoiceALSSetLineSwitching = { 0x00 };
unpack_voice_SLQSVoiceALSSetLineSwitching_t tunpack_voice_SLQSVoiceALSSetLineSwitching = {0, SWI_UINT256_INT_VALUE};

pack_voice_SLQSVoiceALSSelectLine_t tpack_voice_SLQSVoiceALSSelectLine = { 0x00 };
unpack_voice_SLQSVoiceALSSelectLine_t tunpack_voice_SLQSVoiceALSSelectLine = {0, SWI_UINT256_INT_VALUE};

voice_COLPResp    colpresp;
unpack_voice_SLQSVoiceGetCOLP_t tunpack_voice_SLQSVoiceGetCOLP = { &colpresp, &failcause, &alphaidinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE};

voice_COLRResp    colrresp;
unpack_voice_SLQSVoiceGetCOLR_t tunpack_voice_SLQSVoiceGetCOLR = { &colrresp, &failcause, &alphaidinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE};

voice_CNAPResp    cnapresp;
unpack_voice_SLQSVoiceGetCNAP_t tunpack_voice_SLQSVoiceGetCNAP = { &cnapresp, &failcause, &alphaidinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE};

pack_voice_SLQSOriginateUSSD_t tpack_voice_SLQSOriginateUSSD = { 1, 5, { 0x2A, 0x32, 0x32, 0x35, 0x23 }};
struct voice_USSInfo ussinfo;
unpack_voice_SLQSOriginateUSSD_t tunpack_voice_SLQSOriginateUSSD = { &failcause, &alphaidinfo, &ussinfo,
        &ccresulttype, &respcallid, &ccsupstype, SWI_UINT256_INT_VALUE};
//function prototype
void test_voice_dummy_ind();

void dump_OriginateUSSD(void *ptr)
{
    unpack_voice_OriginateUSSD_t *result =
            (unpack_voice_OriginateUSSD_t *) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_AnswerUSSD(void *ptr)
{
    unpack_voice_AnswerUSSD_t *result =
            (unpack_voice_AnswerUSSD_t *) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_CancelUSSD(void *ptr)
{
    unpack_voice_CancelUSSD_t *result =
            (unpack_voice_CancelUSSD_t *) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceDialCall(void *ptr)
{
    unpack_voice_SLQSVoiceDialCall_t *result =
            (unpack_voice_SLQSVoiceDialCall_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallID);

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }
sleep(10);
if(iLocalLog == 0)
    return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceEndCall(void *ptr)
{
    unpack_voice_SLQSVoiceEndCall_t *result =
            (unpack_voice_SLQSVoiceEndCall_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallId) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallId);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceSetSUPSService(void *ptr)
{
    unpack_voice_SLQSVoiceSetSUPSService_t *result =
            (unpack_voice_SLQSVoiceSetSUPSService_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Failure Cause : %x \n", *result->pFailCause);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf("Call Id       : %x \n", *result->pCallID);
    if((result->pCCResultType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("CC Result     : %x \n", *result->pCCResultType);

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }
    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceSetConfig(void *ptr)
{
    unpack_voice_SLQSVoiceSetConfig_t *result =
            (unpack_voice_SLQSVoiceSetConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pAutoAnsStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Auto Answer Outcome :%x\n", *result->pAutoAnsStatus);
    if((result->pAirTimerStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Air Timer Outcome   :%x\n", *result->pAirTimerStatus);
    if((result->pRoamTimerStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf("Roam Timer Outcome  :%x\n", *result->pRoamTimerStatus);
    if((result->pTTYConfigStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("TTY Mode Outcome    :%x\n", *result->pTTYConfigStatus);
    if((result->pPrefVoiceSOStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Voice SO Outcome    :%x\n", *result->pPrefVoiceSOStatus);
    if((result->pVoiceDomainPrefStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
        printf("Voice Domain Outcome:%x\n", *result->pVoiceDomainPrefStatus);
    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceAnswerCall(void *ptr)
{
    unpack_voice_SLQSVoiceAnswerCall_t *result =
            (unpack_voice_SLQSVoiceAnswerCall_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallId) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallId);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCLIR(void *ptr)
{
    unpack_voice_SLQSVoiceGetCLIR_t *result =
            (unpack_voice_SLQSVoiceGetCLIR_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pCLIRResp ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("CLIR Response \n");
        printf("Activation Status  : %x\n",
                 result->pCLIRResp->ActiveStatus);
        printf("Provision Status   : %x\n",
                result->pCLIRResp->ProvisionStatus);
    }
    uint8_t lcount;
    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCLIP(void *ptr)
{
    unpack_voice_SLQSVoiceGetCLIP_t *result =
            (unpack_voice_SLQSVoiceGetCLIP_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pCLIPResp ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("CLIP Response \n");
        printf("Activation Status  : %x\n",
                 result->pCLIPResp->ActiveStatus);
        printf("Provision Status   : %x\n",
                result->pCLIPResp->ProvisionStatus);
    }
    
    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCallWaiting(void *ptr)
{
    unpack_voice_SLQSVoiceGetCallWaiting_t *result =
            (unpack_voice_SLQSVoiceGetCallWaiting_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Failure Cause      : %x\n",*result->pFailCause);
    if((result->pSvcClass) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Service Call(resp) : %x\n",*result->pSvcClass);

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Id            : %x\n",*result->pCallID);
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCallBarring(void *ptr)
{
    unpack_voice_SLQSVoiceGetCallBarring_t *result =
            (unpack_voice_SLQSVoiceGetCallBarring_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Failure Cause      : %x\n",*result->pFailCause);
    if((result->pSvcClass) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Service Call(resp) : %x\n",*result->pSvcClass);

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Id            : %x\n",*result->pCallID);
    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCallForwardingStatus(void *ptr)
{
    unpack_voice_SLQSVoiceGetCallForwardingStatus_t *result =
            (unpack_voice_SLQSVoiceGetCallForwardingStatus_t*) ptr;
    uint8_t lcount, lIcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pGetCallFWInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Instances of Call Forwarding Info : %x\n",
        result->pGetCallFWInfo->numInstances);
        for ( lcount = 0;lcount<result->pGetCallFWInfo->numInstances;lcount++)
        {
            voice_callFWInfo *temp = &result->pGetCallFWInfo->CallFWInfo[lcount];
            printf("\tService Status   : %x \n",temp->SvcStatus);
            printf("\tService Class    : %x \n",temp->SvcClass);
            printf("\tNum Len          : %x \n",temp->numLen);
            printf("\tNumber           : ");
            for ( lIcount=0; lIcount<temp->numLen ;lIcount++ )
            {
                printf("%c",temp->number[lIcount]);
            }
            printf("\n");
            printf("\tNo Reply Timer   : %x \n",temp->noReplyTimer);
        }
    }

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);

    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);

    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if (( NULL != result->pGetCallFWExtInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)))
    {
        printf("Instances of Call Forwarding Ext Info : %x\n",
                 result->pGetCallFWExtInfo->numInstances);
        for ( lcount = 0;lcount<result->pGetCallFWExtInfo->numInstances;lcount++)
        {
            voice_callFWExtInfo *temp = &result->pGetCallFWExtInfo->CallFWExtInfo[lcount];
            printf("\tService Status   : %x \n",temp->SvcStatus);
            printf("\tService Class    : %x \n",temp->SvcClass);
            printf("\tNo Reply Timer   : %x \n",temp->noReplyTimer);
            printf("\tPI               : %x \n",temp->PI);
            printf("\tSI               : %x \n",temp->SI);
            printf("\tNum Type         : %x \n",temp->numType);
            printf("\tNum Plan         : %x \n",temp->numPlan);
            printf("\tNum Len          : %x \n",temp->numLen);
            printf("\tNumber           : ");
            for ( lIcount=0; lIcount<temp->numLen ;lIcount++ )
            {
                printf("%c",temp->number[lIcount]);
            }
            printf("\n");
        }
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceSetCallBarringPassword(void *ptr)
{
    unpack_voice_SLQSVoiceSetCallBarringPassword_t *result =
            (unpack_voice_SLQSVoiceSetCallBarringPassword_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);

    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Id            : %x\n",*result->pCallID);

    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCallInfo(void *ptr)
{
    unpack_voice_SLQSVoiceGetCallInfo_t *resp =
            (unpack_voice_SLQSVoiceGetCallInfo_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);
    if (( NULL != resp->pCallInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 16)))
    {
        printf("Call Id(response): %x \n",resp->pCallInfo->callID);
        printf("Call State       : %x \n",resp->pCallInfo->callState);
        printf("Call Type        : %x \n",resp->pCallInfo->callType);
        printf("Direction        : %x \n",resp->pCallInfo->direction);
        printf("Mode             : %x \n",resp->pCallInfo->mode);
    }
    if (( NULL != resp->pRemotePartyNum ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 17)))
    {
        printf("PI               : %x \n",resp->pRemotePartyNum->presentationInd);
        printf("Num Len          : %d \n",resp->pRemotePartyNum->numLen);
        printf("Remote Party Number : ");
        for ( lcount=0; lcount<resp->pRemotePartyNum->numLen ;lcount++ )
        {
            printf("%c",resp->pRemotePartyNum->remPartyNumber[lcount]);
        }
        printf("\n");
    }
    if((resp->pSrvOpt) && (swi_uint256_get_bit (resp->ParamPresenceMask, 18)))
        printf("Service Option   : %x \n",*resp->pSrvOpt);

    if((resp->pVoicePrivacy) && (swi_uint256_get_bit (resp->ParamPresenceMask, 19)))
        printf("Voice Privacy    : %x \n",*resp->pVoicePrivacy);

    if((resp->pOTASPStatus) && (swi_uint256_get_bit (resp->ParamPresenceMask, 20)))
        printf("OTASP Status     : %x \n",*resp->pOTASPStatus);

    if (( NULL != resp->pRemotePartyName ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)))
    {
        printf("Name PI          : %x \n",resp->pRemotePartyName->namePI);
        printf("Coding Scheme    : %x \n",resp->pRemotePartyName->codingScheme);
        printf("Name Len         : %d \n",resp->pRemotePartyName->nameLen);
        printf("Remote Party Name : ");
        for ( lcount=0; lcount<resp->pRemotePartyName->nameLen ;lcount++ )
        {
            printf("%c",resp->pRemotePartyName->callerName[lcount]);
        }
        printf("\n");
    }

    if (( NULL != resp->pUUSInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 22)))
    {
        printf("UUS Type         : %x \n",resp->pUUSInfo->UUSType);
        printf("UUS DCS          : %x \n",resp->pUUSInfo->UUSDcs);
        printf("UUS Datalen      : %d \n",resp->pUUSInfo->UUSDatalen);
        printf("UUS Data         : ");
        for ( lcount=0; lcount<resp->pUUSInfo->UUSDatalen ;lcount++ )
        {
            printf("%c",resp->pUUSInfo->UUSData[lcount]);
        }
        printf("\n");
    }

    if((resp->pAlertType) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)))
        printf("Alerting Type    : %x \n",*resp->pAlertType);

    if((resp->pAlphaIDInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)))
    {
        printf("Alpha DCS     : %x \n", resp->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", resp->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < resp->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", resp->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if (( NULL != resp->pConnectNumInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)))
    {
        printf("connumPI         : %x \n",resp->pConnectNumInfo->numPresInd);
        printf("connumSI         : %x \n",resp->pConnectNumInfo->screeningInd);
        printf("Num Type         : %x \n",resp->pConnectNumInfo->numType);
        printf("Num Plan         : %x \n",resp->pConnectNumInfo->numPlan);
        printf("Caller ID Len    : %d \n",resp->pConnectNumInfo->callerIDLen);
        printf("Caller ID        : ");
        for ( lcount=0; lcount<resp->pConnectNumInfo->callerIDLen ;lcount++ )
        {
            printf("%c",resp->pConnectNumInfo->callerID[lcount]);
        }
        printf("\n");
    }
    if (( NULL != resp->pDiagInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 26)))
    {
        printf("Diagnostic Info Len: %x \n",resp->pDiagInfo->diagInfoLen);
        printf("Diagnostic Information : ");
        for ( lcount=0; lcount<resp->pDiagInfo->diagInfoLen ;lcount++ )
        {
            printf("%c",resp->pDiagInfo->diagnosticInfo[lcount]);
        }
        printf("\n");
    }
    if((resp->pAlertingPattern) && (swi_uint256_get_bit (resp->ParamPresenceMask, 27)))
        printf("Alerting Pattern : %u \n",*resp->pAlertingPattern);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetAllCallInfo(void *ptr)
{
    unpack_voice_SLQSVoiceGetAllCallInfo_t *resp =
            (unpack_voice_SLQSVoiceGetAllCallInfo_t*) ptr;
    uint8_t lcount, lIcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);
    if (( NULL != resp->pArrCallInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 16)))
    {
        printf("Instances of call Info : %d\n",
        resp->pArrCallInfo->numInstances);
        for ( lcount = 0;lcount<resp->pArrCallInfo->numInstances;lcount++)
        {
            voice_callInfo temp = resp->pArrCallInfo->getAllCallInfo[lcount].Callinfo;
            printf("Call Id            : %x \n",temp.callID);
            printf("\tCall State       : %x \n",temp.callState);
            printf("\tCall Type        : %x \n",temp.callType);
            printf("\tDirection        : %x \n",temp.direction);
            printf("\tMode             : %x \n",temp.mode);
            printf("\tMPI              : %x \n",
            resp->pArrCallInfo->getAllCallInfo[lcount].isEmpty);
            printf("\tALS              : %x \n",
            resp->pArrCallInfo->getAllCallInfo[lcount].ALS);
        }
    }
    if (( NULL != resp->pArrRemotePartyNum ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 17)))
    {
        printf("Instances of remote party numbers : %d\n",
        resp->pArrRemotePartyNum->numInstances);
        for ( lcount = 0;lcount<resp->pArrRemotePartyNum->numInstances;lcount++)
        {
            printf("Call Id          : %x \n",
              resp->pArrRemotePartyNum->RmtPtyNum[lcount].callID);
            voice_remotePartyNum temp =
              resp->pArrRemotePartyNum->RmtPtyNum[lcount].RemotePartyNum;
            printf("\tPI               : %x \n",temp.presentationInd);
            printf("\tNum Len          : %d \n",temp.numLen);
            printf("\tRemote Party Number : ");
            for ( lIcount=0; lIcount<temp.numLen ;lIcount++ )
            {
                printf("%c",temp.remPartyNumber[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrRemotePartyName ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 18)))
    {
        printf("Instances of remote party names : %d\n",
        resp->pArrRemotePartyName->numInstances);
        for ( lcount = 0;lcount<resp->pArrRemotePartyName->numInstances;lcount++)
        {
            printf("Call Id          : %x \n",
              resp->pArrRemotePartyName->GetAllCallRmtPtyName[lcount].callID);
            voice_remotePartyName temp =
              resp->pArrRemotePartyName->GetAllCallRmtPtyName[lcount].RemotePartyName;
            printf("\tName PI          : %x \n",temp.namePI);
            printf("\tCoding Scheme    : %x \n",temp.codingScheme);
            printf("\tName Len         : %d \n",temp.nameLen);
            printf("\tRemote Party Name : ");
            for ( lIcount=0; lIcount<temp.nameLen ;lIcount++ )
            {
                printf("%c",temp.callerName[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrAlertingType ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 19)))
    {
        printf("Instances of Alerting Types : %d\n",
        resp->pArrAlertingType->numInstances);
        for ( lcount = 0;lcount<resp->pArrAlertingType->numInstances;lcount++)
        {
            printf("Call Id          : %x \t",
              resp->pArrAlertingType->callID[lcount]);
            printf("Alerting Type    : %x \n",
              resp->pArrAlertingType->AlertingType[lcount]);
        }
    }
    if (( NULL != resp->pArrUUSInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 20)))
    {
        printf("Instances of UUS Info : %d\n",
        resp->pArrUUSInfo->numInstances);
        for ( lcount = 0;lcount<resp->pArrUUSInfo->numInstances;lcount++)
        {
            printf("Call Id          : %x \n",
              resp->pArrUUSInfo->AllCallsUUSInfo[lcount].callID);
            voice_UUSInfo temp = resp->pArrUUSInfo->AllCallsUUSInfo[lcount].uusInfo;
            printf("\tUUS Type         : %x \n",temp.UUSType);
            printf("\tUUS DCS          : %x \n",temp.UUSDcs);
            printf("\tUUS Datalen      : %d \n",temp.UUSDatalen);
            printf("\tUUS Data         : ");
            for ( lIcount=0; lIcount<temp.UUSDatalen ;lIcount++ )
            {
                printf("%c",temp.UUSData[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrSvcOption ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)))
    {
        printf("Instances of Service Option : %d\n",
        resp->pArrSvcOption->numInstances);
        for ( lcount = 0;lcount<resp->pArrSvcOption->numInstances;lcount++)
        {
            printf("Call Id          : %x \t",
              resp->pArrSvcOption->callID[lcount]);
            printf("Service Option   : %d \n",
              resp->pArrSvcOption->srvOption[lcount]);
        }
    }
    if((resp->pOTASPStatus) && (swi_uint256_get_bit (resp->ParamPresenceMask, 22)))
        printf("OTASP Status     : %x \n",*resp->pOTASPStatus);
    if((resp->pVoicePrivacy) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)))
        printf("Voice Privacy    : %x \n",*resp->pVoicePrivacy);

    if (( NULL != resp->pArrCallEndReason ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)))
    {
        printf("Instances of Call End Reason : %d\n",
        resp->pArrCallEndReason->numInstances);
        for ( lcount = 0;lcount<resp->pArrCallEndReason->numInstances;lcount++)
        {
            printf("Call Id          : %x \t",
              resp->pArrCallEndReason->callID[lcount]);
            printf("Call End Reason  : %d \n",
              resp->pArrCallEndReason->callEndReason[lcount]);
        }
    }
    if (( NULL != resp->pArrAlphaID ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)))
    {
        printf("Instances of Alpha Identifier : %d\n",
        resp->pArrAlphaID->numInstances);
        for ( lcount = 0;lcount<resp->pArrAlphaID->numInstances;lcount++)
        {
            printf("Call Id          : %x \n",
              resp->pArrAlphaID->allCallsAlphaIDInfoArr[lcount].callID);
            voice_alphaIDInfo temp =
              resp->pArrAlphaID->allCallsAlphaIDInfoArr[lcount].AlphaIDInfo;
            printf("\tAlpha DCS        : %x \n",temp.alphaDcs);
            printf("\tAlpha Len        : %d \n",temp.alphaLen);
            printf("\tAlpha Text       : ");
            for ( lIcount=0; lIcount<temp.alphaLen ;lIcount++ )
            {
                printf("%c",temp.alphaText[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrConnectPartyNum ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 26)))
    {
        printf("Instances of Connected Party Numbers : %d\n",
        resp->pArrConnectPartyNum->numInstances);
        for ( lcount = 0;lcount<resp->pArrConnectPartyNum->numInstances;lcount++)
        {
            voice_peerNumberInfo temp =
              resp->pArrConnectPartyNum->ConnectedPartyNum[lcount];
            printf("Call Id          : %x \n",temp.callID);
            printf("\tNum PI           : %x \n",temp.numPI);
            printf("\tNum SI           : %x \n",temp.numSI);
            printf("\tNum Type         : %x \n",temp.numType);
            printf("\tNum Plan         : %x \n",temp.numPlan);
            printf("\tNumber Len       : %d \n",temp.numLen);
            printf("\tNumber           : ");
            for ( lIcount=0; lIcount<temp.numLen ;lIcount++ )
            {
                printf("%c",temp.number[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrDiagInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 27)))
    {
        printf("Instances of Diagnostic Information : %d\n",
        resp->pArrDiagInfo->numInstances);
        for ( lcount = 0;lcount<resp->pArrDiagInfo->numInstances;lcount++)
        {
            printf("Call Id          : %x \n",
            resp->pArrDiagInfo->DiagInfo[lcount].callID);
            voice_diagInfo temp = resp->pArrDiagInfo->DiagInfo[lcount].DiagInfo;
            printf("\tDiagnostic Info Len: %d \n",temp.diagInfoLen);
            printf("\tDiagnostic Information : ");
            for ( lIcount=0; lIcount<temp.diagInfoLen ;lIcount++ )
            {
                printf("%c",temp.diagnosticInfo[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrCalledPartyNum ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 28)))
    {
        printf("Instances of Called Party Numbers : %d\n",
        resp->pArrCalledPartyNum->numInstances);
        for ( lcount = 0;lcount<resp->pArrCalledPartyNum->numInstances;lcount++)
        {
            voice_peerNumberInfo temp =
              resp->pArrCalledPartyNum->CalledPartyNum[lcount];
            printf("Call Id          : %x \n",temp.callID);
            printf("\tNum PI           : %x \n",temp.numPI);
            printf("\tNum SI           : %x \n",temp.numSI);
            printf("\tNum Type         : %x \n",temp.numType);
            printf("\tNum Plan         : %x \n",temp.numPlan);
            printf("\tNumber Len       : %d \n",temp.numLen);
            printf("\tNumber           : ");
            for ( lIcount=0; lIcount<temp.numLen ;lIcount++ )
            {
                printf("%c",temp.number[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrRedirPartyNum ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 29)))
    {
        printf("Instances of Redirected Party Numbers : %d\n",
        resp->pArrRedirPartyNum->numInstances);
        for ( lcount = 0;lcount<resp->pArrRedirPartyNum->numInstances;lcount++)
        {
            voice_peerNumberInfo temp =
              resp->pArrRedirPartyNum->RedirPartyNum[lcount];
            printf("Call Id          : %x \n",temp.callID);
            printf("\tNum PI           : %x \n",temp.numPI);
            printf("\tNum SI           : %x \n",temp.numSI);
            printf("\tNum Type         : %x \n",temp.numType);
            printf("\tNum Plan         : %x \n",temp.numPlan);
            printf("\tNumber Len       : %d \n",temp.numLen);
            printf("\tNumber           : ");
            for ( lIcount=0; lIcount<temp.numLen ;lIcount++ )
            {
                printf("%c",temp.number[lIcount]);
            }
            printf("\n");
        }
    }
    if (( NULL != resp->pArrAlertingPattern ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 30)))
    {
        printf("Instances of Alerting Pattern : %d\n",
        resp->pArrAlertingPattern->numInstances);
        for ( lcount = 0;lcount<resp->pArrAlertingPattern->numInstances;lcount++)
        {
            printf("Call Id          : %x \t",
              resp->pArrAlertingPattern->callID[lcount]);
            printf("Call End Reason  : %u \n",
              resp->pArrAlertingPattern->alertingPattern[lcount]);
        }
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceManageCalls(void *ptr)
{
    unpack_voice_SLQSVoiceManageCalls_t *result =
            (unpack_voice_SLQSVoiceManageCalls_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Failure Cause : %x \n", *result->pFailCause);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceBurstDTMF(void *ptr)
{
    unpack_voice_SLQSVoiceBurstDTMF_t *result =
            (unpack_voice_SLQSVoiceBurstDTMF_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallID);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceStartContDTMF(void *ptr)
{
    unpack_voice_SLQSVoiceStartContDTMF_t *result =
            (unpack_voice_SLQSVoiceStartContDTMF_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallID);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceStopContDTMF(void *ptr)
{
    unpack_voice_SLQSVoiceStopContDTMF_t *result =
            (unpack_voice_SLQSVoiceStopContDTMF_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("Call Id        : %x \n",result->callID);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceSendFlash(void *ptr)
{
    unpack_voice_SLQSVoiceSendFlash_t *result =
            (unpack_voice_SLQSVoiceSendFlash_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Call Id        : %x \n",*result->pCallID);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceSetPreferredPrivacy(void *ptr)
{
    unpack_voice_SLQSVoiceSetPreferredPrivacy_t *result =
            (unpack_voice_SLQSVoiceSetPreferredPrivacy_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceIndicationRegister(void *ptr)
{
    unpack_voice_SLQSVoiceIndicationRegister_t *result =
            (unpack_voice_SLQSVoiceIndicationRegister_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetConfig(void *ptr)
{
    unpack_voice_SLQSVoiceGetConfig_t *result =
            (unpack_voice_SLQSVoiceGetConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pAutoAnswerStat) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Auto Answer Status :%x\n",*result->pAutoAnswerStat);

    if (( NULL != result->pAirTimerCnt ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("NAM-Id(AIR)     :%x\n",result->pAirTimerCnt->namID);
        printf("Air Timer       :%x\n",result->pAirTimerCnt->airTimerValue);
    }

    if (( NULL != result->pRoamTimerCnt ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("NAM-Id(ROAM)    :%x\n",result->pRoamTimerCnt->namID);
        printf("ROAM Timer      :%x\n",result->pRoamTimerCnt->roamTimerValue);
    }

    if((result->pCurrTTYMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Current TTY Mode:%x\n",*result->pCurrTTYMode);

    if (( NULL != result->pCurPrefVoiceSO ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("NAM-Id(Voice SO):%x\n",result->pCurPrefVoiceSO->namID);
        printf("EVRC Capability :%x\n",result->pCurPrefVoiceSO->evrcCapability);
        printf("Home Page VSO   :%x\n",result->pCurPrefVoiceSO->homePageVoiceSO);
        printf("Home Orig VSO   :%x\n",result->pCurPrefVoiceSO->homeOrigVoiceSO);
        printf("Roam Orig VSO   :%x\n",result->pCurPrefVoiceSO->roamOrigVoiceSO);
    }

    if (( NULL != result->pCurAMRConfig ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("GSM AMR Status  :%x\n",result->pCurAMRConfig->gsmAmrStat);
        printf("WCDMA AMR Status:%x\n",result->pCurAMRConfig->wcdmaAmrStat);
    }

    if((result->pCurVoicePrivacyPref) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)))
        printf("Current Voice Privacy:%x\n",*result->pCurVoicePrivacyPref);

    if((result->pCurVoiceDomainPref) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)))
        printf("Current Voice Domain :%x\n",*result->pCurVoiceDomainPref);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceOrigUSSDNoWait(void *ptr)
{
    unpack_voice_SLQSVoiceOrigUSSDNoWait_t *result =
            (unpack_voice_SLQSVoiceOrigUSSDNoWait_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceBindSubscription(void *ptr)
{
    unpack_voice_SLQSVoiceBindSubscription_t *result =
            (unpack_voice_SLQSVoiceBindSubscription_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceALSSetLineSwitching(void *ptr)
{
    unpack_voice_SLQSVoiceALSSetLineSwitching_t *result =
            (unpack_voice_SLQSVoiceALSSetLineSwitching_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceALSSelectLine(void *ptr)
{
    unpack_voice_SLQSVoiceALSSelectLine_t *result =
            (unpack_voice_SLQSVoiceALSSelectLine_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCOLP(void *ptr)
{
    unpack_voice_SLQSVoiceGetCOLP_t *result =
            (unpack_voice_SLQSVoiceGetCOLP_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pCOLPResp ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("COLP Response \n");
        printf("Activation Status  : %x\n",
                result->pCOLPResp->ActiveStatus);
        printf("Provision Status   : %x\n",
                        result->pCOLPResp->ProvisionStatus);
    }
    
    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);

    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);

    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCOLR(void *ptr)
{
    unpack_voice_SLQSVoiceGetCOLR_t *result =
            (unpack_voice_SLQSVoiceGetCOLR_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pCOLRResp ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("COLR Response \n");
        printf("Activation Status  : %x\n",
                result->pCOLRResp->ActiveStatus);
        printf("Provision Status   : %x\n",
                        result->pCOLRResp->ProvisionStatus);
    }
    uint8_t lcount;
    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);

    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);

    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetCNAP(void *ptr)
{
    unpack_voice_SLQSVoiceGetCNAP_t *result =
            (unpack_voice_SLQSVoiceGetCNAP_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if((result->pFailCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf("Failure Cause      : %x\n",*result->pFailCause);

    if (( NULL != result->pCNAPResp ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("CNAP Response \n");
        printf("Activation Status  : %x\n",
                result->pCNAPResp->ActiveStatus);
        printf("Provision Status   : %x\n",
                        result->pCNAPResp->ProvisionStatus);
    }

    uint8_t lcount;
    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCCResType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCCResType);

    if((result->pCallID) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallID);

    if((result->pCCSUPSType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSUPSType->svcType);
        printf("Reason        : %x\n", result->pCCSUPSType->reason);
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSOriginateUSSD(void *ptr)
{
    unpack_voice_SLQSOriginateUSSD_t *result =
            (unpack_voice_SLQSOriginateUSSD_t*) ptr;
    uint8_t lcount;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pfailureCause) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("Failure Cause      : %x\n",*result->pfailureCause);

    if (( NULL != result->pUSSDInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("USSD Ifo \n");
        printf("USS DCS  : %x\n",
                result->pUSSDInfo->ussDCS);
        printf("USS Data   : ");
        for (lcount = 0;lcount < result->pUSSDInfo->ussLen;lcount++)
        {
             printf("%c", result->pUSSDInfo->ussData[lcount]);
        }
        printf("\n");
    }

    if((result->pAlphaIDInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Alpha DCS     : %x \n", result->pAlphaIDInfo->alphaDcs);
        printf("Alpha Len     : %d \n", result->pAlphaIDInfo->alphaLen);
        printf("Alpha Text    : ");
        for (lcount = 0;lcount < result->pAlphaIDInfo->alphaLen;lcount++)
        {
             printf("%c", result->pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }

    if((result->pCcResultType) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("Call Control Result Type : %x\n",*result->pCcResultType);

    if((result->pCallId) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
        printf("Call Id            : %x\n",*result->pCallId);

    if((result->pCCSuppsType) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("Service Type  : %x\n", result->pCCSuppsType->svcType);
        printf("Reason        : %x\n", result->pCCSuppsType->reason);
    }
    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSVoiceGetConfigSettings(void *ptr)
{
    unpack_voice_SLQSVoiceGetConfig_t *result =
            (unpack_voice_SLQSVoiceGetConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pAutoAnswerStat) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Auto Answer Status :%x\n",*result->pAutoAnswerStat);
        *(DefaultVoiceSetConfigSettings.pAutoAnswer) = *result->pAutoAnswerStat;
    }
    if (( NULL != result->pAirTimerCnt ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("NAM-Id(AIR)     :%x\n",result->pAirTimerCnt->namID);
        printf("Air Timer       :%x\n",result->pAirTimerCnt->airTimerValue);
        memcpy(DefaultVoiceSetConfigSettings.pAirTimerConfig,
        result->pAirTimerCnt,
        sizeof(voice_airTimer));
    }
    if (( NULL != result->pRoamTimerCnt ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("NAM-Id(ROAM)    :%x\n",result->pRoamTimerCnt->namID);
        printf("ROAM Timer      :%x\n",result->pRoamTimerCnt->roamTimerValue);
        memcpy(DefaultVoiceSetConfigSettings.pRoamTimerConfig,
            result->pRoamTimerCnt,
            sizeof(voice_roamTimer));
    }
    if((result->pCurrTTYMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Current TTY Mode:%x\n",*result->pCurrTTYMode);
        *(DefaultVoiceSetConfigSettings.pTTYMode) = *result->pCurrTTYMode;
    }
    if (( NULL != result->pCurPrefVoiceSO ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("NAM-Id(Voice SO):%x\n",result->pCurPrefVoiceSO->namID);
        printf("EVRC Capability :%x\n",result->pCurPrefVoiceSO->evrcCapability);
        printf("Home Page VSO   :%x\n",result->pCurPrefVoiceSO->homePageVoiceSO);
        printf("Home Orig VSO   :%x\n",result->pCurPrefVoiceSO->homeOrigVoiceSO);
        printf("Roam Orig VSO   :%x\n",result->pCurPrefVoiceSO->roamOrigVoiceSO);
        memcpy(DefaultVoiceSetConfigSettings.pPrefVoiceSO,
            result->pCurPrefVoiceSO,
            sizeof(voice_prefVoiceSO));
    }
    if (( NULL != result->pCurAMRConfig ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)))
    {
        printf("GSM AMR Status  :%x\n",result->pCurAMRConfig->gsmAmrStat);
        printf("WCDMA AMR Status:%x\n",result->pCurAMRConfig->wcdmaAmrStat);
    }
    if((result->pCurVoicePrivacyPref) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)))
    {
        printf("Current Voice Privacy:%x\n",*result->pCurVoicePrivacyPref);
    }
    if((result->pCurVoiceDomainPref) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)))
    {
        printf("Current Voice Domain :%x\n",*result->pCurVoiceDomainPref);
        *(DefaultVoiceSetConfigSettings.pPrefVoiceDomain) = *result->pCurVoiceDomainPref;
    }
    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

testitem_t voicetotest[] = {
    ////Get Modem Settings
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetConfig, "pack_voice_SLQSVoiceGetConfig",
        &tpack_voice_SLQSVoiceGetConfig,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetConfig, "unpack_voice_SLQSVoiceGetConfig",
        &tunpack_voice_SLQSVoiceGetConfig, dump_SLQSVoiceGetConfigSettings
    },
    /////////////////////////////
    {
        (pack_func_item) &pack_voice_OriginateUSSD, "pack_voice_OriginateUSSD",
        &tpack_voice_OriginateUSSD,
        (unpack_func_item) &unpack_voice_OriginateUSSD, "unpack_voice_OriginateUSSD",
        &tunpack_voice_OriginateUSSD, dump_OriginateUSSD
    },
    {
        (pack_func_item) &pack_voice_AnswerUSSD, "pack_voice_AnswerUSSD",
        &tpack_voice_AnswerUSSD,
        (unpack_func_item) &unpack_voice_AnswerUSSD, "unpack_voice_AnswerUSSD",
        &tunpack_voice_AnswerUSSD, dump_AnswerUSSD
    },
    {
        (pack_func_item) &pack_voice_CancelUSSD, "pack_voice_CancelUSSD",
        NULL,
        (unpack_func_item) &unpack_voice_CancelUSSD, "unpack_voice_CancelUSSD",
        &tunpack_voice_CancelUSSD, dump_CancelUSSD
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceDialCall, "pack_voice_SLQSVoiceDialCall",
        &tpack_voice_SLQSVoiceDialCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceDialCall, "unpack_voice_SLQSVoiceDialCall",
        &tunpack_voice_SLQSVoiceDialCall, dump_SLQSVoiceDialCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceEndCall, "pack_voice_SLQSVoiceEndCall",
        &tpack_voice_SLQSVoiceEndCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceEndCall, "unpack_voice_SLQSVoiceEndCall",
        &tunpack_voice_SLQSVoiceEndCall, dump_SLQSVoiceEndCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetSUPSService, "pack_voice_SLQSVoiceSetSUPSService",
        &tpack_voice_SLQSVoiceSetSUPSService,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetSUPSService, "unpack_voice_SLQSVoiceSetSUPSService",
        &tunpack_voice_SLQSVoiceSetSUPSService, dump_SLQSVoiceSetSUPSService
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetConfig, "pack_voice_SLQSVoiceSetConfig",
        &tpack_voice_SLQSVoiceSetConfig,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetConfig, "unpack_voice_SLQSVoiceSetConfig",
        &tunpack_voice_SLQSVoiceSetConfig, dump_SLQSVoiceSetConfig
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceAnswerCall, "pack_voice_SLQSVoiceAnswerCall",
        &tpack_voice_SLQSVoiceAnswerCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceAnswerCall, "unpack_voice_SLQSVoiceAnswerCall",
        &tunpack_voice_SLQSVoiceAnswerCall, dump_SLQSVoiceAnswerCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCLIR, "pack_voice_SLQSVoiceGetCLIR",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCLIR, "unpack_voice_SLQSVoiceGetCLIR",
        &tunpack_voice_SLQSVoiceGetCLIR, dump_SLQSVoiceGetCLIR
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCLIP, "pack_voice_SLQSVoiceGetCLIP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCLIP, "unpack_voice_SLQSVoiceGetCLIP",
        &tunpack_voice_SLQSVoiceGetCLIP, dump_SLQSVoiceGetCLIP
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallWaiting, "pack_voice_SLQSVoiceGetCallWaiting",
        &tpack_voice_SLQSVoiceGetCallWaiting,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallWaiting, "unpack_voice_SLQSVoiceGetCallWaiting",
        &tunpack_voice_SLQSVoiceGetCallWaiting, dump_SLQSVoiceGetCallWaiting
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallBarring, "pack_voice_SLQSVoiceGetCallBarring",
        &tpack_voice_SLQSVoiceGetCallBarring,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallBarring, "unpack_voice_SLQSVoiceGetCallBarring",
        &tunpack_voice_SLQSVoiceGetCallBarring, dump_SLQSVoiceGetCallBarring
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallForwardingStatus, "pack_voice_SLQSVoiceGetCallForwardingStatus",
        &tpack_voice_SLQSVoiceGetCallForwardingStatus,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallForwardingStatus, "unpack_voice_SLQSVoiceGetCallForwardingStatus",
        &tunpack_voice_SLQSVoiceGetCallForwardingStatus, dump_SLQSVoiceGetCallForwardingStatus
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetCallBarringPassword, "pack_voice_SLQSVoiceSetCallBarringPassword",
        &tpack_voice_SLQSVoiceSetCallBarringPassword,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetCallBarringPassword, "unpack_voice_SLQSVoiceSetCallBarringPassword",
        &tunpack_voice_SLQSVoiceSetCallBarringPassword, dump_SLQSVoiceSetCallBarringPassword
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallInfo, "pack_voice_SLQSVoiceGetCallInfo",
        &tpack_voice_SLQSVoiceGetCallInfo,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallInfo, "unpack_voice_SLQSVoiceGetCallInfo",
        &tunpack_voice_SLQSVoiceGetCallInfo, dump_SLQSVoiceGetCallInfo
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetAllCallInfo, "pack_voice_SLQSVoiceGetAllCallInfo",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetAllCallInfo, "unpack_voice_SLQSVoiceGetAllCallInfo",
        &tunpack_voice_SLQSVoiceGetAllCallInfo, dump_SLQSVoiceGetAllCallInfo
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceManageCalls, "pack_voice_SLQSVoiceManageCalls",
        &tpack_voice_SLQSVoiceManageCalls,
        (unpack_func_item) &unpack_voice_SLQSVoiceManageCalls, "unpack_voice_SLQSVoiceManageCalls",
        &tunpack_voice_SLQSVoiceManageCalls, dump_SLQSVoiceManageCalls
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceBurstDTMF, "pack_voice_SLQSVoiceBurstDTMF",
        &tpack_voice_SLQSVoiceBurstDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceBurstDTMF, "unpack_voice_SLQSVoiceBurstDTMF",
        &tunpack_voice_SLQSVoiceBurstDTMF, dump_SLQSVoiceBurstDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceStartContDTMF, "pack_voice_SLQSVoiceStartContDTMF",
        &tpack_voice_SLQSVoiceStartContDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceStartContDTMF, "unpack_voice_SLQSVoiceStartContDTMF",
        &tunpack_voice_SLQSVoiceStartContDTMF, dump_SLQSVoiceStartContDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceStopContDTMF, "pack_voice_SLQSVoiceStopContDTMF",
        &tpack_voice_SLQSVoiceStopContDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceStopContDTMF, "unpack_voice_SLQSVoiceStopContDTMF",
        &tunpack_voice_SLQSVoiceStopContDTMF, dump_SLQSVoiceStopContDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSendFlash, "pack_voice_SLQSVoiceSendFlash",
        &tpack_voice_SLQSVoiceSendFlash,
        (unpack_func_item) &unpack_voice_SLQSVoiceSendFlash, "unpack_voice_SLQSVoiceSendFlash",
        &tunpack_voice_SLQSVoiceSendFlash, dump_SLQSVoiceSendFlash
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetPreferredPrivacy, "pack_voice_SLQSVoiceSetPreferredPrivacy",
        &tpack_voice_SLQSVoiceSetPreferredPrivacy,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetPreferredPrivacy, "unpack_voice_SLQSVoiceSetPreferredPrivacy",
        &tunpack_voice_SLQSVoiceSetPreferredPrivacy, dump_SLQSVoiceSetPreferredPrivacy
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceIndicationRegister, "pack_voice_SLQSVoiceIndicationRegister",
        &tpack_voice_SLQSVoiceIndicationRegister,
        (unpack_func_item) &unpack_voice_SLQSVoiceIndicationRegister, "unpack_voice_SLQSVoiceIndicationRegister",
        &tunpack_voice_SLQSVoiceIndicationRegister, dump_SLQSVoiceIndicationRegister
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetConfig, "pack_voice_SLQSVoiceGetConfig",
        &tpack_voice_SLQSVoiceGetConfig,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetConfig, "unpack_voice_SLQSVoiceGetConfig",
        &tunpack_voice_SLQSVoiceGetConfig, dump_SLQSVoiceGetConfig
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceOrigUSSDNoWait, "pack_voice_SLQSVoiceOrigUSSDNoWait",
        &tpack_voice_SLQSVoiceOrigUSSDNoWait,
        (unpack_func_item) &unpack_voice_SLQSVoiceOrigUSSDNoWait, "unpack_voice_SLQSVoiceOrigUSSDNoWait",
        &tunpack_voice_SLQSVoiceOrigUSSDNoWait, dump_SLQSVoiceOrigUSSDNoWait
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceBindSubscription, "pack_voice_SLQSVoiceBindSubscription",
        &tpack_voice_SLQSVoiceBindSubscription,
        (unpack_func_item) &unpack_voice_SLQSVoiceBindSubscription, "unpack_voice_SLQSVoiceBindSubscription",
        &tunpack_voice_SLQSVoiceBindSubscription, dump_SLQSVoiceBindSubscription
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceALSSetLineSwitching, "pack_voice_SLQSVoiceALSSetLineSwitching",
        &tpack_voice_SLQSVoiceALSSetLineSwitching,
        (unpack_func_item) &unpack_voice_SLQSVoiceALSSetLineSwitching, "unpack_voice_SLQSVoiceALSSetLineSwitching",
        &tunpack_voice_SLQSVoiceALSSetLineSwitching, dump_SLQSVoiceALSSetLineSwitching
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceALSSelectLine, "pack_voice_SLQSVoiceALSSelectLine",
        &tpack_voice_SLQSVoiceALSSelectLine,
        (unpack_func_item) &unpack_voice_SLQSVoiceALSSelectLine, "unpack_voice_SLQSVoiceALSSelectLine",
        &tunpack_voice_SLQSVoiceALSSelectLine, dump_SLQSVoiceALSSelectLine
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCOLP, "pack_voice_SLQSVoiceGetCOLP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCOLP, "unpack_voice_SLQSVoiceGetCOLP",
        &tunpack_voice_SLQSVoiceGetCOLP, dump_SLQSVoiceGetCOLP
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCOLR, "pack_voice_SLQSVoiceGetCOLR",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCOLR, "unpack_voice_SLQSVoiceGetCOLR",
        &tunpack_voice_SLQSVoiceGetCOLR, dump_SLQSVoiceGetCOLR
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCNAP, "pack_voice_SLQSVoiceGetCNAP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCNAP, "unpack_voice_SLQSVoiceGetCNAP",
        &tunpack_voice_SLQSVoiceGetCNAP, dump_SLQSVoiceGetCNAP
    },
    {
        (pack_func_item) &pack_voice_SLQSOriginateUSSD, "pack_voice_SLQSOriginateUSSD",
        &tpack_voice_SLQSOriginateUSSD,
        (unpack_func_item) &unpack_voice_SLQSOriginateUSSD, "unpack_voice_SLQSOriginateUSSD",
        &tunpack_voice_SLQSOriginateUSSD, dump_SLQSOriginateUSSD
    },
    //// Restore Modem Settings.
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetConfig, "pack_voice_SLQSVoiceSetConfig",
        &DefaultVoiceSetConfigSettings,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetConfig, "unpack_voice_SLQSVoiceSetConfig",
        &tunpack_voice_SLQSVoiceSetConfig, dump_SLQSVoiceSetConfig
    },
    //////////////////////////////////
};

unsigned int voicearraylen = (unsigned int)((sizeof(voicetotest))/(sizeof(voicetotest[0])));

testitem_t voicetotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetConfig, "pack_voice_SLQSVoiceGetConfig",
        &tpack_voice_SLQSVoiceGetConfig,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetConfig, "unpack_voice_SLQSVoiceGetConfig",
        NULL, dump_SLQSVoiceGetConfigSettings
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceDialCall, "pack_voice_SLQSVoiceDialCall",
        &tpack_voice_SLQSVoiceDialCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceDialCall, "unpack_voice_SLQSVoiceDialCall",
        NULL, dump_SLQSVoiceDialCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceEndCall, "pack_voice_SLQSVoiceEndCall",
        &tpack_voice_SLQSVoiceEndCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceEndCall, "unpack_voice_SLQSVoiceEndCall",
        NULL, dump_SLQSVoiceEndCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSetSUPSService, "pack_voice_SLQSVoiceSetSUPSService",
        &tpack_voice_SLQSVoiceSetSUPSService,
        (unpack_func_item) &unpack_voice_SLQSVoiceSetSUPSService, "unpack_voice_SLQSVoiceSetSUPSService",
        NULL, dump_SLQSVoiceSetSUPSService
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceAnswerCall, "pack_voice_SLQSVoiceAnswerCall",
        &tpack_voice_SLQSVoiceAnswerCall,
        (unpack_func_item) &unpack_voice_SLQSVoiceAnswerCall, "unpack_voice_SLQSVoiceAnswerCall",
        NULL, dump_SLQSVoiceAnswerCall
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCLIR, "pack_voice_SLQSVoiceGetCLIR",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCLIR, "unpack_voice_SLQSVoiceGetCLIR",
        NULL, dump_SLQSVoiceGetCLIR
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCLIP, "pack_voice_SLQSVoiceGetCLIP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCLIP, "unpack_voice_SLQSVoiceGetCLIP",
        NULL, dump_SLQSVoiceGetCLIP
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallWaiting, "pack_voice_SLQSVoiceGetCallWaiting",
        &tpack_voice_SLQSVoiceGetCallWaiting,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallWaiting, "unpack_voice_SLQSVoiceGetCallWaiting",
        NULL, dump_SLQSVoiceGetCallWaiting
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallBarring, "pack_voice_SLQSVoiceGetCallBarring",
        &tpack_voice_SLQSVoiceGetCallBarring,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallBarring, "unpack_voice_SLQSVoiceGetCallBarring",
        NULL, dump_SLQSVoiceGetCallBarring
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallForwardingStatus, "pack_voice_SLQSVoiceGetCallForwardingStatus",
        &tpack_voice_SLQSVoiceGetCallForwardingStatus,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallForwardingStatus, "unpack_voice_SLQSVoiceGetCallForwardingStatus",
        NULL, dump_SLQSVoiceGetCallForwardingStatus
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCallInfo, "pack_voice_SLQSVoiceGetCallInfo",
        &tpack_voice_SLQSVoiceGetCallInfo,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCallInfo, "unpack_voice_SLQSVoiceGetCallInfo",
        NULL, dump_SLQSVoiceGetCallInfo
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetAllCallInfo, "pack_voice_SLQSVoiceGetAllCallInfo",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetAllCallInfo, "unpack_voice_SLQSVoiceGetAllCallInfo",
        NULL, dump_SLQSVoiceGetAllCallInfo
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceManageCalls, "pack_voice_SLQSVoiceManageCalls",
        &tpack_voice_SLQSVoiceManageCalls,
        (unpack_func_item) &unpack_voice_SLQSVoiceManageCalls, "unpack_voice_SLQSVoiceManageCalls",
        NULL, dump_SLQSVoiceManageCalls
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceBurstDTMF, "pack_voice_SLQSVoiceBurstDTMF",
        &tpack_voice_SLQSVoiceBurstDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceBurstDTMF, "unpack_voice_SLQSVoiceBurstDTMF",
        NULL, dump_SLQSVoiceBurstDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceStartContDTMF, "pack_voice_SLQSVoiceStartContDTMF",
        &tpack_voice_SLQSVoiceStartContDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceStartContDTMF, "unpack_voice_SLQSVoiceStartContDTMF",
        NULL, dump_SLQSVoiceStartContDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceStopContDTMF, "pack_voice_SLQSVoiceStopContDTMF",
        &tpack_voice_SLQSVoiceStopContDTMF,
        (unpack_func_item) &unpack_voice_SLQSVoiceStopContDTMF, "unpack_voice_SLQSVoiceStopContDTMF",
        NULL, dump_SLQSVoiceStopContDTMF
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceSendFlash, "pack_voice_SLQSVoiceSendFlash",
        &tpack_voice_SLQSVoiceSendFlash,
        (unpack_func_item) &unpack_voice_SLQSVoiceSendFlash, "unpack_voice_SLQSVoiceSendFlash",
        NULL, dump_SLQSVoiceSendFlash
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetConfig, "pack_voice_SLQSVoiceGetConfig",
        &tpack_voice_SLQSVoiceGetConfig,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetConfig, "unpack_voice_SLQSVoiceGetConfig",
        NULL, dump_SLQSVoiceGetConfig
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCOLP, "pack_voice_SLQSVoiceGetCOLP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCOLP, "unpack_voice_SLQSVoiceGetCOLP",
        NULL, dump_SLQSVoiceGetCOLP
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCOLR, "pack_voice_SLQSVoiceGetCOLR",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCOLR, "unpack_voice_SLQSVoiceGetCOLR",
        NULL, dump_SLQSVoiceGetCOLR
    },
    {
        (pack_func_item) &pack_voice_SLQSVoiceGetCNAP, "pack_voice_SLQSVoiceGetCNAP",
        NULL,
        (unpack_func_item) &unpack_voice_SLQSVoiceGetCNAP, "unpack_voice_SLQSVoiceGetCNAP",
        NULL, dump_SLQSVoiceGetCNAP
    },
    {
        (pack_func_item) &pack_voice_SLQSOriginateUSSD, "pack_voice_SLQSOriginateUSSD",
        &tpack_voice_SLQSOriginateUSSD,
        (unpack_func_item) &unpack_voice_SLQSOriginateUSSD, "unpack_voice_SLQSOriginateUSSD",
        NULL, dump_SLQSOriginateUSSD
    },
};

void voice_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======VOICE pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(voicetotest)/sizeof(testitem_t); i++)
    {
        if(voicetotest[i].dump!=NULL)
        {
           voicetotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(voicetotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(voicetotest_invalidunpack[i].dump!=NULL)
        {
           voicetotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(voicetotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(voicetotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,voicetotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(voice<0)
            voice = client_fd(eVOICE);
        if(voice<0)
        {
            fprintf(stderr,"VOICE Service Not Supported!\n");
            return ;
        }
        rtn = write(voice, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(voice>=0)
                close(voice);
            voice=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eVOICE, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(voice, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eVOICE, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(voicetotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                voicetotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           voicetotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    voicetotest_invalidunpack[i].dump(voicetotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(voice>=0)
                close(voice);
            voice=-1;
        }
        sleep(1);
        xid++;
    }
    if(voice>=0)
        close(voice);
    voice=-1;
}



void voice_test_pack_unpack_loop()
{
    unsigned i;
    printf("======Voice dummy unpack test===========\n");
    iLocalLog = 0;
    voice_validate_dummy_unpack();
    iLocalLog = 1;

    printf("======VOICE pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE

    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "VOICE Pack/UnPack API Name", "Status", "Unpack Payload Parsing");
#endif

    unsigned xid =1;
    if(g_runoem_demo==0)
    {
        test_voice_dummy_ind();
    }

    for(i=0; i<sizeof(voicetotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(voicetotest[i].pack)(&req_ctx, req, &reqLen,voicetotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", voicetotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(voice<0)
            voice = client_fd(eVOICE);
        if(voice<0)
        {
            fprintf(stderr,"Voice Service Not Supported!\n");
            return ;
        }
        rtn = write(voice, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eVOICE, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(voice, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eVOICE, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("VOICE RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(voicetotest[i].unpack)(rsp, rspLen, voicetotest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,%s", voicetotest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "");
                    #endif

                        voicetotest[i].dump(voicetotest[i].unpack_ptr);
                        /* sleep 3 seconds for next write */
                        sleep(3);
                        /* skip the while loop so that the the next API can be invoked */
                        break;
                    }
                    else
                    {
                        /* the response is not belongs to this API, skip it and read again */
                        printf("xid %d\n",xid);
                        sleep(1);
                        break;
                    }
                }
                else if (rsp_ctx.type == eIND)
                {
                    printf("VOICE IND: msgid 0x%x\n", rsp_ctx.msgid);
                    break;
                }
            }
            else if (rtn< 0)
            {
                break;//Read Error
            }
            else
            {
                /* read nothing, sleep 1 second and read again */
                sleep(1);
            }
        }
        xid++;
    }
    if(voice>=0)
        close(voice);
    voice=-1;
}

void displayUSSDNotificationCallback(
    unpack_voice_USSDNotificationCallback_ind_t *pOutput)
{    
    uint8_t lLength = 0;

    if (pOutput == NULL)
        return;
    swi_uint256_print_mask (pOutput->ParamPresenceMask);
    if(swi_uint256_get_bit (pOutput->ParamPresenceMask, 1))
        printf("Voice Ussd Notification Type: %d :: \n",
                 (int)pOutput->notification_Type );
    if(swi_uint256_get_bit (pOutput->ParamPresenceMask, 16))
    {        
        printf("USS DCS: %d\n", pOutput->USSDNotificationNetworkInfo.networkInfo.ussDCS );
        printf("USS Len: %d\n", pOutput->USSDNotificationNetworkInfo.networkInfo.ussLen );

        printf("USS Data: ");
        while ( lLength < pOutput->USSDNotificationNetworkInfo.networkInfo.ussLen )
        {
            printf("0x%x", pOutput->USSDNotificationNetworkInfo.networkInfo.ussData[lLength]);
            lLength++;
        }
        printf("\n\n");
    }
}
void dispalyAllCallStatusCallback(
    unpack_voice_allCallStatusCallback_ind_t *pOutput )
{
    int lCount  = 0;

    if (pOutput == NULL)
        return;
    swi_uint256_print_mask (pOutput->ParamPresenceMask);
    if(swi_uint256_get_bit (pOutput->ParamPresenceMask, 1))
    {
        voice_arrCallInfo temp =
                   (voice_arrCallInfo )pOutput->arrCallInfomation;
        printf("\nCall Information:\n" );
        printf("Num of instances:%d\n",temp.numInstances );
        for(lCount = 0; lCount < temp.numInstances; lCount++)
        {
            printf("Call Identifier:%d\n",
                        temp.getAllCallInfo[lCount].Callinfo.callID);
            printf("Call State:%d\n",
                        temp.getAllCallInfo[lCount].Callinfo.callState);
            printf("Call Type:%d\n",
                        temp.getAllCallInfo[lCount].Callinfo.callType);
            printf("Direction:%d\n",
                        temp.getAllCallInfo[lCount].Callinfo.direction);
            printf("Mode:%d\n",
                        temp.getAllCallInfo[lCount].Callinfo.mode);
        }
    }
    if(( pOutput->pArrRemotePartyNum ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 16)))
    {
        
        int lCount = 0;
        voice_arrRemotePartyNum *pTemp =
                    (voice_arrRemotePartyNum * )pOutput->pArrRemotePartyNum;
        printf("\nRemote Party Number:\n" );
        printf("Num Of Instances :%d\n", pTemp->numInstances );

        for(lCount = 0; lCount < pTemp->numInstances; lCount++)
        {
             int lLcount = 0;
             printf("Call Identifier:%d\n",
                          pTemp->RmtPtyNum[lCount].callID);

             printf("Presentation Indicator:%d\n",
                         pTemp->RmtPtyNum[lCount].RemotePartyNum.presentationInd);

             printf("Number Length:%d\n",
                         pTemp->RmtPtyNum[lCount].RemotePartyNum.numLen);

             printf("Number:" );
             for( lLcount = 0; lLcount < pTemp->RmtPtyNum[lCount].
                                              RemotePartyNum.numLen; lLcount++ )
             {
                 printf("%c",pTemp->RmtPtyNum[lCount].
                                      RemotePartyNum.remPartyNumber[lLcount]);
             }
        }
         printf("\n");
    }

    if (( pOutput->pArrRemotePartyName ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 17)))
    {         
        int lCount = 0;
        voice_arrRemotePartyName *pTemp =
               (voice_arrRemotePartyName * )pOutput->pArrRemotePartyName;
        printf("\nRemote Party Name:\n" );
        printf("Num of instances :%d\n", pTemp->numInstances );

        for(lCount = 0; lCount < pTemp->numInstances; lCount++)
        {
            int lLcount = 0;
            printf("Call Identifier:%d\n",
                   pTemp->GetAllCallRmtPtyName[lCount].callID);

            printf("Presentation Indicator :%d\n",pTemp->GetAllCallRmtPtyName[lCount].
                                        RemotePartyName.namePI);

            printf("Coding Scheme:%d\n", pTemp->GetAllCallRmtPtyName[lCount].
                              RemotePartyName.codingScheme);

            printf("Number Length:%d\n", pTemp->GetAllCallRmtPtyName[lCount].
                              RemotePartyName.nameLen);

            printf("Caller Name:\n");
            for( lLcount = 0; lLcount <
                                      pTemp->GetAllCallRmtPtyName[lCount].
                                      RemotePartyName.nameLen; lLcount++ )
            {
                printf("%c", pTemp->GetAllCallRmtPtyName[lCount].
                                     RemotePartyName.callerName[lLcount]);
            }
       }
       printf("\n");
    }
    if (( pOutput->pArrSvcOption ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 19)))
    {
        int lCount = 0;
        voice_arrSvcOption *pTemp =
                   (voice_arrSvcOption* )pOutput->pArrSvcOption;
        printf("\nService Option:\n" );
        printf("Num of instances :%d\n", pTemp->numInstances );

        
        for(lCount = 0; lCount < pTemp->numInstances; lCount++)
        {
            printf("Call Identifier:%d\n",
                         pTemp->callID[lCount]);

            printf("Service Option:%d\n",
                        pTemp->srvOption[lCount]);
        }
    }
    if (( pOutput->pArrCallEndReason ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 20)))
    {
        printf("\nCall End Reason:\n" );
        int lCount = 0;
        voice_arrCallEndReason *pTemp =
                   (voice_arrCallEndReason* )pOutput->pArrCallEndReason;
        printf("Num of instances :%d\n", pTemp->numInstances );

        for(lCount = 0; lCount < pTemp->numInstances; lCount++)
        {
            printf("Call Identifier:%d\n",
                         pTemp->callID[lCount]);

            printf("Service Option:%d\n",
                        pTemp->callEndReason[lCount]);
        }
    }
    if ((pOutput->pArrConnectPartyNum ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 22)))
    {
        int lCount = 0;
        voice_arrConnectPartyNum *pTemp =
                   (voice_arrConnectPartyNum* )pOutput->pArrConnectPartyNum;
        printf("\nConnected Party Number:\n" );
        printf("Num of instances :%d\n", pTemp->numInstances );

        for(lCount = 0; lCount < pTemp->numInstances; lCount++)
        {
            int lLcount = 0;
            printf("Call Identifier:%d\n",
                         pTemp->ConnectedPartyNum[lCount].callID);

            printf("Presentation indicator:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numPI);

            printf("screening indicator:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numSI);

            printf("Connected number type:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numType);

            printf("Connected number plan:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numPlan);

            printf("Connected Length:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numLen);

            printf("Connected Number:");
            for(lLcount = 0; lLcount < pTemp->ConnectedPartyNum[lCount]
                                               .numLen; lLcount++)
            {
                printf("%d",
                           pTemp->ConnectedPartyNum[lCount].number[lLcount]);
            }
        }
        printf("\n");
    }
}

void dispalyVoiceSUPSCallback(
    unpack_voice_SLQSVoiceSUPSCallback_ind_t *pOutput )
{
    int lcount = 0;

    if (pOutput == NULL)
        return;
    swi_uint256_print_mask (pOutput->ParamPresenceMask);
    if(swi_uint256_get_bit (pOutput->ParamPresenceMask, 1))
    {
        voice_SUPSInfo *lTemp = &(pOutput->SUPSInformation);
        printf("Voice SUPS Info From indication\n");
        printf("Service Type   : %x \n", lTemp->svcType);
        printf("Modified by CC : %x \n", lTemp->isModByCC);
    }

    if ((pOutput->pSvcClass != NULL) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 16)))
        printf("Service Class  : %x\n", *(pOutput->pSvcClass));
    if ((pOutput->pReason != NULL) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 17)))
        printf("Reason  : %x\n",*(pOutput->pReason));
    if (( NULL != pOutput->pCallFWNum ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 18)))
    {
        printf("Call FW Number : ");
        for ( lcount = 0 ; pOutput->pCallFWNum[lcount] != '\0' ; lcount++ )
        {
            printf("%x ", pOutput->pCallFWNum[lcount]);
        }
        printf("\n");
    }
    if ((pOutput->pCallFWTimerVal != NULL) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 19)))
        printf("Call FW No Reply Timer: %d\n",*(pOutput->pCallFWTimerVal));
    if (( NULL != pOutput->pUSSInfo ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 20)))
    {
        printf("USS DCS        : %x \n", pOutput->pUSSInfo->ussDCS);
        printf("USS Length     : %d \n", pOutput->pUSSInfo->ussLen);
        printf("USS Data       : ");
        for ( lcount = 0 ; lcount < pOutput->pUSSInfo->ussLen ; lcount++ )
        {
            printf("%c", pOutput->pUSSInfo->ussData[lcount]);
        }
        printf( "\n");
    }
    if ((pOutput->pCallID != NULL) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 21)))
        printf("Call Id        : %x\n",*(pOutput->pCallID));
    if (( NULL != pOutput->pAlphaIDInfo ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 22)))
    {
        voice_alphaIDInfo *pAlphaIDInfo = pOutput->pAlphaIDInfo;
        printf("Alpha DCS       : %x \n", pAlphaIDInfo->alphaDcs);
        printf("Alpha Len       : %d \n", pAlphaIDInfo->alphaLen);
        printf("Alpha Text      : ");
        for (lcount = 0;lcount < pAlphaIDInfo->alphaLen;lcount++)
        {
            printf("%c", pAlphaIDInfo->alphaText[lcount]);
        }
        printf("\n");
    }
    if (( NULL != pOutput->pCallBarPasswd ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 23)))
    {
        printf("Call Bar Password : ");
        for ( lcount = 0 ; lcount < 4 ; lcount++ )
        {
            printf("%c", pOutput->pCallBarPasswd[lcount]);
        }
        printf("\n");
    }
    if ((pOutput->pDataSrc) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 25)))
        printf("Data Source      : %x\n",*(pOutput->pDataSrc));
    if ((pOutput->pFailCause) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 26)))
        printf("Failure Cause    : %x\n",*(pOutput->pFailCause));
    if (( NULL != pOutput->pCLIRstatus ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 28)))
    {
        voice_CLIRResp *pCLIRResp = pOutput->pCLIRstatus;
        printf("CLIR Status:\n");
        printf("Activation Status : %x \n", pCLIRResp->ActiveStatus);
        printf("Provisioned Status: %x \n", pCLIRResp->ProvisionStatus);
    }
    if (( NULL != pOutput->pCLIPstatus ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 29)))
    {
        voice_CLIPResp *pCLIPResp = pOutput->pCLIPstatus;
        printf("CLIP Status:\n");
        printf("Activation Status : %x \n", pCLIPResp->ActiveStatus);
        printf("Provisioned Status: %x \n", pCLIPResp->ProvisionStatus);
    }
    if (( NULL != pOutput->pCOLPstatus ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 30)))
    {
        voice_COLPResp *pCOLPResp = pOutput->pCOLPstatus;
        printf("COLP Status:\n");
        printf("Activation Status : %x \n", pCOLPResp->ActiveStatus);
        printf("Provisioned Status: %x \n", pCOLPResp->ProvisionStatus);
    }
    if (( NULL != pOutput->pCOLRstatus ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 31)))
    {
        voice_COLRResp *pCOLRResp = pOutput->pCOLRstatus;
        printf("COLR Status:\n");
        printf("Activation Status : %x \n", pCOLRResp->ActiveStatus);
        printf("Provisioned Status: %x \n", pCOLRResp->ProvisionStatus);
    }
    if (( NULL != pOutput->pCNAPstatus ) && (swi_uint256_get_bit (pOutput->ParamPresenceMask, 32)))
    {
        voice_CNAPResp *pCNAPResp = pOutput->pCNAPstatus;
        printf("CNAP Status:\n");
        printf("Activation Status : %x \n", pCNAPResp->ActiveStatus);
        printf("Provisioned Status: %x \n", pCNAPResp->ProvisionStatus);
    }
    printf("\n--------\n\n");
}


void *voice_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enVOICEThread)
    {
        rtn = read(voice, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eVOICE, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("VOICE IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("VOICE RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);

            switch(rsp_ctx.msgid)
            {
                case eQMI_VOICE_INDICATION_REGISTER:
                    printf ( "QMI_VOICE_INDICATION_REGISTER resp received\r\n" );
                    break;
                case eQMI_VOICE_DIAL_CALL:
                    printf ( "QMI_VOICE_DIAL_CALL resp received\r\n" );
                    break;
                case eQMI_VOICE_ORIGINATE_USSD:
                    printf ( "QMI_VOICE_ORIGINATE_USSDR resp received\r\n" );
                    break;
                case eQMI_VOICE_CANCEL_USSD:
                    printf ( "QMI_VOICE_CANCEL_USSD resp received\r\n" );
                    break;
                case eQMI_VOICE_USSD_RELEASE_IND:
                    printf ( "VOICE_USSD_RELEASE_INDICATION received\r\n" );
                    break;
                case eQMI_VOICE_USSD_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_voice_USSDNotificationCallback_ind_t  ussdNotificationInd;
                         rtn = unpack_voice_USSDNotificationCallback_ind(msg.buf, 
                                                         QMI_MSG_MAX, 
                                                         &ussdNotificationInd);
                         printf("unpack QMI_VOICE_USSD_IND result :%d\n",rtn);
                         if (rtn == eQCWWAN_ERR_NONE)
                             displayUSSDNotificationCallback(&ussdNotificationInd);
                     }
                     break;
                case eQMI_VOICE_SUPS_NOTIFICATION_IND:
                    if (rsp_ctx.type == eIND)
                     {
                         unpack_voice_SUPSNotificationCallback_ind_t  supsNotifInd;
                         rtn = unpack_voice_SUPSNotificationCallback_ind(msg.buf, 
                                                         QMI_MSG_MAX, 
                                                         &supsNotifInd);
                         printf("unpack QMI_VOICE_SUPS_NOTIFICATION_IND result :%d\n",rtn);
                         if (rtn == eQCWWAN_ERR_NONE) {
                             swi_uint256_print_mask (supsNotifInd.ParamPresenceMask);
                             if(swi_uint256_get_bit (supsNotifInd.ParamPresenceMask, 16))
                             {
                                 printf("Voice SUPS: Call ID: %x :: Notification Type : %x \n",
                                                  supsNotifInd.callID,
                                                  supsNotifInd.notifType );
                             }
                             if (( supsNotifInd.pCUGIndex) && (swi_uint256_get_bit (supsNotifInd.ParamPresenceMask, 16)))
                                 printf("CUG Index: %d\n", *(supsNotifInd.pCUGIndex) );
                         }
                     }
                     break;
                case eQMI_VOICE_ALL_CALL_STATUS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_voice_allCallStatusCallback_ind_t  allCallStatusInd;
                         rtn = unpack_voice_allCallStatusCallback_ind(msg.buf, 
                                                         QMI_MSG_MAX, 
                                                         &allCallStatusInd);
                         printf("unpack QMI_VOICE_ALL_CALL_STATUS_IND result :%d\n",rtn);
                         if (rtn == eQCWWAN_ERR_NONE)
                             dispalyAllCallStatusCallback(&allCallStatusInd);
                     }
                     break;
                case eQMI_VOICE_SUPS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_voice_SLQSVoiceSUPSCallback_ind_t  supsInd;
                         rtn = unpack_voice_SLQSVoiceSUPSCallback_ind(msg.buf, 
                                                         QMI_MSG_MAX, 
                                                         &supsInd);
                         printf("unpack QMI_VOICE_SUPS_IND result :%d\n",rtn);
                         if (rtn == eQCWWAN_ERR_NONE)
                             dispalyVoiceSUPSCallback(&supsInd);
                     }
                     break;

                // 3GPP2 indications
                case eQMI_VOICE_PRIVACY_IND:
                    printf ( "QMI_VOICE_PRIVACY_IND received\r\n" );
                    break;
                case eQMI_VOICE_DTMF_IND:
                    printf ( "QMI_VOICE_DTMF_IND received\r\n" );
                    break;
                case eQMI_VOICE_INFO_REC_IND:
                    printf ( "QMI_VOICE_DTMF_IND received\r\n" );
                    break;              
                case eQMI_VOICE_OTASP_STATUS_IND:
                    printf ( "QMI_VOICE_OTASP_IND received\r\n" );
                    break;              
                
                default:
                    printf("Default msgid 0x%x\n", rsp_ctx.msgid);
                    break;
            }
        }
        else
        {
           enVOICEThread = 0;
           voice = -1;
           break;
        }
    }
    return NULL;
}


void voice_test_ind()
{
    pack_qmi_t req_ctx;
    uint16_t qmi_req_len;
    uint8_t qmi_req[QMI_MSG_MAX];
    int rtn;
    const char *qmi_msg;

    /* Indication Register Parameters */
    uint8_t enable         = 0x01;
    pack_voice_SLQSVoiceIndicationRegister_t voiceIndRegister;

    if(voice<0)
        voice = client_fd(eVOICE);
    if(voice<0)
    {
        fprintf(stderr,"VOICE Service Not Supported\n");
        return ;
    }

    enVOICEThread = 1;
    pthread_create(&voice_tid, &voice_attr, voice_read_thread, NULL);
    sleep(1);

    /* set indication */
    memset(&voiceIndRegister,0,sizeof(pack_voice_SLQSVoiceIndicationRegister_t));
    req_ctx.xid = 0x100;

    voiceIndRegister.pRegDTMFEvents =  &enable;
    voiceIndRegister.pRegVoicePrivacyEvents = &enable;
    voiceIndRegister.pSuppsNotifEvents = &enable;
    rtn = pack_voice_SLQSVoiceIndicationRegister(&req_ctx, 
                                                qmi_req, &qmi_req_len,
                                                (void*)&voiceIndRegister);
    if(rtn!=0)
    {
        printf("Set voice indication register fail");
        return ;
    }
    rtn = write(voice, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("VOICE FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eVOICE, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* originate ussd */
    req_ctx.xid = 0x101;
    rtn = pack_voice_OriginateUSSD(&req_ctx, 
                                   qmi_req, &qmi_req_len,
                                   (void*)&tpack_voice_OriginateUSSD);
    if(rtn!=0)
    {
        printf("originate ussd fail");
        return ;
    }
    rtn = write(voice, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("VOICE FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eVOICE, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    /* dial call */
    req_ctx.xid = 0x102;
    rtn = pack_voice_SLQSVoiceDialCall(&req_ctx, 
                                                qmi_req, &qmi_req_len,
                                                (void*)&tpack_voice_SLQSVoiceDialCall);
    if(rtn!=0)
    {
        printf("Set voice dial call fail");
        return ;
    }
    rtn = write(voice, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("VOICE FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eVOICE, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* cancel ussd */
    req_ctx.xid = 0x103;
    rtn = pack_voice_CancelUSSD(&req_ctx, 
                                   qmi_req, &qmi_req_len);
    if(rtn!=0)
    {
        printf("cancel ussd fail");
        return ;
    }
    rtn = write(voice, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("VOICE FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eVOICE, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(2);
}

void voice_test_ind_exit()
{
    void *pthread_rtn_value;
    printf("\nkilling voice read thread...\n");
    enVOICEThread = 0;
    UNUSEDPARAM(pthread_rtn_value);
#ifdef __ARM_EABI__    
    if(voice>=0)
        close(voice);
    voice = -1;
    if(voice_tid!=0)
        pthread_join(voice_tid, &pthread_rtn_value);
    voice_tid = 0;
#endif
    if(voice_tid!=0)
        pthread_cancel(voice_tid);
    voice_tid = 0;
    if(voice>=0)
        close(voice);
    voice=-1;
}

void dump_voiceInfoRecCallback_ind(unpack_voice_VoiceInfoRecCallback_ind_t* pResp)
{
#define PRINT_HEX_POINTER_STRUCT_VALUE(pStruct,member)\
        printf("\t%s: 0x%x\n",#member,pStruct->member);
#define PRINT_HEX_POINTER_STRUCT_PTR_VALUE(pStruct,member)\
        printf("\t%s: 0x%x\n",#member,*(pStruct->member));
#define PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pStruct,member)\
            printf("\t%s: %s\n",#member,pStruct->member);
#define PRINT_VOICE_CONNECTNUMINFO_STRUCT_VALUE(pStruct)\
        printf("%s\n",strstr(#pStruct,"->p") + 3);\
        PRINT_HEX_POINTER_STRUCT_VALUE(pStruct,numPresInd);\
        PRINT_HEX_POINTER_STRUCT_VALUE(pStruct,numType);\
        PRINT_HEX_POINTER_STRUCT_VALUE(pStruct,numPlan);\
        PRINT_HEX_POINTER_STRUCT_VALUE(pStruct,callerIDLen);\
        PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pStruct,callerID);

    printf("%s \n",__FUNCTION__);
    if(pResp)
    {
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 1))
        {
            printf("callID\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp,callID);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 16) && 
            pResp->pSignalInfo)
        {
            printf("SignalInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pSignalInfo,alertPitch);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pSignalInfo,signalType);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pSignalInfo,signal);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 17) && 
            pResp->pCallerIDInfo)
        {
            printf("CallerIDInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCallerIDInfo,PI);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCallerIDInfo,callerIDLen);
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp->pCallerIDInfo,callerID);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 18) && 
            pResp->pDispInfo)
        {
            printf("DispInfo\n");
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp,pDispInfo);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 19) && 
            pResp->pExtDispInfo)
        {
            printf("ExtDispInfo\n");
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp,pExtDispInfo);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 20) && 
            pResp->pCallerNameInfo)
        {
            printf("CallerNameInfo\n");
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp,pCallerNameInfo);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 21) && 
            pResp->pCallWaitInd)
        {
            printf("CallWaitInd\n");
            PRINT_HEX_POINTER_STRUCT_PTR_VALUE(pResp,pCallWaitInd);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 22) && 
            pResp->pConnectNumInfo)
        {
            PRINT_VOICE_CONNECTNUMINFO_STRUCT_VALUE(pResp->pConnectNumInfo);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 23) &&
            pResp->pCallingPartyInfo)
        {
            PRINT_VOICE_CONNECTNUMINFO_STRUCT_VALUE(pResp->pCallingPartyInfo);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 24) &&
            pResp->pCalledPartyInfo)
        {
            printf("CalledPartyInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCalledPartyInfo,PI);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCalledPartyInfo,numType);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCalledPartyInfo,numPlan);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pCalledPartyInfo,numLen);
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp->pCalledPartyInfo,number);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 25) &&
            pResp->pRedirNumInfo)
        {
            printf("RedirNumInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,PI);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,SI);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,numType);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,numPlan);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,numLen);
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp->pRedirNumInfo,number);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 26) &&
            pResp->pCLIRCause)
        {
            printf("CLIRCause\n");
            PRINT_HEX_POINTER_STRUCT_PTR_VALUE(pResp,pCLIRCause);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 27) &&
            pResp->pNSSAudioCtrl)
        {
            printf("NSSAudioCtrl\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pNSSAudioCtrl,upLink);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pNSSAudioCtrl,downLink);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 28) &&
            pResp->pNSSRelease)
        {
            printf("NSSRelease\n");
            PRINT_HEX_POINTER_STRUCT_PTR_VALUE(pResp,pNSSRelease);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 29) &&
            pResp->pLineCtrlInfo)
        {
            printf("LineCtrlInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pLineCtrlInfo,polarityIncluded);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pLineCtrlInfo,toggleMode);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pLineCtrlInfo,revPolarity);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pLineCtrlInfo,pwrDenialTime);
        }
        if(swi_uint256_get_bit (pResp->ParamPresenceMask, 30) &&
            pResp->pExtDispRecInfo)
        {
            printf("ExtDispRecInfo\n");
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pExtDispRecInfo,dispType);
            PRINT_HEX_POINTER_STRUCT_VALUE(pResp->pExtDispRecInfo,extDispInfoLen);
            PRINT_BYTE_STRING_POINTER_STRUCT_VALUE(pResp->pExtDispRecInfo,extDispInfo);
        }
        
    }
}

void test_voice_dummy_ind()
{
    unpack_voice_VoiceInfoRecCallback_ind_t lunpack_voice_VoiceInfoRecCallback_ind_t;
    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_voice_VoiceInfoRecCallback_ind,
        unpack_voice_VoiceInfoRecCallback_ind_t,
        dump_voiceInfoRecCallback_ind,
        &lunpack_voice_VoiceInfoRecCallback_ind_t,
        {
            DUMMY_UNPACK_VOICE_VOICEINFORECCALLBACK_IND_MSG
        }
    );    
}

void dump_voicePrivacyChangeCallback_ind(void *ptr)
{
    unpack_voice_voicePrivacyChangeCallback_ind_t *result =
            (unpack_voice_voicePrivacyChangeCallback_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Privacy Change: Call ID: %x :: Voice Privacy : %x \n",
                result->callID, result->voicePrivacy );

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SUPSNotificationCallback_ind(void *ptr)
{
    unpack_voice_SUPSNotificationCallback_ind_t *result =
            (unpack_voice_SUPSNotificationCallback_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("Voice SUPS: Call ID: %x :: Notification Type : %x \n",
                         result->callID,
                         result->notifType );
    }
    if (( result->pCUGIndex) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("CUG Index: %d\n", *(result->pCUGIndex) );

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_DTMFEventCallback_ind(void *ptr)
{
    unpack_voice_DTMFEventCallback_ind_t *result =
            (unpack_voice_DTMFEventCallback_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        uint16_t lcount;
        voice_DTMFInfo *lTemp = &(result->DTMFInformation);
        printf( "Voice DTMF EVent Info ");
        printf( "Call ID      : %x \n", lTemp->callID);
        printf( "DTMF Event   : %x \n", lTemp->DTMFEvent);
        printf( "No.of Digits : %d \n", lTemp->digitCnt);
        printf( "Digit Values : ");
        for ( lcount = 0 ; lcount < lTemp->digitCnt ; lcount++ )
        {
            printf( "%c", lTemp->digitBuff[lcount]);
           }
        printf( "\n");
    }
    if((result->pOnLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf( "On Length   : %d\n",*result->pOnLength);
    if((result->pOffLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf( "Off Length  : %d\n",*result->pOffLength);
    printf( "\n\n");

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_OTASPStatusCallback_ind(void *ptr)
{
    unpack_voice_OTASPStatusCallback_ind_t *result =
            (unpack_voice_OTASPStatusCallback_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf( "\nOTASP Status Information :\n" );
        {
            printf( "Call Identifier:%d\n", result->callID);
            printf( "OTASP Status   :%d\n", result->OTASPStatus);
        }
    }

    if(iLocalLog == 0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_voice_resp_msg[][QMI_MSG_MAX] ={

        /* eQMI_VOICE_ORIGINATE_USSD */
        {0x02,0x01,0x00,0x3A,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_ANSWER_USSD */
        {0x02,0x02,0x00,0x3B,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_CANCEL_USSD */
        {0x02,0x03,0x00,0x3C,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_DIAL_CALL */
        {0x02,0x04,0x00,0x20,0x00,0x1B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
                0x11,0x04,0x00,0x01,0x02,'S','W',
                0x12,0x01,0x00,0x01,
                0x13,0x02,0x00,0x01,0x00},

        /* eQMI_VOICE_END_CALL */
        {0x02,0x05,0x00,0x21,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01},

        /* eQMI_VOICE_SET_SUPS_SERVICE */
        {0x02,0x06,0x00,0x33,0x00,0x20,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x00,0x00,
                0x11,0x04,0x00,0x01,0x02,'S','W',
                0x12,0x01,0x00,0x00,
                0x13,0x01,0x00,0x00,
                0x14,0x02,0x00,0x01,0x00},

        /* eQMI_VOICE_SET_CONFIG */
        {0x02,0x07,0x00,0x40,0x00,0x1F,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x00,
                0x11,0x01,0x00,0x00,
                0x12,0x01,0x00,0x01,
                0x13,0x01,0x00,0x01,
                0x14,0x01,0x00,0x00,
                0x15,0x01,0x00,0x00,
        },

        /* eQMI_VOICE_ANSWER_CALL */
        {0x02,0x08,0x00,0x22,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
        },

        /* eQMI_VOICE_GET_CLIR */
        {0x02,0x09,0x00,0x37,0x00,0x25,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x01,0x03,
                0x11,0x02,0x00,0x21,0x00,
                0x12,0x04,0x00,0x01,0x02,'S','W',
                0x13,0x01,0x00,0x01,
                0x14,0x01,0x00,0x02,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CLIP */
        {0x02,0x0A,0x00,0x36,0x00,0x25,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x01,0x01,
                0x11,0x02,0x00,0x21,0x00,
                0x12,0x04,0x00,0x01,0x02,'S','W',
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x03,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CALL_WAITING */
        {0x02,0x0B,0x00,0x34,0x00,0x24,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
                0x11,0x02,0x00,0x21,0x00,
                0x12,0x04,0x00,0x01,0x02,'S','W',
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x04,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CALL_BARRING */
        {0x02,0x0C,0x00,0x35,0x00,0x24,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
                0x11,0x02,0x00,0x21,0x00,
                0x12,0x04,0x00,0x01,0x02,'S','W',
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x05,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CALL_FORWARDING */
        {0x02,0x0D,0x00,0x38,0x00,0x48,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x0F,0x00,0x01,0x01,0x0D,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,0x00,
                0x11,0x02,0x00,0x21,0x00,
                0x12,0x04,0x00,0x01,0x02,'S','W',
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x06,
                0x15,0x02,0x00,0x01,0x00,
                0x16,0x13,0x00,0x01,0x01,0x0D,0x00,0x01,0x03,0x02,0x03,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31,
        },

        /* eQMI_VOICE_SET_CALL_BARRING_PASSWORD */
        {0x02,0x0E,0x00,0x39,0x00,0x20,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x21,0x00,
                0x11,0x04,0x00,0x01,0x02,'S','W',
                0x12,0x01,0x00,0x00,
                0x13,0x01,0x00,0x07,
                0x14,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CALL_INFO */
        {0x02,0x0F,0x00,0x24,0x00,0x69,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x05,0x00,0x01,0x01,0x00,0x01,0x02,
                0x11,0x0C,0x00,0x00,0x0A,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x12,0x02,0x00,0x08,0x00,
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x00,
                0x15,0x06,0x00,0x00,0x01,0x03,0x47,0x75,0x72,
                0x16,0x06,0x00,0x00,0x01,0x03,0x31,0x32,0x33,
                0x17,0x01,0x00,0x00,
                0x18,0x05,0x00,0x01,0x03,0x47,0x75,0x72,
                0x19,0x0F,0x00,0x00,0x01,0x02,0x03,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31,
                0x1A,0x04,0x00,0x03,0x31,0x31,0x31,
                0x1B,0x04,0x00,0x22,0x11,0x22,0x11,
        },

        /* eQMI_VOICE_GET_ALL_CALL_INFO */
        {0x02,0x10,0x00,0x2F,0x00,0xB3,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x08,0x00,0x01,0x02,0x01,0x00,0x01,0x02,0x00,0x00,
                0x11,0x0E,0x00,0x01,0x02,0x00,0x0A,0x38,0x34,0x32,0x37,
                    0x32,0x30,0x35,0x38,0x37,0x30,
                0x12,0x08,0x00,0x01,0x02,0x00,0x01,0x03,0x31,0x32,0x33,
                0x13,0x03,0x00,0x01,0x02,0x00,
                0x14,0x08,0x00,0x01,0x02,0x00,0x01,0x03,0x31,0x32,0x33,
                0x15,0x04,0x00,0x01,0x02,0x08,0x00,
                0x16,0x01,0x00,0x00,
                0x17,0x01,0x00,0x00,
                0x18,0x04,0x00,0x01,0x02,0x19,0x00,
                0x19,0x0E,0x00,0x01,0x02,0x00,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31,
                0x1A,0x11,0x00,0x01,0x02,0x01,0x01,0x02,0x03,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x1B,0x05,0x00,0x01,0x02,0x02,'S','W',
                0x1C,0x11,0x00,0x01,0x02,0x00,0x01,0x02,0x03,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x1D,0x11,0x00,0x01,0x02,0x00,0x01,0x02,0x03,0x0A,
                    0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x1E,0x06,0x00,0x01,0x02,0x11,0x11,0x22,0x22,
        },

        /* eQMI_VOICE_MANAGE_CALLS */
        {0x02,0x11,0x00,0x31,0x00,0x0C,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x15,0x00,
        },

        /* eQMI_VOICE_BURST_DTMF */
        {0x02,0x12,0x00,0x28,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
        },

        /* eQMI_VOICE_START_CONT_DTMF */
        {0x02,0x13,0x00,0x29,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x02,
        },

        /* eQMI_VOICE_STOP_CONT_DTMF */
        {0x02,0x14,0x00,0x2A,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x02,
        },

        /* eQMI_VOICE_SEND_FLASH */
        {0x02,0x15,0x00,0x27,0x00,0x0B,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x03,
        },

        /* eQMI_VOICE_SET_PREFERRED_PRIVACY */
        {0x02,0x16,0x00,0x2C,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_INDICATION_REGISTER */
        {0x02,0x17,0x00,0x03,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_GET_CONFIG */
        {0x02,0x18,0x00,0x41,0x00,0x37,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x00,0x01,
                0x11,0x05,0x00,0x01,0x04,0x03,0x02,0x01,
                0x12,0x05,0x00,0x00,0x02,0x02,0x01,0x01,
                0x13,0x01,0x00,0x00,
                0x14,0x08,0x00,0x00,0x01,0x03,0x00,0x01,0x80,0x46,0x00,
                0x15,0x02,0x00,0x01,0x04,
                0x16,0x01,0x00,0x00,
                0x17,0x01,0x00,0x00,
        },

        /* eQMI_VOICE_ORIG_USSD_NO_WAIT */
        {0x02,0x19,0x00,0x43,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_BIND_SUBSCRIPTION */
        {0x02,0x1A,0x00,0x44,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_BIND_SUBSCRIPTION */
        {0x02,0x1B,0x00,0x45,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_BIND_SUBSCRIPTION */
        {0x02,0x1C,0x00,0x46,0x00,0x07,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_VOICE_GET_COLP */
        {0x02,0x1D,0x00,0x4B,0x00,0x26,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x01,0x01,
                0x11,0x02,0x00,0x15,0x00,
                0x12,0x05,0x00,0x01,0x03,0x31,0x32,0x33,
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x05,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_COLR */
        {0x02,0x1E,0x00,0x4C,0x00,0x26,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x01,0x01,
                0x11,0x02,0x00,0x15,0x00,
                0x12,0x05,0x00,0x01,0x03,0x31,0x32,0x33,
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x06,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_GET_CNAP */
        {0x02,0x1F,0x00,0x4D,0x00,0x26,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x01,0x01,
                0x11,0x02,0x00,0x15,0x00,
                0x12,0x05,0x00,0x01,0x03,0x31,0x32,0x33,
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x07,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_ORIGINATE_USSD */
        {0x02,0x20,0x00,0x3A,0x00,0x29,0x00,
                0x02,0x04,0x00,0x00,0x00,0x00,0x00,
                0x10,0x02,0x00,0x15,0x00,
                0x11,0x05,0x00,0x01,0x03,0x31,0x32,0x33,
                0x12,0x05,0x00,0x01,0x03,0x31,0x33,0x35,
                0x13,0x01,0x00,0x00,
                0x14,0x01,0x00,0x08,
                0x15,0x02,0x00,0x01,0x00,
        },

        /* eQMI_VOICE_USSD_IND */
        {0x04,0x21,0x00,0x3E,0x00,0x0C,0x00,
                0x01,0x01,0x00,0x02,
                0x10,0x05,0x00,0x02,0x03,0x31,0x33,0x35,
        },

        /* eQMI_VOICE_SUPS_IND */
        {0x04,0x22,0x00,0x42,0x00,0x0D,0x00,
                0x01,0x02,0x00,0x01,0x01,
                0x10,0x01,0x00,0x02,
                0x11,0x01,0x00,0x05,
        },

        /* eQMI_VOICE_PRIVACY_IND */
        {0x04,0x23,0x00,0x2D,0x00,0x05,0x00,
                0x01,0x02,0x00,0x05,0x00,
        },

        /* eQMI_VOICE_SUPS_NOTIFICATION_IND */
        {0x04,0x24,0x00,0x32,0x00,0x1A,0x00,
                0x01,0x02,0x00,0x06,0x02,
                0x10,0x02,0x00,0x22,0x11,
                0x11,0x0D,0x00,0x00,0x02,0x0A,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
        },

        /* eQMI_VOICE_DTMF_IND */
        {0x04,0x25,0x00,0x2B,0x00,0x12,0x00,
                0x01,0x07,0x00,0x07,0x01,0x04,0x38,0x34,0x32,0x37,
                0x10,0x01,0x00,0x05,
                0x11,0x01,0x00,0x02,
        },

        /* eQMI_VOICE_OTASP_STATUS_IND */
        {0x04,0x26,0x00,0x25,0x00,0x05,0x00,
                0x01,0x02,0x00,0x09,0x02,
        },

        /* eQMI_VOICE_INFO_REC_IND */
        {0x04,0x27,0x00,0x26,0x00,0x30,0x00,//0x98
                0x01,0x01,0x00,0x04,
                0x10,0x03,0x00,0x01,0x02,0x03,
                0x11,0x0C,0x00,0x01,0x0A,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x12,0x0A,0x00,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x13,0x02,0x00,0x30,0x31,
                0x14,0x02,0x00,'S','W',
       },

        /* eQMI_VOICE_ALL_CALL_STATUS_IND */
        {0x04,0x28,0x00,0x2E,0x00,0x57,0x00,
                0x01,0x08,0x00,0x01,0x01,0x02,0x00,0x02,0x02,0x01,0x00,
                0x10,0x0E,0x00,0x01,0x01,0x00,0x0A,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
                0x11,0x07,0x00,0x01,0x01,0x00,0x00,0x02,'S','W',
                0x12,0x03,0x00,0x01,0x01,0x00,
                0x13,0x04,0x00,0x01,0x01,0x02,0x01,
                0x14,0x04,0x00,0x01,0x01,0x15,0x00,
                0x15,0x06,0x00,0x01,0x01,0x01,0x02,0x11,0x22,
                0x16,0x11,0x00,0x01,0x01,0x01,0x01,0x02,0x03,0x0A,0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,
        },

};

       /* eQMI_VOICE_ORIGINATE_USSD */
const unpack_voice_OriginateUSSD_t const_unpack_voice_OriginateUSSD_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

        /* eQMI_VOICE_ANSWER_USSD */
const unpack_voice_AnswerUSSD_t const_unpack_voice_AnswerUSSD_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

        /* eQMI_VOICE_CANCEL_USSD */
const unpack_voice_CancelUSSD_t const_unpack_voice_CancelUSSD_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint8_t           cst_CallID = 0x01;
voice_alphaIDInfo cst_AlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_CCResultType = 0x01;
voice_ccSUPSType  cst_CCSUPSType = {0x01,0x00};

    /* eQMI_VOICE_DIAL_CALL */
const unpack_voice_SLQSVoiceDialCall_t const_unpack_voice_SLQSVoiceDialCall_t = {
        &cst_CallID,&cst_AlphaIDInfo,&cst_CCResultType,&cst_CCSUPSType,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

uint8_t           var_CallID;
voice_alphaIDInfo var_AlphaIDInfo;
uint8_t           var_CCResultType;
voice_ccSUPSType  var_CCSUPSType;

    /* eQMI_VOICE_DIAL_CALL */
unpack_voice_SLQSVoiceDialCall_t var_unpack_voice_SLQSVoiceDialCall_t = {
        &var_CallID,&var_AlphaIDInfo,&var_CCResultType,&var_CCSUPSType,{{0}} };

uint8_t     cst_endCallID = 0x01;

    /* eQMI_VOICE_END_CALL */
const unpack_voice_SLQSVoiceEndCall_t const_unpack_voice_SLQSVoiceEndCall_t = {
        &cst_endCallID,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t     var_endCallID;

    /* eQMI_VOICE_END_CALL */
unpack_voice_SLQSVoiceEndCall_t var_unpack_voice_SLQSVoiceEndCall_t = {
        &var_endCallID,{{0}} };

uint16_t          cst_FailCause = 0x00;
voice_alphaIDInfo cst_SupsAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_SupsCCResultType = 0x00;
uint8_t           cst_SupsCallID = 0x00;
voice_ccSUPSType  cst_SupsCCSUPSType = {0x01,0x00};

    /* eQMI_VOICE_SET_SUPS_SERVICE */
const unpack_voice_SLQSVoiceSetSUPSService_t const_unpack_voice_SLQSVoiceSetSUPSService_t = {
        &cst_FailCause,&cst_SupsAlphaIDInfo,&cst_SupsCCResultType,&cst_SupsCallID,&cst_SupsCCSUPSType,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

uint16_t          var_FailCause;
voice_alphaIDInfo var_SupsAlphaIDInfo;
uint8_t           var_SupsCCResultType;
uint8_t           var_SupsCallID;
voice_ccSUPSType  var_SupsCCSUPSType;
    /* eQMI_VOICE_SET_SUPS_SERVICE */
unpack_voice_SLQSVoiceSetSUPSService_t var_unpack_voice_SLQSVoiceSetSUPSService_t = {
        &var_FailCause, &var_SupsAlphaIDInfo, &var_SupsCCResultType, &var_SupsCallID,
        &var_SupsCCSUPSType,{{0}} };

uint8_t cst_AutoAnsStatus = 0x00;
uint8_t cst_AirTimerStatus = 0x00;
uint8_t cst_RoamTimerStatus = 0x01;
uint8_t cst_TTYConfigStatus = 0x01;
uint8_t cst_PrefVoiceSOStatus = 0x00;
uint8_t cst_VoiceDomainPrefStatus = 0x00;

    /* eQMI_VOICE_SET_CONFIG */
const unpack_voice_SLQSVoiceSetConfig_t const_unpack_voice_SLQSVoiceSetConfig_t = {
        &cst_AutoAnsStatus,&cst_AirTimerStatus,&cst_RoamTimerStatus,&cst_TTYConfigStatus,
        &cst_PrefVoiceSOStatus,&cst_VoiceDomainPrefStatus,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

uint8_t var_AutoAnsStatus;
uint8_t var_AirTimerStatus;
uint8_t var_RoamTimerStatus;
uint8_t var_TTYConfigStatus;
uint8_t var_PrefVoiceSOStatus;
uint8_t var_VoiceDomainPrefStatus;

    /* eQMI_VOICE_SET_CONFIG */
unpack_voice_SLQSVoiceSetConfig_t var_unpack_voice_SLQSVoiceSetConfig_t = {
        &var_AutoAnsStatus, &var_AirTimerStatus, &var_RoamTimerStatus, &var_TTYConfigStatus,
        &var_PrefVoiceSOStatus,&var_VoiceDomainPrefStatus,{{0}} };

uint8_t     cst_AnsCallID = 0x01;
    /* eQMI_VOICE_ANSWER_CALL */
const unpack_voice_SLQSVoiceAnswerCall_t const_unpack_voice_SLQSVoiceAnswerCall_t = {
        &cst_AnsCallID,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t     var_AnsCallID;
    /* eQMI_VOICE_ANSWER_CALL */
unpack_voice_SLQSVoiceAnswerCall_t var_unpack_voice_SLQSVoiceAnswerCall_t = {
        &var_AnsCallID,{{0}} };

voice_CLIRResp    cst_CLIRResp = {0x01,0x03};
uint16_t          cst_ClirFailCause = 0x0021;
voice_alphaIDInfo cst_ClirAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_ClirCCResType = 0x01;
uint8_t           cst_ClirCallID = 0x02;
voice_ccSUPSType  cst_ClirCCSUPSType = { 0x01,0x00};

    /* eQMI_VOICE_GET_CLIR */
const unpack_voice_SLQSVoiceGetCLIR_t const_unpack_voice_SLQSVoiceGetCLIR_t = {
        &cst_CLIRResp,&cst_ClirFailCause,&cst_ClirAlphaIDInfo,&cst_ClirCCResType,
        &cst_ClirCallID,&cst_ClirCCSUPSType,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

voice_CLIRResp    var_CLIRResp;
uint16_t          var_ClirFailCause;
voice_alphaIDInfo var_ClirAlphaIDInfo;
uint8_t           var_ClirCCResType;
uint8_t           var_ClirCallID;
voice_ccSUPSType  var_ClirCCSUPSType;
    /* eQMI_VOICE_GET_CLIR */
unpack_voice_SLQSVoiceGetCLIR_t var_unpack_voice_SLQSVoiceGetCLIR_t = {
        &var_CLIRResp, &var_ClirFailCause, &var_ClirAlphaIDInfo, &var_ClirCCResType,
        &var_ClirCallID,&var_ClirCCSUPSType,{{0}} };

voice_CLIPResp    cst_CLIPResp = {0x01,0x01};
uint16_t          cst_ClipFailCause = 0x0021;
voice_alphaIDInfo cst_ClipAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_ClipCCResType = 0x00;
uint8_t           cst_ClipCallID = 0x03;
voice_ccSUPSType  cst_ClipCCSUPSType = { 0x01,0x00};

    /* eQMI_VOICE_GET_CLIP */
const unpack_voice_SLQSVoiceGetCLIP_t const_unpack_voice_SLQSVoiceGetCLIP_t = {
        &cst_CLIPResp,&cst_ClipFailCause,&cst_ClipAlphaIDInfo,&cst_ClipCCResType,
        &cst_ClipCallID,&cst_ClipCCSUPSType,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

voice_CLIPResp    var_CLIPResp;
uint16_t          var_ClipFailCause;
voice_alphaIDInfo var_ClipAlphaIDInfo;
uint8_t           var_ClipCCResType;
uint8_t           var_ClipCallID;
voice_ccSUPSType  var_ClipCCSUPSType;
    /* eQMI_VOICE_GET_CLIP */
unpack_voice_SLQSVoiceGetCLIP_t var_unpack_voice_SLQSVoiceGetCLIP_t = {
        &var_CLIPResp, &var_ClipFailCause, &var_ClipAlphaIDInfo, &var_ClipCCResType,
        &var_ClipCallID,&var_ClipCCSUPSType,{{0}} };

uint8_t           cst_SvcClass = 0x01;
uint16_t          cst_cWaitFailCause = 0x0021;
voice_alphaIDInfo cst_cWaitAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_cWaitCCResType = 0x00;
uint8_t           cst_cWaitCallID = 0x04;
voice_ccSUPSType  cst_cWaitCCSUPSType = { 0x01,0x00};

    /* eQMI_VOICE_GET_CALL_WAITING */
const unpack_voice_SLQSVoiceGetCallWaiting_t const_unpack_voice_SLQSVoiceGetCallWaiting_t = {
        &cst_SvcClass,&cst_cWaitFailCause,&cst_cWaitAlphaIDInfo,&cst_cWaitCCResType,
        &cst_cWaitCallID,&cst_cWaitCCSUPSType,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

uint8_t           var_SvcClass;
uint16_t          var_cWaitFailCause;
voice_alphaIDInfo var_cWaitAlphaIDInfo;
uint8_t           var_cWaitCCResType;
uint8_t           var_cWaitCallID;
voice_ccSUPSType  var_cWaitCCSUPSType;
    /* eQMI_VOICE_GET_CALL_WAITING */
unpack_voice_SLQSVoiceGetCallWaiting_t var_unpack_voice_SLQSVoiceGetCallWaiting_t = {
        &var_SvcClass, &var_cWaitFailCause, &var_cWaitAlphaIDInfo, &var_cWaitCCResType,
        &var_cWaitCallID,&var_cWaitCCSUPSType,{{0}} };

uint8_t           cst_cBarringSvcClass = 0x01;
uint16_t          cst_cBarringFailCause = 0x0021;
voice_alphaIDInfo cst_cBarringAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_cBarringCCResType = 0x00;
uint8_t           cst_cBarringCallID = 0x05;
voice_ccSUPSType  cst_cBarringCCSUPSType = { 0x01,0x00};

    /* eQMI_VOICE_GET_CALL_BARRING */
const unpack_voice_SLQSVoiceGetCallBarring_t const_unpack_voice_SLQSVoiceGetCallBarring_t = {
        &cst_cBarringSvcClass,&cst_cBarringFailCause,&cst_cBarringAlphaIDInfo,&cst_cBarringCCResType,
        &cst_cBarringCallID,&cst_cBarringCCSUPSType,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

uint8_t           var_cBarringSvcClass;
uint16_t          var_cBarringFailCause;
voice_alphaIDInfo var_cBarringAlphaIDInfo;
uint8_t           var_cBarringCCResType;
uint8_t           var_cBarringCallID;
voice_ccSUPSType  var_cBarringCCSUPSType;
    /* eQMI_VOICE_GET_CALL_BARRING */
unpack_voice_SLQSVoiceGetCallBarring_t var_unpack_voice_SLQSVoiceGetCallBarring_t = {
        &var_cBarringSvcClass, &var_cBarringFailCause, &var_cBarringAlphaIDInfo, &var_cBarringCCResType,
        &var_cBarringCallID,&var_cBarringCCSUPSType,{{0}} };

voice_getCallFWInfo    cst_GetCallFWInfo = {0x01,{{0x01,0x0D,0x0A,
                                            {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30},0x00}}};
uint16_t               cst_CallFWFailCause = 0x0021;
voice_alphaIDInfo      cst_CallFWAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t                cst_CallFWCCResType  = 0x00;
uint8_t                cst_CallFWCallID = 0x06;
voice_ccSUPSType       cst_CallFWCCSUPSType = { 0x01,0x00};
voice_getCallFWExtInfo cst_CallFWGetCallFWExtInfo = { 0x01,{{0x01,0x0D,0x00,0x01,0x03,0x02,0x03,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31}}}};

    /* eQMI_VOICE_GET_CALL_FORWARDING */
const unpack_voice_SLQSVoiceGetCallForwardingStatus_t const_unpack_voice_SLQSVoiceGetCallForwardingStatus_t = {
        &cst_GetCallFWInfo,&cst_CallFWFailCause,&cst_CallFWAlphaIDInfo,&cst_CallFWCCResType,
        &cst_CallFWCallID,&cst_CallFWCCSUPSType,&cst_CallFWGetCallFWExtInfo,
        {{SWI_UINT256_BIT_VALUE(SET_8_BITS,2,16,17,18,19,20,21,22)}} };

voice_getCallFWInfo    var_GetCallFWInfo;
uint16_t               var_CallFWFailCause;
voice_alphaIDInfo      var_CallFWAlphaIDInfo;
uint8_t                var_CallFWCCResType;
uint8_t                var_CallFWCallID;
voice_ccSUPSType       var_CallFWCCSUPSType;
voice_getCallFWExtInfo var_CallFWGetCallFWExtInfo;
    /* eQMI_VOICE_GET_CALL_FORWARDING */
unpack_voice_SLQSVoiceGetCallForwardingStatus_t var_unpack_voice_SLQSVoiceGetCallForwardingStatus_t = {
        &var_GetCallFWInfo, &var_CallFWFailCause, &var_CallFWAlphaIDInfo, &var_CallFWCCResType,
        &var_CallFWCallID,&var_CallFWCCSUPSType,&var_CallFWGetCallFWExtInfo,{{0}} };

uint16_t          cst_cBarPassFailCause = 0x0021;
voice_alphaIDInfo cst_cBarPassAlphaIDInfo = {0x01,0x02,{'S','W'}};
uint8_t           cst_cBarPassCCResType = 0x00;
uint8_t           cst_cBarPassCallID = 0x07;
voice_ccSUPSType  cst_cBarPassCCSUPSType = { 0x01,0x00};

    /* eQMI_VOICE_SET_CALL_BARRING_PASSWORD */
const unpack_voice_SLQSVoiceSetCallBarringPassword_t const_unpack_voice_SLQSVoiceSetCallBarringPassword_t = {
        &cst_cBarPassFailCause,&cst_cBarPassAlphaIDInfo,&cst_cBarPassCCResType,&cst_cBarPassCallID,
        &cst_cBarPassCCSUPSType,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

uint16_t          var_cBarPassFailCause;
voice_alphaIDInfo var_cBarPassAlphaIDInfo;
uint8_t           var_cBarPassCCResType;
uint8_t           var_cBarPassCallID;
voice_ccSUPSType  var_cBarPassCCSUPSType;
    /* eQMI_VOICE_SET_CALL_BARRING_PASSWORD */
unpack_voice_SLQSVoiceSetCallBarringPassword_t var_unpack_voice_SLQSVoiceSetCallBarringPassword_t = {
        &var_cBarPassFailCause, &var_cBarPassAlphaIDInfo, &var_cBarPassCCResType, &var_cBarPassCallID,
        &var_cBarPassCCSUPSType,{{0}} };

voice_callInfo        cst_CallInfo = {0x01,0x01,0x00,0x01,0x02};
voice_remotePartyNum  cst_RemotePartyNum = {0x00,0x0A,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}};
uint16_t              cst_SrvOpt = 0x0008;
uint8_t               cst_VoicePrivacy = 0x00;
uint8_t               cst_OTASPStatus = 0x00;
voice_remotePartyName cst_RemotePartyName = {0x00,0x01,0x03,{0x47,0x75,0x72}};
voice_UUSInfo         cst_UUSInfo = {0x00,0x01,0x03,{0x31,0x32,0x33}};
uint8_t               cst_AlertType = 0x00;
voice_alphaIDInfo     cst_cInfoAlphaIDInfo = {0x01,0x03,{0x47,0x75,0x72}};
voice_connectNumInfo  cst_ConnectNumInfo = {0x00,0x01,0x02,0x03,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31}};
voice_diagInfo        cst_DiagInfo = {0x03,{0x31,0x31,0x31}};
uint32_t              cst_AlertingPattern = 0x11221122;
    /* eQMI_VOICE_GET_CALL_INFO */
const unpack_voice_SLQSVoiceGetCallInfo_t const_unpack_voice_SLQSVoiceGetCallInfo_t = {
        &cst_CallInfo,&cst_RemotePartyNum,&cst_SrvOpt,&cst_VoicePrivacy,
        &cst_OTASPStatus,&cst_RemotePartyName,&cst_UUSInfo,&cst_AlertType,
        &cst_cInfoAlphaIDInfo,&cst_ConnectNumInfo,&cst_DiagInfo,&cst_AlertingPattern,
        {{SWI_UINT256_BIT_VALUE(SET_13_BITS,2,16,17,18,19,20,21,22,23,24,25,26,27)}} };

voice_callInfo        var_CallInfo;
voice_remotePartyNum  var_RemotePartyNum;
uint16_t              var_SrvOpt;
uint8_t               var_VoicePrivacy;
uint8_t               var_OTASPStatus;
voice_remotePartyName var_RemotePartyName;
voice_UUSInfo         var_UUSInfo;
uint8_t               var_AlertType;
voice_alphaIDInfo     var_cInfoAlphaIDInfo;
voice_connectNumInfo  var_ConnectNumInfo;
voice_diagInfo        var_DiagInfo;
uint32_t              var_AlertingPattern;
    /* eQMI_VOICE_GET_CALL_INFO */
unpack_voice_SLQSVoiceGetCallInfo_t var_unpack_voice_SLQSVoiceGetCallInfo_t = {
        &var_CallInfo,&var_RemotePartyNum,&var_SrvOpt,&var_VoicePrivacy,
        &var_OTASPStatus,&var_RemotePartyName,&var_UUSInfo,&var_AlertType,
        &var_cInfoAlphaIDInfo,&var_ConnectNumInfo,&var_DiagInfo,&var_AlertingPattern,
        {{0}} };

voice_arrCallInfo        cst_ArrCallInfo = {0x01,{{{0x02,0x01,0x00,0x01,0x02},0x00,0x00}}};
voice_arrRemotePartyNum  cst_ArrRemotePartyNum = {0x01,{{0x02,{0x00,0x0A,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}}};
voice_arrRemotePartyName cst_ArrRemotePartyName = {0x01,{{0x02,{0x00,0x01,0x03,{0x31,0x32,0x33}}}}};
voice_arrAlertingType    cst_ArrAlertingType = {0x01,{0x02},{0x00}};
voice_arrUUSInfo         cst_ArrUUSInfo = {0x01,{{0x02,{0x00,0x01,0x03,{0x31,0x32,0x33}}}}};
voice_arrSvcOption       cst_ArrSvcOption = {0x01,{0x02},{0x0008}};
uint8_t                  cst_ACInfoOTASPStatus = 0x00;
uint8_t                  cst_ACInfoVoicePrivacy = 0x00;
voice_arrCallEndReason   cst_ArrCallEndReason = {0x01,{0x02},{0x0019}};
voice_arrAlphaID         cst_ArrAlphaID = {0x01,{{0x02,{0x00,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x31}}}}};
voice_arrConnectPartyNum cst_ArrConnectPartyNum ={0x01,{{0x02,0x01,0x01,0x02,0x03,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}} ;
voice_arrDiagInfo        cst_ArrDiagInfo = {0x01,{{0x02,{0x02,{'S','W'}}}}};
voice_arrCalledPartyNum  cst_ArrCalledPartyNum = {0x01,{{0x02,0x00,0x01,0x02,0x03,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}};
voice_arrRedirPartyNum   cst_ArrRedirPartyNum = {0x01,{{0x02,0x00,0x01,0x02,0x03,0x0A,
        {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}};
voice_arrAlertingPattern cst_ArrAlertingPattern = {0x01,{0x02},{0x22221111}};
/* eQMI_VOICE_GET_ALL_CALL_INFO */
const unpack_voice_SLQSVoiceGetAllCallInfo_t const_unpack_voice_SLQSVoiceGetAllCallInfo_t = {
    &cst_ArrCallInfo,&cst_ArrRemotePartyNum,&cst_ArrRemotePartyName,&cst_ArrAlertingType,
    &cst_ArrUUSInfo,&cst_ArrSvcOption,&cst_ACInfoOTASPStatus,&cst_ACInfoVoicePrivacy,
    &cst_ArrCallEndReason,&cst_ArrAlphaID,&cst_ArrConnectPartyNum,&cst_ArrDiagInfo,
    &cst_ArrCalledPartyNum,&cst_ArrRedirPartyNum,&cst_ArrAlertingPattern,
    {{SWI_UINT256_BIT_VALUE(SET_16_BITS,2,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30)}} };

voice_arrCallInfo        var_ArrCallInfo;
voice_arrRemotePartyNum  var_ArrRemotePartyNum;
voice_arrRemotePartyName var_ArrRemotePartyName;
voice_arrAlertingType    var_ArrAlertingType;
voice_arrUUSInfo         var_ArrUUSInfo;
voice_arrSvcOption       var_ArrSvcOption;
uint8_t                  var_ACInfoOTASPStatus;
uint8_t                  var_ACInfoVoicePrivacy;
voice_arrCallEndReason   var_ArrCallEndReason;
voice_arrAlphaID         var_ArrAlphaID;
voice_arrConnectPartyNum var_ArrConnectPartyNum;
voice_arrDiagInfo        var_ArrDiagInfo;
voice_arrCalledPartyNum  var_ArrCalledPartyNum;
voice_arrRedirPartyNum   var_ArrRedirPartyNum;
voice_arrAlertingPattern var_ArrAlertingPattern;
/* eQMI_VOICE_GET_ALL_CALL_INFO */
unpack_voice_SLQSVoiceGetAllCallInfo_t var_unpack_voice_SLQSVoiceGetAllCallInfo_t = {
    &var_ArrCallInfo,&var_ArrRemotePartyNum,&var_ArrRemotePartyName,&var_ArrAlertingType,
    &var_ArrUUSInfo,&var_ArrSvcOption,&var_ACInfoOTASPStatus,&var_ACInfoVoicePrivacy,
    &var_ArrCallEndReason,&var_ArrAlphaID,&var_ArrConnectPartyNum,&var_ArrDiagInfo,
    &var_ArrCalledPartyNum,&var_ArrRedirPartyNum,&var_ArrAlertingPattern,
    {{0}} };

uint16_t cst_cManageFailCause = 0x0015;
    /* eQMI_VOICE_MANAGE_CALLS */
const unpack_voice_SLQSVoiceManageCalls_t const_unpack_voice_SLQSVoiceManageCalls_t = {
        &cst_cManageFailCause,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

uint16_t var_cManageFailCause;
    /* eQMI_VOICE_MANAGE_CALLS */
unpack_voice_SLQSVoiceManageCalls_t var_unpack_voice_SLQSVoiceManageCalls_t = {
        &var_cManageFailCause, {{0}} };

uint8_t cst_burstDTMFCallID = 0x01;
    /* eQMI_VOICE_BURST_DTMF */
const unpack_voice_SLQSVoiceBurstDTMF_t const_unpack_voice_SLQSVoiceBurstDTMF_t = {
        &cst_burstDTMFCallID,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

uint8_t var_burstDTMFCallID;
    /* eQMI_VOICE_BURST_DTMF */
unpack_voice_SLQSVoiceBurstDTMF_t var_unpack_voice_SLQSVoiceBurstDTMF_t = {
        &var_burstDTMFCallID, {{0}} };

uint8_t cst_startContDTMFCallID = 0x02;
    /* eQMI_VOICE_START_CONT_DTMF */
const unpack_voice_SLQSVoiceStartContDTMF_t const_unpack_voice_SLQSVoiceStartContDTMF_t = {
        &cst_startContDTMFCallID,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

uint8_t var_startContDTMFCallID;
    /* eQMI_VOICE_START_CONT_DTMF */
unpack_voice_SLQSVoiceStartContDTMF_t var_unpack_voice_SLQSVoiceStartContDTMF_t = {
        &var_startContDTMFCallID, {{0}} };

    /* eQMI_VOICE_STOP_CONT_DTMF */
const unpack_voice_SLQSVoiceStopContDTMF_t const_unpack_voice_SLQSVoiceStopContDTMF_t = {
        0x02,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

uint8_t cst_sendFlashCallID = 0x03;
    /* eQMI_VOICE_SEND_FLASH */
const unpack_voice_SLQSVoiceSendFlash_t const_unpack_voice_SLQSVoiceSendFlash_t = {
        &cst_sendFlashCallID,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

uint8_t var_sendFlashCallID;
    /* eQMI_VOICE_SEND_FLASH */
unpack_voice_SLQSVoiceSendFlash_t var_unpack_voice_SLQSVoiceSendFlash_t = {
        &var_sendFlashCallID, {{0}} };

/* eQMI_VOICE_SET_PREFERRED_PRIVACY */
const unpack_voice_SLQSVoiceSetPreferredPrivacy_t const_unpack_voice_SLQSVoiceSetPreferredPrivacy_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_VOICE_INDICATION_REGISTER */
const unpack_voice_SLQSVoiceIndicationRegister_t const_unpack_voice_SLQSVoiceIndicationRegister_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint8_t            cst_AutoAnswerStat = 0x01;
voice_airTimer     cst_AirTimerCnt = {0x01,0x01020304};
voice_roamTimer    cst_RoamTimerCnt = {0x00,0x01010202};
uint8_t            cst_CurrTTYMode = 0x00;
voice_prefVoiceSO  cst_CurPrefVoiceSO = {0x00,0x01,0x0003,0x8001,0x0046};
voice_curAMRConfig cst_CurAMRConfig = {0x01,0x04};
uint8_t            cst_CurVoicePrivacyPref = 0x00;
uint8_t            cst_CurVoiceDomainPref = 0x00;
    /* eQMI_VOICE_GET_CONFIG */
const unpack_voice_SLQSVoiceGetConfig_t const_unpack_voice_SLQSVoiceGetConfig_t = {
        &cst_AutoAnswerStat,&cst_AirTimerCnt,&cst_RoamTimerCnt,&cst_CurrTTYMode,
        &cst_CurPrefVoiceSO,&cst_CurAMRConfig,&cst_CurVoicePrivacyPref,&cst_CurVoiceDomainPref,
        {{SWI_UINT256_BIT_VALUE(SET_9_BITS,2,16,17,18,19,20,21,22,23)}} };

uint8_t            var_AutoAnswerStat;
voice_airTimer     var_AirTimerCnt;
voice_roamTimer    var_RoamTimerCnt;
uint8_t            var_CurrTTYMode;
voice_prefVoiceSO  var_CurPrefVoiceSO;
voice_curAMRConfig var_CurAMRConfig;
uint8_t            var_CurVoicePrivacyPref;
uint8_t            var_CurVoiceDomainPref;
    /* eQMI_VOICE_GET_CONFIG */
unpack_voice_SLQSVoiceGetConfig_t var_unpack_voice_SLQSVoiceGetConfig_t = {
        &var_AutoAnswerStat,&var_AirTimerCnt,&var_RoamTimerCnt,&var_CurrTTYMode,
        &var_CurPrefVoiceSO,&var_CurAMRConfig,&var_CurVoicePrivacyPref,&var_CurVoiceDomainPref,
        {{0}} };

    /* eQMI_VOICE_ORIG_USSD_NO_WAIT */
const unpack_voice_SLQSVoiceOrigUSSDNoWait_t const_unpack_voice_SLQSVoiceOrigUSSDNoWait_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_VOICE_BIND_SUBSCRIPTION */
const unpack_voice_SLQSVoiceBindSubscription_t const_unpack_voice_SLQSVoiceBindSubscription_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_VOICE_ALS_SET_LINE_SWITCHING */
const unpack_voice_SLQSVoiceALSSetLineSwitching_t const_unpack_voice_SLQSVoiceALSSetLineSwitching_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_VOICE_ALS_SELECT_LINE */
const unpack_voice_SLQSVoiceALSSelectLine_t const_unpack_voice_SLQSVoiceALSSelectLine_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

voice_COLPResp     cst_COLPResp = {0x01,0x01};
uint16_t           cst_getColpFailCause = 0x0015;
voice_alphaIDInfo  cst_getColpAlphaIDInfo = {0x01,0x03,{0x31,0x32,0x33}};
uint8_t            cst_getColpCCResType = 0x00;
uint8_t            cst_getColpCallID = 0x05;
voice_ccSUPSType   cst_getColpCCSUPSType = {0x01,0x00};
swi_uint256_t      ParamPresenceMask;
    /* eQMI_VOICE_GET_COLP */
const unpack_voice_SLQSVoiceGetCOLP_t const_unpack_voice_SLQSVoiceGetCOLP_t = {
        &cst_COLPResp,&cst_getColpFailCause,&cst_getColpAlphaIDInfo,&cst_getColpCCResType,
        &cst_getColpCallID,&cst_getColpCCSUPSType,{{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

voice_COLPResp     var_COLPResp;
uint16_t           var_getColpFailCause;
voice_alphaIDInfo  var_getColpAlphaIDInfo;
uint8_t            var_getColpCCResType;
uint8_t            var_getColpCallID;
voice_ccSUPSType   var_getColpCCSUPSType;
    /* eQMI_VOICE_GET_COLP */
unpack_voice_SLQSVoiceGetCOLP_t var_unpack_voice_SLQSVoiceGetCOLP_t = {
        &var_COLPResp,&var_getColpFailCause,&var_getColpAlphaIDInfo,&var_getColpCCResType,
        &var_getColpCallID,&var_getColpCCSUPSType,{{0}} };

voice_COLRResp     cst_COLRResp = {0x01,0x01};
uint16_t           cst_getColrFailCause = 0x0015;
voice_alphaIDInfo  cst_getColrAlphaIDInfo = {0x01,0x03,{0x31,0x32,0x33}};
uint8_t            cst_getColrCCResType = 0x00;
uint8_t            cst_getColrCallID = 0x06;
voice_ccSUPSType   cst_getColrCCSUPSType = {0x01,0x00};
swi_uint256_t      ParamPresenceMask;
    /* eQMI_VOICE_GET_COLR */
const unpack_voice_SLQSVoiceGetCOLR_t const_unpack_voice_SLQSVoiceGetCOLR_t = {
        &cst_COLRResp,&cst_getColrFailCause,&cst_getColrAlphaIDInfo,&cst_getColrCCResType,
        &cst_getColrCallID,&cst_getColrCCSUPSType,{{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

voice_COLRResp     var_COLRResp;
uint16_t           var_getColrFailCause;
voice_alphaIDInfo  var_getColrAlphaIDInfo;
uint8_t            var_getColrCCResType;
uint8_t            var_getColrCallID;
voice_ccSUPSType   var_getColrCCSUPSType;
    /* eQMI_VOICE_GET_COLP */
unpack_voice_SLQSVoiceGetCOLR_t var_unpack_voice_SLQSVoiceGetCOLR_t = {
        &var_COLRResp,&var_getColrFailCause,&var_getColrAlphaIDInfo,&var_getColrCCResType,
        &var_getColrCallID,&var_getColrCCSUPSType,{{0}} };

voice_CNAPResp     cst_CNAPResp = {0x01,0x01};
uint16_t           cst_getCnapFailCause = 0x0015;
voice_alphaIDInfo  cst_getCnapAlphaIDInfo = {0x01,0x03,{0x31,0x32,0x33}};
uint8_t            cst_getCnapCCResType = 0x00;
uint8_t            cst_getCnapCallID = 0x07;
voice_ccSUPSType   cst_getCnapCCSUPSType = {0x01,0x00};
swi_uint256_t      ParamPresenceMask;
    /* eQMI_VOICE_GET_CNAP */
const unpack_voice_SLQSVoiceGetCNAP_t const_unpack_voice_SLQSVoiceGetCNAP_t = {
        &cst_CNAPResp,&cst_getCnapFailCause,&cst_getCnapAlphaIDInfo,&cst_getCnapCCResType,
        &cst_getCnapCallID,&cst_getCnapCCSUPSType,{{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

voice_CNAPResp     var_CNAPResp;
uint16_t           var_getCnapFailCause;
voice_alphaIDInfo  var_getCnapAlphaIDInfo;
uint8_t            var_getCnapCCResType;
uint8_t            var_getCnapCallID;
voice_ccSUPSType   var_getCnapCCSUPSType;
    /* eQMI_VOICE_GET_CNAP */
unpack_voice_SLQSVoiceGetCNAP_t var_unpack_voice_SLQSVoiceGetCNAP_t = {
        &var_CNAPResp,&var_getCnapFailCause,&var_getCnapAlphaIDInfo,&var_getCnapCCResType,
        &var_getCnapCallID,&var_getCnapCCSUPSType,{{0}} };

uint16_t                     cst_oUSSDfailureCause = 0x0015;
voice_alphaIDInfo            cst_oUSSDAlphaIDInfo = {0x01,0x03,{0x31,0x32,0x33}};;
struct voice_USSInfo         cst_oUSSDUSSDInfo= {0x01,0x03,{0x31,0x33,0x35}};
uint8_t                      cst_oUSSDCcResultType = 0x00;
uint8_t                      cst_oUSSDCallId = 0x08;
voice_ccSUPSType             cst_oUSSDCCSuppsType = {0x01,0x00};
/* eQMI_VOICE_ORIGINATE_USSD */
const unpack_voice_SLQSOriginateUSSD_t const_unpack_voice_SLQSOriginateUSSD_t = {
    &cst_oUSSDfailureCause,&cst_oUSSDAlphaIDInfo,&cst_oUSSDUSSDInfo,&cst_oUSSDCcResultType,
    &cst_oUSSDCallId,&cst_oUSSDCCSuppsType,{{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} };

uint16_t                     var_oUSSDfailureCause;
voice_alphaIDInfo            var_oUSSDAlphaIDInfo;
struct voice_USSInfo         var_oUSSDUSSDInfo;
uint8_t                      var_oUSSDCcResultType;
uint8_t                      var_oUSSDCallId;
voice_ccSUPSType             var_oUSSDCCSuppsType;

    /* eQMI_VOICE_ORIGINATE_USSD */
unpack_voice_SLQSOriginateUSSD_t var_unpack_voice_SLQSOriginateUSSD_t = {
        &var_oUSSDfailureCause,&var_oUSSDAlphaIDInfo,&var_oUSSDUSSDInfo,&var_oUSSDCcResultType,
        &var_oUSSDCallId,&var_oUSSDCCSuppsType,{{0}} };

    /* eQMI_VOICE_USSD_IND */
const unpack_voice_USSDNotificationCallback_ind_t const_unpack_voice_USSDNotificationCallback_ind_t = {
    0x02,{0x01,{0x02,0x03,{0x31,0x32,0x33}}},{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

    /* eQMI_VOICE_USSD_IND */
unpack_voice_USSDNotificationCallback_ind_t var_unpack_voice_USSDNotificationCallback_ind_t = {
0xFF,{0,{0,0,{0,0,0}}},{{0}} };

uint8_t                   cst_SupsCbSvcClass= 0x02;
uint8_t                   cst_SupsCbReason = 0x05;
uint8_t                   cst_SupsCbCallFWNum[] = {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30,0x00};
uint8_t                   cst_SupsCbCallFWTimerVal = 0x22;
struct voice_USSInfo      cst_SupsCbUSSInfo = {0x02,0x03,{0x01,0x02,0x03}};
uint8_t                   cst_SupsCbCallID = 0x07;
voice_alphaIDInfo         cst_SupsCbAlphaIDInfo = {0x01,0x03,{0x11,0x22,0x33}};
uint8_t                   cst_SupsCbCallBarPasswd[] = {0x39,0x39,0x39,0x39};
voice_newPwdData          cst_SupsCbNewPwdData = {{0x30,0x30,0x30,0x30},{0x30,0x30,0x30,0x30}};
uint8_t                   cst_SupsCbDataSrc = 0x01;
uint16_t                  cst_SupsCbFailCause = 0x0015;
voice_getCallFWInfo       cst_SupsCbCallFwdInfo = {0x01,{{0x01,0x02,0x0A,{0x38,0x34,0x32,0x37,
                                                    0x32,0x30,0x35,0x38,0x37,0x30},0x00}}};
voice_CLIRResp            cst_SupsCbCLIRstatus = {0x01,0x02};
voice_CLIPResp            cst_SupsCbCLIPstatus = {0x01,0x01};
voice_COLPResp            cst_SupsCbCOLPstatus = {0x01,0x01};
voice_COLRResp            cst_SupsCbCOLRstatus = {0x01,0x01};
voice_CNAPResp            cst_SupsCbCNAPstatus = {0x01,0x01};

    /* eQMI_VOICE_SUPS_IND */
const unpack_voice_SLQSVoiceSUPSCallback_ind_t const_unpack_voice_SLQSVoiceSUPSCallback_ind_t = {
{0x01,0x01},&cst_SupsCbSvcClass,&cst_SupsCbReason,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)}} };

uint8_t                   var_SupsCbSvcClass;
uint8_t                   var_SupsCbReason;

/* eQMI_VOICE_SUPS_IND */
unpack_voice_SLQSVoiceSUPSCallback_ind_t var_unpack_voice_SLQSVoiceSUPSCallback_ind_t = {
    {0x01,0x01},&var_SupsCbSvcClass,&var_SupsCbReason,NULL,
    NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,
    NULL,NULL,{{0}} };

    /* eQMI_VOICE_PRIVACY_IND */
const unpack_voice_voicePrivacyChangeCallback_ind_t const_unpack_voice_voicePrivacyChangeCallback_ind_t = {
0x05,0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

uint16_t     cst_supsNotCUGIndex = 0x1122;
voice_ECTNum cst_supsNotECTNum = {0x00,0x02,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}};
    /* eQMI_VOICE_SUPS_NOTIFICATION_IND */
const unpack_voice_SUPSNotificationCallback_ind_t const_unpack_voice_SUPSNotificationCallback_ind_t = {
0x06,0x02,&cst_supsNotCUGIndex,&cst_supsNotECTNum,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)}} };

uint16_t     var_supsNotCUGIndex;
voice_ECTNum var_supsNotECTNum;
    /* eQMI_VOICE_SUPS_NOTIFICATION_IND */
unpack_voice_SUPSNotificationCallback_ind_t var_unpack_voice_SUPSNotificationCallback_ind_t = {
0xFF,0xFF,&var_supsNotCUGIndex,&var_supsNotECTNum,{{0}} };

uint8_t     cst_OnLength = 0x05;
uint8_t     cst_OffLength = 0x02;
    /* eQMI_VOICE_DTMF_IND */
const unpack_voice_DTMFEventCallback_ind_t const_unpack_voice_DTMFEventCallback_ind_t = {
{0x07,0x01,0x04,{0x38,0x34,0x32,0x37}},&cst_OnLength,&cst_OffLength,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)}} };

uint8_t     var_OnLength;
uint8_t     var_OffLength;
    /* eQMI_VOICE_DTMF_IND */
unpack_voice_DTMFEventCallback_ind_t var_unpack_voice_DTMFEventCallback_ind_t = {
{0xFF,0xFF,0xFF,{0}},&var_OnLength,&var_OffLength,{{0}} };

    /* eQMI_VOICE_OTASP_STATUS_IND */
const unpack_voice_OTASPStatusCallback_ind_t const_unpack_voice_OTASPStatusCallback_ind_t = {
0x09,0x02,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

voice_signalInfo      cst_SignalInfo = {0x01,0x02,0x03};
voice_callerIDInfo    cst_CallerIDInfo = {0x01,0x0A,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}};
uint8_t               cst_DispInfo[] = {0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30};
uint8_t               cst_ExtDispInfo[] = {0x30,0x31};
uint8_t               cst_CallerNameInfo[] = {'S','W'};

    /* eQMI_VOICE_INFO_REC_IND */
/*const unpack_voice_VoiceInfoRecCallback_ind_t const_unpack_voice_VoiceInfoRecCallback_ind_t = {
0x04,&cst_SignalInfo,&cst_CallerIDInfo,cst_DispInfo,
cst_ExtDispInfo,cst_CallerNameInfo,&cst_CallWaitInd,&cst_RecIndConnectNumInfo,
&cst_CallingPartyInfo,&cst_CalledPartyInfo,&cst_RedirNumInfo,&cst_CLIRCause,
&cst_NSSAudioCtrl,&cst_NSSRelease,&cst_LineCtrlInfo,&cst_ExtDispRecInfo,
{{SWI_UINT256_BIT_VALUE(SET_16_BITS,1,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30)}} };
*/
const unpack_voice_VoiceInfoRecCallback_ind_t const_unpack_voice_VoiceInfoRecCallback_ind_t = {
0x04,&cst_SignalInfo,&cst_CallerIDInfo,cst_DispInfo,
cst_ExtDispInfo,cst_CallerNameInfo,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
{{SWI_UINT256_BIT_VALUE(SET_6_BITS,1,16,17,18,19,20,)}} };

voice_signalInfo      var_SignalInfo;
voice_callerIDInfo    var_CallerIDInfo;
uint8_t               var_DispInfo[15];
uint8_t               var_ExtDispInfo[5];
uint8_t               var_CallerNameInfo[5];
uint8_t               var_CallWaitInd;
voice_connectNumInfo  var_RecIndConnectNumInfo;
voice_connectNumInfo  var_CallingPartyInfo;
voice_calledPartyInfo var_CalledPartyInfo;
voice_redirNumInfo    var_RedirNumInfo;
uint8_t               var_CLIRCause;
voice_NSSAudioCtrl    var_NSSAudioCtrl;
uint8_t               var_NSSRelease;
voice_lineCtrlInfo    var_LineCtrlInfo;
voice_extDispRecInfo  var_ExtDispRecInfo;

    /* eQMI_VOICE_INFO_REC_IND */
unpack_voice_VoiceInfoRecCallback_ind_t var_unpack_voice_VoiceInfoRecCallback_ind_t = {
0x04,&var_SignalInfo,&var_CallerIDInfo,var_DispInfo,
var_ExtDispInfo,var_CallerNameInfo,&var_CallWaitInd,&var_RecIndConnectNumInfo,
&var_CallingPartyInfo,&var_CalledPartyInfo,&var_RedirNumInfo,&var_CLIRCause,
&var_NSSAudioCtrl,&var_NSSRelease,&var_LineCtrlInfo,&var_ExtDispRecInfo,
{{0}} };

voice_arrRemotePartyNum  cst_CbArrRemotePartyNum = {0x01,{{0x01,{0x00,0x0A,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}}};
voice_arrRemotePartyName cst_CbArrRemotePartyName = {0x01,{{0x01,{0x00,0x00,0x02,{'S','W'}}}}};
voice_arrAlertingType    cst_CbArrAlertingType = {0x01,{0x01},{0x00}};
voice_arrSvcOption       cst_CbArrSvcOption = {0x01,{0x01},{0x0102}};
voice_arrCallEndReason   cst_CbArrCallEndReason = {0x01,{0x01},{0x0015}};
voice_arrAlphaID         cst_CbArrAlphaID = {0x01,{{0x01,{0x01,0x02,{0x11,0x22}}}}};
voice_arrConnectPartyNum cst_CbArrConnectPartyNum = {0x01,{{0x01,0x01,0x01,0x02,0x03,0x0A,{0x38,0x34,0x32,0x37,0x32,0x30,0x35,0x38,0x37,0x30}}}};

    /* eQMI_VOICE_ALL_CALL_STATUS_IND */
const unpack_voice_allCallStatusCallback_ind_t const_unpack_voice_allCallStatusCallback_ind_t = {
        {0x01,{{{0x01,0x02,0x00,0x02,0x02},0x01,0x00}}},&cst_CbArrRemotePartyNum,&cst_CbArrRemotePartyName,&cst_CbArrAlertingType,
        &cst_CbArrSvcOption,&cst_CbArrCallEndReason,&cst_CbArrAlphaID,&cst_CbArrConnectPartyNum,
        NULL,NULL,NULL,NULL,{{SWI_UINT256_BIT_VALUE(SET_8_BITS,1,16,17,18,19,20,21,22)}} };

voice_arrRemotePartyNum  var_CbArrRemotePartyNum;
voice_arrRemotePartyName var_CbArrRemotePartyName;
voice_arrAlertingType    var_CbArrAlertingType;
voice_arrSvcOption       var_CbArrSvcOption;
voice_arrCallEndReason   var_CbArrCallEndReason;
voice_arrAlphaID         var_CbArrAlphaID;
voice_arrConnectPartyNum var_CbArrConnectPartyNum;

    /* eQMI_VOICE_ALL_CALL_STATUS_IND */
unpack_voice_allCallStatusCallback_ind_t var_unpack_voice_allCallStatusCallback_ind_t = {
        {0x01,{{{0x01,0x02,0x00,0x02,0x02},0x01,0x00}}},&var_CbArrRemotePartyNum,&var_CbArrRemotePartyName,&var_CbArrAlertingType,
        &var_CbArrSvcOption,&var_CbArrCallEndReason,&var_CbArrAlphaID,&var_CbArrConnectPartyNum,
        NULL,NULL,NULL,NULL,{{SWI_UINT256_BIT_VALUE(SET_8_BITS,1,16,17,18,19,20,21,22)}} };

int voice_validate_dummy_unpack()
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
    loopCount = sizeof(validate_voice_resp_msg)/sizeof(validate_voice_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_voice_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eVOICE, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("VOICE IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("VOICE RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_VOICE_ORIGINATE_USSD:
                if (eRSP == rsp_ctx.type)
                {
                    if(rsp_ctx.xid == 0x01)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_voice_OriginateUSSD,
                        dump_common_resultcode,
                        msg.buf,
                        rlen,
                        &const_unpack_voice_OriginateUSSD_t);
                    }
                    if(rsp_ctx.xid == 0x20)
                    {
                        unpack_voice_SLQSOriginateUSSD_t *varp = &var_unpack_voice_SLQSOriginateUSSD_t;
                        const unpack_voice_SLQSOriginateUSSD_t *cstp = &const_unpack_voice_SLQSOriginateUSSD_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_voice_SLQSOriginateUSSD,
                        dump_SLQSOriginateUSSD,
                        msg.buf,
                        rlen,
                        &var_unpack_voice_SLQSOriginateUSSD_t,
                        7,
                        CMP_PTR_TYPE, varp->pfailureCause, cstp->pfailureCause,
                        CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                        CMP_PTR_TYPE, varp->pUSSDInfo, cstp->pUSSDInfo,
                        CMP_PTR_TYPE, varp->pCcResultType, cstp->pCcResultType,
                        CMP_PTR_TYPE, varp->pCallId, cstp->pCallId,
                        CMP_PTR_TYPE, varp->pCCSuppsType, cstp->pCCSuppsType,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                    }
                }
                break;
            case eQMI_VOICE_ANSWER_USSD:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_AnswerUSSD,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_AnswerUSSD_t);
                }
                break;
            case eQMI_VOICE_CANCEL_USSD:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_CancelUSSD,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_CancelUSSD_t);
                }
                break;
            case eQMI_VOICE_DIAL_CALL:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceDialCall_t *varp = &var_unpack_voice_SLQSVoiceDialCall_t;
                    const unpack_voice_SLQSVoiceDialCall_t *cstp = &const_unpack_voice_SLQSVoiceDialCall_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceDialCall,
                    dump_SLQSVoiceDialCall,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceDialCall_t,
                    5,
                    CMP_PTR_TYPE, varp->pCallID,        cstp->pCallID,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo,   cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResultType,  cstp->pCCResultType,
                    CMP_PTR_TYPE, varp->pCCSUPSType,    cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_END_CALL:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceEndCall_t *varp = &var_unpack_voice_SLQSVoiceEndCall_t;
                    const unpack_voice_SLQSVoiceEndCall_t *cstp = &const_unpack_voice_SLQSVoiceEndCall_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceEndCall,
                    dump_SLQSVoiceEndCall,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceEndCall_t,
                    2,
                    CMP_PTR_TYPE, varp->pCallId, cstp->pCallId,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_SET_SUPS_SERVICE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceSetSUPSService_t *varp = &var_unpack_voice_SLQSVoiceSetSUPSService_t;
                    const unpack_voice_SLQSVoiceSetSUPSService_t *cstp = &const_unpack_voice_SLQSVoiceSetSUPSService_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceSetSUPSService,
                    dump_SLQSVoiceSetSUPSService,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceSetSUPSService_t,
                    6,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResultType, cstp->pCCResultType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_SET_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceSetConfig_t *varp = &var_unpack_voice_SLQSVoiceSetConfig_t;
                    const unpack_voice_SLQSVoiceSetConfig_t *cstp = &const_unpack_voice_SLQSVoiceSetConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceSetConfig,
                    dump_SLQSVoiceSetConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceSetConfig_t,
                    7,
                    CMP_PTR_TYPE, varp->pAutoAnsStatus, cstp->pAutoAnsStatus,
                    CMP_PTR_TYPE, varp->pAirTimerStatus, cstp->pAirTimerStatus,
                    CMP_PTR_TYPE, varp->pRoamTimerStatus, cstp->pRoamTimerStatus,
                    CMP_PTR_TYPE, varp->pTTYConfigStatus, cstp->pTTYConfigStatus,
                    CMP_PTR_TYPE, varp->pPrefVoiceSOStatus, cstp->pPrefVoiceSOStatus,
                    CMP_PTR_TYPE, varp->pVoiceDomainPrefStatus, cstp->pVoiceDomainPrefStatus,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_ANSWER_CALL:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceAnswerCall_t *varp = &var_unpack_voice_SLQSVoiceAnswerCall_t;
                    const unpack_voice_SLQSVoiceAnswerCall_t *cstp = &const_unpack_voice_SLQSVoiceAnswerCall_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceAnswerCall,
                    dump_SLQSVoiceAnswerCall,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceAnswerCall_t,
                    2,
                    CMP_PTR_TYPE, varp->pCallId, cstp->pCallId,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CLIR:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCLIR_t *varp = &var_unpack_voice_SLQSVoiceGetCLIR_t;
                    const unpack_voice_SLQSVoiceGetCLIR_t *cstp = &const_unpack_voice_SLQSVoiceGetCLIR_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCLIR,
                    dump_SLQSVoiceGetCLIR,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCLIR_t,
                    7,
                    CMP_PTR_TYPE, varp->pCLIRResp, cstp->pCLIRResp,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CLIP:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCLIP_t *varp = &var_unpack_voice_SLQSVoiceGetCLIP_t;
                    const unpack_voice_SLQSVoiceGetCLIP_t *cstp = &const_unpack_voice_SLQSVoiceGetCLIP_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCLIP,
                    dump_SLQSVoiceGetCLIP,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCLIP_t,
                    7,
                    CMP_PTR_TYPE, varp->pCLIPResp, cstp->pCLIPResp,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CALL_WAITING:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCallWaiting_t *varp = &var_unpack_voice_SLQSVoiceGetCallWaiting_t;
                    const unpack_voice_SLQSVoiceGetCallWaiting_t *cstp = &const_unpack_voice_SLQSVoiceGetCallWaiting_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCallWaiting,
                    dump_SLQSVoiceGetCallWaiting,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCallWaiting_t,
                    7,
                    CMP_PTR_TYPE, varp->pSvcClass, cstp->pSvcClass,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CALL_BARRING:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCallBarring_t *varp = &var_unpack_voice_SLQSVoiceGetCallBarring_t;
                    const unpack_voice_SLQSVoiceGetCallBarring_t *cstp = &const_unpack_voice_SLQSVoiceGetCallBarring_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCallBarring,
                    dump_SLQSVoiceGetCallBarring,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCallBarring_t,
                    7,
                    CMP_PTR_TYPE, varp->pSvcClass, cstp->pSvcClass,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CALL_FORWARDING:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCallForwardingStatus_t *varp = &var_unpack_voice_SLQSVoiceGetCallForwardingStatus_t;
                    const unpack_voice_SLQSVoiceGetCallForwardingStatus_t *cstp = &const_unpack_voice_SLQSVoiceGetCallForwardingStatus_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCallForwardingStatus,
                    dump_SLQSVoiceGetCallForwardingStatus,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCallForwardingStatus_t,
                    8,
                    CMP_PTR_TYPE, varp->pGetCallFWInfo, cstp->pGetCallFWInfo,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, varp->pGetCallFWExtInfo, cstp->pGetCallFWExtInfo,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_SET_CALL_BARRING_PASSWORD:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceSetCallBarringPassword_t *varp = &var_unpack_voice_SLQSVoiceSetCallBarringPassword_t;
                    const unpack_voice_SLQSVoiceSetCallBarringPassword_t *cstp = &const_unpack_voice_SLQSVoiceSetCallBarringPassword_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceSetCallBarringPassword,
                    dump_SLQSVoiceSetCallBarringPassword,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceSetCallBarringPassword_t,
                    6,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CALL_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCallInfo_t *varp = &var_unpack_voice_SLQSVoiceGetCallInfo_t;
                    const unpack_voice_SLQSVoiceGetCallInfo_t *cstp = &const_unpack_voice_SLQSVoiceGetCallInfo_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCallInfo,
                    dump_SLQSVoiceGetCallInfo,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCallInfo_t,
                    13,
                    CMP_PTR_TYPE, varp->pCallInfo, cstp->pCallInfo,
                    CMP_PTR_TYPE, varp->pRemotePartyNum, cstp->pRemotePartyNum,
                    CMP_PTR_TYPE, varp->pSrvOpt, cstp->pSrvOpt,
                    CMP_PTR_TYPE, varp->pVoicePrivacy, cstp->pVoicePrivacy,
                    CMP_PTR_TYPE, varp->pOTASPStatus, cstp->pOTASPStatus,
                    CMP_PTR_TYPE, varp->pRemotePartyName, cstp->pRemotePartyName,
                    CMP_PTR_TYPE, varp->pUUSInfo, cstp->pUUSInfo,
                    CMP_PTR_TYPE, varp->pAlertType, cstp->pAlertType,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pConnectNumInfo, cstp->pConnectNumInfo,
                    CMP_PTR_TYPE, varp->pDiagInfo, cstp->pDiagInfo,
                    CMP_PTR_TYPE, varp->pAlertingPattern, cstp->pAlertingPattern,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_ALL_CALL_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetAllCallInfo_t *varp = &var_unpack_voice_SLQSVoiceGetAllCallInfo_t;
                    const unpack_voice_SLQSVoiceGetAllCallInfo_t *cstp = &const_unpack_voice_SLQSVoiceGetAllCallInfo_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetAllCallInfo,
                    dump_SLQSVoiceGetAllCallInfo,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetAllCallInfo_t,
                    16,
                    CMP_PTR_TYPE, varp->pArrCallInfo, cstp->pArrCallInfo,
                    CMP_PTR_TYPE, varp->pArrRemotePartyNum, cstp->pArrRemotePartyNum,
                    CMP_PTR_TYPE, varp->pArrRemotePartyName, cstp->pArrRemotePartyName,
                    CMP_PTR_TYPE, varp->pArrAlertingType, cstp->pArrAlertingType,
                    CMP_PTR_TYPE, varp->pArrUUSInfo, cstp->pArrUUSInfo,
                    CMP_PTR_TYPE, varp->pArrSvcOption, cstp->pArrSvcOption,
                    CMP_PTR_TYPE, varp->pOTASPStatus, cstp->pOTASPStatus,
                    CMP_PTR_TYPE, varp->pVoicePrivacy, cstp->pVoicePrivacy,
                    CMP_PTR_TYPE, varp->pArrCallEndReason, cstp->pArrCallEndReason,
                    CMP_PTR_TYPE, varp->pArrAlphaID, cstp->pArrAlphaID,
                    CMP_PTR_TYPE, varp->pArrConnectPartyNum, cstp->pArrConnectPartyNum,
                    CMP_PTR_TYPE, varp->pArrDiagInfo, cstp->pArrDiagInfo,
                    CMP_PTR_TYPE, varp->pArrCalledPartyNum, cstp->pArrCalledPartyNum,
                    CMP_PTR_TYPE, varp->pArrRedirPartyNum, cstp->pArrRedirPartyNum,
                    CMP_PTR_TYPE, varp->pArrAlertingPattern, cstp->pArrAlertingPattern,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_MANAGE_CALLS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceManageCalls_t *varp = &var_unpack_voice_SLQSVoiceManageCalls_t;
                    const unpack_voice_SLQSVoiceManageCalls_t *cstp = &const_unpack_voice_SLQSVoiceManageCalls_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceManageCalls,
                    dump_SLQSVoiceManageCalls,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceManageCalls_t,
                    2,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_BURST_DTMF:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceBurstDTMF_t *varp = &var_unpack_voice_SLQSVoiceBurstDTMF_t;
                    const unpack_voice_SLQSVoiceBurstDTMF_t *cstp = &const_unpack_voice_SLQSVoiceBurstDTMF_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceBurstDTMF,
                    dump_SLQSVoiceBurstDTMF,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceBurstDTMF_t,
                    2,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_START_CONT_DTMF:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceStartContDTMF_t *varp = &var_unpack_voice_SLQSVoiceStartContDTMF_t;
                    const unpack_voice_SLQSVoiceStartContDTMF_t *cstp = &const_unpack_voice_SLQSVoiceStartContDTMF_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceStartContDTMF,
                    dump_SLQSVoiceStartContDTMF,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceStartContDTMF_t,
                    2,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_STOP_CONT_DTMF:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceStopContDTMF,
                    dump_SLQSVoiceStopContDTMF,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceStopContDTMF_t);
                }
                break;
            case eQMI_VOICE_SEND_FLASH:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceSendFlash_t *varp = &var_unpack_voice_SLQSVoiceSendFlash_t;
                    const unpack_voice_SLQSVoiceSendFlash_t *cstp = &const_unpack_voice_SLQSVoiceSendFlash_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceSendFlash,
                    dump_SLQSVoiceSendFlash,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceSendFlash_t,
                    2,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_SET_PREFERRED_PRIVACY:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_OriginateUSSD,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceSetPreferredPrivacy_t);
                }
                break;
            case eQMI_VOICE_INDICATION_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceIndicationRegister,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceIndicationRegister_t);
                }
                break;
            case eQMI_VOICE_GET_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetConfig_t *varp = &var_unpack_voice_SLQSVoiceGetConfig_t;
                    const unpack_voice_SLQSVoiceGetConfig_t *cstp = &const_unpack_voice_SLQSVoiceGetConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetConfig,
                    dump_SLQSVoiceGetConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetConfig_t,
                    9,
                    CMP_PTR_TYPE, varp->pAutoAnswerStat, cstp->pAutoAnswerStat,
                    CMP_PTR_TYPE, varp->pAirTimerCnt, cstp->pAirTimerCnt,
                    CMP_PTR_TYPE, varp->pRoamTimerCnt, cstp->pRoamTimerCnt,
                    CMP_PTR_TYPE, varp->pCurrTTYMode, cstp->pCurrTTYMode,
                    CMP_PTR_TYPE, varp->pCurPrefVoiceSO, cstp->pCurPrefVoiceSO,
                    CMP_PTR_TYPE, varp->pCurAMRConfig, cstp->pCurAMRConfig,
                    CMP_PTR_TYPE, varp->pCurVoicePrivacyPref, cstp->pCurVoicePrivacyPref,
                    CMP_PTR_TYPE, varp->pCurVoiceDomainPref, cstp->pCurVoiceDomainPref,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_ORIG_USSD_NO_WAIT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceOrigUSSDNoWait,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceOrigUSSDNoWait_t);
                }
                break;
            case eQMI_VOICE_BIND_SUBSCRIPTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceBindSubscription,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceBindSubscription_t);
                }
                break;
            case eQMI_VOICE_ALS_SET_LINE_SWITCHING:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceALSSetLineSwitching,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceALSSetLineSwitching_t);
                }
                break;
            case eQMI_VOICE_ALS_SELECT_LINE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceALSSelectLine,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_SLQSVoiceALSSelectLine_t);
                }
                break;
            case eQMI_VOICE_GET_COLP:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCOLP_t *varp = &var_unpack_voice_SLQSVoiceGetCOLP_t;
                    const unpack_voice_SLQSVoiceGetCOLP_t *cstp = &const_unpack_voice_SLQSVoiceGetCOLP_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCOLP,
                    dump_SLQSVoiceGetCOLP,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCOLP_t,
                    7,
                    CMP_PTR_TYPE, varp->pCOLPResp, cstp->pCOLPResp,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_COLR:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceGetCOLR_t *varp = &var_unpack_voice_SLQSVoiceGetCOLR_t;
                    const unpack_voice_SLQSVoiceGetCOLR_t *cstp = &const_unpack_voice_SLQSVoiceGetCOLR_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceGetCOLR,
                    dump_SLQSVoiceGetCOLR,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceGetCOLR_t,
                    7,
                    CMP_PTR_TYPE, varp->pCOLRResp, cstp->pCOLRResp,
                    CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                    CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                    CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                    CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                    CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_GET_CNAP:
                     if (eRSP == rsp_ctx.type)
                     {
                         unpack_voice_SLQSVoiceGetCNAP_t *varp = &var_unpack_voice_SLQSVoiceGetCNAP_t;
                         const unpack_voice_SLQSVoiceGetCNAP_t *cstp = &const_unpack_voice_SLQSVoiceGetCNAP_t;
                         UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                         unpack_voice_SLQSVoiceGetCNAP,
                         dump_SLQSVoiceGetCNAP,
                         msg.buf,
                         rlen,
                         &var_unpack_voice_SLQSVoiceGetCNAP_t,
                         7,
                         CMP_PTR_TYPE, varp->pCNAPResp, cstp->pCNAPResp,
                         CMP_PTR_TYPE, varp->pFailCause, cstp->pFailCause,
                         CMP_PTR_TYPE, varp->pAlphaIDInfo, cstp->pAlphaIDInfo,
                         CMP_PTR_TYPE, varp->pCCResType, cstp->pCCResType,
                         CMP_PTR_TYPE, varp->pCallID, cstp->pCallID,
                         CMP_PTR_TYPE, varp->pCCSUPSType, cstp->pCCSUPSType,
                         CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                     }
                     break;
            case eQMI_VOICE_USSD_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_USSDNotificationCallback_ind_t *varp = &var_unpack_voice_USSDNotificationCallback_ind_t;
                    const unpack_voice_USSDNotificationCallback_ind_t *cstp = &const_unpack_voice_USSDNotificationCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_USSDNotificationCallback_ind,
                    displayUSSDNotificationCallback,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_USSDNotificationCallback_ind_t,
                    5,
                    CMP_PTR_TYPE, &varp->notification_Type, &cstp->notification_Type,
                    CMP_PTR_TYPE, &varp->USSDNotificationNetworkInfo.networkInfo.ussDCS, &cstp->USSDNotificationNetworkInfo.networkInfo.ussDCS,
                    CMP_PTR_TYPE, varp->USSDNotificationNetworkInfo.networkInfo.ussData, cstp->USSDNotificationNetworkInfo.networkInfo.ussData,
                    CMP_PTR_TYPE, &varp->USSDNotificationNetworkInfo.networkInfo.ussLen, &cstp->USSDNotificationNetworkInfo.networkInfo.ussLen,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_SUPS_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_SLQSVoiceSUPSCallback_ind_t *varp = &var_unpack_voice_SLQSVoiceSUPSCallback_ind_t;
                    const unpack_voice_SLQSVoiceSUPSCallback_ind_t *cstp = &const_unpack_voice_SLQSVoiceSUPSCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SLQSVoiceSUPSCallback_ind,
                    dispalyVoiceSUPSCallback,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SLQSVoiceSUPSCallback_ind_t,
                    4,
                    CMP_PTR_TYPE, &varp->SUPSInformation, &cstp->SUPSInformation,
                    CMP_PTR_TYPE, varp->pSvcClass, cstp->pSvcClass,
                    CMP_PTR_TYPE, varp->pReason, cstp->pReason,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_PRIVACY_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_voicePrivacyChangeCallback_ind,
                    dump_voicePrivacyChangeCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_voicePrivacyChangeCallback_ind_t);
                }
                break;
            case eQMI_VOICE_SUPS_NOTIFICATION_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_SUPSNotificationCallback_ind_t *varp = &var_unpack_voice_SUPSNotificationCallback_ind_t;
                    const unpack_voice_SUPSNotificationCallback_ind_t *cstp = &const_unpack_voice_SUPSNotificationCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_SUPSNotificationCallback_ind,
                    dump_SUPSNotificationCallback_ind,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_SUPSNotificationCallback_ind_t,
                    5,
                    CMP_PTR_TYPE, &varp->callID, &cstp->callID,
                    CMP_PTR_TYPE, &varp->notifType, &cstp->notifType,
                    CMP_PTR_TYPE, varp->pCUGIndex, cstp->pCUGIndex,
                    CMP_PTR_TYPE, varp->pECTNum, cstp->pECTNum,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_DTMF_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_DTMFEventCallback_ind_t *varp = &var_unpack_voice_DTMFEventCallback_ind_t;
                    const unpack_voice_DTMFEventCallback_ind_t *cstp = &const_unpack_voice_DTMFEventCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_DTMFEventCallback_ind,
                    dump_DTMFEventCallback_ind,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_DTMFEventCallback_ind_t,
                    4,
                    CMP_PTR_TYPE, &varp->DTMFInformation, &cstp->DTMFInformation,
                    CMP_PTR_TYPE, varp->pOnLength, cstp->pOnLength,
                    CMP_PTR_TYPE, varp->pOffLength, cstp->pOffLength,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_OTASP_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_OTASPStatusCallback_ind,
                    dump_OTASPStatusCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_voice_OTASPStatusCallback_ind_t);
                }
                break;
            case eQMI_VOICE_INFO_REC_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_VoiceInfoRecCallback_ind_t *varp = &var_unpack_voice_VoiceInfoRecCallback_ind_t;
                    const unpack_voice_VoiceInfoRecCallback_ind_t *cstp = &const_unpack_voice_VoiceInfoRecCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_VoiceInfoRecCallback_ind,
                    dump_voiceInfoRecCallback_ind,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_VoiceInfoRecCallback_ind_t,
                    7,
                    CMP_PTR_TYPE, &varp->callID, &cstp->callID,
                    CMP_PTR_TYPE, varp->pSignalInfo, cstp->pSignalInfo,
                    CMP_PTR_TYPE, varp->pCallerIDInfo, cstp->pCallerIDInfo,
                    CMP_PTR_TYPE, varp->pDispInfo, cstp->pDispInfo,
                    CMP_PTR_TYPE, varp->pExtDispInfo, cstp->pExtDispInfo,
                    CMP_PTR_TYPE, varp->pCallerNameInfo, cstp->pCallerNameInfo,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_VOICE_ALL_CALL_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_voice_allCallStatusCallback_ind_t *varp = &var_unpack_voice_allCallStatusCallback_ind_t;
                    const unpack_voice_allCallStatusCallback_ind_t *cstp = &const_unpack_voice_allCallStatusCallback_ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_voice_allCallStatusCallback_ind,
                    dispalyAllCallStatusCallback,
                    msg.buf,
                    rlen,
                    &var_unpack_voice_allCallStatusCallback_ind_t,
                    9,
                    CMP_PTR_TYPE, &varp->arrCallInfomation, &cstp->arrCallInfomation,
                    CMP_PTR_TYPE, varp->pArrRemotePartyNum, cstp->pArrRemotePartyNum,
                    CMP_PTR_TYPE, varp->pArrRemotePartyName, cstp->pArrRemotePartyName,
                    CMP_PTR_TYPE, varp->pArrAlertingType, cstp->pArrAlertingType,
                    CMP_PTR_TYPE, varp->pArrSvcOption, cstp->pArrSvcOption,
                    CMP_PTR_TYPE, varp->pArrCallEndReason, cstp->pArrCallEndReason,
                    CMP_PTR_TYPE, varp->pArrAlphaID, cstp->pArrAlphaID,
                    CMP_PTR_TYPE, varp->pArrConnectPartyNum, cstp->pArrConnectPartyNum,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
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


