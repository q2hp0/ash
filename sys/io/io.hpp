#pragma once

#include <QByteArray>
#include <QString>

namespace io {
bool rd(const QString &path, QByteArray &data, QString &error);
}