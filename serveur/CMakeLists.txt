# Nom du module
project(ServeurWizzMania LANGUAGES CXX)

# Fichiers source
file(GLOB_RECURSE SERVEUR_SOURCES
    src/*.cpp
)

# Création de l'exécutable serveur
add_executable(serveur
    ${SERVEUR_SOURCES}
)

# Includes
target_include_directories(serveur
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Librairie Windows pour Winsock
target_link_libraries(serveur
    ws2_32
)

message(STATUS "Serveur WIZZ Mania configuré")
