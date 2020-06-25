/**
 * \ingroup liteqmi
 *
 * \file fms.h
 */
#ifndef __FMS_PACH_H_
#define __FMS_PACH_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define FMS_GOBI_MBN_IMG_ID_SIZE      16
#define LITE_TOTAL_IMAGE_ID_STRING_SIZE 100
#define FMS_GOBI_MBN_IMG_ID_STR_LEN   LITE_TOTAL_IMAGE_ID_STRING_SIZE
#define FMS_GOBI_MBN_BUILD_ID_STR_LEN LITE_TOTAL_IMAGE_ID_STRING_SIZE
#define FMS_GOBI_LISTENTRIES_MAX      2
#define FMS_MAX_IMAGE_PREFERENCE_IMAGE_SIZE 255
#define FMS_MAX_IMAGE_ID_ELEMENT 50
#define FMS_IMAGE_ID_MAX_ENTRIES 2
#define FMS_FW_PRI_BUILD_MATCH_LEN 11
#define FMS_IMAGE_ID_IMG_ID_LEN   16
#define FMS_IMAGE_ID_BUILD_ID_LEN   32
#define FMS_IMAGE_ID_PRI_IMGTYPE      0x01

/**
 * This structure contains the Carrier Image parameters.
 *
 *  \param  m_nCarrierId
 *          - Unique numeric carrier ID indicating the carrier that
 *            the following images belong to
 *
 *  \param  m_nFolderId
 *          - Unique numeric folder ID indicating the folder where the
 *            images should reside on the host storage.
 *
 *  \param  m_nStorage
 *          - Information of storage type
 *          - Values
 *            - 0 - Device
 *            - 1 - Host
 *
 *  \param  m_FwImageId
 *          - Firmware image ID
 *
 *  \param  m_FwBuildId
 *          - Firmware build ID
 *
 *  \param  m_PriImageId
 *          - PRI image ID
 *
 *  \param  m_PriBuildId
 *          - PRI build ID
 *
 */

typedef struct
{
    uint32_t     m_nCarrierId;
    uint32_t     m_nFolderId;
    uint32_t     m_nStorage;
    uint8_t      m_FwImageId[FMS_GOBI_MBN_IMG_ID_STR_LEN];
    uint8_t      m_FwBuildId[FMS_GOBI_MBN_BUILD_ID_STR_LEN];
    uint8_t      m_PriImageId[FMS_GOBI_MBN_IMG_ID_STR_LEN];
    uint8_t      m_PriBuildId[FMS_GOBI_MBN_BUILD_ID_STR_LEN];
}CarrierImage_t;

/**
 * This structure contains the Get Image Preference information pack
 *
 *  \param  Tlvresult
 *              - Pack result
 */

typedef struct{
    uint16_t Tlvresult;
}pack_fms_GetImagesPreference_t;

/**
 * This structure contains the Image Element information
 *
 *  \param  imageType
 *              - Type of image
 *                 0 - Modem
 *                 1 - PRI
 *
 *  \param  imageId
 *              - Unique image identifier
 *
 *  \param  buildIdLength
 *              - Length of the build ID string (may be zero)
 *
 *  \param  pBuildId
 *              - Build ID ANSI string with length provided
 *              by the previous field
 *
 */
 
typedef struct
{
    uint8_t imageType;
    uint8_t imageId[FMS_GOBI_MBN_IMG_ID_STR_LEN];
    uint8_t buildIdLength;
    uint8_t buildId[FMS_GOBI_MBN_BUILD_ID_STR_LEN];
}FMSImageElement;

/**
 * This structure contains the Preference Image List information
 *
 *  \param  listSize
 *              - The number of elements in the image list
 *
 *  \param  pListEntries
 *              - Array of Image entries with size provided by
 *                previous field
 *              - See \ref FMSImageElement
 *
 */
 
typedef struct 
{
    uint8_t         listSize;
    FMSImageElement listEntries[FMS_GOBI_LISTENTRIES_MAX];
}FMSPrefImageList;

/**
 * This structure contains the Get Image Preference information unpack
 *
 *  \param  listSize
 *              - The number of elements in the image list
 *
 *  \param  pListEntries
 *              - Array of Image entries with size provided by
 *                previous field
 *              - See \ref FMSImageElement
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *              - Unpack result
 */


typedef struct{
    uint32_t         ImageListSize;
    FMSPrefImageList *pImageList;
    uint16_t         Tlvresult;
    swi_uint256_t    ParamPresenceMask;
}unpack_fms_GetImagesPreference_t;

/**
 * Get Images Preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 */
int pack_fms_GetImagesPreference(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_fms_GetImagesPreference_t *reqArg
);

/**
 * Get Images Preference unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 */
int unpack_fms_GetImagesPreference(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_fms_GetImagesPreference_t *pOutput
);


/**
 * This structure contains the Get Stored Images pack
 *
 *  \param  Tlvresult
 *              - Pack result
 */

typedef struct{
    uint16_t Tlvresult;
}pack_fms_GetStoredImages_t;

/**
 * This structure contains the Image ID list element Information
 *
 *  \param  storageIndex
 *          - Index in storage where the image is located(a value of 0xFF
 *            indicates that the storage for this type of image is not relevant)
 *
 *  \param  failureCount
 *          - Number of consecutive write attempts to this storage index
 *            that have failed(a value of 0xFF indicates unspecified)
 *
 *  \param  imageID
 *          - Image unique identifier(max 16 chars.)
 *
 *  \param  buildIDLength
 *          - Length of the build ID string. If there is no build ID, this
 *            field will be 0 and no data will follow.
 *
 *  \param  buildID
 *          - String containing image build information( Max 100 characters )
 */
typedef struct 
{
    uint8_t storageIndex;
    uint8_t failureCount;
    uint8_t imageID[FMS_GOBI_MBN_IMG_ID_STR_LEN];
    uint8_t buildIDLength;
    uint8_t buildID[FMS_GOBI_MBN_BUILD_ID_STR_LEN];
}FMSImageIdElement;


/**
 * This structure contains the list entry Information
 *
 *  \param  imageType
 *          - Type of image
 *              - 0 - Modem
 *              - 1 - PRI
 *
 *  \param  maxImages
 *          - Maximum number of images of this type that may be stored
 *            concurrently on the device
 *
 *  \param  executingImage
 *          - Index (into the next array) of image that is currently executing
 *
 *  \param  imageIDSize
 *          - The number of elements in the image ID list
 *
 *  \param  imageIDElement
 *          - Array of ImageIDElement Structure ( Max 50 elements )
 *          - See \ref FMSImageIdElement
 */
typedef struct
{
    uint8_t                  imageType;
    uint8_t                  maxImages;
    uint8_t                  executingImage;
    uint8_t                  imageIDSize;
    FMSImageIdElement imageIDElement[FMS_MAX_IMAGE_ID_ELEMENT];
}FMSImageIDEntries;


/**
 * This structure contains the Get Stored Images List
 *
 *  \param  listSize
 *          - The number of elements in the image list
 *
 *  \param  imageIDEntries
 *          - Array of ImageIDEntries Structure ( Max 2 entries )
 */
typedef struct
{
    uint8_t                  listSize;
    FMSImageIDEntries imageIDEntries[FMS_IMAGE_ID_MAX_ENTRIES];
}FMSImageList;


/**
 * This structure contains the Get Stored Images unpack
 *
 *  \param  listSize
 *              - The number of elements in the image list
 *
 *  \param  imageList
 *              - Array of Image entries with size provided by
 *                previous field
 *              - See \ref FMSImageElement
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *              - Unpack result
 */


typedef struct{
    uint32_t       imagelistSize;
    FMSImageList   imageList;
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_fms_GetStoredImages_t;


/**
 * Get Images Preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 */
int pack_fms_GetStoredImages(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_fms_GetStoredImages_t *reqArg
);

/**
 * Get Images Preference unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 */

int unpack_fms_GetStoredImages(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_fms_GetStoredImages_t *pOutput);


/**
 * This structure contains the Set Images Preference pack
 *
 *  \param  imageListSize
 *              - Image List Size
 *
 *  \param  pImageList
 *              - Image List
 *              - See \ref FMSPrefImageList
 *
 *  \param  bForceDownload
 *              - 0 - Not Force Donwload.
 *              - 1 - Focrce Download.
 *
 *  \param  modemindex
 *              - Modem Index.
 *
 *  \param  Tlvresult
 *              - Unpack result
 */

typedef struct{
    uint32_t imageListSize;
    FMSPrefImageList  *pImageList;
    uint32_t bForceDownload;
    uint8_t modemindex;
    uint16_t Tlvresult;
}pack_fms_SetImagesPreference_t;

/**
 * This structure contains the Set Images Preference unpack
 *
 *  \param  ImageTypesSize
 *              - Image Type Size
 *
 *  \param  ImageTypes
 *              - Image Type
 *              - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  Tlvresult
 *              - Unpack result
 */
typedef struct{
    uint32_t       ImageTypesSize;
    uint8_t        ImageTypes[FMS_MAX_IMAGE_PREFERENCE_IMAGE_SIZE];
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_fms_SetImagesPreference_t;

/**
 * Set Images Preference pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 */
int pack_fms_SetImagesPreference(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_fms_SetImagesPreference_t *reqArg
);

/**
 * Set Images Preference unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 */

int unpack_fms_SetImagesPreference(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_fms_SetImagesPreference_t *pOutput);

/**
 *  This API distills valid Firmware/PRI combinations from GetStoredImages result
 *
 *  \param[in]  pStoredImageList
 *          - image list returned from GetStoredImages 
 *          - See \ref FMSImageList
 *
 *  \param[in,out]  pValidCombinationSize
 *          - number of combination passed in and returned
 *
 *  \param[out]  pValidCombinations
 *          - valid combinations returned
 *          - See \ref CarrierImage_t
 *
 *  \return
 *          - eQCWWAN_ERR_INVALID_ARG - Invalid parameters
 *          - eQCWWAN_ERR_BUFFER_SZ - No enough element to store combinatons returned
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */

uint32_t GetValidFwPriCombinations(FMSImageList *pStoredImageList,
        uint32_t *pValidCombinationSize,
        CarrierImage_t *pValidCombinations);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
