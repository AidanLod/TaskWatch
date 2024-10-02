#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , database(nullptr)
    , chartCounter(0)
{
    ui->setupUi(this);
    setWindowTitle("TaskWatch");
    layout = new QVBoxLayout();
    ui->centralwidget->setLayout(layout);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::disableButtons);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::changeActivity);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(QueryC::QueryClass& qc){
    database = &qc;
}

void MainWindow::showChart(){
    QString sort = ui->selectType->currentText();
    int type = ui->selectType->currentIndex();
    QString chartName = sort + " Chart";
    QueryC::data temp;
    temp.request = true;
    const QString date = ui->selectStartDate->date().toString("yyyy-MM-dd");
    temp.date = date.toStdString();
    database->handleTraffic(temp);
    std::queue<QueryC::outPTime> unsortedData;
    graphableData.clear();
    unsortedData = database->getData();
    while(!unsortedData.empty()){
        QueryC::outPTime pTime = unsortedData.front();
        unsortedData.pop();
        bool found = false;
        for (std::vector<QueryC::outPTime>& a: graphableData){
            if(sort == "Category"){
                type = 1;
                if (a[0].tName == pTime.tName){
                    a.push_back(pTime);
                    found = true;
                    break;
                }
            }
            else if(sort == "Date"){
                type = 2;
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
    ui->pushButton->setDisabled(false);
    ui->pushButton_2->setDisabled(false);
    --chartCounter;
}

void MainWindow::changeActivity() {
    QString activity = ui->activity->currentText();
    QueryC::data temp;
    temp.activityName = activity.toStdString();
    temp.table = QueryC::ACTIVITY;

    database->handleTraffic(temp);
}

void MainWindow::disableButtons(){
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
}

void MainWindow::displayMessage(){
    MessagePopup temp;
    std::string title = "Activity Changed";
    std::string info = "Activity changed to " + ui->activity->currentText().toStdString();
    temp.setMessage(title, info);
    temp.showMessage();
}
