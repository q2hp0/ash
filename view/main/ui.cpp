#include "ui.hpp"
#include "fs.hpp"
#include "ops.hpp"
#include "sys.hpp"
#include "ut.hpp"
#include "bar.hpp"
#include "panel.hpp"
#include "item.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QResizeEvent>
#include <QStatusBar>
#include <QAction>
#include <QKeySequence>
#include <QPixmap>
#include <QItemSelectionModel>
#include <QMenu>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QListWidget>
#include <QStandardPaths>
#include <QMimeDatabase>
#include <QMimeType>

ui::ui(QWidget *parent) : QMainWindow(parent) {
    f = new fs(this);

    setWindowTitle("ash");
    resize(1200, 760);
    setMinimumSize(760, 500);

    auto *root = new QWidget(this);
    auto *base = new QVBoxLayout(root);

    base->setContentsMargins(0, 0, 0, 0);
    base->setSpacing(0);

    head = new QLabel(root);
    head->setFixedHeight(135);
    head->setAlignment(Qt::AlignCenter);
    base->addWidget(head);

    tools = new bar(root);
    path = tools->loc;
    find = tools->find;
    base->addWidget(tools);

    auto *prev = tools->prev;
    auto *next = tools->next;
    auto *upbtn = tools->up;
    auto *refresh = tools->ref;
    auto *home = tools->home;
    auto *folder = tools->folder;
    auto *file = tools->file;
    auto *cpbtn = tools->copy;
    auto *cutbtn = tools->cut;
    auto *pstbtn = tools->paste;
    auto *trash = tools->trash;
    auto *props = tools->props;
    auto *hide = tools->hide;

    auto *split = new QSplitter(Qt::Horizontal, root);

    places = new QListWidget(split);
    places->setFixedWidth(155);
    places->setSpacing(3);

    view = new fv(split);
    view->setModel(f->prx());
    view->setDragEnabled(true);
    view->setAcceptDrops(true);
    view->setDropIndicatorShown(true);
    view->setDragDropMode(QAbstractItemView::DragDrop);
    view->setDefaultDropAction(Qt::MoveAction);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(
        QAbstractItemView::EditKeyPressed |
        QAbstractItemView::SelectedClicked
    );
    view->setSortingEnabled(true);
    view->setAlternatingRowColors(false);
    view->setAnimated(false);
    view->setIndentation(18);
    view->setIconSize(QSize(25, 25));
    view->setRootIsDecorated(false);
    view->setDragEnabled(true);
    view->setAcceptDrops(true);
    view->setDropIndicatorShown(true);
    view->setDragDropMode(QAbstractItemView::DragDrop);
    view->setDefaultDropAction(Qt::MoveAction);

    view->header()->setStretchLastSection(false);
    view->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    view->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    side = new panel(split);

    split->addWidget(places);
    split->addWidget(view);
    split->addWidget(side);

    split->setStretchFactor(0, 0);
    split->setStretchFactor(1, 1);
    split->setStretchFactor(2, 0);
    split->setSizes({155, 760, 265});

    base->addWidget(split, 1);
    setCentralWidget(root);

    auto place = [this](const QString &name, const QString &where) {
        if (!where.isEmpty() && QFileInfo(where).isDir()) {
            auto *item = new QListWidgetItem(name, places);
            item->setData(Qt::UserRole, where);
        }
    };

    place("home", QDir::homePath());
    place("desktop", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    place("documents", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    place("downloads", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    place("pictures", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    place("music", QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    place("videos", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    place("filesystem", "/");

    connect(prev, &QPushButton::clicked, this, [this] { bk(); });
    connect(next, &QPushButton::clicked, this, [this] { fw(); });
    connect(upbtn, &QPushButton::clicked, this, [this] { up(); });
    connect(refresh, &QPushButton::clicked, this, [this] { ref(); });
    connect(home, &QPushButton::clicked, this, [this] { go(QDir::homePath()); });

    connect(folder, &QPushButton::clicked, this, [this] { mkd(); });
    connect(file, &QPushButton::clicked, this, [this] { mkf(); });
    connect(cpbtn, &QPushButton::clicked, this, [this] { cpy(); });
    connect(cutbtn, &QPushButton::clicked, this, [this] {
        cpy();
        cut = !clip.isEmpty();
    });
    connect(pstbtn, &QPushButton::clicked, this, [this] { pst(); });
    connect(trash, &QPushButton::clicked, this, [this] { del(); });
    connect(props, &QPushButton::clicked, this, [this] { prp(); });
    connect(hide, &QPushButton::clicked, this, [this] { hid(); });

    connect(path, &QLineEdit::returnPressed, this, [this] {
        go(path->text());
    });

    connect(find, &QLineEdit::textChanged, this, &ui::srch);

    connect(places, &QListWidget::itemClicked, this,
        [this](QListWidgetItem *item) {
            go(item->data(Qt::UserRole).toString());
        });

    connect(view, &QTreeView::doubleClicked, this, &ui::opn);
    connect(view, &fv::dropped, this, [this] { ref(); });

    connect(view->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this, [this] {
            pvw();
            sts();
        });

    view->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(view, &QTreeView::customContextMenuRequested,
        this, [this](const QPoint &point) {
            QMenu menu(this);
            menu.addAction("open", this, [this] {
                opn(view->currentIndex());
            });
            menu.addAction("copy", this, [this] { cpy(); });
            menu.addAction("cut", this, [this] {
                cpy();
                cut = !clip.isEmpty();
            });
            menu.addAction("paste", this, [this] { pst(); });
            menu.addAction("rename", this, [this] { ren(); });
            menu.addAction("properties", this, [this] { prp(); });
            menu.addAction("move to trash", this, [this] { del(); });
            menu.exec(view->viewport()->mapToGlobal(point));
        });

    key();
    go(f->root());
}

void ui::go(const QString &where, bool save) {
    QFileInfo info(where);

    if (!info.exists() || !info.isDir()) {
        statusBar()->showMessage("directory not found", 2500);
        return;
    }

    QString target = info.absoluteFilePath();

    QModelIndex source = f->mdl()->setRootPath(target);
    QModelIndex proxy = f->prx()->mapFromSource(source);

    if (!proxy.isValid()) {
        statusBar()->showMessage("cannot open directory", 2500);
        return;
    }

    view->setRootIndex(proxy);
    view->at(target);
    path->setText(target);
    setWindowTitle("ash - " + info.fileName());
    statusBar()->showMessage(target);

    if (save)
        hist.put(target);

    pvw();
}

void ui::bk() {
    const QString target = hist.bk();
    if (!target.isEmpty())
        go(target, false);
}

void ui::fw() {
    const QString target = hist.fw();
    if (!target.isEmpty())
        go(target, false);
}

void ui::up() {
    QDir dir(path->text());

    if (dir.cdUp())
        go(dir.absolutePath());
}

void ui::ref() {
    go(path->text(), false);
    statusBar()->showMessage("updated", 1200);
}

void ui::mkd() {
    QString name = QInputDialog::getText(this, "ash", "folder name");

    if (name.isEmpty())
        return;

    if (!ut::name(name)) {
        QMessageBox::warning(this, "ash", "invalid folder name");
        return;
    }

    if (!QDir(path->text()).mkdir(name)) {
        QMessageBox::warning(this, "ash", "cannot create folder");
        return;
    }

    ref();
}

void ui::mkf() {
    QString name = QInputDialog::getText(this, "ash", "file name");

    if (name.isEmpty())
        return;

    if (!ut::name(name)) {
        QMessageBox::warning(this, "ash", "invalid file name");
        return;
    }

    QFile file(QDir(path->text()).filePath(name));

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "ash", "cannot create file");
        return;
    }

    file.close();
    ref();
}

QStringList ui::sel() {
    QStringList result;

    for (const QModelIndex &index : view->selectionModel()->selectedRows(0)) {
        const QString item = it::path(index, f->prx(), f->mdl());

        if (!item.isEmpty())
            result.append(item);
    }

    return result;
}

void ui::pvw() {
    side->put(sel());
}

void ui::del() {
    QStringList items = sel();

    if (items.isEmpty())
        return;

    auto answer = QMessageBox::question(
        this, "ash",
        "move " + QString::number(items.size()) + " item(s) to trash?"
    );

    if (answer != QMessageBox::Yes)
        return;

    int failed = 0;

    for (const QString &item : items) {
        if (!QFile::moveToTrash(item))
            ++failed;
    }

    ref();

    if (failed)
        QMessageBox::warning(
            this, "ash",
            QString::number(failed) + " item(s) could not be moved to trash"
        );
}

void ui::opn(const QModelIndex &index) {
    if (!index.isValid())
        return;

    const QString target = it::path(index, f->prx(), f->mdl());
    QFileInfo info(target);

    if (info.isDir()) {
        go(target);
        return;
    }

    if (!lx::open(target))
        statusBar()->showMessage("cannot open file", 2500);
}

void ui::ren() {
    QModelIndex index = view->currentIndex();

    if (index.isValid())
        view->edit(index.siblingAtColumn(0));
}

void ui::cpy() {
    clip = sel();
    cut = false;

    if (!clip.isEmpty())
        statusBar()->showMessage(
            QString::number(clip.size()) + " items selected for copy", 2000
        );
}

void ui::pst() {
    if (clip.isEmpty())
        return;

    QStringList fails;

    for (qsizetype i = 0; i < clip.size(); ++i) {
        const QString source = clip.at(i);
        QFileInfo info(source);
        QString target = QDir(path->text()).filePath(info.fileName());

        if (QFileInfo::exists(target)) {
            QMessageBox box(this);
            box.setWindowTitle("ash");
            box.setText("destination already exists");

            auto *keep = box.addButton("keep both", QMessageBox::AcceptRole);
            auto *skip = box.addButton("skip", QMessageBox::DestructiveRole);
            auto *cancel = box.addButton("cancel", QMessageBox::RejectRole);

            box.exec();

            if (box.clickedButton() == cancel) {
                for (; i < clip.size(); ++i)
                    fails.append(clip.at(i));
                break;
            }

            if (box.clickedButton() == skip) {
                if (cut)
                    fails.append(source);
                continue;
            }

            if (box.clickedButton() == keep)
                target = op::unq(target);

            if (target.isEmpty()) {
                fails.append(source);
                continue;
            }
        }

        bool ok = false;

        if (cut) {
            ok = op::mv(source, target);
        } else {
            ok = op::cp(source, target);
        }

        if (!ok)
            fails.append(source);
    }

    clip = cut ? fails : QStringList{};
    ref();

    if (!fails.isEmpty())
        QMessageBox::warning(
            this, "ash",
            QString::number(fails.size()) + " item(s) could not be processed"
        );
}

void ui::prp() {
    QStringList items = sel();

    if (items.size() != 1)
        return;

    QFileInfo info(items.first());
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(info, QMimeDatabase::MatchContent);

    QString text =
        "name: " + info.fileName() +
        "\npath: " + info.absoluteFilePath() +
        "\ntype: " + (mime.isValid() ? mime.comment() : "unknown") +
        "\nsize: " + ut::sz(info.size()) +
        "\ncreated: " + info.birthTime().toString() +
        "\nmodified: " + info.lastModified().toString() +
        "\nowner: " + info.owner() +
        "\ngroup: " + info.group() +
        "\nreadable: " + QString(info.isReadable() ? "yes" : "no") +
        "\nwritable: " + QString(info.isWritable() ? "yes" : "no") +
        "\nexecutable: " + QString(info.isExecutable() ? "yes" : "no");

    QMessageBox::information(this, "properties", text);
}

void ui::hid() {
    hidden = !hidden;

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot;

    if (hidden)
        filters |= QDir::Hidden | QDir::System;

    f->mdl()->setFilter(filters);

    QModelIndex source = f->mdl()->index(path->text());
    view->setRootIndex(f->prx()->mapFromSource(source));
}

void ui::srch(const QString &value) {
    f->prx()->setFilterFixedString(value);
}

void ui::fit() {
    QPixmap pic(":/assets/fat.png");

    if (pic.isNull()) {
        head->clear();
        head->setStyleSheet("background: #858585;");
        return;
    }

    head->setStyleSheet({});
    head->setPixmap(pic.scaled(
        head->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    ));
}

void ui::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    fit();
}

void ui::sts() {
    QStringList items = sel();

    if (items.isEmpty()) {
        statusBar()->showMessage(path->text());
        return;
    }

    qint64 total = 0;

    for (const QString &item : items) {
        QFileInfo info(item);

        if (info.isFile())
            total += info.size();
    }

    statusBar()->showMessage(
        QString::number(items.size()) + " selected  |  " + ut::sz(total)
    );
}

void ui::key() {
    auto bind = [this](const QKeySequence &key, auto fn) {
        auto *action = new QAction(this);
        action->setShortcut(key);
        action->setShortcutContext(Qt::WindowShortcut);
        addAction(action);
        connect(action, &QAction::triggered, this, fn);
    };

    bind(QKeySequence("Alt+Left"), [this] { bk(); });
    bind(QKeySequence("Alt+Right"), [this] { fw(); });
    bind(QKeySequence("Alt+Up"), [this] { up(); });
    bind(QKeySequence("Ctrl+R"), [this] { ref(); });
    bind(QKeySequence("Ctrl+Shift+N"), [this] { mkd(); });
    bind(QKeySequence("Ctrl+Shift+S"), [this] { mkf(); });
    bind(QKeySequence("Ctrl+C"), [this] { cpy(); });
    bind(QKeySequence("Ctrl+X"), [this] {
        cpy();
        cut = !clip.isEmpty();
    });
    bind(QKeySequence("Ctrl+V"), [this] { pst(); });
    bind(QKeySequence("Ctrl+L"), [this] {
        path->setFocus();
        path->selectAll();
    });
    bind(QKeySequence("Ctrl+F"), [this] {
        find->setFocus();
        find->selectAll();
    });
    bind(QKeySequence("Ctrl+H"), [this] { hid(); });
    bind(QKeySequence("F2"), [this] { ren(); });
    bind(QKeySequence(Qt::Key_Delete), [this] { del(); });
    bind(QKeySequence("Alt+Return"), [this] { prp(); });
}
