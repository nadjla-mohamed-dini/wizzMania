#include "serveur/GestionnaireClients.hpp"
#include "serveur/ClientSession.hpp"

void GestionnaireClients::ajouter(const std::shared_ptr<ClientSession>& session)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_clients.emplace_back(session);
}

void GestionnaireClients::retirer(ClientSession* session)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_clients.begin();
    while (it != m_clients.end())
    {
        auto sp = it->lock();
        if (!sp)
        {
            it = m_clients.erase(it);
            continue;
        }

        if (sp.get() == session)
        {
            it = m_clients.erase(it);
            continue;
        }

        ++it;
    }
}

void GestionnaireClients::diffuser(const Message& message, ClientSession* exclure)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_clients.begin();
    while (it != m_clients.end())
    {
        auto sp = it->lock();
        if (!sp)
        {
            it = m_clients.erase(it);
            continue;
        }

        if (exclure && sp.get() == exclure)
        {
            ++it;
            continue;
        }

        sp->envoyer(message);
        ++it;
    }
}

