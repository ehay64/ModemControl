#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <sys/time.h>

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  3
#define GOOGLE_IP_ADDR "8.8.8.8"

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
int sockfd, datalen = 56;
int nsend = 0, nreceived = 0;
struct sockaddr_in dest_addr;
struct sockaddr_in6 dest_ipv6_addr;
pid_t pid;
struct sockaddr_in from;
struct sockaddr_in6 from6;
struct timeval tvrecv;
void statistics(int signo);
unsigned short cal_chksum(unsigned short *addr, int len);
int pack(int pack_no);
void send_packet(void);
void recv_packet(void);
int unpack(char *buf, int len);
void tv_sub(struct timeval *out, struct timeval *in);

void statistics(int signo)
{
    printf("\n--------------------PING statistics-------------------\n");
    printf("%d packets transmitted, %d received , %%%d lost,signo %d\n", nsend,
        nreceived, (nsend - nreceived) / nsend * 100, signo);
    close(sockfd);
    return ;
}

unsigned short cal_chksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum +=  *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;

    }
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;

}

int pack(int pack_no)
{
    int packsize;
    struct icmp *icmp;
    struct timeval *tval;

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;
    icmp->icmp_id = pid;
    packsize = 8+datalen;
    tval = (struct timeval*)icmp->icmp_data;
    gettimeofday(tval, NULL);
    icmp->icmp_cksum = cal_chksum((unsigned short*)icmp, packsize);
    return packsize;
}

void send_packet(void)
{
    int packetsize;

    while (nsend < MAX_NO_PACKETS)
    {
        nsend++;
        packetsize = pack(nsend);

        if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*)
            &dest_addr, sizeof(struct sockaddr)) < 0)
        {
            perror("sendto error");
            continue;
        } sleep(1);
    }
}

void recv_packet(void)
{
    int n, fromlen;
    extern int errno;
    //signal(SIGALRM, statistics);
    fromlen = sizeof(from);
    while (nreceived < nsend)
    {
        //alarm(MAX_WAIT_TIME);
        if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct
            sockaddr*) &from, (socklen_t *)&fromlen)) < 0)
        {
            if (errno == EINTR)
                continue;
            perror("recvfrom error");
            continue;
        }gettimeofday(&tvrecv, NULL);

        if (unpack(recvpacket, n) ==  - 1)
            continue;
        nreceived++;
    }
}

int unpack(char *buf, int len)
{
    int iphdrlen;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;
    double rtt;

    ip = (struct ip*)buf;
    iphdrlen = ip->ip_hl << 2;
    icmp = (struct icmp*)(buf + iphdrlen);
    len -= iphdrlen;
    if (len < 8)
    {
        printf("ICMP packets\'s length is less than 8\n");
        return  - 1;
    }

    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))
    {
        tvsend = (struct timeval*)icmp->icmp_data;
        tv_sub(&tvrecv, tvsend);
        rtt = tvrecv.tv_sec * 1000+tvrecv.tv_usec / 1000;
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n", len,
            inet_ntoa(from.sin_addr), icmp->icmp_seq, ip->ip_ttl, rtt);
    }

    else
        return  - 1;
    return 0;
}

int ping(int ipFamily, char *szaddress,char *szGatewayaddress, char *szDevice)
{
    int size = 50 * 1024;
    nsend = 0, nreceived = 0;
    int rc;

    if ( ipFamily == 4)
    {
        if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        {
            printf("socket error\n");
            perror("socket error");
            return -1;
        }
        printf("socket bind to device GW:%s,Addr:%s,Dev:%s\n",
        szGatewayaddress,
        szaddress,
        szDevice);
        setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
        memset(&dest_addr, 0 ,sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_addr.s_addr = inet_addr(szaddress);
        pid = getpid();
        printf("%s PING %s(%s): %d bytes data in ICMP packets.\n",szDevice, szaddress, inet_ntoa
            (dest_addr.sin_addr), datalen);
    }
    else if (ipFamily == 6)
    {
        if ((sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_IP)) < 0)
        {
            printf("socket error\n");
            perror("socket error");
            return -1;
        }
        setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
        memset(&dest_ipv6_addr, 0 ,sizeof(dest_ipv6_addr));
        dest_ipv6_addr.sin6_family = AF_INET6;

        rc = inet_pton(AF_INET6, szaddress, &dest_ipv6_addr.sin6_addr);
        if ( rc != 1)
            perror("inet_pton error");
        pid = getpid();
        printf("%s PING %s: %d bytes data in ICMP packets.\n",szDevice, szaddress, datalen);
    }
    send_packet();
    recv_packet();
    statistics(SIGALRM);
    return 0;
}

void tv_sub(struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0)
    {
        --out->tv_sec;
        out->tv_usec += 1000000;
    } out->tv_sec -= in->tv_sec;
}
