import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Dialog {
    id: root

    title: qsTr("Проверка сервера")
    modal: true
    anchors.centerIn: parent
    width: Math.min(parent.width - 48, 480)

    standardButtons: Dialog.Close

    onRejected: app.closeHealthDialog()
    onClosed: app.closeHealthDialog()

    readonly property var vm: health

    onOpened: {
        if (vm) {
            vm.checkHealth()
        }
    }

    ColumnLayout {
        spacing: PlanerTheme.spacingMedium
        width: parent.width

        RowLayout {
            Layout.fillWidth: true

            Label { text: qsTr("API URL:") }

            TextField {
                Layout.fillWidth: true
                readOnly: true
                text: vm ? vm.apiBaseUrl : ""
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
            color: palette.highlight
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
    }
}
