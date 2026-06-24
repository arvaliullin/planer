import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Page {
    id: root

    signal navigateToRegister()

    background: Rectangle {
        color: palette.window
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 400)
        implicitHeight: authColumn.implicitHeight + PlanerTheme.spacingLarge * 2
        radius: PlanerTheme.radius
        color: palette.base
        border.color: palette.mid

        ColumnLayout {
            id: authColumn
            anchors.fill: parent
            anchors.margins: PlanerTheme.spacingLarge
            spacing: PlanerTheme.spacingMedium

            Label {
                text: qsTr("Planer")
                font.pixelSize: PlanerTheme.fontTitle
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: qsTr("Вход в систему")
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: PlanerTheme.fontBody
                color: palette.mid
            }

            TextField {
                id: emailField
                Layout.fillWidth: true
                placeholderText: qsTr("Email")
                inputMethodHints: Qt.ImhEmailCharactersOnly
                onAccepted: passwordField.forceActiveFocus()
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: qsTr("Пароль")
                echoMode: TextInput.Password
                onAccepted: loginButton.clicked()
            }

            Label {
                Layout.fillWidth: true
                visible: session && session.errorMessage.length > 0
                text: session ? session.errorMessage : ""
                color: palette.highlight
                wrapMode: Text.WordWrap
                font.pixelSize: PlanerTheme.fontBody
            }

            Button {
                id: loginButton
                Layout.fillWidth: true
                text: qsTr("Войти")
                highlighted: true
                enabled: session && !session.loading
                       && emailField.text.trim().length > 0
                       && passwordField.text.length > 0
                onClicked: session.login(emailField.text.trim(), passwordField.text)
            }

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: session && session.loading
            }

            Button {
                Layout.fillWidth: true
                flat: true
                text: qsTr("Создать аккаунт")
                onClicked: root.navigateToRegister()
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: palette.mid
                opacity: 0.4
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Демо: demo@planer.local / secret123")
                font.pixelSize: PlanerTheme.fontCaption
                color: palette.mid
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
