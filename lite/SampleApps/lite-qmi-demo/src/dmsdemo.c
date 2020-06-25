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
#include "dms.h"
#include "fms.h"

int dms_validate_dummy_unpack();
int fms_validate_dummy_unpack();
static int iLocalLog = 1;

int dms=-1;
static char remark[255]={0};
static int unpackRetCode = 0;
static char model_id[DMS_SLQSFWINFO_MODELID_SZ]="WP7603";
char fw_version[DMS_SLQSFWINFO_BOOTVERSION_SZ] = "";
char IMEI[16]="";
char ICCID[25]="";
int g_oemApiIndex;

////Modem Settings
pack_dms_SetPower_t DefaultPowerStatus = { DMS_PM_ONLINE, 0};
pack_dms_SetUSBComp_t DefaultUSBCompSettings={0,0};
pack_dms_SetCustFeature_t DefaultCusFeatureSettings=
{
    1,0,0,0,0,0,0,0,0//Enable GPS=1,GPS Select = 0
};
pack_dms_SetCrashAction_t DefaultCrashActionSettings = {2};
pack_dms_SetCustFeaturesV2_t DefaultCusFeaturesV2Settings = 
{
    {0},//""
    1,
    {1,0},
    0
};
uint8_t DefaultModemSmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]={0};
uint8_t DefaultModemSmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]={0};
pack_dms_SLQSSwiSetDyingGaspCfg_t DefaultSwiSetDyingGaspCfg={DefaultModemSmsphoneNumber,DefaultModemSmsMessage};
pack_dms_SLQSSwiSetHostDevInfo_t DefaultSLQSSwiSetHostDevInfoSettings = {"", "", "", "",""};
pack_dms_SLQSSwiSetOSInfo_t DefaultSwiSetOSInfoSettings = {"", ""};

unpack_dms_SLQSDmsSwiGetPCInfo_t tunpack_dms_SLQSDmsSwiGetPCInfo;

dms_PSMEnableStateTlv         PsmEnableState;
dms_PSMDurationThresholdTlv   DurationThreshold;
dms_PSMDurationDueToOOSTlv    DurationDueToOOS;
dms_PSMRandomizationWindowTlv RandomizationWindow;
dms_PSMActiveTimerTlv         ActiveTimer;
dms_PSMPeriodicUpdateTimerTlv PeriodicUpdateTimer;
dms_PSMEarlyWakeupTimeTlv     EarlyWakeupTime;
unpack_dms_SLQSGetPowerSaveModeConfig_t tunpack_dms_SLQSGetPowerSaveModeConfig = {
        &PsmEnableState, &DurationThreshold, &DurationDueToOOS, &RandomizationWindow,
        &ActiveTimer, &PeriodicUpdateTimer, &EarlyWakeupTime,SWI_UINT256_INT_VALUE};

uint8_t    setPsmEnableState = 1;
uint32_t   setActiveTimer = 0x28;
uint32_t   setPeriodicUpdateTimer = 0x28;
pack_dms_SLQSSetPowerSaveModeConfig_t tpack_dms_SLQSSetPowerSaveModeConfig =
    {&setPsmEnableState, &setActiveTimer, &setPeriodicUpdateTimer};

dms_UimAutoSwitchActSlotTlv uimAutoSwitchActSlot;
unpack_dms_SLQSDmsSwiGetUimSelection_t tunpack_dms_SLQSDmsSwiGetUimSelection = {0,&uimAutoSwitchActSlot, SWI_UINT256_INT_VALUE };

////////////////////////////////////
pthread_attr_t dms_attr;
pthread_t dms_tid = 0;
volatile int enDmsThread;
volatile int ReadyToWriteNext;
volatile uint16_t Sendxid;
static char funcName[128] = {0};

void dms_dummy_unpack();

static void checkErrorAndFormErrorString(uint8_t *msg)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    if ( unpackRetCode!=eQCWWAN_ERR_NONE )
    {
        eRCode = helper_get_error_code(msg);
        reason = helper_get_error_reason(eRCode);
        sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
    }
}

/////////////////////////////////////////////////////////////

unpack_dms_GetPower_t power;
unpack_dms_GetFirmwareInfo_t fwinfo;
unpack_dms_GetModelID_t model;
unpack_dms_GetIMSI_t imsi;
unpack_dms_GetFirmwareRevisions_t FirmwareRevisions;
unpack_dms_GetFirmwareRevision_t FirmwareRevision;
unpack_dms_SLQSGetBandCapability_t dmsSLQSGetBandCapability;
unpack_dms_SLQSGetBandCapabilityExt_t dmsSLQSGetBandCapabilityExt;
unpack_dms_GetDeviceSerialNumbers_t DeviceSerialNumbers;
unpack_dms_GetPRLVersion_t PRLVersion;
unpack_dms_GetNetworkTime_t Time;
unpack_dms_GetVoiceNumber_t VoiceNumber;
unpack_dms_GetDeviceHardwareRev_t HardwareRev;
unpack_dms_GetFSN_t FSN;
unpack_dms_GetDeviceCap_t DeviceCap;
unpack_dms_SetPower_t SetPowerResp;
unpack_dms_GetBandCapability_t BandCapability;
unpack_dms_GetUSBComp_t GetUSBComp;
unpack_dms_SetUSBComp_t SetUSBComp;

unpack_dms_SetCustFeature_t SetCusFeatureResp;
unpack_dms_GetCustFeature_t GetCusFeatureResp;
unpack_dms_SetFirmwarePreference_t SetFirmwarePreferenceResp;
unpack_dms_GetCrashAction_t GetCrashAction;
unpack_dms_GetDeviceMfr_t  GetDeviceMfr;
unpack_dms_SetEventReport_t SetEventReportResp;
unpack_dms_SwiSetEventReport_t SwiSetEventResp;
unpack_dms_UIMGetICCID_t UIMGetICCIDResp;
unpack_dms_GetActivationState_t  ActivationStateResp;

image_info_t fw_info[16];

unpack_dms_SLQSSwiGetFirmwareCurr_t cur_fw = {
        sizeof(fw_info)/sizeof(image_info_t), fw_info,
        {0}, {0},{0}, {0} ,SWI_UINT256_INT_VALUE};

uint8_t swiTempInd =0x01;
uint8_t swiUIMStatusInd =0x01;

pack_dms_SetEventReport_t SetEventReport[]={{0},{1}};

pack_dms_SwiSetEventReport_t SwiEventSettings = {&swiTempInd, NULL, NULL};
pack_dms_SwiSetEventReport_t SwiEventSettingsUINStatus = {NULL, NULL, &swiUIMStatusInd};

pack_dms_SetUSBComp_t SetUSBCompReq={0x06,0};

pack_dms_SetPower_t SetPowerOnline     = { DMS_PM_ONLINE, 0};
pack_dms_SetPower_t SetPowerLow     = { DMS_PM_LOW, 0};

pack_dms_SetCustFeature_t SetCusFeature[]=
{
    {1,0,0,0,0,0,0,0,0},//Enable GPS=1,GPS Select = 0
    {0,0,0,0,0,0,0,0,0},//Enable GPS=0,GPS Select = 0
    {9,0,0,0,0,0,0,0,0},//Enable GPS=9,GPS Select = 0
    {0,0,0,0,0,0,0,0,0},//Voice enable = 0 True
    {0,0,0,0,0,0,2,0,0},//Voice enable = 2 False
    {0,0,0,0,0,0,0,1,0},//DHCPRelayEnabled
    {0,0,0,0,0,0,0,0,0},//DHCPRelayDisable
    {0,0,0,0,0,0,0,0,0},//GPSLPM Enable
    {0,0,0,0,0,0,0,0,1},//GPSLPM Disable
};

pack_dms_SetCustFeaturesV2_t SetCusFeaturesV2Req = 
{
    {0x57,0x41,0x4b,0x45,0x48,0x4f,0x53,0x54,0x45,0x4e},//"WAKEHOSTEN"
    1,
    {1,0},
    0
};
unpack_dms_SetCustFeaturesV2_t SetCustFeaturesV2Resp;

FMSPrefImageList fmsPrefImageList;

unpack_fms_GetImagesPreference_t getimagesPreference = {
    sizeof(FMSPrefImageList),
    &fmsPrefImageList,
    0,
    SWI_UINT256_INT_VALUE
};

uint64_t sysTime;
uint64_t usrTime;
unpack_dms_GetNetworkTimeV2_t TimeV2 = {
    0,0,&sysTime,&usrTime,0,SWI_UINT256_INT_VALUE};

uint32_t                         DevSrvCaps;
uint64_t                         DevVoiceCaps;
uint64_t                         DevVoiceDataCaps;
dms_devMultiSimCaps              DevMultiSimCaps;
dms_devMultiSimVoiceDataCaps     DevMultiSimVoiceDataCaps;
dms_devCurSubsCaps               DevCurSubsCaps;
dms_devSubsVoiceDataCaps         DevSubsVoiceDataCaps;
dms_devSubsFeatureModeCaps       DevSubsFeatureModeCaps;
uint8_t                          DevMaxActDataSubsCaps;
dms_devMaxSubsCaps               DevMaxSubsCaps;
dms_devMaxCfgListCaps            DevMaxCfgListCaps;
int16_t                          DevExplicitCfgIndex;

unpack_dms_GetDeviceCapabilitiesV2_t tunpack_dms_GetDeviceCapabilitiesV2 = {
     {0,0,0,0,0,{0}},
     &DevSrvCaps, &DevVoiceCaps, &DevVoiceDataCaps, &DevMultiSimCaps,
     &DevMultiSimVoiceDataCaps, &DevCurSubsCaps, &DevSubsVoiceDataCaps,
     &DevSubsFeatureModeCaps, &DevMaxActDataSubsCaps, &DevMaxSubsCaps,
     &DevMaxCfgListCaps,&DevExplicitCfgIndex,0,
     {{0}} };

DMScustSettingInfo custSettInfo;
DMScustSettingList custSettList;
DMSgetCustomInput custSettInput;

pack_dms_GetCustFeaturesV2_t GetCusFeaturesV2Req =
{
    {0x57,0x41,0x4b,0x45,0x48,0x4f,0x53,0x54,0x45,0x4e},//"WAKEHOSTEN"
    0,
    0
};
unpack_dms_GetCustFeaturesV2_t GetCustFeaturesV2Resp =
{
    {
        &custSettInput,
        &custSettInfo,
        &custSettList
    },
    0,
    SWI_UINT256_INT_VALUE
};

unpack_fms_GetStoredImages_t getStoredImages ;

pack_fms_SetImagesPreference_t packSetImagesPreference = {
    sizeof(FMSImageElement),
    &fmsPrefImageList,
    0,
    0xff,
    0
};

unpack_fms_SetImagesPreference_t setImagePreference ;

uint8_t SetsmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]="+85224335433";
uint8_t SetsmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]="23456hello world1!";
uint8_t ModemSmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]={0};
uint8_t ModemSmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]={0};
uint8_t GetSmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]={0};
uint8_t GetSmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]={0};
pack_dms_SLQSSwiSetDyingGaspCfg_t packdmsSLQSSwiSetDyingGaspCfg={SetsmsphoneNumber,SetsmsMessage};
pack_dms_SLQSSwiSetDyingGaspCfg_t packModemdmsSLQSSwiSetDyingGaspCfg={ModemSmsphoneNumber,ModemSmsMessage};
packgetDyingGaspCfg pModemPackgetDyingGaspCfg={&ModemSmsphoneNumber[0],&ModemSmsMessage[0]};
packgetDyingGaspCfg pGetPackgetDyingGaspCfg={&GetSmsphoneNumber[0],&GetSmsMessage[0]};
unpack_dms_SLQSSwiGetDyingGaspCfg_t unpackModemdmsSLQSSwiGetDyingGaspCfg={&pModemPackgetDyingGaspCfg,0,SWI_UINT256_INT_VALUE};
unpack_dms_SLQSSwiGetDyingGaspCfg_t unpackdmsSLQSSwiGetDyingGaspCfg={&pGetPackgetDyingGaspCfg,0,SWI_UINT256_INT_VALUE};
unpack_dms_SLQSSwiSetDyingGaspCfg_t unpackdmsSLQSSwiSetDyingGaspCfg={0,SWI_UINT256_INT_VALUE};
unpack_dms_SLQSSwiClearDyingGaspStatistics_t unpackdmsSLQSSwiClearDyingGaspStatistics={0,SWI_UINT256_INT_VALUE};
uint32_t TimeStamp = -1;
uint8_t  SMSAttemptedFlag = -1;
packgetDyingGaspStatistics GetDyingGaspStatistics={&TimeStamp,&SMSAttemptedFlag};
unpack_dms_SLQSSwiGetDyingGaspStatistics_t unpackdmsSLQSSwiGetDyingGaspStatistics={&GetDyingGaspStatistics,0,SWI_UINT256_INT_VALUE};
uint8_t ResetType; uint8_t ResetSource;
unpack_dms_SLQSDmsSwiGetResetInfo_t  unpackdmsSLQSDmsSwiGetResetInfo = {~0,~0,0, SWI_UINT256_INT_VALUE};

unpack_dms_SLQSSwiGetFwUpdateStatus_t  unpackdmsSLQSSwiGetFwUpdateStatus ;

unpack_dms_GetManufacturer_t    unpackdmsGetManufacturer;

uint32_t ReasonMask; uint32_t bPlatform;
unpack_dms_GetOfflineReason_t    unpackdmsGetOfflineReason = {&ReasonMask ,&bPlatform, 0,SWI_UINT256_INT_VALUE};

pack_dms_UIMSetPINProtection_t packdmsUIMSetPINProtection = {1, 1, "1234"};
unpack_dms_UIMSetPINProtection_t unpackdmsUIMSetPINProtection;
pack_dms_UIMUnblockPIN_t packdmsUIMUnblockPIN = {1, "35311082", "0000"};
unpack_dms_UIMSetPINProtection_t unpackdmsUIMUnblockPIN;
pack_dms_UIMVerifyPIN_t packdmsUIMVerifyPIN = {1, "0000"};
unpack_dms_UIMSetPINProtection_t unpackdmsUIMVerifyPIN;
pack_dms_UIMChangePIN_t packdmsUIMChangePIN = {1, "0000", "1234"};
unpack_dms_UIMSetPINProtection_t unpackdmsUIMChangePIN;
pack_dms_UIMGetControlKeyStatus_t packdmsUIMGetControlKeyStatus = {0};
unpack_dms_UIMGetControlKeyStatus_t unpackdmsUIMGetControlKeyStatus;
unpack_dms_UIMGetPINStatus_t unpackdmsUIMGetPINStatus;
pack_dms_UIMSetControlKeyProtection_t packdmsUIMSetControlKeyProtection = {0, 0, "12345678"};
unpack_dms_UIMSetControlKeyProtection_t unpackdmsUIMSetControlKeyProtection;
pack_dms_UIMUnblockControlKey_t packdmsUIMUnblockControlKey = {0, "12345678"};
unpack_dms_UIMUnblockControlKey_t unpackdmsUIMUnblockControlKey;
pack_dms_ResetToFactoryDefaults_t packdmsResetToFactoryDefaults = {"000000"};
unpack_dms_ResetToFactoryDefaults_t unpackdmsResetToFactoryDefaults = {0,SWI_UINT256_INT_VALUE};
pack_dms_ActivateAutomatic_t packdmsActivateAutomatic = {"123456"};
unpack_dms_SLQSGetERIFile_t unpackdmsSLQSGetERIFile;
unpack_dms_SLQSUIMGetState_t unpackdmsSLQSUIMGetState;
pack_dms_SLQSSwiGetCrashInfo_t packdmsSLQSSwiGetCrashInfo = {0};
unpack_dms_SLQSSwiGetCrashInfo_t unpackdmsSLQSSwiGetCrashInfo;
unpack_dms_SLQSSwiGetHostDevInfo_t unpackdmsSLQSSwiGetHostDevInfo;
pack_dms_SLQSSwiSetHostDevInfo_t packdmsSLQSSwiSetHostDevInfo = {"Sierra Wireless", "MC7354", "05.05.58.00", "PlasmaID10","Host ID"};
unpack_dms_SLQSSwiSetHostDevInfo_t unpackdmsSLQSSwiSetHostDevInfo;
unpack_dms_SLQSSwiGetOSInfo_t unpackdmsSLQSSwiGetOSInfo;
pack_dms_SLQSSwiSetOSInfo_t packdmsSLQSSwiSetOSInfo = {"Linux", "Ubuntu 14.04"};
unpack_dms_SLQSSwiGetSerialNoExt_t unpackdmsSLQSSwiGetSerialNoExt;

unpack_dms_GetSerialNumbers_t tunpack_dms_GetSerialNumbers;

unpack_dms_GetHardwareRevision_t tunpack_dms_GetHardwareRevision;

unpack_dms_GetDeviceCapabilities_t tunpack_dms_GetDeviceCapabilities;


pack_dms_SetCrashAction_t tpack_dms_SetCrashAction = {2};
unpack_dms_SetCrashAction_t tunpack_dms_SetCrashAction;

pack_dms_SetActivationStatusCallback_t tpack_dms_SetActivationStatusCallback = {1};
unpack_dms_SetActivationStatusCallback_t tunpack_dms_SetActivationStatusCallback;

//External UIM Interface,Embedded UIM,Remote UIM,Auto switch
pack_dms_SwiUimSelect_t SwiUIMSelect[4]={{0},{1},{2},{3}};
unpack_dms_SwiUimSelect_t SwiUIMSelectRsp ={0,SWI_UINT256_INT_VALUE};

/* SetEventReport */
swi_uint256_t unpack_dms_SetEventReportParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetEventReportParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SetEventReportInd */
swi_uint256_t unpack_dms_SetEventReport_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_7_BITS,16,17,18,19,20,21,22)
}};

/* SwiResetInfo */
swi_uint256_t unpack_dms_SLQSDmsSwiGetResetInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_SLQSDmsSwiGetResetInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SwiResetInfoInd */
swi_uint256_t unpack_dms_SLQSDmsSwiGetResetInfo_IndParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

swi_uint256_t unpack_dms_SLQSDmsSwiGetResetInfo_IndParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

/* SwiSetEventReport */
swi_uint256_t unpack_dms_SwiSetEventReportParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SwiSetEventReportParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* GetDeviceCap */
swi_uint256_t unpack_dms_GetDeviceCapabilitiesParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_14_BITS,1,2,16,17,18,19,20,21,22,23,24,25,26,27)
}};

swi_uint256_t unpack_dms_GetDeviceCapabilitiesParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetDeviceHardwareRev */
swi_uint256_t unpack_dms_GetDeviceHardwareRevParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_GetDeviceHardwareRevParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetFirmwareInfo */
swi_uint256_t unpack_dms_GetFirmwareInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,18,19,20,21,22,23,24)
}};

swi_uint256_t unpack_dms_GetFirmwareInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* GetFirmwareRevisions */
swi_uint256_t unpack_dms_GetFirmwareRevisionsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)
}};

swi_uint256_t unpack_dms_GetFirmwareRevisionsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetIMSI */
swi_uint256_t unpack_dms_GetIMSIParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_GetIMSIParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetModelId */
swi_uint256_t unpack_dms_GetModelIDParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_GetModelIDParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetNetworkTime */
swi_uint256_t unpack_dms_GetNetworkTimeParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)
}};

swi_uint256_t unpack_dms_GetNetworkTimeParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetPRLVersion */
swi_uint256_t unpack_dms_GetPRLVersionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)
}};

swi_uint256_t unpack_dms_GetPRLVersionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* GetSerialNumbers */
swi_uint256_t unpack_dms_GetDeviceSerialNumbersParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)
}};

swi_uint256_t unpack_dms_GetDeviceSerialNumbersParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* GetVoiceNumber */
swi_uint256_t unpack_dms_GetVoiceNumberParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)
}};

swi_uint256_t unpack_dms_GetVoiceNumberParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SlqsGetBandCapability */
swi_uint256_t unpack_dms_SLQSGetBandCapabilityParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)
}};

swi_uint256_t unpack_dms_SLQSGetBandCapabilityParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSSwiGetFirmwareCurr */
swi_uint256_t unpack_dms_SLQSSwiGetFirmwareCurrParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_SLQSSwiGetFirmwareCurrParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetICCId */
swi_uint256_t unpack_dms_UIMGetICCIDParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_UIMGetICCIDParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetStoredImage */
swi_uint256_t unpack_fms_GetStoredImagesParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_fms_GetStoredImagesParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetImagesPreference */
swi_uint256_t unpack_fms_GetImagesPreferenceParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_fms_GetImagesPreferenceParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SetImagesPreference */
swi_uint256_t unpack_fms_SetImagesPreferenceParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_fms_SetImagesPreferenceParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* Set/Get CustFeature */
swi_uint256_t unpack_dms_SetCustFeatureParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetCustFeatureParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_GetCustFeatureParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,18,19,20,21,22,23,24)
}};

swi_uint256_t unpack_dms_GetCustFeatureParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

pack_dms_SetCustFeature_t tpack_dms_SetCustFeatureModemSettings;

pack_dms_SetCustFeature_t tpack_dms_SetCustFeatureGPSEnabled ={
    1,0,0,0,0,0,0,0,0};

pack_dms_SetCustFeature_t tpack_dms_SetCustFeatureGPSDisabled ={
    0,0,0,0,0,0,0,0,0};

unpack_dms_GetCustFeature_t tunpack_dms_GetCustFeatureGPSEnabledResult ={
    1,0,0,0,0,0,0,0,0,0,
    {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,18,19,20,21,22,23,24)}}
};

unpack_dms_GetCustFeature_t tunpack_dms_GetCustFeatureGPSDisabledResult ={
    0,0,0,0,0,0,0,0,0,0,
    {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,18,19,20,21,22,23,24)}}
};

pack_dms_SetCustFeature_t tpack_dms_SetCustFeatureDHCPRelayEnabled ={
    0,0,0,0,0,0,0,1,0};

unpack_dms_GetCustFeature_t tunpack_dms_GetCustFeatureDHCPRelayEnabledResult ={
    0,0,0,0,0,0,0,1,0,0,
    {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,18,19,20,21,22,23,24)}}
};

/* Set/Get Power */
swi_uint256_t unpack_dms_GetPowerParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)
}};

swi_uint256_t unpack_dms_GetPowerParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_SetPowerParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetPowerParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

pack_dms_SetPower_t tpack_dms_SetPowerModemSettings;
unpack_dms_GetPower_t tunpack_dms_GetPowerChangeResult ={
    1,0,0,0,
    {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}}
};

/* Set/Get SwiDyingGaspCfg */
swi_uint256_t unpack_dms_SLQSSwiSetDyingGaspCfgParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SLQSSwiSetDyingGaspCfgParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SLQSSwiGetDyingGaspStatParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)
}};

swi_uint256_t unpack_dms_SLQSSwiGetDyingGaspStatParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};



swi_uint256_t unpack_dms_SLQSSwiGetDyingGaspCfgParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)
}};

swi_uint256_t unpack_dms_SLQSSwiGetDyingGaspCfgParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

uint8_t val_setsmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]={0};
uint8_t val_setsmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]={0};

uint8_t val_getsmsphoneNumberChg[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]="+85224335433";
uint8_t val_getsmsMessageChg[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]="23456hello world1!";

uint8_t val_getsmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]="+85224335433";
uint8_t val_getsmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]="23456hello world1!";
packgetDyingGaspCfg val_GetPackgetDyingGaspCfg={&val_getsmsphoneNumber[0],&val_getsmsMessage[0]};

pack_dms_SLQSSwiSetDyingGaspCfg_t tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings = {
    val_setsmsphoneNumber,val_setsmsMessage};

unpack_dms_SLQSSwiGetDyingGaspCfg_t tunpack_dms_SLQSSwiGetDyingGaspCfgResult ={
    &val_GetPackgetDyingGaspCfg,0,
    {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,17)}}
};

pack_dms_SLQSSwiSetDyingGaspCfg_t tpack_dms_SLQSSwiSetDyingGaspCfgChange= {
    &val_getsmsphoneNumberChg[0],&val_getsmsMessageChg[0]};

/* Set/Get USBComp */
swi_uint256_t unpack_dms_GetUSBCompParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)
}};

swi_uint256_t unpack_dms_GetUSBCompParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetUSBCompParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetUSBCompParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

pack_dms_SetUSBComp_t tpack_dms_SetUSBCompModemSettings;
pack_dms_SetUSBComp_t tpack_dms_SetUSBCompChange = {7,0};
unpack_dms_GetUSBComp_t tunpack_dms_GetUSBCompResult ={
    7,0,{0},0,
    {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}}
};

/* Set/Get CrashInfo */
swi_uint256_t unpack_dms_SLQSSwiGetCrashInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)
}};

swi_uint256_t unpack_dms_SLQSSwiGetCrashInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_GetCrashActionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_GetCrashActionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_dms_SetCrashActionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetCrashActionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

pack_dms_SetCrashAction_t tpack_dms_SetCrashActionModemSettings;
pack_dms_SetCrashAction_t tpack_dms_SetCrashActionChange = {1};
unpack_dms_GetCrashAction_t tunpack_dms_GetCrashActionResult ={
    1,0,
    {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}}
};

/* Set/Get CusFeatureV2Settings */
swi_uint256_t unpack_dms_GetCustFeaturesV2ParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,32,33)
}};

swi_uint256_t unpack_dms_GetCustFeaturesV2ParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetCustFeaturesV2ParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_dms_SetCustFeaturesV2ParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

pack_dms_SetCustFeaturesV2_t tpack_dms_SetCustFeaturesV2ModemSettings;
pack_dms_SetCustFeaturesV2_t tpack_dms_SetCustFeaturesV2Change = 
{
    {0x57,0x41,0x4b,0x45,0x48,0x4f,0x53,0x54,0x45,0x4e},//"WAKEHOSTEN"
    1,
    {2},
    0
};

DMScustSettingInfo val_custSettInfo = {
    10,
    {0x57,0x41,0x4b,0x45,0x48,0x4f,0x53,0x54,0x45,0x4e},//"WAKEHOSTEN"
    1,
    {2},
    0
};

DMScustSettingList val_custSettList;
DMSgetCustomInput val_custSettInput;

unpack_dms_GetCustFeaturesV2_t tunpack_dms_GetCustFeaturesV2Resp =
{
    {
        &val_custSettInput,
        &val_custSettInfo,
        &val_custSettList
    },
    0,
    {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,32,33)}}
};


void dump_GetFirmwareInfo(void* ptr)
{
    unpack_dms_GetFirmwareInfo_t *pfwinfo =
        (unpack_dms_GetFirmwareInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, pfwinfo->Tlvresult);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 16))
        printf("\tModel Id: %s\n", pfwinfo->modelid_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 17))
        printf("\tBoot Version: %s\n", pfwinfo->bootversion_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 18))
        printf("\tapp version: %s\n", pfwinfo->appversion_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 19))
        printf("\tSKU Id: %s\n", pfwinfo->sku_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 20))
        printf("\tPackage Id: %s\n", pfwinfo->packageid_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 21))
        printf("\tCarrier: %s\n", pfwinfo->carrier_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 22))
        printf("\tPRI Version: %s\n", pfwinfo->priversion_str);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 23))
        printf("\tCarrier Name: %s\n", pfwinfo->cur_carr_name);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 24))
        printf("\tCarrier Rev: %s\n", pfwinfo->cur_carr_rev);

    if(iLocalLog==0)
      return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    if ( strcmp(pfwinfo->modelid_str, (char*)model_id) ||
        strcmp(pfwinfo->bootversion_str, (char*)fw_version))
        is_matching = 0;

    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 20))
    {
        if (strstr( (char*)model_id, "WP76")  ||
            strstr( (char*)model_id, "EM75"))
            sprintf(remark,"package id TLV is not supported for %s", model_id );
    }
    CHECK_WHITELIST_MASK(
        unpack_dms_GetFirmwareInfoParamPresenceMaskWhiteList,
        pfwinfo->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetFirmwareInfoParamPresenceMaskMandatoryList,
        pfwinfo->ParamPresenceMask);
    local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
    local_fprintf("%s\n", remark);
#endif
}

void dump_GetPowerChangeResult(void* ptr)
{
    unpack_dms_GetPower_t *result =
        (unpack_dms_GetPower_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(memcmp(result,&tunpack_dms_GetPowerChangeResult,sizeof(unpack_dms_GetPower_t)) != 0) 
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_dms_GetPowerModemSettings(void* ptr)
{
    unpack_dms_GetPower_t *ppower =
        (unpack_dms_GetPower_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }

    if(swi_uint256_get_bit (ppower->ParamPresenceMask, 1))
        tpack_dms_SetPowerModemSettings.mode = ppower->OperationMode;

    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", NA_MSG);
    #endif
}

void dump_dms_GetPowerModemSettingsResult(void* ptr)
{
    unpack_dms_GetPower_t *ppower =
        (unpack_dms_GetPower_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }

    #if DEBUG_LOG_TO_FILE
    if(swi_uint256_get_bit (ppower->ParamPresenceMask, 1))
    {
        if (tpack_dms_SetPowerModemSettings.mode != ppower->OperationMode)
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        else
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
    }
    else
    {
        local_fprintf("%s\n", NA_MSG);
    }
    #endif
}


void dump_GetPower(void* ptr)
{
    unpack_dms_GetPower_t *ppower =
        (unpack_dms_GetPower_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, ppower->Tlvresult);
    if(swi_uint256_get_bit (ppower->ParamPresenceMask, 1))
    printf("\topmode: %d\n", ppower->OperationMode);
    if(swi_uint256_get_bit (ppower->ParamPresenceMask, 16))
    printf("\tOffline Reaseon: %d\n", ppower->OfflineReason);
    if(swi_uint256_get_bit (ppower->ParamPresenceMask, 17))
    printf("\tHardwareControlledMode: %d\n", ppower->HardwareControlledMode);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetPowerParamPresenceMaskWhiteList,
        ppower->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetPowerParamPresenceMaskMandatoryList,
        ppower->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((ppower->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((ppower->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetModelId(void* ptr)
{
    unpack_dms_GetModelID_t *pmodel =
        (unpack_dms_GetModelID_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, pmodel->Tlvresult);
    if(swi_uint256_get_bit (pmodel->ParamPresenceMask, 1))
        printf("\tmodelid: %s\n", pmodel->modelid);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if ( strcmp(pmodel->modelid, (char*)model_id) )
        is_matching = 0;
    if(swi_uint256_get_bit (pmodel->ParamPresenceMask, 1)==0)
        is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_dms_GetModelIDParamPresenceMaskWhiteList,
        pmodel->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetModelIDParamPresenceMaskMandatoryList,
        pmodel->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
#endif


}

void dump_GetIMSI(void* ptr)
{
    unpack_dms_GetIMSI_t *pimsi =
        (unpack_dms_GetIMSI_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__func__, pimsi->Tlvresult);
    if(swi_uint256_get_bit (pimsi->ParamPresenceMask, 1))
        printf("\timsi: %s\n", pimsi->imsi);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetIMSIParamPresenceMaskWhiteList,
        pimsi->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetIMSIParamPresenceMaskMandatoryList,
        pimsi->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((pimsi->Tlvresult ==eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        if (strstr( (char*)model_id, "WP76")  ||
            strstr( (char*)model_id, "EM75"))
            sprintf(remark," This API is not supported for %s", model_id );
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((pimsi->Tlvresult ==eQCWWAN_ERR_NONE)? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_FirmwareRevisions(void* ptr)
{
    unpack_dms_GetFirmwareRevisions_t *firmwareRevisions =
            (unpack_dms_GetFirmwareRevisions_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, firmwareRevisions->Tlvresult);
    if(swi_uint256_get_bit (firmwareRevisions->ParamPresenceMask, 1))
    printf("\tAMSS: %s\n", firmwareRevisions->AMSSString);
    if(swi_uint256_get_bit (firmwareRevisions->ParamPresenceMask, 16))
    printf("\tBoot: %s\n", firmwareRevisions->BootString);
    if(swi_uint256_get_bit (firmwareRevisions->ParamPresenceMask, 17))
    printf("\tPRI: %s\n", firmwareRevisions->PRIString);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if ( strcmp(firmwareRevisions->BootString, (char*)fw_version))
        is_matching = 0;
    CHECK_WHITELIST_MASK(
        unpack_dms_GetFirmwareRevisionsParamPresenceMaskWhiteList,
        firmwareRevisions->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetFirmwareRevisionsParamPresenceMaskMandatoryList,
        firmwareRevisions->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1)  ? "Correct": "Wrong"));
#endif
}

void dump_FirmwareRevision(void* ptr)
{
    unpack_dms_GetFirmwareRevision_t *firmwareRevision =
            (unpack_dms_GetFirmwareRevision_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, firmwareRevision->Tlvresult);
    if(firmwareRevision->AMSSString[0]!=0x7f)
    printf("\tAMSS: %s\n", firmwareRevision->AMSSString);
    if(firmwareRevision->PRIString[0]!=0x7f)
    printf("\tPRI: %s\n", firmwareRevision->PRIString);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetBandCapability(void* ptr)
{
    unpack_dms_SLQSGetBandCapability_t *pbandcap =
        (unpack_dms_SLQSGetBandCapability_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 1))
        printf("bandcap    : 0x%"PRIX64"\n", pbandcap->bandCapability);
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 16))
        printf("lte bandcap: 0x%"PRIX64"\n", pbandcap->LteBandCapability);
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 17))
        printf("tds bandcap: 0x%"PRIX64"\n", pbandcap->TdsBandCapability);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 1;
        if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 1)==0)
            is_matching = 0;
        CHECK_WHITELIST_MASK(
            unpack_dms_SLQSGetBandCapabilityParamPresenceMaskWhiteList,
            pbandcap->ParamPresenceMask);
        CHECK_MANDATORYLIST_MASK(
            unpack_dms_SLQSGetBandCapabilityParamPresenceMaskMandatoryList,
            pbandcap->ParamPresenceMask);
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
            local_fprintf("%s\n", remark);
        }
        else
            local_fprintf("%s\n",  ((is_matching == 1)  ? "Correct": "Wrong"));
#endif
}

void dump_SLQSGetBandCapabilityExt(void* ptr)
{
    unpack_dms_SLQSGetBandCapabilityExt_t *pbandcap =
        (unpack_dms_SLQSGetBandCapabilityExt_t*) ptr;
    uint16_t count = 0;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 1))
        printf("bandcap    : 0x%"PRIX64"\n", pbandcap->bandCapability);
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 16))
        printf("lte bandcap: 0x%"PRIX64"\n", pbandcap->LteBandCapability);
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 17))
        printf("tds bandcap: 0x%"PRIX64"\n", pbandcap->TdsBandCapability);
    if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 18))
     {
         for(count=0;count<pbandcap->LteBandsSupport.supportedLteBandLen;count++)
             printf( "Supported LTE Band [%d]: %d\n",
                 count,pbandcap->LteBandsSupport.lteBands[count]);
     }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 1;
        if(swi_uint256_get_bit (pbandcap->ParamPresenceMask, 1)==0)
            is_matching = 0;
        CHECK_WHITELIST_MASK(
            unpack_dms_SLQSGetBandCapabilityParamPresenceMaskWhiteList,
            pbandcap->ParamPresenceMask);
        CHECK_MANDATORYLIST_MASK(
            unpack_dms_SLQSGetBandCapabilityParamPresenceMaskMandatoryList,
            pbandcap->ParamPresenceMask);
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
            local_fprintf("%s\n", remark);
        }
        else
            local_fprintf("%s\n",  ((is_matching == 1)  ? "Correct": "Wrong"));
#endif
}

void dump_GetDeviceSerialNumbers(void* ptr)
{
    unpack_dms_GetDeviceSerialNumbers_t *result =
            (unpack_dms_GetDeviceSerialNumbers_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tESN: %s\n", result->ESNString);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("\tIMEI: %s\n", result->IMEIString);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("\tIMEI SVN: %s\n", result->ImeiSvnString);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("\tMEID: %s\n", result->MEIDString);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;
    if (result->ImeiSvnString)
        is_matching = 1;
    CHECK_WHITELIST_MASK(
        unpack_dms_GetDeviceSerialNumbersParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetDeviceSerialNumbersParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetPRLVersion(void* ptr)
{
    unpack_dms_GetPRLVersion_t *result =
            (unpack_dms_GetPRLVersion_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("\tPRLPreference: %d\n", result->u8PRLPreference);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tPRLVersion: %x\n", result->u16PRLVersion);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetPRLVersionParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetPRLVersionParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult ==eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif
}

void dump_GetNetworkTime(void* ptr)
{
    unpack_dms_GetNetworkTime_t *result =
            (unpack_dms_GetNetworkTime_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("\tTime Source: %d\n", result->source);
        printf("\tTime Stamp: %"PRIu64"\n", result->timestamp);
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetNetworkTimeParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetNetworkTimeParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif
}

void dump_GetNetworkTimeV2(void* ptr)
{
    unpack_dms_GetNetworkTimeV2_t *result =
            (unpack_dms_GetNetworkTimeV2_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("\tTime Source: %d\n", result->source);
        printf("\tTime Stamp: %"PRIu64"\n", result->timestamp);
    }
    if((result->pSysTime) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("\tSystem Time in MS: %"PRIu64"\n", *result->pSysTime);
    }
    if((result->pUsrTime) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("\tUser Time in MS: %"PRIu64"\n", *result->pUsrTime);
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetNetworkTimeParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetNetworkTimeParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif
}

void dump_GetVoiceNumber(void* ptr)
{
    unpack_dms_GetVoiceNumber_t *result =
            (unpack_dms_GetVoiceNumber_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tMobile ID: %s\n", result->MIN);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tVoice Number: %s\n", result->VoiceNumber);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetVoiceNumberParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetVoiceNumberParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif
}

void dump_GetDeviceHardwareRev(void* ptr)

{
    unpack_dms_GetDeviceHardwareRev_t *result =
            (unpack_dms_GetDeviceHardwareRev_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tHardware Revision: %s\n", result->String);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetDeviceHardwareRevParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetDeviceHardwareRevParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG));
#endif
}
void dump_GetFSN(void* ptr)
{
    unpack_dms_GetFSN_t *result =
            (unpack_dms_GetFSN_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tFSN: %s\n", result->String);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_GetDeviceCap(void* ptr)
{
    unpack_dms_GetDeviceCap_t *result =
            (unpack_dms_GetDeviceCap_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    uint32_t i = 0;
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("\tDataServiceCapability: %d\n", result->DataServiceCapability);
        printf("\tMaxRXChannelRate: %d\n", result->MaxRXChannelRate);
        printf("\tMaxTXChannelRate: %d\n", result->MaxTXChannelRate);
        printf("\tRadioIfacesSize: %d\n", result->RadioIfacesSize);
        for(i=0;i<result->RadioIfacesSize;i++)
            printf("\t\t%d.RadioIfaces: %d\n",i, result->RadioIfaces[i]);
        printf("\tSimCapability: %d\n", result->SimCapability);
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetPower(void* ptr)
{
    unpack_dms_SetPower_t *result =
            (unpack_dms_SetPower_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SetPowerParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetPowerParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
        local_fprintf("%s, %s\n", "N/A", remark);
    else
        local_fprintf("%s\n",  "Correct");
#endif
}

void dump_GetBandCapability(void* ptr)
{
    unpack_dms_GetBandCapability_t *result =
            (unpack_dms_GetBandCapability_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tBandCapability: 0x%"PRIX64"\n", result->BandCapability);

    if(iLocalLog==0)
      return ;

#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? "Correct": "Wrong"));
#endif
}

void dump_GetUSBComp(void * ptr)
{
    unpack_dms_GetUSBComp_t *result =
            (unpack_dms_GetUSBComp_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    int i =0;
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("\tCurrent USB Composition: %d\n", result->USBComp);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("\tCount of supported USB Compositions: %d\n", result->NumSupUSBComps);
        for(i=0;i<result->NumSupUSBComps ; i++)
            printf("\t\tSupported USB Compositions: %d\n", result->SupUSBComps[i]);
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetUSBCompParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetUSBCompParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG));
#endif

}

void dump_SetUSBComp(void * ptr)
{
    unpack_dms_SetUSBComp_t *result =
            (unpack_dms_SetUSBComp_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SetUSBCompParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetUSBCompParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
#endif

}


void dump_SetCustFeature(void * ptr)
{
    unpack_dms_SetCustFeature_t *result =
            (unpack_dms_SetCustFeature_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SetCustFeatureParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetCustFeatureParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG);
#endif
}

void dump_GetCustFeature(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    printf("\tDHCP Relay Enable: %d\n", result->DHCPRelayEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("\tIMSI: %d\n", result->DisableIMSI);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tGPS Enable: %d\n", result->GpsEnable);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
    printf("\tGPS LPM Enable: %d\n", result->GPSLPM);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    printf("\tGPS Sel Enable: %d\n", result->GPSSel);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("\tIP Family Support: %d\n", result->IPFamSupport);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    printf("\tVoice Enable: %d\n", result->IsVoiceEnabled);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("\tRM Auto Connect: %d\n", result->RMAutoConnect);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    printf("\tSMS Support: %d\n", result->SMSSupport);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetCustFeatureParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetCustFeatureParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif

}

void dump_SetFirmwarePreference(void * ptr)
{
    unpack_dms_SetFirmwarePreference_t *result =
            (unpack_dms_SetFirmwarePreference_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetCrashAction(void * ptr)
{
    unpack_dms_GetCrashAction_t *result =
            (unpack_dms_GetCrashAction_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(result->Tlvresult==eQCWWAN_ERR_NONE)
    printf("\tCrash State: %d\n", result->DevCrashState);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetCrashActionParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetCrashActionParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetDeviceMfr(void * ptr)
{
    unpack_dms_GetDeviceMfr_t *result =
            (unpack_dms_GetDeviceMfr_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf("\tManufacturer: %s\n", result->String);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetEventReport(void * ptr)
{
    unpack_dms_SetEventReport_t *result =
            (unpack_dms_SetEventReport_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SetEventReportParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetEventReportParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG);
#endif
}

void dump_SwiSetEventReport(void * ptr)
{
    unpack_dms_SwiSetEventReport_t *result =
            (unpack_dms_SwiSetEventReport_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SwiSetEventReportParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SwiSetEventReportParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG: FAILED_MSG);
#endif
}

void dump_UIMGetICCID(void * ptr)
{
    unpack_dms_UIMGetICCID_t *result =
            (unpack_dms_UIMGetICCID_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((eQCWWAN_ERR_NONE==result->Tlvresult) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf("\tUIM ICCID: %s\n", result->String);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if ( strcmp((char*)result->String, (char*)ICCID))
        is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_dms_UIMGetICCIDParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_UIMGetICCIDParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching = 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching = 1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SetCustFeaturesV2(void * ptr)
{
    unpack_dms_SetCustFeaturesV2_t *result =
            (unpack_dms_SetCustFeaturesV2_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
        if (strstr( (char*)model_id, "MC73")  ||
            strstr( (char*)model_id, "EM73"))
            sprintf(remark," This API is not supported for %s", model_id );

    CHECK_WHITELIST_MASK(
        unpack_dms_SetCustFeaturesV2ParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetCustFeaturesV2ParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n", (result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG);
#endif
}

void dump_GetCustFeaturesV2(void * ptr)
{
    unpack_dms_GetCustFeaturesV2_t *result =
            (unpack_dms_GetCustFeaturesV2_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->GetCustomFeatureV2.pCustSettingInfo) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 32)))
    {
        int i = 0;
        uint8_t temp_value=0;
        uint64_t value = 0;
        printf( "Customization Setting:\n"\
                     "id length: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->id_length );
        
        printf("custom id: %s \n", result->GetCustomFeatureV2.pCustSettingInfo->cust_id );
        printf("value length: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->value_length );
        for ( i = 0; i < result->GetCustomFeatureV2.pCustSettingInfo->value_length; i++)
        {
            temp_value = result->GetCustomFeatureV2.pCustSettingInfo->cust_value[i];
            value = value | temp_value << (8*i);
        }
        printf(  "value id: %"PRIu64"\n", value);
        printf(  "custom attribute: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->cust_attr);
    }

    if (( result->GetCustomFeatureV2.pCustSettingList != NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 33)))
    {
        int i = 0;
        uint8_t temp_value=0;
        uint64_t value = 0;
        
        printf("\nCustomization List:\n"\
                     "list type: %d\n",  result->GetCustomFeatureV2.pCustSettingList->list_type);
        printf("number instances: %d\n",  result->GetCustomFeatureV2.pCustSettingList->num_instances);
        for ( i = 0; i < result->GetCustomFeatureV2.pCustSettingList->num_instances; i++)
        {
            int j=0;
            /* reset custom value for every single custom filed */
            value = 0;
            printf("Instance[%i]:\n", i);
            printf("id length: %d\n",  result->GetCustomFeatureV2.pCustSettingList->custSetting[i].id_length);
            printf("custtom id: %s\n",  result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_id);

            for ( j = 0; j < result->GetCustomFeatureV2.pCustSettingList->custSetting[i].value_length; j++ )
            {
                temp_value = result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_value[j];
                value = value | temp_value << (8*j);
            }
            printf("custom value : %"PRIu64"\n", value);
            printf("custom attribute: %d\n", result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_attr);
        }
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_GetCustFeaturesV2ParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetCustFeaturesV2ParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  (result->Tlvresult == eQCWWAN_ERR_NONE) ? SUCCESS_MSG : FAILED_MSG);
#endif

}

void dump_FMS_GetImagesPreference(void * ptr)
{
    int idx = 0, i = 0;
    unpack_fms_GetImagesPreference_t *result =
            (unpack_fms_GetImagesPreference_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("GetImagesPreference Successful\n");

        printf("ImageList Size:: %d\n",
                result->ImageListSize);

        if(result->pImageList)
        {
            printf("Image Element Count:: %d",
                        result->pImageList->listSize);
            for ( idx = 0;
                  idx < result->pImageList->listSize;
                  idx++ )
            {
                printf("\n\n\tImage Type:: %d\n",
                            result->pImageList->listEntries[idx].imageType);
                printf("\tImage Id::");
                for ( i = 0; i < 16; i++ )
                    printf("%x",
                                result->pImageList->listEntries[idx].imageId[i]);
                printf("\n\tBuild Id Length:: %d\n",
                            result->pImageList->listEntries[idx].buildIdLength);
                printf("\tBuild Id :: ");
                for ( i = 0;
                      i < result->pImageList->listEntries[idx].buildIdLength;

                      i++ )
                    printf("%c",
                                result->pImageList->listEntries[idx].buildId[i]);
            }
        }
        printf("\n");
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_fms_GetImagesPreferenceParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_fms_GetImagesPreferenceParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "N/A");
#endif
}

void  dump_FMS_GetStoredImages(void * ptr)
{
    //int idx = 0, i = 0;
    int idx1=0 , idx2 = 0, idx3 = 0;
    uint32_t rtn = 0;
    unpack_fms_GetStoredImages_t *result =
            (unpack_fms_GetStoredImages_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("Get Stored Images Successful\n");

        FMSImageList *pImageList = (FMSImageList *)&result->imageList;

        printf("Total Bytes Copied in Buf: %d\n", result->imagelistSize);
        printf("Image List Size: %d\n", pImageList->listSize );
        printf(" Image List Entries:\n" );
        for( idx1 = 0; idx1 < pImageList->listSize; idx1++ )
        {
            uint8_t imageIdEntries;
            printf("List Entry %d:\n", idx1 + 1 );
            printf("  Image Type: %d\n",
                pImageList->imageIDEntries[idx1].imageType );
            printf("  Max Images: %d\n",
                pImageList->imageIDEntries[idx1].maxImages );
            printf("  Executing Image Index: %x\n",
                pImageList->imageIDEntries[idx1].executingImage );
            printf("  No. of Elements in Image ID List: %x\n",
                pImageList->imageIDEntries[idx1].imageIDSize );
            imageIdEntries =
                pImageList->imageIDEntries[idx1].imageIDSize ;

            printf("\tImage ID List Elements:\n" );
            for ( idx2 = 0; idx2 < imageIdEntries; idx2++ )
            {
                FMSImageIdElement *pImageIdElement = NULL;
                pImageIdElement = ( FMSImageIdElement *)
                                &pImageList->imageIDEntries[idx1].
                                imageIDElement[idx2];
                printf("\tImage ID List Element No.: %d\n",
                                idx2 + 1 );
                printf("\tStorage Index: 0x%x\n",
                                pImageIdElement->storageIndex );
                printf("\tfailure_count: 0x%x\n",
                                pImageIdElement->failureCount );
                printf("\tImage ID: " );
                for ( idx3 = 0; idx3 < 16; idx3++ )
                {
                    printf("0x%x ",
                                pImageIdElement->imageID[idx3] );
                }
                printf("\n");
                printf("\tBuild ID Length: %d\n",
                                pImageIdElement->buildIDLength );
                if( pImageIdElement->buildIDLength)
                {
                    printf("\tBuild ID:%s\n",
                                pImageIdElement->buildID );
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
        CarrierImage_t sCarrierImage[50];
        uint32_t ValidCombinationSize = 50;
        uint32_t idx=0;
        memset(&sCarrierImage,0,sizeof(CarrierImage_t)*50);
        rtn = GetValidFwPriCombinations(&result->imageList,&ValidCombinationSize,sCarrierImage);
        printf("GetValidFwPriCombinations Result:%d\n",rtn);
        if (!rtn)
        {
            printf("Number of valid combibations: %d\n", ValidCombinationSize);
            printf("Combination details\n");
            for( idx = 0; idx < ValidCombinationSize; idx++ )
            {
                printf("Combination Index:: %d\n", idx);
                printf("  Carrier ID  : %u\n", sCarrierImage[idx].m_nCarrierId);
                printf("  Folder ID   : 0x%X\n", sCarrierImage[idx].m_nFolderId);
                printf("  Storage ID  : %u\n", sCarrierImage[idx].m_nStorage);
                printf("  PRI Build ID: %s\n", (char *)sCarrierImage[idx].m_PriBuildId);
                printf("  FW Build ID : %s\n", (char *)sCarrierImage[idx].m_FwBuildId);
                printf("  PRI Image ID: %s\n", (char *)sCarrierImage[idx].m_PriImageId);
                printf("  FW Image ID : %s\n", (char *)sCarrierImage[idx].m_FwImageId);
            }
        }
    }
    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_fms_GetStoredImagesParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_fms_GetStoredImagesParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "N/A");
#endif
}

void  dump_FMS_SetImagesPreference(void * ptr)
{
    uint32_t i = 0;
     unpack_fms_SetImagesPreference_t *result =
            (unpack_fms_SetImagesPreference_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("Image Type Size: %d\n", result->ImageTypesSize);
        for(i=0;i<result->ImageTypesSize;i++)
            printf("Image Type: %d:%d\n",i, result->ImageTypes[i]);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_fms_SetImagesPreferenceParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_fms_SetImagesPreferenceParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_GetActivationState(void * ptr)
{
    unpack_dms_GetActivationState_t *result =
            (unpack_dms_GetActivationState_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tActivation State: %d\n", result->state);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetFirmwareCurr(void * ptr)
{
    int i;
    unpack_dms_SLQSSwiGetFirmwareCurr_t *pfwinfo =
        (unpack_dms_SLQSSwiGetFirmwareCurr_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s \n",__FUNCTION__);

    for(i=0; i< pfwinfo->numEntries; i++)
    {
        printf("[%d] imageType/uniqueID/buildID: %d/%s/%s\n",
                i,
                pfwinfo->pCurrImgInfo[i].imageType,
                pfwinfo->pCurrImgInfo[i].uniqueID,
                pfwinfo->pCurrImgInfo[i].buildID);
    }
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 1))
    printf("PRI Ver.: %s\n", pfwinfo->priver);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 1))
    printf("PKG Ver. : %s\n", pfwinfo->pkgver);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 1))
    printf("FW Ver. : %s\n", pfwinfo->fwvers);
    if(swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 1))
    printf("Carrier : %s\n", pfwinfo->carrier);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if (swi_uint256_get_bit (pfwinfo->ParamPresenceMask, 1) == 0)
        is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_dms_SLQSSwiGetFirmwareCurrParamPresenceMaskWhiteList,
        pfwinfo->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SLQSSwiGetFirmwareCurrParamPresenceMaskMandatoryList,
        pfwinfo->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
#endif

}

void dump_SLQSSwiSetDyingGaspCfg(void * ptr)
{
    unpack_dms_SLQSSwiSetDyingGaspCfg_t *result =
        (unpack_dms_SLQSSwiSetDyingGaspCfg_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result)
    {
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
        swi_uint256_print_mask (result->ParamPresenceMask);
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SLQSSwiSetDyingGaspCfgParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
         unpack_dms_SLQSSwiSetDyingGaspCfgParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
        local_fprintf("%s, %s\n", "N/A", remark);
    else
        local_fprintf("%s\n",  "Correct");
#endif
}

void dump_SLQSSwiClearDyingGaspStatistics(void * ptr)
{
    unpack_dms_SLQSSwiClearDyingGaspStatistics_t *result =
        (unpack_dms_SLQSSwiClearDyingGaspStatistics_t*) ptr;
    if(result)
    {
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
        swi_uint256_print_mask (result->ParamPresenceMask);
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetDyingGaspStatistics(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspStatistics_t *result =
        (unpack_dms_SLQSSwiGetDyingGaspStatistics_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(result)
    {
        if(result->pGetDyingGaspStatistics)
        {
            if((result->pGetDyingGaspStatistics->pSMSAttemptedFlag!=NULL) &&
                (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
            printf("%s SMS Attempted Flag: %d\n",__FUNCTION__, *(result->pGetDyingGaspStatistics->pSMSAttemptedFlag));
            if((result->pGetDyingGaspStatistics->pTimeStamp!=NULL) &&
                (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
            printf("%s Time Stamp: %d\n",__FUNCTION__, *(result->pGetDyingGaspStatistics->pTimeStamp));
        }
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
        CHECK_WHITELIST_MASK(
            unpack_dms_SLQSSwiGetDyingGaspStatParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
        CHECK_MANDATORYLIST_MASK(
            unpack_dms_SLQSSwiGetDyingGaspStatParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
        local_fprintf("%s, %s\n", "N/A", remark);
    else
        local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetDyingGaspCfg(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *result =
        (unpack_dms_SLQSSwiGetDyingGaspCfg_t*) ptr;
    if(result)
    {
        if(result->pGetDyingGaspCfg)
        {
            if((result->pGetDyingGaspCfg->pDestSMSContent!=NULL) &&
                (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
            printf("%s SMS Content: %s\n",__FUNCTION__, result->pGetDyingGaspCfg->pDestSMSContent);
            if((result->pGetDyingGaspCfg->pDestSMSNum!=NULL) &&
                (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
            printf("%s Destination Number: %s\n",__FUNCTION__, result->pGetDyingGaspCfg->pDestSMSNum);
        }
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
        CHECK_WHITELIST_MASK(
            unpack_dms_SLQSSwiGetDyingGaspCfgParamPresenceMaskWhiteList,
            result->ParamPresenceMask);
        CHECK_MANDATORYLIST_MASK(
            unpack_dms_SLQSSwiGetDyingGaspCfgParamPresenceMaskMandatoryList,
            result->ParamPresenceMask);
        local_fprintf("%s\n",  "N/A");
#endif
    }
}

void dump_SLQSDmsSwiGetResetInfo(void * ptr)
{
    unpack_dms_SLQSDmsSwiGetResetInfo_t *result =
            (unpack_dms_SLQSDmsSwiGetResetInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf("\tReset Type: %d\n", result->type);
        printf("\tReset Source: %d\n", result->source);
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SLQSDmsSwiGetResetInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SLQSDmsSwiGetResetInfoParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
        local_fprintf("%s, %s\n", "N/A", remark);
    else
        local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetFwUpdateStatus(void * ptr)
{
    unpack_dms_SLQSSwiGetFwUpdateStatus_t *result =
            (unpack_dms_SLQSSwiGetFwUpdateStatus_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tFirmware Update Result Code: 0x%X\n", result->ResCode);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("\tImage Type: 0x%X\n", result->imgType);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("\tReference Data: 0x%X\n", result->refData);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("\tReference String: %s\n", result->refString);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    printf("\tLog String: %s\n", result->logString);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetManufacturer(void * ptr)
{
    unpack_dms_GetManufacturer_t *result =
            (unpack_dms_GetManufacturer_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf("\tDevice Manufacturer: %s\n", result->manufacturer);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetOfflineReason(void * ptr)
{
    unpack_dms_GetOfflineReason_t *result =
            (unpack_dms_GetOfflineReason_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    printf("Get Offline Reason Successful \n");
    if((result->pReasonMask) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    printf("ReasonMask is : %x\n", *result->pReasonMask);
    if((result->pbPlatform) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    printf("Platform is : %x\n", *result->pbPlatform);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMSetPINProtection(void * ptr)
{
    unpack_dms_UIMSetPINProtection_t *result =
            (unpack_dms_UIMSetPINProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMUnblockPIN(void * ptr)
{
    unpack_dms_UIMSetPINProtection_t *result =
            (unpack_dms_UIMSetPINProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMVerifyPIN(void * ptr)
{
    unpack_dms_UIMSetPINProtection_t *result =
            (unpack_dms_UIMSetPINProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMChangePIN(void * ptr)
{
    unpack_dms_UIMSetPINProtection_t *result =
            (unpack_dms_UIMSetPINProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMGetControlKeyStatus(void * ptr)
{
    unpack_dms_UIMGetControlKeyStatus_t *result =
            (unpack_dms_UIMGetControlKeyStatus_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf("Facility State : %d\n", result->facilityState);    
        printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);
        printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMGetPINStatus(void * ptr)
{
    unpack_dms_UIMGetPINStatus_t *result =
            (unpack_dms_UIMGetPINStatus_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("PIN1 status : %d\n", result->p1Status);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("PIN1 Verify Retries Left : %x\n", result->p1VerifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("PIN1 Unblock Retries Left : %x\n", result->p1UnblockRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("PIN2 status : %d\n", result->p2Status);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("PIN2 Verify Retries Left : %x\n", result->p2VerifyRetriesLeft);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("PIN2 Unblock Retries Left : %x\n", result->p2UnblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMSetControlKeyProtection(void * ptr)
{
    unpack_dms_UIMSetControlKeyProtection_t *result =
            (unpack_dms_UIMSetControlKeyProtection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Verify Retries Left : %x\n", result->verifyRetriesLeft);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_UIMUnblockControlKey(void * ptr)
{
    unpack_dms_UIMUnblockControlKey_t *result =
            (unpack_dms_UIMUnblockControlKey_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("Unblock Retries Left : %x\n", result->unblockRetriesLeft);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ResetToFactoryDefaults(void * ptr)
{
    unpack_dms_ResetToFactoryDefaults_t *result =
            (unpack_dms_ResetToFactoryDefaults_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ValidateSPC(void * ptr)
{
    unpack_dms_ResetToFactoryDefaults_t *result =
            (unpack_dms_ResetToFactoryDefaults_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_ActivateAutomatic(void * ptr)
{
    unpack_dms_ResetToFactoryDefaults_t *result =
            (unpack_dms_ResetToFactoryDefaults_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetERIFile(void * ptr)
{
    int i;
    unpack_dms_SLQSGetERIFile_t *result =
            (unpack_dms_SLQSGetERIFile_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf("ERI data length : %d\n", result->eriFile.eriDataLen);
        printf("ERI data :\n");
        for ( i = 0; i < result->eriFile.eriDataLen; i++)
        {
            if ( i % 16 == 0)
                printf("\n");
            printf("%02x ", result->eriFile.eriData[i]);
        }
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSUIMGetState(void * ptr)
{
    unpack_dms_SLQSUIMGetState_t *result =
            (unpack_dms_SLQSUIMGetState_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE)&&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf("UIM State : %x\n", result->state);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetCrashInfo(void * ptr)
{
    unpack_dms_SLQSSwiGetCrashInfo_t *result =
            (unpack_dms_SLQSSwiGetCrashInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    printf("crash status : %d\n", result->crashInfoParam.crashStatus);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if(result->crashInfoParam.crashInfo.numCrashes>0)
        {
            printf("carsh info :\n");
            printf("number of crashes : %d\n", result->crashInfoParam.crashInfo.numCrashes);
            printf("crash id : %d\n", result->crashInfoParam.crashInfo.crashId);
            printf("crash data : %d\n", result->crashInfoParam.crashInfo.crashData);
            printf("crash string length : %d\n", result->crashInfoParam.crashInfo.crashStrlen);
            printf("gcdump string length : %d\n", result->crashInfoParam.crashInfo.gcdumpStrlen);
        }
        
        if ( result->crashInfoParam.crashInfo.crashString != NULL)
        {
            printf("crash string : ");
            printf("%s\n", result->crashInfoParam.crashInfo.crashString);
        }
        if ( result->crashInfoParam.crashInfo.gcdumpString!= NULL)
        {
            printf("gcdump string : ");
            printf("%s\n", result->crashInfoParam.crashInfo.gcdumpString);
        }
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    if ( result->crashInfoParam.crashStatus > 1)
        is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_dms_SLQSSwiGetCrashInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SLQSSwiGetCrashInfoParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SLQSSwiGetHostDevInfo(void * ptr)
{
    unpack_dms_SLQSSwiGetHostDevInfo_t *result =
            (unpack_dms_SLQSSwiGetHostDevInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);

    if (( result->manString!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Manufacturer : %s\n", result->manString);
    }
    if (( result->modelString!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Model String : %s\n", result->modelString);
    }
    if (( result->swVerString!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
    {
        printf("Software Version : %s\n", result->swVerString);
    }
    if (( result->plasmaIDString!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("Plasma ID String : %s\n", result->plasmaIDString);
    }
    if (( result->hostID!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("Host ID String : %s\n", result->hostID);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiSetHostDevInfo(void * ptr)
{
    unpack_dms_SLQSSwiSetHostDevInfo_t *result =
            (unpack_dms_SLQSSwiSetHostDevInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetOSInfo(void * ptr)
{
    unpack_dms_SLQSSwiGetOSInfo_t *result =
            (unpack_dms_SLQSSwiGetOSInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);

    if (( result->nameString!= NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("Operating System Name : %s\n", result->nameString);
    }
    if (( result->versionString!= NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("Operating System Version : %s\n", result->versionString);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_SLQSSwiSetOSInfo(void * ptr)
{
    unpack_dms_SLQSSwiSetOSInfo_t *result =
            (unpack_dms_SLQSSwiSetOSInfo_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_SLQSSwiGetSerialNoExt(void * ptr)
{
    unpack_dms_SLQSSwiGetSerialNoExt_t *result =
            (unpack_dms_SLQSSwiGetSerialNoExt_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);

    if (( result->meidString!= NULL) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("MEID : %s\n", result->meidString);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetSerialNumbers(void* ptr)
{
    unpack_dms_GetSerialNumbers_t *result =
        (unpack_dms_GetSerialNumbers_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result \n",__FUNCTION__);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    printf("\tESN String : %s\n", result->esn);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    printf("\tIMEI String : %s\n", result->imei_no);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    printf("\tMEID String : %s\n", result->meid);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    printf("\tIMEI SV String : %s\n", result->imeisv_svn);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if ( strcmp(result->imei_no, (char*)IMEI) )
        is_matching = 0; 
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1) ? "Correct": "Wrong"));
#endif
}

void dump_GetHardwareRevision(void* ptr)
{
    unpack_dms_GetHardwareRevision_t *result =
        (unpack_dms_GetHardwareRevision_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result \n",__FUNCTION__);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    printf("\tHardware Revision : %s\n", result->hwVer);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetDeviceCapabilities(void* ptr)
{
    unpack_dms_GetDeviceCapabilities_t *result =
        (unpack_dms_GetDeviceCapabilities_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        uint8_t count = 0 ;
        printf("%s Result \n",__FUNCTION__);
        printf("MaxTXChannelRate : %x\n", result->maxTxChannelRate );
        printf("MaxRXChannelRate : %x\n", result->maxRxChannelRate );
        printf("DataServiceCapability : %x\n",
                result->dataServiceCaCapability );
        printf("SimCapability : %x\n", result->simCapability );
        printf("RadioIfacesSize: %x\n", result->radioIfacesSize);
        for(count = 0; count < result->radioIfacesSize; count++)
        {
            printf("radioInterface[%d] : %x\n",
                    count, result->RadioIfaces[count]);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    if ( (result->dataServiceCaCapability > 4) ||
        (result->simCapability > 2) ||
        (result->radioIfacesSize < 1))
        is_matching =0;
    CHECK_WHITELIST_MASK(
        unpack_dms_GetDeviceCapabilitiesParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetDeviceCapabilitiesParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetDeviceCapabilitiesV2(void* ptr)
{
    unpack_dms_GetDeviceCapabilitiesV2_t *pGetDeviceCapV2 =
        (unpack_dms_GetDeviceCapabilitiesV2_t*) ptr;
    uint8_t count = 0;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 1))
    {
        dms_devCaps *pDevCaps = &pGetDeviceCapV2->DevCaps;

        printf("Device Cap:\n");
        printf("MaxTXChannelRate : 0x%x\n", pDevCaps->MaxTXChannelRate );
        printf("MaxRXChannelRate : 0x%x\n", pDevCaps->MaxRXChannelRate );
        printf("DataServiceCapability : 0x%x\n",
                                           pDevCaps->DataServiceCapability );
        printf("SimCapability : 0x%x\n", pDevCaps->SimCapability );
        printf("RadioIfacesSize: 0x%x\n", pDevCaps->RadioIfacesSize);
        for(count = 0; count < pDevCaps->RadioIfacesSize; count++)
        {
            printf("radioInterface[%d] : 0x%x\n",
                    count, pDevCaps->RadioIfaces[count]);
        }
        printf("\n");
    }

    if ((pGetDeviceCapV2->pDevSrvCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 16)))
    {
        printf("Device Service Cap : 0x%x\n\n", *pGetDeviceCapV2->pDevSrvCaps );
    }

    if ((pGetDeviceCapV2->pDevVoiceCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 17)))
    {
        printf("Device Voice Cap : 0x%"PRIX64"\n\n", *pGetDeviceCapV2->pDevVoiceCaps );
    }

    if ((pGetDeviceCapV2->pDevVoiceDataCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 18)))
    {
        printf("Device Voice Data Cap : 0x%"PRIX64"\n\n", *pGetDeviceCapV2->pDevVoiceDataCaps );
    }

    if ((pGetDeviceCapV2->pDevMultiSimCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 19)))
    {
        dms_devMultiSimCaps  *pDevMultiSimCaps = pGetDeviceCapV2->pDevMultiSimCaps;
        uint8_t tCount;
        printf("Device MultiSim Cap: \n");
        printf("Max Subscriptions : 0x%x\n", pDevMultiSimCaps->MaxSubs );
        printf("Subscriptions Config List Len : 0x%x\n", pDevMultiSimCaps->SubsCfgListLen );
        for(count = 0; count < pDevMultiSimCaps->SubsCfgListLen; count++)
        {
            printf("Subs Config List[%d] :",count);
            printf("Max Active : 0x%x\n",
                    pDevMultiSimCaps->SubsCfgList[count].MaxActive);
            printf("Subs List Len : 0x%x\n",
                    pDevMultiSimCaps->SubsCfgList[count].SubsListLen);
            for (tCount=0; tCount < pDevMultiSimCaps->SubsCfgList[count].SubsListLen; tCount++)
            {
                printf("Subs List[%d] : 0x%"PRIX64"\n",tCount,
                    pDevMultiSimCaps->SubsCfgList[count].SubsList[tCount]);
            }
        }
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevMultiSimVoiceDataCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 20)))
    {
        dms_devMultiSimVoiceDataCaps  *pDevMultiSimVoiceDataCaps = pGetDeviceCapV2->pDevMultiSimVoiceDataCaps;
        printf("Device MultiSim Voice Data Cap :\n");
        printf("Max Subs : 0x%x\n", pDevMultiSimVoiceDataCaps->MaxSubs );
        printf("Max Active : 0x%x\n", pDevMultiSimVoiceDataCaps->MaxActive );
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevCurSubsCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 21)))
    {
        dms_devCurSubsCaps  *pDevCurSubsCaps = pGetDeviceCapV2->pDevCurSubsCaps;
        printf("Device Current Subscriptions Cap :\n");
        printf("Current Subs List Len : 0x%x\n", pDevCurSubsCaps->CurSubsCapsLen );
        for(count = 0; count < pDevCurSubsCaps->CurSubsCapsLen; count++)
        {
            printf("Subs List[%d] : 0x%"PRIX64"\n",
                    count, pDevCurSubsCaps->SubsCapList[count]);
        }
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevSubsVoiceDataCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 22)))
    {
        dms_devSubsVoiceDataCaps  *pDevSubsVoiceDataCaps = pGetDeviceCapV2->pDevSubsVoiceDataCaps;
        printf("Device Subscriptions Voice Data Cap :\n");
        printf("Subs Voice Data List Len : 0x%x\n", pDevSubsVoiceDataCaps->SubsVoiceDataCapLen );
        for(count = 0; count < pDevSubsVoiceDataCaps->SubsVoiceDataCapLen; count++)
        {
            printf("Subs List[%d] SubsVoiceDataCap: 0x%x, SimVoiceDataCap: 0x%x\n",
                    count,
                    pDevSubsVoiceDataCaps->SubsVoiceDataList[count].SubsVoiceDataCap,
                    pDevSubsVoiceDataCaps->SubsVoiceDataList[count].SimVoiceDataCap);
        }
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevSubsFeatureModeCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 23)))
    {
        dms_devSubsFeatureModeCaps  *pDevSubsFeatureModeCaps = pGetDeviceCapV2->pDevSubsFeatureModeCaps;
        printf("Device Subscriptions Feature Mode Cap :\n");
        printf("Subs Feature List Len : 0x%x\n", pDevSubsFeatureModeCaps->SubsFeatureLen );
        for(count = 0; count < pDevSubsFeatureModeCaps->SubsFeatureLen; count++)
        {
            printf("Subs Feature List[%d] : 0x%x\n",
                    count, pDevSubsFeatureModeCaps->SubsFeatureList[count]);
        }
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevMaxActDataSubsCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 24)))
    {
        printf("Device Max Active Data Subs Cap : 0x%x\n\n", *pGetDeviceCapV2->pDevMaxActDataSubsCaps );
    }
    if ((pGetDeviceCapV2->pDevMaxSubsCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 25)))
    {
        dms_devMaxSubsCaps  *pDevMaxSubsCaps = pGetDeviceCapV2->pDevMaxSubsCaps;
        printf("Device Max Subscriptions Cap :\n");
        printf("Max Subs List Len : 0x%x\n", pDevMaxSubsCaps->MaxSubsCapLen );
        for(count = 0; count < pDevMaxSubsCaps->MaxSubsCapLen; count++)
        {
            printf("Subs List[%d] : 0x%"PRIX64"\n",
                    count, pDevMaxSubsCaps->MaxSubsList[count]);
        }
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevMaxCfgListCaps) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 26)))
    {
        dms_devMaxCfgListCaps  *pDevMaxCfgListCaps = pGetDeviceCapV2->pDevMaxCfgListCaps;
        uint8_t tCount;
        printf("Device max Cfg List Cap: \n");
        printf("Max Subscriptions : 0x%x\n", pDevMaxCfgListCaps->MaxSubs );
        printf("Max Active : 0x%x\n", pDevMaxCfgListCaps->MaxActive );
        printf("Subscriptions Config List Len : 0x%x\n", pDevMaxCfgListCaps->DevCfgListLen );
        for(count = 0; count < pDevMaxCfgListCaps->DevCfgListLen; count++)
        {
            printf("Subs Config List[%d] :\n",count);
            printf("Subs List Len : 0x%x\n",
                    pDevMaxCfgListCaps->SubsDevList[count].SubsListLen);
            for (tCount=0; tCount < pDevMaxCfgListCaps->SubsDevList[count].SubsListLen; tCount++)
            {
                printf("Subs List[%d] : 0x%"PRIX64"\n",tCount,
                    pDevMaxCfgListCaps->SubsDevList[count].SubsList[tCount]);
            }
        }
        printf("Current Index : 0x%x\n", pDevMaxCfgListCaps->CurIndex );
        printf("\n");
    }
    if ((pGetDeviceCapV2->pDevExplicitCfgIndex) &&
               ( swi_uint256_get_bit (pGetDeviceCapV2->ParamPresenceMask, 27)))
    {
        printf("Device Explicit Cfg Index : 0x%x\n\n", *pGetDeviceCapV2->pDevExplicitCfgIndex );
    }
    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    if ( (pGetDeviceCapV2->DevCaps.DataServiceCapability > 4) ||
        (pGetDeviceCapV2->DevCaps.SimCapability > 2) ||
        (pGetDeviceCapV2->DevCaps.RadioIfacesSize < 1))
        is_matching =0;
    CHECK_WHITELIST_MASK(
        unpack_dms_GetDeviceCapabilitiesParamPresenceMaskWhiteList,
        pGetDeviceCapV2->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_GetDeviceCapabilitiesParamPresenceMaskMandatoryList,
        pGetDeviceCapV2->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SetCrashAction(void* ptr)
{
    unpack_dms_SetCrashAction_t *result =
        (unpack_dms_SetCrashAction_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    UNUSEDPARAM(result);
    if(result)
    {
        swi_uint256_print_mask (result->ParamPresenceMask);
    }
    printf("%s Result \n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_dms_SetCrashActionParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_dms_SetCrashActionParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if ( unpackRetCode != 0 )
        local_fprintf("%s, %s\n", "N/A", remark);
    else
        local_fprintf("%s\n",  "Correct");
#endif
}

void dump_SetActivationStatusCallback(void * ptr)
{
    unpack_dms_SetActivationStatusCallback_t *result =
            (unpack_dms_SetActivationStatusCallback_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetPowerSettings(void* ptr)
{
    unpack_dms_GetPower_t *ppower =
        (unpack_dms_GetPower_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, ppower->Tlvresult);
    printf("\topmode: %d\n", ppower->OperationMode);
    printf("\tHW Controlled Mode : %d\n", ppower->HardwareControlledMode);
    printf("\tOffline Reason : %d\n", ppower->OfflineReason);
    DefaultPowerStatus.mode = ppower->OperationMode;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
 
    if ( ppower->OperationMode > 7 )
        is_matching = 0;
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1) ? "Correct": "Wrong"));
#endif
}

void dump_GetUSBCompSettings(void * ptr)
{
    unpack_dms_GetUSBComp_t *result =
            (unpack_dms_GetUSBComp_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    int i =0;
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    printf("\tCurrent USB Composition: %d\n", result->USBComp);
    DefaultUSBCompSettings.USBComp = result->USBComp;
    printf("\tCount of supported USB Compositions: %d\n", result->NumSupUSBComps);
    if ( result->NumSupUSBComps!= 0xff)
    {
        for(i=0;i<result->NumSupUSBComps ; i++)
            printf("\t\tSupported USB Compositions: %d\n", result->SupUSBComps[i]);
    }

#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? "Correct": "Wrong"));
#endif
}

void dump_GetUSBCompModemSettings(void * ptr)
{
    unpack_dms_GetUSBComp_t *result =
            (unpack_dms_GetUSBComp_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        tpack_dms_SetUSBCompModemSettings.USBComp = result->USBComp;

    #if DEBUG_LOG_TO_FILE
     local_fprintf("%s\n", NA_MSG);
    #endif
}

void dump_GetUSBCompModemSettingsResult(void * ptr)
{
    unpack_dms_GetUSBComp_t *result =
            (unpack_dms_GetUSBComp_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        #if DEBUG_LOG_TO_FILE
        if (tpack_dms_SetUSBCompModemSettings.USBComp != result->USBComp)
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        else
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
        #endif
    }
    else
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("%s\n", NA_MSG);
        #endif
    }    
}

void dump_GetUSBCompChange(void * ptr)
{
    unpack_dms_GetUSBComp_t *result =
            (unpack_dms_GetUSBComp_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    #if DEBUG_LOG_TO_FILE
    if(result->USBComp != tunpack_dms_GetUSBCompResult.USBComp) 
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}


void dump_dms_GetCustFeatureDefaultSettings(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        tpack_dms_SetCustFeatureModemSettings.DHCPRelayEnabled = result->DHCPRelayEnabled;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        tpack_dms_SetCustFeatureModemSettings.DisableIMSI = result->DisableIMSI;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        tpack_dms_SetCustFeatureModemSettings.GpsEnable = result->GpsEnable;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
        tpack_dms_SetCustFeatureModemSettings.GPSLPM = result->GPSLPM;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        tpack_dms_SetCustFeatureModemSettings.GPSSel = result->GPSSel;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        tpack_dms_SetCustFeatureModemSettings.IPFamSupport = result->IPFamSupport;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        tpack_dms_SetCustFeatureModemSettings.IsVoiceEnabled = result->IsVoiceEnabled;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        tpack_dms_SetCustFeatureModemSettings.RMAutoConnect = result->RMAutoConnect;
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        tpack_dms_SetCustFeatureModemSettings.SMSSupport = result->SMSSupport;

   #if DEBUG_LOG_TO_FILE
       CHECK_WHITELIST_MASK(
           unpack_dms_GetCustFeatureParamPresenceMaskWhiteList,
           result->ParamPresenceMask);
       CHECK_MANDATORYLIST_MASK(
           unpack_dms_GetCustFeatureParamPresenceMaskMandatoryList,
           result->ParamPresenceMask);
       if ( unpackRetCode != 0 )
           local_fprintf("%s, %s\n", "N/A", remark);
       else
           local_fprintf("%s\n",  "N/A");
   #endif
}
void dump_GetCustFeatureGPSEnabled(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(tunpack_dms_GetCustFeatureGPSEnabledResult.GpsEnable != result->GpsEnable)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_GetCustFeatureGPSDisabled(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(tunpack_dms_GetCustFeatureGPSDisabledResult.GpsEnable != result->GpsEnable)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_dms_GetCustFeatureModemSettingsResult(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;
    uint8_t cmpResult = 1;

    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    {
        if (tpack_dms_SetCustFeatureModemSettings.DHCPRelayEnabled != result->DHCPRelayEnabled)
            cmpResult = 0;
    }    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        if (tpack_dms_SetCustFeatureModemSettings.DisableIMSI != result->DisableIMSI)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if (tpack_dms_SetCustFeatureModemSettings.GpsEnable != result->GpsEnable)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
    {
        if (tpack_dms_SetCustFeatureModemSettings.GPSLPM != result->GPSLPM)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        if (tpack_dms_SetCustFeatureModemSettings.GPSSel != result->GPSSel)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        if (tpack_dms_SetCustFeatureModemSettings.IPFamSupport != result->IPFamSupport)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    {
        if(tpack_dms_SetCustFeatureModemSettings.IsVoiceEnabled != result->IsVoiceEnabled)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        if (tpack_dms_SetCustFeatureModemSettings.RMAutoConnect != result->RMAutoConnect)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    {   
        if (tpack_dms_SetCustFeatureModemSettings.SMSSupport != result->SMSSupport)
            cmpResult = 0;
    }

    #if DEBUG_LOG_TO_FILE
    if(cmpResult == 0)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}


void dump_GetCustFeatureDHCPRelayEnabled(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(tunpack_dms_GetCustFeatureDHCPRelayEnabledResult.DHCPRelayEnabled != result->DHCPRelayEnabled)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}


void dump_GetCustFeatureSettings(void * ptr)
{
    unpack_dms_GetCustFeature_t *result =
            (unpack_dms_GetCustFeature_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    printf("\tDHCP Relay Enable: %d\n", result->DHCPRelayEnabled);
    printf("\tIMSI: %d\n", result->DisableIMSI);
    printf("\tGPS Enable: %d\n", result->GpsEnable);
    printf("\tGPS LPM Enable: %d\n", result->GPSLPM);
    printf("\tGPS Sel Enable: %d\n", result->GPSSel);
    printf("\tIP Family Support: %d\n", result->IPFamSupport);
    printf("\tVoice Enable: %d\n", result->IsVoiceEnabled);
    printf("\tRM Auto Connect: %d\n", result->RMAutoConnect);
    printf("\tSMS Support: %d\n", result->SMSSupport);
    DefaultCusFeatureSettings.DHCPRelayEnabled = result->DHCPRelayEnabled;
    DefaultCusFeatureSettings.DisableIMSI = result->DisableIMSI;
    DefaultCusFeatureSettings.GpsEnable = result->GpsEnable;
    DefaultCusFeatureSettings.GPSLPM = result->GPSLPM;
    DefaultCusFeatureSettings.GPSSel = result->GPSSel;
    DefaultCusFeatureSettings.IPFamSupport = result->IPFamSupport;
    DefaultCusFeatureSettings.IsVoiceEnabled = result->IsVoiceEnabled;
    DefaultCusFeatureSettings.RMAutoConnect = result->RMAutoConnect;
    DefaultCusFeatureSettings.SMSSupport = result->SMSSupport;    

#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? "Correct": "Wrong"));
#endif
}

void dump_GetCrashActionSettings(void * ptr)
{
    unpack_dms_GetCrashAction_t *result =
            (unpack_dms_GetCrashAction_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    printf("\tCrash State: %d\n", result->DevCrashState);
    DefaultCrashActionSettings.crashAction = result->DevCrashState;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetCrashActionModemSettings(void * ptr)
{
    unpack_dms_GetCrashAction_t *result =
            (unpack_dms_GetCrashAction_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    tpack_dms_SetCrashActionModemSettings.crashAction = result->DevCrashState;

    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", NA_MSG);
    #endif
}

void dump_GetCrashActionModemSettingsResult(void * ptr)
{
    unpack_dms_GetCrashAction_t *result =
            (unpack_dms_GetCrashAction_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    #if DEBUG_LOG_TO_FILE
    if (tpack_dms_SetCrashActionModemSettings.crashAction != result->DevCrashState)
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_GetCrashActionResult(void * ptr)
{
    unpack_dms_GetCrashAction_t *result =
            (unpack_dms_GetCrashAction_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    #if DEBUG_LOG_TO_FILE
    if(memcmp(result,&tunpack_dms_GetCrashActionResult,sizeof(unpack_dms_GetCrashAction_t)) != 0) 
    {
        local_fprintf("%s\n",  FAILED_MSG);
    }
    else
    {
        local_fprintf("%s\n",  SUCCESS_MSG);
    }
    #endif
}

void dump_GetCustFeaturesV2Settings(void * ptr)
{
    unpack_dms_GetCustFeaturesV2_t *result =
            (unpack_dms_GetCustFeaturesV2_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->GetCustomFeatureV2.pCustSettingInfo) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 32)))
    {
        int i = 0;
        uint8_t temp_value=0;
        uint64_t value = 0;
        printf( "Customization Setting:\n"\
                     "id length: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->id_length );
        
        printf("custom id: %s \n", result->GetCustomFeatureV2.pCustSettingInfo->cust_id );
        memcpy(&DefaultCusFeaturesV2Settings.cust_id,
            result->GetCustomFeatureV2.pCustSettingInfo->cust_id,
            result->GetCustomFeatureV2.pCustSettingInfo->id_length);
        memcpy(&DefaultCusFeaturesV2Settings.cust_value,
            result->GetCustomFeatureV2.pCustSettingInfo->cust_value,
            result->GetCustomFeatureV2.pCustSettingInfo->value_length);
        printf("value length: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->value_length );
        for ( i = 0; i < result->GetCustomFeatureV2.pCustSettingInfo->value_length; i++)
        {
            temp_value = result->GetCustomFeatureV2.pCustSettingInfo->cust_value[i];
            value = value | temp_value << (8*i);
        }
        printf(  "value id: %"PRIu64"\n", value);
        printf(  "custom attribute: %d\n", result->GetCustomFeatureV2.pCustSettingInfo->cust_attr);
    }

    if (( result->GetCustomFeatureV2.pCustSettingList != NULL) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 33)))
    {
        int i = 0;
        uint8_t temp_value=0;
        uint64_t value = 0;
        
        printf("\nCustomization List:\n"\
                     "list type: %d\n",  result->GetCustomFeatureV2.pCustSettingList->list_type);
        printf("number instances: %d\n",  result->GetCustomFeatureV2.pCustSettingList->num_instances);
        for ( i = 0; i < result->GetCustomFeatureV2.pCustSettingList->num_instances; i++)
        {
            int j=0;
            /* reset custom value for every single custom filed */
            value = 0;
            printf("Instance[%i]:\n", i);
            printf("id length: %d\n",  result->GetCustomFeatureV2.pCustSettingList->custSetting[i].id_length);
            printf("custtom id: %s\n",  result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_id);

            for ( j = 0; j < result->GetCustomFeatureV2.pCustSettingList->custSetting[i].value_length; j++ )
            {
                temp_value = result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_value[j];
                value = value | temp_value << (8*j);
            }
            printf("custom value : %"PRIu64"\n", value);
            printf("custom attribute: %d\n", result->GetCustomFeatureV2.pCustSettingList->custSetting[i].cust_attr);
        }
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if ( (result->GetCustomFeatureV2.pCustSettingInfo != NULL ) &&
          (result->GetCustomFeatureV2.pCustSettingList != NULL))
    {
        if (result->GetCustomFeatureV2.pCustSettingList->num_instances > 0)
           is_matching = 1;
    }
    if (strstr( (char*)model_id, "MC73") ||
        strstr( (char*)model_id, "EM73"))
        sprintf(remark," This API is not supported for %s", model_id );

    local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
    local_fprintf("%s\n", remark);
#endif
}

void dump_GetCustFeaturesV2ModemSettings(void * ptr)
{
    unpack_dms_GetCustFeaturesV2_t *result =
            (unpack_dms_GetCustFeaturesV2_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if((result->GetCustomFeatureV2.pCustSettingInfo) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 32)))
    {

        memcpy(tpack_dms_SetCustFeaturesV2ModemSettings.cust_id,
            result->GetCustomFeatureV2.pCustSettingInfo->cust_id,
            result->GetCustomFeatureV2.pCustSettingInfo->id_length);
        memcpy(tpack_dms_SetCustFeaturesV2ModemSettings.cust_value,
            result->GetCustomFeatureV2.pCustSettingInfo->cust_value,
            result->GetCustomFeatureV2.pCustSettingInfo->value_length);
        tpack_dms_SetCustFeaturesV2ModemSettings.value_length = 
                         result->GetCustomFeatureV2.pCustSettingInfo->value_length;

    }
    #if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", NA_MSG);
    #endif
}

void dump_GetCustFeaturesV2ModemSettingsResult(void * ptr)
{
    unpack_dms_GetCustFeaturesV2_t *result =
            (unpack_dms_GetCustFeaturesV2_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if((result->GetCustomFeatureV2.pCustSettingInfo) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 32)))
    {
        int temp1, temp2;

        #if DEBUG_LOG_TO_FILE
        temp1 = memcmp(tpack_dms_SetCustFeaturesV2ModemSettings.cust_id,
                   result->GetCustomFeatureV2.pCustSettingInfo->cust_id,
                   result->GetCustomFeatureV2.pCustSettingInfo->id_length);

        temp2 = memcmp(tpack_dms_SetCustFeaturesV2ModemSettings.cust_value,
                   result->GetCustomFeatureV2.pCustSettingInfo->cust_value,
                   result->GetCustomFeatureV2.pCustSettingInfo->value_length);

        if ((!temp1) && (!temp2))
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
        else
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        #endif
    }
    else
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("\n");
        #endif
    }
}

void dump_GetCustFeaturesV2Result(void * ptr)
{
    unpack_dms_GetCustFeaturesV2_t *result =
            (unpack_dms_GetCustFeaturesV2_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if((result->GetCustomFeatureV2.pCustSettingInfo) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 32)))
    {
        int temp1, temp2;

        #if DEBUG_LOG_TO_FILE
        temp1 = memcmp(tunpack_dms_GetCustFeaturesV2Resp.GetCustomFeatureV2.pCustSettingInfo->cust_id,
                   result->GetCustomFeatureV2.pCustSettingInfo->cust_id,
                   tunpack_dms_GetCustFeaturesV2Resp.GetCustomFeatureV2.pCustSettingInfo->id_length);

        temp2 = memcmp(tunpack_dms_GetCustFeaturesV2Resp.GetCustomFeatureV2.pCustSettingInfo->cust_value,
                   result->GetCustomFeatureV2.pCustSettingInfo->cust_value,
                   tunpack_dms_GetCustFeaturesV2Resp.GetCustomFeatureV2.pCustSettingInfo->value_length);

        if ((!temp1) && (!temp2))
        {
            local_fprintf("%s\n",  SUCCESS_MSG);
        }
        else
        {
            local_fprintf("%s\n",  FAILED_MSG);
        }
        #endif
    }
    else
    {
        #if DEBUG_LOG_TO_FILE
        local_fprintf("\n");
        #endif
    }
}

void dump_SLQSSwiGetDyingGaspCfgModemSettings(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *resp =
        (unpack_dms_SLQSSwiGetDyingGaspCfg_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if (!resp->pGetDyingGaspCfg)
        return;
    swi_uint256_print_mask (resp->ParamPresenceMask);
    if((resp->pGetDyingGaspCfg->pDestSMSNum) && 
       (swi_uint256_get_bit (resp->ParamPresenceMask, 16)))
    {
        memcpy(tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings.pDestSMSNum,
               resp->pGetDyingGaspCfg->pDestSMSNum,
               SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH);
    }

    if((resp->pGetDyingGaspCfg->pDestSMSContent) && 
       (swi_uint256_get_bit (resp->ParamPresenceMask, 17)))
    {
        memcpy(tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings.pDestSMSContent,
               resp->pGetDyingGaspCfg->pDestSMSContent,
               SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetDyingGaspCfgModemSettingsResult(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *resp =
        (unpack_dms_SLQSSwiGetDyingGaspCfg_t*) ptr;
    uint8_t cmpResult = 0;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if (!resp->pGetDyingGaspCfg)
        return;
    swi_uint256_print_mask (resp->ParamPresenceMask);
    if((resp->pGetDyingGaspCfg->pDestSMSNum) && 
       (swi_uint256_get_bit (resp->ParamPresenceMask, 16)))
    {
        cmpResult = memcmp(tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings.pDestSMSNum,
                    resp->pGetDyingGaspCfg->pDestSMSNum,
                    SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH);
    }

    if((resp->pGetDyingGaspCfg->pDestSMSContent) && 
       (swi_uint256_get_bit (resp->ParamPresenceMask, 17)))
    {

        cmpResult = memcmp(tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings.pDestSMSContent,
               resp->pGetDyingGaspCfg->pDestSMSContent,
               SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    if (cmpResult != 0)
        is_matching = 0;

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n", ((is_matching == 1)  ? SUCCESS_MSG: FAILED_MSG));
#endif
}

void dump_SLQSSwiGetDyingGaspCfgResult(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *resp =
        (unpack_dms_SLQSSwiGetDyingGaspCfg_t*) ptr;
    uint8_t cmpResult = 0;

    if (!resp->pGetDyingGaspCfg)
        return;

    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (resp->ParamPresenceMask);
    if((resp->pGetDyingGaspCfg->pDestSMSNum) && 
       ((swi_uint256_get_bit (resp->ParamPresenceMask, 16))))
    {
        cmpResult = memcmp(tunpack_dms_SLQSSwiGetDyingGaspCfgResult.pGetDyingGaspCfg->pDestSMSNum,
                    resp->pGetDyingGaspCfg->pDestSMSNum,
                    SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH);
    }

    if((resp->pGetDyingGaspCfg->pDestSMSContent) && 
       (swi_uint256_get_bit (resp->ParamPresenceMask, 17)))
    {
        cmpResult = memcmp(tunpack_dms_SLQSSwiGetDyingGaspCfgResult.pGetDyingGaspCfg->pDestSMSContent,
               resp->pGetDyingGaspCfg->pDestSMSContent,
               SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH);
    }
    #if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    if (cmpResult != 0)
        is_matching = 0;

    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? SUCCESS_MSG: FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n", ((is_matching == 1)  ? SUCCESS_MSG: FAILED_MSG));
    #endif
}

void dump_SLQSSwiGetDyingGaspCfgSettings(void * ptr)
{
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *result =
        (unpack_dms_SLQSSwiGetDyingGaspCfg_t*) ptr;
    if(result)
    {
        if(result->pGetDyingGaspCfg)
        {
            printf("%s SMS Content: %s\n",__FUNCTION__, result->pGetDyingGaspCfg->pDestSMSContent);
            printf("%s Destination Number: %s\n",__FUNCTION__, result->pGetDyingGaspCfg->pDestSMSNum);
            if(result->pGetDyingGaspCfg->pDestSMSContent)
            memcpy(DefaultSwiSetDyingGaspCfg.pDestSMSContent,
                result->pGetDyingGaspCfg->pDestSMSContent,
                SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH);
            if(result->pGetDyingGaspCfg->pDestSMSNum)
            memcpy(DefaultSwiSetDyingGaspCfg.pDestSMSNum,
                result->pGetDyingGaspCfg->pDestSMSNum,
                SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH);
                
        }
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetHostDevInfoSettings(void * ptr)
{
    unpack_dms_SLQSSwiGetHostDevInfo_t *result =
            (unpack_dms_SLQSSwiGetHostDevInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);

    if (( result->manString!= NULL) && (result->manString[0]!=0x7f))
    {
        printf("Manufacturer : %s\n", result->manString);
        memcpy(&DefaultSLQSSwiSetHostDevInfoSettings.manString,
            &result->manString,
            strlen(result->manString));
    }
    if (( result->modelString!= NULL) && (result->modelString[0]!=0x7f))
    {
        printf("Model String : %s\n", result->modelString);
        memcpy(&DefaultSLQSSwiSetHostDevInfoSettings.modelString,
            &result->modelString,
            strlen(result->modelString));
    }
    if (( result->swVerString!= NULL) && (result->swVerString[0]!=0x7f))
    {
        printf("Software Version : %s\n", result->swVerString);
        memcpy(&DefaultSLQSSwiSetHostDevInfoSettings.swVerString,
            &result->swVerString,
            strlen(result->swVerString));
    }
    if (( result->plasmaIDString!= NULL) && (result->plasmaIDString[0]!=0x7f))
    {
        printf("Plasma ID String : %s\n", result->plasmaIDString);
        memcpy(&DefaultSLQSSwiSetHostDevInfoSettings.plasmaIDString,
            &result->plasmaIDString,
            strlen(result->plasmaIDString));
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetOSInfoSettings(void * ptr)
{
    unpack_dms_SLQSSwiGetOSInfo_t *result =
            (unpack_dms_SLQSSwiGetOSInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);

    if ( result->nameString!= NULL)
    {
        printf("Operating System Name : %s\n", result->nameString);
        memcpy(&DefaultSwiSetOSInfoSettings.nameString,
            &result->nameString,
            strlen(result->nameString));
    }
    if ( result->versionString!= NULL)
    {
        printf("Operating System Version : %s\n", result->versionString);
        memcpy(&DefaultSwiSetOSInfoSettings.versionString,
            &result->versionString,
            strlen(result->versionString));
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSDmsSwiGetPCInfo(void* ptr)
{
    unpack_dms_SLQSDmsSwiGetPCInfo_t *pPCInfo =
        (unpack_dms_SLQSDmsSwiGetPCInfo_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (pPCInfo->ParamPresenceMask, 1))
    printf("opMode         : 0x%x\n", pPCInfo->opMode);
    if ((pPCInfo->has_LpmFlag) &&
        (swi_uint256_get_bit (pPCInfo->ParamPresenceMask, 16)))
        printf("LPM Flag       : %d\n", pPCInfo->LpmFlag);
    if ((pPCInfo->has_Wdisable) &&
        (swi_uint256_get_bit (pPCInfo->ParamPresenceMask, 17)))
        printf("W-Disable      : 0x%x\n", pPCInfo->Wdisable);
    if ((pPCInfo->has_PowerOffMode) &&
        (swi_uint256_get_bit (pPCInfo->ParamPresenceMask, 18)))
        printf("Power Off Mode : 0x%x\n", pPCInfo->PowerOffMode);
    if ((pPCInfo->has_PersistentLpm) &&
        (swi_uint256_get_bit (pPCInfo->ParamPresenceMask, 19)))
        printf("Persistent LPM : 0x%x\n", pPCInfo->PersistentLpm);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetPowerSaveModeConfig(void* ptr)
{
    unpack_dms_SLQSGetPowerSaveModeConfig_t *pPSMConfigParams =
        (unpack_dms_SLQSGetPowerSaveModeConfig_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }

    if(pPSMConfigParams->pPsmEnableState && pPSMConfigParams->pPsmEnableState->TlvPresent)
        printf("PSM Enable State      : %d\n", pPSMConfigParams->pPsmEnableState->enableState);
    if(pPSMConfigParams->pDurationThreshold && pPSMConfigParams->pDurationThreshold->TlvPresent)
        printf("Duration Threshold    : %d\n", pPSMConfigParams->pDurationThreshold->durationThreshold);
    if(pPSMConfigParams->pDurationDueToOOS && pPSMConfigParams->pDurationDueToOOS->TlvPresent)
        printf("Duration due to OOS   : %d\n", pPSMConfigParams->pDurationDueToOOS->durationDueToOOS);
    if(pPSMConfigParams->pRandomizationWindow && pPSMConfigParams->pRandomizationWindow->TlvPresent)
        printf("Randomization Window  : %d\n", pPSMConfigParams->pRandomizationWindow->randomizationWindow);
    if(pPSMConfigParams->pActiveTimer && pPSMConfigParams->pActiveTimer->TlvPresent)
        printf("Active Timer          : %d\n", pPSMConfigParams->pActiveTimer->activeTimer);
    if(pPSMConfigParams->pPeriodicUpdateTimer && pPSMConfigParams->pPeriodicUpdateTimer->TlvPresent)
        printf("Periodic Update Timer : %d\n", pPSMConfigParams->pPeriodicUpdateTimer->periodicUpdateTimer);
    if(pPSMConfigParams->pEarlyWakeupTime && pPSMConfigParams->pEarlyWakeupTime->TlvPresent)
        printf("Early Wakeup Time     : %d\n", pPSMConfigParams->pEarlyWakeupTime->earlyWakeupTime);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSetPowerSaveModeConfig(void * ptr)
{
    unpack_dms_SLQSSetPowerSaveModeConfig_t *result =
            (unpack_dms_SLQSSetPowerSaveModeConfig_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

int restore_pack_dms_SetUSBComp(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetUSBComp_t *reqArg)
{
    int rtn = eQCWWAN_ERR_ENUM_BEGIN;
    if(reqArg->USBComp==0)
        return rtn;
    printf("Restore USB Comp:%d\n",reqArg->USBComp);
    rtn = pack_dms_SetUSBComp(pCtx,pReqBuf,pLen,reqArg);
    return rtn;
}

void dump_SwiUimSelect(void * ptr)
{
    unpack_dms_SwiUimSelect_t *result =
            (unpack_dms_SwiUimSelect_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s\n",  "N/A");
    #endif
        return;
    }
    printf("%s Result : %d\n",__FUNCTION__, result->Tlvresult);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((result->Tlvresult == eQCWWAN_ERR_NONE)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((result->Tlvresult == eQCWWAN_ERR_NONE) ? "Correct": "Wrong"));
#endif
}

void dump_SLQSDmsSwiGetUimSelection(void* ptr)
{
    unpack_dms_SLQSDmsSwiGetUimSelection_t *pUIMSelection =
        (unpack_dms_SLQSDmsSwiGetUimSelection_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if(swi_uint256_get_bit (pUIMSelection->ParamPresenceMask, 1))
        printf("Active Selected UIM Slot    : 0X%x\n", pUIMSelection->uimSelect);
    if ((pUIMSelection->pUimAutoSwitchActSlot) &&
        (swi_uint256_get_bit (pUIMSelection->ParamPresenceMask, 16)))
            printf("UIM Auto Switch Active SLot : 0X%x\n",
                    pUIMSelection->pUimAutoSwitchActSlot->uimAutoSwitchActSlot);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    if(swi_uint256_get_bit (pUIMSelection->ParamPresenceMask, 1)==0)
        is_matching = 0;
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching == 1)  ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
        local_fprintf("%s\n",  ((is_matching == 1)  ? "Correct": "Wrong"));
#endif

}

void dump_SetEventReport_ind(void* ptr)
{
    unpack_dms_SetEventReport_ind_t *result =
        (unpack_dms_SetEventReport_ind_t*) ptr;

    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    if((result->ActivationStatusTlv.TlvPresent!=0)&& 
         (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("DMS evert report indication activation Status :%d\n",
            result->ActivationStatusTlv.activationStatus);
    }
    if((result->OperatingModeTlv.TlvPresent!=0) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 20)))
    {
        printf("DMS evert report indication operatingMode :%d\n",
            result->OperatingModeTlv.operatingMode);
    }
}

void dump_SLQSDmsSwiGetResetInfo_Ind(void * ptr)
{
    unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t *result =
            (unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
    if((result->Tlvresult==eQCWWAN_ERR_NONE) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
    {
        printf("\tReset Type: %d\n", result->type);
        printf("\tReset Source: %d\n", result->source);
    }
}

void dump_PSMCfgChange_ind(void * ptr)
{
    unpack_dms_PSMCfgChange_ind_t *result =
            (unpack_dms_PSMCfgChange_ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if((result->EnableState.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
        printf( "Enable State: %d\n", result->EnableState.EnableStateInd );
    if((result->ActiveTimer.TlvPresent) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
        printf( "Active Timer: %d\n", result->ActiveTimer.ActiveTimerInd );
    if((result->PeriodicUpdateTimer.TlvPresent) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 18)))
        printf( "Periodic Update Timer: %d\n", result->PeriodicUpdateTimer.PeriodicUpdateTimerInd );
}

void dump_SwiEventReportCallBack_ind(void * ptr)
{
    unpack_dms_SwiEventReportCallBack_ind_t *result =
            (unpack_dms_SwiEventReportCallBack_ind_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }

    if((result->TempTlv.TlvPresent) &&
        (swi_uint256_get_bit (result->ParamPresenceMask, 16)))
    {
        printf("QMI_DMS_SWI_SET_EVENT_REPORT IND temp State :%d temperature %d\n",
                result->TempTlv.TempStat,
                result->TempTlv.Temperature);
    }

    if((result->VoltTlv.TlvPresent) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        printf("QMI_DMS_SWI_SET_EVENT_REPORT IND volt State :%d voltage %d\n",
                result->VoltTlv.VoltStat,
                result->VoltTlv.Voltage);
    }

    if((result->UimStatusTlv.TlvPresent) && 
        (swi_uint256_get_bit (result->ParamPresenceMask, 19)))
    {
        printf("QMI_DMS_SWI_SET_EVENT_REPORT IND UIM status interface :%d event %d\n",
                result->UimStatusTlv.intf,
                result->UimStatusTlv.event);
    }
}

testitem_t oemapitest_dms[] = {

     {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[1], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower",
        &SetPowerOnline,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower",
        &SetPowerResp, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapability, "unpack_dms_SQLSSLQSGetBandCapability",
        &dmsSLQSGetBandCapability, dump_SLQSGetBandCapability
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapabilityExt, "unpack_dms_SQLSSLQSGetBandCapabilityExt",
        &dmsSLQSGetBandCapabilityExt, dump_SLQSGetBandCapabilityExt
    },
    {
        (pack_func_item) &pack_dms_SetCrashAction, "pack_dms_SetCrashAction",
        &tpack_dms_SetCrashAction,
        (unpack_func_item) &unpack_dms_SetCrashAction, "unpack_dms_SetCrashAction",
        &tunpack_dms_SetCrashAction, dump_SetCrashAction
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetCrashInfo, "pack_dms_SLQSSwiGetCrashInfo",
        &packdmsSLQSSwiGetCrashInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiGetCrashInfo, "unpack_dms_SLQSSwiGetCrashInfo",
        &unpackdmsSLQSSwiGetCrashInfo, dump_SLQSSwiGetCrashInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspStatistics, "pack_dms_SLQSSwiGetDyingGaspStatistics",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspStatistics, "unpack_dms_SLQSSwiGetDyingGaspStatistics",
        &unpackdmsSLQSSwiGetDyingGaspStatistics, dump_SLQSSwiGetDyingGaspStatistics
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetResetInfo, "pack_dms_SLQSDmsSwiGetResetInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetResetInfo, "unpack_dms_SLQSDmsSwiGetResetInfo",
        &unpackdmsSLQSDmsSwiGetResetInfo, dump_SLQSDmsSwiGetResetInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg",
        &packdmsSLQSSwiSetDyingGaspCfg,
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_GetDeviceSerialNumbers, "pack_dms_GetDeviceSerialNumbers",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceSerialNumbers, "unpack_dms_GetDeviceSerialNumbers",
        &DeviceSerialNumbers, dump_GetDeviceSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_SwiSetEventReport, "pack_dms_SwiSetEventReport",
        &SwiEventSettingsUINStatus,
        (unpack_func_item) &unpack_dms_SwiSetEventReport, "unpack_dms_SwiSetEventReport",
        &SwiSetEventResp, dump_SwiSetEventReport
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature ModemSettings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature ModemSettings",
        &GetCusFeatureResp, dump_dms_GetCustFeatureDefaultSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature",
        &GetCusFeatureResp, dump_GetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature GPSEnabled",
        (void*)&tpack_dms_SetCustFeatureGPSEnabled, 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature GPSEnabled",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Result",
        &GetCusFeatureResp, dump_GetCustFeatureGPSEnabled
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature GPSDisabled",
        (void*)&tpack_dms_SetCustFeatureGPSDisabled, 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature GPSDisabled",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Result",
        &GetCusFeatureResp, dump_GetCustFeatureGPSDisabled
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature DHCPRelayEnabled",
        (void*)&tpack_dms_SetCustFeatureDHCPRelayEnabled, 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature DHCPRelayEnabled",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Result",
        &GetCusFeatureResp, dump_GetCustFeatureDHCPRelayEnabled
    },
    // Restore SetCusFeature
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature Restore",
        (void*)&tpack_dms_SetCustFeatureModemSettings, 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature Restore",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Restore Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Restore Result",
        &GetCusFeatureResp, dump_dms_GetCustFeatureModemSettingsResult
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2 ModemSettings",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2 ModemSettings",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2ModemSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_dms_SetCustFeaturesV2, "pack_dms_SetCustFeaturesV2",
        &tpack_dms_SetCustFeaturesV2Change,
        (unpack_func_item) &unpack_dms_SetCustFeaturesV2, "unpack_dms_SetCustFeaturesV2",
        &SetCustFeaturesV2Resp, dump_SetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2Result
    },
    //Restore
    {
        (pack_func_item) &pack_dms_SetCustFeaturesV2, "pack_dms_SetCustFeaturesV2",
        &tpack_dms_SetCustFeaturesV2ModemSettings,
        (unpack_func_item) &unpack_dms_SetCustFeaturesV2, "unpack_dms_SetCustFeaturesV2",
        &SetCustFeaturesV2Resp, dump_SetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2 RestoreResult",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2 RestoreResult",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2ModemSettingsResult
    },
//3
    {
        (pack_func_item) &pack_dms_GetDeviceCapabilities, "pack_dms_GetDeviceCapabilities",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceCapabilities, "unpack_dms_GetDeviceCapabilities",
        &tunpack_dms_GetDeviceCapabilities, dump_GetDeviceCapabilities
    },
//4
    {
        (pack_func_item) &pack_dms_GetDeviceHardwareRev, "pack_dms_GetDeviceHardwareRev",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceHardwareRev, "unpack_dms_GetDeviceHardwareRev",
        &HardwareRev, dump_GetDeviceHardwareRev
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareInfo, "pack_dms_GetFirmwareInfo",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareInfo, "unpack_dms_GetFirmwareInfo",
        &fwinfo, dump_GetFirmwareInfo
    },

    {
        (pack_func_item) &pack_dms_GetFirmwareRevisions, "pack_dms_GetFirmwareRevisions",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareRevisions, "unpack_dms_GetFirmwareRevisions",
        &FirmwareRevisions, dump_FirmwareRevisions
    },
//7     WP76xx depreciated, use UIM instead
    {
        (pack_func_item) &pack_dms_GetIMSI, "pack_dms_GetIMSI",
        NULL,
        (unpack_func_item) &unpack_dms_GetIMSI, "unpack_dms_GetIMSI",
        &imsi, dump_GetIMSI
    },
    {
        (pack_func_item) &pack_dms_GetModelID, "pack_dms_GetModelID",
        NULL, 
        (unpack_func_item) &unpack_dms_GetModelID, "unpack_dms_GetModelID",
        &model, dump_GetModelId
    },
    {
        (pack_func_item) &pack_dms_GetDeviceCapabilitiesV2, "pack_dms_GetDeviceCapabilitiesV2",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceCapabilitiesV2, "unpack_dms_GetDeviceCapabilitiesV2",
        &tunpack_dms_GetDeviceCapabilitiesV2, dump_GetDeviceCapabilitiesV2
    },
    {
        (pack_func_item) &pack_dms_GetNetworkTime, "pack_dms_GetNetworkTime",
        NULL, 
        (unpack_func_item) &unpack_dms_GetNetworkTime, "unpack_dms_GetNetworkTime",
        &Time, dump_GetNetworkTime
    },
    {
        (pack_func_item) &pack_dms_GetNetworkTimeV2, "pack_dms_GetNetworkTimeV2",
        NULL,
        (unpack_func_item) &unpack_dms_GetNetworkTimeV2, "unpack_dms_GetNetworkTimeV2",
        &TimeV2, dump_GetNetworkTimeV2
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower Modem Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower Modem settings",
        &power, dump_dms_GetPowerModemSettings
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower",
        &power, dump_GetPower
    },
// Set low power mode, it may disrupt the device
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower Change",
        &SetPowerLow,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower Change",
        &SetPowerResp, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower Result",
        &power, dump_GetPowerChangeResult
    },
    // Restore SetPower
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower Restore",
        &tpack_dms_SetPowerModemSettings,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower Restore",
        &SetPowerResp, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower Restore Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower Restore Result",
        &power, dump_dms_GetPowerModemSettingsResult
    },
//11 WP76xx depreciated
    {
        (pack_func_item) &pack_dms_GetPRLVersion, "pack_dms_GetPRLVersion",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPRLVersion, "unpack_dms_GetPRLVersion",
        &PRLVersion, dump_GetPRLVersion
    },
    {
        (pack_func_item) &pack_dms_GetSerialNumbers, "pack_dms_GetSerialNumbers",
        NULL,
        (unpack_func_item) &unpack_dms_GetSerialNumbers, "unpack_dms_GetSerialNumbers",
        &tunpack_dms_GetSerialNumbers, dump_GetSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp Modem Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp Modem Settings",
        &GetUSBComp, dump_GetUSBCompModemSettings
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp",
        &GetUSBComp, dump_GetUSBComp
    },
    {//MC/EM7455 might fail Version < SWI9X30C_02.05.07.00
        (pack_func_item) &pack_dms_SetUSBComp, "pack_dms_SetUSBComp",
        &tpack_dms_SetUSBCompChange,
        (unpack_func_item) &unpack_dms_SetUSBComp, "unpack_dms_SetUSBComp",
        &SetUSBComp, dump_SetUSBComp
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp",
        &GetUSBComp, dump_GetUSBCompChange
    },
    {//Restore USBComp
        (pack_func_item) &pack_dms_SetUSBComp, "pack_dms_SetUSBComp Restore",
        &tpack_dms_SetUSBCompModemSettings,
        (unpack_func_item) &unpack_dms_SetUSBComp, "unpack_dms_SetUSBComp Restore",
        &SetUSBComp, dump_SetUSBComp
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp Restore Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp Restore Result",
        &GetUSBComp, dump_GetUSBCompModemSettingsResult
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction Modem Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction Modem Settings",
        &GetCrashAction, dump_GetCrashActionModemSettings
    },
    {
        (pack_func_item) &pack_dms_SetCrashAction, "pack_dms_SetCrashAction Change",
        &tpack_dms_SetCrashActionChange,
        (unpack_func_item) &unpack_dms_SetCrashAction, "unpack_dms_SetCrashAction Change",
        &tunpack_dms_SetCrashAction, dump_SetCrashAction
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction Get Settings",
        &GetCrashAction, dump_GetCrashActionResult
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetCrashInfo, "pack_dms_SLQSSwiGetCrashInfo",
        &packdmsSLQSSwiGetCrashInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiGetCrashInfo, "unpack_dms_SLQSSwiGetCrashInfo",
        &unpackdmsSLQSSwiGetCrashInfo, dump_SLQSSwiGetCrashInfo
    },
    {//Restore crash action
        (pack_func_item) &pack_dms_SetCrashAction, "pack_dms_SetCrashAction Restore",
        &tpack_dms_SetCrashActionModemSettings,
        (unpack_func_item) &unpack_dms_SetCrashAction, "unpack_dms_SetCrashAction Restore",
        &tunpack_dms_SetCrashAction, dump_SetCrashAction
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction Restore Result",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction Restore Result",
        &GetCrashAction, dump_GetCrashActionModemSettingsResult
    },
//14  need SIM store with phone number
    {
        (pack_func_item) &pack_dms_GetVoiceNumber, "pack_dms_GetVoiceNumber",
        NULL, 
        (unpack_func_item) &unpack_dms_GetVoiceNumber, "unpack_dms_GetVoiceNumber",
        &VoiceNumber, dump_GetVoiceNumber
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg Modem Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg Modem Settings",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfgModemSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg Change",
        &tpack_dms_SLQSSwiSetDyingGaspCfgChange, 
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg Change",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg Result",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg Result",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfgResult
    },
    // Restore to original
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg Restore",
        &tpack_dms_SLQSSwiSetDyingGaspCfgModemSettings, 
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg Restore",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg Restore Result",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg Restore Result",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfgModemSettingsResult
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[0], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetFirmwareCurr, "pack_dms_SLQSSwiGetFirmwareCurr",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetFirmwareCurr, "unpack_dms_SLQSSwiGetFirmwareCurr",
        &cur_fw, dump_SLQSSwiGetFirmwareCurr
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapability, "unpack_dms_SLQSGetBandCapability",
        &dmsSLQSGetBandCapability, dump_SLQSGetBandCapability
    },
//22 WP76xx depreciated, move to UIM
    {
        (pack_func_item) &pack_dms_UIMGetICCID, "pack_dms_UIMGetICCID",
        NULL, 
        (unpack_func_item) &unpack_dms_UIMGetICCID, "unpack_dms_UIMGetICCID",
        &UIMGetICCIDResp, dump_UIMGetICCID
    },
    {
        (pack_func_item) &pack_fms_GetImagesPreference, "pack_fms_GetImagesPreference",
        NULL, 
        (unpack_func_item) &unpack_fms_GetImagesPreference, "unpack_fms_GetImagesPreference",
        &getimagesPreference, dump_FMS_GetImagesPreference
    },
    {
        (pack_func_item) &pack_fms_GetStoredImages, "pack_fms_GetStoredImages",
        NULL, 
        (unpack_func_item) &unpack_fms_GetStoredImages, "unpack_fms_GetStoredImages",
        &getStoredImages, dump_FMS_GetStoredImages
    },//setImagePreference input param is same list fetched from getimagesPreference information.
    {
        (pack_func_item) &pack_fms_SetImagesPreference, "pack_fms_SetImagesPreference",
        &packSetImagesPreference, 
        (unpack_func_item) &unpack_fms_SetImagesPreference, "unpack_fms_SetImagesPreference",
        &setImagePreference, dump_FMS_SetImagesPreference
    },

    //// UIM select test start ////
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        &tunpack_dms_SLQSDmsSwiGetUimSelection, dump_SLQSDmsSwiGetUimSelection
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiUimSelect Embedded UIM",
        &SwiUIMSelect[3],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiUimSelect Embedded UIM",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        &tunpack_dms_SLQSDmsSwiGetUimSelection, dump_SLQSDmsSwiGetUimSelection
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport External UIM",
        &SwiUIMSelect[1],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport External UIM",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        &tunpack_dms_SLQSDmsSwiGetUimSelection, dump_SLQSDmsSwiGetUimSelection
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiUimSelect External UIM",
        &SwiUIMSelect[0],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiUimSelect External UIM",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        &tunpack_dms_SLQSDmsSwiGetUimSelection, dump_SLQSDmsSwiGetUimSelection
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[0], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
};



testitem_t dmstotest[] = {
    /////Get modem settings////////////////////////////////
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp Get Settings",
        &GetUSBComp, dump_GetUSBCompSettings
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower Get Settings",
        &power, dump_GetPowerSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Get Settings",
        &GetCusFeatureResp, dump_GetCustFeatureSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2 Get Settings",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2 Get Settings",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2Settings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg Get Settings",
        &unpackModemdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfgSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetHostDevInfo, "pack_dms_SLQSSwiGetHostDevInfo Get Settings",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetHostDevInfo, "unpack_dms_SLQSSwiGetHostDevInfo Get Settings",
        &unpackdmsSLQSSwiGetHostDevInfo, dump_SLQSSwiGetHostDevInfoSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetOSInfo, "pack_dms_SLQSSwiGetOSInfo Get Settings",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetOSInfo, "unpack_dms_SLQSSwiGetOSInfo Get Settings",
        &unpackdmsSLQSSwiGetOSInfo, dump_SLQSSwiGetOSInfoSettings
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction Get Settings",
        &GetCrashAction, dump_GetCrashActionSettings
    },
    ////////////////////////////////////////////////////////
    {
        (pack_func_item) &pack_dms_SLQSSwiGetFirmwareCurr, "pack_dms_SLQSSwiGetFirmwareCurr",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetFirmwareCurr, "unpack_dms_SLQSSwiGetFirmwareCurr",
        &cur_fw, dump_SLQSSwiGetFirmwareCurr
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[0], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[1], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[0], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        &SetEventReportResp, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_GetModelID, "pack_dms_GetModelID",
        NULL, 
        (unpack_func_item) &unpack_dms_GetModelID, "unpack_dms_GetModelID",
        &model, dump_GetModelId
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareInfo, "pack_dms_GetFirmwareInfo",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareInfo, "unpack_dms_GetFirmwareInfo",
        &fwinfo, dump_GetFirmwareInfo
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower",
        &power, dump_GetPower
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareRevisions, "pack_dms_GetFirmwareRevisions",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareRevisions, "unpack_dms_GetFirmwareRevisions",
        &FirmwareRevisions, dump_FirmwareRevisions
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareRevision, "pack_dms_GetFirmwareRevision",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareRevision, "unpack_dms_GetFirmwareRevision",
        &FirmwareRevision, dump_FirmwareRevision
    },

    {
        (pack_func_item) &pack_dms_GetDeviceSerialNumbers, "pack_dms_GetDeviceSerialNumbers",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceSerialNumbers, "unpack_dms_GetDeviceSerialNumbers",
        &DeviceSerialNumbers, dump_GetDeviceSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_GetPRLVersion, "pack_dms_GetPRLVersion",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPRLVersion, "unpack_dms_GetPRLVersion",
        &PRLVersion, dump_GetPRLVersion
    },
    {
        (pack_func_item) &pack_dms_GetNetworkTime, "pack_dms_GetNetworkTime",
        NULL, 
        (unpack_func_item) &unpack_dms_GetNetworkTime, "unpack_dms_GetNetworkTime",
        &Time, dump_GetNetworkTime
    },
    {
        (pack_func_item) &pack_dms_GetNetworkTimeV2, "pack_dms_GetNetworkTimeV2",
        NULL,
        (unpack_func_item) &unpack_dms_GetNetworkTimeV2, "unpack_dms_GetNetworkTimeV2",
        &TimeV2, dump_GetNetworkTimeV2
    },
    {
        (pack_func_item) &pack_dms_GetVoiceNumber, "pack_dms_GetVoiceNumber",
        NULL, 
        (unpack_func_item) &unpack_dms_GetVoiceNumber, "unpack_dms_GetVoiceNumber",
        &VoiceNumber, dump_GetVoiceNumber
    },
    {
        (pack_func_item) &pack_dms_GetDeviceHardwareRev, "pack_dms_GetDeviceHardwareRev",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceHardwareRev, "unpack_dms_GetDeviceHardwareRev",
        &HardwareRev, dump_GetDeviceHardwareRev
    },

    {
        (pack_func_item) &pack_dms_GetFSN, "pack_dms_GetFSN",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFSN, "unpack_dms_GetFSN",
        &FSN, dump_GetFSN
    },

    {
        (pack_func_item) &pack_dms_GetDeviceCap, "pack_dms_GetDeviceCap",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceCap, "unpack_dms_GetDeviceCap",
        &DeviceCap, dump_GetDeviceCap
    },
    {
        (pack_func_item) &pack_dms_GetDeviceCapabilitiesV2, "pack_dms_GetDeviceCapabilitiesV2",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceCapabilitiesV2, "unpack_dms_GetDeviceCapabilitiesV2",
        &tunpack_dms_GetDeviceCapabilitiesV2, dump_GetDeviceCapabilitiesV2
    },
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower",
        &SetPowerOnline,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower",
        &SetPowerResp, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_GetBandCapability, "pack_dms_GetBandCapability",
        NULL, 
        (unpack_func_item) &unpack_dms_GetBandCapability, "unpack_dms_GetBandCapability",
        &BandCapability, dump_GetBandCapability
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp",
        &GetUSBComp, dump_GetUSBComp
    },
    {//MC/EM7455 might fail Version < SWI9X30C_02.05.07.00
        (pack_func_item) &pack_dms_SetUSBComp, "pack_dms_SetUSBComp",
        NULL, 
        (unpack_func_item) &unpack_dms_SetUSBComp, "unpack_dms_SetUSBComp",
        &SetUSBComp, dump_SetUSBComp
    },
    {//MC/EM7455 might fail Version < SWI9X30C_02.05.07.00
        (pack_func_item) &pack_dms_SetUSBComp, "pack_dms_SetUSBComp",
        (void*)&SetUSBCompReq, 
        (unpack_func_item) &unpack_dms_SetUSBComp, "unpack_dms_SetUSBComp",
        &SetUSBComp, dump_SetUSBComp
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[0], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[1], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[2], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[3], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[4], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[5], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[6], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[7], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&SetCusFeature[8], 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature",
        &GetCusFeatureResp, dump_GetCustFeature
    },
    #if 0 // Cause modem reset
    {
        (pack_func_item) &pack_dms_SetFirmwarePreference, "pack_dms_SetFirmwarePreference",
        NULL, 
        (unpack_func_item) &unpack_dms_SetFirmwarePreference, "unpack_dms_SetFirmwarePreference",
        &SetFirmwarePreferenceResp, dump_SetFirmwarePreference
    },
    #endif
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction",
        &GetCrashAction, dump_GetCrashAction
    },
    {
        (pack_func_item) &pack_dms_GetDeviceMfr, "pack_dms_GetDeviceMfr",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceMfr, "unpack_dms_GetDeviceMfr",
        &GetDeviceMfr, dump_GetDeviceMfr
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapability, "unpack_dms_SLQSGetBandCapability",
        &dmsSLQSGetBandCapability, dump_SLQSGetBandCapability
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapabilityExt, "unpack_dms_SLQSGetBandCapabilityExt",
        &dmsSLQSGetBandCapabilityExt, dump_SLQSGetBandCapabilityExt
    },
    {
        (pack_func_item) &pack_dms_UIMGetICCID, "pack_dms_UIMGetICCID",
        NULL, 
        (unpack_func_item) &unpack_dms_UIMGetICCID, "unpack_dms_UIMGetICCID",
        &UIMGetICCIDResp, dump_UIMGetICCID
    },
    {
        (pack_func_item) &pack_dms_SetCustFeaturesV2, "pack_dms_SetCustFeaturesV2",
        &SetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_SetCustFeaturesV2, "unpack_dms_SetCustFeaturesV2",
        &SetCustFeaturesV2Resp, dump_SetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2",
        &GetCustFeaturesV2Resp, dump_GetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_fms_GetImagesPreference, "pack_fms_GetImagesPreference",
        NULL, 
        (unpack_func_item) &unpack_fms_GetImagesPreference, "unpack_fms_GetImagesPreference",
        &getimagesPreference, dump_FMS_GetImagesPreference
    },
    {
        (pack_func_item) &pack_fms_GetStoredImages, "pack_fms_GetStoredImages",
        NULL, 
        (unpack_func_item) &unpack_fms_GetStoredImages, "unpack_fms_GetStoredImages",
        &getStoredImages, dump_FMS_GetStoredImages
    },//setImagePreference using getimagesPreference information as input parameter.
    {
        (pack_func_item) &pack_fms_SetImagesPreference, "pack_fms_SetImagesPreference",
        &packSetImagesPreference, 
        (unpack_func_item) &unpack_fms_SetImagesPreference, "unpack_fms_SetImagesPreference",
        &setImagePreference, dump_FMS_SetImagesPreference
    },
    {
        (pack_func_item) &pack_dms_GetActivationState, "pack_dms_GetActivationState",
        NULL, 
        (unpack_func_item) &unpack_dms_GetActivationState, "unpack_dms_GetActivationState",
        &ActivationStateResp, dump_GetActivationState
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg",
        &unpackModemdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg",
        &packdmsSLQSSwiSetDyingGaspCfg, 
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiClearDyingGaspStatistics, "pack_dms_SLQSSwiClearDyingGaspStatistics",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiClearDyingGaspStatistics, "unpack_dms_SLQSSwiClearDyingGaspStatistics",
        &unpackdmsSLQSSwiClearDyingGaspStatistics, dump_SLQSSwiClearDyingGaspStatistics
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspStatistics, "pack_dms_SLQSSwiGetDyingGaspStatistics",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspStatistics, "unpack_dms_SLQSSwiGetDyingGaspStatistics",
        &unpackdmsSLQSSwiGetDyingGaspStatistics, dump_SLQSSwiGetDyingGaspStatistics
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg",
        &packModemdmsSLQSSwiSetDyingGaspCfg, 
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg",
        &unpackdmsSLQSSwiGetDyingGaspCfg, dump_SLQSSwiGetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetResetInfo, "pack_dms_SLQSDmsSwiGetResetInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetResetInfo, "unpack_dms_SLQSDmsSwiGetResetInfo",
        &unpackdmsSLQSDmsSwiGetResetInfo, dump_SLQSDmsSwiGetResetInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetFwUpdateStatus, "pack_dms_SLQSSwiGetFwUpdateStatus",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetFwUpdateStatus, "unpack_dms_SLQSSwiGetFwUpdateStatus",
        &unpackdmsSLQSSwiGetFwUpdateStatus, dump_SLQSSwiGetFwUpdateStatus
    },
    {
        (pack_func_item) &pack_dms_GetManufacturer, "pack_dms_GetManufacturer",
        NULL,
        (unpack_func_item) &unpack_dms_GetManufacturer, "unpack_dms_GetManufacturer",
        &unpackdmsGetManufacturer, dump_GetManufacturer
    },
    {
        (pack_func_item) &pack_dms_GetOfflineReason, "pack_dms_GetOfflineReason",
        NULL,
        (unpack_func_item) &unpack_dms_GetOfflineReason, "unpack_dms_GetOfflineReason",
        &unpackdmsGetOfflineReason, dump_GetOfflineReason
    },
    {
        (pack_func_item) &pack_dms_UIMSetPINProtection, "pack_dms_UIMSetPINProtection",
        &packdmsUIMSetPINProtection,
        (unpack_func_item) &unpack_dms_UIMSetPINProtection, "unpack_dms_UIMSetPINProtection",
        &unpackdmsUIMSetPINProtection, dump_UIMSetPINProtection
    },
    {
        (pack_func_item) &pack_dms_UIMUnblockPIN, "pack_dms_UIMUnblockPIN",
        &packdmsUIMUnblockPIN,
        (unpack_func_item) &unpack_dms_UIMUnblockPIN, "unpack_dms_UIMUnblockPIN",
        &unpackdmsUIMUnblockPIN, dump_UIMUnblockPIN
    },
    {
        (pack_func_item) &pack_dms_UIMVerifyPIN, "pack_dms_UIMVerifyPIN",
        &packdmsUIMVerifyPIN,
        (unpack_func_item) &unpack_dms_UIMVerifyPIN, "unpack_dms_UIMVerifyPIN",
        &unpackdmsUIMVerifyPIN, dump_UIMVerifyPIN
    },
    {
        (pack_func_item) &pack_dms_UIMChangePIN, "pack_dms_UIMChangePIN",
        &packdmsUIMChangePIN,
        (unpack_func_item) &unpack_dms_UIMChangePIN, "unpack_dms_UIMChangePIN",
        &unpackdmsUIMChangePIN, dump_UIMChangePIN
    },
    {
        (pack_func_item) &pack_dms_UIMGetControlKeyStatus, "pack_dms_UIMGetControlKeyStatus",
        &packdmsUIMGetControlKeyStatus,
        (unpack_func_item) &unpack_dms_UIMGetControlKeyStatus, "unpack_dms_UIMGetControlKeyStatus",
        &unpackdmsUIMGetControlKeyStatus, dump_UIMGetControlKeyStatus
    },
    {
        (pack_func_item) &pack_dms_UIMGetPINStatus, "pack_dms_UIMGetPINStatus",
        NULL,
        (unpack_func_item) &unpack_dms_UIMGetPINStatus, "unpack_dms_UIMGetPINStatus",
        &unpackdmsUIMGetPINStatus, dump_UIMGetPINStatus
    },
    {
        (pack_func_item) &pack_dms_UIMSetControlKeyProtection, "pack_dms_UIMSetControlKeyProtection",
        &packdmsUIMSetControlKeyProtection,
        (unpack_func_item) &unpack_dms_UIMSetControlKeyProtection, "unpack_dms_UIMSetControlKeyProtection",
        &unpackdmsUIMSetControlKeyProtection, dump_UIMSetControlKeyProtection
    },
    {
        (pack_func_item) &pack_dms_UIMUnblockControlKey, "pack_dms_UIMUnblockControlKey",
        &packdmsUIMUnblockControlKey,
        (unpack_func_item) &unpack_dms_UIMUnblockControlKey, "unpack_dms_UIMUnblockControlKey",
        &unpackdmsUIMUnblockControlKey, dump_UIMUnblockControlKey
    },
    {
        (pack_func_item) &pack_dms_ResetToFactoryDefaults, "pack_dms_ResetToFactoryDefaults",
        &packdmsResetToFactoryDefaults,
        (unpack_func_item) &unpack_dms_ResetToFactoryDefaults, "unpack_dms_ResetToFactoryDefaults",
        &unpackdmsResetToFactoryDefaults, dump_ResetToFactoryDefaults
    },
    {
        (pack_func_item) &pack_dms_ValidateSPC, "pack_dms_ValidateSPC",
        &packdmsResetToFactoryDefaults,
        (unpack_func_item) &unpack_dms_ValidateSPC, "unpack_dms_ValidateSPC",
        &unpackdmsResetToFactoryDefaults, dump_ValidateSPC
    },
    {
        (pack_func_item) &pack_dms_ActivateAutomatic, "pack_dms_ActivateAutomatic",
        &packdmsActivateAutomatic,
        (unpack_func_item) &unpack_dms_ActivateAutomatic, "unpack_dms_ActivateAutomatic",
        &unpackdmsResetToFactoryDefaults, dump_ActivateAutomatic
    },
    {
        (pack_func_item) &pack_dms_SLQSGetERIFile, "pack_dms_SLQSGetERIFile",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetERIFile, "unpack_dms_SLQSGetERIFile",
        &unpackdmsSLQSGetERIFile, dump_SLQSGetERIFile
    },
    {
        (pack_func_item) &pack_dms_SLQSUIMGetState, "pack_dms_SLQSUIMGetState",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSUIMGetState, "unpack_dms_SLQSUIMGetState",
        &unpackdmsSLQSUIMGetState, dump_SLQSUIMGetState
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetCrashInfo, "pack_dms_SLQSSwiGetCrashInfo",
        &packdmsSLQSSwiGetCrashInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiGetCrashInfo, "unpack_dms_SLQSSwiGetCrashInfo",
        &unpackdmsSLQSSwiGetCrashInfo, dump_SLQSSwiGetCrashInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetHostDevInfo, "pack_dms_SLQSSwiSetHostDevInfo",
        &packdmsSLQSSwiSetHostDevInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiSetHostDevInfo, "unpack_dms_SLQSSwiSetHostDevInfo",
        &unpackdmsSLQSSwiSetHostDevInfo, dump_SLQSSwiSetHostDevInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetHostDevInfo, "pack_dms_SLQSSwiGetHostDevInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetHostDevInfo, "unpack_dms_SLQSSwiGetHostDevInfo",
        &unpackdmsSLQSSwiGetHostDevInfo, dump_SLQSSwiGetHostDevInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetOSInfo, "pack_dms_SLQSSwiSetOSInfo",
        &packdmsSLQSSwiSetOSInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiSetOSInfo, "unpack_dms_SLQSSwiSetOSInfo",
        NULL, dump_SLQSSwiSetOSInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetOSInfo, "pack_dms_SLQSSwiGetOSInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetOSInfo, "unpack_dms_SLQSSwiGetOSInfo",
        &unpackdmsSLQSSwiGetOSInfo, dump_SLQSSwiGetOSInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetSerialNoExt, "pack_dms_SLQSSwiGetOSInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetSerialNoExt, "unpack_dms_SLQSSwiGetSerialNoExt",
        &unpackdmsSLQSSwiGetSerialNoExt, dump_SLQSSwiGetSerialNoExt
    },
    {
        (pack_func_item) &pack_dms_GetIMSI, "pack_dms_GetIMSI",
        NULL,
        (unpack_func_item) &unpack_dms_GetIMSI, "unpack_dms_GetIMSI",
        &imsi, dump_GetIMSI
    },
    {
        (pack_func_item) &pack_dms_GetSerialNumbers, "pack_dms_GetSerialNumbers",
        NULL,
        (unpack_func_item) &unpack_dms_GetSerialNumbers, "unpack_dms_GetSerialNumbers",
        &tunpack_dms_GetSerialNumbers, dump_GetSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_GetHardwareRevision, "pack_dms_GetHardwareRevision",
        NULL,
        (unpack_func_item) &unpack_dms_GetHardwareRevision, "unpack_dms_GetHardwareRevision",
        &tunpack_dms_GetHardwareRevision, dump_GetHardwareRevision
    },
    {
        (pack_func_item) &pack_dms_GetDeviceCapabilities, "pack_dms_GetDeviceCapabilities",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceCapabilities, "unpack_dms_GetDeviceCapabilities",
        &tunpack_dms_GetDeviceCapabilities, dump_GetDeviceCapabilities
    },
    {
        (pack_func_item) &pack_dms_SetCrashAction, "pack_dms_SetCrashAction",
        &tpack_dms_SetCrashAction,
        (unpack_func_item) &unpack_dms_SetCrashAction, "unpack_dms_SetCrashAction",
        &tunpack_dms_SetCrashAction, dump_SetCrashAction
    },
    {
        (pack_func_item) &pack_dms_SetActivationStatusCallback, "pack_dms_SetActivationStatusCallback",
        &tpack_dms_SetActivationStatusCallback,
        (unpack_func_item) &unpack_dms_SetActivationStatusCallback, "unpack_dms_SetActivationStatusCallback",
        &tunpack_dms_SetActivationStatusCallback, dump_SetActivationStatusCallback
    },
    {
        (pack_func_item) &pack_dms_SwiSetEventReport, "pack_dms_SwiSetEventReport",
        &SwiEventSettings,
        (unpack_func_item) &unpack_dms_SwiSetEventReport, "unpack_dms_SwiSetEventReport",
        &SwiSetEventResp, dump_SwiSetEventReport
    },
    //// UIM select test start ////
    {
        (pack_func_item) &pack_dms_SwiSetEventReport, "pack_dms_SwiSetEventReport",
        &SwiEventSettingsUINStatus,
        (unpack_func_item) &unpack_dms_SwiSetEventReport, "unpack_dms_SwiSetEventReport",
        &SwiSetEventResp, dump_SwiSetEventReport
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        &tunpack_dms_SLQSDmsSwiGetUimSelection, dump_SLQSDmsSwiGetUimSelection
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport External UIM Interface",
        &SwiUIMSelect[0],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport External UIM Interface",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport Embedded UIM",
        &SwiUIMSelect[1],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport Embedded UIM",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport Remote UIM",
        &SwiUIMSelect[2],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport Remote UIM",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport Auto switch",
        &SwiUIMSelect[3],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport Auto switch",
        NULL, dump_SwiUimSelect
    },
    
    //// UIM select test end ////
    ////Restore Modem Settings  ////
    {
        (pack_func_item) &restore_pack_dms_SetUSBComp, "pack_dms_SetUSBComp",
        (void*)&DefaultUSBCompSettings, 
        (unpack_func_item) &unpack_dms_SetUSBComp, "unpack_dms_SetUSBComp",
        &SetUSBComp, dump_SetUSBComp
    },
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower",
        &DefaultPowerStatus,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower",
        &SetPowerResp, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_SetCustFeature, "pack_dms_SetCustFeature",
        (void*)&DefaultCusFeatureSettings, 
        (unpack_func_item) &unpack_dms_SetCustFeature, "unpack_dms_SetCustFeature",
        &SetCusFeatureResp, dump_SetCustFeature
    },
    {
        (pack_func_item) &pack_dms_SetCustFeaturesV2, "pack_dms_SetCustFeaturesV2",
        &DefaultCusFeaturesV2Settings,
        (unpack_func_item) &unpack_dms_SetCustFeaturesV2, "unpack_dms_SetCustFeaturesV2",
        &SetCustFeaturesV2Resp, dump_SetCustFeaturesV2
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetDyingGaspCfg, "pack_dms_SLQSSwiSetDyingGaspCfg",
        &DefaultSwiSetDyingGaspCfg, 
        (unpack_func_item) &unpack_dms_SLQSSwiSetDyingGaspCfg, "unpack_dms_SLQSSwiSetDyingGaspCfg",
        &unpackdmsSLQSSwiSetDyingGaspCfg, dump_SLQSSwiSetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetHostDevInfo, "pack_dms_SLQSSwiSetHostDevInfo",
        &DefaultSLQSSwiSetHostDevInfoSettings,
        (unpack_func_item) &unpack_dms_SLQSSwiSetHostDevInfo, "unpack_dms_SLQSSwiSetHostDevInfo",
        &unpackdmsSLQSSwiSetHostDevInfo, dump_SLQSSwiSetHostDevInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetOSInfo, "pack_dms_SLQSSwiSetOSInfo",
        &DefaultSwiSetOSInfoSettings,
        (unpack_func_item) &unpack_dms_SLQSSwiSetOSInfo, "unpack_dms_SLQSSwiSetOSInfo",
        NULL, dump_SLQSSwiSetOSInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiSetOSInfo, "pack_dms_SLQSSwiSetOSInfo",
        &DefaultCrashActionSettings,
        (unpack_func_item) &unpack_dms_SLQSSwiSetOSInfo, "unpack_dms_SLQSSwiSetOSInfo",
        NULL, dump_SLQSSwiSetOSInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetPCInfo, "pack_dms_SLQSDmsSwiGetPCInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetPCInfo, "unpack_dms_SLQSDmsSwiGetPCInfo",
        &tunpack_dms_SLQSDmsSwiGetPCInfo, dump_SLQSDmsSwiGetPCInfo
    },
    //////////////////////////////////////////////////
    {
        (pack_func_item) &pack_dms_SLQSGetPowerSaveModeConfig, "pack_dms_SLQSGetPowerSaveModeConfig",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetPowerSaveModeConfig, "unpack_dms_SLQSGetPowerSaveModeConfig",
        &tunpack_dms_SLQSGetPowerSaveModeConfig, dump_SLQSGetPowerSaveModeConfig
    },
    {
        (pack_func_item) &pack_dms_SLQSSetPowerSaveModeConfig, "pack_dms_SLQSSetPowerSaveModeConfig",
        &tpack_dms_SLQSSetPowerSaveModeConfig,
        (unpack_func_item) &unpack_dms_SLQSSetPowerSaveModeConfig, "unpack_dms_SLQSSetPowerSaveModeConfig",
        NULL, dump_SLQSSetPowerSaveModeConfig
    },
    // Switch back to external UIM interface
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport External UIM Interface",
        &SwiUIMSelect[0],
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport External UIM Interface",
        NULL, dump_SwiUimSelect
    },
};

unsigned int dmsarraylen = (unsigned int)((sizeof(dmstotest))/(sizeof(dmstotest[0])));

testitem_t dmstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp Get Settings",
        NULL, dump_GetUSBCompSettings
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower Get Settings",
        NULL, dump_GetPowerSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature Get Settings",
        NULL, dump_GetCustFeatureSettings
    },
    {
        (pack_func_item) &pack_dms_GetCustFeaturesV2, "pack_dms_GetCustFeaturesV2 Get Settings",
        &GetCusFeaturesV2Req,
        (unpack_func_item) &unpack_dms_GetCustFeaturesV2, "unpack_dms_GetCustFeaturesV2 Get Settings",
        NULL, dump_GetCustFeaturesV2Settings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg Get Settings",
        NULL, dump_SLQSSwiGetDyingGaspCfgSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetHostDevInfo, "pack_dms_SLQSSwiGetHostDevInfo Get Settings",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetHostDevInfo, "unpack_dms_SLQSSwiGetHostDevInfo Get Settings",
        NULL, dump_SLQSSwiGetHostDevInfoSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetOSInfo, "pack_dms_SLQSSwiGetOSInfo Get Settings",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetOSInfo, "unpack_dms_SLQSSwiGetOSInfo Get Settings",
        NULL, dump_SLQSSwiGetOSInfoSettings
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction Get Settings",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction Get Settings",
        NULL, dump_GetCrashActionSettings
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetFirmwareCurr, "pack_dms_SLQSSwiGetFirmwareCurr",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetFirmwareCurr, "unpack_dms_SLQSSwiGetFirmwareCurr",
        NULL, dump_SLQSSwiGetFirmwareCurr
    },
    {
        (pack_func_item) &pack_dms_SetEventReport, "pack_dms_SetEventReport",
        &SetEventReport[0], 
        (unpack_func_item) &unpack_dms_SetEventReport, "unpack_dms_SetEventReport",
        NULL, dump_SetEventReport
    },
    {
        (pack_func_item) &pack_dms_GetModelID, "pack_dms_GetModelID",
        NULL, 
        (unpack_func_item) &unpack_dms_GetModelID, "unpack_dms_GetModelID",
        NULL, dump_GetModelId
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareInfo, "pack_dms_GetFirmwareInfo",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareInfo, "unpack_dms_GetFirmwareInfo",
        NULL, dump_GetFirmwareInfo
    },
    {
        (pack_func_item) &pack_dms_GetPower, "pack_dms_GetPower",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPower, "unpack_dms_GetPower",
        NULL, dump_GetPower
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareRevisions, "pack_dms_GetFirmwareRevisions",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareRevisions, "unpack_dms_GetFirmwareRevisions",
        NULL, dump_FirmwareRevisions
    },
    {
        (pack_func_item) &pack_dms_GetFirmwareRevision, "pack_dms_GetFirmwareRevision",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFirmwareRevision, "unpack_dms_GetFirmwareRevision",
        NULL, dump_FirmwareRevision
    },

    {
        (pack_func_item) &pack_dms_GetDeviceSerialNumbers, "pack_dms_GetDeviceSerialNumbers",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceSerialNumbers, "unpack_dms_GetDeviceSerialNumbers",
        NULL, dump_GetDeviceSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_GetPRLVersion, "pack_dms_GetPRLVersion",
        NULL, 
        (unpack_func_item) &unpack_dms_GetPRLVersion, "unpack_dms_GetPRLVersion",
        NULL, dump_GetPRLVersion
    },
    {
        (pack_func_item) &pack_dms_GetNetworkTime, "pack_dms_GetNetworkTime",
        NULL, 
        (unpack_func_item) &unpack_dms_GetNetworkTime, "unpack_dms_GetNetworkTime",
        NULL, dump_GetNetworkTime
    },
    {
        (pack_func_item) &pack_dms_GetVoiceNumber, "pack_dms_GetVoiceNumber",
        NULL, 
        (unpack_func_item) &unpack_dms_GetVoiceNumber, "unpack_dms_GetVoiceNumber",
        NULL, dump_GetVoiceNumber
    },
    {
        (pack_func_item) &pack_dms_GetDeviceHardwareRev, "pack_dms_GetDeviceHardwareRev",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceHardwareRev, "unpack_dms_GetDeviceHardwareRev",
        NULL, dump_GetDeviceHardwareRev
    },

    {
        (pack_func_item) &pack_dms_GetFSN, "pack_dms_GetFSN",
        NULL, 
        (unpack_func_item) &unpack_dms_GetFSN, "unpack_dms_GetFSN",
        NULL, dump_GetFSN
    },

    {
        (pack_func_item) &pack_dms_GetDeviceCap, "pack_dms_GetDeviceCap",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceCap, "unpack_dms_GetDeviceCap",
        NULL, dump_GetDeviceCap
    },
    {
        (pack_func_item) &pack_dms_SetPower,  "pack_dms_SetPower",
        &SetPowerOnline,
        (unpack_func_item) &unpack_dms_SetPower, "unpack_dms_SetPower",
        NULL, dump_SetPower
    },
    {
        (pack_func_item) &pack_dms_GetBandCapability, "pack_dms_GetBandCapability",
        NULL, 
        (unpack_func_item) &unpack_dms_GetBandCapability, "unpack_dms_GetBandCapability",
        NULL, dump_GetBandCapability
    },
    {
        (pack_func_item) &pack_dms_GetUSBComp, "pack_dms_GetUSBComp",
        NULL, 
        (unpack_func_item) &unpack_dms_GetUSBComp, "unpack_dms_GetUSBComp",
        NULL, dump_GetUSBComp
    },
    {
        (pack_func_item) &pack_dms_GetCustFeature, "pack_dms_GetCustFeature",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCustFeature, "unpack_dms_GetCustFeature",
        NULL, dump_GetCustFeature
    },
    {
        (pack_func_item) &pack_dms_GetCrashAction, "pack_dms_GetCrashAction",
        NULL, 
        (unpack_func_item) &unpack_dms_GetCrashAction, "unpack_dms_GetCrashAction",
        NULL, dump_GetCrashAction
    },
    {
        (pack_func_item) &pack_dms_GetDeviceMfr, "pack_dms_GetDeviceMfr",
        NULL, 
        (unpack_func_item) &unpack_dms_GetDeviceMfr, "unpack_dms_GetDeviceMfr",
        NULL, dump_GetDeviceMfr
    },
    {
        (pack_func_item) &pack_dms_SLQSGetBandCapability, "pack_dms_SLQSGetBandCapability",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetBandCapability, "unpack_dms_SLQSGetBandCapability",
        NULL, dump_SLQSGetBandCapability
    },
    {
        (pack_func_item) &pack_dms_UIMGetICCID, "pack_dms_UIMGetICCID",
        NULL, 
        (unpack_func_item) &unpack_dms_UIMGetICCID, "unpack_dms_UIMGetICCID",
        NULL, dump_UIMGetICCID
    },
    {
        (pack_func_item) &pack_fms_GetImagesPreference, "pack_fms_GetImagesPreference",
        NULL, 
        (unpack_func_item) &unpack_fms_GetImagesPreference, "unpack_fms_GetImagesPreference",
        NULL, dump_FMS_GetImagesPreference
    },
    {
        (pack_func_item) &pack_fms_GetStoredImages, "pack_fms_GetStoredImages",
        NULL, 
        (unpack_func_item) &unpack_fms_GetStoredImages, "unpack_fms_GetStoredImages",
        NULL, dump_FMS_GetStoredImages
    },
        {
        (pack_func_item) &pack_dms_GetActivationState, "pack_dms_GetActivationState",
        NULL, 
        (unpack_func_item) &unpack_dms_GetActivationState, "unpack_dms_GetActivationState",
        NULL, dump_GetActivationState
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetDyingGaspCfg, "pack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, 
        (unpack_func_item) &unpack_dms_SLQSSwiGetDyingGaspCfg, "unpack_dms_SLQSSwiGetDyingGaspCfg",
        NULL, dump_SLQSSwiGetDyingGaspCfg
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetResetInfo, "pack_dms_SLQSDmsSwiGetResetInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetResetInfo, "unpack_dms_SLQSDmsSwiGetResetInfo",
        NULL, dump_SLQSDmsSwiGetResetInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetFwUpdateStatus, "pack_dms_SLQSSwiGetFwUpdateStatus",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetFwUpdateStatus, "unpack_dms_SLQSSwiGetFwUpdateStatus",
        NULL, dump_SLQSSwiGetFwUpdateStatus
    },
    {
        (pack_func_item) &pack_dms_GetManufacturer, "pack_dms_GetManufacturer",
        NULL,
        (unpack_func_item) &unpack_dms_GetManufacturer, "unpack_dms_GetManufacturer",
        NULL, dump_GetManufacturer
    },
    {
        (pack_func_item) &pack_dms_GetOfflineReason, "pack_dms_GetOfflineReason",
        NULL,
        (unpack_func_item) &unpack_dms_GetOfflineReason, "unpack_dms_GetOfflineReason",
        NULL, dump_GetOfflineReason
    },
    {
        (pack_func_item) &pack_dms_UIMGetControlKeyStatus, "pack_dms_UIMGetControlKeyStatus",
        &packdmsUIMGetControlKeyStatus,
        (unpack_func_item) &unpack_dms_UIMGetControlKeyStatus, "unpack_dms_UIMGetControlKeyStatus",
        NULL, dump_UIMGetControlKeyStatus
    },
    {
        (pack_func_item) &pack_dms_UIMGetPINStatus, "pack_dms_UIMGetPINStatus",
        NULL,
        (unpack_func_item) &unpack_dms_UIMGetPINStatus, "unpack_dms_UIMGetPINStatus",
        NULL, dump_UIMGetPINStatus
    },
    {
        (pack_func_item) &pack_dms_SLQSGetERIFile, "pack_dms_SLQSGetERIFile",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetERIFile, "unpack_dms_SLQSGetERIFile",
        NULL, dump_SLQSGetERIFile
    },
    {
        (pack_func_item) &pack_dms_SLQSUIMGetState, "pack_dms_SLQSUIMGetState",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSUIMGetState, "unpack_dms_SLQSUIMGetState",
        NULL, dump_SLQSUIMGetState
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetCrashInfo, "pack_dms_SLQSSwiGetCrashInfo",
        &packdmsSLQSSwiGetCrashInfo,
        (unpack_func_item) &unpack_dms_SLQSSwiGetCrashInfo, "unpack_dms_SLQSSwiGetCrashInfo",
        NULL, dump_SLQSSwiGetCrashInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetHostDevInfo, "pack_dms_SLQSSwiGetHostDevInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetHostDevInfo, "unpack_dms_SLQSSwiGetHostDevInfo",
        NULL, dump_SLQSSwiGetHostDevInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetOSInfo, "pack_dms_SLQSSwiGetOSInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetOSInfo, "unpack_dms_SLQSSwiGetOSInfo",
        NULL, dump_SLQSSwiGetOSInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSSwiGetSerialNoExt, "pack_dms_SLQSSwiGetOSInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSwiGetSerialNoExt, "unpack_dms_SLQSSwiGetSerialNoExt",
        NULL, dump_SLQSSwiGetSerialNoExt
    },
    {
        (pack_func_item) &pack_dms_GetIMSI, "pack_dms_GetIMSI",
        NULL,
        (unpack_func_item) &unpack_dms_GetIMSI, "unpack_dms_GetIMSI",
        NULL, dump_GetIMSI
    },
    {
        (pack_func_item) &pack_dms_GetSerialNumbers, "pack_dms_GetSerialNumbers",
        NULL,
        (unpack_func_item) &unpack_dms_GetSerialNumbers, "unpack_dms_GetSerialNumbers",
        NULL, dump_GetSerialNumbers
    },
    {
        (pack_func_item) &pack_dms_GetHardwareRevision, "pack_dms_GetHardwareRevision",
        NULL,
        (unpack_func_item) &unpack_dms_GetHardwareRevision, "unpack_dms_GetHardwareRevision",
        NULL, dump_GetHardwareRevision
    },
    {
        (pack_func_item) &pack_dms_GetDeviceCapabilities, "pack_dms_GetDeviceCapabilities",
        NULL,
        (unpack_func_item) &unpack_dms_GetDeviceCapabilities, "unpack_dms_GetDeviceCapabilities",
        NULL, dump_GetDeviceCapabilities
    },
    {
        (pack_func_item) &pack_dms_SetActivationStatusCallback, "pack_dms_SetActivationStatusCallback",
        &tpack_dms_SetActivationStatusCallback,
        (unpack_func_item) &unpack_dms_SetActivationStatusCallback, "unpack_dms_SetActivationStatusCallback",
        NULL, dump_SetActivationStatusCallback
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetPCInfo, "pack_dms_SLQSDmsSwiGetPCInfo",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetPCInfo, "unpack_dms_SLQSDmsSwiGetPCInfo",
        NULL, dump_SLQSDmsSwiGetPCInfo
    },
    {
        (pack_func_item) &pack_dms_SLQSGetPowerSaveModeConfig, "pack_dms_SLQSGetPowerSaveModeConfig",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSGetPowerSaveModeConfig, "unpack_dms_SLQSGetPowerSaveModeConfig",
        NULL, dump_SLQSGetPowerSaveModeConfig
    },
    {
        (pack_func_item) &pack_dms_SLQSSetPowerSaveModeConfig, "pack_dms_SLQSSetPowerSaveModeConfig",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSSetPowerSaveModeConfig, "unpack_dms_SLQSSetPowerSaveModeConfig",
        NULL, dump_SLQSSetPowerSaveModeConfig
    },
    {
        (pack_func_item) &pack_dms_SwiSetEventReport, "pack_dms_SwiSetEventReport",
        NULL,
        (unpack_func_item) &unpack_dms_SwiSetEventReport, "unpack_dms_SwiSetEventReport",
        NULL, dump_SwiSetEventReport
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport",
        NULL,
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport",
        &SwiUIMSelectRsp, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SwiUimSelect, "pack_dms_SwiSetEventReport",
        NULL,
        (unpack_func_item) &unpack_dms_SwiUimSelect, "unpack_dms_SwiSetEventReport",
        NULL, dump_SwiUimSelect
    },
    {
        (pack_func_item) &pack_dms_SLQSDmsSwiGetUimSelection, "pack_dms_SLQSDmsSwiGetUimSelection",
        NULL,
        (unpack_func_item) &unpack_dms_SLQSDmsSwiGetUimSelection, "unpack_dms_SLQSDmsSwiGetUimSelection",
        NULL, dump_SLQSDmsSwiGetUimSelection
    },
};

void dms_setOemApiTestConfig(dmsConfig *dmsConfigList)
{
    if ( dmsConfigList != NULL)
    {
        strcpy((char*)model_id, dmsConfigList->modelId);
        strcpy((char*)fw_version, dmsConfigList->fwVersion);
        strcpy((char*)IMEI, dmsConfigList->imei);
        strcpy((char*)ICCID, dmsConfigList->iccid);
    }
}

void dms_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    unsigned xid =1;
    const char *qmi_msg;
    printf("======DMS pack/unpack test with invalid unpack param===========\n");
    for(i=0; i<sizeof(dmstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(dmstotest_invalidunpack[i].pack)(&req_ctx, req, 
                                             &reqLen,dmstotest_invalidunpack[i].pack_ptr);
        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }
        if(dms<0)
            dms = client_fd(eDMS);
        if(dms<0)
        {
            fprintf(stderr,"DMS Service Not Supported\n");
            return ;
        }
        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(dms>=0)
                close(dms);
            dms=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eDMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen,req);
        }

        rtn = read(dms, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eDMS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(dmstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                dmstotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    fprintf(stderr,"%s: returned %d, unpack failed!\n", 
                           dmstotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    dmstotest_invalidunpack[i].dump(dmstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(dms>=0)
                close(dms);
            dms=-1;
        }
        sleep(1);
        xid++;
    }
    if(dms>=0)
        close(dms);
    dms=-1;
}


void dms_indication_handler(uint16_t msgid, uint8_t *msg, uint32_t rlen)
{
    unpack_dms_SetEventReport_ind_t setEventReportResp;
    memset(&setEventReportResp,0,sizeof(unpack_dms_SetEventReport_ind_t));
    unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t sLQSDmsSwiGetResetInfoResp;
    memset(&sLQSDmsSwiGetResetInfoResp,0,sizeof(unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t));
    unpack_dms_SwiEventReportCallBack_ind_t swiEventReportCallBackResp;
    memset(&swiEventReportCallBackResp,0,sizeof(unpack_dms_SwiEventReportCallBack_ind_t));
    unpack_dms_PSMCfgChange_ind_t pSMCfgChangeResp;
    memset(&pSMCfgChangeResp,0,sizeof(unpack_dms_PSMCfgChange_ind_t));



#if DEBUG_LOG_TO_FILE
    int is_matching = 1;
#endif

    switch(msgid)
    {
        case eQMI_DMS_SET_EVENT:                 
             /* Set all the TLVs to be in Not-Present State */
             setEventReportResp.ActivationStatusTlv.TlvPresent = 0;
             setEventReportResp.OperatingModeTlv.TlvPresent = 0;
         
             /* Unpack the Event Indication */
             unpackRetCode = unpack_dms_SetEventReport_ind( msg,rlen,&setEventReportResp );
             if((setEventReportResp.ActivationStatusTlv.TlvPresent!=0)&& 
                 (swi_uint256_get_bit (setEventReportResp.ParamPresenceMask, 19)))
             {
                 printf("QMI_DMS_SET_EVENT_REPORT IND activationStatus :%d\n",setEventReportResp.ActivationStatusTlv.activationStatus);
             }
             if((setEventReportResp.OperatingModeTlv.TlvPresent!=0) && 
                 (swi_uint256_get_bit (setEventReportResp.ParamPresenceMask, 20)))
             {
                 printf("QMI_DMS_SET_EVENT_REPORT IND operatingMode :%d\n",setEventReportResp.OperatingModeTlv.operatingMode);
             }

         #if DEBUG_LOG_TO_FILE
             CHECK_WHITELIST_MASK(
                 unpack_dms_SetEventReport_indParamPresenceMaskWhiteList,
                 setEventReportResp.ParamPresenceMask);
             if(setEventReportResp.ActivationStatusTlv.TlvPresent!=0)
             {
                 if (setEventReportResp.ActivationStatusTlv.activationStatus > 10 )
                 {
                     is_matching = 0;
                 }
             }
             if(setEventReportResp.OperatingModeTlv.TlvPresent!=0)
             {
                 if (setEventReportResp.OperatingModeTlv.operatingMode > 8 )
                 {
                     is_matching = 0;
                 }
             }
             local_fprintf("%s,%s,", "unpack_dms_SetEventReport_ind",\
                 ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            
             local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
         #endif
            break;

        case eQMI_DMS_SWI_GET_RESET_INFO:        
            /* Unpack the Event Indication */
            unpackRetCode = unpack_dms_SLQSDmsSwiGetResetInfo_Ind( msg,rlen,&sLQSDmsSwiGetResetInfoResp );
            if(swi_uint256_get_bit (sLQSDmsSwiGetResetInfoResp.ParamPresenceMask, 1))
            {
                printf("QMI_DMS_SWI_GET_RESET_INFO_IND:\n");
                printf("Type :  %d\n",sLQSDmsSwiGetResetInfoResp.type);
                printf("Source :  %d\n",sLQSDmsSwiGetResetInfoResp.source);
            }
        #if DEBUG_LOG_TO_FILE
            CHECK_WHITELIST_MASK(
                unpack_dms_SLQSDmsSwiGetResetInfo_IndParamPresenceMaskWhiteList,
                sLQSDmsSwiGetResetInfoResp.ParamPresenceMask);
            CHECK_WHITELIST_MASK(
                unpack_dms_SLQSDmsSwiGetResetInfo_IndParamPresenceMaskMandatoryList,
                sLQSDmsSwiGetResetInfoResp.ParamPresenceMask);
            if ((sLQSDmsSwiGetResetInfoResp.source > 8) && (sLQSDmsSwiGetResetInfoResp.type > 4))
            {
                is_matching = 0;
            }
            local_fprintf("%s,%s,", "unpack_dms_SLQSDmsSwiGetResetInfo_Ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            
            local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
        #endif
            break;

        case eQMI_DMS_SWI_EVENT_IND:        
            /* Unpack the DMS SWI Event Indication */
            unpackRetCode = unpack_dms_SwiEventReportCallBack_ind( msg,rlen,&swiEventReportCallBackResp );
            printf("QMI_DMS_SWI_SET_EVENT_REPORT IND result :%d\n",unpackRetCode);
            if(swiEventReportCallBackResp.TempTlv.TlvPresent!=0)
            {
                printf("QMI_DMS_SWI_SET_EVENT_REPORT IND temp State :%d temperature %d\n",
                        swiEventReportCallBackResp.TempTlv.TempStat,
                        swiEventReportCallBackResp.TempTlv.Temperature);
            }
        
            if(swiEventReportCallBackResp.VoltTlv.TlvPresent!=0)
            {
                printf("QMI_DMS_SWI_SET_EVENT_REPORT IND volt State :%d voltage %d\n",
                        swiEventReportCallBackResp.VoltTlv.VoltStat,
                        swiEventReportCallBackResp.VoltTlv.Voltage);
            }
            if(swiEventReportCallBackResp.UimStatusTlv.TlvPresent!=0)
            {
                printf("QMI_DMS_SWI_SET_EVENT_REPORT IND UIM status interface :%d event %d\n",
                        swiEventReportCallBackResp.UimStatusTlv.intf,
                        swiEventReportCallBackResp.UimStatusTlv.event);
            }

        #if DEBUG_LOG_TO_FILE
            if(swiEventReportCallBackResp.TempTlv.TlvPresent!=0)
            {
                if (swiEventReportCallBackResp.TempTlv.TempStat > 4 )
                {
                    is_matching = 0;
                }
            }
            if(swiEventReportCallBackResp.VoltTlv.TlvPresent!=0)
            {
                if (swiEventReportCallBackResp.VoltTlv.VoltStat > 4 )
                {
                    is_matching = 0;
                }
            }
            if(swiEventReportCallBackResp.UimStatusTlv.TlvPresent!=0)
            {
                if (swiEventReportCallBackResp.UimStatusTlv.event > 1 )
                {
                    is_matching = 0;
                }
            }

            local_fprintf("%s,%s,", "unpack_dms_SwiEventReportCallBack_ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            
            local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
       #endif
            break;

        case eQMI_DMS_PSM_CFG_PARAMS_CHANGE_IND:       
            /* Unpack the Event Indication */
            unpackRetCode = unpack_dms_PSMCfgChange_ind( msg,rlen,&pSMCfgChangeResp );
            printf("SetCfgParamChangeCallback Callback Received\n");
            if((pSMCfgChangeResp.EnableState.TlvPresent) &&
                (swi_uint256_get_bit (pSMCfgChangeResp.ParamPresenceMask, 16)))
                printf( "Enable State: %d\n", pSMCfgChangeResp.EnableState.EnableStateInd );
            if((pSMCfgChangeResp.ActiveTimer.TlvPresent) && 
                (swi_uint256_get_bit (pSMCfgChangeResp.ParamPresenceMask, 17)))
                printf( "Active Timer: %d\n", pSMCfgChangeResp.ActiveTimer.ActiveTimerInd );
            if((pSMCfgChangeResp.PeriodicUpdateTimer.TlvPresent) && 
                (swi_uint256_get_bit (pSMCfgChangeResp.ParamPresenceMask, 18)))
                printf( "Periodic Update Timer: %d\n", pSMCfgChangeResp.PeriodicUpdateTimer.PeriodicUpdateTimerInd );

        #if DEBUG_LOG_TO_FILE
            if(pSMCfgChangeResp.EnableState.TlvPresent!=0)
            {
                if (pSMCfgChangeResp.EnableState.EnableStateInd > 1 )
                {
                    is_matching = 0;
                }
            }
            local_fprintf("%s,%s,", "unpack_dms_PSMCfgChange_ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            
            local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
        #endif
            break;

        default:
            break;
    }
}

void dms_response_handler(uint16_t msgid, uint8_t *msg, uint32_t rlen)
{
    unpack_dms_SLQSDmsSwiIndicationRegister_t dms_SLQSDmsSwiIndicationRegister;
    memset(&dms_SLQSDmsSwiIndicationRegister, 0, sizeof(dms_SLQSDmsSwiIndicationRegister));
    unpack_dms_SetIndicationRegister_t tunpack_dms_SetIndicationRegister;
    memset(&tunpack_dms_SetIndicationRegister, 0, sizeof(tunpack_dms_SetIndicationRegister));

    switch(msgid)
    {
        case eQMI_DMS_SET_EVENT:
            if (strcmp(funcName, "pack_dms_SetActivationStatusCallback")== 0)
            {    
                unpackRetCode = unpack_dms_SetActivationStatusCallback( msg, rlen ,&tunpack_dms_SetActivationStatusCallback);
                printf("QMI_DMS_SET_EVENT_REPORT RSP result :%d\n",unpackRetCode);

            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_SetActivationStatusCallback",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_SetActivationStatusCallback((void*)&tunpack_dms_SetActivationStatusCallback);
            }
            else
            {
                 unpack_dms_SetEventReport_t dms_SetEventReport;
                 memset(&dms_SetEventReport,0,sizeof(unpack_dms_SetEventReport_t));
                 unpackRetCode = unpack_dms_SetEventReport( msg, rlen ,&dms_SetEventReport);
                 printf("QMI_DMS_SET_EVENT_REPORT RSP result :%d\n",unpackRetCode);
             
            #if DEBUG_LOG_TO_FILE
                 local_fprintf("%s,%s,", "unpack_dms_SetEventReport",\
                     ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                 checkErrorAndFormErrorString(msg);
            #endif
                 dump_SetEventReport((void*)&dms_SetEventReport);
            }
            break;

        case eQMI_DMS_SWI_SET_EVENT_REPORT:
            unpackRetCode = unpack_dms_SwiSetEventReport( msg, rlen ,&SwiSetEventResp);
            printf("QMI_DMS_SWI_SET_EVENT_REPORT RSP result :%d\n",unpackRetCode);

   #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SwiSetEventReport",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
   #endif
            dump_SwiSetEventReport((void*)&SwiSetEventResp);
            break;

        case eQMI_DMS_GET_IMSI:
            unpackRetCode = unpack_dms_GetIMSI(msg, rlen, &imsi);
            fprintf(stderr, "unpack_dms_GetIMSI %d\n",unpackRetCode);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetIMSI",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetIMSI((void*)&imsi);
            break;

        case eQMI_DMS_GET_MODEL_ID:
            unpackRetCode = unpack_dms_GetModelID( msg, rlen,&model);
            fprintf(stderr,"unpack_dms_GetModelID %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetModelID",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetModelId((void*)&model);
            break;

        case eQMI_DMS_GET_CWE_SPKGS_INFO:
            unpackRetCode = unpack_dms_GetFirmwareInfo( msg, rlen,&fwinfo);
            fprintf(stderr,"unpack_dms_GetFirmwareInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetFirmwareInfo",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
         #endif
            dump_GetFirmwareInfo((void*)&fwinfo);
            break;

        case eQMI_DMS_GET_OPERATING_MODE:
            if (strcmp(funcName, "pack_dms_GetPower")== 0)
            {    
                unpackRetCode = unpack_dms_GetPower( msg, rlen,&power);
                fprintf(stderr,"unpack_dms_GetPower %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetPower",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_GetPower((void*)&power);
            }
            else
            {
                unpackRetCode = unpack_dms_GetOfflineReason( msg, rlen,&unpackdmsGetOfflineReason);
                 fprintf(stderr,"unpack_dms_GetOfflineReason %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetOfflineReason",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
    #endif
                dump_GetOfflineReason((void*)&unpackdmsGetOfflineReason);
            }
            break;

        case eQMI_DMS_GET_IDS:
            if (strcmp(funcName, "pack_dms_GetSerialNumbers")== 0)
            {
                unpackRetCode = unpack_dms_GetSerialNumbers( msg, rlen,&tunpack_dms_GetSerialNumbers);
                fprintf(stderr,"unpack_dms_GetSerialNumbers %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetSerialNumbers",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_GetSerialNumbers((void*)&tunpack_dms_GetSerialNumbers);
            }
            else
            {
                 unpackRetCode = unpack_dms_GetDeviceSerialNumbers( msg, rlen,&DeviceSerialNumbers);
                 fprintf(stderr,"unpack_dms_GetDeviceSerialNumbers %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                 local_fprintf("%s,%s,", "unpack_dms_GetDeviceSerialNumbers",\
                         ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                 checkErrorAndFormErrorString(msg);
            #endif
                 dump_GetDeviceSerialNumbers((void*)&DeviceSerialNumbers);
            }
            break;

        case eQMI_DMS_GET_HARDWARE_REV:
            if (strcmp(funcName, "pack_dms_GetDeviceHardwareRev")== 0)
            {
                unpackRetCode = unpack_dms_GetDeviceHardwareRev( msg, rlen,&HardwareRev);
                fprintf(stderr,"unpack_dms_GetDeviceHardwareRev %d\n",unpackRetCode);
    
           #if DEBUG_LOG_TO_FILE
               local_fprintf("%s,%s,", "unpack_dms_GetDeviceHardwareRev",\
                       ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
               checkErrorAndFormErrorString(msg);
           #endif
               dump_GetDeviceHardwareRev((void*)&HardwareRev);
            }
            else
            {
                  unpackRetCode = unpack_dms_GetHardwareRevision( msg, rlen,&tunpack_dms_GetHardwareRevision);
                  fprintf(stderr,"unpack_dms_GetHardwareRevision %d\n",unpackRetCode);
             
            #if DEBUG_LOG_TO_FILE
                 local_fprintf("%s,%s,", "unpack_dms_GetHardwareRevision",\
                         ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                 checkErrorAndFormErrorString(msg);
            #endif
                 dump_GetHardwareRevision((void*)&tunpack_dms_GetHardwareRevision);
    
            }
            break;

        case eQMI_DMS_GET_BAND_CAPS:
            unpackRetCode = unpack_dms_GetBandCapability( msg, rlen,&BandCapability);
            fprintf(stderr,"unpack_dms_GetBandCapability %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetBandCapability",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetBandCapability((void*)&BandCapability);
            memset(&dmsSLQSGetBandCapabilityExt,0,sizeof(dmsSLQSGetBandCapabilityExt));
            unpackRetCode = unpack_dms_SLQSGetBandCapabilityExt( msg, rlen,&dmsSLQSGetBandCapabilityExt);
            dump_SLQSGetBandCapabilityExt((void*)&dmsSLQSGetBandCapabilityExt);
            break;        
    
        case eQMI_DMS_GET_CAPS:
            unpackRetCode = unpack_dms_GetDeviceCapabilities( msg, rlen,&tunpack_dms_GetDeviceCapabilities);
            fprintf(stderr,"unpack_dms_GetDeviceCapabilities %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetDeviceCapabilities",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetDeviceCapabilities((void*)&tunpack_dms_GetDeviceCapabilities);
            break;

        case eQMI_DMS_GET_REV_ID:
            if (strcmp(funcName, "pack_dms_GetFirmwareRevisions")== 0)
            {
                unpackRetCode = unpack_dms_GetFirmwareRevisions( msg, rlen,&FirmwareRevisions);
                fprintf(stderr,"unpack_dms_GetFirmwareRevisions %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetFirmwareRevisions",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_FirmwareRevisions((void*)&FirmwareRevisions);
            }
            else
            {
                unpackRetCode = unpack_dms_GetFirmwareRevision( msg, rlen,&FirmwareRevision);
                fprintf(stderr,"unpack_dms_GetFirmwareRevision %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetFirmwareRevision",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_FirmwareRevision((void*)&FirmwareRevision);        
            }
            break;

        case eQMI_DMS_GET_CURRENT_PRL_INFO:
            unpackRetCode = unpack_dms_GetPRLVersion( msg, rlen,&PRLVersion);
            fprintf(stderr,"unpack_dms_GetPRLVersion %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetPRLVersion",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetPRLVersion((void*)&PRLVersion);
            break;

        case eQMI_DMS_GET_TIME:
            unpackRetCode = unpack_dms_GetNetworkTime( msg, rlen,&Time);
            fprintf(stderr,"unpack_dms_GetNetworkTime %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetNetworkTime",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetNetworkTime((void*)&Time);
            break;

        case eQMI_DMS_GET_MSISDN:
            unpackRetCode = unpack_dms_GetVoiceNumber( msg, rlen,&VoiceNumber);
            fprintf(stderr,"unpack_dms_GetVoiceNumber %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetVoiceNumber",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetVoiceNumber((void*)&VoiceNumber);
            break;

        case eQMI_DMS_GET_FSN:
            unpackRetCode = unpack_dms_GetFSN( msg, rlen,&FSN);
            fprintf(stderr,"unpack_dms_GetFSN %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetFSN",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetFSN((void*)&FSN);
            break;

        case eQMI_DMS_SET_OPERATING_MODE:
            unpackRetCode = unpack_dms_SetPower( msg, rlen, &SetPowerResp);
            fprintf(stderr,"unpack_dms_SetPower %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SetPower",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SetPower((void*)&SetPowerResp);
            break;

        case eQMI_DMS_SWI_GET_USB_COMP:
            unpackRetCode = unpack_dms_GetUSBComp( msg, rlen, &GetUSBComp);
            fprintf(stderr,"unpack_dms_GetUSBComp %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetUSBComp",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetUSBComp((void*)&GetUSBComp);
            break;

        case eQMI_DMS_SWI_SET_USB_COMP:
            unpackRetCode = unpack_dms_SetUSBComp( msg, rlen, &SetUSBComp);
            fprintf(stderr,"unpack_dms_SetUSBComp %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SetUSBComp",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SetUSBComp((void*)&SetUSBComp);
            break;

        case eQMI_DMS_SWI_SET_CUST_FEATURES:
            if (strcmp(funcName, "pack_dms_SetCustFeature")== 0)
            {
                unpackRetCode = unpack_dms_SetCustFeature( msg, rlen, &SetCusFeatureResp);
                fprintf(stderr,"unpack_dms_SetCustFeature %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_SetCustFeature",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_SetCustFeature((void*)&SetCusFeatureResp);
            }
            else
            {
                 unpackRetCode = unpack_dms_SetCustFeaturesV2( msg, rlen, &SetCustFeaturesV2Resp);
                 fprintf(stderr,"unpack_dms_SetCustFeaturesV2 %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                 local_fprintf("%s,%s,", "unpack_dms_SetCustFeaturesV2",\
                         ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                 checkErrorAndFormErrorString(msg);
            #endif
                 dump_SetCustFeaturesV2((void*)&SetCustFeaturesV2Resp);
            }
            break;

        case eQMI_DMS_SWI_GET_CUST_FEATURES:
            if (strcmp(funcName, "pack_dms_GetCustFeature")== 0)
            {
               unpackRetCode = unpack_dms_GetCustFeature( msg, rlen, &GetCusFeatureResp);
               fprintf(stderr,"unpack_dms_GetCustFeature %d\n",unpackRetCode);
           #if DEBUG_LOG_TO_FILE
               local_fprintf("%s,%s,", "unpack_dms_GetCustFeature",\
                       ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
               checkErrorAndFormErrorString(msg);
           #endif
               dump_GetCustFeature((void*)&GetCusFeatureResp);
            }
            else
            {
                unpackRetCode = unpack_dms_GetCustFeaturesV2( msg, rlen, &GetCustFeaturesV2Resp);
                fprintf(stderr,"unpack_dms_GetCustFeaturesV2 %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetCustFeaturesV2",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_GetCustFeaturesV2((void*)&GetCustFeaturesV2Resp);
            }
            break;

        case eQMI_DMS_SWI_GET_CRASH_ACTION:
            unpackRetCode = unpack_dms_GetCrashAction( msg, rlen, &GetCrashAction);
            fprintf(stderr,"unpack_dms_GetCrashAction %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetCrashAction",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetCrashAction((void*)&GetCrashAction);
            break;

        case eQMI_DMS_GET_MANUFACTURER:
            if (strcmp(funcName, "pack_dms_GetDeviceMfr")== 0)
            {    
                unpackRetCode = unpack_dms_GetDeviceMfr( msg, rlen, &GetDeviceMfr);
                fprintf(stderr,"unpack_dms_GetDeviceMfr %d\n",unpackRetCode);
            #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetDeviceMfr",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
            #endif
                dump_GetDeviceMfr((void*)&GetDeviceMfr);
            }
            else
            {
                unpackRetCode = unpack_dms_GetManufacturer( msg, rlen, &unpackdmsGetManufacturer);
                fprintf(stderr,"unpack_dms_GetManufacturer %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_dms_GetManufacturer",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                checkErrorAndFormErrorString(msg);
    #endif
                dump_GetManufacturer((void*)&unpackdmsGetManufacturer);
            }
            break;

        case eQMI_DMS_UIM_GET_ICCID:
            unpackRetCode = unpack_dms_UIMGetICCID( msg, rlen, &UIMGetICCIDResp);
            fprintf(stderr,"unpack_dms_UIMGetICCID %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMGetICCID",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMGetICCID((void*)&UIMGetICCIDResp);
            break;

        case eQMI_DMS_GET_ACTIVATED_STATE:
            unpackRetCode = unpack_dms_GetActivationState( msg, rlen, &ActivationStateResp);
            fprintf(stderr,"unpack_dms_GetActivationState %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_GetActivationState",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_GetActivationState((void*)&ActivationStateResp);
            break;

        case eQMI_DMS_SWI_GET_FIRMWARE_CURR:
            unpackRetCode = unpack_dms_SLQSSwiGetFirmwareCurr( msg, rlen, &cur_fw);
            fprintf(stderr,"unpack_dms_SLQSSwiGetFirmwareCurr %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetFirmwareCurr",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetFirmwareCurr((void*)&cur_fw);
            break;

        case eQMI_DMS_SWI_SET_DYING_GASP_CFG:
            unpackRetCode = unpack_dms_SLQSSwiSetDyingGaspCfg( msg, rlen, &unpackdmsSLQSSwiSetDyingGaspCfg);
            fprintf(stderr,"unpack_dms_SLQSSwiSetDyingGaspCfg %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiSetDyingGaspCfg",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiSetDyingGaspCfg((void*)&unpackdmsSLQSSwiSetDyingGaspCfg);
            break;

        case eQMI_DMS_SWI_CLR_DYING_GASP_STAT:
            unpackRetCode = unpack_dms_SLQSSwiClearDyingGaspStatistics( msg, rlen, &unpackdmsSLQSSwiClearDyingGaspStatistics);
            fprintf(stderr,"unpack_dms_SLQSSwiClearDyingGaspStatistics %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiClearDyingGaspStatistics",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiClearDyingGaspStatistics((void*)&unpackdmsSLQSSwiClearDyingGaspStatistics);
            break;

        case eQMI_DMS_SWI_GET_DYING_GASP_STAT:
            unpackRetCode = unpack_dms_SLQSSwiGetDyingGaspStatistics( msg, rlen, &unpackdmsSLQSSwiGetDyingGaspStatistics);
            fprintf(stderr,"unpack_dms_SLQSSwiGetDyingGaspStatistics %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetDyingGaspStatistics",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetDyingGaspStatistics((void*)&unpackdmsSLQSSwiGetDyingGaspStatistics);
            break;

        case eQMI_DMS_SWI_GET_RESET_INFO:
            unpackRetCode = unpack_dms_SLQSDmsSwiGetResetInfo( msg, rlen, &unpackdmsSLQSDmsSwiGetResetInfo);
            fprintf(stderr,"unpack_dms_SLQSDmsSwiGetResetInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSDmsSwiGetResetInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSDmsSwiGetResetInfo((void*)&unpackdmsSLQSDmsSwiGetResetInfo);
            break;

        case eQMI_DMS_SWI_INDICATION_REGISTER:;
            unpackRetCode = unpack_dms_SLQSDmsSwiIndicationRegister( msg, rlen, &dms_SLQSDmsSwiIndicationRegister);
            fprintf(stderr,"unpack_dms_SLQSDmsSwiIndicationRegister %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s\n", "unpack_dms_SLQSDmsSwiIndicationRegister",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            break;

        case eQMI_DMS_SWI_SET_CRASH_ACTION:
            unpackRetCode = unpack_dms_SetCrashAction( msg, rlen, &tunpack_dms_SetCrashAction);
            fprintf(stderr,"unpack_dms_SetCrashAction %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SetCrashAction",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SetCrashAction((void*)&tunpack_dms_SetCrashAction);
            break;

        case eQMI_DMS_SWI_GET_FW_UPDATE_STAT:
            unpackRetCode = unpack_dms_SLQSSwiGetFwUpdateStatus( msg, rlen, &unpackdmsSLQSSwiGetFwUpdateStatus);
            fprintf(stderr,"unpack_dms_SLQSSwiGetFwUpdateStatus %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetFwUpdateStatus",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetFwUpdateStatus((void*)&unpackdmsSLQSSwiGetFwUpdateStatus);
            break;

        case eQMI_DMS_UIM_SET_PIN_PROT:
            unpackRetCode = unpack_dms_UIMSetPINProtection( msg, rlen, &unpackdmsUIMSetPINProtection);
            fprintf(stderr,"unpack_dms_UIMSetPINProtection %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMSetPINProtection",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMSetPINProtection((void*)&unpackdmsUIMSetPINProtection);
            break;

        case eQMI_DMS_UIM_PIN_UNBLOCK:
            unpackRetCode = unpack_dms_UIMUnblockPIN( msg, rlen, &unpackdmsUIMUnblockPIN);
            fprintf(stderr,"unpack_dms_UIMUnblockPIN %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMUnblockPIN",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMUnblockPIN((void*)&unpackdmsUIMUnblockPIN);
            break;

        case eQMI_DMS_UIM_PIN_VERIFY:
            unpackRetCode = unpack_dms_UIMVerifyPIN( msg, rlen, &unpackdmsUIMVerifyPIN);
            fprintf(stderr,"unpack_dms_UIMVerifyPIN %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMVerifyPIN",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMVerifyPIN((void*)&unpackdmsUIMVerifyPIN);
            break;

        case eQMI_DMS_UIM_PIN_CHANGE:
            unpackRetCode = unpack_dms_UIMChangePIN( msg, rlen, &unpackdmsUIMChangePIN);
            fprintf(stderr,"unpack_dms_UIMChangePIN %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMChangePIN",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMChangePIN((void*)&unpackdmsUIMChangePIN);
            break;

        case eQMI_DMS_UIM_GET_CK_STATUS:
            unpackRetCode = unpack_dms_UIMGetControlKeyStatus( msg, rlen, &unpackdmsUIMGetControlKeyStatus);
            fprintf(stderr,"unpack_dms_UIMGetControlKeyStatus %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMGetControlKeyStatus",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMGetControlKeyStatus((void*)&unpackdmsUIMGetControlKeyStatus);
            break;

        case eQMI_DMS_UIM_GET_PIN_STATUS:
            unpackRetCode = unpack_dms_UIMGetPINStatus( msg, rlen, &unpackdmsUIMGetPINStatus);
            fprintf(stderr,"unpack_dms_UIMGetPINStatus %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMGetPINStatus",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMGetPINStatus((void*)&unpackdmsUIMGetPINStatus);
            break;

        case eQMI_DMS_UIM_SET_CK_PROT:
            unpackRetCode = unpack_dms_UIMSetControlKeyProtection( msg, rlen, &unpackdmsUIMSetControlKeyProtection);
            fprintf(stderr,"unpack_dms_UIMSetControlKeyProtection %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMSetControlKeyProtection",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMSetControlKeyProtection((void*)&unpackdmsUIMSetControlKeyProtection);
            break;

        case eQMI_DMS_UIM_UNBLOCK_CK:
            unpackRetCode = unpack_dms_UIMUnblockControlKey( msg, rlen, &unpackdmsUIMUnblockControlKey);
            fprintf(stderr,"unpack_dms_UIMUnblockControlKey %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_UIMUnblockControlKey",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_UIMUnblockControlKey((void*)&unpackdmsUIMUnblockControlKey);
            break;

        case eQMI_DMS_FACTORY_DEFAULTS:
            unpackRetCode = unpack_dms_ResetToFactoryDefaults( msg, rlen, &unpackdmsResetToFactoryDefaults);
            fprintf(stderr,"unpack_dms_ResetToFactoryDefaults %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_ResetToFactoryDefaults",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_ResetToFactoryDefaults((void*)&unpackdmsResetToFactoryDefaults);
            break;

        case eQMI_DMS_VALIDATE_SPC:
            unpackRetCode = unpack_dms_ValidateSPC( msg, rlen, &unpackdmsResetToFactoryDefaults);
            fprintf(stderr,"unpack_dms_ValidateSPC %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_ValidateSPC",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_ValidateSPC((void*)&unpackdmsResetToFactoryDefaults);
            break;

        case eQMI_DMS_ACTIVATE_AUTOMATIC:
            unpackRetCode = unpack_dms_ActivateAutomatic( msg, rlen, &unpackdmsResetToFactoryDefaults);
            fprintf(stderr,"unpack_dms_ActivateAutomatic %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_ActivateAutomatic",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_ActivateAutomatic((void*)&unpackdmsResetToFactoryDefaults);
            break;

        case eQMI_DMS_READ_ERI_FILE:
            unpackRetCode = unpack_dms_SLQSGetERIFile( msg, rlen, &unpackdmsSLQSGetERIFile);
            fprintf(stderr,"unpack_dms_SLQSGetERIFile %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSGetERIFile",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSGetERIFile((void*)&unpackdmsSLQSGetERIFile);
            break;

        case eQMI_DMS_UIM_GET_STATE:
            unpackRetCode = unpack_dms_SLQSUIMGetState( msg, rlen, &unpackdmsSLQSUIMGetState);
            fprintf(stderr,"unpack_dms_SLQSUIMGetState %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSUIMGetState",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSUIMGetState((void*)&unpackdmsSLQSUIMGetState);
            break;

        case eQMI_DMS_SWI_GET_CRASH_INFO:
            unpackRetCode = unpack_dms_SLQSSwiGetCrashInfo( msg, rlen, &unpackdmsSLQSSwiGetCrashInfo);
            fprintf(stderr,"unpack_dms_SLQSSwiGetCrashInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetCrashInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetCrashInfo((void*)&unpackdmsSLQSSwiGetCrashInfo);
            break;

         case eQMI_DMS_SWI_GET_HOST_DEV_INFO:
            unpackRetCode = unpack_dms_SLQSSwiGetHostDevInfo( msg, rlen, &unpackdmsSLQSSwiGetHostDevInfo);
            fprintf(stderr,"unpack_dms_SLQSSwiGetHostDevInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetHostDevInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetHostDevInfo((void*)&unpackdmsSLQSSwiGetHostDevInfo);
            break;   

        case eQMI_DMS_SWI_SET_HOST_DEV_INFO:
            unpackRetCode = unpack_dms_SLQSSwiSetHostDevInfo( msg, rlen, &unpackdmsSLQSSwiSetHostDevInfo);
            fprintf(stderr,"unpack_dms_SLQSSwiSetHostDevInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiSetHostDevInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiSetHostDevInfo((void*)&unpackdmsSLQSSwiSetHostDevInfo);
            break;

        case eQMI_DMS_SWI_GET_OS_INFO:
            unpackRetCode = unpack_dms_SLQSSwiGetOSInfo( msg, rlen, &unpackdmsSLQSSwiGetOSInfo);
            fprintf(stderr,"unpack_dms_SLQSSwiGetOSInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetOSInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetOSInfo((void*)&unpackdmsSLQSSwiGetOSInfo);
            break;

        case eQMI_DMS_SWI_SET_OS_INFO:
            unpackRetCode = unpack_dms_SLQSSwiSetOSInfo( msg, rlen, NULL);
            fprintf(stderr,"unpack_dms_SLQSSwiSetOSInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiSetOSInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiSetOSInfo(NULL);
            break;

        case eQMI_DMS_SWI_GET_SERIAL_NO_EXT:
            unpackRetCode = unpack_dms_SLQSSwiGetSerialNoExt( msg, rlen, &unpackdmsSLQSSwiGetSerialNoExt);
            fprintf(stderr,"unpack_dms_SLQSSwiGetSerialNoExt %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSwiGetSerialNoExt",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSwiGetSerialNoExt((void*)&unpackdmsSLQSSwiGetSerialNoExt);
            break;

        case eQMI_DMS_SWI_GET_PC_INFO:
            unpackRetCode = unpack_dms_SLQSDmsSwiGetPCInfo( msg, rlen, &tunpack_dms_SLQSDmsSwiGetPCInfo);
            fprintf(stderr,"unpack_dms_SLQSDmsSwiGetPCInfo %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSDmsSwiGetPCInfo",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSDmsSwiGetPCInfo((void*)&tunpack_dms_SLQSDmsSwiGetPCInfo);
            break;

        case eQMI_DMS_PSM_GET_CFG_PARAMS:
            unpackRetCode = unpack_dms_SLQSGetPowerSaveModeConfig( msg, rlen, &tunpack_dms_SLQSGetPowerSaveModeConfig);
            fprintf(stderr,"unpack_dms_SLQSGetPowerSaveModeConfig %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSGetPowerSaveModeConfig",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSGetPowerSaveModeConfig((void*)&tunpack_dms_SLQSGetPowerSaveModeConfig);
            break;

        case eQMI_DMS_PSM_SET_CFG_PARAMS:
            unpackRetCode = unpack_dms_SLQSSetPowerSaveModeConfig( msg, rlen, NULL);
            fprintf(stderr,"unpack_dms_SLQSSetPowerSaveModeConfig %d\n",unpackRetCode);
        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSSetPowerSaveModeConfig",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
        #endif
            dump_SLQSSetPowerSaveModeConfig(NULL);
            break;

        case eQMI_DMS_INDICATION_REGISTER:
            memset(&tunpack_dms_SetIndicationRegister,0,sizeof(unpack_dms_SetIndicationRegister_t));
            unpackRetCode = unpack_dms_SetIndicationRegister( msg, rlen, &tunpack_dms_SetIndicationRegister);
            fprintf(stderr,"unpack_dms_SetIndicationRegister %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s\n", "unpack_dms_SetIndicationRegister",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
    #endif
            break;

        case eQMI_DMS_SWI_UIM_SELECT:
            unpackRetCode = unpack_dms_SwiUimSelect( msg, rlen, &SwiUIMSelectRsp);
            fprintf(stderr,"unpack_dms_SwiUimSelect %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SwiUimSelect",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
    #endif
            dump_SwiUimSelect((void*)&SwiUIMSelectRsp);
            break;

        case eQMI_DMS_SWI_GET_UIM_SELECTION:
            unpackRetCode = unpack_dms_SLQSDmsSwiGetUimSelection( msg, rlen, &tunpack_dms_SLQSDmsSwiGetUimSelection);
            fprintf(stderr,"unpack_dms_SLQSDmsSwiGetUimSelection %d\n",unpackRetCode);
#if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_dms_SLQSDmsSwiGetUimSelection",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            checkErrorAndFormErrorString(msg);
#endif
            dump_SLQSDmsSwiGetUimSelection((void*)&tunpack_dms_SLQSDmsSwiGetUimSelection);
            break;

        default:
            break;
    }

}

void *dms_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;
    uint16_t rspLen = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;

    UNUSEDPARAM(ptr);
    while(enDmsThread)
    {
        //TODO select multiple file and read them
        rspLen = rtn = read(dms, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eDMS, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex((uint16_t )rtn, msg.buf);

            if(g_runoem_demo == 1)
            {
                if (rsp_ctx.type == eIND)
                {
                    printf("DMS IND: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                }
                else
                {
                    printf("DMS RSP: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    if (Sendxid == rsp_ctx.xid)
                    {
                        unpackRetCode = run_unpack_item(oemapitest_dms[g_oemApiIndex].unpack)(msg.buf, rspLen, oemapitest_dms[g_oemApiIndex].unpack_ptr);
                        #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", oemapitest_dms[g_oemApiIndex].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                        if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                        {
                            eRCode = helper_get_error_code(msg.buf);
                            reason = helper_get_error_reason(eRCode);
                            sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                        } 
                        #endif
                        oemapitest_dms[g_oemApiIndex].dump(oemapitest_dms[g_oemApiIndex].unpack_ptr);
                    }
                }
            }
            else
            {
                if (rsp_ctx.type == eIND)
                {
                    printf("DMS IND: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    dms_indication_handler(rsp_ctx.msgid, msg.buf,  rspLen);
                }
                else if (rsp_ctx.type == eRSP)
                {
                    printf("DMS RSP: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    dms_response_handler(rsp_ctx.msgid, msg.buf,  rspLen);
                }
            }

            if ((rsp_ctx.type == eRSP)&&(Sendxid==rsp_ctx.xid))
            {
                ReadyToWriteNext = 1;//Ready to write Next message
            }
        }
        else
        {
            enDmsThread = 0;
            dms = -1;
            printf("enDmsThread exit read error: %d\n",rtn);fflush(stdout);
            break;
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////

void dms_test_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;
    unsigned xid =1;
    const char *qmi_msg;

    printf("======DMS dummy unpack test===========\n");
    iLocalLog = 0;
    dms_validate_dummy_unpack();
    fms_validate_dummy_unpack();
    iLocalLog = 1;

    printf("======DMS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "DMS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    if(dms<0)
        dms = client_fd(eDMS);
    if(dms<0)
    {
        fprintf(stderr,"DMS Service Not Supported\n");
        return ;
    }

    enDmsThread = 1;
    pthread_create(&dms_tid, &dms_attr, dms_read_thread, NULL);
    sleep(1);


    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitest_dms)/sizeof(testitem_t); 
    }
    else
    {
        numoftestcase = sizeof(dmstotest)/sizeof(testitem_t);
    }
 
    for(i=0; i<numoftestcase; i++)
    {
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t req[QMI_MSG_MAX];
        uint16_t reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;

        /* reset the funcName before every pack call */
        memset(funcName, 0, sizeof(funcName));

        if(g_runoem_demo==1)
        {
            if (strstr(oemapitest_dms[i].pack_func_name,"pack_dms_SetCrashAction") != NULL)
            {
                rtn = pack_dms_SetCrashAction(&req_ctx, req, &reqLen,
                          *(pack_dms_SetCrashAction_t *)oemapitest_dms[i].pack_ptr);
            }
            else
            {
                rtn = run_pack_item(oemapitest_dms[i].pack)(&req_ctx, req, &reqLen,oemapitest_dms[i].pack_ptr);
            }
        }
        else
        {
            if (strstr(dmstotest[i].pack_func_name,"pack_dms_SetCrashAction") != NULL)
            {
                rtn = pack_dms_SetCrashAction(&req_ctx, req, &reqLen,
                          *(pack_dms_SetCrashAction_t *)dmstotest[i].pack_ptr);
            }
            else
            {
                rtn = run_pack_item(dmstotest[i].pack)(&req_ctx, req, &reqLen,dmstotest[i].pack_ptr);
            }
        }

#if DEBUG_LOG_TO_FILE
        if(g_runoem_demo==1)
        {
            local_fprintf("%s,%s,%s\n", oemapitest_dms[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
        else
        {
            local_fprintf("%s,%s,%s\n", dmstotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
#endif
        /* remember which pack_function was invoked, so that it can be used to identify which unpack_function should be invoked later on */
        if(g_runoem_demo==1)
        {
            g_oemApiIndex = i;
            strcpy(funcName, oemapitest_dms[i].pack_func_name);
        }
        else
        {
            strcpy(funcName, dmstotest[i].pack_func_name);
        }

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            continue;
        }

        rtn = write(dms, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eDMS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen,req);
        }
        Sendxid = xid; 
        rtn = 0;
        ReadyToWriteNext = 0;
        do
        {
            if(ReadyToWriteNext)
                break;
            sleep(1);
        }while(rtn++<120);//120 Seconds timeout
        xid++;

    }
    sleep(5);
    if(dms>=0)
        close(dms);
    dms=-1;
    enDmsThread = -1;
}


void dms_test_ind()
{
    pack_qmi_t req_ctx;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len;
    const char *qmi_msg;
    int rtn;
    if(dms<0)
        dms = client_fd(eDMS);
    if(dms<0)
    {
        fprintf(stderr,"DMS Service Not Supported\n");
        return ;
    }

    enDmsThread = 1;
    pthread_create(&dms_tid, &dms_attr, dms_read_thread, NULL);
    sleep(1);

    pack_dms_SetEventReport_t dmsReqPara;
    memset(&dmsReqPara,0,sizeof(pack_dms_SetEventReport_t));
    req_ctx.xid = 0x100;
    memset(&dms_attr, 0, sizeof(dms_attr));
    dmsReqPara.mode = DMS_SET_REPORT_ENABLE;
    rtn = pack_dms_SetEventReport(&req_ctx, qmi_req, &qmi_req_len, (void*)&dmsReqPara);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }


    pack_dms_SwiSetEventReport_t SwiEventSettingsUINStatus = {NULL, NULL, &swiUIMStatusInd};
    req_ctx.xid++;
    rtn = pack_dms_SwiSetEventReport(&req_ctx, qmi_req, &qmi_req_len, (void*)&SwiEventSettingsUINStatus);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("SWI Set Event Report Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }

    req_ctx.xid++;
    rtn = pack_dms_SwiSetEventReport(&req_ctx, qmi_req, &qmi_req_len, (void*)&SwiEventSettings);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("SWI Set Event Report Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }

    pack_dms_SLQSDmsSwiIndicationRegister_t dmsIndRegPara;
    memset(&dmsIndRegPara,0,sizeof(pack_dms_SLQSDmsSwiIndicationRegister_t));
    req_ctx.xid++;;
    dmsIndRegPara.resetInfoInd = DMS_SWI_SET_IND_ENABLE;
    rtn = pack_dms_SLQSDmsSwiIndicationRegister(&req_ctx, qmi_req, &qmi_req_len, (void*)&dmsIndRegPara);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set SWI Indication Register Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error \n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }

    pack_dms_SetIndicationRegister_t dmsSetIndRegPara;
    memset(&dmsSetIndRegPara,0,sizeof(pack_dms_SetIndicationRegister_t));
    req_ctx.xid++;;
    dmsSetIndRegPara.PSMCfgChangeInfo = DMS_SET_REG_IND_ENABLE;
    dmsSetIndRegPara.PSMStatus = dmsSetIndRegPara.RptIMSCapability = DMS_SET_REG_IND_NO_CHANGE;
    rtn = pack_dms_SetIndicationRegister(&req_ctx, qmi_req, &qmi_req_len, (void*)&dmsSetIndRegPara);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set DMS Indication Register Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error \n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }

    uint8_t    setPsmEnableState = 1;
    pack_dms_SLQSSetPowerSaveModeConfig_t dmsPSMConfigParam;

    memset(&dmsPSMConfigParam,0,sizeof(pack_dms_SLQSSetPowerSaveModeConfig_t));
    req_ctx.xid++;;
    dmsPSMConfigParam.pPsmEnableState = &setPsmEnableState;
    rtn = pack_dms_SLQSSetPowerSaveModeConfig(&req_ctx, qmi_req, &qmi_req_len, (void*)&dmsPSMConfigParam);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set DMS PSM Change Param Error\n");
        return ;
    }
    rtn = write(dms, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error \n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eDMS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len,qmi_req);
    }
    sleep(5);

}

void dms_test_ind_exit()
{
    if(enDmsThread==0)
    {
        if(dms>=0)
            close(dms);
        dms = -1;
    }
    enDmsThread = 0;
    printf("\nkilling DMS read thread...\n");
#ifdef __ARM_EABI__
    if(dms>=0)
        close(dms);
    dms = -1;
    void *pthread_rtn_value;
    if(dms_tid!=0)
    pthread_join(dms_tid, &pthread_rtn_value);
#endif
    if(dms_tid!=0)
    pthread_cancel(dms_tid);
    dms_tid = 0;
    if(dms>=0)
        close(dms);
    dms = -1;
}

uint8_t dummy_dms_resp_msg[][QMI_MSG_MAX] = { 
    //eQMI_DMS_LIST_FIRMWARE
    {0x02,0x01,0x00,0x49,0x00,0xeb,0x00,
     0x02,0x04,0x00,
        0x00,0x00,0x00,0x00,
     0x01,0xe1,0x00,
        0x02,0x00,0x04,0x02,0x02,0x01,0x00,0x3f,0x5f,
        0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x0d,0x30,0x32,0x2e,
        0x33,0x30,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
        0x3f,0x02,0x00,0x3f,0x5f,0x3f,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x0d,0x30,0x32,0x2e,0x33,0x30,0x2e,0x30,
        0x31,0x2e,0x30,0x31,0x5f,0x3f,0x01,0x32,0x03,
        0x04,0xff,0xff,
        0x30,0x30,0x32,0x2e,0x30,0x35,0x30,0x5f,
        0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x0f,
        0x30,0x32,0x2e,0x33,0x30,0x2e,0x30,
        0x33,0x2e,0x30,0x30,0x5f,0x41,0x54,0x54,0xff,0xff,
        0x30,0x30,0x32,0x2e,0x30,0x34,0x36,0x5f,0x30,0x30,0x30,
        0x00,0x00,0x00,0x00,0x00,0x13,
        0x30,0x32,0x2e,0x33,0x30,0x2e,0x30,0x33,0x2e,
        0x30,0x30,0x5f,0x47,0x45,0x4e,0x45,0x52,0x49,
        0x43,0xff,0xff,0x30,0x30,0x30,0x2e,0x30,0x32,
        0x31,0x5f,0x30,0x30,0x30,0x00,0x00,0x00,0x00,
        0x00,0x17,0x30,0x32,0x2e,0x33,0x30,0x2e,0x30,
        0x33,0x2e,0x30,0x30,0x5f,0x55,0x53,0x2d,0x43,
        0x65,0x6c,0x6c,0x75,0x6c,0x61,0x72,0xff,0xff,
        0x30,0x30,0x32,0x2e,0x30,0x35,0x32,0x5f,0x30,0x30,0x30,
        0x00,0x00,0x00,0x00,0x00,0x13,0x30,
        0x32,0x2e,0x33,0x30,0x2e,0x30,0x31,0x2e,0x30,
        0x31,0x5f,0x56,0x45,0x52,0x49,0x5a,0x4f,0x4e},
    // receiving eQMI_DMS_SWI_GET_USB_COMP
    {0x02,0x01,0x00,0x5b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_GET_USB_COMP
    {0x02,0x19,0x00,0x5b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_GET_USB_COMP
    {0x02,0x1b,0x00,0x5b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_GET_USB_COMP
    {0x02,0x01,0x00,0x5b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x02,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x3b,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x0f,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x02,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x26,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_GET_OPERATING_MODE
    {0x02,0x0d,0x00,0x2d,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x03,0x00,0x57,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x04,0x00,0x57,0x65,0x17,0x02,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x20,0x11,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x21,0xf9,0x01,0x00,0x1b,0x00,
    0x09,0x00,0x47,0x50,0x53,0x45,0x4e,0x41,
    0x42,0x4c,0x45,0x01,0x00,0x01,0x01,0x00,
    0x06,0x00,0x47,0x50,0x53,0x4c,0x50,0x4d,
    0x01,0x00,0x00,0x01,0x00,0x0d,0x00,0x47,
    0x50,0x49,0x4f,0x53,0x41,0x52,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x00,0x01,
    0x00,0x06,0x00,0x47,0x50,0x53,0x53,0x45,
    0x4c,0x01,0x00,0x00,0x01,0x00,0x09,0x00,
    0x47,0x50,0x53,0x52,0x45,0x46,0x4c,0x4f,
    0x43,0x01,0x00,0x00,0x01,0x00,0x0c,0x00,
    0x49,0x4d,0x53,0x57,0x49,0x54,0x43,0x48,
    0x48,0x49,0x44,0x45,0x01,0x00,0x00,0x01,
    0x00,0x08,0x00,0x49,0x4d,0x43,0x4f,0x4e,
    0x46,0x49,0x47,0x01,0x00,0x00,0x01,0x00,
    0x0a,0x00,0x49,0x50,0x56,0x36,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x01,0x01,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x0d,0x00,0x53,0x49,0x4d,0x48,
    0x4f,0x54,0x53,0x57,0x41,0x50,0x44,0x49,
    0x53,0x01,0x00,0x00,0x01,0x00,0x0d,0x00,
    0x43,0x46,0x55,0x4e,0x50,0x45,0x52,0x53,
    0x49,0x53,0x54,0x45,0x4e,0x01,0x00,0x00,
    0x01,0x00,0x0b,0x00,0x51,0x4d,0x49,0x44,
    0x45,0x54,0x41,0x43,0x48,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0a,0x00,0x55,0x49,
    0x4d,0x32,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x02,0x01,0x00,0x0e,0x00,0x4e,
    0x45,0x54,0x57,0x4f,0x52,0x4b,0x4e,0x41,
    0x4d,0x45,0x46,0x4d,0x54,0x01,0x00,0x00,
    0x01,0x00,0x06,0x00,0x53,0x49,0x4d,0x4c,
    0x50,0x4d,0x01,0x00,0x01,0x01,0x00,0x0f,
    0x00,0x55,0x53,0x42,0x53,0x45,0x52,0x49,
    0x41,0x4c,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x01,0x01,0x00,0x0b,0x00,0x50,
    0x43,0x53,0x43,0x44,0x49,0x53,0x41,0x42,
    0x4c,0x45,0x01,0x00,0x00,0x01,0x00,0x0f,
    0x00,0x44,0x48,0x43,0x50,0x52,0x45,0x4c,
    0x41,0x59,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x00,0x01,0x00,0x0f,0x00,0x46,
    0x4c,0x4f,0x57,0x4e,0x4f,0x54,0x49,0x44,
    0x49,0x53,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0a,0x00,0x46,0x41,0x53,
    0x54,0x45,0x4e,0x55,0x4d,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0d,0x00,0x43,0x53,
    0x56,0x4f,0x49,0x43,0x45,0x52,0x45,0x4a,
    0x45,0x43,0x54,0x01,0x00,0x00,0x01,0x00,
    0x0f,0x00,0x49,0x50,0x43,0x48,0x41,0x4e,
    0x4e,0x45,0x4c,0x52,0x41,0x54,0x45,0x45,
    0x4e,0x01,0x00,0x00,0x01,0x00,0x0f,0x00,
    0x53,0x49,0x4e,0x47,0x4c,0x45,0x41,0x50,
    0x4e,0x53,0x57,0x49,0x54,0x43,0x48,0x01,
    0x00,0x00,0x01,0x00,0x08,0x00,0x44,0x47,
    0x45,0x4e,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0e,0x00,0x54,0x58,0x4f,
    0x4e,0x49,0x4e,0x44,0x49,0x43,0x41,0x54,
    0x49,0x4f,0x4e,0x01,0x00,0x00,0x01,0x00,
    0x0d,0x00,0x49,0x43,0x4d,0x50,0x49,0x4e,
    0x54,0x53,0x52,0x56,0x44,0x49,0x53,0x01,
    0x00,0x00,0x01,0x00,0x0f,0x00,0x53,0x55,
    0x42,0x4e,0x45,0x54,0x4d,0x53,0x4b,0x45,
    0x4e,0x41,0x42,0x4c,0x45,0x05,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x2c,0x00,0x57,0x65,0x17,0x02,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x20,0x11,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x21,0xf9,0x01,0x00,0x1b,0x00,
    0x09,0x00,0x47,0x50,0x53,0x45,0x4e,0x41,
    0x42,0x4c,0x45,0x01,0x00,0x01,0x01,0x00,
    0x06,0x00,0x47,0x50,0x53,0x4c,0x50,0x4d,
    0x01,0x00,0x00,0x01,0x00,0x0d,0x00,0x47,
    0x50,0x49,0x4f,0x53,0x41,0x52,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x00,0x01,
    0x00,0x06,0x00,0x47,0x50,0x53,0x53,0x45,
    0x4c,0x01,0x00,0x00,0x01,0x00,0x09,0x00,
    0x47,0x50,0x53,0x52,0x45,0x46,0x4c,0x4f,
    0x43,0x01,0x00,0x00,0x01,0x00,0x0c,0x00,
    0x49,0x4d,0x53,0x57,0x49,0x54,0x43,0x48,
    0x48,0x49,0x44,0x45,0x01,0x00,0x00,0x01,
    0x00,0x08,0x00,0x49,0x4d,0x43,0x4f,0x4e,
    0x46,0x49,0x47,0x01,0x00,0x00,0x01,0x00,
    0x0a,0x00,0x49,0x50,0x56,0x36,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x01,0x01,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x0d,0x00,0x53,0x49,0x4d,0x48,
    0x4f,0x54,0x53,0x57,0x41,0x50,0x44,0x49,
    0x53,0x01,0x00,0x00,0x01,0x00,0x0d,0x00,
    0x43,0x46,0x55,0x4e,0x50,0x45,0x52,0x53,
    0x49,0x53,0x54,0x45,0x4e,0x01,0x00,0x00,
    0x01,0x00,0x0b,0x00,0x51,0x4d,0x49,0x44,
    0x45,0x54,0x41,0x43,0x48,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0a,0x00,0x55,0x49,
    0x4d,0x32,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x02,0x01,0x00,0x0e,0x00,0x4e,
    0x45,0x54,0x57,0x4f,0x52,0x4b,0x4e,0x41,
    0x4d,0x45,0x46,0x4d,0x54,0x01,0x00,0x00,
    0x01,0x00,0x06,0x00,0x53,0x49,0x4d,0x4c,
    0x50,0x4d,0x01,0x00,0x01,0x01,0x00,0x0f,
    0x00,0x55,0x53,0x42,0x53,0x45,0x52,0x49,
    0x41,0x4c,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x01,0x01,0x00,0x0b,0x00,0x50,
    0x43,0x53,0x43,0x44,0x49,0x53,0x41,0x42,
    0x4c,0x45,0x01,0x00,0x00,0x01,0x00,0x0f,
    0x00,0x44,0x48,0x43,0x50,0x52,0x45,0x4c,
    0x41,0x59,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x00,0x01,0x00,0x0f,0x00,0x46,
    0x4c,0x4f,0x57,0x4e,0x4f,0x54,0x49,0x44,
    0x49,0x53,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0a,0x00,0x46,0x41,0x53,
    0x54,0x45,0x4e,0x55,0x4d,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0d,0x00,0x43,0x53,
    0x56,0x4f,0x49,0x43,0x45,0x52,0x45,0x4a,
    0x45,0x43,0x54,0x01,0x00,0x00,0x01,0x00,
    0x0f,0x00,0x49,0x50,0x43,0x48,0x41,0x4e,
    0x4e,0x45,0x4c,0x52,0x41,0x54,0x45,0x45,
    0x4e,0x01,0x00,0x00,0x01,0x00,0x0f,0x00,
    0x53,0x49,0x4e,0x47,0x4c,0x45,0x41,0x50,
    0x4e,0x53,0x57,0x49,0x54,0x43,0x48,0x01,
    0x00,0x00,0x01,0x00,0x08,0x00,0x44,0x47,
    0x45,0x4e,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0e,0x00,0x54,0x58,0x4f,
    0x4e,0x49,0x4e,0x44,0x49,0x43,0x41,0x54,
    0x49,0x4f,0x4e,0x01,0x00,0x00,0x01,0x00,
    0x0d,0x00,0x49,0x43,0x4d,0x50,0x49,0x4e,
    0x54,0x53,0x52,0x56,0x44,0x49,0x53,0x01,
    0x00,0x00,0x01,0x00,0x0f,0x00,0x53,0x55,
    0x42,0x4e,0x45,0x54,0x4d,0x53,0x4b,0x45,
    0x4e,0x41,0x42,0x4c,0x45,0x05,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x26,0x00,0x57,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x03,0x00,0x57,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x04,0x00,0x57,0x65,0x17,0x02,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x20,0x11,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x21,0xf9,0x01,0x00,0x1b,0x00,
    0x09,0x00,0x47,0x50,0x53,0x45,0x4e,0x41,
    0x42,0x4c,0x45,0x01,0x00,0x01,0x01,0x00,
    0x06,0x00,0x47,0x50,0x53,0x4c,0x50,0x4d,
    0x01,0x00,0x00,0x01,0x00,0x0d,0x00,0x47,
    0x50,0x49,0x4f,0x53,0x41,0x52,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x00,0x01,
    0x00,0x06,0x00,0x47,0x50,0x53,0x53,0x45,
    0x4c,0x01,0x00,0x00,0x01,0x00,0x09,0x00,
    0x47,0x50,0x53,0x52,0x45,0x46,0x4c,0x4f,
    0x43,0x01,0x00,0x00,0x01,0x00,0x0c,0x00,
    0x49,0x4d,0x53,0x57,0x49,0x54,0x43,0x48,
    0x48,0x49,0x44,0x45,0x01,0x00,0x00,0x01,
    0x00,0x08,0x00,0x49,0x4d,0x43,0x4f,0x4e,
    0x46,0x49,0x47,0x01,0x00,0x00,0x01,0x00,
    0x0a,0x00,0x49,0x50,0x56,0x36,0x45,0x4e,
    0x41,0x42,0x4c,0x45,0x01,0x00,0x01,0x01,
    0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,0x0d,0x00,0x53,0x49,0x4d,0x48,
    0x4f,0x54,0x53,0x57,0x41,0x50,0x44,0x49,
    0x53,0x01,0x00,0x00,0x01,0x00,0x0d,0x00,
    0x43,0x46,0x55,0x4e,0x50,0x45,0x52,0x53,
    0x49,0x53,0x54,0x45,0x4e,0x01,0x00,0x00,
    0x01,0x00,0x0b,0x00,0x51,0x4d,0x49,0x44,
    0x45,0x54,0x41,0x43,0x48,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0a,0x00,0x55,0x49,
    0x4d,0x32,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x02,0x01,0x00,0x0e,0x00,0x4e,
    0x45,0x54,0x57,0x4f,0x52,0x4b,0x4e,0x41,
    0x4d,0x45,0x46,0x4d,0x54,0x01,0x00,0x00,
    0x01,0x00,0x06,0x00,0x53,0x49,0x4d,0x4c,
    0x50,0x4d,0x01,0x00,0x01,0x01,0x00,0x0f,
    0x00,0x55,0x53,0x42,0x53,0x45,0x52,0x49,
    0x41,0x4c,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x01,0x01,0x00,0x0b,0x00,0x50,
    0x43,0x53,0x43,0x44,0x49,0x53,0x41,0x42,
    0x4c,0x45,0x01,0x00,0x00,0x01,0x00,0x0f,
    0x00,0x44,0x48,0x43,0x50,0x52,0x45,0x4c,
    0x41,0x59,0x45,0x4e,0x41,0x42,0x4c,0x45,
    0x01,0x00,0x00,0x01,0x00,0x0f,0x00,0x46,
    0x4c,0x4f,0x57,0x4e,0x4f,0x54,0x49,0x44,
    0x49,0x53,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0a,0x00,0x46,0x41,0x53,
    0x54,0x45,0x4e,0x55,0x4d,0x45,0x4e,0x01,
    0x00,0x00,0x01,0x00,0x0d,0x00,0x43,0x53,
    0x56,0x4f,0x49,0x43,0x45,0x52,0x45,0x4a,
    0x45,0x43,0x54,0x01,0x00,0x00,0x01,0x00,
    0x0f,0x00,0x49,0x50,0x43,0x48,0x41,0x4e,
    0x4e,0x45,0x4c,0x52,0x41,0x54,0x45,0x45,
    0x4e,0x01,0x00,0x00,0x01,0x00,0x0f,0x00,
    0x53,0x49,0x4e,0x47,0x4c,0x45,0x41,0x50,
    0x4e,0x53,0x57,0x49,0x54,0x43,0x48,0x01,
    0x00,0x00,0x01,0x00,0x08,0x00,0x44,0x47,
    0x45,0x4e,0x41,0x42,0x4c,0x45,0x01,0x00,
    0x00,0x01,0x00,0x0e,0x00,0x54,0x58,0x4f,
    0x4e,0x49,0x4e,0x44,0x49,0x43,0x41,0x54,
    0x49,0x4f,0x4e,0x01,0x00,0x00,0x01,0x00,
    0x0d,0x00,0x49,0x43,0x4d,0x50,0x49,0x4e,
    0x54,0x53,0x52,0x56,0x44,0x49,0x53,0x01,
    0x00,0x00,0x01,0x00,0x0f,0x00,0x53,0x55,
    0x42,0x4e,0x45,0x54,0x4d,0x53,0x4b,0x45,
    0x4e,0x41,0x42,0x4c,0x45,0x05,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x00},
    // receiving eQMI_DMS_SWI_GET_CUST_FEATURES
    {0x02,0x1a,0x00,0x57,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x05,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x37,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x33,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x31,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x05,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_CFG
    {0x02,0x22,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},
    // receiving eQMI_DMS_SWI_GET_HOST_DEV_INFO
    {0x02,0x06,0x00,0x6a,0x55,0x31,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0f,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x11,0x04,0x00,0x4d,0x47,0x39,0x30,0x12,
    0x01,0x00,0x34,0x14,0x0a,0x00,0x31,0x30,
    0x54,0x6b,0x68,0x69,0x32,0x61,0x34,0x67
    },
    // receiving eQMI_DMS_SWI_GET_HOST_DEV_INFO
    {0x02,0x4b,0x00,0x6a,0x55,0x3d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0f,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x11,0x06,0x00,0x4d,0x43,0x37,0x33,0x35,
    0x34,0x12,0x0b,0x00,0x30,0x35,0x2e,0x30,
    0x35,0x2e,0x35,0x38,0x2e,0x30,0x30,0x14,
    0x0a,0x00,0x31,0x30,0x54,0x6b,0x68,0x69,
    0x32,0x61,0x34,0x67},
    // receiving eQMI_DMS_SWI_GET_HOST_DEV_INFO
    {0x02,0x06,0x00,0x6a,0x55,0x31,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0f,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x11,0x04,0x00,0x4d,0x47,0x39,0x30,0x12,
    0x01,0x00,0x34,0x14,0x0a,0x00,0x31,0x30,
    0x54,0x6b,0x68,0x69,0x32,0x61,0x34,0x67
    },
    // receiving eQMI_DMS_SWI_GET_HOST_DEV_INFO
    {0x02,0x2c,0x00,0x6a,0x55,0x31,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0f,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x11,0x04,0x00,0x4d,0x47,0x39,0x30,0x12,
    0x01,0x00,0x34,0x14,0x0a,0x00,0x31,0x30,
    0x54,0x6b,0x68,0x69,0x32,0x61,0x34,0x67
    },
    // receiving eQMI_DMS_SWI_GET_OS_INFO
    {0x02,0x07,0x00,0x6c,0x55,0x0e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x01,0x11,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_OS_INFO
    {0x02,0x4d,0x00,0x6c,0x55,0x1e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x05,
    0x00,0x4c,0x69,0x6e,0x75,0x78,0x11,0x0c,
    0x00,0x55,0x62,0x75,0x6e,0x74,0x75,0x20,
    0x31,0x34,0x2e,0x30,0x34},
    // receiving eQMI_DMS_SWI_GET_OS_INFO
    {0x02,0x07,0x00,0x6c,0x55,0x0e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x01,0x11,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_OS_INFO
    {0x02,0x2d,0x00,0x6c,0x55,0x0e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x01,0x11,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_CRASH_ACTION
    {0x02,0x08,0x00,0x68,0x55,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_SWI_GET_CRASH_ACTION
    {0x02,0x27,0x00,0x68,0x55,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_SWI_GET_CRASH_ACTION
    {0x02,0x08,0x00,0x68,0x55,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_SWI_GET_CRASH_ACTION
    {0x02,0x1b,0x00,0x68,0x55,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},
    // receiving eQMI_DMS_SWI_GET_FIRMWARE_CURR
    {0x02,0x09,0x00,0x63,0x55,0x4f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x45,
    0x00,0x02,0x00,0x30,0x30,0x30,0x2e,0x30,
    0x30,0x30,0x5f,0x30,0x30,0x30,0x00,0x00,
    0x00,0x00,0x00,0x10,0x30,0x31,0x2e,0x30,
    0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
    0x4e,0x4f,0x4e,0x45,0x01,0x30,0x30,0x30,
    0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30,
    0x00,0x00,0x00,0x00,0x00,0x10,0x30,0x31,
    0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,0x30,
    0x30,0x5f,0x4e,0x4f,0x4e,0x45},
    // receiving eQMI_DMS_SWI_GET_FIRMWARE_CURR
    {0x02,0x09,0x00,0x63,0x55,0x4f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x45,
    0x00,0x02,0x00,0x30,0x30,0x30,0x2e,0x30,
    0x30,0x30,0x5f,0x30,0x30,0x30,0x00,0x00,
    0x00,0x00,0x00,0x10,0x30,0x31,0x2e,0x30,
    0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
    0x4e,0x4f,0x4e,0x45,0x01,0x30,0x30,0x30,
    0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30,
    0x00,0x00,0x00,0x00,0x00,0x10,0x30,0x31,
    0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,0x30,
    0x30,0x5f,0x4e,0x4f,0x4e,0x45},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x0a,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x00,0x01,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x53,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x0a,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x0b,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x0c,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_EVENT
    {0x02,0x33,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00},
    // receiving eQMI_DMS_GET_MODEL_ID
    {0x02,0x0b,0x00,0x22,0x00,0x10,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x06,
    0x00,0x45,0x4d,0x37,0x35,0x36,0x35},
    // receiving eQMI_DMS_GET_MODEL_ID
    {0x02,0x0d,0x00,0x22,0x00,0x10,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x06,
    0x00,0x45,0x4d,0x37,0x35,0x36,0x35},
    // receiving eQMI_DMS_GET_CWE_SPKGS_INFO
    {0x02,0x0c,0x00,0x56,0x55,0x57,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x06,
    0x00,0x45,0x4d,0x37,0x35,0x36,0x35,0x11,
    0x14,0x00,0x53,0x57,0x49,0x39,0x58,0x35,
    0x30,0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,
    0x2e,0x30,0x33,0x2e,0x30,0x30,0x12,0x14,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x15,0x01,0x00,
    0x30,0x17,0x04,0x00,0x4e,0x4f,0x4e,0x45,
    0x18,0x0b,0x00,0x30,0x30,0x30,0x2e,0x30,
    0x30,0x30,0x5f,0x30,0x30,0x30},
    // receiving eQMI_DMS_GET_CWE_SPKGS_INFO
    {0x02,0x0e,0x00,0x56,0x55,0x57,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x06,
    0x00,0x45,0x4d,0x37,0x35,0x36,0x35,0x11,
    0x14,0x00,0x53,0x57,0x49,0x39,0x58,0x35,
    0x30,0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,
    0x2e,0x30,0x33,0x2e,0x30,0x30,0x12,0x14,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x15,0x01,0x00,
    0x30,0x17,0x04,0x00,0x4e,0x4f,0x4e,0x45,
    0x18,0x0b,0x00,0x30,0x30,0x30,0x2e,0x30,
    0x30,0x30,0x5f,0x30,0x30,0x30},
    // receiving eQMI_DMS_GET_REV_ID
    {0x02,0x0e,0x00,0x23,0x00,0x41,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x37,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x20,0x35,0x39,
    0x39,0x36,0x38,0x61,0x20,0x6a,0x65,0x6e,
    0x6b,0x69,0x6e,0x73,0x20,0x32,0x30,0x31,
    0x38,0x2f,0x30,0x37,0x2f,0x32,0x33,0x20,
    0x32,0x32,0x3a,0x30,0x31,0x3a,0x35,0x37
    },
    // receiving eQMI_DMS_GET_REV_ID
    {0x02,0x0f,0x00,0x23,0x00,0x41,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x37,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x20,0x35,0x39,
    0x39,0x36,0x38,0x61,0x20,0x6a,0x65,0x6e,
    0x6b,0x69,0x6e,0x73,0x20,0x32,0x30,0x31,
    0x38,0x2f,0x30,0x37,0x2f,0x32,0x33,0x20,
    0x32,0x32,0x3a,0x30,0x31,0x3a,0x35,0x37
    },
    // receiving eQMI_DMS_GET_REV_ID
    {0x02,0x10,0x00,0x23,0x00,0x41,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x37,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x20,0x35,0x39,
    0x39,0x36,0x38,0x61,0x20,0x6a,0x65,0x6e,
    0x6b,0x69,0x6e,0x73,0x20,0x32,0x30,0x31,
    0x38,0x2f,0x30,0x37,0x2f,0x32,0x33,0x20,
    0x32,0x32,0x3a,0x30,0x31,0x3a,0x35,0x37
    },
    // receiving eQMI_DMS_GET_REV_ID
    {0x02,0x11,0x00,0x23,0x00,0x41,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x37,
    0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,0x20,0x35,0x39,
    0x39,0x36,0x38,0x61,0x20,0x6a,0x65,0x6e,
    0x6b,0x69,0x6e,0x73,0x20,0x32,0x30,0x31,
    0x38,0x2f,0x30,0x37,0x2f,0x32,0x33,0x20,
    0x32,0x32,0x3a,0x30,0x31,0x3a,0x35,0x37
    },
    // receiving eQMI_DMS_GET_IDS
    {0x02,0x10,0x00,0x25,0x00,0x1d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x30,0x11,0x0f,0x00,0x30,0x30,0x31,
    0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,
    0x39,0x39,0x39,0x39},
    // receiving eQMI_DMS_GET_IDS
    {0x02,0x50,0x00,0x25,0x00,0x1d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x30,0x11,0x0f,0x00,0x30,0x30,0x31,
    0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,
    0x39,0x39,0x39,0x39},
    // receiving eQMI_DMS_GET_IDS
    {0x02,0x12,0x00,0x25,0x00,0x1d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x30,0x11,0x0f,0x00,0x30,0x30,0x31,
    0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,
    0x39,0x39,0x39,0x39},
    // receiving eQMI_DMS_GET_IDS
    {0x02,0x30,0x00,0x25,0x00,0x1d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x01,
    0x00,0x30,0x11,0x0f,0x00,0x30,0x30,0x31,
    0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,
    0x39,0x39,0x39,0x39},
    // receiving eQMI_DMS_GET_CURRENT_PRL_INFO
    {0x02,0x11,0x00,0x53,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00},
    // receiving eQMI_DMS_GET_CURRENT_PRL_INFO
    {0x02,0x13,0x00,0x53,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00},
    // receiving eQMI_DMS_GET_TIME
    {0x02,0x12,0x00,0x2f,0x00,0x28,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x53,0xd5,0x48,0x08,0x00,0x00,0x00,
    0x00,0x10,0x08,0x00,0xa9,0x0a,0x5b,0x0a,
    0x00,0x00,0x00,0x00,0x11,0x08,0x00,0xa9,
    0x0a,0x5b,0x0a,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_GET_TIME
    {0x02,0x14,0x00,0x2f,0x00,0x28,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x05,0x13,0x49,0x08,0x00,0x00,0x00,
    0x00,0x10,0x08,0x00,0xc7,0x57,0x5b,0x0a,
    0x00,0x00,0x00,0x00,0x11,0x08,0x00,0xc7,
    0x57,0x5b,0x0a,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_GET_MSISDN
    {0x02,0x13,0x00,0x24,0x00,0x0a,0x00,0x02,
    0x04,0x00,0x01,0x00,0x25,0x00,0x01,0x00,
    0x00},
    // receiving eQMI_DMS_GET_MSISDN
    {0x02,0x15,0x00,0x24,0x00,0x0a,0x00,0x02,
    0x04,0x00,0x01,0x00,0x25,0x00,0x01,0x00,
    0x00},
    // receiving eQMI_DMS_GET_HARDWARE_REV
    {0x02,0x14,0x00,0x2c,0x00,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},
    // receiving eQMI_DMS_GET_HARDWARE_REV
    {0x02,0x51,0x00,0x2c,0x00,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},
    // receiving eQMI_DMS_GET_HARDWARE_REV
    {0x02,0x16,0x00,0x2c,0x00,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},
    // receiving eQMI_DMS_GET_HARDWARE_REV
    {0x02,0x31,0x00,0x2c,0x00,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},
    // receiving eQMI_DMS_GET_FSN
    {0x02,0x15,0x00,0x67,0x55,0x1a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x10,
    0x00,0x55,0x46,0x37,0x31,0x32,0x34,0x37,
    0x30,0x30,0x33,0x30,0x31,0x30,0x33,0x31,
    0x36},
    // receiving eQMI_DMS_GET_FSN
    {0x02,0x17,0x00,0x67,0x55,0x1a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x10,
    0x00,0x55,0x46,0x37,0x31,0x32,0x34,0x37,
    0x30,0x30,0x33,0x30,0x31,0x30,0x33,0x31,
    0x36},
    // receiving eQMI_DMS_GET_CAPS
    {0x02,0x16,0x00,0x20,0x00,0x8a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0d,
    0x00,0x80,0xd1,0xf0,0x08,0x00,0x46,0xc3,
    0x23,0x04,0x02,0x02,0x05,0x08,0x10,0x04,
    0x00,0x04,0x00,0x00,0x00,0x11,0x08,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x12,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x13,0x0c,0x00,0x01,0x01,
    0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x14,0x02,0x00,0x01,0x01,0x15,
    0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x16,0x06,0x00,0x01,0x01,
    0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x01,
    0x00,0x00,0x00,0x00,0x18,0x01,0x00,0x01,
    0x19,0x09,0x00,0x01,0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1a,0x0d,0x00,0x01,
    0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1b,0x02,0x00,0xff,
    0xff},
    // receiving eQMI_DMS_GET_CAPS
    {0x02,0x52,0x00,0x20,0x00,0x8a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0d,
    0x00,0x80,0xd1,0xf0,0x08,0x00,0x46,0xc3,
    0x23,0x04,0x02,0x02,0x05,0x08,0x10,0x04,
    0x00,0x04,0x00,0x00,0x00,0x11,0x08,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x12,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x13,0x0c,0x00,0x01,0x01,
    0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x14,0x02,0x00,0x01,0x01,0x15,
    0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x16,0x06,0x00,0x01,0x01,
    0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x01,
    0x00,0x00,0x00,0x00,0x18,0x01,0x00,0x01,
    0x19,0x09,0x00,0x01,0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1a,0x0d,0x00,0x01,
    0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1b,0x02,0x00,0xff,
    0xff},
    // receiving eQMI_DMS_GET_CAPS
    {0x02,0x18,0x00,0x20,0x00,0x8a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0d,
    0x00,0x80,0xd1,0xf0,0x08,0x00,0x46,0xc3,
    0x23,0x04,0x02,0x02,0x05,0x08,0x10,0x04,
    0x00,0x04,0x00,0x00,0x00,0x11,0x08,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x12,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x13,0x0c,0x00,0x01,0x01,
    0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x14,0x02,0x00,0x01,0x01,0x15,
    0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x16,0x06,0x00,0x01,0x01,
    0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x01,
    0x00,0x00,0x00,0x00,0x18,0x01,0x00,0x01,
    0x19,0x09,0x00,0x01,0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1a,0x0d,0x00,0x01,
    0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1b,0x02,0x00,0xff,
    0xff},
    // receiving eQMI_DMS_GET_CAPS
    {0x02,0x32,0x00,0x20,0x00,0x8a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0d,
    0x00,0x80,0xd1,0xf0,0x08,0x00,0x46,0xc3,
    0x23,0x04,0x02,0x02,0x05,0x08,0x10,0x04,
    0x00,0x04,0x00,0x00,0x00,0x11,0x08,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x12,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x13,0x0c,0x00,0x01,0x01,
    0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x14,0x02,0x00,0x01,0x01,0x15,
    0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x16,0x06,0x00,0x01,0x01,
    0x00,0x00,0x00,0x00,0x17,0x05,0x00,0x01,
    0x00,0x00,0x00,0x00,0x18,0x01,0x00,0x01,
    0x19,0x09,0x00,0x01,0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1a,0x0d,0x00,0x01,
    0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1b,0x02,0x00,0xff,
    0xff},
    // receiving eQMI_DMS_SET_OPERATING_MODE
    {0x02,0x17,0x00,0x2e,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x3c,0x00},
    // receiving eQMI_DMS_SET_OPERATING_MODE
    {0x02,0x55,0x00,0x2e,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_OPERATING_MODE
    {0x02,0x19,0x00,0x2e,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x3c,0x00},
    // receiving eQMI_DMS_GET_BAND_CAPS
    {0x02,0x18,0x00,0x45,0x00,0x5b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x00,0x00,0xc0,0x0f,0x00,0x00,0x06,
    0x10,0x10,0x08,0x00,0xdf,0x19,0x0e,0xba,
    0x00,0x27,0x00,0x00,0x11,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,
    0x30,0x00,0x17,0x00,0x01,0x00,0x02,0x00,
    0x03,0x00,0x04,0x00,0x05,0x00,0x07,0x00,
    0x08,0x00,0x09,0x00,0x0c,0x00,0x0d,0x00,
    0x12,0x00,0x13,0x00,0x14,0x00,0x1a,0x00,
    0x1c,0x00,0x1d,0x00,0x1e,0x00,0x20,0x00,
    0x29,0x00,0x2a,0x00,0x2b,0x00,0x2e,0x00,
    0x42,0x00},
    // receiving eQMI_DMS_GET_BAND_CAPS
    {0x02,0x29,0x00,0x45,0x00,0x5b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x00,0x00,0xc0,0x0f,0x00,0x00,0x06,
    0x10,0x10,0x08,0x00,0xdf,0x19,0x0e,0xba,
    0x00,0x27,0x00,0x00,0x11,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,
    0x30,0x00,0x17,0x00,0x01,0x00,0x02,0x00,
    0x03,0x00,0x04,0x00,0x05,0x00,0x07,0x00,
    0x08,0x00,0x09,0x00,0x0c,0x00,0x0d,0x00,
    0x12,0x00,0x13,0x00,0x14,0x00,0x1a,0x00,
    0x1c,0x00,0x1d,0x00,0x1e,0x00,0x20,0x00,
    0x29,0x00,0x2a,0x00,0x2b,0x00,0x2e,0x00,
    0x42,0x00},
    // receiving eQMI_DMS_GET_BAND_CAPS
    {0x02,0x1a,0x00,0x45,0x00,0x5b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x00,0x00,0xc0,0x0f,0x00,0x00,0x06,
    0x10,0x10,0x08,0x00,0xdf,0x19,0x0e,0xba,
    0x00,0x27,0x00,0x00,0x11,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,
    0x30,0x00,0x17,0x00,0x01,0x00,0x02,0x00,
    0x03,0x00,0x04,0x00,0x05,0x00,0x07,0x00,
    0x08,0x00,0x09,0x00,0x0c,0x00,0x0d,0x00,
    0x12,0x00,0x13,0x00,0x14,0x00,0x1a,0x00,
    0x1c,0x00,0x1d,0x00,0x1e,0x00,0x20,0x00,
    0x29,0x00,0x2a,0x00,0x2b,0x00,0x2e,0x00,
    0x42,0x00},
    // receiving eQMI_DMS_GET_BAND_CAPS
    {0x02,0x1d,0x00,0x45,0x00,0x5b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x00,0x00,0xc0,0x0f,0x00,0x00,0x06,
    0x10,0x10,0x08,0x00,0xdf,0x19,0x0e,0xba,
    0x00,0x27,0x00,0x00,0x11,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,
    0x30,0x00,0x17,0x00,0x01,0x00,0x02,0x00,
    0x03,0x00,0x04,0x00,0x05,0x00,0x07,0x00,
    0x08,0x00,0x09,0x00,0x0c,0x00,0x0d,0x00,
    0x12,0x00,0x13,0x00,0x14,0x00,0x1a,0x00,
    0x1c,0x00,0x1d,0x00,0x1e,0x00,0x20,0x00,
    0x29,0x00,0x2a,0x00,0x2b,0x00,0x2e,0x00,
    0x42,0x00},
    // receiving eQMI_DMS_GET_MANUFACTURER
    {0x02,0x1c,0x00,0x21,0x00,0x27,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x1d,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,
    0x6f,0x72,0x61,0x74,0x65,0x64},
    // receiving eQMI_DMS_GET_MANUFACTURER
    {0x02,0x3a,0x00,0x21,0x00,0x27,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x1d,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,
    0x6f,0x72,0x61,0x74,0x65,0x64},
    // receiving eQMI_DMS_GET_MANUFACTURER
    {0x02,0x28,0x00,0x21,0x00,0x27,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x1d,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,
    0x6f,0x72,0x61,0x74,0x65,0x64},
    // receiving eQMI_DMS_GET_MANUFACTURER
    {0x02,0x25,0x00,0x21,0x00,0x27,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x1d,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,
    0x6f,0x72,0x61,0x74,0x65,0x64},
    // receiving eQMI_DMS_UIM_GET_ICCID
    {0x02,0x1e,0x00,0x3c,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_GET_ICCID
    {0x02,0x2a,0x00,0x3c,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_GET_FIRMWARE_PREF
    {0x02,0x1f,0x00,0x47,0x00,0x55,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x4b,
    0x00,0x02,0x00,0x30,0x30,0x32,0x2e,0x30,
    0x30,0x35,0x5f,0x30,0x30,0x30,0x00,0x00,
    0x00,0x00,0x00,0x13,0x30,0x31,0x2e,0x30,
    0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
    0x47,0x45,0x4e,0x45,0x52,0x49,0x43,0x01,
    0x30,0x30,0x32,0x2e,0x30,0x30,0x35,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x33,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43},
    // receiving eQMI_DMS_GET_FIRMWARE_PREF
    {0x02,0x2d,0x00,0x47,0x00,0x55,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x4b,
    0x00,0x02,0x00,0x30,0x30,0x32,0x2e,0x30,
    0x30,0x35,0x5f,0x30,0x30,0x30,0x00,0x00,
    0x00,0x00,0x00,0x13,0x30,0x31,0x2e,0x30,
    0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
    0x47,0x45,0x4e,0x45,0x52,0x49,0x43,0x01,
    0x30,0x30,0x32,0x2e,0x30,0x30,0x35,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x33,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43},
    // receiving eQMI_DMS_LIST_FIRMWARE
    {0x02,0x20,0x00,0x49,0x00,0xa5,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x9b,
    0x00,0x02,0x00,0x04,0x03,0x03,0x01,0x00,
    0x3f,0x5f,0x3f,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0d,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x32,0x2e,0x30,0x30,0x5f,0x3f,0x02,0x00,
    0x30,0x30,0x31,0x2e,0x30,0x33,0x36,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x36,0x2e,0x30,
    0x34,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43,0x03,0x00,0x30,0x30,
    0x32,0x2e,0x30,0x30,0x35,0x5f,0x30,0x30,
    0x30,0x00,0x00,0x00,0x00,0x00,0x13,0x30,
    0x31,0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,
    0x30,0x30,0x5f,0x47,0x45,0x4e,0x45,0x52,
    0x49,0x43,0x01,0x32,0xff,0x01,0xff,0xff,
    0x30,0x30,0x32,0x2e,0x30,0x30,0x35,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x33,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43},
    // receiving eQMI_DMS_LIST_FIRMWARE
    {0x02,0x2e,0x00,0x49,0x00,0xa5,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x9b,
    0x00,0x02,0x00,0x04,0x03,0x03,0x01,0x00,
    0x3f,0x5f,0x3f,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0d,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x32,0x2e,0x30,0x30,0x5f,0x3f,0x02,0x00,
    0x30,0x30,0x31,0x2e,0x30,0x33,0x36,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x36,0x2e,0x30,
    0x34,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43,0x03,0x00,0x30,0x30,
    0x32,0x2e,0x30,0x30,0x35,0x5f,0x30,0x30,
    0x30,0x00,0x00,0x00,0x00,0x00,0x13,0x30,
    0x31,0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,
    0x30,0x30,0x5f,0x47,0x45,0x4e,0x45,0x52,
    0x49,0x43,0x01,0x32,0xff,0x01,0xff,0xff,
    0x30,0x30,0x32,0x2e,0x30,0x30,0x35,0x5f,
    0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,
    0x13,0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,
    0x33,0x2e,0x30,0x30,0x5f,0x47,0x45,0x4e,
    0x45,0x52,0x49,0x43},
    // receiving eQMI_DMS_GET_ACTIVATED_STATE
    {0x02,0x21,0x00,0x31,0x00,0x0c,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00,0x01,0x02,
    0x00,0x26,0x06},
    // receiving eQMI_DMS_GET_ACTIVATED_STATE
    {0x02,0x30,0x00,0x31,0x00,0x0c,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00,0x01,0x02,
    0x00,0x6a,0x06},
    // receiving eQMI_DMS_SWI_GET_RESET_INFO
    {0x02,0x23,0x00,0x80,0x55,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x00,0x03,0x00},
    // receiving eQMI_DMS_SWI_GET_RESET_INFO
    {0x02,0x38,0x00,0x80,0x55,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x00,0x03,0x00},
    // receiving eQMI_DMS_SWI_GET_FW_UPDATE_STAT
    {0x02,0x24,0x00,0x64,0x55,0x0e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
    0x00,0xff,0xff,0xff,0xff},
    // receiving eQMI_DMS_SWI_GET_FW_UPDATE_STAT
    {0x02,0x39,0x00,0x64,0x55,0x0e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
    0x00,0xff,0xff,0xff,0xff},
    // receiving eQMI_DMS_UIM_GET_CK_STATUS
    {0x02,0x27,0x00,0x40,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_GET_CK_STATUS
    {0x02,0x40,0x00,0x40,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_GET_PIN_STATUS
    {0x02,0x28,0x00,0x2b,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_GET_PIN_STATUS
    {0x02,0x41,0x00,0x2b,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_READ_ERI_FILE
    {0x02,0x29,0x00,0x39,0x00,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x00,0x00,0x00},
    // receiving eQMI_DMS_READ_ERI_FILE
    {0x02,0x47,0x00,0x39,0x00,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x00,0x00,0x00},
    // receiving eQMI_DMS_UIM_GET_STATE
    {0x02,0x2a,0x00,0x44,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_GET_STATE
    {0x02,0x48,0x00,0x44,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_GET_CRASH_INFO
    {0x02,0x2b,0x00,0x65,0x55,0xc4,0x06,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,0x10,0xb6,0x06,0x02,0x83,0x24,
    0x86,0x11,0x00,0x00,0x00,0x00,0x0f,0x00,
    0x0f,0x41,0x50,0x50,0x53,0x20,0x66,0x6f,
    0x72,0x63,0x65,0x20,0x73,0x74,0x6f,0x70,
    0xc4,0x03,0xc3,0x03,0x0a,0x53,0x72,0x63,
    0x3a,0x20,0x20,0x46,0x61,0x74,0x61,0x6c,
    0x45,0x72,0x72,0x6f,0x72,0x0a,0x46,0x69,
    0x6c,0x65,0x3a,0x20,0x73,0x79,0x73,0x5f,
    0x6d,0x5f,0x73,0x6d,0x73,0x6d,0x5f,0x6d,
    0x70,0x73,0x73,0x2e,0x63,0x0a,0x4c,0x69,
    0x6e,0x65,0x3a,0x20,0x30,0x30,0x30,0x30,
    0x30,0x31,0x31,0x44,0x0a,0x53,0x74,0x72,
    0x3a,0x20,0x20,0x41,0x50,0x50,0x53,0x20,
    0x66,0x6f,0x72,0x63,0x65,0x20,0x73,0x74,
    0x6f,0x70,0x0a,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x0a,0x50,
    0x72,0x63,0x3a,0x20,0x20,0x4d,0x50,0x53,
    0x53,0x0a,0x54,0x61,0x73,0x6b,0x3a,0x20,
    0x41,0x4d,0x53,0x53,0x0a,0x54,0x69,0x6d,
    0x65,0x3a,0x20,0x30,0x30,0x36,0x46,0x38,
    0x37,0x38,0x42,0x0a,0x20,0x52,0x30,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x31,0x3a,0x20,0x43,
    0x30,0x30,0x34,0x36,0x31,0x33,0x30,0x20,
    0x20,0x52,0x32,0x3a,0x20,0x45,0x42,0x31,
    0x30,0x32,0x30,0x30,0x30,0x20,0x20,0x52,
    0x33,0x3a,0x20,0x44,0x38,0x35,0x32,0x45,
    0x42,0x36,0x30,0x20,0x20,0x52,0x34,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x46,
    0x45,0x0a,0x20,0x52,0x35,0x3a,0x20,0x43,
    0x33,0x36,0x41,0x38,0x41,0x36,0x34,0x20,
    0x20,0x52,0x36,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x52,
    0x37,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x31,0x20,0x20,0x52,0x38,0x3a,
    0x20,0x43,0x30,0x30,0x41,0x38,0x37,0x37,
    0x30,0x20,0x20,0x52,0x39,0x3a,0x20,0x30,
    0x31,0x38,0x46,0x30,0x30,0x34,0x35,0x0a,
    0x52,0x31,0x30,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x52,0x31,
    0x31,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x31,0x32,0x3a,
    0x20,0x43,0x35,0x30,0x33,0x38,0x30,0x30,
    0x30,0x20,0x52,0x31,0x33,0x3a,0x20,0x44,
    0x38,0x35,0x32,0x45,0x42,0x36,0x30,0x20,
    0x52,0x31,0x34,0x3a,0x20,0x46,0x46,0x46,
    0x46,0x30,0x30,0x30,0x34,0x0a,0x52,0x31,
    0x35,0x3a,0x20,0x43,0x30,0x30,0x31,0x34,
    0x38,0x35,0x30,0x20,0x52,0x31,0x36,0x3a,
    0x20,0x43,0x35,0x30,0x34,0x39,0x31,0x42,
    0x32,0x20,0x52,0x31,0x37,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x31,0x38,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x52,0x31,
    0x39,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x52,0x32,0x30,0x3a,
    0x20,0x43,0x30,0x30,0x34,0x36,0x30,0x31,
    0x43,0x20,0x52,0x32,0x31,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x35,0x41,0x20,
    0x52,0x32,0x32,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x37,0x46,0x20,0x52,0x32,
    0x33,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x32,0x34,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0a,0x52,0x32,0x35,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x32,0x36,0x3a,0x20,0x44,0x38,0x35,
    0x32,0x45,0x42,0x36,0x30,0x20,0x52,0x32,
    0x37,0x3a,0x20,0x43,0x31,0x35,0x30,0x34,
    0x45,0x35,0x43,0x20,0x52,0x32,0x38,0x3a,
    0x20,0x44,0x38,0x34,0x45,0x38,0x34,0x33,
    0x30,0x20,0x53,0x50,0x3a,0x20,0x20,0x44,
    0x38,0x35,0x32,0x45,0x35,0x30,0x38,0x0a,
    0x46,0x50,0x3a,0x20,0x20,0x44,0x38,0x35,
    0x32,0x45,0x35,0x31,0x30,0x20,0x4c,0x52,
    0x3a,0x20,0x20,0x43,0x30,0x30,0x39,0x46,
    0x32,0x37,0x43,0x0a,0x50,0x43,0x3a,0x20,
    0x35,0x33,0x36,0x30,0x31,0x38,0x38,0x32,
    0x0a,0x43,0x50,0x53,0x52,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x0a,
    0x4d,0x6f,0x64,0x3a,0x20,0x55,0x6e,0x6b,
    0x6e,0x6f,0x77,0x6e,0x0a,0x43,0x74,0x72,
    0x3a,0x20,0x41,0x52,0x4d,0x2c,0x20,0x49,
    0x52,0x51,0x20,0x64,0x69,0x73,0x2c,0x46,
    0x49,0x51,0x20,0x64,0x69,0x73,0x0a,0x54,
    0x4f,0x53,0x0a,0x44,0x38,0x35,0x32,0x45,
    0x35,0x33,0x30,0x20,0x35,0x33,0x36,0x30,
    0x31,0x39,0x42,0x36,0x20,0x43,0x30,0x37,
    0x38,0x33,0x45,0x34,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x35,0x41,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x32,0x20,
    0x43,0x34,0x38,0x37,0x44,0x45,0x32,0x38,
    0x20,0x43,0x35,0x30,0x34,0x39,0x31,0x42,
    0x32,0x0a,0x43,0x35,0x30,0x33,0x46,0x31,
    0x44,0x38,0x20,0x44,0x38,0x35,0x32,0x45,
    0x35,0x34,0x30,0x20,0x35,0x33,0x36,0x30,
    0x31,0x38,0x38,0x32,0x20,0x43,0x35,0x30,
    0x34,0x39,0x31,0x39,0x34,0x20,0x43,0x35,
    0x30,0x33,0x46,0x31,0x44,0x38,0x20,0x44,
    0x38,0x35,0x32,0x45,0x35,0x36,0x30,0x20,
    0x35,0x33,0x31,0x31,0x44,0x34,0x33,0x41,
    0x0a,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x31,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x43,0x34,0x38,0x37,
    0x44,0x45,0x32,0x38,0x20,0x43,0x32,0x30,
    0x42,0x43,0x35,0x45,0x34,0x20,0x46,0x46,
    0x46,0x46,0x46,0x46,0x46,0x46,0x20,0x44,
    0x38,0x35,0x32,0x45,0x36,0x30,0x38,0x0a,
    0x35,0x32,0x34,0x32,0x32,0x36,0x37,0x36,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x36,0x33,0x31,0x33,0x34,
    0x30,0x44,0x20,0x43,0x32,0x30,0x42,0x43,
    0x35,0x43,0x38,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x20,0x43,0x32,0x30,
    0x42,0x43,0x35,0x45,0x34,0x20,0x46,0x46,
    0x46,0x46,0x46,0x46,0x46,0x46,0x0a,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x43,0x34,0x38,0x37,0x44,0x45,0x32,0x38,
    0x20,0x43,0x30,0x37,0x38,0x33,0x45,0x34,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x35,0x41,0x0a,0x42,0x4f,0x53,0x0a,0x89,
    0x31,0xa1,0xa1,0x00,0x00,0x00,0x00,0x11,
    0x00,0x11,0x52,0x65,0x63,0x75,0x72,0x73,
    0x69,0x76,0x65,0x20,0x66,0x61,0x75,0x6c,
    0x74,0x21,0x0a,0xb5,0x02,0xb4,0x02,0x0a,
    0x53,0x72,0x63,0x3a,0x20,0x20,0x46,0x61,
    0x74,0x61,0x6c,0x45,0x72,0x72,0x6f,0x72,
    0x0a,0x53,0x74,0x72,0x3a,0x20,0x20,0x52,
    0x65,0x63,0x75,0x72,0x73,0x69,0x76,0x65,
    0x20,0x66,0x61,0x75,0x6c,0x74,0x21,0x0a,
    0x0a,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x0a,0x50,0x72,0x63,
    0x3a,0x20,0x20,0x41,0x50,0x53,0x53,0x0a,
    0x54,0x61,0x73,0x6b,0x3a,0x20,0x43,0x35,
    0x31,0x43,0x32,0x41,0x30,0x30,0x0a,0x54,
    0x69,0x6d,0x65,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x0a,0x20,0x52,
    0x30,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x20,0x52,0x31,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x32,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x20,0x52,0x33,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x52,
    0x34,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x20,0x52,0x35,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x36,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x20,0x52,0x37,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x52,
    0x38,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x20,0x52,0x39,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0a,0x52,0x31,0x30,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x31,0x31,0x3a,0x20,0x43,0x35,0x32,
    0x35,0x37,0x45,0x42,0x43,0x20,0x52,0x31,
    0x32,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x31,0x33,0x3a,
    0x20,0x43,0x35,0x32,0x35,0x37,0x45,0x39,
    0x30,0x20,0x52,0x31,0x34,0x3a,0x20,0x43,
    0x30,0x30,0x31,0x31,0x44,0x43,0x34,0x0a,
    0x50,0x43,0x3a,0x20,0x43,0x30,0x30,0x31,
    0x34,0x38,0x33,0x43,0x0a,0x43,0x50,0x53,
    0x52,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x4d,0x6f,0x64,0x3a,
    0x20,0x55,0x6e,0x6b,0x6e,0x6f,0x77,0x6e,
    0x0a,0x43,0x74,0x72,0x3a,0x20,0x41,0x52,
    0x4d,0x2c,0x20,0x49,0x52,0x51,0x20,0x64,
    0x69,0x73,0x2c,0x46,0x49,0x51,0x20,0x64,
    0x69,0x73,0x0a,0x54,0x4f,0x53,0x0a,0x43,
    0x30,0x30,0x31,0x31,0x44,0x43,0x34,0x20,
    0x36,0x38,0x31,0x41,0x34,0x33,0x31,0x46,
    0x20,0x43,0x30,0x30,0x31,0x46,0x31,0x32,
    0x34,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x32,0x33,0x20,0x43,0x30,0x37,0x42,0x41,
    0x42,0x45,0x38,0x20,0x43,0x30,0x45,0x30,
    0x45,0x34,0x46,0x38,0x20,0x43,0x30,0x41,
    0x31,0x37,0x33,0x30,0x34,0x0a,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x30,
    0x30,0x31,0x31,0x39,0x30,0x44,0x38,0x20,
    0x43,0x35,0x32,0x35,0x37,0x46,0x38,0x38,
    0x20,0x43,0x30,0x30,0x31,0x46,0x31,0x44,
    0x34,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x32,0x33,0x20,0x43,0x30,0x41,0x31,
    0x32,0x45,0x42,0x34,0x0a,0x43,0x30,0x37,
    0x42,0x41,0x42,0x45,0x38,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x43,
    0x31,0x30,0x36,0x34,0x32,0x46,0x38,0x20,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x20,0x43,0x30,0x37,0x44,0x38,0x33,0x39,
    0x38,0x20,0x43,0x35,0x32,0x35,0x37,0x46,
    0x38,0x38,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x31,0x0a,0x43,0x30,0x45,0x39,
    0x30,0x46,0x42,0x43,0x20,0x43,0x36,0x36,
    0x42,0x39,0x36,0x30,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x43,
    0x30,0x31,0x33,0x41,0x37,0x45,0x30,0x20,
    0x43,0x35,0x32,0x35,0x37,0x46,0x38,0x38,
    0x20,0x30,0x30,0x31,0x31,0x39,0x30,0x44,
    0x38,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x32,0x0a,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x32,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x32,0x20,0x43,0x35,0x37,
    0x36,0x46,0x32,0x34,0x30,0x20,0x30,0x30,
    0x31,0x31,0x39,0x30,0x44,0x38,0x0a,0x42,
    0x4f,0x53,0x0a},
    // receiving eQMI_DMS_SWI_GET_CRASH_INFO
    {0x02,0x49,0x00,0x65,0x55,0xc4,0x06,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,0x10,0xb6,0x06,0x02,0x83,0x24,
    0x86,0x11,0x00,0x00,0x00,0x00,0x0f,0x00,
    0x0f,0x41,0x50,0x50,0x53,0x20,0x66,0x6f,
    0x72,0x63,0x65,0x20,0x73,0x74,0x6f,0x70,
    0xc4,0x03,0xc3,0x03,0x0a,0x53,0x72,0x63,
    0x3a,0x20,0x20,0x46,0x61,0x74,0x61,0x6c,
    0x45,0x72,0x72,0x6f,0x72,0x0a,0x46,0x69,
    0x6c,0x65,0x3a,0x20,0x73,0x79,0x73,0x5f,
    0x6d,0x5f,0x73,0x6d,0x73,0x6d,0x5f,0x6d,
    0x70,0x73,0x73,0x2e,0x63,0x0a,0x4c,0x69,
    0x6e,0x65,0x3a,0x20,0x30,0x30,0x30,0x30,
    0x30,0x31,0x31,0x44,0x0a,0x53,0x74,0x72,
    0x3a,0x20,0x20,0x41,0x50,0x50,0x53,0x20,
    0x66,0x6f,0x72,0x63,0x65,0x20,0x73,0x74,
    0x6f,0x70,0x0a,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x0a,0x50,
    0x72,0x63,0x3a,0x20,0x20,0x4d,0x50,0x53,
    0x53,0x0a,0x54,0x61,0x73,0x6b,0x3a,0x20,
    0x41,0x4d,0x53,0x53,0x0a,0x54,0x69,0x6d,
    0x65,0x3a,0x20,0x30,0x30,0x36,0x46,0x38,
    0x37,0x38,0x42,0x0a,0x20,0x52,0x30,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x31,0x3a,0x20,0x43,
    0x30,0x30,0x34,0x36,0x31,0x33,0x30,0x20,
    0x20,0x52,0x32,0x3a,0x20,0x45,0x42,0x31,
    0x30,0x32,0x30,0x30,0x30,0x20,0x20,0x52,
    0x33,0x3a,0x20,0x44,0x38,0x35,0x32,0x45,
    0x42,0x36,0x30,0x20,0x20,0x52,0x34,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x46,
    0x45,0x0a,0x20,0x52,0x35,0x3a,0x20,0x43,
    0x33,0x36,0x41,0x38,0x41,0x36,0x34,0x20,
    0x20,0x52,0x36,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x52,
    0x37,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x31,0x20,0x20,0x52,0x38,0x3a,
    0x20,0x43,0x30,0x30,0x41,0x38,0x37,0x37,
    0x30,0x20,0x20,0x52,0x39,0x3a,0x20,0x30,
    0x31,0x38,0x46,0x30,0x30,0x34,0x35,0x0a,
    0x52,0x31,0x30,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x52,0x31,
    0x31,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x31,0x32,0x3a,
    0x20,0x43,0x35,0x30,0x33,0x38,0x30,0x30,
    0x30,0x20,0x52,0x31,0x33,0x3a,0x20,0x44,
    0x38,0x35,0x32,0x45,0x42,0x36,0x30,0x20,
    0x52,0x31,0x34,0x3a,0x20,0x46,0x46,0x46,
    0x46,0x30,0x30,0x30,0x34,0x0a,0x52,0x31,
    0x35,0x3a,0x20,0x43,0x30,0x30,0x31,0x34,
    0x38,0x35,0x30,0x20,0x52,0x31,0x36,0x3a,
    0x20,0x43,0x35,0x30,0x34,0x39,0x31,0x42,
    0x32,0x20,0x52,0x31,0x37,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x31,0x38,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x52,0x31,
    0x39,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x52,0x32,0x30,0x3a,
    0x20,0x43,0x30,0x30,0x34,0x36,0x30,0x31,
    0x43,0x20,0x52,0x32,0x31,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x35,0x41,0x20,
    0x52,0x32,0x32,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x37,0x46,0x20,0x52,0x32,
    0x33,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x32,0x34,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0a,0x52,0x32,0x35,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x32,0x36,0x3a,0x20,0x44,0x38,0x35,
    0x32,0x45,0x42,0x36,0x30,0x20,0x52,0x32,
    0x37,0x3a,0x20,0x43,0x31,0x35,0x30,0x34,
    0x45,0x35,0x43,0x20,0x52,0x32,0x38,0x3a,
    0x20,0x44,0x38,0x34,0x45,0x38,0x34,0x33,
    0x30,0x20,0x53,0x50,0x3a,0x20,0x20,0x44,
    0x38,0x35,0x32,0x45,0x35,0x30,0x38,0x0a,
    0x46,0x50,0x3a,0x20,0x20,0x44,0x38,0x35,
    0x32,0x45,0x35,0x31,0x30,0x20,0x4c,0x52,
    0x3a,0x20,0x20,0x43,0x30,0x30,0x39,0x46,
    0x32,0x37,0x43,0x0a,0x50,0x43,0x3a,0x20,
    0x35,0x33,0x36,0x30,0x31,0x38,0x38,0x32,
    0x0a,0x43,0x50,0x53,0x52,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x0a,
    0x4d,0x6f,0x64,0x3a,0x20,0x55,0x6e,0x6b,
    0x6e,0x6f,0x77,0x6e,0x0a,0x43,0x74,0x72,
    0x3a,0x20,0x41,0x52,0x4d,0x2c,0x20,0x49,
    0x52,0x51,0x20,0x64,0x69,0x73,0x2c,0x46,
    0x49,0x51,0x20,0x64,0x69,0x73,0x0a,0x54,
    0x4f,0x53,0x0a,0x44,0x38,0x35,0x32,0x45,
    0x35,0x33,0x30,0x20,0x35,0x33,0x36,0x30,
    0x31,0x39,0x42,0x36,0x20,0x43,0x30,0x37,
    0x38,0x33,0x45,0x34,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x35,0x41,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x32,0x20,
    0x43,0x34,0x38,0x37,0x44,0x45,0x32,0x38,
    0x20,0x43,0x35,0x30,0x34,0x39,0x31,0x42,
    0x32,0x0a,0x43,0x35,0x30,0x33,0x46,0x31,
    0x44,0x38,0x20,0x44,0x38,0x35,0x32,0x45,
    0x35,0x34,0x30,0x20,0x35,0x33,0x36,0x30,
    0x31,0x38,0x38,0x32,0x20,0x43,0x35,0x30,
    0x34,0x39,0x31,0x39,0x34,0x20,0x43,0x35,
    0x30,0x33,0x46,0x31,0x44,0x38,0x20,0x44,
    0x38,0x35,0x32,0x45,0x35,0x36,0x30,0x20,
    0x35,0x33,0x31,0x31,0x44,0x34,0x33,0x41,
    0x0a,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x31,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x43,0x34,0x38,0x37,
    0x44,0x45,0x32,0x38,0x20,0x43,0x32,0x30,
    0x42,0x43,0x35,0x45,0x34,0x20,0x46,0x46,
    0x46,0x46,0x46,0x46,0x46,0x46,0x20,0x44,
    0x38,0x35,0x32,0x45,0x36,0x30,0x38,0x0a,
    0x35,0x32,0x34,0x32,0x32,0x36,0x37,0x36,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x36,0x33,0x31,0x33,0x34,
    0x30,0x44,0x20,0x43,0x32,0x30,0x42,0x43,
    0x35,0x43,0x38,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x20,0x43,0x32,0x30,
    0x42,0x43,0x35,0x45,0x34,0x20,0x46,0x46,
    0x46,0x46,0x46,0x46,0x46,0x46,0x0a,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x43,0x34,0x38,0x37,0x44,0x45,0x32,0x38,
    0x20,0x43,0x30,0x37,0x38,0x33,0x45,0x34,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x35,0x41,0x0a,0x42,0x4f,0x53,0x0a,0x89,
    0x31,0xa1,0xa1,0x00,0x00,0x00,0x00,0x11,
    0x00,0x11,0x52,0x65,0x63,0x75,0x72,0x73,
    0x69,0x76,0x65,0x20,0x66,0x61,0x75,0x6c,
    0x74,0x21,0x0a,0xb5,0x02,0xb4,0x02,0x0a,
    0x53,0x72,0x63,0x3a,0x20,0x20,0x46,0x61,
    0x74,0x61,0x6c,0x45,0x72,0x72,0x6f,0x72,
    0x0a,0x53,0x74,0x72,0x3a,0x20,0x20,0x52,
    0x65,0x63,0x75,0x72,0x73,0x69,0x76,0x65,
    0x20,0x66,0x61,0x75,0x6c,0x74,0x21,0x0a,
    0x0a,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x0a,0x50,0x72,0x63,
    0x3a,0x20,0x20,0x41,0x50,0x53,0x53,0x0a,
    0x54,0x61,0x73,0x6b,0x3a,0x20,0x43,0x35,
    0x31,0x43,0x32,0x41,0x30,0x30,0x0a,0x54,
    0x69,0x6d,0x65,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x0a,0x20,0x52,
    0x30,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x20,0x52,0x31,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x32,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x20,0x52,0x33,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x52,
    0x34,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x20,0x52,0x35,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x36,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x20,0x52,0x37,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x20,0x52,
    0x38,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x20,0x52,0x39,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0a,0x52,0x31,0x30,0x3a,0x20,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x20,
    0x52,0x31,0x31,0x3a,0x20,0x43,0x35,0x32,
    0x35,0x37,0x45,0x42,0x43,0x20,0x52,0x31,
    0x32,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x52,0x31,0x33,0x3a,
    0x20,0x43,0x35,0x32,0x35,0x37,0x45,0x39,
    0x30,0x20,0x52,0x31,0x34,0x3a,0x20,0x43,
    0x30,0x30,0x31,0x31,0x44,0x43,0x34,0x0a,
    0x50,0x43,0x3a,0x20,0x43,0x30,0x30,0x31,
    0x34,0x38,0x33,0x43,0x0a,0x43,0x50,0x53,
    0x52,0x3a,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x0a,0x4d,0x6f,0x64,0x3a,
    0x20,0x55,0x6e,0x6b,0x6e,0x6f,0x77,0x6e,
    0x0a,0x43,0x74,0x72,0x3a,0x20,0x41,0x52,
    0x4d,0x2c,0x20,0x49,0x52,0x51,0x20,0x64,
    0x69,0x73,0x2c,0x46,0x49,0x51,0x20,0x64,
    0x69,0x73,0x0a,0x54,0x4f,0x53,0x0a,0x43,
    0x30,0x30,0x31,0x31,0x44,0x43,0x34,0x20,
    0x36,0x38,0x31,0x41,0x34,0x33,0x31,0x46,
    0x20,0x43,0x30,0x30,0x31,0x46,0x31,0x32,
    0x34,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x32,0x33,0x20,0x43,0x30,0x37,0x42,0x41,
    0x42,0x45,0x38,0x20,0x43,0x30,0x45,0x30,
    0x45,0x34,0x46,0x38,0x20,0x43,0x30,0x41,
    0x31,0x37,0x33,0x30,0x34,0x0a,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x20,0x30,
    0x30,0x31,0x31,0x39,0x30,0x44,0x38,0x20,
    0x43,0x35,0x32,0x35,0x37,0x46,0x38,0x38,
    0x20,0x43,0x30,0x30,0x31,0x46,0x31,0x44,
    0x34,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x32,0x33,0x20,0x43,0x30,0x41,0x31,
    0x32,0x45,0x42,0x34,0x0a,0x43,0x30,0x37,
    0x42,0x41,0x42,0x45,0x38,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x43,
    0x31,0x30,0x36,0x34,0x32,0x46,0x38,0x20,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x20,0x43,0x30,0x37,0x44,0x38,0x33,0x39,
    0x38,0x20,0x43,0x35,0x32,0x35,0x37,0x46,
    0x38,0x38,0x20,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x31,0x0a,0x43,0x30,0x45,0x39,
    0x30,0x46,0x42,0x43,0x20,0x43,0x36,0x36,
    0x42,0x39,0x36,0x30,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x31,0x20,0x43,
    0x30,0x31,0x33,0x41,0x37,0x45,0x30,0x20,
    0x43,0x35,0x32,0x35,0x37,0x46,0x38,0x38,
    0x20,0x30,0x30,0x31,0x31,0x39,0x30,0x44,
    0x38,0x20,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x32,0x0a,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x32,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x32,0x20,0x43,0x35,0x37,
    0x36,0x46,0x32,0x34,0x30,0x20,0x30,0x30,
    0x31,0x31,0x39,0x30,0x44,0x38,0x0a,0x42,
    0x4f,0x53,0x0a},
    // receiving eQMI_DMS_SWI_GET_SERIAL_NO_EXT
    {0x02,0x2e,0x00,0x75,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x39,0x00},
    // receiving eQMI_DMS_SWI_GET_SERIAL_NO_EXT
    {0x02,0x4e,0x00,0x75,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x39,0x00},
    // receiving eQMI_DMS_GET_IMSI
    {0x02,0x2f,0x00,0x43,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_GET_IMSI
    {0x02,0x4f,0x00,0x43,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_GET_PC_INFO
    {0x02,0x34,0x00,0x6e,0x55,0x1a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,0x10,0x04,0x00,0x20,0x00,0x00,
    0x00,0x11,0x01,0x00,0x00,0x13,0x01,0x00,
    0x00},
    // receiving eQMI_DMS_SWI_GET_PC_INFO
    {0x02,0x5c,0x00,0x6e,0x55,0x1a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,0x10,0x04,0x00,0x22,0x00,0x00,
    0x00,0x11,0x01,0x00,0x00,0x13,0x01,0x00,
    0x00},
    // receiving eQMI_DMS_PSM_GET_CFG_PARAMS
    {0x02,0x35,0x00,0x60,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_PSM_GET_CFG_PARAMS
    {0x02,0x5d,0x00,0x60,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_SET_USB_COMP
    {0x02,0x1c,0x00,0x5c,0x55,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x1d,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x1e,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x1f,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x20,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x21,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x22,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x23,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x24,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x25,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x2b,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x56,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_CUST_FEATURES
    {0x02,0x57,0x00,0x58,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SET_FIRMWARE_PREF
    {0x02,0x2f,0x00,0x48,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_DYING_GASP_CFG
    {0x02,0x32,0x00,0x7c,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_DYING_GASP_CFG
    {0x02,0x36,0x00,0x7c,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_DYING_GASP_CFG
    {0x02,0x58,0x00,0x7c,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_CLR_DYING_GASP_STAT
    {0x02,0x34,0x00,0x7e,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_GET_DYING_GASP_STAT
    {0x02,0x35,0x00,0x7d,0x55,0x12,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
    0x00,0x00,0x00,0x00,0x00,0x11,0x01,0x00,
    0x00},
    // receiving eQMI_DMS_UIM_SET_PIN_PROT
    {0x02,0x3c,0x00,0x27,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_PIN_UNBLOCK
    {0x02,0x3d,0x00,0x29,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_PIN_VERIFY
    {0x02,0x3e,0x00,0x28,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_PIN_CHANGE
    {0x02,0x3f,0x00,0x2a,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_SET_CK_PROT
    {0x02,0x42,0x00,0x41,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_UIM_UNBLOCK_CK
    {0x02,0x43,0x00,0x42,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x13,0x00},
    // receiving eQMI_DMS_FACTORY_DEFAULTS
    {0x02,0x44,0x00,0x3a,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_VALIDATE_SPC
    {0x02,0x45,0x00,0x3b,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_ACTIVATE_AUTOMATIC
    {0x02,0x46,0x00,0x32,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x19,0x00},
    // receiving eQMI_DMS_SWI_SET_HOST_DEV_INFO
    {0x02,0x4a,0x00,0x6b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_HOST_DEV_INFO
    {0x02,0x59,0x00,0x6b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_OS_INFO
    {0x02,0x4c,0x00,0x6d,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_OS_INFO
    {0x02,0x5a,0x00,0x6d,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_OS_INFO
    {0x02,0x5b,0x00,0x6d,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_EVENT_REPORT
    {0x02,0x54,0x00,0x57,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_SET_EVENT_REPORT
    {0x02,0x01,0x01,0x57,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_PSM_SET_CFG_PARAMS
    {0x02,0x5e,0x00,0x66,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_PSM_SET_CFG_PARAMS
    {0x02,0x04,0x01,0x66,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    // receiving eQMI_DMS_SWI_EVENT_IND
    {0x04,0x01,0x00,0x57,0x55,0x06,0x00,0x11,
    0x03,0x00,0x01,0x01,0x0d},
    // receiving eQMI_DMS_SWI_EVENT_IND
    {0x04,0x02,0x00,0x57,0x55,0x06,0x00,0x10,
    0x03,0x00,0x01,0x1e,0x00},
    // receiving eQMI_DMS_SWI_EVENT_IND UIM status
    {0x04,0x01,0x00,0x57,0x55,0x05,
        0x00,0x13,0x02,0x00,0x00,0x00},
    // receiving eQMI_DMS_SWI_EVENT_IND UIM status
    {0x04,0x02,0x00,0x57,0x55,0x05,
        0x00,0x13,0x02,0x00,0x01,0x01},
    // receiving eQMI_DMS_SWI_INDICATION_REGISTER
    {0x02,0x02,0x01,0x7f,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},
    // receiving eQMI_DMS_INDICATION_REGISTER
    {0x02,0x03,0x01,0x03,0x00,0x07,0x00,0x02,
    0x04,0x00,0x01,0x00,0x5e,0x00},
    //receiving eQMI_DMS_SWI_UIM_SELECT
    {0x02,0x02,0x00,0x5A,0x65,0x07,0x00,0x02,
        0x04,0x00,0x01,0x00,0x5E,0x00},
    //receiving eQMI_DMS_SWI_UIM_SELECT
    {0x02,0x04,0x00,0x5A,0x65,0x07,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00
    },
    //receiving eQMI_DMS_SWI_UIM_SELECT
    {0x02,0x05,0x00,0x5A,0x65,0x07,0x00,0x02,
        0x04,0x00,0x01,0x00,0x03,0x00},
};

void dms_dummy_unpack()
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    int rtn;
    ssize_t rlen;
    pack_qmi_t req_ctx;
    int loopCount = 0;
    int index = 0;
    memset(&req_ctx, 0, sizeof(req_ctx));    
    ////
    uint32_t lReasonMask = 0;
    uint32_t lbPlatform = 0;
    unpack_dms_GetOfflineReason_t lunpackdmsGetOfflineReason = 
        {&lReasonMask ,&lbPlatform, 0,SWI_UINT256_INT_VALUE};
    uint8_t lGetSmsphoneNumber[SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH]={0};
    uint8_t lGetSmsMessage[SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH]={0};
    packgetDyingGaspCfg lpGetPackgetDyingGaspCfg = 
        {&lGetSmsphoneNumber[0],&lGetSmsMessage[0]};
    unpack_dms_SLQSSwiGetDyingGaspCfg_t lunpackdmsSLQSSwiGetDyingGaspCfg = 
        {&lpGetPackgetDyingGaspCfg,0,SWI_UINT256_INT_VALUE};
    image_info_t lfw_info[16];
    unpack_dms_SLQSSwiGetFirmwareCurr_t lunpack_dms_SLQSSwiGetFirmwareCurr_t = {
        sizeof(lfw_info)/sizeof(image_info_t), lfw_info,
        {0}, {0},{0}, {0} ,SWI_UINT256_INT_VALUE};
    unpack_dms_SetEventReport_ind_t lunpack_dms_SetEventReport_ind_t;
    dms_PSMEnableStateTlv         lPsmEnableState;
    dms_PSMDurationThresholdTlv   lDurationThreshold;
    dms_PSMDurationDueToOOSTlv    lDurationDueToOOS;
    dms_PSMRandomizationWindowTlv lRandomizationWindow;
    dms_PSMActiveTimerTlv         lActiveTimer;
    dms_PSMPeriodicUpdateTimerTlv lPeriodicUpdateTimer;
    dms_PSMEarlyWakeupTimeTlv     lEarlyWakeupTime;
    unpack_dms_SLQSGetPowerSaveModeConfig_t lunpack_dms_SLQSGetPowerSaveModeConfig = {
        &lPsmEnableState, &lDurationThreshold, &lDurationDueToOOS, &lRandomizationWindow,
        &lActiveTimer, &lPeriodicUpdateTimer, &lEarlyWakeupTime,SWI_UINT256_INT_VALUE};
    unpack_dms_UIMSetPINProtection_t lunpack_dms_UIMUnblockPIN;
    unpack_dms_UIMSetPINProtection_t lunpack_dms_UIMVerifyPIN;
    unpack_dms_UIMSetPINProtection_t lunpack_dms_UIMChangePIN;
    unpack_dms_SwiEventReportCallBack_ind_t lunpack_dms_SwiEventReportCallBack_ind;
    unpack_dms_ResetToFactoryDefaults_t lunpack_dms_ValidateSPC;
    unpack_dms_ResetToFactoryDefaults_t lunpack_dms_ActivateAutomatic;
    unpack_dms_SLQSDmsSwiIndicationRegister_t lunpack_dms_SLQSDmsSwiIndicationRegister;
    unpack_dms_SetIndicationRegister_t lunpack_dms_SetIndicationRegister;
    FMSPrefImageList lfmsPrefImageList;
    unpack_fms_GetImagesPreference_t lunpack_fms_GetImagesPreference = {
        sizeof(FMSPrefImageList),
        &lfmsPrefImageList,
        0,
        SWI_UINT256_INT_VALUE
    };
    uint32_t lTimeStamp = -1;
    uint8_t  lSMSAttemptedFlag = -1;
    packgetDyingGaspStatistics lGetDyingGaspStatistics={&lTimeStamp,&lSMSAttemptedFlag};
    unpack_dms_SLQSSwiGetDyingGaspStatistics_t lunpack_dms_SLQSSwiGetDyingGaspStatistics=
    {&lGetDyingGaspStatistics,0,SWI_UINT256_INT_VALUE};
    ///
    loopCount = sizeof(dummy_dms_resp_msg)/sizeof(dummy_dms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index>=loopCount)
            return ;
        //TODO select multiple file and read them
        memcpy(&msg.buf,&dummy_dms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn + 7;
            qmi_msg = helper_get_resp_ctx(eDMS, msg.buf, rlen, &rsp_ctx);

            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);

            if (rsp_ctx.type == eIND)
                printf("DMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("DMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
                case eQMI_DMS_SWI_GET_USB_COMP:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetUSBComp,
                            dump_GetUSBComp,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_OPERATING_MODE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetOfflineReason,
                            dump_GetOfflineReason,
                            msg.buf, 
                            rlen);
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetOfflineReason,
                            dump_GetOfflineReason,
                            msg.buf, 
                            rlen,
                            &lunpackdmsGetOfflineReason);
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetPower,
                            dump_GetPower,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_CUST_FEATURES:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetCustFeature,
                            dump_GetCustFeature,
                            msg.buf, 
                            rlen);
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetCustFeaturesV2,
                            dump_GetCustFeaturesV2,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_DYING_GASP_CFG:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetDyingGaspCfg,
                            dump_SLQSSwiGetDyingGaspCfg,
                            msg.buf, 
                            rlen);
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetDyingGaspCfg,
                            dump_SLQSSwiGetDyingGaspCfg,
                            msg.buf, 
                            rlen,
                            &lunpackdmsSLQSSwiGetDyingGaspCfg);
                    }
                    break;
                case eQMI_DMS_SWI_GET_HOST_DEV_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetHostDevInfo,
                            dump_SLQSSwiGetHostDevInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_OS_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetOSInfo,
                            dump_SLQSSwiGetOSInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_CRASH_ACTION:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetCrashAction,
                            dump_GetCrashAction,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_FIRMWARE_CURR:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetFirmwareCurr,
                            dump_SLQSSwiGetFirmwareCurr,
                            msg.buf, 
                            rlen);
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetFirmwareCurr,
                            dump_SLQSSwiGetFirmwareCurr,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SLQSSwiGetFirmwareCurr_t);
                    }
                    break;
                case eQMI_DMS_SET_EVENT:
                    {
                        if (rsp_ctx.type == eIND)
                        {
                            UNPACK_IND_RESP(unpackRetCode,
                            unpack_dms_SetEventReport_ind,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SetEventReport_ind_t);
                            if((lunpack_dms_SetEventReport_ind_t.ActivationStatusTlv.TlvPresent!=0)&& 
                            (swi_uint256_get_bit (lunpack_dms_SetEventReport_ind_t.ParamPresenceMask, 19)))
                            {
                                printf("QMI_DMS_SET_EVENT_REPORT IND activationStatus :%d\n",lunpack_dms_SetEventReport_ind_t.ActivationStatusTlv.activationStatus);
                            }
                            if((lunpack_dms_SetEventReport_ind_t.OperatingModeTlv.TlvPresent!=0) && 
                                (swi_uint256_get_bit (lunpack_dms_SetEventReport_ind_t.ParamPresenceMask, 20)))
                            {
                                printf("QMI_DMS_SET_EVENT_REPORT IND operatingMode :%d\n",lunpack_dms_SetEventReport_ind_t.OperatingModeTlv.operatingMode);
                            }
                        }
                        else if(rsp_ctx.type == eRSP)
                        {
                            UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetEventReport,
                            dump_SetEventReport,
                            msg.buf, 
                            rlen);
                        }
                    }
                    break;
                case eQMI_DMS_GET_MODEL_ID:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetModelID,
                            dump_GetModelId,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_CWE_SPKGS_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetFirmwareInfo,
                            dump_GetFirmwareInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_REV_ID:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetFirmwareRevisions,
                            dump_FirmwareRevisions,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_IDS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetDeviceSerialNumbers,
                            dump_GetDeviceSerialNumbers,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_CURRENT_PRL_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetPRLVersion,
                            dump_GetPRLVersion,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_TIME:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetNetworkTime,
                            dump_GetNetworkTime,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_MSISDN:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetVoiceNumber,
                            dump_GetVoiceNumber,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_HARDWARE_REV:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetDeviceHardwareRev,
                            dump_GetDeviceHardwareRev,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_FSN:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetFSN,
                            dump_GetFSN,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_CAPS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetDeviceCap,
                            dump_GetDeviceCap,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SET_OPERATING_MODE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetPower,
                            dump_SetPower,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_BAND_CAPS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetBandCapability,
                            dump_GetBandCapability,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_MANUFACTURER:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetDeviceMfr,
                            dump_GetDeviceMfr,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_GET_ICCID:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMGetICCID,
                            dump_UIMGetICCID,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_FIRMWARE_PREF:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_fms_GetImagesPreference,
                            dump_FMS_GetImagesPreference,
                            msg.buf, 
                            rlen);
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_fms_GetImagesPreference,
                            dump_FMS_GetImagesPreference,
                            msg.buf, 
                            rlen,
                            &lunpack_fms_GetImagesPreference);
                    }
                    break;
                case eQMI_DMS_LIST_FIRMWARE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_fms_GetStoredImages,
                            dump_FMS_GetStoredImages,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_ACTIVATED_STATE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetActivationState,
                            dump_GetActivationState,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_RESET_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSDmsSwiGetResetInfo,
                            dump_SLQSDmsSwiGetResetInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_FW_UPDATE_STAT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetFwUpdateStatus,
                            dump_SLQSSwiGetFwUpdateStatus,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_GET_CK_STATUS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMGetControlKeyStatus,
                            dump_UIMGetControlKeyStatus,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_GET_PIN_STATUS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMGetPINStatus,
                            dump_UIMGetPINStatus,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_READ_ERI_FILE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSGetERIFile,
                            dump_SLQSGetERIFile,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_GET_STATE:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSUIMGetState,
                            dump_SLQSUIMGetState,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_CRASH_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetCrashInfo,
                            dump_SLQSSwiGetCrashInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_SERIAL_NO_EXT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetSerialNoExt,
                            dump_SLQSSwiGetSerialNoExt,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_GET_IMSI:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_GetIMSI,
                            dump_GetIMSI,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_PC_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSDmsSwiGetPCInfo,
                            dump_SLQSDmsSwiGetPCInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_PSM_GET_CFG_PARAMS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSGetPowerSaveModeConfig,
                            dump_SLQSGetPowerSaveModeConfig,
                            msg.buf, 
                            rlen);
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSGetPowerSaveModeConfig,
                            dump_SLQSGetPowerSaveModeConfig,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SLQSGetPowerSaveModeConfig);
                    }
                    break;
                case eQMI_DMS_SWI_SET_USB_COMP:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetUSBComp,
                            dump_SetUSBComp,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_SET_CUST_FEATURES:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetCustFeature,
                            dump_SetCustFeature,
                            msg.buf, 
                            rlen);
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetCustFeaturesV2,
                            dump_SetCustFeaturesV2,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SET_FIRMWARE_PREF:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SetFirmwarePreference,
                            dump_SetFirmwarePreference,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_SET_DYING_GASP_CFG:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiSetDyingGaspCfg,
                            dump_SLQSSwiSetDyingGaspCfg,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_CLR_DYING_GASP_STAT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiClearDyingGaspStatistics,
                            dump_SLQSSwiClearDyingGaspStatistics,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_GET_DYING_GASP_STAT:
                    {                           
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetDyingGaspStatistics,
                            dump_SLQSSwiGetDyingGaspStatistics,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SLQSSwiGetDyingGaspStatistics);
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiGetDyingGaspStatistics,
                            dump_SLQSSwiGetDyingGaspStatistics,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_SET_PIN_PROT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMSetPINProtection,
                            dump_UIMSetPINProtection,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_PIN_UNBLOCK:
                    {
                        memset(&lunpack_dms_UIMUnblockPIN,0,
                            sizeof(lunpack_dms_UIMUnblockPIN));
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMUnblockPIN,
                            dump_UIMUnblockPIN,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_UIMUnblockPIN);
                    }
                    break;
                case eQMI_DMS_UIM_PIN_VERIFY:
                    {
                        memset(&lunpack_dms_UIMVerifyPIN,0,
                            sizeof(lunpack_dms_UIMVerifyPIN));
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMVerifyPIN,
                            dump_UIMVerifyPIN,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_UIMVerifyPIN);
                    }
                    break;
                case eQMI_DMS_UIM_PIN_CHANGE:
                    {
                        memset(&lunpack_dms_UIMChangePIN,0,
                            sizeof(lunpack_dms_UIMChangePIN));
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMChangePIN,
                            dump_UIMChangePIN,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_UIMChangePIN);
                    }
                    break;
                case eQMI_DMS_UIM_SET_CK_PROT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMSetControlKeyProtection,
                            dump_UIMSetControlKeyProtection,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_UIM_UNBLOCK_CK:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_UIMSetControlKeyProtection,
                            dump_UIMSetControlKeyProtection,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_FACTORY_DEFAULTS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_ResetToFactoryDefaults,
                            dump_ResetToFactoryDefaults,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_VALIDATE_SPC:
                    {
                        memset(&lunpack_dms_ValidateSPC,0,
                            sizeof(lunpack_dms_ValidateSPC));
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_ValidateSPC,
                            dump_ValidateSPC,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_ValidateSPC);
                    }
                    break;
                case eQMI_DMS_ACTIVATE_AUTOMATIC:
                    {
                        memset(&lunpack_dms_ActivateAutomatic,0,
                            sizeof(lunpack_dms_ActivateAutomatic));
                        UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_ActivateAutomatic,
                            dump_ActivateAutomatic,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_ActivateAutomatic);
                    }
                    break;
                case eQMI_DMS_SWI_SET_HOST_DEV_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiSetHostDevInfo,
                            dump_SLQSSwiSetHostDevInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_SET_OS_INFO:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSwiSetOSInfo,
                            dump_SLQSSwiSetOSInfo,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_SET_EVENT_REPORT:
                    {
                        if(rsp_ctx.type == eIND)
                        {
                            memset(&lunpack_dms_SwiEventReportCallBack_ind,0,
                            sizeof(lunpack_dms_SwiEventReportCallBack_ind));
                            UNPACK_IND_RESP(unpackRetCode,
                                unpack_dms_SwiEventReportCallBack_ind,
                                msg.buf, 
                                rlen,
                                &lunpack_dms_SwiEventReportCallBack_ind);
                            if(unpackRetCode==0)
                            {
                                if(lunpack_dms_SwiEventReportCallBack_ind.TempTlv.TlvPresent!=0)
                                {
                                    printf("QMI_DMS_SWI_SET_EVENT_REPORT IND temp State :%d temperature %d\n",
                                            lunpack_dms_SwiEventReportCallBack_ind.TempTlv.TempStat,
                                            lunpack_dms_SwiEventReportCallBack_ind.TempTlv.Temperature);
                                }

                                if(lunpack_dms_SwiEventReportCallBack_ind.VoltTlv.TlvPresent!=0)
                                {
                                    printf("QMI_DMS_SWI_SET_EVENT_REPORT IND volt State :%d voltage %d\n",
                                            lunpack_dms_SwiEventReportCallBack_ind.VoltTlv.VoltStat,
                                            lunpack_dms_SwiEventReportCallBack_ind.VoltTlv.Voltage);
                                }
                                if(lunpack_dms_SwiEventReportCallBack_ind.UimStatusTlv.TlvPresent!=0)
                                {
                                    printf("QMI_DMS_SWI_SET_EVENT_REPORT IND UIM status interface :%d event %d\n",
                                            lunpack_dms_SwiEventReportCallBack_ind.UimStatusTlv.intf,
                                            lunpack_dms_SwiEventReportCallBack_ind.UimStatusTlv.event);
                                }
                            }
                        }
                        else if(rsp_ctx.type == eRSP)
                        {
                            UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                                unpack_dms_SwiSetEventReport,
                                dump_SwiSetEventReport,
                                msg.buf, 
                                rlen);
                        }
                    }
                    break;
                case eQMI_DMS_PSM_SET_CFG_PARAMS:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SLQSSetPowerSaveModeConfig,
                            dump_SLQSSetPowerSaveModeConfig,
                            msg.buf, 
                            rlen);
                    }
                    break;
                case eQMI_DMS_SWI_INDICATION_REGISTER:
                    {
                        memset(&lunpack_dms_SLQSDmsSwiIndicationRegister,0,
                            sizeof(lunpack_dms_SLQSDmsSwiIndicationRegister));
                        UNPACK_RESP(unpackRetCode,
                            unpack_dms_SLQSDmsSwiIndicationRegister,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SLQSDmsSwiIndicationRegister);
                    }
                    break;
                case eQMI_DMS_INDICATION_REGISTER:
                    {
                        memset(&lunpack_dms_SetIndicationRegister,0,
                            sizeof(lunpack_dms_SetIndicationRegister));
                        UNPACK_RESP(unpackRetCode,
                            unpack_dms_SetIndicationRegister,
                            msg.buf, 
                            rlen,
                            &lunpack_dms_SetIndicationRegister);
                    }
                    break;
                case eQMI_DMS_SWI_UIM_SELECT:
                    {
                        UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                            unpack_dms_SwiUimSelect,
                            dump_SwiUimSelect,
                            msg.buf, 
                            rlen);
                    }
                    break;
                default:
                    break;

            }
        }
    }
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_dms_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_DMS_GET_MODEL_ID */
    {0x02,0x01,0x00,0x22,0x00,0x10,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x06,
    0x00,0x45,0x4d,0x37,0x35,0x36,0x35
    },

    /* eQMI_DMS_GET_IMSI */
    {0x02,0x02,0x00,0x43,0x00,0x12,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48
    },

    /* eQMI_DMS_GET_CWE_SPKGS_INFO */
    {0x02,0x03,0x00,0x56,0x55,0x57,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x06,0x00,0x45,0x4d,0x37,0x35,0x36,0x35,
    0x11,0x14,0x00,0x53,0x57,0x49,0x39,0x58,0x35,
     0x30,0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,
     0x2e,0x30,0x33,0x2e,0x30,0x30,
    0x12,0x14,0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
     0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
    0x30,0x33,0x2e,0x30,0x30,
    0x15,0x01,0x00,0x30,
    0x17,0x04,0x00,0x4e,0x4f,0x4e,0x45,
    0x18,0x0b,0x00,0x30,0x30,0x30,0x2e,0x30,
     0x30,0x30,0x5f,0x30,0x30,0x30
    },

    /* eQMI_DMS_GET_OPERATING_MODE */
    {0x02,0x04,0x00,0x2d,0x00,0x14,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,
    0x10,0x02,0x00,0x02,0x00,
    0x11,0x01,0x00,0x04,},

    /* eQMI_DMS_GET_IDS */
    {0x02,0x05,0x00,0x25,0x00,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x01,0x00,0x30,
    0x11,0x0f,0x00,0x30,0x30,0x31,
     0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,
     0x39,0x39,0x39,0x39,
    0x12,0x04,0x00,0x31,0x32,0x33,0x34,
    0x13,0x04,0x00,0x41,0x42,0x43,0x44,
    },

    /* eQMI_DMS_GET_HARDWARE_REV */
    {0x02,0x06,0x00,0x2c,0x00,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},

    /* eQMI_DMS_GET_BAND_CAPS */
    {0x02,0x07,0x00,0x45,0x00,0x28,0x00,
    0x02,0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x08,
     0x00,0x00,0x00,0xc0,0x0f,0x00,0x00,0x06,
     0x10,
    0x10,0x08,0x00,0xdf,0x19,0x0e,0xba,
     0x00,0x27,0x00,0x00,
    0x11,0x08,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00,0x00
    },

    /* eQMI_DMS_GET_CAPS */
    {0x02,0x08,0x00,0x20,0x00,0x8a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x0d,
     0x00,0x80,0xd1,0xf0,0x08,0x00,0x46,0xc3,
     0x23,0x04,0x02,0x02,0x05,0x08,
    0x10,0x04,0x00,0x04,0x00,0x00,0x00,
    0x11,0x08,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x12,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x13,0x0c,0x00,0x01,0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x14,0x02,0x00,0x01,0x01,
    0x15,0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x16,0x06,0x00,0x01,0x01,0x00,0x00,0x00,0x00,
    0x17,0x05,0x00,0x01,0x00,0x00,0x00,0x00,
    0x18,0x01,0x00,0x01,
    0x19,0x09,0x00,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x1a,0x0d,0x00,0x01,0x01,0x01,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x1b,0x02,0x00,0xff,0xff
    },

    /* eQMI_DMS_GET_REV_ID */
    {0x02,0x09,0x00,0x23,0x00,0x25,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x07,0x00,0x53,0x57,0x49,0x39,0x58,0x35,0x30,
    0x10,0x07,0x00,0x43,0x5f,0x30,0x31,0x32,0x30,0x37,
    0x11,0x07,0x00,0x30,0x33,0x33,0x30,0x30,0x30,0x35,
    },

    /* eQMI_DMS_GET_CURRENT_PRL_INFO */
    {0x02,0x0A,0x00,0x53,0x00,0x10,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x02,0x00,0x02,0x00,
    0x11,0x01,0x00,0x01
    },

    /* eQMI_DMS_GET_TIME */
    {0x02,0x0B,0x00,0x2f,0x00,0x28,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x08,
    0x00,0x53,0xd5,0x48,0x08,0x00,0x00,0x00,
    0x00,0x10,0x08,0x00,0xa9,0x0a,0x5b,0x0a,
    0x00,0x00,0x00,0x00,0x11,0x08,0x00,0xa9,
    0x0a,0x5b,0x0a,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_GET_MSISDN */
    {0x02,0x0c,0x00,0x24,0x00,0x1c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0A,
    0x00,0x39,0x32,0x33,0x35,0x37,0x39,0x30,0x33,0x36,0x34,
    0x10,0x05,
    0x00,0x39,0x30,0x33,0x36,0x34},

    /* eQMI_DMS_GET_FSN */
    {0x02,0x0d,0x00,0x67,0x55,0x0f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05,
    0x00,0x31,0x30,0x30,0x30,0x31},

    /* eQMI_DMS_SET_OPERATING_MODE */
    {0x02,0x0e,0x00,0x2e,0x00,0x07,0x00,0x02,
      0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_USB_COMP */
    {0x02,0x0f,0x00,0x5b,0x55,0x12,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x01,0x00,0x02,
    0x11,0x04,0x00,0x03,0x01,0x02,0x03},

    /* eQMI_DMS_SWI_SET_USB_COMP */
    {0x02,0x10,0x00,0x5c,0x55,0x07,0x00,0x02,
      0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_CUST_FEATURES */
    {0x02,0x11,0x00,0x57,0x65,0x7A,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x04,0x00,0x01,0x00,0x00,0x00,
    0x11,0x01,0x00,0x01,
    0x12,0x02,0x00,0x01,0x00,
    0x13,0x01,0x00,0x01,
    0x14,0x01,0x00,0x01,
    0x15,0x01,0x00,0x01,
    0x16,0x01,0x00,0x01,
    0x17,0x01,0x00,0x01,
    0x18,0x01,0x00,0x01,
    0x20,0x11,0x00,0x0a,0x00,0x57,0x41,0x4b,0x45,0x48,
    0x4f,0x53,0x54,0x45,0x4e,0x01,0x00,0x01,
    0x01,0x00,

    0x21,0x34,0x00,
    0x00,0x03,0x00,0x09,0x00,0x47,0x50,0x53,0x45,
    0x4e,0x41,0x42,0x4c,0x45,0x01,0x00,0x01,0x01,0x00,
    0x06,0x00,0x47,0x50,0x53,0x4c,0x50,0x4d,0x01,
    0x00,0x00,0x01,0x00,
    0x0d,0x00,0x47,0x50,0x49,0x4f,0x53,0x41,0x52,
    0x45,0x4e,0x41,0x42,0x4c,0x45,0x01,0x00,0x00,0x01,0x00,
    },

    /* eQMI_DMS_SWI_SET_CUST_FEATURES */
    {0x02,0x12,0x00,0x58,0x65,0x07,0x00,0x02,
      0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SET_FIRMWARE_PREF */
    {0x02,0x13,0x00,0x48,0x00,0x07,0x00,0x02,
      0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_CRASH_ACTION */
    {0x02,0x14,0x00,0x68,0x55,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01},

    /* eQMI_DMS_SWI_SET_CRASH_ACTION */
    {0x02,0x15,0x00,0x69,0x55,0x07,0x00,0x02,
      0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_GET_MANUFACTURER */
    {0x02,0x16,0x00,0x21,0x00,0x27,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x1d,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,
    0x6f,0x72,0x61,0x74,0x65,0x64},

    /* eQMI_DMS_SET_EVENT */
    {0x02,0x17,0x00,0x01,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    {0x04,0x18,0x00,0x01,0x00,0x0A,0x00,
     0x13,0x02,0x00,0x01,0x00,
     0x14,0x02,0x00,0x01,0x00},

    /* eQMI_DMS_UIM_GET_ICCID */
    {0x02,0x19,0x00,0x3C,0x00,0x18,0x00,0x02,
     0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x0E,
     0x00,0x39,0x38,0x31,0x30,0x35,0x33,0x39,
     0x35,0x36,0x31,0x32,0x31,0x30,0x34},

    /* eQMI_DMS_GET_ACTIVATED_STATE */
    {0x02,0x1A,0x00,0x31,0x00,0x0c,0x00,0x02,
     0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
     0x00,0x04,0x00},

    /* eQMI_DMS_SWI_GET_FIRMWARE_CURR */
    {0x02,0x1B,0x00,0x63,0x55,0x4f,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x45,
    0x00,0x02,0x00,0x30,0x30,0x30,0x2e,0x30,
    0x30,0x30,0x5f,0x30,0x30,0x30,0x00,0x00,
    0x00,0x00,0x00,0x10,0x30,0x31,0x2e,0x30,
    0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,
    0x4e,0x4f,0x4e,0x45,0x01,0x30,0x30,0x30,
    0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30,
    0x00,0x00,0x00,0x00,0x00,0x10,0x30,0x31,
    0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,0x30,
    0x30,0x5f,0x4e,0x4f,0x4e,0x45},

    /* eQMI_DMS_SWI_SET_DYING_GASP_CFG */
    {0x02,0x1C,0x00,0x7c,0x55,0x07,0x00,0x02,
     0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_CLR_DYING_GASP_STAT */
    {0x02,0x1D,0x00,0x7e,0x55,0x07,0x00,0x02,
     0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_DYING_GASP_STAT */
    {0x02,0x1E,0x00,0x7d,0x55,0x12,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
    0x00,0x01,0x02,0x00,0x00,0x11,0x01,0x00,
    0x01},

    /* eQMI_DMS_SWI_GET_DYING_GASP_CFG */
    {0x02,0x1F,0x00,0x7b,0x55,0x2b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
    0x00,0x2b,0x38,0x35,0x32,0x32,0x34,0x33,
    0x33,0x35,0x34,0x33,0x33,0x11,0x12,0x00,
    0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,
    0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
    0x31,0x21},

    /* eQMI_DMS_SWI_GET_RESET_INFO */
    {0x02,0x20,0x00,0x80,0x55,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x00,0x02,0x01},

    {0x04,0x21,0x00,0x80,0x55,0x05,0x00,0x01,0x02,
    0x00,0x02,0x01},

    /* eQMI_DMS_SWI_INDICATION_REGISTER */
    {0x02,0x22,0x01,0x7f,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_FW_UPDATE_STAT */
    {0x02,0x23,0x00,0x64,0x55,0x22,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x04,
    0x00,0xff,0xff,0xff,0xff,
    0x11,0x01,0x00,0x01,
    0x12,0x04,0x00,0x04,0x00,0x00,0x00,
    0x13,0x06,0x00,0x41,0x42,0x43,0x44,0x45,0x46},

    /* eQMI_DMS_UIM_SET_PIN_PROT */
    {0x02,0x24,0x00,0x27,0x00,0x0c,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x02,0x00,0x03,0x03},

    /* eQMI_DMS_UIM_GET_CK_STATUS */
    {0x02,0x25,0x00,0x40,0x00,0x0d,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x03,0x00,0x01,0x03,0x03},

    /* eQMI_DMS_UIM_GET_PIN_STATUS */
    {0x02,0x26,0x00,0x2b,0x00,0x13,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x11,0x03,0x00,0x01,0x02,0x02,
    0x12,0x03,0x00,0x01,0x04,0x04},

    /* eQMI_DMS_UIM_SET_CK_PROT */
    {0x02,0x27,0x00,0x41,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x01,0x00,0x02},

    /* eQMI_DMS_UIM_UNBLOCK_CK */
    {0x02,0x28,0x00,0x42,0x00,0x0b,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x01,0x00,0x03},

    /* eQMI_DMS_FACTORY_DEFAULTS */
    {0x02,0x29,0x00,0x3a,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_READ_ERI_FILE */
    {0x02,0x2A,0x00,0x39,0x00,0x1e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x14,
    0x00,0x12,0x00,0x41,0x44,0x45,0x46,0x41,0x44,0x45,0x46,
    0x41,0x44,0x45,0x46,0x41,0x44,0x45,0x46,0x41,0x41},

    /* eQMI_DMS_UIM_GET_STATE */
    {0x02,0x2B,0x00,0x44,0x00,0x0B,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x00,0x01},

    /* eQMI_DMS_SWI_GET_CRASH_INFO */
    {0x02,0x2c,0x00,0x65,0x55,0x2A,0x01,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x00,0x01,
    0x10,0x1C,0x01,0x02,0x83,0x24,
    0x86,0x11,0x00,0x00,0x00,0x00,0x0f,0x0f,
    0x00,0x41,0x50,0x50,0x53,0x20,0x66,0x6f,
    0x72,0x63,0x65,0x20,0x73,0x74,0x6f,0x70,
     0xff,0x00,0xc3,0x03,0x0a,0x53,0x72,0x63,
    0x3a,0x20,0x20,0x46,0x61,0x74,0x61,0x6c,
    0x45,0x72,0x72,0x6f,0x72,0x0a,0x46,0x69,
    0x6c,0x65,0x3a,0x20,0x73,0x79,0x73,0x5f,
    0x6d,0x5f,0x73,0x6d,0x73,0x6d,0x5f,0x6d,
    0x70,0x73,0x73,0x2e,0x63,0x0a,0x4c,0x69,
    0x6e,0x65,0x3a,0x20,0x30,0x30,0x30,0x30,
    0x30,0x31,0x31,0x44,0x0a,0x53,0x74,0x72,
    0x3a,0x20,0x20,0x41,0x50,0x50,0x53,0x20,
    0x66,0x6f,0x72,0x63,0x65,0x20,0x73,0x74,
    0x6f,0x70,0x0a,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x20,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x0a,0x50,
    0x72,0x63,0x3a,0x20,0x20,0x4d,0x50,0x53,
    0x53,0x0a,0x54,0x61,0x73,0x6b,0x3a,0x20,
    0x41,0x4d,0x53,0x53,0x0a,0x54,0x69,0x6d,
    0x65,0x3a,0x20,0x30,0x30,0x36,0x46,0x38,
    0x37,0x38,0x42,0x0a,0x20,0x52,0x30,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x20,0x20,0x52,0x31,0x3a,0x20,0x43,
    0x30,0x30,0x34,0x36,0x31,0x33,0x30,0x20,
    0x20,0x52,0x32,0x3a,0x20,0x45,0x42,0x31,
    0x30,0x32,0x30,0x30,0x30,0x20,0x20,0x52,
    0x33,0x3a,0x20,0x44,0x38,0x35,0x32,0x45,
    0x42,0x36,0x30,0x20,0x20,0x52,0x34,0x3a,
    0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x46,
    0x45,0x0a,0x20,0x52,0x35,0x3a,0x20,0x43,
    0x33,0x36,0x41,0x38,0x41,0x36,0x34,0x20,
    0x20,0x52,0x36,0x3a,0x20,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x52,0x37},

    /* eQMI_DMS_SWI_GET_HOST_DEV_INFO */
    {0x02,0x2D,0x00,0x6a,0x55,0x31,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0f,
    0x00,0x53,0x69,0x65,0x72,0x72,0x61,0x20,
    0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
    0x11,0x04,0x00,0x4d,0x47,0x39,0x30,0x12,
    0x01,0x00,0x34,0x14,0x0a,0x00,0x31,0x30,
    0x54,0x6b,0x68,0x69,0x32,0x61,0x34,0x67
    },

    /* eQMI_DMS_SWI_SET_HOST_DEV_INFO */
    {0x02,0x2E,0x00,0x6b,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_OS_INFO */
    {0x02,0x2F,0x00,0x6c,0x55,0x1e,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x05,
    0x00,0x4c,0x69,0x6e,0x75,0x78,0x11,0x0c,
    0x00,0x55,0x62,0x75,0x6e,0x74,0x75,0x20,
    0x31,0x34,0x2e,0x30,0x34},

    /* eQMI_DMS_SWI_SET_OS_INFO */
    {0x02,0x30,0x00,0x6d,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_SERIAL_NO_EXT */
    {0x02,0x31,0x00,0x75,0x55,0x10,0x00,0x02,
     0x04,0x00,0x00,0x00,0x00,0x00,
     0x10,0x06,0x00,
     0x53,0x69,0x65,0x72,0x72,0x61},

    /* eQMI_DMS_SWI_GET_PC_INFO */
    {0x02,0x32,0x00,0x6e,0x55,0x1a,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
    0x00,0x01,0x10,0x04,0x00,0x22,0x00,0x00,
    0x00,0x11,0x01,0x00,0x00,0x13,0x01,0x00,
    0x00},

    /* eQMI_DMS_PSM_GET_CFG_PARAMS */
    {0x02,0x33,0x00,0x60,0x00,0x35,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x01,0x00,0x01,
    0x11,0x04,0x00,0x01,0x02,0x00,0x00,
    0x12,0x04,0x00,0x03,0x04,0x00,0x00,
    0x13,0x04,0x00,0x05,0x06,0x00,0x00,
    0x14,0x04,0x00,0x07,0x08,0x00,0x00,
    0x15,0x04,0x00,0x11,0x22,0x00,0x00,
    0x16,0x04,0x00,0x21,0x22,0x00,0x00 },

    /* eQMI_DMS_SWI_SET_OS_INFO */
    {0x02,0x34,0x00,0x66,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_INDICATION_REGISTER */
    {0x02,0x35,0x00,0x03,0x00,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_PSM_CFG_PARAMS_CHANGE_IND */
    {0x04,0x36,0x00,0x67,0x00,0x12,0x00,
    0x10,0x01,0x00,0x01,
    0x11,0x04,0x00,0x01,0x02,0x00,0x00,
    0x12,0x04,0x00,0x03,0x04,0x00,0x00},

    /* eQMI_DMS_SWI_SET_EVENT_REPORT */
    {0x02,0x37,0x00,0x57,0x55,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    {0x04,0x38,0x00,0x57,0x55,0x11,0x00,
    0x10,0x03,0x00,0x01,0x1e,0x00,
    0x11,0x03,0x00,0x01,0x01,0x0d,
    0x13,0x02,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_UIM_SELECT */
    {0x02,0x35,0x00,0x5a,0x65,0x07,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_DMS_SWI_GET_UIM_SELECTION */
    {0x02,0x35,0x00,0x69,0x65,0x0F,0x00,0x02,
    0x04,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x00,0x01,
    0x10,0x01,0x00,0x01,},

    /* eQMI_DMS_GET_BAND_CAPS */
    {0x02,0x36,0x00,0x45,0x00,0x39,0x00,
     0x02,0x04,0x00,0x00,0x00,0x00,0x00,
     0x01,0x08,0x00,0x80,0x01,0x40,0x00,0x00,0x00,0x02,0x00,
     0x10,0x08,0x00,0xc5,0x00,0x08,0x08,0x00,0x00,0x00,0x00,
     0x11,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x12,0x0e,0x00,0x06,0x00,0x01,0x00,0x03,0x00,0x07,0x00,0x08,0x00,0x14,0x00,0x1c,0x00},

    };

/* eQMI_DMS_GET_MODEL_ID */
const unpack_dms_GetModelID_t const_unpack_dms_GetModelID_t = {
        {0x45,0x4d,0x37,0x35,0x36,0x35},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_GET_IMSI */
const unpack_dms_GetIMSI_t const_unpack_dms_GetIMSI_t = {
        {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_GET_CWE_SPKGS_INFO */
const unpack_dms_GetFirmwareInfo_t const_unpack_dms_GetFirmwareInfo_t = {
        {0x45,0x4d,0x37,0x35,0x36,0x35},
        {0x53,0x57,0x49,0x39,0x58,0x35, 0x30,0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,
            0x2e,0x30,0x33,0x2e,0x30,0x30
        },
        {0x53,0x57,0x49,0x39,0x58,0x35,0x30,0x43,0x5f,0x30,0x31,0x2e,0x30,0x37,0x2e,
           0x30,0x33,0x2e,0x30,0x30
        },
        {0},{0},{0x30},{0},
        {0x4e,0x4f,0x4e,0x45},
        {0x30,0x30,0x30,0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30},
        0,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,21,23,24)}} };

/* eQMI_DMS_GET_OPERATING_MODE */
const unpack_dms_GetPower_t const_unpack_dms_GetPower_t = {
        1,2,4,0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

uint32_t var_ReasonMask;
uint32_t var_bPlatform;

unpack_dms_GetOfflineReason_t var_unpack_dms_GetOfflineReason_t = {
        &var_ReasonMask,&var_bPlatform, 0,{{0}} };

uint32_t cst_ReasonMask = 2;
uint32_t cst_bPlatform = 4;
const unpack_dms_GetOfflineReason_t const_unpack_dms_GetOfflineReason_t = {
         &cst_ReasonMask,&cst_bPlatform, 0,
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_DMS_GET_IDS */
const unpack_dms_GetSerialNumbers_t const_unpack_dms_GetSerialNumbers_t = {
        {0x30}, 
        {0x30,0x30,0x31,0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,0x39,0x39,0x39,0x39},
        {0x31,0x32,0x33,0x34},
        {0x41,0x42,0x43,0x44},
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

const unpack_dms_GetDeviceSerialNumbers_t const_unpack_dms_GetDeviceSerialNumbers_t = {
        255,{0x30}, 
        255,{0x30,0x30,0x31,0x30,0x32,0x37,0x30,0x30,0x39,0x39,0x39,0x39,0x39,0x39,0x39},
        255,{0x31,0x32,0x33,0x34},
        255,{0x41,0x42,0x43,0x44},0,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };


/* eQMI_DMS_GET_HARDWARE_REV */
const unpack_dms_GetHardwareRevision_t const_unpack_dms_GetHardwareRevision_t = {
        {0x31,0x30,0x30,0x30,0x31},{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

const unpack_dms_GetDeviceHardwareRev_t const_unpack_dms_GetDeviceHardwareRev_t = {
        255,{0x31,0x30,0x30,0x30,0x31},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_GET_BAND_CAPS */
const unpack_dms_SLQSGetBandCapability_t const_unpack_dms_SLQSGetBandCapability_t = {
     0x100600000fc00000ULL,1, 0x00002700ba0e19dfULL,1,0x0000000000000000ULL,
      {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_DMS_GET_BAND_CAPS */
const unpack_dms_GetBandCapability_t const_unpack_dms_GetBandCapability_t = {
     0x100600000fc00000ULL,0,
      {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_DMS_GET_CAPS */
const unpack_dms_GetDeviceCapabilities_t const_unpack_dms_GetDeviceCapabilities_t = {
     0x08f0d180,0x23c34600,4,2,2,{5,8},
      {{SWI_UINT256_BIT_VALUE(SET_14_BITS,1,2,16,17,18,19,20,21,22,23,24,25,26,27)}} };

uint32_t                         var_pDevSrvCaps;
uint64_t                         var_DevVoiceCaps;
uint64_t                         var_DevVoiceDataCaps;
dms_devMultiSimCaps              var_DevMultiSimCaps;
dms_devMultiSimVoiceDataCaps     var_DevMultiSimVoiceDataCaps;
dms_devCurSubsCaps               var_DevCurSubsCaps;
dms_devSubsVoiceDataCaps         var_DevSubsVoiceDataCaps;
dms_devSubsFeatureModeCaps       var_DevSubsFeatureModeCaps;
uint8_t                          var_DevMaxActDataSubsCaps;
dms_devMaxSubsCaps               var_DevMaxSubsCaps;
dms_devMaxCfgListCaps            var_DevMaxCfgListCaps;
int16_t                          var_DevExplicitCfgIndex;

unpack_dms_GetDeviceCapabilitiesV2_t var_unpack_dms_GetDeviceCapabilitiesV2_t = {
     {0,0,0,0,0,{0}},
     &var_pDevSrvCaps, &var_DevVoiceCaps, &var_DevVoiceDataCaps, &var_DevMultiSimCaps,
     &var_DevMultiSimVoiceDataCaps, &var_DevCurSubsCaps, &var_DevSubsVoiceDataCaps,
     &var_DevSubsFeatureModeCaps, &var_DevMaxActDataSubsCaps, &var_DevMaxSubsCaps,
     &var_DevMaxCfgListCaps,&var_DevExplicitCfgIndex,0,
     {{0}} };

uint32_t                         cst_pDevSrvCaps = 0x00000004;
uint64_t                         cst_DevVoiceCaps= 0x0000000000000001ULL;
uint64_t                         cst_DevVoiceDataCaps = 0x0000000000000000ULL;
dms_devMultiSimCaps              cst_DevMultiSimCaps =
                  {1,1,{{1,1,{0x0000000000000030ULL}}}};
dms_devMultiSimVoiceDataCaps     cst_DevMultiSimVoiceDataCaps = {1,1};
dms_devCurSubsCaps               cst_DevCurSubsCaps = {1,{0x0000000000000030ULL}};
dms_devSubsVoiceDataCaps         cst_DevSubsVoiceDataCaps = {1,{{1,0}}};
dms_devSubsFeatureModeCaps       cst_DevSubsFeatureModeCaps = {1,{0}};
uint8_t                          cst_DevMaxActDataSubsCaps = 1;
dms_devMaxSubsCaps               cst_DevMaxSubsCaps = {1,{0x0000000000000030ULL}};;
dms_devMaxCfgListCaps            cst_DevMaxCfgListCaps = {1,1,1,{{1,{0x0000000000000030ULL}}},0};
int16_t                          cst_DevExplicitCfgIndex = 0xFFFF;

const unpack_dms_GetDeviceCapabilitiesV2_t const_unpack_dms_GetDeviceCapabilitiesV2_t = {
     {0x08f0d180,0x23c34600,4,2,2,{5,8}},
     &cst_pDevSrvCaps, &cst_DevVoiceCaps, &cst_DevVoiceDataCaps, &cst_DevMultiSimCaps,
     &cst_DevMultiSimVoiceDataCaps, &cst_DevCurSubsCaps, &cst_DevSubsVoiceDataCaps,
     &cst_DevSubsFeatureModeCaps, &cst_DevMaxActDataSubsCaps, &cst_DevMaxSubsCaps,
     &cst_DevMaxCfgListCaps,&cst_DevExplicitCfgIndex,0,
     {{SWI_UINT256_BIT_VALUE(SET_14_BITS,1,2,16,17,18,19,20,21,22,23,24,25,26,27)}}  };

/* eQMI_DMS_GET_REV_ID */
const unpack_dms_GetFirmwareRevisions_t const_unpack_dms_GetFirmwareRevisions_t = {
        255,{0x53,0x57,0x49,0x39,0x58,0x35,0x30},
        255,{0x43,0x5f,0x30,0x31,0x32,0x30,0x37},
        255,{0x30,0x33,0x33,0x30,0x30,0x30,0x35},
        0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

const unpack_dms_GetFirmwareRevision_t const_unpack_dms_GetFirmwareRevision_t = {
        255,{0x53,0x57,0x49,0x39,0x58,0x35,0x30},        
        {0x30,0x33,0x33,0x30,0x30,0x30,0x35},
        0,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_DMS_GET_CURRENT_PRL_INFO */
const unpack_dms_GetPRLVersion_t const_unpack_dms_GetPRLVersion_t = {
     1,2,0,
      {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_DMS_GET_TIME */
const unpack_dms_GetNetworkTime_t const_unpack_dms_GetNetworkTime_t = {
     0,0x000000000848d553ULL,0,
      {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

uint64_t var_SysTime;
uint64_t var_UsrTime;
unpack_dms_GetNetworkTimeV2_t var_unpack_dms_GetNetworkTimeV2_t = {
     0,0,&var_SysTime, &var_UsrTime,0,{{0}} };

uint64_t cst_SysTime = 0x000000000a5b0aa9ULL;
uint64_t cst_UsrTime = 0x000000000a5b0aa9ULL;
const unpack_dms_GetNetworkTimeV2_t const_unpack_dms_GetNetworkTimeV2_t = {
     0,0x000000000848d553ULL,&cst_SysTime, &cst_UsrTime,0,
    {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };


/* eQMI_DMS_GET_MSISDN */
const unpack_dms_GetVoiceNumber_t const_unpack_dms_GetVoiceNumber_t = {
     255,{0x39,0x32,0x33,0x35,0x37,0x39,0x30,0x33,0x36,0x34},
     255,{0x39,0x30,0x33,0x36,0x34},0,
      {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_DMS_GET_FSN */
const unpack_dms_GetFSN_t const_unpack_dms_GetFSN_t = {
        {0x31,0x30,0x30,0x30,0x31},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SET_OPERATING_MODE */
const unpack_dms_SetPower_t const_unpack_dms_SetPower_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_USB_COMP */
const unpack_dms_GetUSBComp_t const_unpack_dms_GetUSBComp_t = {
        2,3,{1,2,3},0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_DMS_SWI_SET_USB_COMP */
const unpack_dms_SetUSBComp_t const_unpack_dms_SetUSBComp_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_CUST_FEATURES */
const unpack_dms_GetCustFeature_t const_unpack_dms_GetCustFeature_t = {
        1,1,1,1,1,1,1,1,1,0,
        {{SWI_UINT256_BIT_VALUE(SET_12_BITS,2,16,17,18,19,20,21,22,23,24,32,33)}} };

DMSgetCustomInput var_GetCustomInput;
DMScustSettingInfo var_CustSettingInfo;
DMScustSettingList var_CustSettingList;

unpack_dms_GetCustFeaturesV2_t var_unpack_dms_GetCustFeaturesV2_t = {
        {&var_GetCustomInput,&var_CustSettingInfo,&var_CustSettingList},
        0,{{0}} };

DMSgetCustomInput cst_GetCustomInput;
DMScustSettingInfo cst_CustSettingInfo =
           {0x000a,{0x57,0x41,0x4b,0x45,0x48,0x4f,0x53,0x54,0x45,0x4e},
            0x0001,{1},1};
DMScustSettingList cst_CustSettingList =
           {0,3,{{9,{0x47,0x50,0x53,0x45,0x4e,0x41,0x42,0x4c,0x45},1,{0},1},
                 {6,{0x47,0x50,0x53,0x4c,0x50,0x4d},1,{0},1},
                 {0x0d,{0x47,0x50,0x49,0x4f,0x53,0x41,0x52,0x45,0x4e,0x41,0x42,0x4c,0x45},1,{0},1}}};

const unpack_dms_GetCustFeaturesV2_t const_unpack_dms_GetCustFeaturesV2_t = {
         {&var_GetCustomInput,&var_CustSettingInfo,&var_CustSettingList},0,
        {{SWI_UINT256_BIT_VALUE(SET_12_BITS,2,16,17,18,19,20,21,22,23,24,32,33)}} };

/* eQMI_DMS_SWI_SET_CUST_FEATURES */
const unpack_dms_SetCustFeature_t const_unpack_dms_SetCustFeature_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

const unpack_dms_SetCustFeaturesV2_t const_unpack_dms_SetCustFeaturesV2_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SET_FIRMWARE_PREF */
const unpack_dms_SetFirmwarePreference_t const_unpack_dms_SetFirmwarePreference_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_CRASH_ACTION */
const unpack_dms_GetCrashAction_t const_unpack_dms_GetCrashAction_t = {
        1,0,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SWI_SET_CRASH_ACTION */
const unpack_dms_SetCrashAction_t const_unpack_dms_SetCrashAction_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_GET_MANUFACTURER */
const unpack_dms_GetDeviceMfr_t const_unpack_dms_GetDeviceMfr_t = {
        255,
        {0x53,0x69,0x65,0x72,0x72,0x61,0x20, 0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
         0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,0x6f,0x72,0x61,0x74,0x65,0x64},0,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

const unpack_dms_GetManufacturer_t const_unpack_dms_GetManufacturer_t = {
         {0x53,0x69,0x65,0x72,0x72,0x61,0x20, 0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73,
         0x2c,0x20,0x49,0x6e,0x63,0x6f,0x72,0x70,0x6f,0x72,0x61,0x74,0x65,0x64},0,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SET_EVENT */
const unpack_dms_SetEventReport_t const_unpack_dms_SetEventReport_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

const unpack_dms_SetEventReport_ind_t const_unpack_dms_SetEventReport_ind_t = {
        {1,1},{1,1},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,19,20)}} };

/* eQMI_DMS_UIM_GET_ICCID */
const unpack_dms_UIMGetICCID_t const_unpack_dms_UIMGetICCID_t = {
        255,
        {0x39,0x38,0x31,0x30,0x35,0x33,0x39,0x35,0x36,0x31,0x32,0x31,0x30,0x34},0,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_GET_ACTIVATED_STATE */
const unpack_dms_GetActivationState_t const_unpack_dms_GetActivationState_t = {
        4,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SWI_GET_FIRMWARE_CURR */
image_info_t var_CurrImgInfo[2];

unpack_dms_SLQSSwiGetFirmwareCurr_t var_unpack_dms_SLQSSwiGetFirmwareCurr_t = {
        2, var_CurrImgInfo, {0},{0},{0},{0},{{0}} };

image_info_t cst_CurrImgInfo[2] = {
       {0,{0x30,0x30,0x30,0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},
         16,{0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,0x4e,0x4f,0x4e,0x45}},
       {1,{0x30,0x30,0x30,0x2e,0x30,0x30,0x30,0x5f,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},
         16,{0x30,0x31,0x2e,0x30,0x37,0x2e,0x30,0x33,0x2e,0x30,0x30,0x5f,0x4e,0x4f,0x4e,0x45}}};

const unpack_dms_SLQSSwiGetFirmwareCurr_t const_unpack_dms_SLQSSwiGetFirmwareCurr_t = {
         2, cst_CurrImgInfo, "000.000","000","01.07.03.00","000.000",
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SWI_SET_DYING_GASP_CFG */
const unpack_dms_SLQSSwiSetDyingGaspCfg_t const_unpack_dms_SLQSSwiSetDyingGaspCfg_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_CLR_DYING_GASP_STAT */
const unpack_dms_SLQSSwiClearDyingGaspStatistics_t const_unpack_dms_SLQSSwiClearDyingGaspStatistics_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_DYING_GASP_STAT */
uint32_t var_TimeStamp;
uint8_t  var_SMSAttemptedFlag;
packgetDyingGaspStatistics var_GetDyingGaspStatistics = {&var_TimeStamp,&var_SMSAttemptedFlag};

unpack_dms_SLQSSwiGetDyingGaspStatistics_t var_unpack_dms_SLQSSwiGetDyingGaspStatistics_t = {
        &var_GetDyingGaspStatistics, 0,{{0}} };

uint32_t cst_TimeStamp = 0x0201;
uint8_t  cst_SMSAttemptedFlag = 1;
packgetDyingGaspStatistics cst_GetDyingGaspStatistics = {&cst_TimeStamp,&cst_SMSAttemptedFlag};

const unpack_dms_SLQSSwiGetDyingGaspStatistics_t const_unpack_dms_SLQSSwiGetDyingGaspStatistics_t = {
         &cst_GetDyingGaspStatistics,0,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_DMS_SWI_GET_DYING_GASP_CFG  */
uint8_t var_DestSMSNum[21] ;
uint8_t  var_DestSMSContent[161];
packgetDyingGaspCfg var_GetDyingGaspCfg = {var_DestSMSNum,var_DestSMSContent};

unpack_dms_SLQSSwiGetDyingGaspCfg_t var_unpack_dms_SLQSSwiGetDyingGaspCfg_t = {
        &var_GetDyingGaspCfg, 0,{{0}} };

uint8_t cst_DestSMSNum[21] = {0x2b,0x38,0x35,0x32,0x32,0x34,0x33,0x33,0x35,0x34,0x33,0x33};
uint8_t  cst_DestSMSContent[161] =
    {0x32,0x33,0x34,0x35,0x36,0x68,0x65,0x6c,0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,
      0x31,0x21};
packgetDyingGaspCfg cst_GetDyingGaspCfg = {cst_DestSMSNum,cst_DestSMSContent};

const unpack_dms_SLQSSwiGetDyingGaspCfg_t const_unpack_dms_SLQSSwiGetDyingGaspCfg_t = {
         &cst_GetDyingGaspCfg,0,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_DMS_SWI_GET_RESET_INFO */
const unpack_dms_SLQSDmsSwiGetResetInfo_t const_unpack_dms_SLQSDmsSwiGetResetInfo_t = {
        2,1,0,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

const unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t const_unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t = {
        2,1,0,
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_DMS_SWI_INDICATION_REGISTER */
const unpack_dms_SLQSDmsSwiIndicationRegister_t const_unpack_dms_SLQSDmsSwiIndicationRegister_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_FW_UPDATE_STAT */
const unpack_dms_SLQSSwiGetFwUpdateStatus_t const_unpack_dms_SLQSSwiGetFwUpdateStatus_t = {
        0xffffffff,1,4,{0x41,0x42,0x43,0x44,0x45,0x46},{0},0,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

/* eQMI_DMS_UIM_SET_PIN_PROT */
const unpack_dms_UIMSetPINProtection_t const_unpack_dms_UIMSetPINProtection_t = {
        3,3,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_DMS_UIM_GET_CK_STATUS */
const unpack_dms_UIMGetControlKeyStatus_t const_unpack_dms_UIMGetControlKeyStatus_t = {
        1,3,3,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_UIM_GET_PIN_STATUS */
const unpack_dms_UIMGetPINStatus_t const_unpack_dms_UIMGetPINStatus_t = {
        1,2,2,1,4,4,0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,17,18)}} };

/* eQMI_DMS_UIM_SET_CK_PROT */
const unpack_dms_UIMSetControlKeyProtection_t const_unpack_dms_UIMSetControlKeyProtection_t = {
        2,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_DMS_UIM_UNBLOCK_CK */
const unpack_dms_UIMUnblockControlKey_t const_unpack_dms_UIMUnblockControlKey_t = {
        3,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_DMS_FACTORY_DEFAULTS */
const unpack_dms_ResetToFactoryDefaults_t const_unpack_dms_ResetToFactoryDefaults_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_READ_ERI_FILE */
const unpack_dms_SLQSGetERIFile_t const_unpack_dms_SLQSGetERIFile_t = {
        {18,{0x41,0x44,0x45,0x46,0x41,0x44,0x45,0x46,0x41,0x44,0x45,0x46,0x41,
          0x44,0x45,0x46,0x41,0x41}},0,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_UIM_GET_STATE */
const unpack_dms_SLQSUIMGetState_t const_unpack_dms_SLQSUIMGetState_t = {
        1,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SWI_GET_CRASH_INFO */
const unpack_dms_SLQSSwiGetCrashInfo_t const_unpack_dms_SLQSSwiGetCrashInfo_t = {
        0,
        {1, {0x8302,0x00118624,0x0f000000,0x000f,
         {0x41,0x50,0x50,0x53,0x20,0x66,0x6f,
          0x72,0x63,0x65,0x20,0x73,0x74,0x6f,0x70},
         0x00ff,
         {0xc3,0x03,0x0a,0x53,0x72,0x63,
            0x3a,0x20,0x20,0x46,0x61,0x74,0x61,0x6c,
            0x45,0x72,0x72,0x6f,0x72,0x0a,0x46,0x69,
            0x6c,0x65,0x3a,0x20,0x73,0x79,0x73,0x5f,
            0x6d,0x5f,0x73,0x6d,0x73,0x6d,0x5f,0x6d,
            0x70,0x73,0x73,0x2e,0x63,0x0a,0x4c,0x69,
            0x6e,0x65,0x3a,0x20,0x30,0x30,0x30,0x30,
            0x30,0x31,0x31,0x44,0x0a,0x53,0x74,0x72,
            0x3a,0x20,0x20,0x41,0x50,0x50,0x53,0x20,
            0x66,0x6f,0x72,0x63,0x65,0x20,0x73,0x74,
            0x6f,0x70,0x0a,0x30,0x30,0x30,0x30,0x30,
            0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,
            0x30,0x30,0x30,0x30,0x20,0x30,0x30,0x30,
            0x30,0x30,0x30,0x30,0x30,0x20,0x30,0x30,
            0x30,0x30,0x30,0x30,0x30,0x30,0x0a,0x50,
            0x72,0x63,0x3a,0x20,0x20,0x4d,0x50,0x53,
            0x53,0x0a,0x54,0x61,0x73,0x6b,0x3a,0x20,
            0x41,0x4d,0x53,0x53,0x0a,0x54,0x69,0x6d,
            0x65,0x3a,0x20,0x30,0x30,0x36,0x46,0x38,
            0x37,0x38,0x42,0x0a,0x20,0x52,0x30,0x3a,
            0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
            0x30,0x20,0x20,0x52,0x31,0x3a,0x20,0x43,
            0x30,0x30,0x34,0x36,0x31,0x33,0x30,0x20,
            0x20,0x52,0x32,0x3a,0x20,0x45,0x42,0x31,
            0x30,0x32,0x30,0x30,0x30,0x20,0x20,0x52,
            0x33,0x3a,0x20,0x44,0x38,0x35,0x32,0x45,
            0x42,0x36,0x30,0x20,0x20,0x52,0x34,0x3a,
            0x20,0x30,0x30,0x30,0x30,0x30,0x30,0x46,
            0x45,0x0a,0x20,0x52,0x35,0x3a,0x20,0x43,
            0x33,0x36,0x41,0x38,0x41,0x36,0x34,0x20,
            0x20,0x52,0x36,0x3a,0x20,0x30,0x30,0x30,
            0x30,0x30,0x30,0x30,0x31,0x20,0x20,0x52,0x37}}},
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_DMS_SWI_GET_HOST_DEV_INFO */
const unpack_dms_SLQSSwiGetHostDevInfo_t const_unpack_dms_SLQSSwiGetHostDevInfo_t = {
        0,{0x53,0x69,0x65,0x72,0x72,0x61,0x20,0x57,0x69,0x72,0x65,0x6c,0x65,0x73,0x73},
        {0x4d,0x47,0x39,0x30},{0x34},{0},
        {0x31,0x30,0x54,0x6b,0x68,0x69,0x32,0x61,0x34,0x67},
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,20)}} };

/* eQMI_DMS_SWI_SET_HOST_DEV_INFO */
const unpack_dms_SLQSSwiSetHostDevInfo_t const_unpack_dms_SLQSSwiSetHostDevInfo_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_OS_INFO */
const unpack_dms_SLQSSwiGetOSInfo_t const_unpack_dms_SLQSSwiGetOSInfo_t = {
        0,{0x4c,0x69,0x6e,0x75,0x78},
        {0x55,0x62,0x75,0x6e,0x74,0x75,0x20,0x31,0x34,0x2e,0x30,0x34},
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_DMS_SWI_SET_OS_INFO */
const unpack_dms_SLQSSwiSetOSInfo_t const_unpack_dms_SLQSSwiSetOSInfo_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_SERIAL_NO_EXT */
const unpack_dms_SLQSSwiGetSerialNoExt_t const_unpack_dms_SLQSSwiGetSerialNoExt_t = {
        0,
        {0x53,0x69,0x65,0x72,0x72,0x61},
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_DMS_SWI_GET_PC_INFO */
const unpack_dms_SLQSDmsSwiGetPCInfo_t const_unpack_dms_SLQSDmsSwiGetPCInfo_t = {
        1,1,0x00000022,1,0,0,0,1,0,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,19)}} };

/* eQMI_DMS_PSM_GET_CFG_PARAMS */
dms_PSMEnableStateTlv         var_PsmEnableState;
dms_PSMDurationThresholdTlv   var_DurationThreshold;
dms_PSMDurationDueToOOSTlv    var_DurationDueToOOS;
dms_PSMRandomizationWindowTlv var_RandomizationWindow;
dms_PSMActiveTimerTlv         var_ActiveTimer;
dms_PSMPeriodicUpdateTimerTlv var_PeriodicUpdateTimer;
dms_PSMEarlyWakeupTimeTlv     var_EarlyWakeupTime;

unpack_dms_SLQSGetPowerSaveModeConfig_t var_unpack_dms_SLQSGetPowerSaveModeConfig_t = {
        &var_PsmEnableState, &var_DurationThreshold, &var_DurationDueToOOS,
        &var_RandomizationWindow, &var_ActiveTimer, &var_PeriodicUpdateTimer,
        &var_EarlyWakeupTime,{{0}} };

dms_PSMEnableStateTlv         cst_PsmEnableState = {1,1};
dms_PSMDurationThresholdTlv   cst_DurationThreshold = {1,0x0201};
dms_PSMDurationDueToOOSTlv    cst_DurationDueToOOS = {1,0x0403};
dms_PSMRandomizationWindowTlv cst_RandomizationWindow = {1,0x0605};
dms_PSMActiveTimerTlv         cst_ActiveTimer = {1,0x0807};
dms_PSMPeriodicUpdateTimerTlv cst_PeriodicUpdateTimer = {1,0x2211};
dms_PSMEarlyWakeupTimeTlv     cst_EarlyWakeupTime = {1,0x2221};;

unpack_dms_SLQSGetPowerSaveModeConfig_t const_unpack_dms_SLQSGetPowerSaveModeConfig_t = {
        &cst_PsmEnableState, &cst_DurationThreshold, &cst_DurationDueToOOS,
        &cst_RandomizationWindow, &cst_ActiveTimer, &cst_PeriodicUpdateTimer,
        &cst_EarlyWakeupTime,{{SWI_UINT256_BIT_VALUE(SET_8_BITS,2,16,17,18,19,20,21,22)}} };

/* eQMI_DMS_PSM_SET_CFG_PARAMS */
const unpack_dms_SLQSSetPowerSaveModeConfig_t const_unpack_dms_SLQSSetPowerSaveModeConfig_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_INDICATION_REGISTER */
const unpack_dms_SetIndicationRegister_t const_unpack_dms_SetIndicationRegister_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_PSM_CFG_PARAMS_CHANGE_IND */
const unpack_dms_PSMCfgChange_ind_t const_unpack_dms_PSMCfgChange_ind_t = {
        {1,1},{1,0x0201},{1,0x0403},0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,16,17,18)}} };

/* eQMI_DMS_SWI_SET_EVENT_REPORT */
const unpack_dms_SwiSetEventReport_t const_unpack_dms_SwiSetEventReport_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

const unpack_dms_SwiEventReportCallBack_ind_t const_unpack_dms_SwiEventReportCallBack_ind_t = {
        {1,1,0x001e},
        {1,1,0x0d01},
        {1,0,0},
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,16,17,19)}} };

/* eQMI_DMS_SWI_UIM_SELECT */
const unpack_dms_SwiUimSelect_t const_unpack_dms_SwiUimSelect_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_DMS_SWI_GET_UIM_SELECTION */
dms_UimAutoSwitchActSlotTlv var_UimAutoSwitchActSlot;

unpack_dms_SLQSDmsSwiGetUimSelection_t var_unpack_dms_SLQSDmsSwiGetUimSelection_t = {
        0,&var_UimAutoSwitchActSlot,{{0}} };

dms_UimAutoSwitchActSlotTlv cst_UimAutoSwitchActSlot ={1,1};
const unpack_dms_SLQSDmsSwiGetUimSelection_t const_unpack_dms_SLQSDmsSwiGetUimSelection_t = {
         1, &cst_UimAutoSwitchActSlot,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_DMS_GET_BAND_CAPS */
const unpack_dms_SLQSGetBandCapabilityExt_t const_unpack_dms_SLQSGetBandCapabilityExt_t = {
        0x2000000400180ULL,1, 0x80800C5ULL,1,0x0000000000000000ULL,{0x06,{ 0x01,0x03,0x07,0x08,0x14,0x01c},1},
      {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)}} };


int dms_validate_dummy_unpack()
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
    loopCount = sizeof(validate_dms_resp_msg)/sizeof(validate_dms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_dms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_DMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("DMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("DMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_DMS_GET_MODEL_ID:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetModelID,
                        dump_GetModelId,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetModelID_t);
                }
                break;
            case eQMI_DMS_GET_IMSI:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetIMSI,
                        dump_GetIMSI,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetIMSI_t);
                }
                break;
            case eQMI_DMS_GET_CWE_SPKGS_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetFirmwareInfo,
                        dump_GetFirmwareInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetFirmwareInfo_t);
                }
                break;
            case eQMI_DMS_GET_OPERATING_MODE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_GetOfflineReason_t *varp = &var_unpack_dms_GetOfflineReason_t;
                    const unpack_dms_GetOfflineReason_t *cstp = &const_unpack_dms_GetOfflineReason_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_GetOfflineReason,
                    dump_GetOfflineReason,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_GetOfflineReason_t,
                    3,
                    CMP_PTR_TYPE, varp->pReasonMask, cstp->pReasonMask,
                    CMP_PTR_TYPE, varp->pbPlatform, cstp->pbPlatform,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetPower,
                        dump_GetPower,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetPower_t);
                }
                break;
            case eQMI_DMS_GET_IDS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetSerialNumbers,
                        dump_GetSerialNumbers,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetSerialNumbers_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetDeviceSerialNumbers,
                        dump_GetDeviceSerialNumbers,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetDeviceSerialNumbers_t);
                }
                break;
            case eQMI_DMS_GET_HARDWARE_REV:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetHardwareRevision,
                        dump_GetHardwareRevision,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetHardwareRevision_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetDeviceHardwareRev,
                        dump_GetDeviceHardwareRev,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetDeviceHardwareRev_t);
                }
                break;
            case eQMI_DMS_GET_BAND_CAPS:
                if (eRSP == rsp_ctx.type)
                {
                    if(rsp_ctx.xid == 0x36)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_dms_SLQSGetBandCapabilityExt,
                            dump_SLQSGetBandCapabilityExt,
                            msg.buf,
                            rlen,
                            &const_unpack_dms_SLQSGetBandCapabilityExt_t);
                    }
                    else
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_dms_SLQSGetBandCapability,
                            dump_SLQSGetBandCapability,
                            msg.buf,
                            rlen,
                            &const_unpack_dms_SLQSGetBandCapability_t);

                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_dms_GetBandCapability,
                            dump_GetBandCapability,
                            msg.buf,
                            rlen,
                            &const_unpack_dms_GetBandCapability_t);
                    }
                }
                break;
            case eQMI_DMS_GET_CAPS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_GetDeviceCapabilitiesV2_t *varp = &var_unpack_dms_GetDeviceCapabilitiesV2_t;
                    const unpack_dms_GetDeviceCapabilitiesV2_t *cstp = &const_unpack_dms_GetDeviceCapabilitiesV2_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_GetDeviceCapabilitiesV2,
                    dump_GetDeviceCapabilitiesV2,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_GetDeviceCapabilitiesV2_t,
                    15,
                    CMP_PTR_TYPE, &varp->DevCaps, &cstp->DevCaps,
                    CMP_PTR_TYPE, varp->pDevSrvCaps, cstp->pDevSrvCaps,
                    CMP_PTR_TYPE, varp->pDevVoiceCaps, cstp->pDevVoiceCaps,
                    CMP_PTR_TYPE, varp->pDevVoiceDataCaps, cstp->pDevVoiceDataCaps,
                    CMP_PTR_TYPE, varp->pDevMultiSimCaps, cstp->pDevMultiSimCaps,
                    CMP_PTR_TYPE, varp->pDevMultiSimVoiceDataCaps, cstp->pDevMultiSimVoiceDataCaps,
                    CMP_PTR_TYPE, varp->pDevCurSubsCaps, cstp->pDevCurSubsCaps,
                    CMP_PTR_TYPE, varp->pDevSubsVoiceDataCaps, cstp->pDevSubsVoiceDataCaps,
                    CMP_PTR_TYPE, varp->pDevSubsFeatureModeCaps, cstp->pDevSubsFeatureModeCaps,
                    CMP_PTR_TYPE, varp->pDevMaxActDataSubsCaps, cstp->pDevMaxActDataSubsCaps,
                    CMP_PTR_TYPE, varp->pDevMaxSubsCaps, cstp->pDevMaxSubsCaps,
                    CMP_PTR_TYPE, varp->pDevMaxCfgListCaps, cstp->pDevMaxCfgListCaps,
                    CMP_PTR_TYPE, varp->pDevExplicitCfgIndex, cstp->pDevExplicitCfgIndex,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetDeviceCapabilities,
                        dump_GetDeviceCapabilities,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetDeviceCapabilities_t);
                }
                break;
            case eQMI_DMS_GET_REV_ID:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetFirmwareRevisions,
                        dump_FirmwareRevisions,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetFirmwareRevisions_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetFirmwareRevision,
                        dump_FirmwareRevision,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetFirmwareRevision_t);
                }
                break;
            case eQMI_DMS_GET_CURRENT_PRL_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetPRLVersion,
                        dump_GetPRLVersion,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetPRLVersion_t);
                }
                break;
           case eQMI_DMS_GET_TIME:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_GetNetworkTimeV2_t *varp = &var_unpack_dms_GetNetworkTimeV2_t;
                    const unpack_dms_GetNetworkTimeV2_t *cstp = &const_unpack_dms_GetNetworkTimeV2_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_GetNetworkTimeV2,
                    dump_GetNetworkTimeV2,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_GetNetworkTimeV2_t,
                    6,
                    CMP_PTR_TYPE, &varp->source, &cstp->source,
                    CMP_PTR_TYPE, &varp->timestamp, &cstp->timestamp,
                    CMP_PTR_TYPE, varp->pSysTime, cstp->pSysTime,
                    CMP_PTR_TYPE, varp->pUsrTime, cstp->pUsrTime,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetNetworkTime,
                        dump_GetNetworkTime,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetNetworkTime_t);
                }
                break;
           case eQMI_DMS_GET_MSISDN:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetVoiceNumber,
                        dump_GetVoiceNumber,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetVoiceNumber_t);
                }
                break;
           case eQMI_DMS_GET_FSN:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetFSN,
                        dump_GetFSN,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetFSN_t);
                }
                break;
           case eQMI_DMS_SET_OPERATING_MODE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetPower,
                        dump_SetPower,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetPower_t);
                }
                break;
           case eQMI_DMS_SWI_GET_USB_COMP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetUSBComp,
                        dump_GetUSBComp,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetUSBComp_t);
                }
                break;
           case eQMI_DMS_SWI_SET_USB_COMP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetUSBComp,
                        dump_SetUSBComp,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetUSBComp_t);
                }
                break;
           case eQMI_DMS_SWI_GET_CUST_FEATURES:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_GetCustFeaturesV2_t *varp = &var_unpack_dms_GetCustFeaturesV2_t;
                    const unpack_dms_GetCustFeaturesV2_t *cstp = &const_unpack_dms_GetCustFeaturesV2_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_GetCustFeaturesV2,
                    dump_GetCustFeaturesV2,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_GetCustFeaturesV2_t,
                    3,
                    CMP_PTR_TYPE, varp->GetCustomFeatureV2.pCustSettingInfo, cstp->GetCustomFeatureV2.pCustSettingInfo,
                    CMP_PTR_TYPE, varp->GetCustomFeatureV2.pCustSettingList, cstp->GetCustomFeatureV2.pCustSettingList,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetCustFeature,
                        dump_GetCustFeature,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetCustFeature_t);

                }
                break;
           case eQMI_DMS_SWI_SET_CUST_FEATURES:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetCustFeature,
                        dump_SetCustFeature,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetCustFeature_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetCustFeaturesV2,
                        dump_SetCustFeaturesV2,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetCustFeaturesV2_t);
                }
                break;
           case eQMI_DMS_SET_FIRMWARE_PREF:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetFirmwarePreference,
                        dump_SetFirmwarePreference,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetFirmwarePreference_t);
                }
                break;
           case eQMI_DMS_SWI_GET_CRASH_ACTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetCrashAction,
                        dump_GetCrashAction,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetCrashAction_t);
                }
                break;
           case eQMI_DMS_SWI_SET_CRASH_ACTION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetCrashAction,
                        dump_SetCrashAction,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetCrashAction_t);
                }
                break;
           case eQMI_DMS_GET_MANUFACTURER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetDeviceMfr,
                        dump_GetDeviceMfr,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetDeviceMfr_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetManufacturer,
                        dump_GetManufacturer,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetManufacturer_t);
                }
                break;
           case eQMI_DMS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetEventReport,
                        dump_SetEventReport,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetEventReport_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SetEventReport_ind,
                    dump_SetEventReport_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_dms_SetEventReport_ind_t);
                }
                break;
           case eQMI_DMS_UIM_GET_ICCID:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMGetICCID,
                        dump_UIMGetICCID,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMGetICCID_t);
                }
                break;
           case eQMI_DMS_GET_ACTIVATED_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_GetActivationState,
                        dump_GetActivationState,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_GetActivationState_t);
                }
                break;
           case eQMI_DMS_SWI_GET_FIRMWARE_CURR:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_SLQSSwiGetFirmwareCurr_t *varp = &var_unpack_dms_SLQSSwiGetFirmwareCurr_t;
                    const unpack_dms_SLQSSwiGetFirmwareCurr_t *cstp = &const_unpack_dms_SLQSSwiGetFirmwareCurr_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSSwiGetFirmwareCurr,
                    dump_SLQSSwiGetFirmwareCurr,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_SLQSSwiGetFirmwareCurr_t,
                    7,
                    CMP_PTR_TYPE, &varp->numEntries, &cstp->numEntries,
                    CMP_PTR_TYPE, varp->pCurrImgInfo, cstp->pCurrImgInfo,
                    CMP_PTR_TYPE, varp->priver, cstp->priver,
                    CMP_PTR_TYPE, varp->pkgver, cstp->pkgver,
                    CMP_PTR_TYPE, varp->fwvers, cstp->fwvers,
                    CMP_PTR_TYPE, varp->carrier, cstp->carrier,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_DMS_SWI_SET_DYING_GASP_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiSetDyingGaspCfg,
                        dump_SLQSSwiSetDyingGaspCfg,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiSetDyingGaspCfg_t);
                }
                break;
           case eQMI_DMS_SWI_CLR_DYING_GASP_STAT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiClearDyingGaspStatistics,
                        dump_SLQSSwiClearDyingGaspStatistics,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiClearDyingGaspStatistics_t);
                }
                break;
           case eQMI_DMS_SWI_GET_DYING_GASP_STAT:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_SLQSSwiGetDyingGaspStatistics_t *varp = &var_unpack_dms_SLQSSwiGetDyingGaspStatistics_t;
                    const unpack_dms_SLQSSwiGetDyingGaspStatistics_t *cstp = &const_unpack_dms_SLQSSwiGetDyingGaspStatistics_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSSwiGetDyingGaspStatistics,
                    dump_SLQSSwiGetDyingGaspStatistics,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_SLQSSwiGetDyingGaspStatistics_t,
                    3,
                    CMP_PTR_TYPE, varp->pGetDyingGaspStatistics->pTimeStamp, cstp->pGetDyingGaspStatistics->pTimeStamp,
                    CMP_PTR_TYPE, varp->pGetDyingGaspStatistics->pSMSAttemptedFlag, cstp->pGetDyingGaspStatistics->pSMSAttemptedFlag,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_DMS_SWI_GET_DYING_GASP_CFG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_SLQSSwiGetDyingGaspCfg_t *varp = &var_unpack_dms_SLQSSwiGetDyingGaspCfg_t;
                    const unpack_dms_SLQSSwiGetDyingGaspCfg_t *cstp = &const_unpack_dms_SLQSSwiGetDyingGaspCfg_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSSwiGetDyingGaspCfg,
                    dump_SLQSSwiGetDyingGaspCfg,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_SLQSSwiGetDyingGaspCfg_t,
                    3,
                    CMP_PTR_TYPE, varp->pGetDyingGaspCfg->pDestSMSNum, cstp->pGetDyingGaspCfg->pDestSMSNum,
                    CMP_PTR_TYPE, varp->pGetDyingGaspCfg->pDestSMSContent, cstp->pGetDyingGaspCfg->pDestSMSContent,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_DMS_SWI_GET_RESET_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSDmsSwiGetResetInfo,
                        dump_SLQSDmsSwiGetResetInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSDmsSwiGetResetInfo_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSDmsSwiGetResetInfo_Ind,
                    dump_SLQSDmsSwiGetResetInfo_Ind,
                    msg.buf,
                    rlen,
                    &const_unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t);
                }
                break;
           case eQMI_DMS_SWI_INDICATION_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSDmsSwiIndicationRegister,
                        dump_common_resultcode,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSDmsSwiIndicationRegister_t);
                }
                break;
           case eQMI_DMS_SWI_GET_FW_UPDATE_STAT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiGetFwUpdateStatus,
                        dump_SLQSSwiGetFwUpdateStatus,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiGetFwUpdateStatus_t);
                }
                break;
           case eQMI_DMS_UIM_SET_PIN_PROT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMSetPINProtection,
                        dump_UIMSetPINProtection,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMSetPINProtection_t);
                }
                break;
           case eQMI_DMS_UIM_GET_CK_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMGetControlKeyStatus,
                        dump_UIMGetControlKeyStatus,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMGetControlKeyStatus_t);
                }
                break;
           case eQMI_DMS_UIM_GET_PIN_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMGetPINStatus,
                        dump_UIMGetPINStatus,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMGetPINStatus_t);
                }
                break;
           case eQMI_DMS_UIM_SET_CK_PROT :
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMSetControlKeyProtection,
                        dump_UIMSetControlKeyProtection,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMSetControlKeyProtection_t);
                }
                break;
           case eQMI_DMS_UIM_UNBLOCK_CK :
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_UIMUnblockControlKey,
                        dump_UIMUnblockControlKey,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_UIMUnblockControlKey_t);
                }
                break;
           case eQMI_DMS_FACTORY_DEFAULTS :
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_ResetToFactoryDefaults,
                        dump_ResetToFactoryDefaults,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_ResetToFactoryDefaults_t);
                }
                break;
           case eQMI_DMS_READ_ERI_FILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSGetERIFile,
                        dump_SLQSGetERIFile,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSGetERIFile_t);
                }
                break;
           case eQMI_DMS_UIM_GET_STATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSUIMGetState,
                        dump_SLQSUIMGetState,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSUIMGetState_t);
                }
                break;
           case eQMI_DMS_SWI_GET_CRASH_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiGetCrashInfo,
                        dump_SLQSSwiGetCrashInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiGetCrashInfo_t);
                }
                break;
           case eQMI_DMS_SWI_GET_HOST_DEV_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiGetHostDevInfo,
                        dump_SLQSSwiGetHostDevInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiGetHostDevInfo_t);
                }
                break;
           case eQMI_DMS_SWI_SET_HOST_DEV_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiSetHostDevInfo,
                        dump_SLQSSwiSetHostDevInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiSetHostDevInfo_t);
                }
                break;
           case eQMI_DMS_SWI_GET_OS_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiGetOSInfo,
                        dump_SLQSSwiGetOSInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiGetOSInfo_t);
                }
                break;
           case eQMI_DMS_SWI_SET_OS_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiSetOSInfo,
                        dump_SLQSSwiSetOSInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiSetOSInfo_t);
                }
                break;
           case eQMI_DMS_SWI_GET_SERIAL_NO_EXT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSwiGetSerialNoExt,
                        dump_SLQSSwiGetSerialNoExt,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSwiGetSerialNoExt_t);
                }
                break;
           case eQMI_DMS_SWI_GET_PC_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSDmsSwiGetPCInfo,
                        dump_SLQSDmsSwiGetPCInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSDmsSwiGetPCInfo_t);
                }
                break;
           case eQMI_DMS_PSM_GET_CFG_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_SLQSGetPowerSaveModeConfig_t *varp = &var_unpack_dms_SLQSGetPowerSaveModeConfig_t;
                    const unpack_dms_SLQSGetPowerSaveModeConfig_t *cstp = &const_unpack_dms_SLQSGetPowerSaveModeConfig_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSGetPowerSaveModeConfig,
                    dump_SLQSGetPowerSaveModeConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_SLQSGetPowerSaveModeConfig_t,
                    8,
                    CMP_PTR_TYPE, varp->pPsmEnableState, cstp->pPsmEnableState,
                    CMP_PTR_TYPE, varp->pDurationThreshold, cstp->pDurationThreshold,
                    CMP_PTR_TYPE, varp->pDurationDueToOOS, cstp->pDurationDueToOOS,
                    CMP_PTR_TYPE, varp->pRandomizationWindow, cstp->pRandomizationWindow,
                    CMP_PTR_TYPE, varp->pActiveTimer, cstp->pActiveTimer,
                    CMP_PTR_TYPE, varp->pPeriodicUpdateTimer, cstp->pPeriodicUpdateTimer,
                    CMP_PTR_TYPE, varp->pEarlyWakeupTime, cstp->pEarlyWakeupTime,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_DMS_PSM_SET_CFG_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SLQSSetPowerSaveModeConfig,
                        dump_SLQSSetPowerSaveModeConfig,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SLQSSetPowerSaveModeConfig_t);
                }
                break;
           case eQMI_DMS_INDICATION_REGISTER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SetIndicationRegister,
                        dump_common_resultcode,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SetIndicationRegister_t);
                }
                break;
           case eQMI_DMS_PSM_CFG_PARAMS_CHANGE_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_PSMCfgChange_ind,
                    dump_PSMCfgChange_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_dms_PSMCfgChange_ind_t);
                }
                break;
           case eQMI_DMS_SWI_SET_EVENT_REPORT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SwiSetEventReport,
                        dump_SwiSetEventReport,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SwiSetEventReport_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SwiEventReportCallBack_ind,
                    dump_SwiEventReportCallBack_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_dms_SwiEventReportCallBack_ind_t);
                }
                break;
           case eQMI_DMS_SWI_UIM_SELECT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_dms_SwiUimSelect,
                        dump_SwiUimSelect,
                        msg.buf,
                        rlen,
                        &const_unpack_dms_SwiUimSelect_t);
                }
                break;
           case eQMI_DMS_SWI_GET_UIM_SELECTION:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_dms_SLQSDmsSwiGetUimSelection_t *varp = &var_unpack_dms_SLQSDmsSwiGetUimSelection_t;
                    const unpack_dms_SLQSDmsSwiGetUimSelection_t *cstp = &const_unpack_dms_SLQSDmsSwiGetUimSelection_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_dms_SLQSDmsSwiGetUimSelection,
                    dump_SLQSDmsSwiGetUimSelection,
                    msg.buf,
                    rlen,
                    &var_unpack_dms_SLQSDmsSwiGetUimSelection_t,
                    3,
                    CMP_PTR_TYPE, &varp->uimSelect, &cstp->uimSelect,
                    CMP_PTR_TYPE, varp->pUimAutoSwitchActSlot, cstp->pUimAutoSwitchActSlot,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            default:
                    printf("msgid 0x%x Unknown/NotHandled\n", rsp_ctx.msgid);
                break;
            }
        }
    }
    return 0;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
uint8_t validate_fms_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_DMS_GET_FIRMWARE_PREF */
    {0x02,0x01,0x00,0x47,0x00,0x55,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x4B,0x00,0x02,0x00,0x30,0x30,0x32,0x2E,0x30,0x32,0x33,
                0x5F,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x13,0x30,
                0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,0x2E,0x30,0x30,0x5F,
                0x47,0x45,0x4E,0x45,0x52,0x49,0x43,0x01,0x30,0x30,0x32,
                0x2E,0x30,0x32,0x33,0x5F,0x30,0x30,0x30,0x00,0x00,0x00,
                0x00,0x00,0x13,0x30,0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,
                0x2E,0x30,0x30,0x5F,0x47,0x45,0x4E,0x45,0x52,0x49,0x43
    },

    /* eQMI_DMS_LIST_FIRMWARE */
    {0x02,0x02,0x00,0x49,0x00,0x79,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x6F,0x00,0x02,0x00,0x04,0x02,0x02,0x01,0x00,0x3F,0x5F,0x3F,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x0D,0x30,0x31,0x2E,0x30,0x39,0x2E,0x30,0x31,0x2E,0x30,0x30,0x5F,
                0x3F,0x02,0x00,0x3F,0x5F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x30,0x31,0x2E,0x31,0x31,0x2E,
                0x30,0x30,0x2E,0x30,0x30,0x5F,0x3F,0x01,0x32,0x00,0x01,0xFF,0xFF,
                0x30,0x30,0x32,0x2E,0x30,0x32,0x33,0x5F,0x30,0x30,0x30,0x00,0x00,
                0x00,0x00,0x00,0x13,0x30,0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,0x2E,
                0x30,0x30,0x5F,0x47,0x45,0x4E,0x45,0x52,0x49,0x43
    },

    /* eQMI_DMS_SET_FIRMWARE_PREF */
    {0x02,0x03,0x00,0x48,0x00,0x10,0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x01,0x06,0x00,0x05,0x01,0x02,0x03,0x04,0x05
    },

};


FMSPrefImageList cst_ImageList = {
        0x02,
        {
            {0x00,
            {0x30,0x30,0x32,0x2E,0x30,0x32,0x33,0x5F,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},
            0x13,
            {0x30,0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,0x2E,0x30,0x30,0x5F,0x47,0x45,0x4E,0x45,0x52,0x49,0x43}},
            {0x01,
            {0x30,0x30,0x32,0x2E,0x30,0x32,0x33,0x5F,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},
            0x13,
            {0x30,0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,0x2E,0x30,0x30,0x5F,0x47,0x45,0x4E,0x45,0x52,0x49,0x43}},
        }};



/* eQMI_DMS_GET_FIRMWARE_PREF  */
const unpack_fms_GetImagesPreference_t const_unpack_fms_GetImagesPreference_t = {
        0x004B,&cst_ImageList,0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

FMSPrefImageList var_ImageList;

unpack_fms_GetImagesPreference_t var_unpack_fms_GetImagesPreference_t = {
        0xFF,&var_ImageList,0,{{0}} };

/* eQMI_DMS_LIST_FIRMWARE  */
const unpack_fms_GetStoredImages_t const_unpack_fms_GetStoredImages_t = {
        0x6F,{
                0x02,
                {
                    {0x00,0x04,0x02,0x02,
                        {{0x01,0x00,{0x3f,0x5f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                            0x0D,{0x30,0x31,0x2E,0x30,0x39,0x2E,0x30,0x31,0x2E,0x30,0x30,0x5F,
                            0x3F}},
                    {0x02,0x00,{0x3F,0x5F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                            0x0D,{0x30,0x31,0x2E,0x31,0x31,0x2E,
                            0x30,0x30,0x2E,0x30,0x30,0x5F,0x3F}}}},
                    {0x01,0x32,0x00,0x01,
                            {{0xFF,0xFF,{0x30,0x30,0x32,0x2E,0x30,0x32,0x33,0x5F,0x30,0x30,0x30,0x00,0x00,
                             0x00,0x00,0x00},
                             0x13,{0x30,0x31,0x2E,0x31,0x31,0x2E,0x30,0x30,0x2E,
                             0x30,0x30,0x5F,0x47,0x45,0x4E,0x45,0x52,0x49,0x43}}}},
                }
        },0x00,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_DMS_SET_FIRMWARE_PREF  */
const unpack_fms_SetImagesPreference_t const_unpack_fms_SetImagesPreference_t = {
        0x05,{0x01,0x02,0x03,0x04,0x05},0,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

int fms_validate_dummy_unpack()
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
    loopCount = sizeof(validate_fms_resp_msg)/sizeof(validate_fms_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_fms_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_DMS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("FMS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("FMS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_DMS_GET_FIRMWARE_PREF:
                 if (eRSP == rsp_ctx.type)
                 {
                     unpack_fms_GetImagesPreference_t *varp = &var_unpack_fms_GetImagesPreference_t;
                     const unpack_fms_GetImagesPreference_t *cstp = &const_unpack_fms_GetImagesPreference_t;

                     UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                     unpack_fms_GetImagesPreference,
                     dump_FMS_GetImagesPreference,
                     msg.buf,
                     rlen,
                     &var_unpack_fms_GetImagesPreference_t,
                     3,
                     CMP_PTR_TYPE, &varp->ImageListSize, &cstp->ImageListSize,
                     CMP_PTR_TYPE, varp->pImageList, cstp->pImageList,
                     CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                     );
                 }
                 break;
            case eQMI_DMS_LIST_FIRMWARE:
                 if (eRSP == rsp_ctx.type)
                 {
                     UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                         unpack_fms_GetStoredImages,
                         dump_FMS_GetStoredImages,
                         msg.buf,
                         rlen,
                         &const_unpack_fms_GetStoredImages_t);
                 }
                 break;
            case eQMI_DMS_SET_FIRMWARE_PREF:
                 if (eRSP == rsp_ctx.type)
                 {
                     UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                         unpack_fms_SetImagesPreference,
                         dump_FMS_SetImagesPreference,
                         msg.buf,
                         rlen,
                         &const_unpack_fms_SetImagesPreference_t);
                 }
                 break;
            default:
                    printf("msgid 0x%x Unknown/NotHandled\n", rsp_ctx.msgid);
                break;
            }
        }
    }
    return 0;
}
