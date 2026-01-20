#include "serveur/ServeurTCP.hpp"
#include <iostream>

int main()
{
    const int PORT = 4242;

    ServeurTCP serveur(PORT);

    if (!serveur.demarrer())
    {
        std::cerr << "Impossible de démarrer le serveur\n";
        return 1;
    }

    std::cout << "Serveur démarré sur le port " << PORT << "\n";
    serveur.attendreClient();

    return 0;
}
