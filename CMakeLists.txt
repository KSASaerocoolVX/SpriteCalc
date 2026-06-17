cmake_minimum_required(VERSION 3.28)

project(SpriteCalc LANGUAGES CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(SFML)

add_library(SpriteCalcCore
    src/calc/ExpressionEvaluator.cpp
    src/app/SelfTest.cpp
)

target_include_directories(SpriteCalcCore
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(SpriteCalc
    src/main.cpp
    src/app/Repl.cpp
)

target_link_libraries(SpriteCalc
    PRIVATE
        SpriteCalcCore
)

add_executable(SpriteCalcGui
    UI/main.cpp
    UI/Background.cpp
    UI/Button.cpp
    UI/CalculatorUI.cpp
    UI/InputScreen.cpp
    UI/layout/TextNode.cpp
    UI/layout/FractionNode.cpp
)

target_link_libraries(SpriteCalcGui
    PRIVATE
        SpriteCalcCore
        SFML::Graphics
        SFML::Window
        SFML::System
)

add_executable(SpriteCalcTests
    tests/expression_evaluator_tests.cpp
)

target_link_libraries(SpriteCalcTests
    PRIVATE
        SpriteCalcCore
)

enable_testing()
add_test(NAME ExpressionEvaluatorTests COMMAND SpriteCalcTests)

foreach(asset_dir IN ITEMS assets shaders)
    add_custom_command(TARGET SpriteCalcGui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/UI/${asset_dir}"
            "$<TARGET_FILE_DIR:SpriteCalcGui>/UI/${asset_dir}"
        VERBATIM
    )
endforeach()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SpriteCalc)
