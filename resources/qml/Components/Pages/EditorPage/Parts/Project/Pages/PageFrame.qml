import QtQuick

import "../../../../../Theme"

Item {
    id: root

    property var    info:       ({})
    property string pageTitle:  ""
    property int    pageNumber: 1
    property int    pageTotal:  1
    property string pageDate:   ""

    readonly property real mm:          10
    readonly property real frameLeft:   20 * mm //Left margin
    readonly property real frameTop:    10 * mm //Top  margin
    readonly property real frameWidth:  width  - frameLeft - (10 * mm)
    readonly property real frameHeight: height - frameTop  - (10 * mm)
    readonly property real frameLine:   5


    // ==== Border ====
    Rectangle {
        id:     frameBorder
        x:      root.frameLeft
        y:      root.frameTop
        width:  root.frameWidth
        height: root.frameHeight
        color:  "transparent"
        border.width: root.frameLine
        border.color: Theme.ink
    }
    // ==== Page Number Footer ====
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y:    root.height - root.frameTop / 2 - height / 2
        text: qsTr("Page %1 / %2").arg(root.pageNumber).arg(root.pageTotal)
        color:          Theme.ink
        font.pixelSize: 32
        font.bold:      true
    }
}
