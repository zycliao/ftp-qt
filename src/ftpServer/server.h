#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "serverconfig.h"
#include <WinSock2.h>
#include <windows.h>
#include <random>
#include <fstream>
#include <time.h>
#include <dirent.h>

const int PORT = 21;
const int BUFLEN = 255;
const int DATABUFLEN = 1024;

class Server
{
public:
    Server();
    ~Server();
    int setup();
    int listenClient();
    void bindClientSocket(SOCKET c);
    ServerConfig* config;

private:
    SOCKET clientSocket;
    SOCKET dataListenSocket;
    SOCKET dataSocket;
    SOCKADDR_IN dataListenAddr;
    SOCKADDR_IN dataAddr;
    std::string buf;
    std::string cmd;
    std::string arg;
    std::string pwd;
    std::string rootdir;
    std::string localIp;

    int getPortNum();
    int sendMessage(std::string s);
    int waitMessage(std::string s);
    int recvStr();
    int setPasv();
    bool getLocalIp();
    std::vector<std::string> getPwdInfo();
    std::vector<std::string> getFileSize(std::string fname);
    std::string pathConcat(std::string, std::string);
    std::string abs2rel(std::string);
    std::string rel2abs(std::string);
};

#endif // SERVER_H
