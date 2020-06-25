
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <qmerrno.h>
#include <common.h>
#include <wds.h>

#include "wds.hpp"

#define QMI_IOCTL_GET_SERVICE_FILE (0x8BE0 + 1)

Wds::Wds(std::string qmiDevice, std::string atDevice)
{
    int status = 0;

    this->qmiFd = open(qmiDevice.c_str(), O_RDWR);
    if (this->qmiFd == -1)
    {
        printf("Unable to open %s\n", qmiDevice.c_str());
    }

    status = ioctl(this->qmiFd, QMI_IOCTL_GET_SERVICE_FILE, eWDS);
    if (status != 0)
    {
        printf("Unable to configure file descriptor\n");
    }

    this->atFd = open(atDevice.c_str(), O_RDWR);
    if (this->atFd == -1)
    {
        printf("Unable to open %s\n", atDevice.c_str());
    }

    this->transcation_id = 1;
}

Wds::~Wds()
{
    close(this->qmiFd);
}

int Wds::getTransactionId(void)
{
    return this->transcation_id++;
}

std::string Wds::getApn(void)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    pack_wds_SLQSGetProfileSettings_t pack = {0};

    pack.ProfileId = 1;
    pack.ProfileType = 0;

    uint8_t buffer[2048];
    uint16_t length = 2048;

    res = pack_wds_SLQSGetProfileSettings(&request, buffer, &length, &pack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack SLQSGetProfileSettings: %i\n", res);
        return "none";
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return "none";
    }

    length = read(this->qmiFd, buffer, 2048);

    uint8_t apn_name[256] = {0};
    uint16_t apn_name_size = 256;
    uint16_t error;

    UnPackGetProfileSettingOut profile = {0};

    profile.curProfile.SlqsProfile3GPP.pAPNName = apn_name;
    profile.curProfile.SlqsProfile3GPP.pAPNnameSize = &apn_name_size;
    profile.pExtErrCode = &error;

    unpack_wds_SLQSGetProfileSettings_t unpack = {0};

    unpack.pProfileSettings = &profile;

    res = unpack_wds_SLQSGetProfileSettings(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack SLQSGetProfileSettings: %i\n", res);
        return "none";
    }

    return std::string((char *)apn_name);
}

int Wds::setApn(std::string apn)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    uint8_t apn_name[256] = {0};
    uint16_t apn_name_size = 0;

    strncpy((char *)apn_name, apn.c_str(), 256);
    apn_name_size = apn.length();

    wds_profileInfo profile = {0};

    profile.SlqsProfile3GPP.pAPNName = apn_name;
    profile.SlqsProfile3GPP.pAPNnameSize = &apn_name_size;

    uint8_t profile_id = 1;
    uint8_t profile_type = 0;

    pack_wds_SLQSCreateProfile_t pack = {0};

    pack.pProfileId = &profile_id;
    pack.pProfileType = &profile_type;
    pack.pCurProfile = &profile;

    uint8_t buffer[2048] = {0};
    uint16_t length = 2048;

    res = pack_wds_SLQSCreateProfile(&request, buffer, &length, &pack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack SLQSCreateProfile: %i\n", res);
        return -1;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return -1;
    }

    length = read(this->qmiFd, buffer, 2048);

    PackCreateProfileOut result = {0};
    uint8_t result_profile_id = 0;

    unpack_wds_SLQSCreateProfile_t unpack = {0};

    unpack.pCreateProfileOut = &result;
    unpack.pProfileID = &result_profile_id;

    res = unpack_wds_SLQSCreateProfile(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack SLQSGetProfileSettings: %i\n", res);
        return -1;
    }

    return 0;
}

bool Wds::startDataSession(void)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    pack_wds_SLQSStartDataSession_t pack = {0};

    uint8_t buffer[2048] = {0};
    uint16_t length = 2048;

    res = pack_wds_SLQSStartDataSession(&request, buffer, &length, &pack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack SLQSStartDataSession: %i\n", res);
        return false;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return -1;
    }

    length = read(this->qmiFd, buffer, 2048);

    uint32_t sessionId = 0;
    uint32_t failReason = 0;
    uint32_t verboseFailReasonType = 0;
    uint32_t verboseFailReason = 0;

    unpack_wds_SLQSStartDataSession_t unpack;

    unpack.psid = &sessionId;
    unpack.pFailureReason = &failReason;
    unpack.pVerboseFailReasonType = &verboseFailReasonType;
    unpack.pVerboseFailureReason = &verboseFailReason;

    res = unpack_wds_SLQSStartDataSession(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack SLQSStartDataSession: %i\n", res);
        return false;
    }

    printf("Session Id: %i\n", sessionId);

    return true;
}

bool Wds::startAtDataSession(void)
{
    int res = 0;

    char command[] = "AT!SCACT=1,1\n";

    res = write(this->atFd, command, sizeof(command));
    if (res != sizeof(command))
    {
        printf("Unable to send AT command: %s\n", command);
        return false;
    }

    return true;
}

bool Wds::stopAtDataSession(void)
{
    int res = 0;

    char command[] = "AT!SCACT=0,1\n";

    res = write(this->atFd, command, sizeof(command));
    if (res != sizeof(command))
    {
        printf("Unable to send AT command: %s\n", command);
        return false;
    }

    return true;
}

bool Wds::setAutoConnect(bool autoConnect)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    pack_wds_SetAutoconnect_t pack = {0};

    if (autoConnect)
    {
        pack.acsetting = 0x01;
    }
    else
    {
        pack.acsetting = 0x00;
    }

    pack.acroamsetting = 0x01;

    uint8_t buffer[2048] = {0};
    uint16_t length = 2048;

    res = pack_wds_SetAutoconnect(&request, buffer, &length, &pack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack SetAutoconnect: %i\n", res);
        return false;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return -1;
    }

    length = read(this->qmiFd, buffer, 2048);

    unpack_wds_SetAutoconnect_t unpack;

    res = unpack_wds_SetAutoconnect(buffer, length, &unpack);
    if (res == eQCWWAN_ERR_QMI_NO_EFFECT)
    {
        return true;
    }
    else if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack SetAutoconnect: %i\n", res);
        return false;
    }

    return true;
}
