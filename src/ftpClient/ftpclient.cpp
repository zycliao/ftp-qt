#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    clientThread = new ClientThread();

    connect(clientThread, SIGNAL(emitListItem(QString)), this, SLOT(recvListItem(QString)));
    connect(clientThread, SIGNAL(emitSuccess()), this, SLOT(recvSuccess()));
    connect(clientThread, SIGNAL(finished()), clientThread, SLOT(stop()));
    connect(clientThread, SIGNAL(emitClearList()), this, SLOT(recvClearList()));
    connect(clientThread->curClient->infoThread, SIGNAL(emitInfo(QString)), this, SLOT(recvInfo(QString)));
    //若执行此行，run结束后clientThread会调用析构
    //connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));

    ui->setupUi(this);
}

ftpClient::~ftpClient()
{
    delete ui;
    delete clientThread;
}

//slot function------------------------------------------------
void ftpClient::on_connectButton_clicked()
{
    if(!clientThread->isRunning()){
    if(!connected) {
        QString ip_addr = ui->ipEdit->text();
        QString username = ui->userEdit->text();
        QString password = ui->passEdit->text();
        clientThread->curClient->login(ip_addr, username, password);
        clientThread->task = TConnect;
        clientThread->start();
    }
    else {
        clientThread->task = TDisconnect;
        clientThread->start();
        connected = false;
        ui->connectButton->setText("Connect");
        ui->fileList->clear();
    }
    }
}

void ftpClient::on_fileList_itemDoubleClicked(QListWidgetItem *item)
{
    QString a = item->text();
    if(!clientThread->isRunning()) {
    clientThread->arglist[0] = a.toStdString();
    clientThread->task = TCd;
    clientThread->start();
    }
}

void ftpClient::on_downButton_clicked()
{
    QListWidgetItem* curItem = ui->fileList->currentItem();
    QString downName;
    if(curItem)
        downName = curItem->text();
    else
        return;
    QString saveDir = QFileDialog::getExistingDirectory(this, "Choose save path");
    if(!clientThread->isRunning()) {
    clientThread->task = TDown;
    clientThread->arglist[0] = downName.toStdString();
    clientThread->arglist[1] = saveDir.toStdString();
    clientThread->start();
    }
}

void ftpClient::recvListItem(QString item) {
    ui->fileList->addItem(item);
}

void ftpClient::recvInfo(QString info) {
    allInfo += info;
    if(allInfo.size()>=10000) {
        allInfo = allInfo.mid(allInfo.size()-10000);
    }
    ui->infoEdit->setText(allInfo);
    QTextCursor cursor = ui->infoEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->infoEdit->setTextCursor(cursor);
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

void ftpClient::on_upButton_clicked()
{
    std::string localFile;
    localFile = QFileDialog::getOpenFileName(this, "Choose the file to upload").toStdString();
    clientThread->task = TUp;
    clientThread->arglist[0] = localFile;
    clientThread->start();
}
