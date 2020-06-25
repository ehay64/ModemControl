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
#include "tmd.h"

int tmd_validate_dummy_unpack();

int tmd=-1;
pthread_attr_t tmd_attr;
pthread_t tmd_tid;
volatile int enTmdThread;
static int iLocalLog = 1;

unpack_tmd_SLQSTmdGetMitigationDevList_t  getMitigationDevList;

void dump_SLQSTmdGetMitigationDevList(void* ptr)
{
    unpack_tmd_SLQSTmdGetMitigationDevList_t *pMitigationDevList =
        (unpack_tmd_SLQSTmdGetMitigationDevList_t*) ptr;
    uint8_t lcount, lcount1;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (pMitigationDevList->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, pMitigationDevList->Tlvresult);
    if(swi_uint256_get_bit (pMitigationDevList->ParamPresenceMask, 16))
    {
        printf("\tMitigation device list length: %d\n", pMitigationDevList->MitigationDevListLen);
        for ( lcount=0 ; lcount < pMitigationDevList->MitigationDevListLen; lcount++ )
        {
            printf("\nMitigation Device ID Length %d\n ",pMitigationDevList->MitigationDevList[lcount].mitigationDevIdLen);
            printf("\t\tMitigation Device ID : ");
            for ( lcount1 = 0 ; lcount1 < pMitigationDevList->MitigationDevList[lcount].mitigationDevIdLen; lcount1++ )
            {
                printf("%c", pMitigationDevList->MitigationDevList[lcount].mitigationDevId[lcount1]);
            }
            printf("\n");
            printf("\t\tMax Mitigation Level : %d\n", pMitigationDevList->MitigationDevList[lcount].maxMitigationLevel);
        }
    }
}

void dump_SLQSTmdGetMitigationLvl(void* ptr)
{
    unpack_tmd_SLQSTmdGetMitigationLvl_t *pLvl =
        (unpack_tmd_SLQSTmdGetMitigationLvl_t*) ptr;

    printf("[%s]\n", __func__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pLvl->ParamPresenceMask);

    if(swi_uint256_get_bit (pLvl->ParamPresenceMask, 16))
    {
        printf("\tCurrent Mitigation Level: %d\n", pLvl->CurrentmitigationLvl);
    }
    if(swi_uint256_get_bit (pLvl->ParamPresenceMask, 17))
    {
        printf("\tReq Mitigation Level: %d\n", pLvl->ReqMitigationLvl);
    }
}

void dump_SLQSTmdRegNotMitigationLvl(void* ptr)
{
    unpack_tmd_SLQSTmdRegNotMitigationLvl_t *pRsp =
        (unpack_tmd_SLQSTmdRegNotMitigationLvl_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pRsp->ParamPresenceMask);
    if(swi_uint256_get_bit (pRsp->ParamPresenceMask, 2))
    {
        printf("[%s] result %d\n", __func__, pRsp->Tlvresult);
    }
}

void dump_SLQSTmdDeRegNotMitigationLvl(void* ptr)
{
    unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t *pRsp =
        (unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pRsp->ParamPresenceMask);
    if(swi_uint256_get_bit (pRsp->ParamPresenceMask, 2))
    {
        printf("[%s] result %d\n", __func__, pRsp->Tlvresult);
    }
}

void dump_SLQSTmdMitigationLvlRptCallback_ind(void* ptr)
{
    unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t *ind_rsp =
        (unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (ind_rsp->ParamPresenceMask);
    if(swi_uint256_get_bit (ind_rsp->ParamPresenceMask, 1))
    {
        printf("\tdevice id len: %d\n", ind_rsp->deviceIdLen);
        printf("\tdevice id: %s\n", ind_rsp->deviceID);
    }
    if(swi_uint256_get_bit (ind_rsp->ParamPresenceMask, 2))
    {
        printf("\tlevel: %d\n", ind_rsp->lvl);
    }
}

pack_tmd_SLQSTmdGetMitigationLvl_t lvl_req = { 5, "modem"};
unpack_tmd_SLQSTmdGetMitigationLvl_t lvl_rsp;

pack_tmd_SLQSTmdRegNotMitigationLvl_t reg_req = {5, "modem"};
unpack_tmd_SLQSTmdRegNotMitigationLvl_t reg_rsp;

pack_tmd_SLQSTmdDeRegNotMitigationLvl_t dereg_req = {5, "modem"};
unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t dereg_rsp;

testitem_t tmdtotest[] = {
    {
        (pack_func_item) &pack_tmd_SLQSTmdGetMitigationDevList, "pack_tmd_SLQSTmdGetMitigationDevList",
        NULL,
        (unpack_func_item) &unpack_tmd_SLQSTmdGetMitigationDevList, "unpack_tmd_SLQSTmdGetMitigationDevList",
        &getMitigationDevList, dump_SLQSTmdGetMitigationDevList
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdGetMitigationLvl, "pack_tmd_SLQSTmdGetMitigationLvl",
        &lvl_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdGetMitigationLvl, "unpack_tmd_SLQSTmdGetMitigationLvl",
        &lvl_rsp, dump_SLQSTmdGetMitigationLvl
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdRegNotMitigationLvl, "pack_tmd_SLQSTmdRegNotMitigationLvl",
        &reg_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdRegNotMitigationLvl, "unpack_tmd_SLQSTmdRegNotMitigationLvl",
        &reg_rsp, dump_SLQSTmdRegNotMitigationLvl
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdDeRegNotMitigationLvl, "pack_tmd_SLQSTmdDeRegNotMitigationLvl",
        &dereg_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdDeRegNotMitigationLvl, "unpack_tmd_SLQSTmdDeRegNotMitigationLvl",
        &dereg_rsp, dump_SLQSTmdDeRegNotMitigationLvl
    },
};

unsigned int tmdarraylen = (unsigned int)((sizeof(tmdtotest))/(sizeof(tmdtotest[0])));

testitem_t tmdtotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_tmd_SLQSTmdGetMitigationDevList, "pack_tmd_SLQSTmdGetMitigationDevList",
        NULL,
        (unpack_func_item) &unpack_tmd_SLQSTmdGetMitigationDevList, "unpack_tmd_SLQSTmdGetMitigationDevList",
        NULL, dump_SLQSTmdGetMitigationDevList
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdGetMitigationLvl, "pack_tmd_SLQSTmdGetMitigationLvl",
        &lvl_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdGetMitigationLvl, "unpack_tmd_SLQSTmdGetMitigationLvl",
        NULL, dump_SLQSTmdGetMitigationLvl
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdRegNotMitigationLvl, "pack_tmd_SLQSTmdRegNotMitigationLvl",
        &reg_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdRegNotMitigationLvl, "unpack_tmd_SLQSTmdRegNotMitigationLvl",
        NULL, dump_SLQSTmdRegNotMitigationLvl
    },
    {
        (pack_func_item) &pack_tmd_SLQSTmdDeRegNotMitigationLvl, "pack_tmd_SLQSTmdDeRegNotMitigationLvl",
        &dereg_req,
        (unpack_func_item) &unpack_tmd_SLQSTmdDeRegNotMitigationLvl, "unpack_tmd_SLQSTmdDeRegNotMitigationLvl",
        NULL, dump_SLQSTmdDeRegNotMitigationLvl
    },
};

/////////////////////////////////////////////////////////////
//
void try_run_indication_unpack()
{
    int rtn;
    uint8_t ind[] = {
        0x04 ,0x00 ,0x00 ,0x25 ,0x00 ,0x0D ,0x00 ,
        0x01 ,0x06 ,0x00 ,0x05 ,0x6D ,0x6F ,0x64 ,0x65 ,0x6D,
        0x02 ,0x01 ,0x00 ,0xFF,
    };
    unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t ind_rsp;

    rtn = unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind(
            ind, sizeof(ind), &ind_rsp);
    if (!rtn)
    {
        printf("[unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind]\n");
        if(swi_uint256_get_bit (ind_rsp.ParamPresenceMask, 1))
        {
            printf("\tdevice id len: %d\n", ind_rsp.deviceIdLen);
            printf("\tdevice id: %s\n", ind_rsp.deviceID);
        }
        if(swi_uint256_get_bit (ind_rsp.ParamPresenceMask, 2))
        {
            printf("\tlevel: %d\n", ind_rsp.lvl);
        }
    }
}

void tmd_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======TMD pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(tmdtotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(tmdtotest_invalidunpack[i].dump!=NULL)
        {
            tmdtotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(tmdtotest)/sizeof(testitem_t); i++)
    {
        if(tmdtotest[i].dump!=NULL)
        {
            tmdtotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(tmdtotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(tmdtotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,tmdtotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(tmd<0)
            tmd = client_fd(eTMD);
        if(tmd<0)
        {
            fprintf(stderr,"TMD Service Not Supported!\n");
            return ;
        }
        rtn = write(tmd, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(tmd>=0)
                close(tmd);
            tmd=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eTMD, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(tmd, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eTMD, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(tmdtotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                tmdtotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           tmdtotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    tmdtotest_invalidunpack[i].dump(tmdtotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(tmd>=0)
                close(tmd);
            tmd=-1;
        }
        sleep(1);
        xid++;
    }
    if(tmd>=0)
        close(tmd);
    tmd=-1;
}


void tmd_test_pack_unpack_loop()
{
    unsigned i;
    unsigned xid =1;

    printf("======TMD dummy unpack test===========\n");
    iLocalLog = 0;
    tmd_validate_dummy_unpack();
    iLocalLog = 1;

    printf("======TMD pack/unpack test===========\n");
    for(i=0; i<sizeof(tmdtotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(tmdtotest[i].pack)(&req_ctx, req, &reqLen,tmdtotest[i].pack_ptr);
        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        if(tmd<0)
            tmd = client_fd(eTMD);
        if(tmd<0)
        {
            fprintf(stderr,"TMD Service Not Supported!\n");
            return ;
        }
        rtn = write(tmd, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eTMD, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(tmd, rsp, QMI_MSG_MAX);

        if(rtn > 0)
        {
            rspLen = (uint16_t) rtn;
            qmi_msg = helper_get_resp_ctx(eTMD, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
            run_unpack_item(tmdtotest[i].unpack)(rsp, rspLen, tmdtotest[i].unpack_ptr);
            tmdtotest[i].dump(tmdtotest[i].unpack_ptr);
            }
        }
        else
        {
            printf("read %d \n", rtn);
        }
        xid++;
    }
    if(tmd>=0)
        close(tmd);
    tmd=-1;

    try_run_indication_unpack();
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_tmd_resp_msg[][QMI_MSG_MAX] ={

        /* eQMI_TMD_GET_MITIGATION_DEV_LIST */
    {0x02,0x01,0x00,0x20,0x00,0x16,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x0C,0x00,0x02,0x02,0x70,0x61,0x03,0x05,0x6D,0x6F,0x64,0x65,0x6D,0x03},

        /* eQMI_TMD_GET_MITIGATION_LEVEL */
    {0x02,0x02,0x00,0x22,0x00,0x0F,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x01,0x00,0x00},

        /* eQMI_TMD_REG_NOT_MITIGATION_LEVEL */
    {0x02,0x03,0x00,0x23,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_TMD_DEREG_NOT_MITIGATION_LEVEL */
    {0x02,0x04,0x00,0x24,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

        /* eQMI_TMD_MITIGATION_LEVEL_REPORT_IND */
    {0x04,0x05,0x00,0x25,0x00,0x0A,0x00,
            0x01,0x03,0x00,0x02,0x70,0x61,
            0x02,0x01,0x00,0x03},

 };

       /* eQMI_TMD_GET_MITIGATION_DEV_LIST */
const unpack_tmd_SLQSTmdGetMitigationDevList_t const_unpack_tmd_SLQSTmdGetMitigationDevList_t = {
        2,{{2,"pa",3},{5,"modem",3}},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

        /* eQMI_TMD_GET_MITIGATION_LEVEL */
const unpack_tmd_SLQSTmdGetMitigationLvl_t const_unpack_tmd_SLQSTmdGetMitigationLvl_t = {
 0,0,0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

        /* eQMI_TMD_REG_NOT_MITIGATION_LEVEL */
const unpack_tmd_SLQSTmdRegNotMitigationLvl_t const_unpack_tmd_SLQSTmdRegNotMitigationLvl_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

        /* eQMI_TMD_DEREG_NOT_MITIGATION_LEVEL */
const unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t const_unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t = {
0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_TMD_MITIGATION_LEVEL_REPORT_IND */
const unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t const_unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t = {
2,{"pa"},3,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

int tmd_validate_dummy_unpack()
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
    loopCount = sizeof(validate_tmd_resp_msg)/sizeof(validate_tmd_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_tmd_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eTMD, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("TMD IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("TMD RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_TMD_GET_MITIGATION_DEV_LIST:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_tmd_SLQSTmdGetMitigationDevList,
                    dump_SLQSTmdGetMitigationDevList,
                    msg.buf,
                    rlen,
                    &const_unpack_tmd_SLQSTmdGetMitigationDevList_t);
                }
                break;
            case eQMI_TMD_GET_MITIGATION_LEVEL:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_tmd_SLQSTmdGetMitigationLvl,
                    dump_SLQSTmdGetMitigationLvl,
                    msg.buf,
                    rlen,
                    &const_unpack_tmd_SLQSTmdGetMitigationLvl_t);
                }
                break;
            case eQMI_TMD_REG_NOT_MITIGATION_LEVEL:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_tmd_SLQSTmdRegNotMitigationLvl,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_tmd_SLQSTmdRegNotMitigationLvl_t);
                }
                break;
            case eQMI_TMD_DEREG_NOT_MITIGATION_LEVEL:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_tmd_SLQSTmdDeRegNotMitigationLvl,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_tmd_SLQSTmdDeRegNotMitigationLvl_t);
                }
                break;
            case eQMI_TMD_MITIGATION_LEVEL_REPORT_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind,
                    dump_SLQSTmdMitigationLvlRptCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_tmd_SLQSTmdMitigationLvlRptCallback_ind_t);
                }
                break;
            }
        }
    }
    return 0;
}

