#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "principalwindow.h"
#include "loginwindow.h"
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MSN Connexion");

    // Connecter les boutons
    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginClicked);

    connect(ui->registerButton, &QPushButton::clicked,
            this, &MainWindow::onRegisterClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoginClicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs !");
        return;
    }
    PrincipalWindow *principalWin = new PrincipalWindow();
    principalWin ->setUsername(username);
    principalWin->show();
    this->close();


    QMessageBox::information(this, "Connexion",
                             "Connexion de : " + username);
}

void MainWindow::onRegisterClicked()
{
    // Ouvrir la fenêtre d'inscription
    LoginWindow *loginWin = new LoginWindow();
    loginWin->show();
    this->close();
}
