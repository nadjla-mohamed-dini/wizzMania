#ifndef SERVEUR_TCP_HPP
#define SERVEUR_TCP_HPP

#include "commun/SocketTCP.hpp"
#include "commun/Message.hpp"

class ServeurTCP
{
public:
    explicit ServeurTCP(int port);

    bool demarrer();
    void attendreClient();

private:
    int m_port;
    SocketTCP m_socketEcoute;
};

#endif // SERVEUR_TCP_HPP
