#include "server.h"

using namespace std;

void char2Wchar(const char *chr, wchar_t *wchar, int size)
{
    MultiByteToWideChar(CP_ACP, 0, chr,
        strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}


void wchar2Char(const wchar_t *wchar, char *chr, int length)
{
    WideCharToMultiByte(CP_ACP, 0, wchar, -1,
        chr, length, NULL, NULL);
}

Server::Server()
{
    config = new ServerConfig;
}

Server::~Server()
{
    delete config;
    closesocket(listenSocket);
    closesocket(clientSocket);
    closesocket(dataSocket);
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
            sendMessage("257 \"" + pwd + "\" is the current directory");
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
            closesocket(dataListenSocket);
            continue;
        }

        if(cmd == "LIST") {
            sendMessage("150 Here comes the directory listing.");
            if (arg == "-al") {
                string allInfo;
                string curFile;
                vector<string> sizeAndType;
                vector<string> allFiles = getPwdInfo();
                for(int i=0; i<allFiles.size(); i++) {
                    curFile = allFiles[i];
                    sizeAndType = getFileSize(curFile);
                    if(sizeAndType.size()==0)
                        continue;
                    allInfo += sizeAndType[1];
                    allInfo += "--------- 1 user group ";
                    allInfo += sizeAndType[0];
                    allInfo += " Dec 10 14:50 ";
                    allInfo += curFile;
                    allInfo += "\r\n";
                }
                send(dataSocket, allInfo.c_str(), allInfo.size(), 0);
                // Must close the data socket!!!
                closesocket(dataSocket);
                sendMessage("226 Directory send OK.");
                continue;
            }
            else {
                cout << cmd << " " << arg << endl;
                return -1;
            }
        }

        if(cmd == "CWD") {
            string toDir = arg;
            if(toDir.size()>=2 && toDir[1] == ':')
                    pwd = toDir;
                else {
                    if(pwd[pwd.size()-1] != '/')
                        pwd += "/";
                    pwd += toDir;
                }
            sendMessage("250 CWD successfully.");
            continue;
        }

        if(cmd == "CDUP") {
            int p = pwd.find_last_of("/");
            if(p==pwd.size()+1) {
                pwd = pwd.substr(0, p);
                p = pwd.find_last_of("/");
            }
            pwd = pwd.substr(0, p);
            sendMessage("250 CDUP successfully.");
            continue;
        }

        if(cmd == "QUIT") {
            sendMessage("221 BYE!");
            break;
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
    dataListenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(dataListenSocket==INVALID_SOCKET)
    {
        cout << "Creating Data Socket Failed: " << GetLastError() << endl;
        system("pause");
        return -1;
    }
    default_random_engine random(time(NULL));
    uniform_int_distribution<int> dis1(config->pasvDown, config->pasvUp);
    int pasvPort = dis1(random);
    dataListenAddr.sin_port = htons(pasvPort);
    while(bind(dataListenSocket, (LPSOCKADDR)&dataListenAddr, sizeof(dataListenAddr)) == SOCKET_ERROR) {
        cout << "Data Socket Bind Error!" << endl;
        pasvPort = dis1(random);
        dataListenAddr.sin_port = htons(pasvPort);
        Sleep(100);
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

vector<string> Server::getPwdInfo() {
    DIR* dir;
    dirent* ptr;
    vector<string> allFiles;
    dir = opendir(pwd.c_str());
    while((ptr = readdir(dir)) != NULL)
        allFiles.push_back(ptr->d_name);
    return allFiles;
}

vector<string> Server::getFileSize(string fname) {
    vector<string> sizeAndAttrib;
    const char* fullname;
    string strfullname = (pwd+"/"+fname);
    fullname = strfullname.c_str();
    wchar_t* wfullname = new wchar_t[strfullname.size()*2];
    //wfullname = (wchar_t *)malloc(sizeof(wchar_t)* strfullname.size()/2);
    char2Wchar(fullname, wfullname, sizeof(wchar_t)*strfullname.size());
    wfullname[strfullname.size()] = 0;
    DWORD fAttr = GetFileAttributes(wfullname);
    delete wfullname;
    if(fAttr == INVALID_FILE_ATTRIBUTES) {
        return sizeAndAttrib;
    }
    if(fAttr == FILE_ATTRIBUTE_DIRECTORY) {
        sizeAndAttrib.push_back("0");
        sizeAndAttrib.push_back("d");
    }
    else {
        FILE* file = fopen((pwd+'/'+fname).c_str(), "rb");
        int size;
        if(file) {
            size = filelength(fileno(file));
            fclose(file);
        }
        sizeAndAttrib.push_back(to_string(size));
        sizeAndAttrib.push_back("-");
    }
    return sizeAndAttrib;
}

string Server::unix2Win() {
    string winAddr = pwd;
    int p;
    p = winAddr.find('/');
    while(p>=0) {
        winAddr[p] = '\\';
        p = winAddr.find('/');
    }
    return winAddr;
}
