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

    // ===== Background =====
    Rectangle {
        anchors.fill: parent
        color:        Theme.bgCanvas
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

                TopToolBar {
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    project: root.project

                    onBlockPicked:       (uuid) => projectView.addBlockAtCenter(uuid)
                    onContextPicked:     (uuid) => {
                        if(root.project && root.project.selection){
                            var iUuid = root.project.selection.instance.iUuid
                            root.project.cmdAddChild(iUuid, uuid)
                        }
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
                SplitView.preferredWidth: 240
                onContextItemPicked: (iUuid, item) => root.runContextItem(iUuid, item)
                project: root.project
            }

            Project {
                id: projectView
                SplitView.fillWidth:  true
                SplitView.fillHeight: true
                project: root.project
            }

            Properties {
                SplitView.preferredWidth: 300
                project: root.project
            }
        }
    }
}
