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

    property bool ctrlPressed: false

    readonly property int workPageWidth:  project ? project.info.paperSize.width  : 4200
    readonly property int workPageHeight: project ? project.info.paperSize.height : 2970
    readonly property int snap:           10 // 1mm grid
    readonly property string mimeBlock:   "application/x-sti-block"

    function snapped(value){
        return Math.round(value / snap) * snap
    }

    /* top-left corner of an item whose header is under the pointer, kept on the paper */
    function itemPosition(pointerPoint, itemWidth, itemHeight){
        return Qt.point(Math.max(0, Math.min(workPageWidth  - itemWidth,  snapped(pointerPoint.x - itemWidth / 2))),
                        Math.max(0, Math.min(workPageHeight - itemHeight, snapped(pointerPoint.y - 24))))
    }

    function select(iUuid){
        if(project){
            if(ctrlPressed){
                project.addSelectedItem(iUuid)
            }else{
                project.setSelectedItem(iUuid)
            }
        }
        root.forceActiveFocus()
    }

    function saveView(){
        if(!pageRef) return
        pageRef.viewX     = world.x
        pageRef.viewY     = world.y
        pageRef.viewScale = world.scale
        pageRef.viewValid = true
    }

    function movePlacement(pagePoint){
        pendingPlacement.pointer  = pagePoint
        pendingPlacement.overPage = pagePoint.x >= 0 && pagePoint.y >= 0 && pagePoint.x <= workPageWidth && pagePoint.y <= workPageHeight
    }

    function beginBlockPlacement(uuid){
        if(!project || coverPage)  return
        const blockGhost = root.pageRef.getBlockGhost(uuid)
        if(!blockGhost || !blockGhost.uuid) return
        pendingPlacement.kind      = blockGhost.tagName
        pendingPlacement.reference = blockGhost
        root.forceActiveFocus()
    }

    function cancelPlacement(){
        pendingPlacement.kind      = ""
        pendingPlacement.reference = null
        pendingPlacement.fromDrag  = false
        pendingPlacement.overPage  = false
    }

    function commitPlacement(pagePoint){
        movePlacement(pagePoint)
        const reference = pendingPlacement.reference
        if(!reference || !pendingPlacement.overPage){
            cancelPlacement()
            return
        }
        if(pendingPlacement.kind === "BlockInstance"){
            const position = itemPosition(pagePoint, placementBlockGhost.width, placementBlockGhost.height)
            root.project.cmdPlaceBlock(reference.uuid, position.x, position.y)
        }else{
            const position = itemPosition(pagePoint, 1, 1)
        }
        cancelPlacement()
        root.forceActiveFocus()
    }

    // ====== Keboard Shortcuts ======
    Keys.onPressed: (event) => {
        if(event.key === Qt.Key_Escape){
            cancelPlacement()
            event.accepted = true
        }else if(event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace){
            const SelectedItems = project.getSelectedItems();
            SelectedItems.forEach(function (item) {
                project.cmdRemove(item)
                event.accepted = true
            })
        }else if(event.key === Qt.ControlModifier || event.key === Qt.Key_Control){
            ctrlPressed = true;
        }
    }

    Keys.onReleased: (event) => {
        if(event.key === Qt.ControlModifier || event.key === Qt.Key_Control){
            ctrlPressed = false;
        }
    }

    // ====== Generic Placement Object ======
    QtObject{
        id: pendingPlacement
        property var    reference:  null
        property string kind:       ""
        property point  pointer:    Qt.point(0, 0)
        property bool   overPage:   false
        property bool   fromDrag:   false

        readonly property bool active: reference !== null
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
            if(pos.button === Qt.LeftButton)
                root.select("")
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
        scale:  root.pageRef ? root.pageRef.viewScale : 0.2
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
                info:       root.project ? root.project.info : ({})
                pageTotal:  root.pageTotal
                pageTitle:  qsTr("Cover")
                pageDate:   root.pageMDate
            }
        }
        // ================ Program on Page =================
        Repeater {
            id: rep_Blocks

            model: root.pageRef ? root.pageRef.blocks : ({})

            delegate: Block {
                id: blockItem
                worldScale: world.scale
                snap:       root.snap

                Connections {
                    target: root.project
                    /* deferred: never touch the TreeView layout from inside a model change / ongoing layout */
                    function onSelectedItemsChanged(){
                        b_Selected = root.project.isSelectedItem(modelData.iUuid);
                    }
                }

                blockRef:   modelData

                onBlockPressed: root.select(modelData.iUuid)
                onBlockDragged: (wx, wy) => {

                }
                onBlockReleased: (wx, wy) => {
                    root.project.cmdMoveItem(modelData.iUuid, wx, wy);
                }

                onPortPressed: (index, output) => {

                }
                onPortDragged: (wx, wy) => {

                }
                onPortReleased: (wx, wy) => {

                }
            }
        }

        Block{
            id: placementBlockGhost

            visible:  pendingPlacement.active && pendingPlacement.overPage
            ghost:    true
            blockRef: pendingPlacement.reference
            z: 10
            x: root.itemPosition(pendingPlacement.pointer, width, height).x
            y: root.itemPosition(pendingPlacement.pointer, width, height).y
        }
    }

    // ===== Placement of a block chosen in the menu: follows the pointer (mouse) or the finger (touch) =====
    MouseArea {
        anchors.fill: parent
        enabled:      pendingPlacement.active && !pendingPlacement.fromDrag
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton


        onPositionChanged: (mouse) => {
            root.movePlacement(mapToItem(world, mouse.x, mouse.y))
        }

        onReleased: (mouse) => {
            if(mouse.button === Qt.LeftButton)
                root.commitPlacement(mapToItem(world, mouse.x, mouse.y))
        }
        onExited: pendingPlacement.overPage = false
    }

    // ===== Placement of a signal dragged from the tree =====
    DropArea {
        anchors.fill: parent
        enabled:      !root.coverPage
        keys:         [ root.mimeSignal ]

        onEntered: (drag) => {
            root.beginSignalPlacement(drag.getDataAsString(root.mimeSignal))
            root.movePlacement(mapToItem(world, drag.x, drag.y))
        }
        onPositionChanged: (drag) => root.movePlacement(mapToItem(world, drag.x, drag.y))
        onExited:          root.cancelPlacement()
        onDropped: (drop) => {
            root.commitPlacement(mapToItem(world, drop.x, drop.y))
            drop.acceptProposedAction()
        }
    }

    // ===== Placement of a block dragged from the top toolbar =====
    DropArea {
        anchors.fill: parent
        enabled:      !root.coverPage
        keys:         [ root.mimeBlock ]

        onEntered: (drag) => {
            root.beginBlockPlacement(drag.getDataAsString(root.mimeBlock))
            pendingPlacement.fromDrag = true
            root.movePlacement(mapToItem(world, drag.x, drag.y))
        }
        onPositionChanged: (drag) => root.movePlacement(mapToItem(world, drag.x, drag.y))
        onExited:          root.cancelPlacement()
        onDropped: (drop) => {
            root.commitPlacement(mapToItem(world, drop.x, drop.y))
            drop.acceptProposedAction()
        }
    }
}
