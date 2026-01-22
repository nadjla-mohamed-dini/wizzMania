project(ClientWizzMania LANGUAGES CXX)

file(GLOB_RECURSE CLIENT_SOURCES
    src/*.cpp
)

add_executable(client
    ${CLIENT_SOURCES}
)

target_include_directories(client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(client
    commun
)

message(STATUS "Client WIZZ Mania configuré")
