#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "serverconfig.h"
#include <WinSock2.h>
#include <random>
#include <time.h>

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
    ServerConfig* config;

private:
    SOCKET listenSocket;
    SOCKET clientSocket;
    SOCKET dataListenSocket;
    SOCKET dataSocket;
    SOCKADDR_IN listenAddr;
    SOCKADDR_IN remoteAddr;
    SOCKADDR_IN dataListenAddr;
    SOCKADDR_IN dataAddr;
    std::string buf;
    std::string cmd;
    std::string arg;
    std::string pwd;
    std::string localIp;

    int getPortNum();
    int sendMessage(std::string s);
    int waitMessage(std::string s);
    int recvStr();
    int setPasv();
    bool getLocalIp();
};

#endif // SERVER_H
