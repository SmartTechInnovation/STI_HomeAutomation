import QtQuick
import QtQuick.Controls.impl

/*
 *  Iconita monocroma, colorata din tema.
 *  SVG-urile din resources/icons sunt desenate cu fill="#000", deci un Image simplu
 *  ar fi invizibil pe fundalul inchis. IconImage (acelasi tip folosit intern de
 *  Button pentru icon.source) suporta colorare.
 */
IconImage {
    id: themeIcon

    property color iconColor: "#F7F7F7"

    color:    themeIcon.iconColor
    fillMode: Image.PreserveAspectFit

    sourceSize.width:  themeIcon.width
    sourceSize.height: themeIcon.height
}
