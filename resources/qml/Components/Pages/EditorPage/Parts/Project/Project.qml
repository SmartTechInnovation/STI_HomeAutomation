import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../Theme"
import "Pages"

Item {
    id: root

    property var projectPages

    StackLayout {
        width: parent.width
        height: parent.height

        currentIndex: pageTabs.currentIndex

        Rectangle {
            id: homeTab
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "green"
        }


        Page {
            id: page1
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Page {
            id: page2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    TabBar {
        id: pageTabs
        anchors.top: parent.top
        width: parent.width
        background: Rectangle {
            color: "transparent"
        }

        TabButton {
            text: qsTr("Home")
        }
        TabButton {
            text: qsTr("Page 1")
        }
        TabButton {
            text: qsTr("Page 2")
        }
    }
}
