#include "server.h"

using namespace std;

Server::Server()
{
    config = new ServerConfig;
}

Server::~Server()
{
    delete config;
    closesocket(listenSocket);
    closesocket(clientSocket);
    WSACleanup();
}

int Server::setup() {
    WSADATA dat;

    if(!config->configed) {
        cout << "Not configed" << endl;
        return -1;
    }
    pwd = config->wd;

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

    dataListenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(dataListenSocket==INVALID_SOCKET)
    {
        cout << "Creating Data Socket Failed: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    dataListenAddr.sin_family = AF_INET;
    dataListenAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    getLocalIp();

    return 0;
}

int Server::listenClient() {
    int ret;
    int pasvPort, pasvArg1, pasvArg2;
    string pasvIp;
    int remoteAddrLen = sizeof(remoteAddr);
    clientSocket = accept(listenSocket, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
    if(clientSocket == INVALID_SOCKET) return -1;
    cout << "Client Socket Received!" << endl;
    sendMessage("220 Welcome to FTP server. Author: Patrick Liao.");
    if(!waitMessage("USER")) return -1;
    string username, password;
    username = arg;
    cout << username << endl;
    if(username == "anonymous") {
        if(!config->allowAnony) {
            sendMessage("530 Anonymous is not allowed");
            return -1;
        }
        sendMessage("331 Please specify the password.");
        waitMessage("PASS");
        password = arg;
        sendMessage("230 Login successful.");
    }
    else {
        sendMessage("331 Please specify the password.");
        waitMessage("PASS");
        password = arg;
        if(username == config->username && password == config->password)
            sendMessage("230 Login successful.");
        else {
            sendMessage("530 Login incorrect.");
            return -1;
        }
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
            sendMessage("257 " + pwd + " is the current directory");
            continue;
        }
        if(cmd == "TYPE") {
            if (arg == "A")
                sendMessage("200 Switching to ASCII mode.");
            else
                sendMessage("200 Switching to xxx mode.");
            continue;
        }
        if(cmd == "PASV") {
            pasvPort = setPasv();
            pasvArg2 = pasvPort % 256;
            pasvArg1 = pasvPort / 256;
            pasvIp = localIp;
            int loc = pasvIp.find('.');
            while(loc>=0) {
                pasvIp=pasvIp.replace(loc, 1, ",");
                loc = pasvIp.find('.');
            }

            int dataAddrLen = sizeof(dataAddr);
            sendMessage("227 Entering Passive Mode (" + pasvIp +
                        ", " + to_string(pasvArg1) + "," + to_string(pasvArg2) + ").");
            dataSocket = accept(dataListenSocket, (SOCKADDR *)&dataAddr, &dataAddrLen);
            continue;
        }
    }
}

//private functions-----------------------------------------------
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

int Server::setPasv() {
    default_random_engine random(time(NULL));
    uniform_int_distribution<int> dis1(config->pasvDown, config->pasvUp);
    int pasvPort = dis1(random);
    dataListenAddr.sin_port = htons(pasvPort);
    while(bind(dataListenSocket, (LPSOCKADDR)&dataListenAddr, sizeof(dataListenAddr)) == SOCKET_ERROR) {
        cout << "Data Socket Bind Error!" << endl;
        pasvPort = dis1(random);
        dataListenAddr.sin_port = htons(pasvPort);
    }

    if(listen(dataListenSocket, 5) == SOCKET_ERROR) {
        cout << "Listen Error: " << GetLastError() << endl;
        system("pause");
        return -1;
    }

    return pasvPort;
}

bool Server::getLocalIp() {
    char hostname[256];
    char ip[256];
    int ret = gethostname(hostname, sizeof(hostname));
    if (ret == SOCKET_ERROR)
    {
        return false;
    }
    HOSTENT* host = gethostbyname(hostname);
    if (host == NULL)
    {
        return false;
    }
    strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
    localIp = ip;
    return true;
}
