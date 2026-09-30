import QtQuick

import "../../../../../Theme"

// Title page (page 1) - generated from the project data, not editable on the canvas.
// Designed on an A3 sheet (4200 x 2970 units) and scaled to the project paper format.

Item {
    id: root
    property var info:     ({})
    property int pageTotal: 1
    property string pageTitle: "Cover"
    property string pageDate:  ""

    readonly property real baseW: 4200
    readonly property real baseH: 2970
    readonly property real scaleFactor:     Math.min(width / baseW, height / baseH)

    // Title block
    readonly property real mm:      10
    readonly property real tbW: 180 * mm
    readonly property real tbH: 36  * mm
    readonly property real tbX: baseW - tbW - 300
    readonly property real tbY: baseH - tbH - 200

    function field(value){ return value !== undefined && value !== null && String(value).length > 0 ? String(value) : "—" }
    function has(value)  { return value !== undefined && value !== null && String(value).length > 0 }

    readonly property var company: info && info.company ? info.company : ({})
    readonly property var client:  info && info.client  ? info.client  : ({})

    component KeyValue: Row {
        property string key:   ""
        property string value: ""
        spacing: 24
        Text { width: 520; text: parent.key;   color: Theme.inkMuted; font.pixelSize: 44 }
        Text { text: parent.value; color: Theme.ink; font.pixelSize: 44; font.bold: true }
    }

    component Label: Text {
        color: Theme.inkMuted
        font.pixelSize: 21
        font.capitalization: Font.AllUppercase
        font.letterSpacing: 1.5
    }
    component Value: Text {
        color: Theme.ink
        font.pixelSize: 34
        elide: Text.ElideRight
    }

    Item {
        id:     contentData
        width:  root.baseW
        height: root.baseH
        scale:  root.scaleFactor
        transformOrigin: Item.TopLeft
        x: (root.width  - root.baseW * root.scaleFactor) / 2
        y: (root.height - root.baseH * root.scaleFactor) / 2

        // ================= Header: company =================
        Item {
            id: logoBox
            x: 360
            y: 260
            width:  450
            height: 450

            Image {
                id: img_Logo
                anchors.fill: parent
                fillMode:     Image.PreserveAspectFit
                sourceSize.width: logoBox.width
                visible: status === Image.Ready
                source: root.company.logoUrl !== undefined ? root.company.logoUrl : ""
            }
            Rectangle {
                anchors.fill: parent
                visible: !img_Logo.visible
                radius: 60
                color: "transparent"
                border.width: 6
                border.color: Theme.ink
                Text {
                    anchors.centerIn: parent
                    text: qsTr("HA")
                    color: Theme.gray
                    font.pixelSize: 150
                    font.bold: true
                }
            }
        }
        // ================ Company Details =================
        Column {
            x: logoBox.x + logoBox.width + 110
            y: logoBox.y + 10
            width: 1900
            spacing: 14

            Text {
                width: parent.width
                text:  root.field(root.company.name)
                color: Theme.ink
                font.pixelSize: 110
                font.bold: true
                elide: Text.ElideRight
            }
            Item { width: 1; height: 20 }
            Text {
                width: parent.width
                color: Theme.ink
                elide: Text.ElideRight
                font.pixelSize: 52
                text:  root.field(root.company.address)
            }
            Text {
                width: parent.width
                color: Theme.ink
                elide: Text.ElideRight
                font.pixelSize: 52
                text:  [root.company.phone, root.company.email].filter(function(v){ return root.has(v) }).join("   ·   ") || "—"
            }
            Text {
                width: parent.width
                color: Theme.blue
                elide: Text.ElideRight
                font.pixelSize: 52
                text:  '<a href="' + root.field(root.company.web) + '">' + root.field(root.company.web) + '</a>'
                onLinkActivated: (link) => {
                    if(root.company.web) Qt.openUrlExternally(link)
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: (parent.hoveredLink && root.company.web) ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
        // ====== Modified date (top-right corner) ======
        Column {
            anchors.right: parent.right
            anchors.rightMargin: 360
            y: 280
            spacing: 10

            Text {
                anchors.right: parent.right
                text: qsTr("Last modified")
                color: Theme.inkMuted
                font.pixelSize: 40
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 4
            }
            Text {
                anchors.right: parent.right;
                text: root.field(root.info.mDate);
                color: Theme.ink;
                font.pixelSize: 64;
                font.bold: true
            }
            Item { width: 1; height: 20 }
            Text {
                anchors.right: parent.right;
                text: qsTr("Created");
                color: Theme.inkMuted;
                font.pixelSize: 36;
                font.capitalization: Font.AllUppercase;
                font.letterSpacing: 4
            }
            Text {
                anchors.right: parent.right;
                text: root.field(root.info.cDate);
                color: Theme.ink;
                font.pixelSize: 44
            }
        }

        // ====== Accent Lines ======
        Rectangle { x: 360; y: 820; width: parent.width - 720; height: 14; color: Theme.inkFaint }
        Rectangle { x: 360; y: 834; width: 520;                height: 14; color: Theme.green    }

        // ================= Project title =================
        Column {
            x: 360; y: 1020
            width: parent.width - 720
            spacing: 30

            Text {
                color: Theme.ink
                font.pixelSize: 40
                font.bold: true
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 6
                text: qsTr("Automation project · Configuration")
            }
            Text {
                width: parent.width
                text:  root.field(root.info.title)
                color: Theme.ink
                font.pixelSize: 230
                font.bold: true
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text:  [root.info.street, root.info.town, root.info.postCode, root.info.country].filter(function(v){ return root.has(v) }).join(", ") || "—"
                color: Theme.inkMuted
                font.pixelSize: 64
                elide: Text.ElideRight
            }
        }
        // ================= Client card =================
        Rectangle {
            x: 360; y: 1820
            width: 1640; height: 560
            radius: 30
            color: "transparent"
            border.width: 5
            border.color: Theme.inkFaint

            Column {
                anchors.fill: parent
                anchors.margins: 60
                spacing: 18
                Text {
                    color: Theme.ink
                    font.pixelSize: 40
                    font.bold: true
                    font.capitalization: Font.AllUppercase
                    font.letterSpacing: 6
                    text: qsTr("Client")
                }
                Text {
                    width: parent.width
                    text: root.field(root.client.name)
                    color: Theme.ink
                    font.pixelSize: 90
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    width: parent.width
                    color: Theme.ink
                    elide: Text.ElideRight
                    font.pixelSize: 52
                    text:  root.field(root.client.address)
                }
                Text {
                    width: parent.width
                    color: Theme.ink
                    elide: Text.ElideRight
                    font.pixelSize: 52
                    text:  [root.client.phone, root.client.email].filter(function(v){ return root.has(v) }).join("   ·   ") || "—"
                }
            }
        }
        // ================= Project data =================
        Column {
            x: 2160; y: 1880
            spacing: 22
            Text {
                color: Theme.ink
                font.pixelSize: 40
                font.bold: true
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 6
                text: qsTr("Project Data")
            }
            Item { width: 1; height: 6 }
            KeyValue { key: qsTr("Config version"); value: root.field(root.info.configVers) }
            KeyValue { key: qsTr("Paper format");   value: root.field(root.info.paperFormat) }
            KeyValue { key: qsTr("Pages");          value: String(root.pageCount) }
            KeyValue { key: qsTr("Author");         value: root.field(root.info.author) }
        }
    }
    // ===== Title block =====
    Rectangle {
        id: tb
        x: root.tbX
        y: root.tbY
        width:  root.tbW
        height: root.tbH
        color: Theme.paper
        border.width: 5
        border.color: Theme.ink

        readonly property real rowH: height / 3
        readonly property real c1: 30 * root.mm     // logo
        readonly property real c2: 80 * root.mm     // company | project
        readonly property real c3: 140 * root.mm    // project | page

        // grid lines
        Rectangle { x: tb.c1; width: 3; height: parent.height; color: Theme.ink }
        Rectangle { x: tb.c2; width: 3; height: parent.height; color: Theme.ink }
        Rectangle { x: tb.c3; width: 3; height: parent.height; color: Theme.ink }
        Repeater {
            model: 2
            delegate: Rectangle {
                required property int index
                x: tb.c1;
                y: (index + 1) * tb.rowH
                width: tb.width - tb.c1;
                height: 3
                color: Theme.ink
            }
        }

        // ---- Logo ----
        Item {
            x: 0; width: tb.c1; height: tb.height
            Image {
                id: img_Logo_Bottom
                anchors.centerIn: parent
                width: parent.width - 60; height: parent.height - 60
                fillMode: Image.PreserveAspectFit
                source:   root.info && root.info.company ? root.info.company.logoUrl : ""
                sourceSize.width: 512
                visible: status === Image.Ready
            }
            Text {
                anchors.centerIn: parent
                visible: !img_Logo.visible
                text:  root.info && root.info.company && root.info.company.name ? root.info.company.name.split(" ").map(function(w){ return w.charAt(0) }).join("").substring(0, 3).toUpperCase() : "LOGO"
                color: Theme.blue
                font.pixelSize: 70
                font.bold: true
            }
        }

        // ---- Column 2: company ----
        Column {
            x: tb.c1 + 20; y: 12; width: tb.c2 - tb.c1 - 40
            spacing: 2
            Label { text: qsTr("Company") }
            Value { width: parent.width; font.bold: true; text: root.field(root.info.company ? root.info.company.name : "") }
        }
        Column {
            x: tb.c1 + 20; y: tb.rowH + 12; width: tb.c2 - tb.c1 - 40
            spacing: 2
            Label { text: qsTr("Contact") }
            Value { width: parent.width; font.pixelSize: 28; text: root.field(root.info.company ? [root.info.company.phone, root.info.company.email].filter(function(v){ return v }).join("  ·  ") : "") }
        }
        Column {
            x: tb.c1 + 20; y: 2 * tb.rowH + 12; width: tb.c2 - tb.c1 - 40
            spacing: 2
            Label { text: qsTr("Web") }
            Value { width: parent.width; font.pixelSize: 28; text: root.field(root.info.company ? root.info.company.web : "") }
        }

        // ---- Column 3: project ----
        Column {
            x: tb.c2 + 20; y: 12; width: tb.c3 - tb.c2 - 40
            spacing: 2
            Label { text: qsTr("Project") }
            Value { width: parent.width; font.bold: true; text: root.field(root.info.title) }
        }
        Column {
            x: tb.c2 + 20; y: tb.rowH + 12; width: tb.c3 - tb.c2 - 40
            spacing: 2
            Label { text: qsTr("Client") }
            Value { width: parent.width; text: root.field(root.info.client ? root.info.client.name : "") }
        }
        Column {
            x: tb.c2 + 20; y: 2 * tb.rowH + 12; width: tb.c3 - tb.c2 - 40
            spacing: 2
            Label { text: qsTr("Page title") }
            Value { width: parent.width; text: root.field(root.pageTitle) }
        }

        // ---- Column 4: page ----
        Column {
            x: tb.c3 + 20; y: 12; width: tb.width - tb.c3 - 40
            spacing: 2
            Label { text: qsTr("Page") }
            Value { width: parent.width; font.bold: true; text:  "1 / " + root.pageTotal }
        }
        Column {
            x: tb.c3 + 20; y: tb.rowH + 12; width: tb.width - tb.c3 - 40
            spacing: 2
            Label { text: qsTr("Modified") }
            Value { width: parent.width; font.pixelSize: 28; text: root.field(root.pageDate) }
        }
        Column {
            x: tb.c3 + 20; y: 2 * tb.rowH + 12; width: tb.width - tb.c3 - 40
            spacing: 2
            Label { text: qsTr("Format · Version") }
            Value { width: parent.width; font.pixelSize: 28; text: root.field(root.info.paperFormat) + "  ·  v" + root.field(root.info.configVers) }
        }
    }
}
