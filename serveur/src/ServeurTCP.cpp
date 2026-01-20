#include "serveur/ServeurTCP.hpp"
#include <iostream>

ServeurTCP::ServeurTCP(int port)
    : m_port(port), m_socketEcoute(INVALID_SOCKET)
{
}

ServeurTCP::~ServeurTCP()
{
    if (m_socketEcoute != INVALID_SOCKET)
        closesocket(m_socketEcoute);

    WSACleanup();
}

bool ServeurTCP::initialiserWinsock()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cerr << "Erreur : WSAStartup a échoué\n";
        return false;
    }
    return true;
}

bool ServeurTCP::creerSocket()
{
    m_socketEcoute = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socketEcoute == INVALID_SOCKET)
    {
        std::cerr << "Erreur : création du socket\n";
        return false;
    }
    return true;
}

bool ServeurTCP::lierSocket()
{
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(m_port);

    if (bind(m_socketEcoute, (sockaddr*)&adresse, sizeof(adresse)) == SOCKET_ERROR)
    {
        std::cerr << "Erreur : bind\n";
        return false;
    }
    return true;
}

bool ServeurTCP::ecouter()
{
    if (listen(m_socketEcoute, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Erreur : listen\n";
        return false;
    }
    return true;
}

bool ServeurTCP::demarrer()
{
    return initialiserWinsock()
        && creerSocket()
        && lierSocket()
        && ecouter();
}

void ServeurTCP::attendreClient()
{
    std::cout << "Serveur en attente d'un client...\n";

    SOCKET client = accept(m_socketEcoute, nullptr, nullptr);
    if (client == INVALID_SOCKET)
    {
        std::cerr << "Erreur : accept\n";
        return;
    }

    std::cout << "Client connecté !\n";

    char buffer[512];
    int bytesRecus = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (bytesRecus > 0)
    {
        buffer[bytesRecus] = '\0';
        std::cout << "Message reçu : " << buffer << "\n";
    }

    closesocket(client);
}
