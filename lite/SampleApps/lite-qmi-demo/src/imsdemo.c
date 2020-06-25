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
#include "ims.h"

#define STAGESIZECOUNT 20

int ims_validate_dummy_unpack();

int ims = -1;
volatile int enIMSThread =0;
pthread_t ims_tid;
pthread_attr_t ims_attr;

uint16_t SIPLocalPort   = 0x00;
uint32_t TimerSIPReg    = 0x1000;
uint32_t SubscribeTimer = 0x1000;
uint32_t TimerT1        = 0x1000;
uint32_t TimerT2        = 0x1000;
uint32_t TimerTf        = 0x1000;
uint8_t  SigCompEnabled = 0x01;

pack_ims_SLQSSetSIPConfig_t tpack_ims_SLQSSetSIPConfig = {&SIPLocalPort, &TimerSIPReg, &SubscribeTimer,
        &TimerT1, &TimerT2, &TimerTf, &SigCompEnabled };

uint8_t SettingResp = 0xFF;
unpack_ims_SLQSSetSIPConfig_t tunpack_ims_SLQSSetSIPConfig = { &SettingResp, {{0}} };

uint16_t PriCSCFPort      = 0x00;
uint8_t  CSCFPortNameLen  = 0x06;
uint8_t  CSCFPortName[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };
uint8_t  IMSTestMode      = 0x01;

pack_ims_SLQSSetRegMgrConfig_t tpack_ims_SLQSSetRegMgrConfig = {&PriCSCFPort, &CSCFPortNameLen,
        CSCFPortName, &IMSTestMode };

unpack_ims_SLQSSetRegMgrConfig_t tunpack_ims_SLQSSetRegMgrConfig = { &SettingResp, {{0}} };

uint8_t SMSFormat        =  0x00; //3GPP
uint8_t SMSOverIPNwInd   =  0x01;
uint8_t PhoneCtxtURILen  =  0x06;
uint8_t PhoneCtxtURI[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };
pack_ims_SLQSSetIMSSMSConfig_t tpack_ims_SLQSSetIMSSMSConfig = {&SMSFormat, &SMSOverIPNwInd,
        &PhoneCtxtURILen, PhoneCtxtURI };

unpack_ims_SLQSSetIMSSMSConfig_t tunpack_ims_SLQSSetIMSSMSConfig= { &SettingResp, {{0}} };

uint8_t IMSDomainLen  = 0x06;;
uint8_t IMSDomain[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };
pack_ims_SLQSSetIMSUserConfig_t tpack_ims_SLQSSetIMSUserConfig = {&IMSDomainLen ,IMSDomain};

unpack_ims_SLQSSetIMSUserConfig_t tunpack_ims_SLQSSetIMSUserConfig = { &SettingResp, {{0}} };

uint16_t SessionExpiryTimer = 0x100;
uint16_t MinSessionExpiryTimer = 0x10;
uint8_t  AmrWbEnable = 0x01;
uint8_t  ScrAmrEnable = 0x01;
uint8_t  ScrAmrWbEnable = 0x01;
uint8_t  AmrMode = 0xFF;
uint16_t AmrWBMode = 0xFF;
uint8_t  AmrOctetAligned = 0x01;
uint8_t  AmrWBOctetAligned = 0x01;
uint16_t RingingTimer = 0x40;
uint16_t RingBackTimer = 0x40;
uint16_t RTPRTCPInactTimer = 0x40;

pack_ims_SLQSSetIMSVoIPConfig_t tpack_ims_SLQSSetIMSVoIPConfig ={&SessionExpiryTimer,
        &MinSessionExpiryTimer, &AmrWbEnable, &ScrAmrEnable, &ScrAmrWbEnable, &AmrMode,
        &AmrWBMode, &AmrOctetAligned, &AmrWBOctetAligned, &RingingTimer, &RingBackTimer,
        &RTPRTCPInactTimer};

unpack_ims_SLQSSetIMSVoIPConfig_t tunpack_ims_SLQSSetIMSVoIPConfig = { &SettingResp, {{0}} };

uint8_t  settingsResp   = 0xFF;
uint16_t gSIPLocalPort   = 0xFFFF;
uint32_t gTimerSIPReg    = 0xFFFFFFFF;
uint32_t gSubscribeTimer = 0xFFFFFFFF;
uint32_t gTimerT1        = 0xFFFFFFFF;
uint32_t gTimerT2        = 0xFFFFFFFF;
uint32_t gTimerTf        = 0xFFFFFFFF;
uint8_t  gSigCompEnabled = 0xFF;

unpack_ims_SLQSGetSIPConfig_t tunpack_ims_SLQSGetSIPConfig = {&settingsResp, &gSIPLocalPort,
        &gTimerSIPReg, &gSubscribeTimer, &gTimerT1, &gTimerT2, &gTimerTf, &gSigCompEnabled, {{0}} };

uint16_t gProxyCSCF    = 0xFFFF;
uint8_t gPriCSCFLen    = 0xFF;
uint8_t gPriCSCFName[255];
uint8_t gIMSTestMode   = 0xFF;

unpack_ims_SLQSGetRegMgrConfig_t tunpack_ims_SLQSGetRegMgrConfig ={&settingsResp, &gProxyCSCF,
        &gPriCSCFLen, gPriCSCFName, &gIMSTestMode, {{0}} };

uint8_t gSMSFmt       = 0xFF;
uint8_t gSMSoIPNIF    = 0xFF;
uint8_t gPhCtxtURILen = 0xFF;
uint8_t gPhCtxtURI[255];

unpack_ims_SLQSGetIMSSMSConfig_t tunpack_ims_SLQSGetIMSSMSConfig = {&settingsResp, &gSMSFmt,
        &gSMSoIPNIF, &gPhCtxtURILen, gPhCtxtURI, {{0}} };

uint8_t gIMSDomainLen = 0xFF;
uint8_t gIMSDomain[255];
unpack_ims_SLQSGetIMSUserConfig_t tunpack_ims_SLQSGetIMSUserConfig = {&settingsResp,
        &gIMSDomainLen, gIMSDomain, {{0}} };

uint16_t gSessExpTimer    = 0xFFFF;
uint16_t gMinSessExpTmr   = 0xFFFF;
uint8_t  gAmrWbEnable     = 0xFF;
uint8_t  gScrAmrEnable    = 0xFF;
uint8_t  gScrAmrWbEnable  = 0xFF;
uint8_t  gAmrMode         = 0xFF;
uint16_t gAmrWbMode       = 0xFF;
uint8_t  gAmrOctetAlign   = 0xFF;
uint8_t  gAmrWbOctetAlign = 0xFF;
uint16_t gRingTmr         = 0xFFFF;
uint16_t gRingBkTmr       = 0xFFFF;
uint16_t gRTPRTCPInactTmr = 0xFFFF;
unpack_ims_SLQSGetIMSVoIPConfig_t tunpack_ims_SLQSGetIMSVoIPConfig = {&settingsResp, &gSessExpTimer,
        &gMinSessExpTmr, &gAmrWbEnable, &gScrAmrEnable, &gScrAmrWbEnable, &gAmrMode, &gAmrWbMode,
        &gAmrOctetAlign, &gAmrWbOctetAlign, &gRingTmr, &gRingBkTmr, &gRTPRTCPInactTmr, {{0}} };

uint8_t SIPConfigEvent = 0x01;
uint8_t RegMgrCfgEvent = 0x01;
uint8_t SmsCfgEvent    = 0x01;
uint8_t UserCfgEvent   = 0x01;
uint8_t VoipCfgEvent   = 0x01;
pack_ims_SLQSImsConfigIndicationRegister_t tpack_ims_SLQSImsConfigIndicationRegister = {&SIPConfigEvent,
        &RegMgrCfgEvent, &SmsCfgEvent, &UserCfgEvent, &VoipCfgEvent};
unpack_ims_SLQSImsConfigIndicationRegister_t tunpack_ims_SLQSImsConfigIndicationRegister;
void dump_SLQSSetSIPConfig (void *ptr)
{
    unpack_ims_SLQSSetSIPConfig_t *result =
            (unpack_ims_SLQSSetSIPConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
}

void dump_SLQSSetRegMgrConfig (void *ptr)
{
    unpack_ims_SLQSSetRegMgrConfig_t *result =
            (unpack_ims_SLQSSetRegMgrConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
}

void dump_SLQSSetIMSSMSConfig (void *ptr)
{
    unpack_ims_SLQSSetIMSSMSConfig_t *result =
            (unpack_ims_SLQSSetIMSSMSConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
}

void dump_SLQSSetIMSUserConfig (void *ptr)
{
    unpack_ims_SLQSSetIMSUserConfig_t *result =
            (unpack_ims_SLQSSetIMSUserConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
}

void dump_SLQSSetIMSVoIPConfig (void *ptr)
{
    unpack_ims_SLQSSetIMSVoIPConfig_t *result =
            (unpack_ims_SLQSSetIMSVoIPConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
}

void dump_SLQSGetSIPConfig (void *ptr)
{
    unpack_ims_SLQSGetSIPConfig_t *result =
            (unpack_ims_SLQSGetSIPConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
    if( (result->pSIPLocalPort) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("SIP Local Port    :0x%x\n", *result->pSIPLocalPort );
    if( (result->pTimerSIPReg) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf("Timer SIP Regn.   :0x%x\n", *result->pTimerSIPReg );
    if( (result->pSubscribeTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf("Subscribe Timer   :0x%x\n", *result->pSubscribeTimer );
    if( (result->pTimerT1) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
        printf("Timer T1          :0x%x\n", *result->pTimerT1 );
    if( (result->pTimerT2) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
        printf("Timer T2          :0x%x\n", *result->pTimerT2 );
    if( (result->pTimerTf) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)) )
        printf("Timer Tf          :0x%x\n", *result->pTimerTf );
    if( (result->pSigCompEnabled) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)) )
        printf("SigComp Status    :0x%x\n", *result->pSigCompEnabled );
}

void dump_SLQSGetRegMgrConfig (void *ptr)
{
    unpack_ims_SLQSGetRegMgrConfig_t *result =
            (unpack_ims_SLQSGetRegMgrConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);    
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response   :0x%x\n", *result->pSettingResp );
    if( (result->pPCSCFPort) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Proxy CSCF port     :0x%x\n", *result->pPCSCFPort );
    if( (result->pPriCSCFPortName) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf("Prim. CSCF port name :%s\n", result->pPriCSCFPortName );
    if( (result->pIMSTestMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf("IMS Test Mode        :0x%x\n", *result->pIMSTestMode );
}

void dump_SLQSGetIMSSMSConfig (void *ptr)
{
    unpack_ims_SLQSGetIMSSMSConfig_t *result =
            (unpack_ims_SLQSGetIMSSMSConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);    
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf( "Settings Response    :0x%x\n", *result->pSettingResp );
    if( (result->pSMSFormat) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("SMS Format           :0x%x\n", *result->pSMSFormat );
    if( (result->pSMSOverIPNwInd) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf("SMS over NW IP Ind.  :0x%x\n", *result->pSMSOverIPNwInd );
    if( (result->pPhoneCtxtURI) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf("Phone Context URI    :%s\n", result->pPhoneCtxtURI );
}

void dump_SLQSGetIMSUserConfig (void *ptr)
{
    unpack_ims_SLQSGetIMSUserConfig_t *result =
            (unpack_ims_SLQSGetIMSUserConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response :0x%x\n", *result->pSettingResp );
    if( (result->pIMSDomain) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("IMS Domain Name   :%s\n", result->pIMSDomain );
}

void dump_SLQSGetIMSVoIPConfig (void *ptr)
{
    unpack_ims_SLQSGetIMSVoIPConfig_t *result =
            (unpack_ims_SLQSGetIMSVoIPConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if( (result->pSettingResp) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("Settings Response :0x%x\n", *result->pSettingResp );
    if( (result->pSessionExpiryTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Session Expiry Timer      :0x%x\n", *result->pSessionExpiryTimer );
    if( (result->pMinSessionExpiryTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf("Min. Session Expiry Timer :0x%x\n", *result->pMinSessionExpiryTimer );
    if( (result->pAmrWbEnable) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf("Enable AMR WB             :0x%x\n", *result->pAmrWbEnable );
    if( (result->pScrAmrEnable) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
        printf("SCR AMR Enable            :0x%x\n", *result->pScrAmrEnable );
    if( (result->pScrAmrWbEnable) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
        printf("SCR AMR WB Enable         :0x%x\n", *result->pScrAmrWbEnable );
    if( (result->pAmrMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)) )
        printf("AMR Mode                  :0x%x\n", *result->pAmrMode );
    if( (result->pAmrWBMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)) )
        printf("AMR WB Mode               :0x%x\n", *result->pAmrWBMode );
    if( (result->pAmrOctetAligned) && (swi_uint256_get_bit (result->ParamPresenceMask, 24)) )
        printf("AMR Octet Aligned         :0x%x\n", *result->pAmrOctetAligned );
    if( (result->pAmrWBOctetAligned) && (swi_uint256_get_bit (result->ParamPresenceMask, 25)) )
        printf("AMR WB Octet Aligned      :0x%x\n", *result->pAmrWBOctetAligned );
    if( (result->pRingingTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 26)) )
        printf("Ringing Timer             :0x%x\n", *result->pRingingTimer );
    if( (result->pRingBackTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 27)) )
        printf("Ring Back Timer           :0x%x\n", *result->pRingBackTimer );
    if( (result->pRTPRTCPInactTimer) && (swi_uint256_get_bit (result->ParamPresenceMask, 28)) )
        printf("RTP/RTCP Inactivity Timer :0x%x\n", *result->pRTPRTCPInactTimer );
}

void dump_SLQSImsConfigIndicationRegister (void *ptr)
{
    unpack_ims_SLQSImsConfigIndicationRegister_t *result =
            (unpack_ims_SLQSImsConfigIndicationRegister_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

testitem_t imstotest[] = {
    {
        (pack_func_item) &pack_ims_SLQSSetSIPConfig, "pack_ims_SLQSSetSIPConfig",
        &tpack_ims_SLQSSetSIPConfig,
        (unpack_func_item) &unpack_ims_SLQSSetSIPConfig, "unpack_ims_SLQSSetSIPConfig",
        &tunpack_ims_SLQSSetSIPConfig, dump_SLQSSetSIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetRegMgrConfig, "pack_ims_SLQSSetRegMgrConfig",
        &tpack_ims_SLQSSetRegMgrConfig,
        (unpack_func_item) &unpack_ims_SLQSSetRegMgrConfig, "unpack_ims_SLQSSetRegMgrConfig",
        &tunpack_ims_SLQSSetRegMgrConfig, dump_SLQSSetRegMgrConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSSMSConfig, "pack_ims_SLQSSetIMSSMSConfig",
        &tpack_ims_SLQSSetIMSSMSConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSSMSConfig, "unpack_ims_SLQSSetIMSSMSConfig",
        &tunpack_ims_SLQSSetIMSSMSConfig, dump_SLQSSetIMSSMSConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSUserConfig, "pack_ims_SLQSSetIMSUserConfig",
        &tpack_ims_SLQSSetIMSUserConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSUserConfig, "unpack_ims_SLQSSetIMSUserConfig",
        &tunpack_ims_SLQSSetIMSUserConfig, dump_SLQSSetIMSUserConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSVoIPConfig, "pack_ims_SLQSSetIMSVoIPConfig",
        &tpack_ims_SLQSSetIMSVoIPConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSVoIPConfig, "unpack_ims_SLQSSetIMSVoIPConfig",
        &tunpack_ims_SLQSSetIMSVoIPConfig, dump_SLQSSetIMSVoIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetSIPConfig, "pack_ims_SLQSGetSIPConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetSIPConfig, "unpack_ims_SLQSGetSIPConfig",
        &tunpack_ims_SLQSGetSIPConfig, dump_SLQSGetSIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetRegMgrConfig, "pack_ims_SLQSGetRegMgrConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetRegMgrConfig, "unpack_ims_SLQSGetRegMgrConfig",
        &tunpack_ims_SLQSGetRegMgrConfig, dump_SLQSGetRegMgrConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSSMSConfig, "pack_ims_SLQSGetIMSSMSConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSSMSConfig, "unpack_ims_SLQSGetIMSSMSConfig",
        &tunpack_ims_SLQSGetIMSSMSConfig, dump_SLQSGetIMSSMSConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSUserConfig, "pack_ims_SLQSGetIMSUserConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSUserConfig, "unpack_ims_SLQSGetIMSUserConfig",
        &tunpack_ims_SLQSGetIMSUserConfig, dump_SLQSGetIMSUserConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSVoIPConfig, "pack_ims_SLQSGetIMSVoIPConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSVoIPConfig, "unpack_ims_SLQSGetIMSVoIPConfig",
        &tunpack_ims_SLQSGetIMSVoIPConfig, dump_SLQSGetIMSVoIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSImsConfigIndicationRegister, "pack_ims_SLQSImsConfigIndicationRegister",
        &tpack_ims_SLQSImsConfigIndicationRegister,
        (unpack_func_item) &unpack_ims_SLQSImsConfigIndicationRegister, "unpack_ims_SLQSImsConfigIndicationRegister",
        &tunpack_ims_SLQSImsConfigIndicationRegister, dump_SLQSImsConfigIndicationRegister
    },
};

unsigned int imsarraylen = (unsigned int)((sizeof(imstotest))/(sizeof(imstotest[0])));

testitem_t imstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_ims_SLQSSetSIPConfig, "pack_ims_SLQSSetSIPConfig",
        &tpack_ims_SLQSSetSIPConfig,
        (unpack_func_item) &unpack_ims_SLQSSetSIPConfig, "unpack_ims_SLQSSetSIPConfig",
        NULL, dump_SLQSSetSIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetRegMgrConfig, "pack_ims_SLQSSetRegMgrConfig",
        &tpack_ims_SLQSSetRegMgrConfig,
        (unpack_func_item) &unpack_ims_SLQSSetRegMgrConfig, "unpack_ims_SLQSSetRegMgrConfig",
        NULL, dump_SLQSSetRegMgrConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSSMSConfig, "pack_ims_SLQSSetIMSSMSConfig",
        &tpack_ims_SLQSSetIMSSMSConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSSMSConfig, "unpack_ims_SLQSSetIMSSMSConfig",
        NULL, dump_SLQSSetIMSSMSConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSUserConfig, "pack_ims_SLQSSetIMSUserConfig",
        &tpack_ims_SLQSSetIMSUserConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSUserConfig, "unpack_ims_SLQSSetIMSUserConfig",
        NULL, dump_SLQSSetIMSUserConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSSetIMSVoIPConfig, "pack_ims_SLQSSetIMSVoIPConfig",
        &tpack_ims_SLQSSetIMSVoIPConfig,
        (unpack_func_item) &unpack_ims_SLQSSetIMSVoIPConfig, "unpack_ims_SLQSSetIMSVoIPConfig",
        NULL, dump_SLQSSetIMSVoIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetSIPConfig, "pack_ims_SLQSGetSIPConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetSIPConfig, "unpack_ims_SLQSGetSIPConfig",
        NULL, dump_SLQSGetSIPConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetRegMgrConfig, "pack_ims_SLQSGetRegMgrConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetRegMgrConfig, "unpack_ims_SLQSGetRegMgrConfig",
        NULL, dump_SLQSGetRegMgrConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSSMSConfig, "pack_ims_SLQSGetIMSSMSConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSSMSConfig, "unpack_ims_SLQSGetIMSSMSConfig",
        NULL, dump_SLQSGetIMSSMSConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSUserConfig, "pack_ims_SLQSGetIMSUserConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSUserConfig, "unpack_ims_SLQSGetIMSUserConfig",
        NULL, dump_SLQSGetIMSUserConfig
    },
    {
        (pack_func_item) &pack_ims_SLQSGetIMSVoIPConfig, "pack_ims_SLQSGetIMSVoIPConfig",
        NULL,
        (unpack_func_item) &unpack_ims_SLQSGetIMSVoIPConfig, "unpack_ims_SLQSGetIMSVoIPConfig",
        NULL, dump_SLQSGetIMSVoIPConfig
    },
};

void* ims_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    uint8_t settingResp;
    unpack_ims_SLQSSetSIPConfig_t resultSIP;
    unpack_ims_SLQSSetRegMgrConfig_t resultRegMgr;
    unpack_ims_SLQSSetIMSSMSConfig_t resultSms;
    unpack_ims_SLQSSetIMSUserConfig_t resultUser;
    unpack_ims_SLQSSetIMSVoIPConfig_t resultVoIP;
    unpack_ims_SLQSImsConfigIndicationRegister_t resultIndReg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enIMSThread)
    {
        rtn = read(ims, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eIMS, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("IMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("IMS RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
            settingResp = 0xFF;

            switch(rsp_ctx.msgid)
            {
                case eQMI_IMS_CONFIG_INDICATION_REGISTER:
                    rtn = unpack_ims_SLQSImsConfigIndicationRegister(msg.buf, QMI_MSG_MAX, &resultIndReg);
                    printf ( "QMI_IMS_CONFIG_INDICATION_REGISTER resp with result %d\r\n",rtn );
                    break;
                case eQMI_IMS_SETTINGS_SET_SIP_CONFIG:
                    resultSIP.pSettingResp = &settingResp;
                    rtn = unpack_ims_SLQSSetSIPConfig(msg.buf, QMI_MSG_MAX, &resultSIP);
                    printf ( "QMI_IMS_SETTINGS_SET_SIP_CONFIG resp with result %d \r\n",rtn );
                    if (rtn == eQCWWAN_ERR_NONE) {
                        if ((resultSIP.pSettingResp ) && (swi_uint256_get_bit (resultSIP.ParamPresenceMask, 16)) )
                            printf ( "setting response result 0x%x \r\n",*(resultSIP.pSettingResp));
                    }
                    break;
                case eQMI_IMS_SETTINGS_SET_REG_MGR_CFG:
                    resultRegMgr.pSettingResp = &settingResp;
                    rtn = unpack_ims_SLQSSetRegMgrConfig(msg.buf, QMI_MSG_MAX, &resultRegMgr);
                    printf ( "QMI_IMS_SETTINGS_SET_REG_MGR_CFG resp with result %d \r\n",rtn );
                    if (rtn == eQCWWAN_ERR_NONE) {
                        if ((resultRegMgr.pSettingResp) && (swi_uint256_get_bit (resultRegMgr.ParamPresenceMask, 16)) )
                            printf ( "setting response result 0x%x \r\n",*(resultRegMgr.pSettingResp));
                    }
                    break;
                case eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG:
                    resultSms.pSettingResp = &settingResp;
                    rtn = unpack_ims_SLQSSetIMSSMSConfig(msg.buf, QMI_MSG_MAX, &resultSms);
                    printf ( "QMI_IMS_SETTINGS_SET_IMS_SMS_CFG resp with result %d \r\n",rtn );
                    if (rtn == eQCWWAN_ERR_NONE) {
                        if ((resultSms.pSettingResp) && (swi_uint256_get_bit (resultSms.ParamPresenceMask, 16)) )
                            printf ( "setting response result 0x%x \r\n",*(resultSms.pSettingResp) );
                    }  
                    break;
               case eQMI_IMS_SETTINGS_SET_IMS_USER_CFG:
                    resultUser.pSettingResp = &settingResp;
                    rtn = unpack_ims_SLQSSetIMSUserConfig(msg.buf, QMI_MSG_MAX, &resultUser);
                    printf ( "QMI_IMS_SETTINGS_SET_IMS_USER_CFG resp with result %d \r\n",rtn );
                    if (rtn == eQCWWAN_ERR_NONE) {
                        if ((resultUser.pSettingResp) && (swi_uint256_get_bit (resultUser.ParamPresenceMask, 16)) )
                            printf ( "setting response result 0x%x \r\n",*(resultUser.pSettingResp) );
                    }  
                    break;
                case eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG:
                    resultVoIP.pSettingResp = &settingResp;
                    rtn = unpack_ims_SLQSSetIMSVoIPConfig(msg.buf, QMI_MSG_MAX, &resultVoIP);
                    printf ( "QMI_IMS_SETTINGS_SET_IMS_VOIP_CFG resp with result %d \r\n",rtn );
                    if (rtn == eQCWWAN_ERR_NONE) {
                        if ((resultVoIP.pSettingResp) && (swi_uint256_get_bit (resultVoIP.ParamPresenceMask, 16)) )
                            printf ( "setting response result 0x%x \r\n",*(resultVoIP.pSettingResp) );
                    } 
                    break;
                case eQMI_IMS_SIP_CONFIG_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_ims_SLQSSIPCfgCallBack_ind_t  sipInd;
                         unpack_ims_SLQSSIPCfgCallBack_ind(msg.buf,
                                                         QMI_MSG_MAX, 
                                                         &sipInd);
                         printf("unpack QMI_IMS_SIP_CONFIG_IND \n ");
                         swi_uint256_print_mask (sipInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 16))
                             printf("SIP port number: 0x%x\n",sipInd.SPTlv.SIPLocalPort);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 17))
                             printf("SIP registration timer: 0x%x\n",sipInd.SRTlv.tmrSIPRegn);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 18))
                             printf("Subscribe Timer 0x%x \n",sipInd.STTlv.subscrTmr);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 19))
                             printf("Timer T1 0x%x \n",sipInd.TT1Tlv.tmrT1);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 20))
                             printf("Timer T2 0x%x \n",sipInd.TT2Tlv.tmrT2);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 21))
                             printf("Timer Tf 0x%x \n",sipInd.TTfTlv.tmrTf);
                         if(swi_uint256_get_bit (sipInd.ParamPresenceMask, 22))
                             printf("SigComp Enabled 0x%x \n",sipInd.SCTlv.SigCompEn);
                     }
                     break;
                case eQMI_IMS_REG_MGR_CONFIG_IND:
                    if (rsp_ctx.type == eIND)
                     {
                         unpack_ims_SLQSRegMgrCfgCallBack_ind_t  regInd;
                         unpack_ims_SLQSRegMgrCfgCallBack_ind(msg.buf,
                                                         QMI_MSG_MAX, 
                                                         &regInd);
                         printf("unpack QMI_IMS_REG_MGR_CONFIG_IND \n");
                         swi_uint256_print_mask (regInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (regInd.ParamPresenceMask, 16))
                             printf("Primary CSCF port number: 0x%x\n",regInd.PCTlv.priCSCFPort);
                         if(swi_uint256_get_bit (regInd.ParamPresenceMask, 17))
                             printf("CSCF port name: %s\n",regInd.PNTlv.cscfPortName);
                         if(swi_uint256_get_bit (regInd.ParamPresenceMask, 18))
                             printf("IMS test mode: 0x%x\n",regInd.TMTlv.imsTestMode);
                     }
                     break;
                case eQMI_IMS_SMS_CONFIG_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_ims_SLQSSMSCfgCallBack_ind_t  smsInd;
                         unpack_ims_SLQSSMSCfgCallBack_ind(msg.buf,
                                                         QMI_MSG_MAX, 
                                                         &smsInd);
                         printf("unpack QMI_IMS_SMS_CONFIG_IND \n");
                         swi_uint256_print_mask (smsInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (smsInd.ParamPresenceMask, 16))
                             printf("SMS format: 0x%x\n",smsInd.SFTlv.smsFormat);
                         if(swi_uint256_get_bit (smsInd.ParamPresenceMask, 17))
                             printf("SMS over IP network indication flag: 0x%x \n",
                                    smsInd.SINTlv.smsoIPNW);
                         if(swi_uint256_get_bit (smsInd.ParamPresenceMask, 18))
                             printf("Phone Context URI: %s \n",
                                    smsInd.PCURTlv.PhCtxtURI);
                     }
                     break;
                case eQMI_IMS_USER_CONFIG_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_ims_SLQSUserCfgCallBack_ind_t  userInd;
                         unpack_ims_SLQSUserCfgCallBack_ind(msg.buf,
                                                         QMI_MSG_MAX, 
                                                         &userInd);
                         printf("unpack QMI_IMS_USER_CONFIG_IND \n");
                         swi_uint256_print_mask (userInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (userInd.ParamPresenceMask, 16))
                             printf("IMS domain name: %s \n",userInd.IDTlv.imsDomainName);
                     }
                     break;

                case eQMI_IMS_VOIP_CONFIG_IND:
                     if (rsp_ctx.type == eIND)
                     {
                         unpack_ims_SLQSVoIPCfgCallBack_ind_t  voipInd;
                         unpack_ims_SLQSVoIPCfgCallBack_ind(msg.buf,
                                                         QMI_MSG_MAX, 
                                                         &voipInd);
                         printf("unpack QMI_IMS_VOIP_CONFIG_IND\n");
                         swi_uint256_print_mask (voipInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 16))
                             printf("Session Expiry Timer: 0x%x \n",voipInd.SDTlv.sessExp);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 17))
                             printf("Minimum Session Expiry Timer: 0x%x \n",
                                    voipInd.MSETlv.minSessExp);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 18))
                             printf("AMR WB Enable: 0x%x \n",
                                    voipInd.EAWTlv.amrWBEnable);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 19))
                             printf("SCR AMR Enable: 0x%x \n",
                                    voipInd.ESATlv.scrAmrEnable);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 20))
                             printf("SCR AMR WB Enable: 0x%x \n",
                                    voipInd.ESAWTlv.scrAmrWBEnable);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 21))
                             printf("AMR Mode: 0x%x \n",
                                    voipInd.AMTlv.amrMode);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 22))
                             printf("AMR WB Mode: 0x%x \n",
                                    voipInd.AWMTlv.amrWBMode);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 23))
                             printf("AMR Octet Aligned: 0x%x \n",
                                    voipInd.AOATlv.amrOctAlgn);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 24))
                             printf("AMR WB Octet Aligned: 0x%x \n",
                                    voipInd.AWOATlv.amrWBOctAlgn);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 25))
                             printf("Ringing Timer: 0x%x \n",
                                    voipInd.RTTlv.RingTmr);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 26))
                             printf("Ringback Timer: 0x%x \n",
                                    voipInd.RBTTlv.RingBkTmr);
                         if(swi_uint256_get_bit (voipInd.ParamPresenceMask, 27))
                             printf("RTP/RTCP Inactivity Timer: 0x%x \n",
                                    voipInd.RTIDTlv.InactTmr);
                     }
                     break;

                default:
                    printf("Default msgid 0x%x\n", rsp_ctx.msgid);
                    break;
            }
        }
        else
        {
           enIMSThread = 0;
           ims = -1;
           break;
        }
    }
    return NULL;
}

void ims_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======IMS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    const char *qmi_msg;
    for(i=0; i<sizeof(imstotest)/sizeof(testitem_t); i++)
    {
        if(imstotest[i].dump!=NULL)
        {
            imstotest[i].dump(NULL); 
        }
    }
    for(i=0; i<sizeof(imstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(imstotest_invalidunpack[i].dump!=NULL)
        {
            imstotest_invalidunpack[i].dump(NULL); 
        }
    }
    for(i=0; i<sizeof(imstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(imstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,imstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(ims<0)
            ims = client_fd(eIMS);
        if(ims<0)
        {
            fprintf(stderr,"IMS Service Not Supported!\n");
            return ;
        }
        rtn = write(ims, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(ims>=0)
                close(ims);
            ims=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eIMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(ims, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eIMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(imstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                imstotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           imstotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    imstotest_invalidunpack[i].dump(imstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(ims>=0)
                close(ims);
            ims=-1;
        }
        sleep(1);
        xid++;
    }
    if(ims>=0)
        close(ims);
    ims=-1;
}

void ims_test_pack_unpack_loop()
{
    unsigned i;
    printf("======IMS pack/unpack test===========\n");

    printf("======IMS dummy unpack test===========\n");
    ims_validate_dummy_unpack();
    printf("======IMS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s\n", "IMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing");
#endif

    unsigned xid =1;
    for(i=0; i<sizeof(imstotest)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(imstotest[i].pack)(&req_ctx, req, &reqLen,imstotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", imstotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(ims<0)
            ims = client_fd(eIMS);
        if(ims<0)
        {
            fprintf(stderr,"IMS Service Not Supported!\n");
            return ;
        }
        rtn = write(ims, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eIMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(ims, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eIMS, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("IMS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        rtn = run_unpack_item(imstotest[i].unpack)(rsp, rspLen, imstotest[i].unpack_ptr);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", imstotest[i].unpack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                    #endif

                        imstotest[i].dump(imstotest[i].unpack_ptr);
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
                    printf("IMS IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(ims>=0)
        close(ims);
    ims=-1;
}

void ims_test_ind()
{
    pack_qmi_t req_ctx;
    uint16_t qmi_req_len;
    uint8_t qmi_req[QMI_MSG_MAX];
    int rtn;
    const char *qmi_msg;

    if(ims<0)
        ims = client_fd(eIMS);
    if(ims<0)
    {
        fprintf(stderr,"IMS Service Not Supported\n");
        return ;
    }

    enIMSThread = 1;
    pthread_create(&ims_tid, &ims_attr, ims_read_thread, NULL);
    sleep(1);

    /* register IMS indications */
    req_ctx.xid = 0x100;
    rtn = pack_ims_SLQSImsConfigIndicationRegister(&req_ctx, 
                                                qmi_req, &qmi_req_len,
                                                (void*)&tpack_ims_SLQSImsConfigIndicationRegister);
    if(rtn!=0)
    {
        printf("Set IMS indication register fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* set SIP config */
    req_ctx.xid = 0x101;
    rtn = pack_ims_SLQSSetSIPConfig(&req_ctx, 
                                    qmi_req, &qmi_req_len,
                                    (void*)&tpack_ims_SLQSSetSIPConfig);
    if(rtn!=0)
    {
        printf("Set SIP config fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* set registration manager config */
    req_ctx.xid = 0x102;
    rtn = pack_ims_SLQSSetRegMgrConfig(&req_ctx, 
                                    qmi_req, &qmi_req_len,
                                    (void*)&tpack_ims_SLQSSetRegMgrConfig);
    if(rtn!=0)
    {
        printf("Set registration manager config fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* set IMS SMS config */
    req_ctx.xid = 0x103;
    rtn = pack_ims_SLQSSetIMSSMSConfig(&req_ctx, 
                                    qmi_req, &qmi_req_len,
                                    (void*)&tpack_ims_SLQSSetIMSSMSConfig);
    if(rtn!=0)
    {
        printf("Set IMS SMS config fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* set IMS user config */
    req_ctx.xid = 0x104;
    rtn = pack_ims_SLQSSetIMSUserConfig(&req_ctx, 
                                    qmi_req, &qmi_req_len,
                                    (void*)&tpack_ims_SLQSSetIMSUserConfig);
    if(rtn!=0)
    {
        printf("Set IMS user config fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    /* set IMS voice config */
    req_ctx.xid = 0x105;
    rtn = pack_ims_SLQSSetIMSVoIPConfig(&req_ctx, 
                                    qmi_req, &qmi_req_len,
                                    (void*)&tpack_ims_SLQSSetIMSVoIPConfig);
    if(rtn!=0)
    {
        printf("Set IMS voip config fail");
        return ;
    }
    rtn = write(ims, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("IMS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eIMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(2);
}

void ims_test_ind_exit()
{
    void *pthread_rtn_value;
    printf("\nkilling ims read thread...\n");
    enIMSThread = 0;
    UNUSEDPARAM(pthread_rtn_value);
#ifdef __ARM_EABI__    
    if(ims>=0)
           close(ims);
    ims = -1;
    if(ims_tid!=0)
        pthread_join(ims_tid, &pthread_rtn_value);
    ims_tid = 0;
#endif
    if(ims_tid!=0)
        pthread_cancel(ims_tid);
    ims_tid = 0;
    if(ims>=0)
        close(ims);
    ims=-1;
}

void dump_SLQSRegMgrCfgCallBack(void *ptr)
{
    unpack_ims_SLQSRegMgrCfgCallBack_ind_t *result =
            (unpack_ims_SLQSRegMgrCfgCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("Primary CSCF port number: 0x%x\n",result->PCTlv.priCSCFPort);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("CSCF port name: %s\n",result->PNTlv.cscfPortName);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("IMS test mode: 0x%x\n",result->TMTlv.imsTestMode);
}

void dump_SLQSSIPCfgCallBack(void *ptr)
{
    unpack_ims_SLQSSIPCfgCallBack_ind_t *result =
            (unpack_ims_SLQSSIPCfgCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("SIP port number: 0x%x\n",result->SPTlv.SIPLocalPort);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("SIP registration timer: 0x%x\n",result->SRTlv.tmrSIPRegn);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("Subscribe Timer 0x%x \n",result->STTlv.subscrTmr);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        printf("Timer T1 0x%x \n",result->TT1Tlv.tmrT1);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        printf("Timer T2 0x%x \n",result->TT2Tlv.tmrT2);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("Timer Tf 0x%x \n",result->TTfTlv.tmrTf);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        printf("SigComp Enabled 0x%x \n",result->SCTlv.SigCompEn);
}
void dump_SLQSSMSCfgCallBack(void *ptr)
{
    unpack_ims_SLQSSMSCfgCallBack_ind_t *result =
            (unpack_ims_SLQSSMSCfgCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("SMS format: 0x%x\n",result->SFTlv.smsFormat);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("SMS over IP network indication flag: 0x%x \n",
               result->SINTlv.smsoIPNW);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("Phone Context URI: %s \n",
               result->PCURTlv.PhCtxtURI);
}

void dump_SLQSUserCfgCallBack(void *ptr)
{
    unpack_ims_SLQSUserCfgCallBack_ind_t *result =
            (unpack_ims_SLQSUserCfgCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("IMS domain name: %s \n",result->IDTlv.imsDomainName);
}

void dump_SLQSVoIPCfgCallBack(void *ptr)
{
    unpack_ims_SLQSVoIPCfgCallBack_ind_t *result =
            (unpack_ims_SLQSVoIPCfgCallBack_ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf("Session Expiry Timer: 0x%x \n",result->SDTlv.sessExp);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf("Minimum Session Expiry Timer: 0x%x \n",
               result->MSETlv.minSessExp);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf("AMR WB Enable: 0x%x \n",
               result->EAWTlv.amrWBEnable);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        printf("SCR AMR Enable: 0x%x \n",
               result->ESATlv.scrAmrEnable);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        printf("SCR AMR WB Enable: 0x%x \n",
               result->ESAWTlv.scrAmrWBEnable);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        printf("AMR Mode: 0x%x \n",
               result->AMTlv.amrMode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        printf("AMR WB Mode: 0x%x \n",
               result->AWMTlv.amrWBMode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        printf("AMR Octet Aligned: 0x%x \n",
               result->AOATlv.amrOctAlgn);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
        printf("AMR WB Octet Aligned: 0x%x \n",
               result->AWOATlv.amrWBOctAlgn);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
        printf("Ringing Timer: 0x%x \n",
               result->RTTlv.RingTmr);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
        printf("Ringback Timer: 0x%x \n",
               result->RBTTlv.RingBkTmr);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 27))
        printf("RTP/RTCP Inactivity Timer: 0x%x \n",
               result->RTIDTlv.InactTmr);
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_ims_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_IMS_SETTINGS_SET_SIP_CONFIG */
    {0x02,0x01,0x00,0x20,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x01 },

    /* eQMI_IMS_SETTINGS_SET_REG_MGR_CFG */
    {0x02,0x02,0x00,0x21,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x02 },

    /* eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG */
    {0x02,0x03,0x00,0x22,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x03 },

    /* eQMI_IMS_SETTINGS_SET_IMS_USER_CFG */
    {0x02,0x04,0x00,0x23,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x04 },

    /* eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG */
    {0x02,0x05,0x00,0x24,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x05 },

    /* eQMI_IMS_SETTINGS_GET_SIP_CONFIG */
    {0x02,0x06,0x00,0x25,0x00,0x37,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x02,0x00,0x02,0x01,
            0x12,0x04,0x00,0x00,0x00,0x34,0x12,
            0x13,0x04,0x00,0x00,0x00,0x78,0x56,
            0x14,0x04,0x00,0x00,0x56,0x34,0x12,
            0x15,0x04,0x00,0x78,0x56,0x34,0x12,
            0x16,0x04,0x00,0x12,0x34,0x56,0x78,
            0x17,0x01,0x00,0x01},

    /* eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG */
    {0x02,0x07,0x00,0x26,0x00,0x2A,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x02,0x00,0x02,0x01,
            0x12,0x13,0x00,'s','i','e','r','r','a',
            'w','i','r','e','l','e','s','s','.','c','o','m','\0',
            0x13,0x01,0x00,0x01},

    /* eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG */
    {0x02,0x08,0x00,0x27,0x00,0x24,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x01,0x00,0x01,
            0x12,0x01,0x00,0x01,
            0x13,0x0E,0x00,'+','9','1','8','4','2',
            '7','2','0','5','8','7','0','\0',},

    /* eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG */
    {0x02,0x09,0x00,0x28,0x00,0x1C,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x0E,0x00,'g','d','a','w','r','a',
            '@','s','i','e','r','r','a','\0',},

    /* eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG */
    {0x02,0x0A,0x00,0x29,0x00,0x41,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x01,0x00,0x00,
            0x11,0x02,0x00,0x34, 0x12,
            0x12,0x02,0x00,0x56, 0x00,
            0x13,0x01,0x00,0x01,
            0x14,0x01,0x00,0x01,
            0x15,0x01,0x00,0x01,
            0x16,0x01,0x00,0x80,
            0x17,0x02,0x00,0x00, 0x01,
            0x18,0x01,0x00,0x00,
            0x19,0x01,0x00,0x00,
            0x1A,0x02,0x00,0x22,0x11,
            0x1B,0x02,0x00,0x22,0x33,
            0x1C,0x02,0x00,0x44,0x55},

    /* eQMI_IMS_CONFIG_INDICATION_REGISTER */
    {0x02,0x0B,0x00,0x2A,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00 },

    /* eQMI_IMS_REG_MGR_CONFIG_IND */
    {0x04,0x0C,0x00,0x2C,0x00,0x1F,0x00,
            0x10,0x02,0x00,0x80,0x80,
            0x11,0x13,0x00,'s','i','e','r','r','a',
            'w','i','r','e','l','e','s','s','.','c','o','m','\0',
            0x12,0x01,0x00,0x01},

    /* eQMI_IMS_SIP_CONFIG_IND */
    {0x04,0x0D,0x00,0x2B,0x00,0x2C,0x00,
            0x10,0x02,0x00,0x80,0x80,
            0x11,0x04,0x00,0x34,0x12,0x00,0x00,
            0x12,0x04,0x00,0x78,0x56,0x00,0x00,
            0x13,0x04,0x00,0x78,0x56,0x34,0x12,
            0x14,0x04,0x00,0x22,0x11,0x00,0x00,
            0x15,0x04,0x00,0x44,0x33,0x00,0x00,
            0x16,0x01,0x00,0x01},

    /* eQMI_IMS_SMS_CONFIG_IND */
    {0x04,0x0E,0x00,0x2D,0x00,0x16,0x00,
            0x10,0x01,0x00,0x01,
            0x11,0x01,0x00,0x01,
            0x12,0x0B,0x00,'8','4','2','7','2','0','1','2','3','4','\0'},

    /* eQMI_IMS_USER_CONFIG_IND */
    {0x04,0x0F,0x00,0x2E,0x00,0x1A,0x00,
            0x10,0x17,0x00,'i','m','s','.','s','i','e','r','r','a',
                        'w','i','r','e','l','e','s','s','.','c','o','m','\0'},

    /* eQMI_IMS_VOIP_CONFIG_IND */
    {0x04,0x10,0x00,0x2F,0x00,0x36,0x00,
            0x10,0x02,0x00,0x34,0x12,
            0x11,0x02,0x00,0x22,0x11,
            0x12,0x01,0x00,0x01,
            0x13,0x01,0x00,0x01,
            0x14,0x01,0x00,0x00,
            0x15,0x01,0x00,0x80,
            0x16,0x02,0x00,0x00,0x01,
            0x17,0x01,0x00,0x01,
            0x18,0x01,0x00,0x01,
            0x19,0x02,0x00,0x01,0x10,
            0x1A,0x02,0x00,0x33,0x11,
            0x1B,0x02,0x00,0x12,0x34},
};

/* eQMI_IMS_SETTINGS_SET_SIP_CONFIG */
uint8_t    cst_settingResp = 0x01;
const unpack_ims_SLQSSetSIPConfig_t const_unpack_ims_SLQSSetSIPConfig_t = {
        &cst_settingResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t    var_settingResp;
unpack_ims_SLQSSetSIPConfig_t var_unpack_ims_SLQSSetSIPConfig_t = {
        &var_settingResp,{{0}} };

/* eQMI_IMS_SETTINGS_SET_REG_MGR_CFG */
uint8_t    cst_reqMgrSetResp = 0x02;
const unpack_ims_SLQSSetRegMgrConfig_t const_unpack_ims_SLQSSetRegMgrConfig_t = {
        &cst_reqMgrSetResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t    var_reqMgrSetResp;
unpack_ims_SLQSSetRegMgrConfig_t var_unpack_ims_SLQSSetRegMgrConfig_t = {
        &var_reqMgrSetResp,{{0}} };

/* eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG */
uint8_t    cst_smsCfgSetResp = 0x03;
const unpack_ims_SLQSSetIMSSMSConfig_t const_unpack_ims_SLQSSetIMSSMSConfig_t = {
        &cst_smsCfgSetResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t    var_smsCfgSetResp;
unpack_ims_SLQSSetIMSSMSConfig_t var_unpack_ims_SLQSSetIMSSMSConfig_t = {
        &var_smsCfgSetResp,{{0}} };

/* eQMI_IMS_SETTINGS_SET_IMS_USER_CFG */
uint8_t    cst_usrCfgSetResp = 0x04;
const unpack_ims_SLQSSetIMSUserConfig_t const_unpack_ims_SLQSSetIMSUserConfig_t = {
        &cst_usrCfgSetResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t    var_usrCfgSetResp;
unpack_ims_SLQSSetIMSUserConfig_t var_unpack_ims_SLQSSetIMSUserConfig_t = {
        &var_usrCfgSetResp,{{0}} };

/* eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG */
uint8_t    cst_voipCfgSetResp = 0x05;
const unpack_ims_SLQSSetIMSVoIPConfig_t const_unpack_ims_SLQSSetIMSVoIPConfig_t = {
        &cst_voipCfgSetResp,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint8_t    var_voipCfgSetResp;
unpack_ims_SLQSSetIMSVoIPConfig_t var_unpack_ims_SLQSSetIMSVoIPConfig_t = {
        &var_voipCfgSetResp,{{0}} };

/* eQMI_IMS_SETTINGS_GET_SIP_CONFIG */
uint8_t   cst_SettingResp = 0;
uint16_t  cst_SIPLocalPort = 0x0102;
uint32_t  cst_TimerSIPReg = 0x12340000;
uint32_t  cst_SubscribeTimer = 0x56780000;
uint32_t  cst_SipTimerT1 = 0x12345600;
uint32_t  cst_SipTimerT2 = 0x12345678;
uint32_t  cst_SipTimerTf = 0x78563412;
uint8_t   cst_SipSigCompEnabled = 1;
const unpack_ims_SLQSGetSIPConfig_t const_unpack_ims_SLQSGetSIPConfig_t = {
        &cst_SettingResp,&cst_SIPLocalPort, &cst_TimerSIPReg,&cst_SubscribeTimer,
        &cst_SipTimerT1,&cst_SipTimerT2,&cst_SipTimerTf,&cst_SipSigCompEnabled,
        {{SWI_UINT256_BIT_VALUE(SET_9_BITS,2,16,17,18,19,20,21,22,23)}} };

uint8_t   var_SettingResp;
uint16_t  var_SIPLocalPort;
uint32_t  var_TimerSIPReg;
uint32_t  var_SubscribeTimer;
uint32_t  var_SipTimerT1;
uint32_t  var_SipTimerT2;
uint32_t  var_SipTimerTf;
uint8_t   var_SipSigCompEnabled;
unpack_ims_SLQSGetSIPConfig_t var_unpack_ims_SLQSGetSIPConfig_t = {
        &var_SettingResp,&var_SIPLocalPort, &var_TimerSIPReg,&var_SubscribeTimer,
        &var_SipTimerT1,&var_SipTimerT2,&var_SipTimerTf,&var_SipSigCompEnabled, {{0}} };

/* eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG */
uint8_t  cst_RegMgrSettingResp = 0;
uint16_t cst_PCSCFPort = 0x0102;
uint8_t  cst_PriCSCFPortNameLen = 19;
uint8_t  cst_PriCSCFPortName[] = "sierrawireless.com";
uint8_t  cst_IMSTestMode = 1;
const unpack_ims_SLQSGetRegMgrConfig_t const_unpack_ims_SLQSGetRegMgrConfig_t = {
        &cst_RegMgrSettingResp,&cst_PCSCFPort, &cst_PriCSCFPortNameLen,cst_PriCSCFPortName,
        &cst_IMSTestMode,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

uint8_t  var_RegMgrSettingResp;
uint16_t var_PCSCFPort;
uint8_t  var_PriCSCFPortNameLen;
uint8_t  var_PriCSCFPortName[19];
uint8_t  var_IMSTestMode;
unpack_ims_SLQSGetRegMgrConfig_t var_unpack_ims_SLQSGetRegMgrConfig_t = {
        &var_RegMgrSettingResp,&var_PCSCFPort, &var_PriCSCFPortNameLen,var_PriCSCFPortName,
        &var_IMSTestMode,{{0}} };

/* eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG */
uint8_t  cst_smsCfgSettingResp = 0;
uint8_t  cst_SMSFormat = 1;
uint8_t  cst_SMSOverIPNwInd = 1;
uint8_t  cst_PhoneCtxtURILen = 14;
uint8_t  cst_PhoneCtxtURI[] = "+918427205870";
const unpack_ims_SLQSGetIMSSMSConfig_t const_unpack_ims_SLQSGetIMSSMSConfig_t = {
        &cst_smsCfgSettingResp, &cst_SMSFormat, &cst_SMSOverIPNwInd,&cst_PhoneCtxtURILen,
        cst_PhoneCtxtURI,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

uint8_t  var_smsCfgSettingResp;
uint8_t  var_SMSFormat;
uint8_t  var_SMSOverIPNwInd;
uint8_t  var_PhoneCtxtURILen;
uint8_t  var_PhoneCtxtURI[14];
unpack_ims_SLQSGetIMSSMSConfig_t var_unpack_ims_SLQSGetIMSSMSConfig_t = {
        &var_smsCfgSettingResp,&var_SMSFormat, &var_SMSOverIPNwInd,&var_PhoneCtxtURILen,
        var_PhoneCtxtURI,{{0}} };

/* eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG */
uint8_t  cst_usrCfgSettingResp = 0;
uint8_t  cst_IMSDomainLen = 14;
uint8_t  cst_IMSDomain[] = "gdawra@sierra";
const unpack_ims_SLQSGetIMSUserConfig_t const_unpack_ims_SLQSGetIMSUserConfig_t = {
        &cst_usrCfgSettingResp, &cst_IMSDomainLen, cst_IMSDomain,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

uint8_t  var_usrCfgSettingResp;
uint8_t  var_IMSDomainLen;
uint8_t  var_IMSDomain[14];
unpack_ims_SLQSGetIMSUserConfig_t var_unpack_ims_SLQSGetIMSUserConfig_t = {
        &cst_usrCfgSettingResp, &cst_IMSDomainLen, cst_IMSDomain,
        {{0}} };

/* eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG */
uint8_t  cst_voipSettingResp = 0;
uint16_t cst_SessionExpiryTimer = 0x1234;
uint16_t cst_MinSessionExpiryTimer = 0x0056;
uint8_t  cst_AmrWbEnable = 1;
uint8_t  cst_ScrAmrEnable =1 ;
uint8_t  cst_ScrAmrWbEnable = 1;
uint8_t  cst_AmrMode = 0x80;
uint16_t cst_AmrWBMode = 0x0100;
uint8_t  cst_AmrOctetAligned = 0;
uint8_t  cst_AmrWBOctetAligned = 0;
uint16_t cst_RingingTimer = 0x1122;
uint16_t cst_RingBackTimer = 0x3322;
uint16_t cst_RTPRTCPInactTimer = 0x5544;

const unpack_ims_SLQSGetIMSVoIPConfig_t const_unpack_ims_SLQSGetIMSVoIPConfig_t = {
        &cst_voipSettingResp,&cst_SessionExpiryTimer, &cst_MinSessionExpiryTimer,
        &cst_AmrWbEnable,&cst_ScrAmrEnable,&cst_ScrAmrWbEnable,&cst_AmrMode,
        &cst_AmrWBMode,&cst_AmrOctetAligned,&cst_AmrWBOctetAligned,&cst_RingingTimer,
        &cst_RingBackTimer,&cst_RTPRTCPInactTimer,
        {{SWI_UINT256_BIT_VALUE(SET_14_BITS,2,16,17,18,19,20,21,22,23,24,25,26,27,28)}} };

uint8_t  var_voipSettingResp;
uint16_t var_SessionExpiryTimer;
uint16_t var_MinSessionExpiryTimer;
uint8_t  var_AmrWbEnable;
uint8_t  var_ScrAmrEnable;
uint8_t  var_ScrAmrWbEnable;
uint8_t  var_AmrMode;
uint16_t var_AmrWBMode;
uint8_t  var_AmrOctetAligned;
uint8_t  var_AmrWBOctetAligned;
uint16_t var_RingingTimer;
uint16_t var_RingBackTimer;
uint16_t var_RTPRTCPInactTimer;

unpack_ims_SLQSGetIMSVoIPConfig_t var_unpack_ims_SLQSGetIMSVoIPConfig_t = {
        &var_voipSettingResp,&var_SessionExpiryTimer, &var_MinSessionExpiryTimer,
        &var_AmrWbEnable,&var_ScrAmrEnable,&var_ScrAmrWbEnable,&var_AmrMode,
        &var_AmrWBMode,&var_AmrOctetAligned,&var_AmrWBOctetAligned,&var_RingingTimer,
        &var_RingBackTimer,&var_RTPRTCPInactTimer,
        {{0}} };

/* eQMI_IMS_CONFIG_INDICATION_REGISTER */
const unpack_ims_SLQSImsConfigIndicationRegister_t const_unpack_ims_SLQSImsConfigIndicationRegister_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_IMS_REG_MGR_CONFIG_IND */
const unpack_ims_SLQSRegMgrCfgCallBack_ind_t const_unpack_ims_SLQSRegMgrCfgCallBack_ind_t = {
        {1,0x8080},{1,"sierrawireless.com"},{1,1}, {{SWI_UINT256_BIT_VALUE(SET_3_BITS,16,17,18)}} };

/* eQMI_IMS_SIP_CONFIG_IND */
const unpack_ims_SLQSSIPCfgCallBack_ind_t const_unpack_ims_SLQSSIPCfgCallBack_ind_t = {
        {1,0x8080},{1,0x1234},{1,0x5678},{1,0x12345678},{1,0x1122},{1,0x3344},{1,1},
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,16,17,18,19,20,21,22)}} };

/* eQMI_IMS_SMS_CONFIG_IND */
const unpack_ims_SLQSSMSCfgCallBack_ind_t const_unpack_ims_SLQSSMSCfgCallBack_ind_t = {
        {1,1},{1,1},{1,"8427201234"},{{SWI_UINT256_BIT_VALUE(SET_3_BITS,16,17,18)}} };

/* eQMI_IMS_USER_CONFIG_IND */
const unpack_ims_SLQSUserCfgCallBack_ind_t const_unpack_ims_SLQSUserCfgCallBack_ind_t = {
        {1,"ims.sierrawireless.com"},{{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

/* eQMI_IMS_VOIP_CONFIG_IND */
const unpack_ims_SLQSVoIPCfgCallBack_ind_t const_unpack_ims_SLQSVoIPCfgCallBack_ind_t = {
        {1,0x1234},{1,0x1122},{1,1},{1,1},{1,0},{1,0x80},
        {1,0x100},{1,1},{1,1},{1,0x1001},{1,0x1133},{1,0x3412},
        {{SWI_UINT256_BIT_VALUE(SET_12_BITS,16,17,18,19,20,21,22,23,24,25,26,27)}} };

int ims_validate_dummy_unpack()
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
    loopCount = sizeof(validate_ims_resp_msg)/sizeof(validate_ims_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_ims_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eIMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("IMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("IMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_IMS_SETTINGS_SET_SIP_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSSetSIPConfig_t *varp = &var_unpack_ims_SLQSSetSIPConfig_t;
                    const unpack_ims_SLQSSetSIPConfig_t *cstp = &const_unpack_ims_SLQSSetSIPConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSetSIPConfig,
                    dump_SLQSSetSIPConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSSetSIPConfig_t,
                    2,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_SET_REG_MGR_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSSetRegMgrConfig_t *varp = &var_unpack_ims_SLQSSetRegMgrConfig_t;
                    const unpack_ims_SLQSSetRegMgrConfig_t *cstp = &const_unpack_ims_SLQSSetRegMgrConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSetRegMgrConfig,
                    dump_SLQSSetRegMgrConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSSetRegMgrConfig_t,
                    2,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSSetIMSSMSConfig_t *varp = &var_unpack_ims_SLQSSetIMSSMSConfig_t;
                    const unpack_ims_SLQSSetIMSSMSConfig_t *cstp = &const_unpack_ims_SLQSSetIMSSMSConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSetIMSSMSConfig,
                    dump_SLQSSetIMSSMSConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSSetIMSSMSConfig_t,
                    2,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_SET_IMS_USER_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSSetIMSUserConfig_t *varp = &var_unpack_ims_SLQSSetIMSUserConfig_t;
                    const unpack_ims_SLQSSetIMSUserConfig_t *cstp = &const_unpack_ims_SLQSSetIMSUserConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSetIMSUserConfig,
                    dump_SLQSSetIMSUserConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSSetIMSUserConfig_t,
                    2,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSSetIMSVoIPConfig_t *varp = &var_unpack_ims_SLQSSetIMSVoIPConfig_t;
                    const unpack_ims_SLQSSetIMSVoIPConfig_t *cstp = &const_unpack_ims_SLQSSetIMSVoIPConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSetIMSVoIPConfig,
                    dump_SLQSSetIMSVoIPConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSSetIMSVoIPConfig_t,
                    2,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_GET_SIP_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSGetSIPConfig_t *varp = &var_unpack_ims_SLQSGetSIPConfig_t;
                    const unpack_ims_SLQSGetSIPConfig_t *cstp = &const_unpack_ims_SLQSGetSIPConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSGetSIPConfig,
                    dump_SLQSGetSIPConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSGetSIPConfig_t,
                    9,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, varp->pSIPLocalPort, cstp->pSIPLocalPort,
                    CMP_PTR_TYPE, varp->pTimerSIPReg, cstp->pTimerSIPReg,
                    CMP_PTR_TYPE, varp->pSubscribeTimer, cstp->pSubscribeTimer,
                    CMP_PTR_TYPE, varp->pTimerT1, cstp->pTimerT1,
                    CMP_PTR_TYPE, varp->pTimerT2, cstp->pTimerT2,
                    CMP_PTR_TYPE, varp->pTimerTf, cstp->pTimerTf,
                    CMP_PTR_TYPE, varp->pSigCompEnabled, cstp->pSigCompEnabled,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSGetRegMgrConfig_t *varp = &var_unpack_ims_SLQSGetRegMgrConfig_t;
                    const unpack_ims_SLQSGetRegMgrConfig_t *cstp = &const_unpack_ims_SLQSGetRegMgrConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSGetRegMgrConfig,
                    dump_SLQSGetRegMgrConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSGetRegMgrConfig_t,
                    5,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, varp->pPCSCFPort, cstp->pPCSCFPort,
                    CMP_PTR_TYPE, varp->pPriCSCFPortName, cstp->pPriCSCFPortName,
                    CMP_PTR_TYPE, varp->pIMSTestMode, cstp->pIMSTestMode,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSGetIMSSMSConfig_t *varp = &var_unpack_ims_SLQSGetIMSSMSConfig_t;
                    const unpack_ims_SLQSGetIMSSMSConfig_t *cstp = &const_unpack_ims_SLQSGetIMSSMSConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSGetIMSSMSConfig,
                    dump_SLQSGetIMSSMSConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSGetIMSSMSConfig_t,
                    5,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, varp->pSMSFormat, cstp->pSMSFormat,
                    CMP_PTR_TYPE, varp->pSMSOverIPNwInd, cstp->pSMSOverIPNwInd,
                    CMP_PTR_TYPE, varp->pPhoneCtxtURI, cstp->pPhoneCtxtURI,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSGetIMSUserConfig_t *varp = &var_unpack_ims_SLQSGetIMSUserConfig_t;
                    const unpack_ims_SLQSGetIMSUserConfig_t *cstp = &const_unpack_ims_SLQSGetIMSUserConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSGetIMSUserConfig,
                    dump_SLQSGetIMSUserConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSGetIMSUserConfig_t,
                    3,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, varp->pIMSDomain, cstp->pIMSDomain,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_ims_SLQSGetIMSVoIPConfig_t *varp = &var_unpack_ims_SLQSGetIMSVoIPConfig_t;
                    const unpack_ims_SLQSGetIMSVoIPConfig_t *cstp = &const_unpack_ims_SLQSGetIMSVoIPConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSGetIMSVoIPConfig,
                    dump_SLQSGetIMSVoIPConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_ims_SLQSGetIMSVoIPConfig_t,
                    14,
                    CMP_PTR_TYPE, varp->pSettingResp, cstp->pSettingResp,
                    CMP_PTR_TYPE, varp->pSessionExpiryTimer, cstp->pSessionExpiryTimer,
                    CMP_PTR_TYPE, varp->pMinSessionExpiryTimer, cstp->pMinSessionExpiryTimer,
                    CMP_PTR_TYPE, varp->pAmrWbEnable, cstp->pAmrWbEnable,
                    CMP_PTR_TYPE, varp->pScrAmrEnable, cstp->pScrAmrEnable,
                    CMP_PTR_TYPE, varp->pScrAmrWbEnable, cstp->pScrAmrWbEnable,
                    CMP_PTR_TYPE, varp->pAmrMode, cstp->pAmrMode,
                    CMP_PTR_TYPE, varp->pAmrWBMode, cstp->pAmrWBMode,
                    CMP_PTR_TYPE, varp->pAmrOctetAligned, cstp->pAmrOctetAligned,
                    CMP_PTR_TYPE, varp->pAmrWBOctetAligned, cstp->pAmrWBOctetAligned,
                    CMP_PTR_TYPE, varp->pRingingTimer, cstp->pRingingTimer,
                    CMP_PTR_TYPE, varp->pRingBackTimer, cstp->pRingBackTimer,
                    CMP_PTR_TYPE, varp->pRTPRTCPInactTimer, cstp->pRTPRTCPInactTimer,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_IMS_CONFIG_INDICATION_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSImsConfigIndicationRegister,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSImsConfigIndicationRegister_t);
                }
                break;
            case eQMI_IMS_REG_MGR_CONFIG_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSRegMgrCfgCallBack_ind,
                    dump_SLQSRegMgrCfgCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSRegMgrCfgCallBack_ind_t);
                }
                break;
            case eQMI_IMS_SIP_CONFIG_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSIPCfgCallBack_ind,
                    dump_SLQSSIPCfgCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSSIPCfgCallBack_ind_t);
                }
                break;
            case eQMI_IMS_SMS_CONFIG_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSSMSCfgCallBack_ind,
                    dump_SLQSSMSCfgCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSSMSCfgCallBack_ind_t);
                }
                break;
            case eQMI_IMS_USER_CONFIG_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSUserCfgCallBack_ind,
                    dump_SLQSUserCfgCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSUserCfgCallBack_ind_t);
                }
                break;
            case eQMI_IMS_VOIP_CONFIG_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_ims_SLQSVoIPCfgCallBack_ind,
                    dump_SLQSVoIPCfgCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_ims_SLQSVoIPCfgCallBack_ind_t);
                }
                break;
            }
        }
    }
    return 0;
}
