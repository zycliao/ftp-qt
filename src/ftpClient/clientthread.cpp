#include "clientthread.h"

ClientThread::ClientThread() {
    for(int i=0; i<5; i++)
        arglist.push_back("");
}

ClientThread::~ClientThread() {
}

void ClientThread::bind(Client *c) {
    client = c;
}

void ClientThread::run() {
    switch (task) {
    case TConnect:
        if(!client->connectServer())
            emit emitSuccess();
        flushList();
        break;
    case TDisconnect:

        break;
    case TCd:
        client->changeDir(arglist[0]);
        flushList();
        break;
    case TDown:
        client->downFile(arglist[0], arglist[1]);
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
    int num = client->pwdFiles.size();
    QString item;
    for(int i=0; i<num; i++) {
        item = pch2qstr(client->pwdFiles[i]);
        emit emitListItem(item);
    }
}

