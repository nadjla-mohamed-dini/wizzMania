#ifndef SERVEUR_TCP_HPP
#define SERVEUR_TCP_HPP

#include <string>
#include <winsock2.h>

class ServeurTCP
{
public:
    ServeurTCP(int port);
    ~ServeurTCP();

    bool demarrer();
    void attendreClient();

private:
    bool initialiserWinsock();
    bool creerSocket();
    bool lierSocket();
    bool ecouter();

private:
    int m_port;
    SOCKET m_socketEcoute;
};

#endif // SERVEUR_TCP_HPP
