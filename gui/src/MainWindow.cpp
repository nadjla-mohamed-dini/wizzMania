#include "MainWindow.hpp"

#include <QApplication>
#include <QDateTime>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QListWidget>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QTimer>

static constexpr const char* kGlobalKey = "__global__";

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
        "QLabel#title { color:#123; font-weight:800; font-size:18pt; }"
        "QLabel#subtitle { color:#345; font-weight:600; }"
        "QLabel#loginStatus { color:#234; }"
    );

    auto* root = new QVBoxLayout(central);
    m_pages = new QStackedWidget(central);
    root->addWidget(m_pages, 1);

    // ---------- Page LOGIN ----------
    m_pageLogin = new QWidget(central);
    auto* loginRoot = new QVBoxLayout(m_pageLogin);
    loginRoot->setContentsMargins(24, 24, 24, 24);
    loginRoot->setSpacing(14);

    auto* header = new QVBoxLayout();
    auto* title = new QLabel("WizzMania", m_pageLogin);
    title->setObjectName("title");
    auto* subtitle = new QLabel("Connexion — crée un compte ou connecte-toi.", m_pageLogin);
    subtitle->setObjectName("subtitle");
    header->addWidget(title);
    header->addWidget(subtitle);
    loginRoot->addLayout(header);

    auto* rowHost = new QHBoxLayout();
    m_host = new QLineEdit("127.0.0.1", m_pageLogin);
    m_port = new QSpinBox(m_pageLogin);
    m_port->setRange(1, 65535);
    m_port->setValue(4242);
    rowHost->addWidget(new QLabel("Host:", m_pageLogin));
    rowHost->addWidget(m_host, 1);
    rowHost->addWidget(new QLabel("Port:", m_pageLogin));
    rowHost->addWidget(m_port);
    loginRoot->addLayout(rowHost);

    m_login = new QLineEdit(m_pageLogin);
    m_login->setPlaceholderText("Login");
    m_password = new QLineEdit(m_pageLogin);
    m_password->setPlaceholderText("Password");
    m_password->setEchoMode(QLineEdit::Password);
    loginRoot->addWidget(m_login);
    loginRoot->addWidget(m_password);

    auto* rowBtns = new QHBoxLayout();
    m_btnRegister = new QPushButton("Créer un compte", m_pageLogin);
    m_btnLogin = new QPushButton("Se connecter", m_pageLogin);
    rowBtns->addWidget(m_btnRegister);
    rowBtns->addWidget(m_btnLogin);
    loginRoot->addLayout(rowBtns);

    m_loginStatus = new QLabel("", m_pageLogin);
    m_loginStatus->setObjectName("loginStatus");
    m_loginStatus->setWordWrap(true);
    loginRoot->addWidget(m_loginStatus);
    loginRoot->addStretch(1);

    m_pages->addWidget(m_pageLogin);

    // ---------- Page MESSENGER ----------
    m_pageMessenger = new QWidget(central);
    auto* chatRoot = new QVBoxLayout(m_pageMessenger);

    auto* top = new QHBoxLayout();
    m_me = new QLabel("Non connecté", m_pageMessenger);
    m_me->setStyleSheet("color:#123; font-weight:800; font-size:12pt;");
    m_status = new QLabel("Disconnected", m_pageMessenger);
    m_status->setStyleSheet("color:#345; font-weight:600;");
    m_disconnectBtn = new QPushButton("Disconnect", m_pageMessenger);
    m_disconnectBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    top->addWidget(m_me, 1);
    top->addWidget(m_status);
    top->addWidget(m_disconnectBtn);
    chatRoot->addLayout(top);

    auto* middle = new QHBoxLayout();
    m_chat = new QTextBrowser(m_pageMessenger);
    m_chat->setOpenExternalLinks(false);
    m_chat->setStyleSheet("font-family: Segoe UI; font-size: 11pt;");

    m_contacts = new QListWidget(m_pageMessenger);
    m_contacts->setFixedWidth(220);
    m_contacts->setStyleSheet("font-family: Segoe UI; font-size: 10.5pt;");
    m_contacts->setSelectionMode(QAbstractItemView::SingleSelection);

    middle->addWidget(m_chat, 1);
    middle->addWidget(m_contacts);
    chatRoot->addLayout(middle, 1);

    auto* bottom = new QHBoxLayout();
    m_input = new QLineEdit(m_pageMessenger);
    m_input->setPlaceholderText("Écris ton message…");
    m_emojiBtn = new QPushButton(QString::fromUtf8(u8"🙂"), m_pageMessenger);
    m_emojiBtn->setToolTip("Emojis");
    m_emojiBtn->setFixedWidth(44);
    m_emojiBtn->setStyleSheet("font-size: 14pt; padding: 2px 6px;");
    m_sendBtn = new QPushButton("Send", m_pageMessenger);
    m_sendBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    m_wizzBtn = new QPushButton("Wizz", m_pageMessenger);
    m_wizzBtn->setObjectName("wizzBtn");
    m_wizzBtn->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    bottom->addWidget(m_emojiBtn);
    bottom->addWidget(m_input, 1);
    bottom->addWidget(m_sendBtn);
    bottom->addWidget(m_wizzBtn);
    chatRoot->addLayout(bottom);

    m_pages->addWidget(m_pageMessenger);

    setCentralWidget(central);
    setWindowTitle("WizzMania");
    resize(820, 560);
    connect(m_btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(m_btnRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(m_password, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);

    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_wizzBtn, &QPushButton::clicked, this, &MainWindow::onWizzClicked);
    connect(m_emojiBtn, &QPushButton::clicked, this, &MainWindow::onEmojiClicked);
    connect(m_input, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);

    connect(m_client, &ChatClient::connected, this, &MainWindow::onConnected);
    connect(m_client, &ChatClient::disconnected, this, &MainWindow::onDisconnected);
    connect(m_client, &ChatClient::errorOccurred, this, &MainWindow::onError);
    connect(m_client, &ChatClient::authOk, this, &MainWindow::onAuthOk);
    connect(m_client, &ChatClient::authFail, this, &MainWindow::onAuthFail);
    connect(m_client, &ChatClient::contactsReceived, this, &MainWindow::onContacts);
    connect(m_client, &ChatClient::privateReceived, this, &MainWindow::onPrivate);
    connect(m_client, &ChatClient::messageReceived, this, &MainWindow::onMessage);
    connect(m_client, &ChatClient::wizzReceived, this, &MainWindow::onWizz);
    connect(m_client, &ChatClient::userConnected, this, &MainWindow::onUserConnected);
    connect(m_client, &ChatClient::userDisconnected, this, &MainWindow::onUserDisconnected);
    connect(m_contacts, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (!item)
            return;
        const QString key = item->data(Qt::UserRole).toString();
        if (key == kGlobalKey)
            m_currentTarget.clear();
        else
            m_currentTarget = key;

        // clear unread
        if (!m_currentTarget.isEmpty())
            m_unread[m_currentTarget] = 0;
        redrawConversation();
    });

    setPageLogin();
    setUiConnected(false);
}

void MainWindow::onLoginClicked()
{
    const QString user = m_login->text().trimmed();
    const QString pass = m_password->text();
    if (user.isEmpty() || pass.isEmpty())
    {
        m_loginStatus->setText("Login et password requis.");
        return;
    }

    m_pending = PendingAuth::Login;
    m_pendingUser = user;
    m_pendingPass = pass;
    m_loginStatus->setText("Connexion au serveur…");
    m_client->connectToServer(m_host->text(), static_cast<quint16>(m_port->value()));
}

void MainWindow::onRegisterClicked()
{
    const QString user = m_login->text().trimmed();
    const QString pass = m_password->text();
    if (user.isEmpty() || pass.isEmpty())
    {
        m_loginStatus->setText("Login et password requis.");
        return;
    }

    m_pending = PendingAuth::Register;
    m_pendingUser = user;
    m_pendingPass = pass;
    m_loginStatus->setText("Connexion au serveur…");
    m_client->connectToServer(m_host->text(), static_cast<quint16>(m_port->value()));
}

void MainWindow::onDisconnectClicked()
{
    appendSystem("<i>Déconnexion…</i>");
    m_client->disconnectFromServer();
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

    const QString t = QDateTime::currentDateTime().toString("HH:mm");
    if (m_currentTarget.isEmpty())
    {
        m_client->sendMessage(text);
        m_history[kGlobalKey].push_back(ChatEntry{m_client->pseudo(), text, t});
        appendChatLine(m_client->pseudo(), text);
    }
    else
    {
        m_client->sendPrivate(m_currentTarget, text);
        m_history[m_currentTarget].push_back(ChatEntry{m_client->pseudo(), text, t});
        appendChatLine(m_client->pseudo(), text);
    }
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

void MainWindow::onEmojiClicked()
{
    if (!m_input)
        return;

    QMenu menu(this);
    menu.setStyleSheet("QMenu { font-size: 14pt; }");

    const QStringList emojis = {
        QString::fromUtf8(u8"😀"), QString::fromUtf8(u8"😁"), QString::fromUtf8(u8"😂"),
        QString::fromUtf8(u8"🤣"), QString::fromUtf8(u8"😊"), QString::fromUtf8(u8"😍"),
        QString::fromUtf8(u8"😘"), QString::fromUtf8(u8"😎"), QString::fromUtf8(u8"🤔"),
        QString::fromUtf8(u8"😅"), QString::fromUtf8(u8"😭"), QString::fromUtf8(u8"😡"),
        QString::fromUtf8(u8"👍"), QString::fromUtf8(u8"👎"), QString::fromUtf8(u8"🙏"),
        QString::fromUtf8(u8"👏"), QString::fromUtf8(u8"🔥"), QString::fromUtf8(u8"💯"),
        QString::fromUtf8(u8"❤️"), QString::fromUtf8(u8"💙"), QString::fromUtf8(u8"✨"),
        QString::fromUtf8(u8"🎉"), QString::fromUtf8(u8"😴"), QString::fromUtf8(u8"🙃")
    };

    // Display as a compact grid-ish menu: 8 per row using separators
    int col = 0;
    for (const QString& e : emojis)
    {
        QAction* a = menu.addAction(e);
        connect(a, &QAction::triggered, this, [this, e]() {
            m_input->insert(e);
            m_input->setFocus();
        });

        col++;
        if (col % 8 == 0)
            menu.addSeparator();
    }

    const QPoint pos = m_emojiBtn ? m_emojiBtn->mapToGlobal(QPoint(0, m_emojiBtn->height())) : QCursor::pos();
    menu.exec(pos);
}

void MainWindow::onConnected()
{
    // TCP connected: send pending auth action now.
    if (m_pending == PendingAuth::Login)
    {
        m_loginStatus->setText("Authentification…");
        m_client->sendLogin(m_pendingUser, m_pendingPass);
    }
    else if (m_pending == PendingAuth::Register)
    {
        m_loginStatus->setText("Création du compte…");
        m_client->sendRegister(m_pendingUser, m_pendingPass);
    }
}

void MainWindow::onDisconnected()
{
    appendSystem("<span style='color:#666'><b>Déconnecté.</b></span>");
    setUiConnected(false);
    m_contacts->clear();
    setPageLogin();
}

void MainWindow::onError(const QString& message)
{
    const QString msg = QString("<span style='color:#c00'><b>Erreur:</b> %1</span>")
                            .arg(message.toHtmlEscaped());
    appendSystem(msg);
    m_loginStatus->setText(message);
    setUiConnected(false);
    setPageLogin();
}

void MainWindow::onAuthOk(const QString& info)
{
    if (m_pending == PendingAuth::Register)
    {
        m_loginStatus->setText("Compte créé. Tu peux te connecter.");
        m_pending = PendingAuth::None;
        // stay on login page
        return;
    }

    if (m_pending == PendingAuth::Login)
    {
        m_pending = PendingAuth::None;
        m_loginStatus->setText("");
        setPageMessenger();

        m_me->setText(QString("Connecté: %1").arg(m_client->pseudo().toHtmlEscaped()));
        appendSystem(QString("<span style='color:#090'><b>AUTH OK</b></span> %1").arg(info.toHtmlEscaped()));
        setUiConnected(true);
        m_contacts->clear();
        // Global chat entry
        {
            auto* global = new QListWidgetItem("Salon (global)", m_contacts);
            global->setData(Qt::UserRole, QString::fromUtf8(kGlobalKey));
            global->setIcon(style()->standardIcon(QStyle::SP_DesktopIcon));
            m_contacts->addItem(global);
            m_contacts->setCurrentItem(global);
        }

        upsertContact(m_client->pseudo(), true);
        m_currentTarget.clear();
        redrawConversation();
        return;
    }
}

void MainWindow::onAuthFail(const QString& reason)
{
    m_pending = PendingAuth::None;
    m_loginStatus->setText(QString("Échec: %1").arg(reason));
}

void MainWindow::onContacts(const QString& payload)
{
    // Format: "name=1;other=0;..."
    const QStringList parts = payload.split(';', Qt::SkipEmptyParts);
    for (const QString& p : parts)
    {
        const int eq = p.indexOf('=');
        if (eq <= 0)
            continue;
        const QString name = p.left(eq).trimmed();
        const QString val = p.mid(eq + 1).trimmed();
        const bool online = (val == "1");
        if (name.isEmpty())
            continue;
        if (name.compare(m_client->pseudo(), Qt::CaseInsensitive) == 0)
            continue;
        upsertContact(name, false);
        setContactOnline(name, online);
    }
}

void MainWindow::onPrivate(const QString& from, const QString& to, const QString& content)
{
    Q_UNUSED(to);
    const QString t = QDateTime::currentDateTime().toString("HH:mm");
    const QString key = from;
    m_history[key].push_back(ChatEntry{from, content, t});

    // Notification sonore: 1 beep pour un message privé entrant
    QApplication::beep();

    if (m_currentTarget.compare(key, Qt::CaseInsensitive) != 0)
    {
        m_unread[key] = m_unread.value(key, 0) + 1;
    }
    else
    {
        appendChatLine(from, content);
    }

    upsertContact(key, false);
}

void MainWindow::onMessage(const QString& author, const QString& content)
{
    const QString t = QDateTime::currentDateTime().toString("HH:mm");
    m_history[kGlobalKey].push_back(ChatEntry{author, content, t});
    if (m_currentTarget.isEmpty())
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
    const QString bubbleBg = isSelf ? "#c9f7c7" : "#eaf2ff";
    const QString bubbleBorder = isSelf ? "#6ad66a" : "#9dbaf0";
    const QString shadow = isSelf ? "rgba(106,214,106,0.20)" : "rgba(77,123,224,0.18)";

    m_chat->append(QString(
        "<div style='text-align:%1; margin:10px 0;'>"
        "  <div style='display:inline-block; max-width:72%%; padding:10px 12px;"
        "              border:1px solid %2; border-radius:14px; background:%3;"
        "              box-shadow: 0 6px 16px %4;'>"
        "    <div style='font-size:9.5pt; color:#5a6b88; margin-bottom:4px;'>"
        "      <b>%5</b> <span style='color:#8aa0c4'>&nbsp;•&nbsp;%6</span>"
        "    </div>"
        "    <div style='font-size:11pt; color:#10223a; white-space:pre-wrap;'>%7</div>"
        "  </div>"
        "</div>"
    ).arg(align, bubbleBorder, bubbleBg, shadow, a, t.toHtmlEscaped(), c));
}

void MainWindow::redrawConversation()
{
    m_chat->clear();

    const QString key = m_currentTarget.isEmpty() ? QString::fromUtf8(kGlobalKey) : m_currentTarget;
    const auto items = m_history.value(key);

    if (m_currentTarget.isEmpty())
        appendSystem("<i>Salon global</i>");
    else
        appendSystem(QString("<i>Conversation avec <b>%1</b></i>").arg(m_currentTarget.toHtmlEscaped()));

    for (const auto& e : items)
    {
        // render using bubble HTML but keep time from stored entry
        const bool isSelf = e.author.trimmed().compare(m_client->pseudo().trimmed(), Qt::CaseInsensitive) == 0;
        const QString a = e.author.toHtmlEscaped();
        const QString c = e.content.toHtmlEscaped();
        const QString time = e.time.toHtmlEscaped();

        const QString align = isSelf ? "right" : "left";
        const QString bubbleBg = isSelf ? "#c9f7c7" : "#eaf2ff";
        const QString bubbleBorder = isSelf ? "#6ad66a" : "#9dbaf0";
        const QString shadow = isSelf ? "rgba(106,214,106,0.20)" : "rgba(77,123,224,0.18)";

        m_chat->append(QString(
            "<div style='text-align:%1; margin:10px 0;'>"
            "  <div style='display:inline-block; max-width:72%%; padding:10px 12px;"
            "              border:1px solid %2; border-radius:14px; background:%3;"
            "              box-shadow: 0 6px 16px %4;'>"
            "    <div style='font-size:9.5pt; color:#5a6b88; margin-bottom:4px;'>"
            "      <b>%5</b> <span style='color:#8aa0c4'>&nbsp;•&nbsp;%6</span>"
            "    </div>"
            "    <div style='font-size:11pt; color:#10223a; white-space:pre-wrap;'>%7</div>"
            "  </div>"
            "</div>"
        ).arg(align, bubbleBorder, bubbleBg, shadow, a, time, c));
    }

    // update unread badges in list
    for (int i = 0; i < m_contacts->count(); ++i)
    {
        auto* it = m_contacts->item(i);
        if (!it) continue;
        const QString base = it->data(Qt::UserRole).toString();
        if (base == kGlobalKey) continue;
        const int u = m_unread.value(base, 0);
        const QString rawName = base;
        const QString label = (u > 0) ? QString("%1 (%2)").arg(rawName).arg(u) : rawName;
        if (it->text() != label && !it->text().startsWith(rawName + " (toi)"))
            it->setText(label);
    }
}

void MainWindow::showWizzEffect(const QString& author)
{
    // Notification sonore: 2 beeps pour un Wizz
    QApplication::beep();
    QTimer::singleShot(140, [] { QApplication::beep(); });
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
    m_disconnectBtn->setEnabled(connected);
    m_sendBtn->setEnabled(connected);
    m_wizzBtn->setEnabled(connected);
    m_input->setEnabled(connected);
}

void MainWindow::setPageLogin()
{
    m_pages->setCurrentWidget(m_pageLogin);
    m_disconnectBtn->setEnabled(false);
    setUiConnected(false);
}

void MainWindow::setPageMessenger()
{
    m_pages->setCurrentWidget(m_pageMessenger);
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
    item->setIcon(makeAvatarIcon(trimmed, isSelf, true));

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

void MainWindow::setContactOnline(const QString& name, bool online)
{
    const QString trimmed = name.trimmed();
    for (int i = 0; i < m_contacts->count(); ++i)
    {
        auto* item = m_contacts->item(i);
        if (!item) continue;
        const QString base = item->data(Qt::UserRole).toString();
        if (base.compare(trimmed, Qt::CaseInsensitive) != 0)
            continue;
        item->setIcon(makeAvatarIcon(trimmed, false, online));
        item->setData(Qt::UserRole + 1, online);
        return;
    }
}

QIcon MainWindow::makeAvatarIcon(const QString& name, bool isSelf, bool online) const
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

    // Online/offline dot
    QPainter d(&pm);
    d.setRenderHint(QPainter::Antialiasing, true);
    const int dot = 10;
    const int pad = 2;
    const QRect r(size - dot - pad, size - dot - pad, dot, dot);
    d.setPen(QPen(QColor(255, 255, 255), 2));
    d.setBrush(online ? QColor(0, 200, 80) : QColor(220, 60, 60));
    d.drawEllipse(r);
    d.end();
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

