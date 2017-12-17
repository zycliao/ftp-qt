#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <QString>
#include <common.h>
#include <algorithm>
#include <WinSock2.h>

const int PORT = 21;
const int BUFLEN = 100;
const int DATABUFLEN = 1000;
const char* const DELIMITER = "\r\n";

class Client
{
private:
    int executeFTPCmd(int stateCode, char* cmd, char* arg=nullptr);
    int getStateCode();
    int getPortNum();
    int getFileSize(char* fname);
    int listPwd();
    int intoPasv();

    SOCKADDR_IN serverAddr;
    QString ip_addr, username, password, INFO;
    char* buf = new char[BUFLEN];
    char* databuf = new char[DATABUFLEN];
    SOCKET controlSocket;
    SOCKET dataSocket;
    char* pwd;

public:
    Client();
    ~Client();
    int connectServer();
    int disconnect();
    int changeDir(char* tardir);
    int login(QString ip_addr, QString username, QString password);
    int downFile(char* remoteName, char* localDir);

    std::vector<char*> pwdFiles;
};

#endif // CLIENT_H
