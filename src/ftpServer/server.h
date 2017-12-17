#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <WinSock2.h>

const int PORT = 21;
const int BUFLEN = 255;

class Server
{
public:
    Server();
    ~Server();
    int setup();
    int listenClient();

private:
    SOCKET listenSocket;
    SOCKET clientSocket;
    SOCKADDR_IN listenAddr;
    SOCKADDR_IN remoteAddr;
    std::string buf;
    std::string cmd;
    std::string arg;

    int getPortNum();
    int sendMessage(std::string s);
    int waitMessage(std::string s);
    std::string getArg();
    std::string getCmd();
    int recvStr();
};

#endif // SERVER_H
