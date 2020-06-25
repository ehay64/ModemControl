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
#include "loc.h"

int loc_validate_dummy_unpack();

int loc=-1;
volatile int enLOCThread =0;
pthread_t loc_tid = 0;
pthread_attr_t loc_attr;
static char remark[255]={0};
static int unpackRetCode = 0;
static int iLocalLog = 1;

pack_loc_EventRegister_t LocEventRegister[]={
    {LOCEVENTMASKPOSITIONREPORT,0},
    {LOCEVENTMASKGNSSSVINFO,0},
    {LOCEVENTMASKNMEA,0},
    {LOCEVENTMASKNINOTIFYVERIFYREQ,0},
    {LOCEVENTMASKINJECTTIMEREQ,0},
    {LOCEVENTMASKINJECTPREDICTEDORBITSREQ,0},
    {LOCEVENTMASKINJECTPOSITIONREQ,0},
    {LOCEVENTMASKENGINESTATE,0},
    {LOCEVENTMASKFIXSESSIONSTATE,0},
    {LOCEVENTMASKWIFIREQ,0},
    {LOCEVENTMASKSENSORSTREAMINGREADYSTATUS,0},
    {LOCEVENTMASKTIMESYNCREQ,0},
    {LOCEVENTMASKSETSPISTREAMINGREPORT,0},
    {LOCEVENTMASKLOCATIONSERVERCONNECTIONREQ,0},
    {LOCEVENTMASKNIGEOFENCENOTIFICATION,0},
    {LOCEVENTMASKGEOFENCEGENALERT,0},
    {LOCEVENTMASKGEOFENCEBREACHNOTIFICATION,0},
    {LOCEVENTMASKPEDOMETERCONTROL,0},
    {LOCEVENTMASKMOTIONDATACONTROL,0},
    {LOCEVENTMASKBATCHFULLNOTIFICATION,0},
    {LOCEVENTMASKLIVEBATCHEDPOSITIONREPORT,0},
    {LOCEVENTMASKINJECTWIFIAPDATAREQ,0},
    {LOCEVENTMASKGEOFENCEBATCHBREACHNOTIFICATION,0},
    {LOCEVENTMASKVEHICLEDATAREADYSTATUS,0},
    {LOCEVENTMASKGNSSMEASUREMENTREPORT,0},
    {LOCEVENTMASKINVALIDVALUE,0}
};

pack_loc_SetExtPowerState_t LocSetExtPowerState[]={
     {0,0},//Device is not connected to an external power source
     {1,0},//Device is connected to an external power source
     {2,0},//Unknown external power state
     {3,0},//Fail Case
};
uint32_t LocStartappInterval =1000;
uint8_t LocStartappProvider[] = "ABC";
uint8_t LocStartappName[]     = "XYZ";
uint8_t LocStartappVer[]      = "12";

loc_LocApplicationInfo LocStartapp ={3, LocStartappProvider,3,LocStartappName,1,2,LocStartappVer };
uint32_t   LocStartTestLocRecurrence        = 1;
uint32_t   LocStartTestLocAccuracy          = 1;
uint32_t   LocStartintermediateReportState = 1;
uint32_t   LocStartconfigAltitudeAssumed   = 1;


pack_loc_Start_t LocStart[]={
    {
        0,//SessionId
        NULL,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        NULL,//MinIntervalTime;
        NULL,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
    {
        0,//SessionId
        NULL,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        NULL,//MinIntervalTime;
        &LocStartapp,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
    {
        0,//SessionId
        &LocStartTestLocRecurrence,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        NULL,//MinIntervalTime;
        NULL,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
    {
        0,//SessionId
        NULL,//pRecurrenceType;
        &LocStartTestLocAccuracy,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        NULL,//MinIntervalTime;
        NULL,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
    {
        0,//SessionId
        NULL,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        &LocStartintermediateReportState,//IntermediateReportState;
        NULL,//MinIntervalTime;
        NULL,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
     {
        0,//SessionId
        NULL,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        &LocStartappInterval,//MinIntervalTime;
        NULL,//ApplicationInfo;
        NULL,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
    {
        0,//SessionId
        NULL,//pRecurrenceType;
        NULL,//HorizontalAccuracyLvl;
        NULL,//IntermediateReportState;
        NULL,//MinIntervalTime;
        NULL,//ApplicationInfo;
        &LocStartconfigAltitudeAssumed,//ConfigAltitudeAssumed;
        0// Tlvresult;
    },
};

pack_loc_Stop_t LocStop[]={
        {
        0,//SessionId
        0// Tlvresult;
    },
};

pack_loc_SetOperationMode_t LopSetOperationMode[]={
    {
        1,//Default Mode
        0// Tlvresult;
    },
    {
        2,//MS based Mode
        0// Tlvresult;
    },
    {
        3,//MS assisted Mode
        0// Tlvresult;
    },
    {
        4,//MS standalone Mode
        0// Tlvresult;
    },
    {
        5,//Use Cell ID (GSM / UMTS)
        0// Tlvresult;
    },
    {
        6,//WWAN
        0// Tlvresult;
    },
};



loc_SV LocDeleteAssistDataSVInfoSv[1] = {{1,1,1}};
loc_SVInfo LocDeleteAssistDataSVInfo={1,&LocDeleteAssistDataSVInfoSv[0]};

loc_GnssData LocDeleteAssistDataGnssData={1};
loc_CellDb LocDeleteAssistDataCellDb={1};
loc_ClkInfo LocDeleteAssistDataClkInfo= {1};

loc_BdsSV LocDeleteAssistDataBdsSVInfoBdsSV[2] = {{201,1},{301,2}};
loc_BdsSVInfo LocDeleteAssistDataBdsSVInfo ={1,&LocDeleteAssistDataBdsSVInfoBdsSV[0]};


pack_loc_Delete_Assist_Data_t LocDeleteAssistData[]={
    { //Delete All
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0
    },
    { //Delete All
        &LocDeleteAssistDataSVInfo,
        NULL,
        NULL,
        NULL,
        NULL,
        0
    },
    { //Delete All
        NULL,
        &LocDeleteAssistDataGnssData,
        NULL,
        NULL,
        NULL,
        0
    },
    { //Delete All
        NULL,
        NULL,
        &LocDeleteAssistDataCellDb,
        NULL,
        NULL,
        0
    },
    { //Delete All
        NULL,
        NULL,
        NULL,
        &LocDeleteAssistDataClkInfo,//{1},
        NULL,
        0
    },
    { //Delete All
        NULL,
        NULL,
        NULL,
        NULL,
        &LocDeleteAssistDataBdsSVInfo,//{1,{201,1}},
        0
    },
};
pack_loc_SLQSLOCGetBestAvailPos_t LOCGetBestAvailPosReq= {0xaabbccdd,0};
pack_loc_SLQSLOCInjectUTCTime_t inject_time_pack = {0,10};
unpack_loc_SLQSLOCInjectUTCTime_t inject_time_unpack = {0, {{0}} };

pack_loc_SLQSLOCInjectPosition_t inject_pos_pack = {
    .has_latitude = 1, .latitude = 49.1725f,
    .has_longitude = 1, .longitude = -123.0713f,
    .has_horUncCircular = 1, .horUncCircular = 1.0f,
    .has_horConfidence = 1, .horConfidence = 99,
    .has_horReliability = 1, .horReliability = 4,
    .has_altitudeWrtEllipsoid = 1, .altitudeWrtEllipsoid = 0,
    .has_altitudeWrtMeanSeaLevel = 1, .altitudeWrtMeanSeaLevel = 0,
    .has_vertUnc = 1, .vertUnc = 1,
    .has_vertConfidence = 1, .vertConfidence = 99,
    .has_vertRelicability = 1, .vertReliability = 4,
    .has_altitudeSrcInfo = 1,
    .altitudeSrcInfo = {.source = 0, .linkage = 0, .coverage = 0},
    .has_timestampUtc = 1, .timestampUtc = 1,
    .has_timestampAge = 1, .timestampAge = 1,
    .has_positionSrc = 1, .positionSrc = 0,
    .has_rawHorUncCircular = 1, .rawHorUncCircular = 1,
    .has_rawHorConfidence = 1, .rawHorConfidence = 99
    };
unpack_loc_SLQSLOCInjectPosition_t inject_pos_unpack = { 0, {{0}} };

pack_loc_SLQSLOCSetCradleMountConfig_t cradle_mnt_pack;
unpack_loc_SLQSLOCSetCradleMountConfig_t cradle_mnt_unpack = { 0, {{0}} };

pack_loc_SLQSLOCInjectSensorData_t inject_sensor_pack;
unpack_loc_SLQSLOCInjectSensorData_t inject_sensor_unpack = {0, {{0}} };

unpack_loc_EventRegister_t      LocEventRegisterResp;
unpack_loc_SetExtPowerState_t   LocSetExtPowerStateResp;
unpack_loc_Start_t              LocStartResp;
unpack_loc_Stop_t               LocStopResp;
unpack_loc_SetOperationMode_t   LocSetOperationModeResp;
unpack_loc_Delete_Assist_Data_t LocDeleteAssistDataResp;
unpack_loc_SLQSLOCGetBestAvailPos_t LOCGetBestAvailPosResp;
unpack_loc_SLQSLOCGetOpMode_t   LocGetOperationModeResp;

pack_loc_SLQSLOCGetServer_t tpack_loc_SLQSLOCGetServer = {3,1,1};
unpack_loc_SLQSLOCGetServer_t tunpack_loc_SLQSLOCGetServer = {0, {{0}} };

loc_IPv4Config          IPv4Config = {0x4A7d44C0, 7276};
loc_IPv6Config          IPv6Config = {{0x2607, 0xF8B0, 0x4002, 0x080A, 0x0000, 0x0000,
        0x0000, 0x200E}, 7276};
loc_URLAddrInfo         URLAddr = {"supl.google.com"};
pack_loc_SLQSLOCSetServer_t tpack_loc_SLQSLOCSetServer = {3, &IPv4Config,
        &IPv6Config, &URLAddr};

swi_uint256_t unpack_loc_EventRegisterParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_loc_EventRegisterParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_loc_DeleteAssistDataParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_loc_DeleteAssistDataParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

unpack_loc_GetFixCriteria_t tunpack_wds_GetFixCriteria = SWI_INIT_UNPACK_RESULT_VALUE;


void dump_LOC_EventRegister(void * ptr)
{
    unpack_loc_EventRegister_t *result =
            (unpack_loc_EventRegister_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
   CHECK_WHITELIST_MASK(
       unpack_loc_EventRegisterParamPresenceMaskWhiteList,
       result->ParamPresenceMask);
   CHECK_MANDATORYLIST_MASK(
       unpack_loc_EventRegisterParamPresenceMaskMandatoryList,
       result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG));
    }
#endif
}

void dump_LOC_SetExtPowerState(void * ptr)
{
    unpack_loc_SetExtPowerState_t *result =
            (unpack_loc_SetExtPowerState_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_Start(void * ptr)
{
    unpack_loc_Start_t *result =
            (unpack_loc_Start_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_LOC_Stop(void * ptr)
{
    unpack_loc_Stop_t *result =
            (unpack_loc_Stop_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SetOperationMode(void * ptr)
{
    unpack_loc_SetOperationMode_t *result =
            (unpack_loc_SetOperationMode_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_DeleteAssistData(void * ptr)
{
    unpack_loc_DeleteAssistData_t *result =
            (unpack_loc_DeleteAssistData_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
   CHECK_WHITELIST_MASK(
       unpack_loc_DeleteAssistDataParamPresenceMaskWhiteList,
       result->ParamPresenceMask);
   CHECK_MANDATORYLIST_MASK(
       unpack_loc_DeleteAssistDataParamPresenceMaskMandatoryList,
       result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG));
    }
#endif
}

void dump_LOC_SLQSLOCGetBestAvailPos(void * ptr)
{
    unpack_loc_SLQSLOCGetBestAvailPos_t *result =
            (unpack_loc_SLQSLOCGetBestAvailPos_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCInjectUTCTime (void *ptr)
{
    unpack_loc_SLQSLOCInjectUTCTime_t *result =
            (unpack_loc_SLQSLOCInjectUTCTime_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCInjectPosition (void *ptr)
{
    UNUSEDPARAM(ptr);
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCSetCradleMountConfig (void *ptr)
{
    unpack_loc_SLQSLOCSetCradleMountConfig_t *result =
            (unpack_loc_SLQSLOCSetCradleMountConfig_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCInjectSensorData (void *ptr)
{
    unpack_loc_SLQSLOCInjectSensorData_t *result =
            (unpack_loc_SLQSLOCInjectSensorData_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCGetServer (void *ptr)
{
    UNUSEDPARAM(ptr);
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCSetServer (void *ptr)
{
    UNUSEDPARAM(ptr);
    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_EngineStateCallBack(void * ptr)
{
    unpack_loc_EngineState_Ind_t *result =
            (unpack_loc_EngineState_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Engine State :%d\n",result->engineState);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetExtPowerConfigCallBack(void * ptr)
{
    unpack_loc_SetExtPowerConfig_Ind_t *result =
            (unpack_loc_SetExtPowerConfig_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Set External Power Configure Status :%d\n",result->status);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_EventNMEACallBack(void * ptr)
{
    unpack_loc_EventNMEA_Ind_t *result =
            (unpack_loc_EventNMEA_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("LOC NMEA String :%s\n",result->NMEAData);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_SLQSLOCGetOpMode(void * ptr)
{
    unpack_loc_SLQSLOCGetOpMode_t *result =
            (unpack_loc_SLQSLOCGetOpMode_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_LOC_GetFixCriteria(void* ptr)
{
    unpack_loc_GetFixCriteria_t *result =
        (unpack_loc_GetFixCriteria_t*)ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf( "%s\n",__FUNCTION__ );
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

testitem_t oemapitestloc[] = {
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        NULL, 
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        NULL, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[0],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[1],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[2],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[3],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[4],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[5],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[6],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[7],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[8],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[9],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[10],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[11],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[12],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[13],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[14],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[15],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[16],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[17],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[18],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[19],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[20],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[21],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[22],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp,
        dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[23],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[24],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[25],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[0],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[1],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[2],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[3],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[4],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
       (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
       &LocDeleteAssistData[5],
       (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
       &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    }
};


testitem_t totestloc[] = {
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        NULL, 
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        NULL, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[0],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[1],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[2],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[3],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[4],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[5],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[6],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[7],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[8],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[9],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[10],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[11],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[12],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[13],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[14],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[15],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[16],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[17],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[18],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[19],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[20],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[21],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[22],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp,
        dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[23],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[24],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[25],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        &LocEventRegisterResp, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[0],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        &LocSetExtPowerStateResp, dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[0],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        &LocSetExtPowerStateResp, dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[1],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        &LocSetExtPowerStateResp, dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[2],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        &LocSetExtPowerStateResp,
        dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[3],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        &LocSetExtPowerStateResp, dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[0],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[1],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
        {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[2],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[3],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[4],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[5],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
        {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[6],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        &LocStartResp, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_Stop, "pack_loc_Stop",
        &LocStop[0],
        (unpack_func_item) &unpack_loc_Stop, "unpack_loc_Stop",
        &LocStopResp, dump_LOC_Stop
    },
    {
        (pack_func_item) &pack_loc_Stop, "pack_loc_Stop",
        &LocStop[1],
        (unpack_func_item) &unpack_loc_Stop, "unpack_loc_Stop",
        &LocStopResp, dump_LOC_Stop
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[0],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[1],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[2],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[3],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[4],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[5],
        (unpack_func_item) &unpack_loc_SetOperationMode, "unpack_loc_SetOperationMode",
        &LocSetOperationModeResp, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[0],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[1],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[2],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[3],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[4],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
       (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
       &LocDeleteAssistData[5],
       (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
       &LocDeleteAssistDataResp, dump_LOC_DeleteAssistData
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCInjectUTCTime, "pack_loc_SLQSLOCInjectUTCTime",
       &inject_time_pack,
       (unpack_func_item) &unpack_loc_SLQSLOCInjectUTCTime, "unpack_loc_SLQSLOCInjectUTCTime",
       &inject_time_unpack, dump_LOC_SLQSLOCInjectUTCTime
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCInjectPosition, "pack_loc_SLQSLOCInjectPosition",
       &inject_pos_pack,
       (unpack_func_item) &unpack_loc_SLQSLOCInjectPosition, "unpack_loc_SLQSLOCInjectPosition",
       &inject_pos_unpack, dump_LOC_SLQSLOCInjectPosition
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCSetCradleMountConfig, "pack_loc_SLQSLOCSetCradleMountConfig",
       &cradle_mnt_pack,
       (unpack_func_item) &unpack_loc_SLQSLOCSetCradleMountConfig, "unpack_loc_SLQSLOCSetCradleMountConfig",
       &cradle_mnt_unpack, dump_LOC_SLQSLOCSetCradleMountConfig
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCInjectSensorData, "pack_loc_SLQSLOCInjectSensorData",
       &inject_sensor_pack,
       (unpack_func_item) &unpack_loc_SLQSLOCInjectSensorData, "unpack_loc_SLQSLOCInjectSensorData",
       &inject_sensor_unpack, dump_LOC_SLQSLOCInjectSensorData
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCGetBestAvailPos, "pack_loc_SLQSLOCGetBestAvailPos",
       &LOCGetBestAvailPosReq,
       (unpack_func_item) &unpack_loc_SLQSLOCGetBestAvailPos, "unpack_loc_SLQSLOCGetBestAvailPos",
       &LOCGetBestAvailPosResp, dump_LOC_SLQSLOCGetBestAvailPos
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCGetServer, "pack_loc_SLQSLOCGetServer",
       &tpack_loc_SLQSLOCGetServer,
       (unpack_func_item) &unpack_loc_SLQSLOCGetServer, "unpack_loc_SLQSLOCGetServer",
       &tunpack_loc_SLQSLOCGetServer, dump_LOC_SLQSLOCGetServer
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCSetServer, "pack_loc_SLQSLOCSetServer",
       &tpack_loc_SLQSLOCSetServer,
       (unpack_func_item) &unpack_loc_SLQSLOCSetServer, "unpack_loc_SLQSLOCSetServer",
       NULL, dump_LOC_SLQSLOCSetServer
    },
    {
        (pack_func_item) &pack_loc_SLQSLOCGetOpMode, "pack_loc_SLQSLOCGetOpMode",
        NULL,
        (unpack_func_item) &unpack_loc_SLQSLOCGetOpMode, "unpack_loc_SLQSLOCGetOpMode",
        &LocGetOperationModeResp, dump_LOC_SLQSLOCGetOpMode
    },
    {
       (pack_func_item) &pack_loc_GetFixCriteria, "pack_loc_GetFixCriteria",
       NULL,
       (unpack_func_item) &unpack_loc_GetFixCriteria, "unpack_loc_GetFixCriteria",
       &tunpack_wds_GetFixCriteria, dump_LOC_GetFixCriteria
    },
};

unsigned int locarraylen = (unsigned int)((sizeof(totestloc))/(sizeof(totestloc[0])));

testitem_t loctotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_loc_EventRegister, "pack_loc_EventRegister",
        &LocEventRegister[0],
        (unpack_func_item) &unpack_loc_EventRegister, "unpack_loc_EventRegister",
        NULL, dump_LOC_EventRegister
    },
    {
        (pack_func_item) &pack_loc_SetExtPowerState, "pack_loc_SetExtPowerState",
        &LocSetExtPowerState[0],
        (unpack_func_item) &unpack_loc_SetExtPowerState, "unpack_loc_SetExtPowerState",
        NULL, dump_LOC_SetExtPowerState
    },
    {
        (pack_func_item) &pack_loc_Start, "pack_loc_Start",
        &LocStart[0],
        (unpack_func_item) &unpack_loc_Start, "unpack_loc_Start",
        NULL, dump_LOC_Start
    },
    {
        (pack_func_item) &pack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        &LopSetOperationMode[0],
        (unpack_func_item) &unpack_loc_SetOperationMode, "pack_loc_SetOperationMode",
        NULL, dump_LOC_SetOperationMode
    },
    {
        (pack_func_item) &pack_loc_DeleteAssistData, "pack_loc_DeleteAssistData",
        &LocDeleteAssistData[0],
        (unpack_func_item) &unpack_loc_DeleteAssistData, "unpack_loc_DeleteAssistData",
        NULL, dump_LOC_DeleteAssistData
    },
    {
       (pack_func_item) &pack_loc_SLQSLOCGetBestAvailPos, "pack_loc_SLQSLOCGetBestAvailPos",
       &LOCGetBestAvailPosReq,
       (unpack_func_item) &unpack_loc_SLQSLOCGetBestAvailPos, "unpack_loc_SLQSLOCGetBestAvailPos",
       NULL, dump_LOC_SLQSLOCGetBestAvailPos
    },
    {
       (pack_func_item) &pack_loc_GetFixCriteria, "pack_loc_GetFixCriteria",
       NULL,
       (unpack_func_item) &unpack_loc_GetFixCriteria, "unpack_loc_GetFixCriteria",
       NULL, dump_LOC_GetFixCriteria
    },
};



void dump_PositionRpt_Ind(unpack_loc_PositionRpt_Ind_t Resp)
{
    printf("PositionRptInd Tlv result:%d\n",Resp.Tlvresult);
    swi_uint256_print_mask (Resp.ParamPresenceMask);

    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 2))
        printf("\tsessionId:%d\n",Resp.sessionId);
    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
        printf("\tsessionStatus:%d\n",(int)Resp.sessionStatus);
    if( (Resp.pLatitude!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
        printf("\tLatitude:%lf\n", *(double*)(Resp.pLatitude));
    if( (Resp.pLongitude!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 17)) )
        printf("\tLongitude:%lf\n", *(double*)(Resp.pLongitude));
    if( (Resp.pHorUncCircular!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 18)) )
        printf("\tHorUncCircular:%"PRIu32"\n",*(Resp.pHorUncCircular));
    if( (Resp.pHorUncEllipseSemiMinor!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 19)) )
        printf("\tHorUncEllipseSemiMinor:%"PRIu32"\n",*(Resp.pHorUncEllipseSemiMinor));
    if( (Resp.pHorUncEllipseSemiMajor!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 20)) )
        printf("\tHorUncEllipseSemiMajor:%"PRIu32"\n",*(Resp.pHorUncEllipseSemiMajor));
    if( (Resp.pHorUncEllipseOrientAzimuth!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 21)) )
        printf("\tHorUncEllipseOrientAzimuth:%"PRIu32"\n",*(Resp.pHorUncEllipseOrientAzimuth));
    if( (Resp.pHorConfidence!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 22)) )
        printf("\tHorConfidence:%d\n",(int)*(Resp.pHorConfidence));
    if( (Resp.pHorReliability!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 23)) )
        printf("\tHorReliability:%"PRIu32"\n",*(Resp.pHorReliability));
    if( (Resp.pSpeedHorizontal!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 24)) )
        printf("\tSpeedHorizontal:%"PRIu32"\n",*(Resp.pSpeedHorizontal));
    if( (Resp.pSpeedUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 25)) )
        printf("\tSpeedUnc:%"PRIu32"\n",*(Resp.pSpeedUnc));
    if( (Resp.pAltitudeWrtEllipsoid!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 26)) )
        printf("\tAltitudeWrtEllipsoid:%"PRIu32"\n",*(Resp.pAltitudeWrtEllipsoid));
    if( (Resp.pAltitudeWrtMeanSeaLevel!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 27)) )
        printf("\tAltitudeWrtMeanSeaLevel:%"PRIu32"\n",*(Resp.pAltitudeWrtMeanSeaLevel));
    if( (Resp.pVertUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 28)) )
        printf("\tVertUnc:%"PRIu32"\n",*(Resp.pVertUnc));
    if( (Resp.pVertConfidence!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 29)) )
        printf("\tVertConfidence:%"PRIu32"\n",*(Resp.pVertConfidence));
    if( (Resp.pVertReliability!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 30)) )
        printf("\tVertReliability:%"PRIu32"\n",*(Resp.pVertReliability));
    if( (Resp.pSpeedVertical!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 31)) )
        printf("\tSpeedVertical:%"PRIu32"\n",*(Resp.pSpeedVertical));
    if( (Resp.pHeading!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 32)) )
        printf("\tHeading:%"PRIu32"\n",*(Resp.pHeading));
    if( (Resp.pHeadingUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 33)) )
        printf("\tHeadingUnc:%"PRIu32"\n",*(Resp.pHeadingUnc));
    if( (Resp.pMagneticDeviation!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 34)) )
        printf("\tMagneticDeviation:%"PRIu32"\n",*(Resp.pMagneticDeviation));
    if( (Resp.pTechnologyMask!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 35)) )
        printf("\tTechnologyMask:%"PRIu32"\n",*(Resp.pTechnologyMask));
    if( (Resp.pTimestampUtc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 37)) )
        printf("\tTimestampUtc:%"PRIu64"\n",*(Resp.pTimestampUtc));
    if( (Resp.pLeapSeconds!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 38)) )
        printf("\tLeapSeconds:%"PRIu32"\n",*(Resp.pLeapSeconds));
    if( (Resp.pTimeUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 40)) )
        printf("\tTimeUnc:%"PRIu32"\n",*(Resp.pTimeUnc));
    if( (Resp.pTimeSrc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 41)) )
        printf("\tTimeSrc:%"PRIu32"\n",*(Resp.pTimeSrc));
    if( (Resp.pFixId!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 43)) )
        printf("\tFixId:%"PRIu32"\n",*(Resp.pFixId));
    if( (Resp.pAltitudeAssumed!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 45)) )
        printf("\tAltitudeAssumed:%"PRIu32"\n",*(Resp.pAltitudeAssumed));
    if( (Resp.pPrecisionDilution!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 36)) )
    {
        printf("\tPrecisionDilution\n");
        printf("\tHDOP:%"PRIu32"\n",Resp.pPrecisionDilution->HDOP);
        printf("\tPDOP:%"PRIu32"\n",Resp.pPrecisionDilution->PDOP);
        printf("\tVDOP:%"PRIu32"\n",Resp.pPrecisionDilution->VDOP);
    }
    if( (Resp.pGpsTime!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 39)) )
    {
        printf("\tGPS Time\n");
        printf("\tgpsTimeOfWeekMs:%d\n",Resp.pGpsTime->gpsTimeOfWeekMs);
        printf("\tgpsWeek:%d\n",Resp.pGpsTime->gpsWeek);
    }
    if( (Resp.pSensorDataUsage!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 42)) )
    {
        printf("\tSensorDataUsage\n");
        printf("\taidingIndicatorMask:%d\n",Resp.pSensorDataUsage->aidingIndicatorMask);
        printf("\tusageMask:%d\n",Resp.pSensorDataUsage->usageMask);
    }

    if( (Resp.pSvUsedforFix!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 44)) )
    {
        int i = 0;
        printf("\tSvUsedforFix\n");
        printf("\tSvUsedList Length:%d\n",Resp.pSvUsedforFix->gnssSvUsedList_len);
        for(i =0;i<Resp.pSvUsedforFix->gnssSvUsedList_len;i++)
            printf("\tgnssSvUsedList(%d):%d\n",i,Resp.pSvUsedforFix->gnssSvUsedList[i]);

    }
    
}

void dump_Position_Rpt_Ind(unpack_loc_PositionRpt_Ind_t *Resp)
{

    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    dump_PositionRpt_Ind(*Resp);
}

void dump_BestAvailPos_Ind(unpack_loc_BestAvailPos_Ind_t Resp)
{
    printf("BestAvailPosInd\n");
    swi_uint256_print_mask (Resp.ParamPresenceMask);
    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
        printf("\tsessionStatus:%d\n",(int)Resp.status);
    if( (Resp.pXid!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
        printf("\tTransaction ID : 0x%x\n",(int)*(Resp.pXid));
    if( (Resp.pLatitude!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 17)) )
        printf("\tLatitude:%lf\n", *(double*)(Resp.pLatitude));
    if( (Resp.pLongitude!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 18)) )
        printf("\tLongitude:%lf\n", *(double*)(Resp.pLongitude));
    if( (Resp.pHorUncCircular!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 19)) )
        printf("\tHorUncCircular:%f\n",*(float*)(Resp.pHorUncCircular));
    if( (Resp.pAltitudeWrtEllipsoid!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 20)) )
        printf("\tAltitudeWrtEllipsoid:%f\n",*(float*)(Resp.pAltitudeWrtEllipsoid));
    if( (Resp.pVertUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 21)) )
        printf("\tVertUnc:%f\n",*(float*)(Resp.pVertUnc));
    if( (Resp.pTimestampUtc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 22)) )
        printf("\tTimestampUtc:%"PRIu64"\n",*(Resp.pTimestampUtc));
    if( (Resp.pTimeUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 23)) )
        printf("\tTimeUnc:%"PRIu32"\n",*(Resp.pTimeUnc));
    if( (Resp.pHorUncEllipseSemiMinor!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 24)) )
        printf("\tHorUncEllipseSemiMinor:%f\n",*(float*)(Resp.pHorUncEllipseSemiMinor));
    if( (Resp.pHorUncEllipseSemiMajor!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 25)) )
        printf("\tHorUncEllipseSemiMajor:%f\n",*(float*)(Resp.pHorUncEllipseSemiMajor));
    if( (Resp.pHorUncEllipseOrientAzimuth!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 26)) )
        printf("\tHorUncEllipseOrientAzimuth:%f\n",*(float*)(Resp.pHorUncEllipseOrientAzimuth));
    if( (Resp.pHorCirConf!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 27)) )
        printf("\tHorCircularConfidence:%d\n",(int)*(Resp.pHorCirConf));
    if( (Resp.pHorEllpConf!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 28)) )
        printf("\tHorEllipticalConfidence :%d\n",(int)*(Resp.pHorEllpConf));
    if( (Resp.pHorReliability!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 29)) )
        printf("\tHorReliability:%"PRIu32"\n",*(Resp.pHorReliability));
    if( (Resp.pSpeedHorizontal!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 30)) )
        printf("\tSpeedHorizontal:%f\n",*(float*)(Resp.pSpeedHorizontal));
    if( (Resp.pSpeedUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 31)) )
        printf("\tSpeedUnc:%f\n",*(float*)(Resp.pSpeedUnc));
    if( (Resp.pAltitudeWrtMeanSeaLevel!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 32)) )
        printf("\tAltitudeWrtMeanSeaLevel:%f\n",*(float*)(Resp.pAltitudeWrtMeanSeaLevel));
    if( (Resp.pVertConfidence!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 33)) )
        printf("\tVertConfidence:%"PRIu32"\n",*(Resp.pVertConfidence));
    if( (Resp.pVertReliability!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 34)) )
        printf("\tVertReliability:%"PRIu32"\n",*(Resp.pVertReliability));
    if( (Resp.pSpeedVertical!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 35)) )
        printf("\tSpeedVertical:%f\n",*(float*)(Resp.pSpeedVertical));
    if( (Resp.pSpeedVerticalUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 36)) )
        printf("\tSpeedVerticalUnc:%f\n",*(float*)(Resp.pSpeedVerticalUnc));
    if( (Resp.pHeading!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 37)) )
        printf("\tHeading:%f\n",*(float*)(Resp.pHeading));
    if( (Resp.pHeadingUnc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 38)) )
        printf("\tHeadingUnc:%f\n",*(float*)(Resp.pHeadingUnc));
    if( (Resp.pMagneticDeviation!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 39)) )
        printf("\tMagneticDeviation:%f\n",*(float*)(Resp.pMagneticDeviation));
    if( (Resp.pTechnologyMask!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 40)) )
        printf("\tTechnologyMask:%"PRIu32"\n",*(Resp.pTechnologyMask));
    if( (Resp.pPrecisionDilution!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 41)) )
    {
        printf("\tPrecisionDilution\n");
        printf("\tHDOP:%f\n",(float)Resp.pPrecisionDilution->HDOP);
        printf("\tPDOP:%f\n",(float)Resp.pPrecisionDilution->PDOP);
        printf("\tVDOP:%f\n",(float)Resp.pPrecisionDilution->VDOP);
    }
    if( (Resp.pGpsTime!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 42)) )
    {
        printf("\tGPS Time\n");
        printf("\tgpsTimeOfWeekMs:%d\n",Resp.pGpsTime->gpsTimeOfWeekMs);
        printf("\tgpsWeek:%d\n",Resp.pGpsTime->gpsWeek);
    }
    if( (Resp.pTimeSrc!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 43)) )
        printf("\tTimeSrc:%"PRIu32"\n",*(Resp.pTimeSrc));
    if( (Resp.pSensorDataUsage!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 44)) )
    {
        printf("\tSensorDataUsage\n");
        printf("\taidingIndicatorMask:%d\n",Resp.pSensorDataUsage->aidingIndicatorMask);
        printf("\tusageMask:%d\n",Resp.pSensorDataUsage->usageMask);
    }
    if( (Resp.pSvUsedforFix!=NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 45)) )
    {
        int i = 0;
        printf("\tSvUsedforFix\n");
        printf("\tSvUsedList Length:%d\n",Resp.pSvUsedforFix->gnssSvUsedList_len);
        for(i =0;i<Resp.pSvUsedforFix->gnssSvUsedList_len;i++)
            printf("\tgnssSvUsedList(%d):%d\n",i,Resp.pSvUsedforFix->gnssSvUsedList[i]);
    }
}


void dump_BestAvail_Pos_Ind(unpack_loc_BestAvailPos_Ind_t *Resp)
{
    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
   dump_BestAvailPos_Ind(*Resp);
}

void dump_GnssSvInfo_Ind(unpack_loc_GnssSvInfo_Ind_t Resp)
{
    uint8_t i;
    printf("\nLoc GNSS SV Info:\n");
    swi_uint256_print_mask (Resp.ParamPresenceMask);
    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
        printf("Altitude Assumed:%d\n", Resp.altitudeAssumed);
    if ( (Resp.pSatelliteInfo != NULL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
    {
        printf("\nLoc Satellite Info:\n");
        printf("\nNumber of sets of the following elements: %d\n", Resp.pSatelliteInfo->svListLen);
        for ( i = 0; i < Resp.pSatelliteInfo->svListLen; i++ )
        {
            printf("\nInstance[%d]\n", i);
            printf("Valid Mask: 0x%08x\n", Resp.pSatelliteInfo[i].validMask);
            printf("System: %u\n", Resp.pSatelliteInfo[i].system);
            printf("GNSS SV ID: %d\n", Resp.pSatelliteInfo[i].gnssSvId);
            printf("Health Status: %d\n", Resp.pSatelliteInfo[i].healthStatus);
            printf("SV Status: %u\n", Resp.pSatelliteInfo[i].svStatus);
            printf("SV Info Mask: %d\n", Resp.pSatelliteInfo[i].svInfoMask);
            printf("Elevation: %f\n", Resp.pSatelliteInfo[i].elevation);
            printf("Azimuth: %f\n", Resp.pSatelliteInfo[i].azimuth);
            printf("SNR: %f\n", Resp.pSatelliteInfo[i].snr);
        }
    }
}

void dump_GnssSv_Info_Ind(unpack_loc_GnssSvInfo_Ind_t *Resp)
{
    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    dump_GnssSvInfo_Ind(*Resp);
}

void dump_InjectSensorDataCallback_Ind(
    unpack_loc_InjectSensorDataCallback_Ind_t Resp)
{
    swi_uint256_print_mask (Resp.ParamPresenceMask);
    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
        printf("Inject Sensor Data Status: %d\n",
            Resp.injectSensorDataStatus);
    if ( (Resp.pOpaqueIdentifier) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
         printf("\nOpaque Identifier: %d\n",
             *(Resp.pOpaqueIdentifier));
    if ( (Resp.pAccelSamplesAccepted) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 17)) )
     printf("\nAccel Samples Accepted: %d\n",
             *(Resp.pAccelSamplesAccepted));
    if ( (Resp.pGyroSamplesAccepted) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 18)) )
     printf("\nGyro Samples Accepted: %d\n",
             *(Resp.pGyroSamplesAccepted));
    if ( (Resp.pAccelTempSamplesAccepted) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 19)) )
     printf("\nAccel Temp Samples Acceptedr: %d\n",
             *(Resp.pAccelTempSamplesAccepted));
    if ( (Resp.pGyroTempSamplesAccepted) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 20)) )
     printf("\nGyro Temp Samples Accepted: %d\n",
             *(Resp.pGyroTempSamplesAccepted));
}

void dump_InjectSensorData_Callback_Ind(unpack_loc_InjectSensorDataCallback_Ind_t *Resp)
{
    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    dump_InjectSensorDataCallback_Ind(*Resp);
}

void dump_GetServer_Ind(
    unpack_loc_GetServer_Ind_t Resp)
{
    uint16_t lcount = 0;
    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
        printf( "\nServer Status: %d\n",
                Resp.serverStatus);

    if(swi_uint256_get_bit (Resp.ParamPresenceMask, 2))
        printf( "\nServer Type: %d\n",
                Resp.serverType);
    if ( (Resp.pIPv4AddrInfo ) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
    {
        if ( Resp.pIPv4AddrInfo->TlvPresent)
        {
            printf( "\tIPV4 Address : %d\n", Resp.pIPv4AddrInfo->address);
            printf( "\tIPV4 Port : %d\n", Resp.pIPv4AddrInfo->port);
        }
    }
    if ( ( Resp.pIPv6AddrInfo ) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 17)) )
    {
        if ( Resp.pIPv6AddrInfo->TlvPresent)
        {
            printf( "\tIPV6 Address ");
            for (lcount=0; lcount < 8; lcount++)
                printf( "%d :", Resp.pIPv6AddrInfo->address[lcount]);
            printf( "\n");
            printf("\tIPV6 Port : %d\n", Resp.pIPv6AddrInfo->port);
        }
    }
    if ( ( Resp.pURL) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 18)) )
    {
        if ( Resp.pURL->TlvPresent)
        {
            printf("\tURL Address : %s\n", Resp.pURL->address);
        }
    }
}
void dump_Get_Server_Ind(unpack_loc_GetServer_Ind_t *Resp)
{
    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    dump_GetServer_Ind(*Resp);
}

void dump_SensorStreamingCallback_Ind( 
    unpack_loc_SensorStreamingCallback_Ind_t Resp)
{
    swi_uint256_print_mask (Resp.ParamPresenceMask);
    if ( (Resp.pAccelAcceptReady) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 16)) )
    {
         printf("\nAccel Inject Enable: %d\n",
             Resp.pAccelAcceptReady->injectEnable);
         printf("\nAccel SamplesPerBatch: %d\n",
             Resp.pAccelAcceptReady->samplesPerBatch);
         printf("\nAccel BatchesPerSecond: %d\n",
              Resp.pAccelAcceptReady->batchPerSec);
    }
    if ( (Resp.pGyroAcceptReady) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 17)) )
    {
         printf("\nGyro Inject Enable: %d\n",
             Resp.pGyroAcceptReady->injectEnable);
         printf("\nGyro SamplesPerBatch: %d\n",
             Resp.pGyroAcceptReady->samplesPerBatch);
         printf("\nGyro BatchesPerSecond: %d\n",
              Resp.pGyroAcceptReady->batchPerSec);
                          }
    if ( (Resp.pAccelTempAcceptReady) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 18)) )
    {
         printf("\nAccel Temp. Enable: %d\n",
             Resp.pAccelTempAcceptReady->injectEnable);
         printf("\nAccel Temp. SamplesPerBatch: %d\n",
             Resp.pAccelTempAcceptReady->samplesPerBatch);
         printf("\nAccel Temp. BatchesPerSecond: %d\n",
              Resp.pAccelTempAcceptReady->batchPerSec);
    }
    if ( (Resp.pGyroTempAcceptReady) && (swi_uint256_get_bit (Resp.ParamPresenceMask, 19)) )
    {
        printf("\nGyro Temp. Enable: %d\n",
            Resp.pGyroTempAcceptReady->injectEnable);
        printf("\nGyro Temp. SamplesPerBatch: %d\n",
            Resp.pGyroTempAcceptReady->samplesPerBatch);
        printf("\nGyro Temp. BatchesPerSecond: %d\n",
            Resp.pGyroTempAcceptReady->batchPerSec);
    }
}

void dump_SensorStreaming_Callback_Ind(unpack_loc_SensorStreamingCallback_Ind_t *Resp)
{
    if(Resp == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    dump_SensorStreamingCallback_Ind(*Resp);
}

void dump_SetOperationMode_Ind( void * ptr)
{
    unpack_loc_SetOperationMode_Ind_t *result =
            (unpack_loc_SetOperationMode_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Set Operation Mode Indication Status :%d\n",result->status);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_DeleteAssistData_Ind( void * ptr)
{
    unpack_loc_DeleteAssistData_Ind_t *result =
            (unpack_loc_DeleteAssistData_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("Delete Assist Data Indication Status :%d\n",result->status);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_CradleMountCallback_Ind( void * ptr)
{
    unpack_loc_CradleMountCallback_Ind_t *result =
            (unpack_loc_CradleMountCallback_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1)) {
        printf("LOC Cradle Mount Config: %d\n",
                result->cradleMountConfigStatus);
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_EventTimeSyncCallback_Ind( void * ptr)
{
    unpack_loc_EventTimeSyncCallback_Ind_t *result =
            (unpack_loc_EventTimeSyncCallback_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1)) {
        printf("LOC Time Sync Ref Counter: %d\n",
                result->timeSyncRefCounter);
    }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_InjectTimeSyncDataCallback_Ind( void * ptr)
{
    unpack_loc_InjectTimeSyncDataCallback_Ind_t *result =
            (unpack_loc_InjectTimeSyncDataCallback_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1)) {
        printf("LOC Inject Time Sync Status: %d\n",
                result->injectTimeSyncStatus);
   }

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_InjectUTCTimeCallback_Ind( void * ptr)
{
    unpack_loc_InjectUTCTimeCallback_Ind_t *result =
            (unpack_loc_InjectUTCTimeCallback_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("LOC Inject UTC Time Status: %d\n",
                result->status);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_InjectPositionCallback_Ind( void * ptr)
{
    unpack_loc_InjectPositionCallback_Ind_t *result =
            (unpack_loc_InjectPositionCallback_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf("LOC Inject Position Status: %d\n",
                result->status);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetServer_Ind( void * ptr)
{
    unpack_loc_SetServer_Ind_t *result =
            (unpack_loc_SetServer_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "\nServer Status: %d\n", result->serverStatus);

    if(iLocalLog==0)
      return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_Get_OpMode_Ind( void *ptr)
{
    unpack_loc_GetOpMode_Ind_t *result =
            (unpack_loc_GetOpMode_Ind_t *)ptr;

    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Status: %d\n", result->Status);
    if ( (result->pMode ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf( "Operation Mode : %x\n", *(result->pMode));
}

void dump_GetFixCriteria_ind(void *ptr)
{
    unpack_loc_FixCriteria_Ind_t *result =
            (unpack_loc_FixCriteria_Ind_t *)ptr;

    if(ptr == NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        printf( "Get Fix Criteria Status: %d\n", result->loc_fixCriteriaStatus.status);
    if( swi_uint256_get_bit (result->ParamPresenceMask, 16))
        printf( "Get Fix Criteria Accuracy: %d\n", (int) result->loc_horAccuracy.accuracy);
    if( swi_uint256_get_bit (result->ParamPresenceMask, 17))
        printf( "Intermediate Report status: %d\n", (int) result->loc_intermediateRptState.intermediate);
    if( swi_uint256_get_bit (result->ParamPresenceMask, 18))
        printf( "Time interval between Fixes: %d\n", (int) result->loc_minInterval.interval);
    if( swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        uint8_t i = 0;
        printf("Application Provider Len: %d\n", result->loc_appProviderInfo.providerLen);
        printf("Application Provider : ");
        for ( i = 0 ; i < result->loc_appProviderInfo.providerLen ; i++ )
        {
            printf("%c",result->loc_appProviderInfo.provider[i]);
        }
        printf("\n");

        printf( "Application Name Len:  %d\n", result->loc_appProviderInfo.nameLen);
        printf( "Application Name : ");
        for ( i = 0 ; i < result->loc_appProviderInfo.nameLen ; i++ )
        {
            printf("%c",result->loc_appProviderInfo.name[i]);
        }
        printf("\n");

        printf( "Application version valid: %d \n", result->loc_appProviderInfo.verValid );
        printf( "Application Version Len:  %d\n", result->loc_appProviderInfo.versionLen);
        printf( "Application version : ");
        for ( i = 0 ; i < result->loc_appProviderInfo.versionLen ; i++ )
        {
            printf("%c",result->loc_appProviderInfo.version[i]);
        }
        printf( "\n");
    }
}

void *loc_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;

    printf("%s param %p\n", __func__, ptr);
    
    while(enLOCThread)
    {
        if(loc<1)
        {
            usleep(1000);
            printf("%s fd error\n", __func__);
            continue;
        }
        //TODO select multiple file and read them
        rtn = read(loc, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eLOC, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t)rtn, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("LOC IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("LOC RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);

            switch(rsp_ctx.msgid)
            {
                case eQMI_LOC_START:
                    if (rsp_ctx.type == eIND)
                    {
                        printf("QMI_LOC_START IND result :%d\n",rtn);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_Start_t loc_Start;
                        memset(&loc_Start,0,sizeof(unpack_loc_Start_t));
                        unpackRetCode = unpack_loc_Start( msg.buf, QMI_MSG_MAX ,&loc_Start);
                        printf("QMI_LOC_START RSP result :%d\n",unpackRetCode);
                    }
                    break;
                case eQMI_LOC_EVENT_POSITION_REPORT_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_PositionRpt_Ind_t Resp;
                        loc_gpsTime GpsTime;
                        loc_precisionDilution precisionDilution;
                        loc_svUsedforFix svUsedforFix;
                        loc_sensorDataUsage sensorDataUsage;
                        
                        memset(&Resp,0,sizeof(unpack_loc_PositionRpt_Ind_t));
                        Resp.pGpsTime = &GpsTime;
                        Resp.pPrecisionDilution = &precisionDilution;
                        Resp.pSvUsedforFix = &svUsedforFix;
                        Resp.pSensorDataUsage = &sensorDataUsage;
                        Resp.sessionStatus = ~0;
                        /* Unpack the WDS Event Indication */
                        unpackRetCode = unpack_loc_PositionRpt_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        if(Resp.sessionStatus <= eQMI_LOC_SESS_STATUS_IN_PROGRESS)
                        {
                            dump_PositionRpt_Ind(Resp);
                        }
                        printf("sessionStatus :%d\n",Resp.sessionStatus);
                        printf("QMI_LOC_EVENT_POSITION_REPORT_IND IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        printf("QMI_LOC_EVENT_POSITION_REPORT_IND RSP result :%d\n",unpackRetCode);
                    }
                    break;
                case eQMI_LOC_EVENT_GNSS_SV_INFO_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_GnssSvInfo_Ind_t Resp;

                        memset(&Resp,0,sizeof(unpack_loc_GnssSvInfo_Ind_t));

                        /* Unpack the LOC EVENT GNSS Info Indication */
                        unpackRetCode = unpack_loc_GnssSvInfo_Ind( msg.buf,QMI_MSG_MAX,&Resp );

                        dump_GnssSvInfo_Ind(Resp);

                        printf("QMI_LOC_EVENT_GNSS_SV_INFO_IND IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        printf("QMI_LOC_EVENT_GNSS_SV_INFO_IND RSP result :%d\n",unpackRetCode);
                    }
                    break;

                case eQMI_LOC_EVENT_ENGINE_STATE_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_EngineState_Ind_t Resp;

                        memset(&Resp,0,sizeof(unpack_loc_EngineState_Ind_t));
                        /* Unpack the LOC Engine State Indication */
                        unpackRetCode = unpack_loc_EngineState_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        swi_uint256_print_mask (Resp.ParamPresenceMask);
                        if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
                            printf("Engine State :%d\n",Resp.engineState);
                        printf("QMI_LOC_EVENT_ENGINE_STATE_IND result :%d\n",unpackRetCode);
                    }
                    break;
                case eQMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND:
                        if (rsp_ctx.type == eIND)
                        {
                            unpack_loc_SetExtPowerConfig_Ind_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_SetExtPowerConfig_Ind_t));
                            /* Unpack the LOC Set External Configure State Indication */
                            unpackRetCode = unpack_loc_SetExtPowerConfig_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                            swi_uint256_print_mask (Resp.ParamPresenceMask);
                            if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
                                printf("Set External Power Configure Status :%d\n",Resp.status);
                            printf("eQMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND result :%d\n",unpackRetCode);
                        }
                        else if (rsp_ctx.type == eRSP)
                        {
                            unpack_loc_SetExtPowerState_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_SetExtPowerState_t));
                            /* Unpack the LOC Set External Configure State Indication */
                            unpackRetCode= unpack_loc_SetExtPowerState( msg.buf,QMI_MSG_MAX,&Resp );
                            printf("eQMI_LOC_SET_EXTERNAL_POWER_CONFIG result :%d\n",unpackRetCode);
                        }
                        
                        break;

                case eQMI_LOC_DELETE_ASSIST_DATA:
                        if (rsp_ctx.type == eIND)
                        {
                            unpack_loc_DeleteAssistData_Ind_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_DeleteAssistData_Ind_t));
                            /* Unpack the LOC Delete Assist Data Indication */
                            unpackRetCode = unpack_loc_DeleteAssistData_Ind( msg.buf,QMI_MSG_MAX, &Resp );
                            swi_uint256_print_mask (Resp.ParamPresenceMask);
                            if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
                                printf("Delete Assist Data Indication Status :%d\n",Resp.status);
                            printf("eQMI_LOC_DELETE_ASSIST_DATA_IND result :%d\n",unpackRetCode);
                        }
                        else if (rsp_ctx.type == eRSP)
                        {
                            unpack_loc_Delete_Assist_Data_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_Delete_Assist_Data_t));
                            /* Unpack the LOC Set Operation Mode */
                            unpackRetCode = unpack_loc_DeleteAssistData( msg.buf,QMI_MSG_MAX,&Resp );
                            printf("eQMI_LOC_DELETE_ASSIST_DATA result :%d\n",unpackRetCode);
                        }
                        break;

                case eQMI_LOC_SET_OPERATION_MODE:
                        if (rsp_ctx.type == eIND)
                        {
                            unpack_loc_SetOperationMode_Ind_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_SetOperationMode_Ind_t));
                            /* Unpack the LOC Set Operation Mode Indication */
                            unpackRetCode = unpack_loc_SetOperationMode_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                            swi_uint256_print_mask (Resp.ParamPresenceMask);
                            if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
                                printf("Set Operation Mode Indication Status :%d\n",Resp.status);
                            printf("eQMI_LOC_SET_OPERATION_MODE_IND result :%d\n",unpackRetCode);
                        }
                        else if (rsp_ctx.type == eRSP)
                        {
                            unpack_loc_SetOperationMode_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_SetOperationMode_t));
                            /* Unpack the LOC Set Operation Mode */
                            unpackRetCode = unpack_loc_SetOperationMode( msg.buf,QMI_MSG_MAX,&Resp );
                            printf("eQMI_LOC_SET_OPERATION_MODE result :%d\n",unpackRetCode);
                        }
                        break;

                case eQMI_LOC_GET_BEST_AVAIL_POS_IND:
                        if (rsp_ctx.type == eIND)
                        {
                            unpack_loc_BestAvailPos_Ind_t Resp;
                            loc_gpsTime GpsTime;
                            loc_precisionDilution precisionDilution;
                            loc_svUsedforFix svUsedforFix;
                            loc_sensorDataUsage sensorDataUsage;

                            memset(&Resp,0,sizeof(unpack_loc_BestAvailPos_Ind_t));
                            Resp.pGpsTime = &GpsTime;
                            Resp.pPrecisionDilution = &precisionDilution;
                            Resp.pSvUsedforFix = &svUsedforFix;
                            Resp.pSensorDataUsage = &sensorDataUsage;
                            Resp.status = ~0;
                            /* Unpack the WDS Event Indication */
                            unpackRetCode= unpack_loc_BestAvailPos_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                            if(Resp.status <= eQMI_LOC_SESS_STATUS_IN_PROGRESS)
                            {
                            	dump_BestAvailPos_Ind(Resp);
                            }
                            printf("sessionStatus :%d\n",Resp.status);
                            printf("QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND IND result :%d\n",unpackRetCode);
                        }
                        else if (rsp_ctx.type == eRSP)
                        {
                            unpack_loc_SLQSLOCGetBestAvailPos_t Resp;
                            memset(&Resp,0,sizeof(unpack_loc_SLQSLOCGetBestAvailPos_t));
                            /* Unpack the LOC Best Avialable Position */
                            unpackRetCode = unpack_loc_SLQSLOCGetBestAvailPos( msg.buf,QMI_MSG_MAX,&Resp );
                            printf("eQMI_LOC_GET_BEST_AVAILABLE_POSITION result :%d\n",unpackRetCode);
                        }

                        break;
                case eQMI_LOC_EVENT_NMEA_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_EventNMEA_Ind_t Resp;

                        memset(&Resp,0,sizeof(unpack_loc_EventNMEA_Ind_t));

                        /* Unpack the LOC EVENT NMEA Indication */
                        unpackRetCode = unpack_loc_EventNMEA_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        swi_uint256_print_mask (Resp.ParamPresenceMask);
                        if(swi_uint256_get_bit (Resp.ParamPresenceMask, 1))
                            printf("LOC NMEA String :%s\n",Resp.NMEAData);
                        printf("QMI_LOC_EVENT_NMEA_IND IND result :%d\n",unpackRetCode);
                    }
                    break;
                case eQMI_LOC_SET_SERVER_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_SetServer_Ind_t Resp;

                        memset(&Resp,0,sizeof(unpack_loc_SetServer_Ind_t));
                        /* Unpack the LOC Set Server ind*/
                        unpackRetCode = unpack_loc_SetServer_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        printf( "\nServer Status: %d\n",
                                Resp.serverStatus);

                        printf("QMI_LOC_SET_SERVER_IND IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        /* Unpack the LOC Set Server*/
                        unpackRetCode = unpack_loc_SLQSLOCSetServer( msg.buf,QMI_MSG_MAX );
                        printf("QMI_LOC_SET_SERVER result :%d\n",unpackRetCode);
                    }
                    break;
                case eQMI_LOC_GET_SERVER_IND:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_GetServer_Ind_t Resp;
                        memset(&Resp,0,sizeof(unpack_loc_GetServer_Ind_t));
                        /* Unpack the WDS Event Indication */
                        unpackRetCode = unpack_loc_GetServer_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        if(unpackRetCode==0)
                        {
                            dump_GetServer_Ind(Resp);
                        }
                        printf("QMI_LOC_GET_SERVER_IND IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        printf("QMI_LOC_GET_SERVER_IND RSP result :%d\n",unpackRetCode);
                    }
                    break;
               case eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_CradleMountCallback_Ind_t  cradleMountInd;
                         unpack_loc_CradleMountCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &cradleMountInd);
                         printf("unpack QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (cradleMountInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (cradleMountInd.ParamPresenceMask, 1)) {
                             printf("LOC Cradle Mount Config: %d\n",
                                         cradleMountInd.cradleMountConfigStatus);
                         }
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_SLQSLOCSetCradleMountConfig_t  cradleMountResp;
                        unpackRetCode= unpack_loc_SLQSLOCSetCradleMountConfig( msg.buf,QMI_MSG_MAX, &cradleMountResp);
                        printf("QMI_LOC_SET_CRADLE_MOUNT_CONFIG result :%d\n",unpackRetCode);
                    }
                    break;
               case eQMI_LOC_EVENT_TIME_SYNC_REQ_IND:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_EventTimeSyncCallback_Ind_t  timeSyncInd;
                         unpack_loc_EventTimeSyncCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &timeSyncInd);
                         printf("unpack QMI_LOC_EVENT_TIME_SYNC_REQ_IND result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (timeSyncInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (timeSyncInd.ParamPresenceMask, 1)) {
                             printf("LOC Time Sync Ref Counter: %d\n",
                                         timeSyncInd.timeSyncRefCounter);
                         }
                    }
                    break;
               case eQMI_LOC_INJECT_TIME_SYNC_DATA_IND:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_InjectTimeSyncDataCallback_Ind_t  injectTimeInd;
                          unpack_loc_InjectTimeSyncDataCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &injectTimeInd);
                         printf("unpack QMI_LOC_INJECT_TIME_SYNC_DATA_IND result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (injectTimeInd.ParamPresenceMask);
                         if(swi_uint256_get_bit (injectTimeInd.ParamPresenceMask, 1)) {
                             printf("LOC Inject Time Sync Status: %d\n",
                                         injectTimeInd.injectTimeSyncStatus);
                         }
                    }
                    break;
               case eQMI_LOC_INJECT_SENSOR_DATA_IND:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_InjectSensorDataCallback_Ind_t  injectSensorInd;
                         unpack_loc_InjectSensorDataCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &injectSensorInd);
                         printf("unpack QMI_LOC_INJECT_SENSOR_DATA_IND result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (injectSensorInd.ParamPresenceMask);

                         if(swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 1))
                             printf("Inject Sensor Data Status: %d\n",
                                         injectSensorInd.injectSensorDataStatus);
                         if ( (injectSensorInd.pOpaqueIdentifier) && (swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 16)) )
                                 printf("\nOpaque Identifier: %d\n",
                                     *(injectSensorInd.pOpaqueIdentifier));
                         if ( (injectSensorInd.pAccelSamplesAccepted) && (swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 17)) )
                             printf("\nAccel Samples Accepted: %d\n",
                                     *(injectSensorInd.pAccelSamplesAccepted));
                         if ( (injectSensorInd.pGyroSamplesAccepted) && (swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 18)) )
                             printf("\nGyro Samples Accepted: %d\n",
                                     *(injectSensorInd.pGyroSamplesAccepted));
                         if ( (injectSensorInd.pAccelTempSamplesAccepted) && (swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 19)) )
                             printf("\nAccel Temp Samples Acceptedr: %d\n",
                                     *(injectSensorInd.pAccelTempSamplesAccepted));
                         if ( (injectSensorInd.pGyroTempSamplesAccepted) && (swi_uint256_get_bit (injectSensorInd.ParamPresenceMask, 20)) )
                             printf("\nGyro Temp Samples Accepted: %d\n",
                                     *(injectSensorInd.pGyroTempSamplesAccepted));
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_SLQSLOCInjectSensorData_t  injectSensorResp;
                        unpackRetCode = unpack_loc_SLQSLOCInjectSensorData( msg.buf,QMI_MSG_MAX, &injectSensorResp);
                        printf("QMI_LOC_INJECT_SENSOR_DATA result :%d\n",unpackRetCode);
                    }
                    break;
               case eQMI_LOC_SENSOR_STREAMING_STATUS_IND:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_SensorStreamingCallback_Ind_t  sensorStreamingInd;
                          unpack_loc_SensorStreamingCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &sensorStreamingInd);
                         printf("unpack QMI_LOC_SENSOR_STREAMING_STATUS_IND result :%d\n",unpackRetCode);
                         if(unpackRetCode==0)
                         {
                            dump_SensorStreamingCallback_Ind(sensorStreamingInd);
                         }
                    }
                    break;
               case eQMI_LOC_INJECT_UTC_TIME:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_InjectUTCTimeCallback_Ind_t  injectUTCTimeInd;
                         unpack_loc_InjectUTCTimeCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &injectUTCTimeInd);
                         printf("unpack QMI_LOC_INJECT_UTC_TIME ind result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (injectUTCTimeInd.ParamPresenceMask);

                         if(swi_uint256_get_bit (injectUTCTimeInd.ParamPresenceMask, 1))
                             printf("LOC Inject UTC Time Status: %d\n",
                                         injectUTCTimeInd.status);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_SLQSLOCInjectUTCTime_t  injectUTCTimeResp;
                        unpackRetCode = unpack_loc_SLQSLOCInjectUTCTime( msg.buf,QMI_MSG_MAX, &injectUTCTimeResp);
                        printf("QMI_LOC_INJECT_UTC_TIME result :%d\n",unpackRetCode);
                    }
                    break;
               case eQMI_LOC_INJECT_POSITION:
                    if (rsp_ctx.type == eIND)
                    {
                         unpack_loc_InjectPositionCallback_Ind_t  injectPosInd;
                         unpack_loc_InjectPositionCallback_Ind(msg.buf,
                                                         QMI_MSG_MAX,
                                                         &injectPosInd);
                         printf("unpack QMI_LOC_INJECT_POSITION ind result :%d\n",unpackRetCode);
                         swi_uint256_print_mask (injectPosInd.ParamPresenceMask);

                         if(swi_uint256_get_bit (injectPosInd.ParamPresenceMask, 1))
                             printf("LOC Inject Position Status: %d\n",
                                         injectPosInd.status);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_SLQSLOCInjectPosition_t  injectPosResp;
                        unpackRetCode = unpack_loc_SLQSLOCInjectPosition( msg.buf,QMI_MSG_MAX, &injectPosResp);
                        printf("QMI_LOC_INJECT_POSITION result :%d\n",unpackRetCode);
                    }
                    break;

                    case eQMI_LOC_GET_OPERATION_MODE:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_GetOpMode_Ind_t Resp;
                        memset(&Resp,0,sizeof(unpack_loc_GetOpMode_Ind_t));
                        unpackRetCode = unpack_loc_GetOpMode_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        if(unpackRetCode==0)
                        {
                            dump_Get_OpMode_Ind(&Resp);
                        }
                        printf("QMI_LOC_GET_OPERATION_MODE IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_SLQSLOCGetOpMode_t  getOpModeResp;
                        unpackRetCode = unpack_loc_SLQSLOCGetOpMode( msg.buf,QMI_MSG_MAX, &getOpModeResp);
                        printf("QMI_LOC_GET_OPERATION_MODE RSP result :%d\n",unpackRetCode);
                    }
                    break;

                    case eQMI_LOC_GET_FIX_CRITERIA:
                    if (rsp_ctx.type == eIND)
                    {
                        unpack_loc_FixCriteria_Ind_t Resp;
                        memset(&Resp,0,sizeof(unpack_loc_FixCriteria_Ind_t));
                        unpackRetCode = unpack_loc_FixCriteria_Ind( msg.buf,QMI_MSG_MAX,&Resp );
                        if(unpackRetCode==0)
                        {
                            dump_GetFixCriteria_ind(&Resp);
                        }
                        printf("QMI_LOC_GET_FIX_CRITERIA_IND result :%d\n",unpackRetCode);
                    }
                    else if (rsp_ctx.type == eRSP)
                    {
                        unpack_loc_GetFixCriteria_t  getFixCriteriaResp = SWI_INIT_UNPACK_RESULT_VALUE;
                        unpackRetCode = unpack_loc_GetFixCriteria( msg.buf,QMI_MSG_MAX, &getFixCriteriaResp);
                        printf("QMI_LOC_GET_FIX_CRITERIA RSP result :%d\n",unpackRetCode);
                    }
                    break;
                default:
                    printf("Default msgid 0x%x\n", rsp_ctx.msgid);
                    break;
            }
        }
        else
        {
           enLOCThread = 0;
           loc = -1;
           break;
        }
    }
    return NULL;
}

void loc_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======LOC pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    
    for(i=0; i<sizeof(loctotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(loctotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,loctotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(loc<0)
            loc = client_fd(eLOC);
        if(loc<0)
        {
            fprintf(stderr,"LOC Service Not Supported!\n");
            return ;
        }
        rtn = write(loc, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(loc>=0)
                close(loc);
            loc=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eLOC, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(loc, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eLOC, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);


            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(loctotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                loctotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           loctotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    loctotest_invalidunpack[i].dump(loctotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(loc>=0)
                close(loc);
            loc=-1;
        }
        sleep(1);
        xid++;
    }
    if(loc>=0)
        close(loc);
    loc=-1;
}

void test_loc_dummy_ind()
{
    unpack_loc_GnssSvInfo_Ind_t lunpack_loc_GnssSvInfo_Ind_t;
    unpack_loc_BestAvailPos_Ind_t lunpack_loc_BestAvailPos_Ind_t;
    unpack_loc_GetServer_Ind_t lunpack_loc_GetServer_Ind_t;
    unpack_loc_PositionRpt_Ind_t lunpack_loc_PositionRpt_Ind_t;
    unpack_loc_InjectSensorDataCallback_Ind_t lunpack_loc_InjectSensorDataCallback_Ind_t;
    unpack_loc_SensorStreamingCallback_Ind_t lunpack_loc_SensorStreamingCallback_Ind_t;

    memset(&lunpack_loc_GnssSvInfo_Ind_t, 0, sizeof(lunpack_loc_GnssSvInfo_Ind_t));
    memset(&lunpack_loc_BestAvailPos_Ind_t, 0, sizeof(lunpack_loc_BestAvailPos_Ind_t));
    memset(&lunpack_loc_GetServer_Ind_t, 0, sizeof(lunpack_loc_GetServer_Ind_t));
    memset(&lunpack_loc_PositionRpt_Ind_t, 0, sizeof(lunpack_loc_PositionRpt_Ind_t));
    memset(&lunpack_loc_InjectSensorDataCallback_Ind_t, 0, sizeof(lunpack_loc_InjectSensorDataCallback_Ind_t));
    memset(&lunpack_loc_SensorStreamingCallback_Ind_t, 0, sizeof(lunpack_loc_SensorStreamingCallback_Ind_t));
    
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_GnssSvInfo_Ind,
        dump_GnssSvInfo_Ind,
        lunpack_loc_GnssSvInfo_Ind_t,
        {
            DUMMY_LOC_GNSSSVINFO_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_BestAvailPos_Ind,
        dump_BestAvailPos_Ind,
        lunpack_loc_BestAvailPos_Ind_t,
        {
            DUMMY_LOC_BESTAVAILPOS_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_InjectSensorDataCallback_Ind,
        dump_InjectSensorDataCallback_Ind,
        lunpack_loc_InjectSensorDataCallback_Ind_t,
        {
            DUMMY_LOC_INJECTSENSORDATACALLBACK_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_GetServer_Ind,
        dump_GetServer_Ind,
        lunpack_loc_GetServer_Ind_t,
        {
            DUMMY_LOC_GETSERVER_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_PositionRpt_Ind,
        dump_PositionRpt_Ind,
        lunpack_loc_PositionRpt_Ind_t,
        {
            DUMMY_LOC_POSITIONRPT_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_InjectSensorDataCallback_Ind,
        dump_InjectSensorDataCallback_Ind,
        lunpack_loc_InjectSensorDataCallback_Ind_t,
        {
            DUMMY_LOC_INJECTSENSORDATACALLBACK_IND_MSG
        }
    );
    TEST_IND_UNPACK_DUMP_NON_PTR_ARG(
        unpack_loc_SensorStreamingCallback_Ind,
        dump_SensorStreamingCallback_Ind,
        lunpack_loc_SensorStreamingCallback_Ind_t,
        {
            DUMMY_LOC_SENSORSTREAMINGCALLBACK_IND_MSG
        }
    );
    
}

void loc_test_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;
    unsigned xid =1;
    const char *qmi_msg;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    printf("======LOC dummy unpack test===========\n");
    iLocalLog = 0;
    loc_validate_dummy_unpack();
    iLocalLog = 1;
    printf("======LOC dummy ind test===========\n");
    if(g_runoem_demo==0)
    {
        test_loc_dummy_ind();
    }
    printf("======LOC pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "LOC Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif
    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitestloc)/sizeof(testitem_t);
    }
    else
    {
        numoftestcase = sizeof(totestloc)/sizeof(testitem_t);
    }
    for(i=0; i<numoftestcase; i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;

        if(g_runoem_demo==1)
        {
            rtn = run_pack_item(oemapitestloc[i].pack)(&req_ctx, req, &reqLen,oemapitestloc[i].pack_ptr);
        }
        else
        {
            rtn = run_pack_item(totestloc[i].pack)(&req_ctx, req, &reqLen,totestloc[i].pack_ptr);
        }
#if DEBUG_LOG_TO_FILE
        if(g_runoem_demo==1)
        {
            local_fprintf("%s,%s,%s\n", oemapitestloc[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
        else
        {
            local_fprintf("%s,%s,%s\n", totestloc[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
#endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        if(loc<0)
            loc = client_fd(eLOC);
        if(loc<0)
        {
            printf("LOC Service Not Supported!\n");
            return ;
        }
        rtn = write(loc, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eLOC, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = rspLen = read(loc, rsp, QMI_MSG_MAX);

        if(rtn>0)
        {
            qmi_msg = helper_get_resp_ctx(eLOC, rsp, rspLen, &rsp_ctx);

            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                if(g_runoem_demo==1)
                {
                    unpackRetCode = run_unpack_item(oemapitestloc[i].unpack)(rsp, rspLen, oemapitestloc[i].unpack_ptr);
                }
                else
                {
                    unpackRetCode = run_unpack_item(totestloc[i].unpack)(rsp, rspLen, totestloc[i].unpack_ptr);
                }
                
#if DEBUG_LOG_TO_FILE
                memset(remark, 0, sizeof(remark));
                if(g_runoem_demo==1)
                {
                   local_fprintf("%s,%s,", oemapitestloc[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                }
                else
                {
                   local_fprintf("%s,%s,", totestloc[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                }
                if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                {
                    eRCode = helper_get_error_code(rsp);
                    reason = helper_get_error_reason(eRCode);
                    sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                } 
#endif
                if(g_runoem_demo==1)
                {
                    oemapitestloc[i].dump(oemapitestloc[i].unpack_ptr);
                }
                else
                {
                    totestloc[i].dump(totestloc[i].unpack_ptr);
                }
            }
        }
        xid++;
        if(loc>=0)
            close(loc);
        loc = -1;
    }
}


void loc_test_ind()
{
    pack_qmi_t req_ctx;
    uint16_t qmi_req_len;
    uint8_t qmi_req[QMI_MSG_MAX];
    int rtn;
    int i = 0,iSize = 0;
    const char *qmi_msg;

    if(loc<0)
    loc = client_fd(eLOC);
    if(loc<0)
    {
        printf("LOC Service Not Supported!\n");
        return ;
    }
    pack_loc_EventRegister_t locReqPara;
    pack_loc_Start_t locStart;
    pack_loc_SLQSLOCGetBestAvailPos_t GetBestAvailPosReq= {0xaabbccdd,0};
    pack_loc_SetOperationMode_t SetOperationMode = {1,0};
    memset(&locReqPara,0,sizeof(pack_loc_EventRegister_t));
    memset(&locStart,0,sizeof(pack_loc_Start_t));
    req_ctx.xid = 0x100;
    memset(&loc_attr, 0, sizeof(loc_attr));
    locStart.SessionId = 0;
    locStart.pApplicationInfo = NULL;
    locStart.pConfigAltitudeAssumed = NULL;
    locStart.pHorizontalAccuracyLvl = NULL;
    locStart.pIntermediateReportState = NULL;
    locStart.pMinIntervalTime = NULL;
    locStart.pRecurrenceType = NULL;
    locStart.Tlvresult = 0;
        
    locReqPara.eventRegister = (LOCEVENTMASKPOSITIONREPORT |  LOCEVENTMASKENGINESTATE |\
                                LOCEVENTMASKGNSSSVINFO | LOCEVENTMASKNMEA |\
                                LOCEVENTMASKINJECTTIMEREQ | LOCEVENTMASKINJECTPOSITIONREQ |\
                                LOCEVENTMASKSENSORSTREAMINGREADYSTATUS |\
                                LOCEVENTMASKTIMESYNCREQ);

    rtn = pack_loc_Start(&req_ctx, qmi_req, &qmi_req_len, (void*)&locStart);
    if(rtn!=0)
    {
        printf("LOC Start Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD Write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid ++;
    rtn = pack_loc_EventRegister(&req_ctx, qmi_req, &qmi_req_len, (void*)&locReqPara);
    if(rtn!=0)
    {
        printf("LOC Event Register Pack Fail");
        return ;
    }

    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    enLOCThread = 1;
    pthread_create(&loc_tid, &loc_attr, loc_read_thread, NULL);

    sleep(1);

    req_ctx.xid ++;
    // Set cradle mount config
    rtn = pack_loc_SLQSLOCSetCradleMountConfig(&req_ctx, qmi_req, 
                                               &qmi_req_len, (void*)&cradle_mnt_pack);
    if(rtn!=0)
    {
        printf("LOC Set Cradle Mount Config Pack Fail");
        return ;
    }

    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    req_ctx.xid ++;
    // Set inject utc time
    rtn = pack_loc_SLQSLOCInjectUTCTime(&req_ctx, qmi_req, 
                                               &qmi_req_len, (void*)&inject_time_pack);
    if(rtn!=0)
    {
        printf("LOC Inject UTC Time Pack Fail");
        return ;
    }

    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    req_ctx.xid ++;
    // Set inject sensor data
    rtn = pack_loc_SLQSLOCInjectSensorData(&req_ctx, qmi_req, 
                                               &qmi_req_len, (void*)&inject_sensor_pack);
    if(rtn!=0)
    {
        printf("LOC Inject Sensor Data Pack Fail");
        return ;
    }

    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(1);

    req_ctx.xid ++;
    // Set inject position
    rtn = pack_loc_SLQSLOCInjectPosition(&req_ctx, qmi_req, 
                                               &qmi_req_len, (void*)&inject_pos_pack);
    if(rtn!=0)
    {
        printf("LOC Inject Position Pack Fail");
        return ;
    }

    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }
    sleep(1);

    iSize = sizeof(LocSetExtPowerState)/sizeof(LocSetExtPowerState[0]);
    for(i=0;i<iSize;i++)
    {


        req_ctx.xid ++;
        //Set Different Power States
        pack_loc_SetExtPowerState(&req_ctx, qmi_req, &qmi_req_len, (void*)&LocSetExtPowerState[i]);
        rtn = write(loc, qmi_req, qmi_req_len);
        if(rtn == qmi_req_len)
        {
            qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(qmi_req_len, qmi_req);
        }

        sleep(1);
    }
    req_ctx.xid ++;
    rtn = pack_loc_SLQSLOCGetBestAvailPos(&req_ctx, qmi_req, &qmi_req_len, (void*)&GetBestAvailPosReq);
    if(rtn!=0)
    {
        printf("LOC Get Best Available Position Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid ++;
    rtn = pack_loc_SetOperationMode(&req_ctx, qmi_req, &qmi_req_len, (void*)&SetOperationMode);
    if(rtn!=0)
    {
        printf("LOC Set Operation Mode Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }

    req_ctx.xid ++;
    rtn = pack_loc_DeleteAssistData(&req_ctx, qmi_req, &qmi_req_len, (void*)&LocDeleteAssistData[1]);
    if(rtn!=0)
    {
        printf("LOC Delete Assist Data Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid ++;
    rtn = pack_loc_SLQSLOCGetServer(&req_ctx, qmi_req, &qmi_req_len, &tpack_loc_SLQSLOCGetServer);
    if(rtn!=0)
    {
        printf("LOC Get Server Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid ++;
    rtn = pack_loc_SLQSLOCSetServer(&req_ctx, qmi_req, &qmi_req_len, &tpack_loc_SLQSLOCSetServer);
    if(rtn!=0)
    {
        printf("LOC Set Server Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid ++;
    rtn = pack_loc_SLQSLOCGetOpMode(&req_ctx, qmi_req, &qmi_req_len, NULL);
    if(rtn!=0)
    {
        printf("LOC Get Operation Mode Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }

    req_ctx.xid ++;
    rtn = pack_loc_GetFixCriteria(&req_ctx, qmi_req, &qmi_req_len, NULL);
    if(rtn!=0)
    {
        printf("LOC Get Fix Criteria Fail");
        return ;
    }
    rtn = write(loc, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("LOC FD write Fail");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eLOC, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(2);
    enLOCThread = 0;
}

void loc_test_ind_exit()
{
    printf("\nkilling LOC read thread...\n");
    enLOCThread = 0;
#ifdef __ARM_EABI__
    if(loc>=0)
    close(loc);
    loc=-1;
    void *pthread_rtn_value;
    if(loc_tid!=0)
    pthread_join(loc_tid, &pthread_rtn_value);
#endif
    if(loc_tid!=0)
    pthread_cancel(loc_tid);
    loc_tid = 0;
    if(loc>=0)
    close(loc);
    loc=-1;
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_loc_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_LOC_REG_EVENTS */
    {0x02,0x01,0x00,0x21,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_SET_EXTERNAL_POWER_CONFIG */
    {0x02,0x02,0x00,0x51,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_START */
    {0x02,0x03,0x00,0x22,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_STOP */
    {0x02,0x04,0x00,0x23,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_SET_OPERATION_MODE */
    {0x02,0x05,0x00,0x4A,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_DELETE_ASSIST_DATA */
    {0x02,0x06,0x00,0x44,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_EVENT_POSITION_REPORT_IND */
    {0x04,0x07,0x00,0x24,0x00,0xF4,0x00,
            0x01,0x04,0x00,0x01,0x00,0x00,0x00,
            0x02,0x01,0x00,0x01,
            0x10,0x08,0x00,0x00,0x00,0x00,0x10,0x85,0xEB,0x35,0x40,
            0x11,0x08,0x00,0x00,0x00,0x00,0x7B,0xEB,0xB1,0x54,0x40,
            0x12,0x04,0x00,0xB4,0x39,0x3C,0x49,
            0x13,0x04,0x00,0xB4,0x39,0x3C,0x49,
            0x14,0x04,0x00,0xB4,0x39,0x3C,0x49,
            0x15,0x04,0x00,0x00,0x80,0x20,0x43,
            0x16,0x01,0x00,0x35,
            0x17,0x04,0x00,0x03,0x00,0x00,0x00,
            0x18,0x04,0x00,0x9A,0x99,0xA4,0x42,
            0x19,0x04,0x00,0x66,0x66,0x83,0x42,
            0x1A,0x04,0x00,0x00,0x00,0x00,0x00,
            0x1B,0x04,0x00,0x00,0x00,0x68,0x42,
            0x1C,0x04,0x00,0x00,0x80,0x20,0x43,
            0x1D,0x01,0x00,0x35,
            0x1E,0x04,0x00,0x03,0x00,0x00,0x00,
            0x1F,0x04,0x00,0xCD,0xCC,0x98,0x42,
            0x20,0x04,0x00,0xBE,0x8F,0x86,0x43,
            0x21,0x04,0x00,0xBE,0x8F,0x86,0x43,
            0x22,0x04,0x00,0x14,0xAE,0x23,0x41,
            0x23,0x04,0x00,0x01,0x00,0x00,0x00,
            0x24,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x25,0x08,0x00,0xF7,0x3E,0xEB,0x2A,0x6A,0x01,0x00,0x00,
            0x26,0x01,0x00,0x10,
            0x27,0x06,0x00,0x01,0x08,0x78,0x13,0xC8,0x01,
            0x28,0x04,0x00,0x66,0x66,0x83,0x42,
            0x29,0x04,0x00,0x0D,0x00,0x00,0x00,
            0x2A,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x2B,0x04,0x00,0x3C,0x00,0x00,0x00,
            0x2C,0x0D,0x00,0x06,0x19,0x00,0x1A,0x00,0x1B,0x00,0x42,0x00,0x43,0x00,0x44,0x00,
            0x2D,0x01,0x00,0x01},

    /* eQMI_LOC_EVENT_ENGINE_STATE_IND */
    {0x04,0x08,0x00,0x2B,0x00,0x07,0x00,
            0x01,0x04,0x00,0x01,0x00,0x00,0x00},

    /* eQMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND */
    {0x04,0x09,0x00,0x51,0x00,0x07,0x00,
            0x01,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_GET_BEST_AVAIL_POS */
    {0x02,0x0A,0x00,0x67,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_GET_BEST_AVAIL_POS_IND */
    {0x04,0x0B,0x00,0x67,0x00,0xE9,0x00,
            0x01,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x04,0x00,0x11,0x12,0x13,0x14,
            0x11,0x08,0x00,0xEA,0x95,0xB2,0x0C,0x71,0x50,0x52,0x40,
            0x12,0x08,0x00,0xC7,0x4B,0x37,0x89,0x41,0x68,0x64,0x40,
            0x13,0x04,0x00,0x13,0x14,0x15,0x16,
            0x14,0x04,0x00,0x00,0xD0,0x6D,0x46,
            0x15,0x04,0x00,0x15,0x16,0x17,0x18,
            0x16,0x08,0x00,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,
            0x17,0x04,0x00,0x17,0x18,0x19,0x1a,
            0x18,0x04,0x00,0x17,0x18,0x19,0x1a,
            0x19,0x04,0x00,0x17,0x18,0x19,0x1a,
            0x1a,0x04,0x00,0x00,0x00,0x1C,0x43,
            0x1b,0x01,0x00,0x00,
            0x1c,0x01,0x00,0x00,
            0x1d,0x04,0x00,0x03,0x00,0x00,0x00,
            0x1e,0x04,0x00,0x00,0x01,0x02,0x03,
            0x1f,0x04,0x00,0x00,0x01,0x02,0x03,
            0x20,0x04,0x00,0x00,0x01,0x02,0x03,
            0x21,0x01,0x00,0x00,
            0x22,0x04,0x00,0x04,0x00,0x00,0x00,
            0x23,0x04,0x00,0x00,0x01,0x02,0x03,
            0x24,0x04,0x00,0x00,0x01,0x02,0x03,
            0x25,0x04,0x00,0x00,0x80,0x5C,0x43,
            0x26,0x04,0x00,0x00,0x80,0x5C,0x43,
            0x27,0x04,0x00,0x00,0x01,0x02,0x03,
            0x28,0x04,0x00,0x80,0x00,0x00,0x00,
            0x29,0x0C,0x00,0x00,0x01,0x02,0x03,0x00,0x01,0x02,0x03,0x00,0x01,0x02,0x03,
            0x2a,0x06,0x00,0x12,0x35,0x00,0x01,0x02,0x03,
            0x2b,0x04,0x00,0x0C,0x00,0x00,0x00,
            0x2c,0x08,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
            0x2d,0x03,0x00,0x01,0x02,0x03},

    /* eQMI_LOC_SET_OPERATION_MODE */
    {0x04,0x0C,0x00,0x4A,0x00,0x07,0x00,
            0x01,0x04,0x00,0x04,0x00,0x00,0x00},

    /* eQMI_LOC_DELETE_ASSIST_DATA */
    {0x04,0x0D,0x00,0x44,0x00,0x07,0x00,
            0x01,0x04,0x00,0x06,0x00,0x00,0x00},

    /* eQMI_LOC_EVENT_GNSS_SV_INFO_IND */
    {0x04,0x0E,0x00,0x25,0x00,0x24,0x00,
            0x01,0x01,0x00,0x01,
            0x10,0x1D,0x00,0x01,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x10,0x00,
                0x01,0x03,0x00,0x00,0x00,0x01,0x00,0x80,0x5C,0x43,0x00,0x80,0x5C,0x43,
                0x00,0x80,0x5C,0x43},

    /* eQMI_LOC_INJECT_UTC_TIME */
    {0x02,0x0F,0x00,0x38,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_INJECT_POSITION */
    {0x02,0x10,0x00,0x39,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_SET_CRADLE_MOUNT_CONFIG */
    {0x02,0x11,0x00,0x4F,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_INJECT_SENSOR_DATA */
    {0x02,0x12,0x00,0x4D,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_EVENT_NMEA_IND */
    {0x04,0x13,0x00,0x26,0x00,0x4A,0x00,
            0x01,0x47,0x00,'$','G','P','G','G','A',',','0','9','2','7','5','0','.','0',
            '0','0',',','5','3','2','1','.','6','8','0','2',',','N',',','0','0','6',
            '3','0','.','3','3','7','2',',','W',',','1',',','8',',','1','.','0',
            '3',',','6','1','.','7',',','M',',','5','5','.','2',',','M',',',',','*',
            '7','6','\0'},

    /* eQMI_LOC_GET_SERVER */
    {0x02,0x14,0x00,0x43,0x00,0x07,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_LOC_GET_SERVER_IND */
    {0x04,0x15,0x00,0x43,0x00,0x40,0x00,
            0x01,0x04,0x00,0x00,0x00,0x00,0x00,
            0x02,0x04,0x00,0x03,0x00,0x00,0x00,
            0x10,0x06,0x00,0xC0,0xA8,0x00,0x01,0x34,0x12,
            0x11,0x14,0x00,0x02,0x01,0x04,0x03,0x06,0x05,0x08,0x07,0x10,0x09,
                           0x12,0x11,0x14,0x13,0x16,0x15,0x20,0x19,0x18,0x17,
            0x12,0x0F,0x00,'w','w','w','.','g','o','o','g','l','e','.','c','o','m','\0'},

    /* eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND */
    {0x04,0x16,0x00,0x4F,0x00,0x07,0x00,
           0x01,0x04,0x00,0x04,0x00,0x00,0x00},

    /* eQMI_LOC_EVENT_TIME_SYNC_REQ_IND */
    {0x04,0x17,0x00,0x2F,0x00,0x07,0x00,
          0x01,0x04,0x00,0x04,0x03,0x02,0x01},

    /* eQMI_LOC_INJECT_TIME_SYNC_DATA_IND */
    {0x04,0x18,0x00,0x4E,0x00,0x07,0x00,
        0x01,0x04,0x00,0x08,0x00,0x00,0x00},

    /* eQMI_LOC_INJECT_SENSOR_DATA_IND */
    {0x04,0x19,0x00,0x4D,0x00,0x1E,0x00,
            0x01,0x04,0x00,0x00,0x00,0x00,0x00,
            0x10,0x04,0x00,0x20,0x10,0x00,0x00,
            0x11,0x01,0x00,0x01,
            0x12,0x01,0x00,0x00,
            0x13,0x01,0x00,0x01,
            0x14,0x01,0x00,0x00},

    /* eQMI_LOC_SENSOR_STREAMING_STATUS_IND */
    {0x04,0x1A,0x00,0x2E,0x00,0x20,0x00,
            0x10,0x05,0x00,0x01,0x34,0x12,0x44,0x33,
            0x11,0x05,0x00,0x01,0x56,0x34,0x22,0x11,
            0x12,0x05,0x00,0x01,0x78,0x56,0x66,0x55,
            0x13,0x05,0x00,0x01,0x98,0x76,0x99,0x88},

    /* eQMI_LOC_INJECT_UTC_TIME */
    {0x04,0x1B,0x00,0x38,0x00,0x07,0x00,
        0x01,0x04,0x00,0x04,0x00,0x00,0x00},

    /* eQMI_LOC_INJECT_POSITION */
    {0x04,0x1C,0x00,0x39,0x00,0x07,0x00,
        0x01,0x04,0x00,0x05,0x00,0x00,0x00},

    /* eQMI_LOC_SET_SERVER_IND */
    {0x04,0x1D,0x00,0x42,0x00,0x07,0x00,
        0x01,0x04,0x00,0x01,0x00,0x00,0x00},

     /* eQMI_LOC_GET_FIX_CRITERIA */
     {0x02,0x1E,0x00,0x33,0x00,0x07,0x00,
         0x02,0x04,0x00,0x00,0x00,0x00,0x00},

     /* eQMI_LOC_GET_FIX_CRITERIA IND*/
     {0x04,0x1F,0x00,0x33,0x00,0x2B,0x00,
         0x01,0x04,0x00,0x01,0x00,0x00,0x00,
         0x10,0x04,0x00,0x03,0x00,0x00,0x00,
         0x11,0x04,0x00,0x01,0x00,0x00,0x00,
         0x12,0x04,0x00,0xA0,0x00,0x00,0x00,
         0x13,0x0C,0x00,0x03,'A','B','C',0x03,'x','y','z',0x01,0x02,'1','0'},

};

/* eQMI_LOC_REG_EVENTS */
const unpack_loc_EventRegister_t const_unpack_loc_EventRegister_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_SET_EXTERNAL_POWER_CONFIG */
const unpack_loc_SetExtPowerState_t const_unpack_loc_SetExtPowerState_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_START */
const unpack_loc_Start_t const_unpack_loc_Start_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_STOP */
const unpack_loc_Stop_t const_unpack_loc_Stop_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_SET_OPERATION_MODE */
const unpack_loc_SetOperationMode_t const_unpack_loc_SetOperationMode_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_DELETE_ASSIST_DATA */
const unpack_loc_Delete_Assist_Data_t const_unpack_loc_Delete_Assist_Data_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_EVENT_POSITION_REPORT_IND */
uint64_t                    cst_posIndLatitude = 0x4035EB8510000000ull;
uint64_t                    cst_posIndIndLongitude = 0x4054B1EB7B000000ull;
uint32_t                    cst_posIndHorUncCircular = 0x493C39B4;
uint32_t                    cst_posIndHorUncEllipseSemiMinor = 0x493C39B4;
uint32_t                    cst_posIndHorUncEllipseSemiMajor = 0x493C39B4;
uint32_t                    cst_posIndHorUncEllipseOrientAzimuth = 0x43208000;
uint8_t                     cst_posIndHorConfidence = 0x35;
uint32_t                    cst_posIndHorReliability = 0x03;
uint32_t                    cst_posIndSpeedHorizontal = 0x42A4999A;
uint32_t                    cst_posIndSpeedUnc = 0x42836666;
uint32_t                    cst_posIndAltitudeWrtEllipsoid = 0;
uint32_t                    cst_posIndAltitudeWrtMeanSeaLevel = 0x42680000;
uint32_t                    cst_posIndVertUnc = 0x43208000;
uint8_t                     cst_posIndVertConfidence = 0x35;
uint32_t                    cst_posIndVertReliability = 3;
uint32_t                    cst_posIndSpeedVertical = 0x4298CCCD;
uint32_t                    cst_posIndHeading = 0x43868FBE;
uint32_t                    cst_posIndHeadingUnc = 0x43868FBE;
uint32_t                    cst_posIndMagneticDeviation = 0x4123AE14;
uint32_t                    cst_posIndTechnologyMask = 1;
loc_precisionDilution       cst_posIndPrecisionDilution = {0,0,0};
uint64_t                    cst_posIndTimestampUtc = 0x016A2AEB3EF7ull;
uint8_t                     cst_posIndLeapSeconds = 0x10;
loc_gpsTime                 cst_posIndGpsTime = {0x0801,0x01C81378};
uint32_t                    cst_posIndTimeUnc = 0x42836666;
uint32_t                    cst_posIndTimeSrc = 0x0D;
loc_sensorDataUsage         cst_posIndSensorDataUsage = {0,0};
uint32_t                    cst_posIndFixId = 0x3C;
loc_svUsedforFix            cst_posIndSvUsedforFix = {6,{0x19,0x1A,0x1B,0x42,0x43,0x44}};
uint8_t                     cst_posIndAltitudeAssumed = 1;

const unpack_loc_PositionRpt_Ind_t const_unpack_loc_PositionRpt_Ind_t = {
        1,1,&cst_posIndLatitude, &cst_posIndIndLongitude,&cst_posIndHorUncCircular,
        &cst_posIndHorUncEllipseSemiMinor,&cst_posIndHorUncEllipseSemiMajor,&cst_posIndHorUncEllipseOrientAzimuth,
        &cst_posIndHorConfidence,&cst_posIndHorReliability,&cst_posIndSpeedHorizontal,&cst_posIndSpeedUnc,
        &cst_posIndAltitudeWrtEllipsoid,&cst_posIndAltitudeWrtMeanSeaLevel,&cst_posIndVertUnc,&cst_posIndVertConfidence,
        &cst_posIndVertReliability,&cst_posIndSpeedVertical,&cst_posIndHeading,&cst_posIndHeadingUnc,
        &cst_posIndMagneticDeviation,&cst_posIndTechnologyMask,&cst_posIndPrecisionDilution,&cst_posIndTimestampUtc,
        &cst_posIndLeapSeconds,&cst_posIndGpsTime,&cst_posIndTimeUnc,&cst_posIndTimeSrc,&cst_posIndSensorDataUsage,
        &cst_posIndFixId,&cst_posIndSvUsedforFix,&cst_posIndAltitudeAssumed,0,
        {{SWI_UINT256_BIT_VALUE(SET_32_BITS,1,2,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,
                39,40,41,42,43,44,45)}} };

/* eQMI_LOC_EVENT_POSITION_REPORT_IND */
uint64_t                    var_posIndLatitude;
uint64_t                    var_posIndIndLongitude;
uint32_t                    var_posIndHorUncCircular;
uint32_t                    var_posIndHorUncEllipseSemiMinor;
uint32_t                    var_posIndHorUncEllipseSemiMajor;
uint32_t                    var_posIndHorUncEllipseOrientAzimuth;
uint8_t                     var_posIndHorConfidence;
uint32_t                    var_posIndHorReliability;
uint32_t                    var_posIndSpeedHorizontal;
uint32_t                    var_posIndSpeedUnc;
uint32_t                    var_posIndAltitudeWrtEllipsoid;
uint32_t                    var_posIndAltitudeWrtMeanSeaLevel;
uint32_t                    var_posIndVertUnc;
uint8_t                     var_posIndVertConfidence;
uint32_t                    var_posIndVertReliability;
uint32_t                    var_posIndSpeedVertical;
uint32_t                    var_posIndHeading;
uint32_t                    var_posIndHeadingUnc;
uint32_t                    var_posIndMagneticDeviation;
uint32_t                    var_posIndTechnologyMask;
loc_precisionDilution       var_posIndPrecisionDilution;
uint64_t                    var_posIndTimestampUtc;
uint8_t                     var_posIndLeapSeconds;
loc_gpsTime                 var_posIndGpsTime;
uint32_t                    var_posIndTimeUnc;
uint32_t                    var_posIndTimeSrc;
loc_sensorDataUsage         var_posIndSensorDataUsage;
uint32_t                    var_posIndFixId;
loc_svUsedforFix            var_posIndSvUsedforFix;
uint8_t                     var_posIndAltitudeAssumed;

unpack_loc_PositionRpt_Ind_t var_unpack_loc_PositionRpt_Ind_t = {
        0,0,&var_posIndLatitude, &var_posIndIndLongitude,&var_posIndHorUncCircular,
        &var_posIndHorUncEllipseSemiMinor,&var_posIndHorUncEllipseSemiMajor,&var_posIndHorUncEllipseOrientAzimuth,
        &var_posIndHorConfidence,&var_posIndHorReliability,&var_posIndSpeedHorizontal,&var_posIndSpeedUnc,
        &var_posIndAltitudeWrtEllipsoid,&var_posIndAltitudeWrtMeanSeaLevel,&var_posIndVertUnc,&var_posIndVertConfidence,
        &var_posIndVertReliability,&var_posIndSpeedVertical,&var_posIndHeading,&var_posIndHeadingUnc,
        &var_posIndMagneticDeviation,&var_posIndTechnologyMask,&var_posIndPrecisionDilution,&var_posIndTimestampUtc,
        &var_posIndLeapSeconds,&var_posIndGpsTime,&var_posIndTimeUnc,&var_posIndTimeSrc,&var_posIndSensorDataUsage,
        &var_posIndFixId,&var_posIndSvUsedforFix,&var_posIndAltitudeAssumed,0,
        {{0}}  };

/* eQMI_LOC_EVENT_ENGINE_STATE_IND */
const unpack_loc_EngineState_Ind_t const_unpack_loc_EngineState_Ind_t = {
 1,0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND */
const unpack_loc_SetExtPowerConfig_Ind_t const_unpack_loc_SetExtPowerConfig_Ind_t = {
 0,0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_GET_BEST_AVAIL_POS */
const unpack_loc_SLQSLOCGetBestAvailPos_t const_unpack_loc_SLQSLOCGetBestAvailPos_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t            cst_Xid = 0x14131211;;
uint64_t            cst_Latitude = 0x405250710CB295EAull;
uint64_t            cst_Longitude = 0x4064684189374BC7ull;
uint32_t            cst_HorUncCircular = 0x16151413;
uint32_t            cst_AltitudeWrtEllipsoid = 0x466DD000;
uint32_t            cst_VertUnc = 0x18171615;
uint64_t            cst_TimestampUtc = 0x1D1C1B1A19181716ull;
uint32_t            cst_TimeUnc = 0x1A191817;
uint32_t            cst_HorUncEllipseSemiMinor = 0x1A191817;
uint32_t            cst_HorUncEllipseSemiMajor = 0x1A191817;
uint32_t            cst_HorUncEllipseOrientAzimuth = 0x431C0000;
uint8_t             cst_HorCirConf = 0;
uint8_t             cst_HorEllpConf = 0;
uint32_t            cst_HorReliability = 0x03;
uint32_t            cst_SpeedHorizontal = 0x03020100;
uint32_t            cst_SpeedUnc = 0x03020100;
uint32_t            cst_AltitudeWrtMeanSeaLevel = 0x03020100;
uint8_t             cst_VertConfidence = 0;
uint32_t            cst_VertReliability = 0x04;
uint32_t            cst_SpeedVertical = 0x03020100;
uint32_t            cst_SpeedVerticalUnc = 0x03020100;
uint32_t            cst_Heading = 0x435C8000;
uint32_t            cst_HeadingUnc = 0x435C8000;
uint32_t            cst_MagneticDeviation = 0x03020100;
uint32_t            cst_TechnologyMask = 0x00000080;
loc_precisionDilution cst_PrecisionDilution = {0x03020100,0x03020100,0x03020100};
loc_gpsTime           cst_GpsTime = {0x3512, 0x03020100};
uint32_t              cst_TimeSrc = 0x0000000C;
loc_sensorDataUsage   cst_SensorDataUsage = {0x01,0x02};
loc_svUsedforFix      cst_SvUsedforFix = {1,{0x0302}};

/* eQMI_LOC_GET_BEST_AVAIL_POS_IND */
const unpack_loc_BestAvailPos_Ind_t const_unpack_loc_BestAvailPos_Ind_t = {
        0,&cst_Xid, &cst_Latitude,&cst_Longitude,
        &cst_HorUncCircular,&cst_AltitudeWrtEllipsoid,&cst_VertUnc,
        &cst_TimestampUtc,&cst_TimeUnc,&cst_HorUncEllipseSemiMinor,&cst_HorUncEllipseSemiMajor,
        &cst_HorUncEllipseOrientAzimuth,&cst_HorCirConf,&cst_HorEllpConf,&cst_HorReliability,
        &cst_SpeedHorizontal,&cst_SpeedUnc,&cst_AltitudeWrtMeanSeaLevel,&cst_VertConfidence,
        &cst_VertReliability,&cst_SpeedVertical,&cst_SpeedVerticalUnc,&cst_Heading,
        &cst_HeadingUnc,&cst_MagneticDeviation,&cst_TechnologyMask,&cst_PrecisionDilution,&cst_GpsTime,
        &cst_TimeSrc,&cst_SensorDataUsage,&cst_SvUsedforFix,0,
        {{SWI_UINT256_BIT_VALUE(SET_31_BITS,1,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,
                39,40,41,42,43,44,45)}} };

uint32_t            var_Xid;;
uint64_t            var_Latitude;
uint64_t            var_Longitude;
uint32_t            var_HorUncCircular;
uint32_t            var_AltitudeWrtEllipsoid;
uint32_t            var_VertUnc;
uint64_t            var_TimestampUtc;
uint32_t            var_TimeUnc;
uint32_t            var_HorUncEllipseSemiMinor;
uint32_t            var_HorUncEllipseSemiMajor;
uint32_t            var_HorUncEllipseOrientAzimuth;
uint8_t             var_HorCirConf;
uint8_t             var_HorEllpConf;
uint32_t            var_HorReliability;
uint32_t            var_SpeedHorizontal;
uint32_t            var_SpeedUnc;
uint32_t            var_AltitudeWrtMeanSeaLevel;
uint8_t             var_VertConfidence;
uint32_t            var_VertReliability;
uint32_t            var_SpeedVertical;
uint32_t            var_SpeedVerticalUnc;
uint32_t            var_Heading ;
uint32_t            var_HeadingUnc;
uint32_t            var_MagneticDeviation;
uint32_t            var_TechnologyMask;
loc_precisionDilution var_PrecisionDilution;
loc_gpsTime           var_GpsTime;
uint32_t              var_TimeSrc;
loc_sensorDataUsage   var_SensorDataUsage;
loc_svUsedforFix      var_SvUsedforFix;

/* eQMI_LOC_GET_BEST_AVAIL_POS_IND */
unpack_loc_BestAvailPos_Ind_t var_unpack_loc_BestAvailPos_Ind_t = {
        0,&var_Xid, &var_Latitude,&var_Longitude,
        &var_HorUncCircular,&var_AltitudeWrtEllipsoid,&var_VertUnc,
        &var_TimestampUtc,&var_TimeUnc,&var_HorUncEllipseSemiMinor,&var_HorUncEllipseSemiMajor,
        &var_HorUncEllipseOrientAzimuth,&var_HorCirConf,&var_HorEllpConf,&var_HorReliability,
        &var_SpeedHorizontal,&var_SpeedUnc,&var_AltitudeWrtMeanSeaLevel,&var_VertConfidence,
        &var_VertReliability,&var_SpeedVertical,&var_SpeedVerticalUnc,&var_Heading,
        &var_HeadingUnc,&var_MagneticDeviation,&var_TechnologyMask,&var_PrecisionDilution,&var_GpsTime,
        &var_TimeSrc,&var_SensorDataUsage,&var_SvUsedforFix,0, {{0}} };

/* eQMI_LOC_SET_OPERATION_MODE */
const unpack_loc_SetOperationMode_Ind_t const_unpack_loc_SetOperationMode_Ind_t = {
 4,0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_DELETE_ASSIST_DATA */
const unpack_loc_DeleteAssistData_Ind_t const_unpack_loc_DeleteAssistData_Ind_t = {
 6,0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

loc_satelliteInfo cst_SatelliteInfo = {0x01,0x01,0x02,0x10,
        0x01,0x03,0x01,220.50,220.50,220.50};

/* eQMI_LOC_EVENT_GNSS_SV_INFO_IND */
const unpack_loc_GnssSvInfo_Ind_t const_unpack_loc_GnssSvInfo_Ind_t = {
 1,&cst_SatelliteInfo,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

loc_satelliteInfo var_SatelliteInfo;

/* eQMI_LOC_EVENT_GNSS_SV_INFO_IND */
unpack_loc_GnssSvInfo_Ind_t var_unpack_loc_GnssSvInfo_Ind_t = {
 0,&var_SatelliteInfo,0,{{0}} };

/* eQMI_LOC_INJECT_UTC_TIME */
const unpack_loc_SLQSLOCInjectUTCTime_t const_unpack_loc_SLQSLOCInjectUTCTime_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_INJECT_POSITION */
const unpack_loc_SLQSLOCInjectPosition_t const_unpack_loc_SLQSLOCInjectPosition_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_SET_CRADLE_MOUNT_CONFIG */
const unpack_loc_SLQSLOCSetCradleMountConfig_t const_unpack_loc_SLQSLOCSetCradleMountConfig_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_INJECT_SENSOR_DATA */
const unpack_loc_SLQSLOCInjectSensorData_t const_unpack_loc_SLQSLOCInjectSensorData_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_EVENT_NMEA_IND */
const unpack_loc_EventNMEA_Ind_t const_unpack_loc_EventNMEA_Ind_t = {
        {"$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76"},0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_GET_SERVER */
const unpack_loc_SLQSLOCGetServer_t const_unpack_loc_SLQSLOCGetServer_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

loc_IPv4Info     cst_IPv4AddrInfo = {0x0100A8C0,0x1234,1};
loc_IPv6Info     cst_IPv6AddrInfo = {{0x0102,0x0304,0x0506,0x0708,0x0910,0x1112,0x1314,0x1516},
                                      0x17181920,1};
loc_urlAddr      cst_URL          = {{"www.google.com"},1};

const unpack_loc_GetServer_Ind_t const_unpack_loc_GetServer_Ind_t = {
        0,3,&cst_IPv4AddrInfo, &cst_IPv6AddrInfo,&cst_URL,0,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)}} };

loc_IPv4Info     var_IPv4AddrInfo;
loc_IPv6Info     var_IPv6AddrInfo;
loc_urlAddr      var_URL         ;

unpack_loc_GetServer_Ind_t var_unpack_loc_GetServer_Ind_t = {
        0,0,&var_IPv4AddrInfo, &var_IPv6AddrInfo,&var_URL,0,
        {{0}} };

/* eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND */
const unpack_loc_CradleMountCallback_Ind_t const_unpack_loc_CradleMountCallback_Ind_t = {
 4,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_EVENT_TIME_SYNC_REQ_IND */
const unpack_loc_EventTimeSyncCallback_Ind_t const_unpack_loc_EventTimeSyncCallback_Ind_t = {
 0x01020304,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_INJECT_TIME_SYNC_DATA_IND */
const unpack_loc_InjectTimeSyncDataCallback_Ind_t const_unpack_loc_InjectTimeSyncDataCallback_Ind_t = {
 0x08,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

uint32_t cst_OpaqueIdentifier = 0x1020;
uint8_t  cst_AccelSamplesAccepted = 1;
uint8_t  cst_GyroSamplesAccepted = 0;
uint8_t  cst_AccelTempSamplesAccepted = 1;
uint8_t  cst_GyroTempSamplesAccepted = 0;

/* eQMI_LOC_INJECT_SENSOR_DATA_IND */
const unpack_loc_InjectSensorDataCallback_Ind_t const_unpack_loc_InjectSensorDataCallback_Ind_t = {
        0,&cst_OpaqueIdentifier,&cst_AccelSamplesAccepted, &cst_GyroSamplesAccepted,&cst_AccelTempSamplesAccepted,
        &cst_GyroTempSamplesAccepted,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,1,16,17,18,19,20)}} };

uint32_t var_OpaqueIdentifier;
uint8_t  var_AccelSamplesAccepted;
uint8_t  var_GyroSamplesAccepted;
uint8_t  var_AccelTempSamplesAccepted;
uint8_t  var_GyroTempSamplesAccepted;

/* eQMI_LOC_INJECT_SENSOR_DATA_IND */
unpack_loc_InjectSensorDataCallback_Ind_t var_unpack_loc_InjectSensorDataCallback_Ind_t = {
        0,&var_OpaqueIdentifier,&var_AccelSamplesAccepted, &var_GyroSamplesAccepted,&var_AccelTempSamplesAccepted,
        &var_GyroTempSamplesAccepted,{{0}} };

loc_accelAcceptReady       cst_AccelAcceptReady = {0x01,0x1234,0x3344};
loc_gyroAcceptReady        cst_GyroAcceptReady = {0x01,0x3456,0x1122};
loc_accelTempAcceptReady   cst_AccelTempAcceptReady = {0x01,0x5678,0x5566};
loc_gyroTempAcceptReady    cst_GyroTempAcceptReady = {0x01,0x7698,0x8899};

const unpack_loc_SensorStreamingCallback_Ind_t const_unpack_loc_SensorStreamingCallback_Ind_t = {
        &cst_AccelAcceptReady,&cst_GyroAcceptReady, &cst_AccelTempAcceptReady,&cst_GyroTempAcceptReady,
       {{SWI_UINT256_BIT_VALUE(SET_4_BITS,16,17,18,19)}} };

loc_accelAcceptReady       var_AccelAcceptReady;
loc_gyroAcceptReady        var_GyroAcceptReady;
loc_accelTempAcceptReady   var_AccelTempAcceptReady;
loc_gyroTempAcceptReady    var_GyroTempAcceptReady;

unpack_loc_SensorStreamingCallback_Ind_t var_unpack_loc_SensorStreamingCallback_Ind_t = {
        &var_AccelAcceptReady,&var_GyroAcceptReady, &var_AccelTempAcceptReady,&var_GyroTempAcceptReady,
       {{SWI_UINT256_BIT_VALUE(SET_4_BITS,16,17,18,19)}} };

/* eQMI_LOC_INJECT_UTC_TIME */
const unpack_loc_InjectUTCTimeCallback_Ind_t const_unpack_loc_InjectUTCTimeCallback_Ind_t = {
 0x04,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_INJECT_POSITION */
const unpack_loc_InjectPositionCallback_Ind_t const_unpack_loc_InjectPositionCallback_Ind_t = {
 0x05,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_SET_SERVER_IND */
const unpack_loc_SetServer_Ind_t const_unpack_loc_SetServer_Ind_t = {
 0x01,0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_LOC_GET_OPERATION_MODE */
const unpack_loc_SLQSLOCGetOpMode_t const_unpack_loc_SLQSLOCGetOpMode_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

uint32_t cst_Mode = 2;

const unpack_loc_GetOpMode_Ind_t const_unpack_loc_GetOpMode_Ind_t = {
        0,&cst_Mode,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

uint32_t var_Mode;

unpack_loc_GetOpMode_Ind_t var_unpack_loc_GetOpMode_Ind_t = {
        0,&var_Mode,{{0}} };

/* eQMI_LOC_GET_FIX_CRITERIA */
const unpack_loc_GetFixCriteria_t const_unpack_loc_GetFixCriteria_t = {
 0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_LOC_GET_FIX_CRITERIA_IND */
const unpack_loc_FixCriteria_Ind_t const_unpack_loc_FixCriteria_Ind_t = {
        {0x01,0x01},{0x03,0x01},{0x01,0x01},{0xA0,0x01},{0x03,{'A','B','C'},0x03,{'x','y','z'},0x01,0x02,{'1','0'},0x01},
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,16,17,18,19)}} };

int loc_validate_dummy_unpack()
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
    loopCount = sizeof(validate_loc_resp_msg)/sizeof(validate_loc_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_loc_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eLOC, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("LOC IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("LOC RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_LOC_REG_EVENTS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_EventRegister,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_EventRegister_t);
                }
                break;
            case eQMI_LOC_SET_EXTERNAL_POWER_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SetExtPowerState,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SetExtPowerState_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SetExtPowerConfig_Ind,
                    dump_SetExtPowerConfigCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SetExtPowerConfig_Ind_t);
                }
                break;
            case eQMI_LOC_START:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_Start,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_Start_t);
                }
                break;
            case eQMI_LOC_STOP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_Stop,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_Stop_t);
                }
                break;
            case eQMI_LOC_SET_OPERATION_MODE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SetOperationMode,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SetOperationMode_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SetOperationMode_Ind,
                    dump_SetOperationMode_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SetOperationMode_Ind_t);
                }
                break;
            case eQMI_LOC_DELETE_ASSIST_DATA:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_DeleteAssistData,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_Delete_Assist_Data_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_DeleteAssistData_Ind,
                    dump_DeleteAssistData_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_DeleteAssistData_Ind_t);
                }
                break;

            case eQMI_LOC_EVENT_POSITION_REPORT_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_PositionRpt_Ind_t *varp = &var_unpack_loc_PositionRpt_Ind_t;
                    const unpack_loc_PositionRpt_Ind_t *cstp = &const_unpack_loc_PositionRpt_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_PositionRpt_Ind,
                    dump_Position_Rpt_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_PositionRpt_Ind_t,
                    33,
                    CMP_PTR_TYPE, &varp->sessionStatus, &cstp->sessionStatus,
                    CMP_PTR_TYPE, &varp->sessionId, &cstp->sessionId,
                    CMP_PTR_TYPE, varp->pLatitude, cstp->pLatitude,
                    CMP_PTR_TYPE, varp->pLongitude, cstp->pLongitude,
                    CMP_PTR_TYPE, varp->pHorUncCircular, cstp->pHorUncCircular,
                    CMP_PTR_TYPE, varp->pHorUncEllipseSemiMinor, cstp->pHorUncEllipseSemiMinor,
                    CMP_PTR_TYPE, varp->pHorUncEllipseSemiMajor, cstp->pHorUncEllipseSemiMajor,
                    CMP_PTR_TYPE, varp->pHorUncEllipseOrientAzimuth, cstp->pHorUncEllipseOrientAzimuth,
                    CMP_PTR_TYPE, varp->pHorConfidence, cstp->pHorConfidence,
                    CMP_PTR_TYPE, varp->pHorReliability, cstp->pHorReliability,
                    CMP_PTR_TYPE, varp->pSpeedHorizontal, cstp->pSpeedHorizontal,
                    CMP_PTR_TYPE, varp->pSpeedUnc, cstp->pSpeedUnc,
                    CMP_PTR_TYPE, varp->pAltitudeWrtEllipsoid, cstp->pAltitudeWrtEllipsoid,
                    CMP_PTR_TYPE, varp->pAltitudeWrtMeanSeaLevel, cstp->pAltitudeWrtMeanSeaLevel,
                    CMP_PTR_TYPE, varp->pVertUnc, cstp->pVertUnc,
                    CMP_PTR_TYPE, varp->pVertConfidence, cstp->pVertConfidence,
                    CMP_PTR_TYPE, varp->pVertReliability, cstp->pVertReliability,
                    CMP_PTR_TYPE, varp->pSpeedVertical, cstp->pSpeedVertical,
                    CMP_PTR_TYPE, varp->pHeading, cstp->pHeading,
                    CMP_PTR_TYPE, varp->pHeadingUnc, cstp->pHeadingUnc,
                    CMP_PTR_TYPE, varp->pMagneticDeviation, cstp->pMagneticDeviation,
                    CMP_PTR_TYPE, varp->pTechnologyMask, cstp->pTechnologyMask,
                    CMP_PTR_TYPE, varp->pPrecisionDilution, cstp->pPrecisionDilution,
                    CMP_PTR_TYPE, varp->pTimestampUtc, cstp->pTimestampUtc,
                    CMP_PTR_TYPE, varp->pLeapSeconds, cstp->pLeapSeconds,
                    CMP_PTR_TYPE, varp->pGpsTime, cstp->pGpsTime,
                    CMP_PTR_TYPE, varp->pTimeUnc, cstp->pTimeUnc,
                    CMP_PTR_TYPE, varp->pTimeSrc, cstp->pTimeSrc,
                    CMP_PTR_TYPE, varp->pSensorDataUsage, cstp->pSensorDataUsage,
                    CMP_PTR_TYPE, varp->pFixId, cstp->pFixId,
                    CMP_PTR_TYPE, varp->pSvUsedforFix, cstp->pSvUsedforFix,
                    CMP_PTR_TYPE, varp->pAltitudeAssumed, cstp->pAltitudeAssumed,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_EVENT_ENGINE_STATE_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_EngineState_Ind,
                    dump_EngineStateCallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_EngineState_Ind_t);
                }
                break;
            case eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCSetCradleMountConfig,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCSetCradleMountConfig_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_CradleMountCallback_Ind,
                    dump_CradleMountCallback_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_CradleMountCallback_Ind_t);
                }
                break;
            case eQMI_LOC_GET_BEST_AVAIL_POS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCGetBestAvailPos,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCGetBestAvailPos_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_BestAvailPos_Ind_t *varp = &var_unpack_loc_BestAvailPos_Ind_t;
                    const unpack_loc_BestAvailPos_Ind_t *cstp = &const_unpack_loc_BestAvailPos_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_BestAvailPos_Ind,
                    dump_BestAvail_Pos_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_BestAvailPos_Ind_t,
                    31,
                    CMP_PTR_TYPE, &varp->status, &cstp->status,
                    CMP_PTR_TYPE, varp->pXid, cstp->pXid,
                    CMP_PTR_TYPE, varp->pLatitude, cstp->pLatitude,
                    CMP_PTR_TYPE, varp->pLongitude, cstp->pLongitude,
                    CMP_PTR_TYPE, varp->pHorUncCircular, cstp->pHorUncCircular,
                    CMP_PTR_TYPE, varp->pAltitudeWrtEllipsoid, cstp->pAltitudeWrtEllipsoid,
                    CMP_PTR_TYPE, varp->pVertUnc, cstp->pVertUnc,
                    CMP_PTR_TYPE, varp->pTimestampUtc, cstp->pTimestampUtc,
                    CMP_PTR_TYPE, varp->pTimeUnc, cstp->pTimeUnc,
                    CMP_PTR_TYPE, varp->pHorUncEllipseSemiMinor, cstp->pHorUncEllipseSemiMinor,
                    CMP_PTR_TYPE, varp->pHorUncEllipseSemiMajor, cstp->pHorUncEllipseSemiMajor,
                    CMP_PTR_TYPE, varp->pHorUncEllipseOrientAzimuth, cstp->pHorUncEllipseOrientAzimuth,
                    CMP_PTR_TYPE, varp->pHorCirConf, cstp->pHorCirConf,
                    CMP_PTR_TYPE, varp->pHorEllpConf, cstp->pHorEllpConf,
                    CMP_PTR_TYPE, varp->pHorReliability, cstp->pHorReliability,
                    CMP_PTR_TYPE, varp->pSpeedHorizontal, cstp->pSpeedHorizontal,
                    CMP_PTR_TYPE, varp->pSpeedUnc, cstp->pSpeedUnc,
                    CMP_PTR_TYPE, varp->pAltitudeWrtMeanSeaLevel, cstp->pAltitudeWrtMeanSeaLevel,
                    CMP_PTR_TYPE, varp->pVertConfidence, cstp->pVertConfidence,
                    CMP_PTR_TYPE, varp->pVertReliability, cstp->pVertReliability,
                    CMP_PTR_TYPE, varp->pSpeedVertical, cstp->pSpeedVertical,
                    CMP_PTR_TYPE, varp->pSpeedVerticalUnc, cstp->pSpeedVerticalUnc,
                    CMP_PTR_TYPE, varp->pHeading, cstp->pHeading,
                    CMP_PTR_TYPE, varp->pHeadingUnc, cstp->pHeadingUnc,
                    CMP_PTR_TYPE, varp->pMagneticDeviation, cstp->pMagneticDeviation,
                    CMP_PTR_TYPE, varp->pTechnologyMask, cstp->pTechnologyMask,
                    CMP_PTR_TYPE, varp->pPrecisionDilution, cstp->pPrecisionDilution,
                    CMP_PTR_TYPE, varp->pGpsTime, cstp->pGpsTime,
                    CMP_PTR_TYPE, varp->pTimeSrc, cstp->pTimeSrc,
                    CMP_PTR_TYPE, varp->pSensorDataUsage, cstp->pSensorDataUsage,
                    CMP_PTR_TYPE, varp->pSvUsedforFix, cstp->pSvUsedforFix,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_EVENT_GNSS_SV_INFO_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_GnssSvInfo_Ind_t *varp = &var_unpack_loc_GnssSvInfo_Ind_t;
                    const unpack_loc_GnssSvInfo_Ind_t *cstp = &const_unpack_loc_GnssSvInfo_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_GnssSvInfo_Ind,
                    dump_GnssSv_Info_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_GnssSvInfo_Ind_t,
                    3,
                    CMP_PTR_TYPE, &varp->altitudeAssumed, &cstp->altitudeAssumed,
                    CMP_PTR_TYPE, varp->pSatelliteInfo, cstp->pSatelliteInfo,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_INJECT_UTC_TIME:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCInjectUTCTime,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCInjectUTCTime_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_InjectUTCTimeCallback_Ind,
                    dump_InjectUTCTimeCallback_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_InjectUTCTimeCallback_Ind_t);
                }
                break;
            case eQMI_LOC_INJECT_POSITION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCInjectPosition,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCInjectPosition_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_InjectPositionCallback_Ind,
                    dump_InjectPositionCallback_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_InjectPositionCallback_Ind_t);
                }
                break;
            case eQMI_LOC_INJECT_SENSOR_DATA:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCInjectSensorData,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCInjectSensorData_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_InjectSensorDataCallback_Ind_t *varp = &var_unpack_loc_InjectSensorDataCallback_Ind_t;
                    const unpack_loc_InjectSensorDataCallback_Ind_t *cstp = &const_unpack_loc_InjectSensorDataCallback_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_InjectSensorDataCallback_Ind,
                    dump_InjectSensorData_Callback_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_InjectSensorDataCallback_Ind_t,
                    7,
                    CMP_PTR_TYPE, &varp->injectSensorDataStatus, &cstp->injectSensorDataStatus,
                    CMP_PTR_TYPE, varp->pOpaqueIdentifier, cstp->pOpaqueIdentifier,
                    CMP_PTR_TYPE, varp->pAccelSamplesAccepted, cstp->pAccelSamplesAccepted,
                    CMP_PTR_TYPE, varp->pGyroSamplesAccepted, cstp->pGyroSamplesAccepted,
                    CMP_PTR_TYPE, varp->pAccelTempSamplesAccepted, cstp->pAccelTempSamplesAccepted,
                    CMP_PTR_TYPE, varp->pGyroTempSamplesAccepted, cstp->pGyroTempSamplesAccepted,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_EVENT_NMEA_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_EventNMEA_Ind,
                    dump_EventNMEACallBack,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_EventNMEA_Ind_t);
                }
                break;
            case eQMI_LOC_GET_SERVER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCGetServer,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCGetServer_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_GetServer_Ind_t *varp = &var_unpack_loc_GetServer_Ind_t;
                    const unpack_loc_GetServer_Ind_t *cstp = &const_unpack_loc_GetServer_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_GetServer_Ind,
                    dump_Get_Server_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_GetServer_Ind_t,
                    6,
                    CMP_PTR_TYPE, &varp->serverStatus, &cstp->serverStatus,
                    CMP_PTR_TYPE, &varp->serverType, &cstp->serverType,
                    CMP_PTR_TYPE, varp->pIPv4AddrInfo, cstp->pIPv4AddrInfo,
                    CMP_PTR_TYPE, varp->pIPv6AddrInfo, cstp->pIPv6AddrInfo,
                    CMP_PTR_TYPE, varp->pURL, cstp->pURL,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_EVENT_TIME_SYNC_REQ_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_EventTimeSyncCallback_Ind,
                    dump_EventTimeSyncCallback_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_EventTimeSyncCallback_Ind_t);
                }
                break;
            case eQMI_LOC_INJECT_TIME_SYNC_DATA_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_InjectTimeSyncDataCallback_Ind,
                    dump_InjectTimeSyncDataCallback_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_InjectTimeSyncDataCallback_Ind_t);
                }
                break;
            case eQMI_LOC_SENSOR_STREAMING_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_SensorStreamingCallback_Ind_t *varp = &var_unpack_loc_SensorStreamingCallback_Ind_t;
                    const unpack_loc_SensorStreamingCallback_Ind_t *cstp = &const_unpack_loc_SensorStreamingCallback_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SensorStreamingCallback_Ind,
                    dump_SensorStreaming_Callback_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_SensorStreamingCallback_Ind_t,
                    5,
                    CMP_PTR_TYPE, varp->pAccelAcceptReady, cstp->pAccelAcceptReady,
                    CMP_PTR_TYPE, varp->pGyroAcceptReady, cstp->pGyroAcceptReady,
                    CMP_PTR_TYPE, varp->pAccelTempAcceptReady, cstp->pAccelTempAcceptReady,
                    CMP_PTR_TYPE, varp->pGyroTempAcceptReady, cstp->pGyroTempAcceptReady,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_SET_SERVER_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SetServer_Ind,
                    dump_SetServer_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SetServer_Ind_t);
                }
                break;
            case eQMI_LOC_GET_OPERATION_MODE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_SLQSLOCGetOpMode,
                    dump_common_resultcode,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_SLQSLOCGetOpMode_t);
                }
                if (eIND == rsp_ctx.type)
                {
                    unpack_loc_GetOpMode_Ind_t *varp = &var_unpack_loc_GetOpMode_Ind_t;
                    const unpack_loc_GetOpMode_Ind_t *cstp = &const_unpack_loc_GetOpMode_Ind_t;
                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_GetOpMode_Ind,
                    dump_Get_OpMode_Ind,
                    msg.buf,
                    rlen,
                    &var_unpack_loc_GetOpMode_Ind_t,
                    3,
                    CMP_PTR_TYPE, &varp->Status, &cstp->Status,
                    CMP_PTR_TYPE, varp->pMode, cstp->pMode,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask);
                }
                break;
            case eQMI_LOC_GET_FIX_CRITERIA:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_GetFixCriteria,
                    dump_LOC_GetFixCriteria,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_GetFixCriteria_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_loc_FixCriteria_Ind,
                    dump_GetFixCriteria_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_loc_FixCriteria_Ind_t);
                }
                break;
            }
        }
    }
    return 0;
}

