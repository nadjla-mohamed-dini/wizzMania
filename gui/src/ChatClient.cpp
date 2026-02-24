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

void ChatClient::connectToServer(const QString& host, quint16 port, const QString& pseudo)
{
    m_pseudo = pseudo;

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

    Message deco(Message::Type::DECONNEXION, m_pseudo.toStdString());
    sendMessageObject(deco);
    m_socket->disconnectFromHost();
}

void ChatClient::sendMessage(const QString& text)
{
    if (!isConnected())
        return;

    Message msg(Message::Type::MESSAGE, m_pseudo.toStdString(), text.toStdString());
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
    Message co(Message::Type::CONNEXION, m_pseudo.toStdString());
    sendMessageObject(co);
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

