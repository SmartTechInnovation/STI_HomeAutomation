import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../Theme"
import "Parts"

Item {
    id: root

    signal sig_New_Project()
    signal sig_Open_Project()

    readonly property var projectColumns: [
        { key: "name",         title: "Project Name",  width: 160 },
        { key: "location",     title: "        ",      width: 240 },
        { key: "workspace",    title: "Workspace",     width: 120 },
        { key: "lastModified", title: "Last Modified", width: 0, fill: true }
    ]

    function colWidth(key) {
        for (var i = 0; i < projectColumns.length; i++)
            if (projectColumns[i].key === key) return projectColumns[i].width
        return 0
    }

    function colFill(key) {
        for (var i = 0; i < projectColumns.length; i++)
            if (projectColumns[i].key === key) return projectColumns[i].fill === true
        return false
    }

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

    component Cell: Label {
        property string col: ""

        color:             Theme.white
        elide:             Text.ElideRight
        verticalAlignment: Text.AlignVCenter

        Layout.fillWidth:      root.colFill(col)
        Layout.preferredWidth: root.colFill(col) ? 0 : root.colWidth(col)
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
                Repeater {
                    model: [
                        { title: "PROJECT",       items: ["My Projects", "New  Project", "Open Project"] },
                        { title: "CONTROLLER",    items: ["My Controllers", "Search Local Network", "Connect..."] },
                        { title: "TOOLS",         items: ["Firmware Manager", "Flash Hardware"] }
                    ]

                    Column {
                        required property string title
                        required property var    items
                        required property int    index

                        SectionLabel {
                            text:       parent.title
                            topPadding: parent.index === 0 ? 0 : 3
                        }

                        Repeater {
                            model: parent.items

                            MenuButton {
                                required property string modelData
                                text: modelData
                            }
                        }
                    }
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

                        model: [ "Workspace", "Local", "Cloude" ]

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
                        anchors.rightMargin: 8
                        spacing:             8

                        Repeater {
                            model: root.projectColumns

                            Cell {
                                required property var modelData
                                col:            modelData.key
                                text:           modelData.title
                                color:          Theme.white
                                font.pixelSize: 20
                            }
                        }
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
                    id: listView_Projects
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    clip:              true

                    model: [
                        {name: "Duplex R", location: "Stauceni..dsadsad.", workspace: "Trustera", lastModified: "9/4/2026 3:00 PM"},
                        {name: "Duplex L", location: "Staucenfsdfi...", workspace: "Trustera", lastModified: "9/4/2026 4:00 PM"},
                        {name: "Casa 1  ", location: "Staucenfsdffdsffdsfi...", workspace: "Trustera", lastModified: "9/4/2026 5:00 PM"},
                        {name: "Casa 2  ", location: "Staucenfsdfsdfdsfi...", workspace: "Trustera", lastModified: "9/4/2026 6:00 PM"},
                    ]

                    ScrollBar.vertical: ScrollBar {}

                    delegate: Rectangle {
                        id: project_Row
                        required property string name
                        required property string location
                        required property string workspace
                        required property string lastModified

                        width:  ListView.view.width
                        height: 50

                        color: project_MouseArea.containsMouse ? Theme.bgHover : "transparent"

                        MouseArea {
                            id: project_MouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: console.info("Open Project: " + project_Row.name)

                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                        }

                        RowLayout {
                            anchors.fill:        parent
                            anchors.leftMargin:  14
                            anchors.rightMargin: 8
                            spacing:             8

                            Cell {
                                col:            "name"
                                text:           project_Row.name
                                font.bold:      true
                                font.pixelSize: 18
                            }

                            Cell {
                                col:   "location"
                                text:  project_Row.location
                                font.pixelSize: 14
                                color: Theme.gray
                            }

                            Cell {
                                col:  "workspace"
                                font.bold: true
                                font.pixelSize: 18
                                text: project_Row.workspace
                            }

                            Cell {
                                col:            "lastModified"
                                text:           project_Row.lastModified
                                color:          Theme.white
                                font.pixelSize: 14
                            }
                        }
                    }
                }
            }
        }
    }
}