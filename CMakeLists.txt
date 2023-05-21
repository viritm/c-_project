cmake_minimum_required(VERSION 3.5)

project(todo-list VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем ресурсы в проект
set(RESOURCES
    resources.qrc
)
set(ICON_FILES todo-list.rc)


find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        todo-list.rc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(todo-list WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RESOURCES} # Добавляем ресурсы к сборке
        todo-list.rc # Добавляем файл ресурсов с иконкой
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET todo-list APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(todo-list SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(todo-list
            ${PROJECT_SOURCES}
            ${RESOURCES} # Добавляем ресурсы к сборке
            todo-list.rc # Добавляем файл ресурсов с иконкой
        )
    endif()
endif()

target_sources(todo-list PRIVATE ${ICON_FILES})

target_link_libraries(todo-list PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

#Добавляем библиотеку sqlite3 и подключаем к проекту
#set(CMAKE_PREFIX_PATH "C:/vcpkg-master/packages/sqlite3_x64-windows")
#find_package(SQLite3 REQUIRED)
#target_link_libraries(todo-list PRIVATE SQLite::SQLite3)

#set(CMAKE_PREFIX_PATH "C:/Qt/6.4.3/mingw_64/lib/cmake")


find_package(Qt6 COMPONENTS Sql REQUIRED)
target_link_libraries(todo-list PRIVATE Qt6::Sql)



set_target_properties(todo-list PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS todo-list
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(todo-list)
endif()
