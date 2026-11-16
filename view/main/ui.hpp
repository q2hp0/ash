#pragma once

#include <QMainWindow>
#include <QTreeView>
#include <QStringList>
#include "nav.hpp"

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QResizeEvent;
class QModelIndex;
class bar;
class fs;
class panel;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

class fv : public QTreeView {
    Q_OBJECT

public:
    explicit fv(QWidget *parent = nullptr);
    void at(const QString &path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void dropped();

private:
    QString base;
};

class ui : public QMainWindow {
    Q_OBJECT

public:
    explicit ui(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    fs *f;
    QLabel *head;
    bar *tools;
    panel *side;
    QLineEdit *path;
    QLineEdit *find;
    fv *view;
    QListWidget *places;

    nv hist;
    QStringList clip;
    bool cut = false;
    bool hidden = false;

    void go(const QString &where, bool save = true);
    void bk();
    void fw();
    void up();
    void ref();
    void mkd();
    void mkf();
    void del();
    void opn(const QModelIndex &index);
    void ren();
    void cpy();
    void pst();
    void prp();
    void hid();
    void srch(const QString &value);
    void pvw();
    void fit();
    void key();
    void sts();

    QStringList sel();
};
