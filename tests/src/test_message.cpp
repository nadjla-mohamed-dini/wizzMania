#include "commun/Message.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Message - sérialisation et parsing (MESSAGE)")
{
    Message m(Message::Type::MESSAGE, "Alice", "Salut");
    const std::string brut = m.toString();

    Message parsed = Message::depuisString(brut);
    REQUIRE(parsed.getType() == Message::Type::MESSAGE);
    REQUIRE(parsed.getAuteur() == "Alice");
    REQUIRE(parsed.getContenu() == "Salut");
}

TEST_CASE("Message - sérialisation et parsing (WIZZ)")
{
    Message m(Message::Type::WIZZ, "Bob");
    const std::string brut = m.toString();

    Message parsed = Message::depuisString(brut);
    REQUIRE(parsed.getType() == Message::Type::WIZZ);
    REQUIRE(parsed.getAuteur() == "Bob");
}

TEST_CASE("Message - type inconnu")
{
    Message parsed = Message::depuisString("TRUC|X|Y");
    REQUIRE(parsed.getType() == Message::Type::INCONNU);
    REQUIRE(parsed.getAuteur() == "X");
    REQUIRE(parsed.getContenu() == "Y");
}

TEST_CASE("Message - parsing: contenu peut contenir des séparateurs '|'")
{
    // Le protocole utilise 2 séparateurs max: TYPE|AUTEUR|CONTENU.
    // Tout ce qui suit le 2e '|' fait partie du contenu.
    Message parsed = Message::depuisString("MESSAGE|Alice|Salut|Extra|OK");
    REQUIRE(parsed.getType() == Message::Type::MESSAGE);
    REQUIRE(parsed.getAuteur() == "Alice");
    REQUIRE(parsed.getContenu() == "Salut|Extra|OK");
}

TEST_CASE("Message - parsing: champs manquants")
{
    SECTION("Pas de contenu (TYPE|AUTEUR)")
    {
        Message parsed = Message::depuisString("MESSAGE|Alice");
        REQUIRE(parsed.getType() == Message::Type::MESSAGE);
        REQUIRE(parsed.getAuteur() == "Alice");
        REQUIRE(parsed.getContenu() == "");
    }

    SECTION("Auteur vide (TYPE||CONTENU)")
    {
        Message parsed = Message::depuisString("MESSAGE||Hello");
        REQUIRE(parsed.getType() == Message::Type::MESSAGE);
        REQUIRE(parsed.getAuteur() == "");
        REQUIRE(parsed.getContenu() == "Hello");
    }

    SECTION("Chaîne vide")
    {
        Message parsed = Message::depuisString("");
        REQUIRE(parsed.getType() == Message::Type::INCONNU);
        REQUIRE(parsed.getAuteur() == "");
        REQUIRE(parsed.getContenu() == "");
    }
}

TEST_CASE("Message - sérialisation: WIZZ produit un contenu vide")
{
    Message m(Message::Type::WIZZ, "Bob");
    const std::string brut = m.toString();

    // Format attendu: "WIZZ|Bob|"
    REQUIRE(brut.rfind("WIZZ|Bob|", 0) == 0);

    Message parsed = Message::depuisString(brut);
    REQUIRE(parsed.getType() == Message::Type::WIZZ);
    REQUIRE(parsed.getAuteur() == "Bob");
    REQUIRE(parsed.getContenu() == "");
}

