import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../../Theme"
import "Parts"

Item {
    id: root

    clip: true

    property string pageTitle: qsTr("Untitled")
    property string pageMDate:  ""

    property var  project:    null
    property var  pageRef:    null
    property int  pageNumber: 1
    property int  pageTotal:  1
    property bool coverPage:  false

    readonly property int workPageWidth:  project ? project.pageSize.width  : 4200
    readonly property int workPageHeight: project ? project.pageSize.height : 2970
    readonly property int snap:           10 // 1mm grid

    // ====== Editor State ======
    readonly property string selectedInstanceUuid: ""

    signal blockSelected(string uuid)

    function saveView(){
        if(!pageRef) return
        pageRef.viewX     = world.x
        pageRef.viewY     = world.y
        pageRef.viewScale = world.scale
        pageRef.viewValid = true
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
            info:         root.project ? root.project.infoMap : ({})
            pageTitle:    root.pageTitle
            pageNumber:   root.pageNumber
            pageTotal:    root.pageTotal
            pageDate:     root.pageMDate
        }
        // ===== Cover Page Content =====
        Loader {
            active: root.coverPage
            x: 200
            y: 100
            width:  root.workPageWidth  - 300
            height: root.workPageHeight - 200
            sourceComponent: PageCover {
                info:       root.project ? root.project.infoMap : ({})
                pageTotal:  root.pageTotal
                pageTitle:  qsTr("Cover")
                pageDate:   root.pageMDate
            }
        }
        // ================ Program on Page =================
        Repeater {
            id: rep_Blocks

            model: root.pageRef ? root.pageRef.blockList : ({})

            delegate: Block {
                id: blockDelegate
                required property int index
                required property var modelData

                property real dragDx: 0
                property real dragDy: 0

                x: Math.max(0, Math.min(root.workPageWidth  - width,  modelData.position.x + dragDx))
                y: Math.max(0, Math.min(root.workPageHeight - height, modelData.position.y + dragDy))
                z: b_Selected ? 3 : 2

                blockTypeUuid: modelData.uuid
                blockInstUuid: modelData.iUuid
                blockTitle:    modelData.title
                blockType:     modelData.type
                blockColor:    modelData.color
                blockIcon:     modelData.icon
                defaultWidth:  modelData.width
                inputs:        modelData.inputs
                outputs:       modelData.outputs
                properties:    modelData.properties

                b_Selected:    root.selectedInstanceUuid === modelData.iUuid

            }
        }
    }
}
