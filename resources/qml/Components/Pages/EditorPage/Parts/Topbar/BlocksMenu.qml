import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import "../../../../Theme"

Rectangle {
    id: root

    property string categoryName: ""
    property var    blocks:       []
    property int    rowHeight:    28
    property int    maxListHeight: 420

    readonly property int headerHeight: 24
    readonly property int listHeight:   Math.min(maxListHeight, Math.max(rowHeight, (blocks ? blocks.length : 0) * rowHeight))

    implicitWidth: 250
    implicitHeight: headerHeight + listHeight + 10

    color: Theme.bgElevated
    border.color: Theme.gray
    border.width: 1
    radius:       4

    ColumnLayout {
        anchors.fill:    parent
        anchors.margins: 2
        spacing: 0

        Text {
            Layout.fillWidth:       true
            Layout.preferredHeight: headerHeight
            text:  root.categoryName
            color: Theme.gray
            font.pixelSize: 12
            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.gray
            opacity: 0.5
        }

        //==== List of blocks ====
        ListView{
            id: list_Blocks

            Layout.fillWidth:  true
            Layout.fillHeight: true
            clip:  true
            model: root.blocks
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                policy: list_Blocks.contentHeight > list_Blocks.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }
            delegate: Rectangle {
                id: blockRow

                width:  list_Blocks.width
                height: root.rowHeight
                color:  ma_BlockRow.containsMouse ? Theme.bgHover : "transparent"

                RowLayout {
                    anchors.fill: parent
                    spacing: 8

                    IconImage {
                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        visible: modelData.icon.length > 0
                        color: Theme.white
                        fillMode: Image.PreserveAspectFit
                        source: modelData.icon
                        sourceSize.width:  width - 4
                        sourceSize.height: height - 4
                    }

                    Column {
                        Layout.fillHeight: true;
                        Layout.fillWidth:  true;
                        Text{
                            text: modelData.title
                            color: Theme.white
                            font.pixelSize: 13
                            font.bold:      true
                        }
                        Text{
                            text: modelData.description
                            color: Theme.white
                            font.pixelSize: 9
                        }
                    }



                    Item { Layout.fillWidth: true }
                }

                MouseArea {
                    id: ma_BlockRow
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape:  Qt.PointingHandCursor

                    onClicked: console.info(modelData.type)
                }
            }
        }

    }
}
