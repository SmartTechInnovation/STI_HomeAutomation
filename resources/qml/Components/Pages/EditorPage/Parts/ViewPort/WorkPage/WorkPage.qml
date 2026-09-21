import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../../Theme"
import "Parts"

Item {
    id: root

    clip: true

    property var project: null
    property int workPageWidth:  4200
    property int workPageHeight: 2970

    // ====== Background ======
    Rectangle {
        anchors.fill: parent
        color:        Theme.bgCanvas
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        property real lastX: 0
        property real lastY: 0
        onPressed: (pos) => {
            lastX = pos.x
            lastY = pos.y
        }
        onPositionChanged: (mouse) => {
            // ===== Pan =====
            if(mouse.buttons & Qt.MiddleButton){
                world.x += mouse.x - lastX
                world.y += mouse.y - lastY
                lastX = mouse.x
                lastY = mouse.y
            }
        }
        onWheel: (wheel) => {
            if((wheel.modifiers & Qt.MetaModifier && (Qt.platform.os === "osx" || Qt.platform.os === "macos")) || wheel.modifiers & Qt.ControlModifier){
                // ==== Zoom ====
                if(wheel.pixelDelta.y !== 0){
                    var scale_factor = wheel.angleDelta.y > 0 ? 1.12 : 1 / 1.12
                    var world_x = (wheel.x - world.x) / world.scale
                    var world_y = (wheel.y - world.y) / world.scale
                    var new_scale = Math.max(0.10, Math.min(20, world.scale * scale_factor))
                    world.scale = new_scale
                    world.x = wheel.x - world_x * new_scale
                    world.y = wheel.y - world_y * new_scale
                }
            }else{
                // ==== Pan ====
                world.x += wheel.pixelDelta.x
                world.y += wheel.pixelDelta.y / 2
            }
        }
    }

    Item {
        id: world
        width:  root.workPageWidth
        height: root.workPageHeight
        scale:  0.2
        //x:     (root.width  - root.workPageWidth)  / 2
        //y:     (root.height - root.workPageHeight) / 2
        transformOrigin: Item.TopLeft

        // === Handler for tarck pad ====
        PinchHandler {
            target: world
            rotationAxis.enabled: false
        }

        Rectangle {
            id: page
            anchors.fill: parent
            color: Theme.bgTab
        }

        Block{
            id: exampleOfBlock

            x: 100
            y: 50
        }

        Block{
            id: exampleOfBlo

            x: 300
            y: 50

            onBlockDragged: (x, y) => console.info(x + " " + y)
        }

        InputSignal {
            x: 1000
            y: 100
        }
    }
}
