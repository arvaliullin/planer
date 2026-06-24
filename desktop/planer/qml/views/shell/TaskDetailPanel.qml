import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

ScrollView {
    id: root

    property bool hasSelection: tasks && tasks.selectedTaskId.length > 0

    readonly property var currentTask: taskDetail && taskDetail.task ? taskDetail.task : ({})

    function field(name) {
        return PlanerTheme.str(currentTask[name])
    }

    visible: hasSelection
    implicitWidth: hasSelection ? PlanerTheme.detailPanelWidth : 0
    clip: true

    background: Rectangle {
        color: palette.alternateBase
        border.color: palette.mid
        border.width: 1
    }

    ColumnLayout {
        width: PlanerTheme.detailPanelWidth - 16
        spacing: PlanerTheme.spacingSmall

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: PlanerTheme.spacingMedium
            Layout.topMargin: PlanerTheme.spacingMedium

            TaskKeyLabel {
                taskKey: field("key")
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                display: AbstractButton.TextOnly
                text: "×"
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Закрыть (Esc)")
                onClicked: tasks.clearSelection()
            }
        }

        SectionHeader {
            Layout.leftMargin: PlanerTheme.spacingMedium
            caption: qsTr("ОСНОВНОЕ")
        }

        TextField {
            id: titleField
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium
            placeholderText: qsTr("Заголовок")
            font.pixelSize: PlanerTheme.fontHeading
            font.bold: true
            onEditingFinished: if (hasSelection) taskDetail.saveField("title", text)

            Connections {
                target: taskDetail
                function onTaskChanged() { titleField.text = root.field("title") }
            }
            Component.onCompleted: text = root.field("title")
        }

        TextArea {
            id: descriptionField
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium
            Layout.preferredHeight: 80
            placeholderText: qsTr("Описание")
            wrapMode: TextArea.Wrap
            onEditingFinished: if (hasSelection) taskDetail.saveField("description", text)

            Connections {
                target: taskDetail
                function onTaskChanged() { descriptionField.text = root.field("description") }
            }
            Component.onCompleted: text = root.field("description")
        }

        SectionHeader {
            Layout.leftMargin: PlanerTheme.spacingMedium
            caption: qsTr("ПАРАМЕТРЫ")
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium
            columns: 2
            columnSpacing: PlanerTheme.spacingSmall
            rowSpacing: PlanerTheme.spacingSmall

            Label { text: qsTr("Статус"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            ComboBox {
                id: statusCombo
                Layout.fillWidth: true
                model: [
                    { value: "backlog", text: qsTr("Backlog") },
                    { value: "todo", text: qsTr("Todo") },
                    { value: "in_progress", text: qsTr("In Progress") },
                    { value: "review", text: qsTr("Review") },
                    { value: "done", text: qsTr("Done") },
                    { value: "cancelled", text: qsTr("Cancelled") }
                ]
                textRole: "text"
                valueRole: "value"
                onActivated: if (hasSelection) taskDetail.saveField("status", currentValue)

                function syncFromTask() {
                    var s = root.field("status")
                    currentIndex = Math.max(0, indexOfValue(s))
                }
                Connections {
                    target: taskDetail
                    function onTaskChanged() { statusCombo.syncFromTask() }
                }
                Component.onCompleted: syncFromTask()
            }

            Label { text: qsTr("Приоритет"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            ComboBox {
                id: priorityCombo
                Layout.fillWidth: true
                model: [
                    { value: "low", text: qsTr("Low") },
                    { value: "normal", text: qsTr("Normal") },
                    { value: "high", text: qsTr("High") },
                    { value: "critical", text: qsTr("Critical") }
                ]
                textRole: "text"
                valueRole: "value"
                onActivated: if (hasSelection) taskDetail.saveField("priority", currentValue)

                function syncFromTask() {
                    var p = root.field("priority")
                    currentIndex = Math.max(0, indexOfValue(p || "normal"))
                }
                Connections {
                    target: taskDetail
                    function onTaskChanged() { priorityCombo.syncFromTask() }
                }
                Component.onCompleted: syncFromTask()
            }

            Label { text: qsTr("Тип"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            ComboBox {
                id: typeCombo
                Layout.fillWidth: true
                model: [
                    { value: "task", text: qsTr("Task") },
                    { value: "bug", text: qsTr("Bug") }
                ]
                textRole: "text"
                valueRole: "value"
                onActivated: if (hasSelection) taskDetail.saveField("type", currentValue)

                function syncFromTask() {
                    var t = root.field("type")
                    currentIndex = t === "bug" ? 1 : 0
                }
                Connections {
                    target: taskDetail
                    function onTaskChanged() { typeCombo.syncFromTask() }
                }
                Component.onCompleted: syncFromTask()
            }

            Label { text: qsTr("Исполнитель"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            Label {
                Layout.fillWidth: true
                text: field("assigneeName").length > 0 ? field("assigneeName") : "—"
                elide: Text.ElideRight
            }
        }

        SectionHeader {
            Layout.leftMargin: PlanerTheme.spacingMedium
            caption: qsTr("СРОКИ")
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium
            columns: 2
            columnSpacing: PlanerTheme.spacingSmall
            rowSpacing: PlanerTheme.spacingSmall

            Label { text: qsTr("Начало"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            TextField {
                id: startDateField
                Layout.fillWidth: true
                placeholderText: "YYYY-MM-DD"
                onEditingFinished: if (hasSelection) taskDetail.saveField("startDate", text)
                Connections {
                    target: taskDetail
                    function onTaskChanged() { startDateField.text = root.field("startDate") }
                }
                Component.onCompleted: text = root.field("startDate")
            }

            Label { text: qsTr("Окончание"); font.pixelSize: PlanerTheme.fontCaption; color: palette.mid }
            TextField {
                id: dueDateField
                Layout.fillWidth: true
                placeholderText: "YYYY-MM-DD"
                onEditingFinished: if (hasSelection) taskDetail.saveField("dueDate", text)
                Connections {
                    target: taskDetail
                    function onTaskChanged() { dueDateField.text = root.field("dueDate") }
                }
                Component.onCompleted: text = root.field("dueDate")
            }
        }

        SectionHeader {
            Layout.leftMargin: PlanerTheme.spacingMedium
            caption: qsTr("ЗАВИСИМОСТИ")
        }

        Repeater {
            model: taskDetail ? taskDetail.dependencies : []

            delegate: RowLayout {
                required property var modelData
                Layout.fillWidth: true
                Layout.leftMargin: PlanerTheme.spacingMedium
                Layout.rightMargin: PlanerTheme.spacingMedium

                Label {
                    Layout.fillWidth: true
                    text: PlanerTheme.str(modelData.blockerKey) + " → " + PlanerTheme.str(modelData.blockedKey)
                    font.pixelSize: PlanerTheme.fontCaption
                }

                ToolButton {
                    text: "×"
                    onClicked: taskDetail.removeDependency(modelData.id)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium

            TextField {
                id: blockedTaskField
                Layout.fillWidth: true
                placeholderText: qsTr("UUID заблокированной задачи")
            }

            Button {
                text: qsTr("Добавить")
                flat: true
                enabled: blockedTaskField.text.length > 0
                onClicked: {
                    taskDetail.addDependency(blockedTaskField.text.trim())
                    blockedTaskField.text = ""
                }
            }
        }

        SectionHeader {
            Layout.leftMargin: PlanerTheme.spacingMedium
            caption: qsTr("ВЛОЖЕНИЯ")
        }

        Repeater {
            model: taskDetail ? taskDetail.attachments : []

            delegate: RowLayout {
                required property var modelData
                Layout.fillWidth: true
                Layout.leftMargin: PlanerTheme.spacingMedium
                Layout.rightMargin: PlanerTheme.spacingMedium

                Label {
                    Layout.fillWidth: true
                    text: PlanerTheme.str(modelData.fileName)
                    elide: Text.ElideMiddle
                    font.pixelSize: PlanerTheme.fontBody
                }

                ToolButton {
                    text: "×"
                    onClicked: taskDetail.deleteAttachment(modelData.id)
                }
            }
        }

        Button {
            Layout.leftMargin: PlanerTheme.spacingMedium
            Layout.rightMargin: PlanerTheme.spacingMedium
            text: qsTr("Загрузить файл (mock)")
            flat: true
            onClicked: taskDetail.uploadAttachment("document.pdf", "application/pdf", 4096)
        }

        Item { Layout.fillHeight: true; Layout.minimumHeight: PlanerTheme.spacingMedium }
    }
}
