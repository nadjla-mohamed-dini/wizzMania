#include "commun/Message.hpp"
#include <sstream>

Message::Message(Type type,
                 const std::string& auteur,
                 const std::string& contenu)
    : m_type(type), m_auteur(auteur), m_contenu(contenu)
{
}

std::string Message::toString() const
{
    std::ostringstream oss;
    oss << typeVersString(m_type)
        << "|" << m_auteur
        << "|" << m_contenu;
    return oss.str();
}

Message Message::depuisString(const std::string& brut)
{
    std::istringstream iss(brut);
    std::string typeStr;
    std::string auteur;
    std::string contenu;

    std::getline(iss, typeStr, '|');
    std::getline(iss, auteur, '|');
    std::getline(iss, contenu);

    Type type = typeDepuisString(typeStr);

    return Message(type, auteur, contenu);
}

Message::Type Message::typeDepuisString(const std::string& type)
{
    if (type == "MESSAGE") return Type::MESSAGE;
    if (type == "WIZZ") return Type::WIZZ;
    if (type == "CONNEXION") return Type::CONNEXION;
    if (type == "DECONNEXION") return Type::DECONNEXION;
    return Type::INCONNU;
}

std::string Message::typeVersString(Type type)
{
    switch (type)
    {
    case Type::MESSAGE:     return "MESSAGE";
    case Type::WIZZ:        return "WIZZ";
    case Type::CONNEXION:   return "CONNEXION";
    case Type::DECONNEXION: return "DECONNEXION";
    default:                return "INCONNU";
    }
}

Message::Type Message::getType() const
{
    return m_type;
}

std::string Message::getAuteur() const
{
    return m_auteur;
}

std::string Message::getContenu() const
{
    return m_contenu;
}
