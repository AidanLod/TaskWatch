#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QVBoxLayout>
#include "QueryClass.h"
#include <vector>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

enum sortType {ACTIVITY, TYPE, DATE};

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr, QString name = "Chart");

    void addSlice(std::vector<std::vector<Q::outPTime>>&, sortType);

private:
    QChart *chart;
    QPieSeries *series;
    QChartView *chartView;
    std::vector<std::vector<Q::outPTime>> *data;
    QPushButton *closeButton;

signals:
    void closeChart(GraphWidget *chart);
};

#endif // GRAPHWIDGET_H
