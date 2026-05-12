cmake_minimum_required(VERSION 3.10)
project(SennaStudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Поиск зависимостей (убедись, что qtbase5- установлен)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# 1. Компилятор (предполагаем, что он лежит в подпапке SennaCompiler)
# Если папка называется иначе, поправь путь здесь:
# В файле SennaIDE/CMakeLists.txt
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SennaCompiler ${CMAKE_CURRENT_BINARY_DIR}/compiler_build)

# 2. Настройка путей для IDE и твоего Engine
include_directories(
    engine/include
    ide/include
    SennaCompiler/include
    SennaCompiler/src/graph_dump
)

# 3. Сборка IDE и твоего Engine (Разработчик 2)
add_executable(senna_ide
    ide/src/main.cpp            # Путь без лишних префиксов
    ide/src/main_window.cpp
    ide/src/code_editor.cpp
    ide/src/project_tree.cpp
    engine/src/analysis.cpp      # Твой файл (проверь инклюд внутри!)
    engine/src/data_bridge.cpp
    engine/src/visualizer.cpp

    ide/include/main_window.hpp
    ide/include/code_editor.hpp
    ide/include/project_tree.hpp
    engine/include/analysis.hpp
    engine/include/data_bridge.hpp
    engine/include/visualizer.hpp
)

# Линковка: IDE + Engine + динамическая библиотека компилятора + Qt
target_link_libraries(senna_ide
    senna_compiler               # Имя цели из SennaCompiler/CMakeLists.txt
    Qt5::Widgets
    pthread
)
