#pragma once

#include <QWidget>
#include <QStringList>

class QLabel;

class panel : public QWidget {
public:
    explicit panel(QWidget *parent = nullptr);
    void put(const QStringList &paths);

private:
    QLabel *thumb;
    QLabel *meta;
};