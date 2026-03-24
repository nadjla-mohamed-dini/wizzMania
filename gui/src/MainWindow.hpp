#pragma once

#include "ChatClient.hpp"
#include <QMainWindow>

class QTextBrowser;
class QLineEdit;
class QPushButton;
class QLabel;
class QSpinBox;
class QListWidget;
class QStackedWidget;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onDisconnectClicked();
    void onSendClicked();
    void onWizzClicked();

    void onConnected();
    void onDisconnected();
    void onError(const QString& message);
    void onAuthOk(const QString& info);
    void onAuthFail(const QString& reason);
    void onContacts(const QString& payload);
    void onPrivate(const QString& from, const QString& to, const QString& content);

    void onMessage(const QString& author, const QString& content);
    void onWizz(const QString& author);
    void onUserConnected(const QString& author);
    void onUserDisconnected(const QString& author);

private:
    void appendSystem(const QString& html);
    void appendChatLine(const QString& author, const QString& content);
    void redrawConversation();
    void showWizzEffect(const QString& author);
    void setUiConnected(bool connected);
    void startShake();
    void startFlash();
    void upsertContact(const QString& name, bool isSelf);
    void removeContact(const QString& name);
    void setContactOnline(const QString& name, bool online);
    QIcon makeAvatarIcon(const QString& name, bool isSelf, bool online) const;
    void setPageLogin();
    void setPageMessenger();

    ChatClient* m_client = nullptr;

    QStackedWidget* m_pages = nullptr;
    QWidget* m_pageLogin = nullptr;
    QWidget* m_pageMessenger = nullptr;

    // Login page
    QLineEdit* m_host = nullptr;
    QSpinBox* m_port = nullptr;
    QLineEdit* m_login = nullptr;
    QLineEdit* m_password = nullptr;
    QLabel* m_loginStatus = nullptr;
    QPushButton* m_btnLogin = nullptr;
    QPushButton* m_btnRegister = nullptr;

    enum class PendingAuth { None, Login, Register };
    PendingAuth m_pending = PendingAuth::None;
    QString m_pendingUser;
    QString m_pendingPass;

    // Messenger page
    QTextBrowser* m_chat = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_me = nullptr;
    QPushButton* m_disconnectBtn = nullptr;
    QListWidget* m_contacts = nullptr;

    QLineEdit* m_input = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_wizzBtn = nullptr;

    QPoint m_shakeOrigin;
    int m_shakeStep = 0;

    // Conversations
    struct ChatEntry
    {
        QString author;
        QString content;
        QString time;
    };
    QString m_currentTarget; // empty = global
    QHash<QString, QVector<ChatEntry>> m_history; // key = contact or "__global__"
    QHash<QString, int> m_unread; // key = contact
};

