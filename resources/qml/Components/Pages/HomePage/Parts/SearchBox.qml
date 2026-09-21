import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../../../Theme"

Rectangle {
    width:  220
    height: 34

    color: Theme.bgHover
    radius: 5

    RowLayout {
        width:  parent.width
        height: parent.height
        Button {
            Layout.preferredWidth:  34
            Layout.preferredHeight: 34
            icon.source: "qrc:/icons/navigation/search.svg"

            background: Rectangle{
                color: "transparent"
            }
        }

        TextField {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            Layout.rightMargin: 5
            placeholderText: "Search..."

            background: Rectangle {
                color: "transparent"
            }
        }
    }
}