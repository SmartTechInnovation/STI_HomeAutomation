import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import STI.BlockManager

import "../../../../Theme"

Item {
    id: root

    readonly property var list_Categories:         BlockManager.categories
    readonly property var list_ShortcutCategories: BlockManager.shortcutCategories

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        // ======= Tab Bar =======
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 20

            color: "black"
        }

        // ========= Tool Bar Menu =========
        RowLayout {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            spacing: 8

            // ====== All function blocks ======
            ToolBtn{
                id: btn_FunctionBlocks

                Layout.fillHeight: true
                Layout.preferredWidth: height

                color: Theme.white
                firstRowText:  "Function"
                secondRowText: "Block"

                property bool wasOpen: false

                onPressed: wasOpen = popup_FunctionBlocks.opened

                onClicked: {
                    if (wasOpen){
                        popup_FunctionBlocks.close()
                    }else{
                        popup_FunctionBlocks.hoveredCategory     = null
                        popup_FunctionBlocks.hoveredCategoryName = ""
                        var pos = btn_FunctionBlocks.mapToItem(root, 0, btn_FunctionBlocks.height)
                        popup_FunctionBlocks.x = pos.x
                        popup_FunctionBlocks.y = pos.y + 4
                        popup_FunctionBlocks.open()
                    }
                }
            }

            // ====== Separator ======
            Rectangle{
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: Theme.gray
            }

            // ====== Shortcuts ======
            ListView {
                id: list_Shortcuts
                Layout.fillWidth:  true
                Layout.fillHeight: true

                orientation: ListView.Horizontal
                clip:        true
                spacing:     8
                model:       root.list_ShortcutCategories
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.horizontal: ScrollBar {
                    policy: list_Shortcuts.contentWidth > list_Shortcuts.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                delegate: ToolBtn {
                    id: btn_ShortcutCategory
                    height: parent.height
                    width:  parent.height

                    firstRowText: modelData.name
                    iconSrc:      modelData.icon

                    property bool wasOpen: false

                    onPressed: wasOpen = popup_ShortcutCategory.opened

                    onClicked: {
                        if (wasOpen){
                            popup_ShortcutCategory.close()
                        }else{
                            var pos = btn_ShortcutCategory.mapToItem(root, 0, btn_ShortcutCategory.height)
                            popup_ShortcutCategory.category     = modelData
                            popup_ShortcutCategory.categoryName = modelData.name
                            popup_ShortcutCategory.x = pos.x
                            popup_ShortcutCategory.y = pos.y + 4
                            popup_ShortcutCategory.open()
                        }
                    }
                }
            }
        }
    }

    // ======== Popup menu: Categories + Submenus Blocks =========
    Popup {
        id: popup_FunctionBlocks

        property var    hoveredCategory:     null
        property string hoveredCategoryName: ""
        property real   hoverY:              0

        readonly property int rowHeight: 30

        padding: 0
        margins: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Item { }

        onClosed: {
            hoveredCategory     = null
            hoveredCategoryName = ""
        }

        contentItem: Item {
            // ======= Categories List ======
            Rectangle {
                id: categoriesPanel
                x: 0
                y: 0
                width: 230
                height: 440

                color:        Theme.bgElevated
                border.color: Theme.gray
                border.width: 1
                radius:       4

                ListView {
                    id: list_Categories

                    anchors.fill:    parent
                    anchors.margins: 2
                    clip:            true
                    model:           root.list_Categories
                    boundsBehavior:  Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy: list_Categories.contentHeight > list_Categories.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }

                    delegate: Rectangle {
                        id: categoryRow

                        width: categoriesPanel.width
                        height: popup_FunctionBlocks.rowHeight
                        color:  (popup_FunctionBlocks.hoveredCategoryName === modelData.name ) ? Theme.bgHover : "transparent"

                        RowLayout {
                            anchors.fill: parent
                            spacing: 8

                            IconImage {
                                Layout.fillHeight: true
                                Layout.preferredWidth: height
                                color:  btn_FunctionBlocks.color
                                fillMode: Image.PreserveAspectFit
                                source: modelData.icon
                                sourceSize.width:  width  - 6
                                sourceSize.height: height - 6
                            }

                            Text {
                                Layout.fillWidth:  true
                                Layout.fillHeight: true
                                text: modelData.name
                                color: btn_FunctionBlocks.color
                                font.pixelSize: 13
                                verticalAlignment: Text.AlignVCenter
                            }

                            Text {
                                Layout.preferredWidth: 10
                                Layout.fillHeight:     true
                                Layout.rightMargin: 16
                                text:  "›"
                                color: btn_FunctionBlocks.color
                                font.pixelSize: 13
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        MouseArea {
                            id: ma_Category
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape:  Qt.PointingHandCursor

                            onEntered: {
                                popup_FunctionBlocks.hoveredCategory     = modelData
                                popup_FunctionBlocks.hoveredCategoryName = modelData.name
                                popup_FunctionBlocks.hoverY              = categoryRow.y - list_Categories.contentY
                            }

                            onClicked: {
                                popup_FunctionBlocks.hoveredCategory     = modelData
                                popup_FunctionBlocks.hoveredCategoryName = modelData.name
                                popup_FunctionBlocks.hoverY              = categoryRow.y - list_Categories.contentY
                            }
                        }
                    }
                }
            }
            BlocksMenu {
                id: submenuPanel
                x:  categoriesPanel.width + 2
                y:  Math.max(0, Math.min(popup_FunctionBlocks.hoverY,
                                        categoriesPanel.height - height))

                visible:      popup_FunctionBlocks.hoveredCategory !== null
                categoryName: popup_FunctionBlocks.hoveredCategory ? popup_FunctionBlocks.hoveredCategory.name   : ""
                blocks:       popup_FunctionBlocks.hoveredCategory ? popup_FunctionBlocks.hoveredCategory.blocks : []

                //onBlockPicked: (category, type, title) => root.pickBlock(category, type, title)
            }
        }
    }

    Popup {
        id: popup_ShortcutCategory

        property var    category:     null
        property string categoryName: ""

        padding: 0
        margins: 0
        modal:   false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Item {}

        onClosed: {
            category: null
            categoryName: ""
        }

        contentItem: BlocksMenu {
            id: submenuShortcutsBlocks
            categoryName: popup_ShortcutCategory.category ? popup_ShortcutCategory.category.name   : ""
            blocks:       popup_ShortcutCategory.category ? popup_ShortcutCategory.category.blocks : []
        }
    }
}
