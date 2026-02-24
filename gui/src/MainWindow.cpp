#include "MainWindow.hpp"

#include <QApplication>
#include <QDateTime>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QListWidget>
#include <QPainter>
#include <QPixmap>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_client = new ChatClient(this);

    auto* central = new QWidget(this);
    central->setObjectName("central");
    central->setStyleSheet(
        "QWidget#central {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f4f8ff, stop:1 #e7f0ff);"
        "}"
        "QLineEdit { padding: 6px 8px; border: 1px solid #b9c8e6; border-radius: 8px; background: white; }"
        "QSpinBox  { padding: 6px 8px; border: 1px solid #b9c8e6; border-radius: 8px; background: white; }"
        "QPushButton { padding: 6px 12px; border-radius: 10px; background: #2f6fed; color: white; }"
        "QPushButton:disabled { background: #9cb7f7; }"
        "QPushButton#wizzBtn { background: #ff4da6; }"
        "QTextBrowser { border: 1px solid #b9c8e6; border-radius: 12px; background: rgba(255,255,255,0.92); }"
        "QListWidget { border: 1px solid #b9c8e6; border-radius: 12px; background: rgba(255,255,255,0.92); }"
    );
    auto* root = new QVBoxLayout(central);

    auto* top = new QHBoxLayout();
    m_host = new QLineEdit("127.0.0.1", central);
    m_port = new QSpinBox(central);
    m_port->setRange(1, 65535);
    m_port->setValue(4242);
    m_pseudo = new QLineEdit(central);
    m_pseudo->setPlaceholderText("Pseudo");
    m_connectBtn = new QPushButton("Connect", central);
    m_status = new QLabel("Disconnected", central);
    m_status->setStyleSheet("color:#345; font-weight:600;");

    top->addWidget(new QLabel("Host:", central));
    top->addWidget(m_host, 1);
    top->addWidget(new QLabel("Port:", central));
    top->addWidget(m_port);
    top->addWidget(new QLabel("Pseudo:", central));
    top->addWidget(m_pseudo);
    top->addWidget(m_connectBtn);
    top->addWidget(m_status);
    root->addLayout(top);

    auto* middle = new QHBoxLayout();
    m_chat = new QTextBrowser(central);
    m_chat->setOpenExternalLinks(false);
    m_chat->setStyleSheet("font-family: Segoe UI; font-size: 11pt;");

    m_contacts = new QListWidget(central);
    m_contacts->setFixedWidth(220);
    m_contacts->setStyleSheet("font-family: Segoe UI; font-size: 10.5pt;");

    middle->addWidget(m_chat, 1);
    middle->addWidget(m_contacts);
    root->addLayout(middle, 1);

    auto* bottom = new QHBoxLayout();
    m_input = new QLineEdit(central);
    m_input->setPlaceholderText("Écris ton message…");
    m_sendBtn = new QPushButton("Send", central);
    m_wizzBtn = new QPushButton("Wizz", central);
    m_wizzBtn->setObjectName("wizzBtn");
    bottom->addWidget(m_input, 1);
    bottom->addWidget(m_sendBtn);
    bottom->addWidget(m_wizzBtn);
    root->addLayout(bottom);

    setCentralWidget(central);
    setWindowTitle("WizzMania");
    resize(820, 560);

    m_connectBtn->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    m_sendBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    m_wizzBtn->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));

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
    appendSystem("<b>Bienvenue sur WizzMania.</b> Renseigne ton pseudo puis clique sur <b>Connect</b>.");
}

void MainWindow::onConnectClicked()
{
    if (m_client->isConnected())
    {
        appendSystem("<i>Déconnexion…</i>");
        m_client->disconnectFromServer();
        return;
    }

    const QString pseudo = m_pseudo->text().trimmed();
    if (pseudo.isEmpty())
    {
        appendSystem("<span style='color:#c00'><b>Pseudo manquant.</b></span>");
        return;
    }

    appendSystem("<i>Connexion…</i>");
    m_client->connectToServer(m_host->text(),
                              static_cast<quint16>(m_port->value()),
                              pseudo);
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
    m_contacts->clear();
    upsertContact(m_client->pseudo(), true);
}

void MainWindow::onDisconnected()
{
    appendSystem("<span style='color:#666'><b>Déconnecté.</b></span>");
    setUiConnected(false);
    m_contacts->clear();
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
    upsertContact(author, false);
}

void MainWindow::onUserDisconnected(const QString& author)
{
    appendSystem(QString("<b>%1</b> s'est déconnecté.").arg(author.toHtmlEscaped()));
    removeContact(author);
}

void MainWindow::appendSystem(const QString& html)
{
    m_chat->append(QString(
        "<div style='color:#3b4b66; margin:6px 0; font-size:10pt;'>%1</div>"
    ).arg(html));
}

void MainWindow::appendChatLine(const QString& author, const QString& content)
{
    const bool isSelf = author.trimmed().compare(m_client->pseudo().trimmed(), Qt::CaseInsensitive) == 0;
    const QString a = author.toHtmlEscaped();
    const QString c = content.toHtmlEscaped();
    const QString t = QDateTime::currentDateTime().toString("HH:mm");

    const QString align = isSelf ? "right" : "left";
    const QString bubbleBg = isSelf ? "#d6f5d6" : "#ffffff";
    const QString bubbleBorder = isSelf ? "#a6e3a6" : "#d6ddf0";

    m_chat->append(QString(
        "<div style='text-align:%1; margin:10px 0;'>"
        "  <div style='display:inline-block; max-width:70%%; padding:10px 12px;"
        "              border:1px solid %2; border-radius:14px; background:%3;'>"
        "    <div style='font-size:9.5pt; color:#5a6b88; margin-bottom:4px;'>"
        "      <b>%4</b> <span style='color:#8aa0c4'>&nbsp;•&nbsp;%5</span>"
        "    </div>"
        "    <div style='font-size:11pt; color:#10223a; white-space:pre-wrap;'>%6</div>"
        "  </div>"
        "</div>"
    ).arg(align, bubbleBorder, bubbleBg, a, t.toHtmlEscaped(), c));
}

void MainWindow::showWizzEffect(const QString& author)
{
    QApplication::beep();
    QApplication::alert(this, 0);

    appendSystem(QString(
        "<div style='border:2px solid #e39; padding:6px; background:#fff3fa'>"
        "<span style='color:#b05'><b>W I Z Z</b></span> de <b>%1</b>"
        "</div>"
    ).arg(author.toHtmlEscaped()));

    if (isMinimized())
        showNormal();
    raise();
    activateWindow();

    startFlash();
    startShake();

    // Toast stylée (auto-détruite) près du haut de la fenêtre
    auto* toast = new QFrame(nullptr, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    toast->setAttribute(Qt::WA_DeleteOnClose, true);
    toast->setAttribute(Qt::WA_TranslucentBackground, true);

    auto* card = new QFrame(toast);
    card->setObjectName("toastCard");
    card->setStyleSheet(
        "QFrame#toastCard {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #fff7fb, stop:1 #ffe3f2);"
        "  border: 1px solid #ff7fbf;"
        "  border-radius: 14px;"
        "}"
        "QLabel { color: #5a1240; font-family: Segoe UI; }"
    );

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 90));
    card->setGraphicsEffect(shadow);

    auto* title = new QLabel("W I Z Z !", card);
    title->setStyleSheet("font-weight: 800; font-size: 14pt; letter-spacing: 2px;");
    auto* msg = new QLabel(QString("de %1").arg(author.toHtmlEscaped()), card);
    msg->setStyleSheet("font-weight: 600; font-size: 11pt;");

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 12, 14, 12);
    lay->addWidget(title);
    lay->addWidget(msg);

    auto* outer = new QVBoxLayout(toast);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(card);

    toast->adjustSize();
    const QRect g = frameGeometry();
    const int x = g.center().x() - toast->width() / 2;
    const int y = g.top() + 18;
    toast->move(x, y);
    toast->show();

    QTimer::singleShot(1800, toast, &QWidget::close);
}

void MainWindow::setUiConnected(bool connected)
{
    m_status->setText(connected ? "Connected" : "Disconnected");
    m_connectBtn->setText(connected ? "Disconnect" : "Connect");

    m_host->setEnabled(!connected);
    m_port->setEnabled(!connected);
    m_pseudo->setEnabled(!connected);

    m_sendBtn->setEnabled(connected);
    m_wizzBtn->setEnabled(connected);
    m_input->setEnabled(connected);
}

void MainWindow::startFlash()
{
    // Petit flash (opacité) + changement de titre temporaire
    const QString originalTitle = windowTitle();
    setWindowTitle("WizzMania — WIZZ!");

    int step = 0;
    auto* timer = new QTimer(this);
    timer->setInterval(65);

    connect(timer, &QTimer::timeout, this, [this, timer, originalTitle, step]() mutable {
        const bool on = (step % 2 == 0);
        setWindowOpacity(on ? 0.86 : 1.0);
        step++;
        if (step >= 10)
        {
            timer->stop();
            setWindowOpacity(1.0);
            setWindowTitle(originalTitle);
            timer->deleteLater();
        }
    });

    timer->start();
}

void MainWindow::upsertContact(const QString& name, bool isSelf)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        return;

    for (int i = 0; i < m_contacts->count(); ++i)
    {
        auto* item = m_contacts->item(i);
        if (!item)
            continue;
        const QString base = item->data(Qt::UserRole).toString();
        if (base.compare(trimmed, Qt::CaseInsensitive) == 0)
            return;
    }

    const QString label = isSelf ? QString("%1 (toi)").arg(trimmed) : trimmed;
    auto* item = new QListWidgetItem(label, m_contacts);
    item->setData(Qt::UserRole, trimmed);
    item->setIcon(makeAvatarIcon(trimmed, isSelf));

    if (isSelf)
        m_contacts->insertItem(0, item);
    else
        m_contacts->addItem(item);
}

void MainWindow::removeContact(const QString& name)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        return;

    for (int i = 0; i < m_contacts->count(); ++i)
    {
        auto* item = m_contacts->item(i);
        if (!item)
            continue;
        const QString base = item->data(Qt::UserRole).toString();
        if (base.compare(trimmed, Qt::CaseInsensitive) == 0)
        {
            delete m_contacts->takeItem(i);
            return;
        }
    }
}

QIcon MainWindow::makeAvatarIcon(const QString& name, bool isSelf) const
{
    const int size = 36;
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);

    const QString trimmed = name.trimmed();
    const QString initial = trimmed.isEmpty() ? "?" : trimmed.left(1).toUpper();

    // Couleur stable basée sur le nom
    const uint h = qHash(trimmed);
    const int hue = static_cast<int>(h % 360);
    QColor base = QColor::fromHsv(hue, 110, 235);
    if (isSelf)
        base = QColor::fromRgb(80, 200, 120);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(base);
    p.drawEllipse(0, 0, size, size);

    // petit reflet
    QColor shine = QColor(255, 255, 255, 70);
    p.setBrush(shine);
    p.drawEllipse(4, 3, size - 10, size - 14);

    p.setPen(QColor(255, 255, 255));
    QFont f("Segoe UI");
    f.setBold(true);
    f.setPointSize(14);
    p.setFont(f);
    p.drawText(QRect(0, 0, size, size), Qt::AlignCenter, initial);

    p.end();
    return QIcon(pm);
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

