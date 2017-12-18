#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include "serverthread.h"
#include <QThread>

namespace Ui {
class ftpServer;
}

class ftpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpServer(QWidget *parent = 0);
    ~ftpServer();
    ServerThread* serverThread;

private slots:
    void on_startButton_clicked();

    void on_dirButton_clicked();

    void on_maxClientSlide_sliderMoved(int position);

private:
    Ui::ftpServer *ui;
    int maxClientSlideNum=1;
    std::string wd;
};


#endif // FTPSERVER_H
