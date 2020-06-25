/**
 * \ingroup liteqmi
 *
 * \file qos.h
 */

#ifndef __LITEQMI_QOS_H__
#define __LITEQMI_QOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#include "msgid.h"

#define LITEQMI_MAX_QOS_FLOW_PER_APN_STATS    10
#define LITEQMI_MAX_QOS_FILTERS               25
#define LITEQMI_MAX_QOS_FLOWS                  8
/**
 *  Function to pack command to retrieve QoS status of the network.
 *  This maps to SLQSQosGetNetworkStatus
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *		
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note
 *          - Timeout: 2 seconds
 *          - Technology Supported: CDMA
 *          - PDN Specific: No
 */
int pack_qos_SLQSQosGetNetworkStatus (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        ); 

/**
 *  Structure that contains the response to get NW QoS status command
 *  \param  NWQoSStatus
 *              Network QoS support status
 *              - 0 - No QoS support in network
 *              - 1 - Network supports QoS 
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint8_t NWQoSStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSQosGetNetworkStatus_t;

/**
 *  Function to unpack the response to get NW QoS status command
 *  This maps to SLQSQosGetNetworkStatus
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSQosGetNetworkStatus_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_qos_SLQSQosGetNetworkStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSQosGetNetworkStatus_t *pOutput
        );

/**
 *  Structure that contains the APN ID to obtain extra APN parameters
 *  \param  apnId[IN]
 *          - APN id
 */
typedef struct {
    uint32_t apnId;
} pack_qos_SLQSQosSwiReadApnExtraParams_t;

/**
 *  Function to pack QMI command to query extra APN parameters 
 *  This maps to SLQSQosSwiReadApnExtraParams
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *		
 *  \param[in]  reqParam
 *              - See \ref pack_qos_SLQSQosSwiReadApnExtraParams_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   - Timeout: 2 seconds
 *          - PDN Specific: Yes
 */
 int pack_qos_SLQSQosSwiReadApnExtraParams (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_qos_SLQSQosSwiReadApnExtraParams_t  reqParam 
        );

/**
 *  Structure that contains extra APN parameters
 *  \param  apnId
 *          - APN id
 *          - ID identifing the APN that the client would like to query the AMBR params
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_ul
 *          - APN AMBR uplink
 *          - APN AMBR uplink values from 1 kbps to 8640 kbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_dl
 *          - APN AMBR downlink
 *          - APN AMBR downlink values from 1 kbps to 8640 kbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_ul_ext
 *          - Extended APN AMBR uplink
 *          - APN AMBR uplink values from 8700 kbps  to 256 Mbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_dl_ext
 *          - Extended APN AMBR downlink
 *          - APN AMBR downlink values from 8700 kbps  to 256 Mbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_ul_ext2
 *          - Second extended APN AMBR uplink
 *          - APN AMBR uplink values from 256 Mbps to 65280 Mbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ambr_dl_ext2
 *          - Second extended APN AMBR downlink
 *          - APN AMBR downlink values from 256 Mbps to 65280 Mbps
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t    apnId;
    uint8_t     ambr_ul;
    uint8_t     ambr_dl;
    uint8_t     ambr_ul_ext;
    uint8_t     ambr_dl_ext;
    uint8_t     ambr_ul_ext2;
    uint8_t     ambr_dl_ext2;
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSQosSwiReadApnExtraParams_t;

/**
 *  Function to unpack the response to get NW QoS status command
 *  This maps to SLQSQosSwiReadApnExtraParams
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSQosSwiReadApnExtraParams_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_qos_SLQSQosSwiReadApnExtraParams(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSQosSwiReadApnExtraParams_t *pOutput
        );

/**
 *  Structure that contains the APN ID to obtain data statistics
 *  \param  apnId[IN]
 *          - APN id
 */
typedef struct {
    uint32_t apnId;
} pack_qos_SLQSQosSwiReadDataStats_t;

/**
 *  Function to pack QMI command to query APN data statistics 
 *  This maps to SLQSQosSwiReadDataStats
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param[in]  reqParam
 *              - See \ref pack_qos_SLQSQosSwiReadDataStats_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   - Timeout: 2 seconds
 *          - PDN Specific: Yes
 */
 int pack_qos_SLQSQosSwiReadDataStats (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_qos_SLQSQosSwiReadDataStats_t  reqParam 
        );

/**
 * This structure contains the Data statistic per QoS flow
 *
 *  \param  bearerId
 *          - Bearer ID
 *
 *  \param  tx_pkt
 *          - number of sent packets for the QoS flow ID
 *
 *  \param  tx_pkt_drp
 *          - number of dropped(TX) packets for the QoS flow ID
 *
 *  \param  tx_bytes
 *          - number of sent bytes for the QoS flow ID
 *
 *  \param  tx_bytes_drp
 *          - number of dropped(TX) bytes for the QoS flow ID
 */
typedef struct
{
    uint32_t   bearerId;
    uint32_t   tx_pkt;
    uint32_t   tx_pkt_drp;
    uint64_t   tx_bytes;
    uint64_t   tx_bytes_drp;
} unpack_QosFlowStat_t;
/**
 *  Structure that contains APN data statistics
 *  \param  apnId
 *          - APN id
 *          - ID identifing the connected APN that the client would like to query the data statistic for
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_tx_pkt
 *          - sum of all packets sent
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_tx_pkt_drp
 *          - sum of all(TX) packets dropped
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_rx_pkt
 *          - sum of all packets received
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_tx_bytes
 *          - sum of all bytes sent
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_tx_bytes_drp
 *          - sum of all(TX) bytes dropped
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  total_rx_bytes
 *          - number of received bytes for the QoS flow ID
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  \param  numQosFlow
 *          - pointer to number of QoS flow Stat
 *          - Bit to check in ParamPresenceMask - <B>4</B>
 *
 *  \param  qosFlow[LITEQMI_MAX_QOS_FLOW_PER_APN_STATS]
 *          - Data statistic per QoS flow
 *          - See \ref unpack_QosFlowStat_t for more information
 *          - See \ref LITEQMI_MAX_QOS_FLOW_PER_APN_STATS for more information
 *          - Bit to check in ParamPresenceMask - <B>4</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t             apnId;
    uint32_t             total_tx_pkt;
    uint32_t             total_tx_pkt_drp;
    uint32_t             total_rx_pkt;
    uint64_t             total_tx_bytes;
    uint64_t             total_tx_bytes_drp;
    uint64_t             total_rx_bytes;
    uint32_t             numQosFlow;
    unpack_QosFlowStat_t qosFlow[LITEQMI_MAX_QOS_FLOW_PER_APN_STATS];
    swi_uint256_t        ParamPresenceMask;
} unpack_qos_SLQSQosSwiReadDataStats_t;

/**
 *  Function to unpack APN data statistics response
 *  This maps to SLQSQosSwiReadDataStats
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSQosSwiReadDataStats_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_qos_SLQSQosSwiReadDataStats(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSQosSwiReadDataStats_t *pOutput
        );

/**
 *  Structure with network's QoS status
 *  \param  status
 *          Network QoS support status
 *          - 0x00 - Current network does not support QoS
 *          - 0x01 - Current network supports QoS
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  \note   - Technology Supported: CDMA 
 */
typedef struct {
    uint8_t status;
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSSetQosNWStatusCallback_ind_t;

/**
 *  Function to unpack QoS NW status indication. 
 *  This maps to SLQSSetQosNWStatusCallback
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSSetQosNWStatusCallback_ind_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   
 *          - Technology Supported: CDMA 
 *          - This is a broadcast notification that is sent to the registered QoS service
 *          - No explicit function to register for this indication is needed
 *          - Please use eQMI_QOS_NETWORK_STATUS_IND indication to identify this event from QOS service read function
 */
int unpack_qos_SLQSSetQosNWStatusCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSSetQosNWStatusCallback_ind_t *pOutput
        );

/**
 *  Structure with QoS status indication details
 *  \param  id
 *          - Index identifying the QoS flow whose status is being reported
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  status
 *          Current QoS flow status:
 *          - 0x01 - QMI_QOS_STATUS_ACTIVATED
 *          - 0x02 - QMI_QOS_STATUS_SUSPENDED
 *          - 0x03 - QMI_QOS_STATUS_GONE
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  event
 *          - 0x01 - QMI_QOS_ACTIVATED_EV
 *          - 0x02 - QMI_QOS_SUSPENDED_EV
 *          - 0x03 - QMI_QOS_GONE_EV
 *          - 0x04 - QMI_QOS_MODIFY_ACCEPTED_EV
 *          - 0x05 - QMI_QOS_MODIFY_REJECTED_EV
 *          - 0x06 - QMI_QOS_INFO_CODE_UPDATED_EV
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  reason
 *          - 0x01 - QMI_QOS_INVALID_PARAMS
 *          - 0x02 - QMI_QOS_INTERNAL_CALL_ENDED
 *          - 0x03 - QMI_QOS_INTERNAL_ERROR
 *          - 0x04 - QMI_QOS_INSUFFICIENT_LOCAL_Resources
 *          - 0x05 - QMI_QOS_TIMED_OUT_OPERATION
 *          - 0x06 - QMI_QOS_INTERNAL_UNKNOWN_CAUSE_CODE
 *          - 0x07 - QMI_QOS_INTERNAL_MODIFY_IN_PROGRESS
 *          - 0x08 - QMI_QOS_NOT_SUPPORTED
 *          - 0x09 - QMI_QOS_NOT_AVAILABLE
 *          - 0x0A - QMI_QOS_NOT_GUARANTEED
 *          - 0x0B - QMI_QOS_INSUFFICIENT_NETWORK_RESOURCES
 *          - 0x0C - QMI_QOS_AWARE_SYSTEM
 *          - 0x0D - QMI_QOS_UNAWARE_SYSTEM
 *          - 0x0E - QOS_REJECTED_OPERATION
 *          - 0x0F - QMI_QOS_WILL_GRANT_WHEN_QOS_RESUMED
 *          - 0x10 - QMI_QOS_NETWORK_CALL_ENDED
 *          - 0x11 - QMI_QOS_NETWORK_SERVICE_NOT_AVAILABLE
 *          - 0x12 - QMI_QOS_NETWORK_L2_LINK_RELEASED
 *          - 0x13 - QMI_QOS_NETWORK_L2_LINK_REESTAB_REJ
 *          - 0x14 - QMI_QOS_NETWORK_L2_LINK_REESTAB_IND
 *          - 0x15 - QMI_QOS_NETWORK_UNKNOWN_CAUSE_CODE
 *          - 0x16 - QMI_NETWORK_BUSY
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t id;
    uint8_t  status;
    uint8_t  event;
    uint8_t  reason;
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSSetQosStatusCallback_ind_t;

/**
 *  Function to unpack QoS status indications. 
 *  This maps to SLQSSetQosStatusCallback
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSSetQosStatusCallback_ind_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   
 *          - This is a broadcast notification that is sent to the registered QoS service
 *          - No explicit function to register for this indication is needed
 *          - Please use eQMI_QOS_FLOW_STATUS_IND indication to identify this event from QOS service read function
 */
int unpack_qos_SLQSSetQosStatusCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSSetQosStatusCallback_ind_t *pOutput
        );

/**
 *  Structure with QoS primary flow events
 *  \param  event
 *          Event which causes this indication:
 *          - 0x0001 - Primary flow QoS modify operation success
 *          - 0x0002 - Primary flow QoS modify operation failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint16_t  event;
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSSetQosPriEventCallback_ind_t;

/**
 *  Function to unpack QoS primary flow events. 
 *  This maps to SLQSSetQosPriEventCallback
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSSetQosPriEventCallback_ind_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   
 *          - This is a broadcast notification that is sent to the registered QoS service
 *          - No explicit function to register for this indication is needed
 *          - Please use eQMI_QOS_PRIMARY_QOS_EVENT_IND indication to identify this event from QOS service read function
 *          - This is only generated when the primary flow is modified by the host
 */
int unpack_qos_SLQSSetQosPriEventCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSSetQosPriEventCallback_ind_t *pOutput
        );

/**
 *  Structure that contains the APN ID to obtain data statistics
 *  \param[in]  enable
 *          - 1 - Enable QoS event reporting
 *          - 0 - Disable QoS event reporting
 */
typedef struct {
    uint8_t enable;
} pack_qos_SLQSSetQosEventCallback_t;

/**
 *  Function to pack QMI command to enable QoS event indications 
 *  This maps to SLQSSetQosEventCallback
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param[in]  reqParam
 *              - See \ref pack_qos_SLQSSetQosEventCallback_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   - Timeout: 2 seconds
 *          - PDN Specific: Yes
 */
 int pack_qos_SLQSSetQosEventCallback (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_qos_SLQSSetQosEventCallback_t  reqParam 
        );

/**
 *  This structure contains unpack QOS SLQSSetQosEventCallback.
 *  param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct {
    swi_uint256_t  ParamPresenceMask;
}unpack_qos_SLQSSetQosEventCallback_t;
/**
 *  Function to unpack enable QoS event indications command's response
 *  This maps to SLQSSetQosEventCallback
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *               - See \ref unpack_qos_SLQSSetQosEventCallback_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_qos_SLQSSetQosEventCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSSetQosEventCallback_t *pOutput
        );

/**
 *  This structure contains QoS flow state
 *
 *  \param  id
 *          QoS identifier
 *
 *  \param  isNewFlow
 *          - 1 - Newly added flow
 *          - 0 - Existing flow
 *
 *  \param  state
 *          This indicates that the flow that was added/modified/deleted:
 *          - 0x01 - Flow activated
 *          - 0x02 - Flow modified
 *          - 0x03 - Flow deleted
 *          - 0x04 - Flow suspended
 *          - 0x05 - Flow enabled
 *          - 0x06 - Flow disabled
 */
typedef struct
{
    uint32_t   id;
    uint8_t    isNewFlow;
    uint8_t    state;
} unpack_qos_QosFlowInfoState_t;

/**
 * This structure contains the IP flow data rate min max
 *
 *  \param  dataRateMax
 *          Maximum required data rate (bits per second)
 *
 *  \param  guaranteedRate
 *          Minimum guaranteed data rate (bits per second)
 *
 */
typedef struct
{
    uint32_t   dataRateMax;
    uint32_t   guaranteedRate;
} unpack_qos_dataRate_t;

/**
 * This structure contains the TP flow data rate token bucket
 *
 *  \param  peakRate
 *          Maximum rate at which data can be transmitted when the token bucket is full (bits per second)
 *  \param  tokenRate
 *          Rate at which tokens will be put in the token bucket (bits per second); a token is required to be present in the bucket to send a byte of data
 *
 *  \param  bucketSz
 *          Maximum number of tokens that can be accumulated at any instance (bytes); controls the size of the burst that is allowed at any given time
 *
 */
typedef struct
{
    uint32_t   peakRate;
    uint32_t   tokenRate;
    uint32_t   bucketSz;
} unpack_qos_tokenBucket_t;

/**
 * This structure contains the IP flow packet error rate
 *
 *  \param  multiplier
 *          Factor m in calculating packet error rate:
 *          E = m*10**(-p)
 *
 *  \param  exponent
 *          Factor p in calculating packet error rate (see above)
 *
 */
typedef struct
{
    uint16_t    multiplier;
    uint16_t    exponent;
} unpack_qos_pktErrRate_t;

/**
 * This structure contains the QoS Flow Request.\n
 * Please check is_<Param_Name>_Available field for presence of optional parameters
 *
 *  \param  index
 *          - Mandatory parameter
 *          - IP flow index
 *          - Integer that uniquely identifies each flow instance
 *          - Unique index must be assigned by the control point to every flow_spec instance
 *
 *  \param  ProfileId3GPP2
 *          - IP flow 3GPP2 profile ID
 *          - A profile ID is shorthand for a defined set of QoS flow parameters specified by the network; to be present while requesting QoS for a CDMA device
 *
 *  \param  val_3GPP2Pri
 *          - IP flow 3GPP2 flow priority
 *          - Flow priority used by the network in case of contention between flows with same QoS; this parameter applies for CDMA devices
 *
 *  \param  TrafficClass
 *          - IP flow traffic class
 *          - Integer that designates the requested traffic class:
 *          - 0 - Conversational
 *          - 1 - Streaming
 *          - 2 - Interactive
 *          - 3 - Background
 *
 *  \param  DataRate
 *          - IP flow data rate min max
 *          - See \ref unpack_qos_dataRate_t for more information
 *
 *  \param  TokenBucket
 *          - IP flow data rate token bucket
 *          - See \ref unpack_qos_tokenBucket_t for more information
 *
 *  \param  Latency
 *          - IP flow latency
 *          - Maximum delay (in milliseconds) that can be tolerated by an IP packet during transfer through the wireless link
 *
 *  \param  Jitter
 *          - IP flow jitter
 *          - Difference between the maximum and minimum latency (in milliseconds) that can be tolerated by an IP packet during the transfer through the wireless link
 *
 *  \param  PktErrRate
 *          - IP flow packet error rate
 *          - See \ref unpack_qos_pktErrRate_t for more information
 *
 *  \param  MinPolicedPktSz
 *          - IP flow minimum policed packet size
 *          - Integer that defines the minimum packet size (in bytes) that will be policed for QoS guarantees; any IP packets that are smaller than the minimum specified policed size may not receive requested QoS
 *
 *  \param  MaxAllowedPktSz
 *          - IP flow maximum allowed packet size
 *          - Integer that defines the maximum packet size (in bytes) allowed in the IP flow; any IP packets greater in size than the maximum allowed packet size are not queued for transmission
 *
 *  \param  val_3GPPResResidualBER
 *          - IP flow 3GPP residual bit error rate
 *          - residual_bit_error_rate
 *          - 0 = 5*10-2 residual BER
 *          - 1 = 1*10-2 residual BER
 *          - 2 = 5*10-3 residual BER
 *          - 3 = 4*10-3 residual BER
 *          - 4 = 1*10-3 residual BER
 *          - 5 = 1*10-4 residual BER
 *          - 6 = 1*10-5 residual BER
 *          - 7 = 1*10-6 residual BER
 *          - 8 = 6*10-8 residual BER
 *          - Integer that indicates the undetected BER for each IP flow in the delivered packets; Tapplies only to 3GPP networks
 *
 *  \param  val_3GPPTraHdlPri
 *          - 3GPP traffic handling priority
 *          - 0 - Relative traffic handling priority 1
 *          - 1 - Relative traffic handling priority 2
 *          - 2 - Relative traffic handling priority 3
 *          - Defines the relative priority of the flow; applies only to 3GPP networks
 *
 *  \param  val_3GPPImCn
 *          - IP flow 3GPP IM CN flag
 *          - IM CN subsystem signaling flag:
 *          - 0x00 - FALSE
 *          - 0x01 - TRUE
 *          - This parameter applies only to 3GPP networks
 *
 *  \param  val_3GPPSigInd
 *          - IP flow 3GPP signaling indication
 *          - 0x00 - FALSE
 *          - 0x01 - TRUE
 *          - This parameter applies only to 3GPP networks
 *
 *  \param  LteQci
 *          - LTE QoS Class Identifier
 *          - QoS Class Identifier(QCI) is a required parameter to request QoS in LTE
 *          - QCI values:
 *            - QCI value 0 requests the network to assign the appropriate QCI value
 *            - QCI values 1-4 are associated with guaranteed bitrates
 *            - QCI values 5-9 are associated with nonguaranteed bitrates, so the values specified as guaranteed and maximum bitrates are ignored
 */
typedef struct
{
    uint8_t                  index;
	uint8_t                  is_ProfileId3GPP2_Available;
    uint16_t                 ProfileId3GPP2;
	uint8_t                  is_val_3GPP2Pri_Available;
    uint8_t                  val_3GPP2Pri;
	uint8_t                  is_TrafficClass_Available;
    uint8_t                  TrafficClass;
	uint8_t                  is_DataRate_Available;
    unpack_qos_dataRate_t    DataRate;
	uint8_t                  is_TokenBucket_Available;
    unpack_qos_tokenBucket_t TokenBucket;
	uint8_t                  is_Latency_Available;
    uint32_t                 Latency;
	uint8_t                  is_Jitter_Available;
    uint32_t                 Jitter;
	uint8_t                  is_PktErrRate_Available;
    unpack_qos_pktErrRate_t  PktErrRate;
	uint8_t                  is_MinPolicedPktSz_Available;
    uint32_t                 MinPolicedPktSz;
	uint8_t                  is_MaxAllowedPktSz_Available;
    uint32_t                 MaxAllowedPktSz;
	uint8_t                  is_val_3GPPResResidualBER_Available;
    uint16_t                 val_3GPPResResidualBER;
	uint8_t                  is_val_3GPPTraHdlPri_Available;
    uint8_t                  val_3GPPTraHdlPri;
	uint8_t                  is_val_3GPPImCn_Available;
    uint8_t                  val_3GPPImCn;
	uint8_t                  is_val_3GPPSigInd_Available;
    uint8_t                  val_3GPPSigInd;
	uint8_t                  is_LteQci_Available;
    uint8_t                  LteQci;
} unpack_qos_swiQosFlow_t;

/**
 * This structure contains the IPv4 filter address
 *
 *  \param  addr
 *          IPv4 address
 *
 *  \param  subnetMask
 *          A packet matches if:
 *          - (addr and subnetMask) == (IP pkt addr & subnetMask)
 *          Callers to set up a filter with a range of source addresses, if needed; subnet mask of all 1s (255.255.255.255) specifies a single address value
 *
 */
typedef struct
{
    uint32_t   addr;
    uint32_t   subnetMask;
} unpack_qos_IPv4Addr_t;

/**
 * This structure contains the IPv4 filter type of service
 *
 *  \param  val
 *          Type of service value
 *
 *  \param  mask
 *          Packet matches the TOS filter if:
 *          (IPv4_filter_tos_val and IPv4_filter_tos_mask) == (TOS value in the IP packet & IPv4_filter_tos_mask)
 *          Example:
 *          - IPv4_filter_tos_val = 00101000
 *          - IPv4_filter_tos_mask = 11111100
 *          The filter will compare only the first 6 bits in the IPv4_filter_type_of_service with the first 6 bits in the TOS field of the IP packet. The first 6 bits in the TOS field of the IP packet must be 001010 to match the filter. The last 2 bits can be anything since they are ignored by filtering.
 *
 */
typedef struct
{
    uint8_t   val;
    uint8_t   mask;
} unpack_qos_Tos_t;

/**
 * This structure contains the IPv6 filter address
 *
 *  \param  addr
 *          IPv6 address (in network byte order); this is a 16-byte byte array (in Big-endian format)
 *
 *  \param  prefixLen
 *          IPv6 filter prefix length; can take a value between 0 and 128
 *          Note: A packet matches if the IPv6 source address bytes until the prefix lengths are equal. Therefore prefix length can be used to set a filter with a range of source addresses. A prefix length of 128 specifies a single address value.
 *
 */
typedef struct
{
    uint8_t    addr[16];
    uint8_t    prefixLen;
} unpack_qos_IPv6Addr_t;

/**
 * This structure contains the IPv6 filter traffic class
 *
 *  \param  val
 *          The traffic class value
 *
 *  \param  mask
 *          The packet matches the traffic class filter if:
 *          (IPv6_filter_traffic_class_val and IPv6_filter_traffic_class_mask) == (Traffic class value in the IP packet & IPv6_filter_traffic_class_mask)
 *          Example:
 *          - IPv6_filter_tc_val = 00101000
 *          - IPv6_filter_tc_mask = 11111100
 *          Filter will compare only the first 6 bits in IPv6_filter_traffic_class with the first 6 bits in the traffic class field of the IP packet; first 6 bits in the traffic class field of the IP packet must be 001010 to match filter; last 2 bits can be anything, since they are ignored by filtering
 *
 */
typedef struct
{
    uint8_t   val;
    uint8_t   mask;
} unpack_qos_IPv6TrafCls_t;

/**
 * This structure contains the Port Filter
 *
 *  \param  port
 *          port value of the filter
 *
 *  \param  range
 *          range specifies the number of ports to be included in the filter starting from port; filter will match if port in the IP packet lies between port and (port + range )
 *          Range value of 0 implies that only one value of the port is valid, as specified by the port
 *
 */
typedef struct
{
    uint16_t   port;
    uint16_t   range;
} unpack_qos_Port_t;

/**
 * This structure contains the QoS Filter Request.\n
 * Please check is_<Param_Name>_Available field for presence of optional parameters
 *
 *  \param  index
 *          Mandatory parameter
 *          IP filter index
 *          Integer that uniquely identifies each filter instance
 *          This TLV must be present in the request
 *
 *  \param  version
 *          Mandatory parameter
 *          IP filter version
 *          Identifies whether the filter is associated with IPv4 or IPv6; value specified also implies that only TLVs defined for that IP version, i.e., TLVs with IPv4 or IPv6 in the name, can be specified
 *          - 0x04 - IPv4
 *          - 0x06 - Ipv6
 *
 *  \param  IPv4SrcAddr
 *          IPv4 filter soruce address
 *          See \ref unpack_qos_IPv4Addr_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  IPv4DstAddr
 *          IPv4 filter destination address
 *          See \ref unpack_qos_IPv4Addr_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  NxtHdrProto
 *          IP filter next header protocol
 *          This TLV must be present if any non-IP filter TLV(s) are provided
 *          If this field is specified, only IP packets belonging to specified higher layer protocol are considered when filtering
 *          The following protocols may be specified:
 *          - 0x01 = ICMP
 *          - 0x06 = TCP
 *          - 0x11 = UDP
 *          - 0x32 = ESP
 *          Note: The next header protocol field will be set to 0xFD (TCP & UDP)
 *                if a TFT is received specifying a source or destination port number,
 *                but IP next header type is not specified.
 *
 *  \param  IPv4Tos
 *          IPv4 filter type of service
 *          See \ref unpack_qos_Tos_t for more information
 *
 *  \param  IPv6SrcAddr
 *          IPv6 filter soruce address
 *          See \ref unpack_qos_IPv6Addr_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  IPv6DstAddr
 *          IPv6 filter destination address
 *          See \ref unpack_qos_IPv6Addr_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  IPv6TrafCls
 *          IPv6 filter traffic class
 *          See \ref unpack_qos_IPv6TrafCls_t for more information
 *
 *  \param  IPv6Label
 *          IPv6 flow label
 *          Packet matches the IPv6 flow label filter if:
 *          ( *pIPv6Label == flow label in the IPv6 header)
 *          - Implemented only for unsolicited indication
 *
 *  \param  TCPSrcPort
 *          TCP filter source port filter
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  TCPDstPort
 *          TCP filter destination port filter
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  UDPSrcPort
 *          UDP filter source port filter
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  UDPDstPort
 *          UDP filter destination port filter
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  EspSpi
 *          ESP filter security policy index
 *          Security policy index to uniquely identify each IP flow for filtering encrypted packets for encapsulating security payload
 *          - Implemented only for unsolicited indication
 *
 *  \param  Precedence
 *          Filter Precedence
 *          Specifies the order in which filters are applied; lower numerical value has higher precedence
 *          Note: This TLV only applies to network-initiated QoS; QoS requests containing this TLV from control points will be ignored
 *
 *  \param  Id
 *          Filter ID
 *          Unique identifier for each filter;filter ID is assigned by the modem
 *          Note: This TLV only applies to network-initiated QoS; QoS requests containing this TLV from control points will be ignored
 *
 *  \param  TranSrcPort
 *          Transport protocolfilter source port
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 *
 *  \param  UDPDstPort
 *          Transport protocol filter destination port
 *          See \ref unpack_qos_Port_t for more information
 *          - Implemented only for unsolicited indication
 */
typedef struct
{
    uint8_t                  index;
    uint8_t                  version;
	uint8_t                  is_IPv4SrcAddr_Available;
    unpack_qos_IPv4Addr_t    IPv4SrcAddr;
	uint8_t                  is_IPv4DstAddr_Available;
    unpack_qos_IPv4Addr_t    IPv4DstAddr;
	uint8_t                  is_NxtHdrProto_Available;
    uint8_t                  NxtHdrProto;
	uint8_t                  is_IPv4Tos_Available;
    unpack_qos_Tos_t         IPv4Tos;
	uint8_t                  is_IPv6SrcAddr_Available;
    unpack_qos_IPv6Addr_t    IPv6SrcAddr;
	uint8_t                  is_IPv6DstAddr_Available;
    unpack_qos_IPv6Addr_t    IPv6DstAddr;
	uint8_t                  is_IPv6TrafCls_Available;
    unpack_qos_IPv6TrafCls_t IPv6TrafCls;
	uint8_t                  is_IPv6Label_Available;
    uint32_t                 IPv6Label;
	uint8_t                  is_TCPSrcPort_Available;
    unpack_qos_Port_t        TCPSrcPort;
	uint8_t                  is_TCPDstPort_Available;
    unpack_qos_Port_t        TCPDstPort;
	uint8_t                  is_UDPSrcPort_Available;
    unpack_qos_Port_t        UDPSrcPort;
	uint8_t                  is_UDPDstPort_Available;
    unpack_qos_Port_t        UDPDstPort;
	uint8_t                  is_EspSpi_Available;
    uint32_t                 EspSpi;
	uint8_t                  is_Precedence_Available;
    uint16_t                 Precedence;
	uint8_t                  is_Id_Available;
    uint16_t                 Id;
	uint8_t                  is_TranSrcPort_Available;
    unpack_qos_Port_t        TranSrcPort;
	uint8_t                  is_TranDstPort_Available;
    unpack_qos_Port_t        TranDstPort;
} unpack_qos_swiQosFilter_t;
		
/**
 *  Structure with QoS flow details.\n
 *  Please check is_<Param_Name>_Available field for presence of optional parameters
 *
 *  \param  QFlowState
 *          - QoS flow state information, please check \ref unpack_qos_QosFlowInfoState_t for more information
 *
 *  \param  is_TxQFlowGranted_Available
 *          - TRUE if optional TxQFlowGranted is available
 *
 *  \param  TxQFlowGranted
 *          - The Tx Qos flow granted, please check \ref unpack_qos_swiQosFlow_t for more information
 *
 *  \param  is_RxQFlowGranted_Available
 *          - TRUE if optional RxQFlowGranted is available
 * 
 *  \param  RxQFlowGranted
 *          - The Rx Qos flow granted, please check \ref unpack_qos_swiQosFlow_t for more information
 *
 *  \param  NumTxFilters
 *          - Number of Tx filters available
 * 
 *  \param  TxQFilter
 *          - The Tx Qos filter, please check \ref unpack_qos_swiQosFilter_t for more information
 *          - See \ref LITEQMI_MAX_QOS_FILTERS for more information
 *
 *  \param  NumRxFilters
 *          - Number of Tx filters available
 * 
 *  \param  RxQFilter
 *          - The Rx Qos filter, please check \ref unpack_qos_swiQosFilter_t for more information
 *          - See \ref LITEQMI_MAX_QOS_FILTERS for more information
 *
 *  \param  BearerID
 *          - The bearer ID
 *          - Bearer ID or Radio Link Protocol (RLP) ID of the activated flow.
 *          - Valid Values - 0 to 16
 *          - 0xFF - Invalid value. 
 */
typedef struct {
    unpack_qos_QosFlowInfoState_t QFlowState;
	uint8_t                       is_TxQFlowGranted_Available;
    unpack_qos_swiQosFlow_t       TxQFlowGranted;
	uint8_t                       is_RxQFlowGranted_Available;
    unpack_qos_swiQosFlow_t       RxQFlowGranted;
	uint8_t                       NumTxFilters;
    unpack_qos_swiQosFilter_t     TxQFilter[LITEQMI_MAX_QOS_FILTERS];
	uint8_t                       NumRxFilters;
    unpack_qos_swiQosFilter_t     RxQFilter[LITEQMI_MAX_QOS_FILTERS];
    uint8_t                       BearerID;	
} unpack_qos_QosFlowInfo_t;


/**
 *  Structure with QoS event details
 *
 *  \param  NumFlows
 *          - Number of QoS flows available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  QosFlowInfo
 *          - The Qos flow details, please check \ref unpack_qos_QosFlowInfo_t for more information
 *          - See \ref LITEQMI_MAX_QOS_FLOWS for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t                       NumFlows;
    unpack_qos_QosFlowInfo_t      QosFlowInfo[LITEQMI_MAX_QOS_FLOWS];
    swi_uint256_t  ParamPresenceMask;
} unpack_qos_SLQSSetQosEventCallback_ind_t;

/**
 *  Function to unpack QoS event indications  
 *  This maps to SLQSSetQosEventCallback
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *          - See \ref unpack_qos_SLQSSetQosEventCallback_ind_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   
 *          - This is a broadcast notification that is sent to the registered QoS service
 *          - No explicit function to register for this indication is needed
 *          - Please use eQMI_QOS_NETWORK_STATUS_IND indication to identify this event from QOS service read function
 */
int unpack_qos_SLQSSetQosEventCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_SLQSSetQosEventCallback_ind_t *pOutput
        );



/**
 *  Structure that contains the request Binds a control point to a data port Periapheral End Point Type data.
 *  \param  EndPointType
 *              Peripheral end point type. Values:
 *              - 0 - Reserved
 *              - 1 - HSIC 
 *              - 2 - HSUSB
 *              - 3 - PCIE 
 *              - 4 - Embedded 
 *
 *  \param  IfaceID
 *              Peripheral interface number.
 */
typedef struct {
    uint32_t EndPointType;
    uint32_t IfaceID;
} qos_BindDataPortPeripheralEndPointID_t;


/**
 *  Structure that contains the request Binds a control point to a data port Mux ID data.
 *  \param  MuxID
 *              Mux ID:
 *              - 0x80-0x8F : valid value
 */
typedef struct {
    uint8_t MuxID;
} qos_BindDataPortMuxID_t;

/**
 *  Structure that contains the request Binds a control point to a data port SIO Data Port data.
 *  \param  u16SIODataPort
 *              SIO Data Port to which the client binds
 */
typedef struct {
    uint16_t SIODataPort;
} qos_BindDataPortSIODataPort_t;


/**
 *  Structure that contains the request Binds a control point to a data port.
 *
 *  \param  pPeripheralEndPointID
 *       - See \ref qos_BindDataPortPeripheralEndPointID_t for more information.
 *
 *  \param  pMuxID
 *       - See \ref qos_BindDataPortMuxID_t for more information.
 *
 *  \param  pSIODataPort
 *       - See \ref qos_BindDataPortSIODataPort_t for more information.
 */
typedef struct {
    qos_BindDataPortPeripheralEndPointID_t *pPeripheralEndPointID;  
    qos_BindDataPortMuxID_t *pMuxID;
    qos_BindDataPortSIODataPort_t *pSIODataPort;
} pack_qos_BindDataPort_t;

/**
 *  Function to pack command to Binds a control point to a data port.
 *  This maps to PkQmiQosBindDataPort
 *
 *  \param[out]  pCtx
 *               - See \ref pack_qmi_t for more information
 *
 *  \param[in,out]  pReqBuf
 *                  - Buffer for packed QMI command to be provided by the host application
 *                  - Minimum expected size is 2048 bytes
 *
 *  \param[in,out]  pLen
 *                  - On input, size of pReqBuf
 *                  - On ouptut, number of bytes actually packed
 *
 *  \param[in]  req
 *              - See \ref pack_qos_BindDataPort_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note
 *          - Timeout: 2 seconds
 */
int pack_qos_BindDataPort (
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_qos_BindDataPort_t req
        ); 
/**
 * This structure contains unpack Qos BindDataPort.
 * \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
**/
typedef struct{
    swi_uint256_t  ParamPresenceMask;
}unpack_qos_BindDataPort_t;
/**
 *  Function to unpack the response to Binds a control point to a data port.
 *  This maps to UpkQmiQosBindDataPort
 *
 *  \param[in]  pResp
 *              - Response from modem
 *
 *  \param[in]  respLen
 *              - Length of pResp from modem
 *
 *  \param[out]  pOutput
 *          - See \ref unpack_qos_BindDataPort_t for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int unpack_qos_BindDataPort(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_qos_BindDataPort_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif //__LITEQMI_QOS_H__
