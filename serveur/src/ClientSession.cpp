#include "serveur/ClientSession.hpp"
#include "serveur/GestionnaireClients.hpp"
#include <iostream>

ClientSession::ClientSession(SocketTCP socket, GestionnaireClients& gestionnaire, UserStore& users)
    : m_socket(std::move(socket)), m_gestionnaire(gestionnaire), m_users(users)
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

bool ClientSession::estAuthentifie() const
{
    return m_auth;
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
        case Message::Type::REGISTER:
        {
            std::string err;
            const bool ok = m_users.registerUser(message.getAuteur(), message.getContenu(), err);
            if (ok)
                envoyer(Message(Message::Type::AUTH_OK, "serveur", "REGISTER_OK"));
            else
                envoyer(Message(Message::Type::AUTH_FAIL, "serveur", err));
            break;
        }

        case Message::Type::LOGIN:
        {
            const std::string username = message.getAuteur();
            const std::string password = message.getContenu();

            if (!m_users.verifyUser(username, password))
            {
                envoyer(Message(Message::Type::AUTH_FAIL, "serveur", "LOGIN_FAIL"));
                break;
            }

            // Refuse duplicate online usernames by renaming (keeps session usable)
            const std::string unique = m_gestionnaire.pseudoUnique(username);
            m_pseudo = unique;
            m_auth = true;

            envoyer(Message(Message::Type::AUTH_OK, "serveur", "LOGIN_OK"));
            m_gestionnaire.diffuser(Message(Message::Type::CONNEXION, m_pseudo), this);
            m_gestionnaire.diffuserContacts(m_users);
            std::cout << m_pseudo << " s'est connecté (login)\n";
            break;
        }

        case Message::Type::CONNEXION:
            // Compat legacy: treat CONNEXION as "guest login" (no password).
            if (!m_auth)
            {
                const std::string desired = message.getAuteur().empty() ? "Guest" : message.getAuteur();
                m_pseudo = m_gestionnaire.pseudoUnique(desired);
                m_auth = true;
                std::cout << m_pseudo << " s'est connecté (legacy)\n";
                m_gestionnaire.diffuser(Message(Message::Type::CONNEXION, m_pseudo), this);
                m_gestionnaire.diffuserContacts(m_users);
            }
            break;

        case Message::Type::MESSAGE:
            if (!m_auth || m_pseudo.empty())
                break;
            std::cout << "[" << m_pseudo << "] " << message.getContenu() << "\n";
            m_gestionnaire.diffuser(Message(Message::Type::MESSAGE, m_pseudo, message.getContenu()), this);
            break;

        case Message::Type::WIZZ:
            if (!m_auth || m_pseudo.empty())
                break;
            std::cout << "*** WIZZ de " << m_pseudo << " ***\n";
            m_gestionnaire.diffuser(Message(Message::Type::WIZZ, m_pseudo), this);
            break;

        case Message::Type::PRIVE:
        {
            if (!m_auth || m_pseudo.empty())
                break;
            const std::string cible = message.getCible();
            if (cible.empty())
                break;

            auto dest = m_gestionnaire.trouverParPseudo(cible);
            if (!dest)
            {
                envoyer(Message(Message::Type::MESSAGE, "serveur", "Utilisateur hors ligne"));
                break;
            }

            dest->envoyer(Message(Message::Type::PRIVE, m_pseudo, cible, message.getContenu()));
            break;
        }

        case Message::Type::DECONNEXION:
            if (m_pseudo.empty())
                m_pseudo = message.getAuteur();
            std::cout << m_pseudo << " s'est déconnecté\n";
            m_gestionnaire.diffuser(Message(Message::Type::DECONNEXION, m_pseudo), this);
            m_gestionnaire.retirer(this);
            m_gestionnaire.diffuserContacts(m_users);
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
        m_gestionnaire.diffuserContacts(m_users);
    }

    m_gestionnaire.retirer(this);
}

