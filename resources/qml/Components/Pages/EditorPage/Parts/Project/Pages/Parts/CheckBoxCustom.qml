// CheckBox.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Shapes

CheckBox {
    id: root

    property color colorChecked:         "#A4D874"
    property color colorCheckedDisabled: "#C7DDB0"   // verde stins, dar tot verde -- se vede ca e bifat
    property color colorUnchecked:       "#7D8491"
    property color colorDisabled:        "#D9DCE1"
    property color colorText:            "#272727"
    property color colorTextDim:         "#7D8491"

    readonly property real boxSize: 16

    spacing: 8

    indicator: Rectangle {
        id: box
        implicitWidth:  root.boxSize
        implicitHeight: root.boxSize
        x: 0
        y: (root.height - height) / 2
        radius: 4
        antialiasing: true

        // checked decide DACA se umple; enabled decide doar CE nuanta
        color: root.checked
                   ? (root.enabled ? root.colorChecked : root.colorCheckedDisabled)
                   : "transparent"

        border.width: 1.5
        border.color: root.checked
                          ? (root.enabled ? root.colorChecked : root.colorCheckedDisabled)
                          : (!root.enabled
                                ? root.colorDisabled
                                : (ma_hover.containsMouse ? root.colorText : root.colorUnchecked))

        Behavior on color        { ColorAnimation { duration: 100 } }
        Behavior on border.color { ColorAnimation { duration: 100 } }

        Shape {
            id: check
            anchors.centerIn: parent
            width:  parent.width  * 0.6
            height: parent.height * 0.6
            antialiasing: true

            // vizibilitatea depinde DOAR de checked, niciodata de enabled
            opacity: root.checked ? 1 : 0
            scale:   root.checked ? 1 : 0.4
            Behavior on opacity { NumberAnimation { duration: 90 } }
            Behavior on scale   { NumberAnimation { duration: 120; easing.type: Easing.OutBack } }

            ShapePath {
                strokeWidth: 2
                // pe fundal verde (checked=true) checkmark-ul e mereu alb, indiferent de enabled
                strokeColor: "#FFFFFF"
                fillColor: "transparent"
                capStyle:  ShapePath.RoundCap
                joinStyle: ShapePath.RoundJoin

                startX: check.width * 0.06; startY: check.height * 0.55
                PathLine { x: check.width * 0.40; y: check.height * 0.88 }
                PathLine { x: check.width * 0.95; y: check.height * 0.12 }
            }
        }

        MouseArea {
            id: ma_hover
            anchors.fill: parent
            anchors.margins: -4
            hoverEnabled: true
            enabled: false
        }
    }

    contentItem: Text {
        text: root.text
        leftPadding: root.indicator.width + root.spacing
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        color: root.enabled ? root.colorText : root.colorTextDim
    }
}