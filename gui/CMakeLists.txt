project(GuiWizzMania LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(wizz_gui
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
    src/ChatClient.cpp
    src/ChatClient.hpp
)

target_link_libraries(wizz_gui
    PRIVATE
        Qt6::Widgets
        Qt6::Network
        commun
)

set_target_properties(wizz_gui PROPERTIES
    WIN32_EXECUTABLE TRUE
)

message(STATUS "GUI Qt6 configurée (wizz_gui)")

