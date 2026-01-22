#include "client/ClientTCP.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    const std::string IP = "127.0.0.1";
    const int PORT = 4242;

    std::string pseudo;
    bool modeAuto = false;

    // Usage :
    //   client.exe <pseudo> [--auto]
    //   client.exe            (mode interactif)
    // En mode --auto : envoie 1 message + 1 wizz, puis quitte.
    if (argc >= 2)
    {
        pseudo = argv[1];
        if (argc >= 3 && std::string(argv[2]) == "--auto")
            modeAuto = true;
    }
    else
    {
        std::cout << "Entrez votre pseudo : ";
        std::getline(std::cin, pseudo);
    }

    ClientTCP client(IP, PORT, pseudo);

    if (!client.connecter())
    {
        std::cerr << "Impossible de se connecter au serveur\n";
        return 1;
    }

    std::cout << "Connecté au serveur\n";
    std::cout << "Commandes :\n";
    std::cout << "  - /wizz : envoyer un Wizz\n";
    std::cout << "  - /quit : quitter\n";

    client.demarrerReception();

    if (modeAuto)
    {
        client.envoyerMessage("Bonjour depuis le client (mode auto)");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        client.envoyerWizz();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        client.envoyerMessage("Fin du mode auto, au revoir");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        client.deconnecter();
        return 0;
    }

    while (true)
    {
        std::string saisie;
        std::getline(std::cin, saisie);

        if (saisie == "/quit")
            break;

        if (saisie == "/wizz")
            client.envoyerWizz();
        else
            client.envoyerMessage(saisie);
    }

    client.deconnecter();
    return 0;
}
