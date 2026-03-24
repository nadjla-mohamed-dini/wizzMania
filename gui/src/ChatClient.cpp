#include "ChatClient.hpp"
#include <QString>

ChatClient::ChatClient(QObject* parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &ChatClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ChatClient::onError);
}

void ChatClient::connectToServer(const QString& host, quint16 port)
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        m_socket->abort();

    m_buffer.clear();
    m_socket->connectToHost(host, port);
}

void ChatClient::disconnectFromServer()
{
    if (!isConnected())
    {
        m_socket->disconnectFromHost();
        return;
    }

    if (!m_pseudo.trimmed().isEmpty())
        sendMessageObject(Message(Message::Type::DECONNEXION, m_pseudo.toStdString()));
    m_socket->disconnectFromHost();
}

void ChatClient::sendRegister(const QString& username, const QString& password)
{
    if (!isConnected())
        return;
    sendMessageObject(Message(Message::Type::REGISTER,
                              username.toStdString(),
                              password.toStdString()));
}

void ChatClient::sendLogin(const QString& username, const QString& password)
{
    if (!isConnected())
        return;
    // Store pseudo locally for the UI. (Server may adjust duplicates later.)
    m_pseudo = username;
    sendMessageObject(Message(Message::Type::LOGIN,
                              username.toStdString(),
                              password.toStdString()));
}

void ChatClient::sendGuest(const QString& pseudo)
{
    if (!isConnected())
        return;
    m_pseudo = pseudo;
    sendMessageObject(Message(Message::Type::CONNEXION, pseudo.toStdString()));
}

void ChatClient::sendMessage(const QString& text)
{
    if (!isConnected())
        return;

    Message msg(Message::Type::MESSAGE, m_pseudo.toStdString(), text.toStdString());
    sendMessageObject(msg);
}

void ChatClient::sendPrivate(const QString& to, const QString& text)
{
    if (!isConnected())
        return;
    if (to.trimmed().isEmpty())
        return;

    Message msg(Message::Type::PRIVE,
                m_pseudo.toStdString(),
                to.toStdString(),
                text.toStdString());
    sendMessageObject(msg);
}

void ChatClient::sendWizz()
{
    if (!isConnected())
        return;

    Message wizz(Message::Type::WIZZ, m_pseudo.toStdString());
    sendMessageObject(wizz);
}

bool ChatClient::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

QString ChatClient::pseudo() const
{
    return m_pseudo;
}

void ChatClient::onConnected()
{
    emit connected();
}

void ChatClient::onDisconnected()
{
    emit disconnected();
}

void ChatClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (true)
    {
        const int nl = m_buffer.indexOf('\n');
        if (nl < 0)
            break;

        QByteArray line = m_buffer.left(nl);
        m_buffer.remove(0, nl + 1);

        if (!line.isEmpty() && line.endsWith('\r'))
            line.chop(1);

        if (!line.isEmpty())
            handleLine(line);
    }
}

void ChatClient::onError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_socket->errorString());
}

void ChatClient::sendMessageObject(const Message& msg)
{
    const std::string payload = msg.toString();
    QByteArray data(payload.data(), static_cast<int>(payload.size()));
    data.append('\n');
    m_socket->write(data);
}

void ChatClient::handleLine(const QByteArray& line)
{
    const std::string brut = QString::fromUtf8(line).toStdString();
    const Message msg = Message::depuisString(brut);

    switch (msg.getType())
    {
    case Message::Type::AUTH_OK:
        emit authOk(QString::fromStdString(msg.getContenu()));
        break;
    case Message::Type::AUTH_FAIL:
        emit authFail(QString::fromStdString(msg.getContenu()));
        break;
    case Message::Type::CONTACTS:
        emit contactsReceived(QString::fromStdString(msg.getContenu()));
        break;
    case Message::Type::PRIVE:
        emit privateReceived(QString::fromStdString(msg.getAuteur()),
                             QString::fromStdString(msg.getCible()),
                             QString::fromStdString(msg.getContenu()));
        break;
    case Message::Type::MESSAGE:
        emit messageReceived(QString::fromStdString(msg.getAuteur()),
                             QString::fromStdString(msg.getContenu()));
        break;
    case Message::Type::WIZZ:
        emit wizzReceived(QString::fromStdString(msg.getAuteur()));
        break;
    case Message::Type::CONNEXION:
        emit userConnected(QString::fromStdString(msg.getAuteur()));
        break;
    case Message::Type::DECONNEXION:
        emit userDisconnected(QString::fromStdString(msg.getAuteur()));
        break;
    default:
        break;
    }
}

