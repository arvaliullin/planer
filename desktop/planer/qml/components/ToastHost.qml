import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    anchors.fill: parent
    z: 1000

    Repeater {
        model: app ? app.toasts : []

        delegate: Rectangle {
            required property int index
            required property string modelData

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20 + index * 60
            width: Math.min(root.width - 32, 480)
            radius: PlanerTheme.radius
            color: palette.window
            border.color: palette.mid
            border.width: 1

            layer.enabled: true
            layer.effect: null

            RowLayout {
                anchors.fill: parent
                anchors.margins: PlanerTheme.spacingMedium

                Label {
                    Layout.fillWidth: true
                    text: modelData
                    wrapMode: Text.WordWrap
                    font.pixelSize: PlanerTheme.fontBody
                }

                ToolButton {
                    text: "×"
                    onClicked: app.dismissToast(index)
                }
            }
        }
    }
}
