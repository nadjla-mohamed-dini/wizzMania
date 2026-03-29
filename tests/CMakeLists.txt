cmake_minimum_required(VERSION 3.20)

project(TestsWizzMania LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(catch2)

add_executable(tests_wizzmania
  src/test_message.cpp
  src/test_userstore.cpp
  src/test_integration.cpp
  ../serveur/src/UserStore.cpp
)

target_link_libraries(tests_wizzmania
  PRIVATE
    commun
    Catch2::Catch2WithMain
)

target_include_directories(tests_wizzmania
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/serveur/include
)

message(STATUS "Tests WIZZ Mania configurés (Catch2)")