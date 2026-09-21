import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../Theme"
import "Parts/Controller"
import "Parts/ViewPort"
import "Parts/ViewPort/Properties"
import "Parts/ViewPort/Tree"
import "Parts/ViewPort/WorkPage"

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
                    Layout.fillWidth: true;
                    Layout.fillHeight: true

                    color: "transparent"

                    // ===== To be done =====
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

            ViewPort {
                SplitView.fillWidth: true
                SplitView.fillHeight: true
            }

            Properties {
                SplitView.preferredWidth: 200
            }
        }
    }
}
