import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

ApplicationWindow {
    id: root

    width: 960
    height: 640
    visible: true
    title: qsTr("Planer")

    header: ToolBar {
        Label {
            anchors.centerIn: parent
            text: qsTr("Planer")
            font.pixelSize: 18
        }
    }

    StackView {
        id: stackView

        anchors.fill: parent
        initialItem: settingsPageComponent
    }

    Component {
        id: settingsPageComponent

        SettingsPage {}
    }
}
