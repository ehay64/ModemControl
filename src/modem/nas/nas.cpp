
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

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
        std::cerr << "Unable to open " << device << std::endl;
    }

    status = ioctl(this->fd, QMI_IOCTL_GET_SERVICE_FILE, eNAS);
    if (status != 0)
    {
        std::cerr << "Unable to configure file descriptor" << std::endl;
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
        std::cerr << "Unable to pack GetSignalStrengths: " << res << std::endl;
        return 0;
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
        return 0;
    }

    length = read(this->fd, buffer, 2048);

    unpack_nas_GetSignalStrengths_t unpack = {0};

    res = unpack_nas_GetSignalStrengths(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to unpack GetSignalStrengths: " << res << std::endl;
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
        std::cerr << "Unable to pack GetHomeNetwork: " << res << std::endl;
        return "unknown";
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
        return "unknown";
    }

    length = read(this->fd, buffer, 2048);

    unpack_nas_GetHomeNetwork_t unpack = {0};

    res = unpack_nas_GetHomeNetwork(buffer, length, &unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to unpack GetHomeNetwork: " << res << std::endl;
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
        std::cerr << "Unable to pack GetServingNetwork: " << res << std::endl;
        return -1;
    }

    res = write(this->fd, buffer, length);
    if (res != length)
    {
        std::cerr << "Unable to write entire buffer" << std::endl;
        return -1;
    }

    length = read(this->fd, buffer, 2048);

    res = unpack_nas_GetServingNetwork(buffer, length, unpack);
    if (res != eQCWWAN_ERR_NONE)
    {
        std::cerr << "Unable to unpack GetServingNetwork: " << res << std::endl;
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
        std::cerr << "Unable to GetServingNetwork" << std::endl;
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
        std::cerr << "Unable to GetServingNetwork" << std::endl;
        return false;
    }

    if (unpack.PSDomain == 1)
    {
        return true;
    }

    return false;
}
