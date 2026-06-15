cmake_minimum_required(VERSION 3.20)
project(AsciCalc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(SFML)

add_executable(AsciCalc
    "UI/main.cpp"
    "UI/Background.cpp"
    "UI/Button.cpp"
    "UI/CalculatorUI.cpp"
    "UI/InputScreen.cpp"
    "UI/layout/TextNode.cpp"
)

target_sources(AsciCalc PRIVATE
    "UI/Background.h"
    "UI/Button.h"
    "UI/CalculatorUI.h"
    "UI/CalculatorUITab.h"
    "UI/ITransformable.h"
    "UI/InputScreen.h"
    "UI/layout/IMathNode.h"
    "UI/layout/TextNode.h"
)

target_sources(AsciCalc
    PRIVATE
    FILE_SET CXX_MODULES FILES
    "src/core/context.ixx"
    "src/core/exceptions.ixx"
    "src/core/value.ixx"
    "src/math/rational.ixx"
    "src/parser/core.ixx"
    "src/parser/lexer.ixx"
)


add_custom_command(TARGET AsciCalc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/UI/assets"
    "$<TARGET_FILE_DIR:AsciCalc>/UI/assets"
)

add_custom_command(TARGET AsciCalc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/UI/shaders"
    "$<TARGET_FILE_DIR:AsciCalc>/UI/shaders"
)

target_link_libraries(AsciCalc PRIVATE SFML::Graphics SFML::Window SFML::System)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AsciCalc)
