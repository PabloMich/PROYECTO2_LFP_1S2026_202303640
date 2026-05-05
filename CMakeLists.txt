cmake_minimum_required(VERSION 3.16)
project(TaskScript)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(TaskScript
        src/main.cpp
        src/Token.h
        src/Token.cpp
        src/LexicalAnalyzer.h
        src/LexicalAnalyzer.cpp
        src/ErrorManager.h
        src/ErrorManager.cpp
        src/SyntaxAnalyzer.cpp
        src/SyntaxAnalyzer.h
        src/MainWindow.h
        src/MainWindow.cpp
        src/BoardData.cpp
        src/BoardData.h
        src/ReportGenerator.cpp
        src/ReportGenerator.h
)

target_link_libraries(TaskScript Qt6::Widgets)