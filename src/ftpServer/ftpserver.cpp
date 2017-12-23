#include "ftpserver.h"
#include "ui_ftpserver.h"

using namespace std;

ftpServer::ftpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpServer)
{
    listenThread = new ListenThread();
    serverConfig = new ServerConfig();
    ui->setupUi(this);
    connected = false;
    connect(listenThread, SIGNAL(emitSocket(SOCKET, QString)), this, SLOT(recvSocket(SOCKET, QString)), Qt::DirectConnection);
}

ftpServer::~ftpServer()
{
    delete ui;
    delete serverConfig;
    delete listenThread;
}

void ftpServer::on_startButton_clicked()
{
    string info;
    if(!connected) {
        info = serverConfig->setup(ui->userEdit->text().toStdString(),
                                   ui->passEdit->text().toStdString(), ui->pasvDown->text().toStdString(),
                                   ui->pasvUp->text().toStdString(), maxClientSlideNum,
                                   ui->allowAnony->isChecked(), wd);
        if(info!="success") {
            QMessageBox::warning(this, "Warning!", QString::fromStdString(info));
            return;
        }
        listenThread->max_client = maxClientSlideNum;
        listenThread->start();
        connected = true;
        ui->startButton->setText("End");
    }
    else {
        subThread.clear();
        //serverThread->stop();
        connected = false;
        ui->startButton->setText("Start");
    }
}


void ftpServer::on_dirButton_clicked()
{
    QString workDir;
    workDir = QFileDialog::getExistingDirectory(this, "Please select a working direcotry.");
    wd = workDir.toStdString();
}

void ftpServer::on_maxClientSlide_sliderMoved(int position)
{
    maxClientSlideNum = position;
    ui->maxNum->setText(QString::fromStdString(std::to_string(maxClientSlideNum)));
}

void ftpServer::recvSocket(SOCKET s, QString ip) {
    if(!connected)
        return;
    ServerThread* serverThread = new ServerThread;
    QDateTime dt;
    QTime time;
    QDate date;
    dt.setTime(time.currentTime());
    dt.setDate(date.currentDate());
    serverThread->time = dt.toString("yyyy.MM.dd hh:mm:ss");
    connect(serverThread, SIGNAL(finished()), serverThread, SLOT(stop()));
    serverThread->ip = ip;
    serverThread->curServer->bindClientSocket(s);
    serverThread->curServer->config = serverConfig;
    subThread.push_back(serverThread);
    serverThread->num = subThread.size() - 1;
    serverThread->start();
    flushList();
}

void ftpServer::flushList() {
    ui->infoTree->clear();
    QTreeWidgetItem* item;;
    for(int i=0; i<subThread.size(); i++) {
        item = new QTreeWidgetItem(ui->infoTree);
        item->setText(0, QString::fromStdString(to_string(i+1)));
        item->setText(1, subThread[i]->ip);
        item->setText(2, subThread[i]->time);
        ui->infoTree->addTopLevelItem(item);
    }
}
