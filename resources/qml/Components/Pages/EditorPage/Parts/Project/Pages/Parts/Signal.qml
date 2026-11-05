import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import "../../../../../../Theme"

Item {
    id: root

    property var    signalRef:    null
    property string title:        signalRef ? signalRef.title : ""
    property string icon:         signalRef ? signalRef.icon  : ""
    property color  signalColor:  signalRef ? signalRef.color : Theme.gray
    property real   defaultWidth: signalRef ? signalRef.width : 200

    property real   worldScale:    1
    property int    snap:          10
    property bool   ghost:         false

    property bool   selected: false

    property var    inputs:     signalRef ? signalRef.inputs    : []
    property var    outputs:    signalRef ? signalRef.outputs   : []

    readonly property real rowSpacing: 30

    implicitWidth:  body.implicitWidth
    implicitHeight: body.implicitHeight
    width:  implicitWidth
    height: implicitHeight

    opacity: ghost ? 0.6 : 1
    layer.enabled: ghost

    x: signalRef ? signalRef.pX : 0
    y: signalRef ? signalRef.pY : 0
    z: selected ? 3 : 2

    signal signalPressed(real wx, real wy)
    signal signalDragged(real wx, real wy)
    signal signalReleased(real wx, real wy)
    signal portPressed(int index, bool isOutput)
    signal portDragged(real wx,  real wy)
    signal portReleased(real wx, real wy)

    //===== Drag Area ======
    MouseArea {
        anchors.fill: parent

        property real lastX: 0
        property real lastY: 0

        onPressed: (mouse) => {
            lastX = mouse.x
            lastY = mouse.y
            root.signalPressed(root.x, root.y)
        }
        onPositionChanged: (mouse) => {
            if(pressed){
                var nX = root.x + (mouse.x - lastX)
                var nY = root.y + (mouse.y - lastY)
                if(nX < 0) nX = 0
                if(nY < 0) nY = 0
                if(nX > (root.parent.width - root.width))   nX = root.parent.width - root.width
                if(nY > (root.parent.height - root.height)) nY = root.parent.height - root.height
                nX     = Math.round(nX / snap) * snap
                nY     = Math.round(nY / snap) * snap
                root.x = nX
                root.y = nY
                root.signalDragged(root.x, root.y)
            }
        }
        onReleased: (mouse) => {
            root.signalReleased(root.x, root.y)
        }
    }

    Rectangle {
        id: body

        anchors.fill: parent
        radius: 10
        color: root.signalColor

        border.width: root.selected ? 3 : 2
        border.color: root.selected ? Qt.darker(root.signalColor) : root.signalColor

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
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                spacing: 2

                Repeater{
                    model: root.inputs.filter (function(e) { return e.visible || e.connected })
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
                            blockRef: root.parent
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
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                spacing: 2

                Repeater{
                    model: root.outputs.filter(function(e) { return e.visible || e.connected })
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
                            blockRef: root.parent
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
