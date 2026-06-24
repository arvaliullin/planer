import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    property string title
    property string message
    property string actionText
    property var onAction: null

    ColumnLayout {
        anchors.centerIn: parent
        spacing: PlanerTheme.spacingMedium
        width: Math.min(parent.width - 48, 360)

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: root.title
            font.pixelSize: PlanerTheme.fontHeading
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: root.message
            font.pixelSize: PlanerTheme.fontBody
            color: palette.mid
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            visible: root.actionText.length > 0
            text: root.actionText
            highlighted: true
            onClicked: {
                if (root.onAction) {
                    root.onAction()
                }
            }
        }
    }
}
