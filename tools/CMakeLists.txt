project(ToolsWizzMania LANGUAGES CXX)

add_executable(auth_tester
    src/auth_tester.cpp
)

target_link_libraries(auth_tester
    PRIVATE
        commun
)

message(STATUS "Tools configurés (auth_tester)")

