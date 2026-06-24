import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Dialog {
    id: root

    title: qsTr("Новая задача")
    modal: true
    anchors.centerIn: parent
    width: Math.min(parent.width - 48, 420)

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        if (tasks && titleInput.text.trim().length > 0) {
            tasks.createTask(titleInput.text.trim(), descriptionInput.text)
        }
    }

    onRejected: app.closeCreateTaskDialog()
    onClosed: app.closeCreateTaskDialog()

    ColumnLayout {
        spacing: PlanerTheme.spacingMedium
        width: parent.width

        TextField {
            id: titleInput
            Layout.fillWidth: true
            placeholderText: qsTr("Заголовок")
        }

        TextField {
            id: descriptionInput
            Layout.fillWidth: true
            placeholderText: qsTr("Описание")
        }
    }
}
