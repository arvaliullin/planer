import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingSmall
            currentIndex: app ? app.activeTab : 0
            onCurrentIndexChanged: if (app) app.activeTab = currentIndex

            TabButton { text: qsTr("Список") }
            TabButton { text: qsTr("Kanban") }
            TabButton { text: qsTr("Gantt") }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: palette.mid
            opacity: 0.35
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            TaskListView {}
            KanbanView {}
            GanttView {}
        }
    }
}
