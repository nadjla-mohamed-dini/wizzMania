#include "principalwindow.h"
#include "ui_principalwindow.h"
#include <QPushButton>
#include <QMessageBox>

PrincipalWindow::PrincipalWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrincipalWindow)
{
    ui->setupUi(this);
    setWindowTitle("MSN CHAT - Accueil");
    resize(700, 600);

    // Corriger les noms de classe : PrincipalWindow au lieu de ChatWindow
    connect(ui->sendButton, &QPushButton::clicked,
            this, &PrincipalWindow::onSendClicked);

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, &PrincipalWindow::onSendClicked);

    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PrincipalWindow::onStatusChanged);

    connect(ui->lunchGameButton, &QPushButton::clicked,  // launchGameButton (pas lauchGameButton)
            this, &PrincipalWindow::onLaunchGameClicked);

    setupGamesTab();  // setupGamesTab (pas setUpGamesTab)
}

PrincipalWindow::~PrincipalWindow()
{
    delete ui;
}

void PrincipalWindow::setUsername(const QString &username)
{
    currentUsername = username;

    // Afficher dans l'onglet Home
    ui->homeUsernameLabel->setText("👤 " + username);

    ui->chatDisplay->append("===== Bienvenue " + username + "=====!");
    ui->chatDisplay->append("Commencez une discussion");

    ui->homeTitleLabel->setText("==== Bienvenue " + username + "====!");
    ui->homeUsernameLabel->setText(username);
}

void PrincipalWindow::onSendClicked()
{
    QString message = ui->messageInput->text();

    if (!message.isEmpty())
    {
        ui->chatDisplay->append(currentUsername + ": " + message);
        ui->messageInput->clear();
        // TODO: Envoyer au serveur TCP/IP
    }
}

void PrincipalWindow::onStatusChanged(int index)
{
    QString status = ui->statusComboBox->currentText();
    ui->chatDisplay->append("📢 Statut changé : " + status);
    // TODO: Envoyer le changement de statut au serveur
}

void PrincipalWindow::onLaunchGameClicked()
{
    QString selectedGame = ui->gamesListWidget->currentItem()
                               ? ui->gamesListWidget->currentItem()->text()
                               : "";

    if (selectedGame.isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Sélectionnez un jeu !");
        return;
    }

    QMessageBox::information(this, "Jeu", "Lancement de : " + selectedGame);
}

void PrincipalWindow::setupGamesTab()
{
    // Ajouter des jeux à la liste

}
