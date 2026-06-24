import QtQuick
import QtQuick.Controls
import planer

Rectangle {
    id: root

    property string status
    property string label

    radius: PlanerTheme.radiusSmall
    color: PlanerTheme.statusColor(status, palette)
    implicitWidth: statusLabel.implicitWidth + 12
    implicitHeight: statusLabel.implicitHeight + 6

    Label {
        id: statusLabel
        anchors.centerIn: parent
        text: label.length > 0 ? label : PlanerTheme.statusLabel(status)
        font.pixelSize: PlanerTheme.fontCaption
        color: palette.buttonText
    }
}
