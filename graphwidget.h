#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QVBoxLayout>
#include "QueryClass.h"
#include "pielist.h"
#include <vector>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

enum sortType {ACTIVITY, TYPE, DATE};

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr, QString name = "Pie");

    void addSlice(std::vector<std::vector<Q::outPTime>>&, sortType);

public slots:
    void updateSliceLabels();
    void showList(QPieSlice*);
    void removeList(PieList*);
    void displayLabel(bool state);

private:
    QComboBox *displayMode;
    QChart *chart;
    QPieSeries *series;
    QChartView *chartView;
    QLabel *hoverLabel;
    std::vector<std::vector<Q::outPTime>> *data;
    QPushButton *closeButton;
    qreal totalValue = 0;
    std::vector<QPieSlice*> slices;
    sortType sT;
    QVBoxLayout *layout;

signals:
    void closeChart(GraphWidget *chart);
};

#endif // GRAPHWIDGET_H
