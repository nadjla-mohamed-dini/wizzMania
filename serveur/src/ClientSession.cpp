#include "serveur/ClientSession.hpp"
#include "serveur/GestionnaireClients.hpp"
#include <iostream>

ClientSession::ClientSession(SocketTCP socket, GestionnaireClients& gestionnaire)
    : m_socket(std::move(socket)), m_gestionnaire(gestionnaire)
{
}

ClientSession::~ClientSession()
{
    arreter();
}

void ClientSession::demarrer()
{
    auto self = shared_from_this();
    m_thread = std::thread([self]() { self->boucle(); });
    m_thread.detach(); // le serveur tourne en continu, on évite de bloquer au join
}

void ClientSession::arreter()
{
    m_arret = true;
    m_socket.fermer();
}

void ClientSession::envoyer(const Message& message)
{
    std::lock_guard<std::mutex> lock(m_mutexEnvoi);
    if (!m_socket.estValide())
        return;
    m_socket.envoyer(message.toString());
}

std::string ClientSession::getPseudo() const
{
    return m_pseudo;
}

void ClientSession::boucle()
{
    std::cout << "Client connecté (session)\n";

    while (!m_arret)
    {
        const std::string brut = m_socket.recevoir();
        if (brut.empty())
            break;

        Message message = Message::depuisString(brut);

        switch (message.getType())
        {
        case Message::Type::CONNEXION:
            m_pseudo = message.getAuteur();
            std::cout << m_pseudo << " s'est connecté\n";
            m_gestionnaire.diffuser(Message(Message::Type::CONNEXION, m_pseudo), this);
            break;

        case Message::Type::MESSAGE:
            std::cout << "[" << message.getAuteur() << "] " << message.getContenu() << "\n";
            m_gestionnaire.diffuser(message, this);
            break;

        case Message::Type::WIZZ:
            std::cout << "*** WIZZ de " << message.getAuteur() << " ***\n";
            m_gestionnaire.diffuser(message, this);
            break;

        case Message::Type::DECONNEXION:
            if (m_pseudo.empty())
                m_pseudo = message.getAuteur();
            std::cout << m_pseudo << " s'est déconnecté\n";
            m_gestionnaire.diffuser(Message(Message::Type::DECONNEXION, m_pseudo), this);
            m_gestionnaire.retirer(this);
            return;

        default:
            std::cout << "(Message inconnu) " << brut << "\n";
            break;
        }
    }

    if (!m_pseudo.empty())
    {
        std::cout << m_pseudo << " s'est déconnecté (socket fermé)\n";
        m_gestionnaire.diffuser(Message(Message::Type::DECONNEXION, m_pseudo), this);
    }

    m_gestionnaire.retirer(this);
}

