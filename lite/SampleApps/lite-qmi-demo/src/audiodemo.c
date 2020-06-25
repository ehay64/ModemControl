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
#include "audio.h"

#define IFPRINTF(s,p)   if( NULL != p ){ printf(s,*p); }
#define STAGESIZECOUNT 20

int audio = -1;
volatile int enAUDIOThread =0;
pthread_t audio_tid;
pthread_attr_t audio_attr;


pack_audio_SLQSGetAudioProfile_t tpack_audio_SLQSGetAudioProfile = {0x00};    //Audio Generator Voice
unpack_audio_SLQSGetAudioProfile_t tunpack_audio_SLQSGetAudioProfile;

pack_audio_SLQSSetAudioProfile_t tpack_audio_SLQSSetAudioProfile = {0x00 , 0x00, 0x00, 0x00, 0x04};//AudioProfileHS, earUnmute, micUnmute. audioGenVoice, audioVolume=4

pack_audio_SLQSGetAudioPathConfig_t tpack_audio_SLQSGetAudioPathConfig = {0x04, 0x00}; //Profile = 0x04; Item EC = 0x00;

/* Parameters for unpack_audio_SLQSGetAudioPathConfig_t */
uint8_t ECMode , NSEnable, MICGainSelect, RXSwitch, TXSwitch;
uint16_t TXGain, DTMFTXGain, CodecSTGain, TXPCMFlag, TXStageCnt, RXPCMFlag, RXStageCnt;
uint16_t RXStatGain, RXAIG, RXExpThr, RXExpSlope, RXComprThr, RXComprSlope;
uint16_t RXSens, RXHeadroom, TXStatGain, TXAIG, TXExpThr, TXExpSlope, TXComprThr, TXComprSlope;
uint8_t TXStage0[20], TXStage1[20], TXStage2[20], TXStage3[20], TXStage4[20];
uint8_t RXStage0[20], RXStage1[20], RXStage2[20], RXStage3[20], RXStage4[20];
audio_TXPCMIIRFltr tempTXPCMFLTR = { &TXPCMFlag, &TXStageCnt, TXStage0, TXStage1, TXStage2, TXStage3, TXStage4 };
audio_RXPCMIIRFltr tempRXPCMFLTR = { &RXPCMFlag, &RXStageCnt, RXStage0, RXStage1, RXStage2, RXStage3, RXStage4 };
audio_RXAGCList    tempRXAGCList = { &RXStatGain, &RXAIG, &RXExpThr, &RXExpSlope, &RXComprThr,&RXComprSlope };
audio_RXAVCList    tempRXAVCList = { &RXSens, &RXHeadroom };
audio_TXAGCList    tempTXAGCList = { &TXStatGain, &TXAIG, &TXExpThr, &TXExpSlope, &TXComprThr, &TXComprSlope};

unpack_audio_SLQSGetAudioPathConfig_t tunpack_audio_SLQSGetAudioPathConfig = {
        &ECMode, &NSEnable, &TXGain, &DTMFTXGain, &CodecSTGain, &tempTXPCMFLTR,
        &tempRXPCMFLTR, &MICGainSelect, &RXSwitch, &TXSwitch, &tempRXAGCList,
        &tempRXAVCList, &tempTXAGCList,{{0}} };

pack_audio_SLQSSetAudioPathConfig_t tpack_audio_SLQSSetAudioPathConfig = {
        0x00, &ECMode, &NSEnable, &TXGain, &DTMFTXGain, &CodecSTGain, &tempTXPCMFLTR,
        &tempRXPCMFLTR, &RXSwitch, &TXSwitch, &tempRXAGCList,
        &tempRXAVCList, &tempTXAGCList};

pack_audio_SLQSGetAudioVolTLBConfig_t tpack_audio_SLQSGetAudioVolTLBConfig = {
        0x00, 0x00, 0x03, 0x0E}; //audioProfile HS, audioGenerator =0 , audioVolLev = 3, audio DTMFVol
unpack_audio_SLQSGetAudioVolTLBConfig_t tunpack_audio_SLQSGetAudioVolTLBConfig;

pack_audio_SLQSSetAudioVolTLBConfig_t tpack_audio_SLQSSetAudioVolTLBConfig = {
        0x00, 0x00, 0x03, 0x0E ,00}; //audioProfile HS, audioGenerator =0 , audioVolLev = 3, audio DTMFVol, vol table = 0
unpack_audio_SLQSSetAudioVolTLBConfig_t tunpack_audio_SLQSSetAudioVolTLBConfig;

int audio_validate_dummy_unpack();

void dump_SLQSGetAudioProfile (void *ptr)
{
    unpack_audio_SLQSGetAudioProfile_t *result =
            (unpack_audio_SLQSGetAudioProfile_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf( "Profile          : %x\n", result->Profile );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf( "Ear Mute Setting : %x\n", result->EarMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf( "Mic Mute Setting : %x\n", result->MicMute );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        printf( "Volume Level     : %x\n", result->Volume );
}

void dump_SLQSSetAudioProfile (void *ptr)
{
    UNUSEDPARAM(ptr);
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetAudioPathConfig (void *ptr)
{
    unpack_audio_SLQSGetAudioPathConfig_t *result =
            (unpack_audio_SLQSGetAudioPathConfig_t*) ptr;
    uint8_t  lCtr = 0;
    printf("%s Data\n",__FUNCTION__);
    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
    #endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 33) && (result->pECMode))
        printf( "EC Mode          : 0x%x\n", *(result->pECMode) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 34) && (result->pNSEnable))
        printf( "NS Enable        : 0x%x\n", *(result->pNSEnable) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 35) && (result->pTXGain))
        printf( "TX Gain          : 0x%x\n", *(result->pTXGain) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 36) && (result->pDTMFTXGain))
        printf( "DTMF TX Gain     : 0x%x\n", *(result->pDTMFTXGain) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 37) && (result->pCodecSTGain))
        printf( "Codec ST Gain    : 0x%x\n", *(result->pCodecSTGain) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 38) && (result->pTXPCMIIRFltr))
    {
        printf( "TX PCM FLTR parameters: \n" );
        IFPRINTF( "Flag             : 0x%x\n", (result->pTXPCMIIRFltr->pFlag) );
        IFPRINTF( "Stage Count      : 0x%x\n", (result->pTXPCMIIRFltr->pStageCnt) );
        if(result->pTXPCMIIRFltr->pStage0Val)
        {
            printf( "Stage 0 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pTXPCMIIRFltr->pStage0Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pTXPCMIIRFltr->pStage1Val)
        {
            printf( "Stage 1 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pTXPCMIIRFltr->pStage1Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pTXPCMIIRFltr->pStage2Val)
        {
            printf( "Stage 2 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pTXPCMIIRFltr->pStage2Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pTXPCMIIRFltr->pStage3Val)
        {
            printf( "Stage 3 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pTXPCMIIRFltr->pStage3Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pTXPCMIIRFltr->pStage4Val)
        {
            printf( "Stage 4 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pTXPCMIIRFltr->pStage4Val[lCtr] );
            }
            printf( "\n" );
        }
    }
    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 39) && (result->pRXPCMIIRFltr))
    {
        printf( "RX PCM FLTR parameters: \n" );
        IFPRINTF( "Flag             : 0x%x\n", (result->pRXPCMIIRFltr->pFlag) );
        IFPRINTF( "Stage Count      : 0x%x\n", (result->pRXPCMIIRFltr->pStageCnt) );
        if(result->pRXPCMIIRFltr->pStage0Val)
        {
            printf( "Stage 0 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pRXPCMIIRFltr->pStage0Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pRXPCMIIRFltr->pStage1Val)
        {
            printf( "Stage 1 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pRXPCMIIRFltr->pStage1Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pRXPCMIIRFltr->pStage2Val)
        {
            printf( "Stage 2 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pRXPCMIIRFltr->pStage2Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pRXPCMIIRFltr->pStage3Val)
        {
            printf( "Stage 3 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pRXPCMIIRFltr->pStage3Val[lCtr] );
            }
            printf( "\n" );
        }

        if(result->pRXPCMIIRFltr->pStage4Val)
        {
            printf( "Stage 4 Value: " );
            for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
            {
                printf( "%X ", result->pRXPCMIIRFltr->pStage4Val[lCtr] );
            }
            printf( "\n" );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 40) && (result->pMICGainSelect))
        printf( "MIC Gain              : 0x%x\n", *(result->pMICGainSelect) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 41) && (result->pRXAVCAGCSwitch))
        printf( "RX AVC/AGC Switch     : 0x%x\n", *(result->pRXAVCAGCSwitch) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 42) && (result->pTXAVCSwitch))
        printf( "TX AVC Switch         : 0x%x\n", *(result->pTXAVCSwitch) );
    if(swi_uint256_get_bit (result->ParamPresenceMask, 43) && (result->pRXAGCList))
    {
        printf( "RX AGC List: \n" );
        IFPRINTF( "Static Gain           : 0x%x\n", (result->pRXAGCList->pRXStaticGain) );
        IFPRINTF( "Gain Selection Flag   : 0x%x\n", (result->pRXAGCList->pRXAIG) );
        IFPRINTF( "Expansion Threshold   : 0x%x\n", (result->pRXAGCList->pRXExpThres) );
        IFPRINTF( "Expansion Slope       : 0x%x\n", (result->pRXAGCList->pRXExpSlope) );
        IFPRINTF( "Compression Threshold : 0x%x\n", (result->pRXAGCList->pRXComprThres) );
        IFPRINTF( "Compression Slope     : 0x%x\n", (result->pRXAGCList->pRXComprSlope) );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 44) && (result->pRXAVCList))
    {
        printf( "RX AVC List: \n" );
        IFPRINTF( "AVC Sensitivity       : 0x%x\n", (result->pRXAVCList->pAVRXAVCSens) );
        IFPRINTF( "AVC Headroom          : 0x%x\n", (result->pRXAVCList->pAVRXAVCHeadroom) );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 45) && (result->pTXAGCList))
    {
        printf( "TX AGC List: \n" );
        IFPRINTF( "Static Gain           : 0x%x\n", (result->pTXAGCList->pTXStaticGain) );
        IFPRINTF( "Gain Selection Flag   : 0x%x\n", (result->pTXAGCList->pTXAIG) );
        IFPRINTF( "Expansion Threshold   : 0x%x\n", (result->pTXAGCList->pTXExpThres) );
        IFPRINTF( "Expansion Slope       : 0x%x\n", (result->pTXAGCList->pTXExpSlope) );
        IFPRINTF( "Compression Threshold : 0x%x\n", (result->pTXAGCList->pTXComprThres) );
        IFPRINTF( "Compression Slope     : 0x%x\n", (result->pTXAGCList->pTXComprSlope) );
    }
}

void dump_SLQSSetAudioPathConfig (void *ptr)
{
    UNUSEDPARAM(ptr);
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetAudioVolTLBConfig (void *ptr)
{
    unpack_audio_SLQSGetAudioVolTLBConfig_t *result =
            (unpack_audio_SLQSGetAudioVolTLBConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 33))
        printf( "Result Code Value          : %x\n", result->ResCode );
}

void dump_SLQSSetAudioVolTLBConfig (void *ptr)
{
    unpack_audio_SLQSSetAudioVolTLBConfig_t *result =
            (unpack_audio_SLQSSetAudioVolTLBConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
    #endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 33))
        printf( "Result Code Value          : %x\n", result->ResCode );
}

testitem_t audiototest[] = {
    {
        (pack_func_item) &pack_audio_SLQSGetAudioProfile, "pack_audio_SLQSGetAudioProfile",
        &tpack_audio_SLQSGetAudioProfile,
        (unpack_func_item) &unpack_audio_SLQSGetAudioProfile, "unpack_audio_SLQSGetAudioProfile",
        &tunpack_audio_SLQSGetAudioProfile, dump_SLQSGetAudioProfile
    },
    {
        (pack_func_item) &pack_audio_SLQSSetAudioProfile, "pack_audio_SLQSSetAudioProfile",
        &tpack_audio_SLQSSetAudioProfile,
        (unpack_func_item) &unpack_audio_SLQSSetAudioProfile, "unpack_audio_SLQSSetAudioProfile",
        NULL, dump_SLQSSetAudioProfile
    },
    {
        (pack_func_item) &pack_audio_SLQSGetAudioPathConfig, "pack_audio_SLQSGetAudioPathConfig",
        &tpack_audio_SLQSGetAudioPathConfig,
        (unpack_func_item) &unpack_audio_SLQSGetAudioPathConfig, "unpack_audio_SLQSGetAudioPathConfig",
        &tunpack_audio_SLQSGetAudioPathConfig, dump_SLQSGetAudioPathConfig
    },
    {
        (pack_func_item) &pack_audio_SLQSSetAudioPathConfig, "pack_audio_SLQSSetAudioPathConfig",
        &tpack_audio_SLQSSetAudioPathConfig,
        (unpack_func_item) &unpack_audio_SLQSSetAudioPathConfig, "unpack_audio_SLQSSetAudioPathConfig",
        NULL, dump_SLQSSetAudioPathConfig
    },
    {
        (pack_func_item) &pack_audio_SLQSGetAudioVolTLBConfig, "pack_audio_SLQSGetAudioVolTLBConfig",
        &tpack_audio_SLQSGetAudioVolTLBConfig,
        (unpack_func_item) &unpack_audio_SLQSGetAudioVolTLBConfig, "unpack_audio_SLQSGetAudioVolTLBConfig",
        &tunpack_audio_SLQSGetAudioVolTLBConfig, dump_SLQSGetAudioVolTLBConfig
    },
    {
        (pack_func_item) &pack_audio_SLQSSetAudioVolTLBConfig, "pack_audio_SLQSGetAudioVolTLBConfig",
        &tpack_audio_SLQSSetAudioVolTLBConfig,
        (unpack_func_item) &unpack_audio_SLQSSetAudioVolTLBConfig, "unpack_audio_SLQSGetAudioVolTLBConfig",
        &tunpack_audio_SLQSSetAudioVolTLBConfig, dump_SLQSSetAudioVolTLBConfig
    },
};

unsigned int audioarraylen = (unsigned int)((sizeof(audiototest))/(sizeof(audiototest[0])));

testitem_t audiototest_invalidunpack[] = {
    {
        (pack_func_item) &pack_audio_SLQSGetAudioProfile, "pack_audio_SLQSGetAudioProfile",
        &tpack_audio_SLQSGetAudioProfile,
        (unpack_func_item) &unpack_audio_SLQSGetAudioProfile, "unpack_audio_SLQSGetAudioProfile",
        NULL, dump_SLQSGetAudioProfile
    },
    {
        (pack_func_item) &pack_audio_SLQSGetAudioPathConfig, "pack_audio_SLQSGetAudioPathConfig",
        &tpack_audio_SLQSGetAudioPathConfig,
        (unpack_func_item) &unpack_audio_SLQSGetAudioPathConfig, "unpack_audio_SLQSGetAudioPathConfig",
        NULL, dump_SLQSGetAudioPathConfig
    },
    {
        (pack_func_item) &pack_audio_SLQSGetAudioVolTLBConfig, "pack_audio_SLQSGetAudioVolTLBConfig",
        &tpack_audio_SLQSGetAudioVolTLBConfig,
        (unpack_func_item) &unpack_audio_SLQSGetAudioVolTLBConfig, "unpack_audio_SLQSGetAudioVolTLBConfig",
        NULL, dump_SLQSGetAudioVolTLBConfig
    },
    {
        (pack_func_item) &pack_audio_SLQSSetAudioVolTLBConfig, "pack_audio_SLQSGetAudioVolTLBConfig",
        &tpack_audio_SLQSSetAudioVolTLBConfig,
        (unpack_func_item) &unpack_audio_SLQSSetAudioVolTLBConfig, "unpack_audio_SLQSGetAudioVolTLBConfig",
        NULL, dump_SLQSSetAudioVolTLBConfig
    },
};


void *audio_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enAUDIOThread)
    {
        if(audio<1)
        {
            usleep(1000);
            printf("%s fd error\n", __func__);
            continue;
        }
        //TODO select multiple file and read them
        rtn = read(audio, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            uint16_t rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eAUDIO, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("AUDIO IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("AUDIO RSP: ");
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

void audio_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======AUDIO pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(audiototest)/sizeof(testitem_t); i++)
    {
        if(audiototest[i].dump!=NULL)
        {
            audiototest[i].dump(NULL); 
        }
    }
    for(i=0; i<sizeof(audiototest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(audiototest_invalidunpack[i].dump!=NULL)
        {
            audiototest_invalidunpack[i].dump(NULL); 
        }
    }
    for(i=0; i<sizeof(audiototest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(audiototest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,audiototest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(audio<0)
            audio = client_fd(eAUDIO);
        if(audio<0)
        {
            fprintf(stderr,"AUDIO Service Not Supported!\n");
            return ;
        }
        rtn = write(audio, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(audio>=0)
                close(audio);
            audio=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eAUDIO, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen,req);
        }

        rtn = read(audio, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eAUDIO, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(audiototest_invalidunpack[i].unpack)(rsp, rspLen, 
                                       audiototest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           audiototest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    audiototest_invalidunpack[i].dump(audiototest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(audio>=0)
                close(audio);
            audio=-1;
        }
        sleep(1);
        xid++;
    }
    if(audio>=0)
        close(audio);
    audio=-1;
}

void audio_test_pack_unpack_loop()
{
    unsigned i;

    printf("======AUDIO dummy unpack test===========\n");
    audio_validate_dummy_unpack();
    printf("======AUDIO pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "AUDIO Pack/UnPack API Name", "Status", "Unpack Payload Parsing");
#endif

    unsigned xid =1;
    if(audio<0)
       audio = client_fd(eAUDIO);
    if(audio<0)
    {
        fprintf(stderr,"AUDIO Service Not Supported!\n");
        return ;
    }
    for(i=0; i<sizeof(audiototest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(audiototest[i].pack)(&req_ctx, req, &reqLen,audiototest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", audiototest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }


        rtn = write(audio, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eAUDIO, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen,req);
        }

        while (1)
        {
            rtn = rspLen = read(audio, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                helper_get_resp_ctx(eAUDIO, rsp, rspLen, &rsp_ctx);

                printf("<< receiving\n");
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("AUDIO RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(audiototest[i].unpack)(rsp, rspLen, audiototest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", audiototest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                    #endif

                        audiototest[i].dump(audiototest[i].unpack_ptr);
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
                    printf("AUDIO IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(audio>=0)
        close(audio);
    audio=-1;
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_audio_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_AUDIO_GET_AUDIO_PATH_CONFIG */
   {0x02,0x01,0x00,0x64,0x00,0x25,0x01,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x21,0x01,
    0x00,0x01,0x22,0x01,0x00,0x01,0x23,0x02,0x00,0x01,0x00,0x24,0x02,0x00,0x01,0x00,
    0x25,0x02,0x00,0x01,0x00,0x26,0x68,0x00,0x01,0x00,0x02,0x00,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x27,
    0x68,0x00,0x01,0x00,0x02,0x00,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
    0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,
    0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,
    0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,
    0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
    0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,
    0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x28,0x01,0x00,0x01,0x29,0x01,0x00,
    0x01,0x2A,0x01,0x00,0x01,0x2B,0x0c,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
    0x01,0x00,0x01,0x00,0x2D,0x0c,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,
    0x00,0x01,0x00,0x2C,0x04,0x00,0x01,0x00,0x01,0x00},

    /* eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG */
   {0x02,0x02,0x00,0x66,0x00,0x0C,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x21,0x02,
    0x00,0x01,0x00},

    /* eQMI_AUDIO_GET_PROFILE */
   {0x02,0x03,0x00,0x60,0x00,0x17,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x01,0x11,0x01,0x00,0x01,0x12,0x01,0x00,0x01,0x14,0x01,0x00,0x01},

    /* eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG */
   {0x02,0x04,0x00,0x67,0x00,0x0C,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x21,0x02,
    0x00,0x02,0x00}
   };


/* eQMI_AUDIO_GET_AUDIO_PATH_CONFIG */
uint8_t cst_ECMode = 1; 
uint8_t cst_NSEnable = 1;
uint8_t cst_MICGainSelect = 1;
uint8_t cst_RXSwitch = 1;
uint8_t cst_TXSwitch = 1;
uint16_t cst_TXGain = 1;
uint16_t cst_DTMFTXGain = 1;
uint16_t cst_CodecSTGain = 1;
uint16_t cst_TXPCMFlag = 1;
uint16_t cst_TXStageCnt = 2;
uint16_t cst_RXPCMFlag = 1;
uint16_t cst_RXStageCnt = 2;
uint16_t cst_RXStatGain = 1;
uint16_t cst_RXAIG = 1;
uint16_t cst_RXExpThr = 1;
uint16_t cst_RXExpSlope = 1;
uint16_t cst_RXComprThr = 1;
uint16_t cst_RXComprSlope = 1;
uint16_t cst_RXSens = 1;
uint16_t cst_RXHeadroom = 1;
uint16_t cst_TXStatGain = 1;
uint16_t cst_TXAIG = 1; 
uint16_t cst_TXExpThr = 1;
uint16_t cst_TXExpSlope = 1;
uint16_t cst_TXComprThr = 1;
uint16_t cst_TXComprSlope = 1;
uint8_t cst_TXStage0[20] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
uint8_t cst_TXStage1[20] = {23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
uint8_t cst_TXStage2[20] = {43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62};
uint8_t cst_TXStage3[20] = {63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82}; 
uint8_t cst_TXStage4[20] = {83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102};
uint8_t cst_RXStage0[20] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
uint8_t cst_RXStage1[20] = {23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
uint8_t cst_RXStage2[20] = {43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62};
uint8_t cst_RXStage3[20] = {63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82}; 
uint8_t cst_RXStage4[20] = {83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102};
audio_TXPCMIIRFltr cst_tempTXPCMFLTR = { &cst_TXPCMFlag, &cst_TXStageCnt, cst_TXStage0, cst_TXStage1, cst_TXStage2, cst_TXStage3, cst_TXStage4 };
audio_RXPCMIIRFltr cst_tempRXPCMFLTR = { &cst_RXPCMFlag, &cst_RXStageCnt, cst_RXStage0, cst_RXStage1, cst_RXStage2, cst_RXStage3, cst_RXStage4 };
audio_RXAGCList    cst_tempRXAGCList = { &cst_RXStatGain, &cst_RXAIG, &cst_RXExpThr, &cst_RXExpSlope, &cst_RXComprThr,&cst_RXComprSlope };
audio_RXAVCList    cst_tempRXAVCList = { &cst_RXSens, &cst_RXHeadroom };
audio_TXAGCList    cst_tempTXAGCList = { &cst_TXStatGain, &cst_TXAIG, &cst_TXExpThr, &cst_TXExpSlope, &cst_TXComprThr, &cst_TXComprSlope};

const unpack_audio_SLQSGetAudioPathConfig_t const_unpack_audio_SLQSGetAudioPathConfig_t = {
        &cst_ECMode, &cst_NSEnable, &cst_TXGain, &cst_DTMFTXGain, &cst_CodecSTGain, &cst_tempTXPCMFLTR,
        &cst_tempRXPCMFLTR, &cst_MICGainSelect, &cst_RXSwitch, &cst_TXSwitch, &cst_tempRXAGCList,
        &cst_tempRXAVCList, &cst_tempTXAGCList,{{SWI_UINT256_BIT_VALUE(SET_14_BITS,2,33,34,35,36,37,38,39,40,41,42,43,44,45)}} };

uint8_t var_ECMode; 
uint8_t var_NSEnable;
uint8_t var_MICGainSelect;
uint8_t var_RXSwitch;
uint8_t var_TXSwitch;
uint16_t var_TXGain;
uint16_t var_DTMFTXGain;
uint16_t var_CodecSTGain;
uint16_t var_TXPCMFlag;
uint16_t var_TXStageCnt;
uint16_t var_RXPCMFlag;
uint16_t var_RXStageCnt;
uint16_t var_RXStatGain;
uint16_t var_RXAIG;
uint16_t var_RXExpThr;
uint16_t var_RXExpSlope;
uint16_t var_RXComprThr;
uint16_t var_RXComprSlope;
uint16_t var_RXSens;
uint16_t var_RXHeadroom;
uint16_t var_TXStatGain;
uint16_t var_TXAIG; 
uint16_t var_TXExpThr;
uint16_t var_TXExpSlope;
uint16_t var_TXComprThr;
uint16_t var_TXComprSlope;
uint8_t var_TXStage0[20];
uint8_t var_TXStage1[20];
uint8_t var_TXStage2[20];
uint8_t var_TXStage3[20]; 
uint8_t var_TXStage4[20];
uint8_t var_RXStage0[20];
uint8_t var_RXStage1[20];
uint8_t var_RXStage2[20];
uint8_t var_RXStage3[20]; 
uint8_t var_RXStage4[20];
audio_TXPCMIIRFltr var_tempTXPCMFLTR = { &var_TXPCMFlag, &var_TXStageCnt, var_TXStage0, var_TXStage1, var_TXStage2, var_TXStage3, var_TXStage4 };
audio_RXPCMIIRFltr var_tempRXPCMFLTR = { &var_RXPCMFlag, &var_RXStageCnt, var_RXStage0, var_RXStage1, var_RXStage2, var_RXStage3, var_RXStage4 };
audio_RXAGCList    var_tempRXAGCList = { &var_RXStatGain, &var_RXAIG, &var_RXExpThr, &var_RXExpSlope, &var_RXComprThr,&var_RXComprSlope };
audio_RXAVCList    var_tempRXAVCList = { &var_RXSens, &var_RXHeadroom };
audio_TXAGCList    var_tempTXAGCList = { &var_TXStatGain, &var_TXAIG, &var_TXExpThr, &var_TXExpSlope, &var_TXComprThr, &var_TXComprSlope};
unpack_audio_SLQSGetAudioPathConfig_t var_unpack_audio_SLQSGetAudioPathConfig_t = {
        &var_ECMode, &var_NSEnable, &var_TXGain, &var_DTMFTXGain, &var_CodecSTGain, &var_tempTXPCMFLTR,
        &var_tempRXPCMFLTR, &var_MICGainSelect, &var_RXSwitch, &var_TXSwitch, &var_tempRXAGCList,
        &var_tempRXAVCList, &var_tempTXAGCList,{{0}} };


/* eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG */
const unpack_audio_SLQSGetAudioVolTLBConfig_t const_unpack_audio_SLQSGetAudioVolTLBConfig_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,33)}} };

/* eQMI_AUDIO_GET_PROFILE */
const unpack_audio_SLQSGetAudioProfile_t const_unpack_audio_SLQSGetAudioProfile_t = {
        1,1,1,1,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,20)}} };

/* eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG */
const unpack_audio_SLQSSetAudioVolTLBConfig_t const_unpack_audio_SLQSSetAudioVolTLBConfig_t = {
        2,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,33)}} };


int audio_validate_dummy_unpack()
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
    loopCount = sizeof(validate_audio_resp_msg)/sizeof(validate_audio_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_audio_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eAUDIO, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("AUDIO IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("AUDIO RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_AUDIO_GET_AUDIO_PATH_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_audio_SLQSGetAudioPathConfig_t *varp = &var_unpack_audio_SLQSGetAudioPathConfig_t;
                    const unpack_audio_SLQSGetAudioPathConfig_t *cstp = &const_unpack_audio_SLQSGetAudioPathConfig_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_audio_SLQSGetAudioPathConfig,
                    dump_SLQSGetAudioPathConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_audio_SLQSGetAudioPathConfig_t,
                    37,
                    CMP_PTR_TYPE, varp->pECMode, cstp->pECMode,
                    CMP_PTR_TYPE, varp->pNSEnable, cstp->pNSEnable,
                    CMP_PTR_TYPE, varp->pTXGain, cstp->pTXGain,
                    CMP_PTR_TYPE, varp->pDTMFTXGain, cstp->pDTMFTXGain,
                    CMP_PTR_TYPE, varp->pCodecSTGain, cstp->pCodecSTGain,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pFlag, cstp->pTXPCMIIRFltr->pFlag,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStageCnt, cstp->pTXPCMIIRFltr->pStageCnt,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStage0Val, cstp->pTXPCMIIRFltr->pStage0Val,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStage1Val, cstp->pTXPCMIIRFltr->pStage1Val,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStage2Val, cstp->pTXPCMIIRFltr->pStage2Val,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStage3Val, cstp->pTXPCMIIRFltr->pStage3Val,
                    CMP_PTR_TYPE, varp->pTXPCMIIRFltr->pStage4Val, cstp->pTXPCMIIRFltr->pStage4Val,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pFlag, cstp->pRXPCMIIRFltr->pFlag,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStageCnt, cstp->pRXPCMIIRFltr->pStageCnt,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStage0Val, cstp->pRXPCMIIRFltr->pStage0Val,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStage1Val, cstp->pRXPCMIIRFltr->pStage1Val,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStage2Val, cstp->pRXPCMIIRFltr->pStage2Val,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStage3Val, cstp->pRXPCMIIRFltr->pStage3Val,
                    CMP_PTR_TYPE, varp->pRXPCMIIRFltr->pStage4Val, cstp->pRXPCMIIRFltr->pStage4Val,
                    CMP_PTR_TYPE, varp->pMICGainSelect, cstp->pMICGainSelect,
                    CMP_PTR_TYPE, varp->pRXAVCAGCSwitch, cstp->pRXAVCAGCSwitch,
                    CMP_PTR_TYPE, varp->pTXAVCSwitch, cstp->pTXAVCSwitch,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXStaticGain, cstp->pRXAGCList->pRXStaticGain,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXAIG, cstp->pRXAGCList->pRXAIG,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXExpThres, cstp->pRXAGCList->pRXExpThres,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXExpSlope, cstp->pRXAGCList->pRXExpSlope,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXComprThres, cstp->pRXAGCList->pRXComprThres,
                    CMP_PTR_TYPE, varp->pRXAGCList->pRXComprSlope, cstp->pRXAGCList->pRXComprSlope,
                    CMP_PTR_TYPE, varp->pRXAVCList->pAVRXAVCSens, cstp->pRXAVCList->pAVRXAVCSens,
                    CMP_PTR_TYPE, varp->pRXAVCList->pAVRXAVCHeadroom, cstp->pRXAVCList->pAVRXAVCHeadroom,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXStaticGain, cstp->pTXAGCList->pTXStaticGain,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXAIG, cstp->pTXAGCList->pTXAIG,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXExpThres, cstp->pTXAGCList->pTXExpThres,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXExpSlope, cstp->pTXAGCList->pTXExpSlope,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXComprThres, cstp->pTXAGCList->pTXComprThres,
                    CMP_PTR_TYPE, varp->pTXAGCList->pTXComprSlope, cstp->pTXAGCList->pTXComprSlope,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_audio_SLQSGetAudioVolTLBConfig,
                    dump_SLQSGetAudioVolTLBConfig,
                    msg.buf,
                    rlen,
                    &const_unpack_audio_SLQSGetAudioVolTLBConfig_t);
                }
                break;
            case eQMI_AUDIO_GET_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_audio_SLQSGetAudioProfile,
                    dump_SLQSGetAudioProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_audio_SLQSGetAudioProfile_t);
                }
                break;
            case eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_audio_SLQSSetAudioVolTLBConfig,
                    dump_SLQSSetAudioVolTLBConfig,
                    msg.buf,
                    rlen,
                    &const_unpack_audio_SLQSSetAudioVolTLBConfig_t);
                }
                break;
            }
        }
    }
    return 0;
}

