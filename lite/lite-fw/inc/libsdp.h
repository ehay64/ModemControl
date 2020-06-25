/**
 *
 * @ingroup litefw
 *
 * @file lite-fw.h
 * Filename:    lite-fw.h
 *
 * Purpose:     Global definitions used inside the SDK
 *
 * Copyright: © 2016 Sierra Wireless Inc., all rights reserved
 *
 */
#ifndef _LITE_FW_H_
#define _LITE_FW_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Pragmas */
#pragma once
#include "qmerrno.h"
#include "fms.h"
#include "dms.h"

#define IMG_MASK_MDM (1<<0)
#define IMG_MASK_PRI (1<<1)
#define IMG_MASK_CLEAR   (0x000)
#define IMG_MASK_GENERIC (0xFFFF)

// Carrier package SKU Info
#define LITEFW_CARRIER_PACKAGE_SKU "9999999"
#define LITEFW_SKU_STRING_LENGTH   7

// Fimware Info String size
#define FIRMWARE_INFO_STRING_SIZE  20
#define NULL_TERMINATOR_CHAR_SIZE  1
#define FIRMWARE_IMAGE_SIZE_MAX    512
#define FIRMWARE_BCHVERSTRSIZE     84
#define FIRMWARE_BCHDATESIZE       8
#define MAX_IMAGE_PRODUCT_LENGTH   10

/* error Codes */
enum litefw_fwdwl_error_codes
{
    eSDP_FWDWL_SUCCESS = 0,             //       Success
    eSDP_FWDWL_ERR_GENERAL = 100,       // 100 - Generic FW download error
    eSDP_FWDWL_ERR_SDK,                 // 101 - SDK specific error. Please check syslog for SDK error codes
    eSDP_FWDWL_ERR_SET_CBK,             // 102 - Error in setting SDK callbacks
    eSDP_FWDWL_ERR_PATH_TOO_LONG,       // 103 - Path is too long
    eSDP_FWDWL_ERR_PATH_NOT_SPECIFIED,  // 104 - Mandatory SDK/FW path not set
    eSDP_FWDWL_ERR_FW_UPGRADE,          // 105 - Post FW download check indicates that the FW upgrade has failed. 
                                        //       For example, PRI not updated as expected, FW version not updated etc
    eSDP_FWDWL_ERR_INVALID_DEV,         // 106 - Invalid device
    eSDP_FWDWL_ERR_INVALID_PATH,        // 107 - Invalid Path or No valid firmware or nvu file exist in the path
    eSDP_FWDWL_ERR_TIMEOUT,             // 108 - Download Process Timeout
    eSDP_FWDWL_ERR_FAIL,                // 109 - Download Process Fail
    eSDP_FWDWL_ERR_PRI_FAIL,            // 110 - Download PRI Fail
    eSDP_FWDWL_ERR_FW_VERSION_FAIL,     // 111 - Download Fail Version Not Match
    eSDP_FWDWL_ERR_SDP_TIMEOUT,         // 112 - SSDP mode timeout
    eFIREHOSE_ERR_SECBOOT_INVALID_CERT_CHAIN,
    eSDP_FWDWL_ERR_END
};

// Match this enum with fw_image_type_e in imudefs.h
enum litefw_Fw_Type{
    eFW_TYPE_MBN_GOBI, // Not supported in litefw
    eFW_TYPE_MBN,      // Not supported in litefw
    eFW_TYPE_CWE,      //CWE File
    eFW_TYPE_NVU,      //NVU File
    eFW_TYPE_SPK,      //SPK File
    eFW_TYPE_INVALID,  //Invalid File
    eFW_TYPE_CWE_NVU,  //CWE and NVU
};


/*************
 *
 * Name:     litefw_fileimgtype
 *
 * Purpose:  Enumerate different PRI type
 *
 * Members:  eFILE_TYPE_NONE       - Start of list
 *           eFILE_TYPE_CAR_PRI    - Carrier PRI file (file name starts with "9999999_")
 *           eFILE_TYPE_OEM_PRI    - OEM PRI file     (file name strats with valid SKU such as "1104125_")
 *           eFILE_TYPE_COMPO_PRI  - A compound PRI files contain both Carrier PRI and OEM PRI
 */
enum litefw_fileimgtype
{
    eFILE_TYPE_NONE = 0,
    eFILE_TYPE_CAR_PRI,
    eFILE_TYPE_OEM_PRI,
    eFILE_TYPE_COMPO_PRI
};

/*************
 *
 * Name:     litefw_bcimagetype
 *
 * Purpose:  Enumerate some supported BC image types
 *
 * Members:  eIMAGE_TYPE_MIN       - Start of list
 *           eIMAGE_TYPE_BOOT      - boot composite image
 *           eIMAGE_TYPE_APPL      - application composite image
 *           eIMAGE_TYPE_AMSS      - amss
 *           eIMAGE_TYPE_APPS      - apps such as yocto image
 *           eIMAGE_TYPE_FILE      - Generic file such as Carrier PRI
 *           eIMAGE_TYPE_SPKG      - Sierra package, spk image, OEM PRI image
 *           eIMAGE_TYPE_MODM      - modem composite image, modem firmware cwe image
 *           eIMAGE_TYPE_USER      - image for 0:USERDATA, e.g. legato
 *           eIMAGE_TYPE_KEYS,     - Keystore image, signed key information
 *           eIMAGE_TYPE_MAX       - End of list
 *           eIMAGE_TYPE_INVALID   - invalid image type
 *           eIMAGE_TYPE_ANY       - any image type
 *
 * Notes:    none
 *
 **************/
enum litefw_imagetype
{
  eIMAGE_TYPE_MIN = 0,
  eIMAGE_TYPE_BOOT = eIMAGE_TYPE_MIN,
  eIMAGE_TYPE_APPL,
  eIMAGE_TYPE_APPS,
  eIMAGE_TYPE_FILE,
  eIMAGE_TYPE_SPKG,
  eIMAGE_TYPE_MODM,
  eIMAGE_TYPE_USER,
  eIMAGE_TYPE_KEYS,
  eIMAGE_TYPE_MAX,
  eIMAGE_TYPE_INVALID = 0xFF,
  eIMAGE_TYPE_ANY = eIMAGE_TYPE_INVALID,
};


/* Model Family */
enum litefw_Models{
    eModel_Unknown=-1,
    eModel_9X15=0,
    eModel_WP9X15=1,
    eModel_9X30=2,
    eModel_9x07=3,
    eModel_9x06=4
};

/* QDL mode */
enum litefw_QDL_MODEs{
    eQDL_MODE_Unknown=-1,
    eQDL_MODE_INIT=0,
    eQDL_MODE_TTYUSB=1,
    eQDL_MODE_UART0=2,
};

/* QDL mode Hardware flow control */
enum litefw_QDL_FLOW_CONTROLs{
    eQDL_HW_FLOW_Unknown=-1,
    eQDL_HW_FLOW_INIT=0,
    eQDL_HW_FLOW_ENABLE=1,
    eQDL_HW_FLOW_DISABLE=2,
};

/**
 *  This function enable/disable lite firmware debug logs.
 *
 *  \param[in]  log_en
 *              - Logs enable/disable flag.
 *
 */
void litefw_logsenable(int log_en);

/**
 *  This structure contains information of the provided firmware file
 *
 *  \param  szModelid_str
 *          - Model Name String
 *
 *  \param  szFwversion_str
 *          - Firmware Version String.
 *
 *  \param  szSku_str
 *          - SKU String.
 *
 *  \param  szPackageid_str
 *          - Package ID String.
 *
 *  \param  szCarrier_str
 *          - Carrier String.
 *
 *  \param  szCarrierPriversion_str
 *          - Carrier PRI Version String.
 *
 */

typedef struct _litefw_FirmwareInfo_
{
    char szModelid_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char szFwversion_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char szSku_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char szPackageid_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char szCarrier_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char szCarrierPriversion_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
}litefw_FirmwareInfo;

/**
 *  This structure contains information of the response parameters associated
 *  with a Read Transparent API.
 *
 *  \param  szPartno_str
 *          - Part Number of the provided firmware image
 *
 */

typedef struct _litefw_FirmwarePartNo_
{
    char szPartno_str[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
}litefw_FirmwarePartNo;

/**
 *  This structure provided more detailed information of the provided firmware file
 *
 *  \param  fullPath
 *          - full path of the file
 *  \param  imagemask
 *          - Bitmask provides type of file
 *            - bit0 - cwe
 *            - bit1 - nvu
 *            - bit2 - spk
 *  \param  headerType
 *          - see \ref litefw_imagetype
 *  \param  imageType
 *          - see \ref litefw_fileimgtype
 *  \param  modelIdStr
 *          - device model id
 *  \param  partNoStr
 *          - part number id
 *  \param  skuStr
 *          - sku id
 *  \param  packageIdStr
 *          - cwe sierra package id
 *  \param  carrierStr
 *          - carrier id
 *  \param  priVersionStr
 *          - pri version
 *  \param  versionStr
 *          - firmware version
 *  \param  releaseDate
 *          - release date of the file
 *
 */

typedef struct _litefw_FirmwareFileInfo
{
    char                    fullPath[FIRMWARE_IMAGE_SIZE_MAX];
    uint8_t                 imageMask;
    enum litefw_imagetype   headerType;
    enum litefw_fileimgtype imageType;
    char                    modelIdStr[FIRMWARE_INFO_STRING_SIZE + NULL_TERMINATOR_CHAR_SIZE];
    char                    partNoStr[FIRMWARE_INFO_STRING_SIZE+NULL_TERMINATOR_CHAR_SIZE];
    char                    skuStr[FIRMWARE_INFO_STRING_SIZE + NULL_TERMINATOR_CHAR_SIZE];
    char                    packageIdStr[FIRMWARE_INFO_STRING_SIZE + NULL_TERMINATOR_CHAR_SIZE];
    char                    carrierStr[FIRMWARE_INFO_STRING_SIZE + NULL_TERMINATOR_CHAR_SIZE];
    char                    priVersionStr[FIRMWARE_INFO_STRING_SIZE + NULL_TERMINATOR_CHAR_SIZE];
    char                    versionStr[FIRMWARE_BCHVERSTRSIZE];
    char                    releaseDate[FIRMWARE_BCHDATESIZE];;
}litefw_FirmwareFileInfo;

/**
 *  This API Extrace Firmware Parameters From Path.
 *
 *  \param[in]  pImagePath
 *              - Firmware Folder Path.
 *
 *  \param[out]  info
 *               - See \ref litefw_FirmwareInfo for more information.
 *
 *  \return 0 on success, litefw_fwdwl_error_codes error value otherwise
 *
 *  \sa     See litefw_fwdwl_error_codes for error values
 *
 *
 */
int litefw_ExtractFirmwareParametersByPath(char *pImagePath, litefw_FirmwareInfo *info);

/**
 *  This API Extrace Firmware Part Number From Path.
 *
 *  \param[in]  pImagePath
 *              - Firmware Folder Path.
 *
 *  \param[out]  partno
 *               - See \ref litefw_FirmwarePartNo for more information.
 *
 *  \return 0 on success, litefw_fwdwl_error_codes error value otherwise
 *
 *  \sa     See litefw_fwdwl_error_codes for error values
 *
 *
 */
int litefw_ExtractFirmwarePartNoByPath(char *pImagePath, litefw_FirmwarePartNo *partno);

/**
 *  This API Build Image Preference Request Using Firmware Information.
 *
 *  \param[in]  info
 *              - See \ref litefw_FirmwareInfo for more information.
 *
 *  \param[out]  pack
 *               - See \ref pack_fms_SetImagesPreference_t for more information.
 *
 *  \return 0 on success, litefw_fwdwl_error_codes error value otherwise
 *
 *  \sa     See litefw_fwdwl_error_codes for error values
 *
 *
 */
int litefw_BuildImagesPreferenceRequest(litefw_FirmwareInfo info,pack_fms_SetImagesPreference_t *pack);

/**
 *  This API Calculate Image Mask for Firmware Download.
 *
 *  \param[in]  SetPrefRspFromModem
 *              - See \ref unpack_fms_SetImagesPreference_t for more information.
 *
 *  \return Image Mask
 *          - IMG_MASK_MDM | IMG_MASK_PRI
 *
 */

int litefw_CalculateImageMask (unpack_fms_SetImagesPreference_t SetPrefRspFromModem);

/**
 *  This API Get File Type By Path
 *
 *  \param[in]  szPath
 *              - See \ref litefw_FirmwareInfo for more information.
 *
 *  \return eFW_TYPE_INVALID on error, litefw_Fw_Type value otherwise
 *
 *  \sa     See litefw_Fw_Type for values
 *
 *
 */
int litefw_getFileType(char* szPath);

/**
 *  This API Download Firmware.
 *
 *  \param[in]  pImagePath
 *              - Firmware Folder Path.
 *
 *  \param[in]  szTTYPath
 *              - QDL Device Path.
 *
 *  \param[in]  iFWImageType
 *              - Firmware Type.
 *              - See \ref litefw_Fw_Type
 *
 *  \param[in]  image_mask
 *              - Image Mask.
 *                - IMG_MASK_MDM | IMG_MASK_PRI
 *
 *  \param[in]  iModelFamily
 *              - Modem Family.
 *              - See \ref litefw_Models
 *
 *  \return 0 on success, litefw_fwdwl_error_codes error value otherwise
 *
 *  \sa     See litefw_fwdwl_error_codes for error values
 *
 *
 */
unsigned int litefw_DownloadFW(char *pImagePath,char *szTTYPath,int iFWImageType, int image_mask, int iModelFamily);

/**
 *  This API Get Model Famliy from a model string.
 *
 *  \param[in]  pModelString
 *              - Model String.
 *
 *  \return eModel_Unknown on Error, litefw_Models value otherwise
 *
 *  \sa     See litefw_Models for values
 *
 *
 */
int litefw_GetModelFamily(char *pModelString);

/**
 *  This API Check Valid Firmware Information to build Image Prefernce Request.
 *
 *  \param[in]  info
 *              - See \ref litefw_FirmwareInfo for more information.
 *
 *  \return 0 on success, -1 error value otherwise
 *
 */
int litefw_CheckValidFirmwareInfo(litefw_FirmwareInfo info);


/**
 *  This API Get Lib SDP Version.
 *
 *  \return Version String
 *
 */
char* litefw_GetVersion();


/**
 *  This Custom Log prototype.
 *
 *  \param[in]  lvl
 *              - Log level.
 *
 *  \param[in]  buff
 *              - Log String.
 *
 *  \return none
 *
 */
typedef void (* litefwlogger)(uint8_t lvl, const char* buff);

/**
 *  This API Set Custom Log function.
 *
 *  \param[in]  func
 *              - See \ref litefwlogger for more information.
 *
 *  \return none
 *
 */

int litefw_set_log_func(litefwlogger *func);

/**
 *  This API Set Read BlockSize.
 *
 *  \param[in] lBlockSize
 *             - Firmware Read Block Size.
 *
 *  \return none
 *
 */
void litefw_SetReadBlockSize(unsigned long lBlockSize);


/**
 *  This API switch 9x07 modem to download mode.
 *
 *  \param[in]  szTTYPath
 *              - QDL Device Path.
 *
 * \return 0 on success, -1 error value otherwise
 *
 */

int litefw_switch_9x07_to_downloadmode(char *szTTYPath);


/**
 *  This API switch modem to boot hold modem via QDL port.
 *
 *  \param[in]  szTTYPath
 *              - QDL Device Path.
 *
 * \return 0 on success, -1 error value otherwise
 *
 */
int litefw_switch_to_BootHoldMode(char *szTTYPath);

/**
 *  This API set QDL port donwload mode.
 *
 *  \param[in]  iMode
 *              - QDL Download Mode.
 *                  - litefw_QDL_MODEs
 *
 * \return 0 on success, -1 error value otherwise
 *
 */
int litefw_SetQTLDownloadMode(int iMode);

/**
 *  This API Get QDL port donwload mode.
 *
 * \return eQDL_MODE_Unknown on Error, litefw_QDL_MODEs value otherwise
 *
 */
int litefw_GetQTLDownloadMode();

/**
 *  This API Set QDL port Hardware flow control.
 *
 *  \param[in]  iMode
 *              - litefw_QDL_FLOW_CONTROLs
 *
 *
 * \return 0 on success, -1 error value otherwise
 *
 */
int litefw_SetQTLHWFlowControl(int iMode);

/**
 *  This API Get QDL port Hardware flow control.
 *
 *
 * \return eQDL_HW_FLOW_Unknown on Error, litefw_QDL_FLOW_CONTROLs value otherwise
 *
 */
int litefw_GetQTLHWFlowControl();

/**
 *  This API Get the firmware information of the provided image
 *
 *  \param[in]  fullFilePath
 *              - file path or directory of the firmware image
 * 
 *  \param[out]  info
 *              - See \ref litefw_FirmwareFileInfo, application should allocate the memory
 *              - for the struct array, this API will fill in the details in the provided storage 
 * 
  *  \param[out]  pNumOfItems
 *              - number of litefw_FirmwareFileInfo
 *
 * \return enum eQCWWANError
 *
 */
enum eQCWWANError  litefw_SLQSGetFirmwareFileInfo(
    char *fullFilePath,
    litefw_FirmwareFileInfo* info,
    uint8_t *pNumOfItems);


#define libSDP_GetVersion litefw_GetVersion
#define libSDP_CalculateImageMask litefw_CalculateImageMask
#define libSDP_getFileType litefw_getFileType
#define libSDP_ExtractFirmwareParametersByPath litefw_ExtractFirmwareParametersByPath
#define libSDP_GetModelFamily litefw_GetModelFamily
#define libSDP_CheckValidFirmwareInfo litefw_CheckValidFirmwareInfo
#define libSDP_BuildImagesPreferenceRequest litefw_BuildImagesPreferenceRequest
#define libSDP_DownloadFW litefw_DownloadFW
#define libsdp_set_log_func litefw_set_log_func
#define libsdp_SetReadBlockSize litefw_SetReadBlockSize
#define LIBSDP_CARRIER_PACKAGE_SKU LITEFW_CARRIER_PACKAGE_SKU
#define LIBSDP_SKU_STRING_LENGTH LITEFW_SKU_STRING_LENGTH
typedef litefw_FirmwareInfo libSDP_FirmwareInfo;
typedef litefwlogger libsdplogger;

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif // _LITE_FW_H_

