#include "graphwidget.h"

GraphWidget::GraphWidget(QWidget *parent, QString name)
    : QWidget{parent}
{
    chart = new QChart();
    series = new QPieSeries();
    chart->addSeries(series);
    chart->setTitle(name);

    chartView = new QChartView(chart);

    closeButton = new QPushButton("Close", this);
    closeButton->setFixedSize(50,30);
    closeButton->move(this->width() - closeButton->width() - 10, 10);
    closeButton->setStyleSheet("QPushButton { "
                               "background-color: white; "
                               "border: 2px solid rgb(192, 191, 188); "
                               "}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    layout->addWidget(closeButton);
    setLayout(layout);

    connect(closeButton, &QPushButton::clicked, [this](){
        emit closeChart(this);
    });

    /*connect(this, &QWidget::resizeEvent, [this](QResizeEvent *) {
        closeButton->move(this->width() - closeButton->width() - 10, 10);
    });*/
}

void GraphWidget::addSlice(std::vector<std::vector<Q::outPTime>>& values, sortType t){
    QPieSlice *slice;
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta};
    int i = 0;
    for (std::vector<Q::outPTime>& a: values){
        bool set = false;
        QString label;
        qreal value = 0;
        for (Q::outPTime& b: a){

            if (!set){
                if (t == ACTIVITY){
                    label = QString::fromStdString(b.aName);
                }
                else if (t == TYPE){
                    label = QString::fromStdString(b.tName);
                }
                else if (t == DATE){
                    label = QString::fromStdString(b.date);
                }
                set = true;
            }
            value += static_cast<qreal>(b.timeUsed);
        }
        slice = new QPieSlice(label, value);
        slice->setBrush(colors[i % colors.size()]);
        i++;
        series->append(slice);
    }
}
