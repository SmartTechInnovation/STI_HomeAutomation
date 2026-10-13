import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../../../../Theme"
import "../../../../Tab"
import "Pages"

Item {
    id: root

    property var project: null

    readonly property var pageList:   project ? project.pageList : []
    readonly property int pageActv:   project ? project.pageActv : -1
    readonly property int totalPages: pageList.length + 1 //Pages + HomePage

    property int menuIndex: -1

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

                // ===== Cover page (pinned) =====
                Item {
                    Layout.fillHeight:     true
                    Layout.preferredWidth: 80
                    Layout.leftMargin:     8

                    Tab {
                        anchors.fill: parent
                        label:     qsTr("Cover")
                        icon:      "qrc:/icons/document/cover.svg"
                        active:    root.pageActv === -1
                        hovered:   ma_CoverTab.containsMouse
                        closable:  false
                        separator: false
                    }

                    MouseArea {
                        id: ma_CoverTab
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

                    model:        root.pageList
                    currentIndex: root.pageActv
                    defaultIcon:  ""
                    closable:     root.pageList.length > 1
                    renamable:    true
                    showAdd:      true
                    addToolTip:   qsTr("New page")
                    maxTabWidth:  190
                    leadingSeparator: root.pageActv !== -1
                    confirmClose: true
                    closeMessage: "Do tou want delete this page ?"

                    onActivated:      (index)    => root.project.pageActv = index
                    onCloseRequested: (index)    => root.project.cmdRemovePage(index)
                    onMoved:          (from, to) => root.project.cmdMovePage(from, to)
                    onAddRequested:                 root.project.cmdAddPage(-1, qsTr("Page"))
                    onRenamed:        (index, text) => root.project.cmdRenamePage(index, text)
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
                visible:    root.pageActv === -1 && root.project !== null
                project:    root.project
                coverPage:  true
                pageNumber: 1
                pageTotal:  root.totalPages
                pageTitle:  qsTr("Cover")
                pageMDate:  root.project ? root.project.info.mDate : ""
            }

            Repeater {
                id: rep_Pages
                model: root.pageList

                delegate: Page {
                    required property int index
                    required property var modelData

                    anchors.fill: parent
                    visible:      root.pageActv === index
                    project:      root.project
                    pageRef:      modelData.pageRef
                    pageTitle:    modelData.title
                    pageMDate:    modelData.mDate
                    pageNumber:   modelData.number
                    pageTotal:    root.totalPages

                    onVisibleChanged: if(visible) forceActiveFocus();
                }
            }
        }
    }
}