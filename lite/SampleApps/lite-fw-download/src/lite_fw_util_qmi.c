#include "lite_fw_dld.h"


extern int g_devMode;
extern unsigned g_xid;

int client_fd(uint8_t svc,char *szQMIPath)
{
    int fd = open(szQMIPath, O_RDWR);
    ioctl(fd, QMI_GET_SERVICE_FILE_IOCTL, svc);
    return fd;
}

int ResetModem_QMI (char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    int i=0;

    pack_dms_SetPower_t SetPowerReq;
    unpack_dms_SetPower_t SetPowerResp;
    unpack_dms_GetPower_t GetPowerResp;

    int dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    {
        // Set modem offline
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;        
        memset(&rsp,0,QMI_MSG_MAX);
        memset(&req,0,QMI_MSG_MAX);        
        SetPowerReq.mode = DMS_PM_OFFLINE;
        
        if(pack_dms_SetPower(&req_ctx, req, &reqLen,&SetPowerReq)!=0)
        {
            printf ("pack_dms_SetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rspLen = read(dms, rsp, QMI_MSG_MAX);    
        if(unpack_dms_SetPower(rsp, rspLen,&SetPowerResp)!=0)
        {
            printf("unpack_dms_SetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
    }
    usleep(SECOND);
    
    // Get Power
    while ( i < MAX_POWER_REQUEST_RETRY)
    { 
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        memset(&GetPowerResp, 0, sizeof(GetPowerResp));
        req_ctx.xid = g_xid;        
        memset(&rsp,0,QMI_MSG_MAX);
        memset(&req,0,QMI_MSG_MAX); 
        
        if(pack_dms_GetPower(&req_ctx, req, &reqLen, NULL)!=0)
        {
            printf ("pack_dms_GetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rspLen = read(dms, rsp, QMI_MSG_MAX);    
        if(unpack_dms_GetPower(rsp, rspLen,&GetPowerResp)!=0)
        {
            printf("unpack_dms_GetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        printf("QMI current modem power %d\n",GetPowerResp.OperationMode);
        if (GetPowerResp.OperationMode == DMS_PM_OFFLINE)
            break;
        i++;
        usleep(SECOND); 
    }

    if (i == MAX_POWER_REQUEST_RETRY) {
        printf("Modem could not set in power offline mode!\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    {
        // Reset the modem
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;                
        memset(&rsp,0,QMI_MSG_MAX);
        memset(&req,0,QMI_MSG_MAX);        
        SetPowerReq.mode = DMS_PM_RESET;
        
        if(pack_dms_SetPower(&req_ctx, req, &reqLen,&SetPowerReq)!=0)
        {
            printf("pack_dms_SetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rspLen = read(dms, rsp, QMI_MSG_MAX);    
        if(unpack_dms_SetPower(rsp, rspLen,&SetPowerResp)!=0)
        {
            printf("unpack_dms_SetPower error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
    }    
    close (dms);
    return eLITE_FWDWL_APP_OK;
}

int SendSetImagePreferenceToModem_QMI(unpack_dms_SetFirmwarePreference_t *pSetPrefRspFromModem, char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    struct timeval timeout;
    fd_set set;
    int rv = -1;
    memset(&req_ctx, 0, sizeof(req_ctx));
    g_xid++;
    req_ctx.xid = g_xid;

    if (NULL == pSetPrefRspFromModem)
    {
        fprintf (stderr, "%s: pSetPrefRspFromModem is NULL\n", __func__);
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    }
    int dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        fprintf (stderr, "%s: dms<0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    FD_ZERO(&set); /* clear the set */
    FD_SET(dms, &set); /* add our file descriptor to the set */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    memset(&rsp,0,QMI_MSG_MAX);
    memset(&req,0,QMI_MSG_MAX);
    if(pack_dms_SetFirmwarePreference(&req_ctx, req, &reqLen,NULL)!=0)
    {
        printf("pack_dms_SetFirmwarePreference error\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    rtn = write(dms, req, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    rv = select(dms + 1, &set, NULL, NULL, &timeout);
    if(rv<=0)
    {
        // skip unpack anyway
        fprintf (stderr,"error read %d \n", rv);
        close(dms);
        return eLITE_FWDWL_APP_OK;
    }
    rspLen = read(dms, rsp, QMI_MSG_MAX);

    if(unpack_dms_SetFirmwarePreference(rsp, rspLen,pSetPrefRspFromModem)!=0)
    {
        printf("unpack_dms_SetFirmwarePreference failed\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    close(dms);
    return eLITE_FWDWL_APP_OK;
}


int SendImagePreferenceToModem_QMI(pack_fms_SetImagesPreference_t pack_request, unpack_fms_SetImagesPreference_t *pSetPrefRspFromModem, char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    memset(&req_ctx, 0, sizeof(req_ctx));
    g_xid++;
    req_ctx.xid = g_xid;

    if (NULL == pSetPrefRspFromModem)
    {
        fprintf (stderr, "%s: pSetPrefRspFromModem is NULL\n", __func__);
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    }
    int dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        fprintf (stderr, "%s: dms<0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    memset(&rsp,0,QMI_MSG_MAX);
    memset(&req,0,QMI_MSG_MAX);
    if(pack_fms_SetImagesPreference(&req_ctx, req, &reqLen,&pack_request)!=0)
    {
        printf("pack_fms_SetImagesPreference error\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rtn = write(dms, req, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rspLen = read(dms, rsp, QMI_MSG_MAX);
    if(rspLen < QMI_MSG_MAX)
    {
        if(unpack_fms_SetImagesPreference(rsp, rspLen,pSetPrefRspFromModem)!=0)
        {
            printf("unpack_fms_SetImagesPreference failed\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
    }
    close(dms);
    return eLITE_FWDWL_APP_OK;
}

int verify_modem_Crash_Action_QMI (char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    int dms = -1;
    if (NULL == szQMIPath)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    //  Read what the current action is. If modem is not set to RESET, then update the setting.
    {
        unpack_dms_GetCrashAction_t GetCrashActionRsp;
        memset (&GetCrashActionRsp,0,sizeof(GetCrashActionRsp));
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;        
        memset(&rsp,0,QMI_MSG_MAX);
        memset(&req,0,QMI_MSG_MAX);        
        if(pack_dms_GetCrashAction(&req_ctx, req, &reqLen,NULL)!=0)
        {
            printf ("pack_dms_GetCrashAction error\n");
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            close(dms);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        rspLen = read(dms, rsp, QMI_MSG_MAX);
        close(dms);
        if(unpack_dms_GetCrashAction(rsp, rspLen,&GetCrashActionRsp)!=0)
        {
            printf("unpack_dms_GetCrashAction error\n");
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        if (1==GetCrashActionRsp.DevCrashState || 2==GetCrashActionRsp.DevCrashState )
        {
            // Modem in correct state. 
            return eLITE_FWDWL_APP_OK;
        }
        else
        {
            return eLITE_FWDWL_APP_ERR_INVALID_CRASH_ACTION_STATE;
        }
    }        
    return eLITE_FWDWL_APP_OK;
}

int FWVersionAfterDownload_QMI (char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    int dms = -1;
    unpack_dms_GetFirmwareInfo_t fwInfo;    
    if (NULL == szQMIPath)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    
    memset(&rsp,0,QMI_MSG_MAX);
    memset(&req,0,QMI_MSG_MAX);        
    memset (&fwInfo,0,sizeof(fwInfo));    
    
    dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        return eLITE_FWDWL_APP_ERR_QMI;
    }    
    g_xid++;
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = g_xid;        
    if(pack_dms_GetFirmwareInfo(&req_ctx, req, &reqLen,NULL)!=0)
    {
        printf ("pack_dms_GetFirmwareInfo error\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rtn = write(dms, req, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rspLen = read(dms, rsp, QMI_MSG_MAX);
    close(dms);
    if(unpack_dms_GetFirmwareInfo(rsp, rspLen,&fwInfo)!=0)
    {
        printf("unpack_dms_GetFirmwareInfo error\n");
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    fprintf (stderr, "FW info from modem:\n");
    fprintf (stderr, "Model ID            : %s\n", fwInfo.modelid_str);
//    fprintf (stderr, "Boot Version        : %s\n", fwInfo.bootversion_str);
    fprintf (stderr, "FW Version          : %s\n", fwInfo.appversion_str);
//    fprintf (stderr, "SKU                 : %s\n", fwInfo.sku_str);
//    fprintf (stderr, "Package ID          : %s\n", fwInfo.packageid_str);
//    fprintf (stderr, "Carrier string      : %s\n", fwInfo.carrier_str);
//    fprintf (stderr, "PRI Version         : %s\n", fwInfo.priversion_str);
    fprintf (stderr, "Carrier Name        : %s\n", fwInfo.cur_carr_name);
    fprintf (stderr, "Carrier PRI Revision: %s\n", fwInfo.cur_carr_rev);
    return eLITE_FWDWL_APP_OK;
}

