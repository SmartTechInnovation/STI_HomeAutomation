import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.impl

import "../../../../Theme"

Item {
    id: root

    property color  color: "#F7F7F7"
    property string firstRowText:  ""
    property string secondRowText: ""
    property string iconSrc: "qrc:/icons/misc/blocks.svg"

    signal clicked()
    signal pressed()

    Rectangle {
        id: btn_Tool
        anchors.fill: parent

        radius: 6
        color: ma_Tool.containsMouse ? Theme.bgHover : Theme.bgPanel

        border.color: Theme.gray
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 4
            IconImage {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                color: root.color
                fillMode: Image.PreserveAspectFit
                source: root.iconSrc.length > 0 ? root.iconSrc : "qrc:/icons/misc/blocks.svg"
                sourceSize.width:  height
                sourceSize.height: height

            }

            Text {
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize
                horizontalAlignment: Text.AlignHCenter

                text: qsTr(root.firstRowText)

                color: root.color
                font.pixelSize: root.secondRowText.length > 0 ? 10 : 14
                fontSizeMode:   Text.Fit
                font.bold:      true
            }

            Text {
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize
                horizontalAlignment: Text.AlignHCenter
                visible: root.secondRowText.length > 0
                text: qsTr(root.secondRowText)
                color: root.color
                font.pixelSize: 10
                fontSizeMode:   Text.Fit
                font.bold:      true
            }
        }

        MouseArea {
            id: ma_Tool
            anchors.fill: parent

            hoverEnabled: true
            cursorShape:  Qt.PointingHandCursor

            onClicked: root.clicked()
            onPressed: root.pressed()
        }
    }
}
