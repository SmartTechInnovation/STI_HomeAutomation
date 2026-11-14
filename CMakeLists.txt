cmake_minimum_required(VERSION 3.16)

project(STIx_Config VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(STIX_BUILD_TESTS "Build the unit and integration tests" ON)

set(STIX_QT_COMPONENTS Core Gui Quick Qml QuickControls2 Svg Xml)
if(STIX_BUILD_TESTS)
    list(APPEND STIX_QT_COMPONENTS Test)
endif()
find_package(Qt6 REQUIRED COMPONENTS ${STIX_QT_COMPONENTS})

# Adjust to the Qt version actually installed
qt_standard_project_setup(REQUIRES 6.11)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

# =============================================================================
# Auto-discovery (story 1.1): new sources, QML files and tests are picked up
# automatically, so later stories never have to edit this file.
# Re-run CMake configure after adding a file (CONFIGURE_DEPENDS does it for you).
# =============================================================================

# ---- stix_core: layers L1-L3 (no QtQuick), shared by the app and the tests ----
file(GLOB_RECURSE STIX_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Components/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Components/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Components/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Setup/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.h"
)
add_library(stix_core STATIC
    ${STIX_CORE_SOURCES}
    src/App/application.h
    src/App/application.cpp
)
target_include_directories(stix_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(stix_core PUBLIC PROJECT_VERSION_STR="${PROJECT_VERSION}")
target_link_libraries(stix_core PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Svg
    Qt6::Xml
)

# ---- The application: entry point + QML module (layer L4) ----
qt_add_executable(appSTIx_Config
    src/App/main.cpp
)

file(GLOB_RECURSE STIX_QML_FILES CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/qml/*.qml"
)
list(SORT STIX_QML_FILES)

# Theme.qml is a singleton - the path must be EXACTLY as in QML_FILES below
set_source_files_properties(
    resources/qml/Components/Theme/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appSTIx_Config
    URI STI_HomeAutomation
    QML_FILES
        ${STIX_QML_FILES}
    RESOURCES
        resources/qml/Components/Theme/qmldir
)

qt_add_resources(appSTIx_Config_res
    resources/icons/Icons.qrc
)

target_sources(appSTIx_Config PRIVATE
    ${appSTIx_Config_res}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appSTIx_Config PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appSTIx_Config
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_QML_CONTROLS_STYLE Basic
)

target_link_libraries(appSTIx_Config PRIVATE
    stix_core
    Qt6::Quick
    Qt6::QuickControls2
)

# ---- Optional build modules: one file per concern (warnings, translations, zip, crypto…) ----
file(GLOB STIX_CMAKE_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/*.cmake")
list(SORT STIX_CMAKE_MODULES)
foreach(stix_module IN LISTS STIX_CMAKE_MODULES)
    include(${stix_module})
endforeach()

# ---- Tools (e.g. tools/stix-mock-controller) and the SDK tests, when present ----
file(GLOB STIX_TOOL_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/tools/*")
foreach(stix_tool_dir IN LISTS STIX_TOOL_DIRS)
    if(IS_DIRECTORY ${stix_tool_dir} AND EXISTS ${stix_tool_dir}/CMakeLists.txt)
        add_subdirectory(${stix_tool_dir})
    endif()
endforeach()

# ---- Tests ----
if(STIX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tests/CMakeLists.txt)
        add_subdirectory(sdk/tests)
    endif()
endif()

install(TARGETS appSTIx_Config
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
