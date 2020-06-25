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
#include "sar.h"

int sar_validate_dummy_unpack();
int sar = -1;
////Modem Settings
pack_sar_SLQSSetRfSarState_t SLQSSetRfSarStateSettings = {
        4 //QMI_SAR_RF_STATE_4
};
/////////////////
uint32_t SarRFState = 0xFFFFFFFF;
unpack_sar_SLQSGetRfSarState_t tunpack_sar_SLQSGetRfSarState = {&SarRFState, {{0}} };

pack_sar_SLQSSetRfSarState_t tpack_sar_SLQSSetRfSarState = {
        4 //QMI_SAR_RF_STATE_4
    };
unpack_sar_SLQSSetRfSarState_t tunpack_sar_SLQSSetRfSarState = {0, {{0}} };

void dump_SLQSGetRfSarState(void *ptr)
{
    unpack_sar_SLQSGetRfSarState_t *result =
            (unpack_sar_SLQSGetRfSarState_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s\n",  "N/A");
#endif
    if( CHECK_PTR_IS_NOT_NULL(result->pSarRFState) &&
        swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf( "RF SAR State : %x\n", *result->pSarRFState);
}

void dump_SLQSSetRfSarState(void *ptr)
{
    unpack_sar_SLQSSetRfSarState_t *result =
            (unpack_sar_SLQSSetRfSarState_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetRfSarStateSettings(void *ptr)
{
    unpack_sar_SLQSGetRfSarState_t *result =
            (unpack_sar_SLQSGetRfSarState_t*) ptr;
    memset(&SLQSSetRfSarStateSettings,0,sizeof(pack_sar_SLQSSetRfSarState_t));
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if(CHECK_PTR_IS_NOT_NULL(result->pSarRFState))
        {
            printf( "RF SAR State : %x\n", *result->pSarRFState);
            SLQSSetRfSarStateSettings.RfSarState = *result->pSarRFState;
        }        
    }
}

testitem_t sartotest[] = {
    {
        (pack_func_item) &pack_sar_SLQSGetRfSarState, "pack_sar_SLQSGetRfSarState get setttings",
        NULL,
        (unpack_func_item) &unpack_sar_SLQSGetRfSarState, "unpack_sar_SLQSGetRfSarState get setttings",
        &tunpack_sar_SLQSGetRfSarState, dump_SLQSGetRfSarStateSettings
    },
    {
        (pack_func_item) &pack_sar_SLQSSetRfSarState, "pack_sar_SLQSSetRfSarState",
        &tpack_sar_SLQSSetRfSarState,
        (unpack_func_item) &unpack_sar_SLQSSetRfSarState, "unpack_sar_SLQSSetRfSarState",
        &tunpack_sar_SLQSSetRfSarState, dump_SLQSSetRfSarState
    },
    {
        (pack_func_item) &pack_sar_SLQSGetRfSarState, "pack_sar_SLQSGetRfSarState",
        NULL,
        (unpack_func_item) &unpack_sar_SLQSGetRfSarState, "unpack_sar_SLQSGetRfSarState",
        &tunpack_sar_SLQSGetRfSarState, dump_SLQSGetRfSarState
    },
    {
        (pack_func_item) &pack_sar_SLQSSetRfSarState, "pack_sar_SLQSSetRfSarState restore settings",
        &SLQSSetRfSarStateSettings,
        (unpack_func_item) &unpack_sar_SLQSSetRfSarState, "unpack_sar_SLQSSetRfSarState restore settings",
        &tunpack_sar_SLQSSetRfSarState, dump_SLQSSetRfSarState
    },
};

unsigned int sararraylen = (unsigned int)((sizeof(sartotest))/(sizeof(sartotest[0])));

testitem_t sartotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_sar_SLQSGetRfSarState, "pack_sar_SLQSGetRfSarState get setttings",
        NULL,
        (unpack_func_item) &unpack_sar_SLQSGetRfSarState, "unpack_sar_SLQSGetRfSarState get setttings",
        NULL, dump_SLQSGetRfSarStateSettings
    },
};


void sar_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======SAR pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(sartotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(sartotest_invalidunpack[i].dump!=NULL)
        {
            sartotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(sartotest)/sizeof(testitem_t); i++)
    {
        if(sartotest[i].dump!=NULL)
        {
            sartotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(sartotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(sartotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,sartotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(sar<0)
            sar = client_fd(eSAR);
        if(sar<0)
        {
            fprintf(stderr,"SAR Service Not Supported!\n");
            return ;
        }
        rtn = write(sar, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(sar>=0)
                close(sar);
            sar=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSAR, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(sar, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eSAR, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(sartotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                sartotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           sartotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    sartotest_invalidunpack[i].dump(sartotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(sar>=0)
                close(sar);
            sar=-1;
        }
        sleep(1);
        xid++;
    }
    if(sar>=0)
        close(sar);
    sar=-1;
}

void sar_test_pack_unpack_loop()
{
    unsigned i;

    printf("======SAR dummy unpack test===========\n");
    sar_validate_dummy_unpack();
    printf("======SAR pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "SAR Pack/UnPack API Name", 
                      "Status",
                      "Unpack Payload Parsing");
#endif

    unsigned xid =1;
    for(i=0; i<sizeof(sartotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(sartotest[i].pack)(&req_ctx, req, &reqLen,sartotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", sartotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(sar<0)
            sar = client_fd(eSAR);
        if(sar<0)
        {
            fprintf(stderr,"SAR Service Not Supported!\n");
            return ;
        }
        rtn = write(sar, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSAR, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(sar, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eSAR, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("SAR RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(sartotest[i].unpack)(rsp, rspLen, sartotest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,%s,", sartotest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "");
                    #endif

                        sartotest[i].dump(sartotest[i].unpack_ptr);
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
                    }
                }
                else if (rsp_ctx.type == eIND)
                {
                    printf("SAR IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(sar>=0)
        close(sar);
    sar=-1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_sar_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_SAR_RF_SET_STATE*/
    {0x02,0x01,0x00,0x01,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SAR_RF_GET_STATE*/
    {0x02,0x02,0x00,0x02,0x00,0x0E,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
     0x00,0x05,0x00,0x00,0x00 },
   };

/* eQMI_SAR_RF_SET_STATE */
const unpack_sar_SLQSSetRfSarState_t const_unpack_sar_SLQSSetRfSarState_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SAR_RF_GET_STATE */
uint32_t cst_RFState = 0x05;
const unpack_sar_SLQSGetRfSarState_t const_unpack_sar_SLQSGetRfSarState_t = {
        &cst_RFState,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2, 16)}} };

uint32_t var_RFState;
unpack_sar_SLQSGetRfSarState_t var_unpack_sar_SLQSGetRfSarState_t = {
        &var_RFState, {{0}}};

int sar_validate_dummy_unpack()
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
    loopCount = sizeof(validate_sar_resp_msg)/sizeof(validate_sar_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_sar_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_SAR, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SAR IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SAR RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_SAR_RF_SET_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sar_SLQSSetRfSarState,
                    dump_SLQSSetRfSarState,
                    msg.buf,
                    rlen,
                    &const_unpack_sar_SLQSSetRfSarState_t);
                }
                break;
            case eQMI_SAR_RF_GET_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_sar_SLQSGetRfSarState_t *varp = &var_unpack_sar_SLQSGetRfSarState_t;
                    const unpack_sar_SLQSGetRfSarState_t *cstp = &const_unpack_sar_SLQSGetRfSarState_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_sar_SLQSGetRfSarState,
                    dump_SLQSGetRfSarState,
                    msg.buf,
                    rlen,
                    &var_unpack_sar_SLQSGetRfSarState_t,
                    2,
                    CMP_PTR_TYPE, varp->pSarRFState, cstp->pSarRFState,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                break;
            }
        }
    }
    return 0;
}

