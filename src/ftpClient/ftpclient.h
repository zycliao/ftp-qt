#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include "common.h"

namespace Ui {
class ftpClient;
}

class ftpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpClient(QWidget *parent = 0);
    ~ftpClient();

private:
    Ui::ftpClient *ui;
};

#endif // FTPCLIENT_H
