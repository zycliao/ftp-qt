#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <QString>
#include <common.h>
#include <algorithm>
#include <WinSock2.h>
#include "infothread.h"

const int PORT = 21;
const int BUFLEN = 1000;
const int DATABUFLEN = 1000;
const char* const DELIMITER = "\r\n";

class Client
{
private:
    //int executeFTPCmd(int stateCode, char* cmd, char* arg=nullptr);
    int getStateCode();
    int getPortNum();
    int getFileSize(std::string fname);
    int listPwd();
    int intoPasv();
    int recvControl(int stateCode, std::string errorInfo="0");
    int executeCmd(std::string cmd);
    void removeSpace(std::string&);

    SOCKADDR_IN serverAddr;
    std::string ip_addr, username, password, INFO;
    char* buf = new char[BUFLEN];
    char* databuf = new char[DATABUFLEN];
    SOCKET controlSocket;
    SOCKET dataSocket;
    std::string recvInfo;
    std::string nextInfo; //JUNK

public:
    Client();
    ~Client();
    int connectServer();
    int disconnect();
    int changeDir(std::string tardir);
    int login(QString ip_addr, QString username, QString password);
    int downFile(std::string remoteName, std::string localDir);
    int upFile(std::string localName);

    InfoThread* infoThread;
    std::string pwd;
    std::vector<std::vector<std::string>> filelist;
};

#endif // CLIENT_H
