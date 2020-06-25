/**
 * \ingroup liteqmi
 *
 * \file nas.h
 */
#ifndef __LITEQMI_NAS_H__
#define __LITEQMI_NAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common.h"
#define NAS_OTA_MESSAGE_MAX_BUF_SIZE 2048
#define NAS_MAX_NUM_NETWORKS 30
#define NAS_MAX_DESCRIPTION_LENGTH           255
#define NAS_PLMN_LENGTH                      3
#define NAS_MAX_SCC_RX_INFO_INSTANCES        255
#define NAS_NAM_NAME_LENGTH                  12
#define NAS_IMSI_M_S1_LENGTH                 7
#define NAS_IMSI_M_S2_LENGTH                 3
#define  NAS_SERVING_SYSTEM_INFO_MAX_RADIO_INTERFACE_LIST 255
#define NAS_MAX_SLQS_NAS_PCI_INFO_PLMN_LENGTH 255
#define NAS_MAX_SLQS_NAS_PCI_INFO_LENGTH 255
#define NAS_MCC_MNC_INST_LENGTH    255
#define LITE_NW_SCAN_LTE_OP_MODE_MAX_LENGTH 255
#define NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE 255
#define LITE_MAX_NAS_3GPP2_MDN_LEN          15
#define LITE_MAX_SLQS_NAS_NW_NAME_SRC_LENGTH  255

/**
 * This structure contains unpack get signal strengths parameters.
 *
 *  @param len
 *          - number of rssi & radio items following
 *
 *  @param rssi
 *          - signal strength array
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param radio
 *          - radio interface array
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t len;
    signed char rssi[8];
    uint32_t radio[8];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetSignalStrengths_t;

/**
 * Returns the available signal strengths ( in dBm ) as measured by the
 * device in an array. This will also provides the corresponding radio
 * interfaces in an array. This API is deprecated on MC73xx/EM73xx modules
 * since firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use unpack_nas_SLQSNasGetSigInfo() for new firmware versions
 * and new modules
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetSignalStrengths(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetSignalStrengths_t *pOutput
        );

/**
 * get signal strengths pack. This API is deprecated on MC73xx/EM73xx modules
 * since firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use pack_nas_SLQSNasGetSigInfo() for new firmware versions
 * and new modules
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReq qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetSignalStrengths(
        pack_qmi_t *pCtx,
        uint8_t *pReq,
        uint16_t *pLen
        );

/**
 * This structure contains unpack get get system selection preferences parameters.
 *  @param  pEmerMode
 *          - Optional parameter specifying the emergency Mode
 *          - Values:
 *              - 0 - OFF (normal)
 *              - 1 - ON (Emergency)
+ *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pModePref
 *          - Optional parameter
 *          - Bit Mask indicating the radio technology mode preference
 *          - Bit values:
 *              - Bit 0 - cdma2000 1x
 *              - Bit 1 - cdma2000 HRPD(1xEV-DO)
 *              - Bit 2 - GSM
 *              - Bit 3 - UMTS
 *              - Bit 4 - LTE
+ *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pBandPref
 *          - Optional parameter
 *          - Bit mask representing the band preference
 *          - Bit values:
 *              - Bit 0 - Band Class 0, A-System
 *              - Bit 1 - Band Class 0, B-System, Band Class 0
 *                        AB, GSM 850 Band
 *              - Bit 2 - Band Class 1, all blocks
 *              - Bit 3 - Band Class 2 place holder
 *              - Bit 4 - Band Class 3, A-System
 *              - Bit 5 - Band Class 4, all blocks
 *              - Bit 6 - Band Class 5, all blocks
 *              - Bit 7 - GSM_DCS_1800 band
 *              - Bit 8 - GSM Extended GSM (E-GSM) 900 band
 *              - Bit 9 - GSM Primary GSM (P-GSM) 900 band
 *              - Bit 10 - Band Class 6
 *              - Bit 11 - Band Class 7
 *              - Bit 12 - Band Class 8
 *              - Bit 13 - Band Class 9
 *              - Bit 14 - Band Class 10
 *              - Bit 15 - Band Class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM Railways GSM 900 Band
 *              - Bit 21 - GSM PCS 1900 band
 *              - Bit 22 - WCDMA Europe, Japan, and China IMT 2100 band
 *              - Bit 23 - WCDMA U.S. PCS 1900 band
 *              - Bit 24 - WCDMA Europe and China DCS 1800 band
 *              - Bit 25 - WCDMA U.S. 1700 band
 *              - Bit 26 - WCDMA U.S. 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band Class 12
 *              - Bit 29 - Band Class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band Class 15
 *              - Bit 32 to 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bit 51 to 55 - Reserved
 *              - Bit 56 - Band Class 16
 *              - Bit 57 - Band Class 17
 *              - Bit 58 - Band Class 18
 *              - Bit 59 - Band Class 19
 *              - Bit 60 to 64 - Reserved
+ *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pPRLPref
 *          - Optional parameter indicating the CDMA PRL Preference
 *          - Values:
 *              - 0x0001 - Acquire available system only on the A side
 *              - 0x0002 - Acquire available system only on the B side
 *              - 0x3FFF - Acquire any available systems
+ *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pRoamPref
 *          - Optional parameter indicating the roaming Preference
 *          - Values:
 *              - 0x01 - Acquire only systems for which the roaming indicator
 *                       is off
 *              - 0x02 - Acquire a system as long as its roaming indicator is
 *                       not off
 *              - 0x03 - Acquire only systems for which the roaming indicator
 *                       is off or solid on, i.e. not flashing; CDMA only
 *              - 0xFF - Acquire systems, regardless of their roaming indicator
+ *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pLTEBandPref
 *          - Optional parameter
 *          - Bit mask representing the LTE band preference
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - All other bits are reserved
+ *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pNetSelPref
 *          - Optional parameter indicating network selection preference
 *          - Values:
 *              - 0x00 - Automatic network selection
 *              - 0x01 - Manual network selection.
+ *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pSrvDomainPref
 *          - Optional parameter indicating Service domain preference
 *          - Values:
 *              - 0x00 - Circuit switched only
 *              - 0x01 - Packet switched only
 *              - 0x02 - Circuit switched and packet switched
 *              - 0x03 - Packet switched attach
 *              - 0x04 - Packet switched detach
+ *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pGWAcqOrderPref
 *          - Optional parameter indicating GSM/WCDMA Acquisition
 *            order Preference
 *          - Values:
 *              - 0x00 - Automatic
 *              - 0x01 - GSM then WCDMA
 *              - 0x02 - WCDMA then GSM
+ *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t              *pEmerMode;
    uint16_t             *pModePref;
    uint64_t             *pBandPref;
    uint16_t             *pPRLPref;
    uint16_t             *pRoamPref;
    uint64_t             *pLTEBandPref;
    uint8_t              *pNetSelPref;
    uint32_t             *pSrvDomainPref;
    uint32_t             *pGWAcqOrderPref;
    swi_uint256_t        ParamPresenceMask;
} unpack_nas_SLQSGetSysSelectionPref_t;

/**
 * Queries the different system selection preferences of the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSysSelectionPref(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Queries the different system selection preferences of the device unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSysSelectionPref(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSysSelectionPref_t *pOutput
        );

/**
 *  Contain the network selection preference.
 *
 *  @param  netReg
 *          - specifies one of the following actions:
 *              - 0x00 - Automatic registration
 *                     - Device registers according to its provisioning; mcc
 *                       and mnc fields are ignored
 *              - 0x01 - Manual Registration
 *                     - Device registers to specified network; mcc and mnc
 *                       must contain valid values
 *
 *  @param  mcc
 *          - MCC value. Range 0 to 999
 *
 *  @param  mnc
 *          - MNC value. Range 0 to 999
 *
 */
struct nas_netSelectionPref
{
    uint8_t netReg;
    uint16_t mcc;
    uint16_t mnc;
};

/**
 *  Contain the Acquisition Order Preference.
 *
 *  @param  acqOrdeLen
 *          - Number of sets of the following elements.
 *
 *  @param  pAcqOrder
 *          - Acquisition order preference to be set. Values:
 *            - 0x01 - NAS_RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - NAS_RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x04 - NAS_RADIO_IF_GSM - GSM
 *            - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *            - 0x08 - NAS_RADIO_IF_LTE - LTE
 *            - 0x09 - NAS_RADIO_IF_TDSCDMA - TD-SCDMA
 *
 */
struct nas_acqOrderPref
{
    uint8_t acqOrdeLen;
    uint8_t *pAcqOrder;
};

/**
 *  Contain the CSGID.
 *
 *  @param  mcc
 *          - MCC value. Range 0 to 999
 *
 *  @param  mnc
 *          - MNC value. Range 0 to 999
 *
 *   @param  mncPcsDigits
 *           - TRUE - MNC is a three-digit value; e.g., a reported
 *                   value of 90 corresponds to an MNC value of 090
 *           - FALSE - MNC is a two-digit value; e.g., a reported
 *                   value of 90 corresponds to an MNC value of 90
 *   @param   id
 *            - Closed subscriber group identifier.
 *
 *   @param   rat
 *            - Radio interface technology of the CSG network.
 *              Values:
 *              - 0x04 - RADIO_IF_GSM - GSM
 *              - 0x05 - RADIO_IF_UMTS - UMTS
 *              - 0x08 - RADIO_IF_LTE - LTE
 *              - 0x09 - RADIO_IF_TDSCDMA - TDS
 *
 */
struct nas_CSGID
{
    uint16_t  mcc;
    uint16_t  mnc;
    uint8_t  mncPcsDigits;
    uint32_t id;
    uint8_t  rat;
};

/**
 *  Contain the system selection preferences.
 *
 *  @param  pEmerMode
 *          - Optional parameter specifying the emergency Mode
 *          - Values:
 *              - 0 - OFF (normal)
 *              - 1 - ON (Emergency)
 *
 *  @param  pModePref
 *          - Optional parameter
 *          - Bit Mask indicating the radio technology mode preference
 *          - Bit values:
 *              - Bit 0 - cdma2000 1x
 *              - Bit 1 - cdma2000 HRPD(1xEV-DO)
 *              - Bit 2 - GSM
 *              - Bit 3 - UMTS
 *              - Bit 4 - LTE
 *
 *  @param  pBandPref
 *          - Optional parameter
 *          - Bit mask representing the band preference
 *          - Bit values:
 *              - Bit 0 - Band Class 0, A-System
 *              - Bit 1 - Band Class 0, B-System, Band Class 0
 *                        AB, GSM 850 Band
 *              - Bit 2 - Band Class 1, all blocks
 *              - Bit 3 - Band Class 2 place holder
 *              - Bit 4 - Band Class 3, A-System
 *              - Bit 5 - Band Class 4, all blocks
 *              - Bit 6 - Band Class 5, all blocks
 *              - Bit 7 - GSM_DCS_1800 band
 *              - Bit 8 - GSM Extended GSM (E-GSM) 900 band
 *              - Bit 9 - GSM Primary GSM (P-GSM) 900 band
 *              - Bit 10 - Band Class 6
 *              - Bit 11 - Band Class 7
 *              - Bit 12 - Band Class 8
 *              - Bit 13 - Band Class 9
 *              - Bit 14 - Band Class 10
 *              - Bit 15 - Band Class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM Railways GSM 900 Band
 *              - Bit 21 - GSM PCS 1900 band
 *              - Bit 22 - WCDMA Europe, Japan, and China IMT 2100 band
 *              - Bit 23 - WCDMA U.S. PCS 1900 band
 *              - Bit 24 - WCDMA Europe and China DCS 1800 band
 *              - Bit 25 - WCDMA U.S. 1700 band
 *              - Bit 26 - WCDMA U.S. 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band Class 12
 *              - Bit 29 - Band Class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band Class 15
 *              - Bit 32 to 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bit 51 to 55 - Reserved
 *              - Bit 56 - Band Class 16
 *              - Bit 57 - Band Class 17
 *              - Bit 58 - Band Class 18
 *              - Bit 59 - Band Class 19
 *              - Bit 60 to 64 - Reserved
 *
 *  @param  pPRLPref
 *          - Optional parameter indicating the CDMA PRL Preference
 *          - Values:
 *              - 0x0001 - Acquire available system only on the A side
 *              - 0x0002 - Acquire available system only on the B side
 *              - 0x3FFF - Acquire any available systems
 *
 *  @param  pRoamPref
 *          - Optional parameter indicating the roaming Preference
 *          - Values:
 *              - 0x01 - Acquire only systems for which the roaming indicator
 *                       is off
 *              - 0x02 - Acquire a system as long as its roaming indicator is
 *                       not off
 *              - 0x03 - Acquire only systems for which the roaming indicator
 *                       is off or solid on, i.e. not flashing; CDMA only
 *              - 0xFF - Acquire systems, regardless of their roaming indicator
 *
 *  @param  pLTEBandPref
 *          - Optional parameter
 *          - Bit mask representing the LTE band preference
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - All other bits are reserved
 *
 *  @param  pNetSelPref - netSelectionPref
 *          - Optional parameter for specifying Network Selection Preference
 *          - Modem selects networks based on this parameter(if present).
 *          - see \ref nas_netSelectionPref for more information
 *
 *  @param  pChgDuration
 *          - Optional parameter specifying the duration of the change
 *          - Values:
 *              - 0x00 - Power cycle - Remains active until the next device
 *                                     power cycle
 *              - 0x01 - Permanent - Remains active through power cycles until
 *                                   changed by client
 *              - Device will use "0x01 - permanent" as default if this
 *                parameter is omitted
 *
 *  @param  pMNCIncPCSDigStat
 *          - Optional parameter indicating if MNC includes PCS digit
 *          - Values:
 *              - TRUE - MNC is a 3 digit value; e.g., a reported value of
 *                       90 corresponds to an MNC value of 090
 *              - FALSE - MNC is a 2-digit value; e.g., a reported value of
 *                        90 corresponds to an MNC value of 90
 *
 *  @param  pSrvDomainPref
 *          - Optional parameter indicating Service domain preference
 *          - Values:
 *              - 0x00 - Circuit switched only
 *              - 0x01 - Packet switched only
 *              - 0x02 - Circuit switched and packet switched
 *              - 0x03 - Packet switched attach
 *              - 0x04 - Packet switched detach
 *
 *  @param  pGWAcqOrderPref
 *          - Optional parameter indicating GSM/WCDMA Acquisition
 *            order Preference
 *          - Values:
 *              - 0x00 - Automatic
 *              - 0x01 - GSM then WCDMA
 *              - 0x02 - WCDMA then GSM
 *
 *  \param  pTdscdmaBandPref
 *          - Optional parameter indicating bitmask representing the TD-SCDMA band
 *            preference to be set.
 *          - Values:
 *              - 0x01 - TD-SCDMA Band A
 *              - 0x02 - TD-SCDMA Band B
 *              - 0x04 - TD-SCDMA Band C
 *              - 0x08 - TD-SCDMA Band D
 *              - 0x10 - TD-SCDMA Band E
 *              - 0x20 - TD-SCDMA Band F
 *              - All other bits are reserved
 *
 *  \param  pAcqOrderPref - acqOrderPref
 *          - Optional parameter for specifying Acquisition Order Preference
 *          - see \ref nas_acqOrderPref for more information
 *
 *  \param  pSrvRegRestriction
 *          - Optional parameter indicating Network Selection Registration Restriction Preference
 *          - Values:
 *              - 0x00 - Device follows the normal registration process
 *              - 0x01 - Device camps on the network according to its provisioning,
 *                       but does not register
 *              - 0x02 - Device selects the network for limited service
 *              - All other values are reserved.
 *
 *  \param  pCSGID - CSGID
 *          - Optional parameter for specifying CSG ID
 *          - Either of pNetSelPref or pCSGID can be set.
 *          - see \ref nas_CSGID for more information
 *
 *  \param  pRAT
 *          - Optional parameter Radio Access Technology
 *            order Preference
 *          - Values:
 *              - 0x04 - GSM
 *              - 0x05 - UMTS
 *              - 0x08 - LTE
 *              - 0x09 - TDSCDMA
 */
typedef struct
{
    uint8_t                     *pEmerMode;
    uint16_t                    *pModePref;
    uint64_t                    *pBandPref;
    uint16_t                    *pPRLPref;
    uint16_t                    *pRoamPref;
    uint64_t                    *pLTEBandPref;
    struct nas_netSelectionPref *pNetSelPref;
    uint8_t                     *pChgDuration;
    uint8_t                     *pMNCIncPCSDigStat;
    uint32_t                    *pSrvDomainPref;
    uint32_t                    *pGWAcqOrderPref;
    uint64_t                    *pTdscdmaBandPref;
    struct nas_acqOrderPref     *pAcqOrderPref;
    uint32_t                    *pSrvRegRestriction;
    struct nas_CSGID            *pCSGID;
    unsigned char               *pRAT;
} pack_nas_SLQSSetSysSelectionPref_t;

typedef unpack_result_t  unpack_nas_SLQSSetSysSelectionPref_t;

/**
 *  Sets the different system selection preferences of the device pack.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_nas_SLQSSetSysSelectionPref(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSSetSysSelectionPref_t *pReqParam
        );

/**
 *  Sets the different system selection preferences of the device unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSetSysSelectionPref(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSetSysSelectionPref_t *pOutput
        );

/**
 *  Contain the LTE Band Preference Extended parameters
 *
 *  \param  bits_1_64
 *          - Bits 1 to 64 of the 256-bit LTE E-UTRA Operating Band bitmask.
 *
 *  \param  bits_65_128
 *          - Bits 65 to 128 of the 256-bit LTE E-UTRA Operating Band bitmask.
 *
 *  \param  bits_129_192
 *          - Bits 129 to 192 of the 256-bit LTE E-UTRA Operating Band bitmask.
 *
 *  \param  bits_193_256
 *          - Bits 193 to 256 of the 256-bit LTE E-UTRA Operating Band bitmask.
 *
 */
struct nas_lteBandPrefExt
{
    uint64_t bits_1_64;
    uint64_t bits_65_128;
    uint64_t bits_129_192;
    uint64_t bits_193_256;
};
/**
 *  Contain the CIOT Acquisition Order Preference.
 *
 *  @param  ciotAcqOrderLen
 *          - Number of sets of the following elements.
 *
 *  @param  pCiotAcqOrder
 *          - CIOT Acquisition order preference to be set.
 *          - Values
 *            - 0x01 - cdma2000 &reg;1X
 *            - 0x02 - cdma2000 &reg; HRPD (1xEV-DO)
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *            - 0x0a - LTE_M1
 *            - 0x0b - LTE_NB1
 *
 */
struct nas_ciotAcqOrderPref
{
    uint8_t  ciotAcqOrderLen;
    uint32_t *pCiotAcqOrder;
};

/**
 *  Contain the NR5G Band Preference.
 *
 *  @param  bits_1_64
 *          - Bits 1 to 64 of the 256-bit NR5G Operating Band bitmask.
 *
 *  @param  bits_65_128
 *          - Bits 65 to 128 of the 256-bit NR5G Operating Band bitmask.
 *
 *  @param  bits_129_192
 *          - Bits 129 to 192 of the 256-bit NR5G Operating Band bitmask.
 *
 *  @param  bits_193_256
 *          - Bits 193 to 256 of the 256-bit NR5G Operating Band bitmask.
 *
 */
struct nas_nr5gBandPref
{
    uint64_t bits_1_64;
    uint64_t bits_65_128;
    uint64_t bits_129_192;
    uint64_t bits_193_256;
};

/**
 *  Contain the system selection preferences.
 *
 *  @param  pEmerMode
 *          - Optional parameter specifying the emergency Mode
 *          - Values:
 *              - 0 - OFF (normal)
 *              - 1 - ON (Emergency)
 *
 *  @param  pModePref
 *          - Optional parameter
 *          - Bit Mask indicating the radio technology mode preference
 *          - Bit values:
 *              - Bit 0 - cdma2000 1x
 *              - Bit 1 - cdma2000 HRPD(1xEV-DO)
 *              - Bit 2 - GSM
 *              - Bit 3 - UMTS
 *              - Bit 4 - LTE
 *              - Bit 5 - TD-SCDMA
 *              - Bit 6 - NR5G
 *
 *  @param  pBandPref
 *          - Optional parameter
 *          - Bit mask representing the band preference
 *          - Bit values:
 *              - Bit 0 - Band Class 0, A-System
 *              - Bit 1 - Band Class 0, B-System, Band Class 0
 *                        AB, GSM 850 Band
 *              - Bit 2 - Band Class 1, all blocks
 *              - Bit 3 - Band Class 2 place holder
 *              - Bit 4 - Band Class 3, A-System
 *              - Bit 5 - Band Class 4, all blocks
 *              - Bit 6 - Band Class 5, all blocks
 *              - Bit 7 - GSM_DCS_1800 band
 *              - Bit 8 - GSM Extended GSM (E-GSM) 900 band
 *              - Bit 9 - GSM Primary GSM (P-GSM) 900 band
 *              - Bit 10 - Band Class 6
 *              - Bit 11 - Band Class 7
 *              - Bit 12 - Band Class 8
 *              - Bit 13 - Band Class 9
 *              - Bit 14 - Band Class 10
 *              - Bit 15 - Band Class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM Railways GSM 900 Band
 *              - Bit 21 - GSM PCS 1900 band
 *              - Bit 22 - WCDMA Europe, Japan, and China IMT 2100 band
 *              - Bit 23 - WCDMA U.S. PCS 1900 band
 *              - Bit 24 - WCDMA Europe and China DCS 1800 band
 *              - Bit 25 - WCDMA U.S. 1700 band
 *              - Bit 26 - WCDMA U.S. 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band Class 12
 *              - Bit 29 - Band Class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band Class 15
 *              - Bit 32 to 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bit 51 to 55 - Reserved
 *              - Bit 56 - Band Class 16
 *              - Bit 57 - Band Class 17
 *              - Bit 58 - Band Class 18
 *              - Bit 59 - Band Class 19
 *              - Bit 60 to 64 - Reserved
 *
 *  @param  pPRLPref
 *          - Optional parameter indicating the CDMA PRL Preference
 *          - Values:
 *              - 0x0001 - Acquire available system only on the A side
 *              - 0x0002 - Acquire available system only on the B side
 *              - 0x3FFF - Acquire any available systems
 *
 *  @param  pRoamPref
 *          - Optional parameter indicating the roaming Preference
 *          - Values:
 *              - 0x01 - Acquire only systems for which the roaming indicator
 *                       is off
 *              - 0x02 - Acquire a system as long as its roaming indicator is
 *                       not off
 *              - 0x03 - Acquire only systems for which the roaming indicator
 *                       is off or solid on, i.e. not flashing; CDMA only
 *              - 0xFF - Acquire systems, regardless of their roaming indicator
 *
 *  @param  pLTEBandPref
 *          - Optional parameter
 *          - Bit mask representing the LTE band preference
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - All other bits are reserved
 *
 *  @param  pNetSelPref - netSelectionPref
 *          - Optional parameter for specifying Network Selection Preference
 *          - Modem selects networks based on this parameter(if present).
 *          - Either of pNetSelPref or pCSGID can be set.
 *          - see @ref nas_netSelectionPref for more information
 *
 *  @param  pChgDuration
 *          - Optional parameter specifying the duration of the change
 *          - At least one system selection setting to be set if pChgDuration is populated.
 *          - Values:
 *              - 0x00 - Power cycle - Remains active until the next device
 *                                     power cycle
 *              - 0x01 - Permanent - Remains active through power cycles until
 *                                   changed by client
 *              - Device will use "0x01 - permanent" as default if this
 *                parameter is omitted
 *
 *  @param  pMNCIncPCSDigStat
 *          - Optional parameter indicating if MNC includes PCS digit
 *          - pNetSelPref is expected if MNC includes PCS digit is set to 1.
 *          - Values:
 *              - TRUE - MNC is a 3 digit value; e.g., a reported value of
 *                       90 corresponds to an MNC value of 090
 *              - FALSE - MNC is a 2-digit value; e.g., a reported value of
 *                        90 corresponds to an MNC value of 90
 *
 *  @param  pSrvDomainPref
 *          - Optional parameter indicating Service domain preference
 *          - Values:
 *              - 0x00 - Circuit switched only
 *              - 0x01 - Packet switched only
 *              - 0x02 - Circuit switched and packet switched
 *              - 0x03 - Packet switched attach
 *              - 0x04 - Packet switched detach
 *
 *  @param  pGWAcqOrderPref
 *          - Optional parameter indicating GSM/WCDMA Acquisition
 *            order Preference
 *          - Values:
 *              - 0x00 - Automatic
 *              - 0x01 - GSM then WCDMA
 *              - 0x02 - WCDMA then GSM
 *
 *  @param  pTdscdmaBandPref
 *          - Optional parameter indicating bitmask representing the TD-SCDMA band
 *            preference to be set.
 *          - Values:
 *              - 0x01 - TD-SCDMA Band A
 *              - 0x02 - TD-SCDMA Band B
 *              - 0x04 - TD-SCDMA Band C
 *              - 0x08 - TD-SCDMA Band D
 *              - 0x10 - TD-SCDMA Band E
 *              - 0x20 - TD-SCDMA Band F
 *              - All other bits are reserved
 *
 *  @param  pAcqOrderPref - acqOrderPref
 *          - Optional parameter for specifying Acquisition Order Preference
 *          - see @ref nas_acqOrderPref for more information
 *
 *  @param  pSrvRegRestriction
 *          - Optional parameter indicating Network Selection Registration Restriction Preference
 *          - Values:
 *              - 0x00 - Device follows the normal registration process
 *              - 0x01 - Device camps on the network according to its provisioning,
 *                       but does not register
 *              - 0x02 - Device selects the network for limited service
 *              - All other values are reserved.
 *
 *  @param  pCSGID - CSGID
 *          - Optional parameter for specifying CSG ID
 *          - Either of pNetSelPref or pCSGID can be set.
 *          - see @ref nas_CSGID for more information
 *
 *  @param  pRAT
 *          - Optional parameter Radio Access Technology
 *            order Preference
 *          - Values:
 *              - 0x04 - GSM
 *              - 0x05 - UMTS
 *              - 0x08 - LTE
 *              - 0x09 - TDSCDMA
 *              - 0x0A - LTE-M1
 *              - 0x0B - LTE-NB1
 *              - 0x0C - NR5G
 *
 *  @param  pLTEBandPrefExt[IN]
 *          - LTE Band Preference Extended
 *          - see @ref nas_lteBandPrefExt for more information
 *
 *  @param  pCiotLteOpModePref[IN]
 *          - CIOT LTE Operational Mode Preference
 *          - Bitmask representing the CIoT LTE operational mode preference to be set.
 *          - Values :
 *            - Bit 0 - LTE wideband
 *            - Bit 1 - LTE M1
 *            - Bit 2 - LTE NB1
 *            All unlisted bits are reserved for future use and the service point ignores them
 *            if used. If this TLV is not present, the modem will use the value read from the
 *            NV during bootup.
 *
 *  @param  pLteM1BandPref[IN]
 *          - Bitmask representing the LTE M1 band preference to be set.
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 *           - If this field is not present, the modem will use the
 *             M1 band preference value read from the NV.
 *
 *  @param  pLteNb1BandPref[IN]
 *          - Bitmask representing the LTE NB1 band preference to be set.
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 *          - If this field is not present, the modem will use the
 *            NB1 band preference value read from the NV.
 *
 *  @param  pCiotLteOpMode
 *          - This TLV is used when the net_sel_pref is MANUAL and RAT TLV is LTE.
 *          - This TLV indicates the the operational mode to be used during the manual
 *            LTE PLMN selection.
 *          - Values :
 *            - 0x00 - No service
 *            - 0x01 - Camped on LTE wideband
 *            - 0x02 - Camped on LTE M1
 *            - 0x03 - Camped on LTE NB1
 *
 *  @param  pCiotAcqOrderPref[IN]
 *          - CIOT Acquisition Order Preference
 *          - see @ref nas_ciotAcqOrderPref for more information
 *
 *  @param  pNr5gBandPref[IN]
 *          - NR5G Band Preference
 *          - see @ref nas_nr5gBandPref for more information
 *
 */
typedef struct
 {
    uint8_t                     *pEmerMode;
    uint16_t                    *pModePref;
    uint64_t                    *pBandPref;
    uint16_t                    *pPRLPref;
    uint16_t                    *pRoamPref;
    uint64_t                    *pLTEBandPref;
    struct nas_netSelectionPref *pNetSelPref;
    uint8_t                     *pChgDuration;
    uint8_t                     *pMNCIncPCSDigStat;
    uint32_t                    *pSrvDomainPref;
    uint32_t                    *pGWAcqOrderPref;
    uint64_t                    *pTdscdmaBandPref;
    struct nas_acqOrderPref     *pAcqOrderPref;
    uint32_t                    *pSrvRegRestriction;
    struct nas_CSGID            *pCSGID;
    unsigned char               *pRAT;
    struct nas_lteBandPrefExt   *pLTEBandPrefExt;
    uint64_t                    *pCiotLteOpModePref;
    uint64_t                    *pLteM1BandPref;
    uint64_t                    *pLteNb1BandPref;
    uint32_t                    *pCiotLteOpMode;
    struct nas_ciotAcqOrderPref *pCiotAcqOrderPref;
    struct nas_nr5gBandPref     *pNr5gBandPref;
} pack_nas_SLQSSetSysSelectionPrefExt_t;

typedef unpack_result_t  unpack_nas_SLQSSetSysSelectionPrefExt_t;

/**
 *  Sets the different system selection preferences of the device pack.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_nas_SLQSSetSysSelectionPrefExt(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSSetSysSelectionPrefExt_t *pReqParam
        );

/**
 *  Sets the different system selection preferences of the device unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSetSysSelectionPrefExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSetSysSelectionPrefExt_t *pOutput
        );

/**
 *  Provides information about the band preference.
 *
 *  @param  bandpreference
 *          - Bit mask representing the band preference to be set.
 *          - Bit position meanings:
 *              - 0 - BC0_A - Band Class 0, A-System
 *              - 1 - BC0_B - Band Class 0, B-System, Band Class 0
 *                    AB , GSM 850 Band
 *              - 2 - BC1 - Band Class 1, all blocks
 *              - 3 - BC2 - Band Class 2 place holder
 *              - 4 - BC3 - Band Class 3, A-System
 *              - 5 - BC4 - Band Class 4, all blocks
 *              - 6 - BC5 - Band Class 5, all blocks
 *              - 7 - GSM_DCS_1800 - GSM DCS band
 *              - 8 - GSM_EGSM_900 - GSM Extended GSM (E-GSM) band
 *              - 9 - GSM_PGSM_900 - GSM Primary GSM (P-GSM) band
 *              - 10 - BC6 - Band Class 6
 *              - 11 - BC7 - Band Class 7
 *              - 12 - BC8 - Band Class 8
 *              - 13 - BC9 - Band Class 9
 *              - 14 - BC10 - Band Class 10
 *              - 15 - BC11 - Band Class 11
 *              - 16 - GSM_450 - GSM 450 band
 *              - 17 - GSM_480 - GSM 480 band
 *              - 18 - GSM_750 - GSM 750 band
 *              - 19 - GSM_850 - GSM 850 band
 *              - 20 - GSM_RGSM_900 - GSM Railways GSM Band
 *              - 21 - GSM_PCS_1900 - GSM PCS band
 *              - 22 - WCDMA_I_IMT_2000 - WCDMA EUROPE JAPAN
 *                     and CHINA IMT 2100 band
 *              - 23 - WCDMA_II_PCS_1900 - WCDMA US PCS 1900 band
 *              - 24 - WCDMA_III_1700 - WCDMA EUROPE and CHINA DCS 1800 band
 *              - 25 - WCDMA_IV_1700 - WCDMA US 1700 band
 *              - 26 - WCDMA_V_850 - WCDMA US 850 band
 *              - 27 - WCDMA_VI_800 - WCDMA JAPAN 800 band
 *              - 28 - BC12 - Band Class 12
 *              - 29 - BC14 - Band Class 14
 *              - 30 - RESERVED_2 - Reserved 2
 *              - 31 - BC15 - Band Class 15
 *              - 32 - 47 - Reserved
 *              - 48 - WCDMA_VII_2600 - WCDMA EUROPE 2600 band
 *              - 49 - WCDMA_VIII_900 - WCDMA EUROPE and JAPAN 900 band
 *              - 50 - WCDMA_IX_1700 - WCDMA JAPAN 1700 band
 *              - 51 to 55 - Reserved
 *              - 56 - BBC16 - Band Class 16
 *              - 57 - BC17 - Band Class 17
 *              - 58 - BC18 - Band Class 18
 *              - 59 - BC19 - Band Class 19
 *              - 60 to 64 - Reserved
 *
 */
typedef struct
{
    uint64_t bandPref;
}pack_nas_SLQSSetBandPreference_t;

typedef unpack_result_t  unpack_nas_SLQSSetBandPreference_t;

/**
 * Pack sets the different system selection preferences of the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSetBandPreference(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSSetBandPreference_t *pReqParam
        );

/**
 * Unpack sets the different system selection preferences of the device.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 */
int unpack_nas_SLQSSetBandPreference(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSetBandPreference_t *pOutput
        );

/**
 *  This structure contains pack indication register parameters.
 *  @param  pSystemSelectionInd [Optional]
 *          - System Selection Preference indication registration. The
 *            callbacks would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pDDTMInd [Optional]
 *          - DDTM (Data Dedicated Transmission Mode) indication registration.
 *            The callbacks would not be invoked if the indication
 *            is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pServingSystemInd [Optional]
 *          - Serving System indication registration. The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pDualStandByPrefInd [Optional]
 *          - Dual Standby Preference indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSubscriptionInfoInd [Optional]
 *          - Subscription Information indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pNetworkTimeInd [Optional]
 *          - Network Time indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSysInfoInd [Optional]
 *          - System Information indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSignalStrengthInd [Optional]
 *          - Signal Strength indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pErrorRateInd [Optional]
 *          - Error Rate indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pHDRNewUATIAssInd [Optional]
 *          - HDR New UATI Assigned indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pHDRSessionCloseInd [Optional]
 *          - HDR Session Closed indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pManagedRoamingInd [Optional]
 *          - Managed Roaming indication registration.The callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *
 *  @param  pNetworkRejectInd [Optional]
 *          - Network reject information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSuppressSysInfoInd [Optional]
 *          - Controls the reporting of QMI_NAS_SYS_INFO_IND when only the
 *            reject_cause field has changed. Values:\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable- 0x01 - Enable
 *
 *  @param  pLTECphyCa [Optional]
 *          - LTE Physical Carrier Aggregation Information.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable (default value)
 *              - 0x01 - Enable
 *
 *  @param  pEdrxChangeInfoInd [Optional]
 *          - EDRX Change Info indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 */
typedef struct
{
    uint8_t *pSystemSelectionInd;
    uint8_t *pDDTMInd;
    uint8_t *pServingSystemInd;
    uint8_t *pDualStandByPrefInd;
    uint8_t *pSubscriptionInfoInd;
    uint8_t *pNetworkTimeInd;
    uint8_t *pSysInfoInd;
    uint8_t *pSignalStrengthInd;
    uint8_t *pErrorRateInd;
    uint8_t *pHDRNewUATIAssInd;
    uint8_t *pHDRSessionCloseInd;
    uint8_t *pManagedRoamingInd;
    uint8_t *pNetworkRejectInd;
    uint8_t *pSuppressSysInfoInd;
    uint8_t *pLTECphyCa;
    uint8_t *pEdrxChangeInfoInd;
} pack_nas_SLQSNasIndicationRegisterExt_t;

typedef unpack_result_t  unpack_nas_SLQSNasIndicationRegisterExt_t;

/**
 * Pack used to Registers/De-registers for different NAS (Network access service)
 * indications.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note  This function is used by a control point to register/deregister for
 *        different QMI_NAS indications. The control point's registration
 *        state variables, controlling registration for indications, are
 *        modified to reflect the settings indicated in the parameters that
 *        are present in the request message. At least one optional parameter
 *        must be present in the request.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasIndicationRegisterExt(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasIndicationRegisterExt_t *pReqParam
        );

/**
 * Unpack used to Registers/De-registers for different NAS (Network access service)
 * indications.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasIndicationRegisterExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasIndicationRegisterExt_t *pOutput
        );

/**
 *  This structure contains the SLQSNasIndicationRegisterExt request
 *  parameters.
 *
 *  @param  pSystemSelectionInd [Optional]
 *          - System Selection Preference indication registration. The following
 *            callbacks would not be invoked if the indication is disabled.\n
 *            @ref tFNRoamingIndicator @ref tFNDataCapabilities and
 *            @ref tFNServingSystem
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pDDTMInd [Optional]
 *          - DDTM (Data Dedicated Transmission Mode) indication registration.
 *            The following callbacks would not be invoked if the indication
 *            is disabled.\n
 *            @ref tFNDDTM
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pServingSystemInd [Optional]
 *          - Serving System indication registration. The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNBandPreference
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pDualStandByPrefInd [Optional]
 *          - Dual Standby Preference indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNDualStandByPref
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSubscriptionInfoInd [Optional]
 *          - Subscription Information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNSubscriptionInfo
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pNetworkTimeInd [Optional]
 *          - Network Time indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNNetworkTime
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSysInfoInd [Optional]
 *          - System Information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNSysInfo
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSignalStrengthInd [Optional]
 *          - Signal Strength indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNSigInfo
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pErrorRateInd [Optional]
 *          - Error Rate indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNErrRate
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pHDRNewUATIAssInd [Optional]
 *          - HDR New UATI Assigned indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNHDRUATIUpdate
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pHDRSessionCloseInd [Optional]
 *          - HDR Session Closed indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNHDRSessionClose
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pManagedRoamingInd [Optional]
 *          - Managed Roaming indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *            @ref tFNManagedRoaming
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pCurrentPLMNNameInd [Optional]
 *          - Current PLMN Name indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_CURRENT_PLMN_NAME_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pEMBMSStatusInd [Optional]
 *          - eMBMS Status indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_EMBMS_STATUS_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pRFBandInfoInd [Optional]
 *          - RF Band Information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_RF_BAND_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pNetworkRejectInd [Optional]
 *          - Network reject information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSuppressSysInfoInd [Optional]
 *          - Controls the reporting of QMI_NAS_SYS_INFO_IND when only the
 *            reject_cause field has changed. Values:\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pOperatorNameDataInd [Optional]
 *          - Operator Name Data indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_OPERATOR_NAME_DATA_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pCSPPLMNModeInd [Optional]
 *          - CSP PLMN Mode Bit indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_CSP_PLMN_MODE_BIT_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pRTREConfigurationInd [Optional]
 *          - RTRE Configuration indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_RTRE_CONFIG_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pIMSPrefStatusInd [Optional]
 *          - IMS Preference Status indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_IMS_PREF_STATUS_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pE911StateReadyStatusInd [Optional]
 *          - E911 State Ready Status indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_E911_STATE_READY_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pLTESIB16NetworkTimeInd [Optional]
 *          - LTE SIB16 Network Time indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_LTE_SIB16_NETWORK_TIME_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pLTECphyCa [Optional]
 *          - LTE Physical Carrier Aggregation Information indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSubscriptionChangeInd [Optional]
 *          - Subscription Change indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_SUBSCRIPTION_CHANGE_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSSACBarringInd [Optional]
 *          - Service-Specific Access Class Barring indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_SSAC_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pT3402TimerValueInd [Optional]
 *          - T3402 Timer Value indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_EMM_T3402_CHANGED_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pAccessClassBarringInd [Optional]
 *          - Access Class Barring indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_ACB_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pDataSubscriptionPriorityInd [Optional]
 *          - Data Subscription Priority indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_DATA_SUBS_PRIORITY_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pT3346TimerStatusChangeInd [Optional]
 *          - T3346 Timer Status Change indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_DATA_SUBS_PRIORITY_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pCallModeStatusInd [Optional]
 *          - Call Mode Status indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_CALL_MODE_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pSSACBarringExtInd [Optional]
 *          - Service-Specific Access Class Barring Ext indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_SSAC_CHANGE_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pManualNWScanFailureInd [Optional]
 *          - Manual Network Scan Failure indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_MANUAL_SCAN_FAIL_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pTimerExpiryInd [Optional]
 *          - Timer Expiry indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_TIMER_EXPIRY_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pEmergencyModeStatusInd [Optional]
 *          - Emergency Mode Status indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_EMERGENCY_MODE_STATUS_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pGcellInfoInd [Optional]
 *          - Gcell Info indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_GCELL_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pEdrxChangeInfoInd [Optional]
 *          - EDRX Change Info indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pLTERACHFailInd [Optional]
 *          - LTE RACH Failure Indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_LTE_RACH_FAIL_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  @param  pLTERRCTxInfoInd [Optional]
 *          - LTE RRC Tx Info Indication registration.The following callbacks
 *            would not be invoked if the indication is disabled.\n
 *          - QMI_NAS_LTE_RRC_TX_INFO_IND
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \note Atleast one parameter must be provided as request. 'NULL' value
 *        confirms that the indication value is not sent.
 */
typedef struct
{
    uint8_t *pSystemSelectionInd;
    uint8_t *pDDTMInd;
    uint8_t *pServingSystemInd;
    uint8_t *pDualStandByPrefInd;
    uint8_t *pSubscriptionInfoInd;
    uint8_t *pNetworkTimeInd;
    uint8_t *pSysInfoInd;
    uint8_t *pSignalStrengthInd;
    uint8_t *pErrorRateInd;
    uint8_t *pHDRNewUATIAssInd;
    uint8_t *pHDRSessionCloseInd;
    uint8_t *pManagedRoamingInd;
    uint8_t *pCurrentPLMNNameInd;
    uint8_t *pEMBMSStatusInd;
    uint8_t *pRFBandInfoInd;
    uint8_t *pNetworkRejectInd;
    uint8_t *pSuppressSysInfoInd;
    uint8_t *pOperatorNameDataInd;
    uint8_t *pCSPPLMNModeInd;
    uint8_t *pRTREConfigurationInd;
    uint8_t *pIMSPrefStatusInd;
    uint8_t *pE911StateReadyStatusInd;
    uint8_t *pLTESIB16NetworkTimeInd;
    uint8_t *pLTECphyCa;
    uint8_t *pSubscriptionChangeInd;
    uint8_t *pSSACBarringInd;
    uint8_t *pT3402TimerValueInd;
    uint8_t *pAccessClassBarringInd;
    uint8_t *pDataSubscriptionPriorityInd;
    uint8_t *pT3346TimerStatusChangeInd;
    uint8_t *pCallModeStatusInd;
    uint8_t *pSSACBarringExtInd;
    uint8_t *pManualNWScanFailureInd;
    uint8_t *pTimerExpiryInd;
    uint8_t *pEmergencyModeStatusInd;
    uint8_t *pGcellInfoInd;
    uint8_t *pEdrxChangeInfoInd;
    uint8_t *pLTERACHFailInd;
    uint8_t *pLTERRCTxInfoInd;
} pack_nas_SLQSNasIndicationRegisterV2_t;

typedef unpack_result_t  unpack_nas_SLQSNasIndicationRegisterV2_t;

/**
 * Pack used to Registers/De-registers for different NAS (Network access service)
 * indications.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note  This function is used by a control point to register/deregister for
 *        different QMI_NAS indications. The control point's registration
 *        state variables, controlling registration for indications, are
 *        modified to reflect the settings indicated in the parameters that
 *        are present in the request message. At least one optional parameter
 *        must be present in the request.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasIndicationRegisterV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasIndicationRegisterV2_t *pReqParam
        );

/**
 * Unpack used to Registers/De-registers for different NAS (Network access service)
 * indications.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasIndicationRegisterV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasIndicationRegisterV2_t *pOutput
        );

/**
 * This structure contains the RFBandInfo response parameters.
 *
 *  @param  radioInterface
 *          - Radio interface technology
 *              - See @ref Tables for Radio Interface
 *
 *  @param  activeBandClass
 *          - Active Band Class
 *              - See @ref Tables for Band Classes
 *
 *  @param  activeChannel
 *          - Active channel (0 if channel is not relevant to the
 *            reported technology)
 */
typedef struct {
    uint8_t  radioInterface;
    uint16_t activeBandClass;
    uint16_t activeChannel;
} RFBandInfoElements;

/**
 *  Structur to strore the RFInfoList
 *
 *  @param  instanceSize
 *          - Upon input, maximum number of elements that the RF info instances
 *            array can contain.
 *          - Upon successful output, actual number of elements in RF info
 *            instances array.
 *
 *  @param  RFBandInfoElements
 *          - RF info instances array
 *              - See @ref RFBandInfoElements for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t     instancesSize;
    RFBandInfoElements RFBandInfoElements[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetRFInfo_t;

/**
 * get rf info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetRFInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );
/**
 * get rf info unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetRFInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetRFInfo_t *pOutput
        );

/**
 * This structure contains the parameters for CDMA/WCDMA Signal Strength
 * Information
 *
 *  \param  rssi
 *          - RSSI in dBm (signed value).
 *          - A value of -125 dBm or lower is used to indicate No Signal.
 *
 *  \param  ecio
 *          - ECIO value representing negative 0.5 dBm increments, i.e.,
 *            2 means -1 dBm (14 means -7 dBm, 63 means -31.5 dBm).
 *
 */
typedef struct
{
    int8_t  rssi;
    int16_t ecio;
} cdmaSSInfo;

/**
 * This structure contains the parameters for HDR Signal Strength Information
 *
 *  \param  rssi
 *          - RSSI in dBm (signed value).
 *          - A value of -125 dBm or lower is used to indicate No Signal.
 *
 *  \param  ecio
 *          - ECIO value representing negative 0.5 dBm increments, i.e.,
 *            2 means -1 dBm (14 means -7 dBm, 63 means -31.5 dBm).
 *
 *  \param  sinr
 *          - SINR level.
 *          - SINR is only applicable for 1xEV-DO.
 *          - Valid levels are 0 to 8, where the maximum value for:
 *              - 0 - SINR_LEVEL_0 is -9 dB
 *              - 1 - SINR_LEVEL_1 is -6 dB
 *              - 2 - SINR_LEVEL_2 is -4.5 dB
 *              - 3 - SINR_LEVEL_3 is -3 dB
 *              - 4 - SINR_LEVEL_4 is -2 dB
 *              - 5 - SINR_LEVEL_5 is +1 dB
 *              - 6 - SINR_LEVEL_6 is +3 dB
 *              - 7 - SINR_LEVEL_7 is +6 dB
 *              - 8 - SINR_LEVEL_8 is +9 dB
 *              - 0xFF - Not Available
 *
 *  \param  io
 *          - Received IO in dBm.
 *          - IO is only applicable for 1xEV-DO.
 *
 */
typedef struct
{
    int8_t  rssi;
    int16_t ecio;
    uint8_t  sinr;
    int32_t io;
} hdrSSInfo;

/**
 * This structure contains the parameters for LTE Signal Strength Information
 *
 *  \param  rssi
 *          - RSSI in dBm (signed value).
 *          - A value of -125 dBm or lower is used to indicate No Signal.
 *            - For CDMA and UMTS, this indicates forward link
 *              pilot Ec
 *            - For GSM, this indicates received signal strength
 *
 *  \param  rsrq
 *          - RSRQ value in dB (signed integer value) as measured by L1.
 *          - Range: -3 to -20 (-3 means -3 dB, -20 means -20 dB).
 *
 *  \param  rsrp
 *          - Current RSRP in dBm as measured by L1.
 *          - Range: -44 to -140 (-44 means -44 dBm, -140 means -140 dBm).
 *
 *  \param  snr
 *          - SNR level as a scaled integer in units of 0.1 dB. e.g., -16
 *            dB has a value of -160 and 24.6 dB has a value of 246,
 *
 */
typedef struct
{
    int8_t  rssi;
    int8_t  rsrq;
    int16_t rsrp;
    int16_t snr;
} lteSSInfo;

/**
 *  This structure contains the TDSCDMA Signal Strength Info Extended
 *
 *  \param  rssi
 *          - Measured RSSI in dB
 *
 *  \param  rscp[Optional]
 *          - Measured RSCP in dBm
 *
 *  \param  ecio[Optional]
 *          - Measured ECIO in dBm.
 *
 *  \param  sinr[Optional]
 *          - Measured SINR in dB. -15 dB is sent to clients if the
 *            actual SINR is less than -15 dB
 *
 */
typedef struct
{
    float rssi;
    float rscp;
    float ecio;
    float sinr;
} tdscdmaSigInfoExt;

/**
 *  This structure contains the SLQSNasGetSigInfo response parameters.
 *
 *  @param  CDMASSInfo
 *          - See @ref cdmaSSInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  HDRSSInfo
 *          - See @ref hdrSSInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  GSMSSInfo
 *          - GSM signal strength is the RSSI in dBm (signed value).
 *          - A value of -125 dBm or lower is used to indicate No Signal.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  WCDMASSInfo
 *          - See @ref cdmaSSInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  LTESSInfo
 *          - See @ref lteSSInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    cdmaSSInfo CDMASSInfo;
    hdrSSInfo  HDRSSInfo;
    int8_t     GSMSSInfo;
    cdmaSSInfo WCDMASSInfo;
    lteSSInfo  LTESSInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasGetSigInfo_t;

/**
 * get sig info pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetSigInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * get sig info unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note This command queries the signal strength information for currently
 *       active RATs. Information is reported only if the corresponding
 *       RATs have signal strength values to be reported.
 *       If no signal strength information is available for any RAT, the
 *       response message contains only the mandatory response message
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetSigInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasGetSigInfo_t *pOutput
        );

/**
 * valid get sig info 
 * @param[in]   pResp       qmi response from modem
 * @param[in]   u8Info      value to check unpack_nas_SLQSNasGetSigInfo_t param valid.
 *                   - 0 - CDMASSInfo
 *                   - 1 - HDRSSInfo.
 *                   - 2 - GSMSSInfo.
 *                   - 3 - WCDMASSInfo.
 *                   - 4 - LTESSInfo.
 *
 * @return eQCWWAN_ERR_NONE on signal info valid, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_valid_nas_SLQSNasGetSigInfo(uint8_t *pResp,
        uint8_t u8Info);

/**
 *  Structure for storing the nasSigInfo indication parameters.
 *
 *  \param  pCDMASigInfo
 *          - See \ref cdmaSSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pHDRSigInfo
 *          - See \ref hdrSSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pGSMSigInfo
 *          - one byte value, GSM signal strength is the RSSI in dBm (signed
 *            value). A value of -125 dBm or lower is used to indicate No Signal
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pWCDMASigInfo
 *          - See \ref cdmaSSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pLTESigInfo
 *          - See \ref lteSSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  pRscp
 *          - RSCP of the Primary Common Control Physical
 *            Channel (PCCPCH) in dBm. Measurement range:
 *            -120 dBm to -25 dBm.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  pTDSCDMASigInfoExt
 *          - See \ref tdscdmaSigInfoExt for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    cdmaSSInfo        *pCDMASigInfo;
    hdrSSInfo         *pHDRSigInfo;
    int8_t            *pGSMSigInfo;
    cdmaSSInfo        *pWCDMASigInfo;
    lteSSInfo         *pLTESigInfo;
    int8_t            *pRscp;
    tdscdmaSigInfoExt *pTDSCDMASigInfoExt;
    swi_uint256_t     ParamPresenceMask;
} unpack_nas_SLQSNasSigInfoCallback_ind_t;

/**
 * Unpack Signal Information callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     sig info indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasSigInfoCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasSigInfoCallback_ind_t *pOutput
        );

/**
 *  This structure contains unpack get get home network parameters.  
 *  \param  mcc
 *          - Mobile country code (UMTS only).
 *          - A 16-bit integer representation of MCC. Range: 0 to
 *            999.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  mnc
 *          - Mobile network code (UMTS only).
 *          - A 16-bit integer representation of MNC. Range: 0 to
 *            999.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  name
 *          - Network name or description represented as a NULL terminated
 *            string (empty string returned when unknown) (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  sid
 *          - Home network system ID
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  nid
 *          - Home network ID\n
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint16_t    mcc;
    uint16_t    mnc;
    char        name[255];
    uint16_t    sid;
    uint16_t    nid;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetHomeNetwork_t;

/**
 * Retrieves information about the home network of the device unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetHomeNetwork(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetHomeNetwork_t *pOutput
        );

/**
 * get home network pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetHomeNetwork(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *
 *  This structure contains the 3GPP Home Network MNC parameters.
 *
 *  @param  is3GppNw[OUT]
 *          - Value
 *            - True - if TLV 0x01 corresponds to a 3GPP network
 *            - False - In all other cases.
 *
 *  @param  mccIncPcsDigit[OUT]
 *          - This field is used to interpret the length of the mobile_network_code
 *            reported in TLV 0x01.
 *          - Values
 *            - TRUE - MNC is a three-digit value if a reported value of 90 corresponds to an MNC
 *                     value of 090
 *            - FALSE - MNC is a two-digit value if a reported value of 90 corresponds to an MNC value of 90
 *
 *  @param  TlvPresent[OUT]
 *          - Tlv Present
 *
 *  \note  mccIncPcsDigit value is ignored if is3GppNw is FALSE.
 *
 */
typedef struct{
    uint8_t    is3GppNw;
    uint8_t    mccIncPcsDigit;
    uint8_t    TlvPresent;
}nas_homeNwMNC3GppTlv;

/**
 *  This structure contains the 3GPP Network Name Source parameters.
 *
 *  @param  NwNameSrc3Gpp[OUT]
 *          - 3GPP Network Name Source
 *          - Values
 *            - 0x00 - Unknown
 *            - 0x01 - Operator PLMN list and PLMN network name
 *            - 0x02 - Common PCN handset specification and operator name string
 *            - 0x03 - Network identity and time zone
 *            - 0x04 - GSMA SE13 table
 *            - 0x05 - Mobile country code and mobile network code
 *            - 0x06 - Service provider name
 *
 *
 *  @param  TlvPresent[OUT]
 *          - Tlv Present
 *
 *
 */
typedef struct{
    uint32_t   NwNameSrc3Gpp;
    uint8_t    TlvPresent;
}nas_nwNameSrc3GppTlv;

/**
 *  This structure contains unpack get get home network parameters.
 *  \param  mcc
 *          - Mobile country code (UMTS only).
 *          - A 16-bit integer representation of MCC. Range: 0 to
 *            999.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  mnc
 *          - Mobile network code (UMTS only).
 *          - A 16-bit integer representation of MNC. Range: 0 to
 *            999.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  name
 *          - Network name or description represented as a NULL terminated
 *            string (empty string returned when unknown) (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  sid
 *          - Home network system ID
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  nid
 *          - Home network ID\n
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pHomeNwMNC3Gpp[OUT]
 *          - 3GPP Home Network MNC
 *          - See \ref nas_homeNwMNC3GppTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pNwNameSrc3Gpp[OUT]
 *          - 3GPP Network Name Source
 *          - See \ref nas_nwNameSrc3GppTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint16_t    mcc;
    uint16_t    mnc;
    char        name[255];
    uint16_t    sid;
    uint16_t    nid;
    nas_homeNwMNC3GppTlv *pHomeNwMNC3Gpp;
    nas_nwNameSrc3GppTlv *pNwNameSrc3Gpp;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSGetHomeNetwork_t;

/**
 * Retrieves information about the home network of the device unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetHomeNetwork(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetHomeNetwork_t *pOutput
        );

/**
 * get home network pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetHomeNetwork(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  Structure for storing the service status information for CDMA and HDR
 *  networks.
 *
 *  @param  srvStatus
 *          - Service status of the system.
 *              - 0x00 - No service
 *              - 0x01 - Limited service
 *              - 0x02 - Service
 *              - 0x03 - Limited regional service
 *              - 0x04 - Power save
 *              - 0xFF - Not Available
 *
 *  @param  isPrefDataPath
 *          - Whether the RAT is the preferred data path.
 *              - 0x00 - Not preferred
 *              - 0x01 - Preferred
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t srvStatus;
    uint8_t isPrefDataPath;
} nas_SrvStatusInfo;

/**
 *  Structure for storing the service status information for GSM, WCDMA and
 *  LTE networks.
 *
 *  @param  srvStatus
 *          - Service status of the system.
 *              - 0x00 - No service
 *              - 0x01 - Limited service
 *              - 0x02 - Service
 *              - 0x03 - Limited regional service
 *              - 0x04 - Power save
 *              - 0xFF - Not Available
 *
 *  @param  trueSrvStatus
 *          - True service status of the system.
 *          - Not applicable to CDMA/HDR.
 *              - 0x00 - No service
 *              - 0x01 - Limited service
 *              - 0x02 - Service
 *              - 0x03 - Limited regional service
 *              - 0x04 - Power save
 *              - 0xFF - Not Available
 *
 *  @param  isPrefDataPath
 *          - Whether the RAT is the preferred data path.
 *              - 0x00 - Not preferred
 *              - 0x01 - Preferred
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t srvStatus;
    uint8_t trueSrvStatus;
    uint8_t isPrefDataPath;
} nas_GSMSrvStatusInfo;

/**
 *  Structure for storing the System Information common to CDMA, HDR, GSM,
 *  WCDMA and LTE networks.
 *
 *  @param  srvDomainValid
 *          - Indicates whether the service domain is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  srvDomain
 *          - Service domain registered on the system.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *              - 0xFF - Not Available
 *
 *  @param  srvCapabilityValid
 *          - Indicates whether the service capability is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  srvCapability
 *          - Current system's service capability.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *              - 0xFF - Not Available
 *
 *  @param  roamStatusValid
 *          - Indicates whether the roaming status is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  roamStatus
 *          - Current roaming status.
 *              - 0x00 - Off
 *              - 0x01 - On
 *              - 0x02 - Blinking
 *              - 0x03 - Out of the neighborhood
 *              - 0x04 - Out of the building
 *              - 0x05 - Preferred system
 *              - 0x06 - Available system
 *              - 0x07 - Alliance partner
 *              - 0x08 - Premium partner
 *              - 0x09 - Full service
 *              - 0x0A - Partial service
 *              - 0x0B - Banner is on
 *              - 0x0C - Banner is off
 *              - 0x0D to 0x3F - Reserved for Standard Enhanced Roaming
 *                               Indicator Numbers
 *              - 0x40 to 0x7F - Reserved for Non-Standard Enhanced Roaming
 *                               Indicator Numbers
 *              - 0x40 to 0xFF - Reserved.
 *              - 0xFF - Not Available
 *          - Values from 0x02 onward are only applicable for 3GPP2
 *
 *  @param  isSysForbiddenValid
 *          - Indicates whether the forbidden system is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  isSysForbidden
 *          - Whether the system is forbidden.
 *              - 0x00 - Not forbidden
 *              - 0x01 - Forbidden
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t srvDomainValid;
    uint8_t srvDomain;
    uint8_t srvCapabilityValid;
    uint8_t srvCapability;
    uint8_t roamStatusValid;
    uint8_t roamStatus;
    uint8_t isSysForbiddenValid;
    uint8_t isSysForbidden;
} nas_sysInfoCommon;

/**
 *  Structure for storing the CDMA System Information.
 *
 *  @param  sysInfoCDMA
 *          - See \ref nas_sysInfoCommon for more information.
 *
 *  @param  isSysPrlMatchValid
 *          - Indicates whether the system PRL match is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  isSysPrlMatch
 *          - Indicates whether the system is in a PRL.
 *          - Only applies to CDMA/HDR.
 *              - 0x00 - System is not in a PRL
 *              - 0x01 - System is in a PRL
 *              - 0xFF - Not Available
 *          - If the system is not in a PRL, roam_status carries the value from
 *            the default roaming indicator in the PRL.
 *          - If the system is in a PRL, roam_status is set to the value based
 *            on the standard specification.
 *
 *  @param  pRevInUseValid
 *          - Indicates whether the P_Rev in use is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  pRevInUse
 *          - The lesser of the base station P_Rev and mobile P_Rev
 *          - Only applicable for CDMA.
 *              - 0xFF - Not Available
 *
 *  @param  bsPRevValid
 *          - Indicates whether the base station P_Rev is valid
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  bsPRev
 *          - Base station P_Rev.
 *          - Only applicable for CDMA.
 *              - 0xFF - Not Available
 *
 *  @param  ccsSupportedValid
 *          - Indicates whether the supported concurrent service is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  ccsSupported
 *          - Whether concurrent service is supported.
 *          - Only applicable for CDMA.
 *              - 0x00 - Not supported
 *              - 0x01 - Supported
 *              - 0xFF - Not Available
 *
 *  @param  cdmaSysIdValid
 *          - Indicates whether the CDMA system ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  systemID
 *          - System ID.
 *              - 0xFFFF - Not Available
 *
 *  @param  networkID
 *          - Network ID.
 *              - 0xFFFF - Not Available
 *
 *  @param  bsInfoValid
 *          - Indicates whether the base station information is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  baseLat
 *          - Base station latitude in units of 0.25 sec.
 *          - Expressed as a two's complement signed number with positive
 *            numbers signifying North latitudes.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  baseLong
 *          - Base station longitude in units of 0.25 sec.
 *          - Expressed as a two's complement signed number with positive
 *            numbers signifying East latitudes.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  packetZoneValid
 *          - Indicates whether the packet zone is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  packetZone
 *          - Packet zone (8-bit).
 *              - 0xFFFF indicates no packet zone.
 *          - Only applicable for CDMA.
 *
 *  @param  networkIdValid
 *          - Indicates whether the network ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  MCC[NAS_PLMN_LENGTH]
 *          - Mobile Country Code.
 *          - MCC digits in ASCII characters
 *          - MCC wildcard value is returned as {'3', 0xFF, 0xFF}.
 *
 *  @param  MNC[NAS_PLMN_LENGTH]
 *          - Mobile Network Code.
 *          - MNC digits in ASCII characters
 *          - An unused byte is set to 0xFF.
 *          - MNC wildcard value is returned as {'7', 0xFF, 0xFF}.
 *
 */
typedef struct
{
    nas_sysInfoCommon sysInfoCDMA;
    uint8_t          isSysPrlMatchValid;
    uint8_t          isSysPrlMatch;
    uint8_t          pRevInUseValid;
    uint8_t          pRevInUse;
    uint8_t          bsPRevValid;
    uint8_t          bsPRev;
    uint8_t          ccsSupportedValid;
    uint8_t          ccsSupported;
    uint8_t          cdmaSysIdValid;
    uint16_t          systemID;
    uint16_t          networkID;
    uint8_t          bsInfoValid;
    uint16_t               baseId;
    uint32_t              baseLat;
    uint32_t              baseLong;
    uint8_t          packetZoneValid;
    uint16_t               packetZone;
    uint8_t          networkIdValid;
    uint8_t          MCC[NAS_PLMN_LENGTH];
    uint8_t          MNC[NAS_PLMN_LENGTH];
} nas_CDMASysInfo;

/**
 *  Structure for storing the HDR System Information.
 *
 *  @param  sysInfoHDR
 *          - See \ref nas_sysInfoCommon for more information.
 *
 *  @param  isSysPrlMatchValid
 *          - Indicates whether the system PRL match is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  isSysPrlMatch
 *          - Indicates whether the system is in a PRL.
 *          - Only applies to CDMA/HDR.
 *              - 0x00 - System is not in a PRL
 *              - 0x01 - System is in a PRL
 *              - 0xFF - Not Available
 *          - If the system is not in a PRL, roam_status carries the value from
 *            the default roaming indicator in the PRL.
 *          - If the system is in a PRL, roam_status is set to the value based
 *            on the standard specification.
 *
 *  @param  hdrPersonalityValid
 *          - Indicates whether the HDR personality is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  hdrPersonality
 *          - HDR personality information.
 *          - Only applicable for HDR.
 *              - 0x00 - None
 *              - 0x02 - HRPD
 *              - 0x03 - eHRPD
 *              - 0xFF - Not Available
 *
 *  @param  hdrActiveProtValid
 *          - Indicates whether the HDR active protocol revision information
 *            is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  hdrActiveProt
 *          - HDR active protocol revision information .
 *          - Only applicable for HDR.
 *              - 0x00 - None
 *              - 0x02 - HDR Rel 0
 *              - 0x03 - HDR Rel A
 *              - 0x04 - HDR Rel B
 *              - 0xFF - Not Available
 *
 *  @param  is856SysIdValid
 *          - Indicates whether the IS-856 system ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  is856SysId[SLQS_SYSTEM_ID_SIZE]
 *          - IS-856 system ID.
 *          - Only applicable for HDR.
 *
 */
typedef struct
{
   nas_sysInfoCommon sysInfoHDR;
   uint8_t          isSysPrlMatchValid;
   uint8_t          isSysPrlMatch;
   uint8_t          hdrPersonalityValid;
   uint8_t          hdrPersonality;
   uint8_t          hdrActiveProtValid;
   uint8_t          hdrActiveProt;
   uint8_t          is856SysIdValid;
   uint8_t          is856SysId[16];
} nas_HDRSysInfo;

/**
 *  Structure for storing the GSM System Information.
 *
 *  @param  sysInfoGSM
 *          - See \ref nas_sysInfoCommon for more information.
 *
 *  @param  lacValid
 *          - Indicates whether the location area code is valid..
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  lac
 *          - Location area code.
 *          - Only applies to 3GPP.
 *              - 0xFFFF - Not Available
 *
 *  @param  cellIdValid
 *          - Indicates whether the cell ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  cellId
 *          - Cell ID.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  regRejectInfoValid
 *          - Indicates whether the registration reject information is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  rejectSrvDomain
 *          - Type of service domain in which the registration is rejected.
 *              - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *              - 0xFF - Not Available
 *
 *  @param  rejCause
 *          - Reject cause values sent are specified in
 *            [3GPP TS 24.008, Section 10.5.3.6 and 10.5.5.14]
 *              - 0xFF - Not Available
 *
 *
 *  @param  networkIdValid
 *          - Indicates whether the network ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  MCC[NAS_PLMN_LENGTH]
 *          - Mobile Country Code.
 *          - MCC digits in ASCII characters
 *
 *  @param  MNC[NAS_PLMN_LENGTH]
 *          - Mobile Network Code.
 *          - MNC digits in ASCII characters
 *          - An unused byte is set to 0xFF.
 *          - In case of two-digit MNC values, the third (unused) digit is set to 0xFF.
 *            For example, 15 (a two-digit MNC) is reported using the byte stream
 *            0x31 0x35 0xFF.
 *
 *  @param  egprsSuppValid
 *          - Indicates whether the EGPRS support is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  egprsSupp
 *          - EGPRS support indication.
 *          - Only applicable for GSM.
 *              - 0x00 - Not available
 *              - 0x01 - Available
 *              - 0xFF - Not Available
 *
 *  @param  dtmSuppValid
 *          - Indicates whether Dual Transfer mode support is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  dtmSupp
 *          - Dual Transfer mode support indication.
 *          - Only applicable for GSM.
 *              - 0x00 - Not available
 *              - 0x01 - Available
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
     nas_sysInfoCommon sysInfoGSM;
     uint8_t          lacValid;
     uint16_t          lac;
     uint8_t          cellIdValid;
     uint32_t         cellId;
     uint8_t          regRejectInfoValid;
     uint8_t          rejectSrvDomain;
     uint8_t          rejCause;
     uint8_t          networkIdValid;
     uint8_t          MCC[NAS_PLMN_LENGTH];
     uint8_t          MNC[NAS_PLMN_LENGTH];
     uint8_t          egprsSuppValid;
     uint8_t          egprsSupp;
     uint8_t          dtmSuppValid;
     uint8_t          dtmSupp;
} nas_GSMSysInfo;

/**
 *  Structure for storing the WCDMA System Information.
 *
 *  @param  sysInfoWCDMA
 *          - See \ref nas_sysInfoCommon for more information.
 *
 *  @param  lacValid
 *          - Indicates whether the location area code is valid..
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  lac
 *          - Location area code.
 *          - Only applies to 3GPP.
 *              - 0xFFFF - Not Available
 *
 *  @param  cellIdValid
 *          - Indicates whether the cell ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  cellId
 *          - Cell ID.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  regRejectInfoValid
 *          - Indicates whether the registration reject information is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  rejectSrvDomain
 *          - Type of service domain in which the registration is rejected.
 *              - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *              - 0xFF - Not Available
 *
 *  @param  rejCause
 *          - Reject cause values sent are specified in
 *            [3GPP TS 24.008, Section 10.5.3.6].
 *              - 0xFF - Not Available
 *
 *
 *  @param  networkIdValid
 *          - Indicates whether the network ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  MCC[NAS_PLMN_LENGTH]
 *          - Mobile Country Code.
 *          - MCC digits in ASCII characters
 *
 *  @param  MNC[NAS_PLMN_LENGTH]
 *          - Mobile Network Code.
 *          - MNC digits in ASCII characters
 *          - An unused byte is set to 0xFF.
 *          - In case of two-digit MNC values, the third (unused) digit is set to 0xFF.
 *            For example, 15 (a two-digit MNC) is reported using the byte stream
 *            0x31 0x35 0xFF.
 *
 *  @param  hsCallStatusValid
 *          - Indicates whether the high-speed call status is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  hsCallStatus
 *          - Call status on high speed.
 *          - Only applicable for WCDMA.
 *              - 0x00 - HSDPA and HSUPA are unsupported
 *              - 0x01 - HSDPA is supported
 *              - 0x02 - HSUPA is supported
 *              - 0x03 - HSDPA and HSUPA are supported
 *              - 0x04 - HSDPA+ is supported
 *              - 0x05 - HSDPA+ and HSUPA are supported
 *              - 0x06 - Dual-cell HSDPA+ is supported
 *              - 0x07 - Dual-cell HSDPA+ and HSUPA are supported
 *              - 0xFF - Not Available
 *
 *  @param  hsIndValid
 *          - Indicates whether high-speed service indication is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  hsInd
 *          - High-speed service indication
 *          - Only applicable for WCDMA.
 *              - 0x00 - HSDPA and HSUPA are unsupported
 *              - 0x01 - HSDPA is supported
 *              - 0x02 - HSUPA is supported
 *              - 0x03 - HSDPA and HSUPA are supported
 *              - 0x04 - HSDPA+ is supported
 *              - 0x05 - HSDPA+ and HSUPA are supported
 *              - 0x06 - Dual-cell HSDPA+ is supported
 *              - 0x07 - Dual-cell HSDPA+ and HSUPA are supported
 *              - 0xFF - Not Available
 *
 *  @param  pscValid
 *          - Indicates whether primary scrambling code is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  psc
 *          - Primary scrambling code.
 *              - 0xFFFF - Not Available
 *
 */
typedef struct
{
    nas_sysInfoCommon sysInfoWCDMA;
    uint8_t          lacValid;
    uint16_t          lac;
    uint8_t          cellIdValid;
    uint32_t         cellId;
    uint8_t          regRejectInfoValid;
    uint8_t          rejectSrvDomain;
    uint8_t          rejCause;
    uint8_t          networkIdValid;
    uint8_t          MCC[NAS_PLMN_LENGTH];
    uint8_t          MNC[NAS_PLMN_LENGTH];
    uint8_t          hsCallStatusValid;
    uint8_t          hsCallStatus;
    uint8_t          hsIndValid;
    uint8_t          hsInd;
    uint8_t          pscValid;
    uint16_t          psc;
} nas_WCDMASysInfo;

/**
 *  Structure for storing the LTE System Information.
 *
 *  @param  sysInfoLTE
 *          - See \ref nas_sysInfoCommon for more information.
 *
 *  @param  lacValid
 *          - Indicates whether the location area code is valid..
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  lac
 *          - Location area code.
 *          - Only applies to 3GPP.
 *              - 0xFFFF - Not Available
 *
 *  @param  cellIdValid
 *          - Indicates whether the cell ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  cellId
 *          - Cell ID.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  regRejectInfoValid
 *          - Indicates whether the registration reject information is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  rejectSrvDomain
 *          - Type of service domain in which the registration is rejected.
 *              - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *              - 0xFF - Not Available
 *
 *  @param  rejCause
 *          - Reject cause values sent are specified in
 *            [3GPP TS 24.008, Section 10.5.3.6].
 *            and
 *            [3GPP TS 24.301, Section 9.9.3.9]
 *              - 0xFF - Not Available
 *
 *
 *  @param  networkIdValid
 *          - Indicates whether the network ID is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  MCC[NAS_PLMN_LENGTH]
 *          - Mobile Country Code.
 *          - MCC digits in ASCII characters
 *
 *  @param  MNC[NAS_PLMN_LENGTH]
 *          - Mobile Network Code.
 *          - MNC digits in ASCII characters
 *          - An unused byte is set to 0xFF.
 *          - In case of two-digit MNC values, the third (unused) digit is set to 0xFF.
 *            For example, 15 (a two-digit MNC) is reported using the byte stream
 *            0x31 0x35 0xFF.
 *
 *  @param  tacValid
 *          - Indicates whether tracking area code is valid.
 *              - 0x00 - Invalid
 *              - 0x01 - Valid
 *              - 0xFF - Not Available
 *
 *  @param  tac
 *          - Tracking area code.
 *          - Only applicable for LTE.
 *              - 0xFFFF - Not Available
 *
 */
typedef struct
{
    nas_sysInfoCommon sysInfoLTE;
    uint8_t          lacValid;
    uint16_t          lac;
    uint8_t          cellIdValid;
    uint32_t         cellId;
    uint8_t          regRejectInfoValid;
    uint8_t          rejectSrvDomain;
    uint8_t          rejCause;
    uint8_t          networkIdValid;
    uint8_t          MCC[NAS_PLMN_LENGTH];
    uint8_t          MNC[NAS_PLMN_LENGTH];
    uint8_t          tacValid;
    uint16_t          tac;
} nas_LTESysInfo;

/**
 *  Structure for storing the Additional CDMA System Information.
 *
 *  @param  geoSysIdx
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *
 *  @param  regPrd
 *          - Registration period after the CDMA system is acquired.
 *          - When the CDMA registration period is not valid, 0xFFFF is used.
 *
 */
typedef struct
{
    uint16_t geoSysIdx;
    uint16_t regPrd;
} nas_AddCDMASysInfo;

/**
 *  Structure for storing the Additional GSM and WCDMA System Information.
 *
 *  @param  geoSysIdx
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *
 *  @param  cellBroadcastCap
 *          - Cell broadcast capability of the serving system.
 *          - When the CDMA registration period is not valid, 0xFFFF is used.
 *              - NAS_CELL_BROADCAST_CAP_UNKNOWN - Cell broadcast support is unknown
 *              - NAS_CELL_BROADCAST_CAP_OFF - Cell broadcast is not supported
 *              - NAS_CELL_BROADCAST_CAP_ON - Cell broadcast is supported
 *
 */
typedef struct
{
    uint16_t  geoSysIdx;
    uint32_t cellBroadcastCap;
} nas_AddSysInfo;

/**
 *  Structure for storing the GSM and WCDMA Call Barring System Information.
 *
 *  @param  csBarStatus
 *          - Call barring status for circuit-switched calls.
 *              - NAS_CELL_ACCESS_NORMAL_ONLY - Cell access is allowed for normal calls only
 *              - NAS_CELL_ACCESS_EMERGENCY_ONLY - Cell access is allowed for emergency calls only
 *              - NAS_CELL_ACCESS_NO_CALLS - Cell access is not allowed for any call type
 *              - NAS_CELL_ACCESS_ALL_CALLS - Cell access is allowed for all call types
 *              - NAS_CELL_ACCESS_UNKNOWN - Cell access type is unknown
 *
 *  @param  psBarStatus
 *          - Call barring status for packet-switched calls.
 *              - NAS_CELL_ACCESS_NORMAL_ONLY - Cell access is allowed for normal calls only
 *              - NAS_CELL_ACCESS_EMERGENCY_ONLY - Cell access is allowed for emergency calls only
 *              - NAS_CELL_ACCESS_NO_CALLS - Cell access is not allowed for any call type
 *              - NAS_CELL_ACCESS_ALL_CALLS - Cell access is allowed for all call types
 *              - NAS_CELL_ACCESS_UNKNOWN - Cell access type is unknown
 *
 */
typedef struct
{
    uint32_t csBarStatus;
    uint32_t psBarStatus;
} nas_CallBarringSysInfo;

/**
 *  This structure contains Camped CIOT LTE Operational Mode parameters.
 *
 *  @param  campedCiotLteOpMode
 *          - Indicates the camped CIoT LTE mode of operation.
 *          - Values:
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_ NB1 (0x03) - Camped on LTE NB1
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t campedCiotLteOpMode;
    uint8_t TlvPresent;
}nas_LteCiotOpModeTlv;

/**
 *  This structure contains NR5G Service Status Info parameters.
 *
 *  @param  srvStatus
 *          - Service status of the system.
 *          - Values:
 *            - 0x00 - SYS_SRV_STATUS_NO_SRV - No service
 *            - 0x01 - SYS_SRV_STATUS_LIMITED - Limited service
 *            - 0x02 - SYS_SRV_STATUS_SRV - Service
 *            - 0x03 - SYS_SRV_STATUS_LIMITED_REGIONAL - Limited regional service
 *            - 0x04 - SYS_SRV_STATUS_PWR_SAVE - Power save
 *
 *  @param  trueSrvStatus
 *          - True service status of the system (not applicable to CDMA/HDR).
 *          - Values:
 *            - 0x00 - SYS_SRV_STATUS_NO_SRV - No service
 *            - 0x01 - SYS_SRV_STATUS_LIMITED - Limited service
 *            - 0x02 - SYS_SRV_STATUS_SRV - Service
 *            - 0x03 - SYS_SRV_STATUS_LIMITED_REGIONAL - Limited regional service
 *            - 0x04 - SYS_SRV_STATUS_PWR_SAVE - Power save
 *
 *  @param  isPrefDataPath
 *          - Whether the RAT is the preferred data path:
 *          - Values
 *            - 0x00 - Not preferred
 *            - 0x01 - Preferred
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint8_t srvStatus;
    uint8_t trueSrvStatus;
    uint8_t isPrefDataPath;
    uint8_t TlvPresent;
}nas_NR5GSerStatTlv;

/**
 *  This structure contains NR5G System Info parameters.
 *
 *  @param  srvDomainValid
 *          - Indicates whether the service domain is valid.
 *
 *  @param  srvDomain
 *          - Service domain registered on the system.
 *            - Values:
 *              - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *              - 0x01 - SYS_SRV_DOMAIN_CS_ONLY - Circuit-switched only
 *              - 0x02 - SYS_SRV_DOMAIN_PS_ONLY - Packet-switched only
 *              - 0x03 - SYS_SRV_DOMAIN_CS_PS - Circuit-switched and packet-switched
 *              - 0x04 - SYS_SRV_DOMAIN_CAMPED -Camped
 *
 *  @param  srvCapValid
 *          - Indicates whether the service capability is valid.
 *
 *  @param  srvCapability
 *          - Current system service capability.
 *            - Values:
 *             - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *             - 0x01 - SYS_SRV_DOMAIN_CS_ONLY - Circuit-switched only
 *             - 0x02 - SYS_SRV_DOMAIN_PS_ONLY - Packet-switched only
 *             - 0x03 - SYS_SRV_DOMAIN_CS_PS - Circuit-switched and packet-switched
 *             - 0x04 - SYS_SRV_DOMAIN_CAMPED - Camped
 *
 *  @param  roamStatusValid
 *          - Indicates whether the roaming status is valid.
 *
 *  @param  roamStatus
 *          - Current roaming status.
 *            - Values:
 *             - 0x00 - SYS_ROAM_STATUS_OFF - Off
 *             - 0x01 - SYS_ROAM_STATUS_ON - On
 *             - 0x02 - SYS_ROAM_STATUS_BLINK - Blinking
 *             - 0x03 - SYS_ROAM_STATUS_OUT_OF_NEIGHBORHOOD - Out of the neighborhood
 *             - 0x04 - SYS_ROAM_STATUS_OUT_OF_BLDG - Out of the building
 *             - 0x05 - SYS_ROAM_STATUS_PREF_SYS - Preferred system
 *             - 0x06 - SYS_ROAM_STATUS_AVAIL_SYS - Available system
 *             - 0x07 - SYS_ROAM_STATUS_ALLIANCE_PARTNER - Alliance partner
 *             - 0x08 - SYS_ROAM_STATUS_PREMIUM_PARTNER - Premium partner
 *             - 0x09 - SYS_ROAM_STATUS_FULL_SVC - Full service
 *             - 0x0A - SYS_ROAM_STATUS_PARTIAL_SVC -Partial service
 *             - 0x0B - SYS_ROAM_STATUS_BANNER_ON -Banner is on
 *             - 0x0C - SYS_ROAM_STATUS_BANNER_OFF -Banner is off
 *            Remainder of the values are per 3GPP2 C.R1001-F.
 *            Values from 0x02 onward are only applicable for 3GPP2.
 *
 *  @param  sysForbiddenValid
 *          - Indicates whether the forbidden system is valid.
 *
 *  @param  sysForbidden
 *             - Whether the system is forbidden:
 *             - 0x00  Not forbidden
 *             - 0x01  Forbidden
 *
 *  @param  lacValid
 *             - Indicates whether the location area code is valid.
 *
 *  @param  lac
 *             - Location area code (only applicable for 3GPP).
 *
 *  @param  cellIdValid
 *             - Indicates whether the cell ID is valid.
 *
 *  @param  cellId
 *             - Cell ID.
 *
 *  @param  rejectSrvDomain
 *             - Type of service domain in which the registration is rejected.
 *             - Values:
 *             - 0x00 - SYS_SRV_DOMAIN_NO_SRV - No service
 *             - 0x01 - SYS_SRV_DOMAIN_CS_ONLY - Circuit-switched only
 *             - 0x02 - SYS_SRV_DOMAIN_PS_ONLY - Packet-switched only
 *             - 0x03 - SYS_SRV_DOMAIN_CS_PS - Circuit-switched and packet-switched
 *             - 0x04 - SYS_SRV_DOMAIN_CAMPED - Camped
 *
 *  @param  rejCause
 *             - Reject cause values sent are specified in 3GPP TS
 *             - 24.008 Sections 10.5.3.6 and 10.5.5.14, and 3GPP
 *             - TS 24.301 Section 9.9.3.9.
 *
 *  @param  nwIdValid
 *             - Indicates whether the network ID is valid.
 *
 *  @param  MCC
 *             - MCC digits in ASCII characters.For CDMA, the MCC wildcard value is returned 
 *               as{3, 0xFF, 0xFF}..
 *
 *  @param  MNC
 *             - MNC digits in ASCII characters. For this field:
 *             - Unused byte is set to 0xFF
 *             - In the case of two-digit MNC values, the third
 *               (unused) digit is set to 0xFF. For example, 15 (a two-digit MNC) is reported 
 *               using thebyte stream 0x35 0x31 0xFF.For CDMA, the MNC wildcard value is returned
 *               as {7, 0xFF, 0xFF}.
 *
 *  @param  tacValid
 *             - Indicates whether the tracking area code is valid.
 *
 *  @param  tac
 *             - Tracking area code (only applicable for NR5G).
 *
 *  @param  TlvPresent
 *             - Tlv Present or not
 */
typedef struct
{
    uint8_t srvDomainValid;
    uint8_t srvDomain;
    uint8_t srvCapValid;
    uint8_t srvcapability;
    uint8_t roamStatusValid;
    uint8_t roamStatus;
    uint8_t sysForbiddenValid;
    uint8_t sysForbidden;
    uint8_t lacValid;
    uint16_t lac;
    uint8_t cellIdValid;
    uint32_t cellId;
    uint8_t regRejectInfoValid;
    uint8_t rejectSrvDomain;
    uint8_t rejCause;
    uint8_t nwIdValid;
    int8_t MCC[NAS_PLMN_LENGTH];
    int8_t MNC[NAS_PLMN_LENGTH];
    uint8_t tacValid;
    uint16_t tac;
    uint8_t TlvPresent;
}nas_NR5GSystemInfoTlv;

/**
 *  Structure for storing the NR5G Cell Access Status Info parameters
 *
 *  @param  nr5gCellStatus
 *          - Cell access status for NR5G calls.
 *           - Values:
 *             - NAS_CELL_ACCESS_NORMAL_ ONLY (0x00) - Cell access is allowed for normal calls only
 *             - NAS_CELL_ACCESS_EMERGENCY_ ONLY (0x01) - Cell access is allowed for emergency calls only
 *             - NAS_CELL_ACCESS_NO_CALLS (0x02) - Cell access is not allowed for any call type
 *             - NAS_CELL_ACCESS_ALL_CALLS (0x03) - Cell access is allowed for all call types
 *             - NAS_CELL_ACCESS_UNKNOWN (-1) - Cell access type is unknown
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 *
 */
typedef struct
{
    uint32_t nr5gCellStatus;
    uint8_t TlvPresent;
}nas_NR5GCellStatusTlv;

/**
 *  This structure contains unpack get system information parameters.
 *  @param  pCDMASrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHDRSrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pGSMSrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pWCDMASrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pLTESrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCDMASysInfo
 *          - See \ref nas_CDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pHDRSysInfo
 *          - See \ref nas_HDRSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pGSMSysInfo
 *          - See \ref nas_GSMSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pWCDMASysInfo
 *          - See \ref nas_WCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pLTESysInfo
 *          - See \ref nas_LTESysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pAddCDMASysInfo
 *          - See \ref nas_AddCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pAddHDRSysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pAddGSMSysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pAddWCDMASysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pAddLTESysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pGSMCallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pWCDMACallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  pLTEVoiceSupportSysInfo
 *          - Indicates voice support status on LTE.
 *              - 0x00 - Voice is not supported
 *              - 0x01 - Voice is supported
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pGSMCipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pWCDMACipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pCampedCiotLteOpMode
 *          - Camped CIOT LTE Operational Mode.
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_NB1 (0x03) - Camped on LTE NB1
 *          - Bit to check in ParamPresenceMask - <B>73</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_SrvStatusInfo      *pCDMASrvStatusInfo;
    nas_SrvStatusInfo      *pHDRSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pGSMSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pWCDMASrvStatusInfo;
    nas_GSMSrvStatusInfo   *pLTESrvStatusInfo;
    nas_CDMASysInfo        *pCDMASysInfo;
    nas_HDRSysInfo         *pHDRSysInfo;
    nas_GSMSysInfo         *pGSMSysInfo;
    nas_WCDMASysInfo       *pWCDMASysInfo;
    nas_LTESysInfo         *pLTESysInfo;
    nas_AddCDMASysInfo     *pAddCDMASysInfo;
    uint16_t               *pAddHDRSysInfo;
    nas_AddSysInfo         *pAddGSMSysInfo;
    nas_AddSysInfo         *pAddWCDMASysInfo;
    uint16_t               *pAddLTESysInfo;
    nas_CallBarringSysInfo *pGSMCallBarringSysInfo;
    nas_CallBarringSysInfo *pWCDMACallBarringSysInfo;
    uint8_t                *pLTEVoiceSupportSysInfo;
    uint8_t                *pGSMCipherDomainSysInfo;
    uint8_t                *pWCDMACipherDomainSysInfo;
    uint32_t               *pCampedCiotLteOpMode;
    swi_uint256_t          ParamPresenceMask;
} unpack_nas_SLQSGetSysInfo_t;

/**
 * Pack get system information.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSysInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Provides the system information. This API is preferred when trying to get
 * the service status info and serving system info. The function unpack_nas_SLQSGetServingSystem_t()
 * reports similar NAS information, but it is deprecated. Please refer to the header
 * description of unpack_nas_SLQSGetServingSystem_t() for more information.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note This API queries current serving system information, including
 *       registration information and system property. The registration
 *       information for all RATs specified in the mode capability setting
 *       are included regardless of registration status. The RAT-specific
 *       system property are included only for RATs that are specified in
 *       the mode capability setting and which are not in either No Service
 *       or Power Save modes.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSysInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSysInfo_t *pOutput
        );

/**
 *  Structure for storing the LTE eMBMS Coverage Information.
 *
 *  \param  LteEmbmsCoverage
 *          - Values
 *            - TRUE - Current LTE system supports eMBMS
 *            - FALSE - Current LTE system does not support eMBMS
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint8_t       LteEmbmsCoverage;
    uint8_t       TLVPresent;
} nas_LteEmbmsCoverageTlv;

/**
 *  Structure for storing the SIM Reject Information.
 *
 *  \param  SimRejInfo
 *          - Current reject state information of the SIM.
 *          - Values
 *            - 0 - SIM is not available
 *            - 1 - SIM is available
 *            - 2 - SIM has been marked by the network as invalid
 *                  for circuit-switched services
 *            - 3 - SIM has been marked by the network as invalid for
 *                  packet-switched services
 *            - 4 - SIM has been marked by the network as invalid for
 *                  circuit-switched and packet-switched services
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint32_t      SimRejInfo;
    uint8_t       TLVPresent;
} nas_SimRejInfoTlv;

/**
 *  Structure for storing the IMS Voice Support Status on LTE
 *
 *  \param  ImsVoiceSupportLte
 *          - IMS Voice Support Status on LTE
 *          - Values
 *            - 0x00 - Support is not available
 *            - 0x01 - Support is available
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint8_t       ImsVoiceSupportLte;
    uint8_t       TLVPresent;
} nas_ImsVoiceSupportLteTlv;

/**
 *  Structure for storing the LTE Voice Domain
 *
 *  \param  LteVoiceDomain
 *          - LTE voice domain.
 *            - Values
 *              - 0 - Data-centric devices: No voice, stay on LTE
 *              - 1 - Voice is supported over the IMS network
 *              - 2 - Voice is supported over the 1X network
 *              - 3 - Voice is supported over the 3GPP network
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint32_t      LteVoiceDomain;
    uint8_t       TLVPresent;
} nas_LteVoiceDomainTlv;

/**
 *  Structure for storing the Network Selection Registration Restriction
 *
 *  \param  SrvRegRestriction
 *          - Registration restriction.
 *           - Values
 *             - 0x00 - Device follows the normal registration process
 *             - 0x01 - Device follows the camp-only registration process
 *             - All other values are reserved.
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint32_t      SrvRegRestriction;
    uint8_t       TLVPresent;
} nas_SrvRegRestrictionTlv;

/**
 *  Structure for storing the LTE Registration Domain
 *
 *  \param  LteRegDomain
 *          - LTE registration domain.
 *          - Values
 *            - 0 - Not applicable because the UE is not in Camp Only mode
 *            - 1 - UE is in Camp Only mode and the PLMN can provide CS service only
 *            - 2 - UE is in Camp Only mode and the PLMN can provide PS service only
 *            - 3 - UE is in Camp Only mode and the PLMN can provide CS and PS service
 *            - 4 - UE is in Camp Only
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint32_t      LteRegDomain;
    uint8_t       TLVPresent;
} nas_LteRegDomainTlv;

/**
 *  Structure for storing the LTE eMBMS Coverage Info Trace ID.
 *
 *  \param  LteEmbmsTraceId
 *          - LTE eMBMS coverage information trace ID.
 *          - Values
 *            - 0 to 32768 - Valid trace ID
 *            - -1 - Trace ID is not used
 *
 *  \param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    int16_t       LteEmbmsTraceId;
    uint8_t       TLVPresent;
} nas_LteEmbmsTraceIdTlv;

/**
 *  Structure for storing the NR5G Service Status Info.
 *
 *  @param  srvStatus
 *          - Service status of the system.
 *          - Values
 *            - 0x00 - No service
 *            - 0x01 - Limited service
 *            - 0x02 - Service
 *            - 0x03 - Limited regional service
 *            - 0x04 - Power save
 *
 *  @param  trueSrvStatus
 *          - True service status of the system (not applicable to CDMA/HDR).
 *          - Values
 *          - 0x00 - No service
 *          - 0x01 - Limited service
 *          - 0x02 - Service
 *          - 0x03 - Limited regional service
 *          - 0x04 - Power save
 *
 *  @param  isPrefDataPath
 *          - Whether the RAT is the preferred data path
 *          - Values
 *            - 0x00 - Not preferred
 *            - 0x01 - Preferred

 *  @param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint8_t       srvStatus;
    uint8_t       trueSrvStatus;
    uint8_t       isPrefDataPath;
    uint8_t       TLVPresent;
} nas_NR5GSrvStatusTlv;

/**
 *  Structure for storing the NR5G System Info.
 *
 *  @param  srvDomainValid
 *          - Indicates whether the service domain is valid.
 *
 *  @param  srvDomain
 *          - Service domain registered on the system.
 *            - Values
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *              - 0x04 - Camped
 *
 *  @param  srvCapabilityValid
 *          - Indicates whether the service capability is valid.
 *
 *  @param  srvCapability
 *          - Current system’s service capability.
 *          - Values
 *            - 0x00 - No service
 *            - 0x01 - Circuit-switched only
 *            - 0x02 - Packet-switched only
 *            - 0x03 - Circuit-switched and packet-switched
 *            - 0x04 - Camped
 *
 *  @param  roamStatusValid
 *          - Indicates whether the roaming status is valid.
 *
 *  @param  roamStatus
 *          - Current roaming status.
 *          - Values
 *          - 0x00 - Off
 *          - 0x01 - On
 *          - 0x02 - Blinking
 *          - 0x03 - Out of the neighborhood
 *          - 0x04 - Out of the building
 *          - 0x05 - Preferred system
 *          - 0x06 - Available system
 *          - 0x07 - Alliance partner
 *          - 0x08 - Premium partner
 *          - 0x09 - Full service
 *          - 0x0A - Partial service
 *          - 0x0B - Banner is on
 *          - 0x0C - Banner is off
 *          - Remainder of the values are per 3GPP2 C.R1001-F.
 *            Values from 0x02 onward are only applicable for 3GPP2.
 *
 *  @param  isSysForbiddenValid
 *          - Indicates whether the forbidden system is valid.
 *
 *  @param  isSysForbidden
 *          - Whether the system is forbidden
 *            - 0x00 - Not forbidden
 *            - 0x01 - Forbidden
 *
 *  @param  lacValid
 *          - Indicates whether the location area code is valid.
 *
 *  @param  lac
 *          - Location area code (only applicable for 3GPP).
 *
 *  @param  cellIdValid
 *          - Indicates whether the cell ID is valid.
 *
 *  @param  cellId
 *          - Cell ID.
 *
 *  @param  regRejectInfoValid
 *          - Indicates whether the registration reject information is valid.
 *
 *  @param  rejectSrvDomain
 *          - Type of service domain in which the registration is rejected.
 *          - Values
 *            - 0x00 - No service
 *            - 0x01 - Circuit-switched only
 *            - 0x02 - Packet-switched only
 *            - 0x03 - Circuit-switched and packet-switched
 *            - 0x04 - Camped
 *
 *  @param  rejectCause
 *          - Reject cause values sent are specified in 3GPP TS 24.008
 *            Sections 10.5.3.6 and 10.5.5.14, and
 *            3GPP TS 24.301 Section 9.9.3.9.
 *
 *  @param  nwIdValid
 *          - Indicates whether the network ID is valid.
 *
 *  @param  mcc
 *          - MCC digits in ASCII characters.
 *          - For CDMA, the MCC wildcard value is returned as {‘3’, 0xFF, 0xFF}.
 *
 *  @param  mnc
 *          - MNC digits in ASCII characters.
 *          - For this field
 *            - Unused byte is set to 0xFF
 *            - In the case of two-digit MNC values, the third (unused)
 *              digit is set to 0xFF. For example, 15 (a two-digit MNC)
 *              is reported using the byte stream 0x35 0x31 0xFF.
 *            - For CDMA, the MNC wildcard value is returned as {‘7’, 0xFF, 0xFF}.
 *
 *  @param  tacValid
 *          - Indicates whether the tracking area code is valid.
 *
 *  @param  tac
 *          - Tracking area code (only applicable for NR5G).
 *
 *  @param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint8_t        srvDomainValid;
    uint8_t        srvDomain;
    uint8_t        srvCapabilityValid;
    uint8_t        srvCapability;
    uint8_t        roamStatusValid;
    uint8_t        roamStatus;
    uint8_t        isSysForbiddenValid;
    uint8_t        isSysForbidden;
    uint8_t        lacValid;
    uint16_t        lac;
    uint8_t        cellIdValid;
    uint32_t       cellId;
    uint8_t        regRejectInfoValid;
    uint8_t        rejectSrvDomain;
    uint8_t        rejectCause;
    uint8_t        nwIdValid;
    uint8_t        mcc[NAS_PLMN_LENGTH];
    uint8_t        mnc[NAS_PLMN_LENGTH];
    uint8_t        tacValid;
    uint16_t        tac;
    uint8_t        TLVPresent;
} nas_NR5GSysInfoTlv;

/**
 *  Structure for storing the NR5G Cell Access Status Info
 *
 *  @param  nr5gCellStatus
 *          - Cell access status for NR5G calls.
 *           - Values
 *           - 0x00 - Cell access is allowed for normal calls only
 *           - 0x01 - Cell access is allowed for emergency calls only
 *           - 0x02 - Cell access is not allowed for any call type
 *           - 0x03 - Cell access is allowed for all call types
 *           - -1 - Cell access type is unknown
 *
 *  @param  TLVPresent
 *          - TLV Present
 */
typedef struct
{
    uint32_t       nr5gCellStatus;
    uint8_t        TLVPresent;
} nas_NR5GCellStatusInfoTlv;

/**
 *  This structure contains unpack get system information parameters.
 *
 *  @param  pCDMASrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHDRSrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pGSMSrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pWCDMASrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pLTESrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCDMASysInfo
 *          - See \ref nas_CDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pHDRSysInfo
 *          - See \ref nas_HDRSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pGSMSysInfo
 *          - See \ref nas_GSMSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pWCDMASysInfo
 *          - See \ref nas_WCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pLTESysInfo
 *          - See \ref nas_LTESysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pAddCDMASysInfo
 *          - See \ref nas_AddCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pAddHDRSysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pAddGSMSysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pAddWCDMASysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pAddLTESysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pGSMCallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pWCDMACallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  pLTEVoiceSupportSysInfo
 *          - Indicates voice support status on LTE.
 *              - 0x00 - Voice is not supported
 *              - 0x01 - Voice is supported
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pGSMCipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pWCDMACipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pCampedCiotLteOpMode
 *          - Camped CIOT LTE Operational Mode.
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_NB1 (0x03) - Camped on LTE NB1
 *          - Bit to check in ParamPresenceMask - <B>73</B>
 *
 *  @param  pLteEmbmsCoverage
 *          - See @ref nas_LteEmbmsCoverageTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  pSimRejInfo
 *          - See @ref nas_SimRejInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  pImsVoiceSupportLte
 *          - See @ref nas_ImsVoiceSupportLteTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  @param  pLteVoiceDomain
 *          - See @ref nas_LteVoiceDomainTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *  @param  pSrvRegRestriction
 *          - See @ref nas_SrvRegRestrictionTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>47</B>
 *
 *  @param  pLteRegDomain
 *          - See @ref nas_LteRegDomainTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>49</B>
 *
 *  @param  pLteEmbmsTraceId
 *          - See @ref nas_LteEmbmsTraceIdTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>52</B>
 *
 *  @param  pNR5GSrvStatusinfo
 *          - See @ref nas_NR5GSrvStatusTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>74</B>
 *
 *  @param  pNR5gSysInfo
 *          - See @ref nas_NR5GSysInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>75</B>
 *
 *  @param  pNR5GCellStatusInfo
 *          - See @ref nas_NR5GCellStatusInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>76</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_SrvStatusInfo      *pCDMASrvStatusInfo;
    nas_SrvStatusInfo      *pHDRSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pGSMSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pWCDMASrvStatusInfo;
    nas_GSMSrvStatusInfo   *pLTESrvStatusInfo;
    nas_CDMASysInfo        *pCDMASysInfo;
    nas_HDRSysInfo         *pHDRSysInfo;
    nas_GSMSysInfo         *pGSMSysInfo;
    nas_WCDMASysInfo       *pWCDMASysInfo;
    nas_LTESysInfo         *pLTESysInfo;
    nas_AddCDMASysInfo     *pAddCDMASysInfo;
    uint16_t               *pAddHDRSysInfo;
    nas_AddSysInfo         *pAddGSMSysInfo;
    nas_AddSysInfo         *pAddWCDMASysInfo;
    uint16_t               *pAddLTESysInfo;
    nas_CallBarringSysInfo *pGSMCallBarringSysInfo;
    nas_CallBarringSysInfo *pWCDMACallBarringSysInfo;
    uint8_t                *pLTEVoiceSupportSysInfo;
    uint8_t                *pGSMCipherDomainSysInfo;
    uint8_t                *pWCDMACipherDomainSysInfo;
    uint32_t               *pCampedCiotLteOpMode;
    nas_LteEmbmsCoverageTlv   *pLteEmbmsCoverage;
    nas_SimRejInfoTlv         *pSimRejInfo;
    nas_ImsVoiceSupportLteTlv *pImsVoiceSupportLte;
    nas_LteVoiceDomainTlv     *pLteVoiceDomain;
    nas_SrvRegRestrictionTlv  *pSrvRegRestriction;
    nas_LteRegDomainTlv       *pLteRegDomain;
    nas_LteEmbmsTraceIdTlv    *pLteEmbmsTraceId;
    nas_NR5GSrvStatusTlv      *pNR5GSrvStatusinfo;
    nas_NR5GSysInfoTlv        *pNR5GSysInfo;
    nas_NR5GCellStatusInfoTlv *pNR5GCellStatusInfo;
    swi_uint256_t             ParamPresenceMask;
} unpack_nas_SLQSGetSysInfoV2_t;

/**
 * Provides the system information. This API is preferred when trying to get
 * the service status info and serving system info. The function unpack_nas_SLQSGetServingSystem_t()
 * reports similar NAS information, but it is deprecated. Please refer to the header
 * description of unpack_nas_SLQSGetServingSystem_t() for more information.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note This API queries current serving system information, including
 *       registration information and system property. The registration
 *       information for all RATs specified in the mode capability setting
 *       are included regardless of registration status. The RAT-specific
 *       system property are included only for RATs that are specified in
 *       the mode capability setting and which are not in either No Service
 *       or Power Save modes.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSysInfoV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSysInfoV2_t *pOutput
        );

/**
 * Pack get system information.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSysInfoV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  This structure contains unpack system information callback indication parameters.
 *  @param  pCDMASrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHDRSrvStatusInfo
 *          - See \ref nas_SrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pGSMSrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pWCDMASrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pLTESrvStatusInfo
 *          - See \ref nas_GSMSrvStatusInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCDMASysInfo
 *          - See \ref nas_CDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pHDRSysInfo
 *          - See \ref nas_HDRSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pGSMSysInfo
 *          - See \ref nas_GSMSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pWCDMASysInfo
 *          - See \ref nas_WCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pLTESysInfo
 *          - See \ref nas_LTESysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pAddCDMASysInfo
 *          - See \ref nas_AddCDMASysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pAddHDRSysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  @param  pAddGSMSysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pAddWCDMASysInfo
 *          - See \ref nas_AddSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pAddLTESysInfo
 *          - System table index referencing the beginning of the geo in which
 *            the current serving system is present.
 *          - When the system index is not known, 0xFFFF is used.
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  @param  pGSMCallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pWCDMACallBarringSysInfo
 *          - See \ref nas_CallBarringSysInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  pLTEVoiceSupportSysInfo
 *          - Indicates voice support status on LTE.
 *              - 0x00 - Voice is not supported
 *              - 0x01 - Voice is supported
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pGSMCipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pWCDMACipherDomainSysInfo
 *          - Ciphering on the service domain.
 *              - 0x00 - No service
 *              - 0x01 - Circuit-switched only
 *              - 0x02 - Packet-switched only
 *              - 0x03 - Circuit-switched and packet-switched
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pSysInfoNoChange
 *          - System Info No Change.
 *          - Flag used to notify clients that a request to select a network
 *            ended with no change in the PLMN.
 *              - 0x01 - No change in system information
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  pLteCiotOpModeTlv
 *          - See \ref nas_LteCiotOpModeTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>75</B>
 *
 *  @param  pNR5GSerStatTlv
 *          - See \ref nas_NR5GSerStatTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>76</B>
 *
 *  @param  pNR5GSystemInfoTlv
 *          - See \ref nas_NR5GSystemInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>77</B>
 *
 *  @param  pNR5GCellStatusTlv
 *          - See \ref nas_NR5GCellStatusTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>78</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_SrvStatusInfo      *pCDMASrvStatusInfo;
    nas_SrvStatusInfo      *pHDRSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pGSMSrvStatusInfo;
    nas_GSMSrvStatusInfo   *pWCDMASrvStatusInfo;
    nas_GSMSrvStatusInfo   *pLTESrvStatusInfo;
    nas_CDMASysInfo        *pCDMASysInfo;
    nas_HDRSysInfo         *pHDRSysInfo;
    nas_GSMSysInfo         *pGSMSysInfo;
    nas_WCDMASysInfo       *pWCDMASysInfo;
    nas_LTESysInfo         *pLTESysInfo;
    nas_AddCDMASysInfo     *pAddCDMASysInfo;
    uint16_t               *pAddHDRSysInfo;
    nas_AddSysInfo         *pAddGSMSysInfo;
    nas_AddSysInfo         *pAddWCDMASysInfo;
    uint16_t               *pAddLTESysInfo;
    nas_CallBarringSysInfo *pGSMCallBarringSysInfo;
    nas_CallBarringSysInfo *pWCDMACallBarringSysInfo;
    uint8_t               *pLTEVoiceSupportSysInfo;
    uint8_t               *pGSMCipherDomainSysInfo;
    uint8_t               *pWCDMACipherDomainSysInfo;
    uint8_t               *pSysInfoNoChange;
    nas_LteCiotOpModeTlv   *pLteCiotOpModeTlv;
    nas_NR5GSerStatTlv     *pNR5GSerStatTlv;
    nas_NR5GSystemInfoTlv  *pNR5GSystemInfoTlv;
    nas_NR5GCellStatusTlv  *pNR5GCellStatus;
    swi_uint256_t         ParamPresenceMask;
} unpack_nas_SLQSSysInfoCallback_ind_t;

/**
 * Unpack system information callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasSysInfoCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSysInfoCallback_ind_t *pOutput
        );

/**
 *  This structure contains unpack get get serving network parameters.
 *  \param  RegistrationState
 *          - Registration state:
 *              - 0 - Not registered
 *              - 1 - Registered
 *              - 2 - Searching/Not Registered
 *              - 3 - Registration Denied
 *              - 4 - Unknown
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  CSDomain
 *          - Circuit switch domain status:
 *              - 0 - Unknown/Not Applicable
 *              - 1 - Attached
 *              - 2 - Detached
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  PSDomain
 *          - Packet switch domain status
 *              - 0 - Unknown/Not Applicable
 *              - 1 - Attached
 *              - 2 - Detached
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  RAN
 *          - Type of radio access network on which mobile is registered:\n
 *              - 0 - Unknown
 *              - 1 - cdma2000 network
 *              - 2 - UMTS network
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  RadioIfacesSize
 *          - Upon input, maximum number of elements that the radio interface
 *            array contain.
 *          - Upon successful output, actual number of elements in the radio
 *            interface array.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  RadioIfaces
 *          - An array of Radio Interface Technology
 *              - See qaGobiApiTableRadioInterfaces.h for the Radio Interface Technologies
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Roaming
 *          - Roaming indicator
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  MCC
 *          - Mobile country code
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  MNC
 *          - Mobile network code
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  nameSize
 *          - Maximum number of characters (including NULL terminator)
 *            that network name array can contain; applicable only for
 *            UMTS networks
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  Name
 *          - Network name or description represented as a NULL terminated
 *            string; empty string is returned when unknown; applicable only
 *            for UMTS networks
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  DataCapsLen[IN/OUT]
 *          - Upon input, the maximum number of elements the data capabilities
 *            array can contain.
 *          - Upon output, the actual number of elements in the data
 *            capabilities array.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  DataCaps[OUT]
 *          - Data capabilities array of unsigned long type
 *              - 1  - GPRS\n
 *              - 2  - EDGE\n
 *              - 3  - HSDPA\n
 *              - 4  - HSUPA\n
 *              - 5  - WCDMA\n
 *              - 6  - CDMA 1xRTT\n
 *              - 7  - CDMA 1xEV-DO Rev 0\n
 *              - 8  - CDMA 1xEV-DO Rev. A\n
 *              - 9  - GSM\n
 *              - 10 - EVDO Rev. B\n
 *              - 11 - LTE\n
 *              - 12 - HSDPA Plus\n
 *              - 13 - Dual Carrier HSDPA Plus\n
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t   RegistrationState;
    uint32_t   CSDomain;
    uint32_t   PSDomain;
    uint32_t   RAN;
    uint8_t    RadioIfacesSize;
    uint8_t    RadioIfaces[255];
    uint32_t   Roaming;
    uint16_t   MCC;
    uint16_t   MNC;
    uint8_t    nameSize;
    uint8_t    Name[255];
    uint8_t    DataCapsLen;
    uint8_t    DataCaps[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetServingNetwork_t;

/**
 * Pack provides information about the system that provides service to the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetServingNetwork(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Unpack provides information about the system that provides service to the device.
 * This API is deprecated on MC73xx/EM73xx modules since firmware version
 * SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions. Please use API
 * unpack_nas_SLQSGetSysInfo() for new firmware versions and new modules
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetServingNetwork(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetServingNetwork_t *pOutput
        );

/**
 *  This structure contains unpack get serving network capabilities parameters.
 *  \param  DataCapsLen
 *          - Upon input, the maximum number of elements the data capabilities
 *            array can contain.
 *          - Upon output, the actual number of elements in the data
 *            capabilities array.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  DataCaps[OUT]
 *          - Data capabilities array of unsigned long type
 *              - 1  - GPRS\n
 *              - 2  - EDGE\n
 *              - 3  - HSDPA\n
 *              - 4  - HSUPA\n
 *              - 5  - WCDMA\n
 *              - 6  - CDMA 1xRTT\n
 *              - 7  - CDMA 1xEV-DO Rev 0\n
 *              - 8  - CDMA 1xEV-DO Rev. A\n
 *              - 9  - GSM\n
 *              - 10 - EVDO Rev. B\n
 *              - 11 - LTE\n
 *              - 12 - HSDPA Plus\n
 *              - 13 - Dual Carrier HSDPA Plus\n
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t    DataCapsLen;
    uint8_t    DataCaps[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetServingNetworkCapabilities_t;

/**
 * Pack returns information regarding the data capabilities of the system
 * that currently provides service to the device.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetServingNetworkCapabilities(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Unpack returns information regarding the data capabilities of the system
 * that currently provides service to the device.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetServingNetworkCapabilities(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetServingNetworkCapabilities_t *pOutput
        );

/**
 *  Contain the 3GPP network information.
 *
 *  \param  MCC
 *          - Mobile Country Code
 *
 *  \param  MNC
 *          - Mobile Network Code
 *
 *  \param  InUse
 *          - Is the Network the current serving Network
 *              - 0 - Unknown
 *              - 1 - Current serving network
 *              - 2 - Not current serving network, available
 *
 *  \param  Roaming
 *          - Home/Roam Status of the Network
 *              - 0 - Unknown
 *              - 1 - Home
 *              - 2 - Roam
 *
 *  \param  Forbidden
 *          - Is the Network in the forbidden network list
 *              - 0 - Unknown
 *              - 1 - Forbidden
 *              - 2 - Not Forbidden
 *
 *  \param  Preferred
 *          - Is the Network in the Preferred network list
 *              - 0 - Unknown
 *              - 1 - Preferred
 *              - 2 - Not Preferred
 *
 *  \param  Description
 *          - Network Name/Description
 *          - This is a NULL terminated string.
 */
typedef struct
{
    uint16_t          MCC;
    uint16_t          MNC;
    uint32_t          InUse;
    uint32_t          Roaming;
    uint32_t          Forbidden;
    uint32_t          Preferred;
    char              Desription[255];
}nas_QmiNas3GppNetworkInfo;

/**
 *  Contain the 3GPP radio access technology information.
 *
 *  \param  MCC
 *          - Mobile Country Code
 *
 *  \param  MNC
 *          - Mobile Network Code
 *
 *  \param  RAT
 *          - Radio Access Technology
 *              - 0x04 - GERAN
 *              - 0x05 - UMTS
 *              - 0x08 - LTE
 *              - 0x09 - TD-SCDMA
 *
 */
typedef struct 
{
    uint16_t MCC;
    uint16_t MNC;
    uint8_t RAT;
}nas_QmiNas3GppNetworkRAT;


/**
 *  Contain the PCS Digit information
 *
 *  \param  MCC
 *          - Mobile Country Code
 *
 *  \param  MNC
 *          - Mobile Network Code
 *
 *  \param  includes_pcs_digit
 *          - this field is use to interpret the length of corresponding MNC reported
 *          - 0x01 - MNC is a three-digit value
 *          - 0x00 - MNC is a two-digit value
 *
 */
typedef struct 
{
    uint16_t MCC;
    uint16_t MNC;
    uint8_t includes_pcs_digit;
}nas_QmisNasPcsDigit;

/**
 *  Contain the PCI Cell Info.
 *
 *  \param  freq
 *          - Aboslute cell's frequency. Range 0 to 65535.
 *
 *  \param  cellID
 *          - Cell ID.
 *
 *  \param  GlobalCellID
 *          - Global Cell ID.
 *
 *  \param  PlmnLen
 *          - Number of Plmn(nasQmisNasPcsDigit) information sets.
 *
 *  \param nasQmisNasPcsDigit
 *          - See \ref nas_QmisNasPcsDigit for more information.
 *
 */
typedef struct
{
    uint32_t freq;
    uint16_t cellID;
    uint32_t GlobalCellID;
    uint8_t PlmnLen;
    nas_QmisNasPcsDigit nasQmisNasPcsDigit[NAS_MAX_SLQS_NAS_PCI_INFO_PLMN_LENGTH];
}nas_QmisNasSlqsNasPCICellInfo;

/**
 *  Contain the NAS Perform network scan PCI information
 *
 *  \param  PCICellInfoLen
 *          - Number of PCI Cell Info(PCICellInfo) information sets.
 *
 *  \param  nasQmisNasSlqsNasPCICellInfo
 *          - See \ref nas_QmisNasSlqsNasPCICellInfo for more information.
 *
 *  \param  rsrp
 *          - Combined RSRP.
 *
 *  \param  rsrpRx0
 *          - Rx0 RSRP.
 *
 *  \param  rsrpRx1
 *          - Rx1 RSRP.
 *
 *  \param  rsrq
 *          - Combined RSRQ.
 *
 *  \param  rsrqRx0
 *          - Rx0 RSRQ.
 *
 *  \param  rsrpRx1
 *          - Rx1 RSRQ.
 *
 */
typedef struct
{
    uint8_t PCICellInfoLen;
    nas_QmisNasSlqsNasPCICellInfo nasQmisNasSlqsNasPCICellInfo[NAS_MAX_SLQS_NAS_PCI_INFO_LENGTH];
    uint16_t rsrp;
    uint16_t rsrpRx0;
    uint16_t rsrpRx1;
    uint16_t rsrq;
    uint16_t rsrqRx0;
    uint16_t rsrqRx1;
}nas_QmisNasSlqsNasPCIInfo;

/**
 * This structure contains the LTE Operational Mode response parameters.
 *
 *  \param  lteOpModeLen
 *              - Number of sets of the following elements
 *                - MCC
 *                - MNC
 *                - lteOpMode
 *
 *  \param  MCC
 *          - Mobile Country Code
 *          - A 16-bit integer representation of MCC
 *          - Range: 0 to 999
 *
 *  \param  MNC
 *          - Mobile Network Code
 *          - A 16-bit integer representation of MNC
 *          - Range: 0 to 999
 *
 *  \param  lteOpMode
 *          - Indicates the LTE mode of operation.
 *          - Values:
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_NB1 (0x03) - Camped on LTE NB1
 *
 *  \param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct{
    uint8_t      lteOpModeLen;
    uint16_t     MCC[LITE_NW_SCAN_LTE_OP_MODE_MAX_LENGTH];
    uint16_t     MNC[LITE_NW_SCAN_LTE_OP_MODE_MAX_LENGTH];
    uint32_t     lteOpMode[LITE_NW_SCAN_LTE_OP_MODE_MAX_LENGTH];
    uint8_t      TlvPresent;
}nas_lteOpModeTlv;

/**
 *  Contain the network scan information.
 *
 *  \param  p3GppNetworkInstanceSize
 *          - Upon input, maximum number of elements that the network info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            network info instance array.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  p3GppNetworkInfoInstances
 *          - Network info instance array
 *              - See \ref nas_QmiNas3GppNetworkInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pRATInstanceSize
 *          - Upon input, maximum number of elements that the RAT info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            RAT info instance array.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pRATINstance
 *          - RAT info instance array
 *              - See \ref nas_QmiNas3GppNetworkRAT for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pPCSInstanceSize
 *          - Upon input, maximum number of elements that the PCS Digit info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            PCS Digit info instance array.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pPCSInstance
 *          - PCS Digit info instance array
 *              - See \ref nas_QmisNasPcsDigit for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pScanResult
 *          - status of network scan
 *          - 0x00 - scan successful
 *          - 0x01 - scan was aborted
 *          - 0x02 - scan did not complete due to a radio link failure recovery in progress
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pPCIInfo
 *          - PCI Information
 *              - See \ref nas_QmisNasSlqsNasPCIInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  pLteOpModeTlv[OUT]
 *          - LTE Operational Mode.
 *          - See \ref nas_lteOpModeTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t                    *p3GppNetworkInstanceSize;
    nas_QmiNas3GppNetworkInfo  *p3GppNetworkInfoInstances;
    uint8_t                    *pRATInstanceSize;
    nas_QmiNas3GppNetworkRAT   *pRATINstance;
    uint8_t                    *pPCSInstanceSize;
    nas_QmisNasPcsDigit        *pPCSInstance;
    uint32_t                   *pScanResult;
    nas_QmisNasSlqsNasPCIInfo  *pPCIInfo;
    nas_lteOpModeTlv           *pLteOpModeTlv;
    swi_uint256_t              ParamPresenceMask;
}unpack_nas_PerformNetworkScan_t;

/**
 * Pack performs scan for available networks and scans for RAT info as well.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_PerformNetworkScan(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Unpack performs scan for available networks and scans for RAT info as well.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_PerformNetworkScan(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_PerformNetworkScan_t *pOutput
        );

/**
 * This structure contains the Network Name Source parameters.
 *
 *  @param  nwNameSrcLen
 *              - Number of sets of the following elements
 *                - nwNameSrc
 *
 *  @param  nwNameSrc
 *          - Network name source.
 *          - Values
 *            - 0x00 - Unknown
 *            - 0x01 - Operator PLMN list and PLMN network name
 *            - 0x02 - Common PCN handset specification and operator name string
 *            - 0x03 - Network identity and time zone
 *            - 0x04 - GSMA SE13 table
 *            - 0x05 - Mobile country code and mobile network code
 *            - 0x06 - Service provider name
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct{
    uint8_t      nwNameSrcLen;
    uint32_t     nwNameSrc[LITE_MAX_SLQS_NAS_NW_NAME_SRC_LENGTH];
    uint8_t      TlvPresent;
}nas_networkNameSrcTlv;

/**
 *  Contain the network scan information.
 *
 *  \param  p3GppNetworkInstanceSize
 *          - Upon input, maximum number of elements that the network info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            network info instance array.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  p3GppNetworkInfoInstances
 *          - Network info instance array
 *              - See \ref nas_QmiNas3GppNetworkInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pRATInstanceSize
 *          - Upon input, maximum number of elements that the RAT info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            RAT info instance array.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pRATINstance
 *          - RAT info instance array
 *              - See \ref nas_QmiNas3GppNetworkRAT for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pPCSInstanceSize
 *          - Upon input, maximum number of elements that the PCS Digit info
 *            instance array can contain.
 *          - Upon successful output, the actual number of elements in the
 *            PCS Digit info instance array.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pPCSInstance
 *          - PCS Digit info instance array
 *              - See \ref nas_QmisNasPcsDigit for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pScanResult
 *          - status of network scan
 *          - 0x00 - scan successful
 *          - 0x01 - scan was aborted
 *          - 0x02 - scan did not complete due to a radio link failure recovery in progress
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pPCIInfo
 *          - PCI Information
 *              - See \ref nas_QmisNasSlqsNasPCIInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  pLteOpModeTlv[OUT]
 *          - LTE Operational Mode.
 *          - See \ref nas_lteOpModeTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  \param  pNetworkNameSrcTlv[OUT]
 *          - Network Name Source.
 *          - See \ref nas_networkNameSrcTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t                    *p3GppNetworkInstanceSize;
    nas_QmiNas3GppNetworkInfo  *p3GppNetworkInfoInstances;
    uint8_t                    *pRATInstanceSize;
    nas_QmiNas3GppNetworkRAT   *pRATINstance;
    uint8_t                    *pPCSInstanceSize;
    nas_QmisNasPcsDigit        *pPCSInstance;
    uint32_t                   *pScanResult;
    nas_QmisNasSlqsNasPCIInfo  *pPCIInfo;
    nas_lteOpModeTlv           *pLteOpModeTlv;
    nas_networkNameSrcTlv      *pNetworkNameSrcTlv;
    swi_uint256_t              ParamPresenceMask;
}unpack_nas_SLQSPerformNetworkScanV2_t;

/**
 * Unpack performs scan for available networks and scans for RAT info as well.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSPerformNetworkScanV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSPerformNetworkScanV2_t *pOutput
        );

/**
 * Pack performs scan for available networks and scans for RAT info as well.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSPerformNetworkScanV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  This structure contains paramaters unpack fetch CQI parameters for LTE data session.
 *  @param  ValidityCW0[OUT]
 *          - Values
 *              - 0- Invalid.
 *              - 1- Valid.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  CQIValueCW0[OUT]
 *          - Values
 *              - Range 0~15
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ValidityCW1[OUT]
 *          - Values
 *              - 0- Invalid.
 *              - 1- Valid.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  CQIValueCW1[OUT]
 *          - Values
 *              - Range 0~15
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t   ValidityCW0;
    uint8_t   CQIValueCW0;
    uint8_t   ValidityCW1;
    uint8_t   CQIValueCW1;
    swi_uint256_t  ParamPresenceMask;
}unpack_nas_SLQSSwiGetLteCQI_t;

/**
 * Pack fetch CQI parameters for LTE data session.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiGetLteCQI(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Unack fetch CQI parameters for LTE data session.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiGetLteCQI(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiGetLteCQI_t *pOutput
        );

/**
 *  Structure for storing the common information for the device.
 *
 *  @param  temperature
 *          - Temperature.
 *              - 8-bit signed integer
 *              - 0xFF - Not Available.
 *
 *  @param  modemMode
 *          - Modem Operating Mode.
 *              - 0x00 - POWERING OFF
 *              - 0x01 - FACTORY TEST
 *              - 0x02 - OFFLINE
 *              - 0x03 - OFFLINE_AMPS
 *              - 0x04 - OFFLINE_CDMA
 *              - 0x05 - ONLINE
 *              - 0x06 - LOW POWER MODE
 *              - 0x07 - RESETTING
 *              - 0x08 - NETWORK TEST
 *              - 0x09 - OFFLINE REQUEST
 *              - 0x0A - PSEUDO ONLINE
 *              - 0x0B - RESETTING MODEM
 *              - 0xFF - Unknown
 *
 *  @param  systemMode
 *          - System Acquisition Mode.
 *              - 0x00 - No service
 *              - 0x01 - AMPS
 *              - 0x02 - CDMA
 *              - 0x03 - GSM
 *              - 0x04 - HDR
 *              - 0x05 - WCDMA
 *              - 0x06 - GPS
 *              - 0x08 - WLAN
 *              - 0x09 - LTE
 *              - 0xFF - Unknown
 *
 *  @param  imsRegState
 *          - IMS Registration State.
 *              - 0x00 - NO SRV
 *              - 0x01 - IN PROG
 *              - 0x02 - FAILED
 *              - 0x03 - LIMITED
 *              - 0x04 - FULL SRV
 *              - 0xFF - Unknown
 *
 *  @param  psState
 *          - PS Attach State.
 *              - 0x00 - Attached
 *              - 0x01 - Detached
 *              - 0xFF - Unknown
 *
 */
typedef struct
{
    int8_t temperature;
    uint8_t modemMode;
    uint8_t systemMode;
    uint8_t imsRegState;
    uint8_t psState;
} nas_CommInfo;

/**
 *  Structure for storing the LTE information for the device.
 *
 *  @param  band
 *          - LTE Band
 *              - 1 ~ 40 (Band in decimal)
 *              - 0xFF - Invalid
 *
 *
 *  @param  bandwidth
 *          - BandWidth.
 *              - 0x00 - 1.4 MHz
 *              - 0x01 - 3 MHz
 *              - 0x02 - 5 MHz
 *              - 0x03 - 10 MHz
 *              - 0x04 - 15 MHz
 *              - 0x05 - 20 MHz
 *              - 0x06 - Invalid
 *              - 0xFF - Unknown
 *
 *  @param  RXChan
 *          - RX channel number in decimal
 *              - 0xFFFF - Not Available
 *
 *  @param  TXChan
 *          - TX channel number in decimal
 *              - 0xFFFF - Not Available
 *
 *  @param  emmState
 *          - EMM State.
 *              - 0x00 - Deregistered
 *              - 0x01 - Reg Initiated
 *              - 0x02 - Registered
 *              - 0x03 - TAU Initiated
 *              - 0x04 - SR Initiated
 *              - 0x05 - Dereg Initiated
 *              - 0x06 - Invalid
 *              - 0xFF - Unknown
 *
 *  @param  emmSubState
 *          - EMM Sub State.
 *              - 0xFF - NOT Applicable
 *          - When EMM_state is 0x00:
 *              - 0x00 - No IMSI
 *              - 0x01 - PLMN Search
 *              - 0x02 - Attach Needed
 *              - 0x03 - No Cell
 *              - 0x04 - Attaching
 *              - 0x05 - Normal Service
 *              - 0x06 - Limited Service
 *              - 0x07 - Waiting for PDN
 *          - When EMM_state is 0x01:
 *              - 0x00 - Waiting for NW
 *              - 0x01 - Waiting for ESM
 *          - When EMM_state is 0x02:
 *              - 0x00 - Normal Service
 *              - 0x01 - Update Needed
 *              - 0x02 - Attempt Update
 *              - 0x03 - No Cell
 *              - 0x04 - PLMN Search
 *              - 0x05 - Limited Service
 *              - 0x06 - MM Update
 *              - 0x07 - IMSI Detach
 *              - 0x08 - Waiting for ESM
 *
 *  @param  emmConnState
 *          - EMM Connected Mode State.
 *              - 0x00 - RRC Idle
 *              - 0x01 - Waiting RRC Cfm
 *              - 0x02 - RRC Connected
 *              - 0x03 - RRC Releasing
 *              - 0xFF - Unknown
 *
 */
typedef struct
{
    uint8_t band;
    uint8_t bandwidth;
    uint16_t RXChan;
    uint16_t TXChan;
    uint8_t emmState;
    uint8_t emmSubState;
    uint8_t emmConnState;
} nas_LTEInfo;

/**
 *  Structure for storing the SLQS Nas Swi Modem Status response parameters.
 *
 *  @param  commonInfo (mandatory)
 *          - See \ref nas_CommInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *
 *  @param  pLTEInfo (optional)
 *          - See \ref nas_LTEInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_CommInfo commonInfo;
    nas_LTEInfo  *pLTEInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasSwiModemStatus_t;


/**
 * This function requests the device to return the current status of modem.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasSwiModemStatus(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * This function requests the device to return the current status of modem.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasSwiModemStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasSwiModemStatus_t *pOutput
        );


/**
 * This structure contains the Serving System parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param regState
 *        - Registration state - Registration state of the mobile
 *        - Values:
 *            - 0 - Not Registered; mobile is not currently searching
 *                  for a new network to provide service
 *            - 1 - Registered with a network
 *            - 2 - Not registered, but mobile is currently searching
 *                  for a new network to provide service
 *            - 3 - Registration denied by visible network
 *            - 4 - Registration state is unknown
 *
 * @param csAttachState
 *        - CS Attach State - Circuit-switched domain attach state of the mobile
 *        - Values:
 *            - 0 - Unknown or not applicable
 *            - 1 - Attached
 *            - 2 - Detached
 *
 * @param psAttachState
 *        - PS Attach State - Packet-switched domain attach state of the mobile
 *        - Values:
 *            - 0 - Unknown or not applicable
 *            - 1 - Attached
 *            - 2 - Detached
 *
 * @param selNetwork
 *        - Selected Network - Type of selected radio access network
 *        - Values:
 *            - 0 - Unknown
 *            - 1 - 3GPP2 network
 *            - 2 - 3GPP network
 *
 * @param numRadioInterfaces
 *        - In Use Radio Interfaces Number
 *            - Number of radio interfaces currently in use
 *            - defaults to zero
 *
 * @param radioInterface
 *        - Radio Interface currently in use
 *        - Values:
 *            - 0x00 - RADIO_IF_NO_SVC - None(no service)
 *            - 0x01 - RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x03 - RADIO_IF_AMPS - AMPS
 *            - 0x04 - RADIO_IF_GSM - GSM
 *            - 0x05 - RADIO_IF_UMTS - UMTS
 *            - 0x08 - RADIO_IF_LTE - LTE
 *
 */
typedef struct {
    uint8_t regState;
    uint8_t csAttachState;
    uint8_t psAttachState;
    uint8_t selNetwork;
    uint8_t numRadioInterfaces;
    uint8_t radioInterface[32];
}nas_servSystem;

/**
 * This structure contains the data services capability
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param dataCapabilitiesLen
 *        - Length of data capabilities list
 *        - Defaults to zero
 *
 * @param dataCapabilities
 *        - List of data capabilities
 *        - Values:
 *            - 0x01 - GPRS
 *            - 0x02 - EDGE
 *            - 0x03 - HSDPA
 *            - 0x04 - HSUPA
 *            - 0x05 - WCDMA
 *            - 0x06 - CDMA
 *            - 0x07 - EV-DO Rev0
 *            - 0x08 - EV-DO RevA
 *            - 0x09 - GSM
 *            - 0x0A - EV-DO Rev B
 *            - 0x0B - LTE
 *            - 0x0C - HSDPA+
 *            - 0x0D - DC-HSDPA+
 *
 */
typedef struct {
    uint8_t dataCapabilitiesLen;
    uint8_t dataCapabilities[32];
}nas_dataSrvCapabilities;

/**
 * This structure contains the current PLMN parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param MCC
 *        - mobile country code
 *            - A 16 bit representation of MCC
 *            - Range 0 to 999
 *
 * @param MNC
 *        - mobile network code
 *            - A 16 bit representation of MNC
 *            - Range 0 to 999
 *
 * @param netDescrLength
 *        - Length of Network description field
 *        - Defaults to zero
 *
 * @param netDescr
 *        - Network Description
 *            - optional string containing network name or description
 *
 */
typedef struct {
    uint16_t MCC;
    uint16_t MNC;
    uint8_t netDescrLength;
    uint8_t netDescr[255];
}nas_currentPLMN;

/**
 * This structure contains the Roaming Indicator List
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param numInstances
 *        - number of sets of radio interface currently in use and roaming
 *          indicator
 *            - defaults to zero
 *
 * @param radioInterface
 *        - Radio Interface currently in use
 *        - Values:
 *            - 0x01 - RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x03 - RADIO_IF_AMPS - AMPS
 *            - 0x04 - RADIO_IF_GSM - GSM
 *            - 0x05 - RADIO_IF_UMTS - UMTS
 *            - 0x08 - RADIO_IF_LTE - LTE
 *
 * @param roamIndicator
 *        - Roaming Indicator
 *        - Values:
 *            - 0x00 - Roaming
 *            - 0x01 - Home
 *
 */
typedef struct {
    uint8_t numInstances;
    uint8_t radioInterface[32];
    uint8_t roamIndicator[32];
}nas_roamIndList;

/**
 * This structure contains the 3GPP2TimeZone parameters
 *
 * @param leapSeconds
 *        - leap seconds - Number of leap seconds since the start of
 *                          CDMA system time.
 *
 * @param localTimeOffset
 *        - Local Time Offset - Offset of system time in units of 30 minutes;
 *                             the value in this field conveys as 8 bit 2's
 *                             compliment number.
 *
 * @param daylightSavings
 *        - Day Light Savings Indicator
 *            - 0x00 - OFF (daylight savings not in effect)
 *            - 0x01 - ON (daylight savings in effect)
 *
 */
typedef struct {
    uint8_t leapSeconds;
    uint8_t localTimeOffset;
    uint8_t daylightSavings;
}nas_qaQmi3Gpp2TimeZone;

/**
 * This structure contains Detailed Service information
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param srvStatus
 *        - Service status
 *        - Values:
 *            - 0x00 - No service
 *            - 0x01 - Limited service
 *            - 0x02 - Service available
 *            - 0x03 - Limited regional service
 *            - 0x04 - MS in power save or deep sleep
 *
 * @param srvCapability
 *        - System's service capability
 *        - Values:
 *            - 0x00 - No Service
 *            - 0x01 - Circuit-switched only
 *            - 0x02 - Packet-switched only
 *            - 0x03 - Circuit-switched and packet-switched
 *            - 0x04 - MS found the right system but not yet
 *                     registered/attached
 *
 * @param hdrSrvStatus
 *        - HDR service status
 *        - Values:
 *            - 0x00 - No service
 *            - 0x01 - Limited service
 *            - 0x02 - Service available
 *            - 0x03 - Limited regional service
 *            - 0x04 - MS in power save or deep sleep
 *
 * @param hdrHybrid
 *        - HDR hybrid information
 *        - Values:
 *            - 0x00 - System is not hybrid
 *            - 0x01 - System is hybrid
 *
 * @param isSysForbidden
 *        - Forbidden system information
 *        - Values:
 *            - 0x00 - System is not a forbidden system
 *            - 0x01 - System is a forbidden system
 *
 */
typedef struct {
    uint8_t srvStatus;
    uint8_t srvCapability;
    uint8_t hdrSrvStatus;
    uint8_t hdrHybrid;
    uint8_t isSysForbidden;
}nas_detailSvcInfo;

/**
 * This structure contains CDMA system information extension
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param MCC
 *        - Mobile Country Code
 *
 * @param imsi_11_12
 *        - IMSI_11_12
 */
typedef struct {
    uint16_t MCC;
    uint8_t imsi_11_12;
}nas_CDMASysInfoExt;

/**
 * This structure contains Call Barring Status.
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 * @param csBarStatus
 *        - Call Barring Status for circuit-switched calls.
 *        - Values:
 *        - NAS_CELL_ACCESS_NORMAL_ONLY - Cell access is allowed for normal
 *                                        calls only
 *        - NAS_CELL_ACCESS_EMERGENCY_ONLY - Cell access is allowed for
 *                                           emergency calls only
 *        - NAS_CELL_ACCESS_NO_CALLS - Cell access is not allowed for any
 *                                     call type
 *        - NAS_CELL_ACCESS_ALL_CALLS - Cell access is allowed for all call
 *                                      types
 *        - NAS_CELL_ACCESS_UNKNOWN - Cell access type is unknown
 *
 * @param psBarStatus
 *        - Call Barring Status for packet-switched calls.
 *        - Values:
 *            - NAS_CELL_ACCESS_NORMAL_ONLY - Cell access is allowed for normal
 *                                            calls only
 *            - NAS_CELL_ACCESS_EMERGENCY_ONLY - Cell access is allowed for
 *                                               emergency calls only
 *            - NAS_CELL_ACCESS_NO_CALLS - Cell access is not allowed for any
 *                                         call type
 *            - NAS_CELL_ACCESS_ALL_CALLS - Cell access is allowed for all call
 *                                          types
 *            - NAS_CELL_ACCESS_UNKNOWN - Cell access type is unknown
 */
typedef struct {
    uint32_t csBarStatus;
    uint32_t psBarStatus;
}nas_callBarStatus;

/**
 * This structure contains the Serving System parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  \param  ServingSystem
 *          - Serving System
 *          - See \ref nas_servSystem for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  RoamIndicatorVal
 *          - Optional parameter indicating Roaming Indicator value
 *          - Values:
 *              - 0x00 - Roaming
 *              - 0x01 - Home
 *              - 0x02 - Flashing
 *              - 0x03 and above - Operator defined values
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  DataSrvCapabilities
 *          - Optional parameter indicating Data services capability
 *          - See \ref nas_dataSrvCapabilities for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  CurrentPLMN
 *          - Optional parameter indicating Current PLMN
 *          -  See \ref nas_currentPLMN for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  SystemID
 *          - Optional parameter indicating System ID
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  NetworkID
 *          - Optional parameter indicating Network ID
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  BaseStationID
 *          - Optional parameter indicating Base Station Identification Number
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  BaseStationLatitude
 *          - Optional parameter indicating Base station latitude in units of
 *            0.25 sec,expressed as a two's complement signed number with
 *            positive numbers signifying North latitude
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  BasestationLongitude
 *          - Optional parameter indicating Base station longitude in units of
 *            0.25 sec, expressed as a Two's complement signed number with
 *            positive numbers signifying East longitude
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  RoamingIndicatorList
 *          - Optional parameter indicating Roaming Indicator List
 *          - See \ref nas_roamIndList for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  DefaultRoamInd
 *          - Optional parameter indicating Default Roaming Indicator
 *          - Values:
 *              - 0x00 - Roaming
 *              - 0x01 - Home
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  Gpp2TimeZone
 *          - Optional parameter indicating 3GPP2 Time Zone
 *          - See \ref nas_qaQmi3Gpp2TimeZone for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  CDMA_P_Rev
 *          - Optional parameter indicating CDMA P_Rev in use
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  GppTimeZone
 *          - Optional parameter indicating Offset from Universal time, i.e.,
 *            difference between local time and Universal time, in increments
 *            of 15 min. (signed value).
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  \param  GppNetworkDSTAdjustment
 *          - Optional parameter indicating 3GPP network daylight
 *            saving adjustment
 *          - Values:
 *              - 0x00 - No adjustment for Daylight Saving Time
 *              - 0x01 - 1 hr adjustment for Daylight Saving Time
 *              - 0x02 - 2 hr adjustment for Daylight Saving Time
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  \param  Lac
 *          - Optional parameter indicating 3GPP Location Area Code
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  \param  CellID
 *          - Optional parameter indicating 3GPP Cell ID
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  \param  ConcSvcInfo
 *          - Optional parameter indicating 3GPP2 concurrent service Info
 *          - Values:
 *              - 0x00 - Concurrent service not available
 *              - 0x01 - Concurrent service available
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  \param  PRLInd
 *          - Optional parameter indicating 3GPP2 PRL Indicator
 *          - Values:
 *              - 0x00 - System not in PRL
 *              - 0x01 - System is in PRL
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  \param  DTMInd
 *          - Optional parameter indicating Dual Transfer Mode
 *            Indication(GSM Only)
 *          - Values:
 *              - 0x00 - DTM not supported
 *              - 0x01 - DTM supported
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  \param  DetailedSvcInfo
 *          - Optional parameter indicating Detailed service information
 *          - See \ref nas_detailSvcInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  \param  CDMASystemInfoExt
 *          - Optional parameter indicating CDMA System Info Ext
 *          - See \ref nas_CDMASysInfoExt for more information
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  \param  HdrPersonality
 *          - Optional parameter indicating HDR Personality Information
 *          - Values:
 *              - 0x00 - Unknown
 *              - 0x01 - HRPD
 *              - 0x02 - eHRPD
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  \param  TrackAreaCode
 *          - Optional parameter indicating Tracking area code information
 *            for LTE
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  \param  CallBarStatus
 *          - Optional parameter indicating Call Barring Status
 *          - See \ref nas_callBarStatus for more information
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_servSystem              ServingSystem;
    uint8_t                     RoamIndicatorVal;
    nas_dataSrvCapabilities     DataSrvCapabilities;
    nas_currentPLMN             CurrentPLMN;
    uint16_t                    SystemID;
    uint16_t                    NetworkID;
    uint16_t                    BasestationID;
    uint32_t                    BasestationLatitude;
    uint32_t                    BasestationLongitude;
    nas_roamIndList             RoamingIndicatorList;
    uint8_t                     DefaultRoamInd;
    nas_qaQmi3Gpp2TimeZone      Gpp2TimeZone;
    uint8_t                     CDMA_P_Rev;
    uint8_t                     GppTimeZone;
    uint8_t                     GppNetworkDSTAdjustment;
    uint16_t                    Lac;
    uint32_t                    CellID;
    uint8_t                     ConcSvcInfo;
    uint8_t                     PRLInd;
    uint8_t                     DTMInd;
    nas_detailSvcInfo           DetailedSvcInfo;
    nas_CDMASysInfoExt          CDMASystemInfoExt;
    uint8_t                     HdrPersonality;
    uint16_t                    TrackAreaCode;
    nas_callBarStatus           CallBarStatus;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSGetServingSystem_t;

/**
 * Pack queries information regarding the system that currently provides service.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetServingSystem(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Unack queries information regarding the system that currently provides service.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetServingSystem(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetServingSystem_t *pOutput
        );

/**
 * This structure contains MNC PCS Digit Include Status.
 *
 * @param   MCC
 *          - A 16-bit integer representation of MCC.
 *          - Range - 0 to 999.
 *
 * @param   MNC
 *          - A 16-bit integer representation of MNC.
 *          - Range - 0 to 999.
 *
 * @param   MNCIncPCSDigit
 *          - This field is used to interpret the length of the
 *            corresponding MNC reported in the TLVs with an MNC.
 *          - Values
 *            - TRUE - MNC is a three-digit value.
 *                     e.g., a reported value of 90 corresponds to an MNC value of 090
 *            - FALSE - MNC is a two-digit value.
 *                      e.g., a reported value of 90 corresponds to an MNC value of 90
 *
 * @param   TLVPresent
 *          - TLV Present.
 */
typedef struct {
    uint16_t   MCC;
    uint16_t   MNC;
    uint8_t    MNCIncPCSDigit;
    uint8_t    TLVPresent;
}nas_MNCPCSDigitStatus;

/**
 * This structure contains the Serving System parameters
 *  - Parameter values default to their data type's maximum unsigned value
 *    unless explicitly stated otherwise.
 *
 *  \param  ServingSystem
 *          - Serving System
 *          - See \ref nas_servSystem for more information
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  RoamIndicatorVal
 *          - Optional parameter indicating Roaming Indicator value
 *          - Values:
 *              - 0x00 - Roaming
 *              - 0x01 - Home
 *              - 0x02 - Flashing
 *              - 0x03 and above - Operator defined values
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  DataSrvCapabilities
 *          - Optional parameter indicating Data services capability
 *          - See \ref nas_dataSrvCapabilities for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  CurrentPLMN
 *          - Optional parameter indicating Current PLMN
 *          -  See \ref nas_currentPLMN for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  SystemID
 *          - Optional parameter indicating System ID
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  NetworkID
 *          - Optional parameter indicating Network ID
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  BaseStationID
 *          - Optional parameter indicating Base Station Identification Number
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  BaseStationLatitude
 *          - Optional parameter indicating Base station latitude in units of
 *            0.25 sec,expressed as a two's complement signed number with
 *            positive numbers signifying North latitude
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  BasestationLongitude
 *          - Optional parameter indicating Base station longitude in units of
 *            0.25 sec, expressed as a Two's complement signed number with
 *            positive numbers signifying East longitude
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  RoamingIndicatorList
 *          - Optional parameter indicating Roaming Indicator List
 *          - See \ref nas_roamIndList for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  DefaultRoamInd
 *          - Optional parameter indicating Default Roaming Indicator
 *          - Values:
 *              - 0x00 - Roaming
 *              - 0x01 - Home
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  Gpp2TimeZone
 *          - Optional parameter indicating 3GPP2 Time Zone
 *          - See \ref nas_qaQmi3Gpp2TimeZone for more information
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  CDMA_P_Rev
 *          - Optional parameter indicating CDMA P_Rev in use
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  GppTimeZone
 *          - Optional parameter indicating Offset from Universal time, i.e.,
 *            difference between local time and Universal time, in increments
 *            of 15 min. (signed value).
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  \param  GppNetworkDSTAdjustment
 *          - Optional parameter indicating 3GPP network daylight
 *            saving adjustment
 *          - Values:
 *              - 0x00 - No adjustment for Daylight Saving Time
 *              - 0x01 - 1 hr adjustment for Daylight Saving Time
 *              - 0x02 - 2 hr adjustment for Daylight Saving Time
 *          - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *  \param  Lac
 *          - Optional parameter indicating 3GPP Location Area Code
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  \param  CellID
 *          - Optional parameter indicating 3GPP Cell ID
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  \param  ConcSvcInfo
 *          - Optional parameter indicating 3GPP2 concurrent service Info
 *          - Values:
 *              - 0x00 - Concurrent service not available
 *              - 0x01 - Concurrent service available
 *          - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *  \param  PRLInd
 *          - Optional parameter indicating 3GPP2 PRL Indicator
 *          - Values:
 *              - 0x00 - System not in PRL
 *              - 0x01 - System is in PRL
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  \param  DTMInd
 *          - Optional parameter indicating Dual Transfer Mode
 *            Indication(GSM Only)
 *          - Values:
 *              - 0x00 - DTM not supported
 *              - 0x01 - DTM supported
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  \param  DetailedSvcInfo
 *          - Optional parameter indicating Detailed service information
 *          - See \ref nas_detailSvcInfo for more information
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  \param  CDMASystemInfoExt
 *          - Optional parameter indicating CDMA System Info Ext
 *          - See \ref nas_CDMASysInfoExt for more information
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  \param  HdrPersonality
 *          - Optional parameter indicating HDR Personality Information
 *          - Values:
 *              - 0x00 - Unknown
 *              - 0x01 - HRPD
 *              - 0x02 - eHRPD
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  \param  TrackAreaCode
 *          - Optional parameter indicating Tracking area code information
 *            for LTE
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  \param  CallBarStatus
 *          - Optional parameter indicating Call Barring Status
 *          - See \ref nas_callBarStatus for more information
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  \param  MNCPCSDigitTlv
 *          - Optional parameter indicating MNC PCS Digit Include Status
 *          - See \ref nas_MNCPCSDigitStatus for more information
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_servSystem              ServingSystem;
    uint8_t                     RoamIndicatorVal;
    nas_dataSrvCapabilities     DataSrvCapabilities;
    nas_currentPLMN             CurrentPLMN;
    uint16_t                    SystemID;
    uint16_t                    NetworkID;
    uint16_t                    BasestationID;
    uint32_t                    BasestationLatitude;
    uint32_t                    BasestationLongitude;
    nas_roamIndList             RoamingIndicatorList;
    uint8_t                     DefaultRoamInd;
    nas_qaQmi3Gpp2TimeZone      Gpp2TimeZone;
    uint8_t                     CDMA_P_Rev;
    uint8_t                     GppTimeZone;
    uint8_t                     GppNetworkDSTAdjustment;
    uint16_t                    Lac;
    uint32_t                    CellID;
    uint8_t                     ConcSvcInfo;
    uint8_t                     PRLInd;
    uint8_t                     DTMInd;
    nas_detailSvcInfo           DetailedSvcInfo;
    nas_CDMASysInfoExt          CDMASystemInfoExt;
    uint8_t                     HdrPersonality;
    uint16_t                    TrackAreaCode;
    nas_callBarStatus           CallBarStatus;
    nas_MNCPCSDigitStatus       MNCPCSDigitStatTlv;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSGetServingSystemV2_t;

/**
 * Unack queries information regarding the system that currently provides service.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetServingSystemV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetServingSystemV2_t *pOutput
        );

/**
 * Pack queries information regarding the system that currently provides service.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetServingSystemV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * valid queries information regarding the system that currently provides service
 * @param[in]   pResp       qmi response from modem
 * @param[in]   u8Info      value to check unpack_nas_SLQSGetServingSystem_t param valid.
 *                   - 0 -  RoamIndicatorVal
 *                   - 1 -  DataSrvCapabilities
 *                   - 2 -  CurrentPLMN
 *                   - 3 -  SystemID and NetworkID
 *                   - 4 -  BaseStationID, BaseStationLatitude and BasestationLongitude
 *                   - 5 -  RoamingIndicatorList
 *                   - 6 -  DefaultRoamInd
 *                   - 7 -  Gpp2TimeZone
 *                   - 8 -  CDMA_P_Rev
 *                   - 9 -  GppTimeZone
 *                   - 10 -  GppNetworkDSTAdjustment
 *                   - 11 -  Lac
 *                   - 12 -  CellID
 *                   - 13 -  ConcSvcInfo
 *                   - 14 -  PRLInd
 *                   - 15 -  DTMInd
 *                   - 16 -  DetailedSvcInfo
 *                   - 17 -  CDMASystemInfoExt
 *                   - 18 -  HdrPersonality
 *                   - 19 -  TrackAreaCode
 *                   - 20 -  CallBarStatus
 *
 * @return eQCWWAN_ERR_NONE on signal info valid, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_valid_nas_SLQSGetServingSystem(uint8_t *pResp,
        uint8_t u8Info);

/**
 * This structure contains the Received Signal Strength Information
 *
 *  @param  rxSignalStrength
 *          - Received signal strength in dBm
 *              - For CDMA and UMTS, this indicates forward link pilotEc.
 *              - For GSM, the received signal strength.
 *              - For LTE, this indicates the total received wideband power
 *                observed by UE.
 *
 *  @param  radioIf
 *          - Radio interface technology of the signal being radio_if measured
 *              - 0x00 - RADIO_IF_NO_SVC - None (no service)
 *              - 0x01 - RADIO_IF_CDMA_1X - cdma2000&reg; 1X
 *              - 0x02 - RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - RADIO_IF_AMPS - AMPS
 *              - 0x04 - RADIO_IF_GSM - GSM
 *              - 0x05 - RADIO_IF_UMTS - UMTS
 *              - 0x08 - RADIO_IF_LTE - LTE
 *
 *  \note   First elemnet of the RSSI list always contains the current Signal
 *          strength and Radio Interface.
 */
typedef struct 
{
    int16_t rxSignalStrength;
    uint8_t  radioIf;
} nas_rxSignalStrengthListElement;

/**
 * This structure contains the ECIO Information
 *
 *  @param  ecio
 *          - ECIO value in dBm
 *
 *  @param  radioIf
 *          - Radio interface technology of the signal being measured
 *              - 0x00 - RADIO_IF_NO_SVC - None (no service)
 *              - 0x01 - RADIO_IF_CDMA_1X - cdma2000&reg; 1X
 *              - 0x02 - RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - RADIO_IF_AMPS - AMPS
 *              - 0x04 - RADIO_IF_GSM - GSM
 *              - 0x05 - RADIO_IF_UMTS - UMTS
 */
typedef struct 
{
    int16_t ecio;
    uint8_t  radioIf;
}nas_ecioListElement;

/**
 * This structure contains the Error Rate Information
 *
 *  @param  errorRate
 *          - Error rate value corresponds to the RAT that is currently
 *            registered.
 *              - For CDMA, the error rate reported is Frame Error Rate:
 *                  - Valid error rate values between 1 and 10000 are returned
 *                    to indicate percentage, e.g., a value of 300 means the
 *                    error rate is 3%
 *                  - A value of 0xFFFF indicates that the error rate is
 *                    unknown or unavailable
 *              - For HDR, the error rate reported is Packet Error Rate:
 *                  - Valid error rate values between 1 and 10000 are returned
 *                    to indicate percentage, e.g., a value of 300 means the
 *                    error rate is 3%
 *                  - A value of 0xFFFF indicates that the error rate is
 *                    unknown or unavailable
 *              - For GSM, the error rate reported is Bit Error Rate:
 *                  - Valid values are 0, 100, 200, 300, 400, 500, 600, and 700
 *                    The reported value divided by 100 gives the error rate as
 *                    an RxQual value, e.g.,a value of 300 represents an RxQual
 *                    value of 3.
 *                  - A value of 25500 indicates No Data
 *              - For WCDMA, the error rate reported is Block Error Rate (BLER):
 *                  - Valid values are 1 to 10000
 *                  - The reported value divided by 100 provides the error rate
 *                    in percentages, e.g., a value of 300 represents a BLER of
 *                    3%.
 *                  - A value of 0 indicates No Data
 *
 *  @param  radioIf
 *          - Radio interface technology of the signal being measured
 *              - 0x00 - RADIO_IF_NO_SVC - None (no service)
 *              - 0x01 - RADIO_IF_CDMA_1X - cdma2000&reg; 1X
 *              - 0x02 - RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *              - 0x03 - RADIO_IF_AMPS - AMPS
 *              - 0x04 - RADIO_IF_GSM - GSM
 *              - 0x05 - RADIO_IF_UMTS - UMTS
 */
typedef struct 
{
    uint16_t errorRate;
    uint8_t   radioIf;
} nas_errorRateListElement;

/**
 * This structure contains the RSRQ Information
 *
 *  @param  rsrq
 *          - RSRQ value in dB (signed integer value); valid range is -3
 *            to -20 (-3 means -3 dB, -20 means -20 dB)
 *
 *  @param  radioIf
 *          - Radio interface technology of the signal being measured
 *              - 0x08 - LTE
 */
typedef struct 
{
    int8_t rsrq;
    uint8_t radioIf;
} nas_rsrqInformation;

/**
 * This structure contains the LTE SNR Information
 *
 *  @param  snrlevel
 *          - LTE SNR level as a scaled integer in units of 0.1dB
 *             e.g. -16dB has a value of -160 and 24.6dB has value of 246.
 */
typedef struct
{
    int16_t snrlevel;
} nas_lteSnrinformation;

/**
 * This structure contains the LTE RSRP Information
 *
 *  @param  rsrplevel
 *          - LTE RSRP in dBm as a mesaured by L1.
 *            Range: -44 to -140(-44 means -44dBm, -140 means -140dBm).
 */
typedef struct
{
    int16_t rsrplevel;
} nas_lteRsrpinformation;


/**
 * This structure contains the Signal Strength Information
 *
 *  \param  signalStrengthReqMask
 *          - Request Mask\n
 *              - Request additional signal information for:
 *                Bit 0 - RSSI Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for RSSI\n
 *                        1 - Request Additional Info for RSSI\n
 *                Bit 1 - ECIO Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for ECIO\n
 *                        1 - Request Additional Info for ECIO\n
 *                Bit 2 - IO Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for IO\n
 *                        1 - Request Additional Info for IO\n
 *                Bit 3 - SINR Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for SINR\n
 *                        1 - Request Additional Info for SINR\n
 *                Bit 4 - ERROR RATE Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for Error Rate\n
 *                        1 - Request Additional Info for Error Rate\n
 *                Bit 5 - RSRQ Information bit\n
 *                        Valid values are:\n
 *                        0 - Do Not Request Additional Info for RSRQ\n
 *                        1 - Request Additional Info for RSRQ\n
 *                Bit 6 - LTE SNR information bit\n
 *                        Valid values are:\n
 *                        0 - Do not request additional information for LTE SNR\n
 *                        1 - Request additional information for LTE SNR
 *                Bit 7 - LTE RSRP Information bit\n
 *                        Valid values are:\n
 *                        0 - Do not request additional information for LTE RSRP\n
 *                        1 - Request additional information for LTE RSRP\n
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  rxSignalStrengthListLen
 *          - Number of elements in Receive Signal Strength List
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  rxSignalStrengthList
 *          - See \ref nas_rxSignalStrengthListElement for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  ecioListLen
 *          - Number of elements in ECIO List
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  ecioList
 *          - See \ref nas_ecioListElement for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  Io
 *          - Received Io in dBm; IO is only applicable for 1xEV-DO
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  sinr
 *          - SINR level
 *              - SINR is only applicable for 1xEV-DO; valid levels are 0 to 8
 *                where maximum value for
 *                0 - SINR_LEVEL_0 is -9 dB
 *                1 - SINR_LEVEL_1 is -6 dB
 *                2 - SINR_LEVEL_2 is -4.5 dB
 *                3 - SINR_LEVEL_3 is -3 dB
 *                4 - SINR_LEVEL_4 is -2 dB
 *                5 - SINR_LEVEL_5 is +1 dB
 *                6 - SINR_LEVEL_6 is +3 dB
 *                7 - SINR_LEVEL_7 is +6 dB
 *                8 - SINR_LEVEL_8 is +9 dB
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  errorRateListLen
 *          - Number of elements in Error Rate List
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  errorRateList
 *          - See \ref nas_errorRateListElement for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  rsrqInfo
 *          - See \ref nas_rsrqInformation for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  ltesnr
 *          - LTE SNR level as a scaled integer in units of 0.1 dB;
 *            e.g., -16 dB has a value of -160 and 24.6 dB has a
 *            value of 246. LTE SNR is included only when the current
 *            serving system is LTE
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  \param  ltersrp
 *          - LTE SNR level as a scaled integer in units of 0.1 dB;
 *            e.g., -16 dB has a value of -160 and 24.6 dB has a
 *            value of 246. LTE SNR is included only when the current
 *            serving system is LTE
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint16_t   signalStrengthReqMask; //don't use this value
    uint16_t   rxSignalStrengthListLen;
    nas_rxSignalStrengthListElement rxSignalStrengthList[18];
    uint16_t   ecioListLen;
    nas_ecioListElement ecioList[18];
    int32_t    Io;
    uint8_t    sinr;
    uint16_t   errorRateListLen;
    nas_errorRateListElement errorRateList[18];
    nas_rsrqInformation rsrqInfo;
    int16_t    ltesnr;
    int16_t    ltersrp;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSGetSignalStrength_t;

/**
 * Queries the current signal strength as measured by the device. This API is
 * deprecated on MC73xx/EM73xx modules since firmware version SWI9X15C_05_xx_xx_xx
 * and all EM74xx firmware versions. Please use pack_nas_SLQSNasGetSigInfo() for new
 * firmware versions and new modules
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqMask request mask for fetching extra signal info
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSignalStrength(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        uint16_t reqMask
        );

/**
 * Unpack get signal strength.This API is deprecated on MC73xx/EM73xx modules
 * since firmware version SWI9X15C_05_xx_xx_xx and all EM74xx firmware versions.
 * Please use unpack_nas_SLQSNasGetSigInfo() for new firmware versions and new modules
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSignalStrength(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSignalStrength_t *pOutput
        );

/**
 * valid queries information regarding the system that currently provides service
 * @param[in]   pResp       qmi response from modem
 * @param[in]   u8Info      value to check unpack_nas_SLQSGetSignalStrength_t param valid.
 *                   - 0 -  rxSignalStrengthList and rxSignalStrengthListLen
 *                   - 1 -  ecioList and ecioListLen
 *                   - 2 -  Io
 *                   - 3 -  sinr
 *                   - 4 -  errorRateListLen, errorRateList
 *                   - 5 -  rsrqInfo
 *                   - 6 -  ltesnr
 *                   - 7 -  ltersrp
 *
 * @return eQCWWAN_ERR_NONE on signal info valid, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_valid_nas_SLQSGetSignalStrength(uint8_t *pResp,
        uint8_t u8Info);

/**
 *  Structure for storing the input parameters passed for
 *  SLQSSetSignalStrengthsCallback by the user.
 *
 *  \param  rxSignalStrengthDelta
 *          - RSSI delta(in dBm) at which an event report indication,
 *            including the current RSSI, will be sent to the requesting
 *            control point.
 *
 *  \param  ecioDelta
 *          - ECIO delta at which an event report indication, ecioDelta
 *            including the current ECIO, will be sent to the requesting
 *            control point.
 *          - ECIO delta is an unsigned 1 byte value that increments in
 *            negative 0.5 dBm, e.g., ecio_delta of 2 means a change of -1 dBm.
 *
 *  \param  ioDelta
 *          - IO delta (in dBm) at which an event report indication,
 *            ioDelta including the current IO, will be sent to the
 *            requesting control point.
 *
 *
 *  \param  sinrDelta
 *          - SINR delta level at which an event report indication, sinrDelta
 *            including the current SINR, will be sent to the requesting control
 *            point.
 *
 *  \param  rsrqDelta
 *          - RSRQ delta level at which an event report indication,
 *            including the current RSRQ, will be sent to the requesting
 *            control point.
 *
 *  \param  ecioThresholdListLen
 *          - Number of elements in the ECIO threshold list.
 *
 *  \param  ecioThresholdList
 *          - A sequence of thresholds delimiting EcIo event reporting
 *            bands. Every time a new EcIo value crosses a threshold value,
 *            an event report indication message with the new ECIO
 *            value is sent to the requesting control point. For this field:
 *            - Maximum number of threshold values is 10
 *            - At least one value must be specified.
 *
 *  \param  sinrThresholdListLen
 *          - Number of elements in the SINR threshold list.
 *
 *  \param  sinrThresholdList
 *          - A sequence of thresholds delimiting SINR event reporting bands.
 *            Every time a new SINR value crosses a threshold value, an event
 *            report indication message with the new sinr value is sent to the
 *            requesting control point. For this field:
 *            - Maximum number of threshold values is 5
 *            - At least one value must be specified.
 *
 *  \param  ltesnrdelta
 *          - LTE SNR delta level at which an event report
 *             indication, including the current SNR, will be sent to
 *             the requesting control point. LTE SNR delta level is
 *             an unsigned 2 byte value, representing the delta in
 *             units of 0.1 dB, e.g., lte_snr_delta of 3 means a
 *             change 0.3dB.
 *
 *  \param  ltersrpdelta
 *          -LTE RSRP delta level at which an event report
 *          -indication, including the current RSRP, will be sent
 *          -to the requesting control point. LTE RSRP delta
 *          -level is an unsigned 1 byte value, representing the
 *          -delta in dB.
 *
 */
typedef struct
{
    uint8_t  rxSignalStrengthDelta;
    uint8_t  ecioDelta;
    uint8_t  ioDelta;
    uint8_t  sinrDelta;
    uint8_t  rsrqDelta;
    uint8_t  ecioThresholdListLen;
    int16_t  ecioThresholdList[10];
    uint8_t  sinrThresholdListLen;
    uint8_t  sinrThresholdList[5];
    uint16_t lteSnrDelta;
    uint8_t  lteRsrpDelta;
} nas_SLQSSignalStrengthsIndReq;

/**
 *  This structure contains paramaters pack set strength thresholds callback.
 *  @param  bEnable 0/1 to disable/enable RSSI signal stregth indication
 *  @param  pSigIndReq parameters to control signal strength indication
 */
typedef struct
{
    uint8_t bEnable;
    nas_SLQSSignalStrengthsIndReq *pSigIndReq;
} pack_nas_SLQSSetSignalStrengthsCallback_t;

typedef unpack_result_t  unpack_nas_SLQSSetSignalStrengthsCallback_t;

/**
 * Pack set signal strength thresholds callback.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSetSignalStrengthsCallback(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSSetSignalStrengthsCallback_t *pReqParam
        );

/**
 * Unpack set signal strength thresholds callback.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSetSignalStrengthsCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSetSignalStrengthsCallback_t *pOutput
        );

typedef unpack_result_t  unpack_nas_SetRFInfoCallback_t;

/**
 * Pack Set RF Band Information callback.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pBenable 0/1 value to disable/enable indication respectively
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SetRFInfoCallback(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        uint8_t *pBenable
        );

/**
 * Unack Set RF Band Information callback.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetRFInfoCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetRFInfoCallback_t *pOutput
        );

typedef unpack_result_t  unpack_nas_SetLURejectCallback_t;

/**
 * Pack set registration reject Callback
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pBenable 0/1 value to disable/enable indication respectively
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SetLURejectCallback(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        uint8_t *pBenable
        );

/**
 * Unpack set registration reject Callback
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetLURejectCallback(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetLURejectCallback_t *pOutput
        );

/**
 *  This structure contains paramaters signal strengths information.
 *
 *  @param  rxSignalStrengthInfo
 *          - signal strength info list
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ecioInfo
 *          - ecio info list
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  io
 *          - received IO in dBm; IO is only applicable for 1xEV-DO
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  sinr
 *          - SINR level
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  errorRateInfo
 *          - error rate info
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  rsrqInfo
 *          - rsrq info
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  lteSnrinfo
 *          - lte Snr information
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  lteRsrpinfo
 *          - lte rsrp info
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 */
typedef struct{
    nas_rxSignalStrengthListElement rxSignalStrengthInfo;
    nas_ecioListElement             ecioInfo;
    uint32_t                   io;
    uint8_t                         sinr;
    nas_errorRateListElement        errorRateInfo;
    nas_rsrqInformation             rsrqInfo;
    nas_lteSnrinformation           lteSnrinfo;
    nas_lteRsrpinformation          lteRsrpinfo;
} nas_SLQSSignalStrengthsInformation;


/**
 *  This structure contains paramaters reject reason.
 *  @param TlvPresent indicating the presence of the TLV in the QMI ind
 *  @param serviceDomain service domain
 *  @param rejectCause cause of reject
 */
typedef struct 
{
    uint8_t TlvPresent;
    uint32_t serviceDomain;
    uint32_t rejectCause;
} nas_RejectReasonTlv;

/**
 *  This structure contains paramaters Signal Strength.
 *  @param TlvPresent indicating the presence of the TLV in the QMI ind
 *  @param signalStrength signal strength
 *  @param radioInterface radio interface technology of the signal being measured
 */
typedef struct 
{
    uint8_t  TlvPresent;
    int8_t  signalStrength;
    uint32_t radioInterface;
} nas_SignalStrengthTlv;

/**
 *  This structure contains paramaters RF Band Information List.
 *  @param TlvPresent indicating the presence of the TLV in the QMI ind
 *  @param radioInterfaceSize Number of sets of the following elements
 *         - radioInterface
 *         - activeBandClass
 *         - activeChannel
 *  @param radioInterface radio interface technology of the signal being measured
 *         - 0x01 - cdma2000 1X 
 *         - 0x02 - cdma2000 HRPD (1xEV-DO)
 *         - 0x03 - AMPS
 *         - 0x04 - GSM
 *         - 0x05 - UMTS
 *         - 0x08 - LTE
 *         - 0x09 - TD-SCDMA
 *  @param activeBandClass active band class
 *         - 00 to 39 - CDMA band classes
 *         - 40 to 79 - GSM band classes
 *         - 80 to 91 - WCDMA band classes
 *         - 120 to 160 - LTE band classes
 *         - 200 to 205 - TD-SCDMA band classes
 *  @param  activeChannel active channel
 *         - Active channel. If the channel is not relevant to the
 *           technology, a value of 0 is returned.
 */
typedef struct 
{
    uint8_t  TlvPresent;
    uint8_t  radioInterfaceSize;
    uint32_t radioInterface[255];
    uint32_t activeBandClass[255];
    uint32_t activeChannel[255];
} nas_RFInfoTlv;

/**
 *  This structure contains paramaters SLQS Signal Strength.
 *  @param TlvPresent indicating the presence of the TLV in the QMI ind
 *  @param sSLQSSignalStrengthsInfo signal strength info
 */
typedef struct 
{
    uint8_t                                  TlvPresent;
    nas_SLQSSignalStrengthsInformation sSLQSSignalStrengthsInfo;
} nas_SLQSSignalStrengthsTlv;

/**
 *  This structure contains unpack set event report indication parameters.
 *
 *  @param  SSTlv
 *          - signal strength tlv
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  RFTlv
 *          - RF tlv
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  RRTlv
 *          - RR tlv
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  SLQSSSTlv
 *          - signal strength complete info tlv
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */
typedef struct {
    nas_SignalStrengthTlv      SSTlv;
    nas_RFInfoTlv              RFTlv;
    nas_RejectReasonTlv        RRTlv;
    nas_SLQSSignalStrengthsTlv SLQSSSTlv;
    swi_uint256_t              ParamPresenceMask;
} unpack_nas_SetEventReportInd_t;

/**
 * Unpack set event report indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]   pOutput    response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_nas_SetEventReportInd(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetEventReportInd_t *pOutput
        );

/**
 *  Gets the current CDMA network parameters
 *
 *  \param  SCI
 *          - Slot cycle index\n
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  SCM
 *          - Station class mark\n
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  RegHomeSID
 *          - Register on home SID\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  RegForeignSID
 *          - Register on foreign SID\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  RegForeignNID
 *          - Register on foreign NID\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  ForceRev0
 *          - Force CDMA 1x-EV-DO Rev. 0 mode\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  CustomSCP
 *          - Use a custom config for CDMA 1x-EV-DO SCP\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  Protocol
 *          - Protocol mask for custom SCP config\n
 *            - 0x00000001 - Subtype 2 Physical Layer
 *            - 0x00000002 - Enhanced CCMAC
 *            - 0x00000004 - Enhanced ACMAC
 *            - 0x00000008 - Enhanced FTCMAC
 *            - 0x00000010 - Subtype 3 RTCMAC
 *            - 0x00000020 - Subsystem 1 RTCMAC
 *            - 0x00000040 - Enhanced Idle
 *            - 0x00000080 - Generic Multimode Capable Disc Port
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  Broadcast
 *          - Broadcast mask for custom SCP config
 *              - 0x00000001 - Generic broadcast enabled
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  Application
 *          - Application mask for custom SCP config\n
 *            - 0x00000001 - SN Multiflow Packet Application
 *            - 0x00000002 - SN Enhanced Multiflow Packet Application
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  \param  Roaming
 *          - Roaming preference\n
 *            - 0 - Automatic
 *            - 1 - Home Only
 *            - 2 - Affiliated Roaming Only
 *            - 3 - Home and Affiliated Roaming
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t  SCI;
    uint8_t  SCM;
    uint8_t  RegHomeSID;
    uint8_t  RegForeignSID;
    uint8_t  RegForeignNID;
    uint8_t  ForceRev0;
    uint8_t  CustomSCP;
    uint32_t Protocol;
    uint32_t Broadcast;
    uint32_t Application;
    uint32_t Roaming;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetCDMANetworkParameters_t;

/**
 * Get CDMA Network Parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetCDMANetworkParameters(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get CDMA Network Parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     qmi output parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetCDMANetworkParameters(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetCDMANetworkParameters_t *pOutput
        );

/**
 * valid CDMA Network Parameters
 * @param[in]   pResp       qmi response from modem
 * @param[in]   u8Info      value to check unpack_nas_GetCDMANetworkParameters_t param valid.
 *                   - 0 -  SCI
 *                   - 1 -  SCM
 *                   - 2 -  RegHomeSID, RegForeignSID and RegForeignNID
 *                   - 3 -  ForceRev0
 *                   - 4 -  CustomSCP, Protocol ,Broadcast and Application
 *                   - 5 -  Roaming
 *
 * @return eQCWWAN_ERR_NONE on signal info valid, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
 int unpack_valid_nas_GetCDMANetworkParameters(uint8_t *pResp,
        uint8_t u8Info);


/**
 * AN-AAA  authentication status of the device pack.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetANAAAAuthenticationStatus(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  AN-AAA  authentication status of the device
 *
 *  @param  AN-AAA Authentication Status.
 *          - Status of the last AN-AAA authentication request, if
 *            any, for the current 1xEV-DO session.
 *            - 0 - AAA_STATUS_FAILED - Authentication failed
 *            - 1 - AAA_STATUS_SUCCESS - Authentication success
 *            - 2 - AAA_STATUS_NO_REQUEST - No authentication requested
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint32_t *pAuthStatus;
    swi_uint256_t  ParamPresenceMask;
}unpack_nas_GetANAAAAuthenticationStatus_t;

/**
 * AN-AAA  authentication status of the device unpack.
 *
 * @param[in]   pResp          qmi response from modem
 * @param[in]   respLen        qmi response length
 * @param[out]  pOutput        response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetANAAAAuthenticationStatus(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetANAAAAuthenticationStatus_t *pOutput
        );

/**
 *  Retrieves information about the access overload class unpack structure
 *
 *  @param  pAccolc
 *          - An 8-bit integer representation of the ACCOLC.
 *            - Range: 0 to 15 (0x00 to 0x0F).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    uint8_t *pAccolc;
    swi_uint256_t  ParamPresenceMask;
}unpack_nas_GetACCOLC_t;

/**
 *  Retrieves information about the access overload class (ACCOLC) pack
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetACCOLC(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Retrieves information about the access overload class (ACCOLC) unpack
 *
 * @param[in]   pResp          qmi response from modem
 * @param[in]   respLen        qmi response length
 * @param[out]  pOutput        response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetACCOLC(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetACCOLC_t *pOutput
        );

/**
 *  Sets the access overload class (ACCOLC)
 *
 *  \param  spc
 *          - service programming code NULL-terminated string of six digit
 *
 *  \param  accolc
 *          - ACCOLC : Valid range is 0 to 15
 */
typedef struct
{
   int8_t spc[6];
   uint8_t accolc;
}  pack_nas_SetACCOLC_t;

typedef unpack_result_t  unpack_nas_SetACCOLC_t;

/**
 * Pack Sets the access overload class (ACCOLC)
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  reqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SetACCOLC(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SetACCOLC_t reqParam
        );

/**
 * Unpack Sets the access overload class (ACCOLC)
 * @param[in]   pResp          qmi response from modem
 * @param[in]   respLen        qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetACCOLC(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetACCOLC_t *pOutput
        );

/**
 *  This structure contains CDMA RSSI threshold related parameters.
 *
 *  @param  CDMARSSIThreshListLen
 *          - Length of the CDMARSSI threshold list parameter to follow
 *
 *  @param  pCDMARSSIThreshList
 *          - Array of RSSI thresholds (in units of 0.1 dBm)
 *          - maximum of 32 values.
 *          - Range for RSSI values:-105 to -21 (in dBm).
 *
 */
typedef struct
{
    uint8_t  CDMARSSIThreshListLen;
    int16_t  *pCDMARSSIThreshList;
}nas_CDMARSSIThresh;

/**
 *  This structure contains CDMA ECIO threshold related parameters.
 *
 *  @param  CDMAECIOThreshListLen
 *          - Length of the CDMA ECIO threshold list parameter to follow
 *
 *  @param  pCDMAECIOThreshList
 *          - Array of ECIO thresholds (in units of 0.1 dB)
 *          - Maximum of 32 values.
 *          Range for ECIO values: -31.5 to 0 (in dB)
 *
 */
typedef struct
{
    uint8_t  CDMAECIOThreshListLen;
    int16_t  *pCDMAECIOThreshList;
}nas_CDMAECIOThresh;

/**
 *  This structure contains HDR RSSI threshold related parameters.
 *
 *  @param  HDRRSSIThreshListLen
 *          - Length of the HDR RSSI threshold list parameter to follow
 *
 *  @param  pHDRRSSIThreshList
 *          - Array of RSSI thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values.
 *          - Range for RSSI values: -118 to -13 (in dBm).
 *
 */
typedef struct
{
    uint8_t  HDRRSSIThreshListLen;
    int16_t *pHDRRSSIThreshList;
}nas_HDRRSSIThresh;

/**
 *  This structure contains HDR ECIO threshold related parameters.
 *
 *  @param  HDRECIOThreshListLen
 *          - Length of the HDR ECIO threshold list parameter to follow
 *
 *  @param  pHDRECIOThreshList
 *          - Array of ECIO thresholds (in units of 0.1 dB)
 *          - Maximum of 32 values
 *          - Range for ECIO values: -31.5 to 0 (in dB).
 */
typedef struct
{
    uint8_t  HDRECIOThreshListLen;
    int16_t  *pHDRECIOThreshList;
}nas_HDRECIOThresh;

/**
 *  This structure contains HDR SINR threshold related parameters.
 *
 *  @param  HDRSINRThreshListLen
 *          - Length of the HDR ECIO threshold list parameter to follow
 *
 *  @param  pHDRSINRThreshList
 *          - Array of SINR level thresholds (in units of 1)
 *          - maximum of 32 values.
 *          - Valid levels are 0 to 8
 *            - 0x00 - SINR_LEVEL_0 is -9 dB
 *            - 0x01 - SINR_LEVEL_1 is -6 dB
 *            - 0x02 - SINR_LEVEL_2 is -4.5 dB
 *            - 0x03 - SINR_LEVEL_3 is -3 dB
 *            - 0x04 - SINR_LEVEL_4 is -2 dB
 *            - 0x05 - SINR_LEVEL_5 is +1 dB
 *            - 0x06 - SINR_LEVEL_6 is +3 dB
 *            - 0x07 - SINR_LEVEL_7 is +6 dB
 *            - 0x08 - SINR_LEVEL_8 is +9 dB
*/
typedef struct
{
    uint8_t  HDRSINRThreshListLen;
    uint16_t  *pHDRSINRThreshList;
}nas_HDRSINRThreshold;

/**
 *  This structure contains HDR IO threshold related parameters.
 *
 *  @param  HDRIOThreshListLen
 *          - Length of the HDR IO threshold list parameter to follow
 *
 *  @param  pHDRIOThreshList
 *          - Array of IO thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values
 *          - Range for IO values: -128 to -13 (in dBm).
 */

typedef struct
{
    uint8_t  HDRIOThreshListLen;
    int16_t  *pHDRIOThreshList;
}nas_HDRIOThresh;

/**
 *  This structure contains GSM RSSI threshold related parameters.
 *
 *  @param  GSMRSSIThreshListLen
 *          - Length of the GSM RSSI threshold list parameter to follow
 *
 *  @param  pGSMRSSIThreshList
 *          - Array of RSSI thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values
 *          - Range for RSSI values: -111 to -48 (in dBm)
 */

typedef struct
{
    uint8_t  GSMRSSIThreshListLen;
    int16_t  *pGSMRSSIThreshList;
}nas_GSMRSSIThresh;

/**
 *  This structure contains WCDMA RSSI threshold related parameters.
 *
 *  @param  WCDMARSSIThreshListLen
 *          - Length of the WCDMA RSSI threshold list parameter to follow
 *
 *  @param  pWCDMARSSIThreshList
 *          - Array of RSSI thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values.
 *          - Range for RSSI values: -121 to 0 (in dBm)
 */
typedef struct
{
    uint8_t  WCDMARSSIThreshListLen;
    int16_t  *pWCDMARSSIThreshList;
}nas_WCDMARSSIThresh;

/**
 *  This structure contains WCDMA ECIO threshold related parameters.
 *
 *  @param  WCDMAECIOThreshListLen
 *          - Length of the WCDMA ECIO threshold list parameter to follow
 *
 *  @param  pWCDMAECIOThreshList
 *          - Array of ECIO thresholds (in units of 0.1 dB)
 *          - Maximum of 32 values
 *          - Range for ECIO values: -31.5 to 0 (in dB)
 */
typedef struct
{
    uint8_t  WCDMAECIOThreshListLen;
    int16_t  *pWCDMAECIOThreshList;
}nas_WCDMAECIOThresh;

/**
 *  This structure contains LTE RSSI threshold related parameters.
 *
 *  @param  LTERSSIThreshListLen
 *          - Length of the LTE RSSI threshold list parameter to follow
 *
 *  @param  pLTERSSIThreshList
 *          - Array of RSSI thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values.
 *          - Range for RSSI values: -120 to 0 (in dBm)
 */
typedef struct
{
    uint8_t  LTERSSIThreshListLen;
    int16_t  *pLTERSSIThreshList;
}nas_LTERSSIThresh;

/**
 *  This structure contains LTE SNR threshold related parameters.
 *
 *  @param  LTESNRThreshListLen
 *          - Length of the LTE SNR threshold list parameter to follow
 *
 *  @param  pLTESNRThreshList
 *          - Array of SNR thresholds (in units of 0.1 dB)
 *          - Maximum of 32 values
 *          - Range for SNR values: -20 to 30 (in dB).
 */
typedef struct
{
    uint8_t  LTESNRThreshListLen;
    int16_t  *pLTESNRThreshList;
}nas_LTESNRThreshold;

/**
 *  This structure contains LTE RSRQ threshold related parameters.
 *
 *  @param  LTERSRQThreshListLen
 *          - Length of the LTE RSRQ threshold list parameter to follow
 *
 *  @param  pLTERSRQThreshList
 *          - Array of RSRQ thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values.
 *          - Range for RSRQ values: -20 to -3 (in dBm)
 */
typedef struct
{
    uint8_t  LTERSRQThreshListLen;
    int16_t  *pLTERSRQThreshList;
}nas_LTERSRQThresh;

/**
 *  This structure contains LTE RSRP threshold related parameters.
 *
 *  @param  LTERSRPThreshListLen
 *          - Length of the LTE RSRP threshold list parameter to follow
 *
 *  @param  pLTERSRPThreshList
 *          - Array of RSRP thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values
 *          - Range for RSRP values: -140 to -44 (in dBm).
 */
typedef struct
{
    uint8_t  LTERSRPThreshListLen;
    int16_t  *pLTERSRPThreshList;
}nas_LTERSRPThresh;

/**
 *  This structure contains LTE RSRP threshold related parameters.
 *
 *  @param  rptRate
 *          - Rate on how often the LTE signal must be checked for reporting
 *          Values
 *          - 0 - Report using the default configuration
 *          - 1 - Report every 1 sec
 *          - 2 - Report every 2 sec
 *          - 3 - Report every 3 sec
 *          - 4 - Report every 4 sec
 *          - 5 - Report every 5 sec
 *
 *  @param  avgPeriod
 *          - Averaging period to be used for the LTE signal.
 *          - Values
 *           - 0 - Average using the default configuration
 *           - 1 - Average over 1 sec
 *           - 2 - Average over 2 sec
 *           - 3 - Average over 3 sec
 *           - 4 - Average over 4 sec
 *           - 5 - Average over 5 sec
 *           - 6 - Average over 6 sec
 *           - 7 - Average over 7 sec
 *           - 8 - Average over 8 sec
 *           - 9 - Average over 9 sec
 *           - 10 - Average over 10 sec
 */
typedef struct
{
    uint8_t  rptRate;
    uint8_t  avgPeriod;
}nas_LTESigRptConfig;

/**
 *  This structure contains TDSCDMA RSCP threshold related parameters.
 *
 *  @param  TDSCDMARSCPThreshListLen
 *          - Length of the TDSCDMA RSCP threshold list parameter to follow
 *
 *  @param  pTDSCDMARSCPThreshList
 *          - Array of RSCP thresholds (in units of 0.1 dBm)
 *          - Maximum of 32 values
 *          - Range for RSCP values: -120 to -25 (in dBm).
 */
typedef struct
{
    uint8_t  TDSCDMARSCPThreshListLen;
    int16_t  *pTDSCDMARSCPThreshList;
}nas_TDSCDMARSCPThresh;

/**
 *  This structure contains TDSCDMA RSSI threshold related parameters.
 *
 *  @param  TDSCDMARSSIThreshListLen
 *          - Length of the TDSCDMA RSSI threshold list parameter to follow
 *
 *  @param  pTDSCDMARSSIThreshList
 *          - Array of RSSI thresholds (in dBm) used by TD-SCDMA
 *          - Maximum of 32 values.
 */
typedef struct
{
    uint8_t   TDSCDMARSSIThreshListLen;
    float  *pTDSCDMARSSIThreshList;
}nas_TDSCDMARSSIThresh;

/**
 *  This structure contains TDSCDMA ECIO threshold related parameters.
 *
 *  @param  TDSCDMAECIOThreshListLen
 *          - Length of the TDSCDMA ECIO threshold list parameter to follow
 *
 *  @param  pTDSCDMAECIOThreshList
 *          - Array of ECIO thresholds (in dB) used by TD-SCDMA
 *          - Maximum of 32 values.
 */
typedef struct
{
    uint8_t   TDSCDMAECIOThreshListLen;
    float  *pTDSCDMAECIOThreshList;
}nas_TDSCDMAECIOThresh;

/**
 *  This structure contains TDSCDMA SINR threshold related parameters.
 *
 *  @param  TDSCDMASINRThreshListLen
 *          - Length of the TDSCDMA SINR threshold list parameter to follow
 *
 *  @param  pTDSCDMASINRThreshList
 *          - Array of SINR thresholds (in dB) used by TD-SCDMA
 *          - Maximum of 32 values
 */
typedef struct
{
    uint8_t   TDSCDMASINRThreshListLen;
    float  *pTDSCDMASINRThreshList;
}nas_TDSCDMASINRThresh;

/**
 *  This structure contains pack set the signal strength reporting thresholds parameters.
 *  @param  pCDMARSSIThresh
 *          - CDMA RSSI threshold List
 *
 *  @param  pCDMARSSIDelta
 *          - RSSI delta (in units of 0.1 dBm).
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pCDMAECIOThresh
 *          - CDMA ECIO Threshold List
 *
 *  @param  pCDMAECIODelta
 *          - ECIO delta (in units of 0.1 dB).
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pHDRRSSIThresh
 *          - HDR RSSI Threshold List
 *
 *  @param  pHDRRSSIDelta
 *          - RSSI delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pHDRECIOThresh
 *          - HDR ECIO Threshold List
 *
 *  @param  pHDRECIODelta
 *          - ECIO delta (in units of 0.1 dB)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pHDRSINRThresh
 *          - HDR SINR Threshold List
 *
 *  @param  pHDRSINRDelta
 *          - SINR delta (in units of 1 SINR level)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pHDRIOThresh
 *          - HDR IO Threshold List
 *
 *  @param  pHDRIODelta
 *          - IO delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pGSMRSSIThresh
 *          - GSM RSSI Threshold List
 *          - See \ref nas_GSMRSSIThresh for more details
 *
 *  @param  pGSMRSSIDelta
 *          - RSSI delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pWCDMARSSIThresh
 *          - WCDMA RSSI Threshold List
 *          - See \ref nas_WCDMARSSIThresh for more details
 *
 *  @param  pWCDMARSSIDelta
 *          - RSSI delta (in units of 0.1 dBm).
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pWCDMAECIOThresh
 *          - WCDMA ECIO Threshold List
 *
 *  @param  pWCDMAECIODelta
 *          - ECIO delta (in units of 0.1 dB)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pLTERSSIThresh
 *          - LTE RSSI Threshold List
 *
 *  @param  pLTERSSIDelta
 *          - RSSI delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pLTESNRThresh
 *          - LTE SNR Threshold List
 *
 *  @param  pLTESNRDelta
 *          - SNR delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pLTERSRQThresh
 *          - LTE RSRQ Threshold List
 *
 *  @param  pLTERSRQDelta
 *          - RSRQ delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pLTERSRPThresh
 *          - LTE RSRP Threshold List
 *
 *  @param  pLTERSRPDelta
 *          - RSRP delta (in units of 0.1 dBm).
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pLTESigRptConfig
 *          - LTE Signal Report Config
 *
 *  @param  pTDSCDMARSCPThresh
 *          - TDSCDMA RSCP Threshold List
 *
 *  @param  pTDSCDMARSCPDelta
 *          - RSCP delta (in units of 0.1 dBm)
 *          - A value of 0 is rejected with a QMI_ERR_INVALID_ARG error.
 *
 *  @param  pTDSCDMARSSIThresh
 *          - TDSCDMA RSSI Threshold List
 *
 *  @param  pTDSCDMARSSIDelta
 *          - RSSI delta (in dBm) used by TD-SCDMA.
 *
 *  @param  pTDSCDMAECIOThresh
 *          - TDSCDMA ECIO Threshold List
 *
 *  @param  pTDSCDMAECIODelta
 *          - ECIO delta (in dB) used by TD-SCDMA
 *
 *  @param  pTDSCDMASINRThresh
 *          - TDSCDMA SINR Threshold List
 *
 *  @param  pTDSCDMASINRDelta
 *          - SINR delta (in dB) used by TD-SCDMA.
 *
 *  @note  Mixture of threshold and delta values can be provided in the request.
 *         But for each type and RAT,only one of threshold list or delta value is
 *         to be provided.
 *
 */
typedef struct
{
    nas_CDMARSSIThresh    *pCDMARSSIThresh;
    uint16_t              *pCDMARSSIDelta;
    nas_CDMAECIOThresh    *pCDMAECIOThresh;
    uint16_t              *pCDMAECIODelta;
    nas_HDRRSSIThresh     *pHDRRSSIThresh;
    uint16_t              *pHDRRSSIDelta;
    nas_HDRECIOThresh     *pHDRECIOThresh;
    uint16_t              *pHDRECIODelta;
    nas_HDRSINRThreshold  *pHDRSINRThresh;
    uint16_t              *pHDRSINRDelta;
    nas_HDRIOThresh       *pHDRIOThresh;
    uint16_t              *pHDRIODelta;
    nas_GSMRSSIThresh     *pGSMRSSIThresh;
    uint16_t              *pGSMRSSIDelta;
    nas_WCDMARSSIThresh   *pWCDMARSSIThresh;
    uint16_t              *pWCDMARSSIDelta;
    nas_WCDMAECIOThresh   *pWCDMAECIOThresh;
    uint16_t              *pWCDMAECIODelta;
    nas_LTERSSIThresh     *pLTERSSIThresh;
    uint16_t              *pLTERSSIDelta;
    nas_LTESNRThreshold   *pLTESNRThresh;
    uint16_t              *pLTESNRDelta;
    nas_LTERSRQThresh     *pLTERSRQThresh;
    uint16_t              *pLTERSRQDelta;
    nas_LTERSRPThresh     *pLTERSRPThresh;
    uint16_t              *pLTERSRPDelta;
    nas_LTESigRptConfig   *pLTESigRptConfig;
    nas_TDSCDMARSCPThresh *pTDSCDMARSCPThresh;
    uint16_t              *pTDSCDMARSCPDelta;
    nas_TDSCDMARSSIThresh *pTDSCDMARSSIThresh;
    float             *pTDSCDMARSSIDelta;
    nas_TDSCDMAECIOThresh *pTDSCDMAECIOThresh;
    float             *pTDSCDMAECIODelta;
    nas_TDSCDMASINRThresh *pTDSCDMASINRThresh;
    float             *pTDSCDMASINRDelta;
} pack_nas_SLQSNasConfigSigInfo2_t;

typedef unpack_result_t  unpack_nas_SLQSNasConfigSigInfo2_t;

/**
 * Pack sets the signal strength reporting thresholds
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasConfigSigInfo2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasConfigSigInfo2_t *pReqParam
        );

/**
 * Unpack sets the signal strength reporting thresholds
 * @param[in]   pResp          qmi response from modem
 * @param[in]   respLen        qmi response length
 * @param[out]  pOutput        response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasConfigSigInfo2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasConfigSigInfo2_t *pOutput
        );

/**
 * This structure contains unpack set data capabilities callback indication parameters.
 *
 *  @param  dataCapsSize
 *          - Number of Data Capabilities
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  dataCaps
 *          - Data Capabilities
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t dataCapsSize;
    uint8_t dataCaps[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SetDataCapabilitiesCallback_ind_t;

/**
 * Data Capabilities indication unpack
 * @param[in]   pResp       qmi indication from modem
 * @param[in]   respLen     qmi indication length
 * @param[out]  pOutput      indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetDataCapabilitiesCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetDataCapabilitiesCallback_ind_t *pOutput
        );

/**
 * Gets the network registration preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 */

int pack_nas_GetNetworkPreference(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

/**
 *  Returns the network registration preference. This API is deprecated
 *  on MC73xx/EM73xx modules since firmware version SWI9X15C_05_xx_xx_xx
 *  and all EM74xx firmware versions. Please use unpack_nas_SLQSGetSysSelectionPref()
 *  for new firmware versions and new modules
 *
 *  \param  TechnologyPref[OUT]
 *          - Bitmask representing the radio technology preference set.
 *          - No bits set indicates to the device to automatically
 *            determine the technology to use
 *          - Values:
 *              - Bit 0 - Technology is 3GPP2
 *              - Bit 1 - Technology is 3GPP
 *          - Any combination of the following may be returned:
 *              - Bit 2 - Analog - AMPS if 3GPP2, GSM if 3GPP
 *              - Bit 3 - Digital - CDMA if 3GPP2, WCDMA if 3GPP
 *              - Bit 4 - HDR
 *              - Bit 5 - LTE
 *              - Bits 6 to 15 - Reserved
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Duration[OUT]
 *          - Duration of active preference
 *              - 0 - Permanent
 *              - 1 - Power cycle
 *              - 2 - Until the end of the next call or a power cycle
 *              - 3 - Until the end of the next call, a specified time, or a power cycle
 *              - 4 to 6 - Until the end of the next call
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  PersistentTechnologyPref[OUT]
 *          - Bit field representing persistent radio technology preference
 *              - Same representation as the pTechnologyPref parameter
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 * \param  Tlvresult
 *         - unpack result
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint32_t ActiveTechPref;
    uint32_t Duration;
    uint32_t PersistentTechPref;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetNetworkPreference_t;

/**
 * Gets the network registration preference unpack
 * @param[in]   pResp       qmi resp from modem
 * @param[in]   respLen     qmi resp length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetNetworkPreference(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_GetNetworkPreference_t *pOutput
);

/**
 *  This structure contains pack set network preference parameters.
 *  \param  TechnologyPref[IN]
 *          - Bitmask representing the radio technology preference set.
 *          - No bits set indicates to the device to automatically
 *            determine the technology to use
 *          - Values:
 *              - Bit 0 - Technology is 3GPP2
 *              - Bit 1 - Technology is 3GPP
 *          - Any combination of the following may be returned:
 *              - Bit 2 - Analog - AMPS if 3GPP2, GSM if 3GPP
 *              - Bit 3 - Digital - CDMA if 3GPP2, WCDMA if 3GPP
 *              - Bit 4 - HDR
 *              - Bit 5 - LTE
 *              - Bits 6 to 15 - Reserved
 *
 *  \param  Duration[IN]
 *          - Duration of active preference
 *              - 0 - Permanent
 *              - 1 - Power cycle
 *              - 2 - Until the end of the next call or a power cycle
 *              - 3 - Until the end of the next call, a specified time, or a power cycle
 *              - 4 to 6 - Until the end of the next call
 *
 * \param Tlvresult 
            - pack result
 */
typedef struct
{
    uint32_t TechnologyPref;
    uint32_t Duration;
    uint16_t Tlvresult;
} pack_nas_SetNetworkPreference_t;

/**
 *
 * Sets the network registration preference. This API is deprecated
 * on MC73xx/EM73xx modules since firmware version SWI9X15C_05_xx_xx_xx
 * and all EM74xx firmware versions. Please use pack_nas_SLQSSetSysSelectionPref()
 * for new firmware versions and new modules
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 */

int pack_nas_SetNetworkPreference(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_nas_SetNetworkPreference_t *reqArg
);

/**
 * This structure contains unpack set network preference parameters.
 *
 *  @param   Tlvresult
 *          - unpack result
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SetNetworkPreference_t;

/**
 * Sets the network registration preference unpack
 * @param[in]   pResp       qmi resp from modem
 * @param[in]   respLen     qmi resp length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetNetworkPreference(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SetNetworkPreference_t *pOutput
);


/**
 *  This structure contains unpack set roaming indicator callback indication parameters.
 *  @param  roaming
 *          - Roaming Indication\n
 *              - 0  - Roaming\n
 *              - 1  - Home\n
 *              - 2  - Roaming partner\n
 *              - >2 - Operator defined values
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t roaming;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SetRoamingIndicatorCallback_ind_t;

/**
 * Roaming indication unpack
 * @param[in]   pResp       qmi indication from modem
 * @param[in]   respLen     qmi indication length
 * @param[out]  pOutput     indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetRoamingIndicatorCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetRoamingIndicatorCallback_ind_t *pOutput
        );

/**
 *  This structure will hold the serving system parameters information
 *
 *  \param  registrationState - Registration state of the mobile
 *          - 0 - QMI_NAS_NOT_REGISTERED  Not registered;mobile is not
 *                currently searching for a new network to provide service
 *          - 1 - QMI_NAS_REGISTERED  Registered with a network
 *          - 2 - QMI_NAS_NOT_REGISTERED_SEARCHING  Not registered, but mobile
 *                is currently searching for a new network to provide service
 *          - 3 - QMI_NAS_REGISTRATION_DENIED Registration denied by the
 *                visible network
 *          - 4 - QMI_NAS_REGISTRATION_UNKNOWN Registration state is unknown
 *
 *  \param  csAttachState  - Circuit Switch domain attach state of the mobile
 *          - 0 - Unknown or not applicable
 *          - 1 - Attached
 *          - 2 - Detached
 *
 *  \param  psAttachState  - Packet domain attach state of the mobile
 *          - 0 - Unknown or not applicable
 *          - 1 - Attached
 *          - 2 - Detached
 *
 *  \param  selectedNetwork - Type of selected radio access network
 *          - 0x00 - Unknown
 *          - 0x01 - 3GPP2 network
 *          - 0x02 - 3GPP network
 *
 *  \param  radioInterfaceNo - Number of radio interfaces currently in use;
 *                             this  indicates how many radio_if identifiers
 *                             follow this field
 *
 *  \param  radioInterfaceList - Radio interface currently in use
 *                               (each is 1 byte)
 *          - 0x00 - None (no service)
 *          - 0x01 - cdma2000 1X
 *          - 0x02 - cdma2000 HRPD (1xEV-DO)
 *          - 0x03 - AMPS
 *          - 0x04 - GSM
 *          - 0x05 - UMTS
 *          - 0x08 - LTE
 *
 *  \param  hdrPersonality - HDR personality information (valid only for EVDO)
 *          - 0x00 - Unknown
 *          - 0x01 - HRPD
 *          - 0x02 - eHRPD
 *
 *  Note:   None
 */
typedef struct 
{
    uint8_t registrationState;
    uint8_t csAttachState;
    uint8_t psAttachState;
    uint8_t selectedNetwork;
    uint8_t radioInterfaceNo;
    uint8_t radioInterfaceList[NAS_SERVING_SYSTEM_INFO_MAX_RADIO_INTERFACE_LIST];
    uint8_t hdrPersonality;
}NASServingSystemInfo;


/**
 *  This structure contains unpack set serving system callback indication parameters.
 *  @param  SSInfo
 *          - Serving system parameters information\n
 *              - See \ref NASServingSystemInfo for more details
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    NASServingSystemInfo SSInfo;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SetServingSystemCallback_ind_t;

/**
 * Set Serving system indication unpack
 * @param[in]   pResp       qmi indication from modem
 * @param[in]   respLen     qmi indication length
 * @param[out]  pOutput     indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetServingSystemCallback_ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SetServingSystemCallback_ind_t *pOutput
);

/**
 * Pack get carrier aggregation event information.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 */

int pack_nas_SlqsGetLTECphyCAInfo(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen
);

/*
 * NAS LTE CPHY Scell States.
 */
typedef enum {
    eLITEQMI_NAS_LTE_CPHY_SCELL_STATE_DECONFIGURED=0x00,
    eLITEQMI_NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED=0x01,
    eLITEQMI_NAS_LTE_CPHY_SCELL_STATE_CONFIGURED_ACTIVATED=0x02,
}LITEQMI_NAS_LTE_CPHY_SCELL_STATE;

/*
 * NAS LTE CPHY Downlink Values.
 */
typedef enum {
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_6=0x00,
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_15=0x01,
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_25=0x02,
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_50=0x03,
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_75=0x04,
    eLITEQMI_NAS_LTE_CPHY_CA_BW_NRB_100=0x05,
}LITEQMI_NAS_LTE_CPHY_CA_BW_NRB;

/*
 * NAS LTE BAND CLASS E-UTRA Operating BANDs
 */
typedef enum {
    eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_1 = 120,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_2 = 121,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_3 = 122,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_4 = 123,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_5 = 124,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_6 = 125,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_7 = 126,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_8 = 127,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_9 = 128,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_10 = 129,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_11 = 130,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_12 = 131,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_13 = 132,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_14 = 133,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_17 = 134,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_33 = 135,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_34 = 136,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_35 = 137,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_36 = 138,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_37 = 139,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_38 = 140,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_39 = 141,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_40 = 142,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_18 = 143,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_19 = 144,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_20 = 145,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_21 = 146,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_24 = 147,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_25 = 148,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_41 = 149,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_42 = 150,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_43 = 151,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_23 = 152,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_26 = 153,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_32 = 154,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_125 = 155,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_126 = 156,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_127 = 157,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_28 = 158,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_29 = 159,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_30 = 160,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_66 = 161,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_250 = 162,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_46 = 163,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_27 = 164,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_31 = 165,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_71 = 166,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_47 = 167,
   eLITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND_48 = 168,
}LITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Indeicator Type.
 *
 *  \param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  \param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  \param  scell_state
 *          - Scell state Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_SCELL_STATE for more information.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */

typedef struct{
    uint32_t pci;
    uint32_t freq;
    LITEQMI_NAS_LTE_CPHY_SCELL_STATE scell_state;
    uint8_t TlvPresent;
}NASPhyCaAggScellIndType;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation Downlink Bandwidth of Scell.
 *
 *  \param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_CA_BW_NRB for more information.
 *
 */
typedef struct{
    LITEQMI_NAS_LTE_CPHY_CA_BW_NRB dl_bw_value;
    uint8_t TlvPresent;
}NASPhyCaAggScellDlBw;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Information.
 *
 *  \param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  \param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  \param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_CA_BW_NRB for more information.
 *
 *  \param  iLTEbandValue
 *          - Band value.
 *          - Range for LTE Band class 120 to 160.
 *
 *
 *  \param  scell_state
 *          - Scell state Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_SCELL_STATE for more information.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint32_t pci;
    uint32_t freq;
    LITEQMI_NAS_LTE_CPHY_CA_BW_NRB dl_bw_value;
    uint32_t iLTEbandValue;
    LITEQMI_NAS_LTE_CPHY_SCELL_STATE scell_state;
    uint8_t TlvPresent;
}NASPhyCaAggScellInfo;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Pcell Information.
 *
 *  \param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  \param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  \param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_CA_BW_NRB for more information.
 *
 *  \param  scell_state
 *          - Scell state Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_SCELL_STATE for more information.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint32_t pci;
    uint32_t freq;
    LITEQMI_NAS_LTE_CPHY_CA_BW_NRB dl_bw_value;
    uint32_t iLTEbandValue;
    uint8_t TlvPresent;
}NASPhyCaAggPcellInfo;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Index.
 *
 *  \param  scell_idx
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t scell_idx;
    uint8_t TlvPresent;
}NASPhyCaAggScellIndex;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Index.
 *
 *  \param  cphy_scell_info_list_len
 *          - Number of sets of the following elements:
 *             - pci
 *             - freq
 *             - cphy_ca_dl_bandwidth
 *             - band
 *             - scell_state
 *             - scell_idx
 *  \param pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *  \param freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *  \param cphy_ca_dl_bandwidth
 *          - Downlink Bandwidth Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_CA_BW_NRB for more information.
 *  \param band
 *          - Band value.
 *          - See \ref LITEQMI_LTE_BAND_CLASS_E_UTRA_OPERATING_BAND for more information.
 *  \param scell_state
 *          - Scell state Values.
 *          - See \ref LITEQMI_NAS_LTE_CPHY_SCELL_STATE for more information.
 *  \param scell_idx
 *          - Scell index Values.
 *          - Range for index values: 0 to 7.
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t cphy_scell_info_list_len;
    uint16_t pci[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    uint16_t freq[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    LITEQMI_NAS_LTE_CPHY_CA_BW_NRB cphy_ca_dl_bandwidth[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    uint16_t band[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    LITEQMI_NAS_LTE_CPHY_SCELL_STATE scell_state[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    uint16_t scell_idx[NAS_MAX_PHY_CA_AGG_SCELL_ARRRY_SIZE];
    uint8_t TlvPresent;
}NASPhyCaAggScellArray;

/**
 * This structure contains the parameters for aggregation event information.
 *
 *  \param  PhyCaAggScellIndType
 *          - see \ref NASPhyCaAggScellIndType
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  PhyCaAggScellDlBw
 *          - see \ref NASPhyCaAggScellDlBw
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  PhyCaAggScellInfo
 *          - see \ref NASPhyCaAggScellInfo
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  PhyCaAggPcellInfo
 *          - see \ref NASPhyCaAggPcellInfo
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  PhyCaAggScellIndex
 *          - see \ref NASPhyCaAggScellIndex
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  \param  PhyCaAggScellArray
 *          - see \ref NASPhyCaAggScellArray
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 */
typedef struct
{
    NASPhyCaAggScellIndType    PhyCaAggScellIndType;
    NASPhyCaAggScellDlBw       PhyCaAggScellDlBw;
    NASPhyCaAggScellInfo       PhyCaAggScellInfo;
    NASPhyCaAggPcellInfo       PhyCaAggPcellInfo;
    NASPhyCaAggScellIndex      PhyCaAggScellIndex;
    NASPhyCaAggScellArray      PhyCaAggScellArray;
} NasGetLTECphyCaInfo;

/**
 *  This structure contains unpack get carrier aggregation event information parameters.
 *  @param  LTECphyCa
 *          - Carrier aggregation event information\n
 *              - See \ref NasGetLTECphyCaInfo for more details
 *
 *  @param  Tlvresult
 *          - unpack result
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    NasGetLTECphyCaInfo LTECphyCaInfo;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SlqsGetLTECphyCAInfo_t;

/**
 * Unack get carrier aggregation event information.
 * @param[in]   pResp       qmi resp from modem
 * @param[in]   respLen     qmi resp length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SlqsGetLTECphyCAInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SlqsGetLTECphyCAInfo_t *pOutput
);

/**
 * Structure used to store Emergency Mode TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *  @param EmerMode   - Emergency Mode Value
 */
typedef struct 
{
    uint8_t TlvPresent;
    uint8_t EmerMode;
}NASEmergencyModeTlv;

/**
 *
 * Structure used to store Mode Preference TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                      in the QMI response
 *
 *   @param ModePref   - Mode Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t ModePref;
}NASModePreferenceTlv;

/**
 *
 * Structure used to store all Band Preference TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *  @param band_pref  - Roaming Indication Value
 */
typedef struct 
{
    uint8_t      TlvPresent;
    uint64_t band_pref;
}NASBandPreferenceTlv;

/**
 * Structure used to store CDMA PRL Preference TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                      in the QMI response
 *
 *   @param PRLPref    - CDMA PRL Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t PRLPref;
}NASPRLPreferenceTlv;

/**
 * Structure used to store Roaming Preference TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                      in the QMI response
 *
 *  @param  RoamPref  - Roaming Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t RoamPref;
}NASRoamPreferenceTlv;

/**
 * Structure used to store LTE Band Preference TLV Value.
 *
 *  @param TlvPresent  - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *  @param LTEBandPref - LTE Band Preference Value
 */
typedef struct 
{
    uint8_t      TlvPresent;
    uint64_t LTEBandPref;
}NASLTEBandPreferenceTlv;

/**
 * Structure used to store Network Selection Preference TLV Value.
 *
 *  @param TlvPresent - Boolean indicating the presence of the TLV
 *                      in the QMI response
 *
 *  @param NetSelPref - Network Selection Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t NetSelPref;
}NASNetSelPreferenceTlv;

/**
 * Structure used to store Service domain preference TLV Value.
 *
 * @param TlvPresent    - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *  @param SrvDomainPref - Service Domain Preference Value
 */
typedef struct 
{
    uint8_t TlvPresent;
    uint32_t SrvDomainPref;
}NASServDomainPrefTlv;

/**
 * Structure used to store GSM/WCDMA acquisition order preference TLV Value.
 *
 *  @param TlvPresent     - Boolean indicating the presence of the TLV
 *                          in the QMI response
 *
 *  @param GWAcqOrderPref - GSM/WCDMA acquisition order Preference Value
 */
typedef struct 
{
    uint8_t TlvPresent;
    uint32_t GWAcqOrderPref;
}NASGWAcqOrderPrefTlv;

/**
 *
 * Structure used to store acquisition order preference TLV Value.
 *
 * @param TlvPresent - Boolean indicating the presence of the TLV
 *                     in the QMI response
 *
 * @param AcqOrderPref - Acquisition order Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t AcqOrderLen;
    uint8_t AcqOrderPref[255];
}NASAcqOrderPrefTlv;

/**
 *
 * Structure used to store Disabled RAT Bitmask TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 * @param RatDisabledMask - Bitmask representing the radio technologies that are disabled
 */
typedef struct
{
    uint8_t TlvPresent;
    uint16_t RatDisabledMask;
}NASRatDisabledMaskTlv;

/**
 *
 * Structure used to store CIOT LTE Operational Mode Preference TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 * @param CiotLteOpModePref - Bitmask representing the CIoT LTE operational mode preference
 */
typedef struct
{
    uint8_t TlvPresent;
    uint64_t CiotLteOpModePref;
} NASCiotLteOpModePrefTlv;

/**
 *
 * Purpose: Structure used to store LTE M1 Band Preference TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                          in the QMI response
 *
 * @param LteM1BandPref  - Bitmask representing the LTE M1 band preference
 */
typedef struct
{
    uint8_t TlvPresent;
    uint64_t LteM1BandPref;
}NASLteM1BandPrefTlv;

/**
 *
 * Structure used to store LTE NB1 Band Preference TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                          in the QMI response
 *
 * @param LteNB1BandPref - Bitmask representing the LTE NB1 band preference
 */
typedef struct
{
    uint8_t      TlvPresent;
    uint64_t LteNB1BandPref;
} NASLteNB1BandPrefTlv;

/**
 *
 * Structure used to store CIOT acquisition order preference TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                          in the QMI response
 *
 * @param CiotAcqOrderPref - CIOT Acquisition order Preference Value
 */
typedef struct
{
    uint8_t TlvPresent;
    uint8_t CiotAcqOrderLen;
    uint32_t CiotAcqOrderPref[255];
} NASCiotAcqOrderPrefTlv;

/**
 *
 * Structure used to store NR5G Band Preference TLV Value.
 *
 * @param TlvPresent     - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 * @param Nr5gBandPref    - NR5G Band Preference
 */
typedef struct
{
    uint8_t      TlvPresent;
    uint64_t Nr5gBandPrefbits1_64;
    uint64_t Nr5gBandPrefbits_65_128;
    uint64_t Nr5gBandPrefbits_129_192;
    uint64_t Nr5gBandPrefbits_193_256;
}NASNr5gBandPrefTlv;

/**
 * Structure used to store all QMI Notification parameters.
 *
 *  @param  EMTlv    - Emergency mode Tlv
 *                   -  See \ref NASEmergencyModeTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  MPTlv    - Mode preference Tlv
 *                   -  See \ref NASModePreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  BPTlv    - Band preference Tlv
 *                   -  See \ref NASBandPreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  PRLPTlv  - PRL preference Tlv
 *                   -  See \ref NASPRLPreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  RPTlv    - Roaming preference Tlv
 *                   -  See \ref NASRoamPreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  LBPTlv   - LTE band preference Tlv
 *                   -  See \ref NASLTEBandPreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  NSPTlv   - Network selection preference Tlv
 *                   -  See \ref NASNetSelPreferenceTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  SDPTlv   - Service domain preference Tlv
 *                   -  See \ref NASServDomainPrefTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  GWAOPTlv - GSM WCDMA Acquisition Order Preference Tlv
 *                   -  See \ref NASGWAcqOrderPrefTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  AOPTlv   - Acquisition Order Preference Tlv
 *                   -  See \ref NASAcqOrderPrefTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  RatDMTlv - Disabled RAT Bitmask Tlv
 *                   -  See \ref NASRatDisabledMaskTlv for more details
 *                   - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  CiotOpMPTlv - CIOT LTE Operational Mode Preference Tlv
 *                      -  See \ref NASCiotLteOpModePrefTlv for more details
 *                      - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  M1BandPTlv - LTE M1 Band Preference Tlv
 *                     -  See \ref NASLteM1BandPrefTlv for more details
 *                     - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  NB1BandPTlv - LTE NB1 Band Preference Tlv
 *                      -  See \ref NASLteNB1BandPrefTlv for more details
 *                      - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  CiotAOPTlv - CIOT acquisition order preference Tlv
 *                     -  See \ref NASCiotAcqOrderPrefTlv for more details
 *                     - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  NR5gBandPTlv - NR5G Band Preference Tlv
 *                       - See \ref NASNr5gBandPrefTlv for more details
 *                       - Bit to check in ParamPresenceMask - <B>40</B>
 *
 * Note:    None
 */
typedef struct{
    NASEmergencyModeTlv     EMTlv;
    NASModePreferenceTlv    MPTlv;
    NASBandPreferenceTlv    BPTlv;
    NASPRLPreferenceTlv     PRLPTlv;
    NASRoamPreferenceTlv    RPTlv;
    NASLTEBandPreferenceTlv LBPTlv;
    NASNetSelPreferenceTlv  NSPTlv;
    NASServDomainPrefTlv    SDPTlv;
    NASGWAcqOrderPrefTlv    GWAOPTlv;
    NASAcqOrderPrefTlv      AOPTlv;
    NASRatDisabledMaskTlv   RatDMTlv;
    NASCiotLteOpModePrefTlv CiotOpMPTlv;
    NASLteM1BandPrefTlv     M1BandPTlv;
    NASLteNB1BandPrefTlv    NB1BandPTlv;
    NASCiotAcqOrderPrefTlv  CiotAOPTlv;
    NASNr5gBandPrefTlv      NR5gBandPTlv;
}NASQmiCbkNasSystemSelPrefInd;


/**
 *  This structure contains unpack set system selection preference callback indication parameters.
 *  @param Info 
 *          - Structure used to store all QMI Notification Info.
 *              - See \ref NASQmiCbkNasSystemSelPrefInd for more details
 *
 *  @param Tlvresult 
 *          - unpack result
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    NASQmiCbkNasSystemSelPrefInd Info;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t;

/**
 * System Selection Preference indication unpack
 * @param[in]   pResp       qmi indication from modem
 * @param[in]   respLen     qmi indication length
 * @param[out]  pOutput     indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */

int unpack_nas_SLQSSetSysSelectionPrefCallBack_ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSSetSysSelectionPrefCallBack_ind_t *pOutput
);


/**
 *  This structure contains OTA message
 *
 *  \param  Tlvresult
 *          - unpack result
 *
 *  \param  message_type
 *          - message type
 *            - 0 - LTE ESM uplink
 *            - 1 - LTE ESM downlink
 *            - 2 - LTE EMM uplink
 *            - 3 - LTE EMM downlink
 *            - 4 - GSM/UMTS uplink
 *            - 5 - GSM/UMTS downlink
 *
 *  \param  data_len
 *          - OTA Message Content Length
 *
 *  \param  data_buf
 *          - OTA Message Content
 *
 */
typedef struct  {
    uint8_t    TlvPresent;
    uint32_t    message_type;
    uint16_t    data_len;
    uint8_t    data_buf[NAS_OTA_MESSAGE_MAX_BUF_SIZE];
}NASOTAMessageTlv;

/**
 *  This structure contains LTE Nas Release Infomation
 *
 *  \param  Tlvresult
 *          - unpack result
 *
 *  \param  nas_release
 *          - LTE NAS release
 *
 *  \param  nas_major
 *          - LTE NAS version major
 *
 *  \param  nas_minor
 *          - LTE NAS version minor
 *
 */
typedef struct {
    uint8_t    TlvPresent;
    uint8_t    nas_release;
    uint8_t    nas_major;
    uint8_t    nas_minor;
}NASLteNasReleaseInfoTlv;

/**
 *  This structure contains LTE Nas time Infomation
 *
 *  \param  Tlvresult
 *          - unpack result
 *
 *  \param  time
 *          - Seconds in local time since Jan. 6th 1980 00:00:00 UTC
 */
typedef struct {
    uint8_t    TlvPresent;
    uint64_t time;
}NASTimeInfoTlv;

/**
 *  This structure contains OTA message
 *
 *  \param   otaMsgTlv
 *              - See \ref NASOTAMessageTlv for more details
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param   nasRelInfoTlv
 *              - See \ref NASLteNasReleaseInfoTlv for more details
 *              - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param   timeTlv
 *              - See \ref NASTimeInfoTlv for more details
 *              - Bit to check in ParamPresenceMask - <B>17</B>
 *
 */
typedef struct  {
    NASOTAMessageTlv otaMsgTlv;
    NASLteNasReleaseInfoTlv nasRelInfoTlv;
    NASTimeInfoTlv timeTlv;
}NASQmiCbkNasSwiOTAMessageInd;


/**
 *  This structure contains unpack OTA message indication parameters.
 *  @param Info 
 *          - Structure used to store all QMI Notification Info.
 *              - See \ref NASQmiCbkNasSwiOTAMessageInd for more details
 *
 *  @param Tlvresult 
 *          - unpack result
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    NASQmiCbkNasSwiOTAMessageInd Info;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t;

/**
 * OTA message indication unpack
 * @param[in]   pResp       qmi indication from modem
 * @param[in]   respLen     qmi indication length
 * @param[out]  pOutput     indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasSwiOTAMessageCallback_ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSNasSwiOTAMessageCallback_ind_t *pOutput
);

/**
 *  Structure contains Manual Network Register Information parameters
 *
 *  \param  mcc
 *          - A 16-bit integer representation of Mobile Country Code.
 *            Range - 0 to 999.
 *
 *  \param  mnc
 *          - A 16-bit integer representation of Mobile Network Code.
 *            Range - 0 to 999.
 *
 *  \param  rat
 *          - Radio access technology for which to register.
 *              - 0x04 - RADIO_IF_GSM
 *              - 0x05 - RADIO_IF_UMTS
 *              - 0x08 - RADIO_IF_LTE
 */
typedef struct
{
    uint16_t  mcc;
    uint16_t  mnc;
    uint32_t rat;
} nas_MNRInfo;

/**
 *  This structure contains Initiate Network Registration request parameters
 *
 *  \param  regAction
 *          - Specifies one of the following register actions :
 *            - AUTO_REGISTER - Device registers according to its provisioning
 *                  and optional parameters supplied with the command are ignored.
 *            - MANUAL_REGISTER - Device registers to a specified network and
 *                  the optional Manual Network Register Information parameter
 *                  pMNRInfo must also be included for the command to process
 *                  successfully and supported only for 3GPP.
 *
 *  \param  pMNRInfo [Optional]
 *          - Pointer to structure MNRInfo
 *              - See \ref nas_MNRInfo for more information
 *
 *  \param  pChangeDuration [Optional]
 *          - Duration of the change.
 *              - 0x00 - Power cycle - Remains active until the next device power
 *                       cycle
 *              - 0x01 - Permanent - Remains active through power cycles until
 *                       changed by the client
 *
 * \param  pMncPcsDigitStatus [Optional]
 *         - MNC PCS Digit Include Status
 *             - True - MNC is a 3-digit value.
 *             - False - MNC is a 2-digit value.
 */
typedef struct
{
    uint32_t    regAction;
    nas_MNRInfo *pMNRInfo;
    uint32_t    *pChangeDuration;
    uint8_t     *pMncPcsDigitStatus;
} pack_nas_SLQSInitiateNetworkRegistration_t;

typedef unpack_result_t  unpack_nas_SLQSInitiateNetworkRegistration_t;

/**
 * Pack initiates a network registration.This API is deprecated on
 *  MC73xx/EM73xx modules since firmware version SWI9X15C_05_xx_xx_xx
 *  and all EM74xx firmware versions. Please use pack_nas_SLQSSetSysSelectionPrefExt()
 *  instead for new firmware versions and new modules.
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSInitiateNetworkRegistration(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSInitiateNetworkRegistration_t *pReqParam
        );

/**
 * Unpack initiates a network registration.This API is deprecated on
 *  MC73xx/EM73xx modules since firmware version SWI9X15C_05_xx_xx_xx
 *  and all EM74xx firmware versions. Please use unpack_nas_SLQSSetSysSelectionPrefExt()
 *  instead for new firmware versions and new modules.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSInitiateNetworkRegistration(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSInitiateNetworkRegistration_t *pOutput
        );

/**
 *  This structure contains the OTA message indication.
 *
 *  \param  lteEsmUl
 *          - 0 - do not report
 *          - 1 - report LTE ESM uplink messages
 *
 *  \param  lteEsmDl
 *          - 0 - do not report
 *          - 1 - report LTE ESM downlink messages
 *
 *  \param  lteEmmUl
 *          - 0 - do not report
 *          - 1 - report LTE EMM uplink messages
 *
 *  \param  lteEmmDl
 *          - 0 - do not report
 *          - 1 - report GSM/UMTS uplink messages
 *
 *  \param  gsmUmtsUl
 *          - 0 - do not report
 *          - 1 - report GSM/UMTS uplink messages
 *
 *  \param  gsmUmtsDl
 *          - 0 - do not report
 *          - 1 - report GSM/UMTS downlink messages
 *
 *  \param  pRankIndicatorInd
 *          - 0 - do not report
 *          - 1 - report Rank Indicator messages
 *
 *  \param  pTimer
 *          - 0 - do not report
 *          - 1 - report Timer Indicator messages
 */
typedef struct
{
    uint8_t lteEsmUl;
    uint8_t lteEsmDl;
    uint8_t lteEmmUl;
    uint8_t lteEmmDl;
    uint8_t gsmUmtsUl;
    uint8_t gsmUmtsDl;
    uint8_t *pRankIndicatorInd;
    uint8_t *pTimer;
}pack_nas_SLQSNasSwiIndicationRegister_t;

typedef unpack_result_t  unpack_nas_SLQSNasSwiIndicationRegister_t;

/**
 * Pack sets the registration state for different QMI_NAS SWI indications.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasSwiIndicationRegister(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasSwiIndicationRegister_t *pReqParam
        );

/**
 * unpack sets the registration state for different QMI_NAS SWI indications.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasSwiIndicationRegister(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasSwiIndicationRegister_t *pOutput
        );

/**
 *  This structure contains pack get operator name for specificed network parameters.
 *  @param  mcc
 *          - A 16-bit integer representation of MCC. Range: 0 to 999
 *
 *  @param  mnc
 *          - A 16-bit integer representation of MNC. Range: 0 to 999
 *
 *  @param  pMncPcsStatus
 *          - MNC PCS Digit Include Status
 *          - Used to interpret the length of the corresponding MNC reported
 *            in the PLMN TLV(0x01).
 *          - Values
 *            - TRUE -  MNC is a three-digit value. e.g. a reported value of 90
 *                      corresponds to an MNC value of 090
 *            - FALSE - MNC is a two-digit value. e.g. a reported value of 90
 *                      corresponds to an MNC value of 90
 *
 *  \note    If pMncPcsStatus is not present, an MNC smaller than 100
 *           is assumed to be a two-digit value, and an MNC greater than
 *           or equal to 100 is assumed to be a three digit value.
 */
typedef struct
{
    uint16_t mcc;
    uint16_t mnc;
    uint8_t  *pMncPcsStatus;
}pack_nas_SLQSGetPLMNName_t;

/**
 *  This structure contains unpack get operator name for specificed network parameters.
 *  @param  spnEncoding
 *          - Coding scheme used for service provider name. This value is
 *            ignored if spn_len is zero
 *            Values:
 *              - 0x00 - SMS default 7-bit coded alphabet as defined in
 *                3GPP TS 23.038 with bit 8 set to 0
 *              - 0x01 - UCS2 (16 bit, little-endian) 3GPP TS 23.038
 *              - Note: This value is ignored if spnLength is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  spnLength
 *          - Length of SPN which follows
 *          - Note: This SPN value is ignored if spnLength is zero and spnEncoding is 0xff.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  spn
 *          - Service Provider name string
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  shortNameEn
 *          - Coding scheme used for PLMN short name. This value is
 *            ignored if PLMN short name length is zero
 *            Values:
 *              - 0x00 - SMS default 7-bit coded alphabet as defined in
 *                3GPP TS 23.038 with bit 8 set to 0
 *              - 0x01 - UCS2 (16 bit, little-endian) 3GPP TS 23.038
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  shortNameCI
 *          - Indicates whether the country initials are to be added to the
 *            shortName. This value is ignored if shortNameLen is zero. Values:
 *              - 0x00 - Do not add the letters for the countrys initials
 *                to the name
 *              - 0x01 - Add the countrys initials and a text string to
 *                the name
 *              - 0xFF - Not specified
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  shortNameSB
 *          - PLMN short name spare bits. This value is ignored if
 *            shortNameLen is zero. Values:
 *              - 0x01 - Bit 8 is spare and set to 0 in octet
 *              - 0x02 - Bits 7 and 8 are spare and set to 0 in octet n
 *              - 0x03 - Bits 6 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x04 - Bits 5 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x05 - Bits 4 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x06 - Bits 3 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x07 - Bits 2 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x00 - Carries no information about the number of spare
 *                bits in octet n
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  shortNameLen
 *          - Length of shortName which follows
 *          - Note: This shortName value is ignored if shortNameLen is zero and shortNameEn is 0xff.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  shortName
 *          - PLMN short name
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  longNameEn
 *          - Coding scheme used for PLMN long name. This value is
 *            ignored if PLMN long name length is zero
 *            Values:
 *              - 0x00 - SMS default 7-bit coded alphabet as defined in
 *                3GPP TS 23.038 with bit 8 set to 0
 *              - 0x01 - UCS2 (16 bit, little-endian) 3GPP TS 23.038
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  longNameCI
 *          - Indicates whether the country initials are to be added to the
 *            longName. This value is ignored if longNameLen is zero. Values:
 *              - 0x00 - Do not add the letters for the countrys initials
 *                to the name
 *              - 0x01 - Add the countrys initials and a text string to
 *                the name
 *              - 0xFF - Not specified
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  longNameSB
 *          - PLMN long name spare bits. This value is ignored if
 *            longNameLen is zero. Values:
 *              - 0x01 - Bit 8 is spare and set to 0 in octet
 *              - 0x02 - Bits 7 and 8 are spare and set to 0 in octet n
 *              - 0x03 - Bits 6 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x04 - Bits 5 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x05 - Bits 4 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x06 - Bits 3 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x07 - Bits 2 to 8 (inclusive) are spare and set to 0 in
 *                octet n
 *              - 0x00 - Carries no information about the number of spare
 *                bits in octet n
 *              - Note: This value is ignored if shortNameLen is zero.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  longNameLen
 *          - Length of longName which follows
 *          - Note: This longName value is ignored if longNameLen is zero and longNameEn is 0xff.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  longName
 *          - PLMN long name
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t spnEncoding;
    uint8_t spnLength;
    char spn[255];
    uint8_t shortNameEn;
    uint8_t shortNameCI;
    uint8_t shortNameSB;
    char shortNameLen;
    uint8_t shortName[255];
    uint8_t longNameEn;
    uint8_t longNameCI;
    uint8_t longNameSB;
    uint8_t longNameLen;
    char longName[255];
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSGetPLMNName_t;

/**
 * Pack get operator name for specificed network.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetPLMNName(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSGetPLMNName_t *reqArg
        );

/**
 * unpack get operator name for specificed network.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetPLMNName(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSGetPLMNName_t *pOutput
);

/**
 * Pack retrieves cell location-related information.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetCellLocationInfo(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );
/**
 *  This structure contains information about the Network Measurement Report
 *  (NMR) Cell Information.
 *
 *  @param  nmrCellID
 *          - Cell ID.
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *
 *  @param  nmrPlmn[NAS_PLMN_LENGTH]
 *          - MCC/MNC information coded as octet 3, 4, and 5.
 *          - This field is ignored when nmrCellID is not present.
 *
 *  @param  nmrLac
 *          - Location area code.
 *          - This field is ignored when nmrCellID is not present.
 *              - 0xFFFF - Not Available
 *
 *  @param  nmrArfcn
 *          - Absolute RF channel number.
 *              - 0xFFFF - Not Available
 *
 *  @param  nmrBsic
 *          - Base station identity code.
 *              - 0xFF - Not Available
 *
 *  @param  nmrRxLev
 *          - Cell Rx measurement.
 *          - Values range between 0 and 63.
 *          - Mapped to a measured signal level:
 *              - Rxlev 0 is a signal strength less than -110 dBm
 *              - Rxlev 1 is -110 dBm to -109 dBm
 *              - Rxlev 2 is -109 dBm to -108 dBm
 *              - ...
 *              - Rxlev 62 is -49 dBm to -48 dBm
 *              - Rxlev 63 is greater than -48 dBm
 *              - 0xFFFF - Not Available
 *
 */
typedef struct
{
    uint32_t nmrCellID;
    uint8_t  nmrPlmn[NAS_PLMN_LENGTH];
    uint16_t  nmrLac;
    uint16_t  nmrArfcn;
    uint8_t  nmrBsic;
    uint16_t  nmrRxLev;
} nas_nmrCellInfo;

/**
 *  This structure contains information about the GERAN Network.
 *
 *  @param  cellID
 *          - Cell ID.
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *
 *  @param  plmn[NAS_PLMN_LENGTH]
 *          - MCC/MNC information coded as octet 3, 4, and 5.
 *          - This field is ignored when nmrCellID is not present.
 *
 *  @param  lac
 *          - Location area code.
 *          - This field is ignored when nmrCellID is not present.
 *              - 0xFFFF - Not Available
 *
 *  @param  arfcn
 *          - Absolute RF channel number.
 *              - 0xFFFF - Not Available
 *
 *  @param  bsic
 *          - Base station identity code.
 *              - 0xFF - Not Available
 *
 *  @param  timingAdvance
 *          - Measured delay (in bit periods; 1 bit period = 48/13 microsecond)
 *            of access burst transmission on RACH or PRACH to the expected
 *            signal from an MS at zero distance under static channel
 *            conditions.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  rxLev
 *          - Serving Cell Rx measurement.
 *          - Values range between 0 and 63.
 *          - Mapped to a measured signal level:
 *              - Rxlev 0 is a signal strength less than -110 dBm
 *              - Rxlev 1 is -110 dBm to -109 dBm
 *              - Rxlev 2 is -109 dBm to -108 dBm
 *              - ...
 *              - Rxlev 62 is -49 dBm to -48 dBm
 *              - Rxlev 63 is greater than -48 dBm
 *              - 0xFFFF - Not Available
 *
 *  @param  nmrInst
 *          - Provides the number of set of instances which follow.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  insNmrCellInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_nmrCellInfo for more information.
 *
 */
typedef struct
{
    uint32_t        cellID;
    uint8_t         plmn[NAS_PLMN_LENGTH];
    uint16_t        lac;
    uint16_t        arfcn;
    uint8_t         bsic;
    uint32_t        timingAdvance;
    uint16_t        rxLev;
    uint8_t         nmrInst;
    nas_nmrCellInfo insNmrCellInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_GERANInfo;

/**
 *  This structure contains information about the GERAN Instances in UMTS
 *  Network.
 *
 *  @param  geranArfcn
 *          - Absolute RF channel number.
 *
 *  @param  geranBsicNcc
 *          - Base station identity code network color code.
 *          - 0xFF indicates information is not present.
 *
 *  @param  geranBsicBcc
 *          - Base station identity code base station color code.
 *          - 0xFF indicates information is not present.
 *
 *  @param  geranRssi
 *          - Received signal strength indicator.
 *
 */
typedef struct
{
    uint16_t geranArfcn;
    uint8_t  geranBsicNcc;
    uint8_t  geranBsicBcc;
    int16_t  geranRssi;
} nas_geranInstInfo;

/**
 *  This structure contains information about the UMTS Instances in UMTS
 *  Network.
 *
 *  @param  umtsUarfcn
 *          - UTRA absolute RF channel number.
 *
 *  @param  umtsPsc
 *          - Primary scrambling code.
 *
 *  @param  umtsRscp
 *          - Received signal code power.
 *
 *  @param  umtsEcio
 *          - ECIO(Signal-to-Interference-ratio).
 *
 */
typedef struct
{
    uint16_t umtsUarfcn;
    uint16_t umtsPsc;
    int16_t umtsRscp;
    int16_t umtsEcio;
} nas_UMTSinstInfo;

/**
 *  This structure contains information about the UMTS Network.
 *
 *  @param  cellID
 *          - Cell ID.
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *
 *  @param  plmn[NAS_PLMN_LENGTH]
 *          - MCC/MNC information coded as octet 3, 4, and 5.
 *          - This field is ignored when nmrCellID is not present.
 *
 *  @param  lac
 *          - Location area code.
 *          - This field is ignored when nmrCellID is not present.
 *              - 0xFFFF - Not Available
 *
 *  @param  uarfcn
 *          - UTRA absolute RF channel number.
 *              - 0xFFFF - Not Available
 *
 *  @param  psc
 *          - Primary scrambling code.
 *              - 0xFFFF - Not Available
 *
 *  @param  rscp
 *          - Received signal code power.
 *              - 0xFFFF - Not Available
 *
 *  @param  ecio
 *          - ECIO(Signal-to-Interference-ratio).
 *              - 0xFFFF - Not Available
 *
 *  @param  umtsInst
 *          - Provides the number of set of UMTS info instances.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  UMTSInstInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_UMTSinstInfo for more information.
 *
 *  @param  geranInst
 *          - Provides the number of set of GERAN info instances.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  GeranInstInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_geranInstInfo for more information.
 *
 */
typedef struct
{
    uint16_t          cellID;
    uint8_t           plmn[NAS_PLMN_LENGTH];
    uint16_t          lac;
    uint16_t          uarfcn;
    uint16_t          psc;
    int16_t           rscp;
    int16_t           ecio;
    uint8_t           umtsInst;
    nas_UMTSinstInfo  UMTSInstInfo[NAS_MAX_DESCRIPTION_LENGTH];
    uint8_t           geranInst;
    nas_geranInstInfo GeranInstInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_UMTSInfo;

/**
 *  This structure contains information about the CDMA Network.
 *
 *  @param  sid
 *          - System ID.
 *              - 0xFFFF - Not Available
 *
 *  @param  nid
 *          - Network ID.
 *              - 0xFFFF - Not Available
 *
 *  @param  baseId
 *          - Base station ID.
 *              - 0xFFFF - Not Available
 *
 *  @param  refpn
 *          - Reference PN.
 *              - 0xFFFF - Not Available
 *
 *  @param  baseLat
 *          - Latitude of the current base station in units of 0.25 sec.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  baseLong
 *          - Longitude of the current base station in units of 0.25 sec.
 *              - 0xFFFFFFFF - Not Available
 *
 */
typedef struct
{
    uint16_t  sid;
    uint16_t  nid;
    uint16_t  baseId;
    uint16_t  refpn;
    uint32_t  baseLat;
    uint32_t  baseLong;
} nas_CDMAInfo;

/**
 *  This structure contains information about the Cell parameters.
 *
 *  @param  pci
 *          - Physical cell ID.
 *          - Range: 0 to 503.
 *
 *  @param  rsrq
 *          - Current RSRQ in 1/10 dB as measured by L1.
 *          - Range: -20.0 dB to -3.0 dB.
 *
 *  @param  rsrp
 *          - Current RSRP in 1/10 dBm as measured by L1.
 *          - Range: -140.0 dBm to -44.0 dBm.
 *
 *  @param  rssi
 *          - Current RSSI in 1/10 dBm as measured by L1.
 *          - Range: -120.0 dBm to 0.
 *
 *  @param  srxlev
 *          - Cell selection Rx level (Srxlev) value.
 *          - Range: -128 to 128.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 */
typedef struct
{
    uint16_t pci;
    int16_t rsrq;
    int16_t rsrp;
    int16_t rssi;
    int16_t srxlev;
} nas_cellParams;

/**
 *  This structure contains information about the LTE Intra-Frequency Network.
 *
 *  @param  ueInIdle
 *          - TRUE if the UE is in Idle mode, otherwise FALSE.
 *              - 0xFF - Not Available
 *
 *  @param  plmn[NAS_PLMN_LENGTH]
 *          - PLMN ID coded as octet 3, 4, and 5.
 *
 *  @param  tac
 *          - Tracking area code.
 *              - 0xFFFF - Not Available
 *
 *  @param  globalCellId
 *          - Global cell ID in the system information block.
 *              - 0xFFFFFFFF - Not Available
 *
 *  @param  earfcn
 *          - E-UTRA absolute radio frequency channel number of the serving
 *            cell.
 *          - Range: 0 to 65535.
 *              - 0xFFFF - Not Available
 *
 *  @param  servingCellId
 *          - LTE serving cell ID.
 *          - Range: 0 to 503.
 *          - This is the cell ID of the serving cell and can be found in the
 *            cell list.
 *              - 0xFFFF - Not Available
 *
 *  @param  cellReselPriority
 *          - Priority for serving frequency.
 *          - Range: 0 to 7.
 *          - This field is only valid when ue_in_idle is TRUE.
 *              - 0xFF - Not Available
 *
 *  @param  sNonIntraSearch
 *          - S non-intra search threshold to control non-intrafrequency
 *            searches.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *              - 0xFF - Not Available
 *
 *  @param  threshServingLow
 *          - Serving cell low threshold.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *              - 0xFF - Not Available
 *
 *  @param  sIntraSearch
 *          - S Intra search threshold.
 *          - Range: 0 to 31.
 *          - The current cell measurement must fall below this threshold to
 *            consider intrafrequency for reselection.
 *          - This field is only valid when ue_in_idle is TRUE.
 *              - 0xFF - Not Available
 *
 *  @param  cellsLen
 *          - Provides the number of set of cell params.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  CellParams[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_cellParams for more information.
 *
 */
typedef struct
{
    uint8_t        ueInIdle;
    uint8_t        plmn[NAS_PLMN_LENGTH];
    uint16_t       tac;
    uint32_t       globalCellId;
    uint16_t       earfcn;
    uint16_t       servingCellId;
    uint8_t        cellReselPriority;
    uint8_t        sNonIntraSearch;
    uint8_t        threshServingLow;
    uint8_t        sIntraSearch;
    uint8_t        cellsLen;
    nas_cellParams CellParams[NAS_MAX_DESCRIPTION_LENGTH];
} nas_LTEInfoIntrafreq;

/**
 *  This structure contains information about the inter-frequency.
 *
 *  @param  earfcn
 *          - E-UTRA absolute radio frequency channel number of the serving
 *            cell.
 *          - Range: 0 to 65535.
 *
 *  @param  threshXLow
 *          - Cell Srxlev low threshold.
 *          - Range: 0 to 31.
 *          - When the serving cell does not exceed thresh_serving_low, the
 *            value of an evaluated cell must be smaller than this value to be
 *            considered for re-selection.
 *
 *  @param  threshXHigh
 *          - Cell Srxlev high threshold.
 *          - Range: 0 to 31.
 *          - When the serving cell exceeds thresh_serving_low, the value of an
 *            evaluated cell must be greater than this value to be considered
 *            for re-selection.
 *
 *  @param  cell_resel_priority
 *          - Cell re-selection priority
 *          - Range: 0 to 7.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  cells_len
 *          - Provides the number of set of cell params.
 *
 *  @param  cellInterFreqParams[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_cellParams for more information.
 *
 */
typedef struct
{
    uint16_t       earfcn;
    uint8_t        threshXLow;
    uint8_t        threshXHigh;
    uint8_t        cell_resel_priority;
    uint8_t        cells_len;
    nas_cellParams cellInterFreqParams[NAS_MAX_DESCRIPTION_LENGTH];
} nas_infoInterFreq;

/**
 *  This structure contains information about the LTE Inter-Frequency Network.
 *
 *  @param  ueInIdle
 *          - TRUE if the UE is in Idle mode, otherwise FALSE.
 *              - 0xFF - Not Available
 *
 *  @param  freqsLen
 *          - Provides the number of set of inter frequency information.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  InfoInterfreq[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_infoInterFreq for more information.
 *
 */
typedef struct
{
    uint8_t          ueInIdle;
    uint8_t          freqsLen;
    nas_infoInterFreq InfoInterfreq[NAS_MAX_DESCRIPTION_LENGTH];
} nas_LTEInfoInterfreq;

/**
 *  This structure contains information about the GSM Cell.
 *
 *  @param  arfcn
 *          - GSM frequency being reported.
 *          - Range: 0 to 1023.
 *
 *  @param  band1900
 *          - Band indicator for the GSM ARFCN
 *          - This field is only valid if arfcn is in the overlapping region.
 *          - If TRUE and the cell is in the overlapping region, the ARFCN is
 *            on the 1900 band.
 *          - If FALSE, it is on the 1800 band.
 *
 *  @param  cellIdValid
 *          - Flag indicating whether the base station identity code ID is
 *            valid.
 *
 *  @param  bsicId
 *          - Base station identity code ID, including base station color code
 *            and network color code.
 *          - The lower 6 bits can be set to any value.
 *
 *  @param  rssi
 *          - Measured RSSI value in 1/10 dB.
 *          - Range: -200.0 dB to 0
 *
 *  @param  srxlev
 *          - Cell selection Rx level (Srxlev) value.
 *          - Range: -128 to 128.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 */
typedef struct
{
    uint16_t arfcn;
    uint8_t  band1900;
    uint8_t  cellIdValid;
    uint8_t  bsicId;
    int16_t  rssi;
    int16_t  srxlev;
} nas_gsmCellInfo;

/**
 *  This structure contains information about the LTE GSM Cell.
 *
 *  @param  cellReselPriority
 *          - Priority of this frequency group.
 *          - Range: 0 to 7.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  threshGsmHigh
 *          - Reselection threshold for high priority layers.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  threshGsmLow
 *          - Reselection threshold for low priority layers.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  nccPermitted
 *          - Bitmask specifying whether a neighbor with a specific network
 *            color code is to be reported.
 *          - Range: 0 to 255.
 *          - Bit n set to 1 means a neighbor with NCC n must be included in
 *            the report. This flag is synonymous with a blacklist in other
 *            RATs.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  cells_len
 *          - Provides the number of set of gsm cells.
 *
 *  @param  GsmCellInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_gsmCellInfo for more information.
 *
 */
typedef struct
{
    uint8_t         cellReselPriority;
    uint8_t         threshGsmHigh;
    uint8_t         threshGsmLow;
    uint8_t         nccPermitted;
    uint8_t         cells_len;
    nas_gsmCellInfo GsmCellInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_lteGsmCellInfo;

/**
 *  This structure contains information about the LTE Neighboring GSM Network.
 *
 *  @param  ueInIdle
 *          - TRUE if the UE is in Idle mode, otherwise FALSE.
 *              - 0xFF - Not Available
 *
 *  @param  freqsLen
 *          - Provides the number of set of LTE GSM cell information.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  LteGsmCellInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_lteGsmCellInfo for more information.
 *
 */
typedef struct
{
    uint8_t            ueInIdle;
    uint8_t            freqsLen;
    nas_lteGsmCellInfo LteGsmCellInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_LTEInfoNeighboringGSM;

/**
 *  This structure contains information about the WCDMA Cell.
 *
 *  @param  psc
 *          - Primary scrambling code.
 *          - Range: 0 to 511.
 *
 *  @param  cpich_rscp
 *          - Absolute power level (in 1/10 dBm) of the common pilot channel
 *            as received by the UE.
 *          - Range: -120.0 dBm to -25.0 dBm
 *
 *  @param  cpich_ecno
 *          - CPICH Ec/No; ratio (in 1/10 dB) of the received energy per PN
 *            chip for the CPICH to the total received power spectral density
 *            at the UE antenna connector.
 *          - Range: -50.0 dB to 0.
 *
 *  @param  srxlev
 *          - Cell selection Rx level (Srxlev) value.
 *          - Range: -128 to 128.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 */
typedef struct
{
    uint16_t psc;
    int16_t  cpich_rscp;
    int16_t  cpich_ecno;
    int16_t  srxlev;
} nas_wcdmaCellInfo;

/**
 *  This structure contains information about the LTE WCDMA Cell.
 *
 *  @param  uarfcn
 *          - WCDMA layer frequency.
 *          - Range: 0 to 16383.
 *
 *  @param  cellReselPriority
 *          - Cell re-selection priority.
 *          - Range: 0 to 7.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  threshXhigh
 *          - Re-selection low threshold.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  threshXlow
 *          - Re-selection high threshold.
 *          - Range: 0 to 31.
 *          - This field is only valid when ue_in_idle is TRUE.
 *
 *  @param  cellsLen
 *          - Provides the number of set of WCDMA cells.
 *
 *  @param  WCDMACellInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_wcdmaCellInfo for more information.
 *
 */
typedef struct
{
    uint16_t          uarfcn;
    uint8_t           cellReselPriority;
    uint16_t          threshXhigh;
    uint16_t          threshXlow;
    uint8_t           cellsLen;
    nas_wcdmaCellInfo WCDMACellInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_lteWcdmaCellInfo;

/**
 *  This structure contains information about the LTE Neighboring WCDMA Network.
 *
 *  @param  ueInIdle
 *          - TRUE if the UE is in Idle mode, otherwise FALSE.
 *              - 0xFF - Not Available
 *
 *  @param  freqsLen
 *          - Provides the number of set of LTE WCDMA cell information.
 *          - If 0(zero), then no information follows it.
 *
 *  @param  LTEWCDMACellInfo[NAS_MAX_DESCRIPTION_LENGTH]
 *          - See @ref nas_lteWcdmaCellInfo for more information.
 *
 */
typedef struct
{
    uint8_t             ueInIdle;
    uint8_t             freqsLen;
    nas_lteWcdmaCellInfo LTEWCDMACellInfo[NAS_MAX_DESCRIPTION_LENGTH];
} nas_LTEInfoNeighboringWCDMA;

/**
 *  This structure contains information about the UMTS LTE neighbour Cell.
 *
 *  @param  earfcn
 *          - E-UTRA absolute RF channel number of the detected cell.
 *
 *  @param  pci
 *          - Physical cell ID of the detected cell.
 *          - Range is defined in 3GPP TS 36.211
 *
 *  @param  rsrp
 *          - Current received signal strength indication (in dBm)
 *            of the detected cell.
 *
 *  @param  rsrq
 *          - Current reference signal received quality (in dB) of
 *            the detected cell.
 *
 *  @param  srxlev
 *          - Cell selection Rx level (Srxlev) value of the detected
 *            cell in linear scale.
 *          - This field is only valid when wcdma_rrc_state is not
 *            NAS_WCDMA_RRC_STATE_CEL_FACH or NAS_WCDMA_RRC_STATE_CELL_DCH.
 *
 *  @param  cellIsTDD
 *          - TRUE if the cell is TDD; FALSE if the cell is FDD.
 *
 */
typedef struct
{
    uint16_t  earfcn;
    uint16_t  pci;
    uint32_t  rsrp;
    uint32_t  rsrq;
    int16_t   srxlev;
    uint8_t   cellIsTDD;
} nas_umtsLTENbrCell;

/**
 *  This structure contains information about the WCDMA - LTE Neighboring
 *  Cell Info Set.
 *
 *  @param  wcdmaRRCState
 *          - WCDMA RRC states.
 *          - Defined in 3GPP TS 25.331
 *          - Values:
 *              - 0x00 - NAS_WCDMA_RRC_STATE_DISCONNECTED
 *                         - WCDMA RRC State is IDLE
 *              - 0x01 - NAS_WCDMA_RRC_STATE_CELL_PCH
 *                         - WCDMA RRC state is CELL_PCH
 *              - 0x02 - NAS_WCDMA_RRC_STATE_URA_PCH
 *                         - WCDMA RRC state is URA_PCH
 *              - 0x03 - NAS_WCDMA_RRC_STATE_CELL_FACH
 *                         - WCDMA RRC state is CELL_FACH
 *              - 0x04 - NAS_WCDMA_RRC_STATE_CELL_DCH
 *                         - WCDMA RRC state is CELL_DCH
 *
 *  @param  umtsLTENbrCellLen
 *          - Number of sets of UMTS LTE Neighbors.
 *
 *  @param  UMTSLTENbrCell
 *          - See @ref nas_umtsLTENbrCell for more information.
 *
 */
typedef struct
{
    uint32_t           wcdmaRRCState;
    uint8_t            umtsLTENbrCellLen;
    nas_umtsLTENbrCell UMTSLTENbrCell[NAS_MAX_DESCRIPTION_LENGTH];
} nas_WCDMAInfoLTENeighborCell;

/**
 *  This structure contains information about the Get Cell Location response
 *  parameters.
 *
 *  @param  pGERANInfo
 *          - See @ref nas_GERANInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pUMTSInfo
 *          - See @ref nas_UMTSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCDMAInfo
 *          - See @ref nas_CDMAInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pLTEInfoIntrafreq
 *          - See @ref nas_LTEInfoIntrafreq for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pLTEInfoInterfreq
 *          - See @ref nas_LTEInfoInterfreq for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pLTEInfoNeighboringGSM
 *          - See @ref nas_LTEInfoNeighboringGSM for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pLTEInfoNeighboringWCDMA
 *          - See @ref nas_LTEInfoNeighboringWCDMA for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pUMTSCellID
 *          - Cell ID.
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pWCDMAInfoLTENeighborCell
 *          - See @ref nas_WCDMAInfoLTENeighborCell for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_GERANInfo                *pGERANInfo;
    nas_UMTSInfo                 *pUMTSInfo;
    nas_CDMAInfo                 *pCDMAInfo;
    nas_LTEInfoIntrafreq         *pLTEInfoIntrafreq;
    nas_LTEInfoInterfreq         *pLTEInfoInterfreq;
    nas_LTEInfoNeighboringGSM    *pLTEInfoNeighboringGSM;
    nas_LTEInfoNeighboringWCDMA  *pLTEInfoNeighboringWCDMA;
    uint32_t                     *pUMTSCellID;
    nas_WCDMAInfoLTENeighborCell *pWCDMAInfoLTENeighborCell;
    swi_uint256_t                ParamPresenceMask;
} unpack_nas_SLQSNasGetCellLocationInfo_t;

/**
 * Unpack retrieves cell location-related information.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetCellLocationInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSNasGetCellLocationInfo_t *pOutput
);

/**
 *  This structure contains information about the WCDMA Cell Info Ext.
 *
 *  @param  wAgc
 *          - Power in dB
 *
 *  @param  wTxAgc
 *          - Tx power in dB.
 *
 *  @param  wDlBler
 *          - Downlink block error rate percentage.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    float            wAgc;
    float            wTxAgc;
    uint16_t         wDlBler;
    uint8_t          TlvPresent;
} nas_WCDMACellInfoExt;

/**
 *  This structure contains information about the UMTS Info.
 *
 *  @param  umtsUarfcn
 *          - UTRA absolute RF channel number.
 *
 *  @param  umtsPsc
 *          - Primary scrambling code.
 *
 *  @param  umtsRscp
 *          - Received signal code power
 *          - The received power on one code measured in dBm
 *            on the primary CPICH channel of the neighbor/monitored cell.
 *
 *  @param  umtsEcio
 *          - ECIO
 *          - The received energy per chip divided by the power density
 *            in the band measured in dBm on the primary CPICH channel
 *            of the neighbor/monitored cell.
 *
 *  @param  umtsSqual
 *          - Squal
 *          - cell selection quality value in dB.
 *
 *  @param  umtsSrxLvl
 *          - Srxlev
 *          - cell selection Rx level value in dB.
 *
 *  @param  umtsRank
 *          - Rank of the cell.
 *
 *  @param  umtsSet
 *          - Set of the cell.
 *
 */
typedef struct
{
    uint16_t         umtsUarfcn;
    uint16_t         umtsPsc;
    int16_t          umtsRscp;
    int16_t          umtsEcio;
    int16_t          umtsSqual;
    int16_t          umtsSrxLvl;
    int16_t          umtsRank;
    uint8_t          umtsSet;
} nas_umtsInstArr;


/**
 *  This structure contains information about the GERAN Info.
 *
 *  @param  geranArfcn
 *          - Absolute RF channel number.
 *
 *  @param  geranBsicNcc
 *          - Base station identity code network color code
 *          - 0xFF indicates information is not present.
 *
 *  @param  geranBsicBcc
 *          - Base station identity code base station color code
 *          - 0xFF indicates information is not present.
 *
 *  @param  geranRssi
 *          - Received signal strength indicator.
 *
 *  @param  geranRank
 *          - Rank of the cell.
 *
 */
typedef struct
{
    uint16_t           geranArfcn;
    uint8_t            geranBsicNcc;
    uint8_t            geranBsicBcc;
    int16_t            geranRssi;
    int16_t            geranRank;
} nas_geranInstArr;

/**
 *  This structure contains information about the UMTS Extended Info.
 *
 *  @param  cellId
 *          - Cell ID
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *
 *  @param  plmn
 *          - MCC/MNC information
 *
 *  @param  lac
 *          - Location area code.
 *
 *  @param  uarfcn
 *          - UTRA absolute RF channel number.
 *
 *  @param  psc
 *          - Primary scrambling code.
 *
 *  @param  rscp
 *          - Received signal code power
 *          - The received power on one code measured in dBm on the
 *            primary CPICH channel of the serving cell.
 *
 *  @param  ecio
 *          - The received energy per chip divided by the power density in the
 *            band measured in dBm on the primary CPICH channel of the serving cell.
 *
 *  @param  squal
 *          - Cell selection quality value in dB.
 *
 *  @param  srxLvl
 *          - Cell selection Rx level value in dB.
 *
 *  @param  umtsInst
 *          - Number of sets of the umtsInstElement
 *
 *  @param  umtsInstElement
 *          - See @ref nas_umtsInstArr for more information.
 *
 *  @param  geranInst
 *          - Number of sets of the geranInstElement
 *
 *  @param  geranInstElement
 *          - See @ref nas_geranInstArr for more information.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint16_t         cellId;
    uint8_t          plmn[NAS_PLMN_LENGTH];
    uint16_t         lac;
    uint16_t         uarfcn;
    uint16_t         psc;
    int16_t          rscp;
    int16_t          ecio;
    int16_t          squal;
    int16_t          srxLvl;
    uint8_t          umtsInst;
    nas_umtsInstArr  umtsInstElement[NAS_MAX_DESCRIPTION_LENGTH];
    uint8_t          geranInst;
    nas_geranInstArr geranInstElement[NAS_MAX_DESCRIPTION_LENGTH];
    uint8_t          TlvPresent;
} nas_UMTSExtInfo;

/**
 *  This structure contains information about the LTE EARFCN Info.
 *
 *  @param  lteInterEarfcnlen
 *          - Number of sets of the lteEarfcn
 *
 *  @param  lteEarfcn
 *          - LTE neighbor cell information EARFCN.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint8_t           lteInterEarfcnlen;
    uint32_t          lteEarfcn[NAS_MAX_DESCRIPTION_LENGTH];
    uint8_t           TlvPresent;
} nas_LteEarfcnInfo;

/**
 *  This structure contains information about the Get Cell Location response
 *  parameters.
 *
 *  @param  pGERANInfo
 *          - See @ref nas_GERANInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pUMTSInfo
 *          - See @ref nas_UMTSInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pCDMAInfo
 *          - See @ref nas_CDMAInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pLTEInfoIntrafreq
 *          - See @ref nas_LTEInfoIntrafreq for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pLTEInfoInterfreq
 *          - See @ref nas_LTEInfoInterfreq for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pLTEInfoNeighboringGSM
 *          - See @ref nas_LTEInfoNeighboringGSM for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pLTEInfoNeighboringWCDMA
 *          - See @ref nas_LTEInfoNeighboringWCDMA for more information.
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pUMTSCellID
 *          - Cell ID.
 *          - 0xFFFFFFFF indicates cell ID information is not present.
 *          - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *  @param  pWCDMAInfoLTENeighborCell
 *          - See @ref nas_WCDMAInfoLTENeighborCell for more information.
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pWCDMACellInfoExt
 *          - See @ref nas_WCDMACellInfoExt for more information.
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pUMTSExtInfo
 *          - See @ref nas_UMTSExtInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pLteEarfcnInfo
 *          - See @ref na_LteEarfcnInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_GERANInfo                *pGERANInfo;
    nas_UMTSInfo                 *pUMTSInfo;
    nas_CDMAInfo                 *pCDMAInfo;
    nas_LTEInfoIntrafreq         *pLTEInfoIntrafreq;
    nas_LTEInfoInterfreq         *pLTEInfoInterfreq;
    nas_LTEInfoNeighboringGSM    *pLTEInfoNeighboringGSM;
    nas_LTEInfoNeighboringWCDMA  *pLTEInfoNeighboringWCDMA;
    uint32_t                     *pUMTSCellID;
    nas_WCDMAInfoLTENeighborCell *pWCDMAInfoLTENeighborCell;
    nas_WCDMACellInfoExt         *pWCDMACellInfoExt;
    nas_UMTSExtInfo              *pUMTSExtInfo;
    nas_LteEarfcnInfo            *pLteEarfcnInfo;
    swi_uint256_t                ParamPresenceMask;
} unpack_nas_SLQSNasGetCellLocationInfoV2_t;

/**
 * Unpack retrieves cell location-related information.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetCellLocationInfoV2(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSNasGetCellLocationInfoV2_t *pOutput
);

/**
 * Pack retrieves cell location-related information.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetCellLocationInfoV2(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Pack Get Network Time.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetNetworkTime(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );
/**
 * This structure contains the parameters for Network Time.
 *
 *  @param  year
 *          - Year
 *
 *  @param  month
 *          - Month
 *          - 1 is January and 12 is December
 *
 *  @param  day
 *          - Day
 *          - Range - 1 to 31
 *
 *  @param  hour
 *          - Hour
 *          - Range - 0 to 59
 *
 *  @param  minute
 *          - Minute
 *          - Range - 0 to 59
 *
 *  @param  second
 *          - Second
 *          - Range - 0 to 59
 *
 *  @param  dayOfWeek
 *          - Day of the week
 *          - 0 is Monday and 6 is Sunday
 *
 *  @param  timeZone
 *          - Offset from Universal time
 *          - The difference between local time and Universal time,
 *            in increments of 15 min
 *          - Signed Value
 *
 *  @param  dayLtSavingAdj
 *          - Daylight saving adjustment in hours
 *          - Possible values - 0, 1, and 2.
 *          - This field is ignored if radio_if is NAS_RADIO_IF_CDMA_1XEVDO
 *
 *  @param  radioInterface
 *          - Radio interface from which the information comes
 *          - Values
 *            - 0x01 - NAS_RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - NAS_RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x04 - NAS_RADIO_IF_GSM - GSM
 *            - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *            - 0x08 - NAS_RADIO_IF_LTE - LTE
 *            - 0x09 - NAS_RADIO_IF_TDSCDMA -TD-SCDMA
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 */
typedef struct{
    uint16_t   year;
    uint8_t    month;
    uint8_t    day;
    uint8_t    hour;
    uint8_t    minute;
    uint8_t    second;
    uint8_t    dayOfWeek;
    int8_t     timeZone;
    uint8_t    dayLtSavingAdj;
    uint8_t    radioInterface;
    uint8_t    TlvPresent;
}nas_timeInfo;

/**
 *  This structure contains information about the GetNetworkTime response
 *  parameters.
 *
 *  \param  p3GPP2TimeInfo [Optional]
 *          - See @ref nas_timeInfo for more information
+ *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  p3GPPTimeInfo [Optional]
 *          - See @ref nas_timeInfo for more information
+ *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_timeInfo                *p3GPP2TimeInfo;
    nas_timeInfo                *p3GPPTimeInfo;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSGetNetworkTime_t;

/**
 * Unpack get network time.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetNetworkTime(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSGetNetworkTime_t *pOutput
);
/**
 * This structure contains the parameters for Universal Time Information.
 *
 *  @param  year
 *          - Year.
 *
 *  @param  month
 *          - Month.
 *              - 1 is January and 12 is December.
 *
 *  @param  day
 *          - Day.
 *              - Range 1 to 31.
 *
 *  @param  hour
 *          - Hour.
 *              - Range 0 to 59.
 *
 *  @param  minute
 *          - Minute.
 *              - Range 0 to 59.
 *
 *  @param  second
 *          - Second.
 *              - Range 0 to 59.
 *
 *  @param  dayOfWeek
 *          - Day of the Week.
 *              - 0 is Monday and 6 is Sunday.
 *
 */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
} nas_UniversalTime;

/**
 *  Structure for storing the NAS Network Time indication parameters.
 *
 *  @param  universalTime
 *          - See \ref nas_UniversalTime for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pTimeZone
 *          - Time Zone.
 *          - Offset from Universal time, i.e., the difference between local
 *            time and Universal time, in increments of 15 min (signed value).
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pDayltSavAdj
 *          - Daylight Saving Adjustment.
 *          - Daylight saving adjustment in hr.
 *              - Possible values: 0, 1, and 2.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pRadioInterface
 *          - Radio interface from which the information comes
 *          - Values
 *            - 0x01 - NAS_RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - NAS_RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x04 - NAS_RADIO_IF_GSM - GSM
 *            - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *            - 0x08 - NAS_RADIO_IF_LTE - LTE
 *            - 0x09 - NAS_RADIO_IF_TDSCDMA -TD-SCDMA
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_UniversalTime  universalTime;
    uint8_t              *pTimeZone;
    uint8_t              *pDayltSavAdj;
    uint8_t              *pRadioInterface;
    swi_uint256_t        ParamPresenceMask;
} unpack_nas_SLQSNasNetworkTimeCallBack_ind_t;

/**
 * Unpack Network time callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     sig info indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasNetworkTimeCallBack_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasNetworkTimeCallBack_ind_t *pOutput
        );

/*
 * NAS LTE CPHY Downlink Values.
 */
typedef enum {
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_6=0x00,
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_15=0x01,
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_25=0x02,
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_50=0x03,
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_75=0x04,
    eNAS_LTE_CPHY_CA_BW_NRB_LITE_100=0x05,
}NAS_LTE_CPHY_CA_BW_NRB_LITE;

/*
 * NAS LTE CPHY Scell States.
 */
typedef enum {
    eNAS_LTE_CPHY_SCELL_STATE_DECONFIGURED_LITE=0x00,
    eNAS_LTE_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED_LITE =0x01,
    eNAS_LTE_CPHY_SCELL_STATE_CONFIGURED_ACTIVATED_LITE=0x02,
}NAS_LTE_CPHY_SCELL_STATE_LITE;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Indicator Type.
 *
 *  @param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  @param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  @param  scell_state
 *          - Scell state Values.
 *          - See @ref NAS_LTE_CPHY_SCELL_STATE_LITE for more information.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */

typedef struct{
    int       pci;
    int       freq;
    NAS_LTE_CPHY_SCELL_STATE_LITE scell_state;
    uint8_t   TlvPresent;
}nas_PhyCaAggScellIndType;
/**
 * This structure contains the parameters for Physical Carrier
 * aggregation Downlink Bandwidth of Scell.
 *
 *  @param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See @ref NAS_LTE_CPHY_CA_BW_NRB_LITE for more information.
 *
 */
typedef struct{
    NAS_LTE_CPHY_CA_BW_NRB_LITE dl_bw_value;
    uint8_t TlvPresent;
}nas_PhyCaAggScellDlBw;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Information.
 *
 *  @param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  @param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  @param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See @ref NAS_LTE_CPHY_CA_BW_NRB_LITE for more information.
 *
 *  @param  iLTEbandValue
 *          - Band value.
 *          - Range for LTE Band class 120 to 160.
 *              - 120 - LTE E-UTRA Operating Band 1
 *              - 121 - LTE E-UTRA Operating Band 2
 *              - 122 - LTE E-UTRA Operating Band 3
 *              - 123 - LTE E-UTRA Operating Band 4
 *              - 124 - LTE E-UTRA Operating Band 5
 *              - 125 - LTE E-UTRA Operating Band 6
 *              - 126 - LTE E-UTRA Operating Band 7
 *              - 127 - LTE E-UTRA Operating Band 8
 *              - 128 - LTE E-UTRA Operating Band 9
 *              - 129 - LTE E-UTRA Operating Band 10
 *              - 130 - LTE E-UTRA Operating Band 11
 *              - 131 - LTE E-UTRA Operating Band 12
 *              - 132 - LTE E-UTRA Operating Band 13
 *              - 133 - LTE E-UTRA Operating Band 14
 *              - 134 - LTE E-UTRA Operating Band 17
 *              - 135 - LTE E-UTRA Operating Band 33
 *              - 136 - LTE E-UTRA Operating Band 34
 *              - 137 - LTE E-UTRA Operating Band 35
 *              - 138 - LTE E-UTRA Operating Band 36
 *              - 139 - LTE E-UTRA Operating Band 37
 *              - 140 - LTE E-UTRA Operating Band 38
 *              - 141 - LTE E-UTRA Operating Band 39
 *              - 142 - LTE E-UTRA Operating Band 40
 *              - 143 - LTE E-UTRA Operating Band 18
 *              - 144 - LTE E-UTRA Operating Band 19
 *              - 145 - LTE E-UTRA Operating Band 20
 *              - 146 - LTE E-UTRA Operating Band 21
 *              - 147 - LTE E-UTRA Operating Band 24
 *              - 148 - LTE E-UTRA Operating Band 25
 *              - 149 - LTE E-UTRA Operating Band 41
 *              - 150 - LTE E-UTRA Operating Band 42
 *              - 151 - LTE E-UTRA Operating Band 43
 *              - 152 - LTE E-UTRA Operating Band 23
 *              - 153 - LTE E-UTRA Operating Band 26
 *              - 154 - LTE E-UTRA Operating Band 32
 *              - 155 - LTE E-UTRA Operating Band 125
 *              - 156 - LTE E-UTRA Operating Band 126
 *              - 157 - LTE E-UTRA Operating Band 127
 *              - 158 - LTE E-UTRA Operating Band 28
 *              - 159 - LTE E-UTRA Operating Band 29
 *              - 160 - LTE E-UTRA Operating Band 30
 *
 *
 *  @param  scell_state
 *          - Scell state Values.
 *          - See @ref NAS_LTE_CPHY_SCELL_STATE_LITE for more information.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    int pci;
    int freq;
    NAS_LTE_CPHY_CA_BW_NRB_LITE dl_bw_value;
    uint16_t iLTEbandValue;
    NAS_LTE_CPHY_SCELL_STATE_LITE scell_state;
    uint8_t TlvPresent;
}nas_PhyCaAggScellInfo;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Pcell Information.
 *
 *  @param  pci
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  @param  freq
 *          - Frequency of the absolute cell Range.
 *          - Range for ID values: 0 to 65535.
 *
 *  @param  dl_bw_value
 *          - Downlink Bandwidth Values.
 *          - See @ref NAS_LTE_CPHY_CA_BW_NRB_LITE for more information.
 *
 *  @param  scell_state
 *          - Scell state Values.
 *          - See @ref NAS_LTE_CPHY_SCELL_STATE_LITE for more information.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    int    pci;
    int    freq;
    NAS_LTE_CPHY_CA_BW_NRB_LITE dl_bw_value;
    int    iLTEbandValue;
    uint8_t TlvPresent;
}nas_PhyCaAggPcellInfo;

/**
 * This structure contains the parameters for Physical Carrier
 * aggregation of Scell Index.
 *
 *  @param  scell_idx
 *          - Physical cell ID of the SCell Range.
 *          - Range for ID values: 0 to 503.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t scell_idx;
    uint8_t TlvPresent;
}nas_PhyCaAggScellIndex;

/**
 * This structure contains the parameters for Aggregated DL Bandwidth.
 *
 *  \param  aggDlBW
 *          - Aggregated DL bandwidth.
 *          - Units: MHz
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint16_t aggDlBW;
    uint8_t TlvPresent;
}nas_PhyCaAggDlBW;

/**
 * This structure contains the parameters for Number of Scells Configured
 *
 *  \param  numScellsCfg
 *          - Number of Scells Configured.
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint16_t numScellsCfg;
    uint8_t TlvPresent;
}nas_NumScellsConfig;

/**
 *  Structure for storing the LTE PHY CA indication parameters.
 *
 *  @param  pPhyCaAggScellIndType
 *          - See @ref nas_PhyCaAggScellIndType for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  sPhyCaAggScellDlBw
 *          - See @ref nas_PhyCaAggScellDlBw for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  sPhyCaAggScellInfo
 *          - See @ref nas_PhyCaAggScellInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  sPhyCaAggPcellInfo
 *          - See @ref nas_PhyCaAggPcellInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  sPhyCaAggScellIndex
 *          - See @ref nas_PhyCaAggScellIndex for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  sPhyCaAggDlBW
 *          - See @ref nas_PhyCaAggDlBW for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  sNumScellsConfig
 *          - See @ref nas_NumScellsConfig for more information.
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_PhyCaAggScellIndType    sPhyCaAggScellIndType;
    nas_PhyCaAggScellDlBw       sPhyCaAggScellDlBw;
    nas_PhyCaAggScellInfo       sPhyCaAggScellInfo;
    nas_PhyCaAggPcellInfo       sPhyCaAggPcellInfo;
    nas_PhyCaAggScellIndex      sPhyCaAggScellIndex;
    nas_PhyCaAggDlBW            sPhyCaAggDlBW;
    nas_NumScellsConfig         sNumScellsConfig;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SetNasLTECphyCaIndCallback_ind_t;

/**
 * Unpack set LTE PHY CA indication callback. 
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     sig info indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetNasLTECphyCaIndCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetNasLTECphyCaIndCallback_ind_t *pOutput
        );

/**
 * This structure contains the parameters for Rx Signal Info.
 *
 *  @param  rxChainIndex
 *          - Rx antenna path
 *          - Valid Values
 *            - 0 - Primary Rx
 *            - 1 - Diversity Rx
 *
 *  @param  isRadioTuned
 *          - Rx path is tuned to a channel or Not
 *          - Values
 *            - 0x00 - Not tuned
 *            - 0x01 - Tuned
 *            @note   If the radio is tuned, the instantaneous values are set for the fields below.
 *                    If the radio is not tuned, the values set below may be invalid.
 *
 *  @param  rxPower
 *          - Rx power value in 1/10 dBm resolution
 *
 *  @param  rsrp
 *          - Current reference signal received power in 1/10 dBm resolution
 *
 */
typedef struct{
    uint8_t      rxChainIndex;
    uint8_t      isRadioTuned;
    int32_t      rxPower;
    int32_t      rsrp;
}nas_RxSigInfo;

/**
 *  This structure contains information about the SccRxInfo parameters.
 *
 *  @param  rsrq
 *          - Current reference signal
 *          - Receive quality in 1/10 dB resolution
 *
 *  @param  snr
 *          - Reference signal signal-to-noise ratio in dB.
 *          - Range -10 to 30
 *
 *  @param  numInstances
 *          - Number of sets of the following
 *            - rxChainIndex
 *            - isRadioTuned
 *            - rxPower
 *            - rsrp
 *
 *  @param  sigInfo
 *          - See \ref nas_RxSigInfo for more information
 *
 *  \param  TlvPresent
 *          - Tlv Present.
 *
 */
typedef struct
{
    int32_t         rsrq;
    int16_t         snr;
    uint8_t          numInstances;
    nas_RxSigInfo    sigInfo[NAS_MAX_SCC_RX_INFO_INSTANCES];
    uint8_t          TlvPresent;
}nas_SccRxInfo;


/**
 * get LTE Secondary carrier Rx signal level information pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReq qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiGetLteSccRxInfo(
        pack_qmi_t *pCtx,
        uint8_t *pReq,
        uint16_t *pLen
        );

/**
 * This structure contains unpack get LTE Secondary carrier Rx signal level information parameters.
 * @param pSccRxInfo Secondary carrier Rx signal level info
 *     - See \ref nas_SccRxInfo
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct {
    nas_SccRxInfo *pSccRxInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSSwiGetLteSccRxInfo_t;

/**
 * get LTE Secondary carrier Rx signal level information unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiGetLteSccRxInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiGetLteSccRxInfo_t *pOutput
        );

/**
 *  Structure for Network Timer indication parameters.
 *
 *  @param  t3396_apn
 *          - apn
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  t3396_plmn_id
 *          - plmn id
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  t3396_val
 *          - timer value
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    char t3396_apn[101];
    uint8_t t3396_plmn_id[3];
    uint32_t t3396_val;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasTimerCallback_ind_t;

/**
 * Unpack timer callback indication.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     network timer indication unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasTimerCallback_ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasTimerCallback_ind_t *pOutput
        );

/**
 *  Pack structure for Initiates a domain attach/detach of the device.
 *
 *  @param  action
 *          - Domain action to attempt\n
 *          1 - Attach\n
 *          2 - Detach
 *
 */
typedef struct{
        uint32_t action;
} pack_nas_InitiateDomainAttach_t;

typedef unpack_result_t  unpack_nas_InitiateDomainAttach_t;

/**
 * Initiates a domain attach/detach of the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_InitiateDomainAttach(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_InitiateDomainAttach_t *pReqParam
        );

/**
 * Initiates a domain attach/detach of the device unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_InitiateDomainAttach(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_InitiateDomainAttach_t *pOutput
        );

/**
 * Pack structure to set CDMA Network parameters.
 *
 *  @param  pSPC
 *          - Six digit service programming code (not necessary
 *            when only the roaming field is being set)
 *
 *  @param  pForceRev0
 *          - (Optional)Force CDMA 1x-EV-DO Rev. 0 mode\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *              Note: Enabled can only be specified if pCustomSCP
 *                    state is set to Disabled
 *
 *  @param  pCustomSCP
 *          - (Optional)Use a custom config for CDMA 1x-EV-DO SCP\n
 *            - 0 - Disabled
 *            - 1 - Enabled
 *              Note: Enabled can only be specified if pForceRev0
 *                    is set to Disabled
 *
 *  @param  pProtocol
 *          - Protocol mask for custom SCP config\n
 *            - 0x00000001 - Subtype 2 Physical Layer
 *            - 0x00000002 - Enhanced CCMAC
 *            - 0x00000004 - Enhanced ACMAC
 *            - 0x00000008 - Enhanced FTCMAC
 *            - 0x00000010 - Subtype 3 RTCMAC
 *            - 0x00000020 - Subsystem 1 RTCMAC
 *            - 0x00000040 - Enhanced Idle
 *            - 0x00000080 - Generic Multimode Capable Disc Port
 *            - 0xFFFFFFFF - Unknown\n
 *
 *  @param  pBroadcast
 *          - Broadcast mask for custom SCP config
 *              - 0x00000001 - Generic broadcast enabled
 *              - 0xFFFFFFFF - Unknown
 *
 *  @param  pApplication
 *          - Application mask for custom SCP config\n
 *            - 0x00000001 - SN Multiflow Packet Application
 *            - 0x00000002 - SN Enhanced Multiflow Packet Application
 *            - 0xFFFFFFFF - Unknown
 *
 *  @param  pRoaming
 *          - Roaming preference\n
 *            - 0 - Automatic
 *            - 1 - Home Only
 *            - 2 - Affiliated Roaming Only
 *            - 3 - Home and Affiliated Roaming
 *            - 0xFFFFFFFF - Unknown
 *
 */
typedef struct{
  char     *pSPC;
  uint8_t  *pForceRev0;
  uint8_t  *pCustomSCP;
  uint32_t *pProtocol;
  uint32_t *pBroadcast;
  uint32_t *pApplication;
  uint32_t *pRoaming;
} pack_nas_SetCDMANetworkParameters_t;

typedef unpack_result_t  unpack_nas_SetCDMANetworkParameters_t;

/**
 * CDMA Network Parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SetCDMANetworkParameters(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SetCDMANetworkParameters_t *pReqParam
        );

/**
 * CDMA Network Parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SetCDMANetworkParameters(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SetCDMANetworkParameters_t *pOutput
        );

/**
 *  Structure for storing the current preferred system selection settings
 *  for the device.
 *
 *  @param  pColorCode [Optional]
 *          - Color code value
 *          - Color code corresponding to the sector to which the AT is
 *            sending the access probe
 *          - See 3GPP2 C.S0024-B V3.0, Section 7.11.6.2.1 for more information.
 *              - 0xFF - Not Available
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t *pColorCode;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasGetHDRColorCode_t;

/**
 * HDR Color Code Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetHDRColorCode(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * HDR Color Code Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetHDRColorCode(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasGetHDRColorCode_t *pOutput
        );

/**
 * Structure for TxRx Info pack
 *
 *  @param  radio_if [Mandatory]
 *          - Radio interface technology of the signal being measured
 *          - Valid Values
 *              - 0x01 - NAS_RADIO_IF_CDMA_1X - CDMA
 *              - 0x02 - NAS_RADIO_IF_CDMA_1XEVDO - HDR
 *              - 0x04 - NAS_RADIO_IF_GSM - GSM
 *              - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *              - 0x08 - NAS_RADIO_IF_LTE - LTE
 */
typedef struct
{
    uint8_t radio_if;
} pack_nas_SLQSNasGetTxRxInfo_t;

/**
 *  This structure contains rx radio infomation.
 *  @param  isRadioTuned
 *          - Whether Rx is tuned to a channel:
 *            - 0x00 - Not tuned
 *            - 0x01 - Tuned
 *            - 0xFF - Not Available
 *          - If the radio is tuned, instantaneous values are set for the
 *            signal information fields below.
 *          - If the radio is not tuned, or is delayed or invalid, the values
 *            are set depending on each technology.
 *
 *  @param  rx_pwr
 *          - Rx power value in 1/10 dbm resolution.
 *
 *  @param  ecio
 *          - ECIO in 1/10 dB; valid for CDMA, HDR, GSM, WCDMA, and LTE.
 *
 *  @param  rscp
 *          - Received signal code power in 1/10 dbm.
 *          - Valid for WCDMA.
 *
 *  @param  rsrp
 *          - Current reference signal received power in 1/10 dbm valid for LTE.
 *
 *  @param  phase
 *          - Phase in 1/100 degrees; valid for LTE.
 *          - When the phase is unknown, 0xFFFFFFFF is used.
 *
 */
typedef struct
{
    uint8_t isRadioTuned;
    int32_t rxPower;
    int32_t ecio;
    int32_t rscp;
    int32_t rsrp;
    uint32_t phase;
} nas_rxInfo;

/**
 *  This structure contains tx radio infomation.
 *  @param  isInTraffic
 *          - Whether the device is in traffic.
 *            - 0x00 - not in traffic
 *            - 0x01 - in traffic
 *          - The txPower field is only meaningful when in the device is in
 *            traffic.
 *
 *  @param  txPower
 *          - Tx power value in 1/10 dbm.
 *
 */
typedef struct
{
   uint8_t isInTraffic;
   int32_t txPower;
} nas_txInfo;

/**
 * Structure for TxRx Info unpack
 *
 *  @param  pRXChain0Info [Optional]
 *          - See @ref nas_rxInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pRXChain1Info [Optional]
 *          - See @ref nas_rxInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pTXInfo [Optional]
 *          - See @ref nas_txInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_rxInfo *pRXChain0Info;
    nas_rxInfo *pRXChain1Info;
    nas_txInfo *pTXInfo;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNasGetTxRxInfo_t;

/**
 * Get Tx Rx Info Parameters pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetTxRxInfo(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasGetTxRxInfo_t *reqArg
        );

/**
 * Get Tx Rx Info Parameters unpack
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetTxRxInfo(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSNasGetTxRxInfo_t *pOutput
);

/**
 *  This structure contains Operator PLMN.
 *  @param  mcc
 *          - MCC in ASCII string (a value of D in any of the digits is to
 *            be used to indicate a "wild" value for that corresponding
 *            digit).
 *
 *  @param  mnc
 *          - MNC in ASCII string (a value of D in any of the digits is to
 *            be used to indicate a "wild" value for that corresponding
 *            digit; digit 3 in MNC is optional and when not present, will
 *            be set as ASCII F).
 *
 *  @param  lac1
 *          - Location area code 1.
 *
 *  @param  lac2
 *          - Location area code 1.
 *
 *  @param  PLMNRecID
 *          - PLMN network name record identifier.
 *
 */
typedef struct
{
    uint8_t mcc[NAS_PLMN_LENGTH];
    uint8_t mnc[NAS_PLMN_LENGTH];
    uint16_t lac1;
    uint16_t lac2;
    uint8_t PLMNRecID;
} nas_OperatorPLMNData;

/**
 *  This structure contains Operator PLMN List.
 *  @param  numInstance
 *          - Number of sets of the elements.
 *
 *  @param  PLMNData
 *          - Refer OperatorPLMNLData for details (Optional).
 */
typedef struct
{
    uint16_t numInstance;
    nas_OperatorPLMNData PLMNData[NAS_MAX_DESCRIPTION_LENGTH];
} nas_operatorPLMNList;

/**
 *  This structure contains service provider name.
 *  @param  displayCondition
 *          - Display condition.
 *
 *  @param  spnLength
 *          - It provides length of spn.
 *
 *  @param  spn
 *          - Service provider name string must use:
 *            The SMS default 7-bit coded alphabet as defined in 3GPP TS 23.038
 *            with bit 8 set to 9.
 */
typedef struct
{
    uint8_t displayCondition;
    uint8_t spnLength;
    uint8_t spn[NAS_MAX_DESCRIPTION_LENGTH];
} nas_serviceProviderName;

/**
 *  This structure contains NITZ Information.
 *  @param  codingScheme
 *          - Coding scheme:
 *              - 0 - CODING_SCHEME_CELL_BROADCAST_GSM -
 *                  Cell broadcast data coding scheme, GSM default alphabet,
 *                  language unspecified;defined in 3GPP TS 23.038.
 *              - 1 - CODING_SCHEME_UCS2 - UCS2 (16 bit);defined in ISO/IEC 10646
 *
 *  @param  countryInitials
 *          - Country's initials:
 *              - 0 - COUNTRY_INITIALS_DO_NOT_ADD - MS
 *                  should not add the letters for the country's initials to the
 *                  text string.
 *              - 1 - COUNTRY_INITIALS_ADD - MS should add the
 *                  letters for the country's initials and a separator, e.g., a
 *                  space, to the text string.
 *
 *  @param  longNameSpareBits
 *          - Long Name Spare Bits:
 *              - 1 - SPARE_BITS_8 - Bit 8 is spare and set to 0 in octet n
 *              - 2 - SPARE_BITS_7_TO_8 - Bits 7 and 8 are spare and
 *                  set to 0 in octet n.
 *              - 3 - SPARE_BITS_6_TO_8 - Bits 6 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 4 - SPARE_BITS_5_TO_8 - Bits 5 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 5 - SPARE_BITS_4_TO_8 - Bits 4 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 6 - SPARE_BITS_3_TO_8 - Bits 3 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 7 - SPARE_BITS_2_TO_8 - Bits 2 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 0 - SPARE_BITS_UNKNOWN - Carries no information
 *                  about the number of spare bits in octet n.
 *
 *  @param  shortNameSpareBits
 *          - Short Name Spare Bits:
 *              - 1 - SPARE_BITS_8 - Bit 8 is spare and set to 0 in octet n.
 *              - 2 - SPARE_BITS_7_TO_8 - Bits 7 and 8 are spare and
 *                  set to 0 in octet n.
 *              - 3 - SPARE_BITS_6_TO_8 - Bits 6 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 4 - SPARE_BITS_5_TO_8 - Bits 5 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 5 - SPARE_BITS_4_TO_8 - Bits 4 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 6 - SPARE_BITS_3_TO_8 - Bits 3 to 8 (inclusive) are
 *                  spare and set to 0 in octet n.
 *              - 7 - SPARE_BITS_2_TO_8 - Bits 2 to 8 (inclusive) are spare and set
 *                  to 0 in octet n.
 *              - 0 - SPARE_BITS_UNKNOWN - Carries no information
 *                  about the number of spare bits in octet n.
 *
 *  @param  longNameLen
 *          - It provides the length of long name.
 *
 *  @param  longName
 *          - Long name string in coding_scheme.
 *
 *  @param  shortNameLen
 *          - It provides the length of short name.
 *
 *  @param  shortName
 *          - Short name string in coding_scheme.
 */
typedef struct
{
    uint8_t codingScheme;
    uint8_t countryInitials;
    uint8_t longNameSpareBits;
    uint8_t shortNameSpareBits;
    uint8_t longNameLen;
    uint8_t longName[NAS_MAX_DESCRIPTION_LENGTH];
    uint8_t shortNameLen;
    uint8_t shortName[NAS_MAX_DESCRIPTION_LENGTH];
} nas_PLMNNetworkNameData;

/**
 *  This structure contains NITZ Information list.
 *  @param  numInstance
 *          - Number of sets of the elements.
 *
 *  @param  PLMNNetName
 *          - Refer nas_PLMNNetworkNameData for details (Optional).
 */
typedef struct
{
    uint8_t numInstance;
    nas_PLMNNetworkNameData PLMNNetName[NAS_MAX_DESCRIPTION_LENGTH];
} nas_PLMNNetworkName;

/**
 * This structure contains PLMN name.
 * @param  PLMNName
 *          - PLMN name must be coded in a default 7-bit alphabet with b8
 *            set to 0.
 */
typedef struct
{
    uint8_t PLMNName[NAS_MAX_DESCRIPTION_LENGTH];
} nas_operatorNameString;

/**
 * Structure for Operator Name unpack.
 *
 *  @param  pSrvcProviderName
 *          - Refer nas_serviceProviderName for details (Optional).
 *          - Can provide NULL if this parameter is not required.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pOperatorPLMNList
 *          - Refer nas_operatorPLMNList for details (Optional).
 *          - Can provide NULL if this parameter is not required.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pPLMNNetworkName
 *          - Refer nas_PLMNNetworkName for details (Optional).
 *          - Can provide NULL if this parameter is not required.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pOperatorNameString
 *          - Refer nas_operatorNameString for details (Optional).
 *          - Can provide NULL if this parameter is not required.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pNITZInformation
 *          - Refer nas_PLMNNetworkNameData for details (Optional).
 *          - Can provide NULL if this parameter is not required.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_serviceProviderName *pSrvcProviderName;
    nas_operatorPLMNList    *pOperatorPLMNList;
    nas_PLMNNetworkName     *pPLMNNetworkName;
    nas_operatorNameString  *pOperatorNameString;
    nas_PLMNNetworkNameData *pNITZInformation;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSGetOperatorNameData_t;

/**
 * Operator Name Data Parameters pack
 *
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetOperatorNameData(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Operator Name Data Parameters unpack
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetOperatorNameData(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetOperatorNameData_t *pOutput
        );

/**
 * Structure for 3GPP2 Subscription pack.
 *
 *  @param  namID [Mandatory]
 *          - NAM ID of the information to be retrieved. The index
 *            starts from 0. A nam_id of 0xFF is used to retrieve
 *            information of current NAM.
 */
typedef struct
{
    uint8_t namID;
} pack_nas_SLQSNasGet3GPP2Subscription_t;

/**
 *  This structure contains NAM Name.
 *  @param  namNameLen
 *          - Number of sets of the following elements:
 *              - nam_name
 *          - If zero(0), then no information follows.
 *
 *  @param  namName
 *          - Name information in ASCII. The maximum length of
 *            nam_name is 12.
 *
 */
typedef struct
{
    uint8_t namNameLen;
    uint8_t namName[NAS_NAM_NAME_LENGTH];
} nas_namName;

/**
 * This structure contains the parameters for Directory Number Information
 *
 *  @param  dirNumLen
 *          - Number of sets of the following elements:
 *              - dir_num
 *          - If zero(0), then no information follows.
 *
 *  @param  dirNum
 *          - Directory number in ASCII characters.
 *
 */
typedef struct
{
    uint8_t dirNumLen;
    uint8_t dirNum [NAS_MAX_DESCRIPTION_LENGTH];
} nas_dirNum;

/**
 * This structure contains the parameters for SidNid Information
 *
 *  @param  nid
 *          - Network ID
 *
 *  @param  sid
 *          - System ID
 *
 */
typedef struct
{
    uint16_t nid;
    uint16_t sid;
} nas_sidNid;

/**
 * This structure contains Home SID/NID
 *  @param  numInstances
 *          - Number of sets of the following elements:
 *              - sid
 *              - nid
 *          - If zero(0), then no information follows.
 *
 *  @param  SidNid
 *          - See @ref nas_sidNid for more information
 *
 */
typedef struct
{
    uint8_t   numInstances;
    nas_sidNid SidNid[NAS_MAX_DESCRIPTION_LENGTH];
} nas_homeSIDNID;

/**
 * This structure contains MIN-based IMSI.
 *  @param  mccM
 *          - ASCII character representation of MCC_M
 *
 *  @param  imsiM1112
 *          - ASCII character representation of IMSI_M_11_12 value
 *              - 0xFFFF - Not Available
 *
 *  @param  imsiMS1
 *          - ASCII character representation of IMSI_M_S1 value
 *
 *  @param  imsiMS2
 *          - ASCII character representation of IMSI_M_S2 value
 *
 */
typedef struct
{
    uint8_t  mccM[NAS_PLMN_LENGTH];
    uint16_t imsiM1112;
    uint8_t  imsiMS1[NAS_IMSI_M_S1_LENGTH];
    uint8_t  imsiMS2[NAS_IMSI_M_S2_LENGTH];
} nas_minBasedIMSI;

/**
 * This structure contains True IMSI.
 *  @param  mccT
 *          - ASCII character representation of MCC_T
 *
 *  @param  imsiT1112
 *          - ASCII character representation of IMSI_T_11_12 value
 *              - 0xFFFF - Not Available
 *
 *  @param  imsiTS1
 *          - ASCII character representation of IMSI_T_S1 value
 *
 *  @param  imsiTS2
 *          - ASCII character representation of IMSI_T_S2 value
 *
 *  @param  imsiTaddrNum
 *          - Value of IMSI_T_ADDR_NUM
 *              - 0xFF - Not Available
 *
 */
typedef struct
{
    uint8_t mccT[NAS_PLMN_LENGTH];
    uint16_t imsiT1112;
    uint8_t imsiTS1[NAS_IMSI_M_S1_LENGTH];
    uint8_t imsiTS2[NAS_IMSI_M_S2_LENGTH];
    uint8_t imsiTaddrNum;
} nas_trueIMSI;

/**
 * This structure contains CDMA channel.
 *  @param  priChA
 *          - A Channel number for the primary carrier.
 *              - 0xFFFF - Not Available
 *
 *  @param  priChB
 *          - B Channel number for the primary carrier.
 *              - 0xFFFF - Not Available
 *
 *  @param  secChA
 *          - A Channel number for the secondary carrier.
 *              - 0xFFFF - Not Available
 *
 *  @param  secChB
 *          - B Channel number for the secondary carrier.
 *              - 0xFFFF - Not Available
 *
 */
typedef struct
{
    uint16_t priChA;
    uint16_t priChB;
    uint16_t secChA;
    uint16_t secChB;
} nas_CDMAChannel;

/**
 * This structure contains the parameters for CDMA MDN Information
 *
 *  @param  mdnLen
 *          - Length of MDN
 *
 *  @param  MDN
 *          - MDN in ASCII format, max length is 15
 */
typedef struct
{
    uint8_t mdnLen;
    uint8_t mdn[LITE_MAX_NAS_3GPP2_MDN_LEN];
} nas_Mdn;

/**
 * 3GPP2 Subscription Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGet3GPP2Subscription(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasGet3GPP2Subscription_t *reqArg
        );

/**
 *  Structure for 3GPP2Subscription unpack.
 *
 *  @param  pNAMNameInfo [Optional]
 *          - See @ref nas_namName for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pDirNum [Optional]
 *          - See @ref nas_dirNum for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pHomeSIDNID [Optional]
 *          - See @ref nas_homeSIDNID for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pMinBasedIMSI [Optional]
 *          - See @ref nas_minBasedIMSI for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pTrueIMSI [Optional]
 *          - See @ref nas_trueIMSI for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pCDMAChannel [Optional]
 *          - See @ref nas_CDMAChannel for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pMdn [Optional]
 *          - See @ref nas_mdn for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_namName      *pNAMNameInfo;
    nas_dirNum       *pDirNum;
    nas_homeSIDNID   *pHomeSIDNID;
    nas_minBasedIMSI *pMinBasedIMSI;
    nas_trueIMSI     *pTrueIMSI;
    nas_CDMAChannel  *pCDMAChannel;
    nas_Mdn          *pMdn;
    swi_uint256_t    ParamPresenceMask;
} unpack_nas_SLQSNasGet3GPP2Subscription_t;

/**
 * 3GPP2 Subscription Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGet3GPP2Subscription(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNasGet3GPP2Subscription_t *reqArg
        );

/**
 * 3GPP2 Subscription Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGet3GPP2Subscription(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_nas_SLQSNasGet3GPP2Subscription_t *pOutput
);


/**
 *  This structure contains protocol subtype element.
 *  @param  PhysicalLayer
 *          - Specifies Physical Layer Protocol subtype
 *
 *  @param  ControlMac
 *          - Specifies Control Channel MAC Protocol subtype
 *
 *  @param  AccessMac
 *          - Specifies Access Channel MAC Protocol subtype
 *
 *  @param  ForwardMac
 *          - Specifies Forward Traffic Channel MAC Protocol subtype
 *
 *  @param  ReverseMac
 *          - Specifies Reverse Traffic Channel MAC Protocol subtype
 *
 *  @param  KeyExchange
 *          - Specifies Key exchange Protocol subtype
 *
 *  @param  AuthProt
 *          - Specifies Authentication Protocol subtype
 *
 *  @param  EncryptProt
 *          - Specifies Encryption Protocol subtype
 *
 *  @param  SecProt
 *          - Specifies Security Protocol subtype
 *
 *  @param  IdleState
 *          - Specifies Idle state Protocol subtype
 *
 *  @param  MultDisc
 *          - Specifies Generic multimode capability discovery Protocol subtype
 *
 *  @param  VirtStream
 *          - Specifies Generic Virtual Stream Protocol subtype
 *
 */
typedef struct
{
    uint16_t PhysicalLayer;
    uint16_t ControlMac;
    uint16_t AccessMac;
    uint16_t ForwardMac;
    uint16_t ReverseMac;
    uint16_t KeyExchange;
    uint16_t AuthProt;
    uint16_t EncryptProt;
    uint16_t SecProt;
    uint16_t IdleState;
    uint16_t MultDisc;
    uint16_t VirtStream;
} nas_protocolSubtypeElement;

/**
 *  Structure for HDRPersonality unpack.
 *
 *  @param  pCurrentPersonality[Out]
 *          - Current active personality index.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPersonalityListLength[In/Out]
 *          - Number of Personality Protocol Subtype contains in this response.
 *          - maximum input value is 3
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pProtocolSubtypeElement[Out]
 *          - See @ref nas_protocolSubtypeElement for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t                   *pCurrentPersonality;
    uint8_t                    *pPersonalityListLength;
    nas_protocolSubtypeElement *pProtocolSubtypeElement;
    swi_uint256_t              ParamPresenceMask;
} unpack_nas_SLQSSwiGetHDRPersonality_t;

/**
 * HDR Personality Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiGetHDRPersonality(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * HDR Personality Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiGetHDRPersonality(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiGetHDRPersonality_t *pOutput
        );

/**
 * Structure for HDRProtSubtype unpack.
 *
 *  @param  pCurrentPersonality
 *          - Current active personality index.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPersonalityListLength
 *          - Number of Personality Protocol Subtype contains in this response.
 *          - maximum input value is 4
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pProtocolSubtypeElement
 *          - See @ref nas_protocolSubtypeElement for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pAppSubType
 *          - Stream application subtype
 *          - Application subtype for each stream
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t                   *pCurrentPrsnlty;
    uint8_t                    *pPersonalityListLength;
    nas_protocolSubtypeElement *pProtoSubTypElmnt;
    uint64_t                   *pAppSubType;
    swi_uint256_t              ParamPresenceMask;
}unpack_nas_SLQSSwiGetHDRProtSubtype_t;

/**
 * HDR Protocol Sub type Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiGetHDRProtSubtype(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * HDR Protocol Sub type Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiGetHDRProtSubtype(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiGetHDRProtSubtype_t *pOutput
        );

/**
 * Structure for PS connection detach pack.
 *
 *  @param  pDetachAction[IN]
 *          - Values
 *              - 2- Initiates an immediate packet domain detach.
 *
 */
typedef struct
{
    uint8_t *pDetachAction;
}pack_nas_SLQSSwiPSDetach_t;

typedef unpack_result_t  unpack_nas_SLQSSwiPSDetach_t;

/**
 * PS Connection Detach Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in]  pReqParam request Parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiPSDetach(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSSwiPSDetach_t *pReqParam
        );

/**
 * PS Connection Detach Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiPSDetach(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiPSDetach_t *pOutput
        );

/**
 * Structure for Error Rate unpack parameters.
 *
 *  @param  pCDMAFrameErrRate
 *          - CDMA Frame Error Rate
 *          - Valid error rate values between 1 and 10000 are returned to
 *            indicate the percentage, e.g., a value of 300 means the error
 *            rate is 3%.
 *          - A value of 0xFFFF indicates that the error rate is
 *            unknown/unavailable.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pHDRPackErrRate
 *          - HDR Packet Error Rate
 *          - Valid error rate values between 1 and 10000 are returned to
 *            indicate the percentage, e.g., a value of 300 means the error
 *            rate is 3%.
 *          - A value of 0xFFFF indicates that the error rate is
 *            unknown/unavailable.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pGSMBER
 *          - GSM Bit Error Rate
 *          - Valid error rate values between 1 and 100 are returned to
 *            indicate the percentage value.
 *          - A 0% block error rate (BLER) indicates No Data.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pWCDMABER
 *          - WCDMA Block Error Rate
 *          - Valid error rate values between 1 and 100 are returned to
 *            indicate the percentage value.
 *          - A value of 0xFF indicates that the error rate is
 *            unknown/unavailable.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t *pCDMAFrameErrRate;
    uint16_t *pHDRPackErrRate;
    uint8_t  *pGSMBER;
    uint8_t  *pWCDMABER;
    swi_uint256_t  ParamPresenceMask;
}unpack_nas_SLQSGetErrorRate_t;

/**
 * Get Error Rate Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetErrorRate(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get Error Rate Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetErrorRate(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetErrorRate_t *pOutput
        );

/**
 *  This structure contains DRC information.
 *  @param  DRCValue
 *          - Current Data Rate Channel
 *
 *  @param  DRCCover
 *          - Current Data Rate Channel cover
 *
 */
typedef struct
{
    uint8_t DRCValue;
    uint8_t DRCCover;
} nas_DRCParams;

/**
 *  This structure contains pilot sets parameters.
 *  @param  PilotType
 *          - 0x00 - NAS_HRPD_PILOT_CURR_ACT_PLT
 *                   Current Active Pilot
 *          - 0x01 - NAS_HRPD_PILOT_NEIGHBOR_PLT
 *                   Neighbor pilot information
 *
 *  @param  PilotPN
 *          - Pilot PN sequence offset index
 *
 *  @param  PilotStrength
 *          - Strength of the pilot (in dB)
 *
 */
typedef struct
{
    uint32_t  PilotType;
    uint16_t  PilotPN;
    uint16_t  PilotStrength;
} nas_PilotSetParams;

/**
 * This structure contains pilot sets data.
 *  @param  NumPilots
 *          - Number of Pilot Sets
 *          - As input specifies number of sets of parameter pPilotSetInfo
 *            for which memory has been assigned
 *          - As output specifies the actual number of sets of parameter
 *            pPilotSetInfo returned by device
 *
 *  @param  pPilotSetInfo
 *          - Pilot Set Parameters
 *          - See @ref nas_PilotSetParams for more information.
 *
 *  note   A buffer under sized error is returned if the number of sets of
 *         pPilotSetInfo returned by the device is greater than the value in
 *         NumPilots input parameter.
 */
typedef struct
{
    uint8_t            NumPilots;
    nas_PilotSetParams *pPilotSetInfo;
} nas_PilotSetData;

/**
 * Structure for HRPD Stats unpack.
 *
 *  @param  pDRCParams
 *          - See @ref nas_DRCParams for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pUATI
 *          - A 128-bit address that includes the access terminal
 *            identifier and subnet ID
 *          - Size must be 16 bytes
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pPilotSetData
 *          - See @ref nas_PilotSetData for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_DRCParams    *pDRCParams;
    uint8_t          *pUATI;
    nas_PilotSetData *pPilotSetData;
    swi_uint256_t    ParamPresenceMask;
}unpack_nas_SLQSSwiGetHRPDStats_t;

/**
 * HRPD Stats Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiGetHRPDStats(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * HRPD Stats Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiGetHRPDStats(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiGetHRPDStats_t *pOutput
        );

/**
 * This structure contains pilot PN Element.
 *  @param  ActSetPilotPN
 *          - Active Pilot PN
 *
 *  @param  ActSetPilotPNStrength
 *          - Active Pilot PN strength
 */
typedef struct
{
    uint16_t ActSetPilotPN;
    uint8_t  ActSetPilotPNStrength;
} nas_ActPilotPNElement;

/**
  * This structure contains network stat 1x.
 *  @param  State
 *          - CDMA current access state
 *              - 0x01 - Searching
 *              - 0x02 - Idle
 *              - 0x03 - Traffic
 *              - Others - NA
 *
 *  @param  SO
 *          - CDMA service option
 *              - 0xFFFF - Not in a call
 *              - 0x0001 - Basic Variable Rate Voice Service(8kbps)
 *              - 0x0002 - Mobile Station Loopback(8kbps)
 *              - 0x0003 - Enhanced Variable Rate Codec(EVRC) Voice
 *                         Service(8kbps)
 *              - 0x0006 - Short message Services(Rate Set 1)
 *              - 0x0009 - Mobile Station Loopback(13kbps)
 *              - 0x000E - Short Message Service (Rate Set 2)
 *              - 0x0011 - High Rate Voice Service(13kbps)
 *              - 0x0020 - Test Data Service Option(TDSO)
 *              - 0x0021 - cdma2000 High Speed Packet Data Service,
 *                         Internet or ISO Protocol Stack
 *              - 0x0044 - EVRC-B Voice Service(8 kbps)
 *              - 0x0046 - EVRC-WB Voice Service(8 kbps)
 *              - 0x0049 - Voice Echo mode supports smart blanking(EVRC-NW)
 *              - 0x004B - Enhanced loopback
 *              - 0x8000 - Proprietary Service Option (Qualcomm Inc.)
 *
 *  @param  RX_PWR
 *          - RX Pwr(dBm)
 *              - 0xABCD00EF - -ABCD.EF dBm
 *              - ABCD00EF should be transferred to decimal while displaying
 *              - Example: 0x12340056 - -4660.86dBm\n
 *                 0x1234 = 4660, 0x0056 = 86
 *              - 0xFFFFFFFF - NA
 *
 *  @param  RX_EC_IO
 *          - RX EC/IO(dB)
 *              - 0xABCD - -AB.CD dB
 *              - ABCD should be transferred to decimal while displaying
 *              - Example: 0x1234 - -18.52dB \n
 *                 0x12 = 18, 0x34 = 52
 *              - 0xFFFF - NA
 *
 *  @param  TX_PWR
 *          - TX PWR(dBm)
 *              - 0xFFFFFFFF - NA
 *              - Others - display actual value in decimal
 *              - Example: 0x1234 - -4660dBm \n
 *                 0x1234 = 4660
 *
 *  @param  ActSetCnt
 *          - Count of active pilot PN elements
 *          - As input specifies number of sets of parameter pActPilotElements
 *            for which memory has been assigned
 *          - As output specifies the actual number of sets of parameter
 *            pActPilotElements returned by device
 *
 *  @param  pActPilotPNElements
 *          - See @ref nas_ActPilotPNElement for more information
 *
 *  @param  NeighborSetCnt
 *          - Count of neighbor pilot PN elements
 *          - As input specifies number of sets of parameter pNeighborSetPilotPN
 *            for which memory has been assigned
 *          - As output specifies the actual number of sets of parameter
 *            pNeighborSetPilotPN returned by device
 *
 *  @param  pNeighborSetPilotPN
 *          - Neighbor pilot PN
 */
typedef struct
{
    uint8_t               State;
    uint16_t              SO;
    uint32_t              RX_PWR;
    uint16_t              RX_EC_IO;
    uint32_t              TX_PWR;
    uint8_t               ActSetCnt;
    nas_ActPilotPNElement *pActPilotPNElements;
    uint8_t               NeighborSetCnt;
    uint16_t              *pNeighborSetPilotPN;
} nas_NetworkStat1x;

/**
 *  This structure contains network stat EVDO.
 *  @param  State
 *          - EVDO network access state
 *              - 0x00 - Sleep
 *              - 0x01 - Searching
 *              - 0x02 - Idle
 *              - 0x03 - Active
 *              - 0xFF - NA
 *
 *  @param  MACIndex
 *          - HDR Mac index
 *              - 0xFF - NA
 *              - Others - Display the actual value in decimal
 *              - Example: 0x12 - 18 \n
 *                 0x12 = 18
 *
 *  @param  SectorIDLen (IN/OUT)
 *          - Sector ID length
 *          - As input specifies length of parameter pSectorID
 *            for which memory has been assigned
 *          - As output specifies the actual length of parameter
 *            pSectorID returned by device
 *
 *  @param  pSectorID
 *          - Sector ID
 *
 *  @param  RX_PWR
 *          - TX PWR(dBm)
 *              - 0xABCD - -ABCD dBm
 *              - ABCD should be transferred to decimal while displaying
 *              - Example: 0x1234 - -4660dBm \n
 *                 0x1234 = 4660
 *              - 0xFFFF - NA
 *
 *  @param  PER
 *          - HDR Packet Error Rate
 *              - 0xFFFF - Unknown
 *              - Others - display the actual value in decimal
 *              - Example: 0x1234 - -4660dBm \n
 *                 0x1234 = 4660
 *
 *  @param  PilotEnergy
 *          - Pilt Energy (dB)
 *              - 0xFFFF - NA
 *              - 0xABCD should be transferred to decimal while displaying
 *              - Example: 0x1234 - -4660dBm \n
 *                 0x1234 = 4660
 *
 *  @param  SNR
 *          - Signal to Noise ratio (dB)
 */
typedef struct
{
    uint8_t   State;
    uint8_t   MACIndex;
    uint8_t   SectorIDLen;
    uint16_t  *pSectorID;
    uint16_t  RX_PWR;
    uint16_t  PER;
    uint16_t  PilotEnergy;
    uint8_t   SNR;
} nas_NetworkStatEVDO;

/**
 * This structure contains device configure detail.
 *  @param  Technology
 *          - Current technology in use
 *              - 0x00 - 1x RTT
 *              - 0x01 - EVDO Rev 0
 *              - 0x02 - EVDO Rev A
 *              - 0xFF - NA
 *
 *  @param  QLIC
 *          - Quasi Linear Interference Cancellation
 *              - 0x00 - Not supported
 *              - 0x01 - Supported
 *
 *  @param  Chipset
 *          - Qualcomm platform
 *              - 0x4E - MDM6200
 *              - 0x4F - MDM6600
 *              - 0xFF - NA
 *
 *  @param  HWVersion
 *          - Hardware version
 *              - 0x00 - BSHWREV0
 *              - 0x01 - BSHWREV1
 *              - 0x02 - BSHWREV2
 *              - 0x03 - BSHWREV3
 *              - 0x04 - BSHWREVMAX
 *              - 0xFF - BSHWREVUNKNOWN
 *
 */
typedef struct
{
    uint8_t Technology;
    uint8_t QLIC;
    uint8_t Chipset;
    uint8_t HWVersion;
} nas_DeviceConfigDetail;

/**
 * This structure contains data status detail
 *  @param  IPAddress
 *          - IP Address
 *          - 0xABCDEFGH - AB.CD.EF.GH
 *          - Example:
 *              - 0x12345678 - 18.52.86.120 \n
 *              0x12=18 0x34=52 0x56=86 0x78=120
 *          - 0xFFFFFFFF - NA
 *
 *  @param  LastErrCode
 *          - MIP Error code
 *              - 0x00 - MIP_RRP_CODE_SUCCESS
 *              - 0x01 - MIP_RRP_CODE_SUCCESS_NO_SIM_BINDINGS
 *              - 0x40 - MIP_RRP_CODE_FAILURE_FA_REASON_UNSPECIFIED
 *              - 0x41 - MIP_RRP_CODE_FAILURE_FA_ADMIN_PROHIBITED
 *              - 0x42 - MIP_RRP_CODE_FAILURE_FA_INSUFFICIENT_RESOURCES
 *              - 0x43 - MIP_RRP_CODE_FAILURE_FA_MOBILE_NODE_FAILED_AUTH
 *              - 0x44 - MIP_RRP_CODE_FAILURE_FA_HA_FAILED_AUTH
 *              - 0x45 - MIP_RRP_CODE_FAILURE_FA_REQUESTED_LIFETIME_TOO_LONG
 *              - 0x46 - MIP_RRP_CODE_FAILURE_FA_MALFORMED_REQUEST
 *              - 0x47 - MIP_RRP_CODE_FAILURE_FA_MALFORMED_REPLY
 *              - 0x48 - MIP_RRP_CODE_FAILURE_FA_ENCAPSULATION_UNAVAILABLE
 *              - 0x49 - MIP_RRP_CODE_FAILURE_FA_VJHC_UNAVAILABLE
 *              - 0x4A - MIP_RRP_CODE_FAILURE_FA_CANT_REV_TUN
 *              - 0x4B - MIP_RRP_CODE_FAILURE_FA_MUST_REV_TUN
 *              - 0x4C - MIP_RRP_CODE_FAILURE_FA_BAD_TTL
 *              - 0x4D - MIP_RRP_CODE_FAILURE_INVALID_COA
 *              - 0x4F - MIP_RRP_CODE_FAILURE_FA_DELIVERY_STYLE_NOT_SUPPORTED
 *              - 0x59 - MIP_RRP_CODE_FAILURE_FA_VS_REASON
 *              - 0x61 - MIP_RRP_CODE_FAILURE_MISSING_NAI
 *              - 0x62 - MIP_RRP_CODE_FAILURE_MISSING_HA_ADDR
 *              - 0x63 - MIP_RRP_CODE_FAILURE_MISSING_HOMEADDR
 *              - 0x68 - MIP_RRP_CODE_FAILURE_UNKNOWN_CHALLENGE
 *              - 0x69 - MIP_RRP_CODE_FAILURE_MISSING_CHALLENGE
 *              - 0x6A - MIP_RRP_CODE_FAILURE_STALE_CHALLENGE
 *              - 0x6B - MIP_RRP_CODE_FAILURE_MISSING_MN_FA
 *              - 0x80 - MIP_RRP_CODE_FAILURE_HA_REASON_UNSPECIFIED
 *              - 0x81 - MIP_RRP_CODE_FAILURE_HA_ADMIN_PROHIBITED
 *              - 0x82 - MIP_RRP_CODE_FAILURE_HA_INSUFFICIENT_RESOURCES
 *              - 0x83 - MIP_RRP_CODE_FAILURE_HA_MOBILE_NODE_FAILED_AUTH
 *              - 0x84 - MIP_RRP_CODE_FAILURE_HA_FA_FAILED_AUTH
 *              - 0x85 - MIP_RRP_CODE_FAILURE_HA_REG_ID_MISMATCH
 *              - 0x86 - MIP_RRP_CODE_FAILURE_HA_MALFORMED_REQUEST
 *              - 0x88 - MIP_RRP_CODE_FAILURE_UNKNOWN_HA
 *              - 0x89 - MIP_RRP_CODE_FAILURE_HA_CANT_REV_TUN
 *              - 0x8A - MIP_RRP_CODE_FAILURE_HA_MUST_REV_TUN
 *              - 0x8B - MIP_RRP_CODE_FAILURE_HA_ENCAPSULATION_UNAVAILABLE
 *              - 0x8F - MIP_RRP_CODE_FAILURE_REDIRECTED_HA
 *              - 0x90 - MIP_RRP_CODE_FAILURE_HA_BAD_AAA_AUTH
 *              - 0xFF - NA
 */
typedef struct
{
    uint32_t IPAddress;
    uint8_t  LastErrCode;
} nas_DataStatusDetail;

/**
 * Structure for SwiNetworkDebug unpack.
 *
 *  @param  pObjectVer
 *          - Object's version number for the host to handle
 *              - 0xFF - NA
 *              - Others - shows in decimal
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pNetworkStat1x
 *          - See @ref nas_NetworkStat1x for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNetworkStatEVDO
 *          - See @ref nas_NetworkStatEVDO for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pDeviceConfigDetail
 *          - See @ref nas_DeviceConfigDetail for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pDataStatusDetail
 *          - See @ref nas_DataStatusDetail for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t                *pObjectVer;
    nas_NetworkStat1x      *pNetworkStat1x;
    nas_NetworkStatEVDO    *pNetworkStatEVDO;
    nas_DeviceConfigDetail *pDeviceConfigDetail;
    nas_DataStatusDetail   *pDataStatusDetail;
    swi_uint256_t          ParamPresenceMask;
}unpack_nas_SLQSSwiNetworkDebug_t;

/**
 * Network Debug Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSSwiNetworkDebug(
        pack_qmi_t  *pCtx,
        uint8_t     *pReqBuf,
        uint16_t    *pLen
        );

/**
 * Network Debug Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiNetworkDebug(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiNetworkDebug_t *pOutput
        );

/**
 *  This sturcture contain RSSI threshold list.
 *  @param  RSSIThresListLen
 *          - Length of the RSSI threshold list parameter to follow
 *
 *  @param  pRSSIThresList
 *          - RSSI in dBm( signed bytes )
 *          - A value of -125 dBm or lower is used to indicate No Signal
 *          - RSSI values have the following ranges (in dBm)
 *              - CDMA is -105 to -21
 *              - HDR is -118 to -13
 *              - GSM is -111 to -48
 *              - WCDMA is -121 to 0
 *              - LTE is -120 to 0
 *          - Threshold values specified above are used for all RATs
 *          - The maximum number of threshold values is 16, each a signed byte value.
 *
 */
typedef struct
{
    uint8_t RSSIThresListLen;
    int8_t *pRSSIThresList;
} nas_RSSIThresh;

/**
 *  This sturcture contain ECIO threshold list.
 *  @param  ECIOThresListLen
 *          - Length of the ECIO threshold list parameter to follow
 *
 *  @param  pECIOThresList
 *          - Sequence of thresholds delimiting ECIO event reporting bands
 *          - Every time a new ECIO value crosses a threshold value, an
 *            event report indication message with the new ECIO value is sent
 *            to the requesting control point. For this field
 *              - Each ECIO threshold value is a signed 2 byte value
 *              - Each ECIO threshold value increments in negative
 *                0.5 dB, e.g., an ECIO threshold value of 2 means -1dB
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *              - Threshold values specified above are used for all RATs
 *
 */
typedef struct
{
    uint8_t ECIOThresListLen;
    int16_t *pECIOThresList;
} nas_ECIOThresh;

/**
 *  This structure contains HDR SINR threshold list.
 *  @param  HDRSINRThresListLen
 *          - Length of the HDR SINR threshold list parameter to follow
 *
 *  @param  pHDRSINRThresList
 *          - Sequence of thresholds delimiting SINR event reporting bands
 *          - Every time a new SINR value crosses a threshold value, an
 *            event report indication message with the new SINR value is sent
 *            to the requesting control point. For this field
 *              - SINR is reported only for HDR
 *              - Each SINR threshold value is an unsigned 1 byte value
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *
 */
typedef struct
{
    uint8_t HDRSINRThresListLen;
    uint8_t *pHDRSINRThresList;
} nas_HDRSINRThresh;

/**
 *  This structure contains LTE SNR threshold list.
 *  @param  LTESNRThresListLen
 *          - Length of the LTE SNR threshold list parameter to follow
 *
 *  @param  pLTESNRThresList
 *          - Sequence of thresholds delimiting SNR event reporting bands
 *          - Every time a SNR value crosses a threshold value, an
 *            event report indication message with the new SNR value is sent
 *            to the requesting control point. For this field
 *              - For LTE, each SNR threshold value is a signed 2 Byte value
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *              - SNR level as a scaled integer in units of 0.1 dB; e.g., -16 dB
 *                has a value of -160 and 24.6 dB has a value of 246
 *
 */
typedef struct
{
    uint8_t LTESNRThresListLen;
    int16_t *pLTESNRThresList;
} nas_LTESNRThresh;

/**
 *  This structure contains IO threshold list.
 *  @param  IOThresListLen
 *          - Length of the IO threshold list parameter to follow
 *
 *  @param  pIOThresList
 *          - Sequence of thresholds delimiting IO event reporting bands
 *          - Every time a new IO value crosses a threshold value, an
 *            event report indication message with the new IO value is sent
 *            to the requesting control point. For this field
 *              - IO is applicable only for HDR
 *              - Each IO threshold value is a signed 4 byte value
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *
 */
typedef struct
{
    uint8_t IOThresListLen;
    int32_t *pIOThresList;
} nas_IOThresh;

/**
 *  This structure contains RSRQ threshold list.
 *  @param  RSRQThresListLen
 *          - Length of the RSRQ threshold list parameter to follow
 *
 *  @param  pRSRQThresList
 *          - Sequence of thresholds delimiting current RSRQ event reporting
 *            bands
 *          - Every time a new RSRQ value crosses a threshold value, an
 *            event report indication message with the new RSRQ value is sent
 *            to the requesting control point. For this field
 *              - RSRQ values are applicable only for LTE
 *              - RSRQ values are measured in dBm, with a range of -20
 *                dBm to -3 dBm
 *              - Each RSRQ threshold value is a signed byte value
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *
 */
typedef struct
{
    uint8_t RSRQThresListLen;
    int8_t *pRSRQThresList;
} nas_RSRQThresh;

/**
 *  This structure contains RSRP threshold list.
 *  @param  RSRPThresListLen
 *          - Length of the RSRP threshold list parameter to follow
 *
 *  @param  pRSRPThresList
 *          - Sequence of thresholds delimiting current RSRP event reporting
 *            bands
 *          - Every time a new RSRP value crosses a specified threshold value,
 *            an event report indication message with the new RSRQ value is
 *            sent to the requesting control point. For this field
 *              - RSRP values are applicable only for LTE
 *              - RSRP values are measured in dBm, with a range of -44
 *                dBm to -140 dBm
 *              - Each RSRP threshold value is a signed byte value
 *              - Maximum number of threshold values is 16
 *              - At least one value must be specified
 *
 */
typedef struct
{
    uint8_t  RSRPThresListLen;
    int16_t *pRSRPThresList;
} nas_RSRPThresh;

/**
 *  This structure contains LTE signal report config.
 *  @param  rptRate
 *          - Rate on how often the LTE signal must be checked for reporting
 *          - Values:
 *              - 0 - Report using the default configuration
 *              - 1 - Report every 1 sec
 *              - 2 - Report every 2 sec
 *              - 3 - Report every 3 sec
 *              - 4 - Report every 4 sec
 *              - 5 - Report every 5 sec
 *
 *  @param  avgPeriod
 *          - Averaging period to be used for the LTE signal
 *          - Values:
 *              - 0 - Average using the default configuration
 *              - 1 - Average over 1 sec
 *              - 2 - Average over 2 sec
 *              - 3 - Average over 3 sec
 *              - 4 - Average over 4 sec
 *              - 5 - Average over 5 sec
 *              - 6 - Average over 6 sec
 *              - 7 - Average over 7 sec
 *              - 8 - Average over 8 sec
 *              - 9 - Average over 9 sec
 *              - 10 - Average over 10 sec
 *
 */
typedef struct
{
    uint8_t rptRate;
    uint8_t avgPeriod;
} nas_LTESigRptCfg;

/**
 *  This structure contains TDSCDMA SINR threshold list.
 *  @param  TDSCDMASINRCONFThreshListLen
 *          - Length of the TDSCDMA SINR threshold list parameter to follow
 *
 *  @param  pTDSCDMASINRCONFThreshList
 *          - Array of SINR thresholds (in dB) used by TD-SCDMA
 *          - Maximum of 32 values
 */
typedef struct
{
    uint8_t   TDSCDMASINRCONFThreshListLen;
    float     *pTDSCDMASINRCONFThreshList;
} nas_TDSCDMASINRCONFThresh;

/**
 * Structure for ConfigSigInfo pack.
 *
 *  @param  pRSSIThresh
 *          - RSSI threshold List
 *          - See @ref nas_RSSIThresh for more details
 *
 *  @param  pECIOThresh
 *          - ECIO Threshold List
 *          - See @ref nas_ECIOThresh for more details
 *
 *  @param  pHDRSINRThresh
 *          - HDR SINR Threshold List
 *          - See @ref nas_HDRSINRThresh for more details
 *
 *  @param  pLTESNRThresh
 *          - LTE SNR Threshold List
 *          - See @ref nas_LTESNRThresh for more details
 *
 *  @param  pIOThresh
 *          - IO Threshold List
 *          - See @ref nas_IOThresh for more details
 *
 *  @param  pRSRQThresh
 *          - RSRQ Threshold List
 *          - See @ref nas_RSRQThresh for more details
 *
 *  @param  pRSRPThresh
 *          - RSRP Threshold List
 *          - See @ref nas_RSRPThresh for more details
 *
 *  @param  pLTESigRptCfg
 *          - LTE signal report config
 *          - See @ref nas_LTESigRptCfg for more details
 *
 *  @param  pTDSCDMASINRCONFThresh
 *          - TD-SCDMA SINR Threshold List
 *          - See @ref nas_TDSCDMASINRCONFThresh for more details
 *
 */
typedef struct
{
    nas_RSSIThresh            *pRSSIThresh;
    nas_ECIOThresh            *pECIOThresh;
    nas_HDRSINRThresh         *pHDRSINRThresh;
    nas_LTESNRThresh          *pLTESNRThresh;
    nas_IOThresh              *pIOThresh;
    nas_RSRQThresh            *pRSRQThresh;
    nas_RSRPThresh            *pRSRPThresh;
    nas_LTESigRptCfg          *pLTESigRptCfg;
    nas_TDSCDMASINRCONFThresh *pTDSCDMASINRCONFThresh;
} pack_nas_SLQSConfigSigInfo_t;

typedef unpack_result_t  unpack_nas_SLQSConfigSigInfo_t;

/**
 * Config Sig Info Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSConfigSigInfo(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSConfigSigInfo_t *pReqParam
        );

/**
 * Config Sig Info Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSConfigSigInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSConfigSigInfo_t *pOutput
        );

/**
 * Structure for HomeNetwork3GPP2 unpack.
 *
 *  @param  pMCC
 *          - Mobile country code (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pMNC
 *          - Mobile network code (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  nameSize
 *          - Maximum number of characters (including NULL terminator) that
 *            network name array can contain (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pName
 *          - Network name or description represented as a NULL terminated
 *            string (empty string returned when unknown) (UMTS only).
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSID
 *          - Home network system ID
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pNID
 *          - Home network ID\n
 *              - 0xFFFF - Unknown.
 *              - Only applies to cdma2000
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pNw2MCC
 *          - Mobile country code (3GPP2 only).
 *          - Range : 0 to 999
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNw2MNC
 *          - Mobile network code (3GPP2 only).
 *          - Range : 0 to 999
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNw2DescDisp
 *          - Network Name Display (3GPP2 only).
 *          -Valid Value
 *             - 0x00 - Do not display
 *             - 0x01 - Display
 *             - 0xFF - Unknown
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNw2DescEnc
 *          - Encoding of the network description (3GPP2 only).
 *          - Valid Value
 *              - 0x00 - Octet, unspecified
 *              - 0x02 - 7-bit ASCII (\ref liteqmi_helper_decode7bitAsciiEncString)
 *              - 0x04 - Unicode
 *              - 0x09 - GSM 7-bit default
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNw2DescLen
 *          - Network Description Length (3GPP2 only).
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pNw2Name
 *          - Network Name (3GPP2 only).
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t *pMCC;
    uint16_t *pMNC;
    uint8_t   nameSize;
    uint8_t  *pName;
    uint16_t *pSID;
    uint16_t *pNID;
    uint16_t *pNw2MCC;
    uint16_t *pNw2MNC;
    uint8_t  *pNw2DescDisp;
    uint8_t  *pNw2DescEnc;
    uint8_t  *pNw2DescLen;
    uint8_t  *pNw2Name;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_GetHomeNetwork3GPP2_t;

/**
 * Get Home Network 3GPP2 Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_GetHomeNetwork3GPP2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get Home Network 3GPP2 Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_GetHomeNetwork3GPP2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_GetHomeNetwork3GPP2_t *pOutput
        );

/**
 * This structure contains WCDMA UARFCN infromation.
 *  @param  status
 *          - 0 - Disable
 *          - 1 - Enable
 *
 *  @param  uarfcn
 *          - UARFCN to which UE is locked
 */
typedef struct{
    uint8_t  status;
    uint32_t uarfcn;
} nas_wcdmaUARFCN;

/**
 *  This structure contains LTE EARFCN infromation.
 *  @param  status
 *          - 0 - Disable
 *          - 1 - Enable
 *
 *  @param  earfcn0
 *          - Primary DL EARFCN to which the UE is locked
 *
 *  @param  earfcn1
 *          - Secondary DL EARFCN to which the UE is locked
 *          - Note : Make earfcn1 value equal to earfcn0
 *                   if only one EARFCN is desired.
 */
typedef struct{
    uint8_t  status;
    uint32_t earfcn0;
    uint32_t earfcn1;
} nas_lteEARFCN;

/**
 *  This structure contains LTE PCI infromation.
 *  @param  status
 *          - 0 - Disable
 *          - 1 - Enable
 *
 *  @param  earfcn
 *          - UARFCN to which UE is locked
 *
 *  @param  pci
 *          - PCI to which the UE is locked
 */
typedef struct{
    uint8_t  status;
    uint32_t earfcn;
    uint32_t pci;
} nas_ltePCI;

/**
 * Structure for ChannelLock pack.
 *
 *  @param  pWcdmaUARFCN [Optional]
 *          - See @ref nas_wcdmaUARFCN for more information
 *
 *  @param  pLteEARFCN [Optional]
 *          - See @ref nas_lteEARFCN for more information
 *
 *  @param  pLtePCI [Optional]
 *          - See @ref nas_ltePCI for more information
 */
typedef struct
{
    nas_wcdmaUARFCN    *pWcdmaUARFCN;
    nas_lteEARFCN      *pLteEARFCN;
    nas_ltePCI         *pLtePCI;
} pack_nas_SLQSNASSwiSetChannelLock_t;

typedef unpack_result_t  unpack_nas_SLQSNASSwiSetChannelLock_t;

/**
 * Set Channel Lock Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASSwiSetChannelLock(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNASSwiSetChannelLock_t *pReqParam
        );

/**
 * Set Channel Lock Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASSwiSetChannelLock(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASSwiSetChannelLock_t *pOutput
        );

/**
 *Structure for Get Channel Lock unpack.
 *
 *  @param  pWcdmaUARFCN [Optional]
 *          - See @ref nas_wcdmaUARFCN for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pLteEARFCN [Optional]
 *          - See @ref nas_lteEARFCN for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pLtePCI [Optional]
 *          - See @ref nas_ltePCI for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_wcdmaUARFCN *pWcdmaUARFCN;
    nas_lteEARFCN   *pLteEARFCN;
    nas_ltePCI      *pLtePCI;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNASSwiGetChannelLock_t;

/**
 * Get Channel Lock Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASSwiGetChannelLock(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get Channel Lock Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASSwiGetChannelLock(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASSwiGetChannelLock_t *pOutput
        );

/**
 *  This structure contains the SLQSNASGeteDRXParams unpack parameters.
 *
 *  \param  pCycleLen [Optional]
 *          - eDRX cycle length
 *          - Value is per 3GPP TS 24.008 Rel-13 Section 10.5.5.32
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pPagingTimeWindow [Optional]
 *          - eDRX paging time window
 *          - Value is per 3GPP TS 24.008 Rel-13 Section10.5.5.32
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pEdrxEnable [Optional]
 *          - eDRX Enable
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t      *pCycleLen;
    uint8_t      *pPagingTimeWindow;
    uint8_t      *pEdrxEnable;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNASGeteDRXParams_t;

/**
 * Get eDRX Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASGeteDRXParams(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get eDRX Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASGeteDRXParams(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASGeteDRXParams_t *pOutput
        );

/**
 *  This structure contains the SLQSNASGeteDRXParamsExt request parameters.
 *
 *  \param  pEdrxRAT [Optional]
 *          - eDRX Radio Access Technology
 *          - Values
 *            - NAS_RADIO_IF_NO_SVC (0x00) - None (no service)
 *            - NAS_RADIO_IF_CDMA_1X (0x01) - cdma2000 &reg; 1X
 *            - NAS_RADIO_IF_CDMA_1XEVDO (0x02) - cdma2000 &reg; HRPD (1xEV-DO)
 *            - NAS_RADIO_IF_AMPS (0x03) - AMPS
 *            - NAS_RADIO_IF_GSM (0x04) - GSM
 *            - NAS_RADIO_IF_UMTS (0x05) - UMTS
 *            - NAS_RADIO_IF_WLAN (0x06) - WLAN
 *            - NAS_RADIO_IF_GPS (0x07) - GPS
 *            - NAS_RADIO_IF_LTE (0x08) - LTE
 *            - NAS_RADIO_IF_TDSCDMA (0x09) - TD-SCDMA
 *            - NAS_RADIO_IF_LTE_M1 (0x0a) - LTE-M1
 *            - NAS_RADIO_IF_LTE_NB1 (0x0b) - LTE-NB1
 *            - NAS_RADIO_IF_NO_CHANGE (-1) - No change
 *            Note: The device will use "0x08 - NAS_RADIO_IF_LTE"
 *                  as the default value if the TLV is omitted.
 *
 *  \param  pLteOpMode [Optional]
 *          - LTE Operational Mode
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_ SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_ WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_ M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_ NB1 (0x03) - Camped on LTE NB1
 *            Note: This TLV should be set when edrx_rat_type is LTE.
 *
 */
typedef struct
{
    uint8_t      *pEdrxRAT;
    uint32_t     *pLteOpMode;
} pack_nas_SLQSNASGeteDRXParamsExt_t;

/**
 *  This structure contains the SLQSNASGeteDRXParams unpack parameters.
 *
 *  \param  pCycleLen [Optional]
 *          - eDRX cycle length
 *          - Value is per 3GPP TS 24.008 Rel-13 Section 10.5.5.32
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pPagingTimeWindow [Optional]
 *          - eDRX paging time window
 *          - Value is per 3GPP TS 24.008 Rel-13 Section10.5.5.32
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pEdrxEnable [Optional]
 *          - eDRX Enable
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  pEdrxRAT [Optional]
 *          - eDRX Radio Access Technology
 *          - Values
 *            - NAS_RADIO_IF_NO_SVC (0x00) - None (no service)
 *            - NAS_RADIO_IF_CDMA_1X (0x01) - cdma2000 &reg; 1X
 *            - NAS_RADIO_IF_CDMA_1XEVDO (0x02) - cdma2000 &reg; HRPD (1xEV-DO)
 *            - NAS_RADIO_IF_AMPS (0x03) - AMPS
 *            - NAS_RADIO_IF_GSM (0x04) - GSM
 *            - NAS_RADIO_IF_UMTS (0x05) - UMTS
 *            - NAS_RADIO_IF_WLAN (0x06) - WLAN
 *            - NAS_RADIO_IF_GPS (0x07) - GPS
 *            - NAS_RADIO_IF_LTE (0x08) - LTE
 *            - NAS_RADIO_IF_TDSCDMA (0x09) - TD-SCDMA
 *            - NAS_RADIO_IF_LTE_M1 (0x0a) - LTE-M1
 *            - NAS_RADIO_IF_LTE_NB1 (0x0b) - LTE-NB1
 *            - NAS_RADIO_IF_NO_CHANGE (-1) - No change
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  pLteOpMode [Optional]
 *          - LTE Operational Mode
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_ SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_ WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_ M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_ NB1 (0x03) - Camped on LTE NB1
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t      *pCycleLen;
    uint8_t      *pPagingTimeWindow;
    uint8_t      *pEdrxEnable;
    uint8_t      *pEdrxRAT;
    uint32_t     *pLteOpMode;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSNASGeteDRXParamsExt_t;

/**
 * Get eDRX Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASGeteDRXParamsExt(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNASGeteDRXParamsExt_t *pReqParam
        );

/**
 * Get eDRX Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASGeteDRXParamsExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASGeteDRXParamsExt_t *pOutput
        );

/**
 *  This structure contains the SLQSNASSeteDRXParams request parameters.
 *
 *  \param  pEdrxEnable [Optional]
 *          - eDRX Enable
 *          - Values
 *            - 0x00 - Disable
 *            - 0x01 - Enable
 *
 *  \param  pCycleLen [Optional]
 *          - eDRX cycle length
 *          - Value to be set per 3GPP TS 24.008 Rel-13 Section 10.5.5.32
 *
 *  \param  pPagingTimeWindow [Optional]
 *          - eDRX paging time window
 *          - Value to be set per 3GPP TS 24.008 Rel-13 Section 10.5.5.32
 *
 *  \param  pEdrxRatType [Optional]
 *          - Radio access technology
 *          - Values
 *            - NAS_RADIO_IF_NO_SVC (0x00) - None (no service)
 *            - NAS_RADIO_IF_CDMA_1X (0x01) - cdma2000 &reg; 1X
 *            - NAS_RADIO_IF_CDMA_1XEVDO (0x02)  cdma2000 &reg; HRPD (1xEV-DO)
 *            - NAS_RADIO_IF_AMPS (0x03) - AMPS
 *            - NAS_RADIO_IF_GSM (0x04) - GSM
 *            - NAS_RADIO_IF_UMTS (0x05) - UMTS
 *            - NAS_RADIO_IF_WLAN (0x06) - WLAN
 *            - NAS_RADIO_IF_GPS (0x07) - GPS
 *            - NAS_RADIO_IF_LTE (0x08) - LTE
 *            - NAS_RADIO_IF_TDSCDMA (0x09)- TD-SCDMA
 *            - NAS_RADIO_IF_LTE_M1 (0x0a) - LTE-M1
 *            - NAS_RADIO_IF_LTE_NB1 (0x0b) - LTE-NB1
 *            - NAS_RADIO_IF_NO_CHANGE (-1)- No change
 *            Note: The device will use “0x08 - NAS_RADIO_IF_LTE” as the default
 *            value if the TLV is omitted.
 *
 *  \param  pEdrxCiotLteMode [Optional]
 *          - CIOT LTE mode
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_ SRV(0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_ WB(0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_ M1(0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_ NB1(0x03) - Camped on LTE NB1
 *            Note: This TLV should be set when pEdrxRatType is LTE.
 */
typedef struct
{
    uint8_t      *pEdrxEnable;
    uint8_t      *pCycleLen;
    uint8_t      *pPagingTimeWindow;
    uint8_t      *pEdrxRatType;
    uint32_t     *pEdrxCiotLteMode;
} pack_nas_SLQSNASSeteDRXParams_t;

typedef unpack_result_t  unpack_nas_SLQSNASSeteDRXParams_t;

/**
 * Set eDRX Parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASSeteDRXParams(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_SLQSNASSeteDRXParams_t *pReqParam
        );

/**
 * Set eDRX Parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASSeteDRXParams(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASSeteDRXParams_t *pOutput
        );

/**
 *  Elements for HDR Personality indication.
 *
 *  @param  pCurrentPersonality
 *          - Current active personality index.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pPersonalityListLength
 *          - Number of Personality Protocol Subtype contains in this response.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pProtocolSubtypeElement
 *          - See @ref nas_protocolSubtypeElement for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 */
typedef struct
{
    uint16_t                   *pCurrentPersonality;
    uint8_t                    *pPersonalityListLength;
    nas_protocolSubtypeElement *pProtocolSubtypeElement;
} nas_HDRPersonality_Ind_Data;

/**
 *  Structure for HDR Personality indication unpack.
 *
 *  @param  pHDRPersInd
 *          - See @ref nas_HDRPersonality_Ind_Data for more information.
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_HDRPersonality_Ind_Data *pHDRPersInd;
    swi_uint256_t  ParamPresenceMask;
} unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t;

/**
 * unpack nas SWI Hdr Personality indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiHDRPersonalityCallback_Ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiHDRPersonalityCallback_Ind_t *pOutput
        );


/**
 *  Elements for Rank indicator TLV.
 *
 *  @param  TlvPresent
 *          - value is 1 if TLV is present, otherwise 0
 *
 *  @param  count0
 *          - count of Rank1
 *
 *  @param  count1
 *          - count of Rank2
 *
 */
typedef struct
{
    uint8_t                   TlvPresent;
    uint32_t                  count0;
    uint32_t                  count1;
} nas_RankIndicatorTlv;


/**
 *  Structure for Rank Indicator indication unpack.
 *
 *  @param  rankIndicatorTlv
 *          - See @ref nas_RankIndicatorTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_RankIndicatorTlv rankIndicatorTlv;
    swi_uint256_t        ParamPresenceMask;
} unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t;

/**
 * unpack nas SWI Rank indicator indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSSwiRandIndicatorCallback_Ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSSwiRandIndicatorCallback_Ind_t *pOutput
        );

/**
 * This structure contains the RFBandInfo response parameters.
 *
 *  @param  radioInterface
 *          - Radio interface technology
 *              - See @ref Tables for Radio Interface
 *
 *  @param  activeBandClass
 *          - Active Band Class
 *              - See @ref Tables for Band Classes
 *
 *  @param  activeChannel
 *          - Active channel (0 if channel is not relevant to the
 *            reported technology)
 */
struct nas_RFBandInfoElements
{
    uint8_t  radioInterface;
    uint16_t activeBandClass;
    uint16_t activeChannel;
};

/**
 * This structure contains the RF Dedicated Band Info.
 *
 *  @param  radioInterface
 *          - Radio interface technology
 *              - See @ref Tables for Radio Interface
 *
 *  @param  dedicatedBand
 *          - Dedicated Band Class
 *              - See @ref Tables for Band Classes
 */
struct nas_RfDedicatedBandInfoElements
{
    uint8_t  radioInterface;
    uint16_t dedicatedBand;
};
/**
 * This structure contains the RF Band Information, Extended Format.
 *
 *  @param  radioInterface
 *          - Radio interface technology
 *              - See @ref Tables for Radio Interface
 *
 *  @param  activeBand
 *          - Active Band Class
 *              - See @ref Tables for Band Classes
 *
 *  @param  activeChannel
 *          - Active channel (0 if channel is not relevant to the
 *            reported technology)
 */
struct nas_RfBandInfoExtFormatElements
{
    uint8_t  radioInterface;
    uint16_t activeBand;
    uint32_t activeChannel;
};

/**
 * This structure contains the RF Bandwidth Information List parameters.
 *
 *  @param  radioInterface
 *          - Radio interface technology
 *              - See @ref Tables for Radio Interface
 *
 *  @param  bandwidth
 *          - Bandwidth
 *              - Values
 *                - NAS_LTE_BW_NRB_6 (0)     - LTE 1.4 MHz bandwidth
 *                - NAS_LTE_BW_NRB_15 (1)    - LTE 3 MHz bandwidth
 *                - NAS_LTE_BW_NRB_25 (2)    - LTE 5 MHz bandwidth
 *                - NAS_LTE_BW_NRB_50 (3)    - LTE 10 MHz bandwidth
 *                - NAS_LTE_BW_NRB_75 (4)    - LTE 15 MHz bandwidth
 *                - NAS_LTE_BW_NRB_100 (5)   - LTE 20 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_6 (6)    - NR5G 1.4 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_15 (7)   - NR5G 3 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_25 (8)   - NR5G 5 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_50 (9)   - NR5G 10 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_75 (10)  - NR5G 15 MHz bandwidth
 *                - NAS_NR5G_BW_NRB_100 (11) - NR5G 20 MHz bandwidth
 */
struct nas_RfBandwidthInfoElements
{
    uint8_t  radioInterface;
    uint32_t bandwidth;
};

/**
 * This structure contains the RfBandInfoList response parameters.
 *
 *  @param  pInstanceSize[IN/OUT]
 *          - Number of sets of the RFBandInfoElements
 *          - Upon input, maximum number of elements that the RF info instances
 *            array can contain.
 *          - Upon successful output, actual number of elements in RF info
 *            instances array.
 *
 *  @param  pRFBandInfoParam
 *              - See @ref nas_RFBandInfoElements for more information
 */
typedef struct{
    uint8_t *pInstanceSize;
    struct nas_RFBandInfoElements *pRFBandInfoParam;
}nas_RfBandInfoList;

/**
 * This structure contains the nas_RfDedicatedBandInfo response parameters.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 *  @param  pInstancesSize[IN/OUT]
 *          - Number of sets of the RFBandInfoElements
 *          - Upon input, maximum number of elements that the RF info instances
 *            array can contain.
 *          - Upon successful output, actual number of elements in RF info
 *            instances array.
 *
 *  @param  pRfDedicatedBandInfoParam
 *              - See @ref nas_RfDedicatedBandInfoElements for more information
 */
typedef struct{
    uint8_t  TlvPresent;
    uint8_t *pInstancesSize;
    struct nas_RfDedicatedBandInfoElements *pRfDedicatedBandInfoParam;
}nas_RfDedicatedBandInfo;

/**
 * This structure contains the nas_RfBandInfoExtFormat response parameters.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 *  @param  pInstancesSize[IN/OUT]
 *          - Number of sets of the RFBandInfoElements
 *          - Upon input, maximum number of elements that the RF info instances
 *            array can contain.
 *          - Upon successful output, actual number of elements in RF info
 *            instances array.
 *
 *  @param  pRfBandInfoExtFormatParam
 *              - See @ref nas_RfBandInfoExtFormatElements for more information
 */
typedef struct{
    uint8_t  TlvPresent;
    uint8_t *pInstancesSize;
    struct nas_RfBandInfoExtFormatElements *pRfBandInfoExtFormatParam;
}nas_RfBandInfoExtFormat;

/**
 * This structure contains the nas_RfBandwidthInfo response parameters.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 *  @param  pInstancesSize[IN/OUT]
 *          - Number of sets of the RFBandInfoElements
 *          - Upon input, maximum number of elements that the RF info instances
 *            array can contain.
 *          - Upon successful output, actual number of elements in RF info
 *            instances array.
 *
 *  @param  pRfBandwidthInfoParam
 *              - See @ref nas_RfBandwidthInfoElements for more information
 */
typedef struct{
    uint8_t  TlvPresent;
    uint8_t *pInstancesSize;
    struct nas_RfBandwidthInfoElements *pRfBandwidthInfoParam;
}nas_RfBandwidthInfo;

/**
 * This structure contains the nas_LTEOperationMode response parameters.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 *  @param  pLTEOperationMode
 *              - LTE Operational Mode
 *
 */
typedef struct{
    uint8_t  TlvPresent;
    uint32_t *pLTEOperationMode;
}nas_LTEOperationMode;

/**
 *  This structure contains the SLQSNasGetRFInfo response parameters.
 *
 *  @param  rfbandInfoList
 *          - See @ref nas_RfBandInfoList for more information
 *
 *  @param  pRfDedicatedBandInfo [Optional]
 *          - See @ref nas_RfDedicatedBandInfo for more information
 *
 *  @param  pRfBandInfoExtFormat [Optional]
 *          - See @ref nas_RfBandInfoExtFormat for more information
 *
 *  @param  pRfBandwidthInfo [Optional]
 *          - See @ref nas_RfBandwidthInfo for more information
 *
 *  @param  pLTEOprationMode [Optional]
 *          - See @ref nas_LTEOperationMode for more information
 */
typedef struct
{
    nas_RfBandInfoList            rfbandInfoList;
    nas_RfDedicatedBandInfo      *pRfDedicatedBandInfo;
    nas_RfBandInfoExtFormat      *pRfBandInfoExtFormat;
    nas_RfBandwidthInfo          *pRfBandwidthInfo;
    nas_LTEOperationMode         *pLTEOperationMode;
    uint16_t Tlvresult;
} unpack_nas_SLQSNasGetRFInfo_t;

/**
 * Gets radio band/channel information pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNasGetRFInfo(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Gets radio band/channel information unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasGetRFInfo(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasGetRFInfo_t *pOutput
        );

/**
 *  Contain the emergency Mode parameters for system selection preferences.
 *
 *  @param  EmerMode
 *          - Emergency Mode
 *          - Values:
 *              - 0 - OFF (normal)
 *              - 1 - ON (Emergency)
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint8_t       EmerMode;
    uint8_t       TlvPresent;
}nas_EmerModeTlv;

/**
 *  Contain the radio technology mode preference
 *
 *  @param  ModePref
 *          - Bit Mask indicating the radio technology mode preference
 *          - Bit values:
 *              - Bit 0 - cdma2000 1x
 *              - Bit 1 - cdma2000 HRPD(1xEV-DO)
 *              - Bit 2 - GSM
 *              - Bit 3 - UMTS
 *              - Bit 4 - LTE
 *              - Bit 5 - TD-SCDMA
 *              - Bit 6 - NR5G
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint16_t      ModePref;
    uint8_t       TlvPresent;
}nas_ModePrefTlv;

/**
 *  Contain the band preference for system selection preferences.
 *
 *  @param  BandPref
 *          - Bitmask representing the radio technologies that are disabled.
 *          - Bit values:
 *          - Bit mask representing the band preference
 *          - Bit values:
 *              - Bit 0 - Band Class 0, A-System
 *              - Bit 1 - Band Class 0, B-System, Band Class 0
 *                        AB, GSM 850 Band
 *              - Bit 2 - Band Class 1, all blocks
 *              - Bit 3 - Band Class 2 place holder
 *              - Bit 4 - Band Class 3, A-System
 *              - Bit 5 - Band Class 4, all blocks
 *              - Bit 6 - Band Class 5, all blocks
 *              - Bit 7 - GSM_DCS_1800 band
 *              - Bit 8 - GSM Extended GSM (E-GSM) 900 band
 *              - Bit 9 - GSM Primary GSM (P-GSM) 900 band
 *              - Bit 10 - Band Class 6
 *              - Bit 11 - Band Class 7
 *              - Bit 12 - Band Class 8
 *              - Bit 13 - Band Class 9
 *              - Bit 14 - Band Class 10
 *              - Bit 15 - Band Class 11
 *              - Bit 16 - GSM 450 band
 *              - Bit 17 - GSM 480 band
 *              - Bit 18 - GSM 750 band
 *              - Bit 19 - GSM 850 band
 *              - Bit 20 - GSM Railways GSM 900 Band
 *              - Bit 21 - GSM PCS 1900 band
 *              - Bit 22 - WCDMA Europe, Japan, and China IMT 2100 band
 *              - Bit 23 - WCDMA U.S. PCS 1900 band
 *              - Bit 24 - WCDMA Europe and China DCS 1800 band
 *              - Bit 25 - WCDMA U.S. 1700 band
 *              - Bit 26 - WCDMA U.S. 850 band
 *              - Bit 27 - WCDMA Japan 800 band
 *              - Bit 28 - Band Class 12
 *              - Bit 29 - Band Class 14
 *              - Bit 30 - Reserved
 *              - Bit 31 - Band Class 15
 *              - Bit 32 to 47 - Reserved
 *              - Bit 48 - WCDMA Europe 2600 band
 *              - Bit 49 - WCDMA Europe and Japan 900 band
 *              - Bit 50 - WCDMA Japan 1700 band
 *              - Bit 51 to 55 - Reserved
 *              - Bit 56 - Band Class 16
 *              - Bit 57 - Band Class 17
 *              - Bit 58 - Band Class 18
 *              - Bit 59 - Band Class 19
 *              - Bit 60 to 64 - Reserved
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t      BandPref;
    uint8_t       TlvPresent;
}nas_BandPrefTlv;

/**
 *  Contain the PRL preference for system selection preferences.
 *
 *  @param  PRLPref
 *          - Parameter indicating the CDMA PRL Preference
 *          - Values:
 *              - 0x0001 - Acquire available system only on the A side
 *              - 0x0002 - Acquire available system only on the B side
 *              - 0x3FFF - Acquire any available systems
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint16_t      PRLPref;
    uint8_t       TlvPresent;
}nas_PRLPrefTlv;

/**
 *  Contain the roaming preference for system selection preferences.
 *
 *  @param  RoamPref
 *          - Parameter indicating the roaming Preference
 *          - Values:
 *              - 0x01 - Acquire only systems for which the roaming indicator
 *                       is off
 *              - 0x02 - Acquire a system as long as its roaming indicator is
 *                       not off
 *              - 0x03 - Acquire only systems for which the roaming indicator
 *                       is off or solid on, i.e. not flashing; CDMA only
 *              - 0xFF - Acquire systems, regardless of their roaming indicator
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint16_t      RoamPref;
    uint8_t       TlvPresent;
}nas_RoamPrefTlv;

/**
 *  Contain the LTE Band Preference for system selection preferences.
 *
 *  @param  LTEBandPref
 *          - Bit mask representing the LTE band preference
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - All other bits are reserved
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t      LTEBandPref;
    uint8_t       TlvPresent;
}nas_LTEBandPrefTlv;

/**
 *  Contain the Network selection Preference for system selection preferences.
 *
 *  @param  NetSelPref
 *          - Parameter indicating network selection preference
 *          - Values:
 *              - 0x00 - Automatic network selection
 *              - 0x01 - Manual network selection
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint8_t       NetSelPref;
    uint8_t       TlvPresent;
}nas_NetSelPrefTlv;

/**
 *  Contain the Service domain Preference for system selection preferences.
 *
 *  @param  SrvDomainPref
 *          - Parameter indicating Service domain preference
 *          - Values:
 *              - 0x00 - Circuit switched only
 *              - 0x01 - Packet switched only
 *              - 0x02 - Circuit switched and packet switched
 *              - 0x03 - Packet switched attach
 *              - 0x04 - Packet switched detach
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint32_t       SrvDomainPref;
    uint8_t        TlvPresent;
}nas_SrvDomainPrefTlv;

/**
 *  Contain the GSM/WCDMA Acquisition order preference for system selection preferences.
 *
 *  @param  GWAcqOrderPref
 *          - Parameter indicating GSM/WCDMA Acquisition order Preference
 *          - Values:
 *              - 0x00 - Automatic
 *              - 0x01 - GSM then WCDMA
 *              - 0x02 - WCDMA then GSM
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint32_t       GWAcqOrderPref;
    uint8_t        TlvPresent;
}nas_GWAcqOrderPrefTlv;

/**
 *  Contain the Acquisition Order Preference.
 *
 *  @param  acqOrdeLen
 *          - Number of sets of the following elements.
 *
 *  @param  pAcqOrder
 *          - Acquisition order preference to be set. Values:
 *            - 0x01 - NAS_RADIO_IF_CDMA_1X - cdma2000 1X
 *            - 0x02 - NAS_RADIO_IF_CDMA_1XEVDO - cdma2000 HRPD (1xEV-DO)
 *            - 0x04 - NAS_RADIO_IF_GSM - GSM
 *            - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *            - 0x08 - NAS_RADIO_IF_LTE - LTE
 *            - 0x09 - NAS_RADIO_IF_TDSCDMA - TD-SCDMA
 *            - 0x0C - NAS_RADIO_IF_NR5G - NR5G
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint8_t   acqOrdeLen;
    uint8_t   *pAcqOrder;
    uint8_t   TlvPresent;
}nas_AcqOrderPrefTlv;

/**
 *  Contain the RAT disabled mask
 *
 *  @param  ratDisabledMask
 *          - Bitmask representing the radio technologies that are disabled.
 *          - Bit values:
 *            - Bit 0 (0x01) - cdma2000 &reg; 1X
 *            - Bit 1 (0x02) - cdma2000 &reg; HRPD(1xEV-DO)
 *            - Bit 2 (0x04) - GSM
 *            - Bit 3 (0x08) - UMTS
 *            - Bit 4 (0x10) - LTE
 *            - Bit 5 (0x20) - TD-SCDMA
 *            - Bit 6 (0x40) - NR5G
 *            All unlisted bits are reserved for future use and the service point
 *            ignores them if used.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint16_t      ratDisabledMask;
    uint8_t       TlvPresent;
}nas_RatDisabledMaskTlv;

/**
 *  Contain the CIOT LTE Operational Mode Preference.
 *
 *  @param  ciotLteOpModePref
 *          - CIOT LTE Operational Mode Preference
 *          - Values:
 *              - Bit 0 (0x01) - LTE wideband
 *              - Bit 1 (0x02) - LTE M1
 *              - Bit 2 (0x04) - LTE NB1
 *              All unlisted bits are reserved for future use and
 *              the service point ignores them if used.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t     ciotLteOpModePref;
    uint8_t      TlvPresent;
}nas_CiotLteOpModePrefTlv;

/**
 *  Contain the LTE M1 band Preference.
 *
 *  @param  lteM1BandPref
 *          - Bitmask representing the LTE M1 band preference
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t     lteM1BandPref;
    uint8_t      TlvPresent;
}nas_LteM1BandPrefTlv;

/**
 *  Contain the LTE NB1 band Preference.
 *
 *  @param  lteNb1BandPref
 *          - Bitmask representing the LTE NB1 band preference.
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t     lteNb1BandPref;
    uint8_t      TlvPresent;
}nas_LteNb1BandPrefTlv;

/**
 *  Contain the CIOT Acquisition Order Preference.
 *
 *  @param  ciotAcqOrderLen
 *          - Number of sets of the following elements.
 *
 *  @param  pCiotAcqOrder
 *          - CIOT Acquisition order preference to be set.
 *          - Values
 *            - 0x01 - cdma2000 &reg;1X
 *            - 0x02 - cdma2000 &reg; HRPD (1xEV-DO)
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *            - 0x0a - LTE_M1
 *            - 0x0b - LTE_NB1
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint8_t     ciotAcqOrderLen;
    uint32_t    *pCiotAcqOrder;
    uint8_t     TlvPresent;
}nas_CiotAcqOrderPrefTlv;

/**
 *  Contain the Band Preference.
 *
 *  @param  bits_1_64
 *          - Bits 1 to 64 of the 256-bit Operating Band bitmask.
 *
 *  @param  bits_65_128
 *          - Bits 65 to 128 of the 256-bit Operating Band bitmask.
 *
 *  @param  bits_129_192
 *          - Bits 129 to 192 of the 256-bit Operating Band bitmask.
 *
 *  @param  bits_193_256
 *          - Bits 193 to 256 of the 256-bit Operating Band bitmask.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t    bits_1_64;
    uint64_t    bits_65_128;
    uint64_t    bits_129_192;
    uint64_t    bits_193_256;
    uint8_t     TlvPresent;
}nas_BandPrefInfoTlv;

/**
 *  Contain the NR5G Band Preference.
 *
 *    - See \ref nas_BandPrefInfoTlv for more information.
 *
 */
typedef nas_BandPrefInfoTlv nas_NR5gBandPrefTlv;


/**
 *  Contain the LTE Band Preference Extended.
 *
 *    - See \ref nas_BandPrefInfoTlv for more information.
 *
 */
typedef nas_BandPrefInfoTlv nas_LTEBandPrefExtTlv;

/**
 *  Structure for storing the current preferred system selection settings
 *  for the device.
 *
 *  @param  pEmerMode
 *          - Optional parameter specifying the emergency Mode
 *          - See @ref nas_EmerModeTlv for more information
 *
 *  @param  pModePref
 *          - Optional parameterindicating the radio technology mode preference
 *          - See @ref nas_ModePrefTlv for more information
 *
 *  @param  pBandPref
 *          - Optional parameter representing the band preference
 *          - See @ref nas_BandPrefTlv for more information
 *
 *  @param  pPRLPref
 *          - Optional parameter indicating the CDMA PRL Preference
 *          - See @ref nas_PRLPrefTlv for more information
 *
 *  @param  pRoamPref
 *          - Optional parameter indicating the roaming Preference
 *          - See @ref nas_RoamPrefTlv for more information
 *
 *  @param  pLTEBandPref
 *          - Optional parameter representing the LTE band preference
 *          - See @ref nas_LTEBandPrefTlv for more information
 *
 *  @param  pNetSelPref
 *          - Optional parameter indicating network selection preference
 *          - See @ref nas_NetSelPrefTlv for more information
 *
 *  @param  pSrvDomainPref
 *          - Optional parameter indicating Service domain preference
 *          - See @ref nas_SrvDomainPrefTlv for more information
 *
 *  @param  pGWAcqOrderPref
 *          - Optional parameter indicating GSM/WCDMA Acquisition order Preference
 *          - See @ref nas_GWAcqOrderPrefTlv for more information
 *
 *  @param  pAcqOrderPref
 *          - Optional parameter
 *          - Acquisition Order Preference
 *          - See @ref nas_AcqOrderPrefTlv for more information
 *
 *  @param  pRatDisabledMask
 *          - Optional parameter representing the radio technologies that are disabled.
 *          - See @ref nas_RatDisabledMaskTlv for more information
 *
 *  @param  pCiotLteOpModePref
 *          - Optional parameter
 *          - CIOT LTE Operational Mode Preference
 *          - See @ref nas_CiotLteOpModePrefTlv for more information
 *
 *  @param  pLteM1BandPref
 *          - Optional parameter representing the LTE M1 band preference
 *          - See @ref nas_LteM1BandPrefTlv for more information
 *
 *  @param  pLteNb1BandPref
 *          - Optional parameter representing the LTE NB1 band preference.
 *          - See @ref nas_LteNb1BandPrefTlv for more information
 *
 *  @param  pCiotAcqOrderPref
 *          - Optional parameter indicating CIOT Acquisition Order Preference
 *          - See @ref nas_CiotAcqOrderPrefTlv for more information
 *
 *  @param  pNr5gBandPref
 *          - Optional parameter indicating NR5G Band Preference
 *          - See @ref nas_NR5gBandPrefTlv for more information
 *
 *  @param  pLTEBandPrefExt
 *          - Optional parameter indicating LTE Band Preference Extended
 *          - See @ref nas_LTEBandPrefExtTlv for more information
 *
 */
typedef struct
{
    nas_EmerModeTlv          *pEmerMode;
    nas_ModePrefTlv          *pModePref;
    nas_BandPrefTlv          *pBandPref;
    nas_PRLPrefTlv           *pPRLPref;
    nas_RoamPrefTlv          *pRoamPref;
    nas_LTEBandPrefTlv       *pLTEBandPref;
    nas_NetSelPrefTlv        *pNetSelPref;
    nas_SrvDomainPrefTlv     *pSrvDomainPref;
    nas_GWAcqOrderPrefTlv    *pGWAcqOrderPref;
    nas_AcqOrderPrefTlv      *pAcqOrderPref;
    nas_RatDisabledMaskTlv   *pRatDisabledMask;
    nas_CiotLteOpModePrefTlv *pCiotLteOpModePref;
    nas_LteM1BandPrefTlv     *pLteM1BandPref;
    nas_LteNb1BandPrefTlv    *pLteNb1BandPref;
    nas_CiotAcqOrderPrefTlv  *pCiotAcqOrderPref;
    nas_NR5gBandPrefTlv      *pNr5gBandPref;
    nas_LTEBandPrefExtTlv    *pLTEBandPrefExt;
} unpack_nas_SLQSGetSysSelectionPrefExt_t;

/**
 * Queries the different system selection preferences of the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSysSelectionPrefExt(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Queries the different system selection preferences of the device unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSysSelectionPrefExt(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSysSelectionPrefExt_t *pOutput
        );

/**
 *  Contain the TDSCDMA Band Preference.
 *
 *  @param  TdscdmaBandPref
 *          - Bitmask representing the TD-SCDMA band preference to be set.
 *          - Values
 *            - 0x01 - NAS_TDSCDMA_BAND_A - TD-SCDMA Band A
 *            - 0x02 - NAS_TDSCDMA_BAND_B - TD-SCDMA Band B
 *            - 0x04 - NAS_TDSCDMA_BAND_C - TD-SCDMA Band C
 *            - 0x08 - NAS_TDSCDMA_BAND_D - TD-SCDMA Band D
 *            - 0x10 - NAS_TDSCDMA_BAND_E - TD-SCDMA Band E
 *            - 0x20 - NAS_TDSCDMA_BAND_F - TD-SCDMA Band F
 *            - All other bits are reserved.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint64_t     TdscdmaBandPref;
    uint8_t      TlvPresent;
}nas_TDSCDMABandPrefTlv;

/**
 *  Contain the Network Selection Registration Restriction Preference.
 *
 *  @param  SrvRegRestriction
 *          - Registration restriction preference.
 *          - Values
 *            - 0x00 - Device follows the normal registration process
 *            - 0x01 - Device camps on the network according to its provisioning,
 *                     but does not register
 *            - 0x02 - Device selects the network for limited service
 *            - All other values are reserved.
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint32_t     SrvRegRestriction;
    uint8_t      TlvPresent;
}nas_SrvRegRestricTlv;

/**
 *  Contain the Usage Preference info.
 *
 *  @param  UsageSetting
 *          - Modem usage preference to be set.
 *          - Values
 *            - 0 - Unknown
 *            - 1 - Voice centric
 *            - 2 - Data centric
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint32_t     UsageSetting;
    uint8_t      TlvPresent;
}nas_UsageSettingTlv;

/**
 *  Contain the Voice Domain Preference Info.
 *
 *  @param  VoiceDomainPref
 *          - Voice domain preference.
 *          - Values
 *            - 0x00 - Circuit-switched (CS) voice only
 *            - 0x01 - Packet-switched (PS) voice only
 *            - 0x02 - CS is preferred; PS is secondary
 *            - 0x03 - PS is preferred; CS is secondary
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct
{
    uint32_t     VoiceDomainPref;
    uint8_t      TlvPresent;
}nas_VoiceDomainPrefTlv;

/**
 *  Structure for storing the current preferred system selection settings
 *  for the device.
 *
 *  @param  pEmerMode
 *          - Optional parameter specifying the emergency Mode
 *          - See @ref nas_EmerModeTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pModePref
 *          - Optional parameterindicating the radio technology mode preference
 *          - See @ref nas_ModePrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pBandPref
 *          - Optional parameter representing the band preference
 *          - See @ref nas_BandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  pPRLPref
 *          - Optional parameter indicating the CDMA PRL Preference
 *          - See @ref nas_PRLPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  @param  pRoamPref
 *          - Optional parameter indicating the roaming Preference
 *          - See @ref nas_RoamPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  pLTEBandPref
 *          - Optional parameter representing the LTE band preference
 *          - See @ref nas_LTEBandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *  @param  pNetSelPref
 *          - Optional parameter indicating network selection preference
 *          - See @ref nas_NetSelPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *  @param  pSrvDomainPref
 *          - Optional parameter indicating Service domain preference
 *          - See @ref nas_SrvDomainPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *  @param  pGWAcqOrderPref
 *          - Optional parameter indicating GSM/WCDMA Acquisition order Preference
 *          - See @ref nas_GWAcqOrderPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *  @param  pAcqOrderPref
 *          - Optional parameter
 *          - Acquisition Order Preference
 *          - See @ref nas_AcqOrderPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *  @param  pRatDisabledMask
 *          - Optional parameter representing the radio technologies that are disabled.
 *          - See @ref nas_RatDisabledMaskTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pCiotLteOpModePref
 *          - Optional parameter
 *          - CIOT LTE Operational Mode Preference
 *          - See @ref nas_CiotLteOpModePrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  pLteM1BandPref
 *          - Optional parameter representing the LTE M1 band preference
 *          - See @ref nas_LteM1BandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  pLteNb1BandPref
 *          - Optional parameter representing the LTE NB1 band preference.
 *          - See @ref nas_LteNb1BandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  pCiotAcqOrderPref
 *          - Optional parameter indicating CIOT Acquisition Order Preference
 *          - See @ref nas_CiotAcqOrderPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  pNr5gBandPref
 *          - Optional parameter indicating NR5G Band Preference
 *          - See @ref nas_NR5gBandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *  @param  pLTEBandPrefExt
 *          - Optional parameter indicating LTE Band Preference Extended
 *          - See @ref nas_LTEBandPrefExtTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pTDSCDMABandPref
 *          - Optional parameter indicating TDSCDMA Preference
 *          - See @ref nas_TDSCDMABandPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *  @param  pSrvRegRestric
 *          - Optional parameter indicating SRV Registration Restriction
 *          - See @ref nas_SrvRegRestricTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *  @param  pUsageSetting
 *          - Optional parameter indicating Usage settings
 *          - See @ref nas_UsageSettingTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *  @param  pVoiceDomainPref
 *          - Optional parameter indicating Voice Domain Preference
 *          - See @ref nas_VoiceDomainPrefTlv for more information
 *          - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_EmerModeTlv          *pEmerMode;
    nas_ModePrefTlv          *pModePref;
    nas_BandPrefTlv          *pBandPref;
    nas_PRLPrefTlv           *pPRLPref;
    nas_RoamPrefTlv          *pRoamPref;
    nas_LTEBandPrefTlv       *pLTEBandPref;
    nas_NetSelPrefTlv        *pNetSelPref;
    nas_SrvDomainPrefTlv     *pSrvDomainPref;
    nas_GWAcqOrderPrefTlv    *pGWAcqOrderPref;
    nas_AcqOrderPrefTlv      *pAcqOrderPref;
    nas_RatDisabledMaskTlv   *pRatDisabledMask;
    nas_CiotLteOpModePrefTlv *pCiotLteOpModePref;
    nas_LteM1BandPrefTlv     *pLteM1BandPref;
    nas_LteNb1BandPrefTlv    *pLteNb1BandPref;
    nas_CiotAcqOrderPrefTlv  *pCiotAcqOrderPref;
    nas_NR5gBandPrefTlv      *pNr5gBandPref;
    nas_LTEBandPrefExtTlv    *pLTEBandPrefExt;
    nas_TDSCDMABandPrefTlv   *pTDSCDMABandPref;
    nas_SrvRegRestricTlv     *pSrvRegRestric;
    nas_UsageSettingTlv      *pUsageSetting;
    nas_VoiceDomainPrefTlv   *pVoiceDomainPref;
    swi_uint256_t             ParamPresenceMask;
} unpack_nas_SLQSGetSysSelectionPrefExtV2_t;

/**
 * Queries the different system selection preferences of the device unpack.
 *
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSGetSysSelectionPrefExtV2(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSGetSysSelectionPrefExtV2_t *pOutput
        );

/**
 * Queries the different system selection preferences of the device pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSGetSysSelectionPrefExtV2(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 *  Contain the network scan information.
 *
 *  \param  pNetworkType
 *          - network type.
 *          - Bit 0 - GSM.
 *          - Bit 1 - UMTS.
 *          - Bit 2 - LTE.
 *          - Bit 3 - TD-SCDMA.
 *
 *  \param  pScanType
 *          - Scan type.
 *          - 0x00 - PLMN(default).
 *          - 0x01 - Closed subscriber group.
 *          - 0x02 - Mode prference.
 *          - 0x03 - Physical cell ID.
 *
 *  \param  pCiotOpModePref[IN]
 *          - CIOT LTE Operational Mode Preference
 *          - Bitmask representing the Cellular Internet of Things
 *            (CIoT) LTE operational mode preference.
 *          - Values:
 *             - Bit 0 (0x01) - QMI_NAS_CIOT_LTE_OP_MODE_PREF_WB - LTE wideband (legacy LTE)
 *             - Bit 1 (0x02) - QMI_NAS_CIOT_LTE_OP_MODE_PREF_M1 - LTE M1
 *             - Bit 2 (0x04) - QMI_NAS_CIOT_LTE_OP_MODE_PREF_NB1 - LTE NB1
 *             - All unlisted bits are reserved for future use and the service point
 *               ignores them if used.
 *
 *  \param  pLteM1BandPref[IN]
 *          - Bitmask representing the LTE M1 band preferencet.
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 *
 *  \param  pLteNb1BandPref[IN]
 *          - Bitmask representing the LTE NB1 band preference.
 *          - Bit Values
 *              - Bit 0 - E-UTRA Operating Band 1
 *              - Bit 1 - E-UTRA Operating Band 2
 *              - Bit 2 - E-UTRA Operating Band 3
 *              - Bit 3 - E-UTRA Operating Band 4
 *              - Bit 4 - E-UTRA Operating Band 5
 *              - Bit 5 - E-UTRA Operating Band 6
 *              - Bit 6 - E-UTRA Operating Band 7
 *              - Bit 7 - E-UTRA Operating Band 8
 *              - Bit 8 - E-UTRA Operating Band 9
 *              - Bit 9 - E-UTRA Operating Band 10
 *              - Bit 10 - E-UTRA Operating Band 11
 *              - Bit 11 - E-UTRA Operating Band 12
 *              - Bit 12 - E-UTRA Operating Band 13
 *              - Bit 13 - E-UTRA Operating Band 14
 *              - Bit 16 - E-UTRA Operating Band 17
 *              - Bit 17 - E-UTRA Operating Band 18
 *              - Bit 18 - E-UTRA Operating Band 19
 *              - Bit 19 - E-UTRA Operating Band 20
 *              - Bit 20 - E-UTRA Operating Band 21
 *              - Bit 22 - E-UTRA Operating Band 23
 *              - Bit 23 - E-UTRA Operating Band 24
 *              - Bit 24 - E-UTRA Operating Band 25
 *              - Bit 25 - E-UTRA Operating Band 26
 *              - Bit 27 - E-UTRA Operating Band 28
 *              - Bit 28 - E-UTRA Operating Band 29
 *              - Bit 29 - E-UTRA Operating Band 32
 *              - Bit 32 - E-UTRA Operating Band 33
 *              - Bit 33 - E-UTRA Operating Band 34
 *              - Bit 34 - E-UTRA Operating Band 35
 *              - Bit 35 - E-UTRA Operating Band 36
 *              - Bit 36 - E-UTRA Operating Band 37
 *              - Bit 37 - E-UTRA Operating Band 38
 *              - Bit 38 - E-UTRA Operating Band 39
 *              - Bit 39 - E-UTRA Operating Band 40
 *              - Bit 40 - E-UTRA Operating Band 41
 *              - Bit 41 - E-UTRA Operating Band 42
 *              - Bit 42 - E-UTRA Operating Band 43
 *              - Bit 60 - E-UTRA Operating Band 125
 *              - Bit 61 - E-UTRA Operating Band 126
 *              - Bit 62 - E-UTRA Operating Band 127
 */

typedef struct
{
    uint8_t                     *pNetworkType;
    uint32_t                    *pScanType;
    uint64_t                    *pCiotOpModePref;
    uint64_t                    *pLteM1BandPref;
    uint64_t                    *pLteNB1BandPref;
} pack_nas_PerformNetworkScanPCI_t;

/**
 * Pack performs scan for available networks and 
 * scans for LTE RAT info (specific Network or Scan Type).
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request prarmeters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: MC75xx, EM75xx and WP76xx.
 */
int pack_nas_PerformNetworkScanPCI(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_nas_PerformNetworkScanPCI_t *pReqParam
        );

/**
 * This structure contains the parameters for eDRX Enabled Indicator Type.
 *
 *  @param  edrxEnabled
 *          - Values
 *            - 0x00 - Disabled
 *            - 0x01 - Enabled
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t edrxEnabled;
    uint8_t TlvPresent;
}nas_EdrxEnableType;

/**
 * This structure contains the parameters for eDRX Cycle Length Indicator Type.
 *
 *  @param  cycleLength
 *          - eDRX cycle length. Value is per 3GPP TS 24.008 Rel-13 Section 10.5.5.32.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t cycleLength;
    uint8_t TlvPresent;
}nas_EdrxCycleLength;

/**
 * This structure contains the parameters for eDRX Paging Time Window Indicator Type.
 *
 *  @param  edrxPtw
 *          - eDRX paging time window. Value is per 3GPP TS 24.008 Rel-13 Section 10.5.5.32
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t edrxPtw;
    uint8_t TlvPresent;
}nas_EdrxPagingTimeWindow;

/**
 * This structure contains the parameters for eDRX RAT type Indicator Type.
 *
 *  @param  edrxRatType
 *          - Radio access technology
 *          - Values
 *            - NAS_RADIO_IF_NO_SVC (0x00) - None (no service)
 *            - NAS_RADIO_IF_CDMA_1X (0x01) - cdma2000 &reg; 1X
 *            - NAS_RADIO_IF_CDMA_1XEVDO (0x02) - cdma2000 &reg; HRPD (1xEV-DO)
 *            - NAS_RADIO_IF_AMPS (0x03) - AMPS
 *            - NAS_RADIO_IF_GSM (0x04) - GSM
 *            - NAS_RADIO_IF_UMTS (0x05) - UMTS
 *            - NAS_RADIO_IF_WLAN (0x06) - WLAN
 *            - NAS_RADIO_IF_GPS (0x07) - GPS
 *            - NAS_RADIO_IF_LTE (0x08) - LTE
 *            - NAS_RADIO_IF_TDSCDMA (0x09) - TD-SCDMA
 *            - NAS_RADIO_IF_LTE_M1 (0x0a) - LTE-M1
 *            - NAS_RADIO_IF_LTE_NB1 (0x0b) - LTE-NB1
 *            - NAS_RADIO_IF_NO_CHANGE (-1) - No change
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint8_t edrxRatType;
    uint8_t TlvPresent;
}nas_EdrxRatType;

/**
 * This structure contains the parameters for LTE Operational Mode Indicator Type.
 *
 *  @param  lteOpMode
 *          - CIOT LTE mode.
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_NB1 (0x03) - Camped on LTE NB1
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 */
typedef struct{
    uint32_t lteOpMode;
    uint8_t  TlvPresent;
}nas_EdrxCiotLteMode;

/**
 *  Structure for storing the eDRX Change Info indication parameters.
 *
 *  \param  edrxEnableTypeTlv
 *          - See \ref nas_EdrxEnableType for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  edrxCycleLengthTlv
 *          - See \ref nas_EdrxCycleLength for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  edrxPagingTimeWindowTlv
 *          - See \ref nas_EdrxPagingTimeWindow for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  \param  edrxRatTypeTlv
 *          - See \ref nas_EdrxRatType for more information.
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *  \param  edrxCiotLteModeTlv
 *          - See \ref nas_EdrxCiotLteMode for more information.
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_EdrxEnableType          edrxEnableTypeTlv;
    nas_EdrxCycleLength         edrxCycleLengthTlv;
    nas_EdrxPagingTimeWindow    edrxPagingTimeWindowTlv;
    nas_EdrxRatType             edrxRatTypeTlv;
    nas_EdrxCiotLteMode         edrxCiotLteModeTlv;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t;

/**
 * unpack nas eDRX change Info indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasEdrxChangeInfoCallBack_Ind_t *pOutput
        );

typedef enum
{
    LITE_SYS_SRV_DOMAIN_NO_SRV  = 0x00,
    LITE_SYS_SRV_DOMAIN_CS_ONLY = 0x01,
    LITE_SYS_SRV_DOMAIN_PS_ONLY = 0x02,
    LITE_SYS_SRV_DOMAIN_CS_PS   = 0x03,
    LITE_SYS_SRV_DOMAIN_CAMPED  = 0x04
}LITE_TYPE_OF_SERVICE_DOMAIN;

/**
 *  This structure contains csg ID.
 *
 *  @param  csgId
 *          - Closed subscriber group identifier.
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t csgId;
    uint8_t TlvPresent;
}nas_CsgId;

/**
 *  This structure contains PLMN ID information
 *
 *  @param  mcc
 *          - A 16-bit integer representation of MCC. Range: 0 to 999
 *
 *  @param  mnc
 *          - A 16-bit integer representation of MNC. Range: 0 to 999
 *
 *  @param  pcsDigit
 *          - This field is used to interpret the length of the corresponding MNC
 *            reported in the TLVs (in this table) with an mnc or mobile_network_code
 *            field. Values:
 *            - TRUE - MNC is a three-digit value; for example, a reported value of 90
 *                     corresponds to an MNC value of 090
 *            - FALSE - MNC is a two-digit value; for example, a reported value of 90
 *                     corresponds to an MNC value of 90
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint16_t mcc;
    uint16_t mnc;
    uint8_t pcsDigit;
    uint8_t TlvPresent;
}nas_PlmnID;

/**
 *  This structure contains LTE Operational Mode
 *
 *  @param  lteOpMode
 *          - LTE Operational Mode
 *          - CIOT LTE mode on which reject indication is received.
 *          - Values
 *            - NAS_CIOT_SYS_MODE_NO_SRV (0x00) - No service
 *            - NAS_CIOT_SYS_MODE_LTE_WB (0x01) - Camped on LTE wideband
 *            - NAS_CIOT_SYS_MODE_LTE_M1 (0x02) - Camped on LTE M1
 *            - NAS_CIOT_SYS_MODE_LTE_NB1 (0x03) - Camped on LTE NB1
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t lteOpMode;
    uint8_t  TlvPresent;
}nas_LteOpMode;


/**
 *  Structure for storing the network reject indication parameters.
 *
 *  @param  radioIf
 *          - Radio interface from which to get the information. Values:
 *            - 0x04 - NAS_RADIO_IF_GSM - GSM
 *            - 0x05 - NAS_RADIO_IF_UMTS - UMTS
 *            - 0x08 - NAS_RADIO_IF_LTE - LTE
 *            - 0x09 - NAS_RADIO_IF_TDSCDMA - TDSCDMA
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  serviceDomain
 *          - Type of service domain in which the registration is rejected. Values:
 *            - 0x00 - LITE_SYS_SRV_DOMAIN_NO_SRV  - No service
 *            - 0x01 - LITE_SYS_SRV_DOMAIN_CS_ONLY - Circuit-switched only
 *            - 0x02 - LITE_SYS_SRV_DOMAIN_PS_ONLY - Packet-switched only
 *            - 0x03 - LITE_SYS_SRV_DOMAIN_CS_PS   - Circuit-switched and packet-switched
 *            - 0x04 - LITE_SYS_SRV_DOMAIN_CAMPED  - Camped
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  rejectCause
 *          - Reject cause values.
 *            - 2 - IMSI unknown in HLR
 *            - 3 - Illegal MS
 *            - 4 - IMSI unknown in VLR
 *            - 5 - IMEI not accepted
 *            - 6 - Illegal ME
 *            - 7 - EPS services not allowed
 *            - 8 - EPS services and non-EPS services not allowed
 *            - 9 - UE identity cannot be derived by the network
 *            - 10 - Implicitly detached
 *            - 11 - PLMN not allowed
 *            - 12 - Location Area not allowed
 *            - 13 - Roaming not allowed in this location area
 *            - 14 - EPS services not allowed in this PLMN
 *            - 15 - No Suitable Cells In Location Area
 *            - 16 - MSC temporarily not reachable
 *            - 17 - Network failure
 *            - 18 - CS domain not available
 *            - 19 - ESM failure
 *            - 20 - MAC failure
 *            - 21 - Synch failure
 *            - 22 - Congestion
 *            - 23 - GSM authentication unacceptable
 *            - 24 - Security mode rejected, unspecified
 *            - 25 - Not authorized for this CSG
 *            - 26 - Non-EPS authentication unacceptable
 *            - 32 - Service option not supported
 *            - 33 - Requested service option not subscribed
 *            - 34 - Service option temporarily out of order
 *            - 38 - Call cannot be identified
 *            - 48 to 63 - retry upon entry into a new cell
 *            - 95 - Semantically incorrect message
 *            - 96 - Invalid mandatory information
 *            - 97 - Message type non-existent or not implemented
 *            - 98 - Message type not compatible with the protocol state
 *            - 99 - Information element non-existent or not implemented
 *            - 100 - Conditional IE error
 *            - 101 - Message not compatible with the protocol state
 *            - 111 - Protocol error, unspecified
 *            - Note - Any other value received by the mobile station shall be treated
 *                     as 34, 'Service option temporarily out of order'.
 *                   - Any other value received by the network shall be treated
 *                     as 111, 'Protocol error, unspecified'.\n
 *              See 3GPP TS 24.008, Section 4.4.4.7 and Section 10.5.3.6
 *              See \ref qaGobiApiTableCallEndReasons.h for Call End reasons
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  @param  pPlmnId (optional)
 *          - See @ref nas_PlmnID for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pCsgId (optional)
 *          - See @ref nas_CsgId for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pLteOpMode (optional)
 *          - See @ref nas_LteOpMode for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t radioIf;
    LITE_TYPE_OF_SERVICE_DOMAIN serviceDomain;
    uint8_t rejectCause;
    nas_PlmnID *pPlmnId;
    nas_CsgId *pCsgId;
    nas_LteOpMode *pLteOpMode;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSNasNetworkRejectCallback_Ind_t;

/**
 * unpack nas Network Reject Callback indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasNetworkRejectCallback_Ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasNetworkRejectCallback_Ind_t *pOutput
        );

/**
 * This structure contains the ForbiddenNetworks3GPP response parameters.
 *
 *  @param  forbiddenNwInstLen
 *              - Number of sets of the following elements
 *                - MCC
 *                - MNC
 *
 *  @param  MCC
 *          - Mobile Country Code
 *          - A 16-bit integer representation of MCC
 *          - Range: 0 to 999
 *
 *  @param  MNC
 *          - Mobile Network Code
 *          - A 16-bit integer representation of MNC
 *          - Range: 0 to 999
 *
 *  @param  TlvPresent
 *          - Tlv Present
 *
 */
typedef struct{
    uint16_t      forbiddenNwInstLen;
    uint16_t      MCC[NAS_MCC_MNC_INST_LENGTH];
    uint16_t      MNC[NAS_MCC_MNC_INST_LENGTH];
    uint8_t       TlvPresent;
}nas_ForbiddenNetworks3GPP;

/**
 *  This structure contains the SLQSNASGeteDRXParams request parameters.
 *
 *  @param  pForbiddenNetworks3GPP [Optional]
 *          - See @ref nas_ForbiddenNetworks3GPP for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    nas_ForbiddenNetworks3GPP *pForbiddenNetworks3GPP;
    swi_uint256_t             ParamPresenceMask;
} unpack_nas_SLQSNASGetForbiddenNetworks_t;

/**
 * Get  forbidden networks pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_nas_SLQSNASGetForbiddenNetworks(
        pack_qmi_t  *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Get forbidden networks unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNASGetForbiddenNetworks(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNASGetForbiddenNetworks_t *pOutput
        );

/**
 *  Contain fields in struct nas_RFBandInfoTlv
 *
 *  @param  radioIf
 *          - Radio interface currently in use.
 *          - Values
 *            - 0x01 - cdma2000 ® 1X
 *            - 0x02 - cdma2000 ® HRPD (1xEV-DO)
 *            - 0x03 - AMPS
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *            - 0x0C - NR5G
 *
 *  @param  activeBand
 *          - Active band class.
 *          - Values
 *            - 00 to 39 - CDMA band classes
 *            - 40 to 79 - GSM band classes
 *            - 80 to 91 - WCDMA band classes
 *            - 120 to 168 - LTE band classes
 *            - 200 to 205 - TD-SCDMA band classes
 *            - 250 to 251 - NR-5G band classes
 *
 *  @param  activeChannel
 *          - Active channel.
 *          - If the channel is not relevant to the technology,
 *            a value of 0 is returned.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint8_t   radioIf;
    uint16_t  activeBand;
    uint16_t  activeChannel;
    uint8_t   TlvPresent;
} nas_RFBandInfoTlv;

/**
 *  Contain fields in struct nas_RFDedicatedBandInfoTlv
 *
 *  @param  radioIf
 *          - Radio interface currently in use.
 *          - Values
 *            - 0x01 - cdma2000 ® 1X
 *            - 0x02 - cdma2000 ® HRPD (1xEV-DO)
 *            - 0x03 - AMPS
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *
 *  @param  dedicatedBand
 *          - Dedicated band class.
 *          - Values
 *            - 00 to 39 - CDMA band classes
 *            - 40 to 79 - GSM band classes
 *            - 80 to 91 - WCDMA band classes
 *            - 120 to 168 - LTE band classes
 *            - 200 to 205 - TD-SCDMA band classes
 *            - 0xFFFF - invalid - indicates that the UE moved out from the dedicated band
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint8_t  radioIf;
    uint16_t  dedicatedBand;
    uint8_t  TlvPresent;
} nas_RFDedicatedBandInfoTlv;

/**
 *  Contain fields in struct nas_RFBandInfoExtTlv
 *
 *  @param  radioIf
 *          - Radio interface currently in use.
 *          - Values
 *            - 0x01 - cdma2000 ® 1X
 *            - 0x02 - cdma2000 ® HRPD (1xEV-DO)
 *            - 0x03 - AMPS
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *            - 0x0C - NR5G
 *
 *  @param  activeBand
 *          - Active band class.
 *          - Values
 *            - 00 to 39 - CDMA band classes
 *            - 40 to 79 - GSM band classes
 *            - 80 to 91 - WCDMA band classes
 *            - 120 to 168 - LTE band classes
 *            - 200 to 205 - TD-SCDMA band classes
 *            - 250 to 251 - NR-5G band classes
 *
 *  @param  activeChannel
 *          - Active channel.
 *          - If the channel is not relevant to the technology,
 *            a value of 0 is returned.
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint8_t   radioIf;
    uint16_t   activeBand;
    uint32_t  activeChannel;
    uint8_t   TlvPresent;
} nas_RFBandInfoExtTlv;

/**
 *  Contain fields in struct nas_RFBandwidthInfoTlv
 *
 *  @param  radioIf
 *          - Radio interface currently in use.
 *          - Values
 *            - 0x00 - no service
 *            - 0x01 - cdma2000 ® 1X
 *            - 0x02 - cdma2000 ® HRPD (1xEV-DO)
 *            - 0x03 - AMPS
 *            - 0x04 - GSM
 *            - 0x05 - UMTS
 *            - 0x06 - WLAN
 *            - 0x07 - GPS
 *            - 0x08 - LTE
 *            - 0x09 - TD-SCDMA
 *            - 0x0a - LTE-M1
 *            - 0x0b - LTE-NB1
 *            - -1   - No change
 *
 *  @param  bandwidth
 *          - Bandwidth.
 *          - Values
 *            - 0 - LTE 1.4 MHz bandwidth
 *            - 1 - LTE 3 MHz bandwidth
 *            - 2 - LTE 5 MHz bandwidth
 *            - 3 - LTE 10 MHz bandwidth
 *            - 4 - LTE 15 MHz bandwidth
 *            - 5 - LTE 20 MHz bandwidth
 *            - 6 - NR5G 1.4 MHz bandwidth
 *            - 7 - NR5G 3 MHz bandwidth
 *            - 8 - NR5G 5 MHz bandwidth
 *            - 9 - NR5G 10 MHz bandwidth
 *            - 10 - NR5G 15 MHz bandwidth
 *            - 11 - NR5G 20 MHz bandwidth
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint8_t   radioIf;
    uint32_t  bandwidth;
    uint8_t   TlvPresent;
} nas_RFBandwidthInfoTlv;

/**
 *  Contain fields in struct nas_LTEOperationalModeTlv
 *
 *  @param  ciotLteOpMode
 *          - CIOT LTE operational mode.
 *          - Values
 *            - 0x00 - No service
 *            - 0x01 - Camped on LTE wideband
 *            - 0x02 - Camped on LTE M1
 *            - 0x03 - Camped on LTE NB1
 *
 *  @param  TlvPresent
 *          - Tlv Present.
 *
 *  @note    None
 *
 */
typedef struct
{
    uint32_t  ciotLteOpMode;
    uint8_t   TlvPresent;
} nas_LTEOperationalModeTlv;

/**
 *  Contain the parameters passed for unpack_nas_SLQSNasRFBandInfoCallback_Ind by
 *  the device.
 *
 *  @param  rfBandInfo
 *          - See @ref nas_RFBandInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param   rfDedicatedBandInfo
 *           - See @ref nas_RFDedicatedBandInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param   rfBandInfoExt
 *          - See @ref nas_RFBandInfoExtTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param   rfBandwidthInfo
 *          - See @ref nas_RFBandwidthInfoTlv for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param   lteOperationalMode
 *           - See @ref nas_LTEOperationalModeTlv for more information.
 *           - Bit to check in ParamPresenceMask - <B>19</B>
 *
 * \note    None
 *
 */
typedef struct
{
    nas_RFBandInfoTlv           rfBandInfo;
    nas_RFDedicatedBandInfoTlv  rfDedicatedBandInfo;
    nas_RFBandInfoExtTlv        rfBandInfoExt;
    nas_RFBandwidthInfoTlv      rfBandwidthInfo;
    nas_LTEOperationalModeTlv   lteOperationalMode;
    swi_uint256_t               ParamPresenceMask;
} unpack_nas_SLQSNasRFBandInfoCallback_Ind_t;

/**
 * unpack nas RF Band Info Callback indication
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_nas_SLQSNasRFBandInfoCallback_Ind(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_nas_SLQSNasRFBandInfoCallback_Ind_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif

