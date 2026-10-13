cmake_minimum_required(VERSION 3.16)

project(STIx_Config VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml QuickControls2 Svg Xml)

# Ajusteaza la versiunea de Qt chiar instalata (proiectul e compilat pe 6.4)
qt_standard_project_setup(REQUIRES 6.11)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appSTIx_Config)

target_compile_definitions(appSTIx_Config PRIVATE
    PROJECT_VERSION_STR="${PROJECT_VERSION}"
)

target_include_directories(appSTIx_Config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_qml_module(appSTIx_Config
    URI STI_HomeAutomation
    QML_FILES
        resources/qml/Main.qml
        resources/qml/Components/Theme/Theme.qml
        resources/qml/Components/Tab/Tab.qml
        resources/qml/Components/Tab/TabStrip.qml
        resources/qml/Components/Logger/LogTab.qml
        resources/qml/Components/Pages/HomePage/HomePage.qml
        resources/qml/Components/Pages/HomePage/Parts/SearchBox.qml
        resources/qml/Components/Pages/EditorPage/EditorPage.qml
        resources/qml/Components/Pages/EditorPage/Parts/Controller/ControllerConnection.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Project.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Properties/Properties.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Tree/ProjectTree.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Page.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/Block.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/Port.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/DisplayConnections.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/CheckBoxCustom.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/OutputSignal.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/Parts/InputSignal.qml
        resources/qml/Components/Pages/EditorPage/Parts/Topbar/TopToolBar.qml
        resources/qml/Components/Pages/EditorPage/Parts/Topbar/ToolBtn.qml
        resources/qml/Components/Pages/EditorPage/Parts/Topbar/BlocksMenu.qml
        resources/qml/Components/Pages/EditorPage/Parts/Topbar/ShortcutsMenu.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/PageFrame.qml
        resources/qml/Components/Pages/EditorPage/Parts/Project/Pages/PageCover.qml
    SOURCES
        src/App/main.cpp
        src/Setup/Setup.hpp
        src/App/application.h
        src/App/application.cpp
        src/Components/Project/project.h
        src/Components/Project/project.cpp
        src/Components/Project/project_tokens.h
        src/Components/Project/project_commands.h
        src/Components/Project/Tree/projecttree.h
        src/Components/Project/Tree/projecttree.cpp
        src/Components/Project/Page/page.h
        src/Components/Project/Page/page.cpp
        src/Components/ProjectManager/projectmanager.h
        src/Components/ProjectManager/projectmanager.cpp
        src/Components/BlockManager/blockmanager.h
        src/Components/BlockManager/blockmanager.cpp
        src/Components/BlockManager/Block/blockbase.h
        src/Components/BlockManager/Block/blockbase.cpp
        src/Components/BlockManager/Block/Ports/port.h
        src/Components/BlockManager/Block/Ports/port.cpp
        src/Components/SchemaManager/schemamanager.h
        src/Components/SchemaManager/schemamanager.cpp
        src/Components/Document/document_tokens.h
        src/Components/Document/xmlnode.h
        src/Components/Document/xmlnode.cpp
        src/Components/Logger/logger.h
        src/Components/Logger/logger.cpp

    RESOURCES
        resources/qml/Components/Theme/qmldir
)

# Theme.qml e singleton — calea trebuie sa fie EXACT ca in QML_FILES de mai sus
set_source_files_properties(
    resources/qml/Components/Theme/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
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

target_link_libraries(appSTIx_Config
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::Qml
    PRIVATE Qt6::Svg
    PRIVATE Qt6::Xml
)

install(TARGETS appSTIx_Config
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)