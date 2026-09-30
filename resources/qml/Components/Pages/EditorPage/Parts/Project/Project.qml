import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../Theme"
import "../../../../Tab"
import "Pages"

Item {
    id: root

    property var project: null

    readonly property var projectPages:  project ? project.pages : []
    readonly property int currentPage:   project ? project.pageActv : -1
    readonly property int totalPages:    projectPages.length + 1 //Pages + HomePage

    signal blockSelected(string uuid)

    ColumnLayout {
        anchors.fill: parent
        spacing:      0

        // ======== Pages Tabs =======
        Rectangle {
            Layout.fillWidth:       true
            Layout.preferredHeight: 34
            color: Theme.bgStrip

            RowLayout {
                anchors.fill:      parent
                anchors.topMargin: 5
                spacing: 0

                // ===== Title page (pinned) =====
                Item {
                    Layout.fillHeight:     true
                    Layout.preferredWidth: 80
                    Layout.leftMargin:     8

                    Tab {
                        anchors.fill: parent
                        label:     qsTr("Cover")
                        icon:      "qrc:/icons/document/cover.svg"
                        active:    root.currentPage === -1
                        hovered:   ma_TitleTab.containsMouse
                        closable:  false
                        separator: false
                    }

                    MouseArea {
                        id: ma_TitleTab
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape:  Qt.PointingHandCursor
                        onPressed:    if(root.project) root.project.pageActv = -1;
                    }
                }

                TabStrip {
                    id: strip_Pages
                    Layout.fillWidth:  true
                    Layout.fillHeight: true

                    model:        root.projectPages
                    currentIndex: root.currentPage
                    defaultIcon:  ""
                    closable:     root.projectPages.length > 1
                    renamable:    true
                    showAdd:      true
                    addToolTip:   qsTr("New page")
                    maxTabWidth:  190
                    leadingSeparator: root.currentPage !== -1
                    confirmClose: true
                    closeMessage: "Do tou want delete this page ?"

                    onActivated:      (index) => root.project.pageActv = index
                    onCloseRequested: (index) => root.project.cmdRemovePage(index)
                    onMoved:          (from, to) => root.project.cmdMovePage(from, to)
                    onAddRequested:              root.project.cmdAddPage(-1, qsTr("Page"))
                    onRenamed:        (index, text) => root.project.cmdRenamePage(index, text)
                    onContextRequested: (index, sx, sy) => {
                        root.menuIndex = index
                        var p = root.mapFromItem(null, sx, sy)
                        menu_Page.popup(root, p.x, p.y)
                    }
                }
            }
        }

        // ============= Pages ===============
        Item {
            Layout.fillWidth:  true
            Layout.fillHeight: true

            // ======= Cover Page =========
            Page {
                id: coverPageView
                anchors.fill: parent
                visible:   root.currentPage === -1 && root.project !== null
                project:   root.project
                coverPage: true
                pageNumber:1
                pageTotal: root.totalPages
                pageTitle: qsTr("Cover")
                pageDate:  root.project ? root.project.info.mDate : ""
            }

            Repeater {
                id: workPageView
                model: root.projectPages

                delegate: Page {
                    required property int index
                    required property var modelData

                    anchors.fill: parent

                    visible:      root.currentPage === index
                    project:      root.project
                    pageModel:    modelData.page
                    pageNumber:   modelData.number
                    pageTotal:    root.totalPages
                    pageTitle:    modelData.title
                    pageDate:     modelData.mDate
                }
            }
        }
    }
}
