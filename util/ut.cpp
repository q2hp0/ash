#include "ut.hpp"

#include <QStringList>

QString ut::sz(qint64 bytes) {
    double value = static_cast<double>(qMax<qint64>(0, bytes));
    const QStringList units = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;

    while (value >= 1024.0 && unit < units.size() - 1) {
        value /= 1024.0;
        ++unit;
    }

    return QString::number(value, 'f', unit == 0 ? 0 : 2) + " " + units.at(unit);
}

QString ut::pm(QFileDevice::Permissions flags) {
    QString result;
    result += flags.testFlag(QFileDevice::ReadOwner) ? "r" : "-";
    result += flags.testFlag(QFileDevice::WriteOwner) ? "w" : "-";
    result += flags.testFlag(QFileDevice::ExeOwner) ? "x" : "-";
    result += flags.testFlag(QFileDevice::ReadGroup) ? "r" : "-";
    result += flags.testFlag(QFileDevice::WriteGroup) ? "w" : "-";
    result += flags.testFlag(QFileDevice::ExeGroup) ? "x" : "-";
    result += flags.testFlag(QFileDevice::ReadOther) ? "r" : "-";
    result += flags.testFlag(QFileDevice::WriteOther) ? "w" : "-";
    result += flags.testFlag(QFileDevice::ExeOther) ? "x" : "-";
    return result;
}

bool ut::name(const QString &value) {
    return !value.isEmpty() &&
        value != "." &&
        value != ".." &&
        !value.contains('/') &&
        !value.contains(QChar(0));
}