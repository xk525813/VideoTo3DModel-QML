cmake_minimum_required(VERSION 3.20)

project(VideoTo3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt 6 路径
set(QT_PATH "/home/myTool/Qt/6.10.3/gcc_64")
set(CMAKE_PREFIX_PATH "${QT_PATH}" ${CMAKE_PREFIX_PATH})

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    Core
    Qml
    Gui
    Widgets
    Concurrent
    OpenGL
)

add_subdirectory(src)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()
