project(ServeurWizzMania LANGUAGES CXX)

file(GLOB_RECURSE SERVEUR_SOURCES
    src/*.cpp
)

add_executable(serveur
    ${SERVEUR_SOURCES}
)

target_include_directories(serveur
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(serveur
    commun
)

message(STATUS "Serveur lié à la bibliothèque commun")
