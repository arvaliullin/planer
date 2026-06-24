import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    readonly property string projectName: projects && PlanerTheme.str(projects.activeProject.name).length > 0
        ? PlanerTheme.str(projects.activeProject.name) : qsTr("Planer")
    readonly property string projectKey: projects ? PlanerTheme.str(projects.activeProject.key) : ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: PlanerTheme.toolbarHeight
            color: palette.window

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: PlanerTheme.spacingMedium
                anchors.rightMargin: PlanerTheme.spacingMedium
                spacing: PlanerTheme.spacingSmall

                Rectangle {
                    visible: projectKey.length > 0
                    Layout.preferredWidth: 44
                    Layout.preferredHeight: 28
                    radius: PlanerTheme.radiusSmall
                    color: palette.link

                    Label {
                        anchors.centerIn: parent
                        text: projectKey
                        font.bold: true
                        font.pixelSize: PlanerTheme.fontCaption
                        color: palette.buttonText
                    }
                }

                Label {
                    text: projectName
                    font.bold: true
                    font.pixelSize: PlanerTheme.fontHeading
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Label {
                    visible: session && PlanerTheme.str(session.currentUser.displayName).length > 0
                    text: session ? PlanerTheme.str(session.currentUser.displayName) : ""
                    font.pixelSize: PlanerTheme.fontCaption
                    color: palette.mid
                    elide: Text.ElideRight
                    Layout.maximumWidth: 140
                }

                ToolButton {
                    display: AbstractButton.TextOnly
                    text: qsTr("Выйти")
                    onClicked: session.logout()
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: palette.mid
                opacity: 0.35
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            ProjectSidebar {}

            MainShell {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            TaskDetailPanel {}
        }
    }
}
