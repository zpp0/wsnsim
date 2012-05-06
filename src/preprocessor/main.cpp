#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Q_INIT_RESOURCE(configdialog);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
