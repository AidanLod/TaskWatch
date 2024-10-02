#ifndef PIELIST_H
#define PIELIST_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>
#include "QueryClass.h"


class PieList : public QWidget
{
    Q_OBJECT
public:
    explicit PieList(QWidget *parent = nullptr);
    void addData(std::vector<QueryC::outPTime>&);
signals:
    void closeList(PieList *list);
private slots:
    void sortList();

private:
    QListWidget* dataList;
    QComboBox* chooseSort;
    QPushButton* closeButton;
    std::vector<QueryC::outPTime> data;
};

#endif // PIELIST_H
