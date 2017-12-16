#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    curClient = new Client();
    clientThread = new ClientThread();
    ui->setupUi(this);
}

ftpClient::~ftpClient()
{
    delete ui;
    delete curClient;
}

void ftpClient::on_connectButton_clicked()
{

    QString ip_addr = ui->ipEdit->text();
    QString username = ui->userEdit->text();
    QString password = ui->passEdit->text();
    curClient->login(ip_addr, username, password);
    clientThread->bind(curClient);
    connect(clientThread, SIGNAL(emitListItem(QString)), this, SLOT(recvListItem(QString)));
    //connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));
    clientThread->start();
}

void ftpClient::recvListItem(QString item) {
    ui->fileList->addItem(item);
}

//sub thread-------------------------------------------
ClientThread::ClientThread() {
    stopped = false;
}

ClientThread::~ClientThread() {
}

void ClientThread::bind(Client *c) {
    client = c;
}

void ClientThread::run() {
    client->connectServer();
    flushList();
    stopped = false;
}

void ClientThread::flushList() {
    emit emitListItem(QString("."));
    emit emitListItem(QString(".."));
    int num = client->pwdFiles.size();
    QString item;
    for(int i=0; i<num; i++) {
        item = pch2qstr(client->pwdFiles[i]);
        emit emitListItem(item);
    }
}



