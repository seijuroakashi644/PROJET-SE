cmake_minimum_required(VERSION 3.5)

project(PROJET_SE VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trouve Qt (Widgets + Charts)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        process.h
        process.cpp
        statistics.h
        algorithmbase.h
        algorithmbase.cpp
        fcfs.h
        fcfs.cpp
        sjf.h
        sjf.cpp
        round_robin.h
        round_robin.cpp
        round_robin_multiniveaux.h
        round_robin_multiniveaux.cpp
        edf.h
        edf.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PROJET_SE
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(PROJET_SE SHARED ${PROJECT_SOURCES})
    else()
        add_executable(PROJET_SE ${PROJECT_SOURCES})
    endif()
endif()

# 🔹 LIAISON ICI (APRÈS add_executable)
target_link_libraries(PROJET_SE PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

set_target_properties(PROJET_SE PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PROJET_SE)
endif()
