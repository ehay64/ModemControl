
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

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
        std::cerr << "Unable to open " << qmiDevice << std::endl;
    }

    status = ioctl(this->qmiFd, QMI_IOCTL_GET_SERVICE_FILE, eWDS);
    if (status != 0)
    {
        std::cerr << "Unable to configure file descriptor" << std::endl;
    }

    this->atFd = open(atDevice.c_str(), O_RDWR);
    if (this->atFd == -1)
    {
        std::cerr << "Unable to open " << atDevice << std::endl;
    }

    this->transcationId = 1;
}

Wds::~Wds()
{
    close(this->qmiFd);
}

int Wds::getTransactionId(void)
{
    return this->transcationId++;
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
        std::cerr << "Unable to pack SLQSGetProfileSettings: " << res << std::endl;
        return "unknown";
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
        return "unknown";
    }

    length = read(this->qmiFd, buffer, 2048);

    uint8_t apnName[256] = {0};
    uint16_t apnNameSize = 256;
    uint16_t error;

    UnPackGetProfileSettingOut profile = {0};

    profile.curProfile.SlqsProfile3GPP.pAPNName = apnName;
    profile.curProfile.SlqsProfile3GPP.pAPNnameSize = &apnNameSize;
    profile.pExtErrCode = &error;

    unpack_wds_SLQSGetProfileSettings_t unpack = {0};

    unpack.pProfileSettings = &profile;

    res = unpack_wds_SLQSGetProfileSettings(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to unpack SLQSGetProfileSettings: " << res << std::endl;
        return "unknown";
    }

    return std::string((char *)apnName);
}

bool Wds::setApn(std::string apn)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    uint8_t apnName[256] = {0};
    uint16_t apnNameSize = 0;

    strncpy((char *)apnName, apn.c_str(), 256);
    apnNameSize = apn.length();

    wds_profileInfo profile = {0};

    profile.SlqsProfile3GPP.pAPNName = apnName;
    profile.SlqsProfile3GPP.pAPNnameSize = &apnNameSize;

    uint8_t profileId = 1;
    uint8_t profileType = 0;

    pack_wds_SLQSCreateProfile_t pack = {0};

    pack.pProfileId = &profileId;
    pack.pProfileType = &profileType;
    pack.pCurProfile = &profile;

    uint8_t buffer[2048] = {0};
    uint16_t length = 2048;

    res = pack_wds_SLQSCreateProfile(&request, buffer, &length, &pack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to pack SLQSCreateProfile: " << res << std::endl;
        return false;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
        return false;
    }

    length = read(this->qmiFd, buffer, 2048);

    PackCreateProfileOut result = {0};
    uint8_t resultProfileId = 0;

    unpack_wds_SLQSCreateProfile_t unpack = {0};

    unpack.pCreateProfileOut = &result;
    unpack.pProfileID = &resultProfileId;

    res = unpack_wds_SLQSCreateProfile(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to unpack SLQSGetProfileSettings: " << res << std::endl;
        return false;
    }

    return true;
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
        std::cerr << "Unable to pack SLQSStartDataSession: " << res << std::endl;
        return false;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
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
        std::cerr << "Unable to unpack SLQSStartDataSession: " << res << std::endl;
        return false;
    }

    std::cerr << "Session Id: " << sessionId << std::endl;

    return true;
}

bool Wds::startAtDataSession(void)
{
    int res = 0;

    char command[] = "AT!SCACT=1\n";

    res = write(this->atFd, command, sizeof(command));
    if (res != sizeof(command))
    {
        std::cerr << "Unable to send AT command: " << command << std::endl;
        return false;
    }

    return true;
}

bool Wds::stopAtDataSession(void)
{
    int res = 0;

    char command[] = "AT!SCACT=0\n";

    res = write(this->atFd, command, sizeof(command));
    if (res != sizeof(command))
    {
        std::cerr << "Unable to send AT command: " << command << std::endl;
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
        std::cerr << "Unable to pack SetAutoconnect: " << res << std::endl;
        return false;
    }

    res = write(this->qmiFd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
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
        std::cerr << "Unable to unpack SetAutoconnect: " << res << std::endl;
        return false;
    }

    return true;
}
