import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../../Theme"
import "Parts"

Item {
    id: root

    clip: true

    property var project:    null
    property var pageModel:  null
    property int pageNumber: 1
    property int pageTotal:  1
    property bool coverPage: false
    property string pageTitle: qsTr("Untitled")
    property string pageDate:  ""

    readonly property int workPageWidth:  project ? project.pageSize.width  : 4200
    readonly property int workPageHeight: project ? project.pageSize.height : 2970
    readonly property int snap:           10 // 1mm grid

    // ====== Editor State ======
    readonly property string selectedInstanceUuid: ""

    signal blockSelected(string uuid)

    function saveView(){
        if(!pageModel) return
        pageModel.viewX     = world.x
        pageModel.viewY     = world.y
        pageModel.viewScale = world.scale
        pageModel.viewValid = true
    }

    // ====== Background ======
    Rectangle {
        anchors.fill: parent
        color:        Theme.bgCanvas
    }

    // ===== Pan / wheel / deselect =====
    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        property real lastX: 0
        property real lastY: 0
        onPressed: (pos) => {
            lastX = pos.x
            lastY = pos.y
            root.forceActiveFocus()
        }
        onReleased: root.saveView()
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
        transformOrigin: Item.TopLeft

        // ====== Handler for tarck pad ====
        PinchHandler {
            target: world
            rotationAxis.enabled: false
        }

        // ====== Page shadow ======
        Rectangle {
            id: pageShadow
            x: 12
            y: 16
            width:  parent.width
            height: parent.height
            color: "black"; opacity: 0.35
        }
        // ====== Page background ======
        Rectangle {
            id: pageBackground
            anchors.fill: parent
            color: Theme.paper
        }
        // ===== Page frame ===========
        PageFrame {
            id: pageFrame
            anchors.fill: parent
            info:         root.project ? root.project.info : ({})
            pageTitle:    root.pageTitle
            pageNumber:   root.pageNumber
            pageTotal:    root.pageTotal
            pageDate:     root.pageDate
        }
        // ===== Cover Page Content =====
        Loader {
            active: root.coverPage
            x: 200
            y: 100
            width:  root.workPageWidth  - 300
            height: root.workPageHeight - 200
            sourceComponent: PageCover {
                info:       root.project ? root.project.info : ({})
                pageTotal:  root.pageTotal
                pageTitle:  qsTr("Cover")
                pageDate:   root.pageDate
            }
        }
        // ================ Program on Page =================
        Repeater {
            id: rep_Blocks

            model: root.pageModel

            delegate: Block {
                id: blockDelegate
                required property int    index
                required property string uuidInstance
                required property string uuidType
                required property string title
                required property string type
                required property string color
                required property string icon
                required property real   pX
                required property real   pY
                required property real   bWidth
                required inputs
                required outputs

                property real dragDx: 0
                property real dragDy: 0

                x: Math.max(0, Math.min(root.workPageWidth  - width,  pX + dragDx))
                y: Math.max(0, Math.min(root.workPageHeight - height, pY + dragDy))
                z: b_selected ? 3 : 2

                blockInstUuid: uuidInstance
                blockTypeUuid: uuidType
                blockTitle:    title
                blockType:     type
                blockColor:    color
                blockIcon:     icon
                defaultWidth:  bWidth

                b_Selected:    root.selectedInstanceUuid == uuidInstance

            }
        }
    }
}
