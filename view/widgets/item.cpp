#include "item.hpp"

#include <QFileSystemModel>
#include <QSortFilterProxyModel>

QString it::path(
    const QModelIndex &index,
    const QSortFilterProxyModel *proxy,
    const QFileSystemModel *model
) {
    if (!index.isValid() || !proxy || !model)
        return {};

    const QModelIndex source = proxy->mapToSource(index);
    return source.isValid() ? model->filePath(source) : QString{};
}