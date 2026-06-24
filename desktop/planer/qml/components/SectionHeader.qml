import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Label {
    id: root

    property string caption

    text: caption
    font.pixelSize: PlanerTheme.fontCaption
    font.bold: true
    font.letterSpacing: 0.5
    color: palette.mid
    topPadding: PlanerTheme.spacingSmall
    bottomPadding: 4
}
