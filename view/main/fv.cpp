#include "ui.hpp"
#include "ops.hpp"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QDir>

fv::fv(QWidget *parent) : QTreeView(parent) {
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
}

void fv::at(const QString &path) {
    base = path;
}

void fv::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }

    QTreeView::dragEnterEvent(event);
}

void fv::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }

    QTreeView::dragMoveEvent(event);
}

void fv::dropEvent(QDropEvent *event) {
    if (!event->mimeData()->hasUrls()) {
        QTreeView::dropEvent(event);
        return;
    }

    QModelIndex ti = indexAt(event->position().toPoint());
    QString destination = base;

    if (ti.isValid()) {
        QFileInfo target(model()->data(ti, QFileSystemModel::FilePathRole).toString());
        destination = target.isDir()
            ? target.absoluteFilePath()
            : target.absolutePath();
    }

    if (!QFileInfo(destination).isDir()) {
        event->ignore();
        return;
    }

    bool copying = event->modifiers().testFlag(Qt::ControlModifier) || event->proposedAction() == Qt::CopyAction;
    int failed = 0;

    for (const QUrl &url : event->mimeData()->urls()) {
        if (!url.isLocalFile()) {
            ++failed;
            continue;
        }

        QString source = url.toLocalFile();
        QFileInfo info(source);

        if (!info.exists() || info.absolutePath() == destination) {
            ++failed;
            continue;
        }

        QString target = QDir(destination).filePath(info.fileName());

        if (target == source ||
            (info.isDir() && destination.startsWith(source + "/")) ||
            QFileInfo::exists(target)) {
            ++failed;
            continue;
        }

        const bool ok = copying
            ? op::cp(source, target)
            : op::mv(source, target);

        if (!ok)
            ++failed;
    }

    event->acceptProposedAction();
    emit dropped();

    if (failed)
        setToolTip(QString::number(failed) + " items could not be dropped");
}
