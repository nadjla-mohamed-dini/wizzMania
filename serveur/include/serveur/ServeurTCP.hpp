#ifndef SERVEUR_TCP_HPP
#define SERVEUR_TCP_HPP

#include "commun/SocketTCP.hpp"
#include "serveur/GestionnaireClients.hpp"
#include "serveur/UserStore.hpp"

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
    UserStore m_users;
};

#endif // SERVEUR_TCP_HPP
