#include "serveur/ServeurTCP.hpp"
#include "serveur/ClientSession.hpp"
#include <iostream>

ServeurTCP::ServeurTCP(int port)
    : m_port(port)
{
}

bool ServeurTCP::demarrer()
{
    if (!m_socketEcoute.creer())
    {
        std::cerr << "Erreur : création du socket serveur\n";
        return false;
    }

    if (!m_socketEcoute.lier(m_port))
    {
        std::cerr << "Erreur : bind serveur\n";
        return false;
    }

    if (!m_socketEcoute.ecouter())
    {
        std::cerr << "Erreur : listen serveur\n";
        return false;
    }

    return true;
}

void ServeurTCP::attendreClient()
{
    std::cout << "Serveur en attente de clients...\n";

    while (true)
    {
        SocketTCP client = m_socketEcoute.accepter();

        if (!client.estValide())
        {
            std::cerr << "Erreur : accept client\n";
            continue;
        }

        auto session = std::make_shared<ClientSession>(std::move(client), m_gestionnaire);
        m_gestionnaire.ajouter(session);
        session->demarrer();
    }
}


