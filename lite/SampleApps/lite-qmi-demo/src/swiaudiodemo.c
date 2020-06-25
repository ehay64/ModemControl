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
#include "swiaudio.h"

#define STAGESIZECOUNT 20

int swiaudio_validate_dummy_unpack();
int swiaudio = -1;
volatile int enSWIAUDIOThread =0;
pthread_t swiaudio_tid;
pthread_attr_t swiaudio_attr;

uint8_t generator = 0x00;
pack_swiaudio_SLQSGetM2MAudioProfile_t tpack_swiaudio_SLQSGetM2MAudioProfile = {&generator};    //Audio Generator Voice
unpack_swiaudio_SLQSGetM2MAudioProfile_t tunpack_swiaudio_SLQSGetM2MAudioProfile;

/* Valid values */
uint8_t EarMute = 1;
uint8_t MicMute = 1;
uint8_t Genrtr  = 0;
uint8_t Volume  = 3;
uint8_t CwtMute = 1;
pack_swiaudio_SLQSSetM2MAudioProfile_t tpack_swiaudio_SLQSSetM2MAudioProfile = {
        3, &EarMute, &MicMute, &Genrtr, &Volume, &CwtMute};
unpack_swiaudio_SLQSSetM2MAudioProfile_t tunpack_swiaudio_SLQSSetM2MAudioProfile = {
        0, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSGetM2MAudioVolume_t tpack_swiaudio_SLQSGetM2MAudioVolume = {3, 0}; //Profile -3 Generator -0
unpack_swiaudio_SLQSGetM2MAudioVolume_t tunpack_swiaudio_SLQSGetM2MAudioVolume = {0xFF, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSSetM2MAudioVolume_t tpack_swiaudio_SLQSSetM2MAudioVolume = {3, 0, 2}; //Profile-3 Generator-0 Level-2
unpack_swiaudio_SLQSSetM2MAudioVolume_t tunpack_swiaudio_SLQSSetM2MAudioVolume = {0, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSSetM2MAudioAVCFG_t tpack_swiaudio_SLQSSetM2MAudioAVCFG = {
        3, 3, 3, NULL}; //Profile-3 Device-3(USB) PIFACEId-3(USB)
unpack_swiaudio_SLQSSetM2MAudioAVCFG_t tunpack_swiaudio_SLQSSetM2MAudioAVCFG = {
        0, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSSetM2MAudioLPBK_t tpack_swiaudio_SLQSSetM2MAudioLPBK = { 0 }; //Enable = 0
unpack_swiaudio_SLQSSetM2MAudioLPBK_t tunpack_swiaudio_SLQSSetM2MAudioLPBK = {
       0, SWI_UINT256_INT_VALUE };

unpack_swiaudio_SLQSSetM2MAudioNVDef_t tunpack_swiaudio_SLQSSetM2MAudioNVDef = {
       0, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSGetM2MSpkrGain_t tpack_swiaudio_SLQSGetM2MSpkrGain = { 3 };//Profile-3
unpack_swiaudio_SLQSGetM2MSpkrGain_t tunpack_swiaudio_SLQSGetM2MSpkrGain;

pack_swiaudio_SLQSSetM2MSpkrGain_t tpack_swiaudio_SLQSSetM2MSpkrGain = { 3 ,0x02}; //Profile-3 Value-0x02
unpack_swiaudio_SLQSSetM2MSpkrGain_t tunpack_swiaudio_SLQSSetM2MSpkrGain = {0, SWI_UINT256_INT_VALUE };

pack_swiaudio_SLQSGetM2MAVMute_t tpack_swiaudio_SLQSGetM2MAVMute = {3}; //Profile-3
unpack_swiaudio_SLQSGetM2MAVMute_t tunpack_swiaudio_SLQSGetM2MAVMute;

pack_swiaudio_SLQSSetM2MAVMute_t tpack_swiaudio_SLQSSetM2MAVMute = {3, 1, 0, &CwtMute}; //Profile-3 EarMute-1 MicMute-0
unpack_swiaudio_SLQSSetM2MAVMute_t tunpack_swiaudio_SLQSSetM2MAVMute = {0, SWI_UINT256_INT_VALUE };

void dump_SLQSGetM2MAudioProfile (void *ptr)
{
    unpack_swiaudio_SLQSGetM2MAudioProfile_t *result =
            (unpack_swiaudio_SLQSGetM2MAudioProfile_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 7))
        printf( "Audio Profile successfully retrieved\n" );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Profile   : %x\n", result->Profile );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf( "Ear Mute  : %x\n", result->EarMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
        printf( "Mic Mute  : %x\n", result->MicMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 4))
        printf( "Generator : %x\n", result->Generator );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 5))
        printf( "Volume    : %x\n", result->Volume );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 6))
        printf( "Cwt Mute  : %x\n", result->CwtMute );
}

void dump_SLQSSetM2MAudioProfile (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAudioProfile_t *result =
            (unpack_swiaudio_SLQSSetM2MAudioProfile_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetM2MAudioVolume (void *ptr)
{
    unpack_swiaudio_SLQSGetM2MAudioVolume_t *result =
            (unpack_swiaudio_SLQSGetM2MAudioVolume_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf( "Audio Volume successfully retrieved\n" );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Level : 0x%x\n", result->Level );
}

void dump_SLQSSetM2MAudioVolume (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAudioVolume_t *result =
            (unpack_swiaudio_SLQSSetM2MAudioVolume_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSetM2MAudioAVCFG (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAudioAVCFG_t *result =
            (unpack_swiaudio_SLQSSetM2MAudioAVCFG_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSetM2MAudioLPBK (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAudioLPBK_t *result =
            (unpack_swiaudio_SLQSSetM2MAudioLPBK_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSetM2MAudioNVDef (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAudioNVDef_t *result =
            (unpack_swiaudio_SLQSSetM2MAudioNVDef_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetM2MSpkrGain (void *ptr)
{
    unpack_swiaudio_SLQSGetM2MSpkrGain_t *result =
            (unpack_swiaudio_SLQSGetM2MSpkrGain_t*) ptr;    
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf( "Speaker Gain Value Successfully Retrieved\n" );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Value   : %x\n", result->Value );
}

void dump_SLQSSetM2MSpkrGain (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MSpkrGain_t *result =
            (unpack_swiaudio_SLQSSetM2MSpkrGain_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetM2MAVMute (void *ptr)
{
    unpack_swiaudio_SLQSGetM2MAVMute_t *result =
            (unpack_swiaudio_SLQSGetM2MAVMute_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf( "AV Mute Value successfully Retrieved\n" );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "EarMute   : %x\n", result->EarMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf( "MicMute   : %x\n", result->MicMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 3))
        printf( "CwtMute   : %x\n", result->CwtMute );
}

void dump_SLQSSetM2MAVMute (void *ptr)
{
    unpack_swiaudio_SLQSSetM2MAVMute_t *result =
            (unpack_swiaudio_SLQSSetM2MAVMute_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

testitem_t swiaudiototest[] = {
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioProfile, "pack_swiaudio_SLQSGetM2MAudioProfile",
        &tpack_swiaudio_SLQSGetM2MAudioProfile,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioProfile, "unpack_swiaudio_SLQSGetM2MAudioProfile",
        &tunpack_swiaudio_SLQSGetM2MAudioProfile, dump_SLQSGetM2MAudioProfile
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAudioProfile, "pack_swiaudio_SLQSSetM2MAudioProfile",
        &tpack_swiaudio_SLQSSetM2MAudioProfile,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAudioProfile, "unpack_swiaudio_SLQSSetM2MAudioProfile",
        &tunpack_swiaudio_SLQSSetM2MAudioProfile, dump_SLQSSetM2MAudioProfile
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioProfile, "pack_swiaudio_SLQSGetM2MAudioProfile",
        &tpack_swiaudio_SLQSGetM2MAudioProfile,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioProfile, "unpack_swiaudio_SLQSGetM2MAudioProfile",
        &tunpack_swiaudio_SLQSGetM2MAudioProfile, dump_SLQSGetM2MAudioProfile
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioVolume, "pack_swiaudio_SLQSGetM2MAudioVolume",
        &tpack_swiaudio_SLQSGetM2MAudioVolume,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioVolume, "unpack_swiaudio_SLQSGetM2MAudioVolume",
        &tunpack_swiaudio_SLQSGetM2MAudioVolume, dump_SLQSGetM2MAudioVolume
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAudioVolume, "pack_swiaudio_SLQSSetM2MAudioVolume",
        &tpack_swiaudio_SLQSSetM2MAudioVolume,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAudioVolume, "unpack_swiaudio_SLQSSetM2MAudioVolume",
        &tunpack_swiaudio_SLQSSetM2MAudioVolume, dump_SLQSSetM2MAudioVolume
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioVolume, "pack_swiaudio_SLQSGetM2MAudioVolume",
        &tpack_swiaudio_SLQSGetM2MAudioVolume,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioVolume, "unpack_swiaudio_SLQSGetM2MAudioVolume",
        &tunpack_swiaudio_SLQSGetM2MAudioVolume, dump_SLQSGetM2MAudioVolume
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAudioAVCFG, "pack_swiaudio_SLQSSetM2MAudioAVCFG",
        &tpack_swiaudio_SLQSSetM2MAudioAVCFG,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAudioAVCFG, "unpack_swiaudio_SLQSSetM2MAudioAVCFG",
        &tunpack_swiaudio_SLQSSetM2MAudioAVCFG, dump_SLQSSetM2MAudioAVCFG
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAudioLPBK, "pack_swiaudio_SLQSSetM2MAudioLPBK",
        &tpack_swiaudio_SLQSSetM2MAudioLPBK,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAudioLPBK, "unpack_swiaudio_SLQSSetM2MAudioLPBK",
        &tunpack_swiaudio_SLQSSetM2MAudioLPBK, dump_SLQSSetM2MAudioLPBK
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAudioNVDef, "pack_swiaudio_SLQSSetM2MAudioNVDef",
        NULL,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAudioNVDef, "unpack_swiaudio_SLQSSetM2MAudioNVDef",
        &tunpack_swiaudio_SLQSSetM2MAudioNVDef, dump_SLQSSetM2MAudioNVDef
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MSpkrGain, "pack_swiaudio_SLQSGetM2MSpkrGain",
        &tpack_swiaudio_SLQSGetM2MSpkrGain,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MSpkrGain, "unpack_swiaudio_SLQSGetM2MSpkrGain",
        &tunpack_swiaudio_SLQSGetM2MSpkrGain, dump_SLQSGetM2MSpkrGain
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MSpkrGain, "pack_swiaudio_SLQSSetM2MSpkrGain",
        &tpack_swiaudio_SLQSSetM2MSpkrGain,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MSpkrGain, "unpack_swiaudio_SLQSSetM2MSpkrGain",
        &tunpack_swiaudio_SLQSSetM2MSpkrGain, dump_SLQSSetM2MSpkrGain
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MSpkrGain, "pack_swiaudio_SLQSGetM2MSpkrGain",
        &tpack_swiaudio_SLQSGetM2MSpkrGain,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MSpkrGain, "unpack_swiaudio_SLQSGetM2MSpkrGain",
        &tunpack_swiaudio_SLQSGetM2MSpkrGain, dump_SLQSGetM2MSpkrGain
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAVMute, "pack_swiaudio_SLQSGetM2MAVMute",
        &tpack_swiaudio_SLQSGetM2MAVMute,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAVMute, "unpack_swiaudio_SLQSGetM2MAVMute",
        &tunpack_swiaudio_SLQSGetM2MAVMute, dump_SLQSGetM2MAVMute
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSSetM2MAVMute, "pack_swiaudio_SLQSSetM2MAVMute",
        &tpack_swiaudio_SLQSSetM2MAVMute,
        (unpack_func_item) &unpack_swiaudio_SLQSSetM2MAVMute, "unpack_swiaudio_SLQSSetM2MAVMute",
        &tunpack_swiaudio_SLQSSetM2MAVMute, dump_SLQSSetM2MAVMute
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAVMute, "pack_swiaudio_SLQSGetM2MAVMute",
        &tpack_swiaudio_SLQSGetM2MAVMute,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAVMute, "unpack_swiaudio_SLQSGetM2MAVMute",
        &tunpack_swiaudio_SLQSGetM2MAVMute, dump_SLQSGetM2MAVMute
    },
};

unsigned int swiaudioarraylen = (unsigned int)((sizeof(swiaudiototest))/(sizeof(swiaudiototest[0])));

testitem_t swiaudiototest_invalidunpack[] = {
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioProfile, "pack_swiaudio_SLQSGetM2MAudioProfile",
        &tpack_swiaudio_SLQSGetM2MAudioProfile,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioProfile, "unpack_swiaudio_SLQSGetM2MAudioProfile",
        NULL, dump_SLQSGetM2MAudioProfile
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAudioVolume, "pack_swiaudio_SLQSGetM2MAudioVolume",
        &tpack_swiaudio_SLQSGetM2MAudioVolume,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAudioVolume, "unpack_swiaudio_SLQSGetM2MAudioVolume",
        NULL, dump_SLQSGetM2MAudioVolume
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MSpkrGain, "pack_swiaudio_SLQSGetM2MSpkrGain",
        &tpack_swiaudio_SLQSGetM2MSpkrGain,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MSpkrGain, "unpack_swiaudio_SLQSGetM2MSpkrGain",
        NULL, dump_SLQSGetM2MSpkrGain
    },
    {
        (pack_func_item) &pack_swiaudio_SLQSGetM2MAVMute, "pack_swiaudio_SLQSGetM2MAVMute",
        &tpack_swiaudio_SLQSGetM2MAVMute,
        (unpack_func_item) &unpack_swiaudio_SLQSGetM2MAVMute, "unpack_swiaudio_SLQSGetM2MAVMute",
        NULL, dump_SLQSGetM2MAVMute
    },
};

void *swiaudio_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enSWIAUDIOThread)
    {
        if(swiaudio<1)
        {
            usleep(1000);
            printf("%s fd error\n", __func__);
            continue;
        }
        //TODO select multiple file and read them
        rtn = read(swiaudio, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eSWIAUDIO, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("SWIAUDIO IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIAUDIO RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);

            switch(rsp_ctx.msgid)
            {
                default:
                    printf("Default msgid 0x%x\n", rsp_ctx.msgid);
                    break;
            }
        }
    }
    return NULL;
}

void swiaudio_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======SWIAUDIO pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    
    for(i=0; i<sizeof(swiaudiototest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(swiaudiototest_invalidunpack[i].dump!=NULL)
        {
            swiaudiototest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(swiaudiototest)/sizeof(testitem_t); i++)
    {
        if(swiaudiototest[i].dump!=NULL)
        {
            swiaudiototest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(swiaudiototest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swiaudiototest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,swiaudiototest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(swiaudio<0)
            swiaudio = client_fd(eSWIAUDIO);
        if(swiaudio<0)
        {
            fprintf(stderr,"SWIAUDIO Service Not Supported!\n");
            return ;
        }
        rtn = write(swiaudio, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(swiaudio>=0)
                close(swiaudio);
            swiaudio=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIAUDIO, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(swiaudio, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eSWIAUDIO, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(swiaudiototest_invalidunpack[i].unpack)(rsp, rspLen, 
                                       swiaudiototest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           swiaudiototest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    swiaudiototest_invalidunpack[i].dump(swiaudiototest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(swiaudio>=0)
                close(swiaudio);
            swiaudio=-1;
        }
        sleep(1);
        xid++;
    }
    if(swiaudio>=0)
        close(swiaudio);
    swiaudio=-1;
}

void swiaudio_test_pack_unpack_loop()
{
    unsigned i;

    printf("======SWIAUDIO dummy unpack test===========\n");
    swiaudio_validate_dummy_unpack();
    printf("======SWIAUDIO pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE    
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "SWIAUDIO Pack/UnPack API Name", 
                      "Status",
                      "Unpack Payload Parsing");
#endif

    unsigned xid =1;
    for(i=0; i<sizeof(swiaudiototest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(swiaudiototest[i].pack)(&req_ctx, req, &reqLen,swiaudiototest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", swiaudiototest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(swiaudio < 0)
            swiaudio = client_fd(eSWIAUDIO);
        if(swiaudio < 0)
        {
            fprintf(stderr,"SWIAUDIO Service Not Supported!\n");
            return ;
        }
        rtn = write(swiaudio, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eSWIAUDIO, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(swiaudio, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eSWIAUDIO, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("SWIAUDIO RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(swiaudiototest[i].unpack)(rsp, rspLen, swiaudiototest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,%s,", swiaudiototest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "");
                    #endif

                        swiaudiototest[i].dump(swiaudiototest[i].unpack_ptr);
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
                    printf("SWIAUDIO IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(swiaudio >= 0)
        close(swiaudio);
    swiaudio = -1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
 uint8_t validate_swiaudio_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_SWIAUDIO_GET_PROFILE */
    {0x02,0x01,0x00,0x01,0x00,0x1F,0x00,0x07,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,
     0x02,0x02,0x01,0x00,0x01,0x03,0x01,0x00,0x01,0x04,0x01,0x00,0x00,0x05,0x01,0x00,0x03,
     0x06,0x01,0x00,0x01},

    /* eQMI_SWIAUDIO_SET_PROFILE */
    {0x02,0x02,0x00,0x02,0x00,0x0E,0x00,0x01,0x04,0x00,0x00,0x00,0x00,0x00,0x02,0x04,
     0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_GET_VOLUME */
    {0x02,0x03,0x00,0x03,0x00,0x0B,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
     0x00,0x03 },

    /* eQMI_SWIAUDIO_SET_VOLUME */
    {0x02,0x04,0x00,0x04,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_SET_LPBK */
    {0x02,0x05,0x00,0x09,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_SET_NV_DEF */
    {0x02,0x06,0x00,0x0C,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_GET_SPKRGAIN */
    {0x02,0x03,0x00,0x12,0x00,0x0C,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
     0x00,0x03,0x02 },

    /* eQMI_SWIAUDIO_SET_SPKRGAIN */
    {0x02,0x08,0x00,0x13,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_SET_AVCFG */
    {0x02,0x09,0x00,0x1B,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_SWIAUDIO_GET_AVMUTE */
    {0x02,0x03,0x00,0x1C,0x00,0x13,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
     0x00,0x01,0x02,0x01,0x00,0x01,0x03,0x01,0x000,0x03 },

    /* eQMI_SWIAUDIO_SET_AVMUTE */
    {0x02,0x0B,0x00,0x1D,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00 },
   };

/* eQMI_SWIAUDIO_GET_PROFILE */
const unpack_swiaudio_SLQSGetM2MAudioProfile_t const_unpack_swiaudio_SLQSGetM2MAudioProfile_t = {
        2,1,1,0,3,1,{{SWI_UINT256_BIT_VALUE(SET_7_BITS,1,2,3,4,5,6,7)}} };

/* eQMI_SWIAUDIO_SET_PROFILE */
const unpack_swiaudio_SLQSSetM2MAudioProfile_t const_unpack_swiaudio_SLQSSetM2MAudioProfile_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_SWIAUDIO_GET_VOLUME */
const unpack_swiaudio_SLQSGetM2MAudioVolume_t const_unpack_swiaudio_SLQSGetM2MAudioVolume_t = {
        3,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* QMI_SWIAUDIO_SET_VOLUME */
const unpack_swiaudio_SLQSSetM2MAudioVolume_t const_unpack_swiaudio_SLQSSetM2MAudioVolume_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIAUDIO_SET_LPBK */
const unpack_swiaudio_SLQSSetM2MAudioLPBK_t const_unpack_swiaudio_SLQSSetM2MAudioLPBK_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIAUDIO_SET_NV_DEF */
const unpack_swiaudio_SLQSSetM2MAudioNVDef_t const_unpack_swiaudio_SLQSSetM2MAudioNVDef_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIAUDIO_GET_SPKRGAIN */
const unpack_swiaudio_SLQSGetM2MSpkrGain_t const_unpack_swiaudio_SLQSGetM2MSpkrGain_t = {
        0x0203,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_SWIAUDIO_SET_SPKRGAIN */
const  unpack_swiaudio_SLQSSetM2MSpkrGain_t const_unpack_swiaudio_SLQSSetM2MSpkrGain_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIAUDIO_SET_AVCFG */
const  unpack_swiaudio_SLQSSetM2MAudioAVCFG_t const_unpack_swiaudio_SLQSSetM2MAudioAVCFG_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_SWIAUDIO_GET_AVMUTE */
const unpack_swiaudio_SLQSGetM2MAVMute_t const_unpack_swiaudio_SLQSGetM2MAVMute_t = {
        1,1,3,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,3,4)}} };

/* eQMI_SWIAUDIO_SET_AVMUTE */
const  unpack_swiaudio_SLQSSetM2MAVMute_t const_unpack_swiaudio_SLQSSetM2MAVMute_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

int swiaudio_validate_dummy_unpack()
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
    loopCount = sizeof(validate_swiaudio_resp_msg)/sizeof(validate_swiaudio_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_swiaudio_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_SWIAUDIO, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("SWIAUDIO IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("SWIAUDIO RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {

            case eQMI_SWIAUDIO_GET_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSGetM2MAudioProfile,
                    dump_SLQSGetM2MAudioProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSGetM2MAudioProfile_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAudioProfile,
                    dump_SLQSSetM2MAudioProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAudioProfile_t);
                }
                break;
            case eQMI_SWIAUDIO_GET_VOLUME:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSGetM2MAudioVolume,
                    dump_SLQSGetM2MAudioVolume,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSGetM2MAudioVolume_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_VOLUME:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAudioVolume,
                    dump_SLQSSetM2MAudioVolume,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAudioVolume_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_LPBK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAudioLPBK,
                    dump_SLQSSetM2MAudioLPBK,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAudioLPBK_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_NV_DEF:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAudioNVDef,
                    dump_SLQSSetM2MAudioNVDef,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAudioNVDef_t);
                }
                break;
            case eQMI_SWIAUDIO_GET_SPKRGAIN:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSGetM2MSpkrGain,
                    dump_SLQSGetM2MSpkrGain,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSGetM2MSpkrGain_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_SPKRGAIN:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MSpkrGain,
                    dump_SLQSSetM2MSpkrGain,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MSpkrGain_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_AVCFG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAudioAVCFG,
                    dump_SLQSSetM2MAudioAVCFG,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAudioAVCFG_t);
                }
                break;
            case eQMI_SWIAUDIO_GET_AVMUTE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSGetM2MAVMute,
                    dump_SLQSGetM2MAVMute,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSGetM2MAVMute_t);
                }
                break;
            case eQMI_SWIAUDIO_SET_AVMUTE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_swiaudio_SLQSSetM2MAVMute,
                    dump_SLQSSetM2MAVMute,
                    msg.buf,
                    rlen,
                    &const_unpack_swiaudio_SLQSSetM2MAVMute_t);
                }
                break;
            }
        }
    }
    return 0;
}

