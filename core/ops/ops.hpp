#pragma once

#include <QString>

class op {
public:
    static QString unq(const QString &path);
    static bool cp(const QString &from, const QString &to);
    static bool mv(const QString &from, const QString &to);
    static bool rm(const QString &path);
};