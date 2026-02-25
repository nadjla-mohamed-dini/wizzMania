#ifndef USERMANAGERTEST_H
#define USERMANAGERTEST_H
#include <QObject>
#include <QTest>
#include "usermanager.h"


class UserManagerTest : public QObject
{
    Q_OBJECT
public:
    UserManagerTest();

private slots:
    void initTestCase();

    void cleanupTestCase();

    void init();

    void cleanup();

    //====TEST===
    void testUserExistsWithExistingUser();

    void testUserExistsWithNonExistingUser();

    void testCheckPasswordCorrect();

    //void testCheckPasswordIncorrect();

    //void testAddExistingUser();

    //void testAddNewUser();

private:
    UserManager *userManager;
};

#endif // USERMANAGERTEST_H
