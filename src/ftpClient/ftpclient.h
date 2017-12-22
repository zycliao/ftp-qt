#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include "common.h"
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
    void recvListItem(QString, QString, QString);
    void recvInfo(QString);
    void recvSuccess();
    void recvClearList();

    void on_connectButton_clicked();
    void on_downButton_clicked();
    void on_upButton_clicked();
    void on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_renameButton_clicked();
    void on_pushButton_2_clicked();

    void on_newButton_clicked();

private:
    Ui::ftpClient *ui;
    ClientThread* clientThread;
    bool connected = false;
    QString allInfo;
};



#endif // FTPCLIENT_H
