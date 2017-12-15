#include "ftpserver.h"
#include "ui_ftpserver.h"

ftpServer::ftpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpServer)
{
    ui->setupUi(this);
}

ftpServer::~ftpServer()
{
    delete ui;
}
