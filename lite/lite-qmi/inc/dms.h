/**
 * \ingroup liteqmi
 *
 * \file dms.h
 */
#ifndef __LITEQMI_DMS_H__
#define __LITEQMI_DMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>
#define DMS_UINT8_MAX_STRING_SZ 255
#define DMS_MAX_CUST_ID_LEN   64
#define DMS_MAX_CUST_VALUE_LEN 8
#define DMS_IMGDETAILS_LEN    16
#define DMS_MAX_FWUPDATE_LOG_STR_SZ 255
#define DMS_MAX_FWUPDATE_REF_STR_SZ 15
#define DMS_VALID_FSN_LEN 14
#define DMS_MAX_RADIO_IFCS_SIZE   255

#define DMS_MAX_SUBS_CFG_LIST_SIZE    32
#define DMS_MAX_SUBS_LIST_SIZE   32
#define DMS_MAX_SUPPORTED_LTE_BANDS 255
/**
 * This structure used to store unpack_dms_GetModelID parameters
 *
 * @param modelid - Device model id.
 *                - NULL-terminated String.
 *                - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param Tlvresult - unpack Tlv Result.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    char modelid[255];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetModelID_t;

/**
 * This structure used to store unpack_dms_GetIMSI parameters
 *
 * @param imsi - IMSI no.
 *                - NULL-terminated String.
 *                - Bit to check in ParamPresenceMask - <B>1</B>
 * @param Tlvresult - unpack Tlv Result.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    char imsi[255];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetIMSI_t;

#define DMS_SLQSFWINFO_MODELID_SZ     20
#define DMS_SLQSFWINFO_BOOTVERSION_SZ 85
#define DMS_SLQSFWINFO_APPVERSION_SZ  85
#define DMS_SLQSFWINFO_SKU_SZ         15
#define DMS_SLQSFWINFO_PACKAGEID_SZ   85
#define DMS_SLQSFWINFO_CARRIER_SZ     20
#define DMS_SLQSFWINFO_PRIVERSION_SZ  16
#define DMS_SLQSFWINFO_CUR_CARR_NAME  17
#define DMS_SLQSFWINFO_CUR_CARR_REV   13

#define MAX_BUILD_ID_LEN  255
#define UNIQUE_ID_LEN     16
#define SLQS_MAX_DYING_GASP_CFG_SMS_CONTENT_LENGTH 160
#define SLQS_MAX_DYING_GASP_CFG_SMS_NUMBER_LENGTH 20
#define SPC_SIZE 6
#define CK_MAX_SIZE 8
#define ACT_CODE_MAX_SIZE 81
#define ERI_DATA_MAX_SIZE 1024
#define MEID_MAX_SIZE 8

/**
 * This structure used to store unpack_dms_GetFirmwareInfo parameters
 *
 * @param modelid_str - Mode ID String.
 *                    - NULL-terminated Mode ID String.
 *                    - Bit to check in ParamPresenceMask - <B>16</B>
 * @param bootversion_str - Boot Version.
 *                    - NULL-terminated Boot Version String.
 *                    - Bit to check in ParamPresenceMask - <B>17</B>
 * @param appversion_str - Application Version String.
 *                    - NULL-terminated Application Version String.
 *                    - Bit to check in ParamPresenceMask - <B>18</B>
 * @param sku_str - SKU String.
 *                    - NULL-terminated SKU String.
 *                    - Bit to check in ParamPresenceMask - <B>19</B>
 * @param packageid_str - Package ID String.
 *                    - NULL-terminated Package ID String.
 *                    - deprecated on EM/MC74xx(9x30) devices
 *                    - Bit to check in ParamPresenceMask - <B>20</B>
 * @param carrier_str - Carrier String.
 *                    - NULL-terminated Carrier String.
 *                    - Bit to check in ParamPresenceMask - <B>21</B>
 * @param priversion_str - PRI Version String.
 *                    - NULL-terminated PRI Version String.
 *                    - Bit to check in ParamPresenceMask - <B>22</B>
 * @param cur_carr_name - Current Carrier Name String.
 *                    - NULL-terminated Current Carrier Name String.
 *                    - Bit to check in ParamPresenceMask - <B>23</B>
 * @param cur_carr_rev - Current Carrier Revision String.
 *                    - NULL-terminated Current Carrier Revision String.
 *                    - Bit to check in ParamPresenceMask - <B>24</B>
 * @param Tlvresult - unpack Tlv Result.
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct {
    char modelid_str[DMS_SLQSFWINFO_MODELID_SZ];
    char bootversion_str[DMS_SLQSFWINFO_BOOTVERSION_SZ];
    char appversion_str[DMS_SLQSFWINFO_APPVERSION_SZ];
    char sku_str[DMS_SLQSFWINFO_SKU_SZ];
    char packageid_str[DMS_SLQSFWINFO_PACKAGEID_SZ];
    char carrier_str[DMS_SLQSFWINFO_CARRIER_SZ];
    char priversion_str[DMS_SLQSFWINFO_PRIVERSION_SZ];
    char cur_carr_name[DMS_SLQSFWINFO_CUR_CARR_NAME];
    char cur_carr_rev[DMS_SLQSFWINFO_CUR_CARR_REV];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetFirmwareInfo_t;

/**
 * This structure used to store unpack_dms_GetPower parameters
 *
 * @param OperationMode - operating mode.
 *        - 0 - Online (default).
 *        - 1 - Low power (airplane) mode.
 *        - 2 - Factory test mode.
 *        - 3 - Offline.
 *        - 4 - Resetting.
 *        - 5 - Power off.
 *        - 6 - Persistent low power (airplane) mode.
 *        - 7 - Mode - only low power.
 *        - Bit to check in ParamPresenceMask - <B>1</B>
 *        
 * @note  Valid transitions for Power Modes
 *        - Online to Low Power, Persistent low power, Factory test, Offline or Shut Down
 *        - Low power to online, Persistent low power, Offline, or Shut Down
 *        - Persistent low power to Online, Low power, Offline or Shut down
 *        - Factory test to online
 *        - Offline to Reset
 *
 * @param OfflineReason - offline reason.
 *        - 0x0001 - Host image misconfiguration.
 *        - 0x0002 - PRI image misconfiguration.
 *        - 0x0004 - PRI version incompatible.
 *        - 0x0008 - Device memory is full,cannot copy PRI information.
 *        - Bit to check in ParamPresenceMask - <B>16</B>
 * @param HardwareControlledMode - hardware restricted mode.
 *        - 0x00 - FALSE.
 *        - 0x01 - TRUE.
 *        - Bit to check in ParamPresenceMask - <B>17</B>
 * @param Tlvresult - unpack Tlv Result.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t OperationMode;
    uint32_t OfflineReason;
    uint32_t HardwareControlledMode;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetPower_t;

/**
 * Get IMSI pack. 
 * This API is deprecated on MC73xx/EM73xx modules since
 * firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use pack_uim_ReadTransparent()(EF ID: 3F00 7F20 6F07 for 2G card and
 * 3F00 7FFF 6F07 for 3G card) instead for new firmware versions and new modules.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetIMSI(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get IMSI unpack. 
 * This API is deprecated on MC73xx/EM73xx modules since
 * firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use unpack_uim_ReadTransparent()(EF ID: 3F00 7F20 6F07 for 2G card and
 * 3F00 7FFF 6F07 for 3G card) instead for new firmware versions and new modules.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetIMSI(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetIMSI_t *pOutput
        );

/**
 * Get model id pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetModelID(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get model id unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetModelID(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetModelID_t *pOutput
        );

/**
 * Get firmware info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetFirmwareInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get firmware info unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetFirmwareInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetFirmwareInfo_t *pOutput
        );

/**
 * Get operating mode of the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetPower(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
        );

/**
 * Get operating mode of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetPower(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetPower_t *pOutput
        );

/**
 * This structure used to store unpack_dms_GetSerialNumbers parameters
 *
 * @param esn        - Electronic Serial Number of the device.
 *                   - NULL-terminated ESN string. Empty string is
 *                     returned when ESN is not supported/programmed.
 *                   - Bit to check in ParamPresenceMask - <B>16</B>
 * @param imei_no    - International Mobile Equipment Identity of the device.
 *                   - NULL terminated IMEI string. Empty string is returned
 *                     when IMEI is not supported/programmed.
 *                   - Bit to check in ParamPresenceMask - <B>17</B>
 * @param meid       - Mobile Equipment Identifier of the device.
 *                   - NULL-terminated MEID string. Empty string is returned
 *                     when MEID is not supported/programmed.
 *                   - Bit to check in ParamPresenceMask - <B>18</B>
 * @param imeisv_svn - NULL-terminated IMEI SVN string. Empty string is returned
 *                     when IMEI SVN is not supported/programmed.imei software
 *                     version revision.
 *                   - Bit to check in ParamPresenceMask - <B>19</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    char esn[255];
    char imei_no[255];
    char meid[255];
    char imeisv_svn[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetSerialNumbers_t;

/**
 * Get serial numbers pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetSerialNumbers(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get serial numbers unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetSerialNumbers(
       uint8_t *pResp,
       uint16_t respLen,
       unpack_dms_GetSerialNumbers_t *pOutput
       );

/**
 * This structure used to store unpack_dms_GetHardwareRevision parameters
 *
 * @param hwVer - Hardware version
 *              - NULL-terminated string
 *              - Maximum Length is 255 Bytes
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    char hwVer[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetHardwareRevision_t;

/**
 * Get hardware revision of the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetHardwareRevision(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );
/**
 * Get hardware revision of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetHardwareRevision(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetHardwareRevision_t *pOutput
        );

/**
 * This structure contains the Band Capabilities response.\n
 * Please check is_<Param_Name>_Available field for presence of optional parameters
 *  @param  bandCapability[OUT]
 *          Bitmask of bands supported by the device
 *              - Bit 0 - Band class 0, A-system
 *              - Bit 1 - Band class 0, B-system
 *              - Bit 2 - Band class 1, all blocks
 *              - Bit 3 - Band class 2
 *              - Bit 4 - Band class 3, A-system
 *              - Bit 5 - Band class 4, all blocks
 *              - Bit 6 - Band class 5, all blocks
 *              - Bit 7 - GSM DCS band (1800)
 *              - Bit 8 - GSM Extended GSM (E-GSM) band (900)
 *              - Bit 9 - GSM Primary GSM (P-GSM) band (900)
 *              - Bit 10 - Band class 6
 *              - Bit 11 - Band class 7
 *              - Bit 12 - Band class 8
 *              - Bit 13 - Band class 9
 *              - Bit 14 - Band class 10
 *              - Bit 15 - Band class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM railways GSM band (900)
 *              - Bit 21 - GSM PCS band (1900)
 *              - Bit 22 - WCDMA (Europe, Japan, and China) 2100 band
 *              - Bit 23 - WCDMA US PCS 1900 band
 *              - Bit 24 - WCDMA (Europe and China) DCS 1800 band
 *              - Bit 25 - WCDMA US 1700 band
 *              - Bit 26 - WCDMA US 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band class 12
 *              - Bit 29 - Band class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band class 15
 *              - Bits 32 through 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bits 51 through 55 - Reserved
 *              - Bit 56 - Band class 16
 *              - Bit 57 - Band class 17
 *              - Bit 58 - Band class 18
 *              - Bit 59 - Band class 19
 *              - Bit 60 - WCDMA Japan 850 band
 *              - Bit 61 - WCDMA 1500 band
 *              - Bits 62 and 63 - Reserved
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  is_LteBandCapability_Available[OUT]
 *              LTE Band Capabilities Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *                  - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  LteBandCapability[OUT]
 *               Bitmask of LTE bands supported by the device
 *                   - Bit 0 - LTE EUTRAN Band 1 UL:1920-1980; DL: 2110-2170
 *                   - Bit 1 - LTE EUTRAN Band 2 UL:1850-1910; DL: 1930-1990
 *                   - Bit 2 - LTE EUTRAN Band 3 UL:1710-1785; DL: 1805-1880
 *                   - Bit 3 - LTE EUTRAN Band 4 UL:1710-1755; DL: 2110-2155
 *                   - Bit 4 - LTE EUTRAN Band 5 UL: 824-849; DL: 869-894
 *                   - Bit 5 - LTE EUTRAN Band 6 UL: 830-840; DL: 875-885
 *                   - Bit 6 - LTE EUTRAN Band 7 UL:2500-2570; DL: 2620-2690
 *                   - Bit 7 - LTE EUTRAN Band 8 UL: 880-915; DL: 925-960
 *                   - Bit 8 - LTE EUTRAN Band 9 UL:1749.9-1784.9; DL: 1844.9-1879.9
 *                   - Bit 9 - LTE EUTRAN Band 10 UL:1710-1770; DL: 2110-2170
 *                   - Bit 10 - LTE EUTRAN Band 11 UL:1427.9-1452.9; DL: 1475.9-1500.9
 *                   - Bit 11 - LTE EUTRAN Band 12 UL:698-716; DL: 728-746
 *                   - Bit 12 - LTE EUTRAN Band 13 UL: 777-787; DL: 746-756
 *                   - Bit 13 - LTE EUTRAN Band 14 UL: 788-798; DL: 758-768
 *                   - Bits 14 and 15 - Reserved
 *                   - Bit 16 - LTE EUTRAN Band 17 UL: 704-716; DL: 734-746
 *                   - Bit 17 - LTE EUTRAN Band 18 UL: 815-830; DL: 860-875
 *                   - Bit 18 - LTE EUTRAN Band 19 UL: 830-845; DL: 875-890
 *                   - Bit 19 - LTE EUTRAN Band 20 UL: 832-862; DL: 791-821
 *                   - Bit 20 - LTE EUTRAN Band 21 UL: 1447.9-1462.9; DL: 1495.9-1510.9
 *                   - Bit 21 - Reserved
 *                   - Bit 22 - LTE EUTRAN Band 23 UL: 2000-2020; DL: 2180-2200
 *                   - Bit 23 - LTE EUTRAN Band 24 UL: 1626.5-1660.5; DL: 1525-1559
 *                   - Bit 24 - LTE EUTRAN Band 25 UL: 1850-1915; DL: 1930-1995
 *                   - Bit 25 - LTE EUTRAN Band 26 UL: 814-849; DL: 859-894
 *                   - Bit 26 - Reserved
 *                   - Bit 27 - LTE EUTRAN Band 28 UL: 703-748; DL: 758-803
 *                   - Bit 28 - LTE EUTRAN Band 29 UL: 1850-1910 or 1710-1755; DL: 716-728
 *                   - Bit 29 - LTE EUTRAN Band 30 UL: 2350-2360; DL: 2305-2315
 *                   - Bit 30 - Reserved
 *                   - Bit 31 - LTE EUTRAN Band 32 DL: 9920-10359
 *                   - Bit 32 - LTE EUTRAN Band 33 UL: 1900-1920; DL: 1900-1920
 *                   - Bit 33 - LTE EUTRAN Band 34 UL: 2010-2025; DL: 2010-2025
 *                   - Bit 34 - LTE EUTRAN Band 35 UL: 1850-1910; DL: 1850-1910
 *                   - Bit 35 - LTE EUTRAN Band 36 UL: 1930-1990; DL: 1930-1990
 *                   - Bit 36 - LTE EUTRAN Band 37 UL: 1910-1930; DL: 1910-1930
 *                   - Bit 37 - LTE EUTRAN Band 38 UL: 2570-2620; DL: 2570-2620
 *                   - Bit 38 - LTE EUTRAN Band 39 UL: 1880-1920; DL: 1880-1920
 *                   - Bit 39 - LTE EUTRAN Band 40 UL: 2300-2400; DL: 2300-2400
 *                   - Bit 40 - LTE EUTRAN Band 41 UL: 2496-2690; DL: 2496-2690
 *                   - Bit 41 - LTE EUTRAN Band 42 UL: 3400-3600; DL: 3400-3600
 *                   - Bit 42 - LTE EUTRAN Band 43 UL: 3600-3800; DL: 3600-3800
 *                   - Bits 43 through 64 - Reserved
 *                   - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  is_TdsBandCapability_Available[OUT]
 *              TDS Band Capabilities Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  TdsBandCapability[OUT]
 *               Bitmask of TDS bands supported by the device.
 *                   - Bit 0 - TDS Band A 1900 to 1920 MHz, 2010 to 2020 MHz
 *                   - Bit 1 - TDS Band B 1850 to 1910 MHz, 1930 to 1990 MHz
 *                   - Bit 2 - TDS Band C 1910 to 1930 MHz
 *                   - Bit 3 - TDS Band D 2570 to 2620 MHz
 *                   - Bit 4 - TDS Band E 2300 to 2400 MHz
 *                   - Bit 5 - TDS Band F 1880 to 1920 MHz
 *                   - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint64_t bandCapability;
    int is_LteBandCapability_Available;
    uint64_t LteBandCapability;
    int is_TdsBandCapability_Available;
    uint64_t TdsBandCapability;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSGetBandCapability_t;

/**
 *  This structure contains the TLV to get Info of Supported LTE Bands.
 *
 *  \param[OUT]  supportedLteBandLen[OUT]
 *               - Supported LTE Bands length
 *               - Number of set of following elements
 *                   - lteBands
 *
 *  \param[OUT]  lteBands[OUT]
 *               - Array of supported LTE bands where each entry is decimal
 *                 representative of the LTE band supported.
 *
 *  \param[OUT]  TLVPresent[OUT]
 *               - TLV Present
 *
 */
typedef struct
{
    uint16_t    supportedLteBandLen;
    uint16_t    lteBands[DMS_MAX_SUPPORTED_LTE_BANDS];
    uint8_t     TLVPresent;
} dms_LteBandsSupport;

/**
 * This structure contains the Band Capabilities response.\n
 * Please check is_<Param_Name>_Available field for presence of optional parameters
 *  @param  bandCapability[OUT]
 *          Bitmask of bands supported by the device
 *              - Bit 0 - Band class 0, A-system
 *              - Bit 1 - Band class 0, B-system
 *              - Bit 2 - Band class 1, all blocks
 *              - Bit 3 - Band class 2
 *              - Bit 4 - Band class 3, A-system
 *              - Bit 5 - Band class 4, all blocks
 *              - Bit 6 - Band class 5, all blocks
 *              - Bit 7 - GSM DCS band (1800)
 *              - Bit 8 - GSM Extended GSM (E-GSM) band (900)
 *              - Bit 9 - GSM Primary GSM (P-GSM) band (900)
 *              - Bit 10 - Band class 6
 *              - Bit 11 - Band class 7
 *              - Bit 12 - Band class 8
 *              - Bit 13 - Band class 9
 *              - Bit 14 - Band class 10
 *              - Bit 15 - Band class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM railways GSM band (900)
 *              - Bit 21 - GSM PCS band (1900)
 *              - Bit 22 - WCDMA (Europe, Japan, and China) 2100 band
 *              - Bit 23 - WCDMA US PCS 1900 band
 *              - Bit 24 - WCDMA (Europe and China) DCS 1800 band
 *              - Bit 25 - WCDMA US 1700 band
 *              - Bit 26 - WCDMA US 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band class 12
 *              - Bit 29 - Band class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band class 15
 *              - Bits 32 through 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bits 51 through 55 - Reserved
 *              - Bit 56 - Band class 16
 *              - Bit 57 - Band class 17
 *              - Bit 58 - Band class 18
 *              - Bit 59 - Band class 19
 *              - Bit 60 - WCDMA Japan 850 band
 *              - Bit 61 - WCDMA 1500 band
 *              - Bits 62 and 63 - Reserved
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  is_LteBandCapability_Available[OUT]
 *              LTE Band Capabilities Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *                  - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  LteBandCapability[OUT]
 *               Bitmask of LTE bands supported by the device
 *                   - Bit 0 - LTE EUTRAN Band 1 UL:1920-1980; DL: 2110-2170
 *                   - Bit 1 - LTE EUTRAN Band 2 UL:1850-1910; DL: 1930-1990
 *                   - Bit 2 - LTE EUTRAN Band 3 UL:1710-1785; DL: 1805-1880
 *                   - Bit 3 - LTE EUTRAN Band 4 UL:1710-1755; DL: 2110-2155
 *                   - Bit 4 - LTE EUTRAN Band 5 UL: 824-849; DL: 869-894
 *                   - Bit 5 - LTE EUTRAN Band 6 UL: 830-840; DL: 875-885
 *                   - Bit 6 - LTE EUTRAN Band 7 UL:2500-2570; DL: 2620-2690
 *                   - Bit 7 - LTE EUTRAN Band 8 UL: 880-915; DL: 925-960
 *                   - Bit 8 - LTE EUTRAN Band 9 UL:1749.9-1784.9; DL: 1844.9-1879.9
 *                   - Bit 9 - LTE EUTRAN Band 10 UL:1710-1770; DL: 2110-2170
 *                   - Bit 10 - LTE EUTRAN Band 11 UL:1427.9-1452.9; DL: 1475.9-1500.9
 *                   - Bit 11 - LTE EUTRAN Band 12 UL:698-716; DL: 728-746
 *                   - Bit 12 - LTE EUTRAN Band 13 UL: 777-787; DL: 746-756
 *                   - Bit 13 - LTE EUTRAN Band 14 UL: 788-798; DL: 758-768
 *                   - Bits 14 and 15 - Reserved
 *                   - Bit 16 - LTE EUTRAN Band 17 UL: 704-716; DL: 734-746
 *                   - Bit 17 - LTE EUTRAN Band 18 UL: 815-830; DL: 860-875
 *                   - Bit 18 - LTE EUTRAN Band 19 UL: 830-845; DL: 875-890
 *                   - Bit 19 - LTE EUTRAN Band 20 UL: 832-862; DL: 791-821
 *                   - Bit 20 - LTE EUTRAN Band 21 UL: 1447.9-1462.9; DL: 1495.9-1510.9
 *                   - Bit 21 - Reserved
 *                   - Bit 22 - LTE EUTRAN Band 23 UL: 2000-2020; DL: 2180-2200
 *                   - Bit 23 - LTE EUTRAN Band 24 UL: 1626.5-1660.5; DL: 1525-1559
 *                   - Bit 24 - LTE EUTRAN Band 25 UL: 1850-1915; DL: 1930-1995
 *                   - Bit 25 - LTE EUTRAN Band 26 UL: 814-849; DL: 859-894
 *                   - Bit 26 - Reserved
 *                   - Bit 27 - LTE EUTRAN Band 28 UL: 703-748; DL: 758-803
 *                   - Bit 28 - LTE EUTRAN Band 29 UL: 1850-1910 or 1710-1755; DL: 716-728
 *                   - Bit 29 - LTE EUTRAN Band 30 UL: 2350-2360; DL: 2305-2315
 *                   - Bit 30 - Reserved
 *                   - Bit 31 - LTE EUTRAN Band 32 DL: 9920-10359
 *                   - Bit 32 - LTE EUTRAN Band 33 UL: 1900-1920; DL: 1900-1920
 *                   - Bit 33 - LTE EUTRAN Band 34 UL: 2010-2025; DL: 2010-2025
 *                   - Bit 34 - LTE EUTRAN Band 35 UL: 1850-1910; DL: 1850-1910
 *                   - Bit 35 - LTE EUTRAN Band 36 UL: 1930-1990; DL: 1930-1990
 *                   - Bit 36 - LTE EUTRAN Band 37 UL: 1910-1930; DL: 1910-1930
 *                   - Bit 37 - LTE EUTRAN Band 38 UL: 2570-2620; DL: 2570-2620
 *                   - Bit 38 - LTE EUTRAN Band 39 UL: 1880-1920; DL: 1880-1920
 *                   - Bit 39 - LTE EUTRAN Band 40 UL: 2300-2400; DL: 2300-2400
 *                   - Bit 40 - LTE EUTRAN Band 41 UL: 2496-2690; DL: 2496-2690
 *                   - Bit 41 - LTE EUTRAN Band 42 UL: 3400-3600; DL: 3400-3600
 *                   - Bit 42 - LTE EUTRAN Band 43 UL: 3600-3800; DL: 3600-3800
 *                   - Bits 43 through 64 - Reserved
 *                   - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  is_TdsBandCapability_Available[OUT]
 *              TDS Band Capabilities Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *                  - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  TdsBandCapability[OUT]
 *               Bitmask of TDS bands supported by the device.
 *                   - Bit 0 - TDS Band A 1900 to 1920 MHz, 2010 to 2020 MHz
 *                   - Bit 1 - TDS Band B 1850 to 1910 MHz, 1930 to 1990 MHz
 *                   - Bit 2 - TDS Band C 1910 to 1930 MHz
 *                   - Bit 3 - TDS Band D 2570 to 2620 MHz
 *                   - Bit 4 - TDS Band E 2300 to 2400 MHz
 *                   - Bit 5 - TDS Band F 1880 to 1920 MHz
 *                   - Bit to check in ParamPresenceMask - <B>17</B>
 *
 * @param  LteBandsSupport[OUT]
 *          - See @ref dms_LteBandsSupport for more information
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint64_t bandCapability;
    int is_LteBandCapability_Available;
    uint64_t LteBandCapability;
    int is_TdsBandCapability_Available;
    uint64_t TdsBandCapability;
    dms_LteBandsSupport LteBandsSupport;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSGetBandCapabilityExt_t;
/**
 * Get band capability of the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSGetBandCapability(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get band capability of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSGetBandCapability(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSGetBandCapability_t *pOutput
        );

/**
 * Get band capability of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSGetBandCapabilityExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSGetBandCapabilityExt_t *pOutput
        );
/**
 *  Gets the device capabilities structure
 *
 *  @param  maxTxChannelRate
 *          - Maximum transmission rate (in bps) supported by the device
 *          - In multi-technology devices, this value will be the greatest rate
 *            among all supported technologies
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  maxRxChannelRate
 *          - Maximum reception rate (in bps) supported by the device
 *          - In multi-technology devices, this value will be the greatest
 *            rate among all supported technologies
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  dataServiceCaCapability
 *          - CS/PS data service capability
 *              - 0 - No data services supported
 *              - 1 - Only Circuit Switched (CS) services supported
 *              - 2 - Only Packet Switched (PS) services supported
 *              - 3 - Simultaneous CS and PS
 *              - 4 - Non-simultaneous CS and PS
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  simCapability
 *          - Device SIM capability
 *              - 0 - SIM not supported
 *              - 1 - SIM supported
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  radioIfacesSize
 *          - Upon input, the maximum number of elements that the radio
 *            interface array can contain
 *          - Upon successful output, actual number of elements in the radio
 *            interface array
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  RadioIfaces[OUT]
 *          - Radio interface array. This is a structure of array containing the
 *            elements below.
 *              - See qaGobiApiTableRadioInterfaces.h for Radio Interfaces
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t  maxTxChannelRate;
    uint32_t  maxRxChannelRate;
    uint32_t  dataServiceCaCapability;
    uint32_t  simCapability;
    uint32_t  radioIfacesSize;
    uint8_t        RadioIfaces[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_GetDeviceCapabilities_t;

/**
 * Get device capability pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceCapabilities(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );
/**
 * Get device capability unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceCapabilities(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetDeviceCapabilities_t *pOutput
        );

/**
 * This structure contains the dms_devCaps parameters.
 *
 *  \param  MaxTXChannelRate
 *            - Maximum Tx channel rate in bits per second
 *
 *  \param  MaxRXChannelRate
 *            - Maximum Rx channel rate in bits per second
 *
 *  \param  DataServiceCapability
 *            - DMS_DATA_CAP_NONE - 0x00
 *            - DMS_DATA_CAP_CS_ONLY-circuit switched only - 0x01
 *            - DMS_DATA_CAP_PS_ONLY-packet switched only - 0x02
 *            - DMS_DATA_CAP_SIMUL_CS_AND_PS - 0x03
 *            - DMS_DATA_CAP_NONSIMUL_CS_AND_PS - 0x04
 *
 *  \param  SimCapability
 *            - SIM_NOT_SUPPORTED - 0x01
 *            - SIM_SUPPORTED - 0x02
 *
 *  \param  RadioIfacesSize
 *            -Number of radio interfaces
 *
 *  \param  RadioIfaces
 *            -List of radio interfaces, each byte will be one of following
 *               - DMS_RADIO_IF_1X- CDMA2000 1X - 0x01
 *               - DMS_RADIO_IF_1X_EVDO- CDMA2000 HRPD - 0x02
 *               - DMS_RADIO_IF_GSM- GSM - 0x04
 *               - DMS_RADIO_IF_UMTS - 0x05
 *               - DMS_RADIO_IF_LTE - 0x08
 *               - DMS_RADIO_IF_TD - 0x09
 *
 */
typedef struct{
    uint32_t   MaxTXChannelRate;
    uint32_t   MaxRXChannelRate;
    uint8_t   DataServiceCapability;
    uint8_t   SimCapability;
    uint8_t   RadioIfacesSize;
    uint8_t   RadioIfaces[DMS_MAX_RADIO_IFCS_SIZE];
}dms_devCaps;

/**
 * This structure contains the dms_devSubsCfgList parameters.
 *
 *  \param  MaxActive
 *          - Maximum number of subscriptions active
 *
 *  \param  SubsListLen
 *          - Length of subscription list
 *
 *  \param  SubsList
 *          - Subscription list, each array element will be one of following
 *            - DMS_SUBS_CAPABILITY_AMPS - 0x00000001
 *            - DMS_SUBS_CAPABILITY_CDMA - 0x00000002
 *            - DMS_SUBS_CAPABILITY_HDR - 0x00000004
 *            - DMS_SUBS_CAPABILITY_GSM - 0x00000008
 *            - DMS_SUBS_CAPABILITY_WCDMA - 0x00000010
 *            - DMS_SUBS_CAPABILITY_LTE - 0x00000020
 *            - DMS_SUBS_CAPABILITY_TDS - 0x00000040
 *
 */
typedef struct{
    uint8_t  MaxActive;
    uint8_t  SubsListLen;
    uint64_t SubsList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devSubsCfgList ;

/**
 * This structure contains the dms_devMultiSimCaps parameters.
 *
 *  \param  MaxSubs
 *          - Maximum number of subscriptions supported
 *
 *  \param  SubsCfgListLen
 *          - Length of subscription config list
 *
 *  \param  SubsCfgList
 *          - Subscription config list
 *            - See \ref devSubsCfgList for more information
 *
 */
typedef struct{
    uint8_t  MaxSubs;
    uint8_t  SubsCfgListLen;
    dms_devSubsCfgList SubsCfgList[DMS_MAX_SUBS_CFG_LIST_SIZE];
}dms_devMultiSimCaps ;

/**
 * This structure contains the dms_devMultiSimVoiceDataCaps parameters.
 *
 *  \param  MaxSubs
 *          - Maximum number of subscriptions supported
 *
 *  \param  MaxActive
 *          - Maximum number of subscriptions active
 *
 */
typedef struct{
    uint8_t  MaxSubs;
    uint8_t  MaxActive;
}dms_devMultiSimVoiceDataCaps ;

/**
 * This structure contains the dms_devCurSubsCaps  response parameters.
 *
 *  \param  CurSubsCapLen
 *          - Length of subscription list
 *
 *  \param  SubsCapList
 *          - Subscription cap list, each array element will be one of following
 *            - DMS_SUBS_CAPABILITY_AMPS -0x00000001
 *            - DMS_SUBS_CAPABILITY_CDMA -0x00000002
 *            - DMS_SUBS_CAPABILITY_HDR -0x00000004
 *            - DMS_SUBS_CAPABILITY_GSM -0x00000008
 *            - DMS_SUBS_CAPABILITY_WCDMA -0x00000010
 *            - DMS_SUBS_CAPABILITY_LTE -0x00000020
 *            - DMS_SUBS_CAPABILITY_TDS -0x00000040
 *
 */
typedef struct{
    uint8_t  CurSubsCapsLen;
    uint64_t SubsCapList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devCurSubsCaps ;

/**
 * This structure contains the dms_devSubsVoiceDataList parameters.
 *
 *  \param  SubsVoiceDataCap
 *          - Voice data capabilities of a subscription
 *            - DMS_SUBS_VOICE_DATA_CAPABILITY_NORMAL -0x01
 *            - DMS_SUBS_VOICE_DATA_CAPABILITY_SGLTE  - 0x02
 *            - DMS_SUBS_VOICE_DATA_CAPABILITY_CSFB -0x03
 *            - DMS_SUBS_VOICE_DATA_CAPABILITY_SVLTE -0x04
 *            - DMS_SUBS_VOICE_DATA_CAPABILITY_ SRLTE -0x05
 *
 *  \param  SimVoiceDataCap
 *          - Simultaneous Voice data capabilities of subscription
 *
 */
typedef struct{
    uint32_t  SubsVoiceDataCap;
    uint8_t   SimVoiceDataCap;
}dms_devSubsVoiceDataList ;

/**
 * This structure contains the dms_devSubsVoiceDataCaps  response parameters.
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 *
 *  \param  SubsVoiceDataCapLen
 *          - Length of subscription list
 *
 *  \param  SubsVoiceDataList
 *          - See \ref dms_devSubsVoiceDataList for more information
 *
 */
typedef struct{
    uint8_t  SubsVoiceDataCapLen;
    dms_devSubsVoiceDataList SubsVoiceDataList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devSubsVoiceDataCaps ;

/**
 * This structure contains the dms_devSubsFeatureModeCaps  response parameters.
 *
 *  \param  SubsFeatureLen
 *          - Length of subscription feature list
 *
 *  \param  SubsFeatureList
 *          - Subscription feature mode list, each array element will be one of following
 *            - DMS_DEVICE_SUBS_FEATURE_MODE_NORMAL -0
 *            - DMS_DEVICE_SUBS_FEATURE_MODE_SGLTE -1
 *            - DMS_DEVICE_SUBS_FEATURE_MODE_SVLTE -2
 *            - DMS_DEVICE_SUBS_FEATURE_MODE_SRLTE -3
 *            - DMS_DEVICE_SUBS_FEATURE_MODE_DUAL_MULTIMODE -4
 *
 */
typedef struct{
    uint8_t  SubsFeatureLen;
    uint32_t SubsFeatureList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devSubsFeatureModeCaps ;

/**
 * This structure contains the dms_devMaxSubsCaps  response parameters.
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 *
 *  \param  MaxSubsCapLen
 *          - Length of subscription list
 *
 *  \param  MaxSubsList
 *          - Subscription cap list, each array element will be one of following
 *            - DMS_SUBS_CAPABILITY_AMPS -0x00000001
 *            - DMS_SUBS_CAPABILITY_CDMA -0x00000002
 *            - DMS_SUBS_CAPABILITY_HDR -0x00000004
 *            - DMS_SUBS_CAPABILITY_GSM -0x00000008
 *            - DMS_SUBS_CAPABILITY_WCDMA -0x00000010
 *            - DMS_SUBS_CAPABILITY_LTE -0x00000020
 *            - DMS_SUBS_CAPABILITY_TDS -0x00000040
 *
 */
typedef struct{
    uint8_t  MaxSubsCapLen;
    uint64_t MaxSubsList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devMaxSubsCaps ;

/**
 * This structure contains the dms_devSubsList parameters.
 *
 *  \param  SubsListLen
 *          - Length of subscription list
 *
 *  \param  SubsList
 *          - Subscription list, each array element will be one of following
 *            - DMS_SUBS_CAPABILITY_AMPS -0x00000001
 *            - DMS_SUBS_CAPABILITY_CDMA -0x00000002
 *            - DMS_SUBS_CAPABILITY_HDR -0x00000004
 *            - DMS_SUBS_CAPABILITY_GSM -0x00000008
 *            - DMS_SUBS_CAPABILITY_WCDMA -0x00000010
 *            - DMS_SUBS_CAPABILITY_LTE -0x00000020
 *            - DMS_SUBS_CAPABILITY_TDS -0x00000040
 *
 */
typedef struct{
    uint8_t  SubsListLen;
    uint64_t SubsList[DMS_MAX_SUBS_LIST_SIZE];
}dms_devSubsList ;

/**
 * This structure contains the devMaxCfgListCaps parameters.
 *
 *  \param  MaxSubs
 *          - Maximum number of subscriptions supported
 *
 *  \param  MaxActive
 *          - Maximum number of subscriptions active
 *
 *  \param  DevCfgListLen
 *          - Length of subscription config list
 *
 *  \param  SubsDevList
 *          - Subscription config list
 *            - See \ref dms_devSubsList for more information
 *
 *  \param  CurIndex
 *          - Current Config Index
 *
 */
typedef struct{
    uint8_t  MaxSubs;
    uint8_t  MaxActive;
    uint8_t  DevCfgListLen;
    dms_devSubsList SubsDevList[DMS_MAX_SUBS_CFG_LIST_SIZE];
    uint8_t  CurIndex;
}dms_devMaxCfgListCaps ;


/**
 *  This structure contains the unpack_dms_GetDeviceCapabilitiesV2 response parameters.
 *  \param  DevCaps
 *          - See \ref dms_devCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  pDevSrvCaps
 *          - Device service capabilities
 *             - DMS_DEVICE_CAP_DATA_ONLY -0x01
 *             - DMS_DEVICE_CAP_VOICE_ONLY -0x02
 *             - DMS_DEVICE_CAP_SIMUL_VOICE_AND_DATA -0x03
 *             - DMS_DEVICE_CAP_NONSIMUL_VOICE_AND_DATA -0x04
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pDevVoiceCaps
 *           - Device voice capabilities
 *             - Bit 0 – GW CSFB
 *               - 0 – Not capable
 *               - 1 – Capable
 *             - Bit 1 – 1x CSFB
 *               - 0 – Not capable
 *               - 1 – Capable
 *             - Bit 2 – VoLTE
 *               - 0 – Not capable
 *               - 1 – Capable
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pDevVoiceDataCaps
 *          - DevVoiceDataCaps
 *           - Bit 0 – SVLTE capability
 *           - Bit 1 – SVDO capability
 *           - Bit 2 – SGLTE capability
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pDevMultiSimCaps
 *          - See \ref dms_devMultiSimCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pDevMultiSimVoiceDataCaps
 *          - See \ref dms_devMultiSimVoiceDataCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  pDevCurSubsCaps
 *          - See \ref dms_devCurSubsCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  pDevSubsVoiceDataCaps
 *          - See \ref dms_devSubsVoiceDataCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  pDevSubsFeatureModeCaps
 *          - See \ref dms_devSubsFeatureModeCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  pDevMaxActDataSubsCaps
 *          - Max number of subscriptions for data activity
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  pDevMaxSubsCaps
 *          - See \ref dms_devMaxSubsCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  \param  pDevMaxActDataSubsCaps
 *          - See \ref dms_devMaxCfgListCaps for more information
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  \param  pDevExplicitCfgIndex
 *          - Explicit Cfg Index
 *            - (-1) - Modem controlled cfg
 *            - any other valid value
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  \param Tlvresult
 *          - unpack Tlv Result
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    dms_devCaps                      DevCaps;
    uint32_t                         *pDevSrvCaps;
    uint64_t                         *pDevVoiceCaps;
    uint64_t                         *pDevVoiceDataCaps;
    dms_devMultiSimCaps              *pDevMultiSimCaps;
    dms_devMultiSimVoiceDataCaps     *pDevMultiSimVoiceDataCaps;
    dms_devCurSubsCaps               *pDevCurSubsCaps;
    dms_devSubsVoiceDataCaps         *pDevSubsVoiceDataCaps;
    dms_devSubsFeatureModeCaps       *pDevSubsFeatureModeCaps;
    uint8_t                          *pDevMaxActDataSubsCaps;
    dms_devMaxSubsCaps               *pDevMaxSubsCaps;
    dms_devMaxCfgListCaps            *pDevMaxCfgListCaps;
    int16_t                          *pDevExplicitCfgIndex;
    uint16_t                         Tlvresult;
    swi_uint256_t                    ParamPresenceMask;
}unpack_dms_GetDeviceCapabilitiesV2_t;

/**
 * Get device capability pack v2
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceCapabilitiesV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );
/**
 * Get device capability unpack v2
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceCapabilitiesV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_GetDeviceCapabilitiesV2_t *pOutput
        );

/**
 * This structure used to store unpack_dms_GetFirmwareRevisions parameters
 *
 *  @param  amssSize
 *          - Maximum number of characters (including NULL terminator) that
 *            the AMSS string array can contain
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  AMSSString
 *          - NULL-terminated AMSS revision string
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  bootSize
 *          - Maximum number of characters (including NULL terminator) that
 *            the boot string array can contain
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  BootString
 *          - NULL-terminated boot code revision string
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  priSize
 *          - Maximum number of characters (including NULL terminator) that
 *            the PRI string array can contain
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  PRIString
 *          - NULL-terminated PRI revision string
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t    amssSize;
    char    AMSSString[DMS_UINT8_MAX_STRING_SZ];
    uint8_t    bootSize;
    char    BootString[DMS_UINT8_MAX_STRING_SZ];
    uint8_t    priSize;
    char    PRIString[DMS_UINT8_MAX_STRING_SZ];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetFirmwareRevisions_t;
        
        
/**
 * Get Firmware Revisions pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
    
int pack_dms_GetFirmwareRevisions(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Firmware Revisions unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetFirmwareRevisions(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetFirmwareRevisions_t *pOutput
);

/**
 * This structure used to store unpack_dms_GetFirmwareRevision parameters
 *
 *  @param  amssSize
 *          - Maximum number of characters (including NULL terminator) that
 *            the AMSS string array can contain.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  AMSSString
 *          - NULL-terminated AMSS revision string.
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  PRIString[OUT]
 *          - NULL-terminated PRI revision string.
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t    amssSize;
    char    AMSSString[DMS_UINT8_MAX_STRING_SZ];
    char    PRIString[DMS_UINT8_MAX_STRING_SZ];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetFirmwareRevision_t;
        
        
/**
 * Get Firmware Revision pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
    
int pack_dms_GetFirmwareRevision(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
    
);

/**
 * Get Firmware Revision unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetFirmwareRevision(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetFirmwareRevision_t *pOutput
);
/**
 * This structure used to store unpack_dms_GetDeviceSerialNumbers parameters
 *
 *  @param  esnSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the ESN string array can contain
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ESNString
 *          - NULL-terminated ESN string. Empty string is returned when ESN is
 *            not supported/programmed
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  imeiSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the IMEI string array can contain
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  IMEIString
 *          - NULL terminated IMEI string. Empty string is returned when IMEI
 *            is not supported/programmed
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  meidSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the MEID string array can contain
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  MEIDString
 *          - NULL-terminated MEID string. Empty string is returned when MEID
 *            is not supported/programmed
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  imeiSvnSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the IMEI SVN string array can contain.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ImeiSvnString
 *          - NULL-terminated IMEI SVN string. Empty string is returned when IMEI
 *            SVN is not supported/programmed.
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t    esnSize;
    char       ESNString[DMS_UINT8_MAX_STRING_SZ];
    uint8_t    imeiSize;
    char       IMEIString[DMS_UINT8_MAX_STRING_SZ];
    uint8_t    meidSize;
    char       MEIDString[DMS_UINT8_MAX_STRING_SZ];
    uint8_t    imeiSvnSize;
    char       ImeiSvnString[DMS_UINT8_MAX_STRING_SZ];
    uint16_t   Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetDeviceSerialNumbers_t;


/**
 * Get Device Serial Number pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceSerialNumbers(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);


/**
 * Get Device Serial Number unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceSerialNumbers(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetDeviceSerialNumbers_t *pOutput
);

/**
 * This structure used to store unpack_dms_GetPRLVersion parameters
 *
 *  @param  u8PRLPreference
 *          -  PRL Preference
 *              - 0 - Unset
 *              - 1 - Set
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  u16PRLVersion
 *          - PRL version of device.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t    u8PRLPreference;
    uint16_t   u16PRLVersion;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetPRLVersion_t;

/**
 * Get PRL Versions pack.
 * Return version of the active Preferred Roaming List (PRL) in use by
 * the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetPRLVersion(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get PRL Versions unpack
 * Return version of the active Preferred Roaming List (PRL) in use by
 * the device.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetPRLVersion(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetPRLVersion_t *pOutput
);

/**
 * This structure used to store unpack_dms_GetNetworkTime parameters
 *
 *  @param  source
 *          - Source of timestamp\n
 *            0 - 32 kHz device clock\n
 *            1 - CDMA network\n
 *            2 - cdma2000 1xEV-DO network
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param  timestamp
 *          - Count of 1.25 ms that have elapsed from the start of GPS time
 *            (Jan 6, 1980)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  @note   The source of the timestamp provided specifies how the timestamp
 *          was determined. The first network time that is available will be
 *          returned. If no network time is available, the timestamp is taken
 *          from the 32 kHz slow-clock of the device.
 */
typedef struct{
    uint16_t source;
    uint64_t timestamp;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetNetworkTime_t;

/**
 * Get Network Time pack
 * Returns the current time of the device based on the value supported by the
 * network.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetNetworkTime(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Network Time unpack.
 * Returns the current time of the device based on the value supported by the
 * network.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetNetworkTime(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetNetworkTime_t *pOutput
);


/**
 * This structure used to store unpack_dms_GetNetworkTimeV2 parameters
 *
 *  @param  source
 *          - Source of timestamp\n
 *            0 - 32 kHz device clock\n
 *            1 - CDMA network\n
 *            2 - cdma2000 1xEV-DO network
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  timestamp
 *          - Count of 1.25 ms that have elapsed from the start of GPS time
 *            (Jan 6, 1980)
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSysTime
 *          - Count of system time in ms that have elapsed from the start of GPS time
 *            (Jan 6, 1980)
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pUsrTime
 *          - Count of user time in ms that have elapsed from the start of GPS time
 *            (Jan 6, 1980)
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  @note   The source of the timestamp provided specifies how the timestamp
 *          was determined. The first network time that is available will be
 *          returned. If no network time is available, the timestamp is taken
 *          from the 32 kHz slow-clock of the device.
 */
typedef struct{
    uint16_t source;
    uint64_t timestamp;
    uint64_t *pSysTime;
    uint64_t *pUsrTime;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetNetworkTimeV2_t;

/**
 * Get Network Time pack
 * Returns the current time of the device based on the value supported by the
 * network.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetNetworkTimeV2(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Network Time unpack.
 * Returns the current time of the device based on the value supported by the
 * network.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetNetworkTimeV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetNetworkTimeV2_t *pOutput
);
/**
 *  Structure to store the voice number in use by the device
 *
 *  @param  voiceNumberSize
 *          - Maximum number of characters (including NULL terminator) that the
 *            voice number array can contain.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  VoiceNumber
 *          - Voice number string: MDN or MS ISDN
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  minSize
 *          - Maximum number of characters (including NULL terminator) that the
 *            MIN array can contain.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  MIN
 *          - Optional Parameter
 *          - MIN string: Empty string returned when MIN is not supported/
 *            programmed.
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t voiceNumberSize;
    char    VoiceNumber[DMS_UINT8_MAX_STRING_SZ];
    uint8_t minSize;
    char    MIN[DMS_UINT8_MAX_STRING_SZ];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetVoiceNumber_t;

/**
 * Get Voice Number pack.
 * Returns the voice number in use by the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetVoiceNumber(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Voice Number unpack
 * Returns the voice number in use by the device.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetVoiceNumber(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetVoiceNumber_t *pOutput
);


/**
 *  Structure to store the hardware revision of the device
 *
 *  @param  stringSize
 *          - The maximum number of characters (including NULL terminator) that
 *            the string array can contain
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  String
 *          - NULL terminated Hardware Revision string
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t    stringSize;
    char       String[DMS_UINT8_MAX_STRING_SZ];
    uint16_t   Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetDeviceHardwareRev_t;

/**
 * Get Hardware Revision of the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceHardwareRev(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Hardware Revision of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceHardwareRev(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetDeviceHardwareRev_t *pOutput
);

/**
 *  This structure used to store Factory Sequence Number parameter
 *
 *  @param  String
 *          - Facorty Sequence Number
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    char      String[DMS_UINT8_MAX_STRING_SZ];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetFSN_t;

/**
 * Get FSN pack.
 * This API get the Factory Sequence Number of the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetFSN(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get FSN unpack
 * This API get the Factory Sequence Number of the device.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetFSN(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetFSN_t *pOutput
);

/**
 *  Structure to store the device capabilities
 *
 *  @param  MaxTXChannelRate
 *          - Maximum transmission rate (in bps) supported by the device
 *          - In multi-technology devices, this value will be the greatest rate
 *            among all supported technologies
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  MaxRXChannelRate
 *          - Maximum reception rate (in bps) supported by the device
 *          - In multi-technology devices, this value will be the greatest
 *            rate among all supported technologies
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  DataServiceCapability
 *          - CS/PS data service capability
 *              - 0 - No data services supported
 *              - 1 - Only Circuit Switched (CS) services supported
 *              - 2 - Only Packet Switched (PS) services supported
 *              - 3 - Simultaneous CS and PS
 *              - 4 - Non-simultaneous CS and PS
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  SimCapability
 *           Device SIM capability
 *              - 0 - SIM not supported
 *              - 1 - SIM supported
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  RadioIfacesSize
 *          - Upon input, the maximum number of elements that the radio
 *            interface array can contain
 *          - Upon successful output, actual number of elements in the radio
 *            interface array
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  RadioIfaces
 *          - Radio interface array. This is a structure of array containing the
 *            elements below.\n
 *            uint8_t  RadioIfaces
 *              - See qaGobiApiTableRadioInterfaces.h for Radio Interfaces
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint32_t MaxTXChannelRate;
    uint32_t MaxRXChannelRate;
    uint32_t DataServiceCapability;
    uint32_t SimCapability;
    uint32_t RadioIfacesSize;
    uint8_t  RadioIfaces[DMS_MAX_RADIO_IFCS_SIZE];
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetDeviceCap_t;

/**
 * Get Device Capabilities pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceCap(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Device Capabilities unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceCap(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetDeviceCap_t *pOutput
);


#define DMS_PM_ONLINE         0x00 /* Online */
#define DMS_PM_LOW            0x01 /* Low Power */
#define DMS_PM_FACTORY        0x02 /* Factory Test Mode */
#define DMS_PM_OFFLINE        0x03 /* Offline */
#define DMS_PM_RESET          0x04 /* Reset */
#define DMS_PM_SHUT_DOWN      0x05 /* Shut Down */
#define DMS_PM_PERSISTENT_LOW 0x06 /* Persistent Low Power */


/**
 *  Structure to store pack the operating mode of the device.
 *
 *  @param  mode
 *          - Selected operating mode
 *             - See qaGobiApiTablePowerModes.h for power modes
 *  @param Tlvresult 
 *          - pack Tlv Result.
 *
 */
typedef struct{
    uint32_t  mode;
    uint16_t  Tlvresult;
}pack_dms_SetPower_t;

/**
 *  Structure to store unpack the operating mode of the device.
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetPower_t;

/**
 * Set Power pack
 *  Sets the operating mode of the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetPower(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetPower_t *reqArg
);

/**
 * Set Power unpack.
 * Sets the operating mode of the device.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetPower(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetPower_t *pOutput
);

/**
 *  Structure to store the band capability of the device.
 *
 *  @param  BandCapability
 *          Bitmask of bands supported by the device
 *              - Bit 0 - Band class 0, A-system
 *              - Bit 1 - Band class 0, B-system
 *              - Bit 2 - Band class 1, all blocks
 *              - Bit 3 - Band class 2
 *              - Bit 4 - Band class 3, A-system
 *              - Bit 5 - Band class 4, all blocks
 *              - Bit 6 - Band class 5, all blocks
 *              - Bit 7 - GSM DCS band (1800)
 *              - Bit 8 - GSM Extended GSM (E-GSM) band (900)
 *              - Bit 9 - GSM Primary GSM (P-GSM) band (900)
 *              - Bit 10 - Band class 6
 *              - Bit 11 - Band class 7
 *              - Bit 12 - Band class 8
 *              - Bit 13 - Band class 9
 *              - Bit 14 - Band class 10
 *              - Bit 15 - Band class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM railways GSM band (900)
 *              - Bit 21 - GSM PCS band (1900)
 *              - Bit 22 - WCDMA (Europe, Japan, and China) 2100 band
 *              - Bit 23 - WCDMA US PCS 1900 band
 *              - Bit 24 - WCDMA (Europe and China) DCS 1800 band
 *              - Bit 25 - WCDMA US 1700 band
 *              - Bit 26 - WCDMA US 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band class 12
 *              - Bit 29 - Band class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band class 15
 *              - Bits 32 through 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bits 51 through 55 - Reserved
 *              - Bit 56 - Band class 16
 *              - Bit 57 - Band class 17
 *              - Bit 58 - Band class 18
 *              - Bit 59 - Band class 19
 *              - Bit 60 - WCDMA Japan 850 band
 *              - Bit 61 - WCDMA 1500 band
 *              - Bits 62 and 63 - Reserved
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint64_t BandCapability;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetBandCapability_t;

/**
 * Get Band Capability pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetBandCapability(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Band Capabilities unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetBandCapability(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetBandCapability_t *pOutput
);

/**
 *  This structure is used to store retrieved USB Composition
 *
 *  @param  USBComp
 *          - Current USB Composition(optional parameter)
 *          - Values:
 *              - 0..5 - Reserved (non-QMI)
 *              - 6 - DM NMEA AT QMI
 *              - 7 - DM NMEA AT QMI1 QMI2 QMI3
 *              - 8 - DM NMEA AT MBIM
 *              - 9 - MBIM
 *              - 10 - NMEA MBIM
 *              - 11 - DM MBIM
 *              - 12 - DM NMEA MBIM
 *          13-22 are combined compositions. One is for Win8 MBIM interfaces,
 *          another is for legacy QMI interfaces
 *              - 13 - 6 for QMI, 8 for MBIM
 *              - 14 - 6 for QMI, 9 for MBIM
 *              - 15 - 6 for QMI, 10 for MBIM
 *              - 16 - 6 for QMI, 11 for MBIM
 *              - 17 - 6 for QMI, 12 for MBIM
 *              - 18 - 7 for QMI, 8 for MBIM
 *              - 19 - 7 for QMI, 9 for MBIM
 *              - 20 - 7 for QMI, 10 for MBIM
 *              - 21 - 7 for QMI, 11 for MBIM
 *              - 22 - 7 for QMI, 12 for MBIM
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  NumSupUSBComps
 *          - Number of supported USB compositions in the parameter to follow
 *          - Range - 0-255
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  SupUSBComps
 *          - Optional parameter
 *          - List of supported USB compositions( 1 Byte each - Max 255 )
 *          - Total length is defined by pNumSupUSBComps parameter
 *          - Values:
 *              - 0..5 - Reserved (non-QMI)
 *              - 6 - DM NMEA AT QMI
 *              - 7 - DM NMEA AT QMI1 QMI2 QMI3
 *              - 8 - DM NMEA AT MBIM
 *              - 9 - MBIM
 *              - 10 - NMEA MBIM
 *              - 11 - DM MBIM
 *              - 12 - DM NMEA MBIM \n
 *          13-22 are combined compositions. One is for Win8 MBIM interfaces,
 *          another is for legacy QMI interfaces
 *              - 13 - 6 for QMI, 8 for MBIM
 *              - 14 - 6 for QMI, 9 for MBIM
 *              - 15 - 6 for QMI, 10 for MBIM
 *              - 16 - 6 for QMI, 11 for MBIM
 *              - 17 - 6 for QMI, 12 for MBIM
 *              - 18 - 7 for QMI, 8 for MBIM
 *              - 19 - 7 for QMI, 9 for MBIM
 *              - 20 - 7 for QMI, 10 for MBIM
 *              - 21 - 7 for QMI, 11 for MBIM
 *              - 22 - 7 for QMI, 12 for MBIM
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint8_t  USBComp;
    uint8_t  NumSupUSBComps;
    uint8_t  SupUSBComps[255];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetUSBComp_t;

/**
 * Get USB Comp pack
 * This API queries the modem's USB interface configuration and supported
 * configuration parameters.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetUSBComp(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get USB Comp unpack
 * This API queries the modem's USB interface configuration and supported
 * configuration parameters.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetUSBComp(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetUSBComp_t *pOutput
);

/**
 *  This structure is used to store USB composition information pack paramters
 *
 *  @param  USBComp
 *          - Current USB Composition
 *          - Values:
 *              - 0..5 - Reserved (non-QMI)
 *              - 6 - DM NMEA AT QMI
 *              - 7 - DM NMEA AT QMI1 QMI2 QMI3
 *              - 8 - DM NMEA AT MBIM
 *              - 9 - MBIM
 *              - 10 - NMEA MBIM
 *              - 11 - DM MBIM
 *              - 12 - DM NMEA MBIM \n
 *          13-22 are combined compositions. One is for Win8 MBIM interfaces,
 *          another is for legacy QMI interfaces
 *              - 13 - 6 for QMI, 8 for MBIM
 *              - 14 - 6 for QMI, 9 for MBIM
 *              - 15 - 6 for QMI, 10 for MBIM
 *              - 16 - 6 for QMI, 11 for MBIM
 *              - 17 - 6 for QMI, 12 for MBIM
 *              - 18 - 7 for QMI, 8 for MBIM
 *              - 19 - 7 for QMI, 9 for MBIM
 *              - 20 - 7 for QMI, 10 for MBIM
 *              - 21 - 7 for QMI, 11 for MBIM
 *              - 22 - 7 for QMI, 12 for MBIM
 *
 *  @param Tlvresult 
 *          - pack Tlv Result.
 *
 */
typedef struct{
    uint8_t  USBComp;
    uint16_t  Tlvresult;
}pack_dms_SetUSBComp_t;

/**
 *  This structure is used to store unpack USB composition information
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetUSBComp_t;

/**
 * Set USB Comp pack
 * This API is used to change the modem's USB interface configuration thus
 * allowing a device to have multiple USB compositions. Devices will,
 * by default, be configured to support a minimal set of interfaces to reduce
 * end user modem installation time. Developers and some customers, however,
 * require access to a custom set of interfaces. A reset is required for any
 * change in the USB composition to take effect.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetUSBComp(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetUSBComp_t *reqArg
);

/**
 * Set USB Comp unpack
 * This API is used to change the modem's USB interface configuration thus
 * allowing a device to have multiple USB compositions. Devices will,
 * by default, be configured to support a minimal set of interfaces to reduce
 * end user modem installation time. Developers and some customers, however,
 * require access to a custom set of interfaces. A reset is required for any
 * change in the USB composition to take effect.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetUSBComp(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetUSBComp_t *pOutput
);

/**
 *  This structure contains current settings of custom features pack parameters
 *
 *  @param  GpsEnable
 *          - describes if GPS is enabled or disabled
 *          - values:
 *              - 0x00 - GPS is disabled
 *              - 0x01 - GPS is enabled
 *
 *  @param  DisableIMSI
 *          - optional 1 byte parameter
 *          - describes if IMSI display is enabled or disabled
 *          - values:
 *              - 0x00 - Allow display of IMSI
 *              - 0x01 - Do not display IMSI
 *
 *  @param  IPFamSupport
 *          - optional 2 byte BitMask
 *          - bitmask representing the IP families supported
 *          - values:
 *              - 0x01 - IPv4
 *              - 0x02 - IPv6
 *              - 0x04 - IPv4v6
 *
 *  @param  RMAutoConnect
 *          - optional 1 byte parameter
 *          - QMI Mode RM Net Auto Connect Support
 *          - values:
 *              - 0x00 - Not Supported
 *              - 0x01 - Supported
 *
 *  @param  GPSSel
 *          - optional 1 byte parameter
 *          - GPS Antenna Select
 *          - values:
 *              - 0x00 - Dedicated GPS Port
 *              - 0x01 - GPS Rx over AUX Port
 *              - 0x02 - GPS Rx over dedicated GPS port with no bias
 *                       voltage applied
 *
 *  @param  SMSSupport
 *          - optional 1 byte parameter
 *          - SMS support
 *          - values:
 *              - 0x00 - Not supported
 *              - 0x01 - supported
 *          - Used to determine whether or not to hide SMS from user
 *
 *  @param  IsVoiceEnabled
 *          - optional 1 byte parameter
 *          - Voice support
 *          - values:
 *              - 0x00 - Enable voice on both AT and QMI interface (default)
 *              - 0x01 - Reserved
 *              - 0x02 - Disable voice on both AT and QMI interface
 *
 *  @param  DHCPRelayEnabled
 *          - optional 1 byte parameter
 *          - DHCP Relay support
 *          - values:
 *              - 0x00 - Disable DHCP relay
 *              - 0x01 - Enable DHCP relay
 *
 *  @param  GPSLPM
 *          - optional 1 byte parameter
 *          - GPSLPM support
 *          - values:
 *              - 0x00 - Enable GPS in Low Power Mode
 *              - 0x01 - Disable GPS in Low Power Mode
 *
 */
typedef struct{
    uint32_t GpsEnable;
    uint8_t  DisableIMSI;
    uint16_t  IPFamSupport;
    uint8_t  RMAutoConnect;
    uint8_t  GPSSel;
    uint8_t  SMSSupport;
    uint8_t  IsVoiceEnabled;
    uint8_t  DHCPRelayEnabled;
    uint8_t  GPSLPM;
}pack_dms_SetCustFeature_t;

/**
 *  This structure is used to store unpack_dms_SetCustFeature parameters
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetCustFeature_t;

/**
 * Set Custom Feature pack.
 * This API is deprecated for EM74xx/MC74xx,
 * please use pack_dms_SetCustFeaturesV2() instead for EM74xx/MC74xx.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetCustFeature(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetCustFeature_t *reqArg
);

/**
 * Set Custom Feature unpack.
 * This API is deprecated for EM74xx/MC74xx, 
 * please use unpack_dms_SetCustFeaturesV2() instead for EM74xx/MC74xx.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetCustFeature(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetCustFeature_t *pOutput
);


/**
 *  This structure contains current settings of custom features
 *
 *  @param  GpsEnable
 *          - describes if GPS is enabled or disabled
 *          - values:
 *              - 0x00 - GPS is disabled
 *              - 0x01 - GPS is enabled
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  DisableIMSI
 *          - optional 1 byte parameter
 *          - describes if IMSI display is enabled or disabled
 *          - values:
 *              - 0x00 - Allow display of IMSI
 *              - 0x01 - Do not display IMSI
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  IPFamSupport
 *          - optional 2 byte BitMask
 *          - bitmask representing the IP families supported
 *          - values:
 *              - 0x01 - IPv4
 *              - 0x02 - IPv6
 *              - 0x04 - IPv4v6
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  RMAutoConnect
 *          - optional 1 byte parameter
 *          - QMI Mode RM Net Auto Connect Support
 *          - values:
 *              - 0x00 - Not Supported
 *              - 0x01 - Supported
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  GPSSel
 *          - optional 1 byte parameter
 *          - GPS Antenna Select
 *          - values:
 *              - 0x00 - Dedicated GPS Port
 *              - 0x01 - GPS Rx over AUX Port
 *              - 0x02 - GPS Rx over dedicated GPS port with no bias
 *                       voltage applied
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  SMSSupport
 *          - optional 1 byte parameter
 *          - SMS support
 *          - values:
 *              - 0x00 - Not supported
 *              - 0x01 - supported
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *          - Used to determine whether or not to hide SMS from user
 *
 *  @param  IsVoiceEnabled
 *          - optional 1 byte parameter
 *          - Voice support
 *          - values:
 *              - 0x00 - Enable voice on both AT and QMI interface (default)
 *              - 0x01 - Reserved
 *              - 0x02 - Disable voice on both AT and QMI interface
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  DHCPRelayEnabled
 *          - optional 1 byte parameter
 *          - DHCP Relay support
 *          - values:
 *              - 0x00 - Disable DHCP relay
 *              - 0x01 - Enable DHCP relay
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  GPSLPM
 *          - optional 1 byte parameter
 *          - GPSLPM support
 *          - values:
 *              - 0x00 - Enable GPS in Low Power Mode
 *              - 0x01 - Disable GPS in Low Power Mode
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param Tlvresult 
 *          - unpack Tlv Result.
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint32_t GpsEnable;
    uint8_t  DisableIMSI;
    uint16_t  IPFamSupport;
    uint8_t  RMAutoConnect;
    uint8_t  GPSSel;
    uint8_t  SMSSupport;
    uint8_t  IsVoiceEnabled;
    uint8_t  DHCPRelayEnabled;
    uint8_t  GPSLPM;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetCustFeature_t;

/**
 * Get Custom Feature pack. 
 * This API is deprecated for EM74xx/MC74xx, 
 * please use pack_dms_GetCustFeaturesV2() instead for EM74xx/MC74xx.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetCustFeature(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Custom Feature unpack. 
 * This API is deprecated for EM74xx/MC74xx, 
 * please use unpack_dms_GetCustFeaturesV2() instead for EM74xx/MC74xx.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetCustFeature(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetCustFeature_t *pOutput
);

/**
 *  This structure is used to store unpack_dms_SetFirmwarePreference parameters
 *
 *  @param  Tlvresult
 *          - unpack Tlv result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetFirmwarePreference_t;

/**
 * Set Firmware Preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetFirmwarePreference(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Set Firmware Preference unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetFirmwarePreference(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetFirmwarePreference_t *pOutput
);

/**
 *  This structure is used to store the Crash State from the device.
 *
 *  @param  DevCrashState
 *          - Device Crash State
 *          - Values:
 *               - 0 - USB Memory Download
 *                     Modem will reset after a crash and will stay in
 *                     USB download mode with only ttyUSB0 enumerated.
 *                     ramdump tool is to be used to recover the crash dump.
 *                     Modem needs to be reset again to come back in ONLINE mode.
 *               - 1 - Reset
 *                     Modem will reset and come back in ONLINE mode.
 *                     Minimal crash data will be available and can be extracted
 *                     with at!gcdump? AT command or SLQSSwiGetCrashInfo() SDK API
 *               - 2 - No action
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack Tlv result
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t DevCrashState;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetCrashAction_t;

/**
 * Get Crash Action pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetCrashAction(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Crash Action unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetCrashAction(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetCrashAction_t *pOutput
);

/**
 *  This structure is used to store pack_dms_SetCrashAction parameters.
 *
 *  \param  crashAction
 *          - 0 - USB Memory Download. Modem will reset after a crash and will stay in USB download mode with only DM port enumerated. 
 *          - 1 - Reset. Modem will reset and come back in ONLINE mode. Minimal crash data will be available and can be extracted with at!gcdump? AT command
 *          - 2 - No Action
 *
 */
typedef struct{
    uint8_t crashAction;
}pack_dms_SetCrashAction_t;

/**
 * Set Crash Action pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetCrashAction(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetCrashAction_t reqArg
);

/**
 *  This structure is used to store unpack_dms_SetCrashAction parameters
 *
 *  \param  notused
 *           - unpack Tlv result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t notused;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetCrashAction_t;

/**
 * Set Crash Action unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response. Not used
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetCrashAction(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetCrashAction_t *pOutput
);

/**
 *  This structure is used to store unpack_dms_GetDeviceMfr parameters
 *
 *  \param  stringSize
 *           - The maximum number of characters (including NULL terminator) that
 *            the string array can contain
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  String
 *          - NULL terminated Device Manufacture string
 *          - Maximum Length is 255 Bytes
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *           - unpack Tlv result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint8_t stringSize;
    char    String[DMS_UINT8_MAX_STRING_SZ];
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetDeviceMfr_t;

/**
 * Get Device Manufacture Name pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetDeviceMfr(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    void     *reqArg
);

/**
 * Get Device Manufacture Name unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetDeviceMfr(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetDeviceMfr_t *pOutput
);

#define DMS_SET_REPORT_ENABLE 1
#define DMS_SET_REPORT_DISABLE 0

#define DMS_SWI_SET_IND_ENABLE  1
#define DMS_SWI_SET_IND_DISABLE 0

#define DMS_SET_REG_IND_ENABLE  1
#define DMS_SET_REG_IND_DISABLE 0
#define DMS_SET_REG_IND_NO_CHANGE  2

/**
 *  This structure is used to store pack_dms_SetEventReport parameters.
 *
 *  \param  mode
 *           - Operating Mode
 *           - 0 - reset
 *           - 1 - set
 *           - 2 - no change
 *
 */
typedef struct{
    uint8_t mode;
}pack_dms_SetEventReport_t;

/**
 *  This structure is used to store unpack_dms_SetEventReport parameters.
 *
 *  \param  Tlvresult
 *           - unpack Tlv result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetEventReport_t;

/**
 * Set Event Report pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetEventReport(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetEventReport_t *reqArg
);

/**
 * Set Event Report unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetEventReport(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetEventReport_t *pOutput
);

/**
 *  Operating Mode Tlv
 *
 *  \param TlvPresent
 *          - Tlv Present
 *
 *  \param  operatingMode
 *          - 0 - Online
 *          - 1 - Low power
 *          - 2 - Factory test mode
 *          - 3 - Offline
 *          - 4 - Resetting
 *          - 5 - Shutting down
 *          - 6 - Persistent low power
 *          - 7 - Mode-only low power
 *
 */
typedef struct 
{
    uint16_t    TlvPresent;
    uint32_t   operatingMode;
}dms_OperatingModeTlv;

/**
 *  Activation Status Tlv
 *
 *  \param TlvPresent
 *          - Tlv Present
 *
 *  \param  activationStatus
 *          - Service Activation Code
 *              - 0 - Service not activated
 *              - 1 - Service activated
 *              - 2 - Activation connecting
 *              - 3 - Activation connected
 *              - 4 - OTASP security authenticated
 *              - 5 - OTASP NAM downloaded
 *              - 6 - OTASP MDN downloaded
 *              - 7 - OTASP IMSI downloaded
 *              - 8 - OTASP PRL downloaded
 *              - 9 - OTASP SPC downloaded
 *              - 10 - OTASP settings committed
 *
 */
typedef struct 
{
    uint16_t    TlvPresent;
    uint32_t    activationStatus;
}dms_ActivationStatusTlv;

/**
 *  DMS Event Report indication structure
 *
 *  @param  ActivationStatusTlv
 *          - See \ref dms_ActivationStatusTlv
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  OperatingModeTlv
 *          - See \ref dms_OperatingModeTlv
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    dms_ActivationStatusTlv ActivationStatusTlv;
    dms_OperatingModeTlv OperatingModeTlv;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetEventReport_ind_t;

/**
 * Event Report Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SetEventReport_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SetEventReport_ind_t *pOutput
);

/**
 * This structure contains UIM Get ICCID pack
 *
 * \param Tlvresult
 *        - Pack result.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}pack_dms_UIMGetICCID_t;

/**
 * This structure is used to store unpack_dms_UIMGetICCID parameters.
 *
 * \param stringSize
 *        - Size of String.
 *        - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * \param String
 *        - ICCID String.
 *        - Maximum Length is 255 Bytes
 *        - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * \param Tlvresult
 *        - Unack result.
 * \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
 
typedef struct{
    uint8_t stringSize;
    uint8_t String[DMS_UINT8_MAX_STRING_SZ];
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_UIMGetICCID_t;

/**
 * Packs the UIMGetICCID response message to a user-provided response structure. 
 * This API is deprecated on MC73xx/EM73xx modules. Since firmware version SWI9X15C_05_xx_xx_xx 
 * and all EM74xx firmware versions. Please use pack_uim_ReadTransparent()(EF ID: 3F00 2FE2) instead 
 * for new firmware versions and new modules.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMGetICCID(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMGetICCID_t *reqArg
);

/**
 * Unpacks the UIMGetICCID response message to a
 * user-provided response structure.
 * @param [in] pResp qmi response
 * @param [in] respLen qmi response length
 * @param [out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMGetICCID(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMGetICCID_t *pOutput
);

/**
 *  This structure contains customization settings set to modem pack
 *
 *  \param  cust_id
 *          - Customization ID (Maximum 64 bytes)
 *          - NULL terminated ASCII string.
 *
 *  \param  value_length
 *          - length of cust_value field
 *
 *  \param  cust_value
 *          - Customization Setting Value (Maximum 8 bytes)
 *
 *  \param  Tlvresult
 *          - Pack Result
 */

typedef struct
{
    uint8_t cust_id[DMS_MAX_CUST_ID_LEN+1];
    uint16_t value_length;
    uint8_t cust_value[DMS_MAX_CUST_VALUE_LEN+1];
    uint16_t Tlvresult;
} pack_dms_SetCustFeaturesV2_t;

/**
 * Set Cust Features pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetCustFeaturesV2( 
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetCustFeaturesV2_t *reqArg);

/**
 *  This structure contains customization settings set to modem unpack
 *
 *  @param  Tlvresult
 *          - Unpack Result
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct 
{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetCustFeaturesV2_t;

/**
 * Set Cust features unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetCustFeaturesV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetCustFeaturesV2_t *pOutput
);

/**
 *  This structure contains which customization id or the list type want to retrieve
 *  from modem. This TLV is only applicable for 9x30 modules so far
 *
 *  \param  cust_id
 *          - Customization ID (Maximum 64 bytes)
 *          - NULL terminated ASCII string.
 * 
 *  \param  list_type
 *          - list type requested
 *
 *  \param  Tlvresult
 *          - Pack Result
 *
 */
typedef struct
{
    uint8_t cust_id[DMS_MAX_CUST_ID_LEN+1];
    uint8_t list_type;
    uint16_t Tlvresult;
} pack_dms_GetCustFeaturesV2_t;

/**
 * Get Custom Feature pack
 *  This function queries the modem for a list of supported features. This function is for
 *  firmware version 2.0 and newer. Currently supported Customization features: 
 *     - GPIOSARENABLE
 *     - GPSSEL
 *     - IMSWITCHHIDE
 *     - IPV6ENABLE
 *     - WAKEHOSTEN
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_GetCustFeaturesV2( 
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_GetCustFeaturesV2_t *reqArg);


/**
 *  This structure contains which customization id or the list type want to retrieve
 *  from modem. This TLV is only applicable for 9x30 modules so far
 *
 *  \param  cust_id
 *          - Customization ID (Maximum 64 bytes)
 *          - NULL terminated ASCII string.
 * 
 *  \param  list_type
 *          - list type requested
 *
 */
typedef struct
{
    uint8_t cust_id[DMS_MAX_CUST_ID_LEN+1];
    uint8_t list_type;
} DMSgetCustomInput;

 /**
 *  This structure contains information about Customization Setting.
 *  This TLV is only applicable for 9x30 modules so far
 *
 *  \param  id_length
 *          - length of cust_id field
 *
 *  \param  cust_id
 *          - Customization ID (Maximum 64 bytes)
 *          - NULL terminated ASCII string.
 *
 *  \param  value_length
 *          - length of cust_value field
 *
 *  \param  cust_value
 *          - Customization Setting Value (Maximum 8 bytes)
 *
 *  \param  cust_attr
 *          - Customization Setting attribute through QMI
 *              - bit 0: Values:
 *                  - 0 - read only
 *                  - 1 - read/write 
 *
 */
typedef struct
{
    uint16_t id_length;
    uint8_t cust_id[DMS_MAX_CUST_ID_LEN+1];
    uint16_t value_length;
    uint8_t cust_value[DMS_MAX_CUST_VALUE_LEN+1];
    uint16_t cust_attr;
} DMScustSettingInfo;

/**
 *  This structure contains the fields of TLV Customization Setting List. 
 *  This TLV is only applicable for 9x30 modules so far
 *
 *  \param  list_type
 *          - list type requested
 *
 *  \param  num_instances
 *          - number of instances of customization setting
 *
 *  \param  custSetting
 *          -  See \ref DMScustSettingInfo for more information
 *
 */
typedef struct
{
    uint8_t list_type;
    uint16_t num_instances;
    DMScustSettingInfo custSetting[DMS_UINT8_MAX_STRING_SZ+1];
} DMScustSettingList;

/**
 *  This struture contains the TLV required to get the Customization Info and
 *  customization list.
 *
 *  \param pGetCustomInput[IN]
 *          -  See \ref DMSgetCustomInput for more information
 *
 *  \param pCustSettingInfo[OUT]
 *          -  See \ref DMScustSettingInfo for more information
 *
 *  \param pCustSettingList[OUT]
 *          -  See \ref DMScustSettingList for more information
 *
 */
typedef struct
{
    DMSgetCustomInput *pGetCustomInput;
    DMScustSettingInfo *pCustSettingInfo;
    DMScustSettingList *pCustSettingList;
} DMSgetCustomFeatureV2;


/**
 *  This structure contains customization settings set to modem unpack
 *
 *  \param GetCustomFeatureV2
 *         -  See \ref DMSgetCustomFeatureV2 for more information
 *         - pCustSettingInfo
 *              - Bit to check in ParamPresenceMask - <B>32</B>
 *         - pCustSettingList
 *              - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct 
{
    DMSgetCustomFeatureV2 GetCustomFeatureV2;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetCustFeaturesV2_t;

/**
 * Get Custom Feature unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_GetCustFeaturesV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetCustFeaturesV2_t *pOutput
);

/**
 *  This structure contains unpack_dms_GetActivationState parameters.
 *
 *  @param  state[OUT]
 *          -  Service Activation Code\n
 *              0 - Service not activated\n
 *              1 - Service activated\n
 *              2 - Activation connecting\n
 *              3 - Activation connected\n
 *              4 - OTASP security authenticated\n
 *              5 - OTASP NAM downloaded\n
 *              6 - OTASP MDN downloaded\n
 *              7 - OTASP IMSI downloaded\n
 *              8 - OTASP PRL downloaded\n
 *              9 - OTASP SPC downloaded\n
 *              10 - OTASP settings committed
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct 
{
    uint8_t state;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetActivationState_t;

/**
 * Get Activation State pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   This is only supported for 3GPP2 devices.
 */
int pack_dms_GetActivationState(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

/**
 * Get Activation State unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   This is only supported for 3GPP2 devices.
 */
int unpack_dms_GetActivationState(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetActivationState_t *pOutput
);

/**
 *  This structure is used to store image information
 *
 *  @param  imageType
 *          - Image Type
 *          - Values:
 *              - 0 - FW
 *              - 1 - configuration
 *
 *  @param  uniqueID
 *          - Image Unique Identifier ( ASCII characters )
 *
 *  @param  buildIDLen
 *          - Length of build ID string to follow
 *          - If set to zero, build ID string will be blank
 *
 *  @param  buildID
 *          - String containing image information( ASCII characters )
 *          - Maximum length of this string is 255 chars
 */
typedef struct
{
    uint8_t imageType;
    uint8_t uniqueID[UNIQUE_ID_LEN];
    uint8_t buildIDLen;
    uint8_t buildID[MAX_BUILD_ID_LEN];
} image_info_t;

/**
 * This structure is used to store unpack_dms_SLQSSwiGetFirmwareCurr parameters
 *
 * @param  numEntries[IN/OUT]
 *         - Number of entries in the image list to follow
 *         - The size of the list pCurrImgInfo must be specified
 *           when calling the API
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @param  pCurrImgInfo[OUT]
 *         - Currently Active Image List
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  priver[OUT]
 *          - PRI version of the currently running firmware
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pkgver[OUT]
 *          - Package version of the currently running firmware
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  fwvers[OUT]
 *          - firmware version of the currently running firmware
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  carrier[OUT]
 *          - Carrier string of the currently running firmware
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint8_t       numEntries;
    image_info_t *pCurrImgInfo;
    char          priver[DMS_IMGDETAILS_LEN];
    char          pkgver[DMS_IMGDETAILS_LEN];
    char          fwvers[DMS_IMGDETAILS_LEN];
    char          carrier[DMS_IMGDETAILS_LEN];
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSSwiGetFirmwareCurr_t;

/**
 * Get currently active image pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiGetFirmwareCurr(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get currently active image unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiGetFirmwareCurr(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSSwiGetFirmwareCurr_t *pOutput
        );

/**
 * This structure is used to store pack_dms_SLQSSwiSetDyingGaspCfg parameters
 *
 * @param  pDestSMSNum[IN]
 *         - SMS Destination Number as string of 8 bit ASCII Characters Max 20 chars.
 *         - Optional parameter.
 *
 * @param  pDestSMSContent[IN]
 *         - SMS COntent as a string of 8 bit ASCII text characters Max 160 chars.
 *         - Optional parameter.
 */

typedef struct {
    uint8_t *pDestSMSNum;
    uint8_t *pDestSMSContent;
}pack_dms_SLQSSwiSetDyingGaspCfg_t;


/**
 * Set Dying GASP Config pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSSwiSetDyingGaspCfg(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SLQSSwiSetDyingGaspCfg_t *reqArg
);
/**
 *  This structure contains set Dying GASP Config unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct {
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiSetDyingGaspCfg_t;

/**
 * Set Dying GASP Config unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSSwiSetDyingGaspCfg(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiSetDyingGaspCfg_t *pOutput
);

/**
 * Clear Dying GASP Statistics pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSSwiClearDyingGaspStatistics(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

/**
 *  This structure contains Clear Dying GASP unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct {
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiClearDyingGaspStatistics_t;

/**
 * Clear Dying GASP Statistics unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSSwiClearDyingGaspStatistics(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiClearDyingGaspStatistics_t *pOutput
);

/**
 * Get Dying GASP Statistics pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSSwiGetDyingGaspStatistics(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * This structure contains Get Dysing GASP Statistics
 *
 *  \param pTimeStamp[OUT]
 *               - Timestamp of the last time power loss was detected and 
 *                 Dying Gasp feature was triggered. 
 *               - UTC time in seconds since Jan 06, 1980 (GPS Epoch).
 *
 *  \param pSMSAttemptedFlag[OUT]
 *              - Indicates whether device attempted to 
 *                 send SMS in the last power loss event.
 *              - 0 - SMS not attempted
 *              - 1 - SMS attempted
 *              - This only indicates device sent the SMS 
 *                but does not guarantee network delivery.
 */

typedef struct
{
    uint32_t *pTimeStamp;
    uint8_t  *pSMSAttemptedFlag;
} packgetDyingGaspStatistics;

/**
 *  This structure contains Get Dying GASP Statistics unpack.
 *
 *  \param pGetDyingGaspStatistics
 *          - See \ref packgetDyingGaspStatistics
 *          - pTimeStamp.
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 *          - pSMSAttemptedFlag.
 *              - Bit to check in ParamPresenceMask - <B>17</B>
 *  \param  Tlvresult
 *          - Unpack Result
 */

typedef struct {
    packgetDyingGaspStatistics *pGetDyingGaspStatistics;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiGetDyingGaspStatistics_t;

/**
 * Get Dying GASP Statistics unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSSwiGetDyingGaspStatistics(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetDyingGaspStatistics_t *pOutput);

/**
 * Get Dying GASP Config pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSSwiGetDyingGaspCfg(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This struture contains the TLV required to get the Dysing GASP Config.
 *
 * @param  pDestSMSNum[IN]
 *         - SMS Destination Number as string of 8 bit ASCII Characters Max 20 chars.
 *         - Optional parameter.
 *
 * @param  pDestSMSContent[IN]
 *         - SMS COntent as a string of 8 bit ASCII text characters Max 160 chars.
 *         - Optional parameter.
 */
typedef struct
{
    uint8_t *pDestSMSNum;
    uint8_t *pDestSMSContent;
} packgetDyingGaspCfg;

/**
 *  This structure contains Get Dying GASP Config unpack
 *
 *  \param pGetDyingGaspCfg
 *          - See \ref packgetDyingGaspCfg
 *          - pDestSMSNum.
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 *          - pDestSMSContent.
 *              - Bit to check in ParamPresenceMask - <B>17</B>
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    packgetDyingGaspCfg *pGetDyingGaspCfg;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiGetDyingGaspCfg_t;

/**
 * Get Dying GASP Config unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSSwiGetDyingGaspCfg(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetDyingGaspCfg_t *pOutput);

/**
 * To get reset info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSDmsSwiGetResetInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This structure contains the TLV required to Get Reset Info.
 *
 *  \param type[OUT]
 *               - type of reset or power down, possible values listed below:
 *                   - 0 - unknown
 *                   - 1 - warm
 *                   - 2 - hard
 *                   - 3 - crash
 *                   - 4 - power down
 *               - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  source[OUT]
 *               - entity which initiated the reset or power down, possible values listed below:
 *                   - 0 - unknown
 *                   - 1 - user requested ( AT!RESET, AT!BOOTHOLD, FW/PRI download - including host-initiated image switching)
 *                   - 2 - hardware switch (W_DISABLE)
 *                   - 3 - temperature critical
 *                   - 4 - voltage critical
 *                   - 5 - configuration update (SIM-based image switching, RMA reset, NVUPs which request a reset)
 *                   - 6 - LWM2M (Light Weight M2M client (internal process for LWM2M))
 *                   - 7 - OMA-DM
 *                   - 8 - FOTA
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint8_t   type;
    uint8_t   source;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSDmsSwiGetResetInfo_t;

/**
 * To get reset info unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSDmsSwiGetResetInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSDmsSwiGetResetInfo_t *pOutput);

/**
 *  This structure contains the TLV required to Get Reset Info.
 *
 *  \param  type[OUT]
 *               - type of reset or power down, possible values listed below:
 *                   - 0 - unknown
 *                   - 1 - warm
 *                   - 2 - hard
 *                   - 3 - crash
 *                   - 4 - power down
 *               - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param source[OUT]
 *               - entity which initiated the reset or power down, possible values listed below:
 *                   - 0 - unknown
 *                   - 1 - user requested ( AT!RESET, AT!BOOTHOLD, FW/PRI download - including host-initiated image switching)
 *                   - 2 - hardware switch (W_DISABLE)
 *                   - 3 - temperature critical
 *                   - 4 - voltage critical
 *                   - 5 - configuration update (SIM-based image switching, RMA reset, NVUPs which request a reset)
 *                   - 6 - LWM2M (Light Weight M2M client (internal process for LWM2M))
 *                   - 7 - OMA-DM
 *                   - 8 - FOTA
 *                - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult[OUT]
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint8_t    type;
    uint8_t    source;
    uint16_t   Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t;

/**
 * DMS reset info Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 * @note support EM/MC74xx onwards
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSDmsSwiGetResetInfo_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSDmsSwiGetResetInfo_Ind_t *pOutput);

/**
 *  This structure contains set registration 
 *  state for different indication pack
 * @param  resetInfoInd[IN]
 *         - Values
 *            - 0 - Disable
 *            - 1 - Enable
 */

typedef struct {
    uint8_t  resetInfoInd;
}pack_dms_SLQSDmsSwiIndicationRegister_t;


/**
 * Set the registration state for different indication pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 * @note support EM/MC74xx onwards
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSDmsSwiIndicationRegister(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SLQSDmsSwiIndicationRegister_t *reqArg
);

/**
 *  This structure contains set registration state for different indication unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSDmsSwiIndicationRegister_t;

/**
 * Set the registration state for different indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 * @note support EM/MC74xx onwards
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSDmsSwiIndicationRegister(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSDmsSwiIndicationRegister_t *pOutput
);

/**
 * To get Firmware Update status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_SLQSSwiGetFwUpdateStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This structure is used to store Firmware Update Status
 *
 *  @param  ResCode
 *          - FW Update Result Code
 *          - Values:
 *              - 0x00000001 - Successful
 *              - 0xFFFFFFFF - Unknown (due to power off reset after firmware
 *                             update )
 *              - 0x100000nn - File update errors while nn will be the exact
 *                             error number:
 *                             - 00 - General error
 *              - 0x200000nn - NVUP update errors while nn will be the exact
 *                             error number:
 *                             - 00 - General error
 *              - 0x40000nnn - FOTA update agent errors while nnn will be the
 *                             exact error number:
 *                             - 000 ~ 0FF - Insignia defined error code
 *                             - 100 ~ 1FF - Sierra defined error code
 *                             - See qaGobiApiTableFwDldErrorCodes.h for more
 *                               detailed information
 *              - 0x800000nn - FDT/SSDP reported errors while nn will be the
 *                             exact error number
 *                             - See qaGobiApiTableFwDldErrorCodes.h for more
 *                               detailed information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  imgType
 *          - Optional parameter
 *          - Firmware image type that failed the update
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  refData
 *          - Optional parameter
 *          - Failed image reference data
 *          - This is normally the offset of the image that caused the failure
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  refString
 *          - Optional parameter
 *          - Failed image reference string. This is normally the partition
 *            name of the image that caused the failure if applicable.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  logString
 *          - Optional parameter
 *          - Failed image reference string. This is normally the partition
 *            name of the image that caused the failure if applicable.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t  ResCode;
    uint8_t   imgType;
    uint32_t  refData;
    uint8_t   refString[DMS_MAX_FWUPDATE_REF_STR_SZ];
    uint8_t   logString[DMS_MAX_FWUPDATE_LOG_STR_SZ];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiGetFwUpdateStatus_t;

/**
 * To get Firmware Update status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSSwiGetFwUpdateStatus(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetFwUpdateStatus_t *pOutput);

/**
 * To get device manufacturer information pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_GetManufacturer(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This structure is used to store device manufacturer information.
 *
 *  @param  manufacturer[OUT]
 *          - NULL terminated string
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    char manufacturer[255];
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetManufacturer_t;

/**
 * To get device manufacturer information unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_GetManufacturer(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetManufacturer_t *pOutput);

/**
 * To get operating mode offline reason pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_dms_GetOfflineReason(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This structure is used to store reason why the operating mode of the
 *  device is currently offline.
 *
 *  @param  pReasonMask[OUT]
 *          - Optional parameter
 *          - Bitmask of offline reasons
 *              - 0x00000001 - Host image configuration issue
 *              - 0x00000002 - PRI image configuration issue
 *              - 0x00000004 - PRI version incompatible
 *              - 0x00000008 - PRI copy issue
 *              - All others - Reserved
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pbPlatform[OUT]
 *          - Optional parameter
 *          - Is the device offline due to a platform restriction?
 *              - 0 - No
 *              - 1 - Yes
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint32_t *pReasonMask;
    uint32_t *pbPlatform ;
    uint16_t  Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_GetOfflineReason_t;

/**
 * To get operating mode offline reason unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_GetOfflineReason(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_GetOfflineReason_t *pOutput);

/**
 *  This structure is used to store Set Service 
 *  Activation Status callback parameter pack.
 *
 *  @param  activationState
 *          - Service activation state.
 *          - Values
 *            - 0 - Do not report
 *            - 1 - Report activation state changes
 *
 */
typedef struct{
    uint8_t activationState;
}pack_dms_SetActivationStatusCallback_t;

/**
 *  This structure is used to store Set Service 
 *  Activation Status callback parameter unpack.
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetActivationStatusCallback_t;

/**
 * Set activation status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetActivationStatusCallback(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetActivationStatusCallback_t *reqArg
);

/**
 * Set Activation status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetActivationStatusCallback(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetActivationStatusCallback_t *pOutput
);

/**
 *  This structure contains PIN Protection Information
 *
 *  @param  id[IN]
 *          - PIN ID
 *              - 1 ( PIN1 / CHV1 )
 *              - 2 ( PIN2 / CHV2 )
 *
 *  @param  bEnable[IN]
 *          - Enable/disable PIN protection, 0 = Disable
 *
 *  @param  pValue[IN]
 *          - PIN value of the PIN to be enabled/disabled
 *
 */
typedef struct
{
    uint8_t id;
    uint8_t bEnable;
    uint8_t value[DMS_UINT8_MAX_STRING_SZ];
} pack_dms_UIMSetPINProtection_t;

/**
 * To set UIM PIN protection pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMSetPINProtection(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMSetPINProtection_t *pReq);

/**
 *  This structure contains PIN retries status
 *
 *  @param  verifyRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of retries
 *            left, after which PIN will be blocked.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  unblockRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of unblock
 *            retries left, after which the PIN will be permanently blocked
 *            i.e. UIM is unusable.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult[OUT]
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t verifyRetriesLeft;
    uint8_t unblockRetriesLeft;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_UIMSetPINProtection_t;

/**
 * To get UIM Set PIN protection unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMSetPINProtection(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMSetPINProtection_t *pOutput);

/**
 *  This structure contains UIM Unblock PIN Information
 *
 *  @param  id[IN]
 *          - PIN ID
 *              - 1 ( PIN1 / CHV1 )
 *              - 2 ( PIN2 / CHV2 )
 *
 *  @param  pukValue[IN]
 *          - PUK value of PIN to be unblocked
 *
 *  @param  newPin[IN]
 *          - New PIN value for the PIN to be unblocked
 *
 */
typedef struct
{
    uint8_t id;
    uint8_t pukValue[DMS_UINT8_MAX_STRING_SZ];
    uint8_t newPin[DMS_UINT8_MAX_STRING_SZ];
} pack_dms_UIMUnblockPIN_t;

/**
 * To set UIM unblock PIN pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMUnblockPIN(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMUnblockPIN_t *pReq);

/**
 * To get UIM unblock PIN unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMUnblockPIN(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMSetPINProtection_t *pOutput);

/**
 *  This structure contains PIN Value Information
 *
 *  @param  id[IN]
 *          - PIN ID
 *              - 1 ( PIN1 / CHV1 )
 *              - 2 ( PIN2 / CHV2 )
 *
 *  @param  value[IN]
 *          - PIN value of the PIN to be enabled/disabled
 *
 */
typedef struct
{
    uint8_t id;
    uint8_t value[DMS_UINT8_MAX_STRING_SZ];
} pack_dms_UIMVerifyPIN_t;

/**
 * To set UIM verify PIN pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMVerifyPIN(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMVerifyPIN_t *pReq);

/**
 * To get UIM verify PIN unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMVerifyPIN(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMSetPINProtection_t *pOutput);

/**
 *  This structure contains UIM Unblock PIN Information
 *
 *  @param  id[IN]
 *          - PIN ID
 *              - 1 ( PIN1 / CHV1 )
 *              - 2 ( PIN2 / CHV2 )
 *
 *  @param  oldValue[IN]
 *          - Old PIN value of PIN to change
 *
 *  @param  newValue[IN]
 *          - New PIN value of PIN to change
 *
 */
typedef struct
{
    uint8_t id;
    uint8_t oldValue[DMS_UINT8_MAX_STRING_SZ];
    uint8_t newValue[DMS_UINT8_MAX_STRING_SZ];
} pack_dms_UIMChangePIN_t;

/**
 * To set UIM change PIN pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMChangePIN(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMChangePIN_t *pReq);

/**
 * To get UIM change PIN unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_dms_UIMChangePIN(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMSetPINProtection_t *pOutput);

/**
 *  This structure contains UIM get control key status information
 *
 *  @param  facility[IN]
 *          - MT or network facility
 *              - 0 - Network Personalization (PN)
 *              - 1 - Network Subset Personalization (PU)
 *              - 2 - Service Provider Personalization (PP)
 *              - 3 - Corporate Personalization (PC)
 *              - 4 - UIM Personalization (PF)
 *
 */
typedef struct
{
    uint8_t facility;
} pack_dms_UIMGetControlKeyStatus_t;

/**
 * To set UIM Get Control Key Status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMGetControlKeyStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMGetControlKeyStatus_t *pReq);

/**
 *  This structure contains PIN retries status
 *
 *  @param  facilityState[OUT]
 *          - Control key status
 *              - 0 - Deactivated
 *              - 1 - Activated
 *              - 2 - Blocked
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  verifyRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of retries
 *            left, after which PIN will be blocked.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  unblockRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of unblock
 *            retries left, after which the PIN will be permanently blocked
 *            i.e. UIM is unusable.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t facilityState;
    uint8_t verifyRetriesLeft;
    uint8_t unblockRetriesLeft;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_UIMGetControlKeyStatus_t;

/**
 * To get UIM Get ControlKeyStatus unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMGetControlKeyStatus(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMGetControlKeyStatus_t *pOutput);

/**
 * To set UIM Get PIN Status pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMGetPINStatus(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 *  This structure contains Get PIN Status unpack information
 *
 *  @param  p1Status[OUT]
 *          - PIN1 status(0xFFFFFFFF - Unknown)
 *              - 0 - PIN not initialized
 *              - 1 - PIN enabled, not verified
 *              - 2 - PIN enabled, verified
 *              - 3 - PIN disabled
 *              - 4 - PIN blocked
 *              - 5 - PIN permanently blocked
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  p1VerifyRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of retries
 *            left of PIN1, after which PIN will be blocked.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  p1UnblockRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of unblock
 *            retries left of PIN1, after which the PIN will be permanently blocked
 *            i.e. UIM is unusable.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  p2Status[OUT]
 *          - PIN2 status(0xFFFFFFFF - Unknown)
 *              - 0 - PIN not initialized
 *              - 1 - PIN enabled, not verified
 *              - 2 - PIN enabled, verified
 *              - 3 - PIN disabled
 *              - 4 - PIN blocked
 *              - 5 - PIN permanently blocked
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  p2VerifyRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of retries
 *            left of PIN2, after which PIN will be blocked.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  p2UnblockRetriesLeft[OUT]
 *          - Optional parameter
 *          - Upon operational failure, this will indicate number of unblock
 *            retries left of PIN2, after which the PIN will be permanently blocked
 *            i.e. UIM is unusable.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t p1Status;
    uint8_t p1VerifyRetriesLeft;
    uint8_t p1UnblockRetriesLeft;
    uint8_t p2Status;
    uint8_t p2VerifyRetriesLeft;
    uint8_t p2UnblockRetriesLeft;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_UIMGetPINStatus_t;

/**
 * To get UIM Get PIN Status unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMGetPINStatus(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMGetPINStatus_t *pOutput);

/**
 *  This structure contains UIM Set control key protection information
 *
 *  @param  facility[IN]
 *          - MT or network facility
 *              - 0 - Network Personalization (PN)
 *              - 1 - Network Subset Personalization (PU)
 *              - 2 - Service Provider Personalization (PP)
 *              - 3 - Corporate Personalization (PC)
 *              - 4 - UIM Personalization (PF)
 *
 *  @param  facilityState[IN]
 *          - UIM facility state
 *              - 0 - Deactivated
 *              - 1 - Activated
 *              - 2 - Blocked
 *
 *  @param  facilityCk[IN]
 *          - Facility depersonalization control key, 
 *            string in ASCII text (maximum 8 bytes)
 *
 */
typedef struct
{
    uint8_t facility;
    uint8_t facilityState;
    uint8_t facilityCk[CK_MAX_SIZE];
} pack_dms_UIMSetControlKeyProtection_t;

/**
 * To set UIM Set Control Key Protection pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMSetControlKeyProtection(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMSetControlKeyProtection_t *pReq);

/**
 *  This structure contains UIM Set control key protection unpack information
 *
 *  @param  verifyRetriesLeft[OUT]
 *          - Optional parameter
 *          - number of retries left after which the control key is blocked
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t verifyRetriesLeft;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_UIMSetControlKeyProtection_t;

/**
 * To get UIM Set Control Key Protection unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */ 
int unpack_dms_UIMSetControlKeyProtection(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMSetControlKeyProtection_t *pOutput);


/**
 *  This structure contains UIM unblock Control Key information
 *
 *  @param  facility[IN]
 *          - MT or network facility
 *              - 0 - Network Personalization (PN)
 *              - 1 - Network Subset Personalization (PU)
 *              - 2 - Service Provider Personalization (PP)
 *              - 3 - Corporate Personalization (PC)
 *              - 4 - UIM Personalization (PF)
 *
 *  @param  facilityCk[IN]
 *          - Facility depersonalization control key, string in ASCII text (maximum 8 bytes)
 *
 */
typedef struct
{
    uint8_t facility;
    uint8_t facilityCk[CK_MAX_SIZE];
} pack_dms_UIMUnblockControlKey_t;

/**
 * To set UIM Unblock Control Key pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_UIMUnblockControlKey(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_UIMUnblockControlKey_t *pReq);

/**
 *  This structure contains UIM Set control key protection unpack information
 *
 *  @param  unblockRetriesLeft[OUT]
 *          - Optional parameter
 *          - number of unblock retries left after which the control key is permanently blocked 
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult[OUT]
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t unblockRetriesLeft;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_UIMUnblockControlKey_t;

/**
 * To get UIM Unblock Control Key unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_UIMUnblockControlKey(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_UIMUnblockControlKey_t *pOutput);

/**
 *  This structure contains UIM reset to factory default information
 *
 *  @param  spc[IN]
 *          - Service programming code in ASCII format (digits 0 to 9 only)
 *
 */
typedef struct
{
    uint8_t spc[SPC_SIZE];
} pack_dms_ResetToFactoryDefaults_t;

/**
 * Resets to default factory settings of the device pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_ResetToFactoryDefaults(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_ResetToFactoryDefaults_t *pReq);

/**
 *  This structure contains reset to factory default unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_ResetToFactoryDefaults_t;

/**
 * Resets to default factory settings of the device unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_ResetToFactoryDefaults(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_ResetToFactoryDefaults_t *pOutput);

/**
 * Requests the device to perform automatic service activation pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_ValidateSPC(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_ResetToFactoryDefaults_t *pReq);

/**
 * Requests the device to perform automatic service activation unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_ValidateSPC(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_ResetToFactoryDefaults_t *pOutput);

/**
 *  This structure contains UIM activate automation information
 *
 *  @param  actCode
 *          - NULL-terminated string representing activation code
 *          (maximum string length of 12).
 *          - Specific carrier requirements may dictate actual
 *          activation code that is applicable, e.g., "*22899"
 *
 */
typedef struct
{
    uint8_t actCode[ACT_CODE_MAX_SIZE];
} pack_dms_ActivateAutomatic_t;

/**
 * Requests the device to perform automatic service activation pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_ActivateAutomatic(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_ActivateAutomatic_t *pReq);

/**
 * Requests the device to perform automatic service activation unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_ActivateAutomatic(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_ResetToFactoryDefaults_t *pOutput);

/**
 *  This structure contains Extended Roaming Indicator(ERI) file parameters
 *
 *  @param  eriDataLen
 *          - Upon input, the maximum number of bytes that file
 *            contents array can contain.\n
 *          - Upon successful output, actual number of bytes written to
 *            file contents array
 *
 *  @param  eriData
 *          - ERI data read from persistent storage( Max size is 1024 )
 *
 */
typedef struct
{
    uint16_t eriDataLen;
    uint8_t eriData[ERI_DATA_MAX_SIZE];
} eriDataparams;

/**
 *  This structure contains Get ERI file unpack
 *
 *  \param  eriFile
 *          - Pointer to structure ERIFileparams
 *          - See \ref eriDataparams for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters. 
 */
typedef struct
{
    eriDataparams eriFile;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSGetERIFile_t;

/**
 * To set UIM Get ERI file pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSGetERIFile(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * To get UIM Get ERI file unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_SLQSGetERIFile(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSGetERIFile_t *pOutput);

/**
 *  This structure contains UIM get state unpack information
 *
 *  @param  state[OUT]
 *          - UIM state
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t state;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSUIMGetState_t;

/**
 * Returns the UIM state pack. This API is deprecated on MC73xx/EM73xx modules
 * since firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use API \ref unpack_uim_GetCardStatus/ \ref unpack_uim_GetCardStatus
 * for new firmware versions and new modules
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSUIMGetState(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * Returns the UIM state unpack. This API is deprecated on MC73xx/EM73xx modules
 * since firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use API \ref unpack_uim_GetCardStatus / \ref unpack_uim_GetCardStatus
 * for new firmware versions and new modules
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSUIMGetState(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSUIMGetState_t *pOutput);

/**
 *  This structure contains pack_dms_SLQSSwiGetCrashInfo parameters
 *
 *  @param  clear
 *          - 0 - do not clear crash data after response
 *          - 1 - clear crash data after response
 *
 */
typedef struct
{
    uint8_t clear;
} pack_dms_SLQSSwiGetCrashInfo_t;

/**
 *  This structure contains crash information
 *
 *  @param  numCrashes[OUT]
 *          - number of instances of the remaining fields
 *
 *  @param  crashId[OUT]
 *          - ramdom crash id assigned at crash
 *
 *  @param  crashData
 *          - crash data[OUT]
 *
 *  @param  crashStrlen[IN/OUT]
 *          - length of the crashString field as an input,
 *            length of the crashString field returned by API as an OUTPUT
 *
 *  @param  crashString
 *          - crash string
 *
 *  @param  gcdumpStrlen[OUT]
 *          - length of the gcdumpString field as an input,
 *            length of the gcdumpString field returned by API as an OUTPUT
 *
 *  @param  gcdumpString[OUT]
 *          - gcdump string for the crash
 *
 */
typedef struct
{
    uint16_t numCrashes;
    uint32_t crashId;
    uint32_t crashData;
    uint16_t crashStrlen;
    char crashString[255];
    uint16_t gcdumpStrlen;
    char gcdumpString[1024];
} crashInformation;

/**
 *  This structure contains crash information parameters
 *
 *  @param  crashStatus[OUT]
 *          - Device Crash Status
 *          - 0 - no crash
 *          - 1 - crash has occurred
 *
 *  @param  crashInfo[OUT]
 *          - See \ref crashInformation
 */
typedef struct
{
    uint8_t crashStatus;
    crashInformation crashInfo;
} crashInfoParams;

/**
 *  This structure contains SWI get crash information unpack information
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *
 *  @param  crashInfoParam[OUT]
 *          - See \ref crashInfoParams
 *          - crashStatus
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *          - crashInfo
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint16_t Tlvresult;
    crashInfoParams crashInfoParam;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSSwiGetCrashInfo_t;

/**
 * To Get the Crash Information from the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiGetCrashInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SLQSSwiGetCrashInfo_t *pReq);

/**
 * To Get the Crash Information from the device unpack.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiGetCrashInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetCrashInfo_t *pOutput);

/**
 *  This structure contains SWI get host device info unpack information
 *
 *  @param  Tlvresult
 *          - unpack Result
 *
 *  @param  manString[OUT]
 *          - Host Device Manufacturer Name
 *          - Null terminated ASCII String
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  modelString[OUT]
 *          - Host Device Model String
 *          - Null terminated ASCII String
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  swVerString[OUT]
 *          - Host Device Software Version String
 *          - Null terminated ASCII string
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  plasmaIDString[OUT]
 *          - Host Device Plasma ID String
 *          - Null terminated alphanumeric ASCII String.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  hostID[OUT]
 *          - Device Host ID String
 *          - Null terminated alphanumeric ASCII String.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param ParamPresenceMask[OUT]
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint16_t Tlvresult;
    char manString[255];
    char modelString[255];
    char swVerString[255];
    char plasmaIDString[255];
    char hostID[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSSwiGetHostDevInfo_t;

/**
 * To get host dev information pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiGetHostDevInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * To get host dev information unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiGetHostDevInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetHostDevInfo_t *pOutput);

/**
 *  This structure contains SWI set host device info unpack information
 *
 *  @param  manString[IN]
 *          - Host Device Manufacturer Name
 *          - Null terminated ASCII String
 *
 *  @param  modelString[IN]
 *          - Host Device Model String
 *          - Null terminated ASCII string.
 *
 *  @param  swVerString[IN]
 *          - Host Device Software Version String
 *          - Null terminated ASCII string
 *
 *  @param  plasmaIDString[IN]
 *          - Host Device Plasma ID String
 *          - Null terminated alphanumeric ASCII string
 *
 *  @param  HostID[IN]
 *          - Device Host ID String
 *          - Null terminated alphanumeric ASCII string
 */
typedef struct
{
    char manString[255];
    char modelString[255];
    char swVerString[255];
    char plasmaIDString[255];    
    char hostID[255];
} pack_dms_SLQSSwiSetHostDevInfo_t;

/**
 *  This structure contains SWI set host dev info unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiSetHostDevInfo_t;

/**
 * To set Host Dev Info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiSetHostDevInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SLQSSwiSetHostDevInfo_t *pReq);

/**
 * To set Host Dev Info unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiSetHostDevInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiSetHostDevInfo_t *pOutput);

/**
 *  This structure contains SWI get host os info unpack information
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *
 *  @param  nameString[OUT]
 *          - Host operating system name
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  VersionString[OUT]
 *          - Host operating system version
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t Tlvresult;
    char nameString[255];
    char versionString[255];
   swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSSwiGetOSInfo_t;

/**
 * To Get Host OS Info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiGetOSInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * To get Host OS Info unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiGetOSInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetOSInfo_t *pOutput);

/**
 *  This structure contains SWI set host OS info pack information
 *
 *  @param  nameString[IN]
 *          Host device manufacture String
 *          - NULL terminated ASCII string.
 *
 *  @param  VersionString[IN]
 *          Host device model String
 *          - NULL terminated ASCII string.
 *
 */
typedef struct
{
    char nameString[255];
    char versionString[255];
} pack_dms_SLQSSwiSetOSInfo_t;

/**
 *  This structure contains SWI set host OS info unpack
 *
 *  \param  Tlvresult
 *          - Unpack Result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSwiSetOSInfo_t;

/**
 * To set Host OS Info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[out] pReq requeset parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiSetOSInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SLQSSwiSetOSInfo_t *pReq);

/**
 * To set Host OS Info unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiSetOSInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiSetOSInfo_t *pOutput);

/**
 *  This structure contains SWI get device serial number extension unpack information
 *
 *  @param  Tlvresult
 *          - pack Result
 *
 *  @param  meidString[OUT]
 *          - optional parameter, mobile equipment identifier
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t Tlvresult;
    char meidString[MEID_MAX_SIZE];
   swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSSwiGetSerialNoExt_t;

/**
 * To get serial number extension pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSwiGetSerialNoExt(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * To get serial number extension unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSwiGetSerialNoExt(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSwiGetSerialNoExt_t *pOutput);

/**
 *  This structure contains the TLV required to Get device power control status information.
 *
 *  @param  opMode
 *               - Selected operating mode.
 *               - Values
 *                 - 0 - Online
 *                 - 1 - Low power
 *                 - 2 - Factory Test mode
 *                 - 3 - Offline
 *                 - 4 - Resetting
 *                 - 5 - Shutting down
 *                 - 6 - Persistent low power
 *                 - 8 - Conducting network test for GSM/WCDMA
 *               - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  has_LpmFlag
 *              LPM Flag Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *               - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  LpmFlag[Optional]
 *               - LPM Force Flags
 *               - Bitmask
 *                 - bit0 - Tracks the state of W_DISABLE TLV, indicating
 *                          state of the W_DISABLE switch.
 *                 - bit1 - Set if LPM is requested by a host request,
 *                          such as AT command, QMI or MBIM request.
 *                          Cleared by a host request to return to online mode.
 *                          Also set when the device is cold or warm booted in persistent LPM.
 *                 - bit2 - Set when the device temperature is outside the valid operating range.
 *                          Cleared if the temperature returns to the normal range.
 *                 - bit3 - Set when the device voltage is outside the valid operating range.
 *                          Cleared if the voltage returns to the normal range.
 *                 - bit4 - Set on power up when BIOS locking is enabled.  Cleared when the
 *                          host has disabled the BIOS lock.
 *                 - bit5 - Set if the current device configuration does not match the GOBI image preference.
 *                 - bit6-31 - Additional LPM causes may be added to future products.
 *               - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  has_Wdisable
 *              W-Disable Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *               - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  Wdisable[Optional]
 *               - W_DISABLE
 *               - Values
 *                 - 0 - Switch set to ON position
 *                 - 1 - Switch set to OFF position
 *               - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  has_PowerOffMode
 *              Power off mode Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *               - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  PowerOffMode[Optional]
 *               - Power-off Mode
 *               - Action taken when W_DISABLE is switched to the OFF position
 *                 - 0 - LPM
 *                 - 1 - Shutdown
 *                 - 2 - Ignore
 *               - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  has_PersistentLpm
 *              Persistent LPM Availability.
 *                  - 0 : Unavailable.
 *                  - 1 : Available.
 *               - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  PersistentLpm[Optional]
 *               - LPM Persistence
 *               - Values
 *                 - 0 - Non-persistent LPM
 *                 - 1 - Persistent LPM
 *               - Bit to check in ParamPresenceMask - <B>19</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    uint8_t  opMode;
    int      has_LpmFlag;
    uint32_t LpmFlag;
    int      has_Wdisable;
    uint8_t  Wdisable;
    int      has_PowerOffMode;
    uint8_t  PowerOffMode;
    int      has_PersistentLpm;
    uint8_t  PersistentLpm;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSDmsSwiGetPCInfo_t;

/**
 * Get device power control status information pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSDmsSwiGetPCInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get device power control status information unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSDmsSwiGetPCInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSDmsSwiGetPCInfo_t *pOutput
        );
/**
 * This structure contains the parameters for Power Save Mode Enable State.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  enableState
 *          Power Save Mode Enable State
 *          Values
 *            - 0 - PSM is not enabled
 *            - 1 - PSM is enabled
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint8_t  enableState;
} dms_PSMEnableStateTlv;

/**
 * This structure contains the parameters for Power Save Mode Duration Threshold.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  durationThreshold
 *          Duration Threshold
 *          - Minimum duration for the device to benefit by entering PSM (in seconds).
 */
typedef struct
{
    uint8_t   TlvPresent;
    uint32_t  durationThreshold;
} dms_PSMDurationThresholdTlv;

/**
 * This structure contains the parameters for Power Save Mode Duration due
 * to an outage.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  durationDueToOOS
 *          Duration Due to OOS
 *          - Power Save mode duration due to an outage (in seconds).
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t durationDueToOOS;
} dms_PSMDurationDueToOOSTlv;

/**
 * This structure contains the parameters for Power Save Mode wakeup randomization window.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  randomizationWindow
 *          Randomization Window
 *          - Power Save mode wakeup randomization window (in seconds)
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t randomizationWindow;
} dms_PSMRandomizationWindowTlv;

/**
 * This structure contains the parameters for Power Save Mode active timer.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  activeTimer
 *          Active Timer
 *          - Power Save mode active timer value (in seconds).
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t activeTimer;
} dms_PSMActiveTimerTlv;

/**
 * This structure contains the parameters for Power Save Mode
 * periodic update timer.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  periodicUpdateTimer
 *          Periodic Update Timer
 *          - Power Save mode periodic update timer value (in seconds).
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t periodicUpdateTimer;
} dms_PSMPeriodicUpdateTimerTlv;

/**
 * This structure contains the parameters for Power Save Mode
 * early wakeup time.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  earlyWakeupTime
 *          - Early Wakeup Time
 *          - Power Save mode early wakeup time (in seconds)
 *            indicating how early the device should exit PSM
 *            to offset for bootup and acquisition delay.
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t earlyWakeupTime;
} dms_PSMEarlyWakeupTimeTlv;

/**
 *  This structure contains the Power Save Mode (PSM) configuration parameter.
 *
 *  @param  pPsmEnableState [Optional]
 *          - See @ref dms_PSMEnableStateTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pDurationThreshold [Optional]
 *          - See @ref dms_PSMDurationThresholdTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pDurationDueToOOS [Optional]
 *          - See @ref dms_PSMDurationDueToOOSTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pRandomizationWindow [Optional]
 *          - See @ref dms_PSMRandomizationWindowTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pActiveTimer [Optional]
 *          - See @ref dms_PSMActiveTimerTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pPeriodicUpdateTimer [Optional]
 *          - See @ref dms_PSMPeriodicUpdateTimerTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pEarlyWakeupTime [Optional]
 *          - See @ref dms_PSMEarlyWakeupTimeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct
{
    dms_PSMEnableStateTlv         *pPsmEnableState;
    dms_PSMDurationThresholdTlv   *pDurationThreshold;
    dms_PSMDurationDueToOOSTlv    *pDurationDueToOOS;
    dms_PSMRandomizationWindowTlv *pRandomizationWindow;
    dms_PSMActiveTimerTlv         *pActiveTimer;
    dms_PSMPeriodicUpdateTimerTlv *pPeriodicUpdateTimer;
    dms_PSMEarlyWakeupTimeTlv     *pEarlyWakeupTime;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SLQSGetPowerSaveModeConfig_t;

/**
 * Get Power Save Mode (PSM) configuration parameter pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSGetPowerSaveModeConfig(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get Power Save Mode (PSM) configuration parameter unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSGetPowerSaveModeConfig(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSGetPowerSaveModeConfig_t *pOutput
        );

/**
 *  This structure contains the Power Save Mode (PSM) configuration parameter.
 *
 *  @param  pPsmEnableState [Optional]
 *          - PSM Enable State
 *          - Values
 *            - 0 - PSM is not enabled
 *            - 1 - PSM is enabled
 *
 *  @param  pActiveTimer [Optional]
 *          - Active Timer
 *          - PSM active timer value (in seconds).
 *          - ActiveTimer encoding is the GPRS timer 2 encoding specified in
 *            Table 10.5.172 of 3GPP TS 24.008.
 *
 *  @param  pPeriodicUpdateTimer [Optional]
 *          - Periodic Update Timer
 *          - PSM periodic update timer value (in seconds)
 *          - Periodic Update Timer encoding is the GPRS timer 3 encoding
 *            specified in Table 10.5.163a of 3GPP TS 24.008.
 *
 */
typedef struct{
    uint8_t    *pPsmEnableState;
    uint32_t   *pActiveTimer;
    uint32_t   *pPeriodicUpdateTimer;
}pack_dms_SLQSSetPowerSaveModeConfig_t;

/**
 *  This structure is used to store unpack_dms_SLQSSetPowerSaveModeConfig parameters.
 *
 *  \param  Tlvresult
 *           - unpack Tlv result
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SLQSSetPowerSaveModeConfig_t;

/**
 * Sets the Power Save Mode (PSM) configuration parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSSetPowerSaveModeConfig(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_dms_SLQSSetPowerSaveModeConfig_t *reqArg
);

/**
 * Sets the Power Save Mode (PSM) configuration parameters unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSSetPowerSaveModeConfig(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SLQSSetPowerSaveModeConfig_t *pOutput
);

/**
 * This structure contains the DMS Register Indication request parameters
 *
 * @param  PSMStatus
 *         - Power Save Mode Status
 *         - Values
 *           - 0 - Do not report
 *           - 1 - Report PSM status
 *           - 2 - No Change
 *
 * @param  PSMCfgChangeInfo
 *         - Power Save Mode Configuration Change Information
 *         - Values
 *           - 0 - Do not report
 *           - 1 - Report Power Save Mode configuration changes
 *           - 2 - No Change
 *
 * @param  RptIMSCapability
 *         - Report IMS Capability
 *         - Values
 *           - 0 - Do not report
 *           - 1 - Report IMS capability
 *           - 2 - No Change
 *
 */
typedef struct{
    uint8_t PSMStatus;
    uint8_t PSMCfgChangeInfo;
    uint8_t RptIMSCapability;
}pack_dms_SetIndicationRegister_t;

/**
 *  This structure is used to store unpack_dms_SetIndicationRegister parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SetIndicationRegister_t;

/**
 * Set Indication register pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SetIndicationRegister(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SetIndicationRegister_t *reqArg
);

/**
 * Set Indication register unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SetIndicationRegister(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SetIndicationRegister_t *pOutput
);

/**
 * This Structure used to store DMS PSM Enable state TLV Value.
 *
 * @param  TlvPresent
 *         - Boolean indicating the presence of the TLV in the QMI response
 *
 * @param  EnableStateInd
 *         - PSM Enable State
 *         - Values
 *            - 0 - PSM is not enabled
 *            - 1 - PSM is enabled
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t EnableStateInd;
}dms_PSMEnableStateIndTlv;

/**
 * This Structure used to store DMS PSM Active Timer TLV Value.
 *
 * @param  TlvPresent
 *         - Boolean indicating the presence of the TLV in the QMI response
 *
 * @param  ActiveTimerInd
 *         - Active Timer
 *         - PSM active timer value (in seconds).
 *
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t ActiveTimerInd;
}dms_PSMActiveTimerIndTlv;

/**
 * This Structure used to store DMS PSM Periodic Update Timer TLV Value.
 *
 * @param  TlvPresent
 *         - Boolean indicating the presence of the TLV in the QMI response
 *
 * @param  PeriodicUpdateTimerInd
 *         - Periodic Update Timer
 *         - PSM periodic update timer value (in seconds).
 *
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t PeriodicUpdateTimerInd;
}dms_PSMPeriodicUpdateTimerIndTlv;

/**
 *  DMS Event Report indication structure
 *
 *  @param  EnableState
 *          - See @ref dms_PSMEnableStateIndTlv
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ActiveTimer
 *          - See @ref dms_PSMActiveTimerIndTlv
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  PeriodicUpdateTimer
 *          - See @ref dms_PSMPeriodicUpdateTimerIndTlv
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  Tlvresult
 *          - Unpack Result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct{
    dms_PSMEnableStateIndTlv         EnableState;
    dms_PSMActiveTimerIndTlv         ActiveTimer;
    dms_PSMPeriodicUpdateTimerIndTlv PeriodicUpdateTimer;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_PSMCfgChange_ind_t;

/**
 * PSM Configuration change Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_dms_PSMCfgChange_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_PSMCfgChange_ind_t *pOutput
);

/**
 *  This structure is used to store pack_dms_SwiSetEventReport parameters.
 *
 *  @param  pTempReport (optional)
 *          - Temperature indicator.
 *              - 0 - Do not report
 *              - 1 - Report when state changes
 *
 *  @param  pVoltReport (optional)
 *          - Voltage Indicator.
 *              - 0 - Do not report
 *              - 1 - Report when state changes
 *  @param  pUIMStatusReport (optional)
 *          - UIM status Indicator.
 *              - 0 - Do not report
 *              - 1 - Report when state changes
 *
 */
typedef struct{
    uint8_t *pTempReport;
    uint8_t *pVoltReport;
    uint8_t *pUIMStatusReport;
}pack_dms_SwiSetEventReport_t;

/**
 * Sets the DMS swi event report parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 * 
 */

int pack_dms_SwiSetEventReport(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SwiSetEventReport_t *reqArg
);

/**
 *  This structure is used to store unpack_dms_SwiSetEventReport parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SwiSetEventReport_t;

/**
 * Sets the swi event report parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SwiSetEventReport(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SwiSetEventReport_t *pOutput
);

/**
 *  This structure stores information about temperature TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param TempStat
 *          - Temperature  state
 *            - 0 - unknown
 *            - 1 - normal
 *            - 2 - high (warning)
 *            - 3 - high (critical)
 *            - 4 - low (critical)
 *  @param Temperature
 *          - temperature in degree celsius 
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t TempStat;
    uint16_t Temperature;
} dms_TemperatureTlv;


/**
 *  This structure stores information about voltage TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param VoltStat
 *          - Voltage  state
 *            - 0 - unknown
 *            - 1 - normal
 *            - 2 - low (warning)
 *            - 3 - low (critical)
 *            - 4 - high (critical)
 *  @param Voltage
 *          - Voltage in mV 
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t VoltStat;
    uint16_t Voltage;
} dms_VoltageTlv;


/**
 *  This structure stores information about UIM status TLV. 
 *  @param  TlvPresent
 *          - value is 1 if this TLV is present in indication, otherwise 0
 *  @param intf
 *          - interface type
 *            - 0 - External UIM.
 *            - 1 - Embedded UIM.
 *            - 2 - Remote UIM.
 *
 *  @param event
 *          - event type
 *              - 0 - UIM card removed.
 *              - 1 - UIM card inserted.
**/
typedef struct
{
    uint8_t TlvPresent;
    uint8_t intf;
    uint8_t event;
} dms_UimStatusTlv;

/**
 *  Structure used to SWI event report indication parameters. 
 *  @param  TempTlv
 *          - Temperature status tlv
 *          - See @ref dms_TemperatureTlv for more information
 *  @param  VoltTlv
 *          - Voltage status tlv
 *          - see @ref dms_VoltageTlv for more information
 *  @param  UimStatusTlv
 *          - UIM status tlv
 *          - see @ref dms_UimStatusTlv for more information
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 * @note:    None
**/
typedef struct{
    dms_TemperatureTlv TempTlv;
    dms_VoltageTlv     VoltTlv;
    dms_UimStatusTlv   UimStatusTlv;
    swi_uint256_t  ParamPresenceMask;
} unpack_dms_SwiEventReportCallBack_ind_t;

/**
 * unpack for DMS swi event report indication
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SwiEventReportCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SwiEventReportCallBack_ind_t *pOutput
        );

/**
 *  This structure is used to store pack_dms_SwiUimSelect parameters.
 *
 *  @param  uim_select 
 *          - Selection of UIM Card
 *              - 0 - External UIM Interface
 *              - 1 - Embedded UIM
 *              - 2 - Remote UIM
 *              - 3 - Auto switch
 *  @note  Auto switch is applicable only if UIMAUTOSWITCH feature is enabled.
 *         This feature will activate the auto-switch mechanism if it is inactive.
 *
**/
typedef struct{
    uint8_t uim_select;
}pack_dms_SwiUimSelect_t;

/**
 * Sets the DMS swi UIM Select parameters pack. A reset is required for any
 * change in the UIM selection to take effect.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 * 
**/
int pack_dms_SwiUimSelect(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_dms_SwiUimSelect_t *reqArg
);

/**
 *  This structure is used to store unpack_dms_SwiUimSelect parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
**/
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_dms_SwiUimSelect_t;

/**
 * Sets the DMS swi UIM Select parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
**/
int unpack_dms_SwiUimSelect(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_dms_SwiUimSelect_t *pOutput
);

/**
 *  This structure contains the TLV required to Get UIM auto active slot Tlv.
 *
 *  \param   uimAutoSwitchActSlot[Optional]
 *           - When UIMAUTOSWITCH feature enabled and SIM auto-switch
 *             is activated, indicates currently active UIM slot.
 *           - Values
 *             - 0 - slot 1 (e.g. external SIM)
 *             - 1 - slot 2 (e.g. embedded SIM)
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 *
 */
typedef struct
{
    uint8_t  uimAutoSwitchActSlot;
    uint8_t  TlvPresent;
} dms_UimAutoSwitchActSlotTlv;

/**
 *  Structure contains the TLV required to Get currently selected active UIM slot.
 *
 *  @param  uimSelect
 *               - Active selected UIM slot
 *               - Values
 *                 - 0 - slot 1 (e.g. external SIM)
 *                 - 1 - slot 2 (e.g. embedded SIM)
 *                 - 2 - remote SIM (if supported)
 *                 - 3 - SIM auto-switch activated
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pUimAutoSwitchActSlot
 *          - UIM Auto Switch Active slot
 *          - see @ref dms_UimAutoSwitchActSlotTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 *  @note:    None
**/
typedef struct{
    uint8_t                     uimSelect;
    dms_UimAutoSwitchActSlotTlv *pUimAutoSwitchActSlot;
    swi_uint256_t                ParamPresenceMask;
} unpack_dms_SLQSDmsSwiGetUimSelection_t;

/**
 * Get device UIM Selection information pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter(NULL)
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_dms_SLQSDmsSwiGetUimSelection(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        void     *reqArg
        );

/**
 * Get device UIM selection information unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_dms_SLQSDmsSwiGetUimSelection(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_dms_SLQSDmsSwiGetUimSelection_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif//#ifndef  _LITEQMI_DMS_H_

