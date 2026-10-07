import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: root

    property string blockInstUuid: ""
    property string blockTypeUuid: ""
    property string blockType:     ""
    property string blockTitle:    ""
    property string blockIcon:     ""
    property color  blockColor:    "#FF7D30"
    property real   defaultWidth:  400
    property real   worldScale:    1

    property var    inputs:     ({})
    property var    outputs:    ({})
    property var    properties: ({})

    property bool   b_Selected: false
    property bool   b_Compact:  false

    readonly property real rowSpacing: 30

    readonly property var allInputs: {
        var res = []
        for (var i = 0; i < root.inputs.length; ++i) {
            res.push(Object.assign({}, root.inputs[i],
                                   { src: "input", srcIndex: i }))
        }
        for (var j = 0; j < root.properties.length; ++j) {
            // default-uri pentru proprietati; ce e in properties[j] le suprascrie
            res.push(Object.assign({ visible: false, connected: false, color: root.propertyPortColor },
                                   root.properties[j],
                                   { src: "property", srcIndex: j }))
        }
        return res
    }

    readonly property var allOutputs: root.outputs.map(function(o, i) {
        return Object.assign({}, o, { src: "output", srcIndex: i })
    })

    readonly property var visibleInputs:  root.allInputs.filter (function(e) { return e.visible || e.connected })
    readonly property var visibleOutputs: root.allOutputs.filter(function(e) { return e.visible || e.connected })


    implicitWidth:  body.implicitWidth
    implicitHeight: body.implicitHeight
    width:  implicitWidth
    height: implicitHeight

    signal blockPressed(real wx, real wy)
    signal blockDragged(real wx, real wy)
    signal blockReleased(real wx, real wy)
    signal portPressed(int index, bool isOutput)
    signal portDragged(real wx,  real wy)
    signal portReleased(real wx, real wy)

    function toggleInputVisible(fullIndex) {
        var e = root.allInputs[fullIndex]
        if (!e || e.connected) return

        if (e.src === "property") {
            var parr = root.properties.map(function(o) { return Object.assign({}, o) })
            parr[e.srcIndex].visible = !(parr[e.srcIndex].visible === true)
            root.properties = parr
        } else {
            var arr = root.inputs.map(function(o) { return Object.assign({}, o) })
            arr[e.srcIndex].visible = !arr[e.srcIndex].visible
            root.inputs = arr
        }
    }

    function toggleOutputVisible(fullIndex) {
        if (root.outputs[fullIndex].connected) return
        var arr = root.outputs.map(function(o) { return Object.assign({}, o) })
        arr[fullIndex].visible = !arr[fullIndex].visible
        root.outputs = arr
    }

    //===== Drag Area ======
    MouseArea {
        anchors.fill: parent

        property real lastX: 0
        property real lastY: 0

        onPressed: (mouse) => {
            lastX = mouse.x
            lastY = mouse.y
            root.blockPressed(root.x, root.y)
        }

        onPositionChanged: (mouse) => {
            if(pressed){
                var nX = root.x + (mouse.x - lastX)
                var nY = root.y + (mouse.y - lastY)
                if(nX < 0) nX = 0
                if(nY < 0) nY = 0
                if(nX > (root.parent.width - root.width))   nX = root.parent.width - root.width
                if(nY > (root.parent.height - root.height)) nY = root.parent.height - root.height
                root.x = nX
                root.y = nY
                root.blockDragged(root.x, root.y)
            }
        }

        onReleased: (mouse) => {
            root.blockReleased(root.x, root.y)
        }
    }

    // ======= Block Body ========
    Rectangle {
        id: body
        anchors.fill: parent
        radius:       10
        color:        root.blockColor
        border.width: 2
        border.color: root.b_Selected ? "#000000" : root.blockColor

        implicitWidth:  Math.max(root.defaultWidth, mainColumn.implicitWidth  + 2 * mainColumn.anchors.margins)
        implicitHeight: mainColumn.implicitHeight + 2 * mainColumn.anchors.margins

        ColumnLayout{
            id: mainColumn
            anchors.fill: parent
            anchors.margins: 2
            spacing: 0

            RowLayout{
                Layout.fillWidth: true
                Layout.preferredHeight: 28

                Text {
                    id: label
                    Layout.margins: 10
                    text: root.blockTitle
                    font.pixelSize: 14
                    font.bold: true
                    color: "white"
                }
                Item { Layout.fillWidth: true }
            }

            Rectangle {
                id: blockTypeLabel
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                visible: !root.b_Compact
                color: "#f7f7f7"

                Text {
                    anchors.fill: parent
                    text: root.blockType
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
                            model: root.visibleInputs
                            delegate: Item {
                                required property int index
                                required property var modelData
                                width:  inputsColumn.width
                                height: root.rowSpacing

                                Text {
                                    x: 28
                                    anchors.verticalCenter: parent.verticalCenter
                                    text:  modelData.name
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
                                items: root.allInputs
                                onToggleVisible: (fullIndex) => toggleInputVisible(fullIndex)
                                y: parent.width
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
                            model: root.visibleOutputs
                            delegate: Item {
                                required property int index
                                required property var modelData
                                width:  outputsColumn.width
                                height: root.rowSpacing

                                Text {
                                    anchors.right: parent.right
                                    anchors.rightMargin: 25
                                    anchors.verticalCenter: parent.verticalCenter
                                    text:  modelData.name
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
                                items: root.allOutputs
                                onToggleVisible: (fullIndex) => toggleOutputVisible(fullIndex)
                                y: parent.width
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