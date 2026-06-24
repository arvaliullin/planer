import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Dialog {
    id: root

    title: qsTr("Новый проект")
    modal: true
    anchors.centerIn: parent
    width: Math.min(parent.width - 48, 420)

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        if (projects) {
            projects.createProject(keyInput.text.toUpperCase(),
                                   nameInput.text,
                                   descriptionInput.text)
        }
    }

    onRejected: app.closeCreateProjectDialog()
    onClosed: app.closeCreateProjectDialog()

    ColumnLayout {
        spacing: PlanerTheme.spacingMedium
        width: parent.width

        TextField {
            id: keyInput
            Layout.fillWidth: true
            placeholderText: qsTr("Ключ (PLAN)")
            maximumLength: 10
        }

        TextField {
            id: nameInput
            Layout.fillWidth: true
            placeholderText: qsTr("Название")
        }

        TextField {
            id: descriptionInput
            Layout.fillWidth: true
            placeholderText: qsTr("Описание")
        }
    }
}
