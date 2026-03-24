cmake_minimum_required(VERSION 3.20)

project(WizzMania VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(WIZZMANIA_BUILD_GUI "Build Qt6 GUI client (requires Qt6)" OFF)
option(WIZZMANIA_BUILD_TOOLS "Build helper CLI tools" ON)

add_subdirectory(commun)
add_subdirectory(serveur)
add_subdirectory(client)
add_subdirectory(tests)
if (WIZZMANIA_BUILD_GUI)
    add_subdirectory(gui)
endif()
if (WIZZMANIA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

message(STATUS "Projet WIZZ Mania configuré avec succès")

message(STATUS "Standard C++ : ${CMAKE_CXX_STANDARD}")