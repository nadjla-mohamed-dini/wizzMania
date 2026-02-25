#include "usermanager.h"

UserManager::UserManager(QObject *parent)
    :QObject(parent)
{
    users["Emma"] = "Alice123";
    users["Bob"] = "Bobby456!";
}

bool UserManager::userExists(const QString &username)
{
    return users.contains(username);
}

bool UserManager::addUsers(const QString &username, const QString &password)
{
    if (!userExists(username)){
        return false;
    }
    users[username] = password;
    return true;
}

bool UserManager::checkPassword(const QString &username, const QString &password)
{
    if(!userExists(username)){
        return false;
    }
    QString storedPassword = users[username];
        return (storedPassword == password);
}
