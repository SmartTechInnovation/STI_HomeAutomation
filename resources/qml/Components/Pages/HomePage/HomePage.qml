import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../Theme"
import "Parts"

Item {
    id: root

    signal newProject ()
    signal openProject(url projPath)
    signal openFolder()
    signal openMyControllers()
    signal searchLocalNetwork()
    signal connect()
    signal firmwareManager()
    signal flashHardware()

    required property var ref_RecentProjects

    component SectionLabel: Label {
        color:          Theme.white
        font.bold:      true
        font.pixelSize: 14
        bottomPadding:  3
    }

    component MenuButton: Button {
        width:        left_Menu.width
        padding:      6
        hoverEnabled: true

        contentItem: Text {
            text:                parent.text
            font:                parent.font
            color:               Theme.white
            horizontalAlignment: Text.AlignLeft
            verticalAlignment:   Text.AlignVCenter
            elide:               Text.ElideRight
        }

        HoverHandler { cursorShape: Qt.PointingHandCursor }

        background: Rectangle {
            radius: 5
            color:  parent.down    ? Qt.darker(Theme.bgChrome, 1.35)
                  : parent.hovered ? Qt.darker(Theme.bgChrome, 1.18)
                  : "transparent"
        }
    }

    // ===== Background =====
    Rectangle {
        anchors.fill: parent
        color:        Theme.bgCanvas

        TapHandler {
            onTapped: root.forceActiveFocus()
        }
    }

    // ===== Workspace Layout =======
    RowLayout {
        anchors.fill: parent
        spacing:      0

        // ===== Left Menu =====
        Rectangle {
            id: left_Menu
            Layout.preferredWidth: 186
            Layout.fillHeight:     true
            Layout.margins:        40
            color:                 "transparent"

            Column {
                SectionLabel {
                    text:       qsTr("PROJECT")
                }
                MenuButton {
                    text: qsTr("My Projects")
                }
                MenuButton {
                    text: qsTr("New Projects")
                    onClicked: newProject()
                }
                MenuButton {
                    text: qsTr("Open Project")
                    onClicked: openFolder()
                }
                SectionLabel {
                    text:       qsTr("CONTROLLER")
                    topPadding: 3
                }
                MenuButton {
                    text: qsTr("My Controllers")
                }
                MenuButton {
                    text: qsTr("Search Local Network")
                }
                MenuButton {
                    text: qsTr("Connect...")
                }
                SectionLabel {
                    text:       qsTr("TOOLS")
                    topPadding: 3
                }
                MenuButton {
                    text: qsTr("Firmware Manager")
                }
                MenuButton {
                    text: qsTr("Flash Hardware")
                }
            }
        }

        // ====== Separator =======
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight:     true
            color:                 Theme.bgPanel
        }

        // ===== Right Menu =====
        Rectangle {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            Layout.topMargin:    40
            Layout.leftMargin:   20
            Layout.rightMargin:  20
            Layout.bottomMargin: 20
            color:             "transparent"

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text:           "My Projects"
                        color:          Theme.white
                        font.bold:      true
                        font.pixelSize: 40
                    }

                    Item { Layout.fillWidth: true }

                    ComboBox {
                        id: comboBox_Workspace
                        Layout.preferredWidth:  200
                        Layout.preferredHeight: 34

                        model: [ "Workspace", "Local", "Cloud" ]

                        background: Rectangle {
                            radius: 5
                            color:  Theme.bgHover
                        }
                    }

                    SearchBox {
                        id: searchBox_Projects
                    }
                }

                // ====== Separator =======
                Rectangle {
                    Layout.preferredHeight: 1
                    Layout.fillWidth:       true
                    color:                  Theme.bgPanel
                }

                // ===== Header =====
                Item {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: 30

                    RowLayout {
                        anchors.fill:        parent
                        anchors.leftMargin:  14
                        spacing:             8

                        Repeater{
                            id: header_Titles
                            model: [
                                { name: "Title"         },
                                { name: "Location"      },
                                { name: "Workspace"     },
                                { name: "Last modified" },
                            ]
                            delegate: Text {
                                Layout.preferredWidth: 180
                                Layout.fillHeight: true
                                color: Theme.white
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignLeft
                                text: modelData.name
                                font.pixelSize: 20
                                font.bold:      true
                            }
                        }
                        Item { Layout.fillWidth: true }
                    }
                }

                // ====== Separator =======
                Rectangle {
                    Layout.preferredHeight: 1
                    Layout.fillWidth:       true
                    color:                  Theme.bgPanel
                }

                // ===== Rows =====
                ListView {
                    id: listView_RecentProjects
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    clip:              true

                    model: root.ref_RecentProjects
                    ScrollBar.vertical: ScrollBar {}

                    delegate: Rectangle {
                        id: rowProject
                        required property var modelData

                        width:  ListView.view.width
                        height: 50
                        color:  rowHover.hovered ? Theme.bgHover : "transparent"

                        HoverHandler {
                            id: rowHover
                            cursorShape: Qt.PointingHandCursor
                        }

                        TapHandler {
                            onTapped: root.openProject(rowProject.modelData.path)
                        }

                        RowLayout {
                            anchors.fill:       parent
                            anchors.leftMargin: 14
                            spacing:            8

                            Text {
                                Layout.preferredWidth: 180
                                color:          Theme.white
                                elide:          Text.ElideRight
                                text:           rowProject.modelData.title
                                font.pixelSize: 16
                                font.bold:      true
                            }
                            Text {
                                Layout.preferredWidth: 180
                                color:          Theme.white
                                elide:          Text.ElideRight
                                text:           rowProject.modelData.location
                                font.pixelSize: 14
                            }
                            Text {
                                Layout.preferredWidth: 180
                                color:          Theme.white
                                elide:          Text.ElideRight
                                text:           rowProject.modelData.workspace === "" ? "local" : rowProject.modelData.workspace
                                font.pixelSize: 16
                                font.bold:      true
                            }
                            Text {
                                Layout.preferredWidth: 180
                                color:          Theme.white
                                elide:          Text.ElideRight
                                text:           rowProject.modelData.mDate
                                font.pixelSize: 14
                            }

                            Item { Layout.fillWidth: true }
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}