#include "pielist.h"

PieList::PieList(QWidget *parent)
    : QWidget{parent}
{
    chooseSort = new QComboBox(this);
    chooseSort->addItem("Sort by Activity");
    chooseSort->addItem("Sort by Category");
    chooseSort->addItem("Sort by Date");
    chooseSort->setStyleSheet("QComboBox { "
                              "   background-color: white; "
                              "   border: 2px solid rgb(192, 191, 188); "
                              "   font-family: Serif; "
                              "   color: black; "  // Set text color
                              "} "
                              "QComboBox QAbstractItemView { "
                              "    background-color: white; "
                              "    color: black; "
                              "    font-family: Serif; "
                              "}");

    closeButton = new QPushButton("Close", this);
    closeButton->setStyleSheet("QPushButton { "
                               "    background-color: white; "
                               "    border: 2px solid rgb(192, 191, 188); "
                               "    font-family: Serif; "
                               "}");

    dataList = new QListWidget(this);
    dataList->setStyleSheet("QListWidget { "
                            "    background-color: white; "
                            "    border: 2px solid rgb(192, 191, 188); "
                            "    color: black; "  // Set text color
                            "    font-family: Serif; "
                            "} "
                            "QListWidget::item { "
                            "    color: black; "  // Set item text color
                            "}");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chooseSort);
    layout->addWidget(dataList);
    layout->addWidget(closeButton);

    setLayout(layout);
    connect(chooseSort, &QComboBox::currentIndexChanged, this, &PieList::sortList);
    connect(closeButton, &QPushButton::clicked, [this](){
        emit closeList(this);
    });

}

void PieList::addData(std::vector<Q::outPTime>& d){
    data = d;
    if (d.empty()){
        std::cout << "empty\n";
    }
    sortList();
}

void PieList::sortList(){
    std::cout << "sort\n";
    dataList->clear();
    QString cSection;
    QString item;
    if (chooseSort->currentIndex() == 0){
        for (Q::outPTime &o: data){
            if (cSection.toStdString() != o.aName){
                dataList->addItem(QString::fromStdString(o.aName) + ":");
                cSection = QString::fromStdString(o.aName);
            }
            item = QString::fromStdString(o.pName) + ":: Time Used: " + QString::number(o.timeUsed);
            std::cout << item.toStdString() << std::endl;
            dataList->addItem(item);
        }
    }
    else if (chooseSort->currentIndex() == 1){
        for (Q::outPTime &o: data){
            if (cSection.toStdString() != o.tName){
                dataList->addItem(QString::fromStdString(o.tName) + ":");
                cSection = QString::fromStdString(o.tName);
            }
            item = QString::fromStdString(o.pName) + ":: Time Used: " + QString::number(o.timeUsed);
            std::cout << item.toStdString() << std::endl;
            dataList->addItem(item);
        }
    }
    else if (chooseSort->currentIndex() == 2){
        for (Q::outPTime &o: data){
            if (cSection.toStdString() != o.date){
                dataList->addItem(QString::fromStdString(o.date) + ":");
                cSection = QString::fromStdString(o.date);
            }
            item = QString::fromStdString(o.pName) + ":: Time Used: " + QString::number(o.timeUsed);
            std::cout << item.toStdString() << std::endl;
            dataList->addItem(item);
        }
    }


}
