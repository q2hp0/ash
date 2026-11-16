#pragma once

#include <QFileDevice>
#include <QString>

namespace ut {
QString sz(qint64 bytes);
QString pm(QFileDevice::Permissions flags);
bool name(const QString &value);
}