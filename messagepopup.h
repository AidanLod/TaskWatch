#ifndef MESSAGEPOPUP_H
#define MESSAGEPOPUP_H

#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <string>


class MessagePopup : public QMessageBox
{
public:
    explicit MessagePopup(QWidget *parent = nullptr);

    void setMessage(std::string& title, std::string& info);
    void showMessage();

};

#endif // MESSAGEPOPUP_H
