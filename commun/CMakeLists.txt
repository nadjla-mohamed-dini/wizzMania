project(CommunWizzMania LANGUAGES CXX)

# Récupération des fichiers source
file(GLOB_RECURSE COMMUN_SOURCES
    src/*.cpp
)

# Création de la bibliothèque statique
add_library(commun STATIC
    ${COMMUN_SOURCES}
)

# Includes publics (pour serveur et client)
target_include_directories(commun
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Librairie Winsock
target_link_libraries(commun
    ws2_32
)

message(STATUS "Bibliothèque commun configurée")
