import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts

import STI.BlockManager

import "../../../../Theme"

Item {
    id: root

    property int currentTab: 0 // Blocks, 1 Monitor, 2 Diagnostics, 3 Context
    property var project: null
    property var context: (project && project.context && project.context.Title) ? project.context : null

    readonly property var list_Categories:         BlockManager.categories
    readonly property var list_ShortcutCategories: BlockManager.shortcutCategories

    onContextChanged: currentTab = context ? 3 : (currentTab === 3 ? 0 : currentTab)

    signal blockPicked(string uuid)

    component TopTab: Rectangle {
        id: topTab
        property string title: ""
        property bool   active: false

        signal clicked()

        implicitWidth: txt_TopTab.implicitWidth + 24
        height: parent ? parent.height : 22
        radius: 5
        color: active ? Theme.bgHover : Theme.bgCanvas

        Text {
            id: txt_TopTab
            anchors.centerIn: parent
            text:      topTab.title
            color:     topTab.active ? Theme.white : Theme.gray
            font.bold: topTab.active
            font.pixelSize: 11
        }

        Rectangle {
            anchors.bottom: parent.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
            width: parent.width - 12;
            height: 2;
            radius: 1;
            color: Theme.green
            visible: topTab.active
        }

        MouseArea {
            id: ma_TopTab
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: topTab.clicked()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        // ======= Tab Bar =======
        Row {
            Layout.fillWidth: true
            Layout.preferredHeight: 22
            spacing: 4

            TopTab {
                title: qsTr("Blocks");
                active: root.currentTab === 0;
                onClicked: root.currentTab = 0
            }
            TopTab {
                title: qsTr("Monitor");
                active: root.currentTab === 1;
                onClicked: root.currentTab = 1
            }
            TopTab {
                title: qsTr("Diagnostics");
                active: root.currentTab === 2;
                onClicked: root.currentTab = 2
            }

            Rectangle {
                visible: topTab_Context.visible
                anchors.verticalCenter: parent.verticalCenter
                width: 2
                height: parent.height - 4
                color: Theme.gray
                radius: 0.5
            }

            TopTab {
                id: topTab_Context
                visible: root.context !== null
                title:   root.context ? (root.context.Title ?? "") : ""
                active: root.currentTab === 3
                onClicked: root.currentTab = 3
            }
        }

        StackLayout {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            currentIndex: root.currentTab

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

            // ========= Monitor =========
            Item {
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: 8
                    text:  qsTr("Monitor: live values from the controller will be shown here (needs the connection to the controller).")
                    color: Theme.gray
                    font.pixelSize: 12
                }
            }

            // ========= Diagnostics =========
            Item {
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: 8
                    text:  qsTr("Diagnostics: bus statistics, errors and logs from the controller will be shown here.")
                    color: Theme.gray
                    font.pixelSize: 12
                }
            }

            // ========= Context Menus =======
            RowLayout {
                spacing: 8

                // Main menu with all functions
                ToolBtn {
                    id: btn_MainMenu

                    Layout.fillHeight: true
                    Layout.preferredWidth: height

                    visible:      root.context ? (root.context.MenuName !== undefined ? true : false) : false
                    firstRowText: root.context ? (root.context.MenuName !== undefined ? root.context.MenuName : "") : ""
                    iconSrc:      root.context ? (root.context.MenuName !== undefined ? root.context.Icon : "") : ""

                    property bool wasOpen: false

                    onPressed: wasOpen = popup_ShortcutContext.opened

                    onClicked: {
                        if (wasOpen){
                            popup_ShortcutContext.close()
                        }else{
                            var pos = btn_MainMenu.mapToItem(root, 0, btn_MainMenu.height)
                            popup_ShortcutContext.x = pos.x
                            popup_ShortcutContext.y = pos.y + 4
                            popup_ShortcutContext.open()
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
                    id: list_ContextShortcuts
                    Layout.fillWidth:  true
                    Layout.fillHeight: true

                    orientation: ListView.Horizontal
                    clip:        true
                    spacing:     8
                    model:       root.context ? root.context.Items : ({})
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.horizontal: ScrollBar {
                        policy: list_Shortcuts.contentWidth > list_Shortcuts.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }

                    delegate: ToolBtn {
                        id: btn_ShortcutContext
                        height: parent.height
                        width:  parent.height

                        firstRowText: modelData.Title
                        iconSrc:      modelData.Icon

                        onPressed: console.log(modelData.Uuid)
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
        id: popup_ShortcutContext


        padding: 0
        margins: 0
        modal:   false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Item {}

        onClosed: {
            context     = null
        }

        contentItem: ShortcutsMenu {
            id: submenuShortcutsContext

            title: root.context ? root.context.Title : ""
            items: root.context ? root.context.Items : []
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
            category     = null
            categoryName = ""
        }

        contentItem: BlocksMenu {
            id: submenuShortcutsBlocks
            categoryName: popup_ShortcutCategory.category ? popup_ShortcutCategory.category.name   : ""
            blocks:       popup_ShortcutCategory.category ? popup_ShortcutCategory.category.blocks : []
        }
    }
}
