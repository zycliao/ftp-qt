#include "server.h"

using namespace std;

Server::Server()
{

}

Server::~Server()
{
    closesocket(listenSocket);
    closesocket(clientSocket);
    WSACleanup();
}

int Server::setup() {
    WSADATA dat;

    if(WSAStartup(MAKEWORD(2,2),&dat)!=0)
    {
        cout << "Init Falied: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    listenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenSocket==INVALID_SOCKET)
    {
        cout << "Creating Listen Socket Failed: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(PORT);
    listenAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(listenSocket, (LPSOCKADDR)&listenAddr, sizeof(listenAddr)) == SOCKET_ERROR) {
        cout << "Bind Error: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    if(listen(listenSocket, 5) == SOCKET_ERROR) {
        cout << "Listen Error: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    return 0;
}

int Server::listenClient() {
    int ret;
    int remoteAddrLen = sizeof(remoteAddr);
    clientSocket = accept(listenSocket, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
    if(clientSocket == INVALID_SOCKET) return -1;
    cout << "Client Socket Received!" << endl;
    sendMessage("220 Welcome to FTP server. Author: Patrick Liao.");
    if(!waitMessage("USER")) return -1;
    string username, password;
    username = arg;
    cout << username << endl;
    if(username != "anonymous") {
        sendMessage("331 Please specify the password.");
        waitMessage("PASS");
        password = arg;
        sendMessage("230 Login successful.");
    }

    while(true) {
        ret = recvStr();
        if(ret<0) {
            Sleep(50);
            continue;
        }
        if(cmd == "SYST") {
            sendMessage("215 Windows 10.");
            continue;
        }
        if(cmd == "FEAT") {
            sendMessage("211-Features:\nPASV");
            sendMessage("211 End");
            continue;
        }
        if(cmd == "PWD") {
            sendMessage("257 \"xxx\" is the current directory");
            continue;
        }
        if(cmd == "TYPE A") {
            sendMessage("200 Switching to ASCII mode.");
            continue;
        }
        if(cmd == "PASV") {
            sendMessage("221 Bye");
            continue;
        }
    }
}

int Server::sendMessage(string s) {
    s.push_back('\r');
    s.push_back('\n');
    int ret = send(clientSocket,  s.data(), s.size(), 0);
    if(ret>0) return 0;
    else return -1;
}

int Server::waitMessage(string s) {
    int ret;
    while(true) {
        ret = recvStr();
        if(ret) {
            Sleep(50);
            continue;
        }
        else
            break;
    }
    if(s == cmd)
        return -1;
    else
        return 0;
}

int Server::getPortNum()
{/*
    int num1=0,num2=0;

    char* p=buf;
    int cnt=0;
    while( 1 )
    {
        if(cnt == 4 && (*p) != ',')
        {
            num1 = 10*num1+(*p)-'0';
        }
        if(cnt == 5)
        {
            num2 = 10*num2+(*p)-'0';
        }
        if((*p) == ',')
        {
            cnt++;
        }
        p++;
        if((*p) == ')')
        {
            break;
        }
    }
    std::cout<<"The data port number is "<<num1*256+num2<<std::endl;
    return num1*256+num2;*/
}

/*
string Server::getCmd() {
    string tempstr;
    int idxr = buf.find('\r');
    int idx = buf.find(' ');
    if(idx >= 0) tempstr = buf.substr(0, idx);
    else tempstr = buf;
    return tempstr;
}

string Server::getArg() {
    int idx = buf.find(' ') + 1;
    int idxr = buf.find('\r');
    if(idx >= 0) {
        string arg = buf.substr(idx, idxr-idx);
        return arg;
    }
    else {
        return nullptr;
    }
} */

int Server::recvStr() {
    char tempbuf[BUFLEN];
    int ret = recv(clientSocket, tempbuf, BUFLEN, 0);
    if(ret<0) return ret;
    buf.clear();
    buf = tempbuf;
    int idx = buf.find(' ');
    int idxr = buf.find('\r');
    if(idx>0 && idx<idxr) {
        cmd = buf.substr(0, idx);
        idx++;
        arg = buf.substr(idx, idxr-idx);
    }
    else {
        cmd = buf.substr(0, idxr);
        arg = "";
    }
    return 0;
}
