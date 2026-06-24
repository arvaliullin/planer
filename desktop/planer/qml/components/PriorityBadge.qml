import QtQuick
import QtQuick.Controls
import planer

Rectangle {
    id: root

    property string priority
    property string label

    radius: PlanerTheme.radiusSmall
    color: priority === "critical" ? palette.highlight :
           priority === "high" ? palette.midlight :
           priority === "low" ? palette.alternateBase : palette.button
    implicitWidth: priorityLabel.implicitWidth + 12
    implicitHeight: priorityLabel.implicitHeight + 6

    Label {
        id: priorityLabel
        anchors.centerIn: parent
        text: label.length > 0 ? label : PlanerTheme.priorityLabel(priority)
        font.pixelSize: PlanerTheme.fontCaption
    }
}
