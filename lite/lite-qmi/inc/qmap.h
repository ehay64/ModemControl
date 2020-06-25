#ifndef _QMAP_H_
#define _QMAP_H_
#pragma once

#define QMI_GET_QMAP_SUPPORT         0x8BE0 + 15
#define QMI_SET_QMAP_IP_TABLE        0x8BE0 + 16
#define QMI_SET_QMAP_IPv6_TABLE      0x8BE0 + 17

/**
 * Qmux IP Table Structure, the mux id is associated with the ip address
 *
 *  \param  muxID [IN]
 *          - mux ID of the PDN
 *
 *  \param  ipAddress [IN]
 *          - ip address of the PDN
 */
struct QmuxIPTable
{
    unsigned char muxID;
    unsigned int ipAddress;
};

#define IPV6_ADDR_LEN 16
/**
 * Qmux IPv6 Table Structure, the mux id is associated with the ipv6 address
 *
 *  \param  muxID
 *          - mux ID of the PDN
 *
 *  \param  ipv6addr
 *          - IPv6 address of the PDN
 *
 *  \param  prefix_len
 *          - IPv6 Prefix Length
 */
typedef struct
{
    unsigned char muxID;
    unsigned char ipv6addr[IPV6_ADDR_LEN];
    unsigned char prefix_len;
}QmuxIPv6Table;

#endif
