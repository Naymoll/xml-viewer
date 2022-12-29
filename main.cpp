#include "mainwindow.h"
#include <QSqlError>

#include <QApplication>

int main(int argc, char *argv[]) {
    qRegisterMetaType<QVector<CodeEditor>>("QVector<CodeEditor>");
    qRegisterMetaType<CodeEditor>("CodeEditor");
    qRegisterMetaType<QDir>("QDir");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
