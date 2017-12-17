#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QMainWindow>
#include "server.h"
#include "serverthread.h"
#include <QThread>

namespace Ui {
class ftpServer;
}

class ftpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpServer(QWidget *parent = 0);
    ~ftpServer();
    Server* curServer;
    ServerThread* serverThread;

private slots:
    void on_startButton_clicked();

private:
    Ui::ftpServer *ui;
};


#endif // FTPSERVER_H
