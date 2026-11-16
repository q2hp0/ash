#include <QApplication>
#include <QIcon>
#include <QDebug>
#include "app.hpp"
#include "ui.hpp"
#include "io.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(ash::name);
    app.setApplicationVersion(ash::ver);
    app.setWindowIcon(QIcon(":/icons/ash.svg"));

    QByteArray style;
    QString error;

    if (io::rd(":/styles/main.qss", style, error))
        app.setStyleSheet(QString::fromUtf8(style));
    else
        qWarning().noquote() << error;

    ui window;
    window.show();

    return app.exec();
}
