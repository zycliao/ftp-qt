#include "listenthread.h"

using namespace std;

const int PORT = 21;

ListenThread::ListenThread() {
    cur_client = 0;
}

ListenThread::~ListenThread() {
    closesocket(clientSocket);
    closesocket(listenSocket);
}

int ListenThread::setup() {
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

void ListenThread::run() {
    QString ip;
    string b1, b2, b3, b4, b0;
    setup();
    int remoteAddrLen = sizeof(remoteAddr);
    while(true) {
        if(cur_client<max_client) {
            clientSocket = accept(listenSocket, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
            if(clientSocket == INVALID_SOCKET) {
                cout << "Create client socket error!" << endl;
                continue;
            }
            cur_client++;
            b1 = to_string(remoteAddr.sin_addr.S_un.S_un_b.s_b1);
            b2 = to_string(remoteAddr.sin_addr.S_un.S_un_b.s_b2);
            b3 = to_string(remoteAddr.sin_addr.S_un.S_un_b.s_b3);
            b4 = to_string(remoteAddr.sin_addr.S_un.S_un_b.s_b4);
            b0 = b1 + "." + b2 + "." + b3 + "." + b4;
            ip = QString::fromStdString(b0);
            cout << "No." << cur_client << " client socket received!" << endl;
            emit emitSocket(clientSocket, ip);
        }
        Sleep(100);
    }
}

void ListenThread::stop() {
    terminate();
    int a = this->isRunning();
    int b = this->isFinished();
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
}
