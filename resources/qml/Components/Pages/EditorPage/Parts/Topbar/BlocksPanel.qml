import QtQuick
import QtQuick.Controls

import "../../../../Theme"

/*
 *  Panou cu blocurile unei categorii.
 *  Folosit si ca submeniu pentru "Function Blocks", si ca popup pentru shortcut-uri.
 */
Rectangle {
    id: panel

    property string categoryName: ""
    property var    blocks:       []
    property int    rowHeight:    28
    property int    maxListHeight: 420

    signal blockPicked(string category, string type, string title)

    readonly property int headerHeight: 24
    readonly property int listHeight:   Math.min(maxListHeight, Math.max(rowHeight, (blocks ? blocks.length : 0) * rowHeight))

    implicitWidth:  250
    implicitHeight: headerHeight + listHeight + 4

    color:        Theme.bgElevated
    border.color: Theme.gray
    border.width: 1
    radius:       4

    Column {
        anchors.fill:    parent
        anchors.margins: 2
        spacing: 0

        // ===== Header =====
        Rectangle {
            width:  parent.width
            height: panel.headerHeight
            color:  "transparent"

            Text {
                anchors.left:           parent.left
                anchors.leftMargin:     8
                anchors.verticalCenter: parent.verticalCenter
                text:  panel.categoryName
                color: Theme.gray
                font.pixelSize: 12
                font.bold: true
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width:  parent.width
                height: 1
                color:  Theme.gray
                opacity: 0.5
            }
        }

        // ===== Lista de blocuri =====
        ListView {
            id: blocksList
            width:  parent.width
            height: panel.listHeight
            clip:   true
            model:  panel.blocks
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {
                policy: blocksList.contentHeight > blocksList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }

            delegate: Rectangle {
                id: blockRow
                width:  blocksList.width
                height: panel.rowHeight
                color:  ma_block.containsMouse ? Theme.bgHover : "transparent"

                Row {
                    anchors.left:           parent.left
                    anchors.leftMargin:     8
                    anchors.right:          parent.right
                    anchors.rightMargin:    8
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 8

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width:  10
                        height: 10
                        radius: 2
                        color:  modelData.color !== undefined ? modelData.color : Theme.gray
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text:  modelData.title
                        color: Theme.white
                        font.pixelSize: 13
                    }
                }

                Text {
                    anchors.right:          parent.right
                    anchors.rightMargin:    8
                    anchors.verticalCenter: parent.verticalCenter
                    text:  modelData.inputs + "/" + modelData.outputs
                    color: Theme.gray
                    font.pixelSize: 11
                    visible: ma_block.containsMouse
                }

                MouseArea {
                    id: ma_block
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape:  Qt.PointingHandCursor

                    onClicked: panel.blockPicked(panel.categoryName, modelData.type, modelData.title)
                }
            }
        }
    }
}
