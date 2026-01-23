#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QPushButton>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setWindowTitle("MSN Chat - Inscription");
    resize(400, 500);

    // Connecter les boutons aux fonctions
    connect(ui->registerButton, &QPushButton::clicked,
            this, &LoginWindow::onRegisterClicked);

    connect(ui->backToLoginButton, &QPushButton::clicked,
            this, &LoginWindow::onBackToLoginClicked);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::onRegisterClicked()
{
    // Récupérer les données des champs
    QString username = ui->usernameLineEdit->text();
    QString email = ui->emailLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString confirm = ui->confirmPasswordLineEdit->text();

    // Vérifier que tous les champs sont remplis
    if (username.isEmpty() || email.isEmpty() ||
        password.isEmpty() || confirm.isEmpty())
    {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs !");
        return;
    }

    // Vérifier que les mots de passe correspondent
    if (password != confirm)
    {
        QMessageBox::warning(this, "Erreur",
                             "Les mots de passe ne correspondent pas !");
        return;
    }

    // Inscription réussie
    QMessageBox::information(this, "Succès",
                             "Compte créé pour : " + username);

    // Ouvrir la fenêtre de connexion
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
    this->close();
}

void LoginWindow::onBackToLoginClicked()
{
    // Retour à la page de connexion
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
    this->close();
}
