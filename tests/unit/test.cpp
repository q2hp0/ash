#include "nav.hpp"
#include "ops.hpp"
#include "ut.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    nv nav;
    if (!nav.put("/a") || !nav.put("/b") || nav.bk() != "/a" ||
        nav.fw() != "/b" || !nav.put("/c") || !nav.fw().isEmpty())
        return 1;

    if (ut::sz(0) != "0 B" || ut::sz(1024) != "1.00 KB" ||
        !ut::name("file.txt") || ut::name("../file") || ut::name("."))
        return 2;

    QTemporaryDir temp;
    if (!temp.isValid())
        return 3;

    const QString root = temp.path();
    const QString source = QDir(root).filePath("src");
    const QString target = QDir(root).filePath("dst");
    if (!QDir().mkpath(QDir(source).filePath("nested")))
        return 4;

    QFile file(QDir(source).filePath("nested/data.txt"));
    if (!file.open(QIODevice::WriteOnly) || file.write("ash") != 3)
        return 5;
    file.close();

    if (!op::cp(source, target) ||
        !QFileInfo::exists(QDir(target).filePath("nested/data.txt")) ||
        op::cp(source, target) ||
        op::cp(source, QDir(source).filePath("child")))
        return 6;

    const QString unique = op::unq(QDir(root).filePath("name.txt"));
    if (!unique.endsWith("name (copy 1).txt"))
        return 7;

    const QString moved = QDir(root).filePath("moved");
    if (!op::mv(target, moved) || QFileInfo::exists(target) ||
        !QFileInfo::exists(QDir(moved).filePath("nested/data.txt")) ||
        !op::rm(moved) || QFileInfo::exists(moved))
        return 8;

    return 0;
}