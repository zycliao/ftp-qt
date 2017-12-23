#ifndef LISTENTHREAD_H
#define LISTENTHREAD_H

#include <QThread>
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <QString>
#include <string>

class ListenThread : public QThread
{
    Q_OBJECT
public:
    ListenThread();
    ~ListenThread();

    void listening();
    int setup();
    void stop();

    int max_client;

protected:
    void run();

private:
    int cur_client;
    bool stopped;
    SOCKET listenSocket;
    SOCKET clientSocket;
    SOCKADDR_IN listenAddr;
    SOCKADDR_IN remoteAddr;

private slots:

signals:
    void emitSocket(SOCKET, QString);
};

#endif // LISTENTHREAD_H
