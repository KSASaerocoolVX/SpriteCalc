cmake_minimum_required(VERSION 3.28)
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
 "UI/layout/FractionNode.cpp")

target_sources(AsciCalc
    PRIVATE
    FILE_SET CXX_MODULES FILES
    
    "src/core/context.ixx"
    "src/core/exceptions.ixx"
    "src/core/value.ixx"
    "src/math/rational.ixx"
    "src/parser/core.ixx"
    "src/parser/lexer.ixx"
    
    "UI/ITransformable.ixx"
    "UI/Background.ixx"
    "UI/Button.ixx"
    "UI/InputScreen.ixx"
    "UI/CalculatorUI.ixx"
    "UI/CalculatorUITab.ixx"
    
    "UI/layout/IMathNode.ixx"
    "UI/layout/TextNode.ixx"
    "UI/layout/FractionNode.ixx"
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
