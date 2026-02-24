#include "MainWindow.hpp"

#include <QApplication>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_client = new ChatClient(this);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);

    auto* top = new QHBoxLayout();
    m_host = new QLineEdit("127.0.0.1", central);
    m_port = new QSpinBox(central);
    m_port->setRange(1, 65535);
    m_port->setValue(4242);
    m_connectBtn = new QPushButton("Connect", central);
    m_status = new QLabel("Disconnected", central);

    top->addWidget(new QLabel("Host:", central));
    top->addWidget(m_host, 1);
    top->addWidget(new QLabel("Port:", central));
    top->addWidget(m_port);
    top->addWidget(m_connectBtn);
    top->addWidget(m_status);
    root->addLayout(top);

    m_chat = new QTextBrowser(central);
    m_chat->setOpenExternalLinks(false);
    root->addWidget(m_chat, 1);

    auto* bottom = new QHBoxLayout();
    m_input = new QLineEdit(central);
    m_sendBtn = new QPushButton("Send", central);
    m_wizzBtn = new QPushButton("Wizz", central);
    bottom->addWidget(m_input, 1);
    bottom->addWidget(m_sendBtn);
    bottom->addWidget(m_wizzBtn);
    root->addLayout(bottom);

    setCentralWidget(central);
    setWindowTitle("WizzMania");
    resize(820, 560);

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_wizzBtn, &QPushButton::clicked, this, &MainWindow::onWizzClicked);
    connect(m_input, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);

    connect(m_client, &ChatClient::connected, this, &MainWindow::onConnected);
    connect(m_client, &ChatClient::disconnected, this, &MainWindow::onDisconnected);
    connect(m_client, &ChatClient::errorOccurred, this, &MainWindow::onError);
    connect(m_client, &ChatClient::messageReceived, this, &MainWindow::onMessage);
    connect(m_client, &ChatClient::wizzReceived, this, &MainWindow::onWizz);
    connect(m_client, &ChatClient::userConnected, this, &MainWindow::onUserConnected);
    connect(m_client, &ChatClient::userDisconnected, this, &MainWindow::onUserDisconnected);

    setUiConnected(false);

    bool ok = false;
    QString pseudo = QInputDialog::getText(
        this,
        "Pseudo",
        "Entrez votre pseudo :",
        QLineEdit::Normal,
        "",
        &ok
    ).trimmed();

    if (!ok || pseudo.isEmpty())
    {
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    m_client->connectToServer(m_host->text(), static_cast<quint16>(m_port->value()), pseudo);
    appendSystem(QString("<i>Pseudo: <b>%1</b></i>").arg(pseudo.toHtmlEscaped()));
}

void MainWindow::onConnectClicked()
{
    if (m_client->isConnected())
    {
        appendSystem("<i>Déconnexion…</i>");
        m_client->disconnectFromServer();
        return;
    }

    if (m_client->pseudo().trimmed().isEmpty())
    {
        appendSystem("<span style='color:#c00'><b>Pseudo manquant.</b></span>");
        return;
    }

    appendSystem("<i>Connexion…</i>");
    m_client->connectToServer(m_host->text(),
                              static_cast<quint16>(m_port->value()),
                              m_client->pseudo());
}

void MainWindow::onSendClicked()
{
    const QString text = m_input->text().trimmed();
    if (text.isEmpty())
        return;

    if (!m_client->isConnected())
    {
        appendSystem("<span style='color:#c00'><b>Non connecté.</b></span>");
        return;
    }

    m_client->sendMessage(text);
    appendChatLine(m_client->pseudo(), text);
    m_input->clear();
    m_input->setFocus();
}

void MainWindow::onWizzClicked()
{
    if (!m_client->isConnected())
    {
        appendSystem("<span style='color:#c00'><b>Non connecté.</b></span>");
        return;
    }

    m_client->sendWizz();
    appendSystem("<b>Tu as envoyé un Wizz.</b>");
}

void MainWindow::onConnected()
{
    appendSystem("<span style='color:#090'><b>Connecté.</b></span>");
    setUiConnected(true);
}

void MainWindow::onDisconnected()
{
    appendSystem("<span style='color:#666'><b>Déconnecté.</b></span>");
    setUiConnected(false);
}

void MainWindow::onError(const QString& message)
{
    appendSystem(QString("<span style='color:#c00'><b>Erreur:</b> %1</span>")
                     .arg(message.toHtmlEscaped()));
    setUiConnected(m_client->isConnected());
}

void MainWindow::onMessage(const QString& author, const QString& content)
{
    appendChatLine(author, content);
}

void MainWindow::onWizz(const QString& author)
{
    showWizzEffect(author);
}

void MainWindow::onUserConnected(const QString& author)
{
    appendSystem(QString("<b>%1</b> s'est connecté.").arg(author.toHtmlEscaped()));
}

void MainWindow::onUserDisconnected(const QString& author)
{
    appendSystem(QString("<b>%1</b> s'est déconnecté.").arg(author.toHtmlEscaped()));
}

void MainWindow::appendSystem(const QString& html)
{
    m_chat->append(QString("<div style='color:#444'>%1</div>").arg(html));
}

void MainWindow::appendChatLine(const QString& author, const QString& content)
{
    const QString a = author.toHtmlEscaped();
    const QString c = content.toHtmlEscaped();
    m_chat->append(QString("<div><b>[%1]</b> %2</div>").arg(a, c));
}

void MainWindow::showWizzEffect(const QString& author)
{
    QApplication::beep();

    appendSystem(QString(
        "<div style='border:2px solid #e39; padding:6px; background:#fff3fa'>"
        "<span style='color:#b05'><b>W I Z Z</b></span> de <b>%1</b>"
        "</div>"
    ).arg(author.toHtmlEscaped()));

    if (isMinimized())
        showNormal();
    raise();
    activateWindow();

    startShake();
}

void MainWindow::setUiConnected(bool connected)
{
    m_status->setText(connected ? "Connected" : "Disconnected");
    m_connectBtn->setText(connected ? "Disconnect" : "Connect");

    m_host->setEnabled(!connected);
    m_port->setEnabled(!connected);

    m_sendBtn->setEnabled(connected);
    m_wizzBtn->setEnabled(connected);
    m_input->setEnabled(connected);
}

void MainWindow::startShake()
{
    m_shakeOrigin = pos();
    m_shakeStep = 0;

    auto* timer = new QTimer(this);
    timer->setInterval(16);

    connect(timer, &QTimer::timeout, this, [this, timer]() {
        constexpr int totalSteps = 20;
        constexpr int amplitude = 10;

        const int dir = (m_shakeStep % 2 == 0) ? 1 : -1;
        const int dx = dir * amplitude;
        const int dy = -dir * (amplitude / 2);
        move(m_shakeOrigin + QPoint(dx, dy));

        m_shakeStep++;
        if (m_shakeStep >= totalSteps)
        {
            timer->stop();
            move(m_shakeOrigin);
            timer->deleteLater();
        }
    });

    timer->start();
}

