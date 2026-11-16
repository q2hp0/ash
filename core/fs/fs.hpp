#pragma once

#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QFileIconProvider>
#include <QHash>

class pv : public QFileIconProvider {
public:
    QIcon icon(const QFileInfo &info) const override;

private:
    mutable QHash<QString, QIcon> cache;
};

class fs {
public:
    explicit fs(QObject *parent = nullptr);

    QFileSystemModel *mdl();
    QSortFilterProxyModel *prx();
    QString root();

private:
    QFileSystemModel *m;
    QSortFilterProxyModel *p;
};
