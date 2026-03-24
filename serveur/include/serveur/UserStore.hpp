#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class UserStore
{
public:
    explicit UserStore(std::string filePath);

    // Register a new user. Returns true on success, false otherwise and sets err.
    bool registerUser(const std::string& username, const std::string& password, std::string& err);

    // Verify credentials. Returns true if username exists and password matches.
    bool verifyUser(const std::string& username, const std::string& password) const;

    // Returns list of all registered usernames.
    std::vector<std::string> listUsers() const;

private:
    void loadFromDisk();
    void saveToDiskLocked() const;

    static std::string makeSalt();
    static std::string hashPassword(const std::string& salt, const std::string& password);
    static bool isValidUsername(const std::string& username);

private:
    std::string m_filePath;

    // username -> "salt|hash"
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, std::string> m_users;
};

