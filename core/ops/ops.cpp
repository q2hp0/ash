#include "ops.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {
bool nest(const QString &from, const QString &to) {
    const QString src = QFileInfo(from).canonicalFilePath();
    const QString dst = QDir::cleanPath(QFileInfo(to).absoluteFilePath());
    return !src.isEmpty() && (dst == src || dst.startsWith(src + QDir::separator()));
}

bool dup(const QString &from, const QString &to) {
    const QFileInfo info(from);

    if (!info.exists() && !info.isSymLink())
        return false;

    if (info.isSymLink())
        return QFile::link(info.symLinkTarget(), to);

    if (info.isFile()) {
        if (!QFile::copy(from, to))
            return false;

        if (QFile::setPermissions(to, info.permissions()))
            return true;

        QFile::remove(to);
        return false;
    }

    if (!info.isDir() || nest(from, to) || !QDir().mkpath(to))
        return false;

    const QFileInfoList entries = QDir(from).entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System,
        QDir::Name
    );

    for (const QFileInfo &entry : entries) {
        const QString next = QDir(to).filePath(entry.fileName());
        if (!dup(entry.absoluteFilePath(), next)) {
            QDir(to).removeRecursively();
            return false;
        }
    }

    if (QFile::setPermissions(to, info.permissions()))
        return true;

    QDir(to).removeRecursively();
    return false;
}
}

QString op::unq(const QString &path) {
    const QFileInfo info(path);
    QString stem = info.isDir() || info.suffix().isEmpty()
        ? info.fileName()
        : info.completeBaseName();
    const QString suffix = info.isDir() ? QString{} : info.suffix();

    for (int i = 1; i < 10000; ++i) {
        QString name = stem + " (copy " + QString::number(i) + ")";
        if (!suffix.isEmpty())
            name += "." + suffix;

        const QString next = QDir(info.absolutePath()).filePath(name);
        if (!QFileInfo::exists(next))
            return next;
    }

    return {};
}

bool op::cp(const QString &from, const QString &to) {
    if (QFileInfo::exists(to) || from.isEmpty() || to.isEmpty() || nest(from, to))
        return false;

    return dup(from, to);
}

bool op::mv(const QString &from, const QString &to) {
    if (QFileInfo::exists(to) || from.isEmpty() || to.isEmpty() || nest(from, to))
        return false;

    if (QFile::rename(from, to))
        return true;

    if (!dup(from, to))
        return false;

    if (rm(from))
        return true;

    rm(to);
    return false;
}

bool op::rm(const QString &path) {
    const QFileInfo info(path);
    if (!info.exists() && !info.isSymLink())
        return false;
    if (info.isSymLink() || info.isFile())
        return QFile::remove(path);
    return info.isDir() && QDir(path).removeRecursively();
}