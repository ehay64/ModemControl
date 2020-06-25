
#include <unistd.h>

#include <iostream>

#include "modem/modem.hpp"

#define MODEM_QMI_DEVICE "/dev/qcqmi0"
#define MODEM_AT_DEVICE "/dev/ttyUSB2"

int main()
{
    std::cout << "Hello world!" << std::endl;

    Modem modem(MODEM_QMI_DEVICE, MODEM_AT_DEVICE);

    std::cout << "Modem QMI device: " << modem.getQmiDevice() << std::endl;

    std::cout << "Modem AT device: " << modem.getAtDevice() << std::endl;

    std::cout << "Apn: " << modem.wdsService().getApn() << std::endl;

    std::cout << "Signal Strength: " << modem.nasService().getSignalStrength() << std::endl;

    std::cout << "Network: " << modem.nasService().getNetworkName() << std::endl;

    std::cout << "Registered: " << modem.nasService().getRegistrationState() << std::endl;

    std::cout << "Attached: " << modem.nasService().getAttachmentState() << std::endl;

    std::cout << "Auto Connect: " << modem.wdsService().setAutoConnect(false) << std::endl;

    std::cout << "Start Data Session: " << modem.wdsService().startAtDataSession() << std::endl;

    return 0;
}
