#include "commun/Message.hpp"
#include <sstream>

Message::Message(Type type,
                 const std::string& auteur,
                 const std::string& contenu)
    : m_type(type), m_auteur(auteur), m_contenu(contenu)
{
}

Message::Message(Type type,
                 const std::string& auteur,
                 const std::string& cible,
                 const std::string& contenu)
    : m_type(type), m_auteur(auteur), m_cible(cible), m_contenu(contenu)
{
}

std::string Message::toString() const
{
    std::ostringstream oss;
    oss << typeVersString(m_type)
        << "|" << m_auteur;

    if (typeUtiliseCible(m_type))
    {
        // Format étendu (4 champs): TYPE|AUTEUR|CIBLE|CONTENU
        oss << "|" << m_cible
            << "|" << m_contenu;
    }
    else
    {
        // Format historique (3 champs): TYPE|AUTEUR|CONTENU
        oss << "|" << m_contenu;
    }
    return oss.str();
}

Message Message::depuisString(const std::string& brut)
{
    std::istringstream iss(brut);
    std::string typeStr;
    std::string auteur;
    std::string reste;

    std::getline(iss, typeStr, '|');
    std::getline(iss, auteur, '|');
    std::getline(iss, reste);

    Type type = typeDepuisString(typeStr);

    if (typeUtiliseCible(type))
    {
        // Reste = CIBLE|CONTENU (CONTENU peut contenir des '|')
        std::string cible;
        std::string contenu;
        {
            std::istringstream r(reste);
            std::getline(r, cible, '|');
            std::getline(r, contenu);
        }
        return Message(type, auteur, cible, contenu);
    }

    return Message(type, auteur, reste);
}

Message::Type Message::typeDepuisString(const std::string& type)
{
    if (type == "MESSAGE") return Type::MESSAGE;
    if (type == "WIZZ") return Type::WIZZ;
    if (type == "CONNEXION") return Type::CONNEXION;
    if (type == "DECONNEXION") return Type::DECONNEXION;
    if (type == "REGISTER") return Type::REGISTER;
    if (type == "LOGIN") return Type::LOGIN;
    if (type == "AUTH_OK") return Type::AUTH_OK;
    if (type == "AUTH_FAIL") return Type::AUTH_FAIL;
    if (type == "CONTACTS") return Type::CONTACTS;
    if (type == "PRIVE") return Type::PRIVE;
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
    case Type::REGISTER:    return "REGISTER";
    case Type::LOGIN:       return "LOGIN";
    case Type::AUTH_OK:     return "AUTH_OK";
    case Type::AUTH_FAIL:   return "AUTH_FAIL";
    case Type::CONTACTS:    return "CONTACTS";
    case Type::PRIVE:       return "PRIVE";
    default:                return "INCONNU";
    }
}

bool Message::typeUtiliseCible(Type type)
{
    switch (type)
    {
    case Type::PRIVE:
        return true;
    default:
        return false;
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

bool Message::aCible() const
{
    return !m_cible.empty();
}

std::string Message::getCible() const
{
    return m_cible;
}

std::string Message::getContenu() const
{
    return m_contenu;
}
