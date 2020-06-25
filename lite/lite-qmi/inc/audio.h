/**
 * \ingroup liteqmi
 *
 * \file audio.h
 */
#ifndef __LITEQMI_AUDIO_H__
#define __LITEQMI_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

/**
 * This structure contains the pack parameters to Get Audio Profile.
 *
 *  @param  Generator
 *          - Audio Generator
 *              - 0 - Voice
 *              - 1 - Key Beep
 *              - 2 - MIDI
 *
 */
typedef struct
{
    uint8_t Generator;
} pack_audio_SLQSGetAudioProfile_t;

/**
 *  This structure contains the unpack parameters to Get Audio Profile.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0 - Handset
 *              - 1 - Headset
 *              - 2 - Car Kit
 *              - 3 - Speaker phone
 *              - 4 - Auxiliary
 *              - 5 - TTY
 *              - 6 - Auxiliary external PCM
 *              - 7 - Primary external PCM
 *              - 8 - External slave PCM
 *              - 9 - I2S
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  EarMute
 *          - Ear Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  MicMute
 *          - MIC Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  Volume
 *          - Audio Volume Level
 *              - 0 to 7
 *          - Bit to check in ParamPresenceMask - <B>20</B>
 *
 */
typedef struct
{
    uint8_t        Profile;
    uint8_t        EarMute;
    uint8_t        MicMute;
    uint8_t        Volume;
    swi_uint256_t  ParamPresenceMask;
} unpack_audio_SLQSGetAudioProfile_t;

/**
 * Gets get the profile content of the requested audio generator. pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSGetAudioProfile(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_audio_SLQSGetAudioProfile_t *pReqParam
        );

/**
 * Gets get the profile content of the requested audio generator unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSGetAudioProfile(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_audio_SLQSGetAudioProfile_t *pOutput
        );

/**
 *  This structure contains pack parameters to Set Audio Profile.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0 - Handset
 *              - 1 - Headset
 *              - 2 - Car Kit
 *              - 3 - Speaker phone
 *              - 4 - Auxiliary
 *              - 5 - TTY
 *              - 6 - Auxiliary external PCM
 *              - 7 - Primary external PCM
 *              - 8 - External slave PCM
 *              - 9 - I2S
 *
 *  @param  EarMute
 *          - Ear Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  @param  MicMute
 *          - MIC Mute Setting
 *              - 0 - unmuted
 *              - 1 - muted
 *
 *  @param  Generator
 *          - Audio Generator
 *              - 0 - Voice
 *              - 1 - Key Beep
 *              - 2 - MIDI
 *
 *  @param  Volume
 *          - Audio Volume Level
 *              - 0 to 7
 *
 */
typedef struct
{
    uint8_t Profile;
    uint8_t EarMute;
    uint8_t MicMute;
    uint8_t Generator;
    uint8_t Volume;
} pack_audio_SLQSSetAudioProfile_t;

/**
 * sets an audio profile pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSSetAudioProfile(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_audio_SLQSSetAudioProfile_t *pReqParam
        );

/**
 * sets an audio profile unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSSetAudioProfile(
        uint8_t *pResp,
        uint16_t respLen
        );

/**
 * This structure contains the pack parameters for Get Audio Path Config.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  @param  Item
 *          - Item
 *              - 0 - AV_EC
 *              - 1 - AV_NS
 *              - 2 - AV_TXVOL
 *              - 3 - AV_DTMFTXG
 *              - 4 - AV_CODECSTG
 *              - 5 - AV_TXPCMIIRFLTR
 *              - 6 - AV_RXPCMIIRFLTR
 *              - 7 - AV_MICGAIN
 *              - 8 - AV_RXAGC
 *              - 9 - AV_TXAGC
 *              - 10 - AV_RXAGCLIST
 *              - 11 - AV_RXAVCLIST
 *              - 12 - AV_TXAGCLIST
 */
typedef struct
{
    uint8_t Profile;
    uint8_t Item;
} pack_audio_SLQSGetAudioPathConfig_t;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_TXPCMIIRFLTR.
 *
 *  @param  pFlag
 *          - Flag
 *              - 0x0000 - IIR filter disable
 *              - 0xffff - IIR filter enable
 *
 *  @param  pStageCnt
 *          - Stage Count
 *              - 0-4
 *
 *  @param  pStage0Val
 *          - A 20 BYTE sized parameter indicating Stage 0 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage1Val
 *          - A 20 BYTE sized parameter indicating Stage 1 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage2Val
 *          - A 20 BYTE sized parameter indicating Stage 2 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage3Val
 *          - A 20 BYTE sized parameter indicating Stage 3 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage4Val
 *          - A 20 BYTE sized parameter indicating Stage 4 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 */
typedef struct
{
    uint16_t *pFlag;
    uint16_t *pStageCnt;
    uint8_t  *pStage0Val;
    uint8_t  *pStage1Val;
    uint8_t  *pStage2Val;
    uint8_t  *pStage3Val;
    uint8_t  *pStage4Val;
} audio_TXPCMIIRFltr;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXPCMIIRFLTR.
 *
 *  @param  pFlag
 *          - Flag
 *              - 0x0000 - IIR filter disable
 *              - 0xffff - IIR filter enable
 *
 *  @param  pStageCnt
 *          - Stage Count
 *              - 0-4
 *
 *  @param  pStage0Val
 *          - A 20 BYTE sized parameter indicating Stage 0 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage1Val
 *          - A 20 BYTE sized parameter indicating Stage 1 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage2Val
 *          - A 20 BYTE sized parameter indicating Stage 2 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage3Val
 *          - A 20 BYTE sized parameter indicating Stage 3 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 *
 *  @param  pStage4Val
 *          - A 20 BYTE sized parameter indicating Stage 4 value
 *              - A1
 *              - A2
 *              - B0
 *              - B1
 *              - B2
 */
typedef struct
{
    uint16_t *pFlag;
    uint16_t *pStageCnt;
    uint8_t  *pStage0Val;
    uint8_t  *pStage1Val;
    uint8_t  *pStage2Val;
    uint8_t  *pStage3Val;
    uint8_t  *pStage4Val;
} audio_RXPCMIIRFltr;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXAGCLIST.
 *
 *  @param  pRXStaticGain
 *          - RX pre-compressor static gain
 *
 *  @param  pRXAIG
 *          - RX pre-compressor gain selection flag
 *
 *  @param  pRXExpThres
 *          - RX expansion threshold
 *
 *  @param  pRXExpSlope
 *          - RX expansion slope
 *
 *  @param  pRXComprThres
 *          - RX compression threshold
 *
 *  @param  pRXComprSlope
 *          - RX compression slope
 */
typedef struct
{
    uint16_t *pRXStaticGain;
    uint16_t *pRXAIG;
    uint16_t *pRXExpThres;
    uint16_t *pRXExpSlope;
    uint16_t *pRXComprThres;
    uint16_t *pRXComprSlope;
} audio_RXAGCList;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_RXAVCLIST.
 *
 *  @param  pAVRXAVCSens
 *          - AVC variation from nominal sensitivity
 *
 *  @param  pAVRXAVCHeadroom
 *          - AVC headroom
 *
 */
typedef struct
{
    uint16_t *pAVRXAVCSens;
    uint16_t *pAVRXAVCHeadroom;
} audio_RXAVCList;

/**
 * This structure contains the SLQSGetAudioPathConfig parameters related to
 * AV_TXAGCLIST.
 *
 *  @param  pTXStaticGain
 *          - TX pre-compressor static gain
 *
 *  @param  pTXAIG
 *          - TX pre-compressor gain selection flag
 *
 *  @param  pTXExpThres
 *          - TX expansion threshold
 *
 *  @param  pTXExpSlope
 *          - TX expansion slope
 *
 *  @param  pTXComprThres
 *          - TX compression threshold
 *
 *  @param  pTXComprSlope
 *          - TX compression slope
 */
typedef struct
{
    uint16_t *pTXStaticGain;
    uint16_t *pTXAIG;
    uint16_t *pTXExpThres;
    uint16_t *pTXExpSlope;
    uint16_t *pTXComprThres;
    uint16_t *pTXComprSlope;
} audio_TXAGCList;

/**
 *  This structure contains the SLQSGetAudioPathConfig response parameters.
 *
 *  @param  pECMode [Optional]
 *          - AV_EC
 *              - 0 - Echo cancellation off
 *              - 1 - Handset mode
 *              - 2 - Headset mode
 *              - 3 - Car kit mode
 *              - 4 - Speaker Mode
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *  @param  pNSEnable [Optional]
 *          - AV_NS
 *              - 0 - Noise suppression off
 *              - 1 - Noise suppression on
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *  @param  pTXGain [Optional]
 *          - AV_TXVOL
 *              - 0x0000 - 0xffff
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  @param  pDTMFTXGain [Optional]
 *          - AV_DTMFTXG
 *              - 0x0000 - 0xffff
 *          - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *  @param  pCodecSTGain [Optional]
 *          - AV_CODECSTG
 *              - 0x0000 - 0xffff
 *          - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *  @param  pTXPCMIIRFltr [Optional]
 *          - See @ref audio_TXPCMIIRFltr for more information
 *          - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  @param  pRXPCMIIRFltr [Optional]
 *          - See @ref audio_RXPCMIIRFltr for more information
 *          - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *  @param  pMICGainSelect [Optional]
 *          - AV_MICGAIN
 *          - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *  @param  pRXAVCAGCSwitch [Optional]
 *          - RX AVC/AGC Switch
 *          - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  @param  pTXAVCSwitch [Optional]
 *          - TX AVC Switch
 *          - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *  @param  pRXAGCList [Optional]
 *          - See @ref audio_RXAGCList for more information
 *          - Bit to check in ParamPresenceMask - <B>43</B>
 *
 *  @param  pRXAVCList [Optional]
 *          - See @ref audio_RXAVCList for more information
 *          - Bit to check in ParamPresenceMask - <B>44</B>
 *
 *  @param  pTXAGCList [Optional]
 *          - See @ref audio_TXAGCList for more information
 *          - Bit to check in ParamPresenceMask - <B>45</B>
 */
typedef struct
{
    uint8_t            *pECMode;
    uint8_t            *pNSEnable;
    uint16_t           *pTXGain;
    uint16_t           *pDTMFTXGain;
    uint16_t           *pCodecSTGain;
    audio_TXPCMIIRFltr *pTXPCMIIRFltr;
    audio_RXPCMIIRFltr *pRXPCMIIRFltr;
    uint8_t            *pMICGainSelect;
    uint8_t            *pRXAVCAGCSwitch;
    uint8_t            *pTXAVCSwitch;
    audio_RXAGCList    *pRXAGCList;
    audio_RXAVCList    *pRXAVCList;
    audio_TXAGCList    *pTXAGCList;
    swi_uint256_t      ParamPresenceMask;
} unpack_audio_SLQSGetAudioPathConfig_t;

/**
 * Gets the audio path configuration parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSGetAudioPathConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_audio_SLQSGetAudioPathConfig_t *pReqParam
        );

/**
 * Gets the audio path configuration parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSGetAudioPathConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_audio_SLQSGetAudioPathConfig_t *pOutput
        );

/**
 *  This structure contains the pack parameters for SLQSSetAudioPathConfig .
 *
 *  @param  Profile [Mandatory]
 *          - Audio Profile
 *              - 0-9
 *
 *  @param  pECMode [Optional]
 *          - AV_EC
 *              - 0 - Echo cancellation off
 *              - 1 - Handset echo mode
 *              - 2 - Headset mode
 *              - 3 - Car kit mode
 *              - 4 - Speaker Mode
 *
 *  @param  pNSEnable [Optional]
 *          - Noise Suppression
 *              - 0 - Noise suppression off
 *              - 1 - Noise suppression on
 *
 *  @param  pTXGain [Optional]
 *          - TX Voice volume
 *              - 0x0000 - 0xffff
 *
 *  @param  pDTMFTXGain [Optional]
 *          - AV_DTMFTXG
 *              - 0x0000 - 0xffff
 *
 *  @param  pCodecSTGain [Optional]
 *          - AV_CODECSTG
 *              - 0x0000 - 0xffff
 *
 *  @param  pTXPCMIIRFltr [Optional]
 *          - See @ref audio_TXPCMIIRFltr for more information
 *
 *  @param  pRXPCMIIRFltr [Optional]
 *          - See @ref audio_RXPCMIIRFltr for more information
 *
 *  @param  pRXAVCAGCSwitch [Optional]
 *          - RX AVC/AGC Switch
 *
 *  @param  pTXAVCSwitch [Optional]
 *          - TX AVC Switch
 *
 *  @param  pRXAGCList [Optional]
 *          - See @ref audio_RXAGCList for more information
 *
 *  @param  pRXAVCList [Optional]
 *          - See @ref audio_RXAVCList for more information
 *
 *  @param  pTXAGCList [Optional]
 *          - See @ref audio_TXAGCList for more information
 */
typedef struct
{
    uint8_t            Profile;
    uint8_t            *pECMode;
    uint8_t            *pNSEnable;
    uint16_t           *pTXGain;
    uint16_t           *pDTMFTXGain;
    uint16_t           *pCodecSTGain;
    audio_TXPCMIIRFltr *pTXPCMIIRFltr;
    audio_RXPCMIIRFltr *pRXPCMIIRFltr;
    uint8_t            *pRXAVCAGCSwitch;
    uint8_t            *pTXAVCSwitch;
    audio_RXAGCList    *pRXAGCList;
    audio_RXAVCList    *pRXAVCList;
    audio_TXAGCList    *pTXAGCList;
} pack_audio_SLQSSetAudioPathConfig_t;

/**
 * sets the audio path configuration parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSSetAudioPathConfig(
        pack_qmi_t *pCtx,
        uint8_t *pReqBuf,
        uint16_t *pLen,
        pack_audio_SLQSSetAudioPathConfig_t *pReqParam
        );

/**
 * sets the audio path configuration parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSSetAudioPathConfig(
        uint8_t *pResp,
        uint16_t respLen
        );

/**
 * This structure contains the pack parameters for SLQSGetAudioVolTLBConfig.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  @param  Generator
 *          - Audio Generator
 *              - 0-2
 *
 *  @param  Volume
 *          - Audio Volume Level
 *              - 0-7
 *
 *  @param  Item
 *          - Item
 *              - 13 - AV_RXVOLDB
 *              - 14 - AV_DTMFVOLDB
 *              - 15 - AV_PAD
 */
typedef struct
{
    uint8_t Profile;
    uint8_t Generator;
    uint8_t Volume;
    uint8_t Item;
} pack_audio_SLQSGetAudioVolTLBConfig_t;

/**
 *  This structure contains the unpack parameters for SLQSGetAudioVolTLBConfig.
 *
 *  @param  ResCode
 *          - Result of requested item
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 */
typedef struct
{
    uint16_t       ResCode;
    swi_uint256_t  ParamPresenceMask;
} unpack_audio_SLQSGetAudioVolTLBConfig_t;

/**
 * Gets the audio VolTLB configuration parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSGetAudioVolTLBConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_audio_SLQSGetAudioVolTLBConfig_t *pReqParam
        );

/**
 * Gets the audio VolTLB configuration parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSGetAudioVolTLBConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_audio_SLQSGetAudioVolTLBConfig_t *pOutput
        );

/**
 * This structure contains the pack parameters for  SLQSSetAudioVolTLBConfig.
 *
 *  @param  Profile
 *          - Audio Profile
 *              - 0-9
 *
 *  @param  Generator
 *          - Audio Generator
 *              - 0-2
 *
 *  @param  Volume
 *          - Audio Volume Level
 *              - 0-7
 *
 *  @param  Item
 *          - Item
 *              - 13 - AV_RXVOLDB
 *              - 14 - AV_DTMFVOLDB
 *              - 15 - AV_PAD
 *
 *  @param  Value
 *          - Value to be set to the volume table
 */
typedef struct
{
    uint8_t  Profile;
    uint8_t  Generator;
    uint8_t  Volume;
    uint8_t  Item;
    uint16_t VolValue;
} pack_audio_SLQSSetAudioVolTLBConfig_t;

/**
 *  This structure contains the unpack parameters for SLQSSetAudioVolTLBConfig.
 *
 *  @param  ResCode
 *          - Result of requested item
 *          - Bit to check in ParamPresenceMask - <B>33</B>
 */
typedef struct
{
    uint16_t       ResCode;
    swi_uint256_t  ParamPresenceMask;
} unpack_audio_SLQSSetAudioVolTLBConfig_t;

/**
 * Sets the audio VolTLB configuration parameters pack.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] pReqParam request parameters
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 * @note   Device Supported: SL9090
 *
 */
int pack_audio_SLQSSetAudioVolTLBConfig(
        pack_qmi_t *pCtx,
        uint8_t    *pReqBuf,
        uint16_t   *pLen,
        pack_audio_SLQSSetAudioVolTLBConfig_t *pReqParam
        );

/**
 * Sets the audio VolTLB configuration parameters unpack.
 * @param[in]   pResp       qmi response from modem
 * @param[in]   respLen     qmi response length
 * @param[out]  pOutput      response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_audio_SLQSSetAudioVolTLBConfig(
        uint8_t  *pResp,
        uint16_t respLen,
        unpack_audio_SLQSSetAudioVolTLBConfig_t *pOutput
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
