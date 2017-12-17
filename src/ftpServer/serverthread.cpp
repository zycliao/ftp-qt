#include "serverthread.h"


//Server thread-----------------------------------------------
ServerThread::ServerThread() {

}

ServerThread::~ServerThread() {

}

void ServerThread::bind(Server *s) {
    server = s;
}

void ServerThread::run() {
    std::cout<<"sub thread started"<<std::endl;
    server->setup();
    server->listenClient();
}

void ServerThread::stop() {
    std::cout<<"subthread finished"<<std::endl;
    isInterruptionRequested();
    quit();
    wait();
}

