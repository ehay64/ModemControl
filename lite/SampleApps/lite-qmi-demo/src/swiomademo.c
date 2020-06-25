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
#include "swioma.h"
#include "packingdemo.h"

#define QMI_SWIOMA_MSG_MAX QMI_MSG_MAX

// FUNCTION PROTOTYPES
int client_fd(uint8_t svc);
int swioma_validate_dummy_unpack();

// SWIOMA GLOBALS
volatile int enSwiOmaThread;
int swioma_fd = -1;
pthread_t swioma_tid = 0;
pthread_attr_t swioma_attr;
volatile int ReadyToWriteNext;
volatile uint16_t Sendxid;
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;

int g_oemApiIndex_swioma = 0;

// TEST VALUES
#ifdef SWIOMA_EVENTS_DEBUG
uint8_t swioma_event_sample[] = {};
#endif //SWIOMA_EVENTS_DEBUG
////Modem Settings
int iGetOMADMSettings=0;
uint8_t  defaultAutosdm=0;
uint8_t  defaultFwAutoCheck=0;
pack_swioma_SLQSOMADMSetSettings_t DefaultOMADMSetSettings={0,0,&defaultAutosdm,&defaultFwAutoCheck};
pack_swioma_SLQSOMADMStartSession_t packStartSessionReq = {3};
unpack_swioma_SLQSOMADMStartSession_t unpackStartSessionRsp={0, SWI_UINT256_INT_VALUE};
pack_swioma_SLQSOMADMGetSessionInfo_t packGetSessionInfoReq = {0xFF}; //any active OMADM session type
unpack_swioma_SLQSOMADMGetSessionInfo_t unpackGetSessionInfoReq = {
 0,
 0,
 0,
 LITEQMI_MAX_SWIOMA_STR_LEN,
 {0},
 LITEQMI_MAX_SWIOMA_STR_LEN,
 {0},
 LITEQMI_MAX_SWIOMA_STR_LEN,
 {0}, 
 LITEQMI_MAX_SWIOMA_STR_LEN,
 {0},
 LITEQMI_MAX_SWIOMA_STR_LEN,
 {0},
 0,
 0,
 0,
 SWI_UINT256_INT_VALUE
};

uint8_t autosdm     = 0;
uint8_t fwAutoCheck = 0;
pack_swioma_SLQSOMADMSetSettings_t packSetSettingsReq = {0,0,&autosdm, &fwAutoCheck};
unpack_swioma_SLQSOMADMSetSettings_t tunpack_swioma_SLQSOMADMSetSettings = {0, SWI_UINT256_INT_VALUE};

uint8_t defautosdm     = 0;
uint8_t deffwAutoCheck = 0;
pack_swioma_SLQSOMADMSetSettings_t defpackSetSettingsReq = {0,0,&defautosdm, &deffwAutoCheck};

unpack_swioma_SLQSOMADMGetSettings_t unpackGetSettingsRsp = {0,0,0,0,0,SWI_UINT256_INT_VALUE};

uint32_t  deferTime    = 0;
uint32_t  rejectReason = 0;
pack_swioma_SLQSOMADMSendSelection_t  packSendSelectionReq ={1, &deferTime, &rejectReason};
unpack_swioma_SLQSOMADMSendSelection_t tunpack_swioma_SLQSOMADMSendSelection = {0, SWI_UINT256_INT_VALUE};

swi_uint256_t unpack_swioma_SLQSOMADMStartSessionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)
}};

swi_uint256_t unpack_swioma_SLQSOMADMStartSessionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)
}};

swi_uint256_t unpack_swioma_SLQSOMADMGetSessionInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)
}};

swi_uint256_t unpack_swioma_SLQSOMADMGetSessionInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)
}};

swi_uint256_t unpack_swioma_SLQSOMADMSendSelectionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swioma_SLQSOMADMSendSelectionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swioma_SLQSOMADMSetSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swioma_SLQSOMADMSetSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)
}};

swi_uint256_t unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)
}};

void dump_swioma_SLQSOMADMStartSession(void* ptr)
{
    unpack_swioma_SLQSOMADMStartSession_t *result =
        (unpack_swioma_SLQSOMADMStartSession_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("FW Availability : 0x%x\n", result->FwAvailability);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
            unpack_swioma_SLQSOMADMStartSessionParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_swioma_SLQSOMADMStartSessionParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);

    if (result->FwAvailability > 3) 
        is_matching = 0;

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

void dump_swioma_SLQSOMADMGetSessionInfo(void* ptr)
{
    unpack_swioma_SLQSOMADMGetSessionInfo_t *result =
        (unpack_swioma_SLQSOMADMGetSessionInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf ("Status               : %u\n",result->Status);
        printf ("UpdateCompleteStatus : %u\n",result->UpdateCompleteStatus);
        printf ("Severity             : %u\n",result->Severity);
        printf ("SourceLength         : %u\n",result->SourceLength);
        printf ("Source               : %s\n",result->Source);
        printf ("PkgNameLength        : %u\n",result->PkgNameLength);
        printf ("PkgName              : %s\n",result->PkgName);
        printf ("PkgDescLength        : %u\n",result->PkgDescLength);
        printf ("PkgDescription       : %s\n",result->PkgDescription);
        printf ("DateLength           : %u\n",result->DateLength);
        printf ("Date                 : %s\n",result->Date);
        printf ("TimeLength           : %u\n",result->TimeLength);
        printf ("Time                 : %s\n",result->Time);
        printf ("SessionType          : %u\n",result->SessionType);
        printf ("SessionState         : %u\n",result->SessionState);
        printf ("RetryCount           : %u\n",result->RetryCount);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_swioma_SLQSOMADMGetSessionInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swioma_SLQSOMADMGetSessionInfoParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((0xFF != result->Status) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((0xFF != result->Status) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_swioma_SLQSOMADMSetSettings(void* ptr)
{
    unpack_swioma_SLQSOMADMSetSettings_t *result =
        (unpack_swioma_SLQSOMADMSetSettings_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_swioma_SLQSOMADMSetSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swioma_SLQSOMADMSetSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

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
}

void dump_swioma_SLQSOMADMGetSettingsStore(void* ptr)
{
    unpack_swioma_SLQSOMADMGetSettings_t *result =
        (unpack_swioma_SLQSOMADMGetSettings_t*) ptr;

    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("  OMADMEnabled: %u\n", result->OMADMEnabled);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("  FOTAdownload: %u\n", result->FOTAdownload);
        defpackSetSettingsReq.FOTAdownload = result->FOTAdownload;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("  FOTAUpdate  : %u\n", result->FOTAUpdate);
        defpackSetSettingsReq.FOTAUpdate = result->FOTAUpdate;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19) && (defpackSetSettingsReq.pAutosdm != NULL))
    {
        printf("  Autosdm     : %u\n", result->Autosdm);
        defpackSetSettingsReq.pAutosdm = &result->Autosdm;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20) && (defpackSetSettingsReq.pFwAutoCheck != NULL))
    {
        printf("  FwAutoCheck : %u\n", result->FwAutoCheck);
        defpackSetSettingsReq.pFwAutoCheck = &result->FwAutoCheck;
    }

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

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

void dump_swioma_SLQSOMADMGetSettingsVerify(void* ptr)
{
    unpack_swioma_SLQSOMADMGetSettings_t *result =
        (unpack_swioma_SLQSOMADMGetSettings_t*) ptr;

    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("  OMADMEnabled: %u\n", result->OMADMEnabled);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("  FOTAdownload: %u\n", result->FOTAdownload);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("  FOTAUpdate  : %u\n", result->FOTAUpdate);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        printf("  Autosdm     : %u\n", result->Autosdm);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        printf("  FwAutoCheck : %u\n", result->FwAutoCheck);
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        if(packSetSettingsReq.FOTAdownload != result->FOTAdownload)
            is_matching = 0;

    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        if(packSetSettingsReq.FOTAUpdate != result->FOTAUpdate)
            is_matching = 0;

    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        if(*packSetSettingsReq.pAutosdm != result->Autosdm)
            is_matching = 0;

    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        if(*packSetSettingsReq.pFwAutoCheck != result->FwAutoCheck)
            is_matching = 0;

    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_swioma_SLQSOMADMGetSettings(void* ptr)
{
    unpack_swioma_SLQSOMADMGetSettings_t *result =
        (unpack_swioma_SLQSOMADMGetSettings_t*) ptr;

    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("  OMADMEnabled: %u\n", result->OMADMEnabled);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("  FOTAdownload: %u\n", result->FOTAdownload);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("  FOTAUpdate  : %u\n", result->FOTAUpdate);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        printf("  Autosdm     : %u\n", result->Autosdm);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        printf("  FwAutoCheck : %u\n", result->FwAutoCheck);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swioma_SLQSOMADMGetSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
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

void dump_swioma_SLQSOMADMSendSelection(void* ptr)
{
    unpack_swioma_SLQSOMADMSendSelection_t *result =
        (unpack_swioma_SLQSOMADMSendSelection_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_swioma_SLQSOMADMSendSelectionParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_swioma_SLQSOMADMSendSelectionParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);

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

void swioma_loop_exit(void)
{
    printf("\nkilling SWIOMA read thread...\n");
    enSwiOmaThread = 0;
#ifdef __ARM_EABI__
    if(swioma_fd>=0)
        close(swioma_fd);
    swioma_fd=-1;
    void *pthread_rtn_value;
    if(swioma_tid!=0)
    pthread_join(swioma_tid, &pthread_rtn_value);
#endif
    if(swioma_tid!=0)
    pthread_cancel(swioma_tid);
    swioma_tid = 0;
     if(swioma_fd>=0)
        close(swioma_fd);
    swioma_fd=-1;
    return;
}

void _print_swioma_event(unpack_swioma_SLQSOMADMAlertCallback_ind_t SwiOmaAlert)
{
    printf("SwiOma Alert Type: %u\n", SwiOmaAlert.eventType);
    swi_uint256_print_mask (SwiOmaAlert.ParamPresenceMask);
    switch(SwiOmaAlert.eventType)
    {
    case 0x00: //SWIOMA-DM FOTA 
        if(swi_uint256_get_bit (SwiOmaAlert.ParamPresenceMask, 16))
        {
            printf ("SWIOMA-DM FOTA\n");
            printf ("==============\n");
            printf ("   state                : %u\n",SwiOmaAlert.SessionInfoFota.state);
            printf ("   userInputReq         : %u\n",SwiOmaAlert.SessionInfoFota.userInputReq);
            printf ("   userInputTimeout     : %u\n",SwiOmaAlert.SessionInfoFota.userInputTimeout);
            printf ("   fwdloadsize          : %u\n",SwiOmaAlert.SessionInfoFota.fwdloadsize);
            printf ("   fwloadComplete       : %u\n",SwiOmaAlert.SessionInfoFota.fwloadComplete);
            printf ("   updateCompleteStatus : %u\n",SwiOmaAlert.SessionInfoFota.updateCompleteStatus);
            printf ("   severity             : %u\n",SwiOmaAlert.SessionInfoFota.severity);
            printf ("   version              : %s\n",SwiOmaAlert.SessionInfoFota.version);
            printf ("   package_name         : %s\n",SwiOmaAlert.SessionInfoFota.package_name);
            printf ("   description          : %s\n",SwiOmaAlert.SessionInfoFota.description);
            printf ("   sessionType          : %u\n",SwiOmaAlert.SessionInfoFota.sessionType);
        }
        break;
    case 0x01: //SWIOMA-DM Config 
        if(swi_uint256_get_bit (SwiOmaAlert.ParamPresenceMask, 17))
        {
            printf ("SWIOMA-DM Config\n");
            printf ("================\n");
            printf ("   state            : %u\n",SwiOmaAlert.SessionInfoConfig.state);
            printf ("   userInputReq     : %u\n",SwiOmaAlert.SessionInfoConfig.userInputReq);
            printf ("   userInputTimeout : %u\n",SwiOmaAlert.SessionInfoConfig.userInputTimeout);
            printf ("   alertmsg         : %s\n",SwiOmaAlert.SessionInfoConfig.alertmsg);
        }
        break;
    case 0x02: //SWIOMA-DM Notification 
        if(swi_uint256_get_bit (SwiOmaAlert.ParamPresenceMask, 18))
        {
            printf ("SWIOMA-DM Notification\n");
            printf ("======================\n");
            printf ("   notification  : %u\n",SwiOmaAlert.SessionInfoNotification.notification);
            printf ("   sessionStatus : %u\n",SwiOmaAlert.SessionInfoNotification.sessionStatus);
        }
        break;        
    }
}

void _print_swioma_session_info(unpack_swioma_SLQSOMADMGetSessionInfo_t  SwiOmaSessionInfo)
{
    printf ("Status               : %u\n",SwiOmaSessionInfo.Status);
    printf ("UpdateCompleteStatus : %u\n",SwiOmaSessionInfo.UpdateCompleteStatus);
    printf ("Severity             : %u\n",SwiOmaSessionInfo.Severity);
    printf ("SourceLength         : %u\n",SwiOmaSessionInfo.SourceLength);
    printf ("Source               : %s\n",SwiOmaSessionInfo.Source);
    printf ("PkgNameLength        : %u\n",SwiOmaSessionInfo.PkgNameLength);
    printf ("PkgName              : %s\n",SwiOmaSessionInfo.PkgName);
    printf ("PkgDescLength        : %u\n",SwiOmaSessionInfo.PkgDescLength);
    printf ("PkgDescription       : %s\n",SwiOmaSessionInfo.PkgDescription);
    printf ("DateLength           : %u\n",SwiOmaSessionInfo.DateLength);
    printf ("Date                 : %s\n",SwiOmaSessionInfo.Date);
    printf ("TimeLength           : %u\n",SwiOmaSessionInfo.TimeLength);
    printf ("Time                 : %s\n",SwiOmaSessionInfo.Time);
    printf ("SessionType          : %u\n",SwiOmaSessionInfo.SessionType);
    printf ("SessionState         : %u\n",SwiOmaSessionInfo.SessionState);
    printf ("RetryCount           : %u\n",SwiOmaSessionInfo.RetryCount);
}

testitem_t oemapitestswioma[] = {
    {
        (pack_func_item) &pack_swioma_SLQSOMADMStartSession, "pack_swioma_SLQSOMADMStartSession",
        &packStartSessionReq,
        (unpack_func_item) &unpack_swioma_SLQSOMADMStartSession, "unpack_swioma_SLQSOMADMStartSession",
        &unpackStartSessionRsp, dump_swioma_SLQSOMADMStartSession
    },
    {
        (pack_func_item) &pack_swioma_SLQSOMADMGetSessionInfo, "pack_swioma_SLQSOMADMGetSessionInfo",
        &packGetSessionInfoReq,
        (unpack_func_item) &unpack_swioma_SLQSOMADMGetSessionInfo, "unpack_swioma_SLQSOMADMGetSessionInfo",
        &unpackGetSessionInfoReq, dump_swioma_SLQSOMADMGetSessionInfo
    },
    {
        (pack_func_item) &pack_swioma_SLQSOMADMGetSettings, "pack_swioma_SLQSOMADMGetSettings Modem",
        NULL,
        (unpack_func_item) &unpack_swioma_SLQSOMADMGetSettings, "unpack_swioma_SLQSOMADMGetSettings Modem",
        &unpackGetSettingsRsp, dump_swioma_SLQSOMADMGetSettingsStore
    },
    {
        (pack_func_item) &pack_swioma_SLQSOMADMSetSettings, "pack_swioma_SLQSOMADMSetSettings Change",
        &packSetSettingsReq,
        (unpack_func_item) &unpack_swioma_SLQSOMADMSetSettings, "unpack_swioma_SLQSOMADMSetSettings Change",
        &tunpack_swioma_SLQSOMADMSetSettings, dump_swioma_SLQSOMADMSetSettings
    },    
    {
        (pack_func_item) &pack_swioma_SLQSOMADMGetSettings, "pack_swioma_SLQSOMADMGetSettings Result",
        NULL,
        (unpack_func_item) &unpack_swioma_SLQSOMADMGetSettings, "unpack_swioma_SLQSOMADMGetSettings Result",
        &unpackGetSettingsRsp, dump_swioma_SLQSOMADMGetSettingsVerify
    }, 
    {
        (pack_func_item) &pack_swioma_SLQSOMADMSetSettings, "pack_swioma_SLQSOMADMSetSettings Restore",
        &defpackSetSettingsReq,
        (unpack_func_item) &unpack_swioma_SLQSOMADMSetSettings, "unpack_swioma_SLQSOMADMSetSettings Restore",
        &tunpack_swioma_SLQSOMADMSetSettings, dump_swioma_SLQSOMADMSetSettings
    },
    {
        (pack_func_item) &pack_swioma_SLQSOMADMSendSelection, "pack_swioma_SLQSOMADMSendSelection",
        &packSendSelectionReq,
        (unpack_func_item) &unpack_swioma_SLQSOMADMSendSelection, "unpack_swioma_SLQSOMADMSendSelection",
        &tunpack_swioma_SLQSOMADMSendSelection, dump_swioma_SLQSOMADMSendSelection
    }
};

// run dump functions with NULL argument
testitem_t swiomadumptest[] =
{
    {
        NULL, "",
        NULL,
        NULL, "run dump_swioma_SLQSOMADMStartSession",
        NULL, 
        dump_swioma_SLQSOMADMStartSession
    },
    {
        NULL, "",
        NULL,
        NULL, "run dump_swioma_SLQSOMADMGetSessionInfo",
        NULL, 
        dump_swioma_SLQSOMADMGetSessionInfo
    },
    {
        NULL, "",
        NULL,
        NULL, "run dump_swioma_SLQSOMADMSetSettings",
        NULL, 
        dump_swioma_SLQSOMADMSetSettings
    },
    {
        NULL, "",
        NULL,
        NULL, "run dump_swioma_SLQSOMADMGetSettings",
        NULL, 
        dump_swioma_SLQSOMADMGetSettings
    },
    {
        NULL, "",
        NULL,
        NULL, "run dump_swioma_SLQSOMADMSendSelection",
        NULL, 
        dump_swioma_SLQSOMADMSendSelection
    },
};

void *swioma_read_thread(void* ptr)
{
    UNUSEDPARAM(ptr);
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  buffer[QMI_SWIOMA_MSG_MAX]={'0'};
    int rtn;
    ssize_t rlen;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    printf ("SWIOMA read thread\n");
    sleep(1);
    while(enSwiOmaThread)
    {
        //TODO select multiple file and read them
        memset (buffer,0,QMI_SWIOMA_MSG_MAX);
        rtn = rlen = read(swioma_fd, buffer, QMI_SWIOMA_MSG_MAX);
        if ((rtn > 0) && (rlen > 0))
        {
            qmi_msg = helper_get_resp_ctx(eSWIOMA, buffer, rlen, &rsp_ctx);
            printf("<< receiving %s, Len: %zu\n", qmi_msg, rlen);
            dump_hex(rlen, buffer);
            if(g_runoem_demo == 1)
            {
                if (rsp_ctx.type == eIND)
                {
                    printf("SWIOMA IND: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                }
                else
                {
                    printf("SWIOMA RSP: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    if (Sendxid == rsp_ctx.xid)
                    {
                        unpackRetCode = run_unpack_item(oemapitestswioma[g_oemApiIndex_swioma].unpack)(buffer, rlen, oemapitestswioma[g_oemApiIndex_swioma].unpack_ptr);
                        #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,",oemapitestswioma[g_oemApiIndex_swioma].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                        if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                        {
                         eRCode = helper_get_error_code(buffer);
                         reason = helper_get_error_reason(eRCode);
                         sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                        }
                        #endif
                        oemapitestswioma[g_oemApiIndex_swioma].dump(oemapitestswioma[g_oemApiIndex_swioma].unpack_ptr);
                    }
                 }
                if ((rsp_ctx.type == eRSP)&&(Sendxid==rsp_ctx.xid))
                {
                    ReadyToWriteNext = 1;//Ready to write Next message
                }
                continue;
            }
            if (eIND == rsp_ctx.type)
                printf("SWIOMA IND: ");
            else if (eRSP == rsp_ctx.type)
            {
                printf("SWIOMA RSP: ");
                if (Sendxid==rsp_ctx.xid)       
                    ReadyToWriteNext = 1;//Ready to write Next message
            }
            printf("msgid 0x%02x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                /** eQMI_SWIOMA_EVENT_IND **/
                case eQMI_SWIOMA_EVENT_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_swioma_SLQSOMADMAlertCallback_ind_t SwiOmaInd;
                        memset (&SwiOmaInd,0,sizeof(SwiOmaInd));
                        unpackRetCode = unpack_swioma_SLQSOMADMAlertCallback_ind (buffer, rlen, &SwiOmaInd);
                        printf("unpack_swioma_SLQSOMADMAlertCallback_ind return: %d\n", unpackRetCode);

                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMAlertCallback_ind",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        local_fprintf("%s\n", "Correct");
                #endif                        
                        _print_swioma_event(SwiOmaInd);
                    }
                    else if (eRSP == rsp_ctx.type)
                    {
                        unpack_swioma_SLQSOMADMAlertCallback_t omadmCallbackResp = {0 ,SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMAlertCallback (buffer, rlen, &omadmCallbackResp);
                        printf("unpack_swioma_SLQSOMADMAlertCallback return: %d\n", unpackRetCode);
                    }                    

                #if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMAlertCallback",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                    checkErrorAndFormErrorString(buffer);
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

                    break; //eQMI_SWIOMA_EVENT_IND
                
                /** eQMI_SWIOMA_SET_EVENT **/
                case eQMI_SWIOMA_SET_EVENT:
                    {
                        unpack_swioma_SLQSOMADMAlertCallback_t omadmAlertCallbackResp = {0, SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMAlertCallback (buffer, rlen, &omadmAlertCallbackResp);
                        printf("unpack_swioma_SLQSOMADMAlertCallback return: %d\n", unpackRetCode);
                    } 

                #if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMAlertCallback",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                    checkErrorAndFormErrorString(buffer);
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


                    break;//eQMI_SWIOMA_SET_EVENT
                    
                /** eQMI_SWIOMA_START_SESSION **/
                case eQMI_SWIOMA_START_SESSION:
                    {
                        unpack_swioma_SLQSOMADMStartSession_t SwiOmaRsp={0, SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMStartSession (buffer, rlen, &SwiOmaRsp);
                        swi_uint256_print_mask (SwiOmaRsp.ParamPresenceMask);
                        if(swi_uint256_get_bit (SwiOmaRsp.ParamPresenceMask, 2))
                        {
                            printf("unpack_swioma_SLQSOMADMStartSession return: %d\n", unpackRetCode);
                        }
                        if(swi_uint256_get_bit (SwiOmaRsp.ParamPresenceMask, 16))
                        {
                            printf("\tFW Available: %u\n", SwiOmaRsp.FwAvailability);
                        }

                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMStartSession",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                #endif
                        dump_swioma_SLQSOMADMStartSession((void*)&SwiOmaRsp);

                    }
                    break; //eQMI_SWIOMA_START_SESSION
                
                /** eQMI_SWIOMA_CANCEL_SESSION **/
                case eQMI_SWIOMA_CANCEL_SESSION:
                    {
                        unpack_swioma_SLQSOMADMCancelSession_t cancelSessionResp = {0, SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMCancelSession (buffer, rlen, &cancelSessionResp);
                        swi_uint256_print_mask (cancelSessionResp.ParamPresenceMask);
                        if(swi_uint256_get_bit (cancelSessionResp.ParamPresenceMask, 2))
                        {
                            printf("unpack_swioma_SLQSOMADMCancelSession return: %d\n", unpackRetCode);
                        }
                    }

                #if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMCancelSession",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                    checkErrorAndFormErrorString(buffer);
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

                    break; //eQMI_SWIOMA_CANCEL_SESSION
                    
                /** eQMI_SWIOMA_SET_SETTINGS **/
                case eQMI_SWIOMA_SET_SETTINGS:
                    {
                        unpack_swioma_SLQSOMADMSetSettings_t omadmSetSettingsResp = {0,SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMSetSettings (buffer, rlen, &omadmSetSettingsResp);
                        printf("unpack_swioma_SLQSOMADMSetSettings return: %d\n", unpackRetCode);
                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMSetSettings",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                #endif
                        dump_swioma_SLQSOMADMSetSettings((void*)&omadmSetSettingsResp);
                    }
                    break; //eQMI_SWIOMA_SET_SETTINGS
                
                /** eQMI_SWIOMA_GET_SETTINGS **/
                case eQMI_SWIOMA_GET_SETTINGS:
                    {
                        unpack_swioma_SLQSOMADMGetSettings_t SwiOmaRsp={0,0,0,0,0,SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMGetSettings (buffer, rlen, &SwiOmaRsp);
                        printf("unpack_swioma_SLQSOMADMGetSettings return: %d\n", unpackRetCode);
                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMGetSettings",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                #endif
                        dump_swioma_SLQSOMADMGetSettings((void*)&SwiOmaRsp);                              

                        if(iGetOMADMSettings==1)
                        {
                            DefaultOMADMSetSettings.FOTAdownload = SwiOmaRsp.FOTAdownload;
                            DefaultOMADMSetSettings.FOTAUpdate = SwiOmaRsp.FOTAUpdate;
                            *(DefaultOMADMSetSettings.pAutosdm) = SwiOmaRsp.Autosdm;
                            *(DefaultOMADMSetSettings.pFwAutoCheck) = SwiOmaRsp.FwAutoCheck;
                        }                        
                    }
                    break; //eQMI_SWIOMA_GET_SETTINGS
                    
                /** eQMI_SWIOMA_SEND_SELECTION **/
                case eQMI_SWIOMA_SEND_SELECTION:
                    {
                        unpack_swioma_SLQSOMADMSendSelection_t OMADMSendSelResp = {0, SWI_UINT256_INT_VALUE};
                        unpackRetCode = unpack_swioma_SLQSOMADMSendSelection (buffer, rlen, &OMADMSendSelResp);
                        printf("unpack_swioma_SLQSOMADMSendSelection return: %d\n", unpackRetCode);

                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMSendSelection",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                #endif
                        dump_swioma_SLQSOMADMSendSelection((void*)&OMADMSendSelResp);
                    }
                    break; //eQMI_SWIOMA_SEND_SELECTION
                    
                /** eQMI_SWIOMA_GET_SESSION_INFO **/
                case eQMI_SWIOMA_GET_SESSION_INFO:
                    {
                        unpack_swioma_SLQSOMADMGetSessionInfo_t SwiOmaRsp;
                        memset (&SwiOmaRsp,0,sizeof(unpack_swioma_SLQSOMADMGetSessionInfo_t));
                        SwiOmaRsp.SourceLength  = LITEQMI_MAX_SWIOMA_STR_LEN;
                        SwiOmaRsp.PkgNameLength = LITEQMI_MAX_SWIOMA_STR_LEN;
                        SwiOmaRsp.PkgDescLength = LITEQMI_MAX_SWIOMA_STR_LEN;
                        SwiOmaRsp.DateLength    = LITEQMI_MAX_SWIOMA_STR_LEN;
                        SwiOmaRsp.TimeLength    = LITEQMI_MAX_SWIOMA_STR_LEN;
                        
                        unpackRetCode = unpack_swioma_SLQSOMADMGetSessionInfo (buffer, rlen, &SwiOmaRsp);
                        printf("unpack_swioma_SLQSOMADMGetSessionInfo return: %d\n", unpackRetCode);

                #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swioma_SLQSOMADMGetSessionInfo",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                #endif
                        dump_swioma_SLQSOMADMGetSessionInfo((void*)&SwiOmaRsp);

                    }
                    break; //eQMI_SWIOMA_GET_SESSION_INFO
                    
                default:
                    break;
            }
        }
        else
        {
            enSwiOmaThread = 0;
            swioma_fd = -1;
            printf("enSwiOmaThread exit read error: %d\n",rtn);fflush(stdout);
            break;
        }
    }    
    return NULL;
}

void swioma_pack_unpack_loop()
{
    unsigned i;
    printf("======SWIOMA pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "SWIOMA Pack/UnPack API Name", 
                      "Status",
                      "Unpack Payload Parsing",
                      "Remark");
#endif
    
    unsigned xid =1;
    enSwiOmaThread = 1;
    if(swioma_fd<0)
        swioma_fd = client_fd(eSWIOMA);
    if(swioma_fd<0)
    {
        fprintf(stderr,"SWIOMA Service Not Supported!\n");
        return ;
    }

    memset(&swioma_attr, 0, sizeof(swioma_attr));
    pthread_create(&swioma_tid, &swioma_attr, swioma_read_thread, NULL);

    sleep(1);

    for(i=0; i<sizeof(oemapitestswioma)/sizeof(testitem_t); i++)
    {
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t req[QMI_MSG_MAX];
        uint16_t reqLen;
        const char *qmi_msg;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        reqLen = sizeof(req);

        if (strstr(oemapitestswioma[i].pack_func_name,"pack_swioma_SLQSOMADMStartSession") != NULL)
        {
            rtn = pack_swioma_SLQSOMADMStartSession(&req_ctx, req, &reqLen,
                      *(pack_swioma_SLQSOMADMStartSession_t *)oemapitestswioma[i].pack_ptr);
        }
        else if (strstr(oemapitestswioma[i].pack_func_name,"pack_swioma_SLQSOMADMGetSessionInfo") != NULL)
        {
            rtn = pack_swioma_SLQSOMADMGetSessionInfo(&req_ctx, req, &reqLen,
                      *(pack_swioma_SLQSOMADMGetSessionInfo_t *)oemapitestswioma[i].pack_ptr);
        }
        else if (strstr(oemapitestswioma[i].pack_func_name,"pack_swioma_SLQSOMADMSetSettings") != NULL)
        {
            rtn = pack_swioma_SLQSOMADMSetSettings(&req_ctx, req, &reqLen,
                      *(pack_swioma_SLQSOMADMSetSettings_t *)oemapitestswioma[i].pack_ptr);
        }
        else if (strstr(oemapitestswioma[i].pack_func_name,"pack_swioma_SLQSOMADMSendSelection") != NULL)
        {
            rtn = pack_swioma_SLQSOMADMSendSelection(&req_ctx, req, &reqLen,
                      *(pack_swioma_SLQSOMADMSendSelection_t *)oemapitestswioma[i].pack_ptr);
        }
        else
        {
            rtn = run_pack_item(oemapitestswioma[i].pack)(&req_ctx, req, &reqLen, oemapitestswioma[i].pack_ptr);
        }
        g_oemApiIndex_swioma = i;

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%-60s,%-20s,%-s\n", oemapitestswioma[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        rtn = write(swioma_fd, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSWIOMA, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        Sendxid = xid; 
        rtn = 0;
        ReadyToWriteNext = 0;
        do
        {
            if(ReadyToWriteNext)
                break;
            sleep(1);
        }while(rtn++<120);//120 Seconds timeout
        xid++;
    }
    sleep(5);
}

void swioma_loop(void)
{
    uint8_t  buffer[QMI_SWIOMA_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_SWIOMA_MSG_MAX;
    pack_qmi_t req_ctx;
    uint16_t   txID = 0x101;
    int rtn = -1;
    const char *qmi_msg;

    printf("======swioma dummy unpack test===========\n");
    iLocalLog = 0;
    swioma_validate_dummy_unpack();
    iLocalLog = 1;

    unsigned i = 0;
    for(i=0; i < sizeof(swiomadumptest)/sizeof(testitem_t); i++)
    {
        fprintf(stdout, "%s\n",swiomadumptest[i].unpack_func_name);
        swiomadumptest[i].dump(NULL);
    }
    // Get SWIOMA FD
    swioma_fd = client_fd(eSWIOMA);
    if(swioma_fd<0)
    {
        fprintf(stderr,"SWI OMA Service Not Supported!\n");
        return ;
    }
    sleep(1);
    // Start SWIOMA read thread
    memset(&swioma_attr, 0, sizeof(swioma_attr));
    enSwiOmaThread = 1;
    pthread_create(&swioma_tid, &swioma_attr, swioma_read_thread, NULL);

    usleep(500);

    printf("======SWIOMA pack/unpack test===========\n");
   
#if DEBUG_LOG_TO_FILE
       mkdir("./TestResults/",0777);
       local_fprintf("\n");
       local_fprintf("%s,%s,%s,%s\n", "SWIOMA Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

   // Get Modem Settings///
   // OMA Get Settings
   {
        iGetOMADMSettings = 1;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMGetSettings(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swioma_SLQSOMADMGetSettings ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMGetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }    
    sleep(5);

    iGetOMADMSettings = 0;
    /////////////////////////////////////////////
    // Register for SWIOMA event notification
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        rtn = pack_swioma_SLQSOMADMAlertCallback (&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swioma_SLQSOMADMAlertCallback ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMAlertCallback", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }

    sleep(5);
    // Start CI-PRL Session
    {
        pack_swioma_SLQSOMADMStartSession_t swioma_req;
        swioma_req.sessionType = 0x03; //CI PRL
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swioma_SLQSOMADMStartSession(&req_ctx,buffer,&reqLen,swioma_req);
        fprintf (stderr, "pack_swioma_SLQSOMADMStartSession (CI-PRL) ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMStartSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }

    sleep(5);
    // OMA Get Session Info
    {
        pack_swioma_SLQSOMADMGetSessionInfo_t swioma_req;
        swioma_req.SessionType     = 0xFF; // Any active OMADM session
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMGetSessionInfo(&req_ctx,buffer,&reqLen,swioma_req);
        fprintf (stderr, "pack_swioma_SLQSOMADMGetSessionInfo ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMGetSessionInfo", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }        

    sleep(5);
    // Cancel CI-PRL Session
    {
        pack_swioma_SLQSOMADMCancelSession_t swioma_req;
        swioma_req.sessionType = 0xFF; //Any OMA session
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swioma_SLQSOMADMCancelSession(&req_ctx,buffer,&reqLen,swioma_req);
        fprintf (stderr, "pack_swioma_SLQSOMADMCancelSession (Any OMA) ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMCancelSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }

    sleep(7);
    // OMA Set Settings
    {
        uint8_t Autosdm     = 0;
        uint8_t FwAutoCheck = 0;
        pack_swioma_SLQSOMADMSetSettings_t swioma_req;
        swioma_req.FOTAdownload = 0;
        swioma_req.FOTAUpdate   = 0;
        swioma_req.pAutosdm     = &Autosdm;
        swioma_req.pFwAutoCheck = &FwAutoCheck;
        
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMSetSettings(&req_ctx,buffer,&reqLen,swioma_req);
        fprintf (stderr, "pack_swioma_SLQSOMADMSetSettings ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMSetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }    
    
    sleep(5);
    // OMA Get Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMGetSettings(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swioma_SLQSOMADMGetSettings ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMGetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }

    sleep(5);
    // OMA Send Selection
    {
        uint32_t  DeferTime    = 0;
        uint32_t  RejectReason = 1; // Just some value
        pack_swioma_SLQSOMADMSendSelection_t  swioma_req;
        swioma_req.selection     = 0x01; // Accept
        swioma_req.pDeferTime    = &DeferTime;
        swioma_req.pRejectReason = &RejectReason;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMSendSelection(&req_ctx,buffer,&reqLen,swioma_req);
        fprintf (stderr, "pack_swioma_SLQSOMADMSendSelection ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMSendSelection", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }

    sleep(5);
    //Restore Modem Settings
    // restore OMA Settings
    {  
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMSetSettings(&req_ctx,buffer,&reqLen,DefaultOMADMSetSettings);
        fprintf (stderr, "pack_swioma_SLQSOMADMSetSettings ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMSetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swioma_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMA, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        else
        {
            return ;
        }
    }
    
    return;
}

void dump_SLQSOMADMAlertCallback_ind(void *ptr)
{
    unpack_swioma_SLQSOMADMAlertCallback_ind_t *result =
            (unpack_swioma_SLQSOMADMAlertCallback_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    _print_swioma_event(*result);
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_swioma_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_SWIOMA_START_SESSION */
    {0x02,0x01,0x00,0x01,0x00,0x0E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_SWIOMA_CANCEL_SESSION */
    {0x02,0x02,0x00,0x02,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWIOMA_GET_SETTINGS */
    {0x02,0x03,0x00,0x06,0x00,0x1E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x04,0x00,0x01,0x00,0x00,0x00,
            0x11,0x01,0x00,0x01,
            0x12,0x01,0x00,0x00,
            0x13,0x01,0x00,0x01,
            0x14,0x01,0x00,0x01},

    /* eQMI_SWIOMA_SET_SETTINGS */
    {0x02,0x04,0x00,0x07,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWIOMA_SEND_SELECTION */
    {0x02,0x05,0x00,0x05,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWIOMA_GET_SESSION_INFO */
    {0x02,0x06,0x00,0x03,0x00,0x2F,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x25,0x00,0x07,0xF7,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x14,0x00,0x30,0x00,0x36,0x00,0x2F,0x00,0x30,0x00,0x31,0x00,
                        0x2F,0x00,0x32,0x00,0x30,0x00,0x31,0x00,0x35,0x00,0x00,0x00,
                        0x01,0x01,0x00},

    /* eQMI_SWIOMA_SET_EVENT */
    {0x02,0x07,0x00,0x08,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWIOMA_EVENT_IND */
    {0x04,0x08,0x00,0x04,0x00,0x76,0x00,
            0x10,0x73,0x00,0x03,0x00,0x00,0x00,0x44,0x33,0x22,0x11,0x00,0x33,0x22,0x11,0xc8,0x00,
                    0x01,0x14,0x00,'S','W','I','9','6','0','0','M','_','0','1','.','0','0','.','0','0','.','0','1',
                    0x24,0x00,0x46,0x00,0x57,0x00,0x20,0x00,0x55,0x00,0x70,0x00,0x67,0x00,0x72,0x00,
                    0x61,0x00,0x64,0x00,0x65,0x00,0x20,0x00,0x50,0x00,0x61,0x00,0x63,0x00,0x6B,0x00,
                    0x61,0x00,0x67,0x00,0x65,0x00,//"FW Upgrade Package",
                    0x26,0x00,0x55,0x00,0x70,0x00,0x67,0x00,0x72,0x00,0x61,0x00,0x64,0x00,0x65,0x00,
                    0x20,0x00,0x66,0x00,0x72,0x00,0x6F,0x00,0x6D,0x00,0x20,0x00,0x31,0x00,0x20,0x00,
                    0x74,0x00,0x6F,0x00,0x20,0x00,0x32,0x00,//"Upgrade from 1 to 2",0x00,
                    },

};

    /* eQMI_SWIOMA_START_SESSION */
const unpack_swioma_SLQSOMADMStartSession_t const_unpack_swioma_SLQSOMADMStartSession_t = {
        0x01,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

    /* eQMI_SWIOMA_CANCEL_SESSION */
const unpack_swioma_SLQSOMADMCancelSession_t const_unpack_swioma_SLQSOMADMCancelSession_t = {
 0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_SWIOMA_GET_SETTINGS */
const unpack_swioma_SLQSOMADMGetSettings_t const_unpack_swioma_SLQSOMADMGetSettings_t = {
0x01,0x01,0x00,0x01,0x01,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

    /* eQMI_SWIOMA_SET_SETTINGS */
const unpack_swioma_SLQSOMADMSetSettings_t const_unpack_swioma_SLQSOMADMSetSettings_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_SWIOMA_SEND_SELECTION */
const unpack_swioma_SLQSOMADMSendSelection_t const_unpack_swioma_SLQSOMADMSendSelection_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_SWIOMA_GET_SESSION_INFO */
const unpack_swioma_SLQSOMADMGetSessionInfo_t const_unpack_swioma_SLQSOMADMGetSessionInfo_t = {
0x07,0x01F7,0x02,0x0000,{0},0x0000,{0},0x0000,{0},
0x14,{0x30,0x00,0x36,0x00,0x2F,0x00,0x30,0x00,0x31,0x00,0x2F,0x00,0x32,0x00,0x30,0x00,0x31,0x00,0x35,0x00},
0x00,{0},0x01,0x01,0x00,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_SWIOMA_SET_EVENT */
const unpack_swioma_SLQSOMADMAlertCallback_t const_unpack_swioma_SLQSOMADMAlertCallback_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIOMA_EVENT_IND */
const unpack_swioma_SLQSOMADMAlertCallback_ind_t const_unpack_swioma_SLQSOMADMAlertCallback_ind_t = {
0x00,{0x03,0x00,0x0000,0x11223344,0x11223300,0x00C8,0x01,0x0014,{"SWI9600M_01.00.00.01"},
0x0024,{0x46,0x00,0x57,0x00,0x20,0x00,0x55,0x00,0x70,0x00,0x67,0x00,0x72,0x00,
        0x61,0x00,0x64,0x00,0x65,0x00,0x20,0x00,0x50,0x00,0x61,0x00,0x63,0x00,0x6B,0x00,
        0x61,0x00,0x67,0x00,0x65,0x00},0x0026,{0x55,0x00,0x70,0x00,0x67,0x00,0x72,0x00,0x61,0x00,0x64,0x00,0x65,0x00,
                0x20,0x00,0x66,0x00,0x72,0x00,0x6F,0x00,0x6D,0x00,0x20,0x00,0x31,0x00,0x20,0x00,
                0x74,0x00,0x6F,0x00,0x20,0x00,0x32,0x00},0x00},
                {0,0,0,0,{0}},
                {0,0},
                {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

int swioma_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swioma_resp_msg)/sizeof(validate_swioma_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swioma_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSWIOMA, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWIOMA IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIOMA RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_SWIOMA_START_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMStartSession,
                    dump_swioma_SLQSOMADMStartSession,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMStartSession_t);
                }
                break;
            case eQMI_SWIOMA_CANCEL_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMCancelSession,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMCancelSession_t);
                }
                break;
            case eQMI_SWIOMA_GET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMGetSettings,
                    dump_swioma_SLQSOMADMGetSettings,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMGetSettings_t);
                }
                break;
            case eQMI_SWIOMA_SET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMSetSettings,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMSetSettings_t);
                }
                break;
            case eQMI_SWIOMA_SEND_SELECTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMSendSelection,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMSendSelection_t);
                }
                break;
            case eQMI_SWIOMA_GET_SESSION_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMGetSessionInfo,
                    dump_swioma_SLQSOMADMGetSessionInfo,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMGetSessionInfo_t);
                }
                break;
            case eQMI_SWIOMA_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMAlertCallback,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMAlertCallback_t);
                }
                break;
            case eQMI_SWIOMA_EVENT_IND:
                if (eIND== rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMAlertCallback_ind,
                    dump_SLQSOMADMAlertCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMAlertCallback_ind_t);
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

