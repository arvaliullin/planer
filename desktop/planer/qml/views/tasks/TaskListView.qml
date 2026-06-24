import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    readonly property var activeProjectId: projects ? PlanerTheme.str(projects.activeProject.id) : ""

    ListView {
        id: taskList
        anchors.fill: parent
        clip: true
        spacing: 1
        model: tasks ? tasks.tasks : []

        header: Rectangle {
            width: taskList.width
            height: 40
            color: palette.alternateBase
            border.color: palette.mid
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: PlanerTheme.spacingMedium
                anchors.rightMargin: PlanerTheme.spacingMedium
                spacing: PlanerTheme.spacingSmall

                Label { Layout.preferredWidth: 72; text: qsTr("Ключ"); font.bold: true; font.pixelSize: PlanerTheme.fontCaption }
                Label { Layout.fillWidth: true; text: qsTr("Заголовок"); font.bold: true; font.pixelSize: PlanerTheme.fontCaption }
                Label { Layout.preferredWidth: 96; text: qsTr("Статус"); font.bold: true; font.pixelSize: PlanerTheme.fontCaption }
                Label { Layout.preferredWidth: 96; text: qsTr("Исполнитель"); font.bold: true; font.pixelSize: PlanerTheme.fontCaption }
                Label { Layout.preferredWidth: 88; text: qsTr("Срок"); font.bold: true; font.pixelSize: PlanerTheme.fontCaption }
            }
        }

        delegate: ItemDelegate {
            id: rowDelegate
            required property var modelData
            required property int index

            width: taskList.width
            height: 44

            readonly property bool isSelected: tasks && tasks.selectedTaskId === modelData.id

            background: Rectangle {
                color: rowDelegate.isSelected ? palette.highlight
                    : (rowDelegate.index % 2 === 0 ? palette.base : palette.alternateBase)
                opacity: rowDelegate.isSelected ? 0.18 : 1
            }

            contentItem: RowLayout {
                anchors.leftMargin: PlanerTheme.spacingMedium
                anchors.rightMargin: PlanerTheme.spacingMedium
                spacing: PlanerTheme.spacingSmall

                TaskKeyLabel {
                    Layout.preferredWidth: 72
                    taskKey: modelData.key
                }

                Label {
                    Layout.fillWidth: true
                    text: modelData.title
                    elide: Text.ElideRight
                    font.pixelSize: PlanerTheme.fontBody
                }

                StatusBadge {
                    Layout.preferredWidth: 96
                    status: modelData.status
                    label: modelData.statusLabel
                }

                Label {
                    Layout.preferredWidth: 96
                    text: modelData.assigneeName || "—"
                    elide: Text.ElideRight
                    font.pixelSize: PlanerTheme.fontCaption
                    color: palette.mid
                }

                Label {
                    Layout.preferredWidth: 88
                    text: modelData.dueDate || "—"
                    font.pixelSize: PlanerTheme.fontCaption
                    color: PlanerTheme.isOverdue(modelData.dueDate, modelData.status)
                           ? palette.highlight : palette.text
                }
            }

            onClicked: tasks.selectTask(modelData.id)
            onDoubleClicked: tasks.selectTask(modelData.id)
        }

        EmptyState {
            anchors.centerIn: parent
            visible: taskList.count === 0 && !(tasks && tasks.loading)
            title: qsTr("Нет задач")
            message: qsTr("Создайте первую задачу в этом проекте")
            actionText: qsTr("Создать задачу")
            onAction: app.openCreateTaskDialog()
        }
    }

    LoadingOverlay {
        anchors.fill: parent
        visible: tasks && tasks.loading
        label: qsTr("Загрузка задач...")
    }

    Component.onCompleted: {
        if (tasks && activeProjectId.length > 0) {
            tasks.loadTasks()
        }
    }
}
