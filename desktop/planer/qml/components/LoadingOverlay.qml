import QtQuick
import QtQuick.Controls
import planer

Rectangle {
    id: root

    property alias label: labelItem.text
    property bool running: visible

    color: Qt.rgba(palette.window.r, palette.window.g, palette.window.b, 0.82)
    visible: running

    Column {
        anchors.centerIn: parent
        spacing: PlanerTheme.spacingMedium

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            running: root.running
        }

        Label {
            id: labelItem
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: PlanerTheme.fontBody
            color: palette.mid
        }
    }
}
