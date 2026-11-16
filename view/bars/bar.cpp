#include "bar.hpp"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

bar::bar(QWidget *parent) : QWidget(parent) {
    auto *base = new QVBoxLayout(this);
    base->setContentsMargins(9, 8, 9, 5);
    base->setSpacing(5);

    auto *nav = new QHBoxLayout;
    nav->setSpacing(5);
    prev = new QPushButton("<", this);
    next = new QPushButton(">", this);
    up = new QPushButton("up", this);
    ref = new QPushButton("↻", this);
    home = new QPushButton("home", this);
    loc = new QLineEdit(this);
    loc->setPlaceholderText("location");
    nav->addWidget(prev);
    nav->addWidget(next);
    nav->addWidget(up);
    nav->addWidget(ref);
    nav->addWidget(home);
    nav->addWidget(loc, 1);
    base->addLayout(nav);

    auto *tools = new QHBoxLayout;
    tools->setSpacing(5);
    folder = new QPushButton("new folder", this);
    file = new QPushButton("new file", this);
    copy = new QPushButton("copy", this);
    cut = new QPushButton("cut", this);
    paste = new QPushButton("paste", this);
    trash = new QPushButton("trash", this);
    props = new QPushButton("properties", this);
    hide = new QPushButton("hidden", this);
    find = new QLineEdit(this);
    find->setPlaceholderText("search files...");
    tools->addWidget(folder);
    tools->addWidget(file);
    tools->addWidget(copy);
    tools->addWidget(cut);
    tools->addWidget(paste);
    tools->addWidget(trash);
    tools->addWidget(props);
    tools->addWidget(hide);
    tools->addWidget(find, 1);
    base->addLayout(tools);
}