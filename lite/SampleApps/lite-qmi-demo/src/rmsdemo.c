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
#include "rms.h"

int rms = -1;
int rms_validate_dummy_unpack();

unpack_rms_GetSMSWake_t tunpack_rms_GetSMSWake = {0, 0, SWI_UINT256_INT_VALUE};
unpack_rms_SetSMSWake_t tunpack_rms_SetSMSWake = {SWI_UINT256_INT_VALUE};

pack_rms_SetSMSWake_t tpack_rms_SetSMSWake = {
        0, //enable
        0  //wake mask
    };
////Modem Settings
pack_rms_SetSMSWake_t SMSWakeSettings;
//////////////////////////////////////////
void rms_dummy_unpack();

void dump_GetSMSWake(void *ptr)
{
    unpack_rms_GetSMSWake_t *result =
            (unpack_rms_GetSMSWake_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
        printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(result)
    {
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
            printf( "RMS GetSMSWake enabled: %x\n", result->enabled);
        if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
            printf( "RMS GetSMSWake wake mask: %x\n", result->wake_mask);
    }
}

void dump_SetSMSWake(void *ptr)
{
    unpack_rms_SetSMSWake_t *result =
            (unpack_rms_SetSMSWake_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    if(result)
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetSMSWakeSettings(void *ptr)
{
    unpack_rms_GetSMSWake_t *result =
            (unpack_rms_GetSMSWake_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    memset(&SMSWakeSettings,0,sizeof(pack_rms_SetSMSWake_t));
        printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(result)
    {
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        {
            printf( "RMS GetSMSWake enabled: %x\n", result->enabled);
            SMSWakeSettings.enabled = result->enabled;
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        {
            printf( "RMS GetSMSWake wake mask: %x\n", result->wake_mask);
            SMSWakeSettings.wake_mask = result->wake_mask;
        }
    }
}

testitem_t rmstotest[] = {
    {
        (pack_func_item) &pack_rms_GetSMSWake, "pack_rms_GetSMSWake Get Modem Settings",
        NULL,
        (unpack_func_item) &unpack_rms_GetSMSWake, "unpack_rms_GetSMSWake Get Modem Settings",
        &tunpack_rms_GetSMSWake, dump_GetSMSWakeSettings
    },
    {
        (pack_func_item) &pack_rms_SetSMSWake, "pack_rms_SetSMSWake",
        &tpack_rms_SetSMSWake,
        (unpack_func_item) &unpack_rms_SetSMSWake, "unpack_rms_SetSMSWake",
        &tunpack_rms_SetSMSWake, dump_SetSMSWake
    },
    {
        (pack_func_item) &pack_rms_GetSMSWake, "pack_rms_GetSMSWake",
        NULL,
        (unpack_func_item) &unpack_rms_GetSMSWake, "unpack_rms_GetSMSWake",
        &tunpack_rms_GetSMSWake, dump_GetSMSWake
    },
    {
        (pack_func_item) &pack_rms_SetSMSWake, "pack_rms_SetSMSWake retore modem settings",
        &SMSWakeSettings,
        (unpack_func_item) &unpack_rms_SetSMSWake, "unpack_rms_SetSMSWake retore modem settings",
        &tunpack_rms_SetSMSWake, dump_SetSMSWake
    },
};

unsigned int rmsarraylen = (unsigned int)((sizeof(rmstotest))/(sizeof(rmstotest[0])));

testitem_t rmstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_rms_GetSMSWake, "pack_rms_GetSMSWake Get Modem Settings",
        NULL,
        (unpack_func_item) &unpack_rms_GetSMSWake, "unpack_rms_GetSMSWake Get Modem Settings",
        NULL, dump_GetSMSWakeSettings
    }
};

void rms_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======RMS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(rmstotest)/sizeof(testitem_t); i++)
    {
        if(rmstotest[i].dump!=NULL)
        {
            rmstotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(rmstotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(rmstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,rmstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(rms<0)
            rms = client_fd(eRMS);
        if(rms<0)
        {
            fprintf(stderr,"RMS Service Not Supported!\n");
            return ;
        }
        rtn = write(rms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(rms>=0)
                close(rms);
            rms=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eRMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(rms, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg =  helper_get_resp_ctx(eRMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(rmstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                rmstotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           rmstotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    rmstotest_invalidunpack[i].dump(rmstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(rms>=0)
                close(rms);
            rms=-1;
        }
        sleep(1);
        xid++;
    }
    if(rms>=0)
        close(rms);
    rms=-1;
}

void rms_test_pack_unpack_loop()
{
    unsigned i;
    printf("\n======RMS dummy unpack and compare test===========\n");
    rms_validate_dummy_unpack();
    printf("======RMS dummy pack/unpack test===========\n");
    rms_dummy_unpack();
    printf("======RMS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "RMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing");
#endif
    if(rms<0)
        rms = client_fd(eRMS);
    if(rms<0)
    {
        fprintf(stderr,"RMS Service Not Supported\n");
        return ;
    }
    unsigned xid =1;
    for(i=0; i<sizeof(rmstotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(rmstotest[i].pack)(&req_ctx, req, &reqLen,rmstotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", rmstotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

       
        rtn = write(rms, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eRMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(rms, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eRMS, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("RMS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(rmstotest[i].unpack)(rsp, rspLen, rmstotest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,%s", rmstotest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "");
                    #endif

                        rmstotest[i].dump(rmstotest[i].unpack_ptr);
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
                    printf("RMS IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(rms>=0)
        close(rms);
    rms=-1;
}
uint8_t dummy_rms_resp_msg[][QMI_MSG_MAX] = { 
    {// eQMI_RMS_GET_SMS_WAKE
        0x02,0x03,0x00,0x20,0x00,0x12,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00,0x11,0x04,
        0x00,0x00,0x00,0x00,0x00,0x10,0x01,0x00,
        0x00
    },
    {// eQMI_RMS_GET_SMS_WAKE
        0x02,0x03,0x00,0x20,0x00,0x12,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00,0x11,0x04,
        0x00,0x00,0x00,0x00,0x00,0x10,0x01,0x00,
        0x00
    },
    {// eQMI_RMS_GET_SMS_WAKE
        0x02,0x01,0x00,0x20,0x00,0x12,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00,0x11,0x04,
        0x00,0x5A,0xA5,0x5A,0xA5,0x10,0x01,0x00,
        0x01
    },
    {// eQMI_RMS_GET_SMS_WAKE
        0x02,0x01,0x00,0x20,0x00,0x12,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00,0x11,0x04,
        0x00,0xA5,0x5A,0xA5,0x5A,0x10,0x01,0x00,
        0x00
    },
    {// eQMI_RMS_GET_SMS_WAKE
        0x02,0x01,0x00,0x20,0x00,0x07,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00
    },
    {// eQMI_RMS_SET_SMS_WAKE
        0x02,0x04,0x00,0x21,0x00,0x07,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00
    },
    {// eQMI_RMS_SET_SMS_WAKE
        0x02,0x02,0x00,0x21,0x00,0x07,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00
    }
};

void rms_dummy_unpack()
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;
    ssize_t rlen;
    pack_qmi_t req_ctx;
    int loopCount = 0;
    int index = 0;
    int unpackRetCode = 0;
    memset(&req_ctx, 0, sizeof(req_ctx));
    loopCount = sizeof(dummy_rms_resp_msg)/sizeof(dummy_rms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index>=loopCount)
            return ;
        memcpy(&msg.buf,&dummy_rms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)(rtn);
            qmi_msg = helper_get_resp_ctx(eRMS, msg.buf, rlen, &rsp_ctx);

            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("SMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                case eQMI_RMS_SET_SMS_WAKE:
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_rms_SetSMSWake,
                        dump_SetSMSWake,
                        msg.buf, 
                        rlen);
                    break;
                case eQMI_RMS_GET_SMS_WAKE:
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_rms_GetSMSWake,
                        dump_GetSMSWake,
                        msg.buf, 
                        rlen);
                    break;
                default:
                    break;
            }
         }
    }
    return ;
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_rms_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_RMS_GET_SMS_WAKE */
    {0x02,0x01,0x00,0x20,0x00,0x12,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x01,
            0x11,0x04,0x00,0x01,0x11,0x00,0x00},

    /* eQMI_RMS_SET_SMS_WAKE */
    {0x02,0x02,0x00,0x21,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00 },
   };

/* eQMI_RMS_GET_SMS_WAKE */
const unpack_rms_GetSMSWake_t const_unpack_rms_GetSMSWake_t = {
        1,0x1101,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_RMS_SET_SMS_WAKE */
const unpack_rms_SetSMSWake_t const_unpack_rms_SetSMSWake_t = {
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

int rms_validate_dummy_unpack()
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
    loopCount = sizeof(validate_rms_resp_msg)/sizeof(validate_rms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_rms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_RMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("RMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("RMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {

            case eQMI_RMS_GET_SMS_WAKE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_rms_GetSMSWake,
                    dump_GetSMSWake,
                    msg.buf,
                    rlen,
                    &const_unpack_rms_GetSMSWake_t);
                }
                break;
            case eQMI_RMS_SET_SMS_WAKE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_rms_SetSMSWake,
                    dump_SetSMSWake,
                    msg.buf,
                    rlen,
                    &const_unpack_rms_SetSMSWake_t);
                }
                break;
            }
        }
    }
    return 0;
}


