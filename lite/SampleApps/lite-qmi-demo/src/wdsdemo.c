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
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "packingdemo.h"
#include "wds.h"

#define IFPRINTF(s,p)   if( NULL != p ){ printf(s,*p); }

#define PKT_STAT_STAT_MASK 0X0000003F
#define BYT_STAT_STAT_MASK 0X000000C0

#define QMI_WDS_CURRENT_CALL_DB_MASK 0x01
#define QMI_WDS_LAST_CALL_DB_MASK    0x02

#define QMI_WDS_MSG_MAX QMI_MSG_MAX

#define PROFILEID_1 1
#define PROFILEID_2 2
#define PROFILEID_3 3
#define PROFILEID_4 4
#define PROFILEID_5 5
#define PROFILEID_6 6
#define PROFILEID_7 7
#define PROFILEID_8 8

#define WDS_MAX_SIZE_IP 128
#define IPV4 4
#define IPV6 6

struct in6_ifreq {
    struct in6_addr ifr6_addr;
    uint32_t        ifr6_prefixlen;
    unsigned int    ifr6_ifindex;
};

enum {
    eProfile1Slot=0,
    eProfile2Slot=1,
    eProfile3Slot=2,
    eProfile4Slot=3,
    eProfile5Slot=4,
    eProfile6Slot=5,
    eProfile7Slot=6,
    eProfile8Slot=7,
};

int g_oemApiIndex_wds;
extern int ping(int ipFamily, char *szaddress,char *szGatewayaddress, char *szDevice);
void SetIPAddress(int ipFamily);
extern int GetIPAliasMode();
int main_mqid = -1;
int iConnected = 0;
uint8_t g_profileIndex;
uint8_t uMuxID = 0x80;
uint8_t uIPv4Count = 0;
char szIPAddress[64]={0};
char szNetMask[64]={0};
char szGWAddress[64]={0};
static uint8_t preferNetworkType=0;
static char remark[255]={0};
static int unpackRetCode = 0;
uint32_t qmap_profile_number=0;
static int ipfamily = IPV4;

const char *dhClientBin[NUMBER_OF_DHCLINET_PROGRAMS]={"dhclient","udhcpc"};

void upAdaptorAddress(char *szDeviceName);
void setAdaptorAddress(char *szDeviceName,char *IPAddress,char *GateWay,char *NetMask);
void setAdaptorIpv6Address(char *szDeviceName,char *IPAddress);
int iIsAdaptorUp(char *szDeviceName);
void SetRoute(char *szaddress,char *szMask,char *szGatewayaddress, char *szDevice);
void setAdaptorMtu(char *szDeviceName,int mtu);
int SendCMD(int iMsqid,char *szCMD);
void wds_dummy_unpack();
int wds_validate_dummy_unpack();

char routeTable[MAX_QMAP_INSTANCE][MAX_ROUTE_STRING] =
{
  {"100.168.1.0/24"},
  {"101.168.1.0/24"},
  {"102.168.1.0/24"},
  {"103.168.1.0/24"},
  {""},
  {""},
  {""},
  {""}   
};

char setrouteTable[MAX_QMAP_INSTANCE][2][MAX_ROUTE_STRING] =
{
  {{"100.168.1.0"},{"255.255.255.0"}},
  {{"101.168.1.0"},{"255.255.255.0"}},
  {{"102.168.1.0"},{"255.255.255.0"}},
  {{"103.168.1.0"},{"255.255.255.0"}},
  {{""},{""}},
  {{""},{""}},
  {{""},{""}},
  {{""},{""}}   
};

char pingTable[MAX_QMAP_INSTANCE][MAX_ROUTE_STRING] =
{
  {"8.8.8.8"},
  {"216.105.38.13"},
  {"1.1.1.1"},
  {"2.2.2.2"},
  {""},
  {""},
  {""},
  {""}   
};

char ping6Table[MAX_QMAP_INSTANCE][MAX_ROUTE_STRING] =
{
  {"fc01:cafe::1"},
  {""},
  {""},
  {""},
  {""},
  {""},
  {""},
  {""}   
};


int PrintIPv4RuntimeSettings(unpack_wds_SLQSGetRuntimeSettings_t *runtime);

pack_wds_SLQSSetIPFamilyPreference_t packMUXIPFamilyPreference ={
    PACK_WDS_IPV4
};

char szEthName[64]="eth1\0";

uint16_t setdfltpfl_xid;
uint16_t getbytestotal_xid;

int wds=-1;
pthread_attr_t wds_attr;
pthread_t wds_tid = 0;
pthread_attr_t wds_cmd_attr;
pthread_t wds_cmd_tid = 0;
volatile int enWdsThread;
volatile int ReadyToWriteNext;
volatile uint16_t Sendxid;

void *wds_read_thread(void* ptr);
uint16_t u8toU16(uint8_t u8Array[2]);


unpack_wds_SLQSGetRuntimeSettings_t runtimeinfo;
unpack_wds_GetSessionState_t stateinfo;
unpack_wds_GetConnectionRate_t connrate;
unpack_wds_GetSessionDuration_t sessionduration;
uint64_t callActiveDuration;
uint64_t lastCallDuration;
uint64_t lastCallActiveDuration;
unpack_wds_GetSessionDurationV2_t sessiondurationV2 = {
    0,
    &lastCallDuration,
    &callActiveDuration,
    &lastCallActiveDuration,
    SWI_UINT256_INT_VALUE
};
unpack_wds_GetDormancyState_t   dormancystate;
unpack_wds_SLQSGet3GPPConfigItem_t  get3GPPconfigitem = {
   0,
   {0},
   0,
   0,
   {0},
   MAX_WDS_3GPP_CONF_LTE_ATTACH_PROFILE_LIST_SIZE,
   SWI_UINT256_INT_VALUE
};
unpack_wds_GetMobileIP_t  mipMode;
unpack_wds_SLQSGetCurrDataSystemStat_t dataStat;
unpack_wds_GetLastMobileIPError_t mobileIPError;
////Modem Settings//
unpack_wds_SLQSGet3GPPConfigItem_t  g3GPPconfigitemSettings = {
   0,
   {0},
   0,
   0,
   {0},
   MAX_WDS_3GPP_CONF_LTE_ATTACH_PROFILE_LIST_SIZE,
   SWI_UINT256_INT_VALUE
};
pack_wds_SetDefaultProfileNum_t DefaultProfileNumSettings = {0, 0, 3};

uint16_t Default3GPPConfigItemSettingsLTEAttachProfileList[] = {1};
uint16_t Default3GPPConfigItemSettingsProfileList[]={1,2,3,4};
uint8_t  Default3GPPConfigItemSettingsPDNEnabled = 1;
uint8_t  Default3GPPConfigItemSettings_3gppRelease = 7;

pack_wds_SLQSSet3GPPConfigItem_t Default3GPPConfigItemSettings  = {
   NULL,
   Default3GPPConfigItemSettingsProfileList,
   &Default3GPPConfigItemSettingsPDNEnabled,
   &Default3GPPConfigItemSettings_3gppRelease,
   Default3GPPConfigItemSettingsLTEAttachProfileList,
   1
};
pack_wds_SetAutoconnect_t DefaultAutoconnectSettings = {0,0};
pack_wds_SLQSSSetLoopback_t DefaultLoopbackSettings ={0,0};
uint8_t  DefaultProfileSettingsPName[255] = {0};
uint8_t  DefaultProfileSettingsAPNName[255] = {0};
uint8_t  DefaultProfileSettingsUsername[255] = {0};
uint8_t  DefaultProfileSettingsPassword[255] = {0};

pack_wds_SetDefaultProfile_t DefaultProfileSettings = {0, 0, 0, 0, 0, 1,
          DefaultProfileSettingsPName, DefaultProfileSettingsAPNName,
          DefaultProfileSettingsUsername, DefaultProfileSettingsPassword };
unpack_wds_SetDefaultProfile_t tunpack_wds_SetDefaultProfile;

uint16_t LTEAttachProfile = 1;
uint16_t ProfileList[]={1,2,3,4};
uint8_t  DefaultPDNEnabled = 1;
uint8_t  _3gppRelease = 3;
uint8_t  _3gppRelease_9x15 = 4;
uint16_t LTEAttachProfileList[] = {1};

pack_wds_SLQSSet3GPPConfigItem_t tpack_wds_SLQSSet3GPPConfigItem_9x15  = {
   &LTEAttachProfile,
   ProfileList,
   &DefaultPDNEnabled,
   &_3gppRelease_9x15,
   NULL,
   0
};

pack_wds_SLQSSet3GPPConfigItem_t tpack_wds_SLQSSet3GPPConfigItem  = {
   NULL,
   ProfileList,
   &DefaultPDNEnabled,
   &_3gppRelease,
   LTEAttachProfileList,
   1
};

unpack_wds_SLQSSet3GPPConfigItem_t tunpack_wds_SLQSSet3GPPConfigItem =
    SWI_INIT_UNPACK_RESULT_VALUE;


/* GetPacketStatus */
swi_uint256_t unpack_wds_GetPacketStatusParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_13_BITS,2,16,17,18,19,20,21,25,26,27,28,29,30)
}};

swi_uint256_t unpack_wds_GetPacketStatusParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* GetSessionDuration */
swi_uint256_t unpack_wds_GetSessionDurationParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)
}};

swi_uint256_t unpack_wds_GetSessionDurationParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* GetSessionState */
swi_uint256_t unpack_wds_GetSessionStateParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_wds_GetSessionStateParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSCreateProfile */
swi_uint256_t unpack_wds_SLQSCreateProfileParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,224)
}};

swi_uint256_t unpack_wds_SLQSCreateProfileParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSDeleteProfile */
swi_uint256_t unpack_wds_SLQSDeleteProfileParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,224)
}};

swi_uint256_t unpack_wds_SLQSDeleteProfileParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSGetProfileSettings */
swi_uint256_t unpack_wds_SLQSGetProfileSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_90_BITS,2,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38, \
          39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72, \
          143,144,145,146,147,148,149,150,151,152,153,154,155,156, \
          157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,222,223,224)
}};

swi_uint256_t unpack_wds_SLQSGetProfileSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSGetRuntimeSettings */
swi_uint256_t unpack_wds_SLQSGetRuntimeSettingsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_26_BITS,2,16,17,20,21,22,23,25,27,29,30,31,32,33,34,35,36,37,38, \
          39,40,41,42,43,44,45)
}};

swi_uint256_t unpack_wds_SLQSGetRuntimeSettingsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSModifyProfile */
swi_uint256_t unpack_wds_SLQSModifyProfileParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,2,224)
}};

swi_uint256_t unpack_wds_SLQSModifyProfileParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSStartDataSession */
swi_uint256_t unpack_wds_SLQSStartDataSessionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)
}};

swi_uint256_t unpack_wds_SLQSStartDataSessionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSStopDataSession */
swi_uint256_t unpack_wds_SLQSStopDataSessionParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_wds_SLQSStopDataSessionParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSSetIPFamilyPreference */
swi_uint256_t unpack_wds_SLQSSetIPFamilyPreferenceParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_wds_SLQSSetIPFamilyPreferenceParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSSetWdsEventCallback */
swi_uint256_t unpack_wds_SLQSSetWdsEventCallbackParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_wds_SLQSSetWdsEventCallbackParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSSetWdsEventCallback_ind */
swi_uint256_t unpack_wds_SLQSSetWdsEventCallback_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_10_BITS,16,17,23,24,25,26,27,28,36,42)
}};

/* SLQSSetPacketSrvStatusCallback */
swi_uint256_t unpack_wds_SLQSSetPacketSrvStatusCallbackParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,1,16,17,18,19,20)
}};

swi_uint256_t unpack_wds_SLQSSetPacketSrvStatusCallbackParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,1)
}};

/* GetDormancyState */
swi_uint256_t unpack_wds_GetDormancyStateParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

swi_uint256_t unpack_wds_GetDormancyStateParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)
}};

/* SLQSGet3GPPConfigItem */
swi_uint256_t unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)
}};

swi_uint256_t unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SLQSSet3GPPConfigItem */
swi_uint256_t unpack_wds_SLQSSet3GPPConfigItemParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_wds_SLQSSet3GPPConfigItemParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};


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

/////////////////////////////////////////////////////////////////////

void dump_GetSessionState(void* ptr)
{
    /* expected result */
    unpack_wds_GetSessionState_t *psessionstate =
        (unpack_wds_GetSessionState_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (psessionstate->ParamPresenceMask);
    if(swi_uint256_get_bit (psessionstate->ParamPresenceMask, 1))
    {
        printf("sessionstate: %d\n", psessionstate->connectionStatus);
    }
#if DEBUG_LOG_TO_FILE
    uint32_t connection_status = 2; /* connected */
    CHECK_WHITELIST_MASK(
        unpack_wds_GetSessionStateParamPresenceMaskWhiteList,
        psessionstate->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_GetSessionStateParamPresenceMaskMandatoryList,
        psessionstate->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,",((connection_status==psessionstate->connectionStatus) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((connection_status==psessionstate->connectionStatus) ? "Correct": "Wrong"));
    }
#endif
}

void dump_wds_SLQSCreateProfile(void* ptr)
{
    unpack_wds_SLQSCreateProfile_t *result =
        (unpack_wds_SLQSCreateProfile_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    printf("%s Data\n",__FUNCTION__);
    if(result->pProfileID)
    printf("%s profile id: %d\n",__FUNCTION__, *result->pProfileID);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        if(result->pCreateProfileOut)
        {
            printf("ProfileIndex: %d\n", result->pCreateProfileOut->ProfileIndex);
            printf("ProfileType: %d\n", result->pCreateProfileOut->ProfileType);
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 224))
    {
        if(result->pCreateProfileOut)
        {
            printf("ExtErrorCode: %d\n", result->pCreateProfileOut->ExtErrorCode);    
        }
    }
    
#if DEBUG_LOG_TO_FILE
    /* expected result, profile id should not equal to 0 */
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSCreateProfileParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSCreateProfileParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    if(result->pProfileID)
    {

        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",((0 !=  *result->pProfileID ) ? "Correct": "Wrong"));
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n", ((0 !=  *result->pProfileID ) ? "Correct": "Wrong"));
        }
    }
    else
        local_fprintf("%s\n", "N/A");
#endif
}

void dump_wds_SLQSGetProfileSettings(void* ptr)
{
    unpack_wds_SLQSGetProfileSettings_t *result =
        (unpack_wds_SLQSGetProfileSettings_t*) ptr;
    uint8_t  idx  = 0;
    LibpackProfile3GPP *pProfile = NULL;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    RETURN_IF_PTR_IS_NULL(result->pProfileSettings)
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    pProfile = &result->pProfileSettings->curProfile.SlqsProfile3GPP;
    RETURN_IF_PTR_IS_NULL(pProfile)
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if( pProfile->pProfilename )
        {
            printf("\nProfile Name                 : %s",      pProfile->pProfilename );
        }
        IFPRINTF("\nProfile Name Size               : 0x%x",    pProfile->pProfilenameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        IFPRINTF("\nPDP Type                        : 0x%x",    pProfile->pPDPtype );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        IFPRINTF("\nPDP Header Compression Type     : 0x%X",    pProfile->pPdpHdrCompType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        IFPRINTF("\nPDP data Compression Type       : 0x%X",    pProfile->pPdpDataCompType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        if( pProfile->pAPNName )
        {
            printf("\nAPN Name                     : %s",      pProfile->pAPNName );
        }
        IFPRINTF("\nAPN Name Size                   : 0x%x",   pProfile->pAPNnameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    {
        IFPRINTF("\nPrimary DNS IPv4 address Pref.  : 0x%X",   pProfile->pPriDNSIPv4AddPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    {
        IFPRINTF("\nSecondary DNS IPv4 address Pref.: 0x%X",   pProfile->pSecDNSIPv4AddPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    {
        if( pProfile->pUMTSReqQoS )
        {
            LibPackUMTSQoS *pUMTSQoS = pProfile->pUMTSReqQoS;

            printf("\n\n UMTS Requested QoS" );
            printf("\nTraffic Class                : 0x%X",   pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",   pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",   pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",   pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",   pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X\n", pUMTSQoS->trafficPriority );
        }
    }
    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
    {
        if( pProfile->pUMTSMinQoS )
        {
            LibPackUMTSQoS *pUMTSQoS = pProfile->pUMTSMinQoS;

            printf("\n\n UMTS Minimum QoS" );
            printf("\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X\n", pUMTSQoS->trafficPriority );
        }
    }
    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
    {        
        if( pProfile->pGPRSRequestedQos )
        {
            LibPackGPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSRequestedQos;
    
            printf("\n\n GPRS Requested QoS" );
            printf("\nPrecedence Class             : 0x%X",   pGPRSQoS->precedenceClass );
            printf("\nDelay Class                  : 0x%X",   pGPRSQoS->delayClass );
            printf("\nReliability Class            : 0x%X",   pGPRSQoS->reliabilityClass );
            printf("\nPeak Throughput Class        : 0x%X",   pGPRSQoS->peakThroughputClass );
            printf("\nMean Throughput Class        : 0x%X\n", pGPRSQoS->meanThroughputClass );
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
    {
        if( pProfile->pGPRSMinimumQoS )
        {
            LibPackGPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSMinimumQoS;

            printf("\n\n GPRS Minimum QoS" );
            printf("\nPrecedence Class             : 0x%X",   pGPRSQoS->precedenceClass );
            printf("\nDelay Class                  : 0x%X",   pGPRSQoS->delayClass );
            printf("\nReliability Class            : 0x%X",   pGPRSQoS->reliabilityClass );
            printf("\nPeak Throughput Class        : 0x%X",   pGPRSQoS->peakThroughputClass );
            printf("\nMean Throughput Class        : 0x%X\n", pGPRSQoS->meanThroughputClass );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 27))
    {
        if( pProfile->pUsername )
        {
            printf("\nUser Name                    : %s",      pProfile->pUsername );
        }
        IFPRINTF("\nUser Name Size                  : 0x%x",    pProfile->pUsernameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 28))
    {
        if( pProfile->pPassword )
        {
            printf("\nPassword                    : %s",      pProfile->pPassword );
        }
        IFPRINTF("\nPassword Size                   : 0x%x",    pProfile->pPasswordSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 29))
    {
        IFPRINTF("\nAuthentication Preference       : 0x%X",    pProfile->pAuthenticationPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 30))
    {
        IFPRINTF("\nIPv4 Address Preference         : 0x%X",   pProfile->pIPv4AddrPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 31))
    {
        IFPRINTF("\nPCSCF Address using PCO         : 0x%X",    pProfile->pPcscfAddrUsingPCO );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 32))
    {
        IFPRINTF("\nPDP Access Control Flag         : 0x%X",    pProfile->pPdpAccessConFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 33))
    {
        IFPRINTF("\nPCSCF Address using DHCP        : 0x%X",    pProfile->pPcscfAddrUsingDhcp );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 34))
    {
        IFPRINTF("\nIM CN Flag                      : 0x%X",    pProfile->pImCnFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 35))
    {
        if( pProfile->pTFTID1Params )
        {
            LibPackTFTIDParams *pTFTIDParams = pProfile->pTFTID1Params;
    
            printf("\n\n Traffic Flow Template ID1 Parameters" );
            printf("\nFilter ID                    : 0x%X",    pTFTIDParams->filterId );
            printf("\nEvaluation Precedence ID     : 0x%X",    pTFTIDParams->eValid );
            printf("\nIP Version                   : 0x%X",    pTFTIDParams->ipVersion );
    
            printf("\nSource IP                   : ");
            if( IPV6 == pTFTIDParams->ipVersion )
            {
                if(pTFTIDParams->pSourceIP)
                {
                    for( idx = 0; idx < 8; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                        if (7 != idx)
                        {
                            printf(":" );
                        }
                    }
                }
            }
            else
            {
                if(pTFTIDParams->pSourceIP)
                {
                   printf("0x");
                   for( idx = 0; idx < 2; idx++ )
                   {
                       printf("%X", pTFTIDParams->pSourceIP[idx] );
                   }
                }
            }
            printf("\nSource IP mask               : 0x%X",    pTFTIDParams->sourceIPMask );
            printf("\nNext Header                  : 0x%X",    pTFTIDParams->nextHeader );
            printf("\nDestination Port Range Start : 0x%X",    pTFTIDParams->destPortRangeStart );
            printf("\nDestination Port Range End   : 0x%X",    pTFTIDParams->destPortRangeEnd );
            printf("\nSource Port Range Start      : 0x%X",    pTFTIDParams->srcPortRangeStart );
            printf("\nSource Port Range End        : 0x%X",    pTFTIDParams->srcPortRangeEnd );
            printf("\nIPSec SPI                    : 0x%X",   pTFTIDParams->IPSECSPI );
            printf("\nTOS Mask                     : 0x%X",    pTFTIDParams->tosMask );
            printf("\nFlow Label                   : 0x%X\n", pTFTIDParams->flowLabel );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 36))
    {
        if( pProfile->pTFTID2Params )
        {
            LibPackTFTIDParams *pTFTIDParams = pProfile->pTFTID2Params;

            printf("\n\n Traffic Flow Template ID2 Parameters" );
            printf("\nFilter ID                    : 0x%X",    pTFTIDParams->filterId );
            printf("\nEvaluation Precedence ID     : 0x%X",    pTFTIDParams->eValid );
            printf("\nIP Version                   : 0x%X",    pTFTIDParams->ipVersion );

            printf("\nSource IP                   : ");
            if( IPV6 == pTFTIDParams->ipVersion )
            {
                if(pTFTIDParams->pSourceIP)
                {
                    for( idx = 0; idx < 8; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                        if (7 != idx)
                        {
                            printf( ":" );
                        }
                    }
                }
            }
            else
            {
                if(pTFTIDParams->pSourceIP)
                {
                    printf("0x");
                    for( idx = 0; idx < 2; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                    }
                }
            }

            printf("\nSource IP mask               : 0x%X",    pTFTIDParams->sourceIPMask );
            printf("\nNext Header                  : 0x%X",    pTFTIDParams->nextHeader );
            printf("\nDestination Port Range Start : 0x%X",    pTFTIDParams->destPortRangeStart );
            printf("\nDestination Port Range End   : 0x%X",    pTFTIDParams->destPortRangeEnd );
            printf("\nSource Port Range Start      : 0x%X",    pTFTIDParams->srcPortRangeStart );
            printf("\nSource Port Range End        : 0x%X",    pTFTIDParams->srcPortRangeEnd );
            printf("\nIPSec SPI                    : 0x%X",   pTFTIDParams->IPSECSPI );
            printf("\nTOS Mask                     : 0x%X",    pTFTIDParams->tosMask );
            printf("\nFlow Label                   : 0x%X\n",   pTFTIDParams->flowLabel );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 37))
    {
        if(pProfile->pPdpContext)
        IFPRINTF("\nPDP Context Number              : 0x%X",    pProfile->pPdpContext );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 38))
    {
        if(pProfile->pSecondaryFlag)
        IFPRINTF("\nSecondary Flag                  : 0x%X",    pProfile->pSecondaryFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 39))
    {
        if(pProfile->pPrimaryID)
        IFPRINTF("\nPDP Context Primary ID          : 0x%X",  pProfile->pPrimaryID );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 40))
    {
        if( pProfile->pIPv6AddPref )
        {
            printf("\nIPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
                printf("%X", pProfile->pIPv6AddPref[idx] );
                if (7 != idx)
                {
                    printf(":" );
                }
            }
        }
    }
    
    if(swi_uint256_get_bit (result->ParamPresenceMask, 41))
    {
        if( pProfile->pUMTSReqQoSSigInd )
        {
            LibPackUMTSQoS *pUMTSQoS = &(pProfile->pUMTSReqQoSSigInd->UMTSReqQoS);

            printf("\n\n UMTS Requested QoS with Signalling Indication" );
            printf("\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X",   pUMTSQoS->trafficPriority );
            printf("\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSReqQoSSigInd->SigInd );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 42))
    {
        if( pProfile->pUMTSMinQosSigInd )
        {
            LibPackUMTSQoS *pUMTSQoS = &(pProfile->pUMTSMinQosSigInd->UMTSReqQoS);

            printf("\n\n UMTS Minimum QoS with Signalling Indication" );
            printf("\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X",   pUMTSQoS->trafficPriority );
            printf("\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSMinQosSigInd->SigInd );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 43))
    {
        if( pProfile->pPriDNSIPv6addpref )
        {
            printf("\nPrimary DNS IPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
               printf("%X", pProfile->pPriDNSIPv6addpref[idx] );
               if (7 != idx)
               {
                   printf(":" );
               }
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 44))
    {
        if( pProfile->pSecDNSIPv6addpref )
        {
            printf("\nSecondary DNS IPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
                printf("%X", pProfile->pSecDNSIPv6addpref[idx] );
                if (7 != idx)
                {
                    printf(":" );
                }
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 45))
    {
        IFPRINTF("\nAddress Allocation Preference   : 0x%X",    pProfile->pAddrAllocPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 46))
    {
        if( pProfile->pQosClassID )
        {
            LibPackQosClassID *pQosClassID = pProfile->pQosClassID;

            printf("\n\n 3GPP LTE QoS Parameters" );
            printf("\nQoS Class identifier         : 0x%X",    pQosClassID->QCI );
            printf("\nGuaranteed DL Bit Rate       : 0x%X",   pQosClassID->gDlBitRate );
            printf("\nMaximum DL Bit Rate          : 0x%X",   pQosClassID->maxDlBitRate );
            printf("\nGuaranteed UL Bit Rate       : 0x%X",   pQosClassID->gUlBitRate );
            printf("\nMaximum UL Bit Rate          : 0x%X\n", pQosClassID->maxUlBitRate );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 47))
    {
        IFPRINTF("\nAPN disabled flag               : 0x%X",    pProfile->pAPNDisabledFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 48))
    {
        IFPRINTF("\nPDN Inactivity Timeout          : 0x%X",   pProfile->pPDNInactivTimeout );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 49))
    {
        IFPRINTF("\nAPN class                       : 0x%X",    pProfile->pAPNClass );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 54))
    {
        IFPRINTF("\nEmergency Call Support          : 0x%X",    pProfile->pSupportEmergencyCalls );
    }
    printf("\n");

#if DEBUG_LOG_TO_FILE
    /* expected result, profile id should not equal to 0 */
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSGetProfileSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSGetProfileSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if(result->pProfileSettings!=NULL)
    {
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",((1 == swi_uint256_get_bit (result->ParamPresenceMask, 20) ) ? "Correct": "Wrong"));
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n", ((1 == swi_uint256_get_bit(result->ParamPresenceMask, 20) ) ? "Correct": "Wrong"));
        }
    }
    else
        local_fprintf("%s\n", "N/A");
#endif
}

void dump_wds_SLQSGetProfileSettingsV2(void* ptr)
{
    unpack_wds_SLQSGetProfileSettingsV2_t *result =
        (unpack_wds_SLQSGetProfileSettingsV2_t*) ptr;
    uint8_t  idx  = 0;
    LibpackProfile3GPPV2 *pProfile = NULL;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    RETURN_IF_PTR_IS_NULL(result->pProfileSettings)
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    pProfile = &result->pProfileSettings->curProfile.SlqsProfile3GPP;
    RETURN_IF_PTR_IS_NULL(pProfile)
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        if( pProfile->pProfilename )
        {
            printf("\nProfile Name                  : %s",      pProfile->pProfilename );
        }
        IFPRINTF("\nProfile Name Size               : 0x%x",    pProfile->pProfilenameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        IFPRINTF("\nPDP Type                        : 0x%x",    pProfile->pPDPtype );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        IFPRINTF("\nPDP Header Compression Type     : 0x%X",    pProfile->pPdpHdrCompType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        IFPRINTF("\nPDP data Compression Type       : 0x%X",    pProfile->pPdpDataCompType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        if( pProfile->pAPNName )
        {
            printf("\nAPN Name                      : %s",      pProfile->pAPNName );
        }
        IFPRINTF("\nAPN Name Size                   : 0x%x",   pProfile->pAPNnameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    {
        IFPRINTF("\nPrimary DNS IPv4 address Pref.  : 0x%X",   pProfile->pPriDNSIPv4AddPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    {
        IFPRINTF("\nSecondary DNS IPv4 address Pref.: 0x%X",   pProfile->pSecDNSIPv4AddPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    {
        if( pProfile->pUMTSReqQoS )
        {
            LibPackUMTSQoS *pUMTSQoS = pProfile->pUMTSReqQoS;

            printf("\n\n UMTS Requested QoS" );
            printf("\nTraffic Class                 : 0x%X",   pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate       : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate     : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate    : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate  : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order            : 0x%X",   pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size              : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio               : 0x%X",   pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio      : 0x%X",   pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's      : 0x%X",   pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay                : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority     : 0x%X\n", pUMTSQoS->trafficPriority );
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
    {
        if( pProfile->pUMTSMinQoS )
        {
            LibPackUMTSQoS *pUMTSQoS = pProfile->pUMTSMinQoS;

            printf("\n\n UMTS Minimum QoS" );
            printf("\nTraffic Class                 : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate       : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate     : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate    : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate  : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order            : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size              : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio               : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio      : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's      : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay                : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority     : 0x%X\n", pUMTSQoS->trafficPriority );
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
    {        
        if( pProfile->pGPRSRequestedQos )
        {
            LibPackGPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSRequestedQos;

            printf("\n\n GPRS Requested QoS" );
            printf("\nPrecedence Class              : 0x%X",   pGPRSQoS->precedenceClass );
            printf("\nDelay Class                   : 0x%X",   pGPRSQoS->delayClass );
            printf("\nReliability Class             : 0x%X",   pGPRSQoS->reliabilityClass );
            printf("\nPeak Throughput Class         : 0x%X",   pGPRSQoS->peakThroughputClass );
            printf("\nMean Throughput Class         : 0x%X\n", pGPRSQoS->meanThroughputClass );
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
    {
        if( pProfile->pGPRSMinimumQoS )
        {
            LibPackGPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSMinimumQoS;

            printf("\n\n GPRS Minimum QoS" );
            printf("\nPrecedence Class              : 0x%X",   pGPRSQoS->precedenceClass );
            printf("\nDelay Class                   : 0x%X",   pGPRSQoS->delayClass );
            printf("\nReliability Class             : 0x%X",   pGPRSQoS->reliabilityClass );
            printf("\nPeak Throughput Class         : 0x%X",   pGPRSQoS->peakThroughputClass );
            printf("\nMean Throughput Class         : 0x%X\n", pGPRSQoS->meanThroughputClass );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 27))
    {
        if( pProfile->pUsername )
        {
            printf("\nUser Name                     : %s",      pProfile->pUsername );
        }
        IFPRINTF("\nUser Name Size                  : 0x%x",    pProfile->pUsernameSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 28))
    {
        if( pProfile->pPassword )
        {
            printf("\nPassword                      : %s",      pProfile->pPassword );
        }
        IFPRINTF("\nPassword Size                   : 0x%x",    pProfile->pPasswordSize );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 29))
    {
        IFPRINTF("\nAuthentication Preference       : 0x%X",    pProfile->pAuthenticationPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 30))
    {
        IFPRINTF("\nIPv4 Address Preference         : 0x%X",   pProfile->pIPv4AddrPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 31))
    {
        IFPRINTF("\nPCSCF Address using PCO         : 0x%X",    pProfile->pPcscfAddrUsingPCO );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 32))
    {
        IFPRINTF("\nPDP Access Control Flag         : 0x%X",    pProfile->pPdpAccessConFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 33))
    {
        IFPRINTF("\nPCSCF Address using DHCP        : 0x%X",    pProfile->pPcscfAddrUsingDhcp );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 34))
    {
        IFPRINTF("\nIM CN Flag                      : 0x%X",    pProfile->pImCnFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 35))
    {
        if( pProfile->pTFTID1Params )
        {
            LibPackTFTIDParams *pTFTIDParams = pProfile->pTFTID1Params;

            printf("\n\n Traffic Flow Template ID1 Parameters" );
            printf("\nFilter ID                     : 0x%X",    pTFTIDParams->filterId );
            printf("\nEvaluation Precedence ID      : 0x%X",    pTFTIDParams->eValid );
            printf("\nIP Version                    : 0x%X",    pTFTIDParams->ipVersion );

            printf("\nSource IP                     : ");
            if( IPV6 == pTFTIDParams->ipVersion )
            {
                if(pTFTIDParams->pSourceIP)
                {
                    for( idx = 0; idx < 8; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                        if (7 != idx)
                        {
                            printf(":" );
                        }
                    }
                }
            }
            else
            {
                if(pTFTIDParams->pSourceIP)
                {
                   printf("0x");
                   for( idx = 0; idx < 2; idx++ )
                   {
                       printf("%X", pTFTIDParams->pSourceIP[idx] );
                   }
                }
            }
            printf("\nSource IP mask                : 0x%X",    pTFTIDParams->sourceIPMask );
            printf("\nNext Header                   : 0x%X",    pTFTIDParams->nextHeader );
            printf("\nDestination Port Range Start  : 0x%X",    pTFTIDParams->destPortRangeStart );
            printf("\nDestination Port Range End    : 0x%X",    pTFTIDParams->destPortRangeEnd );
            printf("\nSource Port Range Start       : 0x%X",    pTFTIDParams->srcPortRangeStart );
            printf("\nSource Port Range End         : 0x%X",    pTFTIDParams->srcPortRangeEnd );
            printf("\nIPSec SPI                     : 0x%X",   pTFTIDParams->IPSECSPI );
            printf("\nTOS Mask                      : 0x%X",    pTFTIDParams->tosMask );
            printf("\nFlow Label                    : 0x%X\n", pTFTIDParams->flowLabel );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 36))
    {
        if( pProfile->pTFTID2Params )
        {
            LibPackTFTIDParams *pTFTIDParams = pProfile->pTFTID2Params;

            printf("\n\n Traffic Flow Template ID2 Parameters" );
            printf("\nFilter ID                     : 0x%X",    pTFTIDParams->filterId );
            printf("\nEvaluation Precedence ID      : 0x%X",    pTFTIDParams->eValid );
            printf("\nIP Version                    : 0x%X",    pTFTIDParams->ipVersion );

            printf("\nSource IP                    : ");
            if( IPV6 == pTFTIDParams->ipVersion )
            {
                if(pTFTIDParams->pSourceIP)
                {
                    for( idx = 0; idx < 8; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                        if (7 != idx)
                        {
                            printf( ":" );
                        }
                    }
                }
            }
            else
            {
                if(pTFTIDParams->pSourceIP)
                {
                    printf("0x");
                    for( idx = 0; idx < 2; idx++ )
                    {
                        printf("%X", pTFTIDParams->pSourceIP[idx] );
                    }
                }
            }

            printf("\nSource IP mask               : 0x%X",    pTFTIDParams->sourceIPMask );
            printf("\nNext Header                  : 0x%X",    pTFTIDParams->nextHeader );
            printf("\nDestination Port Range Start : 0x%X",    pTFTIDParams->destPortRangeStart );
            printf("\nDestination Port Range End   : 0x%X",    pTFTIDParams->destPortRangeEnd );
            printf("\nSource Port Range Start      : 0x%X",    pTFTIDParams->srcPortRangeStart );
            printf("\nSource Port Range End        : 0x%X",    pTFTIDParams->srcPortRangeEnd );
            printf("\nIPSec SPI                    : 0x%X",   pTFTIDParams->IPSECSPI );
            printf("\nTOS Mask                     : 0x%X",    pTFTIDParams->tosMask );
            printf("\nFlow Label                   : 0x%X\n",   pTFTIDParams->flowLabel );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 37))
    {
        if(pProfile->pPdpContext)
        IFPRINTF("\nPDP Context Number             : 0x%X",    pProfile->pPdpContext );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 38))
    {
        if(pProfile->pSecondaryFlag)
        IFPRINTF("\nSecondary Flag                 : 0x%X",    pProfile->pSecondaryFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 39))
    {
        if(pProfile->pPrimaryID)
        IFPRINTF("\nPDP Context Primary ID         : 0x%X",  pProfile->pPrimaryID );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 40))
    {
        if( pProfile->pIPv6AddPref )
        {
            printf("\nIPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
                printf("%X", pProfile->pIPv6AddPref[idx] );
                if (7 != idx)
                {
                    printf(":" );
                }
            }
        }
    }

    if(swi_uint256_get_bit (result->ParamPresenceMask, 41))
    {
        if( pProfile->pUMTSReqQoSSigInd )
        {
            LibPackUMTSQoS *pUMTSQoS = &(pProfile->pUMTSReqQoSSigInd->UMTSReqQoS);

            printf("\n\n UMTS Requested QoS with Signalling Indication" );
            printf("\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X",   pUMTSQoS->trafficPriority );
            printf("\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSReqQoSSigInd->SigInd );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 42))
    {
        if( pProfile->pUMTSMinQosSigInd )
        {
            LibPackUMTSQoS *pUMTSQoS = &(pProfile->pUMTSMinQosSigInd->UMTSReqQoS);

            printf("\n\n UMTS Minimum QoS with Signalling Indication" );
            printf("\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
            printf("\nMaximum Uplink Bit Rate      : 0x%X",   pUMTSQoS->maxUplinkBitrate );
            printf("\nMaximum Downlink Bit Rate    : 0x%X",   pUMTSQoS->maxDownlinkBitrate );
            printf("\nGuaranteed Uplink Bit Rate   : 0x%X",   pUMTSQoS->grntUplinkBitrate );
            printf("\nGuaranteed Downlink Bit Rate : 0x%X",   pUMTSQoS->grntDownlinkBitrate );
            printf("\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
            printf("\nMaximum SDU Size             : 0x%X",   pUMTSQoS->maxSDUSize );
            printf("\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
            printf("\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
            printf("\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
            printf("\nTransfer Delay               : 0x%X",   pUMTSQoS->transferDelay );
            printf("\nTraffic Handling Priority    : 0x%X",   pUMTSQoS->trafficPriority );
            printf("\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSMinQosSigInd->SigInd );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 43))
    {
        if( pProfile->pPriDNSIPv6addpref )
        {
            printf("\nPrimary DNS IPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
               printf("%X", pProfile->pPriDNSIPv6addpref[idx] );
               if (7 != idx)
               {
                   printf(":" );
               }
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 44))
    {
        if( pProfile->pSecDNSIPv6addpref )
        {
            printf("\nSecondary DNS IPV6 address Preference: ");
            for( idx = 0; idx < 8; idx++ )
            {
                printf("%X", pProfile->pSecDNSIPv6addpref[idx] );
                if (7 != idx)
                {
                    printf(":" );
                }
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 45))
    {
        IFPRINTF("\nAddress Allocation Preference  : 0x%X",    pProfile->pAddrAllocPref );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 46))
    {
        if( pProfile->pQosClassID )
        {
            LibPackQosClassID *pQosClassID = pProfile->pQosClassID;

            printf("\n\n 3GPP LTE QoS Parameters" );
            printf("\nQoS Class identifier         : 0x%X",    pQosClassID->QCI );
            printf("\nGuaranteed DL Bit Rate       : 0x%X",   pQosClassID->gDlBitRate );
            printf("\nMaximum DL Bit Rate          : 0x%X",   pQosClassID->maxDlBitRate );
            printf("\nGuaranteed UL Bit Rate       : 0x%X",   pQosClassID->gUlBitRate );
            printf("\nMaximum UL Bit Rate          : 0x%X\n", pQosClassID->maxUlBitRate );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 47))
    {
        IFPRINTF("\nAPN disabled flag              : 0x%X",    pProfile->pAPNDisabledFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 48))
    {
        IFPRINTF("\nPDN Inactivity Timeout         : 0x%X",   pProfile->pPDNInactivTimeout );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 49))
    {
        IFPRINTF("\nAPN class                      : 0x%X",    pProfile->pAPNClass );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 53))
    {
        IFPRINTF("\nAPN Bearer Mask                : %"PRIu64"\n",    pProfile->pAPNBearer );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 54))
    {
        IFPRINTF("\nEmergency Call Support         : 0x%X",    pProfile->pSupportEmergencyCalls );
    }
    printf("\n");
    if(swi_uint256_get_bit (result->ParamPresenceMask, 55))
    {
        IFPRINTF("\nOperator PCO ID                : 0x%X",    pProfile->pOperatorPCOID );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 56))
    {
        IFPRINTF("\nMCC                            : 0x%X",    pProfile->pMcc );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 57))
    {
        if( pProfile->pMnc )
        {
            LibPackProfileMnc           *pMnc = pProfile->pMnc;

            printf("\n\n3GPP MNC Parameters:" );
            printf("\n MNC                         : 0x%X",    pMnc->MNC );
            printf("\n PCSFlag                     : 0x%X",   pMnc->PCSFlag );
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 58))
    {
        IFPRINTF("\nMax PDN                        : 0x%X",    pProfile->pMaxPDN );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 59))
    {
        IFPRINTF("\nMax PDN Timer                  : 0x%X",    pProfile->pMaxPDNTimer );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 60))
    {
        IFPRINTF("\nPDN Wait Timer                 : 0x%X",    pProfile->pPDNWaitTimer );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 61))
    {
        IFPRINTF("\nApp user data Id               : 0x%X",    pProfile->pAppUserData );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 62))
    {
        IFPRINTF("\nRoaming disallow flag          : 0x%X",    pProfile->pRoamDisallowFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 63))
    {
        IFPRINTF("\nPDN Disconnect Timer           : 0x%X",    pProfile->pPDNDisconnectWaitTimer );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 64))
    {
        IFPRINTF("\nDNS with DHCP Flag             : 0x%X",    pProfile->pDnsWithDHCPFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 65))
    {
        IFPRINTF("\nLTE Roam PDP Type              : 0x%X",    pProfile->pLteRoamPDPType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 66))
    {
        IFPRINTF("\nUMTS Roam PDP Type             : 0x%X",    pProfile->pUmtsRoamPDPType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 67))
    {
        IFPRINTF("\nIWLANtoLTE Handover Flag       : 0x%X",    pProfile->pIWLANtoLTEHandoverFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 68))
    {
        IFPRINTF("\nLTEtoIWLAN Handover Flag       : 0x%X",    pProfile->pLTEtoIWLANHandoverFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 69))
    {
        if( pProfile->pPDNThrottleTimer )
        {
            LibPackPDNThrottleTimer    *pPDNThrottleTimer = pProfile->pPDNThrottleTimer;

            printf("\n\n3GPP PDN Throttle Timer:" );
            for( idx = 0; idx < LITE_MAX_PDN_THROTTLE_TIMER; idx++ )
            {
                printf("\n Throttle Timer          : 0x%X",    pPDNThrottleTimer->ThrottleTimer[idx] );
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 70))
    {
        IFPRINTF("\nOverride PDP Type              : 0x%X",    pProfile->pOverridePDPType );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 71))
    {
        if( pProfile->pPCOIDList )
        {
            LibPackPCOIDList    *pPCOIDList = pProfile->pPCOIDList;

            printf("\n\n3GPP PCO ID List:" );
            for( idx = 0; idx < LITE_MAX_PCOID_LIST; idx++ )
            {
                printf("\n PCO ID                  : 0x%X",    pPCOIDList->PcoList[idx] );
            }
        }
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 72))
    {
        IFPRINTF("\nMSISDN Flag                    : 0x%X",    pProfile->pMsisdnFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 143))
    {
        IFPRINTF("\nPersist Flag                   : 0x%X",    pProfile->pPersistFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 222))
    {
        IFPRINTF( "\nCLAT Flag                     : 0x%X",  pProfile->pClatFlag );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 223))
    {
        IFPRINTF( "\nIPV6 Prefix Delegation Flag   : 0x%X",  pProfile->pIPV6DelegFlag );
    }

    printf("\n");

#if DEBUG_LOG_TO_FILE
    /* expected result, profile id should not equal to 0 */
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSGetProfileSettingsParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSGetProfileSettingsParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);

    if(result->pProfileSettings!=NULL)
    {
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",((1 == swi_uint256_get_bit (result->ParamPresenceMask, 20) ) ? "Correct": "Wrong"));
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n", ((1 == swi_uint256_get_bit(result->ParamPresenceMask, 20) ) ? "Correct": "Wrong"));
        }
    }
    else
        local_fprintf("%s\n", "N/A");
#endif
}


void dump_wds_SLQSGetProfileSettings_3GPP2(void* ptr)
{
    unpack_wds_SLQSGetProfileSettings_t *result =
        (unpack_wds_SLQSGetProfileSettings_t*) ptr;
    uint8_t  idx  = 0;
    LibpackProfile3GPP2 *pProfile = NULL;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return;
    }
    RETURN_IF_PTR_IS_NULL(result->pProfileSettings);
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    pProfile = &result->pProfileSettings->curProfile.SlqsProfile3GPP2;
    if(pProfile)
    {
        if(swi_uint256_get_bit (result->ParamPresenceMask, 144))
        {
            IFPRINTF( "\nNegotiate DNS Server Preference : 0x%X",  pProfile->pNegoDnsSrvrPref );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 145))
        {
            IFPRINTF( "\nPPP Session close Timer for DO  : 0x%X",  pProfile->pPppSessCloseTimerDO );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 146))
        {
            IFPRINTF( "\nPPP Session close Timer for 1X  : 0x%X",  pProfile->pPppSessCloseTimer1x );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 147))
        {
            IFPRINTF( "\nAllow Linger                    : 0x%X",  pProfile->pAllowLinger );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 148))
        {
            IFPRINTF( "\nLCP ACK Timeout                 : 0x%X",  pProfile->pLcpAckTimeout );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 149))
        {
            IFPRINTF( "\nIPCP ACK Timeout                : 0x%X",  pProfile->pIpcpAckTimeout );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 150))
        {
            IFPRINTF( "\nAuth Timeout                    : 0x%X",  pProfile->pAuthTimeout );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 151))
        {
            IFPRINTF( "\nLCP Config. Request Retry Count : 0x%X",  pProfile->pLcpCreqRetryCount );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 152))
        {
            IFPRINTF( "\nIPCP Config. Request Retry Count: 0x%X",  pProfile->pIpcpCreqRetryCount );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 153))
        {
            IFPRINTF( "\nAuthentication Retry Count      : 0x%X",  pProfile->pAuthRetryCount );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 154))
        {
            IFPRINTF( "\nAuthentication Protocol         : 0x%X",  pProfile->pAuthProtocol );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 155))
        {
            if( pProfile->pUserId )
            {
                printf( "\nUser ID                      : %s",    pProfile->pUserId );
            }
            IFPRINTF( "\nUser ID Size                    : 0x%X",  pProfile->pUserIdSize );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 156))
        {
            if( pProfile->pAuthPassword )
            {
                printf( "\nAuthentication Password      : %s",    pProfile->pAuthPassword );
            }
            IFPRINTF( "\nAuthentication Password Size    : 0x%X",  pProfile->pAuthPasswordSize );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 157))
        {
            IFPRINTF( "\nData Rate                       : 0x%X",  pProfile->pDataRate );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 158))
        {
            IFPRINTF( "\nApplication Type                : 0x%X", pProfile->pAppType );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 159))
        {
            IFPRINTF( "\nData Mode                       : 0x%X",  pProfile->pDataMode );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 160))
        {
            IFPRINTF( "\nApplication Priority            : 0x%X",  pProfile->pAppPriority );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 161))
        {
            if( pProfile->pApnString )
            {
                printf( "\nAPN String                   : %s",    pProfile->pApnString );
            }
            IFPRINTF( "\nAPN String Size                 : 0x%X",  pProfile->pApnStringSize );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 162))
        {
            IFPRINTF( "\nPDN Type                        : 0x%X",  pProfile->pPdnType );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 163))
        {
            IFPRINTF( "\nIs Pcscf Address Needed         : 0x%X",  pProfile->pIsPcscfAddressNedded );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 164))
        {
            IFPRINTF( "\nIPv4 Primary DNS Address        : 0x%X", pProfile->pPrimaryV4DnsAddress );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 165))
        {
            IFPRINTF( "\nIPv4 Secondary DNS Address      : 0x%X", pProfile->pSecondaryV4DnsAddress );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 166))
        {
            if( pProfile->pPriV6DnsAddress )
            {
                printf( "\nPrimary DNS IPV6 address     : ");
                for( idx = 0; idx < 8; idx++ )
                {
                   printf( "%X", pProfile->pPriV6DnsAddress[idx] );
                   if (7 != idx)
                   {
                       printf( ":" );
                   }
                }
            }
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 167))
        {
            if( pProfile->pSecV6DnsAddress )
            {
                printf( "\nSecondary DNS IPV6 address   : ");
                for( idx = 0; idx < 8; idx++ )
                {
                    printf( "%X", pProfile->pSecV6DnsAddress[idx] );
                    if (7 != idx)
                    {
                        printf( ":" );
                    }
                }
            }
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 168))
        {
            IFPRINTF( "\nRAT Type                        : 0x%X",  pProfile->pRATType );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 169))
        {
            IFPRINTF( "\nAPN Enabled                     : 0x%X",  pProfile->pAPNEnabled3GPP2 );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 170))
        {
            IFPRINTF( "\nPDN Inactivity Timeout          : 0x%X", pProfile->pPDNInactivTimeout3GPP2 );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 171))
        {
            IFPRINTF( "\nAPN class                       : 0x%X",  pProfile->pAPNClass3GPP2 );
        }
    }// if(pProfile)
    if(swi_uint256_get_bit (result->ParamPresenceMask, 224))
    {
        if(result->pProfileSettings->pExtErrCode)
        {
            IFPRINTF( "\nExterned Error Code             : 0x%X",  result->pProfileSettings->pExtErrCode );
        }
    }
    printf( "\n" );

#if DEBUG_LOG_TO_FILE
    /* expected result, profile id should not equal to 0 */
    if(result->pProfileSettings!=NULL)
        local_fprintf("%s\n", ((NULL != result->pProfileSettings->curProfile.SlqsProfile3GPP2.pApnString )? "Correct": "Wrong"));
#endif
}

void dump_wds_SLQSModifyProfile(void* ptr)
{
    unpack_wds_SLQSModifyProfile_t *result = 
        (unpack_wds_SLQSModifyProfile_t*)ptr;
    
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 224))
    {
        if(result->pExtErrorCode)
        {
            printf("ExtErrorCode:%d\n",*result->pExtErrorCode);
        }
    }
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSModifyProfileParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSModifyProfileParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "Correct");
#endif
}

void dump_wds_SLQSSetWdsEventCallback(void* ptr)
{
    unpack_wds_SLQSSetWdsEventCallback_t *result = 
        (unpack_wds_SLQSSetWdsEventCallback_t*)ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSSetWdsEventCallbackParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSSetWdsEventCallbackParamPresenceMaskMandatoryList,
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
}

void dump_wds_SetMuxID(void* ptr)
{
    UNUSEDPARAM(ptr);
    unpack_wds_SetMuxID_t *result = 
        (unpack_wds_SetMuxID_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(result->ParamPresenceMask);
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

void dump_wds_SLQSStartDataSession(void* ptr)
{
    unpack_wds_SLQSStartDataSession_t *pstartdatas =
        (unpack_wds_SLQSStartDataSession_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pstartdatas->ParamPresenceMask);
    if((pstartdatas->psid!=NULL) && 
        swi_uint256_get_bit (pstartdatas->ParamPresenceMask, 1))
    {
        IFPRINTF("Data Session ID: 0x%X\n", pstartdatas->psid);
    }
    if((pstartdatas->pFailureReason!=NULL) && 
        swi_uint256_get_bit (pstartdatas->ParamPresenceMask, 16))
    {
        IFPRINTF("failure reason: %u\n", pstartdatas->pFailureReason);
    }
    if((pstartdatas->pVerboseFailReasonType!=NULL) && 
        swi_uint256_get_bit (pstartdatas->ParamPresenceMask, 17))
    {
        IFPRINTF("verbose failure reason type: %u\n", pstartdatas->pVerboseFailReasonType);
    }
    if((pstartdatas->pVerboseFailureReason!=NULL) && 
        swi_uint256_get_bit (pstartdatas->ParamPresenceMask, 17))
    {
        IFPRINTF("verbose failure reason : %u\n", pstartdatas->pVerboseFailureReason);
    }

#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSStartDataSessionParamPresenceMaskWhiteList,
        pstartdatas->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSStartDataSessionParamPresenceMaskMandatoryList,
        pstartdatas->ParamPresenceMask);
    if ( pstartdatas->psid != NULL )
    {
        if ( unpackRetCode != 0 )
        {
            local_fprintf("%s,",  (*(pstartdatas->psid) > 0) ? "Correct": "Wrong");
            local_fprintf("%s\n", remark);
        }
        else
        {
            local_fprintf("%s\n",  (*(pstartdatas->psid) > 0) ? "Correct": "Wrong");
        }
    }
    else
        local_fprintf("%s\n",  "N/A");
#endif
}
void dump_wds_SLQSStopDataSession(void* ptr)
{
    UNUSEDPARAM(ptr);
    unpack_wds_SLQSStopDataSession_t *result =
        (unpack_wds_SLQSStopDataSession_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    if(!result)
        return ;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSStopDataSessionParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSStopDataSessionParamPresenceMaskMandatoryList,
        result->ParamPresenceMask);
    local_fprintf("%s\n",  "Correct");
#endif
}

void dump_GetConnectionRate(void* ptr)
{
    unpack_wds_GetConnectionRate_t *pconnrate =
        (unpack_wds_GetConnectionRate_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pconnrate->ParamPresenceMask);
    if(swi_uint256_get_bit (pconnrate->ParamPresenceMask, 1))
    {
        printf("current ch tx rate: 0x%X\n", pconnrate->currentChannelTXRate);
        printf("current ch rx rate: 0x%X\n", pconnrate->currentChannelRXRate);
        printf("max tx rate: 0x%X\n", pconnrate->maxChannelTXRate);
        printf("max rx rate: 0x%X\n", pconnrate->maxChannelRXRate);
    }    
#if DEBUG_LOG_TO_FILE
    /* expected result, max connection rate should be larger than 0 */
    local_fprintf("%s\n", ((0 < pconnrate->maxChannelTXRate )? "Correct": "Wrong"));
#endif
}

void dump_GetSessionDuration(void* ptr)
{

    unpack_wds_GetSessionDuration_t *psessionduration =
        (unpack_wds_GetSessionDuration_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (psessionduration->ParamPresenceMask);
    if(swi_uint256_get_bit (psessionduration->ParamPresenceMask, 1))
    {
        printf("session duration: %"PRIu64"\n", psessionduration->callDuration);
    }
#if DEBUG_LOG_TO_FILE
    /* expected result, duration should be larger than 0 */
    CHECK_WHITELIST_MASK(
        unpack_wds_GetSessionDurationParamPresenceMaskWhiteList,
        psessionduration->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_GetSessionDurationParamPresenceMaskMandatoryList,
        psessionduration->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((0 < psessionduration->callDuration ) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((0 < psessionduration->callDuration ) ? "Correct": "Wrong"));
    }
#endif 
}

void dump_GetSessionDurationV2(void* ptr)
{

    unpack_wds_GetSessionDurationV2_t *psessionduration =
        (unpack_wds_GetSessionDurationV2_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (psessionduration->ParamPresenceMask);
    if(swi_uint256_get_bit (psessionduration->ParamPresenceMask, 1))
    {
        printf("session duration: %"PRIu64"\n", psessionduration->callDuration);
    }
    if(swi_uint256_get_bit (psessionduration->ParamPresenceMask, 16))
    {
        IFPRINTF("\nlast Call duration            : %"PRIu64"", psessionduration->pLastCallDuration );
    }
    if(swi_uint256_get_bit (psessionduration->ParamPresenceMask, 17))
    {
        IFPRINTF("\ncall active duration          : %"PRIu64"", psessionduration->pCallActiveDuration );
    }
    if(swi_uint256_get_bit (psessionduration->ParamPresenceMask, 18))
    {
        IFPRINTF("\nlast call active duration     : %"PRIu64"", psessionduration->pLastCallActiveDuration );
    }
#if DEBUG_LOG_TO_FILE
    /* expected result, duration should be larger than 0 */
    CHECK_WHITELIST_MASK(
        unpack_wds_GetSessionDurationParamPresenceMaskWhiteList,
        psessionduration->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_GetSessionDurationParamPresenceMaskMandatoryList,
        psessionduration->ParamPresenceMask);
    if ( unpackRetCode != 0 )
    {
        local_fprintf("%s,", ((0 < psessionduration->callDuration ) ? "Correct": "Wrong"));
        local_fprintf("%s\n", remark);
    }
    else
    {
        local_fprintf("%s\n", ((0 < psessionduration->callDuration ) ? "Correct": "Wrong"));
    }
#endif
}

void dump_GetDormancyState(void* ptr)
{
    /* expected result, dormancy status should be 2 (channel active) */
    unpack_wds_GetDormancyState_t *pdormancystate =
        (unpack_wds_GetDormancyState_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pdormancystate->ParamPresenceMask);
    if(swi_uint256_get_bit (pdormancystate->ParamPresenceMask, 1))
    {
        printf("dormancy state: %d\n", pdormancystate->dormancyState);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching=0;
    if ( (pdormancystate->dormancyState ==1) || (pdormancystate->dormancyState ==2))
        is_matching = 1;

    CHECK_WHITELIST_MASK(
        unpack_wds_GetDormancyStateParamPresenceMaskWhiteList,
        pdormancystate->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_GetDormancyStateParamPresenceMaskMandatoryList,
        pdormancystate->ParamPresenceMask);

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

void dump_SLQSSet3GPPConfigItem(void* ptr)
{
    unpack_wds_SLQSSet3GPPConfigItem_t *result = 
        (unpack_wds_SLQSSet3GPPConfigItem_t*) ptr;

    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSSet3GPPConfigItemParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSSet3GPPConfigItemParamPresenceMaskMandatoryList,
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
}

void dump_SetDefaultProfileNum(void* ptr)
{
   unpack_wds_SetDefaultProfile_t *result = 
    (unpack_wds_SetDefaultProfile_t*)ptr;
   CHECK_DUMP_ARG_PTR_IS_NULL
   printf("%s Data\n",__FUNCTION__);
   swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}
pack_wds_SetDefaultProfileNum_t tpack_wds_SetDefaultProfileNum = {0, 0, 3};

void dump_GetDefaultProfileNum(void* ptr)
{
    unpack_wds_GetDefaultProfileNum_t *p =
        (unpack_wds_GetDefaultProfileNum_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (p->ParamPresenceMask, 1))
    {
        printf("default profile number: %d\n", p->index);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    /* Restore to set default profile number */
    tpack_wds_SetDefaultProfileNum.index = p->index;
}

void dump_SLQSGet3GPPConfigItem(void* ptr)
{
    unpack_wds_SLQSGet3GPPConfigItem_t *p3gppconfigitem =
        (unpack_wds_SLQSGet3GPPConfigItem_t*) ptr;
    uint8_t    count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (p3gppconfigitem->ParamPresenceMask);
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 16))
    {
        printf( "\nLTE Attach Profile : %d", p3gppconfigitem->LTEAttachProfile );
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 17))
    {
        for( count = 0; count < 4; count++ )
        {
             printf( "\n\tprofileList[%d] : %d",
                      count,
                      p3gppconfigitem->profileList[count] );
        }
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 18))
    {
        printf( "\nDefault PDN enabled : %d",
            p3gppconfigitem->defaultPDNEnabled );
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 19))
    {
        printf( "\n3gppRelease : %d\n",
                p3gppconfigitem->_3gppRelease );
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 20))
    {
        printf("LTE attach profile list: %d", p3gppconfigitem->LTEAttachProfileListLen);
        for( count = 0; count < p3gppconfigitem->LTEAttachProfileListLen; count++ )
        {
             printf( "\n\tLTEAttachProfileList[%d] : %d",
                      count,
                      p3gppconfigitem->LTEAttachProfileList[count] );
        }
        printf("\n");
    }
#if DEBUG_LOG_TO_FILE
    /* expected result */
    unpack_wds_SLQSGet3GPPConfigItem_t config_item;
    config_item.profileList[0] = 1;
    uint8_t is_matching = 0;

    if ( config_item.profileList[0] == p3gppconfigitem->profileList[0] )
    {
        is_matching = 1;
    }

    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskWhiteList,
        p3gppconfigitem->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskMandatoryList,
        p3gppconfigitem->ParamPresenceMask);

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

void dump_SLQSGet3GPPConfigItemResult(void* ptr)
{
    unpack_wds_SLQSGet3GPPConfigItem_t *p3gppconfigitem =
        (unpack_wds_SLQSGet3GPPConfigItem_t*) ptr;
    uint8_t cmpResult = 1;

    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (p3gppconfigitem->ParamPresenceMask);

    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 17))
    {
        if (memcmp(p3gppconfigitem->profileList,ProfileList, sizeof(ProfileList)))
            cmpResult = 0;  
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 18))
    {
        if (p3gppconfigitem->defaultPDNEnabled != DefaultPDNEnabled)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 19))
    {
        if (p3gppconfigitem->_3gppRelease != _3gppRelease)
            cmpResult = 0;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 20))
    {
        if (memcmp(p3gppconfigitem->LTEAttachProfileList,LTEAttachProfileList, sizeof(LTEAttachProfileList)))
            cmpResult = 0;
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 0;

    if ( cmpResult == 1 )
    {
        is_matching = 1;
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

void dump_GetMobileIP(void* ptr)
{   
    unpack_wds_GetMobileIP_t *pMobileIP =
        (unpack_wds_GetMobileIP_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pMobileIP->ParamPresenceMask);
    if(swi_uint256_get_bit (pMobileIP->ParamPresenceMask, 1))
    {
        printf("mobile IP: 0x%x\n", pMobileIP->mipMode);
    }

    /* mobile ip mode should not larger than 2 */
#if DEBUG_LOG_TO_FILE
    /* mobile ip mode should not larger than 2 */
    local_fprintf("%s\n", ((2 <= pMobileIP->mipMode )? "Correct": "Wrong"));
#endif 

}
void dump_GetLastMobileIPError(void* ptr)
{
    unpack_wds_GetLastMobileIPError_t *pMobileIPError =
        (unpack_wds_GetLastMobileIPError_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (pMobileIPError->ParamPresenceMask);
    if(swi_uint256_get_bit (pMobileIPError->ParamPresenceMask, 1))
    {
        printf("last mobile IP error: 0x%x\n", pMobileIPError->error);
    }
    /* the error code can be any value, hence treat as always correct */
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n", "Correct");
#endif     
}
void dump_SetMobileIPProfile(void* ptr)
{
    unpack_wds_SetMobileIPProfile_t *result =
        (unpack_wds_SetMobileIPProfile_t*)ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}
void dump_GetMobileIPProfile(void* ptr)
{
    unpack_wds_GetMobileIPProfile_t *result =
        (unpack_wds_GetMobileIPProfile_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    if(result)
    {
        swi_uint256_print_mask (result->ParamPresenceMask);
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        {
            printf("enabled %d\n",  result->enabled);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        {
            printf("ip address: %x\n", result->address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        {
            printf("primary home address: %x\n",  result->primaryHA);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        {
            printf("seconday home address:%x\n",  result->secondaryHA);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        {
            printf("rev tunneling: %d\n",  result->revTunneling);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        {
            printf("HASPI: %d\n",  result->HASPI);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        {
            printf("NAI: %s\n",  result->NAI);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        {
            printf("AASPI: %x\n", result->AAASPI);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
        {
            printf("HA State: %x\n",result->HAState);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 27))
        {
            printf("AAA State: %x\n",result->AAAState );
        }
    }

#if DEBUG_LOG_TO_FILE
    /* expected result , NAI string should not equal to NULL  */
    if(result!=NULL)
        local_fprintf("%s\n", (('\0' != result->NAI[0] )? "Correct": "Wrong"));
#endif 

}

void dump_RMSetTransferStatistics(void* ptr)
{
    unpack_wds_RMSetTransferStatistics_t *result =
        (unpack_wds_RMSetTransferStatistics_t*) ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask (result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_SLQSWdsSwiPDPRuntimeSettings(void* ptr)
{
    unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t *result =
        (unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t*) ptr;
    if(result)
    {
        int idx = 0;
        swi_uint256_print_mask (result->ParamPresenceMask);
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        {
            printf("context Id      : %x \n",result->contextId);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        {
            printf("bearer Id       : %x \n",result->bearerId);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
        {
            printf("apn name        : %s \n",result->apnName);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
        {
            printf("IPv4 Address    : %x \n",result->ipv4Address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
        {
            printf("IPv4 GW Address : %x \n",result->ipv4GWAddress);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
        {
            printf("primary DNS IPv4 address   : %x \n",result->prDNSIPv4Address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
        {
            printf("secondary DNS IPv4 address : %x \n",result->seDNSIPv4Address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
        {
            printf("Primary DNS V6   : ");
            for (idx = 0; idx < 8; idx++)
                printf("%hx ", result->prDNSIPv6Address[idx] );
            printf("\n");
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
        {
            printf("Secondary DNS V6 : ");
            for (idx = 0; idx < 8; idx++)
                printf("%hx ", result->seDNSIPv6Address[idx] );
            printf("\n");
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
        {
            printf("IPAddressV6 Information\n");
            printf("\tIPAddressV6 Address   :");
            for (idx = 0; idx < 8; idx++)
                printf("%hx ",
                             result->ipv6Address.IPAddressV6[idx] );
            printf( "\n");
            printf("\tIPAddressV6 Length    : %d\n",
                    result->ipv6Address.IPV6PrefixLen );
        }

        if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
        {
            printf("IPV6 Gateway Address Information\n");
            printf("\tIPV6 Gateway Address   :");
            for (idx = 0; idx < 8; idx++)
                printf("%hx ",
                            result->ipv6GWAddress.IPAddressV6[idx] );
            printf( "\n");
            printf("\tIPV6 Gateway Address Length    : %d\n",
                        result->ipv6GWAddress.IPV6PrefixLen );
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 27))
        {
            printf("Primary PCSCF IPv4 Address (%d)  : ",result->prPCSCFIPv4Address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 29))
        {
            printf("Primary PCSCF IPv6 Address   : ");
            if(result->prPCSCFIPv6Address)
            {
                for ( idx = 0 ; idx  < IPV6_ADDRESS_ARRAY_SIZE ; idx++ )
                {
                    printf( "%x ",result->prPCSCFIPv6Address[idx]);
                }
            }
            printf("\n");
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 28))
        {
            printf("Secondary PCSCF IPv4 Address (%d): ",result->sePCSCFIPv4Address);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 30))
        {
            printf("Secondary PCSCF IPv6 Address : ");
            if(result->sePCSCFIPv6Address)
            {
                for ( idx = 0 ; idx < IPV6_ADDRESS_ARRAY_SIZE ; idx++ )
                {
                    printf("%x ",result->sePCSCFIPv6Address[idx]);
                }
            }
            printf("\n");
        }
        
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result */
    if(result!=NULL)
    {
        if ((result->bearerId == 0) ||
            (result->apnName == NULL) ||
            (result->ipv4Address == 0) ||
            (result->ipv4GWAddress == 0 ))
        {
            is_matching = 0;
        }
    }
    /* NAI string should not equal to NULL  */
    local_fprintf("%s\n", (( is_matching == 1 )? "Correct": "Wrong"));
#endif

}


void dump_SLQSGetCurrDataSystemStat(void* ptr)
{
    unpack_wds_SLQSGetCurrDataSystemStat_t *pDataStat =
        (unpack_wds_SLQSGetCurrDataSystemStat_t*) ptr;
    uint8_t lCtr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("current data system status\n" );
    swi_uint256_print_mask (pDataStat->ParamPresenceMask);
    if(swi_uint256_get_bit (pDataStat->ParamPresenceMask, 16))
    {
        printf("preferred network      : 0x%x\n", pDataStat->prefNetwork );
        printf("network info length    : 0x%x\n", pDataStat->networkInfoLen );

        printf("network information data\n\n" );
        for( lCtr = 0; lCtr < pDataStat->networkInfoLen; lCtr++ )
        {
            printf("\nnetwork info set: 0x%x\n", lCtr );
            printf("network          : 0x%x\n",  pDataStat->currNetworkInfo[lCtr].NetworkType );
            printf("RAT mask         : 0x%x\n", pDataStat->currNetworkInfo[lCtr].RATMask );
            printf("SO mask          : 0x%x\n", pDataStat->currNetworkInfo[lCtr].SOMask );
        }
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result, RAT Mask should not equal to 0 */
    if ( (pDataStat->prefNetwork  > 1)  || 
       (pDataStat->currNetworkInfo[0].NetworkType > 1) ||
       (pDataStat->currNetworkInfo[0].RATMask == 0))
    {
        is_matching = 0;
    }
    /* mobile ip mode should not larger than 2 */
    local_fprintf("%s\n", ((1== is_matching)? "Correct": "Wrong"));
#endif 
}

void dump_wds_GetPacketStatus( void *ptr)
{
    unpack_wds_GetPacketStatus_t *pStatus =
        (unpack_wds_GetPacketStatus_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (pStatus->ParamPresenceMask);
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 16))
    {
        printf("\ttx packet successes %d\n", pStatus->tXPacketSuccesses);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 17))
    {
        printf("\trx packet successes %d\n", pStatus->rXPacketSuccesses);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 18))
    {
        printf("\ttx packet error %d\n", pStatus->tXPacketErrors);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 19))
    {
        printf("\trx packet error %d\n", pStatus->rXPacketErrors);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 20))
    {
        printf("\ttx packet overflows %d\n", pStatus->tXPacketOverflows);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 21))
    {
        printf("\trx packet overflows %d\n", pStatus->rXPacketOverflows);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 25))
    {
        printf("\ttx ok bytes count %"PRIu64"\n", pStatus->tXOkBytesCount);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 26))
    {
        printf("\trx ok bytes count %"PRIu64"\n", pStatus->rXOkBytesCount);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 27))
    {
        printf("\ttx ok bytes last call %"PRIu64"\n", pStatus->tXOKBytesLastCall);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 28))
    {
        printf("\trx ok bytes last call %"PRIu64"\n", pStatus->rXOKBytesLastCall);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 29))
    {
        printf("\ttx dropped count %d\n", pStatus->tXDroppedCount);
    }
    if(swi_uint256_get_bit (pStatus->ParamPresenceMask, 30))
    {
        printf("\trx dropped count %d\n", pStatus->rXDroppedCount);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result */
    if (pStatus->tXPacketSuccesses == 0)
    {
        is_matching = 0;
    }
    CHECK_WHITELIST_MASK(
        unpack_wds_GetPacketStatusParamPresenceMaskWhiteList,
        pStatus->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_GetPacketStatusParamPresenceMaskMandatoryList,
        pStatus->ParamPresenceMask);

    /* packet success counter and bytes ok count should not equal to 0 */
    local_fprintf("%s\n", (( is_matching == 1 )? "Correct": "Wrong"));
#endif
}

void dump_wds_qmap_SLQSGetRuntimeSettings(void* ptr)
{
    unpack_wds_SLQSGetRuntimeSettings_t *runtime =
        (unpack_wds_SLQSGetRuntimeSettings_t*) ptr;
    
    if( CHECK_PTR_IS_NOT_NULL(runtime) &&
        (PrintIPv4RuntimeSettings(runtime)==1))
    {
        return ;
    }

    if(wds>=0)
    close(wds);
    wds=-1;
    exit(1);
}



void dump_wds_SLQSGetRuntimeSettings(void* ptr)
{
    unpack_wds_SLQSGetRuntimeSettings_t *runtime =
        (unpack_wds_SLQSGetRuntimeSettings_t*) ptr;

    struct in_addr ip_addr;
    struct in_addr gw_addr;
    struct in_addr subnet;
    int idx=0;
    int8_t count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (runtime->ParamPresenceMask);
    ip_addr.s_addr = htonl(runtime->IPv4);
    gw_addr.s_addr = htonl(runtime->GWAddressV4);
    subnet.s_addr = htonl(runtime->SubnetMaskV4);
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 30))
    {
        printf("\tipv4 %s\n", inet_ntoa(ip_addr));
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 32))
    {
        printf("\tgateway addr %s\n", inet_ntoa(gw_addr));
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 33))
    {
        printf("\tsubnet mask %s\n", inet_ntoa(subnet));
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 16))
    {
        printf("\tprofile name %s\n",runtime->ProfileName);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 17))
    {
        printf("\tpdp type %d\n",runtime->PDPType);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 21))
    {
        printf("\tPrimaryDNSV4   : %x\n",runtime->PrimaryDNSV4);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 22))
    {
        printf("\tSecondaryDNSV4 : %x\n",runtime->SecondaryDNSV4);
    }
    
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 23))
    {
        printf("UMTS Granted QoS Parameters\n");
        printf("\ttrafficClass       : %d\n", runtime->UMTSGrantedQoS.trafficClass);
        printf("\tmaxUplinkBitrate   : %x\n",runtime->UMTSGrantedQoS.maxUplinkBitrate);
        printf("\tmaxDownlinkBitrate : %x\n",runtime->UMTSGrantedQoS.maxDownlinkBitrate);
        printf("\tgrntUplinkBitrate  : %x\n",runtime->UMTSGrantedQoS.grntUplinkBitrate);
        printf("\tgrntDownlinkBitrate: %x\n",runtime->UMTSGrantedQoS.grntDownlinkBitrate);
        printf("\tqosDeliveryOrder   : %d\n",runtime->UMTSGrantedQoS.qosDeliveryOrder);
        printf("\tmaxSDUSize         : %x\n",runtime->UMTSGrantedQoS.maxSDUSize);
        printf("\tsduErrorRatio      : %d\n",runtime->UMTSGrantedQoS.sduErrorRatio);
        printf("\tresBerRatio        : %d\n",runtime->UMTSGrantedQoS.resBerRatio);
        printf("\tdeliveryErrSDU     : %d\n",runtime->UMTSGrantedQoS.deliveryErrSDU);
        printf("\ttransferDelay      : %x\n",runtime->UMTSGrantedQoS.transferDelay);
        printf("\ttrafficPriority    : %x\n",runtime->UMTSGrantedQoS.trafficPriority);
    }
    
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 25))
    {
        printf("GPRS Granted QoS Parameters\n");
        printf("\tprecedenceClass     : %x\n",runtime->GPRSGrantedQoS.precedenceClass);
        printf("\tdelayClass          : %x\n",runtime->GPRSGrantedQoS.delayClass);
        printf("\treliabilityClass    : %x\n",runtime->GPRSGrantedQoS.reliabilityClass);
        printf("\tpeakThroughputClass : %x\n",runtime->GPRSGrantedQoS.peakThroughputClass);
        printf("\tmeanThroughputClass : %x\n",runtime->GPRSGrantedQoS.meanThroughputClass);
    }
    
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 27))
    {
        printf("Username       : %s\n",runtime->Username);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 29))
    {
        printf("Authentication : %x\n",runtime->Authentication);
    }

    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 20))
    {
        printf("\tapn name %s\n",runtime->APNName);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 17))
    {
        printf("\tProfile ID:\n\t\tIndex:%d, ProfileType:%d\n",runtime->ProfileID.profileIndex,runtime->ProfileID.profileType);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 43))
    {
        printf("\tIP Family Preference : %d\n",runtime->IPFamilyPreference);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 44))
    {
        printf("\tIMCNflag       : %d\n",runtime->IMCNflag);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 45))
    {
        printf("\tTechnology     : %d\n",runtime->Technology);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 41))
    {
        printf("\tMTU : %d\n",runtime->Mtu);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 34))
    {
        printf("PCSCFAddrPCO   : %d\n",runtime->PCSCFAddrPCO);
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 35))
    {
        printf("IPV4 Server AddressList \n");
        printf("\tPCSCF Server AddressList Count %d\n",runtime->ServerAddrList.numInstances);
        for ( idx = 0; idx < runtime->ServerAddrList.numInstances; idx++ )
            printf("\tPCSCF Server Address[%d] %x\n",
                        idx,runtime->ServerAddrList.pscsfIPv4Addr[idx] );
    }
    
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 36))
    {
        printf("PCSCFFQDNAddressList \n");
        printf("\tPCSCFFQDNAddressList Count %d\n",runtime->PCSCFFQDNAddrList.numInstances);
        for ( idx = 0;
              idx < runtime->PCSCFFQDNAddrList.numInstances ;
              idx++ )
        {
            printf("\tPCSCFFQDNAddressLength[%d]  %d\n",
                        idx,runtime->PCSCFFQDNAddrList.pcsfFQDNAddress[idx].fqdnLen );
            printf("\tPCSCFFQDNAddress[%d]        %s\n",
                        idx,runtime->PCSCFFQDNAddrList.pcsfFQDNAddress[idx].fqdnAddr );
        }
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 42))
    {
        printf("DomainNameList \n");
        printf("\tDomainNameList Count %d\n",runtime->DomainList.numInstances);

        for ( idx = 0; idx < runtime->DomainList.numInstances; idx++ )
        {
            printf("\tDomainLen[%d]   %d\n",
                        idx, runtime->DomainList.domain[idx].domainLen);
            printf("\tDomainName[%d]  %s\n",
                        idx,runtime->DomainList.domain[idx].domainName);
        }
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 39))
    {
        printf("\tPrimary DNS V6   : ");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ", runtime->PrimaryDNSV6[idx] );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 40))
    {
        printf("\tSecondary DNS V6 : ");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ", runtime->SecondaryDNSV6[idx] );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 37))
    {
        printf("\tIPAddressV6 Information\n");
        printf("\t\tIPAddressV6 Address   :");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ",
                         runtime->IPV6AddrInfo.IPAddressV6[idx] );
        printf( "\n");
        printf("\t\tIPAddressV6 Length    : %d\n",
                    runtime->IPV6AddrInfo.IPV6PrefixLen );
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 38))
    {
        printf("\tIPV6 Gateway Address Information\n");
        printf("\t\tIPV6 Gateway Address  :");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ",
                        runtime->IPV6GWAddrInfo.gwAddressV6[idx] );
        printf( "\n");
        printf("\t\tIPV6 Gateway Address Length    : %d\n",
                    runtime->IPV6GWAddrInfo.gwV6PrefixLen );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 37))
    {
        printf("\tIPAddressV6 Address    ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                 runtime->IPV6AddrInfo.IPAddressV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 38))
    {
        printf("\tIPV6 Gateway Address   ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime->IPV6GWAddrInfo.gwAddressV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 39))
    {
        printf("\tIPV6 Primary DNS       ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime->PrimaryDNSV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime->ParamPresenceMask, 40))
    {
        printf("\tIPV6 Secondary DNS     ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime->SecondaryDNSV6[count] );
        printf("\b \b\n");
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result, ipv4 address should not equal to 0 */
    if (!swi_uint256_get_bit(runtime->ParamPresenceMask, 30) ||
        !swi_uint256_get_bit(runtime->ParamPresenceMask, 32) ||
        !swi_uint256_get_bit(runtime->ParamPresenceMask, 20))
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", (( is_matching == 1 )? "Correct": "Wrong"));
#endif

}

uint16_t u8toU16(uint8_t u8Array[2])
{
    return (u8Array[0] + u8Array[1]*256);
}

int PrintIPv6RuntimeSettings(unpack_wds_SLQSGetRuntimeSettings_t *runtime)
{

    int idx=0;
    int retval=0;
    int8_t count;
    QmuxIPv6Table ipTable;
    if(runtime==NULL)
    {
        printf("runtime NULL\n");
        return 0;
    }
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (runtime->ParamPresenceMask);
    if(!swi_uint256_get_bit(runtime->ParamPresenceMask, 37) ||
       !swi_uint256_get_bit(runtime->ParamPresenceMask, 38) ||
       !swi_uint256_get_bit(runtime->ParamPresenceMask, 39) 
        )
    {
        printf("runtime No IPv6 Info\n");
        return 0;
    }
    memset(&ipTable,0,sizeof(QmuxIPv6Table));
    printf("\tProfile ID:\n\t\tIndex:%d, ProfileType:%d\n",runtime->ProfileID.profileIndex,runtime->ProfileID.profileType);
    printf("\tIP Family Preference : %d\n",runtime->IPFamilyPreference);
    printf("\tMTU : %d\n",runtime->Mtu);
    
    printf("Primary DNS V6   : ");
    for (idx = 0; idx < 8; idx++)
        printf("%hx ", runtime->PrimaryDNSV6[idx] );
    printf("\n");
    printf("Secondary DNS V6 : ");
    for (idx = 0; idx < 8; idx++)
        printf("%hx ", runtime->SecondaryDNSV6[idx] );
    printf("\n");
    printf("IPAddressV6 Information\n");
    printf("IPAddressV6 Address   :");
    for (idx = 0; idx < 8; idx++)
        printf("%hx ",
                     runtime->IPV6AddrInfo.IPAddressV6[idx] );
    printf( "\n");
    printf("IPAddressV6 Length    : %d\n",
                runtime->IPV6AddrInfo.IPV6PrefixLen );
    printf("IPV6 Gateway Address Information\n");
    printf("IPV6 Gateway Address  :");
    for (idx = 0; idx < 8; idx++)
        printf("%hx ",
                    runtime->IPV6GWAddrInfo.gwAddressV6[idx] );
    printf( "\n");
    printf("IPV6 Gateway Address Length    : %d\n",
                runtime->IPV6GWAddrInfo.gwV6PrefixLen );
    printf("\n");
    printf("IPAddressV6 Address    ");
    for (count = 0; count < 8; count++)
        printf("%hx:",
             runtime->IPV6AddrInfo.IPAddressV6[count] );
    printf("\b \b\n");

    printf("IPV6 Gateway Address   ");
    for (count = 0; count < 8; count++)
        printf("%hx:",
                runtime->IPV6GWAddrInfo.gwAddressV6[count] );
    printf("\b \b\n");

    printf("IPV6 Primary DNS       ");
    for (count = 0; count < 8; count++)
        printf("%hx:",
                runtime->PrimaryDNSV6[count] );
    printf("\b \b\n");

    printf("IPV6 Secondary DNS     ");
    for (count = 0; count < 8; count++)
        printf("%hx:",
                runtime->SecondaryDNSV6[count] );
    printf("\b \b\n");

    ipfamily = IPV6;
    ipTable.muxID = uMuxID; 
    ipTable.prefix_len = runtime->IPV6AddrInfo.IPV6PrefixLen;
    for(idx=0;idx<8;idx++)
    {
        uint8_t tipv6addr[2]={0};
        memcpy(&tipv6addr,&runtime->IPV6AddrInfo.IPAddressV6[idx],sizeof(uint16_t));
        if(u8toU16(tipv6addr)==runtime->IPV6AddrInfo.IPAddressV6[idx])
        {
            ipTable.ipv6addr[idx*2] = tipv6addr[1];
            ipTable.ipv6addr[idx*2+1] = tipv6addr[0];
        }
        else
        {
            ipTable.ipv6addr[idx*2] = tipv6addr[0];
            ipTable.ipv6addr[idx*2+1] = tipv6addr[1];
        }
    }

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 1;

        /* expected result, ipv6 address should not equal to 0 */
        if (runtime->IPV6AddrInfo.IPV6PrefixLen == 0)
        {
            is_matching = 0;
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


    printf("Mux ID : 0x%02x\n", uMuxID);
    printf("IPv6 Address :\n");
    printf("ifconfig %s inet6 add ",szEthName);
    for(idx=0;idx<8;idx++)
    {
        printf("%02x%02x",ipTable.ipv6addr[idx*2],ipTable.ipv6addr[(idx*2)+1]);
        sprintf(&szIPAddress[idx*4+idx],"%02x%02x", ipTable.ipv6addr[idx*2],ipTable.ipv6addr[(idx*2)+1]);
        if(idx!=7)
        {
            printf(":");
            sprintf(&szIPAddress[(idx*4)+4+idx],":");
        }
    }
    
    printf("/%d up\n",ipTable.prefix_len);
    //ifconfig eth0 inet6 up
    retval = ioctl(wds, QMI_SET_QMAP_IPv6_TABLE, (void *)&ipTable);
    if(retval < 0)
    {
        printf("QMI_SET_QMAP_IP_TABLE error!\n");
        return 0;
    }
    retval = 0;
    for(idx=0;idx<8;idx++)
    {
        if( (ipTable.ipv6addr[idx*2] !=0)||
            (ipTable.ipv6addr[idx*2+1]!=0) )
        {    
            retval = 1;
            break;
        }
    }
    if(retval==0)
    {
        fprintf(stderr,"Fail to get IPv6\n");
        return 0;
    }

    /* command to set mtu size */
    printf("ifconfig %s mtu %d\n", szEthName, runtime->Mtu);
    if (g_mtu_auto_update_enable && (runtime->Mtu != 0xFFFFFFFF) && (runtime->Mtu >= 68)) // minimum mtu size is 68
    {
        setAdaptorMtu(szEthName, runtime->Mtu);
    }

    
    if ( g_ip_auto_assign )
    {
        int timeout=0;
        iConnected = 1;
        {
            SetIPAddress(IPV6);
            /* after setting the ip address, network adapter need sometime to become ready */
            do
            {
                sleep(1);
                timeout++;
                if(timeout>60)
                    break;
            }
            while(iIsAdaptorUp(szEthName)==0);
        } 
        if(strlen(ping6Table[0])>0 && g_auto_test_enable)
           ping(IPV6, ping6Table[0],szGWAddress,szEthName);
    }

    return 1;
}

void dump_wds_SLQSGetRuntimeSettingsV6(void* ptr)
{
    unpack_wds_SLQSGetRuntimeSettings_t *runtime =
        (unpack_wds_SLQSGetRuntimeSettings_t*) ptr;
    if( CHECK_PTR_IS_NOT_NULL(runtime) &&
        (PrintIPv6RuntimeSettings(runtime)==1))
    {
        return ;
    }
    if(wds>=0)
    close(wds);
    wds=-1;
    exit(1);
}

uint8_t  PName[] = "Sierra";
uint8_t  APNName[] = "internet.com";
uint8_t  Username[] = "wapuser";
uint8_t  Password[] = "";

pack_wds_SetDefaultProfile_t tpack_wds_SetDefaultProfile = {0, 0, 0, 0, 0, 1,
          PName, APNName, Username, Password };

void dump_wds_GetDefaultProfile( void *ptr)
{
    unpack_wds_GetDefaultProfile_t *defprofile =
        (unpack_wds_GetDefaultProfile_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (defprofile->ParamPresenceMask);
    printf("received profile details are as follows:");
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 17))
    {
        printf("\npdpype :%x",defprofile->pdptype);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 30))
    {
        printf("\nipaddress :%x",defprofile->ipaddr);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 21))
    {
        printf("\nprimarydns :%x",defprofile->pridns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 22))
    {
        printf("\nsecondarydns :%x",defprofile->secdns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 29))
    {
        printf("\nauthentication :%x",defprofile->auth);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 16))
    {
        printf("\nname : %s",defprofile->name);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 20))
    {
        printf("\napnname :%s",defprofile->apnname);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 27))
    {
        printf("\nusername :%s\n",defprofile->username);
    }
#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result, ipv4 address should not equal to 0 */
    if ((defprofile->name == NULL) ||
        (defprofile->apnname == NULL) )
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", (( is_matching == 1 )? "Correct": "Wrong"));
#endif
    /*Restore to get the default profile*/
    tpack_wds_SetDefaultProfile.pdpType      = defprofile->pdptype;
    tpack_wds_SetDefaultProfile.ipAddress    = defprofile->ipaddr;
    tpack_wds_SetDefaultProfile.primaryDNS   = defprofile->pridns;
    tpack_wds_SetDefaultProfile.secondaryDNS = defprofile->secdns;
    tpack_wds_SetDefaultProfile.authentication = defprofile->auth;
    if(tpack_wds_SetDefaultProfile.pName)
    memcpy(tpack_wds_SetDefaultProfile.pName,&defprofile->name,255);
    if(tpack_wds_SetDefaultProfile.pUsername)
    memcpy(tpack_wds_SetDefaultProfile.pUsername,&defprofile->username,255);
    if(tpack_wds_SetDefaultProfile.pApnname)
    memcpy(tpack_wds_SetDefaultProfile.pApnname,&defprofile->apnname,255);
}

void dump_wds_GetDefaultProfileV2( void *ptr)
{
    unpack_wds_GetDefaultProfileV2_t *defprofile =
        (unpack_wds_GetDefaultProfileV2_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (defprofile->ParamPresenceMask);
    printf("received profile details are as follows:");

    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 17))
    {
        printf("\npdpype :%x",defprofile->pdptype);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 30))
    {
        printf("\nipaddress :%x",defprofile->ipaddr);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 21))
    {
        printf("\nprimarydns :%x",defprofile->pridns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 22))
    {
        printf("\nsecondarydns :%x",defprofile->secdns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 29))
    {
        printf("\nauthentication :%x",defprofile->auth);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 16))
    {
        printf("\nname : %s",defprofile->name);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 20))
    {
        printf("\napnname :%s",defprofile->apnname);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 27))
    {
        printf("\nusername :%s",defprofile->username);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 28))
    {
        printf("\npassword :%s\n",defprofile->pwd);
    }

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result, profile/apn name is not null */
    if ((defprofile->name == NULL) ||
        (defprofile->apnname == NULL) )
    {
        is_matching = 0;
    }
    local_fprintf("%s\n", (( is_matching == 1 )? "Correct": "Wrong"));
#endif
}

void dump_wds_SLQSGetDataBearerTechnology ( void *ptr)
{
    unpack_wds_SLQSGetDataBearerTechnology_t *dbTech =
    (unpack_wds_SLQSGetDataBearerTechnology_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (dbTech->ParamPresenceMask, 1))
    {
        printf("DataBearerMask: 0x%x\n",
            dbTech->dataBearerMask );
        if (dbTech->dataBearerMask & QMI_WDS_CURRENT_CALL_DB_MASK)
        {
            printf("Current data bearer details\n" );
            printf("Network: %d\n",
                 dbTech->curDataBearerTechnology.currentNetwork );
            printf("Rat Mask: %d\n", dbTech->curDataBearerTechnology.ratMask);
            printf("So Mask:  %d\n", dbTech->curDataBearerTechnology.soMask );
        }
        if (dbTech->dataBearerMask & QMI_WDS_LAST_CALL_DB_MASK)
        {
            if(swi_uint256_get_bit (dbTech->ParamPresenceMask, 16))
            {
                printf("Last Call data bearer details\n" );
                printf("Network:  %d\n",
                        dbTech->lastCallDataBearerTechnology.currentNetwork );
                printf("Rat Mask: %d\n", dbTech->lastCallDataBearerTechnology.ratMask);
                printf("So Mask:  %d\n", dbTech->lastCallDataBearerTechnology.soMask );
            }            
        }
    }
    

#if DEBUG_LOG_TO_FILE
    uint8_t is_matching = 1;

    /* expected result, current RAT_MASK should be presented */
    if ((dbTech->curDataBearerTechnology.currentNetwork == 0) ||
        (dbTech->curDataBearerTechnology.ratMask == 0))
    {
        is_matching = 0;
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

void dump_wds_SLQSDeleteProfile ( void *ptr)
{
    unpack_wds_SLQSDeleteProfile_t *result =
    (unpack_wds_SLQSDeleteProfile_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 224))
    {
        printf("extendedErrorCode:%d\n",result->extendedErrorCode);
    }
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSDeleteProfileParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSDeleteProfileParamPresenceMaskMandatoryList,
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
}

void dump_wds_SLQSSetIPFamilyPreference ( void *ptr)
{
    unpack_wds_SLQSSetIPFamilyPreference_t *result =
        (unpack_wds_SLQSSetIPFamilyPreference_t*)ptr;
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSSetIPFamilyPreferenceParamPresenceMaskWhiteList,
        result->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSSetIPFamilyPreferenceParamPresenceMaskMandatoryList,
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
}

void dump_wds_SLQSSGetDHCPv4ClientConfig ( void *ptr)
{
    uint8_t i, j;
    unpack_wds_SLQSSGetDHCPv4ClientConfig_t *pConfig =
    (unpack_wds_SLQSSGetDHCPv4ClientConfig_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(pConfig->ParamPresenceMask);
    if ( ( pConfig->pHwConfig != NULL) &&
        swi_uint256_get_bit (pConfig->ParamPresenceMask, 16))
    {
        printf( "HW Config:\n");
        printf( "DHCP HW Type:%d\n", pConfig->pHwConfig->hwType);
        printf( "Length of chaddr field:%d\n", pConfig->pHwConfig->chaddrLen);
        printf( "Client HW address: ");
        for ( i = 0; i < pConfig->pHwConfig->chaddrLen; i++)
        {
            printf( "0x%02x ", pConfig->pHwConfig->chaddr[i]);
        }
        printf( "\n");
    }
    if ((pConfig->pRequestOptionList != NULL) && 
            (pConfig->pRequestOptionList->numOpt > 0) &&
            swi_uint256_get_bit (pConfig->ParamPresenceMask, 17) )
    {
        printf( "Additional Request Options:\n");
        printf( "number of sets of opts: %d\n", pConfig->pRequestOptionList->numOpt );
        for (i = 0; i < pConfig->pRequestOptionList->numOpt; i++)
        {
            if ( pConfig->pRequestOptionList->pOptList != NULL)
            {
                printf( "Option code: %d\n", pConfig->pRequestOptionList->pOptList->optCode);
                printf( "Length of Option value: %d\n", pConfig->pRequestOptionList->pOptList->optValLen);
                for ( j = 0; j < pConfig->pRequestOptionList->pOptList->optValLen; j++)
                {
                    printf( "0x%02x ", pConfig->pRequestOptionList->pOptList->optVal[j]);
                }
                printf( "\n");
                (pConfig->pRequestOptionList->pOptList)++;
            }
        }
    }    

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_GetPacketStatistics ( void *ptr)
{
    unpack_wds_GetPacketStatistics_t *pPktStat =
    (unpack_wds_GetPacketStatistics_t*) ptr;
    printf("%s\n",__FUNCTION__);
    if(pPktStat)
    {
        swi_uint256_print_mask(pPktStat->ParamPresenceMask);
        if( (pPktStat->pTXPacketSuccesses!=NULL) &&
            swi_uint256_get_bit(pPktStat->ParamPresenceMask, 16) )
        {
            printf("TX Packet Successes 0X%x\n",*pPktStat->pTXPacketSuccesses);
        }
        if( (pPktStat->pRXPacketSuccesses!=NULL) &&
            swi_uint256_get_bit(pPktStat->ParamPresenceMask, 17))
        {
            printf("RX Packet Successes 0X%x\n",*pPktStat->pRXPacketSuccesses);
        }
        if( (pPktStat->pTXPacketErrors!=NULL) &&
            swi_uint256_get_bit(pPktStat->ParamPresenceMask, 18))
        {
            printf("TX Packet Errors 0X%x\n",*pPktStat->pTXPacketErrors);
        }
        if( (pPktStat->pRXPacketErrors!=NULL) &&
            swi_uint256_get_bit(pPktStat->ParamPresenceMask, 19))
        {
            printf("RX Packet Errors 0X%x\n",*pPktStat->pRXPacketErrors);
        }
        if( (pPktStat->pTXPacketOverflows!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 20))
        {
            printf("TX Packet Overflows 0X%x\n",*pPktStat->pTXPacketOverflows);
        }
        if( (pPktStat->pRXPacketOverflows!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 21))
        {
            printf("RX Packet Overflows 0X%x\n",*pPktStat->pRXPacketOverflows);
        }
        if( (pPktStat->pTXOkBytesCount!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 25) )
        {
            printf("TX Ok Bytes Count 0X%"PRIX64"\n", *pPktStat->pTXOkBytesCount);
        }
        if( (pPktStat->pRXOkBytesCount!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 26) )
        {
            printf("RX Ok Bytes Count 0X%"PRIX64"\n", *pPktStat->pRXOkBytesCount);
        }
        if( (pPktStat->pTXOKBytesLastCall!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 27) )
        {
            printf("TX OK Bytes Last Call 0X%"PRIX64"\n", *pPktStat->pTXOKBytesLastCall);
        }
        if( (pPktStat->pRXOKBytesLastCall!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 28) )
        {
            printf("RX OK Bytes Last Call 0X%"PRIX64"\n", *pPktStat->pRXOKBytesLastCall);
        }
        if( (pPktStat->pTXDroppedCount!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 29) )
        {
            printf("TX Dropped Count 0X%x\n",*pPktStat->pTXDroppedCount);
        }
        if( (pPktStat->pRXDroppedCount!=NULL) &&
            swi_uint256_get_bit (pPktStat->ParamPresenceMask, 30) )
        {
            printf("RX Dropped Count 0X%x\n",*pPktStat->pRXDroppedCount);
        }
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}
void dump_wds_GetByteTotals ( void *ptr)
{
    unpack_wds_GetByteTotals_t *pBytesTotal =
    (unpack_wds_GetByteTotals_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s\n",__FUNCTION__);
    if( (pBytesTotal->pTXTotalBytes) &&
        swi_uint256_get_bit (pBytesTotal->ParamPresenceMask, 16))
    {
        printf("TX Total Bytes Count 0X%"PRIX64"\n", *pBytesTotal->pTXTotalBytes);
    }
    if( (pBytesTotal->pRXTotalBytes) &&
        swi_uint256_get_bit (pBytesTotal->ParamPresenceMask, 17))
    {
        printf("RX Total Bytes Count 0X%"PRIX64"\n", *pBytesTotal->pRXTotalBytes);
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SLQSGetCurrentChannelRate ( void *ptr)
{
    unpack_wds_SLQSGetCurrentChannelRate_t *pCurrChannelRate =
    (unpack_wds_SLQSGetCurrentChannelRate_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(pCurrChannelRate->ParamPresenceMask);
    if(swi_uint256_get_bit (pCurrChannelRate->ParamPresenceMask, 1))
    {
        printf( "Channel Rx Rate:%u\n", pCurrChannelRate->current_channel_rx_rate);
        printf( "Channel Tx Rate:%u\n", pCurrChannelRate->current_channel_tx_rate);
        printf( "Max Channel Rx Rate:%u\n", pCurrChannelRate->max_channel_rx_rate);
        printf( "Max Channel Tx Rate:%u\n", pCurrChannelRate->max_channel_tx_rate);
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SLQSSSetLoopback(void* ptr)
{
    unpack_wds_SLQSSSetLoopback_t *result = 
        (unpack_wds_SLQSSSetLoopback_t*)ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
        return ;
    }
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SLQSSGetLoopback ( void *ptr)
{
    unpack_wds_SLQSSGetLoopback_t *pLoopBack =
    (unpack_wds_SLQSSGetLoopback_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(pLoopBack->ParamPresenceMask);
    if(swi_uint256_get_bit (pLoopBack->ParamPresenceMask, 16))
    {
        printf("Loopback Mode:0X%02X\n",pLoopBack->ByteLoopbackMode);
    }
    if(swi_uint256_get_bit (pLoopBack->ParamPresenceMask, 17))
    {
        printf("Loopback Multiplier:0X%02X\n",pLoopBack->ByteLoopbackMultiplier);
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_DHCPv4ClientLeaseChange ( void *ptr)
{
    unpack_wds_DHCPv4ClientLeaseChange_t *result =
        (unpack_wds_DHCPv4ClientLeaseChange_t*)ptr;
    UNUSEDPARAM(ptr);
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SetMobileIP(void* ptr)
{
    unpack_wds_SetMobileIP_t *result = 
        (unpack_wds_SetMobileIP_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SetAutoconnect(void* ptr)
{
    unpack_wds_SetAutoconnect_t *result =
        (unpack_wds_SetAutoconnect_t*) ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SetMobileIPParameters(void* ptr)
{
    unpack_wds_SetMobileIPParameters_t *result =
        (unpack_wds_SetMobileIPParameters_t*)ptr;
    if(ptr==NULL)
    {
        printf("%s NULL Data\n",__FUNCTION__);
    }
    printf("%s Data\n",__FUNCTION__);
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(result->ParamPresenceMask);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_GetAutoconnect(void* ptr)
{
    /* expected result */
    unpack_wds_GetAutoconnect_t *ps =
        (unpack_wds_GetAutoconnect_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(ps->ParamPresenceMask);
    if( (ps->psetting) &&
        swi_uint256_get_bit(ps->ParamPresenceMask, 1) )
    {
        printf("Setting: %d\n", *(ps->psetting));
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SLQSWdsSetEventReport(void* ptr)
{
    unpack_wds_SLQSWdsSetEventReport_t *result = 
        (unpack_wds_SLQSWdsSetEventReport_t*) ptr;
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

void dump_wds_SLQSWdsGoDormant(void* ptr)
{
    unpack_wds_SLQSWdsGoDormant_t *result =
        (unpack_wds_SLQSWdsGoDormant_t*)ptr;
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

void dump_wds_SLQSWdsGoActive(void* ptr)
{
    unpack_wds_SLQSWdsGoActive_t *result =
        (unpack_wds_SLQSWdsGoActive_t*)ptr;
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

void dump_wds_SLQSResetPacketStatics(void* ptr)
{
    unpack_wds_SLQSResetPacketStatics_t *result =
        (unpack_wds_SLQSResetPacketStatics_t*)ptr;
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

void dump_wds_SLQSSSetDHCPv4ClientConfig(void* ptr)
{
    unpack_wds_SLQSSSetDHCPv4ClientConfig_t *result =
        (unpack_wds_SLQSSSetDHCPv4ClientConfig_t*)ptr;
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

void dump_wds_GetDataBearerTechnology ( void *ptr)
{
    unpack_wds_GetDataBearerTechnology_t *pBearer =
    (unpack_wds_GetDataBearerTechnology_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf( "%s\n",__FUNCTION__ );
    if( (pBearer->pDataBearer) && 
        swi_uint256_get_bit(pBearer->ParamPresenceMask, 1) )
    {
        printf("Databearer %u\n", *(pBearer->pDataBearer) );
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SetDefaultProfile(void* ptr)
{
    unpack_wds_SetDefaultProfile_t *result = 
        (unpack_wds_SetDefaultProfile_t*) ptr;
    UNUSEDPARAM(ptr);
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

void dump_wds_SLQSGetDUNCallInfo( void *ptr)
{
    unpack_wds_SLQSGetDUNCallInfo_t *result =
    (unpack_wds_SLQSGetDUNCallInfo_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf( "DUN Call Info\n" );
    swi_uint256_print_mask (result->ParamPresenceMask);
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf( "Modem Connection Status   : 0x%x\n", result->connectionStatus.MDMConnStatus );
        printf( "Modem Call Duration       : 0x%"PRIX64"\n", result->connectionStatus.MDMCallDuration );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("Call End Reason            : 0x%x\n", result->callEndReason );        
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("Tx Bytes OK                : 0x%"PRIX64"\n", result->txOKBytesCount );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        printf("Rx Bytes OK                : 0x%"PRIX64"\n", result->rxOKBytesCount );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        printf("Dormancy Status            : 0x%x\n", result->dormancyStatus );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 21))
    {
        printf("Data Bearer Technology     : 0x%x\n", result->dataBearerTech );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 22))
    {
        printf( "Current channel Tx Rate   : 0x%x\n", result->channelRate.CurrChanTxRate );
        printf("Current channel Rx Rate    : 0x%x\n", result->channelRate.CurrChanRxRate );
        printf("Max channel Tx Rate        : 0x%x\n", result->channelRate.MaxChanTxRate );
        printf("Max channel Rx Rate        : 0x%x\n", result->channelRate.MaxChanRxRate );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 23))
    {
        printf("Last Call Tx Bytes OK      : 0x%"PRIX64"\n", result->lastCallTXOKBytesCnt );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 24))
    {
            printf("Last Call Rx Bytes OK      : 0x%"PRIX64"\n", result->lastCallRXOKBytesCnt );    
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
    {
        printf("Call Active Duration       : 0x%"PRIX64"\n", result->mdmCallDurationActive );
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 32))
    {
        printf("Last Call Data Bearer Tech : 0x%x\n", result->lastCallDataBearerTech );
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_GetDefaultProfileNumSettings(void* ptr)
{
    unpack_wds_GetDefaultProfileNum_t *p =
        (unpack_wds_GetDefaultProfileNum_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (p->ParamPresenceMask, 1))
    {
        printf("default profile number: %d\n", p->index);
    }
    memset(&DefaultProfileNumSettings,0,sizeof(pack_wds_GetDefaultProfileNum_t));
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
    DefaultProfileNumSettings.index = p->index;
}

void dump_SLQSGet3GPPConfigItemSettings(void* ptr)
{
    unpack_wds_SLQSGet3GPPConfigItem_t *p3gppconfigitem =
        (unpack_wds_SLQSGet3GPPConfigItem_t*) ptr;
    uint8_t    count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    memcpy(&g3GPPconfigitemSettings,p3gppconfigitem,sizeof(unpack_wds_SLQSGet3GPPConfigItem_t));
    printf( "\nLTE Attach Profile : %d", g3GPPconfigitemSettings.LTEAttachProfile );
    for( count = 0; count < 4; count++ )
     {
         printf( "\n\tprofileList[%d] : %d",
                  count,
                  g3GPPconfigitemSettings.profileList[count] );
     }
    printf( "\nDefault PDN enabled : %d",
            g3GPPconfigitemSettings.defaultPDNEnabled );
    printf( "\n3gppRelease : %d\n",
            g3GPPconfigitemSettings._3gppRelease );
    printf("LTE attach profile list: %d", g3GPPconfigitemSettings.LTEAttachProfileListLen);
    if(g3GPPconfigitemSettings.LTEAttachProfileListLen!=0xffff)
    for( count = 0; count < g3GPPconfigitemSettings.LTEAttachProfileListLen; count++ )
    {
         printf( "\n\tLTEAttachProfileList[%d] : %d",
                  count,
                  g3GPPconfigitemSettings.LTEAttachProfileList[count] );
    }
    printf("\n");

#if DEBUG_LOG_TO_FILE
    /* expected result */
    unpack_wds_SLQSGet3GPPConfigItem_t config_item;
    config_item.profileList[0] = 1;
    uint8_t is_matching = 0;

    if ( config_item.profileList[0] == p3gppconfigitem->profileList[0] )
    {
        is_matching = 1;
    }
    local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
#endif
    Default3GPPConfigItemSettings.LTEAttachProfileListLen = g3GPPconfigitemSettings.LTEAttachProfileListLen;
    Default3GPPConfigItemSettings.pLTEAttachProfileList= &g3GPPconfigitemSettings.LTEAttachProfileList[0];
    Default3GPPConfigItemSettings.pLTEAttachProfile = &g3GPPconfigitemSettings.LTEAttachProfile;
    Default3GPPConfigItemSettings.p3gppRelease = &g3GPPconfigitemSettings._3gppRelease;
    Default3GPPConfigItemSettings.pDefaultPDNEnabled = &g3GPPconfigitemSettings.defaultPDNEnabled;
    Default3GPPConfigItemSettings.pProfileList = &g3GPPconfigitemSettings.profileList[0];
}

void dump_SLQSGet3GPPConfigItemModemSettings(void* ptr)
{
    unpack_wds_SLQSGet3GPPConfigItem_t *p3gppconfigitem =
        (unpack_wds_SLQSGet3GPPConfigItem_t*) ptr;
    uint8_t    count;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);

    memset(&Default3GPPConfigItemSettings,0,sizeof(Default3GPPConfigItemSettings));
    swi_uint256_print_mask (p3gppconfigitem->ParamPresenceMask);
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 16))
    {
        printf( "\nLTE Attach Profile : %d", p3gppconfigitem->LTEAttachProfile );
        Default3GPPConfigItemSettings.pLTEAttachProfile = &p3gppconfigitem->LTEAttachProfile;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 17))
    {
        for( count = 0; count < 4; count++ )
        {
             printf( "\n\tprofileList[%d] : %d",
                      count,
                      p3gppconfigitem->profileList[count] );
        }
        Default3GPPConfigItemSettings.pProfileList = p3gppconfigitem->profileList;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 18))
    {
        printf( "\nDefault PDN enabled : %d",
            p3gppconfigitem->defaultPDNEnabled );
        Default3GPPConfigItemSettings.pDefaultPDNEnabled = &p3gppconfigitem->defaultPDNEnabled;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 19))
    {
        printf( "\n3gppRelease : %d\n",
                p3gppconfigitem->_3gppRelease );
        Default3GPPConfigItemSettings.p3gppRelease = &p3gppconfigitem->_3gppRelease;
    }
    if(swi_uint256_get_bit (p3gppconfigitem->ParamPresenceMask, 20))
    {
        printf("LTE attach profile list: %d", p3gppconfigitem->LTEAttachProfileListLen);
        for( count = 0; count < p3gppconfigitem->LTEAttachProfileListLen; count++ )
        {
             printf( "\n\tLTEAttachProfileList[%d] : %d",
                      count,
                      p3gppconfigitem->LTEAttachProfileList[count] );
        }
        printf("\n");
        Default3GPPConfigItemSettings.LTEAttachProfileListLen = p3gppconfigitem->LTEAttachProfileListLen;
        Default3GPPConfigItemSettings.pLTEAttachProfileList= p3gppconfigitem->LTEAttachProfileList;
    }
#if DEBUG_LOG_TO_FILE
    /* expected result */
    unpack_wds_SLQSGet3GPPConfigItem_t config_item;
    config_item.profileList[0] = 1;
    uint8_t is_matching = 0;

    if ( config_item.profileList[0] == p3gppconfigitem->profileList[0] )
    {
        is_matching = 1;
    }

    CHECK_WHITELIST_MASK(
        unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskWhiteList,
        p3gppconfigitem->ParamPresenceMask);
    CHECK_MANDATORYLIST_MASK(
        unpack_wds_SLQSGet3GPPConfigItemParamPresenceMaskMandatoryList,
        p3gppconfigitem->ParamPresenceMask);

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

void dump_wds_GetAutoconnectSettings(void* ptr)
{
    /* expected result */
    unpack_wds_GetAutoconnect_t *ps =
        (unpack_wds_GetAutoconnect_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    swi_uint256_print_mask(ps->ParamPresenceMask);
    if(swi_uint256_get_bit(ps->ParamPresenceMask, 1))
    {
        if(ps->psetting)
        DefaultAutoconnectSettings.acsetting = *(ps->psetting);
        if(ps->psetting)
        DefaultAutoconnectSettings.acroamsetting = *(ps->psetting);
        if(ps->psetting)
        printf("Setting: %d\n", *(ps->psetting));
    }
    

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_SLQSSGetLoopbackSettings ( void *ptr)
{
    unpack_wds_SLQSSGetLoopback_t *pLoopBack =
    (unpack_wds_SLQSSGetLoopback_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (pLoopBack->ParamPresenceMask, 16))
    {
        DefaultLoopbackSettings.loopbackMode = pLoopBack->ByteLoopbackMode;
        printf("Loopback Mode:0X%02X\n",DefaultLoopbackSettings.loopbackMode);
    }
    if(swi_uint256_get_bit (pLoopBack->ParamPresenceMask, 17))
    {
        DefaultLoopbackSettings.loopbackMultiplier = pLoopBack->ByteLoopbackMultiplier;
        printf("Loopback Multiplier:0X%02X\n",DefaultLoopbackSettings.loopbackMultiplier);
    }

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif
}

void dump_wds_GetDefaultProfileSettings( void *ptr)
{
    unpack_wds_GetDefaultProfile_t *defprofile =
        (unpack_wds_GetDefaultProfile_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (defprofile->ParamPresenceMask);
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 17))
    {
        DefaultProfileSettings.pdpType      = defprofile->pdptype;
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 30))
    {
        DefaultProfileSettings.ipAddress    = defprofile->ipaddr;
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 21))
    {
        DefaultProfileSettings.primaryDNS   = defprofile->pridns;
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 22))
    {
        DefaultProfileSettings.secondaryDNS = defprofile->secdns;
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 29))
    {
        DefaultProfileSettings.authentication = defprofile->auth;
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 16))
    {
        memcpy(DefaultProfileSettings.pUsername,&defprofile->name,defprofile->namesize);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 20))
    {
        memcpy(DefaultProfileSettings.pApnname,&defprofile->apnname,defprofile->apnsize);
    }
    printf("received profile details are as follows:");
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 17))
    {
        printf("\npdpype :%x",defprofile->pdptype);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 30))
    {
        printf("\nipaddress :%x",defprofile->ipaddr);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 21))
    {
        printf("\nprimarydns :%x",defprofile->pridns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 22))
    {
        printf("\nsecondarydns :%x",defprofile->secdns);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 29))
    {
        printf("\nauthentication :%x",defprofile->auth);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 16))
    {
        printf("\nname : %s",defprofile->name);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 20))
    {
        printf("\napnname :%s",defprofile->apnname);
    }
    if(swi_uint256_get_bit(defprofile->ParamPresenceMask, 27))
    {
        printf("\nusername :%s\n",defprofile->username);
    }
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s\n",  "N/A");
#endif

}

void dump_wds_SLQSSwiProfileChangeCallback(void* ptr)
{
    unpack_wds_SLQSSwiProfileChangeCallback_t *result =
        (unpack_wds_SLQSSwiProfileChangeCallback_t*)ptr;
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

void dump_wds_SLQSSwiProfileChangeCallback_ind(void* ptr)
{
    unpack_wds_SLQSSwiProfileChangeCallback_Ind_t *result =
        (unpack_wds_SLQSSwiProfileChangeCallback_Ind_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);

    if(swi_uint256_get_bit (result->ParamPresenceMask, 1) )
    {
        printf ("Profile Index: 0x%02x\n",result->ProfileTlv.profileInx );
        printf ("Profile Type: 0x%02x\n",result->ProfileTlv.profileType );
    }
    if( swi_uint256_get_bit (result->ParamPresenceMask, 16) )
    {
        printf ("Profile Chnage Source: 0x%02x\n",result->srcTlv.source );
    }
}

uint8_t wds_profileType = 0; // 3GPP profile
uint8_t wds_ProfileId = 15;
uint8_t wds_pdpType = 0x00;

#define WDS_DEFAULT_PROFILE_NAME "profile1"
#define WDS_DEFAULT_PROFILE_NAME_LENGTH (sizeof(WDS_DEFAULT_PROFILE_NAME))
#define WDS_DEFAULT_APN_NAME "internet"
#define WDS_DEFAULT_APN_NAME_LENGTH (sizeof(WDS_DEFAULT_APN_NAME))
#define WDS_DEFAULT_USER_NAME "sierra"
#define WDS_DEFAULT_USER_NAME_LENGTH (sizeof(WDS_DEFAULT_USER_NAME))
#define WDS_DEFAULT_PASSWORD "123456"
#define WDS_DEFAULT_PASSWORD_LENGTH (sizeof(WDS_DEFAULT_PASSWORD))

 
uint8_t   wds_profilename[]=WDS_DEFAULT_PROFILE_NAME;
uint16_t  wds_profilename_length=WDS_DEFAULT_PROFILE_NAME_LENGTH;
uint8_t   wds_pdpHdrCompType = 2;
uint8_t   wds_pdpDataCompType = 3;
uint8_t   wds_APNname[]=WDS_DEFAULT_APN_NAME;
uint16_t   wds_APNnameLength=WDS_DEFAULT_APN_NAME_LENGTH;
uint32_t  wds_priDNSIPv4AddPref = 0;
uint32_t  wds_secDNSIPv4AddPref = 0;
uint8_t   wds_username[]=WDS_DEFAULT_USER_NAME;
uint16_t   wds_usernamelength=WDS_DEFAULT_USER_NAME_LENGTH;
uint8_t   wds_password[]=WDS_DEFAULT_PASSWORD;
uint16_t   wds_passwordlength=WDS_DEFAULT_PASSWORD_LENGTH;
uint8_t   wds_resp_profileType = 1;
uint8_t  wds_EmgCallSupport = 1;




wds_profileInfo wdsprofileInfo={
    {
        wds_profilename,//uint8_t                    *pProfilename;
        &wds_profilename_length,//uint16_t                   *pProfilenameSize;
        &wds_pdpType,//uint8_t                    *pPDPtype;
        &wds_pdpHdrCompType,//uint8_t                    *pPdpHdrCompType;
        &wds_pdpDataCompType,//uint8_t                    *pPdpDataCompType;
        wds_APNname,//uint8_t                    *pAPNName;
        &wds_APNnameLength,//uint16_t                   *pAPNnameSize;
        &wds_priDNSIPv4AddPref,//uint32_t                   *pPriDNSIPv4AddPref;
        &wds_secDNSIPv4AddPref,//uint32_t                   *pSecDNSIPv4AddPref;
        0,//LibPackUMTSQoS             *pUMTSReqQoS;
        0,//LibPackUMTSQoS             *pUMTSMinQoS;
        0,//LibPackGPRSRequestedQoS    *pGPRSRequestedQos;
        0,//LibPackGPRSRequestedQoS    *pGPRSMinimumQoS;
        wds_username,//uint8_t                    *pUsername;
        &wds_usernamelength,//uint16_t                    *pUsernameSize;
        wds_password,//uint8_t                    *pPassword;
        &wds_passwordlength,//uint16_t                   *pPasswordSize;
        0,//uint8_t                    *pAuthenticationPref;
        0,//uint32_t                   *pIPv4AddrPref;
        0,//uint8_t                    *pPcscfAddrUsingPCO;
        0,//uint8_t                    *pPdpAccessConFlag;
        0,//uint8_t                    *pPcscfAddrUsingDhcp;
        0,//uint8_t                    *pImCnFlag;
        0,//struct wds_TFTIDParams         *pTFTID1Params;
        0,//struct wds_TFTIDParams         *pTFTID2Params;
        0,//uint8_t                    *pPdpContext;
        0,//uint8_t                    *pSecondaryFlag;
        0,//uint8_t                    *pPrimaryID;
        0,//uint16_t                   *pIPv6AddPref;
        0,//LibPackUMTSReqQoSSigInd    *pUMTSReqQoSSigInd;
        0,//.LibPackUMTSReqQoSSigInd    *pUMTSMinQosSigInd;
        0,//uint16_t                   *pPriDNSIPv6addpref;
        0,//uint16_t                   *pSecDNSIPv6addpref;
        0,//uint8_t                    *pAddrAllocPref;
        0,//LibPackQosClassID          *pQosClassID;
        0,//uint8_t                    *pAPNDisabledFlag;
        0,//uint32_t                   *pPDNInactivTimeout;
        0,//uint8_t                    *pAPNClass;
        &wds_EmgCallSupport//uint8_t   *pSupportEmergencyCalls
    }
};
uint8_t wds_profileTypeResp = 255; // 3GPP profile
uint8_t wds_ProfileIdResp = 255;
uint8_t wds_pdpTypeResp = 0xff;

LibPackprofile_3GPP wds_profile3GPPResp;
uint8_t   wds_profilenameResp[255]={0};
uint16_t  wds_profilename_lengthResp=255;
uint8_t   wds_pdpHdrCompTypeResp = 0;
uint8_t   wds_pdpDataCompTypeResp = 0;
uint8_t   wds_APNnameResp[255]={0};
uint16_t   wds_APNnameLengthResp=255;
uint32_t  wds_priDNSIPv4AddPrefResp = 0;
uint32_t  wds_secDNSIPv4AddPrefResp = 0;
uint8_t   wds_usernameResp[255]={0};
uint16_t   wds_usernamelengthResp=255;
uint8_t   wds_passwordResp[255]={0};
uint16_t   wds_passwordlengthResp=255;
uint8_t   wds_resp_profileTypeResp = 0;
uint8_t   wds_resp_EmgCallResp = 0;

wds_profileInfo wdsprofileInforesp={
    {
        wds_profilenameResp,//uint8_t                    *pProfilename;
        &wds_profilename_lengthResp,//uint16_t                   *pProfilenameSize;
        &wds_pdpTypeResp,//uint8_t                    *pPDPtype;
        &wds_pdpHdrCompTypeResp,//uint8_t                    *pPdpHdrCompType;
        &wds_pdpDataCompTypeResp,//uint8_t                    *pPdpDataCompType;
        wds_APNnameResp,//uint8_t                    *pAPNName;
        &wds_APNnameLengthResp,//uint16_t                   *pAPNnameSize;
        &wds_priDNSIPv4AddPrefResp,//uint32_t                   *pPriDNSIPv4AddPref;
        &wds_secDNSIPv4AddPrefResp,//uint32_t                   *pSecDNSIPv4AddPref;
        0,//LibPackUMTSQoS             *pUMTSReqQoS;
        0,//LibPackUMTSQoS             *pUMTSMinQoS;
        0,//LibPackGPRSRequestedQoS    *pGPRSRequestedQos;
        0,//LibPackGPRSRequestedQoS    *pGPRSMinimumQoS;
        wds_usernameResp,//uint8_t                    *pUsername;
        &wds_usernamelengthResp,//uint16_t                    *pUsernameSize;
        wds_passwordResp,//uint8_t                    *pPassword;
        &wds_passwordlengthResp,//uint16_t                   *pPasswordSize;
        0,//uint8_t                    *pAuthenticationPref;
        0,//uint32_t                   *pIPv4AddrPref;
        0,//uint8_t                    *pPcscfAddrUsingPCO;
        0,//uint8_t                    *pPdpAccessConFlag;
        0,//uint8_t                    *pPcscfAddrUsingDhcp;
        0,//uint8_t                    *pImCnFlag;
        0,//struct wds_TFTIDParams         *pTFTID1Params;
        0,//struct wds_TFTIDParams         *pTFTID2Params;
        0,//uint8_t                    *pPdpContext;
        0,//uint8_t                    *pSecondaryFlag;
        0,//uint8_t                    *pPrimaryID;
        0,//uint16_t                   *pIPv6AddPref;
        0,//LibPackUMTSReqQoSSigInd    *pUMTSReqQoSSigInd;
        0,//.LibPackUMTSReqQoSSigInd    *pUMTSMinQosSigInd;
        0,//uint16_t                   *pPriDNSIPv6addpref;
        0,//uint16_t                   *pSecDNSIPv6addpref;
        0,//uint8_t                    *pAddrAllocPref;
        0,//LibPackQosClassID          *pQosClassID;
        0,//uint8_t                    *pAPNDisabledFlag;
        0,//uint32_t                   *pPDNInactivTimeout;
        0,//uint8_t                    *pAPNClass;
        &wds_resp_EmgCallResp//uint8_t   *pSupportEmergencyCalls
    }
};


pack_wds_SLQSCreateProfile_t tSLQSCreateProfile = {
    &wds_ProfileId,&wds_profileType,&wdsprofileInfo
};

PackCreateProfileOut profile_out;
uint8_t profile_id_out;
unpack_wds_SLQSCreateProfile_t tunpack_create_profile = {
    &profile_out, &profile_id_out, 0,SWI_UINT256_INT_VALUE
};

pack_wds_SLQSGetProfileSettings_t tSLQSGetProfileSettings ={
    1,// 1-16
    0
};
uint16_t SLQSGetProfileSettingsExtErrCode = 0;
wds_profileInfo SLQSGetProfileSettingsCurProfile;
uint16_t uResult;

#if 1
uint8_t unPackGetProfileSettingOutProfilenameResp[255]={0};
uint16_t unPackGetProfileSettingOutProfilenameSizeResp=255;
uint8_t unPackGetProfileSettingOutAPNNameResp[255]={0};
uint16_t unPackGetProfileSettingOutAPNNameSizeResp=255;
uint8_t unPackGetProfileSettingDPtypeResp;

uint8_t unPackGetProfileSettingOutPasswordResp[255]={0};
uint16_t unPackGetProfileSettingOutPasswordSizeResp=255;
uint8_t unPackGetProfileSettingOutUserResp[255]={0};
uint16_t unPackGetProfileSettingOutUserSizeResp=255;
uint32_t unPackGetProfileSettingOutIPv4AddrPrefResp=255;
uint32_t unPackGetProfileSettingOutDNSPriAddrPrefResp=255;
uint32_t unPackGetProfileSettingOutDNSSecAddrPrefResp=255;
uint8_t unPackGetProfileSettingOutAuthenticationPrefResp=255;
uint8_t unPackGetProfileSettingOutAPNClassResp=255;
uint64_t unPackGetProfileSettingOutAPNBearer;
uint8_t unPackGetProfileSettingOutEmergencyCallResp=255;

uint16_t                    unPackGetProfileSettingOperatorPCOID;
uint16_t                    unPackGetProfileSettingMcc;
LibPackProfileMnc           unPackGetProfileSettingMnc;
uint16_t                    unPackGetProfileSettingMaxPDN;
uint16_t                    unPackGetProfileSettingMaxPDNTimer;
uint16_t                    unPackGetProfileSettingPDNWaitTimer;
uint32_t                    unPackGetProfileSettingAppUserData;
uint8_t                     unPackGetProfileSettingRoamDisallowFlag;
uint8_t                     unPackGetProfileSettingPDNDisconnectWaitTimer;
uint8_t                     unPackGetProfileSettingDnsWithDHCPFlag;
uint32_t                    unPackGetProfileSettingLteRoamPDPType;
uint32_t                    unPackGetProfileSettingUmtsRoamPDPType;
uint8_t                     unPackGetProfileSettingIWLANtoLTEHandoverFlag;
uint8_t                     unPackGetProfileSettingLTEtoIWLANHandoverFlag;
LibPackPDNThrottleTimer     unPackGetProfileSettingPDNThrottleTimer;
uint32_t                    unPackGetProfileSettingOverridePDPType;
LibPackPCOIDList            unPackGetProfileSettingPCOIDList;
uint8_t                     unPackGetProfileSettingMsisdnFlag;
uint8_t                     unPackGetProfileSettingPersistFlag;
uint8_t                     unPackGetProfileSettingOutClatFlag;
uint8_t                     unPackGetProfileSettingOutIPV6DelegFlag;


uint8_t unPackGetProfileSettingOutPdpHdrCompTypeResp=255;
uint8_t unPackGetProfileSettingOutPdpDataCompTypeResp=255;
LibPackUMTSQoS          unPackGetProfileSettingOutUMTSReqQoS;
LibPackUMTSQoS          unPackGetProfileSettingOutUMTSMinQoS;
LibPackGPRSRequestedQoS unPackGetProfileSettingOutGPRSRequestedQos;
LibPackGPRSRequestedQoS unPackGetProfileSettingOutGPRSMinimumQoS;
uint8_t unPackGetProfileSettingOutPcscfAddrUsingPCO=255;

#endif

uint8_t   unPackGetProfileSettingOutNegoDnsSrvrPref;
uint32_t  unPackGetProfileSettingOutPppSessCloseTimerDO;
uint32_t  unPackGetProfileSettingOutPppSessCloseTimer1x;
uint8_t   unPackGetProfileSettingOutAllowLinger;
uint16_t  unPackGetProfileSettingOutLcpAckTimeout;
uint16_t  unPackGetProfileSettingOutIpcpAckTimeout;
uint16_t  unPackGetProfileSettingOutAuthTimeout;
uint8_t   unPackGetProfileSettingOutLcpCreqRetryCount;
uint8_t   unPackGetProfileSettingOutIpcpCreqRetryCount;
uint8_t   unPackGetProfileSettingOutAuthRetryCount;
uint8_t   unPackGetProfileSettingOutAuthProtocol;
uint8_t   unPackGetProfileSettingOutUserId[127];
uint16_t  unPackGetProfileSettingOutUserIdSize = 255;
uint8_t   unPackGetProfileSettingOutAuthPassword[127];
uint16_t  unPackGetProfileSettingOutAuthPasswordSize = 255;
uint8_t   unPackGetProfileSettingOutDataRate;
uint32_t  unPackGetProfileSettingOutAppType;
uint8_t   unPackGetProfileSettingOutDataMode;
uint8_t   unPackGetProfileSettingOutAppPriority;
uint8_t   unPackGetProfileSettingOutApnString[100];
uint16_t  unPackGetProfileSettingOutApnStringSize = 255;
uint8_t   unPackGetProfileSettingOutPdnType;
uint8_t   unPackGetProfileSettingOutIsPcscfAddressNedded;
uint32_t  unPackGetProfileSettingOutPrimaryV4DnsAddress[8];
uint32_t  unPackGetProfileSettingOutSecondaryV4DnsAddress[8];
uint16_t  unPackGetProfileSettingOutPriV6DnsAddress;
uint16_t  unPackGetProfileSettingOutSecV6DnsAddress;
uint8_t   unPackGetProfileSettingOutRATType;
uint8_t   unPackGetProfileSettingOutAPNEnabled3GPP2;
uint32_t  unPackGetProfileSettingOutPDNInactivTimeout3GPP2;
uint8_t   unPackGetProfileSettingOutAPNClass3GPP2;

uint16_t unPackGetProfileSettingExtErrCodeResp;
uint8_t                    unPackGetProfileSettingOutPdpAccessConFlag;
uint8_t                    unPackGetProfileSettingOutPcscfAddrUsingDhcp;
uint8_t                    unPackGetProfileSettingOutImCnFlag;
uint16_t TFTID1SourceIP[WDS_TFTID_SOURCE_IP_SIZE];
uint16_t TFTID2SourceIP[WDS_TFTID_SOURCE_IP_SIZE];

LibPackTFTIDParams      unPackGetProfileSettingOutTFTID1Params = {
    0,//uint8_t  filterId;
    0,//uint8_t  eValid;
    0,//uint8_t  ipVersion;
    &TFTID1SourceIP[0],//uint16_t  *pSourceIP;
    0,//uint8_t  sourceIPMask;
    0,//uint8_t  nextHeader;
    0,//uint32_t  destPortRangeStart;
    0,//uint16_t  destPortRangeEnd;
    0,//uint16_t  srcPortRangeStart;
    0,//uint16_t  srcPortRangeEnd;
    0,//uint32_t IPSECSPI;
    0,//uint16_t  tosMask;
    0,//uint32_t flowLabel;
};
LibPackTFTIDParams      unPackGetProfileSettingOutTFTID2Params= {
    0,//uint8_t  filterId;
    0,//uint8_t  eValid;
    0,//uint8_t  ipVersion;
    &TFTID2SourceIP[0],//uint16_t  *pSourceIP;
    0,//uint8_t  sourceIPMask;
    0,//uint8_t  nextHeader;
    0,//uint32_t  destPortRangeStart;
    0,//uint16_t  destPortRangeEnd;
    0,//uint16_t  srcPortRangeStart;
    0,//uint16_t  srcPortRangeEnd;
    0,//uint32_t IPSECSPI;
    0,//uint16_t  tosMask;
    0,//uint32_t flowLabel;
};
uint8_t                    unPackGetProfileSettingOutPdpContext;
uint8_t                    unPackGetProfileSettingOutSecondaryFlag;
uint8_t                    unPackGetProfileSettingOutPrimaryID;
uint16_t                  unPackGetProfileSettingOutIPv6AddPref;
LibPackUMTSReqQoSSigInd unPackGetProfileSettingOutUMTSReqQoSSigInd;
LibPackUMTSReqQoSSigInd unPackGetProfileSettingOutUMTSMinQosSigInd;
uint16_t                  unPackGetProfileSettingOutPriDNSIPv6addpref;
uint16_t                  unPackGetProfileSettingOutSecDNSIPv6addpref;
uint8_t                    unPackGetProfileSettingOutAddrAllocPref;
LibPackQosClassID       unPackGetProfileSettingOutQosClassID;
uint8_t                    unPackGetProfileSettingOutAPNDisabledFlag;
uint32_t                   unPackGetProfileSettingOutPDNInactivTimeout;

UnPackGetProfileSettingOut unPackGetProfileSettingOut={
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        &unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &unPackGetProfileSettingOutEmergencyCallResp //uint8_t                     *pSupportEmergencyCalls
        }
    },
    &unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 unPackGetProfileSettingOutV2={
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        &unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &unPackGetProfileSettingOutAPNBearer,
        &unPackGetProfileSettingOutEmergencyCallResp, //uint8_t                     *pSupportEmergencyCalls
        &unPackGetProfileSettingOperatorPCOID, //uint16_t                    *pOperatorPCOID
        &unPackGetProfileSettingMcc, //uint16_t                    *pMcc
        &unPackGetProfileSettingMnc, //LibPackProfileMnc           *pMnc;
        &unPackGetProfileSettingMaxPDN, //uint16_t                    *pMaxPDN
        &unPackGetProfileSettingMaxPDNTimer, //uint16_t                    *pMaxPDNWaitTimer
        &unPackGetProfileSettingPDNWaitTimer, //uint16_t                    *pMaxPDNTimer
        &unPackGetProfileSettingAppUserData, //uint32_t                   *pAppUserData
        &unPackGetProfileSettingRoamDisallowFlag, //uint8_t                    *pRoamDisallowFlag
        &unPackGetProfileSettingPDNDisconnectWaitTimer, //uint8_t                    *pPDNDisconnectWaitTimer
        &unPackGetProfileSettingDnsWithDHCPFlag, //uint8_t                    *pDnsWithDHCPFlag
        &unPackGetProfileSettingLteRoamPDPType, //uint32_t                   *pLteRoamPDPType
        &unPackGetProfileSettingUmtsRoamPDPType, //uint32_t                   *pUmtsRoamPDPType
        &unPackGetProfileSettingIWLANtoLTEHandoverFlag,
        &unPackGetProfileSettingLTEtoIWLANHandoverFlag,
        &unPackGetProfileSettingPDNThrottleTimer, //LibPackPDNThrottleTimer    *pPDNThrottleTimer
        &unPackGetProfileSettingOverridePDPType, //uint32_t                   *pOverridePDPType
        &unPackGetProfileSettingPCOIDList,
        &unPackGetProfileSettingMsisdnFlag,
        &unPackGetProfileSettingPersistFlag,
        &unPackGetProfileSettingOutClatFlag,
        &unPackGetProfileSettingOutIPV6DelegFlag
        }
    },
    &unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

unpack_wds_SLQSGetProfileSettings_t tSLQSGetProfileSettingsRsp ={
    &unPackGetProfileSettingOut,WDS_PROFILE_3GPP,0,SWI_UINT256_INT_VALUE};

unpack_wds_SLQSGetProfileSettingsV2_t tSLQSGetProfileSettingsRspV2 ={
    &unPackGetProfileSettingOutV2,WDS_PROFILE_3GPP,0,SWI_UINT256_INT_VALUE};


pack_wds_SLQSGetProfileSettings_t tSLQSGetProfileSettings_3GPP2 ={
    0,// Profile ID
    1 //Profile Type (3GPP2)
};


UnPackGetProfileSettingOut unPackGetProfileSetting3GPP2Out={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&unPackGetProfileSettingOutAllowLinger,
        (void*)&unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&unPackGetProfileSettingOutAuthTimeout,
        (void*)&unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&unPackGetProfileSettingOutAuthRetryCount,
        (void*)&unPackGetProfileSettingOutAuthProtocol,
        (void*)unPackGetProfileSettingOutUserId,
        (void*)&unPackGetProfileSettingOutUserIdSize,
        (void*)unPackGetProfileSettingOutAuthPassword,
        (void*)&unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&unPackGetProfileSettingOutDataRate,
        (void*)&unPackGetProfileSettingOutAppType,
        (void*)&unPackGetProfileSettingOutDataMode,
        (void*)&unPackGetProfileSettingOutAppPriority,
        (void*)unPackGetProfileSettingOutApnString,
        (void*)&unPackGetProfileSettingOutApnStringSize,
        (void*)&unPackGetProfileSettingOutPdnType,
        (void*)&unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)&unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)&unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&unPackGetProfileSettingOutRATType,
        (void*)&unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&unPackGetProfileSettingOutAPNClass3GPP2,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
        }
    },
        &unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 unPackGetProfileSetting3GPP2OutV2={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&unPackGetProfileSettingOutAllowLinger,
        (void*)&unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&unPackGetProfileSettingOutAuthTimeout,
        (void*)&unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&unPackGetProfileSettingOutAuthRetryCount,
        (void*)&unPackGetProfileSettingOutAuthProtocol,
        (void*)unPackGetProfileSettingOutUserId,
        (void*)&unPackGetProfileSettingOutUserIdSize,
        (void*)unPackGetProfileSettingOutAuthPassword,
        (void*)&unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&unPackGetProfileSettingOutDataRate,
        (void*)&unPackGetProfileSettingOutAppType,
        (void*)&unPackGetProfileSettingOutDataMode,
        (void*)&unPackGetProfileSettingOutAppPriority,
        (void*)unPackGetProfileSettingOutApnString,
        (void*)&unPackGetProfileSettingOutApnStringSize,
        (void*)&unPackGetProfileSettingOutPdnType,
        (void*)&unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)&unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)&unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&unPackGetProfileSettingOutRATType,
        (void*)&unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&unPackGetProfileSettingOutAPNClass3GPP2,
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
        }
    },
        &unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

unpack_wds_SLQSGetProfileSettings_t tSLQSGetProfileSettings3GPP2Rsp ={
    &unPackGetProfileSetting3GPP2Out,WDS_PROFILE_3GPP2,0,SWI_UINT256_INT_VALUE};

unpack_wds_SLQSGetProfileSettingsV2_t tSLQSGetProfileSettings3GPP2RspV2 ={
    &unPackGetProfileSetting3GPP2OutV2,WDS_PROFILE_3GPP2,0,SWI_UINT256_INT_VALUE};

uint8_t wdsModiflyprofileType = 0; // 3GPP profile
uint8_t wdsModiflyProfileId = 15;
uint8_t wdsModiflypdpType = 0x00;

uint8_t ModifyProfileFileName[]="newProfile";
uint16_t ModifyProfileFileNameSize=10;
uint8_t ModifyProfileAPNName[]="internet";
uint16_t ModifyProfileAPNNameSize=8;

pack_wds_GetDefaultProfileNum_t tpack_wds_GetDefaultProfileNum = {0, 0};
unpack_wds_GetDefaultProfileNum_t tunpack_wds_GetDefaultProfileNum;

pack_wds_SLQSModifyProfile_t tpack_wds_SLQSModifyProfile={
    &wdsModiflyProfileId,//Profile ID
    &wdsModiflyprofileType,//Profile Type
    {
        {
            ModifyProfileFileName,//uint8_t                    *pProfilename;
            &ModifyProfileFileNameSize,//uint16_t                   *pProfilenameSize;
            &wdsModiflypdpType,//uint8_t                    *pPDPtype;
            0,//uint8_t                    *pPdpHdrCompType;
            0,//uint8_t                    *pPdpDataCompType;
            ModifyProfileAPNName,//uint8_t                    *pAPNName;
            &ModifyProfileAPNNameSize,//uint16_t                   *pAPNnameSize;
            0,//uint32_t                   *pPriDNSIPv4AddPref;
            0,//uint32_t                   *pSecDNSIPv4AddPref;
            0,//LibPackUMTSQoS             *pUMTSReqQoS;
            0,//LibPackUMTSQoS             *pUMTSMinQoS;
            0,//LibPackGPRSRequestedQoS    *pGPRSRequestedQos;
            0,//LibPackGPRSRequestedQoS    *pGPRSMinimumQoS;
            0,//uint8_t                    *pUsername;
            0,//uint16_t                    *pUsernameSize;
            0,//uint8_t                    *pPassword;
            0,//uint16_t                   *pPasswordSize;
            0,//uint8_t                    *pAuthenticationPref;
            0,//uint32_t                   *pIPv4AddrPref;
            0,//uint8_t                    *pPcscfAddrUsingPCO;
            0,//uint8_t                    *pPdpAccessConFlag;
            0,//uint8_t                    *pPcscfAddrUsingDhcp;
            0,//uint8_t                    *pImCnFlag;
            0,//struct wds_TFTIDParams         *pTFTID1Params;
            0,//struct wds_TFTIDParams         *pTFTID2Params;
            0,//uint8_t                    *pPdpContext;
            0,//uint8_t                    *pSecondaryFlag;
            0,//uint8_t                    *pPrimaryID;
            0,//uint16_t                   *pIPv6AddPref;
            0,//LibPackUMTSReqQoSSigInd    *pUMTSReqQoSSigInd;
            0,//.LibPackUMTSReqQoSSigInd    *pUMTSMinQosSigInd;
            0,//uint16_t                   *pPriDNSIPv6addpref;
            0,//uint16_t                   *pSecDNSIPv6addpref;
            0,//uint8_t                    *pAddrAllocPref;
            0,//LibPackQosClassID          *pQosClassID;
            0,//uint8_t                    *pAPNDisabledFlag;
            0,//uint32_t                   *pPDNInactivTimeout;
            0,//uint8_t                    *pAPNClass;
            0,//uint8_t                    *pSupportEmergencyCalls
        }
    }
};
unpack_wds_SLQSModifyProfile_t tunpack_wds_SLQSModifyProfile;

uint32_t tpack_wds_SLQSStartDataSessionProfileid3gpp[]={PROFILEID_1,PROFILEID_2,PROFILEID_3,
   PROFILEID_4,PROFILEID_5,PROFILEID_6,
   PROFILEID_7,PROFILEID_8,
};
pack_wds_SLQSStartDataSession_t tpack_wds_SLQSStartDataSession[]={
    {
        NULL,
        &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile1Slot],
        NULL,
        NULL,
        NULL,
        NULL},
   {
      NULL,
      &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile2Slot],
      NULL,
      NULL,
      NULL,
      NULL},

    {
        NULL,
        &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile3Slot],
        NULL,
        NULL,
        NULL,
        NULL},
    {
        NULL,
        &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile4Slot],
        NULL,
        NULL,
        NULL,
        NULL},
   {
      NULL,
      &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile5Slot],
      NULL,
      NULL,
      NULL,
      NULL},

    {
        NULL,
        &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile6Slot],
        NULL,
        NULL,
        NULL,
        NULL},
    {
        NULL,
        &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile7Slot],
        NULL,
        NULL,
        NULL,
        NULL},
   {
      NULL,
      &tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile8Slot],
      NULL,
      NULL,
      NULL,
      NULL}
    
};

uint32_t tpack_wds_GetRuntimeSettings=0xFFFF;
pack_wds_SLQSGetRuntimeSettings_t tpack_wds_SLQSGetRuntimeSettings={
    &tpack_wds_GetRuntimeSettings
};

uint32_t tunpack_wds_SLQSStartDataSessionsid=0;
uint32_t tunpack_wds_SLQSStartDataSessionFailureReason=0;
uint32_t tunpack_wds_SLQSStartDataSessionVerboseFailReasonType=0;
uint32_t tunpack_wds_SLQSStartDataSessionVerboseFailureReason=0;
unpack_wds_SLQSStartDataSession_t tunpack_wds_SLQSStartDataSession={
    &tunpack_wds_SLQSStartDataSessionsid, &tunpack_wds_SLQSStartDataSessionFailureReason,
    &tunpack_wds_SLQSStartDataSessionVerboseFailReasonType,
    &tunpack_wds_SLQSStartDataSessionVerboseFailureReason,
    SWI_UINT256_INT_VALUE
};

uint32_t tpack_wds_SLQSStopDataSessionsid=0;
pack_wds_SLQSStopDataSession_t tpack_wds_SLQSStopDataSession={
    &tunpack_wds_SLQSStartDataSessionsid
};

unpack_wds_SLQSStopDataSession_t tunpack_wds_SLQSStopDataSession=
    SWI_INIT_UNPACK_RESULT_VALUE;

uint8_t  tpack_wds_SetMobileIPProfileEnabled = 1;
uint32_t tpack_wds_SetMobileIPProfileAddress = 0xFFFFFFFF;
uint32_t tpack_wds_SetMobileIPProfilePrimaryHA  = 0xFFFFFFFF;
uint32_t tpack_wds_SetMobileIPProfileSecondaryHA = 0xFFFFFFFF;
uint8_t  tpack_wds_SetMobileIPProfileRevTunneling = 1;
uint32_t tpack_wds_SetMobileIPProfileHASPI = 0xFFFFFFFF;
uint32_t tpack_wds_SetMobileIPProfileAAASPI = 0xFFFFFFFF;
int8_t   tpack_wds_SetMobileIPProfileNAI[] = "www.sierrawireless.com";
int8_t   tpack_wds_SetMobileIPProfileMNHA[] = "test";
int8_t   tpack_wds_SetMobileIPProfileMNAAA[] = "test";


pack_wds_SetMobileIPProfile_t tpack_wds_SetMobileIPProfile={
    {0},
    1,
    &tpack_wds_SetMobileIPProfileEnabled,
    &tpack_wds_SetMobileIPProfileAddress,
    &tpack_wds_SetMobileIPProfilePrimaryHA,
    &tpack_wds_SetMobileIPProfileSecondaryHA,
    &tpack_wds_SetMobileIPProfileRevTunneling,
    tpack_wds_SetMobileIPProfileNAI,
    &tpack_wds_SetMobileIPProfileHASPI,
    &tpack_wds_SetMobileIPProfileAAASPI,
    tpack_wds_SetMobileIPProfileMNHA,
    tpack_wds_SetMobileIPProfileMNAAA,
};
unpack_wds_SetMobileIPProfile_t tunpack_wds_SetMobileIPProfile={
    SWI_UINT256_INT_VALUE
};

pack_wds_GetMobileIPProfile_t tpack_wds_GetMobileIPProfile={
    0
};
unpack_wds_GetMobileIPProfile_t tunpack_wds_GetMobileIPProfile ={
    0,
    0,
    0,
    0,
    0,
    0,
    {0},
    0,
    0,
    0,
    0,
    SWI_UINT256_INT_VALUE
};


pack_wds_RMSetTransferStatistics_t  tpack_wds_RMSetTransferStatistics={
        {0x01,0x000003C3}};
unpack_wds_RMSetTransferStatistics_t tunpack_wds_RMSetTransferStatistics={
    SWI_UINT256_INT_VALUE
};
pack_wds_SLQSWdsSwiPDPRuntimeSettings_t tpack_wds_SLQSWdsSwiPDPRuntimeSettings[]={
    {PROFILEID_1,0},
    {PROFILEID_3,0}
};
unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t tunpack_wds_SLQSWdsSwiPDPRuntimeSettings={
    0,//uint8_t                   contextId;
    0,//uint8_t                   bearerId;
    {0},//int8_t                    apnName[100];
    0,//uint32_t                  ipv4Address;
    0,//uint32_t                  ipv4GWAddress;
    0,//uint32_t                  prDNSIPv4Address;
    0,//uint32_t                  seDNSIPv4Address;
    {0,{0}},//struct ipv6AddressInfo    ipv6Address;
    {0,{0}},//struct ipv6AddressInfo    ipv6GWAddress;
    {0},//uint16_t                  prDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    {0},//uint16_t                  seDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
      0,//uint32_t                  prPCSCFIPv4Address;
      0,//uint32_t                  sePCSCFIPv4Address;
    {0},//uint16_t                  prPCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    {0},//uint16_t                  sePCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    SWI_UINT256_INT_VALUE,//swi_uint256_t  ParamPresenceMask;
};
pack_wds_SLQSSetWdsEventCallback_t tpack_wds_SLQSSetWdsEventCallback = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    5
};

unpack_wds_SLQSSetWdsEventCallback_t tunpack_wds_SLQSSetWdsEventCallback = 
    SWI_INIT_UNPACK_RESULT_VALUE;

pack_wds_GetPacketStatus_t tpack_wds_GetPacketStatus = {
    PKT_STAT_STAT_MASK
};

unpack_wds_GetPacketStatus_t tunpack_wds_GetPacketStatus = {
    0, // uint32_t    tXPacketSuccesses - Tx Packets OK
    0, // uint32_t    rXPacketSuccesses - Rx Packets OK
    0, // uint32_t    tXPacketErrors Tx - Packet Errors
    0, // uint32_t    rXPacketErrors Rx - Packet Errors
    0, // uint32_t    tXPacketOverflows - Tx Overflows
    0, // uint32_t    rXPacketOverflows - Rx Overflows
    0, // uint64_t    tXOkBytesCount Tx - Bytes OK
    0, // uint64_t    rXOkBytesCount Rx - Bytes OK
    0, // uint64_t    tXOKBytesLastCall - Last call Tx Bytes OK
    0, // uint64_t    rXOKBytesLastCall - Last call Rx Bytes OK
    0, // uint32_t    tXDroppedCount Tx - Packets Dropped
    0, // uint32_t    rXDroppedCount Rx - Packets Dropped
    SWI_UINT256_INT_VALUE//wi_uint256_t  ParamPresenceMask;
};

pack_wds_GetDefaultProfile_t tpack_wds_GetDefaultProfile  = {
    0 // PDP profile type, IPV4
};

unpack_wds_GetDefaultProfile_t tunpack_wds_GetDefaultProfile = {
    0, // uint32_t    pdptype pdp type
    0, // uint32_t    ipaddr ip address
    0, // uint32_t    pridns primary dns
    0, // uint32_t    secdns secondry dns
    {0}, // uint16_t    ipaddrv6 ip address v6
    {0}, // uint16_t    pridnsv6 primary dns v6
    {0}, // uint16_t    secdnsv6 secondry dns v6
    0, // uint32_t    auth; authentication type
    0, // uint8_t     namesize profile name size
    {0}, // int8_t    name profile name
    0, //uint8_t      apnsize apn size
    {0}, // int8_t    apnname apn name
    0, // uint8_t     usersize username size
    {0}, // int8_t     username username
    SWI_UINT256_INT_VALUE // ParamPresenceMask
};

unpack_wds_GetDefaultProfileV2_t tunpack_wds_GetDefaultProfileV2 = {
    0, // uint32_t    pdptype pdp type
    0, // uint32_t    ipaddr ip address
    0, // uint32_t    pridns primary dns
    0, // uint32_t    secdns secondry dns
    {0}, // uint16_t    ipaddrv6 ip address v6     
    {0}, // uint16_t    pridnsv6 primary dns v6
    {0}, // uint16_t    secdnsv6 secondry dns v6
    0, // uint32_t    auth; authentication type
    0, // uint8_t     namesize profile name size
    {0}, // int8_t    name profile name
    0, //uint8_t      apnsize apn size
    {0}, // int8_t    apnname apn name
    0, // uint8_t     usersize username size
    {0}, // int8_t     username username
    0, // uint8_t     password size
    {0}, // int8_t     password
    SWI_UINT256_INT_VALUE // ParamPresenceMask
};


unpack_wds_SLQSGetDataBearerTechnology_t tunpack_wds_SLQSGetDataBearerTechnology = {
    0, // uint8_t    dataBearerMask  bit mask indicates bearer info is for current and/or last call
    {0, 0, 0}, // curDataBearerTechnology current data bearer technology value
    {0, 0, 0},  // lastCallDataBearerTechnology last call data bearer technology value
    SWI_UINT256_INT_VALUE// ParamPresenceMask
};

pack_wds_SLQSDeleteProfile_t tpack_wds_SLQSDeleteProfile = {
    0, // uint8_t    profileType  - profile type
    15  // uint8_t    profileIndex - profile index
};

unpack_wds_SLQSDeleteProfile_t tunpack_wds_SLQSDeleteProfile = {
    0,
    SWI_UINT256_INT_VALUE
};



pack_wds_SLQSSetIPFamilyPreference_t pack_wds_SLQSSetIPFamilyPreference_IPV4 ={
    PACK_WDS_IPV4
};
pack_wds_SLQSSetIPFamilyPreference_t pack_wds_SLQSSetIPFamilyPreference_IPV6 ={
    PACK_WDS_IPV6
};
unpack_wds_SLQSSetIPFamilyPreference_t unpack_wds_SLQSSetIPFamilyPreference_result ={
    0,
    SWI_UINT256_INT_VALUE
};

wdsDhcpv4ProfileId tpack_wds_SLQSSGetDHCPv4ClientConfigProfileId = {0, 1};


pack_wds_SLQSSGetDHCPv4ClientConfig_t tpack_wds_SLQSSGetDHCPv4ClientConfig ={
    &tpack_wds_SLQSSGetDHCPv4ClientConfigProfileId
};

uint32_t StatMask = 0xFF;

pack_wds_GetPacketStatistics_t tpack_wds_GetPacketStatistics ={
    &StatMask
};

pack_wds_SLQSSSetLoopback_t tpack_wds_SLQSSSetLoopback ={0x00,0x04};
unpack_wds_SLQSSSetLoopback_t tunpack_wds_SLQSSSetLoopback = SWI_INIT_UNPACK_RESULT_VALUE;


wdsDhcpv4HwConfig hwConfig;
wdsDhcpv4Option options;
wdsDhcpv4OptionList optionList = {0, &options};

unpack_wds_SLQSSGetDHCPv4ClientConfig_t tunpack_wds_SLQSSGetDHCPv4ClientConfig ={
    &hwConfig,
    &optionList,
    SWI_UINT256_INT_VALUE
};
uint32_t    TXPacketSuccesses;
uint32_t    RXPacketSuccesses;
uint32_t    TXPacketErrors;
uint32_t    RXPacketErrors;
uint32_t    TXPacketOverflows;
uint32_t    RXPacketOverflows;
uint64_t    TXOkBytesCount;
uint64_t    RXOkBytesCount;
uint64_t    TXOKBytesLastCall;
uint64_t    RXOKBytesLastCall;
uint32_t    TXDroppedCount;
uint32_t    RXDroppedCount;

unpack_wds_GetPacketStatistics_t tunpack_wds_GetPacketStatistics ={
        &TXPacketSuccesses,
        &RXPacketSuccesses,
        &TXPacketErrors,
        &RXPacketErrors,
        &TXPacketOverflows,
        &RXPacketOverflows,
        &TXOkBytesCount,
        &RXOkBytesCount,
        &TXOKBytesLastCall,
        &RXOKBytesLastCall,
        &TXDroppedCount,
        &RXDroppedCount,
        SWI_UINT256_INT_VALUE};

uint64_t TXTotalBytes;
uint64_t RXTotalBytes;
unpack_wds_GetByteTotals_t tunpack_wds_GetByteTotals = {
        &TXTotalBytes,
        &RXTotalBytes,
        SWI_UINT256_INT_VALUE
};
unpack_wds_SLQSGetCurrentChannelRate_t tunpack_wds_SLQSGetCurrentChannelRate ={
    0,0,0,0,SWI_UINT256_INT_VALUE
};
unpack_wds_SLQSSGetLoopback_t tunpack_wds_SLQSSGetLoopback ={
    0,0,SWI_UINT256_INT_VALUE
};

uint8_t enableNotification = 1; //enable notification

pack_wds_DHCPv4ClientLeaseChange_t tpack_wds_DHCPv4ClientLeaseChange = {&enableNotification};
unpack_wds_DHCPv4ClientLeaseChange_t tunpack_wds_DHCPv4ClientLeaseChange = SWI_INIT_UNPACK_RESULT_VALUE;

pack_wds_SetMobileIP_t tpack_wds_SetMobileIP ={0x01};
unpack_wds_SetMobileIP_t tunpack_wds_SetMobileIP = SWI_INIT_UNPACK_RESULT_VALUE;

char      SPC[] ="123456";
uint32_t  Mode=0x01;
uint8_t   RetryLimit=0x05;
uint8_t   RetryInterval=0x10;
uint8_t   ReRegPeriod=0x05;
uint8_t   ReRegTraffic=0x01;
uint8_t   HAAuthenticator=0x01;
uint8_t   HA2002bis=0x01;

pack_wds_SetMobileIPParameters_t tpack_wds_SetMobileIPParameters={SPC,&Mode,&RetryLimit,&RetryInterval,&ReRegPeriod,&ReRegTraffic,&HAAuthenticator,&HA2002bis};
unpack_wds_SetMobileIPParameters_t tunpack_wds_SetMobileIPParameters = SWI_INIT_UNPACK_RESULT_VALUE;

pack_wds_SetAutoconnect_t tpack_wds_SetAutoconnect= {0x00,0x00};
unpack_wds_SetAutoconnect_t tunpack_wds_SetAutoconnect = SWI_INIT_UNPACK_RESULT_VALUE;

uint8_t setind=0x01;
wds_TrStatInd transfStatInd = { 0x10, 0x00000080 };

pack_wds_SLQSWdsSetEventReport_t tpack_wds_SetEventReport = {&setind, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
unpack_wds_SLQSWdsSetEventReport_t tunpack_wds_SLQSWdsSetEventReport = SWI_INIT_UNPACK_RESULT_VALUE;

uint32_t setting;
unpack_wds_GetAutoconnect_t tunpack_wds_GetAutoconnect={&setting,SWI_UINT256_INT_VALUE};

wds_DHCPv4ProfileId   profileId = {0,1};

wds_DHCPv4HWConfig    dhcpHwConfig = {1,6,{ 0xA0, 0xB1, 0xC2, 0xD3, 0xE4, 0xF5 }};
wds_DHCPv4Option      optList[3] = {{ 55, 7 , { 0x1C, 0x02, 0x03, 0x0F, 0x06, 0x06, 0x0C, 0x2B }},
    { 60, 16 ,{ 0x30, 0x41, 0x3A, 0x42, 0x30, 0x3A, 0x30, 0x43, 0x3A, 0x3A, 0x54,0x65, 0x73, 0x74, 0x30, 0x31 }},
    { 61, 6 , { 0xA0, 0xF6, 0xB1, 0xE5, 0xC3, 0xD4 }}};

wds_DHCPv4OptionList  requestOptionList ={ 3, optList};


pack_wds_SLQSSSetDHCPv4ClientConfig_t tpack_wds_SLQSSSetDHCPv4ClientConfig = {&profileId, &dhcpHwConfig, &requestOptionList};

unpack_wds_SLQSWdsGoDormant_t tunpack_wds_SLQSWdsGoDormant = SWI_INIT_UNPACK_RESULT_VALUE;
unpack_wds_SLQSWdsGoActive_t tunpack_wds_SLQSWdsGoActive = SWI_INIT_UNPACK_RESULT_VALUE;
unpack_wds_SLQSResetPacketStatics_t tunpack_wds_SLQSResetPacketStatics = SWI_INIT_UNPACK_RESULT_VALUE;
unpack_wds_SLQSSSetDHCPv4ClientConfig_t tunpack_wds_SLQSSSetDHCPv4ClientConfig = SWI_INIT_UNPACK_RESULT_VALUE;
unpack_wds_SetMuxID_t tunpack_wds_SetMuxID = SWI_INIT_UNPACK_RESULT_VALUE;


uint32_t    dataBearer = 0;
unpack_wds_GetDataBearerTechnology_t tunpack_wds_GetDataBearerTechnology = { &dataBearer ,SWI_UINT256_INT_VALUE};

uint8_t            ConnStatusReport     = 0x01;
wds_transferStatInd TransStatus = {0x05, 0xC0};
uint8_t            DormStatusReport     = 0x01;
uint8_t            DataBearerTechReport = 0x01;
uint8_t            ChannelRateReport    = 0x01;

pack_wds_SLQSGetDUNCallInfo_t tpack_wds_SLQSGetDUNCallInfo = { 0x7F, &ConnStatusReport,
        &TransStatus, &DormStatusReport, &DataBearerTechReport, &ChannelRateReport};

unpack_wds_SLQSGetDUNCallInfo_t tunpack_wds_SLQSGetDUNCallInfo ;

uint8_t            ProfileChangeNotification     = 0x01;
pack_wds_SLQSSwiProfileChangeCallback_t tpack_wds_ProfileChangeCallback = { &ProfileChangeNotification };
unpack_wds_SLQSSwiProfileChangeCallback_t tunpack_wds_ProfileChangeCallback = SWI_INIT_UNPACK_RESULT_VALUE;

void dump_null(void* ptr)
{
   UNUSEDPARAM(ptr);
   return ;
}

testitem_t oemapitestwds[] = {


    {
        (pack_func_item) &pack_wds_SetMuxID, "pack_wds_SetMuxID",
        &uMuxID,
        (unpack_func_item) &unpack_wds_SetMuxID, "unpack_wds_SetMuxID",
        &tunpack_wds_SetMuxID, dump_wds_SetMuxID
    },
    {
        (pack_func_item) &pack_wds_SLQSSetWdsEventCallback, "pack_wds_SLQSSetWdsEventCallback",
        &tpack_wds_SLQSSetWdsEventCallback,
        (unpack_func_item) &unpack_wds_SLQSSetWdsEventCallback, "unpack_wds_SLQSSetWdsEventCallback",
        &tunpack_wds_SLQSSetWdsEventCallback, dump_wds_SLQSSetWdsEventCallback
    },
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &packMUXIPFamilyPreference,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_qmap_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_GetSessionState, "pack_wds_GetSessionState",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionState, "unpack_wds_GetSessionState",
        &stateinfo, dump_GetSessionState
    },
    {
        (pack_func_item) &pack_wds_GetSessionDuration, "pack_wds_GetSessionDuration",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionDuration, "unpack_wds_GetSessionDuration",
        &sessionduration, dump_GetSessionDuration
    },
    {
        (pack_func_item) &pack_wds_GetSessionDurationV2, "pack_wds_GetSessionDurationV2",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionDurationV2, "unpack_wds_GetSessionDurationV2",
        &sessiondurationV2, dump_GetSessionDurationV2
    },
    {
        (pack_func_item) &pack_wds_GetPacketStatus, "pack_wds_GetPacketStatus",
        &tpack_wds_GetPacketStatus,
        (unpack_func_item) &unpack_wds_GetPacketStatus, "unpack_wds_GetPacketStatus",
        &tunpack_wds_GetPacketStatus, dump_wds_GetPacketStatus
    },
    {
        (pack_func_item) &pack_wds_GetDormancyState, "pack_wds_GetDormancyState",
        NULL,
        (unpack_func_item) &unpack_wds_GetDormancyState, "unpack_wds_GetDormancyState",
        &dormancystate, dump_GetDormancyState
    },
    {
        (pack_func_item) &pack_wds_SLQSGetDataBearerTechnology, "pack_wds_SLQSGetDataBearerTechnology",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetDataBearerTechnology, "unpack_wds_SLQSGetDataBearerTechnology",
        &tunpack_wds_SLQSGetDataBearerTechnology, dump_wds_SLQSGetDataBearerTechnology
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem ModemSettings",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        &g3GPPconfigitemSettings, dump_SLQSGet3GPPConfigItemModemSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        &get3GPPconfigitem, dump_SLQSGet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSSet3GPPConfigItem, "pack_wds_SLQSSet3GPPConfigItem Change",
        &tpack_wds_SLQSSet3GPPConfigItem,
        (unpack_func_item) &unpack_wds_SLQSSet3GPPConfigItem, "unpack_wds_SLQSSet3GPPConfigItem Change",
        &tunpack_wds_SLQSSet3GPPConfigItem, dump_SLQSSet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem Result",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem Result",
        &get3GPPconfigitem, dump_SLQSGet3GPPConfigItemResult
    },
    {
        (pack_func_item) &pack_wds_SLQSSet3GPPConfigItem, "pack_wds_SLQSSet3GPPConfigItem Restore settings",
        &Default3GPPConfigItemSettings,
        (unpack_func_item) &unpack_wds_SLQSSet3GPPConfigItem, "unpack_wds_SLQSSet3GPPConfigItem Restore settings",
        &tunpack_wds_SLQSSet3GPPConfigItem, dump_SLQSSet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSCreateProfile, "pack_wds_SLQSCreateProfile",
        &tSLQSCreateProfile,
        (unpack_func_item) &unpack_wds_SLQSCreateProfile, "unpack_wds_SLQSCreateProfile",
        &tunpack_create_profile, dump_wds_SLQSCreateProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettings, "pack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettings,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettings, "unpack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettingsRsp, dump_wds_SLQSGetProfileSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettingsV2, "pack_wds_SLQSGetProfileSettingsV2",
        &tSLQSGetProfileSettings,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettingsV2, "unpack_wds_SLQSGetProfileSettingsV2",
        &tSLQSGetProfileSettingsRspV2, dump_wds_SLQSGetProfileSettingsV2
    },
    {
        (pack_func_item) &pack_wds_SLQSModifyProfile, "pack_wds_SLQSModifyProfile",
        &tpack_wds_SLQSModifyProfile,
        (unpack_func_item) &unpack_wds_SLQSModifyProfile, "unpack_wds_SLQSModifyProfile",
        &tunpack_wds_SLQSModifyProfile, dump_wds_SLQSModifyProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSDeleteProfile, "pack_wds_SLQSDeleteProfile",
        &tpack_wds_SLQSDeleteProfile,
        (unpack_func_item) &unpack_wds_SLQSDeleteProfile, "unpack_wds_SLQSDeleteProfile",
        &tunpack_wds_SLQSDeleteProfile, dump_wds_SLQSDeleteProfile
    }
};


#define MAX_QMAP_TEST_CASE 4
testitem_t wdsqmaptest[MAX_QMAP_TEST_CASE] = {
    {
    (pack_func_item) &pack_wds_SetMuxID, "pack_wds_SetMuxID",
    &uMuxID,
    (unpack_func_item) &unpack_wds_SetMuxID, "unpack_wds_SetMuxID",
    &tunpack_wds_SetMuxID, dump_null
    },
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &packMUXIPFamilyPreference,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_qmap_SLQSGetRuntimeSettings
    }
};

testitem_t wdstotest[] = {
    //////Get Modem Settings///////////////////////////////////////////
    {
        (pack_func_item) &pack_wds_GetDefaultProfileNum, "pack_wds_GetDefaultProfileNum",
        &tpack_wds_GetDefaultProfileNum,
        (unpack_func_item) &unpack_wds_GetDefaultProfileNum, "unpack_wds_GetDefaultProfileNum",
        &tunpack_wds_GetDefaultProfileNum, dump_GetDefaultProfileNumSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        &g3GPPconfigitemSettings, dump_SLQSGet3GPPConfigItemSettings
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        &tunpack_wds_GetDefaultProfile, dump_wds_GetDefaultProfileSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetLoopback, "pack_wds_SLQSSGetLoopback",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSSGetLoopback, "unpack_wds_SLQSSGetLoopback",
        &tunpack_wds_SLQSSGetLoopback, dump_wds_SLQSSGetLoopbackSettings
    },
    {
        (pack_func_item) &pack_wds_GetAutoconnect, "pack_wds_GetAutoconnect",
        NULL,
        (unpack_func_item) &unpack_wds_GetAutoconnect, "unpack_wds_GetAutoconnect",
        &tunpack_wds_GetAutoconnect, dump_wds_GetAutoconnectSettings
    },
    ////////////////////////////////////////////////////////////////////
    {
        (pack_func_item) &pack_wds_GetDefaultProfileNum, "pack_wds_GetDefaultProfileNum",
        &tpack_wds_GetDefaultProfileNum,
        (unpack_func_item) &unpack_wds_GetDefaultProfileNum, "unpack_wds_GetDefaultProfileNum",
        &tunpack_wds_GetDefaultProfileNum, dump_GetDefaultProfileNum
    },
    {
        (pack_func_item) &pack_wds_SetDefaultProfileNum, "pack_wds_SetDefaultProfileNum",
        &tpack_wds_SetDefaultProfileNum,
        (unpack_func_item) &unpack_wds_SetDefaultProfileNum, "unpack_wds_SetDefaultProfileNum",
        &tunpack_wds_SetDefaultProfile, dump_SetDefaultProfileNum
    },
    {
        (pack_func_item) &pack_wds_SLQSSet3GPPConfigItem, "pack_wds_SLQSSet3GPPConfigItem",
        &tpack_wds_SLQSSet3GPPConfigItem_9x15,
        (unpack_func_item) &unpack_wds_SLQSSet3GPPConfigItem, "unpack_wds_SLQSSet3GPPConfigItem",
        &tunpack_wds_SLQSSet3GPPConfigItem, dump_SLQSSet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSSet3GPPConfigItem, "pack_wds_SLQSSet3GPPConfigItem",
        &tpack_wds_SLQSSet3GPPConfigItem,
        (unpack_func_item) &unpack_wds_SLQSSet3GPPConfigItem, "unpack_wds_SLQSSet3GPPConfigItem",
        &tunpack_wds_SLQSSet3GPPConfigItem, dump_SLQSSet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        &get3GPPconfigitem, dump_SLQSGet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SLQSSetWdsEventCallback, "pack_wds_SLQSSetWdsEventCallback",
        &tpack_wds_SLQSSetWdsEventCallback,
        (unpack_func_item) &unpack_wds_SLQSSetWdsEventCallback, "unpack_wds_SLQSSetWdsEventCallback",
        &tunpack_wds_SLQSSetWdsEventCallback, dump_wds_SLQSSetWdsEventCallback
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        &tunpack_wds_GetDefaultProfile, dump_wds_GetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfileV2, "pack_wds_GetDefaultProfileV2",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfileV2, "unpack_wds_GetDefaultProfileV2",
        &tunpack_wds_GetDefaultProfileV2, dump_wds_GetDefaultProfileV2
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_GetSessionState, "pack_wds_GetSessionState",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionState, "unpack_wds_GetSessionState",
        &stateinfo, dump_GetSessionState
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_GetSessionDuration, "pack_wds_GetSessionDuration",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionDuration, "unpack_wds_GetSessionDuration",
        &sessionduration, dump_GetSessionDuration
    },
    {
        (pack_func_item) &pack_wds_GetSessionDurationV2, "pack_wds_GetSessionDurationV2",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionDurationV2, "unpack_wds_GetSessionDurationV2",
        &sessiondurationV2, dump_GetSessionDurationV2
    },
    {
        (pack_func_item) &pack_wds_GetPacketStatus, "pack_wds_GetPacketStatus",
        &tpack_wds_GetPacketStatus,
        (unpack_func_item) &unpack_wds_GetPacketStatus, "unpack_wds_GetPacketStatus",
        &tunpack_wds_GetPacketStatus, dump_wds_GetPacketStatus
    },
    {
        (pack_func_item) &pack_wds_GetDormancyState, "pack_wds_GetDormancyState",
        NULL,
        (unpack_func_item) &unpack_wds_GetDormancyState, "unpack_wds_GetDormancyState",
        &dormancystate, dump_GetDormancyState
    },
    {
        (pack_func_item) &pack_wds_SLQSGetDataBearerTechnology, "pack_wds_SLQSGetDataBearerTechnology",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetDataBearerTechnology, "unpack_wds_SLQSGetDataBearerTechnology",
        &tunpack_wds_SLQSGetDataBearerTechnology, dump_wds_SLQSGetDataBearerTechnology
    },
    {
        (pack_func_item) &pack_wds_GetConnectionRate, "pack_wds_GetConnectionRate",
        NULL,
        (unpack_func_item) &unpack_wds_GetConnectionRate, "unpack_wds_GetConnectionRate",
        &connrate, dump_GetConnectionRate
    },
    {
        (pack_func_item) &pack_wds_GetMobileIP, "pack_wds_GetMobileIP",
        NULL,
        (unpack_func_item) &unpack_wds_GetMobileIP, "unpack_wds_GetMobileIP",
        &mipMode, dump_GetMobileIP
    },
    {
        (pack_func_item) &pack_wds_SLQSGetCurrDataSystemStat, "pack_wds_SLQSGetCurrDataSystemStat",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetCurrDataSystemStat, "unpack_wds_SLQSGetCurrDataSystemStat",
        &dataStat, dump_SLQSGetCurrDataSystemStat
    },
    {
        (pack_func_item) &pack_wds_SLQSStopDataSession, "pack_wds_SLQSStopDataSession",
        &tpack_wds_SLQSStopDataSession,
        (unpack_func_item) &unpack_wds_SLQSStopDataSession, "unpack_wds_SLQSStopDataSession",
        &tunpack_wds_SLQSStopDataSession, dump_wds_SLQSStopDataSession
    },
    {
        (pack_func_item) &pack_wds_GetLastMobileIPError, "pack_wds_GetLastMobileIPError",
        NULL,
        (unpack_func_item) &unpack_wds_GetLastMobileIPError, "unpack_wds_GetLastMobileIPError",
        &mobileIPError, dump_GetLastMobileIPError
    },
    {
        (pack_func_item) &pack_wds_SetMobileIPProfile, "pack_wds_SetMobileIPProfile",
        &tpack_wds_SetMobileIPProfile,
        (unpack_func_item) &unpack_wds_SetMobileIPProfile, "unpack_wds_SetMobileIPProfile",
        &tunpack_wds_SetMobileIPProfile, dump_SetMobileIPProfile
    },
    {
        (pack_func_item) &pack_wds_GetMobileIPProfile, "pack_wds_GetMobileIPProfile",
        &tpack_wds_GetMobileIPProfile,
        (unpack_func_item) &unpack_wds_GetMobileIPProfile, "unpack_wds_GetMobileIPProfile",
        &tunpack_wds_GetMobileIPProfile, dump_GetMobileIPProfile
    },
    {
        (pack_func_item) &pack_wds_RMSetTransferStatistics, "pack_wds_RMSetTransferStatistics",
        &tpack_wds_RMSetTransferStatistics,
        (unpack_func_item) &unpack_wds_RMSetTransferStatistics, "unpack_wds_RMSetTransferStatistics",
        &tunpack_wds_RMSetTransferStatistics, dump_RMSetTransferStatistics
    },
    {
        (pack_func_item) &pack_wds_SLQSWdsSwiPDPRuntimeSettings, "pack_wds_SLQSWdsSwiPDPRuntimeSettings",
        &tpack_wds_SLQSWdsSwiPDPRuntimeSettings[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSWdsSwiPDPRuntimeSettings, "unpack_wds_SLQSWdsSwiPDPRuntimeSettings",
        &tunpack_wds_SLQSWdsSwiPDPRuntimeSettings, dump_SLQSWdsSwiPDPRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSCreateProfile, "pack_wds_SLQSCreateProfile",
        &tSLQSCreateProfile,
        (unpack_func_item) &unpack_wds_SLQSCreateProfile, "unpack_wds_SLQSCreateProfile",
        &tunpack_create_profile, dump_wds_SLQSCreateProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettings, "pack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettings,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettings, "unpack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettingsRsp, dump_wds_SLQSGetProfileSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettingsV2, "pack_wds_SLQSGetProfileSettingsV2",
        &tSLQSGetProfileSettings,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettingsV2, "unpack_wds_SLQSGetProfileSettingsV2",
        &tSLQSGetProfileSettingsRspV2, dump_wds_SLQSGetProfileSettingsV2
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettings, "pack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettings_3GPP2,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettings, "unpack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettings3GPP2Rsp, dump_wds_SLQSGetProfileSettings_3GPP2
    },
    {
        (pack_func_item) &pack_wds_SLQSModifyProfile, "pack_wds_SLQSModifyProfile",
        &tpack_wds_SLQSModifyProfile,
        (unpack_func_item) &unpack_wds_SLQSModifyProfile, "unpack_wds_SLQSModifyProfile",
        &tunpack_wds_SLQSModifyProfile, dump_wds_SLQSModifyProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSDeleteProfile, "pack_wds_SLQSDeleteProfile",
        &tpack_wds_SLQSDeleteProfile,
        (unpack_func_item) &unpack_wds_SLQSDeleteProfile, "unpack_wds_SLQSDeleteProfile",
        NULL, dump_wds_SLQSDeleteProfile
    },
    // Profile 1 IPV6
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &pack_wds_SLQSSetIPFamilyPreference_IPV6,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSStopDataSession, "pack_wds_SLQSStopDataSession",
        &tpack_wds_SLQSStopDataSession,
        (unpack_func_item) &unpack_wds_SLQSStopDataSession, "unpack_wds_SLQSStopDataSession",
        &tunpack_wds_SLQSStopDataSession, dump_wds_SLQSStopDataSession
    },
    // Profile 1 IPV4
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &pack_wds_SLQSSetIPFamilyPreference_IPV4,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },

    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSStopDataSession, "pack_wds_SLQSStopDataSession",
        &tpack_wds_SLQSStopDataSession,
        (unpack_func_item) &unpack_wds_SLQSStopDataSession, "unpack_wds_SLQSStopDataSession",
        &tunpack_wds_SLQSStopDataSession, dump_wds_SLQSStopDataSession
    },
    // Profile 3 IPV6
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &pack_wds_SLQSSetIPFamilyPreference_IPV6,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile3Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSStopDataSession, "pack_wds_SLQSStopDataSession",
        &tpack_wds_SLQSStopDataSession,
        (unpack_func_item) &unpack_wds_SLQSStopDataSession, "unpack_wds_SLQSStopDataSession",
        &tunpack_wds_SLQSStopDataSession, dump_wds_SLQSStopDataSession
    },
    // Profile 3 IPV4
    {
        (pack_func_item) &pack_wds_SLQSSetIPFamilyPreference, "pack_wds_SLQSSetIPFamilyPreference",
        &pack_wds_SLQSSetIPFamilyPreference_IPV4,
        (unpack_func_item) &unpack_wds_SLQSSetIPFamilyPreference, "unpack_wds_SLQSSetIPFamilyPreference",
        &unpack_wds_SLQSSetIPFamilyPreference_result, dump_wds_SLQSSetIPFamilyPreference
    },
    {
        (pack_func_item) &pack_wds_SLQSStartDataSession, "pack_wds_SLQSStartDataSession",
        &tpack_wds_SLQSStartDataSession[eProfile3Slot],
        (unpack_func_item) &unpack_wds_SLQSStartDataSession, "unpack_wds_SLQSStartDataSession",
        &tunpack_wds_SLQSStartDataSession, dump_wds_SLQSStartDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        &runtimeinfo, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSStopDataSession, "pack_wds_SLQSStopDataSession",
        &tpack_wds_SLQSStopDataSession,
        (unpack_func_item) &unpack_wds_SLQSStopDataSession, "unpack_wds_SLQSStopDataSession",
        &tunpack_wds_SLQSStopDataSession, dump_wds_SLQSStopDataSession
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetDHCPv4ClientConfig, "pack_wds_SLQSSGetDHCPv4ClientConfig",
        &tpack_wds_SLQSSGetDHCPv4ClientConfig,
        (unpack_func_item) &unpack_wds_SLQSSGetDHCPv4ClientConfig, "unpack_wds_SLQSSGetDHCPv4ClientConfig",
        &tunpack_wds_SLQSSGetDHCPv4ClientConfig, dump_wds_SLQSSGetDHCPv4ClientConfig
    },    
    {
        (pack_func_item) &pack_wds_GetPacketStatistics, "pack_wds_GetPacketStatistics",
        &tpack_wds_GetPacketStatistics,
        (unpack_func_item) &unpack_wds_GetPacketStatistics, "unpack_wds_GetPacketStatistics",
        &tunpack_wds_GetPacketStatistics, dump_wds_GetPacketStatistics
    },
    {
        (pack_func_item) &pack_wds_GetByteTotals, "pack_wds_GetByteTotals",
        NULL,
        (unpack_func_item) &unpack_wds_GetByteTotals, "unpack_wds_GetByteTotals",
        &tunpack_wds_GetByteTotals, dump_wds_GetByteTotals
    },
    {
        (pack_func_item) &pack_wds_SLQSGetCurrentChannelRate, "pack_wds_SLQSGetCurrentChannelRate",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetCurrentChannelRate, "unpack_wds_SLQSGetCurrentChannelRate",
        &tunpack_wds_SLQSGetCurrentChannelRate, dump_wds_SLQSGetCurrentChannelRate
    },
    {
        (pack_func_item) &pack_wds_SLQSSSetLoopback, "pack_wds_SLQSSSetLoopback",
        &tpack_wds_SLQSSSetLoopback,
        (unpack_func_item) &unpack_wds_SLQSSSetLoopback, "unpack_wds_SLQSSSetLoopback",
        &tunpack_wds_SLQSSSetLoopback, dump_wds_SLQSSSetLoopback
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetLoopback, "pack_wds_SLQSSGetLoopback",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSSGetLoopback, "unpack_wds_SLQSSGetLoopback",
        &tunpack_wds_SLQSSGetLoopback, dump_wds_SLQSSGetLoopback
    },
    {
        (pack_func_item) &pack_wds_DHCPv4ClientLeaseChange, "pack_wds_DHCPv4ClientLeaseChange",
        &tpack_wds_DHCPv4ClientLeaseChange,
        (unpack_func_item) &unpack_wds_DHCPv4ClientLeaseChange, "unpack_wds_DHCPv4ClientLeaseChange",
        &tunpack_wds_DHCPv4ClientLeaseChange, dump_wds_DHCPv4ClientLeaseChange
    },
    {
        (pack_func_item) &pack_wds_SetMobileIP, "pack_wds_SetMobileIP",
        &tpack_wds_SetMobileIP,
        (unpack_func_item) &unpack_wds_SetMobileIP, "unpack_wds_SetMobileIP",
        &tunpack_wds_SetMobileIP, dump_wds_SetMobileIP
    },
    {
        (pack_func_item) &pack_wds_SetMobileIPParameters, "pack_wds_SetMobileIPParameters",
        &tpack_wds_SetMobileIPParameters,
        (unpack_func_item) &unpack_wds_SetMobileIPParameters, "unpack_wds_SetMobileIPParameters",
        &tunpack_wds_SetMobileIPParameters, dump_wds_SetMobileIPParameters
    },
    {
        (pack_func_item) &pack_wds_SetAutoconnect, "pack_wds_SetAutoconnect",
        &tpack_wds_SetAutoconnect,
        (unpack_func_item) &unpack_wds_SetAutoconnect, "unpack_wds_SetAutoconnect",
        &tunpack_wds_SetAutoconnect, dump_wds_SetAutoconnect
    },
    {
        (pack_func_item) &pack_wds_GetAutoconnect, "pack_wds_GetAutoconnect",
        NULL,
        (unpack_func_item) &unpack_wds_GetAutoconnect, "unpack_wds_GetAutoconnect",
        &tunpack_wds_GetAutoconnect, dump_wds_GetAutoconnect
    },
    {
        (pack_func_item) &pack_wds_SLQSWdsSetEventReport, "pack_wds_SLQSWdsSetEventReport",
        &tpack_wds_SetEventReport,
        (unpack_func_item) &unpack_wds_SLQSWdsSetEventReport, "unpack_wds_SLQSWdsSetEventReport",
        &tunpack_wds_SLQSWdsSetEventReport, dump_wds_SLQSWdsSetEventReport
    },
    {
        (pack_func_item) &pack_wds_SLQSWdsGoDormant, "pack_wds_SLQSWdsGoDormant",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSWdsGoDormant, "unpack_wds_SLQSWdsGoDormant",
        &tunpack_wds_SLQSWdsGoDormant, dump_wds_SLQSWdsGoDormant
    },
    {
        (pack_func_item) &pack_wds_SLQSWdsGoActive, "pack_wds_SLQSWdsGoActive",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSWdsGoActive, "unpack_wds_SLQSWdsGoActive",
        &tunpack_wds_SLQSWdsGoActive, dump_wds_SLQSWdsGoActive
    },
    {
        (pack_func_item) &pack_wds_SLQSResetPacketStatics, "pack_wds_SLQSResetPacketStatics",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSResetPacketStatics, "unpack_wds_SLQSResetPacketStatics",
        &tunpack_wds_SLQSResetPacketStatics, dump_wds_SLQSResetPacketStatics
    },
    {
        (pack_func_item) &pack_wds_SLQSSSetDHCPv4ClientConfig, "pack_wds_SLQSSSetDHCPv4ClientConfig",
        &tpack_wds_SLQSSSetDHCPv4ClientConfig,
        (unpack_func_item) &unpack_wds_SLQSSSetDHCPv4ClientConfig, "unpack_wds_SLQSSSetDHCPv4ClientConfig",
        &tunpack_wds_SLQSSSetDHCPv4ClientConfig, dump_wds_SLQSSSetDHCPv4ClientConfig
    },
    {
        (pack_func_item) &pack_wds_GetDataBearerTechnology, "pack_wds_GetDataBearerTechnology",
        NULL,
        (unpack_func_item) &unpack_wds_GetDataBearerTechnology, "unpack_wds_GetDataBearerTechnology",
        &tunpack_wds_GetDataBearerTechnology, dump_wds_GetDataBearerTechnology
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        &tunpack_wds_GetDefaultProfile, dump_wds_GetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_SetDefaultProfile, "pack_wds_SetDefaultProfile",
        &tpack_wds_SetDefaultProfile,
        (unpack_func_item) &unpack_wds_SetDefaultProfile, "unpack_wds_SetDefaultProfile",
        &tunpack_wds_SetDefaultProfile, dump_wds_SetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSGetDUNCallInfo, "pack_wds_SLQSGetDUNCallInfo",
        &tpack_wds_SLQSGetDUNCallInfo,
        (unpack_func_item) &unpack_wds_SLQSGetDUNCallInfo, "unpack_wds_SLQSGetDUNCallInfo",
        &tunpack_wds_SLQSGetDUNCallInfo, dump_wds_SLQSGetDUNCallInfo
    },
    //////Restore Modem Settings///////////////////////////////////////////
    {
        (pack_func_item) &pack_wds_SetDefaultProfileNum, "pack_wds_SetDefaultProfileNum Restore settings",
        &DefaultProfileNumSettings,
        (unpack_func_item) &unpack_wds_SetDefaultProfileNum, "unpack_wds_SetDefaultProfileNum Restore settings",
        &tunpack_wds_SetDefaultProfile, dump_SetDefaultProfileNum
    },
    {
        (pack_func_item) &pack_wds_SLQSSet3GPPConfigItem, "pack_wds_SLQSSet3GPPConfigItem Restore settings",
        &Default3GPPConfigItemSettings,
        (unpack_func_item) &unpack_wds_SLQSSet3GPPConfigItem, "unpack_wds_SLQSSet3GPPConfigItem Restore settings",
        &tunpack_wds_SLQSSet3GPPConfigItem, dump_SLQSSet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_SetDefaultProfile, "pack_wds_SetDefaultProfile Restore settings",
        &DefaultProfileSettings,
        (unpack_func_item) &unpack_wds_SetDefaultProfile, "unpack_wds_SetDefaultProfile Restore settings",
        &tunpack_wds_SetDefaultProfile, dump_wds_SetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSSSetLoopback, "pack_wds_SLQSSSetLoopback Restore settings",
        &DefaultLoopbackSettings,
        (unpack_func_item) &unpack_wds_SLQSSSetLoopback, "unpack_wds_SLQSSSetLoopback Restore settings",
        &tunpack_wds_SLQSSSetLoopback, dump_wds_SLQSSSetLoopback
    },
    {
        (pack_func_item) &pack_wds_SetAutoconnect, "pack_wds_SetAutoconnect Restore settings",
        &DefaultAutoconnectSettings,
        (unpack_func_item) &unpack_wds_SetAutoconnect, "unpack_wds_SetAutoconnect Restore settings",
        &tunpack_wds_SetAutoconnect, dump_wds_SetAutoconnect
    },
    {
        (pack_func_item) &pack_wds_SLQSSwiProfileChangeCallback, "pack_wds_SLQSSwiProfileChangeCallback",
        &tpack_wds_ProfileChangeCallback,
        (unpack_func_item) &unpack_wds_SLQSSwiProfileChangeCallback, "unpack_wds_SLQSSwiProfileChangeCallback",
        &tunpack_wds_ProfileChangeCallback, dump_wds_SLQSSwiProfileChangeCallback
    },
    ////////////////////////////////////////////////////////////////////
};

unsigned int wdsarraylen = (unsigned int)((sizeof(wdstotest))/(sizeof(wdstotest[0])));

testitem_t wdstotest_invalidunpack[] = {
    {
        (pack_func_item) &pack_wds_GetDefaultProfileNum, "pack_wds_GetDefaultProfileNum",
        &tpack_wds_GetDefaultProfileNum,
        (unpack_func_item) &unpack_wds_GetDefaultProfileNum, "unpack_wds_GetDefaultProfileNum",
        NULL, dump_GetDefaultProfileNumSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        NULL, dump_SLQSGet3GPPConfigItemSettings
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        NULL, dump_wds_GetDefaultProfileSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetLoopback, "pack_wds_SLQSSGetLoopback",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSSGetLoopback, "unpack_wds_SLQSSGetLoopback",
        NULL, dump_wds_SLQSSGetLoopbackSettings
    },
    {
        (pack_func_item) &pack_wds_GetAutoconnect, "pack_wds_GetAutoconnect",
        NULL,
        (unpack_func_item) &unpack_wds_GetAutoconnect, "unpack_wds_GetAutoconnect",
        NULL, dump_wds_GetAutoconnectSettings
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfileNum, "pack_wds_GetDefaultProfileNum",
        &tpack_wds_GetDefaultProfileNum,
        (unpack_func_item) &unpack_wds_GetDefaultProfileNum, "unpack_wds_GetDefaultProfileNum",
        NULL, dump_GetDefaultProfileNum
    },
    {
        (pack_func_item) &pack_wds_SLQSGet3GPPConfigItem, "pack_wds_SLQSGet3GPPConfigItem",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGet3GPPConfigItem, "unpack_wds_SLQSGet3GPPConfigItem",
        NULL, dump_SLQSGet3GPPConfigItem
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        NULL, dump_wds_GetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_GetSessionState, "pack_wds_GetSessionState",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionState, "unpack_wds_GetSessionState",
        NULL, dump_GetSessionState
    },
    {
        (pack_func_item) &pack_wds_SLQSGetRuntimeSettings, "pack_wds_SLQSGetRuntimeSettings",
        &tpack_wds_SLQSGetRuntimeSettings,
        (unpack_func_item) &unpack_wds_SLQSGetRuntimeSettings, "unpack_wds_SLQSGetRuntimeSettings",
        NULL, dump_wds_SLQSGetRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_GetSessionDuration, "pack_wds_GetSessionDuration",
        NULL,
        (unpack_func_item) &unpack_wds_GetSessionDuration, "unpack_wds_GetSessionDuration",
        NULL, dump_GetSessionDuration
    },
    {
        (pack_func_item) &pack_wds_GetPacketStatus, "pack_wds_GetPacketStatus",
        &tpack_wds_GetPacketStatus,
        (unpack_func_item) &unpack_wds_GetPacketStatus, "unpack_wds_GetPacketStatus",
        NULL, dump_wds_GetPacketStatus
    },
    {
        (pack_func_item) &pack_wds_GetDormancyState, "pack_wds_GetDormancyState",
        NULL,
        (unpack_func_item) &unpack_wds_GetDormancyState, "unpack_wds_GetDormancyState",
        NULL, dump_GetDormancyState
    },
    {
        (pack_func_item) &pack_wds_SLQSGetDataBearerTechnology, "pack_wds_SLQSGetDataBearerTechnology",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetDataBearerTechnology, "unpack_wds_SLQSGetDataBearerTechnology",
        NULL, dump_wds_SLQSGetDataBearerTechnology
    },
    {
        (pack_func_item) &pack_wds_GetConnectionRate, "pack_wds_GetConnectionRate",
        NULL,
        (unpack_func_item) &unpack_wds_GetConnectionRate, "unpack_wds_GetConnectionRate",
        NULL, dump_GetConnectionRate
    },
    {
        (pack_func_item) &pack_wds_GetMobileIP, "pack_wds_GetMobileIP",
        NULL,
        (unpack_func_item) &unpack_wds_GetMobileIP, "unpack_wds_GetMobileIP",
        NULL, dump_GetMobileIP
    },
    {
        (pack_func_item) &pack_wds_SLQSGetCurrDataSystemStat, "pack_wds_SLQSGetCurrDataSystemStat",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetCurrDataSystemStat, "unpack_wds_SLQSGetCurrDataSystemStat",
        NULL, dump_SLQSGetCurrDataSystemStat
    },
    {
        (pack_func_item) &pack_wds_GetLastMobileIPError, "pack_wds_GetLastMobileIPError",
        NULL,
        (unpack_func_item) &unpack_wds_GetLastMobileIPError, "unpack_wds_GetLastMobileIPError",
        NULL, dump_GetLastMobileIPError
    },
    {
        (pack_func_item) &pack_wds_GetMobileIPProfile, "pack_wds_GetMobileIPProfile",
        &tpack_wds_GetMobileIPProfile,
        (unpack_func_item) &unpack_wds_GetMobileIPProfile, "unpack_wds_GetMobileIPProfile",
        NULL, dump_GetMobileIPProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSWdsSwiPDPRuntimeSettings, "pack_wds_SLQSWdsSwiPDPRuntimeSettings",
        &tpack_wds_SLQSWdsSwiPDPRuntimeSettings[eProfile1Slot],
        (unpack_func_item) &unpack_wds_SLQSWdsSwiPDPRuntimeSettings, "unpack_wds_SLQSWdsSwiPDPRuntimeSettings",
        NULL, dump_SLQSWdsSwiPDPRuntimeSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSGetProfileSettings, "pack_wds_SLQSGetProfileSettings",
        &tSLQSGetProfileSettings,
        (unpack_func_item) &unpack_wds_SLQSGetProfileSettings, "unpack_wds_SLQSGetProfileSettings",
        NULL, dump_wds_SLQSGetProfileSettings
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetDHCPv4ClientConfig, "pack_wds_SLQSSGetDHCPv4ClientConfig",
        &tpack_wds_SLQSSGetDHCPv4ClientConfig,
        (unpack_func_item) &unpack_wds_SLQSSGetDHCPv4ClientConfig, "unpack_wds_SLQSSGetDHCPv4ClientConfig",
        NULL, dump_wds_SLQSSGetDHCPv4ClientConfig
    },    
    {
        (pack_func_item) &pack_wds_GetPacketStatistics, "pack_wds_GetPacketStatistics",
        &tpack_wds_GetPacketStatistics,
        (unpack_func_item) &unpack_wds_GetPacketStatistics, "unpack_wds_GetPacketStatistics",
        NULL, dump_wds_GetPacketStatistics
    },
    {
        (pack_func_item) &pack_wds_GetByteTotals, "pack_wds_GetByteTotals",
        NULL,
        (unpack_func_item) &unpack_wds_GetByteTotals, "unpack_wds_GetByteTotals",
        NULL, dump_wds_GetByteTotals
    },
    {
        (pack_func_item) &pack_wds_SLQSGetCurrentChannelRate, "pack_wds_SLQSGetCurrentChannelRate",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSGetCurrentChannelRate, "unpack_wds_SLQSGetCurrentChannelRate",
        NULL, dump_wds_SLQSGetCurrentChannelRate
    },
    {
        (pack_func_item) &pack_wds_SLQSSGetLoopback, "pack_wds_SLQSSGetLoopback",
        NULL,
        (unpack_func_item) &unpack_wds_SLQSSGetLoopback, "unpack_wds_SLQSSGetLoopback",
        NULL, dump_wds_SLQSSGetLoopback
    },
    {
        (pack_func_item) &pack_wds_GetAutoconnect, "pack_wds_GetAutoconnect",
        NULL,
        (unpack_func_item) &unpack_wds_GetAutoconnect, "unpack_wds_GetAutoconnect",
        NULL, dump_wds_GetAutoconnect
    },
    {
        (pack_func_item) &pack_wds_GetDataBearerTechnology, "pack_wds_GetDataBearerTechnology",
        NULL,
        (unpack_func_item) &unpack_wds_GetDataBearerTechnology, "unpack_wds_GetDataBearerTechnology",
        NULL, dump_wds_GetDataBearerTechnology
    },
    {
        (pack_func_item) &pack_wds_GetDefaultProfile, "pack_wds_GetDefaultProfile",
        &tpack_wds_GetDefaultProfile,
        (unpack_func_item) &unpack_wds_GetDefaultProfile, "unpack_wds_GetDefaultProfile",
        NULL, dump_wds_GetDefaultProfile
    },
    {
        (pack_func_item) &pack_wds_SLQSGetDUNCallInfo, "pack_wds_SLQSGetDUNCallInfo",
        &tpack_wds_SLQSGetDUNCallInfo,
        (unpack_func_item) &unpack_wds_SLQSGetDUNCallInfo, "unpack_wds_SLQSGetDUNCallInfo",
        NULL, dump_wds_SLQSGetDUNCallInfo
    },
    {
        (pack_func_item) &pack_wds_SLQSSwiProfileChangeCallback, "pack_wds_SLQSSwiProfileChangeCallback",
        &tpack_wds_ProfileChangeCallback,
        (unpack_func_item) &unpack_wds_SLQSSwiProfileChangeCallback, "unpack_wds_SLQSSwiProfileChangeCallback",
        NULL, dump_wds_SLQSSwiProfileChangeCallback
    },
};


void wds_test_pack_unpack_loop_invalid_unpack()
{
    unsigned i;
    int wds=-1;
    printf("======WDS pack/unpack test with invalid unpack===========\n");
    unsigned xid =1;
    const char *qmi_msg;
    for(i=0; i<sizeof(wdstotest)/sizeof(testitem_t); i++)
    {
        if(wdstotest[i].dump!=NULL)
        {
            wdstotest[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(wdstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        if(wdstotest_invalidunpack[i].dump!=NULL)
        {
            wdstotest_invalidunpack[i].dump(NULL);
        }
    }
    for(i=0; i<sizeof(wdstotest_invalidunpack)/sizeof(testitem_t); i++)
    {
        unpack_qmi_t rsp_ctx;
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t rsp[QMI_MSG_MAX];
        uint8_t req[QMI_MSG_MAX];
        uint16_t rspLen, reqLen;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;
        rtn = run_pack_item(wdstotest_invalidunpack[i].pack)(&req_ctx, req, 
                       &reqLen,wdstotest_invalidunpack[i].pack_ptr);

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }

        if(wds<0)
            wds = client_fd(eWDS);
        if(wds<0)
        {
            fprintf(stderr,"WDS Service Not Supported!\n");
            return ;
        }
        rtn = write(wds, req, reqLen);
        if (rtn!=reqLen)
        {
            printf("write %d wrote %d\n", reqLen, rtn);
            if(wds>=0)
                close(wds);
            wds=-1;
            continue;
        }
        else
        {
            qmi_msg = helper_get_req_str(eWDS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        rtn = read(wds, rsp, QMI_MSG_MAX);
        if(rtn > 0)
        {
            rspLen = (uint16_t ) rtn;
            qmi_msg = helper_get_resp_ctx(eWDS, rsp, rspLen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, rsp);

            if (rsp_ctx.xid == xid)
            {
                unpackRetCode = run_unpack_item(wdstotest_invalidunpack[i].unpack)(rsp, rspLen, 
                                                wdstotest_invalidunpack [i].unpack_ptr);
                if(unpackRetCode!=eQCWWAN_ERR_NONE) {
                    printf("%s: returned %d, unpack failed!\n", 
                           wdstotest_invalidunpack[i].unpack_func_name, unpackRetCode);
                    xid++;
                    continue;
                }
                else
                    wdstotest_invalidunpack[i].dump(wdstotest_invalidunpack[i].unpack_ptr);
            }
        }
        else
        {
            printf("Read Error\n");
            if(wds>=0)
                close(wds);
            wds=-1;
        }
        sleep(1);
        xid++;
    }
    if(wds>=0)
        close(wds);
    wds=-1;
}

void wds_response_handler(uint16_t msgid, uint8_t *msg, uint32_t rlen)
{
switch(msgid)
{
    case eQMI_WDS_SET_EVENT:
        unpackRetCode = unpack_wds_SLQSSetWdsEventCallback(msg, rlen, &tunpack_wds_SLQSSetWdsEventCallback);
        fprintf(stderr,"unpack_wds_SLQSSetWdsEventCallback %d\n", unpackRetCode);
     #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSSetWdsEventCallback",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
     #endif
        dump_wds_SLQSSetWdsEventCallback(&tunpack_wds_SLQSSetWdsEventCallback);
        break;

    case eQMI_WDS_START_NET:
        unpackRetCode = unpack_wds_SLQSStartDataSession(msg, rlen, &tunpack_wds_SLQSStartDataSession);
        fprintf(stderr, "unpack_wds_SLQSStartDataSession %d\n",unpackRetCode);
        fprintf(stderr,"StartDataSessions id %d\n",tunpack_wds_SLQSStartDataSessionsid);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSStartDataSession",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif
        dump_wds_SLQSStartDataSession((void*)&tunpack_wds_SLQSStartDataSession);
        break;
    case eQMI_WDS_SET_QMUX_ID:
        unpackRetCode = unpack_wds_SetMuxID( msg, rlen,&tunpack_wds_SetMuxID);
        fprintf(stderr,"unpack_wds_SetMuxID %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SetMuxID",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif
        dump_wds_SetMuxID(NULL);
        break;
    case eQMI_WDS_SET_IP_FAMILY:
        unpackRetCode = unpack_wds_SLQSSetIPFamilyPreference( msg, rlen,&unpack_wds_SLQSSetIPFamilyPreference_result);
        fprintf(stderr,"unpack_wds_SLQSSetIPFamilyPreference %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSSetIPFamilyPreference",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
     #endif

        dump_wds_SLQSSetIPFamilyPreference((void*)&unpack_wds_SLQSSetIPFamilyPreference_result);
        break;
    case eQMI_WDS_GET_SETTINGS:
        unpackRetCode = unpack_wds_SLQSGetRuntimeSettings( msg, rlen,&runtimeinfo);
        fprintf(stderr,"unpack_wds_SLQSGetRuntimeSettings %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSGetRuntimeSettings",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif


        if(packMUXIPFamilyPreference.IPFamilyPreference==6)
            dump_wds_SLQSGetRuntimeSettingsV6((void*)&runtimeinfo);
        else
            dump_wds_qmap_SLQSGetRuntimeSettings((void*)&runtimeinfo); 
        break;
    case eQMI_WDS_GET_PKT_STATUS:
        unpackRetCode = unpack_wds_GetSessionState(msg, rlen, &stateinfo);
        fprintf(stderr,"unpack_wds_GetSessionState %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_GetSessionState",\
            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_GetSessionState((void*)&stateinfo);
        break;
    case eQMI_WDS_GET_DURATION:
        unpackRetCode = unpack_wds_GetSessionDuration(msg, rlen, &sessionduration);
        fprintf(stderr,"unpack_wds_GetSessionDuration %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_GetSessionDuration",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_GetSessionDuration((void*)&sessionduration);
        break;
    case eQMI_WDS_GET_STATISTICS:
        unpackRetCode = unpack_wds_GetPacketStatus(msg, rlen, &tunpack_wds_GetPacketStatus);
        fprintf(stderr,"unpack_wds_GetPacketStatus %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_GetPacketStatus",\
               ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_GetPacketStatus((void*)&tunpack_wds_GetPacketStatus);
        break;
    case eQMI_WDS_GET_DORMANCY:
        unpackRetCode = unpack_wds_GetDormancyState(msg, rlen, &dormancystate);
        fprintf(stderr,"unpack_wds_GetDormancyState %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_GetDormancyState",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_GetDormancyState((void*)&dormancystate);
        break;
    case eQMI_WDS_GET_CUR_DATA_BEARER:
        unpackRetCode = unpack_wds_SLQSGetDataBearerTechnology(msg, rlen, &tunpack_wds_SLQSGetDataBearerTechnology);
        fprintf(stderr,"unpack_wds_SLQSGetDataBearerTechnology %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSGetDataBearerTechnology",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_SLQSGetDataBearerTechnology((void*)&tunpack_wds_SLQSGetDataBearerTechnology);
        break;
    case eQMI_WDS_SWI_GET_3GPP_CFG_ITEM:
        unpackRetCode = unpack_wds_SLQSGet3GPPConfigItem(msg, rlen, &get3GPPconfigitem);
        fprintf(stderr,"unpack_wds_SLQSGet3GPPConfigItem %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSGet3GPPConfigItem",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_SLQSGet3GPPConfigItem((void*)&get3GPPconfigitem);
        break;
    case eQMI_WDS_SWI_SET_3GPP_CFG_ITEM:
        unpackRetCode = unpack_wds_SLQSSet3GPPConfigItem(msg, rlen,&tunpack_wds_SLQSSet3GPPConfigItem);
        fprintf(stderr,"unpack_wds_SLQSSet3GPPConfigItem %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSSet3GPPConfigItem",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_SLQSSet3GPPConfigItem(NULL);
        break;
    case eQMI_WDS_CREATE_PROFILE:
        unpackRetCode = unpack_wds_SLQSCreateProfile(msg, rlen, &tunpack_create_profile);
        fprintf(stderr,"unpack_wds_SLQSCreateProfile %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSCreateProfile",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_SLQSCreateProfile((void*)&tunpack_create_profile);
        break;
    case eQMI_WDS_GET_PROFILE:
        unpackRetCode = unpack_wds_SLQSGetProfileSettings(msg, rlen, &tSLQSGetProfileSettingsRsp);
        fprintf(stderr,"unpack_wds_SLQSGetProfileSettings %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSGetProfileSettings",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_SLQSGetProfileSettings((void*)&tSLQSGetProfileSettingsRsp);
        break;
    case eQMI_WDS_MODIFY_PROFILE:
        unpackRetCode = unpack_wds_SLQSModifyProfile(msg, rlen, &tunpack_wds_SLQSModifyProfile);
        fprintf(stderr,"unpack_wds_SLQSModifyProfile %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSModifyProfile",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_SLQSModifyProfile((void*)&tunpack_wds_SLQSModifyProfile);
        break;
    case eQMI_WDS_DELETE_PROFILE:
        unpackRetCode = unpack_wds_SLQSDeleteProfile(msg, rlen, &tunpack_wds_SLQSDeleteProfile);
        fprintf(stderr,"unpack_wds_SLQSDeleteProfile %d\n",unpackRetCode);
    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,", "unpack_wds_SLQSDeleteProfile",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        checkErrorAndFormErrorString(msg);
    #endif

        dump_wds_SLQSDeleteProfile(&tunpack_wds_SLQSDeleteProfile);
        break;

    default:
        break;
    }

}

void wds_indication_handler(uint16_t msgid, uint8_t *msg, uint32_t rlen)
{
    unpack_wds_SLQSSetWdsEventCallback_ind_t event;
    unpack_wds_SLQSSetPacketSrvStatusCallback_t stats;
    unpack_RMTransferStatistics_ind_t rmtraferstat;
    unpack_wds_DHCPv4ClientLease_ind_t dhcpclientlease;
    unpack_wds_SLQSDUNCallInfoCallBack_ind_t dunCallInd;
    unpack_wds_SLQSSwiProfileChangeCallback_Ind_t profileChangeCallbackInd;
    uint8_t count;
    wds_DHCPOpt *pOption = NULL;
    uint8_t i,j;

#if DEBUG_LOG_TO_FILE
    int is_matching = 1;
#endif
    memset(&event,0,sizeof(unpack_wds_SLQSSetWdsEventCallback_ind_t));
    memset(&stats,0,sizeof(unpack_wds_SLQSSetPacketSrvStatusCallback_t));

    switch(msgid)
    {
        case eQMI_WDS_SET_EVENT:
            /* provide the PDN number this WDS indication belongs to */
            fprintf(stderr,"WDS INDICATION from Mux ID : 0x%02x\n", uMuxID);

            unpackRetCode = unpack_wds_SLQSSetWdsEventCallback_ind( msg, rlen, &event);
            //it is normal that indication unpack returns eQCWWAN_ERR_INVALID_QMI_RSP(0x11)
            //which simply indicate absense of result code TLV(0x02).
            //Result code TLV is only available on Response
            fprintf(stderr,"WDS SET EVENT Callback Info\n");
            swi_uint256_print_mask (event.ParamPresenceMask);
            if (event.xferStatAvail)
            {
                if(swi_uint256_get_bit (event.ParamPresenceMask, 25))
                {
                    printf("tx_bytes = %"PRIu64"\n", event.tx_bytes);
                }
                if(swi_uint256_get_bit (event.ParamPresenceMask, 26))
                {
                    printf("rx_bytes = %"PRIu64"\n", event.rx_bytes);
                }
                if(swi_uint256_get_bit (event.ParamPresenceMask, 16))
                {
                    printf("tx_pkts = %"PRIu64"\n",  event.tx_pkts);
                }
                if(swi_uint256_get_bit (event.ParamPresenceMask, 17))
                {
                    printf("rx_pkts = %"PRIu64"\n",  event.rx_pkts);
                }
            }
            if ( (event.mipstatAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 27))
            {
                printf("mip status = %d\n", event.mipStatus);
            }
            if ((event.dBTechAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 23))
            {
                printf("Data Bearer technology = %d\n", event.dBTechnology);
            }
            if ( (event.dormancyStatAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 24))
            {
                printf("Dormancy Status = %d\n", event.dormancyStatus);
            }
            if ( (event.currDBTechAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 29))
            {
                printf("RAT Mask = %d\n", event.ratMask);
                printf("SO Mask = %d\n", event.soMask);
            }
            if ( (event.dataSysStatAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 36))
            {
                printf("Preferred Network = %d\n", event.prefNetwork);
                for(count=0;count<event.netInfoLen; count++)
                {
                    printf("Network Type = %d\n", event.currNWInfo[count].NetworkType);
                    printf("RAT Mask = %d\n", event.currNWInfo[count].RATMask);
                    printf("SO Mask = %d\n", event.currNWInfo[count].SOMask);
                }
            }
            if ( (event.dBtechExtAvail) &&
                    swi_uint256_get_bit(event.ParamPresenceMask, 42))
            {
                printf("Technology = %d\n", event.dBtechnologyExt);
                printf("RAT Mask = %d\n", event.dBTechExtRatValue);
                printf("SO Mask = %"PRIu64"\n", event.dBTechExtSoMask);
            }

        #if DEBUG_LOG_TO_FILE
            if ( g_qos_started == 0 )
            {
                if ( event.dataSysStatAvail )
                {
                    if ( event.prefNetwork == preferNetworkType)
                        is_matching = 1;
                    else
                        is_matching = 0;
                }
                else if (event.xferStatAvail)
                {
                    if ((event.tx_bytes== 0) &&
                        (event.rx_bytes== 0)  &&
                        (event.tx_pkts== 0) &&
                        (event.rx_pkts== 0) )
                        is_matching = 0;
                    else
                        is_matching = 1;
                }
                else if (event.mipstatAvail)
                {
                    if ( event.mipStatus != 0)
                        is_matching = 0;
                    else
                        is_matching = 1;
                }
                else if (event.dBTechAvail)
                {
                    if ( event.dBTechnology > 0x14)
                        is_matching = 0;
                    else
                        is_matching = 1;
                }
                else if (event.dormancyStatAvail)
                {
                    if (  event.dormancyStatus > 2)
                        is_matching = 0;
                    else
                        is_matching = 1;
                }
                local_fprintf("%s,%s,", "unpack_wds_SLQSSetWdsEventCallback_ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));

                CHECK_WHITELIST_MASK(
                    unpack_wds_SLQSSetWdsEventCallback_indParamPresenceMaskWhiteList,
                    event.ParamPresenceMask);
    
                local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
            }
        #endif

            break;

        case eQMI_WDS_GET_PKT_STATUS:
            /* provide the PDN number this WDS indication belongs to */
            fprintf(stderr,"WDS INDICATION from Mux ID : 0x%02x\n", uMuxID);
            swi_uint256_print_mask (stats.ParamPresenceMask);
            unpackRetCode = unpack_wds_SLQSSetPacketSrvStatusCallback( msg, rlen, &stats);
            printf("WDS GET PKT STATUS Callback Info\n");
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 1))
            {
                printf("connection status:%d\n", stats.conn_status);
                printf("reconfiguration required:%d\n", stats.reconfigReqd);
            }
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 16))
            {
                printf("Session end reason:%d\n", stats.sessionEndReason);
            }
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 17))
            {
                printf("Verbose session end reason type:%d\n", stats.verboseSessnEndReasonType);
                printf("Verbose session end reason:%d\n", stats.verboseSessnEndReason);
            }
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 18))
            {
                printf("IP family:%d\n", stats.ipFamily);
            }
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 19))
            {
                printf("technology name:%d\n", stats.techName);
            }
            if(swi_uint256_get_bit (stats.ParamPresenceMask, 20))
            {
                printf("bearer id:%d\n", stats.bearerID);
            }
        
        #if DEBUG_LOG_TO_FILE
            if ( g_qos_started == 0 )
            {
                /* expected result, connection status can be some value except 0 & reconfigReqd should be 0 or 1 */
                if (( stats.conn_status == 0) ||
                        ( stats.reconfigReqd >1))
                {
                    is_matching = 0;
                }
                local_fprintf("%s,%s,", "unpack_wds_SLQSSetPacketSrvStatusCallback",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));

                CHECK_WHITELIST_MASK(
                    unpack_wds_SLQSSetPacketSrvStatusCallbackParamPresenceMaskWhiteList,
                    stats.ParamPresenceMask);
                CHECK_MANDATORYLIST_MASK(
                    unpack_wds_SLQSSetPacketSrvStatusCallbackParamPresenceMaskMandatoryList,
                    stats.ParamPresenceMask);
    
                local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
            }
        #endif

            break;

        case eQMI_WDS_SWI_RM_TRANSFER_STATISTICS:
            /* provide the PDN number this WDS indication belongs to */
            printf("WDS INDICATION from Mux ID : 0x%02x\n", uMuxID);

            unpackRetCode = unpack_wds_RMTransferStatistics_ind( msg, rlen, &rmtraferstat);
            fprintf(stderr,"WDS RM TRANSFER STATISTICS Callback Info\n");
            swi_uint256_print_mask(rmtraferstat.ParamPresenceMask);
            if( (rmtraferstat.TxOkConutTlv.TlvPresent) && 
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 16) )
            {
                printf ("TxOkConut: %u\n",rmtraferstat.TxOkConutTlv.ulData );
            }
            if( (rmtraferstat.RxOkConutTlv.TlvPresent) && 
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 17) )
            {
                printf ("RxOkConut: %u\n",rmtraferstat.RxOkConutTlv.ulData );
            }
            if( (rmtraferstat.TxOkByteCountTlv.TlvPresent) &&
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 25) )
            {
                printf ("TxOkByteCount: %"PRIu64"\n",rmtraferstat.TxOkByteCountTlv.ullData );
            }
            if( (rmtraferstat.RxOkByteCountTlv.TlvPresent) &&
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 26) ) 
            {
                printf ("RxOkByteCount: %"PRIu64"\n",rmtraferstat.RxOkByteCountTlv.ullData );
            }
            if( (rmtraferstat.TxDropConutTlv.TlvPresent) && 
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 37) )
            {
                printf ("TxDropConut: %u\n",rmtraferstat.TxDropConutTlv.ulData );
            }
            if( (rmtraferstat.RxDropConutTlv.TlvPresent) &&
                swi_uint256_get_bit (rmtraferstat.ParamPresenceMask, 38))
            {
                printf ("RxDropConut: %u\n",rmtraferstat.RxDropConutTlv.ulData );
            }

        #if DEBUG_LOG_TO_FILE
            if ( g_qos_started == 0 )
            {
                if ( (event.tx_bytes == 0) &&
                    (event.tx_pkts == 0))
                {
                    is_matching = 0;
                }
                else
                {
                    is_matching = 1;
                }
                local_fprintf("%s,%s,", "unpack_wds_RMTransferStatistics_ind",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "N/A"));
            }
        #endif

            break;

        case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND:
            unpackRetCode = unpack_wds_DHCPv4ClientLease_ind( msg, rlen, &dhcpclientlease);
            fprintf(stderr,"WDS DHCPV4 CLIENT LEASE CHANGE Callback Info\n");
            swi_uint256_print_mask(dhcpclientlease.ParamPresenceMask);
            if( (dhcpclientlease.ProfileIdTlv.TlvPresent) &&
                swi_uint256_get_bit(dhcpclientlease.ParamPresenceMask, 1))
            {
                printf ("Profile type: %d\n", dhcpclientlease.ProfileIdTlv.profileType );
                printf ("Profile id: %d\n", dhcpclientlease.ProfileIdTlv.profileId );
            }
            
            if( (dhcpclientlease.DHCPv4LeaseStateTlv.TlvPresent) &&
                swi_uint256_get_bit(dhcpclientlease.ParamPresenceMask, 2))
            {
                printf ("Lease state: %d\n", dhcpclientlease.DHCPv4LeaseStateTlv.leaseState );
            }
            if( (dhcpclientlease.IPv4AddrTlv.TlvPresent) &&
                swi_uint256_get_bit(dhcpclientlease.ParamPresenceMask, 16))
            {

                printf ( "IPv4 address: %x\n", dhcpclientlease.IPv4AddrTlv.IPv4Addr );
            }
            if( (dhcpclientlease.DHCPv4LeaseOptTlv.TlvPresent) &&
                swi_uint256_get_bit(dhcpclientlease.ParamPresenceMask, 17))
            {
                printf ("Num lease options: %d\n", dhcpclientlease.DHCPv4LeaseOptTlv.numOpt );
                printf ("Lease options:\n" );
                for (i=0; i < dhcpclientlease.DHCPv4LeaseOptTlv.numOpt; i++)
                {
                    pOption = dhcpclientlease.DHCPv4LeaseOptTlv.optList + i;
                    printf ( "%2d  ", pOption->optCode);
                    for (j=0; j < pOption->optValLen; j++)
                    {
                        printf ("%02X", *(pOption->pOptVal + j) );
                    }
                    printf ("\n" );
                }
            }

        #if DEBUG_LOG_TO_FILE
            if(dhcpclientlease.ProfileIdTlv.TlvPresent)
            {
                if (dhcpclientlease.ProfileIdTlv.profileId ==0 )
                {
                    is_matching = 0;
                }
            }
            local_fprintf("%s,%s,", "unpack_wds_DHCPv4ClientLease_ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));

            local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
        #endif
            break;
        case eQMI_WDS_GET_DUN_CALL_INFO_IND:            
            memset(&dunCallInd,0,sizeof(unpack_wds_SLQSDUNCallInfoCallBack_ind_t));
            unpackRetCode = unpack_wds_SLQSDUNCallInfoCallBack_ind( msg, rlen, &dunCallInd);
            fprintf(stderr,"WDS DUN CALL INFO indication received with result %d\n", unpackRetCode);
            swi_uint256_print_mask(dunCallInd.ParamPresenceMask);
            if (!unpackRetCode)
            {
                if ( (dunCallInd.CSTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 16) )
                {
                    printf("Connection Status: 0x%x \n",
                        dunCallInd.CSTlv.MDMConnStatus );
                }
                if ( (dunCallInd.LMCERTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 17) )
                {
                    printf("Call End Reason: 0x%x \n",
                        dunCallInd.LMCERTlv.CallEndReason );
                }
                if ( (dunCallInd.TXBOTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 18) )
                {
                    printf("Tx Bytes OK:: %"PRIu64" \n",
                        dunCallInd.TXBOTlv.TxByteOKCnt );
                }
                if ( (dunCallInd.RXBOTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 19) )
                {
                    printf("Rx Bytes OK:: %"PRIu64" \n",
                        dunCallInd.RXBOTlv.RxByteOKCnt );
                }
                if ( (dunCallInd.DSTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 20) )
                {
                    printf("Dormancy Status:: 0x%x \n",
                        dunCallInd.DSTlv.DormancyStat );
                }
                if ( (dunCallInd.DBTTlv.TlvPresent) &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 21) )
                {
                    printf("Data Bearer Technology: 0x%x \n",
                        dunCallInd.DBTTlv.DataBearerTech );
                }
                if ( (dunCallInd.CRTlv.TlvPresent)  &&
                    swi_uint256_get_bit(dunCallInd.ParamPresenceMask, 22) )
                {
                    printf("Current Channel Tx rate: 0x%x \n",
                        dunCallInd.CRTlv.ChannelRate.CurrChanTxRate );
                    printf("Current Channel Rx rate: 0x%x \n",
                        dunCallInd.CRTlv.ChannelRate.CurrChanRxRate );
                }
            }

        #if DEBUG_LOG_TO_FILE
            if ((dunCallInd.TXBOTlv.TxByteOKCnt == 0) ||
                (dunCallInd.RXBOTlv.RxByteOKCnt == 0))
            {
                is_matching = 0;
            }
            local_fprintf("%s,%s,", "unpack_wds_SLQSDUNCallInfoCallBack_ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));

            local_fprintf("%s\n", ((is_matching ==1) ? "Correct": "Wrong"));
        #endif
            break;
        case eQMI_WDS_SWI_PROFILE_CHANGE_IND:
            memset(&profileChangeCallbackInd,0,sizeof(unpack_wds_SLQSSwiProfileChangeCallback_Ind_t));
            unpackRetCode = unpack_wds_SLQSSwiProfileChangeCallback_Ind( msg, rlen, &profileChangeCallbackInd);
            fprintf(stderr,"WDS PROFILE CHANGE INFO indication received with result %d\n", unpackRetCode);
            swi_uint256_print_mask(profileChangeCallbackInd.ParamPresenceMask);
            if (!unpackRetCode)
            {
                if(swi_uint256_get_bit (profileChangeCallbackInd.ParamPresenceMask, 1) )
                   {
                       printf ("Profile Index: 0x%02x\n",profileChangeCallbackInd.ProfileTlv.profileInx );
                       printf ("Profile Type: 0x%02x\n",profileChangeCallbackInd.ProfileTlv.profileType );
                   }
                if( swi_uint256_get_bit (profileChangeCallbackInd.ParamPresenceMask, 16) )
                   {
                       printf ("Profile Change Source: 0x%02x\n",profileChangeCallbackInd.srcTlv.source );
                   }
            }

            #if DEBUG_LOG_TO_FILE
            local_fprintf("%s,%s,", "unpack_wds_SLQSSwiProfileChangeCallback_Ind",\
                ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
        #endif
            break;
        default:
            break;
    }
}

void wds_test_pack_unpack_loop()
{
    unsigned i;
    int wds=-1;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;
    printf("\n======WDS dummy unpack and compare test===========\n");
    wds_validate_dummy_unpack();
    printf("======WDS dummy unpack test===========\n");
    wds_dummy_unpack();
    printf("======WDS pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE
    mkdir("./TestResults/",0777);
    local_fprintf("\n");
    local_fprintf("%s,%s,%s,%s\n", "WDS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    unsigned xid =1;
    i = 54;//57
    for(i=0; i<sizeof(wdstotest)/sizeof(testitem_t); i++)
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
        printf("run %s\n",wdstotest[i].pack_func_name);fflush(stdout);
        rtn = run_pack_item(wdstotest[i].pack)(&req_ctx, req, &reqLen,wdstotest[i].pack_ptr);

    #if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", wdstotest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }
        if(wds<0)
            wds = client_fd(eWDS);
        if(wds<0)
        {
            fprintf(stderr,"WDS Service Not Supported!\n");
            return ;
        }
        rtn = write(wds, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eWDS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
        }

        while (1)
        {
            memset (rsp,0,QMI_MSG_MAX);
            rtn = rspLen = read(wds, rsp, QMI_MSG_MAX);

            if ((rtn>0) && (rspLen > 0) && (rspLen != 0xffff))
            {
                rspLen = (uint16_t) rtn;
                printf("read %d\n", rspLen);
                qmi_msg = helper_get_resp_ctx(eWDS, rsp, rspLen, &rsp_ctx);

                printf("<< receiving %s\n", qmi_msg);
                dump_hex(rspLen, rsp);

                if (rsp_ctx.type == eRSP)
                {
                    printf("WDS RSP: ");
                    printf("msgid 0x%x\n", rsp_ctx.msgid);
                    if (rsp_ctx.xid == xid)
                    {
                        printf("run unpack %d %s\n",xid,wdstotest[i].unpack_func_name);
                        unpackRetCode = run_unpack_item(wdstotest[i].unpack)(rsp, rspLen, wdstotest[i].unpack_ptr);
                        printf("rtn %d\n",unpackRetCode);

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", wdstotest[i].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                         if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                         {
                            eRCode = helper_get_error_code(rsp);
                            reason = helper_get_error_reason(eRCode);
                            sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                        }
                    #endif
                        wdstotest[i].dump(wdstotest[i].unpack_ptr);
                        /* sleep 3 seconds for next write */
                        //sleep(3);
                        usleep(100 * 1e3);
                        /* skip the while loop so that the the next API can be invoked */
                        break;
                    }
                    else
                    {
                        /* the response is not belongs to this API, skip it and read again */
                        printf("xid %d\n",xid);
                        sleep(1);
                        break;
                    }
                }
                else if (rsp_ctx.type == eIND)
                {
                    printf("WDS IND: msgid 0x%x\n", rsp_ctx.msgid);
                    wds_indication_handler(rsp_ctx.msgid, rsp,  rspLen);
                    sleep(1);
                    break;
                }
            }
            else
            {
                /* read nothing, sleep 1 second and read again */
                sleep(1);
            }
        }
        xid++;
    }
    if(wds>=0)
        close(wds);
    wds=-1;
}

void wds_setEthName(char *newName)
{
    strncpy (szEthName,newName,sizeof(szEthName));
    return;
}
void wds_setID(uint8_t newID)
{   
    if((newID-0x80)<MAX_QMAP_INSTANCE)
    uMuxID = newID;
    else
    uMuxID = 0;
    if(g_runoem_demo==1)
    {
        oemapitestwds[0].pack_ptr = &uMuxID;
        oemapitestwds[3].pack_ptr = &tpack_wds_SLQSStartDataSession[uMuxID-0x80];
    }
    else
    {
        wdsqmaptest[0].pack_ptr = &uMuxID;
        wdsqmaptest[2].pack_ptr = &tpack_wds_SLQSStartDataSession[uMuxID-0x80];
    }
}

void wds_setIPFamily(int iIPVersion)
{
    if(iIPVersion==4)
    {
        packMUXIPFamilyPreference.IPFamilyPreference = PACK_WDS_IPV4;
        oemapitestwds[2].pack_ptr = &packMUXIPFamilyPreference;
        oemapitestwds[4].dump = dump_wds_qmap_SLQSGetRuntimeSettings;
        printf("Set IPV4\n");
        uIPv4Count++;
    }
    else if(iIPVersion==6)
    {
        packMUXIPFamilyPreference.IPFamilyPreference = PACK_WDS_IPV6;
        printf("Set IPV6\n");
        oemapitestwds[2].pack_ptr = &packMUXIPFamilyPreference;
        oemapitestwds[4].dump = dump_wds_SLQSGetRuntimeSettingsV6;
    }
}

void setPingAndRouteAddress(wdsConfig *wdsConfigList)
{
    char *pch;
    char tmp[128]={0};
    char tmp1[128]= {0};
    int i;
    if ( wdsConfigList != NULL)
    {
        for ( i = 0; i < MAX_NUMBER_OEM_CONNECTION; i++)
        {
            memset(tmp, 0, sizeof(tmp));
            memset(tmp1, 0, sizeof(tmp1));
            pch = strchr(wdsConfigList->pdnDestIp[i], ',');
            if (pch)
            {
                strncpy(pingTable[i], wdsConfigList->pdnDestIp[i], pch-wdsConfigList->pdnDestIp[i] );
                strcpy(ping6Table[i], pch+1);
            }
            else
            {
                strcpy(pingTable[i], wdsConfigList->pdnDestIp[i]);
            }

            pch = strrchr(wdsConfigList->pdnDestIp[i], '.');
            if (pch)
            {
                memcpy(tmp, wdsConfigList->pdnDestIp[i], (++pch-wdsConfigList->pdnDestIp[i]));
                strcpy(tmp1, tmp);
                strcat(tmp, "0/24");
                strcpy(routeTable[i], tmp);
                strcat(tmp1, "0");
                strcpy(setrouteTable[i][0], tmp1);
            }
            else if (strrchr(wdsConfigList->pdnDestIp[i], ':'))
            {
                
                /* TBD for IPV6 routing handling, so far it seems no need to set routing for IPV6 */
            }
        }
    }
}

void wds_setOemApiTestConfig(wdsConfig *wdsConfigList)
{
    if ( wdsConfigList != NULL)
    {
        setPingAndRouteAddress(wdsConfigList);
        tpack_wds_SLQSStartDataSessionProfileid3gpp[eProfile1Slot] = wdsConfigList->profileId;
        preferNetworkType = wdsConfigList->prefNetworkType;
    }
}

void wds_qmap_pack_unpack_loop()
{
    unsigned i;
    unsigned numoftestcase = 0;
    pid_t pid;
    FILE *pFile;
    char path[20]={0};

#if DEBUG_LOG_TO_FILE
        mkdir("./TestResults/",0777);
        local_fprintf("\n");
        local_fprintf("%s,%s,%s,%s\n", "WDS Pack/UnPack API Name", "Status", "Unpack Payload Parsing","Remark");
#endif

    unsigned xid =1;
    enWdsThread = 1;
    if(wds<0)
    {
        wds = client_fd(eWDS);
    }
    if(wds<0)
    {
        fprintf(stderr,"WDS Service Not Supported!\n");
        return ;
    }
    /* create the pid file in /tmp */
    pid = getpid();
    sprintf(path, "/tmp/%d.txt", pid);
    pFile = fopen(path, "w");
    if (pFile != NULL)
        fclose(pFile);

    pthread_create(&wds_tid, &wds_attr, wds_read_thread, NULL);
    sleep(1);

    if(g_runoem_demo==1)
    {
        numoftestcase = sizeof(oemapitestwds)/sizeof(testitem_t);
        i=0;
        if(g_runqmap_demo==0)
        {
            i=1; //Skip set mux id
        }
    }
    else
    {
        numoftestcase = MAX_QMAP_TEST_CASE;
        i=0;
        if(g_num_of_rmnet_supported==0)
        {
            i=1;
        }
    }
    for(; i<numoftestcase; i++)
    {
        int rtn;
        pack_qmi_t req_ctx;
        uint8_t req[QMI_MSG_MAX];
        uint16_t reqLen;
        const char *qmi_msg;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid;

        if(g_runoem_demo==1)
        {
            printf("%d. run %s\n",i,oemapitestwds[i].pack_func_name);fflush(stdout);
            g_oemApiIndex_wds = i;
            rtn = run_pack_item(oemapitestwds[i].pack)(&req_ctx, req, &reqLen,oemapitestwds[i].pack_ptr);
        }
        else
        {
            printf("%d. run %s\n",i,wdsqmaptest[i].pack_func_name);fflush(stdout);
            rtn = run_pack_item(wdsqmaptest[i].pack)(&req_ctx, req, &reqLen,wdsqmaptest[i].pack_ptr);
        }

    #if DEBUG_LOG_TO_FILE
        if(g_runoem_demo==1)
        {
            local_fprintf("%s,%s,%s\n", oemapitestwds[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
        else
        {
            local_fprintf("%s,%s,%s\n", wdsqmaptest[i].pack_func_name, (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
        }
    #endif

        if(rtn!=eQCWWAN_ERR_NONE)
        {
            printf("pack fail %d \n", rtn);
            continue;
        }
        if(wds<0)
            wds = client_fd(eWDS);
        if(wds<0)
        {
            fprintf(stderr,"WDS Service Not Supported!\n");
            return ;
        }

        rtn = write(wds, req, reqLen);
        if (rtn!=reqLen)
            printf("write %d wrote %d\n", reqLen, rtn);
        else
        {
            qmi_msg = helper_get_req_str(eWDS, req, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, req);
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
    while(enWdsThread)
    {
        /* if the created file was removed, it means that the application wants to stop the data call */
        if ( access(path, F_OK) == -1)
        {
            if(wds>=0)
            {
                close(wds);
            }
            wds = -1;
            exit(0);
        }
        /* test ping every 5 seconds when data session is active */
        sleep(5);
        if(iConnected==1)
        {
            if (g_auto_test_enable)
            {
               if ( ipfamily == IPV4)
                   ping(IPV4, pingTable[uIPv4Count-1],szGWAddress,szEthName);
               else if ( ipfamily == IPV6)
                   ping(IPV6, ping6Table[0],szGWAddress,szEthName);
            }
        }
        fflush(stdout);
        fflush(stderr);

    }
    /* reset the thread flag so that it can be terminated gracefully after timeout */
    enWdsThread = 0;

}

void *wds_read_thread(void* ptr)
{
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    msgbuf msg;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    char *reason=NULL;
    uint16_t rspLen = 0;

    int rtn;
    UNUSEDPARAM(ptr);
    while(enWdsThread)
    {
        //TODO select multiple file and read them
        rspLen = rtn = read(wds, msg.buf, QMI_MSG_MAX);
        if (rtn > 0)
        {
            qmi_msg = helper_get_resp_ctx(eWDS, msg.buf, 255, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rspLen, msg.buf);
            fflush(stdout);

            if(g_runoem_demo == 1)
            {
                if (rsp_ctx.type == eIND)
                {
                    printf("WDS IND: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    wds_indication_handler(rsp_ctx.msgid, msg.buf,  rspLen);
                }
                else
                {
                    printf("WDS RSP: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    if (Sendxid == rsp_ctx.xid)
                    {
                        unpackRetCode = run_unpack_item(oemapitestwds[g_oemApiIndex_wds].unpack)(msg.buf, rspLen, oemapitestwds[g_oemApiIndex_wds].unpack_ptr);
                        #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", oemapitestwds[g_oemApiIndex_wds].unpack_func_name, (unpackRetCode==eQCWWAN_ERR_NONE ? "Success": "Fail"));
                        if ( unpackRetCode!=eQCWWAN_ERR_NONE )
                        {
                            eRCode = helper_get_error_code(msg.buf);
                            reason = helper_get_error_reason(eRCode);
                            sprintf((char*)remark, "Error code:%d reason:%s", eRCode, reason);
                        } 
                        #endif
                        oemapitestwds[g_oemApiIndex_wds].dump(oemapitestwds[g_oemApiIndex_wds].unpack_ptr);
                    }
                }
            }
            else
                {
                if (rsp_ctx.type == eIND)
                {
                    printf("WDS IND: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    wds_indication_handler(rsp_ctx.msgid, msg.buf,  rspLen);
                }
                else if (rsp_ctx.type == eRSP)
                {
                    printf("WDS RSP: ");
                    printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
                    wds_response_handler(rsp_ctx.msgid, msg.buf,  rspLen);
                }
            }
            if ((rsp_ctx.type == eRSP)&&(Sendxid==rsp_ctx.xid))
            {
                ReadyToWriteNext = 1;//Ready to write Next message
            }

        }
        else
        {
           enWdsThread = 0;
           wds = -1;
           printf("enWdsThread exit read error: %d\n",rtn);fflush(stdout);
           break;
        }
    }
    return NULL;
}

void wds_test_ind()
{
    pack_qmi_t req_ctx;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len;
    const char *qmi_msg;

    int rtn;
    if(wds<0)
        wds = client_fd(eWDS);
    if(wds<0)
    {
        fprintf(stderr,"WDS Service Not Supported\n");
        return ;
    }
    pack_wds_SLQSSetWdsEventCallback_t wdsEventsReqParam = {
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2
    };
    req_ctx.xid = 0x100;
    memset(&wds_attr, 0, sizeof(wds_attr));
    rtn = pack_wds_SLQSSetWdsEventCallback(&req_ctx, qmi_req, &qmi_req_len, (void*)&wdsEventsReqParam);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid++;
    uint32_t profileNo = 1;
    pack_wds_SLQSStartDataSession_t SLQSStartDataSessionReq={
            NULL,
            &profileNo,
            NULL,
            NULL,
            NULL,
            NULL };

    enWdsThread = 1;
    pthread_create(&wds_tid, &wds_attr, wds_read_thread, NULL);
    usleep(100);
    rtn = pack_wds_SLQSStartDataSession(&req_ctx, qmi_req, &qmi_req_len, (void*)&SLQSStartDataSessionReq);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(5);
    req_ctx.xid++;
    pack_wds_SLQSStopDataSession_t  SLQSStopDataSessionReq;
    SLQSStopDataSessionReq.psid = &tunpack_wds_SLQSStartDataSessionsid;
    rtn = pack_wds_SLQSStopDataSession(&req_ctx, qmi_req, &qmi_req_len, (void*)&SLQSStopDataSessionReq);

    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    req_ctx.xid++;
    rtn = pack_wds_SLQSSwiProfileChangeCallback(&req_ctx, qmi_req, &qmi_req_len, &tpack_wds_ProfileChangeCallback);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Profile Change Callback Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error \n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    sleep(30);
}

void wds_test_ind_exit()
{
    if(enWdsThread==0)
    {
        if(wds>=0)
            close(wds);
        wds = -1;
    }
    enWdsThread = 0;
    printf("\nkilling WDS read thread...\n");
#ifdef __ARM_EABI__
    if(wds>=0)
        close(wds);
    wds = -1;
    void *pthread_rtn_value;
    if(wds_tid!=0)
    pthread_join(wds_tid, &pthread_rtn_value);
#endif
    if(wds_tid!=0)
    pthread_cancel(wds_tid);
    wds_tid = 0;
    if(wds>=0)
        close(wds);
    wds = -1;
}

void wds_qmap_test_ind()
{
    pack_qmi_t req_ctx;
    uint8_t qmi_req[QMI_MSG_MAX];
    uint16_t qmi_req_len;
    const char *qmi_msg;

    int rtn;
    if(wds<0)
        wds = client_fd(eWDS);
    if(wds<0)
    {
        fprintf(stderr,"WDS Service Not Supported\n");
        return ;
    }

    req_ctx.xid = 0x100;
    memset(&wds_attr, 0, sizeof(wds_attr));

    /* this sample app forks the process when start a data call, for multiple PDN data call, it forks for every PDN,
       as a result, the uMuxID is unique for every PDN, the uMuxID value retrieved from the process is the mux id
       of a specific PDN */
    rtn = pack_wds_SetMuxID(&req_ctx, qmi_req, &qmi_req_len, &uMuxID);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    pack_wds_SLQSSetWdsEventCallback_t wdsEventsReqParam = {
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        5
    };
    req_ctx.xid++;
    memset(&wds_attr, 0, sizeof(wds_attr));
    rtn = pack_wds_SLQSSetWdsEventCallback(&req_ctx, qmi_req, &qmi_req_len, (void*)&wdsEventsReqParam);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        printf("Set Event Report Error\n");
        return ;
    }
    rtn = write(wds, qmi_req, qmi_req_len);
    if(rtn<0)
    {
        printf("Write Error Error\n");
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eWDS, qmi_req, qmi_req_len);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(qmi_req_len, qmi_req);
    }

    enWdsThread = 1;
    pthread_create(&wds_tid, &wds_attr, wds_read_thread, NULL);
    sleep(5);
}

void SetIPAddress(int ipFamily)
{
    char *parentAdaptorName = strstr(szEthName, ":");
    if(parentAdaptorName!=NULL)
    {
        char sztempEthName[255]="";
        int copysize = 0;
        memset(sztempEthName,0,255);
        copysize = strlen(szEthName)-strlen(parentAdaptorName);
        if((copysize<0)||(copysize>=255))
        {
            fprintf(stderr,"unexpteced adaptor name:%s\n",szEthName);
            return;
        }
        strncpy(sztempEthName,szEthName,copysize);
        fprintf(stderr,"Parent Adaptor Name:%s\n",sztempEthName);
        if(iIsAdaptorUp(sztempEthName)==0)
        {
            fprintf(stderr,"Parent Adaptor Name:%s:%s\n",sztempEthName,szIPAddress);
            upAdaptorAddress(sztempEthName);
            sleep(5);
        }
        copysize = strlen(sztempEthName) + strlen("\\") + 1;
        if(copysize>=255)
        {
            fprintf(stderr,"unexpteced adaptor name:%s\n",szEthName);
            return ;
        }
        strncat(sztempEthName,"\\\0",strlen("\\")+1);
        copysize = strlen(sztempEthName) + strlen(parentAdaptorName) + 1;
        if(copysize>=255)
        {
            fprintf(stderr,"unexpteced adaptor name:%s\n",szEthName);
            return ;
        }
        strncat(sztempEthName,parentAdaptorName,strlen(parentAdaptorName));
        if ( ipFamily == 4 )
            setAdaptorAddress(szEthName,szIPAddress,szGWAddress,szNetMask);
        else if ( ipFamily == 6)
        {
            setAdaptorIpv6Address(szEthName, szIPAddress);
            fprintf(stderr, "line:%d ##### szEthName = %s\n", __LINE__, szEthName);
        }
    }
    else
    {
        if ( ipFamily == 4 )
            setAdaptorAddress(szEthName,szIPAddress,szGWAddress,szNetMask);
        else if ( ipFamily == 6)
        {
            setAdaptorIpv6Address(szEthName, szIPAddress);
            fprintf(stderr, "line:%d ##### szEthName = %s\n", __LINE__, szEthName);
        }
    }
}



int PrintIPv4RuntimeSettings(unpack_wds_SLQSGetRuntimeSettings_t *runtime)
{
    struct QmuxIPTable ipTable;
    int retval;
    struct in_addr ip_addr;
    struct in_addr gw_addr;
    struct in_addr subnet;

    if(runtime == NULL)
    {
        printf("runtime NULL\n");
        return 0;
    }
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (runtime->ParamPresenceMask);
    if(!swi_uint256_get_bit(runtime->ParamPresenceMask, 30) ||
       !swi_uint256_get_bit(runtime->ParamPresenceMask, 32) ||
       !swi_uint256_get_bit(runtime->ParamPresenceMask, 33) 
        )
    {
        printf("runtime NO IPv4 Info\n");
        return 0;
    }
    ip_addr.s_addr = htonl(runtime->IPv4);
    gw_addr.s_addr = htonl(runtime->GWAddressV4);
    subnet.s_addr = htonl(runtime->SubnetMaskV4);
    memset(szIPAddress, 0, sizeof(szIPAddress));
    snprintf(szIPAddress,63,"%s",inet_ntoa(ip_addr));
    snprintf(szNetMask,63,"%s",inet_ntoa(subnet));
    snprintf(szGWAddress,63,"%s",inet_ntoa(gw_addr));
    ipfamily = IPV4;


    if(uIPv4Count == 1)
    {
        printf("\nifconfig %s ",szEthName);
        printf("%s ", inet_ntoa(ip_addr));
        printf("netmask %s up\n",inet_ntoa(subnet));
        printf("ip route add %s via %s", routeTable[uIPv4Count-1], inet_ntoa(gw_addr));
        printf(" dev %s\n",szEthName);
    }
    else
    {        
        printf("\nifconfig %s %s ",szEthName, inet_ntoa(ip_addr));
        printf("netmask %s up\n",inet_ntoa(subnet));
    }
    
    if ((uIPv4Count > 1)&&(uIPv4Count <= MAX_QMAP_INSTANCE))
    {
        if(strlen(routeTable[uIPv4Count-1])>0)
        printf("ip route add %s via %s dev %s\n",routeTable[uIPv4Count-1], inet_ntoa(gw_addr),szEthName);
    }

    /* command to set mtu size */
    printf("ifconfig %s mtu %d\n", szEthName, runtime->Mtu);
    if (g_mtu_auto_update_enable && (runtime->Mtu != 0xFFFFFFFF) && (runtime->Mtu != 68)) // minimum mtu size is 68
    {
        setAdaptorMtu(szEthName, runtime->Mtu);
    }

    /* set ip address to driver */
    ipTable.muxID = uMuxID;
    ipTable.ipAddress = runtime->IPv4;
    printf("Mux ID : 0x%02x\n", uMuxID);
    printf("IP Address : 0x%08x\n", ipTable.ipAddress);
    fflush(stdout);
    retval = ioctl(wds, QMI_SET_QMAP_IP_TABLE, (void *)&ipTable);
    if(retval < 0)
    {
        printf("QMI_SET_QMAP_IP_TABLE error!\n");
        return 0;
    }
    if(strstr(szIPAddress,"0.0.0.0") ||
       strstr(szNetMask,"0.0.0.0") ||
       strstr(szGWAddress,"0.0.0.0") )
    {
        fprintf(stderr,"Fail to get IPv4\n");
        return 0;
    }
    else
    {
        if ( g_ip_auto_assign )
        {
            if((uIPv4Count>0)&&(uIPv4Count<=MAX_QMAP_INSTANCE))
            {
                if(strlen(routeTable[uIPv4Count-1])>0)
                {
                    int timeout=0;
                    iConnected = 1;
                    if(GetIPAliasMode()==0)
                    {
                        char szCMD[128]={0};
                        int k = 0;
                        for(k=0;k<NUMBER_OF_DHCLINET_PROGRAMS;k++)
                        {
                            if(checkPath(dhClientBin[k]))
                            {
                                snprintf(szCMD,127,"%s -d -v %s",dhClientBin[k],szEthName);
                                fprintf(stderr,"%s\n",szCMD);
                                SendCMD(main_mqid,szCMD);
                                do
                                {
                                    sleep(1);
                                    timeout++;
                                    if(timeout>60)
                                        break;
                                }
                                while(iIsAdaptorUp(szEthName)==0);
                                k=NUMBER_OF_DHCLINET_PROGRAMS+1;
                                break;
                            }
                        }
                        if(k==NUMBER_OF_DHCLINET_PROGRAMS)
                        {
                            //No dhclient found
                            SetIPAddress(IPV4);
                            /* after setting the ip address, network adapter need sometime to become ready */
                            do
                            {
                                sleep(1);
                                timeout++;
                                if(timeout>60)
                                    break;
                            }
                            while(iIsAdaptorUp(szEthName)==0);
                        }
                    }
                    else
                    {
                        SetIPAddress(IPV4);
                        /* after setting the ip address, network adapter need sometime to become ready */
                        do
                        {
                            sleep(1);
                            timeout++;
                            if(timeout>60)
                                break;
                        }
                        while(iIsAdaptorUp(szEthName)==0);
                    }
                    SetRoute(setrouteTable[uIPv4Count-1][0],
                       setrouteTable[uIPv4Count-1][1],
                        szGWAddress,szEthName);
                    if(strlen(pingTable[uIPv4Count-1])>0 && g_auto_test_enable)
                       ping(IPV4, pingTable[uIPv4Count-1],szGWAddress,szEthName);
                }
            }
        }
    }

#if DEBUG_LOG_TO_FILE
        uint8_t is_matching = 1;

        /* expected result, ipv4 address should not equal to 0 */
        if ((inet_ntoa(ip_addr) == NULL) ||
           (inet_ntoa(gw_addr) == NULL) ||
            (runtime->APNName == NULL ))
        {
            is_matching = 0;
        }
        CHECK_WHITELIST_MASK(
            unpack_wds_SLQSGetRuntimeSettingsParamPresenceMaskWhiteList,
            runtime->ParamPresenceMask);
        CHECK_MANDATORYLIST_MASK(
            unpack_wds_SLQSGetRuntimeSettingsParamPresenceMaskMandatoryList,
            runtime->ParamPresenceMask);
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
    return 1;
}

void wds_setprofile(uint8_t iProfileIndex)
{

    qmap_profile_number = iProfileIndex;
    tpack_wds_SLQSStartDataSession[0].pprofileid3gpp = &qmap_profile_number;
    if(g_runoem_demo==1)
    {
        oemapitestwds[3].pack_ptr = &tpack_wds_SLQSStartDataSession[0];
    }
    else
    {
        wdsqmaptest[2].pack_ptr = &tpack_wds_SLQSStartDataSession[0];
    }
    return;
}

void SetRoute(char *szaddress,char *szMask,char *szGatewayaddress, char *szDevice)
{
    struct rtentry route;
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in sin;
    struct sockaddr_in *addr;
    memset(&route, 0, sizeof(route));
    addr = &sin;
    fprintf(stderr,"%s/%s via %s dev %s\n",szaddress,szMask,szGatewayaddress,szDevice);
    route.rt_dev=szDevice;
    addr = (struct sockaddr_in*) &route.rt_gateway;
    addr = (struct sockaddr_in*)&route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(szaddress);

    addr = (struct sockaddr_in*)&route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(szGatewayaddress);

    addr = (struct sockaddr_in*)&route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(szMask);
    route.rt_flags = RTF_UP | RTF_GATEWAY;//RTF_UP | RTF_HOST | RTF_REJECT;
    route.rt_metric = 0;
    ioctl(fd, SIOCADDRT, &route);
    close(fd);
}

int iIsAdaptorUp(char *szDeviceName)
{
    struct ifreq ifr;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    strncpy(ifr.ifr_name, szDeviceName, IFNAMSIZ);
    ioctl(fd, SIOCGIFFLAGS, &ifr);
    close(fd);
    return (ifr.ifr_flags & (IFF_UP|IFF_RUNNING));
}

void setAdaptorAddress(char *szDeviceName,char *IPAddress,char *GateWay,char *NetMask)
{
    struct ifreq ifr;
    struct rtentry route;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    int result = 0;
    struct sockaddr_in sin;
    struct sockaddr_in *addr;
    addr = &sin;

    memset(&route, 0, sizeof(route));
    strncpy(ifr.ifr_name, szDeviceName, IFNAMSIZ);
    ifr.ifr_addr.sa_family = AF_INET;
    inet_pton(AF_INET, IPAddress, ifr.ifr_addr.sa_data + 2);
    ioctl(fd, SIOCSIFADDR, &ifr);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCSIFADDR Fail\n",szDeviceName);
    }

    inet_pton(AF_INET, NetMask, ifr.ifr_addr.sa_data + 2);
    ioctl(fd, SIOCSIFNETMASK, &ifr);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCSIFNETMASK Fail\n",szDeviceName);
    }
    ioctl(fd, SIOCGIFFLAGS, &ifr);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCGIFFLAGS Fail\n",szDeviceName);
    }
    strncpy(ifr.ifr_name, szDeviceName, IFNAMSIZ);
    
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);

    addr = (struct sockaddr_in*) &route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(GateWay);
    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("0.0.0.0");
    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("0.0.0.0");
    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_metric = 0;
    result = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCSIFFLAGS2 Fail\n",szDeviceName);
    }
    result = ioctl(fd, SIOCADDRT, &route);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCADDRT Fail\n",szDeviceName);
    }
    close(fd);
}

void setAdaptorIpv6Address(char *szDeviceName,char *IPAddress)
{
    struct ifreq ifr;
    struct in6_ifreq ifr6;
    int fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);
    struct sockaddr_in6 sai;

    strncpy(ifr.ifr_ifrn.ifrn_name, szDeviceName, IFNAMSIZ);

    memset(&sai, 0, sizeof(struct sockaddr));
    sai.sin6_family = AF_INET6;
    sai.sin6_port = 0;

    if (inet_pton(AF_INET6, IPAddress, (void *)&sai.sin6_addr) <= 0)
    {
        printf("Bad address\n");
        close(fd);
        return;
    }

    memcpy((char *) &ifr6.ifr6_addr, (char *) &sai.sin6_addr,
               sizeof(struct in6_addr));

    if (ioctl(fd, SIOGIFINDEX, &ifr) < 0)
    {
        perror("SIOGIFINDEX error");
    }

    ifr6.ifr6_ifindex = ifr.ifr_ifru.ifru_ivalue;
    ifr6.ifr6_prefixlen = 64;
    if (ioctl(fd, SIOCSIFADDR, &ifr6) < 0)
    {
        perror("SIOCSIFADDR error");
    }

    /* if the up flag and running flag of the ethernet adapter was set, skip touching it to avoid causing issues   */
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) >= 0)
    {
        if ((ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING))
        {
            close(fd);
            return;
        }
    }

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr))
    {
        perror("SIOCSIFFLAGS error");
    }

    close(fd);

}

void upAdaptorAddress(char *szDeviceName)
{
    struct ifreq ifr;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    strncpy(ifr.ifr_name, szDeviceName, IFNAMSIZ);

    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    ioctl(fd, SIOCSIFFLAGS, &ifr);
    close(fd);
}

void setAdaptorMtu(char *szDeviceName,int mtu)
{
    struct ifreq ifr;

    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    int result = 0;

    strncpy(ifr.ifr_name, szDeviceName, IFNAMSIZ);
    ifr.ifr_mtu = mtu;
    ioctl(fd, SIOCSIFMTU, &ifr);
    if(result!=0)
    {
        fprintf(stderr,"%s SIOCSIFADDR Fail\n",szDeviceName);
    }
    close(fd);
}

int SendCMD(int iMsqid,char *szCMD)
{
   msgbuf buf;
   int length ;
   msgbuf *pbuf = NULL;
   buf.type = 1;
   length = sizeof(msgbuf) - sizeof(long);  
   pbuf = & buf;
   memcpy(buf.buf,szCMD,strlen(szCMD));
   msgsnd(iMsqid,pbuf,length,IPC_NOWAIT);
   return 1;
}

uint8_t dummy_wds_resp_msg[][QMI_MSG_MAX] ={

    //WDS_GET_DEFAULT_PROFILE_NUM
    {0x02, 0x01, 0x00, 0x49, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x09, 0x00, 0x01, 0x01,
    0x00, 0x01},

    //WDS_GET_DEFAULT_PROFILE_NUM
    {0x02, 0x06, 0x00, 0x49, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,},

    //WDS_GET_DEFAULT_PROFILE_NUM
    {0x02, 0x06, 0x00, 0x49, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x01},

    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x14, 0x00, 0x02,
     0x04, 0x00, 0x01, 0x09, 0x00, 0x00, 0x11, 0x0a,
     0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03,
     0x00, 0x04, 0x00 },
     
    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, },

    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x0e, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x04,
     0x00, 0x01, 0x00, 0x01, 0x00,},

    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x01, 0x00, 0x07,},

    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x01, 0x00, 0x01,},

    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x02, 0x00, 0x59, 0x55, 0x14, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x0a,
     0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03,
     0x00, 0x04, 0x00 },
    
    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x0a, 0x00, 0x59, 0x55, 0x23, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x04,
     0x00, 0x01, 0x00, 0x01, 0x00, 0x13, 0x01, 0x00,
     0x07, 0x12, 0x01, 0x00, 0x01, 0x11, 0x0a, 0x00,
     0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00,
     0x04, 0x00},
    
    //WDS_SWI_GET_3GPP_CFG_ITEM
    {0x02, 0x0a, 0x00, 0x59, 0x55, 0x28, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x04,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x13, 0x01, 0x00,
    0x07, 0x12, 0x01, 0x00, 0x01, 0x11, 0x0a, 0x00,
    0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00,
    0x04, 0x00, 0x10, 0x02, 0x00, 0xe1, 0x10},

    //WDS_GET_PKT_STATUS
    {0x02, 0x0e, 0x00, 0x22, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x01, 0x01,
    0x00, 0x01},
    
    //WDS_GET_PKT_STATUS
    {0x02, 0x0e, 0x00, 0x22, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x01},
        
    //WDS_GET_DURATION
    {0x02, 0x10, 0x00, 0x35, 0x00, 0x28, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x01, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x12, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_GET_DURATION
    {0x02, 0x10, 0x00, 0x35, 0x00, 0x28, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,
    0x00, 0xd2, 0x02, 0x96, 0x49, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x12, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_DORMANCY
    {0x02, 0x12, 0x00, 0x30, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x01, 0x01,
    0x00, 0x30},

    //WDS_GET_DORMANCY
    {0x02, 0x12, 0x00, 0x30, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x31},

    //WDS_GET_CUR_DATA_BEARER
    {0x02, 0x13, 0x00, 0x44, 0x00, 0x13, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x01, 0x09,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00},
    
    //WDS_GET_CUR_DATA_BEARER
    {0x02, 0x13, 0x00, 0x44, 0x00, 0x13, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x00},

    //WDS_GET_CUR_DATA_BEARER
    {0x02, 0x13, 0x00, 0x44, 0x00, 0x1F, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x00, 0x10, 0x09, 0x00, 0x04, 0x05, 0x00,
    0x00, 0x00, 0x06, 0x00, 0x00, 0x00 },

    //WDS_GET_RATES
    {0x02, 0x14, 0x00, 0x23, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, },
    
    //WDS_GET_RATES
    {0x02, 0x14, 0x00, 0x23, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00},
    
    //WDS_GET_MIP
    {0x02, 0x15, 0x00, 0x2f, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00, 0x01, 0x01,
    0x00, 0x00},

    //WDS_GET_MIP
    {0x02, 0x15, 0x00, 0x2f, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,},
    
    //WDS_GET_MIP
    {0x02, 0x15, 0x00, 0x2f, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x01},

    //WDS_GET_CURR_DATA_SYS_STAT
    {0x02, 0x16, 0x00, 0x6b, 0x00, 0x07, 0x00, 0x02,
     0x04, 0x00, 0x01, 0x00, 0x19, 0x00,
    },
    
    //WDS_GET_CURR_DATA_SYS_STAT
    {0x02, 0x16, 0x00, 0x6b, 0x00, 0x07, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    
    //WDS_GET_CURR_DATA_SYS_STAT
    {0x02, 0x16, 0x00, 0x6b, 0x00, 0x1e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x14,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_GET_LAST_MIP_STATUS
    {0x02, 0x18, 0x00, 0x42, 0x00, 0x07, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, },

    //WDS_GET_LAST_MIP_STATUS
    {0x02, 0x18, 0x00, 0x42, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00},
    
    //WDS_GET_LAST_MIP_STATUS
    {0x02, 0x18, 0x00, 0x42, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00, 0x01, 0x01,
    0x00, 0x00},
    
    //WDS_SET_MIP_PROFILE
    {0x02, 0x19, 0x00, 0x3f, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00},
    
    //WDS_SET_MIP_PROFILE
    {0x02, 0x19, 0x00, 0x3f, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00},
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x07, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x01, 0x00, 0x01
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x11, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x12, 0x04, 0x00, 0x21, 0x22, 0x23, 0x24,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x13, 0x04, 0x00, 0x31, 0x32, 0x33, 0x34,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x01, 0x00, 0x14,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x1c, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x15, 0x12, 0x00, 0x4d, 0x6f, 0x62, 0x69,
        0x6c, 0x65, 0x49, 0x50, 0x50, 0x72, 0x6f,
        0x66, 0x69, 0x6c, 0x65, 0x4E, 0x41, 0x49,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x16, 0x04, 0x00, 0x61, 0x62, 0x63, 0x64,
    },
    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x17, 0x04, 0x00, 0x71, 0x72, 0x73, 0x74,
    },

    //WDS_GET_MIP_PROFILE
    {0x02, 0x1a, 0x00, 0x3e, 0x00, 0x47, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x01, 0x00, 0x01, 0x11, 0x04, 0x00,
        0x11, 0x12, 0x13, 0x14, 0x12, 0x04, 0x00,
        0x21, 0x22, 0x23, 0x24, 0x13, 0x04, 0x00,
        0x31, 0x32, 0x33, 0x34, 0x14, 0x01, 0x00,
        0x14, 0x15, 0x12, 0x00, 0x4d, 0x6f, 0x62,
        0x69, 0x6c, 0x65, 0x49, 0x50, 0x50, 0x72,
        0x6f, 0x66, 0x69, 0x6c, 0x65, 0x4E, 0x41,
        0x49, 0x16, 0x04, 0x00, 0x61, 0x62, 0x63,
        0x64, 0x17, 0x04, 0x00, 0x71, 0x72, 0x73,
        0x74,
    },
    
    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x02, 0x1b, 0x00, 0x68, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
     0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49},
    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
     0x11, 0x04, 0x00, 0x35, 0x38, 0xd0, 0x8b},

    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x0b, 0x00, 
     0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d, 0xf4, 0x10, 0x22, 0x11},
    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x0b, 0x00, 
     0x1a, 0x08, 0x00, 0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d, 0x20},

    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
     0x25, 0x04, 0x00, 0x35, 0xa4, 0x9a, 0x14},
    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
     0x26, 0x04, 0x00, 0x14, 0x0c, 0x3a, 0x1b},

    //WDS_SWI_RM_TRANSFER_STATISTICS
    {0x04, 0x02, 0x00, 0x68, 0x55, 0x32, 0x00, 
     0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
     0x11, 0x04, 0x00, 0x35, 0x38, 0xd0, 0x8b, 
     0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d, 0xf4, 0x10, 0x22, 0x11,
     0x1a, 0x08, 0x00, 0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d, 0x20,
     0x25, 0x04, 0x00, 0x35, 0xa4, 0x9a, 0x14, 
     0x26, 0x04, 0x00, 0x14, 0x0c, 0x3a, 0x1b},
     
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x01, 0x01
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x01, 0x00, 0x10
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x11, 0x01, 0x00, 0x11
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x13, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x12, 0x09, 0x00 , 0x69, 0x6e, 0x74, 0x65,
        0x72, 0x6e, 0x65, 0x74,0x00
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x13, 0x04, 0x00, 0x31, 0x32, 0x33, 0x34
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x04, 0x00, 0x41, 0x42, 0x43, 0x44
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x15, 0x04, 0x00, 0x51, 0x52, 0x53, 0x54
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x16, 0x04, 0x00, 0x61, 0x62, 0x63, 0x64
    },

    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x17, 0x11, 0x00, 0x11, 0x12, 0x21, 0x22,
        0x31, 0x32, 0x41, 0x42, 0x51, 0x52, 0x61,
        0x62, 0x71, 0x72, 0x81, 0x82,0x10
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x11, 0x00, 0xF1, 0xF2, 0xE1, 0xE2,
        0xD1, 0xD2, 0xC1, 0xC2, 0xB1, 0xB2, 0xA1,
        0xA2, 0x91, 0x92, 0x01, 0x02, 0x01
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x19, 0x10, 0x00, 0x91, 0x92, 0xa1, 0xa2,
        0xb1, 0xb2, 0xc1, 0xc2, 0xd1, 0xd2, 0xe1,
        0xe2, 0xf1, 0xf2, 0x11, 0x12
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1a, 0x10, 0x00, 0xa1, 0xa2, 0xb1, 0xb2,
        0xc1, 0xc2, 0xd1, 0xd2, 0xe1, 0xe2, 0xf1,
        0xf2, 0x11, 0x12, 0x21, 0x22,
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1b, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1c, 0x04, 0x00, 0xb1, 0x68, 0xde, 0x3a,
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1d, 0x10, 0x00, 0xd1, 0xd2, 0xe1, 0xe2, 0xf1,
        0xa1, 0xa2, 0xb1, 0xb2, 0xc1, 0xc2, 0xf2, 0x11,
        0x12, 0x21, 0x22,
    },
    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1e, 0x10, 0x00, 0xe2, 0xf1, 0xa1, 0xa2,
        0xb1, 0xb2, 0xc1, 0xc2, 0xd1, 0xd2, 0xe1,
        0xf2, 0x11, 0x12, 0x21, 0x22,
    },

    //WDS_SWI_PDP_RUNTIME_SETTINGS
    {0x02, 0x1c, 0x00, 0x5b, 0x55, 0xb9, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x10, 0x01, 0x00, 0x10, 0x11, 0x01, 0x00,
        0x11, 0x12, 0x09, 0x00, 0x69, 0x6e, 0x74,
        0x65, 0x72, 0x6e, 0x65, 0x74, 0x00, 0x13,
        0x04, 0x00, 0x31, 0x32, 0x33, 0x34, 0x14,
        0x04, 0x00, 0x41, 0x42, 0x43, 0x44, 0x15,
        0x04, 0x00, 0x51, 0x52, 0x53, 0x54, 0x16,
        0x04, 0x00, 0x61, 0x62, 0x63, 0x64, 0x17,
        0x11, 0x00, 0x11, 0x12, 0x21, 0x22, 0x31,
        0x32, 0x41, 0x42, 0x51, 0x52, 0x61, 0x62,
        0x71, 0x72, 0x81, 0x82,0x10,  0x18, 0x11,
        0x00, 0xF1, 0xF2, 0xE1, 0xE2, 0xD1, 0xD2,
        0xC1, 0xC2, 0xB1, 0xB2, 0xA1, 0xA2, 0x91,
        0x92, 0x01, 0x02, 0x01, 0x19, 0x10, 0x00,
        0x91, 0x92, 0xa1, 0xa2, 0xb1, 0xb2, 0xc1,
        0xc2, 0xd1, 0xd2, 0xe1, 0xe2, 0xf1, 0xf2,
        0x11, 0x12, 0x1a, 0x10, 0x00, 0xa1, 0xa2,
        0xb1, 0xb2, 0xc1, 0xc2, 0xd1, 0xd2, 0xe1,
        0xe2, 0xf1, 0xf2, 0x11, 0x12, 0x21, 0x22,
        0x1b, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
        0x1c, 0x04, 0x00, 0xb1, 0x68, 0xde, 0x3a,
        0x1d, 0x10, 0x00, 0xd1, 0xd2, 0xe1, 0xe2,
        0xf1, 0xa1, 0xa2, 0xb1, 0xb2, 0xc1, 0xc2,
        0xf2, 0x11, 0x12, 0x21, 0x22, 0x1e, 0x10,
        0x00, 0xe2, 0xf1, 0xa1, 0xa2, 0xb1, 0xb2,
        0xc1, 0xc2, 0xd1, 0xd2, 0xe1, 0xf2, 0x11,
        0x12, 0x21, 0x22,
    },
    
    //WDS_SWI_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x58, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x30, 0x00},

    //WDS_SWI_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x58, 0x55, 0x0c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0x11, 0x21},

    //WDS_SWI_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x58, 0x55, 0x0c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0xe0, 0x02, 0x00, 0xe1, 0xe2},

    //WDS_SWI_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x27, 0x00, 0x11, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0x11, 0x21,
     0xe0, 0x02, 0x00, 0xe1, 0xe2},

    //WDS_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x27, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x30, 0x00},

    //WDS_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x27, 0x00, 0x0c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0x11, 0x21},

    //WDS_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x27, 0x00, 0x0c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0xe0, 0x02, 0x00, 0xe1, 0xe2},
    
    //WDS_CREATE_PROFILE
    {0x02, 0x1d, 0x00, 0x27, 0x00, 0x11, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0x11, 0x21,
     0xe0, 0x02, 0x00, 0xe1, 0xe2},

    //WDS_GET_PROFILE
    {0x02, 0x1e, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x51, 0x00, 0xe0, 0x02,
    0x00, 0x05, 0x00},
    
    //WDS_GET_PROFILE
    {0x02, 0x1f, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x51, 0x00, 0xe0, 0x02,
    0x00, 0x05, 0x00},
    
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x19, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x0f, 0x00, 0x69, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65, 0x74,
    0x70 ,0x72 ,0x6F ,0x66 ,0x69 ,0x6C ,0x65 
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x01, 0x00, 0x11,
    },
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x01, 0x00, 0x11,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x12, 0x01, 0x00, 0x12,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x13, 0x01, 0x00, 0x13,
    },
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x15, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x14, 0x0b, 0x00, 0x69, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65, 0x74, 
    0x41, 0x50, 0x4E 
    },
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x15, 0x04, 0x00, 0x15, 0x16, 0x17, 0x18
    },
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x16, 0x04, 0x00, 0x16, 0x17, 0x18, 0x19
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x2b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x17, 0x21, 0x00, 0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20,0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20,0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20,0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20,0x17,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x2b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x21, 0x00, 0x18, 0x19, 0x1a, 0x1b,
    0x18, 0x19, 0x1a, 0x1b,0x18, 0x19, 0x1a, 0x1b,
    0x18, 0x19, 0x1a, 0x1b,0x18, 0x19, 0x1a, 0x1b,
    0x18, 0x19, 0x1a, 0x1b,0x18, 0x19, 0x1a, 0x1b,
    0x18, 0x19, 0x1a, 0x1b,0x18,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x19, 0x14, 0x00, 0x19, 0x1a, 0x1b, 0x1c,
    0x19, 0x1a, 0x1b, 0x1c,0x19, 0x1a, 0x1b, 0x1c,
    0x19, 0x1a, 0x1b, 0x1c,0x19, 0x1a, 0x1b, 0x1c,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1a, 0x14, 0x00, 0x1a, 0x1b, 0x01c, 0x1d,
    0x1a, 0x1b, 0x01c, 0x1d,0x1a, 0x1b, 0x01c, 0x1d,
    0x1a, 0x1b, 0x01c, 0x1d,0x1a, 0x1b, 0x01c, 0x1d,
    },

    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x12, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1b, 0x08, 0x00, 0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65
    },
    //WDS_GET_PROFILE
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x12, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1c, 0x08, 0x00, 0x50, 0x61, 0x73, 0x73, 0x43, 0x6F, 0x64, 0x65 
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1d, 0x01, 0x00, 0x1d
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1e, 0x04, 0x00, 0x1e, 0x1f, 0x20, 0x21
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0x01, 0x00, 0x1f
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x01, 0x00, 0x20
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x21, 0x01, 0x00, 0x21
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x22, 0x01, 0x00, 0x22
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x31, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x23, 0x27, 0x00, 0x01, 0x02, 0x04,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c ,0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x34 ,0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44,
    },
    
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x31, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x23, 0x27, 0x00, 0x01, 0x02, 0x06,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0x51, 0x52, 0x53, 0x54
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x31, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x27, 0x00, 0x01, 0x02, 0x04,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c ,0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x34 ,0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44,
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x31, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x27, 0x00, 0x01, 0x02, 0x06,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0x51, 0x52, 0x53, 0x54
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x25, 0x01, 0x00, 0x25,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x26, 0x01,
    0x00, 0x26,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x27, 0x01, 0x00, 0x27,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x28, 0x10, 0x00, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
    0x35, 0x36, 0x37, 0x38,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x2c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x22,
     0x00, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
     0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
     0x48, 0x49, 0x4a, 0x4b,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x2c, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x22,
     0x00, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
     0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
     0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
     0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
     0x49, 0x4a, 0x4b, 0x4c,
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1a, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x10,
     0x00, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
     0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
     0x3a, 0x3b,
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1a, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x10,
     0x00, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
     0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a,
     0x3b, 0x3c,
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x01,
     0x00, 0x2d,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0x11,
     0x00, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
     0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
     0x3d, 0x3e, 0x3f,
    },
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x01,
     0x00, 0x2f,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x30, 0x04,
    0x00, 0x31, 0x32, 0x33, 0x34,},
    // WDS_GET_PROFILE 3GPP
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x31, 0x01,
    0x00, 0x31,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x90, 0x01,
    0x00, 0x90,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x91, 0x04,
    0x00, 0x91, 0x92, 0x93, 0x94,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x92, 0x04,
    0x00, 0x92, 0x93, 0x94, 0x95,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x93, 0x01,
    0x00, 0x93,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x94, 0x02,
    0x00, 0x94,0x95 },

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x95, 0x02,
    0x00, 0x95, 0x96 },
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x96, 0x02,
    0x00, 0x96, 0x97 },
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x97, 0x01,
    0x00, 0x97,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x98, 0x01,
    0x00, 0x98,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x99, 0x01,
    0x00, 0x99,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x9a, 0x01,
    0x00, 0x9a,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x12, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9b, 0x08,
    0x00, 0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D,
    0x65,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x12, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x08,
    0x00, 0x50, 0x61, 0x73, 0x73, 0x43, 0x6F, 0x64,
    0x65 ,},

    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9d, 0x01,
    0x00, 0x9d,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x04,
    0x00, 0x9e, 0x9f, 0xa0, 0xa1,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x01,
    0x00, 0x9f,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x01,
    0x00, 0xa0,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x14, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa1, 0x0a,
    0x00, 0x41, 0x70, 0x6E, 0x5F, 0x73, 0x74, 0x72,
    0x69, 0x6E, 0x67,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0x01,
    0x00, 0xa2,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0x01,
    0x00, 0xa3,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x04,
    0x00, 0xa4, 0xa5, 0xa6, 0xa7,},

    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x04,
    0x00, 0xa5, 0xa6, 0xa7, 0xa8,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x10,
    0x00, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac,
    0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x10,
    0x00, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad,
    0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
    0xb6, 0xb7,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa8, 0x01,
    0x00, 0xa8,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x01,
    0x00, 0xa9,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0x04,
    0x00, 0xaa, 0xab, 0xac, 0xad,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0x01,
    0x00, 0xab,},
    
    // WDS_GET_PROFILE 3GPP2
    {0x02, 0x21, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02,
    0x00, 0xe0,0xe1},

    //WDS_DELETE_PROFILE
    {0x02, 0x21, 0x00, 0x29, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    //WDS_DELETE_PROFILE
    {0x02, 0x21, 0x00, 0x29, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02,
    0x00, 0x05, 0x00},
    
    //WDS_SET_IP_FAMILY
    {0x02, 0x26, 0x00, 0x4d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_IP_FAMILY
    {0x02, 0x2a, 0x00, 0x4d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x03, 0x00},
    
    //WDS_GET_SETTINGS
    {0x02, 0x0f, 0x00, 0x2d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_GET_SETTINGS
    {0x02, 0x24, 0x00, 0x2d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_GET_SETTINGS
    {0x02, 0x28, 0x00, 0x2d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},
    
    //WDS_GET_SETTINGS
    {0x02, 0x2c, 0x00, 0x2d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    {//WDS_GET_SETTINGS
        0x02,0x04,0x00,0x2d,0x00,0x6c,0x00,0x02,
        0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x0c,
        0x00,0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,
        0x5f,0x70,0x72,0x6f,0x66,0x11,0x01,0x00,
        0x00,0x14,0x08,0x00,0x73,0x6d,0x61,0x72,
        0x74,0x6f,0x6e,0x65,0x15,0x04,0x00,0x85,
        0x17,0x1e,0x0a,0x16,0x04,0x00,0x86,0x17,
        0x1e,0x0a,0x1d,0x01,0x00,0x00,0x1e,0x04,
        0x00,0xf9,0xea,0xa6,0x0a,0x1f,0x02,0x00,
        0x00,0x01,0x20,0x04,0x00,0xfa,0xea,0xa6,
        0x0a,0x21,0x04,0x00,0xfc,0xff,0xff,0xff,
        0x22,0x01,0x00,0x00,0x23,0x01,0x00,0x00,
        0x24,0x01,0x00,0x00,0x29,0x04,0x00,0xdc,
        0x05,0x00,0x00,0x2a,0x01,0x00,0x00,0x2b,
        0x01,0x00,0x04
    },
    
    //WDS_GET_SETTINGS
    {0x02, 0x30, 0x00, 0x2d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_STOP_NET
    {0x02, 0x17, 0x00, 0x21, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x09, 0x00},
    
    //WDS_STOP_NET
    {0x02, 0x25, 0x00, 0x21, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
        
    //WDS_SWI_GET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x32, 0x00, 0x6f, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x47, 0x00},

    //WDS_SWI_GET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x32, 0x00, 0x6f, 0x55, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x04, 0x00, 0x01, 0x02, 0x01, 0x02
    },
    
    //WDS_SWI_GET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x32, 0x00, 0x6f, 0x55, 0x12, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x08, 0x00, 0xA5,0x06,0xfc, 0xca, 0x4c, 0x36, 0x5a, 0xa3
    },

    //WDS_SWI_GET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x32, 0x00, 0x6f, 0x55, 0x2b, 0x00,
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x11, 0x21, 0x00, 0x05, 0x04, 0x04, 0x41,
     0x42, 0x43, 0x44, 0x03, 0x03, 0x31, 0x32,
     0x33, 0x01, 0x01, 0x11, 0x06, 0x06, 0x61,
     0x62, 0x63, 0x64, 0x65, 0x66, 0x08, 0x08,
     0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
     0x88    
    },
    //WDS_SWI_GET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x32, 0x00, 0x6f, 0x55, 0x36, 0x00,
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x11, 0x21, 0x00, 0x05, 0x04, 0x04, 0x41,
     0x42, 0x43, 0x44, 0x03, 0x03, 0x31, 0x32,
     0x33, 0x01, 0x01, 0x11, 0x06, 0x06, 0x61,
     0x62, 0x63, 0x64, 0x65, 0x66, 0x08, 0x08,
     0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
     0x88, 0x10, 0x08, 0x00, 0xA5, 0x06, 0xfc,
     0xca, 0x4c, 0x36, 0x5a, 0xa3
    },
    
    //WDS_GET_STATISTICS
    {0x02, 0x11, 0x00, 0x24, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},
    
    //WDS_GET_STATISTICS
    {0x02, 0x33, 0x00, 0x24, 0x00, 0x1d, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x1b, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_STATISTICS
    {0x02, 0x34, 0x00, 0x24, 0x00, 0x1d, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x08,
    0x00, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x00, 0x1c, 0x08, 0x00, 0x01, 0x23, 0x45, 0x67, 
    0x89, 0xAB, 0xCD, 0xef
    },
    
    //WDS_GET_STATISTICS
    {0x02, 0x33, 0x00, 0x24, 0x00, 0x1d, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x07, 0x00, 
      0x02, 0x04, 0x00, 0x01, 0x00, 0x19, 0x00,
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x07, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x11, 0x04, 0x00, 0x15, 0xcd, 0x5b, 0x07
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x12, 0x04, 0x00, 0xd3, 0x38, 0xfb, 0x0d
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x13, 0x04, 0x00, 0x40, 0xa4, 0x9a, 0x14
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x14, 0x04, 0x00, 0x83, 0x0c, 0x3a, 0x1b
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x15, 0x04, 0x00, 0x22, 0x55, 0xd9, 0x21
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1d, 0x04, 0x00, 0x59, 0x61, 0x77, 0x28
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x0e, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1e, 0x04, 0x00, 0x80, 0x11, 0x09, 0x2f
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x12, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d,
      0xf4, 0x10, 0x22, 0x11,
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x12, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1a, 0x08, 0x00, 0xd2, 0x0a, 0x37, 0x61,
      0x88, 0x86, 0x8d, 0x20
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x12, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1b, 0x08, 0x00, 0x35, 0x6c, 0x56, 0xb8,
      0x4a, 0xfb, 0xf8, 0x2f,
    },
    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x12, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1c, 0x08, 0x00, 0x14, 0x3a, 0xa8, 0x95,
      0xdd, 0x67, 0x64, 0x3f
    },

    //WDS_GET_STATISTICS
    { 0x02, 0x02, 0x00, 0x24, 0x00, 0x6b, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
      0x11, 0x04, 0x00, 0x15, 0xcd, 0x5b, 0x07,
      0x12, 0x04, 0x00, 0xd3, 0x38, 0xfb, 0x0d,
      0x13, 0x04, 0x00, 0x40, 0xa4, 0x9a, 0x14,
      0x14, 0x04, 0x00, 0x83, 0x0c, 0x3a, 0x1b,
      0x15, 0x04, 0x00, 0x22, 0x55, 0xd9, 0x21,
      0x1d, 0x04, 0x00, 0x59, 0x61, 0x77, 0x28,
      0x1e, 0x04, 0x00, 0x80, 0x11, 0x09, 0x2f,
      0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d,
      0xf4, 0x10, 0x22, 0x11, 0x1a, 0x08, 0x00,
      0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d,
      0x20, 0x1b, 0x08, 0x00, 0x35, 0x6c, 0x56,
      0xb8, 0x4a, 0xfb, 0xf8, 0x2f, 0x1c, 0x08,
      0x00, 0x14, 0x3a, 0xa8, 0x95, 0xdd, 0x67,
      0x64, 0x3f
    },
    
    //WDS_SWI_GET_CURRENT_CHANNEL_RATE
    {0x02, 0x35, 0x00, 0x6b, 0x55, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00},

    //WDS_SWI_GET_DATA_LOOPBACK
    {0x02, 0x04, 0x00, 0x69, 0x55, 0x0f, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x01,
    0x00, 0x06, 0x10, 0x01, 0x00, 0x00},
    
    //WDS_SWI_GET_DATA_LOOPBACK
    {0x02, 0x37, 0x00, 0x69, 0x55, 0x0f, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x01,
    0x00, 0x06, 0x10, 0x01, 0x00, 0x00},
    
    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE
    {0x02, 0x38, 0x00, 0x71, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x47, 0x00},

    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE
    {0x02, 0x38, 0x00, 0x71, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_MIP
    {0x02, 0x39, 0x00, 0x2e, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00},

    //WDS_SET_MIP
    {0x02, 0x39, 0x00, 0x2e, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_MIP_PARAMS
    {0x02, 0x3a, 0x00, 0x41, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x19, 0x00},

    //WDS_SET_MIP_PARAMS
    {0x02, 0x3a, 0x00, 0x41, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_GET_AUTOCONNECT
    {0x02, 0x05, 0x00, 0x34, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00},
    
    //WDS_GET_AUTOCONNECT
    {0x02, 0x3c, 0x00, 0x34, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00},
    
    //WDS_G0_DORMANT
    {0x02, 0x3e, 0x00, 0x25, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_G0_DORMANT
    {0x02, 0x3e, 0x00, 0x25, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_G0_ACTIVE
    {0x02, 0x3f, 0x00, 0x26, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_G0_ACTIVE
    {0x02, 0x3f, 0x00, 0x26, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_RESET_PKT_STATISTICS
    {0x02, 0x40, 0x00, 0x86, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0f, 0x00},

    //WDS_RESET_PKT_STATISTICS
    {0x02, 0x40, 0x00, 0x86, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SWI_SET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x41, 0x00, 0x70, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x47, 0x00},

    //WDS_SWI_SET_DHCPV4_CLIENT_CONFIG
    {0x02, 0x41, 0x00, 0x70, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_DATA_BEARER
    {0x02, 0x42, 0x00, 0x37, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x47, 0x00},

    //WDS_GET_DATA_BEARER
    {0x02, 0x42, 0x00, 0x37, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_GET_DATA_BEARER
    {0x02, 0x42, 0x00, 0x37, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  0x01, 0x01,
    0x00, 0x5a},
    
    //WDS_GET_DEFAULTS
    {0x02, 0x03, 0x00, 0x2c, 0x00, 0x33, 0x01, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x01,
    0x00, 0x01, 0xdf, 0x01, 0x00, 0x00, 0xde, 0x01,
    0x00, 0x00, 0x48, 0x01, 0x00, 0x00, 0x47, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x04, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x45, 0x28, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
    0x01, 0x00, 0x01, 0x43, 0x01, 0x00, 0x01, 0x42,
    0x04, 0x00, 0xff, 0x00, 0x00, 0x00, 0x41, 0x04,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00,
    0x00, 0x3f, 0x01, 0x00, 0x00, 0x3e, 0x01, 0x00,
    0x00, 0x3d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x02, 0x00, 0x00, 0x00, 0x3b, 0x02, 0x00,
    0x2c, 0x01, 0x3a, 0x02, 0x00, 0x14, 0x00, 0x39,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x38, 0x02, 0x00,
    0x00, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x36,
    0x01, 0x00, 0x00, 0x35, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0x01,
    0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x2f, 0x01, 0x00, 0x00, 0x2d, 0x01, 0x00,
    0x00, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00,
    0x26, 0x01, 0x00, 0x00, 0x25, 0x01, 0x00, 0x01,
    0x22, 0x01, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00,
    0x20, 0x01, 0x00, 0x00, 0x1f, 0x01, 0x00, 0x00,
    0x1e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x1b, 0x00,
    0x00, 0x16, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x15, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x09, 0x00, 0x62, 0x72, 0x6f, 0x61, 0x64, 0x62,
    0x61, 0x6e, 0x64, 0x13, 0x01, 0x00, 0x00, 0x12,
    0x01, 0x00, 0x00, 0x11, 0x01, 0x00, 0x03, 0x10,
    0x00, 0x00},
    
    //WDS_GET_DEFAULTS
    {0x02, 0x0c, 0x00, 0x2c, 0x00, 0x33, 0x01, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x01,
    0x00, 0x01, 0xdf, 0x01, 0x00, 0x00, 0xde, 0x01,
    0x00, 0x00, 0x48, 0x01, 0x00, 0x00, 0x47, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x04, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x45, 0x28, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
    0x01, 0x00, 0x01, 0x43, 0x01, 0x00, 0x01, 0x42,
    0x04, 0x00, 0xff, 0x00, 0x00, 0x00, 0x41, 0x04,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00,
    0x00, 0x3f, 0x01, 0x00, 0x00, 0x3e, 0x01, 0x00,
    0x00, 0x3d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x02, 0x00, 0x00, 0x00, 0x3b, 0x02, 0x00,
    0x2c, 0x01, 0x3a, 0x02, 0x00, 0x14, 0x00, 0x39,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x38, 0x02, 0x00,
    0x00, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x36,
    0x01, 0x00, 0x00, 0x35, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0x01,
    0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x2f, 0x01, 0x00, 0x00, 0x2d, 0x01, 0x00,
    0x00, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00,
    0x26, 0x01, 0x00, 0x00, 0x25, 0x01, 0x00, 0x01,
    0x22, 0x01, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00,
    0x20, 0x01, 0x00, 0x00, 0x1f, 0x01, 0x00, 0x00,
    0x1e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x1b, 0x00,
    0x00, 0x16, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x15, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x09, 0x00, 0x62, 0x72, 0x6f, 0x61, 0x64, 0x62,
    0x61, 0x6e, 0x64, 0x13, 0x01, 0x00, 0x00, 0x12,
    0x01, 0x00, 0x00, 0x11, 0x01, 0x00, 0x03, 0x10,
    0x00, 0x00},
    
    //WDS_GET_DEFAULTS
    {0x02, 0x43, 0x00, 0x2c, 0x00, 0x33, 0x01, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x01,
    0x00, 0x01, 0xdf, 0x01, 0x00, 0x00, 0xde, 0x01,
    0x00, 0x00, 0x48, 0x01, 0x00, 0x00, 0x47, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x04, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x45, 0x28, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
    0x01, 0x00, 0x01, 0x43, 0x01, 0x00, 0x01, 0x42,
    0x04, 0x00, 0xff, 0x00, 0x00, 0x00, 0x41, 0x04,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00,
    0x00, 0x3f, 0x01, 0x00, 0x00, 0x3e, 0x01, 0x00,
    0x00, 0x3d, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x02, 0x00, 0x00, 0x00, 0x3b, 0x02, 0x00,
    0x2c, 0x01, 0x3a, 0x02, 0x00, 0x14, 0x00, 0x39,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x38, 0x02, 0x00,
    0x00, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x36,
    0x01, 0x00, 0x00, 0x35, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0x01,
    0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x2f, 0x01, 0x00, 0x00, 0x2d, 0x01, 0x00,
    0x00, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00,
    0x26, 0x01, 0x00, 0x00, 0x25, 0x01, 0x00, 0x01,
    0x22, 0x01, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00,
    0x20, 0x01, 0x00, 0x00, 0x1f, 0x01, 0x00, 0x00,
    0x1e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x1b, 0x00,
    0x00, 0x16, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x15, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x09, 0x00, 0x62, 0x72, 0x6f, 0x61, 0x64, 0x62,
    0x61, 0x6e, 0x64, 0x13, 0x01, 0x00, 0x00, 0x12,
    0x01, 0x00, 0x00, 0x11, 0x01, 0x00, 0x03, 0x10,
    0x00, 0x00},
    
    //WDS_GET_DUN_CALL_INFO
    {0x02, 0x45, 0x00, 0x38, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x47, 0x00},

    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x13, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x09, 0x00, 0X01, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17, 0x18
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x0e, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x11, 0x04, 0x00, 0x01, 0X02, 0x03, 0x04 
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x12, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x12, 0x08, 0x00, 0x01, 0X02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x12, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x13, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,
        0x40, 0x30, 0x20, 0x10 
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x01, 0x00, 0xA5
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x15, 0x01, 0x00, 0x5A
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x16, 0x10, 0x00, 0x01, 0X02, 0x03, 0x04,
        0x11, 0x12, 0x13, 0x14, 0x21, 0x22, 0x23,
        0x24, 0x31, 0x32, 0x33, 0x34
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x17, 0x08, 0x00, 0x81, 0X72, 0x63, 0x54,
        0x45, 0x36, 0x27, 0x18
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,
        0x40, 0x30, 0x20, 0x10
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x1a, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x19, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,
        0x40, 0x30, 0x20, 0x10
    },
    //WDS_GET_DUN_CALL_INFO
    {
        0x02, 0x45, 0x00, 0x38, 0x00, 0x0b, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x01, 0x00, 0xFF
    },
    
    //WDS_SET_DEFAULT_PROFILE_NUM
    {0x02, 0x07, 0x00, 0x4a, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_DEFAULT_PROFILE_NUM
    {0x02, 0x46, 0x00, 0x4a, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_DEFAULT_PROFILE_NUM
    {0x02, 0x07, 0x00, 0x4a, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_DEFAULT_PROFILE_NUM
    {0x02, 0x46, 0x00, 0x4a, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x09, 0x00},

    //WDS_SWI_SET_3GPP_CFG_ITEM
    {0x02, 0x08, 0x00, 0x5a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x30, 0x00},
    
    //WDS_SWI_SET_3GPP_CFG_ITEM
    {0x02, 0x09, 0x00, 0x5a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SWI_SET_3GPP_CFG_ITEM
    {0x02, 0x47, 0x00, 0x5a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x30, 0x00},

    //WDS_MODIFY_PROFILE
    {0x02, 0x20, 0x00, 0x28, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x51, 0x00, 0xe0, 0x02,
    0x00, 0x05, 0x00},
    
    //WDS_MODIFY_PROFILE
    {0x02, 0x44, 0x00, 0x28, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_MODIFY_PROFILE
    {0x02, 0x48, 0x00, 0x28, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_MODIFY_PROFILE
    {0x02, 0x48, 0x00, 0x28, 0x00, 0x0e, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0xE0, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04 },

    //WDS_SWI_SET_DATA_LOOPBACK
    {0x02, 0x36, 0x00, 0x6a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SWI_SET_DATA_LOOPBACK
    {0x02, 0x49, 0x00, 0x6a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_AUTOCONNECT
    {0x02, 0x3b, 0x00, 0x51, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_SET_AUTOCONNECT
    {0x02, 0x4a, 0x00, 0x51, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_SET_EVENT
    {0x02, 0x0b, 0x00, 0x01, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    //WDS_SET_EVENT
    {0x02, 0x00, 0x01, 0x01, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_SET_EVENT
    {0x02, 0x3d, 0x00, 0x01, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    //WDS_START_NET
    {0x02, 0x0d, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},
    
    //WDS_START_NET
    {0x02, 0x23, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},
    
    //WDS_START_NET
    {0x02, 0x27, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},
    //WDS_START_NET
    {0x02, 0x2b, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},
    //WDS_START_NET
    {0x02, 0x2f, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},
    
    //WDS_START_NET
    {0x02, 0x01, 0x01, 0x20, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
    0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
    0x07},    
    { //WDS_START_NET
      0x02,0x03,0x00,0x20,0x00,0x0e,0x00,0x02,0x04,
      0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x00,0x40,
      0x4f,0x97,0x28
    },
    { //WDS_PKT_STATUS_IND
      0x04,0x01,0x00,0x22,0x00,0x16,0x00,0x01,0x02,
      0x00,0x02,0x00,0x12,0x01,0x00,0x04,0x13,0x02,
      0x00,0x80,0x88,0x14,0x01,0x00,0x05,0x15,0x01,
      0x00,0x00
    },
     { //WDS_PKT_STATUS_IND
        0x04,0x01,0x00,0x22,0x00,0x05,0x00,
        0x01,0x02,0x00,0x02,0x01
    },
    
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x04, 0x00, 
      0x17, 0x01, 0x00, 0x71 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x04, 0x00, 
      0x18, 0x01, 0x00, 0x81 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x04, 0x00, 
      0x1B, 0x01, 0x00, 0x91 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x07, 0x00, 
      0x10, 0x04, 0x00, 0x10, 0x11, 0x12, 0x13 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x07, 0x00, 
      0x11, 0x04, 0x00, 0x10, 0x11, 0x12, 0x13 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x0b, 0x00, 
      0x19, 0x08, 0x00, 0x10, 0x11, 0x12, 0x13,
      0x20, 0x21, 0x22, 0x23 },
    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x0b, 0x00, 
      0x1A, 0x08, 0x00, 0x10, 0x11, 0x12, 0x13,
      0x30, 0x31, 0x32, 0x33 },

    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x0c, 0x00, 
      0x1D, 0x09, 0x00, 0x5a, 0x10, 0x11, 0x12,
      0x13, 0x30, 0x31, 0x32, 0x33 },

    //WDS_EVENT_IND
    { 0x04, 0x00, 0x01, 0x01, 0x00, 0x13, 0x00, 
      0x2a, 0x10, 0x00, 0x01, 0x01, 0x01, 0x01,
      0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
      0x00, 0x01, 0x01, 0x01, 0x01
    },
    //WDS_SET_QMUX_ID
    {0x02, 0x07, 0x00, 0xa2, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x09, 0x00},

    //WDS_SET_QMUX_ID
    {0x02, 0x07, 0x00, 0xa2, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x01, 0x38, 0x00, 0x04, 0x00, 
      0x10, 0x01, 0x00, 0x10,
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x05, 0x00, 
      0x11, 0x02, 0x00, 0x11, 0x12
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x0b, 0x00, 
      0x12, 0x08, 0x00, 0x21, 0x22, 0x23, 0x24,
      0x25, 0x26, 0x27, 0x28
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x0b, 0x00, 
      0x13, 0x08, 0x00, 0x31, 0x32, 0x33, 0x34,
      0x35, 0x36, 0x37, 0x38
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x04, 0x00, 
      0x14, 0x01, 0x00, 0x14
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x04, 0x00, 
      0x15, 0x01, 0x00, 0x15
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x02, 0x38, 0x00, 0x0b, 0x00, 
      0x16, 0x08, 0x00, 0x61, 0x62, 0x63, 0x64,
      0x16, 0x26, 0x36, 0x46
    },
    //WDS_GET_DUN_CALL_INFO_IND 
    { 0x04, 0x00, 0x01, 0x38, 0x00, 0x32, 0x00, 
      0x10, 0x01, 0x00, 0x10, 0x11, 0x02, 0x00,
      0x11, 0x12, 0x12, 0x08, 0x00, 0x21, 0x22,
      0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x13,
      0x08, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35,
      0x36, 0x37, 0x38, 0x14, 0x01, 0x00, 0x14,
      0x15, 0x01, 0x00, 0x15, 0x16, 0x08, 0x00,
      0x61, 0x62, 0x63, 0x64, 0x16, 0x26, 0x36,
      0x46
    },

    //WDS_G0_DORMANT 
    {0x02, 0x07, 0x00, 0x25, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x01, 0x00, 0x09, 0x00},
    //WDS_G0_DORMANT
    {0x02, 0x07, 0x00, 0x25, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x01, 0x00, 0x71, 0x55, 0x05, 0x00, 
      0x01, 0x02, 0x00, 0x11, 0x21,
    },

    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x02, 0x00, 0x71, 0x55, 0x04, 0x00, 
      0x02, 0x01, 0x00, 0x12
    },

    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x01, 0x00, 0x71, 0x55, 0x09, 0x00, 
      0x01, 0x02, 0x00, 0x11, 0x21,
      0x02, 0x01, 0x00, 0x12
    },
    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x01, 0x00, 0x71, 0x55, 0x07, 0x00, 
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14
    },
    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x01, 0x00, 0x71, 0x55, 0x47, 0x00, 
      0x11, 0x44, 0x00, 0x08, 0x01, 0x02, 0x11, 0x21,
      0x02, 0x03, 0x12, 0x22, 0x32, 0x05, 0x06, 0x15,
      0x25, 0x35, 0x45, 0x55, 0x65, 0x06, 0x07, 0x16,
      0x26, 0x36, 0x46, 0x56, 0x66, 0x67, 0x07, 0x08,
      0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87,
      0x08, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
      0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
      0x1e, 0x1f, 0x04, 0x05, 0x14, 0x24, 0x34, 0x44,
      0x54, 0x03, 0x04, 0x13, 0x23, 0x33, 0x43,
    },

    //WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND 
    { 0x04, 0x01, 0x00, 0x71, 0x55, 0x57, 0x00, 
      0x01, 0x02, 0x00, 0x11, 0x21,    
      0x02, 0x01, 0x00, 0x12,
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14,
      0x11, 0x44, 0x00, 0x08, 0x01, 0x02, 0x11, 0x21,
      0x02, 0x03, 0x12, 0x22, 0x32, 0x05, 0x06, 0x15,
      0x25, 0x35, 0x45, 0x55, 0x65, 0x06, 0x07, 0x16,
      0x26, 0x36, 0x46, 0x56, 0x66, 0x67, 0x07, 0x08,
      0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87,
      0x08, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
      0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
      0x1e, 0x1f, 0x04, 0x05, 0x14, 0x24, 0x34, 0x44,
      0x54, 0x03, 0x04, 0x13, 0x23, 0x33, 0x43,
    },

    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14
    },
    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x11, 0x04, 0x00, 0x21, 0x22, 0x23, 0x24
    },

    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x19, 0x08, 0x00, 0x31, 0x32, 0x33, 0x34,
      0x35, 0x36, 0x37, 0x38,
    },
    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x1a, 0x08, 0x00, 0x41, 0x42, 0x43, 0x44,
      0x45, 0x46, 0x47, 0x48,
    },
    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x25, 0x04, 0x00, 0x51, 0x52, 0x53, 0x54
    },
    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x07, 0x00, 
      0x26, 0x04, 0x00, 0x51, 0x52, 0x53, 0x54
    },

    //WDS_RM_TRANSFER_STATISTICS_IND
    { 0x04, 0x02, 0x00, 0x68, 0x55, 0x32, 0x00, 
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14,
      0x11, 0x04, 0x00, 0x21, 0x22, 0x23, 0x24,
      0x19, 0x08, 0x00, 0x31, 0x32, 0x33, 0x34,
      0x35, 0x36, 0x37, 0x38, 0x1a, 0x08, 0x00,
      0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
      0x48, 0x25, 0x04, 0x00, 0x51, 0x52, 0x53,
      0x54, 0x26, 0x04, 0x00, 0x51, 0x52, 0x53,
      0x54,
    },
};

void dump_wds_SLQSSetWdsEventCallback_ind(void* ptr)
{
    unpack_wds_SLQSSetWdsEventCallback_ind_t *result = 
        (unpack_wds_SLQSSetWdsEventCallback_ind_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    if (result->xferStatAvail)
    {
        if(swi_uint256_get_bit (result->ParamPresenceMask, 25))
        {
            printf("tx_bytes = %"PRIu64"\n", result->tx_bytes);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 26))
        {
            printf("rx_bytes = %"PRIu64"\n", result->rx_bytes);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
        {
            printf("tx_pkts = %"PRIu64"\n",  result->tx_pkts);
        }
        if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
        {
            printf("rx_pkts = %"PRIu64"\n",  result->rx_pkts);
        }
    }
    if ( (result->mipstatAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 27))
    {
        printf("mip status = %d\n", result->mipStatus);
    }
    if ((result->dBTechAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 23))
    {
        printf("Data Bearer technology = %d\n", result->dBTechnology);
    }
    if ( (result->dormancyStatAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 24))
    {
        printf("Dormancy Status = %d\n", result->dormancyStatus);
    }
    if ( (result->currDBTechAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 29))
    {
        printf("RAT Mask = %d\n", result->ratMask);
        printf("SO Mask = %d\n", result->soMask);
    }
    if ( (result->dataSysStatAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 36))
    {
        int count =0;
        printf("Preferred Network = %d\n", result->prefNetwork);
        for(count=0;count<result->netInfoLen; count++)
        {
            printf("Network Type = %d\n", result->currNWInfo[count].NetworkType);
            printf("RAT Mask = %d\n", result->currNWInfo[count].RATMask);
            printf("SO Mask = %d\n", result->currNWInfo[count].SOMask);
        }
    }
    if ( (result->dBtechExtAvail) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 42))
    {
        printf("Technology = %d\n", result->dBtechnologyExt);
        printf("RAT Mask = %d\n", result->dBTechExtRatValue);
        printf("SO Mask = %"PRIu64"\n", result->dBTechExtSoMask);
    }
}

void dump_wds_SLQSSetPacketSrvStatusCallback(void* ptr)
{
    unpack_wds_SLQSSetPacketSrvStatusCallback_t *result = 
        (unpack_wds_SLQSSetPacketSrvStatusCallback_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    printf("WDS GET PKT STATUS Callback Info\n");
    if(swi_uint256_get_bit (result->ParamPresenceMask, 1))
    {
        printf("connection status:%d\n", result->conn_status);
        printf("reconfiguration required:%d\n", result->reconfigReqd);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 16))
    {
        printf("Session end reason:%d\n", result->sessionEndReason);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 17))
    {
        printf("Verbose session end reason type:%d\n", result->verboseSessnEndReasonType);
        printf("Verbose session end reason:%d\n", result->verboseSessnEndReason);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 18))
    {
        printf("IP family:%d\n", result->ipFamily);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 19))
    {
        printf("technology name:%d\n", result->techName);
    }
    if(swi_uint256_get_bit (result->ParamPresenceMask, 20))
    {
        printf("bearer id:%d\n", result->bearerID);
    }
}

void dump_wds_RMTransferStatistics_ind(void* ptr)
{
    unpack_RMTransferStatistics_ind_t *result = 
        (unpack_RMTransferStatistics_ind_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    
    if( (result->TxOkConutTlv.TlvPresent) && 
        swi_uint256_get_bit (result->ParamPresenceMask, 16) )
    {
        printf ("TxOkConut: %u\n",result->TxOkConutTlv.ulData );
    }
    if( (result->RxOkConutTlv.TlvPresent) && 
        swi_uint256_get_bit (result->ParamPresenceMask, 17) )
    {
        printf ("RxOkConut: %u\n",result->RxOkConutTlv.ulData );
    }
    if( (result->TxOkByteCountTlv.TlvPresent) &&
        swi_uint256_get_bit (result->ParamPresenceMask, 25) )
    {
        printf ("TxOkByteCount: %"PRIu64"\n",result->TxOkByteCountTlv.ullData );
    }
    if( (result->RxOkByteCountTlv.TlvPresent) &&
        swi_uint256_get_bit (result->ParamPresenceMask, 26) ) 
    {
        printf ("RxOkByteCount: %"PRIu64"\n",result->RxOkByteCountTlv.ullData );
    }
    if( (result->TxDropConutTlv.TlvPresent) && 
        swi_uint256_get_bit (result->ParamPresenceMask, 37) )
    {
        printf ("TxDropConut: %u\n",result->TxDropConutTlv.ulData );
    }
    if( (result->RxDropConutTlv.TlvPresent) &&
        swi_uint256_get_bit (result->ParamPresenceMask, 38))
    {
        printf ("RxDropConut: %u\n",result->RxDropConutTlv.ulData );
    }
}

void dump_wds_DHCPv4ClientLease_ind(void* ptr)
{
    unpack_wds_DHCPv4ClientLease_ind_t *result = 
        (unpack_wds_DHCPv4ClientLease_ind_t*) ptr;
    int i = 0, j = 0;
    wds_DHCPOpt *pOption = NULL;
    CHECK_DUMP_ARG_PTR_IS_NULL
    printf("%s Data\n",__FUNCTION__);
    swi_uint256_print_mask (result->ParamPresenceMask);
    fprintf(stdout,"WDS DHCPV4 CLIENT LEASE CHANGE Callback Info\n");
    swi_uint256_print_mask(result->ParamPresenceMask);
    if( (result->ProfileIdTlv.TlvPresent) &&
        swi_uint256_get_bit(result->ParamPresenceMask, 1))
    {
        printf ("Profile type: %d\n", result->ProfileIdTlv.profileType );
        printf ("Profile id: %d\n", result->ProfileIdTlv.profileId );
    }
    
    if( (result->DHCPv4LeaseStateTlv.TlvPresent) &&
        swi_uint256_get_bit(result->ParamPresenceMask, 2))
    {
        printf ("Lease state: %d\n", result->DHCPv4LeaseStateTlv.leaseState );
    }
    if( (result->IPv4AddrTlv.TlvPresent) &&
        swi_uint256_get_bit(result->ParamPresenceMask, 16))
    {

        printf ( "IPv4 address: %x\n", result->IPv4AddrTlv.IPv4Addr );
    }
    if( (result->DHCPv4LeaseOptTlv.TlvPresent) &&
        swi_uint256_get_bit(result->ParamPresenceMask, 17))
    {
        printf ("Num lease options: %d\n", result->DHCPv4LeaseOptTlv.numOpt );
        printf ("Lease options:\n" );
        for (i=0; i < result->DHCPv4LeaseOptTlv.numOpt; i++)
        {
            pOption = &result->DHCPv4LeaseOptTlv.optList[i];
            if(pOption)
            {
                printf ( "%2d  ", pOption->optCode);
                for (j=0; j < pOption->optValLen; j++)
                {
                    printf ("%02X", *(pOption->pOptVal + j) );
                }
                printf ("\n" );
            }            
        }
    }
}

void dump_wds_SLQSDUNCallInfoCallBack_ind(void* ptr)
{
    unpack_wds_SLQSDUNCallInfoCallBack_ind_t *result = 
        (unpack_wds_SLQSDUNCallInfoCallBack_ind_t*) ptr;
    fprintf(stdout,"WDS DUN CALL INFO indication received with result %d\n", unpackRetCode);
    swi_uint256_print_mask(result->ParamPresenceMask);
    if (!unpackRetCode)
    {
        if ( (result->CSTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 16) )
        {
            printf("Connection Status: 0x%x \n",
                result->CSTlv.MDMConnStatus );
        }
        if ( (result->LMCERTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 17) )
        {
            printf("Call End Reason: 0x%x \n",
                result->LMCERTlv.CallEndReason );
        }
        if ( (result->TXBOTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 18) )
        {
            printf("Tx Bytes OK:: %"PRIu64" \n",
                result->TXBOTlv.TxByteOKCnt );
        }
        if ( (result->RXBOTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 19) )
        {
            printf("Rx Bytes OK:: %"PRIu64" \n",
                result->RXBOTlv.RxByteOKCnt );
        }
        if ( (result->DSTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 20) )
        {
            printf("Dormancy Status:: 0x%x \n",
                result->DSTlv.DormancyStat );
        }
        if ( (result->DBTTlv.TlvPresent) &&
            swi_uint256_get_bit(result->ParamPresenceMask, 21) )
        {
            printf("Data Bearer Technology: 0x%x \n",
                result->DBTTlv.DataBearerTech );
        }
        if ( (result->CRTlv.TlvPresent)  &&
            swi_uint256_get_bit(result->ParamPresenceMask, 22) )
        {
            printf("Current Channel Tx rate: 0x%x \n",
                result->CRTlv.ChannelRate.CurrChanTxRate );
            printf("Current Channel Rx rate: 0x%x \n",
                result->CRTlv.ChannelRate.CurrChanRxRate );
        }
    }
}
extern char* get_cmd_str(uint16_t svc, uint16_t cmd);
void wds_dummy_unpack()
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
    loopCount = sizeof(dummy_wds_resp_msg)/sizeof(dummy_wds_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index>=loopCount)
            return ;
        //TODO select multiple file and read them
        memcpy(&msg.buf,&dummy_wds_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eWDS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("WDS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("WDS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_WDS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSWdsSetEventReport,
                        dump_wds_SLQSWdsSetEventReport,
                        msg.buf,
                        rlen);
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSetWdsEventCallback,
                        dump_wds_SLQSSetWdsEventCallback,
                        msg.buf,
                        rlen);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSetWdsEventCallback_ind,
                        dump_wds_SLQSSetWdsEventCallback_ind,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_DEFAULT_PROFILE_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetDefaultProfileNum,
                        dump_GetDefaultProfileNum,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_GET_3GPP_CFG_ITEM:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSGet3GPPConfigItem_t tunpack_wds_SLQSGet3GPPConfigItem;
                    memset(&tunpack_wds_SLQSGet3GPPConfigItem,
                        0,
                        sizeof(tunpack_wds_SLQSGet3GPPConfigItem));
                    tunpack_wds_SLQSGet3GPPConfigItem.LTEAttachProfileListLen = MAX_WDS_3GPP_CONF_LTE_ATTACH_PROFILE_LIST_SIZE-1;
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGet3GPPConfigItem,
                        dump_SLQSGet3GPPConfigItem,
                        msg.buf,
                        rlen,
                        &tunpack_wds_SLQSGet3GPPConfigItem);
                }
                break;
            case eQMI_WDS_GET_DEFAULTS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetDefaultProfile,
                        dump_wds_GetDefaultProfile,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_GET_DATA_LOOPBACK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSGetLoopback,
                        dump_wds_SLQSSGetLoopback,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_AUTOCONNECT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetAutoconnect,
                        dump_wds_GetAutoconnect,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetAutoconnect,
                        dump_wds_GetAutoconnect,
                        msg.buf,
                        rlen,
                        &tunpack_wds_GetAutoconnect);
                }
                break;
            case eQMI_WDS_GET_PKT_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetSessionState,
                        dump_GetSessionState,
                        msg.buf,
                        rlen);
                }
                else if (eIND == rsp_ctx.type)
                {
                    fprintf(stderr,"%s:%d\n",__FUNCTION__,__LINE__);
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSetPacketSrvStatusCallback,
                        dump_wds_SLQSSetPacketSrvStatusCallback,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_DURATION:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetSessionDuration,
                        dump_GetSessionDuration,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_GetByteTotals_t lunpack_wds_GetByteTotals;
                    unpack_wds_GetPacketStatistics_t lunpack_wds_GetPacketStatistics;
                    uint64_t lGetByteTotalsRXTotalBytes = 0;
                    uint64_t lGetByteTotalsTXTotalBytes = 0;
                    uint32_t lTXPacketSuccesses = 0;
                    uint32_t lRXPacketSuccesses = 0;
                    uint32_t lTXPacketErrors = 0;
                    uint32_t lRXPacketErrors = 0;
                    uint32_t lTXPacketOverflows = 0;
                    uint32_t lRXPacketOverflows = 0;
                    uint64_t lTXOkBytesCount = 0;
                    uint64_t lRXOkBytesCount = 0;
                    uint64_t lTXOKBytesLastCall = 0;
                    uint64_t lRXOKBytesLastCall = 0;
                    uint32_t lTXDroppedCount = 0;
                    uint32_t lRXDroppedCount = 0;
                    memset(&lunpack_wds_GetByteTotals,0,sizeof(unpack_wds_GetByteTotals_t));
                    memset(&lunpack_wds_GetPacketStatistics,0,sizeof(unpack_wds_GetPacketStatistics_t));
                    lunpack_wds_GetByteTotals.pRXTotalBytes = &lGetByteTotalsRXTotalBytes;
                    lunpack_wds_GetByteTotals.pTXTotalBytes = &lGetByteTotalsTXTotalBytes;
                    lunpack_wds_GetPacketStatistics.pTXPacketSuccesses = &lTXPacketSuccesses;
                    lunpack_wds_GetPacketStatistics.pRXPacketSuccesses = &lRXPacketSuccesses;
                    lunpack_wds_GetPacketStatistics.pTXPacketErrors = &lTXPacketErrors;
                    lunpack_wds_GetPacketStatistics.pRXPacketErrors = &lRXPacketErrors;
                    lunpack_wds_GetPacketStatistics.pTXPacketOverflows = &lTXPacketOverflows;
                    lunpack_wds_GetPacketStatistics.pRXPacketOverflows = &lRXPacketOverflows;
                    lunpack_wds_GetPacketStatistics.pTXOkBytesCount = &lTXOkBytesCount;
                    lunpack_wds_GetPacketStatistics.pRXOkBytesCount = &lRXOkBytesCount;
                    lunpack_wds_GetPacketStatistics.pTXOKBytesLastCall = &lTXOKBytesLastCall;
                    lunpack_wds_GetPacketStatistics.pRXOKBytesLastCall = &lRXOKBytesLastCall;
                    lunpack_wds_GetPacketStatistics.pTXDroppedCount = &lTXDroppedCount;
                    lunpack_wds_GetPacketStatistics.pRXDroppedCount = &lRXDroppedCount;
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetPacketStatus,
                        dump_wds_GetPacketStatus,
                        msg.buf,
                        rlen);
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetByteTotals,
                        dump_wds_GetByteTotals,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetByteTotals,
                        dump_wds_GetByteTotals,
                        msg.buf,
                        rlen,
                        &lunpack_wds_GetByteTotals);
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetPacketStatistics,
                        dump_wds_GetPacketStatistics,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetPacketStatistics,
                        dump_wds_GetPacketStatistics,
                        msg.buf,
                        rlen,
                        &lunpack_wds_GetPacketStatistics);
                    
                }
                break;
            case eQMI_WDS_GET_DORMANCY:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetDormancyState,
                        dump_GetDormancyState,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_CUR_DATA_BEARER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetDataBearerTechnology,
                        dump_wds_SLQSGetDataBearerTechnology,
                        msg.buf,
                        rlen);
                }                
                break;
            case eQMI_WDS_GET_RATES:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetConnectionRate,
                        dump_GetConnectionRate,
                        msg.buf,
                        rlen);
                }                
                break;
            case eQMI_WDS_GET_MIP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetMobileIP,
                        dump_GetMobileIP,
                        msg.buf,
                        rlen);
                }                
                break;
            case eQMI_WDS_GET_CURR_DATA_SYS_STAT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetCurrDataSystemStat,
                        dump_SLQSGetCurrDataSystemStat,
                        msg.buf,
                        rlen);
                }           
                break;                
            case eQMI_WDS_GET_LAST_MIP_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetLastMobileIPError,
                        dump_GetLastMobileIPError,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_GET_CURRENT_CHANNEL_RATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetCurrentChannelRate,
                        dump_wds_SLQSGetCurrentChannelRate,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_DATA_BEARER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetDataBearerTechnology,
                        dump_wds_GetDataBearerTechnology,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetDataBearerTechnology,
                        dump_wds_GetDataBearerTechnology,
                        msg.buf,
                        rlen,
                    &tunpack_wds_GetDataBearerTechnology);
                }
                break;
            case eQMI_WDS_GET_DUN_CALL_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetDUNCallInfo,
                        dump_wds_SLQSGetDUNCallInfo,
                        msg.buf,
                        rlen);
                }
                else if (eIND == rsp_ctx.type) //case eQMI_WDS_GET_DUN_CALL_INFO_IND:
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSDUNCallInfoCallBack_ind,
                        dump_wds_SLQSDUNCallInfoCallBack_ind,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_DEFAULT_PROFILE_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetDefaultProfile,
                        dump_wds_SetDefaultProfile,
                        msg.buf,
                        rlen);
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetDefaultProfileNum,
                        dump_SetDefaultProfileNum,
                        msg.buf,
                        rlen);
                        
                }
                break;
            case eQMI_WDS_SWI_SET_3GPP_CFG_ITEM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSet3GPPConfigItem,
                        dump_SLQSSet3GPPConfigItem,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetRuntimeSettings,
                        dump_wds_SLQSGetRuntimeSettings,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_STOP_NET:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSStopDataSession,
                        dump_wds_SLQSStopDataSession,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_MIP_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetMobileIPProfile,
                        dump_SetMobileIPProfile,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_GET_MIP_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetMobileIPProfile,
                        dump_GetMobileIPProfile,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_GetMobileIPProfile,
                        dump_GetMobileIPProfile,
                        msg.buf,
                        rlen,
                        &tunpack_wds_GetMobileIPProfile);
                }
                break;
            case eQMI_WDS_SWI_RM_TRANSFER_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_RMSetTransferStatistics,
                        dump_RMSetTransferStatistics,
                        msg.buf,
                        rlen);
                }
                else if (eIND == rsp_ctx.type)
                {
                    unpack_RMTransferStatistics_ind_t resp;
                    memset(&resp,0,sizeof(resp));
                    UNPACK_IND_AND_DUMP(unpackRetCode,
                        unpack_wds_RMTransferStatistics_ind,
                        dump_wds_RMTransferStatistics_ind,
                        msg.buf,
                        rlen,
                        &resp);
                }
                break;
            case eQMI_WDS_SWI_PDP_RUNTIME_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t lunpack_wds_SLQSWdsSwiPDPRuntimeSettings;
                    memset(&lunpack_wds_SLQSWdsSwiPDPRuntimeSettings,0,sizeof(unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t));
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSWdsSwiPDPRuntimeSettings,
                        dump_SLQSWdsSwiPDPRuntimeSettings,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSWdsSwiPDPRuntimeSettings,
                        dump_SLQSWdsSwiPDPRuntimeSettings,
                        msg.buf,
                        rlen,
                        &lunpack_wds_SLQSWdsSwiPDPRuntimeSettings);
                }
                break;
            case eQMI_WDS_CREATE_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSCreateProfile_t lunpack_wds_SLQSCreateProfile;
                    PackCreateProfileOut lCreateProfileOut;
                    uint8_t  lProfileID = 0;
                    memset(&lunpack_wds_SLQSCreateProfile,0,sizeof(unpack_wds_SLQSCreateProfile_t));
                    memset(&lCreateProfileOut,0,sizeof(PackCreateProfileOut));
                    lunpack_wds_SLQSCreateProfile.pCreateProfileOut = &lCreateProfileOut;
                    lunpack_wds_SLQSCreateProfile.pProfileID = &lProfileID;
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_wds_SLQSCreateProfile,
                            dump_wds_SLQSCreateProfile,
                            msg.buf,
                            rlen,
                            &lunpack_wds_SLQSCreateProfile);
                }
                break;
            case eQMI_WDS_SWI_CREATE_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSCreateProfile_t lunpack_wds_SLQSCreateProfile;
                    PackCreateProfileOut lCreateProfileOut;
                    memset(&lunpack_wds_SLQSCreateProfile,0,sizeof(unpack_wds_SLQSCreateProfile_t));
                    memset(&lCreateProfileOut,0,sizeof(PackCreateProfileOut));
                    lunpack_wds_SLQSCreateProfile.pCreateProfileOut = &lCreateProfileOut;
                    lunpack_wds_SLQSCreateProfile.pProfileID = NULL;
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSCreateProfile,
                        dump_wds_SLQSCreateProfile,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                            unpack_wds_SLQSCreateProfile,
                            dump_wds_SLQSCreateProfile,
                            msg.buf,
                            rlen,
                            &lunpack_wds_SLQSCreateProfile);
                }
                break;
            case eQMI_WDS_START_NET:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSStartDataSession_t lunpack_wds_SLQSStartDataSession;
                    uint32_t lsid = 0;
                    uint32_t lFailureReason = 0;
                    uint32_t lVerboseFailReasonType = 0;
                    uint32_t lVerboseFailureReason = 0;
                    memset(&lunpack_wds_SLQSStartDataSession,0,sizeof(unpack_wds_SLQSStartDataSession_t));
                    lunpack_wds_SLQSStartDataSession.pFailureReason = &lFailureReason;
                    lunpack_wds_SLQSStartDataSession.psid= &lsid;
                    lunpack_wds_SLQSStartDataSession.pVerboseFailReasonType= &lVerboseFailReasonType;
                    lunpack_wds_SLQSStartDataSession.pVerboseFailureReason= &lVerboseFailureReason;
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSStartDataSession,
                        dump_wds_SLQSStartDataSession,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSStartDataSession,
                        dump_wds_SLQSStartDataSession,
                        msg.buf,
                        rlen,
                        &lunpack_wds_SLQSStartDataSession);
                }
                break;
            case eQMI_WDS_MODIFY_PROFILE:
                if (eRSP == rsp_ctx.type)
                {

                    unpack_wds_SLQSModifyProfile_t lunpack_wds_SLQSModifyProfile;
                    uint16_t ExtErrorCode = 0;
                    memset(&lunpack_wds_SLQSModifyProfile,0,sizeof(unpack_wds_SLQSModifyProfile_t));
                    lunpack_wds_SLQSModifyProfile.pExtErrorCode = &ExtErrorCode;
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSModifyProfile,
                        dump_wds_SLQSModifyProfile,
                        msg.buf,
                        rlen);

                     UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSModifyProfile,
                        dump_wds_SLQSModifyProfile,
                        msg.buf,
                        rlen,
                        &lunpack_wds_SLQSModifyProfile);
                }
                break;
            case eQMI_WDS_DELETE_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSDeleteProfile,
                        dump_wds_SLQSDeleteProfile,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_IP_FAMILY:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSetIPFamilyPreference,
                        dump_wds_SLQSSetIPFamilyPreference,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_SET_DATA_LOOPBACK:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSSetLoopback,
                        dump_wds_SLQSSSetLoopback,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_DHCPv4ClientLeaseChange,
                        dump_wds_DHCPv4ClientLeaseChange,
                        msg.buf,
                        rlen);
                }
                else if (eIND == rsp_ctx.type) //case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND:
                {
                     UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_wds_DHCPv4ClientLease_ind,
                        dump_wds_DHCPv4ClientLease_ind,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_MIP:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetMobileIP,
                        dump_wds_SetMobileIP,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_MIP_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetMobileIPParameters,
                        dump_wds_SetMobileIPParameters,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SET_AUTOCONNECT:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetAutoconnect,
                        dump_wds_SetAutoconnect,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_G0_DORMANT:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSWdsGoDormant,
                        dump_wds_SLQSWdsGoDormant,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_G0_ACTIVE:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSWdsGoActive,
                        dump_wds_SLQSWdsGoActive,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_RESET_PKT_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSResetPacketStatics,
                        dump_wds_SLQSResetPacketStatics,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                     UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSSetDHCPv4ClientConfig,
                        dump_wds_SLQSSSetDHCPv4ClientConfig,
                        msg.buf,
                        rlen);
                }
                break;
            default:
                fprintf(stderr,"======NO unpack handler %s\n", get_cmd_str(eWDS,rsp_ctx.msgid));
                exit(1);
                break;
            case eQMI_WDS_GET_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetProfileSettings,
                        dump_wds_SLQSGetProfileSettings,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetProfileSettings,
                        dump_wds_SLQSGetProfileSettings,
                        msg.buf,
                        rlen,
                        &tSLQSGetProfileSettingsRsp);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSGetProfileSettings,
                        dump_wds_SLQSGetProfileSettings_3GPP2,
                        msg.buf,
                        rlen,
                        &tSLQSGetProfileSettings3GPP2Rsp);
                }
                break;
           case eQMI_WDS_SWI_GET_DHCPV4_CLIENT_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSSGetDHCPv4ClientConfig_t lunpack_wds_SLQSSGetDHCPv4ClientConfig;
                    wdsDhcpv4HwConfig lwdsDhcpv4HwConfig;
                    wdsDhcpv4OptionList lwdsDhcpv4OptionList;
                    wdsDhcpv4Option lWdsDHCPv4Option[255];
                    memset(&lwdsDhcpv4HwConfig,0,sizeof(wdsDhcpv4HwConfig));
                    memset(&lwdsDhcpv4OptionList,0,sizeof(wdsDhcpv4OptionList));
                    memset(&lWdsDHCPv4Option,0,sizeof(lWdsDHCPv4Option));
                    lwdsDhcpv4OptionList.numOpt = 0;
                    lwdsDhcpv4OptionList.pOptList = &lWdsDHCPv4Option[0];
                    memset(&lunpack_wds_SLQSSGetDHCPv4ClientConfig,0,sizeof(unpack_wds_SLQSSGetDHCPv4ClientConfig_t));
                    lunpack_wds_SLQSSGetDHCPv4ClientConfig.pHwConfig = &lwdsDhcpv4HwConfig;
                    lunpack_wds_SLQSSGetDHCPv4ClientConfig.pRequestOptionList = &lwdsDhcpv4OptionList;
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSGetDHCPv4ClientConfig,
                        dump_wds_SLQSSGetDHCPv4ClientConfig,
                        msg.buf,
                        rlen);
                    UNPACK_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SLQSSGetDHCPv4ClientConfig,
                        dump_wds_SLQSSGetDHCPv4ClientConfig,
                        msg.buf,
                        rlen,
                        &lunpack_wds_SLQSSGetDHCPv4ClientConfig);
                }
                break;
             case eQMI_WDS_SET_QMUX_ID:
                 UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_wds_SetMuxID,
                        dump_wds_SetMuxID,
                        msg.buf,
                        rlen);
                break;
            
            }
        }
    }
    return ;
}

/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
 uint8_t validate_wds_resp_msg[][QMI_MSG_MAX] ={

    { /* eQMI_WDS_PKT_STATUS_IND */
      0x04,0x01,0x00,0x22,0x00,0x12,0x00,0x01,0x02,
      0x00,0x02,0x00,0x12,0x01,0x00,0x04,0x13,0x02,
      0x00,0x80,0x88,0x14,0x01,0x00,0x05
    },

    /* eQMI_WDS_SET_EVENT_IND */
    { 0x04, 0x02, 0x00, 0x01, 0x00, 0x2C, 0x00, 
      0x10, 0x04 ,0x00, 0xF1, 0x00, 0x00, 0x00,
      0x11, 0x04 ,0x00, 0xF2, 0x00, 0x00, 0x00,
      0x19, 0x08 ,0x00, 0x02, 0x03, 0x00, 0x00,0x00,0x00,0x00,0x00,
      0x1A, 0x08 ,0x00, 0x01, 0x04, 0x00, 0x00,0x00,0x00,0x00,0x00,
      0x17, 0x01 ,0x00, 0x04,
      0x18, 0x01 ,0x00, 0x02,
    },

    /* eQMI_WDS_GET_DUN_CALL_INFO_IND */
    { 0x04, 0x03, 0x00, 0x38, 0x00, 0x32, 0x00, 
      0x10, 0x01, 0x00, 0x01, 0x11, 0x02, 0x00,
      0x11, 0x12, 0x12, 0x08, 0x00, 0x21, 0x22,
      0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x13,
      0x08, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35,
      0x36, 0x37, 0x38, 0x14, 0x01, 0x00, 0x02,
      0x15, 0x01, 0x00, 0x04, 0x16, 0x08, 0x00,
      0x61, 0x62, 0x63, 0x64, 0x16, 0x26, 0x36,
      0x46
    },

    /* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND */
    { 0x04, 0x04, 0x00, 0x71, 0x55, 0x2E, 0x00, 
      0x01, 0x02, 0x00, 0x11, 0x21,
      0x02, 0x01, 0x00, 0x03,
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14,
      0x11, 0x1B, 0x00, 0x04, 0x01, 0x02, 0x11, 0x21,
      0x02, 0x03, 0x12, 0x22, 0x32, 0x05, 0x06, 0x15,
      0x25, 0x35, 0x45, 0x55, 0x65, 0x06, 0x07, 0x16,
      0x26, 0x36, 0x46, 0x56, 0x66, 0x67, 
    },

    /* eQMI_WDS_RM_TRANSFER_STATISTICS_IND */
    { 0x04, 0x05, 0x00, 0x68, 0x55, 0x32, 0x00, 
      0x10, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14,
      0x11, 0x04, 0x00, 0x21, 0x22, 0x23, 0x24,
      0x19, 0x08, 0x00, 0x31, 0x32, 0x33, 0x34,
      0x35, 0x36, 0x37, 0x38, 0x1a, 0x08, 0x00,
      0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
      0x48, 0x25, 0x04, 0x00, 0x51, 0x52, 0x53,
      0x54, 0x26, 0x04, 0x00, 0x51, 0x52, 0x53,
      0x54,
    },

    /* eQMI_WDS_START_NET */
    {0x02, 0x06, 0x00, 0x20, 0x00, 0x1a, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00,
     0x03, 0x00, 0x11, 0x04, 0x00, 0x03, 0x00, 0xd1,
     0x07
    },

    /* eQMI_WDS_STOP_NET */
    {0x02, 0x07, 0x00, 0x21, 0x00, 0x7, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    },

    {/* eQMI_WDS_RUNTIME_SETTINGS */
        0x02,0x08,0x00,0x2d,0x00,0x6c,0x00,
        0x02,0x04,0x00,0x00,0x00,0x00,0x00,
        0x10,0x0c,0x00,0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66,
        0x11,0x01,0x00,0x00,
        0x14,0x08,0x00,0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65,
        0x15,0x04,0x00,0x85,0x17,0x1e,0x0a,
        0x16,0x04,0x00,0x86,0x17,0x1e,0x0a,
        0x1d,0x01,0x00,0x00,
        0x1e,0x04,0x00,0xf9,0xea,0xa6,0x0a,
        0x1f,0x02,0x00,0x00,0x01,
        0x20,0x04,0x00,0xfa,0xea,0xa6,0x0a,
        0x21,0x04,0x00,0xfc,0xff,0xff,0xff,
        0x22,0x01,0x00,0x00,
        0x23,0x01,0x00,0x00,
        0x24,0x01,0x00,0x00,
        0x29,0x04,0x00,0xdc,0x05,0x00,0x00,
        0x2a,0x01,0x00,0x00,
        0x2b,0x01,0x00,0x04
    },

    /* eQMI_WDS_SET_EVENT */
    {0x02, 0x09, 0x00, 0x01, 0x00, 0x7, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    },

    /* eQMI_WDS_CREATE_PROFILE */
    {0x02, 0x0a, 0x00, 0x27, 0x00, 0x11, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x01, 0x02, 0x00, 0x11, 0x21,
     0xe0, 0x02, 0x00, 0xe1, 0xe2},

    /* eQMI_WDS_MODIFY_PROFILE */
    {0x02, 0x0b, 0x00, 0x28, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0x02, 0x00, 0xe1, 0xe2},

    /* eQMI_WDS_GET_PROFILE 3GPP */
    {0x02, 0x0c, 0x00, 0x2b, 0x00, 0x85, 0x01, 
    0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,//7
    0x10,0x0c,0x00,0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66,//15
    0x11,0x01,0x00,0x00,//4
    0x12,0x01,0x00,0x01,//4
    0x13,0x01,0x00,0x01,//4
    0x14,0x08,0x00,0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65,//11
    0x15,0x04,0x00,0x85,0x17,0x1e,0x0a,//7
    0x16,0x04,0x00,0x86,0x17,0x1e,0x0a,//7

    0x17, 0x21, 0x00, 0x17, 0x18, 0x19, 0x20,//36
    0x17, 0x18, 0x19, 0x20, 0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20, 0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20, 0x17, 0x18, 0x19, 0x20,
    0x17, 0x18, 0x19, 0x20, 0x17,

    0x19, 0x14, 0x00, 0x19, 0x1a, 0x1b, 0x1c,//23
    0x19, 0x1a, 0x1b, 0x1c,0x19, 0x1a, 0x1b, 0x1c,
    0x19, 0x1a, 0x1b, 0x1c,0x19, 0x1a, 0x1b, 0x1c,

    0x1b, 0x08, 0x00, 0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65,//11

    0x1d,0x01,0x00,0x00,//4
    0x1e,0x04,0x00,0xf9,0xea,0xa6,0x0a,//7

    0x24, 0x27, 0x00, 0x01, 0x02, 0x06,//42
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c ,0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x34 ,0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44,

    0x25, 0x01, 0x00, 0x25,//4
    0x26, 0x01, 0x00, 0x01,//4
    0x27, 0x01, 0x00, 0x27,//4

    0x28, 0x10, 0x00, 0x28, 0x29, 0x2a, 0x2b, 0x2c,//19
    0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
    0x35, 0x36, 0x37,
    0x8f, 0x01, 0x00, 0x01, //4
    0xdf, 0x01, 0x00, 0x00, //4
    0xde, 0x01, 0x00, 0x00, //4
    0x48, 0x01, 0x00, 0x00, //4
    0x47, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //23
    0x46, 0x04, 0x00, 0xff, 0x00, 0x00, 0x00, //7
    0x45, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, //43
    0x44, 0x01, 0x00, 0x01, //4
    0x43, 0x01, 0x00, 0x01, //4
    0x42, 0x04, 0x00, 0xff, 0x00, 0x00, 0x00,//7
    0x41, 0x04, 0x00, 0xff, 0x00, 0x00, 0x00,//7
    0x40, 0x01, 0x00, 0x01, //4
    0x3f, 0x01, 0x00, 0x01, //4
    0x3e, 0x01, 0x00, 0x01, //4
    0x3d, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, //7
    0x3c, 0x02, 0x00, 0x00, 0x00, //5
    0x3b, 0x02, 0x00, 0x2c, 0x01, //5
    0x3a, 0x02, 0x00, 0x14, 0x00, //5
    0x39, 0x03, 0x00, 0x00, 0x00, 0x00, //6
    0x38, 0x02, 0x00, 0x00, 0x00, //5
    0x37, 0x02, 0x00, 0x00, 0x00, //5
    0x36, 0x01, 0x00, 0x00, //4
    0x35, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, //11
    },

    /* eQMI_WDS_GET_PROFILE 3GPP2 */
    {0x02, 0x0d, 0x00, 0x2b, 0x00, 0xae, 0x00, 
    0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, //7
    0x90, 0x01, 0x00, 0x90,//4
    0x91, 0x04, 0x00, 0x91, 0x92, 0x93, 0x94,//7
    0x92, 0x04, 0x00, 0x92, 0x93, 0x94, 0x95,//7
    0x93, 0x01, 0x00, 0x93,//4
    0x94, 0x02, 0x00, 0x94, 0x95,//5
    0x95, 0x02, 0x00, 0x95, 0x96,//5
    0x96, 0x02, 0x00, 0x96, 0x97,//5
    0x97, 0x01, 0x00, 0x97,//4
    0x98, 0x01, 0x00, 0x98,//4
    0x99, 0x01, 0x00, 0x99,//4
    0x9a, 0x01, 0x00, 0x9a,//4
    0x9b, 0x08, 0x00, 0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D,//11
    0x65,
    0x9c, 0x08, 0x00, 0x50, 0x61, 0x73, 0x73, 0x43, 0x6F, 0x64,//11
    0x65 ,
    0x9d, 0x01, 0x00, 0x9d,//4
    0x9e, 0x04, 0x00, 0x9e, 0x9f, 0xa0, 0xa1,//7
    0x9f, 0x01, 0x00, 0x9f,//4
    0xa0, 0x01, 0x00, 0xa0,//4
    0xa1, 0x0a, 0x00, 0x41, 0x70, 0x6E, 0x5F, 0x73, 0x74, 0x72,//13
    0x69, 0x6E, 0x67,
    0xa2, 0x01, 0x00, 0xa2,//4
    0xa3, 0x01, 0x00, 0xa3,//4
    0xa4, 0x04, 0x00, 0xa4, 0xa5, 0xa6, 0xa7,//7
    0xa5, 0x04, 0x00, 0xa5, 0xa6, 0xa7, 0xa8, //7
    0xa6, 0x10, 0x00, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad,//19
    0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
    0xb6,
    0xa7, 0x10, 0x00, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad,//19
    0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
    0xb6
    },

    /* eQMI_WDS_GET_PKT_STATUS */
    {0x02, 0x0e, 0x00, 0x22, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
     0x00, 0x02
    },

    /* eQMI_WDS_GET_DEFAULTS */
    {0x02, 0x0f, 0x00, 0x2c, 0x00, 0x54, 0x00, 
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 

    0x10,0x0c,0x00,0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66,//15
    0x11, 0x01, 0x00, 0x00,
    0x14,0x08,0x00,0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65,//11
    0x15,0x04,0x00,0x85,0x17,0x1e,0x0a,//7
    0x16,0x04,0x00,0x86,0x17,0x1e,0x0a,//7
    0x1b, 0x08, 0x00, 0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65,//11
    0x1c, 0x08, 0x00, 0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65,//11
    0x1d,0x01,0x00,0x00,//4
    0x1e,0x04,0x00,0xf9,0xea,0xa6,0x0a,//7 
    },

    /* eQMI_WDS_GET_RATES */
    {0x02, 0x10, 0x00, 0x23, 0x00, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00},

    /* eQMI_WDS_GET_DATA_BEARER */
    {0x02, 0x11, 0x00, 0x37, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
     0x00, 0x04
    },

    /* eQMI_WDS_GET_DURATION */
    {0x02, 0x12, 0x00, 0x35, 0x00, 0x33, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
     0x01, 0x08, 0x00, 0xd2, 0x02, 0x96, 0x49, 0x00, 0x00, 0x00, 0x00,
     0x11, 0x08, 0x00, 0xd1, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x12, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x10, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },

    /* eQMI_WDS_GET_DORMANCY */
    {0x02, 0x13, 0x00, 0x30, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
     0x00, 0x02
    },

    /* eQMI_WDS_DELETE_PROFILE */
    {0x02, 0x14, 0x00, 0x29, 0x00, 0x0c, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0x02, 0x00, 0xe1, 0xe2},

    /* eQMI_WDS_SWI_SET_3GPP_CFG_ITEM */
    {0x02,0x15,0x00,0x5A,0x55,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WDS_SWI_GET_3GPP_CFG_ITEM */
    {0x02, 0x16, 0x00, 0x59, 0x55, 0x28, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x04,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x13, 0x01, 0x00,
    0x07, 0x12, 0x01, 0x00, 0x01, 0x11, 0x0A, 0x00,
    0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00,
    0x04, 0x00, 0x10, 0x02, 0x00, 0xe1, 0x10},

    /* eQMI_WDS_GET_MIP */
    {0x02, 0x17, 0x00, 0x2F, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
     0x00, 0x01
    },

    /* eQMI_WDS_GET_MIP_PROFILE */
    {0x02, 0x18, 0x00, 0x3e, 0x00, 0x47, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x01, 0x00, 0x01, 
        0x11, 0x04, 0x00, 0x11, 0x12, 0x13, 0x14, 
        0x12, 0x04, 0x00, 0x21, 0x22, 0x23, 0x24, 
        0x13, 0x04, 0x00, 0x31, 0x32, 0x33, 0x34, 
        0x14, 0x01, 0x00, 0x01, 
        0x15, 0x12, 0x00, 0x4d, 0x6f, 0x62,
        0x69, 0x6c, 0x65, 0x49, 0x50, 0x50, 0x72,
        0x6f, 0x66, 0x69, 0x6c, 0x65, 0x4E, 0x41,
        0x49, 
        0x16, 0x04, 0x00, 0x61, 0x62, 0x63,0x64, 
        0x17, 0x04, 0x00, 0x71, 0x72, 0x73,0x74,
    },

    /* eQMI_WDS_GET_CURR_DATA_SYS_STAT */
    {0x02, 0x19, 0x00, 0x6b, 0x00, 0x1e, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x14,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_GET_LAST_MIP_STATUS */
    {0x02, 0x1A, 0x00, 0x42, 0x00, 0x0b, 0x00, 0x02,
     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
     0x00, 0x02
    },

    /* eQMI_WDS_SWI_RM_TRANSFER_STATISTICS */
    {0x02,0x1b,0x00,0x68,0x55,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WDS_SET_MIP_PROFILE */
    {0x02,0x1c,0x00,0x3f,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_WDS_SWI_PDP_RUNTIME_SETTINGS */
    {0x02, 0x1d, 0x00, 0x5b, 0x55, 0xb9, 0x00, 
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x10, 0x01, 0x00, 0x10,
        0x11, 0x01, 0x00, 0x11,
        0x12, 0x09, 0x00, 0x69, 0x6e, 0x74,
        0x65, 0x72, 0x6e, 0x65, 0x74, 0x00, 
        0x13, 0x04, 0x00, 0x31, 0x32, 0x33, 0x34, 
        0x14, 0x04, 0x00, 0x41, 0x42, 0x43, 0x44, 
        0x15, 0x04, 0x00, 0x51, 0x52, 0x53, 0x54, 
        0x16, 0x04, 0x00, 0x61, 0x62, 0x63, 0x64, 
        0x17, 0x11, 0x00, 0x11, 0x12, 0x21, 0x22, 0x31,
        0x32, 0x41, 0x42, 0x51, 0x52, 0x61, 0x62,
        0x71, 0x72, 0x81, 0x82,0x10,
        0x18, 0x11, 0x00, 0xF1, 0xF2, 0xE1, 0xE2, 0xD1, 0xD2,
        0xC1, 0xC2, 0xB1, 0xB2, 0xA1, 0xA2, 0x91,
        0x92, 0x01, 0x02, 0x10, 
        0x19, 0x10, 0x00, 0x91, 0x92, 0xa1, 0xa2, 0xb1, 0xb2, 0xc1,
        0xc2, 0xd1, 0xd2, 0xe1, 0xe2, 0xf1, 0xf2,
        0x11, 0x12, 
        0x1a, 0x10, 0x00, 0xa1, 0xa2,
        0xb1, 0xb2, 0xc1, 0xc2, 0xd1, 0xd2, 0xe1,
        0xe2, 0xf1, 0xf2, 0x11, 0x12, 0x21, 0x22,

        0x1b, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
        0x1c, 0x04, 0x00, 0xb1, 0x68, 0xde, 0x3a,
        0x1d, 0x10, 0x00, 0xd1, 0xd2, 0xe1, 0xe2,
        0xf1, 0xa1, 0xa2, 0xb1, 0xb2, 0xc1, 0xc2,
        0xf2, 0x11, 0x12, 0x21, 0x22, 
        0x1e, 0x10, 0x00, 0xe2, 0xf1, 0xa1, 0xa2, 0xb1, 0xb2,
        0xc1, 0xc2, 0xd1, 0xd2, 0xe1, 0xf2, 0x11,
        0x12, 0x21, 0x22,
    },

    /* eQMI_WDS_GET_DUN_CALL_INFO */
    {
        0x02, 0x1e, 0x00, 0x38, 0x00, 0x6E, 0x00,
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, //7
        0x10, 0x09, 0x00, 0x02, 0x01, 0x02, 0x03, 0x04,0x05,0x06,0x07,0x08,//12
        0x11, 0x02, 0x00, 0x01, 0x00,//5
        0x12, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,//11
        0x40, 0x30, 0x20, 0x11,
        0x13, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,//11
        0x40, 0x30, 0x20, 0x10,
        0x14, 0x01, 0x00, 0x02,//4
        0x15, 0x01, 0x00, 0x04,//4
        0x16, 0x10, 0x00, 0x01, 0X02, 0x03, 0x04,//19
        0x11, 0x12, 0x13, 0x14, 0x21, 0x22, 0x23,
        0x24, 0x31, 0x32, 0x33, 0x34,
        0x17, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,//11
        0x40, 0x30, 0x20, 0x11,
        0x18, 0x08, 0x00, 0x80, 0X70, 0x60, 0x50,//11
        0x40, 0x30, 0x20, 0x10,
        0x19, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04,0x05,0x06,0x07,0x08,//11
        0x20, 0x01, 0x00, 0x04,//4
    },

    /* eQMI_WDS_GET_CUR_DATA_BEARER */
    {0x02, 0x1f, 0x00, 0x44, 0x00, 0x1F, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x00, 0x10, 0x09, 0x00, 0x04, 0x05, 0x00,
    0x00, 0x00, 0x06, 0x00, 0x00, 0x00 },

    /* eQMI_WDS_SET_IP_FAMILY */
    {0x02, 0x20, 0x00, 0x4d, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SET_DEFAULT_PROFILE_NUM */
    {0x02, 0x21, 0x00, 0x4a, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_GET_DEFAULT_PROFILE_NUM */
    {0x02, 0x22, 0x00, 0x49, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,0x01,0x00, 0x01},

    /* eQMI_WDS_SWI_GET_DHCPV4_CLIENT_CONFIG */
    {0x02, 0x23, 0x00, 0x6f, 0x55, 0x36, 0x00,
     0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x10, 0x08, 0x00, 0xA5,0x06,0xfc, 0xca, 0x4c, 0x36, 0x5a, 0xa3,
     0x11, 0x21, 0x00, 0x05, 0x04, 0x04, 0x41,
     0x42, 0x43, 0x44, 0x03, 0x03, 0x31, 0x32,
     0x33, 0x01, 0x01, 0x11, 0x06, 0x06, 0x61,
     0x62, 0x63, 0x64, 0x65, 0x66, 0x08, 0x08,
     0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
     0x88
    },

    /* eQMI_WDS_GET_STATISTICS */
    { 0x02, 0x24, 0x00, 0x24, 0x00, 0x6b, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
      0x11, 0x04, 0x00, 0x15, 0xcd, 0x5b, 0x07,
      0x12, 0x04, 0x00, 0xd3, 0x38, 0xfb, 0x0d,
      0x13, 0x04, 0x00, 0x40, 0xa4, 0x9a, 0x14,
      0x14, 0x04, 0x00, 0x83, 0x0c, 0x3a, 0x1b,
      0x15, 0x04, 0x00, 0x22, 0x55, 0xd9, 0x21,
      0x1d, 0x04, 0x00, 0x59, 0x61, 0x77, 0x28,
      0x1e, 0x04, 0x00, 0x80, 0x11, 0x09, 0x2f,
      0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d,
      0xf4, 0x10, 0x22, 0x11, 0x1a, 0x08, 0x00,
      0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d,
      0x20, 0x1b, 0x08, 0x00, 0x35, 0x6c, 0x56,
      0xb8, 0x4a, 0xfb, 0xf8, 0x2f, 0x1c, 0x08,
      0x00, 0x14, 0x3a, 0xa8, 0x95, 0xdd, 0x67,
      0x64, 0x3f
    },

    { 0x02, 0x25, 0x00, 0x24, 0x00, 0x6b, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x04, 0x00, 0xd2, 0x02, 0x96, 0x49,
      0x11, 0x04, 0x00, 0x15, 0xcd, 0x5b, 0x07,
      0x12, 0x04, 0x00, 0xd3, 0x38, 0xfb, 0x0d,
      0x13, 0x04, 0x00, 0x40, 0xa4, 0x9a, 0x14,
      0x14, 0x04, 0x00, 0x83, 0x0c, 0x3a, 0x1b,
      0x15, 0x04, 0x00, 0x22, 0x55, 0xd9, 0x21,
      0x1d, 0x04, 0x00, 0x59, 0x61, 0x77, 0x28,
      0x1e, 0x04, 0x00, 0x80, 0x11, 0x09, 0x2f,
      0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d,
      0xf4, 0x10, 0x22, 0x11, 0x1a, 0x08, 0x00,
      0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d,
      0x20, 0x1b, 0x08, 0x00, 0x35, 0x6c, 0x56,
      0xb8, 0x4a, 0xfb, 0xf8, 0x2f, 0x1c, 0x08,
      0x00, 0x14, 0x3a, 0xa8, 0x95, 0xdd, 0x67,
      0x64, 0x3f
    },

    { 0x02, 0x26, 0x00, 0x24, 0x00, 0x1d, 0x00, 
      0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x19, 0x08, 0x00, 0x15, 0x81, 0xe9, 0x7d,
      0xf4, 0x10, 0x22, 0x11, 0x1a, 0x08, 0x00,
      0xd2, 0x0a, 0x37, 0x61, 0x88, 0x86, 0x8d,
      0x20, 
    },
    
    /* eQMI_WDS_SWI_GET_CURRENT_CHANNEL_RATE */
    {0x02, 0x27, 0x00, 0x6b, 0x55, 0x1a, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00},

    /* eQMI_WDS_SWI_GET_DATA_LOOPBACK */
    {0x02, 0x28, 0x00, 0x69, 0x55, 0x0f, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x01,
    0x00, 0x06, 0x10, 0x01, 0x00, 0x02},

    /* eQMI_WDS_SWI_SET_DATA_LOOPBACK */
    {0x02, 0x29, 0x00, 0x6a, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    
    /* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE */
    {0x02, 0x2A, 0x00, 0x71, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SET_MIP */
    {0x02, 0x2b, 0x00, 0x2e, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* QMI_WDS_SET_MIP_PARAMS */
    {0x02, 0x2c, 0x00, 0x41, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SET_AUTOCONNECT */
    {0x02, 0x2d, 0x00, 0x51, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_GET_AUTOCONNECT */
    {0x02, 0x2e, 0x00, 0x34, 0x00, 0x0b, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,0x01,0x00, 0x01},

    /* eQMI_WDS_G0_DORMANT */
    {0x02, 0x2f, 0x00, 0x25, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_G0_ACTIVE */
    {0x02, 0x30, 0x00, 0x26, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_RESET_PKT_STATISTICS */
    {0x02, 0x31, 0x00, 0x86, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_CONFIG */
    {0x02, 0x32, 0x00, 0x70, 0x55, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SET_QMUX_ID */
    {0x02, 0x33, 0x00, 0xA2, 0x00, 0x07, 0x00, 0x02,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

    /* eQMI_WDS_SWI_PROFILE_CHANGE */
    {0x02, 0x34, 0x00, 0x67, 0x55, 0x07, 0x00,
            0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},

     /* eQMI_WDS_SWI_PROFILE_CHANGE */
    {0x04, 0x35, 0x00, 0x67, 0x55, 0x09, 0x00,
          0x01, 0x02, 0x00, 0x01, 0x05,
          0x10, 0x01, 0x00, 0x02},

   };

/* eQMI_WDS_PKT_STATUS_IND */
const unpack_wds_SLQSSetPacketSrvStatusCallback_t const_unpack_wds_SLQSSetPacketSrvStatusCallback_t = {
        2,0,0,0,0,4,0x8880,5,{{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,18,19,20)}} };

/* eQMI_WDS_SET_EVENT IND */
const unpack_wds_SLQSSetWdsEventCallback_ind_t const_unpack_wds_SLQSSetWdsEventCallback_ind_t = {
        1,0x0000000000000302ULL,0x0000000000000401ULL,0x000000F1,0x000000F2,0,0,1,4,1,2,
        0,0,0,0,0,0,{{0,0,0}},0,0,0,0,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,16,17,23,24,25,26)}} };

/* eQMI_WDS_GET_DUN_CALL_INFO_IND */
const unpack_wds_SLQSDUNCallInfoCallBack_ind_t const_unpack_wds_SLQSDUNCallInfoCallBack_ind_t = {
        {1,1},{1,0x1211},{1,0x2827262524232221ULL},{1,0x3837363534333231ULL},
        {1,2},{1,4},{1,{0x64636261,0x46362616}},
        {{SWI_UINT256_BIT_VALUE(SET_7_BITS,16,17,18,19,20,21,22)}} };

/* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND */
unpack_wds_DHCPv4ClientLease_ind_t var_unpack_wds_DHCPv4ClientLease_ind_t;
uint8_t const_DhcpOptData[WDS_DHCP_OPTION_DATA_BUF_SIZE] = 
           {0x11, 0x21, 0x12, 0x22, 0x32, 0x15, 0x25, 0x35, 0x45, 0x55, 
                  0x65,0x16, 0x26, 0x36, 0x46, 0x56, 0x66, 0x67};

const unpack_wds_DHCPv4ClientLease_ind_t const_unpack_wds_DHCPv4ClientLease_ind_t = {
       {1,0x11, 0x21},{1,3},{1,0x14131211},
       {1,4,{{1,2,&const_DhcpOptData[0]},{2,3,&const_DhcpOptData[2]},
        {5,6,&const_DhcpOptData[5]},{6,7,&const_DhcpOptData[11]}},
        {0x11, 0x21, 0x12, 0x22, 0x32, 0x15, 0x25, 0x35, 0x45, 0x55, 
         0x65,0x16, 0x26, 0x36, 0x46, 0x56, 0x66, 0x67}},
       {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_WDS_RM_TRANSFER_STATISTICS_IND */
const unpack_RMTransferStatistics_ind_t const_unpack_RMTransferStatistics_ind_t = {
       {1,0x14131211},{1,0x24232221},
       {1,0x3837363534333231ULL},{1,0x4847464544434241ULL},
       {1,0x54535251},{1,0x54535251},
       {{SWI_UINT256_BIT_VALUE(SET_6_BITS,16,17,25,26,37,38)}} };

/* eQMI_WDS_START_NET */
uint32_t var_sid;
uint32_t var_FailureReason;
uint32_t var_VerboseFailReasonType;
uint32_t var_VerboseFailureReason;

unpack_wds_SLQSStartDataSession_t var_unpack_wds_SLQSStartDataSession_t = {
       &var_sid,&var_FailureReason,&var_VerboseFailReasonType,
       &var_VerboseFailureReason,{{0}} };

uint32_t cst_sid = 0;
uint32_t cst_FailureReason = 0x00000003;
uint32_t cst_VerboseFailReasonType = 0x00000003;
uint32_t cst_VerboseFailureReason =0x000007d1;
const unpack_wds_SLQSStartDataSession_t const_unpack_wds_SLQSStartDataSession_t = {
       &cst_sid,&cst_FailureReason,&cst_VerboseFailReasonType,
       &cst_VerboseFailureReason,
       {{SWI_UINT256_BIT_VALUE(SET_4_BITS,1,2,16,17)}} };

/* eQMI_WDS_STOP_NET */
const unpack_wds_SLQSStopDataSession_t const_unpack_wds_SLQSStopDataSession_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_RUNTIME_SETTINGS */
const unpack_wds_SLQSGetRuntimeSettings_t const_unpack_wds_SLQSGetRuntimeSettings_t = {
        0x0aa6eaf9,{0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66},0,
        {0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65},0x0a1e1785,0x0a1e1786,
        {0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0},{0},
        0,{0,1},
        0x0aa6eafa,0xfffffffc,
        0,{0,{0}},{0,{{0,{0}}}},
        {0},{0},
        0x000005dc,{0,{{0,{0}}}},
        4,0,0,
        {0,{0}},{0,{0}},
        {{SWI_UINT256_BIT_VALUE(SET_17_BITS,2,16,17,20,21,22,29,30,31,32,33,34,35,36,41,42,43)}} };

/* eQMI_WDS_SET_EVENT */
const unpack_wds_SLQSSetWdsEventCallback_t const_unpack_wds_SLQSSetWdsEventCallback_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };


/* eQMI_WDS_CREATE_PROFILE */
PackCreateProfileOut var_ProfileInfo;
unpack_wds_SLQSCreateProfile_t var_unpack_wds_SLQSCreateProfile_t = {
       &var_ProfileInfo,NULL,0,{{0}} };

PackCreateProfileOut cst_ProfileInfo = {0x11,0x21,0xe2e1};
const unpack_wds_SLQSCreateProfile_t const_unpack_wds_SLQSCreateProfile_t = {
       &cst_ProfileInfo,NULL,0,
       {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,224)}} };

/* eQMI_WDS_MODIFY_PROFILE */
uint16_t var_extcode;
unpack_wds_SLQSModifyProfile_t var_unpack_wds_SLQSModifyProfile_t = {
       &var_extcode,{{0}} };

uint16_t cst_extcode = 0xe2e1;
const unpack_wds_SLQSModifyProfile_t const_unpack_wds_SLQSModifyProfile_t = {
       &cst_extcode,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,224)}} };

/* eQMI_WDS_GET_PROFILE */
uint8_t var_unPackGetProfileSettingOutProfilenameResp[255]={0};
uint16_t var_unPackGetProfileSettingOutProfilenameSizeResp = 255;
uint8_t var_unPackGetProfileSettingOutAPNNameResp[255]={0};
uint16_t var_unPackGetProfileSettingOutAPNNameSizeResp=255;
uint8_t var_unPackGetProfileSettingDPtypeResp;

uint8_t var_unPackGetProfileSettingOutPasswordResp[255]={0};
uint16_t var_unPackGetProfileSettingOutPasswordSizeResp=255;
uint8_t var_unPackGetProfileSettingOutUserResp[255]={0};
uint16_t var_unPackGetProfileSettingOutUserSizeResp=255;
uint32_t var_unPackGetProfileSettingOutIPv4AddrPrefResp=0;
uint32_t var_unPackGetProfileSettingOutDNSPriAddrPrefResp=0;
uint32_t var_unPackGetProfileSettingOutDNSSecAddrPrefResp=0;
uint8_t var_unPackGetProfileSettingOutAuthenticationPrefResp=0;
uint8_t var_unPackGetProfileSettingOutAPNClassResp=0;
uint8_t var_unPackGetProfileSettingOutEmergencyCallResp=0;

uint8_t var_unPackGetProfileSettingOutPdpHdrCompTypeResp=0;
uint8_t var_unPackGetProfileSettingOutPdpDataCompTypeResp=0;
LibPackUMTSQoS          var_unPackGetProfileSettingOutUMTSReqQoS;
LibPackUMTSQoS          var_unPackGetProfileSettingOutUMTSMinQoS;
LibPackGPRSRequestedQoS var_unPackGetProfileSettingOutGPRSRequestedQos;
LibPackGPRSRequestedQoS var_unPackGetProfileSettingOutGPRSMinimumQoS;
uint8_t var_unPackGetProfileSettingOutPcscfAddrUsingPCO=0;

uint8_t                    var_unPackGetProfileSettingOutPdpAccessConFlag;
uint8_t                    var_unPackGetProfileSettingOutPcscfAddrUsingDhcp;
uint8_t                    var_unPackGetProfileSettingOutImCnFlag;
uint16_t var_TFTID1SourceIP[WDS_TFTID_SOURCE_IP_SIZE];
uint16_t var_TFTID2SourceIP[WDS_TFTID_SOURCE_IP_SIZE];

LibPackTFTIDParams      var_unPackGetProfileSettingOutTFTID1Params = {
    0,//uint8_t  filterId;
    0,//uint8_t  eValid;
    0,//uint8_t  ipVersion;
    &var_TFTID1SourceIP[0],//uint16_t  *pSourceIP;
    0,//uint8_t  sourceIPMask;
    0,//uint8_t  nextHeader;
    0,//uint32_t  destPortRangeStart;
    0,//uint16_t  destPortRangeEnd;
    0,//uint16_t  srcPortRangeStart;
    0,//uint16_t  srcPortRangeEnd;
    0,//uint32_t IPSECSPI;
    0,//uint16_t  tosMask;
    0,//uint32_t flowLabel;
};
LibPackTFTIDParams      var_unPackGetProfileSettingOutTFTID2Params= {
    0,//uint8_t  filterId;
    0,//uint8_t  eValid;
    0,//uint8_t  ipVersion;
    &var_TFTID2SourceIP[0],//uint16_t  *pSourceIP;
    0,//uint8_t  sourceIPMask;
    0,//uint8_t  nextHeader;
    0,//uint32_t  destPortRangeStart;
    0,//uint16_t  destPortRangeEnd;
    0,//uint16_t  srcPortRangeStart;
    0,//uint16_t  srcPortRangeEnd;
    0,//uint32_t IPSECSPI;
    0,//uint16_t  tosMask;
    0,//uint32_t flowLabel;
};
uint8_t                    var_unPackGetProfileSettingOutPdpContext;
uint8_t                    var_unPackGetProfileSettingOutSecondaryFlag;
uint8_t                    var_unPackGetProfileSettingOutPrimaryID;
uint16_t                  var_unPackGetProfileSettingOutIPv6AddPref[8];
LibPackUMTSReqQoSSigInd var_unPackGetProfileSettingOutUMTSReqQoSSigInd;
LibPackUMTSReqQoSSigInd var_unPackGetProfileSettingOutUMTSMinQosSigInd;
uint16_t                  var_unPackGetProfileSettingOutPriDNSIPv6addpref;
uint16_t                  var_unPackGetProfileSettingOutSecDNSIPv6addpref;
uint8_t                    var_unPackGetProfileSettingOutAddrAllocPref;
LibPackQosClassID       var_unPackGetProfileSettingOutQosClassID;
uint8_t                    var_unPackGetProfileSettingOutAPNDisabledFlag;
uint32_t                   var_unPackGetProfileSettingOutPDNInactivTimeout;

uint16_t                    var_unPackGetProfileSettingOperatorPCOID;
uint16_t                    var_unPackGetProfileSettingMcc;
LibPackProfileMnc           var_unPackGetProfileSettingMnc;
uint16_t                    var_unPackGetProfileSettingMaxPDN;
uint16_t                    var_unPackGetProfileSettingMaxPDNTimer;
uint16_t                    var_unPackGetProfileSettingPDNWaitTimer;
uint32_t                    var_unPackGetProfileSettingAppUserData;
uint8_t                     var_unPackGetProfileSettingRoamDisallowFlag;
uint8_t                     var_unPackGetProfileSettingPDNDisconnectWaitTimer;
uint8_t                     var_unPackGetProfileSettingDnsWithDHCPFlag;
uint32_t                    var_unPackGetProfileSettingLteRoamPDPType;
uint32_t                    var_unPackGetProfileSettingUmtsRoamPDPType;
LibPackPDNThrottleTimer     var_unPackGetProfileSettingPDNThrottleTimer;
uint32_t                    var_unPackGetProfileSettingOverridePDPType;
uint8_t                     var_unPackGetProfileSettingOutClatFlag;
uint8_t                     var_unPackGetProfileSettingOutIPV6DelegFlag;
uint64_t                    var_unPackGetProfileSettingOutAPNBearer;
uint8_t                     var_unPackGetProfileSettingOutIWLANtoLTEHandoverFlag;
uint8_t                     var_unPackGetProfileSettingOutLTEtoIWLANHandoverFlag;
LibPackPCOIDList            var_unPackGetProfileSettingOutPCOIDList;
uint8_t                     var_unPackGetProfileSettingOutMsisdnFlag;
uint8_t                     var_unPackGetProfileSettingOutPersistFlag;


uint16_t var_unPackGetProfileSettingExtErrCodeResp;

UnPackGetProfileSettingOut var_ProfileSettings = {
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        var_unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &var_unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &var_unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &var_unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &var_unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        var_unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &var_unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &var_unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &var_unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &var_unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &var_unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &var_unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &var_unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        var_unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &var_unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        var_unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &var_unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &var_unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &var_unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &var_unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &var_unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &var_unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &var_unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &var_unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &var_unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &var_unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &var_unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &var_unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        var_unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &var_unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &var_unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &var_unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &var_unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &var_unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &var_unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &var_unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &var_unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &var_unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &var_unPackGetProfileSettingOutEmergencyCallResp //uint8_t                     *pSupportEmergencyCalls
        }
    },
    &var_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 var_ProfileSettingsV2 = {
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        var_unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &var_unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &var_unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &var_unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &var_unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        var_unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &var_unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &var_unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &var_unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &var_unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &var_unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &var_unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &var_unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        var_unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &var_unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        var_unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &var_unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &var_unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &var_unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &var_unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &var_unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &var_unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &var_unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &var_unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &var_unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &var_unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &var_unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &var_unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        var_unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &var_unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &var_unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &var_unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &var_unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &var_unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &var_unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &var_unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &var_unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &var_unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &var_unPackGetProfileSettingOutAPNBearer,
        &var_unPackGetProfileSettingOutEmergencyCallResp, //uint8_t                     *pSupportEmergencyCalls
        &var_unPackGetProfileSettingOperatorPCOID, //uint16_t                    *pOperatorPCOID
        &var_unPackGetProfileSettingMcc, //uint16_t                    *pMcc
        &var_unPackGetProfileSettingMnc, //LibPackProfileMnc           *pMnc;
        &var_unPackGetProfileSettingMaxPDN, //uint16_t                    *pMaxPDN
        &var_unPackGetProfileSettingMaxPDNTimer, //uint16_t                    *pMaxPDNWaitTimer
        &var_unPackGetProfileSettingPDNWaitTimer, //uint16_t                    *pMaxPDNTimer
        &var_unPackGetProfileSettingAppUserData, //uint32_t                   *pAppUserData
        &var_unPackGetProfileSettingRoamDisallowFlag, //uint8_t                    *pRoamDisallowFlag
        &var_unPackGetProfileSettingPDNDisconnectWaitTimer, //uint8_t                    *pPDNDisconnectWaitTimer
        &var_unPackGetProfileSettingDnsWithDHCPFlag, //uint8_t                    *pDnsWithDHCPFlag
        &var_unPackGetProfileSettingLteRoamPDPType, //uint32_t                   *pLteRoamPDPType
        &var_unPackGetProfileSettingUmtsRoamPDPType, //uint32_t                   *pUmtsRoamPDPType
        &var_unPackGetProfileSettingOutIWLANtoLTEHandoverFlag,
        &var_unPackGetProfileSettingOutLTEtoIWLANHandoverFlag,
        &var_unPackGetProfileSettingPDNThrottleTimer, //LibPackPDNThrottleTimer    *pPDNThrottleTimer
        &var_unPackGetProfileSettingOverridePDPType, //uint32_t                   *pOverridePDPType
        &var_unPackGetProfileSettingOutPCOIDList,
        &var_unPackGetProfileSettingOutMsisdnFlag,
        &var_unPackGetProfileSettingOutPersistFlag,
        &var_unPackGetProfileSettingOutClatFlag,
        &var_unPackGetProfileSettingOutIPV6DelegFlag,
        }
    },
    &var_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

uint8_t   var_unPackGetProfileSettingOutNegoDnsSrvrPref;
uint32_t  var_unPackGetProfileSettingOutPppSessCloseTimerDO;
uint32_t  var_unPackGetProfileSettingOutPppSessCloseTimer1x;
uint8_t   var_unPackGetProfileSettingOutAllowLinger;
uint16_t  var_unPackGetProfileSettingOutLcpAckTimeout;
uint16_t  var_unPackGetProfileSettingOutIpcpAckTimeout;
uint16_t  var_unPackGetProfileSettingOutAuthTimeout;
uint8_t   var_unPackGetProfileSettingOutLcpCreqRetryCount;
uint8_t   var_unPackGetProfileSettingOutIpcpCreqRetryCount;
uint8_t   var_unPackGetProfileSettingOutAuthRetryCount;
uint8_t   var_unPackGetProfileSettingOutAuthProtocol;
uint8_t   var_unPackGetProfileSettingOutUserId[255];
uint16_t  var_unPackGetProfileSettingOutUserIdSize = 255;
uint8_t   var_unPackGetProfileSettingOutAuthPassword[255];
uint16_t  var_unPackGetProfileSettingOutAuthPasswordSize = 255;
uint8_t   var_unPackGetProfileSettingOutDataRate;
uint32_t  var_unPackGetProfileSettingOutAppType;
uint8_t   var_unPackGetProfileSettingOutDataMode;
uint8_t   var_unPackGetProfileSettingOutAppPriority;
uint8_t   var_unPackGetProfileSettingOutApnString[255];
uint16_t  var_unPackGetProfileSettingOutApnStringSize = 255;
uint8_t   var_unPackGetProfileSettingOutPdnType;
uint8_t   var_unPackGetProfileSettingOutIsPcscfAddressNedded;
uint32_t  var_unPackGetProfileSettingOutPrimaryV4DnsAddress;
uint32_t  var_unPackGetProfileSettingOutSecondaryV4DnsAddress;
uint16_t  var_unPackGetProfileSettingOutPriV6DnsAddress[8];
uint16_t  var_unPackGetProfileSettingOutSecV6DnsAddress[8];
uint8_t   var_unPackGetProfileSettingOutRATType;
uint8_t   var_unPackGetProfileSettingOutAPNEnabled3GPP2;
uint32_t  var_unPackGetProfileSettingOutPDNInactivTimeout3GPP2;
uint8_t   var_unPackGetProfileSettingOutAPNClass3GPP2;

UnPackGetProfileSettingOut var_ProfileSettings2={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&var_unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&var_unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&var_unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&var_unPackGetProfileSettingOutAllowLinger,
        (void*)&var_unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&var_unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&var_unPackGetProfileSettingOutAuthTimeout,
        (void*)&var_unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&var_unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&var_unPackGetProfileSettingOutAuthRetryCount,
        (void*)&var_unPackGetProfileSettingOutAuthProtocol,
        (void*)var_unPackGetProfileSettingOutUserId,
        (void*)&var_unPackGetProfileSettingOutUserIdSize,
        (void*)var_unPackGetProfileSettingOutAuthPassword,
        (void*)&var_unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&var_unPackGetProfileSettingOutDataRate,
        (void*)&var_unPackGetProfileSettingOutAppType,
        (void*)&var_unPackGetProfileSettingOutDataMode,
        (void*)&var_unPackGetProfileSettingOutAppPriority,
        (void*)var_unPackGetProfileSettingOutApnString,
        (void*)&var_unPackGetProfileSettingOutApnStringSize,
        (void*)&var_unPackGetProfileSettingOutPdnType,
        (void*)&var_unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)&var_unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)&var_unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)var_unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)var_unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&var_unPackGetProfileSettingOutRATType,
        (void*)&var_unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&var_unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&var_unPackGetProfileSettingOutAPNClass3GPP2,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
        }
    },
        &var_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 var_ProfileSettings2V2={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&var_unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&var_unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&var_unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&var_unPackGetProfileSettingOutAllowLinger,
        (void*)&var_unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&var_unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&var_unPackGetProfileSettingOutAuthTimeout,
        (void*)&var_unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&var_unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&var_unPackGetProfileSettingOutAuthRetryCount,
        (void*)&var_unPackGetProfileSettingOutAuthProtocol,
        (void*)var_unPackGetProfileSettingOutUserId,
        (void*)&var_unPackGetProfileSettingOutUserIdSize,
        (void*)var_unPackGetProfileSettingOutAuthPassword,
        (void*)&var_unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&var_unPackGetProfileSettingOutDataRate,
        (void*)&var_unPackGetProfileSettingOutAppType,
        (void*)&var_unPackGetProfileSettingOutDataMode,
        (void*)&var_unPackGetProfileSettingOutAppPriority,
        (void*)var_unPackGetProfileSettingOutApnString,
        (void*)&var_unPackGetProfileSettingOutApnStringSize,
        (void*)&var_unPackGetProfileSettingOutPdnType,
        (void*)&var_unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)&var_unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)&var_unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)var_unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)var_unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&var_unPackGetProfileSettingOutRATType,
        (void*)&var_unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&var_unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&var_unPackGetProfileSettingOutAPNClass3GPP2,
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
        }
    },
        &var_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

unpack_wds_SLQSGetProfileSettings_t var_unpack_wds_SLQSGetProfileSettings_t[2] = {
       {
           &var_ProfileSettings,0,0,{{0}} 
       },
       {
           &var_ProfileSettings2,1,0,{{0}} 
       }};

unpack_wds_SLQSGetProfileSettingsV2_t var_unpack_wds_SLQSGetProfileSettingsV2_t[2] = {
       {
           &var_ProfileSettingsV2,0,0,{{0}} 
       },
       {
           &var_ProfileSettings2V2,1,0,{{0}} 
       }};

uint8_t cst_unPackGetProfileSettingOutProfilenameResp[255]=
           {0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66};
uint16_t cst_unPackGetProfileSettingOutProfilenameSizeResp = 255;
uint8_t cst_unPackGetProfileSettingOutAPNNameResp[255]={0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65};
uint16_t cst_unPackGetProfileSettingOutAPNNameSizeResp=255;;
uint8_t cst_unPackGetProfileSettingDPtypeResp = 0;

uint8_t cst_unPackGetProfileSettingOutPasswordResp[255]={0};
uint16_t cst_unPackGetProfileSettingOutPasswordSizeResp=255;
uint8_t cst_unPackGetProfileSettingOutUserResp[255]=
                     {0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65};
uint16_t cst_unPackGetProfileSettingOutUserSizeResp=255;
uint32_t cst_unPackGetProfileSettingOutIPv4AddrPrefResp=0x0aa6eaf9;
uint32_t cst_unPackGetProfileSettingOutDNSPriAddrPrefResp=0x0a1e1785;
uint32_t cst_unPackGetProfileSettingOutDNSSecAddrPrefResp=0x0a1e1786;
uint8_t cst_unPackGetProfileSettingOutAuthenticationPrefResp=0;
uint8_t cst_unPackGetProfileSettingOutAPNClassResp=0;
uint8_t cst_unPackGetProfileSettingOutEmergencyCallResp=0;

uint8_t cst_unPackGetProfileSettingOutPdpHdrCompTypeResp=1;
uint8_t cst_unPackGetProfileSettingOutPdpDataCompTypeResp=1;

LibPackUMTSQoS          cst_unPackGetProfileSettingOutUMTSReqQoS = 
          {0x17,0x17201918,0x17201918,0x17201918,0x17201918,0x18,0x18172019,0x19,0x20,0x17,
           0x17201918,0x17201918};

LibPackUMTSQoS          cst_unPackGetProfileSettingOutUMTSMinQoS;
LibPackGPRSRequestedQoS cst_unPackGetProfileSettingOutGPRSRequestedQos =
        {0x1c1b1a19,0x1c1b1a19,0x1c1b1a19,0x1c1b1a19,0x1c1b1a19};

LibPackGPRSRequestedQoS cst_unPackGetProfileSettingOutGPRSMinimumQoS;
uint8_t cst_unPackGetProfileSettingOutPcscfAddrUsingPCO=0;

uint8_t                    cst_unPackGetProfileSettingOutPdpAccessConFlag;
uint8_t                    cst_unPackGetProfileSettingOutPcscfAddrUsingDhcp;
uint8_t                    cst_unPackGetProfileSettingOutImCnFlag;
uint16_t cst_TFTID1SourceIP[WDS_TFTID_SOURCE_IP_SIZE];
uint16_t cst_TFTID2SourceIP[WDS_TFTID_SOURCE_IP_SIZE] = 
          {0x2122, 0x2324, 0x2526, 0x2728,
           0x292a, 0x2b2c ,0x2d2e, 0x2f30};

LibPackTFTIDParams      cst_unPackGetProfileSettingOutTFTID1Params = {
    0,//uint8_t  filterId;
    0,//uint8_t  eValid;
    0,//uint8_t  ipVersion;
    &cst_TFTID1SourceIP[0],//uint16_t  *pSourceIP;
    0,//uint8_t  sourceIPMask;
    0,//uint8_t  nextHeader;
    0,//uint16_t  destPortRangeStart;
    0,//uint16_t  destPortRangeEnd;
    0,//uint16_t  srcPortRangeStart;
    0,//uint16_t  srcPortRangeEnd;
    0,//uint32_t IPSECSPI;
    0,//uint16_t  tosMask;
    0,//uint32_t flowLabel;
};
LibPackTFTIDParams      cst_unPackGetProfileSettingOutTFTID2Params= {
    1,//uint8_t  filterId;
    2,//uint8_t  eValid;
    6,//uint8_t  ipVersion;
    &cst_TFTID2SourceIP[0],//uint16_t  *pSourceIP;
    0x31,//uint8_t  sourceIPMask;
    0x32,//uint8_t  nextHeader;
    0x3433,//uint16_t  destPortRangeStart;
    0x3635,//uint16_t  destPortRangeEnd;
    0x3837,//uint16_t  srcPortRangeStart;
    0x3a39,//uint16_t  srcPortRangeEnd;
    0x3e3d3c3b,//uint32_t IPSECSPI;
    0x403f,//uint16_t  tosMask;
    0x44434241,//uint32_t flowLabel;
};
uint8_t                    cst_unPackGetProfileSettingOutPdpContext = 0x25;
uint8_t                    cst_unPackGetProfileSettingOutSecondaryFlag = 1;
uint8_t                    cst_unPackGetProfileSettingOutPrimaryID = 0x27;
uint16_t                  cst_unPackGetProfileSettingOutIPv6AddPref[8] = 
             {0x2829, 0x2a2b, 0x2c2d, 0x2e2f, 0x3031, 0x3233, 0x3435, 0x3637};

LibPackUMTSReqQoSSigInd cst_unPackGetProfileSettingOutUMTSReqQoSSigInd;
LibPackUMTSReqQoSSigInd cst_unPackGetProfileSettingOutUMTSMinQosSigInd;
uint16_t                  cst_unPackGetProfileSettingOutPriDNSIPv6addpref;
uint16_t                  cst_unPackGetProfileSettingOutSecDNSIPv6addpref;
uint8_t                    cst_unPackGetProfileSettingOutAddrAllocPref;
LibPackQosClassID       cst_unPackGetProfileSettingOutQosClassID;
uint8_t                    cst_unPackGetProfileSettingOutAPNDisabledFlag;
uint32_t                   cst_unPackGetProfileSettingOutPDNInactivTimeout;

uint16_t                    cst_unPackGetProfileSettingOperatorPCOID = 0;
uint16_t                    cst_unPackGetProfileSettingMcc = 0;
LibPackProfileMnc           cst_unPackGetProfileSettingMnc = 
      {0,0};
uint16_t                    cst_unPackGetProfileSettingMaxPDN = 0x0014;
uint16_t                    cst_unPackGetProfileSettingMaxPDNTimer = 0x012c;
uint16_t                    cst_unPackGetProfileSettingPDNWaitTimer = 0;
uint32_t                    cst_unPackGetProfileSettingAppUserData = 0x00000001;
uint8_t                     cst_unPackGetProfileSettingRoamDisallowFlag = 0x01;
uint8_t                     cst_unPackGetProfileSettingPDNDisconnectWaitTimer = 0x01;
uint8_t                     cst_unPackGetProfileSettingDnsWithDHCPFlag = 0x01;
uint32_t                    cst_unPackGetProfileSettingLteRoamPDPType = 0x000000ff;
uint32_t                    cst_unPackGetProfileSettingUmtsRoamPDPType = 0x000000ff;
LibPackPDNThrottleTimer     cst_unPackGetProfileSettingPDNThrottleTimer = 
                {{0}};
uint32_t                    cst_unPackGetProfileSettingOverridePDPType = 0x000000ff;;
uint8_t                     cst_unPackGetProfileSettingOutClatFlag = 0;
uint8_t                     cst_unPackGetProfileSettingOutIPV6DelegFlag = 0;
uint64_t                    cst_unPackGetProfileSettingOutAPNBearer = 0x8000000000000000ULL;
uint8_t                     cst_unPackGetProfileSettingOutIWLANtoLTEHandoverFlag = 0x01;
uint8_t                     cst_unPackGetProfileSettingOutLTEtoIWLANHandoverFlag = 0x01;
LibPackPCOIDList            cst_unPackGetProfileSettingOutPCOIDList =
 {{0}};
uint8_t                     cst_unPackGetProfileSettingOutMsisdnFlag = 0;
uint8_t                     cst_unPackGetProfileSettingOutPersistFlag = 0x01;

uint16_t cst_unPackGetProfileSettingExtErrCodeResp;

UnPackGetProfileSettingOut cst_ProfileSettings = {
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        cst_unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &cst_unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &cst_unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &cst_unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &cst_unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        cst_unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &cst_unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &cst_unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &cst_unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &cst_unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &cst_unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &cst_unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &cst_unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        cst_unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &cst_unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        cst_unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &cst_unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &cst_unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &cst_unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &cst_unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &cst_unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &cst_unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &cst_unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &cst_unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &cst_unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &cst_unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &cst_unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &cst_unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        cst_unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &cst_unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &cst_unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &cst_unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &cst_unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &cst_unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &cst_unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &cst_unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &cst_unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &cst_unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &cst_unPackGetProfileSettingOutEmergencyCallResp //uint8_t                     *pSupportEmergencyCalls
        }
    },
    &cst_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 cst_ProfileSettingsV2 = {
    { //UnpackQmiProfileInfo
        { //LibpackProfile3GPP
        cst_unPackGetProfileSettingOutProfilenameResp, //uint8_t                    *pProfilename;
        &cst_unPackGetProfileSettingOutProfilenameSizeResp, //uint16_t                    *pProfilenameSize;
        &cst_unPackGetProfileSettingDPtypeResp, // uint8_t                    *pPDPtype;
        &cst_unPackGetProfileSettingOutPdpHdrCompTypeResp, // uint8_t                    *pPdpHdrCompType;
        &cst_unPackGetProfileSettingOutPdpDataCompTypeResp, // uint8_t                    *pPdpDataCompType;
        cst_unPackGetProfileSettingOutAPNNameResp, //uint8_t                    *pAPNName;
        &cst_unPackGetProfileSettingOutAPNNameSizeResp, //uint16_t                    *pAPNnameSize;
        &cst_unPackGetProfileSettingOutDNSPriAddrPrefResp, // uint32_t                   *pPriDNSIPv4AddPref;
        &cst_unPackGetProfileSettingOutDNSSecAddrPrefResp, // uint32_t                   *pSecDNSIPv4AddPref;
        &cst_unPackGetProfileSettingOutUMTSReqQoS, // LibPackUMTSQoS          *pUMTSReqQoS;
        &cst_unPackGetProfileSettingOutUMTSMinQoS, // LibPackUMTSQoS          *pUMTSMinQoS;
        &cst_unPackGetProfileSettingOutGPRSRequestedQos, // LibPackGPRSRequestedQoS *pGPRSRequestedQos;
        &cst_unPackGetProfileSettingOutGPRSMinimumQoS, // LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
        cst_unPackGetProfileSettingOutUserResp, // uint8_t                    *pUsername;
        &cst_unPackGetProfileSettingOutUserSizeResp, // uint16_t                    *pUsernameSize;
        cst_unPackGetProfileSettingOutPasswordResp, // uint8_t                    *pPassword;
        &cst_unPackGetProfileSettingOutPasswordSizeResp, // uint16_t                    *pPasswordSize;
        &cst_unPackGetProfileSettingOutAuthenticationPrefResp, // uint8_t                    *pAuthenticationPref;
        &cst_unPackGetProfileSettingOutIPv4AddrPrefResp, // uint32_t                   *pIPv4AddrPref;
        &cst_unPackGetProfileSettingOutPcscfAddrUsingPCO, // uint8_t                    *pPcscfAddrUsingPCO;
        &cst_unPackGetProfileSettingOutPdpAccessConFlag, // uint8_t                    *pPdpAccessConFlag;
        &cst_unPackGetProfileSettingOutPcscfAddrUsingDhcp, // uint8_t                    *pPcscfAddrUsingDhcp;
        &cst_unPackGetProfileSettingOutImCnFlag, // uint8_t                    *pImCnFlag;
        &cst_unPackGetProfileSettingOutTFTID1Params, // LibPackTFTIDParams      *pTFTID1Params;
        &cst_unPackGetProfileSettingOutTFTID2Params, // LibPackTFTIDParams      *pTFTID2Params;
        &cst_unPackGetProfileSettingOutPdpContext, // uint8_t                    *pPdpContext;
        &cst_unPackGetProfileSettingOutSecondaryFlag, // uint8_t                    *pSecondaryFlag;
        &cst_unPackGetProfileSettingOutPrimaryID, // uint8_t                    *pPrimaryID;
        cst_unPackGetProfileSettingOutIPv6AddPref, // uint16_t                  *pIPv6AddPref;
        &cst_unPackGetProfileSettingOutUMTSReqQoSSigInd, //LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
        &cst_unPackGetProfileSettingOutUMTSMinQosSigInd, // LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
        &cst_unPackGetProfileSettingOutPriDNSIPv6addpref, // uint16_t                  *pPriDNSIPv6addpref;
        &cst_unPackGetProfileSettingOutSecDNSIPv6addpref, // uint16_t                  *pSecDNSIPv6addpref;
        &cst_unPackGetProfileSettingOutAddrAllocPref, // uint8_t                    *pAddrAllocPref;
        &cst_unPackGetProfileSettingOutQosClassID, // LibPackQosClassID       *pQosClassID;
        &cst_unPackGetProfileSettingOutAPNDisabledFlag, // uint8_t                    *pAPNDisabledFlag;
        &cst_unPackGetProfileSettingOutPDNInactivTimeout, // uint32_t                   *pPDNInactivTimeout;
        &cst_unPackGetProfileSettingOutAPNClassResp, // uint8_t                    *pAPNClass;
        &cst_unPackGetProfileSettingOutAPNBearer,
        &cst_unPackGetProfileSettingOutEmergencyCallResp, //uint8_t                     *pSupportEmergencyCalls
        &cst_unPackGetProfileSettingOperatorPCOID, //uint16_t                    *pOperatorPCOID
        &cst_unPackGetProfileSettingMcc, //uint16_t                    *pMcc
        &cst_unPackGetProfileSettingMnc, //LibPackProfileMnc           *pMnc;
        &cst_unPackGetProfileSettingMaxPDN, //uint16_t                    *pMaxPDN
        &cst_unPackGetProfileSettingMaxPDNTimer, //uint16_t                    *pMaxPDNWaitTimer
        &cst_unPackGetProfileSettingPDNWaitTimer, //uint16_t                    *pMaxPDNTimer
        &cst_unPackGetProfileSettingAppUserData, //uint32_t                   *pAppUserData
        &cst_unPackGetProfileSettingRoamDisallowFlag, //uint8_t                    *pRoamDisallowFlag
        &cst_unPackGetProfileSettingPDNDisconnectWaitTimer, //uint8_t                    *pPDNDisconnectWaitTimer
        &cst_unPackGetProfileSettingDnsWithDHCPFlag, //uint8_t                    *pDnsWithDHCPFlag
        &cst_unPackGetProfileSettingLteRoamPDPType, //uint32_t                   *pLteRoamPDPType
        &cst_unPackGetProfileSettingUmtsRoamPDPType, //uint32_t                   *pUmtsRoamPDPType
        &cst_unPackGetProfileSettingOutIWLANtoLTEHandoverFlag,
        &cst_unPackGetProfileSettingOutLTEtoIWLANHandoverFlag,
        &cst_unPackGetProfileSettingPDNThrottleTimer, //LibPackPDNThrottleTimer    *pPDNThrottleTimer
        &cst_unPackGetProfileSettingOverridePDPType, //uint32_t                   *pOverridePDPType
        &cst_unPackGetProfileSettingOutPCOIDList,
        &cst_unPackGetProfileSettingOutMsisdnFlag,
        &cst_unPackGetProfileSettingOutPersistFlag,
        &cst_unPackGetProfileSettingOutClatFlag,
        &cst_unPackGetProfileSettingOutIPV6DelegFlag,
        }
    },
    &cst_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

uint8_t   cst_unPackGetProfileSettingOutNegoDnsSrvrPref = 0x90;
uint32_t  cst_unPackGetProfileSettingOutPppSessCloseTimerDO = 0x94939291;
uint32_t  cst_unPackGetProfileSettingOutPppSessCloseTimer1x = 0x95949392;
uint8_t   cst_unPackGetProfileSettingOutAllowLinger = 0x93;
uint16_t  cst_unPackGetProfileSettingOutLcpAckTimeout = 0x9594;
uint16_t  cst_unPackGetProfileSettingOutIpcpAckTimeout = 0x9695;
uint16_t  cst_unPackGetProfileSettingOutAuthTimeout = 0x9796;
uint8_t   cst_unPackGetProfileSettingOutLcpCreqRetryCount = 0x97;
uint8_t   cst_unPackGetProfileSettingOutIpcpCreqRetryCount = 0x98;
uint8_t   cst_unPackGetProfileSettingOutAuthRetryCount = 0x99;
uint8_t   cst_unPackGetProfileSettingOutAuthProtocol = 0x9a;
uint8_t   cst_unPackGetProfileSettingOutUserId[255] = 
               {0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D,0x65};
uint16_t  cst_unPackGetProfileSettingOutUserIdSize = 255;
uint8_t   cst_unPackGetProfileSettingOutAuthPassword[255] = 
              {0x50, 0x61, 0x73, 0x73, 0x43, 0x6F, 0x64, 0x65};
uint16_t  cst_unPackGetProfileSettingOutAuthPasswordSize = 255;
uint8_t   cst_unPackGetProfileSettingOutDataRate = 0x9d;
uint32_t  cst_unPackGetProfileSettingOutAppType = 0xa1a09f9e;
uint8_t   cst_unPackGetProfileSettingOutDataMode = 0x9f;
uint8_t   cst_unPackGetProfileSettingOutAppPriority = 0xa0;
uint8_t   cst_unPackGetProfileSettingOutApnString[255] = 
         {0x41, 0x70, 0x6E, 0x5F, 0x73, 0x74, 0x72,0x69, 0x6E, 0x67};
uint16_t  cst_unPackGetProfileSettingOutApnStringSize = 255;
uint8_t   cst_unPackGetProfileSettingOutPdnType = 0xa2;
uint8_t   cst_unPackGetProfileSettingOutIsPcscfAddressNedded = 0xa3;
uint32_t  cst_unPackGetProfileSettingOutPrimaryV4DnsAddress = 0xa7a6a5a4;
uint32_t  cst_unPackGetProfileSettingOutSecondaryV4DnsAddress = 0xa8a7a6a5;
uint16_t  cst_unPackGetProfileSettingOutPriV6DnsAddress[8] = 
       {0xa7a8, 0xa9aa, 0xabac, 0xadae, 0xafb0, 0xb1b2, 0xb3b4, 0xb5b6};
uint16_t  cst_unPackGetProfileSettingOutSecV6DnsAddress[8] = 
       {0xa7a8, 0xa9aa, 0xabac, 0xadae, 0xafb0, 0xb1b2, 0xb3b4, 0xb5b6};;
uint8_t   cst_unPackGetProfileSettingOutRATType;
uint8_t   cst_unPackGetProfileSettingOutAPNEnabled3GPP2;
uint32_t  cst_unPackGetProfileSettingOutPDNInactivTimeout3GPP2;
uint8_t   cst_unPackGetProfileSettingOutAPNClass3GPP2;


UnPackGetProfileSettingOut cst_ProfileSettings2={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&cst_unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&cst_unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&cst_unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&cst_unPackGetProfileSettingOutAllowLinger,
        (void*)&cst_unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&cst_unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&cst_unPackGetProfileSettingOutAuthTimeout,
        (void*)&cst_unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&cst_unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&cst_unPackGetProfileSettingOutAuthRetryCount,
        (void*)&cst_unPackGetProfileSettingOutAuthProtocol,
        (void*)cst_unPackGetProfileSettingOutUserId,
        (void*)&cst_unPackGetProfileSettingOutUserIdSize,
        (void*)cst_unPackGetProfileSettingOutAuthPassword,
        (void*)&cst_unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&cst_unPackGetProfileSettingOutDataRate,
        (void*)&cst_unPackGetProfileSettingOutAppType,
        (void*)&cst_unPackGetProfileSettingOutDataMode,
        (void*)&cst_unPackGetProfileSettingOutAppPriority,
        (void*)cst_unPackGetProfileSettingOutApnString,
        (void*)&cst_unPackGetProfileSettingOutApnStringSize,
        (void*)&cst_unPackGetProfileSettingOutPdnType,
        (void*)&cst_unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)&cst_unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)&cst_unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)cst_unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)cst_unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&cst_unPackGetProfileSettingOutRATType,
        (void*)&cst_unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&cst_unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&cst_unPackGetProfileSettingOutAPNClass3GPP2,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
        }
    },
        &cst_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

UnPackGetProfileSettingOutV2 cst_ProfileSettings2V2={
    { //UnpackQmiProfileInfo
        {//LibpackProfile3GPP2
        (void*)&cst_unPackGetProfileSettingOutNegoDnsSrvrPref,
        (void*)&cst_unPackGetProfileSettingOutPppSessCloseTimerDO,
        (void*)&cst_unPackGetProfileSettingOutPppSessCloseTimer1x,
        (void*)&cst_unPackGetProfileSettingOutAllowLinger,
        (void*)&cst_unPackGetProfileSettingOutLcpAckTimeout,
        (void*)&cst_unPackGetProfileSettingOutIpcpAckTimeout,
        (void*)&cst_unPackGetProfileSettingOutAuthTimeout,
        (void*)&cst_unPackGetProfileSettingOutLcpCreqRetryCount,
        (void*)&cst_unPackGetProfileSettingOutIpcpCreqRetryCount,
        (void*)&cst_unPackGetProfileSettingOutAuthRetryCount,
        (void*)&cst_unPackGetProfileSettingOutAuthProtocol,
        (void*)cst_unPackGetProfileSettingOutUserId,
        (void*)&cst_unPackGetProfileSettingOutUserIdSize,
        (void*)cst_unPackGetProfileSettingOutAuthPassword,
        (void*)&cst_unPackGetProfileSettingOutAuthPasswordSize,
        (void*)&cst_unPackGetProfileSettingOutDataRate,
        (void*)&cst_unPackGetProfileSettingOutAppType,
        (void*)&cst_unPackGetProfileSettingOutDataMode,
        (void*)&cst_unPackGetProfileSettingOutAppPriority,
        (void*)cst_unPackGetProfileSettingOutApnString,
        (void*)&cst_unPackGetProfileSettingOutApnStringSize,
        (void*)&cst_unPackGetProfileSettingOutPdnType,
        (void*)&cst_unPackGetProfileSettingOutIsPcscfAddressNedded,
        (void*)&cst_unPackGetProfileSettingOutPrimaryV4DnsAddress,
        (void*)&cst_unPackGetProfileSettingOutSecondaryV4DnsAddress,
        (void*)cst_unPackGetProfileSettingOutPriV6DnsAddress,
        (void*)cst_unPackGetProfileSettingOutSecV6DnsAddress,
        (void*)&cst_unPackGetProfileSettingOutRATType,
        (void*)&cst_unPackGetProfileSettingOutAPNEnabled3GPP2,
        (void*)&cst_unPackGetProfileSettingOutPDNInactivTimeout3GPP2,
        (void*)&cst_unPackGetProfileSettingOutAPNClass3GPP2,
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
        }
    },
        &cst_unPackGetProfileSettingExtErrCodeResp //pExtErrCode
};

const unpack_wds_SLQSGetProfileSettings_t const_unpack_wds_SLQSGetProfileSettings_t[2] = {
       {
           &cst_ProfileSettings,0,0,
           {{SWI_UINT256_BIT_VALUE(SET_41_BITS,2,16,17,18,19,20,21,22,23,25,27,29,30,36,37,38,39,40,
              53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,143,222,223)}} 
       },
       {
           &cst_ProfileSettings2,1,0,
           {{SWI_UINT256_BIT_VALUE(SET_25_BITS,2,144,145,146,147,148,149,150,151,152,153,154,155,156,
              157,158,159,160,161,162,163,164,165,166,167)}} 
       }};

const unpack_wds_SLQSGetProfileSettingsV2_t const_unpack_wds_SLQSGetProfileSettingsV2_t[2] = {
       {
           &cst_ProfileSettingsV2,0,0,
           {{SWI_UINT256_BIT_VALUE(SET_41_BITS,2,16,17,18,19,20,21,22,23,25,27,29,30,36,37,38,39,40,
              53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,143,222,223)}} 
       },
       {
           &cst_ProfileSettings2V2,1,0,
           {{SWI_UINT256_BIT_VALUE(SET_25_BITS,2,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167)}} 
       }};

/* eQMI_WDS_GET_PKT_STATUS */
const unpack_wds_GetSessionState_t const_unpack_wds_GetSessionState_t = {
        2,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_GET_DEFAULTS */
const unpack_wds_GetDefaultProfile_t const_unpack_wds_GetDefaultProfile_t = {
        0,0x0aa6eaf9,0x0a1e1785,0x0a1e1786,{0},{0},{0},0,
        255,{0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66},
        255,{0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65},
        255,{0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65},
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,20,21,22,27,28,29,30)}} };

const unpack_wds_GetDefaultProfileV2_t const_unpack_wds_GetDefaultProfileV2_t = {
        0,0x0aa6eaf9,0x0a1e1785,0x0a1e1786,{0},{0},{0},0,
        255,{0x61,0x6e,0x64,0x72,0x6f,0x69,0x64,0x5f,0x70,0x72,0x6f,0x66},
        255,{0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65},
        255,{0x55, 0x73, 0x65, 0x72, 0x4E, 0x61, 0x6D, 0x65},
        255,{0x73,0x6d,0x61,0x72,0x74,0x6f,0x6e,0x65},
        {{SWI_UINT256_BIT_VALUE(SET_10_BITS,2,16,17,20,21,22,27,28,29,30)}} };

/* eQMI_WDS_GET_RATES */
const unpack_wds_GetConnectionRate_t const_unpack_wds_GetConnectionRate_t = {
        1,2,3,4,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_GET_STATISTICS */
const unpack_wds_GetPacketStatus_t const_unpack_wds_GetPacketStatus_t = {
        0x499602d2,0x075bcd15,0x0dfb38d3,0x149aa440, 0x1b3a0c83,0x21d95522,
        0x112210f47de98115ULL,0x208d868861370ad2ULL,0x2ff8fb4ab8566c35ULL,0x3f6467dd95a83a14ULL,
        0x28776159,0x2f091180,
        {{SWI_UINT256_BIT_VALUE(SET_13_BITS,2,16,17,18,19,20,21,25,26,27,28,29,30)}} };

uint32_t    var_TXPacketSuccesses;
uint32_t    var_RXPacketSuccesses;
uint32_t    var_TXPacketErrors;
uint32_t    var_RXPacketErrors;
uint32_t    var_TXPacketOverflows;
uint32_t    var_RXPacketOverflows;
uint64_t    var_TXOkBytesCount;
uint64_t    var_RXOkBytesCount;
uint64_t    var_TXOKBytesLastCall;
uint64_t    var_RXOKBytesLastCall;
uint32_t    var_TXDroppedCount;
uint32_t    var_RXDroppedCount;

unpack_wds_GetPacketStatistics_t var_unpack_wds_GetPacketStatistics_t = {
        &var_TXPacketSuccesses,&var_RXPacketSuccesses,&var_TXPacketErrors,&var_RXPacketErrors,
        &var_TXPacketOverflows,&var_RXPacketOverflows,&var_TXOkBytesCount,&var_RXOkBytesCount,
        &var_TXOKBytesLastCall,&var_RXOKBytesLastCall,&var_TXDroppedCount, &var_RXDroppedCount,
        {{0}} };

uint32_t    cst_TXPacketSuccesses = 0x499602d2 ;
uint32_t    cst_RXPacketSuccesses = 0x075bcd15;
uint32_t    cst_TXPacketErrors = 0x0dfb38d3;
uint32_t    cst_RXPacketErrors = 0x149aa440;
uint32_t    cst_TXPacketOverflows = 0x1b3a0c83;
uint32_t    cst_RXPacketOverflows = 0x21d95522;
uint64_t    cst_TXOkBytesCount = 0x112210f47de98115ULL;
uint64_t    cst_RXOkBytesCount = 0x208d868861370ad2ULL;
uint64_t    cst_TXOKBytesLastCall = 0x2ff8fb4ab8566c35ULL;
uint64_t    cst_RXOKBytesLastCall = 0x3f6467dd95a83a14ULL;
uint32_t    cst_TXDroppedCount = 0x28776159;
uint32_t    cst_RXDroppedCount = 0x2f091180;

const unpack_wds_GetPacketStatistics_t const_unpack_wds_GetPacketStatistics_t = {
        &cst_TXPacketSuccesses,&cst_RXPacketSuccesses,&cst_TXPacketErrors,&cst_RXPacketErrors,
        &cst_TXPacketOverflows,&cst_RXPacketOverflows,&cst_TXOkBytesCount,&cst_RXOkBytesCount,
        &cst_TXOKBytesLastCall,&cst_RXOKBytesLastCall,&cst_TXDroppedCount, &cst_RXDroppedCount,
        {{SWI_UINT256_BIT_VALUE(SET_13_BITS,2,16,17,18,19,20,21,25,26,27,28,29,30)}} };

unpack_wds_GetByteTotals_t var_unpack_wds_GetByteTotals_t = {
       &var_TXOkBytesCount,&var_RXOkBytesCount,
       {{0}} };

const unpack_wds_GetByteTotals_t const_unpack_wds_GetByteTotals_t = {
       &cst_TXOkBytesCount,&cst_RXOkBytesCount,
       {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,25,26)}} };

/* eQMI_WDS_GET_DURATION */
const unpack_wds_GetSessionDuration_t const_unpack_wds_GetSessionDuration_t = {
        0x00000000499602d2ULL,{{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)}} };

uint64_t var_callActDur;
uint64_t var_lastcallActDur;
uint64_t var_lastcallDur;
unpack_wds_GetSessionDurationV2_t var_unpack_wds_GetSessionDurationV2_t = {
       0,&var_lastcallDur,&var_callActDur,&var_lastcallActDur,{{0}} };

uint64_t cst_callActDur = 0x00000000000016d1ULL;
uint64_t cst_lastcallActDur = 0x0000000000000001ULL;
uint64_t cst_lastcallDur =    0x0000000000000002ULL;
const unpack_wds_GetSessionDurationV2_t const_unpack_wds_GetSessionDurationV2_t = {
        0x00000000499602d2ULL,&cst_lastcallDur,&cst_callActDur,&cst_lastcallActDur,
        {{SWI_UINT256_BIT_VALUE(SET_5_BITS,1,2,16,17,18)}} };

/* eQMI_WDS_GET_DORMANCY */
const unpack_wds_GetDormancyState_t const_unpack_wds_GetDormancyState_t = {
        2,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_DELETE_PROFILE */
const unpack_wds_SLQSDeleteProfile_t const_unpack_wds_SLQSDeleteProfile_t = {
       0xe2e1,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,224)}} };

/* eQMI_WDS_SWI_SET_3GPP_CFG_ITEM */
const unpack_wds_SLQSSet3GPPConfigItem_t const_unpack_wds_SLQSSet3GPPConfigItem_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_GET_3GPP_CFG_ITEM */
const unpack_wds_SLQSGet3GPPConfigItem_t const_unpack_wds_SLQSGet3GPPConfigItem_t = {
        0x10e1,{0x0001,0x0002,0x0003,0x0004},
        1,7,{0x0001},1,
        {{SWI_UINT256_BIT_VALUE(SET_6_BITS,2,16,17,18,19,20)}} };

/* eQMI_WDS_GET_MIP */
const unpack_wds_GetMobileIP_t const_unpack_wds_GetMobileIP_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_GET_MIP_PROFILE */
const unpack_wds_GetMobileIPProfile_t const_unpack_wds_GetMobileIPProfile_t = {
        1,0x14131211,0x24232221,0x34333231,1,
        255,
        { 0x4d, 0x6f, 0x62, 0x69, 0x6c, 0x65, 0x49, 0x50, 0x50, 0x72,
        0x6f, 0x66, 0x69, 0x6c, 0x65, 0x4E, 0x41,0x49},
        0x64636261,0x74737271,0,0,
       {{SWI_UINT256_BIT_VALUE(SET_9_BITS,2,16,17,18,19,20,21,22,23)}} };

/* eQMI_WDS_GET_CURR_DATA_SYS_STAT */
const unpack_wds_SLQSGetCurrDataSystemStat_t const_unpack_wds_SLQSGetCurrDataSystemStat_t = {
        0,2,{{0,0,0},{1,0,0}},
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,16,)}} };

/* eQMI_WDS_GET_LAST_MIP_STATUS */
const unpack_wds_GetLastMobileIPError_t const_unpack_wds_GetLastMobileIPError_t = {
        2,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_SWI_RM_TRANSFER_STATISTICS */
const unpack_wds_RMSetTransferStatistics_t const_unpack_wds_RMSetTransferStatistics_t = {
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SET_MIP_PROFILE */
const unpack_wds_SetMobileIPProfile_t const_unpack_wds_SetMobileIPProfile_t = {
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_PDP_RUNTIME_SETTINGS */
const unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t const_unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t = {
        0x10,0x11,{0x69, 0x6e, 0x74,0x65, 0x72, 0x6e, 0x65, 0x74, 0x00},
        0x34333231,0x44434241,0x54535251,0x64636261,
        {0x10,{0x1112, 0x2122, 0x3132, 0x4142, 0x5152, 0x6162,0x7172, 0x8182}},
        {0x10,{0xF1F2, 0xE1E2, 0xD1D2, 0xC1C2, 0xB1B2, 0xA1A2, 0x9192, 0x0102}},
        {0x9192, 0xa1a2, 0xb1b2, 0xc1c2, 0xd1d2, 0xe1e2, 0xf1f2,0x1112},
        {0xa1a2,0xb1b2, 0xc1c2, 0xd1d2, 0xe1e2, 0xf1f2, 0x1112, 0x2122},
        0x499602d2,0x3ade68b1,
        {0xd1d2, 0xe1e2,0xf1a1, 0xa2b1, 0xb2c1, 0xc2f2, 0x1112, 0x2122},
        {0xe2f1, 0xa1a2, 0xb1b2, 0xc1c2, 0xd1d2, 0xe1f2, 0x1112, 0x2122},
        {{SWI_UINT256_BIT_VALUE(SET_16_BITS,2,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30)}} };

/* eQMI_WDS_GET_DUN_CALL_INFO */
const unpack_wds_SLQSGetDUNCallInfo_t const_unpack_wds_SLQSGetDUNCallInfo_t = {
        {2,0x0807060504030201ULL},0x0001,0x1120304050607080ULL,0x1020304050607080ULL,
        2,4,{0x04030201,0x14131211,0x24232221,0x34333231},
        0x1120304050607080ULL,0x1020304050607080ULL,0x0807060504030201ULL,4,
        {{SWI_UINT256_BIT_VALUE(SET_12_BITS,2,16,17,18,19,20,21,22,23,24,25,32)}} };

/* eQMI_WDS_GET_CUR_DATA_BEARER */
const unpack_wds_SLQSGetDataBearerTechnology_t const_unpack_wds_SLQSGetDataBearerTechnology_t = {
        0x03,{1,0x00000002,0x00000003},{4,0x00000005,0x00000006},
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,1,2,16)}} };

/* eQMI_WDS_SET_IP_FAMILY */
const unpack_wds_SLQSSetIPFamilyPreference_t const_unpack_wds_SLQSSetIPFamilyPreference_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SET_DEFAULT_PROFILE_NUM */
const unpack_wds_SetDefaultProfileNum_t const_unpack_wds_SetDefaultProfileNum_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_GET_DEFAULT_PROFILE_NUM */
const unpack_wds_GetDefaultProfileNum_t const_unpack_wds_GetDefaultProfileNum_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_SWI_GET_DHCPV4_CLIENT_CONFIG */
wdsDhcpv4HwConfig var_Dhcpv4HwConfig;
wdsDhcpv4Option   var_Dhcpv4Option[255];
wdsDhcpv4OptionList var_Dhcpv4OptionList = {0,&var_Dhcpv4Option[0]};
unpack_wds_SLQSSGetDHCPv4ClientConfig_t var_unpack_wds_SLQSSGetDHCPv4ClientConfig_t = {
       &var_Dhcpv4HwConfig,&var_Dhcpv4OptionList,{{0}} };

wdsDhcpv4HwConfig cst_Dhcpv4HwConfig = {0xA5,6,{0xfc, 0xca, 0x4c, 0x36, 0x5a, 0xa3}};
wdsDhcpv4Option   cst_Dhcpv4Option[255] = {{4,4,{0x41,0x42, 0x43, 0x44}},
    {3,3,{0x31,0x32, 0x33}},{1,1,{0x11}}, {6,6,{0x61,0x62, 0x63,0x64,0x65,0x66}},
    {8,8,{0x81,0x82, 0x83,0x84,0x85,0x86,0x87,0x88}}};

wdsDhcpv4OptionList cst_Dhcpv4OptionList = {5,&cst_Dhcpv4Option[0]};
const unpack_wds_SLQSSGetDHCPv4ClientConfig_t const_unpack_wds_SLQSSGetDHCPv4ClientConfig_t = {
        &cst_Dhcpv4HwConfig,&cst_Dhcpv4OptionList,
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_WDS_SWI_GET_CURRENT_CHANNEL_RATE */
const unpack_wds_SLQSGetCurrentChannelRate_t const_unpack_wds_SLQSGetCurrentChannelRate_t = {
        1,2,3,4,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_SWI_GET_DATA_LOOPBACK */
const unpack_wds_SLQSSGetLoopback_t const_unpack_wds_SLQSSGetLoopback_t = {
        2,6,{{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,16,17)}} };

/* eQMI_WDS_SWI_SET_DATA_LOOPBACK */
const unpack_wds_SLQSSSetLoopback_t const_unpack_wds_SLQSSSetLoopback_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_CHANGE */
const unpack_wds_DHCPv4ClientLeaseChange_t const_unpack_wds_DHCPv4ClientLeaseChange_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SET_MIP */
const unpack_wds_SetMobileIP_t const_unpack_wds_SetMobileIP_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* QMI_WDS_SET_MIP_PARAMS */
const unpack_wds_SetMobileIPParameters_t const_unpack_wds_SetMobileIPParameters_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SET_AUTOCONNECT */
const unpack_wds_SetAutoconnect_t const_unpack_wds_SetAutoconnect_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_GET_AUTOCONNECT */
uint32_t var_enable;
unpack_wds_GetAutoconnect_t var_unpack_wds_GetAutoconnect_t = {
       &var_enable,{{0}} };

uint32_t cst_enable = 1;
const unpack_wds_GetAutoconnect_t const_unpack_wds_GetAutoconnect_t = {
       &cst_enable,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_G0_DORMANT */
const unpack_wds_SLQSWdsGoDormant_t const_unpack_wds_SLQSWdsGoDormant_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_G0_ACTIVE */
const unpack_wds_SLQSWdsGoActive_t const_unpack_wds_SLQSWdsGoActive_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_RESET_PKT_STATISTICS */
const unpack_wds_SLQSResetPacketStatics_t const_unpack_wds_SLQSResetPacketStatics_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_SET_DHCPV4_CLIENT_CONFIG */
const unpack_wds_SLQSSSetDHCPv4ClientConfig_t const_unpack_wds_SLQSSSetDHCPv4ClientConfig_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_GET_DATA_BEARER */
uint32_t var_databearer;
unpack_wds_GetDataBearerTechnology_t var_unpack_wds_GetDataBearerTechnology_t = {
       &var_databearer,{{0}} };

uint32_t cst_databearer = 4;
const unpack_wds_GetDataBearerTechnology_t const_unpack_wds_GetDataBearerTechnology_t = {
       &cst_databearer,
       {{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_WDS_SET_QMUX_ID */
const unpack_wds_SetMuxID_t const_unpack_wds_SetMuxID_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_PROFILE_CHANGE */
const unpack_wds_SLQSSwiProfileChangeCallback_t const_unpack_wds_SLQSwiProfileChangeCallback_t = {
        0,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_WDS_SWI_PROFILE_CHANGE_IND */
const unpack_wds_SLQSSwiProfileChangeCallback_Ind_t const_unpack_wds_SLQSwiProfileChangeCallback_Ind_t = {
        {0x01,0x05},{0x02},{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };


int wds_validate_dummy_unpack()
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
    loopCount = sizeof(validate_wds_resp_msg)/sizeof(validate_wds_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_wds_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_WDS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("WDS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("WDS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_WDS_GET_PKT_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetSessionState,
                    dump_GetSessionState,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetSessionState_t);

                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSetPacketSrvStatusCallback,
                    dump_wds_SLQSSetPacketSrvStatusCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSetPacketSrvStatusCallback_t);
                }
                break;
            case eQMI_WDS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSetWdsEventCallback,
                    dump_wds_SLQSSetWdsEventCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSetWdsEventCallback_t);

                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSetWdsEventCallback_ind,
                    dump_wds_SLQSSetWdsEventCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSetWdsEventCallback_ind_t);
                }
                break;
            case eQMI_WDS_GET_DUN_CALL_INFO:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetDUNCallInfo,
                    dump_wds_SLQSGetDUNCallInfo,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGetDUNCallInfo_t);

                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSDUNCallInfoCallBack_ind,
                    dump_wds_SLQSDUNCallInfoCallBack_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSDUNCallInfoCallBack_ind_t);
                }
                break;
            case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_LEASE_IND:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_DHCPv4ClientLeaseChange,
                    dump_wds_DHCPv4ClientLeaseChange,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_DHCPv4ClientLeaseChange_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    unpack_wds_DHCPv4ClientLease_ind_t *varp = &var_unpack_wds_DHCPv4ClientLease_ind_t;
                    const unpack_wds_DHCPv4ClientLease_ind_t *cstp = &const_unpack_wds_DHCPv4ClientLease_ind_t;

                    UNPACK_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_DHCPv4ClientLease_ind,
                    dump_wds_DHCPv4ClientLease_ind,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_DHCPv4ClientLease_ind_t,
                    19,
                    CMP_PTR_TYPE, &varp->ProfileIdTlv, &cstp->ProfileIdTlv,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseStateTlv, &cstp->DHCPv4LeaseStateTlv,
                    CMP_PTR_TYPE, &varp->IPv4AddrTlv, &cstp->IPv4AddrTlv,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.TlvPresent,&cstp->DHCPv4LeaseOptTlv.TlvPresent,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.numOpt,&cstp->DHCPv4LeaseOptTlv.numOpt,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[0].optCode,&cstp->DHCPv4LeaseOptTlv.optList[0].optCode,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[0].optValLen,&cstp->DHCPv4LeaseOptTlv.optList[0].optValLen,
                    CMP_PTR_TYPE, varp->DHCPv4LeaseOptTlv.optList[0].pOptVal,cstp->DHCPv4LeaseOptTlv.optList[0].pOptVal,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[1].optCode,&cstp->DHCPv4LeaseOptTlv.optList[1].optCode,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[1].optValLen,&cstp->DHCPv4LeaseOptTlv.optList[1].optValLen,
                    CMP_PTR_TYPE, varp->DHCPv4LeaseOptTlv.optList[1].pOptVal,cstp->DHCPv4LeaseOptTlv.optList[1].pOptVal,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[2].optCode,&cstp->DHCPv4LeaseOptTlv.optList[2].optCode,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[2].optValLen,&cstp->DHCPv4LeaseOptTlv.optList[2].optValLen,
                    CMP_PTR_TYPE, varp->DHCPv4LeaseOptTlv.optList[2].pOptVal,cstp->DHCPv4LeaseOptTlv.optList[2].pOptVal,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[3].optCode,&cstp->DHCPv4LeaseOptTlv.optList[3].optCode,
                    CMP_PTR_TYPE, &varp->DHCPv4LeaseOptTlv.optList[3].optValLen,&cstp->DHCPv4LeaseOptTlv.optList[3].optValLen,
                    CMP_PTR_TYPE, varp->DHCPv4LeaseOptTlv.optList[3].pOptVal,cstp->DHCPv4LeaseOptTlv.optList[3].pOptVal,
                    CMP_PTR_TYPE, varp->DHCPv4LeaseOptTlv.optListData,cstp->DHCPv4LeaseOptTlv.optListData,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WDS_SWI_RM_TRANSFER_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_RMSetTransferStatistics,
                    dump_RMSetTransferStatistics,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_RMSetTransferStatistics_t);

                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_RMTransferStatistics_ind,
                    dump_wds_RMTransferStatistics_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_RMTransferStatistics_ind_t);
                }
                break;
            case eQMI_WDS_START_NET:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSStartDataSession_t *varp = &var_unpack_wds_SLQSStartDataSession_t;
                    const unpack_wds_SLQSStartDataSession_t *cstp = &const_unpack_wds_SLQSStartDataSession_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSStartDataSession,
                    dump_wds_SLQSStartDataSession,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSStartDataSession_t,
                    5,
                    CMP_PTR_TYPE, varp->psid, cstp->psid,
                    CMP_PTR_TYPE, varp->pFailureReason, cstp->pFailureReason,
                    CMP_PTR_TYPE, varp->pVerboseFailReasonType, cstp->pVerboseFailReasonType,
                    CMP_PTR_TYPE, varp->pVerboseFailureReason,cstp->pVerboseFailureReason,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_WDS_STOP_NET:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSStopDataSession,
                    dump_wds_SLQSStopDataSession,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSStopDataSession_t);
                }
                break;
           case eQMI_WDS_GET_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetRuntimeSettings,
                    dump_wds_SLQSGetRuntimeSettings,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGetRuntimeSettings_t);
                }
                break;
            case eQMI_WDS_CREATE_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSCreateProfile_t *varp = &var_unpack_wds_SLQSCreateProfile_t;
                    const unpack_wds_SLQSCreateProfile_t *cstp = &const_unpack_wds_SLQSCreateProfile_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSCreateProfile,
                    dump_wds_SLQSCreateProfile,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSCreateProfile_t,
                    3,
                    CMP_PTR_TYPE, varp->pCreateProfileOut, cstp->pCreateProfileOut,
                    CMP_PTR_TYPE, &varp->Tlvresult, &cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WDS_MODIFY_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSModifyProfile_t *varp = &var_unpack_wds_SLQSModifyProfile_t;
                    const unpack_wds_SLQSModifyProfile_t *cstp = &const_unpack_wds_SLQSModifyProfile_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSModifyProfile,
                    dump_wds_SLQSModifyProfile,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSModifyProfile_t,
                    2,
                    CMP_PTR_TYPE, varp->pExtErrorCode, cstp->pExtErrorCode,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
            case eQMI_WDS_GET_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    if (rsp_ctx.xid == 0x000c)
                    {
                    unpack_wds_SLQSGetProfileSettings_t *varp = &var_unpack_wds_SLQSGetProfileSettings_t[0];
                    const unpack_wds_SLQSGetProfileSettings_t *cstp = &const_unpack_wds_SLQSGetProfileSettings_t[0];

                    LibpackProfile3GPP  *varprofile = &varp->pProfileSettings->curProfile.SlqsProfile3GPP;
                    LibpackProfile3GPP  *cstprofile = &cstp->pProfileSettings->curProfile.SlqsProfile3GPP;

                    unpack_wds_SLQSGetProfileSettingsV2_t *varp1 = &var_unpack_wds_SLQSGetProfileSettingsV2_t[0];
                    const unpack_wds_SLQSGetProfileSettingsV2_t *cstp1 = &const_unpack_wds_SLQSGetProfileSettingsV2_t[0];

                    LibpackProfile3GPPV2  *varprofile1 = &varp1->pProfileSettings->curProfile.SlqsProfile3GPP;
                    LibpackProfile3GPPV2  *cstprofile1 = &cstp1->pProfileSettings->curProfile.SlqsProfile3GPP;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetProfileSettings,
                    dump_wds_SLQSGetProfileSettings,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSGetProfileSettings_t[0],
                    32,
                    CMP_PTR_TYPE, varprofile->pProfilename,cstprofile->pProfilename,
                    CMP_PTR_TYPE, varprofile->pPDPtype,cstprofile->pPDPtype,
                    CMP_PTR_TYPE, varprofile->pPdpHdrCompType,cstprofile->pPdpHdrCompType,
                    CMP_PTR_TYPE, varprofile->pPdpDataCompType,cstprofile->pPdpDataCompType,
                    CMP_PTR_TYPE, varprofile->pAPNName,cstprofile->pAPNName,
                    CMP_PTR_TYPE, varprofile->pPriDNSIPv4AddPref,cstprofile->pPriDNSIPv4AddPref,
                    CMP_PTR_TYPE, varprofile->pSecDNSIPv4AddPref,cstprofile->pSecDNSIPv4AddPref,
                    CMP_PTR_TYPE, varprofile->pUMTSReqQoS,cstprofile->pUMTSReqQoS,
                    CMP_PTR_TYPE, varprofile->pGPRSRequestedQos,cstprofile->pGPRSRequestedQos,
                    CMP_PTR_TYPE, varprofile->pUsername,cstprofile->pUsername,
                    CMP_PTR_TYPE, varprofile->pAuthenticationPref,cstprofile->pAuthenticationPref,
                    CMP_PTR_TYPE, varprofile->pIPv4AddrPref,cstprofile->pIPv4AddrPref,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->filterId,&cstprofile->pTFTID2Params->filterId,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->eValid,&cstprofile->pTFTID2Params->eValid,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->ipVersion,&cstprofile->pTFTID2Params->ipVersion,
                    CMP_PTR_TYPE, varprofile->pTFTID2Params->pSourceIP,cstprofile->pTFTID2Params->pSourceIP,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->sourceIPMask,&cstprofile->pTFTID2Params->sourceIPMask,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->nextHeader,&cstprofile->pTFTID2Params->nextHeader,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->destPortRangeStart,&cstprofile->pTFTID2Params->destPortRangeStart,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->destPortRangeEnd,&cstprofile->pTFTID2Params->destPortRangeEnd,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->srcPortRangeStart,&cstprofile->pTFTID2Params->srcPortRangeStart,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->srcPortRangeEnd,&cstprofile->pTFTID2Params->srcPortRangeEnd,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->tosMask,&cstprofile->pTFTID2Params->tosMask,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->IPSECSPI,&cstprofile->pTFTID2Params->IPSECSPI,
                    CMP_PTR_TYPE, &varprofile->pTFTID2Params->flowLabel,&cstprofile->pTFTID2Params->flowLabel,
                    CMP_PTR_TYPE, varprofile->pPdpContext,cstprofile->pPdpContext,
                    CMP_PTR_TYPE, varprofile->pSecondaryFlag,cstprofile->pSecondaryFlag,
                    CMP_PTR_TYPE, varprofile->pPrimaryID,cstprofile->pPrimaryID,
                    CMP_PTR_TYPE, varprofile->pIPv6AddPref,cstprofile->pIPv6AddPref,
                    CMP_PTR_TYPE, &varp->ProfileType,&cstp->ProfileType,
                    CMP_PTR_TYPE, &varp->Tlvresult,&cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetProfileSettingsV2,
                    dump_wds_SLQSGetProfileSettingsV2,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSGetProfileSettingsV2_t[0],
                    55,
                    CMP_PTR_TYPE, varprofile1->pProfilename,cstprofile1->pProfilename,
                    CMP_PTR_TYPE, varprofile1->pPDPtype,cstprofile1->pPDPtype,
                    CMP_PTR_TYPE, varprofile1->pPdpHdrCompType,cstprofile1->pPdpHdrCompType,
                    CMP_PTR_TYPE, varprofile1->pPdpDataCompType,cstprofile1->pPdpDataCompType,
                    CMP_PTR_TYPE, varprofile1->pAPNName,cstprofile1->pAPNName,
                    CMP_PTR_TYPE, varprofile1->pPriDNSIPv4AddPref,cstprofile1->pPriDNSIPv4AddPref,
                    CMP_PTR_TYPE, varprofile1->pSecDNSIPv4AddPref,cstprofile1->pSecDNSIPv4AddPref,
                    CMP_PTR_TYPE, varprofile1->pUMTSReqQoS,cstprofile1->pUMTSReqQoS,
                    CMP_PTR_TYPE, varprofile1->pGPRSRequestedQos,cstprofile1->pGPRSRequestedQos,
                    CMP_PTR_TYPE, varprofile1->pUsername,cstprofile1->pUsername,
                    CMP_PTR_TYPE, varprofile1->pAuthenticationPref,cstprofile1->pAuthenticationPref,
                    CMP_PTR_TYPE, varprofile1->pIPv4AddrPref,cstprofile1->pIPv4AddrPref,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->filterId,&cstprofile1->pTFTID2Params->filterId,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->eValid,&cstprofile1->pTFTID2Params->eValid,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->ipVersion,&cstprofile1->pTFTID2Params->ipVersion,
                    CMP_PTR_TYPE, varprofile1->pTFTID2Params->pSourceIP,cstprofile1->pTFTID2Params->pSourceIP,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->sourceIPMask,&cstprofile1->pTFTID2Params->sourceIPMask,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->nextHeader,&cstprofile1->pTFTID2Params->nextHeader,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->destPortRangeStart,&cstprofile1->pTFTID2Params->destPortRangeStart,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->destPortRangeEnd,&cstprofile1->pTFTID2Params->destPortRangeEnd,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->srcPortRangeStart,&cstprofile1->pTFTID2Params->srcPortRangeStart,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->srcPortRangeEnd,&cstprofile1->pTFTID2Params->srcPortRangeEnd,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->tosMask,&cstprofile1->pTFTID2Params->tosMask,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->IPSECSPI,&cstprofile1->pTFTID2Params->IPSECSPI,
                    CMP_PTR_TYPE, &varprofile1->pTFTID2Params->flowLabel,&cstprofile1->pTFTID2Params->flowLabel,
                    CMP_PTR_TYPE, varprofile1->pPdpContext,cstprofile1->pPdpContext,
                    CMP_PTR_TYPE, varprofile1->pSecondaryFlag,cstprofile1->pSecondaryFlag,
                    CMP_PTR_TYPE, varprofile1->pPrimaryID,cstprofile1->pPrimaryID,
                    CMP_PTR_TYPE, varprofile1->pIPv6AddPref,cstprofile1->pIPv6AddPref,
                    CMP_PTR_TYPE, varprofile1->pAPNBearer,cstprofile1->pAPNBearer,
                    CMP_PTR_TYPE, varprofile1->pSupportEmergencyCalls,cstprofile1->pSupportEmergencyCalls,
                    CMP_PTR_TYPE, varprofile1->pOperatorPCOID,cstprofile1->pOperatorPCOID,
                    CMP_PTR_TYPE, varprofile1->pMcc,cstprofile1->pMcc,
                    CMP_PTR_TYPE, varprofile1->pMnc,cstprofile1->pMnc,
                    CMP_PTR_TYPE, varprofile1->pMaxPDN,cstprofile1->pMaxPDN,
                    CMP_PTR_TYPE, varprofile1->pMaxPDNTimer,cstprofile1->pMaxPDNTimer,
                    CMP_PTR_TYPE, varprofile1->pPDNWaitTimer,cstprofile1->pPDNWaitTimer,
                    CMP_PTR_TYPE, varprofile1->pAppUserData,cstprofile1->pAppUserData,
                    CMP_PTR_TYPE, varprofile1->pRoamDisallowFlag,cstprofile1->pRoamDisallowFlag,
                    CMP_PTR_TYPE, varprofile1->pPDNDisconnectWaitTimer,cstprofile1->pPDNDisconnectWaitTimer,
                    CMP_PTR_TYPE, varprofile1->pDnsWithDHCPFlag,cstprofile1->pDnsWithDHCPFlag,
                    CMP_PTR_TYPE, varprofile1->pLteRoamPDPType,cstprofile1->pLteRoamPDPType,
                    CMP_PTR_TYPE, varprofile1->pUmtsRoamPDPType,cstprofile1->pUmtsRoamPDPType,
                    CMP_PTR_TYPE, varprofile1->pIWLANtoLTEHandoverFlag,cstprofile1->pIWLANtoLTEHandoverFlag,
                    CMP_PTR_TYPE, varprofile1->pLTEtoIWLANHandoverFlag,cstprofile1->pLTEtoIWLANHandoverFlag,
                    CMP_PTR_TYPE, varprofile1->pPDNThrottleTimer,cstprofile1->pPDNThrottleTimer,
                    CMP_PTR_TYPE, varprofile1->pOverridePDPType,cstprofile1->pOverridePDPType,
                    CMP_PTR_TYPE, varprofile1->pPCOIDList,cstprofile1->pPCOIDList,
                    CMP_PTR_TYPE, varprofile1->pMsisdnFlag,cstprofile1->pMsisdnFlag,
                    CMP_PTR_TYPE, varprofile1->pPersistFlag,cstprofile1->pPersistFlag,
                    CMP_PTR_TYPE, varprofile1->pClatFlag,cstprofile1->pClatFlag,
                    CMP_PTR_TYPE, varprofile1->pIPV6DelegFlag,cstprofile1->pIPV6DelegFlag,
                    CMP_PTR_TYPE, &varp1->ProfileType,&cstp1->ProfileType,
                    CMP_PTR_TYPE, &varp1->Tlvresult,&cstp1->Tlvresult,
                    CMP_PTR_TYPE, &varp1->ParamPresenceMask, &cstp1->ParamPresenceMask,
                    );


                    }
                    else if (rsp_ctx.xid == 0x000d)
                    {
                    unpack_wds_SLQSGetProfileSettings_t *varp = &var_unpack_wds_SLQSGetProfileSettings_t[1];
                    const unpack_wds_SLQSGetProfileSettings_t *cstp = &const_unpack_wds_SLQSGetProfileSettings_t[1];

                    LibpackProfile3GPP2  *varprofile = &varp->pProfileSettings->curProfile.SlqsProfile3GPP2;
                    LibpackProfile3GPP2  *cstprofile = &cstp->pProfileSettings->curProfile.SlqsProfile3GPP2;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetProfileSettings,
                    dump_wds_SLQSGetProfileSettings_3GPP2,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSGetProfileSettings_t[1],
                    27,
                    CMP_PTR_TYPE, varprofile->pNegoDnsSrvrPref,cstprofile->pNegoDnsSrvrPref,
                    CMP_PTR_TYPE, varprofile->pPppSessCloseTimerDO,cstprofile->pPppSessCloseTimerDO,
                    CMP_PTR_TYPE, varprofile->pPppSessCloseTimer1x,cstprofile->pPppSessCloseTimer1x,
                    CMP_PTR_TYPE, varprofile->pAllowLinger,cstprofile->pAllowLinger,
                    CMP_PTR_TYPE, varprofile->pLcpAckTimeout,cstprofile->pLcpAckTimeout,
                    CMP_PTR_TYPE, varprofile->pIpcpAckTimeout,cstprofile->pIpcpAckTimeout,
                    CMP_PTR_TYPE, varprofile->pAuthTimeout,cstprofile->pAuthTimeout,
                    CMP_PTR_TYPE, varprofile->pLcpCreqRetryCount,cstprofile->pLcpCreqRetryCount,
                    CMP_PTR_TYPE, varprofile->pIpcpCreqRetryCount,cstprofile->pIpcpCreqRetryCount,
                    CMP_PTR_TYPE, varprofile->pAuthRetryCount,cstprofile->pAuthRetryCount,
                    CMP_PTR_TYPE, varprofile->pAuthProtocol,cstprofile->pAuthProtocol,
                    CMP_PTR_TYPE, varprofile->pUserId,cstprofile->pUserId,
                    CMP_PTR_TYPE, varprofile->pAuthPassword,cstprofile->pAuthPassword,
                    CMP_PTR_TYPE, varprofile->pDataRate,cstprofile->pDataRate,
                    CMP_PTR_TYPE, varprofile->pAppType,cstprofile->pAppType,
                    CMP_PTR_TYPE, varprofile->pDataMode,cstprofile->pDataMode,
                    CMP_PTR_TYPE, varprofile->pAppPriority,cstprofile->pAppPriority,
                    CMP_PTR_TYPE, varprofile->pApnString,cstprofile->pApnString,
                    CMP_PTR_TYPE, varprofile->pPdnType,cstprofile->pPdnType,
                    CMP_PTR_TYPE, varprofile->pIsPcscfAddressNedded,cstprofile->pIsPcscfAddressNedded,
                    CMP_PTR_TYPE, varprofile->pPrimaryV4DnsAddress,cstprofile->pPrimaryV4DnsAddress,
                    CMP_PTR_TYPE, varprofile->pSecondaryV4DnsAddress,cstprofile->pSecondaryV4DnsAddress,
                    CMP_PTR_TYPE, varprofile->pPriV6DnsAddress,cstprofile->pPriV6DnsAddress,
                    CMP_PTR_TYPE, varprofile->pSecV6DnsAddress,cstprofile->pSecV6DnsAddress,
                    CMP_PTR_TYPE, &varp->ProfileType,&cstp->ProfileType,
                    CMP_PTR_TYPE, &varp->Tlvresult,&cstp->Tlvresult,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                    }
                }
                break;
           case eQMI_WDS_GET_DEFAULTS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetDefaultProfile,
                    dump_wds_GetDefaultProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetDefaultProfile_t);

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetDefaultProfileV2,
                    dump_wds_GetDefaultProfileV2,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetDefaultProfileV2_t);
                }
                break;
           case eQMI_WDS_GET_RATES:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetConnectionRate,
                    dump_GetConnectionRate,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetConnectionRate_t);
                }
                break;
           case eQMI_WDS_GET_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                    if (rsp_ctx.xid == 0x0024)
                    {
                        UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_wds_GetPacketStatus,
                        dump_wds_GetPacketStatus,
                        msg.buf,
                        rlen,
                        &const_unpack_wds_GetPacketStatus_t);
                    }
                    else if (rsp_ctx.xid == 0x0025)
                    {
                        unpack_wds_GetPacketStatistics_t *varp = &var_unpack_wds_GetPacketStatistics_t;
                        const unpack_wds_GetPacketStatistics_t *cstp = &const_unpack_wds_GetPacketStatistics_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_wds_GetPacketStatistics,
                        dump_wds_GetPacketStatistics,
                        msg.buf,
                        rlen,
                        &var_unpack_wds_GetPacketStatistics_t,
                        13,
                        CMP_PTR_TYPE, varp->pTXPacketSuccesses, cstp->pTXPacketSuccesses,
                        CMP_PTR_TYPE, varp->pRXPacketSuccesses, cstp->pRXPacketSuccesses,
                        CMP_PTR_TYPE, varp->pTXPacketErrors, cstp->pTXPacketErrors,
                        CMP_PTR_TYPE, varp->pRXPacketErrors, cstp->pRXPacketErrors,
                        CMP_PTR_TYPE, varp->pTXPacketOverflows, cstp->pTXPacketOverflows,
                        CMP_PTR_TYPE, varp->pRXPacketOverflows, cstp->pRXPacketOverflows,
                        CMP_PTR_TYPE, varp->pTXOkBytesCount, cstp->pTXOkBytesCount,
                        CMP_PTR_TYPE, varp->pRXOkBytesCount, cstp->pRXOkBytesCount,
                        CMP_PTR_TYPE, varp->pTXOKBytesLastCall, cstp->pTXOKBytesLastCall,
                        CMP_PTR_TYPE, varp->pRXOKBytesLastCall, cstp->pRXOKBytesLastCall,
                        CMP_PTR_TYPE, varp->pTXDroppedCount, cstp->pTXDroppedCount,
                        CMP_PTR_TYPE, varp->pRXDroppedCount, cstp->pRXDroppedCount,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                    }
                    else if (rsp_ctx.xid == 0x0026)
                    {
                        unpack_wds_GetByteTotals_t *varp = &var_unpack_wds_GetByteTotals_t;
                        const unpack_wds_GetByteTotals_t *cstp = &const_unpack_wds_GetByteTotals_t;

                        UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                        unpack_wds_GetByteTotals,
                        dump_wds_GetByteTotals,
                        msg.buf,
                        rlen,
                        &var_unpack_wds_GetByteTotals_t,
                        3,
                        CMP_PTR_TYPE, varp->pTXTotalBytes, cstp->pTXTotalBytes,
                        CMP_PTR_TYPE, varp->pRXTotalBytes, cstp->pRXTotalBytes,
                        CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                        );
                     }
                }
                break;
           case eQMI_WDS_GET_DURATION:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_GetSessionDurationV2_t *varp = &var_unpack_wds_GetSessionDurationV2_t;
                    const unpack_wds_GetSessionDurationV2_t *cstp = &const_unpack_wds_GetSessionDurationV2_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetSessionDurationV2,
                    dump_GetSessionDurationV2,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_GetSessionDurationV2_t,
                    5,
                    CMP_PTR_TYPE, &varp->callDuration, &cstp->callDuration,
                    CMP_PTR_TYPE, varp->pLastCallDuration, cstp->pLastCallDuration,
                    CMP_PTR_TYPE, varp->pCallActiveDuration, cstp->pCallActiveDuration,
                    CMP_PTR_TYPE, varp->pLastCallActiveDuration, cstp->pLastCallActiveDuration,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );

                    printf("\n\n");

                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetSessionDuration,
                    dump_GetSessionDuration,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetSessionDuration_t);
                }
                break;
           case eQMI_WDS_GET_DORMANCY:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetDormancyState,
                    dump_GetDormancyState,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetDormancyState_t);
                }
                break;
            case eQMI_WDS_DELETE_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSDeleteProfile,
                    dump_wds_SLQSDeleteProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSDeleteProfile_t);
                }
                break;
            case eQMI_WDS_SWI_SET_3GPP_CFG_ITEM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSet3GPPConfigItem,
                    dump_SLQSSet3GPPConfigItem,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSet3GPPConfigItem_t);
                }
                break;
            case eQMI_WDS_SWI_GET_3GPP_CFG_ITEM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGet3GPPConfigItem,
                    dump_SLQSGet3GPPConfigItem,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGet3GPPConfigItem_t);
                }
                break;
            case eQMI_WDS_GET_MIP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetMobileIP,
                    dump_GetMobileIP,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetMobileIP_t);
                }
                break;
            case eQMI_WDS_GET_MIP_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetMobileIPProfile,
                    dump_GetMobileIPProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetMobileIPProfile_t);
                }
                break;
            case eQMI_WDS_GET_CURR_DATA_SYS_STAT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetCurrDataSystemStat,
                    dump_SLQSGetCurrDataSystemStat,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGetCurrDataSystemStat_t);
                }
                break;
            case eQMI_WDS_GET_LAST_MIP_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetLastMobileIPError,
                    dump_GetLastMobileIPError,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetLastMobileIPError_t);
                }
                break;
            case eQMI_WDS_SET_MIP_PROFILE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetMobileIPProfile,
                    dump_SetMobileIPProfile,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetMobileIPProfile_t);
                }
                break;
            case eQMI_WDS_SWI_PDP_RUNTIME_SETTINGS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSWdsSwiPDPRuntimeSettings,
                    dump_SLQSWdsSwiPDPRuntimeSettings,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t);
                }
                break;
            case eQMI_WDS_GET_CUR_DATA_BEARER:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetDataBearerTechnology,
                    dump_wds_SLQSGetDataBearerTechnology,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGetDataBearerTechnology_t);
                }
                break;
            case eQMI_WDS_SET_IP_FAMILY:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSetIPFamilyPreference,
                    dump_wds_SLQSSetIPFamilyPreference,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSetIPFamilyPreference_t);
                }
                break;
            case eQMI_WDS_SET_DEFAULT_PROFILE_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetDefaultProfileNum,
                    dump_SetDefaultProfileNum,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetDefaultProfileNum_t);
                }
                break;
            case eQMI_WDS_GET_DEFAULT_PROFILE_NUM:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetDefaultProfileNum,
                    dump_GetDefaultProfileNum,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_GetDefaultProfileNum_t);
                }
                break;
           case eQMI_WDS_SWI_GET_DHCPV4_CLIENT_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_SLQSSGetDHCPv4ClientConfig_t *varp = &var_unpack_wds_SLQSSGetDHCPv4ClientConfig_t;
                    const unpack_wds_SLQSSGetDHCPv4ClientConfig_t *cstp = &const_unpack_wds_SLQSSGetDHCPv4ClientConfig_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSGetDHCPv4ClientConfig,
                    dump_wds_SLQSSGetDHCPv4ClientConfig,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_SLQSSGetDHCPv4ClientConfig_t,
                    4,
                    CMP_PTR_TYPE, varp->pHwConfig, cstp->pHwConfig,
                    CMP_PTR_TYPE, &var_Dhcpv4Option, &cst_Dhcpv4Option,
                    CMP_PTR_TYPE, &varp->pRequestOptionList->numOpt, &cstp->pRequestOptionList->numOpt,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
          case eQMI_WDS_SWI_GET_CURRENT_CHANNEL_RATE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSGetCurrentChannelRate,
                    dump_wds_SLQSGetCurrentChannelRate,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSGetCurrentChannelRate_t);
                }
                break;
          case eQMI_WDS_SWI_GET_DATA_LOOPBACK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSGetLoopback,
                    dump_wds_SLQSSGetLoopback,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSGetLoopback_t);
                }
                break;
          case eQMI_WDS_SWI_SET_DATA_LOOPBACK:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSSetLoopback,
                    dump_wds_SLQSSSetLoopback,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSSetLoopback_t);
                }
                break;
          case eQMI_WDS_SET_MIP:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetMobileIP,
                    dump_wds_SetMobileIP,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetMobileIP_t);
                }
                break;
          case eQMI_WDS_SET_MIP_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetMobileIPParameters,
                    dump_wds_SetMobileIPParameters,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetMobileIPParameters_t);
                }
                break;
          case eQMI_WDS_SET_AUTOCONNECT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetAutoconnect,
                    dump_wds_SetAutoconnect,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetAutoconnect_t);
                }
                break;
            case eQMI_WDS_GET_AUTOCONNECT:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_GetAutoconnect_t *varp = &var_unpack_wds_GetAutoconnect_t;
                    const unpack_wds_GetAutoconnect_t *cstp = &const_unpack_wds_GetAutoconnect_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetAutoconnect,
                    dump_wds_GetAutoconnect,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_GetAutoconnect_t,
                    2,
                    CMP_PTR_TYPE, varp->psetting, cstp->psetting,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
          case eQMI_WDS_G0_DORMANT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSWdsGoDormant,
                    dump_wds_SLQSWdsGoDormant,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSWdsGoDormant_t);
                }
                break;
          case eQMI_WDS_G0_ACTIVE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSWdsGoActive,
                    dump_wds_SLQSWdsGoActive,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSWdsGoActive_t);
                }
                break;
          case eQMI_WDS_RESET_PKT_STATISTICS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSResetPacketStatics,
                    dump_wds_SLQSResetPacketStatics,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSResetPacketStatics_t);
                }
                break;
          case eQMI_WDS_SWI_SET_DHCPV4_CLIENT_CONFIG:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSSetDHCPv4ClientConfig,
                    dump_wds_SLQSSSetDHCPv4ClientConfig,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSSSetDHCPv4ClientConfig_t);
                }
                break;
           case eQMI_WDS_GET_DATA_BEARER:
                if (eRSP == rsp_ctx.type)
                {
                    unpack_wds_GetDataBearerTechnology_t *varp = &var_unpack_wds_GetDataBearerTechnology_t;
                    const unpack_wds_GetDataBearerTechnology_t *cstp = &const_unpack_wds_GetDataBearerTechnology_t;

                    UNPACK_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_GetDataBearerTechnology,
                    dump_wds_GetDataBearerTechnology,
                    msg.buf,
                    rlen,
                    &var_unpack_wds_GetDataBearerTechnology_t,
                    2,
                    CMP_PTR_TYPE, varp->pDataBearer, cstp->pDataBearer,
                    CMP_PTR_TYPE, &varp->ParamPresenceMask, &cstp->ParamPresenceMask,
                    );
                }
                break;
           case eQMI_WDS_SET_QMUX_ID:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SetMuxID,
                    dump_wds_SetMuxID,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SetMuxID_t);
                }
                break; 
           case eQMI_WDS_SWI_PROFILE_CHANGE:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSwiProfileChangeCallback,
                    dump_wds_SLQSSwiProfileChangeCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSwiProfileChangeCallback_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_wds_SLQSSwiProfileChangeCallback_Ind,
                    dump_wds_SLQSSwiProfileChangeCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_wds_SLQSwiProfileChangeCallback_Ind_t);
                }
                break;
            }
        }
    }
    return 0;
}




