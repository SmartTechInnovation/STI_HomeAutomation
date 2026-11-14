import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import "../../../../../../Theme"

Item {
    id: root

    property var    ref:          null
    property string title:        ref ? ref.title : ""
    property string icon:         ref ? ref.icon  : ""
    property color  color:        ref ? ref.color : Theme.gray
    property real   defaultWidth: ref ? ref.width : 200

    property real   worldScale:    1
    property int    snap:          10
    property bool   ghost:         false

    property real   lastX: 0
    property real   lastY: 0

    property bool   selected: false

    property var    inputs:     ref ? ref.inputs    : []
    property var    outputs:    ref ? ref.outputs   : []

    readonly property real rowSpacing: 30

    implicitWidth:  body.implicitWidth
    implicitHeight: body.implicitHeight
    width:  implicitWidth
    height: implicitHeight

    opacity: ghost ? 0.6 : 1
    layer.enabled: ghost

    x: ref ? ref.pX : 0
    y: ref ? ref.pY : 0
    z: selected ? 3 : 2

    signal clicked ()
    signal pressed (real wx, real wy)
    signal dragged (real wx, real wy)
    signal released(real wx, real wy)
    signal portPressed (int index, bool isOutput)
    signal portDragged (real wx,   real wy)
    signal portReleased(real wx,   real wy)

    function move(dX, dY){
        root.x = Math.max(0, Math.min(root.parent.width  - root.width,  root.x + dX))
        root.y = Math.max(0, Math.min(root.parent.height - root.height, root.y + dY))
    }

    //===== Drag Area ======
    MouseArea {
        anchors.fill: parent

        onClicked: (mouse) => {
            root.clicked()
        }

        onPressed: (mouse) => {
            root.lastX = mouse.x
            root.lastY = mouse.y
            root.pressed(root.x, root.y)
        }
        onPositionChanged: (mouse) => {
            if(pressed){
                var nX = root.x + (mouse.x - root.lastX)
                var nY = root.y + (mouse.y - root.lastY)
                if(nX < 0) nX = 0
                if(nY < 0) nY = 0
                if(nX > (root.parent.width - root.width))   nX = root.parent.width - root.width
                if(nY > (root.parent.height - root.height)) nY = root.parent.height - root.height
                nX     = Math.round(nX / snap) * snap
                nY     = Math.round(nY / snap) * snap

                var dX = nX - root.x
                var dY = nY - root.y
                if(dX === 0 && dY === 0) return

                root.x = nX
                root.y = nY
                root.dragged(dX, dY)
            }
        }
        onReleased: (mouse) => {
            root.released(root.x, root.y)
        }
    }

    Rectangle {
        id: body

        anchors.fill: parent
        radius: 10
        color: root.color

        border.width: root.selected ? 3 : 2
        border.color: root.selected ? Qt.darker(root.color) : root.color

        implicitWidth:  Math.max(root.defaultWidth, mainRow.implicitWidth  + 2 * mainRow.anchors.margins)
        implicitHeight: mainRow.implicitHeight + 2 * mainRow.anchors.margins

        RowLayout {
            id: mainRow
            anchors.fill:    parent
            anchors.margins: 2
            spacing: 0

            // ---- Inputs ---
            Column {
                id: inputsColumn
                Layout.preferredWidth: 30
                Layout.alignment: Qt.AlignTop
                spacing: 2

                Repeater{
                    model: root.inputs
                    delegate: Item {
                        required property int index
                        required property var modelData
                        width:  inputsColumn.width
                        height: root.rowSpacing

                        Text {
                            x: 28
                            anchors.verticalCenter: parent.verticalCenter
                            text:  modelData.shortName
                            color: "black"
                            font.pixelSize: 14
                        }
                        Port {
                            x: 6
                            anchors.verticalCenter: parent.verticalCenter
                            portModel: modelData
                            parentRef: root.parent
                            onPressedAt:  root.portPressed(index, false)
                            onDraggedTo:  (wx, wy) => root.portDragged(wx, wy)
                            onReleasedAt: (wx, wy) => root.portReleased(wx, wy)
                            worldScale: root.worldScale
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
                Layout.margins: 10
                Text {
                    id: label
                    anchors.fill: parent
                    text: modelData.title
                    font.pixelSize: 14
                    font.bold: true
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
            }

            // ---- Outputs (mirror, cand vei avea date) ----
            Column {
                id: outputsColumn
                Layout.preferredWidth: 30
                Layout.alignment: Qt.AlignTop
                spacing: 2

                Repeater{
                    model: root.outputs
                    delegate: Item {
                        required property int index
                        required property var modelData
                        width:  outputsColumn.width
                        height: root.rowSpacing

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 25
                            anchors.verticalCenter: parent.verticalCenter
                            text:  modelData.shortName
                            color: "black"
                            font.pixelSize: 14
                        }
                        Port {
                            anchors.right: parent.right
                            anchors.rightMargin: 6
                            anchors.verticalCenter: parent.verticalCenter
                            portModel: modelData
                            parentRef: root.parent
                            onPressedAt:  root.portPressed(index, true)
                            onDraggedTo:  (wx, wy) => root.portDragged(wx, wy)
                            onReleasedAt: (wx, wy) => root.portReleased(wx, wy)
                            worldScale: root.worldScale
                        }
                    }
                }
            }
        }

    }
}
