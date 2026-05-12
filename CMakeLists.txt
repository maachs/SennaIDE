cmake_minimum_required(VERSION 3.10)
project(SennaIDE)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(../compiler ${CMAKE_CURRENT_BINARY_DIR}/compiler_build)

include_directories(
    ../include
    ../engine/include
    ${CMAKE_CURRENT_BINARY_DIR}/compiler_build
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(senna_ide
    main_ide.cpp
    ../engine/src/data_bridge.cpp
    ../engine/src/visualizer.cpp
    ../engine/src/analysis_worker.cpp
)

target_link_libraries(senna_ide
    senna_compiler
    Qt5::Widgets
    pthread
)
