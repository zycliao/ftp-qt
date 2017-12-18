#include "infothread.h"

InfoThread::InfoThread()
{

}

void InfoThread::sendInfo(std::string info) {
    emit emitInfo(QString::fromStdString(info));
}
