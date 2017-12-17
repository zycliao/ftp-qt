#include "server.h"

using namespace std;

Server::Server()
{

}

Server::~Server()
{
    delete buf;
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
    while(true) {
        clientSocket = accept(listenSocket, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
        if(clientSocket == INVALID_SOCKET) continue;
        cout << "Client Socket Received!" << endl;
        while(true) {
            Sleep(50);
            ret = recv(clientSocket, buf, BUFLEN, 0);
            if(ret<=0) continue;
        }
    }
}
