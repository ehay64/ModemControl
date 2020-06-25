/**
 * \ingroup liteqmi
 *
 * \file loc.h
 */

#ifndef __LITEQMI_LOC_H__
#define __LITEQMI_LOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdint.h>

#define LOC_UINT8_MAX_STRING_SZ 255

/*!
 * \def LOCEVENTMASKPOSITIONREPORT
 * The control point must enable this mask to receive 
 * position report event indications.
 */
#define LOCEVENTMASKPOSITIONREPORT            0x00000001
/*!
 * \def LOCEVENTMASKGNSSSVINFO
 * The control point must enable this mask to receive 
 * satellite report event indications. These reports 
 * are sent at a 1 Hz rate.
 */
#define LOCEVENTMASKGNSSSVINFO                0x00000002
/*!
 * \def LOCEVENTMASKNMEA
 * The control point must enable this mask to receive 
 * NMEA reports for position and satellites in view. 
 * The report is at a 1 Hz rate.
 */
#define LOCEVENTMASKNMEA                      0x00000004
/*!
 * \def LOCEVENTMASKNINOTIFYVERIFYREQ
 * The control point must enable this mask to receive 
 * NI Notify/Verify request event indications.
 */
#define LOCEVENTMASKNINOTIFYVERIFYREQ         0x00000008
/*!
 * \def LOCEVENTMASKINJECTTIMEREQ
 * The control point must enable this mask to receive 
 * time injection request event indications.
 */
#define LOCEVENTMASKINJECTTIMEREQ             0x00000010
/*!
 * \def LOCEVENTMASKINJECTPREDICTEDORBITSREQ
 * The control point must enable this mask to receive 
 * predicted orbits request event indications.
 */
#define LOCEVENTMASKINJECTPREDICTEDORBITSREQ  0x00000020
/*!
 * \def LOCEVENTMASKINJECTPOSITIONREQ
 * The control point must enable this mask to receive 
 * position injection request event indications.
 */
#define LOCEVENTMASKINJECTPOSITIONREQ         0x00000040
/*!
 * \def LOCEVENTMASKENGINESTATE
 * The control point must enable this mask to receive 
 * engine state report event indications.
 */
#define LOCEVENTMASKENGINESTATE               0x00000080
/*!
 * \def LOCEVENTMASKFIXSESSIONSTATE
 * The control point must enable this mask to receive 
 * fix session status report event indications.
 */
#define LOCEVENTMASKFIXSESSIONSTATE           0x00000100
/*! 
 * \def LOCEVENTMASKWIFIREQ
 * The control point must enable this mask to receive 
 * Wi-Fi position request event indications.
 */
#define LOCEVENTMASKWIFIREQ                   0x00000200
/*!
 \def LOCEVENTMASKSENSORSTREAMINGREADYSTATUS
 * The control point must enable this mask to receive 
 * notifications from the location engine indicating 
 * its readiness to accept data from the 
 * sensors (accelerometer, gyroscope, etc.).
 */
#define LOCEVENTMASKSENSORSTREAMINGREADYSTATUS  0x00000400
/*!
 * \def LOCEVENTMASKTIMESYNCREQ
 * The control point must enable this mask to receive 
 * time sync requests from the GPS engine. 
 * Time sync enables the GPS engine to synchronize 
 * its clock with the sensor processor's clock.
 */
#define LOCEVENTMASKTIMESYNCREQ               0x00000800
/*!
 * \def LOCEVENTMASKSETSPISTREAMINGREPORT
 * The control point must enable this mask to receive 
 * Stationary Position Indicator (SPI) 
 * streaming report indications.
 */
#define LOCEVENTMASKSETSPISTREAMINGREPORT     0x00001000
/*!
 * \def LOCEVENTMASKLOCATIONSERVERCONNECTIONREQ
 * The control point must enable this mask
 * to receive location server requests. These
 * requests are generated when the service
 * wishes to establish a connection with a
 * location server.
 */
#define LOCEVENTMASKLOCATIONSERVERCONNECTIONREQ  0x00002000
/*!
 * \def LOCEVENTMASKNIGEOFENCENOTIFICATION
 * The control point must
 * enable this mask to receive notifications
 * related to network-initiated Geofences.
 * These events notify the client when a
 * network-initiated Geofence is added,
 * deleted, or edited.
 */
#define LOCEVENTMASKNIGEOFENCENOTIFICATION    0x00004000
/*!
 * \def LOCEVENTMASKGEOFENCEGENALERT
 * The control point must
 * enable this mask to receive Geofence
 * alerts. These alerts are generated to
 * inform the client of the changes that may
 * affect a Geofence, for example, if GPS is
 * turned off or if the network is
 * unavailable.
 */
#define LOCEVENTMASKGEOFENCEGENALERT          0x00008000
/*!
 * \def LOCEVENTMASKGEOFENCEBREACHNOTIFICATION
 * The control point must enable this mask to
 * receive notifications when a Geofence is
 * breached. These events are generated
 * when a UE enters or leaves the perimeter
 * of a Geofence. This breach report is for a
 * single Geofence.
 */
#define LOCEVENTMASKGEOFENCEBREACHNOTIFICATION  0x00010000
/*!
 * \def LOCEVENTMASKPEDOMETERCONTROL
 * The control point must
 * enable this mask to register for
 * pedometer control requests from the
 * location engine. The location engine
 * sends this event to control the injection
 * of pedometer reports.
 */
#define LOCEVENTMASKPEDOMETERCONTROL          0x00020000
/*!
 * \def LOCEVENTMASKMOTIONDATACONTROL
 * The control point must
 * enable this mask to register for motion
 * data control requests from the location
 * engine. The location engine sends this
 * event to control the injection of motion
 * data.
 */
#define LOCEVENTMASKMOTIONDATACONTROL         0x00040000
/*!
 * \def LOCEVENTMASKBATCHFULLNOTIFICATION
 * The control point must enable this
 * mask to receive notification when a
 * batch is full. The location engine sends
 * this event to notify of Batch Full for
 * ongoing batching session.
 */
#define LOCEVENTMASKBATCHFULLNOTIFICATION     0x00080000
/*!
 * \def LOCEVENTMASKLIVEBATCHEDPOSITIONREPORT
 * The control point must
 * enable this mask to receive position
 * report indications along with an ongoing
 * batching session. The location engine
 * sends this event to notify the batched
 * position report while a batching session
 * is ongoing.
 */
#define LOCEVENTMASKLIVEBATCHEDPOSITIONREPORT 0x00100000
/*!
 * \def LOCEVENTMASKINJECTWIFIAPDATAREQ
 * The control point must enable this mask
 * to receive Wi-Fi Access Point (AP) data
 * inject request event indications.
 */
#define LOCEVENTMASKINJECTWIFIAPDATAREQ       0x00200000
/*!
 * \def LOCEVENTMASKGEOFENCEBATCHBREACHNOTIFICATION
 * The control point must enable this mask to
 * receive notifications when a Geofence is
 * breached. These events are generated
 * when a UE enters or leaves the perimeter
 * of a Geofence. This breach notification
 * is for multiple Geofences. Breaches
 * from multiple Geofences are all batched
 * and sent in the same notification.
 */
#define LOCEVENTMASKGEOFENCEBATCHBREACHNOTIFICATION 0x00400000
/*!
 * \def LOCEVENTMASKVEHICLEDATAREADYSTATUS
 * The control point must
 * enable this mask to receive notifications
 * from the location engine indicating its
 * readiness to accept vehicle data (vehicle
 * accelerometer, vehicle angular rate,
 * vehicle odometry, etc.).
 */
#define LOCEVENTMASKVEHICLEDATAREADYSTATUS    0x00800000
/*! 
 \def LOCEVENTMASKGNSSMEASUREMENTREPORT
 * The control point must
 * enable this mask to receive system clock
 * and satellite measurement report events
 * (system clock, SV time, Doppler, etc.).
 * Reports are generated only for the GNSS
 * satellite constellations that are enabled
 * using QMI_LOC_SET_GNSS_
 * CONSTELL_REPORT_CONFIG(Not yet supported).
 */
#define LOCEVENTMASKGNSSMEASUREMENTREPORT     0x01000000
/*!
 * \def LOCEVENTMASKINVALIDVALUE
 * Invalid Event Mask
 */
#define LOCEVENTMASKINVALIDVALUE              0xFFFFFFFF

#define  MAX_SENSOR_DATA_LEN       64
#define  MAX_TEMP_DATA_LEN         64

#define  MAX_LOC_NMEA_STR_LEN      201

enum {
    eQMI_LOC_SESS_STATUS_SUCCESS=0,
    eQMI_LOC_SESS_STATUS_IN_PROGRESS=1,
    eQMI_LOC_SESS_STATUS_FAILURE=2,
    eQMI_LOC_SESS_STATUS_TIMEOUT=3,
};


/**
 * This structure contains the Application Information
 *
 * @param appProviderLength
 *        - Length of the Application Provider
 *
 * @param pAppProvider
 *        - Application Provider
 *        - Depends upon the Length of application Provider
 *
 * @param appNameLength
 *        - Length of Application Name
 *
 * @param pAppName
 *        - Application Name
 *        - Depends upon the Length of application Name
 *
 * @param appVersionValid
 *        - Specifies whether the application version string contains a
 *          valid value
 *        - 0x00 (FALSE) Application version string is invalid
 *        - 0x01 (TRUE) Application version string is valid
 *
 * @param appVersionLength
 *        - Length of Application Version
 *
 * @param pAppVersion
 *        - Application Version
 *        - Depends upon the Length of application Version
 *
 */

typedef struct 
{
    uint8_t appProviderLength;
    uint8_t *pAppProvider;
    uint8_t appNameLength;
    uint8_t *pAppName;
    uint8_t appVersionValid;
    uint8_t appVersionLength;
    uint8_t *pAppVersion;
}loc_LocApplicationInfo;

/**
 * This structure contains the Delete LOC SV Info
 *
 * \param id
 *        - LOC SV ID of the satellite whose data is to be
 *          deleted
 *        - Range:
 *           - For GPS: 1 to 32
 *           - For SBAS: 33 to 64
 *           - For GLONASS: 65 to 96
 *
 * \param system
 *        - Indicates to which constellation this loc_SV belongs
 *        - Valid values:
 *           - eQMI_LOC_SV_SYSTEM_GPS (1) - GPS satellite
 *           - eQMI_LOC_SV_SYSTEM_GALILEO (2) - GALILEO satellite
 *           - eQMI_LOC_SV_SYSTEM_SBAS (3) - SBAS satellite
 *           - eQMI_LOC_SV_SYSTEM_COMPASS (4) - COMPASS satellite
 *           - eQMI_LOC_SV_SYSTEM_GLONASS (5) - GLONASS satellite
 *           - eQMI_LOC_SV_SYSTEM_BDS (6) - BDS satellite
 *
 * \param mask
 *        - Indicates if the ephemeris or almanac for a satellite
 *          is to be deleted
 *        - Valid values:
 *           - 0x01 - DELETE_EPHEMERIS
 *           - 0x02 - DELETE_ALMANAC
 *
 */

typedef struct
{
    uint16_t    id;
    uint32_t   system;
    uint8_t    mask;
}loc_SV;

/**
 * This structure contains the elements of Delete LOC SV Info
 *
 * \param len
 *        - Number of sets of the following elements in struct loc_SV:
 *           - gnssSvId
 *           - system
 *           - deleteSvInfoMask
 *
 * \param pSV
 *        - Pointer to struct loc_SV. See \ref loc_SV for more information
 *
 */

typedef struct
{
    uint8_t    len;
    loc_SV      *pSV;
}loc_SVInfo;

/**
 * This structure contains the GNSS data
 *
 * \param mask
 *        - Mask for the GNSS data that is to be deleted
 *        - Valid values:
 *           - QMI_LOC_MASK_DELETE_GPS_SVDIR (0x00000001) - Mask to
 *             delete GPS SVDIR
 *           - QMI_LOC_MASK_DELETE_GPS_SVSTEER (0x00000002) - Mask to
 *             delete GPS SVSTEER 
 *           - QMI_LOC_MASK_DELETE_GPS_TIME (0x00000004) - Mask to
 *             delete GPS time
 *           - QMI_LOC_MASK_DELETE_GPS_ALM_CORR (0x00000008) - Mask to
 *             delete almanac correlation
 *           - QMI_LOC_MASK_DELETE_GLO_SVDIR (0x00000010) - Mask to
 *             delete GLONASS SVDIR
 *           - QMI_LOC_MASK_DELETE_GLO_SVSTEER (0x00000020) - Mask to
 *             delete GLONASS SVSTEER
 *           - QMI_LOC_MASK_DELETE_GLO_TIME (0x00000040) - Mask to
 *             delete GLONASS time
 *           - QMI_LOC_MASK_DELETE_GLO_ALM_CORR (0x00000080) - Mask to
 *             delete GLONASS almanac correlation
 *           - QMI_LOC_MASK_DELETE_SBAS_SVDIR (0x00000100) - Mask to
 *             delete SBAS SVDIR
 *           - QMI_LOC_MASK_DELETE_SBAS_SVSTEER (0x00000200) - Mask to
 *             delete SBAS SVSTEER
 *           - QMI_LOC_MASK_DELETE_POSITION (0x00000400) - Mask to
 *             delete position estimate
 *           - QMI_LOC_MASK_DELETE_TIME (0x00000800) - Mask to
 *             delete time estimate
 *           - QMI_LOC_MASK_DELETE_IONO (0x00001000) - Mask to
 *             delete IONO
 *           - QMI_LOC_MASK_DELETE_UTC (0x00002000) - Mask to
 *             delete UTC estimate
 *           - QMI_LOC_MASK_DELETE_HEALTH (0x00004000) - Mask to
 *             delete SV health record
 *           - QMI_LOC_MASK_DELETE_SADATA (0x00008000) - Mask to
 *             delete SADATA
 *           - QMI_LOC_MASK_DELETE_RTI (0x00010000) - Mask to
 *             delete RTI
 *           - QMI_LOC_MASK_DELETE_SV_NO_EXIST (0x00020000) - Mask to
 *             delete SV_NO_EXIST
 *           - QMI_LOC_MASK_DELETE_FREQ_BIAS_EST (0x00040000) - Mask to
 *             delete frequency bias estimate
 *           - QMI_LOC_MASK_DELETE_BDS_SVDIR (0x00080000) - Mask to
 *             delete BDS SVDIR
 *           - QMI_LOC_MASK_DELETE_BDS_SVSTEER (0x00100000) - Mask to
 *             delete BDS SVSTEER
 *           - QMI_LOC_MASK_DELETE_BDS_TIME (0x00200000) - Mask to
 *             delete BDS time
 *           - QMI_LOC_MASK_DELETE_BDS_ALM_CORR (0x00400000) - Mask to
 *             delete BDS almanac correlation
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GPS (0x00800000) -
 *             Mask to delete GNSS SV blacklist GPS
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GLO (0x01000000) -
 *             Mask to delete GNSS SV blacklist GLO
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_BDS (0x02000000) -
 *             Mask to delete GNSS SV blacklist BDS
 *
 */
 
typedef struct
{
    uint64_t   mask;
}loc_GnssData;

/**
 * This structure contains the cell database
 *
 * \param mask
 *        - Mask for the cell database assistance data that is to be deleted
 *        - Valid values:
 *           - 0x00000001 - DELETE_CELLDB_POS
 *           - 0x00000002 - DELETE_CELLDB_LATEST_GPS_POS
 *           - 0x00000004 - DELETE_CELLDB_OTA_POS
 *           - 0x00000008 - DELETE_CELLDB_EXT_REF_POS
 *           - 0x00000010 - DELETE_CELLDB_TIMETAG
 *           - 0x00000020 - DELETE_CELLDB_CELLID
 *           - 0x00000040 - DELETE_CELLDB_CACHED_CELLID
 *           - 0x00000080 - DELETE_CELLDB_LAST_SRV_CELL
 *           - 0x00000100 - DELETE_CELLDB_CUR_SRV_CELL
 *           - 0x00000200 - DELETE_CELLDB_NEIGHBOR_INFO
 *
 */

typedef struct
{
    uint32_t   mask;
}loc_CellDb;

/**
 * This structure contains the clock Info
 *
 * \param mask
 *        - Mask for the clock information assistance data that is to be deleted
 *        - Valid bitmasks:
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST (0x00000001) - Mask to
 *             delete time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST (0x00000002) - Mask to
 *             delete frequency estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER (0x00000004) - Mask to
 *             delete week number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME (0x00000008) - Mask to
 *             delete RTC time from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER (0x00000010) - Mask to
 *             delete time transfer from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST (0x00000020) - Mask to
 *             delete GPS time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST (0x00000040) - Mask to
 *             delete GLONASS time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER (0x00000080) - Mask to
 *             delete GLONASS day number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER (0x00000100) - Mask to
 *             delete GLONASS four year number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY (0x00000200) - Mask to
 *             delete GLONASS RF GRP delay from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT (0x00000400) - Mask to
 *             delete disable TT from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC (0x00000800) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB (0x00001000) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST (0x00002000) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB (0x00004000) - Mask to delete
 *             Glonass-to-BDS time bias-related information from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB (0x00008000) - Mask to delete
 *             BDS-to-GLONASS time bias-related information from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER (0x00010000) - Mask to
 *             delete the BDS week number from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY (0x00020000) - Mask to
 *             delete the BDS RF GRP delay from the clock information
 *
 */

typedef struct
{
    uint32_t   mask;
}loc_ClkInfo;

/**
 * This structure contains the BDS SV Info
 *
 * \param id
 *        - SV ID of the satellite whose data is to be deleted.
 *           - Range for BDS: 201 to 237
 *
 * \param mask
 *        - Indicates if the ephemeris or almanac for a satellite is to be deleted
 *        - Valid values:
 *           - QMI_LOC_MASK_DELETE_EPHEMERIS (0x01) - Delete ephemeris for the satellite
 *           - QMI_LOC_MASK_DELETE_ALMANAC (0x02) - Delete almanac for the satellite
 *
*/

typedef struct
{
    uint16_t    id;
    uint8_t    mask;
}loc_BdsSV;

/**
 * This structure contains the number of sets of the BDS SVN Info
 *
 * \param len
 *        - Number of sets of the following elements:
 *           - gnssSvId
 *           - deleteSvInfoMask
 *
 * \param pSV
 *        - Pointer to struct loc_BdsSV. See \ref loc_BdsSV for more information
 *
*/

typedef struct
{
    uint8_t    len;
    loc_BdsSV   *pSV;
}loc_BdsSVInfo;

/**
 *  This structure contains the Parameter for RegisterEvents
 *
 *  \param  eventRegister
 *          - Specifies the events that the control point is interested
 *          in receiving.
 *          -Values
 *             - 0x00000001 - to receive position report event
 *                            indications
 *             - 0x00000002 - to receive satellite report event
 *                            indications.These reports are sent
 *                            at a 1 Hz rate.
 *             - 0x00000004 - to receive NMEA reports for position
 *                            and satellites in view.The report is
 *                            at a 1 Hz rate.
 *             - 0x00000008 - to receive NI Notify/Verify request
 *                            event indications
 *             - 0x00000010 - to receive time injection request
 *                            event indications.
 *             - 0x00000020 - to receive predicted orbits request
 *                            event indications.
 *             - 0x00000040 - to receive position injection request
 *                            event indications.
 *             - 0x00000080 - to receive engine state report event
 *                            indications.
 *             - 0x00000100 - to receive fix session status report
 *                            event indications.
 *             - 0x00000200 - to receive Wi-Fi position request
 *                            event indications.
 *             - 0x00000400 - to receive notifications from the location
 *                            engine indicating its readiness to accept
 *                            data from the sensors(accelerometer, gyroscope, etc.).
 *             - 0x00000800 - to receive time sync requests from
 *                            the GPS engine. Time sync enables the
 *                            GPS engine to synchronize its clock
 *                            with the sensor processor’s clock.
 *             - 0x00001000 - to receive Stationary Position Indicator (SPI)
 *                            streaming report indications.
 *             - 0x00002000 - to receive location server requests.
 *                            These requests are generated when
 *                            the service wishes to establish a
 *                            connection with a location server.
 *             - 0x00004000 - to receive notifications related
 *                            to network-initiated Geofences. These
 *                            events notify the client when a network-initiated
 *                            Geofence is added, deleted, or edited.
 *             - 0x00008000 - to receive Geofence alerts. These alerts
 *                            are generated to inform the client of
 *                            the changes that may affect a Geofence,
 *                            e.g., if GPS is turned off or if the
 *                            network is unavailable.
 *             - 0x00010000 - to receive notifications when a Geofence
 *                            is breached. These events are generated
 *                            when a UE enters or leaves the perimeter
 *                            of a Geofence. This breach report is for
 *                            a single Geofence.
 *             - 0x00020000 - to register for pedometer control requests
 *                            from the location engine. The location
 *                            engine sends this event to control the
 *                            injection of pedometer reports.
 *             - 0x00040000 - to register for motion data control requests
 *                            from the location engine. The location
 *                            engine sends this event to control the
 *                            injection of motion data.
 *             - 0x00080000 - to receive notification when a batch is full.
 *                            The location engine sends this event to
 *                            notify of Batch Full for ongoing batching
 *                            session.
 *             - 0x00100000 - to receive position report indications
 *                            along with an ongoing batching session.
 *                            The location engine sends this event to
 *                            notify the batched position report while
 *                            a batching session is ongoing.
 *             - 0x00200000 - to receive Wi-Fi Access Point (AP) data
 *                            inject request event indications.
 *             - 0x00400000 - to receive notifications when a Geofence
 *                            is breached. These events are generated
 *                            when a UE enters or leaves the perimeter
 *                            of a Geofence. This breach notification
 *                            is for multiple Geofences. Breaches from
 *                            multiple Geofences are all batched and
 *                            sent in the same notification.
 *             - 0x00800000 - to receive notifications from the location
 *                            engine indicating its readiness to accept
 *                            vehicle data (vehicle accelerometer,
 *                            vehicle angular rate, vehicle odometry, etc.).
 *             - 0x01000000 - to receive system clock and satellite
 *                            measurement report events (system clock,
 *                            SV time, Doppler, etc.).
 *             - 0x02000000 - to receive satellite position reports
 *                            as polynomials.Reports are generated only
 *                            for the GNSS satellite constellations that
 *                            are enabled using QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG.
 *
 *  \note   Multiple events can be registered by OR the individual masks and sending
 *          them in this TLV. All unused bits in this mask must be set to 0.
 *
 *  \param Tlvresult
 *        - Pack result.
 *
 */
 
typedef struct{
    uint64_t eventRegister;
    uint16_t Tlvresult;
}pack_loc_EventRegister_t;

/**
 * This structure contains Event Register unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 *
 */

typedef struct{
    uint16_t         Tlvresult;
    swi_uint256_t    ParamPresenceMask;
}unpack_loc_EventRegister_t;


/**
 * Event Register pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_EventRegister(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_EventRegister_t     *reqArg
);

/**
 * Event Register unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_EventRegister(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_EventRegister_t *pOutput
);

/**
 *  This structure contains the Parameter External Power Source State pack.
 *
 *  \param  extPowerState
 *          - Specifies the Power state; injected by the control
 *            point.
 *          - Values
 *             - 0 - Device is not connected to an external power
 *                   source
 *             - 1 - Device is connected to an external power source
 *             - 2 - Unknown external power state
 * \param Tlvresult
 *        - Pack result.
 */


typedef struct{
    uint32_t extPowerState;
    uint16_t Tlvresult;
}pack_loc_SetExtPowerState_t;

/**
 * This structure contains Set Ext Power State unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_SetExtPowerState_t;


/**
 * Set Ext Power State pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SetExtPowerState(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SetExtPowerState_t *reqArg
);

/**
 * Set Ext Power State unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SetExtPowerState(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SetExtPowerState_t *pOutput
);

/**
 * This structure contains the LOC Start pack
 *
 * @param SessionId
 *        - ID of the session as identified by the control point.
 *        - Range: 0 to 255
 *
 * @param pRecurrenceType
 *        - Optional Parameter
 *        - Specifies the type of session in which the control point
 *          is interested.
 *        - Defaults to SINGLE.
 *        -Values
 *           - 1 - Request periodic position fixes
 *           - 2 - Request a single position fix
 *
 * @param pHorizontalAccuracyLvl
 *        - Optional Parameter
 *        - Specifies the horizontal accuracy level required by the
 *        control point.
 *        - Defaults to LOW
 *        - Values
 *           - 1 - Low accuracy
 *           - 2 - Medium accuracy
 *           - 3 - High accuracy
 *
 * @param pIntermediateReportState
 *        - Optional Parameter
 *        - Specifies if the control point is interested
 *        in receiving intermediate reports.
 *        - ON by default.
 *        - Values
 *          - 1 - Intermediate reports are turned on
 *          - 2 - Intermediate reports are turned off
 *
 * @param pMinIntervalTime
 *        - Optional Parameter
 *        - Minimum time interval, specified by the control point,
 *          that must elapse between position reports.
 *        - Units - Milliseconds
 *        - Default - 1000 ms
 *
 * @param LocApplicationInfo
 *        - Optional Parameter
 *        - LOC Application Parameters
 *        - See @ref loc_LocApplicationInfo for more information
 *
 * @param pConfigAltitudeAssumed
 *        - Optional Parameter
 *        - Configuration for Altitude Assumed Info in GNSS SV
 *        Info Event
 *        - Defaults to ENABLED.
 *        - Values
 *          - 1 - Enable Altitude Assumed information in GNSS SV
 *               Info Event
 *          - 2 - Disable Altitude Assumed information in GNSS SV
 *                Info Event
 *
 * @param Tlvresult
 *        - Unpack result.
 */

typedef struct{
    uint8_t    SessionId;
    uint32_t   *pRecurrenceType;
    uint32_t   *pHorizontalAccuracyLvl;
    uint32_t   *pIntermediateReportState;
    uint32_t   *pMinIntervalTime;
    loc_LocApplicationInfo *pApplicationInfo;
    uint32_t   *pConfigAltitudeAssumed;
    uint16_t    Tlvresult;
}pack_loc_Start_t;

/**
 * This structure contains Start LOC unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 *
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_Start_t;

/**
 * LOC Start pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_Start(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_Start_t *reqArg
);

/**
 * Loc Start  unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_Start(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_Start_t *pOutput
);


/**
 * This structure contains Stop LOC pack
 *
 * \param sessionId
 *        - ID of the session as identified by the control point.
 *        - Range: 0 to 255
 *
 * \param Tlvresult
 *        - Unpack result.
 */


typedef struct{
    uint8_t     SessionId;
    uint16_t    Tlvresult;
}pack_loc_Stop_t;

/**
 * This structure contains Stop LOC unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_Stop_t;

/**
 * Loc Stop pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_Stop(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_Stop_t *reqArg
);

/**
 * Loc Stop unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_Stop(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_Stop_t *pOutput
);

/**
 * This structure contains Set Operation Mode pack
 *
 * \param mode
 *        - Valid values:
 *           - eQMI_LOC_OPER_MODE_DEFAULT (1) - Use the default engine mode
 *           - eQMI_LOC_OPER_MODE_MSB (2) - Use the MS-based mode
 *           - eQMI_LOC_OPER_MODE_MSA (3) - Use the MS-assisted mode
 *           - eQMI_LOC_OPER_MODE_STANDALONE (4) - Use Standalone mode
 *           - eQMI_LOC_OPER_MODE_CELL_ID (5) - Use cell ID; this mode is
 *             only valid for GSM/UMTS networks
 *           - eQMI_LOC_OPER_MODE_WWAN (6) - Use WWAN measurements to calculate
 *             the position; if this mode is set, AFLT will be used for 1X
 *             networks and OTDOA will be used for LTE networks
 *
 * \param Tlvresult
 *        - Pack result.
 *
 */
typedef struct{
    uint32_t mode;
    uint16_t Tlvresult;
}pack_loc_SetOperationMode_t;

/**
 * This structure contains Set Operation Mode unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_SetOperationMode_t;

/**
 * Set Operation Mode pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SetOperationMode(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SetOperationMode_t *reqArg
);

/**
 * Set Operation Mode unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SetOperationMode(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SetOperationMode_t *pOutput
);


/**
 * This structure contains LOC delete assist data pack
 *
 * \param pSVInfo
 *        - Pointer to struct loc_SVInfo. See \ref loc_SVInfo for more information
 *
 * \param pGnssData
 *        - Pointer to struct loc_GnssData. See \ref loc_GnssData for more information
 *
 * \param pCellDb
 *        - Pointer to struct loc_CellDb. See \ref loc_CellDb for more information
 *
 * \param pClkInfo
 *        - Pointer to struct loc_ClkInfo. See \ref loc_ClkInfo for more information
 *
 * \param pBdsSVInfo
 *        - Pointer to struct loc_BdsSVInfo. See \ref loc_BdsSVInfo for more information
 *
 * \param Tlvresult
 *        - Pack delete assist data request result.
 */

typedef struct{
    loc_SVInfo *pSVInfo;
    loc_GnssData *pGnssData;
    loc_CellDb *pCellDb;
    loc_ClkInfo *pClkInfo;
    loc_BdsSVInfo *pBdsSVInfo;
    uint16_t Tlvresult;
}pack_loc_Delete_Assist_Data_t;

/**
 * This structure contains LOC delete assist data unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 */
 
typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_Delete_Assist_Data_t;

typedef unpack_loc_Delete_Assist_Data_t unpack_loc_DeleteAssistData_t;
/**
 * Delete Assistant Data pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_DeleteAssistData(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_Delete_Assist_Data_t *reqArg
);

/**
 * Delete Assistant Data unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_DeleteAssistData(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_Delete_Assist_Data_t *pOutput
);

/**
 *  This structure contains Dilution of precision associated with this position.
 *
 *  \param  PDOP
 *          - Position dilution of precision.
 *          - Range - 1 (highest accuracy) to 50 (lowest accuracy)
 *          - PDOP = square root of (Square of HDOP + Square of VDOP2 )
 *  \param  HDOP
 *          - Horizontal dilution of precision.
 *          - Range - 1 (highest accuracy) to 50 (lowest accuracy)
 *  \param  VDOP
 *          - Vertical dilution of precision.
 *          - Range-  1 (highest accuracy) to 50 (lowest accuracy)
 *
 */

typedef struct {
     uint32_t    PDOP;
     uint32_t    HDOP;
     uint32_t    VDOP;
}loc_precisionDilution;

/**
 *  This structure contains Sensor Data Usage info.
 *
 *  \param  usageMask
 *          - Specifies which sensors were used in calculating the position
 *            in the position report.
 *            - Value
 *              - 0x00000001 - Accelerometer used
 *              - 0x00000002 - Gyroscope used
 *  \param  aidingIndicatorMask
 *          - Specifies which results were aided by sensors.
 *            - Value
 *             - 0x00000001 - AIDED_HEADING
 *             - 0x00000002 - AIDED_SPEED
 *             - 0x00000004 - AIDED_POSITION
 *             - 0x00000008 - AIDED_VELOCITY
 *
 */

typedef struct {
     uint32_t     usageMask;
     uint32_t     aidingIndicatorMask;
}loc_sensorDataUsage;

/**
 *  This structure contains SVs Used to Calculate the Fix.
 *
 *  \param  gnssSvUsedList_len
 *          - Number of sets of gnssSvUsedList
 *  \param  pGnssSvUsedList
 *          - Entry in the list contains the SV ID of a satellite
 *            used for calculating this position report.
 *          - Following information is associated with each SV ID:
 *            - GPS - 1 to 32
 *            - SBAS - 33 to 64
 *            - GLONASS - 65 to 96
 *            - QZSS - 193 to 197
 *            - BDS - 201 to 237
 *
 */

typedef struct {
     uint8_t     gnssSvUsedList_len ;
     uint16_t     gnssSvUsedList[LOC_UINT8_MAX_STRING_SZ];
}loc_svUsedforFix;

/**
 *  This structure contains GPS Time info.
 *
 *  \param  gpsWeek
 *          - Current GPS week as calculated from midnight, Jan. 6, 1980.
 *          - Units - Weeks
 *  \param  gpsTimeOfWeekMs
 *          - Amount of time into the current GPS week.
 *          - Units - Milliseconds
 *
 */

typedef struct {
     uint16_t     gpsWeek;
     uint32_t    gpsTimeOfWeekMs;
}loc_gpsTime;

/**
 *  This structure contains Event Position Report Indication unpack
 *
 *  \param  sessionStatus
 *          - Values
 *            - 0 - Session was successful
 *            - 1 - Session is still in progress; further position reports will be generated
 *                  until either the fix criteria specified by the client are met or the client
 *                  response timeout occurs.
 *            - 2 - Session failed..
 *            - 3 - Fix request failed because the session timed out.
 *            - 4 - Fix request failed because the session was ended by the user.
 *            - 5 - Fix request failed due to bad parameters in the request.
 *            - 6 - Fix request failed because the phone is offline.
 *            - 7 - Fix request failed because the engine is locked
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  sessionId
 *             - ID of the session that was specified in the Start request
 *             - Range - 0 to 255
 *             - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  \param  pLatitude
 *             - Latitude (specified in WGS84 datum)
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -90.0 to 90.0
 *             - Positive values indicate northern latitude
 *             - Negative values indicate southern latitude
 *             - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pLongitude
 *             - Longitude (specified in WGS84 datum)
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -180.0 to 180.0
 *             - Positive values indicate eastern latitude
 *             - Negative values indicate western latitude
 *             - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *   \param  pHorUncCircular
 *             - Horizontal position uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *   \param  pHorUncEllipseSemiMinor
 *             - Semi-minor axis of horizontal elliptical uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *   \param  pHorUncEllipseSemiMajor
 *             - Semi-major axis of horizontal elliptical uncertainty.
 *             - Units: Meters
 *             - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *   \param  pHorUncEllipseOrientAzimuth
 *             - Elliptical horizontal uncertainty azimuth of orientation.
 *             - Units - Decimal degrees
 *             - Range - 0 to 180
 *             - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *   \param  pHorConfidence
 *             - Horizontal uncertainty confidence.
 *             - If both elliptical and horizontal uncertainties
 *               are specified in this message, the confidence
 *               corresponds to the elliptical uncertainty.
 *             - Units - Percentage
 *             - Range 0-99
 *             - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *   \param  pHorReliability
 *          - Values
 *            - 0 - Location reliability is not set.
 *            - 1 - Location reliability is very low; use it at
 *                  your own risk
 *            - 2 - Location reliability is low; little or no
 *                  cross-checking is possible.
 *            - 3 - Location reliability is medium; limited cross-check passed
 *            - 4 - Location reliability is high; strong cross-check passed
 *         - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *   \param  pSpeedHorizontal
 *             - Horizontal speed.
 *             - Units - Meters/second
 *             - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *   \param  pSpeedUnc
 *             - 3-D Speed uncertainty.
 *             - Units - Meters/second.
 *             - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *   \param  pAltitudeWrtEllipsoid
 *             - Altitude With Respect to WGS84 Ellipsoid.
 *             - Units - Meters
 *             - Range -500 to 15883
 *             - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *   \param  pAltitudeWrtMeanSeaLevel
 *             - Altitude With Respect to Sea Level.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *   \param  pVertUnc
 *             - Vertical uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *   \param  pVertConfidence
 *             - Vertical uncertainty confidence.
 *             - Units - Percentage
 *             - Range 0 to 99
 *             - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *   \param  pVertReliability
 *          - Values
 *             - 0 - Location reliability is not set.
 *             - 1 - Location reliability is very low;
 *                   use it at your own risk.
 *             - 2 - Location reliability is low; little or no
 *                   cross-checking is possible
 *             - 3 - Location reliability is medium; limited
 *                   cross-check passed
 *             - 4 - Location reliability is high; strong
 *                   cross-check passed
 *             - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *   \param  pSpeedVertical
 *             - Vertical speed.
 *             - Units - Meters/second
 *             - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *   \param  pHeading
 *             - Heading.
 *             - Units - Degree
 *             - Range 0 to 359.999
 *             - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *   \param  pHeadingUnc
 *             - Heading uncertainty.
 *             - Units - Degree
 *             - Range 0 to 359.999
 *             - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *   \param  pMagneticDeviation
 *             - Difference between the bearing to true north and the
 *               bearing shown on a magnetic compass. The deviation is
 *               positive when the magnetic north is east of true north.
 *             - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *   \param  pTechnologyMask
 *          - Values
 *             - 0x00000001 - Satellites were used to generate the fix
 *             - 0x00000002 - Cell towers were used to generate the fix
 *             - 0x00000004 - Wi-Fi access points were used to generate the fix
 *             - 0x00000008 - Sensors were used to generate the fix
 *             - 0x00000010 - Reference Location was used to generate the fix
 *             - 0x00000020 - Coarse position injected into the location engine
 *                            was used to generate the fix
 *             - 0x00000040 - AFLT was used to generate the fix
 *             - 0x00000080 - GNSS and network-provided measurements were
 *                            used to generate the fix
 *          - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *  \param  pPrecisionDilution
 *            - See \ref loc_precisionDilution for more information
 *            - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *   \param  pTimestampUtc
 *             - UTC timestamp
 *             - Units - Milliseconds since Jan. 1, 1970
 *             - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *   \param  pLeapSeconds
 *             - Leap second information. If leapSeconds is not available,
 *               timestampUtc is calculated based on a hard-coded value
 *               for leap seconds.
 *             - Units - Seconds
 *             - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *  \param  pGpsTime
 *            - See \ref loc_gpsTime for more information
 *            - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *   \param  pTimeUnc
 *             - Time uncertainty.
 *             - Units - Milliseconds
 *             - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *   \param  pTimeSrc
 *          - Values
 *             - 0 - Invalid time.
 *             - 1 - Time is set by the 1X system.
 *             - 2 - Time is set by WCDMA/GSM time tagging.
 *             - 3 - Time is set by an external injection.
 *             - 4 - Time is set after decoding over-the-air GPS navigation
 *                   data from one GPS satellite.
 *             - 5 - Time is set after decoding over-the-air GPS navigation
 *                   data from multiple satellites.
 *             - 6 - Both time of the week and the GPS week number
 *                   are known.
 *             - 7 - Time is set by the position engine after the
 *                   fix is obtained
 *             - 8 - Time is set by the position engine after performing SFT,
 *                   this is done when the clock time uncertainty is large.
 *             - 9 - Time is set after decoding GLO satellites.
 *             - 10- Time is set after transforming the GPS to GLO time
 *             - 11- Time is set by the sleep time tag provided by
 *                   the WCDMA network.
 *             - 12- Time is set by the sleep time tag provided by the
 *                   GSM network
 *             - 13- Source of the time is unknown
 *             - 14- Time is derived from the system clock (better known
 *                   as the slow clock); GNSS time is maintained
 *                   irrespective of the GNSS receiver state
 *             - 15- Time is set after decoding QZSS satellites.
 *             - 16- Time is set after decoding BDS satellites.
 *           - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  \param  pSensorDataUsage
 *            - See \ref loc_sensorDataUsage for more information
 *            - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *   \param  pFixId
 *             - Fix count for the session. Starts with 0 and increments
 *               by one for each successive position report for a
 *               particular session.
 *             - Bit to check in ParamPresenceMask - <B>43</B>
 *
 *  \param  pSvUsedforFix
 *            - See \ref loc_svUsedforFix for more information
 *            - Bit to check in ParamPresenceMask - <B>44</B>
 *
 *   \param  pAltitudeAssumed
 *             - Indicates whether altitude is assumed or calculated.
 *             - Value
 *               - 0x00 - Altitude is calculated
 *               - 0x01 - Altitude is assumed
 *             - Bit to check in ParamPresenceMask - <B>45</B>
 *
 *
 */

typedef struct{
    uint32_t                    sessionStatus;
    uint8_t                     sessionId;
    uint64_t                    *pLatitude;
    uint64_t                    *pLongitude;
    uint32_t                    *pHorUncCircular;
    uint32_t                    *pHorUncEllipseSemiMinor;
    uint32_t                    *pHorUncEllipseSemiMajor;
    uint32_t                    *pHorUncEllipseOrientAzimuth;
    uint8_t                     *pHorConfidence;
    uint32_t                    *pHorReliability;
    uint32_t                    *pSpeedHorizontal;
    uint32_t                    *pSpeedUnc;
    uint32_t                    *pAltitudeWrtEllipsoid;
    uint32_t                    *pAltitudeWrtMeanSeaLevel;
    uint32_t                    *pVertUnc;
    uint8_t                     *pVertConfidence;
    uint32_t                    *pVertReliability;
    uint32_t                    *pSpeedVertical;
    uint32_t                    *pHeading;
    uint32_t                    *pHeadingUnc;
    uint32_t                    *pMagneticDeviation;
    uint32_t                    *pTechnologyMask;
    loc_precisionDilution       *pPrecisionDilution;
    uint64_t                    *pTimestampUtc;
    uint8_t                     *pLeapSeconds;
    loc_gpsTime                 *pGpsTime;
    uint32_t                    *pTimeUnc;
    uint32_t                    *pTimeSrc;
    loc_sensorDataUsage         *pSensorDataUsage;
    uint32_t                    *pFixId;
    loc_svUsedforFix            *pSvUsedforFix;
    uint8_t                     *pAltitudeAssumed;
    uint16_t                    Tlvresult;
    swi_uint256_t               ParamPresenceMask;
}unpack_loc_PositionRpt_Ind_t;

/**
 * Loc Position Report Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_PositionRpt_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_PositionRpt_Ind_t *pOutput
);

/**
 *  This structure contains LOC Engine State field.
 *
 *  @param  engineState
 *          - Location engine state.
 *          - Valid values
 *            - 1 - Location engine is on
 *            - 2 - Loction engine is off
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 */
typedef struct
{
    uint32_t engineState;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_EngineState_Ind_t;

/**
 * Loc Engine State Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_EngineState_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_EngineState_Ind_t *pOutput
);


/**
 *  This structure contains LOC Set External Power Configure status field.
 *
 *  @param  status
 *          - Valid values
 *            - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 */
typedef struct
{
    uint32_t status;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_SetExtPowerConfig_Ind_t;

/**
 * Loc Set External Power Configure Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SetExtPowerConfig_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SetExtPowerConfig_Ind_t *pOutput
);

/**
 * This structure contains Set Operation Mode unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_SLQSLOCGetBestAvailPos_t;

/**
 * This structure contains Set Operation Mode pack
 *
 * \param xid
 *        - Identifies the transaction.
 *        - The transaction ID is returned in the Get Best Available
 *          Position indication.
 *
 * \param Tlvresult
 *        - Pack result.
 *
 */
typedef struct{
    uint32_t xid;
    uint16_t Tlvresult;
}pack_loc_SLQSLOCGetBestAvailPos_t;

/**
 * Get Best Avail position pack
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCGetBestAvailPos(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCGetBestAvailPos_t *reqArg
);

/**
 * Get Best Avail position unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCGetBestAvailPos(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCGetBestAvailPos_t *pOutput
);

/**
 *  This structure contains Best Available Position
 *
 *  \param  status
 *          - Valid values:
 *             - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *             - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of
 *               a general failure
 *             - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is
 *               not supported
 *             - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because
 *               it contained invalid parameters
 *             - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine
 *               is busy
 *             - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone
 *               is offline
 *             - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *             - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) - Request failed because
 *               an undefined configuration was requested
 *             - eQMI_LOC_INSUFFICIENT_MEMORY (8) - Request failed because
 *               the engine could not allocate sufficient memory for the request
 *             - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) - Request failed because
 *               the maximum number of Geofences are already programmed
 *             - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) - Location service failed
 *               because of an XTRA version-based file format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  xid
 *             Transaction ID that was specified in the Get Best Available Position request.
 *             - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  \param  pLatitude
 *             - Latitude (specified in WGS84 datum)
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -90.0 to 90.0
 *             - Positive values indicate northern latitude
 *             - Negative values indicate southern latitude
 *             - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  \param  pLongitude
 *             - Longitude (specified in WGS84 datum)
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -180.0 to 180.0
 *             - Positive values indicate eastern latitude
 *             - Negative values indicate western latitude
 *             - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *   \param  pHorUncCircular
 *             - Horizontal position uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *   \param  pAltitudeWrtEllipsoid
 *             - Altitude With Respect to WGS84 Ellipsoid.
 *             - Units - Meters
 *             - Range -500 to 15883
 *             - Bit to check in ParamPresenceMask - <B>20</B>
 *
 *   \param  pVertUnc
 *             - Vertical uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>21</B>
 *
 *   \param  pTimestampUtc
 *             - UTC timestamp
 *             - Units - Milliseconds since Jan. 1, 1970
 *             - Bit to check in ParamPresenceMask - <B>22</B>
 *
 *   \param  pTimeUnc
 *             - Time uncertainty.
 *             - Units - Milliseconds
 *             - Bit to check in ParamPresenceMask - <B>23</B>
 *
 *   \param  pHorUncEllipseSemiMinor
 *             - Semi-minor axis of horizontal elliptical uncertainty.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>24</B>
 *
 *   \param  pHorUncEllipseSemiMajor
 *             - Semi-major axis of horizontal elliptical uncertainty.
 *             - Units: Meters
 *             - Bit to check in ParamPresenceMask - <B>25</B>
 *
 *   \param  pHorUncEllipseOrientAzimuth
 *             - Elliptical horizontal uncertainty azimuth of orientation.
 *             - Units - Decimal degrees
 *             - Range - 0 to 180
 *             - Bit to check in ParamPresenceMask - <B>26</B>
 *
 *   \param  pHorCirConf
 *           - Horizontal circular uncertainty confidence
 *           - Units: Precent
 *           - Range: 0 to 99
 *           - Bit to check in ParamPresenceMask - <B>27</B>
 *
 *   \param  pHorEllpConf
 *           - Horizontal elliptical uncertainty confidence
 *           - Units: Precent
 *           - Range: 0 to 99
 *           - Bit to check in ParamPresenceMask - <B>28</B>
 *
 *   \param  pHorReliability
 *          - Values
 *            - 0 - Location reliability is not set.
 *            - 1 - Location reliability is very low; use it at
 *                  your own risk
 *            - 2 - Location reliability is low; little or no
 *                  cross-checking is possible.
 *            - 3 - Location reliability is medium; limited cross-check passed
 *            - 4 - Location reliability is high; strong cross-check passed
 *          - Bit to check in ParamPresenceMask - <B>29</B>
 *
 *   \param  pSpeedHorizontal
 *             - Horizontal speed.
 *             - Units - Meters/second
 *             - Bit to check in ParamPresenceMask - <B>30</B>
 *
 *   \param  pSpeedUnc
 *             - 3-D Speed uncertainty.
 *             - Units - Meters/second.
 *             - Bit to check in ParamPresenceMask - <B>31</B>
 *
 *   \param  pAltitudeWrtMeanSeaLevel
 *             - Altitude With Respect to Sea Level.
 *             - Units - Meters
 *             - Bit to check in ParamPresenceMask - <B>32</B>
 *
 *   \param  pVertConfidence
 *             - Vertical uncertainty confidence.
 *             - Units - Percentage
 *             - Range 0 to 99
 *             - Bit to check in ParamPresenceMask - <B>33</B>
 *
 *   \param  pVertReliability
 *          - Values
 *             - 0 - Location reliability is not set.
 *             - 1 - Location reliability is very low;
 *                   use it at your own risk.
 *             - 2 - Location reliability is low; little or no
 *                   cross-checking is possible
 *             - 3 - Location reliability is medium; limited
 *                   cross-check passed
 *             - 4 - Location reliability is high; strong
 *                   cross-check passed
 *          - Bit to check in ParamPresenceMask - <B>34</B>
 *
 *   \param  pSpeedVertical
 *             - Vertical speed.
 *             - Units - Meters/second
 *             - Bit to check in ParamPresenceMask - <B>35</B>
 *
 *   \param  pSpeedVerticalUnc
 *           - Veritical speed
 *           - Units: Meters/second
 *           - Bit to check in ParamPresenceMask - <B>36</B>
 *
 *   \param  pHeading
 *             - Heading.
 *             - Units - Degree
 *             - Range 0 to 359.999
 *             - Bit to check in ParamPresenceMask - <B>37</B>
 *
 *   \param  pHeadingUnc
 *             - Heading uncertainty.
 *             - Units - Degree
 *             - Range 0 to 359.999
 *             - Bit to check in ParamPresenceMask - <B>38</B>
 *
 *   \param  pMagneticDeviation
 *             - Difference between the bearing to true north and the
 *               bearing shown on a magnetic compass. The deviation is
 *               positive when the magnetic north is east of true north.
 *             - Bit to check in ParamPresenceMask - <B>39</B>
 *
 *   \param  pTechnologyMask
 *          - Values
 *             - 0x00000001 - Satellites were used to generate the fix
 *             - 0x00000002 - Cell towers were used to generate the fix
 *             - 0x00000004 - Wi-Fi access points were used to generate the fix
 *             - 0x00000008 - Sensors were used to generate the fix
 *             - 0x00000010 - Reference Location was used to generate the fix
 *             - 0x00000020 - Coarse position injected into the location engine
 *                            was used to generate the fix
 *             - 0x00000040 - AFLT was used to generate the fix
 *             - 0x00000080 - GNSS and network-provided measurements were
 *                            used to generate the fix
 *          - Bit to check in ParamPresenceMask - <B>40</B>
 *
 *  \param  -pPrecisionDilution
 *             - See \ref loc_precisionDilution for more information
 *             - Bit to check in ParamPresenceMask - <B>41</B>
 *
 *  \param  -pGpsTime
 *             - See \ref loc_gpsTime for more information
 *             - Bit to check in ParamPresenceMask - <B>42</B>
 *
 *  \param  pTimeSrc
 *          - Values
 *             - 0 - Invalid time.
 *             - 1 - Time is set by the 1X system.
 *             - 2 - Time is set by WCDMA/GSM time tagging.
 *             - 3 - Time is set by an external injection.
 *             - 4 - Time is set after decoding over-the-air GPS navigation
 *                   data from one GPS satellite.
 *             - 5 - Time is set after decoding over-the-air GPS navigation
 *                   data from multiple satellites.
 *             - 6 - Both time of the week and the GPS week number
 *                   are known.
 *             - 7 - Time is set by the position engine after the
 *                   fix is obtained
 *             - 8 - Time is set by the position engine after performing SFT,
 *                   this is done when the clock time uncertainty is large.
 *             - 9 - Time is set after decoding GLO satellites.
 *             - 10- Time is set after transforming the GPS to GLO time
 *             - 11- Time is set by the sleep time tag provided by
 *                   the WCDMA network.
 *             - 12- Time is set by the sleep time tag provided by the
 *                   GSM network
 *             - 13- Source of the time is unknown
 *             - 14- Time is derived from the system clock (better known
 *                   as the slow clock); GNSS time is maintained
 *                   irrespective of the GNSS receiver state
 *             - 15- Time is set after decoding QZSS satellites.
 *             - 16- Time is set after decoding BDS satellites.
 *          - Bit to check in ParamPresenceMask - <B>43</B>
 *
 *  \param  -pSensorDataUsage
 *             - See \ref loc_sensorDataUsage for more information
 *             - Bit to check in ParamPresenceMask - <B>44</B>
 *
 *  \param  -pSvUsedforFix
 *             - See \ref loc_svUsedforFix for more information
 *             - Bit to check in ParamPresenceMask - <B>45</B>
 *
 */

typedef struct{
    uint32_t                    status;
    uint32_t                    *pXid;
    uint64_t                    *pLatitude;
    uint64_t                    *pLongitude;
    uint32_t                    *pHorUncCircular;
    uint32_t                    *pAltitudeWrtEllipsoid;
    uint32_t                    *pVertUnc;
    uint64_t                    *pTimestampUtc;
    uint32_t                    *pTimeUnc;
    uint32_t                    *pHorUncEllipseSemiMinor;
    uint32_t                    *pHorUncEllipseSemiMajor;
    uint32_t                    *pHorUncEllipseOrientAzimuth;
    uint8_t                     *pHorCirConf; //doc
    uint8_t                     *pHorEllpConf; //doc
    uint32_t                    *pHorReliability;
    uint32_t                    *pSpeedHorizontal;
    uint32_t                    *pSpeedUnc;
    uint32_t                    *pAltitudeWrtMeanSeaLevel;
    uint8_t                     *pVertConfidence;
    uint32_t                    *pVertReliability;
    uint32_t                    *pSpeedVertical;
    uint32_t                    *pSpeedVerticalUnc;
    uint32_t                    *pHeading;
    uint32_t                    *pHeadingUnc;
    uint32_t                    *pMagneticDeviation;
    uint32_t                    *pTechnologyMask;
    loc_precisionDilution       *pPrecisionDilution;
    loc_gpsTime                 *pGpsTime;
    uint32_t                    *pTimeSrc;
    loc_sensorDataUsage         *pSensorDataUsage;
    loc_svUsedforFix            *pSvUsedforFix;
    uint16_t                    Tlvresult;
    swi_uint256_t               ParamPresenceMask;
}unpack_loc_BestAvailPos_Ind_t;

/**
 * Loc Best Avial position Indication unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_BestAvailPos_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_BestAvailPos_Ind_t *pOutput
);

/**
 *  This structure contains LOC Set External Power Configure status field.
 *
 *  @param  status
 *           - Status of the Set Operation Mode request.
 *           - Valid values:
 *             - 0 - Request was completed successfully
 *             - 1 - Request failed because of a general failure
 *             - 2 - Request failed because it is not supported
 *             - 3 - Request failed because it contained invalid parameters
 *             - 4 - Request failed because the engine is busy
 *             - 5 - Request failed because the phone is offline
 *             - 6 - Request failed because it timed out
 *             - 7 - Request failed because an undefined configuration was requested
 *             - 8 - Request failed because the engine could not allocate sufficient memory for the request
 *             - 9 - Request failed because the maximum number of Geofences are already programmed
 *             - 10 - Location service failed because of an XTRA version-based file format check failure
 *           - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 */
typedef struct
{
    uint32_t status;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_SetOperationMode_Ind_t;

/**
 * Unpack the engine to use the specified operation mode.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SetOperationMode_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SetOperationMode_Ind_t *pOutput
);

/**
 *  Contain the parameters passed for SetLocDeleteAssistDataCallback by
 *  the device.
 *
 *  @param  status
 *          - Status of the Delete Assist Data request
 *          - Valid values:
 *             - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *             - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of
 *               a general failure
 *             - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is
 *               not supported
 *             - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because
 *               it contained invalid parameters
 *             - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine
 *               is busy
 *             - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone
 *               is offline
 *             - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *             - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) - Request failed because
 *               an undefined configuration was requested
 *             - eQMI_LOC_INSUFFICIENT_MEMORY (8) - Request failed because
 *               the engine could not allocate sufficient memory for the request
 *             - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) - Request failed because
 *               the maximum number of Geofences are already programmed
 *             - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) - Location service
 *               failed because of an XTRA version-based file format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 */
typedef struct
{
    uint32_t status;
    uint16_t Tlvresult;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_DeleteAssistData_Ind_t;

/**
 * Unpack the status of delete the location engine assistance data
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_DeleteAssistData_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_DeleteAssistData_Ind_t *pOutput
);

/**
 *  Contain fields in struct loc_satelliteInfo
 *
 *  @param  svListLen
 *          - number of sets of the following elements:
 *              - validMask
 *              - system
 *              - gnssSvid
 *              - healthStatus
 *              - svStatus
 *              - svInfoMask
 *              - elevation
 *              - azimuth
 *              - snr
 *
 *  @param  validMask
 *          - Bitmask indicating which of the fields in this TLV are valid. Valid bitmasks:
 *              - 0x00000001 - VALID_SYSTEM
 *              - 0x00000002 - VALID_GNSS_SVID
 *              - 0x00000004 - VALID_HEALTH_STATUS
 *              - 0x00000008 - VALID_PROCESS_STATUS
 *              - 0x00000010 - VALID_SVINFO_MASK
 *              - 0x00000020 - VALID_ELEVATION
 *              - 0x00000040 - VALID_AZIMUTH
 *              - 0x00000080 - VALID_SNR
 *
 *  @param  system
 *          - Indicates to which constellation this SV belongs. Valid values:
 *              - eQMI_LOC_SV_SYSTEM_GPS (1) - GPS satellite
 *              - eQMI_LOC_SV_SYSTEM_GALILEO (2) - GALILEO satellite
 *              - eQMI_LOC_SV_SYSTEM_SBAS (3) - SBAS satellite
 *              - eQMI_LOC_SV_SYSTEM_COMPASS (4) - COMPASS satellite
 *              - eQMI_LOC_SV_SYSTEM_GLONASS (5) - GLONASS satellite
 *              - eQMI_LOC_SV_SYSTEM_BDS (6) - BDS satellite
 *
 *  @param  gnssSvId
 *          - GNSS SV ID. The GPS and GLONASS SVs can be disambiguated using the system field.
 *            Range:
 *              - FOR GPS: 1 to 32
 *              - FOR GLONASS: 1 to 32
 *              - FOR SBAS: 120 to 151
 *              - for BDS: 201 to 237
 *
 *  @param  healthStatus
 *          - health status. Range: 0 - 1
 *              - 0 - unhealthy
 *              - 1 - healthy
 *
 *  @param  svStatus
 *          - SV process status. Valid values:
 *              - eQMI_LOC_SV_STATUS_IDLE (1) - SV is not being actively processed
 *              - eQMI_LOC_SV_STATUS_SEARCH (2) - The system is searching for this SV
 *              - eQMI_LOC_SV_STATUS_TRACK (3) - SV is being tracked
 *
 *  @param  svInfoMask
 *          - Indicates whether almanac and ephemeris information is available.
 *            Valid bitmasks:
 *              - 0x01 - SVINFO_HAS_EPHEMERIS
 *              - 0x02 - SVINFO_HAS_ALMANAC
 *
 *  @param  elevation
 *          - SV elevation angle.
 *              - Units: Degrees
 *              - Range: 0 to 90
 *
 *  @param  azimuth
 *          - SV azimuth angle.
 *              - Units: Degrees
 *              - Range: 0 to 360
 *
 *  @param  snr
 *           - SV signal-to-noise ratio
 *               - Units: dB-Hz
 *
 */
typedef struct
{
    uint8_t   svListLen;
    uint32_t  validMask;
    uint32_t  system;
    uint16_t  gnssSvId;
    uint8_t   healthStatus;
    uint32_t  svStatus;
    uint8_t   svInfoMask;
    float     elevation;
    float     azimuth;
    float     snr;
} loc_satelliteInfo;

/**
 *  Contain the parameters passed for SetLocGnssSvInfoCallback by
 *  the device.
 *
 *  @param  altitudeAssumed
 *          - Indicates whether altitude is assumed or calculated
 *              - 0x00 (FALSE) - Valid altitude is calculated
 *              - 0x01 (TRUE) - Valid altitude is assumed; there may not be enough
 *                satellites to determine precise altitude
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pSatelliteInfo
 *          - See @ref loc_satelliteInfo for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 *
 */
typedef struct
{
    uint8_t           altitudeAssumed;
    loc_satelliteInfo *pSatelliteInfo;
    uint16_t          Tlvresult;
    swi_uint256_t     ParamPresenceMask;
} unpack_loc_GnssSvInfo_Ind_t;

/**
 * Unpack the GNSS SV Info Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_GnssSvInfo_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_GnssSvInfo_Ind_t *pOutput
);

/**
 *  This structure contains inject UTC time parameter.
 *  \param  timeMsec
 *          - The UTC time since Jan. 1, 1970
 *
 *  \param  timeUncMsec
 *           - The time Uncertainty
 */
typedef struct{
    uint64_t timeMsec;
    uint32_t timeUncMsec;
}pack_loc_SLQSLOCInjectUTCTime_t;

typedef unpack_result_t unpack_loc_SLQSLOCInjectUTCTime_t;

/**
 * Pack inject UTC time.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCInjectUTCTime(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCInjectUTCTime_t *reqArg
);

/**
 * Unpack inject UTC time.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCInjectUTCTime(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCInjectUTCTime_t *pOutput
);

/**
 * This structure specifies information regarding the altitude source
 *
 * \param source
 *        - Specifies the source of the altitude
 *        - Valid values
 *           - 0 - Source is unknown
 *           - 1 - GPS is the source
 *           - 2 - Cell ID provided the source
 *           - 3 - Source is enhanced cell ID
 *           - 4 - Wi-Fi is the source
 *           - 5 - Terrestrial source
 *           - 6 - Hybrid terrestrial source
 *           - 7 - Altitude database is the source
 *           - 8 - Barometric altimeter is the source
 *           - 9 - Other sources
 *
 * \param linkage
 *        - Specifies the dependency between the horizontal
 *          and altitude position components
 *        - Valid values
 *           - 0 - Not specified
 *           - 1 - Fully interdependent
 *           - 2 - Depends on latitude and longitude
 *           - 3 - Fully independent
 *
 * \param coverage
 *        - Specifies the region of uncertainty.
 *        - Valid values
 *           - 0 - Not specified
 *           - 1 - Altitude uncertainty is valid at the injected
 *                 horizontal position coordinates only
 *           - 2 - Altitude uncertainty applies to the position
 *                 of the device regardless of horizontal position
 *
*/
typedef struct{
    uint32_t    source;
    uint32_t    linkage;
    uint32_t    coverage;
}altSrcInfo_t;

/**
 *  This structure contains LOC Inject Position parameters
 *  Please check has_<Param_Name> field for presence of optional parameters
 *
 *  \param  latitude
 *             - Optional parameter
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -90.0 to 90.0
 *             - Positive values indicate northern latitude
 *             - Negative values indicate southern latitude
 *
 *             - Note - This field must be specified together with
 *                        pLongitude and pHorUncCircular.
 *
 *  \param  longitude
 *             - Optional parameter
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -180.0 to 180.0
 *             - Positive values indicate eastern latitude
 *             - Negative values indicate western latitude
 *
 *             - Note - This field must be specified together with
 *                        pLatitude and pHorUncCircular.
 *
 *   \param  horUncCircular
 *             - Optional parameter
 *             - Horizontal position uncertainty.
 *             - Units - Meters
 *
 *             - Note - This field must be specified together with
 *                        pLatitude and pLongitude.
 *
 *   \param  horConfidence
 *             - Optional parameter
 *             - Horizontal confidence.
 *             - Units - Percent
 *             - Values
 *               - Valid Values - 1 to 99
 *               - Invalid Values - 0, 101 to 255
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      horizontal uncertainty. If not specified
 *                      when pHorUncCircular is set, the default
 *                      value is 50.
 *
 *   \param  horReliability
 *             - Optional parameter
 *               - Values
 *                 - 0 - Location reliability is not set.
 *                 - 1 - Location reliability is very low; use it at
 *                       your own risk
 *                 - 2 - Location reliability is low; little or no
 *                       cross-checking is possible.
 *                 - 3 - Location reliability is medium; limited cross-check passed
 *                 - 4 - Location reliability is high; strong cross-check passed
 *
 *   \param  altitudeWrtEllipsoid
 *             - Optional parameter
 *             - Altitude With Respect to Ellipsoid.
 *             - Units - Meters
 *             - Values
 *               - Positive - height
 *               - Negative = depth
 *
 *   \param  altitudeWrtMeanSeaLevel
 *             - Optional parameter
 *             - Altitude With Respect to Sea Level.
 *             - Units - Meters
 *
 *   \param  vertUnc
 *             - Optional parameter
 *             - Vertical uncertainty.
 *             - Units - Meters
 *
 *             - Note - This is mandatory if either pAltitudeWrtEllipsoid
 *                      or pAltitudeWrtMeanSeaLevel is specified.
 *
 *   \param  vertConfidence
 *             - Optional parameter
 *             - Vertical confidence.
 *             - Units - Percentage
 *             - Values
 *               - Valid Values - 0 to 99
 *               - Invalid Values - 0, 100-256
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      the vertical uncertainty. If not specified,
 *                      the default value will be 50.
 *
 *   \param  vertReliability
 *             - Optional parameter
 *               - Values
 *                  - 0 - Location reliability is not set.
 *                  - 1 - Location reliability is very low;
 *                        use it at your own risk.
 *                  - 2 - Location reliability is low; little or no
 *                        cross-checking is possible
 *                  - 3 - Location reliability is medium; limited
 *                        cross-check passed
 *                  - 4 - Location reliability is high; strong
 *                        cross-check passed
 *
 *   \param  altitudeSrcInfo
 *             - Optional parameter
 *               - Pointer to struct altitudeSrcInfo. See \ref altitudeSrcInfo for more information
 *
 *   \param  timestampUtc
 *             - Optional parameter
 *             - UTC timestamp
 *             - Units - Milliseconds since Jan. 1, 1970
 *
 *   \param  timestampAge
 *             - Optional parameter
 *             - Position age, which is an estimate of how long ago
 *               this fix was made.
 *             - Units - Milliseconds
 *
 *   \param  positionSrc
 *             - Optional parameter
 *             - Source from which this position was obtained
 *             - Valid values
 *               - 0 - Position source is GNSS
 *               - 1 - Position source is Cell ID
 *               - 2 - Position source is Enhanced Cell ID
 *               - 3 - Position source is Wi-Fi
 *               - 4 - Position source is Terrestrial
 *               - 5 - Position source is GNSS Terrestrial Hybrid
 *               - 6 - Other sources
 *
 *             - Note - If altitude is specified and the altitude
 *                      source is not specified, the engine assumes
 *                      that the altitude was obtained using the
 *                      specified position source.
 *                    - If both altitude and altitude source are
 *                      specified, the engine assumes that only
 *                      latitude and longitude were obtained using
 *                      the specified position source.
 *
 *   \param  rawHorUncCircular
 *             - Optional parameter
 *             - Horizontal position uncertainty (circular) without
 *               any optimization.
 *             - Units - Meters
 *
 *   \param  rawHorConfidence
 *             - Optional parameter
 *             - Horizontal confidence associated with raw horizontal
 *               uncertainty
 *             - Units: Percent
 *             - Values
 *               - Valid values - 1 to 99
 *               - Invalid values - 0, 101 to 255
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      raw horizontal uncertainty. If not specified
 *                      when rawHorUncCircular is set, the default value is 50.
 */
typedef struct{
    double          latitude;
    int             has_latitude;
    double          longitude;
    int             has_longitude;
    float           horUncCircular;
    int             has_horUncCircular;
    uint8_t         horConfidence;
    int             has_horConfidence;
    uint32_t        horReliability;
    int             has_horReliability;
    float           altitudeWrtEllipsoid;
    int             has_altitudeWrtEllipsoid;
    float           altitudeWrtMeanSeaLevel;
    int             has_altitudeWrtMeanSeaLevel;
    float           vertUnc;
    int             has_vertUnc;
    uint8_t         vertConfidence;
    int             has_vertConfidence;
    uint32_t        vertReliability;
    int             has_vertRelicability;
    altSrcInfo_t    altitudeSrcInfo;
    int             has_altitudeSrcInfo;
    uint64_t        timestampUtc;
    int             has_timestampUtc;
    uint32_t        timestampAge;
    int             has_timestampAge;
    uint32_t        positionSrc;
    int             has_positionSrc;
    float           rawHorUncCircular;
    int             has_rawHorUncCircular;
    uint8_t         rawHorConfidence;
    int             has_rawHorConfidence;
}pack_loc_SLQSLOCInjectPosition_t;

typedef unpack_result_t  unpack_loc_SLQSLOCInjectPosition_t;
/**
 * Pack inject position.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCInjectPosition(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCInjectPosition_t *reqArg
);

/**
 * Unpack inject position.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCInjectPosition(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCInjectPosition_t *pOutput
);

/**
 *  This structure contains parameters to set current cradle mount configuration
 *  Please check has_<Param_Name> field for presence of optional parameters
 *
 *  \param  state
 *             - Cradle Mount State
 *             - Valid values:
 *               - 0 - Device is mounted on the cradle
 *               - 1 - Device is not mounted on the cradle
 *               - 2 - Unknown cradle mount state
 *
 *  \param  confidence
 *             - Cradle Mount Confidence (Optional)
 *             - Confidence in the Cradle Mount state expressed as a
 *               percentage.
 *             - Range - 0 to 100
 */
typedef struct{
    uint32_t    state;
    int         has_confidence;
    uint8_t     confidence;
}pack_loc_SLQSLOCSetCradleMountConfig_t;

typedef unpack_result_t  unpack_loc_SLQSLOCSetCradleMountConfig_t;

/**
 * Pack set cradle mount configure.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCSetCradleMountConfig(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCSetCradleMountConfig_t *reqArg
);

/**
 * Unpack set cradle mount configure.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCSetCradleMountConfig(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCSetCradleMountConfig_t *pOutput
);

/**
 * This structure specifies information regarding the 3-Axis Sensor Data.
 * Please check has_<Param_Name> field for presence of optional parameters
 *
 * \param timeOfFirstSample
 *        - Denotes a full 32-bit time stamp of the first (oldest) sample in this message.
 *        - The time stamp is in the time reference scale that is used by the sensor time source.
 *        - Units - Milliseconds
 *
 * \param flags
 *        - Flags to indicate any deviation from the default measurement assumptions.
 *        - All unused bits in this field must be set to 0.
 *        - Valid bitmasks
 *           - 0x01 - Bitmask to specify that a sign reversal is required
 *                    while interpreting the sensor data; only applies to the
 *                    accelerometer samples
 *           - 0x02 - Bitmask to specify that the sensor time stamp is the same
 *                    as the modem time stamp
 *
 * \param sensorDataLen
 *        - Number of sets of the following elements
 *          - timeOffset
 *          - xAxis
 *          - yAxis
 *          - zAxis
 *
 * \param timeOffset
 *        - Sample time offset
 *        - Units - Milliseconds
 *
 * \param xAxis
 *        - Sensor x-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 *
 * \param yAxis
 *        - Sensor Y-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 *
 * \param xAxis
 *        - Sensor Z-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 */
typedef struct
{
    uint32_t    timeOfFirstSample ;
    uint8_t     flags;
    uint8_t     sensorDataLen;
    uint16_t    timeOffset[MAX_SENSOR_DATA_LEN];
    uint32_t    xAxis[MAX_SENSOR_DATA_LEN];
    uint32_t    yAxis[MAX_SENSOR_DATA_LEN];
    uint32_t    zAxis[MAX_SENSOR_DATA_LEN];
}sensorData_t;

/**
 * This structure specifies information regarding the Temperature Data.
 * Please check has_<Param_Name> field for presence of optional parameters
 *
 * \param timeSource
 *        - Time source of the sensor data
 *        - Valid values
 *          - 0 - Sensor time source is unspecified
 *          - 1 - Time source is common between the sensors and
 *                the location engine
 *
 * \param timeOfFirstSample
 *        - Denotes a full 32-bit time stamp of the first (oldest) sample in this message.
 *        - The time stamp is in the time reference scale that is used by the sensor time source.
 *        - Units - Milliseconds
 *
 * \param temperatureDataLen
 *        - Number of sets of the following elements
 *          - timeOffset
 *          - temperature
 *
 * \param timeOffset
 *        - Sample time offset
 *        - Units - Milliseconds
 *
 * \param temperature
 *        - Sensor temperature.
 *        - Type - Floating point
 *        - Units - Degrees Celsius
 *        - Range -50 to +100.00
 */
typedef struct
{
    uint32_t    timeSource;
    uint32_t    timeOfFirstSample;
    uint8_t     temperatureDataLen;
    uint16_t    timeOffset[MAX_TEMP_DATA_LEN];
    uint32_t    temperature[MAX_TEMP_DATA_LEN];
}tempData_t;

/**
 *  This structure contains parameters to inject sensor data into the GNSS location engine
 *  Please check has_<Param_Name> field for presence of optional parameters
 *
 *  \param  opaqueId
 *             - Opaque Identifier (Optional parameter)
 *             - An opaque identifier that is sent in by the client that
 *               will be echoed in the indication so the client can relate
 *               the indication to the request.
 *
 *   \param  acceleroData
 *             - 3-Axis Accelerometer Data (Optional parameter)
 *             - Pointer to struct sensorData. See \ref sensorData_t for more information
 *
 *   \param  gyroData
 *             - 3-Axis Cyroscope Data (Optional parameter)
 *             - Pointer to struct sensorData. See \ref sensorData_t for more information
 *
 *  \param  acceleroTimeSrc
 *             - 3-Axis Accelerometer Data Time Source (Optional parameter)
 *             - The location service uses this field to identify the time reference
 *               used in the accelerometer data time stamps.
 *             - If not specified, the location service assumes that the time source
 *               for the accelerometer data is unknown.
 *             - Valid values
 *                - 0 - Sensor time source is unspecified
 *                - 1 - Time source is common between the sensors and
 *                      the location engine
 *
 *  \param  gyroTimeSrc
 *             - 3-Axis Gyroscope Data Time Source (Optional)
 *             - The location service uses this field to identify the time reference
 *               used in the gyroscope data time stamps.
 *             - If not specified, the location service assumes that the time source
 *               for the gyroscope data is unknown.
 *             - Valid values
 *                - 0 - Sensor time source is unspecified
 *                - 1 - Time source is common between the sensors and
 *                      the location engine
 *
 *   \param  accelTemp
 *             - Accelerometer Temperature Data (Optional parameter)
 *             - Pointer to struct tempratureData. See \ref tempData_t for more information
 *
 *   \param  gyroTemp
 *             - Gyroscope Temperature Data (Optional parameter)
 *             - Pointer to struct tempratureData. See \ref tempData_t for more information
 *
 */
typedef struct{
    int             has_opaqueId;
    uint32_t        opaqueId;
    int             has_accleroData;
    sensorData_t    acceleroData;
    int             has_gyroData;
    sensorData_t    gyroData;
    int             has_acceleroTimeSrc;
    uint32_t        acceleroTimeSrc;
    int             has_gyroTimeSrc;
    uint32_t        gyroTimeSrc;
    int             has_accelTemp;
    tempData_t      accelTemp;
    int             has_gyroTemp;
    tempData_t      gyroTemp;
}pack_loc_SLQSLOCInjectSensorData_t;

typedef unpack_result_t  unpack_loc_SLQSLOCInjectSensorData_t;

/**
 * Pack inject sensor data.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCInjectSensorData(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCInjectSensorData_t *reqArg
);

/**
 * Unpack inject sensor data.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCInjectSensorData(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCInjectSensorData_t *pOutput
);

/**
 *  Contain the parameters passed for SetLocEventMaskNMEACallback by
 *  the device.
 *
 *  @param  NMEAData
 *          - NMEA string
 *          - Type - NULL terminated string
 *          - Maximum string length (including NULL terminator) - 201
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  Tlvresult
 *          - unpack result
 *
 */
typedef struct
{
    char              NMEAData[MAX_LOC_NMEA_STR_LEN];
    uint16_t          Tlvresult;
    swi_uint256_t     ParamPresenceMask;
} unpack_loc_EventNMEA_Ind_t;

/**
 * Unpack the NMEA Event Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_EventNMEA_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_EventNMEA_Ind_t *pOutput
);

/**
 *  This structure contains parameters to Gets the location server.
 *  Please check has_<Param_Name> field for presence of optional parameters
 *
 *  \param  serverType
 *             - Type of server.
 *             - Valid values
 *               - eQMI_LOC_SERVER_TYPE_CDMA_PDE (1) - Server type is CDMA PDE
 *               - eQMI_LOC_SERVER_TYPE_CDMA_MPC (2) - Server type is CDMA MPC
 *               - eQMI_LOC_SERVER_TYPE_UMTS_SLP (3) - Server type is UMTS SLP
 *               - eQMI_LOC_SERVER_TYPE_CUSTOM_PDE (4)- Server type is custom PDE
 *
 *   \param  serverAddrTypeMask
 *             - Optional parameter
 *             - Type of address the client wants. If unspecified, the indication will
 *               contain all the types of addresses it has for the specified server type.
 *             - Valid bitmasks
 *                - 0x01 - IPv4
 *                - 0x02 - IPv6
 *                - 0x04 - URL
 */
typedef struct{
    uint32_t    serverType;
    int         has_serverAddrTypeMask;
    uint8_t     serverAddrTypeMask;
}pack_loc_SLQSLOCGetServer_t;

typedef unpack_result_t  unpack_loc_SLQSLOCGetServer_t;

/**
 * Pack get server.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note   The request is acknowledged through the response, but the SUCCESS/FAILURE
 *         status is sent through \ref unpack_loc_GetServer_Ind callback
 *         If successful, the callback also contains the A-GPS server address.
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCGetServer(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCGetServer_t *reqArg
);

/**
 * Unpack get server.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out]  pOutput     response unpacked
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCGetServer(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SLQSLOCGetServer_t *pOutput
);

/**
 *  This structure contains LOC Server IPV4 info field.
 *
 *  @param  address
 *          - IPv4 address.
 *
 *  @param  port
 *          - IPv4 port.
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t  address;
    uint16_t  port;
    uint8_t   TlvPresent;
} loc_IPv4Info;

/**
 *  This structure contains LOC Get Server IPV6 info field.
 *
 *  @param  address
 *          - IPv6 address.
 *          - Type - Array of unsigned integers
 *          - Maximum length of the array - 8
 *
 *  @param  port
 *          - IPv6 port.
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint16_t address[8];
    uint32_t port;
    uint8_t  TlvPresent;
} loc_IPv6Info;

/**
 *  This structure contains LOC Get Server URL address field.
 *
 *  @param  address
 *          - Uniform Resource Locator
 *          - Type - NULL terminated string
 *          - Maximum string length (including NULL terminator) 256
 *
 *  @param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    char  address[256];
    uint8_t TlvPresent;
} loc_urlAddr;

/**
 *  Contain the parameters passed for SetLocGetServerCallback by
 *  the device.
 *
 *  @param  serverStatus
 *          - Status of the Get Server request.
 *          - Valid values
 *            - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *            - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of a general failure
 *            - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is not supported
 *            - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because it contained invalid parameters
 *            - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine is busy
 *            - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone is offline
 *            - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *            - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) - Request failed because an undefined configuration was requested
 *            - eQMI_LOC_INSUFFICIENT_MEMORY (8) - Request failed because the engine could not allocate sufficient memory for the request
 *            - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) - Request failed because the maximum number of Geofences are already programmed
 *            - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) - Location service failed because of an XTRA version-based file format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  serverType
 *          - Type of server.
 *          - Valid values
 *            - eQMI_LOC_SERVER_TYPE_CDMA_PDE (1) - Server type is CDMA PDE
 *            - eQMI_LOC_SERVER_TYPE_CDMA_MPC (2) - Server type is CDMA MPC
 *            - eQMI_LOC_SERVER_TYPE_UMTS_SLP (3) - Server type is UMTS SLP
 *            - eQMI_LOC_SERVER_TYPE_CUSTOM_PDE (4) - Server type is custom PDE
 *          - Bit to check in ParamPresenceMask - <B>2</B>
 *
 *  @param  pIPv4AddrInfo
 *          - See @ref loc_IPv4Info for more information.
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pIPv6AddrInfo
 *          - See @ref loc_IPv6Info for more information.
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pURL
 *          - See @ref loc_urlAddr for more information.
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *
 */
typedef struct
{
    uint32_t         serverStatus;
    uint32_t         serverType;
    loc_IPv4Info     *pIPv4AddrInfo;
    loc_IPv6Info     *pIPv6AddrInfo;
    loc_urlAddr      *pURL;
    uint16_t         Tlvresult;
    swi_uint256_t    ParamPresenceMask;
} unpack_loc_GetServer_Ind_t;

/**
 * Unpack the Get Server Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_GetServer_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_GetServer_Ind_t *pOutput
);

/**
 *  This structure contains LOC Cradle Mount Config Status
 *
 *  @param  cradleMountConfigStatus
 *          - Values
 *            - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 */
typedef struct
{
    uint32_t cradleMountConfigStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_CradleMountCallback_Ind_t;

/**
 * Unpack the Cradle Mount Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_CradleMountCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_CradleMountCallback_Ind_t *pOutput
);

/**
 *  This structure contains LOC Event Time Sync Reference COunter
 *
 *  @param  timeSyncRefCounter
 *          -  Sent by the location engine when it needs to synchronize
 *             location engine and control point (sensor processor) times.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 */
typedef struct
{
    uint32_t timeSyncRefCounter;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_EventTimeSyncCallback_Ind_t;

/**
 * Unpack the Event Time Sync indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_EventTimeSyncCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_EventTimeSyncCallback_Ind_t *pOutput
);

/**
 *  This structure contains LOC Inject Time Sync Data Status
 *
 *  @param  injectTimeSyncStatus
 *          - Values
 *            - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
+ *         - Bit to check in ParamPresenceMask - <B>1</B>
 */
typedef struct
{
    uint32_t injectTimeSyncStatus;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_InjectTimeSyncDataCallback_Ind_t;

/**
 * Unpack the inject time sync data indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_InjectTimeSyncDataCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_InjectTimeSyncDataCallback_Ind_t *pOutput
);

/**
 *  This structure contains LOC Inject Sensor Data
 *
 *  @param  injectSensorDataStatus
 *          - Values
  *           - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param  pOpaqueIdentifier[OPTIONAL]
 *             - Sent in by the client echoed so the client can
 *               relate the indication to the request.
 *             - NULL when this TLV not present
 *             - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  pAccelSamplesAccepted[OPTIONAL]
 *             - Lets the client know how many 3-axis accelerometer
 *               samples were accepted.
 *             - This field is present only if the accelerometer samples
 *               were sent in the request.
 *             - NULL when this TLV not present
 *             - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  pGyroSamplesAccepted[OPTIONAL]
 *             - Lets the client know how many 3-axis gyroscope
 *               samples were accepted.
 *             - This field is present only if the gyroscope samples
 *               were sent in the request.
 *             - NULL when this TLV not present
 *             - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *   @param  pAccelTempSamplesAccepted[OPTIONAL]
 *             - Lets the client know how many accelerometer temperature
 *               samples were accepted.
 *             - This field is present only if the accelerometer
 *               temperature samples were sent in the request.
 *             - NULL when this TLV not present
 *             - Bit to check in ParamPresenceMask - <B>19</B>
 *
 *   @param  pGyroTempSamplesAccepted[OPTIONAL]
 *             - Lets the client know how many gyroscope temperature
 *               samples were accepted.
 *             - This field is present only if the gyroscope temperature
 *               samples were sent in the request.
 *             - NULL when this TLV not present
 *             - Bit to check in ParamPresenceMask - <B>20</B>
 *
 */
typedef struct
{
    uint32_t injectSensorDataStatus;
    uint32_t *pOpaqueIdentifier;
    uint8_t  *pAccelSamplesAccepted;
    uint8_t  *pGyroSamplesAccepted;
    uint8_t  *pAccelTempSamplesAccepted;
    uint8_t  *pGyroTempSamplesAccepted;
    swi_uint256_t  ParamPresenceMask;
} unpack_loc_InjectSensorDataCallback_Ind_t;

/**
 * Unpack the inject sensor data indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_InjectSensorDataCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_InjectSensorDataCallback_Ind_t *pOutput
);

/**
 *  This structure contains Accelerometer Accept Ready Info
 *
 *  @param  injectEnable
 *          - GNSS location engine is ready to accept data from sensor.
 *          - Values
 *          - 0x01 - Ready to accept sensor data
 *          - 0x00 - Not ready to accept sensor data
 *
 *  @param  samplesPerBatch
 *          - number of samples per batch the GNSS location engine is to receive.
 *          - samplingFrequency = samplesPerBatch * batchesPerSecond
 *          - samplesPerBatch must be a nonzero positive value.
 *
 *  @param  batchPerSec
 *          - LTE NAS version minor
 *          - Number of sensor-data batches the GNSS location engine is to
 *            receive per second.
 *          - BatchesPerSecond must be a nonzero positive value.
 *
 */
typedef struct {
     uint8_t    injectEnable;
     uint16_t    samplesPerBatch;
     uint16_t    batchPerSec;
}loc_accelAcceptReady;

/**
 *  This structure contains Gyroscope Accept Ready Info
 *
 *  @param  injectEnable
 *          - GNSS location engine is ready to accept data from sensor.
 *          - Values
 *          - 0x01 - Ready to accept sensor data
 *          - 0x00 - Not ready to accept sensor data
 *
 *  @param  samplesPerBatch
 *          - number of samples per batch the GNSS location engine is to receive.
 *          - samplingFrequency = samplesPerBatch * batchesPerSecond
 *          - samplesPerBatch must be a nonzero positive value.
 *
 *  @param  batchPerSec
 *          - LTE NAS version minor
 *          - Number of sensor-data batches the GNSS location engine is to
 *            receive per second.
 *          - BatchesPerSecond must be a nonzero positive value.
 *
 */
typedef struct {
     uint8_t    injectEnable;
     uint16_t    samplesPerBatch;
     uint16_t    batchPerSec;
}loc_gyroAcceptReady;

/**
 *  This structure contains Accelerometer Temperature Accept Ready Info
 *
 *  @param  injectEnable
 *          - GNSS location engine is ready to accept data from sensor.
 *          - Values
 *          - 0x01 - Ready to accept sensor data
 *          - 0x00 - Not ready to accept sensor data
 *
 *  @param  samplesPerBatch
 *          - number of samples per batch the GNSS location engine is to receive.
 *          - samplingFrequency = samplesPerBatch * batchesPerSecond
 *          - samplesPerBatch must be a nonzero positive value.
 *
 *  @param  batchPerSec
 *          - LTE NAS version minor
 *          - Number of sensor-data batches the GNSS location engine is to
 *            receive per second.
 *          - BatchesPerSecond must be a nonzero positive value.
 *
 */
typedef struct {
     uint8_t    injectEnable;
     uint16_t    samplesPerBatch;
     uint16_t    batchPerSec;
}loc_accelTempAcceptReady;

/**
 *  This structure contains Gyroscope Temperature Accept Ready Info
 *
 *  @param  injectEnable
 *          - GNSS location engine is ready to accept data from sensor.
 *          - Values
 *          - 0x01 - Ready to accept sensor data
 *          - 0x00 - Not ready to accept sensor data
 *
 *  @param  samplesPerBatch
 *          - number of samples per batch the GNSS location engine is to receive.
 *          - samplingFrequency = samplesPerBatch * batchesPerSecond
 *          - samplesPerBatch must be a nonzero positive value.
 *
 *  @param  batchPerSec
 *          - LTE NAS version minor
 *          - Number of sensor-data batches the GNSS location engine is to
 *            receive per second.
 *          - BatchesPerSecond must be a nonzero positive value.
 *
 */
typedef struct {
     uint8_t    injectEnable;
     uint16_t    samplesPerBatch;
     uint16_t    batchPerSec;
}loc_gyroTempAcceptReady;

/**
 *  This structure contains LOC Event Sensor Streaming Ready Status
 *
 *  @param  -pAccelAcceptReady[OPTIONAL]
 *          - See @ref loc_accelAcceptReady for more information
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param  -pGyroAcceptReady[OPTIONAL]
 *          - See @ref loc_gyroAcceptReady for more information
 *          - Bit to check in ParamPresenceMask - <B>17</B>
 *
 *  @param  -pAccelTempAcceptReady[OPTIONAL]
 *          - See @ref loc_accelTempAcceptReady for more information
 *          - Bit to check in ParamPresenceMask - <B>18</B>
 *
 *  @param  -pGyroTempAcceptReady[OPTIONAL]
 *          - See @ref loc_gyroTempAcceptReady for more information
 *          - Bit to check in ParamPresenceMask - <B>19</B>
 *
 */
typedef struct{
    loc_accelAcceptReady       *pAccelAcceptReady;
    loc_gyroAcceptReady        *pGyroAcceptReady;
    loc_accelTempAcceptReady   *pAccelTempAcceptReady;
    loc_gyroTempAcceptReady    *pGyroTempAcceptReady;
    swi_uint256_t              ParamPresenceMask;
} unpack_loc_SensorStreamingCallback_Ind_t;

/**
 * Unpack the Sensor streaming status indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SensorStreamingCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SensorStreamingCallback_Ind_t *pOutput
);

/**
 *  Contain the parameters passed for Inject UTC time indication by
 *  the device.
 *
 *  @param  status
 *          - Status of the UTC Time Injection request
 *          - Valid values:
 *            - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @note    None
 *
 */
typedef struct
{
    uint32_t status;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_InjectUTCTimeCallback_Ind_t;

/**
 * Unpack the Inject UTC time indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_InjectUTCTimeCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_InjectUTCTimeCallback_Ind_t *pOutput
);

/**
 *  Contain the parameters passed for LOC Inject Position indication by
 *  the device.
 *
 *  @param  status
 *          - Position Injection Status
 *          - Valid values:
 *            - 0 - Request was completed successfully
 *            - 1 - Request failed because of a general failure.
 *            - 2 - Request failed because it is not supported.
 *            - 3 - Request failed because it contained invalid parameters
 *            - 4 - Request failed because the engine is busy
 *            - 5 - Request failed because the phone is offline
 *            - 6 - Request failed because it timed out
 *            - 7 - Request failed because an undefined configuration was requested
 *            - 8 - engine could not allocate sufficient memory
 *            - 9 - Request failed because the maximum number of
 *                  Geofences are already programmed
 *            - 10 -Location service failed because of an XTRA version-based file
 *                  format check failure
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 * @note    None
 *
 */
typedef struct
{
    uint32_t status;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_InjectPositionCallback_Ind_t;

/**
 * Unpack the Inject position indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_InjectPositionCallback_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_InjectPositionCallback_Ind_t *pOutput
);

/**
 * This structure specifies information regarding the IPv4 address and port.
 *
 * @param IPv4Addr
 *        - IPv4 address.
 *
 * @param IPv4Port
 *        - IPv4 port.
 */
typedef struct
{
    uint32_t    IPv4Addr;
    uint16_t    IPv4Port;
}loc_IPv4Config;

/**
 * This structure specifies information regarding the IPv6 address and port.
 *
 * @param IPv6Addr
 *        - IPv6 address.
 *        - Type - Array of unsigned integers
 *        - Maximum length of the array: 8
 *
 * @param IPv6Port
 *        - IPv6 port.
 */
typedef struct
{
    uint16_t    IPv6Addr[8];
    uint32_t    IPv6Port;
}loc_IPv6Config;

/**
 * This structure specifies information regarding the URL.
 *
 * @param urlAddr
 *        - URL address.
 *        - Type - NULL-terminated string
 *        - Maximum string length (including NULL terminator) - 256
 */
typedef struct
{
    char    urlAddr[256];
}loc_URLAddrInfo;

/**
 *  This structure contains parameters to set A-GPS Server
 *
 *  @param  serverType
 *             - Type of server
 *             - Valid values:
 *               - 1 - Server type is CDMA PDE
 *               - 2 - Server type is CDMA MPC
 *               - 3 - Server type is UMTS SLP
 *               - 4 - Server type is custom PDE
 *
 *  @param  pIPv4Config
 *             - IPv4 address and port (Optional)
 *             -  See @ref loc_IPv4Config for more information
 *
 *  @param  pIPv6Config
 *             - IPv6 address and port (Optional)
 *             -  See @ref loc_IPv6Config for more information
 *
 *  \param  pURLAddr
 *             - Uniform Resource Locator (Optional)
 *             -  See @ref loc_URLAddrInfo for more information
 */
typedef struct{
    uint32_t                serverType;
    loc_IPv4Config          *pIPv4Config;
    loc_IPv6Config          *pIPv6Config;
    loc_URLAddrInfo         *pURLAddr;
}pack_loc_SLQSLOCSetServer_t;

/**
 * Pack Set server.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @note   If multiple types of addresses are specified in the request, the IPv4 address
 *         takes precedence over the IPv6 address and the IPv6 address takes precedence
 *         over the URL address.The request is acknowledged through the response, but
 *         the SUCCESS/FAILURE status is sent through \ref unpack_loc_SetServer_Ind callback
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCSetServer(
    pack_qmi_t  *pCtx,
    uint8_t *pReqBuf,
    uint16_t *pLen,
    pack_loc_SLQSLOCSetServer_t *reqArg
);

/**
 * Unpack set server.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCSetServer(
    uint8_t *pResp,
    uint16_t respLen
);

/**
 *  Contain the parameters passed for SetLocSetServerCallback by
 *  the device.
 *
 *  @param  serverStatus
 *          - Status of the Get Server request.
 *          - Valid values
 *            - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *            - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of a general failure
 *            - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is not supported
 *            - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because it contained invalid parameters
 *            - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine is busy
 *            - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone is offline
 *            - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *            - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) - Request failed because an undefined configuration was requested
 *            - eQMI_LOC_INSUFFICIENT_MEMORY (8) - Request failed because the engine could not allocate sufficient memory for the request
 *            - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) - Request failed because the maximum number of Geofences are already programmed
 *            - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) - Location service failed because of an XTRA version-based file format check failure
 *            - 0xffffffff - Invalid data.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint32_t         serverStatus;
    uint16_t         Tlvresult;
    swi_uint256_t    ParamPresenceMask;
} unpack_loc_SetServer_Ind_t;

/**
 * Unpack the Set Server Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SetServer_Ind(
    uint8_t *pResp,
    uint16_t respLen,
    unpack_loc_SetServer_Ind_t *pOutput
);

/**
 * This structure contains Start LOC unpack
 *
 * \param Tlvresult
 *        - Unpack result.
 *        - Bit to check in ParamPresenceMask - <B>2</B>
 *
 * \param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 */

typedef struct{
    uint16_t       Tlvresult;
    swi_uint256_t  ParamPresenceMask;
}unpack_loc_SLQSLOCGetOpMode_t;

/**
 * Pack Get Operation mode.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_SLQSLOCGetOpMode(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    void        *reqArg
);

/**
 * Get Operation mode unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_SLQSLOCGetOpMode(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_loc_SLQSLOCGetOpMode_t *pOutput
);


/**
 *  Contain the parameters passed for SetLocGetOpModeCallback by
 *  the device.
 *
 *  @param  Status
 *          - Status of the Get Server request.
 *          - Valid values
 *            - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *            - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of a general failure
 *            - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is not supported
 *            - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because it contained invalid parameters
 *            - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine is busy
 *            - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone is offline
 *            - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *            - eQMI_LOC_CONFIG_NOT_SUPPORTED (7) - Request failed because an undefined configuration was requested
 *            - eQMI_LOC_INSUFFICIENT_MEMORY (8) - Request failed because the engine could not allocate sufficient memory for the request
 *            - eQMI_LOC_MAX_GEOFENCE_PROGRAMMED (9) - Request failed because the maximum number of Geofences are already programmed
 *            - eQMI_LOC_XTRA_VERSION_CHECK_FAILURE (10) - Location service failed because of an XTRA version-based file format check failure
 *            - 0xffffffff - Invalid data.
 *          - Bit to check in ParamPresenceMask - <B>1</B>
 *
 *  \param  pMode
 *          - Current operation mode.
 *          - Valid values:
 *            - eQMI_LOC_OPER_MODE_DEFAULT(1) - Use the default engine mode
 *            - eQMI_LOC_OPER_MODE_MSB (2)- Use the MS-based mode
 *            - eQMI_LOC_OPER_MODE_MSA (3)- Use the MS-assisted mode
 *            - eQMI_LOC_OPER_MODE_STANDALONE (4) - Use Standalone mode
 *            - eQMI_LOC_OPER_MODE_CELL_ID (5) - Use cell ID; this mode is only valid for GSM/UMTS networks
 *            - eQMI_LOC_OPER_MODE_WWAN(6) - Use WWAN measurements to calculate the position; if this mode is set, AFLT will be used for 1X networks and OTDOA will be used for LTE networks
 *          - Bit to check in ParamPresenceMask - <B>16</B>
 *
 *  @param ParamPresenceMask
 *          - bitmask representation to indicate valid parameters.
 *
 */
typedef struct
{
    uint32_t         Status;
    uint32_t         *pMode;
    swi_uint256_t    ParamPresenceMask;
} unpack_loc_GetOpMode_Ind_t;

/**
 * Unpack the Set Server Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_GetOpMode_Ind(
    uint8_t     *pResp,
    uint16_t    respLen,
    unpack_loc_GetOpMode_Ind_t *pOutput
);

/**
 * Pack Get Fix Criteria.
 * @param[in,out] pCtx qmi request context
 * @param[out] pReqBuf qmi request buffer
 * @param[out] pLen qmi request length
 * @param[in] reqArg request parameter
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int pack_loc_GetFixCriteria(
    pack_qmi_t  *pCtx,
    uint8_t     *pReqBuf,
    uint16_t    *pLen,
    void        *reqArg
);

typedef unpack_result_t unpack_loc_GetFixCriteria_t;

/**
 * Get Fix Criteria unpack
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_GetFixCriteria(
        uint8_t     *pResp,
        uint16_t    respLen,
        unpack_loc_GetFixCriteria_t *pOutput
);

/**
 * Contain fields in struct loc_FixCriteriaStatusTlv
 *
 *  \param  status
 *          - Status of the get fix criteria request
 *          - Valid values:
 *             - eQMI_LOC_SUCCESS (0) - Request was completed successfully
 *             - eQMI_LOC_GENERAL_FAILURE (1) - Request failed because of
 *               a general failure
 *             - eQMI_LOC_UNSUPPORTED (2) - Request failed because it is
 *               not supported
 *             - eQMI_LOC_INVALID_PARAMETER (3) - Request failed because
 *               it contained invalid parameters
 *             - eQMI_LOC_ENGINE_BUSY (4) - Request failed because the engine
 *               is busy
 *             - eQMI_LOC_PHONE_OFFLINE (5) - Request failed because the phone
 *               is offline
 *             - eQMI_LOC_TIMEOUT (6) - Request failed because it timed out
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t status;
    uint8_t   TlvPresent;
} loc_FixCriteriaStatusTlv;

/**
 * Contain fields in struct loc_HorAccuracyLvlTlv
 *
 *  \param   accuracy
 *          - Horizontal accuracy level
 *          - Valid values:
 *             - 0x00000001 – LOW: Client requires low horizontal accuracy
 *             - 0x00000002 – MED: Client requires medium horizontal accuracy
 *             - 0x00000003 – HIGH: Client requires high horizontal accuracy
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t accuracy;
    uint8_t   TlvPresent;
} loc_HorAccuracyLvlTlv;

/**
 * Contain fields in struct loc_IntermediateRptStateTlv
 *
 *  \param   intermediate
 *          - Intermediate Report state (ON, OFF). The client must explicitly set this field to
 *            OFF to stop receiving intermediate position reports. Intermediate position reports
 *            are generated at 1 Hz and are ON by default. If intermediate reports are turned ON,
 *            the client receives position reports even if the accuracy criteria is not met. The
 *            status in the position report is set to IN_PROGRESS for intermediate reports.
 *          - Valid values:
 *              0x00000001 – ON: Client is interested in receiving intermediate reports
 *              0x00000002 – OFF: Client is not interested in receiving intermediate reports
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t intermediate;
    uint8_t   TlvPresent;
} loc_IntermediateRptStateTlv;

/**
 * Contain fields in struct loc_MinIntervalTlv
 *
 *  \param   interval
 *          - Minimum Interval Between fixes
 *          - Units: Milliseconds
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 */
typedef struct
{
    uint32_t interval;
    uint8_t   TlvPresent;
} loc_MinIntervalTlv;

/**
 *  Contain fields in struct loc_AppProviderInfoTlv
 *
 *  \param  providerLen
 *          - number of sets of the following elements:
 *              - provider
 *
 *  \param  provider
 *          - Application provider.
 *
 *  \param  nameLen
 *          - number of sets of the following elements:
 *              - name
 *
 *  \param  name
 *          - Application name.
 *
 *  \param  verValid
 *          - Specifies whether the application version string contains a valid value:
 *            0x00 (FALSE) – Application version string is invalid
 *            0x01 (TRUE)  – Application version string is valid
 *
 *  \param  versionLen
 *          - Number of sets of the following elements:
 *            version
 *
 *  \param  version
 *          - Application version.
 *
 *  \param  TlvPresent
 *          - Tlv Present or not
 * \note    None
 *
 */
typedef struct
{
    uint8_t  providerLen;
    char     provider[LOC_UINT8_MAX_STRING_SZ];
    uint8_t  nameLen;
    char     name[LOC_UINT8_MAX_STRING_SZ];
    uint8_t  verValid;
    uint8_t  versionLen;
    char     version[LOC_UINT8_MAX_STRING_SZ];
    uint8_t   TlvPresent;
} loc_AppProviderInfoTlv;

/**
 *  Contain the parameters passed for SetLocGetFixCriteriaCallback by
 *  the device.
 *
 *  \param  fixCriteriaStatus
 *          - Status of the get fix criteria request
            - See \ref loc_FixCriteriaStatusTlv for more information.
 *
 *  \param   horAccuracyLvl
 *          - Horizontal accuracy level
            - See \ref loc_HorAccuracyLvlTlv for more information.
 *
 *  \param   intermediateRptState
 *          - Intermediate Report state (ON, OFF).
 *          - See \ref loc_IntermediateRptStateTlv for more information.
 *
 *  \param   minInterval
 *          - Minimum Interval Between fixes
 *          - See \ref loc_MinIntervalTlv for more information.
 *
 *  \param   appProviderInfo
 *          - ID of the Application that Sent the Position Request Application provider, name, and version.
            - See \ref loc_AppProviderInfoTlv for more information.
 * \note    None
 *
 */
typedef struct
{
    loc_FixCriteriaStatusTlv    loc_fixCriteriaStatus;
    loc_HorAccuracyLvlTlv       loc_horAccuracy;
    loc_IntermediateRptStateTlv loc_intermediateRptState;
    loc_MinIntervalTlv          loc_minInterval;
    loc_AppProviderInfoTlv      loc_appProviderInfo;
    swi_uint256_t               ParamPresenceMask;
} unpack_loc_FixCriteria_Ind_t;

/**
 * Unpack the Get Fix Criteria Indication.
 * @param[in] pResp qmi response
 * @param[in] respLen qmi response length
 * @param[out] pOutput unpacked response
 *
 * @return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 * @sa     See qmerrno.h for eQCWWAN_xxx error values
 */
int unpack_loc_FixCriteria_Ind(
    uint8_t     *pResp,
    uint16_t    respLen,
    unpack_loc_FixCriteria_Ind_t *pOutput
);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
