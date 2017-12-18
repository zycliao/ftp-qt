#include "clientthread.h"

ClientThread::ClientThread() {
    for(int i=0; i<5; i++)
        arglist.push_back("");
    curClient = new Client();

}

ClientThread::~ClientThread() {
    delete curClient;
}


void ClientThread::run() {
    switch (task) {
    case TConnect:
        if(!curClient->connectServer())
            emit emitSuccess();
        flushList();
        break;
    case TDisconnect:

        break;
    case TCd:
        curClient->changeDir(arglist[0]);
        flushList();
        break;
    case TDown:
        curClient->downFile(arglist[0], arglist[1]);
        break;
    default:
        break;
    }

}

void ClientThread::stop() {
    std::cout<<"subthread finished"<<std::endl;
    isInterruptionRequested();
    quit();
    wait();
}

void ClientThread::flushList() {
    emit emitClearList();
    emit emitListItem(QString("."));
    emit emitListItem(QString(".."));
    int num = curClient->pwdFiles.size();
    QString item;
    for(int i=0; i<num; i++) {
        item = QString::fromStdString(curClient->pwdFiles[i]);
        emit emitListItem(item);
    }
}

