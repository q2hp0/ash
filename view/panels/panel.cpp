#include "panel.hpp"
#include "ut.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QImageReader>
#include <QLabel>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPixmap>
#include <QVBoxLayout>

panel::panel(QWidget *parent) : QWidget(parent) {
    setMinimumWidth(210);
    setMaximumWidth(340);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);

    auto *caption = new QLabel("preview", this);
    caption->setAlignment(Qt::AlignCenter);
    thumb = new QLabel(this);
    thumb->setMinimumHeight(210);
    thumb->setAlignment(Qt::AlignCenter);
    thumb->setWordWrap(true);
    meta = new QLabel("select a file", this);
    meta->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    meta->setWordWrap(true);
    meta->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addWidget(caption);
    layout->addWidget(thumb, 1);
    layout->addWidget(meta);
}

void panel::put(const QStringList &paths) {
    thumb->clear();

    if (paths.size() != 1) {
        thumb->setText(paths.isEmpty() ? "no preview" :
            QString::number(paths.size()) + " items selected");
        meta->setText(paths.isEmpty() ? "select a file" :
            QString::number(paths.size()) + " selected items");
        return;
    }

    const QFileInfo info(paths.first());
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForFile(info, QMimeDatabase::MatchContent);
    QString details =
        "name: " + info.fileName() +
        "\ntype: " + (mime.isValid() ? mime.comment() : "unknown") +
        "\nsize: " + ut::sz(info.size()) +
        "\nmodified: " + info.lastModified().toString("yyyy-MM-dd hh:mm") +
        "\npermissions: " + ut::pm(info.permissions()) +
        "\nlocation: " + info.absolutePath();

    if (info.isSymLink())
        details += "\nsymlink: " + info.symLinkTarget();

    meta->setText(details);

    if (!info.isFile()) {
        thumb->setText("folder");
        return;
    }

    QImageReader reader(info.absoluteFilePath());
    reader.setAutoTransform(true);
    if (!reader.canRead()) {
        thumb->setText("no image preview");
        return;
    }

    QSize size = reader.size();
    if (size.isValid())
        size.scale(420, 300, Qt::KeepAspectRatio);
    if (size.isValid())
        reader.setScaledSize(size);

    const QImage image = reader.read();
    if (image.isNull()) {
        thumb->setText("preview unavailable");
        return;
    }

    thumb->setPixmap(QPixmap::fromImage(image).scaled(
        420, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation
    ));
}