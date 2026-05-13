cmake_minimum_required(VERSION 3.10)
project(SennaStudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)

add_subdirectory(SennaCompiler compiler_build)

include_directories(
    engine/include
    ide/include
)

set(IDE_SOURCES
    ide/src/main.cpp
    ide/src/main_window.cpp
    ide/src/code_editor.cpp
    ide/src/project_tree.cpp
    engine/src/analysis.cpp
    engine/src/data_bridge.cpp
    engine/src/visualizer.cpp
)

set(IDE_HEADERS
    ide/include/main_window.hpp
    ide/include/code_editor.hpp
    ide/include/project_tree.hpp
    engine/include/analysis.hpp
    engine/include/data_bridge.hpp
    engine/include/visualizer.hpp
)

add_executable(senna_ide ${IDE_SOURCES} ${IDE_HEADERS})

target_link_libraries(senna_ide
    PRIVATE
    senna_compiler
    Qt5::Widgets
    Qt5::Svg
    pthread
)
