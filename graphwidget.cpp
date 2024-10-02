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

    howSort = new QComboBox(this);
    howSort->addItem("Sort by Activity");
    howSort->addItem("Sort by Category");
    howSort->addItem("Sort by Date");
    howSort->setStyleSheet("QComboBox { "
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
    layout->addWidget(howSort);
    layout->addWidget(displayMode);
    layout->addWidget(chartView);
    layout->addWidget(closeButton);
    setLayout(layout);

    connect(closeButton, &QPushButton::clicked, [this](){
        emit closeChart(this);
    });
    connect(displayMode, &QComboBox::currentIndexChanged, this, &GraphWidget::updateSliceLabels);
    connect(howSort, &QComboBox::currentIndexChanged, this, &GraphWidget::addSliceReset);

    /*connect(this, &QWidget::resizeEvent, [this](QResizeEvent *) {
        closeButton->move(this->width() - closeButton->width() - 10, 10);
    });*/
}

void GraphWidget::addSlice(std::vector<std::vector<QueryC::outPTime>>& values, int t){
    data = &values;
    sT = t;
    QPieSlice *slice;
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta};
    int i = 0;
    for (std::vector<QueryC::outPTime>& a: values){
        bool set = false;
        QString label;
        qreal value = 0;
        for (QueryC::outPTime& b: a){

            if (!set){
                if (t == 0){
                    label = QString::fromStdString(b.aName);
                    howSort->setCurrentIndex(0);
                }
                else if (t == 1){
                    label = QString::fromStdString(b.tName);
                    howSort->setCurrentIndex(1);
                }
                else if (t == 2){
                    label = QString::fromStdString(b.date);
                    howSort->setCurrentIndex(2);
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

void GraphWidget::addSliceReset(){
    series->clear();
    slices.clear();
    sT = howSort->currentIndex();
    resortData();
    QPieSlice *slice;
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan, Qt::magenta};
    int i = 0;
    for (std::vector<QueryC::outPTime>& a: *data){
        bool set = false;
        QString label;
        qreal value = 0;
        for (QueryC::outPTime& b: a){

            if (!set){
                if (sT == 0){
                    label = QString::fromStdString(b.aName);
                    howSort->setCurrentIndex(0);
                    chart->setTitle("Activity Chart");
                }
                else if (sT == 1){
                    label = QString::fromStdString(b.tName);
                    howSort->setCurrentIndex(1);
                    chart->setTitle("Category Chart");
                }
                else if (sT == 2){
                    label = QString::fromStdString(b.date);
                    howSort->setCurrentIndex(2);
                    chart->setTitle("Date Chart");
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
        if (sT == 0){
            if (p->property("baseLabel").toString().toStdString() == data->at(i).at(0).aName){
                list->addData(data->at(i));
                break;
            }

        }
        else if (sT == 1){
            if (p->property("baseLabel").toString().toStdString() == data->at(i).at(0).tName){
                list->addData(data->at(i));
                break;
            }

        }
        else if (sT == 2){
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

void GraphWidget::resortData(){
    std::vector<QueryC::outPTime> temp;
    for (std::vector<QueryC::outPTime>& v: *data){
        for (QueryC::outPTime o: v){
            temp.push_back(o);
        }
    }
    data->clear();
    std::vector<std::vector<QueryC::outPTime>> sortedData;
    bool found = false;
    while(!temp.empty()){
        QueryC::outPTime pTime = temp.back();
        temp.pop_back();
        bool found = false;
        for (std::vector<QueryC::outPTime>& a: *data){
            if(sT == 1){
                if (a[0].tName == pTime.tName){
                    a.push_back(pTime);
                    found = true;
                    break;
                }
            }
            else if(sT == 2){
                if (a[0].date == pTime.date){
                    a.push_back(pTime);
                    found = true;
                    break;
                }
            }
            else {

                if (a[0].aName == pTime.aName){
                    a.push_back(pTime);
                    found = true;
                    break;
                }
            }

        }
        if (!found){
            std::vector<QueryC::outPTime> newCat;
            newCat.push_back(pTime);
            data->push_back(newCat);
        }
    }
}
