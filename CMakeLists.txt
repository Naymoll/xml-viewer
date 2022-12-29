cmake_minimum_required(VERSION 3.5)

project(xml-viewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets Xml Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Sql REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        models/codeeditortablemodel.cpp
        models/codeeditortablemodel.h

        data/codeeditor.h

        dialogs/progressdialog.cpp
        dialogs/progressdialog.h
        dialogs/progressdialog.ui

        dialogs/codeeditordialog.cpp
        dialogs/codeeditordialog.h
        dialogs/codeeditordialog.ui

        workers/databaseworker.cpp
        workers/databaseworker.h
        workers/xmlworker.cpp
        workers/xmlworker.h

        recources.qrc
)

add_executable(xml-viewer ${PROJECT_SOURCES})

target_link_libraries(xml-viewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::Sql)

set_target_properties(xml-viewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
