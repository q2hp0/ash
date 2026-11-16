#include "ops.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QTemporaryDir temp;
    if (!temp.isValid())
        return 1;

    const QString src = QDir(temp.path()).filePath("src");
    const QString dst = QDir(temp.path()).filePath("dst");
    if (!QDir().mkpath(src))
        return 2;

    for (int i = 0; i < 256; ++i) {
        QFile file(QDir(src).filePath("f" + QString::number(i)));
        if (!file.open(QIODevice::WriteOnly) || file.write(QByteArray(4096, 'x')) != 4096)
            return 3;
    }

    QElapsedTimer timer;
    timer.start();
    if (!op::cp(src, dst))
        return 4;

    QTextStream(stdout) << "copied 256 files in "
                        << timer.elapsed() << " ms\n";
    return 0;
}