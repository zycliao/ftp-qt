#include "serverthread.h"


//Server thread-----------------------------------------------
ServerThread::ServerThread() {
    curServer = new Server;
}

ServerThread::~ServerThread() {
    delete curServer;
}

void ServerThread::run() {
    std::cout<<"subthread started"<<std::endl;
    curServer->setup();
    curServer->listenClient();
    stop();
}

void ServerThread::stop() {
    std::cout<<"subthread finished"<<std::endl;
    isInterruptionRequested();
    quit();
    wait();
    emit emitSubThreadStop(num);
}

void ServerThread::forceStop() {
    std::cout<<"subthread terminated"<<std::endl;
    terminate();
}
