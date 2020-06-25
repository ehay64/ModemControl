#include "lite_fw_dld.h"
#include <errno.h>
    
//#define MBIM_DATA_DEBUG 1    
extern int g_devMode;
extern unsigned int g_xid;
unsigned int mbim_xid = 1;

/* UUID for MBIM over QMI */
const uint8_t mbim_uuid_qmi[] = {0xd1, 0xa3, 0x0b, 0xc2, 0xf9, 0x7a, 0x6e, 0x43, 0xbf, 
                    0x65, 0xc7, 0xe2 , 0x4f, 0xb0, 0xf0, 0xd3 };

#define QMI_HDR_LEN 6
#define MBIM_CMD_MSG_HDR_LEN 48
#define MAX_MBIM_CONTROL_TRANSFER_SIZE	0x00001000
#define MBIM_INDEX_OPEN_DONE_STATUS_CODE 12

#define MBIM_PUT_UINT32_LE(p,val) \
    do{ \
        (p)[0] = (val); \
        (p)[1] = (val) >> 8; \
        (p)[2] = (val) >> 16; \
        (p)[3] = (val) >> 24; \
    } while(0)


#define MBIM_GET_UINT32_LE(p,val) \
    do { \
       (val) = ((p)[0] | (((p)[1] << 8) & 0x0000ff00) | \
       (((p)[2] << 16) & 0x00ff0000) | (((p)[3] << 24) & 0xff000000)); \
    } while(0)


void print_hex(
   void *      pBuffer,
   int    	   bufSize,
   uint32_t    type )
{

#ifndef MBIM_DATA_DEBUG
    UNUSEDPARAM(pBuffer);
    UNUSEDPARAM(bufSize);
    UNUSEDPARAM(type);

#else
    char * pPrintBuf;
    uint16_t pos;
    int status;

    if (bufSize <= 0)
    {
      return;
    }

    pPrintBuf =malloc( bufSize * 3 + 1);
    if (pPrintBuf == NULL)
    {
        printf("Unable to allocate buffer\n" );
        return;
    }
    memset( pPrintBuf, 0 , bufSize * 3 + 1 );

    for (pos = 0; pos < bufSize; pos++)
    {
        status = snprintf( (pPrintBuf + (pos * 3)),
                         4,
                         "%02X ",
                         *(uint8_t *)(pBuffer + pos) );
        if (status != 3)
        {
            printf( "snprintf error %d\n", status );
            free( pPrintBuf );
            return;
        }
    }

    printf("MBIM buffer length  %d\n", bufSize);

    if (type == 1)       
        printf( "MBIM WRITE   : %s\n", pPrintBuf );
   
    else if (type == 2)
        printf( "MBIM READ    : %s\n", pPrintBuf );   

    free( pPrintBuf );
    pPrintBuf = NULL;
    return;
#endif
}

int mbim_write(int fd, uint8_t *mbim_msg, uint32_t reqLen)
{
    int rtn = 0;

    rtn = write(fd, mbim_msg, reqLen);
    if (rtn < 0)
    {
        #ifdef MBIM_DATA_DEBUG
        printf("mbim write error %d errcode %d\n", rtn, errno);
        #endif
    }
    else if (rtn < (int)reqLen)
    {
        printf("mbim write len error %d\n", rtn);
        close(fd);
    }
    return rtn;
} 

int mbim_read(int fd, uint8_t *mbim_msg, uint32_t maxLen)
{
    int rtn = 0;
    uint32_t msgType;    

    while(1) {
        memset(mbim_msg,0, maxLen);
        rtn = read(fd,mbim_msg, maxLen);
        print_hex((void *)mbim_msg,rtn,2);
        if (rtn < 0)
        {
            #ifdef MBIM_DATA_DEBUG
            printf("mbim read error %d errcode %d\n", rtn, errno);
            #endif         
            break;
        }
        else if (rtn < ((int)(sizeof(struct mbim_header))))
        {
            printf("mbim read len error %d\n", rtn);
            close(fd);
            break;
        }
        MBIM_GET_UINT32_LE(mbim_msg, msgType);
        if (msgType == MBIM_MESSAGE_TYPE_INDICATE_STATUS)
            continue;
        else
            break;
    }
    return rtn;
} 

// Create MBIM CMD message
void mbim_create_mbimcmdmsg(uint8_t *mbimbuf, 
                            uint8_t *qmibuf, 
                            uint32_t qmiLen, 
                            uint32_t cmd_type, 
                            uint32_t *pTotalLen)
{
    struct mbim_full_message cmdMsg;
    uint32_t mbim_hdr_len;
    uint32_t tLen;

    memset(&cmdMsg,0, sizeof(struct mbim_full_message));
   
    /* fill command message */
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.message.command.fragment_header.total,1);
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.message.command.fragment_header.current,0);

    memcpy(cmdMsg.message.command.service_id,mbim_uuid_qmi, 16);

    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.message.command.command_id,MBIM_CID_QMI_MSG);
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.message.command.command_type,cmd_type);
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.message.command.buffer_length,qmiLen);

    mbim_hdr_len = (uint32_t)(sizeof(struct mbim_header) + sizeof(struct mbim_command_message));
    tLen = mbim_hdr_len + qmiLen;
    *pTotalLen = tLen;  

    /* fill cmd message header */
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.header.type,MBIM_MESSAGE_TYPE_COMMAND);
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.header.length, tLen);
    MBIM_PUT_UINT32_LE((uint8_t *)&cmdMsg.header.transaction_id,
                       mbim_xid);

    mbim_xid++;

    /* copy qmi data */
    memcpy(mbimbuf + mbim_hdr_len , qmibuf, qmiLen);

    /* copy MBIM header */
    memcpy(mbimbuf , &cmdMsg, mbim_hdr_len);
    if (tLen > 0) {        
        if (tLen > 256)
            tLen = 256;
        print_hex((void *)mbimbuf,tLen, 1);
    }   
}    

void mbim_add_qmi_header(uint8_t *buf, uint16_t msgLen, uint8_t svc, uint8_t cliId)
{
    uint16_t len;

    len = msgLen + 5;  // QMI header Len -1
    buf[0] = 0x01;
    buf[1] = (uint8_t)len & 0x00ff;
    buf[2] = (uint8_t)((len >> 8) & 0x00ff);
    buf[3] = 0x00;     // control flags (sender)
    buf[4] = svc;
    buf[5] = cliId;
}

int mbim_verify_cmd_done(uint8_t *mbim_rsp, uint32_t rtn)
{
    uint32_t rspType = 0;
    uint32_t len;
    uint32_t status_code;

    UNUSEDPARAM(rtn);

    /* fetch the type of response */
    MBIM_GET_UINT32_LE(mbim_rsp, rspType);

    if (rspType != MBIM_MESSAGE_TYPE_COMMAND_DONE)
        return eLITE_FWDWL_APP_ERR_QMI;

    MBIM_GET_UINT32_LE(mbim_rsp + 4, len);

    if (len < MBIM_CMD_MSG_HDR_LEN)
        return eLITE_FWDWL_APP_ERR_QMI;

    /* service id */
    if (memcmp(mbim_rsp + 20, mbim_uuid_qmi, 16) != 0)
        return eLITE_FWDWL_APP_ERR_QMI;

    MBIM_GET_UINT32_LE(mbim_rsp + 40, status_code);

#ifdef MBIM_DATA_DEBUG
    printf("mbim cmd done status code %d\n", status_code);
#else
    UNUSEDPARAM(status_code);
#endif

    return eLITE_FWDWL_APP_OK; 
}

int mbim_verify_qmi_hdr_in_resp(uint8_t *qmi_rsp, uint8_t cliId, uint8_t svc)
{
    if ((qmi_rsp[0] != 0x01) && (qmi_rsp[3] != 0x80) &&
        (qmi_rsp[4] != svc) && (qmi_rsp[5] != cliId)) {
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    return eLITE_FWDWL_APP_OK;
}
    
int mbim_qmi_get_client_id(int fd, uint8_t svc, uint8_t *pclid)
{
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];    
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint8_t qmi_ctl_clid_sdu[8] = {0x22, 0x00, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00};
    uint32_t qmiLen;
    uint32_t totalLen = 0;
    uint32_t qmiRespLen;
    int rtn;    

    memset(mbim_msg,0, sizeof(mbim_msg));
    memset(qmi_msg,0, sizeof(qmi_msg));
    qmi_msg[QMI_HDR_LEN] = 0x00; // QMI control flag (request)

    g_xid++;
    qmi_msg[QMI_HDR_LEN + 1] = (uint8_t)g_xid;

    /* update the service at last byte in SDU */
    qmi_ctl_clid_sdu[7] = svc;
    memcpy(&qmi_msg[QMI_HDR_LEN + 2],&qmi_ctl_clid_sdu[0],sizeof(qmi_ctl_clid_sdu));

    mbim_add_qmi_header(qmi_msg, sizeof(qmi_ctl_clid_sdu) + 2, 0x00 /* Ctl svc */, 0x00);

    qmiLen = (uint32_t)(sizeof(qmi_ctl_clid_sdu) + 2) + QMI_HDR_LEN;

    mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, qmiLen, MBIM_MESSAGE_COMMAND_TYPE_SET, &totalLen);

    rtn = mbim_write(fd, mbim_msg, totalLen);

    if (rtn != (int)totalLen)
    {
        printf("mbim cmd ifc write %d wrote %d\n", totalLen, rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    rtn = mbim_read(fd,mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);
    if (rtn < ((int)(sizeof(struct mbim_header))))
    {
        printf("mbim cmd ifc read %d\n", rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify that it is a correct MBIM comamnd response */
    rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
    if (rtn != eLITE_FWDWL_APP_OK)
    {
        printf("mbim cmd response error received %d\n", rtn);
        goto closing;
    }
    MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

    if (qmiRespLen > 0) 
    {
        uint32_t lenRecv;

        memset(qmi_msg,0, sizeof(qmi_msg));
        memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
        lenRecv = (*(qmi_msg + 1) & 0x00ff) | ((*(qmi_msg + 2) << 8) & 0xff00);

        if (lenRecv != 0x17)
        {
            printf("qmi resp len for GET_CLIENT_ID_RESP not correct %d\n", lenRecv);
            goto closing;
        }
        
        /* some checking for qmi hdr */
        if ((qmi_msg[3] != 0x80) && (qmi_msg[4] != 0x00) &&
            (qmi_msg[5] != 0x00))
        {
            printf("qmi resp not correct\n");
            goto closing;
        }

        if (qmi_msg[0x16] == svc) {
            /* get client id */
            *pclid = qmi_msg[0x17];
            return eLITE_FWDWL_APP_OK;             
        }
    }   
    printf("qmi resp for GET_CLIENT_ID_RESP not correct \n");

closing:
    close(fd);
    return eLITE_FWDWL_APP_ERR_QMI;    
}

int configure_mbim_control_size(int fd)
{
    struct mbim_full_message openMsg;
    uint8_t mbimopen_rsp[256];
    uint16_t reqLen;
    uint32_t rspType = 0;
    uint32_t statusCode;
    uint32_t max_ctl_len = MAX_MBIM_CONTROL_TRANSFER_SIZE;
    int rtn;
    
    memset(&openMsg,0, sizeof(struct mbim_full_message)); 

    /* fill open message */
    MBIM_PUT_UINT32_LE((uint8_t *)&openMsg.message.open.max_control_transfer,max_ctl_len);


    /* fill open message header */
    MBIM_PUT_UINT32_LE((uint8_t *)&openMsg.header.type,MBIM_MESSAGE_TYPE_OPEN);
    MBIM_PUT_UINT32_LE((uint8_t *)&openMsg.header.length,
                       (uint32_t)(sizeof(struct mbim_header) + sizeof(struct mbim_open_message)));
    MBIM_PUT_UINT32_LE((uint8_t *)&openMsg.header.transaction_id,
                       mbim_xid);

    mbim_xid++;

    reqLen = sizeof(struct mbim_header) + sizeof(struct mbim_open_message);

    print_hex((void *)&openMsg, reqLen, 1);
    rtn = mbim_write(fd, (uint8_t *)&openMsg, reqLen);

    if (rtn != reqLen)
    {
        printf("mbim open ifc write %d wrote %d\n", reqLen, rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rtn = mbim_read(fd, mbimopen_rsp, 256);
    if (rtn < ((int)(sizeof(struct mbim_header))))
    {
        printf("mbim open ifc read %d\n", rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }    
    
    /* fetch the type of response */
    MBIM_GET_UINT32_LE(mbimopen_rsp,rspType);
    
    if (rspType == MBIM_MESSAGE_TYPE_OPEN_DONE)
    {
        /* get status code */
        MBIM_GET_UINT32_LE(&mbimopen_rsp[MBIM_INDEX_OPEN_DONE_STATUS_CODE],statusCode);
        if (statusCode != 0)
        {
            printf("mbim open done status  %d\n", statusCode);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
    }
    else
    {
        printf("mbim open respons type error  %d\n", rspType);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    return eLITE_FWDWL_APP_OK;
}

int open_mbim(char *szMBIMPath)
{
    int fd = open(szMBIMPath, O_RDWR);
    return fd;
}

int ResetModem_MBIM (char *szMBIMPath)
{
    pack_qmi_t req_ctx;
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint16_t reqLen;
    int rtn;
    int fd = -1;
    uint8_t client_id = 0; 
    uint32_t totalLen = 0;
    uint32_t qmiRespLen = 0;
    int i = 0;

    pack_dms_SetPower_t SetPowerReq;
    unpack_dms_SetPower_t SetPowerResp;
    unpack_dms_GetPower_t GetPowerResp;

#ifdef MBIM_DATA_DEBUG
    printf("%s:\n",__func__);
#endif

    fd = open_mbim(szMBIMPath);
    if(fd < 0)
    {
        fprintf (stderr, "%s: mbim fd < 0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    /* configure max control size as 0x1000 */
    rtn = configure_mbim_control_size(fd);
    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    rtn = mbim_qmi_get_client_id(fd, 2 /* DMS */, &client_id);

    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;    
    {
        // Set modem offline
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;        
        memset(qmi_msg,0,QMI_MSG_MAX);   
        SetPowerReq.mode = DMS_PM_OFFLINE;
        
        if(pack_dms_SetPower(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen, &SetPowerReq)!=0)
        {
            printf ("pack_dms_SetPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        // Add qmi header
        mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

        memset(mbim_msg,0, sizeof(mbim_msg));

        // Add MBIM CMD msg header
        mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                               reqLen + QMI_HDR_LEN, 
                               MBIM_MESSAGE_COMMAND_TYPE_SET, 
                               &totalLen);
        
        rtn = mbim_write(fd, mbim_msg, totalLen);
        if (rtn != (int)totalLen)
        {
            printf("mbim dms set power write %d wrote %d\n", totalLen, rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        memset(qmi_msg,0,QMI_MSG_MAX);
        rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

        if (rtn < ((int)(sizeof(struct mbim_header))))
        {
            printf("mbim dms set power cmd read %d\n", rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }      

        /* verify that it is a correct MBIM command response */
        rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
        if (rtn != eLITE_FWDWL_APP_OK)
        {
           printf("mbim dms set power resp error received %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }
        MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

        if (qmiRespLen < QMI_HDR_LEN) 
        {
           printf("mbim dms set power qmi length error %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify qmi header data */
        memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
        rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

        if (rtn != eLITE_FWDWL_APP_OK)
        {
            printf("mbim qmi hdr not correct in rsp %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        qmiRespLen = qmiRespLen - QMI_HDR_LEN;  
        if(unpack_dms_SetPower(&qmi_msg[QMI_HDR_LEN], qmiRespLen, &SetPowerResp)!=0)
        {
            printf("unpack_dms_SetPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }        
    }
    usleep(SECOND);

    // Get power
    while ( i < MAX_POWER_REQUEST_RETRY)
    {
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        memset(&GetPowerResp, 0, sizeof(GetPowerResp));
        req_ctx.xid = g_xid;        
        memset(qmi_msg,0,QMI_MSG_MAX); 
        
        if(pack_dms_GetPower(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen, NULL)!=0)
        {
            printf ("pack_dms_GetPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        // Add qmi header
        mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

        memset(mbim_msg,0, sizeof(mbim_msg));

        // Add MBIM CMD msg header
        mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                               reqLen + QMI_HDR_LEN, 
                               MBIM_MESSAGE_COMMAND_TYPE_SET, 
                               &totalLen);
        
        rtn = mbim_write(fd, mbim_msg, totalLen);
        if (rtn != (int)totalLen)
        {
            printf("mbim dms get power write %d wrote %d\n", totalLen, rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        memset(qmi_msg,0,QMI_MSG_MAX);
        rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

        if (rtn < ((int)(sizeof(struct mbim_header))))
        {
            printf("mbim dms get power cmd read %d\n", rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }      

        /* verify that it is a correct MBIM command response */
        rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
        if (rtn != eLITE_FWDWL_APP_OK)
        {
           printf("mbim dms get power resp error received %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }
        MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

        if (qmiRespLen < QMI_HDR_LEN) 
        {
           printf("mbim dms get power qmi length error %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify qmi header data */
        memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
        rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

        if (rtn != eLITE_FWDWL_APP_OK)
        {
            printf("mbim qmi hdr not correct in rsp %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        qmiRespLen = qmiRespLen - QMI_HDR_LEN;  
        if(unpack_dms_GetPower(&qmi_msg[QMI_HDR_LEN], qmiRespLen, &GetPowerResp)!=0)
        {
            printf("unpack_dms_getPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
#ifdef MBIM_DATA_DEBUG
        printf("mbim current modem power %d\n",GetPowerResp.OperationMode);
#endif
        if (GetPowerResp.OperationMode == DMS_PM_OFFLINE)
            break;
        i++;
        usleep(SECOND);       
    }

    if (i == MAX_POWER_REQUEST_RETRY) {
        printf("modem could not set in power offline mode!\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    {
        // Reset the modem
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;                
        memset(&qmi_msg,0,QMI_MSG_MAX);   
        SetPowerReq.mode = DMS_PM_RESET;
        
        if(pack_dms_SetPower(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen,&SetPowerReq)!=0)
        {
            printf("pack_dms_SetPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        // Add qmi header
        mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

        memset(mbim_msg,0, sizeof(mbim_msg));

        // Add MBIM CMD msg header
        mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                               reqLen + QMI_HDR_LEN, 
                               MBIM_MESSAGE_COMMAND_TYPE_SET, 
                               &totalLen);
        
        rtn = mbim_write(fd, mbim_msg, totalLen);
        if (rtn != (int)totalLen)
        {
            printf("mbim dms set power write %d wrote %d\n", totalLen, rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        memset(&qmi_msg,0,QMI_MSG_MAX);  
        rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

        if (rtn < ((int)(sizeof(struct mbim_header))))
        {
            printf("mbim dms set power cmd read %d\n", rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify that it is a correct MBIM command response */
        rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
        if (rtn != eLITE_FWDWL_APP_OK)
        {
           printf("mbim dms set power resp error received %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }
        MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

        if (qmiRespLen < QMI_HDR_LEN) 
        {
           printf("mbim dms set power qmi length error %d\n", rtn);
           close(fd);
           return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify qmi header data */
        memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
        rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

        if (rtn != eLITE_FWDWL_APP_OK)
        {
            printf("mbim qmi hdr not correct in rsp %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        qmiRespLen = qmiRespLen - QMI_HDR_LEN;  
        if(unpack_dms_SetPower(&qmi_msg[QMI_HDR_LEN], qmiRespLen, &SetPowerResp)!=0)
        {
            printf("unpack_dms_SetPower error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
    }   
    close (fd);
    return eLITE_FWDWL_APP_OK;
}

int SendSetImagePreferenceToModem_MBIM(unpack_dms_SetFirmwarePreference_t *pSetPrefRspFromModem, char *szMBIMPath)
{
    pack_qmi_t req_ctx;
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint16_t reqLen;
    int rtn;
    int fd = -1;
    uint8_t client_id = 0; 
    uint32_t totalLen = 0;
    uint32_t qmiRespLen = 0;
    struct timeval timeout;
    fd_set set;
    int rv = -1;

#ifdef MBIM_DATA_DEBUG
    printf("%s:\n",__func__);
#endif

    if (NULL == pSetPrefRspFromModem)
    {
        fprintf (stderr, "%s: pSetPrefRspFromModem is NULL\n", __func__);
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    }

    fd = open_mbim(szMBIMPath);
    if(fd < 0)
    {
        fprintf (stderr, "%s: mbim fd < 0 error %d \n", __func__, errno);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    /* configure max control size as 0x1000 */
    rtn = configure_mbim_control_size(fd);
    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    rtn = mbim_qmi_get_client_id(fd, 2 /* DMS */, &client_id);

    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;    

    FD_ZERO(&set); /* clear the set */
    FD_SET(fd, &set); /* add our file descriptor to the set */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    memset(&req_ctx, 0, sizeof(req_ctx));
    g_xid++;
    req_ctx.xid = g_xid;

    memset(&qmi_msg,0,QMI_MSG_MAX);
    if(pack_dms_SetFirmwarePreference(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen,NULL)!=0)
    {
        printf("pack_dms_SetFirmwarePreference error\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    // Add qmi header
    mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

    memset(mbim_msg,0, sizeof(mbim_msg));

    // Add MBIM CMD msg header
    mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                           reqLen + QMI_HDR_LEN, 
                           MBIM_MESSAGE_COMMAND_TYPE_SET, 
                           &totalLen);
        
    rtn = mbim_write(fd, mbim_msg, totalLen);
    if (rtn != (int)totalLen)
    {
        printf("mbim dms set firmware pref write %d wrote %d\n", totalLen, rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    rv = select(fd + 1, &set, NULL, NULL, &timeout);
    if(rv<=0)
    {
        // skip unpack anyway
        fprintf (stderr,"error read %d \n", rv);
        close(fd);
        return eLITE_FWDWL_APP_OK;
    }

    memset(&qmi_msg,0,QMI_MSG_MAX);  
    rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

    if (rtn < ((int)(sizeof(struct mbim_header))))
    {
        printf("mbim dms set firm pref read %d\n", rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify that it is a correct MBIM command response */
    rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
    if (rtn != eLITE_FWDWL_APP_OK)
    {
       printf("mbim dms set firm pref error received %d\n", rtn);
       close(fd);
       return eLITE_FWDWL_APP_ERR_QMI;
    }
    MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

    if (qmiRespLen < QMI_HDR_LEN) 
    {
       printf("mbim dms set firm pref qmi length error %d\n", rtn);
       close(fd);
       return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify qmi header data */
    memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
    rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

    if (rtn != eLITE_FWDWL_APP_OK)
    {
        printf("mbim qmi hdr not correct in rsp %d\n", rtn);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    qmiRespLen = qmiRespLen - QMI_HDR_LEN;  
    if(unpack_dms_SetFirmwarePreference(&qmi_msg[QMI_HDR_LEN], qmiRespLen, pSetPrefRspFromModem)!=0)
    {
        printf("unpack_dms_SetFirmwarePreference failed\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    close(fd);
    return eLITE_FWDWL_APP_OK;
}


int SendImagePreferenceToModem_MBIM(pack_fms_SetImagesPreference_t pack_request, unpack_fms_SetImagesPreference_t *pSetPrefRspFromModem, char *szMBIMPath)
{
    pack_qmi_t req_ctx;
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint16_t reqLen;
    int rtn;
    int fd = -1;
    uint8_t client_id = 0; 
    uint32_t totalLen = 0;
    uint32_t qmiRespLen = 0;

#ifdef MBIM_DATA_DEBUG
    printf("%s:\n",__func__);
#endif

    if (NULL == pSetPrefRspFromModem)
    {
        fprintf (stderr, "%s: pSetPrefRspFromModem is NULL\n", __func__);
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    }

    memset(&req_ctx, 0, sizeof(req_ctx));
    g_xid++;
    req_ctx.xid = g_xid;

    fd = open_mbim(szMBIMPath);
    if(fd < 0)
    {
        fprintf (stderr, "%s: mbim fd<0\n error %d", __func__, errno);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    /* configure max control size as 0x1000 */
    rtn = configure_mbim_control_size(fd);
    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    rtn = mbim_qmi_get_client_id(fd, 2 /* DMS */, &client_id);

    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    memset(qmi_msg,0,QMI_MSG_MAX);
    if(pack_fms_SetImagesPreference(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen, &pack_request)!=0)
    {
        printf("pack_dms_SetFirmwarePreference error\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    // Add qmi header
    mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

    memset(mbim_msg,0, sizeof(mbim_msg));

    // Add MBIM CMD msg header
    mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                           reqLen + QMI_HDR_LEN, 
                           MBIM_MESSAGE_COMMAND_TYPE_SET, 
                           &totalLen);
        
    rtn = mbim_write(fd, mbim_msg, totalLen);
    if (rtn != (int)totalLen)
    {
        printf("mbim dms set firmware pref write %d wrote %d\n", totalLen, rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    memset(mbim_msg,0, sizeof(mbim_msg));
    memset(qmi_msg,0,QMI_MSG_MAX);  
    rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

    if (rtn < ((int)(sizeof(struct mbim_header))))
    {
        printf("mbim dms set firm pref read %d\n", rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify that it is a correct MBIM command response */
    rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
    if (rtn != eLITE_FWDWL_APP_OK)
    {
       printf("mbim dms set firm pref error received %d\n", rtn);
       close(fd);
       return eLITE_FWDWL_APP_ERR_QMI;
    }

    MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

    if (qmiRespLen < QMI_HDR_LEN) 
    {
       printf("mbim dms set firm pref qmi length error %d\n", rtn);
       close(fd);
       return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify qmi header data */
    memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
    rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

    if (rtn != eLITE_FWDWL_APP_OK)
    {
        printf("mbim qmi hdr not correct in rsp %d\n", rtn);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    qmiRespLen = qmiRespLen - QMI_HDR_LEN;  
    if(unpack_fms_SetImagesPreference(&qmi_msg[QMI_HDR_LEN], qmiRespLen, pSetPrefRspFromModem)!=0)
    {
        printf("unpack_dms_SetFirmwarePreference failed\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    close(fd);
    return eLITE_FWDWL_APP_OK;
}

int verify_modem_Crash_Action_MBIM (char *szMBIMPath)
{
    pack_qmi_t req_ctx;
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint16_t reqLen;
    int rtn;
    int fd = -1;
    uint8_t client_id = 0; 
    uint32_t totalLen = 0;
    uint32_t qmiRespLen = 0;

#ifdef MBIM_DATA_DEBUG
    printf("%s:\n",__func__);
#endif

    if (NULL == szMBIMPath)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;

    fd = open_mbim(szMBIMPath);
    if(fd < 0)
    {
        fprintf (stderr, "%s: mbim fd<0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    /* configure max control size as 0x1000 */
    rtn = configure_mbim_control_size(fd);
    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    rtn = mbim_qmi_get_client_id(fd, 2 /* DMS */, &client_id);

    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    //  Read what the current action is. If modem is not set to RESET, then update the setting.
    {
        unpack_dms_GetCrashAction_t GetCrashActionRsp;
        memset (&GetCrashActionRsp,0,sizeof(GetCrashActionRsp));
        g_xid++;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = g_xid;        
        memset(qmi_msg,0,QMI_MSG_MAX);   
        if(pack_dms_GetCrashAction(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen,NULL)!=0)
        {
            printf ("pack_dms_GetCrashAction error\n");
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        // Add qmi header
        mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

        memset(mbim_msg,0, sizeof(mbim_msg));

        // Add MBIM CMD msg header
        mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                               reqLen + QMI_HDR_LEN, 
                               MBIM_MESSAGE_COMMAND_TYPE_SET, 
                               &totalLen);
        
        rtn = mbim_write(fd, mbim_msg, totalLen);
        if (rtn != (int)totalLen)
        {
            printf("mbim dms get crash write %d wrote %d\n", totalLen, rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        memset(qmi_msg,0,QMI_MSG_MAX);  
        rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

        if (rtn < ((int)(sizeof(struct mbim_header))))
        {
            printf("mbim dms get crash read %d\n", rtn);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify that it is a correct MBIM command response */
        rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
        if (rtn != eLITE_FWDWL_APP_OK)
        {
            printf("mbim dms get crash error received %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

        if (qmiRespLen < QMI_HDR_LEN) 
        {
            printf("mbim dms get crash qmi length error %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }

        /* verify qmi header data */
        memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
        rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

        if (rtn != eLITE_FWDWL_APP_OK)
        {
            printf("mbim qmi hdr not correct in rsp %d\n", rtn);
            close(fd);
            return eLITE_FWDWL_APP_ERR_QMI;
        }
        qmiRespLen = qmiRespLen - QMI_HDR_LEN;
        close(fd);
        if(unpack_dms_GetCrashAction(&qmi_msg[QMI_HDR_LEN], qmiRespLen,&GetCrashActionRsp)!=0)
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

int FWVersionAfterDownload_MBIM (char *szMBIMPath)
{
    pack_qmi_t req_ctx;
    uint8_t mbim_msg[MAX_MBIM_CONTROL_TRANSFER_SIZE];
    uint8_t qmi_msg[QMI_MSG_MAX];
    uint16_t reqLen;
    int rtn;
    int fd = -1;
    uint8_t client_id = 0; 
    uint32_t totalLen = 0;
    uint32_t qmiRespLen = 0;

    unpack_dms_GetFirmwareInfo_t fwInfo;

#ifdef MBIM_DATA_DEBUG
    printf("%s:\n",__func__);
#endif
  
    if (NULL == szMBIMPath)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    
    memset(qmi_msg,0,QMI_MSG_MAX);   
    memset (&fwInfo,0,sizeof(fwInfo));    
    
    fd = open_mbim(szMBIMPath);
    if(fd < 0)
    {
        fprintf (stderr, "%s: mbim fd<0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    /* configure max control size as 0x1000 */
    rtn = configure_mbim_control_size(fd);
    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    rtn = mbim_qmi_get_client_id(fd, 2 /* DMS */, &client_id);

    if (rtn != eLITE_FWDWL_APP_OK)
        return rtn;

    g_xid++;
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = g_xid;        
    if(pack_dms_GetFirmwareInfo(&req_ctx, &qmi_msg[QMI_HDR_LEN], &reqLen,NULL)!=0)
    {
        printf ("pack_dms_GetFirmwareInfo error\n");
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    // Add qmi header
    mbim_add_qmi_header(qmi_msg, reqLen, 0x02 /* dms svc */, client_id);

    memset(mbim_msg,0, sizeof(mbim_msg));

    // Add MBIM CMD msg header
    mbim_create_mbimcmdmsg(mbim_msg, qmi_msg, 
                           reqLen + QMI_HDR_LEN, 
                           MBIM_MESSAGE_COMMAND_TYPE_SET, 
                           &totalLen);
        
    rtn = mbim_write(fd, mbim_msg, totalLen);
    if (rtn != (int)totalLen)
    {
        printf("mbim dms get firm info write %d wrote %d\n", totalLen, rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    memset(qmi_msg,0,QMI_MSG_MAX);  
    rtn = mbim_read(fd, mbim_msg, MAX_MBIM_CONTROL_TRANSFER_SIZE);

    if (rtn < ((int)(sizeof(struct mbim_header))))
    {
        printf("mbim dms get firm info read %d\n", rtn);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify that it is a correct MBIM command response */
    rtn  = mbim_verify_cmd_done(mbim_msg, rtn);
    if (rtn != eLITE_FWDWL_APP_OK)
    {
        printf("mbim dms get firm error received %d\n", rtn);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    MBIM_GET_UINT32_LE(mbim_msg + 44, qmiRespLen);

    if (qmiRespLen < QMI_HDR_LEN) 
    {
        printf("mbim dms get firm qmi length error %d\n", rtn);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }

    /* verify qmi header data */
    memcpy(qmi_msg,mbim_msg + MBIM_CMD_MSG_HDR_LEN, qmiRespLen);
    rtn = mbim_verify_qmi_hdr_in_resp(qmi_msg,client_id, 2 /* svc dms */);

    if (rtn != eLITE_FWDWL_APP_OK)
    {
        printf("mbim qmi hdr not correct in rsp %d\n", rtn);
        close(fd);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    qmiRespLen = qmiRespLen - QMI_HDR_LEN;
    close(fd);
    if(unpack_dms_GetFirmwareInfo(&qmi_msg[QMI_HDR_LEN], qmiRespLen,&fwInfo)!=0)
    {
        printf("unpack_dms_GetFirmwareInfo error\n");
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    fprintf (stderr, "FW info from modem:\n");
    fprintf (stderr, "Model ID            : %s\n", fwInfo.modelid_str);
    fprintf (stderr, "FW Version          : %s\n", fwInfo.appversion_str);
    fprintf (stderr, "Carrier Name        : %s\n", fwInfo.cur_carr_name);
    fprintf (stderr, "Carrier PRI Revision: %s\n", fwInfo.cur_carr_rev);
    return eLITE_FWDWL_APP_OK;
}
