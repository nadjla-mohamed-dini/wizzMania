#include "principalwindow.h"
#include "ui_principalwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDateTime>

PrincipalWindow::PrincipalWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrincipalWindow)
{
    ui->setupUi(this);
    setWindowTitle("MSN CHAT - Accueil");
    resize(700, 600);

    connect(ui->sendButton, &QPushButton::clicked,
            this, &PrincipalWindow::onSendClicked);

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, &PrincipalWindow::onSendClicked);

    connect(ui->statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PrincipalWindow::onStatusChanged);

    connect(ui->lunchGameButton, &QPushButton::clicked,
            this, &PrincipalWindow::onLaunchGameClicked);

    setupGamesTab();
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
        QString currentTime = QDateTime::currentDateTime().toString("hh:mm");

        QString formettedMessage = QString ("<span style= 'color:#999;'>[%1]</span>"
                                            "<span style= 'color:#FF6600; font-weight:bold; '%2:</span>"
                                            "<span style= 'color:#E0E0E0; '>%3</span>")
                                        .arg(currentTime)
                                        .arg(currentUsername)
                                        .arg(message);

        ui->chatDisplay->append(formettedMessage);
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
//ajouter la possibilite d'ajouter une photo de profil
