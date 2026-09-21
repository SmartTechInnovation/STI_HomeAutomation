import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property color colorPort: "#4aa3df"
    property Item  blockRef: null
    property bool  connected: false   // false = gol (doar contur), true = plin

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
            color: root.connected ? root.colorPort : "transparent"
            border.width: 3
            border.color: root.colorPort
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
                strokeColor: root.colorPort
                fillColor:   root.colorPort
                joinStyle: ShapePath.RoundJoin
                capStyle:  ShapePath.RoundCap

                startX: 0; startY: 0
                PathLine { x: 0; y: tri.height }
                PathLine { x: tri.width; y: tri.height / 2 }
                PathLine { x: 0; y: 0 }
            }
        }
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