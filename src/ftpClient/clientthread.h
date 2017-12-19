#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include "client.h"

enum subThreadTask{TConnect, TDisconnect, TCd, TDown, TUp};

class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread();
    ~ClientThread();
    void bind(Client *c);
    subThreadTask task;
    std::vector<std::string> arglist;
    Client* curClient;

protected:
    void run();
private:
    Client* client;
    void flushList();
private slots:
    void stop();
signals:
    void emitListItem(QString);
    void emitInfo(QString);
    void emitSuccess();
    void emitClearList();
};


#endif // CLIENTTHREAD_H
