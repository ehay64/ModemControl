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
#include "nas.h"

#define IFPRINTF(s,p)   if( NULL != p ){ printf(s,*p); }
#define PLMN_LENGTH    3
#define UATISIZE                         16
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static int iLocalLog = 1;

int nas=-1;
void CloseNasFd();
int nas_validate_dummy_uppack();

static char remark[255]={0};
static int unpackRetCode = 0;

////Modem Settings
pack_nas_SLQSSetBandPreference_t DefaultBandPreference = {0x0000000000000003};
uint8_t DefaultsetEmerMode = 0; // OFF, normal mode
uint16_t DefaultsetModePref = 0x1F; // GSM, UMTS, LTE enabled
uint64_t DefaultsetBandPref = 0xffffffffffffffffLL; // select all band
uint16_t DefaultsetPRLPref = 0x3fff; // any available system
uint16_t DefaultsetRoamPref = 0xff; // Acquire systems, regardless of their roaming indicator
uint64_t DefaultsetLTEBandPref = 0x44; // BAND 3 and BAND 7 enabled 
uint8_t DefaultsetChgDuration = 0x00;
uint8_t DefaultsetMNCIncPCSDigStat = 0x00;
uint32_t DefaultsetSrvDomainPref = 0x00;
uint32_t DefaultsetGWAcqOrderPref = 0x00;
uint32_t DefaultsetSrvRegRestriction = 0x00;
pack_nas_SLQSSetSysSelectionPref_t DefaultSysSelPref = {
    &DefaultsetEmerMode,
    &DefaultsetModePref,
    &DefaultsetBandPref,
    &DefaultsetPRLPref,
    &DefaultsetRoamPref,
    &DefaultsetLTEBandPref,
    NULL,
    &DefaultsetChgDuration,
    NULL,
    &DefaultsetSrvDomainPref,
    &DefaultsetGWAcqOrderPref,
    NULL,
    NULL,
    &DefaultsetSrvRegRestriction,
    NULL,
    NULL
};

uint8_t mSetEmerMode = 0; // OFF, normal mode
uint16_t mSetModePref = 0x1F; // GSM, UMTS, LTE enabled
uint64_t mSetBandPref = 0xffffffffffffffffLL; // select all band
uint16_t mSetPRLPref = 0x3fff; // any available system
uint16_t mSetRoamPref = 0xff; // Acquire systems, regardless of their roaming indicator
uint64_t mSetLTEBandPref = 0x44; // BAND 3 and BAND 7 enabled
uint8_t mSetChgDuration = 0x00;
uint8_t mSetMNCIncPCSDigStat = 0x00;
uint32_t mSetSrvDomainPref = 0x00;
uint32_t mSetGWAcqOrderPref = 0x00;
uint32_t mSetSrvRegRestriction = 0x00;
pack_nas_SLQSSetSysSelectionPref_t ModemSetSysSelPref = {
    &mSetEmerMode,
    &mSetModePref,
    &mSetBandPref,
    &mSetPRLPref,
    &mSetRoamPref,
    &mSetLTEBandPref,
    NULL,
    NULL,
    NULL,
    &mSetSrvDomainPref,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

uint8_t DefaultForceRev0=1;
uint8_t DefaultCustomSCP =1;
uint32_t DefaultProtocol = 0x00000001;
uint32_t DefaultBroadcast = 0x00000001;
uint32_t DefaultApplication = 0x00000001;
uint32_t DefaultRoaming= 1;
pack_nas_SetCDMANetworkParameters_t DefaultCDMANetworkParameters={
        "", &DefaultForceRev0, &DefaultCustomSCP,&DefaultProtocol, &DefaultBroadcast,
        &DefaultApplication, &DefaultRoaming};
unpack_nas_SetCDMANetworkParameters_t tunpack_nas_SetCDMANetworkParameters = {
        0, SWI_UINT256_INT_VALUE };

pack_nas_SetACCOLC_t DefaultACCOLC = {
   {0}, // int8_t spc[6]
   0    // uint8_t accolc
};
unpack_nas_SetACCOLC_t tunpack_nas_SetACCOLC = {0, SWI_UINT256_INT_VALUE };

nas_wcdmaUARFCN DefaultwcdmaUARFCN = {0, 0};
nas_ltePCI DefaultltePCI;
nas_lteEARFCN DefaultLteEARFCN;
pack_nas_SLQSNASSwiSetChannelLock_t DefaultNASSwiSetChannelLock={
        &DefaultwcdmaUARFCN,&DefaultLteEARFCN, &DefaultltePCI};
unpack_nas_SLQSNASSwiSetChannelLock_t tunpack_nas_SLQSNASSwiSetChannelLock = {
        0, SWI_UINT256_INT_VALUE };

pack_nas_SetNetworkPreference_t   DefaultNetworkPreference ={0,0,0};

uint8_t      DefEdrxEnable;
uint8_t      DefCycleLen;
uint8_t      DefPagingTimeWindow;
uint8_t      DefEdrxRatType;
uint32_t     DefEdrxCiotLteMode;
pack_nas_SLQSNASSeteDRXParams_t DefaulteDRXParams = { &DefEdrxEnable, &DefCycleLen, &DefPagingTimeWindow, &DefEdrxRatType, &DefEdrxCiotLteMode};
unpack_nas_SLQSNASSeteDRXParams_t tunpack_nas_SLQSNASSeteDRXParams = {0, SWI_UINT256_INT_VALUE};

/////////////////////////

/*  Parameters for SLQSNasIndicationRegisterExt() API */
uint8_t setInd   = 0x01;
uint8_t unsetInd = 0x00;
pack_nas_SLQSNasIndicationRegisterExt_t nasIndicationRegisterExt = {
    &setInd, // uint8_t *pSystemSelectionInd
    &setInd, // uint8_t *pDDTMInd
    &setInd, // uint8_t *pServingSystemInd
    &setInd, // uint8_t *pDualStandByPrefInd
    &setInd, // uint8_t *pSubscriptionInfoInd
    &setInd, // uint8_t *pNetworkTimeInd
    &setInd, // uint8_t *pSysInfoInd
    &setInd, // uint8_t *pSignalStrengthInd
    &setInd, // uint8_t *pErrorRateInd
    &setInd, // uint8_t *pHDRNewUATIAssInd
    &setInd, // uint8_t *pHDRSessionCloseInd
    &setInd, // uint8_t *pManagedRoamingInd
    &setInd, // uint8_t *pNetworkRejectInd
    &setInd, // uint8_t *pSuppressSysInfoInd
    &setInd, // uint8_t *pLTECphyCa
    &setInd  // uint8_t *pEdrxChangeInfoInd;
};
unpack_nas_SLQSNasIndicationRegisterExt_t tunpack_nas_SLQSNasIndicationRegisterExt = {0, SWI_UINT256_INT_VALUE };

pack_nas_SLQSNasIndicationRegisterV2_t nasIndicationRegisterV2 = {
    &setInd, // uint8_t *pSystemSelectionInd
    &setInd, // uint8_t *pDDTMInd
    &setInd, // uint8_t *pServingSystemInd
    &setInd, // uint8_t *pDualStandByPrefInd
    &setInd, // uint8_t *pSubscriptionInfoInd
    &setInd, // uint8_t *pNetworkTimeInd
    &setInd, // uint8_t *pSysInfoInd
    &setInd, // uint8_t *pSignalStrengthInd
    &setInd, // uint8_t *pErrorRateInd
    &setInd, // uint8_t *pHDRNewUATIAssInd
    &setInd, // uint8_t *pHDRSessionCloseInd
    &setInd, // uint8_t *pManagedRoamingInd
    NULL,    // uint8_t *pCurrentPLMNNameInd;
    NULL,    // uint8_t *pEMBMSStatusInd;
    &setInd, // uint8_t *pRFBandInfoInd;
    &setInd, // uint8_t *pNetworkRejectInd
    &setInd, // uint8_t *pSuppressSysInfoInd
    NULL,    // uint8_t *pOperatorNameDataInd;
    NULL,    // uint8_t *pCSPPLMNModeInd;
    NULL,    // uint8_t *pRTREConfigurationInd;
    NULL,    // uint8_t *pIMSPrefStatusInd;
    NULL,    // uint8_t *pE911StateReadyStatusInd;
    NULL,    // uint8_t *pLTESIB16NetworkTimeInd;
    &setInd, // uint8_t *pLTECphyCa
    NULL,    // uint8_t *pSubscriptionChangeInd;
    NULL,    // uint8_t *pSSACBarringInd;
    NULL,    // uint8_t *pT3402TimerValueInd;
    NULL,    // uint8_t *pAccessClassBarringInd;
    NULL,    // uint8_t *pDataSubscriptionPriorityInd;
    NULL,    // uint8_t *pT3346TimerStatusChangeInd;
    NULL,    // uint8_t *pCallModeStatusInd;
    NULL,    // uint8_t *pSSACBarringExtInd;
    NULL,    // uint8_t *pManualNWScanFailureInd;
    NULL,    // uint8_t *pTimerExpiryInd;
    NULL,    // uint8_t *pEmergencyModeStatusInd;
    NULL,    // uint8_t *pGcellInfoInd;
    &setInd, // uint8_t *pEdrxChangeInfoInd;
    NULL,    // uint8_t *pLTERACHFailInd;
    NULL,    // uint8_t *pLTERRCTxInfoInd;
};
unpack_nas_SLQSNasIndicationRegisterV2_t tunpack_nas_SLQSNasIndicationRegisterV2 = {0, SWI_UINT256_INT_VALUE };

nas_SLQSSignalStrengthsIndReq sigIndReq = {
    10, // uint8_t  rxSignalStrengthDelta
    1, // uint8_t  ecioDelta
    10, // uint8_t  ioDelta
    1, // uint8_t  sinrDelta
    10, // uint8_t  rsrqDelta
    5, // uint8_t  ecioThresholdListLen
    {-10, -20, -30, -40, -50 }, // int16_t  ecioThresholdList[10]
    5, // uint8_t  sinrThresholdListLen
    {10, 20, 30, 40, 50 }, // uint8_t  sinrThresholdList[5]
    1, // uint16_t lteSnrDelta
    1 // uint8_t  lteRsrpDelta
};

pack_nas_SLQSSetSignalStrengthsCallback_t setSigStrengthCallback = {
    1, // uint8_t bEnable
    &sigIndReq
};
unpack_nas_SLQSSetSignalStrengthsCallback_t tunpack_nas_SLQSSetSignalStrengthsCallback =
        {0, SWI_UINT256_INT_VALUE };

uint8_t setRFInfoCallback = 1;
unpack_nas_SetRFInfoCallback_t tunpack_nas_SetRFInfoCallback = {0, SWI_UINT256_INT_VALUE};

unpack_nas_SetLURejectCallback_t tunpack_nas_SetLURejectCallback = {0, SWI_UINT256_INT_VALUE};

unpack_nas_GetHomeNetwork_t homenw;
nas_homeNwMNC3GppTlv HomeNwMNC3Gpp;
nas_nwNameSrc3GppTlv NwNameSrc3Gpp;
swi_uint256_t  ParamPresenceMask;
unpack_nas_SLQSGetHomeNetwork_t slqshomenw =
{0xFF,0xFF,{0},0xFF,0xFF,&HomeNwMNC3Gpp,&NwNameSrc3Gpp, SWI_UINT256_INT_VALUE};

unpack_nas_GetRFInfo_t      rfinfo;
unpack_nas_SLQSNasGetSigInfo_t siginfo;

uint8_t emerMode = 0xFF;
uint16_t modePref = 0xFFFF;
uint64_t bandPref = 0xFFFFFFFFFFFFFFFFLL;
uint16_t prlPref = 0xFFFF;
uint16_t roamPref = 0xFFFF;
uint64_t lteBandPref =0xFFFFFFFFFFFFFFFFLL;
uint8_t netSelPref = 0xFF;
uint32_t srvDomainPref = 0xFFFFFFFFLL;
uint32_t gwaAcqOrderPref = 0xFFFFFFFFLL;
unpack_nas_SLQSGetSysSelectionPref_t sysSelPref = {
    &emerMode,
    &modePref,
    &bandPref,
    &prlPref,
    &roamPref,
    &lteBandPref,
    &netSelPref,
    &srvDomainPref,
    &gwaAcqOrderPref,
    SWI_UINT256_INT_VALUE
};

nas_EmerModeTlv          getEmerMode = {0,0};
nas_ModePrefTlv          getModePref = {0,0};
nas_BandPrefTlv          getBandPref = {0,0};
nas_PRLPrefTlv           getPRLPref  = {0,0};
nas_RoamPrefTlv          getRoamPref = {0,0};
nas_LTEBandPrefTlv       getLTEBandPref = {0,0};
nas_NetSelPrefTlv        getNetSelPref  = {0,0};
nas_SrvDomainPrefTlv     getSrvDomainPref = {0,0};
nas_GWAcqOrderPrefTlv    getGWAcqOrderPref = {0,0};
uint8_t                  acqOrder[0xFF];
nas_AcqOrderPrefTlv      getAcqOrderPref = { 0xFF, acqOrder, 0 };
nas_RatDisabledMaskTlv   getRatDisabledMask = {0,0};
nas_CiotLteOpModePrefTlv getCiotLteOpModePref = {0,0};
nas_LteM1BandPrefTlv     getLteM1BandPref = {0,0};
nas_LteNb1BandPrefTlv    getLteNb1BandPref = {0,0};
uint32_t                 ciotAcqOrderPref[0xFF];
nas_CiotAcqOrderPrefTlv  getCiotAcqOrderPref = { 0xFF, ciotAcqOrderPref, 0 };
nas_NR5gBandPrefTlv      getNr5gBandPref;
nas_LTEBandPrefExtTlv    getLTEBandPrefExt;

unpack_nas_SLQSGetSysSelectionPrefExt_t sysSelPrefExt = {
    &getEmerMode,
    &getModePref,
    &getBandPref,
    &getPRLPref,
    &getRoamPref,
    &getLTEBandPref,
    &getNetSelPref,
    &getSrvDomainPref,
    &getGWAcqOrderPref,
    &getAcqOrderPref,
    &getRatDisabledMask,
    &getCiotLteOpModePref,
    &getLteM1BandPref,
    &getLteNb1BandPref,
    &getCiotAcqOrderPref,
    &getNr5gBandPref,
    &getLTEBandPrefExt
};

nas_TDSCDMABandPrefTlv   getTDSCDMABandPref;
nas_SrvRegRestricTlv     getSrvRegRestric;
nas_UsageSettingTlv      getUsageSetting;
nas_VoiceDomainPrefTlv   getVoiceDomainPref;

unpack_nas_SLQSGetSysSelectionPrefExtV2_t sysSelPrefExtv2 = {
    &getEmerMode,
    &getModePref,
    &getBandPref,
    &getPRLPref,
    &getRoamPref,
    &getLTEBandPref,
    &getNetSelPref,
    &getSrvDomainPref,
    &getGWAcqOrderPref,
    &getAcqOrderPref,
    &getRatDisabledMask,
    &getCiotLteOpModePref,
    &getLteM1BandPref,
    &getLteNb1BandPref,
    &getCiotAcqOrderPref,
    &getNr5gBandPref,
    &getLTEBandPrefExt,
    &getTDSCDMABandPref,
    &getSrvRegRestric,
    &getUsageSetting,
    &getVoiceDomainPref,
    SWI_UINT256_INT_VALUE
};

uint8_t setEmerMode = 0; // OFF, normal mode
uint16_t setModePref = 0x1F; // GSM, UMTS, LTE enabled
uint64_t setBandPref = 0xffffffffffffffffLL; // select all band
uint16_t setPRLPref = 0x3fff; // any available system
uint16_t setRoamPref = 0xff; // Acquire systems, regardless of their roaming indicator
uint64_t setLTEBandPref = 0x44; // BAND 3 and BAND 7 enabled 
uint8_t setChgDuration = 0x00;
uint8_t setMNCIncPCSDigStat = 0x00;
uint32_t setSrvDomainPref = 0x00;
uint32_t setGWAcqOrderPref = 0x00;
uint32_t setSrvRegRestriction = 0x00;

pack_nas_SLQSSetSysSelectionPref_t setSysSelPref = {
    &setEmerMode,
    &setModePref,
    &setBandPref,
    &setPRLPref,
    &setRoamPref,
    &setLTEBandPref,
    NULL,
    &setChgDuration,
    NULL,
    &setSrvDomainPref,
    &setGWAcqOrderPref,
    NULL,
    NULL,
    &setSrvRegRestriction,
    NULL,
    NULL
};

pack_nas_SLQSSetSysSelectionPref_t setSysSelPref2 = {
    &setEmerMode,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    //&setSrvRegRestriction,
    NULL,
    NULL
};

unpack_nas_SLQSSetSysSelectionPref_t tunpack_nas_SLQSSetSysSelectionPref = {0, SWI_UINT256_INT_VALUE };

uint64_t              lteWideBand = 0x01;
uint64_t              lteM1BandPref  = 0x000000000000004; //E-UTRA Operating Band 3
uint64_t              lteNB1BandPref = 0x000000000000010; //E-UTRA Operating Band 5
uint32_t              lteOpMode = 0x02; //Camped on LTE M1
struct nas_lteBandPrefExt lteBandPrefExtReq = {0x000000000000002,0,0,0};
pack_nas_SLQSSetSysSelectionPrefExt_t setSysSelPrefExt = {
    &setEmerMode,
    &setModePref,
    &setBandPref,
    &setPRLPref,
    &setRoamPref,
    &setLTEBandPref,
    NULL,
    &setChgDuration,
    NULL,
    &setSrvDomainPref,
    &setGWAcqOrderPref,
    NULL,
    NULL,
    &setSrvRegRestriction,
    NULL,
    NULL,
    NULL,
    &lteWideBand,
    &lteM1BandPref,
    &lteNB1BandPref,
    &lteOpMode,
    NULL,
    NULL,
};

pack_nas_SLQSSetSysSelectionPrefExt_t setLTEBandprefExt = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &lteBandPrefExtReq,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

uint8_t pack_nas_PerformNetworkScanPCINetworkType = 4;//LTE
uint32_t pack_nas_PerformNetworkScanPCIScanType = 3 ;//Physical cell ID
uint64_t   pack_nas_PerformNetworkScanPCICiotOpModePref = 0x01;
uint64_t   pack_nas_PerformNetworkScanPCILteM1BandPref = 0x4000000000000000LL;
uint64_t   pack_nas_PerformNetworkScanPCILteNB1BandPref = 0x4000000000000000LL;

pack_nas_PerformNetworkScanPCI_t tpack_nas_PerformNetworkScanPCI ={
    &pack_nas_PerformNetworkScanPCINetworkType,
    &pack_nas_PerformNetworkScanPCIScanType,
    NULL,
    NULL,
    NULL,
};

pack_nas_PerformNetworkScanPCI_t tpack_nas_PerformNetworkScanPCI2 ={
    &pack_nas_PerformNetworkScanPCINetworkType,
    &pack_nas_PerformNetworkScanPCIScanType,
    &pack_nas_PerformNetworkScanPCICiotOpModePref,
    &pack_nas_PerformNetworkScanPCILteM1BandPref,
    &pack_nas_PerformNetworkScanPCILteNB1BandPref,
};

unpack_nas_SLQSSetSysSelectionPrefExt_t tunpack_nas_SLQSSetSysSelectionPrefExt = {
        0, SWI_UINT256_INT_VALUE };

unpack_nas_GetServingNetwork_t getServingNW = {
    0, // uint32_t   RegistrationState
    0, // uint32_t   CSDomain
    0, // uint32_t   PSDomain
    0, // uint32_t   RAN
    0, // uint8_t    RadioIfacesSize
    {0}, // uint8_t    RadioIfaces[255]
    0, // uint32_t   Roaming
    0, // uint16_t   MCC
    0, // uint16_t   MNC
    0, // uint8_t    nameSize
    {0}, // uint8_t    Name[255]
    0, // uint8_t    DataCapsLen
    {0}, // uint8_t    DataCaps[255]
    SWI_UINT256_INT_VALUE
};

unpack_nas_GetServingNetworkCapabilities_t getServingNwCap = {
    0, // uint8_t    DataCapsLen
    {0}, // uint8_t    DataCaps[255]
    SWI_UINT256_INT_VALUE
};
nas_QmiNas3GppNetworkInfo nasPerformNetworkScan3GppNetworkInfo[NAS_MAX_NUM_NETWORKS];
nas_QmiNas3GppNetworkRAT nasPerformNetworkScanRATINstance[NAS_MAX_NUM_NETWORKS];
nas_QmisNasPcsDigit  nasPerformNetworkScanPCSInstance[NAS_MAX_NUM_NETWORKS];
uint8_t u8NasPerformNetworkScan3GppNetworkInfoSize = NAS_MAX_NUM_NETWORKS;
uint8_t u8NasPerformNetworkScanGppNetworkRATSize = NAS_MAX_NUM_NETWORKS;
uint8_t u8NasPerformNetworkScanPCSInstanceSize = NAS_MAX_NUM_NETWORKS;
uint32_t u32NasPerformNetworkScanScanResult = 0;
nas_QmisNasSlqsNasPCIInfo NasQmisNasSlqsNasPCIInfo;
nas_lteOpModeTlv          NwScanLteOpModeTlv;


unpack_nas_PerformNetworkScan_t performNwScan = {
    &u8NasPerformNetworkScan3GppNetworkInfoSize, // uint8_t   *p3GppNetworkInstanceSize
    (nas_QmiNas3GppNetworkInfo*)&nasPerformNetworkScan3GppNetworkInfo, // nas_QmiNas3GppNetworkInfo  *pInstances
    &u8NasPerformNetworkScanGppNetworkRATSize, // uint8_t   *pRATInstanceSize
    (nas_QmiNas3GppNetworkRAT*)&nasPerformNetworkScanRATINstance, // nas_QmiNas3GppNetworkRAT  RATINstance
    &u8NasPerformNetworkScanPCSInstanceSize, // uint8_t   *pPCSInstanceSize
    (nas_QmisNasPcsDigit*)&nasPerformNetworkScanPCSInstance,// nas_QmisNasPcsDigit  *pPCSInstance
    &u32NasPerformNetworkScanScanResult, //uint32_t   *pScanResult
    &NasQmisNasSlqsNasPCIInfo, //nas_QmisNasSlqsNasPCIInfo  *pPCIInfo;
    &NwScanLteOpModeTlv, //nas_lteOpModeTlv *pLteOpModeTlv;
    SWI_UINT256_INT_VALUE
};

nas_networkNameSrcTlv      NetworkNameSrcTlv;
unpack_nas_SLQSPerformNetworkScanV2_t performNwScanv2 = {
        &u8NasPerformNetworkScan3GppNetworkInfoSize, // uint8_t   *p3GppNetworkInstanceSize
        (nas_QmiNas3GppNetworkInfo*)&nasPerformNetworkScan3GppNetworkInfo, // nas_QmiNas3GppNetworkInfo  *pInstances
        &u8NasPerformNetworkScanGppNetworkRATSize, // uint8_t   *pRATInstanceSize
        (nas_QmiNas3GppNetworkRAT*)&nasPerformNetworkScanRATINstance, // nas_QmiNas3GppNetworkRAT  RATINstance
        &u8NasPerformNetworkScanPCSInstanceSize, // uint8_t   *pPCSInstanceSize
        (nas_QmisNasPcsDigit*)&nasPerformNetworkScanPCSInstance,// nas_QmisNasPcsDigit  *pPCSInstance
        &u32NasPerformNetworkScanScanResult, //uint32_t   *pScanResult
        &NasQmisNasSlqsNasPCIInfo, //nas_QmisNasSlqsNasPCIInfo  *pPCIInfo;
        &NwScanLteOpModeTlv, //nas_lteOpModeTlv *pLteOpModeTlv;
        &NetworkNameSrcTlv, // nas_networkNameSrcTlv *pNetworkNameSrcTlv;
        SWI_UINT256_INT_VALUE
};

unpack_nas_SLQSSwiGetLteCQI_t swiGetLTECQI = {
    0, // uint8_t   ValidityCW0
    0, // uint8_t   CQIValueCW0
    0, // uint8_t   ValidityCW1
    0, // uint8_t   CQIValueCW1
    SWI_UINT256_INT_VALUE
};

nas_LTEInfo lteInfo;

unpack_nas_SLQSNasSwiModemStatus_t swiGetModemStatus = {
    { // nas_CommInfo commonInfo
        0, // int8_t temperature
        0, // uint8_t modemMode
        0, // uint8_t systemMode
        0, // uint8_t imsRegState
        0  // uint8_t psState
    },
    &lteInfo,
    SWI_UINT256_INT_VALUE
};

unpack_nas_SLQSGetServingSystem_t getServingSystem = {
    {0,0,0,0,0,{0}}, // nas_servSystem    ServingSystem
    0,               // uint8_t           RoamIndicatorVal
    {0,{0}},         // nas_dataSrvCapabilities DataSrvCapabilities
    {0,0,0,{0}},     // nas_currentPLMN   CurrentPLMN
    0,               // uint16_t          SystemID
    0,               // uint16_t          NetworkID
    0,               // uint16_t          BasestationID
    0,               // uint32_t          BasestationLatitude
    0,               // uint32_t          BasestationLongitude
    {0,{0},{0}},     // nas_roamIndList   RoamingIndicatorList
    0,               // uint8_t           DefaultRoamInd
    {0,0,0},         // nas_qaQmi3Gpp2TimeZone Gpp2TimeZone
    0,               // uint8_t           CDMA_P_Rev
    0,               // uint8_t           GppTimeZone
    0,               // uint8_t           GppNetworkDSTAdjustment
    0,               // uint16_t          Lac
    0,               // uint32_t          CellID
    0,               // uint8_t           ConcSvcInfo
    0,               // uint8_t           PRLInd
    0,               // uint8_t           DTMInd
    {0,0,0,0,0},     // nas_detailSvcInfo DetailedSvcInfo
    {0,0},           // nas_CDMASysInfoExt CDMASystemInfoExt
    0,               // uint8_t           HdrPersonality
    0,               // uint16_t          TrackAreaCode
    {0,0},           // nas_callBarStatus CallBarStatus
    SWI_UINT256_INT_VALUE
};

unpack_nas_SLQSGetServingSystemV2_t getServingSystemv2 = {
    {0,0,0,0,0,{0}}, // nas_servSystem    ServingSystem
    0,               // uint8_t           RoamIndicatorVal
    {0,{0}},         // nas_dataSrvCapabilities DataSrvCapabilities
    {0,0,0,{0}},     // nas_currentPLMN   CurrentPLMN
    0,               // uint16_t          SystemID
    0,               // uint16_t          NetworkID
    0,               // uint16_t          BasestationID
    0,               // uint32_t          BasestationLatitude
    0,               // uint32_t          BasestationLongitude
    {0,{0},{0}},     // nas_roamIndList   RoamingIndicatorList
    0,               // uint8_t           DefaultRoamInd
    {0,0,0},         // nas_qaQmi3Gpp2TimeZone Gpp2TimeZone
    0,               // uint8_t           CDMA_P_Rev
    0,               // uint8_t           GppTimeZone
    0,               // uint8_t           GppNetworkDSTAdjustment
    0,               // uint16_t          Lac
    0,               // uint32_t          CellID
    0,               // uint8_t           ConcSvcInfo
    0,               // uint8_t           PRLInd
    0,               // uint8_t           DTMInd
    {0,0,0,0,0},     // nas_detailSvcInfo DetailedSvcInfo
    {0,0},           // nas_CDMASysInfoExt CDMASystemInfoExt
    0,               // uint8_t           HdrPersonality
    0,               // uint16_t          TrackAreaCode
    {0,0},           // nas_callBarStatus CallBarStatus
    {0,0,0,0},       // nas_MNCPCSDigitStatus MNCPCSDigitStatTlv
    SWI_UINT256_INT_VALUE
};

uint16_t signalStrenghMask = 0xFF; // bit mask, request for everything

unpack_nas_SLQSGetSignalStrength_t getSignalStrength = {
    0,       // uint16_t   signalStrengthReqMask
    0,       // uint16_t   rxSignalStrengthListLen
    {{0,0}}, // nas_rxSignalStrengthListElement rxSignalStrengthList[18]
    0,       // uint16_t   ecioListLen 
    {{0,0}}, // nas_ecioListElement ecioList[18]
    0,       // int32_t    Io
    0,       // uint8_t    sinr
    0,       // uint16_t   errorRateListLen
    {{0,0}}, // nas_errorRateListElement errorRateList[18]
    {0,0},   // nas_rsrqInformation rsrqInfo
    0,       // int16_t    ltesnr
    0,       // int16_t    ltersrp
    SWI_UINT256_INT_VALUE
};

unpack_nas_GetCDMANetworkParameters_t getCDMANetworkParameters = {
    0, // uint8_t  SCI
    0, // uint8_t  SCM
    0, // uint8_t  RegHomeSID
    0, // uint8_t  RegForeignSID
    0, // uint8_t  RegForeignNID
    0, // uint8_t  ForceRev0
    0, // uint8_t  CustomSCP
    0, // uint32_t Protocol
    0, // uint32_t Broadcast
    0, // uint32_t Application
    0, // uint32_t Roaming
    SWI_UINT256_INT_VALUE
};

uint32_t authStatus;
unpack_nas_GetANAAAAuthenticationStatus_t getANAAAStatus = {&authStatus, SWI_UINT256_INT_VALUE};

pack_nas_SetACCOLC_t setACCOLC = {
   {0}, // int8_t spc[6]
   0    // uint8_t accolc
};

uint8_t getAccolc;
unpack_nas_GetACCOLC_t getACCOLC = {&getAccolc, SWI_UINT256_INT_VALUE};

int16_t cdmaRssiThresList[2] = {-1020, -400}; /* -102dB, -40dB */
int16_t cdmaEcioThresList[2] = {-400, -310};  /* -20dB, -15.5dB */
int16_t hdrRssiThresList[2] = {-500, -150};   /* -50dB, -15dB */
int16_t hdrEcioThresList[2] = {-400, -310};   /* -20dB, -15.5dB */
uint16_t hdrSinrThresList[2] = {0x01, 0x03};   /* -6dB, -3dB */
int16_t hdrIoThresList[2] = {-1110, -730};    /* -110dB, -73dB */
int16_t gsmRssiThresList[2] = {-950, -800 };  /* -95dB, -80dB */
int16_t wcdmaRssiThreshList[2] = {-1000, -200}; /* -100dB, -20dB */
int16_t wcdmaEcioThreshList[2] = {-400, -310}; /* -20dB, -15.5dB */
int16_t lteRssiThreshList[2] = {-1000, -400}; /* -100dB, -40dB */
int16_t lteSnrThreshList[2] = {-198, -230}; /* -19.8dB, 23dB */
int16_t lteRsrqThreshList[2] = {-110, -60}; /* -11dB, -6dB */
int16_t lteRsrpThreshList[2] = {-1250, -640}; /* -125dB, -64dB */
int16_t tdscdmaRscpThreshList[2] = {-950, -800}; /* -95dB, -80dB */
float tdscdmaRssiThreshList[2] = {-950, -800}; /* -95dB, -80dB */
float tdscdmaEcioThreshList[2] = {-400, -310}; /* -20dB, -15.5dB */
float tdscdmaSinrThreshList[2] = {0x01, 0x03}; /* -6dB, -3dB */

nas_CDMARSSIThresh    CDMARSSIThresh = {2, &cdmaRssiThresList[0]};
uint16_t              CDMARSSIDelta = 100; /* 10dBm */

nas_CDMAECIOThresh    CDMAECIOThresh = {2, &cdmaEcioThresList[0]};
uint16_t              CDMAECIODelta = 100; /* 10dBm */

nas_HDRRSSIThresh     HDRRSSIThresh = {2, &hdrRssiThresList[0]};
uint16_t              HDRRSSIDelta = 100; /* 10dBm */

nas_HDRECIOThresh     HDRECIOThresh = {2, &hdrEcioThresList[0]};
uint16_t              HDRECIODelta = 100; /* 10dBm */

nas_HDRSINRThreshold  HDRSINRThresh = {2, &hdrSinrThresList[0]};
uint16_t              HDRSINRDelta = 1;

nas_HDRIOThresh       HDRIOThresh = {2, &hdrIoThresList[0]};
uint16_t              HDRIODelta = 100; /* 10dBm */

nas_GSMRSSIThresh     GSMRSSIThresh = {2, &gsmRssiThresList[0]};
uint16_t              GSMRSSIDelta = 100; /* 10dBm */

nas_WCDMARSSIThresh   WCDMARSSIThresh = {2, &wcdmaRssiThreshList[0]};
uint16_t              WCDMARSSIDelta = 100; /* 10dBm */

nas_WCDMAECIOThresh   WCDMAECIOThresh = {2, &wcdmaEcioThreshList[0]};
uint16_t              WCDMAECIODelta = 100; /* 10dBm */

nas_LTERSSIThresh     LTERSSIThresh = {2, &lteRssiThreshList[0]};
uint16_t              LTERSSIDelta = 100; /* 10dBm */

nas_LTESNRThreshold   LTESNRThresh = {2, &lteSnrThreshList[0]};
uint16_t              LTESNRDelta = 100; /* 10dBm */

nas_LTERSRQThresh     LTERSRQThresh = {2, &lteRsrqThreshList[0]};
uint16_t              LTERSRQDelta = 100; /* 10dBm */

nas_LTERSRPThresh     LTERSRPThresh = {2, &lteRsrpThreshList[0]};
uint16_t              LTERSRPDelta = 100; /* 10dBm */

nas_LTESigRptConfig   LTESigRptConfig = {0,0}; /* report using default configuration */

nas_TDSCDMARSCPThresh TDSCDMARSCPThresh = {2, &tdscdmaRscpThreshList[0]};
uint16_t              TDSCDMARSCPDelta = 100; /* 10dBm */

nas_TDSCDMARSSIThresh TDSCDMARSSIThresh = {2, &tdscdmaRssiThreshList[0]};
float         TDSCDMARSSIDelta = 100; /* 10dBm */

nas_TDSCDMAECIOThresh TDSCDMAECIOThresh = {2, &tdscdmaEcioThreshList[0]};
float         TDSCDMAECIODelta = 100; /* 10dBm */

nas_TDSCDMASINRThresh TDSCDMASINRThresh = {2, &tdscdmaSinrThreshList[0]};
float         TDSCDMASINRDelta = 1;

pack_nas_SLQSNasConfigSigInfo2_t nasConfigSigInfo2 = {
    &CDMARSSIThresh,
    NULL, //&CDMARSSIDelta,
    &CDMAECIOThresh,
    NULL, //&CDMAECIODelta,
    &HDRRSSIThresh,
    NULL, //&HDRRSSIDelta,
    &HDRECIOThresh,
    NULL, //&HDRECIODelta,
    &HDRSINRThresh,
    NULL, //&HDRSINRDelta,
    &HDRIOThresh,
    NULL, //&HDRIODelta,
    &GSMRSSIThresh,
    NULL, //&GSMRSSIDelta,
    &WCDMARSSIThresh,
    NULL, //&WCDMARSSIDelta,
    &WCDMAECIOThresh,
    NULL, //&WCDMAECIODelta,
    &LTERSSIThresh,
    NULL, //&LTERSSIDelta,
    &LTESNRThresh,
    NULL, //&LTESNRDelta,
    &LTERSRQThresh,
    NULL, //&LTERSRQDelta,
    &LTERSRPThresh,
    NULL, //&LTERSRPDelta,
    &LTESigRptConfig,
    NULL,//&TDSCDMARSCPThresh,
    NULL, //&TDSCDMARSCPDelta,
    NULL, //&TDSCDMARSSIThresh,
    NULL, //&TDSCDMARSSIDelta,
    NULL, //&TDSCDMAECIOThresh,
    NULL, //&TDSCDMAECIODelta,
    NULL, //&TDSCDMASINRThresh,
    NULL  //&TDSCDMASINRDelta
};
unpack_nas_SLQSNasConfigSigInfo2_t tunpack_nas_SLQSNasConfigSigInfo2 = {
        0, SWI_UINT256_INT_VALUE };

uint8_t mncPcsStatus = 0;
pack_nas_SLQSGetPLMNName_t tpack_nas_SLQSGetPLMNName_t = {454, 6, &mncPcsStatus};
unpack_nas_SLQSGetPLMNName_t tunpack_nas_SLQSGetPLMNName_t ;

unpack_nas_GetNetworkPreference_t GetNetworkPreferenceResp;
pack_nas_SetNetworkPreference_t   SetNetworkPreferenceReq = {
    0x00,
    0x00,
    0
};

pack_nas_SetNetworkPreference_t   SetNetworkPreferenceReq3GPPANDLTE = {
    0x22,//3GPP+LTE
    0x00,
    0
};

unpack_nas_SetNetworkPreference_t   SetNetworkPreferenceResp;


unpack_nas_SlqsGetLTECphyCAInfo_t SlqsGetLTECphyCAInfo;

nas_MNRInfo MNRInfoDigit= {454,6,-1}; // -1 means No change in the mode preference

uint32_t  changeDurationPC = 0;

pack_nas_SLQSInitiateNetworkRegistration_t   nasInitiateNetworkRegistration = {
    0x02, //Reg Action
    &MNRInfoDigit,
    &changeDurationPC,
    NULL
};
unpack_nas_SLQSInitiateNetworkRegistration_t tunpack_nas_SLQSInitiateNetworkRegistration = {
        0, SWI_UINT256_INT_VALUE };

uint8_t rankInd = 0;
uint8_t timerInd = 1;

pack_nas_SLQSNasSwiIndicationRegister_t tpack_nas_SLQSNasSwiIndicationRegister ={
        1,1,1,1,1,1, &rankInd, &timerInd};
unpack_nas_SLQSNasSwiIndicationRegister_t tunpack_nas_SLQSNasSwiIndicationRegister = {
        0, SWI_UINT256_INT_VALUE };

nas_GERANInfo                geranInfo;
nas_UMTSInfo                 umtsInfo;
nas_CDMAInfo                 cdmaInfo;
nas_LTEInfoIntrafreq         lteInfoIntrafreq;
nas_LTEInfoInterfreq         lteInfoInterfreq;
nas_LTEInfoNeighboringGSM    lteInfoNeighboringGSM;
nas_LTEInfoNeighboringWCDMA  lteInfoNeighboringWCDMA;
uint32_t                     UMTSCellID;
nas_WCDMAInfoLTENeighborCell WCDMAInfLTENeighborCell;

unpack_nas_SLQSNasGetCellLocationInfo_t nasgetcelllocinfo ={
        &geranInfo,
        &umtsInfo,
        &cdmaInfo,
        &lteInfoIntrafreq,
        &lteInfoInterfreq,
        &lteInfoNeighboringGSM,
        &lteInfoNeighboringWCDMA,
        &UMTSCellID,
        &WCDMAInfLTENeighborCell,
        SWI_UINT256_INT_VALUE
};

nas_WCDMACellInfoExt         WCDMACellInfoExt;
nas_UMTSExtInfo              UMTSExtInfo;
nas_LteEarfcnInfo            LteEarfcnInfo;
unpack_nas_SLQSNasGetCellLocationInfoV2_t nasgetcelllocinfov2 ={
        &geranInfo,
        &umtsInfo,
        &cdmaInfo,
        &lteInfoIntrafreq,
        &lteInfoInterfreq,
        &lteInfoNeighboringGSM,
        &lteInfoNeighboringWCDMA,
        &UMTSCellID,
        &WCDMAInfLTENeighborCell,
        &WCDMACellInfoExt,
        &UMTSExtInfo,
        &LteEarfcnInfo,
        SWI_UINT256_INT_VALUE
};

nas_timeInfo                timeInfo3GPP2;
nas_timeInfo                timeInfo3GPP;

unpack_nas_SLQSGetNetworkTime_t nasgetnetworktimeinfo ={
        &timeInfo3GPP2,
        &timeInfo3GPP,
        SWI_UINT256_INT_VALUE
};

nas_SccRxInfo SccRxInfo;

unpack_nas_SLQSSwiGetLteSccRxInfo_t nasgetsccrxinfo = {&SccRxInfo, SWI_UINT256_INT_VALUE};

pack_nas_InitiateDomainAttach_t InitiateDomainAttach_req={0x01};
unpack_nas_InitiateDomainAttach_t tunpack_nas_InitiateDomainAttach = {
        0, SWI_UINT256_INT_VALUE };

uint8_t ForceRev0=1;
uint8_t CustomSCP =1;
uint32_t Protocol = 0x00000001;
uint32_t Broadcast = 0x00000001;
uint32_t Application = 0x00000001;
uint32_t Roaming= 1;
pack_nas_SetCDMANetworkParameters_t setCDMANetworkParameters={
        "12345", &ForceRev0, &CustomSCP,&Protocol, &Broadcast,
        &Application, &Roaming};

uint8_t colorCode;
unpack_nas_SLQSNasGetHDRColorCode_t tunpack_nas_SLQSNasGetHDRColorCode_t = {&colorCode, SWI_UINT256_INT_VALUE};

pack_nas_SLQSNasGetTxRxInfo_t tpack_nas_SLQSNasGetTxRxInfo_t = {0x05}; //UMTS
nas_rxInfo rxInfo0;
nas_rxInfo rxInfo1;
nas_txInfo txInfo;
unpack_nas_SLQSNasGetTxRxInfo_t tunpack_nas_SLQSNasGetTxRxInfo_t = {&rxInfo0,&rxInfo1,&txInfo,
        SWI_UINT256_INT_VALUE};

nas_serviceProviderName srvcProviderName;
nas_operatorPLMNList operatorPLMNList;
nas_PLMNNetworkName lmnNetworkName;
nas_operatorNameString operatorNameString;
nas_PLMNNetworkNameData nitzInformation;
unpack_nas_SLQSGetOperatorNameData_t tunpack_nas_SLQSGetOperatorNameData_t =
    {&srvcProviderName, &operatorPLMNList, &lmnNetworkName, &operatorNameString, &nitzInformation,
    SWI_UINT256_INT_VALUE};

pack_nas_SLQSNasGet3GPP2Subscription_t tpack_nas_SLQSNasGet3GPP2Subscription_t = {0xFF};
nas_namName      namNameInfo;
nas_dirNum       dirNum;
nas_homeSIDNID   homeSIDNID;
nas_minBasedIMSI minBasedIMSI;
nas_trueIMSI     trueIMSI;
nas_CDMAChannel  cdmaChannel;
nas_Mdn          get3gpp2Mdn;

unpack_nas_SLQSNasGet3GPP2Subscription_t tunpack_nas_SLQSNasGet3GPP2Subscription_t =
    {&namNameInfo, &dirNum, &homeSIDNID, &minBasedIMSI, &trueIMSI, &cdmaChannel,&get3gpp2Mdn,
    SWI_UINT256_INT_VALUE};

uint16_t currentPersonality;
uint8_t personalityListLength = 0x03;
nas_protocolSubtypeElement protSubTypeElmnt[3];
unpack_nas_SLQSSwiGetHDRPersonality_t tunpack_nas_SLQSSwiGetHDRPersonality_t =
    {&currentPersonality, &personalityListLength, protSubTypeElmnt, SWI_UINT256_INT_VALUE};

uint16_t currentPrsnlty;
uint8_t personlityListLength = 0x04;
nas_protocolSubtypeElement protSubTypeElemnt[4];
uint64_t              appSubType;
unpack_nas_SLQSSwiGetHDRProtSubtype_t tunpack_nas_SLQSSwiGetHDRProtSubtype_t =
    {&currentPrsnlty, &personlityListLength, protSubTypeElemnt, &appSubType, SWI_UINT256_INT_VALUE};

uint8_t PSDetach = 2; //immediate packet domain detach action.
pack_nas_SLQSSwiPSDetach_t tpack_nas_SLQSSwiPSDetach_t={&PSDetach};
unpack_nas_SLQSSwiPSDetach_t tunpack_nas_SLQSSwiPSDetach = {0, SWI_UINT256_INT_VALUE };

uint16_t cdmaFrameErrRate;
uint16_t hdrPackErrRate;
uint8_t gsmBER;
uint8_t wcdmaBER;
unpack_nas_SLQSGetErrorRate_t tunpack_nas_SLQSGetErrorRate_t =
    {&cdmaFrameErrRate, &hdrPackErrRate, &gsmBER, &wcdmaBER, SWI_UINT256_INT_VALUE};

nas_DRCParams DRCParameters;
uint8_t UATI[UATISIZE];
nas_PilotSetParams PSParams[255];
nas_PilotSetData PSData={255,PSParams};
unpack_nas_SLQSSwiGetHRPDStats_t tunpack_nas_SLQSSwiGetHRPDStats_t = {&DRCParameters,UATI,&PSData,
        SWI_UINT256_INT_VALUE};

uint8_t objVer;
uint16_t neighPilSet[255];
uint16_t sectID[255];
nas_ActPilotPNElement actPilPNElmt[255];
nas_NetworkStat1x netStat1x={0, 0, 0, 0, 0, 255, actPilPNElmt, 255, neighPilSet};
nas_NetworkStatEVDO netStatEVDO={0, 0, 0, sectID, 0, 0, 0, 0};
nas_DeviceConfigDetail devCfgDetail;
nas_DataStatusDetail   dataStatDetail;
unpack_nas_SLQSSwiNetworkDebug_t tunpack_nas_SLQSSwiNetworkDebug_t=
    {&objVer, &netStat1x, &netStatEVDO, &devCfgDetail, &dataStatDetail,
    SWI_UINT256_INT_VALUE};

int8_t rssiThresList[3] = { 0x96, 0xAA, 0xB4 };
int16_t ecioThresList[3] = { 0x02, 0x10, 0x18 }; /* -1dB, -8dB, -12dB */
uint8_t hdrsinrThresList[3] = { 0x02, 0x10, 0x1A };
int16_t ltesnrThresList[3] = { -2, 24, 26 }; /* -2dB, 24dB, 26dB */
int32_t ioThresList[3] = { -10, 6, 14 };
int8_t rsrqThresList[1] = { -11 };
int16_t rsrpThresList[3] = { 150, 200 };
nas_LTESigRptCfg lteSigRptCfg = {2,2};
float TDSCDMASINRCONFThreshList[3]={-1.1,-2.1,-5.99};
nas_RSSIThresh rssiThresh = {0x03,&rssiThresList[0]};
nas_ECIOThresh ecioThresh = {0x03,&ecioThresList[0]};
nas_HDRSINRThresh hdrsinrThresh = {3,&hdrsinrThresList[0]};
nas_LTESNRThresh ltesnrThresh = {3,&ltesnrThresList[0]};
nas_IOThresh ioThresh = {3,&ioThresList[0]};
nas_RSRQThresh rsrqThresh = {1,&rsrqThresList[0]};
nas_RSRPThresh rsrpThresh = {3,&rsrpThresList[0]};
nas_TDSCDMASINRCONFThresh tDSCDMASINRCONFThresh = {3, &TDSCDMASINRCONFThreshList[0]};
pack_nas_SLQSConfigSigInfo_t tpack_nas_SLQSConfigSigInfo_t = {
        &rssiThresh, &ecioThresh, &hdrsinrThresh, &ltesnrThresh, &ioThresh, &rsrqThresh, &rsrpThresh, &lteSigRptCfg,
        &tDSCDMASINRCONFThresh};
unpack_nas_SLQSConfigSigInfo_t tunpack_nas_SLQSConfigSigInfo = {0, SWI_UINT256_INT_VALUE };

uint16_t MCC = 455; // HK MCC
uint16_t MNC = 6; // HK Smartone MNC
uint8_t  Name[255] ="SMC HK"; // HK Smartone Newtork Name 
uint16_t SID;
uint16_t NID;
uint16_t Nw2MCC;
uint16_t Nw2MNC;
uint8_t  Nw2DescDisp;
uint8_t  Nw2DescEnc;
uint8_t  Nw2DescLen=254;
uint8_t  Nw2Name[255];
unpack_nas_GetHomeNetwork3GPP2_t tunpack_nas_GetHomeNetwork3GPP2_t={
        &MCC, &MNC, 254, Name, &SID, &NID, &Nw2MCC, &Nw2MNC, &Nw2DescDisp, &Nw2DescEnc, &Nw2DescLen, Nw2Name,
        SWI_UINT256_INT_VALUE};

nas_wcdmaUARFCN wcdmaUARFCN = {0, 0};
nas_lteEARFCN lteEARFCN = { 1, 2500, 2700};
nas_ltePCI ltePCI;
pack_nas_SLQSNASSwiSetChannelLock_t tpack_nas_SLQSNASSwiSetChannelLock_t={
        &wcdmaUARFCN, &lteEARFCN, NULL};

nas_wcdmaUARFCN gWcdmaUARFCN ;
nas_lteEARFCN gLteEARFCN ;
nas_ltePCI gLtePCI;
unpack_nas_SLQSNASSwiGetChannelLock_t tunpack_nas_SLQSNASSwiGetChannelLock_t={
        &gWcdmaUARFCN, &gLteEARFCN, &gLtePCI, SWI_UINT256_INT_VALUE};

unpack_nas_GetSignalStrengths_t tunpack_nas_GetSignalStrengths;

pack_nas_SLQSSetBandPreference_t tpack_nas_SLQSSetBandPreference = {0x0000000000000003};
unpack_nas_SLQSSetBandPreference_t tunpack_nas_SLQSSetBandPreference = {0, SWI_UINT256_INT_VALUE };

nas_SrvStatusInfo      cdmassi;
nas_SrvStatusInfo      hdrssi;
nas_GSMSrvStatusInfo   gsmssi;
nas_GSMSrvStatusInfo   wcdmassi;
nas_GSMSrvStatusInfo   ltessi;
nas_CDMASysInfo        cdmasi;
nas_HDRSysInfo         hdrsi;
nas_GSMSysInfo         gsmsi;
nas_WCDMASysInfo       wcdmasi;
nas_LTESysInfo         ltesi;
nas_AddCDMASysInfo     addcdmasi;
uint16_t               addhdrsi;
nas_AddSysInfo         addgsmsi;
nas_AddSysInfo         addwcdmasi;
uint16_t               addltesi;
nas_CallBarringSysInfo gsmcbsi;
nas_CallBarringSysInfo wcdmacbsi;
uint8_t                ltevssi;
uint8_t                gsmcdsi;
uint8_t                wcdmacdsi;
uint32_t               ciotlteopmode;

unpack_nas_SLQSGetSysInfo_t tunpack_nas_SLQSGetSysInfo = { &cdmassi, &hdrssi, &gsmssi,
        &wcdmassi, &ltessi, &cdmasi, &hdrsi, &gsmsi, &wcdmasi, &ltesi, &addcdmasi,
        &addhdrsi, &addgsmsi, &addwcdmasi, &addltesi, &gsmcbsi, &wcdmacbsi, &ltevssi,
        &gsmcdsi, &wcdmacdsi, &ciotlteopmode, SWI_UINT256_INT_VALUE };

nas_LteEmbmsCoverageTlv   LteEmbmsCoverage;
nas_SimRejInfoTlv         SimRejInfo;
nas_ImsVoiceSupportLteTlv ImsVoiceSupportLte;
nas_LteVoiceDomainTlv     LteVoiceDomain;
nas_SrvRegRestrictionTlv  SrvRegRestriction;
nas_LteRegDomainTlv       LteRegDomain;
nas_LteEmbmsTraceIdTlv    LteEmbmsTraceId;
nas_NR5GSrvStatusTlv      NR5GSrvStatusinfo;
nas_NR5GSysInfoTlv        NR5GSysInfo;
nas_NR5GCellStatusInfoTlv NR5GCellStatusInfo;

unpack_nas_SLQSGetSysInfoV2_t tunpack_nas_SLQSGetSysInfov2 = { &cdmassi, &hdrssi, &gsmssi,
        &wcdmassi, &ltessi, &cdmasi, &hdrsi, &gsmsi, &wcdmasi, &ltesi, &addcdmasi,
        &addhdrsi, &addgsmsi, &addwcdmasi, &addltesi, &gsmcbsi, &wcdmacbsi, &ltevssi,
        &gsmcdsi, &wcdmacdsi, &ciotlteopmode, &LteEmbmsCoverage, &SimRejInfo, &ImsVoiceSupportLte,
        &LteVoiceDomain, &SrvRegRestriction,&LteRegDomain, &LteEmbmsTraceId, &NR5GSrvStatusinfo,
        &NR5GSysInfo, &NR5GCellStatusInfo,SWI_UINT256_INT_VALUE };

uint8_t      CycleLen;
uint8_t      PagingTimeWindow;
uint8_t      EdrxEnable;
uint8_t      EdrxRAT;
uint32_t     LteOpMode;

unpack_nas_SLQSNASGeteDRXParams_t tunpack_nas_SLQSNASGeteDRXParams = { &CycleLen, &PagingTimeWindow, &EdrxEnable,
        SWI_UINT256_INT_VALUE};

uint8_t      EdrxRATReq = 8;
uint32_t     LteOpModeReq = 0;

pack_nas_SLQSNASGeteDRXParamsExt_t tpack_nas_SLQSNASGeteDRXParamsExt = { &EdrxRATReq, &LteOpModeReq};
unpack_nas_SLQSNASGeteDRXParamsExt_t tunpack_nas_SLQSNASGeteDRXParamsExt = { &CycleLen, &PagingTimeWindow, &EdrxEnable,
        &EdrxRAT, &LteOpMode, SWI_UINT256_INT_VALUE};

uint8_t InstanceSize[4] = {255,255,255,255};
struct nas_RFBandInfoElements lRFBandInfoParam[255];

struct nas_RfDedicatedBandInfoElements RfDedicatedBandInfoParam[255] ;
nas_RfDedicatedBandInfo      RfDedicatedBandInfo = {0, &InstanceSize[0], RfDedicatedBandInfoParam};
struct nas_RfBandInfoExtFormatElements RfBandInfoExtFormatParam[255];
nas_RfBandInfoExtFormat      RfBandInfoExtFormat= {0, &InstanceSize[1], RfBandInfoExtFormatParam};
struct nas_RfBandwidthInfoElements RfBandwidthInfoParam[255];
nas_RfBandwidthInfo          RfBandwidthInfo = {0, &InstanceSize[2], RfBandwidthInfoParam};
uint32_t lLTEOperationMode;
nas_LTEOperationMode         LTEOperationMode = {0, &lLTEOperationMode};

unpack_nas_SLQSNasGetRFInfo_t tunpack_nas_SLQSNasGetRFInfo = { {&InstanceSize[3], lRFBandInfoParam},
        &RfDedicatedBandInfo, &RfBandInfoExtFormat, &RfBandwidthInfo,
        &LTEOperationMode, 0xFFFF};

nas_ForbiddenNetworks3GPP forbiddenNetworks3GPP;
unpack_nas_SLQSNASGetForbiddenNetworks_t tunpack_nas_SLQSNASGetForbiddenNetworks = { &forbiddenNetworks3GPP,
        SWI_UINT256_INT_VALUE};


uint8_t ssPrefEmerMode = 0; // OFF, normal mode
uint16_t ssPrefModePref = 0x1F; // GSM, UMTS, LTE enabled
uint64_t ssPrefBandPref = 0xffffffffffffffffLL; // select all band
uint16_t ssPrefPRLPref = 0x3fff; // any available system
uint16_t ssPrefRoamPref = 0xff; // Acquire systems, regardless of their roaming indicator
uint64_t ssPrefLTEBandPref = 0x44; // BAND 3 and BAND 7 enabled
struct nas_netSelectionPref ssPrefNetSelPref = {0x01,402,3};
uint8_t ssPrefChgDuration = 0x00;
uint8_t ssPrefMNCIncPCSDigStat = 0x00;
uint32_t ssPrefSrvDomainPref = 0x00;
uint32_t ssPrefGWAcqOrderPref = 0x00;
uint8_t setAcqOrder = 0x09;


uint64_t                    ssPrefTdscdmaBandPref = 0x01; //TD-SCDMA Band A
struct nas_acqOrderPref     ssPrefAcqOrderPref = {0x01,&setAcqOrder};
uint32_t ssPrefSrvRegRestriction = 0x00;
unsigned char               ssPrefRAT = 0x09 ; // - 0x09 - TDSCDMA


pack_nas_SLQSSetSysSelectionPref_t tpack_nas_SLQSSetSysSelectionPref[] = {
        {
                &ssPrefEmerMode,
                &ssPrefModePref,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            },
            {
                NULL,
                NULL,
                &ssPrefBandPref,
                &ssPrefPRLPref,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            },
            {
                NULL,
                NULL,
                NULL,
                NULL,
                &ssPrefRoamPref,
                &ssPrefLTEBandPref,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            },
            {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                &ssPrefNetSelPref,
                NULL,
                NULL,
                &ssPrefSrvDomainPref,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
            }
};

swi_uint256_t unpack_nas_SLQSNasIndicationRegisterExtParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSNasIndicationRegisterExtParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_GetHomeNetworkParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,18,19)
}};

swi_uint256_t unpack_nas_GetHomeNetworkParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_GetRFInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,1,2,16,17,18,19)
}};

swi_uint256_t unpack_nas_GetRFInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SLQSNasGetSigInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_8_BITS,2,16,17,18,19,20,21,22)
}};

swi_uint256_t unpack_nas_SLQSNasGetSigInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSSetSysSelectionPrefParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSSetSysSelectionPrefParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_PerformNetworkScanParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_8_BITS,2,16,17,18,19,22,23,24)
}};

swi_uint256_t unpack_nas_PerformNetworkScanParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t  unpack_nas_SLQSSwiGetLteCQIParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t  unpack_nas_SLQSSwiGetLteCQIParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SLQSNasSwiModemStatusParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)
}};

swi_uint256_t unpack_nas_SLQSNasSwiModemStatusParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SLQSGetServingSystemParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_24_BITS,1,2,16,17,18,19,20,21,22,23,24,26,27,
            28,29,30,31,32,33,34,35,36,37,39)
}};

swi_uint256_t unpack_nas_SLQSGetServingSystemParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SLQSGetSignalStrengthParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_10_BITS,1,2,17,18,19,20,21,22,23,24)
}};

swi_uint256_t unpack_nas_SLQSGetSignalStrengthParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SlqsGetLTECphyCAInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)
}};

swi_uint256_t unpack_nas_SlqsGetLTECphyCAInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t  unpack_nas_SLQSInitiateNetworkRegistrationParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t  unpack_nas_SLQSInitiateNetworkRegistrationParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSNasSwiIndicationRegisterParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t  unpack_nas_SLQSNasSwiIndicationRegisterParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_13_BITS,2,16,17,18,19,20,21,22,23,24,28,34,41)
}};

swi_uint256_t  unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_SLQSGetSysInfoParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_32_BITS,2,16,17,18,19,20,21,22,23,24,25,26,
            27,28,29,30,31,32,33,34,35,38,39,41,42,47,49,52,73,74,75,76)
}};

swi_uint256_t unpack_nas_SLQSGetSysInfoParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_nas_GetNetworkPreferenceParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)
}};

swi_uint256_t unpack_nas_GetNetworkPreferenceParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_22_BITS,2,16,17,18,19,20,21,22,24,25,26,28,29,31,32,34,35,36,37,38,39,40)
}};

swi_uint256_t  unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};


void dump_SLQSNasIndicationRegisterExt (void *ptr)
{
    unpack_nas_SLQSNasIndicationRegisterExt_t *result =
        (unpack_nas_SLQSNasIndicationRegisterExt_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasIndicationRegisterExtParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasIndicationRegisterExtParamPresenceMaskMandatoryList,
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

void dump_SLQSNasIndicationRegisterV2 (void *ptr)
{
    unpack_nas_SLQSNasIndicationRegisterV2_t *result =
        (unpack_nas_SLQSNasIndicationRegisterV2_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSetSignalStrengthsCallback (void *ptr)
{
    unpack_nas_SLQSSetSignalStrengthsCallback_t *result =
        (unpack_nas_SLQSSetSignalStrengthsCallback_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetRFInfoCallback (void *ptr)
{
    unpack_nas_SetRFInfoCallback_t *result =
        (unpack_nas_SetRFInfoCallback_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SetLURejectCallback (void *ptr)
{
    unpack_nas_SetLURejectCallback_t *result =
        (unpack_nas_SetLURejectCallback_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_GetHomeNetwork(void *ptr)
{ 
    unpack_nas_GetHomeNetwork_t *phomenw =
        (unpack_nas_GetHomeNetwork_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (phomenw->ParamPresenceMask);
    if ( phomenw != NULL)
    {
        if(swi_uint256_get_bit (phomenw->ParamPresenceMask, 1))
        {
            printf("mcc: %d\n", phomenw->mcc);
            printf("mnc: %d\n", phomenw->mnc);
            printf("home nw: %s\n", phomenw->name);
        }
        if(swi_uint256_get_bit (phomenw->ParamPresenceMask, 16))
        {
            printf("sytem id: %d\n", phomenw->sid);
            printf("network id: %d\n", phomenw->nid);
       }
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    CHECK_WHITELIST_MASK(
            unpack_nas_GetHomeNetworkParamPresenceMaskWhiteList,
            phomenw->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_nas_GetHomeNetworkParamPresenceMaskMandatoryList,
            phomenw->ParamPresenceMask);

    /* mcc and mnc should not equal to 0 */
    if ( phomenw != NULL)
    {
        if ( (phomenw->mcc != MCC) ||
              (phomenw->mnc != MNC) ||
              (strcmp(phomenw->name, (char*)Name) != 0))
            is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetHomeNetwork(void *ptr)
{
    unpack_nas_SLQSGetHomeNetwork_t *phomenw =
        (unpack_nas_SLQSGetHomeNetwork_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (phomenw->ParamPresenceMask);
    if ( phomenw != NULL)
    {
        if(swi_uint256_get_bit (phomenw->ParamPresenceMask, 1))
        {
            printf("mcc: %d\n", phomenw->mcc);
            printf("mnc: %d\n", phomenw->mnc);
            printf("home nw: %s\n", phomenw->name);
        }
        if(swi_uint256_get_bit (phomenw->ParamPresenceMask, 16))
        {
            printf("sytem id: %d\n", phomenw->sid);
            printf("network id: %d\n", phomenw->nid);
        }
        if((phomenw->pHomeNwMNC3Gpp) && (swi_uint256_get_bit (phomenw->ParamPresenceMask, 18)))
        {
            printf("is 3Gpp Nw  : %d\n",phomenw->pHomeNwMNC3Gpp->is3GppNw);
            printf("MCC Inc Pcs Digit : %d\n",phomenw->pHomeNwMNC3Gpp->mccIncPcsDigit);
        }
        if((phomenw->pNwNameSrc3Gpp) && (swi_uint256_get_bit (phomenw->ParamPresenceMask, 19)))
            printf("3GPP Network Name Source  : %d\n",phomenw->pNwNameSrc3Gpp->NwNameSrc3Gpp);
    }
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    CHECK_WHITELIST_MASK(
            unpack_nas_GetHomeNetworkParamPresenceMaskWhiteList,
            phomenw->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
            unpack_nas_GetHomeNetworkParamPresenceMaskMandatoryList,
            phomenw->ParamPresenceMask);

    /* mcc and mnc should not equal to 0 */
    if ( phomenw != NULL)
    {
        if ( (phomenw->mcc != MCC) ||
              (phomenw->mnc != MNC) ||
              (strcmp(phomenw->name, (char*)Name) != 0))
            is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_GetRFInfo(void* ptr)
{
    unpack_nas_GetRFInfo_t *prfinfo =
        (unpack_nas_GetRFInfo_t*) ptr;
    uint8_t    count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (prfinfo->ParamPresenceMask);
    if(swi_uint256_get_bit (prfinfo->ParamPresenceMask, 1))
    {
        for(count = 0; count < prfinfo->instancesSize; count++)
        {
            printf("radioInterface[%d] : %d\n",
                    count, prfinfo->RFBandInfoElements[count].radioInterface);
            printf("activeBandClass[%d] : %d\n",
                    count, prfinfo->RFBandInfoElements[count].activeBandClass);
            printf("activeChannel[%d] : %d\n",
                    count, prfinfo->RFBandInfoElements[count].activeChannel);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_GetRFInfoParamPresenceMaskWhiteList,
        prfinfo->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_GetRFInfoParamPresenceMaskMandatoryList,
        prfinfo->ParamPresenceMask);

    /* radio interface range is 1-9 */
    if ((prfinfo->RFBandInfoElements[0].radioInterface == 0) ||
        (prfinfo->RFBandInfoElements[0].radioInterface > 9))
    {
        is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG: FAILED_MSG));
    }
#endif

}

void dump_SLQSNasGetSigInfo(void* ptr)
{
    unpack_nas_SLQSNasGetSigInfo_t *psiginfo =
        (unpack_nas_SLQSNasGetSigInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (psiginfo->ParamPresenceMask);

    if(swi_uint256_get_bit (psiginfo->ParamPresenceMask, 16))
    {
        printf("cdma ss information \n");
        printf("rssi  : %d\n", psiginfo->CDMASSInfo.rssi);
        printf("ecio  : %d\n", psiginfo->CDMASSInfo.ecio);
    }
    if(swi_uint256_get_bit (psiginfo->ParamPresenceMask, 17))
    {
        printf("hdr ss information \n");
        printf("rssi  : %d\n", psiginfo->HDRSSInfo.rssi);
        printf("ecio  : %d\n", psiginfo->HDRSSInfo.ecio);
        printf("sinr  : %d\n", psiginfo->HDRSSInfo.sinr);
        printf("io    : %d\n", psiginfo->HDRSSInfo.io);
    }

    if(swi_uint256_get_bit (psiginfo->ParamPresenceMask, 18))
    {
        printf("gsm sig info : %d\n", psiginfo->GSMSSInfo);
    }

    if(swi_uint256_get_bit (psiginfo->ParamPresenceMask, 19))
    {
        printf("wcdma ss information \n");
        printf("rssi  : %d\n", psiginfo->WCDMASSInfo.rssi);
        printf("ecio  : %d\n", psiginfo->WCDMASSInfo.ecio);
    }

    if(swi_uint256_get_bit (psiginfo->ParamPresenceMask, 20))
    {
        printf("lte ss information \n");
        printf("rssi  : %d\n", psiginfo->LTESSInfo.rssi);
        printf("rsrq  : %d\n", psiginfo->LTESSInfo.rsrq);
        printf("rsrp  : %d\n", psiginfo->LTESSInfo.rsrp);
        printf("snr   : %d\n", psiginfo->LTESSInfo.snr);
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasGetSigInfoParamPresenceMaskWhiteList,
        psiginfo->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasGetSigInfoParamPresenceMaskMandatoryList,
        psiginfo->ParamPresenceMask);

    if ((psiginfo->CDMASSInfo.rssi != 0) ||
       (psiginfo->HDRSSInfo.rssi != 0) ||
       (psiginfo->GSMSSInfo != 0) ||
       (psiginfo->WCDMASSInfo.rssi != 0) ||
       (psiginfo->LTESSInfo.rssi != 0))
        {
            is_matching = 1;
        }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSSetSysSelectionPref (void *ptr)
{
    unpack_nas_SLQSSetSysSelectionPref_t *pSetSysSelPref =
        (unpack_nas_SLQSSetSysSelectionPref_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pSetSysSelPref->ParamPresenceMask);
    if(swi_uint256_get_bit (pSetSysSelPref->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, pSetSysSelPref->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSSetSysSelectionPrefParamPresenceMaskWhiteList,
        pSetSysSelPref->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSSetSysSelectionPrefParamPresenceMaskMandatoryList,
        pSetSysSelPref->ParamPresenceMask);

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

void dump_SLQSGetSysSelectionPref (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(pGetSysSelPref!=NULL)
    {
        if( (pGetSysSelPref->pEmerMode!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 16)) )
            printf( "Emergency Mode: 0x%x\n",               *pGetSysSelPref->pEmerMode );
        if( (pGetSysSelPref->pModePref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 17)) )
            printf( "Mode Preference: 0x%x\n",              *pGetSysSelPref->pModePref );
        if( (pGetSysSelPref->pBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 18)) )
            printf( "Band Preference: 0x%"PRIX64"\n",       *pGetSysSelPref->pBandPref );
        if( (pGetSysSelPref->pPRLPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 19)) )
            printf( "CDMA PRL Preference : 0x%x\n",         *pGetSysSelPref->pPRLPref );
        if( (pGetSysSelPref->pRoamPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 20)) )
            printf( "Roaming Preference : 0x%x\n",          *pGetSysSelPref->pRoamPref );
        if( (pGetSysSelPref->pLTEBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 21)) )
            printf( "LTE Band Preference : 0x%"PRIX64"\n",  *pGetSysSelPref->pLTEBandPref );
        if( (pGetSysSelPref->pNetSelPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 22)) )
            printf( "Network Selection Preference: 0x%x\n", *pGetSysSelPref->pNetSelPref );
        if( (pGetSysSelPref->pSrvDomainPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 24)) )
            printf( "Service Domain Preference : 0x%x\n",  *pGetSysSelPref->pSrvDomainPref );
        if( (pGetSysSelPref->pGWAcqOrderPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 25)) )
            printf( "GSM - WCDMA Acquisition Order Preference : 0x%x\n",*pGetSysSelPref->pGWAcqOrderPref );
    }


#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;
    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
        (pGetSysSelPref->pModePref!=NULL))
    {
        /* mcc and mnc should not equal to 0 */
        if ((*pGetSysSelPref->pEmerMode == 0) &&
           (*pGetSysSelPref->pModePref == 0x18))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif

}

void dump_SLQSGetSysSelectionPrefRestoreSettings (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    if(pGetSysSelPref!=NULL)
    {
        if( (pGetSysSelPref->pEmerMode!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 16)) )
            if(*ModemSetSysSelPref.pEmerMode != *pGetSysSelPref->pEmerMode)
                is_matching = 0;
        if( (pGetSysSelPref->pModePref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 17)) )
            if(*ModemSetSysSelPref.pModePref != *pGetSysSelPref->pModePref)
                is_matching = 0;
        if( (pGetSysSelPref->pBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 18)) )
            if(*ModemSetSysSelPref.pBandPref != *pGetSysSelPref->pBandPref)
                is_matching = 0;
        if( (pGetSysSelPref->pPRLPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 19)) )
            if(*ModemSetSysSelPref.pPRLPref != *pGetSysSelPref->pPRLPref)
                is_matching = 0;
        if( (pGetSysSelPref->pRoamPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 20)) )
            if(*ModemSetSysSelPref.pRoamPref != *pGetSysSelPref->pRoamPref)
                is_matching = 0;
        if( (pGetSysSelPref->pLTEBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 21)) )
            if(*ModemSetSysSelPref.pLTEBandPref != *pGetSysSelPref->pLTEBandPref)
                is_matching = 0;
        if( (pGetSysSelPref->pSrvDomainPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 24)) )
            if(*ModemSetSysSelPref.pSrvDomainPref != *pGetSysSelPref->pSrvDomainPref)
                is_matching = 0;
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_SLQSGetSysSelectionPref_EmerMode_ModePref (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
        (pGetSysSelPref->pModePref!=NULL))
    {
        if ((*pGetSysSelPref->pEmerMode == *tpack_nas_SLQSSetSysSelectionPref[0].pEmerMode) &&
           (*pGetSysSelPref->pModePref == *tpack_nas_SLQSSetSysSelectionPref[0].pModePref))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SLQSGetSysSelectionPref_Band_PrlPref (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pBandPref!=NULL)&&
        (pGetSysSelPref->pPRLPref!=NULL))
    {
        if ((*pGetSysSelPref->pBandPref == *tpack_nas_SLQSSetSysSelectionPref[1].pBandPref) &&
           (*pGetSysSelPref->pPRLPref == *tpack_nas_SLQSSetSysSelectionPref[1].pPRLPref))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SLQSGetSysSelectionPref_Roam_LteBand (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pRoamPref!=NULL)&&
        (pGetSysSelPref->pLTEBandPref!=NULL))
    {
        if ((*pGetSysSelPref->pRoamPref == *tpack_nas_SLQSSetSysSelectionPref[2].pRoamPref) &&
           (*pGetSysSelPref->pLTEBandPref == *tpack_nas_SLQSSetSysSelectionPref[2].pLTEBandPref))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_SLQSGetSysSelectionPref_NetSel_SrvDomain (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pNetSelPref!=NULL)&&
        (pGetSysSelPref->pSrvDomainPref!=NULL))
    {
        if ((*pGetSysSelPref->pNetSelPref == tpack_nas_SLQSSetSysSelectionPref[3].pNetSelPref->netReg) &&
           (*pGetSysSelPref->pSrvDomainPref == *tpack_nas_SLQSSetSysSelectionPref[3].pSrvDomainPref))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
#endif
}

void dump_GetServingNetwork (void *ptr)
{
    unpack_nas_GetServingNetwork_t *pServingNW =
    (unpack_nas_GetServingNetwork_t*) ptr;
    uint8_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pServingNW->ParamPresenceMask);
    if(pServingNW!=NULL)
    {
        if(swi_uint256_get_bit (pServingNW->ParamPresenceMask, 1))
        {
            printf("RegistrationState  : %d\n", pServingNW->RegistrationState);
            printf("CSDomain  : %d\n", pServingNW->CSDomain);
            printf("PSDomain : %d\n", pServingNW->PSDomain);
            printf("RAN  : %d\n", pServingNW->RAN);
            printf("RadioIfacesSize  : %d\n", pServingNW->RadioIfacesSize);
            for(count = 0; count < pServingNW->RadioIfacesSize; count++)
                printf("RadioIfaces  : %d\n", pServingNW->RadioIfaces[count]);
        }
        if(swi_uint256_get_bit (pServingNW->ParamPresenceMask, 16))
            printf("Roaming  : %d\n", pServingNW->Roaming);
        if(swi_uint256_get_bit (pServingNW->ParamPresenceMask, 18))
        {
            printf("MCC  : %d\n", pServingNW->MCC);
            printf("MNC  : %d\n", pServingNW->MNC);
            if(pServingNW->nameSize>0)
                printf("Name  : %s\n", pServingNW->Name);
        }
        if(swi_uint256_get_bit (pServingNW->ParamPresenceMask, 17))
        {
            printf("Data Service Capability Len  : %d\n", pServingNW->DataCapsLen);
            for(count = 0; count < pServingNW->DataCapsLen; count++)
                printf("Data Capability  : 0x%02x\n", pServingNW->DataCaps[count]);
        }
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;
    if(pServingNW!=NULL)
    {
        /* mcc and mnc should not equal to 0 */
        if ((pServingNW->RegistrationState == 1) &&
           (pServingNW->RadioIfacesSize > 0))
        {
            is_matching = 1;
        }
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif

}

void dump_GetServingNetworkCapabilities (void *ptr)
{
    unpack_nas_GetServingNetworkCapabilities_t *pNwCap =
    (unpack_nas_GetServingNetworkCapabilities_t*) ptr;
    uint8_t i;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pNwCap->ParamPresenceMask);
    if(swi_uint256_get_bit (pNwCap->ParamPresenceMask, 17))
    {
        printf("Data Service Capability Len  : %d\n", pNwCap->DataCapsLen);
        for(i = 0; i < pNwCap->DataCapsLen; i++)
            printf("Data Capability   : 0x%02x\n", pNwCap->DataCaps[i]);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* mcc and mnc should not equal to 0 */
    if ( pNwCap->DataCapsLen == 0) 
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif    

}

void dump_PerformNetworkScan (void *ptr)
{
    unpack_nas_PerformNetworkScan_t *pNetworkScan =
    (unpack_nas_PerformNetworkScan_t*) ptr;
    uint8_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(ptr==NULL)
        return;

    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pNetworkScan->ParamPresenceMask);

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 16))
    {
        if(pNetworkScan->p3GppNetworkInstanceSize)
        {
            if(pNetworkScan->p3GppNetworkInfoInstances)
            {
                printf("NetworkInfoInstances from network : %d\n",(int)*(pNetworkScan->p3GppNetworkInstanceSize));
                for(count = 0; count < *(pNetworkScan->p3GppNetworkInstanceSize); count++)
                {
                    printf("\tMCC[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].MCC);
                    printf("\tMNC[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].MNC);
                    printf("\tInUse[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].InUse);
                    printf("\tRoaming[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Roaming);
                    printf("\tForbidden[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Forbidden);
                    printf("\tPreferred[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Preferred);
                    printf("\tDesription[%d] : %s\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Desription);
                }
            }
        }
    }

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 17))
    {
        if(pNetworkScan->pRATInstanceSize)
        {
            if(pNetworkScan->pRATINstance)
            {
                printf("RATInstances from network : %d\n",(int)*(pNetworkScan->pRATInstanceSize));
                for(count = 0; count < *(pNetworkScan->pRATInstanceSize); count++)
                {
                    printf("\tRAT-MCC[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].MCC);
                    printf("\tRAT-MNC[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].MNC);
                    printf("\tRAT-Technology[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].RAT);
                }
            }
        }
    }

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 18))
    {
         if(pNetworkScan->pPCSInstanceSize &&
                (*pNetworkScan->pPCSInstanceSize != NAS_MAX_NUM_NETWORKS)  )
        {
            if(pNetworkScan->pPCSInstance)
            {
                printf("PCSInfoInstances from network : %d\n",(int)*(pNetworkScan->pPCSInstanceSize));
                for(count = 0; count < *(pNetworkScan->pPCSInstanceSize); count++)
                {
                    printf("\tMCC[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].MCC);
                    printf("\tMNC[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].MNC);
                    printf("\tPCS-Digit-included[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].includes_pcs_digit);
                }
            }
        }
    }

    if((pNetworkScan->pScanResult) && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 19)) )
    {
        printf("Scan Result : %u\n", 
            (uint32_t)*(pNetworkScan->pScanResult));
    }

    if((pNetworkScan->pPCIInfo) && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 23)) )
    {
        uint8_t u8NumOfPCICellInfo = 0;
        printf("PCICellInfoLen : %d\n",pNetworkScan->pPCIInfo->PCICellInfoLen);
        for(u8NumOfPCICellInfo=0;
            u8NumOfPCICellInfo < pNetworkScan->pPCIInfo->PCICellInfoLen;
            u8NumOfPCICellInfo++)
        {            
            nas_QmisNasSlqsNasPCICellInfo *pNasQmisNasSlqsNasPCICellInfo = NULL;
            uint8_t NumOfPlmn;
            pNasQmisNasSlqsNasPCICellInfo = &pNetworkScan->pPCIInfo->nasQmisNasSlqsNasPCICellInfo[u8NumOfPCICellInfo];
            printf("PCI Cell Info : %d/%d\n",u8NumOfPCICellInfo+1, pNetworkScan->pPCIInfo->PCICellInfoLen);
            printf("\tFrequency : %d\n",
                pNasQmisNasSlqsNasPCICellInfo->freq);
            printf("\tcellID : 0x%x\n",
                pNasQmisNasSlqsNasPCICellInfo->cellID);
            printf("\tGolbal Cell ID : 0x%x\n",
                pNasQmisNasSlqsNasPCICellInfo->GlobalCellID);
            printf("\tNumber of Plmn : 0x%d\n",
                pNasQmisNasSlqsNasPCICellInfo->PlmnLen);
            for(NumOfPlmn=0;
                NumOfPlmn<pNasQmisNasSlqsNasPCICellInfo->PlmnLen;
                NumOfPlmn++)
            {
                nas_QmisNasPcsDigit *pnasQmisNasPcsDigit = NULL;
                pnasQmisNasPcsDigit = &pNasQmisNasSlqsNasPCICellInfo->nasQmisNasPcsDigit[NumOfPlmn];
                printf("\t\tPlmn Info: 0x%d/%d\n",
                NumOfPlmn+1,
                pNasQmisNasSlqsNasPCICellInfo->PlmnLen);
                printf("\t\tMCC: 0x%d\n",
                    pnasQmisNasPcsDigit->MCC);
                printf("\t\tMNC: 0x%d\n",
                    pnasQmisNasPcsDigit->MNC);
                if(pnasQmisNasPcsDigit->includes_pcs_digit == 1)
                {
                    printf("\t\tMNC is a three-digit value\n");
                }
                else if(pnasQmisNasPcsDigit->includes_pcs_digit == 0)
                {
                    printf("\t\tMNC is a two-digit value\n");
                }
                else
                {
                    printf("\t\tunexpected value\n");
                }
            }
        }
    }
    if (pNetworkScan->pLteOpModeTlv
              && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 26)) )
    {
        printf( "LTE Operational Mode Length : %d\n",
                pNetworkScan->pLteOpModeTlv->lteOpModeLen );
        for(count = 0; count < pNetworkScan->pLteOpModeTlv->lteOpModeLen; count++)
        {
            printf("MCC[%d]        : %d\n",
                  count, pNetworkScan->pLteOpModeTlv->MCC[count]);
            printf( "MNC[%d]        : %d\n",
                  count, pNetworkScan->pLteOpModeTlv->MNC[count]);
            printf("LTE Operational Mode[%d]      : %u\n",
                  count, pNetworkScan->pLteOpModeTlv->lteOpMode[count]);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_nas_PerformNetworkScanParamPresenceMaskWhiteList,
        pNetworkScan->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_PerformNetworkScanParamPresenceMaskMandatoryList,
        pNetworkScan->ParamPresenceMask);

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 16))
    {
        if(pNetworkScan->p3GppNetworkInstanceSize)
        {
            /* at least one network was searched, mcc and mnc should not equal to 0 */
            if(pNetworkScan->p3GppNetworkInfoInstances)
            {
                if ((*(pNetworkScan->p3GppNetworkInstanceSize) > 0) &&
                    (pNetworkScan->p3GppNetworkInfoInstances[0].MCC > 0) &&
                    (pNetworkScan->p3GppNetworkInfoInstances[0].MNC > 0))
                {
                    is_matching = 1;
                }
            } 
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSPerformNetworkScanV2 (void *ptr)
{
    unpack_nas_SLQSPerformNetworkScanV2_t *pNetworkScan =
    (unpack_nas_SLQSPerformNetworkScanV2_t*) ptr;
    uint8_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(ptr==NULL)
        return;

    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pNetworkScan->ParamPresenceMask);

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 16))
    {
        if(pNetworkScan->p3GppNetworkInstanceSize)
        {
            if(pNetworkScan->p3GppNetworkInfoInstances)
            {
                printf("NetworkInfoInstances from network : %d\n",(int)*(pNetworkScan->p3GppNetworkInstanceSize));
                for(count = 0; count < *(pNetworkScan->p3GppNetworkInstanceSize); count++)
                {
                    printf("\tMCC[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].MCC);
                    printf("\tMNC[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].MNC);
                    printf("\tInUse[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].InUse);
                    printf("\tRoaming[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Roaming);
                    printf("\tForbidden[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Forbidden);
                    printf("\tPreferred[%d] : %d\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Preferred);
                    printf("\tDesription[%d] : %s\n",
                        count, pNetworkScan->p3GppNetworkInfoInstances[count].Desription);
                }
            }
        }
    }

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 17))
    {
        if(pNetworkScan->pRATInstanceSize)
        {
            if(pNetworkScan->pRATINstance)
            {
                printf("RATInstances from network : %d\n",(int)*(pNetworkScan->pRATInstanceSize));
                for(count = 0; count < *(pNetworkScan->pRATInstanceSize); count++)
                {
                    printf("\tRAT-MCC[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].MCC);
                    printf("\tRAT-MNC[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].MNC);
                    printf("\tRAT-Technology[%d] : %d\n",
                        count, pNetworkScan->pRATINstance[count].RAT);
                }
            }
        }
    }

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 18))
    {
         if(pNetworkScan->pPCSInstanceSize &&
                (*pNetworkScan->pPCSInstanceSize != NAS_MAX_NUM_NETWORKS)  )
        {
            if(pNetworkScan->pPCSInstance)
            {
                printf("PCSInfoInstances from network : %d\n",(int)*(pNetworkScan->pPCSInstanceSize));
                for(count = 0; count < *(pNetworkScan->pPCSInstanceSize); count++)
                {
                    printf("\tMCC[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].MCC);
                    printf("\tMNC[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].MNC);
                    printf("\tPCS-Digit-included[%d] : %d\n",
                        count, pNetworkScan->pPCSInstance[count].includes_pcs_digit);
                }
            }
        }
    }

    if((pNetworkScan->pScanResult) && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 19)) )
    {
        printf("Scan Result : %u\n",
            (uint32_t)*(pNetworkScan->pScanResult));
    }

    if((pNetworkScan->pPCIInfo) && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 23)) )
    {
        uint8_t u8NumOfPCICellInfo = 0;
        printf("PCICellInfoLen : %d\n",pNetworkScan->pPCIInfo->PCICellInfoLen);
        for(u8NumOfPCICellInfo=0;
            u8NumOfPCICellInfo < pNetworkScan->pPCIInfo->PCICellInfoLen;
            u8NumOfPCICellInfo++)
        {
            nas_QmisNasSlqsNasPCICellInfo *pNasQmisNasSlqsNasPCICellInfo = NULL;
            uint8_t NumOfPlmn;
            pNasQmisNasSlqsNasPCICellInfo = &pNetworkScan->pPCIInfo->nasQmisNasSlqsNasPCICellInfo[u8NumOfPCICellInfo];
            printf("PCI Cell Info : %d/%d\n",u8NumOfPCICellInfo+1, pNetworkScan->pPCIInfo->PCICellInfoLen);
            printf("\tFrequency : %d\n",
                pNasQmisNasSlqsNasPCICellInfo->freq);
            printf("\tcellID : 0x%x\n",
                pNasQmisNasSlqsNasPCICellInfo->cellID);
            printf("\tGolbal Cell ID : 0x%x\n",
                pNasQmisNasSlqsNasPCICellInfo->GlobalCellID);
            printf("\tNumber of Plmn : 0x%d\n",
                pNasQmisNasSlqsNasPCICellInfo->PlmnLen);
            for(NumOfPlmn=0;
                NumOfPlmn<pNasQmisNasSlqsNasPCICellInfo->PlmnLen;
                NumOfPlmn++)
            {
                nas_QmisNasPcsDigit *pnasQmisNasPcsDigit = NULL;
                pnasQmisNasPcsDigit = &pNasQmisNasSlqsNasPCICellInfo->nasQmisNasPcsDigit[NumOfPlmn];
                printf("\t\tPlmn Info: 0x%d/%d\n",
                NumOfPlmn+1,
                pNasQmisNasSlqsNasPCICellInfo->PlmnLen);
                printf("\t\tMCC: 0x%d\n",
                    pnasQmisNasPcsDigit->MCC);
                printf("\t\tMNC: 0x%d\n",
                    pnasQmisNasPcsDigit->MNC);
                if(pnasQmisNasPcsDigit->includes_pcs_digit == 1)
                {
                    printf("\t\tMNC is a three-digit value\n");
                }
                else if(pnasQmisNasPcsDigit->includes_pcs_digit == 0)
                {
                    printf("\t\tMNC is a two-digit value\n");
                }
                else
                {
                    printf("\t\tunexpected value\n");
                }
            }
        }
    }
    if (pNetworkScan->pLteOpModeTlv
              && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 26)) )
    {
        printf( "LTE Operational Mode Length : %d\n",
                pNetworkScan->pLteOpModeTlv->lteOpModeLen );
        for(count = 0; count < pNetworkScan->pLteOpModeTlv->lteOpModeLen; count++)
        {
            printf("MCC[%d]        : %d\n",
                  count, pNetworkScan->pLteOpModeTlv->MCC[count]);
            printf( "MNC[%d]        : %d\n",
                  count, pNetworkScan->pLteOpModeTlv->MNC[count]);
            printf("LTE Operational Mode[%d]      : %u\n",
                  count, pNetworkScan->pLteOpModeTlv->lteOpMode[count]);
        }
    }
    if (pNetworkScan->pNetworkNameSrcTlv
              && (swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 22)) )
    {
        printf("Network Name Source Length : %d\n",
                            pNetworkScan->pNetworkNameSrcTlv->nwNameSrcLen );
        for (count = 0; count < pNetworkScan->pNetworkNameSrcTlv->nwNameSrcLen; count++)
        {
            printf( "Network Name Source[%d] : %d\n",
                     count, pNetworkScan->pNetworkNameSrcTlv->nwNameSrc[count]);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    CHECK_WHITELIST_MASK(
        unpack_nas_PerformNetworkScanParamPresenceMaskWhiteList,
        pNetworkScan->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_PerformNetworkScanParamPresenceMaskMandatoryList,
        pNetworkScan->ParamPresenceMask);

    if(swi_uint256_get_bit (pNetworkScan->ParamPresenceMask, 16))
    {
        if(pNetworkScan->p3GppNetworkInstanceSize)
        {
            /* at least one network was searched, mcc and mnc should not equal to 0 */
            if(pNetworkScan->p3GppNetworkInfoInstances)
            {
                if ((*(pNetworkScan->p3GppNetworkInstanceSize) > 0) &&
                    (pNetworkScan->p3GppNetworkInfoInstances[0].MCC > 0) &&
                    (pNetworkScan->p3GppNetworkInfoInstances[0].MNC > 0))
                {
                    is_matching = 1;
                }
            }
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSSwiGetLteCQI (void *ptr)
{
    unpack_nas_SLQSSwiGetLteCQI_t *pSwiGetLteCqi =
    (unpack_nas_SLQSSwiGetLteCQI_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pSwiGetLteCqi->ParamPresenceMask);
    if(swi_uint256_get_bit (pSwiGetLteCqi->ParamPresenceMask, 1))
    {
        printf( "Validity of cw0  : 0x%x\n", pSwiGetLteCqi->ValidityCW0 );
        printf( "CQI value of cw0 : 0x%x\n", pSwiGetLteCqi->CQIValueCW0 );
        printf( "Validity of cw1  : 0x%x\n", pSwiGetLteCqi->ValidityCW1 );
        printf( "CQI value of cw1 : 0x%x\n", pSwiGetLteCqi->CQIValueCW1 );
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSSwiGetLteCQIParamPresenceMaskWhiteList,
        pSwiGetLteCqi->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSSwiGetLteCQIParamPresenceMaskMandatoryList,
        pSwiGetLteCqi->ParamPresenceMask);

    /* validity should be either 0 or 1 */
    if ((pSwiGetLteCqi->ValidityCW0 > 1) ||
        (pSwiGetLteCqi->ValidityCW1 > 1) )
    {
        is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_SLQSNasSwiModemStatus (void *ptr)
{
    unpack_nas_SLQSNasSwiModemStatus_t *pSwiGetModSta =
    (unpack_nas_SLQSNasSwiModemStatus_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pSwiGetModSta->ParamPresenceMask);

    if(swi_uint256_get_bit (pSwiGetModSta->ParamPresenceMask, 1))
    {
        printf( "Modem Temperature : %d\n",pSwiGetModSta->commonInfo.temperature );
        printf( "Modem Mode        : 0x%02x\n",pSwiGetModSta->commonInfo.modemMode );
        printf( "System Mode       : 0x%02x\n",pSwiGetModSta->commonInfo.systemMode );
        printf( "IMS Reg State     : 0x%02x\n",pSwiGetModSta->commonInfo.imsRegState );
        printf( "PS Attach State   : 0x%02x\n",pSwiGetModSta->commonInfo.psState );
    }
    if (( pSwiGetModSta->pLTEInfo ) && (swi_uint256_get_bit (pSwiGetModSta->ParamPresenceMask, 16)) )
    {
        printf( "Band              : 0x%02x\n",pSwiGetModSta->pLTEInfo->band );
        printf( "BandWidth         : 0x%02x\n",pSwiGetModSta->pLTEInfo->bandwidth );
        printf( "RX Channel        : 0x%02x\n",pSwiGetModSta->pLTEInfo->RXChan );
        printf( "TX Channel        : 0x%02x\n",pSwiGetModSta->pLTEInfo->TXChan );
        printf( "EMM State         : 0x%02x\n",pSwiGetModSta->pLTEInfo->emmState );
        printf( "EMM Sub State     : 0x%02x\n",pSwiGetModSta->pLTEInfo->emmSubState );
        printf( "EMM Conn State    : 0x%02x\n",pSwiGetModSta->pLTEInfo->emmConnState );
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasSwiModemStatusParamPresenceMaskWhiteList,
        pSwiGetModSta->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasSwiModemStatusParamPresenceMaskMandatoryList,
        pSwiGetModSta->ParamPresenceMask);

    /* modem mode should not equal to 0 (power off), system mode should not be Unknown */
    if ((pSwiGetModSta->commonInfo.modemMode == 0) ||
        (pSwiGetModSta->commonInfo.systemMode == 0) ||
        (pSwiGetModSta->commonInfo.psState == 0xFF) )
    {
        is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetServingSystem (void *ptr)
{
    unpack_nas_SLQSGetServingSystem_t *pServingSystem =
    (unpack_nas_SLQSGetServingSystem_t*) ptr;
    uint8_t index, ctr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pServingSystem->ParamPresenceMask);

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 1))
    {
        printf( "Registration state: 0x%x\n",         pServingSystem->ServingSystem.regState );
        printf( "CS Attach State: 0x%x\n",            pServingSystem->ServingSystem.csAttachState );
        printf( "PS Attach State: 0x%x\n",            pServingSystem->ServingSystem.psAttachState );
        printf( "Selected Network: 0x%x\n",           pServingSystem->ServingSystem.selNetwork );
        printf( "Number of Radio Interfaces: 0x%x\n", pServingSystem->ServingSystem.numRadioInterfaces );
        ctr = pServingSystem->ServingSystem.numRadioInterfaces;
        for( index = 0; index < ctr; index++ )
        {
            printf( "Radio Interface in use : 0x%x\n", pServingSystem->ServingSystem.radioInterface[index] );
        }
    printf( "\n" );
    }
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 16))
        printf( "Roaming Indicator value: 0x%x\n",     pServingSystem->RoamIndicatorVal );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 17))
    {
        printf( "\n Data Service capability \n" );
        printf( "Data Capability List Length: 0x%x\n", pServingSystem->DataSrvCapabilities.dataCapabilitiesLen );
        ctr = pServingSystem->DataSrvCapabilities.dataCapabilitiesLen;
        for( index = 0; index < ctr; index++ )
        {
            printf( "Data Capability : 0x%x\n", pServingSystem->DataSrvCapabilities.dataCapabilities[index] );
        }
    }
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 18))
    {
        printf( "\n Current PLMN Information \n" );
        printf( "Mobile Country Code: 0x%x\n",        pServingSystem->CurrentPLMN.MCC );
        printf( "Mobile Country Code: 0x%x\n",        pServingSystem->CurrentPLMN.MNC );
        printf( "Network Description Length: 0x%x\n", pServingSystem->CurrentPLMN.netDescrLength );
        ctr = pServingSystem->CurrentPLMN.netDescrLength;

            printf( "Network Description : ");
            for( index = 0; index < ctr; index++ )
            {
                printf( "%c",  (char) pServingSystem->CurrentPLMN.netDescr[index] );
            }
            printf( "\n");
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 19))
    {
        printf( "\n CDMA System ID \n" );
        printf( "System ID: 0x%x\n",                  pServingSystem->SystemID );
        printf( "Network ID: 0x%x\n",                 pServingSystem->NetworkID );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 20))
    {
        printf( "\n CDMA Base Station Information \n" );
        printf( "Base ID: 0x%x\n",                    pServingSystem->BasestationID );
        printf( "Base Station latitude: 0x%x\n",     pServingSystem->BasestationLatitude );
        printf( "Base Station longitude: 0x%x\n",    pServingSystem->BasestationLongitude );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 21))
    {
        printf( "\n Roaming Indicator List \n" );
        printf( "Number of Instances: 0x%x\n",        pServingSystem->RoamingIndicatorList.numInstances );
        ctr = pServingSystem->RoamingIndicatorList.numInstances;
            for( index = 0; index < ctr; index++ )
            {
                printf( "Radio Interface : 0x%x\n",   pServingSystem->RoamingIndicatorList.radioInterface[index] );
                printf( "Roaming Indicator : 0x%x\n", pServingSystem->RoamingIndicatorList.roamIndicator[index] );
            }

    printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 22))
        printf( "Default Roaming Indicator: 0x%x\n",  pServingSystem->DefaultRoamInd );
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 23))
    {
        printf( "\n 3GPP2 Time Zone \n" );
        printf( "Leap Seconds: 0x%x\n",               pServingSystem->Gpp2TimeZone.leapSeconds );
        printf( "Local Time Offset: 0x%x\n",          pServingSystem->Gpp2TimeZone.localTimeOffset );
        printf( "Daylight Saving indicator: 0x%x\n",  pServingSystem->Gpp2TimeZone.daylightSavings );
        printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 24))
        printf( "CDMA P_Rev in use : 0x%x\n",         pServingSystem->CDMA_P_Rev );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 26))
        printf( "3GPP Time Zone : 0x%x\n",            pServingSystem->GppTimeZone );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 27))
        printf( "3GPP NW Daylight Saving Adjustment : 0x%x\n", pServingSystem->GppNetworkDSTAdjustment );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 28))
        printf( "3GPP Location Area Code : 0x%x\n",    pServingSystem->Lac );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 29))
        printf( "Cell ID : 0x%x\n",                   pServingSystem->CellID );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 30))
        printf( "Concurrent Service Info : 0x%x\n",    pServingSystem->ConcSvcInfo );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 31))
        printf( "PRL Indicator : 0x%x\n",              pServingSystem->PRLInd );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 32))
        printf( "Dual Transfer Mode Ind : 0x%x\n",    pServingSystem->DTMInd );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 33))
    {
        printf( "\n Detailed Service Information \n" );
        printf( "Service Status: 0x%x\n",              pServingSystem->DetailedSvcInfo.srvStatus );
        printf( "Service Capability: 0x%x\n",          pServingSystem->DetailedSvcInfo.srvCapability );
        printf( "HDR service status: 0x%x\n",         pServingSystem->DetailedSvcInfo.hdrSrvStatus );
        printf( "HDR hybrid information: 0x%x\n",     pServingSystem->DetailedSvcInfo.hdrHybrid );
        printf( "Forbidden system information: 0x%x\n", pServingSystem->DetailedSvcInfo.isSysForbidden );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 34))
    {
        printf( "\n CDMA System Info Ext \n" );
        printf( "MCC: 0x%x\n",                         pServingSystem->CDMASystemInfoExt.MCC );
        printf( "IMSI_11_12: 0x%x\n",                  pServingSystem->CDMASystemInfoExt.imsi_11_12 );
        printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 35))
        printf( "HDR Personality: 0x%x\n",             pServingSystem->HdrPersonality );
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 36))
        printf( "TAC info for LTE: 0x%x\n",            pServingSystem->TrackAreaCode );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 37))
    {
        printf( "\n Call Barring Status \n" );
        printf( "CS Bar Status: 0x%x\n",               pServingSystem->CallBarStatus.csBarStatus );
        printf( "PS Bar Status: 0x%x\n",               pServingSystem->CallBarStatus.psBarStatus );
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetServingSystemParamPresenceMaskWhiteList,
        pServingSystem->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetServingSystemParamPresenceMaskMandatoryList,
        pServingSystem->ParamPresenceMask);

    /* the following items should not equal to 0 */
    if ((pServingSystem->ServingSystem.regState == 0) ||
        (pServingSystem->ServingSystem.csAttachState == 0) ||
        (pServingSystem->ServingSystem.psAttachState == 0) ||
        (pServingSystem->ServingSystem.selNetwork == 0) ||
        (pServingSystem->ServingSystem.numRadioInterfaces == 0))
        is_matching = 0;
    else if ( (pServingSystem->CurrentPLMN.MCC != MCC)  ||
                  (pServingSystem->CurrentPLMN.MNC != MNC) )
        is_matching = 0;
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetServingSystemV2 (void *ptr)
{
    unpack_nas_SLQSGetServingSystemV2_t *pServingSystem =
    (unpack_nas_SLQSGetServingSystemV2_t*) ptr;
    uint8_t index, ctr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pServingSystem->ParamPresenceMask);

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 1))
    {
        printf( "Registration state: 0x%x\n",         pServingSystem->ServingSystem.regState );
        printf( "CS Attach State: 0x%x\n",            pServingSystem->ServingSystem.csAttachState );
        printf( "PS Attach State: 0x%x\n",            pServingSystem->ServingSystem.psAttachState );
        printf( "Selected Network: 0x%x\n",           pServingSystem->ServingSystem.selNetwork );
        printf( "Number of Radio Interfaces: 0x%x\n", pServingSystem->ServingSystem.numRadioInterfaces );
        ctr = pServingSystem->ServingSystem.numRadioInterfaces;
        for( index = 0; index < ctr; index++ )
        {
            printf( "Radio Interface in use : 0x%x\n", pServingSystem->ServingSystem.radioInterface[index] );
        }
    printf( "\n" );
    }
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 16))
        printf( "Roaming Indicator value: 0x%x\n",     pServingSystem->RoamIndicatorVal );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 17))
    {
        printf( "\n Data Service capability \n" );
        printf( "Data Capability List Length: 0x%x\n", pServingSystem->DataSrvCapabilities.dataCapabilitiesLen );
        ctr = pServingSystem->DataSrvCapabilities.dataCapabilitiesLen;
        for( index = 0; index < ctr; index++ )
        {
            printf( "Data Capability : 0x%x\n", pServingSystem->DataSrvCapabilities.dataCapabilities[index] );
        }
    }
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 18))
    {
        printf( "\n Current PLMN Information \n" );
        printf( "Mobile Country Code: 0x%x\n",        pServingSystem->CurrentPLMN.MCC );
        printf( "Mobile Country Code: 0x%x\n",        pServingSystem->CurrentPLMN.MNC );
        printf( "Network Description Length: 0x%x\n", pServingSystem->CurrentPLMN.netDescrLength );
        ctr = pServingSystem->CurrentPLMN.netDescrLength;

            printf( "Network Description : ");
            for( index = 0; index < ctr; index++ )
            {
                printf( "%c",  (char) pServingSystem->CurrentPLMN.netDescr[index] );
            }
            printf( "\n");
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 19))
    {
        printf( "\n CDMA System ID \n" );
        printf( "System ID: 0x%x\n",                  pServingSystem->SystemID );
        printf( "Network ID: 0x%x\n",                 pServingSystem->NetworkID );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 20))
    {
        printf( "\n CDMA Base Station Information \n" );
        printf( "Base ID: 0x%x\n",                    pServingSystem->BasestationID );
        printf( "Base Station latitude: 0x%x\n",     pServingSystem->BasestationLatitude );
        printf( "Base Station longitude: 0x%x\n",    pServingSystem->BasestationLongitude );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 21))
    {
        printf( "\n Roaming Indicator List \n" );
        printf( "Number of Instances: 0x%x\n",        pServingSystem->RoamingIndicatorList.numInstances );
        ctr = pServingSystem->RoamingIndicatorList.numInstances;
            for( index = 0; index < ctr; index++ )
            {
                printf( "Radio Interface : 0x%x\n",   pServingSystem->RoamingIndicatorList.radioInterface[index] );
                printf( "Roaming Indicator : 0x%x\n", pServingSystem->RoamingIndicatorList.roamIndicator[index] );
            }

    printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 22))
        printf( "Default Roaming Indicator: 0x%x\n",  pServingSystem->DefaultRoamInd );
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 23))
    {
        printf( "\n 3GPP2 Time Zone \n" );
        printf( "Leap Seconds: 0x%x\n",               pServingSystem->Gpp2TimeZone.leapSeconds );
        printf( "Local Time Offset: 0x%x\n",          pServingSystem->Gpp2TimeZone.localTimeOffset );
        printf( "Daylight Saving indicator: 0x%x\n",  pServingSystem->Gpp2TimeZone.daylightSavings );
        printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 24))
        printf( "CDMA P_Rev in use : 0x%x\n",         pServingSystem->CDMA_P_Rev );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 26))
        printf( "3GPP Time Zone : 0x%x\n",            pServingSystem->GppTimeZone );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 27))
        printf( "3GPP NW Daylight Saving Adjustment : 0x%x\n", pServingSystem->GppNetworkDSTAdjustment );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 28))
        printf( "3GPP Location Area Code : 0x%x\n",    pServingSystem->Lac );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 29))
        printf( "Cell ID : 0x%x\n",                   pServingSystem->CellID );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 30))
        printf( "Concurrent Service Info : 0x%x\n",    pServingSystem->ConcSvcInfo );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 31))
        printf( "PRL Indicator : 0x%x\n",              pServingSystem->PRLInd );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 32))
        printf( "Dual Transfer Mode Ind : 0x%x\n",    pServingSystem->DTMInd );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 33))
    {
        printf( "\n Detailed Service Information \n" );
        printf( "Service Status: 0x%x\n",              pServingSystem->DetailedSvcInfo.srvStatus );
        printf( "Service Capability: 0x%x\n",          pServingSystem->DetailedSvcInfo.srvCapability );
        printf( "HDR service status: 0x%x\n",         pServingSystem->DetailedSvcInfo.hdrSrvStatus );
        printf( "HDR hybrid information: 0x%x\n",     pServingSystem->DetailedSvcInfo.hdrHybrid );
        printf( "Forbidden system information: 0x%x\n", pServingSystem->DetailedSvcInfo.isSysForbidden );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 34))
    {
        printf( "\n CDMA System Info Ext \n" );
        printf( "MCC: 0x%x\n",                         pServingSystem->CDMASystemInfoExt.MCC );
        printf( "IMSI_11_12: 0x%x\n",                  pServingSystem->CDMASystemInfoExt.imsi_11_12 );
        printf( "\n" );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 35))
        printf( "HDR Personality: 0x%x\n",             pServingSystem->HdrPersonality );
    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 36))
        printf( "TAC info for LTE: 0x%x\n",            pServingSystem->TrackAreaCode );

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 37))
    {
        printf( "\n Call Barring Status \n" );
        printf( "CS Bar Status: 0x%x\n",               pServingSystem->CallBarStatus.csBarStatus );
        printf( "PS Bar Status: 0x%x\n",               pServingSystem->CallBarStatus.psBarStatus );
    }

    if(swi_uint256_get_bit (pServingSystem->ParamPresenceMask, 39))
    {
        printf( "\n MNC PCS Digit Include Status \n" );
        printf( "MCC: %d\n",               pServingSystem->MNCPCSDigitStatTlv.MCC );
        printf( "MNC: %d\n",               pServingSystem->MNCPCSDigitStatTlv.MNC );
        printf( "MNC PCS Digit: 0x%x\n",   pServingSystem->MNCPCSDigitStatTlv.MNCIncPCSDigit );
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetServingSystemParamPresenceMaskWhiteList,
        pServingSystem->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetServingSystemParamPresenceMaskMandatoryList,
        pServingSystem->ParamPresenceMask);

    /* the following items should not equal to 0 */
    if ((pServingSystem->ServingSystem.regState == 0) ||
        (pServingSystem->ServingSystem.csAttachState == 0) ||
        (pServingSystem->ServingSystem.psAttachState == 0) ||
        (pServingSystem->ServingSystem.selNetwork == 0) ||
        (pServingSystem->ServingSystem.numRadioInterfaces == 0))
        is_matching = 0;
    else if ( (pServingSystem->CurrentPLMN.MCC != MCC)  ||
                  (pServingSystem->CurrentPLMN.MNC != MNC) )
        is_matching = 0;
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetSignalStrength (void *ptr)
{
    unpack_nas_SLQSGetSignalStrength_t *pGetSigStren =
    (unpack_nas_SLQSGetSignalStrength_t*) ptr;
    uint8_t listLen, idx;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pGetSigStren->ParamPresenceMask);

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 1))
    {
        printf( "Signal Strength Information:\n" );
       /* Display the current signal strength and radio inteface */
        printf( "Current Signal Strength : %d\n",
            pGetSigStren->rxSignalStrengthList[0].rxSignalStrength );
        printf( "Current Radio Interface : %d\n\n",
            pGetSigStren->rxSignalStrengthList[0].radioIf );
    }

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 17))
    {
        /* Display Receive Signal Strength list */
        printf( "Receive Signal Strength List:\n" );
        listLen = pGetSigStren->rxSignalStrengthListLen;
        printf( "List's Length : %d\n", listLen );
        if( 0 != listLen )
        {
            for( idx = 0; idx < listLen; idx++ )
            {
                printf( "Receive Signal Strength[%d]: %d\n",
                        idx,
                        pGetSigStren->rxSignalStrengthList[idx].rxSignalStrength );
                printf( "Radio Interface[%d]        : %d\n",
                        idx,
                        pGetSigStren->rxSignalStrengthList[idx].radioIf );
            }
        }
    }
    printf( "\n" );
    idx = 0;
    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 18))
    {
        /* Display ECIO list */
        printf( "ECIO List:\n" );
        listLen = pGetSigStren->ecioListLen;
        printf( "List's Length : %d\n", listLen );
        if( 0 != listLen )
        {
            for( idx = 0; idx < listLen; idx++ )
            {
                printf( "ECIO[%d]            : %d\n",
                        idx,
                        pGetSigStren->ecioList[idx].ecio );
                printf( "Radio Interface[%d] : %d\n",
                        idx,
                        pGetSigStren->ecioList[idx].radioIf );
            }
        }
    }
    printf( "\n" );
    idx = 0;

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 19))
        printf( "IO   : %d\n", pGetSigStren->Io );

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 20))
        printf( "SINR : %d\n\n", pGetSigStren->sinr );

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 21))
    {
        /* Display Error Rate list */
        printf( "Error Rate List:\n" );
        listLen = pGetSigStren->errorRateListLen;
        printf( "List's Length : %d\n", listLen );
        if( 0 != listLen )
        {
            for( idx = 0; idx < listLen; idx++ )
            {
                printf( "Error Rate[%d]      : %d\n",
                        idx,
                        pGetSigStren->errorRateList[idx].errorRate );
                printf( "Radio Interface[%d] : %d\n",
                        idx,
                        pGetSigStren->errorRateList[idx].radioIf );
            }
        }
    }
    printf( "\n" );

    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 22))
    {
        /* Display RSRQ Information */
        printf( "RSRQ Information\n" );
        printf( "RSRQ            : %d\n",
                pGetSigStren->rsrqInfo.rsrq );
        printf( "Radio Interface : %d\n",
                pGetSigStren->rsrqInfo.radioIf );
    }
    /* Display LTE SNR information */
    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 23))
        printf( "LTE SNR  : %d\n", pGetSigStren->ltesnr);

    /* Display LTE RSRP information*/
    if(swi_uint256_get_bit (pGetSigStren->ParamPresenceMask, 24))
        printf( "LTE RSRP : %d\n\n", pGetSigStren->ltersrp);

    if(iLocalLog==0)
        return;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSignalStrengthParamPresenceMaskWhiteList,
        pGetSigStren->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSignalStrengthParamPresenceMaskMandatoryList,
        pGetSigStren->ParamPresenceMask);

    /* the following items should not equal to 0 */
    if ((pGetSigStren->rxSignalStrengthList[0].rxSignalStrength == 0) ||
        (pGetSigStren->rxSignalStrengthList[0].radioIf == 0))
    {
        is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
    
}

void dump_GetCDMANetworkParameters (void *ptr)
{
    unpack_nas_GetCDMANetworkParameters_t *pGetCDMANetPara =
    (unpack_nas_GetCDMANetworkParameters_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pGetCDMANetPara->ParamPresenceMask);

    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 17))
        printf("SCI           : %d\n", pGetCDMANetPara->SCI);
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 18))
        printf("SCM           : %d\n", pGetCDMANetPara->SCM);
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 19))
    {
        printf("RegHomeSID    : %d\n", pGetCDMANetPara->RegHomeSID);
        printf("RegForeignSID : %d\n", pGetCDMANetPara->RegForeignSID);
        printf("RegForeignNID : %d\n", pGetCDMANetPara->RegForeignNID);
    }
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 21))
    {
        printf("CustomSCP     : %d\n", pGetCDMANetPara->CustomSCP);
        printf("Protocol      : %d\n", pGetCDMANetPara->Protocol);
        printf("Broadcast     : %d\n", pGetCDMANetPara->Broadcast);
        printf("Application   : %d\n", pGetCDMANetPara->Application);
    }
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 22))
        printf("Roaming       : %d\n", pGetCDMANetPara->Roaming);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", "N/A");
#endif
}

void dump_GetANAAAAuthenticationStatus (void *ptr)
{
    unpack_nas_GetANAAAAuthenticationStatus_t *result =
            (unpack_nas_GetANAAAAuthenticationStatus_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pAuthStatus) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
        printf("Status : %d\n", *result->pAuthStatus);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* status should be 0~2 */
    if ((result->pAuthStatus) && ( *result->pAuthStatus > 2))
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif
}

void dump_GetACCOLC (void *ptr)
{
    unpack_nas_GetACCOLC_t *result =
            (unpack_nas_GetACCOLC_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if((result->pAccolc) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)))
        printf("ACCOLC : %d\n", *result->pAccolc );

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* status should be 0~15 */
    if ((result->pAccolc) && ( *result->pAccolc > 15))
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif
}

void dump_SetACCOLC (void *ptr)
{
    unpack_nas_SetACCOLC_t *result =
            (unpack_nas_SetACCOLC_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNasConfigSigInfo2 (void *ptr)
{
    unpack_nas_SLQSNasConfigSigInfo2_t *result =
            (unpack_nas_SLQSNasConfigSigInfo2_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetPLMNName(void *ptr)
{
    unpack_nas_SLQSGetPLMNName_t *result =
            (unpack_nas_SLQSGetPLMNName_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if( (result->shortNameLen>0)||
        ((result->shortNameLen==0)&&(result->shortNameEn!=0xff)))
        {
            printf("%s> short name: %s\n", __func__, result->shortName);
        }
        if( (result->longNameLen>0)||
            ((result->longNameLen==0)&&(result->longNameEn!=0xff)))
        {
            printf("%s> long name: %s\n", __func__, result->longName);
        }
        if( (result->spnLength>0)||
            ((result->spnLength==0)&&(result->spnEncoding!=0xff)))
        {
            printf("%s> spn: %s\n", __func__, result->spn);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SetNetworkPreference(void *ptr)
{
    unpack_nas_SetNetworkPreference_t *result =
            (unpack_nas_SetNetworkPreference_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    if (  unpackRetCode != 0 )
    {
        local_fprintf("%s,",  "Correct");
        local_fprintf("%s\n", remark);
    }
    else {
        local_fprintf("%s\n",  "Correct");
    }
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SlqsGetLTECphyCAInfo(void *ptr)
{
    unpack_nas_SlqsGetLTECphyCAInfo_t *result =
            (unpack_nas_SlqsGetLTECphyCAInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((swi_uint256_get_bit (result->ParamPresenceMask, 16)) &&
            (result->LTECphyCaInfo.PhyCaAggScellIndType.TlvPresent))
    {
        printf("Scell Indicator Type:\n");
        printf("\tPCI: %d\n",result->LTECphyCaInfo.PhyCaAggScellIndType.pci);
        printf("\tFrequency: %d\n",result->LTECphyCaInfo.PhyCaAggScellIndType.freq);
        printf("\tScell State: %02X\n",result->LTECphyCaInfo.PhyCaAggScellIndType.scell_state);
    }
    
    if((swi_uint256_get_bit (result->ParamPresenceMask, 17)) &&
            (result->LTECphyCaInfo.PhyCaAggScellDlBw.TlvPresent))
        printf("Downlink Bandwidth for Scell: %02X\n",result->LTECphyCaInfo.PhyCaAggScellDlBw.dl_bw_value);

    if((swi_uint256_get_bit (result->ParamPresenceMask, 18)) &&
    (result->LTECphyCaInfo.PhyCaAggScellInfo.TlvPresent))
    {
        printf("Scell Info:\n");
        printf("\tDownlink Bandwidth: %02X\n",result->LTECphyCaInfo.PhyCaAggScellInfo.dl_bw_value);
        printf("\tFrequency: %d\n",result->LTECphyCaInfo.PhyCaAggScellInfo.freq);
        printf("\tLTE Band Value: %d\n",result->LTECphyCaInfo.PhyCaAggScellInfo.iLTEbandValue);
        printf("\tPCI: %d\n",result->LTECphyCaInfo.PhyCaAggScellInfo.pci);
        printf("\tCell State: %d\n",result->LTECphyCaInfo.PhyCaAggScellInfo.scell_state);
    }

    if((swi_uint256_get_bit (result->ParamPresenceMask, 19)) &&
            (result->LTECphyCaInfo.PhyCaAggPcellInfo.TlvPresent))
    {
        printf("Pcell Info\n");
        printf("\tDownlink Bandwidth: %02X\n",result->LTECphyCaInfo.PhyCaAggPcellInfo.dl_bw_value);
        printf("\tFrequency: %d\n",result->LTECphyCaInfo.PhyCaAggPcellInfo.freq);
        printf("\tLTE Band Value: %d\n",result->LTECphyCaInfo.PhyCaAggPcellInfo.iLTEbandValue);
        printf("\tPCI: %d\n",result->LTECphyCaInfo.PhyCaAggPcellInfo.pci);
    }

    if((swi_uint256_get_bit (result->ParamPresenceMask, 20)) &&
            (result->LTECphyCaInfo.PhyCaAggScellIndex.TlvPresent))
    {
        printf("Scell Index : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellIndex.scell_idx);
    }

    if((swi_uint256_get_bit (result->ParamPresenceMask, 21)) &&
            (result->LTECphyCaInfo.PhyCaAggScellArray.TlvPresent))
    {
        uint8_t i = 0;
        printf("Scell Array Length:%d\n",
            (int)result->LTECphyCaInfo.PhyCaAggScellArray.cphy_scell_info_list_len);
        for(i=0;i<result->LTECphyCaInfo.PhyCaAggScellArray.cphy_scell_info_list_len;i++)
        {
            printf("%d.\n",(int)i+1);
            printf("\tPCI : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.pci[i]);
            printf("\tFreq : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.freq[i]);
            printf("\tDownlink Bandwidth Values : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.cphy_ca_dl_bandwidth[i]);
            printf("\tBand : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.band[i]);
            printf("\tScell State : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.scell_state[i]);
            printf("\tScell idx : %d\n",(int)result->LTECphyCaInfo.PhyCaAggScellArray.scell_idx[i]);
        }
    }
    printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_nas_SlqsGetLTECphyCAInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SlqsGetLTECphyCAInfoParamPresenceMaskMandatoryList,
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

void dump_SLQSInitiateNetworkRegistration (void *ptr)
{
    unpack_nas_SLQSInitiateNetworkRegistration_t *result =
            (unpack_nas_SLQSInitiateNetworkRegistration_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSInitiateNetworkRegistrationParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSInitiateNetworkRegistrationParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if (  unpackRetCode != 0 )
    {
        local_fprintf("%s,", SUCCESS_MSG);
        local_fprintf("%s\n", remark);
    }
    else {
        local_fprintf("%s\n", SUCCESS_MSG);
    }
#endif

}

void dump_SLQSNasSwiIndicationRegister (void *ptr)
{
    unpack_nas_SLQSNasSwiIndicationRegister_t *result =
            (unpack_nas_SLQSNasSwiIndicationRegister_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasSwiIndicationRegisterParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasSwiIndicationRegisterParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if (  unpackRetCode != 0 )
    {
        local_fprintf("%s,", SUCCESS_MSG);
        local_fprintf("%s\n", remark);
    }
    else {
        local_fprintf("%s\n", SUCCESS_MSG);
    }
#endif
}

void dump_SLQSNasGetCellLocationInfo(void *ptr)
{
    unpack_nas_SLQSNasGetCellLocationInfo_t *result =
            (unpack_nas_SLQSNasGetCellLocationInfo_t*) ptr;
    uint16_t lC=0, lC1=0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if ((result->pGERANInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf( "\nGERAN Information \n" );
        nas_GERANInfo *temp = result->pGERANInfo;
        printf( "Cell Id   : %"PRIx32" \n",temp->cellID );
        printf( "PLMN info : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "LAC       : %x \n",temp->lac );
        printf( "ARFCN     : %x \n",temp->arfcn );
        printf( "BSIC      : %x \n",temp->bsic );
        printf( "Timing Adv: %"PRIx32" \n",temp->timingAdvance );
        printf( "RX-lev    : %x \n",temp->rxLev );
        printf( "No. of sets of following info : %x \n",temp->nmrInst );
        for (lC=0;lC<temp->nmrInst;lC++)
        {
            nas_nmrCellInfo *temp1 = &temp->insNmrCellInfo[lC];
            printf( "\tNMR Cell %d Information \n",lC+1 );
            printf( "\tNMR Cell Id   : %"PRIx32" \n",temp1->nmrCellID );
            printf( "\tNMR PLMN info : " );
            for (lC1=0;lC1<PLMN_LENGTH;lC1++)
            {
                printf( "%x ",temp1->nmrPlmn[lC1] );
            }
            printf( "\n" );
            printf( "\tNMR LAC       : %x \n",temp1->nmrLac );
            printf( "\tNMR ARFCN     : %x \n",temp1->nmrArfcn );
            printf( "\tNMR BSIC      : %x \n",temp1->nmrBsic );
            printf( "\tNMR RX-lev    : %x \n",temp1->nmrRxLev );
        }
    }
    if ( (result->pUMTSInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\nUMTS Information \n" );
        nas_UMTSInfo *temp = result->pUMTSInfo;
        printf( "Cell Id   : %x \n",temp->cellID );
        printf( "PLMN info : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "LAC       : %x \n",temp->lac );
        printf( "UARFCN    : %x \n",temp->uarfcn );
        printf( "PSC       : %x \n",temp->psc );
        printf( "RSCP      : %d \n",temp->rscp );
        printf( "ECIO      : %d \n",temp->ecio );
        printf( "No. of sets of umts instance : %x \n",temp->umtsInst );
        for (lC=0;lC<temp->umtsInst;lC++)
        {
            nas_UMTSinstInfo *temp1 = &temp->UMTSInstInfo[lC];
            printf( "\tUMTS Instance %d Information \n",lC+1 );
            printf( "\tUMTS UARFCN  : %x \n",temp1->umtsUarfcn );
            printf( "\tUMTS PSC     : %x \n",temp1->umtsPsc);
            printf( "\tUMTS RSCP    : %d \n",temp1->umtsRscp );
            printf( "\tUMTS ECIO    : %d \n",temp1->umtsEcio );
        }
        printf( "No. of sets of geran instance : %x \n",temp->geranInst );
        for (lC=0;lC<temp->geranInst;lC++)
        {
            nas_geranInstInfo *temp1 = &temp->GeranInstInfo[lC];
            printf( "\tGERAN Instance %d Information \n",lC+1 );
            printf( "\tGERAN ARFCN   : %x \n",temp1->geranArfcn );
            printf( "\tGERAN BSIC NCC: %x \n",temp1->geranBsicNcc);
            printf( "\tGERAN BSIC BCC: %x \n",temp1->geranBsicBcc );
            printf( "\tGERAN RSSI    : %d \n",temp1->geranRssi );
        }
    }
    if ( (result->pCDMAInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
         printf("\nCDMA Information \n" );
         nas_CDMAInfo *temp = result->pCDMAInfo;
         printf("System Id  : %x \n",temp->sid );
         printf("Network Id : %x \n",temp->nid );
         printf("Base Id    : %x \n",temp->baseId );
         printf("Reference PN: %x \n",temp->refpn );
         printf("Base Lat   : %"PRIx32" \n",temp->baseLat );
         printf("Base Long  : %"PRIx32" \n",temp->baseLong );
    }
    if ( (result->pLTEInfoIntrafreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        printf("\nLTE Information - INtra Frequency \n" );
        nas_LTEInfoIntrafreq *temp = result->pLTEInfoIntrafreq;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "PLMN info     : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "TAC                   : %x \n",temp->tac );
        printf( "Global Cell Id        : %"PRIx32" \n",temp->globalCellId );
        printf( "EARFCN                : %x \n",temp->earfcn );
        printf( "Serving Cell Id       : %x \n",temp->servingCellId );
        printf( "Cell Resel Priority   : %x \n",temp->cellReselPriority );
        printf( "Non-Intra Search      : %x \n",temp->sNonIntraSearch );
        printf( "Serving Cell Threshold: %x \n",temp->threshServingLow );
        printf( "Intra Search          : %x \n",temp->sIntraSearch );
        printf( "No. of sets of following info : %x \n",temp->cellsLen );
        for (lC=0;lC<temp->cellsLen;lC++)
        {
            nas_cellParams *temp1 = &temp->CellParams[lC];
            printf( "\tCell Number %d Information \n",lC+1 );
            printf( "\tPhysical Cell Id: %x \n",temp1->pci );
            printf( "\tRSRQ            : %d \n",temp1->rsrq );
            printf( "\tRSRP            : %d \n",temp1->rsrp );
            printf( "\tRSSI            : %d \n",temp1->rssi );
            printf( "\tSRX-lev         : %d \n",temp1->srxlev );
        }
    }
    if ( (result->pLTEInfoInterfreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        printf("\nLTE Information - Inter Frequency \n" );
        nas_LTEInfoInterfreq *temp = result->pLTEInfoInterfreq;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_infoInterFreq *t = &(temp->InfoInterfreq[lC]);
            printf( "Frequency Number %d Information \n",lC+1 );
            printf( "EARFCN            : %d \n",t->earfcn );
            printf( "Thresh X Low      : %d \n",t->threshXLow );
            printf( "Thresh X High     : %d \n",t->threshXHigh );
            printf( "Cell Resel Pty    : %d \n",t->cell_resel_priority );
            printf( "Cells Length      : %d \n",t->cells_len );
            for (lC1=0;lC1<t->cells_len;lC1++)
            {
                nas_cellParams *temp1 = &t->cellInterFreqParams[lC1];
                printf( "Cell Number %d Information \n",lC1+1 );
                printf( "\tPhysical Cell Id: %x \n",temp1->pci );
                printf( "\tRSRQ            : %d \n",temp1->rsrq );
                printf( "\tRSRP            : %d \n",temp1->rsrp );
                printf( "\tRSSI            : %d \n",temp1->rssi );
                printf( "\tSRX-lev         : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pLTEInfoNeighboringGSM ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
    {
        printf("\nLTE Information - Neighboring GSM \n" );
        nas_LTEInfoNeighboringGSM *temp = result->pLTEInfoNeighboringGSM;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_lteGsmCellInfo *t = &(temp->LteGsmCellInfo[lC]);
            printf( "\tFrequency Number %d Information \n",lC+1 );
            printf( "\tCell Resel Pty    : %d \n",t->cellReselPriority );
            printf( "\tThresh GSM High   : %d \n",t->threshGsmHigh );
            printf( "\tThresh GSM Low    : %d \n",t->threshGsmLow );
            printf( "\tNCC Permitted     : %x \n",t->nccPermitted );
            printf( "\tCells Length      : %d \n",t->cells_len );
            for (lC1=0;lC1<t->cells_len;lC1++)
            {
                nas_gsmCellInfo *temp1 = &t->GsmCellInfo[lC1];
                printf( "\tGSM Cell Number %d Information \n",lC1+1 );
                printf( "\t\tARFCN         : %d \n",temp1->arfcn );
                printf( "\t\tBand_1900     : %x \n",temp1->band1900 );
                printf( "\t\tCell ID Valid : %x \n",temp1->cellIdValid );
                printf( "\t\tBSIC ID       : %x \n",temp1->bsicId );
                printf( "\t\tRSSI          : %d \n",temp1->rssi );
                printf( "\t\tSRX-lev       : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pLTEInfoNeighboringWCDMA ) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)) )
    {
        printf("\nLTE Information - Neighboring WCDMA \n" );
        nas_LTEInfoNeighboringWCDMA *temp = result->pLTEInfoNeighboringWCDMA;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_lteWcdmaCellInfo *t = &(temp->LTEWCDMACellInfo[lC]);
            printf( "\tFrequency Number %d Information \n",lC+1 );
            printf( "\tUARFCN          : %d \n",t->uarfcn );
            printf( "\tCell Resel Pty  : %d \n",t->cellReselPriority );
            printf( "\tThresh X High   : %d \n",t->threshXhigh );
            printf( "\tThresh X Low    : %d \n",t->threshXlow );
            printf( "\tCells Length    : %d \n",t->cellsLen );
            for (lC1=0;lC1<t->cellsLen;lC1++)
            {
                nas_wcdmaCellInfo *temp1 = &t->WCDMACellInfo[lC1];
                printf( "\tWCDMA Cell Number %d Information \n",lC1+1 );
                printf( "\t\tPSC        : %d \n",temp1->psc );
                printf( "\t\tCPICH_RSCP : %d \n",temp1->cpich_rscp );
                printf( "\t\tCPICH_ECNO : %d \n",temp1->cpich_ecno );
                printf( "\t\tSRX-lev    : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pUMTSCellID ) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)) )
    {
         printf("\nUMTS Cell ID \n" );
         printf( "UMTS Cell ID  : 0x%"PRIx32" \n", *result->pUMTSCellID );
    }
    if ( (result->pWCDMAInfoLTENeighborCell ) && (swi_uint256_get_bit (result->ParamPresenceMask, 24)) )
    {
        printf("\nWCDMA Information - LTE Neighboring cell info \n" );
        nas_WCDMAInfoLTENeighborCell *temp = result->pWCDMAInfoLTENeighborCell;
        printf( "WCDMA RRC State            : %"PRIx32" \n",temp->wcdmaRRCState );
        printf( "UMTS LTE neighbor cell Len : %x \n",temp->umtsLTENbrCellLen );
        for (lC=0;lC<temp->umtsLTENbrCellLen;lC++)
        {
            nas_umtsLTENbrCell *t = &(temp->UMTSLTENbrCell[lC]);
            printf( "\tNumber %d Information \n",lC+1 );
            printf( "\tEARFCN           : %d \n",t->earfcn );
            printf( "\tPhysical Cell ID : %d \n",t->pci );
            printf( "\tRSRP             : %f \n",(float)t->rsrp );
            printf( "\tRSRQ             : %f \n",(float)t->rsrq );
            printf( "\tSRX Lev          : %d \n",t->srxlev );
            printf( "\tCell is TDD      : %d \n",t->cellIsTDD );
        }
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( (result->pLTEInfoInterfreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        if ( (result->pLTEInfoInterfreq->InfoInterfreq[0].cellInterFreqParams[0].rssi < -1200) ||
           (result->pLTEInfoInterfreq->InfoInterfreq[0].cellInterFreqParams[0].rssi > 0))
                is_matching = 0;
    }
    else if ( (result->pLTEInfoNeighboringGSM ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
    {
        if ( (result->pLTEInfoNeighboringGSM->LteGsmCellInfo[0].GsmCellInfo[0].rssi < -2000) ||
           (result->pLTEInfoNeighboringGSM->LteGsmCellInfo[0].GsmCellInfo[0].rssi  > 0))
           is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_SLQSNasGetCellLocationInfoV2(void *ptr)
{
    unpack_nas_SLQSNasGetCellLocationInfoV2_t *result =
            (unpack_nas_SLQSNasGetCellLocationInfoV2_t*) ptr;
    uint16_t lC=0, lC1=0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if ((result->pGERANInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf( "\nGERAN Information \n" );
        nas_GERANInfo *temp = result->pGERANInfo;
        printf( "Cell Id   : %"PRIx32" \n",temp->cellID );
        printf( "PLMN info : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "LAC       : %x \n",temp->lac );
        printf( "ARFCN     : %x \n",temp->arfcn );
        printf( "BSIC      : %x \n",temp->bsic );
        printf( "Timing Adv: %"PRIx32" \n",temp->timingAdvance );
        printf( "RX-lev    : %x \n",temp->rxLev );
        printf( "No. of sets of following info : %x \n",temp->nmrInst );
        for (lC=0;lC<temp->nmrInst;lC++)
        {
            nas_nmrCellInfo *temp1 = &temp->insNmrCellInfo[lC];
            printf( "\tNMR Cell %d Information \n",lC+1 );
            printf( "\tNMR Cell Id   : %"PRIx32" \n",temp1->nmrCellID );
            printf( "\tNMR PLMN info : " );
            for (lC1=0;lC1<PLMN_LENGTH;lC1++)
            {
                printf( "%x ",temp1->nmrPlmn[lC1] );
            }
            printf( "\n" );
            printf( "\tNMR LAC       : %x \n",temp1->nmrLac );
            printf( "\tNMR ARFCN     : %x \n",temp1->nmrArfcn );
            printf( "\tNMR BSIC      : %x \n",temp1->nmrBsic );
            printf( "\tNMR RX-lev    : %x \n",temp1->nmrRxLev );
        }
    }
    if ( (result->pUMTSInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\nUMTS Information \n" );
        nas_UMTSInfo *temp = result->pUMTSInfo;
        printf( "Cell Id   : %x \n",temp->cellID );
        printf( "PLMN info : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "LAC       : %x \n",temp->lac );
        printf( "UARFCN    : %x \n",temp->uarfcn );
        printf( "PSC       : %x \n",temp->psc );
        printf( "RSCP      : %d \n",temp->rscp );
        printf( "ECIO      : %d \n",temp->ecio );
        printf( "No. of sets of umts instance : %x \n",temp->umtsInst );
        for (lC=0;lC<temp->umtsInst;lC++)
        {
            nas_UMTSinstInfo *temp1 = &temp->UMTSInstInfo[lC];
            printf( "\tUMTS Instance %d Information \n",lC+1 );
            printf( "\tUMTS UARFCN  : %x \n",temp1->umtsUarfcn );
            printf( "\tUMTS PSC     : %x \n",temp1->umtsPsc);
            printf( "\tUMTS RSCP    : %d \n",temp1->umtsRscp );
            printf( "\tUMTS ECIO    : %d \n",temp1->umtsEcio );
        }
        printf( "No. of sets of geran instance : %x \n",temp->geranInst );
        for (lC=0;lC<temp->geranInst;lC++)
        {
            nas_geranInstInfo *temp1 = &temp->GeranInstInfo[lC];
            printf( "\tGERAN Instance %d Information \n",lC+1 );
            printf( "\tGERAN ARFCN   : %x \n",temp1->geranArfcn );
            printf( "\tGERAN BSIC NCC: %x \n",temp1->geranBsicNcc);
            printf( "\tGERAN BSIC BCC: %x \n",temp1->geranBsicBcc );
            printf( "\tGERAN RSSI    : %d \n",temp1->geranRssi );
        }
    }
    if ( (result->pCDMAInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
         printf("\nCDMA Information \n" );
         nas_CDMAInfo *temp = result->pCDMAInfo;
         printf("System Id  : %x \n",temp->sid );
         printf("Network Id : %x \n",temp->nid );
         printf("Base Id    : %x \n",temp->baseId );
         printf("Reference PN: %x \n",temp->refpn );
         printf("Base Lat   : %"PRIx32" \n",temp->baseLat );
         printf("Base Long  : %"PRIx32" \n",temp->baseLong );
    }
    if ( (result->pLTEInfoIntrafreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        printf("\nLTE Information - INtra Frequency \n" );
        nas_LTEInfoIntrafreq *temp = result->pLTEInfoIntrafreq;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "PLMN info     : " );
        for (lC=0;lC<PLMN_LENGTH;lC++)
        {
            printf( "%x ",temp->plmn[lC] );
        }
        printf( "\n" );
        printf( "TAC                   : %x \n",temp->tac );
        printf( "Global Cell Id        : %"PRIx32" \n",temp->globalCellId );
        printf( "EARFCN                : %x \n",temp->earfcn );
        printf( "Serving Cell Id       : %x \n",temp->servingCellId );
        printf( "Cell Resel Priority   : %x \n",temp->cellReselPriority );
        printf( "Non-Intra Search      : %x \n",temp->sNonIntraSearch );
        printf( "Serving Cell Threshold: %x \n",temp->threshServingLow );
        printf( "Intra Search          : %x \n",temp->sIntraSearch );
        printf( "No. of sets of following info : %x \n",temp->cellsLen );
        for (lC=0;lC<temp->cellsLen;lC++)
        {
            nas_cellParams *temp1 = &temp->CellParams[lC];
            printf( "\tCell Number %d Information \n",lC+1 );
            printf( "\tPhysical Cell Id: %x \n",temp1->pci );
            printf( "\tRSRQ            : %d \n",temp1->rsrq );
            printf( "\tRSRP            : %d \n",temp1->rsrp );
            printf( "\tRSSI            : %d \n",temp1->rssi );
            printf( "\tSRX-lev         : %d \n",temp1->srxlev );
        }
    }
    if ( (result->pLTEInfoInterfreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        printf("\nLTE Information - Inter Frequency \n" );
        nas_LTEInfoInterfreq *temp = result->pLTEInfoInterfreq;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_infoInterFreq *t = &(temp->InfoInterfreq[lC]);
            printf( "Frequency Number %d Information \n",lC+1 );
            printf( "EARFCN            : %d \n",t->earfcn );
            printf( "Thresh X Low      : %d \n",t->threshXLow );
            printf( "Thresh X High     : %d \n",t->threshXHigh );
            printf( "Cell Resel Pty    : %d \n",t->cell_resel_priority );
            printf( "Cells Length      : %d \n",t->cells_len );
            for (lC1=0;lC1<t->cells_len;lC1++)
            {
                nas_cellParams *temp1 = &t->cellInterFreqParams[lC1];
                printf( "Cell Number %d Information \n",lC1+1 );
                printf( "\tPhysical Cell Id: %x \n",temp1->pci );
                printf( "\tRSRQ            : %d \n",temp1->rsrq );
                printf( "\tRSRP            : %d \n",temp1->rsrp );
                printf( "\tRSSI            : %d \n",temp1->rssi );
                printf( "\tSRX-lev         : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pLTEInfoNeighboringGSM ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
    {
        printf("\nLTE Information - Neighboring GSM \n" );
        nas_LTEInfoNeighboringGSM *temp = result->pLTEInfoNeighboringGSM;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_lteGsmCellInfo *t = &(temp->LteGsmCellInfo[lC]);
            printf( "\tFrequency Number %d Information \n",lC+1 );
            printf( "\tCell Resel Pty    : %d \n",t->cellReselPriority );
            printf( "\tThresh GSM High   : %d \n",t->threshGsmHigh );
            printf( "\tThresh GSM Low    : %d \n",t->threshGsmLow );
            printf( "\tNCC Permitted     : %x \n",t->nccPermitted );
            printf( "\tCells Length      : %d \n",t->cells_len );
            for (lC1=0;lC1<t->cells_len;lC1++)
            {
                nas_gsmCellInfo *temp1 = &t->GsmCellInfo[lC1];
                printf( "\tGSM Cell Number %d Information \n",lC1+1 );
                printf( "\t\tARFCN         : %d \n",temp1->arfcn );
                printf( "\t\tBand_1900     : %x \n",temp1->band1900 );
                printf( "\t\tCell ID Valid : %x \n",temp1->cellIdValid );
                printf( "\t\tBSIC ID       : %x \n",temp1->bsicId );
                printf( "\t\tRSSI          : %d \n",temp1->rssi );
                printf( "\t\tSRX-lev       : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pLTEInfoNeighboringWCDMA ) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)) )
    {
        printf("\nLTE Information - Neighboring WCDMA \n" );
        nas_LTEInfoNeighboringWCDMA *temp = result->pLTEInfoNeighboringWCDMA;
        printf( "UE in Idle    : %x \n",temp->ueInIdle );
        printf( "Frequency Len : %d \n",temp->freqsLen );
        for (lC=0;lC<temp->freqsLen;lC++)
        {
            nas_lteWcdmaCellInfo *t = &(temp->LTEWCDMACellInfo[lC]);
            printf( "\tFrequency Number %d Information \n",lC+1 );
            printf( "\tUARFCN          : %d \n",t->uarfcn );
            printf( "\tCell Resel Pty  : %d \n",t->cellReselPriority );
            printf( "\tThresh X High   : %d \n",t->threshXhigh );
            printf( "\tThresh X Low    : %d \n",t->threshXlow );
            printf( "\tCells Length    : %d \n",t->cellsLen );
            for (lC1=0;lC1<t->cellsLen;lC1++)
            {
                nas_wcdmaCellInfo *temp1 = &t->WCDMACellInfo[lC1];
                printf( "\tWCDMA Cell Number %d Information \n",lC1+1 );
                printf( "\t\tPSC        : %d \n",temp1->psc );
                printf( "\t\tCPICH_RSCP : %d \n",temp1->cpich_rscp );
                printf( "\t\tCPICH_ECNO : %d \n",temp1->cpich_ecno );
                printf( "\t\tSRX-lev    : %d \n",temp1->srxlev );
            }
        }
    }
    if ( (result->pUMTSCellID ) && (swi_uint256_get_bit (result->ParamPresenceMask, 23)) )
    {
         printf("\nUMTS Cell ID \n" );
         printf( "UMTS Cell ID  : 0x%"PRIx32" \n", *result->pUMTSCellID );
    }
    if ( (result->pWCDMAInfoLTENeighborCell ) && (swi_uint256_get_bit (result->ParamPresenceMask, 24)) )
    {
        printf("\nWCDMA Information - LTE Neighboring cell info \n" );
        nas_WCDMAInfoLTENeighborCell *temp = result->pWCDMAInfoLTENeighborCell;
        printf( "WCDMA RRC State            : %"PRIx32" \n",temp->wcdmaRRCState );
        printf( "UMTS LTE neighbor cell Len : %x \n",temp->umtsLTENbrCellLen );
        for (lC=0;lC<temp->umtsLTENbrCellLen;lC++)
        {
            nas_umtsLTENbrCell *t = &(temp->UMTSLTENbrCell[lC]);
            printf( "\tNumber %d Information \n",lC+1 );
            printf( "\tEARFCN           : %d \n",t->earfcn );
            printf( "\tPhysical Cell ID : %d \n",t->pci );
            printf( "\tRSRP             : %f \n",(float)t->rsrp );
            printf( "\tRSRQ             : %f \n",(float)t->rsrq );
            printf( "\tSRX Lev          : %d \n",t->srxlev );
            printf( "\tCell is TDD      : %d \n",t->cellIsTDD );
        }
    }
    if ( (result->pWCDMACellInfoExt)  && (swi_uint256_get_bit (result->ParamPresenceMask, 28)) )
    {
         printf( "\nWCDMA Cell Info Ext \n" );
         printf( "Power in dB  : %f \n", result->pWCDMACellInfoExt->wAgc );
         printf( "Tx Power in dB  : %f \n", result->pWCDMACellInfoExt->wTxAgc );
         printf( "Downlink block error rate percentage  : 0x%4x \n", result->pWCDMACellInfoExt->wDlBler );
    }
    if ( (result->pUMTSExtInfo)  &&  (swi_uint256_get_bit (result->ParamPresenceMask, 34)) )
    {
         printf( "\nUMTS Extended Info \n" );
         printf( "Cell ID  : 0x%04x \n", result->pUMTSExtInfo->cellId );
         printf( "PLMN  : ");
         for (lC=0; lC<3; lC++)
             printf( " 0x%02x ", result->pUMTSExtInfo->plmn[lC] );
         printf( "\n");
         printf( "Location area code  : 0x%04x \n", result->pUMTSExtInfo->lac );
         printf( "UTRA absolute RF channel number  : %d \n", result->pUMTSExtInfo->uarfcn );
         printf( "Primary scrambling code  : %d \n", result->pUMTSExtInfo->psc );
         printf( "Received signal code power  : %d \n", result->pUMTSExtInfo->rscp );
         printf( "ECIO  : %d \n", result->pUMTSExtInfo->ecio );
         printf( "Squal  : %d \n", result->pUMTSExtInfo->squal );
         printf( "cell selection Rx level  : %d \n", result->pUMTSExtInfo->srxLvl );

         printf( "UMTS Instances Len : 0x%02x \n",result->pUMTSExtInfo->umtsInst );
         for (lC=0; lC<result->pUMTSExtInfo->umtsInst; lC++)
         {
             nas_umtsInstArr *t = &(result->pUMTSExtInfo->umtsInstElement[lC]);
             printf( "\tNumber %d Information \n",lC+1 );
             printf( "\tUARFCN           : %d \n",t->umtsUarfcn );
             printf( "\tPrimary scrambling code : %d \n",t->umtsPsc );
             printf( "\tReceived signal code power  : %d \n",t->umtsRscp );
             printf( "\tECIO             : %d \n",t->umtsEcio );
             printf( "\tSqual            : %d \n",t->umtsSqual );
             printf( "\tCell selection Rx level  : %d \n",t->umtsSrxLvl );
             printf( "\tRank of the cell     : %d \n",t->umtsRank );
             printf( "\tSet of the cell      : %d \n",t->umtsSet );
         }
         printf( "GERAN Instances Len : 0x%02x \n",result->pUMTSExtInfo->geranInst );
         for (lC=0; lC<result->pUMTSExtInfo->geranInst; lC++)
         {
             nas_geranInstArr *t = &(result->pUMTSExtInfo->geranInstElement[lC]);
             printf( "\tNumber %d Information \n",lC+1 );
             printf( "\tAbsolute RF channel number : %d \n",t->geranArfcn );
             printf( "\tBase station identity code network color code : %d \n",t->geranBsicNcc );
             printf( "\tBase station identity code base station color code : %d \n",t->geranBsicBcc );
             printf( "\tReceived signal strength indicator : %d \n",t->geranRssi );
             printf( "\tRank of the cell : %d \n",t->geranRank );
         }
    }
    if ( (result->pLteEarfcnInfo)  && (swi_uint256_get_bit (result->ParamPresenceMask, 41)) )
    {
         printf( "\nLTE Neighbor Cell Info EARFCN \n" );
         printf( "LTE EARFCN Length : 0x%02x \n",result->pLteEarfcnInfo->lteInterEarfcnlen );
         for (lC=0; lC<result->pLteEarfcnInfo->lteInterEarfcnlen; lC++)
         {
             printf( "\tLTE EARFCN [%d]           : %d \n",lC,lC<result->pLteEarfcnInfo->lteEarfcn[lC] );
         }
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSNasGetCellLocationInfoParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if ( (result->pLTEInfoInterfreq ) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        if ( (result->pLTEInfoInterfreq->InfoInterfreq[0].cellInterFreqParams[0].rssi < -1200) ||
           (result->pLTEInfoInterfreq->InfoInterfreq[0].cellInterFreqParams[0].rssi > 0))
                is_matching = 0;
    }
    else if ( (result->pLTEInfoNeighboringGSM ) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
    {
        if ( (result->pLTEInfoNeighboringGSM->LteGsmCellInfo[0].GsmCellInfo[0].rssi < -2000) ||
           (result->pLTEInfoNeighboringGSM->LteGsmCellInfo[0].GsmCellInfo[0].rssi  > 0))
           is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_SLQSGetNetworkTime(void *ptr)
{
    unpack_nas_SLQSGetNetworkTime_t *result =
            (unpack_nas_SLQSGetNetworkTime_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
        printf("%s Data\n",__FUNCTION__);
        swi_uint256_print_mask (result->ParamPresenceMask);
        if((result->p3GPP2TimeInfo ) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        {
            printf( "\n3GPP2 Network Time Information \n" );
            nas_timeInfo *temp = result->p3GPP2TimeInfo;
            printf( "Year: %u\n",temp->year);
            printf("Month: %u\n",temp->month);
            printf( "Day: %u\n",temp->day);
            printf( "Hour: %u\n",temp->hour);
            printf( "Minute: %u\n",temp->minute);
            printf( "Second: %u\n",temp->second);
            printf( "Day of Week: %u\n",temp->dayOfWeek);
            printf( "Time Zone: %d\n",temp->timeZone);
            printf( "Day Light Saving Adj: %u\n",temp->dayLtSavingAdj);
            printf( "Radio Interface: %d\n",temp->radioInterface);
        }
        if((result->p3GPPTimeInfo) &&  (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        {
            printf( "\n3GPP Network Time Information \n" );
            nas_timeInfo *temp = result->p3GPPTimeInfo;
            printf( "Year: %u\n",temp->year);
            printf("Month: %u\n",temp->month);
            printf( "Day: %u\n",temp->day);
            printf( "Hour: %u\n",temp->hour);
            printf( "Minute: %u\n",temp->minute);
            printf( "Second: %u\n",temp->second);
            printf( "Day of Week: %u\n",temp->dayOfWeek);
            printf( "Time Zone: %d\n",temp->timeZone);
            printf( "Day Light Saving Adj: %u\n",temp->dayLtSavingAdj);
            printf( "Radio Interface: %d\n",temp->radioInterface);
        }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetLteSccRxInfo(void *ptr)
{
    unpack_nas_SLQSSwiGetLteSccRxInfo_t *result =
            (unpack_nas_SLQSSwiGetLteSccRxInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
        printf("%s Data\n",__FUNCTION__);
        swi_uint256_print_mask (result->ParamPresenceMask);
        uint8_t lcount;
                 if ((result->pSccRxInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
                 {
                     printf( "\nLTE Secondary carrier Rx signal level information:\n");
                     printf( "RSRQ: %.1f\n",(float)result->pSccRxInfo->rsrq/10);
                     printf( "SNR: %d\n",result->pSccRxInfo->snr);
                     for(lcount=0; lcount<result->pSccRxInfo->numInstances; lcount++)
                     {
                         printf( "Rx Chain Index: %d\n",result->pSccRxInfo->sigInfo[lcount].rxChainIndex);
                         printf( "Is Radio Tuned: %d\n",result->pSccRxInfo->sigInfo[lcount].isRadioTuned);
                         printf( "Rx Power: %.1f\n",(float)result->pSccRxInfo->sigInfo[lcount].rxPower/10);
                         printf( "RSRP : %.1f\n",(float)result->pSccRxInfo->sigInfo[lcount].rsrp/10);
                     }
                 }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_InitiateDomainAttach (void *ptr)
{
    unpack_nas_InitiateDomainAttach_t *result =
            (unpack_nas_InitiateDomainAttach_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Data\n",__FUNCTION__);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_SetCDMANetworkParameters (void *ptr)
{
    unpack_nas_SetCDMANetworkParameters_t *result =
            (unpack_nas_SetCDMANetworkParameters_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Data\n",__FUNCTION__);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSNasGetHDRColorCode(void *ptr)
{
    unpack_nas_SLQSNasGetHDRColorCode_t *result =
            (unpack_nas_SLQSNasGetHDRColorCode_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
       IFPRINTF("Color Code : %d\n", result->pColorCode );

    printf("%s Data\n",__FUNCTION__);
    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNasGetTxRxInfo(void *ptr)
{
    unpack_nas_SLQSNasGetTxRxInfo_t *result =
            (unpack_nas_SLQSNasGetTxRxInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("Rx Chain 0 Info Response \n");
        printf("Is radio Tuned  : 0x%X\n",result->pRXChain0Info->isRadioTuned);
        printf("Rx Power Value  : %d\n",result->pRXChain0Info->rxPower);
        printf("ECIO            : %d\n",result->pRXChain0Info->ecio);
        printf("RSCP            : %d\n",result->pRXChain0Info->rscp);
        printf("RSRP            : %d\n",result->pRXChain0Info->rsrp);
        printf("Phase           : 0x%X\n",result->pRXChain0Info->phase);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("Rx Chain 1 Info Response \n");
        printf("Is radio Tuned  : 0x%X\n",result->pRXChain1Info->isRadioTuned);
        printf("Rx Power Value  : %d\n",result->pRXChain1Info->rxPower);
        printf("ECIO            : %d\n",result->pRXChain1Info->ecio);
        printf("RSCP            : %d\n",result->pRXChain1Info->rscp);
        printf("RSRP            : %d\n",result->pRXChain1Info->rsrp);
        printf("Phase           : 0x%X\n",result->pRXChain1Info->phase);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("Tx Info Response \n");
        printf("Is in traffic   : 0x%X\n",result->pTXInfo->isInTraffic);
        printf("Tx Power Value  : %d\n",result->pTXInfo->txPower);
    }

    if(iLocalLog==0)
        return ;

    printf("%s Data\n",__FUNCTION__);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetOperatorNameData(void *ptr)
{
    unpack_nas_SLQSGetOperatorNameData_t *result =
            (unpack_nas_SLQSGetOperatorNameData_t*) ptr;
    uint16_t index=0;
    uint16_t index_1=0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if( (result->pSrvcProviderName) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {        
        printf("\nService Provider Name Information\n" );
        printf("Display Condition %x\n",result->pSrvcProviderName->displayCondition);
        printf("SPN Length :: %d \n",result->pSrvcProviderName->spnLength);
        printf("SPN :: ");
        for(index = 0; index < result->pSrvcProviderName->spnLength; index++)
        {
            printf("%c", result->pSrvcProviderName->spn[index]);
        }
        printf("\n" );
    }
    if( (result->pOperatorPLMNList) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\nOperator PLMN List Information\n" );
        printf("Num Instance :: %d\n", result->pOperatorPLMNList->numInstance );
        for(index = 0; index < result->pOperatorPLMNList->numInstance; index++)
        {
            printf("MCC :: %c %c %c\n",result->pOperatorPLMNList->PLMNData[index].mcc[0],result->pOperatorPLMNList->PLMNData[index].mcc[1],result->pOperatorPLMNList->PLMNData[index].mcc[2] );
            printf("MNC :: %c %c %c",result->pOperatorPLMNList->PLMNData[index].mnc[0],result->pOperatorPLMNList->PLMNData[index].mnc[1],result->pOperatorPLMNList->PLMNData[index].mnc[2] );
            printf("LAC1 :: %x\n",result->pOperatorPLMNList->PLMNData[index].lac1);
            printf("LAC2 :: %x\n",result->pOperatorPLMNList->PLMNData[index].lac2 );
            printf("PLMNRecID :: %x\n",result->pOperatorPLMNList->PLMNData[index].PLMNRecID );
        }
    }
    if( (result->pPLMNNetworkName) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf("\nPLMN Network Name\n" );
        printf("Num Instance :: %d\n", result->pPLMNNetworkName->numInstance );
        for(index = 0; index < result->pPLMNNetworkName->numInstance; index++)
        {
            printf("Config Scheme :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].codingScheme);
            printf("CountryInitails :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].countryInitials );
            printf("Long Name Length :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].longNameLen );
            printf("Long Name :: ");
            for(index_1 = 0; index_1 < result->pPLMNNetworkName->PLMNNetName[index].longNameLen;index_1++)
            {
                printf("%x ", result->pPLMNNetworkName->PLMNNetName[index].longName[index_1] );
            }
            printf("\n" );
            printf("Long Name Spare Bits :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].longNameSpareBits );
            printf("Short Name Length :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].shortNameLen );
            printf("Short Name :: ");
            for(index_1 = 0; index_1 < result->pPLMNNetworkName->PLMNNetName[index].shortNameLen;index_1++)
            {
                printf("%x ", result->pPLMNNetworkName->PLMNNetName[index].shortName[index_1] );
            }
            printf("\n" );
            printf("Short Name Spare Bits :: %x\n",result->pPLMNNetworkName->PLMNNetName[index].shortNameSpareBits );
        }
    }
    if( (result->pOperatorNameString) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        printf( "\nOperator Name String\n" );
        printf("PLMN Name :: ");
        for(index = 0; result->pOperatorNameString->PLMNName[index] != '\0'; index++)
        {
             printf("%c ", result->pOperatorNameString->PLMNName[index] );
        }
        printf("\n" );
    }

    if( (result->pNITZInformation) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        printf("\nNITZ Information\n" );
        printf("Config Scheme :: %x\n",result->pNITZInformation->codingScheme );
        printf("CountryInitails :: %x\n",result->pNITZInformation->countryInitials );
        printf("Long Name Spare Bits :: %x\n",result->pNITZInformation->longNameSpareBits );
        printf("Short Name Spare Bits :: %x\n",result->pNITZInformation->shortNameSpareBits );
        printf("Long Name Length :: %d\n",result->pNITZInformation->longNameLen );
        printf("Long Name :: ");
        for(index = 0; index < result->pNITZInformation->longNameLen; index++)
        {
            printf("%X ", result->pNITZInformation->longName[index]);
        }
        printf("\n" );
        printf("Short Name Length :: %d\n",result->pNITZInformation->shortNameLen);
        printf("Short Name :: ");
        for(index = 0; index < result->pNITZInformation->shortNameLen; index++)
        {
            printf("%c", result->pNITZInformation->shortName[index]);
        }
        printf("\n" );
    }

    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNasGet3GPP2Subscription(void *ptr)
{
    unpack_nas_SLQSNasGet3GPP2Subscription_t *result =
            (unpack_nas_SLQSNasGet3GPP2Subscription_t*) ptr;
    uint16_t lC;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pNAMNameInfo) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("SLQSNasGet3GPP2Subscription Successful\n");
        printf("Nam Name Response \n");
        printf("NAM name length  : %x\n",result->pNAMNameInfo->namNameLen);
        printf("Nam Name Info:");
                    for ( lC = 0 ; lC < result->pNAMNameInfo->namNameLen ; lC++ )
                    {
                        printf(" %x", result->pNAMNameInfo->namName[lC]);
                    }
        printf("\n");
    }
    if((result->pDirNum) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("Directory Number Response \n");
        printf("Directory number length  : %x\n",result->pDirNum->dirNumLen);
        printf("Directory Number Info:");
        for ( lC = 0 ; lC < result->pDirNum->dirNumLen ; lC++ )
        {
            printf(" %x", result->pDirNum->dirNum[lC]);
        }
        printf("\n");
    }
    if((result->pHomeSIDNID) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf("SID/NID Response \n");
        printf("Number of sets : %x\n",result->pHomeSIDNID->numInstances);
        for ( lC = 0 ; lC < result->pHomeSIDNID->numInstances ; lC++ )
        {
            nas_sidNid *t = &result->pHomeSIDNID->SidNid[lC];
            printf("SID NID Info %d\n",  lC);
            printf("System ID  :%x ",   t->sid);
            printf("Network ID :%x\n", t->nid);
        }
    }
    if((result->pMinBasedIMSI) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        printf("Min Based IMSI Response \n");
        printf("MCCM :");
        for ( lC = 0; lC < PLMN_LENGTH ; lC++)
        {
            printf(" %x ", result->pMinBasedIMSI->mccM[lC]);
        }
        printf("\n");

        printf("IMSI_M_11_12 : %x\n", result->pMinBasedIMSI->imsiM1112);

        printf("IMSI_M 1 :");
        for ( lC = 0; lC < NAS_IMSI_M_S1_LENGTH ; lC++)
        {
            printf(" %x ", result->pMinBasedIMSI->imsiMS1[lC]);
        }
        printf("\n");

        printf("IMSI_M 2 :");
        for ( lC = 0; lC < NAS_IMSI_M_S2_LENGTH ; lC++)
        {
            printf(" %x", result->pMinBasedIMSI->imsiMS2[lC]);
        }
        printf("\n");
    }
    if((result->pTrueIMSI) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        printf("True IMSI Response \n");
        printf("MCCT :");
        for ( lC = 0; lC < PLMN_LENGTH ; lC++)
        {
            printf(" %x ", result->pTrueIMSI->mccT[lC]);
        }
        printf("\n");

        printf("IMSI_T_11_12 : %x\n", result->pTrueIMSI->imsiT1112);

        printf("IMSI_T 1 :");
        for ( lC = 0; lC < NAS_IMSI_M_S1_LENGTH ; lC++)
        {
            printf(" %x ", result->pTrueIMSI->imsiTS1[lC]);
        }
        printf("\n");

        printf("IMSI_T 2 :");
        for ( lC = 0; lC < NAS_IMSI_M_S2_LENGTH ; lC++)
        {
            printf(" %x ", result->pTrueIMSI->imsiTS2[lC]);
        }
        printf("\n");
        printf("IMSI_T_ADDR_NUM : %x\n", result->pTrueIMSI->imsiTaddrNum);
    }
    if((result->pCDMAChannel) && (swi_uint256_get_bit (result->ParamPresenceMask, 21)) )
    {
        printf("CDMA Channel Information \n");
        printf("Primary Carrier A   : %x\n", result->pCDMAChannel->priChA);
        printf("Primary Carrier B   : %x\n", result->pCDMAChannel->priChB);
        printf("Secondary Carrier A : %x\n", result->pCDMAChannel->secChA);
        printf("Secondary Carrier A : %x\n", result->pCDMAChannel->secChB);
    }
    if ( (result->pMdn ) && (swi_uint256_get_bit (result->ParamPresenceMask, 22)) )
    {
        printf( "CDMA MDN Information \n");
        printf( "MDN Length   : %d\n", result->pMdn->mdnLen);
        printf( "MDN :");
        for ( lC = 0; lC < result->pMdn->mdnLen ; lC++)
        {
            printf("%c", result->pMdn->mdn[lC]);
        }
        printf("\n");
    }
    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetHDRPersonality(void *ptr)
{
    unpack_nas_SLQSSwiGetHDRPersonality_t *result =
            (unpack_nas_SLQSSwiGetHDRPersonality_t*) ptr;
    uint16_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("HDR Personality Retrieved successfully\n" );
    if((result->pCurrentPersonality) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        IFPRINTF("Current Personality  : 0x%x\n", result->pCurrentPersonality);
    }
    if((result->pPersonalityListLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        IFPRINTF( "PersonalityListLength:0x%x\n", result->pPersonalityListLength);
        for( count = 0; count < *(result->pPersonalityListLength); count++ )
        {
            printf("Accessmac      : %d\n", result->pProtocolSubtypeElement[count].AccessMac );
            printf("AuthPort       : %d\n", result->pProtocolSubtypeElement[count].AuthProt );
            printf("Controlmac     : %d\n", result->pProtocolSubtypeElement[count].ControlMac );
            printf("EncryptProt    : %d\n", result->pProtocolSubtypeElement[count].EncryptProt );
            printf("ForwardMac     : %d\n", result->pProtocolSubtypeElement[count].ForwardMac );
            printf("Idle State     : %d\n", result->pProtocolSubtypeElement[count].IdleState );
            printf("KeyExchange    : %d\n", result->pProtocolSubtypeElement[count].KeyExchange );
            printf("MultDisc       : %d\n", result->pProtocolSubtypeElement[count].MultDisc );
            printf("Physical Layer : %d\n", result->pProtocolSubtypeElement[count].PhysicalLayer );
            printf("Reversemac     : %d\n", result->pProtocolSubtypeElement[count].ReverseMac );
            printf("Secprot        : %d\n", result->pProtocolSubtypeElement[count].SecProt );
            printf("VirtStream     : %d\n", result->pProtocolSubtypeElement[count].VirtStream );
        }
    }
    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetHDRProtSubtype(void *ptr)
{
    unpack_nas_SLQSSwiGetHDRProtSubtype_t *result =
            (unpack_nas_SLQSSwiGetHDRProtSubtype_t*) ptr;
    uint16_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("HDR Protosubtype Retrieved successfully\n" );
    if((result->pCurrentPrsnlty) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        IFPRINTF("Current Personality  : 0x%x\n", result->pCurrentPrsnlty);
    }
    if((result->pPersonalityListLength) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)))
    {
        IFPRINTF("PersonalityListLength:0x%x\n", result->pPersonalityListLength);
        for( count = 0; count < *(result->pPersonalityListLength); count++ )
        {
            printf("Accessmac      : %d\n", result->pProtoSubTypElmnt[count].AccessMac );
            printf("AuthPort       : %d\n", result->pProtoSubTypElmnt[count].AuthProt );
            printf("Controlmac     : %d\n", result->pProtoSubTypElmnt[count].ControlMac );
            printf("EncryptProt    : %d\n", result->pProtoSubTypElmnt[count].EncryptProt );
            printf("ForwardMac     : %d\n", result->pProtoSubTypElmnt[count].ForwardMac );
            printf("Idle State     : %d\n", result->pProtoSubTypElmnt[count].IdleState );
            printf("KeyExchange    : %d\n", result->pProtoSubTypElmnt[count].KeyExchange );
            printf("MultDisc       : %d\n", result->pProtoSubTypElmnt[count].MultDisc );
            printf("Physical Layer : %d\n", result->pProtoSubTypElmnt[count].PhysicalLayer );
            printf("Reversemac     : %d\n", result->pProtoSubTypElmnt[count].ReverseMac );
            printf("Secprot        : %d\n", result->pProtoSubTypElmnt[count].SecProt );
            printf("VirtStream     : %d\n", result->pProtoSubTypElmnt[count].VirtStream );
        }
    }
    if((result->pAppSubType != NULL ) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        printf("App Sub Type: %"PRIu64"\n", *result->pAppSubType);

    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiPSDetach (void *ptr)
{
    unpack_nas_SLQSSwiPSDetach_t *result =
            (unpack_nas_SLQSSwiPSDetach_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetErrorRate(void *ptr)
{
    unpack_nas_SLQSGetErrorRate_t *result =
            (unpack_nas_SLQSGetErrorRate_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL

    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("Error Rate successfully retrieved\n" );
    if((result->pCDMAFrameErrRate) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        IFPRINTF("CDMA Frame Error Rate : 0x%x\n", result->pCDMAFrameErrRate );
    if((result->pHDRPackErrRate) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        IFPRINTF("HDR Packet Error Rate :0x%x\n", result->pHDRPackErrRate );
    if((result->pGSMBER) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        IFPRINTF("GSM Bit Error Rate : 0x%x\n", result->pGSMBER );
    if((result->pWCDMABER) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
        IFPRINTF("WCDMA Block Error Rate :0x%x\n", result->pWCDMABER );
        printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiGetHRPDStats(void *ptr)
{
    unpack_nas_SLQSSwiGetHRPDStats_t *result =
            (unpack_nas_SLQSSwiGetHRPDStats_t*) ptr;
    uint16_t ctr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("HRPD Stats successfully retrieved\n" );
    if((result->pDRCParams) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        /* Display DRC*/
        printf("DRC Parameters\n" );
        printf("DRC Value : 0x%x\n", result->pDRCParams->DRCCover);
        printf("DRC Cover : 0x%x\n", result->pDRCParams->DRCValue);
    }
    if((result->pUATI) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        /* Display UATI */
        printf( "UATI: " );
        for( ctr = 0; ctr < UATISIZE; ctr++ )
        {
            printf("%x ", result->pUATI[ctr] );
        }
        printf("\n" );
    }
    if((result->pPilotSetData) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        /* Display Pilot Set */
        printf("Pilot Set Data \n" );
        printf("Number of Pilot Sets: 0x%x \n",result->pPilotSetData->NumPilots);
        for( ctr = 0; ctr < result->pPilotSetData->NumPilots; ctr++ )
        {
             printf("Pilot Type: 0x%u \n",result->pPilotSetData->pPilotSetInfo[ctr].PilotType);
             printf("Pilot PN: 0x%d \n",result->pPilotSetData->pPilotSetInfo[ctr].PilotPN );
             printf("Pilot Strength: 0x%d \n",result->pPilotSetData->pPilotSetInfo[ctr].PilotStrength );
        }
    }

    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSwiNetworkDebug(void *ptr)
{
    unpack_nas_SLQSSwiNetworkDebug_t *result =
            (unpack_nas_SLQSSwiNetworkDebug_t*) ptr;
    uint16_t ctr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pObjectVer) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        /* Display Object version */
       IFPRINTF("Object Version : 0x%x\n", result->pObjectVer );
    }
    if((result->pNetworkStat1x) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("1x Network Status\n" );
        printf("State   : 0x%x\n", result->pNetworkStat1x->State);
        printf("SO      : 0x%x\n", result->pNetworkStat1x->SO);
        printf("RX Pwr  : 0x%x\n", result->pNetworkStat1x->RX_PWR);
        printf("RX ECIO : 0x%x\n", result->pNetworkStat1x->RX_EC_IO);
        printf("TX Pwr  : 0x%x\n", result->pNetworkStat1x->TX_PWR);
        printf("Active Set Pilot PN Elements\n" );
        printf("Active Set Pilot Count : 0x%x\n",result->pNetworkStat1x->ActSetCnt );
        for( ctr = 0; ctr < result->pNetworkStat1x->ActSetCnt; ctr++ )
        {
            printf("Active Set Pilot PN : 0x%x\n",result->pNetworkStat1x->pActPilotPNElements[ctr].ActSetPilotPN );
            printf("Active Set Pilot Strength  : 0x%x\n",result->pNetworkStat1x->pActPilotPNElements[ctr].ActSetPilotPNStrength );
        }
        printf("Neighbouring Set Pilot Count : 0x%x\n",result->pNetworkStat1x->NeighborSetCnt );
        for( ctr = 0; ctr < result->pNetworkStat1x->NeighborSetCnt; ctr++ )
        {
            printf("Neighbouring Set Pilot PN : 0x%x\n",result->pNetworkStat1x->pNeighborSetPilotPN[ctr] );
        }
    }
    if((result->pNetworkStatEVDO) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        /* Display EVDO network details */
        printf("EVDO network status:\n" );
        printf("State   : 0x%x\n", result->pNetworkStatEVDO->State );
        printf("SO      : 0x%x\n", result->pNetworkStatEVDO->MACIndex );
        printf("Sector ID Length : 0x%x\n",result->pNetworkStatEVDO->SectorIDLen );
        printf("Sector ID: " );
        for( ctr = 0; ctr < result->pNetworkStatEVDO->SectorIDLen; ctr++ )
        {
            printf("0x%x ", result->pNetworkStatEVDO->pSectorID[ctr] );
        }
        printf("\n" );
        printf("RX PWR       : 0x%x\n", result->pNetworkStatEVDO->RX_PWR );
        printf("PER          : 0x%x\n", result->pNetworkStatEVDO->PER );
        printf("Pilot Energy : 0x%x\n", result->pNetworkStatEVDO->PilotEnergy );
        printf("SNR          : 0x%x\n", result->pNetworkStatEVDO->SNR );
    }
    if((result->pDeviceConfigDetail) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        /*Display device configuration details */
        printf("Device Configuration details:\n" );
        printf("Technology   : 0x%x\n", result->pDeviceConfigDetail->Technology );
        printf("QLIC         : 0x%x\n", result->pDeviceConfigDetail->QLIC );
        printf("Chipset      : 0x%x\n", result->pDeviceConfigDetail->Chipset );
        printf("Hw Version   : 0x%x\n", result->pDeviceConfigDetail->HWVersion );
    }
    if((result->pDataStatusDetail) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        /* Data status details */
        printf("Data status details:\n" );
        printf("IP address      : 0x%x\n", result->pDataStatusDetail->IPAddress );
        printf("Last Error Code : 0x%x\n", result->pDataStatusDetail->LastErrCode );
    }
    printf("%s Data\n",__FUNCTION__);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSConfigSigInfo (void *ptr)
{
    unpack_nas_SLQSConfigSigInfo_t *result =
            (unpack_nas_SLQSConfigSigInfo_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetHomeNetwork3GPP2(void *ptr)
{
    unpack_nas_GetHomeNetwork3GPP2_t *result =
            (unpack_nas_GetHomeNetwork3GPP2_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("%s Data\n",__FUNCTION__);
    if((result->pMCC) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
        printf("MCC         : %d\n", *result->pMCC);
    if((result->pMNC) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
        printf("MNC         : %d\n", *result->pMNC);
    if((result->pName) && (swi_uint256_get_bit (result->ParamPresenceMask, 1)) )
        printf("Name        : %s\n", result->pName);
    if((result->pSID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("SID         : %d\n", *result->pSID);
    if((result->pNID) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        printf("NID         : %d\n", *result->pNID);
    if((result->pNw2MCC) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Nw2MCC      : %d\n", *result->pNw2MCC);
    if((result->pNw2MNC) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Nw2MNC      : %d\n", *result->pNw2MNC);
    if((result->pNw2DescDisp) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Nw2DescDisp : %d\n", *result->pNw2DescDisp);
    if((result->pNw2DescEnc) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Nw2DescEnc  : %d\n", *result->pNw2DescEnc);
    if((result->pNw2Name) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        printf("Nw2Name     : %s\n", result->pNw2Name);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNASSwiSetChannelLock (void *ptr)
{
    unpack_nas_SLQSNASSwiSetChannelLock_t *result =
            (unpack_nas_SLQSNASSwiSetChannelLock_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNASSwiGetChannelLock(void *ptr)
{
    unpack_nas_SLQSNASSwiGetChannelLock_t *result =
            (unpack_nas_SLQSNASSwiGetChannelLock_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(result)
    {
        if((result->pWcdmaUARFCN) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
        {
            printf("\nWCDMA UARFCN:\n");
            printf("Status: 0x%02X\n",result->pWcdmaUARFCN->status);
            printf("UARFCN: %u\n",result->pWcdmaUARFCN->uarfcn);
        }
        if((result->pLteEARFCN) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
        {
            printf("\nLTE EARFCN:\n");
            printf("Status: 0x%02X\n",result->pLteEARFCN->status);
            printf("EARFCN Primary: %u\n",result->pLteEARFCN->earfcn0);
            printf("EARFCN Secondary: %u\n",result->pLteEARFCN->earfcn1);
        }
        if((result->pLtePCI) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
        {
            printf("\nLTE PCI:\n");
            printf("Status: 0x%02X\n",result->pLtePCI->status);
            printf("EARFCN: %u\n",result->pLtePCI->earfcn);
            printf("PCI: %u\n",result->pLtePCI->pci);
        }
    }
    printf("%s Data\n",__FUNCTION__);
    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetSignalStrengths(void *ptr)
{
    unpack_nas_GetSignalStrengths_t *result =
            (unpack_nas_GetSignalStrengths_t*) ptr;

    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        uint8_t  count;
        for(count = 0; count < result->len; count++)
        {
            printf("SignalStrengths[%d] : %d\n",
                    count, result->rssi[count]);
            printf("RadioInterfaces[%d] : %u\n",
                    count, result->radio[count]);
        }
    }
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(iLocalLog==0)
        return;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSSetBandPreference (void *ptr)
{
    unpack_nas_SLQSSetBandPreference_t *result =
            (unpack_nas_SLQSSetBandPreference_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
}

void dump_SLQSNASSeteDRXParams (void *ptr)
{
    unpack_nas_SLQSNASSeteDRXParams_t *result =
            (unpack_nas_SLQSNASSeteDRXParams_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void doprintsysInfoCommon( nas_sysInfoCommon *pSys )
{
    RETURN_IF_PTR_IS_NULL(pSys)
    printf( "Service Domain Valid   : %x\n",pSys->srvDomainValid );
    printf( "Service Domain         : %x\n",pSys->srvDomain );
    printf( "Service Capability Valid: %x\n",pSys->srvCapabilityValid );
    printf( "Service Capability     : %x\n",pSys->srvCapability );
    printf( "Roam Status Valid      : %x\n",pSys->roamStatusValid );
    printf( "Roam Status            : %x\n",pSys->roamStatus );
    printf( "Forbidden System Valid : %x\n",pSys->isSysForbiddenValid );
    printf( "Is Forbidden System    : %x\n",pSys->isSysForbiddenValid );
}

void dump_SLQSGetSysInfo (void *ptr)
{
    unpack_nas_SLQSGetSysInfo_t *resp =
            (unpack_nas_SLQSGetSysInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    uint16_t              lC;
    if ( ( resp->pCDMASrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 16)) )
    {
        printf( "Service Status(CDMA) : %x\n",resp->pCDMASrvStatusInfo->srvStatus );
        printf( "Is Preferred(CDMA)   : %x\n",resp->pCDMASrvStatusInfo->isPrefDataPath );
    }
    if ( ( resp->pHDRSrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 17)) )
    {
        printf( "Service Status(HDR) : %x\n",resp->pHDRSrvStatusInfo->srvStatus );
        printf( "Is Preferred(HDR)   : %x\n",resp->pHDRSrvStatusInfo->isPrefDataPath );
    }
    if (( resp->pGSMSrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 18)) )
    {
        printf( "Service Status(GSM)      : %x\n",resp->pGSMSrvStatusInfo->srvStatus );
        printf( "True Service Status(GSM) : %x\n",resp->pGSMSrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(GSM)        : %x\n",resp->pGSMSrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pWCDMASrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 19)) )
    {
        printf( "Service Status(WCDMA)     : %x\n",resp->pWCDMASrvStatusInfo->srvStatus );
        printf( "True Service Status(WCDMA): %x\n",resp->pWCDMASrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(WCDMA)       : %x\n",resp->pWCDMASrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pLTESrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 20)) )
    {
        printf( "Service Status(LTE)      : %x\n",resp->pLTESrvStatusInfo->srvStatus );
        printf( "True Service Status(LTE) : %x\n",resp->pLTESrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(LTE)        : %x\n",resp->pLTESrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)) )
    {
        printf( "\nCDMA SYSTEM INFORMATION ----\n");
        nas_CDMASysInfo *t = resp->pCDMASysInfo;
        doprintsysInfoCommon( &t->sysInfoCDMA );
        printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
        printf( "System PRL          : %x\n",t->isSysPrlMatch );
        printf( "P_Rev Valid         : %x\n",t->pRevInUseValid );
        printf( "P_Rev In Use        : %x\n",t->pRevInUse );
        printf( "BS P_Rev Valid      : %x\n",t->bsPRevValid );
        printf( "P_Rev In Use        : %x\n",t->bsPRev );
        printf( "CCS_supp Valid      : %x\n",t->ccsSupportedValid );
        printf( "CCS_supp            : %x\n",t->ccsSupported );
        printf( "System Id           : %x\n",t->systemID );
        printf( "Network Id          : %x\n",t->networkID );
        printf( "BS Info Valid       : %x\n",t->bsInfoValid );
        printf( "Base ID             : %x\n",t->baseId );
        printf( "Base Latitude       : %x\n",t->baseLat );
        printf( "Base Longitude      : %x\n",t->baseLong );
        printf( "Packet Zone Valid   : %x\n",t->packetZoneValid );
        printf( "Packet Zone         : %x\n",t->packetZone );
        printf( "Network ID Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MCC[lC] != 0xFF)
                printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC] != 0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
    }

    if ( (resp->pHDRSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 22)) )
    {
        printf( "\nHDR SYSTEM INFORMATION ----\n");
        nas_HDRSysInfo *t = resp->pHDRSysInfo;
        doprintsysInfoCommon( &t->sysInfoHDR );
        printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
        printf( "System PRL          : %x\n",t->isSysPrlMatch );
        printf( "Personality Valid   : %x\n",t->hdrPersonalityValid );
        printf( "Personality         : %x\n",t->hdrPersonality );
        printf( "Active Prot Valid   : %x\n",t->hdrActiveProtValid );
        printf( "Active Protocol     : %x\n",t->hdrActiveProt );
        printf( "IS-856 Sys Valid    : %x\n",t->is856SysIdValid );
        printf( "IS-856 system ID    : " );
        for ( lC = 0 ; lC < 16 ; lC++ )
        {
            printf( "%x ",t->is856SysId[lC] );
        }
        printf( "\n" );
    }
    if ( (resp->pGSMSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)) )
    {
        printf( "\nGSM SYSTEM INFORMATION ----\n");
        nas_GSMSysInfo *t = resp->pGSMSysInfo;
        doprintsysInfoCommon( &t->sysInfoGSM );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "EGPRS Support Valid : %x\n",t->egprsSuppValid );
        printf( "EGPRS Support       : %x\n",t->egprsSupp );
        printf( "DTM Support Valid   : %x\n",t->dtmSuppValid );
        printf( "DTM Support         : %x\n",t->dtmSupp );
    }
    if ( (resp->pWCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)) )
    {
        printf( "\nWCDMA SYSTEM INFORMATION ----\n");
        nas_WCDMASysInfo *t = resp->pWCDMASysInfo;
        doprintsysInfoCommon( &t->sysInfoWCDMA );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "HS Call Status Valid: %x\n",t->hsCallStatusValid );
        printf( "HS Call Status      : %x\n",t->hsCallStatus );
        printf( "HS Ind Valid        : %x\n",t->hsIndValid );
        printf( "HS Indication       : %x\n",t->hsInd );
        printf( "PSC Valid           : %x\n",t->pscValid );
        printf( "Primary Scrambling Code : %x\n",t->psc );
    }
    if ( (resp->pLTESysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)) )
    {
        printf( "\nLTE SYSTEM INFORMATION ----\n");
        nas_LTESysInfo *t = resp->pLTESysInfo;
        doprintsysInfoCommon( &t->sysInfoLTE );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "TAC Valid           : %x\n",t->tacValid );
        printf( "Tracking Area Code  : %x\n",t->tac );
    }
    if ( (resp->pAddCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 26)) )
    {
        printf( "Geo Sys Idx(CDMA) : %x\n", resp->pAddCDMASysInfo->geoSysIdx );
        printf("Reg Period (CDMA) : %x\n", resp->pAddCDMASysInfo->regPrd );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 27))
    {
        IFPRINTF( "Geo Sys Idx(HDR) : %x\n", resp->pAddHDRSysInfo );
    }
        if ( (resp->pAddGSMSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 28)) )
    {
        printf("Geo Sys Idx(GSM) : %x\n", resp->pAddGSMSysInfo->geoSysIdx );
        printf("Cell Br Cap(GSM) : %x\n", resp->pAddGSMSysInfo->cellBroadcastCap );
    }
    if ( (resp->pAddWCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 29)) )
    {
        printf("Geo Sys Idx(WCDMA) : %x\n", resp->pAddWCDMASysInfo->geoSysIdx );
        printf("Cell Br Cap(WCDMA) : %x\n", resp->pAddWCDMASysInfo->cellBroadcastCap );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 30))
    {
        IFPRINTF( "Geo Sys Idx(LTE) : %x\n", resp->pAddLTESysInfo );
    }
    if ( (resp->pGSMCallBarringSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 31)) )
    {
        printf( "CS Bar Status(GSM): %x\n", resp->pGSMCallBarringSysInfo->csBarStatus );
        printf( "PS Bar Status(GSM): %x\n", resp->pGSMCallBarringSysInfo->psBarStatus );
    }
    if ( (resp->pWCDMACallBarringSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 32)) )
    {
        printf( "CS Bar Status(WCDMA): %x\n", resp->pWCDMACallBarringSysInfo->csBarStatus );
        printf( "PS Bar Status(WCDMA): %x\n", resp->pWCDMACallBarringSysInfo->psBarStatus );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 33))
        IFPRINTF( "Voice Supp on LTE: %x\n", resp->pLTEVoiceSupportSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 34))
        IFPRINTF( "GSM Cipher Domain: %x\n", resp->pGSMCipherDomainSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 35))
        IFPRINTF( "WCDMA Cipher Domain: %x\n", resp->pWCDMACipherDomainSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 73))
        IFPRINTF( "Camped CIOT LTE Operational Mode: %04x\n", resp->pCampedCiotLteOpMode );

    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSysInfoParamPresenceMaskWhiteList,
        resp->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSysInfoParamPresenceMaskMandatoryList,
        resp->ParamPresenceMask);

    if ((resp->pCDMASysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)) )
    {
        if ( atoi((char*)resp->pCDMASysInfo->MCC) != MCC ||
             atoi((char*)resp->pCDMASysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pGSMSysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)) )
    {
        if ( atoi((char*)resp->pGSMSysInfo->MCC) != MCC ||
             atoi((char*)resp->pGSMSysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pWCDMASysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)) )
    {
        if ( atoi((char*)resp->pWCDMASysInfo->MCC) != MCC ||
             atoi((char*)resp->pWCDMASysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pLTESysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)) )
    {
        if ( atoi((char*)resp->pLTESysInfo->MCC) != MCC ||
             atoi((char*)resp->pLTESysInfo->MNC) != MNC)
             is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_SLQSGetSysInfoV2 (void *ptr)
{
    unpack_nas_SLQSGetSysInfoV2_t *resp =
            (unpack_nas_SLQSGetSysInfoV2_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    uint16_t              lC;
    if ( ( resp->pCDMASrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 16)) )
    {
        printf( "Service Status(CDMA) : %x\n",resp->pCDMASrvStatusInfo->srvStatus );
        printf( "Is Preferred(CDMA)   : %x\n",resp->pCDMASrvStatusInfo->isPrefDataPath );
    }
    if ( ( resp->pHDRSrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 17)) )
    {
        printf( "Service Status(HDR) : %x\n",resp->pHDRSrvStatusInfo->srvStatus );
        printf( "Is Preferred(HDR)   : %x\n",resp->pHDRSrvStatusInfo->isPrefDataPath );
    }
    if (( resp->pGSMSrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 18)) )
    {
        printf( "Service Status(GSM)      : %x\n",resp->pGSMSrvStatusInfo->srvStatus );
        printf( "True Service Status(GSM) : %x\n",resp->pGSMSrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(GSM)        : %x\n",resp->pGSMSrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pWCDMASrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 19)) )
    {
        printf( "Service Status(WCDMA)     : %x\n",resp->pWCDMASrvStatusInfo->srvStatus );
        printf( "True Service Status(WCDMA): %x\n",resp->pWCDMASrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(WCDMA)       : %x\n",resp->pWCDMASrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pLTESrvStatusInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 20)) )
    {
        printf( "Service Status(LTE)      : %x\n",resp->pLTESrvStatusInfo->srvStatus );
        printf( "True Service Status(LTE) : %x\n",resp->pLTESrvStatusInfo->trueSrvStatus );
        printf( "Is Preferred(LTE)        : %x\n",resp->pLTESrvStatusInfo->isPrefDataPath );
    }
    if ( (resp->pCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)) )
    {
        printf( "\nCDMA SYSTEM INFORMATION ----\n");
        nas_CDMASysInfo *t = resp->pCDMASysInfo;
        doprintsysInfoCommon( &t->sysInfoCDMA );
        printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
        printf( "System PRL          : %x\n",t->isSysPrlMatch );
        printf( "P_Rev Valid         : %x\n",t->pRevInUseValid );
        printf( "P_Rev In Use        : %x\n",t->pRevInUse );
        printf( "BS P_Rev Valid      : %x\n",t->bsPRevValid );
        printf( "P_Rev In Use        : %x\n",t->bsPRev );
        printf( "CCS_supp Valid      : %x\n",t->ccsSupportedValid );
        printf( "CCS_supp            : %x\n",t->ccsSupported );
        printf( "System Id           : %x\n",t->systemID );
        printf( "Network Id          : %x\n",t->networkID );
        printf( "BS Info Valid       : %x\n",t->bsInfoValid );
        printf( "Base ID             : %x\n",t->baseId );
        printf( "Base Latitude       : %x\n",t->baseLat );
        printf( "Base Longitude      : %x\n",t->baseLong );
        printf( "Packet Zone Valid   : %x\n",t->packetZoneValid );
        printf( "Packet Zone         : %x\n",t->packetZone );
        printf( "Network ID Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MCC[lC] != 0xFF)
                printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC] != 0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
    }

    if ( (resp->pHDRSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 22)) )
    {
        printf( "\nHDR SYSTEM INFORMATION ----\n");
        nas_HDRSysInfo *t = resp->pHDRSysInfo;
        doprintsysInfoCommon( &t->sysInfoHDR );
        printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
        printf( "System PRL          : %x\n",t->isSysPrlMatch );
        printf( "Personality Valid   : %x\n",t->hdrPersonalityValid );
        printf( "Personality         : %x\n",t->hdrPersonality );
        printf( "Active Prot Valid   : %x\n",t->hdrActiveProtValid );
        printf( "Active Protocol     : %x\n",t->hdrActiveProt );
        printf( "IS-856 Sys Valid    : %x\n",t->is856SysIdValid );
        printf( "IS-856 system ID    : " );
        for ( lC = 0 ; lC < 16 ; lC++ )
        {
            printf( "%x ",t->is856SysId[lC] );
        }
        printf( "\n" );
    }
    if ( (resp->pGSMSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)) )
    {
        printf( "\nGSM SYSTEM INFORMATION ----\n");
        nas_GSMSysInfo *t = resp->pGSMSysInfo;
        doprintsysInfoCommon( &t->sysInfoGSM );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "EGPRS Support Valid : %x\n",t->egprsSuppValid );
        printf( "EGPRS Support       : %x\n",t->egprsSupp );
        printf( "DTM Support Valid   : %x\n",t->dtmSuppValid );
        printf( "DTM Support         : %x\n",t->dtmSupp );
    }
    if ( (resp->pWCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)) )
    {
        printf( "\nWCDMA SYSTEM INFORMATION ----\n");
        nas_WCDMASysInfo *t = resp->pWCDMASysInfo;
        doprintsysInfoCommon( &t->sysInfoWCDMA );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "HS Call Status Valid: %x\n",t->hsCallStatusValid );
        printf( "HS Call Status      : %x\n",t->hsCallStatus );
        printf( "HS Ind Valid        : %x\n",t->hsIndValid );
        printf( "HS Indication       : %x\n",t->hsInd );
        printf( "PSC Valid           : %x\n",t->pscValid );
        printf( "Primary Scrambling Code : %x\n",t->psc );
    }
    if ( (resp->pLTESysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)) )
    {
        printf( "\nLTE SYSTEM INFORMATION ----\n");
        nas_LTESysInfo *t = resp->pLTESysInfo;
        doprintsysInfoCommon( &t->sysInfoLTE );
        printf( "LAC Valid           : %x\n",t->lacValid );
        printf( "LAC                 : %x\n",t->lac );
        printf( "Cell ID Valid       : %x\n",t->cellIdValid );
        printf( "Cell ID             : %x\n",t->cellId );
        printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : %x\n",t->rejCause );
        printf( "Network Id Valid    : %x\n",t->networkIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%c",t->MCC[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            if(t->MNC[lC]!=0xFF)
                printf( "%c",t->MNC[lC] );
        }
        printf( "\n" );
        printf( "TAC Valid           : %x\n",t->tacValid );
        printf( "Tracking Area Code  : %x\n",t->tac );
    }
    if ( (resp->pAddCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 26)) )
    {
        printf( "Geo Sys Idx(CDMA) : %x\n", resp->pAddCDMASysInfo->geoSysIdx );
        printf("Reg Period (CDMA) : %x\n", resp->pAddCDMASysInfo->regPrd );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 27))
    {
        IFPRINTF( "Geo Sys Idx(HDR) : %x\n", resp->pAddHDRSysInfo );
    }
        if ( (resp->pAddGSMSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 28)) )
    {
        printf("Geo Sys Idx(GSM) : %x\n", resp->pAddGSMSysInfo->geoSysIdx );
        printf("Cell Br Cap(GSM) : %x\n", resp->pAddGSMSysInfo->cellBroadcastCap );
    }
    if ( (resp->pAddWCDMASysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 29)) )
    {
        printf("Geo Sys Idx(WCDMA) : %x\n", resp->pAddWCDMASysInfo->geoSysIdx );
        printf("Cell Br Cap(WCDMA) : %x\n", resp->pAddWCDMASysInfo->cellBroadcastCap );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 30))
    {
        IFPRINTF( "Geo Sys Idx(LTE) : %x\n", resp->pAddLTESysInfo );
    }
    if ( (resp->pGSMCallBarringSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 31)) )
    {
        printf( "CS Bar Status(GSM): %x\n", resp->pGSMCallBarringSysInfo->csBarStatus );
        printf( "PS Bar Status(GSM): %x\n", resp->pGSMCallBarringSysInfo->psBarStatus );
    }
    if ( (resp->pWCDMACallBarringSysInfo ) && (swi_uint256_get_bit (resp->ParamPresenceMask, 32)) )
    {
        printf( "CS Bar Status(WCDMA): %x\n", resp->pWCDMACallBarringSysInfo->csBarStatus );
        printf( "PS Bar Status(WCDMA): %x\n", resp->pWCDMACallBarringSysInfo->psBarStatus );
    }
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 33))
        IFPRINTF( "Voice Supp on LTE: %x\n", resp->pLTEVoiceSupportSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 34))
        IFPRINTF( "GSM Cipher Domain: %x\n", resp->pGSMCipherDomainSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 35))
        IFPRINTF( "WCDMA Cipher Domain: %x\n", resp->pWCDMACipherDomainSysInfo );
    if(swi_uint256_get_bit (resp->ParamPresenceMask, 73))
        IFPRINTF( "Camped CIOT LTE Operational Mode: %04x\n", resp->pCampedCiotLteOpMode );
    if ( resp->pLteEmbmsCoverage &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 38)))
    {
        printf( "LTE eMBMS Coverage Info: %x\n", resp->pLteEmbmsCoverage->LteEmbmsCoverage );
    }
    if ( resp->pSimRejInfo &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 39)))
    {
        printf( "SIM Reject Information: %d\n", resp->pSimRejInfo->SimRejInfo );
    }
    if ( resp->pImsVoiceSupportLte &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 41)))
    {
        printf( "IMS Voice Support Status on LTE: %x\n", resp->pImsVoiceSupportLte->ImsVoiceSupportLte );
    }
    if ( resp->pLteVoiceDomain &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 42)))
    {
        printf( "LTE Voice Domain: %d\n", resp->pLteVoiceDomain->LteVoiceDomain );
    }
    if ( resp->pSrvRegRestriction &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 47)))
    {
        printf( "Network Selection Registration Restriction: %d\n", resp->pSrvRegRestriction->SrvRegRestriction );
    }
    if ( resp->pLteRegDomain &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 49)))
    {
        printf( "LTE Registration Domain: %d\n", resp->pLteRegDomain->LteRegDomain );
    }
    if ( resp->pLteEmbmsTraceId &&  (swi_uint256_get_bit (resp->ParamPresenceMask, 52)))
    {
        printf( "LTE eMBMS Coverage Info Trace ID: %d\n", resp->pLteEmbmsTraceId->LteEmbmsTraceId );
    }
    if ( resp->pNR5GSrvStatusinfo && (swi_uint256_get_bit (resp->ParamPresenceMask, 74)))
    {
         printf( "\nNR5G Service Status Info \n");
         nas_NR5GSrvStatusTlv *t = resp->pNR5GSrvStatusinfo;

         printf( "Service Status   : 0x%02x\n",t->srvStatus );
         printf( "True Service Status    : 0x%02x\n",t->trueSrvStatus );
         printf( "Is RAT Preferred Data Path: 0x%02x\n",t->isPrefDataPath );
    }
    if ( resp->pNR5GSysInfo && (swi_uint256_get_bit (resp->ParamPresenceMask, 75)))
    {
        printf( "\nNR5G System Information\n");
        nas_NR5GSysInfoTlv *t = resp->pNR5GSysInfo;

        printf( "Service Domain Valid   : 0x%02x\n",t->srvDomainValid );
        printf( "Service Domain         : 0x%02x\n",t->srvDomain );
        printf( "Service Capability Valid: 0x%02x\n",t->srvCapabilityValid );
        printf( "Service Capability     : 0x%02x\n",t->srvCapability );
        printf( "Roam Status Valid      : 0x%02x\n",t->roamStatusValid );
        printf( "Roam Status            : 0x%02x\n",t->roamStatus );
        printf( "Forbidden System Valid : 0x%02x\n",t->isSysForbiddenValid );
        printf( "Is Forbidden System    : 0x%02x\n",t->isSysForbidden );
        printf( "LAC Valid           : 0x%02x\n",t->lacValid );
        printf( "LAC                 : 0x%04x\n",t->lac );
        printf( "Cell ID Valid       : 0x%02x\n",t->cellIdValid );
        printf( "Cell ID             : %d\n",t->cellId );
        printf( "Reg Rej Info Valid  : 0x%02x\n",t->regRejectInfoValid );
        printf( "Reject Srvc Domain  : 0x%02x\n",t->rejectSrvDomain );
        printf( "Reject Cause        : 0x%02x\n",t->rejectCause );
        printf( "Network Id Valid    : 0x%02x\n",t->nwIdValid );
        printf( "MCC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%x ",t->mcc[lC] );
        }
        printf( "\n" );
        printf( "MNC info            : " );
        for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
        {
            printf( "%x ",t->mnc[lC] );
        }
        printf( "\n" );
        printf( "TAC Valid           : 0x%02x\n",t->tacValid );
        printf( "Tracking Area Code  : 0x%04x\n",t->tac );
    }
    if ( resp->pNR5GCellStatusInfo && (swi_uint256_get_bit (resp->ParamPresenceMask, 76)))
    {
        printf( "\nNR5G Cell Information \n");
        nas_NR5GCellStatusInfoTlv *t = resp->pNR5GCellStatusInfo;

        printf( "NR 5G Cell Status   : %d\n",t->nr5gCellStatus );
    }

    printf("%s Data\n",__FUNCTION__);

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSysInfoParamPresenceMaskWhiteList,
        resp->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSysInfoParamPresenceMaskMandatoryList,
        resp->ParamPresenceMask);

    if ((resp->pCDMASysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 21)) )
    {
        if ( atoi((char*)resp->pCDMASysInfo->MCC) != MCC ||
             atoi((char*)resp->pCDMASysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pGSMSysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 23)) )
    {
        if ( atoi((char*)resp->pGSMSysInfo->MCC) != MCC ||
             atoi((char*)resp->pGSMSysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pWCDMASysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 24)) )
    {
        if ( atoi((char*)resp->pWCDMASysInfo->MCC) != MCC ||
             atoi((char*)resp->pWCDMASysInfo->MNC) != MNC)
             is_matching = 0;
    }
    else if ((resp->pLTESysInfo) && (swi_uint256_get_bit (resp->ParamPresenceMask, 25)) )
    {
        if ( atoi((char*)resp->pLTESysInfo->MCC) != MCC ||
             atoi((char*)resp->pLTESysInfo->MNC) != MNC)
             is_matching = 0;
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif
}

void dump_GetNetworkPreference(void *ptr)
{
    unpack_nas_GetNetworkPreference_t *result =
            (unpack_nas_GetNetworkPreference_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if ( result != NULL)
    {
        printf("%s Result: %d\n",__FUNCTION__, result->Tlvresult);
        if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
        {
            printf("ActiveTechPref: %d\n", result->ActiveTechPref);
            printf("Duration: %d\n", result->Duration);
            DefaultNetworkPreference.Duration = result->Duration;
            DefaultNetworkPreference.TechnologyPref = result->ActiveTechPref;
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        {
            printf("PersistentTechPref: %d\n", result->PersistentTechPref);
        }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_GetNetworkPreferenceParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_GetNetworkPreferenceParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

        if ( result != NULL)
        {
            /* Bit 0 - Technology is 3GPP2, Bit 1 - Technology is 3GPP, Bits 0 and 1 are exclusive;
               only one may be set at a time, valid range for duration is 0-6 */
            if (((CHECK_BIT(result->ActiveTechPref, 0)&CHECK_BIT(result->ActiveTechPref, 1)) == 1) ||
                (result->Duration > 6))
               is_matching = 0;
        }
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        }
#endif    
}

void dump_SLQSNASSwiGetChannelLockSettings(void *ptr)
{
    unpack_nas_SLQSNASSwiGetChannelLock_t *result =
            (unpack_nas_SLQSNASSwiGetChannelLock_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if((result->pWcdmaUARFCN) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("\nWCDMA UARFCN:\n");
        printf("Status: 0x%02X\n",result->pWcdmaUARFCN->status);
        printf("UARFCN: %u\n",result->pWcdmaUARFCN->uarfcn);
    }
    if((result->pLteEARFCN) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("\nLTE EARFCN:\n");
        printf("Status: 0x%02X\n",result->pLteEARFCN->status);
        printf("EARFCN Primary: %u\n",result->pLteEARFCN->earfcn0);
        printf("EARFCN Secondary: %u\n",result->pLteEARFCN->earfcn1);
    }
    if((result->pLtePCI) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf("\nLTE PCI:\n");
        printf("Status: 0x%02X\n",result->pLtePCI->status);
        printf("EARFCN: %u\n",result->pLtePCI->earfcn);
        printf("PCI: %u\n",result->pLtePCI->pci);
    }
    printf("%s Data\n",__FUNCTION__);
    if( CHECK_PTR_IS_NOT_NULL(DefaultNASSwiSetChannelLock.pLtePCI) &&
         (result->pLtePCI) )
    {
        memcpy(DefaultNASSwiSetChannelLock.pLtePCI,result->pLtePCI,sizeof(nas_ltePCI));
    }    
    if( CHECK_PTR_IS_NOT_NULL(DefaultNASSwiSetChannelLock.pLteEARFCN) &&
         (result->pLteEARFCN))
    {
        memcpy(DefaultNASSwiSetChannelLock.pLteEARFCN,result->pLteEARFCN,sizeof(nas_lteEARFCN));
    }
    if( CHECK_PTR_IS_NOT_NULL(DefaultNASSwiSetChannelLock.pWcdmaUARFCN) &&
         (result->pWcdmaUARFCN))
    {
        memcpy(DefaultNASSwiSetChannelLock.pWcdmaUARFCN,result->pWcdmaUARFCN,sizeof(nas_wcdmaUARFCN));
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetACCOLCSettings (void *ptr)
{
    uint8_t *pAccolc = (uint8_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("ACCOLC : %d\n", *pAccolc );
    DefaultACCOLC.accolc = *pAccolc;
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* status should be 0~15 */
    if ( *pAccolc > 15) 
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif
}

void dump_GetCDMANetworkParametersSettings (void *ptr)
{
    unpack_nas_GetCDMANetworkParameters_t *pGetCDMANetPara =
    (unpack_nas_GetCDMANetworkParameters_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pGetCDMANetPara->ParamPresenceMask);
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 17))
        printf("SCI           : %d\n", pGetCDMANetPara->SCI);
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 18))
        printf("SCM           : %d\n", pGetCDMANetPara->SCM);
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 19))
    {
        printf("RegHomeSID    : %d\n", pGetCDMANetPara->RegHomeSID);
        printf("RegForeignSID : %d\n", pGetCDMANetPara->RegForeignSID);
        printf("RegForeignNID : %d\n", pGetCDMANetPara->RegForeignNID);
    }
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 21))
    {
        printf("CustomSCP     : %d\n", pGetCDMANetPara->CustomSCP);
        printf("Protocol      : %d\n", pGetCDMANetPara->Protocol);
        printf("Broadcast     : %d\n", pGetCDMANetPara->Broadcast);
        printf("Application   : %d\n", pGetCDMANetPara->Application);
    }
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 22))
    {
        printf("Roaming       : %d\n", pGetCDMANetPara->Roaming);
    }
    if((CHECK_PTR_IS_NOT_NULL(DefaultCDMANetworkParameters.pRoaming)) &&
            (swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 22)))
    {
        *(DefaultCDMANetworkParameters.pRoaming) = pGetCDMANetPara->Roaming;
    }
    if(swi_uint256_get_bit (pGetCDMANetPara->ParamPresenceMask, 21))
    {
        if(CHECK_PTR_IS_NOT_NULL(DefaultCDMANetworkParameters.pApplication))
        {
            *(DefaultCDMANetworkParameters.pApplication) = pGetCDMANetPara->Application;
        }
        if(CHECK_PTR_IS_NOT_NULL(DefaultCDMANetworkParameters.pBroadcast))
        {
            *(DefaultCDMANetworkParameters.pBroadcast) = pGetCDMANetPara->Broadcast;
        }
        if(CHECK_PTR_IS_NOT_NULL(DefaultCDMANetworkParameters.pCustomSCP))
        {
            *(DefaultCDMANetworkParameters.pCustomSCP) = pGetCDMANetPara->CustomSCP;
        }
        if(CHECK_PTR_IS_NOT_NULL(DefaultCDMANetworkParameters.pProtocol))
        {
            *(DefaultCDMANetworkParameters.pProtocol) = pGetCDMANetPara->Protocol;
        }
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSGetSysSelectionPrefSettings (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    if(pGetSysSelPref!=NULL)
    {
        if( (pGetSysSelPref->pEmerMode!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 16)) )
            printf( "Emergency Mode: 0x%x\n",               *pGetSysSelPref->pEmerMode );
        if( (pGetSysSelPref->pModePref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 17)) )
            printf( "Mode Preference: 0x%x\n",              *pGetSysSelPref->pModePref );
        if( (pGetSysSelPref->pBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 18)) )
            printf( "Band Preference: 0x%"PRIX64"\n",       *pGetSysSelPref->pBandPref );
        if( (pGetSysSelPref->pPRLPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 19)) )
            printf( "CDMA PRL Preference : 0x%x\n",         *pGetSysSelPref->pPRLPref );
        if( (pGetSysSelPref->pRoamPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 20)) )
            printf( "Roaming Preference : 0x%x\n",          *pGetSysSelPref->pRoamPref );
        if( (pGetSysSelPref->pLTEBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 21)) )
            printf( "LTE Band Preference : 0x%"PRIX64"\n",      *pGetSysSelPref->pLTEBandPref );
        if( (pGetSysSelPref->pNetSelPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 22)) )
            printf( "Network Selection Preference: 0x%x\n", *pGetSysSelPref->pNetSelPref );
        if( (pGetSysSelPref->pSrvDomainPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 24)) )
            printf( "Service Domain Preference : 0x%x\n",  *pGetSysSelPref->pSrvDomainPref );
        if( (pGetSysSelPref->pGWAcqOrderPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 25)) )
            printf( "GSM - WCDMA Acquisition Order Preference : 0x%x\n",*pGetSysSelPref->pGWAcqOrderPref );
        DefaultSysSelPref.pEmerMode = pGetSysSelPref->pEmerMode;
        DefaultSysSelPref.pModePref = pGetSysSelPref->pModePref;
        DefaultSysSelPref.pPRLPref = pGetSysSelPref->pPRLPref;
        DefaultSysSelPref.pRoamPref = pGetSysSelPref->pRoamPref;
        DefaultSysSelPref.pSrvDomainPref = pGetSysSelPref->pSrvDomainPref;
        DefaultSysSelPref.pLTEBandPref = pGetSysSelPref->pLTEBandPref;
        DefaultSysSelPref.pBandPref = pGetSysSelPref->pBandPref;
        if( (pGetSysSelPref->pBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 18)) )
            DefaultBandPreference.bandPref = *(pGetSysSelPref->pBandPref); 
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskWhiteList,
        pGetSysSelPref->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskMandatoryList,
        pGetSysSelPref->ParamPresenceMask);

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
      (pGetSysSelPref->pModePref!=NULL))
    {
        /* emergency mode can be OFF (0) or ON(1), valid bit mask for mode preference is Bit0-Bit5 */
        if ((*pGetSysSelPref->pEmerMode > 1) ||
           CHECK_BIT(*pGetSysSelPref->pModePref, 6))
        {
            is_matching = 0;
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetSysSelectionPrefModemSettings (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPref_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPref_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    if(pGetSysSelPref!=NULL)
    {
        if( (pGetSysSelPref->pEmerMode!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 16)) )
            *ModemSetSysSelPref.pEmerMode = *pGetSysSelPref->pEmerMode;
        else
            ModemSetSysSelPref.pEmerMode = NULL;
        if( (pGetSysSelPref->pModePref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 17)) )
            *ModemSetSysSelPref.pModePref = *pGetSysSelPref->pModePref;
        else
            ModemSetSysSelPref.pModePref = NULL;
        if( (pGetSysSelPref->pBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 18)) )
            *ModemSetSysSelPref.pBandPref = *pGetSysSelPref->pBandPref;
        else
            ModemSetSysSelPref.pBandPref = NULL;
        if( (pGetSysSelPref->pPRLPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 19)) )
            *ModemSetSysSelPref.pPRLPref = *pGetSysSelPref->pPRLPref;
        else
            ModemSetSysSelPref.pPRLPref = NULL;
        if( (pGetSysSelPref->pRoamPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 20)) )
            *ModemSetSysSelPref.pRoamPref = *pGetSysSelPref->pRoamPref;
        else
            ModemSetSysSelPref.pRoamPref = NULL;
        if( (pGetSysSelPref->pLTEBandPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 21)) )
            *ModemSetSysSelPref.pLTEBandPref = *pGetSysSelPref->pLTEBandPref;
        else
            ModemSetSysSelPref.pLTEBandPref = NULL;
        if( (pGetSysSelPref->pSrvDomainPref!=NULL) && (swi_uint256_get_bit (pGetSysSelPref->ParamPresenceMask, 24)) )
            *ModemSetSysSelPref.pSrvDomainPref = *pGetSysSelPref->pSrvDomainPref;
        else
            ModemSetSysSelPref.pSrvDomainPref = NULL;

    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskWhiteList,
        pGetSysSelPref->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskMandatoryList,
        pGetSysSelPref->ParamPresenceMask);

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
      (pGetSysSelPref->pModePref!=NULL))
    {
        /* emergency mode can be OFF (0) or ON(1), valid bit mask for mode preference is Bit0-Bit5 */
        if ((*pGetSysSelPref->pEmerMode > 1) ||
           CHECK_BIT(*pGetSysSelPref->pModePref, 6))
        {
            is_matching = 0;
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? SUCCESS_MSG : FAILED_MSG));
    }
#endif

}

void dump_SLQSGetSysSelectionPrefExtSettings (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPrefExt_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPrefExt_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    if(pGetSysSelPref!=NULL)
    {
        if(pGetSysSelPref->pEmerMode!=NULL && pGetSysSelPref->pEmerMode->TlvPresent)
            printf( "Emergency Mode: 0x%x\n",               pGetSysSelPref->pEmerMode->EmerMode );
        if(pGetSysSelPref->pModePref!=NULL && pGetSysSelPref->pModePref->TlvPresent)
            printf( "Mode Preference: 0x%x\n",              pGetSysSelPref->pModePref->ModePref);
        if(pGetSysSelPref->pBandPref!=NULL && pGetSysSelPref->pBandPref->TlvPresent)
            printf( "Band Preference: 0x%"PRIX64"\n",       pGetSysSelPref->pBandPref->BandPref );
        if(pGetSysSelPref->pPRLPref!=NULL && pGetSysSelPref->pPRLPref->TlvPresent)
            printf( "CDMA PRL Preference : 0x%x\n",         pGetSysSelPref->pPRLPref->PRLPref );
        if(pGetSysSelPref->pRoamPref!=NULL && pGetSysSelPref->pRoamPref->TlvPresent)
            printf( "Roaming Preference : 0x%x\n",          pGetSysSelPref->pRoamPref->RoamPref );
        if(pGetSysSelPref->pLTEBandPref!=NULL && pGetSysSelPref->pLTEBandPref->TlvPresent)
        printf( "LTE Band Preference : 0x%"PRIX64"\n",      pGetSysSelPref->pLTEBandPref->LTEBandPref );
        if(pGetSysSelPref->pNetSelPref!=NULL && pGetSysSelPref->pNetSelPref->TlvPresent)
            printf( "Network Selection Preference: 0x%x\n", pGetSysSelPref->pNetSelPref->NetSelPref );
        if(pGetSysSelPref->pSrvDomainPref!=NULL && pGetSysSelPref->pSrvDomainPref->TlvPresent)
            printf( "Service Domain Preference : 0x%x\n",  pGetSysSelPref->pSrvDomainPref->SrvDomainPref );
        if(pGetSysSelPref->pGWAcqOrderPref!=NULL && pGetSysSelPref->pGWAcqOrderPref->TlvPresent)
            printf( "GSM - WCDMA Acquisition Order Preference : 0x%x\n",pGetSysSelPref->pGWAcqOrderPref->GWAcqOrderPref );
        if( pGetSysSelPref->pAcqOrderPref && pGetSysSelPref->pAcqOrderPref->TlvPresent)
        {
            printf( "\nAcquisition Order Preference :\n");
            uint8_t i = 0;

            printf( "acq order len : 0x%x\n", pGetSysSelPref->pAcqOrderPref->acqOrdeLen);

            for ( i = 0; i < pGetSysSelPref->pAcqOrderPref->acqOrdeLen; i++ )
            {
                printf( "acq order: 0x%d\n", pGetSysSelPref->pAcqOrderPref->pAcqOrder[i] );
            }
        }
        if( pGetSysSelPref->pRatDisabledMask && pGetSysSelPref->pRatDisabledMask->TlvPresent)
            printf( "Disabled RAT Bitmask: 0x%04x\n", pGetSysSelPref->pRatDisabledMask->ratDisabledMask );
        if( pGetSysSelPref->pCiotLteOpModePref && pGetSysSelPref->pCiotLteOpModePref->TlvPresent)
            printf( "CIOT LTE Operational Mode Preference : 0x%"PRIX64"\n", pGetSysSelPref->pCiotLteOpModePref->ciotLteOpModePref );
        if( pGetSysSelPref->pLteM1BandPref && pGetSysSelPref->pLteM1BandPref->TlvPresent)
            printf( "LTE M1 Band Preference : 0x%"PRIX64"\n", pGetSysSelPref->pLteM1BandPref->lteM1BandPref );
        if( pGetSysSelPref->pLteNb1BandPref && pGetSysSelPref->pLteNb1BandPref->TlvPresent)
            printf( "LTE NB1 Band Preference : 0x%"PRIX64"\n", pGetSysSelPref->pLteNb1BandPref->lteNb1BandPref );
        if( pGetSysSelPref->pCiotAcqOrderPref &&  pGetSysSelPref->pCiotAcqOrderPref->TlvPresent)
        {
            printf( "\nCIOT Acquisition Order Preference :\n");
            uint8_t i = 0;

            printf( "CIOT acq order len : 0x%x\n", pGetSysSelPref->pCiotAcqOrderPref->ciotAcqOrderLen);

            for ( i = 0; i < pGetSysSelPref->pCiotAcqOrderPref->ciotAcqOrderLen; i++ )
            {
                printf( "CIOTacq order: 0x%x\n", pGetSysSelPref->pCiotAcqOrderPref->pCiotAcqOrder[i] );
            }
        }
        if( pGetSysSelPref->pNr5gBandPref && pGetSysSelPref->pNr5gBandPref->TlvPresent)
        {
            printf( "\nNR5G Band Preference :\n");
            printf( "bits_1_64: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_1_64 );
            printf( "bits_65_128: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_65_128 );
            printf( "bits_129_192: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_129_192 );
            printf( "bits_193_256: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_193_256 );
         }
        if( pGetSysSelPref->pLTEBandPrefExt && pGetSysSelPref->pLTEBandPrefExt->TlvPresent)
        {
            printf( "\nLTE Band Preference Ext :\n");
            printf( "bits_1_64: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_1_64 );
            printf( "bits_65_128: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_65_128 );
            printf( "bits_129_192: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_129_192 );
            printf( "bits_193_256: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_193_256 );
         }
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;
    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
      (pGetSysSelPref->pModePref!=NULL))
    {
        /* emergency mode can be OFF (0) or ON(1), valid bit mask for mode preference is Bit0-Bit5 */
        if ((pGetSysSelPref->pEmerMode->EmerMode > 1) ||
           CHECK_BIT(pGetSysSelPref->pModePref->ModePref, 6))
        {
            is_matching = 0;
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
    }
#endif

}

void dump_SLQSGetSysSelectionPrefExtV2 (void *ptr)
{
    unpack_nas_SLQSGetSysSelectionPrefExtV2_t *pGetSysSelPref =
        (unpack_nas_SLQSGetSysSelectionPrefExtV2_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    if(pGetSysSelPref!=NULL)
    {
        if(pGetSysSelPref->pEmerMode!=NULL && pGetSysSelPref->pEmerMode->TlvPresent)
            printf( "Emergency Mode: 0x%x\n",               pGetSysSelPref->pEmerMode->EmerMode );
        if(pGetSysSelPref->pModePref!=NULL && pGetSysSelPref->pModePref->TlvPresent)
            printf( "Mode Preference: 0x%x\n",              pGetSysSelPref->pModePref->ModePref);
        if(pGetSysSelPref->pBandPref!=NULL && pGetSysSelPref->pBandPref->TlvPresent)
            printf( "Band Preference: 0x%"PRIX64"\n",       pGetSysSelPref->pBandPref->BandPref );
        if(pGetSysSelPref->pPRLPref!=NULL && pGetSysSelPref->pPRLPref->TlvPresent)
            printf( "CDMA PRL Preference : 0x%x\n",         pGetSysSelPref->pPRLPref->PRLPref );
        if(pGetSysSelPref->pRoamPref!=NULL && pGetSysSelPref->pRoamPref->TlvPresent)
            printf( "Roaming Preference : 0x%x\n",          pGetSysSelPref->pRoamPref->RoamPref );
        if(pGetSysSelPref->pLTEBandPref!=NULL && pGetSysSelPref->pLTEBandPref->TlvPresent)
        printf( "LTE Band Preference : 0x%"PRIX64"\n",      pGetSysSelPref->pLTEBandPref->LTEBandPref );
        if(pGetSysSelPref->pNetSelPref!=NULL && pGetSysSelPref->pNetSelPref->TlvPresent)
            printf( "Network Selection Preference: 0x%x\n", pGetSysSelPref->pNetSelPref->NetSelPref );
        if(pGetSysSelPref->pSrvDomainPref!=NULL && pGetSysSelPref->pSrvDomainPref->TlvPresent)
            printf( "Service Domain Preference : 0x%x\n",  pGetSysSelPref->pSrvDomainPref->SrvDomainPref );
        if(pGetSysSelPref->pGWAcqOrderPref!=NULL && pGetSysSelPref->pGWAcqOrderPref->TlvPresent)
            printf( "GSM - WCDMA Acquisition Order Preference : 0x%x\n",pGetSysSelPref->pGWAcqOrderPref->GWAcqOrderPref );
        if( pGetSysSelPref->pAcqOrderPref && pGetSysSelPref->pAcqOrderPref->TlvPresent)
        {
            printf( "\nAcquisition Order Preference :\n");
            uint8_t i = 0;

            printf( "acq order len : 0x%x\n", pGetSysSelPref->pAcqOrderPref->acqOrdeLen);

            for ( i = 0; i < pGetSysSelPref->pAcqOrderPref->acqOrdeLen; i++ )
            {
                printf( "acq order: 0x%d\n", pGetSysSelPref->pAcqOrderPref->pAcqOrder[i] );
            }
        }
        if( pGetSysSelPref->pRatDisabledMask && pGetSysSelPref->pRatDisabledMask->TlvPresent)
            printf( "Disabled RAT Bitmask: 0x%04x\n", pGetSysSelPref->pRatDisabledMask->ratDisabledMask );
        if( pGetSysSelPref->pCiotLteOpModePref && pGetSysSelPref->pCiotLteOpModePref->TlvPresent)
            printf( "CIOT LTE Operational Mode Preference : 0x%"PRIX64"\n", pGetSysSelPref->pCiotLteOpModePref->ciotLteOpModePref );
        if( pGetSysSelPref->pLteM1BandPref && pGetSysSelPref->pLteM1BandPref->TlvPresent)
            printf( "LTE M1 Band Preference : 0x%"PRIX64"\n", pGetSysSelPref->pLteM1BandPref->lteM1BandPref );
        if( pGetSysSelPref->pLteNb1BandPref && pGetSysSelPref->pLteNb1BandPref->TlvPresent)
            printf( "LTE NB1 Band Preference : 0x%"PRIX64"\n", pGetSysSelPref->pLteNb1BandPref->lteNb1BandPref );
        if( pGetSysSelPref->pCiotAcqOrderPref &&  pGetSysSelPref->pCiotAcqOrderPref->TlvPresent)
        {
            printf( "\nCIOT Acquisition Order Preference :\n");
            uint8_t i = 0;

            printf( "CIOT acq order len : 0x%x\n", pGetSysSelPref->pCiotAcqOrderPref->ciotAcqOrderLen);

            for ( i = 0; i < pGetSysSelPref->pCiotAcqOrderPref->ciotAcqOrderLen; i++ )
            {
                printf( "CIOTacq order: 0x%x\n", pGetSysSelPref->pCiotAcqOrderPref->pCiotAcqOrder[i] );
            }
        }
        if( pGetSysSelPref->pNr5gBandPref && pGetSysSelPref->pNr5gBandPref->TlvPresent)
        {
            printf( "\nNR5G Band Preference :\n");
            printf( "bits_1_64: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_1_64 );
            printf( "bits_65_128: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_65_128 );
            printf( "bits_129_192: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_129_192 );
            printf( "bits_193_256: 0x%"PRIX64"\n", pGetSysSelPref->pNr5gBandPref->bits_193_256 );
         }
        if( pGetSysSelPref->pLTEBandPrefExt && pGetSysSelPref->pLTEBandPrefExt->TlvPresent)
        {
            printf( "\nLTE Band Preference Ext :\n");
            printf( "bits_1_64: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_1_64 );
            printf( "bits_65_128: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_65_128 );
            printf( "bits_129_192: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_129_192 );
            printf( "bits_193_256: 0x%"PRIX64"\n", pGetSysSelPref->pLTEBandPrefExt->bits_193_256 );
         }
        if(pGetSysSelPref->pTDSCDMABandPref && pGetSysSelPref->pTDSCDMABandPref->TlvPresent)
            printf( "TDSCDMA CDMA Band Preference : 0x%"PRIX64"\n", pGetSysSelPref->pTDSCDMABandPref->TdscdmaBandPref );
        if(pGetSysSelPref->pSrvRegRestric && pGetSysSelPref->pSrvRegRestric->TlvPresent)
            printf( "Service Registration Restriction: 0x%04x\n", pGetSysSelPref->pSrvRegRestric->SrvRegRestriction );
        if(pGetSysSelPref->pUsageSetting && pGetSysSelPref->pUsageSetting->TlvPresent)
            printf( "Modem Usage Pref: 0x%04x\n", pGetSysSelPref->pUsageSetting->UsageSetting );
        if(pGetSysSelPref->pVoiceDomainPref && pGetSysSelPref->pVoiceDomainPref->TlvPresent)
            printf( "Voice Domain Pref: 0x%04x\n", pGetSysSelPref->pVoiceDomainPref->VoiceDomainPref );
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskWhiteList,
        pGetSysSelPref->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_nas_SLQSGetSysSelectionPrefParamPresenceMaskMandatoryList,
        pGetSysSelPref->ParamPresenceMask);

    if(pGetSysSelPref!=NULL)
    if((pGetSysSelPref->pEmerMode!=NULL)&&
      (pGetSysSelPref->pModePref!=NULL))
    {
        /* emergency mode can be OFF (0) or ON(1), valid bit mask for mode preference is Bit0-Bit5 */
        if ((pGetSysSelPref->pEmerMode->EmerMode > 1) ||
           CHECK_BIT(pGetSysSelPref->pModePref->ModePref, 6))
        {
            is_matching = 0;
        }
    }
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((is_matching ==1) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
    }
#endif

}

void dump_SLQSNASGeteDRXParams (void *ptr)
{
    unpack_nas_SLQSNASGeteDRXParams_t *result =
    (unpack_nas_SLQSNASGeteDRXParams_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCycleLen) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("CycleLen               : %x\n", *result->pCycleLen);
        DefaulteDRXParams.pCycleLen         = result->pCycleLen;
    }
    if((result->pPagingTimeWindow) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("PagingTimeWindow       : %x\n", *result->pPagingTimeWindow);
        DefaulteDRXParams.pPagingTimeWindow = result->pPagingTimeWindow;
    }
    if((result->pEdrxEnable) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf("EdrxEnable             : %x\n", *result->pEdrxEnable);
        DefaulteDRXParams.pEdrxEnable       = result->pEdrxEnable;
    }

    if(iLocalLog==0)
        return ;
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNASGeteDRXParamsExt (void *ptr)
{
    unpack_nas_SLQSNASGeteDRXParamsExt_t *result =
    (unpack_nas_SLQSNASGeteDRXParamsExt_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if((result->pCycleLen) && (swi_uint256_get_bit (result->ParamPresenceMask, 16)) )
    {
        printf("CycleLen               : %x\n", *result->pCycleLen);
        DefaulteDRXParams.pCycleLen         = result->pCycleLen;
    }
    if((result->pPagingTimeWindow) && (swi_uint256_get_bit (result->ParamPresenceMask, 17)) )
    {
        printf("PagingTimeWindow       : %x\n", *result->pPagingTimeWindow);
        DefaulteDRXParams.pPagingTimeWindow = result->pPagingTimeWindow;
    }
    if((result->pEdrxEnable) && (swi_uint256_get_bit (result->ParamPresenceMask, 18)) )
    {
        printf("EdrxEnable             : %x\n", *result->pEdrxEnable);
        DefaulteDRXParams.pEdrxEnable       = result->pEdrxEnable;
    }
    if((result->pEdrxRAT) && (swi_uint256_get_bit (result->ParamPresenceMask, 19)) )
    {
        printf("EdrxRAT             : %x\n", *result->pEdrxRAT);
        DefaulteDRXParams.pEdrxRatType       = result->pEdrxRAT;
    }
    if((result->pLteOpMode) && (swi_uint256_get_bit (result->ParamPresenceMask, 20)) )
    {
        printf("LTE Operating Mode   : %08x\n", *result->pLteOpMode);
        DefaulteDRXParams.pEdrxCiotLteMode       = result->pLteOpMode;
    }

    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSNasGetRFInfo (void *ptr)
{
    unpack_nas_SLQSNasGetRFInfo_t *result =
    (unpack_nas_SLQSNasGetRFInfo_t*) ptr;
    uint8_t  count = 0;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("LTE Operational Mode is optional TLV. "
               "It might not be present for all LTE Modes\n");
    printf("RF Band Information List\n");
    printf("\tInstance Size  : %d\n", *result->rfbandInfoList.pInstanceSize);
    for(count = 0; count < *result->rfbandInfoList.pInstanceSize; count++)
    {
        printf("\tradioInterface[%d] : %d\n",
                count, result->rfbandInfoList.pRFBandInfoParam[count].radioInterface);
        printf("\tactiveBandClass[%d] : %d\n",
                count, result->rfbandInfoList.pRFBandInfoParam[count].activeBandClass);
        printf("\tactiveChannel[%d] : %d\n",
                count, result->rfbandInfoList.pRFBandInfoParam[count].activeChannel);
    }
    printf("\n");

    if( (result->pRfDedicatedBandInfo!=NULL) &&
        (result->pRfDedicatedBandInfo->TlvPresent))
    {
        printf("RF Dedicated Band Information List\n");
        printf("\tInstance Size  : %d\n", *result->pRfDedicatedBandInfo->pInstancesSize);
        for(count = 0; count < *result->pRfDedicatedBandInfo->pInstancesSize; count++)
        {
            printf("\tradioInterface[%d] : %d\n",
                    count, result->pRfDedicatedBandInfo->pRfDedicatedBandInfoParam[count].radioInterface);
            printf("\tdedicatedBand[%d] : %d\n",
                    count, result->pRfDedicatedBandInfo->pRfDedicatedBandInfoParam[count].dedicatedBand);
        }
        printf("\n");
    }

    if( (result->pRfBandInfoExtFormat != NULL) && 
        (result->pRfBandInfoExtFormat->TlvPresent) )
    {
        printf("RF Band Information List, Extended Format\n");
        printf("\tInstance Size  : %d\n", *result->pRfBandInfoExtFormat->pInstancesSize);
        for(count = 0; count < *result->pRfBandInfoExtFormat->pInstancesSize; count++)
        {
            printf("\tradioInterface[%d] : %d\n",
                    count, result->pRfBandInfoExtFormat->pRfBandInfoExtFormatParam[count].radioInterface);
            printf("\tactiveBandClass[%d] : %d\n",
                    count, result->pRfBandInfoExtFormat->pRfBandInfoExtFormatParam[count].activeBand);
            printf("\tactiveChannel[%d] : %d\n",
                    count, result->pRfBandInfoExtFormat->pRfBandInfoExtFormatParam[count].activeChannel);
        }
        printf("\n");
    }

    if( (result->pRfBandwidthInfo != NULL) && 
        (result->pRfBandwidthInfo->TlvPresent) )
    {
        printf("RF Bandwidth Information List\n");
        printf("\tInstance Size  : %d\n", *result->pRfBandwidthInfo->pInstancesSize);
        for(count = 0; count < *result->pRfBandwidthInfo->pInstancesSize; count++)
        {
            printf("\tradioInterface[%d] : %d\n",
                    count, result->pRfBandwidthInfo->pRfBandwidthInfoParam[count].radioInterface);
            printf("\tbandwidth[%d] : %d\n",
                    count, result->pRfBandwidthInfo->pRfBandwidthInfoParam[count].bandwidth);
         }
        printf("\n");
    }

    if( (result->pLTEOperationMode!=NULL) &&
        ( result->pLTEOperationMode->TlvPresent) )
    {
        printf("LTE Operational Mode  : %d\n", *result->pLTEOperationMode->pLTEOperationMode);
    }

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

void dump_SLQSNASGetForbiddenNetworks (void *ptr)
{
    unpack_nas_SLQSNASGetForbiddenNetworks_t *pForbiddenNWs =
        (unpack_nas_SLQSNASGetForbiddenNetworks_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pForbiddenNWs->ParamPresenceMask);

    if((NULL != pForbiddenNWs->pForbiddenNetworks3GPP) && (swi_uint256_get_bit (pForbiddenNWs->ParamPresenceMask, 16)))
    {
        uint8_t count = 0;
        for(count=0; count<pForbiddenNWs->pForbiddenNetworks3GPP->forbiddenNwInstLen; count++)
        {
            printf( "\tMCC[%d] : %d\n",count,pForbiddenNWs->pForbiddenNetworks3GPP->MCC[count] );
            printf( "\tMNC[%d] : %d\n",count,pForbiddenNWs->pForbiddenNetworks3GPP->MNC[count] );
        }
    }

    if(iLocalLog==0)
        return ;
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
}

void dump_SLQSNasRFBandInfoCallback_Ind (void *ptr)
{
    unpack_nas_SLQSNasRFBandInfoCallback_Ind_t *pInfo =
        (unpack_nas_SLQSNasRFBandInfoCallback_Ind_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pInfo->ParamPresenceMask);

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
    {
        printf( "RF Band Info\n");
        printf( "\tRadio interface in use : 0x%02x\n",pInfo->rfBandInfo.radioIf );
        printf( "\tActive band class      : 0x%04x\n",pInfo->rfBandInfo.activeBand );
        printf( "\tActive channel         : 0x%04x\n",pInfo->rfBandInfo.activeChannel );
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
    {
        printf( "Dedicated Band Info\n");
        printf( "\tRadio interface        : 0x%02x\n",pInfo->rfDedicatedBandInfo.radioIf );
        printf( "\tDedicated Band         : 0x%04x\n",pInfo->rfDedicatedBandInfo.dedicatedBand );
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))
    {
        printf( "Band Info Ext\n");
        printf( "\tRadio interface        : 0x%02x\n",pInfo->rfBandInfoExt.radioIf );
        printf( "\tActive band class      : 0x%04x\n",pInfo->rfBandInfoExt.activeBand );
        printf( "\tActive channel         : 0x%08x\n",pInfo->rfBandInfoExt.activeChannel );
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 18))
    {
        printf( "Bandwidth Info\n");
        printf( "\tRadio interface        : 0x%02x\n",pInfo->rfBandwidthInfo.radioIf );
        printf( "\tBandwidth              : 0x%08x\n",pInfo->rfBandwidthInfo.bandwidth );
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 19))
    {
        printf( "LTE Op Mode\n");
        printf( "\tCIOT LTE op Mode       : 0x%08x\n",pInfo->lteOperationalMode.ciotLteOpMode );
    }
    if(iLocalLog==0)
        return ;

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

testitem_t oemapitestnas[] = {
    {
        (pack_func_item) &pack_nas_GetHomeNetwork, "pack_nas_GetHomeNetwork",
        NULL,
        (unpack_func_item) &unpack_nas_GetHomeNetwork, "unpack_nas_GetHomeNetwork",
        &homenw, dump_GetHomeNetwork
    },
    {
        (pack_func_item) &pack_nas_SLQSGetHomeNetwork, "pack_nas_SLQSGetHomeNetwork",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetHomeNetwork, "unpack_nas_GetHomeNetwork",
        &slqshomenw, dump_SLQSGetHomeNetwork
    },
    {
        (pack_func_item) &pack_nas_GetNetworkPreference, "pack_nas_GetNetworkPreference",
        NULL,
        (unpack_func_item) &unpack_nas_GetNetworkPreference, "unpack_nas_GetNetworkPreference",
        &GetNetworkPreferenceResp, dump_GetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_GetRFInfo, "pack_nas_GetRFInfo",
        NULL,
        (unpack_func_item) &unpack_nas_GetRFInfo, "unpack_nas_GetRFInfo",
        &rfinfo, dump_GetRFInfo
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScan, "pack_nas_PerformNetworkScan",
        NULL,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_SLQSPerformNetworkScanV2, "pack_nas_SLQSPerformNetworkScanV2",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSPerformNetworkScanV2, "unpack_nas_SLQSPerformNetworkScanV2",
        &performNwScanv2, dump_SLQSPerformNetworkScanV2
    },
    {
        (pack_func_item) &pack_nas_SlqsGetLTECphyCAInfo, "pack_nas_SlqsGetLTECphyCAInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SlqsGetLTECphyCAInfo, "unpack_nas_SlqsGetLTECphyCAInfo",
        &SlqsGetLTECphyCAInfo, dump_SlqsGetLTECphyCAInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetServingSystem, "pack_nas_SLQSGetServingSystem",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetServingSystem, "unpack_nas_SLQSGetServingSystem",
        &getServingSystem, dump_SLQSGetServingSystem
    },
    {
        (pack_func_item) &pack_nas_SLQSGetServingSystemV2, "pack_nas_SLQSGetServingSystemV2",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetServingSystemV2, "unpack_nas_SLQSGetServingSystemV2",
        &getServingSystemv2, dump_SLQSGetServingSystemV2
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSignalStrength, "pack_nas_SLQSGetSignalStrength",
        &signalStrenghMask,
        (unpack_func_item) &unpack_nas_SLQSGetSignalStrength, "unpack_nas_SLQSGetSignalStrength",
        &getSignalStrength, dump_SLQSGetSignalStrength
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysInfo, "pack_nas_SLQSGetSysInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysInfo, "unpack_nas_SLQSGetSysInfo",
        &tunpack_nas_SLQSGetSysInfo, dump_SLQSGetSysInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysInfoV2, "pack_nas_SLQSGetSysInfoV2",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysInfoV2, "unpack_nas_SLQSGetSysInfoV2",
        &tunpack_nas_SLQSGetSysInfov2, dump_SLQSGetSysInfoV2
    },
//9 update default
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPrefExtV2, "pack_nas_SLQSGetSysSelectionPrefExtV2",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPrefExtV2, "unpack_nas_SLQSGetSysSelectionPrefExtV2",
        &sysSelPrefExtv2, dump_SLQSGetSysSelectionPrefExtV2
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPrefModemSettings
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &tpack_nas_SLQSSetSysSelectionPref[0],
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPref_EmerMode_ModePref
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &tpack_nas_SLQSSetSysSelectionPref[1],
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPref_Band_PrlPref
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &tpack_nas_SLQSSetSysSelectionPref[2],
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPref_Roam_LteBand
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &tpack_nas_SLQSSetSysSelectionPref[3],
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPref_NetSel_SrvDomain
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &ModemSetSysSelPref,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPrefRestoreSettings
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetCellLocationInfo, "pack_nas_SLQSNasGetCellLocationInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetCellLocationInfo, "unpack_nas_SLQSNasGetCellLocationInfo",
        &nasgetcelllocinfo, dump_SLQSNasGetCellLocationInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetCellLocationInfoV2, "pack_nas_SLQSNasGetCellLocationInfoV2",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetCellLocationInfoV2, "unpack_nas_SLQSNasGetCellLocationInfoV2",
        &nasgetcelllocinfov2, dump_SLQSNasGetCellLocationInfoV2
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetSigInfo, "pack_nas_SLQSNasGetSigInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetSigInfo, "unpack_nas_SLQSNasGetSigInfo",
        &siginfo, dump_SLQSNasGetSigInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasSwiIndicationRegister, "pack_nas_SLQSNasSwiIndicationRegister",
        &tpack_nas_SLQSNasSwiIndicationRegister,
        (unpack_func_item) &unpack_nas_SLQSNasSwiIndicationRegister, "unpack_nas_SLQSNasSwiIndicationRegister",
        &tunpack_nas_SLQSNasSwiIndicationRegister, dump_SLQSNasSwiIndicationRegister
    },
    {
        (pack_func_item) &pack_nas_SLQSNasIndicationRegisterExt, "pack_nas_SLQSNasIndicationRegisterExt",
        &nasIndicationRegisterExt,
        (unpack_func_item) &unpack_nas_SLQSNasIndicationRegisterExt, "unpack_nas_SLQSNasIndicationRegisterExt",
        &tunpack_nas_SLQSNasIndicationRegisterExt, dump_SLQSNasIndicationRegisterExt
    },
    {
        (pack_func_item) &pack_nas_SLQSNasSwiModemStatus, "pack_nas_SLQSNasSwiModemStatus",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasSwiModemStatus, "unpack_nas_SLQSNasSwiModemStatus",
        &swiGetModemStatus, dump_SLQSNasSwiModemStatus
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &setSysSelPref2,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },

    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteSccRxInfo, "pack_nas_SLQSSwiGetLteSccRxInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteSccRxInfo, "unpack_nas_SLQSSwiGetLteSccRxInfo",
        &nasgetsccrxinfo, dump_SLQSSwiGetLteSccRxInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteCQI, "pack_nas_SLQSSwiGetLteCQI",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteCQI, "unpack_nas_SLQSSwiGetLteCQI",
        &swiGetLTECQI, dump_SLQSSwiGetLteCQI
    },
    {
        (pack_func_item) &pack_nas_SLQSInitiateNetworkRegistration, "pack_nas_SLQSInitiateNetworkRegistration",
        &nasInitiateNetworkRegistration,
        (unpack_func_item) &unpack_nas_SLQSInitiateNetworkRegistration, "unpack_nas_SLQSInitiateNetworkRegistration",
        &tunpack_nas_SLQSInitiateNetworkRegistration, dump_SLQSInitiateNetworkRegistration
    },
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &SetNetworkPreferenceReq3GPPANDLTE,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        &SetNetworkPreferenceResp, dump_SetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScanPCI, "pack_nas_PerformNetworkScanPCI",
        &tpack_nas_PerformNetworkScanPCI,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScanPCI, "pack_nas_PerformNetworkScanPCI",
        &tpack_nas_PerformNetworkScanPCI2,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    //Restore from case 2
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &DefaultNetworkPreference,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        &SetNetworkPreferenceResp, dump_SetNetworkPreference
    },
#if 0
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &DefaultSysSelPref,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
#endif
};



testitem_t nastotest[] = {
    /////Get Modem settings///
    {
        (pack_func_item) &pack_nas_GetNetworkPreference, "pack_nas_GetNetworkPreference",
        NULL,
        (unpack_func_item) &unpack_nas_GetNetworkPreference, "unpack_nas_GetNetworkPreference",
        &GetNetworkPreferenceResp, dump_GetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPrefSettings
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPrefExt, "pack_nas_SLQSGetSysSelectionPrefExt",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPrefExt, "unpack_nas_SLQSGetSysSelectionPrefExt",
        &sysSelPrefExt, dump_SLQSGetSysSelectionPrefExtSettings
    },
    {
        (pack_func_item) &pack_nas_GetACCOLC, "pack_nas_GetACCOLC",
        NULL,
        (unpack_func_item) &unpack_nas_GetACCOLC, "unpack_nas_GetACCOLC",
        &getACCOLC, dump_GetACCOLCSettings
    },
    {
        (pack_func_item) &pack_nas_GetCDMANetworkParameters, "pack_nas_GetCDMANetworkParameters",
        NULL,
        (unpack_func_item) &unpack_nas_GetCDMANetworkParameters, "unpack_nas_GetCDMANetworkParameters",
        &getCDMANetworkParameters, dump_GetCDMANetworkParametersSettings
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiGetChannelLock, "pack_nas_SLQSNASSwiGetChannelLock",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASSwiGetChannelLock, "unpack_nas_SLQSNASSwiGetChannelLock",
        &tunpack_nas_SLQSNASSwiGetChannelLock_t, dump_SLQSNASSwiGetChannelLockSettings
    },
    ////////////////////////////////
    {
        (pack_func_item) &pack_nas_SLQSGetPLMNName, "pack_nas_SLQSGetPLMNName",
        &tpack_nas_SLQSGetPLMNName_t,
        (unpack_func_item) &unpack_nas_SLQSGetPLMNName, "unpack_nas_SLQSGetPLMNName",
        &tunpack_nas_SLQSGetPLMNName_t, dump_SLQSGetPLMNName
    },
    {
        (pack_func_item) &pack_nas_GetNetworkPreference, "pack_nas_GetNetworkPreference",
        NULL,
        (unpack_func_item) &unpack_nas_GetNetworkPreference, "unpack_nas_GetNetworkPreference",
        &GetNetworkPreferenceResp, dump_GetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &SetNetworkPreferenceReq,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        &SetNetworkPreferenceResp, dump_SetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSNasIndicationRegisterExt, "pack_nas_SLQSNasIndicationRegisterExt",
        &nasIndicationRegisterExt,
        (unpack_func_item) &unpack_nas_SLQSNasIndicationRegisterExt, "unpack_nas_SLQSNasIndicationRegisterExt",
        &tunpack_nas_SLQSNasIndicationRegisterExt, dump_SLQSNasIndicationRegisterExt
    },
    {
        (pack_func_item) &pack_nas_SLQSNasSwiIndicationRegister, "pack_nas_SLQSNasSwiIndicationRegister",
        &tpack_nas_SLQSNasSwiIndicationRegister,
        (unpack_func_item) &unpack_nas_SLQSNasSwiIndicationRegister, "unpack_nas_SLQSNasSwiIndicationRegister",
        &tunpack_nas_SLQSNasSwiIndicationRegister, dump_SLQSNasSwiIndicationRegister
    },
    {
        (pack_func_item) &pack_nas_SLQSNasIndicationRegisterV2, "pack_nas_SLQSNasIndicationRegisterV2",
        &nasIndicationRegisterV2,
        (unpack_func_item) &unpack_nas_SLQSNasIndicationRegisterV2, "unpack_nas_SLQSNasIndicationRegisterV2",
        &tunpack_nas_SLQSNasIndicationRegisterV2, dump_SLQSNasIndicationRegisterV2
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSignalStrengthsCallback, "pack_nas_SLQSSetSignalStrengthsCallback",
        &setSigStrengthCallback,
        (unpack_func_item) &unpack_nas_SLQSSetSignalStrengthsCallback, "unpack_nas_SLQSSetSignalStrengthsCallback",
        &tunpack_nas_SLQSSetSignalStrengthsCallback, dump_SLQSSetSignalStrengthsCallback
    },
    {
        (pack_func_item) &pack_nas_SetRFInfoCallback, "pack_nas_SetRFInfoCallback",
        &setRFInfoCallback,
        (unpack_func_item) &unpack_nas_SetRFInfoCallback, "unpack_nas_SetRFInfoCallback",
        &tunpack_nas_SetRFInfoCallback, dump_SetRFInfoCallback
    },
    {
        (pack_func_item) &pack_nas_SetLURejectCallback, "pack_nas_SetLURejectCallback",
        &setRFInfoCallback,
        (unpack_func_item) &unpack_nas_SetLURejectCallback, "unpack_nas_SetLURejectCallback",
        &tunpack_nas_SetLURejectCallback, dump_SetLURejectCallback
    },
    {
        (pack_func_item) &pack_nas_GetHomeNetwork, "pack_nas_GetHomeNetwork",
        NULL,
        (unpack_func_item) &unpack_nas_GetHomeNetwork, "unpack_nas_GetHomeNetwork",
        &homenw, dump_GetHomeNetwork
    },
    {
        (pack_func_item) &pack_nas_GetServingNetwork, "pack_nas_GetServingNetwork",
        NULL,
        (unpack_func_item) &unpack_nas_GetServingNetwork, "unpack_nas_GetServingNetwork",
        &getServingNW, dump_GetServingNetwork
    },
    {
        (pack_func_item) &pack_nas_GetServingNetworkCapabilities, "pack_nas_GetServingNetworkCapabilities",
        NULL,
        (unpack_func_item) &unpack_nas_GetServingNetworkCapabilities, "unpack_nas_GetServingNetworkCapabilities",
        &getServingNwCap, dump_GetServingNetworkCapabilities
    },

    {
        (pack_func_item) &pack_nas_GetRFInfo, "pack_nas_GetRFInfo",
        NULL,
        (unpack_func_item) &unpack_nas_GetRFInfo, "unpack_nas_GetRFInfo",
        &rfinfo, dump_GetRFInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetRFInfo, "pack_nas_SLQSNasGetRFInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetRFInfo, "unpack_nas_SLQSNasGetRFInfo",
        &tunpack_nas_SLQSNasGetRFInfo, dump_SLQSNasGetRFInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetSigInfo, "pack_nas_SLQSNasGetSigInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetSigInfo, "unpack_nas_SLQSNasGetSigInfo",
        &siginfo, dump_SLQSNasGetSigInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &setSysSelPref,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPrefExt, "pack_nas_SLQSSetSysSelectionPrefExt",
        &setSysSelPrefExt,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPrefExt, "unpack_nas_SLQSSetSysSelectionPrefExt",
        &tunpack_nas_SLQSSetSysSelectionPrefExt, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPrefExt, "pack_nas_SLQSSetSysSelectionPrefExt",
        &setLTEBandprefExt,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPrefExt, "unpack_nas_SLQSSetSysSelectionPrefExt",
        &tunpack_nas_SLQSSetSysSelectionPrefExt, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        &sysSelPref, dump_SLQSGetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScan, "pack_nas_PerformNetworkScan",
        NULL,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteCQI, "pack_nas_SLQSSwiGetLteCQI",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteCQI, "unpack_nas_SLQSSwiGetLteCQI",
        &swiGetLTECQI, dump_SLQSSwiGetLteCQI
    },
    {
        (pack_func_item) &pack_nas_SLQSNasSwiModemStatus, "pack_nas_SLQSNasSwiModemStatus",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasSwiModemStatus, "unpack_nas_SLQSNasSwiModemStatus",
        &swiGetModemStatus, dump_SLQSNasSwiModemStatus
    },
    {
        (pack_func_item) &pack_nas_SLQSGetServingSystem, "pack_nas_SLQSGetServingSystem",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetServingSystem, "unpack_nas_SLQSGetServingSystem",
        &getServingSystem, dump_SLQSGetServingSystem
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSignalStrength, "pack_nas_SLQSGetSignalStrength",
        &signalStrenghMask,
        (unpack_func_item) &unpack_nas_SLQSGetSignalStrength, "unpack_nas_SLQSGetSignalStrength",
        &getSignalStrength, dump_SLQSGetSignalStrength
    },
    {
        (pack_func_item) &pack_nas_GetCDMANetworkParameters, "pack_nas_GetCDMANetworkParameters",
        NULL,
        (unpack_func_item) &unpack_nas_GetCDMANetworkParameters, "unpack_nas_GetCDMANetworkParameters",
        &getCDMANetworkParameters, dump_GetCDMANetworkParameters
    },
    {
        (pack_func_item) &pack_nas_GetANAAAAuthenticationStatus, "pack_nas_GetANAAAAuthenticationStatus",
        NULL,
        (unpack_func_item) &unpack_nas_GetANAAAAuthenticationStatus, "unpack_nas_GetANAAAAuthenticationStatus",
        &getANAAAStatus, dump_GetANAAAAuthenticationStatus
    },
    {
        (pack_func_item) &pack_nas_SetACCOLC, "pack_nas_SetACCOLC",
        &setACCOLC,
        (unpack_func_item) &unpack_nas_SetACCOLC, "unpack_nas_SetACCOLC",
        &tunpack_nas_SetACCOLC, dump_SetACCOLC
    },
    {
        (pack_func_item) &pack_nas_GetACCOLC, "pack_nas_GetACCOLC",
        NULL,
        (unpack_func_item) &unpack_nas_GetACCOLC, "unpack_nas_GetACCOLC",
        &getACCOLC, dump_GetACCOLC
    },
    {
        (pack_func_item) &pack_nas_SLQSNasConfigSigInfo2, "pack_nas_SLQSNasConfigSigInfo2",
        &nasConfigSigInfo2,
        (unpack_func_item) &unpack_nas_SLQSNasConfigSigInfo2, "unpack_nas_SLQSNasConfigSigInfo2",
        &tunpack_nas_SLQSNasConfigSigInfo2, dump_SLQSNasConfigSigInfo2
    },
    {
        (pack_func_item) &pack_nas_SlqsGetLTECphyCAInfo, "pack_nas_SlqsGetLTECphyCAInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SlqsGetLTECphyCAInfo, "unpack_nas_SlqsGetLTECphyCAInfo",
        &SlqsGetLTECphyCAInfo, dump_SlqsGetLTECphyCAInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSInitiateNetworkRegistration, "pack_nas_SLQSInitiateNetworkRegistration",
        &nasInitiateNetworkRegistration,
        (unpack_func_item) &unpack_nas_SLQSInitiateNetworkRegistration, "unpack_nas_SLQSInitiateNetworkRegistration",
        &tunpack_nas_SLQSInitiateNetworkRegistration, dump_SLQSInitiateNetworkRegistration
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetCellLocationInfo, "pack_nas_SLQSNasGetCellLocationInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetCellLocationInfo, "unpack_nas_SLQSNasGetCellLocationInfo",
        &nasgetcelllocinfo, dump_SLQSNasGetCellLocationInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetNetworkTime, "pack_nas_SLQSGetNetworkTime",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetNetworkTime, "unpack_nas_SLQSGetNetworkTime",
        &nasgetnetworktimeinfo, dump_SLQSGetNetworkTime
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteSccRxInfo, "pack_nas_SLQSSwiGetLteSccRxInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteSccRxInfo, "unpack_nas_SLQSSwiGetLteSccRxInfo",
        &nasgetsccrxinfo, dump_SLQSSwiGetLteSccRxInfo
    },
    {
        (pack_func_item) &pack_nas_InitiateDomainAttach, "pack_nas_InitiateDomainAttach",
        &InitiateDomainAttach_req,
        (unpack_func_item) &unpack_nas_InitiateDomainAttach, "unpack_nas_InitiateDomainAttach",
        &tunpack_nas_InitiateDomainAttach, dump_InitiateDomainAttach
    },
    {
        (pack_func_item) &pack_nas_SetCDMANetworkParameters, "pack_nas_SetCDMANetworkParameters",
        &setCDMANetworkParameters,
        (unpack_func_item) &unpack_nas_SetCDMANetworkParameters, "unpack_nas_SetCDMANetworkParameters",
        &tunpack_nas_SetCDMANetworkParameters, dump_SetCDMANetworkParameters
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetHDRColorCode, "pack_nas_SLQSNasGetHDRColorCode",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetHDRColorCode, "unpack_nas_SLQSNasGetHDRColorCode",
        &tunpack_nas_SLQSNasGetHDRColorCode_t, dump_SLQSNasGetHDRColorCode
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetTxRxInfo, "pack_nas_SLQSNasGetTxRxInfo",
        &tpack_nas_SLQSNasGetTxRxInfo_t,
        (unpack_func_item) &unpack_nas_SLQSNasGetTxRxInfo, "unpack_nas_SLQSNasGetTxRxInfo",
        &tunpack_nas_SLQSNasGetTxRxInfo_t, dump_SLQSNasGetTxRxInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetOperatorNameData, "pack_nas_SLQSGetOperatorNameData",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetOperatorNameData, "unpack_nas_SLQSGetOperatorNameData",
        &tunpack_nas_SLQSGetOperatorNameData_t, dump_SLQSGetOperatorNameData
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGet3GPP2Subscription, "pack_nas_SLQSNasGet3GPP2Subscription",
        &tpack_nas_SLQSNasGet3GPP2Subscription_t,
        (unpack_func_item) &unpack_nas_SLQSNasGet3GPP2Subscription, "unpack_nas_SLQSNasGet3GPP2Subscription",
        &tunpack_nas_SLQSNasGet3GPP2Subscription_t, dump_SLQSNasGet3GPP2Subscription
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHDRPersonality, "pack_nas_SLQSSwiGetHDRPersonality",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHDRPersonality, "unpack_nas_SLQSSwiGetHDRPersonality",
        &tunpack_nas_SLQSSwiGetHDRPersonality_t, dump_SLQSSwiGetHDRPersonality
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHDRProtSubtype, "pack_nas_SLQSSwiGetHDRProtSubtype",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHDRProtSubtype, "unpack_nas_SLQSSwiGetHDRProtSubtype",
        &tunpack_nas_SLQSSwiGetHDRProtSubtype_t, dump_SLQSSwiGetHDRProtSubtype
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiPSDetach, "pack_nas_SLQSSwiPSDetach",
        &tpack_nas_SLQSSwiPSDetach_t,
        (unpack_func_item) &unpack_nas_SLQSSwiPSDetach, "unpack_nas_SLQSSwiPSDetach",
        &tunpack_nas_SLQSSwiPSDetach, dump_SLQSSwiPSDetach
    },
    {
        (pack_func_item) &pack_nas_SLQSGetErrorRate, "pack_nas_SLQSGetErrorRate",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetErrorRate, "unpack_nas_SLQSGetErrorRate",
        &tunpack_nas_SLQSGetErrorRate_t, dump_SLQSGetErrorRate
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHRPDStats, "pack_nas_SLQSSwiGetHRPDStats",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHRPDStats, "unpack_nas_SLQSSwiGetHRPDStats",
        &tunpack_nas_SLQSSwiGetHRPDStats_t, dump_SLQSSwiGetHRPDStats
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiNetworkDebug, "pack_nas_SLQSSwiNetworkDebug",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiNetworkDebug, "unpack_nas_SLQSSwiNetworkDebug",
        &tunpack_nas_SLQSSwiNetworkDebug_t, dump_SLQSSwiNetworkDebug
    },
    {
        (pack_func_item) &pack_nas_SLQSConfigSigInfo, "pack_nas_SLQSConfigSigInfo",
        &tpack_nas_SLQSConfigSigInfo_t,
        (unpack_func_item) &unpack_nas_SLQSConfigSigInfo, "unpack_nas_SLQSConfigSigInfo",
        &tunpack_nas_SLQSConfigSigInfo, dump_SLQSConfigSigInfo
    },
    {
        (pack_func_item) &pack_nas_GetHomeNetwork3GPP2, "pack_nas_GetHomeNetwork3GPP2",
        NULL,
        (unpack_func_item) &unpack_nas_GetHomeNetwork3GPP2, "unpack_nas_GetHomeNetwork3GPP2",
        &tunpack_nas_GetHomeNetwork3GPP2_t, dump_GetHomeNetwork3GPP2
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiSetChannelLock, "pack_nas_SLQSNASSwiSetChannelLock",
        &tpack_nas_SLQSNASSwiSetChannelLock_t,
        (unpack_func_item) &unpack_nas_SLQSNASSwiSetChannelLock, "unpack_nas_SLQSNASSwiSetChannelLock",
        &tunpack_nas_SLQSNASSwiSetChannelLock, dump_SLQSNASSwiSetChannelLock
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiGetChannelLock, "pack_nas_SLQSNASSwiGetChannelLock",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASSwiGetChannelLock, "unpack_nas_SLQSNASSwiGetChannelLock",
        &tunpack_nas_SLQSNASSwiGetChannelLock_t, dump_SLQSNASSwiGetChannelLock
    },
    {
        (pack_func_item) &pack_nas_GetSignalStrengths, "pack_nas_GetSignalStrengths",
        NULL,
        (unpack_func_item) &unpack_nas_GetSignalStrengths, "unpack_nas_GetSignalStrengths",
        &tunpack_nas_GetSignalStrengths, dump_GetSignalStrengths
    },
    {
        (pack_func_item) &pack_nas_SLQSSetBandPreference, "pack_nas_SLQSSetBandPreference",
        &tpack_nas_SLQSSetBandPreference,
        (unpack_func_item) &unpack_nas_SLQSSetBandPreference, "unpack_nas_SLQSSetBandPreference",
        &tunpack_nas_SLQSSetBandPreference, dump_SLQSSetBandPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysInfo, "pack_nas_SLQSGetSysInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysInfo, "unpack_nas_SLQSGetSysInfo",
        &tunpack_nas_SLQSGetSysInfo, dump_SLQSGetSysInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNASGeteDRXParams, "pack_nas_SLQSNASGeteDRXParams",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASGeteDRXParams, "unpack_nas_SLQSNASGeteDRXParams",
        &tunpack_nas_SLQSNASGeteDRXParams, dump_SLQSNASGeteDRXParams
    },
    {
        (pack_func_item) &pack_nas_SLQSNASGeteDRXParamsExt, "pack_nas_SLQSNASGeteDRXParamsExt",
        &tpack_nas_SLQSNASGeteDRXParamsExt,
        (unpack_func_item) &unpack_nas_SLQSNASGeteDRXParamsExt, "unpack_nas_SLQSNASGeteDRXParamsExt",
        &tunpack_nas_SLQSNASGeteDRXParamsExt, dump_SLQSNASGeteDRXParamsExt
    },
    ////Restore modem settings
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &DefaultNetworkPreference,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        &SetNetworkPreferenceResp, dump_SetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPref, "pack_nas_SLQSSetSysSelectionPref",
        &DefaultSysSelPref,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPref, "unpack_nas_SLQSSetSysSelectionPref",
        &tunpack_nas_SLQSSetSysSelectionPref, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SetACCOLC, "pack_nas_SetACCOLC",
        &DefaultACCOLC,
        (unpack_func_item) &unpack_nas_SetACCOLC, "unpack_nas_SetACCOLC",
        &tunpack_nas_SetACCOLC, dump_SetACCOLC
    },
    {
        (pack_func_item) &pack_nas_SetCDMANetworkParameters, "pack_nas_SetCDMANetworkParameters",
        &DefaultCDMANetworkParameters,
        (unpack_func_item) &unpack_nas_SetCDMANetworkParameters, "unpack_nas_SetCDMANetworkParameters",
        &tunpack_nas_SetCDMANetworkParameters, dump_SetCDMANetworkParameters
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiSetChannelLock, "pack_nas_SLQSNASSwiSetChannelLock",
        &DefaultNASSwiSetChannelLock,
        (unpack_func_item) &unpack_nas_SLQSNASSwiSetChannelLock, "unpack_nas_SLQSNASSwiSetChannelLock",
        &tunpack_nas_SLQSNASSwiSetChannelLock, dump_SLQSNASSwiSetChannelLock
    },
    {
        (pack_func_item) &pack_nas_SLQSSetBandPreference, "pack_nas_SLQSSetBandPreference",
        &DefaultBandPreference,
        (unpack_func_item) &unpack_nas_SLQSSetBandPreference, "unpack_nas_SLQSSetBandPreference",
        &tunpack_nas_SLQSSetBandPreference, dump_SLQSSetBandPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSeteDRXParams, "pack_nas_SLQSNASSeteDRXParams",
        &DefaulteDRXParams,
        (unpack_func_item) &unpack_nas_SLQSNASSeteDRXParams, "unpack_nas_SLQSNASSeteDRXParams",
        &tunpack_nas_SLQSNASSeteDRXParams, dump_SLQSNASSeteDRXParams
    },
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &SetNetworkPreferenceReq3GPPANDLTE,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        &SetNetworkPreferenceResp, dump_SetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScanPCI, "pack_nas_PerformNetworkScanPCI",
        &tpack_nas_PerformNetworkScanPCI,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScanPCI, "pack_nas_PerformNetworkScanPCI",
        &tpack_nas_PerformNetworkScanPCI2,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_SLQSNASGetForbiddenNetworks, "pack_nas_SLQSNASGetForbiddenNetworks",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASGetForbiddenNetworks, "unpack_nas_SLQSNASGetForbiddenNetworks",
        &tunpack_nas_SLQSNASGetForbiddenNetworks, dump_SLQSNASGetForbiddenNetworks
    },
    ///////////////////////////////////////////
};

unsigned int nasarraylen = (unsigned int)((sizeof(nastotest))/(sizeof(nastotest[0])));

testitem_t nastotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_nas_GetNetworkPreference, "pack_nas_GetNetworkPreference",
        NULL,
        (unpack_func_item) &unpack_nas_GetNetworkPreference, "unpack_nas_GetNetworkPreference",
        NULL, dump_GetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        NULL, dump_SLQSGetSysSelectionPrefSettings
    },
    {
        (pack_func_item) &pack_nas_GetACCOLC, "pack_nas_GetACCOLC",
        NULL,
        (unpack_func_item) &unpack_nas_GetACCOLC, "unpack_nas_GetACCOLC",
        NULL, dump_GetACCOLCSettings
    },
    {
        (pack_func_item) &pack_nas_GetCDMANetworkParameters, "pack_nas_GetCDMANetworkParameters",
        NULL,
        (unpack_func_item) &unpack_nas_GetCDMANetworkParameters, "unpack_nas_GetCDMANetworkParameters",
        NULL, dump_GetCDMANetworkParametersSettings
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiGetChannelLock, "pack_nas_SLQSNASSwiGetChannelLock",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASSwiGetChannelLock, "unpack_nas_SLQSNASSwiGetChannelLock",
        NULL, dump_SLQSNASSwiGetChannelLockSettings
    },
    ////////////////////////////////
    {
        (pack_func_item) &pack_nas_SLQSGetPLMNName, "pack_nas_SLQSGetPLMNName",
        &tpack_nas_SLQSGetPLMNName_t,
        (unpack_func_item) &unpack_nas_SLQSGetPLMNName, "unpack_nas_SLQSGetPLMNName",
        NULL, dump_SLQSGetPLMNName
    },
    {
        (pack_func_item) &pack_nas_GetNetworkPreference, "pack_nas_GetNetworkPreference",
        NULL,
        (unpack_func_item) &unpack_nas_GetNetworkPreference, "unpack_nas_GetNetworkPreference",
        NULL, dump_GetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_SetNetworkPreference, "pack_nas_SetNetworkPreference",
        &SetNetworkPreferenceReq,
        (unpack_func_item) &unpack_nas_SetNetworkPreference, "unpack_nas_SetNetworkPreference",
        NULL, dump_SetNetworkPreference
    },
    {
        (pack_func_item) &pack_nas_GetHomeNetwork, "pack_nas_GetHomeNetwork",
        NULL,
        (unpack_func_item) &unpack_nas_GetHomeNetwork, "unpack_nas_GetHomeNetwork",
        NULL, dump_GetHomeNetwork
    },
    {
        (pack_func_item) &pack_nas_GetServingNetwork, "pack_nas_GetServingNetwork",
        NULL,
       (unpack_func_item) &unpack_nas_GetServingNetwork, "unpack_nas_GetServingNetwork",
        NULL, dump_GetServingNetwork
    },
    {
        (pack_func_item) &pack_nas_GetServingNetworkCapabilities, "pack_nas_GetServingNetworkCapabilities",
        NULL,
        (unpack_func_item) &unpack_nas_GetServingNetworkCapabilities, "unpack_nas_GetServingNetworkCapabilities",
        NULL, dump_GetServingNetworkCapabilities
    },

    {
        (pack_func_item) &pack_nas_GetRFInfo, "pack_nas_GetRFInfo",
        NULL,
        (unpack_func_item) &unpack_nas_GetRFInfo, "unpack_nas_GetRFInfo",
        NULL, dump_GetRFInfo
   },
   {
        (pack_func_item) &pack_nas_SLQSNasGetSigInfo, "pack_nas_SLQSNasGetSigInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetSigInfo, "unpack_nas_SLQSNasGetSigInfo",
        NULL, dump_SLQSNasGetSigInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPref, "pack_nas_SLQSGetSysSelectionPref",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPref, "unpack_nas_SLQSGetSysSelectionPref",
        NULL, dump_SLQSGetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteCQI, "pack_nas_SLQSSwiGetLteCQI",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteCQI, "unpack_nas_SLQSSwiGetLteCQI",
        NULL, dump_SLQSSwiGetLteCQI
    },
    {
        (pack_func_item) &pack_nas_SLQSNasSwiModemStatus, "pack_nas_SLQSNasSwiModemStatus",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasSwiModemStatus, "unpack_nas_SLQSNasSwiModemStatus",
        NULL, dump_SLQSNasSwiModemStatus
    },
    {
        (pack_func_item) &pack_nas_SLQSGetServingSystem, "pack_nas_SLQSGetServingSystem",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetServingSystem, "unpack_nas_SLQSGetServingSystem",
        NULL, dump_SLQSGetServingSystem
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSignalStrength, "pack_nas_SLQSGetSignalStrength",
        &signalStrenghMask,
        (unpack_func_item) &unpack_nas_SLQSGetSignalStrength, "unpack_nas_SLQSGetSignalStrength",
        NULL, dump_SLQSGetSignalStrength
    },
    {
        (pack_func_item) &pack_nas_GetCDMANetworkParameters, "pack_nas_GetCDMANetworkParameters",
        NULL,
        (unpack_func_item) &unpack_nas_GetCDMANetworkParameters, "unpack_nas_GetCDMANetworkParameters",
        NULL, dump_GetCDMANetworkParameters
    },
    {
        (pack_func_item) &pack_nas_GetANAAAAuthenticationStatus, "pack_nas_GetANAAAAuthenticationStatus",
        NULL,
        (unpack_func_item) &unpack_nas_GetANAAAAuthenticationStatus, "unpack_nas_GetANAAAAuthenticationStatus",
        NULL, dump_GetANAAAAuthenticationStatus
    },
    {
        (pack_func_item) &pack_nas_GetACCOLC, "pack_nas_GetACCOLC",
        NULL,
        (unpack_func_item) &unpack_nas_GetACCOLC, "unpack_nas_GetACCOLC",
        NULL, dump_GetACCOLC
    },
    {
        (pack_func_item) &pack_nas_SlqsGetLTECphyCAInfo, "pack_nas_SlqsGetLTECphyCAInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SlqsGetLTECphyCAInfo, "unpack_nas_SlqsGetLTECphyCAInfo",
        NULL, dump_SlqsGetLTECphyCAInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetCellLocationInfo, "pack_nas_SLQSNasGetCellLocationInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetCellLocationInfo, "unpack_nas_SLQSNasGetCellLocationInfo",
        NULL, dump_SLQSNasGetCellLocationInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetNetworkTime, "pack_nas_SLQSGetNetworkTime",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetNetworkTime, "unpack_nas_SLQSGetNetworkTime",
        NULL, dump_SLQSGetNetworkTime
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetLteSccRxInfo, "pack_nas_SLQSSwiGetLteSccRxInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetLteSccRxInfo, "unpack_nas_SLQSSwiGetLteSccRxInfo",
        NULL, dump_SLQSSwiGetLteSccRxInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetHDRColorCode, "pack_nas_SLQSNasGetHDRColorCode",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetHDRColorCode, "unpack_nas_SLQSNasGetHDRColorCode",
        NULL, dump_SLQSNasGetHDRColorCode
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetTxRxInfo, "pack_nas_SLQSNasGetTxRxInfo",
        &tpack_nas_SLQSNasGetTxRxInfo_t,
        (unpack_func_item) &unpack_nas_SLQSNasGetTxRxInfo, "unpack_nas_SLQSNasGetTxRxInfo",
        NULL, dump_SLQSNasGetTxRxInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSGetOperatorNameData, "pack_nas_SLQSGetOperatorNameData",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetOperatorNameData, "unpack_nas_SLQSGetOperatorNameData",
        NULL, dump_SLQSGetOperatorNameData
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGet3GPP2Subscription, "pack_nas_SLQSNasGet3GPP2Subscription",
        &tpack_nas_SLQSNasGet3GPP2Subscription_t,
        (unpack_func_item) &unpack_nas_SLQSNasGet3GPP2Subscription, "unpack_nas_SLQSNasGet3GPP2Subscription",
        NULL, dump_SLQSNasGet3GPP2Subscription
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHDRPersonality, "pack_nas_SLQSSwiGetHDRPersonality",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHDRPersonality, "unpack_nas_SLQSSwiGetHDRPersonality",
        NULL, dump_SLQSSwiGetHDRPersonality
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHDRProtSubtype, "pack_nas_SLQSSwiGetHDRProtSubtype",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHDRProtSubtype, "unpack_nas_SLQSSwiGetHDRProtSubtype",
        NULL, dump_SLQSSwiGetHDRProtSubtype
    },
    {
        (pack_func_item) &pack_nas_SLQSGetErrorRate, "pack_nas_SLQSGetErrorRate",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetErrorRate, "unpack_nas_SLQSGetErrorRate",
        NULL, dump_SLQSGetErrorRate
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiGetHRPDStats, "pack_nas_SLQSSwiGetHRPDStats",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiGetHRPDStats, "unpack_nas_SLQSSwiGetHRPDStats",
        NULL, dump_SLQSSwiGetHRPDStats
    },
    {
        (pack_func_item) &pack_nas_SLQSSwiNetworkDebug, "pack_nas_SLQSSwiNetworkDebug",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSwiNetworkDebug, "unpack_nas_SLQSSwiNetworkDebug",
        NULL, dump_SLQSSwiNetworkDebug
    },
    {
        (pack_func_item) &pack_nas_GetHomeNetwork3GPP2, "pack_nas_GetHomeNetwork3GPP2",
        NULL,
        (unpack_func_item) &unpack_nas_GetHomeNetwork3GPP2, "unpack_nas_GetHomeNetwork3GPP2",
        NULL, dump_GetHomeNetwork3GPP2
    },
    {
        (pack_func_item) &pack_nas_SLQSNASSwiGetChannelLock, "pack_nas_SLQSNASSwiGetChannelLock",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASSwiGetChannelLock, "unpack_nas_SLQSNASSwiGetChannelLock",
        NULL, dump_SLQSNASSwiGetChannelLock
    },
    {
        (pack_func_item) &pack_nas_GetSignalStrengths, "pack_nas_GetSignalStrengths",
        NULL,
        (unpack_func_item) &unpack_nas_GetSignalStrengths, "unpack_nas_GetSignalStrengths",
        NULL, dump_GetSignalStrengths
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysInfo, "pack_nas_SLQSGetSysInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysInfo, "unpack_nas_SLQSGetSysInfo",
        NULL, dump_SLQSGetSysInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSNASGeteDRXParams, "pack_nas_SLQSNASGeteDRXParams",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASGeteDRXParams, "unpack_nas_SLQSNASGeteDRXParams",
        NULL, dump_SLQSNASGeteDRXParams
    },
    {
        (pack_func_item) &pack_nas_SLQSNasGetRFInfo, "pack_nas_SLQSNasGetRFInfo",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNasGetRFInfo, "unpack_nas_SLQSNasGetRFInfo",
        NULL, dump_SLQSNasGetRFInfo
    },
    {
        (pack_func_item) &pack_nas_SLQSSetSysSelectionPrefExt, "pack_nas_SLQSSetSysSelectionPrefExt NULL argument",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSSetSysSelectionPrefExt, "unpack_nas_SLQSSetSysSelectionPrefExt",
        NULL, dump_SLQSSetSysSelectionPref
    },
    {
        (pack_func_item) &pack_nas_SLQSGetSysSelectionPrefExt, "pack_nas_SLQSGetSysSelectionPrefExt",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSGetSysSelectionPrefExt, "unpack_nas_SLQSGetSysSelectionPrefExt",
        NULL, dump_SLQSGetSysSelectionPrefExtSettings
    },
    {
        (pack_func_item) &pack_nas_PerformNetworkScanPCI, "pack_nas_PerformNetworkScanPCI",
        NULL,
        (unpack_func_item) &unpack_nas_PerformNetworkScan, "unpack_nas_PerformNetworkScan",
        &performNwScan, dump_PerformNetworkScan
    },
    {
        (pack_func_item) &pack_nas_SLQSNASGetForbiddenNetworks, "pack_nas_SLQSNASGetForbiddenNetworks",
        NULL,
        (unpack_func_item) &unpack_nas_SLQSNASGetForbiddenNetworks, "unpack_nas_SLQSNASGetForbiddenNetworks",
        NULL, dump_SLQSNASGetForbiddenNetworks
    },
    ///////////////////////////////////////////

};

void nas_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    printf("======NAS pack/unpack test with invalid unpack params===========\n");
    unsigned xid =1;
    for(i=0; i<sizeof(nastotest)/sizeof(testitem_t); i++)
    {
        if(nastotest[i].dump!=NULL)
        {
            nastotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(oemapitestnas)/sizeof(testitem_t); i++)
    {
        if(oemapitestnas[i].dump!=NULL)
        {
            oemapitestnas[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(nastotest_invalidunpack)/sizeof(testitem_t); i++)
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
        rtn = run_pack_item(nastotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,nastotest_invalidunpack[i].pack_ptr);


        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(nas<0)
            nas = client_fd(eNAS);
        if(nas<0)
        {
            fprintf(stderr,"NAS Service Not Supported!\n");
            return ;
        }
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(nas>=0)
                close(nas);
            nas=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(nas, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(nastotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                nastotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           nastotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    nastotest_invalidunpack[i].dump(nastotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(nas>=0)
                close(nas);
            nas=-1;
        }
        sleep(1);
        xid++;
    }
    if(nas>=0)
        close(nas);
    nas=-1;
}

void displayNasSigInfo (unpack_nas_SLQSNasSigInfoCallback_ind_t *pInfo)
{
        printf( "NAS Signal Strength Information From SLQSNasSigInfoCallBack\n");
        RETURN_IF_PTR_IS_NULL(pInfo)
        if (( pInfo->pCDMASigInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 16)) )
        {
            printf( "RSSI(CDMA) : %d\n",pInfo->pCDMASigInfo->rssi );
            printf( "ECIO(CDMA) : %d\n",pInfo->pCDMASigInfo->ecio );
        }
        if (( pInfo->pHDRSigInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 17)) )
        {
            printf( "RSSI(HDR) : %d\n",pInfo->pHDRSigInfo->rssi );
            printf( "ECIO(HDR) : %d\n",pInfo->pHDRSigInfo->ecio );
            printf( "SINR(HDR) : %d\n",pInfo->pHDRSigInfo->sinr );
            printf( "IO(HDR)   : %d\n",pInfo->pHDRSigInfo->io );
        }
        if (( pInfo->pGSMSigInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 18)) )
        {
            printf( "GSM Signal Info(GSM)      : %d\n",*pInfo->pGSMSigInfo );
        }
        if (( pInfo->pWCDMASigInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 19)) )
        {
            printf( "RSSI(WCDMA)  : %d\n",pInfo->pWCDMASigInfo->rssi );
            printf( "ECIO(WCDMA)  : %d\n",pInfo->pWCDMASigInfo->ecio );
        }
        if (( pInfo->pLTESigInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 20)) )
        {
            printf( "RSSI(LTE)  : %d\n",pInfo->pLTESigInfo->rssi );
            printf( "RSRQ(LTE)  : %d\n",pInfo->pLTESigInfo->rsrq );
            printf( "RSRP(LTE)  : %d\n",pInfo->pLTESigInfo->rsrp );
            printf( "SNR(LTE)   : %d\n",pInfo->pLTESigInfo->snr );
        }
        if (( pInfo->pRscp) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 21)) )
        {
            printf( "RSCP(TDSCDMA)  : %d\n",*pInfo->pRscp );
        }
        if (( pInfo->pTDSCDMASigInfoExt ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 22)) )
        {
            printf( "RSSI(TDSCDMAEXT)  : %f\n",pInfo->pTDSCDMASigInfoExt->rssi );
            printf( "RSCP(TDSCDMAEXT)  : %f\n",pInfo->pTDSCDMASigInfoExt->rscp );
            printf( "ECIO(TDSCDMAEXT)  : %f\n",pInfo->pTDSCDMASigInfoExt->ecio );
            printf( "SINR(TDSCDMAEXT)  : %f\n",pInfo->pTDSCDMASigInfoExt->sinr );
        }
}

void displaySysInfoCommonCB( nas_sysInfoCommon *pSys )
{
    RETURN_IF_PTR_IS_NULL(pSys)
    printf( "Service Domain Valid   : %x\n",pSys->srvDomainValid );
    printf( "Service Domain         : %x\n",pSys->srvDomain );
    printf( "Service Capability Valid: %x\n",pSys->srvCapabilityValid );
    printf( "Service Capability     : %x\n",pSys->srvCapability );
    printf( "Roam Status Valid      : %x\n",pSys->roamStatusValid );
    printf( "Roam Status            : %x\n",pSys->roamStatus );
    printf( "Forbidden System Valid : %x\n",pSys->isSysForbiddenValid );
    printf( "Is Forbidden System    : %x\n",pSys->isSysForbiddenValid );
}

void displayNasSysInfo(unpack_nas_SLQSSysInfoCallback_ind_t *pInfo)
{
        uint8_t i;
        RETURN_IF_PTR_IS_NULL(pInfo)
        printf( "NAS System Information From SLQSNasSysInfoCallBack\n");
        if(pInfo!=NULL)
        {
            if (( pInfo->pCDMASrvStatusInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 16)) )
            {
                printf( "Service Status(CDMA) : %x\n",pInfo->pCDMASrvStatusInfo->srvStatus );
                printf( "Is Preferred(CDMA)   : %x\n",pInfo->pCDMASrvStatusInfo->isPrefDataPath );
            }
            if ( (pInfo->pHDRSrvStatusInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 17)) )
            {
                printf( "Service Status(HDR) : %x\n",pInfo->pHDRSrvStatusInfo->srvStatus );
                printf( "Is Preferred(HDR)   : %x\n",pInfo->pHDRSrvStatusInfo->isPrefDataPath );
            }
            if ( (pInfo->pGSMSrvStatusInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 18)) )
            {
                printf( "Service Status(GSM)      : %x\n",pInfo->pGSMSrvStatusInfo->srvStatus );
                printf( "True Service Status(GSM) : %x\n",pInfo->pGSMSrvStatusInfo->trueSrvStatus );
                printf( "Is Preferred(GSM)        : %x\n",pInfo->pGSMSrvStatusInfo->isPrefDataPath );
            }
            if ( (pInfo->pWCDMASrvStatusInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 19)) )
            {
                printf( "Service Status(WCDMA)     : %x\n",pInfo->pWCDMASrvStatusInfo->srvStatus );
                printf( "True Service Status(WCDMA): %x\n",pInfo->pWCDMASrvStatusInfo->trueSrvStatus );
                printf( "Is Preferred(WCDMA)       : %x\n",pInfo->pWCDMASrvStatusInfo->isPrefDataPath );
            }
            if ( (pInfo->pLTESrvStatusInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 20)) )
            {
                printf( "Service Status(LTE)      : %x\n",pInfo->pLTESrvStatusInfo->srvStatus );
                printf( "True Service Status(LTE) : %x\n",pInfo->pLTESrvStatusInfo->trueSrvStatus );
                printf( "Is Preferred(LTE)        : %x\n",pInfo->pLTESrvStatusInfo->isPrefDataPath );
            }
            if ( (pInfo->pCDMASysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 21)) )
            {
                printf( "\nCDMA SYSTEM INFORMATION ----\n");
                nas_CDMASysInfo *t = pInfo->pCDMASysInfo;
                displaySysInfoCommonCB( &t->sysInfoCDMA );
                printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
                printf( "System PRL          : %x\n",t->isSysPrlMatch );
                printf( "P_Rev Valid         : %x\n",t->pRevInUseValid );
                printf( "P_Rev In Use        : %x\n",t->pRevInUse );
                printf( "BS P_Rev Valid      : %x\n",t->bsPRevValid );
                printf( "P_Rev In Use        : %x\n",t->bsPRev );
                printf( "CCS_supp Valid      : %x\n",t->ccsSupportedValid );
                printf( "CCS_supp            : %x\n",t->ccsSupported );
                printf( "System Id           : %x\n",t->systemID );
                printf( "Network Id          : %x\n",t->networkID );
                printf( "BS Info Valid       : %x\n",t->bsInfoValid );
                printf( "Base ID             : %x\n",t->baseId );
                printf( "Base Latitude       : %x\n",t->baseLat );
                printf( "Base Longitude      : %x\n",t->baseLong );
                printf( "Packet Zone Valid   : %x\n",t->packetZoneValid );
                printf( "Packet Zone         : %x\n",t->packetZone );
                printf( "Network ID Valid    : %x\n",t->networkIdValid );
                printf( "MCC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MCC[i] );
                }
                printf( "\n" );
                printf( "MNC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MNC[i] );
                }
                printf( "\n" );
            }

            if ( (pInfo->pHDRSysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 22)) )
            {
                printf( "\nHDR SYSTEM INFORMATION ----\n");
                nas_HDRSysInfo *t = pInfo->pHDRSysInfo;
                displaySysInfoCommonCB( &t->sysInfoHDR );
                printf( "System PRL Valid    : %x\n",t->isSysPrlMatchValid );
                printf( "System PRL          : %x\n",t->isSysPrlMatch );
                printf( "Personality Valid   : %x\n",t->hdrPersonalityValid );
                printf( "Personality         : %x\n",t->hdrPersonality );
                printf( "Active Prot Valid   : %x\n",t->hdrActiveProtValid );
                printf( "Active Protocol     : %x\n",t->hdrActiveProt );
                printf( "IS-856 Sys Valid    : %x\n",t->is856SysIdValid );
                printf( "IS-856 system ID    : " );
                for ( i = 0 ; i < 16 ; i++ )
                {
                    printf( "%x ",t->is856SysId[i] );
                }
                printf( "\n" );
            }
            if ( (pInfo->pGSMSysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 23)) )
            {
                printf( "\nGSM SYSTEM INFORMATION ----\n");
                nas_GSMSysInfo *t = pInfo->pGSMSysInfo;
                displaySysInfoCommonCB( &t->sysInfoGSM );
                printf( "LAC Valid           : %x\n",t->lacValid );
                printf( "LAC                 : %x\n",t->lac );
                printf( "Cell ID Valid       : %x\n",t->cellIdValid );
                printf( "Cell ID             : %x\n",t->cellId );
                printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
                printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
                printf( "Reject Cause        : %x\n",t->rejCause );
                printf( "Network Id Valid    : %x\n",t->networkIdValid );
                printf( "MCC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MCC[i] );
                }
                printf( "\n" );
                printf( "MNC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MNC[i] );
                }
                printf( "\n" );
                printf( "EGPRS Support Valid : %x\n",t->egprsSuppValid );
                printf( "EGPRS Support       : %x\n",t->egprsSupp );
                printf( "DTM Support Valid   : %x\n",t->dtmSuppValid );
                printf( "DTM Support         : %x\n",t->dtmSupp );
            }
            if ( (pInfo->pWCDMASysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 24)) )
            {
                printf( "\nWCDMA SYSTEM INFORMATION ----\n");
                nas_WCDMASysInfo *t = pInfo->pWCDMASysInfo;
                displaySysInfoCommonCB( &t->sysInfoWCDMA );
                printf( "LAC Valid           : %x\n",t->lacValid );
                printf( "LAC                 : %x\n",t->lac );
                printf( "Cell ID Valid       : %x\n",t->cellIdValid );
                printf( "Cell ID             : %x\n",t->cellId );
                printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
                printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
                printf( "Reject Cause        : %x\n",t->rejCause );
                printf( "Network Id Valid    : %x\n",t->networkIdValid );
                printf( "MCC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MCC[i] );
                }
                printf( "\n" );
                printf( "MNC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MNC[i] );
                }
                printf( "\n" );
                printf( "HS Call Status Valid: %x\n",t->hsCallStatusValid );
                printf( "HS Call Status      : %x\n",t->hsCallStatus );
                printf( "HS Ind Valid        : %x\n",t->hsIndValid );
                printf( "HS Indication       : %x\n",t->hsInd );
                printf( "PSC Valid           : %x\n",t->pscValid );
                printf( "Primary Scrambling Code : %x\n",t->psc );
            }
            if ( (pInfo->pLTESysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 25)) )
            {
                printf( "\nLTE SYSTEM INFORMATION ----\n");
                nas_LTESysInfo *t = pInfo->pLTESysInfo;
                displaySysInfoCommonCB( &t->sysInfoLTE );
                printf( "LAC Valid           : %x\n",t->lacValid );
                printf( "LAC                 : %x\n",t->lac );
                printf( "Cell ID Valid       : %x\n",t->cellIdValid );
                printf( "Cell ID             : %x\n",t->cellId );
                printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
                printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
                printf( "Reject Cause        : %x\n",t->rejCause );
                printf( "Network Id Valid    : %x\n",t->networkIdValid );
                printf( "MCC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MCC[i] );
                }
                printf( "\n" );
                printf( "MNC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MNC[i] );
                }
                printf( "\n" );
                printf( "TAC Valid           : %x\n",t->tacValid );
                printf( "Tracking Area Code  : %x\n",t->tac );
            }
            if ( (pInfo->pAddCDMASysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 26)) )
            {
               printf( "Geo Sys Idx(CDMA) : %x\n", pInfo->pAddCDMASysInfo->geoSysIdx );
               printf( "Reg Period (CDMA) : %x\n", pInfo->pAddCDMASysInfo->regPrd );
            }
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 27))
                IFPRINTF( "Geo Sys Idx(HDR) : %x\n", pInfo->pAddHDRSysInfo );
            if ( (pInfo->pAddGSMSysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 28)) )
            {
                printf( "Geo Sys Idx(GSM) : %x\n", pInfo->pAddGSMSysInfo->geoSysIdx );
                printf( "Cell Br Cap(GSM) : %x\n", pInfo->pAddGSMSysInfo->cellBroadcastCap );
            }
            if ( (pInfo->pAddWCDMASysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 29)) )
            {
                printf( "Geo Sys Idx(WCDMA) : %x\n", pInfo->pAddWCDMASysInfo->geoSysIdx );
                printf( "Cell Br Cap(WCDMA) : %x\n", pInfo->pAddWCDMASysInfo->cellBroadcastCap );
            }
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 30))
                IFPRINTF( "Geo Sys Idx(LTE) : %x\n", pInfo->pAddLTESysInfo );
            if ( (pInfo->pGSMCallBarringSysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 31)) )
            {
                printf( "CS Bar Status(GSM): %x\n", pInfo->pGSMCallBarringSysInfo->csBarStatus );
                printf( "PS Bar Status(GSM): %x\n", pInfo->pGSMCallBarringSysInfo->psBarStatus );
            }
            if ( (pInfo->pWCDMACallBarringSysInfo ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 32)) )
            {
                printf( "CS Bar Status(WCDMA): %x\n", pInfo->pWCDMACallBarringSysInfo->csBarStatus );
                printf( "PS Bar Status(WCDMA): %x\n", pInfo->pWCDMACallBarringSysInfo->psBarStatus );
            }
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 33))
                IFPRINTF( "Voice Supp on LTE: %x\n", pInfo->pLTEVoiceSupportSysInfo );
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 34))
                IFPRINTF( "GSM Cipher Domain: %x\n", pInfo->pGSMCipherDomainSysInfo );
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 35))
                IFPRINTF( "WCDMA Cipher Domain: %x\n", pInfo->pWCDMACipherDomainSysInfo );
            if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 36))
                IFPRINTF( "System Info No Change Indication: %x\n", pInfo->pSysInfoNoChange );
            if ( (pInfo->pLteCiotOpModeTlv ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 75)) )
            {
                printf( "\nLTE CIOT OPMODE INFORMATION ----\n");
                nas_LteCiotOpModeTlv *t = pInfo->pLteCiotOpModeTlv;

                printf( "Camped LTE CIOT Operation Mode   : %x\n",t->campedCiotLteOpMode );
                printf( "Tlv Present  : %x\n",t->TlvPresent );
            }
            if ( (pInfo->pNR5GSerStatTlv ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 76)) )
            {
                printf( "\nNR5G SERVICE STATUS INFORMATION ----\n");
                nas_NR5GSerStatTlv *t = pInfo->pNR5GSerStatTlv;

                printf( "Service Status   : %x\n",t->srvStatus );
                printf( "True Service Status    : %x\n",t->trueSrvStatus );
                printf( "Is RAT Preferred Data Path: %x\n",t->isPrefDataPath );
                printf( "Tlv Present  : %x\n",t->TlvPresent );
            }
            if ( (pInfo->pNR5GSystemInfoTlv ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 77)) )
            {
                printf( "\nNR5G SYSTEM INFORMATION ----\n");
                nas_NR5GSystemInfoTlv *t = pInfo->pNR5GSystemInfoTlv;

                printf( "Service Domain Valid   : %x\n",t->srvDomainValid );
                printf( "Service Domain         : %x\n",t->srvDomain );
                printf( "Service Capability Valid: %x\n",t->srvCapValid );
                printf( "Service Capability     : %x\n",t->srvcapability );
                printf( "Roam Status Valid      : %x\n",t->roamStatusValid );
                printf( "Roam Status            : %x\n",t->roamStatus );
                printf( "Forbidden System Valid : %x\n",t->sysForbiddenValid );
                printf( "Is Forbidden System    : %x\n",t->sysForbidden );
                printf( "LAC Valid           : %x\n",t->lacValid );
                printf( "LAC                 : %x\n",t->lac );
                printf( "Cell ID Valid       : %x\n",t->cellIdValid );
                printf( "Cell ID             : %x\n",t->cellId );
                printf( "Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
                printf( "Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
                printf( "Reject Cause        : %x\n",t->rejCause );
                printf( "Network Id Valid    : %x\n",t->nwIdValid );
                printf( "MCC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MCC[i] );
                }
                printf( "\n" );
                printf( "MNC info            : " );
                for ( i = 0 ; i < PLMN_LENGTH ; i++ )
                {
                    printf( "%x ",t->MNC[i] );
                }
                printf( "\n" );
                printf( "TAC Valid           : %x\n",t->tacValid );
                printf( "Tracking Area Code  : %x\n",t->tac );
                printf( "Tlv Present  : %x\n",t->TlvPresent );
            }
            if ( (pInfo->pNR5GCellStatus ) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 78)) )
            {
                printf( "\nNR 5G CELL INFORMATION ----\n");
                nas_NR5GCellStatusTlv *t = pInfo->pNR5GCellStatus;

                printf( "NR 5G Cell Status   : %x\n",t->nr5gCellStatus );
                printf( "Tlv Present  : %x\n",t->TlvPresent );
            }
        }
}

void displayDataCap(unpack_nas_SetDataCapabilitiesCallback_ind_t *pDataCap)
{
    uint8_t i;
    RETURN_IF_PTR_IS_NULL(pDataCap)
    swi_uint256_print_mask (pDataCap->ParamPresenceMask);
    if(swi_uint256_get_bit (pDataCap->ParamPresenceMask, 17))
    {
        for(i=0;i<pDataCap->dataCapsSize;i++)
            printf("\t Data Capabilities[%d] %d\n", i, pDataCap->dataCaps[i]);
    }
}

void DisplayServingSystem(unpack_nas_SetServingSystemCallback_ind_t *pSSInfo)
{
    uint8_t i;
    RETURN_IF_PTR_IS_NULL(pSSInfo)
    swi_uint256_print_mask (pSSInfo->ParamPresenceMask);
    if(swi_uint256_get_bit (pSSInfo->ParamPresenceMask, 1))
    {
        printf("registrationState:%d\n",pSSInfo->SSInfo.registrationState);
        printf("csAttachState:%d\n",pSSInfo->SSInfo.csAttachState);
        printf("psAttachState:%d\n",pSSInfo->SSInfo.psAttachState) ;
        printf("selectedNetwork:%d\n",pSSInfo->SSInfo.selectedNetwork);
        printf("radioInterfaceNo:%d\n",pSSInfo->SSInfo.radioInterfaceNo );
        printf("radioInterface:\n");
        for(i=0; i < pSSInfo->SSInfo.radioInterfaceNo; i++)
        {
            printf("\t %d.%d\n",i,pSSInfo->SSInfo.radioInterfaceList[i]);
        }
        printf("hdrPersonality:%d\n",pSSInfo->SSInfo.hdrPersonality);
    }
}
void displayNasEventInfo(unpack_nas_SetEventReportInd_t *pEventInfo)
{
    uint8_t i;
    nas_SLQSSignalStrengthsTlv *pSigStrenInfo = NULL;
    nas_RFInfoTlv *pRFInfo = NULL;
    nas_RejectReasonTlv *pRRInfo = NULL;
    RETURN_IF_PTR_IS_NULL(pEventInfo)
    swi_uint256_print_mask (pEventInfo->ParamPresenceMask);
    pSigStrenInfo = &pEventInfo->SLQSSSTlv;
    if( CHECK_PTR_IS_NOT_NULL(pSigStrenInfo) &&
            (swi_uint256_get_bit (pEventInfo->ParamPresenceMask, 16)) )
    {
        nas_rxSignalStrengthListElement *pSSInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.rxSignalStrengthInfo;

        printf( "NAS Signal Strength Information From SLQSSetSignalStrengthsCallback\n");
        printf( "RSSI Information :\n" );
        printf( "Received Signal Strength : %d\n", pSSInfo->rxSignalStrength);
        printf( "Radio IF : %d\n\n", pSSInfo->radioIf);

        nas_ecioListElement *pEcioInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.ecioInfo;
        printf( "ECIO Information :\n" );
        printf( "ECIO     : %d\n", pEcioInfo->ecio);
        printf( "Radio IF : %d\n\n", pEcioInfo->radioIf);

        printf( "IO   : %u\n", pSigStrenInfo->sSLQSSignalStrengthsInfo.io);
        printf( "SINR : %d\n\n", pSigStrenInfo->sSLQSSignalStrengthsInfo.sinr );

        nas_errorRateListElement *pERInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.errorRateInfo;
        printf( "Error Rate Information :\n" );
        printf( "Error Rate : %d\n", pERInfo->errorRate );
        printf( "Radio IF   : %d\n\n", pERInfo->radioIf );

        nas_rsrqInformation *pRsrqInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.rsrqInfo;
        printf( "RSRQ Information :\n" );
        printf( "RSRQ     : %d\n", pRsrqInfo->rsrq );
        printf( "Radio IF : %d\n\n", pRsrqInfo->radioIf );

        nas_lteSnrinformation *pLteSnrInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.lteSnrinfo;
        printf( "LTE SNR LEVEL :%d dB\n\n", pLteSnrInfo->snrlevel/10 );

        nas_lteRsrpinformation *pLteRsrpInfo = &pSigStrenInfo->sSLQSSignalStrengthsInfo.lteRsrpinfo;
        printf( "LTE RSRP(-44 to -140) : %d dBm\n\n", pLteRsrpInfo->rsrplevel );
    }

    pRFInfo = &pEventInfo->RFTlv;
    if ( CHECK_PTR_IS_NOT_NULL(pRFInfo) &&
            (swi_uint256_get_bit (pEventInfo->ParamPresenceMask, 17)) )
    {
        printf("Radio Interface Size: %d\n", pRFInfo->radioInterfaceSize);
        for ( i = 0; i < pRFInfo->radioInterfaceSize; i++)
        {
            printf("Radio Interface[%d]: %u\n", i, pRFInfo->radioInterface[i]);
            printf("Active Band Class[%d]: %u\n", i, pRFInfo->activeBandClass[i] );
            printf("Active Channel[%d]: %u\n", i, pRFInfo->activeChannel[i] );
        }
    }

    pRRInfo = &pEventInfo->RRTlv;
    if ( CHECK_PTR_IS_NOT_NULL(pRRInfo) &&
            (swi_uint256_get_bit (pEventInfo->ParamPresenceMask, 18)) )
    {
        printf("Service Domain: %u\n", pRRInfo->serviceDomain);
        printf("Reject Cause: %u\n", pRRInfo->rejectCause);
    }
}

void displayNasSystemSelectPrefInfo(unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    uint8_t count = 0;
    swi_uint256_print_mask (pInfo->ParamPresenceMask);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
    {
        printf("Emergency Mode:%d\n",pInfo->Info.EMTlv.EmerMode);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))
    {
        printf("Mode Preference:%d\n",pInfo->Info.MPTlv.ModePref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 18))
    {
        printf("Band Preference:%"PRIu64"\n",pInfo->Info.BPTlv.band_pref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 19))
    {
        printf("PRL Preference:%d\n",pInfo->Info.PRLPTlv.PRLPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 20))
    {
        printf("Roam Preference:%d\n",pInfo->Info.RPTlv.RoamPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 21))
    {
        printf("LTE Band Preference:%"PRIu64"\n", pInfo->Info.LBPTlv.LTEBandPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 22))
    {
        printf("Net Selection Preference:%d\n",pInfo->Info.NSPTlv.NetSelPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 24))
    {
        printf("Service Domain Preference:%d\n",pInfo->Info.SDPTlv.SrvDomainPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 25))
    {
        printf("GSM/WCDMA acquisition order preference:%d\n", pInfo->Info.GWAOPTlv.GWAcqOrderPref);
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 28))
    {
        printf( "Acquisition order Preference Length: 0x%x\n",
                (pInfo->Info.AOPTlv.AcqOrderLen) );
        for(count=0; count < pInfo->Info.AOPTlv.AcqOrderLen; count++)
            printf( "Acquisition order Preference [%d] : 0x%x\n", count,
                    (pInfo->Info.AOPTlv.AcqOrderPref[count]) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 34))
    {
        printf( "RAT Disable Mask: 0x%04x\n",
                (pInfo->Info.RatDMTlv.RatDisabledMask) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 36))
    {
        printf( "CIOT LTE Operational Mode Preference: %"PRIu64"\n",
                (pInfo->Info.CiotOpMPTlv.CiotLteOpModePref) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 37))
    {
        printf( "LTE M1 Band Preference: %"PRIu64"\n",
                (pInfo->Info.M1BandPTlv.LteM1BandPref) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 38))
    {
        printf( "LTE NB1 Band Preference: %"PRIu64"\n",
                (pInfo->Info.NB1BandPTlv.LteNB1BandPref) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 39))
    {
        printf( "CIOT Acquisition order Preference Length: 0x%x\n",
                (pInfo->Info.CiotAOPTlv.CiotAcqOrderLen) );
        for(count=0; count < pInfo->Info.CiotAOPTlv.CiotAcqOrderLen; count++)
            printf( "CIOT Acquisition order Preference [%d] : 0x%x\n", count,
                    (pInfo->Info.CiotAOPTlv.CiotAcqOrderPref[count]) );
    }
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 40))
    {
        printf( "NR5G Band Preference Bits 1-64: %"PRIu64"\n",
                (pInfo->Info.NR5gBandPTlv.Nr5gBandPrefbits1_64) );
        printf("NR5G Band Preference Bits 65-128: %"PRIu64"\n",
                (pInfo->Info.NR5gBandPTlv.Nr5gBandPrefbits_65_128) );
        printf( "NR5G Band Preference Bits 129-192: %"PRIu64"\n",
                (pInfo->Info.NR5gBandPTlv.Nr5gBandPrefbits_129_192) );
        printf( "NR5G Band Preference Bits 193-256: %"PRIu64"\n",
                (pInfo->Info.NR5gBandPTlv.Nr5gBandPrefbits_193_256) );
    }
}

void displayNasSwiOTAMessageInfo(unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
    {
        printf("\nOTAMessage Major:%d",pInfo->Info.nasRelInfoTlv.nas_major);
        printf("\nOTAMessage Minor:%d",pInfo->Info.nasRelInfoTlv.nas_minor);
        printf("\nOTAMessage Release:%d",pInfo->Info.nasRelInfoTlv.nas_release); 
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
    {
        printf("\nOTAMessage Message Type:%d",pInfo->Info.otaMsgTlv.message_type);
        printf("\nOTAMessage Message Length:%d",pInfo->Info.otaMsgTlv.data_len);
        printf("\nOTAMessage Message :%s",(char*)pInfo->Info.otaMsgTlv.data_buf);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))
    {
         printf("\nOTAMessage Time :%"PRIu64"", pInfo->Info.timeTlv.time);
    }
    printf("\nSwiOTAMessageInfo\n");
}


void displayNasNetworkTime(unpack_nas_SLQSNasNetworkTimeCallBack_ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    printf( "NAS Network Time From SLQSNasNetworkTimeCallBack\n");
    swi_uint256_print_mask (pInfo->ParamPresenceMask);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
    {
        printf( "Universal Time \n");
        printf( "Year        : %d\n", pInfo->universalTime.year);
        printf( "Month       : %d\n", pInfo->universalTime.month);
        printf( "Day         : %d\n", pInfo->universalTime.day);
        printf( "Hours       : %d\n", pInfo->universalTime.hour);
        printf( "Minutes     : %d\n", pInfo->universalTime.minute);
        printf( "Seconds     : %d\n", pInfo->universalTime.second);
        printf( "Day of Week : %d\n", pInfo->universalTime.dayOfWeek);
    }
    if((pInfo->pTimeZone) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 16)) )
        printf( "Time Zone          : %d\n", *pInfo->pTimeZone );
    if((pInfo->pDayltSavAdj) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 17)) )
        printf( "Daylight Saving Adj: %d\n", *pInfo->pDayltSavAdj );
    if((pInfo->pRadioInterface) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 18)) )
        printf( "Radio Interface : %d\n", *pInfo->pRadioInterface );
}

void displayNasLTECphyCa(unpack_nas_SetNasLTECphyCaIndCallback_ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
    {
        printf("\nScell Indicator Type\n");
        printf("\tPhysical Cell ID %d\n",pInfo->sPhyCaAggScellIndType.pci);
        printf("\tFrequency %d\n",pInfo->sPhyCaAggScellIndType.freq);
        printf("\tScell State %d\n",pInfo->sPhyCaAggScellIndType.scell_state);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
    {
        printf("\nDownlink Bandwidth for Scell %d\n",pInfo->sPhyCaAggScellDlBw.dl_bw_value);
    }


    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))
    {
        printf("\nScell Information\n");
        printf("\tPhysical Cell ID %d\n",pInfo->sPhyCaAggScellInfo.pci);
        printf("\tFrequency %d\n",pInfo->sPhyCaAggScellInfo.freq);
        printf("\tDownlink Bandwidth %d\n",pInfo->sPhyCaAggScellInfo.dl_bw_value);
        printf("\tLTE Band %d\n",pInfo->sPhyCaAggScellInfo.iLTEbandValue);
        printf("\tScell State %d\n",pInfo->sPhyCaAggScellInfo.scell_state);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 18))
    {
        printf("\nPcell Information\n");
        printf("\tPhysical Cell ID %d\n",pInfo->sPhyCaAggPcellInfo.pci);
        printf("\tDownlink Bandwidth %d\n",pInfo->sPhyCaAggPcellInfo.dl_bw_value);
        printf("\tFrequency %d\n",pInfo->sPhyCaAggPcellInfo.freq);
        printf("\tLTE Band %d\n",pInfo->sPhyCaAggPcellInfo.iLTEbandValue);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 19))
    {
        printf("\nScell Index : %02X\n",pInfo->sPhyCaAggScellIndex.scell_idx);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 20))
    {
        printf("\nAggregated DL Bandwidth : %04X\n",pInfo->sPhyCaAggDlBW.aggDlBW);
    }

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 21))
    {
        printf("\nNumber of Scells Configured : %04X\n",pInfo->sNumScellsConfig.numScellsCfg);
    }
}

void displayNasTimer(unpack_nas_SLQSNasTimerCallback_ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
    {
        printf("\nNAS Network Timer\n");
        printf("\tapn: %s\n", pInfo->t3396_apn);
        printf("\tplmn id: %02x %02x %02x\n",
                pInfo->t3396_plmn_id[0],
                pInfo->t3396_plmn_id[1],
                pInfo->t3396_plmn_id[2]);
        printf("\ttimer value: %d\n", pInfo->t3396_val);
    }
}

void displayNasHdrPersonality(unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t *pData)
{
    uint8_t count;
    nas_HDRPersonality_Ind_Data *pInfo;
    RETURN_IF_PTR_IS_NULL(pData)

    pInfo = pData->pHDRPersInd;
    RETURN_IF_PTR_IS_NULL(pInfo)

    swi_uint256_print_mask (pData->ParamPresenceMask);
    if(( (NULL != pInfo->pPersonalityListLength) &&
        (NULL != pInfo->pProtocolSubtypeElement) ) && (swi_uint256_get_bit (pData->ParamPresenceMask, 17)))
    {
        for( count = 0; count < *(pInfo->pPersonalityListLength); count++ )
        {
            printf("Accessmac      : %d\n", pInfo->pProtocolSubtypeElement[count].AccessMac );
            printf("AuthPort       : %d\n", pInfo->pProtocolSubtypeElement[count].AuthProt );
            printf("Controlmac     : %d\n", pInfo->pProtocolSubtypeElement[count].ControlMac );
            printf("EncryptProt    : %d\n", pInfo->pProtocolSubtypeElement[count].EncryptProt );
            printf("ForwardMac     : %d\n", pInfo->pProtocolSubtypeElement[count].ForwardMac );
            printf("Idle State     : %d\n", pInfo->pProtocolSubtypeElement[count].IdleState );
            printf("KeyExchange    : %d\n", pInfo->pProtocolSubtypeElement[count].KeyExchange );
            printf("MultDisc       : %d\n", pInfo->pProtocolSubtypeElement[count].MultDisc );
            printf("Physical Layer : %d\n", pInfo->pProtocolSubtypeElement[count].PhysicalLayer );
            printf("Reversemac     : %d\n", pInfo->pProtocolSubtypeElement[count].ReverseMac );
            printf("Secprot        : %d\n", pInfo->pProtocolSubtypeElement[count].SecProt );
            printf("VirtStream     : %d\n", pInfo->pProtocolSubtypeElement[count].VirtStream );
        }
    }
}

void displayNasRankIndicator(unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
    {
        printf("\nNAS Rank Indicator Indication\n");
        printf("\tcount0: %d\n", pInfo->rankIndicatorTlv.count0);
        printf("\tcount1: %d\n", pInfo->rankIndicatorTlv.count1);
    }
}

void displayEdrxChangeInfo(unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 16))
        printf( "eDRX Enable Disable Status: %x\n",pInfo->edrxEnableTypeTlv.edrxEnabled);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 17))
        printf( "eDRX Cycle Length: %x\n",pInfo->edrxCycleLengthTlv.cycleLength);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 18))
        printf( "eDRX Paging Time Window: %x\n",pInfo->edrxPagingTimeWindowTlv.edrxPtw);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 19))
        printf( "eDRX RAT Type: %x\n",pInfo->edrxRatTypeTlv.edrxRatType);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 20))
        printf( "eDRX LTE Operating Mode: %08x\n",pInfo->edrxCiotLteModeTlv.lteOpMode);
}

void displayNetworkRejectInd(unpack_nas_SLQSNasNetworkRejectCallback_Ind_t *pInfo)
{
    RETURN_IF_PTR_IS_NULL(pInfo)
    swi_uint256_print_mask (pInfo->ParamPresenceMask);

    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 1))
        printf( "\tRadio Interface : %d\n", pInfo->radioIf);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 2))
        printf( "\tService Domain : %d\n", pInfo->serviceDomain);
    if(swi_uint256_get_bit (pInfo->ParamPresenceMask, 3))
        printf( "\tReject Cause : %d\n", pInfo->rejectCause);
    if((pInfo->pPlmnId !=NULL) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 16)))
    {
        printf( "\tMCC : %d\n", pInfo->pPlmnId->mcc);
        printf( "\tMNC : %d\n", pInfo->pPlmnId->mnc);
        printf( "\tMNC includes pcs digit : %d\n", pInfo->pPlmnId->pcsDigit);
    }
    if((pInfo->pCsgId !=NULL) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 17)))
        printf( "\tCSG ID : %ul\n", pInfo->pCsgId->csgId);
    if((pInfo->pLteOpMode !=NULL) && (swi_uint256_get_bit (pInfo->ParamPresenceMask, 18)))
        printf( "\tLTE Operational Mode : %ul\n", pInfo->pLteOpMode->lteOpMode);
}

void
ind_rpt_prt(const char *unpacker)
{
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", unpacker, "Success", "N/A");
#else
    UNUSEDPARAM(unpacker);
#endif
}

void nas_setOemApiTestConfig(nasConfig *nasConfigList)
{
    if ( nasConfigList != NULL)
    {
        MCC = nasConfigList->mcc;
        MNC = nasConfigList->mnc;
        strcpy((char*)Name, nasConfigList->networkName);
        MNRInfoDigit.mcc = nasConfigList->mcc;;
        MNRInfoDigit.mnc = nasConfigList->mnc;
    }
}

swi_uint256_t unpack_nas_SetServingSystemCallback_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_11_BITS,1,16,17,18,19,20,21,22,23,24,36)
}};

swi_uint256_t unpack_nas_SetServingSystemCallback_indParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

swi_uint256_t unpack_nas_SLQSNasSwiOTAMessageCallback_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)
}};

swi_uint256_t unpack_nas_SLQSNasSwiOTAMessageCallback_indParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

void nas_indication_handler(uint16_t msgid, uint8_t *msg, uint32_t rlen)
{
    unpack_nas_SLQSNasSigInfoCallback_ind_t sigInfo;
    unpack_nas_SLQSSysInfoCallback_ind_t sysInfo;
    unpack_nas_SetEventReportInd_t eventReport;
    unpack_nas_SetDataCapabilitiesCallback_ind_t dataCap;
    unpack_nas_SetRoamingIndicatorCallback_ind_t roaming;
    unpack_nas_SetServingSystemCallback_ind_t SSinfo;
    unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t SSPInfo;
    unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t SwiOtaMesInfo;
    unpack_nas_SLQSNasNetworkTimeCallBack_ind_t networktime;
    unpack_nas_SetNasLTECphyCaIndCallback_ind_t ltecphyca;
    unpack_nas_SLQSNasTimerCallback_ind_t timer;
    unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t rankIndicator;
    unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t hdrPersonality;
    unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t edrxChangeInfo;
    unpack_nas_SLQSNasNetworkRejectCallback_Ind_t netRejectInd;
    unpack_nas_SLQSNasRFBandInfoCallback_Ind_t  rfBandInfoInd;

    switch(msgid)
    {
        case eQMI_NAS_SIG_INFO_IND:
            unpack_nas_SLQSNasSigInfoCallback_ind( msg, rlen, &sigInfo);
            //it is normal that indication unpack returns eQCWWAN_ERR_INVALID_QMI_RSP(0x11)
            //which simply indicate absense of result code TLV(0x02).
            //Result code TLV is only available on Response
            swi_uint256_print_mask (sigInfo.ParamPresenceMask);
            displayNasSigInfo(&sigInfo);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "unpack_nas_SLQSNasSigInfoCallback_ind", "Success", "N/A");
        #endif
            break;
        
        case eQMI_NAS_SYS_INFO_IND:
            unpackRetCode = unpack_nas_SLQSNasSysInfoCallback_ind( msg, rlen, &sysInfo);
            printf ("unpack_nas_SLQSNasSysInfoCallback_ind result: %d, 0x%02X\n", unpackRetCode, unpackRetCode);
            swi_uint256_print_mask (sysInfo.ParamPresenceMask);
            displayNasSysInfo(&sysInfo);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSNasSysInfoCallback_ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;

        case eQMI_NAS_SS_INFO_IND:
            unpack_nas_SetDataCapabilitiesCallback_ind( msg, rlen, &dataCap);
            displayDataCap(&dataCap);
            ind_rpt_prt("unpack_nas_SetDataCapabilitiesCallback_ind");
            if(unpack_nas_SetRoamingIndicatorCallback_ind( msg, rlen, &roaming)==eQCWWAN_ERR_NONE)
            {
                swi_uint256_print_mask (roaming.ParamPresenceMask);
                if(swi_uint256_get_bit (roaming.ParamPresenceMask, 16))
                {
                    printf("\t Roaming Indicator %d\n", roaming.roaming);
                }
                ind_rpt_prt("unpack_nas_SetRoamingIndicatorCallback_ind");
            }
            if(unpack_nas_SetServingSystemCallback_ind( msg, rlen, &SSinfo)==eQCWWAN_ERR_NONE)
            {
                DisplayServingSystem(&SSinfo);
#if DEBUG_LOG_TO_FILE
                local_fprintf("%s,%s,", "unpack_nas_SetServingSystemCallback_ind","Success");
                CHECK_WHITELIST_MASK(
                    unpack_nas_SetServingSystemCallback_indParamPresenceMaskWhiteList,
                    SSinfo.ParamPresenceMask);
                CHECK_MANDATORYLIST_MASK(
                    unpack_nas_SetServingSystemCallback_indParamPresenceMaskMandatoryList,
                    SSinfo.ParamPresenceMask);
                local_fprintf("%s\n", "N/A");
#endif
            }
            break;

        case eQMI_NAS_SET_EVENT:
            unpack_nas_SetEventReportInd( msg, rlen, &eventReport);
            displayNasEventInfo(&eventReport);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "unpack_nas_SetEventReportInd", "Success", "N/A");
        #endif
             break;
         case eQMI_NAS_SYS_SELECT_IND:
            unpack_nas_SLQSSetSysSelectionPrefCallBack_ind(msg, rlen, &SSPInfo);
            displayNasSystemSelectPrefInfo(&SSPInfo);
            #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "UpkQmiCbkNasSystemSelPrefInd", "Success", "N/A");
            #endif
             break;
        case eQMI_NAS_SWI_OTA_MESSAGE_INDICATION:
            unpack_nas_SLQSNasSwiOTAMessageCallback_ind(msg, rlen, &SwiOtaMesInfo);
            displayNasSwiOTAMessageInfo(&SwiOtaMesInfo);
            #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SetServingSystemCallback_ind","Success");
            CHECK_WHITELIST_MASK(
                unpack_nas_SLQSNasSwiOTAMessageCallback_indParamPresenceMaskWhiteList,
                SwiOtaMesInfo.ParamPresenceMask);
            CHECK_MANDATORYLIST_MASK(
                unpack_nas_SLQSNasSwiOTAMessageCallback_indParamPresenceMaskMandatoryList,
                SwiOtaMesInfo.ParamPresenceMask);
            local_fprintf("%s\n", "N/A");
            #endif
             break;

        case eQMI_NAS_NETWORK_TIME_IND:
            unpack_nas_SLQSNasNetworkTimeCallBack_ind( msg, rlen, &networktime);
            displayNasNetworkTime(&networktime);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "unpack_nas_SLQSNasNetworkTimeCallBack_ind", "Success", "N/A");
        #endif
            break;

        case eQMI_NAS_LTE_CPHY_CA_IND:
            unpack_nas_SetNasLTECphyCaIndCallback_ind( msg, rlen, &ltecphyca);
            displayNasLTECphyCa(&ltecphyca);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "unpack_nas_SetNasLTECphyCaIndCallback_ind", "Success", "N/A");
        #endif
            break;

        case eQMI_NAS_SWI_NETWORK_TIMERS_IND:
            unpack_nas_SLQSNasTimerCallback_ind( msg, rlen, &timer);
            displayNasTimer(&timer);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,%s\n", "unpack_nas_SLQSNasTimerCallback_ind", "Success", "N/A");
        #endif
            break;
        case eQMI_NAS_SWI_HDR_PERSONALITY_IND:
            memset(&hdrPersonality, 0, sizeof(unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t));
            unpackRetCode = unpack_nas_SLQSSwiHDRPersonalityCallback_Ind( msg, rlen, &hdrPersonality);
            printf ("QMI_NAS_SWI_HDR_PERSONALITY_IND result: %d\n", unpackRetCode);
            if (unpackRetCode == eQCWWAN_ERR_NONE)
                displayNasHdrPersonality(&hdrPersonality);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSSwiHDRPersonalityCallback_Ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;
        case eQMI_NAS_SWI_RANK_INDICATOR_IND:
            memset(&rankIndicator, 0, sizeof(unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t));
            unpackRetCode = unpack_nas_SLQSSwiRandIndicatorCallback_Ind( msg, rlen, &rankIndicator);
            printf ("QMI_NAS_SWI_RANK_INDICATOR_IND result: %d\n", unpackRetCode);
            if (unpackRetCode == eQCWWAN_ERR_NONE)
                displayNasRankIndicator(&rankIndicator);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSSwiRandIndicatorCallback_Ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;
        case eQMI_NAS_EDRX_CHANGE_INFO_IND:
            memset(&edrxChangeInfo, 0, sizeof(unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t));
            unpackRetCode = unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind( msg, rlen, &edrxChangeInfo);
            printf ("QMI_NAS_EDRX_CHANGE_INFO_IND result: %d\n", unpackRetCode);
            if (unpackRetCode == eQCWWAN_ERR_NONE)
                displayEdrxChangeInfo(&edrxChangeInfo);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;
        case eQMI_NAS_NETWORK_REJECT_IND:
            memset(&netRejectInd, 0, sizeof(unpack_nas_SLQSNasNetworkRejectCallback_Ind_t));
            unpackRetCode = unpack_nas_SLQSNasNetworkRejectCallback_Ind( msg, rlen, &netRejectInd);
            printf ("QMI_NAS_NETWORK_REJECT_IND result: %d\n", unpackRetCode);
            if (unpackRetCode == eQCWWAN_ERR_NONE)
                displayNetworkRejectInd(&netRejectInd);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSNasNetworkRejectCallback_Ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;
        case eQMI_NAS_RF_BAND_INFO_IND:
            memset(&rfBandInfoInd, 0, sizeof(unpack_nas_SLQSNasRFBandInfoCallback_Ind_t));
            unpackRetCode = unpack_nas_SLQSNasRFBandInfoCallback_Ind( msg, rlen, &rfBandInfoInd);
            printf ("QMI_NAS_RF_BAND_INFO_IND result: %d\n", unpackRetCode);
            if (unpackRetCode == eQCWWAN_ERR_NONE)
                dump_SLQSNasRFBandInfoCallback_Ind(&rfBandInfoInd);

        #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_nas_SLQSNasRFBandInfoCallback_Ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
            local_fprintf("%s\n", "N/A");
        #endif
            break;
    }
}

//3gpp2 network name unpack with test vector captured from Verizon live network
void get_home_network_unpack_test()
{
    uint8_t name[255];
    uint16_t mcc, mnc;
    uint16_t sid, nid;
    uint16_t nw2mcc, nw2mnc;
    uint8_t nw2descdisp;
    uint8_t encoding_type;
    uint8_t name_3gpp2_encoded[255];
    uint8_t len = sizeof(name_3gpp2_encoded);
    uint8_t name_3gpp2[255];
    uint8_t test_vector[] = {
         0x02,0x02,0x00,0x25,0x00,0x33,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x05
        ,0x00,0x36,0x01,0x00,0x00,0x00,0x10,0x04,0x00,0xf6,0x03,0xff,0xff,0x11,0x15,0x00
        ,0x36,0x01,0x00,0x00,0x01,0x02,0x0e,0xd6,0xb2,0x3c,0xad,0x7f,0xbb,0x41,0xd7,0xb4
        ,0xbc,0xcc,0x2e,0xcf,0xe7,0x12,0x02,0x00,0x00,0x00
    };
    //3gpp unpack check
    unpack_nas_GetHomeNetwork_t network1;
    unpack_nas_GetHomeNetwork(test_vector, sizeof(test_vector), &network1);

    unpack_nas_GetHomeNetwork3GPP2_t network2;
    memset(&network2, 0, sizeof(network2));
    //3gpp2 null param check
    unpack_nas_GetHomeNetwork3GPP2(test_vector, sizeof(test_vector), &network2);

    //3gpp2 param assigned check
    network2.pMCC = &mcc;
    network2.pMNC = &mnc;
    network2.pName = name;
    network2.nameSize = sizeof(name);
    network2.pSID = &sid;
    network2.pNID = &nid;
    network2.pNw2MCC = &nw2mcc;
    network2.pNw2MNC = &nw2mnc;
    network2.pNw2DescDisp = &nw2descdisp;
    network2.pNw2DescEnc = &encoding_type;
    network2.pNw2DescLen = &len;
    network2.pNw2Name = name_3gpp2_encoded;
    unpack_nas_GetHomeNetwork3GPP2(test_vector, sizeof(test_vector), &network2);
    liteqmi_helper_decode7bitAsciiEncString(name_3gpp2_encoded, len, name_3gpp2);
    printf("[%s] 3gpp2 network name(%d): %s\n", __func__, encoding_type, name_3gpp2);
}

//unpack test Indication
void unpack_nas_test_indication()
{
    unpack_nas_SLQSSysInfoCallback_ind_t sysInfo;
    unpack_nas_SLQSNasNetworkTimeCallBack_ind_t networktime;
    unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t hdrPersonality;
    unpack_nas_SLQSNasSigInfoCallback_ind_t sigInfo;
    uint8_t edrx_ind_dummy_msg[] = {0x04, 0x01, 0x00, 0xBF, 0x00, 0x17, 0x00, 0x10,
                      0x01, 0x00, 0x01, 0x11, 0x01, 0x00, 0x01, 0x12,
                      0x01, 0x00, 0x04, 0x13, 0x01, 0x00, 0x08, 0x14,
                      0x04, 0x00, 0x03, 0x00, 0x00, 0x00 };    //3gpp unpack check

    unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t edrxChangeInfo;
    unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind(edrx_ind_dummy_msg, sizeof(edrx_ind_dummy_msg), &edrxChangeInfo);
    printf("[%s] eDRX Change Info: \n", __func__);
    displayEdrxChangeInfo(&edrxChangeInfo);

    uint8_t nw_reject_ind_dummy_msg[] = {0x04, 0x01, 0x00, 0x68, 0x00, 0x22, 0x00, 0x01,
                      0x01, 0x00, 0x08, 0x02, 0x01, 0x00, 0x02, 0x03,
                      0x01, 0x00, 0x05, 0x10, 0x05, 0x00, 0x44, 0x00, 0x52, 0x01, 0x01, 0x11,
                      0x04, 0x00, 0x01, 0x02, 0x03, 0x04, 0x12, 0x04, 0x00, 0x03, 0x00, 0x00,
                      0x00};    //n/w reject unpack check

    uint8_t SLQSNasSysInfo_dmmmy_ind_msg[] = { 
                        0x04,0x06,0x00,0x4e,0x00,0x6d,0x00,0x13,
                        0x03,0x00,0x01,0x03,0x00,0x14,0x03,0x00,
                        0x00,0x00,0x00,0x18,0x21,0x00,0x01,0x00,
                        0x01,0x03,0x01,0x01,0x01,0x00,0x01,0x9a,
                        0x1f,0x01,0xbf,0xb1,0x09,0x00,0x00,0x00,
                        0x00,0x01,0x34,0x35,0x34,0x30,0x36,0xff,
                        0x01,0x00,0x01,0x03,0x01,0x3c,0x01,0x1d,
                        0x06,0x00,0xff,0xff,0x01,0x00,0x00,0x00,
                        0x20,0x08,0x00,0x01,0x00,0x00,0x00,0x01,
                        0x00,0x00,0x00,0x23,0x01,0x00,0x00,0x24,
                        0x01,0x00,0x01,0x28,0x04,0x00,0x00,0x00,
                        0x00,0x00,0x29,0x01,0x00,0x03,0x2e,0x01,
                        0x00,0x01,0x30,0x04,0x00,0x00,0x00,0x00,
                        0x00,0x33,0x04,0x00,0x00,0x00,0x00,0x00,
                        0x48,0x01,0x00,0x01
                      };

    uint8_t SLQSNasSysInfo_dmmmy_ind_msg_ext[] = { 
                        0x04,0x06,0x00,0x4e,0x00,0xA1,0x00,0x13,
                        0x03,0x00,0x01,0x03,0x00,0x14,0x03,0x00,
                        0x00,0x00,0x00,0x18,0x21,0x00,0x01,0x00,
                        0x01,0x03,0x01,0x01,0x01,0x00,0x01,0x9a,
                        0x1f,0x01,0xbf,0xb1,0x09,0x00,0x00,0x00,
                        0x00,0x01,0x34,0x35,0x34,0x30,0x36,0xff,
                        0x01,0x00,0x01,0x03,0x01,0x3c,0x01,0x1d,
                        0x06,0x00,0xff,0xff,0x01,0x00,0x00,0x00,
                        0x20,0x08,0x00,0x01,0x00,0x00,0x00,0x01,
                        0x00,0x00,0x00,0x23,0x01,0x00,0x00,0x24,
                        0x01,0x00,0x01,0x28,0x04,0x00,0x00,0x00,
                        0x00,0x00,0x29,0x01,0x00,0x03,0x2e,0x01,
                        0x00,0x01,0x30,0x04,0x00,0x00,0x00,0x00,
                        0x00,0x33,0x04,0x00,0x00,0x00,0x00,0x00,
                        0x48,0x01,0x00,0x01,0x4B,0x04,0x00,0x02,
                        0x00,0x00,0x00,0x4C,0x03,0x00,0x01,0x02,
                        0x01,0x4D,0x1D,0x00,0x01,0x02,0x01,0x02,
                        0x01,0x05,0x01,0x01,0x01,0x03,0x01,0x01,
                        0x04,0x03,0x02,0x01,0x01,0x03,0x48,0x01,
                        0x05,0x06,0x07,0x08,0x09,0x10,0x01,0x02,
                        0x01,0x4E,0x04,0x00,0x03,0x00,0x00,0x00
                      };
    unpack_qmi_t rsp_ctx;
    const char *qmi_msg = NULL;
    unpack_nas_SLQSNasNetworkRejectCallback_Ind_t nwrejectinfo;
    unpack_nas_SLQSNasNetworkRejectCallback_Ind(nw_reject_ind_dummy_msg, sizeof(nw_reject_ind_dummy_msg), &nwrejectinfo);
    printf("[%s] Network Reject Indication: \n", __func__);
    displayNetworkRejectInd(&nwrejectinfo);
    qmi_msg = helper_get_resp_ctx(eNAS, SLQSNasSysInfo_dmmmy_ind_msg,sizeof(SLQSNasSysInfo_dmmmy_ind_msg), &rsp_ctx);
    UNUSEDPARAM(qmi_msg);
    nas_indication_handler(rsp_ctx.msgid,SLQSNasSysInfo_dmmmy_ind_msg,sizeof(SLQSNasSysInfo_dmmmy_ind_msg));

    qmi_msg = helper_get_resp_ctx(eNAS, SLQSNasSysInfo_dmmmy_ind_msg_ext,sizeof(SLQSNasSysInfo_dmmmy_ind_msg_ext), &rsp_ctx);
    UNUSEDPARAM(qmi_msg);
    nas_indication_handler(rsp_ctx.msgid,SLQSNasSysInfo_dmmmy_ind_msg_ext,sizeof(SLQSNasSysInfo_dmmmy_ind_msg_ext));

    //Test unpack
    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_nas_SLQSNasSysInfoCallback_ind,
        unpack_nas_SLQSSysInfoCallback_ind_t,
        displayNasSysInfo,
        &sysInfo,
        {
            DUMMY_UNPACK_NAS_SLQSSYSINFOCALLBACK_IND_MSG
        }
    );
    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_nas_SLQSNasNetworkTimeCallBack_ind,
        unpack_nas_SLQSNasNetworkTimeCallBack_ind_t,
        displayNasNetworkTime,
        &networktime,
        {
            {
            0x04,0x01,0x00,0x4c,0x00,0x17,0x00,0x01,0x08,0x00,0x00,0x01,0x02,0x03,
            0x04,0x05,0x06,0x07,0x10,0x01,0x00,0x01,0x11,0x01,0x00,0x11,0x12,0x01,
            0x00,0x21
            }
        }
    );
    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_nas_SLQSNasSigInfoCallback_ind,
        unpack_nas_SLQSNasSigInfoCallback_ind_t,
        displayNasSigInfo,
        &sigInfo,
        {
            {
            0x04,0x01,0x00,0x58,0x75,0x21,0x00,0x10,0x02,0x00,0x10,0x11,0x11,0x19,
            0x00,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
            0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18
            }
        }
    );
    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_nas_SLQSSwiHDRPersonalityCallback_Ind,
        unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t,
        displayNasHdrPersonality,
        &hdrPersonality,
        {
            {
            0x04,0x01,0x00,0x58,0x75,0x21,0x00,
            0x10,0x02,0x00,0x00,0x00,
            0x11,0x19,0x00,0x01,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
            0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18
            }
        }
    );

    TEST_IND_UNPACK_WITH_TYPE_DUMP(
        unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind,
        unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t,
        displayEdrxChangeInfo,
        &edrxChangeInfo,
        {
            {
            0x04, 0x01, 0x00, 0xBF, 0x00, 0x17, 0x00, 0x10,
              0x01, 0x00, 0x01, 0x11, 0x01, 0x00, 0x01, 0x12,
              0x01, 0x00, 0x04, 0x13, 0x01, 0x00, 0x08, 0x14,
              0x04, 0x00, 0x03, 0x00, 0x00, 0x00
            }
        }
    );
}

void nas_SLQSNasGetSigInfo_pack_unpack()
{
    if(nas<0)
        nas = client_fd(eNAS);
    if(nas<0)
    {
        fprintf(stderr,"NAS Service Not Supported!\n");
        return ;
    }
    if(nas>=0)
    {
        unsigned xid =1;
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        #if _ASYNC_READ_ENABLE_
        struct aiocb aiocb;
        int ret = -1;
        iAsyncRead(&aiocb,&nas,QMI_MSG_MAX,&rsp[0]);
        #endif
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = pack_nas_SLQSNasGetSigInfo(&req_ctx, req, &reqLen);
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
            #if _ASYNC_READ_ENABLE_
            rspLen = ret = iWaitAsyncReadDataReady(&aiocb,&nas,DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT);
            #else
            rtn = rspLen = read(nas, rsp, QMI_MSG_MAX);
            #endif
            if ((rtn>0) && (rspLen > 0) && (rspLen <= QMI_MSG_MAX))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
                
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("NAS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        unpackRetCode = unpack_valid_nas_SLQSNasGetSigInfo(rsp, 0);
                        printf("CDMASSInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSNasGetSigInfo(rsp, 1);
                        printf("HDRSSInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSNasGetSigInfo(rsp, 2);
                        printf("GSMSSInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSNasGetSigInfo(rsp, 3);
                        printf("WCDMASSInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSNasGetSigInfo(rsp, 4);
                        printf("LTESSInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                    }
                }
            }
            
        }
        
    }
}

void nas_SLQSGetServingSystem_pack_unpack()
{
    if(nas<0)
        nas = client_fd(eNAS);
    if(nas<0)
    {
        fprintf(stderr,"NAS Service Not Supported!\n");
        return ;
    }
    if(nas>=0)
    {
        unsigned xid =1;
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        #if _ASYNC_READ_ENABLE_
        struct aiocb aiocb;
        int ret = -1;
        iAsyncRead(&aiocb,&nas,QMI_MSG_MAX,&rsp[0]);
        #endif
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = pack_nas_SLQSGetServingSystem(&req_ctx, req, &reqLen);
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);

            #if _ASYNC_READ_ENABLE_
            rspLen = ret = iWaitAsyncReadDataReady(&aiocb,&nas,DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT);
            #else
            rtn = rspLen = read(nas, rsp, QMI_MSG_MAX);
            #endif
            if ((rtn>0) && (rspLen > 0) && (rspLen <= QMI_MSG_MAX))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
                
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("NAS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 0);
                        printf("RoamIndicatorVal : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 1);
                        printf("DataSrvCapabilities : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 2);
                        printf("CurrentPLMN : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 3);
                        printf("SystemID and NetworkID : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 4);
                        printf("BaseStationID, BaseStationLatitude and BasestationLongitude : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 5);
                        printf("RoamingIndicatorList : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 6);
                        printf("DefaultRoamInd : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 7);
                        printf("Gpp2TimeZone : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 8);
                        printf("CDMA_P_Rev : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 9);
                        printf("GppTimeZone : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 10);
                        printf("GppNetworkDSTAdjustment : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 11);
                        printf("Lac : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 12);
                        printf("CellID : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 13);
                        printf("ConcSvcInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 14);
                        printf("PRLInd : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 15);
                        printf("DTMInd : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 16);
                        printf("DetailedSvcInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 17);
                        printf("CDMASystemInfoExt : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 18);
                        printf("HdrPersonality : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 19);
                        printf("TrackAreaCode : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetServingSystem(rsp, 20);
                        printf("CallBarStatus : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                    }
                }
            }
            
        }
        
    }
}

void nas_SLQSGetSignalStrength_pack_unpack()
{
    if(nas<0)
        nas = client_fd(eNAS);
    if(nas<0)
    {
        fprintf(stderr,"NAS Service Not Supported!\n");
        return ;
    }
    if(nas>=0)
    {
        unsigned xid =1;
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        #if _ASYNC_READ_ENABLE_
        struct aiocb aiocb;
        memset(&aiocb, 0, sizeof(struct aiocb));
        int ret = -1;
        iAsyncRead(&aiocb,&nas,QMI_MSG_MAX,&rsp[0]);
        #endif
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = pack_nas_SLQSGetSignalStrength(&req_ctx, req, &reqLen,signalStrenghMask);
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);

            #if _ASYNC_READ_ENABLE_
            rspLen = ret = iWaitAsyncReadDataReady(&aiocb,&nas,DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT);
            #else
            rtn = rspLen = read(nas, rsp, QMI_MSG_MAX);
            #endif
            if ((rtn>0) && (rspLen > 0) && (rspLen <= QMI_MSG_MAX))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
                
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("NAS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 0);
                        printf("rxSignalStrengthList and rxSignalStrengthListLen : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 1);
                        printf("ecioList and ecioListLen : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 2);
                        printf("Io : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 3);
                        printf("sinr : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 4);
                        printf("errorRateListLen, errorRateList : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 5);
                        printf("rsrqInfo : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 6);
                        printf("ltesnr : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_SLQSGetSignalStrength(rsp, 7);
                        printf("ltersrp : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                    }
                }
            }
            
        }
        
    }
}

void nas_GetCDMANetworkParameters_pack_unpack()
{
    if(nas<0)
        nas = client_fd(eNAS);
    if(nas<0)
    {
        fprintf(stderr,"NAS Service Not Supported!\n");
        return ;
    }
    if(nas>=0)
    {
        unsigned xid =1;
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        #if _ASYNC_READ_ENABLE_
        struct aiocb aiocb;
        int ret = -1;
        iAsyncRead(&aiocb,&nas,QMI_MSG_MAX,&rsp[0]);
        #endif
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = pack_nas_GetCDMANetworkParameters(&req_ctx, req, &reqLen);
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);

            #if _ASYNC_READ_ENABLE_
            rspLen = ret = iWaitAsyncReadDataReady(&aiocb,&nas,DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT);
            #else
            rtn = rspLen = read(nas, rsp, QMI_MSG_MAX);
            #endif
            if ((rtn>0) && (rspLen > 0) && (rspLen <= QMI_MSG_MAX))
            {
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
                
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("NAS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 0);
                        printf("SCI : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 1);
                        printf("SCM : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 2);
                        printf("RegHomeSID, RegForeignSID and RegForeignNID : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 3);
                        printf("ForceRev0 : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 4);
                        printf("CustomSCP, Protocol ,Broadcast and Application : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                        unpackRetCode = unpack_valid_nas_GetCDMANetworkParameters(rsp, 5);
                        printf("Roaming : %s\n", unpackRetCode==0 ? "Valid":"Invalid");
                    }
                }
            }
            
        }
        
    }
}

void nas_test_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;
    #if _ASYNC_READ_ENABLE_
    struct aiocb aiocb;
    #endif

    if(g_runoem_demo==1)
    {
        get_home_network_unpack_test();
    }

    printf("======NAS dummy pack/unpack test===========\n");
    unpack_nas_test_indication();

    printf("======NAS pack/unpack test===========\n");

#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "NAS Pack/UnPack API Name", 
                      "Status",
                      "Unpack Payload Parsing",
                      "Remark");
#endif

    unsigned xid =1;
    if(g_runoem_demo==0)
    {
        printf("======NAS dummy unpack test===========\n");
        iLocalLog = 0;
        nas_validate_dummy_uppack();
        iLocalLog = 1;
        nas_SLQSNasGetSigInfo_pack_unpack();
        nas_SLQSGetServingSystem_pack_unpack();
        nas_SLQSGetSignalStrength_pack_unpack();
        nas_GetCDMANetworkParameters_pack_unpack();
    }

    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitestnas)/sizeof(testitem_t);
    }
    else
    {
        numoftestcase = sizeof(nastotest)/sizeof(testitem_t);
    }
    for(i=0; i<numoftestcase; i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        const char *qmi_msg;
        #if _ASYNC_READ_ENABLE_
        int ret = -1;
        if(g_auto_test_enable)
        {
            if(i>0)
                i=0;
        }
        #endif
        memset(&req_ctx, 0, sizeof(req_ctx));
        if(xid==0)
            xid=1;
        req_ctx.xid = xid;
        if(g_runoem_demo==1)
        {
            rtn = run_pack_item(oemapitestnas[i].pack)(&req_ctx, req, &reqLen,oemapitestnas[i].pack_ptr);
        }
        else
        {
            rtn = run_pack_item(nastotest[i].pack)(&req_ctx, req, &reqLen,nastotest[i].pack_ptr);
        }
#if DEBUG_LOG_TO_FILE
        if(g_runoem_demo==1)
        {
            local_fprintf("%s,%s,%s\n", oemapitestnas[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
        else
        {
            local_fprintf("%s,%s,%s\n", nastotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
#endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(nas<0)
            nas = client_fd(eNAS);
        if(nas<0)
        {
            fprintf(stderr,"NAS Service Not Supported!\n");
            return ;
        }
        #if _ASYNC_READ_ENABLE_
        iAsyncRead(&aiocb,&nas,QMI_MSG_MAX,&rsp[0]);
        #endif
        rtn = write(nas, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eNAS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            #if _ASYNC_READ_ENABLE_
            rspLen = ret = iWaitAsyncReadDataReady(&aiocb,&nas,DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT);
            #else
            rtn = rspLen = read(nas, rsp, QMI_MSG_MAX);
            #endif
            if ((rtn>0) && (rspLen > 0) && (rspLen <= QMI_MSG_MAX))
            {
                printf("read %d\n", rspLen);
                printf("\n");fflush(stdout);
                qmi_msg = helper_get_resp_ctx(eNAS, rsp, rspLen, &rsp_ctx);
                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("NAS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        if(g_runoem_demo==1)
                        {
                            unpackRetCode = run_unpack_item(oemapitestnas[i].unpack)(rsp, rspLen, oemapitestnas[i].unpack_ptr);
                        }
                        else
                        {
                            unpackRetCode = run_unpack_item(nastotest[i].unpack)(rsp, rspLen, nastotest[i].unpack_ptr);
                        }
                    #if DEBUG_LOG_TO_FILE
                        if(g_runoem_demo==1)
                        {
                            local_fprintf("%s,%s,", oemapitestnas[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                        }
                        else
                        {
                            local_fprintf("%s,%s,", nastotest[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
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
                            oemapitestnas[i].dump(oemapitestnas[i].unpack_ptr);
                        }
                        else
                        {
                            nastotest[i].dump(nastotest[i].unpack_ptr);
                        }   
                        /* sleep 3 seconds for next write */
                        #if !(_ASYNC_READ_ENABLE_)
                        sleep(3);
                        #endif
                        /* skip the while loop so that the the next API can be invoked */
                        break;
                    }
                    else
                    {
                        /* the response is not belongs to this API, skip it and read again */
                        printf("xid %d\n",xid);
                        #if !(_ASYNC_READ_ENABLE_)
                        sleep(1);
                        #endif
                    }
                }
                else if (rsp_ctx.type == eIND)
                {
                    printf("NAS IND: msgid 0x%x\n", rsp_ctx.msgid);
                    nas_indication_handler(rsp_ctx.msgid, rsp,  rspLen);
                }
            }
            else if (rtn< 0)
            {
                printf("Read Error rtn:%d\n", rtn);
                CloseNasFd();
                return ;//Read Error
            }
            else
            {
                /* read nothing, sleep 1 second and read again */
                #if !(_ASYNC_READ_ENABLE_)
                sleep(1);
                #else 
                printf("rtn:%d line:%d\n", rtn,__LINE__);
                #endif
            }            
        }
        xid++;
    }

    if(nas>=0)
        close(nas);
    nas=-1;
}

void CloseNasFd()
{
    if(nas>=0)
        close(nas);
    nas=-1;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable 
 ******************************************************************************/
uint8_t validate_nas_resp_msg[][QMI_MSG_MAX] ={

    /* eQMI_NAS_GET_RSSI */
    {0x02, 0x01, 0x00, 0x20, 0x00, 0x49, 0x00,
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0xD9, 0x08,
     0x10, 0x06, 0x00, 0x02, 0x00, 0x01, 0x02, 0x02, 0x03,
     0x11, 0x06, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x02,
     0x12, 0x06, 0x00, 0x02, 0x00, 0x03, 0x02, 0x01, 0x03,
     0x13, 0x04, 0x00, 0xC8, 0x00, 0x00, 0x00,
     0x14, 0x01, 0x00, 0x04,
     0x15, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02,
     0x16, 0x02, 0x00, 0x03, 0x08,
     0x17, 0x02, 0x00, 0x21, 0x00,
     0x18, 0x02, 0x00, 0x22, 0x00,
     },

    /* eQMI_NAS_GET_SYS_SELECT_PREF */
    {0x02, 0x02, 0x00, 0x34, 0x00, 0xA4, 0x00,
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,//7
     0x10, 0x01, 0x00, 0x00,//4
     0x11, 0x02, 0x00, 0x18, 0x00,//5
     0x12, 0x08, 0x00, 0x00, 0x00, 0x40, 0x0C, 0x00, 0x00, 0x06, 0x10,//11
     0x14, 0x02, 0x00, 0xFF, 0x00,//5
     0x15, 0x08, 0x00, 0xD5, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     0x16, 0x01, 0x00, 0x01,//4
     0x18, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,//7
     0x19, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,//7
     0x1C, 0x06, 0x00, 0x05, 0x08, 0x05, 0x04, 0x01, 0x02,//9
     0x22, 0x02, 0x00, 0x02, 0x00,//5
     0x25, 0x08, 0x00, 0xD1, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     0x26, 0x08, 0x00, 0xD2, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     0x1A,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x1D,0x04,0x00,0x00,0x00,0x00,0x00,
     0x1F,0x04,0x00,0x01,0x00,0x00,0x00,
     0x20,0x04,0x00,0x03,0x00,0x00,0x00,
     0x23,0x20,0x00,0xC5,0x00,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},

    /* eQMI_NAS_SET_SYS_SELECT_PREF */
    {0x02, 0x03, 0x00, 0x33, 0x00, 0x07, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_NAS_SET_REG_EVENT */
    {0x02, 0x04, 0x00, 0x03, 0x00, 0x07, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_NAS_GET_RF_INFO */
    {0x02, 0x05, 0x00, 0x31, 0x00, 0x32, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x06, 0x00, 0x01, 0x08, 0x7A, 0x00, 0x9A, 0x05,
     0x10, 0x04, 0x00, 0x01, 0x07, 0x7B, 0x00,
     0x11, 0x08, 0x00, 0x01, 0x08, 0x7A, 0x00, 0x9A, 0x05, 0x00, 0x00,
     0x12, 0x06, 0x00, 0x01, 0x06, 0x8B, 0x00, 0x00, 0x00,
     0x13, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
     },

     /* eQMI_NAS_GET_SIG_INFO */
    {0x02, 0x06, 0x00, 0x4F, 0x00, 0x2B, 0x00, 
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x10, 0x03, 0x00, 0x01, 0x7B, 0x00,
     0x11, 0x08, 0x00, 0x01, 0x08, 0x7A, 0x00, 0x9A, 0x05, 0x00, 0x00,
     0x12, 0x01, 0x00, 0x20,
     0x13, 0x03, 0x00, 0x05, 0x8B, 0x00,
     0x14, 0x06, 0x00, 0x01, 0x06, 0x8B, 0x00, 0x02, 0x00,
     },

     /* eQMI_NAS_SIG_INFO_IND */
     {0x04, 0x07, 0x00, 0x51, 0x00, 0x24, 0x00,
     0x10, 0x03, 0x00, 0x01, 0x7B, 0x00,
     0x11, 0x08, 0x00, 0x01, 0x08, 0x7A, 0x00, 0x9A, 0x05, 0x00, 0x00,
     0x12, 0x01, 0x00, 0x20,
     0x13, 0x03, 0x00, 0x05, 0x8B, 0x00,
     0x14, 0x06, 0x00, 0x01, 0x06, 0x8B, 0x00, 0x02, 0x00,
     },

     /* eQMI_NAS_GET_HOME_INFO */
     {0x02, 0x08, 0x00, 0x25, 0x00, 0x21, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x10, 0x00, 0x94, 0x01, 0x58, 0x00, 0x0B, 0x56,
      0x6F, 0x64, 0x61, 0x66, 0x6F, 0x6E, 0x65, 0x20, 0x49, 0x4E,
      0x10, 0x04, 0x00, 0x24, 0x00, 0x25, 0x00,
     },

    /* eQMI_NAS_GET_SYS_INFO */
    {0x02,0x09,0x00,0x4D,0x00,0xAC,0x00,
     0x02,0x04,0x00,0x00,0x00,0x00, 0x00,
     0x13,0x03,0x00,0x01,0x03,0x00,
     0x14,0x03,0x00,0x00,0x00,0x00,
     0x18,0x21,0x00,0x01,0x00,0x01,0x03,0x01,0x01,0x01,0x00,0x01,0x9a,
      0x1f,0x01,0xbf,0xb1,0x09,0x00,0x00,0x00,0x00,0x01,0x34,0x35,0x34,0x30,0x36,0xff,
      0x01,0x00,0x01,0x03,0x01,0x3c,0x01,
     0x1d,0x06,0x00,0xff,0xff,0x01,0x00,0x00,0x00,
     0x20,0x08,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
     0x23,0x01,0x00,0x01,
     0x26,0x01,0x00,0x00,
     0x27,0x04,0x00,0x01,0x00,0x00,0x00,
     0x29,0x01,0x00,0x00,
     0x2A,0x04,0x00,0x03,0x00,0x00,0x00,
     0x2F,0x04,0x00,0x00,0x00,0x00,0x00,
     0x31,0x04,0x00,0x00,0x00,0x00,0x00,
     0x34,0x02,0x00,0xFF,0xFF,
     0x49,0x04,0x00,0x03,0x00,0x00,0x00,
     0x4A,0x03,0x00,0x02,0x01,0x00,
     0x4B,0x1D,0x00,0x01,0x01,0x01,0x03,0x01,0x02,0x01,0x00,0x01,0x34,0x12,
      0x01,0x89,0x67,0x45,0x23,0x01,0x03,0x04,0x01,0x11,0x22,0x33,0x44,0x55,0x66,0x01,0xAA,0xBB,
     0x4C,0x04,0x00,0x03,0x00,0x00,0x00,
    },


    /* eQMI_NAS_SYS_INFO_IND */
    {0x04,0x0A,0x00,0x4e,0x00,0xA1,0x00,
     0x13,0x03,0x00,0x01,0x03,0x00,
     0x14,0x03,0x00,0x00,0x00,0x00,
     0x18,0x21,0x00,0x01,0x00,0x01,0x03,0x01,0x01,0x01,0x00,0x01,0x9a,
      0x1f,0x01,0xbf,0xb1,0x09,0x00,0x00,0x00,0x00,0x01,0x34,0x35,0x34,0x30,0x36,0xff,
      0x01,0x00,0x01,0x03,0x01,0x3c,0x01,
     0x1d,0x06,0x00,0xff,0xff,0x01,0x00,0x00,0x00,
     0x20,0x08,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
     0x23,0x01,0x00,0x01,
     0x24,0x01,0x00,0x01,
     0x28,0x04,0x00,0x00,0x00,0x00,0x00,
     0x29,0x01,0x00,0x03,
     0x2e,0x01,0x00,0x01,
     0x30,0x04,0x00,0x00,0x00,0x00,0x00,
     0x33,0x04,0x00,0x00,0x00,0x00,0x00,
     0x48,0x01,0x00,0x01,
     0x4B,0x04,0x00,0x02,0x00,0x00,0x00,
     0x4C,0x03,0x00,0x01,0x02,0x01,
     0x4D,0x1D,0x00,0x01,0x02,0x01,0x02,
      0x01,0x05,0x01,0x01,0x01,0x03,0x01,0x01,
      0x04,0x03,0x02,0x01,0x01,0x03,0x48,0x01,
      0x05,0x06,0x07,0x08,0x09,0x10,0x01,0x02,
      0x01,
     0x4E,0x04,0x00,0x03,0x00,0x00,0x00
    },

     /* eQMI_NAS_GET_SS_INFO */
     {0x02, 0x0B, 0x00, 0x24, 0x00, 0x41, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x06, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x05,
      0x10, 0x01, 0x00, 0x00,
      0x11, 0x03, 0x00, 0x02, 0x01, 0x05,
      0x12, 0x05, 0x00, 0x94, 0x01, 0x0E, 0x00, 0x00,
      0x15, 0x03, 0x00, 0x01, 0x05, 0x00,
      0x21, 0x05, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,
      0x26, 0x02, 0x00, 0xC7, 0x01,
      0x27, 0x05, 0x00, 0x94, 0x01, 0x0E, 0x00, 0x00,
      0x28, 0x01, 0x00, 0x00,
     },

     /* eQMI_NAS_SS_INFO_IND */
     {0x04, 0x0C, 0x00, 0x24, 0x00, 0x3A, 0x00,
      0x01, 0x06, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x05,
      0x10, 0x01, 0x00, 0x00,
      0x11, 0x03, 0x00, 0x02, 0x01, 0x05,
      0x12, 0x05, 0x00, 0x94, 0x01, 0x0E, 0x00, 0x00,
      0x15, 0x03, 0x00, 0x01, 0x05, 0x00,
      0x21, 0x05, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,
      0x26, 0x02, 0x00, 0xC7, 0x01,
      0x27, 0x05, 0x00, 0x94, 0x01, 0x0E, 0x00, 0x00,
      0x28, 0x01, 0x00, 0x00,
     },

     /* eQMI_NAS_SCAN_NETS */
     {0x02, 0x0D, 0x00, 0x21, 0x00, 0x60, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, //7
      0x10, 0x25, 0x00, 0x02, 0x00, 0x94, 0x01, 0x02, 0x00, 0xAA, 0x0A, 0x49,//40
       0x4E, 0x44, 0x20, 0x61, 0x69, 0x72, 0x74, 0x65,
       0x6C, 0x94, 0x01, 0x02, 0x00, 0xAA, 0x0D, 0x54, 0x65, 0x73, 0x74,
       0x20, 0x50, 0x4C, 0x4D, 0x4E, 0x20, 0x31, 0x2D, 0x31,
      0x11, 0x0C, 0x00, 0x02, 0x00, 0x94, 0x01, 0x02, 0x00, 0x05,//15
       0x94, 0x01, 0x35, 0x00, 0x05,
      0x12, 0x0C, 0x00, 0x02, 0x00, 0x94, 0x01, 0x02, 0x00, 0x01,//15
       0x94, 0x01, 0x35, 0x00, 0x00,
      0x13, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,//7
      0x16, 0x09, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00,//12
       0x05, 0x00, 0x00, 0x00,
      },

     /* eQMI_NAS_SWI_GET_CQI */
     {0x02, 0x0E, 0x00, 0x61, 0x75, 0x0E, 0x00, 
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04,
     },

     /* eQMI_NAS_SWI_MODEM_STATUS  */
     {0x02, 0x0F, 0x00, 0x56, 0x55, 0x1B, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x05, 0x00, 0x1B, 0x05, 0x05, 0x00, 0x01,
      0x10, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
      },

     /* eQMI_NAS_SET_EVENT */
     {0x02, 0x10, 0x00, 0x02, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

     /* eQMI_NAS_EVENT_IND */
     {0x04, 0x11, 0x00, 0x02, 0x00, 0x39, 0x00,
      0x10, 0x02, 0x00, 0x04, 0x04,
      0x11, 0x0B, 0x00, 0x02, 0x01, 0x02, 0x00, 0x03, 0x00, 0x02, 0x03, 0x00, 0x04, 0x00,
      0x12, 0x03, 0x00, 0x02, 0x01, 0x0E,
      0x13, 0x02, 0x00, 0x01, 0x08,
      0x14, 0x02, 0x00, 0x03, 0x02,
      0x15, 0x04, 0x00, 0xC8, 0x00, 0x00, 0x00,
      0x16, 0x01, 0x00, 0x04,
      0x17, 0x03, 0x00, 0x01, 0x00, 0x02,
      0x18, 0x02, 0x00, 0x03, 0x08,
     },

     /* eQMI_NAS_GET_NET_PARAMS */
     {0x02, 0x12, 0x00, 0x2F, 0x00, 0x2D, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x11, 0x01, 0x00, 0x01,
      0x12, 0x01, 0x00, 0x02,
      0x13, 0x03, 0x00, 0x01, 0x01, 0x01,
      0x14, 0x01, 0x00, 0x01,
      0x15, 0x0D, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
      0x16, 0x01, 0x00, 0x02,
      },

     /* eQMI_NAS_GET_AAA_AUTH_STATUS */
     {0x02, 0x13, 0x00, 0x32, 0x00, 0x0B, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x00, 0x01
     },

     /* eQMI_NAS_GET_ACCOLC */
     {0x02, 0x14, 0x00, 0x2C, 0x00, 0x0B, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x00, 0x01
     },

     /* eQMI_NAS_SET_ACCOLC */
     {0x02, 0x15, 0x00, 0x2D, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_CONFIG_SIG_INFO2 */
     {0x02, 0x16, 0x00, 0x6C, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_GET_TECH_PREF */
     {0x02, 0x17, 0x00, 0x2B, 0x00, 0x12, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x03, 0x00, 0x02, 0x00, 0x01,
      0x10, 0x02, 0x00, 0x02, 0x00
     },

     /* eQMI_NAS_SET_TECH_PREF */
     {0x02, 0x18, 0x00, 0x2A, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

    /* eQMI_NAS_SYS_SELECT_IND */
    {0x04, 0x19, 0x00, 0x34, 0x00, 0x5A, 0x00,
     0x10, 0x01, 0x00, 0x00,//4
     0x11, 0x02, 0x00, 0x18, 0x00,//5
     0x12, 0x08, 0x00, 0x00, 0x00, 0x40, 0x0C, 0x00, 0x00, 0x06, 0x10,//11
     0x14, 0x02, 0x00, 0xFF, 0x00,//5
     0x15, 0x08, 0x00, 0xD5, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     0x16, 0x01, 0x00, 0x01,//4
     0x18, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,//7
     0x19, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,//7
     0x1C, 0x06, 0x00, 0x05, 0x08, 0x05, 0x04, 0x01, 0x02,//9
     0x22, 0x02, 0x00, 0x02, 0x00,//5
     0x25, 0x08, 0x00, 0xD1, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     0x26, 0x08, 0x00, 0xD2, 0x00, 0x16, 0x08, 0xE0, 0x01, 0x00, 0x00,//11
     },

     /* eQMI_NAS_REGISTER_NET */
     {0x02, 0x1A, 0x00, 0x22, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

    /* eQMI_NAS_SWI_OTA_MESSAGE_INDICATION */
    {0x04, 0x1B, 0x00, 0x67, 0x55, 0x1F, 0x00,
     0x01, 0x0B, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x31,0x32,0x33,0x34,0x35,
     0x10, 0x03, 0x00, 0x01, 0x02, 0x03,
     0x11, 0x08, 0x00, 0x00,0x00, 0x00, 0x04, 0x05, 0x06, 0x07, 0x08
    },

     /* eQMI_NAS_SWI_INDICATION_REGISTER */
     {0x02, 0x1C, 0x00, 0x66, 0x55, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_LTE_CPHY_CA_IND */
     { 0x04,0x1D,0x00,0x82,0x00,0x3E,0x00,
      0x01,0x08,0x00,0x01,0x01,0x22,0x03,0x02,0x00,0x00,0x00,
      0x10,0x04,0x00,0x05,0x00,0x00,0x00,
      0x11,0x0e,0x00,0x02,0x01,0x02,0x03,0x03,0x00,0x00,0x00,0x78,0x00,0x01,0x00,0x00,0x00,
      0x12,0x0a,0x00,0x0F,0x01,0xf2,0x03,0x04,0x00,0x00,0x00,0x98,0x00,
      0x13,0x01,0x00,0x03,
      0x14,0x02,0x00,0x34,0x12,
      0x15,0x02,0x00,0x78,0x56,
     },

     /* eQMI_NAS_ATTACH_DETACH */
     {0x02, 0x1E, 0x00, 0x23, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_SET_NET_PARAMS */
     {0x02, 0x1F, 0x00, 0x30, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_GET_HDR_COLOR_CODE */
     {0x02, 0x20, 0x00, 0x57, 0x00, 0x0B, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x01, 0x00, 0x01
     },

     /* eQMI_NAS_SWI_PS_DETACH */
     {0x02, 0x21, 0x00, 0x5D, 0x65, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_CONFIG_SIG_INFO */
     {0x02, 0x22, 0x00, 0x50, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_SWI_SET_CHANNEL_LOCK */
     {0x02, 0x23, 0x00, 0x69, 0x55, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_SET_EDRX_PARAMS */
     {0x02, 0x24, 0x00, 0xC0, 0x00, 0x07, 0x00, 0x02,
      0x04, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_GET_PLMN_NAME */
     {0x02, 0x25, 0x00, 0x44, 0x00, 0x27, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x1D, 0x00, 0x00, 0x0B, 0x56, 0x6F, 0x64,
      0x61, 0x66, 0x6F, 0x6E, 0x65, 0x20, 0x49, 0x4E,
      0x00, 0x00, 0x00, 0x04, 0x41, 0x54, 0x26, 0x54, 0x00, 0x00, 0x00, 
      0x04, 0x41, 0x54, 0x26, 0x54,
     },

     /* eQMI_NAS_GET_CELL_LOCATION_INFO */
     {0x02, 0x26, 0x00, 0x43, 0x00, 0xAA, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x17, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
      0x16, 0x02, 0x00, 0x01, 0x00, 
      0x15, 0x10, 0x00, 0x01, 0x01, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x00, 0x03, 0x04, 0x05,
       0x06, 0x00, 0x07, 0x00,
      0x14, 0x08, 0x00, 0x01, 0x01, 0x01, 0x00, 0x02, 0x03,0x04, 0x00,
      0x13, 0x31, 0x00, 0x01, 0x04, 0xF4, 0x41, 0x2D, 0x04, 0x2D, 0x87,
       0xAD, 0x00, 0x9A, 0x05, 0xEC, 0x01, 0x07, 0x12, 0x0E, 0x3A, 0x03,
       0xEC, 0x01, 0xC4, 0xFF, 0x8C, 0xFD, 0x83, 0xFE, 0x41, 0x00, 0xD9,
       0x00, 0x38, 0xFF, 0x81, 0xFC, 0x41, 0xFE, 0x00, 0x00, 0xDA, 0x00, 0x38, 0xFF, 
       0x7A, 0xFC, 0x42, 0xFE, 0x00, 0x00,
      0x11, 0x19, 0x00, 0x01, 0x00, 0x04, 0xF4, 0x41, 0x2D, 0x04, 0x2D, 0x87,
       0xAD, 0x00, 0x9A, 0x05, 0xEC, 0x01, 0x01, 0x07, 0x12, 0x0E, 0x3A, 0x03,
       0xEC, 0x01, 0xC4, 0x00,
      0x1C, 0x0A, 0x00, 0x00, 0x00, 0x82, 0xC2, 0x00, 0x00, 0x80, 0xBF, 0xFF, 0xFF,
      0x22, 0x15, 0x00, 0xCE, 0xE1, 0x04, 0xF4, 0x41, 0xCD, 0x90, 0x05, 0x2A, 0xC7,
       0x01, 0xBC, 0xFF, 0xFD, 0xFF, 0x0F, 0x00, 0x2E, 0x00, 0x00, 0x00,
      0x29, 0x01, 0x00, 0x00,
     },

     /* eQMI_NAS_GET_NETWORK_TIME */
     {0x02, 0x27, 0x00, 0x7D, 0x00, 0x15, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x11, 0x0B, 0x00, 0xE3, 0x07, 0x0B, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00
     },

     /* eQMI_NAS_NETWORK_TIME_IND */
     {0x04, 0x28, 0x00, 0x4C, 0x00, 0x17, 0x00,
      0x01, 0x08, 0x00, 0xE3, 0x07, 0x0B, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x10, 0x01, 0x00, 0x01,
      0x11, 0x01, 0x00, 0x02,
      0x12, 0x01, 0x00, 0x05,
     },

     /* eQMI_NAS_SWI_GET_LTE_SCC_RX_INFO */
     {0x02, 0x29, 0x00, 0x6B, 0x55, 0x1B, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x11, 0x00, 0xE3, 0x00, 0x00, 0x00, 0xD2, 0x00, 0x01, 0x01, 0x02, 
       0x03, 0x00, 0x00, 0x00, 0x04, 0x00,0x00, 0x00
     },

     /* eQMI_NAS_SWI_NETWORK_TIMERS_IND */
     {0x04, 0x2A, 0x00, 0x63, 0x75, 0x11, 0x00,
      0x10, 0x0E, 0x00, 0x56, 0x6F, 0x64,
      0x61, 0x66, 0x6F, 0x6E, 0x41, 0x54, 0x26, 0x00, 0x00, 0x02, 0x01,
     },

     /* eQMI_NAS_GET_TX_RX_INFO */
     {0x02, 0x2B, 0x00, 0x5A, 0x00, 0x3F, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x15, 0x00, 0x01, 0x01, 0x02, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 
       0x05, 0x06, 0x00, 0x00, 0x07, 0x08,0x00, 0x00, 0x09, 0x0A,0x00, 0x00,
      0x11, 0x15, 0x00, 0x01, 0x09, 0x0A, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00, 
       0x05, 0x06, 0x00, 0x00, 0x07, 0x08,0x00, 0x00, 0x09, 0x0A,0x00, 0x00,
      0x12, 0x05, 0x00, 0x01, 0x01, 0x02, 0x00, 0x00
     },

     /* eQMI_NAS_GET_OPERATOR_NAME */
     {0x02, 0x2C, 0x00, 0x39, 0x00, 0x4B, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x06, 0x00, 0x01, 0x04, 0x6E, 0x41, 0x54, 0x41,
      0x11, 0x0D, 0x00, 0x01, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x01, 0x00, 0x02, 0x00, 0x01,
      0x12, 0x0F, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x04, 0x55, 0x56, 0x57, 0x58, 0x04, 
        0x55, 0x56, 0x57, 0x58,
      0x13, 0x05, 0x00, 0x41, 0x42, 0x43, 0x44, 0x00,
      0x14, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x55, 0x56, 0x57, 0x58, 0x04, 
        0x55, 0x56, 0x57, 0x58,
     },

     /* eQMI_NAS_GET_3GPP2_SUBSCRIPTION */
     {0x02, 0x2D, 0x00, 0x3E, 0x00, 0x61, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x05, 0x00, 0x04, 0x6E, 0x41, 0x54, 0x41,
      0x11, 0x05, 0x00, 0x04, 0x41, 0x42, 0x43, 0x44,
      0x12, 0x09, 0x00, 0x02, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00,
      0x13, 0x0F, 0x00, 0x31, 0x32, 0x33, 0x02, 0x00, 0x57, 0x58, 0x59, 
        0x55, 0x56, 0x57, 0x58, 0x57, 0x58, 0x59,
      0x14, 0x10, 0x00, 0x31, 0x32, 0x33, 0x02, 0x00, 0x57, 0x58, 0x59, 
        0x55, 0x56, 0x57, 0x58, 0x57, 0x58, 0x59, 0x01,
      0x15, 0x08, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00,
      0x16, 0x0B, 0x00, 0x0A, '9', '8','1', '1','0', '1','2', '3','4', '5',
     },

     /* eQMI_NAS_SWI_HDR_PERSONALITY */
     {0x02, 0x2E, 0x00, 0x58, 0x75, 0x28, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x02, 0x00, 0x00, 0x00, 
      0x11, 0x19, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00,
       0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00
     },

     /* eQMI_NAS_SWI_HDR_PROT_SUBTYPE */
     {0x02, 0x2F, 0x00, 0x59, 0x75, 0x33, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x02, 0x00, 0x02, 0x00, 
      0x11, 0x19, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00,
       0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00,
      0x12, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_GET_ERR_RATE */
     {0x02, 0x30, 0x00, 0x52, 0x00, 0x19, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x02, 0x00, 0x02, 0x01, 
      0x11, 0x02, 0x00, 0x04, 0x03,
      0x12, 0x01, 0x00, 0x04,
      0x13, 0x01, 0x00, 0x08, 
     },

     /* eQMI_NAS_SWI_GET_HRPD_STATS */
     {0x02, 0x31, 0x00, 0x61, 0x55, 0x2B, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x02, 0x00, 0x02, 0x01, 
      0x11, 0x10, 0x00, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
       0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
      0x12, 0x09, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00
     },

     /* eQMI_NAS_SWI_NETWORK_DEBUG */
     {0x02, 0x32, 0x00, 0x62, 0x75, 0x42, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x01, 0x00, 0x02, 
      0x11, 0x16, 0x00, 0x01, 0x03, 0x00, 0x56, 0x00, 0x34, 0x12, 0x34, 0x12,
        0x56, 0x00, 0x34, 0x12, 0x01, 0x04, 0x00, 0x01, 0x02, 0x01, 0x00, 0x02, 0x00,
      0x12, 0x0C, 0x00, 0x01, 0x12, 0x01, 0x01, 0x00, 0x34, 0x12, 0x01, 0x00, 0x02, 0x00, 0x01,
      0x13, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04,
      0x14, 0x05, 0x00, 0x78, 0x56, 0x34, 0x12, 0x01
     },

     /* eQMI_NAS_SWI_GET_CHANNEL_LOCK */
     {0x02, 0x33, 0x00, 0x68, 0x55, 0x27, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x05, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00,
      0x11, 0x09, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00,
      0x12, 0x09, 0x00, 0x01, 0x06, 0x05, 0x00, 0x00, 0x08, 0x07, 0x00, 0x00,
     },

     /* eQMI_NAS_GET_EDRX_PARAMS */
     {0x02, 0x34, 0x00, 0xC1, 0x00, 0x1E, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x01, 0x00, 0x02,
      0x11, 0x01, 0x00, 0x03,
      0x12, 0x01, 0x00, 0x04,
      0x13, 0x01, 0x00, 0x05,
      0x14, 0x04, 0x00, 0x02, 0x01, 0x00, 0x00
     },

     /* eQMI_NAS_SWI_HDR_PERSONALITY_IND */
     {0x04, 0x35, 0x00, 0x58, 0x75, 0x21, 0x00,
      0x10, 0x02, 0x00, 0x00, 0x00, 
      0x11, 0x19, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00,
       0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 0x00
     },

     /* eQMI_NAS_SWI_RANK_INDICATOR_IND */
     {0x04, 0x36, 0x00, 0x6A, 0x55, 0x0B, 0x00,
      0x01, 0x08, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
     },

     /* eQMI_NAS_EDRX_CHANGE_INFO_IND */
     {0x04, 0x37, 0x00, 0xBF, 0x00, 0x17, 0x00,
      0x10, 0x01, 0x00, 0x01,
      0x11, 0x01, 0x00, 0x02,
      0x12, 0x01, 0x00, 0x03,
      0x13, 0x01, 0x00, 0x04,
      0x14, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,
     },

     /* eQMI_NAS_NETWORK_REJECT_IND */
     {0x04, 0x38, 0x00, 0x68, 0x00, 0x22, 0x00,
       0x01, 0x01, 0x00, 0x08,
       0x02, 0x01, 0x00, 0x02,
       0x03, 0x01, 0x00, 0x05,
       0x10, 0x05, 0x00, 0x44, 0x00, 0x52, 0x01, 0x01,
       0x11, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04,
       0x12, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00
     },

     /* eQMI_NAS_GET_NET_BAN_LIST */
     {0x02, 0x39, 0x00, 0x28, 0x00, 0x10, 0x00,
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x06, 0x00, 0x01, 0x00, 0x02, 0x01, 0x28, 0x00
     },

    /* eQMI_NAS_GET_LTE_CPHY_CA_INFO*/
    { 0x02,0x3A,0x03,0xac,0x00,0x6c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
      0x10,0x08,0x00,0x01,0x01,0x22,0x03,0x02,0x00,0x00,0x00,
      0x11,0x04,0x00,0x05,0x00,0x00,0x00,
      0x12,0x0e,0x00,0x02,0x01,0x02,0x03,0x03,0x00,0x00,0x00,0x78,0x00,0x01,0x00,0x00,0x00,
      0x13,0x0a,0x00,0x0F,0x01,0xf2,0x03,0x04,0x00,0x00,0x00,0x98,0x00,
      0x14,0x01,0x00,0x03,
      0x15,0x2e,0x00,0x03,0x64,0x00,0x11,0x22,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x07,0x1f,0x07,0x33,0x44,0x02,0x00,0x00,0x00,0x98,0x00,0x02,0x00,0x00,0x00,0x04,0x2c,0x01,0x55,0x66,0x05,0x00,0x00,0x00,0xa8,0x00,0x01,0x00,0x00,0x00,0x00 },

      /* eQMI_NAS_GET_HOME_INFO */
      {0x02, 0x3B, 0x00, 0x25, 0x00, 0x2D, 0x00,
       0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x01, 0x10, 0x00, 0x94, 0x01, 0x58, 0x00, 0x0B, 0x56,
       0x6F, 0x64, 0x61, 0x66, 0x6F, 0x6E, 0x65, 0x20, 0x49, 0x4E,
       0x10, 0x04, 0x00, 0x24, 0x00, 0x25, 0x00,
       0x12, 0x02, 0x00, 0x01, 0x00,
       0x13, 0x04, 0x00, 0x05, 0x00, 0x00, 0x00,
      },

      /* eQMI_NAS_RF_BAND_INFO_IND */
      { 0x04, 0x3C, 0x00, 0x66, 0x00, 0x27, 0x00,
        0x01, 0x05, 0x00, 0x08, 0x50, 0x00, 0x9A, 0x05,
        0x10, 0x03, 0x00, 0x08, 0x50, 0x00,
        0x11, 0x07, 0x00, 0x08, 0x50, 0x00, 0x9A, 0x05, 0x00, 0x00,
        0x12, 0x05, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00,
        0x13, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
      },

};

/* eQMI_NAS_GET_RSSI */
const unpack_nas_GetSignalStrengths_t const_unpack_nas_GetSignalStrengths_t = {
        3,{0xD9,0x01,0x02},{0x08,0x02,0x03},
        {{SWI_UINT256_BIT_VALUE(SET_11_BITS,1,2,16,17,18,19,20,21,22,23,24)}} };

const unpack_nas_SLQSGetSignalStrength_t const_unpack_nas_SLQSGetSignalStrength_t = {
        0,3,{{-39,0x08},{-2,1},{-3,2}},
        2,{{-1,2},{0,0x03}},0x000000c8,0x04,
        1,{{0x01,0x02}},{0x03,0x08},0x0021,0x0022,
        {{SWI_UINT256_BIT_VALUE(SET_11_BITS,1,2,16,17,18,19,20,21,22,23,24)}} };


/* eQMI_NAS_GET_SYS_SELECT_PREF */
uint8_t              var_EmerMode;
uint16_t             var_ModePref;
uint64_t             var_BandPref;
uint16_t             var_PRLPref;
uint16_t             var_RoamPref;
uint64_t             var_LTEBandPref;
uint8_t              var_NetSelPref;
uint32_t             var_SrvDomainPref;
uint32_t             var_GWAcqOrderPref;

unpack_nas_SLQSGetSysSelectionPref_t var_unpack_nas_SLQSGetSysSelectionPref_t = {
        &var_EmerMode, &var_ModePref, &var_BandPref, &var_PRLPref, &var_RoamPref,
        &var_LTEBandPref, &var_NetSelPref, &var_SrvDomainPref, &var_GWAcqOrderPref,
        {{0}} };

nas_EmerModeTlv          var_EmerModeExt;
nas_ModePrefTlv          var_ModePrefExt;
nas_BandPrefTlv          var_BandPrefExt;
nas_PRLPrefTlv           var_PRLPrefExt;
nas_RoamPrefTlv          var_RoamPrefExt;
nas_LTEBandPrefTlv       var_LTEBandPrefExt;
nas_NetSelPrefTlv        var_NetSelPrefExt;
nas_SrvDomainPrefTlv     var_SrvDomainPrefExt;
nas_GWAcqOrderPrefTlv    var_GWAcqOrderPrefExt;
uint8_t var_Acq[5];
nas_AcqOrderPrefTlv      var_AcqOrderPrefExt = {0,var_Acq,0};
nas_RatDisabledMaskTlv   var_RatDisabledMaskExt;
nas_CiotLteOpModePrefTlv var_CiotLteOpModePrefExt;
nas_LteM1BandPrefTlv     var_LteM1BandPrefExt;
nas_LteNb1BandPrefTlv    var_LteNb1BandPrefExt;
nas_CiotAcqOrderPrefTlv  var_CiotAcqOrderPrefExt;
nas_NR5gBandPrefTlv      var_Nr5gBandPrefExt;
nas_LTEBandPrefExtTlv    var_LTEBandPrefExt2;

unpack_nas_SLQSGetSysSelectionPrefExt_t var_unpack_nas_SLQSGetSysSelectionPrefExt_t = {
        &var_EmerModeExt, &var_ModePrefExt, &var_BandPrefExt, &var_PRLPrefExt, &var_RoamPrefExt,
        &var_LTEBandPrefExt, &var_NetSelPrefExt, &var_SrvDomainPrefExt, &var_GWAcqOrderPrefExt,
        &var_AcqOrderPrefExt, &var_RatDisabledMaskExt, &var_CiotLteOpModePrefExt, &var_LteM1BandPrefExt,
        &var_LteNb1BandPrefExt, &var_CiotAcqOrderPrefExt, &var_Nr5gBandPrefExt, &var_LTEBandPrefExt2
        };

uint8_t              cst_EmerMode = 0;
uint16_t             cst_ModePref = 0x0018;
uint64_t             cst_BandPref = 0x100600000c400000ULL;
uint16_t             cst_PRLPref = 0;
uint16_t             cst_RoamPref = 0x00FF;
uint64_t             cst_LTEBandPref = 0x000001E0081600D5ULL;
uint8_t              cst_NetSelPref = 0x01;
uint32_t             cst_SrvDomainPref = 0x00000002;
uint32_t             cst_GWAcqOrderPref = 0x00000001;

const unpack_nas_SLQSGetSysSelectionPref_t const_unpack_nas_SLQSGetSysSelectionPref_t = {
        &cst_EmerMode, &cst_ModePref, &cst_BandPref, &cst_PRLPref, &cst_RoamPref,
        &cst_LTEBandPref, &cst_NetSelPref, &cst_SrvDomainPref, &cst_GWAcqOrderPref,
        {{SWI_UINT256_BIT_VALUE(SET_18_BITS,2,16,17,18,20,21,22,24,25,28,34,37,38,26,29,31,32,35)}} };

nas_EmerModeTlv          cst_EmerModeExt = {0,1};
nas_ModePrefTlv          cst_ModePrefExt = {0x0018,1};
nas_BandPrefTlv          cst_BandPrefExt = {0x100600000c400000ULL, 1};
nas_PRLPrefTlv           cst_PRLPrefExt;
nas_RoamPrefTlv          cst_RoamPrefExt = {0x00FF,1};
nas_LTEBandPrefTlv       cst_LTEBandPrefExt = {0x000001E0081600D5ULL, 1};
nas_NetSelPrefTlv        cst_NetSelPrefExt = {1,1};
nas_SrvDomainPrefTlv     cst_SrvDomainPrefExt = {0x00000002, 1};
nas_GWAcqOrderPrefTlv    cst_GWAcqOrderPrefExt = {0x00000001, 1};
uint8_t cst_Acq[] = {0x08, 0x05, 0x04, 0x01, 0x02};
nas_AcqOrderPrefTlv      cst_AcqOrderPrefExt = {5,cst_Acq, 1};
nas_RatDisabledMaskTlv   cst_RatDisabledMaskExt = {2,1};
nas_CiotLteOpModePrefTlv cst_CiotLteOpModePrefExt;
nas_LteM1BandPrefTlv     cst_LteM1BandPrefExt = {0x000001E0081600D1ULL, 1};
nas_LteNb1BandPrefTlv    cst_LteNb1BandPrefExt = {0x000001E0081600D2ULL, 1};
nas_CiotAcqOrderPrefTlv  cst_CiotAcqOrderPrefExt;
nas_NR5gBandPrefTlv      cst_Nr5gBandPrefExt;
nas_LTEBandPrefExtTlv    cst_LTEBandPrefExtv1;

const unpack_nas_SLQSGetSysSelectionPrefExt_t const_unpack_nas_SLQSGetSysSelectionPrefExt_t = {
        &cst_EmerModeExt, &cst_ModePrefExt, &cst_BandPrefExt, &cst_PRLPrefExt, &cst_RoamPrefExt,
        &cst_LTEBandPrefExt, &cst_NetSelPrefExt, &cst_SrvDomainPrefExt, &cst_GWAcqOrderPrefExt,
        &cst_AcqOrderPrefExt, &cst_RatDisabledMaskExt, &cst_CiotLteOpModePrefExt, &cst_LteM1BandPrefExt,
        &cst_LteNb1BandPrefExt, &cst_CiotAcqOrderPrefExt, &cst_Nr5gBandPrefExt, &cst_LTEBandPrefExtv1
        };

nas_TDSCDMABandPrefTlv   var_TDSCDMABandPref;
nas_SrvRegRestricTlv     var_SrvRegRestric;
nas_UsageSettingTlv      var_UsageSetting;
nas_VoiceDomainPrefTlv   var_VoiceDomainPref;
unpack_nas_SLQSGetSysSelectionPrefExtV2_t var_unpack_nas_SLQSGetSysSelectionPrefExtV2_t = {
        &var_EmerModeExt, &var_ModePrefExt, &var_BandPrefExt, &var_PRLPrefExt, &var_RoamPrefExt,
        &var_LTEBandPrefExt, &var_NetSelPrefExt, &var_SrvDomainPrefExt, &var_GWAcqOrderPrefExt,
        &var_AcqOrderPrefExt, &var_RatDisabledMaskExt, &var_CiotLteOpModePrefExt, &var_LteM1BandPrefExt,
        &var_LteNb1BandPrefExt, &var_CiotAcqOrderPrefExt, &var_Nr5gBandPrefExt, &var_LTEBandPrefExt2,
        &var_TDSCDMABandPref,&var_SrvRegRestric,&var_UsageSetting,&var_VoiceDomainPref,{{0}}
        };

nas_TDSCDMABandPrefTlv   cst_TDSCDMABandPref= {0x00ULL,0x01};
nas_SrvRegRestricTlv     cst_SrvRegRestric = {0x00,0x01};
nas_UsageSettingTlv      cst_UsageSetting = {0x01,0x01};
nas_VoiceDomainPrefTlv   cst_VoiceDomainPref = {0x03,0x01};
nas_LTEBandPrefExtTlv    cst_LTEBandPrefExt2 = {0x080800C5ULL,0x00ULL,0x00ULL,0x00ULL,0x01};
const unpack_nas_SLQSGetSysSelectionPrefExtV2_t const_unpack_nas_SLQSGetSysSelectionPrefExtV2_t = {
        &cst_EmerModeExt, &cst_ModePrefExt, &cst_BandPrefExt, &cst_PRLPrefExt, &cst_RoamPrefExt,
        &cst_LTEBandPrefExt, &cst_NetSelPrefExt, &cst_SrvDomainPrefExt, &cst_GWAcqOrderPrefExt,
        &cst_AcqOrderPrefExt, &cst_RatDisabledMaskExt, &cst_CiotLteOpModePrefExt, &cst_LteM1BandPrefExt,
        &cst_LteNb1BandPrefExt, &cst_CiotAcqOrderPrefExt, &cst_Nr5gBandPrefExt, &cst_LTEBandPrefExt2,
        &cst_TDSCDMABandPref,&cst_SrvRegRestric,&cst_UsageSetting,&cst_VoiceDomainPref,
        {{SWI_UINT256_BIT_VALUE(SET_18_BITS,2,16,17,18,20,21,22,24,25,28,34,37,38,26,29,31,32,35)}}
};

/* eQMI_NAS_SYS_SELECT_IND */
const unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t const_unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t = {
    {{1,0}, {1,0x0018}, {1,0x100600000c400000ULL},{0,0},{1,0x00FF},{1,0x000001E0081600D5ULL},
     {1,1},{1,2}, {1,1}, {1,5,{0x08, 0x05, 0x04, 0x01, 0x02}},{1,2},{0,0},{1, 0x000001E0081600D1ULL},
     {1,0x000001E0081600D2ULL},{0,0,{0}}, {0,0,0,0,0}},0,
    {{SWI_UINT256_BIT_VALUE(SET_12_BITS,16,17,18,20,21,22,24,25,28,34,37,38)}} };

/* eQMI_NAS_SET_SYS_SELECT_PREF */
const unpack_nas_SLQSSetSysSelectionPref_t const_unpack_nas_SLQSSetSysSelectionPref_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_SET_REG_EVENT */
const unpack_nas_SLQSNasIndicationRegisterExt_t const_unpack_nas_SLQSNasIndicationRegisterExt_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_GET_RF_INFO */
const unpack_nas_GetRFInfo_t const_unpack_nas_GetRFInfo_t = {
        1,{{0x08,0x007A,0x059A}},{{SWI_UINT256_BIT_VALUE(SET_6_BITS,1,2,16,17,18,19)}} };

uint8_t var_RFBandInstanceSize = 4;
struct nas_RFBandInfoElements var_RFBandInfoParam[4];
uint8_t var_RFDedicatedBandInstanceSize = 4;
struct nas_RfDedicatedBandInfoElements var_RfDedicatedBandInfoParam[4];
nas_RfDedicatedBandInfo      var_RfDedicatedBandInfo = 
             {0,&var_RFDedicatedBandInstanceSize,var_RfDedicatedBandInfoParam};

uint8_t var_RFBandExtInstanceSize = 4;
struct nas_RfBandInfoExtFormatElements var_RfBandInfoExtFormatParam[4];
nas_RfBandInfoExtFormat      var_RfBandInfoExtFormat = 
             {0,&var_RFBandExtInstanceSize, var_RfBandInfoExtFormatParam};

uint8_t var_RFBandWidthInstanceSize = 4;
struct nas_RfBandwidthInfoElements var_RfBandwidthInfoParam[4];
nas_RfBandwidthInfo          var_RfBandwidthInfo = 
             {0, &var_RFBandWidthInstanceSize, var_RfBandwidthInfoParam};

uint32_t var_LTEOperation;
nas_LTEOperationMode         var_LTEOperationMode = {0, &var_LTEOperation};

unpack_nas_SLQSNasGetRFInfo_t var_unpack_nas_SLQSNasGetRFInfo_t = {
        {&var_RFBandInstanceSize,var_RFBandInfoParam}, &var_RfDedicatedBandInfo, 
         &var_RfBandInfoExtFormat,
        &var_RfBandwidthInfo,&var_LTEOperationMode,0 };

uint8_t cst_RFBandInstanceSize = 1;
struct nas_RFBandInfoElements cst_RFBandInfoParam[4]= {{0x08,0x007A,0x059A}};
uint8_t cst_RFDedicatedBandInstanceSize = 1;
struct nas_RfDedicatedBandInfoElements cst_RfDedicatedBandInfoParam[4] = {{0x07,0x007B}};
nas_RfDedicatedBandInfo      cst_RfDedicatedBandInfo = 
             {1,&cst_RFDedicatedBandInstanceSize,cst_RfDedicatedBandInfoParam};

uint8_t cst_RFBandExtInstanceSize = 1;
struct nas_RfBandInfoExtFormatElements cst_RfBandInfoExtFormatParam[4] = {{0x08,0x007A,0x0000059A}};
nas_RfBandInfoExtFormat      cst_RfBandInfoExtFormat = 
             {1,&cst_RFBandExtInstanceSize, cst_RfBandInfoExtFormatParam};

uint8_t cst_RFBandWidthInstanceSize = 1;
struct nas_RfBandwidthInfoElements cst_RfBandwidthInfoParam[4] = {{0x06,0x0000008B}};
nas_RfBandwidthInfo          cst_RfBandwidthInfo = 
             {1, &cst_RFBandWidthInstanceSize, cst_RfBandwidthInfoParam};

uint32_t cst_LTEOperation = 0x00000001;
nas_LTEOperationMode         cst_LTEOperationMode = {1, &cst_LTEOperation};

const unpack_nas_SLQSNasGetRFInfo_t const_unpack_nas_SLQSNasGetRFInfo_t = {
        {&cst_RFBandInstanceSize,cst_RFBandInfoParam}, &cst_RfDedicatedBandInfo, 
         &cst_RfBandInfoExtFormat,
         &cst_RfBandwidthInfo,&cst_LTEOperationMode,0 };

/* eQMI_NAS_GET_SIG_INFO */
const unpack_nas_SLQSNasGetSigInfo_t const_unpack_nas_SLQSNasGetSigInfo_t = {
        {1,0x007B},{1,0x7A08,0x00,0x0000059A},0x20,{0x05,0x008B},{1,6,0x008B,0x0002},
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_NAS_SIG_INFO_IND */
cdmaSSInfo        var_CDMASigInfo;
hdrSSInfo         var_HDRSigInfo;
int8_t            var_GSMSigInfo;
cdmaSSInfo        var_WCDMASigInfo;
lteSSInfo         var_LTESigInfo;
int8_t            var_Rscp;
tdscdmaSigInfoExt var_TDSCDMASigInfoExt;

unpack_nas_SLQSNasSigInfoCallback_ind_t var_unpack_nas_SLQSNasSigInfoCallback_ind_t = {
        &var_CDMASigInfo,&var_HDRSigInfo,&var_GSMSigInfo,&var_WCDMASigInfo,&var_LTESigInfo,
        &var_Rscp, &var_TDSCDMASigInfoExt, {{0}} };

cdmaSSInfo        cst_CDMASigInfo = {1,0x007B};
hdrSSInfo         cst_HDRSigInfo = {1,0x7A08,0x00,0x0000059A};
int8_t            cst_GSMSigInfo = 0x20;
cdmaSSInfo        cst_WCDMASigInfo = {0x05,0x008B};
lteSSInfo         cst_LTESigInfo = {1,6,0x008B,0x0002};
int8_t            cst_Rscp;
tdscdmaSigInfoExt cst_TDSCDMASigInfoExt;

const unpack_nas_SLQSNasSigInfoCallback_ind_t const_unpack_nas_SLQSNasSigInfoCallback_ind_t = {
        &cst_CDMASigInfo,&cst_HDRSigInfo,&cst_GSMSigInfo,&cst_WCDMASigInfo,&cst_LTESigInfo,
        &cst_Rscp, &cst_TDSCDMASigInfoExt,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,16,17,18,19,20)}} };

/* eQMI_NAS_GET_HOME_INFO */
const unpack_nas_GetHomeNetwork_t const_unpack_nas_GetHomeNetwork_t = {
        0x194, 0x0058,{0x56,0x6F,0x64,0x61,0x66,0x6F,0x6E,0x65,0x20,0x49,0x4E},
        0x0024, 0x0025,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

nas_homeNwMNC3GppTlv cst_HomeNwMNC3Gpp = {0x01,0x00,0x01};
nas_nwNameSrc3GppTlv cst_NwNameSrc3Gpp = {0x05,0x01};
/* eQMI_NAS_GET_HOME_INFO */
const unpack_nas_SLQSGetHomeNetwork_t const_unpack_nas_SLQSGetHomeNetwork_t = {
        0x194, 0x0058,{0x56,0x6F,0x64,0x61,0x66,0x6F,0x6E,0x65,0x20,0x49,0x4E},
        0x0024, 0x0025,&cst_HomeNwMNC3Gpp,&cst_NwNameSrc3Gpp,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,18,19)}} };

nas_homeNwMNC3GppTlv var_HomeNwMNC3Gpp;
nas_nwNameSrc3GppTlv var_NwNameSrc3Gpp;
/* eQMI_NAS_GET_HOME_INFO */
unpack_nas_SLQSGetHomeNetwork_t var_unpack_nas_SLQSGetHomeNetwork_t = {
        0xFF, 0xFF,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
        0xFF, 0xFF,&var_HomeNwMNC3Gpp,&var_NwNameSrc3Gpp,{{0}} };

/* eQMI_NAS_GET_SYS_INFO */
nas_SrvStatusInfo      var_CDMASrvStatusInfo;
nas_SrvStatusInfo      var_HDRSrvStatusInfo;
nas_GSMSrvStatusInfo   var_GSMSrvStatusInfo;
nas_GSMSrvStatusInfo   var_WCDMASrvStatusInfo;
nas_GSMSrvStatusInfo   var_LTESrvStatusInfo;
nas_CDMASysInfo        var_CDMASysInfo;
nas_HDRSysInfo         var_HDRSysInfo;
nas_GSMSysInfo         var_GSMSysInfo;
nas_WCDMASysInfo       var_WCDMASysInfo;
nas_LTESysInfo         var_LTESysInfo;
nas_AddCDMASysInfo     var_AddCDMASysInfo;
uint16_t               var_AddHDRSysInfo;
nas_AddSysInfo         var_AddGSMSysInfo;
nas_AddSysInfo         var_AddWCDMASysInfo;
uint16_t               var_AddLTESysInfo;
nas_CallBarringSysInfo var_GSMCallBarringSysInfo;
nas_CallBarringSysInfo var_WCDMACallBarringSysInfo;
uint8_t                var_LTEVoiceSupportSysInfo;
uint8_t                var_GSMCipherDomainSysInfo;
uint8_t                var_WCDMACipherDomainSysInfo;
uint32_t               var_CampedCiotLteOpMode;

unpack_nas_SLQSGetSysInfo_t var_unpack_nas_SLQSGetSysInfo_t = {
        &var_CDMASrvStatusInfo,&var_HDRSrvStatusInfo,&var_GSMSrvStatusInfo,&var_WCDMASrvStatusInfo,
        &var_LTESrvStatusInfo,&var_CDMASysInfo,&var_HDRSysInfo,&var_GSMSysInfo,&var_WCDMASysInfo,
        &var_LTESysInfo,&var_AddCDMASysInfo, &var_AddHDRSysInfo,&var_AddGSMSysInfo,&var_AddWCDMASysInfo,
        &var_AddLTESysInfo,&var_GSMCallBarringSysInfo,&var_WCDMACallBarringSysInfo,&var_LTEVoiceSupportSysInfo,
        &var_GSMCipherDomainSysInfo, &var_WCDMACipherDomainSysInfo, &var_CampedCiotLteOpMode,
        {{0}} };

nas_SrvStatusInfo      cst_CDMASrvStatusInfo;
nas_SrvStatusInfo      cst_HDRSrvStatusInfo;
nas_GSMSrvStatusInfo   cst_GSMSrvStatusInfo;
nas_GSMSrvStatusInfo   cst_WCDMASrvStatusInfo = {1,3,0};
nas_GSMSrvStatusInfo   cst_LTESrvStatusInfo = {0,0,0};
nas_CDMASysInfo        cst_CDMASysInfo;
nas_HDRSysInfo         cst_HDRSysInfo;
nas_GSMSysInfo         cst_GSMSysInfo;
nas_WCDMASysInfo       cst_WCDMASysInfo = 
              {{0x01,0x00,0x01,0x03,0x01,0x01,0x01,0x00},1,0x1f9A,1,0x0009b1bf,0,0,0,1,
               {0x34,0x35,0x34},{0x30,0x36,0xff},1,0,1,3,1, 0x013c} ;
nas_LTESysInfo         cst_LTESysInfo;
nas_AddCDMASysInfo     cst_AddCDMASysInfo;
uint16_t               cst_AddHDRSysInfo;
nas_AddSysInfo         cst_AddGSMSysInfo;
nas_AddSysInfo         cst_AddWCDMASysInfo = {0xffff, 0x00000001};
uint16_t               cst_AddLTESysInfo;
nas_CallBarringSysInfo cst_GSMCallBarringSysInfo;
nas_CallBarringSysInfo cst_WCDMACallBarringSysInfo = {0x00000001, 0x00000001};
uint8_t                cst_LTEVoiceSupportSysInfo;
uint8_t                cst_GSMCipherDomainSysInfo;
uint8_t                cst_WCDMACipherDomainSysInfo = 1;
uint32_t               cst_CampedCiotLteOpMode = 0x03;
uint8_t                cst_SysInfoNoChange= 1;
nas_LteCiotOpModeTlv   cst_LteCiotOpModeTlv = {0x00000002,1};
nas_NR5GSerStatTlv     cst_NR5GSerStatTlv = {1,2,1,1};

nas_NR5GSystemInfoTlv  cst_NR5GSystemInfoTlv = 
         {0x01,0x02,0x01,0x02,0x01,0x05,0x01,0x01,0x01,0x0103,1,0x01020304,0x01,0x03,0x48,0x01,
          {0x05,0x06,0x07},{0x08,0x09,0x10},1,0x0102,1};
nas_NR5GCellStatusTlv  cst_NR5GCellStatus = {0x00000003,1};

const unpack_nas_SLQSGetSysInfo_t const_unpack_nas_SLQSGetSysInfo_t = {
        &cst_CDMASrvStatusInfo,&cst_HDRSrvStatusInfo,&cst_GSMSrvStatusInfo,&cst_WCDMASrvStatusInfo,
        &cst_LTESrvStatusInfo,&cst_CDMASysInfo,&cst_HDRSysInfo,&cst_GSMSysInfo,&cst_WCDMASysInfo,
        &cst_LTESysInfo,&cst_AddCDMASysInfo, &cst_AddHDRSysInfo,&cst_AddGSMSysInfo,&cst_AddWCDMASysInfo,
        &cst_AddLTESysInfo,&cst_GSMCallBarringSysInfo,&cst_WCDMACallBarringSysInfo,&cst_LTEVoiceSupportSysInfo,
        &cst_GSMCipherDomainSysInfo, &cst_WCDMACipherDomainSysInfo, &cst_CampedCiotLteOpMode,
        {{SWI_UINT256_BIT_VALUE(SET_18_BITS,2,19,20,24,29,32,35,38,39,41,42,47,49,52,73,74,75,76)}} };

nas_LteEmbmsCoverageTlv   var_LteEmbmsCoverage;
nas_SimRejInfoTlv         var_SimRejInfo;
nas_ImsVoiceSupportLteTlv var_ImsVoiceSupportLte;
nas_LteVoiceDomainTlv     var_LteVoiceDomain;
nas_SrvRegRestrictionTlv  var_SrvRegRestriction;
nas_LteRegDomainTlv       var_LteRegDomain;
nas_LteEmbmsTraceIdTlv    var_LteEmbmsTraceId;
nas_NR5GSrvStatusTlv      var_NR5GSrvStatusinfo;
nas_NR5GSysInfoTlv        var_NR5GSysInfo;
nas_NR5GCellStatusInfoTlv var_NR5GCellStatusInfo;

unpack_nas_SLQSGetSysInfoV2_t var_unpack_nas_SLQSGetSysInfoV2_t = {
        &var_CDMASrvStatusInfo,&var_HDRSrvStatusInfo,&var_GSMSrvStatusInfo,&var_WCDMASrvStatusInfo,
        &var_LTESrvStatusInfo,&var_CDMASysInfo,&var_HDRSysInfo,&var_GSMSysInfo,&var_WCDMASysInfo,
        &var_LTESysInfo,&var_AddCDMASysInfo, &var_AddHDRSysInfo,&var_AddGSMSysInfo,&var_AddWCDMASysInfo,
        &var_AddLTESysInfo,&var_GSMCallBarringSysInfo,&var_WCDMACallBarringSysInfo,&var_LTEVoiceSupportSysInfo,
        &var_GSMCipherDomainSysInfo, &var_WCDMACipherDomainSysInfo, &var_CampedCiotLteOpMode,
        &var_LteEmbmsCoverage, &var_SimRejInfo, &var_ImsVoiceSupportLte, &var_LteVoiceDomain,
        &var_SrvRegRestriction, &var_LteRegDomain, &var_LteEmbmsTraceId, &var_NR5GSrvStatusinfo,
        &var_NR5GSysInfo, &var_NR5GCellStatusInfo,{{0}} };

nas_LteEmbmsCoverageTlv   cst_LteEmbmsCoverage = {0x00,0x01};
nas_SimRejInfoTlv         cst_SimRejInfo = { 0x01,0x01};
nas_ImsVoiceSupportLteTlv cst_ImsVoiceSupportLte = {0x00,0x01};
nas_LteVoiceDomainTlv     cst_LteVoiceDomain = {0x03,0x01};
nas_SrvRegRestrictionTlv  cst_SrvRegRestriction = {0x00,0x01};
nas_LteRegDomainTlv       cst_LteRegDomain = {0x00,0x01};
nas_LteEmbmsTraceIdTlv    cst_LteEmbmsTraceId = {0xFFFF,0x01};
nas_NR5GSrvStatusTlv      cst_NR5GSrvStatusinfo = {0x02,0x01,0x00,0x01};
nas_NR5GSysInfoTlv        cst_NR5GSysInfo = {0x01,0x01,0x01,0x03,0x01,0x02,0x01,0x00,0x01,0x1234,
                                              0x01,0x23456789,0x01,0x03,0x04,0x01,{0x11,0x22,0x33},{0x44,0x55,0x66},0x01,0xBBAA,0x01};
nas_NR5GCellStatusInfoTlv cst_NR5GCellStatusInfo = {0x03,0x01};

const unpack_nas_SLQSGetSysInfoV2_t const_unpack_nas_SLQSGetSysInfoV2_t = {
        &cst_CDMASrvStatusInfo,&cst_HDRSrvStatusInfo,&cst_GSMSrvStatusInfo,&cst_WCDMASrvStatusInfo,
        &cst_LTESrvStatusInfo,&cst_CDMASysInfo,&cst_HDRSysInfo,&cst_GSMSysInfo,&cst_WCDMASysInfo,
        &cst_LTESysInfo,&cst_AddCDMASysInfo, &cst_AddHDRSysInfo,&cst_AddGSMSysInfo,&cst_AddWCDMASysInfo,
        &cst_AddLTESysInfo,&cst_GSMCallBarringSysInfo,&cst_WCDMACallBarringSysInfo,&cst_LTEVoiceSupportSysInfo,
        &cst_GSMCipherDomainSysInfo, &cst_WCDMACipherDomainSysInfo, &cst_CampedCiotLteOpMode,
        &cst_LteEmbmsCoverage, &cst_SimRejInfo, &cst_ImsVoiceSupportLte, &cst_LteVoiceDomain,
        &cst_SrvRegRestriction, &cst_LteRegDomain, &cst_LteEmbmsTraceId,
        &cst_NR5GSrvStatusinfo,&cst_NR5GSysInfo,&cst_NR5GCellStatusInfo,
        {{SWI_UINT256_BIT_VALUE(SET_18_BITS,2,19,20,24,29,32,35,38,39,41,42,47,49,52,73,74,75,76)}} };

/* eQMI_NAS_SYS_INFO_IND */
unpack_nas_SLQSSysInfoCallback_ind_t var_unpack_nas_SLQSSysInfoCallback_ind_t;

const unpack_nas_SLQSSysInfoCallback_ind_t const_unpack_nas_SLQSSysInfoCallback_ind_t = {
        &cst_CDMASrvStatusInfo,&cst_HDRSrvStatusInfo,&cst_GSMSrvStatusInfo,&cst_WCDMASrvStatusInfo,
        &cst_LTESrvStatusInfo,&cst_CDMASysInfo,&cst_HDRSysInfo,&cst_GSMSysInfo,&cst_WCDMASysInfo,
        &cst_LTESysInfo,&cst_AddCDMASysInfo, &cst_AddHDRSysInfo,&cst_AddGSMSysInfo,&cst_AddWCDMASysInfo,
        &cst_AddLTESysInfo,&cst_GSMCallBarringSysInfo,&cst_WCDMACallBarringSysInfo,&cst_LTEVoiceSupportSysInfo,
        &cst_GSMCipherDomainSysInfo, &cst_WCDMACipherDomainSysInfo, &cst_SysInfoNoChange,
        &cst_LteCiotOpModeTlv,&cst_NR5GSerStatTlv,&cst_NR5GSystemInfoTlv,&cst_NR5GCellStatus,
        {{SWI_UINT256_BIT_VALUE(SET_17_BITS,19,20,24,29,32,35,36,40,41,46,48,51,72,75,76,77,78)}} };

/* eQMI_NAS_GET_SS_INFO */
const unpack_nas_GetServingNetwork_t const_unpack_nas_GetServingNetwork_t = {
        0x02, 0x02, 0x02, 0x02, 0x01, {0x05}, 0, 0x0194, 0x000E, 0, {0}, 2, {1,5},
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,1,2,16,17,18,21,33,38,39,40)}} };

const unpack_nas_SLQSGetServingSystem_t const_unpack_nas_SLQSGetServingSystem_t = {
        {0x02, 0x02, 0x02, 0x02, 0x01, {0x05}}, 0, 
         {2, {1,5}}, {0x0194, 0x000E, 0, {0}},0xFFFF,0xFFFF,0xFFFF,0xFFFFFFFF,0xFFFFFFFF,{1,{5},{0}},
         0xFF,{0xFF, 0xFF, 0xFF},0xFF,0xFF,0xFF,0xFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF,
         {1,3,0,0,0},{0xFFFF, 0xFF},0xFF,0xFFFF,{0xFFFFFFFF, 0xFFFFFFFF},
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,1,2,16,17,18,21,33,38,39,40)}} };

const unpack_nas_SLQSGetServingSystemV2_t const_unpack_nas_SLQSGetServingSystemV2_t = {
        {0x02, 0x02, 0x02, 0x02, 0x01, {0x05}}, 0,
         {2, {1,5}}, {0x0194, 0x000E, 0, {0}},0xFFFF,0xFFFF,0xFFFF,0xFFFFFFFF,0xFFFFFFFF,{1,{5},{0}},
         0xFF,{0xFF, 0xFF, 0xFF},0xFF,0xFF,0xFF,0xFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF,
         {1,3,0,0,0},{0xFFFF, 0xFF},0xFF,0xFFFF,{0xFFFFFFFF, 0xFFFFFFFF},{0x0194,0x000E,0x00,0x01},
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,1,2,16,17,18,21,33,38,39,40)}} };

/* eQMI_NAS_SS_INFO_IND */
const unpack_nas_SetServingSystemCallback_ind_t const_unpack_nas_SetServingSystemCallback_ind_t = {
        {0x02, 0x02, 0x02, 0x02, 0x01, {0x05},0},0,
        {{SWI_UINT256_BIT_VALUE(SET_9_BITS,1,16,17,18,21,33,38,39,40)}} };

const unpack_nas_SetDataCapabilitiesCallback_ind_t const_unpack_nas_SetDataCapabilitiesCallback_ind_t = {
        2, {1,5},
        {{SWI_UINT256_BIT_VALUE(SET_9_BITS,1,16,17,18,21,33,38,39,40)}} };

/* eQMI_NAS_SCAN_NETS */
uint8_t                    var_3GppNetworkInstanceSize = 4;
nas_QmiNas3GppNetworkInfo  var_3GppNetworkInfoInstances[4];
uint8_t                    var_RATInstanceSize = 4;
nas_QmiNas3GppNetworkRAT   var_RATINstance[4];
uint8_t                    var_PCSInstanceSize = 4;
nas_QmisNasPcsDigit        var_PCSInstance[4];
uint32_t                   var_ScanResult;
nas_QmisNasSlqsNasPCIInfo  var_PCIInfo;
nas_lteOpModeTlv           var_LteOpModeTlv;

unpack_nas_PerformNetworkScan_t var_unpack_nas_PerformNetworkScan_t = {
        &var_3GppNetworkInstanceSize,var_3GppNetworkInfoInstances,&var_RATInstanceSize,
        var_RATINstance,&var_PCSInstanceSize,var_PCSInstance,&var_ScanResult,&var_PCIInfo,
        &var_LteOpModeTlv,{{0}} };

uint8_t                    cst_3GppNetworkInstanceSize = 2;
nas_QmiNas3GppNetworkInfo  cst_3GppNetworkInfoInstances[2] = {
     {0x0194, 0x0002, (0xAA & 0x03),((0xAA >> 2) & 0x03),((0xAA >> 4) & 0x03),
      ((0xAA >> 6) & 0x03), {0x49,0x4E,0x44,0x20,0x61,0x69,0x72,0x74,0x65,0x6C}},
     {0x0194, 0x0002, (0xAA & 0x03),((0xAA >> 2) & 0x03),((0xAA >> 4) & 0x03),
      ((0xAA >> 6) & 0x03), {0x54,0x65,0x73,0x74,0x20,0x50,0x4C,0x4D,0x4E,0x20,0x31,0x2D,0x31}}};
uint8_t                    cst_RATInstanceSize = 2;
nas_QmiNas3GppNetworkRAT   cst_RATINstance[2] =
      {{0x0194, 0x0002,5},{0x0194, 0x0035,5}};
uint8_t                    cst_PCSInstanceSize = 2;
nas_QmisNasPcsDigit        cst_PCSInstance[2] =
      {{0x0194, 0x0002,1},{0x0194, 0x0035,0}};
uint32_t                   cst_ScanResult = 1;
nas_QmisNasSlqsNasPCIInfo  cst_PCIInfo;
nas_lteOpModeTlv           cst_LteOpModeTlv;

const unpack_nas_PerformNetworkScan_t const_unpack_nas_PerformNetworkScan_t = {
        &cst_3GppNetworkInstanceSize,cst_3GppNetworkInfoInstances,&cst_RATInstanceSize,
        cst_RATINstance,&cst_PCSInstanceSize,cst_PCSInstance,&cst_ScanResult,&cst_PCIInfo,
        &cst_LteOpModeTlv,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,22)}} };

nas_networkNameSrcTlv      var_NetworkNameSrcTlv;

unpack_nas_SLQSPerformNetworkScanV2_t var_unpack_nas_SLQSPerformNetworkScanV2_t = {
        &var_3GppNetworkInstanceSize,var_3GppNetworkInfoInstances,&var_RATInstanceSize,
        var_RATINstance,&var_PCSInstanceSize,var_PCSInstance,&var_ScanResult,&var_PCIInfo,
        &var_LteOpModeTlv,&var_NetworkNameSrcTlv,{{0}} };

nas_networkNameSrcTlv      cst_NetworkNameSrcTlv = {0x02,{0x04,0x05},0x01};
const unpack_nas_SLQSPerformNetworkScanV2_t const_unpack_nas_SLQSPerformNetworkScanV2_t = {
        &cst_3GppNetworkInstanceSize,cst_3GppNetworkInfoInstances,&cst_RATInstanceSize,
        cst_RATINstance,&cst_PCSInstanceSize,cst_PCSInstance,&cst_ScanResult,&cst_PCIInfo,
        &cst_LteOpModeTlv,&cst_NetworkNameSrcTlv,{{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,22)}} };

/* eQMI_NAS_SWI_GET_CQI */
const unpack_nas_SLQSSwiGetLteCQI_t const_unpack_nas_SLQSSwiGetLteCQI_t = {
        1,2,3,4,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };


/* eQMI_NAS_SWI_MODEM_STATUS  */
nas_LTEInfo  var_LTEInfo;
unpack_nas_SLQSNasSwiModemStatus_t var_unpack_nas_SLQSNasSwiModemStatus_t = {
        {0,0,0,0,0},&var_LTEInfo,{{0}} };

nas_LTEInfo  cst_LTEInfo = {1,2,0x0403, 0x0605,7,8,9};
const unpack_nas_SLQSNasSwiModemStatus_t const_unpack_nas_SLQSNasSwiModemStatus_t = {
        {0x1B,5,5,0,1},&cst_LTEInfo,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_NAS_SET_EVENT */
const unpack_nas_SLQSSetSignalStrengthsCallback_t const_unpack_nas_SLQSSetSignalStrengthsCallback_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_EVENT_IND */
const unpack_nas_SetEventReportInd_t const_unpack_nas_SetEventReportInd_t = {
        {1,4,4},{1,2,{1,2},{2,3},{3,4}},{1,2,0x0E01},
        {1,{{((~1) + 1),8},{((~(3 >> 1)) + 1),2},0x000000C8,4,{1,2},{3,8},{0},{0}}},
        {{SWI_UINT256_BIT_VALUE(SET_9_BITS,16,17,18,19,20,21,22,23,24)}} };

/* eQMI_NAS_GET_NET_PARAMS */
const unpack_nas_GetCDMANetworkParameters_t const_unpack_nas_GetCDMANetworkParameters_t = {
        1,2,1,1,1,1,1,2,3,4,2,
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,17,18,19,20,21,22)}} };

/* eQMI_NAS_GET_AAA_AUTH_STATUS */
uint32_t var_AuthStatus;
unpack_nas_GetANAAAAuthenticationStatus_t var_unpack_nas_GetANAAAAuthenticationStatus_t = {
        &var_AuthStatus,{{0}} };

uint32_t cst_AuthStatus = 1;
const unpack_nas_GetANAAAAuthenticationStatus_t const_unpack_nas_GetANAAAAuthenticationStatus_t = {
        &cst_AuthStatus,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_NAS_GET_ACCOLC */
uint8_t var_Accolc;
unpack_nas_GetACCOLC_t var_unpack_nas_GetACCOLC_t = {
        &var_Accolc,{{0}} };

uint8_t cst_Accolc = 1;
const unpack_nas_GetACCOLC_t const_unpack_nas_GetACCOLC_t = {
        &cst_Accolc,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_NAS_SET_ACCOLC */
const unpack_nas_SetACCOLC_t const_unpack_nas_SetACCOLC_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_CONFIG_SIG_INFO2 */
const unpack_nas_SLQSNasConfigSigInfo2_t const_unpack_nas_SLQSNasConfigSigInfo2_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_GET_TECH_PREF */
const unpack_nas_GetNetworkPreference_t const_unpack_nas_GetNetworkPreference_t = {
        2,1,2,0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_NAS_SET_TECH_PREF */
const unpack_nas_SetNetworkPreference_t const_unpack_nas_SetNetworkPreference_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_REGISTER_NET */
const unpack_nas_SLQSInitiateNetworkRegistration_t const_unpack_nas_SLQSInitiateNetworkRegistration_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_SWI_OTA_MESSAGE_INDICATION */
const unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t const_unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t = {
        {{1,4,5,{0x31,0x32,0x33,0x34,0x35}},{1,1,2,3},{1,0x0807060504000000ULL}},
         0,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,16,17)}} };

/* eQMI_NAS_SWI_INDICATION_REGISTER */
const unpack_nas_SLQSNasSwiIndicationRegister_t const_unpack_nas_SLQSNasSwiIndicationRegister_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_ATTACH_DETACH */
const unpack_nas_InitiateDomainAttach_t const_unpack_nas_InitiateDomainAttach_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_SET_NET_PARAMS */
const unpack_nas_SetCDMANetworkParameters_t const_unpack_nas_SetCDMANetworkParameters_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_SWI_SET_CHANNEL_LOCK */
const unpack_nas_SLQSNASSwiSetChannelLock_t const_unpack_nas_SLQSNASSwiSetChannelLock_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_LTE_CPHY_CA_IND */
const unpack_nas_SetNasLTECphyCaIndCallback_ind_t const_unpack_nas_SetNasLTECphyCaIndCallback_ind_t = {
    {257,802,2,1},
    {5,1},
    {258,770,3,120,1,1},
    {271,1010,4,152,1},
    {3,1},
    {0x1234,1},
    {0x5678,1},
    {{SWI_UINT256_BIT_VALUE(SET_7_BITS,1,16,17,18,19,20,21)}}
};

/* eQMI_NAS_GET_HDR_COLOR_CODE */
uint8_t var_color;
unpack_nas_SLQSNasGetHDRColorCode_t var_unpack_nas_SLQSNasGetHDRColorCode_t = {
        &var_color,{{0}} };

uint8_t cst_color = 1;
const unpack_nas_SLQSNasGetHDRColorCode_t const_unpack_nas_SLQSNasGetHDRColorCode_t = {
        &cst_color,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_NAS_SWI_PS_DETACH */
const unpack_nas_SLQSSwiPSDetach_t const_unpack_nas_SLQSSwiPSDetach_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_CONFIG_SIG_INFO */
const unpack_nas_SLQSConfigSigInfo_t const_unpack_nas_SLQSConfigSigInfo_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_SET_EDRX_PARAMS */
const unpack_nas_SLQSNASSeteDRXParams_t const_unpack_nas_SLQSNASSeteDRXParams_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_NAS_GET_PLMN_NAME */
const unpack_nas_SLQSGetPLMNName_t const_unpack_nas_SLQSGetPLMNName_t = {
        0, 0x0B, {0x56,0x6F,0x64,0x61,0x66,0x6F,0x6E,0x65,0x20,0x49,0x4E},
        0,0,0, 4,{0x41,0x54,0x26,0x54},0,0,0,4, {0x41,0x54,0x26,0x54},
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2, 16)}} };

/* eQMI_NAS_GET_CELL_LOCATION_INFO */
nas_GERANInfo                var_GERANInfo;
nas_UMTSInfo                 var_UMTSInfo;
nas_CDMAInfo                 var_CDMAInfo;
nas_LTEInfoIntrafreq         var_LTEInfoIntrafreq;
nas_LTEInfoInterfreq         var_LTEInfoInterfreq;
nas_LTEInfoNeighboringGSM    var_LTEInfoNeighboringGSM;
nas_LTEInfoNeighboringWCDMA  var_LTEInfoNeighboringWCDMA;
uint32_t                     var_UMTSCellID;
nas_WCDMAInfoLTENeighborCell var_WCDMAInfoLTENeighborCell;
unpack_nas_SLQSNasGetCellLocationInfo_t var_unpack_nas_SLQSNasGetCellLocationInfo_t = {
        &var_GERANInfo, &var_UMTSInfo, &var_CDMAInfo, &var_LTEInfoIntrafreq,
        &var_LTEInfoInterfreq, &var_LTEInfoNeighboringGSM,&var_LTEInfoNeighboringWCDMA,
        &var_UMTSCellID,&var_WCDMAInfoLTENeighborCell,{{0}} };

nas_GERANInfo                cst_GERANInfo;
nas_UMTSInfo                 cst_UMTSInfo = 
         {1,{0x04, 0xF4, 0x41}, 0x042D,0x872D,0x00AD, 0x059A, 0x01EC,1,
         {{0x1207, 0x3A0E, 0xEC03, 0xC401}}, 0,{{0,0,0,0}}};
nas_CDMAInfo                 cst_CDMAInfo;
nas_LTEInfoIntrafreq         cst_LTEInfoIntrafreq = 
        {1,{4,0xF4,0x41}, 0x042D,0x00AD872D,0x059A,0x01EC,0x07, 0x12, 0x0E, 0x3A,3,
        {{0x01EC,0xFFC4,0xFD8C,0xFE83,0x0041},{ 0x00D9,0xFF38,0xFC81,0xFE41,0x0000},
        {0x00DA, 0xFF38, 0xFC7A, 0xFE42, 0x0000}}};
nas_LTEInfoInterfreq         cst_LTEInfoInterfreq = 
        {1,1,{{1,2,3,4,0,{{0,0,0,0,0}}}}};

nas_LTEInfoNeighboringGSM    cst_LTEInfoNeighboringGSM = 
        {1,1, {{0x01, 0x02, 0x03, 0x04, 0x01, {{0x0002,3,4,5,0x0006, 0x0007}}}}};
nas_LTEInfoNeighboringWCDMA  cst_LTEInfoNeighboringWCDMA =
        {1,0,{{0,0,0,0,0,{{0,0,0,0}} }} };
uint32_t                     cst_UMTSCellID = 4;
nas_WCDMAInfoLTENeighborCell cst_WCDMAInfoLTENeighborCell;
const unpack_nas_SLQSNasGetCellLocationInfo_t const_unpack_nas_SLQSNasGetCellLocationInfo_t = {
        &cst_GERANInfo, &cst_UMTSInfo, &cst_CDMAInfo, &cst_LTEInfoIntrafreq,
        &cst_LTEInfoInterfreq, &cst_LTEInfoNeighboringGSM,&cst_LTEInfoNeighboringWCDMA,
        &cst_UMTSCellID,&cst_WCDMAInfoLTENeighborCell,
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,17,19,20,21,22,23,28,34,41)}} };

nas_WCDMACellInfoExt         var_WCDMACellInfoExt;
nas_UMTSExtInfo              var_UMTSExtInfo;
nas_LteEarfcnInfo            var_LteEarfcnInfo;
unpack_nas_SLQSNasGetCellLocationInfoV2_t var_unpack_nas_SLQSNasGetCellLocationInfoV2_t = {
        &var_GERANInfo, &var_UMTSInfo, &var_CDMAInfo, &var_LTEInfoIntrafreq,
        &var_LTEInfoInterfreq, &var_LTEInfoNeighboringGSM,&var_LTEInfoNeighboringWCDMA,
        &var_UMTSCellID,&var_WCDMAInfoLTENeighborCell,&var_WCDMACellInfoExt,&var_UMTSExtInfo,
        &var_LteEarfcnInfo,{{0}} };

nas_WCDMACellInfoExt         cst_WCDMACellInfoExt = {-65.0,-1.0,0xFFFF,0x01};
nas_UMTSExtInfo              cst_UMTSExtInfo = {0xe1ce,{0x04,0xF4,0x41}, 0x90cd,0x2a05,0x01c7,0xffbc,0xfffd,0x000f,0x002e,
                                                0x00,{{0,0,0,0,0,0,0,0}},0x00,{{0,0,0,0,0}},0x01 };
nas_LteEarfcnInfo            cst_LteEarfcnInfo = {0,{0},0x01};
const unpack_nas_SLQSNasGetCellLocationInfoV2_t const_unpack_nas_SLQSNasGetCellLocationInfoV2_t = {
        &cst_GERANInfo, &cst_UMTSInfo, &cst_CDMAInfo, &cst_LTEInfoIntrafreq,
        &cst_LTEInfoInterfreq, &cst_LTEInfoNeighboringGSM,&cst_LTEInfoNeighboringWCDMA,
        &cst_UMTSCellID,&cst_WCDMAInfoLTENeighborCell,&cst_WCDMACellInfoExt,&cst_UMTSExtInfo,
        &cst_LteEarfcnInfo,{{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,17,19,20,21,22,23,28,34,41)}} };

/* eQMI_NAS_GET_NETWORK_TIME */
nas_timeInfo                var_3GPP2TimeInfo;
nas_timeInfo                var_3GPPTimeInfo;
unpack_nas_SLQSGetNetworkTime_t var_unpack_nas_SLQSGetNetworkTime_t = {
        &var_3GPP2TimeInfo, &var_3GPPTimeInfo,{{0}} };

nas_timeInfo                cst_3GPPTimeInfo = {0x07E3,11,1,1,1,1,1,1,0,0,1};
const unpack_nas_SLQSGetNetworkTime_t const_unpack_nas_SLQSGetNetworkTime_t = {
        NULL, &cst_3GPPTimeInfo, 
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,17)}} };

/* eQMI_NAS_NETWORK_TIME_IND */
uint8_t              var_TimeZone;
uint8_t              var_DayltSavAdj;
uint8_t              var_RadioInterface;
unpack_nas_SLQSNasNetworkTimeCallBack_ind_t var_unpack_nas_SLQSNasNetworkTimeCallBack_ind_t = {
        {0,0,0,0,0,0,0},&var_TimeZone, &var_DayltSavAdj,&var_RadioInterface,{{0}} };

uint8_t              cst_TimeZone = 1;
uint8_t              cst_DayltSavAdj = 2;
uint8_t              cst_RadioInterface = 5;
const unpack_nas_SLQSNasNetworkTimeCallBack_ind_t const_unpack_nas_SLQSNasNetworkTimeCallBack_ind_t = {
        {0x07E3, 0x0B, 0x01, 0x01, 0x01, 0x01, 0x01}, &cst_TimeZone,
         &cst_DayltSavAdj, &cst_RadioInterface,
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,16,17,18)}} };

/* eQMI_NAS_SWI_GET_LTE_SCC_RX_INFO */
nas_SccRxInfo var_SccRxInfo;
unpack_nas_SLQSSwiGetLteSccRxInfo_t var_unpack_nas_SLQSSwiGetLteSccRxInfo_t = {
        &var_SccRxInfo, {{0}} };

nas_SccRxInfo cst_SccRxInfo = {0x000000E3, 0x00D2,1,{{1,2,3,4}},1};
const unpack_nas_SLQSSwiGetLteSccRxInfo_t const_unpack_nas_SLQSSwiGetLteSccRxInfo_t = {
        &cst_SccRxInfo, 
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_NAS_SWI_NETWORK_TIMERS_IND */
const unpack_nas_SLQSNasTimerCallback_ind_t const_unpack_nas_SLQSNasTimerCallback_ind_t = {
        {0x56,0x6F,0x64,0x61,0x66,0x6F,0x6E},{0x41,0x54,0x26},0x01020000,
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };


/* eQMI_NAS_GET_TX_RX_INFO */
nas_rxInfo              var_RXChain0Info;
nas_rxInfo              var_RXChain1Info;
nas_txInfo              var_TXInfo;
unpack_nas_SLQSNasGetTxRxInfo_t var_unpack_nas_SLQSNasGetTxRxInfo_t = {
        &var_RXChain0Info,&var_RXChain1Info, &var_TXInfo,{{0}} };

nas_rxInfo              cst_RXChain0Info = 
           {1, 0x00000201, 0x00000403, 0x00000605, 0x00000807, 0x00000A09};
nas_rxInfo              cst_RXChain1Info = 
           {1, 0x00000A09, 0x00000807, 0x00000605, 0x00000807, 0x00000A09};
nas_txInfo              cst_TXInfo = {1, 0x00000201};
const unpack_nas_SLQSNasGetTxRxInfo_t const_unpack_nas_SLQSNasGetTxRxInfo_t = {
        &cst_RXChain0Info,&cst_RXChain1Info, &cst_TXInfo,
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

/* eQMI_NAS_GET_OPERATOR_NAME */
nas_serviceProviderName var_SrvcProviderName;
nas_operatorPLMNList    var_OperatorPLMNList;
nas_PLMNNetworkName     var_PLMNNetworkName;
nas_operatorNameString  var_OperatorNameString;
nas_PLMNNetworkNameData var_NITZInformation;
unpack_nas_SLQSGetOperatorNameData_t var_unpack_nas_SLQSGetOperatorNameData_t = {
        &var_SrvcProviderName,&var_OperatorPLMNList, &var_PLMNNetworkName,
        &var_OperatorNameString,&var_NITZInformation,{{0}} };

nas_serviceProviderName cst_SrvcProviderName = 
         {1,4,{0x6E, 0x41, 0x54, 0x41}};
nas_operatorPLMNList    cst_OperatorPLMNList = 
         {1, {{{0x31, 0x32, 0x33},{0x34, 0x35, 0x36}, 0x0001, 0x0002, 0x01}}};
nas_PLMNNetworkName     cst_PLMNNetworkName =
         {1,{{0,0,1,1,4,{0x55, 0x56, 0x57, 0x58}, 4, {0x55, 0x56, 0x57, 0x58}}}};
nas_operatorNameString  cst_OperatorNameString = {{0x41, 0x42, 0x43, 0x44}};
nas_PLMNNetworkNameData cst_NITZInformation = 
         {0,0,1,1,4,{0x55, 0x56, 0x57, 0x58}, 4, {0x55, 0x56, 0x57, 0x58}};
const unpack_nas_SLQSGetOperatorNameData_t const_unpack_nas_SLQSGetOperatorNameData_t = {
        &cst_SrvcProviderName,&cst_OperatorPLMNList, &cst_PLMNNetworkName,
        &cst_OperatorNameString,&cst_NITZInformation,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_NAS_GET_3GPP2_SUBSCRIPTION */
nas_namName      var_NAMNameInfo;
nas_dirNum       var_DirNum;
nas_homeSIDNID   var_HomeSIDNID;
nas_minBasedIMSI var_MinBasedIMSI;
nas_trueIMSI     var_TrueIMSI;
nas_CDMAChannel  var_CDMAChannel;
nas_Mdn      var_Mdn;
unpack_nas_SLQSNasGet3GPP2Subscription_t var_unpack_nas_SLQSNasGet3GPP2Subscription_t = {
        &var_NAMNameInfo,&var_DirNum, &var_HomeSIDNID,
        &var_MinBasedIMSI,&var_TrueIMSI,&var_CDMAChannel,&var_Mdn,{{0}} };

nas_namName      cst_NAMNameInfo = {4, {0x6E, 0x41, 0x54, 0x41}} ;
nas_dirNum       cst_DirNum = {4, {0x41, 0x42, 0x43, 0x44}};
nas_homeSIDNID   cst_HomeSIDNID = {2, {{0x0031, 0x0032}, {0x0033, 0x0034}}};
nas_minBasedIMSI cst_MinBasedIMSI = {{0x31, 0x32, 0x33}, 2, {0x57, 0x58, 0x59, 
        0x55, 0x56, 0x57, 0x58},{0x57, 0x58, 0x59}};
nas_trueIMSI     cst_TrueIMSI = {{0x31, 0x32, 0x33}, 2, {0x57, 0x58, 0x59, 
        0x55, 0x56, 0x57, 0x58},{0x57, 0x58, 0x59},1};
nas_CDMAChannel  cst_CDMAChannel = {1,2,3,4};
nas_Mdn      cst_Mdn = {0x0A,{'9', '8','1', '1','0', '1','2', '3','4', '5'}};

const unpack_nas_SLQSNasGet3GPP2Subscription_t const_unpack_nas_SLQSNasGet3GPP2Subscription_t = {
        &cst_NAMNameInfo,&cst_DirNum, &cst_HomeSIDNID,
        &cst_MinBasedIMSI,&cst_TrueIMSI,&cst_CDMAChannel,&cst_Mdn,
        {{SWI_UINT256_BIT_VALUE(SET_8_BITS,2,16,17,18,19,20,21,22)}} };

/* eQMI_NAS_SWI_HDR_PERSONALITY */
uint16_t                   var_CurrentPersonality;
uint8_t                    var_PersonalityListLength;
nas_protocolSubtypeElement var_ProtocolSubtypeElement[3];
unpack_nas_SLQSSwiGetHDRPersonality_t var_unpack_nas_SLQSSwiGetHDRPersonality_t = {
        &var_CurrentPersonality,&var_PersonalityListLength, var_ProtocolSubtypeElement,
        {{0}} };

uint16_t                   cst_CurrentPersonality = 0;
uint8_t                    cst_PersonalityListLength = 1;
nas_protocolSubtypeElement cst_ProtocolSubtypeElement[3] = {{1,2,3,4,5,6,7,8,9,10,11,12}};
const unpack_nas_SLQSSwiGetHDRPersonality_t const_unpack_nas_SLQSSwiGetHDRPersonality_t = {
        &cst_CurrentPersonality,&cst_PersonalityListLength, cst_ProtocolSubtypeElement,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

uint16_t                   var_CurrentPersonalityInd;
uint8_t                    var_PersonalityListLengthInd;
nas_protocolSubtypeElement var_ProtocolSubtypeElementInd[3];
nas_HDRPersonality_Ind_Data var_HDRPersInd = {
            &var_CurrentPersonalityInd,&var_PersonalityListLengthInd, 
            var_ProtocolSubtypeElementInd };

unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t var_unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t = {
        &var_HDRPersInd, {{0}} };

uint16_t                   cst_CurrentPersonalityInd = 0;
uint8_t                    cst_PersonalityListLengthInd = 1;
nas_protocolSubtypeElement cst_ProtocolSubtypeElementInd[3] = {{1,2,3,4,5,6,7,8,9,10,11,12}};

nas_HDRPersonality_Ind_Data cst_HDRPersInd = {
            &cst_CurrentPersonalityInd,&cst_PersonalityListLengthInd, 
            cst_ProtocolSubtypeElementInd };
const unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t const_unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t = {
        &cst_HDRPersInd,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,16,17)}} };

/* eQMI_NAS_SWI_HDR_PROT_SUBTYPE */
uint16_t                   var_CurrentPrsnlty;
uint8_t                    var_PersonalityListLength_2;
nas_protocolSubtypeElement var_ProtoSubTypElmnt[2];
uint64_t                   var_AppSubType;
unpack_nas_SLQSSwiGetHDRProtSubtype_t var_unpack_nas_SLQSSwiGetHDRProtSubtype_t = {
        &var_CurrentPrsnlty,&var_PersonalityListLength_2, var_ProtoSubTypElmnt,
        &var_AppSubType, {{0}} };

uint16_t                   cst_CurrentPrsnlty = 2;
uint8_t                    cst_PersonalityListLength_2 = 1;
nas_protocolSubtypeElement cst_ProtoSubTypElmnt[2] = {{1,2,3,4,5,6,7,8,9,10,11,12}};
uint64_t                   cst_AppSubType = 2;
const unpack_nas_SLQSSwiGetHDRProtSubtype_t const_unpack_nas_SLQSSwiGetHDRProtSubtype_t = {
        &cst_CurrentPrsnlty,&cst_PersonalityListLength_2, cst_ProtoSubTypElmnt,
        &cst_AppSubType,
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

/* eQMI_NAS_GET_ERR_RATE */
uint16_t var_CDMAFrameErrRate;
uint16_t var_HDRPackErrRate;
uint8_t  var_GSMBER;
uint8_t  var_WCDMABER;
unpack_nas_SLQSGetErrorRate_t var_unpack_nas_SLQSGetErrorRate_t = {
        &var_CDMAFrameErrRate,&var_HDRPackErrRate, &var_GSMBER,
        &var_WCDMABER, {{0}} };

uint16_t cst_CDMAFrameErrRate = 0x0102;
uint16_t cst_HDRPackErrRate = 0x0304;
uint8_t  cst_GSMBER = 4;
uint8_t  cst_WCDMABER = 8;
const unpack_nas_SLQSGetErrorRate_t const_unpack_nas_SLQSGetErrorRate_t = {
        &cst_CDMAFrameErrRate,&cst_HDRPackErrRate, &cst_GSMBER,
        &cst_WCDMABER,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,2,16,17,18,19)}} };

/* eQMI_NAS_SWI_GET_HRPD_STATS */
nas_DRCParams    var_DRCParams;
uint8_t          var_UATI[16];
nas_PilotSetParams cst_PilotSetInfo[2];
nas_PilotSetData var_PilotSetData = {2,cst_PilotSetInfo} ;
unpack_nas_SLQSSwiGetHRPDStats_t var_unpack_nas_SLQSSwiGetHRPDStats_t = {
        &var_DRCParams,var_UATI, &var_PilotSetData, {{0}} };

nas_DRCParams    cst_DRCParams = {2,1};
uint8_t          cst_UATI[16] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
       0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50};
nas_PilotSetParams cst_PilotSetInfo[2] = {{1,2,3}};
nas_PilotSetData cst_PilotSetData = {1, cst_PilotSetInfo};;
const unpack_nas_SLQSSwiGetHRPDStats_t const_unpack_nas_SLQSSwiGetHRPDStats_t = {
        &cst_DRCParams, cst_UATI, &cst_PilotSetData,
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };


/* eQMI_NAS_SWI_NETWORK_DEBUG */
uint8_t                var_ObjectVer;
nas_ActPilotPNElement  var_ActPilotPNElement[2];
uint16_t               var_NeighborSetPilotPN[2];
nas_NetworkStat1x      var_NetworkStat1x = 
       {0,0,0, 0,0, 0, var_ActPilotPNElement, 0, var_NeighborSetPilotPN };
uint16_t  var_SectorID[2];
nas_NetworkStatEVDO    var_NetworkStatEVDO = {0, 0,0, var_SectorID, 0, 0,0, 0};
nas_DeviceConfigDetail var_DeviceConfigDetail;
nas_DataStatusDetail   var_DataStatusDetail;

unpack_nas_SLQSSwiNetworkDebug_t var_unpack_nas_SLQSSwiNetworkDebug_t = {
        &var_ObjectVer,&var_NetworkStat1x, &var_NetworkStatEVDO,
        &var_DeviceConfigDetail, &var_DataStatusDetail, {{0}} };

uint8_t                cst_ObjectVer = 2;
nas_ActPilotPNElement  cst_ActPilotPNElement[2] = {{4,1}};
uint16_t               cst_NeighborSetPilotPN[2] = {1,2};
nas_NetworkStat1x      cst_NetworkStat1x = 
          {1,3,0x12340056, 0x1234, 0x12340056, 1, cst_ActPilotPNElement, 2, cst_NeighborSetPilotPN};
uint16_t  cst_SectorID[2] = {1};
nas_NetworkStatEVDO    cst_NetworkStatEVDO = {1, 0x12,1, cst_SectorID, 0x1234, 1,2, 1};
nas_DeviceConfigDetail cst_DeviceConfigDetail = {1,2,3,4};
nas_DataStatusDetail   cst_DataStatusDetail = {0x12345678,1};

const unpack_nas_SLQSSwiNetworkDebug_t const_unpack_nas_SLQSSwiNetworkDebug_t = {
        &cst_ObjectVer,&cst_NetworkStat1x, &cst_NetworkStatEVDO,
        &cst_DeviceConfigDetail, &cst_DataStatusDetail,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_NAS_SWI_GET_CHANNEL_LOCK */
nas_wcdmaUARFCN  var_WcdmaUARFCN;
nas_lteEARFCN   var_LteEARFCN;
nas_ltePCI      var_LtePCI;
unpack_nas_SLQSNASSwiGetChannelLock_t var_unpack_nas_SLQSNASSwiGetChannelLock_t = {
        &var_WcdmaUARFCN,&var_LteEARFCN, &var_LtePCI, {{0}} };

nas_wcdmaUARFCN  cst_WcdmaUARFCN = {1, 0x00000102};
nas_lteEARFCN   cst_LteEARFCN = {1, 0x00000102, 0x00000304};
nas_ltePCI      cst_LtePCI = {1,0x00000506, 0x00000708};
const unpack_nas_SLQSNASSwiGetChannelLock_t const_unpack_nas_SLQSNASSwiGetChannelLock_t = {
        &cst_WcdmaUARFCN,&cst_LteEARFCN, &cst_LtePCI, 
        {{SWI_UINT256_BIT_VALUE(SET_4_BITS,2,16,17,18)}} };

/* eQMI_NAS_GET_EDRX_PARAMS */
uint8_t      var_CycleLen;
uint8_t      var_PagingTimeWindow;
uint8_t      var_EdrxEnable;
uint8_t      var_EdrxRAT;
uint32_t     var_LteOpMode;
unpack_nas_SLQSNASGeteDRXParamsExt_t var_unpack_nas_SLQSNASGeteDRXParamsExt_t = {
        &var_CycleLen,&var_PagingTimeWindow, &var_EdrxEnable, &var_EdrxRAT, &var_LteOpMode,
        {{0}} };

uint8_t      cst_CycleLen = 2;
uint8_t      cst_PagingTimeWindow = 3;
uint8_t      cst_EdrxEnable = 4;
uint8_t      cst_EdrxRAT = 5;
uint32_t     cst_LteOpMode = 0x0102;
const unpack_nas_SLQSNASGeteDRXParamsExt_t const_unpack_nas_SLQSNASGeteDRXParamsExt_t = {
        &cst_CycleLen,&cst_PagingTimeWindow, &cst_EdrxEnable, &cst_EdrxRAT, &cst_LteOpMode,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_NAS_SWI_RANK_INDICATOR_IND */
const unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t const_unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t = {
        {1,3,4},{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_NAS_EDRX_CHANGE_INFO_IND */
const unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t const_unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t = {
        {1,1}, {2,1}, {3,1}, {4,1}, {2,1},{{SWI_UINT256_BIT_VALUE(SET_5_BITS,16,17,18,19,20)}} };

/* eQMI_NAS_NETWORK_REJECT_IND */
nas_PlmnID var_PlmnId;
nas_CsgId var_CsgId;
nas_LteOpMode var_LteOpModeInd;
unpack_nas_SLQSNasNetworkRejectCallback_Ind_t var_unpack_nas_SLQSNasNetworkRejectCallback_Ind_t = {
        0,0,0,&var_PlmnId,&var_CsgId, &var_LteOpModeInd, {{0}} };

nas_PlmnID cst_PlmnId = {0x0044, 0x0152,1,1};
nas_CsgId cst_CsgId = {0x04030201,1};
nas_LteOpMode cst_LteOpModeInd = {3,1};
const unpack_nas_SLQSNasNetworkRejectCallback_Ind_t const_unpack_nas_SLQSNasNetworkRejectCallback_Ind_t = {
         8,2,5,&cst_PlmnId,&cst_CsgId, &cst_LteOpModeInd,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,1,2,3,16,17,18)}} };

/* eQMI_NAS_GET_NET_BAN_LIST */
nas_ForbiddenNetworks3GPP var_ForbiddenNetworks3GPP;
unpack_nas_SLQSNASGetForbiddenNetworks_t var_unpack_nas_SLQSNASGetForbiddenNetworks_t = {
        &var_ForbiddenNetworks3GPP,
        {{0}} };

nas_ForbiddenNetworks3GPP cst_ForbiddenNetworks3GPP = {1, {0x0102}, {0x0028},1};
const unpack_nas_SLQSNASGetForbiddenNetworks_t const_unpack_nas_SLQSNASGetForbiddenNetworks_t = {
        &cst_ForbiddenNetworks3GPP,
        {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16)}} };

/* eQMI_NAS_GET_LTE_CPHY_CA_INFO */
const unpack_nas_SlqsGetLTECphyCAInfo_t const_unpack_nas_SlqsGetLTECphyCAInfo_t = {
    {
        {.pci=257,.freq=802,.scell_state=2,.TlvPresent=1},
        {.dl_bw_value=5,.TlvPresent=1},
        {.pci=258,.freq=770,.dl_bw_value=3,.iLTEbandValue=120,.scell_state=1,.TlvPresent=1},
        {.pci=271,.freq=1010,.dl_bw_value=4,.iLTEbandValue=152,.TlvPresent=1},
        {.scell_idx=3,.TlvPresent=1},
        {.cphy_scell_info_list_len=3,
         .pci={100,1823,300,SET_252_ZERO},
         .freq={8721,17459,26197,SET_252_ZERO},
         .cphy_ca_dl_bandwidth={0,2,5,SET_252_ZERO},
         .band={120,152,168,SET_252_ZERO},
         .scell_state={0,2,1,SET_252_ZERO},
         .scell_idx={7,4,0,SET_252_ZERO},
         .TlvPresent=1},
    },
    0,
    {{SWI_UINT256_BIT_VALUE(SET_7_BITS,2,16,17,18,19,20,21)}} 
};

/* eQMI_NAS_RF_BAND_INFO_IND */
const unpack_nas_SLQSNasRFBandInfoCallback_Ind_t const_unpack_nas_SLQSNasRFBandInfoCallback_Ind_t = {
        {0x08,0x0050,0x059A,0x01},{0x08,0x0050,0x01}, {0x08,0x0050,0x059A,0x01},{0x08,0x04,0x01}, {0x01,0x01},
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,16,17,18,19)}} };

int nas_validate_dummy_uppack()
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
    loopCount = sizeof(validate_nas_resp_msg)/sizeof(validate_nas_resp_msg[0]);
    while(loopCount)
    {
         memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_nas_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_NAS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("NAS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("NAS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
                case eQMI_NAS_GET_RSSI:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_GetSignalStrengths,
                            dump_GetSignalStrengths,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_GetSignalStrengths_t);

                        printf("\n\n");

                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SLQSGetSignalStrength,
                            dump_SLQSGetSignalStrength,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_SLQSGetSignalStrength_t);
                    }
                    break;

                case eQMI_NAS_GET_SYS_SELECT_PREF:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSGetSysSelectionPref_t *varp = &var_unpack_nas_SLQSGetSysSelectionPref_t;
                        const unpack_nas_SLQSGetSysSelectionPref_t *cstp = &const_unpack_nas_SLQSGetSysSelectionPref_t;

                        unpack_nas_SLQSGetSysSelectionPrefExt_t *varpext = &var_unpack_nas_SLQSGetSysSelectionPrefExt_t;
                        const unpack_nas_SLQSGetSysSelectionPrefExt_t *cstpext = &const_unpack_nas_SLQSGetSysSelectionPrefExt_t;

                        unpack_nas_SLQSGetSysSelectionPrefExtV2_t *varpextv2 = &var_unpack_nas_SLQSGetSysSelectionPrefExtV2_t;
                        const unpack_nas_SLQSGetSysSelectionPrefExtV2_t *cstpextv2 = &const_unpack_nas_SLQSGetSysSelectionPrefExtV2_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetSysSelectionPref,
                        dump_SLQSGetSysSelectionPref,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetSysSelectionPref_t,
                        9,
                        CMP_PTR_TYPE, varp->pEmerMode, cstp->pEmerMode,
                        CMP_PTR_TYPE, varp->pModePref, cstp->pModePref,
                        CMP_PTR_TYPE, varp->pBandPref, cstp->pBandPref,
                        CMP_PTR_TYPE, varp->pRoamPref, cstp->pRoamPref,
                        CMP_PTR_TYPE, varp->pLTEBandPref, cstp->pLTEBandPref,
                        CMP_PTR_TYPE, varp->pNetSelPref, cstp->pNetSelPref,
                        CMP_PTR_TYPE, varp->pSrvDomainPref, cstp->pSrvDomainPref,
                        CMP_PTR_TYPE, varp->pGWAcqOrderPref, cstp->pGWAcqOrderPref,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );

                        printf("\n\n");

                        UNPACK_RESP_AND_DUMPCOMP_NO_MASK(unpackRetCode,
                        unpack_nas_SLQSGetSysSelectionPrefExt,
                        dump_SLQSGetSysSelectionPrefExtSettings,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetSysSelectionPrefExt_t,
                        13,
                        CMP_PTR_TYPE, varpext->pEmerMode, cstpext->pEmerMode,
                        CMP_PTR_TYPE, varpext->pModePref, cstpext->pModePref,
                        CMP_PTR_TYPE, varpext->pBandPref, cstpext->pBandPref,
                        CMP_PTR_TYPE, varpext->pRoamPref, cstpext->pRoamPref,
                        CMP_PTR_TYPE, varpext->pLTEBandPref, cstpext->pLTEBandPref,
                        CMP_PTR_TYPE, varpext->pNetSelPref, cstpext->pNetSelPref,
                        CMP_PTR_TYPE, varpext->pSrvDomainPref, cstpext->pSrvDomainPref,
                        CMP_PTR_TYPE, varpext->pGWAcqOrderPref, cstpext->pGWAcqOrderPref,
                        CMP_PTR_TYPE, &varpext->pAcqOrderPref->acqOrdeLen, &cstpext->pAcqOrderPref->acqOrdeLen,
                        CMP_PTR_TYPE, varpext->pAcqOrderPref->pAcqOrder, cstpext->pAcqOrderPref->pAcqOrder,
                        CMP_PTR_TYPE, varpext->pRatDisabledMask, cstpext->pRatDisabledMask,
                        CMP_PTR_TYPE, varpext->pLteM1BandPref, cstpext->pLteM1BandPref,
                        CMP_PTR_TYPE, varpext->pLteNb1BandPref, cstpext->pLteNb1BandPref,
                        );

                        printf("\n\n");

                        UNPACK_RESP_AND_DUMPCOMP_NO_MASK(unpackRetCode,
                        unpack_nas_SLQSGetSysSelectionPrefExtV2,
                        dump_SLQSGetSysSelectionPrefExtV2,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetSysSelectionPrefExtV2_t,
                        19,
                        CMP_PTR_TYPE, varpextv2->pEmerMode, cstpextv2->pEmerMode,
                        CMP_PTR_TYPE, varpextv2->pModePref, cstpextv2->pModePref,
                        CMP_PTR_TYPE, varpextv2->pBandPref, cstpextv2->pBandPref,
                        CMP_PTR_TYPE, varpextv2->pRoamPref, cstpextv2->pRoamPref,
                        CMP_PTR_TYPE, varpextv2->pLTEBandPref, cstpextv2->pLTEBandPref,
                        CMP_PTR_TYPE, varpextv2->pNetSelPref, cstpextv2->pNetSelPref,
                        CMP_PTR_TYPE, varpextv2->pSrvDomainPref, cstpextv2->pSrvDomainPref,
                        CMP_PTR_TYPE, varpextv2->pGWAcqOrderPref, cstpextv2->pGWAcqOrderPref,
                        CMP_PTR_TYPE, &varpextv2->pAcqOrderPref->acqOrdeLen, &cstpextv2->pAcqOrderPref->acqOrdeLen,
                        CMP_PTR_TYPE, varpextv2->pAcqOrderPref->pAcqOrder, cstpextv2->pAcqOrderPref->pAcqOrder,
                        CMP_PTR_TYPE, varpextv2->pRatDisabledMask, cstpextv2->pRatDisabledMask,
                        CMP_PTR_TYPE, varpextv2->pLteM1BandPref, cstpextv2->pLteM1BandPref,
                        CMP_PTR_TYPE, varpextv2->pLteNb1BandPref, cstpextv2->pLteNb1BandPref,
                        CMP_PTR_TYPE, varpextv2->pLTEBandPrefExt, cstpextv2->pLTEBandPrefExt,
                        CMP_PTR_TYPE, varpextv2->pTDSCDMABandPref, cstpextv2->pTDSCDMABandPref,
                        CMP_PTR_TYPE, varpextv2->pSrvRegRestric, cstpextv2->pSrvRegRestric,
                        CMP_PTR_TYPE, varpextv2->pUsageSetting, cstpextv2->pUsageSetting,
                        CMP_PTR_TYPE, varpextv2->pVoiceDomainPref, cstpextv2->pVoiceDomainPref,
                        CMP_PTR_TYPE, &varpextv2->ParamPresenceMask, &cstpextv2->ParamPresenceMask,
                        );
                    }
                    else if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSetSysSelectionPrefCallBack_ind,
                        displayNasSystemSelectPrefInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t);
                    }
                    break;

                case eQMI_NAS_SET_SYS_SELECT_PREF:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSetSysSelectionPref,
                        dump_SLQSSetSysSelectionPref,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSSetSysSelectionPref_t);
                    }
                    break;
                case eQMI_NAS_SET_REG_EVENT:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasIndicationRegisterExt,
                        dump_SLQSNasIndicationRegisterExt,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasIndicationRegisterExt_t);
                    }
                    break;
                case eQMI_NAS_GET_RF_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasGetRFInfo_t *varp = &var_unpack_nas_SLQSNasGetRFInfo_t;
                        const unpack_nas_SLQSNasGetRFInfo_t *cstp = &const_unpack_nas_SLQSNasGetRFInfo_t;

                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_GetRFInfo,
                        dump_GetRFInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_GetRFInfo_t);

                        printf("\n\n");

                        UNPACK_RESP_AND_DUMPCOMP_NO_MASK(unpackRetCode,
                        unpack_nas_SLQSNasGetRFInfo,
                        dump_SLQSNasGetRFInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGetRFInfo_t,
                        9,
                        CMP_PTR_TYPE, varp->rfbandInfoList.pInstanceSize, cstp->rfbandInfoList.pInstanceSize,
                        CMP_PTR_TYPE, varp->rfbandInfoList.pRFBandInfoParam, cstp->rfbandInfoList.pRFBandInfoParam,
                        CMP_PTR_TYPE, varp->pRfDedicatedBandInfo->pInstancesSize, cstp->pRfDedicatedBandInfo->pInstancesSize,
                        CMP_PTR_TYPE, varp->pRfDedicatedBandInfo->pRfDedicatedBandInfoParam, cstp->pRfDedicatedBandInfo->pRfDedicatedBandInfoParam,
                        CMP_PTR_TYPE, varp->pRfBandInfoExtFormat->pInstancesSize, cstp->pRfBandInfoExtFormat->pInstancesSize,
                        CMP_PTR_TYPE, varp->pRfBandInfoExtFormat->pRfBandInfoExtFormatParam, cstp->pRfBandInfoExtFormat->pRfBandInfoExtFormatParam,
                        CMP_PTR_TYPE, varp->pRfBandwidthInfo->pInstancesSize, cstp->pRfBandwidthInfo->pInstancesSize,
                        CMP_PTR_TYPE, varp->pRfBandwidthInfo->pRfBandwidthInfoParam, cstp->pRfBandwidthInfo->pRfBandwidthInfoParam,
                        CMP_PTR_TYPE, varp->pLTEOperationMode->pLTEOperationMode, cstp->pLTEOperationMode->pLTEOperationMode,
                        );
                    }
                    break;

                case eQMI_NAS_GET_SIG_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGetSigInfo,
                        dump_SLQSNasGetSigInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasGetSigInfo_t);
                    }
                    break;

                case eQMI_NAS_SIG_INFO_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasSigInfoCallback_ind_t *varp = &var_unpack_nas_SLQSNasSigInfoCallback_ind_t;
                        const unpack_nas_SLQSNasSigInfoCallback_ind_t *cstp = &const_unpack_nas_SLQSNasSigInfoCallback_ind_t;

                        UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasSigInfoCallback_ind,
                        displayNasSigInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasSigInfoCallback_ind_t,
                        6,
                        CMP_PTR_TYPE, varp->pCDMASigInfo, cstp->pCDMASigInfo,
                        CMP_PTR_TYPE, varp->pHDRSigInfo, cstp->pHDRSigInfo,
                        CMP_PTR_TYPE, varp->pGSMSigInfo, cstp->pGSMSigInfo,
                        CMP_PTR_TYPE, varp->pWCDMASigInfo, cstp->pWCDMASigInfo,
                        CMP_PTR_TYPE, varp->pLTESigInfo, cstp->pLTESigInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;
                case eQMI_NAS_GET_HOME_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        if (rsp_ctx.xid == 0x3B)
                        {
                            unpack_nas_SLQSGetHomeNetwork_t *varp = &var_unpack_nas_SLQSGetHomeNetwork_t;
                            const unpack_nas_SLQSGetHomeNetwork_t *cstp = &const_unpack_nas_SLQSGetHomeNetwork_t;

                            UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SLQSGetHomeNetwork,
                            dump_SLQSGetHomeNetwork,
                            msg.buf,
                            rlen,
                            &var_unpack_nas_SLQSGetHomeNetwork_t,
                            8,
                            CMP_PTR_TYPE, &varp->mcc, &cstp->mcc,
                            CMP_PTR_TYPE, &varp->mnc, &cstp->mnc,
                            CMP_PTR_TYPE, varp->name, cstp->name,
                            CMP_PTR_TYPE, &varp->sid, &cstp->sid,
                            CMP_PTR_TYPE, &varp->nid, &cstp->nid,
                            CMP_PTR_TYPE, varp->pHomeNwMNC3Gpp, cstp->pHomeNwMNC3Gpp,
                            CMP_PTR_TYPE, varp->pNwNameSrc3Gpp, cstp->pNwNameSrc3Gpp,
                            CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                            );
                        }
                        else
                        {
                            UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                                unpack_nas_GetHomeNetwork,
                                dump_GetHomeNetwork,
                                msg.buf,
                                rlen,
                                &const_unpack_nas_GetHomeNetwork_t);
                        }
                    }
                    break;

                case eQMI_NAS_GET_SYS_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSGetSysInfo_t *varp = &var_unpack_nas_SLQSGetSysInfo_t;
                        const unpack_nas_SLQSGetSysInfo_t *cstp = &const_unpack_nas_SLQSGetSysInfo_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetSysInfo,
                        dump_SLQSGetSysInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetSysInfo_t,
                        7,
                        CMP_PTR_TYPE, varp->pWCDMASrvStatusInfo, cstp->pWCDMASrvStatusInfo,
                        CMP_PTR_TYPE, varp->pLTESrvStatusInfo, cstp->pLTESrvStatusInfo,
                        CMP_PTR_TYPE, varp->pWCDMASysInfo, cstp->pWCDMASysInfo,
                        CMP_PTR_TYPE, varp->pAddWCDMASysInfo, cstp->pAddWCDMASysInfo,
                        CMP_PTR_TYPE, varp->pWCDMACallBarringSysInfo, cstp->pWCDMACallBarringSysInfo,
                        CMP_PTR_TYPE, varp->pWCDMACipherDomainSysInfo, cstp->pWCDMACipherDomainSysInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );

                        printf("\n\n");

                        unpack_nas_SLQSGetSysInfoV2_t *varp_v2 = &var_unpack_nas_SLQSGetSysInfoV2_t;
                        const unpack_nas_SLQSGetSysInfoV2_t *cstp_v2 = &const_unpack_nas_SLQSGetSysInfoV2_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetSysInfoV2,
                        dump_SLQSGetSysInfoV2,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetSysInfoV2_t,
                        18,
                        CMP_PTR_TYPE, varp_v2->pWCDMASrvStatusInfo, cstp_v2->pWCDMASrvStatusInfo,
                        CMP_PTR_TYPE, varp_v2->pLTESrvStatusInfo, cstp_v2->pLTESrvStatusInfo,
                        CMP_PTR_TYPE, varp_v2->pWCDMASysInfo, cstp_v2->pWCDMASysInfo,
                        CMP_PTR_TYPE, varp_v2->pAddWCDMASysInfo, cstp_v2->pAddWCDMASysInfo,
                        CMP_PTR_TYPE, varp_v2->pWCDMACallBarringSysInfo, cstp_v2->pWCDMACallBarringSysInfo,
                        CMP_PTR_TYPE, varp_v2->pWCDMACipherDomainSysInfo, cstp_v2->pWCDMACipherDomainSysInfo,
                        CMP_PTR_TYPE, varp_v2->pLteEmbmsCoverage, cstp_v2->pLteEmbmsCoverage,
                        CMP_PTR_TYPE, varp_v2->pSimRejInfo, cstp_v2->pSimRejInfo,
                        CMP_PTR_TYPE, varp_v2->pImsVoiceSupportLte, cstp_v2->pImsVoiceSupportLte,
                        CMP_PTR_TYPE, varp_v2->pLteVoiceDomain, cstp_v2->pLteVoiceDomain,
                        CMP_PTR_TYPE, varp_v2->pSrvRegRestriction, cstp_v2->pSrvRegRestriction,
                        CMP_PTR_TYPE, varp_v2->pLteRegDomain, cstp_v2->pLteRegDomain,
                        CMP_PTR_TYPE, varp_v2->pLteEmbmsTraceId, cstp_v2->pLteEmbmsTraceId,
                        CMP_PTR_TYPE, varp_v2->pCampedCiotLteOpMode, cstp_v2->pCampedCiotLteOpMode,
                        CMP_PTR_TYPE, varp_v2->pNR5GSrvStatusinfo, cstp_v2->pNR5GSrvStatusinfo,
                        CMP_PTR_TYPE, varp_v2->pNR5GSysInfo, cstp_v2->pNR5GSysInfo,
                        CMP_PTR_TYPE, varp_v2->pNR5GCellStatusInfo, cstp_v2->pNR5GCellStatusInfo,
                        CMP_PTR_TYPE, &varp_v2->ParamPresenceMask, &cstp_v2->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SYS_INFO_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSysInfoCallback_ind_t *varp = &var_unpack_nas_SLQSSysInfoCallback_ind_t;
                        const unpack_nas_SLQSSysInfoCallback_ind_t *cstp = &const_unpack_nas_SLQSSysInfoCallback_ind_t;

                        UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasSysInfoCallback_ind,
                        displayNasSysInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSysInfoCallback_ind_t,
                        12,
                        CMP_PTR_TYPE, varp->pWCDMASrvStatusInfo, cstp->pWCDMASrvStatusInfo,
                        CMP_PTR_TYPE, varp->pLTESrvStatusInfo, cstp->pLTESrvStatusInfo,
                        CMP_PTR_TYPE, varp->pWCDMASysInfo, cstp->pWCDMASysInfo,
                        CMP_PTR_TYPE, varp->pAddWCDMASysInfo, cstp->pAddWCDMASysInfo,
                        CMP_PTR_TYPE, varp->pWCDMACallBarringSysInfo, cstp->pWCDMACallBarringSysInfo,
                        CMP_PTR_TYPE, varp->pWCDMACipherDomainSysInfo, cstp->pWCDMACipherDomainSysInfo,
                        CMP_PTR_TYPE, varp->pSysInfoNoChange, cstp->pSysInfoNoChange,
                        CMP_PTR_TYPE, varp->pLteCiotOpModeTlv, cstp->pLteCiotOpModeTlv,
                        CMP_PTR_TYPE, varp->pNR5GSerStatTlv, cstp->pNR5GSerStatTlv,
                        CMP_PTR_TYPE, varp->pNR5GSystemInfoTlv, cstp->pNR5GSystemInfoTlv,
                        CMP_PTR_TYPE, varp->pNR5GCellStatus, cstp->pNR5GCellStatus,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_SS_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_GetServingNetwork,
                            dump_GetServingNetwork,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_GetServingNetwork_t);

                        printf("\n\n");

                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SLQSGetServingSystem,
                            dump_SLQSGetServingSystem,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_SLQSGetServingSystem_t);

                        printf("\n\n");

                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SLQSGetServingSystemV2,
                            dump_SLQSGetServingSystemV2,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_SLQSGetServingSystemV2_t);
                    }
                    else if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetDataCapabilitiesCallback_ind,
                        displayDataCap,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetDataCapabilitiesCallback_ind_t);

                        printf("\n\n");

                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetServingSystemCallback_ind,
                        DisplayServingSystem,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetServingSystemCallback_ind_t);
                    }
                    break;

                case eQMI_NAS_SCAN_NETS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_PerformNetworkScan_t *varp = &var_unpack_nas_PerformNetworkScan_t;
                        const unpack_nas_PerformNetworkScan_t *cstp = &const_unpack_nas_PerformNetworkScan_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_PerformNetworkScan,
                        dump_PerformNetworkScan,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_PerformNetworkScan_t,
                        8,
                        CMP_PTR_TYPE, varp->p3GppNetworkInstanceSize, cstp->p3GppNetworkInstanceSize,
                        CMP_PTR_TYPE, varp->p3GppNetworkInfoInstances, cstp->p3GppNetworkInfoInstances,
                        CMP_PTR_TYPE, varp->pRATInstanceSize, cstp->pRATInstanceSize,
                        CMP_PTR_TYPE, varp->pRATINstance, cstp->pRATINstance,
                        CMP_PTR_TYPE, varp->pPCSInstanceSize, cstp->pPCSInstanceSize,
                        CMP_PTR_TYPE, varp->pPCSInstance, cstp->pPCSInstance,
                        CMP_PTR_TYPE, varp->pScanResult, cstp->pScanResult,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );

                        printf("\n\n");

                        unpack_nas_SLQSPerformNetworkScanV2_t *varp_v2 = &var_unpack_nas_SLQSPerformNetworkScanV2_t;
                        const unpack_nas_SLQSPerformNetworkScanV2_t *cstp_v2 = &const_unpack_nas_SLQSPerformNetworkScanV2_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSPerformNetworkScanV2,
                        dump_SLQSPerformNetworkScanV2,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSPerformNetworkScanV2_t,
                        9,
                        CMP_PTR_TYPE, varp_v2->p3GppNetworkInstanceSize, cstp_v2->p3GppNetworkInstanceSize,
                        CMP_PTR_TYPE, varp_v2->p3GppNetworkInfoInstances, cstp_v2->p3GppNetworkInfoInstances,
                        CMP_PTR_TYPE, varp_v2->pRATInstanceSize, cstp_v2->pRATInstanceSize,
                        CMP_PTR_TYPE, varp_v2->pRATINstance, cstp_v2->pRATINstance,
                        CMP_PTR_TYPE, varp_v2->pPCSInstanceSize, cstp_v2->pPCSInstanceSize,
                        CMP_PTR_TYPE, varp_v2->pPCSInstance, cstp_v2->pPCSInstance,
                        CMP_PTR_TYPE, varp_v2->pScanResult, cstp_v2->pScanResult,
                        CMP_PTR_TYPE, varp_v2->pNetworkNameSrcTlv, cstp_v2->pNetworkNameSrcTlv,
                        CMP_PTR_TYPE, &varp_v2->ParamPresenceMask, &cstp_v2->ParamPresenceMask,
                        );
             }
                    break;

                case eQMI_NAS_SWI_GET_CQI:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SLQSSwiGetLteCQI,
                            dump_SLQSSwiGetLteCQI,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_SLQSSwiGetLteCQI_t);
                    }
                    break;

                case eQMI_NAS_SWI_MODEM_STATUS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasSwiModemStatus_t *varp = &var_unpack_nas_SLQSNasSwiModemStatus_t;
                        const unpack_nas_SLQSNasSwiModemStatus_t *cstp = &const_unpack_nas_SLQSNasSwiModemStatus_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasSwiModemStatus,
                        dump_SLQSNasSwiModemStatus,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasSwiModemStatus_t,
                        3,
                        CMP_PTR_TYPE, &varp->commonInfo, &cstp->commonInfo,
                        CMP_PTR_TYPE, varp->pLTEInfo, cstp->pLTEInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;
                case eQMI_NAS_SET_EVENT:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSetSignalStrengthsCallback,
                        dump_SLQSSetSignalStrengthsCallback,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSSetSignalStrengthsCallback_t);
                    }
                    else if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetEventReportInd,
                        displayNasEventInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetEventReportInd_t);
                    }
                    break;

                case eQMI_NAS_GET_NET_PARAMS:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_GetCDMANetworkParameters,
                            dump_GetCDMANetworkParameters,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_GetCDMANetworkParameters_t);
                    }
                    break;

                case eQMI_NAS_GET_AAA_AUTH_STATUS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_GetANAAAAuthenticationStatus_t *varp = &var_unpack_nas_GetANAAAAuthenticationStatus_t;
                        const unpack_nas_GetANAAAAuthenticationStatus_t *cstp = &const_unpack_nas_GetANAAAAuthenticationStatus_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_GetANAAAAuthenticationStatus,
                        dump_GetANAAAAuthenticationStatus,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_GetANAAAAuthenticationStatus_t,
                        2,
                        CMP_PTR_TYPE, varp->pAuthStatus, cstp->pAuthStatus,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_ACCOLC:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_GetACCOLC_t *varp = &var_unpack_nas_GetACCOLC_t;
                        const unpack_nas_GetACCOLC_t *cstp = &const_unpack_nas_GetACCOLC_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_GetACCOLC,
                        dump_GetACCOLC,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_GetACCOLC_t,
                        2,
                        CMP_PTR_TYPE, varp->pAccolc, cstp->pAccolc,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SET_ACCOLC:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetACCOLC,
                        dump_SetACCOLC,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetACCOLC_t);
                    }
                    break;

                case eQMI_NAS_CONFIG_SIG_INFO2:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasConfigSigInfo2,
                        dump_SLQSNasConfigSigInfo2,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasConfigSigInfo2_t);
                    }
                    break;

                case eQMI_NAS_GET_TECH_PREF:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_GetNetworkPreference,
                        dump_GetNetworkPreference,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_GetNetworkPreference_t);
                    }
                    break;

                case eQMI_NAS_SET_TECH_PREF:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetNetworkPreference,
                        dump_SetNetworkPreference,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetNetworkPreference_t);
                    }
                    break;

                case eQMI_NAS_REGISTER_NET:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSInitiateNetworkRegistration,
                        dump_SLQSInitiateNetworkRegistration,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSInitiateNetworkRegistration_t);
                    }
                    break;

                case eQMI_NAS_SWI_OTA_MESSAGE_INDICATION:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasSwiOTAMessageCallback_ind,
                        displayNasSwiOTAMessageInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t);
                    }
                    break;

                case eQMI_NAS_SWI_INDICATION_REGISTER:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasSwiIndicationRegister,
                        dump_SLQSNasSwiIndicationRegister,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasSwiIndicationRegister_t);
                    }
                    break;

                case eQMI_NAS_LTE_CPHY_CA_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetNasLTECphyCaIndCallback_ind,
                        displayNasLTECphyCa,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetNasLTECphyCaIndCallback_ind_t);
                    }
                    break;

                case eQMI_NAS_ATTACH_DETACH:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_InitiateDomainAttach,
                        dump_InitiateDomainAttach,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_InitiateDomainAttach_t);
                    }
                    break;

                case eQMI_NAS_SET_NET_PARAMS:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SetCDMANetworkParameters,
                        dump_SetCDMANetworkParameters,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SetCDMANetworkParameters_t);
                    }
                    break;

                case eQMI_NAS_GET_HDR_COLOR_CODE:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasGetHDRColorCode_t *varp = &var_unpack_nas_SLQSNasGetHDRColorCode_t;
                        const unpack_nas_SLQSNasGetHDRColorCode_t *cstp = &const_unpack_nas_SLQSNasGetHDRColorCode_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGetHDRColorCode,
                        dump_SLQSNasGetHDRColorCode,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGetHDRColorCode_t,
                        2,
                        CMP_PTR_TYPE, varp->pColorCode, cstp->pColorCode,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_PS_DETACH:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiPSDetach,
                        dump_SLQSSwiPSDetach,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSSwiPSDetach_t);
                    }
                    break;

                case eQMI_NAS_CONFIG_SIG_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSConfigSigInfo,
                        dump_SLQSConfigSigInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSConfigSigInfo_t);
                    }
                    break;

                case eQMI_NAS_SWI_SET_CHANNEL_LOCK:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNASSwiSetChannelLock,
                        dump_SLQSNASSwiSetChannelLock,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNASSwiSetChannelLock_t);
                    }
                    break;

                case eQMI_NAS_SET_EDRX_PARAMS:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNASSeteDRXParams,
                        dump_SLQSNASSeteDRXParams,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNASSeteDRXParams_t);
                    }
                    break;

                case eQMI_NAS_GET_PLMN_NAME:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetPLMNName,
                        dump_SLQSGetPLMNName,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSGetPLMNName_t);
                    }
                    break;

                case eQMI_NAS_GET_CELL_LOCATION_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasGetCellLocationInfo_t *varp = &var_unpack_nas_SLQSNasGetCellLocationInfo_t;
                        const unpack_nas_SLQSNasGetCellLocationInfo_t *cstp = &const_unpack_nas_SLQSNasGetCellLocationInfo_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGetCellLocationInfo,
                        dump_SLQSNasGetCellLocationInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGetCellLocationInfo_t,
                        7,
                        CMP_PTR_TYPE, varp->pUMTSInfo, cstp->pUMTSInfo,
                        CMP_PTR_TYPE, varp->pLTEInfoIntrafreq, cstp->pLTEInfoIntrafreq,
                        CMP_PTR_TYPE, varp->pLTEInfoInterfreq, cstp->pLTEInfoInterfreq,
                        CMP_PTR_TYPE, varp->pLTEInfoNeighboringGSM, cstp->pLTEInfoNeighboringGSM,
                        CMP_PTR_TYPE, varp->pLTEInfoNeighboringWCDMA, cstp->pLTEInfoNeighboringWCDMA,
                        CMP_PTR_TYPE, varp->pUMTSCellID, cstp->pUMTSCellID,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );

                        printf("\n\n");

                        unpack_nas_SLQSNasGetCellLocationInfoV2_t *varp_v2 = &var_unpack_nas_SLQSNasGetCellLocationInfoV2_t;
                        const unpack_nas_SLQSNasGetCellLocationInfoV2_t *cstp_v2 = &const_unpack_nas_SLQSNasGetCellLocationInfoV2_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGetCellLocationInfoV2,
                        dump_SLQSNasGetCellLocationInfoV2,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGetCellLocationInfoV2_t,
                        10,
                        CMP_PTR_TYPE, varp_v2->pUMTSInfo, cstp->pUMTSInfo,
                        CMP_PTR_TYPE, varp_v2->pLTEInfoIntrafreq, cstp_v2->pLTEInfoIntrafreq,
                        CMP_PTR_TYPE, varp_v2->pLTEInfoInterfreq, cstp_v2->pLTEInfoInterfreq,
                        CMP_PTR_TYPE, varp_v2->pLTEInfoNeighboringGSM, cstp_v2->pLTEInfoNeighboringGSM,
                        CMP_PTR_TYPE, varp_v2->pLTEInfoNeighboringWCDMA, cstp_v2->pLTEInfoNeighboringWCDMA,
                        CMP_PTR_TYPE, varp_v2->pUMTSCellID, cstp_v2->pUMTSCellID,
                        CMP_PTR_TYPE, varp_v2->pWCDMACellInfoExt, cstp_v2->pWCDMACellInfoExt,
                        CMP_PTR_TYPE, varp_v2->pUMTSExtInfo, cstp_v2->pUMTSExtInfo,
                        CMP_PTR_TYPE, varp_v2->pLteEarfcnInfo, cstp_v2->pLteEarfcnInfo,
                        CMP_PTR_TYPE, &varp_v2->ParamPresenceMask, &cstp_v2->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_NETWORK_TIME:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSGetNetworkTime_t *varp = &var_unpack_nas_SLQSGetNetworkTime_t;
                        const unpack_nas_SLQSGetNetworkTime_t *cstp = &const_unpack_nas_SLQSGetNetworkTime_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetNetworkTime,
                        dump_SLQSGetNetworkTime,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetNetworkTime_t,
                        2,
                        CMP_PTR_TYPE, varp->p3GPPTimeInfo, cstp->p3GPPTimeInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_NETWORK_TIME_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasNetworkTimeCallBack_ind_t *varp = &var_unpack_nas_SLQSNasNetworkTimeCallBack_ind_t;
                        const unpack_nas_SLQSNasNetworkTimeCallBack_ind_t *cstp = &const_unpack_nas_SLQSNasNetworkTimeCallBack_ind_t;

                        UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasNetworkTimeCallBack_ind,
                        displayNasNetworkTime,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasNetworkTimeCallBack_ind_t,
                        5,
                        CMP_PTR_TYPE, &varp->universalTime, &cstp->universalTime,
                        CMP_PTR_TYPE, varp->pTimeZone, cstp->pTimeZone,
                        CMP_PTR_TYPE, varp->pDayltSavAdj, cstp->pDayltSavAdj,
                        CMP_PTR_TYPE, varp->pRadioInterface, cstp->pRadioInterface,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_GET_LTE_SCC_RX_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSwiGetLteSccRxInfo_t *varp = &var_unpack_nas_SLQSSwiGetLteSccRxInfo_t;
                        const unpack_nas_SLQSSwiGetLteSccRxInfo_t *cstp = &const_unpack_nas_SLQSSwiGetLteSccRxInfo_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiGetLteSccRxInfo,
                        dump_SLQSSwiGetLteSccRxInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSwiGetLteSccRxInfo_t,
                        2,
                        CMP_PTR_TYPE, varp->pSccRxInfo, cstp->pSccRxInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_NETWORK_TIMERS_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasTimerCallback_ind,
                        displayNasTimer,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasTimerCallback_ind_t);
                    }
                    break;

                case eQMI_NAS_GET_TX_RX_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasGetTxRxInfo_t *varp = &var_unpack_nas_SLQSNasGetTxRxInfo_t;
                        const unpack_nas_SLQSNasGetTxRxInfo_t *cstp = &const_unpack_nas_SLQSNasGetTxRxInfo_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGetTxRxInfo,
                        dump_SLQSNasGetTxRxInfo,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGetTxRxInfo_t,
                        4,
                        CMP_PTR_TYPE, varp->pRXChain0Info, cstp->pRXChain0Info,
                        CMP_PTR_TYPE, varp->pRXChain1Info, cstp->pRXChain1Info,
                        CMP_PTR_TYPE, varp->pTXInfo, cstp->pTXInfo,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_OPERATOR_NAME:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSGetOperatorNameData_t *varp = &var_unpack_nas_SLQSGetOperatorNameData_t;
                        const unpack_nas_SLQSGetOperatorNameData_t *cstp = &const_unpack_nas_SLQSGetOperatorNameData_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetOperatorNameData,
                        dump_SLQSGetOperatorNameData,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetOperatorNameData_t,
                        6,
                        CMP_PTR_TYPE, varp->pSrvcProviderName, cstp->pSrvcProviderName,
                        CMP_PTR_TYPE, varp->pOperatorPLMNList, cstp->pOperatorPLMNList,
                        CMP_PTR_TYPE, varp->pPLMNNetworkName, cstp->pPLMNNetworkName,
                        CMP_PTR_TYPE, varp->pOperatorNameString, cstp->pOperatorNameString,
                        CMP_PTR_TYPE, varp->pNITZInformation, cstp->pNITZInformation,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_3GPP2_SUBSCRIPTION:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasGet3GPP2Subscription_t *varp = &var_unpack_nas_SLQSNasGet3GPP2Subscription_t;
                        const unpack_nas_SLQSNasGet3GPP2Subscription_t *cstp = &const_unpack_nas_SLQSNasGet3GPP2Subscription_t;
                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasGet3GPP2Subscription,
                        dump_SLQSNasGet3GPP2Subscription,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasGet3GPP2Subscription_t,
                        8,
                        CMP_PTR_TYPE, varp->pNAMNameInfo, cstp->pNAMNameInfo,
                        CMP_PTR_TYPE, varp->pDirNum, cstp->pDirNum,
                        CMP_PTR_TYPE, varp->pHomeSIDNID, cstp->pHomeSIDNID,
                        CMP_PTR_TYPE, varp->pMinBasedIMSI, cstp->pMinBasedIMSI,
                        CMP_PTR_TYPE, varp->pTrueIMSI, cstp->pTrueIMSI,
                        CMP_PTR_TYPE, varp->pCDMAChannel, cstp->pCDMAChannel,
                        CMP_PTR_TYPE, varp->pMdn, cstp->pMdn,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_HDR_PERSONALITY:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSwiGetHDRPersonality_t *varp = &var_unpack_nas_SLQSSwiGetHDRPersonality_t;
                        const unpack_nas_SLQSSwiGetHDRPersonality_t *cstp = &const_unpack_nas_SLQSSwiGetHDRPersonality_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiGetHDRPersonality,
                        dump_SLQSSwiGetHDRPersonality,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSwiGetHDRPersonality_t,
                        4,
                        CMP_PTR_TYPE, varp->pCurrentPersonality, cstp->pCurrentPersonality,
                        CMP_PTR_TYPE, varp->pPersonalityListLength, cstp->pPersonalityListLength,
                        CMP_PTR_TYPE, varp->pProtocolSubtypeElement, cstp->pProtocolSubtypeElement,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    else if (eIND == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t *varp = &var_unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t;
                        const unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t *cstp = &const_unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t;

                        UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiHDRPersonalityCallback_Ind,
                        displayNasHdrPersonality,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t,
                        4,
                        CMP_PTR_TYPE, varp->pHDRPersInd->pCurrentPersonality, cstp->pHDRPersInd->pCurrentPersonality,
                        CMP_PTR_TYPE, varp->pHDRPersInd->pPersonalityListLength, cstp->pHDRPersInd->pPersonalityListLength,
                        CMP_PTR_TYPE, varp->pHDRPersInd->pProtocolSubtypeElement, cstp->pHDRPersInd->pProtocolSubtypeElement,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_ERR_RATE:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSGetErrorRate_t *varp = &var_unpack_nas_SLQSGetErrorRate_t;
                        const unpack_nas_SLQSGetErrorRate_t *cstp = &const_unpack_nas_SLQSGetErrorRate_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSGetErrorRate,
                        dump_SLQSGetErrorRate,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSGetErrorRate_t,
                        5,
                        CMP_PTR_TYPE, varp->pCDMAFrameErrRate, cstp->pCDMAFrameErrRate,
                        CMP_PTR_TYPE, varp->pHDRPackErrRate, cstp->pHDRPackErrRate,
                        CMP_PTR_TYPE, varp->pGSMBER, cstp->pGSMBER,
                        CMP_PTR_TYPE, varp->pWCDMABER, cstp->pWCDMABER,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_GET_HRPD_STATS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSwiGetHRPDStats_t *varp = &var_unpack_nas_SLQSSwiGetHRPDStats_t;
                        const unpack_nas_SLQSSwiGetHRPDStats_t *cstp = &const_unpack_nas_SLQSSwiGetHRPDStats_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiGetHRPDStats,
                        dump_SLQSSwiGetHRPDStats,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSwiGetHRPDStats_t,
                        4,
                        CMP_PTR_TYPE, varp->pDRCParams, cstp->pDRCParams,
                        CMP_PTR_TYPE, varp->pUATI, cstp->pUATI,
                        CMP_PTR_TYPE, varp->pPilotSetData, cstp->pPilotSetData,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_NETWORK_DEBUG:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSSwiNetworkDebug_t *varp = &var_unpack_nas_SLQSSwiNetworkDebug_t;
                        const unpack_nas_SLQSSwiNetworkDebug_t *cstp = &const_unpack_nas_SLQSSwiNetworkDebug_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiNetworkDebug,
                        dump_SLQSSwiNetworkDebug,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSSwiNetworkDebug_t,
                        18,
                        CMP_PTR_TYPE, varp->pObjectVer, cstp->pObjectVer,
                        CMP_PTR_TYPE, &varp->pNetworkStat1x->State, &cstp->pNetworkStat1x->State,
                        CMP_PTR_TYPE, &varp->pNetworkStat1x->SO, &cstp->pNetworkStat1x->SO,
                        CMP_PTR_TYPE, &varp->pNetworkStat1x->RX_PWR, &cstp->pNetworkStat1x->RX_PWR,
                        CMP_PTR_TYPE, &varp->pNetworkStat1x->RX_EC_IO, &cstp->pNetworkStat1x->RX_EC_IO,
                        CMP_PTR_TYPE, &varp->pNetworkStat1x->TX_PWR, &cstp->pNetworkStat1x->TX_PWR,
                        CMP_PTR_TYPE, varp->pNetworkStat1x->pActPilotPNElements, cstp->pNetworkStat1x->pActPilotPNElements,
                        CMP_PTR_TYPE, varp->pNetworkStat1x->pNeighborSetPilotPN, cstp->pNetworkStat1x->pNeighborSetPilotPN,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->State, &cstp->pNetworkStatEVDO->State,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->MACIndex, &cstp->pNetworkStatEVDO->MACIndex,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->RX_PWR, &cstp->pNetworkStatEVDO->RX_PWR,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->PER, &cstp->pNetworkStatEVDO->PER,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->PilotEnergy, &cstp->pNetworkStatEVDO->PilotEnergy,
                        CMP_PTR_TYPE, varp->pNetworkStatEVDO->pSectorID, cstp->pNetworkStatEVDO->pSectorID,
                        CMP_PTR_TYPE, &varp->pNetworkStatEVDO->SNR, &cstp->pNetworkStatEVDO->SNR,
                        CMP_PTR_TYPE, varp->pDeviceConfigDetail, cstp->pDeviceConfigDetail,
                        CMP_PTR_TYPE, varp->pDataStatusDetail, cstp->pDataStatusDetail,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_GET_CHANNEL_LOCK:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNASSwiGetChannelLock_t *varp = &var_unpack_nas_SLQSNASSwiGetChannelLock_t;
                        const unpack_nas_SLQSNASSwiGetChannelLock_t *cstp = &const_unpack_nas_SLQSNASSwiGetChannelLock_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNASSwiGetChannelLock,
                        dump_SLQSNASSwiGetChannelLock,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNASSwiGetChannelLock_t,
                        4,
                        CMP_PTR_TYPE, varp->pWcdmaUARFCN, cstp->pWcdmaUARFCN,
                        CMP_PTR_TYPE, varp->pLteEARFCN, cstp->pLteEARFCN,
                        CMP_PTR_TYPE, varp->pLtePCI, cstp->pLtePCI,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_EDRX_PARAMS:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNASGeteDRXParamsExt_t *varp = &var_unpack_nas_SLQSNASGeteDRXParamsExt_t;
                        const unpack_nas_SLQSNASGeteDRXParamsExt_t *cstp = &const_unpack_nas_SLQSNASGeteDRXParamsExt_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNASGeteDRXParamsExt,
                        dump_SLQSNASGeteDRXParamsExt,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNASGeteDRXParamsExt_t,
                        4,
                        CMP_PTR_TYPE, varp->pCycleLen, cstp->pCycleLen,
                        CMP_PTR_TYPE, varp->pPagingTimeWindow, cstp->pPagingTimeWindow,
                        CMP_PTR_TYPE, varp->pEdrxEnable, cstp->pEdrxEnable,
                        CMP_PTR_TYPE, varp->pEdrxRAT, cstp->pEdrxRAT,
                        CMP_PTR_TYPE, varp->pLteOpMode, cstp->pLteOpMode,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_SWI_RANK_INDICATOR_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSSwiRandIndicatorCallback_Ind,
                        displayNasRankIndicator,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t);
                    }
                    break;

                case eQMI_NAS_EDRX_CHANGE_INFO_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind,
                        displayEdrxChangeInfo,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t);
                    }
                    break;

                case eQMI_NAS_NETWORK_REJECT_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNasNetworkRejectCallback_Ind_t *varp = &var_unpack_nas_SLQSNasNetworkRejectCallback_Ind_t;
                        const unpack_nas_SLQSNasNetworkRejectCallback_Ind_t *cstp = &const_unpack_nas_SLQSNasNetworkRejectCallback_Ind_t;

                        UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasNetworkRejectCallback_Ind,
                        displayNetworkRejectInd,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNasNetworkRejectCallback_Ind_t,
                        6,
                        CMP_PTR_TYPE, &varp->radioIf, &cstp->radioIf,
                        CMP_PTR_TYPE, &varp->serviceDomain, &cstp->serviceDomain,
                        CMP_PTR_TYPE, &varp->rejectCause, &cstp->rejectCause,
                        CMP_PTR_TYPE, varp->pPlmnId, cstp->pPlmnId,
                        CMP_PTR_TYPE, varp->pCsgId, cstp->pCsgId,
                        CMP_PTR_TYPE, varp->pLteOpMode, cstp->pLteOpMode,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_NET_BAN_LIST:
                    if (eRSP == rsp_ctx.type)
                    {
                        unpack_nas_SLQSNASGetForbiddenNetworks_t *varp = &var_unpack_nas_SLQSNASGetForbiddenNetworks_t;
                        const unpack_nas_SLQSNASGetForbiddenNetworks_t *cstp = &const_unpack_nas_SLQSNASGetForbiddenNetworks_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNASGetForbiddenNetworks,
                        dump_SLQSNASGetForbiddenNetworks,
                        msg.buf,
                        rlen,
                        &var_unpack_nas_SLQSNASGetForbiddenNetworks_t,
                        2,
                        CMP_PTR_TYPE, varp->pForbiddenNetworks3GPP, cstp->pForbiddenNetworks3GPP,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    break;

                case eQMI_NAS_GET_LTE_CPHY_CA_INFO:
                    if (eRSP == rsp_ctx.type)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                            unpack_nas_SlqsGetLTECphyCAInfo,
                            dump_SlqsGetLTECphyCAInfo,
                            msg.buf,
                            rlen,
                            &const_unpack_nas_SlqsGetLTECphyCAInfo_t);
                    }
                    break;

                case eQMI_NAS_RF_BAND_INFO_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                        unpack_nas_SLQSNasRFBandInfoCallback_Ind,
                        dump_SLQSNasRFBandInfoCallback_Ind,
                        msg.buf,
                        rlen,
                        &const_unpack_nas_SLQSNasRFBandInfoCallback_Ind_t);
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

