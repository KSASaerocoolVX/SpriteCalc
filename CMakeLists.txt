cmake_minimum_required(VERSION 3.20)
project(SpriteCalc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(SFML)

add_executable(SpriteCalc UI/main.cpp)

target_link_libraries(SpriteCalc PRIVATE SFML::Graphics SFML::Window SFML::System)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SpriteCalc)
