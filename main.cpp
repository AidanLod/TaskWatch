#include "mainwindow.h"

#include <QApplication>
#include "QueryClass.h"
#include "WWindow.h"

int main(int argc, char *argv[])
{
    unsigned char ecode;
    Q::QueryClass qc(ecode);
    W::WWindow changeCheck(qc, ecode);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
