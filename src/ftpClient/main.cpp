#include "ftpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ftpClient w;
    w.show();

    return a.exec();
}
