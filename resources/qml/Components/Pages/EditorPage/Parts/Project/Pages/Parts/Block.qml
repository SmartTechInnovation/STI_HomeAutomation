import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: root

    property var    ref:           null
    property string type:     ref ? ref.type  : ""
    property string title:    ref ? ref.title : ""
    property string icon:     ref ? ref.icon  : ""
    property color  color:    ref ? ref.color : "#FF7D30"
    property real   defaultWidth:  ref ? ref.width : 400
    property real   worldScale:    1
    property int    snap:          10
    property bool   ghost:         false

    property real lastX: 0
    property real lastY: 0

    property var    inputs:     ref ? ref.inputs    : []
    property var    outputs:    ref ? ref.outputs   : []

    property bool   selected:  false
    property bool   b_Compact: ref ? ref.compact : false

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

    signal clicked()
    signal pressed(real wx, real wy)
    signal dragged(real wx, real wy)
    signal released(real wx, real wy)
    signal portPressed(int index, bool isOutput)
    signal portDragged(real wx,  real wy)
    signal portReleased(real wx, real wy)

    function move(dX, dY){
        root.x = Math.max(0, Math.min(root.parent.width  - root.width,  root.x + dX))
        root.y = Math.max(0, Math.min(root.parent.height - root.height, root.y + dY))
    }

    function toggleInputVisible(fullIndex) {
        var item = root.inputs[fullIndex]
        if (!item || item.connected)
            return

        var arr = root.inputs.map(function(o) {
            return Object.assign({}, o)
        })
        arr[fullIndex].visible = !arr[fullIndex].visible
        root.inputs = arr
    }

    function toggleOutputVisible(fullIndex) {
        if (root.outputs[fullIndex].connected)
            return
        var arr = root.outputs.map(function(o) {
            return Object.assign({}, o)
        })
        arr[fullIndex].visible = !arr[fullIndex].visible
        root.outputs = arr
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

    // ======= Block Body ========
    Rectangle {
        id: body
        anchors.fill: parent
        radius:       10
        color:        root.color
        border.width: root.selected ? 3 : 2
        border.color: root.selected ? Qt.darker(root.color) : root.color

        implicitWidth:  Math.max(root.defaultWidth, mainColumn.implicitWidth  + 2 * mainColumn.anchors.margins)
        implicitHeight: mainColumn.implicitHeight + 2 * mainColumn.anchors.margins

        ColumnLayout{
            id: mainColumn
            anchors.fill: parent
            anchors.margins: 3
            spacing: 0

            RowLayout{
                Layout.fillWidth: true
                Layout.preferredHeight: 28

                Text {
                    id: label
                    Layout.margins: 10
                    text: root.title
                    font.pixelSize: 14
                    font.bold: true
                    color: "white"
                }
                Item { Layout.fillWidth: true }
            }

            Rectangle {
                id: typeLabel
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                visible: !root.b_Compact
                color: "#f7f7f7"

                Text {
                    anchors.fill: parent
                    text: root.type
                    font.pixelSize: 14
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
            }

            // ==== Zona conexiuni: inaltimea vine din cea mai inalta coloana (inputs vs outputs) ====
            Rectangle {
                id: blockConnections
                Layout.fillWidth: true
                Layout.preferredHeight: connectionsRow.implicitHeight + 8   // 8 = padding sus/jos
                color: "white"

                RowLayout{
                    id: connectionsRow
                    anchors.fill: parent
                    anchors.margins: 4

                    // ---- Inputs ----
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
                                    parentRef: root.parent
                                    onPressedAt:  root.portPressed(index, false)
                                    onDraggedTo:  (wx, wy) => root.portDragged(wx, wy)
                                    onReleasedAt: (wx, wy) => root.portReleased(wx, wy)
                                    worldScale: root.worldScale
                                }
                            }
                        }

                        Rectangle {
                            id: addInputButton
                            anchors.left: parent.left
                            width:  20
                            height: 20
                            radius: 4
                            color:  ma_AddInput.containsMouse ? "#e0e0e0" : "transparent"
                            border.width: 1
                            border.color: "#7D8491"

                            Text {
                                anchors.centerIn: parent
                                text: "+"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#272727"
                            }

                            MouseArea {
                                id: ma_AddInput
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: inputsMenu.opened ? inputsMenu.close() : inputsMenu.open()
                            }

                            DisplayConnections {
                                id: inputsMenu
                                items: root.inputs
                                onToggleVisible: (fullIndex) => toggleInputVisible(fullIndex)
                                y: parent.width

                                //scale: root.worldScale
                            }
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
                                    parentRef: root.parent
                                    onPressedAt:  root.portPressed(index, true)
                                    onDraggedTo:  (wx, wy) => root.portDragged(wx, wy)
                                    onReleasedAt: (wx, wy) => root.portReleased(wx, wy)
                                    worldScale: root.worldScale
                                }
                            }
                        }

                        Rectangle {
                            id: addOutputButton
                            anchors.right: parent.right
                            width:  20
                            height: 20
                            radius: 4
                            color:  ma_AddOutput.containsMouse ? "#e0e0e0" : "transparent"
                            border.width: 1
                            border.color: "#7D8491"

                            Text {
                                anchors.centerIn: parent
                                text: "+"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#272727"
                            }

                            MouseArea {
                                id: ma_AddOutput
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: outputsMenu.opened ? outputsMenu.close() : outputsMenu.open()
                            }

                            DisplayConnections {
                                id: outputsMenu
                                items: root.outputs
                                onToggleVisible: (fullIndex) => toggleOutputVisible(fullIndex)
                                y: parent.width

                                //scale: root.worldScale
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth:       true
                Layout.preferredHeight: 10
            }
        }
    }
}