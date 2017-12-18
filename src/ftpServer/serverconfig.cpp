#include "serverconfig.h"

using namespace std;

ServerConfig::ServerConfig()
{
    configed = false;
}

string ServerConfig::setup(string username, string password,
            string pasvDown, string pasvUp,
            int maxClient, bool allowAnony, string wd) {
    if(username.size() == 0)
        return "Please input your username";
    if(username.size() >= 255)
        return "Username too long";
    if(password.size() == 0)
        return "Please input your password";
    if(password.size() >= 255)
        return "Password too long";
    if(wd.size() == 0)
        return "Please select a working directory";
    int pasvDownInt, pasvUpInt;
    pasvDownInt = std::stoi(pasvDown);
    pasvUpInt = std::stoi(pasvUp);
    this->username = username;
    this->password = password;
    this->pasvDown = pasvDownInt;
    this->pasvUp = pasvUpInt;
    this->maxClient = maxClient;
    this->allowAnony = allowAnony;
    this->wd = wd;
    configed = true;
    return "success";
}
