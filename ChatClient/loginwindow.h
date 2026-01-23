#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow;
}
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots: // slots es une fonction qui permet de réagir à un signal(button, slider, timer)
    void onRegisterClicked();
    void onBackToLoginClicked();

private:
    Ui::LoginWindow *ui;

};

#endif // LOGINWINDOW_H
