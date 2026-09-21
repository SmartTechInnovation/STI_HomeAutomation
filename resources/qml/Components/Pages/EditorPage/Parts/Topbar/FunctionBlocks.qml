import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import STI.Blocks

import "../../../../Theme"

/*
 *  Topbar - Function Blocks (stil Loxone)
 *   - buton patrat mare cu iconita  -> submeniu cu categorii -> la mouse over: blocurile categoriei
 *   - langa el, shortcut-uri (categoriile cu Visible="true") -> direct blocurile categoriei
 */
Item {
    id: root

    property int bigSize:      88
    property int shortcutSize: 64

    signal blockRequested(string category, string type, string title)

    // Cache local: fiecare citire a proprietatii din C++ reconstruieste lista
    readonly property var allCategories:      BlockManager.categories
    readonly property var shortcutCategories: BlockManager.shortcutCategories

    implicitHeight: bigSize + 12

    // ===== Iconite implicite pe categorie (cand Icon="" in Descriptor.xml) =====
    readonly property var categoryIcons: ({
        "Analog":      "qrc:/icons/signals/wave-sine.svg",
        "Mathematics": "qrc:/icons/signals/wave-triangle.svg",
        "Comparisons": "qrc:/icons/action/alternate.svg",
        "Logic":       "qrc:/icons/signals/wave-square.svg",
        "Controllers": "qrc:/icons/navigation/sliders-horizontal.svg",
        "Central":     "qrc:/icons/hardware/cpu.svg",
        "Access":      "qrc:/icons/misc/password.svg",
        "Alarm Clock": "qrc:/icons/misc/clock.svg",
        "Climate":     "qrc:/icons/misc/globe.svg",
        "Lighting":    "qrc:/icons/action/focus.svg",
        "Shading":     "qrc:/icons/navigation/ui-panel-top.svg",
        "Multimedia":  "qrc:/icons/hardware/audio-interface.svg",
        "Energy":      "qrc:/icons/hardware/battery-100.svg",
        "Counters":    "qrc:/icons/misc/bar-code.svg",
        "General":     "qrc:/icons/misc/blocks.svg",
        "Monitoring":  "qrc:/icons/software/system-monitor.svg",
        "Times":       "qrc:/icons/misc/task-soon.svg",
        "Wellness":    "qrc:/icons/misc/user.svg",
        "Window":      "qrc:/icons/navigation/ui-panel-bottom.svg"
    })

    function iconOf(category) {
        if (!category) return "qrc:/icons/misc/blocks.svg"
        if (category.icon !== undefined && category.icon.length > 0) return category.icon
        if (root.categoryIcons[category.name] !== undefined)         return root.categoryIcons[category.name]
        return "qrc:/icons/misc/blocks.svg"
    }

    function pickBlock(category, type, title) {
        categoriesPopup.close()
        shortcutPopup.close()
        root.blockRequested(category, type, title)
    }

    // ================== Topbar ==================
    RowLayout {
        anchors.fill:    parent
        anchors.margins: 6
        spacing: 8

        // ===== Buton mare "Function Blocks" =====
        Rectangle {
            id: btn_FunctionBlocks
            Layout.preferredWidth:  root.bigSize
            Layout.preferredHeight: root.bigSize
            Layout.alignment:       Qt.AlignVCenter

            radius: 6
            color:  categoriesPopup.opened ? Theme.blue
                                           : (ma_FunctionBlocks.containsMouse ? Theme.bgHover : Theme.bgPanel)
            border.color: categoriesPopup.opened ? Theme.blue : Theme.gray
            border.width: 1

            Column {
                anchors.centerIn: parent
                spacing: 4

                ThemeIcon {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width:     40
                    height:    40
                    source:    "qrc:/icons/misc/blocks.svg"
                    iconColor: Theme.white
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:  qsTr("Function")
                    color: Theme.white
                    font.pixelSize: 11
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:  qsTr("Blocks")
                    color: Theme.white
                    font.pixelSize: 11
                    font.bold: true
                }
            }

            MouseArea {
                id: ma_FunctionBlocks
                anchors.fill: parent
                hoverEnabled: true
                cursorShape:  Qt.PointingHandCursor

                /* popup-ul se inchide singur la press-ul din afara lui,
                   deci retinem starea de dinainte de press pentru toggle corect */
                property bool wasOpen: false

                onPressed: wasOpen = categoriesPopup.opened

                onClicked: {
                    shortcutPopup.close()
                    if (wasOpen) {
                        categoriesPopup.close()
                    } else {
                        categoriesPopup.hoveredCategory     = null
                        categoriesPopup.hoveredCategoryName = ""
                        var pos = btn_FunctionBlocks.mapToItem(root, 0, btn_FunctionBlocks.height)
                        categoriesPopup.x = pos.x
                        categoriesPopup.y = pos.y + 4
                        categoriesPopup.open()
                    }
                }
            }
        }

        // ===== Separator =====
        Rectangle {
            Layout.preferredWidth:  1
            Layout.preferredHeight: root.bigSize
            Layout.alignment:       Qt.AlignVCenter
            color: Theme.gray
        }

        // ===== Shortcut-uri (categorii Visible="true") =====
        Item {
            Layout.fillWidth:       true
            Layout.preferredHeight: root.shortcutSize + 14
            Layout.alignment:       Qt.AlignVCenter

            ListView {
                id: shortcutsView
                anchors.fill:   parent
                orientation:    ListView.Horizontal
                clip:           true
                spacing:        6
                model:          root.shortcutCategories
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.horizontal: ScrollBar {
                    policy: shortcutsView.contentWidth > shortcutsView.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                delegate: Rectangle {
                    id: shortcutButton
                    width:  root.shortcutSize
                    height: root.shortcutSize
                    radius: 6

                    property bool isOpen: shortcutPopup.opened && shortcutPopup.categoryName === modelData.name

                    color: isOpen ? Theme.blue : (ma_Shortcut.containsMouse ? Theme.bgHover : Theme.bgPanel)
                    border.color: isOpen ? Theme.blue : Theme.gray
                    border.width: 1

                    Column {
                        anchors.centerIn: parent
                        spacing: 3

                        ThemeIcon {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width:     26
                            height:    26
                            source:    root.iconOf(modelData)
                            iconColor: Theme.white
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: root.shortcutSize - 6
                            text:  modelData.name
                            color: Theme.white
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }
                    }

                    MouseArea {
                        id: ma_Shortcut
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape:  Qt.PointingHandCursor

                        property bool wasOpen: false

                        onPressed: wasOpen = shortcutButton.isOpen

                        onClicked: {
                            categoriesPopup.close()
                            if (wasOpen) {
                                shortcutPopup.close()
                            } else {
                                var pos = shortcutButton.mapToItem(root, 0, shortcutButton.height)
                                shortcutPopup.category     = modelData
                                shortcutPopup.categoryName = modelData.name
                                shortcutPopup.x = pos.x
                                shortcutPopup.y = pos.y + 4
                                shortcutPopup.open()
                            }
                        }
                    }
                }
            }
        }
    }

    // ================== Popup: Categorii + submeniu blocuri ==================
    Popup {
        id: categoriesPopup

        property var    hoveredCategory:     null
        property string hoveredCategoryName: ""
        property real   hoverY: 0

        readonly property int rowHeight: 30

        padding: 0
        margins: 0
        modal:   false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Item { }

        implicitWidth:  categoriesPanel.width + submenuPanel.width
        implicitHeight: Math.max(categoriesPanel.height,
                                 submenuPanel.visible ? submenuPanel.y + submenuPanel.height : 0)

        onClosed: {
            hoveredCategory     = null
            hoveredCategoryName = ""
        }

        contentItem: Item {

            // ===== Lista de categorii =====
            Rectangle {
                id: categoriesPanel
                x: 0
                y: 0
                width:  230
                height: Math.min(440, Math.max(categoriesPopup.rowHeight,
                                               root.allCategories.length * categoriesPopup.rowHeight)) + 4

                color:        Theme.bgElevated
                border.color: Theme.gray
                border.width: 1
                radius:       4

                ListView {
                    id: categoriesList
                    anchors.fill:    parent
                    anchors.margins: 2
                    clip:  true
                    model: root.allCategories
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy: categoriesList.contentHeight > categoriesList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }

                    delegate: Rectangle {
                        id: categoryRow
                        width:  categoriesList.width
                        height: categoriesPopup.rowHeight
                        color:  (categoriesPopup.hoveredCategoryName === modelData.name || ma_Category.containsMouse)
                                ? Theme.bgHover : "transparent"

                        Row {
                            anchors.left:           parent.left
                            anchors.leftMargin:     8
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 8

                            ThemeIcon {
                                anchors.verticalCenter: parent.verticalCenter
                                width:     18
                                height:    18
                                source:    root.iconOf(modelData)
                                iconColor: Theme.white
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text:  modelData.name
                                color: Theme.white
                                font.pixelSize: 13
                            }
                        }

                        Text {
                            anchors.right:          parent.right
                            anchors.rightMargin:    18
                            anchors.verticalCenter: parent.verticalCenter
                            text:  modelData.count
                            color: Theme.gray
                            font.pixelSize: 11
                        }

                        Text {
                            anchors.right:          parent.right
                            anchors.rightMargin:    6
                            anchors.verticalCenter: parent.verticalCenter
                            text:  "›"
                            color: Theme.gray
                            font.pixelSize: 16
                        }

                        MouseArea {
                            id: ma_Category
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape:  Qt.PointingHandCursor

                            onEntered: {
                                categoriesPopup.hoveredCategory     = modelData
                                categoriesPopup.hoveredCategoryName = modelData.name
                                categoriesPopup.hoverY              = categoryRow.y - categoriesList.contentY
                            }
                            onClicked: {
                                categoriesPopup.hoveredCategory     = modelData
                                categoriesPopup.hoveredCategoryName = modelData.name
                                categoriesPopup.hoverY              = categoryRow.y - categoriesList.contentY
                            }
                        }
                    }
                }
            }

            // ===== Submeniu: blocurile categoriei peste care e mouse-ul =====
            BlocksPanel {
                id: submenuPanel
                x: categoriesPanel.width + 2
                y: Math.max(0, Math.min(categoriesPopup.hoverY,
                                        categoriesPanel.height - height))

                visible:      categoriesPopup.hoveredCategory !== null
                categoryName: categoriesPopup.hoveredCategory ? categoriesPopup.hoveredCategory.name   : ""
                blocks:       categoriesPopup.hoveredCategory ? categoriesPopup.hoveredCategory.blocks : []

                onBlockPicked: (category, type, title) => root.pickBlock(category, type, title)
            }
        }
    }

    // ================== Popup: blocurile unui shortcut ==================
    Popup {
        id: shortcutPopup

        property var    category:     null
        property string categoryName: ""

        padding: 0
        margins: 0
        modal:   false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Item { }

        onClosed: {
            category     = null
            categoryName = ""
        }

        contentItem: BlocksPanel {
            id: shortcutBlocks
            categoryName: shortcutPopup.category ? shortcutPopup.category.name   : ""
            blocks:       shortcutPopup.category ? shortcutPopup.category.blocks : []

            onBlockPicked: (category, type, title) => root.pickBlock(category, type, title)
        }
    }
}
