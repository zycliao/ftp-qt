#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "common.h"
#include "client.h"
#include <iostream>

namespace Ui {
class ftpClient;
}

class ClientThread;

class ftpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpClient(QWidget *parent = 0);
    ~ftpClient();

private slots:
    void on_connectButton_clicked();
    void recvListItem(QString);

private:
    Ui::ftpClient *ui;
    Client* curClient;
    ClientThread* clientThread;
    bool connected = false;
};

//sub thread-------------------------------------
class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread();
    ~ClientThread();
    void bind(Client *c);
protected:
    void run();
private:
    Client* client;
    void flushList();
    volatile bool stopped;
signals:
    void emitListItem(QString);
};


#endif // FTPCLIENT_H
