#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    curClient = new Client();
    clientThread = new ClientThread();
    clientThread->bind(curClient);

    connect(clientThread, SIGNAL(emitListItem(QString)), this, SLOT(recvListItem(QString)));
    connect(clientThread, SIGNAL(emitSuccess()), this, SLOT(recvSuccess()));
    connect(clientThread, SIGNAL(finished()), clientThread, SLOT(stop()));
    connect(clientThread, SIGNAL(emitClearList()), this, SLOT(recvClearList()));
    //若执行此行，run结束后clientThread会调用析构
    //connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));

    ui->setupUi(this);
}

ftpClient::~ftpClient()
{
    delete ui;
    delete curClient;
    delete clientThread;
}

void ftpClient::on_connectButton_clicked()
{
    if(!connected) {
        QString ip_addr = ui->ipEdit->text();
        QString username = ui->userEdit->text();
        QString password = ui->passEdit->text();
        curClient->login(ip_addr, username, password);
        clientThread->start();
    }
    else {
        curClient->disconnect();
        connected = false;
        ui->connectButton->setText("Connect");
    }
}

//slot function------------------------------------------------
void ftpClient::recvListItem(QString item) {
    ui->fileList->addItem(item);
}

void ftpClient::recvInfo(QString info) {

}

void ftpClient::recvSuccess() {
    if(!connected) {
        connected = true;
        ui->connectButton->setText("Disconnect");
    }
    else {
        connected = false;
        ui->connectButton->setText("Connect");
    }
}

void ftpClient::recvClearList() {
    ui->fileList->clear();
}

//sub thread---------------------------------------------------
ClientThread::ClientThread() {
}

ClientThread::~ClientThread() {
}

void ClientThread::bind(Client *c) {
    client = c;
}

void ClientThread::run() {
    if(!client->connectServer())
        emit emitSuccess();
    flushList();
}

void ClientThread::stop() {
    isInterruptionRequested();
    quit();
    wait();
}

void ClientThread::flushList() {
    emit emitClearList();
    emit emitListItem(QString("."));
    emit emitListItem(QString(".."));
    int num = client->pwdFiles.size();
    QString item;
    for(int i=0; i<num; i++) {
        item = pch2qstr(client->pwdFiles[i]);
        emit emitListItem(item);
    }
}



