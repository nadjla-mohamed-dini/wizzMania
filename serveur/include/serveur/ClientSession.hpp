#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "commun/Message.hpp"
#include "commun/SocketTCP.hpp"
#include "serveur/UserStore.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class GestionnaireClients;

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    ClientSession(SocketTCP socket, GestionnaireClients& gestionnaire, UserStore& users);
    ~ClientSession();

    void demarrer();
    void arreter();

    void envoyer(const Message& message);

    std::string getPseudo() const;
    bool estAuthentifie() const;

private:
    void boucle();

private:
    SocketTCP m_socket;
    GestionnaireClients& m_gestionnaire;
    UserStore& m_users;
    std::string m_pseudo;
    bool m_auth = false;

    std::atomic<bool> m_arret{false};
    std::thread m_thread;
    mutable std::mutex m_mutexEnvoi;
};

#endif // CLIENT_SESSION_HPP

