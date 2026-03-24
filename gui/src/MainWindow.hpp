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
    void showWizzEffect(const QString& author);
    void setUiConnected(bool connected);
    void startShake();
    void startFlash();
    void upsertContact(const QString& name, bool isSelf);
    void removeContact(const QString& name);
    QIcon makeAvatarIcon(const QString& name, bool isSelf) const;
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
};

