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

enum{
    ACCEPT = 1,
    REJECT,
    DEFER
};

// FUNCTION PROTOTYPES
int client_fd(uint8_t svc);
int swiavms_avc2_validate_dummy_unpack();

// swiavms GLOBALS
volatile int enswiavmsThread;
int swiavms_fd;
static pthread_t swiavms_tid = 0;
pthread_attr_t swiavms_attr;
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;


// TEST VALUES
#ifdef SWIAVMS_EVENTS_DEBUG
uint8_t swiavms_event_sample[] = {};
#endif //SWIAVMS_EVENTS_DEBUG

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

static void _print_swiavms_event_report(unpack_swiavms_SLQSAVMSEventReportInd_t swiavmsInd)
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

static void _print_swiavms_session_info(unpack_swiavms_SLQSAVMSSessionGetInfo_t  swiavmsSessionInfo)
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
    if(swiavmsSessionInfo.pConfig)
    {
        UnpackSwiAvmsEventReportConfig *pConfig = swiavmsSessionInfo.pConfig;
        if(swi_uint256_get_bit (swiavmsSessionInfo.ParamPresenceMask, 17))
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
    
    printf ("\ttlv Reuslt: %u\n",swiavmsSessionInfo.resultcode);
}

static void avms_accept()
{
    int rtn = -1;
    uint8_t  buffer[QMI_SWIAVMS_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_SWIAVMS_MSG_MAX;
    const char *qmi_msg;
    pack_qmi_t req_ctx;
    pack_swiavms_SLQSAVMSSendSelection_t  swiavms_req;
    memset(&swiavms_req,0,sizeof(swiavms_req));
    swiavms_req.selection  = ACCEPT;
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = ~0;
    reqLen = QMI_SWIAVMS_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn = pack_swiavms_SLQSAVMSSendSelection(&req_ctx,buffer,&reqLen,swiavms_req);
    fprintf (stderr, "pack_swiavms_SLQSAVMSSendSelection ret: %d, Len: %d\n", rtn, reqLen);
    rtn = write(swiavms_fd, buffer, reqLen);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn == reqLen)
    {
        qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }

}

static void accept_fota(uint8_t isPrompt)
{
    if (isPrompt)
        avms_accept();
}

static void *swiavms_read_thread(void* ptr)
{
    UNUSEDPARAM(ptr);
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  buffer[QMI_SWIAVMS_MSG_MAX]={'0'};
#if DEBUG_LOG_TO_FILE
    int is_matching = 1;
#endif

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
            qmi_msg = helper_get_resp_ctx(eSWIM2MCMD_AVC2, buffer, rlen, &rsp_ctx);
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
                        printf("unpack_swiavms_SLQSAVMSEventReportInd return: %d\n", unpackRetCode);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSEventReportInd",\
                                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                                    
                        local_fprintf("%s\n", "Correct");
                    #endif

                        swi_uint256_print_mask (swiavmsInd.ParamPresenceMask);
                        _print_swiavms_event_report(swiavmsInd);
                        accept_fota( BinaryUpdateSessionInfo.bUserInputRequest );
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
                        unpackRetCode = unpack_swiavms_SLQSAVMSStopSession_avc2 (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSStopSession_avc2",\
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

                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAVMSStopSession_avc2 return: %d\n", unpackRetCode);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SESSION_STOP
                    
                /** eQMI_SWI_M2M_AVMS_SET_SETTINGS **/
                case eQMI_SWI_M2M_AVMS_SET_SETTINGS:
                    {
                        unpack_swiavms_SLQSAVMSSetSettings_v2_t swiavmsRsp;
                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSSetSettings_v2_t));
                        unpackRetCode = unpack_swiavms_SLQSAVMSSetSettings_v2 (buffer, rlen, &swiavmsRsp);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSSetSettings_v2",\
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

                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);

                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 2))
                            printf("unpack_swiavms_SLQSAVMSSetSettings_v2 return: %d\n", unpackRetCode);
                    }
                    break; //eQMI_SWI_M2M_AVMS_SET_SETTINGS
                
                /** eQMI_SWI_M2M_AVMS_GET_SETTINGS **/
                case eQMI_SWI_M2M_AVMS_GET_SETTINGS:
                    {
                        unpack_swiavms_SLQSAVMSGetSettings_v2_t swiavmsRsp;
                        uint8_t autoReboot=0xFF;
                        uint8_t i=0;
                        PackSwiAVMSSettingsConnectionRetryTimers connectionRetryTimers = {.Timers = {0}};
                        uint32_t polling_timer = 0;
                        PackSwiAVMSSettingsAPNInfo               APNInfo;
                        PackSwiAVMSSettingsPeriodsInfo           PeriodsInfo;
                        uint8_t szPWD[LITEQMI_MAX_GET_SETTINGS_AVMS_PWD_STRING_LENGTH + 1]={0};
                        uint8_t szAPN[LITEQMI_MAX_GET_SETTINGS_AVMS_APN_STRING_LENGTH + 1]={0};
                        uint8_t szUname[LITEQMI_MAX_GET_SETTINGS_AVMS_UNAME_STRING_LENGTH + 1]={0};
                        uint8_t NotificationStore = 0;

                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSGetSettings_v2_t));
                        memset(&connectionRetryTimers,0,sizeof(PackSwiAVMSSettingsConnectionRetryTimers));
                        memset(&APNInfo,0,sizeof(PackSwiAVMSSettingsAPNInfo));
                        memset(&PeriodsInfo,0,sizeof(PackSwiAVMSSettingsPeriodsInfo));

                        /* 1 extra byte to NULL terminate string */
                        APNInfo.bAPNLength = LITEQMI_MAX_GET_SETTINGS_AVMS_APN_STRING_LENGTH + 1;
                        APNInfo.pAPN = &szAPN[0];
                        APNInfo.bPWDLength = LITEQMI_MAX_GET_SETTINGS_AVMS_PWD_STRING_LENGTH + 1;
                        APNInfo.pPWD = &szPWD[0];
                        APNInfo.bUnameLength = LITEQMI_MAX_GET_SETTINGS_AVMS_UNAME_STRING_LENGTH + 1;
                        APNInfo.pUname = &szUname[0];

                        memset(&swiavmsRsp,0,sizeof(unpack_swiavms_SLQSAVMSGetSettings_v2_t));
                        swiavmsRsp.pAutoReboot = &autoReboot;
                        swiavmsRsp.pPollingTimer = &polling_timer;
                        swiavmsRsp.pConnectionRetryTimers = &connectionRetryTimers;
                        swiavmsRsp.pAPNInfo               = &APNInfo;
                        swiavmsRsp.pPeroidsInfo           = &PeriodsInfo;
                        swiavmsRsp.pNotificationStore     = &NotificationStore;
                        unpackRetCode = unpack_swiavms_SLQSAVMSGetSettings_v2 (buffer, rlen, &swiavmsRsp);                        
                        swi_uint256_print_mask (swiavmsRsp.ParamPresenceMask);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSGetSettings_v2",\
                                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        checkErrorAndFormErrorString(buffer);
                        if ( (swiavmsRsp.OMADMEnabled != 0) ||
                             (swiavmsRsp.FwPromptdownload > 1) ||
                             (swiavmsRsp.FwPromptUpdate > 1) ||
                             (swiavmsRsp.AutoConnect > 1))
                        {
                            is_matching = 0;
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

                        printf("unpack_swiavms_SLQSAVMSGetSettings_v2 return: %d\n", unpackRetCode);
                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 1))
                            printf("\tOMADMEnabled: %u\n", swiavmsRsp.OMADMEnabled);
                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 6))
                            printf("\tAutoConnect: %u\n", swiavmsRsp.AutoConnect);
                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 3))
                            printf("\tFwAutodownload: %u\n", swiavmsRsp.FwPromptdownload);
                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 4))
                            printf("\tFwAutoupdate: %u\n", swiavmsRsp.FwPromptUpdate);
                        if(swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 5))
                            printf("\tFwAutoCheck: %u\n", swiavmsRsp.FwAutoSDM);
                        if ((swiavmsRsp.pAutoReboot) && (swi_uint256_get_bit(swiavmsRsp.ParamPresenceMask, 21)))
                            printf("\tFwAutoReboot: %u\n", *(swiavmsRsp.pAutoReboot));
                        if ( (swiavmsRsp.pPollingTimer) && (swi_uint256_get_bit(swiavmsRsp.ParamPresenceMask, 16)) )
                            printf("\tPolling Timer: %u\n", *swiavmsRsp.pPollingTimer);
                        if ( (swiavmsRsp.pConnectionRetryTimers) && (swi_uint256_get_bit(swiavmsRsp.ParamPresenceMask, 17)) ) {
                            printf("\tRetry Timers:\n");
                            for(i=0;i<8;i++)
                            {
                                printf("\t\t[%d]: %u\n", i,  swiavmsRsp.pConnectionRetryTimers->Timers[i]);
                            }
                        }
                        if((swiavmsRsp.pNotificationStore) &&
                           (swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 19)))
                            printf("\tNotification Status: 0x%x\n", *(swiavmsRsp.pNotificationStore));

                        if((swiavmsRsp.pAPNInfo) &&
                           (swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 18)))
                        {
                            printf("\tAPN Info\n");
                            printf("\t APN Length: %d\n",swiavmsRsp.pAPNInfo->bAPNLength);
                            if(swiavmsRsp.pAPNInfo->pAPN)
                                printf("\t APN:  %s\n",swiavmsRsp.pAPNInfo->pAPN);
                            printf("\t Uname Length: %d\n",swiavmsRsp.pAPNInfo->bUnameLength);
                            if(swiavmsRsp.pAPNInfo->pUname)
                                printf("\t Uname : %s\n",swiavmsRsp.pAPNInfo->pUname);
                            printf("\t PWD Length: %d\n",swiavmsRsp.pAPNInfo->bPWDLength);
                            if(swiavmsRsp.pAPNInfo->pPWD)
                                printf("\t PWD : %s\n",swiavmsRsp.pAPNInfo->pPWD);
                        }
                        if((swiavmsRsp.pPeroidsInfo) &&
                           (swi_uint256_get_bit (swiavmsRsp.ParamPresenceMask, 20)))
                        {
                            printf("Min/Max Peroid Info\n");
                            printf("\t Min: %d\n",swiavmsRsp.pPeroidsInfo->min);
                            printf("\t Max: %d\n",swiavmsRsp.pPeroidsInfo->max);
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
                        local_fprintf("%s,%s,", "unpack_swiavms_SLQSAVMSSendSelection",\
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

void swiavms_loop_avc2(void)
{
    uint8_t  buffer[QMI_SWIAVMS_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_SWIAVMS_MSG_MAX;
    pack_qmi_t req_ctx;
    uint16_t   txID = 0x101;
    int rtn = -1;
    const char *qmi_msg;
    printf("======swiavms_avc2 dummy unpack test===========\n");
    iLocalLog = 0;
    swiavms_avc2_validate_dummy_unpack();
    iLocalLog = 1;

    // Get swiavms FD
    swiavms_fd = client_fd(eSWIM2MCMD_AVC2);
    if(0>swiavms_fd)
    {
        fprintf (stderr, "Service(SWIAVMS) Not Supproted %d\n",eSWIM2MCMD_AVC2);
        return ;
    }
    sleep(1);

    printf("======SWIAVMS-avc2 pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "SWIAVMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif


    // Start swiavms read thread
    memset(&swiavms_attr, 0, sizeof(swiavms_attr));
    enswiavmsThread = 1;
    pthread_create(&swiavms_tid, &swiavms_attr, swiavms_read_thread, NULL);
    sleep(1);

    // Register for swiavms event notification
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAvmsSetEventReport (&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swiavms_SLQSAvmsSetEventReport ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAvmsSetEventReport", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(2);

    // AVMS Set Settings with optional auto reboot enabled
    {
        pack_swiavms_SLQSAVMSSetSettings_v2_t swiavms_req;
        uint8_t  autoReboot = 1;
        memset(&swiavms_req, 0, sizeof(swiavms_req));

        swiavms_req.pAutoReboot = &autoReboot;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettings_v2(&req_ctx,buffer,&reqLen, swiavms_req);
        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings_v2 with optional auto reboot enabled ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettings_v2", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(1);

    // AVMS Get Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings_v2(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings_v2 ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings_v2", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }    

    sleep(1);

    // AVMS Set Settings with optional auto reboot disabled
    {
        pack_swiavms_SLQSAVMSSetSettings_v2_t swiavms_req;
        memset(&swiavms_req, 0, sizeof(swiavms_req));
        uint8_t autoReboot = 0;

        uint32_t polling_timer = 200;
        PackSwiAvmsSetSettingsConnectionRetryTimers connectionRetryTimers
            = {.Timers = {1, 2, 3, 4, 5, 6, 7, 8}};

        swiavms_req.pAutoReboot = &autoReboot;
        swiavms_req.pPollingTimer = &polling_timer;
        swiavms_req.pConnectionRetryTimers = &connectionRetryTimers;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSSetSettings_v2(&req_ctx,buffer,&reqLen, swiavms_req);
        fprintf (stderr, "pack_swiavms_SLQSAVMSSetSettings_v2 with optional autoreboot disabled ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSSetSettings_v2", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }

    sleep(1);

    // AVMS Get Settings
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swiavms_SLQSAVMSGetSettings_v2(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swiavms_SLQSAVMSGetSettings_v2 ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSGetSettings_v2", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
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
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swiavms_SLQSAVMSStartSession(&req_ctx,buffer,&reqLen,swiavms_req);
        fprintf (stderr, "pack_swiavms_SLQSAVMSStartSession FOTA ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSStartSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }

    sleep(5);
    // Stop Session
    {
        pack_swiavms_SLQSAVMSStopSession_t swiavms_req;
        swiavms_req.sessionType = 0xFF; //Any ongoing session
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIAVMS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn =  pack_swiavms_SLQSAVMSStopSession(&req_ctx,buffer,&reqLen,swiavms_req);
        fprintf (stderr, "pack_swiavms_SLQSAVMSStoplSession (Any ) ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swiavms_SLQSAVMSStopSession", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(swiavms_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn!=reqLen)
        {
            goto swiavmsdemo_cleanup;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIM2MCMD_AVC2, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
    }
    sleep(5);
swiavmsdemo_cleanup:
    enswiavmsThread = 0;
    if(swiavms_fd>=0)
    {
        close(swiavms_fd);
    }
    sleep(5);
    return;
}

void dump_SLQSAVMSGetSettings_v2(void * ptr)
{
    uint8_t i=0;
    unpack_swiavms_SLQSAVMSGetSettings_v2_t *result =
            (unpack_swiavms_SLQSAVMSGetSettings_v2_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("unpack_swiavms_SLQSAVMSGetSettings_v2 return: %d\n", unpackRetCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tOMADMEnabled: %u\n", result->OMADMEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 6))
    printf("\tAutoConnect: %u\n", result->AutoConnect);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
    printf("\tFwAutodownload: %u\n", result->FwPromptdownload);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
    printf("\tFwAutoupdate: %u\n", result->FwPromptUpdate);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 5))
        printf("\tFwAutoCheck: %u\n", result->FwAutoSDM);
    if ((result->pAutoReboot) && (swi_uint256_get_bit(result->ParamPresenceMask, 21)))
        printf("\tFwAutoReboot: %u\n", *(result->pAutoReboot));
    if ( (result->pPollingTimer) && (swi_uint256_get_bit(result->ParamPresenceMask, 16)) )
        printf("\tPolling Timer: %u\n", *result->pPollingTimer);
    if ( (result->pConnectionRetryTimers) && (swi_uint256_get_bit(result->ParamPresenceMask, 17)) ) {
        printf("\tRetry Timers:\n");
        for(i=0;i<8;i++)
        {
            printf("\t\t[%d]: %u\n", i, result->pConnectionRetryTimers->Timers[i]);
        }
    }
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
    if((result->pPeroidsInfo) &&
       (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Min/Max Peroid Info\n");
        printf("\t Min: %d\n",result->pPeroidsInfo->min);
        printf("\t Max: %d\n",result->pPeroidsInfo->max);
    }
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_swiavms_avc2_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_SWI_M2M_AVMS_SESSION_STOP */
    {0x02,0x01,0x00,0xA1,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_SET_SETTINGS */
    {0x02,0x02,0x00,0xA5,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
    {0x02,0x03,0x00,0xA6,0x00,0x60,0x00,
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
};

/* eQMI_SWI_M2M_AVMS_SESSION_STOP */
const unpack_swiavms_SLQSAVMSStopSession_avc2_t const_unpack_swiavms_SLQSAVMSStopSession_avc2_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWI_M2M_AVMS_SET_SETTINGS */
const unpack_swiavms_SLQSAVMSSetSettings_v2_t const_unpack_swiavms_SLQSAVMSSetSettings_v2_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t  cst_PollingTimer_v2 = 0x12340000;
PackSwiAVMSSettingsConnectionRetryTimers cst_ConnectionRetryTimers_v2 =
        {{0x1122,0x3322,0x3344,0x4455,0x3322,0x2010,0x3020,0x5040}};
uint8_t    cst_getsettingsAPN_v2[] = "www";
uint8_t    cst_getsettingsUname_v2[] ="sierra";
uint8_t    cst_getsettingsPwd_v2[] = "sierra";
PackSwiAVMSSettingsAPNInfo cst_APNInfo_v2 =
        {0x03,cst_getsettingsAPN_v2,0x06,cst_getsettingsUname_v2,0x06,cst_getsettingsPwd_v2};
uint8_t    cst_NotificationStore_v2 = 1 ;
PackSwiAVMSSettingsPeriodsInfo  cst_PeroidsInfo_v2 ={0x3412,0x7856};
uint8_t         cst_AutoReboot_v2 = 1;


/* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
const unpack_swiavms_SLQSAVMSGetSettings_v2_t const_unpack_swiavms_SLQSAVMSGetSettings_v2_t = {
        0, 1, 1, 1, 0, &cst_PollingTimer_v2, &cst_ConnectionRetryTimers_v2, &cst_APNInfo_v2,
        &cst_NotificationStore_v2, &cst_PeroidsInfo_v2, &cst_AutoReboot_v2, 1,{{SWI_UINT256_BIT_VALUE(SET_12_BITS,2,1,3,4,5,6,16,17,18,19,20,21)}} };

uint32_t  var_PollingTimer_v2;
PackSwiAVMSSettingsConnectionRetryTimers var_ConnectionRetryTimers_v2;
uint8_t    getsettingsAPN_v2[49];
uint8_t    getsettingsUname_v2[29];
uint8_t    getsettingsPwd_v2[29];
PackSwiAVMSSettingsAPNInfo var_APNInfo_v2 =
        {49,getsettingsAPN_v2,29,getsettingsUname_v2,29,getsettingsPwd_v2};
uint8_t    var_NotificationStore_v2;
PackSwiAVMSSettingsPeriodsInfo  var_PeroidsInfo_v2;
uint8_t         var_AutoReboot_v2;

/* eQMI_SWI_M2M_AVMS_GET_SETTINGS */
unpack_swiavms_SLQSAVMSGetSettings_v2_t var_unpack_swiavms_SLQSAVMSGetSettings_v2_t = {
        0, 0, 0, 0, 0, &var_PollingTimer_v2, &var_ConnectionRetryTimers_v2, &var_APNInfo_v2,
        &var_NotificationStore_v2, &var_PeroidsInfo_v2,&var_AutoReboot_v2,0,{{0}} };


int swiavms_avc2_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swiavms_avc2_resp_msg)/sizeof(validate_swiavms_avc2_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swiavms_avc2_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eSWIM2MCMD_AVC2, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("eSWIM2MCMD_AVC2 IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("eSWIM2MCMD_AVC2 RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_SWI_M2M_AVMS_SESSION_STOP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSStopSession_avc2,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSStopSession_avc2_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_SET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSSetSettings_v2,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swiavms_SLQSAVMSSetSettings_v2_t);
                }
                break;
            case eQMI_SWI_M2M_AVMS_GET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swiavms_SLQSAVMSGetSettings_v2_t *varp = &var_unpack_swiavms_SLQSAVMSGetSettings_v2_t;
                    const unpack_swiavms_SLQSAVMSGetSettings_v2_t *cstp = &const_unpack_swiavms_SLQSAVMSGetSettings_v2_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiavms_SLQSAVMSGetSettings_v2,
                    dump_SLQSAVMSGetSettings_v2,
                    msg.buf,
                    rlen,
                    &var_unpack_swiavms_SLQSAVMSGetSettings_v2_t,
                    14,
                    CMP_PTR_TYPE, &varp->OMADMEnabled, &cstp->OMADMEnabled,
                    CMP_PTR_TYPE, &varp->AutoConnect, &cstp->AutoConnect,
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
                    CMP_PTR_TYPE, varp->pAutoReboot, cstp->pAutoReboot,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            }
        }
    }
    return 0;
}
