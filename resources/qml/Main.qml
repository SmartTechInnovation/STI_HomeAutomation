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
    width: 1200
    height: 720
    minimumWidth: 720
    minimumHeight: 500
    visible: true
    title: qsTr("Home Automation Config")

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
                        { icon_src: "qrc:/icons/page/page-text.svg"  , id_name: "btn_NP"  },
                        { icon_src: "qrc:/icons/file/folder-open.svg", id_name: "btn_OP" },
                        { icon_src: "qrc:/icons/action/save.svg"     , id_name: "btn_SP" },
                        { icon_src: "qrc:/icons/action/undo.svg"     , id_name: "btn_Undo"        },
                        { icon_src: "qrc:/icons/action/redo.svg"     , id_name: "btn_Redo"        },
                    ]
                    Button {
                        required property string icon_src
                        required property string id_name
                        id: id_topBtn
                        Layout.fillHeight: true
                        Layout.preferredWidth: 25
                        padding: 2
                        icon.source: icon_src

                        hoverEnabled: true

                        HoverHandler { cursorShape: Qt.PointingHandCursor }

                        background: Rectangle {
                            color: id_topBtn.down  ? Qt.darker(Theme.bgChrome, 1.35)  : id_topBtn.hovered ? Qt.darker(Theme.bgChrome, 1.18): "transparent"
                        }

                        onClicked: () =>{
                            if     (id_name === "btn_NP")   ProjectManager.newProject()
                            else if(id_name === "btn_OP")   folder_OpenProject.open()
                            else if(id_name === "btn_SP")   ProjectManager.saveProject()
                            else if(id_name === "btn_Undo") ProjectManager.undoProject()
                            else if(id_name === "btn_Redo") ProjectManager.redoProject()
                        }
                    }
                }

                FolderDialog {
                    id: folder_OpenProject
                    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
                    onAccepted: ProjectManager.openProject(selectedFolder)
                }

                Item { Layout.preferredWidth: 100 }

                Tab { // ==== Home Tab ====
                    active: ProjectManager.activeTabIndex === -1
                    closable: false
                    icon_src: "qrc:/icons/navigation/home.svg"

                    onSig_activate: {
                        ProjectManager.setActiveTab(-1)
                    }
                }

                // ====== Projects Tabs =====
                ListView {
                    id: list_OpenedProjects
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    orientation: ListView.Horizontal
                    clip:        true
                    spacing:     2
                    boundsBehavior: Flickable.StopAtBounds
                    model:       mainApp.openedTabs

                    ScrollBar.horizontal: ScrollBar {
                        policy: list_OpenedProjects.contentWidth > list_OpenedProjects.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }

                    delegate: Tab {
                        id: tab_Project
                        active:   ProjectManager.activeTabIndex === index
                        icon_src: "qrc:/icons/file/file-html.svg"

                        label: modelData.title
                        onSig_activate: {
                            ProjectManager.setActiveTab(index)
                        }
                        onSig_closeTab: ProjectManager.closeProject(index)
                    }
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
            }
        }
    }
    LogTab {
        id: log_Window
    }

}
