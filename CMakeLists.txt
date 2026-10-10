cmake_minimum_required(VERSION 3.16)

project(QmlForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Qml Quick QuickWidgets)

set(SOURCES
    main.cpp
    MainWindow.h
    MainWindow.cpp
    CodeEditor.h
    CodeEditor.cpp
    PreviewPane.h
    PreviewPane.cpp
    ConsolePane.h
    ConsolePane.cpp
    QmlHighlighter.h
    QmlHighlighter.cpp
    resources.qrc
)

if(WIN32)
    list(APPEND SOURCES QmlForge.rc)
endif()

qt_add_executable(QmlForge ${SOURCES})

target_compile_definitions(QmlForge PRIVATE QMLFORGE_VERSION="${PROJECT_VERSION}")

target_link_libraries(QmlForge PRIVATE
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
)

set_target_properties(QmlForge PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE TRUE
)

install(TARGETS QmlForge
    RUNTIME DESTINATION bin
)
