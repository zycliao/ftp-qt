#include "ftpserver.h"
#include "ui_ftpserver.h"

using namespace std;

ftpServer::ftpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpServer)
{
    serverThread = new ServerThread();
    ui->setupUi(this);

    connect(serverThread, SIGNAL(finished()), serverThread, SLOT(stop()));
}

ftpServer::~ftpServer()
{
    delete ui;
    delete serverThread;
}

void ftpServer::on_startButton_clicked()
{
    string info;
    info = serverThread->curServer->config->setup(ui->userEdit->text().toStdString(),
            ui->passEdit->text().toStdString(), ui->pasvDown->text().toStdString(),
            ui->pasvUp->text().toStdString(), maxClientSlideNum,
            ui->allowAnony->isChecked(), wd);
    if(info == "success")
        serverThread->start();
    else
        QMessageBox::warning(this, "Warning!", QString::fromStdString(info));
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
