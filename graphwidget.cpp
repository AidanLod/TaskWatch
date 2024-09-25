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
    closeButton->setStyleSheet("QPushButton { "
                               "    background-color: white; "
                               "    border: 2px solid rgb(192, 191, 188); "
                               "    font-family: Serif; "
                               "}");

    displayMode = new QComboBox(this);
    displayMode->addItem("Display Time");
    displayMode->addItem("Display Percentage");
    displayMode->setStyleSheet("QComboBox { "
                            "   background-color: white; "
                            "   border: 2px solid rgb(192, 191, 188); "
                            "   font-family: Serif; "
                            "   color: black; "  // Set text color
                            "} "
                            "QComboBox QAbstractItemView { "
                            "    background-color: white; "
                            "    color: black; "
                            "    font-family: Serif; "
                            "} ");

    hoverLabel = new QLabel(this);
    hoverLabel->setText("Click to display data list.");
    hoverLabel->setAlignment(Qt::AlignCenter);
    hoverLabel->setStyleSheet("font-size: 16px; "
                          "color: black; "
                          "background-color: rgb(255, 204, 0); "
                          "border: 1px solid gray; "
                          "padding: 5px; "
                          "border-radius: 5px; ");
    hoverLabel->hide();

    setMouseTracking(true);
    layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    layout->addWidget(closeButton);
    layout->addWidget(displayMode);
    setLayout(layout);

    connect(closeButton, &QPushButton::clicked, [this](){
        emit closeChart(this);
    });
    connect(displayMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GraphWidget::updateSliceLabels);

    /*connect(this, &QWidget::resizeEvent, [this](QResizeEvent *) {
        closeButton->move(this->width() - closeButton->width() - 10, 10);
    });*/
}

void GraphWidget::addSlice(std::vector<std::vector<Q::outPTime>>& values, sortType t){
    data = &values;
    sT = t;
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
        totalValue += value;
        slice = new QPieSlice(label, value);
        slice->setBrush(colors[i % colors.size()]);
        slice->setProperty("baseLabel", label);
        connect(slice, &QPieSlice::doubleClicked, this, [this, slice]() {showList(slice);});
        connect(slice, &QPieSlice::hovered, this, &GraphWidget::displayLabel);
        i++;
        slices.push_back(slice);
        series->append(slice);
    }

    updateSliceLabels();

}

void GraphWidget::updateSliceLabels() {
    for (QPieSlice* &p : slices){
        if (displayMode->currentIndex() == 0){
            p->setLabel(QString("%1: %2").arg(p->property("baseLabel").toString()).arg(p->value()));
        }
        else {
            qreal percentage = (p->value() / totalValue) * 100.0;
            p->setLabel(QString("%1: %2%").arg(p->property("baseLabel").toString()).arg(percentage, 0, 'f', 2));
        }
    }
}

void GraphWidget::showList(QPieSlice* p) {

    PieList *list = new PieList(this);

    for (size_t i = 0; i < data->size(); i++){
        if (sT == ACTIVITY){
            if (p->property("baseLabel").toString().toStdString() == data->at(i).at(0).aName){
                list->addData(data->at(i));
                break;
            }

        }
        else if (sT == TYPE){
            if (p->property("baseLabel").toString().toStdString() == data->at(i).at(0).tName){
                list->addData(data->at(i));
                break;
            }

        }
        else if (sT == DATE){
            if (p->property("baseLabel").toString().toStdString() == data->at(i).at(0).date){
                list->addData(data->at(i));
                break;
            }

        }
    }
    connect(list, &PieList::closeList, this, &GraphWidget::removeList);
    layout->addWidget(list);

}

void GraphWidget::displayLabel(bool state){
    if (state) {
        QPoint mousePos = QCursor::pos();
        // Map the position to the widget's coordinates
        QPoint mappedPos = mapFromGlobal(mousePos);
        // Set the label position near the mouse
        hoverLabel->move(mappedPos.x() + 10, mappedPos.y() + 10);
        hoverLabel->show();  // Show label when hovered
    } else {
        hoverLabel->hide();  // Hide label when no longer hovered
    }
}

void GraphWidget::removeList(PieList *list){
    layout->removeWidget(list);
    list->deleteLater();

}
