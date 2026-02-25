#include "usermanagertest.h"

#include <QDebug>
#include <QtTest/QTest>

UserManagerTest::UserManagerTest()
{
}

void UserManagerTest::initTestCase()
{
    qDebug() << "=== Début des tests UserManager ===";
}

void UserManagerTest::cleanupTestCase()
{
    qDebug() << "=== Fin des tests UserManager ===";
}

void UserManagerTest::init()
{
    // Créer un nouveau UserManager avant chaque test
    userManager = new UserManager();
}

void UserManagerTest::cleanup()
{
    // Nettoyer après chaque test
    delete userManager;
}


// TEST 1 : userExists avec utilisateur existant

void UserManagerTest::testUserExistsWithExistingUser()
{
    qDebug() << "Test 1 : userExists avec utilisateur existant";

    bool result = userManager->userExists("Emma");
    QVERIFY(result == true);
}


// TEST 2 : userExists avec utilisateur inexistant

void UserManagerTest::testUserExistsWithNonExistingUser()
{
    qDebug() << "Test 2 : userExists avec utilisateur inexistant";

    bool result = userManager->userExists("Inconnu");
    QVERIFY(result == false);
}

// TEST 3 : checkPassword avec mot de passe correct

void UserManagerTest::testCheckPasswordCorrect()
{
    qDebug() << "Test 3 : checkPassword avec mot de passe correct";

    bool result = userManager->checkPassword("Emma", "Alice123");
    QVERIFY(result == true);
}

// TEST 4 : checkPassword avec mot de passe incorrect

//void UserManagerTest::testCheckPasswordIncorrect()
//{
//    qDebug() << "Test 4 : checkPassword avec mot de passe incorrect";

//    bool result = userManager->checkPassword("Emma", "wrong");
//    QVERIFY(result == false);
//}


// TEST 5 : addUser avec nouvel utilisateur

//void UserManagerTest::testAddNewUser()
//{
//    qDebug() << "Test 5 : addUser avec nouvel utilisateur";

    // Ajouter un nouvel utilisateur
//    bool resultAdd = userManager->addUsers("Alice", "test123");
//    QVERIFY(resultAdd == true);

    // Vérifier qu'Alice existe maintenant
//    bool resultExists = userManager->userExists("Alice");
//    QVERIFY(resultExists == true);
//}

// TEST 6 : addUser avec utilisateur existant
//void UserManagerTest::testAddExistingUser()
//{
//    qDebug() << "Test 6 : addUser avec utilisateur existant";

    // Emma existe déjà, ne devrait pas pouvoir l'ajouter à nouveau
//    bool result = userManager->addUsers("Emma", "newpass");
//    QVERIFY(result == false);
//}
