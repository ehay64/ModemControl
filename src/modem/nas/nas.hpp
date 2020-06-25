
#ifndef NAS_H
#define NAS_H

#include <string>

#include <nas.h>

class Nas
{
    private:
        int fd;
        int transcation_id;

        int getTransactionId(void);

        int getServingNetwork(unpack_nas_GetServingNetwork_t *unpack);

    public:
        Nas(std::string device);

        ~Nas();

        int getSignalStrength(void);

        std::string getNetworkName(void);

        bool getRegistrationState(void);

        bool getAttachmentState(void);
};

#endif
