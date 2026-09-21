import QtQuick
import QtQuick.Controls

import "../Theme"

Item {
    id: root_tab

    property string label:    ""
    property bool   active:   false
    property bool   dirty:    false
    property bool   closable: true
    property string icon_src: ""

    // hover pe TOT tabul, robust: HoverHandler e pasiv, nu il "fura"
    // butoanele copil (icon / close) ca la MouseArea.containsMouse
    property bool hovered: tab_hover.hovered

    signal sig_activate()
    signal sig_closeTab()

    implicitWidth:  Math.min(220, row_content.width + 20)
    implicitHeight: parent.height

    HoverHandler {
        id: tab_hover           // doar pentru starea `hovered` (pasiv, nu-l fura butoanele)
    }

    MouseArea {
        id: tab_mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.PointingHandCursor
        onClicked: root_tab.sig_activate()
    }

    // ==== Fundal + linia de accent ====
    Rectangle {
        anchors.fill: parent
        color: root_tab.active
               ? (root_tab.hovered ? Theme.bgHover : Theme.bgElevated)
               : (root_tab.hovered ? Theme.bgHover : Theme.bgPanel)

        Rectangle {
            anchors.top: parent.top
            width:  parent.width
            height: 2
            color:  root_tab.active ? Theme.blue : "transparent"
        }
    }

    Rectangle {
        anchors.right:  parent.right
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: Theme.gray
    }

    // ==== Continut: [icon] label ● [x] ====
    Row {
        id: row_content
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:           parent.left
        anchors.leftMargin:     10
        spacing:                5
        height:                 parent.height

        // ===== Icon (Button doar ca sa poata colora SVG-ul) =====
        Button {
            id: tab_icon
            visible: root_tab.icon_src.length !== 0   // invizibil => exclus din Row, fara gol fantoma
            anchors.verticalCenter: parent.verticalCenter
            height:  parent.height - 3
            width:   parent.height - 3
            padding: 0

            icon.source: root_tab.icon_src
            icon.color:  root_tab.active ? Theme.white : Theme.gray
            icon.width:  Math.round((root_tab.height - 3) * 0.8)
            icon.height: Math.round((root_tab.height - 3) * 0.8)

            background: null
            onClicked: root_tab.sig_activate()            // click pe iconita = activeaza tabul

            HoverHandler { cursorShape: Qt.PointingHandCursor }
        }

        // ===== Label =====
        Text {
            visible:                root_tab.label.length !== 0
            anchors.verticalCenter: parent.verticalCenter
            text:                   root_tab.label
            color:                  root_tab.active ? Theme.white : Theme.gray
            font.pixelSize:         12
            elide:                  Text.ElideRight
        }

        // ===== Modified dot =====
        Text {
            anchors.verticalCenter: parent.verticalCenter
            visible:                root_tab.dirty
            text:                   "●"
            color:                  Theme.green
            font.pixelSize:         8
        }

        // ===== Close =====
        Button {
            id: btn_CloseProject
            visible: root_tab.closable
            anchors.verticalCenter: parent.verticalCenter
            height:  parent.height - 10
            width:   height
            padding: 0

            icon.source: "qrc:/icons/action/close.svg"
            icon.color:  btn_CloseProject.hovered ? Theme.white : Theme.gray
            icon.width:  Math.round((root_tab.height - 10) * 0.7)
            icon.height: Math.round((root_tab.height - 10) * 0.7)

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            onClicked: root_tab.sig_closeTab()

            background: Rectangle {
                radius: 4
                color: btn_CloseProject.down
                       ? Theme.red
                       : btn_CloseProject.hovered
                         ? Qt.darker(Theme.bgChrome, 1.18)
                         : "transparent"
            }
        }
    }
}
