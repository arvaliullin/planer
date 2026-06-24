import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root

    required property string componentName
    required property bool isOk
    property string errorText: ""

    spacing: 8

    Label {
        Layout.preferredWidth: 120
        text: root.componentName
    }

    Label {
        Layout.fillWidth: true
        text: root.isOk ? qsTr("OK") : root.errorText
        color: root.isOk ? "#2e7d32" : "#c62828"
        wrapMode: Text.WordWrap
    }
}
