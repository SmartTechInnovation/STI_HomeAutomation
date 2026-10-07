import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.impl

import "../../../../../Theme"

Item {
    id: root
    property var project: null
    property var projectTree: project ? project.tree : null
    property string selectedUuid: ""
    property string selectedIUuid: ""

    signal selected(string uuid, string iUuid)

    onProjectTreeChanged: {
        tree.expand(0)
    }

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
        model:           root.project ? root.project.tree : null

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
            // Rol din model (Qt::DisplayRole)
            required property string   display
            required property string   title
            required property string   uuid
            required property string   iUuid
            required property string   icon
            required property color    color
            required property bool     draggable


            readonly property int indent: 14

            HoverHandler { id: hover }

            // ==== Hover Background ====
            Rectangle {
                anchors.fill: parent
                color: (hover.hovered || selectedIUuid == node.iUuid) ? Qt.lighter(Theme.bgPanel, 1.5) : "transparent"
            }

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
                color:    node.color
                fillMode: Image.PreserveAspectFit
                source:   node.icon
                sourceSize.width:  16
                sourceSize.height: 16
            }

            Text{
                x: node.depth * node.indent + 44
                anchors.verticalCenter: parent.verticalCenter
                text: node.title
                color: Theme.white
            }

            MouseArea {
                anchors.fill: parent

                onClicked: (mouse) => {
                    if(node.iUuid !== "{00000000-0000-0000-0000-000000000000}"){
                        selectedUuid  = node.uuid
                        selectedIUuid = node.iUuid
                        root.selected(node.uuid, node.iUuid)
                    }
                    console.log("Element Selected ID: " + node.iUuid + " Model: " + node.uuid)
                }
                onDoubleClicked: {
                    if(node.hasChildren)
                        node.treeView.toggleExpanded(node.row)
                }
            }
        }
    }
}
