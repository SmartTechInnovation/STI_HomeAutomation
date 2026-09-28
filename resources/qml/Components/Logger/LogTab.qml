import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.impl

import "../Theme"

import STI.Logger

// Stiva de notificari in dreapta-jos.
// Se alimenteaza din C++: S_Logger.write(Log::Error, "Comp", "Mesaj", "Titlu", "Sfat", true);
Item {
    id: root

    anchors.right:   parent ? parent.right  : undefined
    anchors.bottom:  parent ? parent.bottom : undefined
    anchors.margins: 16

    width:  380
    height: column_Toasts.implicitHeight
    z: 1000

    property int prop_rw_MaxToasts:   5
    property int prop_rw_InfoTimeout: 5000   // ms
    property int prop_rw_WarnTimeout: 8000   // ms  (erorile raman pana le inchide userul)

    ListModel { id: model_Toasts }

    Connections {
        target: Logger
        function onNotify(type, title, message, tips) {
            model_Toasts.append({ level: type, title: title, message: message, tips: tips })
            while (model_Toasts.count > root.prop_rw_MaxToasts)
                model_Toasts.remove(0)
        }
    }

    Column {
        id: column_Toasts
        width: parent.width
        spacing: 8

        add:  Transition { NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 150 } }
        move: Transition { NumberAnimation { property: "y"; duration: 150; easing.type: Easing.OutCubic } }

        Repeater {
            model: model_Toasts

            delegate: Rectangle {
                id: toast

                required property int    index
                required property int    level
                required property string title
                required property string message
                required property string tips

                readonly property bool  isError: level === Log.Error || level === Log.Critical
                readonly property color accent:  isError               ? Theme.red
                                               : level === Log.Warning ? Theme.yellow
                                               : level === Log.Info    ? Theme.green
                                               :                         Theme.gray

                width:  column_Toasts.width
                height: contentLayout.implicitHeight + 20
                radius: 6
                color:  Theme.bgElevated
                border.color: accent
                border.width: 1

                // tenta de culoare peste fundal
                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    color:  Qt.alpha(toast.accent, 0.15)
                }

                // banda colorata in stanga
                Rectangle {
                    anchors { left: parent.left; top: parent.top; bottom: parent.bottom; margins: 1 }
                    width: 4
                    radius: 2
                    color: toast.accent
                }

                HoverHandler { id: hover_Toast }

                // Info/Warning se inchid singure; timer-ul sta pe pauza cat timp e mouse-ul deasupra
                Timer {
                    interval: toast.level === Log.Warning ? root.prop_rw_WarnTimeout : root.prop_rw_InfoTimeout
                    running:  !toast.isError && !hover_Toast.hovered
                    onTriggered: model_Toasts.remove(toast.index)
                }

                RowLayout {
                    id: contentLayout
                    anchors { left: parent.left; right: parent.right; top: parent.top }
                    anchors.margins:    10
                    anchors.leftMargin: 14
                    spacing: 10

                    IconImage {
                        Layout.preferredWidth:  22
                        Layout.preferredHeight: 22
                        Layout.alignment: Qt.AlignTop
                        color: toast.accent
                        fillMode: Image.PreserveAspectFit
                        sourceSize.width:  width
                        sourceSize.height: height
                        source: toast.isError               ? "qrc:/icons/misc/unavailable.svg"
                              : toast.level === Log.Warning ? "qrc:/icons/misc/warning.svg"
                              : toast.level === Log.Info    ? "qrc:/icons/misc/info.svg"
                              :                               "qrc:/icons/misc/debug.svg"
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            Layout.fillWidth: true
                            text: toast.title
                            color: Theme.white
                            font.bold: true
                            font.pixelSize: 14
                            wrapMode: Text.Wrap
                        }
                        Text {
                            Layout.fillWidth: true
                            text: toast.message
                            color: Theme.white
                            opacity: 0.85
                            font.pixelSize: 12
                            wrapMode: Text.Wrap
                        }
                        Text {
                            Layout.fillWidth: true
                            visible: toast.tips !== ""
                            text: toast.tips
                            color: Theme.gray
                            font.italic: true
                            font.pixelSize: 11
                            wrapMode: Text.Wrap
                        }
                    }

                    // buton inchidere
                    Rectangle {
                        Layout.preferredWidth:  20
                        Layout.preferredHeight: 20
                        Layout.alignment: Qt.AlignTop
                        radius: 3
                        color: hover_Close.hovered ? Theme.bgHover : "transparent"

                        IconImage {
                            anchors.centerIn: parent
                            width: 14; height: 14
                            sourceSize.width: width; sourceSize.height: height
                            source: "qrc:/icons/action/close-small.svg"
                            color: Theme.white
                        }
                        HoverHandler { id: hover_Close; cursorShape: Qt.PointingHandCursor }
                        TapHandler   { onTapped: model_Toasts.remove(toast.index) }
                    }
                }
            }
        }
    }
}
