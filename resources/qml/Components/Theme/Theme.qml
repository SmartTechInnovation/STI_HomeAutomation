pragma Singleton
import QtQuick

QtObject {
    id: theme

    readonly property color green: "#A4D874"
    readonly property color white: "#F7F7F7"
    readonly property color black: "#272727"
    readonly property color gray:  "#7D8491"
    readonly property color blue:  "#3D5ADD"
    readonly property color orange:"#FF8400"
    readonly property color yellow:"#E8C547"

    // ======== Surfaces =========
    readonly property color red:   "#DD3D5A"
    readonly property color amber: "#D8A474"

    // ======== Backgrouds ========
    readonly property color bgChrome:   Qt.darker(black, 1.45)      // TitleBar, Tabs, Statusbar
    readonly property color bgCanvas:   Qt.darker(black, 1.25)      // Background
    readonly property color bgStrip:    Qt.darker(black, 1.9)
    readonly property color bgWindow:   black
    readonly property color bgPanel:    Qt.lighter(black, 1.18)
    readonly property color bgElevated: Qt.lighter(black, 1.5)
    readonly property color bgHover:    Qt.lighter(black, 1.85)
    readonly property color bgTab:      Qt.lighter(black, 1.90)

    // ======== Paper (pages - same look on screen and in PDF) ========
    readonly property color paper:      "#F9FBFF"
    readonly property color ink:        black
    readonly property color inkMuted:   gray
    readonly property color inkFaint:   "#D9DCE1"
}