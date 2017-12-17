#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QFileDialog>
#include "common.h"
#include "client.h"
#include <iostream>

namespace Ui {
class ftpClient;
}

enum subThreadTask {TConnect, TDisconnect, TCd, TDown};

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
    void recvInfo(QString);
    void recvSuccess();
    void recvClearList();

    void on_fileList_itemDoubleClicked(QListWidgetItem *item);

    void on_downButton_clicked();

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
    subThreadTask task;
    std::vector<char*> arglist;

protected:
    void run();
private:
    Client* client;
    void flushList();
private slots:
    void stop();
signals:
    void emitListItem(QString);
    void emitInfo(QString);
    void emitSuccess();
    void emitClearList();
};


#endif // FTPCLIENT_H
