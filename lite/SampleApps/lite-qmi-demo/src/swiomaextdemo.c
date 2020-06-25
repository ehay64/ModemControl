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
#include "swiomaext.h"
#include "packingdemo.h"

#define QMI_SWIOMA_MSG_MAX QMI_MSG_MAX

#define LITEOMA_GET_UINT16_LE(p,val) \
    do { \
       (val) = ((p)[0] | (((p)[1] << 8) & 0xff00)); \
    } while(0)

// FUNCTION PROTOTYPES
int client_fd(uint8_t svc);

// SWIOMA GLOBALS
volatile int enSwiOmaExtThread;
int swiomaext_fd;
pthread_t swiomaext_tid = 0;
pthread_attr_t swiomaext_attr;

int swiomaext_validate_dummy_unpack();

// TEST VALUES
#ifdef SWIOMA_EVENTS_DEBUG
uint8_t swioma_event_sample[] = {};
#endif //SWIOMA_EVENTS_DEBUG

void swiomaext_loop_exit(void)
{
    if (enSwiOmaExtThread == 1)
    {
        printf("\nkilling SWIOMAEXT read thread...\n");
        enSwiOmaExtThread = 0;
#ifdef __ARM_EABI__
        if(swiomaext_fd>=0)
            close(swiomaext_fd);
        swiomaext_fd=-1;
        void *pthread_rtn_value;
        if(swiomaext_tid!=0)
        pthread_join(swiomaext_tid, &pthread_rtn_value);
#endif
        if(swiomaext_tid!=0)
        pthread_cancel(swiomaext_tid);
        swiomaext_tid = 0;
         if(swiomaext_fd>=0)
            close(swiomaext_fd);
        swiomaext_fd=-1;
    }
    return;
}

void dump_swioma_SLQSOMADMGetSessionInfoExt(void* ptr)
{
    unpack_swioma_SLQSOMADMGetSessionInfoExt_t *pSLQSOMADMGetSessionInfoExt =
        (unpack_swioma_SLQSOMADMGetSessionInfoExt_t*) ptr;
    uint8_t ctr = 0;
    uint16_t lWord= 0;

    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pSLQSOMADMGetSessionInfoExt->ParamPresenceMask);
    if(swi_uint256_get_bit (pSLQSOMADMGetSessionInfoExt->ParamPresenceMask, 16))
    {
        printf("OMADM Session Info Ext \n" );

        printf("\nStatus : %d",     pSLQSOMADMGetSessionInfoExt->status );
        printf("\nSession state : %d",    pSLQSOMADMGetSessionInfoExt->sessionState );
        printf("\nHFA status : %d",    pSLQSOMADMGetSessionInfoExt->hfaStatus );

        printf("\nHFC max retries : %d",  pSLQSOMADMGetSessionInfoExt->hfaMaxRetry );
        printf("\nHFA retry interval (seconds) : %d", pSLQSOMADMGetSessionInfoExt->hfaRetryInterval );
        printf("\nHFA retry index : %d",       pSLQSOMADMGetSessionInfoExt->hfaRetryIndex );

        printf("\nFumo state : %d",       pSLQSOMADMGetSessionInfoExt->fumoState );
        printf("\nFumo result code : %d",      pSLQSOMADMGetSessionInfoExt->fumoResultCode );

        printf("\nPackage vendor name length : %d", pSLQSOMADMGetSessionInfoExt->pkgVendorNameLength );
        printf("\nPackage vendor name : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgVendorNameLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgVendorName[ctr],lWord);
            printf("%04x ", lWord);
        }

        printf("\nPackage size : %d",      pSLQSOMADMGetSessionInfoExt->pkgSize );

        printf("\nPackage version name length : %d", pSLQSOMADMGetSessionInfoExt->pkgVersionNameLength );
        printf( "\nPackage version name : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgVersionNameLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgVersionName[ctr],lWord);
            printf( "%04x ", lWord);
        }

        printf("\nPackage name length : %d", pSLQSOMADMGetSessionInfoExt->pkgNameLength );
        printf("\nPackage name : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgNameLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgName[ctr],lWord);
            printf("%04x ", lWord);
        }

        printf("\nPackage desc length : %d", pSLQSOMADMGetSessionInfoExt->pkgDescLength );
        printf("\nPackage desc : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgDescLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgDesc[ctr],lWord);
            printf("%04x ", lWord);
        }

        printf("\nPackage date length : %d", pSLQSOMADMGetSessionInfoExt->pkgDateLength );
        printf("\nPackage date : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgDateLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgDate[ctr],lWord);
            printf("%04x ", lWord);
        }

        printf("\nPackage install time length : %d", pSLQSOMADMGetSessionInfoExt->pkgInstallTimeLength );
        printf("\nPackage install time : " );
        for( ctr = 0; ctr < (2* (pSLQSOMADMGetSessionInfoExt->pkgInstallTimeLength)); ctr += 2 )
        {
            LITEOMA_GET_UINT16_LE(&pSLQSOMADMGetSessionInfoExt->pkgInstallTime[ctr],lWord);
            printf("%04x ", lWord);
        }
        printf("\n");
    }
}

void *swiomaext_read_thread(void* ptr)
{
    UNUSEDPARAM(ptr);
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  buffer[QMI_SWIOMA_MSG_MAX]={'0'};

    int rtn;
    ssize_t rlen;
    printf ("SWIOMAEXT read thread\n");
    sleep(1);
    while(enSwiOmaExtThread)
    {
        //TODO select multiple file and read them
        memset (buffer,0,QMI_SWIOMA_MSG_MAX);
        rtn = rlen = read(swiomaext_fd, buffer, QMI_SWIOMA_MSG_MAX);
        if ((rtn > 0) && (rlen > 0))
        {
            qmi_msg = helper_get_resp_ctx(eSWIOMAEXT, buffer, rlen, &rsp_ctx);
            printf("<< receiving %s, Len: %zu\n", qmi_msg, rlen);
            dump_hex(rlen, buffer);

            if (eIND == rsp_ctx.type)
                printf("SWIOMAEXT IND: ");
            else if (eRSP == rsp_ctx.type)
                printf("SWIOMAEXT RSP: ");
            printf("msgid 0x%02x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                /** eQMI_SWIOMA_START_SESSION **/
                case eQMI_SWIOMA_START_SESSION:
                    {
                        unpack_swioma_SLQSOMADMStartSessionExt_t startSessionExtResp;
                        rtn = unpack_swioma_SLQSOMADMStartSessionExt (buffer, rlen, &startSessionExtResp);
                        swi_uint256_print_mask (startSessionExtResp.ParamPresenceMask);
                        if(swi_uint256_get_bit (startSessionExtResp.ParamPresenceMask, 2))
                            printf("unpack_swioma_SLQSOMADMStartSession return: %d\n", startSessionExtResp.Tlvresult);
                    }
                    break; //eQMI_SWIOMA_START_SESSION

                /** eQMI_SWIOMA_CANCEL_SESSION **/
                case eQMI_SWIOMA_CANCEL_SESSION:
                    {
                        unpack_swioma_SLQSOMADMCancelSessionExt_t cancelSessionExtResp;
                        rtn = unpack_swioma_SLQSOMADMCancelSessionExt (buffer, rlen, &cancelSessionExtResp);
                        swi_uint256_print_mask (cancelSessionExtResp.ParamPresenceMask);
                        if(swi_uint256_get_bit (cancelSessionExtResp.ParamPresenceMask, 2))
                            printf("unpack_swioma_SLQSOMADMCancelSession return: %d\n", cancelSessionExtResp.Tlvresult);
                    }
                    break; //eQMI_SWIOMA_CANCEL_SESSION

                /** eQMI_SWIOMA_SET_SETTINGS **/
                case eQMI_SWIOMA_SET_SETTINGS:
                    {
                        unpack_swioma_SLQSOMADMSetSettingsExt_t setSettingsExtResp;
                        rtn = unpack_swioma_SLQSOMADMSetSettingsExt (buffer, rlen, &setSettingsExtResp);
                        swi_uint256_print_mask (setSettingsExtResp.ParamPresenceMask);
                        if(swi_uint256_get_bit (setSettingsExtResp.ParamPresenceMask, 2))
                            printf("unpack_swioma_SLQSOMADMSetSettings return: %d\n", setSettingsExtResp.Tlvresult);
                    }
                    break; //eQMI_SWIOMA_SET_SETTINGS
                    
                /** eQMI_SWIOMA_SEND_SELECTION **/
                case eQMI_SWIOMA_SEND_SELECTION:
                    {
                        unpack_swioma_SLQSOMADMSendSelectionExt_t sendSelectionExtResp;
                        rtn = unpack_swioma_SLQSOMADMSendSelectionExt (buffer, rlen, &sendSelectionExtResp);
                        swi_uint256_print_mask (sendSelectionExtResp.ParamPresenceMask);
                        if(swi_uint256_get_bit (sendSelectionExtResp.ParamPresenceMask, 2))
                            printf("unpack_swioma_SLQSOMADMSendSelection return: %d\n", sendSelectionExtResp.Tlvresult);
                    }
                    break; //eQMI_SWIOMA_SEND_SELECTION
                /** eQMI_SWIOMA_GET_SESSION_INFO **/
                case eQMI_SWIOMA_GET_SESSION_INFO:
                    {
                        unpack_swioma_SLQSOMADMGetSessionInfoExt_t SwiOmaRsp;
                        memset (&SwiOmaRsp,0,sizeof(unpack_swioma_SLQSOMADMGetSessionInfoExt_t));
                        rtn = unpack_swioma_SLQSOMADMGetSessionInfoExt (buffer, rlen, &SwiOmaRsp);
                        printf("unpack_swioma_SLQSOMADMGetSessionInfoExt return: %d\n", rtn);
                        dump_swioma_SLQSOMADMGetSessionInfoExt((void*)&SwiOmaRsp);
                    }
                    break; //eQMI_SWIOMA_GET_SESSION_INFO

                default:
                    break;
            }
        }
    }    
    return NULL;
}

void swiomaext_loop(void)
{
    uint8_t  buffer[QMI_SWIOMA_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_SWIOMA_MSG_MAX;
    pack_qmi_t req_ctx;
    uint16_t   txID = 0x101;
    int rtn = -1;
    const char *qmi_msg;

    printf("======SWIOMAEXT dummy unpack test===========\n");
    swiomaext_validate_dummy_unpack();
    printf("======SWIOMAEXT pack/unpack test===========\n");

    // Get SWIOMA FD
    swiomaext_fd = client_fd(eSWIOMAEXT); 
    if(swiomaext_fd<0)
    {
        fprintf(stderr,"SWI OMA Ext Service Not Supported!\n");
        return ;
    }
    sleep(1);
    // Start SWIOMA read thread
    memset(&swiomaext_attr, 0, sizeof(swiomaext_attr));
    enSwiOmaExtThread = 1;
    pthread_create(&swiomaext_tid, &swiomaext_attr, swiomaext_read_thread, NULL);

    usleep(500);

    // Start CI-PRL Session
    {
    pack_swioma_SLQSOMADMStartSessionExt_t swioma_req;
    swioma_req.sessionType = 0x03; //CI PRL
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = txID;
    txID++;
    reqLen = QMI_SWIOMA_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn =  pack_swioma_SLQSOMADMStartSessionExt(&req_ctx,buffer,&reqLen,swioma_req);
    fprintf (stderr, "pack_swioma_SLQSOMADMStartSessionExt (CI-PRL) ret: %d, Len: %d\n", rtn, reqLen);
    rtn = write(swiomaext_fd, buffer, reqLen);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn == reqLen)
    {
        qmi_msg = helper_get_req_str(eSWIOMAEXT, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    }

    sleep(25);

    // OMA Get Session Info
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = txID;
        txID++;
        reqLen = QMI_SWIOMA_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_swioma_SLQSOMADMGetSessionInfoExt(&req_ctx,buffer,&reqLen);
        fprintf (stderr, "pack_swioma_SLQSOMADMGetSessionInfoExt ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_swioma_SLQSOMADMGetSessionInfoExt", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif
        rtn = write(swiomaext_fd, buffer, reqLen);
        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn == reqLen)
        {
            qmi_msg = helper_get_req_str(eSWIOMAEXT, buffer, reqLen);
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
    pack_swioma_SLQSOMADMCancelSessionExt_t swioma_req;
    swioma_req.sessionType = 0xFF; //Any OMA session
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = txID;
    txID++;
    reqLen = QMI_SWIOMA_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn =  pack_swioma_SLQSOMADMCancelSessionExt(&req_ctx,buffer,&reqLen,swioma_req);
    fprintf (stderr, "pack_swioma_SLQSOMADMCancelSessionExt (Any OMA) ret: %d, Len: %d\n", rtn, reqLen);
    rtn = write(swiomaext_fd, buffer, reqLen);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn == reqLen)
    {
        qmi_msg = helper_get_req_str(eSWIOMAEXT, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    }

    sleep(25);
    // OMA Set Settings
    {
    pack_swioma_SLQSOMADMSetSettingsExt_t swioma_req;
    swioma_req.FOTAdownload = 0;
    swioma_req.FOTAUpdate   = 0;
    swioma_req.OMADMEnable = 0;
    swioma_req.OMADMLogEnable = 0;
    swioma_req.FUMOEnable = 0;
    swioma_req.PRLEnable = 1;

    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = txID;
    txID++;
    reqLen = QMI_SWIOMA_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn = pack_swioma_SLQSOMADMSetSettingsExt(&req_ctx,buffer,&reqLen,swioma_req);
    fprintf (stderr, "pack_swioma_SLQSOMADMSetSettingsExt ret: %d, Len: %d\n", rtn, reqLen);
    rtn = write(swiomaext_fd, buffer, reqLen);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn == reqLen)
    {
        qmi_msg = helper_get_req_str(eSWIOMAEXT, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    }    

    sleep(30);

    // OMA Send Selection
    {
    pack_swioma_SLQSOMADMSendSelectionExt_t  swioma_req;
    swioma_req.selection     = 0x01; // Accept
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = txID;
    txID++;
    reqLen = QMI_SWIOMA_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn = pack_swioma_SLQSOMADMSendSelectionExt(&req_ctx,buffer,&reqLen,swioma_req);
    fprintf (stderr, "pack_swioma_SLQSOMADMSendSelectionExt ret: %d, Len: %d\n", rtn, reqLen);
    rtn = write(swiomaext_fd, buffer, reqLen);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn == reqLen)
    {
        qmi_msg = helper_get_req_str(eSWIOMAEXT, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    }
    sleep(25);
    return;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_swiomaext_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_SWIOMA_START_SESSION */
    {0x02,0x01,0x00,0x01,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIOMA_CANCEL_SESSION */
    {0x02,0x02,0x00,0x02,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIOMA_SET_SETTINGS */
    {0x02,0x03,0x00,0x07,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIOMA_SEND_SELECTION */
    {0x02,0x04,0x00,0x05,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIOMA_GET_SESSION_INFO */
    {0x02,0x05,0x00,0x03,0x00,0x64,0x00,
     0x02,0x04,0x00,0x00,0x00,0x00,0x00,
     0x10,0x5A,0x00,
     0x01,0x01,0x01,0x03,0x00,0x05,0x00,0x02,0x00, 
     0x01,0xFA,0x00,0x00,0x00,
     0x05,0x00,0x61,0x00,0x62,0x00,0x63,0x00,0x64,0x00,0x65,0x00,
     0xA2,0x23,0x00,0x00,
     0x05,0x00,0x61,0x00,0x62,0x00,0x63,0x00,0x64,0x00,0x65,0x00,
     0x05,0x00,0x66,0x00,0x67,0x00,0x68,0x00,0x69,0x00,0x70,0x00,
     0x05,0x00,0x71,0x00,0x72,0x00,0x73,0x00,0x74,0x00,0x75,0x00,
     0x05,0x00,0x31,0x00,0x32,0x00,0x33,0x00,0x34,0x00,0x35,0x00,
     0x05,0x00,0x35,0x00,0x36,0x00,0x37,0x00,0x38,0x00,0x39,0x00,
     },
   };

/* eQMI_SWIOMA_START_SESSION */
const unpack_swioma_SLQSOMADMStartSessionExt_t const_unpack_swioma_SLQSOMADMStartSessionExt_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIOMA_CANCEL_SESSION */
const unpack_swioma_SLQSOMADMCancelSessionExt_t const_unpack_swioma_SLQSOMADMCancelSessionExt_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIOMA_SET_SETTINGS */
const unpack_swioma_SLQSOMADMSetSettingsExt_t const_unpack_swioma_SLQSOMADMSetSettingsExt_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIOMA_SEND_SELECTION */
const unpack_swioma_SLQSOMADMSendSelectionExt_t const_unpack_swioma_SLQSOMADMSendSelectionExt_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIOMA_GET_SESSION_INFO */
const unpack_swioma_SLQSOMADMGetSessionInfoExt_t const_unpack_swioma_SLQSOMADMGetSessionInfoExt_t = {
        1,1,1,3,5,2,
        1,0x000000FA,0x0005,{0x61,0x00,0x62,0x00,0x63,0x00,0x64,0x00,0x65,0x00},0x000023A2,
        0x0005,{0x61,0x00,0x62,0x00,0x63,0x00,0x64,0x00,0x65,0x00},
        0x0005,{0x66,0x00,0x67,0x00,0x68,0x00,0x69,0x00,0x70,0x00},
        0x0005,{0x71,0x00,0x72,0x00,0x73,0x00,0x74,0x00,0x75,0x00},
        0x0005,{0x31,0x00,0x32,0x00,0x33,0x00,0x34,0x00,0x35,0x00},
        0x0005,{0x35,0x00,0x36,0x00,0x37,0x00,0x38,0x00,0x39,0x00},
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };


int swiomaext_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swiomaext_resp_msg)/sizeof(validate_swiomaext_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swiomaext_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_SWIOMA_MDM6600, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWIOMAEXT IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIOMAEXT RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {

            case eQMI_SWIOMA_START_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMStartSessionExt,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMStartSessionExt_t);
                }
                break;
            case eQMI_SWIOMA_CANCEL_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMCancelSessionExt,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMCancelSessionExt_t);
                }
                break;
            case eQMI_SWIOMA_SET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMSetSettingsExt,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMSetSettingsExt_t);
                }
                break;
            case eQMI_SWIOMA_SEND_SELECTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMSendSelectionExt,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMSendSelectionExt_t);
                }
                break;
            case eQMI_SWIOMA_GET_SESSION_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swioma_SLQSOMADMGetSessionInfoExt,
                    dump_swioma_SLQSOMADMGetSessionInfoExt,
                    msg.buf,
                    rlen,
                    &const_unpack_swioma_SLQSOMADMGetSessionInfoExt_t);
                }
                break;
            }
        }
    }
    return 0;
}


