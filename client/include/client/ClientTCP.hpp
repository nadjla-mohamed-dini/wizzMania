#ifndef CLIENT_TCP_HPP
#define CLIENT_TCP_HPP

#include "commun/SocketTCP.hpp"
#include "commun/Message.hpp"
#include <atomic>
#include <thread>
#include <string>

class ClientTCP
{
public:
    ClientTCP(const std::string& ip, int port, const std::string& pseudo);
    ~ClientTCP();

    bool connecter();
    void deconnecter();

    void envoyerMessage(const std::string& texte);
    void envoyerWizz();

    // Réception asynchrone (recommandé pour multi-clients)
    void demarrerReception();

private:
    void boucleReception();

    std::string m_ip;
    int m_port;
    std::string m_pseudo;
    SocketTCP m_socket;

    std::atomic<bool> m_arret{false};
    std::atomic<bool> m_connecte{false};
    std::thread m_threadReception;
};

#endif // CLIENT_TCP_HPP
