#include "sys.hpp"

#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>

bool lx::open(const QString &path) {
    const QFileInfo info(path);
    return info.exists() &&
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
}