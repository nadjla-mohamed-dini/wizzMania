#ifndef SOCKET_TCP_HPP
#define SOCKET_TCP_HPP

#include <winsock2.h>
#include <string>

class SocketTCP
{
public:
    SocketTCP();
    ~SocketTCP();

    bool creer();
    bool connecter(const std::string& ip, int port);
    bool lier(int port);
    bool ecouter();
    SocketTCP accepter();

    bool envoyer(const std::string& message);
    std::string recevoir();

    bool estValide() const;
    void fermer();

private:
    static bool initialiserWinsock();
    static int s_compteurWinsock;

    SOCKET m_socket;
};

#endif // SOCKET_TCP_HPP
