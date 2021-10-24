cmake_minimum_required(VERSION 3.5)

project(picture_analysis VERSION 0.1 LANGUAGES CXX)

if (WIN32)
  set(CMAKE_C_COMPILER C:/Qt/Tools/mingw810_64/bin/gcc.exe)
  set(CMAKE_CXX_COMPILER C:/Qt/Tools/mingw810_64/bin/g++.exe)
endif()

if (UNIX)
  set(CMAKE_C_COMPILER /usr/bin/gcc)
  set(CMAKE_CXX_COMPILER /usr/bin/gcc)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/network_functions.cpp
        src/mainwindow.ui
        include/mainwindow.h
        include/network_functions.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 5)
    qt_add_executable(picture_analysis
        MANUAL_FINALIZATION
        ${SOURCES}
    )
endif()

target_link_libraries(picture_analysis PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(picture_analysis PRIVATE Qt${QT_VERSION_MAJOR}::Network)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(picture_analysis)
endif()
