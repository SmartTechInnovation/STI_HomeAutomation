import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import "../../../../../../Theme"

Popup {
    id: root

    required property var items

    parent: parent
    x: 0
    y: 0

    width:   200
    padding: 4
    modal: false
    closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

    signal toggleVisible(int fullIndex)

    background: Rectangle {
        color: "#F7F7F7"
        border.color: "#7D8491"
        border.width: 1
        radius: 6
    }

    contentItem:     ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Text {
            id: title
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            Layout.leftMargin: 5
            Layout.topMargin:  5
            text: qsTr("Visible Connections")
            font.pixelSize: 14
            font.bold: true
            color: "black"
        }

        Rectangle {
            Layout.fillWidth:  true
            Layout.preferredHeight: Math.min(contentHeight, 220)
            Layout.leftMargin:  5
            Layout.rightMargin: 5
            color: "white"

            radius: 3

            ListView {
                clip: true
                model: root.items
                anchors.fill: parent
                delegate: RowLayout {
                    required property int index
                    required property var modelData
                    Layout.fillWidth: true
                    spacing: 4

                    CheckBoxCustom {
                        checked: modelData.visible || modelData.connected
                        enabled: !modelData.connected
                        onToggled: root.toggleVisible(index)
                    }

                    Text{
                        Layout.preferredWidth: 40
                        text: modelData.name
                        font.pixelSize: 12
                        color: "black"
                    }

                    Text{
                        Layout.preferredWidth: 40
                        text: modelData.extName
                        font.pixelSize: 12
                        font.bold:      true
                        color: "black"
                    }
                }
            }
        }
    }
}
