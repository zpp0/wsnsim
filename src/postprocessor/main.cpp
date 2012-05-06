#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //устанавливаем кодировку строк в UTF-8 для корректного отображения текста
    QTextCodec *russianCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(russianCodec);

    MainWindow w;
    w.resize(800, 600);
    w.show();

    return a.exec();
}
