import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts
import QtQuick.Dialogs

import "../../../../../Theme"

Item {
    id: root

    property var project: null

    readonly property var selectionData: project ? project.selection : ({})

    /* ===== Styled controls ===== */
    component DarkField: TextField {
        id: field
        color:          Theme.white
        selectionColor: Theme.blue
        selectedTextColor: Theme.white
        placeholderTextColor: Theme.gray
        selectByMouse: true
        font.pixelSize: 12
        padding: 4
        background: Rectangle {
            radius: 3
            color: field.readOnly ? "transparent" : Theme.bgCanvas
            border.color: field.activeFocus ? Theme.green : Theme.bgHover
        }
    }
    component DarkCombo: ComboBox {
        font.pixelSize: 12
        palette.button:     Theme.bgCanvas
        palette.buttonText: Theme.white
        palette.window:     Theme.bgElevated
        palette.windowText: Theme.white
        palette.text:       Theme.white
        palette.base:       Theme.bgElevated
        palette.highlight:  Theme.bgHover
        palette.highlightedText: Theme.white
        palette.light:      Theme.bgHover
        palette.dark:       Theme.gray
        palette.mid:        Theme.bgHover
    }

    ColumnLayout {
        anchors.fill: parent

        // ====== Header =======
        Rectangle {
            id: header
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            color:  Theme.bgStrip
            Text {
                x: 12
                anchors.verticalCenter: parent.verticalCenter
                text:  qsTr("Properties")
                color: Theme.gray
                font.pixelSize: 10
                font.bold:      true
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 1
            }
        }

        // ====== Place holder =====
        Text{
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: root.selectionData.instance ? false : true
            text:  qsTr("Select an element in the tree or on a page")
            color: Theme.gray
            font.pixelSize: 12
            width: parent.width - 24
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment:   Text.AlignVCenter
        }

        Flickable {
            id: flick
            Layout.fillWidth:  true
            Layout.fillHeight: true
            visible: root.selectionData.instance ? true : false

            clip:           true
            contentHeight:  contentLayout.implicitHeight + 20
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}

            // Content Layout
            ColumnLayout {
                id: contentLayout
                x: 10
                y: 10
                width: flick.width - 20
                spacing: 6

                // ===== Element Header =====
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    IconImage {
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        sourceSize.width: 28;
                        sourceSize.height: 28
                        source:  root.selectionData.template ? root.selectionData.template.icon : ""
                        color:   Theme.iconColor(source, Theme.white)
                        visible: source.toString() !== ""
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Text {
                            Layout.fillWidth: true
                            text:  root.selectionData.instance ? root.selectionData.instance.title : ""
                            color: Theme.white
                            font.pixelSize: 15
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            text:  root.selectionData.template ? root.selectionData.template.title : ""
                            color: Theme.gray
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                    }
                }
                Text {
                    Layout.fillWidth: true
                    visible: text !== ""
                    text:  root.selectionData.template ? root.selectionData.template.description : ""
                    color: Theme.gray
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }
                // ==== End Of Header ====

                // ==== Group Properties ====
                Repeater {
                    model: root.selectionData.properties

                    delegate: ColumnLayout {
                        id: groupProperties
                        Layout.fillWidth: true
                        spacing:          4

                        required property int    index
                        required property string title
                        required property var    items

                        function changeAttr(newValue){
                            if(root.project){
                                root.project.cmdSetProperty(root.selectionData.instance.iUuid, tag, newValue);
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            Layout.topMargin: 8
                            visible: {
                                if(groupProperties.title === "")
                                    return false
                                return true
                            }
                            text:  groupProperties.title
                            color: Theme.green
                            font.pixelSize: 10
                            font.bold: true
                            font.capitalization: Font.AllUppercase
                            font.letterSpacing: 1
                        }

                        // ====== Properies List ====
                        Repeater {
                            model: groupProperties.items

                            delegate: RowLayout {
                                id: prop
                                Layout.fillWidth: true
                                spacing: 6

                                required property int    index
                                required property string tagName
                                required property string title
                                required property string value

                                property string type
                                property string defValue
                                property string unit
                                property string docs
                                property bool   readOnly
                                property bool   hidden
                                property var    variants


                                Text {
                                    Layout.preferredWidth: Math.round(contentLayout.width * 0.42)
                                    Layout.alignment: Qt.AlignVCenter

                                    color:            Theme.white
                                    elide:            Text.ElideRight
                                    text:             prop.title
                                    font.pixelSize:   12
                                }

                                Loader {
                                    Layout.fillWidth: true
                                    sourceComponent: {
                                       return cmp_String
                                    }
                                }

                                Component {
                                    id: cmp_String
                                    DarkField {
                                        text: prop.value
                                        validator: {
                                            if(prop.type == "Int"  ) return intValidator;
                                            if(prop.type == "Float") return floatValidator
                                            return null
                                        }
                                        onEditingFinished: {
                                            if(prop.type  === "Float") text = text.replace(",", ".")
                                            if(prop.value !== text)    prop.changeAttr(text)
                                        }
                                        IntValidator {
                                            id: intValidator
                                        }
                                        RegularExpressionValidator {
                                            id: floatValidator;
                                            regularExpression: /^-?\d*([.,]\d*)?$/
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
