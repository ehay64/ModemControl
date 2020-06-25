
#ifndef MODEM_H
#define MODEM_H

#include <string>

#include "wds/wds.hpp"
#include "nas/nas.hpp"

class Modem
{
    private:
        std::string qmiDevice;
        std::string atDevice;

        Wds *wds;
        Nas *nas;

    public:
        Modem(std::string qmiDevice, std::string atDevice);

        ~Modem();

        std::string getQmiDevice(void);
        std::string getAtDevice(void);

        Wds &wdsService(void);
        Nas &nasService(void);
};

#endif
