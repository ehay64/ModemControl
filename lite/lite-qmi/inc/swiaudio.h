/**
 * \ingroup liteqmi
 *
 * \file swiaudio.h
 */
#ifndef __LITEQMI_SWIAUDIO_H__
#define __LITEQMI_SWIAUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define SWIAUDIO_MAX_LEN_IFACE_TABLE 255

/**
 *  This structure contains the SLQSGetM2MAudioProfile pack parameters.
 *
 *  @param  pGenerator[optional]
 *          - Generator
 *              - 0 - Voice
 *
 */
typedef struct
{
    uint8_t *pGenerator;
} pack_swiaudio_SLQSGetM2MAudioProfile_t;

/**
 *  This structure contains the SLQSGetM2MAudioProfile unpack parameters.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-5
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  EarMute
 *          - Ear Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  MicMute
 *          - MIC Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  @param  Generator
 *          - Generator
 *              - 0 - Voice
 *          - Bit to check in ParamPresenceMask - <B>4</B>
 *
 *  @param  Volume
 *          - RX volume level
 *              - 0-5
 *          - Bit to check in ParamPresenceMask - <B>5</B>
 *
 *  @param  CwtMute
 *          - Call waiting tone Mute
 *              - 0 - Mute
 *              - 1 - Unmute
 *          - Bit to check in ParamPresenceMask - <B>6</B>
 *
 *  @param  ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t Profile;
    uint8_t EarMute;
    uint8_t MicMute;
    uint8_t Generator;
    uint8_t Volume;
    uint8_t CwtMute;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiaudio_SLQSGetM2MAudioProfile_t;

/**
 * Gets the profile content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSGetM2MAudioProfile(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_swiaudio_SLQSGetM2MAudioProfile_t *pReqParam
        );

/**
 * Gets the profile content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSGetM2MAudioProfile(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSGetM2MAudioProfile_t *pOutput
        );

/**
 *  This structure contains the SLQSSetM2MAudioProfile pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  @param  pEarMute
 *          - Ear Mute
 *              - 0 - mute
 *              - 1 - unmute
 *
 *  @param  pMicMute
 *          - Mic Mute
 *              - 0 - mute
 *              - 1 - unmute
 *
 *  @param  pGenerator
 *          - Generator
 *              - 0 - voice
 *
 *  @param  pVolume
 *          - Set RX Volume level
 *              - 0-5
 *
 *  @param  pCwtMute
 *          - Call Waiting Tone Mute
 *              - 0 - Mute
 *              - 1 - UnMute
 */
typedef struct
{
    uint8_t Profile;
    uint8_t *pEarMute;
    uint8_t *pMicMute;
    uint8_t *pGenerator;
    uint8_t *pVolume;
    uint8_t *pCwtMute;
} pack_swiaudio_SLQSSetM2MAudioProfile_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAudioProfile_t;

/**
 * sets an audio profile content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAudioProfile(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MAudioProfile_t *pReqParam
        );

/**
 * sets an audio profile content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAudioProfile(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAudioProfile_t *pOutput
        );

/**
 *  This structure contains the SLQSGetM2MAudioVolume pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-5
 *
 *  @param  Generator
 *          - Generator
 *              - 0 - Voice
 */
typedef struct
{
    uint8_t Profile;
    uint8_t Generator;
} pack_swiaudio_SLQSGetM2MAudioVolume_t;

/**
 *  This structure contains the SLQSGetM2MAudioVolume unpack parameters.
 *
 *  @param  Level
 *          - The RX Volume Level
 *              - 0-5
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t Level;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiaudio_SLQSGetM2MAudioVolume_t;

/**
 * Gets the Volume content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSGetM2MAudioVolume(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_swiaudio_SLQSGetM2MAudioVolume_t *pReqParam
        );

/**
 * Gets the Volume content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSGetM2MAudioVolume(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSGetM2MAudioVolume_t *pOutput
        );

/**
 *  This structure contains the SLQSSetM2MAudioProfile pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  @param  Generator
 *          - Generator
 *              - 0 - voice
 *
 *  @param  Level
 *          - Audio volume level
 *              - 0-5
 */
typedef struct
{
    uint8_t Profile;
    uint8_t Generator;
    uint8_t Level;
} pack_swiaudio_SLQSSetM2MAudioVolume_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAudioVolume_t;

/**
 * Sets the Volume content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAudioVolume(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MAudioVolume_t *pReqParam
        );

/**
 * Sets the Volume content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAudioVolume(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAudioVolume_t *pOutput
        );

/**
 *  This structure contains the PCM parameters.
 *
 *  \param  iFaceTabLen
 *          - Number of sets of iface table
 *
 *  \param  iFaceTab
 *          - Physical Interface Parameters
 *          - See @ref qaGobiApiTableSwiAudio.h for more information on
 *            physical interface parameters
 *
 */
typedef struct
{
    uint8_t iFaceTabLen;
    uint8_t iFaceTab[SWIAUDIO_MAX_LEN_IFACE_TABLE];
}swiaudio_PCMparams;

/**
 *  This structure contains the SLQSSetM2MAudioAVCFG pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-5
 *
 *  @param  Device
 *          - ACDB Device
 *          - See @ref qaGobiApiTableSwiAudio.h for more information
 *            on ACDB Device
 *
 *  @param  PIFACEId
 *          - Physical Interface
 *          - See @ref qaGobiApiTableSwiAudio.h for more information
 *            on physical interface
 *
 *  @param  pPCMParams
 *          - PCM parameters
 *          - See @ref swiaudio_PCMparams for more information
 */
typedef struct
{
    uint8_t            Profile;
    uint8_t            Device;
    uint8_t            PIFACEId;
    swiaudio_PCMparams *pPCMParams;
}pack_swiaudio_SLQSSetM2MAudioAVCFG_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAudioAVCFG_t;

/**
 * Sets the AVCFG content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAudioAVCFG(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MAudioAVCFG_t *pReqParam
        );

/**
 * Sets the AVCFG content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAudioAVCFG(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAudioAVCFG_t *pOutput
        );

/**
 *  This structure contains the SLQSSetM2MAudioLPBK pack parameters.
 *
 *  @param  Enable
 *          - Operation to be performed
 *              - 0 - stop
 *              - 1 - VOCODER loop
 *              - 2 - internal codec loop
 *
 */
typedef struct
{
    uint8_t Enable;
} pack_swiaudio_SLQSSetM2MAudioLPBK_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAudioLPBK_t;

/**
 * Sets the LPBK content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAudioLPBK(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MAudioLPBK_t *pReqParam
        );

/**
 * Sets the LPBK content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAudioLPBK(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAudioLPBK_t *pOutput
        );

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAudioNVDef_t;

/**
 * Sets the NVDef content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAudioNVDef(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen
        );

/**
 * Sets the NVDef content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAudioNVDef(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAudioNVDef_t *pOutput
);

/**
 *  This structure contains the SLQSGetM2MSpkrGain pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 */
typedef struct
{
    uint8_t Profile;
} pack_swiaudio_SLQSGetM2MSpkrGain_t;

/**
 *  This structure contains the SLQSGetM2MSpkrGain unpack parameters.
 *
 *  @param  Value
 *          - RX speakerphone gain
 *              - 0x0 - 0x7fff
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint16_t Value;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiaudio_SLQSGetM2MSpkrGain_t;

/**
 * Gets the SPKRGAIN content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSGetM2MSpkrGain(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_swiaudio_SLQSGetM2MSpkrGain_t *pReqParam
        );

/**
 * Gets the SPKRGAIN content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSGetM2MSpkrGain(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSGetM2MSpkrGain_t *pOutput
        );

/**
 *  This structure contains the SLQSSetM2MSpkrGain pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  @param  Value
 *          - RX speakerphone gain
 *              - 0x0 - 0x7fff
 *
 */
typedef struct
{
    uint8_t Profile;
    uint16_t Value;
} pack_swiaudio_SLQSSetM2MSpkrGain_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MSpkrGain_t;

/**
 * Sets the SPKRGAIN content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MSpkrGain(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MSpkrGain_t *pReqParam
        );

/**
 * Sets the SPKRGAIN content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MSpkrGain(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MSpkrGain_t *pOutput
        );

/**
 *  This structure contains the SLQSGetM2MAVMute pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 */
typedef struct
{
    uint8_t Profile;
} pack_swiaudio_SLQSGetM2MAVMute_t;

/**
 *  This structure contains the SLQSGetM2MAVMute unpack parameters.
 *
 *  @param  pEarMute
 *          - Ear Mute
 *              - 0-Mute
 *              - 1-UnMute
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pMicMute
 *          - Mic Mute
 *              - 0-Mute
 *              - 1-unmute
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  CwtMute
 *          - Waiting tone Mute
 *              - 0-5
 *          - Bit to check in ParamPresenceMask - <B>3</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint8_t EarMute;
    uint8_t MicMute;
    uint8_t CwtMute;
    swi_uint256_t  ParamPresenceMask;
} unpack_swiaudio_SLQSGetM2MAVMute_t;

/**
 * Gets the AV Mute content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSGetM2MAVMute(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_swiaudio_SLQSGetM2MAVMute_t *pReqParam
        );

/**
 * Gets the AV Mute content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSGetM2MAVMute(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSGetM2MAVMute_t *pOutput
        );

/**
 *  This structure contains the SLQSSetM2MAVMute pack parameters.
 *
 *  @param  Profile
 *          - Audio Profile Number
 *              - 0-5
 *
 *  @param  EarMute
 *          - Ear Mute
 *              - 0-1
 *
 *  @param  MicMute
 *          - Mic Mute
 *              - 0-1
 *
 *  @param  pCwtMute [ Optional ]
 *          - Call Waiting Tone Mute
 *              - 0-1
 */
typedef struct
{
    uint8_t Profile;
    uint8_t EarMute;
    uint8_t MicMute;
    uint8_t *pCwtMute;
} pack_swiaudio_SLQSSetM2MAVMute_t;

typedef unpack_result_t  unpack_swiaudio_SLQSSetM2MAVMute_t;

/**
 * Sets the AV Mute content pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
int pack_swiaudio_SLQSSetM2MAVMute(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_swiaudio_SLQSSetM2MAVMute_t *pReqParam
        );

/**
 * Sets the AV Mute content unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_swiaudio_SLQSSetM2MAVMute(
        uint8_t *pResp,
        uint16_t respLen,
        unpack_swiaudio_SLQSSetM2MAVMute_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
