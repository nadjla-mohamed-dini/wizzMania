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

