cmake_minimum_required(VERSION 3.5)

project(CS307ParsingGUIOfficial VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk" CACHE STRING "" FORCE )

find_package(PythonInterp 3.12 REQUIRED)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR}  REQUIRED COMPONENTS PrintSupport)
find_package(Qt${QT_VERSION_MAJOR}  REQUIRED COMPONENTS Pdf)

set(TS_FILES CS307ParsingGUIOfficial_en_US.ts)

set(PROJECT_SOURCES
    mainGUI.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        cloud.hpp
        ${TS_FILES}
        file_operations.cpp
        file_operations.hpp

        config.hpp
        json.hpp
        parsing.cpp
        parsing.h
        printer.cpp
        printer.hpp
        findPlaceholders.py
        modify_docx.py
        replace_helper.py
        helpdialog.cpp
        helpdialog.h



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

qt_add_executable(CS307ParsingGUIOfficial
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
        templates.h templates.cpp templates.ui
        nlohmann/json.hpp nlohmann/json_fwd.hpp

        tagmanager.h tagmanager.cpp
        customfilesystemmodel.h customfilesystemmodel.cpp
        ViewDocx.py
        replacementsHelper.h
        replacementsHelper.cpp
        ExportDoc.py
        WatermarkDoc.py



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CS307ParsingGUIOfficial APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CS307ParsingGUIOfficial SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CS307ParsingGUIOfficial
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(CS307ParsingGUIOfficial PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CS307ParsingGUIOfficial PRIVATE Qt6::PrintSupport)
target_link_libraries(CS307ParsingGUIOfficial PRIVATE Qt6::Pdf)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CS307ParsingGUIOfficial)
endif()
set_target_properties(CS307ParsingGUIOfficial PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CS307ParsingGUIOfficial
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CS307ParsingGUIOfficial)
endif()
