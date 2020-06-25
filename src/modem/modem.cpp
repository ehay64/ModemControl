
#include "modem.hpp"

#include "wds/wds.hpp"
#include "nas/nas.hpp"

Modem::Modem(std::string qmiDevice, std::string atDevice)
{
    this->qmiDevice = qmiDevice;
    this->atDevice = atDevice;

    wds = new Wds(this->qmiDevice, this->atDevice);
    nas = new Nas(this->qmiDevice);
}

Modem::~Modem()
{
    delete wds;
    delete nas;
}

std::string Modem::getQmiDevice(void)
{
    return this->qmiDevice;
}

std::string Modem::getAtDevice(void)
{
    return this->atDevice;
}

Wds &Modem::wdsService(void)
{
    return *wds;
}

Nas &Modem::nasService(void)
{
    return *nas;
}
