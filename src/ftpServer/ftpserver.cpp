#include "ftpserver.h"
#include "ui_ftpserver.h"

ftpServer::ftpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpServer)
{
    curServer = new Server();
    serverThread = new ServerThread();
    serverThread->bind(curServer);
    ui->setupUi(this);

    connect(serverThread, SIGNAL(finished()), serverThread, SLOT(stop()));
}

ftpServer::~ftpServer()
{
    delete ui;
    delete serverThread;
    delete curServer;
}

void ftpServer::on_startButton_clicked()
{
    serverThread->start();
}

