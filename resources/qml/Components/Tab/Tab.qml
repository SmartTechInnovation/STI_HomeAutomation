import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Shapes
import QtQuick.Layouts

import "../Theme"

// Chrome-style tab (visual only). Mouse press / drag are handled by TabStrip,
// the close button and the rename editor live here.
// Editarea e gestionata INTERN: TabStrip apeleaza startEdit() si asculta
// renamed / renameCanceled, dar nu scrie niciodata in `editing`.
Item {
    id: root

    property string label:       ""
    property string icon:        ""
    property bool   active:      false
    property bool   hovered:     false
    property bool   dirty:       false
    property bool   confirmClose:false
    property bool   closable:    true
    property bool   separator:   true
    property string closeMessage: ""

    readonly property bool editing: _editing
    property bool _editing: false

    property color  activeColor: Theme.bgCanvas
    property color  hoverColor:  Qt.lighter(Theme.bgChrome, 1.6)

    readonly property int r: 8

    signal activate()
    signal closeTab()
    signal renamed(string text)
    signal renameCanceled()

    implicitHeight: 32

    // ===== Active: rounded top + bottom flares =====
    Shape {
        x:       -root.r
        width:   root.width + 2 * root.r
        height:  root.height
        visible: root.active
        layer.enabled: true
        layer.samples: 4

        ShapePath {
            fillColor:   root.activeColor
            strokeColor: "transparent"
            strokeWidth: 0

            startX: 0; startY: root.height
            PathArc  { x: root.r;                  y: root.height - root.r; radiusX: root.r; radiusY: root.r; direction: PathArc.Counterclockwise }
            PathLine { x: root.r;                  y: root.r }
            PathArc  { x: 2 * root.r;              y: 0;                    radiusX: root.r; radiusY: root.r }
            PathLine { x: root.width;              y: 0 }
            PathArc  { x: root.width + root.r;     y: root.r;               radiusX: root.r; radiusY: root.r }
            PathLine { x: root.width + root.r;     y: root.height - root.r }
            PathArc  { x: root.width + 2 * root.r; y: root.height;          radiusX: root.r; radiusY: root.r; direction: PathArc.Counterclockwise }
            PathLine { x: 0;                       y: root.height }
        }
    }

    // ===== Hover (inactive): soft pill =====
    Rectangle {
        anchors.fill: parent
        radius:  6
        visible: !root.active && root.hovered
        color:   root.hoverColor
    }

    // ===== Separator =====
    Rectangle {
        anchors.right:          parent.right
        anchors.verticalCenter: parent.verticalCenter
        width:   1
        height:  parent.height * 0.45
        color:   Theme.gray
        opacity: 0.6
        visible: root.separator && !root.active && !root.hovered
    }

    // ===== Content: [icon] label ... [● / x] =====
    Item {
        anchors.fill:        parent
        anchors.leftMargin:  10
        anchors.rightMargin: 6

        Row {
            id: row_left
            anchors.verticalCenter: parent.verticalCenter
            anchors.left:           parent.left
            anchors.right:          btn_Close.left
            anchors.rightMargin:    4
            spacing: 6

            IconImage {
                id: tab_icon
                visible: root.icon.length !== 0
                anchors.verticalCenter: parent.verticalCenter
                width:  16
                height: 16
                sourceSize.width:  16
                sourceSize.height: 16
                source: root.icon
                color:  root.active ? Theme.white : Theme.gray
            }

            Text {
                id: txt_Label
                visible: !root.editing && root.label.length !== 0
                anchors.verticalCenter: parent.verticalCenter
                width:  Math.min(implicitWidth, row_left.width - (tab_icon.visible ? tab_icon.width + 6 : 0))
                text:   root.label
                color:  root.active ? Theme.white : Qt.lighter(Theme.gray, 1.25)
                font.pixelSize: 12
                elide:  Text.ElideRight
            }

            // ===== Inline rename =====
            TextField {
                id: edit_Label
                visible: root.editing
                anchors.verticalCenter: parent.verticalCenter
                width:  row_left.width - (tab_icon.visible ? tab_icon.width + 6 : 0)
                height: 22
                padding: 2
                leftPadding: 4
                font.pixelSize: 12
                color: Theme.white
                selectByMouse: true
                background: Rectangle {
                    radius: 4
                    color: Theme.bgWindow
                    border.color: Theme.blue
                    border.width: 1
                }

                onAccepted:            root.commitEdit()      // Enter / keypad Enter
                Keys.onEscapePressed: (event) => { root.cancelEdit(); event.accepted = true }
                onActiveFocusChanged:  if (!activeFocus) root.commitEdit()   // click in alta parte = commit (ca in Chrome)
            }
        }

        // ===== Close / dirty =====
        Item {
            id: btn_Close
            anchors.right:          parent.right
            anchors.verticalCenter: parent.verticalCenter
            width:   root.closable || root.dirty ? 18 : 0
            height:  18
            visible: width > 0

            Rectangle {
                anchors.centerIn: parent
                width: 8; height: 8; radius: 4
                color:   Theme.amber
                visible: root.dirty && !(root.closable && root.hovered)
            }

            Rectangle {
                anchors.fill: parent
                radius:  9
                visible: root.closable && (root.hovered || (root.active && !root.dirty))
                color:   ma_Close.pressed ? Theme.red : ma_Close.containsMouse ? Qt.lighter(Theme.bgChrome, 2.4) : "transparent"

                IconImage {
                    anchors.centerIn: parent
                    width: 10; height: 10
                    sourceSize.width: 10; sourceSize.height: 10
                    source: "qrc:/icons/action/close.svg"
                    color:  ma_Close.containsMouse ? Theme.white : Theme.gray
                }

                MouseArea {
                    id: ma_Close
                    anchors.fill: parent
                    hoverEnabled: root.active
                    cursorShape:  Qt.PointingHandCursor
                    onClicked:    {
                        if(root.confirmClose){
                            dlg_Close.open()
                        }else{
                            root.closeTab()
                        }
                    }
                }
            }
        }
    }

    // ==== Dialog confirmare inchidere ====
    component DlgButton: Rectangle {
        id: btn
        property string text:   ""
        property color  accent: Theme.blue
        property bool   primary: false
        signal clicked()

        implicitWidth:  Math.max(84, lbl.implicitWidth + 24)
        implicitHeight: 28
        radius: 5
        color: primary
               ? (ma.pressed ? Qt.darker(accent, 1.2) : ma.containsMouse ? Qt.lighter(accent, 1.15) : accent)
               : (ma.pressed ? Theme.bgHover : ma.containsMouse ? Theme.bgHover : "transparent")
        border.width: primary ? 0 : 1
        border.color: Theme.gray

        Text {
            id: lbl
            anchors.centerIn: parent
            text:  btn.text
            color: Theme.white
            font.pixelSize: 12
        }
        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            cursorShape:  Qt.PointingHandCursor
            onClicked:    btn.clicked()
        }
    }

    Popup {
        id: dlg_Close
        parent:           Overlay.overlay
        anchors.centerIn: parent
        modal:  true
        focus:  true
        padding: 16
        width:  340
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Overlay.modal: Rectangle {
            color: Qt.rgba(Theme.black.r, Theme.black.g, Theme.black.b, 0.55)
        }

        background: Rectangle {
            radius:       8
            color:        Theme.bgElevated
            border.color: Theme.gray
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: 10

            Text {
                Layout.fillWidth: true
                text:  qsTr("Close „%1”?").arg(root.label)
                color: Theme.white
                font.pixelSize: 14
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: Theme.gray
                font.pixelSize: 12
                text: root.closeMessage
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 6
                spacing: 8

                Item { Layout.fillWidth: true }

                DlgButton {
                    text: qsTr("Cancel")
                    onClicked: dlg_Close.close()
                }
                DlgButton {
                    text:    qsTr("Close")
                    primary: true
                    accent:  root.dirty ? Theme.red : Theme.blue
                    onClicked: {
                        dlg_Close.close()
                        root.closeTab()
                    }
                }
            }
        }
    }

    function startEdit() {
        edit_Label.text = root.label
        root._editing = true
        edit_Label.forceActiveFocus()
        edit_Label.selectAll()
    }

    // _editing = false INAINTE de emit: cand campul se ascunde pierde focusul,
    // iar onActiveFocusChanged nu mai emite a doua oara.
    function commitEdit() {
        if (!root._editing) return
        root._editing = false
        const t = edit_Label.text.trim()
        if (t.length === 0 || t === root.label) root.renameCanceled()
        else                                    root.renamed(t)
    }

    function cancelEdit() {
        if (!root._editing) return
        root._editing = false
        root.renameCanceled()
    }
}