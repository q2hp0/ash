#include "fs.hpp"
#include <QImageReader>
#include <QPixmap>
#include <QDir>

QIcon pv::icon(const QFileInfo &info) const {
    if (!info.isFile())
        return QFileIconProvider::icon(info);

    QString key = info.absoluteFilePath() +
        QString::number(info.lastModified().toMSecsSinceEpoch()) +
        QString::number(info.size());

    if (cache.contains(key))
        return cache.value(key);

    QImageReader reader(info.absoluteFilePath());

    if (!reader.canRead())
        return QFileIconProvider::icon(info);

    reader.setAutoTransform(true);

    QSize size = reader.size();

    if (size.isValid())
        size.scale(64, 64, Qt::KeepAspectRatio);
    else
        size = QSize(64, 64);

    reader.setScaledSize(size);

    QImage image = reader.read();

    if (image.isNull())
        return QFileIconProvider::icon(info);

    QIcon result(QPixmap::fromImage(image));

    if (cache.size() > 600)
        cache.clear();

    cache.insert(key, result);
    return result;
}

fs::fs(QObject *parent) {
    m = new QFileSystemModel(parent);
    m->setRootPath(QDir::homePath());
    m->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m->setReadOnly(false);
    m->setIconProvider(new pv);

    p = new QSortFilterProxyModel(parent);
    p->setSourceModel(m);
    p->setRecursiveFilteringEnabled(true);
    p->setFilterCaseSensitivity(Qt::CaseInsensitive);
    p->setFilterKeyColumn(0);
    p->setSortCaseSensitivity(Qt::CaseInsensitive);
    p->setDynamicSortFilter(true);
}

QFileSystemModel *fs::mdl() {
    return m;
}

QSortFilterProxyModel *fs::prx() {
    return p;
}

QString fs::root() {
    return QDir::homePath();
}
