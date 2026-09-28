import QtQuick
import QtQuick.Window
import QtQuick.Controls.impl

import "../../../../../Theme"

Item {
    id: root
    property var projectTreeModel

    TreeView {
        id: tree
        anchors.fill:    parent
        anchors.margins: 8
        model:           root.projectTreeModel
        clip:            true

        delegate: Item {
            id: node
            implicitWidth:  tree.width
            implicitHeight: 24

            // Proprietăți date de TreeView (nu le setezi tu)
            required property TreeView treeView
            required property bool     isTreeNode
            required property bool     expanded
            required property bool     hasChildren
            required property int      depth
            required property int      row
            // Rol din model (Qt::DisplayRole)
            required property string   display

            readonly property int indent: 16

            HoverHandler { id: hover }

            // ==== Hover Background ====
            Rectangle {
                anchors.fill: parent
                color: hover.hovered ? Qt.lighter(Theme.bgPanel, 1.5) : "transparent"
            }

            IconImage {
                id: arrowIcon
                x:  node.depth * node.indent
                anchors.verticalCenter: parent.verticalCenter
                visible:  node.hasChildren
                color:    Theme.white
                fillMode: Image.PreserveAspectFit
                source:   node.expanded ? "qrc:/icons/action/collapse.svg" : "qrc:/icons/action/expand.svg"
                sourceSize.width:  16
                sourceSize.height: 16
            }

            Text{
                x: node.depth * node.indent + 16
                anchors.verticalCenter: parent.verticalCenter
                text: node.display
                color: Theme.white
            }

            TapHandler {
                onTapped: {
                    if(node.hasChildren){
                        node.treeView.toggleExpanded(node.row)
                    }else{
                        console.log("Element Selected: " + node.display)
                    }
                }
            }

        }
    }
}
