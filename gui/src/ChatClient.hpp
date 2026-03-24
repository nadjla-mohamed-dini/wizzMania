#pragma once

#include "commun/Message.hpp"
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class ChatClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();

    // Auth
    void sendRegister(const QString& username, const QString& password);
    void sendLogin(const QString& username, const QString& password);
    void sendGuest(const QString& pseudo);

    void sendMessage(const QString& text);
    void sendPrivate(const QString& to, const QString& text);
    void sendWizz();

    bool isConnected() const;
    QString pseudo() const;

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& message);

    void authOk(const QString& info);
    void authFail(const QString& reason);
    void contactsReceived(const QString& payload);
    void privateReceived(const QString& from, const QString& to, const QString& content);

    void messageReceived(const QString& author, const QString& content);
    void wizzReceived(const QString& author);
    void userConnected(const QString& author);
    void userDisconnected(const QString& author);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    void sendMessageObject(const Message& msg);
    void handleLine(const QByteArray& line);

    QTcpSocket* m_socket = nullptr;
    QByteArray m_buffer;
    QString m_pseudo;
};

