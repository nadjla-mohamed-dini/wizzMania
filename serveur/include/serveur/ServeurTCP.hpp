#ifndef SERVEUR_TCP_HPP
#define SERVEUR_TCP_HPP

#include "commun/SocketTCP.hpp"
#include "serveur/GestionnaireClients.hpp"

class ServeurTCP
{
public:
    explicit ServeurTCP(int port);

    bool demarrer();
    void attendreClient();

private:
    int m_port;
    SocketTCP m_socketEcoute;
    GestionnaireClients m_gestionnaire;
};

#endif // SERVEUR_TCP_HPP
