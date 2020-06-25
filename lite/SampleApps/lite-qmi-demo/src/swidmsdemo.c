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
#include "swidms.h"

int swidms_validate_dummy_unpack();
int swidms=-1;

swidms_mtuSize3gppTlv         MTUSize3gpp;
swidms_hrpdMTUSizeTlv         HrpdMTUSize;
swidms_ehrpdMTUSizeTlv        EhrpdMTUSize;
swidms_usbMTUSizeTlv          UsbMTUSize;

unpack_swidms_SLQSSwiDmsGetMTU_t tunpack_swidms_SLQSSwiDmsGetMTU ={&MTUSize3gpp,
        &HrpdMTUSize, &EhrpdMTUSize, &UsbMTUSize, SWI_UINT256_INT_VALUE };

pack_swidms_SLQSSwiDmsSetMTU_t tpack_swidms_SLQSSwiDmsSetMTU = {1500};
unpack_swidms_SLQSSwiDmsSetMTU_t tunpack_swidms_SLQSSwiDmsSetMTU ={0xFF, SWI_UINT256_INT_VALUE };

unpack_swidms_SLQSSwiDmsGetUsbNetNum_t usbNetNumResp;
pack_swidms_SLQSSwiDmsSetUsbNetNum_t usbNetNumReq = {3};
unpack_swidms_SLQSSwiDmsSetUsbNetNum_t setUsbNetNumResp = {0, SWI_UINT256_INT_VALUE };

swidms_intfaceCfgTlv          InterfaceCfg;
swidms_supportedIntBitmaskTlv SupportedBitmasks;

unpack_swidms_SLQSSwiDmsGetUsbComp_t tunpack_swidms_SLQSSwiDmsGetUsbComp ={&InterfaceCfg,
        &SupportedBitmasks, SWI_UINT256_INT_VALUE };

pack_swidms_SLQSSwiDmsSetUsbComp_t tpack_swidms_SLQSSwiDmsSetUsbComp = {0x00008000D};
unpack_swidms_SLQSSwiDmsSetUsbComp_t tunpack_swidms_SLQSSwiDmsSetUsbComp ={0xFF, SWI_UINT256_INT_VALUE };
pack_swidms_SLQSSwiDmsSetUsbComp_t originalUsbConfigValue;

swidms_SwiDmsGetHWWatchdog          GetHWWatchdog;
unpack_swidms_SLQSSwiDmsGetHWWatchdog_t tunpack_swidms_SLQSSwiDmsGetHWWatchdog ={&GetHWWatchdog, SWI_UINT256_INT_VALUE };

pack_swidms_SLQSSwiDmsSetHWWatchdog_t tpack_swidms_SLQSSwiDmsSetHWWatchdog = {0x0000005, 0x00000005, 0x00};
unpack_swidms_SLQSSwiDmsSetHWWatchdog_t tunpack_swidms_SLQSSwiDmsSetHWWatchdog ={0xFF, SWI_UINT256_INT_VALUE };
pack_swidms_SLQSSwiDmsSetHWWatchdog_t originaHWWatchdogValue;
unpack_swidms_SLQSSwiDmsGetSecureInfo_t tunpack_swidms_SLQSSwiDmsGetSecureInfo;


void dump_SLQSSwiDmsGetUsbNetNum(void* ptr)
{
    unpack_swidms_SLQSSwiDmsGetUsbNetNum_t *result =
        (unpack_swidms_SLQSSwiDmsGetUsbNetNum_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("USB Net Numbers : %d\n", result->usbNetNum );
}

void dump_SLQSSwiDmsSetUsbNetNum(void* ptr)
{
    unpack_swidms_SLQSSwiDmsSetUsbNetNum_t *result =
            (unpack_swidms_SLQSSwiDmsSetUsbNetNum_t*) ptr;

    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSwiDmsGetMTU(void* ptr)
{
    unpack_swidms_SLQSSwiDmsGetMTU_t *pSwiDmsGetMTU =
        (unpack_swidms_SLQSSwiDmsGetMTU_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if( (pSwiDmsGetMTU->pMTUSize3gpp) && (swi_uint256_get_bit (pSwiDmsGetMTU->ParamPresenceMask, 16)) )
        printf("3GPP MTU Size    : %d\n", pSwiDmsGetMTU->pMTUSize3gpp->MTUSize3gpp);
    if( (pSwiDmsGetMTU->pHrpdMTUSize) && (swi_uint256_get_bit (pSwiDmsGetMTU->ParamPresenceMask, 17)) )
        printf("HRPD MTU Size    : %d\n", pSwiDmsGetMTU->pHrpdMTUSize->hrpdMTUSize);
    if( (pSwiDmsGetMTU->pEhrpdMTUSize) && (swi_uint256_get_bit (pSwiDmsGetMTU->ParamPresenceMask, 18)) )
        printf("EHRPD MTU Size   : %d\n", pSwiDmsGetMTU->pEhrpdMTUSize->ehrpdMTUSize);
    if( (pSwiDmsGetMTU->pUsbMTUSize) && (swi_uint256_get_bit (pSwiDmsGetMTU->ParamPresenceMask, 19)) )
        printf("USB MTU Size  : %d\n", pSwiDmsGetMTU->pUsbMTUSize->UsbMTUSize);
}

void dump_SLQSSwiDmsSetMTU(void * ptr)
{
    unpack_swidms_SLQSSwiDmsSetMTU_t *result =
            (unpack_swidms_SLQSSwiDmsSetMTU_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSwiDmsGetUsbComp(void* ptr)
{
    unpack_swidms_SLQSSwiDmsGetUsbComp_t *pSwiDmsGetUsbComp =
        (unpack_swidms_SLQSSwiDmsGetUsbComp_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if( (pSwiDmsGetUsbComp->pInterfaceCfg) && (swi_uint256_get_bit (pSwiDmsGetUsbComp->ParamPresenceMask, 1)) )
    {
        printf("Host USB Interface Configuration:\n");
        printf("Current Config Type    : %d\n", pSwiDmsGetUsbComp->pInterfaceCfg->CurrentCfgType);
        printf("Current Config Value    : 0X%08X\n", pSwiDmsGetUsbComp->pInterfaceCfg->CfgValue);
        originalUsbConfigValue.CfgValue = pSwiDmsGetUsbComp->pInterfaceCfg->CfgValue;
    }
    if( (pSwiDmsGetUsbComp->pSupportedBitmasks) && (swi_uint256_get_bit (pSwiDmsGetUsbComp->ParamPresenceMask, 16)) )
        printf("Valid Bitmasks   : 0X%08X\n", pSwiDmsGetUsbComp->pSupportedBitmasks->ValidBitmasks);
}

void dump_SLQSSwiDmsSetUsbComp(void * ptr)
{
    unpack_swidms_SLQSSwiDmsSetUsbComp_t *result =
            (unpack_swidms_SLQSSwiDmsSetUsbComp_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSwiDmsGetHWWatchdog(void* ptr)
{
    unpack_swidms_SLQSSwiDmsGetHWWatchdog_t *pSwiDmsGetHWWatchdog =
        (unpack_swidms_SLQSSwiDmsGetHWWatchdog_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if( (pSwiDmsGetHWWatchdog->pHWWatchdog) && (swi_uint256_get_bit (pSwiDmsGetHWWatchdog->ParamPresenceMask, 1)) )
    {
        printf("Hardware Watchdog Settings:\n");
        printf("timeout    : 0X%08X\n", pSwiDmsGetHWWatchdog->pHWWatchdog->timeout);
        printf("Reset Delay: 0X%08X\n", pSwiDmsGetHWWatchdog->pHWWatchdog->resetDelay);
        printf("enable     : %d\n", pSwiDmsGetHWWatchdog->pHWWatchdog->enable);
        printf("count: 0X%08X\n", pSwiDmsGetHWWatchdog->pHWWatchdog->count);
        originaHWWatchdogValue.timeout = pSwiDmsGetHWWatchdog->pHWWatchdog->timeout;
        originaHWWatchdogValue.resetDelay = pSwiDmsGetHWWatchdog->pHWWatchdog->resetDelay;
        originaHWWatchdogValue.enable = pSwiDmsGetHWWatchdog->pHWWatchdog->enable;
    }

}

void dump_SLQSSwiDmsSetHWWatchdog(void * ptr)
{
    unpack_swidms_SLQSSwiDmsSetHWWatchdog_t *result =
            (unpack_swidms_SLQSSwiDmsSetHWWatchdog_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSwiDmsGetSecureInfo(void* ptr)
{
    unpack_swidms_SLQSSwiDmsGetSecureInfo_t *result =
        (unpack_swidms_SLQSSwiDmsGetSecureInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->TlvResult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Secure Boot Enabled:%d\n",result->secureBootEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
        printf("Memory Dump Allowed:%d\n",result->memoryDumpAllowed);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
        printf("Jtag Access Allowed:%d\n",result->jtagAccessAllowed);
}

testitem_t swidmstotest[] = {
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetUsbNetNum, "pack_swidms_SLQSSwiDmsGetUsbNetNum",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetUsbNetNum, "unpack_swidms_SLQSSwiDmsGetUsbNetNum",
        &usbNetNumResp, dump_SLQSSwiDmsGetUsbNetNum
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbNetNum, "pack_swidms_SLQSSwiDmsSetUsbNetNum",
        &usbNetNumReq,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbNetNum, "unpack_swidms_SLQSSwiDmsSetUsbNetNum",
        &setUsbNetNumResp, dump_SLQSSwiDmsSetUsbNetNum
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetMTU, "pack_swidms_SLQSSwiDmsGetMTU",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetMTU, "unpack_swidms_SLQSSwiDmsGetMTU",
        &tunpack_swidms_SLQSSwiDmsGetMTU, dump_SLQSSwiDmsGetMTU
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetMTU, "pack_swidms_SLQSSwiDmsSetMTU",
        &tpack_swidms_SLQSSwiDmsSetMTU,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetMTU, "unpack_swidms_SLQSSwiDmsSetMTU",
        &tunpack_swidms_SLQSSwiDmsSetMTU, dump_SLQSSwiDmsSetMTU
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetUsbComp, "pack_swidms_SLQSSwiDmsGetUsbComp",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetUsbComp, "unpack_swidms_SLQSSwiDmsGetUsbComp",
        &tunpack_swidms_SLQSSwiDmsGetUsbComp, dump_SLQSSwiDmsGetUsbComp
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbComp, "pack_swidms_SLQSSwiDmsSetUsbComp",
        &tpack_swidms_SLQSSwiDmsSetUsbComp,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetUsbComp",
        &tunpack_swidms_SLQSSwiDmsSetUsbComp, dump_SLQSSwiDmsSetUsbComp
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbComp, "pack_swidms_SLQSSwiDmsSetUsbComp",
        &originalUsbConfigValue,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetUsbComp",
        &tunpack_swidms_SLQSSwiDmsSetUsbComp, dump_SLQSSwiDmsSetUsbComp
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetHWWatchdog, "pack_swidms_SLQSSwiDmsGetHWWatchdog",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetHWWatchdog, "unpack_swidms_SLQSSwiDmsGetHWWatchdog",
        &tunpack_swidms_SLQSSwiDmsGetHWWatchdog, dump_SLQSSwiDmsGetHWWatchdog
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetHWWatchdog, "pack_swidms_SLQSSwiDmsSetHWWatchdog",
        &tpack_swidms_SLQSSwiDmsSetHWWatchdog,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetHWWatchdog",
        &tunpack_swidms_SLQSSwiDmsSetHWWatchdog, dump_SLQSSwiDmsSetHWWatchdog
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetHWWatchdog, "pack_swidms_SLQSSwiDmsSetHWWatchdog",
        &originaHWWatchdogValue,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetHWWatchdog",
        &tunpack_swidms_SLQSSwiDmsSetHWWatchdog, dump_SLQSSwiDmsSetHWWatchdog
    },

    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetSecureInfo, "pack_swidms_SLQSSwiDmsGetSecureInfo",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetSecureInfo, "unpack_swidms_SLQSSwiDmsGetSecureInfo",
        &tunpack_swidms_SLQSSwiDmsGetSecureInfo, dump_SLQSSwiDmsGetSecureInfo
    },
};

testitem_t swidmstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetUsbNetNum, "pack_swidms_SLQSSwiDmsGetUsbNetNum",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetUsbNetNum, "unpack_swidms_SLQSSwiDmsGetUsbNetNum",
        NULL, dump_SLQSSwiDmsGetUsbNetNum
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbNetNum, "pack_swidms_SLQSSwiDmsSetUsbNetNum",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbNetNum, "unpack_swidms_SLQSSwiDmsSetUsbNetNum",
        NULL, dump_SLQSSwiDmsSetUsbNetNum
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetMTU, "pack_swidms_SLQSSwiDmsGetMTU",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetMTU, "unpack_swidms_SLQSSwiDmsGetMTU",
        NULL, dump_SLQSSwiDmsGetMTU
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetMTU, "pack_swidms_SLQSSwiDmsSetMTU",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetMTU, "unpack_swidms_SLQSSwiDmsSetMTU",
        NULL, dump_SLQSSwiDmsSetMTU
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbComp, "pack_swidms_SLQSSwiDmsSetUsbComp",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetUsbComp",
        NULL, dump_SLQSSwiDmsSetUsbComp
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetUsbComp, "pack_swidms_SLQSSwiDmsSetUsbComp",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetUsbComp",
        NULL, dump_SLQSSwiDmsSetUsbComp
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetHWWatchdog, "pack_swidms_SLQSSwiDmsGetHWWatchdog",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetHWWatchdog, "unpack_swidms_SLQSSwiDmsGetHWWatchdog",
        NULL, dump_SLQSSwiDmsGetHWWatchdog
    },
    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsSetHWWatchdog, "pack_swidms_SLQSSwiDmsSetHWWatchdog",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsSetUsbComp, "unpack_swidms_SLQSSwiDmsSetHWWatchdog",
        NULL, dump_SLQSSwiDmsSetHWWatchdog
    },

    {
        (pack_func_item) &pack_swidms_SLQSSwiDmsGetSecureInfo, "pack_swidms_SLQSSwiDmsGetSecureInfo",
        NULL,
        (unpack_func_item) &unpack_swidms_SLQSSwiDmsGetSecureInfo, "unpack_swidms_SLQSSwiDmsGetSecureInfo",
        NULL, dump_SLQSSwiDmsGetSecureInfo
    },
};

void swidms_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======SWIDMS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    
    for(i=0; i<sizeof(swidmstotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swidmstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,swidmstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("%s: returned %d, pack failed!\n", 
                       swidmstotest_invalidunpack[i].pack_func_name, rtn);
            continue;
        }

        if(swidms<0)
            swidms = client_fd(eSWIDMS);
        if(swidms<0)
        {
            fprintf(stderr,"SWIDMS Service Not Supported!\n");
            return ;
        }
        rtn = write(swidms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(swidms>=0)
                close(swidms);
            swidms=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIDMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(swidms, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eSWIDMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(swidmstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                swidmstotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           swidmstotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    swidmstotest_invalidunpack[i].dump(swidmstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(swidms>=0)
                close(swidms);
            swidms=-1;
        }
        sleep(1);
        xid++;
    }
    if(swidms>=0)
        close(swidms);
    swidms=-1;
}


void swidms_test_pack_unpack_loop()
{
    unsigned i;
    unsigned xid =1;

    printf("======SWIDMS dummy unpack test===========\n");
    swidms_validate_dummy_unpack();
    printf("======SWIDMS pack/unpack test===========\n");
    for(i=0; i<sizeof(swidmstotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swidmstotest[i].pack)(&req_ctx, req, &reqLen,swidmstotest[i].pack_ptr);
        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack failed with %d for %s \n", rtn, swidmstotest[i].pack_func_name);
            continue;
        }
        if(swidms<0)
            swidms = client_fd(eSWIDMS);
        if(swidms<0)
        {
            fprintf(stderr,"SWIDMS Service Not Supported!\n");
            return ;
        }
        rtn = write(swidms, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eSWIDMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(swidms, rsp, QMI_MSG_MAX);

        if(rtn > 0)
        {
            rspLen = (uint16_t) rtn;
            qmi_msg = helper_get_resp_ctx(eSWIDMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(swidmstotest[i].unpack)(rsp, rspLen, swidmstotest[i].unpack_ptr);
                if (rtn != eQCWWAN_ERR_NONE)
                    printf("unpack failed with %d for %s \n", rtn, swidmstotest[i].unpack_func_name);
                else
                    swidmstotest[i].dump(swidmstotest[i].unpack_ptr);
            }
        }
        else
        {
            printf("read %d \n", rtn);
            break;
        }
        xid++;
    }
    if(swidms>=0)
        close(swidms);
    swidms=-1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_swidms_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_SWIDMS_GET_USB_INTER_COMP */
    {0x02,0x01,0x00,0x01,0x00,0x19,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,0x00,
     0x03,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x10,0x04,0x00,0x07,0x07,0x00,0x00},

    /* eQMI_SWIDMS_SET_USB_INTER_COMP */
    {0x02,0x02,0x00,0x02,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIDMS_GET_USB_NET_NUM */
    {0x02,0x03,0x00,0x03,0x00,0x0B,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
     0x00,0x03 },

    /* eQMI_SWIDMS_SET_USB_NET_NUM */
    {0x02,0x04,0x00,0x04,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIDMS_GET_MTU */
    {0x02,0x05,0x00,0x05,0x00,0x1B,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x02,
     0x00,0x00,0x04,0x11,0x02,0x00,0x01,0x04,0x12,0x02,0x00,0x02,0x04,0x13,0x02,0x00,
     0x03,0x03 },

    /* eQMI_SWIDMS_SET_MTU */
    {0x02,0x06,0x00,0x06,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIDMS_GET_HW_WATCH_DOG */
    {0x02,0x07,0x00,0x08,0x00,0x17,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0d,
     0x00,0x05,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIDMS_SET_HW_WATCH_DOG */
    {0x02,0x08,0x00,0x09,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIDMS_GET_SEC_INFO */
    {0x02,0x09,0x00,0x07,0x00,0x13,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
     0x01,0x01,0x00,0x01,
     0x03,0x01,0x00,0x01,
     0x04,0x01,0x00,0x01,
    },
   };



/* eQMI_SWIDMS_GET_USB_INTER_COMP */
swidms_intfaceCfgTlv cst_ifcCfg = {1,3,0x0100};
swidms_supportedIntBitmaskTlv cst_mask = {1,0x0707};
const unpack_swidms_SLQSSwiDmsGetUsbComp_t const_unpack_swidms_SLQSSwiDmsGetUsbComp_t = {
        &cst_ifcCfg,&cst_mask,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

swidms_intfaceCfgTlv var_ifcCfg;
swidms_supportedIntBitmaskTlv var_mask;
unpack_swidms_SLQSSwiDmsGetUsbComp_t var_unpack_swidms_SLQSSwiDmsGetUsbComp_t = {
        &var_ifcCfg,&var_mask,{{0}} };

/* eQMI_SWIDMS_SET_USB_INTER_COMP */
const unpack_swidms_SLQSSwiDmsSetUsbComp_t const_unpack_swidms_SLQSSwiDmsSetUsbComp_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIDMS_GET_USB_NET_NUM */
const unpack_swidms_SLQSSwiDmsGetUsbNetNum_t const_unpack_swidms_SLQSSwiDmsGetUsbNetNum_t = {
        3,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_SWIDMS_SET_USB_NET_NUM */
const unpack_swidms_SLQSSwiDmsSetUsbNetNum_t const_unpack_swidms_SLQSSwiDmsSetUsbNetNum_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIDMS_GET_MTU */
swidms_mtuSize3gppTlv cst_mtu3gpp = {1,0x400};
swidms_hrpdMTUSizeTlv cst_mtuHrpd = {1,0x401};
swidms_ehrpdMTUSizeTlv cst_mtuEhrpd = {1,0x402};
swidms_usbMTUSizeTlv   cst_mtuUsb = {1,0x303};
const unpack_swidms_SLQSSwiDmsGetMTU_t const_unpack_swidms_SLQSSwiDmsGetMTU_t = {
        &cst_mtu3gpp,&cst_mtuHrpd,&cst_mtuEhrpd,&cst_mtuUsb, {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

swidms_mtuSize3gppTlv var_mtu3gpp;
swidms_hrpdMTUSizeTlv var_mtuHrpd;
swidms_ehrpdMTUSizeTlv var_mtuEhrpd;
swidms_usbMTUSizeTlv   var_mtuUsb;
unpack_swidms_SLQSSwiDmsGetMTU_t var_unpack_swidms_SLQSSwiDmsGetMTU_t = {
        &var_mtu3gpp,&var_mtuHrpd,&var_mtuEhrpd,&var_mtuUsb,{{0}} };

/* eQMI_SWIDMS_SET_MTU */
const unpack_swidms_SLQSSwiDmsSetMTU_t const_unpack_swidms_SLQSSwiDmsSetMTU_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIDMS_GET_HW_WATCH_DOG */
swidms_SwiDmsGetHWWatchdog getHWWatchdog = {0x00000005,0x00000005,0x00, 0x00000000};
const unpack_swidms_SLQSSwiDmsGetHWWatchdog_t const_unpack_swidms_SLQSSwiDmsGetHWWatchdog_t = {
        &getHWWatchdog,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

swidms_SwiDmsGetHWWatchdog var_HWwatchdog;
unpack_swidms_SLQSSwiDmsGetHWWatchdog_t var_unpack_swidms_SLQSSwiDmsGetHWWatchdog_t = {
        &var_HWwatchdog,{{0}} };

/* eQMI_SWIDMS_SET_HW_WATCH_DOG */
const unpack_swidms_SLQSSwiDmsSetHWWatchdog_t const_unpack_swidms_SLQSSwiDmsSetHWWatchdog_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIDMS_GET_SEC_INFO */
const unpack_swidms_SLQSSwiDmsGetSecureInfo_t const_unpack_swidms_SLQSSwiDmsGetSecureInfo_t = {
        1,1,1,0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,3,4)}} };

int swidms_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swidms_resp_msg)/sizeof(validate_swidms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swidms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_SWI_DMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWIDMA IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIDMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {

            case eQMI_SWIDMS_GET_USB_INTER_COMP:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swidms_SLQSSwiDmsGetUsbComp_t *varp = &var_unpack_swidms_SLQSSwiDmsGetUsbComp_t;
                    const unpack_swidms_SLQSSwiDmsGetUsbComp_t *cstp = &const_unpack_swidms_SLQSSwiDmsGetUsbComp_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsGetUsbComp,
                    dump_SLQSSwiDmsGetUsbComp,
                    msg.buf,
                    rlen,
                    &var_unpack_swidms_SLQSSwiDmsGetUsbComp_t,
                    3,
                    CMP_PTR_TYPE, varp->pInterfaceCfg, cstp->pInterfaceCfg,
                    CMP_PTR_TYPE, varp->pSupportedBitmasks, cstp->pSupportedBitmasks,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_SWIDMS_SET_USB_INTER_COMP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsSetUsbComp,
                    dump_SLQSSwiDmsSetUsbComp,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsSetUsbComp_t);
                }
                break;
            case eQMI_SWIDMS_GET_USB_NET_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsGetUsbNetNum,
                    dump_SLQSSwiDmsGetUsbNetNum,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsGetUsbNetNum_t);
                }
                break;
            case eQMI_SWIDMS_SET_USB_NET_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsSetUsbNetNum,
                    dump_SLQSSwiDmsSetUsbNetNum,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsSetUsbNetNum_t);
                }
                break;
            case eQMI_SWIDMS_GET_MTU:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swidms_SLQSSwiDmsGetMTU_t *varp = &var_unpack_swidms_SLQSSwiDmsGetMTU_t;
                    const unpack_swidms_SLQSSwiDmsGetMTU_t *cstp = &const_unpack_swidms_SLQSSwiDmsGetMTU_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsGetMTU,
                    dump_SLQSSwiDmsGetMTU,
                    msg.buf,
                    rlen,
                    &var_unpack_swidms_SLQSSwiDmsGetMTU_t,
                    5,
                    CMP_PTR_TYPE, varp->pMTUSize3gpp, cstp->pMTUSize3gpp,
                    CMP_PTR_TYPE, varp->pHrpdMTUSize, cstp->pHrpdMTUSize,
                    CMP_PTR_TYPE, varp->pEhrpdMTUSize, cstp->pEhrpdMTUSize,
                    CMP_PTR_TYPE, varp->pUsbMTUSize, cstp->pUsbMTUSize,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_SWIDMS_SET_MTU:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsSetMTU,
                    dump_SLQSSwiDmsSetMTU,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsSetMTU_t);
                }
                break;
            case eQMI_SWIDMS_SET_HW_WATCH_DOG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsSetHWWatchdog,
                    dump_SLQSSwiDmsSetHWWatchdog,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsSetHWWatchdog_t);
                }
                break;
            case eQMI_SWIDMS_GET_HW_WATCH_DOG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_swidms_SLQSSwiDmsGetHWWatchdog_t *varp = &var_unpack_swidms_SLQSSwiDmsGetHWWatchdog_t;
                    const unpack_swidms_SLQSSwiDmsGetHWWatchdog_t *cstp = &const_unpack_swidms_SLQSSwiDmsGetHWWatchdog_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsGetHWWatchdog,
                    dump_SLQSSwiDmsGetHWWatchdog,
                    msg.buf,
                    rlen,
                    &var_unpack_swidms_SLQSSwiDmsGetHWWatchdog_t,
                    1,
                    CMP_PTR_TYPE, varp->pHWWatchdog, cstp->pHWWatchdog);
                }
                break;
            case eQMI_SWIDMS_GET_SEC_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swidms_SLQSSwiDmsGetSecureInfo,
                    dump_SLQSSwiDmsGetSecureInfo,
                    msg.buf,
                    rlen,
                    &const_unpack_swidms_SLQSSwiDmsGetSecureInfo_t);
                }
                break;
            }
        }
    }
    return 0;
}

