import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import "../../../../Theme"

Rectangle {
    id: root

    property string title:       ""
    property var    items:       []
    property int    rowHeight:    28
    property int    maxListHeight: 420

    readonly property int headerHeight: 24
    readonly property int listHeight:   Math.min(maxListHeight, Math.max(rowHeight, (items ? items.length : 0) * rowHeight))

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
            text:  root.title
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

        //==== List of items ====
        ListView{
            id: list_Items

            Layout.fillWidth:  true
            Layout.fillHeight: true
            clip:  true
            model: root.items
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                policy: list_Items.contentHeight > list_Items.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }
            delegate: Rectangle {
                id: blockRow

                width:  list_Items.width
                height: root.rowHeight
                color:  ma_BlockRow.containsMouse ? Theme.bgHover : "transparent"

                RowLayout {
                    anchors.fill: parent
                    spacing: 8

                    IconImage {
                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        visible: modelData.Icon.length > 0
                        color: Theme.white
                        fillMode: Image.PreserveAspectFit
                        source: modelData.Icon
                        sourceSize.width:  width - 4
                        sourceSize.height: height - 4
                    }

                    Column {
                        Layout.fillHeight: true;
                        Layout.fillWidth:  true;
                        Text{
                            text: modelData.Title
                            color: Theme.white
                            font.pixelSize: 13
                            font.bold:      true
                        }
                        Text{
                            text: modelData.Description
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

                    onClicked: console.info(modelData.Uuid)
                }
            }
        }

    }
}
