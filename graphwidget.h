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
#include <cmath>



class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr, QString name = "Pie");

    void addSlice(std::vector<std::vector<QueryC::outPTime>>&, int);
    void addSliceReset();
    void resortData();

public slots:
    void updateSliceLabels();
    void showList(QPieSlice*);
    void removeList(PieList*);
    void displayLabel(bool state);

private:
    QComboBox *displayMode;
    QComboBox *howSort;
    QChart *chart;
    QPieSeries *series;
    QChartView *chartView;
    QLabel *hoverLabel;
    std::vector<std::vector<QueryC::outPTime>> *data;
    QPushButton *closeButton;
    qreal totalValue = 0;
    std::vector<QPieSlice*> slices;
    int sT;
    QVBoxLayout *layout;

signals:
    void closeChart(GraphWidget *chart);
};

#endif // GRAPHWIDGET_H
