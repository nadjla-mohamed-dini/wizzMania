#include "serveur/UserStore.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>
#include <sstream>

UserStore::UserStore(std::string filePath)
    : m_filePath(std::move(filePath))
{
    loadFromDisk();
}

bool UserStore::registerUser(const std::string& username, const std::string& password, std::string& err)
{
    if (!isValidUsername(username))
    {
        err = "Nom d'utilisateur invalide (3-20, [A-Za-z0-9_])";
        return false;
    }
    if (password.size() < 4)
    {
        err = "Mot de passe trop court (min 4)";
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_users.find(username) != m_users.end())
    {
        err = "Utilisateur déjà existant";
        return false;
    }

    const std::string salt = makeSalt();
    const std::string h = hashPassword(salt, password);
    m_users[username] = salt + "|" + h;
    saveToDiskLocked();
    return true;
}

bool UserStore::verifyUser(const std::string& username, const std::string& password) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_users.find(username);
    if (it == m_users.end())
        return false;

    const std::string& v = it->second;
    const size_t sep = v.find('|');
    if (sep == std::string::npos)
        return false;
    const std::string salt = v.substr(0, sep);
    const std::string expected = v.substr(sep + 1);
    return hashPassword(salt, password) == expected;
}

std::vector<std::string> UserStore::listUsers() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> out;
    out.reserve(m_users.size());
    for (const auto& kv : m_users)
        out.push_back(kv.first);
    std::sort(out.begin(), out.end());
    return out;
}

void UserStore::loadFromDisk()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_users.clear();

    std::ifstream f(m_filePath);
    if (!f.is_open())
        return;

    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty())
            continue;
        // username|salt|hash
        std::istringstream iss(line);
        std::string username, salt, hash;
        std::getline(iss, username, '|');
        std::getline(iss, salt, '|');
        std::getline(iss, hash);
        if (!isValidUsername(username) || salt.empty() || hash.empty())
            continue;
        m_users[username] = salt + "|" + hash;
    }
}

void UserStore::saveToDiskLocked() const
{
    std::ofstream f(m_filePath, std::ios::trunc);
    if (!f.is_open())
        return;

    for (const auto& kv : m_users)
    {
        // kv.second = "salt|hash"
        const size_t sep = kv.second.find('|');
        if (sep == std::string::npos)
            continue;
        const std::string salt = kv.second.substr(0, sep);
        const std::string hash = kv.second.substr(sep + 1);
        f << kv.first << "|" << salt << "|" << hash << "\n";
    }
}

std::string UserStore::makeSalt()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    std::ostringstream oss;
    for (int i = 0; i < 12; ++i)
    {
        const int b = dist(rng);
        oss << std::hex << ((b >> 4) & 0xF) << (b & 0xF);
    }
    return oss.str();
}

std::string UserStore::hashPassword(const std::string& salt, const std::string& password)
{
    // Simple demo hash (NOT cryptographically secure).
    // Good enough for a school project, but never for real authentication.
    const std::string s = salt + ":" + password;
    const auto h = std::hash<std::string>{}(s);
    return std::to_string(static_cast<unsigned long long>(h));
}

bool UserStore::isValidUsername(const std::string& username)
{
    if (username.size() < 3 || username.size() > 20)
        return false;
    for (unsigned char c : username)
    {
        if (std::isalnum(c) || c == '_')
            continue;
        return false;
    }
    return true;
}

