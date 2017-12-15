#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    Common a;
    ui->setupUi(this);
}

ftpClient::~ftpClient()
{
    delete ui;
}
