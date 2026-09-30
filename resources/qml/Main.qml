import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import "Components/Theme"
import "Components/Tab"
import "Components/Pages/HomePage"
import "Components/Pages/EditorPage"
import "Components/Logger"

import STI.ProjectManager

ApplicationWindow {
    id: mainApp
    width:  1280
    height: 800
    minimumWidth: 720
    minimumHeight: 500
    visible: true
    title: qsTr("Home Automation Config")

    property var activeProject:  null
    property var recentProjects: ProjectManager.recentProjects
    property var openedTabs:     ProjectManager.openedTabs

    readonly property bool prop_ro_Maximized: mainApp.visibility === Window.Maximized

    Shortcut { sequences: [StandardKey.New];  onActivated: ProjectManager.newProject()  }
    Shortcut { sequences: [StandardKey.Open]; onActivated: folder_OpenProject.open()    }
    Shortcut { sequences: [StandardKey.Save]; onActivated: ProjectManager.saveProject() }
    Shortcut { sequences: [StandardKey.Undo]; onActivated: ProjectManager.undoProject() }
    Shortcut { sequences: [StandardKey.Redo]; onActivated: ProjectManager.redoProject() }

    // ========= Window Content ==========
    ColumnLayout { //Vertical layout
        anchors.fill: parent
        spacing: 0

        // ========= Title Bar ==========
        Rectangle {
            Layout.fillWidth:  true
            Layout.preferredHeight: 25
            color: Theme.bgChrome

            // ======== Top Menu ========
            RowLayout {
                anchors.fill: parent
                spacing: 0

                // ====== Burger Menu ======
                Button {
                    id: btn_BurgerMenu
                    Layout.fillHeight: true
                    Layout.preferredWidth: 40
                    padding: 2
                    icon.source: "qrc:/icons/navigation/menu-burger.svg"

                    hoverEnabled: true

                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    background: Rectangle {
                        color: btn_BurgerMenu.down  ? Qt.darker(Theme.bgChrome, 1.35)  : btn_BurgerMenu.hovered ? Qt.darker(Theme.bgChrome, 1.18): "transparent"
                    }
                }

                // ====== Separtor ======
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                    color: Theme.gray
                }

                // ===== Shortcuts ======

                Repeater {
                    model: [
                        { icon_src: "qrc:/icons/page/page-text.svg"  , id_name: "btn_NP"   },
                        { icon_src: "qrc:/icons/file/folder-open.svg", id_name: "btn_OP"   },
                        { icon_src: "qrc:/icons/action/save.svg"     , id_name: "btn_SP"   },
                        { icon_src: "qrc:/icons/action/undo.svg"     , id_name: "btn_Undo" },
                        { icon_src: "qrc:/icons/action/redo.svg"     , id_name: "btn_Redo" },
                    ]
                    Button {
                        id: id_topBtn
                        Layout.fillHeight: true
                        Layout.preferredWidth: 25
                        padding: 2
                        icon.source: modelData.icon_src
                        icon.width:  16
                        icon.height: 16

                        enabled: {
                            switch(modelData.id_name){
                                case "btn_SP":   return mainApp.activeProject !== null;
                                case "btn_Undo": return mainApp.activeProject !== null && mainApp.activeProject.canUndo;
                                case "btn_Redo": return mainApp.activeProject !== null && mainApp.activeProject.canRedo;
                            }
                            return true;
                        }

                        opacity: enabled ? 1.0 : 0.35

                        hoverEnabled: true

                        HoverHandler { cursorShape: Qt.PointingHandCursor }

                        background: Rectangle {
                            color: id_topBtn.down  ? Qt.darker(Theme.bgChrome, 1.35)  : id_topBtn.hovered ? Qt.darker(Theme.bgChrome, 1.18): "transparent"
                        }

                        onClicked: () =>{
                            switch(modelData.id_name){
                                case "btn_NP":   ProjectManager.newProject();  break
                                case "btn_OP":   folder_OpenProject.open();    break
                                case "btn_SP":   ProjectManager.saveProject(); break
                                case "btn_Undo": ProjectManager.undoProject(); break
                                case "btn_Redo": ProjectManager.redoProject(); break
                            }
                        }

                        ToolTip.visible: hovered
                        ToolTip.delay:   600
                        ToolTip.text: {
                            switch(modelData.id_name) {
                                case "btn_NP":   return qsTr("New Project")
                                case "btn_OP":   return qsTr("Open project")
                                case "btn_SP":   return qsTr("Save")
                                case "btn_Undo": return qsTr("Undo")
                                case "btn_Redo": return qsTr("Redo")
                            }
                        }
                    }
                }

                FolderDialog {
                    id: folder_OpenProject
                    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
                    onAccepted: ProjectManager.openProject(selectedFolder)
                }

                Item { Layout.preferredWidth: 55 }

                Item {
                    Layout.fillHeight:     true
                    Layout.preferredWidth: 46
                    Layout.leftMargin:     8
                    Layout.topMargin:      5

                    Tab {
                        anchors.fill: parent
                        active:   ProjectManager.activeTabIndex === -1
                        hovered:  ma_HomeTab.containsMouse
                        closable:  false
                        separator: false
                        icon: "qrc:/icons/navigation/home.svg"
                    }
                    MouseArea {
                        id: ma_HomeTab
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape:  Qt.PointingHandCursor
                        onPressed:    ProjectManager.setActiveTab(-1)
                    }
                }

                // ====== Projects Tabs =====
                TabStrip {
                    id: strip_Projects
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    Layout.topMargin:  5

                    model:        mainApp.openedTabs
                    currentIndex: ProjectManager.activeTabIndex
                    defaultIcon:  "qrc:/icons/file/file-html.svg"
                    showAdd:      true
                    addToolTip:   qsTr("New project")
                    leadingSeparator: ProjectManager.activeTabIndex !== -1

                    onActivated:      (index) => ProjectManager.setActiveTab(index)
                    onCloseRequested: (index) => ProjectManager.closeProject(index)
                    onMoved:          (from, to) => ProjectManager.moveTab(from, to)
                    onAddRequested:   ProjectManager.newProject()
                }
            }
        }

        // ====== Workspace Pages =====
        Item {
            id: workspaceArea
            Layout.fillWidth:  true
            Layout.fillHeight: true

            HomePage {
                anchors.fill: parent
                ref_RecentProjects: mainApp.recentProjects
                visible:      ProjectManager.activeTabIndex === -1

                onNewProject:  ProjectManager.newProject()
                onOpenFolder: folder_OpenProject.open()
                onOpenProject: (path) => ProjectManager.openProject(path)
            }

            EditorPage {
                anchors.fill: parent
                visible:      ProjectManager.activeTabIndex >= 0
                project:      ProjectManager.activeProject
            }
        }
    }
    LogTab {
        id: log_Window
    }

}
