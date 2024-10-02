#include "messagepopup.h"

MessagePopup::MessagePopup(QWidget *parent)
    : QMessageBox{parent}
{
    this->setStyleSheet("QMessageBox { "
                        "background-color: white; "
                        "border: 2px solid rgb(192, 191, 188); "
                        "text-align: center;"
                        "}"
                        "QLabel { "
                        "color: black; "
                        "font-family: serif; "
                        "font-weight: bold; "
                        "text-align: center;"
                        "}"
                        "QLabel#qt_msgbox_informativelabel { "
                        "font-weight: normal; "
                        "text-align: center;"
                        "}"
                        "QPushButton { "
                        "background-color: white; "
                        "color: black; "
                        "font-family: serif; "
                        "font-weight: bold; "
                        "border: 2px solid rgb(192, 191, 188); "
                        "padding: 5px; "
                        "text-align: center;"
                        "}"
                        "QPushButton:focus { "
                        "outline: none; "
                        "}"
                        );
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
}

void MessagePopup::setMessage(std::string& title, std::string& info){
    this->setText(QString::fromStdString(title));
    this->setInformativeText(QString::fromStdString(info));
}

void MessagePopup::showMessage(){
    this->exec();
}
