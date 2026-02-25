#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = nullptr);

    bool userExists(const QString &username);
    bool checkPassword(const QString &username, const QString &password);
    bool addUsers(const QString &username, const QString &password);

private:
    QMap<QString, QString> users;
};

#endif // USERMANAGER_H
