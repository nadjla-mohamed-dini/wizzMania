#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include <windows.h>

namespace {

std::string exeDir()
{
    char buf[MAX_PATH]{};
    const DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::filesystem::path p(std::string(buf, buf + n));
    return p.parent_path().string();
}

struct RunResult
{
    DWORD exitCode = 0;
    std::string out;
};

RunResult runProcessCapture(const std::string& exe, const std::vector<std::string>& args, const std::string& workdir, DWORD timeoutMs)
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE hRead = nullptr;
    HANDLE hWrite = nullptr;
    REQUIRE(CreatePipe(&hRead, &hWrite, &sa, 0) != 0);
    REQUIRE(SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0) != 0);

    std::string cmd = "\"" + exe + "\"";
    for (const auto& a : args)
    {
        cmd += " \"";
        cmd += a;
        cmd += "\"";
    }

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};

    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    BOOL ok = CreateProcessA(
        nullptr,
        cmdBuf.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        workdir.empty() ? nullptr : workdir.c_str(),
        &si,
        &pi
    );

    CloseHandle(hWrite);
    REQUIRE(ok != 0);

    RunResult rr;
    const DWORD wait = WaitForSingleObject(pi.hProcess, timeoutMs);
    if (wait == WAIT_TIMEOUT)
    {
        TerminateProcess(pi.hProcess, 124);
        WaitForSingleObject(pi.hProcess, 2000);
        rr.exitCode = 124;
    }
    else
    {
        GetExitCodeProcess(pi.hProcess, &rr.exitCode);
    }

    // Read all output
    std::string out;
    char buffer[4096];
    DWORD read = 0;
    while (ReadFile(hRead, buffer, sizeof(buffer), &read, nullptr) && read > 0)
        out.append(buffer, buffer + read);

    rr.out = out;

    CloseHandle(hRead);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return rr;
}

struct Proc
{
    PROCESS_INFORMATION pi{};
    bool started = false;
};

struct ProcFile
{
    PROCESS_INFORMATION pi{};
    HANDLE hFile = nullptr;
    bool started = false;
};

Proc startServer(const std::string& serverExe, const std::string& workdir)
{
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    std::string cmd = "\"" + serverExe + "\"";
    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    BOOL ok = CreateProcessA(
        nullptr,
        cmdBuf.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        workdir.empty() ? nullptr : workdir.c_str(),
        &si,
        &pi
    );

    Proc p;
    p.pi = pi;
    p.started = (ok != 0);
    return p;
}

void stopServer(Proc& p)
{
    if (!p.started)
        return;
    TerminateProcess(p.pi.hProcess, 0);
    WaitForSingleObject(p.pi.hProcess, 2000);
    CloseHandle(p.pi.hThread);
    CloseHandle(p.pi.hProcess);
    p.started = false;
}

ProcFile startProcessToFile(const std::string& exe, const std::vector<std::string>& args, const std::string& workdir, const std::string& outFile)
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE h = CreateFileA(outFile.c_str(),
                           GENERIC_WRITE,
                           FILE_SHARE_READ,
                           &sa,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           nullptr);

    REQUIRE(h != INVALID_HANDLE_VALUE);

    std::string cmd = "\"" + exe + "\"";
    for (const auto& a : args)
        cmd += " \"" + a + "\"";

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = h;
    si.hStdError = h;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};

    std::vector<char> cmdBuf(cmd.begin(), cmd.end());
    cmdBuf.push_back('\0');

    BOOL ok = CreateProcessA(
        nullptr,
        cmdBuf.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        workdir.empty() ? nullptr : workdir.c_str(),
        &si,
        &pi
    );

    ProcFile p;
    p.pi = pi;
    p.hFile = h;
    p.started = (ok != 0);
    if (!p.started)
    {
        CloseHandle(h);
        p.hFile = nullptr;
    }
    return p;
}

RunResult waitProcessFile(ProcFile& p, const std::string& outFile, DWORD timeoutMs)
{
    RunResult rr;
    if (!p.started)
    {
        rr.exitCode = 1;
        return rr;
    }

    const DWORD wait = WaitForSingleObject(p.pi.hProcess, timeoutMs);
    if (wait == WAIT_TIMEOUT)
    {
        TerminateProcess(p.pi.hProcess, 124);
        WaitForSingleObject(p.pi.hProcess, 2000);
        rr.exitCode = 124;
    }
    else
    {
        GetExitCodeProcess(p.pi.hProcess, &rr.exitCode);
    }

    if (p.hFile)
    {
        FlushFileBuffers(p.hFile);
        CloseHandle(p.hFile);
        p.hFile = nullptr;
    }

    CloseHandle(p.pi.hThread);
    CloseHandle(p.pi.hProcess);
    p.started = false;

    // Read file
    std::ifstream f(outFile, std::ios::binary);
    rr.out.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return rr;
}

std::string makeTempDir()
{
    namespace fs = std::filesystem;
    const fs::path base = fs::temp_directory_path() / "wizzmania_it";
    fs::create_directories(base);
    const auto t = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path dir = base / ("run_" + std::to_string(static_cast<long long>(t)));
    fs::create_directories(dir);
    return dir.string();
}

} // namespace

TEST_CASE("Integration - server + auth_tester (register/login/private)")
{
    namespace fs = std::filesystem;
    const std::string dir = exeDir();
    const std::string serverExe = (fs::path(dir) / "serveur.exe").string();
    const std::string testerExe = (fs::path(dir) / "auth_tester.exe").string();

    REQUIRE(fs::exists(serverExe));
    REQUIRE(fs::exists(testerExe));

    const std::string work = makeTempDir();

    Proc server = startServer(serverExe, work);
    REQUIRE(server.started);

    // Give the server time to bind/listen
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto run = [&](const std::vector<std::string>& args) {
        // Retry a bit if server isn't ready yet
        for (int attempt = 0; attempt < 5; ++attempt)
        {
            RunResult rr = runProcessCapture(testerExe, args, work, 10000);
            if (rr.exitCode == 0)
                return rr;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        return runProcessCapture(testerExe, args, work, 10000);
    };

    RunResult r1 = run({"register", "Alice", "pass123", "--listen-ms", "600"});
    REQUIRE(r1.exitCode == 0);
    REQUIRE(r1.out.find("AUTH_OK") != std::string::npos);

    RunResult r2 = run({"register", "Bob", "pass123", "--listen-ms", "600"});
    REQUIRE(r2.exitCode == 0);
    REQUIRE(r2.out.find("AUTH_OK") != std::string::npos);

    // Bob listens while Alice sends a private (run concurrently)
    const std::string bobOut = (fs::path(work) / "bob.out").string();
    ProcFile bobProc = startProcessToFile(testerExe,
                                          {"login", "Bob", "pass123", "--listen-ms", "4000"},
                                          work,
                                          bobOut);
    REQUIRE(bobProc.started);

    std::this_thread::sleep_for(std::chrono::milliseconds(350));

    RunResult alice = run({"login", "Alice", "pass123", "--send-private", "Bob", "Salut Bob!", "--listen-ms", "1200"});
    REQUIRE(alice.exitCode == 0);
    REQUIRE(alice.out.find("AUTH_OK") != std::string::npos);

    RunResult bob = waitProcessFile(bobProc, bobOut, 10000);
    REQUIRE(bob.exitCode == 0);
    REQUIRE(bob.out.find("AUTH_OK") != std::string::npos);
    REQUIRE(bob.out.find("[PRIVE]") != std::string::npos);

    stopServer(server);
}

