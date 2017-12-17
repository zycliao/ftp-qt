#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QFileDialog>
#include "common.h"
#include "client.h"
#include "clientthread.h"

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



#endif // FTPCLIENT_H
