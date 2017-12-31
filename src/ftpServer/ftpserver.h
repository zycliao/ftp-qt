#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include "serverthread.h"
#include "listenthread.h"
#include "serverconfig.h"
#include <WinSock2.h>
#include <QThread>
#include <QDateTime>
#include <vector>

namespace Ui {
class ftpServer;
}

class ftpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpServer(QWidget *parent = 0);
    ~ftpServer();
    void stopAll();

private slots:
    void on_startButton_clicked();
    void on_dirButton_clicked();
    void on_maxClientSlide_sliderMoved(int position);
    void recvSocket(SOCKET, QString);
    void recvSubThreadStop(int num);

private:
    void flushList();

    Ui::ftpServer *ui;
    int maxClientSlideNum=1;
    std::string wd;
    bool connected;
    ListenThread* listenThread;
    std::vector<ServerThread*> subThread;
    ServerConfig* serverConfig;
};


#endif // FTPSERVER_H
