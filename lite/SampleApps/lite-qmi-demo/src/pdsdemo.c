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
#include "pds.h"

int pds = -1;
volatile int enPDSThread =0;
static char remark[255]={0};
static int unpackRetCode = 0;
pthread_t pds_tid = 0;
pthread_attr_t pds_attr;
static int iLocalLog = 1;

int pds_validate_dummy_unpack();


////Modem Settings
pack_pds_SetPDSState_t DefaultPDSStateSettings = {
        0 //GPS service state disable
};
uint8_t  DefaultserverUrl[512]     = {0};
uint32_t DefaultserverAddress   = 0x12345678;
uint32_t Defaultserverport      = 5001;
uint8_t  DefaultserverUrlLength = 11;
uint8_t  DefaultnetworkMode     = 0x00;
pack_pds_SLQSSetAGPSConfig_t DefaultAGPSConfigSettings = {&DefaultserverAddress, &Defaultserverport,
        DefaultserverUrl, &DefaultserverUrlLength, &DefaultnetworkMode};
pack_pds_SetPortAutomaticTracking_t DefaultPortAutomaticTrackingSettings = {0x01}; //Enable
pack_pds_SetServiceAutomaticTracking_t DefaultServiceAutomaticTrackingSettings = {1};
pack_pds_SetXTRANetwork_t DefaultXTRANetworkSettings = {1};
pack_pds_SetXTRAAutomaticDownload_t DefaultXTRAAutomaticDownloadSettings = {1, 1};
//////////////////////////////////

uint32_t enabledStatus = 0xFF;
uint32_t trackingStatus = 0xFF ;
unpack_pds_GetPDSState_t tunpack_pds_GetPDSState_t = {&enabledStatus, &trackingStatus, {{0}} };

pack_pds_SetPDSState_t tpack_pds_SetPDSState_enable = {
        1 //GPS service state enable
    };

pack_pds_SetPDSState_t tpack_pds_SetPDSState_disable = {
        0 //GPS service state disable
    };

unpack_pds_SetPDSState_t tunpack_pds_SetPDSState = { 0, {{0}} };

uint32_t operation = 0xFF;
uint8_t  maxtimeout = 0xFF;
uint32_t interval = 0xFFFFFFFF;
uint32_t accuracy = 0xFFFFFFFF;
unpack_pds_GetPDSDefaults_t tunpack_pds_GetPDSDefaults_t = {&operation, &maxtimeout, &interval, &accuracy, {{0}}};

pack_pds_SetPDSDefaults_t default_SetPDSDefaults =  {0x00, 255, 1, 10};

pack_pds_SetPDSDefaults_t tpack_pds_SetPDSDefaults_t = {0x00, 255, 1, 10};
unpack_pds_SetPDSDefaults_t tunpack_pds_SetPDSDefaults_t ={0, {{0}} };

uint32_t bauto = 0xFF;
unpack_pds_GetPortAutomaticTracking_t tunpack_pds_GetPortAutomaticTracking_t = {&bauto, {{0}} };

pack_pds_SetPortAutomaticTracking_t tpack_pds_SetPortAutomaticTracking_t[] = {{0x01},{0x00}}; //Enable- Disable
unpack_pds_SetPortAutomaticTracking_t tunpack_pds_SetPortAutomaticTracking_t = {0, {{0}} };

pack_pds_StartPDSTrackingSessionExt_t tpack_pds_StartPDSTrackingSessionExt ={0, 0, 0, 0, 255, 1, 10, 10};
unpack_pds_StartPDSTrackingSessionExt_t tunpack_pds_StartPDSTrackingSessionExt ={0, {{0}} };

unpack_pds_StopPDSTrackingSession_t tunpack_pds_StopPDSTrackingSession = {0, {{0}} };
pack_pds_PDSInjectTimeReference_t tpack_pds_PDSInjectTimeReference = {100, 4};
unpack_pds_PDSInjectTimeReference_t tunpack_pds_PDSInjectTimeReference = {0, {{0}} };

uint32_t bEnabled = 0xFFFFFFFF;
uint16_t pInterval = 0xFFFF;
unpack_pds_GetXTRAAutomaticDownload_t tunpack_pds_GetXTRAAutomaticDownload = {&bEnabled, &pInterval, {{0}} };

pack_pds_SetXTRAAutomaticDownload_t tpack_pds_SetXTRAAutomaticDownload = {1, 1};
unpack_pds_SetXTRAAutomaticDownload_t tunpack_pds_SetXTRAAutomaticDownload = {0, {{0}} };

uint32_t xtraNwPreference = 0xFF;
unpack_pds_GetXTRANetwork_t tunpack_pds_GetXTRANetwork = {&xtraNwPreference, {{0}} };

pack_pds_SetXTRANetwork_t tpack_pds_SetXTRANetwork = {1};
unpack_pds_SetXTRANetwork_t tunpack_pds_SetXTRANetwork = {0, {{0}} };

uint16_t GPSWeek = 0xFFFF;
uint16_t GPSWeekOffset = 0xFFFF;
uint16_t Duration = 0xFFFF;

unpack_pds_GetXTRAValidity_t tunpack_pds_GetXTRAValidity = {&GPSWeek, &GPSWeekOffset, &Duration, {{0}} };

unpack_pds_ForceXTRADownload_t tunpack_pds_ForceXTRADownload = {0, {{0}} };

uint32_t bAuto = 0xFF;
unpack_pds_GetServiceAutomaticTracking_t tunpack_pds_GetServiceAutomaticTracking = {&bAuto, {{0}} };

pack_pds_SetServiceAutomaticTracking_t tpack_pds_SetServiceAutomaticTracking = {1};
unpack_pds_SetServiceAutomaticTracking_t tunpack_pds_SetServiceAutomaticTracking = {0, {{0}} };

uint32_t GPSDataMask = 0x000003FF; //GPS Data Clear
uint32_t CellDataMask = 0x000000FF; //Cell Data Clear
pack_pds_ResetPDSData_t tpack_pds_ResetPDSData[] = {
        {NULL, &CellDataMask},
        {&GPSDataMask, NULL}
};
unpack_pds_ResetPDSData_t tunpack_pds_ResetPDSData = {0, {{0}} };

uint8_t  serverUrl[]     = "www.abc.com";
uint32_t serverAddress   = 0x12345678;
uint32_t serverport      = 5001;
uint8_t  serverUrlLength = 11;
uint8_t  networkMode     = 0x00;

pack_pds_SLQSSetAGPSConfig_t tpack_pds_SLQSSetAGPSConfig = {&serverAddress, &serverport,
        serverUrl, &serverUrlLength, &networkMode};
unpack_pds_SLQSSetAGPSConfig_t tunpack_pds_SLQSSetAGPSConfig = {0, {{0}} };

pack_pds_SLQSPDSInjectAbsoluteTimeReference_t tpack_pds_SLQSPDSInjectAbsoluteTimeReference = {
        100, 1, 1, 0};
unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t tunpack_pds_SLQSPDSInjectAbsoluteTimeReference = {
        0, {{0}} };

pack_pds_SLQSGetAGPSConfig_t tpack_pds_SLQSGetAGPSConfig = {&networkMode};

unpack_pds_SLQSGetAGPSConfig_t tunpack_pds_SLQSGetAGPSConfig = {&serverAddress, &serverport,
        serverUrl, &serverUrlLength, {{0}} };

uint64_t     timeStamp = 0x12345678;
uint64_t     Latitude = 0x12345678;
uint64_t     Longitude = 0x12345678;
uint32_t     AltitudeWrtEllipsoid;
uint32_t     AltitudeWrtSealevel;
uint32_t     HorizontalUncCircular;
uint32_t     VerticalUnc;
uint8_t      HorizontalConfidence = 0x12;
uint8_t      VerticalConfidence;
uint8_t      PositionSource;
uint8_t      TimeType;
pack_pds_SLQSPDSInjectPositionData_t tpack_pds_SLQSPDSInjectPositionData = {&timeStamp, &Latitude, &Longitude, &AltitudeWrtEllipsoid, &AltitudeWrtSealevel,
        &HorizontalUncCircular, &VerticalUnc, &VerticalConfidence, &HorizontalConfidence, &PositionSource, &TimeType};
unpack_pds_SLQSPDSInjectPositionData_t tunpack_pds_SLQSPDSInjectPositionData = { 0, {{0}} };

unpack_pds_SLQSPDSDeterminePosition_t tunpack_pds_SLQSPDSDeterminePosition = {0, {{0}} };

unpack_pds_SLQSGetGPSStateInfo_t tunpack_pds_SLQSGetGPSStateInfo;
uint8_t      XtraTimeState = 0x00;
uint8_t      XtraDataState = 0x01;
uint8_t      WifiState ;
pack_pds_SLQSSetPositionMethodState_t tpack_pds_SLQSSetPositionMethodState = {&XtraTimeState, &XtraDataState,
        NULL};
unpack_pds_SLQSSetPositionMethodState_t tunpack_pds_SLQSSetPositionMethodState = {0, {{0}} };

pack_pds_SetEventReportCallback_t tpack_pds_SetEventReportCallback = {0x01 , 0x00};
unpack_pds_SetEventReportCallback_t tunpack_pds_SetEventReportCallback = {0 , {{0}} };

swi_uint256_t unpack_pds_GetPDSStateParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_GetPDSStateParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_SetPDSStateParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_SetPDSStateParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_ResetPDSDataParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_ResetPDSDataParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_SetPdsState_IndParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

swi_uint256_t unpack_pds_SetPdsState_IndParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

swi_uint256_t unpack_pds_GetPortAutomaticTrackingParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_GetPortAutomaticTrackingParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_SetPortAutomaticTrackingParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_SetPortAutomaticTrackingParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_GetPDSDefaultsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_GetPDSDefaultsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_pds_SetPDSDefaultsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_pds_SetPDSDefaultsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

void dump_GetPDSState(void *ptr)
{
    unpack_pds_GetPDSState_t *result =
            (unpack_pds_GetPDSState_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if( (result->pEnabledStatus!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
    printf( "GPS Enabled Status : %x\n", *result->pEnabledStatus);
    if( (result->pTrackingStatus!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
    printf( "GPS Tracking Status : %x\n",*result->pTrackingStatus);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_pds_GetPDSStateParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_GetPDSStateParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetPDSState (void *ptr)
{
    unpack_pds_SetPDSState_t *result =
            (unpack_pds_SetPDSState_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_pds_SetPDSStateParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_SetPDSStateParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( result->Tlvresult != 0 )
    {
        local_fprintf("%s,", SUCCESS_MSG);
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", SUCCESS_MSG);
    }
#endif
}

void dump_GetPDSDefaults(void *ptr)
{
    unpack_pds_GetPDSDefaults_t *result =
            (unpack_pds_GetPDSDefaults_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        if(result->pOperation)
            printf( "Operating Mode : %u\n", *result->pOperation);
        if(result->pTimeout)
            printf( "Max. Time for each Fix : %d\n", *result->pTimeout);
        if(result->pInterval)
            printf( "Interval between Fix requests : %u\n", *result->pInterval);
        if(result->pAccuracy)
            printf( "Current Accuracy Threshold : %u\n", *result->pAccuracy);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetPDSDefaults (void *ptr)
{
    unpack_pds_SetPDSDefaults_t *result =
            (unpack_pds_SetPDSDefaults_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_pds_SetPDSDefaultsParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_SetPDSDefaultsParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
 if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", SUCCESS_MSG);
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", SUCCESS_MSG);
    }
#endif
}

void dump_GetPortAutomaticTracking(void *ptr)
{
    unpack_pds_GetPortAutomaticTracking_t *result =
            (unpack_pds_GetPortAutomaticTracking_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if( (result->pbAuto) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
        printf( "Automatic Tracking Status : %x\n", *result->pbAuto);
}

void dump_SetPortAutomaticTracking (void *ptr)
{
    unpack_pds_SetPortAutomaticTracking_t *result =
            (unpack_pds_SetPortAutomaticTracking_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_pds_SetPortAutomaticTrackingParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_SetPortAutomaticTrackingParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", "Correct");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "Correct");
    }
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_StartPDSTrackingSessionExt (void *ptr)
{
    unpack_pds_StartPDSTrackingSessionExt_t *result =
            (unpack_pds_StartPDSTrackingSessionExt_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", "Correct");
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", "Correct");
    }
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_StopPDSTrackingSession (void *ptr)
{
    unpack_pds_StopPDSTrackingSession_t *result =
            (unpack_pds_StopPDSTrackingSession_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_PDSInjectTimeReference (void *ptr)
{
    unpack_pds_PDSInjectTimeReference_t *result =
            (unpack_pds_PDSInjectTimeReference_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetXTRAAutomaticDownload(void *ptr)
{
    unpack_pds_GetXTRAAutomaticDownload_t *result =
            (unpack_pds_GetXTRAAutomaticDownload_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pbEnabled) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf( "Automatic XTRA download status: 0X%x\n", *result->pbEnabled);
    if( (result->pInterval) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf( "Interval between XTRA downloads: 0X%x\n", *result->pInterval);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetXTRAAutomaticDownload (void *ptr)
{
    unpack_pds_SetXTRAAutomaticDownload_t *result =
            (unpack_pds_SetXTRAAutomaticDownload_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetXTRANetwork(void *ptr)
{
    unpack_pds_GetXTRANetwork_t *result =
            (unpack_pds_GetXTRANetwork_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pPreference) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf( "XTRA WWAN network preference: 0x%X\n", *result->pPreference);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetXTRANetwork (void *ptr)
{
    unpack_pds_SetXTRANetwork_t *result =
            (unpack_pds_SetXTRANetwork_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%-10s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetXTRAValidity(void *ptr)
{
    unpack_pds_GetXTRAValidity_t *result =
            (unpack_pds_GetXTRAValidity_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        if(result->pGPSWeek)
            printf("GPS week of validity period: %d\n", *result->pGPSWeek);
        if(result->pGPSWeekOffset)
            printf("GPS week offset of validity period: %d\n", *result->pGPSWeekOffset);
        if(result->pDuration)
            printf( "Length of validity period: %d\n", *result->pDuration);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ForceXTRADownload (void *ptr)
{
    unpack_pds_ForceXTRADownload_t *result =
            (unpack_pds_ForceXTRADownload_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetServiceAutomaticTracking(void *ptr)
{
    unpack_pds_GetServiceAutomaticTracking_t *result =
            (unpack_pds_GetServiceAutomaticTracking_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pbAuto) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
        printf("Automatic Tracking Status : 0x%X\n", *result->pbAuto);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetServiceAutomaticTracking (void *ptr)
{
    unpack_pds_SetServiceAutomaticTracking_t *result =
            (unpack_pds_SetServiceAutomaticTracking_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_ResetPDSData (void *ptr)
{
    unpack_pds_ResetPDSData_t *result =
            (unpack_pds_ResetPDSData_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
            unpack_pds_ResetPDSDataParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_ResetPDSDataParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", SUCCESS_MSG);
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", SUCCESS_MSG);
    }
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSSetAGPSConfig (void *ptr)
{
    unpack_pds_SLQSSetAGPSConfig_t *result =
            (unpack_pds_SLQSSetAGPSConfig_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSPDSInjectAbsoluteTimeReference (void *ptr)
{
    unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t *result =
            (unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetAGPSConfig(void *ptr)
{
    unpack_pds_SLQSGetAGPSConfig_t *result =
            (unpack_pds_SLQSGetAGPSConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
        if( (result->pServerAddress) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
            printf("\tServer Address : %x\n",
                    *(result->pServerAddress) );
        if( (result->pServerPort) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
            printf("\tServer Port : %x\n",
                    *(result->pServerPort) );
        if( (result->pServerURL) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
            printf("\tServer URL : %s\n",
                    result->pServerURL );
        if( (result->pServerURLLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
            printf("\tServer URL Length : 0x%X\n",
                    *(result->pServerURLLength) );
        if(iLocalLog==0)
            return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSPDSInjectPositionData (void *ptr)
{
    unpack_pds_SLQSPDSInjectPositionData_t *result =
            (unpack_pds_SLQSPDSInjectPositionData_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSPDSDeterminePosition (void *ptr)
{
    unpack_pds_SLQSPDSDeterminePosition_t *result =
            (unpack_pds_SLQSPDSDeterminePosition_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSGetGPSStateInfo(void *ptr)
{
    unpack_pds_SLQSGetGPSStateInfo_t *result =
            (unpack_pds_SLQSGetGPSStateInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("Engine State           : 0x%x\n", result->EngineState );
        printf( "Valid Mask            : 0x%x\n", result->ValidMask );
        printf( "Latitude              : 0x%"PRIX64"\n", result->Latitude );

        printf( "Longitude              : 0x%"PRIX64"\n", result->Longitude );

        printf( "Horizontal Uncertainty : 0x%x\n", result->HorizontalUncertainty );
        printf( "Altitude               : 0x%x\n", result->Altitude );
        printf( "Vertical Uncertainty   : 0x%x\n", result->VerticalUncertainty );
        printf( "TimeStamp tow ms       : 0x%x\n", result->TimeStmp_tow_ms );
        printf( "TimeStamp GPS week     : 0x%x\n", result->TimeStmp_gps_week );
        printf( "Time Uncertainty ms    : 0x%x\n", result->Time_uncert_ms );
        printf( "Iono validity          : 0x%x\n", result->Iono_valid );
        printf( "GPS Ephemeris SV Mask  : 0x%x\n", result->gps_ephemeris_sv_msk );
        printf( "GPS Almanac SV Mask    : 0x%x\n", result->gps_almanac_sv_msk );
        printf( "GPS Health SV Mask     : 0x%x\n", result->gps_health_sv_msk );
        printf( "GPS Visible SV Mask    : 0x%x\n", result->gps_visible_sv_msk );
        printf( "GLO Ephemeris SV Mask  : 0x%x\n", result->glo_ephemeris_sv_msk );
        printf( "GLO Almanac SV Mask    : 0x%x\n", result->glo_almanac_sv_msk );
        printf( "GLO Health SV Mask     : 0x%x\n", result->glo_health_sv_msk );
        printf( "GLO Visible SV Mask    : 0x%x\n", result->glo_visible_sv_msk );
        printf( "SBAS Ephemeris SV Mask : 0x%x\n", result->sbas_ephemeris_sv_msk );
        printf( "SBAS Almanac SV Mask   : 0x%x\n", result->sbas_almanac_sv_msk );
        printf( "SBAS Health SV Mask    : 0x%x\n", result->sbas_health_sv_msk );
        printf( "SBAS Visible SV Mask   : 0x%x\n", result->sbas_visible_sv_msk );
        printf( "Xtra Start GPS Week    : 0x%x\n", result->xtra_start_gps_week );
        printf( "Xtra Start GPS minutes : 0x%x\n", result->xtra_start_gps_minutes );
        printf( "Xtra valid duration hrs : 0x%x\n", result->xtra_valid_duration_hours );
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%-10s\n",  "N/A");
#endif
}

void dump_SLQSSetPositionMethodState (void *ptr)
{
    unpack_pds_SLQSSetPositionMethodState_t *result =
            (unpack_pds_SLQSSetPositionMethodState_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetXTRAAutomaticDownloadSettings(void *ptr)
{
    unpack_pds_GetXTRAAutomaticDownload_t *result =
            (unpack_pds_GetXTRAAutomaticDownload_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if(result->pbEnabled)
            printf( "Automatic XTRA download status: 0X%x\n", *result->pbEnabled);
        if(result->pInterval)
            printf( "Interval between XTRA downloads: 0X%x\n", *result->pInterval);
        if(result->pbEnabled)
            DefaultXTRAAutomaticDownloadSettings.bEnabled = *result->pbEnabled;
        if(result->pInterval)
            DefaultXTRAAutomaticDownloadSettings.interval = *result->pInterval;
    }
}

void dump_GetXTRANetworkSettings(void *ptr)
{
    unpack_pds_GetXTRANetwork_t *result =
            (unpack_pds_GetXTRANetwork_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if( (result->pPreference) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf( "XTRA WWAN network preference: 0x%X\n", *result->pPreference);
        DefaultXTRANetworkSettings.preference = *result->pPreference;
    }

}

void dump_GetServiceAutomaticTrackingSettings(void *ptr)
{
    unpack_pds_GetServiceAutomaticTracking_t *result =
            (unpack_pds_GetServiceAutomaticTracking_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if( (result->pbAuto) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
    {
        printf("Automatic Tracking Status : 0x%X\n", *result->pbAuto);
        DefaultServiceAutomaticTrackingSettings.bAuto = *result->pbAuto;
    }    
}

void dump_GetPortAutomaticTrackingSettings(void *ptr)
{
    unpack_pds_GetPortAutomaticTracking_t *result =
            (unpack_pds_GetPortAutomaticTracking_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;
    CHECK_WHITELIST_MASK(
            unpack_pds_GetPortAutomaticTrackingParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_GetPortAutomaticTrackingParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( ( result->pbAuto ) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
    {
        if ( (*result->pbAuto != 0) &&
                (*result->pbAuto != 1))
                is_matching = 0;
    }
        if ( unpackRetCode != 0 )
        {
            local_fprintf( "%s,", ((is_matching ==1)  ? "Correct": "Wrong"));
            local_fprintf( "%s\n", remark);
        }
        else
            local_fprintf( "%s\n",  ((is_matching ==1) ? "Correct": "Wrong"));
#endif

    if((result->pbAuto) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
    {
        printf( "Automatic Tracking Status : %x\n", *result->pbAuto);
        DefaultPortAutomaticTrackingSettings.bAuto = *result->pbAuto;
    }
}

void dump_SLQSGetAGPSConfigSettings(void *ptr)
{
    unpack_pds_SLQSGetAGPSConfig_t *result =
            (unpack_pds_SLQSGetAGPSConfig_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if( (result->pServerAddress) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("\tServer Address : %x\n",
                *(result->pServerAddress) );
        if(CHECK_PTR_IS_NOT_NULL(DefaultAGPSConfigSettings.pServerAddress))
        *(DefaultAGPSConfigSettings.pServerAddress) = *(result->pServerAddress);
    }
    if( (result->pServerPort) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("\tServer Port : %x\n",
                *(result->pServerPort) );
        if(CHECK_PTR_IS_NOT_NULL(DefaultAGPSConfigSettings.pServerPort))
        *(DefaultAGPSConfigSettings.pServerPort) = *(result->pServerPort);
    }
    if( (result->pServerURL) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\tServer URL : %s\n",
            result->pServerURL );
        if(CHECK_PTR_IS_NOT_NULL(DefaultAGPSConfigSettings.pServerURL))
        memcpy(DefaultAGPSConfigSettings.pServerURL,result->pServerURL,*(result->pServerURLLength));
    }
    if( (result->pServerURLLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\tServer URL Length : 0x%X\n",
                *(result->pServerURLLength) );
        if(CHECK_PTR_IS_NOT_NULL(DefaultAGPSConfigSettings.pServerURLLength))
        *(DefaultAGPSConfigSettings.pServerURLLength) = *(result->pServerURLLength);
    }
}

void dump_GetPDSStateSettings(void *ptr)
{
    unpack_pds_GetPDSState_t *result =
            (unpack_pds_GetPDSState_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;
    CHECK_WHITELIST_MASK(
            unpack_pds_GetPDSStateParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_GetPDSStateParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( (result->pEnabledStatus ) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        if ( (*result->pEnabledStatus != 0) &&
                (*result->pEnabledStatus != 1))
                is_matching = 0;
    }
        if ( unpackRetCode != 0 )
        {
            local_fprintf( "%s,", ((is_matching ==1)  ? SUCCESS_MSG : FAILED_MSG));
            local_fprintf( "%s\n", remark);
        }
        else
            local_fprintf( "%s\n",  ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif

    if( (result->pEnabledStatus!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf( "GPS Enabled Status : %x\n", *result->pEnabledStatus);
        DefaultPDSStateSettings.enable = *result->pEnabledStatus;
    }
    if( (result->pTrackingStatus!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
        printf( "GPS Tracking Status : %x\n",*result->pTrackingStatus);
}

void dump_SetEventReportCallback (void *ptr)
{
    unpack_pds_SetEventReportCallback_t *result =
            (unpack_pds_SetEventReportCallback_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetPDSStateVerifyDisable (void *ptr)
{
    unpack_pds_GetPDSState_t *result =
            (unpack_pds_GetPDSState_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if((result!=NULL) && (result->pEnabledStatus !=NULL))
    {
        if ((*result->pEnabledStatus == tpack_pds_SetPDSState_disable.enable))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetPDSStateVerifyEnable (void *ptr)
{
    unpack_pds_GetPDSState_t *result =
            (unpack_pds_GetPDSState_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if((result!=NULL) && (result->pEnabledStatus !=NULL))
    {
        if ((*result->pEnabledStatus == tpack_pds_SetPDSState_enable.enable))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetPortAutomaticTrackingVerifyEnable(void *ptr)
{
    unpack_pds_GetPortAutomaticTracking_t *result =
            (unpack_pds_GetPortAutomaticTracking_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if((result!=NULL) && (result->pbAuto !=NULL))
    {
        if ((*result->pbAuto == tpack_pds_SetPortAutomaticTracking_t[0].bAuto))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetPortAutomaticTrackingVerifyDisable(void *ptr)
{
    unpack_pds_GetPortAutomaticTracking_t *result =
            (unpack_pds_GetPortAutomaticTracking_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if((result!=NULL) && (result->pbAuto !=NULL))
    {
        if ((*result->pbAuto == tpack_pds_SetPortAutomaticTracking_t[1].bAuto))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetPDSDefaultsSettings(void *ptr)
{
    unpack_pds_GetPDSDefaults_t *result =
            (unpack_pds_GetPDSDefaults_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        if(result->pOperation)
        {
            printf( "Operating Mode : %u\n", *result->pOperation);
            default_SetPDSDefaults.operation = *result->pOperation;
        }
        if(result->pTimeout)
        {
            printf( "Max. Time for each Fix : %d\n", *result->pTimeout);
            default_SetPDSDefaults.timeout = *result->pTimeout;
        }
        if(result->pInterval)
        {
            printf( "Interval between Fix requests : %u\n", *result->pInterval);
            default_SetPDSDefaults.interval = *result->pInterval;
        }
        if(result->pAccuracy)
        {
            printf( "Current Accuracy Threshold : %u\n", *result->pAccuracy);
            default_SetPDSDefaults.accuracy = *result->pAccuracy;
        }
    }
}

void dump_GetPDSDefaultsVerify(void *ptr)
{
    unpack_pds_GetPDSDefaults_t *result =
            (unpack_pds_GetPDSDefaults_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        if(result->pOperation)
            printf( "Operating Mode : %u\n", *result->pOperation);
        if(result->pTimeout)
            printf( "Max. Time for each Fix : %d\n", *result->pTimeout);
        if(result->pInterval)
            printf( "Interval between Fix requests : %u\n", *result->pInterval);
        if(result->pAccuracy)
            printf( "Current Accuracy Threshold : %u\n", *result->pAccuracy);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    CHECK_WHITELIST_MASK(
            unpack_pds_GetPDSDefaultsParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_pds_GetPDSDefaultsParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);

    if((result!=NULL) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        if(result->pOperation != NULL)
        {
            if ((*result->pOperation != default_SetPDSDefaults.operation))
            {
                is_matching = 0;
            }
        }
        if(result->pTimeout != NULL)
        {
            if ((*result->pTimeout != default_SetPDSDefaults.timeout))
            {
                is_matching = 0;
            }
        }
        if(result->pInterval != NULL)
        {
            if ((*result->pInterval != default_SetPDSDefaults.interval))
            {
                is_matching = 0;
            }
        }
        if(result->pAccuracy != NULL)
        {
            if ((*result->pAccuracy != default_SetPDSDefaults.accuracy))
            {
                is_matching = 0;
            }
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

testitem_t oemapitest_pds[] = {
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSStateSettings
    },
    {
        (pack_func_item) &pack_pds_SetPDSState, "pack_pds_SetPDSState",
        &tpack_pds_SetPDSState_disable,
        (unpack_func_item) &unpack_pds_SetPDSState, "unpack_pds_SetPDSState",
        &tunpack_pds_SetPDSState, dump_SetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSStateVerifyDisable
    },
    {
        (pack_func_item) &pack_pds_SetPDSState, "pack_pds_SetPDSState",
        &tpack_pds_SetPDSState_enable,
        (unpack_func_item) &unpack_pds_SetPDSState, "unpack_pds_SetPDSState",
        &tunpack_pds_SetPDSState, dump_SetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSStateVerifyEnable
    },
    {
        (pack_func_item) &pack_pds_SetPDSState, "pack_pds_SetPDSState",
        &DefaultPDSStateSettings,
        (unpack_func_item) &unpack_pds_SetPDSState, "unpack_pds_SetPDSState",
        &tunpack_pds_SetPDSState, dump_SetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        &tunpack_pds_GetPortAutomaticTracking_t, dump_GetPortAutomaticTrackingSettings
    },
    {
        (pack_func_item) &pack_pds_SetPortAutomaticTracking, "pack_pds_SetPortAutomaticTracking",
        &tpack_pds_SetPortAutomaticTracking_t[0],
        (unpack_func_item) &unpack_pds_SetPortAutomaticTracking, "unpack_pds_SetPortAutomaticTracking",
        &tunpack_pds_SetPortAutomaticTracking_t, dump_SetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        &tunpack_pds_GetPortAutomaticTracking_t, dump_GetPortAutomaticTrackingVerifyEnable
    },
    {
        (pack_func_item) &pack_pds_SetPortAutomaticTracking, "pack_pds_SetPortAutomaticTracking",
        &tpack_pds_SetPortAutomaticTracking_t[1],
        (unpack_func_item) &unpack_pds_SetPortAutomaticTracking, "unpack_pds_SetPortAutomaticTracking",
        &tunpack_pds_SetPortAutomaticTracking_t, dump_SetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        &tunpack_pds_GetPortAutomaticTracking_t, dump_GetPortAutomaticTrackingVerifyDisable
    },
    {
        (pack_func_item) &pack_pds_SetPortAutomaticTracking, "pack_pds_SetPortAutomaticTracking",
        &DefaultPortAutomaticTrackingSettings,
        (unpack_func_item) &unpack_pds_SetPortAutomaticTracking, "unpack_pds_SetPortAutomaticTracking",
        &tunpack_pds_SetPortAutomaticTracking_t, dump_SetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_ResetPDSData, "pack_pds_ResetPDSData",
        &tpack_pds_ResetPDSData[0],
        (unpack_func_item) &unpack_pds_ResetPDSData, "unpack_pds_ResetPDSData",
        &tunpack_pds_ResetPDSData, dump_ResetPDSData
    },
    {
        (pack_func_item) &pack_pds_GetPDSDefaults, "pack_pds_GetPDSDefaults",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSDefaults, "unpack_pds_GetPDSDefaults",
        &tunpack_pds_GetPDSDefaults_t, dump_GetPDSDefaultsSettings
    },
    {
        (pack_func_item) &pack_pds_SetPDSDefaults, "pack_pds_SetPDSDefaults",
        &default_SetPDSDefaults,
        (unpack_func_item) &unpack_pds_SetPDSDefaults, "unpack_pds_SetPDSDefaults",
        &tunpack_pds_SetPDSDefaults_t, dump_SetPDSDefaults
    },
    {
        (pack_func_item) &pack_pds_GetPDSDefaults, "pack_pds_GetPDSDefaults",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSDefaults, "unpack_pds_GetPDSDefaults",
        &tunpack_pds_GetPDSDefaults_t, dump_GetPDSDefaultsVerify
    },
};

testitem_t pdstotest[] = {
    ///Get Modem Settings
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSStateSettings
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        &tunpack_pds_GetPortAutomaticTracking_t, dump_GetPortAutomaticTrackingSettings
    },
    {
        (pack_func_item) &pack_pds_GetXTRAAutomaticDownload, "pack_pds_GetXTRAAutomaticDownload",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAAutomaticDownload, "unpack_pds_GetXTRAAutomaticDownload",
        &tunpack_pds_GetXTRAAutomaticDownload, dump_GetXTRAAutomaticDownloadSettings
    },
    {
        (pack_func_item) &pack_pds_GetXTRANetwork, "pack_pds_GetXTRANetwork",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRANetwork, "unpack_pds_GetXTRANetwork",
        &tunpack_pds_GetXTRANetwork, dump_GetXTRANetworkSettings
    },
    {
        (pack_func_item) &pack_pds_GetServiceAutomaticTracking, "pack_pds_GetServiceAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetServiceAutomaticTracking, "unpack_pds_GetServiceAutomaticTracking",
        &tunpack_pds_GetServiceAutomaticTracking, dump_GetServiceAutomaticTrackingSettings
    },
    {
        (pack_func_item) &pack_pds_SLQSGetAGPSConfig, "pack_pds_SLQSGetAGPSConfig",
        &tpack_pds_SLQSGetAGPSConfig,
        (unpack_func_item) &unpack_pds_SLQSGetAGPSConfig, "unpack_pds_SLQSGetAGPSConfig",
        &tunpack_pds_SLQSGetAGPSConfig, dump_SLQSGetAGPSConfigSettings
    },
    //////////////
    {
        (pack_func_item) &pack_pds_SetPDSState, "pack_pds_SetPDSState",
        &tpack_pds_SetPDSState_disable,
        (unpack_func_item) &unpack_pds_SetPDSState, "unpack_pds_SetPDSState",
        &tunpack_pds_SetPDSState, dump_SetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSState
    },
    {
        (pack_func_item) &pack_pds_SetPDSState, "pack_pds_SetPDSState",
        &tpack_pds_SetPDSState_enable,
        (unpack_func_item) &unpack_pds_SetPDSState, "unpack_pds_SetPDSState",
        &tunpack_pds_SetPDSState, dump_SetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        &tunpack_pds_GetPDSState_t, dump_GetPDSState
    },
    {
        (pack_func_item) &pack_pds_GetPDSDefaults, "pack_pds_GetPDSDefaults",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSDefaults, "unpack_pds_GetPDSDefaults",
        &tunpack_pds_GetPDSDefaults_t, dump_GetPDSDefaults
    },
    {
        (pack_func_item) &pack_pds_SetPDSDefaults, "pack_pds_SetPDSDefaults",
        &tpack_pds_SetPDSDefaults_t,
        (unpack_func_item) &unpack_pds_SetPDSDefaults, "unpack_pds_SetPDSDefaults",
        &tpack_pds_SetPDSDefaults_t, dump_SetPDSDefaults
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        &tunpack_pds_GetPortAutomaticTracking_t, dump_GetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_SetPortAutomaticTracking, "pack_pds_SetPortAutomaticTracking",
        &tpack_pds_SetPortAutomaticTracking_t,
        (unpack_func_item) &unpack_pds_SetPortAutomaticTracking, "unpack_pds_SetPortAutomaticTracking",
        &tunpack_pds_SetPortAutomaticTracking_t, dump_SetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_StartPDSTrackingSessionExt, "pack_pds_StartPDSTrackingSessionExt",
        &tpack_pds_StartPDSTrackingSessionExt,
        (unpack_func_item) &unpack_pds_StartPDSTrackingSessionExt, "unpack_pds_StartPDSTrackingSessionExt",
        &tunpack_pds_StartPDSTrackingSessionExt, dump_StartPDSTrackingSessionExt
    },
    {
        (pack_func_item) &pack_pds_StopPDSTrackingSession, "pack_pds_StopPDSTrackingSession",
        NULL,
        (unpack_func_item) &unpack_pds_StopPDSTrackingSession, "unpack_pds_StopPDSTrackingSession",
        &tunpack_pds_StopPDSTrackingSession, dump_StopPDSTrackingSession
    },
    {
        (pack_func_item) &pack_pds_PDSInjectTimeReference, "pack_pds_PDSInjectTimeReference",
        &tpack_pds_PDSInjectTimeReference,
        (unpack_func_item) &unpack_pds_PDSInjectTimeReference, "unpack_pds_PDSInjectTimeReference",
        &tunpack_pds_PDSInjectTimeReference, dump_PDSInjectTimeReference
    },
    {
        (pack_func_item) &pack_pds_GetXTRAAutomaticDownload, "pack_pds_GetXTRAAutomaticDownload",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAAutomaticDownload, "unpack_pds_GetXTRAAutomaticDownload",
        &tunpack_pds_GetXTRAAutomaticDownload, dump_GetXTRAAutomaticDownload
    },
    {
        (pack_func_item) &pack_pds_SetXTRAAutomaticDownload, "pack_pds_SetXTRAAutomaticDownload",
        &tpack_pds_SetXTRAAutomaticDownload,
        (unpack_func_item) &unpack_pds_SetXTRAAutomaticDownload, "unpack_pds_SetXTRAAutomaticDownload",
        &tunpack_pds_SetXTRAAutomaticDownload, dump_SetXTRAAutomaticDownload
    },
    {
        (pack_func_item) &pack_pds_GetXTRANetwork, "pack_pds_GetXTRANetwork",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRANetwork, "unpack_pds_GetXTRANetwork",
        &tunpack_pds_GetXTRANetwork, dump_GetXTRANetwork
    },
    {
        (pack_func_item) &pack_pds_SetXTRANetwork, "pack_pds_SetXTRANetwork",
        &tpack_pds_SetXTRANetwork,
        (unpack_func_item) &unpack_pds_SetXTRANetwork, "unpack_pds_SetXTRANetwork",
        &tunpack_pds_SetXTRANetwork, dump_SetXTRANetwork
    },
    {
        (pack_func_item) &pack_pds_GetXTRANetwork, "pack_pds_GetXTRANetwork",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRANetwork, "unpack_pds_GetXTRANetwork",
        &tunpack_pds_GetXTRANetwork, dump_GetXTRANetwork
    },
    {
        (pack_func_item) &pack_pds_GetXTRAValidity, "pack_pds_GetXTRAValidity",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAValidity, "unpack_pds_GetXTRAValidity",
        &tunpack_pds_GetXTRAValidity, dump_GetXTRAValidity
    },
    {
        (pack_func_item) &pack_pds_ForceXTRADownload, "pack_pds_ForceXTRADownload",
        NULL,
        (unpack_func_item) &unpack_pds_ForceXTRADownload, "unpack_pds_ForceXTRADownload",
        &tunpack_pds_ForceXTRADownload, dump_ForceXTRADownload
    },
    {
        (pack_func_item) &pack_pds_GetServiceAutomaticTracking, "pack_pds_GetServiceAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetServiceAutomaticTracking, "unpack_pds_GetServiceAutomaticTracking",
        &tunpack_pds_GetServiceAutomaticTracking, dump_GetServiceAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_SetServiceAutomaticTracking, "pack_pds_SetServiceAutomaticTracking",
        &tpack_pds_SetServiceAutomaticTracking,
        (unpack_func_item) &unpack_pds_SetServiceAutomaticTracking, "unpack_pds_SetServiceAutomaticTracking",
        &tunpack_pds_SetServiceAutomaticTracking, dump_SetServiceAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_ResetPDSData, "pack_pds_ResetPDSData",
        &tpack_pds_ResetPDSData[0],
        (unpack_func_item) &unpack_pds_ResetPDSData, "unpack_pds_ResetPDSData",
        &tunpack_pds_ResetPDSData, dump_ResetPDSData
    },
    {
        (pack_func_item) &pack_pds_ResetPDSData, "pack_pds_ResetPDSData",
        &tpack_pds_ResetPDSData[1],
        (unpack_func_item) &unpack_pds_ResetPDSData, "unpack_pds_ResetPDSData",
        &tunpack_pds_ResetPDSData, dump_ResetPDSData
    },
    {
        (pack_func_item) &pack_pds_SLQSSetAGPSConfig, "pack_pds_SLQSSetAGPSConfig",
        &tpack_pds_SLQSSetAGPSConfig,
        (unpack_func_item) &unpack_pds_SLQSSetAGPSConfig, "unpack_pds_SLQSSetAGPSConfig",
        &tunpack_pds_SLQSSetAGPSConfig, dump_SLQSSetAGPSConfig
    },
    {
        (pack_func_item) &pack_pds_SLQSPDSInjectAbsoluteTimeReference, "pack_pds_SLQSPDSInjectAbsoluteTimeReference",
        &tpack_pds_SLQSPDSInjectAbsoluteTimeReference,
        (unpack_func_item) &unpack_pds_SLQSPDSInjectAbsoluteTimeReference, "unpack_pds_SLQSPDSInjectAbsoluteTimeReference",
        &tunpack_pds_SLQSPDSInjectAbsoluteTimeReference, dump_SLQSPDSInjectAbsoluteTimeReference
    },
    {
        (pack_func_item) &pack_pds_SLQSGetAGPSConfig, "pack_pds_SLQSGetAGPSConfig",
        &tpack_pds_SLQSGetAGPSConfig,
        (unpack_func_item) &unpack_pds_SLQSGetAGPSConfig, "unpack_pds_SLQSGetAGPSConfig",
        &tunpack_pds_SLQSGetAGPSConfig, dump_SLQSGetAGPSConfig
    },
    {
        (pack_func_item) &pack_pds_SLQSPDSInjectPositionData, "pack_pds_SLQSPDSInjectPositionData",
        &tpack_pds_SLQSPDSInjectPositionData,
        (unpack_func_item) &unpack_pds_SLQSPDSInjectPositionData, "unpack_pds_SLQSPDSInjectPositionData",
        &tunpack_pds_SLQSPDSInjectPositionData, dump_SLQSPDSInjectPositionData
    },
    {
        (pack_func_item) &pack_pds_SLQSPDSDeterminePosition, "pack_pds_SLQSPDSDeterminePosition",
        NULL,
        (unpack_func_item) &unpack_pds_SLQSPDSDeterminePosition, "unpack_pds_SLQSPDSDeterminePosition",
        &tunpack_pds_SLQSPDSDeterminePosition, dump_SLQSPDSDeterminePosition
    },
    {
        (pack_func_item) &pack_pds_SLQSGetGPSStateInfo, "pack_pds_SLQSGetGPSStateInfo",
        NULL,
        (unpack_func_item) &unpack_pds_SLQSGetGPSStateInfo, "unpack_pds_SLQSGetGPSStateInfo",
        &tunpack_pds_SLQSGetGPSStateInfo, dump_SLQSGetGPSStateInfo
    },
    {
        (pack_func_item) &pack_pds_SLQSSetPositionMethodState, "pack_pds_SLQSSetPositionMethodState",
        &tpack_pds_SLQSSetPositionMethodState,
        (unpack_func_item) &unpack_pds_SLQSSetPositionMethodState, "unpack_pds_SLQSSetPositionMethodState",
        &tunpack_pds_SLQSSetPositionMethodState, dump_SLQSSetPositionMethodState
    },
    ///Restore Modem settings
    {
        (pack_func_item) &pack_pds_SetPDSDefaults, "pack_pds_SetPDSDefaults",
        &DefaultPDSStateSettings,
        (unpack_func_item) &unpack_pds_SetPDSDefaults, "unpack_pds_SetPDSDefaults",
        &tpack_pds_SetPDSDefaults_t, dump_SetPDSDefaults
    },
    {
        (pack_func_item) &pack_pds_SetPortAutomaticTracking, "pack_pds_SetPortAutomaticTracking",
        &DefaultPortAutomaticTrackingSettings,
        (unpack_func_item) &unpack_pds_SetPortAutomaticTracking, "unpack_pds_SetPortAutomaticTracking",
        &tunpack_pds_SetPortAutomaticTracking_t, dump_SetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_SetXTRAAutomaticDownload, "pack_pds_SetXTRAAutomaticDownload",
        &DefaultXTRAAutomaticDownloadSettings,
        (unpack_func_item) &unpack_pds_SetXTRAAutomaticDownload, "unpack_pds_SetXTRAAutomaticDownload",
        &tunpack_pds_SetXTRAAutomaticDownload, dump_SetXTRAAutomaticDownload
    },
    {
        (pack_func_item) &pack_pds_SetXTRANetwork, "pack_pds_SetXTRANetwork",
        &DefaultXTRANetworkSettings,
        (unpack_func_item) &unpack_pds_SetXTRANetwork, "unpack_pds_SetXTRANetwork",
        &tunpack_pds_SetXTRANetwork, dump_SetXTRANetwork
    },
    {
        (pack_func_item) &pack_pds_SetServiceAutomaticTracking, "pack_pds_SetServiceAutomaticTracking",
        &DefaultServiceAutomaticTrackingSettings,
        (unpack_func_item) &unpack_pds_SetServiceAutomaticTracking, "unpack_pds_SetServiceAutomaticTracking",
        &tunpack_pds_SetServiceAutomaticTracking, dump_SetServiceAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_SLQSSetAGPSConfig, "pack_pds_SLQSSetAGPSConfig",
        &DefaultAGPSConfigSettings,
        (unpack_func_item) &unpack_pds_SLQSSetAGPSConfig, "unpack_pds_SLQSSetAGPSConfig",
        &tunpack_pds_SLQSSetAGPSConfig, dump_SLQSSetAGPSConfig
    },
    {
        (pack_func_item) &pack_pds_SetEventReportCallback, "pack_pds_SetEventReportCallback",
        &tpack_pds_SetEventReportCallback,
        (unpack_func_item) &unpack_pds_SetEventReportCallback, "unpack_pds_SetEventReportCallback",
        &tunpack_pds_SetEventReportCallback, dump_SetEventReportCallback
    },
    /////////////////////////////////////
};

unsigned int pdsarraylen = (unsigned int)((sizeof(pdstotest))/(sizeof(pdstotest[0])));

testitem_t pdstotest_invalidunpack[] = {
    ///Get Modem Settings
    {
        (pack_func_item) &pack_pds_GetPDSState, "pack_pds_GetPDSState",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSState, "unpack_pds_GetPDSState",
        NULL, dump_GetPDSStateSettings
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        NULL, dump_GetPortAutomaticTrackingSettings
    },
    {
        (pack_func_item) &pack_pds_GetXTRAAutomaticDownload, "pack_pds_GetXTRAAutomaticDownload",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAAutomaticDownload, "unpack_pds_GetXTRAAutomaticDownload",
        NULL, dump_GetXTRAAutomaticDownloadSettings
    },
    {
        (pack_func_item) &pack_pds_GetXTRANetwork, "pack_pds_GetXTRANetwork",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRANetwork, "unpack_pds_GetXTRANetwork",
        NULL, dump_GetXTRANetworkSettings
    },
    {
        (pack_func_item) &pack_pds_GetServiceAutomaticTracking, "pack_pds_GetServiceAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetServiceAutomaticTracking, "unpack_pds_GetServiceAutomaticTracking",
        NULL, dump_GetServiceAutomaticTrackingSettings
    },
    {
        (pack_func_item) &pack_pds_SLQSGetAGPSConfig, "pack_pds_SLQSGetAGPSConfig",
        &tpack_pds_SLQSGetAGPSConfig,
        (unpack_func_item) &unpack_pds_SLQSGetAGPSConfig, "unpack_pds_SLQSGetAGPSConfig",
        NULL, dump_SLQSGetAGPSConfigSettings
    },
    {
        (pack_func_item) &pack_pds_GetPDSDefaults, "pack_pds_GetPDSDefaults",
        NULL,
        (unpack_func_item) &unpack_pds_GetPDSDefaults, "unpack_pds_GetPDSDefaults",
        NULL, dump_GetPDSDefaults
    },
    {
        (pack_func_item) &pack_pds_GetPortAutomaticTracking, "pack_pds_GetPortAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetPortAutomaticTracking, "unpack_pds_GetPortAutomaticTracking",
        NULL, dump_GetPortAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_GetXTRAAutomaticDownload, "pack_pds_GetXTRAAutomaticDownload",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAAutomaticDownload, "unpack_pds_GetXTRAAutomaticDownload",
        NULL, dump_GetXTRAAutomaticDownload
    },
    {
        (pack_func_item) &pack_pds_GetXTRAValidity, "pack_pds_GetXTRAValidity",
        NULL,
        (unpack_func_item) &unpack_pds_GetXTRAValidity, "unpack_pds_GetXTRAValidity",
        NULL, dump_GetXTRAValidity
    },
    {
        (pack_func_item) &pack_pds_GetServiceAutomaticTracking, "pack_pds_GetServiceAutomaticTracking",
        NULL,
        (unpack_func_item) &unpack_pds_GetServiceAutomaticTracking, "unpack_pds_GetServiceAutomaticTracking",
        NULL, dump_GetServiceAutomaticTracking
    },
    {
        (pack_func_item) &pack_pds_SLQSGetAGPSConfig, "pack_pds_SLQSGetAGPSConfig",
        &tpack_pds_SLQSGetAGPSConfig,
        (unpack_func_item) &unpack_pds_SLQSGetAGPSConfig, "unpack_pds_SLQSGetAGPSConfig",
        NULL, dump_SLQSGetAGPSConfig
    },
    {
        (pack_func_item) &pack_pds_SLQSGetGPSStateInfo, "pack_pds_SLQSGetGPSStateInfo",
        NULL,
        (unpack_func_item) &unpack_pds_SLQSGetGPSStateInfo, "unpack_pds_SLQSGetGPSStateInfo",
        NULL, dump_SLQSGetGPSStateInfo
    },
    /////////////////////////////////////
};


void *pds_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);

    while(enPDSThread)
    {
        if(pds<1)
        {
            usleep(1000);
            printf("%s fd error\n", __func__);
            continue;
        }
        //TODO select multiple file and read them
        rtn = read(pds, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(ePDS, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("PDS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("PDS RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);

            switch(rsp_ctx.msgid)
            {
                case eQMI_PDS_EVENT_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_pds_SetEventReport_Ind_t pdsEventCallbackInd;
                        rtn = unpack_pds_SetEventReport_Ind(msg.buf, QMI_MSG_MAX, &pdsEventCallbackInd );
                        printf("QMI PDS EVENT IND result :%d\n",rtn);
                        swi_uint256_print_mask (pdsEventCallbackInd.ParamPresenceMask);
                        if( (pdsEventCallbackInd.has_PositionDataNMEA ==1) &&
                                (swi_uint256_get_bit (pdsEventCallbackInd.ParamPresenceMask, 16)) )
                        {
                            printf ( "NMEA Data Follows:\r\n" );
                            printf ( "%s\r\n", pdsEventCallbackInd.PositionDataNMEA);
                        }
                        if( (pdsEventCallbackInd.has_dLatitude ==1) &&
                                (swi_uint256_get_bit (pdsEventCallbackInd.ParamPresenceMask, 19)) )
                            printf ( "Latitude  : %lf\r\n", pdsEventCallbackInd.dLatitude);
                        if( (pdsEventCallbackInd.has_dLongitude ==1) &&
                                (swi_uint256_get_bit (pdsEventCallbackInd.ParamPresenceMask, 19)) )
                            printf ( "Longitude : %lf\r\n", pdsEventCallbackInd.dLongitude);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_pds_SetEventReportCallback_t pdsSetEventRptCallback;
                        rtn = unpack_pds_SetEventReportCallback( msg.buf, QMI_MSG_MAX, &pdsSetEventRptCallback);
                        printf("QMI PDS SET EVENT REPORT RSP result :%d\n",rtn);
                    }
                    break;

                case eQMI_PDS_STATE_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_pds_SetPdsState_Ind_t pdsStateInd;
                        unpackRetCode = unpack_pds_SetPdsState_Ind(msg.buf, QMI_MSG_MAX, &pdsStateInd );
                        printf("QMI PDS GPS SERVICE STATE IND result :%d\n",rtn);
                        swi_uint256_print_mask (pdsStateInd.ParamPresenceMask);

#if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_pds_SetPdsState_Ind",\
                                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        CHECK_WHITELIST_MASK(
                            unpack_pds_SetPdsState_IndParamPresenceMaskWhiteList,
                            pdsStateInd.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_pds_SetPdsState_IndParamPresenceMaskMandatoryList,
                            pdsStateInd.ParamPresenceMask);

                        local_fprintf("%s\n", ((pdsStateInd.TlvPresent ==1) ? SUCCESS_MSG: FAILED_MSG));
#endif
                        if(swi_uint256_get_bit (pdsStateInd.ParamPresenceMask, 1))
                        {
                            printf ("GPS Service State is: %u\r\n\n", pdsStateInd.EnabledStatus );
                            printf ("Tracking session state is: %u\r\n\n", pdsStateInd.TrackingStatus );
                        }
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

void pds_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======PDS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    const char *qmi_msg;
    for(i=0; i<sizeof(pdstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(pdstotest_invalidunpack[i].dump!=NULL)
        {
            pdstotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(pdstotest)/sizeof(testitem_t); i++)
    {
        if(pdstotest[i].dump!=NULL)
        {
            pdstotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(pdstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(pdstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,pdstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(pds<0)
            pds = client_fd(ePDS);
        if(pds<0)
        {
            fprintf(stderr,"PDS Service Not Supported!\n");
            return ;
        }
        rtn = write(pds, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(pds>=0)
                close(pds);
            pds=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(ePDS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(pds, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(ePDS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                rtn = run_unpack_item(pdstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                       pdstotest_invalidunpack [i].unpack_ptr);
                if(rtn!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           pdstotest_invalidunpack[i].unpack_func_name, rtn);
                    xid++;
                    continue;
                }
                else
                    pdstotest_invalidunpack[i].dump(pdstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(pds>=0)
                close(pds);
            pds=-1;
        }
        sleep(1);
        xid++;
    }
    if(pds>=0)
        close(pds);
    pds=-1;
}

void pds_test_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;

    printf("======pds dummy unpack test===========\n");
    iLocalLog = 0;
    pds_validate_dummy_unpack();
    iLocalLog = 1;


    printf("======PDS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "PDS Pack/UnPack API Name",
                      "Status",
                      "Unpack Payload Parsing",
                      "Remark");
#endif
    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitest_pds)/sizeof(testitem_t);
    }
    else
    {
        numoftestcase = sizeof(pdstotest)/sizeof(testitem_t);
    }

    unsigned xid =1;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason = NULL;

    for(i=0; i<numoftestcase ; i++)
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
        if(g_runoem_demo==1)
        {
            rtn = run_pack_item(oemapitest_pds[i].pack)(&req_ctx, req, &reqLen,oemapitest_pds[i].pack_ptr);
        }
        else
        {
            rtn = run_pack_item(pdstotest[i].pack)(&req_ctx, req, &reqLen,pdstotest[i].pack_ptr);
        }

#if DEBUG_LOG_TO_FILE
            if(g_runoem_demo==1)
            {
                local_fprintf("%s,%s,%s\n", oemapitest_pds[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
            }
            else
            {
                local_fprintf("%s,%s,%s\n", pdstotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
            }
#endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(pds<0)
            pds = client_fd(ePDS);
        if(pds<0)
        {
            fprintf(stderr,"PDS Service Not Supported!\n");
            return ;
        }
        rtn = write(pds, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(ePDS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            rtn = rspLen = read(pds, rsp, QMI_MSG_MAX);
            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(ePDS, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("PDS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d\n",xid);
                        if(g_runoem_demo==1)
                        {
                            unpackRetCode = run_unpack_item(oemapitest_pds[i].unpack)(rsp, rspLen, oemapitest_pds[i].unpack_ptr);
                        }
                        else
                        {
                            unpackRetCode = run_unpack_item(pdstotest[i].unpack)(rsp, rspLen, pdstotest[i].unpack_ptr);
                        }
#if DEBUG_LOG_TO_FILE
                    if(g_runoem_demo==1)
                    {
                        local_fprintf("%s,%s,", oemapitest_pds[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                    }
                    else
                    {
                        local_fprintf("%s,%s,", pdstotest[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                    }
                    if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                    {
                        memset(remark,0,sizeof(remark));
                        eRCode = helper_get_error_code(rsp);
                        reason = helper_get_error_reason(eRCode);
                        sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                    }
#endif

                        if(g_runoem_demo==1)
                        {
                            oemapitest_pds[i].dump(oemapitest_pds[i].unpack_ptr);
                        }
                        else
                        {
                            pdstotest[i].dump(pdstotest[i].unpack_ptr);
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
                    printf("PDS IND: msgid 0x%x\n", rsp_ctx.msgid);
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
    if(pds>=0)
        close(pds);
    pds=-1;
}

void pds_test_ind()
{
    pack_qmi_t req_ctx;
    uint16_t qmi_req_len;
    uint8_t qmi_req[QMI_MSG_MAX];
    int rtn;
    const char *qmi_msg;
    pack_pds_SetEventReportCallback_t SetPDSEventReq;
    if(pds<0)
        pds = client_fd(ePDS);
    if(pds<0)
    {
        fprintf(stderr,"PDS Service Not Supported\n");
        return ;
    }

    enPDSThread = 1;
    pthread_create(&pds_tid, &pds_attr, pds_read_thread, NULL);
    sleep(1);
    memset(&SetPDSEventReq,0,sizeof(pack_pds_SetEventReportCallback_t));
    req_ctx.xid = 0x100;
    memset(&pds_attr, 0, sizeof(pds_attr));
    SetPDSEventReq.posDataNmea =  0x01;
    SetPDSEventReq.rptPosData = 0x01;
    rtn = pack_pds_SetEventReportCallback(&req_ctx, qmi_req, &qmi_req_len, (void*)&SetPDSEventReq);
    if(rtn!=0)
    {
        printf("Set Event Callback Fail");
        return ;
    }
    rtn = write(pds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("PDS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(ePDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    pack_pds_SetPDSState_t PDSStateDisable;
    memset(&PDSStateDisable,0,sizeof(pack_pds_SetPDSState_t));
    req_ctx.xid ++;
    PDSStateDisable.enable =  0x00;
    rtn = pack_pds_SetPDSState(&req_ctx, qmi_req, &qmi_req_len, (void*)&PDSStateDisable);
    if(rtn!=0)
    {
        printf("Disable PDS State Fail");
        return ;
    }
    rtn = write(pds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("PDS FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(ePDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);
    sleep(25);
    enPDSThread = 0;
}

void pds_test_ind_exit()
{
    void *pthread_rtn_value;
    printf("\nkilling PDS read thread...\n");
    enPDSThread = 0;
    UNUSEDPARAM(pthread_rtn_value);
#ifdef __ARM_EABI__    
    if(pds>=0)
           close(pds);
        pds = -1;
    if(pds_tid!=0)
    pthread_join(pds_tid, &pthread_rtn_value);
#endif
    if(pds_tid!=0)
    pthread_cancel(pds_tid);
    pds_tid = 0;
    if(pds>=0)
        close(pds);
    pds=-1;
}


void dump_SetEventReport_Ind (void *ptr)
{
    unpack_pds_SetEventReport_Ind_t *result =
            (unpack_pds_SetEventReport_Ind_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->has_PositionDataNMEA ==1) &&
            (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf ( "NMEA Data Follows:\r\n" );
        printf ( "%s\r\n", result->PositionDataNMEA);
    }
    if( (result->has_dLatitude ==1) &&
            (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf ( "Latitude  : %lf\r\n", result->dLatitude);
    if( (result->has_dLongitude ==1) &&
            (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        printf ( "Longitude : %lf\r\n", result->dLongitude);
}

void dump_SetPdsState_Ind (void *ptr)
{
    unpack_pds_SetPdsState_Ind_t *result =
            (unpack_pds_SetPdsState_Ind_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf ("GPS Service State is: %u\r\n\n", result->EnabledStatus );
        printf ("Tracking session state is: %u\r\n\n", result->TrackingStatus );
    }
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_pds_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_PDS_GET_STATE */
    {0x02,0x01,0x00,0x20,0x00,0x0C,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x02,0x00,0x01,0x02},

    /* eQMI_PDS_SET_STATE */
    {0x02,0x02,0x00,0x21,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_DEFAULTS */
    {0x02,0x03,0x00,0x29,0x00,0x14,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x0A,0x00,0x00,0xFF,0x0B,0x00,0x00,0x00,0x0A,0x00,0x00,0x00},

    /* eQMI_PDS_SET_DEFAULTS */
    {0x02,0x04,0x00,0x2A,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_COM_AUTOTRACK */
    {0x02,0x05,0x00,0x32,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x01,0x00,0x01},

    /* eQMI_PDS_SET_COM_AUTOTRACK */
    {0x02,0x06,0x00,0x33,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_START_SESSION */
    {0x02,0x07,0x00,0x22,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_END_SESSION */
    {0x02,0x08,0x00,0x25,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_INJECT_TIME */
    {0x02,0x09,0x00,0x28,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_XTRA_PARAMS */
    {0x02,0x0A,0x00,0x2B,0x00,0x0D,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x03,0x00,0x01,0x06,0x00},

    /* eQMI_PDS_SET_XTRA_PARAMS */
    {0x02,0x0B,0x00,0x2C,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_XTRA_PARAMS */
    {0x02,0x0C,0x00,0x2B,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x12,0x01,0x00,0x01},

    /* eQMI_PDS_SET_XTRA_PARAMS */
    {0x02,0x0D,0x00,0x2C,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_XTRA_PARAMS */
    {0x02,0x0E,0x00,0x2B,0x00,0x10,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x13,0x06,0x00,0x34,0x12,0x22,0x11,0x56,0x23},

    /* eQMI_PDS_FORCE_XTRA_DL */
    {0x02,0x0F,0x00,0x2D,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_SVC_AUTOTRACK */
    {0x02,0x10,0x00,0x30,0x00,0x0B,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x01,0x00,0x01},

    /* eQMI_PDS_SET_SVC_AUTOTRACK */
    {0x02,0x11,0x00,0x31,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_RESET_DATA */
    {0x02,0x12,0x00,0x34,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_SET_AGPS_CONFIG */
    {0x02,0x13,0x00,0x2F,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_INJECT_ABS_TIME */
    {0x02,0x14,0x00,0x3D,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_AGPS_CONFIG */
    {0x02,0x15,0x00,0x2E,0x00,0x28,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x08,0x00,0x34,0xF1,0xD4,0xAD,0x15,0x27,0x00,0x00,
            0x11,0x13,0x00,0x11,'s','1','.','g','p','s','-','s','e','r','v','e','r','.','n','e','t'},

    /* eQMI_PDS_INJECT_POSITION */
    {0x02,0x16,0x00,0x38,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_FIX_POSITION */
    {0x02,0x17,0x00,0x24,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_GET_GPS_STATE */
    {0x02,0x18,0x00,0x44,0x00,0x6C,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x62,0x00,0x00,0xE3,0x03,0x04,0x00,0x72,0x16,0x78,0x1F,0x85,0xEB,0x35,0x40,0x31,0x15,0xEC,0x7F,0xEB,
            0xB1,0x54,0x40,0x20,0x1E,0x85,0x49,0x00,0x00 ,0x00 ,0x00,0x00,0x80,0xBB,0x45,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_SET_METHODS */
    {0x02,0x19,0x00,0x51,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_SET_EVENT */
    {0x02,0x1A,0x00,0x01,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_PDS_EVENT_IND */
    {0x04,0x1B,0x00,0x01,0x00,0xBB,0x00,
            0x10,0x41,0x00, '$','G','P','G','G','A',',','1','2','3','5','1','9',',','4',
                            '8','0','7','.','0','3','8',',','N',',','0','1','1','3','1',
                            '.','0','0','0',',','E',',','1',',','0','8',',','0','.','9',
                            ',','5','4','5','.','4',',','M',',','4','6','.','9',',','M',
                            ',',',','*','4','7',
            0x12,0x01,0x00,0x00,
            0x13,0x69,0x00,0x30,0x00,0x00,0x00,0xE3,0x07,0x08,0x03,0x21,0x12,0x20,0x28,0x32,
                            0xF4,0x01,0x22,0xFD,0xEC,0x5B,0x5D,0x00,0x00,0x00,0x00,0x39,0x02,0x00,
                            0x70,0x25,0x3B,0x36,0x02,0x09,0x48,0x40,0x1F,0x85,0xEB,0x51,0xB8,0x9E,0x26,0x40,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x1C,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_PDS_STATE_IND */
    {0x04,0x1B,0x00,0x20,0x00,0x05,0x00,
            0x01,0x02,0x00,0x01,0x02},
};

uint32_t cst_EnabledStatus = 0x01;
uint32_t cst_TrackingStatus = 0x02;

    /* eQMI_PDS_GET_STATE */
const unpack_pds_GetPDSState_t const_unpack_pds_GetPDSState_t = {
        &cst_EnabledStatus,&cst_TrackingStatus,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint32_t var_EnabledStatus;
uint32_t var_TrackingStatus;

    /* eQMI_PDS_GET_STATE */
unpack_pds_GetPDSState_t var_unpack_pds_GetPDSState_t = {
        &var_EnabledStatus,&var_TrackingStatus,{{0}} };

    /* eQMI_PDS_SET_STATE */
const unpack_pds_SetPDSState_t const_unpack_pds_SetPDSState_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_Operation = 0x00;
uint8_t  cst_Timeout = 0xFF;
uint32_t cst_Interval = 0x0000000B;
uint32_t cst_Accuracy =0x0000000A;

    /* eQMI_PDS_GET_DEFAULTS */
const unpack_pds_GetPDSDefaults_t const_unpack_pds_GetPDSDefaults_t = {
        &cst_Operation,&cst_Timeout,&cst_Interval,&cst_Accuracy,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint32_t var_Operation;
uint8_t  var_Timeout;
uint32_t var_Interval;
uint32_t var_Accuracy;

    /* eQMI_PDS_GET_DEFAULTS */
unpack_pds_GetPDSDefaults_t var_unpack_pds_GetPDSDefaults_t = {
        &var_Operation,&var_Timeout,&var_Interval, &var_Accuracy,{{0}} };

    /* eQMI_PDS_SET_DEFAULTS */
const unpack_pds_SetPDSDefaults_t const_unpack_pds_SetPDSDefaults_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_bAuto = 0x01;
    /* eQMI_PDS_GET_COM_AUTOTRACK */
const unpack_pds_GetPortAutomaticTracking_t const_unpack_pds_GetPortAutomaticTracking_t = {
        &cst_bAuto,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint32_t var_bAuto;
    /* eQMI_PDS_GET_STATE */
unpack_pds_GetPortAutomaticTracking_t var_unpack_pds_GetPortAutomaticTracking_t = {
        &var_bAuto,{{0}} };

    /* eQMI_PDS_SET_COM_AUTOTRACK */
const unpack_pds_SetPortAutomaticTracking_t const_unpack_pds_SetPortAutomaticTracking_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_START_SESSION */
const unpack_pds_StartPDSTrackingSessionExt_t const_unpack_pds_StartPDSTrackingSessionExt_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_END_SESSION */
const unpack_pds_StopPDSTrackingSession_t const_unpack_pds_StopPDSTrackingSession_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_INJECT_TIME */
const unpack_pds_PDSInjectTimeReference_t const_unpack_pds_PDSInjectTimeReference_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t  cst_bEnabled = 0x01;
uint16_t  cst_Xtra_Dld_Interval = 0x06;

    /* eQMI_PDS_GET_XTRA_PARAMS */
const unpack_pds_GetXTRAAutomaticDownload_t const_unpack_pds_GetXTRAAutomaticDownload_t = {
        &cst_EnabledStatus,&cst_Xtra_Dld_Interval,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

uint32_t  var_bEnabled;
uint16_t  var_Xtra_Dld_Interval;

    /* eQMI_PDS_GET_XTRA_PARAMS */
unpack_pds_GetXTRAAutomaticDownload_t var_unpack_pds_GetXTRAAutomaticDownload_t = {
        &var_bEnabled,&var_Xtra_Dld_Interval,{{0}} };

    /* eQMI_PDS_SET_XTRA_PARAMS */
const unpack_pds_SetXTRAAutomaticDownload_t const_unpack_pds_SetXTRAAutomaticDownload_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_nwPreference = 0x01;
    /* eQMI_PDS_GET_XTRA_PARAMS */
const unpack_pds_GetXTRANetwork_t const_unpack_pds_GetXTRANetwork_t = {
        &cst_nwPreference,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,18)}} };

uint32_t  var_nwPreference;
    /* eQMI_PDS_GET_XTRA_PARAMS */
unpack_pds_GetXTRANetwork_t var_unpack_pds_GetXTRANetwork_t = {
        &var_nwPreference,{{0}} };

    /* eQMI_PDS_SET_XTRA_PARAMS */
const unpack_pds_SetXTRANetwork_t const_unpack_pds_SetXTRANetwork_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint16_t cst_GPSWeek = 0x1234;
uint16_t cst_GPSWeekOffset = 0x1122;
uint16_t cst_Duration = 0x2356;
    /* eQMI_PDS_GET_XTRA_PARAMS */
const unpack_pds_GetXTRAValidity_t const_unpack_pds_GetXTRAValidity_t = {
        &cst_GPSWeek,&cst_GPSWeekOffset,&cst_Duration,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,19)}} };

uint16_t var_GPSWeek;
uint16_t var_GPSWeekOffset;
uint16_t var_Duration;
    /* eQMI_PDS_GET_XTRA_PARAMS */
unpack_pds_GetXTRAValidity_t var_unpack_pds_GetXTRAValidity_t = {
        &var_GPSWeek, &var_GPSWeekOffset, &var_Duration,{{0}} };

    /* eQMI_PDS_FORCE_XTRA_DL */
const unpack_pds_ForceXTRADownload_t const_unpack_pds_ForceXTRADownload_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_autoTrack_bAuto = 0x01;
    /* eQMI_PDS_GET_SVC_AUTOTRACK */
const unpack_pds_GetServiceAutomaticTracking_t const_unpack_pds_GetServiceAutomaticTracking_t = {
        &cst_autoTrack_bAuto,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,1)}} };

uint32_t var_autoTrack_bAuto;
    /* eQMI_PDS_GET_SVC_AUTOTRACK */
unpack_pds_GetServiceAutomaticTracking_t var_unpack_pds_GetServiceAutomaticTracking_t = {
        &var_autoTrack_bAuto,{{0}} };

    /* eQMI_PDS_SET_SVC_AUTOTRACK */
const unpack_pds_SetServiceAutomaticTracking_t const_unpack_pds_SetServiceAutomaticTracking_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_RESET_DATA */
const unpack_pds_ResetPDSData_t const_unpack_pds_ResetPDSData_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_SET_AGPS_CONFIG */
const unpack_pds_SLQSSetAGPSConfig_t const_unpack_pds_SLQSSetAGPSConfig_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_INJECT_ABS_TIME */
const unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t const_unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_ServerAddress = 0xADD4F134;
uint32_t cst_ServerPort = 0x2715;
uint8_t  cst_ServerURL[] = "s1.gps-server.net";
uint8_t  cst_ServerURLLength = 0x11;

    /* eQMI_PDS_GET_AGPS_CONFIG */
const unpack_pds_SLQSGetAGPSConfig_t const_unpack_pds_SLQSGetAGPSConfig_t = {
    &cst_ServerAddress,&cst_ServerPort,cst_ServerURL,&cst_ServerURLLength,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

uint32_t var_ServerAddress;
uint32_t var_ServerPort;
uint8_t  var_ServerURL[255];
uint8_t  var_ServerURLLength = 255;
    /* eQMI_PDS_GET_AGPS_CONFIG */
unpack_pds_SLQSGetAGPSConfig_t var_unpack_pds_SLQSGetAGPSConfig_t = {
    &var_ServerAddress, &var_ServerPort, var_ServerURL,&var_ServerURLLength,{{0}} };

    /* eQMI_PDS_INJECT_POSITION */
const unpack_pds_SLQSPDSInjectPositionData_t const_unpack_pds_SLQSPDSInjectPositionData_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_FIX_POSITION */
const unpack_pds_SLQSPDSDeterminePosition_t const_unpack_pds_SLQSPDSDeterminePosition_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_GET_GPS_STATE */
const unpack_pds_SLQSGetGPSStateInfo_t const_unpack_pds_SLQSGetGPSStateInfo_t = {
0x00,0x403e3,0x4035eb851f781672ULL,0x4054b1eb7fec1531ULL, 0x49851e20,0x00,0x45bb8000,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

    /* eQMI_PDS_SET_METHODS */
const unpack_pds_SLQSSetPositionMethodState_t const_unpack_pds_SLQSSetPositionMethodState_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_SET_EVENT */
const unpack_pds_SetEventReportCallback_t const_unpack_pds_SetEventReportCallback_t = {
0x00,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

    /* eQMI_PDS_EVENT_IND */
const unpack_pds_SetEventReport_Ind_t const_unpack_pds_SetEventReport_Ind_t = {
0x01,{"$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"},0x01,11.31000,0x01,48.07038,
0x01,0x00,0x01,0x01,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,16,18,19,28)}} };

/* eQMI_PDS_STATE_IND */
const unpack_pds_SetPdsState_Ind_t const_unpack_pds_SetPdsState_Ind_t = {
        0x01,0x01,0x02,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

int pds_validate_dummy_unpack()
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
    loopCount = sizeof(validate_pds_resp_msg)/sizeof(validate_pds_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_pds_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(ePDS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("PDS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("PDS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_PDS_GET_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_pds_GetPDSState_t *varp = &var_unpack_pds_GetPDSState_t;
                    const unpack_pds_GetPDSState_t *cstp = &const_unpack_pds_GetPDSState_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_GetPDSState,
                    dump_GetPDSState,
                    msg.buf,
                    rlen,
                    &var_unpack_pds_GetPDSState_t,
                    3,
                    CMP_PTR_TYPE, varp->pEnabledStatus, cstp->pEnabledStatus,
                    CMP_PTR_TYPE, varp->pTrackingStatus, cstp->pTrackingStatus,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetPdsState_Ind,
                    dump_SetPdsState_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetPdsState_Ind_t);
                }
                break;
                case eQMI_PDS_SET_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetPDSState,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetPDSState_t);
                }
                break;
            case eQMI_PDS_GET_DEFAULTS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_pds_GetPDSDefaults_t *varp = &var_unpack_pds_GetPDSDefaults_t;
                    const unpack_pds_GetPDSDefaults_t *cstp = &const_unpack_pds_GetPDSDefaults_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_GetPDSDefaults,
                    dump_GetPDSDefaults,
                    msg.buf,
                    rlen,
                    &var_unpack_pds_GetPDSDefaults_t,
                    5,
                    CMP_PTR_TYPE, varp->pOperation, cstp->pOperation,
                    CMP_PTR_TYPE, varp->pTimeout,   cstp->pTimeout,
                    CMP_PTR_TYPE, varp->pInterval,  cstp->pInterval,
                    CMP_PTR_TYPE, varp->pAccuracy,  cstp->pAccuracy,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                break;
            case eQMI_PDS_SET_DEFAULTS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetPDSDefaults,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetPDSDefaults_t);
                }
                break;
            case eQMI_PDS_GET_COM_AUTOTRACK:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_pds_GetPortAutomaticTracking_t *varp = &var_unpack_pds_GetPortAutomaticTracking_t;
                    const unpack_pds_GetPortAutomaticTracking_t *cstp = &const_unpack_pds_GetPortAutomaticTracking_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_GetPortAutomaticTracking,
                    dump_GetPortAutomaticTracking,
                    msg.buf,
                    rlen,
                    &var_unpack_pds_GetPortAutomaticTracking_t,
                    2,
                    CMP_PTR_TYPE, varp->pbAuto, cstp->pbAuto,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                break;
            case eQMI_PDS_SET_COM_AUTOTRACK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetPortAutomaticTracking,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetPortAutomaticTracking_t);
                }
                break;
            case eQMI_PDS_START_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_StartPDSTrackingSessionExt,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_StartPDSTrackingSessionExt_t);
                }
                break;
            case eQMI_PDS_END_SESSION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_StopPDSTrackingSession,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_StopPDSTrackingSession_t);
                }
                break;
            case eQMI_PDS_INJECT_TIME:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_PDSInjectTimeReference,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_PDSInjectTimeReference_t);
                }
                break;
            case eQMI_PDS_GET_XTRA_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    if(rsp_ctx.xid == 0x0A)
                    {
                        unpack_pds_GetXTRAAutomaticDownload_t *varp = &var_unpack_pds_GetXTRAAutomaticDownload_t;
                        const unpack_pds_GetXTRAAutomaticDownload_t *cstp = &const_unpack_pds_GetXTRAAutomaticDownload_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_pds_GetXTRAAutomaticDownload,
                        dump_GetXTRAAutomaticDownload,
                        msg.buf,
                        rlen,
                        &var_unpack_pds_GetXTRAAutomaticDownload_t,
                        3,
                        CMP_PTR_TYPE, varp->pbEnabled, cstp->pbEnabled,
                        CMP_PTR_TYPE, varp->pInterval, cstp->pInterval,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                    }
                    if(rsp_ctx.xid == 0x0C)
                    {
                        unpack_pds_GetXTRANetwork_t *varp = &var_unpack_pds_GetXTRANetwork_t;
                        const unpack_pds_GetXTRANetwork_t *cstp = &const_unpack_pds_GetXTRANetwork_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_pds_GetXTRANetwork,
                        dump_GetXTRANetwork,
                        msg.buf,
                        rlen,
                        &var_unpack_pds_GetXTRANetwork_t,
                        2,
                        CMP_PTR_TYPE, varp->pPreference, cstp->pPreference,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                    }
                    if(rsp_ctx.xid == 0x0E)
                    {
                        unpack_pds_GetXTRAValidity_t *varp = &var_unpack_pds_GetXTRAValidity_t;
                        const unpack_pds_GetXTRAValidity_t *cstp = &const_unpack_pds_GetXTRAValidity_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_pds_GetXTRAValidity,
                        dump_GetXTRAValidity,
                        msg.buf,
                        rlen,
                        &var_unpack_pds_GetXTRAValidity_t,
                        4,
                        CMP_PTR_TYPE, varp->pGPSWeek, cstp->pGPSWeek,
                        CMP_PTR_TYPE, varp->pGPSWeekOffset, cstp->pGPSWeekOffset,
                        CMP_PTR_TYPE, varp->pDuration, cstp->pDuration,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                    }
                }
                break;
            case eQMI_PDS_SET_XTRA_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    if(rsp_ctx.xid == 0x0B)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_pds_SetXTRAAutomaticDownload,
                        dump_common_resultcode,
                        msg.buf,
                        rlen,
                        &const_unpack_pds_SetXTRAAutomaticDownload_t);
                    }
                    if(rsp_ctx.xid == 0x0D)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_pds_SetXTRANetwork,
                        dump_common_resultcode,
                        msg.buf,
                        rlen,
                        &const_unpack_pds_SetXTRANetwork_t);
                    }
                }
                break;
            case eQMI_PDS_FORCE_XTRA_DL:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_ForceXTRADownload,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_ForceXTRADownload_t);
                }
                break;
            case eQMI_PDS_GET_SVC_AUTOTRACK:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_pds_GetServiceAutomaticTracking_t *varp = &var_unpack_pds_GetServiceAutomaticTracking_t;
                    const unpack_pds_GetServiceAutomaticTracking_t *cstp = &const_unpack_pds_GetServiceAutomaticTracking_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_GetServiceAutomaticTracking,
                    dump_GetServiceAutomaticTracking,
                    msg.buf,
                    rlen,
                    &var_unpack_pds_GetServiceAutomaticTracking_t,
                    2,
                    CMP_PTR_TYPE, varp->pbAuto, cstp->pbAuto,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                break;
            case eQMI_PDS_SET_SVC_AUTOTRACK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetServiceAutomaticTracking,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetServiceAutomaticTracking_t);
                }
                break;
            case eQMI_PDS_RESET_DATA:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_ResetPDSData,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_ResetPDSData_t);
                }
                break;
            case eQMI_PDS_SET_AGPS_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSSetAGPSConfig,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSSetAGPSConfig_t);
                }
                break;
            case eQMI_PDS_INJECT_ABS_TIME:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSPDSInjectAbsoluteTimeReference,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSPDSInjectAbsoluteTimeReference_t);
                }
                break;
            case eQMI_PDS_GET_AGPS_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_pds_SLQSGetAGPSConfig_t *varp = &var_unpack_pds_SLQSGetAGPSConfig_t;
                    const unpack_pds_SLQSGetAGPSConfig_t *cstp = &const_unpack_pds_SLQSGetAGPSConfig_t;
                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSGetAGPSConfig,
                    dump_SLQSGetAGPSConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_pds_SLQSGetAGPSConfig_t,
                    5,
                    CMP_PTR_TYPE, varp->pServerAddress, cstp->pServerAddress,
                    CMP_PTR_TYPE, varp->pServerPort, cstp->pServerPort,
                    CMP_PTR_TYPE, varp->pServerURL, cstp->pServerURL,
                    CMP_PTR_TYPE, varp->pServerURLLength, cstp->pServerURLLength,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,);
                }
                break;
            case eQMI_PDS_INJECT_POSITION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSPDSInjectPositionData,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSPDSInjectPositionData_t);
                }
                break;
            case eQMI_PDS_FIX_POSITION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSPDSDeterminePosition,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSPDSDeterminePosition_t);
                }
                break;
            case eQMI_PDS_GET_GPS_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSGetGPSStateInfo,
                    dump_SLQSGetGPSStateInfo,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSGetGPSStateInfo_t);
                }
                break;
            case eQMI_PDS_SET_METHODS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SLQSSetPositionMethodState,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SLQSSetPositionMethodState_t);
                }
                break;
            case eQMI_PDS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetEventReportCallback,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetEventReportCallback_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_pds_SetEventReport_Ind,
                    dump_SetEventReport_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_pds_SetEventReport_Ind_t);
                }
                break;
            default:
                printf("msgid 0x%x is unknown or not handled\n", rsp_ctx.msgid);
                break;
            }
        }
    }
    return 0;
}
