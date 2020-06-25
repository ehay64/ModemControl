/**
 * \ingroup liteqmi
 *
 * \file wds.h
 *
 * \section     TableWDSSOMask SO Mask
 * \li          SO mask to indicate the service option or type of application.
 *              - An SO mask value of zero indicates that
 *                this field is ignored. \n
 *              - Values:
 *                  - 0x00 - DONT_CARE
 *              - CDMA 1X SO mask:
 *                  - 0x01 - CDMA_1X_IS95
 *                  - 0x02 - CDMA_1X_IS2000
 *                  - 0x04 - CDMA_1X_IS2000_REL_A
 *              - CDMA EV-DO Rev 0 SO mask:
 *                  - 0x01 - DPA
 *              - CDMA EV-DO Rev A SO mask:
 *                  - 0x01 - DPA
 *                  - 0x02 - MFPA
 *                  - 0x04 - EMPA
 *                  - 0x08 - EMPA_EHRPD
 *              - CDMA EV-DO Rev B SO mask:
 *                  - 0x01 - DPA
 *                  - 0x02 - MFPA
 *                  - 0x04 - EMPA
 *                  - 0x08 - EMPA_EHRPD
 *                  - 0x10 - MMPA
 *                  - 0x20 - MMPA_EHRPD
 *
 * \section     TableWDSRatMask RAT Mask
 * \li          RAT mask to indicate the type of
 *              technology. A RAT mask value of zero
 *              indicates that this field is ignored.\n
 *              Values:
 *                  - 0x00 - DONT_CARE
 *                  - 0x8000 - NULL_BEARER
 *              - CDMA RAT mask:
 *                  - 0x01 - CDMA_1X
 *                  - 0x02 - EVDO_REV0
 *                  - 0x04 - EVDO_REVA
 *                  - 0x08 - EVDO_REVB
 *                  - 0x10 - EHRPD
 *                  - 0x20 - FMC
 *              - UMTS RAT mask:
 *                  - 0x01 - WCDMA
 *                  - 0x02 - GPRS
 *                  - 0x04 - HSDPA
 *                  - 0x08 - HSUPA
 *                  - 0x10 - EDGE
 *                  - 0x20 - LTE
 *                  - 0x40 - HSDPA+
 *                  - 0x80 - DC_HSDPA+
 *                  - 0x100 - 64_QAM
 *                  - 0x200 - TD-SCDMA
 *
 **/
#ifndef __LITEQMI_WDS_H__
#define __LITEQMI_WDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define IPV6_ADDRESS_ARRAY_SIZE 8
#define MAX_WDS_3GPP_CONF_LTE_ATTACH_PROFILE_LIST_SIZE 24
#define PACK_WDS_IPV4                          4
#define PACK_WDS_IPV6                          6
#define BYT_STAT_STAT_MASK 0X000000C0
#define WDS_DHCP_MAX_NUM_OPTIONS       30
#define WDS_DHCP_OPTION_DATA_BUF_SIZE  2048  /* current max size of raw message in SDK process is 2048 */
#define WDS_TFTID_SOURCE_IP_SIZE 8
#define WDS_PROFILE_3GPP 0
#define WDS_PROFILE_3GPP2 1
#define LITE_MAX_PDN_THROTTLE_TIMER 10
#define LITE_MAX_PCOID_LIST 10
#define MAX_WDS_NAME_ARRAY_SIZE 255


/**
 *  This structure contains 3GPP LTE QoS parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  \param  QCI
 *          - QOS specified using the QOS Class Identifier (QOS) values
 *            QCI value 0    - Requests the network to assign the
 *                             appropriate QCI value
 *            QCI values 1-4 - Associated with guaranteed bit rates
 *            QCI values 5-9 - Associated with non-guaranteed bit rates
 *
 *  \param  gDlBitRate
 *          - Guaranteed DL bit rate
 *
 *  \param  maxDlBitRate
 *          - Maximum DL bit rate
 *
 *  \param  gUlBitRate
 *          - Guaranteed UL bit rate
 *
 *  \param  maxUlBitRate
 *          - Maximum UL bit rate
 *
 */
typedef struct 
{
    uint8_t QCI;
    uint32_t gDlBitRate;
    uint32_t maxDlBitRate;
    uint32_t gUlBitRate;
    uint32_t maxUlBitRate;
}LibPackQosClassID;

/**
 *  This structure contains traffic flow template parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  \param  filterId
 *          - Filter identifier
 *
 *  \param  eValid
 *          - Evaluation precedence index
 *
 *  \param  ipVersion
 *          - IP version number
 *            - 4 - IPv4
 *            - 6 - IPv6
 *
 *  \param  pSourceIP
 *          - Source IP address
 *           - IPv4 - Fill the first 4 uint8_ts
 *           - IPv6 - Fill all the 16 uint8_ts
 *
 *  \param  sourceIPMask
 *          - Mask value for the source address
 *
 *  \param  nextHeader
 *          - Next header/protocol value
 *
 *  \param  destPortRangeStart
 *          - Start value of the destination port range
 *
 *  \param  destPortRangeEnd
 *          - End value of the destination port range
 *
 *  \param  srcPortRangeStart
 *          - Start value of the source port range
 *
 *  \param  srcPortRangeEnd
 *          - End value of the source port range
 *
 *  \param  IPSECSPI
 *          - IPSEC security parameter index
 *
 *  \param  tosMask
 *          - TOS mask (Traffic class for IPv6)
 *
 *  \param  flowLabel
 *          - Flow label
 *
 */
typedef struct 
{
    uint8_t  filterId;
    uint8_t  eValid;
    uint8_t  ipVersion;
    uint16_t  *pSourceIP;
    uint8_t  sourceIPMask;
    uint8_t  nextHeader;
    uint16_t  destPortRangeStart;
    uint16_t  destPortRangeEnd;
    uint16_t  srcPortRangeStart;
    uint16_t  srcPortRangeEnd;
    uint32_t IPSECSPI;
    uint16_t  tosMask;
    uint32_t flowLabel;
}LibPackTFTIDParams;



/**
 * This structure contains the GPRS Quality Of Service Information
 *
 *  @param  precedenceClass
 *          - Precedence class
 *
 *  @param  delayClass
 *          - Delay class
 *
 *  @param  reliabilityClass
 *          - Reliability class
 *
 *  @param  peakThroughputClass
 *          - Peak throughput class
 *
 *  @param  meanThroughputClass
 *          - Mean throughput class
 *
 */
typedef struct 
{
    uint32_t precedenceClass;
    uint32_t delayClass;
    uint32_t reliabilityClass;
    uint32_t peakThroughputClass;
    uint32_t meanThroughputClass;
}LibPackGPRSRequestedQoS;


/**
 * This structure contains the UMTS Quality Of Service Information
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  \param  trafficClass
 *          - 0x00 - Subscribed
 *          - 0x01 - Conversational
 *          - 0x02 - Streaming
 *          - 0x03 - Interactive
 *          - 0x04 - Background
 *          - 0xff - Invalid UMTS Quality Of Service Information.
 *
 *  \param  maxUplinkBitrate
 *          - Maximum uplink bit rate in bits/sec
 *
 *  \param  maxDownlinkBitrate
 *          - Maximum downlink bit rate in bits/sec
 *
 *  \param  grntUplinkBitrate
 *          - Guaranteed uplink bit rate in bits/sec
 *
 *  \param  grntDownlinkBitrate
 *          - Guranteed downlink bit rate in bits/sec
 *
 *  \param  qosDeliveryOrder - Qos delivery order
 *          - 0x00 - Subscribe
 *          - 0x01 - delivery order on
 *          - 0x02 - delivery order off
 *
 *  \param  maxSDUSize
 *          - Maximum SDU size
 *
 *  \param  sduErrorRatio - SDU error ratio
 *          - Target value for fraction of SDUs lost or
 *            detected as erroneous.
 *          - 0x00 - Subscribe
 *          - 0x01 - 1*10^(-2)
 *          - 0x02 - 7*10^(-3)
 *          - 0x03 - 1*10^(-3)
 *          - 0x04 - 1*10^(-4)
 *          - 0x05 - 1*10^(-5)
 *          - 0x06 - 1*10^(-6)
 *          - 0x07 - 1*10^(-1)
 *
 *  \param  resBerRatio - Residual bit error ratio
 *          - Target value for undetected bit error ratio in
 *            in the delivered SDUs.
 *          - 0x00 - Subscribe
 *          - 0x01 - 5*10^(-2)
 *          - 0x02 - 1*10^(-2)
 *          - 0x03 - 5*10^(-3)
 *          - 0x04 - 4*10^(-3)
 *          - 0x05 - 1*10^(-3)
 *          - 0x06 - 1*10^(-4)
 *          - 0x07 - 1*10^(-5)
 *          - 0x08 - 1*10^(-6)
 *          - 0x09 - 1*10^(-8)
 *
 *  \param  deliveryErrSDU - Delivery of erroneous SDUs
 *          - Indicates whether SDUs detected as erroneous shall be
 *            delivered or not.
 *          - 0x00 - Subscribe
 *          - 0x01 - 5*10^(-2)
 *          - 0x02 - 1*10^(-2)
 *          - 0x03 - 5*10^(-3)
 *          - 0x04 - 4*10^(-3)
 *          - 0x05 - 1*10^(-3)
 *          - 0x06 - 1*10^(-4)
 *          - 0x07 - 1*10^(-5)
 *          - 0x08 - 1*10^(-6)
 *          - 0x09 - 1*10^(-8)
 *
 *  \param  transferDelay - Transfer delay (ms)
 *          - Indicates the targeted time between a request to transfer an
 *            SDU at one SAP to its delivery at the other SAP in milliseconds.
 *
 *  \param  trafficPriority - Transfer handling priority
 *          - Specifies the relative importance for handling of SDUs that
 *            belong to the UMTS bearer, compared to the SDUs of other bearers.
 *
 *  \note Check \ref trafficClass before use.
 *
 */
typedef struct 
{
    uint8_t  trafficClass;
    uint32_t maxUplinkBitrate;
    uint32_t maxDownlinkBitrate;
    uint32_t grntUplinkBitrate;
    uint32_t grntDownlinkBitrate;
    uint8_t  qosDeliveryOrder;
    uint32_t maxSDUSize;
    uint8_t  sduErrorRatio;
    uint8_t  resBerRatio;
    uint8_t  deliveryErrSDU;
    uint32_t transferDelay;
    uint32_t trafficPriority;
}LibPackUMTSQoS;

/**
 *  This structure contains UMTS requested QoS with Signaling Indication flag
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  UMTSReqQoS
 *          - Contains the UMTS Quality Of Service Information
 *          - See \ref LibPackUMTSQoS
 *
 *  @param  SigInd - Signaling Indication flag
 *          - TRUE  - Signaling indication ON
 *          - FALSE - Signaling indication OFF
 *
 */
typedef struct
{
    LibPackUMTSQoS UMTSReqQoS;
    uint8_t           SigInd;
}LibPackUMTSReqQoSSigInd;

/**
 * This structure contains the MNC Information
 *
 *  @param  MNC
 *          - Mobile Network Code
 *          - range 0-999
 *
 *  @param  PCSFlag
 *          - Indicate if PCS flag is included
 *           - 0 - FALSE
 *           - 1 - TRUE
 */
typedef struct
{
    uint16_t   MNC;
    uint8_t   PCSFlag;
}LibPackProfileMnc;

/**
 * This structure contains information about the PDN throttle timer
 *
 *  @param  ThrottleTimer
 *          Throttle Timer for Max 10 PDN connections
 */
typedef struct
{
    uint32_t   ThrottleTimer[LITE_MAX_PDN_THROTTLE_TIMER];
}LibPackPDNThrottleTimer;

/**
 * This structure contains information about the PCOID List
 *
 *  @param  PcoList
 *          PCOID for Max 10
 */
typedef struct
{
    uint16_t   PcoList[LITE_MAX_PCOID_LIST];
}LibPackPCOIDList;


/**
 * This structure contains pack Start Data Session Information.
 * 
 * @param pTech
 *          - Indicates the technology preference
 *              - 1 - UMTS
 *              - 2 - CDMA
 *              - 3 - eMBMS
 *              - 4 - Modem Link Label.
 *                    Modem Link is an interface for transferring data between
 *                    entities on AP and modem.
 *          - optional
 * @param pprofileid3gpp
 *          - pointer to 3GPP profile id
 *          - optional
 * @param pprofileid3gpp2
 *          - pointer to 3GPP2 profile id
 *          - optional
 * @param pAuth
 *          - Authentication type, it can be PAP or CHAP
 *          - optional
 * @param pUser
 *          - username for authentication process
 *          - optional
 * @param pPass
 *          - password for authentication process
 *          - optional
 */
typedef struct {
    uint8_t *pTech;
    uint32_t *pprofileid3gpp;
    uint32_t *pprofileid3gpp2;
    uint32_t *pAuth;
    char *pUser;
    char *pPass;
} pack_wds_SLQSStartDataSession_t;

/**
 * This structure contains unpack Start Data Session Information.
 * 
 * @param  psid
 *         - Assigned session ID when starting a data session
 *         - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pFailureReason
 *          - Reason data session failed to be established
 *          - See qaGobiApiTableCallEndReasons.h for Call End Reason
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pVerboseFailReasonType
 *          - Parameter describing type of verbose failure reason
 *          - See qaGobiApiTableCallEndReasons.h for Call End Reason Type
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pVerboseFailureReason
 *          - Verbose reason explaining why call failed. Depends on
 *            verbFailReasonType parameter
 *          - See qaGobiApiTableCallEndReasons.h for Call End Reason
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t *psid;
    uint32_t *pFailureReason;
    uint32_t *pVerboseFailReasonType;
    uint32_t *pVerboseFailureReason;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSStartDataSession_t;

/**
 * Start data session
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSStartDataSession(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSStartDataSession_t *reqArg
        );

/**
 * start data session unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSStartDataSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSStartDataSession_t *pOutput
        );

/**
 * This structure contains unpack set packet service status callback information.
 * @param conn_status connection status.
 *              - Current link status. Values:
 *                  - 1 - DISCONNECTED
 *                  - 2 - CONNECTED
 *                  - 3 - SUSPENDED
 *                  - 4 - AUTHENTICATING
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param reconfigReqd Indicates whether the network interface
 *        on the host needs to be reconfigured.
 *                  - Values:
 *                      - 0 - No need to reconfigure.
 *                      - 1 - Reconfiguration required.
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param sessionEndReason Call End Reason
 *                  - See qaGobiApiTableCallEndReasons.h for Call End Reason
 *                  - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param verboseSessnEndReasonType Verbose call end reason type
 *                  - Call end reason type. Values:
 *                      - 0 - Unspecified
 *                      - 1 - Mobile IP
 *                      - 2 - Internal
 *                      - 3 - Call Manager defined
 *                      - 6 - 3GPP Specification defined
 *                      - 7 - PPP
 *                      - 8 - EHRPD
 *                      - 9 - IPv6
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 *                  
 * @param verboseSessnEndReason Reason the call ended (verbose)
 *                  - See qaGobiApiTableCallEndReasons.h for Call End Reason
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param ipFamily IP family of the packet data connection.
 *                  - Values
 *                      - 4 - IPv4
 *                      - 6 - IPv6
 *                  - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @param techName Technology name of the packet data connection.
 *                  - Values
 *                      - 32767 - CDMA
 *                      - 32764 - UMTS
 *                      - 30592 - EPC
 *                      - 30590 - EMBMS
 *                      - 30584 - Modem Link Local
 *                         EPC is a logical interface to support
 *                         LTE/eHRPD handoff. It is returned if the
 *                         device supports IP session continuity.
 *                         Modem Link Local is an interface for
 *                         transferring data between entities on the
 *                         AP and modem.
 *                  - Bit to check in ParamPresenceMask - <B>19</B>
 * @param bearerID 
 *          - bearer ID (3GPP) or RLP ID (3GPP2) of the
 *            packet data connection.
 *          - Valid Values - 0 to 16
 *                  - Bit to check in ParamPresenceMask - <B>20</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t   conn_status;
    uint8_t   reconfigReqd;
    uint16_t  sessionEndReason;
    uint16_t  verboseSessnEndReasonType;
    uint16_t  verboseSessnEndReason;
    uint8_t   ipFamily;
    uint16_t  techName;
    uint8_t   bearerID;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSSetPacketSrvStatusCallback_t;

/**
 * set packet srv status callback unpack
 * @param[in] pResp qmi response
 * @param[in] respLen length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSetPacketSrvStatusCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSSetPacketSrvStatusCallback_t *pOutput
        );

/**
 * This structure contains pack stop data session information.
 *
 * @param psid session id
 */
typedef struct {
    uint32_t *psid;
} pack_wds_SLQSStopDataSession_t;

/**
 * stop data session pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSStopDataSession(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSStopDataSession_t *reqArg
        );

typedef unpack_result_t  unpack_wds_SLQSStopDataSession_t;

/**
 * stop data session unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSStopDataSession(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSStopDataSession_t *pOutput
        );
/**
 * This structure contains the Profile Identifier Information
 *
 *  @param  profileType
 *          - Identifies the type of profile
 *            0x00 = 3GPP
 *
 *  @param  profileIndex
 *          - Index of profile whose settings were loaded prior to
 *            session parameter negotiation for the current call.
 *            If this TLV is not present, data call parameters are
 *            based on device default settings for each parameter
 *
 *  @note Data invalid when all parameters are equal to 0xff.
 *
 */
struct wds_ProfileIdentifier
{
    uint8_t profileType;
    uint8_t profileIndex;
};


/**
 * This structure contains the GPRS Quality Of Service Information
 *
 *  @param  precedenceClass
 *          - Precedence class
 *
 *  @param  delayClass
 *          - Delay class
 *
 *  @param  reliabilityClass
 *          - Reliability class
 *
 *  @param  peakThroughputClass
 *          - Peak throughput class
 *
 *  @param  meanThroughputClass
 *          - Mean throughput class
 *
 *  @note Data invalid when all parameters are equal to 0xffffffff.
 */
struct wds_GPRSQoS
{
    uint32_t precedenceClass;
    uint32_t delayClass;
    uint32_t reliabilityClass;
    uint32_t peakThroughputClass;
    uint32_t meanThroughputClass;
};

/**
 * This structure contains the PCSCFIPv4ServerAddressList Information
 *
 *  @param  numInstances
 *          - number of address following
 *
 *  @param  pscsfIPv4Addr
 *          - P-CSCF IPv4 server addresses(Max 16 address, 4 bytes each)
 *
 *  @note Data invalid when \ref numInstances equal 0xff and \ref pscsfIPv4Addr equal to 0xffffffff.
 */
struct wds_PCSCFIPv4ServerAddressList
{
    uint8_t  numInstances;
    uint32_t pscsfIPv4Addr[64];
};

/**
 * This structure contains the PCSCFFQDNAddress Information
 *
 *  @param  fqdnLen
 *          - length of the received FQDN address
 *
 *  @param  fqdnAddr
 *          - FQDN address(Max 256 characters)
 */
struct wds_PCSCFFQDNAddress
{
    uint16_t fqdnLen;
    uint8_t fqdnAddr[256];
};

/**
 * This structure contains the PCSCFFQDNAddressList Information
 *
 *  @param  numInstances
 *          - Number of FQDN addresses received
 *
 *  @param  pcsfFQDNAddress
 *          - FQDN address information(Max 10 addresses)
 *
 *  @note Data invalid when \ref numInstances equal 0xff.
 */
struct wds_PCSCFFQDNAddressList
{
    uint8_t                    numInstances;
    struct wds_PCSCFFQDNAddress pcsfFQDNAddress[10];
};

/**
 * This structure contains the DomainName Information
 *
 *  @param  domainLen
 *          - length of the recieved Domain name
 *
 *  @param  domainName
 *          - Domain name(Max 256 characters)
 */
struct wds_Domain
{
    uint16_t domainLen;
    uint8_t domainName[256];
};

/**
 * This structure contains the DomainNameList Information
 *
 *  @param  numInstances
 *          - Number of Domain name receieved
 *
 *  @param  domain
 *          - Domain name information(Max 10 Domain names)
 *
 *  @note Data invalid when \ref numInstances equal 0xff.
 */
struct wds_DomainNameList
{
    uint8_t          numInstances;
    struct wds_Domain domain[10];
};

/**
 * This structure contains the IPV6 Address Information
 *
 *  @param  IPV6PrefixLen
 *          - Length of the received IPv6 address in no. of bits;
 *            can take value between 0 and 128
 *              - 0xFF - Not Available
 *
 *  @param  IPAddressV6
 *          - IPv6 address(in network byte order);
 *            This is an 8-element array of 16 bit numbers,
 *            each of which is in big endian format.
 */
struct wds_IPV6AddressInfo
{
    uint8_t   IPV6PrefixLen;
    uint16_t IPAddressV6[8];
};

/**
 * This structure contains the IPV6 Gateway Address Information
 *
 *  @param  gwV6PrefixLen
 *          - Length of the received IPv6 Gateway address in no. of bits;
 *            can take value between 0 and 128
 *
 *  @param  IPAddressV6
 *          - IPv6 Gateway address(in network byte order);
 *            This is an 8-element array of 16 bit numbers,
 *            each of which is in big endian format.
 *
 *  @note Data invalid when IPV6PrefixLen equal 0xff.
 */
struct wds_IPV6GWAddressInfo
{
    uint8_t   gwV6PrefixLen;
    uint16_t gwAddressV6[8];
};

/**
 * This structure contains unpack get runtime settings information.
 * @param IPv4 ipv4 address
 *        - Bit to check in ParamPresenceMask - <B>30</B>
 * @param ProfileName profile name
 *        - Bit to check in ParamPresenceMask - <B>16</B>
 * @param PDPType PDP type
 *        - Bit to check in ParamPresenceMask - <B>17</B>
 * @param APNName APN name
 *        - Bit to check in ParamPresenceMask - <B>20</B>
 * @param PrimaryDNSV4 primary dns IPV4 
 *        - Bit to check in ParamPresenceMask - <B>21</B>
 * @param SecondaryDNSV4 secondary dns IPV4
 *        - Bit to check in ParamPresenceMask - <B>22</B>
 * @param UMTSGrantedQoS UMTS Granted Qos
 *        - Bit to check in ParamPresenceMask - <B>23</B>
 * @param GPRSGrantedQoS GPRS Granted QoS
 *        - Bit to check in ParamPresenceMask - <B>25</B>
 * @param Username username for authentication process
 *        - Bit to check in ParamPresenceMask - <B>27</B>
 * @param Authentication authentication for authentication process
 *        - Bit to check in ParamPresenceMask - <B>29</B>
 * @param ProfielID profile ID
 *        - Bit to check in ParamPresenceMask - <B>31</B>
 * @param GWAddressV4 Gateway IPv4
 *        - Bit to check in ParamPresenceMask - <B>32</B>
 * @param SubnetMaskV4 Subnet mask IPV4
 *        - Bit to check in ParamPresenceMask - <B>33</B>
 * @param PCSCFAddrPCO PCSCF address PCO
 *        - Bit to check in ParamPresenceMask - <B>34</B>
 * @param ServerAddrList PCSCF server address list IPV4
 *        - Bit to check in ParamPresenceMask - <B>35</B>
 * @param PCSCFFQDNAddrList PCSCF FQDN address list IPV4
 *        - Bit to check in ParamPresenceMask - <B>36</B>
 * @param PrimaryDNSV6 Primary DNS IPV6
 *        - Bit to check in ParamPresenceMask - <B>39</B>
 * @param SecondryDNSV6 Secondry DNS IPV6
 *        - Bit to check in ParamPresenceMask - <B>40</B>
 * @param Mtu actual (runtime) Maximum Transfer Unit
 *        - Bit to check in ParamPresenceMask - <B>41</B>
 * @param DomainList domain list
 *        - Bit to check in ParamPresenceMask - <B>42</B>
 * @param IPFamilyPreference ip family preference, it could be IPV4 or IPV6
 *        - Bit to check in ParamPresenceMask - <B>43</B>
 * @param IMCNflag IM control flag, value: TRUE or FALSE
 *        - Bit to check in ParamPresenceMask - <B>44</B>
 * @param Technology technology on which current packet data session is in progress
 *        - Values:
 *          - 32767 - CDMA
 *          - 32764 - UMTS
 *          - 30592 - EPC
 *          - 30584 - modem link local
 *        - Bit to check in ParamPresenceMask - <B>45</B>
 * @param IPV6AddrInfo address information IPV6
 *        - Bit to check in ParamPresenceMask - <B>37</B>
 * @param IPV6GWAddrInfo gateway address information IPV6
 *        - Bit to check in ParamPresenceMask - <B>38</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t                        IPv4;
    uint8_t                         ProfileName[128];
    uint32_t                        PDPType;
    uint8_t                         APNName[128];
    uint32_t                        PrimaryDNSV4;
    uint32_t                        SecondaryDNSV4;
    LibPackUMTSQoS                  UMTSGrantedQoS;
    struct wds_GPRSQoS              GPRSGrantedQoS;
    uint8_t                         Username[128];
    uint32_t                        Authentication;
    struct wds_ProfileIdentifier    ProfileID;
    uint32_t                        GWAddressV4;
    uint32_t                        SubnetMaskV4;
    uint8_t                         PCSCFAddrPCO;
    struct wds_PCSCFIPv4ServerAddressList ServerAddrList;
    struct wds_PCSCFFQDNAddressList       PCSCFFQDNAddrList;
    uint16_t                        PrimaryDNSV6[8];
    uint16_t                        SecondaryDNSV6[8];
    uint32_t                        Mtu;
    struct wds_DomainNameList       DomainList;
    uint8_t                         IPFamilyPreference;
    uint8_t                         IMCNflag;
    uint16_t                        Technology;
    struct wds_IPV6AddressInfo      IPV6AddrInfo;
    struct wds_IPV6GWAddressInfo    IPV6GWAddrInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetRuntimeSettings_t;

/**
 * get runtime settings unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSGetRuntimeSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSGetRuntimeSettings_t *pOutput
        );
/**
 * Network information structure
 *
 *  @param  NetworkType
 *          - Values:
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  @param  RATMask
 *        - RAT mask to indicate type of technology
 *        - Values
 *          - 0x00 - DONT_CARE
 *          - 0x8000 - NULL_BEARER
 *        - CDMA RAT mask
 *          - 0x01 - CDMA_1X
 *          - 0x02 - EVDO_REV0
 *          - 0x05 - HRPD
 *          - 0x0B - EHRPD
 *        - UMTS RAT mask
 *          - 0x03 - GPRS
 *          - 0x04 - WCDMA
 *          - 0x06 - EDGE
 *          - 0x07 - HSDPA and WCDMA
 *          - 0x08 - WCDMA and HSUPA
 *          - 0x09 - HSDPA and HSUPA
 *          - 0x0A - LTE
 *          - 0x0C - HSDPA+ and WCDMA
 *          - 0x0D - HSDPA+ and HSUPA
 *          - 0x0E - DC_HSDPA+ and WCDMA
 *          - 0x0F - DC_HSDPA+ and HSUPA
 *
 *  @param  SOMask
 *          - @ref TableWDSSOMask
 */
typedef struct
{
    uint8_t  NetworkType;
    uint32_t RATMask;
    uint32_t SOMask;
}wds_currNetworkInfo;

/**
 * This structure contains unpack set WDS event callback information.
 *
 * @param xferStatAvail transfer statistic available
 * @param tx_bytes
 *        - Number of bytes transmitted without error
 *        - Bit to check in ParamPresenceMask - <B>25</B>
 * @param rx_bytes
 *        - Number of bytes received without error
 *        - Bit to check in ParamPresenceMask - <B>26</B>
 * @param tx_pkts
 *        - Number of packets transmitted without error
 *        - Bit to check in ParamPresenceMask - <B>16</B>
 * @param rx_pkts
 *        - Number of packets received without error.
 *        - Bit to check in ParamPresenceMask - <B>17</B>
 * @param mipstatAvail Mobile IP status available
 *        - Bit to check in ParamPresenceMask - <B>27</B>
 * @param mipStatus
 *        - Status of the last MIP call (or attempt).
 *        - Values
 *          - 0x00 - Success
 *        - Bit to check in ParamPresenceMask - <B>27</B>
 * @param dBTechAvail Data Bearer technology available
 *        - Bit to check in ParamPresenceMask - <B>23</B>
 * @param dBTechnology
 *        - Data Bearer technology
 *          - Values
 *            - 0x01 - cdma2000 &reg; 1X
 *            - 0x02 - cdma2000 &reg; HRPD (1xEV-DO)
 *            - 0x03 - GSM
 *            - 0x04 - UMTS
 *            - 0x05 - cdma2000 &reg; HRPD (1xEV-DO RevA)
 *            - 0x06 - EDGE
 *            - 0x07 - HSDPA and WCDMA
 *            - 0x08 - WCDMA and HSUPA
 *            - 0x09 - HSDPA and HSUPA
 *            - 0x0A - LTE
 *            - 0x0B - cdma2000 &reg; EHRPD
 *            - 0x0C - HSDPA+ and WCDMA
 *            - 0x0D - HSDPA+ and HSUPA
 *            - 0x0E - DC_HSDPA+ and WCDMA
 *            - 0x0F - DC_HSDAP+ and HSUPA
 *            - 0x10 - HSDPA+ and 64QAM
 *            - 0x11 - HSDPA+, 64QAM and HSUPA
 *            - 0x12 - TDSCDMA
 *            - 0x13 - TDSCDMA and HSDPA
 *            - 0x14 - TDSCDMA and HSUPA
 *            - -1 - Unknown
 *        - Bit to check in ParamPresenceMask - <B>23</B>
 * @param dormancyStatAvail Dormancy status available
 *        - Bit to check in ParamPresenceMask - <B>24</B>
 * @param dormancyStatus
 *        - Dormancy status
 *        - Values
 *          - 1 - Traffic channel dormant
 *          - 2 - Traffic channel active
 *        - Bit to check in ParamPresenceMask - <B>24</B>
 * @param currDBTechAvail
 *        - Current Data Bearer technology available
 *        - Bit to check in ParamPresenceMask - <B>29</B>
 * @param ratMask
 *        - RAT mask to indicate type of technology
 *        - Values
 *          - 0x00 - DONT_CARE
 *          - 0x8000 - NULL_BEARER
 *        - CDMA RAT mask
 *          - 0x01 - CDMA_1X
 *          - 0x02 - EVDO_REV0
 *          - 0x05 - HRPD
 *          - 0x0B - EHRPD
 *        - UMTS RAT mask
 *          - 0x03 - GPRS
 *          - 0x04 - WCDMA
 *          - 0x06 - EDGE
 *          - 0x07 - HSDPA and WCDMA
 *          - 0x08 - WCDMA and HSUPA
 *          - 0x09 - HSDPA and HSUPA
 *          - 0x0A - LTE
 *          - 0x0C - HSDPA+ and WCDMA
 *          - 0x0D - HSDPA+ and HSUPA
 *          - 0x0E - DC_HSDPA+ and WCDMA
 *          - 0x0F - DC_HSDPA+ and HSUPA
 *        - Bit to check in ParamPresenceMask - <B>29</B>
 * @param soMask
 *        - @ref TableWDSSOMask
 *        - Bit to check in ParamPresenceMask - <B>29</B>
 * @param dataSysStatAvail
 *        - Data System Status available
 *        - Bit to check in ParamPresenceMask - <B>36</B>
 * @param prefNetwork
 *        - preferred network
 *        - Values
 *          - 0 - 3GPP
 *          - 1 - 3GPP2
 * @param currNWInfo
 *        - Current Network Info
 *        - see @ref wds_currNetworkInfo for more info
 * @param dBtechExtAvail Data bearer technology extended available
 *        - Bit to check in ParamPresenceMask - <B>42</B>
 * @param dBtechnologyExt
 *        - Data bearer technology Extended.
 *        - Values
 *          - WDS_BEARER_TECH_NETWORK_3GPP (0) - 3GPP
 *          - WDS_BEARER_TECH_NETWORK_3GPP2 (1) - 3GPP2
 *        - Bit to check in ParamPresenceMask - <B>42</B>
 * @param dBTechExtRatValue
 *        - Data bearer technology Extended RAT Value
 *        - Values
 *          - WDS_BEARER_TECH_RAT_EX_NULL_BEARER (0x00) - NULL bearer
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_WCDMA (0x01) - 3GPP WCDMA
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_GERAN (0x02) - 3GPP GERAN
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_LTE (0x03) - 3GPP LTE
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_TDSCDMA (0x04) - 3GPP TDSCDMA
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_WLAN (0x05) - 3GPP WLAN
 *          - WDS_BEARER_TECH_RAT_EX_3GPP_MAX (0x64) - 3GPP maximum
 *          - WDS_BEARER_TECH_RAT_EX_3GPP2_1X (0x65) - 3GPP2 1X
 *          - WDS_BEARER_TECH_RAT_EX_3GPP2_HRPD (0x66) - 3GPP2 HRPD
 *          - WDS_BEARER_TECH_RAT_EX_3GPP2_EHRPD (0x67) - 3GPP2 EHRPD
 *          - WDS_BEARER_TECH_RAT_EX_3GPP2_WLAN (0x68) - 3GPP2 WLAN
 *          - WDS_BEARER_TECH_RAT_EX_3GPP2_MAX (0xC8) - 3GPP2 maximum
 *        - Bit to check in ParamPresenceMask - <B>42</B>
 * @param dBTechExtSoMask
 *        - Data bearer technology Extended SO Mask
 *        - Service Option (SO) mask to indicate the service option
 *          or type of application.An SO mask value of zero indicates
 *          that this field is ignored.
 *        - Values
 *          - 0x00 - SO mask unspecified
 *          - 3GPP SO mask
 *            - 0x01 - WCDMA
 *            - 0x02 - HSDPA
 *            - 0x04 - HSUPA
 *            - 0x08 - HSDPAPLUS
 *            - 0x10 - DC HSDPAPLUS
 *            - 0x20 - 64 QAM
 *            - 0x40 - HSPA
 *            - 0x80 - GPRS
 *            - 0x100 - EDGE
 *            - 0x200 - GSM
 *            - 0x400 - S2B
 *            - 0x800 - LTE limited service
 *            - 0x1000 - LTE FDD
 *            - 0x2000 - LTE TDD
 *          - 3GPP2 SO mask
 *            - 0x01000000 - 1X IS95
 *            - 0x02000000 - 1X IS2000
 *            - 0x04000000 - 1X IS2000 REL A
 *            - 0x08000000 - HDR REV0 DPA
 *            - 0x10000000 - HDR REVA DPA
 *            - 0x20000000 - HDR REVB DPA
 *            - 0x40000000 - HDR REVA MPA
 *            - 0x80000000 - HDR REVB MPA
 *            - 0x100000000 - HDR REVA EMPA
 *            - 0x200000000 - HDR REVB EMPA
 *            - 0x400000000 - HDR REVB MMPA
 *            - 0x800000000 - HDR EVDO FMC
 *        - Bit to check in ParamPresenceMask - <B>42</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t             xferStatAvail;
    uint64_t            tx_bytes;
    uint64_t            rx_bytes;
    uint64_t            tx_pkts;
    uint64_t            rx_pkts;
    uint8_t             mipstatAvail;
    uint32_t            mipStatus;
    uint8_t             dBTechAvail;
    uint32_t            dBTechnology;
    uint8_t             dormancyStatAvail;
    uint32_t            dormancyStatus;
    uint8_t             currDBTechAvail;
    uint32_t            ratMask;
    uint32_t            soMask;
    uint8_t             dataSysStatAvail;
    uint8_t             prefNetwork;
    uint8_t             netInfoLen;
    wds_currNetworkInfo currNWInfo[255];
    uint8_t             dBtechExtAvail;
    uint32_t            dBtechnologyExt;
    uint32_t            dBTechExtRatValue;
    uint64_t            dBTechExtSoMask;
    swi_uint256_t       ParamPresenceMask;
} unpack_wds_SLQSSetWdsEventCallback_ind_t;

/**
 * set event callback unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSetWdsEventCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSSetWdsEventCallback_ind_t *pOutput
        );

typedef unpack_result_t  unpack_wds_SLQSSetWdsEventCallback_t;

/**
 * set event callback unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSetWdsEventCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSSetWdsEventCallback_t *pOutput
        );

/**
 * This structure contains set WDS event callback information.
 *
 * @param dataBearer data bearer
 * @param dormancyStatus dormancy status
 * @param mobileIP mobile IP
 * @param currentDataBearer current data bearer
 * @param dataSystemStatus data system status
 * @param dataBearerTechExt data Bearer Technology Extended
 * @param interval interval
 */
typedef struct{
    uint8_t   dataBearer;
    uint8_t   dormancyStatus;
    uint8_t   mobileIP;
    uint8_t   transferStats;
    uint8_t   currentDataBearer;
    uint8_t   dataSystemStatus;
    uint8_t   dataBearerTechExt;
    uint8_t   interval;
} pack_wds_SLQSSetWdsEventCallback_t;


/**
 * set event callback pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_SLQSSetWdsEventCallback(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSSetWdsEventCallback_t *reqArg
        );

/**
 * This structure contains pack get runtime settings information
 *
 * @param pReqSettings Requested Settings (Optional Parameter)
          - Set bits to 1, corresponding to requested information.
            All other bits must be set to 0.
          - If the values are not available, the corresponding TLVs are
            not returned in the response.
          - Absence of this mask TLV results in the device returning all
            of the available information corresponding to bits 0 through 12.
          - In cases where the information from bit 13 or greater is required,
            this TLV with all the necessary bits set must be present in the request.
          - Values
            - Bit 0 - Profile identifier
            - Bit 1 - Profile name
            - Bit 2 - PDP type
            - Bit 3 - APN name
            - Bit 4 - DNS address
            - Bit 5 - UMTS/GPRS granted QoS
            - Bit 6 - Username
            - Bit 7 - Authentication Protocol
            - Bit 8 - IP address
            - Bit 9 - Gateway info (address and subnet mask)
            - Bit 10 - PCSCF address using PCO flag
            - Bit 11 - PCSCF server address list
            - Bit 12 - PCSCF domain name list
            - Bit 13 - MTU
            - Bit 14 - domain name list
            - Bit 15 - IP family
            - Bit 16 - IM_CM flag
            - Bit 17 - Technology name
            - Bit 18 - Operator reserved PCO (Not Supported on MC/EM73xx)
 */
typedef struct{
    uint32_t         *pReqSettings;
} pack_wds_SLQSGetRuntimeSettings_t;

/**
 * get runtime settings pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetRuntimeSettings(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSGetRuntimeSettings_t     *reqArg
        );

/**
 * This structure contains the UMTS Quality Of Service Information
 *
 *  @param  trafficClass
 *          - 0x00 - Subscribed
 *          - 0x01 - Conversational
 *          - 0x02 - Streaming
 *          - 0x03 - Interactive
 *          - 0x04 - Background
 *
 *  @param  maxUplinkBitrate
 *          - Maximum uplink bit rate in bits/sec
 *
 *  @param  maxDownlinkBitrate
 *          - Maximum downlink bit rate in bits/sec
 *
 *  @param  grntUplinkBitrate
 *          - Guaranteed uplink bit rate in bits/sec
 *
 *  @param  grntDownlinkBitrate
 *          - Guaranteed downlink bit rate in bits/sec
 *
 *  @param  qosDeliveryOrder - Qos delivery order
 *          - 0x00 - Subscribe
 *          - 0x01 - Delivery order on
 *          - 0x02 - Delivery order off
 *
 *  @param  maxSDUSize
 *          - Maximum SDU size
 *
 *  @param  sduErrorRatio - SDU error ratio
 *          - Target value for fraction of SDUs lost or
 *            detected as erroneous.
 *          - 0x00 - Subscribe
 *          - 0x01 - 1*10^(-2)
 *          - 0x02 - 7*10^(-3)
 *          - 0x03 - 1*10^(-3)
 *          - 0x04 - 1*10^(-4)
 *          - 0x05 - 1*10^(-5)
 *          - 0x06 - 1*10^(-6)
 *          - 0x07 - 1*10^(-1)
 *
 *  @param  resBerRatio - Residual bit error ratio
 *          - Target value for undetected bit error ratio in
 *            in the delivered SDUs.
 *          - 0x00 - Subscribe
 *          - 0x01 - 5*10^(-2)
 *          - 0x02 - 1*10^(-2)
 *          - 0x03 - 5*10^(-3)
 *          - 0x04 - 4*10^(-3)
 *          - 0x05 - 1*10^(-3)
 *          - 0x06 - 1*10^(-4)
 *          - 0x07 - 1*10^(-5)
 *          - 0x08 - 1*10^(-6)
 *          - 0x09 - 1*10^(-8)
 *
 *  @param  deliveryErrSDU - delivery of erroneous SDUs
 *          - Indicates whether SDUs detected as erroneous shall be
 *            delivered or not.
 *          - 0x00 - Subscribe
 *          - 0x01 - 5*10^(-2)
 *          - 0x02 - 1*10^(-2)
 *          - 0x03 - 5*10^(-3)
 *          - 0x04 - 4*10^(-3)
 *          - 0x05 - 1*10^(-3)
 *          - 0x06 - 1*10^(-4)
 *          - 0x07 - 1*10^(-5)
 *          - 0x08 - 1*10^(-6)
 *          - 0x09 - 1*10^(-8)
 *
 *  @param  transferDelay - Transfer delay (ms)
 *          - Indicates the targeted time between a request to transfer an
 *            SDU at one SAP to its delivery at the other SAP in milliseconds.
 *
 *  @param  trafficPriority - Transfer handling priority
 *          - Specifies the relative importance for handling of SDUs that
 *            belong to the UMTS bearer, compared to the SDUs of other bearers.
 */
struct wds_UMTSMinQoS
{
    uint8_t  trafficClass;
    uint32_t maxUplinkBitrate;
    uint32_t maxDownlinkBitrate;
    uint32_t grntUplinkBitrate;
    uint32_t grntDownlinkBitrate;
    uint8_t  qosDeliveryOrder;
    uint32_t maxSDUSize;
    uint8_t  sduErrorRatio;
    uint8_t  resBerRatio;
    uint8_t  deliveryErrSDU;
    uint32_t transferDelay;
    uint32_t trafficPriority;
};


/**
 * This structure contains Input/Output parameters of pack_wds_SLQSCreateProfile
 *
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  pProfileName
 *          - One or more bytes describing the profile
 *
 *  @param  pProfilenameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pProfileName field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pPDPType
 *          - Packet Data Protocol (PDP) type specifies the
 *            type of data payload exchanged over the air link
 *            when the packet data session is established with
 *            this profile
 *            - 0x00 - PDP-IP (IPv4)
 *            - 0x01 - PDP-PPP
 *            - 0x02 - PDP-IPV6
 *            - 0x03 - PDP-IPV4V6
 *
 *  @param  pPdpHdrCompType
 *          - PDP header compression type
 *            - 0 - PDP header compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - PDP header compression based on RFC 1144
 *            - 3 - PDP header compression based on RFC 25074
 *                  PDP header compression based on RFC 3095
 *
 *  @param  pPdpDataCompType
 *          - PDP data compression type
 *            - 0 - PDP data compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - V.42BIS data compression
 *            - 3 - V.44 data compression
 *
 *  @param  pAPNName
 *          - Access point name
 *
 *  @param  pAPNnameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pAPNName field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pPriDNSIPv4AddPref
 *          - Primary DNS IPv4 Address Preference
 *
 *  @param  pSecDNSIPv4AddPref
 *          - Secondary DNS IPv4 Address Preference
 *
 *  @param  pUMTSReqQoS
 *          - UMTS Requested QoS
 *
 *  @param  pUMTSMinQoS
 *          - UMTS Minimum QoS
 *
 *  @param  pGPRSRequestedQoS
 *          - GPRS Minimum QoS
 *
 *  @param  pUsername
 *          - User name
 *
 *  @param  pUsernameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pUsername field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pPassword
 *          - Password
 *
 *  @param  pPasswordSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pPassword field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pAuthenticationPref
 *          - Authentication Preference
 *             - Bit map that indicates the authentication
 *               algorithm preference
 *               - Bit 0 - PAP preference
 *                 - 0 - PAP is never performed
 *                 - 1 - PAP may be performed
 *               - Bit 1 - CHAP preference
 *                 - 0 - CHAP is never performed
 *                 - 1 - CHAP may be performed
 *               - If more than one bit is set, then the device decides
 *                 which authentication procedure is performed while setting
 *                 up the data session. For example, the device may have a
 *                 policy to select the most secure authentication mechanism.
 *
 *  @param  pIPv4AddrPref
 *          - IPv4 Address Preference
 *
 *  @param  pPcscfAddrUsingPCO
 *          - P-CSCF Address using PCO Flag
 *            - 1 - (TRUE) implies request PCSCF address using PCO
 *            - 0 - (FALSE) implies do not request
 *                  By default, this value is 0
 *
 *  @param  pPdpAccessConFlag
 *          - PDP access control flag
 *            - 0 - PDP access control none
 *            - 1 - PDP access control reject
 *            - 2 - PDP access control permission
 *
 *  @param  pPcscfAddrUsingDhcp
 *          - P-CSCF address using DHCP
 *            - 1 - (TRUE) implies Request PCSCF address using DHCP
 *            - 0 - (FALSE) implies do not request
 *                  By default, value is 0
 *
 *  @param  pImCnFlag
 *          - IM CN flag
 *            - 1 - (TRUE) implies request IM CN flag for
 *                   this profile
 *            - 0 - (FALSE) implies do not request IM CN
 *                  flag for this profile
 *
 *  @param  pTFTID1Params
 *          - Traffic Flow Template
 *
 *  @param  pTFTID2Params
 *          - Traffic Flow Template
 *
 *  @param  pPdpContext
 *          - PDP context number
 *
 *  @param  pSecondaryFlag
 *          - PDP context secondary flag
 *            - 1 - (TRUE) implies this is secondary profile
 *            - 0 - (FALSE) implies this is not secondary profile
 *
 *  @param  pPrimaryID
 *          - PDP context primary ID
 *          - function SLQSGetProfileSettings() returns a default value
 *            0xFF if this parameter is not returned by the device
 *
 *  @param  pIPv6AddPref
 *          - IPv6 address preference
 *            Preferred IPv6 address to be assigned to the TE; actual
 *            assigned address is negotiated with the network and may
 *            differ from this value; if not specified, the IPv6
 *            address is obtained automatically from the network
 *
 *  @param  pUMTSReqQoSSigInd
 *          - UMTS requested QoS with Signalling Indication flag
 *
 *  @param  pUMTSMinQoSSigInd
 *          - UMTS minimum QoS with Signalling Indication flag
 *
 *  @param  pPrimaryDNSIPv6addpref
 *          - Primary DNS IPv6 address preference
 *            - The value may be used as a preference during
 *              negotiation with the network; if not specified, the
 *              wireless device will attempt to obtain the DNS
 *              address automatically from the network; the
 *              negotiated value is provided to the host via DHCP
 *
 *  @param  pSecondaryDNSIPv6addpref
 *          - Secondary DNS IPv6 address preference
 *
 *  @param  paddrAllocationPref
 *          - DHCP/NAS preference
 *            - This enumerated value may be used to indicate
 *              the address allocation preference
 *               - 0 - NAS signaling is used for address allocation
 *               - 1 - DHCP is used for address allocation
 *
 *  @param  pQosClassID
 *          - 3GPP LTE QoS parameters
 *
 *  @param  pAPNDisabledFlag
 *          - Optional 1 uint8_t Flag indicating if the APN is disabled/enabled
 *          - If set, the profile can not be used for making data calls
 *          - Any data call is failed locally
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - True
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pPDNInactivTimeout
 *          - Optional 4 Bytes indicating the duration of inactivity timer
 *            in seconds
 *          - If the PDP context/PDN connection is inactive for this duration
 *            i.e. No data Tx/Rx occurs, the PDP context/PDN connection is
 *            disconnected
 *          - Default value of zero indicates infinite value
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pAPNClass
 *          - Optional 1 uint8_t numeric identifier representing the APN in profile
 *          - Can be set and queried but is not used by the modem
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pSupportEmergencyCalls
 *          - Optional 1 Byte Flag indicating if the emergency call support is disabled/enabled
 *          - If set, the profile can be used for making emergency calls
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - TRUE
 *
 *
 */
typedef struct
{
    uint8_t                    *pProfilename;
    uint16_t                   *pProfilenameSize;
    uint8_t                    *pPDPtype;
    uint8_t                    *pPdpHdrCompType;
    uint8_t                    *pPdpDataCompType;
    uint8_t                    *pAPNName;
    uint16_t                   *pAPNnameSize;
    uint32_t                   *pPriDNSIPv4AddPref;
    uint32_t                   *pSecDNSIPv4AddPref;
    LibPackUMTSQoS             *pUMTSReqQoS;
    LibPackUMTSQoS             *pUMTSMinQoS;
    LibPackGPRSRequestedQoS    *pGPRSRequestedQos;
    LibPackGPRSRequestedQoS    *pGPRSMinimumQoS;
    uint8_t                    *pUsername;
    uint16_t                    *pUsernameSize;
    uint8_t                    *pPassword;
    uint16_t                   *pPasswordSize;
    uint8_t                    *pAuthenticationPref;
    uint32_t                   *pIPv4AddrPref;
    uint8_t                    *pPcscfAddrUsingPCO;
    uint8_t                    *pPdpAccessConFlag;
    uint8_t                    *pPcscfAddrUsingDhcp;
    uint8_t                    *pImCnFlag;
    LibPackTFTIDParams         *pTFTID1Params;
    LibPackTFTIDParams         *pTFTID2Params;
    uint8_t                    *pPdpContext;
    uint8_t                    *pSecondaryFlag;
    uint8_t                    *pPrimaryID;
    uint16_t                   *pIPv6AddPref;
    LibPackUMTSReqQoSSigInd    *pUMTSReqQoSSigInd;
    LibPackUMTSReqQoSSigInd    *pUMTSMinQosSigInd;
    uint16_t                   *pPriDNSIPv6addpref;
    uint16_t                   *pSecDNSIPv6addpref;
    uint8_t                    *pAddrAllocPref;
    LibPackQosClassID          *pQosClassID;
    uint8_t                    *pAPNDisabledFlag;
    uint32_t                   *pPDNInactivTimeout;
    uint8_t                    *pAPNClass;
    uint8_t                    *pSupportEmergencyCalls;
} LibPackprofile_3GPP;
/**
 * This structure contains the 3GPP2 profile parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  pNegoDnsSrvrPref
 *          - Negotiate DNS Server Preference
 *            - 1 - (TRUE)implies request DNS addresses
 *             from the PDSN
 *            - 0 - (FALSE)implies do not request DNS
 *             addresses from the PDSN
 *            - Default value is 1 (TRUE)
 *
 *  @param  pPppSessCloseTimerDO
 *          - PPP Session Close Timer for DO
 *            - Timer value (in seconds) on DO indicating how
 *              long the PPP Session should linger before
 *              closing down
 *
 *  @param  pPppSessCloseTimer1x
 *          - PPP Session Close Timer for 1X
 *            - Timer value (in seconds) on 1X indicating how
 *              long the PPP session should linger before closing down
 *
 *  @param  pAllowLinger
 *          - Allow/disallow lingering of interface
 *            - 1 -(TRUE) implies allow lingering
 *            - 0 -(FALSE) implies do not allow lingering
 *
 *  @param  pLcpAckTimeout
 *          - LCP ACK Timeout
 *            - Value of LCP ACK Timeout in milliseconds
 *
 *  @param  pIpcpAckTimeout
 *          - IPCP ACK Timeout
 *            - Value of IPCP ACK Timeout in milliseconds
 *
 *  @param  pAuthTimeout
 *          - AUTH Timeout
 *            - Value of Authentication Timeout in milliseconds
 *
 *  @param  pLcpCreqRetryCount
 *          - LCP Configuration Request Retry Count
 *
 *  @param  pIpcpCreqRetryCount
 *          - IPCP Configuration Request Retry Count
 *
 *  @param  pAuthRetryCount
 *          - Authentication Retry Count value
 *
 *  @param  pAuthProtocol
 *          - Authentication Protocol
 *            - 1 - PAP
 *            - 2 - CHAP
 *            - 3 - PAP or CHAP
 *
 *  @param  pUserId
 *          - User ID to be used during data network authentication
 *          - maximum length allowed is 127 bytes;
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the storage on the
 *            wireless device is insufficient in size to hold the value.
 *
 *  @param  pUserIdSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pUserId field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pAuthPassword
 *          - Password to be used during data network authentication;
 *          - maximum length allowed is 127 bytes
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the storage on the
 *            wireless device is insufficient in size to hold the value.
 *
 *  @param  pAuthPasswordSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pAuthPassword field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pDataRate
 *          - Data Rate Requested
 *            - 0 - Low (Low speed Service Options (SO15) only)
 *            - 1 - Medium (SO33 + low R-SCH)
 *            - 2 - High (SO33 + high R-SCH)
 *            - Default is 2
 *
 *  @param  pAppType
 *          - Application Type:
 *            - 0x00000001 - Default Application Type
 *            - 0x00000020 - LBS Application Type
 *            - 0x00000040 - Tethered Application Type
 *            - This parameter is not used while creating/modifying a profile
 *
 *  @param  pDataMode
 *          - Data Mode to use:
 *            - 0 - CDMA or HDR (Hybrid 1X/1xEV-DO)
 *            - 1 - CDMA Only (1X only)
 *            - 2 - HDR Only (1xEV-DO only)
 *            - Default is 0
 *
 *  @param  pAppPriority
 *          - Application Priority
 *            - Numerical 1 uint8_t value defining the application
 *              priority; higher value implies higher priority
 *            - This parameter is not used while creating/modifying a profile
 *
 *  @param  pApnString
 *          - String representing the Access Point Name
 *          - maximum length allowed is 100 bytes
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the APN
 *            name is too long.
 *
 *  @param  pApnStringSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pApnString field. Size of this parameter is 2
 *            bytes.
 *
 *  @param  pPdnType
 *          - Packed Data Network Type Requested:
 *            - 0 - IPv4 PDN Type
 *            - 1 - IPv6 PDN Type
 *            - 2 - IPv4 or IPv6 PDN Type
 *            - 3 - Unspecified PDN Type (implying no preference)
 *
 *  @param  pIsPcscfAddressNedded
 *          - This boolean value is used to control if PCSCF
 *            address is requested from PDSN
 *            - 1 -(TRUE) implies request for PCSCF value from the PDSN
 *            - 0 -(FALSE) implies do not request for PCSCF value from the PDSN
 *
 *  @param  pPrimaryV4DnsAddress
 *          - IPv4 Primary DNS address
 *            - The Primary IPv4 DNS address that can be statically assigned
 *              to the UE
 *
 *  @param  pSecondaryV4DnsAddress
 *          - IPv4 Secondary DNS address
 *            - The Secondary IPv4 DNS address that can be statically assigned
 *              to the UE
 *
 *  @param  pPriV6DnsAddress
 *          - Primary IPv6 DNS address
 *            - The Primary IPv6 DNS address that can be statically assigned
 *              to the UE
 *
 *  @param  pSecV6DnsAddress
 *          - Secondary IPv6 DNS address
 *            - The Secondary IPv6 DNS address that can be statically assigned
 *              to the UE
 *
 *  @param  pRATType
 *          - Optional 1 uint8_t Flag indicating RAT Type
 *          - Values:
 *            - 1 - HRPD
 *            - 2 - EHRPD
 *            - 3 - HRPD_EHRPD
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pAPNEnabled3GPP2
 *          - Optional 1 uint8_t Flag indicating if the APN is disabled/enabled
 *          - If disabled, the profile can not be used for making data calls
 *          - Values:
 *            - 0 - Disabled
 *            - 1 - Enabled(default value)
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pPDNInactivTimeout3GPP2
 *          - Optional 4 Bytes indicating the duration of inactivity timer
 *            in seconds
 *          - If the PDP context/PDN connection is inactive for this duration
 *            i.e. No data Tx/Rx occurs, the PDP context/PDN connection is
 *            disconnected
 *          - Default value of zero indicates infinite value
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 *  @param  pAPNClass3GPP2
 *          - Optional 1 uint8_t numeric identifier representing the APN in profile
 *          - Can be set and queried but is not used by the modem
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *
 */
typedef struct 
{
    uint8_t   *pNegoDnsSrvrPref;
    uint32_t  *pPppSessCloseTimerDO;
    uint32_t  *pPppSessCloseTimer1x;
    uint8_t   *pAllowLinger;
    uint16_t  *pLcpAckTimeout;
    uint16_t  *pIpcpAckTimeout;
    uint16_t  *pAuthTimeout;
    uint8_t   *pLcpCreqRetryCount;
    uint8_t   *pIpcpCreqRetryCount;
    uint8_t   *pAuthRetryCount;
    uint8_t   *pAuthProtocol;
    uint8_t   *pUserId;
    uint16_t  *pUserIdSize;
    uint8_t   *pAuthPassword;
    uint16_t  *pAuthPassword_tSize;
    uint8_t   *pDataRate;
    uint32_t  *pAppType;
    uint8_t   *pDataMode;
    uint8_t   *pAppPriority;
    uint8_t   *pApnString;
    uint16_t  *pApnStringSize;
    uint8_t   *pPdnType;
    uint8_t   *pIsPcscfAddressNedded;
    uint32_t  *pPrimaryV4DnsAddress;
    uint32_t  *pSecondaryV4DnsAddress;
    uint16_t  *pPriV6DnsAddress;
    uint16_t  *pSecV6DnsAddress;
    uint8_t   *pRATType;
    uint8_t   *pAPNEnabled3GPP2;
    uint32_t  *pPDNInactivTimeout3GPP2;
    uint8_t   *pAPNClass3GPP2;
}LibPackprofile_3GPP2;

/**
 * This union  consist of profile_3GPP and profile_3GPP2
 * out of which one will be used to create profile.
 *  @param  SlqsProfile3GPP 3GPP profile
 *          See \ref LibPackprofile_3GPP
 *  @param  SlqsProfile3GPP2 3GPP2 profile
 *          See \ref LibPackprofile_3GPP2
 *
 */
typedef union
{
    LibPackprofile_3GPP  SlqsProfile3GPP;
    LibPackprofile_3GPP2 SlqsProfile3GPP2;
}wds_profileInfo;

/**
 *  This structure contains pack create profile.
 *
 *  @param  pProfileId
 *          - 1 to 16 for 3GPP profile (EM/MC73xx or earlier)
 *          - 1 to 24 for 3GPP profile (EM/MC74xx onwards)
 *          - 101 to 106 for 3GPP2 profile
 *
 *  @param  pProfileType
 *          - Identifies the technology type of the profile
 *            - 0x00 - 3GPP
 *            - 0x01 - 3GPP2
 *            - NULL is not allowed
 *
 *  @param  pCurProfile
 *          - union of 3GPP and 3GPP2 profile
 *          - See \ref wds_profileInfo
 *
 *  @note   - If profileID is NULL, 3GPP profile will be created
 *            and index will be assigned based on availability in device.
 *  @note   - If profileID is not NULL depending on pProfileType 3GPP/3GPP2
 *            relevant profile will be created
 *
 */
typedef struct
{
    uint8_t   *pProfileId;
    uint8_t   *pProfileType;
    wds_profileInfo  *pCurProfile;
} pack_wds_SLQSCreateProfile_t;

/**
 * Create Profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSCreateProfile(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSCreateProfile_t *reqArg
        );

/**
 *  This structure contains out parameter Information
 *
 *  @param  ProfileType
 *          - Identifies the type of profile
 *            0x00 = 3GPP
 *            0x01 = 3GPP2
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ProfileIndex
 *          - Index identifying the profile that was created
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ExtErrorCode
 *          - The extended error code received from DS Profile
 *           subsystem
 *          - Bit to check in ParamPresenceMask - <B>224</B>
 */
typedef struct 
{
    uint8_t   ProfileType;
    uint8_t   ProfileIndex;
    uint16_t  ExtErrorCode;
}PackCreateProfileOut;

/**
 *  This structure contains unpack create profile information.
 *
 *  @param  pCreateProfileOut
 *          - SLQS Create profile Information
 *          - See \ref PackCreateProfileOut
 *
 *  @param  pProfileID
 *          - SLQS profile identifier information
 *
 *  @param  Tlvresult
 *          - TLV present flag, TURE if presented, otherwise FALSE
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct
{
    PackCreateProfileOut *pCreateProfileOut;
    uint8_t  *pProfileID;
    uint16_t   Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSCreateProfile_t;

/**
 * Create Profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response structure to fill
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_wds_SLQSCreateProfile(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSCreateProfile_t *pOutput
        );

/**
 *  This structure contains pack modify profile information.
 *  @param  pProfileId
 *          - 1 to 16 for 3GPP profile (EM/MC73xx or earlier)
 *          - 1 to 24 for 3GPP profile (EM/MC74xx onwards)
 *          - 101 to 106 for 3GPP2 profile
 *
 *  @param  pProfileType
 *          - Identifies the technology type of the profile
 *            - 0x00 - 3GPP
 *            - 0x01 - 3GPP2
 *            - NULL is not allowed
 *
 *  @param  curProfile
 *          - union of 3GPP and 3GPP2 profile
 *          - See \ref wds_profileInfo
 *
 *  @note   - If profileID is NULL, 3GPP profile will be created
 *            and index will be assigned based on availability in device.
 *  @note   - If profileID is not NULL depending on pProfileType 3GPP/3GPP2
 *            relevant profile will be created
 *
 */
typedef struct
{
    uint8_t   *pProfileId;
    uint8_t   *pProfileType;
    wds_profileInfo  curProfile;
} pack_wds_SLQSModifyProfile_t;

/**
 * Modify Profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSModifyProfile(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSModifyProfile_t *reqArg
        );

/**
 * This structure contains out parameters for unpack_wds_SLQSModifyProfile
 *
 *  @param  pExtErrorCode
 *          - The extended error code received from DS Profile subsystem of type
 *            eWDS_ERR_PROFILE_REG_xxx.
 *          - Error code will only will be present if error code
 *            eQCWWAN_ERR_QMI_EXTENDED_INTERNAL is returned by device.
 *          - See \ref qm_wds_ds_profile_extended_err_codes enum in qmerrno.h
 *            for received error description.
 *          - Bit to check in ParamPresenceMask - <B>224</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
   uint16_t  *pExtErrorCode;
   swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSModifyProfile_t;


/**
 * Modify Profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response structure to fill
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_wds_SLQSModifyProfile(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSModifyProfile_t *pOutput
        );

/**
 *  This structure contains pack get profile settings information.
 *
 *  @param  ProfileID
 *          - 1 to 16 for 3GPP profile (EM/MC73xx or earlier)
 *          - 1 to 24 for 3GPP profile (EM/MC74xx onwards)
 *          - 101 to 106 for 3GPP2 profile
 *
 *  @param  ProfileType
 *          - Identifies the technology type of the profile
 *            - 0x00 - 3GPP
 *            - 0x01 - 3GPP2
 *
 *  @note   - If profileID is NULL, 3GPP profile will be fetched
 *            and index will be assigned based on availability in device.
 *  @note   - If profileID is not NULL depending on pProfileType 3GPP/3GPP2
 *            relevant profile will be fetched
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
typedef struct
{
    uint8_t   ProfileId;
    uint8_t   ProfileType;
} pack_wds_SLQSGetProfileSettings_t;

/**
 * Get Profile Settings pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetProfileSettings(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSGetProfileSettings_t *reqArg
        );

/**
 * This structure contains Input parameters of unpack_wds_SLQSGetProfileSettings_t
 * Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  pProfileName
 *          - One or more uint8_ts describing the profile
 *          - NULL pointer - Invalid data.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pProfilenameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pProfileName field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPDPType
 *          - Packet Data Protocol (PDP) type specifies the
 *            type of data payload exchanged over the air link
 *            when the packet data session is established with
 *            this profile
 *            - 0x00 - PDP-IP (IPv4)
 *            - 0x01 - PDP-PPP
 *            - 0x02 - PDP-IPV6
 *            - 0x03 - PDP-IPV4V6
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pPdpHdrCompType
 *          - PDP header compression type
 *            - 0 - PDP header compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - PDP header compression based on RFC 1144
 *            - 3 - PDP header compression based on RFC 25074
 *                  PDP header compression based on RFC 3095
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pPdpDataCompType
 *          - PDP data compression type
 *            - 0 - PDP data compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - V.42BIS data compression
 *            - 3 - V.44 data compression
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pAPNName
 *          - Access point name
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pAPNnameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pAPNName field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pPriDNSIPv4AddPref
 *          - Primary DNS IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pSecDNSIPv4AddPref
 *          - Secondary DNS IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pUMTSReqQoS
 *          - UMTS Requested QoS
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pUMTSMinQoS
 *          - UMTS Minimum QoS
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pGPRSRequestedQoS
 *          - GPRS Requested QoS
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pGPRSMinimumQoS
 *          - GPRS Minimum QoS
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pUsername
 *          - User name
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pUsernameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pUsername field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pPassword
 *          - Password
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pPasswordSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pPassword field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pAuthenticationPref
 *          - Authentication Preference
 *             - Bit map that indicates the authentication
 *               algorithm preference
 *               - Bit 0 - PAP preference
 *                 - 0 - PAP is never performed
 *                 - 1 - PAP may be performed
 *               - Bit 1 - CHAP preference
 *                 - 0 - CHAP is never performed
 *                 - 1 - CHAP may be performed
 *               - If more than one bit is set, then the device decides
 *                 which authentication procedure is performed while setting
 *                 up the data session. For example, the device may have a
 *                 policy to select the most secure authentication mechanism.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pIPv4AddrPref
 *          - IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pPcscfAddrUsingPCO
 *          - P-CSCF Address using PCO Flag
 *            - 1 - (TRUE) implies request PCSCF address using PCO
 *            - 0 - (FALSE) implies do not request
 *                  By default, this value is 0
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pPdpAccessConFlag
 *          - PDP access control flag
 *            - 0 - PDP access control none
 *            - 1 - PDP access control reject
 *            - 2 - PDP access control permission
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  pPcscfAddrUsingDhcp
 *          - P-CSCF address using DHCP
 *            - 1 - (TRUE) implies Request PCSCF address using DHCP
 *            - 0 - (FALSE) implies do not request
 *                  By default, value is 0
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pImCnFlag
 *          - IM CN flag
 *            - 1 - (TRUE) implies request IM CN flag for
 *                   this profile
 *            - 0 - (FALSE) implies do not request IM CN
 *                  flag for this profile
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pTFTID1Params
 *          - Traffic Flow Template
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pTFTID2Params
 *          - Traffic Flow Template
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  pPdpContext
 *          - PDP context number
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  pSecondaryFlag
 *          - PDP context secondary flag
 *            - 1 - (TRUE) implies this is secondary profile
 *            - 0 - (FALSE) implies this is not secondary profile
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  pPrimaryID
 *          - PDP context primary ID
 *          - function SLQSGetProfileSettings() returns a default value
 *            0xFF if this parameter is not returned by the device
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  pIPv6AddPref
 *          - IPv6 address preference
 *            Preferred IPv6 address to be assigned to the TE; actual
 *            assigned address is negotiated with the network and may
 *            differ from this value; if not specified, the IPv6
 *            address is obtained automatically from the network
 *          - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *  @param  pUMTSReqQoSSigInd
 *          - UMTS requested QoS with Signalling Indication flag
 *          - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  @param  pUMTSMinQoSSigInd
 *          - UMTS minimum QoS with Signalling Indication flag
 *          - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *  @param  pPrimaryDNSIPv6addpref
 *          - Primary DNS IPv6 address preference
 *            - The value may be used as a preference during
 *              negotiation with the network; if not specified, the
 *              wireless device will attempt to obtain the DNS
 *              address automatically from the network; the
 *              negotiated value is provided to the host via DHCP
 *          - Bit to check in ParamPresenceMask - <B>43</B>
 *
 *  @param  pSecondaryDNSIPv6addpref
 *          - Secondary DNS IPv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>44</B>
 *
 *  @param  paddrAllocationPref
 *          - DHCP/NAS preference
 *            - This enumerated value may be used to indicate
 *              the address allocation preference
 *               - 0 - NAS signaling is used for address allocation
 *               - 1 - DHCP is used for address allocation
 *          - Bit to check in ParamPresenceMask - <B>45</B>
 *
 *  @param  pQosClassID
 *          - 3GPP LTE QoS parameters
 *          - Bit to check in ParamPresenceMask - <B>46</B>
 *
 *  @param  pAPNDisabledFlag
 *          - Optional 1 uint8_t Flag indicating if the APN is disabled/enabled
 *          - If set, the profile can not be used for making data calls
 *          - Any data call is failed locally
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - True
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>47</B>
 *
 *  @param  pPDNInactivTimeout
 *          - Optional 4 uint8_ts indicating the duration of inactivity timer
 *            in seconds
 *          - If the PDP context/PDN connection is inactive for this duration
 *            i.e. No data Tx/Rx occurs, the PDP context/PDN connection is
 *            disconnected
 *          - Default value of zero indicates infinite value
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>48</B>
 *
 *  @param  pAPNClass
 *          - Optional 1 uint8_t numeric identifier representing the APN in profile
 *          - Can be set and queried but is not used by the modem
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>49</B>
 *
 * @param  pSupportEmergencyCalls
 *          - Optional 1 Byte Flag indicating if the emergency call support is disabled/enabled
 *          - Can be queried to get current status
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - TRUE
 *          - Bit to check in ParamPresenceMask - <B>54</B>
 *
 */
typedef struct 
{
    uint8_t                    *pProfilename;
    uint16_t                    *pProfilenameSize;
    uint8_t                    *pPDPtype;
    uint8_t                    *pPdpHdrCompType;
    uint8_t                    *pPdpDataCompType;
    uint8_t                    *pAPNName;
    uint16_t                    *pAPNnameSize;
    uint32_t                   *pPriDNSIPv4AddPref;
    uint32_t                   *pSecDNSIPv4AddPref;
    LibPackUMTSQoS          *pUMTSReqQoS;
    LibPackUMTSQoS          *pUMTSMinQoS;
    LibPackGPRSRequestedQoS *pGPRSRequestedQos;
    LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
    uint8_t                    *pUsername;
    uint16_t                    *pUsernameSize;
    uint8_t                    *pPassword;
    uint16_t                    *pPasswordSize;
    uint8_t                    *pAuthenticationPref;
    uint32_t                   *pIPv4AddrPref;
    uint8_t                    *pPcscfAddrUsingPCO;
    uint8_t                    *pPdpAccessConFlag;
    uint8_t                    *pPcscfAddrUsingDhcp;
    uint8_t                    *pImCnFlag;
    LibPackTFTIDParams      *pTFTID1Params;
    LibPackTFTIDParams      *pTFTID2Params;
    uint8_t                    *pPdpContext;
    uint8_t                    *pSecondaryFlag;
    uint8_t                    *pPrimaryID;
    uint16_t                  *pIPv6AddPref;
    LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
    LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
    uint16_t                  *pPriDNSIPv6addpref;
    uint16_t                  *pSecDNSIPv6addpref;
    uint8_t                    *pAddrAllocPref;
    LibPackQosClassID       *pQosClassID;
    uint8_t                    *pAPNDisabledFlag;
    uint32_t                   *pPDNInactivTimeout;
    uint8_t                    *pAPNClass;
    uint8_t                    *pSupportEmergencyCalls;
}LibpackProfile3GPP;
/**
 * This structure contains the 3GPP2 profile parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  pNegoDnsSrvrPref
 *          - Negotiate DNS Server Preference
 *            - 1 - (TRUE)implies request DNS addresses
 *             from the PDSN
 *            - 0 - (FALSE)implies do not request DNS
 *             addresses from the PDSN
 *            - Default value is 1 (TRUE)
 *          - Bit to check in ParamPresenceMask - <B>144</B>
 *
 *  @param  pPppSessCloseTimerDO
 *          - PPP Session Close Timer for DO
 *            - Timer value (in seconds) on DO indicating how
 *              long the PPP Session should linger before
 *              closing down
 *          - Bit to check in ParamPresenceMask - <B>145</B>
 *
 *  @param  pPppSessCloseTimer1x
 *          - PPP Session Close Timer for 1X
 *            - Timer value (in seconds) on 1X indicating how
 *              long the PPP session should linger before closing down
 *          - Bit to check in ParamPresenceMask - <B>146</B>
 *
 *  @param  pAllowLinger
 *          - Allow/disallow lingering of interface
 *            - 1 -(TRUE) implies allow lingering
 *            - 0 -(FALSE) implies do not allow lingering
 *          - Bit to check in ParamPresenceMask - <B>147</B>
 *
 *  @param  pLcpAckTimeout
 *          - LCP ACK Timeout
 *            - Value of LCP ACK Timeout in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>148</B>
 *
 *  @param  pIpcpAckTimeout
 *          - IPCP ACK Timeout
 *            - Value of IPCP ACK Timeout in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>149</B>
 *
 *  @param  pAuthTimeout
 *          - AUTH Timeout
 *            - Value of Authentication Timeout in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>150</B>
 *
 *  @param  pLcpCreqRetryCount
 *          - LCP Configuration Request Retry Count
 *          - Bit to check in ParamPresenceMask - <B>151</B>
 *
 *  @param  pIpcpCreqRetryCount
 *          - IPCP Configuration Request Retry Count
 *          - Bit to check in ParamPresenceMask - <B>152</B>
 *
 *  @param  pAuthRetryCount
 *          - Authentication Retry Count value
 *          - Bit to check in ParamPresenceMask - <B>153</B>
 *
 *  @param  pAuthProtocol
 *          - Authentication Protocol
 *            - 1 - PAP
 *            - 2 - CHAP
 *            - 3 - PAP or CHAP
 *          - Bit to check in ParamPresenceMask - <B>154</B>
 *
 *  @param  pUserId
 *          - User ID to be used during data network authentication
 *          - maximum length allowed is 127 uint8_ts;
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the storage on the
 *            wireless device is insufficient in size to hold the value.
 *          - Bit to check in ParamPresenceMask - <B>155</B>
 *
 *  @param  pUserIdSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pUserId field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>155</B>
 *
 *  @param  pAuthPassword
 *          - Password to be used during data network authentication;
 *          - maximum length allowed is 127 uint8_ts
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the storage on the
 *            wireless device is insufficient in size to hold the value.
 *          - Bit to check in ParamPresenceMask - <B>156</B>
 *
 *  @param  pAuthPasswordSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pAuthPassword field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>156</B>
 *
 *  @param  pDataRate
 *          - Data Rate Requested
 *            - 0 - Low (Low speed Service Options (SO15) only)
 *            - 1 - Medium (SO33 + low R-SCH)
 *            - 2 - High (SO33 + high R-SCH)
 *            - Default is 2
 *          - Bit to check in ParamPresenceMask - <B>157</B>
 *
 *  @param  pAppType
 *          - Application Type:
 *            - 0x00000001 - Default Application Type
 *            - 0x00000020 - LBS Application Type
 *            - 0x00000040 - Tethered Application Type
 *            - This parameter is not used while creating/modifying a profile
 *          - Bit to check in ParamPresenceMask - <B>158</B>
 *
 *  @param  pDataMode
 *          - Data Mode to use:
 *            - 0 - CDMA or HDR (Hybrid 1X/1xEV-DO)
 *            - 1 - CDMA Only (1X only)
 *            - 2 - HDR Only (1xEV-DO only)
 *            - Default is 0
 *          - Bit to check in ParamPresenceMask - <B>159</B>
 *
 *  @param  pAppPriority
 *          - Application Priority
 *            - Numerical 1 uint8_t value defining the application
 *              priority; higher value implies higher priority
 *            - This parameter is not used while creating/modifying a profile
 *          - Bit to check in ParamPresenceMask - <B>160</B>
 *
 *  @param  pApnString
 *          - String representing the Access Point Name
 *          - maximum length allowed is 100 uint8_ts
 *          - QMI_ERR_ARG_TOO_LONG will be returned if the APN
 *            name is too long.
 *          - Bit to check in ParamPresenceMask - <B>161</B>
 *
 *  @param  pApnStringSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pApnString field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>161</B>
 *
 *  @param  pPdnType
 *          - Packed Data Network Type Requested:
 *            - 0 - IPv4 PDN Type
 *            - 1 - IPv6 PDN Type
 *            - 2 - IPv4 or IPv6 PDN Type
 *            - 3 - Unspecified PDN Type (implying no preference)
 *          - Bit to check in ParamPresenceMask - <B>162</B>
 *
 *  @param  pIsPcscfAddressNedded
 *          - This boolean value is used to control if PCSCF
 *            address is requested from PDSN
 *            - 1 -(TRUE) implies request for PCSCF value from the PDSN
 *            - 0 -(FALSE) implies do not request for PCSCF value from the PDSN
 *          - Bit to check in ParamPresenceMask - <B>163</B>
 *
 *  @param  pPrimaryV4DnsAddress
 *          - IPv4 Primary DNS address
 *            - The Primary IPv4 DNS address that can be statically assigned
 *              to the UE
 *          - Bit to check in ParamPresenceMask - <B>164</B>
 *
 *  @param  pSecondaryV4DnsAddress
 *          - IPv4 Secondary DNS address
 *            - The Secondary IPv4 DNS address that can be statically assigned
 *              to the UE
 *          - Bit to check in ParamPresenceMask - <B>165</B>
 *
 *  @param  pPriV6DnsAddress
 *          - Primary IPv6 DNS address
 *            - The Primary IPv6 DNS address that can be statically assigned
 *              to the UE
 *          - Bit to check in ParamPresenceMask - <B>166</B>
 *
 *  @param  pSecV6DnsAddress
 *          - Secondary IPv6 DNS address
 *            - The Secondary IPv6 DNS address that can be statically assigned
 *              to the UE
 *          - Bit to check in ParamPresenceMask - <B>167</B>
 *
 *  @param  pRATType
 *          - Optional 1 uint8_t Flag indicating RAT Type
 *          - Values:
 *            - 1 - HRPD
 *            - 2 - EHRPD
 *            - 3 - HRPD_EHRPD
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>168</B>
 *
 *  @param  pAPNEnabled3GPP2
 *          - Optional 1 uint8_t Flag indicating if the APN is disabled/enabled
 *          - If disabled, the profile can not be used for making data calls
 *          - Values:
 *            - 0 - Disabled
 *            - 1 - Enabled(default value)
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>169</B>
 *
 *  @param  pPDNInactivTimeout3GPP2
 *          - Optional 4 uint8_ts indicating the duration of inactivity timer
 *            in seconds
 *          - If the PDP context/PDN connection is inactive for this duration
 *            i.e. No data Tx/Rx occurs, the PDP context/PDN connection is
 *            disconnected
 *          - Default value of zero indicates infinite value
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>170</B>
 *
 *  @param  pAPNClass3GPP2
 *          - Optional 1 uint8_t numeric identifier representing the APN in profile
 *          - Can be set and queried but is not used by the modem
 *          - This parameter is currently read only and can be read by using
 *            the function SLQSGetProfileSettings().
 *          - Bit to check in ParamPresenceMask - <B>171</B>
 *
 */
typedef struct 
{
    uint8_t   *pNegoDnsSrvrPref;
    uint32_t  *pPppSessCloseTimerDO;
    uint32_t  *pPppSessCloseTimer1x;
    uint8_t   *pAllowLinger;
    uint16_t *pLcpAckTimeout;
    uint16_t *pIpcpAckTimeout;
    uint16_t *pAuthTimeout;
    uint8_t   *pLcpCreqRetryCount;
    uint8_t   *pIpcpCreqRetryCount;
    uint8_t   *pAuthRetryCount;
    uint8_t   *pAuthProtocol;
    uint8_t   *pUserId;
    uint16_t   *pUserIdSize;
    uint8_t   *pAuthPassword;
    uint16_t   *pAuthPasswordSize;
    uint8_t   *pDataRate;
    uint32_t  *pAppType;
    uint8_t   *pDataMode;
    uint8_t   *pAppPriority;
    uint8_t   *pApnString;
    uint16_t   *pApnStringSize;
    uint8_t   *pPdnType;
    uint8_t   *pIsPcscfAddressNedded;
    uint32_t  *pPrimaryV4DnsAddress;
    uint32_t  *pSecondaryV4DnsAddress;
    uint16_t *pPriV6DnsAddress;
    uint16_t *pSecV6DnsAddress;
    uint8_t   *pRATType;
    uint8_t   *pAPNEnabled3GPP2;
    uint32_t  *pPDNInactivTimeout3GPP2;
    uint8_t   *pAPNClass3GPP2;
}LibpackProfile3GPP2;

/**
 * This union WdsProfileParam consists of Profile3GPP and Profile3GPP2
 * out of which one will be used to create profile.
 *
 *  @param  SlqsProfile3GPP 3GPP profile
 *          - See \ref LibpackProfile3GPP
 *
 *  @param  SlqsProfile3GPP2 3GPP2 profile
 *          - See \ref LibpackProfile3GPP2
 */
typedef union unpackWdsProfileParam
{
    LibpackProfile3GPP  SlqsProfile3GPP;
    LibpackProfile3GPP2 SlqsProfile3GPP2;
}UnpackQmiProfileInfo;

/**
 * This structure contains the profile settings output
 *
 *  @param  curProfile
 *          - Structure containing details of the current profile
 *          - See \ref UnpackQmiProfileInfo
 *
 *  @param  pExtErrCode
 *          - pointer to a 2 byte extended error code
 *          - Error code will only will be present if error code
 *            eQCWWAN_ERR_QMI_EXTENDED_INTERNAL is returned by device.
 *          - See \ref qm_wds_ds_profile_extended_err_codes enum in qmerrno.h
 *            for received error description.
 *          - Bit to check in ParamPresenceMask - <B>224</B>
 *
 */
typedef struct
{
    UnpackQmiProfileInfo curProfile;
    uint16_t           *pExtErrCode;
}UnPackGetProfileSettingOut;

/**
 * This structure contains the profile setting information of parameter pOutput
 * for API unpack_wds_SLQSGetProfileSettings
 *
 *  @param  pProfileSettings - Profile Settings
 *
 *  @param  ProfileType - Profile Type
 *          - 0 - Profile 3GPP
 *          - 1 - Profile 3GPP2
 *
 *  @param  Tlvresult - unpack Tlv Result.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    UnPackGetProfileSettingOut *pProfileSettings;
    uint8_t  ProfileType;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetProfileSettings_t;

/**
 * get session state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response structure to fill
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_wds_SLQSGetProfileSettings(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSGetProfileSettings_t *pOutput
        );

/**
 * Get Profile Settings pack V2
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetProfileSettingsV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSGetProfileSettings_t *reqArg
        );

/**
 * This structure contains Input parameters of unpack_wds_SLQSGetProfileSettings_t
 * Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  @param  pProfileName
 *          - One or more uint8_ts describing the profile
 *          - NULL pointer - Invalid data.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pProfilenameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pProfileName field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPDPType
 *          - Packet Data Protocol (PDP) type specifies the
 *            type of data payload exchanged over the air link
 *            when the packet data session is established with
 *            this profile
 *            - 0x00 - PDP-IP (IPv4)
 *            - 0x01 - PDP-PPP
 *            - 0x02 - PDP-IPV6
 *            - 0x03 - PDP-IPV4V6
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pPdpHdrCompType
 *          - PDP header compression type
 *            - 0 - PDP header compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - PDP header compression based on RFC 1144
 *            - 3 - PDP header compression based on RFC 25074
 *                  PDP header compression based on RFC 3095
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pPdpDataCompType
 *          - PDP data compression type
 *            - 0 - PDP data compression is OFF
 *            - 1 - Manufacturer preferred compression
 *            - 2 - V.42BIS data compression
 *            - 3 - V.44 data compression
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pAPNName
 *          - Access point name
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pAPNnameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pAPNName field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pPriDNSIPv4AddPref
 *          - Primary DNS IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pSecDNSIPv4AddPref
 *          - Secondary DNS IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pUMTSReqQoS
 *          - UMTS Requested QoS
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pUMTSMinQoS
 *          - UMTS Minimum QoS
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pGPRSRequestedQoS
 *          - GPRS Requested QoS
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pGPRSMinimumQoS
 *          - GPRS Minimum QoS
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pUsername
 *          - User name
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pUsernameSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pUsername field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pPassword
 *          - Password
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pPasswordSize;
 *          - This parameter is an input parameter and should be initialised
 *            to the size of pPassword field. Size of this parameter is 2
 *            uint8_ts.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pAuthenticationPref
 *          - Authentication Preference
 *             - Bit map that indicates the authentication
 *               algorithm preference
 *               - Bit 0 - PAP preference
 *                 - 0 - PAP is never performed
 *                 - 1 - PAP may be performed
 *               - Bit 1 - CHAP preference
 *                 - 0 - CHAP is never performed
 *                 - 1 - CHAP may be performed
 *               - If more than one bit is set, then the device decides
 *                 which authentication procedure is performed while setting
 *                 up the data session. For example, the device may have a
 *                 policy to select the most secure authentication mechanism.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pIPv4AddrPref
 *          - IPv4 Address Preference
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pPcscfAddrUsingPCO
 *          - P-CSCF Address using PCO Flag
 *            - 1 - (TRUE) implies request PCSCF address using PCO
 *            - 0 - (FALSE) implies do not request
 *                  By default, this value is 0
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pPdpAccessConFlag
 *          - PDP access control flag
 *            - 0 - PDP access control none
 *            - 1 - PDP access control reject
 *            - 2 - PDP access control permission
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  pPcscfAddrUsingDhcp
 *          - P-CSCF address using DHCP
 *            - 1 - (TRUE) implies Request PCSCF address using DHCP
 *            - 0 - (FALSE) implies do not request
 *                  By default, value is 0
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pImCnFlag
 *          - IM CN flag
 *            - 1 - (TRUE) implies request IM CN flag for
 *                   this profile
 *            - 0 - (FALSE) implies do not request IM CN
 *                  flag for this profile
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pTFTID1Params
 *          - Traffic Flow Template
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pTFTID2Params
 *          - Traffic Flow Template
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  pPdpContext
 *          - PDP context number
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  pSecondaryFlag
 *          - PDP context secondary flag
 *            - 1 - (TRUE) implies this is secondary profile
 *            - 0 - (FALSE) implies this is not secondary profile
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  pPrimaryID
 *          - PDP context primary ID
            - default value 0xFF if parameter not returned by the device
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  pIPv6AddPref
 *          - IPv6 address preference
 *            Preferred IPv6 address to be assigned to the TE; actual
 *            assigned address is negotiated with the network and may
 *            differ from this value; if not specified, the IPv6
 *            address is obtained automatically from the network
 *          - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *  @param  pUMTSReqQoSSigInd
 *          - UMTS requested QoS with Signalling Indication flag
 *          - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  @param  pUMTSMinQoSSigInd
 *          - UMTS minimum QoS with Signalling Indication flag
 *          - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *  @param  pPrimaryDNSIPv6addpref
 *          - Primary DNS IPv6 address preference
 *            - The value may be used as a preference during
 *              negotiation with the network; if not specified, the
 *              wireless device will attempt to obtain the DNS
 *              address automatically from the network; the
 *              negotiated value is provided to the host via DHCP
 *          - Bit to check in ParamPresenceMask - <B>43</B>
 *
 *  @param  pSecondaryDNSIPv6addpref
 *          - Secondary DNS IPv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>44</B>
 *
 *  @param  paddrAllocationPref
 *          - DHCP/NAS preference
 *            - This enumerated value may be used to indicate
 *              the address allocation preference
 *               - 0 - NAS signaling is used for address allocation
 *               - 1 - DHCP is used for address allocation
 *          - Bit to check in ParamPresenceMask - <B>45</B>
 *
 *  @param  pQosClassID
 *          - 3GPP LTE QoS parameters
 *          - Bit to check in ParamPresenceMask - <B>46</B>
 *
 *  @param  pAPNDisabledFlag
 *          - Optional 1 uint8_t Flag indicating if the APN is disabled/enabled
 *          - If set, the profile can not be used for making data calls
 *          - Any data call is failed locally
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - True
 *          - Bit to check in ParamPresenceMask - <B>47</B>
 *
 *  @param  pPDNInactivTimeout
 *          - Optional 4 uint8_ts indicating the duration of inactivity timer
 *            in seconds
 *          - If the PDP context/PDN connection is inactive for this duration
 *            i.e. No data Tx/Rx occurs, the PDP context/PDN connection is
 *            disconnected
 *          - Default value of zero indicates infinite value
 *          - Bit to check in ParamPresenceMask - <B>48</B>
 *
 *  @param  pAPNClass
 *          - Optional 1 uint8_t numeric identifier representing the APN in profile
 *          - Can be set and queried but is not used by the modem
 *          - Bit to check in ParamPresenceMask - <B>49</B>
 *
 *  @param  pAPNBearer
 *          - Optional 8 Byte numeric APN bearer mask
 *            - 0x0000000000000001 – GSM
 *            - 0x0000000000000002 – WCDMA
 *            - 0x0000000000000004 – LTE
 *            - 0x8000000000000000 – Any
 *          - Bit to check in ParamPresenceMask - <B>53</B>
 *
 * @param  pSupportEmergencyCalls
 *          - Optional 1 Byte Flag indicating if the emergency call support is disabled/enabled
 *          - Can be queried to get current status
 *          - Values:
 *            - 0 - FALSE(default)
 *            - 1 - TRUE
 *          - Bit to check in ParamPresenceMask - <B>54</B>
 *
 *  @param  pOperatorPCOID
 *          - Optional 2 bytes value indicating container ID of this PCO
 *          - Bit to check in ParamPresenceMask - <B>55</B>
 *
 *  @param  pMcc
 *          - Optional 2 bytes value indicating Mobile country code
 *          - Range 0-999
 *          - Bit to check in ParamPresenceMask - <B>56</B>
 *
 *  @param  pMnc
 *          - Optional 2 bytes Mobile network code and 1 byte flag to indicate if MNC
 *            includes PCS digit
 *          - range 0-999
 *          - Bit to check in ParamPresenceMask - <B>57</B>
 *
 *  @param  pMaxPDN
 *          - Optional 2 bytes value indicating Max PDN connections per time block
 *          - Default is 20
 *          - Range 0-1023
 *          - Bit to check in ParamPresenceMask - <B>58</B>
 *
 *  @param  pMaxPDNTimer
 *          - Optional 2 bytes value indicating Max PDN connection timer
 *          - Default is 300 sec
 *          - Range 0-3600 sec
 *          - Bit to check in ParamPresenceMask - <B>59</B>
 *
 *  @param  pPDNWaitTimer
 *          - Optional 2 bytes value indicating PDN request wait interval
 *          - Default is 0 sec
 *          - Range 0-1023 sec
 *          - Bit to check in ParamPresenceMask - <B>60</B>
 *
 *  @param  pAppUserData
 *          - Optional 4 bytes value indicating user data ID in the profile
 *          - Bit to check in ParamPresenceMask - <B>61</B>
 *
 *  @param  pRoamDisallowFlag
 *          - Optional 1 byte value indicating roaming disallowed flag is set or not
 *          - If flag is set as 1 UE is allowed to connect with APN while roaming
 *          - Bit to check in ParamPresenceMask - <B>62</B>
 *
 *  @param  pPDNDisconnectWaitTimer
 *          - Optional 1 bytes value indicating PDN disconnect wait interval
 *          - Range 0-255 minutes
 *          - Bit to check in ParamPresenceMask - <B>63</B>
 *
 *  @param  pDnsWithDHCPFlag
 *          - Optional 1 byte value indicating getting DNS address using DHCP
 *           - 0 - Dont request DNS with DHCP
 *           - 1 - Request DNS address with DHCP
 *          - Bit to check in ParamPresenceMask - <B>64</B>
 *
 *  @param  pLteRoamPDPType
 *          - Optional 4 bytes value indicating LTE roaming PDP type
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4 (0x00) – IPv4
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV6 (0x01) – IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4V6 (0x02) – IPv4 and IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_MAX (0xFF) – Nothing is configured
 *          - Bit to check in ParamPresenceMask - <B>65</B>
 *
 *  @param  pUmtsRoamPDPType
 *          - Optional 4 bytes value indicating UMTS roaming PDP type
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4 (0x00) – IPv4
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV6 (0x01) – IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4V6 (0x02) – IPv4 and IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_MAX (0xFF) – Nothing is configured
 *          - Bit to check in ParamPresenceMask - <B>66</B>
 *
 *  @param  pIWLANtoLTEHandoverFlag
 *          - This boolean value is used to indicate IWLAN to LTE handover
 *            is allowed or not
 *            - 1 -(TRUE) - Allowed
 *            - 0 -(FALSE) - Disallowed
 *          - Bit to check in ParamPresenceMask - <B>67</B>
 *
 *  @param  pLTEtoIWLANHandoverFlag
 *          - This boolean value is used to indicate LTE to IWLAN handover
 *            is allowed or not
 *            - 1 -(TRUE) - Allowed
 *            - 0 -(FALSE) - Disallowed
 *          - Bit to check in ParamPresenceMask - <B>68</B>
 *
 *  @param  pPDNThrottleTimer
 *          - Optional param for the throttle timer values for Max 10 PDN connection
 *          - Bit to check in ParamPresenceMask - <B>69</B>
 *
 *  @param  pOverridePDPType
 *          - Optional 4 bytes value indicating overriding home PDP type
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4 (0x00) – IPv4
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV6 (0x01) – IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_IPV4V6 (0x02) – IPv4 and IPv6
 *            - WDS_COMMON_PDP_TYPE_PDP_MAX (0xFF) – Nothing is configured
 *          - Bit to check in ParamPresenceMask - <B>70</B>
 *
 *  @param  pPCOIDList
 *          - Optional param with 20 bytes, List of 10 PCOs
 *          - Bit to check in ParamPresenceMask - <B>71</B>
 *
 *  @param  pMsisdnFlag
 *          - This boolean value is used to indicate MSISDN flag
 *            - 1 -(TRUE) - Enabled
 *            - 0 -(FALSE) - Disabled
 *          - Bit to check in ParamPresenceMask - <B>72</B>
 *
 *  @param  pPersistFlag
 *          - This boolean value is used to indicate profile persistent flag
 *            - 1 -(TRUE) - Enabled
 *            - 0 -(FALSE) - Disabled
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  pClatFlag
 *          - This boolean value is used to indicate if CLAT
 *            is enabled or not
 *            - 1 -(TRUE) - Enabled
 *            - 0 -(FALSE) - Disabled
 *          - Bit to check in ParamPresenceMask - <B>222</B>
 *
 *  @param  pIPV6DelegFlag
 *          - This boolean value is used to indicate if IPV6 prefix
 *            delegation flag is enabled or not
 *            - 1 -(TRUE) - Enabled
 *            - 0 -(FALSE) - Disabled
 *          - Bit to check in ParamPresenceMask - <B>223</B>
 *
 */
typedef struct
{
    uint8_t                    *pProfilename;
    uint16_t                    *pProfilenameSize;
    uint8_t                    *pPDPtype;
    uint8_t                    *pPdpHdrCompType;
    uint8_t                    *pPdpDataCompType;
    uint8_t                    *pAPNName;
    uint16_t                    *pAPNnameSize;
    uint32_t                   *pPriDNSIPv4AddPref;
    uint32_t                   *pSecDNSIPv4AddPref;
    LibPackUMTSQoS          *pUMTSReqQoS;
    LibPackUMTSQoS          *pUMTSMinQoS;
    LibPackGPRSRequestedQoS *pGPRSRequestedQos;
    LibPackGPRSRequestedQoS *pGPRSMinimumQoS;
    uint8_t                    *pUsername;
    uint16_t                    *pUsernameSize;
    uint8_t                    *pPassword;
    uint16_t                    *pPasswordSize;
    uint8_t                    *pAuthenticationPref;
    uint32_t                   *pIPv4AddrPref;
    uint8_t                    *pPcscfAddrUsingPCO;
    uint8_t                    *pPdpAccessConFlag;
    uint8_t                    *pPcscfAddrUsingDhcp;
    uint8_t                    *pImCnFlag;
    LibPackTFTIDParams      *pTFTID1Params;
    LibPackTFTIDParams      *pTFTID2Params;
    uint8_t                    *pPdpContext;
    uint8_t                    *pSecondaryFlag;
    uint8_t                    *pPrimaryID;
    uint16_t                  *pIPv6AddPref;
    LibPackUMTSReqQoSSigInd *pUMTSReqQoSSigInd;
    LibPackUMTSReqQoSSigInd *pUMTSMinQosSigInd;
    uint16_t                  *pPriDNSIPv6addpref;
    uint16_t                  *pSecDNSIPv6addpref;
    uint8_t                    *pAddrAllocPref;
    LibPackQosClassID       *pQosClassID;
    uint8_t                    *pAPNDisabledFlag;
    uint32_t                   *pPDNInactivTimeout;
    uint8_t                    *pAPNClass;
    uint64_t                   *pAPNBearer;
    uint8_t                    *pSupportEmergencyCalls;
    uint16_t                    *pOperatorPCOID;
    uint16_t                    *pMcc;
    LibPackProfileMnc           *pMnc;
    uint16_t                    *pMaxPDN;
    uint16_t                    *pMaxPDNTimer;
    uint16_t                    *pPDNWaitTimer;
    uint32_t                   *pAppUserData;
    uint8_t                    *pRoamDisallowFlag;
    uint8_t                    *pPDNDisconnectWaitTimer;
    uint8_t                    *pDnsWithDHCPFlag;
    uint32_t                   *pLteRoamPDPType;
    uint32_t                   *pUmtsRoamPDPType;
    uint8_t                    *pIWLANtoLTEHandoverFlag;
    uint8_t                    *pLTEtoIWLANHandoverFlag;
    LibPackPDNThrottleTimer    *pPDNThrottleTimer;
    uint32_t                   *pOverridePDPType;
    LibPackPCOIDList           *pPCOIDList;
    uint8_t                    *pMsisdnFlag;
    uint8_t                    *pPersistFlag;
    uint8_t                    *pClatFlag;
    uint8_t                    *pIPV6DelegFlag;
}LibpackProfile3GPPV2;


/**
 * This union WdsProfileParam consists of Profile3GPP and Profile3GPP2
 * out of which one will be used to create profile.
 *
 *  @param  SlqsProfile3GPP 3GPP profile
 *          - See \ref LibpackProfile3GPP
 *
 *  @param  SlqsProfile3GPP2 3GPP2 profile
 *          - See \ref LibpackProfile3GPP2
 */
typedef union unpackWdsProfileParamV2
{
    LibpackProfile3GPPV2  SlqsProfile3GPP;
    LibpackProfile3GPP2 SlqsProfile3GPP2;
}UnpackQmiProfileInfoV2;

/**
 * This structure contains the profile settings output
 *
 *  @param  curProfile
 *          - Structure containing details of the current profile
 *          - See \ref UnpackQmiProfileInfoV2
 *
 *  @param  pExtErrCode
 *          - pointer to a 2 byte extended error code
 *          - Error code will only will be present if error code
 *            eQCWWAN_ERR_QMI_EXTENDED_INTERNAL is returned by device.
 *          - See \ref qm_wds_ds_profile_extended_err_codes enum in qmerrno.h
 *            for received error description.
 *          - Bit to check in ParamPresenceMask - <B>224</B>
 *
 */
typedef struct
{
    UnpackQmiProfileInfoV2 curProfile;
    uint16_t           *pExtErrCode;
}UnPackGetProfileSettingOutV2;

/**
 * This structure contains the profile setting information of parameter pOutput
 * for API unpack_wds_SLQSGetProfileSettingsV2
 *
 *  @param  pProfileSettings - Profile Settings
 *
 *  @param  ProfileType - Profile Type
 *          - 0 - Profile 3GPP
 *          - 1 - Profile 3GPP2
 *
 *  @param  Tlvresult - unpack Tlv Result.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    UnPackGetProfileSettingOutV2 *pProfileSettings;
    uint8_t  ProfileType;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetProfileSettingsV2_t;

/**
 * get profile settings unpack V2
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response structure to fill
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_wds_SLQSGetProfileSettingsV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSGetProfileSettingsV2_t *pOutput
        );


/**
 * This structure contains unpack get session state information.
 *
 * @param connectionStatus - state of the current packet data session.
 *        - 1 - Disconnected.
 *        - 2 - Connected.
 *        - 3 - Suspended.
 *        - 4 - Authenticating.
 *        - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t    connectionStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetSessionState_t;

/**
 * get session state pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetSessionState(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * get session state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetSessionState(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetSessionState_t *pOutput
        );

/**
 * This structure contains pack get default profile information.
 *
 * @param profiletype profile type
 *        - 0 - WDS_PROFILE_TYPE_3GPP (0x00) - 3GPP
 *        - 1 - WDS_PROFILE_TYPE_3GPP (0x01) - 3GPP2
 *        - 2 - WDS_PROFILE_TYPE_EPC  (0x02) - EPC
 */
typedef struct {
    uint32_t profiletype;
} pack_wds_GetDefaultProfile_t;

/**
 *  Reads the default profile settings from the device. The default profile is
 *  used to establish an auto connect data session.
 *
 *  @param  pdptype
 *          - Packet Data Protocol (PDP) type specifies the type of data payload
 *            exchanged over the air link when the packet data session is
 *            established with this profile
 *              - 0 - PDP-IP (IPv4)
 *              - 1 - PDP-PPP
 *              - 2 - PDP-IPv6
 *              - 3 - PDP-IPv4v6
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ipaddr
 *          - Preferred IPv4 address to be assigned to device
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pridns
 *          - Primary DNS IPv4 address preference
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  secdns
 *          - Secondary DNS IPv4 address preference
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  ipaddrv6
 *          - Preferred IPv6 address to be assigned to device
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  pridnsv6
 *          - Primary DNS Pv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  secdnsv6
 *          - Secondary DNS IPv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  auth
 *          - Bitmap that indicates authentication algorithm preference
 *              - 0x00000001 - PAP preference
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - 0x00000002 - CHAP preference
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - All other bits are reserved and must be set to 0
 *              - If more than 1 bit is set, then device decides which
 *                authentication procedure is performed while setting up data
 *                session e.g. the device may have a policy to select the most
 *                secure authentication mechanism.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  namesize
 *          - Maximum number of characters (including NULL terminator) that
 *            profile name array can contain.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  name
 *          - Profile name
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  apnsize
 *          - Maximum number of characters (including NULL terminator) that APN
 *            name array can contain
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  apnname
 *          - Access point name. NULL-terminated string parameter that is a
 *            logical name used to select GGSN and external packet data
 *            network.
 *          - If value is NULL or omitted, then subscription default value will
 *            be requested.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  usersize
 *          - Maximum number of characters (including NULL terminator) that
 *            username array can contain.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  username
 *          - Username used during network authentication
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t         pdptype;
    uint32_t         ipaddr;
    uint32_t         pridns;
    uint32_t         secdns;
    uint16_t         ipaddrv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint16_t         pridnsv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint16_t         secdnsv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint32_t         auth;
    uint8_t          namesize;
    int8_t           name[255];
    uint8_t          apnsize;
    int8_t           apnname[255];
    uint8_t          usersize;
    int8_t           username[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetDefaultProfile_t;

/**
 * get default profile pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetDefaultProfile(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetDefaultProfile_t *reqParam
        );

/**
 * get default profile unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetDefaultProfile(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetDefaultProfile_t *pOutput
        );

/**
 * This structure contains pack get default profile information.
 *
 * @param profiletype profile type
 *        - 0 - WDS_PROFILE_TYPE_3GPP (0x00) - 3GPP
 *        - 1 - WDS_PROFILE_TYPE_3GPP (0x01) - 3GPP2
 *        - 2 - WDS_PROFILE_TYPE_EPC  (0x02) - EPC
 */
typedef struct {
    uint32_t profiletype;
} pack_wds_GetDefaultProfileV2_t;

/**
 *  Reads the default profile settings from the device. The default profile is
 *  used to establish an auto connect data session.
 *
 *  @param  pdptype
 *          - Packet Data Protocol (PDP) type specifies the type of data payload
 *            exchanged over the air link when the packet data session is
 *            established with this profile
 *              - 0 - PDP-IP (IPv4)
 *              - 1 - PDP-PPP
 *              - 2 - PDP-IPv6
 *              - 3 - PDP-IPv4v6
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ipaddr
 *          - Preferred IPv4 address to be assigned to device
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pridns
 *          - Primary DNS IPv4 address preference
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  secdns
 *          - Secondary DNS IPv4 address preference
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  ipaddrv6
 *          - Preferred IPv6 address to be assigned to device
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  pridnsv6
 *          - Primary DNS Pv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  secdnsv6
 *          - Secondary DNS IPv6 address preference
 *          - Bit to check in ParamPresenceMask - <B>143</B>
 *
 *  @param  auth
 *          - Bitmap that indicates authentication algorithm preference
 *              - 0x00000001 - PAP preference
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - 0x00000002 - CHAP preference
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - All other bits are reserved and must be set to 0
 *              - If more than 1 bit is set, then device decides which
 *                authentication procedure is performed while setting up data
 *                session e.g. the device may have a policy to select the most
 *                secure authentication mechanism.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  namesize
 *          - Maximum number of characters (including NULL terminator) that
 *            profile name array can contain.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  name
 *          - Profile name
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  apnsize
 *          - Maximum number of characters (including NULL terminator) that APN
 *            name array can contain
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  apnname
 *          - Access point name. NULL-terminated string parameter that is a
 *            logical name used to select GGSN and external packet data
 *            network.
 *          - If value is NULL or omitted, then subscription default value will
 *            be requested.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  usersize
 *          - Maximum number of characters (including NULL terminator) that
 *            username array can contain.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  username
 *          - Username used during network authentication
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pwdsize
 *          - Maximum number of characters (including NULL terminator) that
 *            pwd name array can contain.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pwd
 *          - Password used during network authentication
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t         pdptype;
    uint32_t         ipaddr;
    uint32_t         pridns;
    uint32_t         secdns;
    uint16_t         ipaddrv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint16_t         pridnsv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint16_t         secdnsv6[IPV6_ADDRESS_ARRAY_SIZE];
    uint32_t         auth;
    uint8_t          namesize;
    int8_t           name[MAX_WDS_NAME_ARRAY_SIZE];
    uint8_t          apnsize;
    int8_t           apnname[MAX_WDS_NAME_ARRAY_SIZE];
    uint8_t          usersize;
    int8_t           username[MAX_WDS_NAME_ARRAY_SIZE];
    uint8_t          pwdsize;
    int8_t           pwd[MAX_WDS_NAME_ARRAY_SIZE];
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetDefaultProfileV2_t;

/**
 * get default profile pack V2.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetDefaultProfileV2(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetDefaultProfileV2_t *reqParam
        );

/**
 * get default profile unpack V2.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetDefaultProfileV2(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetDefaultProfileV2_t *pOutput
        );

/**
 *  This structure contains unpack get connection rate information.
 *
 *  @param  currentChannelTXRate
 *          - Current channel Tx rate (in bps)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  currentChannelRXRate
 *          - Current channel Rx rate (in bps)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  maxChannelTXRate
 *          - Maximum Tx rate (bps) that may be assigned  to device by serving
 *            system.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  maxChannelRXRate
 *          - Maximum Rx rate (bps) that may be assigned to device by serving
 *            system.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t         currentChannelTXRate;
    uint32_t         currentChannelRXRate;
    uint32_t         maxChannelTXRate;
    uint32_t         maxChannelRXRate;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetConnectionRate_t;

/**
 * get connection rate pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetConnectionRate(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * get connection rate unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetConnectionRate(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetConnectionRate_t *pOutput
        );

/**
 * This structure contains pack get packet status information.
 *
 * @param statmask packet statistics mask
 *    - 0x00000001 - Tx packets OK
 *    - 0x00000002 - Rx packets OK
 *    - 0x00000004 - Tx packet errors
 *    - 0x00000008 - Rx packet errors
 *    - 0x00000010 - Tx overflows
 *    - 0x00000020 - Rx overflows
 *    - 0x00000040 - Tx bytes OK
 *    - 0x00000080 - Rx bytes OK
 *    - 0x00000100 - Tx packets dropped
 *    - 0x00000200 - Rx packets dropped
 */
typedef struct{
    uint32_t         statmask;
} pack_wds_GetPacketStatus_t;

/**
 *  Returns the packet data transfer statistics since the start of the current
 *  packet data.
 *
 *  @param  tXPacketSuccesses
 *          - No. of packets transmitted without error
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  rXPacketSuccesses
 *          - No. of packets received without error
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  tXPacketErrors
 *          - No. of outgoing packets with framing errors
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  rXPacketErrors
 *          - No. of incoming packets with framing errors
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  txPacketOverflows
 *          - Number of packets dropped  because Tx buffer overflowed
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  rxPacketOverflows
 *          - Number of packets dropped because Rx buffer overflowed
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  txOkBytesCount
 *          - No of bytes transmitted without error.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  rxOkBytesCount
 *          - No of bytes received without error.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  txOKBytesLastCall
 *          - No of bytes transmitted without error during the
 *            last data call (0 if no call was made earlier).
 *            Returned only if not in a call, and when the previous call was
 *            made using RmNet (for any devices that support
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  rxOKBytesLastCall
 *          - Number of bytes received without error
 *            during the last data call (0 if no call was
 *            made earlier). Returned only if not in a
 *            call, and when the previous call was
 *            made using RmNet (for any devices that
 *            support
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  txDroppedCount
 *          - Number of outgoing packets dropped.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  rxDroppedCount
 *          - Number of incoming packets dropped.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t        tXPacketSuccesses;
    uint32_t        rXPacketSuccesses;
    uint32_t        tXPacketErrors;
    uint32_t        rXPacketErrors;
    uint32_t        tXPacketOverflows;
    uint32_t        rXPacketOverflows;
    uint64_t        tXOkBytesCount;
    uint64_t        rXOkBytesCount;
    uint64_t        tXOKBytesLastCall;
    uint64_t        rXOKBytesLastCall;
    uint32_t        tXDroppedCount;
    uint32_t        rXDroppedCount;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetPacketStatus_t;

/**
 * get packet status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetPacketStatus(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetPacketStatus_t *reqParam
        );

/**
 * get packet status unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetPacketStatus(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetPacketStatus_t *pOutput
        );

/**
 *  This structure contains unpack get session duration information.
 *
 *  @param  callDuration
 *          - Duration of the current packet session in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint64_t      callDuration;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetSessionDuration_t;

/**
 *  This structure contains pack get session duration information.
 *
 *  @param  NULL
 *          - this is a dummy structure
 */
typedef struct{

}pack_wds_GetSessionDuration_t;

/**
 * get session duration pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetSessionDuration(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetSessionDuration_t *reqParam
        );

/**
 * get session duration unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetSessionDuration(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetSessionDuration_t *pOutput
        );

/**
 *  This structure contains unpack get session duration information -V2.
 *
 *  @param  callDuration
 *          - Duration of the current packet session in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pLastCallDuration
 *          - Duration of the last data session in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pCallActiveDuration
 *          - Duration of the active time of current data session in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pLastCallActiveDuration
 *          - Duration of the active time of last data session in milliseconds
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint64_t      callDuration;
    uint64_t      *pLastCallDuration;
    uint64_t      *pCallActiveDuration;
    uint64_t      *pLastCallActiveDuration;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetSessionDurationV2_t;


/**
 * get session duration pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetSessionDurationV2(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetSessionDuration_t *reqParam
        );

/**
 * get session duration unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetSessionDurationV2(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetSessionDurationV2_t *pOutput
        );


/**
 *  This structure contains unpack get dormancy state information.
 *
 *  @param  dormancyState
 *          - Dormancy state of current packet data session
 *              - 1 - Traffic channel dormant
 *              - 2 - Traffic channel active
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t    dormancyState;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetDormancyState_t;

/**
 *  This structure contains pack get dormancy state information.
 *
 *  @param  NULL
 *          - this is a dummy structure
 */
typedef struct{
    
}pack_wds_GetDormancyState_t;

/**
 * get dormancy state pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetDormancyState(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetDormancyState_t *reqParam
        );

/**
 * get dormancy state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetDormancyState(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetDormancyState_t *pOutput
        );

/**
 * This structure contains the information about the profile to be deleted.
 *
 *  @param  profileType
 *          - Identifies the type of profile
 *            - 0x00 - 3GPP
 *          - Note: Deletion of 3GPP2 profiles is not supported.
 *
 *  @param  profileIndex
 *          - Index of the configured profile to be deleted
 *               - Value between 1 - 16 (EM/MC73xx or earlier)
 *               - Value between 1 - 24 (EM/MC74xx onwards)
 *
 */
typedef struct{
    uint8_t profileType;
    uint8_t profileIndex;
} pack_wds_SLQSDeleteProfile_t;

/**
 *  This structure contains unpack delete profile information.
 *
 *  @param  extendedErrorCode[OUT]
 *          - The extended error code received from DS Profile subsystem of type
 *            eWDS_ERR_PROFILE_REG_xxx.
 *          - Error code will only will be present if error code
 *            eQCWWAN_ERR_QMI_EXTENDED_INTERNAL is returned by device.
 *          - See \ref qm_wds_ds_profile_extended_err_codes enum in qmerrno.h
 *            for received error description.
 *          - Bit to check in ParamPresenceMask - <B>224</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint16_t extendedErrorCode;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSDeleteProfile_t;

/**
 * delete stored profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSDeleteProfile(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_SLQSDeleteProfile_t *reqParam
        );

/**
 * delete stored profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSDeleteProfile(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSDeleteProfile_t *pOutput
        );

/**
 *  Writes the default profile settings to the device. The default profile is
 *  used to establish an autoconnect data session.
 *
 *  @param  profileType
 *          - Type of profile
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  @param  pdpType
 *          - Packet Data Protocol (PDP) type specifies the type of data payload
 *            exchanged over the air link when the packet data session is
 *            established with this profile (optional)
 *              - 0 - PDP-IP (IPv4)
 *              - 1 - PDP-PPP
 *              - 2 - PDP-IPv6
 *              - 3 - PDP-IPv4v6
 *
 *  @param  ipAddress
 *          - Preferred IPv4 addr to be assigned to device (optional)
 *
 *  @param  primaryDNS
 *          - Primary DNS Ipv4 address preference (optional)
 *
 *  @param  secondaryDNS
 *          - Secondary DNS Ipv4 address preference (optional)
 *
 *  @param  authentication
 *          - Bitmap that indicates authentication algorithm preference (optional)
 *              - 0x00000001 - PAP preference\n
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - 0x00000002 - CHAP preference\n
 *                  - 0 - Never performed
 *                  - 1 - May be performed
 *              - All other bits are reserved and must be set to 0
 *              - If more than 1 bit is set, then device decides which
 *                authentication procedure is performed while setting up data
 *                session e.g. the device may have a policy to select the most
 *                secure authentication mechanism.
 *
 *  @param  pName
 *          - profile Name (optional)
 *
 *  @param  pUsername
 *          - Username used during network authentication (optional)
 *
 *  @param  pAPNName
 *          - Access point name. NULL-terminated string parameter that is a
 *            logical name used to select GGSN and external packet data
 *            network (optional)
 *          - If value is NULL or omitted, then subscription default value will
 *            be requested.
 *
 *  @param  pPassword
 *          - Password used during network authentication (optional)
 */
typedef struct{
    uint32_t    profileType;
    uint32_t    pdpType;
    uint32_t    ipAddress;
    uint32_t    primaryDNS;
    uint32_t    secondaryDNS;
    uint32_t    authentication;
    uint8_t     *pName;
    uint8_t     *pUsername;
    uint8_t     *pApnname;
    uint8_t     *pPassword;
} pack_wds_SetDefaultProfile_t;

/**
 * set default profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SetDefaultProfile(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_SetDefaultProfile_t *reqParam
        );

typedef unpack_result_t unpack_wds_SetDefaultProfile_t;

/**
 * set default profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetDefaultProfile(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SetDefaultProfile_t *pOutput
        );

/**
 * This structure contains unpack get 3GPP configure item information.
 *
 * @param  LTEAttachProfile
 *        - LTE Attach Profile
 *            - points to a single WORD Value indicating the
 *              attached LTE Profile
 *            - Optional parameter with possible values 1-16 (EM/MC73xx or earlier)
 *        - This setting is deprecated on MC/EM74xx
 *        - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param profileList Profile List
 *        - an array of 4 profile configurations
 *        - Each element points to a single WORD value indicating profile
 *        - Optional parameter with possible values
 *            - 1 - 16 (MC/EM73xx and before)
 *            - 1 - 24 (MC/EM74xx and onwards)
 *        - function SLQSGet3GPPConfigItem() returns a default value 255
 *          if no 3gpp configuration is present
 *        - Bit to check in ParamPresenceMask - <B>17</B>
 *        - Note: the 5th entry is currently ignored, please set it to zero
 *
 * @param[out] defaultPDNEnabled 
 *        - 0 - disabled 
 *        - 1 - enabled
 *        - Bit to check in ParamPresenceMask - <B>18</B>
 *
 * @param[out] _3gppRelease 3GPP release 
 *        - 0 - Release_99 
 *        - 1 - Release_5 
 *        - 2 - Release_6 
 *        - 3 - Release_7 
 *        - 4 - Release_8 
 *        - 5 - Release_9  (In 9x30 and toworads)
 *        - 6 - Release_10 (In 9x30 and toworads)
 *        - 7 - Release_11 (In 9x30 and toworads)
 *        - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * @param[out] LTEAttachProfileList
 *          - pointer to WORD array indiciating LTE Attach Profile List
 *              - Optional parameter
 *              - possible values: 1-24
 *              - This setting is only supported for MC/EM74xx onwards
 *              - Please provide attach profiles in order of decreasing priority in this list. 
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * @param[in,out] LTEAttachProfileListLen
 *          - Number of element in pLTEAttachProfileList
 *             - valid range: 1-24
 *             - This setting is only supported for MC/EM74xx onwards
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint16_t LTEAttachProfile;
    uint16_t profileList[5];
    uint8_t  defaultPDNEnabled;
    uint8_t  _3gppRelease;
    uint16_t LTEAttachProfileList[MAX_WDS_3GPP_CONF_LTE_ATTACH_PROFILE_LIST_SIZE];
    uint16_t LTEAttachProfileListLen;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGet3GPPConfigItem_t;

/**
 * get 3Gpp config items pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGet3GPPConfigItem(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * get 3GPP config items unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSGet3GPPConfigItem(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSGet3GPPConfigItem_t *pOutput
        );

/**
 * This structure contains pack set 3GPP configuration item information.
 *
 * @param  pLTEAttachProfile
 *        - Optional parameter
 *        - LTE Attach Profile
 *            - points to a single WORD Value indicating the
 *              attached LTE Profile
 *            - Optional parameter with possible values 1-16 (EM/MC73xx or earlier)
 *        - This setting is deprecated on MC/EM74xx
 *
 * @param ProfileList Profile List
 *        - an array of 4 profile configurations
 *        - Each element points to a single WORD value indicating profile
 *        - Optional parameter with possible values
 *            - 1 - 16 (MC/EM73xx and before)
 *            - 1 - 24 (MC/EM74xx and onwards)
 *        - function SLQSGet3GPPConfigItem() returns a default value 255
 *          if no 3gpp configuration is present
 *
 * @param pDefaultPDNEnabled
 *        - Optional parameter
 *          - 0 - disabled
 *          - 1 - enabled
 *
 * @param p3gppRelease 3GPP release
 *        - Optional parameter
 *          - 0 - Release_99
 *          - 1 - Release_5
 *          - 2 - Release_6
 *          - 3 - Release_7
 *          - 4 - Release_8
 *        - In 9x30 and onwards
 *          - 5 - Release 9
 *          - 6 - Release 10
 *          - 7 - Release 11
 *
 * @param pLTEAttachProfileList
 *          - pointer to WORD array indicating LTE Attach Profile List
 *              - Optional parameter
 *              - possible values: 1-24
 *              - This setting is only supported for MC/EM74xx onwards
 *              - Please provide attach profiles in order of decreasing priority in this list. 
 *
 * @param LTEAttachProfileListLen
 *          - Number of element in pLTEAttachProfileList
 *             - valid range: 1-24
 *             - This setting is only supported for MC/EM74xx onwards
 */
typedef struct{
    uint16_t *pLTEAttachProfile;
    uint16_t *pProfileList;
    uint8_t  *pDefaultPDNEnabled;
    uint8_t  *p3gppRelease;
    uint16_t *pLTEAttachProfileList;
    uint16_t  LTEAttachProfileListLen;
} pack_wds_SLQSSet3GPPConfigItem_t;

/**
 * set 3Gpp config items pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSSet3GPPConfigItem(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_SLQSSet3GPPConfigItem_t *reqParam
        );

typedef unpack_result_t unpack_wds_SLQSSet3GPPConfigItem_t;

/**
 * set 3GPP config items unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 */
int unpack_wds_SLQSSet3GPPConfigItem(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSSet3GPPConfigItem_t *pOutput
        );

/**
 *  This structure contains unpack get mobile IP information.
 *  @param  mipMode
 *          - Mobile IP setting
 *              - 0 - Mobile IP off (simple IP only)
 *              - 1 - Mobile IP preferred
 *              - 2 - Mobile IP only
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
*/
typedef struct{
uint32_t         mipMode;
swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetMobileIP_t;

/**
 *  This structure contains pack get mobile IP information.
 *
 *  @param  NULL
 *          - this is a dummy structure
*/
typedef struct{

}pack_wds_GetMobileIP_t;

/**
 * get mobile ip mode pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetMobileIP(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_GetMobileIP_t *pReqParam
    );

/**
 * get mobile ip mode unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
*/
int unpack_wds_GetMobileIP(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_GetMobileIP_t *pOutput
    );

/**
 *  This structure contains pack get mobile IP profile information.
 *
 *  @param  index
 *          - Mobile IP profile ID
 */
typedef struct{
uint8_t   index;
} pack_wds_GetMobileIPProfile_t;

/**
 *  This structure contains unpack get mobile IP profile information.
 *
 *  @param  enabled
 *          - Profile enabled:
 *              - 0 - Disabled
 *              - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  address
 *          - Home IPv4 address:
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  primaryHA
 *          - Primary home agent IPv4 address
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  secondaryHA
 *          - Secondary home agent IPv4 address
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  revTunneling
 *          - Reverse tunneling enabled
 *              - 0 - Disabled
 *              - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  naiSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the NAI array can contain.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  NAI
 *          - Network access identifier string
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  HASPI
 *          - Home agent security parameter index
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  AAASPI
 *          - AAA server security parameter index
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  HAState
 *          - Home agent key state
 *              - 0 - Unset
 *              - 1 - Set, default value
 *              - 2 - Set, modified from default
 *              - 3 - 0xFFFFFFFF - Unknown
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  AAAState
 *          - AAA key state
 *              - 0 - Unset
 *              - 1 - Set, default value
 *              - 2 - Set, modified from default
 *              - 3 - 0xFFFFFFFF - Unknown
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
*/
typedef struct{
uint8_t   enabled;
uint32_t  address;
uint32_t  primaryHA;
uint32_t  secondaryHA;
uint8_t   revTunneling;
uint8_t   naiSize;
int8_t    NAI[255];
uint32_t  HASPI;
uint32_t  AAASPI;
uint32_t  HAState;
uint32_t  AAAState;
swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetMobileIPProfile_t;

/**
 * get mobile ip profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetMobileIPProfile(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_GetMobileIPProfile_t *reqParam
    );

/**
 * get mobile ip profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetMobileIPProfile(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_GetMobileIPProfile_t *pOutput
    );

/**
 * Network information structure
 *
 *  @param  NetworkType
 *          - Values:
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *
 *  @param  RATMask
 *          @ref TableWDSRatMask
 *
 *  @param  SOMask
 *          @ref TableWDSSOMask
 */
typedef struct
{
uint8_t  NetworkType;
uint32_t RATMask;
uint32_t SOMask;
}currNetworkInfo;

/**
 * Data System Status
 *
 *  @param  prefNetwork
 *          - Preferred Network
 *          - Values:
 *              - 0 - 3GPP
 *              - 1 - 3GPP2
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  networkInfoLen
 *          - As input parameter size assigned to next parameter i.e.
 *            network information
 *          - As output the actual number of network information elements
 *            returned by the device
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param   currNetworkInfo
 *          - Network information
 *          - See \ref currNetworkInfo for more details
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
uint8_t             prefNetwork;
uint8_t             networkInfoLen;
currNetworkInfo     currNetworkInfo[255];
swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetCurrDataSystemStat_t;

/**
 *  This structure contains pack get current data system state information.
 *
 *  @param  NULL
 *          - this is a dummy structure
*/
typedef struct{

}pack_wds_SLQSGetCurrDataSystemStat_t;
/**
 * get current data system pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetCurrDataSystemStat(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SLQSGetCurrDataSystemStat_t *pReqParam
    );

/**
 * get current data system unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSGetCurrDataSystemStat(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SLQSGetCurrDataSystemStat_t *pOutput
    );

/**
 * structure to store last mobile IP error.
 *
 *  @param  error
 *          - Status of last MIP call (or attempt)
 *              - Zero - Success
 *              - NonZero - Error code \n
 *              See \ref qaGobiApiTableCallEndReasons.h for Mobile IP
 *              Error codes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
uint32_t             error;
swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetLastMobileIPError_t;

/**
 *  This structure contains pack get last mobile IP error information.
 *
 *  @param  NULL
 *          - this is a dummy structure
*/
typedef struct{

}pack_wds_GetLastMobileIPError_t;

/**
 * get current data system pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: No
 */
int pack_wds_GetLastMobileIPError(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_GetLastMobileIPError_t *pReqParam
    );

/**
 * get current data system unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetLastMobileIPError(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_GetLastMobileIPError_t *pOutput
    );

/**
  * RM Transfer Satistics Structure
  *  @param  bResetStatistics
  *          - Reset Statistics
  *          - Values:
  *          - 0 - Not Reset
  *          - Other - Reset
  *
  *  @param  ulMask
  *         - Enable/Disable RM Transfer Satatistics Indiscation Mask
  *         - Bit 0: Tx Packet Ok
  *         - Bit 1: Rx Packet Ok
  *         - Bit 2: Tx Bytes Ok
  *         - Bit 3: Rx Bytes Ok
  *         - Bit 4: Tx Packets Dropped
  *         - Bit 5: Rx Packets Dropped
  *         - Value:
  *         -0 - Disable
  *         -1 - Enable
  */
typedef struct
{
uint8_t   bResetStatistics;
uint32_t  ulMask;
} rmTrasnferStaticsReq;

/**
 *  This structure contains pack fetch current data system transfer statistics information.
 *
 *  @param  RmTrasnferStaticsReq[IN]
 *          - See \ref rmTrasnferStaticsReq for more information
 *
 */
typedef struct{
rmTrasnferStaticsReq   RmTrasnferStaticsReq;
} pack_wds_RMSetTransferStatistics_t;

/**
 * RM set transfer statistics pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @note PDN Specific: No
 */
int pack_wds_RMSetTransferStatistics(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_RMSetTransferStatistics_t *reqParam
    );

/**
 *  This structure contains unpack fetch current data system transfer statistics information.
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
*/
typedef struct
{
    swi_uint256_t  ParamPresenceMask;
}unpack_wds_RMSetTransferStatistics_t;

/**
 * RM set transfer statistics unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_RMSetTransferStatistics(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_RMSetTransferStatistics_t *pOutput
    );

/**
 *  Sets the mobile IP parameters.
 *
 *  @param  spc
 *          - Six digit service programming code string
 *
 *  @param  index
 *          - Index of the profile to modify
 *
 *  @param  pEnabled
 *          - (Optional) Enable profile?
 *           0       - Disabled
 *           Nonzero - Enabled
 *
 *  @param  pAddress
 *          - (Optional) Home IPv4 address
 *
 *  @param  pPrimaryHA
 *          - (Optional) Primary home agent IPv4 address
 *
 *  @param  pSecondaryHA
 *          - (Optional) Secondary home agent IPv4 address
 *
 *  @param  pRevTunneling
 *          - (Optional) Enable reverse tunneling?
 *           0       - Disabled
 *           Nonzero - Enabled
 *
 *  @param  pNAI
 *          - (Optional) Network access identifier string
 *
 *  @param  pHASPI
 *          - (Optional) Home agent security parameter index
 *
 *  @param  pAAASPI
 *          - (Optional) AAA server security parameter index
 *
 *  @param  pMNHA
 *          - (Optional) MN-HA key string
 *
 *  @param  pMNAAA
 *          - (Optional) MN-AAA key string
 */
typedef struct{
int8_t   spc[10];
uint8_t  index;
uint8_t  *pEnabled;
uint32_t *pAddress;
uint32_t *pPrimaryHA;
uint32_t *pSecondaryHA;
uint8_t  *pRevTunneling;
int8_t   *pNAI;
uint32_t *pHASPI;
uint32_t *pAAASPI;
int8_t   *pMNHA;
int8_t   *pMNAAA;
} pack_wds_SetMobileIPProfile_t;

/**
 * set mobile ip profile pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SetMobileIPProfile(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SetMobileIPProfile_t *reqParam
    );

/**
 *  This structure contains set mobile IP profile information.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
*/
typedef struct
{
swi_uint256_t  ParamPresenceMask;
}unpack_wds_SetMobileIPProfile_t;
/**
 * set mobile ip profile unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetMobileIPProfile(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SetMobileIPProfile_t *pOutput
    );

/**
 * This structure contains the PDP Runtime Settings Request parameters.
 *
 *  @param  contextId
 *          - Context Identifier
 *
 *  @param  contextType
 *          - Identifies technology type
 *             - 0 - 3GPP
 *             - 1 - 3GPP2
 *
 */
typedef struct
{
uint8_t  contextId;
uint8_t  contextType;
} pack_wds_SLQSWdsSwiPDPRuntimeSettings_t;

/**
 * This structure contains the IPv6 address information
 *
 * @param IPV6PrefixLen Length of the received IPv6 address
 *              - 0xff - Invalid IPv6 address information.
 * @param IPAddressV6 IPv6 address(in network byte order)
 */
typedef struct
{
uint8_t   IPV6PrefixLen;
uint16_t  IPAddressV6[IPV6_ADDRESS_ARRAY_SIZE];
}ipv6AddressInfo;

/**
 * This structure contains the unpack parameters retrieved by the API
 * unpack_wds_SLQSWdsSwiPDPRuntimeSettings
 *
 *  @param  contextId (optional)
 *          - Context Identifier
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  bearerId (optional)
 *          - Bearer Identity
 *          - An EPS bearer identity uniquely identifies an EPS bearer for one
 *            UE accessing via E-UTRAN. The EPS Bearer Identity is allocated by
 *            the MME.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  apnName (optional)
 *          - APN name associated with the context id
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ipv4Address (optional)
 *          - IPv4 Address
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ipv4GWAddress (optional)
 *          - IPv4 Gateway Address
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  prDNSIPv4Address (optional)
 *          - Primary DNS IPv4 Address
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  seDNSIPv4Address (optional)
 *          - Secondary DNS IPv4 Address
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  ipv6Address (optional)
 *          - IPv6 Address
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  ipv6GWAddress (optional)
 *          - IPv6 Gateway Address
 *          - See \ref ipv6AddressInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  prDNSIPv6Address (optional)
 *          - Primary IPv6 DNS Address(in network byte order)
 *          - This is an 8-element array of 16-bit numbers, each of which is
 *            in big-endian format
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  seDNSIPv6Address (optional)
 *          - Secondary IPv6 DNS Address(in network byte order)
 *          - This is an 8-element array of 16-bit numbers, each of which is
 *            in big-endian format
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  prPCSCFIPv4Address (optional)
 *          - Primary PCSCF IPv4 Address
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  sePCSCFIPv4Address (optional)
 *          - Secondary PCSCF IPv4 Address
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  prPCSCFIPv6Address (optional)
 *          - Primary PCSCF IPv6 Address
 *          - This is an 8-element array of 16-bit numbers, each of which is
 *            in big-endian format
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  sePCSCFIPv6Address (optional)
 *          - Secondary PCSCF IPv6 Address
 *          - This is an 8-element array of 16-bit numbers, each of which is
 *            in big-endian format
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
uint8_t                   contextId;
uint8_t                   bearerId;
int8_t                    apnName[100];
uint32_t                  ipv4Address;
uint32_t                  ipv4GWAddress;
uint32_t                  prDNSIPv4Address;
uint32_t                  seDNSIPv4Address;
ipv6AddressInfo           ipv6Address;
ipv6AddressInfo           ipv6GWAddress;
uint16_t                  prDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
uint16_t                  seDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
uint32_t                  prPCSCFIPv4Address;
uint32_t                  sePCSCFIPv4Address;
uint16_t                  prPCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
uint16_t                  sePCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
swi_uint256_t             ParamPresenceMask;
} unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t;

/**
 * swi pdp runtime settings pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSWdsSwiPDPRuntimeSettings(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SLQSWdsSwiPDPRuntimeSettings_t *reqParam
    );

/**
 * get current data system unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSWdsSwiPDPRuntimeSettings(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SLQSWdsSwiPDPRuntimeSettings_t *pOutput
    );

/**
 * This structure contains the transfer statistic indication information
 *
 * @param StatsPeriod Field Period between transfer statistic reports.
 *            - 0 - Do not report.
 *            - 1 - Other - Period between reports (in seconds).
 *
 * @param StatsMask requested statistic bit mask.
 *            - 0x00000040 - Tx bytes OK
 *            - 0x00000080 - Rx bytes OK
 */
typedef struct
{
uint8_t  StatsPeriod;
uint32_t StatsMask;
} wds_transferStatInd;

/**
 * This structure contains the DUN Call Info Request parameters.
 *
 *  @param  Mask
 *          - Mandatory parameter
 *          - Set the bits corresponding to the information requested to 1
 *          - All other bits must be set to 0.
 *          - If any values are not available or applicable, the corresponding
 *            TLVs are not returned in the response.
 *              - Bit 0 - Connection Status
 *              - Bit 1 - Last call end reason
 *              - Bit 2 - Tx/Rx bytes OK
 *              - Bit 3 - Dormancy status
 *              - Bit 4 - Data bearer
 *              - Bit 5 - Channel rate
 *              - Bit 6 - Call active duration
 *
 *  @param  pReportConnStatus
 *          - Connect Status Indicator
 *              - 0 - Do not report
 *              - 1 - Report connection status and call end reason
 *
 *  @param  pTransferStatInd
 *          - See \ref wds_transferStatInd for more information
 *
 *  @param  pReportDormStatus
 *          - Dormancy Status Indicator
 *              - 0 - Do not report
 *              - 1 - Report traffic channel state of interface used for
 *                    data connection
 *
 *  @param  pReportDataBearerTech
 *          - Current Data Bearer Technology Indicator
 *              - 0 - Do not report
 *              - 1 - Report radio interface used for data transfer when
 *                    it changes
 *
 *  @param  pReportChannelRate
 *          - Channel Rate Indicator
 *              - 0 - Do not report
 *              - 1 - Report channel rate
 */
typedef struct
{
uint32_t           Mask;
uint8_t            *pReportConnStatus;
wds_transferStatInd    *pTransferStatInd;
uint8_t            *pReportDormStatus;
uint8_t            *pReportDataBearerTech;
uint8_t            *pReportChannelRate;
} pack_wds_SLQSGetDUNCallInfo_t;

/**
 *  This structure contains modem connection status
 *
 *  @param  MDMConnStatus
 *          - Current link status
 *              - 0x01 - DISCONNECTED
 *              - 0x02 - CONNECTED
 *              - 0xff - Invalid data.
 *
 *  @param  MDMCallDuration
 *          - Call duration in milliseconds.
 *          - If the modem connection status is connected,
 *            this represent the duration of the current DUN call.
 *          - If the modem connection status is disconnected, this
 *            represents the duration of the last DUN call since the
 *            device was powered up (zero, if no call has been made or
 *            if the last call was not DUN).
 *              - 0xffffffffffffffff - Invalid data.
 */
typedef struct
{
uint8_t      MDMConnStatus;
uint64_t     MDMCallDuration;
} connectionStatus;

/**
 *  This structure contains Channel Rate
 *
 *  @param  CurrChanTxRate
 *          - Instantaneous channel Tx rate in bits per second
 *          - 0xffffffff - Invalid data.
 *
 *  @param  CurrChanRxRate
 *          - Instantaneous channel Rx rate in bits per second
 *          - 0xffffffff - Invalid data.
 *
 *  @param  MaxChanTxRate
 *          - maximum Tx rate that can be assigned to the device
 *            by the serving system in bits per second
 *          - 0xffffffff - Invalid data.
 *
 *  @param  MaxChanRxRate
 *          - maximum Rx rate that can be assigned to the device
 *            by the serving system in bits per second
 *          - 0xffffffff - Invalid data.
 */
typedef struct
{
uint32_t CurrChanTxRate;
uint32_t CurrChanRxRate;
uint32_t MaxChanTxRate;
uint32_t MaxChanRxRate;
} dunchannelRate;

/**
 * This structure contains the DUN Call Info response parameters
 *
 *  @param  connectionStatus
 *          - See \ref connectionStatus for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  callEndReason
 *          - Last modem call end reason
 *          - See qaGobiApiTableCallEndReasons.h for Call End Reason
 *          - Only valid if the last call made was DUN, else zero is returned
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  txOKBytesCount
 *          - Number of bytes transmitted without error
 *          - Returned only if a data call is up
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  rxOKBytesCount
 *          - Number of bytes received without error
 *          - Returned only if a data call is up
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  dormancyStatus
 *          - Current traffic channel status
 *          - Returned if a data call is up
 *              - 0x01 - Traffic channel dormant
 *              - 0x02 - Traffic channel active
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  dataBearerTech
 *          - Current data bearer technology
 *          - Returned only if a data call is up
 *              - 0x01 - cdma2000 1X
 *              - 0x02 - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - GSM
 *              - 0x04 - UMTS
 *              - 0x05 - cdma200 HRPD ( 1xEV-DO RevA)
 *              - 0x06 - EDGE
 *              - 0x07 - HSDPA and WCDMA
 *              - 0x08 - WCDMA and HSUPA
 *              - 0x09 - HSDPA and HSUPA
 *              - 0x0A - LTE
 *              - 0x0B - cdma2000 EHRPD
 *              - 0x0C - HSDPA+ and WCDMA
 *              - 0x0D - HSDPA+ and HSUPA
 *              - 0x0E - DC_HSDPA+ and WCDMA
 *              - 0x0F - DC_HSDPA+ and HSUPA
 *              - 0x10 - HSDPA+ and 64QAM
 *              - 0x11 - HSDPA+, 64QAM and HSUPA
 *              - 0x12 - TDSCDMA
 *              - 0x13 - TDSCDMA and HSDPA
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  channelRate
 *          - See \ref dunchannelRate for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  lastCallTXOKBytesCnt
 *          - Number of bytes transmitted without error during the last
 *            data call ( 0 if no call was made ).
 *          - Return only if not in a call and the previous call was made
 *            using DUN.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  lastCallRXOKBytesCnt
 *          - Number of bytes transmitted without error during the last
 *            data call ( 0 if no call was made ).
 *          - Return only if not in a call and the previous call was made
 *            using DUN.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  mdmCallDurationActive
 *          - Duration that the call is active in milliseconds
 *          - If the modem connection status is connected, this represents
 *            the active duration of the current DUN call
 *          - If the modem connection status is disconnected, this represents
 *            the duration of the last DUN call since the device was powered
 *            up (0 if no call has been made or if the last call was not DUN)
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  lastCallDataBearerTech
 *          - Last Call Data Bearer Technology
 *          - Returned only if not in a call and when the previous call was
 *            made using DUN
 *              - 0x01 - cdma2000 1X
 *              - 0x02 - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - GSM
 *              - 0x04 - UMTS
 *              - 0x05 - cdma200 HRPD (1xEV-DO Rev A)
 *              - 0x06 - EDGE
 *              - 0x07 - HSDPA and WCDMA
 *              - 0x08 - WCDMA and HSUPA
 *              - 0x09 - HSDPA and HSUPA
 *              - 0x0A - LTE
 *              - 0x0B - cdma2000 EHRPD
 *              - 0x0C - HSDPA+ and WCDMA
 *              - 0x0D - HSDPA+ and HSUPA
 *              - 0x0E - DC_HSDPA+ and WCDMA
 *              - 0x0F - DC_HSDPA+ and HSUPA
 *              - 0x10 - HSDPA+ and 64QAM
 *              - 0x11 - HSDPA+, 64QAM and HSUPA
 *              - 0x12 - TDSCDMA
 *              - 0x13 - TDSCDMA and HSDPA
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
connectionStatus    connectionStatus;
uint16_t            callEndReason;
uint64_t            txOKBytesCount;
uint64_t            rxOKBytesCount;
uint8_t             dormancyStatus;
uint8_t             dataBearerTech;
dunchannelRate         channelRate;
uint64_t            lastCallTXOKBytesCnt;
uint64_t            lastCallRXOKBytesCnt;
uint64_t            mdmCallDurationActive;
uint8_t             lastCallDataBearerTech;
swi_uint256_t       ParamPresenceMask;
} unpack_wds_SLQSGetDUNCallInfo_t;

/**
 * get dun call info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetDUNCallInfo(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SLQSGetDUNCallInfo_t *reqParam
    );

/**
 * get dun call info unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
*/
int unpack_wds_SLQSGetDUNCallInfo(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SLQSGetDUNCallInfo_t *pOutput
    );

/**
 *  Structure to hold the current data bearer technology values
 *
 *  @param  currentNetwork[OUT]
 *          - current selected network\n
 *              - 0 - UNKNOWN
 *              - 1 - 3GPP2
 *              - 2 - 3GPP
 *              - 0xff - Invalid data.
 *
 *  @param  ratMask[OUT]
 *          @ref TableWDSRatMask
 *
 *  @param  soMask[OUT]
 *          @ref TableWDSSOMask
 */
typedef struct
{
uint8_t  currentNetwork;
uint32_t ratMask;
uint32_t soMask;
} qmiWDSDataBearerTechnology;

/**
 *  Bit mask values to indicate the presence of data bearer information for the
 *  current and last data calls
 */
enum liteQmiDataBearerMasks
{
    QMI_LITE_WDS_CURRENT_CALL_DB_MASK = 0x01,
    QMI_LITE_WDS_LAST_CALL_DB_MASK    = 0x02
};
/**
 *  Structure to hold the data bearer technology values
 *
 *  @param  dataBearerMask[OUT]
 *          - This bit mask indicates if data bearer information for the current
 *            and/or last call has been received from the device. If a bit is
 *            set, then the information is available in the corresponding
 *            structure i.e. the one provided by the caller.
 *            Refer to \ref liteQmiDataBearerMasks for bit-mask positions.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  curDataBearerTechnology[OUT]
 *          - current data bearer technology value.
 *          - See \ref qmiWDSDataBearerTechnology
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  lastCallDataBearerTechnology[OUT]
 *          - last call data bearer technology value.
 *          - See \ref qmiWDSDataBearerTechnology
 *              - NULL if the parameter is not required
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
uint8_t dataBearerMask;
qmiWDSDataBearerTechnology curDataBearerTechnology;
qmiWDSDataBearerTechnology lastCallDataBearerTechnology;
swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetDataBearerTechnology_t;

/**
 *  This structure contains pack get data bearer technology information.
 *
 *  @param  NULL
 *          - this is a dummy structure
 */
typedef struct{

}pack_wds_SLQSGetDataBearerTechnology_t;
/**
 * get data bearer technology pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetDataBearerTechnology(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SLQSGetDataBearerTechnology_t *pReqParam
    );

/**
 * get data bearer technology unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSGetDataBearerTechnology(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SLQSGetDataBearerTechnology_t *pOutput
    );

/**
 * This structure contains pack set IP family preference information
 *
 * @param IPFamilyPreference IP Family preference
 *                               - PACK_WDS_IPV4 IP Version 4
 *                               - PACK_WDS_IPV6 IP Version 6
 */

typedef struct{
    uint8_t IPFamilyPreference;
}pack_wds_SLQSSetIPFamilyPreference_t;

/**
 * This structure contains unpack Set IP family preference information
 * @param Tlvresult unpack result
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_wds_SLQSSetIPFamilyPreference_t;

/**
 * Set IP Family Preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_wds_SLQSSetIPFamilyPreference(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SLQSSetIPFamilyPreference_t *pReqParam
);

/**
 * Set IP Family Preference unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSetIPFamilyPreference(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SLQSSetIPFamilyPreference_t *pOutput
);

/**
 * This structure to hold Set default profile number
 *
 *  @param  type Identifies the technology type of the profile
 *             - 0 - 3GPP
 *             - 1 - 3GPP2
 *
 *  @param  family Identifies the family of profile
 *             - 0 - Embedded
 *             - 1 - Tethered
 *
 *  @param  index Profile number to be set as default profile.
 *
 */
typedef struct{
    uint8_t type;
    uint8_t family;
    uint8_t index;
}pack_wds_SetDefaultProfileNum_t;

/**
 * set default profile number pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_wds_SetDefaultProfileNum(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_wds_SetDefaultProfileNum_t *pReqParam
    );

typedef unpack_result_t unpack_wds_SetDefaultProfileNum_t;
/**
 * set default profile number unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetDefaultProfileNum(
    uint8_t   *pResp,
    uint16_t  respLen,
    unpack_wds_SetDefaultProfileNum_t *pOutput
    );

/**
 * This structure contains pack Get Default Profile Number information.
 *
 * @param type profile type
 *           - 0 - 3GPP
 *           - 1 - 3GPP2
 *
 * @param family profile family
 *           - 0 - Embedded
 *           - 1 - Tethered
 */
typedef struct{
    uint8_t type;
    uint8_t family;
}pack_wds_GetDefaultProfileNum_t;

/**
 * This structure contains unpack Get Default Profile Number information.
 *
 * @param index profile index 
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t index;
    swi_uint256_t  ParamPresenceMask;
}unpack_wds_GetDefaultProfileNum_t;

/**
 * get default profile number pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_wds_GetDefaultProfileNum(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetDefaultProfileNum_t *pReqParam
        );

/**
 * get default profile number unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetDefaultProfileNum(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetDefaultProfileNum_t *pOutput
        );

/**
 * This structure contains DHCP v4 ProfielID Information.
 * @param profileType profile type
 *           - 0 - 3GPP
 *
 * @param profileId profile index
 *           - index identifiying the profile 1-24 valid for 3GPP profile type (EM74xx and onwards)
 */
typedef struct
{
    uint8_t profileType;
    uint8_t profileId;
} wdsDhcpv4ProfileId;

/**
 * Structure contain DHCP V4 Hardware Configuration.
 * @param hwType DHCP HW Type, examples:
 *           - 0 - Ethernet
 *           - 20 - Serial
 *
 * @param chaddrLen Length of chaddr field, examples:
 *           - 6 for Ethernet MAC address
 *
 * @param chaddr Client hardware address
 */
typedef struct
{
    uint8_t hwType;
    uint8_t chaddrLen;
    uint8_t chaddr[16];
} wdsDhcpv4HwConfig;

/**
 * This structure contains DHCP V4 Option information.
 *
 * @param optCode Option code
 *           - 0 - 255
 *
 * @param optValLen Option value length
 *           - 0 - 255
 *
 * @param optVal Option Value
 */
typedef struct
{
    uint8_t optCode;
    uint8_t optValLen;
    uint8_t optVal[255];
} wdsDhcpv4Option;

/**
 * Structure contain DHCP V4 Option List
 * @param numOpt number of options
 *           - 0 - 255
 *
 * @param pOptList pointer to list of DHCP Options
 *           - See \ref wdsDhcpv4Option
 */ 
typedef struct
{
    uint8_t numOpt;
    wdsDhcpv4Option *pOptList;
} wdsDhcpv4OptionList;

/**
 *  This structure contain get DHCPv4 client configure.
 *
 *  @param  pProfileId pointer to Profile Id structure
 *          - See \ref wdsDhcpv4ProfileId
 *          
 *
*/ 
typedef struct
{
    wdsDhcpv4ProfileId   *pProfileId;
} pack_wds_SLQSSGetDHCPv4ClientConfig_t;

/**
 * WDS SWI DHCPv4 Config Structure
 *
 *  @param  pHWConfig
 *          - pointer to HW Config structure
 *          - See \ref wdsDhcpv4HwConfig
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pRequestOptionList
 *          - pointer to Option List structure to be sent in
 *            DHCP request
 *          - See \ref wdsDhcpv4OptionList
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    wdsDhcpv4HwConfig    *pHwConfig;
    wdsDhcpv4OptionList  *pRequestOptionList;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSSGetDHCPv4ClientConfig_t;

/**
 * get DHCPv4 Client Config pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_wds_SLQSSGetDHCPv4ClientConfig(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_SLQSSGetDHCPv4ClientConfig_t *pReq);

 /**
 * get DHCPv4 Client Config  unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSGetDHCPv4ClientConfig(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSSGetDHCPv4ClientConfig_t *pOutput
        );

/**
 *  This structure contains pack get packet statistics information.
 *
 *  @param  pStatMask
 *          - Packet Statistics Mask
 *             - 0x00000001 - Tx packets OK
 *             - 0x00000002 - Rx packets OK
 *             - 0x00000004 - Tx packet errors
 *             - 0x00000008 - Rx packet errors
 *             - 0x00000010 - Tx overflows
 *             - 0x00000020 - Rx overflows
 *             - 0x00000040 - Tx bytes OK
 *             - 0x00000080 - Rx bytes OK
 */
typedef struct
{
    uint32_t *pStatMask;
} pack_wds_GetPacketStatistics_t;

/**
 * gets current packet transfer counter values pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReq request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int pack_wds_GetPacketStatistics(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        pack_wds_GetPacketStatistics_t *pReq);

/**
 *  This structure contains unpack get packet statistics information.
 *
 *  @param  pTXPacketSuccesses
 *          - No of transmitted Packets without error.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pRXPacketSuccesses
 *          - No of received Packets without error.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pTXPacketErrors
 *          - Number of outgoing packets with framing errors.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pRXPacketErrors
 *          - Number of incoming packets with framing errors.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pTXPacketOverflows
 *          - Number of packets dropped because Tx buffer overflowed (out of memory).
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pRXPacketOverflows
 *          - Number of packets dropped because Rx buffer overflowed (out of memory).
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pTXOkBytesCount
 *          - Number of bytes transmitted without error.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pRXOkBytesCount
 *          - Number of bytes received without error.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pTXOKBytesLastCall
 *          - Number of bytes transmitted without error during the
 *            last data call (0 if no call was made earlier).
 *            Returned only if not in a call, and when the previous call was
 *            made using RmNet (for any devices that support
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pRXOKBytesLastCall
 *          - Number of bytes received without error
 *            during the last data call (0 if no call was
 *            made earlier). Returned only if not in a
 *            call, and when the previous call was
 *            made using RmNet (for any devices that
 *            support
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pTXDroppedCount
 *          - Number of outgoing packets dropped.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pRXDroppedCount
 *          - Number of incoming packets dropped.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
 typedef struct
 {
    uint32_t    *pTXPacketSuccesses;
    uint32_t    *pRXPacketSuccesses;
    uint32_t    *pTXPacketErrors;
    uint32_t    *pRXPacketErrors;
    uint32_t    *pTXPacketOverflows;
    uint32_t    *pRXPacketOverflows;
    uint64_t    *pTXOkBytesCount;
    uint64_t    *pRXOkBytesCount;
    uint64_t    *pTXOKBytesLastCall;
    uint64_t    *pRXOKBytesLastCall;
    uint32_t    *pTXDroppedCount;
    uint32_t    *pRXDroppedCount;
    swi_uint256_t  ParamPresenceMask;
 } unpack_wds_GetPacketStatistics_t;

 /**
 * gets current packet transfer counter values unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetPacketStatistics(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetPacketStatistics_t *pOutput
        );

/**
 * get Rx/Tx byte counts since the start of the last packet data session pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetByteTotals(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );
/**
 *  This structure contains unpack get byte totals information.
 *
 *  @param  pTXTotalBytes
 *          - Bytes transmitted without error
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pRXTotalBytes
 *          - Bytes received without error
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint64_t *pTXTotalBytes;
    uint64_t *pRXTotalBytes;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetByteTotals_t;

/**
 * get Rx/Tx byte counts since the start of the last packet data session unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetByteTotals(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetByteTotals_t *pOutput
        );

/**
 * get current Tx/Rx channel bitrate of the current packet data pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSGetCurrentChannelRate(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );
/**
 *  This structure contains unpack get current channel rate information.
 *  
 *  @param  current_channel_tx_rate
 *          - Current Channel Tx Rate.
 *          - Instantaneous channel Tx rate in bits per second.
 *          - In 9x15, this is the total current channel rate for all PDNs combined.
 *          - In 9x30 and later, this is the channel rate for a specific PDN.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  current_channel_rx_rate
 *          - Current Channel Rx Rate.
 *          - Instantaneous channel Rx rate in bits per second.
 *          - In 9x15, this is the total current channel rate for all PDNs combined.
 *          - In 9x30 and later, this is the channel rate for a specific PDN
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  max_channel_tx_rate
 *          - Max Channel Tx Rate.
 *          - Maximum total Tx rate that modem is able to support in current serving
 *            system in bits per second.
 *          - In 9x15, this is a default hard coded value for the current serving system.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  max_channel_rx_rate
 *          - Max Channel Rx Rate.
 *          - Maximum total Rx rate that modem is able to support in current serving
 *            system in bits per second.
 *          - In 9x15, this is a default hard coded value for the current serving system.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t current_channel_tx_rate;
    uint32_t current_channel_rx_rate;
    uint32_t max_channel_tx_rate;
    uint32_t max_channel_rx_rate;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSGetCurrentChannelRate_t;

/**
 * get current Tx/Rx channel bitrate of the current packet data unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSGetCurrentChannelRate(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSGetCurrentChannelRate_t *pOutput
        );

/**
 * get the value of loopback mode and multiplier pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSSGetLoopback(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );
/**
 *  This structure contains unpack Get loopback information.
 *
 *  @param  ByteLoopbackMode
 *          - Loopback Mode.
 *              - 0 - Disable
 *              - 1 - Enable
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ByteLoopbackMultiplier
 *          - Loopback multiplier. Number of downlink bytes to send for each uplink byte.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t ByteLoopbackMode;
    uint8_t ByteLoopbackMultiplier;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSSGetLoopback_t;

/**
 * get the value of loopback mode and multiplier unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSGetLoopback(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSSGetLoopback_t *pOutput
        );

/**
 *  This structure contains pack set loopback information.
 *
 *  @param  loopbackMode
 *          - Loopback Mode.
 *            - 0 - Disable
 *            - 1 - Enable
 *
 *  @param  loopbackMultiplier
 *          - Loopback multiplier. Number of downlink bytes to send for each uplink byte.
 */
typedef struct {
    uint8_t loopbackMode;
    uint8_t loopbackMultiplier;
} pack_wds_SLQSSSetLoopback_t;

/**
 * Enable/disable Data Loopback Mode and set the value of loopback multiplier pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SLQSSSetLoopback(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSSSetLoopback_t *reqArg
        );

typedef unpack_result_t unpack_wds_SLQSSSetLoopback_t;

/**
 * Enable/disable Data Loopback Mode and set the value of loopback multiplier unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSSetLoopback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSSSetLoopback_t *pOutput
        );
/**
 *  This structure contains Data unsigned long TLV data.
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  ulData unsigend long data in 32bit
 */
typedef struct
{
    uint8_t    TlvPresent;
    uint32_t   ulData;
}wds_DataULongTlv;

/**
 *  This structure contains Data unsigned long long TLV data.
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  ullData unsigend long long data in 64bit
 */
typedef struct
{
    uint8_t    TlvPresent;
    uint64_t   ullData;
}wds_DataULongLongTlv;

/**
 * WDS packet RM Transfer Statistics data structure for individual session
 *
 *  @param  TxOkConutTlv
 *          - Tx Ok Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  RxOkConutTlv
 *          - Rx Ok Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  TxOkByteCountTlv
 *          - Tx Ok Byte Count Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  RxOkByteCountTlv
 *          - Rx Ok Byte Count Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  TxDropConutTlv
 *          - Tx Drop Count Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  RxDropConutTlv
 *          - Rx Drop Count Packet Tlv Value.
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 */
typedef struct {
    wds_DataULongTlv        TxOkConutTlv;
    wds_DataULongTlv        RxOkConutTlv;
    wds_DataULongLongTlv    TxOkByteCountTlv;
    wds_DataULongLongTlv    RxOkByteCountTlv;
    wds_DataULongTlv        TxDropConutTlv;
    wds_DataULongTlv        RxDropConutTlv;
    swi_uint256_t  ParamPresenceMask;
}unpack_RMTransferStatistics_ind_t, unpack_wds_RMTransferStatistics_ind_t;

/**
 * RM transfer statistics indication unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_RMTransferStatistics_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_RMTransferStatistics_ind_t *pOutput
        );

/**
 * WDS SWI DHCPv4 Client Lease Change Structure
 *
 *  @param  pEnableNotification
 *          - Enable Notification or not
 */
typedef struct {
    uint8_t *pEnableNotification;
} pack_wds_DHCPv4ClientLeaseChange_t;

/**
 * DHCPv4 lease state changes pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_DHCPv4ClientLeaseChange(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_DHCPv4ClientLeaseChange_t *reqArg
        );
typedef unpack_result_t unpack_wds_DHCPv4ClientLeaseChange_t;

/**
 * DHCPv4 lease state changes unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_DHCPv4ClientLeaseChange(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_DHCPv4ClientLeaseChange_t *pOutput
        );

/**
 * DHCP profile id TLV information
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  profileType identifying the type of the profile
 *          - 0 - 3GPP
 *
 *  @param  profileId index identifying the profile
 *          - 1-24 valid for 3GPP profile type (9x30 and onwards)
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t profileType;
    uint8_t profileId;
} wds_DHCPProfileIdTlv;

/**
 * DHCP lease state information
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  leaseState lease state
 *          - 0 - active, newly acquired
 *          - 1 - active, renewed
 *          - 2 - active, renewing
 *          - 3 - active, rebinding
 *          - 4 - inactive, expired
 *          - 5 - inactive, renew refused
 *          - 6 - inactive, rebind refused
 *          - 7 - inactive, other
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t leaseState;
} wds_DHCPLeaseStateTlv;

/**
 * IPv4 address TLV information
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  IPv4Addr IPv4 address
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t IPv4Addr;
} wds_IPv4AdTlv;

/**
 * DHCP option code information
 *
 *  @param  optCode option code
 *          - values:
 *             - 0 - 255
 *
 *  @param  optValLen length of option code
 *          - values:
 *             - 0 - 255
 *
 *  @param  pOptVal option value
 */
typedef struct
{
    uint8_t optCode;
    uint8_t optValLen;
    uint8_t *pOptVal;
} wds_DHCPOpt;

/**
 * DHCP lease option information
 *
 *  @param  TlvPresent a flag indicates whether TLV presented or not
 *
 *  @param  numOpt number of sets of \ref wds_DHCPOpt
 *
 *  @param  optList option list
 *
 *  @param  optListData option list data
 */
typedef struct
{
    uint8_t     TlvPresent;
    uint8_t     numOpt;
    wds_DHCPOpt optList[WDS_DHCP_MAX_NUM_OPTIONS];
    uint8_t     optListData[WDS_DHCP_OPTION_DATA_BUF_SIZE]; /* internal buffer to store option data */
} wds_DHCPLeaseOptTlv;


/**
 * This structure contains DHCP IPv4 client lease information
 *
 *  @param  ProfileIdTlv profile identifier information, see \ref wds_DHCPProfileIdTlv for more details
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  DHCPv4LeaseStateTlv lease state, see \ref wds_DHCPLeaseStateTlv for more details
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  IPv4AddrTlv IPv4 address, see \ref wds_IPv4AdTlv for more details
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  DHCPv4LeaseOptTlv DHCP lease option, see \ref wds_DHCPLeaseOptTlv for more details
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    wds_DHCPProfileIdTlv   ProfileIdTlv;
    wds_DHCPLeaseStateTlv  DHCPv4LeaseStateTlv;
    wds_IPv4AdTlv          IPv4AddrTlv;
    wds_DHCPLeaseOptTlv    DHCPv4LeaseOptTlv;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_DHCPv4ClientLease_ind_t;

/**
 * DHCP lease state has changed indication unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_DHCPv4ClientLease_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_DHCPv4ClientLease_ind_t *pOutput
        );

/**
 *  This structure contains set mobile IP pack information.
 *
 *  @param  mode
 *          - Mobile IP setting
 *              - 0 - Mobile IP off (simple IP only)
 *              - 1 - Mobile IP preferred
 *              - 2 - Mobile IP only
 */
typedef struct {
    uint32_t mode;
} pack_wds_SetMobileIP_t;

/**
 * Sets the current mobile IP setting pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SetMobileIP(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SetMobileIP_t *reqArg
        );

typedef unpack_result_t unpack_wds_SetMobileIP_t;

/**
 * Sets the current mobile IP setting unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetMobileIP(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SetMobileIP_t *pOutput
        );

/**
 *  Mobile IP parameters information.
 *
 *  @param  pSPC
 *          - NULL-terminated string representing six digit service
 *            programming code.
 *
 *  @param  pMode
 *          - Mode to be set (optional)
 *              - 0 - Mobile IP off (simple IP only)
 *              - 1 - Mobile IP preferred
 *              - 2 - Mobile IP only
 *
 *  @param  pRetryLimit
 *          - Registration retry attempt limit (optional)
 *
 *  @param  pRetryInterval
 *          - Registration retry attempt interval used to determine the time
 *            between registration attempts (optional)
 *
 *  @param  pReRegPeriod
 *          - Period (in minutes) to attempt re-registration before current
 *            registration expires (optional)
 *
 *  @param  pReRegTraffic
 *          - Re-registration only if traffic since last attempt (optional)
 *              - Zero    - Disabled
 *              - NonZero - Enabled
 *
 *  @param  pHAAuthenticator
 *          - MH-HA authenticator calculator (optional)
 *              - Zero    - Disabled
 *              - NonZero - Enabled
 *
 *  @param  pHA2002bis
 *          - MH-HA RFC 2002bis authentication instead of RFC2002 (optional)
 *              - Zero    - Disabled
 *              - NonZero - Enabled
 *
 */
typedef struct {
    char     *pSPC;
    uint32_t *pMode;
    uint8_t  *pRetryLimit;
    uint8_t  *pRetryInterval;
    uint8_t  *pReRegPeriod;
    uint8_t  *pReRegTraffic;
    uint8_t  *pHAAuthenticator;
    uint8_t  *pHA2002bis;
}pack_wds_SetMobileIPParameters_t;

/**
 * Sets the specified mobile IP parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SetMobileIPParameters(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SetMobileIPParameters_t *reqArg
        );

typedef unpack_result_t unpack_wds_SetMobileIPParameters_t;
/**
 * Sets the specified mobile IP parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_wds_SetMobileIPParameters(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SetMobileIPParameters_t *pOutput
        );

/**
 *  auto connect data session parameters.
 *
 *  @param  acsetting
 *          - Current autoconnect setting:
 *            - 0x00 - Autoconnect disabled
 *            - 0x01 - Autoconnect enabled
 *            - 0x02 - Autoconnect paused (resume on powercycle)
 *
 *  @param  acroamsetting
 *          - Current autoconnect roaming status
 *            - 0x00 - Autoconnect always allowed
 *            - 0x01 - Autoconnect while in home service area only
 */
typedef struct {
  uint8_t acsetting;
  uint8_t acroamsetting;
}pack_wds_SetAutoconnect_t;

/**
 * Auto connect data session parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_SetAutoconnect(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SetAutoconnect_t *reqArg
        );

typedef unpack_result_t unpack_wds_SetAutoconnect_t;
/**
 * Auto connect data session parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetAutoconnect(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SetAutoconnect_t *pOutput
        );

/**
 *  auto connect data session setting parameter.
 *
 *  @param  pSetting
 *          - NDIS auto connect setting
 *              - 0 - Disabled
 *              - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
  uint32_t *psetting;
  swi_uint256_t  ParamPresenceMask;
}unpack_wds_GetAutoconnect_t;

/**
 * Gets auto connect data session setting pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */

int pack_wds_GetAutoconnect(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Gets auto connect data session setting unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetAutoconnect(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_GetAutoconnect_t *pOutput);

/**
 * This structure contains the information about the Transfer Statistics
 * Indicator parameters.
 *
 *  @param  statsPeriod
 *          - Period between transfer statistics reports.
 *              - 0 - Do not report
 *              - Other - Period between reports (seconds)
 *
 *  @param  statsMask
 *          - Requested statistic bit mask.
 *              - 0x00000001 - Tx packets OK
 *              - 0x00000002 - Rx packets OK
 *              - 0x00000004 - Tx packet errors
 *              - 0x00000008 - Rx packet errors
 *              - 0x00000010 - Tx overflows
 *              - 0x00000020 - Rx overflows
 *              - 0x00000040 - Tx bytes OK
 *              - 0x00000080 - Rx bytes OK
 *          - Each bit set causes the corresponding optional information to be
 *            sent in SLQSWdsEventReportCallBack.
 *          - All unlisted bits are reserved for future use and must be set to
 *            zero.
 *
 */
typedef struct
{
    uint8_t  statsPeriod;
    uint32_t statsMask;
} wds_TrStatInd;

/**
 * This structure contains the information about the Set Event Report Request
 * parameters.
 *
 *  @param  pCurrChannelRateInd (optional)
 *          - Current Channel Rate Indicator.
 *              - 0 - Do not report
 *              - 1 - Report channel rate when it changes
 *
 *  @param  pTransferStatInd (optional)
 *          - See \ref wds_TrStatInd for more information.
 *
 *  @param  pDataBearerTechInd (optional)
 *          - Data Bearer Technology Indicator.
 *              - 0 - Do not report
 *              - 1 - Report radio interface used for data transfer when it
 *                    changes
 *
 *  @param  pDormancyStatusInd (optional)
 *          - Dormancy Status indicator.
 *              - 0 - Do not report
 *              - 1 - Report traffic channel state of interface used for data
 *                    connection
 *
 *  @param  pMIPStatusInd (optional)
 *          - MIP Status Indicator.
 *              - 0 - Do not report
 *              - 1 - Report MIP status
 *
 *  @param  pCurrDataBearerTechInd (optional)
 *          - Current Data Bearer Technology Indicator.
 *              - 0 - Do not report
 *              - 1 - Report current data bearer technology when it changes
 *
 *  @param  pDataCallStatusChangeInd (optional)
 *          - Data Call Status Change Indicator.
 *              - 0 - Do not report
 *              - 1 - Report data call status change when it changes
 *
 *  @param  pCurrPrefDataSysInd (optional)
 *          - Current Preferred Data System Indicator.
 *              - 0 - Do not report
 *              - 1 - Report preferred data system when it changes
 *
 *  @param  pEVDOPageMonPerChangeInd (optional)
 *          - EV-DO Page Monitor Period Change Indicator.
 *              - 0 - Do not report
 *              - 1 - Report EV-DO page monitor period change event
 *
 *  @param  pDataSystemStatusChangeInd (optional)
 *          - Data System Status Change Indicator.
 *              - 0 - Do not report
 *              - 1 - Report data system status change event
 *
 *  @note At least one parameter should be present.
 *
 */

typedef struct
{
    uint8_t         *pCurrChannelRateInd;
    wds_TrStatInd   *pTransferStatInd;
    uint8_t         *pDataBearerTechInd;
    uint8_t         *pDormancyStatusInd;
    uint8_t         *pMIPStatusInd;
    uint8_t         *pCurrDataBearerTechInd;
    uint8_t         *pDataCallStatusChangeInd;
    uint8_t         *pCurrPrefDataSysInd;
    uint8_t         *pEVDOPageMonPerChangeInd;
    uint8_t         *pDataSystemStatusChangeInd;
}pack_wds_SLQSWdsSetEventReport_t;

/**
 * Sets the event report parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */

int pack_wds_SLQSWdsSetEventReport(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSWdsSetEventReport_t *reqArg
        );

typedef unpack_result_t unpack_wds_SLQSWdsSetEventReport_t;

/**
 * Sets the event report parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSWdsSetEventReport(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSWdsSetEventReport_t *pOutput
        );

/**
 * Gets the device into dormant state pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */

int pack_wds_SLQSWdsGoDormant(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

typedef unpack_result_t unpack_wds_SLQSWdsGoDormant_t;

/**
 * Gets the device into dormant state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSWdsGoDormant(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSWdsGoDormant_t *pOutput
        );

/**
 * Gets the device into Active state pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */

int pack_wds_SLQSWdsGoActive(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );
typedef unpack_result_t unpack_wds_SLQSWdsGoActive_t;

/**
 * Gets the device into Active state unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSWdsGoActive(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSWdsGoActive_t *pOutput
        );

/**
 * Reset packet data transfer statistics pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 *
*/
int pack_wds_SLQSResetPacketStatics(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen);

typedef unpack_result_t unpack_wds_SLQSResetPacketStatics_t;

/**
 * Reset packet data transfer statistics unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSResetPacketStatics(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSResetPacketStatics_t *pOutput
        );

/**
 * WDS SWI DHCPv4 Profile Identifier Structure
 *
 *  @param  profileType
 *          - 0 for 3GPP
 *  @param  profileId
 *          - 1 to 24 for 3GPP profile
 *
 */
typedef struct
{
    uint8_t profileType;
    uint8_t profileId;
} wds_DHCPv4ProfileId;

/**
 * WDS SWI DHCPv4 HW Config Structure.
 *
 *  @param  hwType
 *           - HW Type
 *           1 - Ethernet
 *           20 - Serial
 *
 *  @param  chaddrlen
 *          - chaddrlen
 *
 *  @param  chaddr
 *          - chaddr. Max size 16 bytes
 *
 */
typedef struct
{
    uint8_t hwType;
    uint8_t chaddrLen;
    uint8_t chaddr[16];
}wds_DHCPv4HWConfig;

/**
 * WDS SWI DHCPv4 Option Structure
 *
 *  @param  optCode
 *          - Option code
 *              - 0 - 255
 *
 *  @param  optValLen
 *          - Option value length
 *              - 0 - 255
 *
 *  @param  optVal
 *          - Option value
 *
 */
typedef struct
{
    uint8_t optCode;
    uint8_t optValLen;
    uint8_t optVal[255];
}wds_DHCPv4Option;

/**
 * WDS SWI DHCPv4 Option List Structure
 *
 *  @param  numOpt
 *          - number of options
 *              - 0 - 255
 *
 *  @param  pOptList
 *          - pointer to list of DHCP Options
 *
 */
typedef struct
{
    uint8_t          numOpt;
    wds_DHCPv4Option *pOptList;
}wds_DHCPv4OptionList;

/**
 * WDS SWI DHCPv4 Config Structure
 *
 *  @param  pProfileId
 *          - pointer to Profile Id structure
 *
 *  @param  pHWConfig
 *          - pointer to HW Config structure
 *
 *  @param  pRequestOptionList
 *          - pointer to Option List structure to be sent in
 *            DHCP request
 *
 */
typedef struct
{
    wds_DHCPv4ProfileId   *pProfileId;
    wds_DHCPv4HWConfig    *pHwConfig;
    wds_DHCPv4OptionList  *pRequestOptionList;
}pack_wds_SLQSSSetDHCPv4ClientConfig_t;

/**
 * Gets the DHCP Client V4 Configuration pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 *  @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  @sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_wds_SLQSSSetDHCPv4ClientConfig(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSSSetDHCPv4ClientConfig_t *reqArg
        );

typedef unpack_result_t unpack_wds_SLQSSSetDHCPv4ClientConfig_t;

/**
 * Gets the DHCP Client V4 Configuration unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSSetDHCPv4ClientConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_wds_SLQSSSetDHCPv4ClientConfig_t *pOutput
        );


/**
 * get current data bearer technology pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note PDN Specific: Yes
 */
int pack_wds_GetDataBearerTechnology(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 *  This structure contains unpack get data bearer technology information.
 *
 *  @param  pDataBearer[OUT]
 *          - Data bearer technology\n
 *              - 0x01 - CDMA2000 1x
 *              - 0x02 - CDMA 1xEV-DO Rev 0
 *              - 0x03 - GSM
 *              - 0x04 - UMTS
 *              - 0x05 - CDMA2000 HRPD (1xEV-DO Rev A)
 *              - 0x06 - EDGE
 *              - 0x07 - HSDPA AND WCDMA
 *              - 0x08 - WCDMA AND HSUPA
 *              - 0x09 - HSDPA AND HSUPA
 *              - 0x0A - LTE
 *              - 0x0B - CDMA2000 EHRPD
 *              - 0x0C - HSDPA+ and WCDMA
 *              - 0x0D - HSDPA+ and HSUPA
 *              - 0x0E - DC_HSDPA+ and WCDMA
 *              - 0x0F - DC_HSDPA+ and HSUPA
 *              - 0x10 - HSDPA+ and 64QAM
 *              - 0x11 - HSDPA+, 64QAM and HSUPA
 *              - 0x12 - TDSCDMA
 *              - 0x13 - TDSCDMA and HSDPA
 *              - 0xFF - Unknown
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t    *pDataBearer;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_GetDataBearerTechnology_t;

/**
 * get current data bearer technology unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_GetDataBearerTechnology(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_GetDataBearerTechnology_t *pOutput
        );


/**
 * Set MUX ID pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pMuxID MUX ID.
 *             - 0x80 to 0x88
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_wds_SetMuxID(        
      pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen,
        uint8_t     *pMuxID);

typedef unpack_result_t unpack_wds_SetMuxID_t;

/**
 * Set MUX ID unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SetMuxID(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SetMuxID_t *pOutput
        );

/**
 *  This structure stores information about modem connection status TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param MDMConnStatus
 *          -Modem connecton status
 *              - 0x01 - disconnected
 *              - 0x02 - connected
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t MDMConnStatus;
} wds_ConnStatusTlv;

/**
 *  This structure stores information about modem last call end reason TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param CallEndReason
 *          -Modem call end reason
 *             - See qaGobiApiTableCallEndReasons.h for Call End Reason
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t CallEndReason;
} wds_LastMdmCallEndRsnTlv;

/**
 *  This structure stores information about Tx Bytes OK TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param TxByteOKCnt
 *          - Number of bytes transmitted without error
 */
typedef struct
{
    uint8_t TlvPresent;
    uint64_t TxByteOKCnt;
} wds_TXBytesOKTlv;

/**
 *  This structure stores information about Rx Bytes OK TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param RxByteOKCnt
 *          -Number of bytes received without error
 */
typedef struct
{
    uint8_t TlvPresent;
    uint64_t RxByteOKCnt;
} wds_RXBytesOKTlv;


/**
 *  This structure stores information about dormancy status TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param DormancyStat
 *          -Dormancy status
 *              - 0x01 - Traffic channel dormant
 *              - 0x02 - Traffic channel active
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t DormancyStat;
} wds_DormStatTlv;

/**
 *  This structure hold parameters about data bearer technology TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param DataBearerTech
 *          - Data bear technology
 *              - 0x01 - cdma2000 1X
 *              - 0x02 - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - GSM
 *              - 0x04 - UMTS
 *              - 0x05 - cdma200 HRPD ( 1xEV-DO RevA)
 *              - 0x06 - EDGE
 *              - 0x07 - HSDPA and WCDMA
 *              - 0x08 - WCDMA and HSUPA
 *              - 0x09 - HSDPA and HSUPA
 *              - 0x0A - LTE
 *              - 0x0B - cdma2000 EHRPD
 *              - 0x0C - HSDPA+ and WCDMA
 *              - 0x0D - HSDPA+ and HSUPA
 *              - 0x0E - DC_HSDPA+ and WCDMA
 *              - 0x0F - DC_HSDPA+ and HSUPA
 *              - 0x10 - HSDPA+ and 64QAM
 *              - 0x11 - HSDPA+, 64QAM and HSUPA
 *              - 0x12 - TDSCDMA
 *              - 0x13 - TDSCDMA and HSDPA
 *              - 0xFF - Unknown
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t DataBearerTech;
} wds_DataBearTechTlv;

/**
 *  This structure contains Channel Rate
 *
 *  @param  CurrChanTxRate
 *          - Max channel Tx rate in bits per second
 *
 *  @param  CurrChanRxRate
 *          - Max channel Rx rate in bits per second
 *
 */
typedef struct
{
    uint32_t CurrChanTxRate;
    uint32_t CurrChanRxRate;
} wds_channelRate;

/**
 *  This structure stores information about channel rate TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param ChannelRate
 *          - see @ref wds_channelRate for more info 
 */
typedef struct
{
    uint8_t TlvPresent;
    wds_channelRate ChannelRate;
} wds_ChannelRateTlv;

/**
 *  Structure used to store Reg Mgr Config Indication Parameters. 
 *  @param  CSTlv
 *          - Connection status tlv
 *          - See @ref wds_ConnStatusTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *  @param  LMCERTlv
 *          - last modem call end reason tlv
 *          - see @ref wds_LastMdmCallEndRsnTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *  @param TXBOTlv
 *          - Tx bytes OK tlv
 *          - see @ref wds_TXBytesOKTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *  @param RXBOTlv
 *          - Rx bytes OK tlv
 *          - see @ref wds_RXBytesOKTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *  @param DSTlv
 *          - Dormancy status tlv
 *          - see @ref wds_DormStatTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *  @param DBTTlv
 *          - Data bear technology  tlv
 *          - see @ref wds_DataBearTechTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *  @param CRTlv
 *          - Channel rate Tlv
 *          - see @ref wds_ChannelRateTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 * @note:    None
 */
typedef struct{
    wds_ConnStatusTlv        CSTlv;
    wds_LastMdmCallEndRsnTlv LMCERTlv;
    wds_TXBytesOKTlv         TXBOTlv;
    wds_RXBytesOKTlv         RXBOTlv;
    wds_DormStatTlv          DSTlv;
    wds_DataBearTechTlv      DBTTlv;
    wds_ChannelRateTlv       CRTlv;
    swi_uint256_t  ParamPresenceMask;
} unpack_wds_SLQSDUNCallInfoCallBack_ind_t;

/**
 * unpack DUN call info indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSDUNCallInfoCallBack_ind(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSDUNCallInfoCallBack_ind_t *pOutput
        );

/**
 *  This structure contains Profile Change Enable/Disable Notification Parameter
 *
 *  @param  pProfileChangeInd
 *          - Enable/Disable Notification
 *
 */
typedef struct
{
    uint8_t         *pProfileChangeInd;
}pack_wds_SLQSSwiProfileChangeCallback_t;

/**
 * Gets profile change information pack
 * @param[in,out] pCtx qmi request context
 * @param[out]    pReqBuf qmi request buffer
 * @param[out]    pLen qmi request length
 * @param[in]     reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_wds_SLQSSwiProfileChangeCallback(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_wds_SLQSSwiProfileChangeCallback_t *reqArg
        );

typedef unpack_result_t unpack_wds_SLQSSwiProfileChangeCallback_t;

/**
 * unpack profile change info
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 * @param[out]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSwiProfileChangeCallback(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSSwiProfileChangeCallback_t *pOutput
        );

/**
 *  This structure contains Profile Change Info
 *
 *  @param  profileType
 *          - Identifies the type of the profile
 *
 *  @param  profileInx
 *          - Index identifying the profile
 *
 */
typedef struct
{
    uint8_t profileType;
    uint8_t profileInx;
} wds_profileChange;

/**
 *  This structure contains Source of change Info
 *
 *  @param  source
 *          - source of change
 *
 */
typedef struct
{
    uint8_t source;
} wds_sourceOfChange;

/**
 * Structure for SwiProfileChange unpack.
 *
 *  @param  ProfileTlv
 *          - See @ref wds_profileChange for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  srcTlv
 *          - See @ref wds_sourceOfChange for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    wds_profileChange      ProfileTlv;
    wds_sourceOfChange     srcTlv;
    swi_uint256_t          ParamPresenceMask;
}unpack_wds_SLQSSwiProfileChangeCallback_Ind_t;

/**
 * unpack profile change indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 * @param[out]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_wds_SLQSSwiProfileChangeCallback_Ind(
        uint8_t   *pResp,
        uint16_t  respLen,
        unpack_wds_SLQSSwiProfileChangeCallback_Ind_t *pOutput
        );


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif

