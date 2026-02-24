#pragma once

#include "ChatClient.hpp"
#include <QMainWindow>

class QTextBrowser;
class QLineEdit;
class QPushButton;
class QLabel;
class QSpinBox;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onConnectClicked();
    void onSendClicked();
    void onWizzClicked();

    void onConnected();
    void onDisconnected();
    void onError(const QString& message);

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

    ChatClient* m_client = nullptr;

    QTextBrowser* m_chat = nullptr;
    QLineEdit* m_host = nullptr;
    QSpinBox* m_port = nullptr;
    QLineEdit* m_pseudo = nullptr;
    QLabel* m_status = nullptr;
    QPushButton* m_connectBtn = nullptr;
    QListWidget* m_contacts = nullptr;

    QLineEdit* m_input = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_wizzBtn = nullptr;

    QPoint m_shakeOrigin;
    int m_shakeStep = 0;
};

