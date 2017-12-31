#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "server.h"
#include <QThread>

class ServerThread : public QThread
{
    Q_OBJECT

public:
    explicit ServerThread();
    ~ServerThread();
    void forceStop();
    Server* curServer;
    QString ip;
    QString time;
    int num;

protected:
    void run();

private:


private slots:
    void stop();

signals:
    void emitSubThreadStop(int num);

};

#endif // SERVERTHREAD_H
