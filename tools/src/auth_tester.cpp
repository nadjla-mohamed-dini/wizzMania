#include "commun/Message.hpp"
#include "commun/SocketTCP.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

static void printUsage()
{
    std::cout
        << "Usage:\n"
        << "  auth_tester.exe register <user> <pass> [--host <ip>] [--port <p>]\n"
        << "  auth_tester.exe login    <user> <pass> [--host <ip>] [--port <p>] [--listen-ms <ms>]\n"
        << "                            [--send-private <to> <msg>]\n"
        << "  auth_tester.exe guest    <pseudo>      [--host <ip>] [--port <p>] [--listen-ms <ms>]\n"
        << "                            [--send-private <to> <msg>]\n";
}

static bool argEq(const std::string& a, const char* lit) { return a == lit; }

struct Options
{
    std::string mode;
    std::string user;
    std::string pass;
    std::string pseudo;
    std::string host = "127.0.0.1";
    int port = 4242;
    int listenMs = 1500;
    bool doListen = true;
    bool sendPrivate = false;
    std::string to;
    std::string msg;
};

static bool parseArgs(int argc, char* argv[], Options& opt)
{
    if (argc < 2)
        return false;
    opt.mode = argv[1];

    int i = 2;
    if (opt.mode == "register" || opt.mode == "login")
    {
        if (argc < 4)
            return false;
        opt.user = argv[2];
        opt.pass = argv[3];
        i = 4;
    }
    else if (opt.mode == "guest")
    {
        if (argc < 3)
            return false;
        opt.pseudo = argv[2];
        i = 3;
    }
    else
    {
        return false;
    }

    while (i < argc)
    {
        const std::string a = argv[i];
        if (argEq(a, "--host") && i + 1 < argc)
        {
            opt.host = argv[i + 1];
            i += 2;
        }
        else if (argEq(a, "--port") && i + 1 < argc)
        {
            opt.port = std::stoi(argv[i + 1]);
            i += 2;
        }
        else if (argEq(a, "--listen-ms") && i + 1 < argc)
        {
            opt.listenMs = std::stoi(argv[i + 1]);
            opt.doListen = true;
            i += 2;
        }
        else if (argEq(a, "--no-listen"))
        {
            opt.doListen = false;
            i += 1;
        }
        else if (argEq(a, "--send-private") && i + 2 < argc)
        {
            opt.sendPrivate = true;
            opt.to = argv[i + 1];
            opt.msg = argv[i + 2];
            i += 3;
        }
        else
        {
            return false;
        }
    }

    return true;
}

static void printIncoming(const Message& m, const std::string& raw)
{
    switch (m.getType())
    {
    case Message::Type::AUTH_OK:
        std::cout << "[AUTH_OK] " << m.getContenu() << "\n";
        break;
    case Message::Type::AUTH_FAIL:
        std::cout << "[AUTH_FAIL] " << m.getContenu() << "\n";
        break;
    case Message::Type::CONTACTS:
        std::cout << "[CONTACTS] " << m.getContenu() << "\n";
        break;
    case Message::Type::PRIVE:
        std::cout << "[PRIVE] " << m.getAuteur() << " -> " << m.getCible()
                  << " : " << m.getContenu() << "\n";
        break;
    case Message::Type::MESSAGE:
        std::cout << "[MESSAGE] " << m.getAuteur() << " : " << m.getContenu() << "\n";
        break;
    case Message::Type::CONNEXION:
        std::cout << "[CONNEXION] " << m.getAuteur() << "\n";
        break;
    case Message::Type::DECONNEXION:
        std::cout << "[DECONNEXION] " << m.getAuteur() << "\n";
        break;
    case Message::Type::WIZZ:
        std::cout << "[WIZZ] " << m.getAuteur() << "\n";
        break;
    default:
        std::cout << "[RAW] " << raw << "\n";
        break;
    }
}

int main(int argc, char* argv[])
{
    Options opt;
    if (!parseArgs(argc, argv, opt))
    {
        printUsage();
        return 2;
    }

    SocketTCP sock;
    if (!sock.creer())
    {
        std::cerr << "Erreur: creer() socket\n";
        return 1;
    }
    if (!sock.connecter(opt.host, opt.port))
    {
        std::cerr << "Erreur: connecter(" << opt.host << ":" << opt.port << ")\n";
        return 1;
    }

    std::cout << "Connected to " << opt.host << ":" << opt.port << "\n";

    std::atomic<bool> stop{false};
    std::mutex mtx;
    std::condition_variable cv;
    bool gotAuthResult = false;
    bool authOk = false;

    std::thread rx([&] {
        while (!stop)
        {
            const std::string raw = sock.recevoir();
            if (raw.empty())
                break;
            const Message m = Message::depuisString(raw);
            printIncoming(m, raw);

            if (m.getType() == Message::Type::AUTH_OK || m.getType() == Message::Type::AUTH_FAIL)
            {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    gotAuthResult = true;
                    authOk = (m.getType() == Message::Type::AUTH_OK);
                }
                cv.notify_all();
            }
        }
    });

    auto sendLine = [&](const Message& m) {
        sock.envoyer(m.toString());
    };

    if (opt.mode == "register")
    {
        sendLine(Message(Message::Type::REGISTER, opt.user, opt.pass));
    }
    else if (opt.mode == "login")
    {
        sendLine(Message(Message::Type::LOGIN, opt.user, opt.pass));
    }
    else if (opt.mode == "guest")
    {
        sendLine(Message(Message::Type::CONNEXION, opt.pseudo));
        authOk = true;
        gotAuthResult = true;
    }

    if (opt.mode == "login" || opt.mode == "register")
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(2000), [&] { return gotAuthResult; });
    }

    if (opt.sendPrivate)
    {
        // For PRIVE: auteur field is ignored by server; it uses the session pseudo.
        sendLine(Message(Message::Type::PRIVE,
                         opt.user.empty() ? opt.pseudo : opt.user,
                         opt.to,
                         opt.msg));
        std::cout << "Sent PRIVE to " << opt.to << "\n";
    }

    if (opt.doListen)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(opt.listenMs));
    }

    // Clean disconnect
    if (opt.mode == "guest")
        sendLine(Message(Message::Type::DECONNEXION, opt.pseudo));
    else if (opt.mode == "login" && authOk)
        sendLine(Message(Message::Type::DECONNEXION, opt.user));

    stop = true;
    sock.fermer();
    if (rx.joinable())
        rx.join();

    return 0;
}

