import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string str_Name: "Unnamed"
    property string str_Type: "VI"
    property bool   b_Selected: false
    property bool   b_VisibleError: false


    signal signalPressed(real wx, real wy)
    signal signalDragged(real wx, real wy)
    signal signalReleased(real wx, real wy)
    signal portPressed()
    signal portDragged(real wx,  real wy)
    signal portReleased(real wx, real wy)


    implicitWidth:  body.implicitWidth
    implicitHeight: body.implicitHeight

    // ===== Drag Area ====
    MouseArea {
        anchors.fill: parent

        property real lastX: 0
        property real lastY: 0

        onPressed: (mouse) => {
            lastX = mouse.x
            lastY = mouse.y
            b_Selected = true
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
                root.x = nX
                root.y = nY
                root.signalDragged(root.x, root.y)
            }
        }

        onReleased: (mouse) => {
            b_Selected = true
            root.signalReleased(root.x, root.y)
        }

    }

    // ====== Signal Body ======
    Rectangle {
        id: body
        anchors.fill: parent
        radius: 15
        implicitWidth : 200
        implicitHeight: 30
        color: root.b_Selected ? "gray" : "white"

        border.width: 2
        border.color: root.b_Selected ? "#000000" : color

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Item { Layout.fillWidth: true }
            Text {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                text: root.str_Name
                font.pixelSize: 14
                font.bold:      true
                color: root.b_Selected ? "white" : "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment:   Text.AlignVCenter
            }
            Port {
                Layout.rightMargin: 10
                onPressedAt: root.portPressed()
                onDraggedTo: (wx, wy) => root.portDragged(wx, wy)
                onReleasedAt: (wx, wy) => root.portReleased(wx, wy)
            }
        }

    }
}
