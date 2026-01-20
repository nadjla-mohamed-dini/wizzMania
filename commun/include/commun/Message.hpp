#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

class Message
{
public:
    enum class Type
    {
        MESSAGE,
        WIZZ,
        CONNEXION,
        DECONNEXION,
        INCONNU
    };

    Message(Type type,
            const std::string& auteur,
            const std::string& contenu = "");

    // Sérialisation
    std::string toString() const;

    // Parsing
    static Message depuisString(const std::string& brut);

    // Accesseurs
    Type getType() const;
    std::string getAuteur() const;
    std::string getContenu() const;

private:
    static Type typeDepuisString(const std::string& type);
    static std::string typeVersString(Type type);

private:
    Type m_type;
    std::string m_auteur;
    std::string m_contenu;
};

#endif // MESSAGE_HPP
