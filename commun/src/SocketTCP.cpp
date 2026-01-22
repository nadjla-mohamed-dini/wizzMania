#include "commun/SocketTCP.hpp"
#include <iostream>
#include <utility>
#include <Ws2tcpip.h>

int SocketTCP::s_compteurWinsock = 0;

bool SocketTCP::initialiserWinsock()
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
}

SocketTCP::SocketTCP()
    : m_socket(INVALID_SOCKET)
{
    if (s_compteurWinsock == 0)
    {
        if (!initialiserWinsock())
        {
            std::cerr << "Erreur : initialisation Winsock échouée\n";
        }
    }
    ++s_compteurWinsock;
}

SocketTCP::~SocketTCP()
{
    fermer();
    --s_compteurWinsock;

    if (s_compteurWinsock == 0)
        WSACleanup();
}

SocketTCP::SocketTCP(SocketTCP&& other) noexcept
    : m_socket(other.m_socket)
{
    m_tamponReception = std::move(other.m_tamponReception);
    other.m_socket = INVALID_SOCKET;
    ++s_compteurWinsock;
}

SocketTCP& SocketTCP::operator=(SocketTCP&& other) noexcept
{
    if (this == &other)
        return *this;

    fermer();
    m_socket = other.m_socket;
    m_tamponReception = std::move(other.m_tamponReception);
    other.m_socket = INVALID_SOCKET;
    return *this;
}

bool SocketTCP::creer()
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return m_socket != INVALID_SOCKET;
}

bool SocketTCP::connecter(const std::string& ip, int port)
{
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(static_cast<u_short>(port));
    if (InetPtonA(AF_INET, ip.c_str(), &adresse.sin_addr) != 1)
        return false;

    return connect(m_socket, (sockaddr*)&adresse, sizeof(adresse)) != SOCKET_ERROR;
}

bool SocketTCP::lier(int port)
{
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(static_cast<u_short>(port));

    return bind(m_socket, (sockaddr*)&adresse, sizeof(adresse)) != SOCKET_ERROR;
}

bool SocketTCP::ecouter()
{
    return listen(m_socket, SOMAXCONN) != SOCKET_ERROR;
}

SocketTCP SocketTCP::accepter()
{
    SocketTCP client;
    SOCKET s = accept(m_socket, nullptr, nullptr);
    if (s == INVALID_SOCKET)
        return client;

    client.m_socket = s;
    return client;
}

bool SocketTCP::envoyer(const std::string& message)
{
    // Encodage simple : 1 message = 1 ligne terminée par '\n'
    std::string trame = message;
    trame.push_back('\n');
    return send(m_socket, trame.c_str(),
                static_cast<int>(trame.size()), 0) != SOCKET_ERROR;
}

// 
bool SocketTCP::accepter(SocketTCP& client)
{
    SocketTCP tmp = accepter();
    if (!tmp.estValide())
        return false;
    client = std::move(tmp);
    return client.estValide();
}
// 


std::string SocketTCP::recevoir()
{
    // Réception "ligne par ligne" (TCP = flux, donc on recompose les messages).
    while (true)
    {
        size_t pos = m_tamponReception.find('\n');
        if (pos != std::string::npos)
        {
            std::string ligne = m_tamponReception.substr(0, pos);
            m_tamponReception.erase(0, pos + 1);
            return ligne;
        }

        char buffer[512];
        int bytes = recv(m_socket, buffer, static_cast<int>(sizeof(buffer)), 0);

        if (bytes <= 0)
            return "";

        m_tamponReception.append(buffer, buffer + bytes);
    }
}

bool SocketTCP::estValide() const
{
    return m_socket != INVALID_SOCKET;
}

void SocketTCP::fermer()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}
