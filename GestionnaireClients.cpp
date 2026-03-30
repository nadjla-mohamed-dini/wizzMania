#include "serveur/GestionnaireClients.hpp"
#include "serveur/ClientSession.hpp"

static bool contientPseudoCI(const std::vector<std::string>& v, const std::string& name)
{
    for (const auto& s : v)
    {
        if (s.size() != name.size())
            continue;
        bool eq = true;
        for (size_t i = 0; i < s.size(); ++i)
        {
            const char a = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
            const char b = static_cast<char>(std::tolower(static_cast<unsigned char>(name[i])));
            if (a != b) { eq = false; break; }
        }
        if (eq) return true;
    }
    return false;
}

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

std::shared_ptr<ClientSession> GestionnaireClients::trouverParPseudo(const std::string& pseudo)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        auto sp = it->lock();
        if (!sp)
            continue;
        if (sp->getPseudo() == pseudo)
            return sp;
    }
    return {};
}

std::vector<std::string> GestionnaireClients::pseudosConnectes()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> out;
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        auto sp = it->lock();
        if (!sp)
            continue;
        const std::string p = sp->getPseudo();
        if (!p.empty())
            out.push_back(p);
    }
    return out;
}

std::string GestionnaireClients::pseudoUnique(const std::string& souhait)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto isTaken = [&](const std::string& p) {
        for (const auto& w : m_clients)
        {
            auto sp = w.lock();
            if (!sp) continue;
            if (sp->getPseudo() == p) return true;
        }
        return false;
    };

    if (!isTaken(souhait))
        return souhait;

    for (int i = 2; i < 1000; ++i)
    {
        const std::string candidate = souhait + "_" + std::to_string(i);
        if (!isTaken(candidate))
            return candidate;
    }
    return souhait + "_x";
}

void GestionnaireClients::diffuserContacts(UserStore& users)
{
    // Build payload: "name=1;other=0;..."
    std::vector<std::string> registered = users.listUsers();
    std::vector<std::string> online = pseudosConnectes();

    // include guests that are online but not registered
    for (const auto& p : online)
    {
        if (!contientPseudoCI(registered, p))
            registered.push_back(p);
    }

    std::string payload;
    payload.reserve(registered.size() * 12);
    for (size_t i = 0; i < registered.size(); ++i)
    {
        const std::string& name = registered[i];
        const bool isOnline = contientPseudoCI(online, name);
        payload += name;
        payload += '=';
        payload += (isOnline ? '1' : '0');
        if (i + 1 < registered.size())
            payload += ';';
    }

    diffuser(Message(Message::Type::CONTACTS, "serveur", payload));
}

