#ifndef INFOTHREAD_H
#define INFOTHREAD_H

#include <QThread>
#include <QString>
#include <string>

class InfoThread : public QThread
{
    Q_OBJECT
public:
    explicit InfoThread();
    void sendInfo(std::string);

signals:
    void emitInfo(QString);
};

#endif // INFOTHREAD_H
