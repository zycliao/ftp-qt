#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QMainWindow>
#include "common.h"

namespace Ui {
class ftpServer;
}

class ftpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpServer(QWidget *parent = 0);
    ~ftpServer();

private:
    Ui::ftpServer *ui;
};

#endif // FTPSERVER_H
