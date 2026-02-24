#include "client/ClientTCP.hpp"
#include <iostream>
#include <utility>

ClientTCP::ClientTCP(const std::string& ip, int port, const std::string& pseudo)
    : m_ip(ip), m_port(port), m_pseudo(pseudo)
{
}

ClientTCP::~ClientTCP()
{
    deconnecter();
}

bool ClientTCP::connecter()
{
    if (!m_socket.creer())
    {
        std::cerr << "Erreur : création du socket client\n";
        return false;
    }

    if (!m_socket.connecter(m_ip, m_port))
    {
        std::cerr << "Erreur : connexion au serveur\n";
        return false;
    }

    // Message de connexion
    Message connexion(Message::Type::CONNEXION, m_pseudo);
    m_socket.envoyer(connexion.toString());

    m_connecte = true;
    return true;
}

void ClientTCP::deconnecter()
{
    bool etaitConnecte = m_connecte.exchange(false);
    m_arret = true;

    if (etaitConnecte && m_socket.estValide())
    {
        Message deco(Message::Type::DECONNEXION, m_pseudo);
        m_socket.envoyer(deco.toString());
    }

    // Débloque recv() si nécessaire
    m_socket.fermer();

    if (m_threadReception.joinable())
        m_threadReception.join();
}

void ClientTCP::envoyerMessage(const std::string& texte)
{
    if (!m_connecte)
        return;
    Message msg(Message::Type::MESSAGE, m_pseudo, texte);
    m_socket.envoyer(msg.toString());
}

void ClientTCP::envoyerWizz()
{
    if (!m_connecte)
        return;
    Message wizz(Message::Type::WIZZ, m_pseudo);
    m_socket.envoyer(wizz.toString());
}

void ClientTCP::demarrerReception()
{
    if (!m_connecte)
        return;
    if (m_threadReception.joinable())
        return;

    m_arret = false;
    m_threadReception = std::thread(&ClientTCP::boucleReception, this);
}

void ClientTCP::boucleReception()
{
    while (!m_arret)
    {
        std::string brut = m_socket.recevoir();
        if (brut.empty())
            break;

        Message message = Message::depuisString(brut);

        switch (message.getType())
        {
        case Message::Type::MESSAGE:
            std::cout << "["
                      << message.getAuteur()
                      << "] "
                      << message.getContenu()
                      << "\n";
            break;

        case Message::Type::WIZZ:
        {
            // Alerte visuelle + tentative d'alerte sonore (bell '\a')
            std::cout << '\a' << std::flush;
            std::cout << "\n"
                      << "========================================\n"
                      << "               W I Z Z  !\n"
                      << "            de " << message.getAuteur() << "\n"
                      << "========================================\n";
            break;
        }

        case Message::Type::CONNEXION:
            std::cout << "*** "
                      << message.getAuteur()
                      << " s'est connecté ***\n";
            break;

        case Message::Type::DECONNEXION:
            std::cout << "*** "
                      << message.getAuteur()
                      << " s'est déconnecté ***\n";
            break;

        default:
            std::cout << "(Message inconnu) " << brut << "\n";
            break;
        }
    }

    m_connecte = false;
}
