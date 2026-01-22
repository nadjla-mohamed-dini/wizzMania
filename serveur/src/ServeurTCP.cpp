#include "serveur/ServeurTCP.hpp"
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
    std::cout << "Serveur en attente d'un client...\n";

    SocketTCP client = m_socketEcoute.accepter();

    if (!client.estValide())
    {
        std::cerr << "Erreur : accept client\n";
        return;
    }

    std::cout << "Client connecté\n";

    while (true)
    {
        std::string brut = client.recevoir();

        if (brut.empty())
        {
            std::cout << "Client déconnecté\n";
            break;
        }

        Message message = Message::depuisString(brut);

        switch (message.getType())
        {
        case Message::Type::CONNEXION:
            std::cout << message.getAuteur()
                      << " s'est connecté\n";
            break;

        case Message::Type::DECONNEXION:
            std::cout << message.getAuteur()
                      << " s'est déconnecté\n";
            return;

        case Message::Type::MESSAGE:
            std::cout << "["
                      << message.getAuteur()
                      << "] "
                      << message.getContenu()
                      << "\n";
            break;

        case Message::Type::WIZZ:
            std::cout << "*** WIZZ de "
                      << message.getAuteur()
                      << " ***\n";
            break;

        default:
            std::cout << "Message inconnu reçu\n";
            break;
        }

        // Accusé de réception
        Message reponse(
            Message::Type::MESSAGE,
            "Serveur",
            "OK"
        );

        client.envoyer(reponse.toString());
    }
}


