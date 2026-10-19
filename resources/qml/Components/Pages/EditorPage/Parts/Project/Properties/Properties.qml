import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Layouts
import QtQuick.Dialogs

import "../../../../../Theme"
import "../Pages/Parts"            // CheckBoxCustom

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
    /* Int: text box with up / down arrows */
    component DarkSpin: SpinBox {
        id: spin
        editable:       true
        from:           -2147483647
        to:             2147483647
        font.pixelSize: 12
        implicitHeight: 26
        leftPadding:    4
        rightPadding:   20

        contentItem: TextInput {
            text:              spin.displayText
            font:              spin.font
            color:             Theme.white
            selectionColor:    Theme.blue
            selectedTextColor: Theme.white
            selectByMouse:     true
            verticalAlignment: Qt.AlignVCenter
            readOnly:          !spin.editable
            validator:         spin.validator
            inputMethodHints:  Qt.ImhFormattedNumbersOnly
        }
        up.indicator: Rectangle {
            x: spin.width - width - 1
            y: 1
            width: 18; height: spin.height / 2 - 1
            color: spin.up.pressed ? Theme.bgElevated : (spin.up.hovered ? Theme.bgHover : "transparent")
            Text { anchors.centerIn: parent; text: "\u25B4"; color: Theme.gray; font.pixelSize: 10 }
        }
        down.indicator: Rectangle {
            x: spin.width - width - 1
            y: spin.height / 2
            width: 18; height: spin.height / 2 - 1
            color: spin.down.pressed ? Theme.bgElevated : (spin.down.hovered ? Theme.bgHover : "transparent")
            Text { anchors.centerIn: parent; text: "\u25BE"; color: Theme.gray; font.pixelSize: 10 }
        }
        background: Rectangle {
            radius: 3
            color: Theme.bgCanvas
            border.color: spin.activeFocus ? Theme.green : Theme.bgHover
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

                        function changeAttr(tagName, newValue){
                            if(root.project && root.selectionData.instance)
                                root.project.cmdSetProperty(root.selectionData.instance.iUuid, tagName, newValue)
                        }

                        Text {
                            Layout.fillWidth: true
                            Layout.topMargin: 8
                            visible: groupProperties.title !== ""
                            text:  groupProperties.title
                            color: Theme.green
                            font.pixelSize: 10
                            font.bold: true
                            font.capitalization: Font.AllUppercase
                            font.letterSpacing: 1
                        }

                        // ====== Properties List ====
                        Repeater {
                            model: groupProperties.items

                            delegate: RowLayout {
                                id: prop
                                Layout.fillWidth: true
                                spacing: 6
                                visible: !hidden

                                /* all from modelData: nodes without a template only have tagName/title/value */
                                required property var modelData
                                readonly property string tagName:  modelData.tagName  ?? ""
                                readonly property string title:    modelData.title    ?? tagName
                                readonly property string value:    modelData.value    ?? ""
                                readonly property string type:     modelData.type     ?? "String"
                                readonly property string defValue: modelData.defValue ?? ""
                                readonly property string unit:     modelData.unit     ?? ""
                                readonly property string docs:     modelData.docs     ?? ""
                                readonly property bool   readOnly: modelData.readOnly === true
                                readonly property bool   hidden:   modelData.hidden   === true
                                readonly property var    variants: modelData.variants ?? []

                                function commit(newValue){ groupProperties.changeAttr(prop.tagName, newValue) }

                                Text {
                                    Layout.preferredWidth: Math.round(contentLayout.width * 0.42)
                                    Layout.alignment: Qt.AlignVCenter
                                    color:          Theme.white
                                    elide:          Text.ElideRight
                                    text:           prop.title
                                    font.pixelSize: 12
                                }

                                Loader {
                                    Layout.fillWidth: true
                                    sourceComponent: {
                                        if(prop.readOnly)
                                            return cmp_ReadOnly
                                        switch(prop.type){
                                        case "Bool":     return cmp_Bool
                                        case "Int":      return cmp_Int
                                        case "Enum":     return cmp_Enum
                                        case "Ref":      return cmp_Enum     // variants = nodes of a tag (IUuid / Title)
                                        case "Color":    return cmp_Color
                                        case "Password": return cmp_Password
                                        default:         return cmp_String   // String, Float, Date, Custom
                                        }
                                    }
                                }

                                Text {
                                    visible: prop.unit !== ""
                                    text:    prop.unit
                                    color:   Theme.gray
                                    font.pixelSize: 11
                                }

                                Component {
                                    id: cmp_ReadOnly
                                    DarkField {
                                        text:     prop.value
                                        readOnly: true
                                        color:    Theme.gray
                                    }
                                }

                                Component {
                                    id: cmp_Bool
                                    Item {
                                        implicitHeight: 26
                                        CheckBoxCustom {
                                            anchors.verticalCenter: parent.verticalCenter
                                            colorText: Theme.white            // hover border on the dark panel
                                            checked:   prop.value === "true"
                                            onToggled: prop.commit(checked)
                                        }
                                    }
                                }

                                Component {
                                    id: cmp_Int
                                    DarkSpin {
                                        value: parseInt(prop.value !== "" ? prop.value : prop.defValue) || 0
                                        onValueModified: {
                                            if(value.toString() !== prop.value)
                                                prop.commit(value.toString())
                                        }
                                    }
                                }

                                Component {
                                    id: cmp_String
                                    DarkField {
                                        text: prop.value
                                        placeholderText: prop.type === "Date" ? "YYYY-MM-DD" : prop.defValue
                                        validator: prop.type === "Int"   ? intValidator
                                                 : prop.type === "Float" ? floatValidator : null
                                        onEditingFinished: {
                                            if(prop.type === "Float")
                                                text = text.replace(",", ".")
                                            if(prop.value !== text)
                                                prop.commit(text)
                                        }
                                        IntValidator { id: intValidator }
                                        RegularExpressionValidator {
                                            id: floatValidator
                                            regularExpression: /^-?\d*([.,]\d*)?$/
                                        }
                                    }
                                }

                                Component {
                                    id: cmp_Enum
                                    DarkCombo {
                                        model:        prop.variants
                                        textRole:     "text"
                                        valueRole:    "data"
                                        currentIndex: prop.variants.findIndex(v => v.data === prop.value)
                                        onActivated:  (i) => { if(prop.variants[i].data !== prop.value) prop.commit(prop.variants[i].data) }
                                    }
                                }

                                Component {
                                    id: cmp_Color
                                    Flow {
                                        id: colorFlow
                                        spacing: 6

                                        /* pale default palette; the 9th dot opens the color dialog */
                                        readonly property var  swatches: ["#8dff70", "#ff0054", "#ff7d30", "#ffe647",
                                                                         "#a470ff"]
                                        readonly property string current: prop.value.toUpperCase()
                                        readonly property bool   isCustom: current !== "" && swatches.indexOf(current) < 0

                                        Repeater {
                                            model: colorFlow.swatches
                                            delegate: Rectangle {
                                                required property string modelData
                                                readonly property bool selected: colorFlow.current === modelData

                                                width: 20; height: 20
                                                radius: width / 2
                                                color: modelData
                                                border.width: selected ? 2 : (ma_Dot.containsMouse ? 1 : 0)
                                                border.color: Theme.white

                                                MouseArea {
                                                    id: ma_Dot
                                                    anchors.fill: parent
                                                    hoverEnabled: true
                                                    cursorShape:  Qt.PointingHandCursor
                                                    onClicked: if(!parent.selected) prop.commit(parent.modelData)
                                                }
                                            }
                                        }

                                        /* custom: "+" when empty, filled with the custom color when one is set */
                                        Rectangle {
                                            width: 20; height: 20
                                            radius: width / 2
                                            color: colorFlow.isCustom ? prop.value : "transparent"
                                            border.width: colorFlow.isCustom ? 2 : 1
                                            border.color: colorFlow.isCustom ? Theme.white
                                                                             : (ma_Custom.containsMouse ? Theme.white : Theme.gray)
                                            Text {
                                                anchors.centerIn: parent
                                                visible: !colorFlow.isCustom
                                                text:  "+"
                                                color: ma_Custom.containsMouse ? Theme.white : Theme.gray
                                                font.pixelSize: 14
                                                font.bold: true
                                            }
                                            MouseArea {
                                                id: ma_Custom
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                cursorShape:  Qt.PointingHandCursor
                                                onClicked: dlg_Color.openFor(root.selectionData.instance.iUuid, prop.tagName, prop.value)
                                            }
                                        }
                                    }
                                }

                                Component {
                                    id: cmp_Password
                                    DarkField {
                                        echoMode: TextInput.Password
                                        placeholderText: prop.value !== "" ? "••••••••" : qsTr("not set")
                                        onAccepted: { if(text !== "") prop.commit(text); text = "" }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* One dialog for the whole panel (not inside the delegates): the delegates are destroyed when the
       selection changes, and a dialog living inside one of them crashed on Set / Cancel.
       The target (node + attribute) is captured when the dialog opens. */
    ColorDialog {
        id: dlg_Color

        property string targetIUuid: ""
        property string targetTag:   ""

        modality: Qt.ApplicationModal

        function openFor(iUuid, tagName, current){
            targetIUuid   = iUuid
            targetTag     = tagName
            selectedColor = current !== "" ? current : "white"
            open()
        }

        onAccepted: {
            if(root.project && targetIUuid !== "" && targetTag !== "")
                root.project.cmdSetProperty(targetIUuid, targetTag, selectedColor.toString().toUpperCase())
            targetIUuid = ""
            targetTag   = ""
        }
        onRejected: {
            targetIUuid = ""
            targetTag   = ""
        }
    }

    /* another element selected -> the picker belongs to the old one, close it */
    Connections {
        target: root.project
        ignoreUnknownSignals: true
        function onSelectedIUuidChanged(){
            if(dlg_Color.visible)
                dlg_Color.reject()
        }
    }
}
