/**
 * \ingroup liteqmi
 *
 * \file swidms.h
 */
#ifndef __LITEQMI_SWIDMS_H__
#define __LITEQMI_SWIDMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

/**
 * pack function to get usb net numbers for QMAP configuration
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_swidms_SLQSSwiDmsGetUsbNetNum(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * This structure contains usb net numbers to get from remote 
 * endpoint for QMAP configuration
 *
 *  @param  usbNetNum
 *          - value of usb net numbers on the device
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint8_t                usbNetNum;
    swi_uint256_t          ParamPresenceMask;
}unpack_swidms_SLQSSwiDmsGetUsbNetNum_t;

/**
 * unpack function to get usb net numbers for QMAP configuration
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_swidms_SLQSSwiDmsGetUsbNetNum(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsGetUsbNetNum_t *pOutput);

/**
 * This structure contains usb net number to set on remote endpoint for QMAP configuration
 *
 *  @param  nUsbNetNum
 *          - value of usb net numbers to set for the device
 *          - range is 0-8
 */
typedef struct {
    uint8_t                nUsbNetNum;
}pack_swidms_SLQSSwiDmsSetUsbNetNum_t;

typedef unpack_result_t  unpack_swidms_SLQSSwiDmsSetUsbNetNum_t;

/**
 * pack function to set usb net numbers for QMAP configuration
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request params
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_swidms_SLQSSwiDmsSetUsbNetNum(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_swidms_SLQSSwiDmsSetUsbNetNum_t *pReqParam);

/**
 * unpack function to set usb net numbers for QMAP configuration
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_swidms_SLQSSwiDmsSetUsbNetNum(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsSetUsbNetNum_t *pOutput
    );

/**
 *  This structure contains the Set MTU  request parameter.
 *
 *  @param  MTUSize
 *          - Set MTU size
 *          - MTU Values
 *            - 0 - use default value
 *            - 576 to 2000 - other values required by carrier.
 *            - Set the same MTU for all RAT/interfaces.
 *            - New MTU size effective after modem reboot.
 *
 */
typedef struct{
    uint16_t MTUSize;
}pack_swidms_SLQSSwiDmsSetMTU_t;

/**
 *  This structure is used to store unpack_swidms_SLQSSwiDmsSetMTU_t parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *           - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *           - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_swidms_SLQSSwiDmsSetMTU_t;

/**
 * Sets the preferred MTU size for 3GPP, HRPD, EHRPD Interfaces pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swidms_SLQSSwiDmsSetMTU(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_swidms_SLQSSwiDmsSetMTU_t *reqArg
);

/**
 * Sets preferred MTU size for 3GPP, HRPD, EHRPD Interfaces unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swidms_SLQSSwiDmsSetMTU(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsSetMTU_t *pOutput
);

/**
 * This structure contains the parameters for 3GPP MTU Size.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  MTUSize3gpp
 *          3GPP MTU size
 *          - Size of 3GPP MTU
 */
typedef struct
{
    uint8_t   TlvPresent;
    uint16_t  MTUSize3gpp;
} swidms_mtuSize3gppTlv;

/**
 * This structure contains the parameters for HRPD MTU Size.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  hrpdMTUSize
 *          HRPD size
 *          - Size of HRPD MTU
 */
typedef struct
{
    uint8_t   TlvPresent;
    uint16_t  hrpdMTUSize;
} swidms_hrpdMTUSizeTlv;

/**
 * This structure contains the parameters for EHRPD MTU Size.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  ehrpdMTUSize
 *          EHRPD size
 *          - Size of EHRPD MTU
 */
typedef struct
{
    uint8_t   TlvPresent;
    uint16_t  ehrpdMTUSize;
} swidms_ehrpdMTUSizeTlv;

/**
 * This structure contains the parameters for USB MTU Size
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  UsbMTUSize
 *          USB MTU size
 *          - Size of USB MTU
 */
typedef struct
{
    uint8_t   TlvPresent;
    uint16_t  UsbMTUSize;
} swidms_usbMTUSizeTlv;

/**
 *  This structure contains the Get MTU Response parameter.
 *
 *  @param  pMTUSize3gpp [Optional]
 *          - See @ref swidms_mtuSize3gppTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHrpdMTUSize [Optional]
 *          - See @ref swidms_hrpdMTUSizeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pEhrpdMTUSize [Optional]
 *          - See @ref swidms_ehrpdMTUSizeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pUsbMTUSize [Optional]
 *          - See @ref swidms_usbMTUSizeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    swidms_mtuSize3gppTlv         *pMTUSize3gpp;
    swidms_hrpdMTUSizeTlv         *pHrpdMTUSize;
    swidms_ehrpdMTUSizeTlv        *pEhrpdMTUSize;
    swidms_usbMTUSizeTlv          *pUsbMTUSize;
    swi_uint256_t                 ParamPresenceMask;
 } unpack_swidms_SLQSSwiDmsGetMTU_t;

/**
 * Get the preferred MTU Size of the 3GPP, HRPD, EHRPD and USB descriptor Interfaces pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swidms_SLQSSwiDmsGetMTU(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * Get the MTU Size of the 3GPP, HRPD, EHRPD and USB descriptor Interfaces unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swidms_SLQSSwiDmsGetMTU(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swidms_SLQSSwiDmsGetMTU_t *pOutput
        );

/**
 * This structure contains the parameters for host usb interface composition
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  CurrentCfgType
 *          - Current configure type, see values below:
 *            - 1 - MBIM
 *            - 2 - USBIF + PCIE
 *            - 3 - RMNET
 *            - 4 - RNDIS
 *            - 5 - PCIE only
 *
 *  @param  CfgValue
 *          - Host composition bit mask, see supported USB interface bitmasks,
 *            not supported by QMI object setting for 9x50 modules
 *            - 0x00000001 - DIAG interface
 *            - 0x00000002 - ADB interface
 *            - 0x00000004 - NMEA interface
 *            - 0x00000008 - MODEM interface
 *            - 0x00000010 - RESERVED5
 *            - 0x00000020 - RESERVED6
 *            - 0x00000040 - RESERVED7
 *            - 0x00000080 - RESERVED8
 *            - 0x00000100 - RMENT0 interface
 *            - 0x00000200 - RESERVED10
 *            - 0x00000400 - RMENT1 interface
 *            - 0x00000800 - RESERVED12
 *            - 0x00001000 - MBIM interface
 *            - 0x00002000 - RESERVED14
 *            - 0x00004000 - RNDIS interface
 *            - 0x00008000 - RESERVED16
 *            - 0x00010000 - AUDIO interface
 *            - 0x00020000 - RESERVED18
 *            - 0x00080000 - ECM interface
 *            - 0x00100000 - RESERVED21
 *            - 0x00200000 - RESERVED22
 *            - 0xFFC00000 - RESERVED
 *
 */
typedef struct
{
    uint8_t  TlvPresent;
    uint32_t CurrentCfgType;
    uint32_t CfgValue;
} swidms_intfaceCfgTlv;

/**
 * This structure contains the parameters for supported usb bitmasks
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @param  ValidBitmasks
 *          - for current configuration type, this is the supported bitmasks
 *            that the host can change.
 *
 */

typedef struct
{
    uint8_t  TlvPresent;
    uint32_t ValidBitmasks;
} swidms_supportedIntBitmaskTlv;


/**
 *  This structure contains the get usb composition response
 *
 *  @param  pInterfaceCfg
 *          - See @ref swidms_intfaceCfgTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSupportedBitmasks [Optional]
 *          - See @ref swidms_supportedIntBitmaskTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    swidms_intfaceCfgTlv          *pInterfaceCfg;
    swidms_supportedIntBitmaskTlv *pSupportedBitmasks;
    swi_uint256_t                 ParamPresenceMask;

 } unpack_swidms_SLQSSwiDmsGetUsbComp_t;

/**
 * Gets the usb interface composition pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swidms_SLQSSwiDmsGetUsbComp(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen
);

/**
 * Gets the usb interface composition unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swidms_SLQSSwiDmsGetUsbComp(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsGetUsbComp_t *pOutput
);


/**
 *  This structure contains the Set Usb Interface Config request parameter.
 *
 *  @param  CfgValue
 *          - Host composition bit mask, see supported USB interface bitmasks,
 *            not supported by QMI object setting for 9x50 modules
 *            - 0x00000001 - DIAG interface
 *            - 0x00000002 - ADB interface
 *            - 0x00000004 - NMEA interface
 *            - 0x00000008 - MODEM interface
 *            - 0x00000010 - RESERVED5
 *            - 0x00000020 - RESERVED6
 *            - 0x00000040 - RESERVED7
 *            - 0x00000080 - RESERVED8
 *            - 0x00000100 - RMENT0 interface
 *            - 0x00000200 - RESERVED10
 *            - 0x00000400 - RMENT1 interface
 *            - 0x00000800 - RESERVED12
 *            - 0x00001000 - MBIM interface
 *            - 0x00002000 - RESERVED14
 *            - 0x00004000 - RNDIS interface
 *            - 0x00008000 - RESERVED16
 *            - 0x00010000 - AUDIO interface
 *            - 0x00020000 - RESERVED18
 *            - 0x00080000 - ECM interface
 *            - 0x00100000 - RESERVED21
 *            - 0x00200000 - RESERVED22
 *            - 0xFFC00000 - RESERVED

 *
 */
typedef struct{
    uint32_t CfgValue;
}pack_swidms_SLQSSwiDmsSetUsbComp_t;

/**
 *  This structure is used to store unpack_swidms_SLQSSwiDmsSetUsbComp_t parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *           - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *           - bitmask representation to indicate valid parameters.
 *
 */
typedef struct{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_swidms_SLQSSwiDmsSetUsbComp_t;

/**
 * Sets the usb interface config value pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values. This setting will be effective after modem reboot
 */
int pack_swidms_SLQSSwiDmsSetUsbComp(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_swidms_SLQSSwiDmsSetUsbComp_t *reqArg
);

/**
 * Sets the usb interface config value unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values. This setting will be effective after modem reboot
 */
int unpack_swidms_SLQSSwiDmsSetUsbComp(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsSetUsbComp_t *pOutput
);



/**
 * This structure contains the parameters for hardware watchdog settings
 *
 *  @param  timeout
 *          - timeout value for HW watchdog (unit in second)
 *
 *  @param  resetDelay
 *          - delay before reset after watchdog timeout (unit in second)
 *
 *  @param  enable
 *          - 0 to disable watchdog; 1 to enable watchdog
 *
 *  @param  count
 *          - once <timeout> has occurred, the <count> will be increased by 1 and the timer 
 *            will be restarted automatically. This <count> indicates the number of renewals.
 *
 */
typedef struct
{
    uint32_t timeout;
    uint32_t resetDelay;
    uint8_t  enable;
    uint32_t count;
} swidms_SwiDmsGetHWWatchdog;

/**
 *  This structure contains the get hw watchdog response
 *
 *  @param  pHWWatchdog
 *          - See @ref swidms_SwiDmsGetHWWatchdog for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    swidms_SwiDmsGetHWWatchdog    *pHWWatchdog;
    swi_uint256_t                 ParamPresenceMask;

 } unpack_swidms_SLQSSwiDmsGetHWWatchdog_t;

/**
 * Gets the hardware watchdog settings pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_swidms_SLQSSwiDmsGetHWWatchdog(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
);

/**
 * Gets the hardware watchdog settings unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swidms_SLQSSwiDmsGetHWWatchdog(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsGetHWWatchdog_t *pOutput
);

/**
 *  This structure contains the set hardware watchdong settings request parameter.
 *
 *  @param  timeout
 *          - timeout value for HW watchdog (unit in second)
 *
 *  @param  resetDelay
 *          - delay before reset after watchdog timeout (unit in second)
 *
 *  @param  enable
 *          - 0 to disable watchdog; 1 to enable watchdog
 *
 */
typedef struct
{
    uint32_t timeout;
    uint32_t resetDelay;
    uint8_t  enable;
} pack_swidms_SLQSSwiDmsSetHWWatchdog_t;

/**
 *  This structure is used to store unpack_swidms_SLQSSwiDmsSetHWWatchdog_t parameters.
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *           - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *           - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_swidms_SLQSSwiDmsSetHWWatchdog_t;

/**
 * Sets the usb interface config value pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values.
 */
int pack_swidms_SLQSSwiDmsSetHWWatchdog(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    pack_swidms_SLQSSwiDmsSetHWWatchdog_t *reqArg
);

/**
 * Sets the hardware watchdog settings value unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values. This setting will be effective after modem reboot
 */
int unpack_swidms_SLQSSwiDmsSetHWWatchdog(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsSetHWWatchdog_t *pOutput
);

/**
 * pack function to get secure boot config and other capabilities
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int pack_swidms_SLQSSwiDmsGetSecureInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen);

/**
 * This structure contains secure boot config and other capabilities
 *
 *  @param  secureBootEnabled
 *          - secure boot enabled or disabled
 *            0- disabled
 *            1- enabled
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  memoryDumpAllowed
 *          - memory dump alloweed or disallowed
 *            0- disallowed
 *            1- allowed
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  @param  jtagAccessAllowed
 *          - Jtag access allowed or disallowed
 *            0- disallowed
 *            1- allowed
 *          - Bit to check in ParamPresenceMask - <B>4</B>
 *
 *  @param  Tlvresult
 *           - unpack Tlv result
 *           - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    uint8_t                  secureBootEnabled;
    uint8_t                  memoryDumpAllowed;
    uint8_t                  jtagAccessAllowed;
    uint16_t                 TlvResult;
    swi_uint256_t            ParamPresenceMask;
}unpack_swidms_SLQSSwiDmsGetSecureInfo_t;

/**
 * unpack function to get secure boot config
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_swidms_SLQSSwiDmsGetSecureInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_swidms_SLQSSwiDmsGetSecureInfo_t *pOutput);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif

