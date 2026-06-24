import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Rectangle {
    id: root

    width: PlanerTheme.sidebarWidth
    color: palette.alternateBase
    border.color: palette.mid
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: PlanerTheme.spacingMedium
        spacing: PlanerTheme.spacingSmall

        Label {
            text: qsTr("Проекты")
            font.bold: true
            font.pixelSize: PlanerTheme.fontHeading
        }

        TextField {
            Layout.fillWidth: true
            placeholderText: qsTr("Поиск...")
            text: projects ? projects.filter : ""
            onTextChanged: if (projects) projects.filter = text
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("+ Проект")
            highlighted: true
            onClicked: app.openCreateProjectDialog()
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: projectList
                anchors.fill: parent
                clip: true
                spacing: 4
                model: projects ? projects.filteredProjects : []

                delegate: ItemDelegate {
                    id: projectDelegate
                    required property var modelData
                    width: projectList.width
                    height: 56

                    readonly property bool isActive: projects
                        && PlanerTheme.str(projects.activeProject.id) === PlanerTheme.str(modelData.id)

                    background: Rectangle {
                        radius: PlanerTheme.radius
                        color: projectDelegate.isActive ? palette.highlight : palette.base
                        opacity: projectDelegate.isActive ? 0.15 : (projectDelegate.hovered ? 0.08 : 0)
                        border.color: projectDelegate.isActive ? palette.link : "transparent"
                        border.width: projectDelegate.isActive ? 2 : 0
                    }

                    contentItem: RowLayout {
                        spacing: PlanerTheme.spacingSmall

                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 28
                            radius: PlanerTheme.radiusSmall
                            color: palette.link
                            opacity: 0.85

                            Label {
                                anchors.centerIn: parent
                                text: modelData.key
                                font.bold: true
                                font.pixelSize: PlanerTheme.fontCaption
                                color: palette.buttonText
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: modelData.name
                                font.bold: projectDelegate.isActive
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Label {
                                text: modelData.role
                                font.pixelSize: PlanerTheme.fontCaption
                                color: palette.mid
                            }
                        }
                    }

                    onClicked: projects.selectProject(modelData.id)
                }

                EmptyState {
                    anchors.centerIn: parent
                    visible: projectList.count === 0 && !(projects && projects.loading)
                    title: qsTr("Нет проектов")
                    message: qsTr("Создайте первый проект для начала работы")
                    actionText: qsTr("+ Проект")
                    onAction: app.openCreateProjectDialog()
                }
            }

            LoadingOverlay {
                anchors.fill: parent
                visible: projects && projects.loading
                label: qsTr("Загрузка проектов...")
            }
        }
    }

    Component.onCompleted: if (projects) projects.loadProjects()
}
