
#include <unistd.h>

#include <iostream>

#include "modem/modem.hpp"

#define MODEM_QMI_DEVICE "/dev/qcqmi0"
#define MODEM_AT_DEVICE "/dev/ttyUSB2"

int main()
{
    std::cout << "Starting modem control..." << std::endl;

    std::cout << "Waiting for enumeration";

    while (true)
    {
        if (access(MODEM_QMI_DEVICE, F_OK) == 0)
        {
            std::cout << std::endl << "Modem enumerated" << std::endl;
            break;
        }

        sleep(1);
        std::cout << ".";
    }

    Modem modem(MODEM_QMI_DEVICE, MODEM_AT_DEVICE);

    std::cout << "Modem QMI device: " << modem.getQmiDevice() << std::endl;

    std::cout << "Modem AT device: " << modem.getAtDevice() << std::endl;

    std::cout << "Waiting for network registration";

    while (true)
    {
        if (modem.nasService().getRegistrationState())
        {
            std::cout << std::endl << "Modem registered to network" << std::endl;
            break;
        }

        sleep(1);
        std::cout << ".";
    }

    std::cout << "Waiting for packet switched attachment";

    while (true)
    {
        if (modem.nasService().getAttachmentState())
        {
            std::cout << std::endl << "Modem attached to packet switched network" << std::endl;
            break;
        }

        sleep(1);
        std::cout << ".";
    }

    std::cout << "Network: " << modem.nasService().getNetworkName() << std::endl;
    std::cout << "RSSI: " << modem.nasService().getSignalStrength() << std::endl;
    std::cout << "APN: " << modem.wdsService().getApn() << std::endl;

    std::cout << "Starting data session..." << std::endl;

    if (modem.wdsService().startAtDataSession())
    {
        std::cout << "Data session started, exiting" << std::endl;
    }
    else
    {
        std::cout << "Unable to start data session, exiting" << std::endl;
        return -1;
    }

    return 0;
}
