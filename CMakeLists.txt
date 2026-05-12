cmake_minimum_required(VERSION 3.10)
project(SennaStudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt5 COMPONENTS Svg REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SennaCompiler ${CMAKE_CURRENT_BINARY_DIR}/compiler_build)

include_directories(
    engine/include
    ide/include
    SennaCompiler/include
    SennaCompiler/src/graph_dump
)

add_executable(senna_ide
    ide/src/main.cpp
    ide/src/main_window.cpp
    ide/src/code_editor.cpp
    ide/src/project_tree.cpp
    engine/src/analysis.cpp
    engine/src/data_bridge.cpp
    engine/src/visualizer.cpp

    ide/include/main_window.hpp
    ide/include/code_editor.hpp
    ide/include/project_tree.hpp
    engine/include/analysis.hpp
    engine/include/data_bridge.hpp
    engine/include/visualizer.hpp
)

target_link_libraries(senna_ide
    senna_compiler
    Qt5::Svg
    pthread
)
