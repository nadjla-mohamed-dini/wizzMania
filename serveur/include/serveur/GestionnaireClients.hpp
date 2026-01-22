#ifndef GESTIONNAIRE_CLIENTS_HPP
#define GESTIONNAIRE_CLIENTS_HPP

#include "commun/Message.hpp"
#include <memory>
#include <mutex>
#include <vector>

class ClientSession;

class GestionnaireClients
{
public:
    void ajouter(const std::shared_ptr<ClientSession>& session);
    void retirer(ClientSession* session);

    // Diffuser un message à tous les clients (optionnellement sauf un)
    void diffuser(const Message& message, ClientSession* exclure = nullptr);

private:
    std::mutex m_mutex;
    std::vector<std::weak_ptr<ClientSession>> m_clients;
};

#endif // GESTIONNAIRE_CLIENTS_HPP

