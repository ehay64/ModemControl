
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <qmerrno.h>
#include <common.h>
#include <nas.h>

#include "nas.hpp"

#define QMI_IOCTL_GET_SERVICE_FILE (0x8BE0 + 1)

Nas::Nas(std::string device)
{
    int status = 0;

    this->fd = open(device.c_str(), O_RDWR);
    if (this->fd == -1)
    {
        printf("Unable to open %s\n", device.c_str());
    }

    status = ioctl(this->fd, QMI_IOCTL_GET_SERVICE_FILE, eNAS);
    if (status != 0)
    {
        printf("Unable to configure file descriptor\n");
    }

    this->transcationId = 1;
}

Nas::~Nas()
{
    close(this->fd);
}

int Nas::getTransactionId(void)
{
    return this->transcationId++;
}

int Nas::getSignalStrength(void)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    uint8_t buffer[2048];
    uint16_t length = 2048;

    res = pack_nas_GetSignalStrengths(&request, buffer, &length);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack GetSignalStrengths: %i\n", res);
        return 0;
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return 0;
    }

    length = read(this->fd, buffer, 2048);

    unpack_nas_GetSignalStrengths_t unpack = {0};

    res = unpack_nas_GetSignalStrengths(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack GetSignalStrengths: %i\n", res);
        return 0;
    }

    return unpack.rssi[0];
}

std::string Nas::getNetworkName(void)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    uint8_t buffer[2048];
    uint16_t length = 2048;

    res = pack_nas_GetHomeNetwork(&request, buffer, &length);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack GetHomeNetwork: %i\n", res);
        return "unknown";
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return "unknown";
    }

    length = read(this->fd, buffer, 2048);

    unpack_nas_GetHomeNetwork_t unpack = {0};

    res = unpack_nas_GetHomeNetwork(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack GetHomeNetwork: %i\n", res);
        return "unknown";
    }

    return std::string(unpack.name);
}

int Nas::getServingNetwork(unpack_nas_GetServingNetwork_t *unpack)
{
    int res = 0;

    pack_qmi_t request = {0};

    request.xid = this->getTransactionId();

    uint8_t buffer[2048];
    uint16_t length = 2048;

    res = pack_nas_GetServingNetwork(&request, buffer, &length);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to pack GetServingNetwork: %i\n", res);
        return -1;
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        printf("Unable to write entire buffer\n");
        return -1;
    }

    length = read(this->fd, buffer, 2048);

    res = unpack_nas_GetServingNetwork(buffer, length, unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        printf("Unable to unpack GetServingNetwork: %i\n", res);
        return -1;
    }

    return 0;
}

bool Nas::getRegistrationState(void)
{
    int res = 0;

    unpack_nas_GetServingNetwork_t unpack = {0};

    res = this->getServingNetwork(&unpack);
    if (res != 0)
    {
        printf("Unable to GetServingNetwork\n");
        return false;
    }

    if (unpack.RegistrationState == 1)
    {
        return true;
    }

    return false;
}

bool Nas::getAttachmentState(void)
{
    int res = 0;

    unpack_nas_GetServingNetwork_t unpack = {0};

    res = this->getServingNetwork(&unpack);
    if (res != 0)
    {
        printf("Unable to GetServingNetwork\n");
        return false;
    }

    if (unpack.PSDomain == 1)
    {
        return true;
    }

    return false;
}
