import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../Theme"
import "Parts/Controller"
import "Parts/Topbar"
import "Parts/Project"
import "Parts/Project/Properties"
import "Parts/Project/Tree"
import "Parts/Project/Pages"

import STI.ProjectManager

Item {
    id: root

    property var    project:        null
    property string selectionUuid:  project ? project.selectedUuid  : ""
    property string selectionIUuid: project ? project.selectedIUuid : ""

    onProjectChanged: {
        project.selectedUuid  = ""
        project.selectedIUuid = ""
    }

    function treeSelected(uuid, iUuid){
        selectionUuid  = uuid;
        selectionIUuid = iUuid
        if(!project) return
        project.selectedUuid  = uuid
        project.selectedIUuid = iUuid
    }

    // ===== Background =====
    Rectangle {
        anchors.fill: parent
        color:        Theme.bgCanvas

        TapHandler {
            onTapped: root.forceActiveFocus()
        }
    }

    // ===== Editor Workspace ====
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ===== Top ToolBar ====
        Rectangle {
            id: top_Toolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 100

            color: "transparent"

            RowLayout{
                anchors.fill: parent
                spacing: 0

                ControllerConnection {
                    Layout.preferredWidth: 266
                    Layout.fillHeight: true
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: Theme.gray
                }

                Rectangle {
                    id: topMenu
                    Layout.fillWidth:  true
                    Layout.fillHeight: true

                    color: "transparent"

                    TopToolBar {
                        anchors.fill: parent
                        project: root.project
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.gray
        }

        SplitView {
            Layout.fillWidth:  true
            Layout.fillHeight: true

            ProjectTree {
                id: projectTree
                SplitView.preferredWidth: 200
                project: root.project
                onSelected: (uuid, iUuid) => root.treeSelected(uuid, iUuid)
            }

            Project {
                id: projectView
                SplitView.fillWidth: true
                SplitView.fillHeight: true

                project: root.project
            }

            Properties {
                SplitView.preferredWidth: 200
            }
        }
    }
}
