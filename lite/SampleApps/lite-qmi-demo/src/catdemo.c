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
#include "cat.h"

int cat_validate_dummy_unpack();

int cat=-1;
volatile int enCATThread =0;
pthread_t cat_tid = 0;
pthread_attr_t cat_attr;

uint32_t       eventMask;

pack_cat_SetCATEventCallback_t CATEventCallbackReq = {0};
unpack_cat_SetCATEventCallback_t CATEventCallbackResp = {0,0,{{0}} };

uint8_t buffer[] = { 0xD3, 0x03, 0x03, 0x00, 0x15 };
pack_cat_CATSendEnvelopeCommand_t tpack_cat_CATSendEnvelopeCommand = {0x01, sizeof(buffer), buffer};
unpack_cat_CATSendEnvelopeCommand_t tunpack_cat_CATSendEnvelopeCommand = {0,{{0}} };

pack_cat_CATSendTerminalResponse_t tpack_cat_CATSendTerminalResponse = {0x02, 1, (uint8_t*)"Hello World"};
unpack_cat_CATSendTerminalResponse_t tunpack_cat_CATSendTerminalResponse = {0, {{0}} };

void dump_CAT_EventCallback(void * ptr)
{
    unpack_cat_SetCATEventCallback_t *result =
            (unpack_cat_SetCATEventCallback_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
     printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf( "Error Mask  : 0x%08x\n", result->errorMask );
}

void dump_CAT_CATSendEnvelopeCommand(void* ptr)
{
    unpack_cat_CATSendEnvelopeCommand_t *result =
            (unpack_cat_CATSendEnvelopeCommand_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_CAT_CATSendTerminalResponse(void* ptr)
{
    unpack_cat_CATSendTerminalResponse_t *result =
            (unpack_cat_CATSendTerminalResponse_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

testitem_t totestcat[] = {
    {
        (pack_func_item) &pack_cat_SetCATEventCallback, "pack_cat_SetCATEventCallback",
        &CATEventCallbackReq,
        (unpack_func_item) &unpack_cat_SetCATEventCallback, "unpack_cat_SetCATEventCallback",
        &CATEventCallbackResp, dump_CAT_EventCallback
    },
    {
        (pack_func_item) &pack_cat_CATSendEnvelopeCommand, "pack_cat_CATSendEnvelopeCommand",
        &tpack_cat_CATSendEnvelopeCommand,
        (unpack_func_item) &unpack_cat_CATSendEnvelopeCommand, "unpack_cat_CATSendEnvelopeCommand",
        &tunpack_cat_CATSendEnvelopeCommand, dump_CAT_CATSendEnvelopeCommand
    },
    {
        (pack_func_item) &pack_cat_CATSendTerminalResponse, "pack_cat_CATSendTerminalResponse",
        &tpack_cat_CATSendTerminalResponse,
        (unpack_func_item) &unpack_cat_CATSendTerminalResponse, "unpack_cat_CATSendTerminalResponse",
        &tunpack_cat_CATSendTerminalResponse, dump_CAT_CATSendTerminalResponse
    },
};

unsigned int catarraylen = (unsigned int)((sizeof(totestcat))/(sizeof(totestcat[0])));

testitem_t cattotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_cat_SetCATEventCallback, "pack_cat_SetCATEventCallback",
        &CATEventCallbackReq,
        (unpack_func_item) &unpack_cat_SetCATEventCallback, "unpack_cat_SetCATEventCallback",
        NULL, dump_CAT_EventCallback
    },
};

void displayCatEventCallback(unpack_cat_SetCatEventCallback_ind_t *pInfo)
{
    uint32_t index = 0;
    printf ( "Event ID: %x\r\n\n",pInfo->CCETlv->EventID );
    printf ( "Event Length is: %x\r\n\n", pInfo->CCETlv->EventLength );
    if( (swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 18))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 19))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 20))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 24))           ||
        (swi_uint256_get_bit (pInfo->ParamPresenceMask, 23)) )
    {
        if( (pInfo->CCETlv->EventID == 16)       ||
            (pInfo->CCETlv->EventID == 17)       ||
            (pInfo->CCETlv->EventID == 18)       ||
            (pInfo->CCETlv->EventID == 19)       ||
            (pInfo->CCETlv->EventID == 20)       ||
            (pInfo->CCETlv->EventID == 24)       ||
            (pInfo->CCETlv->EventID == 23) )
        {
            struct cat_EventIDDataTlv *pData =
                    (struct cat_EventIDDataTlv *)&pInfo->CCETlv->CatEvent.CatEvIDData;
            printf ( "Reference ID is: %x\r\n\n", pData->ReferenceID );
            printf ( "Data Length is: %d\r\n\n", pData->DataLength );
            printf ( "Event Data is: \r\n" );
            for ( index = 0; index < pData->DataLength; index++ )
                printf ( "%c", pData->Data[index] );

            printf ("\r\nEvent Data is (in HEX): \r\n" );
            for ( index = 0; index < pData->DataLength; index++ )
                printf ("%x ", pData->Data[index] );

            printf ( "\r\n" );
        }
    }
    else if((swi_uint256_get_bit (pInfo->ParamPresenceMask, 21) )  &&
       (pInfo->CCETlv->EventID == 21) )
    {
        struct cat_AlPhaIdentifierTlv *pData =
                (struct cat_AlPhaIdentifierTlv *)&pInfo->CCETlv->CatEvent.CatAlphaIdtfr;

        printf ( "Reference ID is: %x\r\n\n", pData->ReferenceID );
        printf ( "Alpha ID Length is: %d\r\n\n", pData->AlphaIDLength );
        printf ( "Event Data is: \r\n" );
        for ( index = 0; index < pData->AlphaIDLength; index++ )
            printf ( "%c", pData->AlphaID[index] );

        printf ("\r\nEvent Data is (in HEX): \r\n" );
        for ( index = 0; index < pData->AlphaIDLength; index++ )
            printf ("%x ", pData->AlphaID[index] );
        printf ( "\r\n" );
    }

    else if((swi_uint256_get_bit (pInfo->ParamPresenceMask, 22) )  &&
       (pInfo->CCETlv->EventID == 22) )
    {
        struct cat_EventListTlv *pData =
                (struct cat_EventListTlv *)&pInfo->CCETlv->CatEvent.CatEventLst;
        printf ( "Setup Event List is: %d\r\n\n", pData->SetupEventList );
    }

    else if((swi_uint256_get_bit (pInfo->ParamPresenceMask, 25) )  &&
       (pInfo->CCETlv->EventID == 25) )
    {
        struct cat_RefreshTlv *pData =
                (struct cat_RefreshTlv *)&pInfo->CCETlv->CatEvent.CatRefresh;
        printf ( "Refresh Mode  : %d\r\n\n", pData->RefreshMode );
        printf ( "Refresh Stage : %d\r\n\n", pData->RefreshStage );
    }

    else if((swi_uint256_get_bit (pInfo->ParamPresenceMask, 26) )  &&
       (pInfo->CCETlv->EventID == 26) )
    {
        struct cat_EndProactiveSessionTlv *pData =
                (struct cat_EndProactiveSessionTlv *)&pInfo->CCETlv->CatEvent.CatEndPS;
        printf ( "End Proactive Session : %d\r\n\n", pData->EndProactiveSession );
    }
}

void *cat_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enCATThread)
    {
        if(cat<1)
        {
            usleep(1000);
            printf("%s fd error\n", __func__);
            continue;
        }
        //TODO select multiple file and read them
        rtn = read(cat, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eCAT, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("CAT IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("CAT RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);

            switch(rsp_ctx.msgid)
            {
                case eQMI_CAT_EVENT_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_cat_SetCatEventCallback_ind_t catEventCallbackInd;
                        rtn = unpack_cat_SetCatEventCallback_ind(msg.buf, QMI_MSG_MAX, &catEventCallbackInd );
                        swi_uint256_print_mask (catEventCallbackInd.ParamPresenceMask);
                        printf("QMI CAT EVENT IND result :%d\n",rtn);
                        displayCatEventCallback(&catEventCallbackInd);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_cat_SetCATEventCallback_t setEventResp;
                        memset(&setEventResp,0,sizeof(unpack_cat_SetCATEventCallback_t));
                        rtn = unpack_cat_SetCATEventCallback( msg.buf, QMI_MSG_MAX ,&setEventResp);
                        printf("QMI_CAT_SET_EVENT_REPORT RSP result :%d\n",rtn);
                    }
                    break;
                default:
                    printf("Default msgid 0x%x\n", rsp_ctx.msgid);
                    break;
            }
        }
    }
    return NULL;
}

void cat_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======CAT pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    
    for(i=0; i<sizeof(cattotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(cattotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,cattotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(cat<0)
            cat = client_fd(eCAT);
        if(cat<0)
        {
            fprintf(stderr,"CAT Service Not Supported!\n");
            return ;
        }
        rtn = write(cat, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(cat>=0)
                close(cat);
            cat=-1;
            continue ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eCAT, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(cat, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eCAT, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(cattotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                cattotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           cattotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    cattotest_invalidunpack[i].dump(cattotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(cat>=0)
                close(cat);
            cat=-1;
        }
        sleep(1);
        xid++;
    }
    if(cat>=0)
        close(cat);
    cat=-1;
}

void cat_test_pack_unpack_loop()
{
    unsigned i;
    unsigned xid =1;

    printf("======CAT dummy unpack test===========\n");
    cat_validate_dummy_unpack();
    printf("======CAT pack/unpack test===========\n");
    for(i=0; i<sizeof(totestcat)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(totestcat[i].pack)(&req_ctx, req, &reqLen,totestcat[i].pack_ptr);
        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        if(cat<0)
            cat = client_fd(eCAT);
        if(cat<0)
        {
            fprintf(stderr,"CAT Service Not Supported\n");
            return ;
        }
        rtn = write(cat, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eCAT, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = rspLen = read(cat, rsp, QMI_MSG_MAX);

        if(rtn>0)
        {
            qmi_msg = helper_get_resp_ctx(eCAT, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                run_unpack_item(totestcat[i].unpack)(rsp, rspLen, totestcat[i].unpack_ptr);
                totestcat[i].dump(totestcat[i].unpack_ptr);
            }
        }
        xid++;
        if(cat>=0)
            close(cat);
        cat = -1;
    }
}


void cat_test_ind()
{
    pack_qmi_t req_ctx;
    uint16_t qmi_req_len;
    uint8_t qmi_req[QMI_MSG_MAX];
    int rtn;
    const char *qmi_msg;
    if(cat<0)
        cat = client_fd(eCAT);
    if(cat<0)
    {
        fprintf(stderr,"CAT Service Not Supported\n");
        return ;
    }
    pack_cat_SetCATEventCallback_t SetCATEventReq;
    memset(&SetCATEventReq,0,sizeof(pack_cat_SetCATEventCallback_t));
    req_ctx.xid = 0x100;
    memset(&cat_attr, 0, sizeof(cat_attr));
    SetCATEventReq.eventMask =  0xFFFF;
    rtn = pack_cat_SetCATEventCallback(&req_ctx, qmi_req, &qmi_req_len, (void*)&SetCATEventReq);
    if(rtn!=0)
    {
        printf("Set Event Callback Fail");
        return ;
    }
    rtn = write(cat, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("CAT FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eCAT, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    enCATThread = 1;
    pthread_create(&cat_tid, &cat_attr, cat_read_thread, NULL);

    sleep(1);
    sleep(25);
    enCATThread = 0;
}

void cat_test_ind_exit()
{
    printf("\nkilling CAT read thread...\n");
    enCATThread = 0;
#ifdef __ARM_EABI__
    if(cat>=0)
        close(cat);
    cat=-1;
    void *pthread_rtn_value;
    if(cat_tid!=0)
    pthread_join(cat_tid, &pthread_rtn_value);
#endif
    if(cat_tid!=0)
    pthread_cancel(cat_tid);
    cat_tid = 0;
    if(cat>=0)
        close(cat);
    cat=-1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_cat_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_CAT_EVENT_IND <msg1>*/
    {0x04,0x01,0x00,0x01,0x00,0x11,0x00,0x10,0x0E,0x00,0x01,0x00,0x00,0x00,0x08,0x00,
     0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48},

    /* eQMI_CAT_EVENT_IND <msg2>*/
    {0x04,0x02,0x00,0x01,0x00,0x06,0x00,0x19,0x03,0x00,0x01,0x00,0x02 },

    /* eQMI_CAT_SET_EVENT */
    {0x02,0x03,0x00,0x01,0x00,0x0E,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
     0x00,0x08,0x00,0x00,0x00 },

    /* eQMI_CAT_SEND_ENVELOPE */
    {0x02,0x04,0x00,0x22,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_CAT_SEND_TERMINAL */
    {0x02,0x05,0x00,0x21,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },
   };

/* eQMI_CAT_SET_EVENT */
const unpack_cat_SetCATEventCallback_t const_unpack_cat_SetCATEventCallback_t = {
        8,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2, 16)}} };

/* eQMI_CAT_EVENT_IND */
const unpack_cat_SetCatEventCallback_ind_t const_unpack_cat_SetCatEventCallback_ind_t[] = {
        {
        1, 
        {{1,0x10, 0x0E, {{0x01,0x08,{'A','B','C','D','E','F','G', 'H', '\0'}}}}},
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} },

        {
        1, 
        {{1,0x19, 0x03, {{0x020001,0x00,{0}}}}},
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,25)}} },
        };

/* eQMI_CAT_SEND_ENVELOPE */
const unpack_cat_CATSendEnvelopeCommand_t const_unpack_cat_CATSendEnvelopeCommand_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_CAT_SEND_TERMINAL */
const unpack_cat_CATSendTerminalResponse_t const_unpack_cat_CATSendTerminalResponse_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };


int cat_validate_dummy_unpack()
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
    loopCount = sizeof(validate_cat_resp_msg)/sizeof(validate_cat_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_cat_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_CAT, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("CAT IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("CAT RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {

            case eQMI_CAT_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_cat_SetCATEventCallback,
                    dump_CAT_EventCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_cat_SetCATEventCallback_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_cat_SetCatEventCallback_ind,
                    displayCatEventCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_cat_SetCatEventCallback_ind_t[rsp_ctx.xid-1]);
                }
                break;
            case eQMI_CAT_SEND_ENVELOPE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_cat_CATSendEnvelopeCommand,
                    dump_CAT_CATSendEnvelopeCommand,
                    msg.buf,
                    rlen,
                    &const_unpack_cat_CATSendEnvelopeCommand_t);
                }
                break;
            case eQMI_CAT_SEND_TERMINAL:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_cat_CATSendTerminalResponse,
                    dump_CAT_CATSendTerminalResponse,
                    msg.buf,
                    rlen,
                    &const_unpack_cat_CATSendTerminalResponse_t);
                }
                break;
            }
        }
    }
    return 0;
}


