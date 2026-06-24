import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Page {
    id: root

    title: qsTr("Настройки")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: qsTr("Подключение к серверу")
            font.pixelSize: 20
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: qsTr("API URL:")
            }

            TextField {
                Layout.fillWidth: true
                readOnly: true
                text: healthViewModel.apiBaseUrl
            }
        }

        Button {
            text: qsTr("Проверить сервер")
            enabled: !healthViewModel.loading
            onClicked: healthViewModel.checkHealth()
        }

        BusyIndicator {
            visible: healthViewModel.loading
            running: healthViewModel.loading
        }

        Label {
            Layout.fillWidth: true
            visible: healthViewModel.errorMessage.length > 0
            text: healthViewModel.errorMessage
            color: "#c62828"
            wrapMode: Text.WordWrap
        }

        ColumnLayout {
            Layout.fillWidth: true
            visible: healthViewModel.hasResult
            spacing: 8

            Label {
                text: qsTr("Статус компонентов")
                font.bold: true
            }

            ComponentStatusRow {
                Layout.fillWidth: true
                visible: healthViewModel.postgresPresent
                componentName: qsTr("PostgreSQL")
                isOk: healthViewModel.postgresOk
                errorText: healthViewModel.postgresError
            }

            ComponentStatusRow {
                Layout.fillWidth: true
                visible: healthViewModel.minioPresent
                componentName: qsTr("MinIO")
                isOk: healthViewModel.minioOk
                errorText: healthViewModel.minioError
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
