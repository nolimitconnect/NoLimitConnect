cmake_minimum_required(VERSION 3.16.3)

#=== project ===

project(Assembly1234TestApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Svg Widgets)

qt_add_executable(Assembly1234TestApp
    MANUAL_FINALIZATION
    mainwindow.h
    mainwindow.cpp
    main.cpp
    mainwindow.ui
)

if(DEFINED TARGET_OS_ANDROID)
    set_target_properties(Assembly1234TestApp PROPERTIES
        ANDROID_TARGET_ARCH arm64-v8a
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../android"
    )
else()
    set_target_properties(Assembly1234TestApp PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )
endif()

target_include_directories(Assembly1234TestApp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Assembly1234TestLib
)
target_link_libraries(Assembly1234TestApp PUBLIC
    Assembly1234TestLib
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Svg
)

qt_finalize_target(Assembly1234TestApp)
