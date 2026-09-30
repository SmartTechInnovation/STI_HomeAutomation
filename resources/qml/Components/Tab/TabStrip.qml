import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl

import "../Theme"

// Row of Chrome-style tabs: equal widths, drag to reorder, "+" button.
// model: JS array of { title, icon?, dirty?, number? }
Item {
    id: root

    property var    model:        []
    property int    currentIndex: -1
    property bool   closable:     true
    property bool   reorderable:  true
    property bool   renamable:    false
    property bool   showAdd:      false
    property bool   confirmClose: false
    property string closeMessage: "Do you want delete page ?"
    property string defaultIcon:  ""
    property int    minTabWidth:  80
    property int    maxTabWidth:  210
    property color  activeColor:  Theme.bgCanvas
    property color  hoverColor:   Qt.lighter(Theme.bgChrome, 1.5)
    property bool   leadingSeparator: false   // separator before first tab (pinned tab on the left)
    property string addToolTip:   qsTr("New")

    signal activated(int index)
    signal closeRequested(int index)
    signal moved(int from, int to)
    signal addRequested()
    signal renamed(int index, string text)
    signal contextRequested(int index, real sceneX, real sceneY)

    readonly property int  count:    model ? model.length : 0
    readonly property int  addWidth: showAdd ? 34 : 0
    readonly property int  pad:      8     // room for the flares of the first / last tab
    readonly property real tabWidth: count === 0 ? maxTabWidth
                                   : Math.max(minTabWidth, Math.min(maxTabWidth, (width - addWidth - 2 * pad) / count))

    // ===== Drag state =====
    property int  dragIndex:    -1
    property real dragDx:       0
    property int  hoveredIndex: -1
    property int  editIndex:    -1
    readonly property int targetIndex: dragIndex < 0 ? -1
                          : Math.max(0, Math.min(count - 1, Math.round((dragIndex * tabWidth + dragDx) / tabWidth)))

    function shiftFor(i){
        if(dragIndex < 0 || i === dragIndex) return 0
        if(dragIndex < targetIndex && i > dragIndex && i <= targetIndex) return -tabWidth
        if(dragIndex > targetIndex && i < dragIndex && i >= targetIndex) return  tabWidth
        return 0
    }

    function startRename(i){
        if(!renamable || i < 0 || i >= count) return
        editIndex = i
        var item = rep_Tabs.itemAt(i)
        if(item) item.tab.startEdit()
    }

    implicitHeight: 32

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth:  root.pad * 2 + root.count * root.tabWidth + root.addWidth
        contentHeight: height
        interactive:   contentWidth > width && root.dragIndex < 0
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        ScrollBar.horizontal: ScrollBar {
            height: 3
            policy: flick.contentWidth > flick.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        Rectangle {    // separator before first tab
            x: root.pad - 1
            anchors.verticalCenter: parent.verticalCenter
            width: 1; height: parent.height * 0.45
            color: Theme.gray; opacity: 0.6
            visible: root.leadingSeparator && root.currentIndex !== 0 && root.hoveredIndex !== 0
        }

        Repeater {
            id: rep_Tabs
            model: root.model

            delegate: Item {
                id: tabItem
                required property int index
                required property var modelData
                property alias tab: tab

                readonly property bool dragged: root.dragIndex === index

                x: root.pad + index * root.tabWidth + (dragged ? root.dragDx : root.shiftFor(index))
                z: dragged ? 10 : (root.currentIndex === index ? 5 : 0)
                width:  root.tabWidth
                height: flick.height

                Behavior on x {
                    enabled: !tabItem.dragged
                    NumberAnimation { duration: 140; easing.type: Easing.OutCubic }
                }

                MouseArea {
                    id: ma_Tab
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
                    cursorShape: Qt.PointingHandCursor

                    property real pressX: 0
                    property bool dragging: false

                    onContainsMouseChanged: {
                        if(containsMouse) root.hoveredIndex = tabItem.index
                        else if(root.hoveredIndex === tabItem.index) root.hoveredIndex = -1
                    }

                    onPressed: (mouse) => {
                        if(mouse.button === Qt.LeftButton){
                            pressX   = mapToItem(flick.contentItem, mouse.x, 0).x
                            dragging = false
                            root.activated(tabItem.index)
                        }
                    }
                    onPositionChanged: (mouse) => {
                        if(!(pressedButtons & Qt.LeftButton) || !root.reorderable || root.editIndex >= 0) return
                        var dx = mapToItem(flick.contentItem, mouse.x, 0).x - pressX
                        if(!dragging && Math.abs(dx) > 6){
                            dragging = true
                            root.dragIndex = tabItem.index
                        }
                        if(dragging){
                            // keep the tab inside the strip
                            var minDx = -tabItem.index * root.tabWidth
                            var maxDx = (root.count - 1 - tabItem.index) * root.tabWidth
                            root.dragDx = Math.max(minDx, Math.min(maxDx, dx))
                        }
                    }
                    onReleased: (mouse) => {
                        if(!dragging) return
                        var from = root.dragIndex
                        var to   = root.targetIndex
                        dragging       = false
                        root.dragIndex = -1
                        root.dragDx    = 0
                        if(from !== to) root.moved(from, to)
                    }
                    onClicked: (mouse) => {
                        if(index !== editIndex && editIndex !== -1){
                            rep_Tabs.itemAt(editIndex).tab.commitEdit()
                        }
                    }
                    onDoubleClicked: (mouse) => {
                        if(mouse.button === Qt.LeftButton) root.startRename(tabItem.index)
                    }
                }

                Tab {
                    id: tab
                    anchors.fill: parent
                    label:       tabItem.modelData.title !== undefined ? tabItem.modelData.title : ""
                    icon:        tabItem.modelData.icon  !== undefined ? tabItem.modelData.icon  : root.defaultIcon
                    dirty:       tabItem.modelData.dirty === true
                    active:      root.currentIndex === tabItem.index
                    hovered:     root.hoveredIndex === tabItem.index || tabItem.dragged
                    closable:    root.closable
                    confirmClose: root.confirmClose
                    closeMessage: root.closeMessage

                    onCloseTab: root.closeRequested(tabItem.index)
                    onRenamed:  (text) => {
                        root.editIndex = -1;
                        root.renamed(tabItem.index, text)
                    }
                    onRenameCanceled: { root.editIndex = -1 }
                }
            }
        }

        // ===== "+" new tab =====
        Rectangle {
            visible: root.showAdd
            x: root.pad + root.count * root.tabWidth + 6
            anchors.verticalCenter: parent.verticalCenter
            width: 24; height: 24; radius: 12
            color: ma_Add.pressed ? Qt.lighter(Theme.bgChrome, 2.4) : ma_Add.containsMouse ? Qt.lighter(Theme.bgChrome, 1.8) : "transparent"

            IconImage {
                anchors.centerIn: parent
                width: 14; height: 14
                sourceSize.width: 14; sourceSize.height: 14
                source: "qrc:/icons/action/plus.svg"
                color:  ma_Add.containsMouse ? Theme.white : Theme.gray
            }

            MouseArea {
                id: ma_Add
                anchors.fill: parent
                hoverEnabled: true
                cursorShape:  Qt.PointingHandCursor
                onClicked:    root.addRequested()
            }

            ToolTip.visible: ma_Add.containsMouse
            ToolTip.delay:   600
            ToolTip.text:    root.addToolTip
        }
    }
}
