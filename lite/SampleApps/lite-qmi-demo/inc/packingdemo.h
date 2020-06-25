#ifndef _PACKING_DEMO_H_
#define _PACKING_DEMO_H_
#include <stdbool.h>
#include "common.h"
#include "msgid.h"
#include "qmerrno.h"
#include "qmap.h"
#include "dms.h"
#include "uim.h"
#include "dummy_msg.h"
#include "dummy_msg_cmp.h"
#pragma once

#define QMI_MSG_MAX (2048)

#define MAX_TEST_ID_INPUT_SIZE   16
#define ENTER_KEY                0x0A

//#define QMI_DBG
//#define TEST_DBG
#define HEXDUMP
#define MAX_ROUTE_STRING 255
#define MAX_NUMBER_OEM_CONNECTION 4

#define QMI_GET_SERVICE_FILE_IOCTL  0x8BE0 + 1
#define QMI_GET_QMAP_SUPPORT         0x8BE0 + 15
#define QMI_GET_SVC_VERSION_IOCTL    0x8BE0 + 19

#define MAX_PATH_LEN NAME_MAX + 64 // NAME_MAX
#define MAX_TEST_PROCESS 32
#define STR_VALUE(arg)      #arg
#define FUNCTION_NAME(name) STR_VALUE(name)
#define MAX_QMAP_INSTANCE 8
#define SYS_FS_NET_PATH "/sys/class/net/"
#define SYS_FS_NET_QMI_PATH "/device/GobiQMI/qcqmi"

typedef int (* pack_func) (pack_qmi_t*, uint8_t*, uint16_t*, void*);
typedef int (* unpack_func) (uint8_t*, uint16_t, void*);
typedef void (* dump_func) (void*);

#define pack_func_item pack_func* 
#define unpack_func_item unpack_func*

#define run_pack_item(x) ((pack_func) x)
#define run_unpack_item(x) ((unpack_func) x)

typedef struct{
    pack_func_item   pack;
    char pack_func_name[128];
    void*       pack_ptr;
    unpack_func_item unpack;
    char unpack_func_name[128];
    void*       unpack_ptr;
    dump_func   dump;
} testitem_t;

typedef struct{
    long type;
    uint8_t buf[QMI_MSG_MAX];
} msgbuf;

typedef struct
{
    char pdnDestIp[MAX_NUMBER_OEM_CONNECTION][MAX_ROUTE_STRING];
    uint8_t profileId;
    uint8_t prefNetworkType;
    char modelId[DMS_SLQSFWINFO_MODELID_SZ];
} wdsConfig;

typedef struct
{
    uint16_t mcc;
    uint16_t mnc;
    char networkName[255];
    char modelId[DMS_SLQSFWINFO_MODELID_SZ];
} nasConfig;

typedef struct
{
    char modelId[DMS_SLQSFWINFO_MODELID_SZ];
    char fwVersion[DMS_SLQSFWINFO_BOOTVERSION_SZ];
    char imei[16];
    char iccid[25];
} dmsConfig;

typedef struct
{
    uint32_t storageType ;
    uint32_t msgMode;
    uint32_t msgFormat;
    char number[20];
} smsConfig;

typedef struct
{
    char pin[UIM_MAX_DESCRIPTION_LENGTH];
    char puk[UIM_MAX_DESCRIPTION_LENGTH];
    char modelId[DMS_SLQSFWINFO_MODELID_SZ];
} uimConfig;


typedef enum
{
    E_PDN1_IP_ADDRESS,
    E_PDN2_IP_ADDRESS,
    E_PDN3_IP_ADDRESS,
    E_PDN4_IP_ADDRESS,
    E_PROFILE_ID,
    E_PREFER_NETWORK_TYPE,
    E_MCC,
    E_MNC,
    E_NETWORK_NAME,
    E_MODEL_ID,
    E_FIRMWARE_VERSION,
    E_IMEI,
    E_ICCID,
    E_SMS_STORAGE_TYPE,
    E_SMS_MESSAGE_MODE,
    E_SMS_MESSAGE_FORMAT,
    E_SMS_PHONE_NUMBER,
    E_UIM_PIN_CODE,
    E_UIM_PUK_CODE,
} configFileList;

extern uint8_t g_qmi_req[QMI_MSG_MAX];
extern uint8_t g_rsp[QMI_MSG_MAX];
extern int client_fd(uint8_t svc);
extern int CheckDevcieState(int fd);
extern int g_runoem_demo;
extern int g_runqmap_demo;
extern unsigned int g_num_of_rmnet_supported ;
extern int g_qos_started;
extern void dump_hex(uint16_t dataLen,uint8_t *hexData );
int local_fprintf (const char *format, ...);
void dump_common_resultcode (void *ptr);

#define NUMBER_OF_DHCLINET_PROGRAMS 2
extern const char *dhClientBin[NUMBER_OF_DHCLINET_PROGRAMS];
extern int checkPath(const char *szBinFile);
extern int g_auto_test_enable;
extern int g_mtu_auto_update_enable;
extern int g_ip_auto_assign;
#ifndef _ASYNC_READ_ENABLE_
#define _ASYNC_READ_ENABLE_ 0
#endif
#if _ASYNC_READ_ENABLE_
#include <aio.h>
#define DEFAULT_ASYNC_READ_WAIT_DATA_TIMEOUT 30
int iAsyncRead(struct aiocb *paiocb,int *pFd, size_t size,uint8_t *pBuf);
int iWaitAsyncReadDataReady(struct aiocb *paiocb,int *pFd,int iTimeoutInSec);
#endif


#define MAX_SLAB_TEST_LOOP_COUNT 10000
#define MAX_SLABS 500

typedef struct slabinfo {
    unsigned long objects;
}slabinfo;
unsigned long get_total_slab_activity();

//return if dump function input argument is NULL
#define CHECK_DUMP_ARG_PTR_IS_NULL if (NULL == ptr) { return ;};
//return if pointer is NULL
#define RETURN_IF_PTR_IS_NULL(x) if (NULL == x) { return ;};

#define CHECK_PTR_IS_NOT_NULL(x) (NULL != x)

#define CHECK_WHITELIST_MASK(x,y)\
if(!check_uint256_whitelist_mask(x,y))\
{\
    local_fprintf("%s Unexpected PresenceMask/ ",\
            __FUNCTION__);\
    fprintf(stderr,"%s Unexpected PresenceMask\n",\
            __FUNCTION__);\
    swi_uint256_print_mask(y);\
}

#define CHECK_MANDATORYLIST_MASK(x,y)\
if(!check_uint256_whitelist_mask(y,x))\
{\
    local_fprintf("%s Missing Mandatory Tlv/ ",\
            __FUNCTION__);\
    fprintf(stderr,"%s Missing Mandatory Tlv\n",\
            __FUNCTION__);\
    swi_uint256_print_mask(y);\
}

#define FAILED_MSG "Wrong"
#define SUCCESS_MSG "Correct"
#define NA_MSG "NA"


bool check_uint256_whitelist_mask(
    swi_uint256_t whitelistmask,
    swi_uint256_t mask);

#endif //_PACKING_DEMO_H_

