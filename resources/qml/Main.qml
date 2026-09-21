import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import "Components/Theme"
import "Components/Tab"
import "Components/Pages/HomePage"
import "Components/Pages/EditorPage"

ApplicationWindow {
    id: mainApp
    width: 1200
    height: 720
    minimumWidth: 720
    minimumHeight: 500
    visible: true
    title: qsTr("Home Automation Config")

    property int prop_rw_CurrentTab: 0

    readonly property bool prop_ro_Maximized: mainApp.visibility === Window.Maximized

    // ========= Window Content ==========
    ColumnLayout { //Vertical layout
        anchors.fill: parent
        spacing: 0

        // ========= Title Bar ==========
        Rectangle {
            Layout.fillWidth:  true
            Layout.preferredHeight: 25
            color: Theme.bgChrome

            // ======== Top Menu ========
            RowLayout {
                anchors.fill: parent
                spacing: 0

                // ====== Burger Menu ======
                Button {
                    id: btn_BurgerMenu
                    Layout.fillHeight: true
                    Layout.preferredWidth: 40
                    padding: 2
                    icon.source: "qrc:/icons/navigation/menu-burger.svg"

                    hoverEnabled: true

                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    background: Rectangle {
                        color: btn_BurgerMenu.down  ? Qt.darker(Theme.bgChrome, 1.35)  : btn_BurgerMenu.hovered ? Qt.darker(Theme.bgChrome, 1.18): "transparent"
                    }
                }

                // ====== Separtor ======
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                    color: Theme.gray
                }

                // ===== Shortcuts ======

                Repeater {
                    model: [
                        { icon_src: "qrc:/icons/page/page-text.svg"  , id_name: "btn_NewProject"  },
                        { icon_src: "qrc:/icons/file/folder-open.svg", id_name: "btn_OpenProject" },
                        { icon_src: "qrc:/icons/action/save.svg"     , id_name: "btn_SaveProject" },
                        { icon_src: "qrc:/icons/action/undo.svg"     , id_name: "btn_Undo"        },
                        { icon_src: "qrc:/icons/action/redo.svg"     , id_name: "btn_Redo"        },
                    ]
                    Button {
                        required property string icon_src
                        required property var    id_name
                        id: id_name
                        Layout.fillHeight: true
                        Layout.preferredWidth: 25
                        padding: 2
                        icon.source: icon_src

                        hoverEnabled: true

                        HoverHandler { cursorShape: Qt.PointingHandCursor }

                        background: Rectangle {
                            color: id_name.down  ? Qt.darker(Theme.bgChrome, 1.35)  : id_name.hovered ? Qt.darker(Theme.bgChrome, 1.18): "transparent"
                        }
                    }
                }

                Item { Layout.preferredWidth: 100 }

                Tab { // ==== Home Tab ====
                    active: true
                    closable: false
                    icon_src: "qrc:/icons/navigation/home.svg"
                }

                // ====== Projects Tabs =====



                // ====== Fill space =====
                Item { Layout.fillWidth: true }
            }
        }

        // ====== Workspace Pages =====
        Item {
            id: workspaceArea
            Layout.fillWidth:  true
            Layout.fillHeight: true

            HomePage {
                anchors.fill: parent
                visible:      false
            }

            EditorPage {
                anchors.fill: parent
                visible:      true
            }
        }

    }

}
