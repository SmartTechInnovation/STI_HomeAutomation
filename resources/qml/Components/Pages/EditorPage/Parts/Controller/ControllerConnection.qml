import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../Theme"

Item {
    id: controller_Connection

    enum ConnectionState_e {
        Disconected = 0,
        Connecting  = 1,
        Connected   = 2
    }

    enum NotificationState_e {
        Notif_Ok = 0,
        Notif_Warning = 1,
        Notif_Error   = 2,
        Notif_Update  = 3
    }

    property int connectionState:   ControllerConnection.Disconected
    property int notificationState: ControllerConnection.Notif_Ok
    property bool programDifferent: false
    property bool debugEnabled:     false

    Rectangle {
        id: disconected_State
        anchors.fill: parent
        anchors.margins: 5
        visible: controller_Connection.connectionState !== ControllerConnection.Connected

        color: "transparent"

        ColumnLayout {
            anchors.fill: parent

            Button {
                id: btn_Connect
                Layout.fillHeight: true
                Layout.fillWidth:  true
                padding: 1

                text: controller_Connection.connectionState === ControllerConnection.Connecting ? qsTr("Connecting...") :  qsTr("Connect to controller")
                font.bold: true
                font.pixelSize: 18
                icon.source: "qrc:/icons/misc/link-break.svg"

                icon.color: controller_Connection.connectionState === ControllerConnection.Connecting ? Theme.black : Theme.white
                palette.buttonText: icon.color

                background: Rectangle{
                    anchors.fill: parent
                    color: controller_Connection.connectionState === ControllerConnection.Connecting ? Theme.orange : btn_Connect.hovered ? Theme.bgHover : "transparent"
                    radius: 5
                }

                onClicked: console.info("Connect to device")

                HoverHandler { cursorShape: Qt.PointingHandCursor }
            }

            RowLayout {
                Layout.fillWidth:  true
                Layout.fillHeight: true

                Button {
                    id: btn_SearchControllers
                    Layout.fillHeight: true
                    Layout.fillWidth:  true
                    padding: 1

                    text: qsTr("Search")
                    font.pixelSize:18
                    icon.source: "qrc:/icons/navigation/search.svg"

                    background: Rectangle{
                        anchors.fill: parent
                        color: btn_SearchControllers.hovered ? Theme.bgHover : "transparent"
                        radius: 5
                    }

                    onClicked: console.info("Search device")

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }

                Button {
                    id: btn_ManualConnect
                    Layout.fillHeight: true
                    Layout.fillWidth:  true
                    padding: 1

                    text: qsTr("Manual")
                    font.pixelSize:18
                    icon.source: "qrc:/icons/misc/tool.svg"

                    background: Rectangle{
                        anchors.fill: parent
                        color: btn_ManualConnect.hovered ? Theme.bgHover : "transparent"
                        radius: 5
                    }

                    onClicked: console.info("Search device")

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }
            }
        }
    }
    Rectangle {
        id: connected_State
        anchors.fill: parent
        anchors.margins: 5
        visible: controller_Connection.connectionState === ControllerConnection.Connected

        color: "transparent"

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth:  true
                Rectangle{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: ma_btn_Disconnect.containsMouse ? Theme.red : Theme.green
                    radius: 5

                    Row {
                        anchors.centerIn: parent
                        spacing: 8
                        Image {
                            width:  18
                            height: 18
                            fillMode: Image.PreserveAspectFit
                            source: ma_btn_Disconnect.containsMouse  ? "qrc:/icons/misc/link-break.svg" : "qrc:/icons/misc/link.svg"
                        }

                        Text {
                            text: ma_btn_Disconnect.containsMouse ? qsTr("Disconnect") : qsTr("Connected")
                            font.pixelSize: 18
                        }
                    }

                    MouseArea{
                        id: ma_btn_Disconnect
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: console.info("Disconnect")

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                    }
                }
                // ==== Notification And Info =====
                Item {
                    Layout.fillWidth:  true
                    Layout.preferredHeight: 40
                    RowLayout {
                        anchors.right: parent.right


                        // ===== Space
                        Item{ Layout.fillWidth: true }

                        Button {
                            id: btn_SystemStatus
                            Layout.preferredHeight: 25
                            Layout.preferredWidth:  25
                            padding: 1

                            icon.source: "qrc:/icons/misc/bell.svg"

                            background: Rectangle{
                                color: btn_SystemStatus.hovered ? Theme.bgHover : "transparent"
                                radius: 2

                                Rectangle {
                                    anchors.right: parent.right
                                    visible: controller_Connection.notificationState !== ControllerConnection.Notif_Ok
                                    color: controller_Connection.notificationState === ControllerConnection.Notif_Warning ? Theme.orange : controller_Connection.notificationState === ControllerConnection.Notif_Error ? Theme.red : controller_Connection.notificationState === ControllerConnection.Notif_Update ? Theme.blue : Theme.green
                                    width:  8
                                    height: 8
                                    radius: 8
                                }
                            }

                            onClicked: console.info("open System Status")
                        }

                        Button {
                            id: btn_DeviceStatus
                            Layout.preferredHeight: 25
                            Layout.preferredWidth:  25
                            padding: 1

                            icon.source: "qrc:/icons/misc/info.svg"

                            background: Rectangle{
                                color: btn_DeviceStatus.hovered ? Theme.bgHover : "transparent"
                                radius: 2
                            }

                            onClicked: console.info("open Device Status")
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth:  true
                Button {
                    id: btn_DownloadConfig
                    Layout.fillHeight: true
                    Layout.preferredWidth: 60
                    padding: 1
                    icon.source: "qrc:/icons/action/import.svg"
                    background: Rectangle{
                        color: btn_DownloadConfig.hovered ? Theme.bgHover : "transparent"
                        radius: 5
                    }

                    onClicked: console.info("downlod Config")

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }

                Button {
                    id: btn_UploadConfig
                    Layout.fillHeight: true
                    Layout.preferredWidth: 60
                    padding: 1
                    icon.source: "qrc:/icons/action/export.svg"
                    background: Rectangle{
                        color: btn_UploadConfig.hovered ? Theme.bgHover : "transparent"
                        radius: 5
                    }

                    onClicked: console.info("upload Config")

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }

                Button {
                    id: ico_ProgramDifferent
                    Layout.fillHeight: true
                    Layout.preferredWidth: 60
                    padding: 1
                    icon.source: controller_Connection.programDifferent ? "qrc:/icons/misc/spam.svg" : "qrc:/icons/misc/success.svg"
                    background: Rectangle{
                        color: "transparent"
                    }
                    icon.color: controller_Connection.programDifferent ? Theme.orange : Theme.green
                }

                Button {
                    id: btn_Live
                    Layout.fillHeight: true
                    Layout.preferredWidth: 60
                    padding: 1
                    icon.source: "qrc:/icons/misc/debug.svg"
                    background: Rectangle{
                        color: controller_Connection.debugEnabled ? (btn_Live.hovered ? Theme.bgHover : Theme.orange) : (btn_Live.hovered ? Theme.bgHover : "transparent")
                        radius: 5
                    }

                    onClicked: controller_Connection.debugEnabled = !controller_Connection.debugEnabled

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }
            }
        }
    }

}
