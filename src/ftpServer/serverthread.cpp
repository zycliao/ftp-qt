#include "serverthread.h"


//Server thread-----------------------------------------------
ServerThread::ServerThread() {
    curServer = new Server;
}

ServerThread::~ServerThread() {
    stop();  //TODO!!!!!!!!!!!1
    delete curServer;
}

void ServerThread::run() {
    std::cout<<"sub thread started"<<std::endl;
    curServer->setup();
    curServer->listenClient();
}

void ServerThread::stop() {
    std::cout<<"subthread finished"<<std::endl;
    isInterruptionRequested();
    quit();
    wait();
}

