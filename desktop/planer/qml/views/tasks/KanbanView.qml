import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    property string draggedTaskId: ""
    property string draggedFromStatus: ""

    ScrollView {
        anchors.fill: parent
        anchors.margins: PlanerTheme.spacingMedium
        clip: true

        RowLayout {
            spacing: PlanerTheme.spacingMedium
            height: parent.height

            Repeater {
                model: kanban ? kanban.columns : []

                delegate: Rectangle {
                    id: columnRect
                    required property var modelData
                    Layout.preferredWidth: 240
                    Layout.fillHeight: true
                    radius: PlanerTheme.radius
                    color: palette.base
                    border.color: dropArea.containsDrag ? palette.link : palette.mid

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: PlanerTheme.spacingSmall
                        spacing: PlanerTheme.spacingSmall

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                Layout.fillWidth: true
                                text: modelData.title
                                font.bold: true
                                font.pixelSize: PlanerTheme.fontBody
                            }

                            Rectangle {
                                Layout.preferredWidth: 24
                                Layout.preferredHeight: 20
                                radius: 10
                                color: palette.mid
                                opacity: 0.25

                                Label {
                                    anchors.centerIn: parent
                                    text: modelData.cards.length
                                    font.pixelSize: PlanerTheme.fontCaption
                                }
                            }
                        }

                        DropArea {
                            id: dropArea
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            onDropped: {
                                if (root.draggedTaskId.length > 0 &&
                                    modelData.status !== root.draggedFromStatus) {
                                    kanban.moveTask(root.draggedTaskId, modelData.status)
                                }
                                root.draggedTaskId = ""
                                root.draggedFromStatus = ""
                            }

                            ScrollView {
                                anchors.fill: parent
                                clip: true

                                Column {
                                    width: parent.width
                                    spacing: PlanerTheme.spacingSmall

                                    Repeater {
                                        model: modelData.cards

                                        delegate: Item {
                                            required property var modelData
                                            width: dropArea.width - 4
                                            height: card.implicitHeight

                                            TaskCard {
                                                id: card
                                                width: parent.width
                                                task: modelData

                                                DragHandler {
                                                    onActiveChanged: {
                                                        card.dragging = active
                                                        if (active) {
                                                            root.draggedTaskId = modelData.id
                                                            root.draggedFromStatus = modelData.status
                                                        }
                                                    }
                                                }

                                                TapHandler {
                                                    onDoubleTapped: tasks.selectTask(modelData.id)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
