
#ifndef WDS_H
#define WDS_H

#include <string>

class Wds
{
    private:
        int qmiFd;
        int atFd;
        int transcationId;

        int getTransactionId(void);

    public:
        Wds(std::string qmiDevice, std::string atDevice);

        ~Wds();

        std::string getApn(void);

        int setApn(std::string apn);

        bool startDataSession(void);

        bool startAtDataSession(void);

        bool stopAtDataSession(void);

        bool setAutoConnect(bool autoConnect);
};

#endif
