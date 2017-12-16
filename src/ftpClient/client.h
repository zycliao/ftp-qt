#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <QString>
#include <common.h>
#include <vector>

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
    void listPwd();
public:
    Client();
    ~Client();
    QString ip_addr, username, password;
    char* buf = new char[BUFLEN];
    char* databuf = new char[DATABUFLEN];
    SOCKET controlSocket;
    SOCKET dataSocket;
    char* pwd;
    std::vector<char*> pwdFiles;

    int connectServer();
    int login(QString ip_addr, QString username, QString password);

};




#endif // CLIENT_H
