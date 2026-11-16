#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;

class bar : public QWidget {
public:
    explicit bar(QWidget *parent = nullptr);

    QLineEdit *loc;
    QLineEdit *find;
    QPushButton *prev;
    QPushButton *next;
    QPushButton *up;
    QPushButton *ref;
    QPushButton *home;
    QPushButton *folder;
    QPushButton *file;
    QPushButton *copy;
    QPushButton *cut;
    QPushButton *paste;
    QPushButton *trash;
    QPushButton *props;
    QPushButton *hide;
};