#include "mainwindow.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <fstream>
#include "WWindow.h"
#include "QueryClass.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    unsigned char ecode; //error code for error popup handling
    QApplication a(argc, argv);
    QueryC::QueryClass qc(ecode);
    W::WWindow changeCheck(qc, ecode);
    MainWindow w;
    w.init(qc);

    w.show();
    return a.exec();
}
