import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.impl

import "../../../../../Theme"

Item {
    id: root
    property var project:   null

    readonly property var    treeModel:     project ? project.tree : null
    readonly property string selectedIUuid: project ? project.selectedIUuid : ""
    readonly property string mimeSignal:    "application/x-sti-signal"

    signal contextItemPicked(string iUuid, var item)

    /* Delete / Backspace removes the selected node (when the schema allows it) */
    Keys.onPressed: (event) => {
        if(event.key !== Qt.Key_Delete && event.key !== Qt.Key_Backspace)
            return
        if(root.project && root.project.selection.canDelete === true){
            root.project.cmdRemove(root.selectedIUuid)
            event.accepted = true
        }
    }

    function revealSelected(){ reveal(root.selectedIUuid) }

    function reveal(iUuid){
        if(!treeModel || iUuid === "") return
        const index = treeModel.indexOf(iUuid)
        if(!index.valid) return
        const parents = []
        for(let p = treeModel.parent(index); p.valid; p = treeModel.parent(p)) parents.unshift(p)
        for(const parentIndex of parents){
            const parentRow = tree.rowAtIndex(parentIndex)
            if(parentRow >= 0 && !tree.isExpanded(parentRow)) tree.expand(parentRow)
        }
        tree.forceLayout()
        const row = tree.rowAtIndex(index)
        if(row >= 0) tree.positionViewAtRow(row, TableView.Contain)
    }

    Connections {
        target: root.treeModel
        function onModelReset(){ tree.expand(0) }
    }
    Connections {
        target: root.project
        /* deferred: never touch the TreeView layout from inside a model change / ongoing layout */
        function onSelectedIUuidChanged(){ Qt.callLater(root.revealSelected) }
    }
    onTreeModelChanged: Qt.callLater(function(){ tree.expand(0) })

    // ======== Header =======
    Rectangle{
        id: treeHeader
        width:  root.width
        height: 34
        color:  Theme.bgStrip
        Text {
            x: 12
            anchors.verticalCenter: parent.verticalCenter
            text:  qsTr("Project")
            color: Theme.gray
            font.pixelSize: 10
            font.bold:      true
            font.capitalization: Font.AllUppercase
            font.letterSpacing: 1
        }
    }

    TreeView {
        id: tree
        anchors.top:    treeHeader.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin:  4
        clip:            true
        boundsBehavior:  Flickable.StopAtBounds
        model:           root.treeModel

        ScrollBar.vertical: ScrollBar {}

        delegate: Item {
            id: node
            implicitWidth:  tree.width
            implicitHeight: 26

            // Proprietăți date de TreeView (nu le setezi tu)
            required property TreeView treeView
            required property bool     isTreeNode
            required property bool     expanded
            required property bool     hasChildren
            required property int      depth
            required property int      row
            /* roles of ProjectTree_class */
            required property string   title
            required property string   uuid
            required property string   iUuid
            required property string   icon
            required property color    color
            required property string   kind
            required property bool     ioType
            required property bool     dragEnabled
            required property bool     description


            readonly property int   indent:     14
            readonly property bool  selected:   iUuid !== "" && root.selectedIUuid === iUuid
            readonly property bool  renamable:  kind === "node"

            function select(){
                if(!root.project || iUuid === "")
                    return
                if(kind === "ref")
                    root.project.selectNode(iUuid)      // opens the page with the signal
                else
                    root.project.selectedIUuid = iUuid
            }

            HoverHandler { id: hover }

            // ==== Hover Background ====
            Rectangle {
                anchors.fill: parent
                color: node.selected ? Qt.lighter(Theme.bgPanel, 1.8)
                                     : hover.hovered ? Qt.lighter(Theme.bgPanel, 1.4) : "transparent"
            }
            // ==== Expand arrow ====
            Item {
                id: arrow
                x: 6 + node.depth * node.indent
                width: 14; height: parent.height
                IconImage {
                    anchors.centerIn: parent
                    visible:  node.hasChildren
                    width: 12; height: 12
                    sourceSize.width: 12; sourceSize.height: 12
                    color:    Theme.gray
                    source:   "qrc:/icons/navigation/chevron-up.svg"
                    rotation: node.expanded ? 180 : 90
                    Behavior on rotation { NumberAnimation { duration: 90 } }
                }
            }

            IconImage {
                id: rowIcon
                x:  node.depth * node.indent + 24
                anchors.verticalCenter: parent.verticalCenter
                color:    txt_Title.color
                fillMode: Image.PreserveAspectFit
                source:   node.icon
                sourceSize.width:  16
                sourceSize.height: 16
            }

            Text{
                id: txt_Title
                x: node.depth * node.indent + 46
                anchors.verticalCenter: parent.verticalCenter
                text: node.title
                elide: Text.ElideRight
                color: node.kind === "unknown" ? Theme.amber
                     : node.kind === "ref"     ? Theme.gray
                     : node.kind === "group"   ? Qt.darker(Theme.white, 1.2) : Theme.white
                font.italic: node.kind === "ref"
                visible: !edit_Title.visible
            }

            TextField {
                id: edit_Title
                x: txt_Title.x - 4
                width:  parent.width  - x - 4
                height: parent.height - 2
                anchors.verticalCenter: parent.verticalCenter
                visible: false
                padding: 2
                font.pixelSize: 13

                function start(){
                    text = node.title;
                    visible = true;
                    forceActiveFocus();
                    selectAll()
                }
                function finish(apply){
                    if(!visible)
                        return
                    visible = false
                    if(apply)
                        root.project.cmdRename(node.iUuid, text)
                }
                onAccepted: finish(true)
                /* focus is often lost while the TreeView is laying out (delegate reuse):
                   capture the values now, push the command after the layout finished */
                onActiveFocusChanged: {
                    if(activeFocus || !visible)
                        return
                    visible = false
                    const iUuid = node.iUuid
                    const title = text
                    Qt.callLater(function(){ if(root.project) root.project.cmdRename(iUuid, title) })
                }
                Keys.onEscapePressed: finish(false)
            }

            // ===== Drag source (Drag.Automatic -> the page shows the ghost) =====
            Item {
                id: dragProxy
                width: 1; height: 1
                Drag.active:            ma_Row.drag.active
                Drag.dragType:          Drag.Automatic
                Drag.supportedActions:  Qt.CopyAction
                Drag.mimeData:          ({ "application/x-sti-signal": node.iUuid })
                Drag.onDragFinished:    { x = 0; y = 0 }
            }

            MouseArea {
                id: ma_Row
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                cursorShape: node.draggable ? Qt.OpenHandCursor : Qt.ArrowCursor
                drag.target: node.draggable ? dragProxy : null
                drag.threshold: 6

                /* was the row already selected before the first click of a double-click?
                   (the first click selects it, so node.selected is always true at onDoubleClicked) */
                property bool selectedBeforeClick: false

                onClicked: (mouse) => {
                    if(mouse.button === Qt.LeftButton && mouse.x >= arrow.x && mouse.x <= arrow.x + arrow.width && node.hasChildren){
                        node.treeView.toggleExpanded(node.row)
                        return
                    }
                    selectedBeforeClick = node.selected
                    node.select()
                    root.forceActiveFocus()
                }
                onDoubleClicked: (mouse) => {
                    if(node.renamable && mouse.x >= txt_Title.x && selectedBeforeClick && node.selected)
                        edit_Title.start()
                    else if(node.hasChildren)
                        node.treeView.toggleExpanded(node.row)
                }
            }
            //ToolTip.visible: hover.hovered && node.description !== "" && !ma_Row.drag.active
            //ToolTip.delay:   900
            //ToolTip.text:    node.description

            Keys.onPressed: (event) => {
                if(event.key === Qt.Key_F2 && node.renamable)
                    edit_Title.start()
            }
        }
    }
}
