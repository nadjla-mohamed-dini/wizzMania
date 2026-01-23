#ifndef PRINCIPALWINDOW_H
#define PRINCIPALWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PrincipalWindow; }
QT_END_NAMESPACE

class PrincipalWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PrincipalWindow(QWidget *parent = nullptr);
    ~PrincipalWindow();

    void setUsername(const QString &username);

private slots:
    void onSendClicked();
    void onStatusChanged(int index);
    void onLaunchGameClicked();

private:
    Ui::PrincipalWindow *ui;
    QString currentUsername;
    void setupGamesTab();
};

#endif // PRINCIPALWINDOW_H
