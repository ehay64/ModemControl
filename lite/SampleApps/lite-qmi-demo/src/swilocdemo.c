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

#include "swiloc.h"

int swiloc_validate_dummy_unpack();
int swiloc=-1;
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;

unpack_swiloc_SwiLocGetAutoStart_t tunpack_swiloc_SwiLocGetAutoStart;
pack_swiloc_SwiLocSetAutoStart_t tpack_swiloc_SwiLocSetAutoStart = {
   2, 1,
   1, 1,
   255, 1,
   1000, 1,
   1, 1
};

pack_swiloc_SwiLocSetAutoStart_t tpack_swiloc_SwiLocSetAutoStartModemSettings = {
   2, 1,
   1, 1,
   255, 1,
   1000, 1,
   1, 1
};

unpack_swiloc_SwiLocGetAutoStart_t tunpack_swiloc_SwiLocGetAutoStartModemSettings =
{
    2, 1,
    1, 1,
    255, 1,
    1000, 1,
    1, 1,
    SWI_UINT256_INT_VALUE
};

pack_swiloc_SwiLocSetAutoStart_t tpack_swiloc_SwiLocSetAutoStartEnabled =
{
    2, 1,
    1, 1,
    255, 1,
    1000, 1,
    1, 1
};

unpack_swiloc_SwiLocGetAutoStart_t tunpack_swiloc_SwiLocGetAutoStartEnabledResult =
{
    2, 1,
    1, 1,
    255, 1,
    1000, 1,
    1, 1,
    {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}}
};


pack_swiloc_SwiLocSetAutoStart_t tpack_swiloc_SwiLocSetAutoStartDisabled = {
   0, 1,
   1, 1,
   1, 1,
   1, 1,
   1, 1
};

unpack_swiloc_SwiLocGetAutoStart_t tunpack_swiloc_SwiLocGetAutoStartDisabledResult =
{
    0, 1,
    1, 1,
    1, 1,
    1, 1,
    1, 1,
    {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}}
};


swi_uint256_t unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)
}};

swi_uint256_t unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swiloc_SwiLocSetAutoStartParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_swiloc_SwiLocSetAutoStartParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

unpack_swiloc_SwiLocSetAutoStart_t tunpack_swiloc_SwiLocSetAutoStart = {0, {{0}} };

void dump_swiloc_SwiLocGetAutoStart(void *ptr)
{
    unpack_swiloc_SwiLocGetAutoStart_t *resp =
        (unpack_swiloc_SwiLocGetAutoStart_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);

    if(swi_uint256_get_bit (resp->ParamPresenceMask, 16))
        printf("\tfunction: %d\n", resp->function );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 17))
        printf("\tfix_type: %d \n", resp->fix_type);
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 18))
        printf("\tmax_time: %d\n", resp->max_time);
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 19))
        printf("\tmax_dist: %d\n", resp->max_dist);
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 20))
        printf("\tfix_rate: %d\n", resp->fix_rate);
    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if ( (swi_uint256_get_bit (resp->ParamPresenceMask, 16)) ||
    (swi_uint256_get_bit (resp->ParamPresenceMask, 17)) ||
    (swi_uint256_get_bit (resp->ParamPresenceMask, 18)) ||
    (swi_uint256_get_bit (resp->ParamPresenceMask, 19)) ||
    (swi_uint256_get_bit (resp->ParamPresenceMask, 20)) )
         is_matching = 1;
    CHECK_WHITELIST_MASK(
        unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskWhiteList,
        resp->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
      unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskMandatoryList,
        resp->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching== 1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching== 1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_swiloc_SwiLocGetAutoStartDefaultSettings(void *ptr)
{
    unpack_swiloc_SwiLocGetAutoStart_t *resp =
        (unpack_swiloc_SwiLocGetAutoStart_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);

    if(swi_uint256_get_bit (resp->ParamPresenceMask, 16))
    {
        tpack_swiloc_SwiLocSetAutoStartModemSettings.function = resp->function;
    }
    tpack_swiloc_SwiLocSetAutoStartModemSettings.set_function = resp->function_reported;
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 17))
    {
        tpack_swiloc_SwiLocSetAutoStartModemSettings.fix_type = resp->fix_type;
    }    
    tpack_swiloc_SwiLocSetAutoStartModemSettings.set_fix_type = resp->fix_type_reported;
    
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 18))
    {
        tpack_swiloc_SwiLocSetAutoStartModemSettings.max_time = resp->max_time;
    }    
    tpack_swiloc_SwiLocSetAutoStartModemSettings.set_max_time = resp->max_time_reported;
    

    if(swi_uint256_get_bit (resp->ParamPresenceMask, 19))
    {
        tpack_swiloc_SwiLocSetAutoStartModemSettings.max_dist = resp->max_dist;
    }    
    tpack_swiloc_SwiLocSetAutoStartModemSettings.set_max_dist = resp->max_dist_reported;
    
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 20))
    {
        tpack_swiloc_SwiLocSetAutoStartModemSettings.fix_rate = resp->fix_rate;
    }    
    tpack_swiloc_SwiLocSetAutoStartModemSettings.set_fix_rate = resp->fix_rate_reported;
    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", NA_MSG);
    #endif
}

void dump_swiloc_SwiLocSetAutoStart(void *ptr)
{
    unpack_swiloc_SwiLocSetAutoStart_t *resp =
        (unpack_swiloc_SwiLocSetAutoStart_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);

    if(swi_uint256_get_bit (resp->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, resp->Tlvresult);
   if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
   CHECK_WHITELIST_MASK(
        unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskWhiteList,
        resp->ParamPresenceMask);
   CHECK_MANDATORYLIST_MASK(
        unpack_swiloc_SwiLocGetAutoStartParamPresenceMaskMandatoryList,
        resp->ParamPresenceMask);
   if ( resp->Tlvresult != 0 )
   {
       local_fprintf("%s\n",  SUCCESS_MSG);
       local_fprintf("%s\n", remark);
   }
   else
   {
       local_fprintf("%s\n",  SUCCESS_MSG);
   }
#endif
}

void dump_swiloc_SwiLocGetAutoStartDisabled(void *ptr)
{
    unpack_swiloc_SwiLocGetAutoStart_t *resp =
        (unpack_swiloc_SwiLocGetAutoStart_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(memcmp(resp,&tunpack_swiloc_SwiLocGetAutoStartDisabledResult,sizeof(unpack_swiloc_SwiLocGetAutoStart_t)) != 0) 
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_swiloc_SwiLocGetAutoStartEnabled(void *ptr)
{
    unpack_swiloc_SwiLocGetAutoStart_t *resp =
        (unpack_swiloc_SwiLocGetAutoStart_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(memcmp(resp,&tunpack_swiloc_SwiLocGetAutoStartEnabledResult,sizeof(unpack_swiloc_SwiLocGetAutoStart_t)) != 0) 
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}


testitem_t swiloctotest[] = {
    /////Get Settings from radio module Start///////
    {
        (pack_func_item) &pack_swiloc_SwiLocGetAutoStart, "pack_swiloc_SwiLocGetAutoStart",
        NULL,
        (unpack_func_item) &unpack_swiloc_SwiLocGetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocGetAutoStartModemSettings, dump_swiloc_SwiLocGetAutoStartDefaultSettings
    },
    /////Get Settings from radio module End///////
    {
        (pack_func_item) &pack_swiloc_SwiLocGetAutoStart, "pack_swiloc_SwiLocGetAutoStart",
        NULL,
        (unpack_func_item) &unpack_swiloc_SwiLocGetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocGetAutoStart, dump_swiloc_SwiLocGetAutoStart
    },
    {
        (pack_func_item) &pack_swiloc_SwiLocSetAutoStart, "pack_swiloc_SwiLocSetAutoStart",
        &tpack_swiloc_SwiLocSetAutoStart,
        (unpack_func_item) &unpack_swiloc_SwiLocSetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocSetAutoStart, dump_swiloc_SwiLocSetAutoStart
    },
    ////SET/GET TEST START////////   
    {
        (pack_func_item) &pack_swiloc_SwiLocSetAutoStart, "pack_swiloc_SwiLocSetAutoStart",
        &tpack_swiloc_SwiLocSetAutoStartDisabled,
        (unpack_func_item) &unpack_swiloc_SwiLocSetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocSetAutoStart, NULL
    },
    {
        (pack_func_item) &pack_swiloc_SwiLocGetAutoStart, "pack_swiloc_SwiLocGetAutoStart",
        NULL,
        (unpack_func_item) &unpack_swiloc_SwiLocGetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocGetAutoStart, dump_swiloc_SwiLocGetAutoStartDisabled
    },

    {
        (pack_func_item) &pack_swiloc_SwiLocSetAutoStart, "pack_swiloc_SwiLocSetAutoStart",
        &tpack_swiloc_SwiLocSetAutoStartEnabled,
        (unpack_func_item) &unpack_swiloc_SwiLocSetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocSetAutoStart, NULL
    },
    {
        (pack_func_item) &pack_swiloc_SwiLocGetAutoStart, "pack_swiloc_SwiLocGetAutoStart",
        NULL,
        (unpack_func_item) &unpack_swiloc_SwiLocGetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocGetAutoStart, dump_swiloc_SwiLocGetAutoStartEnabled
    },
    ////SET/GET TEST END////////
    /////Restore Settings to radio module Start///////
    {
        (pack_func_item) &pack_swiloc_SwiLocSetAutoStart, "pack_swiloc_SwiLocSetAutoStart",
        &tpack_swiloc_SwiLocSetAutoStartModemSettings,
        (unpack_func_item) &unpack_swiloc_SwiLocSetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        &tunpack_swiloc_SwiLocSetAutoStart, NULL
    },
    /////Restore Settings to radio module End//////
};

unsigned int swilocarraylen = (unsigned int)((sizeof(swiloctotest))/(sizeof(swiloctotest[0])));

testitem_t swiloctotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_swiloc_SwiLocGetAutoStart, "pack_swiloc_SwiLocGetAutoStart",
        NULL,
        (unpack_func_item) &unpack_swiloc_SwiLocGetAutoStart, "unpack_swiloc_SwiLocGetAutoStart",
        NULL, dump_swiloc_SwiLocGetAutoStart
    }
};

void swiloc_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======SWILOC pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(swiloctotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(swiloctotest_invalidunpack[i].dump!=NULL)
        {
            swiloctotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(swiloctotest)/sizeof(testitem_t); i++)
    {
        if(swiloctotest[i].dump!=NULL)
        {
            swiloctotest[i].dump(NULL);
        }
    }

    for(i=0; i<sizeof(swiloctotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swiloctotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,swiloctotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(swiloc<0)
            swiloc = client_fd(eSWILOC);
        if(swiloc<0)
        {
            fprintf(stderr,"SWILOC Service Not Supported!\n");
            return ;
        }
        rtn = write(swiloc, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(swiloc>=0)
                close(swiloc);
            swiloc=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWILOC, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(swiloc, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eSWILOC, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(swiloctotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                swiloctotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           swiloctotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    swiloctotest_invalidunpack[i].dump(swiloctotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(swiloc>=0)
                close(swiloc);
            swiloc=-1;
        }
        sleep(1);
        xid++;
    }
    if(swiloc>=0)
        close(swiloc);
    swiloc=-1;
}


void swiloc_test_pack_unpack_loop()
{
    unsigned i;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    printf("\n======SWI LOC dummy unpack test===========\n");
    iLocalLog = 0;
    swiloc_validate_dummy_unpack();
    iLocalLog = 1;
    printf("\n======SWI LOC pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "SWILOC Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    unsigned xid =1;
    for(i=0; i<sizeof(swiloctotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swiloctotest[i].pack)(&req_ctx, req, &reqLen, swiloctotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", swiloctotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }
        if(swiloc<0)
            swiloc = client_fd(eSWILOC);
        if(swiloc<0)
        {
            fprintf(stderr,"SWI LOC Service Not Supported!\n");
            return ;
        }
        rtn = write(swiloc, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSWILOC, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(swiloc, rsp, QMI_MSG_MAX);

            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                qmi_msg = helper_get_resp_ctx(eSWILOC, rsp, rspLen, &rsp_ctx);
                
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("SWI LOC RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack on xid %d\n",xid);
                        unpackRetCode = run_unpack_item(swiloctotest[i].unpack)(rsp, rspLen, swiloctotest[i].unpack_ptr);
    
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", swiloctotest[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                        if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                        {
                            eRCode = helper_get_error_code(rsp);
                            reason = helper_get_error_reason(eRCode);
                            sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                        }
                    #endif
                        if(swiloctotest[i].dump)
                        {
                           swiloctotest[i].dump(swiloctotest[i].unpack_ptr);
                        }
                        else
                        {
                           local_fprintf("%s\n",  NA_MSG);
                        }
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
                    printf("SWI LOC IND: msgid 0x%x\n", rsp_ctx.msgid);
                    //swiloc_indication_handler(rsp_ctx.msgid, rsp,  rspLen);
                }
            }
            else
            {
                /* read nothing, sleep 1 second and read again */
                sleep(1);
            }
        }
        xid++;
    }
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_swiloc_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_SWI_LOC_GET_AUTO_START */
    {0x02,0x01,0x00,0x01,0x00,0x21,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
     0x00,0x02,0x11,0x01,0x00,0x03,0x12,0x01,0x00,0x04,0x13,0x04,0x00,0x05,0x04,0x03,
     0x00,0x14,0x04,0x00,0x01,0x02,0x00,0x00},

    /* eQMI_SWI_LOC_SET_AUTO_START */
    {0x02,0x02,0x00,0x02,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },
   };

/* eQMI_SWI_LOC_GET_AUTO_START */
const unpack_swiloc_SwiLocGetAutoStart_t const_unpack_swiloc_SwiLocGetAutoStart_t = {
        2,1,3,1,4,1,0x030405,1,0x0201,1,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_SWI_LOC_GET_AUTO_START */
const unpack_swiloc_SwiLocSetAutoStart_t const_unpack_swiloc_SwiLocSetAutoStart_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

int swiloc_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swiloc_resp_msg)/sizeof(validate_swiloc_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swiloc_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_SWI_LOC, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWILOC IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWILOC RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_SWI_LOC_GET_AUTO_START:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiloc_SwiLocGetAutoStart,
                    dump_swiloc_SwiLocGetAutoStart,
                    msg.buf,
                    rlen,
                    &const_unpack_swiloc_SwiLocGetAutoStart_t);
                }
                break;
            case eQMI_SWI_LOC_SET_AUTO_START:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiloc_SwiLocSetAutoStart,
                    dump_swiloc_SwiLocSetAutoStart,
                    msg.buf,
                    rlen,
                    &const_unpack_swiloc_SwiLocSetAutoStart_t);
                }
                break;
            }
        }
    }
    return 0;
}

