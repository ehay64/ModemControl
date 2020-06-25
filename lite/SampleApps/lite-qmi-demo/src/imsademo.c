#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <packingdemo.h> //TODO rename to lite-qmi-demo.h
#include "imsa.h"

int imsa_validate_dummy_unpack();

int qmi = 0;
int stopReadThread = 0;

uint8_t  req[QMI_MSG_MAX]={'0'};
uint16_t len = QMI_MSG_MAX;
pack_qmi_t ctx;
uint16_t   xid = 0x101;

void* readThread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  req[QMI_MSG_MAX]={'0'};

    int rtn;
    ssize_t rlen;
    printf("%s param %p\n", __func__, ptr);
    while(~stopReadThread)
    {
        memset (req,0,QMI_MSG_MAX);
        if(qmi<0)
            return NULL;
        rtn = rlen = read(qmi, req, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eIMSA, req, rlen, &rsp_ctx);
            printf("<< receiving %s, Len: %zu\n", qmi_msg, rlen);
            dump_hex(rlen, req);

            if (eIND == rsp_ctx.type)
                printf("IMSA IND: ");
            else if (eRSP == rsp_ctx.type)
                printf("IMSA RSP: ");
            printf("msgid 0x%02x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                case eQMI_IMSA_INDICATION_REGISTER:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_imsa_SLQSRegisterIMSAIndication_t output;
                        rtn = unpack_imsa_SLQSRegisterIMSAIndication(
                                req, rlen, &output);

                        printf("unpack_imsa_SLQSRegisterIMSAIndication returned %d\n", rtn);
                        swi_uint256_print_mask (output.ParamPresenceMask);

                        if(swi_uint256_get_bit (output.ParamPresenceMask, 2))
                            printf("%s Result: %d\n",__FUNCTION__, output.Tlvresult);

                    }
                    break;

                case eQMI_IMSA_GET_REGISTRATION_STATUS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_imsa_SLQSGetIMSARegStatus_t output;
                        rtn = unpack_imsa_SLQSGetIMSARegStatus(
                                req, rlen, &output);

                        swi_uint256_print_mask (output.ParamPresenceMask);
                        printf("unpack_imsa_SLQSGetIMSARegStatus returned %d\n", rtn);
                        if (!rtn)
                        {
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 16))
                                printf("\treg status 0x%x\n", output.ImsRegStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 17))
                                printf("\terr code 0x%x\n", output.ImsRegErrCode);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 18))
                                printf("\tnew reg status 0x%x\n", output.NewImsRegStatus);
                        }
                    }
                    break;

                case eQMI_IMSA_GET_SERVICE_STATUS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_imsa_SLQSGetIMSAServiceStatus_t output;
                        rtn = unpack_imsa_SLQSGetIMSAServiceStatus(
                                req, rlen, &output);

                        printf("unpack_imsa_SLQSGetIMSAServiceStatus returned %d\n", rtn);
                        swi_uint256_print_mask (output.ParamPresenceMask);

                        if (!rtn)
                        {
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 16))
                                printf("\tsms status 0x%x\n", output.SmsServiceStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 17))
                                printf("\tvoip status 0x%x\n", output.VoipServiceStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 18))
                                printf("\tvt status 0x%x\n", output.VtServiceStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 19))
                                printf("\tsms rat 0x%x\n", output.SmsServiceRat);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 20))
                                printf("\tvoip rat 0x%x\n", output.VoipServiceRat);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 21))
                                printf("\tvt rat 0x%x\n", output.VtServiceRat);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 22))
                                printf("\tut status 0x%x\n", output.UtServiceStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 23))
                                printf("\tut rat 0x%x\n", output.UtServiceRat);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 24))
                                printf("\tvs status 0x%x\n", output.VsServiceStatus);
                            if(swi_uint256_get_bit (output.ParamPresenceMask, 25))
                                printf("\tvs rat 0x%x\n", output.VsServiceRat);
                        }
                    }
                    break;
                case eQMI_IMSA_REGISTRATION_STATUS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_imsa_SLQSImsaRegStatusCallBack_ind_t  imsaRegStatus;
                         rtn = unpack_imsa_SLQSImsaRegStatusCallBack_ind(req, 
                                                         rlen, 
                                                         &imsaRegStatus);
                         printf("unpack QMI_IMSA_REGISTRATION_STATUS_IND result :%d\n",rtn);
                         swi_uint256_print_mask (imsaRegStatus.ParamPresenceMask);
                         if (rtn == eQCWWAN_ERR_NONE) {
                             if(swi_uint256_get_bit (imsaRegStatus.ParamPresenceMask, 1))
                                 printf("IMS registration status %d\n",
                                         imsaRegStatus.IMSRegistration.ImsRegistered);
                             if(swi_uint256_get_bit (imsaRegStatus.ParamPresenceMask, 16))
                                 printf("IMS registration error code %d\n",
                                         imsaRegStatus.IMSRegistrationError.ErrorCode);
                             if(swi_uint256_get_bit (imsaRegStatus.ParamPresenceMask, 17))
                                 printf("New IMS registration status %d\n",
                                         imsaRegStatus.NewIMSRegistration.ImsRegStatus);
                         }                             
                     }
                     break;
                case eQMI_IMSA_SERVICE_STATUS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t  imsaSvcStatus;
                         rtn = unpack_imsa_SLQSImsaSvcStatusCallBack_ind(req, 
                                                         rlen, 
                                                         &imsaSvcStatus);
                         printf("unpack QMI_IMSA_SERVICE_STATUS_IND result :%d\n",rtn);
                         swi_uint256_print_mask (imsaSvcStatus.ParamPresenceMask);
                         if (rtn == eQCWWAN_ERR_NONE) {
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 16))
                                 printf("SMS service status %u\n",
                                        imsaSvcStatus.SmsService.SmsSvcStatus);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 17))
                                 printf("VOIP service status %u\n",
                                         imsaSvcStatus.VoipService.VoipSvcStatus);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 18))
                                 printf("VT service status %u\n",
                                         imsaSvcStatus.VtService.VtSvcStatus);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 19))
                                 printf("UT service status %u\n",
                                         imsaSvcStatus.UtService.UtSvcStatus);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 20))
                                 printf("SMS RAT %u\n",
                                        imsaSvcStatus.SmsRat.SmsRatVal);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 21))
                                 printf("VOIP RAT %u\n",
                                         imsaSvcStatus.VoipRat.VoipRatVal);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 22))
                                 printf("VT RAT %u\n",
                                         imsaSvcStatus.VtRat.VtRatVal);
                             if(swi_uint256_get_bit (imsaSvcStatus.ParamPresenceMask, 23))
                                 printf("UT RAT %u\n",
                                         imsaSvcStatus.UtRat.UtRatVal);
                         }                             
                     }
                     break;
                case eQMI_IMSA_RAT_HANDOVER_STATUS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_imsa_SLQSImsaRatStatusCallBack_ind_t  ratHandoverStatus;
                         rtn = unpack_imsa_SLQSImsaRatStatusCallBack_ind(req, 
                                                         rlen, 
                                                         &ratHandoverStatus);
                         printf("unpack QMI_IMSA_RAT_HANDOVER_STATUS_IND result :%d\n",rtn);
                         swi_uint256_print_mask (ratHandoverStatus.ParamPresenceMask);
                         if (rtn == eQCWWAN_ERR_NONE) {
                             if(swi_uint256_get_bit (ratHandoverStatus.ParamPresenceMask, 16)){
                                 printf("RAT handover status %u\n",
                                        ratHandoverStatus.RatHandover.RatHandoverStatus);
                                 printf("Source RAT %u\n",
                                        ratHandoverStatus.RatHandover.SourceRAT);
                                 printf("Target RAT %u\n",
                                        ratHandoverStatus.RatHandover.TargetRAT);
                                 printf("Error code Len %u\n",
                                        ratHandoverStatus.RatHandover.ErrorCodeLen);
                                 if (ratHandoverStatus.RatHandover.ErrorCodeLen)
                                     printf("Error code data %s\n",
                                            ratHandoverStatus.RatHandover.ErrorCodeData);
                             }
                         }
                             
                     }
                     break;
                case eQMI_IMSA_PDP_STATUS_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t  pdpStatus;
                         rtn = unpack_imsa_SLQSImsaPdpStatusCallBack_ind(req, 
                                                         rlen, 
                                                         &pdpStatus);
                         printf("unpack QMI_IMSA_PDP_STATUS_IND result :%d\n",rtn);
                         swi_uint256_print_mask (pdpStatus.ParamPresenceMask);
                         if (rtn == eQCWWAN_ERR_NONE) {
                             if(swi_uint256_get_bit (pdpStatus.ParamPresenceMask, 1))
                                 printf("PDP connect status %u\n",
                                         pdpStatus.PdpConnState);
                             if(swi_uint256_get_bit (pdpStatus.ParamPresenceMask, 16))
                                 printf("PDP fail error code %u\n",
                                     pdpStatus.FailErrCode.ImsFailErrCode);
                         }
                     }
                     break;

                default:
                    break;
            }
        }
        else
        {
            qmi = -1;
            stopReadThread = 1;
            break;
        }
    }
    return NULL;
}

int reg_event()
{
    int rtn = -1;
    len = sizeof(req);
    const char *qmi_msg;
    pack_imsa_SLQSRegisterIMSAIndication_t param;
    memset(&param, 0, sizeof(param));
    param.RegStatusConfig = 1;
    param.has_RegStatusConfig = 1;
    param.ServiceStatusConfig = 1;
    param.has_ServiceStatusConfig = 1;
    param.RatHandoverStatusConfig = 1;
    param.has_RatHandoverStatusConfig = 1;
    param.PdpStatusConfig = 1;
    param.has_PdpStatusConfig = 1;

    memset(&ctx, 0, sizeof(ctx));
    ctx.xid = xid;
    xid++;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    fprintf (stderr, "pack_imsa_SLQSRegisterIMSAIndication ret: %d, Len: %d\n", rtn, len);
    rtn = write(qmi, req, len);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn!=len)
    {
        return -1;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMSA, req, len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(len, req);
    }
    return 0;
}

int get_reg_status()
{
    int rtn = -1;
    const char *qmi_msg;
    len = sizeof(req);
    memset(&ctx, 0, sizeof(ctx));
    ctx.xid = xid;
    xid++;
    rtn = pack_imsa_SLQSGetIMSARegStatus(&ctx, req, &len);
    fprintf (stderr, "pack_imsa_SLQSGetIMSARegStatus ret: %d, Len: %d\n", rtn, len);
    rtn = write(qmi, req, len);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn!=len)
    {
        return -1;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMSA, req, len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(len, req);
    }
    return 0;
}

int get_svc_status()
{
    int rtn = -1;
    const char *qmi_msg;
    len = sizeof(req);
    memset(&ctx, 0, sizeof(ctx));
    ctx.xid = xid;
    xid++;
    rtn = pack_imsa_SLQSGetIMSAServiceStatus(&ctx, req, &len);
    fprintf (stderr, "pack_imsa_SLQSGetIMSAServiceStatus ret: %d, Len: %d\n", rtn, len);
    rtn = write(qmi, req, len);
    fprintf (stderr, "Write : %d\n", rtn);
    if(rtn!=len)
    {
        return -1;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMSA, req, len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(len, req);
    }
    return 0;
}

void imsa_loop(){
    pthread_attr_t thread_attr;
    pthread_t thread_id = 0;

    imsa_validate_dummy_unpack();

    qmi = client_fd(eIMSA);
    if(0>qmi)
    {
        fprintf (stderr, "Service(IMSA) Not Supproted %d\n",eIMSA);
        return ;
    }

    // Start imsa read thread
    memset(&thread_attr, 0, sizeof(thread_attr));
    stopReadThread = 0;
    pthread_create(&thread_id, &thread_attr, (void* (*)(void*))readThread, NULL);
    sleep(1);
    if (reg_event()) goto cleanup;
    if (get_reg_status()) goto cleanup;
    if (get_svc_status()) goto cleanup;

cleanup:
    sleep(20);
    stopReadThread = 1;
    printf("\nkilling IMSA read thread...\n");
#ifdef __ARM_EABI__    
    if(qmi>=0)
           close(qmi);
        qmi = -1;
    void *pthread_rtn_value;
    if(thread_id!=0)
        pthread_join(thread_id, &pthread_rtn_value);
    thread_id = 0;
#endif
    if(thread_id!=0)
        pthread_cancel(thread_id);
    thread_id = 0;
    if(qmi>=0)
       close(qmi);
    qmi = -1;
    return;
}

void dump_SLQSGetIMSARegStatus(void *ptr)
{
    unpack_imsa_SLQSGetIMSARegStatus_t *result =
            (unpack_imsa_SLQSGetIMSARegStatus_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("\treg status 0x%x\n", result->ImsRegStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("\terr code 0x%x\n", result->ImsRegErrCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("\tnew reg status 0x%x\n", result->NewImsRegStatus);
}

void dump_SLQSGetIMSAServiceStatus(void *ptr)
{
    unpack_imsa_SLQSGetIMSAServiceStatus_t *result =
            (unpack_imsa_SLQSGetIMSAServiceStatus_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("\tsms status 0x%x\n", result->SmsServiceStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("\tvoip status 0x%x\n", result->VoipServiceStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("\tvt status 0x%x\n", result->VtServiceStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        printf("\tsms rat 0x%x\n", result->SmsServiceRat);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        printf("\tvoip rat 0x%x\n", result->VoipServiceRat);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("\tvt rat 0x%x\n", result->VtServiceRat);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        printf("\tut status 0x%x\n", result->UtServiceStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        printf("\tut rat 0x%x\n", result->UtServiceRat);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
        printf("\tvs status 0x%x\n", result->VsServiceStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
        printf("\tvs rat 0x%x\n", result->VsServiceRat);
}

void dump_SLQSImsaSvcStatusCallBack(void *ptr)
{
    unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t *result =
            (unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("SMS service status %u\n",
               result->SmsService.SmsSvcStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("VOIP service status %u\n",
                result->VoipService.VoipSvcStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("VT service status %u\n",
                result->VtService.VtSvcStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        printf("UT service status %u\n",
                result->UtService.UtSvcStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        printf("SMS RAT %u\n",
               result->SmsRat.SmsRatVal);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("VOIP RAT %u\n",
                result->VoipRat.VoipRatVal);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        printf("VT RAT %u\n",
                result->VtRat.VtRatVal);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        printf("UT RAT %u\n",
                result->UtRat.UtRatVal);
}

void dump_SLQSImsaRegStatusCallBack(void *ptr)
{
    unpack_imsa_SLQSImsaRegStatusCallBack_ind_t *result =
            (unpack_imsa_SLQSImsaRegStatusCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("IMS registration status %d\n",
                result->IMSRegistration.ImsRegistered);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("IMS registration error code %d\n",
                result->IMSRegistrationError.ErrorCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("New IMS registration status %d\n",
                result->NewIMSRegistration.ImsRegStatus);
}

void dump_SLQSImsaRatStatusCallBack(void *ptr)
{
    unpack_imsa_SLQSImsaRatStatusCallBack_ind_t *result =
            (unpack_imsa_SLQSImsaRatStatusCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16)){
        printf("RAT handover status %u\n",
               result->RatHandover.RatHandoverStatus);
        printf("Source RAT %u\n",
               result->RatHandover.SourceRAT);
        printf("Target RAT %u\n",
               result->RatHandover.TargetRAT);
        printf("Error code Len %u\n",
               result->RatHandover.ErrorCodeLen);
        if (result->RatHandover.ErrorCodeLen)
            printf("Error code data %s\n",
                   result->RatHandover.ErrorCodeData);
    }
}

void dump_SLQSImsaPdpStatusCallBack(void *ptr)
{
    unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t *result =
            (unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("PDP connect status %u\n",
                result->PdpConnState);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("PDP fail error code %u\n",
            result->FailErrCode.ImsFailErrCode);
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_imsa_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_IMSA_INDICATION_REGISTER */
    {0x02,0x01,0x00,0x22,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_IMSA_GET_REGISTRATION_STATUS */
   {0x02,0x02,0x00,0x20,0x00,0x17,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x01,0x11,0x02,0x00,0x01,0x01,0x12,0x04,0x00,0x02,0x00,0x00,0x00},

    /* eQMI_IMSA_GET_SERVICE_STATUS */
    {0x02,0x03,0x00,0x21,0x00,0x4D,0x00,
           0x02,0x04,0x00,0x00,0x00,0x00,0x00,
           0x10,0x04,0x00,0x02,0x00,0x00,0x00,
           0x11,0x04,0x00,0x02,0x00,0x00,0x00,
           0x12,0x04,0x00,0x00,0x00,0x00,0x00,
           0x13,0x04,0x00,0x01,0x00,0x00,0x00,
           0x14,0x04,0x00,0x00,0x00,0x00,0x00,
           0x15,0x04,0x00,0x02,0x00,0x00,0x00,
           0x16,0x04,0x00,0x00,0x00,0x00,0x00,
           0x17,0x04,0x00,0x01,0x00,0x00,0x00,
           0x18,0x04,0x00,0x00,0x00,0x00,0x00,
           0x19,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_IMSA_SERVICE_STATUS_IND */
    {0x04,0x04,0x00,0x24,0x00,0x38,0x00,
           0x10,0x04,0x00,0x02,0x00,0x00,0x00,
           0x11,0x04,0x00,0x02,0x00,0x00,0x00,
           0x12,0x04,0x00,0x00,0x00,0x00,0x00,
           0x13,0x04,0x00,0x01,0x00,0x00,0x00,
           0x14,0x04,0x00,0x00,0x00,0x00,0x00,
           0x15,0x04,0x00,0x02,0x00,0x00,0x00,
           0x16,0x04,0x00,0x00,0x00,0x00,0x00,
           0x17,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_IMSA_REGISTRATION_STATUS_IND */
    {0x04,0x05,0x00,0x23,0x00,0x10,0x00,
           0x01,0x01,0x00,0x01,
           0x10,0x02,0x00,0x02,0x03,
           0x11,0x04,0x00,0x02,0x00,0x00,0x00},

    /* eQMI_IMSA_REGISTRATION_STATUS_IND */
    {0x04,0x06,0x00,0x25,0x00,0x2B,0x00,
           0x10,0x28,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
           0x03,0x00,0x00,0x00,0x1B,'I','M','S','A','_','S','T','A','T','U','S',
           '_','R','A','T','_','H','O','_','F','A','I','L','U','R','E','\0'},

    /* eQMI_IMSA_PDP_STATUS_IND */
    {0x04,0x07,0x00,0x26,0x00,0x0B,0x00,
           0x01,0x01,0x00,0x01,
           0x10,0x04,0x00,0x02,0x00,0x00,0x00},
};

       /* eQMI_IMSA_INDICATION_REGISTER */
const unpack_imsa_SLQSRegisterIMSAIndication_t const_unpack_imsa_SLQSRegisterIMSAIndication_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_IMSA_GET_REGISTRATION_STATUS */
const unpack_imsa_SLQSGetIMSARegStatus_t const_unpack_imsa_SLQSGetIMSARegStatus_t = {
        1, 0x0101 ,2 ,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

/* eQMI_IMSA_GET_SERVICE_STATUS */
const unpack_imsa_SLQSGetIMSAServiceStatus_t const_unpack_imsa_SLQSGetIMSAServiceStatus_t = {
        2, 2 ,0 ,1, 0, 2,0,1,0,0,{{SWI_UINT256_BIT_VALUE(SET_11_BITS,2,16,17,18,19,20,21,22,23,24,25)}} };

/* eQMI_IMSA_SERVICE_STATUS_IND */
const unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t const_unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t = {
        {1,2}, {1,2} ,{1,0} ,{1,1}, {1,0}, {1,2},{1,0} ,{1,1}, {{SWI_UINT256_BIT_VALUE(SET_8_BITS,16,17,18,19,20,21,22,23)}} };

/* eQMI_IMSA_REGISTRATION_STATUS_IND */
const unpack_imsa_SLQSImsaRegStatusCallBack_ind_t const_unpack_imsa_SLQSImsaRegStatusCallBack_ind_t = {
        {1,1}, {1,0x302} ,{1,2}, {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)}} };

/* eQMI_IMSA_RAT_HANDOVER_STATUS_IND */
const unpack_imsa_SLQSImsaRatStatusCallBack_ind_t const_unpack_imsa_SLQSImsaRatStatusCallBack_ind_t = {
        {1, 2, 1, 3, 0x1B,"IMSA_STATUS_RAT_HO_FAILURE"},
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

/* eQMI_IMSA_PDP_STATUS_IND */
const unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t const_unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t = {
        1, {1,2}, {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

int imsa_validate_dummy_unpack()
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
    loopCount = sizeof(validate_imsa_resp_msg)/sizeof(validate_imsa_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_imsa_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eIMSA, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("IMSA IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("IMSA RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_IMSA_INDICATION_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSRegisterIMSAIndication,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSRegisterIMSAIndication_t);
                }
                break;
            case eQMI_IMSA_GET_REGISTRATION_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSGetIMSARegStatus,
                    dump_SLQSGetIMSARegStatus,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSGetIMSARegStatus_t);
                }
                break;
            case eQMI_IMSA_GET_SERVICE_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSGetIMSAServiceStatus,
                    dump_SLQSGetIMSAServiceStatus,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSGetIMSAServiceStatus_t);
                }
                break;
            case eQMI_IMSA_SERVICE_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSImsaSvcStatusCallBack_ind,
                    dump_SLQSImsaSvcStatusCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSImsaSvcStatusCallBack_ind_t);
                }
                break;
            case eQMI_IMSA_REGISTRATION_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSImsaRegStatusCallBack_ind,
                    dump_SLQSImsaRegStatusCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSImsaRegStatusCallBack_ind_t);
                }
                break;
            case eQMI_IMSA_RAT_HANDOVER_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSImsaRatStatusCallBack_ind,
                    dump_SLQSImsaRatStatusCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSImsaRatStatusCallBack_ind_t);
                }
                break;
            case eQMI_IMSA_PDP_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_imsa_SLQSImsaPdpStatusCallBack_ind,
                    dump_SLQSImsaPdpStatusCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_imsa_SLQSImsaPdpStatusCallBack_ind_t);
                }
                break;
            }
        }
    }
    return 0;
}

