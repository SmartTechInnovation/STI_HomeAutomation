import QtQuick
import QtQuick.Shapes
import QtQuick.Controls

import "../../../../../../Theme"

Item {
    id: root

    property var   portModel: null
    property Item  blockRef:  null
    property bool  connected: portModel.connected
    property real  worldScale: 1

    readonly property bool hovered: ma_Port.containsMouse

    signal pressedAt()
    signal draggedTo(real wx, real wy)
    signal releasedAt(real wx, real wy)

    width:  15
    height: 15

    Row {
        id: shape
        anchors.centerIn: parent
        spacing: 1
        scale: ma_Port.containsMouse || ma_Port.pressed ? 1.15 : 1.0
        Behavior on scale { NumberAnimation { duration: 70 } }

        Rectangle {
            id: dot
            width: root.width
            height: root.height
            radius: width / 2
            anchors.verticalCenter: parent.verticalCenter
            color: root.connected ? root.portModel.color : "transparent"
            border.width: 3
            border.color: root.portModel.color
            antialiasing: true
        }

        Shape {
            id: tri
            width: 2
            height: 5
            anchors.verticalCenter: parent.verticalCenter
            antialiasing: true

            ShapePath {
                strokeWidth: 1.5
                strokeColor: root.portModel.color
                fillColor:   root.portModel.color
                joinStyle: ShapePath.RoundJoin
                capStyle:  ShapePath.RoundCap

                startX: 0; startY: 0
                PathLine { x: 0; y: tri.height }
                PathLine { x: tri.width; y: tri.height / 2 }
                PathLine { x: 0; y: 0 }
            }
        }
    }

    ToolTip {
        id: port_Tip
        visible: root.hovered
        delay: 500
        text:  root.portModel.extName + " [" + root.portModel.type + "]"
        scale: worldScale

        contentItem: Rectangle {
            radius: 5
            color:        "#222222"
            border.color: "#444444"
            border.width: 2

            Text {
                padding: 8
                text: port_Tip.text
                color: Theme.white
                font.pixelSize: 12
            }
        }

        background: null
    }

    MouseArea {
        id: ma_Port
        anchors.fill: parent
        hoverEnabled: true

        onPressed: (mouse) => root.pressedAt()
        onPositionChanged: (mouse) => {
            if(!pressed || !root.blockRef) return
            var p = mapToItem(blockRef, mouse.x, mouse.y)
            root.draggedTo(p.x, p.y)
        }
        onReleased: (mouse) => {
            if(!root.blockRef) return
            var p = mapToItem(blockRef, mouse.x, mouse.y)
            root.releasedAt(p.x, p.y)
        }
    }
}