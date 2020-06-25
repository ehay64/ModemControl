#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

#include <fcntl.h>
#include <stdbool.h>
#include "lite-fw.h"

#pragma once

#define MAX_PATH_LEN      NAME_MAX + 64
#define MAX_DEV_LEN       MAX_PATH_LEN
#define QMI_MSG_MAX       2048

#define DEFAULT_DM_PORT0  "/dev/ttyUSB0\0"
#define DEFAULT_AT_PORT0  "/dev/ttyUSB2\0"
#define DEFAULT_QMI_PORT0 "/dev/qcqmi0\0"
#define DEFAULT_MBIM_PORT0 "/dev/cdc-wdm0\0"
#define DEFAULT_DEV_MODE  "QMI"

#define QMI_GET_SERVICE_FILE_IOCTL  0x8BE0 + 1

#define MILLISECOND    1000
#define HALF_A_SECOND (500*MILLISECOND)
#define SECOND        (1000*MILLISECOND)

#define CRASH_ACTION_RESET 1

#define MAX_POWER_REQUEST_RETRY 3
#define MAX_USB_PATH_LEN 512
struct _userOptions
{
    char dev_mode_str[MAX_PATH_LEN];
    char qdl_dev_path[MAX_PATH_LEN];
    char dev_path[MAX_PATH_LEN];
    char fw_image_path[MAX_PATH_LEN];
    char log_file_path[MAX_PATH_LEN];
    char qdl_usb_path[MAX_USB_PATH_LEN];
    int  modelfamily;
    unsigned long lBlockSize;
};


enum fwdwl_modem_mode
{
    eLITE_MODEM_MODE_QMI=1,                         // modem is in QMI mode
    eLITE_MODEM_MODE_MBIM                           // modem is in MBIM mode
};

enum
{
    eLITE_FWDWL_APP_OK,                              //       Success
    eLITE_FWDWL_APP_ERR_GEN=101,                     // 101 - Generic error. Check logs
    eLITE_FWDWL_APP_ERR_INVALID_ARG,                 // 102 - Invalid arguments provided
    eLITE_FWDWL_APP_ERR_QMI,                         //     - Error in communicating with QMI FDs. Check logs
    eLITE_FWDWL_APP_ERR_LITEFW,                      //     - A lite-fw API has failed. Check logs.
    eLITE_FWDWL_APP_ERR_MODEM_STATE,                 //     - Modem state is unknown. Maybe not connected to host
    eLITE_FWDWL_APP_ERR_INVALID_CRASH_ACTION_STATE,  //     - Error in setting modem crash action to reset
    eLITE_FWDWL_APP_ERR_INVALID_MODEL_FAMILY,        //     - Error getting Model Family
    eLITE_FWDWL_APP_ERR_QDL                          //     - Error in communicating with QDL FDs. Check logs
};

enum fwdwl_modem_states
{
    eFWDWL_STATE_BOOT_AND_HOLD,
    eFWDWL_STATE_ONLINE,
    eFWDWL_STATE_DISCONNECTED,
    eFWDWL_STATE_UNKNOWN,
};

int FWVersionAfterDownload (char *szDevPath);
int FWVersionAfterDownload_QMI (char *szQMIPath);
int FWVersionAfterDownload_MBIM (char *szMBIMPath);

void HandleOnlineSPK(struct _userOptions userOptData);
int HandleOnlineCWEAndNVU(struct _userOptions userOptData,int  file_type,
                          bool *bValidateFWUpgrade,litefw_FirmwareInfo *info,
                          bool *bIsFWDownloadNeeded,int *image_mask,int *imgListLen);

int SetImagePrefForSavingToSlot(struct _userOptions userOptData, int file_type,
                                 libSDP_FirmwareInfo info, int *image_mask);

int HandleOnline9x07(struct _userOptions userOptData,int  file_type,
                     bool *bValidateFWUpgrade,litefw_FirmwareInfo *info,
                     bool *bIsFWDownloadNeeded,int *image_mask,int *imgListLen);

int SendImagePreferenceToModem_QMI(pack_fms_SetImagesPreference_t pack_request,    unpack_fms_SetImagesPreference_t *pSetPrefRspFromModem, char *szQMIPath);

int SendImagePreferenceToModem_MBIM(pack_fms_SetImagesPreference_t pack_request,    unpack_fms_SetImagesPreference_t *pSetPrefRspFromModem, char *szMBIMPath);

int SendSetImagePreferenceToModem_QMI(unpack_dms_SetFirmwarePreference_t *pSetPrefRspFromModem, char *szQMIPath);

int SendSetImagePreferenceToModem_MBIM(unpack_dms_SetFirmwarePreference_t *pSetPrefRspFromModem, char *szMBIMPath);

int ResetModem (char *szDevPath);
int ResetModem_QMI (char *szQMIPath);
int ResetModem_MBIM (char *szMBIMPath);

int verify_modem_Crash_Action (char *szDevPath);
int verify_modem_Crash_Action_QMI (char *szQMIPath);
int verify_modem_Crash_Action_MBIM (char *szMBIMPath);


/*  MBIM delcarations */

struct mbim_header {
  uint32_t type;
  uint32_t length;
  uint32_t transaction_id;
} __attribute__((packed));

#define MBIM_MESSAGE_GET_MESSAGE_TYPE(self)                             \
    (MbimMessageType) GUINT32_FROM_LE (((struct header *)(self->data))->type)
#define MBIM_MESSAGE_GET_MESSAGE_LENGTH(self)                           \
    GUINT32_FROM_LE (((struct header *)(self->data))->length)
#define MBIM_MESSAGE_GET_TRANSACTION_ID(self)                           \
    GUINT32_FROM_LE (((struct header *)(self->data))->transaction_id)

struct mbim_open_message {
    uint32_t max_control_transfer;
} __attribute__((packed));

struct mbim_open_done_message {
    uint32_t status_code;
} __attribute__((packed));

struct mbim_error_message {
    uint32_t error_status_code;
} __attribute__((packed));

struct mbim_fragment_header {
  uint32_t total;
  uint32_t current;
} __attribute__((packed));

struct mbim_command_message {
    struct mbim_fragment_header fragment_header;
    uint8_t                 service_id[16];
    uint32_t                command_id;
    uint32_t                command_type;
    uint32_t                buffer_length;
} __attribute__((packed));

struct mbim_command_done_message {
    struct mbim_fragment_header fragment_header;
    uint8_t                 service_id[16];
    uint32_t                command_id;
    uint32_t                status_code;
    uint32_t                buffer_length;
} __attribute__((packed));


/* MBIM full message struct */
struct mbim_full_message {
    struct mbim_header header;
    union {
        struct mbim_open_message            open;
        struct mbim_open_done_message       open_done;
        struct mbim_command_message         command;
        struct mbim_command_done_message    command_done;
        struct mbim_error_message           error;
    } message;
} __attribute__((packed));


/**
 * MbimMessageType:
 * MBIM_MESSAGE_TYPE_INVALID: Invalid MBIM message.
 * MBIM_MESSAGE_TYPE_OPEN: Initialization request.
 * MBIM_MESSAGE_TYPE_CLOSE: Close request.
 * MBIM_MESSAGE_TYPE_COMMAND: Command request.
 * MBIM_MESSAGE_TYPE_HOST_ERROR: Host-reported error in the communication.
 * MBIM_MESSAGE_TYPE_OPEN_DONE: Response to initialization request.
 * MBIM_MESSAGE_TYPE_CLOSE_DONE: Response to close request.
 * MBIM_MESSAGE_TYPE_COMMAND_DONE: Response to command request.
 * MBIM_MESSAGE_TYPE_FUNCTION_ERROR: Function-reported error in the communication.
 * MBIM_MESSAGE_TYPE_INDICATE_STATUS: Unsolicited message from the function.
 *
 * Type of MBIM messages.
 */
typedef enum {
    MBIM_MESSAGE_TYPE_INVALID         = 0x00000000,
    /* From Host to Function */
    MBIM_MESSAGE_TYPE_OPEN            = 0x00000001,
    MBIM_MESSAGE_TYPE_CLOSE           = 0x00000002,
    MBIM_MESSAGE_TYPE_COMMAND         = 0x00000003,
    MBIM_MESSAGE_TYPE_HOST_ERROR      = 0x00000004,
    /* From Function to Host */
    MBIM_MESSAGE_TYPE_OPEN_DONE       = 0x80000001,
    MBIM_MESSAGE_TYPE_CLOSE_DONE      = 0x80000002,
    MBIM_MESSAGE_TYPE_COMMAND_DONE    = 0x80000003,
    MBIM_MESSAGE_TYPE_FUNCTION_ERROR  = 0x80000004,
    MBIM_MESSAGE_TYPE_INDICATE_STATUS = 0x80000007
} MbimMessageType;


/**
 * MbimCidQmi:
 * MBIM_CID_QMI_UNKNOWN: Unknown command.
 * MBIM_CID_QMI_MSG: Configuration.
 *
 * MBIM command id for QMI service.
 */
typedef enum {
    MBIM_CID_QMI_UNKNOWN = 0,
    MBIM_CID_QMI_MSG     = 1
} MbimCidQmi;


/**
 * MbimMessageCommandType:
 * MBIM_MESSAGE_COMMAND_TYPE_UNKNOWN: Unknown type.
 * MBIM_MESSAGE_COMMAND_TYPE_QUERY: Query command.
 * MBIM_MESSAGE_COMMAND_TYPE_SET: Set command.
 *
 * Type of command message.
 */
typedef enum {
    MBIM_MESSAGE_COMMAND_TYPE_UNKNOWN = -1,
    MBIM_MESSAGE_COMMAND_TYPE_QUERY   = 0,
    MBIM_MESSAGE_COMMAND_TYPE_SET     = 1
} MbimMessageCommandType;


