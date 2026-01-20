#include "commun/SocketTCP.hpp"
#include <iostream>

int SocketTCP::s_compteurWinsock = 0;

static bool initialisationWinsockGlobale()
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
}

SocketTCP::SocketTCP()
    : m_socket(INVALID_SOCKET)
{
    if (s_compteurWinsock == 0)
    {
        if (!initialisationWinsockGlobale())
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

bool SocketTCP::creer()
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return m_socket != INVALID_SOCKET;
}

bool SocketTCP::connecter(const std::string& ip, int port)
{
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(port);
    adresse.sin_addr.s_addr = inet_addr(ip.c_str());

    return connect(m_socket, (sockaddr*)&adresse, sizeof(adresse)) != SOCKET_ERROR;
}

bool SocketTCP::lier(int port)
{
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(port);

    return bind(m_socket, (sockaddr*)&adresse, sizeof(adresse)) != SOCKET_ERROR;
}

bool SocketTCP::ecouter()
{
    return listen(m_socket, SOMAXCONN) != SOCKET_ERROR;
}

SocketTCP SocketTCP::accepter()
{
    SocketTCP client;
    client.m_socket = accept(m_socket, nullptr, nullptr);
    return client;
}

bool SocketTCP::envoyer(const std::string& message)
{
    return send(m_socket, message.c_str(),
                static_cast<int>(message.size()), 0) != SOCKET_ERROR;
}

std::string SocketTCP::recevoir()
{
    char buffer[512];
    int bytes = recv(m_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
        return "";

    buffer[bytes] = '\0';
    return std::string(buffer);
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
