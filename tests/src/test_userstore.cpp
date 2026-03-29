#include "serveur/UserStore.hpp"
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <filesystem>
#include <string>

static std::string tempFilePath(const std::string& name)
{
    namespace fs = std::filesystem;
    const fs::path base = fs::temp_directory_path() / "wizzmania_tests";
    fs::create_directories(base);
    return (base / name).string();
}

TEST_CASE("UserStore - register/verify/persist")
{
    const std::string file = tempFilePath("users_test.db");
    std::filesystem::remove(file);

    UserStore store(file);

    std::string err;
    REQUIRE(store.registerUser("Alice", "pass123", err));
    REQUIRE(err.empty());
    REQUIRE_FALSE(store.registerUser("Alice", "pass123", err));
    REQUIRE_FALSE(err.empty());

    REQUIRE(store.verifyUser("Alice", "pass123"));
    REQUIRE_FALSE(store.verifyUser("Alice", "wrong"));
    REQUIRE_FALSE(store.verifyUser("Bob", "pass123"));

    // Persistence
    UserStore store2(file);
    REQUIRE(store2.verifyUser("Alice", "pass123"));
    auto users = store2.listUsers();
    REQUIRE(std::find(users.begin(), users.end(), "Alice") != users.end());
}

