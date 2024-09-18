#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , database(nullptr)
    , chartCounter(0)
{
    ui->setupUi(this);
    layout = new QVBoxLayout();
    ui->centralwidget->setLayout(layout);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::showChart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(Q::QueryClass& qc){
    database = &qc;
}

void MainWindow::showChart(){
    QString sort = ui->selectType->currentText();
    sortType type;
    if (sort == "Activity"){
        type = ACTIVITY;
    }
    else if (sort == "Category"){
        type = TYPE;
    }
    else if (sort == "Date"){
        type = DATE;
    }
    QString chartName = sort + " Chart";
    Q::data temp;
    temp.request = true;
    const QString date = ui->selectStartDate->date().toString("yyyy-MM-dd");
    temp.date = date.toStdString();
    database->handleTraffic(temp);
    std::queue<Q::outPTime> unsortedData;
    unsortedData = database->getData();
    while(!unsortedData.empty()){
        Q::outPTime pTime = unsortedData.front();
        unsortedData.pop();
        bool found = false;
        for (std::vector<Q::outPTime>& a: graphableData){
            if(sort == "Category"){
                type = TYPE;
                if (a[0].tName == pTime.tName){
                    a.push_back(pTime);
                    found = true;
                    break;
                }
            }
            else if(sort == "Date"){
                type = DATE;
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
            std::vector<Q::outPTime> newCat;
            newCat.push_back(pTime);
            graphableData.push_back(newCat);
        }
    }

    ++chartCounter;
    GraphWidget *chart = new GraphWidget(this, chartName);
    connect(chart, &GraphWidget::closeChart, this, &MainWindow::removeChart);
    chart->addSlice(graphableData, type);
    layout->addWidget(chart);

}

void MainWindow::removeChart(GraphWidget *chart){
    layout->removeWidget(chart);
    chart->deleteLater();
    --chartCounter;
}

void MainWindow::changeActivity() {
    QString activity = ui->activity->currentText();
    Q::data temp;
    temp.activityName = activity.toStdString();
    temp.table = Q::ACTIVITY;

    database->handleTraffic(temp);
}
