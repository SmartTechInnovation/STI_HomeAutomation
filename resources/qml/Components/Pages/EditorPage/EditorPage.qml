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

Item {
    id: root

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

                    //FunctionBlocks {
                    //    anchors.fill: parent

                    //    onBlockRequested: (category, type, title) => {
                    //        console.info("Add block:", category, "/", type, "(" + title + ")")
                    //    }
                    //}
                    TopToolBar {
                        anchors.fill: parent
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
                SplitView.preferredWidth: 200
            }

            Project {
                SplitView.fillWidth: true
                SplitView.fillHeight: true
            }

            Properties {
                SplitView.preferredWidth: 200
            }
        }
    }
}
