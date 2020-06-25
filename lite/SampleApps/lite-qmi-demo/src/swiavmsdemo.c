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
#include "swiavms.h"
#include "packingdemo.h"

#define QMI_SWIAVMS_MSG_MAX QMI_MSG_MAX

// FUNCTION PROTOTYPES
int client_fd(uint8_t svc);
int swiavms_validate_dummy_unpack();

// swiavms GLOBALS
volatile int enswiavmsThread;
int swiavms_fd;
pthread_t swiavms_tid = 0;
pthread_attr_t swiavms_attr;
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;

// TEST VALUES
#ifdef SWIAVMS_EVENTS_DEBUG
uint8_t swiavms_event_sample[] = {};
#endif //SWIAVMS_EVENTS_DEBUG

////Modem Settings
unpack_swiavms_SLQSAVMSGetSettings_t DefaultAVMSGetSettings;
pack_swiavms_SLQSAVMSSetSettings_t DefaultAVMSSetSettings;


/* SLQSAVMSStartSession */
swi_uint256_t unpack_swiavms_SLQSAVMSStartSessionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_swiavms_SLQSAVMSStartSessionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSAVMSStopSession */
swi_uint256_t unpack_swiavms_SLQSAVMSStopSessionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_swiavms_SLQSAVMSStopSessionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSAvmsSetEventReport */
swi_uint256_t unpack_swiavms_SLQSAvmsSetEventReportParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swiavms_SLQSAvmsSetEventReportParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSAVMSEventReportInd */
swi_uint256_t unpack_swiavms_SLQSAVMSEventReportIndParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_10_BITS,16,17,18,19,20,21,22,23,24,25)
}};

/* SLQSAVMSSendSelection */
swi_uint256_t unpack_swiavms_SLQSAVMSSendSelectionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swiavms_SLQSAVMSSendSelectionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSAVMSSetSettings */
swi_uint256_t unpack_swiavms_SLQSAVMSSetSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_swiavms_SLQSAVMSSetSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSAVMSGetSettings */
swi_uint256_t unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_12_BITS,1,2,3,4,5,6,16,17,18,19,20,21)
}};

swi_uint256_t unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,1,2,3,4,5,6)
}};

unpack_swiavms_SLQSAVMSGetSettings_t tunpack_swiavms_SLQSAVMSGetSettingsEnableResult =
{
    1,1,1,1,1,1,NULL,NULL,NULL,NULL,NULL,
    0,
    {{SWI_UINT256_BIT_VALUE(SET_11_BITS,1,2,3,4,5,6,16,17,18,19,20)}}
};

unpack_swiavms_SLQSAVMSGetSettings_t tunpack_swiavms_SLQSAVMSGetSettingsDisableResult =
{
    0,0,0,0,0,0,NULL,NULL,NULL,NULL,NULL,
    0,
    {{SWI_UINT256_BIT_VALUE(SET_11_BITS,1,2,3,4,5,6,16,17,18,19,20)}}
};


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

void dump_SLQSAVMSGetSettings(void * ptr)
{
    uint8_t i=0;
    unpack_swiavms_SLQSAVMSGetSettings_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("unpack_swiavms_SLQSAVMSGetSettings return: %d\n", unpackRetCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("\tOMADMEnabled: %u\n", result->OMADMEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 6))
        printf("\tAutoConnect: %u\n", result->AutoConnect);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("\tAutoReboot: %u\n", result->AutoReboot);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
        printf("\tFwAutodownload: %u\n", result->FwPromptdownload);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
        printf("\tFwAutoupdate: %u\n", result->FwPromptUpdate);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 5))
        printf("\tFwAutoCheck: %u\n", result->FwAutoSDM);

    if((result->pPollingTimer) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("\tPollingTimer: 0x%x\n", *(result->pPollingTimer));

    if((result->pNotificationStore) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("\tNotification Status: 0x%x\n", *(result->pNotificationStore));

    if((result->pAPNInfo) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("\tAPN Info\n");
        printf("\t APN Length: %d\n",result->pAPNInfo->bAPNLength);
        if(result->pAPNInfo->pAPN)
            printf("\t APN:  %s\n",result->pAPNInfo->pAPN);
        printf("\t Uname Length: %d\n",result->pAPNInfo->bUnameLength);
        if(result->pAPNInfo->pUname)
            printf("\t Uname : %s\n",result->pAPNInfo->pUname);
        printf("\t PWD Length: %d\n",result->pAPNInfo->bPWDLength);
        if(result->pAPNInfo->pPWD)
            printf("\t PWD : %s\n",result->pAPNInfo->pPWD);
    }
    if ( (result->pConnectionRetryTimers) && 
         (swi_uint256_get_bit(result->ParamPresenceMask, 17)) ) 
    {
        printf("\tRetry Timers:\n");
        for(i=0;i<8;i++)
        {
            printf("\t\t[%d]: %u\n", i,  result->pConnectionRetryTimers->Timers[i]);
        }
    }
    if((result->pPeroidsInfo) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Min/Max Period Info\n");
        printf("\t Min: %d\n",result->pPeroidsInfo->min);
        printf("\t Max: %d\n",result->pPeroidsInfo->max);
    }
    CHECK_WHITELIST_MASK(
        unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", "N/A");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "N/A");
    }
}

void dump_SLQSAVMSGetSettingsModemSettings(void * ptr)
{
    uint8_t i=0;
    unpack_swiavms_SLQSAVMSGetSettings_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL

    DefaultAVMSGetSettings.OMADMEnabled = result->OMADMEnabled;
    DefaultAVMSGetSettings.AutoConnect = result->AutoConnect;
    DefaultAVMSGetSettings.AutoReboot = result->AutoReboot;
    DefaultAVMSGetSettings.FwPromptdownload = result->FwPromptdownload;
    DefaultAVMSGetSettings.FwPromptUpdate = result->FwPromptUpdate;
    DefaultAVMSGetSettings.FwAutoSDM = result->FwAutoSDM;

    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("unpack_swiavms_SLQSAVMSGetSettings return: %d\n", unpackRetCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("\tOMADMEnabled: %u\n", result->OMADMEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 6))
        printf("\tAutoConnect: %u\n", result->AutoConnect);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("\tAutoReboot: %u\n", result->AutoReboot);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
        printf("\tFwAutodownload: %u\n", result->FwPromptdownload);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
        printf("\tFwAutoupdate: %u\n", result->FwPromptUpdate);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 5))
        printf("\tFwAutoCheck: %u\n", result->FwAutoSDM);

    if((result->pPollingTimer) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf("\tPollingTimer: 0x%x\n", *(result->pPollingTimer));

    if((result->pNotificationStore) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
        printf("\tNotification Status: 0x%x\n", *(result->pNotificationStore));

    if((result->pAPNInfo) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("\tAPN Info\n");
        printf("\t APN Length: %d\n",result->pAPNInfo->bAPNLength);
        if(result->pAPNInfo->pAPN)
            printf("\t APN:  %s\n",result->pAPNInfo->pAPN);
        printf("\t Uname Length: %d\n",result->pAPNInfo->bUnameLength);
        if(result->pAPNInfo->pUname)
            printf("\t Uname : %s\n",result->pAPNInfo->pUname);
        printf("\t PWD Length: %d\n",result->pAPNInfo->bPWDLength);
        if(result->pAPNInfo->pPWD)
            printf("\t PWD : %s\n",result->pAPNInfo->pPWD);
    }
    if ( (result->pConnectionRetryTimers) && 
         (swi_uint256_get_bit(result->ParamPresenceMask, 17)) ) 
    {
        printf("\tRetry Timers:\n");
        for(i=0;i<8;i++)
        {
            printf("\t\t[%d]: %u\n", i,  result->pConnectionRetryTimers->Timers[i]);
        }
    }
    if((result->pPeroidsInfo) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Min/Max Period Info\n");
        printf("\t Min: %d\n",result->pPeroidsInfo->min);
        printf("\t Max: %d\n",result->pPeroidsInfo->max);
    }
    CHECK_WHITELIST_MASK(
        unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_swiavms_SLQSAVMSGetSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", "N/A");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "N/A");
    }
}
void dump_SLQSAVMSGetSettingsDwldConnectUpdateEnableResult(void * ptr)
{
    unpack_swiavms_SLQSAVMSGetSettings_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL

    if ( unpackRetCode != 0 )
    {
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,", "N/A");
        local_fprintf("%s\n", remark);
#endif
        return;
    }

    if((swi_uint256_get_bit (result->ParamPresenceMask, 3)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 4)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 5)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 6)))
    {
        #if DEBUG_LOG_TO_FILE
        if ((result->FwPromptdownload == tunpack_swiavms_SLQSAVMSGetSettingsEnableResult.FwPromptdownload) &&
            (result->FwPromptUpdate == tunpack_swiavms_SLQSAVMSGetSettingsEnableResult.FwPromptUpdate) &&
            (result->AutoConnect == tunpack_swiavms_SLQSAVMSGetSettingsEnableResult.AutoConnect) &&
            (result->FwAutoSDM == tunpack_swiavms_SLQSAVMSGetSettingsEnableResult.FwAutoSDM))
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
        else
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        #endif
    }
    else
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("\n");
        #endif
    }
}

void dump_SLQSAVMSGetSettingsDwldConnectUpdateDisableResult(void * ptr)
{
    unpack_swiavms_SLQSAVMSGetSettings_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if ( unpackRetCode != 0 )
    {
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,", "N/A");
        local_fprintf("%s\n", remark);
#endif
        return;
    }
    if((swi_uint256_get_bit (result->ParamPresenceMask, 3)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 4)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 5)) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 6)))
    {
        #if DEBUG_LOG_TO_FILE
        if ((result->FwPromptdownload == tunpack_swiavms_SLQSAVMSGetSettingsDisableResult.FwPromptdownload) &&
            (result->FwPromptUpdate == tunpack_swiavms_SLQSAVMSGetSettingsDisableResult.FwPromptUpdate) &&
            (result->AutoConnect == tunpack_swiavms_SLQSAVMSGetSettingsDisableResult.AutoConnect) &&
            (result->FwAutoSDM == tunpack_swiavms_SLQSAVMSGetSettingsDisableResult.FwAutoSDM))
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
        else
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        #endif
    }
    else
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("\n");
        #endif
    }
}

void dump_SLQSAVMSGetSettingsModemRestore(void * ptr)
{
    unpack_swiavms_SLQSAVMSGetSettings_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_t*) ptr;

    uint8_t cmpResult = 1;

    CHECK_DUMP_ARG_PTR_IS_NULL

    if ( unpackRetCode != 0 )
    {
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,", "N/A");
        local_fprintf("%s\n", remark);
#endif
        return;
    }

    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 6))
    {
        if (DefaultAVMSSetSettings.AutoConnect != result->AutoConnect)
            cmpResult = 0;
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
    {
        if (DefaultAVMSSetSettings.PromptFwDownload != result->FwPromptdownload)
            cmpResult = 0;
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
    {
        if (DefaultAVMSSetSettings.PromptFwUpdate != result->FwPromptUpdate)
            cmpResult = 0;
    }

    #if DEBUG_LOG_TO_FILE
    if(cmpResult == 0)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void swiavms_loop_exit(void)
{
    printf("\nkilling swiavms read thread...\n");
    enswiavmsThread = 0;
#ifdef __ARM_EABI__
    void *pthread_rtn_value;
    if(swiavms_tid!=0)
    pthread_join(swiavms_tid, &pthread_rtn_value);
#endif
    if(swiavms_tid!=0)
    pthread_cancel(swiavms_tid);
    swiavms_tid = 0;
     if(swiavms_fd>=0)
        close(swiavms_fd);
    swiavms_fd=-1;
    return;
}

void _print_swiavms_event_report(unpack_swiavms_SLQSAVMSEventReportInd_t swiavmsInd)
{
    if(swiavmsInd.pBinaryUpdateSessionInfo)
    {
        UnpackSwiAvmsEventReportBinaryUpdateSessionInfo *pResult = swiavmsInd.pBinaryUpdateSessionInfo;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 16))
        {
            printf ("\tBinary Update Session Info\n");    
            printf ("\t\tBinary Type : %u\n",pResult->bBinaryType);
            printf ("\t\tSerity : %u\n",pResult->bSerity);
            printf ("\t\tState : %u\n",pResult->bState);
            printf ("\t\tUser Input Request : %u\n",pResult->bUserInputRequest);
            printf ("\t\tPackage Download Complete : %u\n",pResult->ulPkgDownloadComplete);
            printf ("\t\tPackage Download Size : %u\n",pResult->ulPkgDownloadSize);
            printf ("\t\tUpdate Complete Status : %u\n",pResult->wUpdateCompeteStatus);
            printf ("\t\tUser Input Timeout : %u\n",pResult->wUserInputTimeout);        

            printf ("\t\tDescription Length : %u\n",pResult->wDescriptionLength);
            printf ("\t\tDescription : %s\n",pResult->szDescription);
            printf ("\t\tUser Name Length : %u\n",pResult->wNameLength);
            printf ("\t\tUser Name : %s\n",pResult->szName);
            printf ("\t\tVersion Length : %u\n",pResult->wVersionLength);
            printf ("\t\tVersion : %s\n",pResult->szVersion);
        }
        else
        {
            printf ("\tBinary Update Session Info Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pConfig)
    {
        UnpackSwiAvmsEventReportConfig *pResult = swiavmsInd.pConfig;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 17))
        {            
            printf ("\tAVMS Config\n");    
            printf ("\t\tState : %u\n",pResult->bState);
            printf ("\t\tUser Input Request : %u\n",pResult->bUserInputRequest);
            printf ("\t\tAlert Message : %s\n",pResult->szAlertMsg);
            printf ("\t\tAlert Message Length: %u\n",pResult->wAlertMsgLength);
            printf ("\t\tUser Input Timeout: %u\n",pResult->wUserInputTimeout);
        }
        else
        {
            printf ("\tAVMS Config Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pNotification)
    {
        UnpackSwiAvmsEventReportNotification *pResult = swiavmsInd.pNotification;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 18))
        {
            printf ("\tAVMS Notification\n");    
            printf ("\t\tNotification : %u\n",pResult->bNotification);
            printf ("\t\tSession Status : %u\n",pResult->wSessionStatus);
        }
        else
        {
            printf ("\tAVMS Notification Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pPackageID)
    {
        UnpackSwiAvmsEventReportPackageID *pResult = swiavmsInd.pPackageID;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 21))
        {
            printf ("\tPackage ID\n");    
            printf ("\t\tID : %u\n",pResult->bPackageID);
        }
        else
        {
            printf ("\tPackage ID Tlv Absent\n" );
        }
    }

    if(swiavmsInd.pConnectionRequest)
    {
        UnpackSwiAvmsEventReportConnectionRequest *pResult = swiavmsInd.pConnectionRequest;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 19))
        {
            printf ("\tConnection Request\n");
            printf ("\t\tUser Input Request : %u\n",pResult->bUserInputRequest);
            printf ("\t\tUser Input Timeout : %u\n",pResult->wUserInputTimeout);
        }
        else
        {
            printf ("\tConnection Request Tlv Absent\n" );
        }
    }

    if(swiavmsInd.pDataSessionStatus)
    {
        UnpackSwiAvmsEventReportDataSessionStatus *pResult = swiavmsInd.pDataSessionStatus;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 23))
        {
            printf ("\tData Session Status\n");    
            printf ("\t\tType : %u\n",pResult->bType);
            printf ("\t\tError Code : %u\n",pResult->wErrorCode);
        }
        else
        {
            printf ("\tData Session Status Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pWAMSParaChanged)
    {
        UnpackSwiAvmsEventReportWAMSParamChange *pResult = swiavmsInd.pWAMSParaChanged;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 20))
        {
            printf ("\tWAMS Paramaters\n");    
            printf ("\t\tWamsChangeMask : %u\n",pResult->wWamsChangeMask);
        }
        else
        {
            printf ("\tWAMS Paramaters Tlv Absent\n" );
        }
    } 
    if(swiavmsInd.pRegStatus)
    {
        UnpackSwiAvmsEventReportRegStatus *pResult = swiavmsInd.pRegStatus;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 22))
        {
            printf ("\tLWM2M Registration status\n");    
            printf ("\t\tStatus : %u\n",pResult->bRegStatus);
        }
        else
        {
            printf ("\tRegistration status Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pSessionType)
    {
        UnpackSwiAvmsEventReportSessionType *pResult = swiavmsInd.pSessionType;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 24))
        {
            printf ("\tSession Type\n");    
            printf ("\t\tType : %u\n",pResult->bType);
        }
        else
        {
            printf ("\tSessionType Tlv Absent\n" );
        }
    }
    if(swiavmsInd.pHTTPStatus)
    {
        UnpackSwiAvmsEventReportHTTPStatus *pResult = swiavmsInd.pHTTPStatus;
        if(swi_uint256_get_bit (swiavmsInd.ParamPresenceMask, 25))
        {
            printf ("\tHTTP Status\n");    
            printf ("\t\tStatus : %u\n",pResult->wHTTPStatus);
        }
        else
        {
            printf ("\tHTTPStatus Tlv Absent\n" );
        }
    }
}

void _print_swiavms_session_info(unpack_swiavms_SLQSAVMSSessionGetInfo_t  swiavmsSessionInfo)
{
    if(swiavmsSessionInfo.pBinaryUpdateSessionInfo)
    {
        UnpackSwiAvmsEventReportBinaryUpdateSessionInfo *pBinaryUpdateSessionInfo = swiavmsSessionInfo.pBinaryUpdateSessionInfo;
        if(swi_uint256_get_bit (swiavmsSessionInfo.ParamPresenceMask, 16))
        {
            printf ("\tBinary Update Session Info\n");
            printf ("\t\tBinary Type : %u\n",pBinaryUpdateSessionInfo->bBinaryType);
            printf ("\t\tSerity : %u\n",pBinaryUpdateSessionInfo->bSerity);
            printf ("\t\tState : %u\n",pBinaryUpdateSessionInfo->bState);
            printf ("\t\tUser Input Request : %u\n",pBinaryUpdateSessionInfo->bUserInputRequest);
            printf ("\t\tPackage Download Complete : %u\n",pBinaryUpdateSessionInfo->ulPkgDownloadComplete);
            printf ("\t\tPackage Download Size : %u\n",pBinaryUpdateSessionInfo->ulPkgDownloadSize);
            printf ("\t\tUpdate Complete Status : %u\n",pBinaryUpdateSessionInfo->wUpdateCompeteStatus);
            printf ("\t\tUser Input Timeout : %u\n",pBinaryUpdateSessionInfo->wUserInputTimeout);        

            printf ("\t\tDescription Length : %u\n",pBinaryUpdateSessionInfo->wDescriptionLength);
            printf ("\t\tDescription : %s\n",pBinaryUpdateSessionInfo->szDescription);
            printf ("\t\tUser Name Length : %u\n",pBinaryUpdateSessionInfo->wNameLength);
            printf ("\t\tUser Name : %s\n",pBinaryUpdateSessionInfo->szName);
            printf ("\t\tVersion Length : %u\n",pBinaryUpdateSessionInfo->wVersionLength);
            printf ("\t\tVersion : %s\n",pBinaryUpdateSessionInfo->szVersion);
        }
        else
        {
            printf ("\tBinary Update Session Info Tlv Absent\n");
        }
    }
    if(swi_uint256_get_bit (swiavmsSessionInfo.ParamPresenceMask, 17))
    {
        UnpackSwiAvmsEventReportConfig *pConfig = swiavmsSessionInfo.pConfig;
        if(pConfig->TlvPresent!=0)
        {
            printf ("\tAVMS Config\n");    
            printf ("\t\tState : %u\n",pConfig->bState);
            printf ("\t\tUser Input Request : %u\n",pConfig->bUserInputRequest);
            printf ("\t\tAlert Message : %s\n",pConfig->szAlertMsg);
            printf ("\t\tAlert Message Length: %u\n",pConfig->wAlertMsgLength);
            printf ("\t\tUser Input Timeout: %u\n",pConfig->wUserInputTimeout);
        }
        else
        {
            printf ("\tAVMS Config Tlv Absent\n");
        }
    }
    if(swiavmsSessionInfo.pNotification)
    {
        UnpackSwiAvmsEventReportNotification *pNotification = swiavmsSessionInfo.pNotification;
        if(swi_uint256_get_bit (swiavmsSessionInfo.ParamPresenceMask, 18))
        {
            printf ("\tAVMS Notification\n");    
            printf ("\t\tNotification : %u\n",pNotification->bNotification);
            printf ("\t\tSession Status : %u\n",pNotification->wSessionStatus);
        }
        else
        {
            printf ("\tAVMS Notification Tlv Absent\n");
        }
    }
    if(swiavmsSessionInfo.pPackageID)
    {
        UnpackSwiAvmsEventReportPackageID *pPackageID = swiavmsSessionInfo.pPackageID;
        if(swi_uint256_get_bit (swiavmsSessionInfo.ParamPresenceMask, 19))
        {
            printf ("\tPackage ID\n");    
            printf ("\t\tID : %u\n",pPackageID->bPackageID);
        }
        else
        {
            printf ("\tPackage ID Tlv Absent\n");
        }
    }
    
    printf ("\ttlv Result: %u\n",swiavmsSessionInfo.resultcode);
}

void *swiavms_read_thread(void* ptr)
{
    UNUSEDPARAM(ptr);
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  buffer[QMI_SWIAVMS_MSG_MAX]={'0'};
    int rtn;
    ssize_t rlen;
    printf ("swiavms read thread\n");
    sleep(1);
    while(enswiavmsThread)
    {
        //TODO select multiple file and read them
        memset (buffer,0,QMI_SWIAVMS_MSG_MAX);
        rtn = rlen = read(swiavms_fd, buffer, QMI_SWIAVMS_MSG_MAX);
        if ((rtn > 0) && (rlen > 0))
        {
            qmi_msg = helper_get_resp_ctx(eSWIM2MCMD, buffer, rlen, &rsp_ctx);
            printf("<< receiving %s, Len: %zu\n", qmi_msg, rlen);
            dump_hex(rlen, buffer);

            if (eIND == rsp_ctx.type)
                printf("swiavms IND: ");
            else if (eRSP == rsp_ctx.type)
                printf("swiavms RSP: ");
            printf("msgid 0x%02x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                /** eQMI_SWIOMDM_EVENT_IND **/
                case eQMI_SWI_M2M_AVMS_EVENT_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_swiavms_SLQSAVMSEventReportInd_t swiavmsInd;
                        UnpackSwiAvmsEventReportBinaryUpdateSessionInfo BinaryUpdateSessionInfo;    
                        UnpackSwiAvmsEventReportConfig Config;
                        UnpackSwiAvmsEventReportNotification Notification;
                        UnpackSwiAvmsEventReportPackageID PackageID;
                        UnpackSwiAvmsEventReportConnectionRequest ConnectionRequest;
                        UnpackSwiAvmsEventReportWAMSParamChange WAMSParaChanged;
                        UnpackSwiAvmsEventReportRegStatus RegStatus;
                        UnpackSwiAvmsEventReportDataSessionStatus DataSessionStatus;
                        UnpackSwiAvmsEventReportSessionType SessionType;
                        UnpackSwiAvmsEventReportHTTPStatus HTTPStatus;
                        memset (&swiavmsInd,0,sizeof(unpack_swiavms_SLQSAVMSEventReportInd_t));
                        memset(&BinaryUpdateSessionInfo,0,sizeof(BinaryUpdateSessionInfo));
                        memset(&Config,0,sizeof(Config));
                        memset(&Notification,0,sizeof(Notification));
                        memset(&PackageID,0,sizeof(PackageID));
                        memset(&ConnectionRequest,0,sizeof(ConnectionRequest));
                        memset(&WAMSParaChanged,0,sizeof(WAMSParaChanged));
                        memset(&RegStatus,0,sizeof(RegStatus));
                        memset(&DataSessionStatus,0,sizeof(DataSessionStatus));
                        memset(&SessionType,0,sizeof(SessionType));
                        memset(&HTTPStatus,0,sizeof(HTTPStatus));
                        swiavmsInd.pBinaryUpdateSessionInfo = &BinaryUpdateSessionInfo;
                        swiavmsInd.pConfig = &Config;
                        swiavmsInd.pNotification = &Notification;
                        swiavmsInd.pConfig = &Config;
                        swiavmsInd.pPackageID = &PackageID;
                        swiavmsInd.pConnectionRequest = &ConnectionRequest;
                        swiavmsInd.pWAMSParaChanged = &WAMSParaChanged;
                        swiavmsInd.pRegStatus = &RegStatus;
                        swiavmsInd.pDataSessionStatus = &DataSessionStatus;
                        swiavmsInd.pSessionType = &SessionType;
                        swiavmsInd.pHTTPStatus = &HTTPStatus;

                        unpackRetCode = unpack_swiavms_SLQSAVMSEventReportInd (buffer, rlen, &swiavmsInd);
                    #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSEventReportInd",\
                                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                                        
                            local_fprintf("%s\n", "Correct");

                            CHECK_WHITELIST_MASK(
                                unpack_swiavms_SLQSAVMSEventReportIndParamPresenceMaskWhiteList,
                                swiavmsInd.ParamPresenceMask);
                    #endif

                        printf("unpack_swiavms_SLQSAVMSEventReportInd return: %d\n", unpackRetCode);
                        swi_uint256_print_mask (swiavmsInd.ParamPresenceMask);
                        _print_swiavms_event_report(swiavmsInd);
                    }
                    break; //eQMI_SWI_M2M_AVMS_EVENT_IND
                
                /** eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT **/
                case eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT:
                    {
                        unpack_swiavms_SLQSAvmsSetEventReport_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAvmsSetEventReport_t));
                        unpackRetCode = unpack_swiavms_SLQSAvmsSetEventReport (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAvmsSetEventReport",\
                                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        CHECK_WHITELIST_MASK(
                            unpack_swiavms_SLQSAvmsSetEventReportParamPresenceMaskWhiteList,
                            swiavmsRsp.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_swiavms_SLQSAvmsSetEventReportParamPresenceMaskMandatoryList,
                            swiavmsRsp.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAvmsSetEventReport return: %d\n", unpackRetCode);
                    } 
                    break;//eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT
                    
                /** eQMI_SWI_M2M_AVMS_SESSION_START **/
                case eQMI_SWI_M2M_AVMS_SESSION_START:
                    {
                        unpack_swiavms_SLQSAVMSStartSession_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSStartSession_t));
                        unpackRetCode = unpack_swiavms_SLQSAVMSStartSession (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSStartSession",\
                                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        CHECK_WHITELIST_MASK(
                            unpack_swiavms_SLQSAVMSStartSessionParamPresenceMaskWhiteList,
                            swiavmsRsp.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_swiavms_SLQSAVMSStartSessionParamPresenceMaskMandatoryList,
                            swiavmsRsp.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,",  (swiavmsRsp.sessionResponse > 3) ? "Wrong": "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        printf("unpack_swiavms_SLQSAVMSStartSession return: %d\n", unpackRetCode);
                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 1))
                            printf("\tSession Response: %u\n", swiavmsRsp.sessionResponse);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SESSION_START
                
                /** eQMI_SWI_M2M_AVMS_SESSION_STOP **/
                case eQMI_SWI_M2M_AVMS_SESSION_STOP:
                    {
                        unpack_swiavms_SLQSAVMSStopSession_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSStopSession_t));
                        unpackRetCode = unpack_swiavms_SLQSAVMSStopSession (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSStopSession",\
                                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);

                        CHECK_WHITELIST_MASK(
                            unpack_swiavms_SLQSAVMSStopSessionParamPresenceMaskWhiteList,
                            swiavmsRsp.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_swiavms_SLQSAVMSStopSessionParamPresenceMaskMandatoryList,
                            swiavmsRsp.ParamPresenceMask);

                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAVMSStopSession return: %d\n", unpackRetCode);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SESSION_STOP
                    
                /** eQMI_SWI_M2M_AVMS_SET_SETTINGS **/
                case eQMI_SWI_M2M_AVMS_SET_SETTINGS:
                    {
                        unpack_swiavms_SLQSAVMSSetSettings_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSSetSettings_t));
                        rtn = unpack_swiavms_SLQSAVMSSetSettings (buffer, rlen, &swiavmsRsp);
                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSSetSettings",\
                                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        CHECK_WHITELIST_MASK(
                            unpack_swiavms_SLQSAVMSSetSettingsParamPresenceMaskWhiteList,
                            swiavmsRsp.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_swiavms_SLQSAVMSSetSettingsParamPresenceMaskMandatoryList,
                            swiavmsRsp.ParamPresenceMask);

                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAVMSSetSettings return: %d\n", rtn);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SET_SETTINGS
                
                /** eQMI_SWI_M2M_AVMS_GET_SETTINGS **/
                case eQMI_SWI_M2M_AVMS_GET_SETTINGS:
                    {
                        unpack_swiavms_SLQSAVMSGetSettings_t swiavmsRsp;
                        PackSwiAVMSSettingsConnectionRetryTimers ConnectionRetryTimers;
                        PackSwiAVMSSettingsAPNInfo               APNInfo;
                        PackSwiAVMSSettingsPeriodsInfo           PeriodsInfo;

                        uint8_t szPWD[LITEQMI_MAX_GET_SETTINGS_AVMS_PWD_STRING_LENGTH + 1]={0};
                        uint8_t szAPN[LITEQMI_MAX_GET_SETTINGS_AVMS_APN_STRING_LENGTH + 1]={0};
                        uint8_t szUname[LITEQMI_MAX_GET_SETTINGS_AVMS_UNAME_STRING_LENGTH + 1]={0};
                        uint32_t pollingTimer = 0;
                        uint8_t NotificationStore = 0;

                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSGetSettings_t));
                        memset(&ConnectionRetryTimers,0,sizeof(PackSwiAVMSSettingsConnectionRetryTimers));
                        memset(&APNInfo,0,sizeof(PackSwiAVMSSettingsAPNInfo));
                        memset(&PeriodsInfo,0,sizeof(PackSwiAVMSSettingsPeriodsInfo));

                        /* 1 extra byte to NULL terminate string */
                        APNInfo.bAPNLength = LITEQMI_MAX_GET_SETTINGS_AVMS_APN_STRING_LENGTH + 1;
                        APNInfo.pAPN = &szAPN[0];
                        APNInfo.bPWDLength = LITEQMI_MAX_GET_SETTINGS_AVMS_PWD_STRING_LENGTH + 1;
                        APNInfo.pPWD = &szPWD[0];
                        APNInfo.bUnameLength = LITEQMI_MAX_GET_SETTINGS_AVMS_UNAME_STRING_LENGTH + 1;
                        APNInfo.pUname = &szUname[0]; 

                        swiavmsRsp.pConnectionRetryTimers = &ConnectionRetryTimers;
                        swiavmsRsp.pAPNInfo               = &APNInfo;
                        swiavmsRsp.pPeroidsInfo           = &PeriodsInfo;
                        swiavmsRsp.pPollingTimer          = &pollingTimer;
                        swiavmsRsp.pNotificationStore     = &NotificationStore;
                        unpackRetCode = unpack_swiavms_SLQSAVMSGetSettings (buffer, rlen, &swiavmsRsp);

                        #if DEBUG_LOG_TO_FILE
                        checkErrorAndFormErrorString(buffer);
                        #endif

                        if (rsp_ctx.xid == 0x101)
                        {
                        #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings Modem",\
                                     ((unpackRetCode == eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        #endif
                            dump_SLQSAVMSGetSettingsModemSettings((void *)&swiavmsRsp);
                        }
                        else if (rsp_ctx.xid == 0x107)
                        {
                        #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings Enable",\
                                     ((unpackRetCode == eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        #endif
                            dump_SLQSAVMSGetSettingsDwldConnectUpdateEnableResult((void *)&swiavmsRsp);

                        }
                        else if (rsp_ctx.xid == 0x109)
                        {
                        #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings Disable",\
                                     ((unpackRetCode == eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        #endif
                            dump_SLQSAVMSGetSettingsDwldConnectUpdateDisableResult((void *)&swiavmsRsp);
                        }
                        else if (rsp_ctx.xid == 0x10F)
                        {
                        #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings RestoreResult",\
                                     ((unpackRetCode == eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        #endif
                            dump_SLQSAVMSGetSettingsModemRestore((void *)&swiavmsRsp);
                        }
                        else
                        {
                        #if DEBUG_LOG_TO_FILE
                            local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings",\
                                     ((unpackRetCode == eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        #endif
                            dump_SLQSAVMSGetSettings((void *)&swiavmsRsp);
                        }
                    }
                    break; //eQMI_SWI_M2M_AVMS_GET_SETTINGS
                    
                /** eQMI_SWI_M2M_AVMS_SELECTION **/
                case eQMI_SWI_M2M_AVMS_SELECTION:
                    {
                        unpack_swiavms_SLQSAVMSSendSelection_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSSendSelection_t));
                        unpackRetCode = unpack_swiavms_SLQSAVMSSendSelection (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSSendSelection",\
                                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        CHECK_WHITELIST_MASK(
                            unpack_swiavms_SLQSAVMSSendSelectionParamPresenceMaskWhiteList,
                            swiavmsRsp.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_swiavms_SLQSAVMSSendSelectionParamPresenceMaskMandatoryList,
                            swiavmsRsp.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAVMSSendSelection return: %d\n", unpackRetCode);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SELECTION
                    
                /** eQMI_SWI_M2M_AVMS_SESSION_GETINFO **/
                case eQMI_SWI_M2M_AVMS_SESSION_GETINFO:
                    {
                        unpack_swiavms_SLQSAVMSSessionGetInfo_t swiavmsRsp;
                        UnpackSwiAvmsEventReportBinaryUpdateSessionInfo BinaryUpdateSessionInfo;    
                        UnpackSwiAvmsEventReportConfig Config;
                        UnpackSwiAvmsEventReportNotification Notification;
                        UnpackSwiAvmsEventReportPackageID PackageID;
                        memset (&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSSessionGetInfo_t));                        
                        memset(&BinaryUpdateSessionInfo,0,sizeof(BinaryUpdateSessionInfo));
                        memset(&Config,0,sizeof(Config));
                        memset(&Notification,0,sizeof(Notification));
                        memset(&PackageID,0,sizeof(PackageID));
                        swiavmsRsp.pBinaryUpdateSessionInfo = &BinaryUpdateSessionInfo;
                        swiavmsRsp.pConfig = &Config;
                        swiavmsRsp.pNotification = &Notification;
                        swiavmsRsp.pPackageID = &PackageID;
                        unpackRetCode = unpack_swiavms_SLQSAVMSSessionGetInfo (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSSessionGetInfo",\
                                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        if ( unpackRetCode != 0 )
                        {
                            local_fprintf("%s,", "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n", "Correct");
                        }
                    #endif

                        printf("unpack_swiavms_SLQSAVMSGetSessionInfo return: %d\n", unpackRetCode);
                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);
                        _print_swiavms_session_info(swiavmsRsp);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SESSION_GETINFO
                    
                default:
                    break;
            }
        }
        else
        {
            printf("read error: %d\n", rtn);
            return NULL;
        }
    }    
    return NULL;
}

void swiavms_loop(void)
{
    uint8_t  buffer[QMI_SWIAVMS_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_SWIAVMS_MSG_MAX;
    pack_qmi_t req_ctx;
    uint16_t   txID = 0x101;
    int rtn = -1;
    const char *qmi_msg;

    printf("======swiavms dummy unpack test===========\n");
    iLocalLog = 0;
    swiavms_validate_dummy_unpack();
    iLocalLog = 1;
    // Get swiavms FD
    swiavms_fd = client_fd(eSWIM2MCMD);
    if(0>swiavms_fd)
    {
        fprintf (stderr, "Service(SWIAVMS) Not Supproted %d\n",eSWIM2MCMD);
        return ;
    }
    sleep(1);
    printf("======SWIAVMS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "SWIAVMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    // Start swiavms read thread
    memset(&swiavms_attr, 0, sizeof(swiavms_attr));
    enswiavmsThread = 1;
    pthread_create(&swiavms_tid, &swiavms_attr, swiavms_read_thread, NULL);
    memset(&DefaultAVMSGetSettings,0,sizeof(unpack_swiavms_SLQSAVMSGetSettings_t));
    sleep(1);
    // AVMS Get Modem Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x101
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings(&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings Modem", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr,"Get Modem Settings\n");
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }    
    sleep(1);
    // Register for swiavms event notification
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x102
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAvmsSetEventReport (&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAvmsSetEventReport", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAvmsSetEventReport ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    // Start FOTA Session
    {
        pack_swiavms_SLQSAVMSStartSession_t swiavms_req;
        swiavms_req.sessionType = 0x01; //FOTA
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x103
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swiavms_SLQSAVMSStartSession(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSStartSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSStartSession FOTA ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    // AVMS Get Session Info
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x104
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSessionGetInfo(&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSessionGetInfo", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSessionGetInfo ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
     sleep(7);
    // AVMS Set Settings
    {
        uint8_t Autosdm     = 0;
        uint8_t FwAutoCheck = 0;
        pack_swiavms_SLQSAVMSSetSettings_t swiavms_req;
        memset(&swiavms_req, 0, sizeof(swiavms_req));

        swiavms_req.AutoConnect = FwAutoCheck;

        /* Some modules returns error with auto reboot parameter, 
         * so don't change it to nullify its impact during restore
         */
        swiavms_req.AutoReboot = DefaultAVMSGetSettings.AutoReboot;
        swiavms_req.PromptFwDownload = 0;
        swiavms_req.PromptFwUpdate = 0;
        swiavms_req.pFwAutoSDM     = &Autosdm;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x105
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettings(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(5);

    // AVMS Set Setting without Reboot paramter
    {
        uint8_t Autosdm     = 1;
        uint8_t FwAutoCheck = 1;
        pack_swiavms_SLQSAVMSSetSettings_t swiavms_req;
        memset(&swiavms_req,0,sizeof(swiavms_req));

        swiavms_req.AutoConnect = FwAutoCheck;
        swiavms_req.AutoReboot = 0;
        swiavms_req.PromptFwDownload = 1;
        swiavms_req.PromptFwUpdate = 1;
        swiavms_req.pFwAutoSDM     = &Autosdm;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x106
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField Enable", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(5);
    // GetSettings enable
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x107
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings(&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings Enable", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr,"Get Modem Settings\n");
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    // AVMS Set Setting without Reboot paramter
    {
        uint8_t Autosdm     = 0;
        uint8_t FwAutoCheck = 0;
        pack_swiavms_SLQSAVMSSetSettings_t swiavms_req;
        memset(&swiavms_req,0,sizeof(swiavms_req));

        swiavms_req.AutoConnect = FwAutoCheck;
        swiavms_req.AutoReboot = 0;
        swiavms_req.PromptFwDownload = 0;
        swiavms_req.PromptFwUpdate = 0;
        swiavms_req.pFwAutoSDM     = &Autosdm;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x108
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField Disable", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(5);

    // GetSettings disable
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x109
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings(&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings Disable", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr,"Get Modem Settings\n");
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    // AVMS Selection
    {
        pack_swiavms_SLQSAVMSSendSelection_t  swiavms_req;
        memset(&swiavms_req,0,sizeof(swiavms_req));
        swiavms_req.selection     = 0x01; // Accept
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10A
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSendSelection(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSendSelection", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSendSelection ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(5);
    // AVMS Selection
    {
        pack_swiavms_SLQSAVMSSendSelection_t  swiavms_req;
        memset(&swiavms_req,0,sizeof(swiavms_req));
        swiavms_req.selection     = 0x03; // Accept
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10B
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSendSelection(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSendSelection", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSendSelection ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(1);
    #if 0
    fprintf (stderr, "Wait 1200 Sec\n");
    sleep(1200);
    #endif
    // Stop Session
    {
        pack_swiavms_SLQSAVMSStopSession_t swiavms_req;
        swiavms_req.sessionType = 0xFF; //Any ongoing session
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10C
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swiavms_SLQSAVMSStopSession(&req_ctx,buffer,&reqLen,swiavms_req);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSStopSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSStoplSession (Any ) ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }

    sleep(5);
    // AVMS Get Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10D
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    //Restore Settings
    {
        memset(&DefaultAVMSSetSettings, 0, sizeof(DefaultAVMSSetSettings));

        DefaultAVMSSetSettings.AutoConnect = DefaultAVMSGetSettings.AutoConnect;
        DefaultAVMSSetSettings.AutoReboot = DefaultAVMSGetSettings.AutoReboot;
        DefaultAVMSSetSettings.PromptFwDownload = DefaultAVMSGetSettings.FwPromptdownload;
        DefaultAVMSSetSettings.PromptFwUpdate = DefaultAVMSGetSettings.FwPromptUpdate;
        DefaultAVMSSetSettings.pFwAutoSDM     = &DefaultAVMSGetSettings.FwAutoSDM;

        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10E
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettingsNoAutoRebootField(&req_ctx,buffer,&reqLen,DefaultAVMSSetSettings);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettings ModemRestore", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
    //Validate Restore Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID; //0x10F
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings(&req_ctx,buffer,&reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings RestoreResult", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        fprintf (stderr,"Get Modem Settings\n");
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings ret: %d, Len: %d\n", rtn, reqLen);
        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);
swiavmsdemo_cleanup:
    enswiavmsThread = 1;
    sleep(1);
    if(swiavms_fd>=0)
    {
        close(swiavms_fd);
    }
    sleep(5);
    return;
}

void dump_SLQSAVMSStartSession(void *ptr)
{
    unpack_swiavms_SLQSAVMSStartSession_t *result =
            (unpack_swiavms_SLQSAVMSStartSession_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("\tSession Response: %u\n", result->sessionResponse);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSAVMSSessionGetInfo(void *ptr)
{
    unpack_swiavms_SLQSAVMSSessionGetInfo_t *result =
            (unpack_swiavms_SLQSAVMSSessionGetInfo_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    _print_swiavms_session_info(*result);

}
void dump_SLQSAVMSEventReportInd(void *ptr)
{
    unpack_swiavms_SLQSAVMSEventReportInd_t *result =
            (unpack_swiavms_SLQSAVMSEventReportInd_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    _print_swiavms_event_report(*result);

}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_swiavms_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_SWI_M2M_AVMS_SESSION_START */
    {0x02,0x01,0x00,0xA0,0x00,0x0E,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x02,0x00,0x00,0x00 },

    /* eQMI_SWI_M2M_AVMS_SESSION_STOP */
    {0x02,0x02,0x00,0xA1,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_SET_SETTINGS */
    {0x02,0x03,0x00,0xA5,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
    {0x02,0x04,0x00,0xA6,0x00,0x60,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x04,0x00,0x00,0x00,0x00,0x00,
            0x03,0x01,0x00,0x01,
            0x04,0x01,0x00,0x01,
            0x05,0x01,0x00,0x00,
            0x06,0x01,0x00,0x01,
            0x10,0x04,0x00,0x00,0x00,0x34,0x12,
            0x11,0x10,0x00,0x22,0x11,0x22,0x33,0x44,0x33,0x55,0x44,
                              0x22,0x33,0x10,0x20,0x20,0x30,0x40,0x50,
            0x12,0x12,0x00,0x03,'w','w','w',0x06,'s','i','e','r','r','a',
                            0x06,'s','i','e','r','r','a',
            0x13,0x01,0x00,0x01,
            0x14,0x08,0x00,0x12,0x34,0x00,0x00,0x56,0x78,0x00,0x00,
            0x15,0x01,0x00,0x01,
    },

    /* eQMI_SWI_M2M_AVMS_SELECTION */
    {0x02,0x05,0x00,0xA4,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT */
    {0x02,0x06,0x00,0xA7,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_SESSION_GETINFO */
    {0x02,0x07,0x00,0xA2,0x00,0x4D,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x16,0x00,0x01,0x03,0x01,0x00,0x00,0x44,0x33,0x22,0x11,
                            0x44,0x33,0x22,0x00,0x00,0x00,0x01,0x00,0x00,
                            0x00,0x00,0x00,0x00,
            0x11,0x20,0x00,0x01,0x00,0x00,0x00,0x1A,0x00,0x43,0x00,0x6F,0x00,0x6E,0x00,0x66,
                    0x00,0x69,0x00,0x67,0x00,0x75,0x00,0x72,0x00,0x61,0x00,0x74,0x00,0x69,0x00,0x6F,0x00,0x6E,0x00,
            0x12,0x03,0x00,0x14,0x00,0x00,
            0x13,0x01,0x00,0x12},

            /* eQMI_SWI_M2M_AVMS_EVENT_IND */
            {0x04,0x08,0x00,0xA3,0x00,0x64,0x00,
                    0x10,0x16,0x00,0x01,0x03,0x01,0x00,0x00,0x44,0x33,0x22,0x11,
                                    0x44,0x33,0x22,0x00,0x00,0x00,0x01,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                    0x11,0x20,0x00,0x01,0x00,0x00,0x00,0x1A,0x00,0x43,0x00,0x6F,0x00,0x6E,0x00,0x66,
                            0x00,0x69,0x00,0x67,0x00,0x75,0x00,0x72,0x00,0x61,0x00,0x74,0x00,0x69,0x00,0x6F,0x00,0x6E,0x00,
                    0x12,0x03,0x00,0x14,0x00,0x00,
                    0x13,0x03,0x00,0x01,0x00,0x00,
                    0x14,0x02,0x00,0xFF,0x00,
                    0x15,0x01,0x00,0x12,
                    0x16,0x01,0x00,0x03,
                    0x17,0x03,0x00,0x01,0x00,0x00,
                    0x18,0x01,0x00,0x01,
                    0x19,0x02,0x00,0x00,0x00},

};

/* eQMI_SWI_M2M_AVMS_SESSION_START */
const unpack_swiavms_SLQSAVMSStartSession_t const_unpack_swiavms_SLQSAVMSStartSession_t = {
        0x02,1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_SWI_M2M_AVMS_SESSION_STOP */
const unpack_swiavms_SLQSAVMSStopSession_t const_unpack_swiavms_SLQSAVMSStopSession_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWI_M2M_AVMS_SET_SETTINGS */
const unpack_swiavms_SLQSAVMSSetSettings_t const_unpack_swiavms_SLQSAVMSSetSettings_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t  cst_PollingTimer = 0x12340000;
PackSwiAVMSSettingsConnectionRetryTimers cst_ConnectionRetryTimers =
        {{0x1122,0x3322,0x3344,0x4455,0x3322,0x2010,0x3020,0x5040}};
uint8_t    cst_getsettingsAPN[] = "www";
uint8_t    cst_getsettingsUname[] ="sierra";
uint8_t    cst_getsettingsPwd[] = "sierra";
PackSwiAVMSSettingsAPNInfo cst_APNInfo =
        {0x03,cst_getsettingsAPN,0x06,cst_getsettingsUname,0x06,cst_getsettingsPwd};
uint8_t    cst_NotificationStore = 1 ;
PackSwiAVMSSettingsPeriodsInfo  cst_PeroidsInfo ={0x3412,0x7856};

/* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
const unpack_swiavms_SLQSAVMSGetSettings_t const_unpack_swiavms_SLQSAVMSGetSettings_t = {
        0, 1, 1, 1, 1, 0, &cst_PollingTimer, &cst_ConnectionRetryTimers, &cst_APNInfo,
        &cst_NotificationStore, &cst_PeroidsInfo,1,{{SWI_UINT256_BIT_VALUE(SET_12_BITS,2,1,3,4,5,6,16,17,18,19,20,21)}} };

uint32_t  var_PollingTimer;
PackSwiAVMSSettingsConnectionRetryTimers var_ConnectionRetryTimers;
uint8_t    getsettingsAPN[49];
uint8_t    getsettingsUname[29];
uint8_t    getsettingsPwd[29];
PackSwiAVMSSettingsAPNInfo var_APNInfo =
        {49,getsettingsAPN,29,getsettingsUname,29,getsettingsPwd};
uint8_t    var_NotificationStore;
PackSwiAVMSSettingsPeriodsInfo  var_PeroidsInfo;

/* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
unpack_swiavms_SLQSAVMSGetSettings_t var_unpack_swiavms_SLQSAVMSGetSettings_t = {
        0, 0, 0, 0, 0, 0, &var_PollingTimer, &var_ConnectionRetryTimers, &var_APNInfo,
        &var_NotificationStore, &var_PeroidsInfo,0,{{0}} };

/* eQMI_SWI_M2M_AVMS_SELECTION */
const unpack_swiavms_SLQSAVMSSendSelection_t const_unpack_swiavms_SLQSAVMSSendSelection_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT */
const unpack_swiavms_SLQSAvmsSetEventReport_t const_unpack_swiavms_SLQSAvmsSetEventReport_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

UnpackSwiAvmsEventReportBinaryUpdateSessionInfo cst_BinaryUpdateSessionInfo =
        {0x01,0x03,0x01,0x00,0x11223344,0x00223344,0x00,0x01,0,{0},0,{0},0,{0},1};
UnpackSwiAvmsEventReportConfig cst_Config =
        {0x01,0x00,0x00,0x1A,{0x43,0x00,0x6F,0x00,0x6E,0x00,0x66,
                0x00,0x69,0x00,0x67,0x00,0x75,0x00,0x72,0x00,0x61,0x00,0x74,0x00,0x69,0x00,0x6F,0x00,0x6E},
                1};

UnpackSwiAvmsEventReportNotification cst_Notification = {0x14,0x00,0x01};
UnpackSwiAvmsEventReportPackageID cst_PackageID = {0x12,1};

/* eQMI_SWI_M2M_AVMS_SESSION_GETINFO */
const unpack_swiavms_SLQSAVMSSessionGetInfo_t const_unpack_swiavms_SLQSAVMSSessionGetInfo_t = {
        &cst_BinaryUpdateSessionInfo, &cst_Config, &cst_Notification, &cst_PackageID,1,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

UnpackSwiAvmsEventReportBinaryUpdateSessionInfo var_BinaryUpdateSessionInfo;
UnpackSwiAvmsEventReportConfig var_Config;
UnpackSwiAvmsEventReportNotification var_Notification;
UnpackSwiAvmsEventReportPackageID var_PackageID;

/* eQMI_SWI_M2M_AVMS_SESSION_GETINFO */
unpack_swiavms_SLQSAVMSSessionGetInfo_t var_unpack_swiavms_SLQSAVMSSessionGetInfo_t = {
        &var_BinaryUpdateSessionInfo, &var_Config, &var_Notification, &var_PackageID,0,{{0}} };

UnpackSwiAvmsEventReportConnectionRequest cst_ConnectionRequest = {0x01,0x00,1};
UnpackSwiAvmsEventReportWAMSParamChange cst_WAMSParaChanged = {0x00FF,1};
UnpackSwiAvmsEventReportRegStatus cst_RegStatus = {0x03,1};
UnpackSwiAvmsEventReportDataSessionStatus cst_DataSessionStatus = {0x01,0x00,1};
UnpackSwiAvmsEventReportSessionType cst_SessionType = {1,1};
UnpackSwiAvmsEventReportHTTPStatus cst_HTTPStatus = {0x00,1};

/* eQMI_SWI_M2M_AVMS_EVENT_IND */
const unpack_swiavms_SLQSAVMSEventReportInd_t const_unpack_swiavms_SLQSAVMSEventReportInd_t = {
        &cst_BinaryUpdateSessionInfo, &cst_Config, &cst_Notification, &cst_PackageID,
        &cst_ConnectionRequest, &cst_WAMSParaChanged, &cst_RegStatus, &cst_DataSessionStatus,
        &cst_SessionType, &cst_HTTPStatus,
        1,{{SWI_UINT256_BIT_VALUE(SET_10_BITS,16,17,18,19,20,21,22,23,24,25)}} };

UnpackSwiAvmsEventReportConnectionRequest var_ConnectionRequest;
UnpackSwiAvmsEventReportWAMSParamChange var_WAMSParaChanged;
UnpackSwiAvmsEventReportRegStatus var_RegStatus;
UnpackSwiAvmsEventReportDataSessionStatus var_DataSessionStatus;
UnpackSwiAvmsEventReportSessionType var_SessionType;
UnpackSwiAvmsEventReportHTTPStatus var_HTTPStatus;

/* eQMI_SWI_M2M_AVMS_EVENT_IND */
unpack_swiavms_SLQSAVMSEventReportInd_t var_unpack_swiavms_SLQSAVMSEventReportInd_t = {
        &var_BinaryUpdateSessionInfo, &var_Config, &var_Notification, &var_PackageID,
        &var_ConnectionRequest, &var_WAMSParaChanged, &var_RegStatus, &var_DataSessionStatus,
        &var_SessionType, &var_HTTPStatus,0,{{0}} };

int swiavms_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swiavms_resp_msg)/sizeof(validate_swiavms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swiavms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSWIM2MCMD, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWIAVMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIAVMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_SWI_M2M_AVMS_SESSION_START:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSStartSession,
                    dump_SLQSAVMSStartSession,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSStartSession_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SESSION_STOP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSStopSession,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSStopSession_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSSetSettings,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSSetSettings_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_GET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swiavms_SLQSAVMSGetSettings_t *varp = &var_unpack_swiavms_SLQSAVMSGetSettings_t;
                    const unpack_swiavms_SLQSAVMSGetSettings_t *cstp = &const_unpack_swiavms_SLQSAVMSGetSettings_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSGetSettings,
                    dump_SLQSAVMSGetSettings,
                    msg.buf,
                    rlen,
                    &var_unpack_swiavms_SLQSAVMSGetSettings_t,
                    14,
                    CMP_PTR_TYPE, &varp->OMADMEnabled, &cstp->OMADMEnabled,
                    CMP_PTR_TYPE, &varp->AutoConnect, &cstp->AutoConnect,
                    CMP_PTR_TYPE, &varp->AutoReboot, &cstp->AutoReboot,
                    CMP_PTR_TYPE, &varp->FwPromptdownload, &cstp->FwPromptdownload,
                    CMP_PTR_TYPE, &varp->FwPromptUpdate, &cstp->FwPromptUpdate,
                    CMP_PTR_TYPE, &varp->FwAutoSDM, &cstp->FwAutoSDM,
                    CMP_PTR_TYPE, varp->pPollingTimer, cstp->pPollingTimer,
                    CMP_PTR_TYPE, varp->pConnectionRetryTimers, cstp->pConnectionRetryTimers,
                    CMP_PTR_TYPE, varp->pAPNInfo->pAPN, cstp->pAPNInfo->pAPN,
                    CMP_PTR_TYPE, varp->pAPNInfo->pPWD, cstp->pAPNInfo->pPWD,
                    CMP_PTR_TYPE, varp->pAPNInfo->pUname, cstp->pAPNInfo->pUname,
                    CMP_PTR_TYPE, varp->pNotificationStore, cstp->pNotificationStore,
                    CMP_PTR_TYPE, varp->pPeroidsInfo, cstp->pPeroidsInfo,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SELECTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSSendSelection,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSSendSelection_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SET_EVENT_REPORT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAvmsSetEventReport,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAvmsSetEventReport_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SESSION_GETINFO:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swiavms_SLQSAVMSSessionGetInfo_t *varp = &var_unpack_swiavms_SLQSAVMSSessionGetInfo_t;
                    const unpack_swiavms_SLQSAVMSSessionGetInfo_t *cstp = &const_unpack_swiavms_SLQSAVMSSessionGetInfo_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSSessionGetInfo,
                    dump_SLQSAVMSSessionGetInfo,
                    msg.buf,
                    rlen,
                    &var_unpack_swiavms_SLQSAVMSSessionGetInfo_t,
                    5,
                    CMP_PTR_TYPE, varp->pBinaryUpdateSessionInfo, cstp->pBinaryUpdateSessionInfo,
                    CMP_PTR_TYPE, varp->pConfig, cstp->pConfig,
                    CMP_PTR_TYPE, varp->pNotification, cstp->pNotification,
                    CMP_PTR_TYPE, varp->pPackageID, cstp->pPackageID,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_SWI_M2M_AVMS_EVENT_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_swiavms_SLQSAVMSEventReportInd_t *varp = &var_unpack_swiavms_SLQSAVMSEventReportInd_t;
                    const unpack_swiavms_SLQSAVMSEventReportInd_t *cstp = &const_unpack_swiavms_SLQSAVMSEventReportInd_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSEventReportInd,
                    dump_SLQSAVMSEventReportInd,
                    msg.buf,
                    rlen,
                    &var_unpack_swiavms_SLQSAVMSEventReportInd_t,
                    11,
                    CMP_PTR_TYPE, varp->pBinaryUpdateSessionInfo, cstp->pBinaryUpdateSessionInfo,
                    CMP_PTR_TYPE, varp->pConfig, cstp->pConfig,
                    CMP_PTR_TYPE, varp->pNotification, cstp->pNotification,
                    CMP_PTR_TYPE, varp->pPackageID, cstp->pPackageID,
                    CMP_PTR_TYPE, varp->pConnectionRequest, cstp->pConnectionRequest,
                    CMP_PTR_TYPE, varp->pWAMSParaChanged, cstp->pWAMSParaChanged,
                    CMP_PTR_TYPE, varp->pRegStatus, cstp->pRegStatus,
                    CMP_PTR_TYPE, varp->pDataSessionStatus, cstp->pDataSessionStatus,
                    CMP_PTR_TYPE, varp->pSessionType, cstp->pSessionType,
                    CMP_PTR_TYPE, varp->pHTTPStatus, cstp->pHTTPStatus,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            }
        }
    }
    return 0;
}
