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

    std::string brut = client.recevoir();
    if (brut.empty())
        return;

    Message message = Message::depuisString(brut);

    if (message.getType() == Message::Type::MESSAGE)
    {
        std::cout << "["
                  << message.getAuteur()
                  << "] "
                  << message.getContenu()
                  << "\n";
    }

    // Exemple de réponse
    Message reponse(
        Message::Type::MESSAGE,
        "Serveur",
        "Message reçu"
    );

    client.envoyer(reponse.toString());
}
