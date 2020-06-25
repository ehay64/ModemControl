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
#include "qos.h"

#include "packingdemo.h"

#define QMI_QOS_MSG_MAX QMI_MSG_MAX

// FUNCTION PROTOTYPES
int client_fd(uint8_t svc);

// QOS GLOBALS
volatile int enQosThread;
int qos_fd = -1;
pthread_t qos_tid = 0;
pthread_attr_t qos_attr;
static char remark[255]={0};
static int unpackRetCode = 0;

void qos_dummy_unpack();
int qos_validate_dummy_unpack();

/* SetEvent */
swi_uint256_t unpack_qos_SLQSSetQosEventCallbackParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

swi_uint256_t unpack_qos_SLQSSetQosEventCallbackParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,2)
}};

/* SetEventIndication */
swi_uint256_t unpack_qos_SLQSSetQosEventCallback_indParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_1_BITS,16)
}};

/* ReadDataStats */
swi_uint256_t unpack_qos_SLQSQosSwiReadDataStatsParamPresenceMaskWhiteList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,3,4)
}};

swi_uint256_t unpack_qos_SLQSQosSwiReadDataStatsParamPresenceMaskMandatoryList={{
    SWI_UINT256_BIT_VALUE(SET_3_BITS,2,3,4)
}};

// TEST VALUES
#ifdef QOS_EVENTS_DEBUG
    uint8_t qos_event_sample[] = {
        0x04,0x00,0x00,
        0x01,0x00,0x5E +22+22+22,0x02,

        0x10,0x55,0x01,

        0x10,0x06,0x00,0x50,0x94,0xDC,0x47,0x01,0x01,

        0x14,0x95,0x00,
        0x10,0x92,0x00,
        0x23,0x02,0x00,0x01,0x02,
        0x22,0x02,0x00,0x21,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
        0x16,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,


        0x13,0x95,0x00,
        0x10,0x92,0x00,
        0x23,0x02,0x00,0x00,0x02,
        0x22,0x02,0x00,0x20,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x16,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,

        0x12,0x07,0x00,
        0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

        0x11,0x07,0x00,
        0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

        0x15 ,0x01,0x00,0x01,
        // bearer id 
        0x16, 0x01,0x00, 0xAB,

        0x10,0x07+22+22+22,0x01,
        0x10,0x06,0x00,0xB0,0x56,0xD1,0x43,0x01,0x01,

        0x14,0x16+22+22,0x00,
        0x10,0x13,0x00,
        0x23,0x02,0x00,0x01,0x00,
        0x22,0x02,0x00,0x11,0x00,
        0x15,0x02,0x00,0xA0,0xE0,
        0x11,0x01,0x00,0x04,

        0x10,0x13,0x00,
        0x23,0x02,0x00,0x04,0x00,
        0x22,0x02,0x00,0x04,0x00,
        0x15,0x02,0x00,0x04,0x00,
        0x11,0x01,0x00,0x06,

        0x10,0x13,0x00,
        0x23,0x02,0x00,0x08,0x00,
        0x22,0x02,0x00,0x08,0x00,
        0x15,0x02,0x00,0x08,0x00,
        0x11,0x01,0x00,0x08,

        0x13,0x16+22,0x00,
        0x10,0x13,0x00,
        0x23,0x02,0x00,0x00,0x00,
        0x22,0x02,0x00,0x10,0x00,
        0x15,0x02,0x00,0xA0,0xE0,
        0x11,0x01,0x00,0x04,

        0x10,0x13,0x00,
        0x23,0x02,0x00,0x5,0x00,
        0x22,0x02,0x00,0x5,0x00,
        0x15,0x02,0x00,0x5,0x5,
        0x11,0x01,0x00,0x05,

        0x12,0x61,0x00,
        0x10,0x5E,0x00,
        0x12,0x08,0x00,0x80,0x91,0x07,0x00,0x80,0x91,0x07,0x00,
        0x1F,0x01,0x00,0x01,
        0x18,0x04,0x00,0x01,0x01,0x01,0x01,
        0x1B,0x02,0x00,0x1B,0x1B,
        0x11,0x01,0x00,0x11,
        0x13,0x0C,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,
        0x14,0x04,0x00,0x01,0x02,0x03,0x04,
        0x15,0x04,0x00,0x01,0x01,0x01,0x01,
        0x16,0x04,0x00,0x01,0x01,0x01,0x01,
        0x17,0x04,0x00,0x01,0x01,0x01,0x01,
        0x19,0x01,0x00,0x11,
        0x1A,0x01,0x00,0x11,
        0x1C,0x01,0x00,0x11,
        0x1D,0x01,0x00,0x11,
        0x1E,0x01,0x00,0x11,

        0x11,0x61,0x00,
        0x10,0x5E,0x00,
        0x12,0x08,0x00,0x80 ,0x91,0x07,0x00,0x80,0x91,0x07,0x00,
        0x1F,0x01,0x00,0x01,
        0x18,0x04,0x00,0x01,0x01,0x01,0x01,
        0x1B,0x02,0x00,0x1B,0x1B,
        0x11,0x01,0x00,0x11,
        0x13,0x0C,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,
        0x14,0x04,0x00,0x01,0x02,0x03,0x04,
        0x15,0x04,0x00,0x01,0x01,0x01,0x01,
        0x16,0x04,0x00,0x01,0x01,0x01,0x01,
        0x17,0x04,0x00,0x01,0x01,0x01,0x01,
        0x19,0x01,0x00,0x11,
        0x1A,0x01,0x00,0x11,
        0x1C,0x01,0x00,0x11,
        0x1D,0x01,0x00,0x11,
        0x1E,0x01,0x00,0x11,

        0x15,0x01,0x00,0x01

    };
#endif //QOS_EVENTS_DEBUG

void qos_loop_exit(void)
{
    printf("\nkilling QoS read thread...\n");
    if(enQosThread==0)
    {
        if(qos_fd>=0)
            close(qos_fd);
        qos_fd=-1;
    }
    enQosThread = 0;
#ifdef __ARM_EABI__
    if(qos_fd>=0)
        close(qos_fd);
    qos_fd=-1;
    void *pthread_rtn_value;
    if(qos_tid!=0)
    pthread_join(qos_tid, &pthread_rtn_value);
#endif
    if(qos_tid!=0)
    pthread_cancel(qos_tid);
    qos_tid = 0;
    if(qos_fd>=0)
        close(qos_fd);
    qos_fd=-1;
    return;
}

void _print_flow(unpack_qos_swiQosFlow_t flow)
{
    fprintf (stderr, "\t\tIndex: %d\n", flow.index);
    if (flow.is_ProfileId3GPP2_Available)
        fprintf (stderr, "\t\t3Gpp2 Profile ID: %d\n", flow.ProfileId3GPP2);
    if (flow.is_val_3GPP2Pri_Available)
        fprintf (stderr, "\t\t3Gpp2 Flow Priority: %d\n", flow.val_3GPP2Pri);
    if (flow.is_TrafficClass_Available)
        fprintf (stderr, "\t\t3Gpp2 Traffic Class: %d\n", flow.TrafficClass);
    if (flow.is_DataRate_Available)
    {
        fprintf (stderr, "\t\tData Rate\n");
        fprintf (stderr, "\t\t\tMax Rate: %d\n", flow.DataRate.dataRateMax);
        fprintf (stderr, "\t\t\tGuaranteed Rate: %d\n", flow.DataRate.guaranteedRate);
    }
    if (flow.is_TokenBucket_Available)
    {
        fprintf (stderr, "\t\tToken Bucket\n");
        fprintf (stderr, "\t\t\tPeak Rate: %d\n", flow.TokenBucket.peakRate);
        fprintf (stderr, "\t\t\tToken Rate: %d\n", flow.TokenBucket.tokenRate);    
        fprintf (stderr, "\t\t\tBucket Size: %d\n", flow.TokenBucket.bucketSz);    
    }
    if (flow.is_Latency_Available)
        fprintf (stderr, "\t\tLatency: %d\n", flow.Latency);
    if (flow.is_Jitter_Available)
        fprintf (stderr, "\t\tJitter: %d\n", flow.Jitter);
    if (flow.is_PktErrRate_Available)
    {
        fprintf (stderr, "\t\tPacket Error Rate\n");
        fprintf (stderr, "\t\t\tMultiplier: %d\n", flow.PktErrRate.multiplier);
        fprintf (stderr, "\t\t\tExponent: %d\n", flow.PktErrRate.exponent);    
    }
    if (flow.is_MinPolicedPktSz_Available)
        fprintf (stderr, "\t\tMinimum Pkt Size: %d\n", flow.MinPolicedPktSz);
    if (flow.is_MaxAllowedPktSz_Available)
        fprintf (stderr, "\t\tMax Pkt Size: %d\n", flow.MaxAllowedPktSz);
    if (flow.is_val_3GPPResResidualBER_Available)
        fprintf (stderr, "\t\t3GPP residual bit error rate: %d\n", flow.val_3GPPResResidualBER);
    if (flow.is_val_3GPPTraHdlPri_Available)
        fprintf (stderr, "\t\tTraffic handling priority: %d\n", flow.val_3GPPTraHdlPri);
    if (flow.is_val_3GPPImCn_Available)
        fprintf (stderr, "\t\t3GPP IM CN flag: %d\n", flow.val_3GPPImCn);
    if (flow.is_val_3GPPSigInd_Available)
        fprintf (stderr, "\t\t3GPP signaling indication: %d\n", flow.val_3GPPSigInd);
    if (flow.is_LteQci_Available)
        fprintf (stderr, "\t\tLTE QCI: %d\n", flow.LteQci);
    return;
}

void _print_filter(unpack_qos_swiQosFilter_t filter)
{
    int i = 0;
    fprintf (stderr, "\t\tIndex      : %d\n", filter.index);
    fprintf (stderr, "\t\tversion    : %d\n", filter.version);
    if (filter.is_Id_Available)
        fprintf (stderr, "\t\tID         : %d(0x%02x)\n", filter.Id,filter.Id);    
    if (filter.is_Precedence_Available)
        fprintf (stderr, "\t\tPrecedence : %d(0x%02x)\n", filter.Precedence,filter.Precedence);
    if (filter.is_IPv6Label_Available)
        fprintf (stderr, "\t\tIPv6 Label : %d\n", filter.IPv6Label);
    if (filter.is_EspSpi_Available)
        fprintf (stderr, "\t\tESP filter security policy index : %d\n", filter.EspSpi);    
    if (filter.is_NxtHdrProto_Available)
        fprintf (stderr, "\t\tNext Header Protocol             : %d\n", filter.NxtHdrProto);
    
    if (filter.is_IPv4SrcAddr_Available)
    {
        fprintf (stderr, "\t\tIPv4 Src Addr\n");
        fprintf (stderr, "\t\t\tAddress    : %u\n", filter.IPv4SrcAddr.addr);
        fprintf (stderr, "\t\t\tSubnet Mask: %u\n", filter.IPv4SrcAddr.subnetMask);
    }
    if (filter.is_IPv4DstAddr_Available)
    {
        fprintf (stderr, "\t\tIPv4 Dest Addr\n");
        fprintf (stderr, "\t\t\tAddress    : %u\n", filter.IPv4DstAddr.addr);
        fprintf (stderr, "\t\t\tSubnet Mask: %u\n",   filter.IPv4DstAddr.subnetMask);
    }
    if (filter.is_IPv4Tos_Available)
    {
        fprintf (stderr, "\t\tIPv4 ToS\n");
        fprintf (stderr, "\t\t\tValue  : %u(0x%02x)\n", filter.IPv4Tos.val,filter.IPv4Tos.val);
        fprintf (stderr, "\t\t\tMask   : %u(0x%02x)\n", filter.IPv4Tos.mask,filter.IPv4Tos.mask);
    }
    if (filter.is_IPv6SrcAddr_Available)
    {
        fprintf (stderr, "\t\tIPv6 Src Addr\n");
        fprintf (stderr, "\t\t\tAddress     : 0x");
        for(i=0;i<16;i++)
        {
            fprintf (stderr, "%02x", filter.IPv6SrcAddr.addr[i]);
        }
        fprintf (stderr, "\n");
        fprintf (stderr, "\t\t\tPrefix Len  : %u\n",   filter.IPv6SrcAddr.prefixLen);
    }    
    if (filter.is_IPv6DstAddr_Available)
    {
        fprintf (stderr, "\t\tIPv6 Dest Addr\n");
        fprintf (stderr, "\t\t\tAddress     : 0x");
        for(i=0;i<16;i++)
        {
            fprintf (stderr, "%02x", filter.IPv6DstAddr.addr[i]);
        }
        fprintf (stderr, "\n");
        fprintf (stderr, "\t\t\tPrefix Len  : %u\n",   filter.IPv6DstAddr.prefixLen);
    }
    if (filter.is_IPv6TrafCls_Available)
    {
        fprintf (stderr, "\t\tIPv6 Traffic Class\n");
        fprintf (stderr, "\t\t\tValue  : %u\n", filter.IPv6TrafCls.val);
        fprintf (stderr, "\t\t\tMask   : %u\n",   filter.IPv6TrafCls.mask);
    }    
    if (filter.is_TCPSrcPort_Available)
    {
        fprintf (stderr, "\t\tTCP src port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.TCPSrcPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.TCPSrcPort.range);
    }    
    if (filter.is_TCPDstPort_Available)
    {
        fprintf (stderr, "\t\tTCP dest port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.TCPDstPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.TCPDstPort.range);
    }
    if (filter.is_UDPSrcPort_Available)
    {
        fprintf (stderr, "\t\tUDP src port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.UDPSrcPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.UDPSrcPort.range);
    }
    if (filter.is_UDPDstPort_Available)
    {
        fprintf (stderr, "\t\tUDP dest port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.UDPDstPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.UDPDstPort.range);
    }
    if (filter.is_TranSrcPort_Available)
    {
        fprintf (stderr, "\t\tTransport src port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.TranSrcPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.TranSrcPort.range);
    }
    if (filter.is_TranDstPort_Available)
    {
        fprintf (stderr, "\t\tTransport dest port\n");
        fprintf (stderr, "\t\t\tPort  : %d\n", filter.TranDstPort.port);
        fprintf (stderr, "\t\t\tRange : %d\n", filter.TranDstPort.range);
    }    
}

void dump_qos_SLQSSetQosEventCallback(void* ptr)
{
    unpack_qos_SLQSSetQosEventCallback_t *unpackdataptr = NULL;
    unpackdataptr = (unpack_qos_SLQSSetQosEventCallback_t*) ptr;
    if(unpackdataptr==NULL)
        return ;
}

void dump_qos_SLQSSetQosEventCallback_ind(void* ptr)
{
    int i = 0;
    int j = 0;
    unpack_qos_SLQSSetQosEventCallback_ind_t
        *unpackdataptr = NULL;
    unpackdataptr = (unpack_qos_SLQSSetQosEventCallback_ind_t*) ptr;
    if(unpackdataptr==NULL)
        return ;
    if(!swi_uint256_get_bit(unpackdataptr->ParamPresenceMask, 16))
    {
       return ;
    }
    for (i=0;i<unpackdataptr->NumFlows;i++)
    {
        fprintf (stderr, "Flow Number: %d\n", (i+1));
        fprintf (stderr, "\tBearer ID  : %d\n", unpackdataptr->QosFlowInfo[i].BearerID);
        fprintf (stderr, "\tQoS Flow ID: %d\n", unpackdataptr->QosFlowInfo[i].QFlowState.id);
        fprintf (stderr, "\tisNewFlow  : %d\n", unpackdataptr->QosFlowInfo[i].QFlowState.isNewFlow);
        fprintf (stderr, "\tState      : %d\n", unpackdataptr->QosFlowInfo[i].QFlowState.state);
        if (unpackdataptr->QosFlowInfo[i].is_TxQFlowGranted_Available)
        {
            fprintf (stderr, "\tTx Flow Granted\n");
            fprintf (stderr, "\t===============\n");
            _print_flow(unpackdataptr->QosFlowInfo[i].TxQFlowGranted);
        }
        if (unpackdataptr->QosFlowInfo[i].is_TxQFlowGranted_Available)
        {
            fprintf (stderr, "\tRx Flow Granted\n");
            fprintf (stderr, "\t===============\n");
            _print_flow(unpackdataptr->QosFlowInfo[i].RxQFlowGranted);
        }
        for (j=0;j<unpackdataptr->QosFlowInfo[i].NumTxFilters;j++)
        {
            fprintf (stderr, "\tTx Filter Number: %d\n", (j+1));
            fprintf (stderr, "\t====================\n");
            _print_filter(unpackdataptr->QosFlowInfo[i].TxQFilter[j]);
        }
        for (j=0;j<unpackdataptr->QosFlowInfo[i].NumRxFilters;j++)
        {
            fprintf (stderr, "\tRx Filter Number: %d\n", (j+1));
            fprintf (stderr, "\t====================\n");
            _print_filter(unpackdataptr->QosFlowInfo[i].RxQFilter[j]);
        }
    }
}

void dump_qos_SLQSQosGetNetworkStatus(void* ptr)
{
    unpack_qos_SLQSQosGetNetworkStatus_t *unpackdataptr = NULL;
    unpackdataptr = (unpack_qos_SLQSQosGetNetworkStatus_t*) ptr;
    if(unpackdataptr==NULL)
    {
        return ;
    }
    if(swi_uint256_get_bit(unpackdataptr->ParamPresenceMask, 1))
    {
        printf("unpack_qos_SLQSQosGetNetworkStatus NW Status: %d\n", unpackdataptr->NWQoSStatus );
    }
}

void dump_unpack_qos_BindDataPort(void* ptr)
{
    unpack_qos_BindDataPort_t
        *unpackdataptr = NULL;
    unpackdataptr = (unpack_qos_BindDataPort_t*) ptr;
    if(unpackdataptr==NULL)
        return ;
}

void dump_unpack_qos_SLQSSetQosNWStatusCallback_ind(void* ptr)
{
    unpack_qos_SLQSSetQosNWStatusCallback_ind_t *unpackdataptr = NULL;
    unpackdataptr = (unpack_qos_SLQSSetQosNWStatusCallback_ind_t*) ptr;
    if(unpackdataptr==NULL)
    {
        return ;
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 1))
    {
        if(unpackdataptr->status<2)
        {   
            printf("NW Status: %d\n", unpackdataptr->status);
        }
        else
        {
            printf("invalid NW Status: %d\n", unpackdataptr->status);
        }
    }
}

void dump_qos_SLQSQosSwiReadApnExtraParams(void* ptr)
{
    unpack_qos_SLQSQosSwiReadApnExtraParams_t *unpackdataptr;
    unpackdataptr = (unpack_qos_SLQSQosSwiReadApnExtraParams_t*)ptr;
    if(unpackdataptr==NULL)
    {
        return ;
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 3))
    {
        printf("\tAPN ID       : %d\n", unpackdataptr->apnId);
        printf("\tambr_ul      : %d\n", unpackdataptr->ambr_ul);
        printf("\tambr_dl      : %d\n", unpackdataptr->ambr_dl);
        printf("\tambr_ul_ext  : %d\n", unpackdataptr->ambr_ul_ext);
        printf("\tambr_dl_ext  : %d\n", unpackdataptr->ambr_dl_ext);
        printf("\tambr_ul_ext2 : %d\n", unpackdataptr->ambr_ul_ext2);
        printf("\tambr_dl_ext2 : %d\n", unpackdataptr->ambr_dl_ext2);
    }    
}

void dump_unpack_qos_SLQSQosSwiReadDataStats(void* ptr)
{
    unpack_qos_SLQSQosSwiReadDataStats_t *unpackdataptr;
    uint32_t i = 0;
    unpackdataptr = (unpack_qos_SLQSQosSwiReadDataStats_t*)ptr;
    if(unpackdataptr==NULL)
    {
        return ;
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 3))
    {
        printf("\tAPN ID            : %d\n", unpackdataptr->apnId);
        printf("\ttotal_tx_pkt      : %d\n", unpackdataptr->total_tx_pkt);
        printf("\ttotal_tx_pkt_drp  : %d\n", unpackdataptr->total_tx_pkt_drp);
        printf("\ttotal_rx_pkt      : %d\n", unpackdataptr->total_rx_pkt);
        printf("\ttotal_tx_bytes    : %ju\n", unpackdataptr->total_tx_bytes);
        printf("\ttoal_tx_bytes_drp : %ju\n", unpackdataptr->total_tx_bytes_drp);
        printf("\ttotal_rx_bytes    : %ju\n", unpackdataptr->total_rx_bytes);
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 4))
    {
        printf("\tNumber of flows   : %d\n", unpackdataptr->numQosFlow);
        for (i=0;i<unpackdataptr->numQosFlow;i++)
        {
            printf("\t\tbearerId     : %u\n", unpackdataptr->qosFlow[i].bearerId);
            printf("\t\ttx_pkt       : %u\n", unpackdataptr->qosFlow[i].tx_pkt);
            printf("\t\ttx_pkt_drp   : %u\n", unpackdataptr->qosFlow[i].tx_pkt_drp);
            printf("\t\ttx_bytes     : %ju\n", unpackdataptr->qosFlow[i].tx_bytes);
            printf("\t\ttx_bytes_drp : %ju\n", unpackdataptr->qosFlow[i].tx_bytes_drp);
        }
    }
}

void dump_qos_SLQSSetQosStatusCallback_ind(void* ptr)
{
    unpack_qos_SLQSSetQosStatusCallback_ind_t *unpackdataptr;
    unpackdataptr = (unpack_qos_SLQSSetQosStatusCallback_ind_t*)ptr;
    if(unpackdataptr==NULL)
    {
        return ;
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 1))
    {
        printf("\tFlow ID    : %u\n", unpackdataptr->id);
        printf("\t");
        if((unpackdataptr->status<1)||(unpackdataptr->status>3))
        {
            printf("Invalid ");
        }
        printf("Flow status: %d\n", unpackdataptr->status);
        printf("\t");
        if((unpackdataptr->event<1)||(unpackdataptr->event>6))
        {
            printf("Invalid ");
        }
        printf("Flow event : %d\n", unpackdataptr->event);
    }
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 16))
    {
        printf("\t");
        if((unpackdataptr->reason<1)||(unpackdataptr->reason>0x16))
        {
            printf("Invalid ");
        }
        printf("Flow reason: %d\n", unpackdataptr->reason);
    }
    return ;
}

void dump_qos_SLQSSetQosPriEventCallback_ind(void* ptr)
{
    unpack_qos_SLQSSetQosPriEventCallback_ind_t *unpackdataptr;
    unpackdataptr = (unpack_qos_SLQSSetQosPriEventCallback_ind_t*)ptr;
    if(swi_uint256_get_bit (unpackdataptr->ParamPresenceMask, 1))
    {
        if((unpackdataptr->event<1) || (unpackdataptr->event>2))
        {
            printf("Invalid ");
        }
        printf("QoS primary flow events Event: %d\n", unpackdataptr->event );
    }
}

void *qos_read_thread(void* ptr)
{
    UNUSEDPARAM(ptr);
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    uint8_t  buffer[QMI_QOS_MSG_MAX]={'0'};
    int rtn;
    char *reason=NULL;
    ssize_t rlen;
    printf ("QoS read thread\n");
    //sleep(1);
    while(enQosThread)
    {
        //TODO select multiple file and read them
        memset (buffer,0,QMI_QOS_MSG_MAX);
        if(qos_fd<0)
            return NULL;
        rtn = read(qos_fd, buffer, QMI_QOS_MSG_MAX);
        if (rtn > 0)
        {
            rlen = (ssize_t) rtn;
            
            
            qmi_msg = helper_get_resp_ctx(eQOS, buffer, rlen, &rsp_ctx);
            printf("<< receiving %s, Len: %zu\n", qmi_msg, rlen);
            dump_hex(rlen, buffer);
            if (eIND == rsp_ctx.type)
                printf("QOS IND: ");
            else if (eRSP == rsp_ctx.type)
                printf("QOS RSP: ");
            printf("msgid 0x%02x\n", rsp_ctx.msgid);

            switch(rsp_ctx.msgid)
            {
                /** eQMI_QOS_SWI_READ_DATA_STATS **/
                case eQMI_QOS_SWI_READ_DATA_STATS:
                    {
                        uint8_t i =0;
                        unpack_qos_SLQSQosSwiReadDataStats_t QoSDataStats;
                        memset (&QoSDataStats,0,sizeof(QoSDataStats));
                        unpackRetCode = unpack_qos_SLQSQosSwiReadDataStats (buffer, rlen, &QoSDataStats);
                        printf("unpack_qos_SLQSQosSwiReadDataStats return: %d\n", unpackRetCode);
                        if(swi_uint256_get_bit (QoSDataStats.ParamPresenceMask, 3))
                        {
                            printf("\tAPN ID            : %d\n", QoSDataStats.apnId);
                            printf("\ttotal_tx_pkt      : %d\n", QoSDataStats.total_tx_pkt);
                            printf("\ttotal_tx_pkt_drp  : %d\n", QoSDataStats.total_tx_pkt_drp);
                            printf("\ttotal_rx_pkt      : %d\n", QoSDataStats.total_rx_pkt);
                            printf("\ttotal_tx_bytes    : %ju\n", QoSDataStats.total_tx_bytes);
                            printf("\ttoal_tx_bytes_drp : %ju\n", QoSDataStats.total_tx_bytes_drp);
                            printf("\ttotal_rx_bytes    : %ju\n", QoSDataStats.total_rx_bytes);
                            printf("\tNumber of flows   : %d\n", QoSDataStats.numQosFlow);
                        }
                        if(swi_uint256_get_bit (QoSDataStats.ParamPresenceMask, 4))
                        {
                            for (i=0;i<QoSDataStats.numQosFlow;i++)
                            {
                                printf("\t\tbearerId     : %u\n", QoSDataStats.qosFlow[i].bearerId);
                                printf("\t\ttx_pkt       : %u\n", QoSDataStats.qosFlow[i].tx_pkt);
                                printf("\t\ttx_pkt_drp   : %u\n", QoSDataStats.qosFlow[i].tx_pkt_drp);
                                printf("\t\ttx_bytes     : %ju\n", QoSDataStats.qosFlow[i].tx_bytes);
                                printf("\t\ttx_bytes_drp : %ju\n", QoSDataStats.qosFlow[i].tx_bytes_drp);                            
                            }
                        }
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSQosSwiReadDataStats",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        CHECK_WHITELIST_MASK(
                            unpack_qos_SLQSQosSwiReadDataStatsParamPresenceMaskWhiteList,
                            QoSDataStats.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_qos_SLQSQosSwiReadDataStatsParamPresenceMaskMandatoryList,
                            QoSDataStats.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((QoSDataStats.apnId > 0)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (QoSDataStats.apnId > 0) ? "Correct": "Wrong");
                        }
                    #endif
                    }
                    break; //QMI_QOS_SWI_READ_DATA_STATS
                    
                /** eQMI_QOS_SWI_READ_APN_PARAMS **/
                case eQMI_QOS_SWI_READ_APN_PARAMS:
                    {
                        unpack_qos_SLQSQosSwiReadApnExtraParams_t ExtraAPNParams;
                        memset (&ExtraAPNParams,0,sizeof(ExtraAPNParams));
                        unpackRetCode =  unpack_qos_SLQSQosSwiReadApnExtraParams (buffer, rlen, &ExtraAPNParams);
                        printf("unpack_qos_SLQSQosSwiReadApnExtraParams return: %d\n", unpackRetCode);
                        if(swi_uint256_get_bit (ExtraAPNParams.ParamPresenceMask, 3))
                        {
                            printf("\tAPN ID       : %d\n", ExtraAPNParams.apnId);
                            printf("\tambr_ul      : %d\n", ExtraAPNParams.ambr_ul);
                            printf("\tambr_dl      : %d\n", ExtraAPNParams.ambr_dl);
                            printf("\tambr_ul_ext  : %d\n", ExtraAPNParams.ambr_ul_ext);
                            printf("\tambr_dl_ext  : %d\n", ExtraAPNParams.ambr_dl_ext);
                            printf("\tambr_ul_ext2 : %d\n", ExtraAPNParams.ambr_ul_ext2);
                            printf("\tambr_dl_ext2 : %d\n", ExtraAPNParams.ambr_dl_ext2);
                        }
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSQosSwiReadApnExtraParams",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((ExtraAPNParams.apnId)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (ExtraAPNParams.apnId) ? "Correct": "Wrong");
                        }
                    #endif                        
                    }
                    break; //QMI_QOS_SWI_READ_APN_PARAMS                    
                
                /** eQMI_QOS_GET_FLOW_STATUS **/                
                case eQMI_QOS_GET_FLOW_STATUS:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_qos_SLQSSetQosStatusCallback_ind_t QoSFlowStatus;
                        memset (&QoSFlowStatus,0,sizeof(QoSFlowStatus));
                        unpackRetCode = unpack_qos_SLQSSetQosStatusCallback_ind (buffer, rlen, &QoSFlowStatus);
                        printf("unpack_qos_SLQSSetQosStatusCallback_ind return: %d\n", unpackRetCode);
                        if(swi_uint256_get_bit (QoSFlowStatus.ParamPresenceMask, 1))
                        {
                            printf("\tFlow ID    : %d\n", QoSFlowStatus.id);
                            printf("\tFlow status: %d\n", QoSFlowStatus.status);
                            printf("\tFlow event : %d\n", QoSFlowStatus.event);
                        }
                        if(swi_uint256_get_bit (QoSFlowStatus.ParamPresenceMask, 16))
                        {
                            printf("\tFlow reason: %d\n", QoSFlowStatus.reason);
                        }

                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSSetQosStatusCallback_ind",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((QoSFlowStatus.status <= 3)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (QoSFlowStatus.status <= 3) ? "Correct": "Wrong");
                        }
                    #endif

                        
                    }
                    else if (eRSP == rsp_ctx.type)
                    {
                        // Not Supported
                    }                    
                    break; //QMI_QOS_GET_FLOW_STATUS

                /** eQMI_QOS_PRIMARY_QOS_EVENT_IND **/
                case eQMI_QOS_PRIMARY_QOS_EVENT_IND:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_qos_SLQSSetQosPriEventCallback_ind_t QoSPriFlowEvent;
                        memset (&QoSPriFlowEvent,0,sizeof(QoSPriFlowEvent));
                        unpackRetCode = unpack_qos_SLQSSetQosPriEventCallback_ind ( buffer, rlen,&QoSPriFlowEvent);
                        printf("unpack_qos_SLQSSetQosPriEventCallback_ind return: %d", unpackRetCode);
                        if(swi_uint256_get_bit (QoSPriFlowEvent.ParamPresenceMask, 1))
                        {
                            printf(", Event: %d", QoSPriFlowEvent.event );
                        }
                        printf("\n");
                        swi_uint256_print_mask (QoSPriFlowEvent.ParamPresenceMask);
                    }
                    break; //QMI_QOS_PRIMARY_QOS_EVENT_IND

                /** eQMI_QOS_GET_NW_STATUS **/
                case eQMI_QOS_GET_NW_STATUS:
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_qos_SLQSSetQosNWStatusCallback_ind_t QoSNWStatusIndication;
                        memset (&QoSNWStatusIndication,0,sizeof(QoSNWStatusIndication));
                        unpackRetCode = unpack_qos_SLQSSetQosNWStatusCallback_ind (buffer, rlen, &QoSNWStatusIndication);
                        printf("unpack_qos_SLQSSetQosNWStatusCallback_ind return: %d", unpackRetCode);
                        if(swi_uint256_get_bit (QoSNWStatusIndication.ParamPresenceMask, 1))
                        {
                            printf(", NW Status: %d", QoSNWStatusIndication.status);
                        }
                        printf("\n");
                        swi_uint256_print_mask (QoSNWStatusIndication.ParamPresenceMask);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSSetQosNWStatusCallback_ind",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((QoSNWStatusIndication.status <=1)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (QoSNWStatusIndication.status <=1) ? "Correct": "Wrong");
                        }
                    #endif

                    }
                    else if (eRSP == rsp_ctx.type)
                    {                        
                        unpack_qos_SLQSQosGetNetworkStatus_t QoSNWStatusRsp;
                        memset (&QoSNWStatusRsp,0,sizeof(QoSNWStatusRsp));
                        unpackRetCode = unpack_qos_SLQSQosGetNetworkStatus ( buffer, rlen,&QoSNWStatusRsp);
                        printf("unpack_qos_SLQSQosGetNetworkStatus return: %d, NW Status: %d\n", unpackRetCode, QoSNWStatusRsp.NWQoSStatus );
                        swi_uint256_print_mask (QoSNWStatusRsp.ParamPresenceMask);
                    #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSQosGetNetworkStatus",\
                            ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((QoSNWStatusRsp.NWQoSStatus  <=1)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (QoSNWStatusRsp.NWQoSStatus  <=1) ? "Correct": "Wrong");
                        }
                    #endif


                    }                    

                    break; //QMI_QOS_GET_NW_STATUS
                    
                /** eQMI_QOS_SET_EVENT **/                
                case eQMI_QOS_SET_EVENT:
                    printf("eQMI_QOS_SET_EVENT \n");
                    if (eIND == rsp_ctx.type)
                    {
                        unpack_qos_SLQSSetQosEventCallback_ind_t QoSEventIndication;
                        #ifdef QOS_EVENTS_DEBUG
                        unpackRetCode = unpack_qos_SLQSSetQosEventCallback_ind  ( qos_event_sample, sizeof(qos_event_sample), &QoSEventIndication);
                        #else
                        unpackRetCode = unpack_qos_SLQSSetQosEventCallback_ind  ( buffer, rlen, &QoSEventIndication);
                        #endif //QOS_EVENTS_DEBUG
                        swi_uint256_print_mask(QoSEventIndication.ParamPresenceMask);
                        #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSSetQosEventCallback_ind",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        CHECK_WHITELIST_MASK(
                            unpack_qos_SLQSSetQosEventCallback_indParamPresenceMaskWhiteList,
                            QoSEventIndication.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,", ((QoSEventIndication.NumFlows > 01)  ? "Correct": "Wrong"));
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  (QoSEventIndication.NumFlows > 0) ? "Correct": "Wrong");
                        }
                        #endif

                        printf("unpack_qos_SLQSSetQosEventCallback_ind return: %d\n", unpackRetCode);
                        fprintf (stderr, "Num Flows: %d\n", QoSEventIndication.NumFlows);
                        if(swi_uint256_get_bit (QoSEventIndication.ParamPresenceMask, 16))
                        {
                            dump_qos_SLQSSetQosEventCallback_ind(&QoSEventIndication);
                        }                                               
                    }
                    else if (eRSP == rsp_ctx.type)
                    {                        
                        unpack_qos_SLQSSetQosEventCallback_t lunpack_qos_SLQSSetQosEventCallback;
                        unpackRetCode = unpack_qos_SLQSSetQosEventCallback( buffer, rlen, 
                                            &lunpack_qos_SLQSSetQosEventCallback);
                        printf("SLQSSetQosEventCallback return: %d\n", unpackRetCode);
                        swi_uint256_print_mask(lunpack_qos_SLQSSetQosEventCallback.ParamPresenceMask);
                        #if DEBUG_LOG_TO_FILE
                        local_fprintf("%s,%s,", "unpack_qos_SLQSSetQosEventCallback",\
                        ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                        CHECK_WHITELIST_MASK(
                            unpack_qos_SLQSSetQosEventCallbackParamPresenceMaskWhiteList,
                            lunpack_qos_SLQSSetQosEventCallback.ParamPresenceMask);
                        CHECK_MANDATORYLIST_MASK(
                            unpack_qos_SLQSSetQosEventCallbackParamPresenceMaskMandatoryList,
                            lunpack_qos_SLQSSetQosEventCallback.ParamPresenceMask);
                        if ( unpackRetCode != 0 )
                        {
                            rtn = helper_get_error_code(buffer);
                            reason = helper_get_error_reason(rtn);
                            sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                            local_fprintf("%s,",  "Correct");
                            local_fprintf("%s\n", remark);
                        }
                        else
                        {
                            local_fprintf("%s\n",  "Correct");    
                        }
                    #endif
                    }                    
                    break; //QMI_QOS_SET_EVENT
                case eQMI_QOS_BIND_DATA_PORT:
                    {
                    unpack_qos_BindDataPort_t lunpack_qos_BindDataPort = {SWI_UINT256_INT_VALUE};
                    unpackRetCode = unpack_qos_BindDataPort  ( buffer, rlen,
                                        &lunpack_qos_BindDataPort);
                    swi_uint256_print_mask(lunpack_qos_BindDataPort.ParamPresenceMask);
                #if DEBUG_LOG_TO_FILE
                    local_fprintf("%s,%s,", "unpack_qos_BindDataPort",\
                    ((unpackRetCode ==eQCWWAN_ERR_NONE) ? "Success": "Fail"));
                    if ( unpackRetCode != 0 )
                    {
                        rtn = helper_get_error_code(buffer);
                        reason = helper_get_error_reason(rtn);
                        sprintf((char*)remark, "Error code:%d reason:%s", rtn, reason);
                        local_fprintf("%s,",  "Correct");
                        local_fprintf("%s\n", remark);
                    }
                    else
                    {
                        local_fprintf("%s\n",  "Correct");            
                    }
                #endif

                    printf("BIND_DATA_PORT return: %d\n", unpackRetCode);
                    }
                    break;//eQMI_QOS_BIND_DATA_PORT
                default:
                    break;
            }
        }
        else //rlen > 0
        {
            enQosThread = 0;
            printf("Thread Exit \n");
            return NULL;
        }
    }    
    printf("Thread Exit\n");
    return NULL;
}


static uint16_t gMuxID = 0;
void QosSetMuxID(uint16_t MuxID)
{
    gMuxID = MuxID;
}

void qos_loop(void)
{
    uint8_t  buffer[QMI_QOS_MSG_MAX]={'0'};
    uint16_t reqLen = QMI_QOS_MSG_MAX;
    pack_qmi_t req_ctx;
    int rtn = -1;
    uint16_t xid = 1;
    const char *qmi_msg;
    ///////////////
    printf("\n======QOS dummy unpack and compare test===========\n");
    qos_validate_dummy_unpack();
    printf("\n======QOS unpack dummy test===========\n");
    qos_dummy_unpack();
    printf("\n=====================================\n");
    ///////////////
    // Get QoS FD
    if(qos_fd<0)
        qos_fd = client_fd(eQOS); 
    if(qos_fd<0)
    {
        fprintf(stderr,"Qos Service Not Supported!\n");
        return ;
    }

    printf("\n======QOS pack/unpack test===========\n");
#if DEBUG_LOG_TO_FILE
        mkdir("./TestResults/",0777);
        local_fprintf("\n");
        local_fprintf("%s,%s,%s,%s\n", "QoS Pack/UnPack API Name", "Status", "Unpack Payload Parsing", "Remark");
#endif

    sleep(1);
    // Start QoS read thread
    memset(&qos_attr, 0, sizeof(qos_attr));
    enQosThread = 1;
    pthread_create(&qos_tid, &qos_attr, qos_read_thread, NULL);
    sleep(1);
    //Bind QMAP
    if(gMuxID>=0x80)
    {
        pack_qos_BindDataPort_t BindDataPort_t;
        qos_BindDataPortMuxID_t DataPortMuxID;
        memset(&BindDataPort_t,0,sizeof(BindDataPort_t));
        memset(&DataPortMuxID,0,sizeof(DataPortMuxID));
        BindDataPort_t.pMuxID = &DataPortMuxID;
        DataPortMuxID.MuxID = gMuxID;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid++;
        rtn = pack_qos_BindDataPort(&req_ctx,buffer,&reqLen,BindDataPort_t);    
#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_qos_BindDataPort", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(qos_fd, buffer, reqLen);
        if(rtn<0)
        {
            enQosThread = 0;
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eQOS, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }

        fprintf (stderr, "BindDataPort Write : %d\n", rtn);
        sleep(2);
    }
    // Register for QoS event notification
    
    pack_qos_SLQSSetQosEventCallback_t param;
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = xid++;
    param.enable = 1;
    reqLen = QMI_QOS_MSG_MAX;
    rtn = pack_qos_SLQSSetQosEventCallback (&req_ctx,buffer,&reqLen,param);
    fprintf (stderr, "pack_qos_SLQSSetQosEventCallback ret: %d, Len: %d\n", rtn, reqLen);
#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_qos_SLQSSetQosEventCallback", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    rtn = write(qos_fd, buffer, reqLen);

    fprintf (stderr, "Write : %d\n", rtn);
    sleep(2);
    if(rtn<0)
    {
        enQosThread = 0;
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eQOS, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    // SLQSSetQosPriEventCallback does not need to be registered
    // SLQSSetQosStatusCallback does not need to be registered
    // SLQSSetQosNWStatusCallback does not need to be registered

    // Get Network status
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = xid++;
    reqLen = QMI_QOS_MSG_MAX;
    memset(buffer, 0, reqLen);
    rtn = pack_qos_SLQSQosGetNetworkStatus (&req_ctx,buffer,&reqLen);
    fprintf (stderr, "pack_qos_SLQSQosGetNetworkStatus ret: %d, Len: %d\n", rtn, reqLen);

#if DEBUG_LOG_TO_FILE
    local_fprintf("%s,%s,%s\n", "pack_qos_SLQSQosGetNetworkStatus", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

    rtn = write(qos_fd, buffer, reqLen);

    if(rtn<0)
    {
        enQosThread = 0;
        return ;
    }
    else
    {
        qmi_msg = helper_get_req_str(eQOS, buffer, reqLen);
        printf(">> sending %s\n", qmi_msg);
        dump_hex(reqLen, buffer);
    }
    sleep(2);

    if(g_runoem_demo==1)
    {
        // Read extra APN params
        pack_qos_SLQSQosSwiReadApnExtraParams_t ExtraAPNParams;
        ExtraAPNParams.apnId = 3;
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid = xid++;
        reqLen = QMI_QOS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_qos_SLQSQosSwiReadApnExtraParams (&req_ctx,buffer,&reqLen,ExtraAPNParams);
        fprintf (stderr, "pack_qos_SLQSQosSwiReadApnExtraParams ret: %d, Len: %d\n", rtn, reqLen);

#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_qos_SLQSQosSwiReadApnExtraParams", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(qos_fd, buffer, reqLen);

        if(rtn<0)
        {
            enQosThread = 0;
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eQOS, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        sleep(2);
    }
    // Read Data stats
    pack_qos_SLQSQosSwiReadDataStats_t DataStats;
    DataStats.apnId = 3;
    int i;
    for (i=100;i<110;i++)
    {
        memset(&req_ctx, 0, sizeof(req_ctx));
        req_ctx.xid += xid++;
        reqLen = QMI_QOS_MSG_MAX;
        memset(buffer, 0, reqLen);
        rtn = pack_qos_SLQSQosSwiReadDataStats (&req_ctx,buffer,&reqLen,DataStats);
        fprintf (stderr, "pack_qos_SLQSQosSwiReadDataStats ret: %d, Len: %d\n", rtn, reqLen);

#if DEBUG_LOG_TO_FILE
        local_fprintf("%s,%s,%s\n", "pack_qos_SLQSQosSwiReadDataStats", (rtn==eQCWWAN_ERR_NONE ? "Success": "Fail"), "N/A");
#endif

        rtn = write(qos_fd, buffer, reqLen);

        fprintf (stderr, "Write : %d\n", rtn);
        if(rtn<0)
        {
            enQosThread = 0;
            return ;
        }
        else
        {
            qmi_msg = helper_get_req_str(eQOS, buffer, reqLen);
            printf(">> sending %s\n", qmi_msg);
            dump_hex(reqLen, buffer);
        }
        sleep(5);
    }
    if(gMuxID>0)
    {
        while(enQosThread)
        {
            sleep(1);
        }
    }
    fprintf (stderr, "exit\n");
    return;
}

uint8_t dummy_qos_resp_msg[][QMI_MSG_MAX] ={
    //eQMI_QOS_BIND_DATA_PORT
    {0x02,0x01,0x00,0x2b,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
    //eQMI_QOS_SET_EVENT RSP
    {0x02,0x02,0x00,0x01,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},
    //eQMI_QOS_SET_EVENT IND
    {0x04,0x02,0x00,0x01,0x00,
        3 + 0x55,0x01,
        0x10,0x55,0x01,
    0x10,0x06,0x00,0x50,0x94,0xDC,0x47,0x01,0x01,
    0x14,0x95,0x00,
      0x10,0x92,0x00,
        0x23,0x02,0x00,0x01,0x02,
        0x22,0x02,0x00,0x21,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
        0x16,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
    0x13,0x95,0x00,
      0x10,0x92,0x00,
        0x23,0x02,0x00,0x00,0x02,
        0x22,0x02,0x00,0x20,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x16,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00, 0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,

    0x12,0x07,0x00,
      0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

    0x11,0x07,0x00,
      0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

    0x15 ,0x01,0x00,0x01,
    /* bearer id */
    0x16, 0x01,0x00, 0xAB,
    },
    {
    0x04,0x02,0x00,0x01,0x00,
        3 + 0x07+22+22+22,0x01,        
    0x10,0x07+22+22+22,0x01,
    0x10,0x06,0x00,0xB0,0x56,0xD1,0x43,0x01,0x01,
      0x14,0x16+22+22,0x00,
        0x10,0x13,0x00,
          0x23,0x02,0x00,0x01,0x00,
          0x22,0x02,0x00,0x11,0x00,
          0x15,0x02,0x00,0xA0,0xE0,
          0x11,0x01,0x00,0x04,
        0x10,0x13,0x00,
          0x23,0x02,0x00,0x04,0x00,
          0x22,0x02,0x00,0x04,0x00,
          0x15,0x02,0x00,0x04,0x00,
          0x11,0x01,0x00,0x06,
        0x10,0x13,0x00,
          0x23,0x02,0x00,0x08,0x00,
          0x22,0x02,0x00,0x08,0x00,
          0x15,0x02,0x00,0x08,0x00,
          0x11,0x01,0x00,0x08,
    0x13,0x16+22,0x00,
      0x10,0x13,0x00,
        0x23,0x02,0x00,0x00,0x00,
        0x22,0x02,0x00,0x10,0x00,
        0x15,0x02,0x00,0xA0,0xE0,
        0x11,0x01,0x00,0x04,
      0x10,0x13,0x00,
        0x23,0x02,0x00,0x5,0x00,
        0x22,0x02,0x00,0x5,0x00,
        0x15,0x02,0x00,0x5,0x5,
        0x11,0x01,0x00,0x05,
    0x12,0x61,0x00,
      0x10,0x5E,0x00,
        0x12,0x08,0x00,0x80,0x91,0x07,0x00,0x80,0x91,0x07,0x00,
        0x1F,0x01,0x00,0x01,
        0x18,0x04,0x00,0x01,0x01,0x01,0x01,
        0x1B,0x02,0x00,0x1B,0x1B,
        0x11,0x01,0x00,0x11,
        0x13,0x0C,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,
        0x14,0x04,0x00,0x01,0x02,0x03,0x04,
        0x15,0x04,0x00,0x01,0x01,0x01,0x01,
        0x16,0x04,0x00,0x01,0x01,0x01,0x01,
        0x17,0x04,0x00,0x01,0x01,0x01,0x01,
        0x19,0x01,0x00,0x11,
        0x1A,0x01,0x00,0x11,
        0x1C,0x01,0x00,0x11,
        0x1D,0x01,0x00,0x11,
        0x1E,0x01,0x00,0x11,
    0x11,0x61,0x00,
      0x10,0x5E,0x00,
        0x12,0x08,0x00,0x80 ,0x91,0x07,0x00,0x80,0x91,0x07,0x00,
        0x1F,0x01,0x00,0x01,
        0x18,0x04,0x00,0x01,0x01,0x01,0x01,
        0x1B,0x02,0x00,0x1B,0x1B,
        0x11,0x01,0x00,0x11,
        0x13,0x0C,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,0x11,0x11,0x01,0x00,
        0x14,0x04,0x00,0x01,0x02,0x03,0x04,
        0x15,0x04,0x00,0x01,0x01,0x01,0x01,
        0x16,0x04,0x00,0x01,0x01,0x01,0x01,
        0x17,0x04,0x00,0x01,0x01,0x01,0x01,
        0x19,0x01,0x00,0x11,
        0x1A,0x01,0x00,0x11,
        0x1C,0x01,0x00,0x11,
        0x1D,0x01,0x00,0x11,
        0x1E,0x01,0x00,0x11,
    0x15,0x01,0x00,0x01
    },
    //eQMI_QOS_GET_NW_STATUS
    {0x02,0x03,0x00,0x27,0x00,0x0b,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00},
    //eQMI_QOS_GET_NW_STATUS IND
    {0x04,0x03,0x00,0x27,0x00,
        0x04,0x00,
            0x01,0x01,0x00,0x01},
    //eQMI_QOS_GET_NW_STATUS IND
    {0x04,0x03,0x00,0x27,0x00,
        0x04,0x00,
            0x01,0x01,0x00,0x02},
    //eQMI_QOS_GET_NW_STATUS IND invalid
    {0x04,0x03,0x00,0x27,0x00,
        0x04,0x00,
            0x01,0x01,0x00,0x03},
    //eQMI_QOS_SWI_READ_APN_PARAMS
    {0x02,0x04,0x00,0x57,0x55,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x18,0x00},
    //eQMI_QOS_SWI_READ_APN_PARAMS
    {0x02,0x04,0x00,0x57,0x55,
        (3 + 4) + ( 3 + 0x0a),0x00,
            0x02,0x04,0x00,0x00,0x00,0x00,0x00,
            0x03,0x0a,0x00,0x2e,0x3d,0x4c,0x5b,
                0x5a,0xa5,
                0xa5,0x5a,
                0x5a,0xa5,
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x51,0x01,//0x07+(3 + 40) +(3 +4 +28*10)
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x03,0x28,0x00,
            0x0F,0xF0,0x0F,0x0F,
            0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,
            0xA5,0x5A,0xA5,0x5A,
            0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,
            0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,
        0x04,0x1c,0x01, //0x04 + 28*10
            0x0a,0x00,0x00,0x00,
                0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
                0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x2a,0x2b,0x2c,
                0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,
                0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
                0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
                0x2d,0x2e,0x2f,0x20,0x21,0x22,0x23,0x24,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
                0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
                0x3d,0x3e,0x3f,0x30,0x31,0x32,0x33,0x34,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
                0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
                0x4d,0x4e,0x4f,0x40,0x41,0x42,0x43,0x44,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,
                0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,
                0x5d,0x5e,0x5f,0x50,0x51,0x52,0x53,0x54,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
                0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
                0x6d,0x6e,0x6f,0x60,0x61,0x62,0x63,0x64,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,
                0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,
                0x7d,0x7e,0x7f,0x70,0x71,0x72,0x73,0x74,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,
                0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,
                0x8d,0x8e,0x8f,0x80,0x81,0x82,0x83,0x84,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,
                0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,
                0x9d,0x9e,0x9f,0x90,0x91,0x92,0x93,0x94,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x05,0x00,0x56,0x55,0x07,0x00,0x02,0x04,0x00,0x01,0x00,0x0a,0x00},
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x07+3 + 40 ,0x00,
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x03,0x28,0x00,
            0x0F,0xF0,0x0F,0x0F,
            0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,
            0xA5,0x5A,0xA5,0x5A,
            0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,
            0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x07+3 + 40 +3 +4,0x00,
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x03,0x28,0x00,
            0x0F,0xF0,0x0F,0x0F,
            0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,
            0xA5,0x5A,0xA5,0x5A,
            0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,
            0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,
        0x04,0x04,0x00,
            0x00,0x00,0x00,0x00,       
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x07 +3 +4,0x00,
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x04,0x04,0x00,
            0x00,0x00,0x00,0x00,       
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x07+(3 + 40) +(3 +4 +28),0x00,
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x03,0x28,0x00,
            0x0F,0xF0,0x0F,0x0F,
            0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,
            0xA5,0x5A,0xA5,0x5A,
            0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,
            0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,
        0x04,0x04 + 28,0x00,
            0x01,0x00,0x00,0x00,
                0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
                0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,        
    },
    //eQMI_QOS_SWI_READ_DATA_STATS
    {0x02,0x0e,0x00,0x56,0x55,
        0x51,0x01,//0x07+(3 + 40) +(3 +4 +28*10)
        0x02,0x04,0x00,
            0x00,0x00,0x00,0x00,
        0x03,0x28,0x00,
            0x0F,0xF0,0x0F,0x0F,
            0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,
            0xA5,0x5A,0xA5,0x5A,
            0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,
            0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,
            0xF0,0x0F,0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,
        0x04,0x1c,0x01, //0x04 + 28*10
            0x0a,0x00,0x00,0x00,
                0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
                0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x2a,0x2b,0x2c,
                0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,
                0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
                0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
                0x2d,0x2e,0x2f,0x20,0x21,0x22,0x23,0x24,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
                0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
                0x3d,0x3e,0x3f,0x30,0x31,0x32,0x33,0x34,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
                0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
                0x4d,0x4e,0x4f,0x40,0x41,0x42,0x43,0x44,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,
                0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,
                0x5d,0x5e,0x5f,0x50,0x51,0x52,0x53,0x54,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
                0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
                0x6d,0x6e,0x6f,0x60,0x61,0x62,0x63,0x64,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,
                0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,
                0x7d,0x7e,0x7f,0x70,0x71,0x72,0x73,0x74,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,
                0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,
                0x8d,0x8e,0x8f,0x80,0x81,0x82,0x83,0x84,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,
                0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,
                0x9d,0x9e,0x9f,0x90,0x91,0x92,0x93,0x94,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,
    },
    //eQMI_QOS_GET_FLOW_STATUS
    {0x04,0x02,0x00,0x26,0x00,
        (3+6),0x00,
        0x01,0x06,0x00,
            0x01,0x00,0x00,0x00, 0x03, 0x04,

    },
    //eQMI_QOS_GET_FLOW_STATUS
    {0x04,0x02,0x00,0x26,0x00,
        (3+6) + (3+1) ,0x00,
        0x01,0x06,0x00,
            0x12,0x34,0x56,0x78, 0x02, 0x05,
        0x10,0x01,0x00,
            0x0F,
    },
    //eQMI_QOS_GET_FLOW_STATUS
    {0x04,0x02,0x00,0x26,0x00,
        (3+1) ,0x00,
        0x10,0x01,0x00,
            0x07,
    },
    //eQMI_QOS_GET_FLOW_STATUS Invalid value
    {0x04,0x02,0x00,0x26,0x00,
        (3+6) + (3+1) ,0x00,
        0x01,0x06,0x00,
            0x55,0xAA,0x55,0xAA, 0x5A, 0xA5,
        0x10,0x01,0x00,
            0xAF,
    },
    //eQMI_QOS_PRIMARY_QOS_EVENT_IND
    {0x04,0x02,0x00,0x29,0x00,
        (3+2) ,0x00,
        0x01,0x02,0x00,
            0x01,0x00,
    },
    //eQMI_QOS_PRIMARY_QOS_EVENT_IND Invalid value
    {0x04,0x02,0x00,0x29,0x00,
        (3+2) ,0x00,
        0x01,0x02,0x00,
            0x0F,0x00,
    },
    
};
void qos_dummy_unpack()
{
    msgbuf msg;
    const char *qmi_msg;
    unpack_qmi_t rsp_ctx;
    int rtn;
    ssize_t rlen;
    pack_qmi_t req_ctx;
    int loopCount = 0;
    int index = 0;
    memset(&req_ctx, 0, sizeof(req_ctx));
    loopCount = sizeof(dummy_qos_resp_msg)/sizeof(dummy_qos_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index>=loopCount)
            return ;
        //TODO select multiple file and read them
        memcpy(&msg.buf,&dummy_qos_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQOS, msg.buf, rlen, &rsp_ctx);
            printf("<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            if (rsp_ctx.type == eIND)
                printf("QOS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("QOS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_QOS_BIND_DATA_PORT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_qos_BindDataPort,
                        dump_unpack_qos_BindDataPort,                        
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_QOS_GET_NW_STATUS:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                            unpack_qos_SLQSSetQosNWStatusCallback_ind,
                            dump_unpack_qos_SLQSSetQosNWStatusCallback_ind,
                            msg.buf,
                            rlen);
                }
                else if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSQosGetNetworkStatus,
                        dump_qos_SLQSQosGetNetworkStatus,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_QOS_SWI_READ_APN_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSQosSwiReadApnExtraParams,
                        dump_qos_SLQSQosSwiReadApnExtraParams,
                        msg.buf,
                        rlen);
                }                
                break;
            case eQMI_QOS_SWI_READ_DATA_STATS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSQosSwiReadDataStats,
                        dump_unpack_qos_SLQSQosSwiReadDataStats,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_QOS_SET_EVENT:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSSetQosEventCallback_ind,
                        dump_qos_SLQSSetQosEventCallback_ind,
                        msg.buf,
                        rlen);
                }
                else if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSSetQosEventCallback,
                        dump_qos_SLQSSetQosEventCallback,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_QOS_GET_FLOW_STATUS:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSSetQosStatusCallback_ind,
                        dump_qos_SLQSSetQosStatusCallback_ind,
                        msg.buf,
                        rlen);
                }
                break;
            case eQMI_QOS_PRIMARY_QOS_EVENT_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMP(unpackRetCode,
                        unpack_qos_SLQSSetQosPriEventCallback_ind,
                        dump_qos_SLQSSetQosPriEventCallback_ind,
                        msg.buf,
                        rlen);
                }
                break;
            default:
                fprintf(stderr,"======NO unpack handler %s\n", qmi_msg);
                break;
            }
        }
    }
    return ;
}


/*****************************************************************************
 * Validate unpacking by comparing dummy response with constant unpack variable
 ******************************************************************************/
 uint8_t validate_qos_resp_msg[][QMI_MSG_MAX] ={
    /* eQMI_QOS_NETWORK_STATUS_IND */
    {0x04,0x01,0x00,0x27,0x00,0x04,0x00,0x01,0x01,0x00,0x01},

    /* eQMI_QOS_FLOW_STATUS_IND */
    {0x04,0x02,0x00,0x26,0x00,0x0D,0x00,0x01,0x06,0x00,0x01,0x00,0x00,0x00,0x03,0x04,0x10,0x01,0x00,0x01},

    /* eQMI_QOS_PRIMARY_QOS_EVENT_IND */
    {0x04,0x03,0x00,0x29,0x00,0x05,0x00,0x01,0x02,0x00,0x01,0x00,},

    /* eQMI_QOS_EVENT_IND */
    {0x04,0x04,0x00,0x01,0x00,
        0x58,0x01,
        0x10,0x55,0x01,
     0x10,0x06,0x00,0x50,0x94,0xDC,0x47,0x01,0x01,
     0x14,0x95,0x00,
      0x10,0x92,0x00,
        0x23,0x02,0x00,0x01,0x02,
        0x22,0x02,0x00,0x21,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
     0x16,0x11,0x00,
       0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00,
       0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
     0x13,0x95,0x00,
      0x10,0x92,0x00,
        0x23,0x02,0x00,0x00,0x02,
        0x22,0x02,0x00,0x20,0x00,
        0x15,0x02,0x00,0x58,0x78,
        0x11,0x01,0x00,0x00,
        0x12,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x13,0x08,0x00, 0xde,0xad,0xbe,0xef, 0xff,0xff,0xff,0xff,
        0x14,0x01,0x00, 0x01,
        0x16,0x11,0x00,
     0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x17,0x11,0x00,
     0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xff,
        0x19,0x02,0x00, 0x01,0x01,
        0x1A,0x04,0x00, 0x21,0x31,0x01,0x01,
        0x1B,0x04,0x00, 0x01,0x01,0x41,0x01,
        0x1C,0x04,0x00, 0x91,0x01,0x31,0xf1,
        0x1D,0x04,0x00, 0x01,0x54,0x01,0xe1,
        0x1E,0x04,0x00, 0x71,0x01,0xe1,0x01,
        0x21,0x04,0x00, 0x01,0x78,0xc1,0x01,
        0x24,0x04,0x00, 0x51,0x82,0x01,0xd1,
        0x25,0x04,0x00, 0x01,0x91,0xab,0x01,

    0x12,0x07,0x00,
      0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

    0x11,0x07,0x00,
      0x10,0x04,0x00,
        0x1F,0x01,0x00,0x07,

    0x15 ,0x01,0x00,0x01,
    /* bearer id */
    0x16, 0x01,0x00, 0xAB,
    },

    /* eQMI_QOS_SET_EVENT */
    {0x02,0x05,0x00,0x01,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_QOS_BIND_DATA_PORT */
    {0x02,0x06,0x00,0x2b,0x00,0x07,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},

    /* eQMI_QOS_GET_NW_STATUS */
    {0x02,0x07,0x00,0x27,0x00,0x0b,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x01},

    /* eQMI_QOS_SWI_READ_APN_PARAMS*/
    {0x02,0x08,0x00,0x57,0x55,0x14,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x03,0x0a,0x00,0x2e,0x3d,
     0x4c,0x5b,0x5a,0xa5,0xa5,0x5a,0x5a,0xa5},

    /* eQMI_QOS_SWI_READ_DATA_STATS */
    {0x02,0x09,0x00,0x56,0x55,0x8D,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x03,0x28,0x00,
     0x0F,0xF0,0x0F,0x0F,0x5A,0xA5,0x5A,0xA5,0xF0,0x0F,0xF0,0x0F,0xA5,0x5A,0xA5,0x5A,0xA5,
     0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0x5A,0xA5,0xF0,0x0F,
     0xF0,0x0F,0x0F,0xF0,0x0F,0x0F,0x04,0x58,0x00,0x03,0x00,0x00,0x00,
     0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
     0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x2a,0x2b,0x2c,
     0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,
     0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
     0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,
     0x2d,0x2e,0x2f,0x20,0x21,0x22,0x23,0x24,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,
     },
   };

/* eQMI_QOS_NETWORK_STATUS_IND */
const unpack_qos_SLQSSetQosNWStatusCallback_ind_t const_unpack_qos_SLQSSetQosNWStatusCallback_ind_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_QOS_FLOW_STATUS_IND */
const unpack_qos_SLQSSetQosStatusCallback_ind_t const_unpack_qos_SLQSSetQosStatusCallback_ind_t = {
        1,3,4,1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,16)}} };

/* eQMI_QOS_EVENT_IND */
const unpack_qos_SLQSSetQosEventCallback_ind_t const_unpack_qos_SLQSSetQosEventCallback_ind_t = {
        1,
        {{{0x47DC9450,1,1},
        1,{0,0,0,0,0,0,0,0,{0,0},0,{0,0,0},0,0,0,0,0,{0,0},0,0,0,0,0,0,0,0,0,0,0,0,1,7},
        1,{0,0,0,0,0,0,0,0,{0,0},0,{0,0,0},0,0,0,0,0,{0,0},0,0,0,0,0,0,0,0,0,0,0,0,1,7},
        1,{{0,0,1,{4022250974LL,4294967295LL},1,{4022250974LL,4294967295LL},1,1,1,{88,120},
          1,{{0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff},255},
          1,{{0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff},255},
          1,{1,1},
          1,16855329,1,{257,321},1,{401,61745},1,{21505,57601},1,{369,481},1,29456385,1,32,1,512,
          1,{33361,53505},1,{37121,427}}},
        1,{{0,0,1,{4022250974LL,4294967295LL},1,{4022250974LL,4294967295LL},1,1,1,{88,120},
          1,{{0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff},255},
          1,{{0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff,0xde,0xad,0xbe,0xef,0xff,0xff,0xff,0xff},255},
          1,{1,1},
          1,16855329,1,{257,321},1,{401,61745},1,{21505,57601},1,{369,481},1,29456385,1,33,1,513,
          1,{33361,53505},1,{37121,427}}},171}},
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,16)}} };

/* eQMI_QOS_SET_EVENT */
const unpack_qos_SLQSSetQosEventCallback_t const_unpack_qos_SLQSSetQosEventCallback_t = {
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

/* eQMI_QOS_PRIMARY_QOS_EVENT_IND */
const unpack_qos_SLQSSetQosPriEventCallback_ind_t const_unpack_qos_SLQSSetQosPriEventCallback_ind_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_1_BITS,1)}} };

/* eQMI_QOS_GET_NW_STATUS */
const unpack_qos_SLQSQosGetNetworkStatus_t const_unpack_qos_SLQSQosGetNetworkStatus_t = {
        1,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,1,2)}} };

/* eQMI_QOS_SWI_READ_APN_PARAMS */
const unpack_qos_SLQSQosSwiReadApnExtraParams_t const_unpack_qos_SLQSQosSwiReadApnExtraParams_t = {
        0x5b4c3d2e,0x5a,0xa5,0xa5,0x5a,0x5a,0xa5,{{SWI_UINT256_BIT_VALUE(SET_2_BITS,2,3)}} };

/* eQMI_QOS_SWI_READ_DATA_STATS */
const unpack_qos_SLQSQosSwiReadDataStats_t const_unpack_qos_SLQSQosSwiReadDataStats_t = {
        0x0f0ff00f,0xa55aa55a,0x0ff00ff0,0x5aa55aa5,
        0x5aa55aa55aa55aa5ull,0xa55aa55aa55aa55aull,0x0f0ff00f0ff00ff0ull,3,
        {{0x04030201,0x08070605,0x0c0b0a09,0x14131211100f0e0dull,0x2c2b2a1918171615ull},
        {0x14131211,0x18171615,0x1c1b1a19,0x24232221201f1e1dull,0x2c2b2a2928272625ull},
        {0x24232221,0x28272625,0x2c2b2a29,0x24232221202f2e2dull,0x3c3b3a3938373635ull}},
        {{SWI_UINT256_BIT_VALUE(SET_3_BITS,2,3,4)}} };

/* eQMI_QOS_BIND_DATA_PORT */
const unpack_qos_BindDataPort_t const_unpack_qos_BindDataPort_t = {
        {{SWI_UINT256_BIT_VALUE(SET_1_BITS,2)}} };

int qos_validate_dummy_unpack()
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
    loopCount = sizeof(validate_qos_resp_msg)/sizeof(validate_qos_resp_msg[0]);
    while(loopCount)
    {
        memset(&msg, 0, sizeof(msg));
        if(index >= loopCount)
            return 0;
        memcpy(&msg.buf,&validate_qos_resp_msg[index++],QMI_MSG_MAX);
        rtn = msg.buf[5] + msg.buf[6]*256 + 7;
        fflush(stdout);
        if (rtn > 0)
        {
            rlen = (ssize_t)rtn;
            qmi_msg = helper_get_resp_ctx(eQMI_SVC_QOS, msg.buf, rlen, &rsp_ctx);
            printf("\n<< receiving %s\n", qmi_msg);
            dump_hex(rlen, msg.buf);
            fflush(stdout);
            if (rsp_ctx.type == eIND)
                printf("QOS IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("QOS RSP: ");
            printf("msgid 0x%x\n", rsp_ctx.msgid);
            switch(rsp_ctx.msgid)
            {
            case eQMI_QOS_SET_EVENT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSSetQosEventCallback,
                    dump_qos_SLQSSetQosEventCallback,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSSetQosEventCallback_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSSetQosEventCallback_ind,
                    dump_qos_SLQSSetQosEventCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSSetQosEventCallback_ind_t);
                }
                break;
            case eQMI_QOS_GET_NW_STATUS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSQosGetNetworkStatus,
                    dump_qos_SLQSQosGetNetworkStatus,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSQosGetNetworkStatus_t);
                }
                else if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSSetQosNWStatusCallback_ind,
                    dump_unpack_qos_SLQSSetQosNWStatusCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSSetQosNWStatusCallback_ind_t);
                }
                break;
            case eQMI_QOS_FLOW_STATUS_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSSetQosStatusCallback_ind,
                    dump_qos_SLQSSetQosStatusCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSSetQosStatusCallback_ind_t);
                }
                break;
            case eQMI_QOS_PRIMARY_QOS_EVENT_IND:
                if (eIND == rsp_ctx.type)
                {
                    UNPACK_STATIC_IND_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSSetQosPriEventCallback_ind,
                    dump_qos_SLQSSetQosPriEventCallback_ind,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSSetQosPriEventCallback_ind_t);
                }
                break;
            case eQMI_QOS_SWI_READ_APN_PARAMS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSQosSwiReadApnExtraParams,
                    dump_qos_SLQSQosSwiReadApnExtraParams,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSQosSwiReadApnExtraParams_t);
                }
                break;
            case eQMI_QOS_SWI_READ_DATA_STATS:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_SLQSQosSwiReadDataStats,
                    dump_unpack_qos_SLQSQosSwiReadDataStats,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_SLQSQosSwiReadDataStats_t);
                }
                break;
            case eQMI_QOS_BIND_DATA_PORT:
                if (eRSP == rsp_ctx.type)
                {
                    UNPACK_STATIC_RESP_AND_DUMPCOMP(unpackRetCode,
                    unpack_qos_BindDataPort,
                    dump_unpack_qos_BindDataPort,
                    msg.buf,
                    rlen,
                    &const_unpack_qos_BindDataPort_t);
                }
                break;
            }
        }
    }
    return 0;
}


