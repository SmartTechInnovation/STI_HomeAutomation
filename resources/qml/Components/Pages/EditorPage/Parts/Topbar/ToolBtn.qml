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

    readonly property int fontBig:   14
    readonly property int fontSmall: 10

    signal clicked()
    signal pressed()

    readonly property string textFirst:  qsTr(firstRowText)
    readonly property string textSecond: qsTr(secondRowText)
    readonly property real   textWidth:  width - 12          // ColumnLayout margins (2 x 6)

    TextMetrics {
        id: tm_Full
        font.pixelSize: root.fontBig
        font.bold:      true
        text:           root.textFirst
    }

    readonly property bool autoSplit: textSecond.length === 0
                                      && textFirst.indexOf(" ") > 0
                                      && tm_Full.advanceWidth > textWidth
    readonly property var  rows: autoSplit ? splitInTwo(textFirst) : [textFirst, textSecond]
    readonly property bool twoRows: rows[1].length > 0

    function splitInTwo(text){
        const words = text.split(" ")
        let bestIndex = 1
        let bestDiff  = Number.MAX_VALUE
        for(let i = 1; i < words.length; i++){
            const first  = words.slice(0, i).join(" ")
            const second = words.slice(i).join(" ")
            const diff   = Math.abs(first.length - second.length)
            if(diff < bestDiff){
                bestDiff  = diff
                bestIndex = i
            }
        }
        return [words.slice(0, bestIndex).join(" "), words.slice(bestIndex).join(" ")]
    }

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
                color: Theme.iconColor(source, root.color)
                fillMode: Image.PreserveAspectFit
                source: root.iconSrc.length > 0 ? root.iconSrc : "qrc:/icons/misc/blocks.svg"
                sourceSize.width:  height
                sourceSize.height: height

            }

            Text {
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize
                horizontalAlignment: Text.AlignHCenter

                text: root.rows[0]

                color: root.color
                font.pixelSize: root.twoRows ? root.fontSmall : root.fontBig
                fontSizeMode:   Text.Fit       // a single word that is still too long shrinks
                font.bold:      true
            }

            Text {
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize
                horizontalAlignment: Text.AlignHCenter
                visible: root.twoRows
                text: root.rows[1]
                color: root.color
                font.pixelSize: root.fontSmall
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
