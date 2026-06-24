import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Rectangle {
    id: root

    property var task
    property bool dragging: false

    radius: PlanerTheme.radius
    color: dragging ? palette.midlight : palette.base
    border.color: dragging ? palette.link : palette.mid
    border.width: 1

    implicitWidth: 220
    implicitHeight: content.implicitHeight + 16

    ColumnLayout {
        id: content
        anchors.fill: parent
        anchors.margins: PlanerTheme.spacingSmall
        spacing: 6

        TaskKeyLabel {
            taskKey: task ? PlanerTheme.str(task.key) : ""
        }

        Label {
            Layout.fillWidth: true
            text: task ? PlanerTheme.str(task.title) : ""
            font.pixelSize: PlanerTheme.fontBody
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            maximumLineCount: 2
        }

        RowLayout {
            Layout.fillWidth: true

            PriorityBadge {
                priority: task ? PlanerTheme.str(task.priority) : "normal"
                label: task ? PlanerTheme.str(task.priorityLabel) : ""
            }

            Item { Layout.fillWidth: true }

            Label {
                visible: task && PlanerTheme.str(task.assigneeName).length > 0
                text: task ? PlanerTheme.str(task.assigneeName) : ""
                font.pixelSize: PlanerTheme.fontCaption
                color: palette.mid
                elide: Text.ElideRight
                Layout.maximumWidth: 90
            }
        }
    }
}
