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
    void bind(Server *s);

protected:
    void run();

private:
    Server* server;

private slots:
    void stop();

//signals:

};

#endif // SERVERTHREAD_H
