import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Page {
    id: root

    title: qsTr("Настройки")

    readonly property var vm: healthViewModel

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
                text: vm ? vm.apiBaseUrl : ""
            }
        }

        Button {
            text: qsTr("Проверить сервер")
            enabled: vm && !vm.loading
            onClicked: {
                if (vm) {
                    vm.checkHealth()
                }
            }
        }

        BusyIndicator {
            visible: vm && vm.loading
            running: vm && vm.loading
        }

        Label {
            Layout.fillWidth: true
            visible: vm && vm.errorMessage.length > 0
            text: vm ? vm.errorMessage : ""
            color: "#c62828"
            wrapMode: Text.WordWrap
        }

        ColumnLayout {
            Layout.fillWidth: true
            visible: vm && vm.hasResult
            spacing: 8

            Label {
                text: qsTr("Статус компонентов")
                font.bold: true
            }

            ComponentStatusRow {
                Layout.fillWidth: true
                visible: vm && vm.postgresPresent
                componentName: qsTr("PostgreSQL")
                isOk: vm ? vm.postgresOk : false
                errorText: vm ? vm.postgresError : ""
            }

            ComponentStatusRow {
                Layout.fillWidth: true
                visible: vm && vm.minioPresent
                componentName: qsTr("MinIO")
                isOk: vm ? vm.minioOk : false
                errorText: vm ? vm.minioError : ""
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
