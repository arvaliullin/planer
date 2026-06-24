import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

ApplicationWindow {
    id: root

    width: 1280
    height: 800
    visible: true
    title: qsTr("Planer")

    menuBar: MenuBar {
        Menu {
            title: qsTr("Файл")

            Action {
                text: qsTr("Новая задача")
                shortcut: "Ctrl+N"
                onTriggered: app.openCreateTaskDialog()
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Выйти")
                onTriggered: session.logout()
            }
        }

        Menu {
            title: qsTr("Сервис")

            MenuItem {
                text: qsTr("Проверить сервер…")
                onTriggered: app.openHealthDialog()
            }
        }

        Menu {
            title: qsTr("Справка")

            Action {
                text: qsTr("Командная палитра")
                shortcut: "Ctrl+K"
                onTriggered: app.openCommandPalette()
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: session && session.authenticated ? mainShellComponent : loginComponent

        Connections {
            target: session
            function onAuthenticatedChanged() {
                if (session.authenticated) {
                    stackView.replace(mainShellComponent)
                    projects.loadProjects()
                } else {
                    stackView.replace(loginComponent)
                }
            }
            function onSessionExpired() {
                app.showToast(qsTr("Сессия истекла. Войдите снова."))
                stackView.replace(loginComponent)
            }
        }
    }

    Component {
        id: loginComponent
        LoginPage {
            onNavigateToRegister: stackView.push(registerComponent)
        }
    }

    Component {
        id: registerComponent
        RegisterPage {
            onNavigateToLogin: stackView.pop()
        }
    }

    Component {
        id: mainShellComponent
        MainShellLayout {}
    }

    ToastHost {}

    CreateProjectDialog {
        id: createProjectDialog
        parent: root.contentItem
    }

    CreateTaskDialog {
        id: createTaskDialog
        parent: root.contentItem
    }

    CommandPaletteStub {
        id: commandPalette
        parent: root.contentItem
    }

    HealthCheckDialog {
        id: healthDialog
        parent: root.contentItem
    }

    Connections {
        target: app
        function onCreateProjectDialogOpenChanged() {
            if (app.createProjectDialogOpen) createProjectDialog.open()
            else createProjectDialog.close()
        }
        function onCreateTaskDialogOpenChanged() {
            if (app.createTaskDialogOpen) createTaskDialog.open()
            else createTaskDialog.close()
        }
        function onCommandPaletteOpenChanged() {
            if (app.commandPaletteOpen) commandPalette.open()
            else commandPalette.close()
        }
        function onHealthDialogOpenChanged() {
            if (app.healthDialogOpen) healthDialog.open()
            else healthDialog.close()
        }
    }

    Shortcut {
        sequence: "Ctrl+N"
        onActivated: app.openCreateTaskDialog()
    }

    Shortcut {
        sequence: "Ctrl+K"
        onActivated: app.openCommandPalette()
    }

    Shortcut {
        sequence: "Escape"
        onActivated: tasks.clearSelection()
    }
}
