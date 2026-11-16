#pragma once

#include <QString>

class QFileSystemModel;
class QModelIndex;
class QSortFilterProxyModel;

class it {
public:
    static QString path(
        const QModelIndex &index,
        const QSortFilterProxyModel *proxy,
        const QFileSystemModel *model
    );
};