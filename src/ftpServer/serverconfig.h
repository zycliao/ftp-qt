#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "common.h"

class ServerConfig
{
public:
    ServerConfig();
    std::string setup(std::string username, std::string password,
                      std::string pasvDown, std::string pasvUp,
                      int maxClient, bool allowAnony, std::string wd);

    std::string username;
    std::string password;
    int pasvDown;
    int pasvUp;
    int maxClient;
    bool allowAnony;
    std::string wd;
    bool configed;
};

#endif // SERVERCONFIG_H
