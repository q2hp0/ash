#pragma once

#include <QString>
#include <QStringList>

class nv {
public:
    bool put(const QString &path);
    QString bk();
    QString fw();
    void clr();

private:
    QStringList log;
    int at = -1;
};