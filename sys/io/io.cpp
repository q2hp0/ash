#include "io.hpp"

#include <QFile>

bool io::rd(const QString &path, QByteArray &data, QString &error) {
    data.clear();
    error.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }

    data = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        error = file.errorString();
        data.clear();
        return false;
    }

    return true;
}