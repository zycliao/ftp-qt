#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QThread>
#include <string>

char* qstr2pch(QString src);

inline char* qstr2pch(QString src) {
    std::string strsrc = src.toStdString();
    char* pstr=new char[strsrc.length()+1];
    strcpy(pstr,strsrc.c_str());
    return pstr;
}

inline QString pch2qstr(char* src) {
    return QString(QLatin1String(src));
}


#endif // COMMON_H
