import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Page {
    id: root

    signal navigateToLogin()

    background: Rectangle { color: palette.window }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 400)
        implicitHeight: formColumn.implicitHeight + PlanerTheme.spacingLarge * 2
        radius: PlanerTheme.radius
        color: palette.base
        border.color: palette.mid

        ColumnLayout {
            id: formColumn
            anchors.fill: parent
            anchors.margins: PlanerTheme.spacingLarge
            spacing: PlanerTheme.spacingMedium

            Label {
                text: qsTr("Регистрация")
                font.pixelSize: PlanerTheme.fontTitle
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: displayNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Отображаемое имя")
            }

            TextField {
                id: emailField
                Layout.fillWidth: true
                placeholderText: qsTr("Email")
                inputMethodHints: Qt.ImhEmailCharactersOnly
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: qsTr("Пароль (мин. 8 символов)")
                echoMode: TextInput.Password
            }

            Label {
                Layout.fillWidth: true
                visible: session && session.errorMessage.length > 0
                text: session ? session.errorMessage : ""
                color: palette.highlight
                wrapMode: Text.WordWrap
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Зарегистрироваться")
                highlighted: true
                enabled: session && !session.loading
                onClicked: session.registerUser(emailField.text.trim(),
                                                 passwordField.text,
                                                 displayNameField.text.trim())
            }

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: session && session.loading
            }

            Button {
                Layout.fillWidth: true
                flat: true
                text: qsTr("Уже есть аккаунт? Войти")
                onClicked: root.navigateToLogin()
            }
        }
    }
}
