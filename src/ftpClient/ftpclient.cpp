#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    clientThread = new ClientThread();

    connect(clientThread, SIGNAL(emitListItem(QString, QString, QString)), this, SLOT(recvListItem(QString, QString, QString)));
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
    }
    }
}

void ftpClient::on_downButton_clicked()
{
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString downName;
    if(curItem)
        downName = curItem->text(2);
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

void ftpClient::recvListItem(QString type, QString size, QString name) {
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->fileTree);
    item->setText(0, type);
    item->setText(1, size);
    item->setText(2, name);
    ui->fileTree->addTopLevelItem(item);
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
    ui->fileTree->clear();
}

void ftpClient::on_upButton_clicked()
{
    std::string localFile;
    localFile = QFileDialog::getOpenFileName(this, "Choose the file to upload").toStdString();
    clientThread->task = TUp;
    clientThread->arglist[0] = localFile;
    clientThread->start();
}

void ftpClient::on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QString type = item->text(0);
    if(type!="d")
        return;
    QString file = item->text(2);
    if(!clientThread->isRunning()) {
        clientThread->arglist[0] = file.toStdString();
        clientThread->task = TCd;
        clientThread->start();
    }
}

void ftpClient::on_renameButton_clicked()
{
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString srcName, dstName;
    if(curItem)
        srcName = curItem->text(2);
    else
        return;
    if(srcName=="." || srcName=="..")
        return;
    dstName = QInputDialog::getText(this, "Please input a name", "New name of the file");
    if(dstName.isEmpty())
        return;
    if(dstName=="." || dstName=="..")
        return;
    clientThread->arglist[0] = srcName.toStdString();
    clientThread->arglist[1] = dstName.toStdString();
    clientThread->task = TRename;
    clientThread->start();
}


void ftpClient::on_pushButton_2_clicked()
{
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString fname;
    if(curItem)
        fname = curItem->text(2);
    else
        return;
    clientThread->arglist[0] = fname.toStdString();
    if(fname=="." || fname=="..")
        return;
    if(curItem->text(0)=="d")
        clientThread->task = TRmd;
    else
        clientThread->task = TDele;
    clientThread->start();
}

void ftpClient::on_newButton_clicked()
{
    QString name;
    name = QInputDialog::getText(this, "Please input a name.", "Name of new directory");
    if(name.isEmpty())
        return;
    if(name=="." || name=="..")
        return;
    clientThread->arglist[0] = name.toStdString();
    clientThread->task = TMkd;
    clientThread->start();
}
