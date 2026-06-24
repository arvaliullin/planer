import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import planer

Item {
    id: root

    readonly property int dayWidth: gantt && gantt.scale === 0 ? 28 : 12
    readonly property var startDate: chartArea.chartStart
    readonly property var endDate: chartArea.chartEnd

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: PlanerTheme.spacingSmall

            Label { text: qsTr("Масштаб:") }

            ButtonGroup { id: scaleGroup }

            Button {
                checkable: true
                checked: gantt && gantt.scale === 0
                text: qsTr("Неделя")
                ButtonGroup.group: scaleGroup
                onClicked: gantt.setScale(0)
            }

            Button {
                checkable: true
                checked: gantt && gantt.scale === 1
                text: qsTr("Месяц")
                ButtonGroup.group: scaleGroup
                onClicked: gantt.setScale(1)
            }

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Обновить")
                onClicked: gantt.loadGantt()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: unscheduledList.contentHeight + 16
            visible: unscheduledList.count > 0
            color: palette.base
            border.color: palette.mid

            Column {
                id: unscheduledColumn
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Label {
                    text: qsTr("Unscheduled")
                    font.bold: true
                }

                Repeater {
                    id: unscheduledList
                    model: gantt ? gantt.unscheduledTasks : []

                    delegate: ItemDelegate {
                        required property var modelData
                        width: unscheduledColumn.width
                        text: modelData.key + " — " + modelData.title
                        onClicked: tasks.selectTask(modelData.id)
                    }
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Flickable {
                id: chartFlickable
                contentWidth: chartArea.width
                contentHeight: chartArea.height
                boundsBehavior: Flickable.StopAtBounds

                Item {
                    id: chartArea
                    width: Math.max(chartFlickable.width, dayCount * root.dayWidth + 200)
                    height: Math.max(chartFlickable.height, (gantt ? gantt.scheduledTasks.length : 0) * 44 + 60)

                    readonly property int dayCount: {
                        var s = chartStart
                        var e = chartEnd
                        return Math.max(1, Math.ceil((e - s) / 86400000) + 1)
                    }

                    readonly property var chartStart: {
                        var d = new Date()
                        d.setDate(d.getDate() - 14)
                        d.setHours(0, 0, 0, 0)
                        return d
                    }

                    readonly property var chartEnd: {
                        var d = new Date()
                        d.setDate(d.getDate() + 45)
                        d.setHours(0, 0, 0, 0)
                        return d
                    }

                    function dateToX(isoDate) {
                        if (!isoDate || isoDate.length === 0) return 0
                        var p = isoDate.split("-")
                        var d = new Date(p[0], p[1] - 1, p[2])
                        var diff = (d - chartStart) / 86400000
                        return 180 + diff * root.dayWidth
                    }

                    Row {
                        y: 0
                        height: 30
                        x: 180

                        Repeater {
                            model: chartArea.dayCount

                            delegate: Label {
                                required property int index
                                x: index * root.dayWidth
                                width: root.dayWidth
                                text: {
                                    var d = new Date(chartArea.chartStart)
                                    d.setDate(d.getDate() + index)
                                    return d.getDate()
                                }
                                font.pixelSize: 10
                                color: palette.mid
                            }
                        }
                    }

                    Rectangle {
                        x: chartArea.dateToX(todayIso())
                        y: 30
                        width: 2
                        height: parent.height - 30
                        color: palette.highlight
                        opacity: 0.7

                        function todayIso() {
                            var d = new Date()
                            var m = (d.getMonth() + 1).toString().padStart(2, "0")
                            var day = d.getDate().toString().padStart(2, "0")
                            return d.getFullYear() + "-" + m + "-" + day
                        }
                    }

                    Repeater {
                        model: gantt ? gantt.scheduledTasks : []

                        delegate: Item {
                            required property var modelData
                            required property int index

                            y: 40 + index * 44
                            height: 36
                            width: chartArea.width

                            Label {
                                x: 8
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.key
                                elide: Text.ElideRight
                                font.bold: gantt.highlightedTaskId === modelData.id
                            }

                            Rectangle {
                                id: bar
                                y: 4
                                height: 28
                                radius: 4
                                color: PlanerTheme.statusColor(modelData.status, palette)
                                border.width: PlanerTheme.isOverdue(modelData.dueDate, modelData.status) ? 2 : 0
                                border.color: palette.highlight

                                property real startX: chartArea.dateToX(modelData.startDate || modelData.dueDate)
                                property real endX: chartArea.dateToX(modelData.dueDate || modelData.startDate) + root.dayWidth
                                x: startX
                                width: Math.max(root.dayWidth, endX - startX)

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: parent.width * (modelData.progress / 100.0)
                                    radius: 4
                                    color: palette.shadow
                                    opacity: 0.25
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    drag.target: bar
                                    drag.axis: Drag.XAxis
                                    drag.minimumX: 180

                                    property real dragStartX: 0
                                    property string origStart: modelData.startDate
                                    property string origDue: modelData.dueDate

                                    onPressed: dragStartX = bar.x
                                    onReleased: {
                                        var deltaDays = Math.round((bar.x - dragStartX) / root.dayWidth)
                                        if (deltaDays === 0) return
                                        var newStart = shiftDate(origStart || origDue, deltaDays)
                                        var newDue = shiftDate(origDue || origStart, deltaDays)
                                        gantt.moveTaskDates(modelData.id, newStart, newDue)

                                        function shiftDate(iso, days) {
                                            if (!iso) return ""
                                            var p = iso.split("-")
                                            var d = new Date(p[0], p[1] - 1, p[2])
                                            d.setDate(d.getDate() + days)
                                            var m = (d.getMonth() + 1).toString().padStart(2, "0")
                                            var day = d.getDate().toString().padStart(2, "0")
                                            return d.getFullYear() + "-" + m + "-" + day
                                        }
                                    }

                                    onDoubleClicked: tasks.selectTask(modelData.id)
                                }
                            }
                        }
                    }

                    Canvas {
                        id: depCanvas
                        anchors.fill: parent
                        z: 10

                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.clearRect(0, 0, width, height)
                            if (!gantt) return

                            ctx.strokeStyle = palette.mid
                            ctx.lineWidth = 1.5
                            ctx.fillStyle = palette.mid

                            var tasksById = {}
                            for (var i = 0; i < gantt.scheduledTasks.length; ++i) {
                                tasksById[gantt.scheduledTasks[i].id] = i
                            }

                            for (var d = 0; d < gantt.dependencies.length; ++d) {
                                var dep = gantt.dependencies[d]
                                var bIdx = tasksById[dep.blockerId]
                                var tIdx = tasksById[dep.blockedId]
                                if (bIdx === undefined || tIdx === undefined) continue

                                var bTask = gantt.scheduledTasks[bIdx]
                                var tTask = gantt.scheduledTasks[tIdx]
                                var x1 = chartArea.dateToX(bTask.dueDate || bTask.startDate) + root.dayWidth
                                var y1 = 40 + bIdx * 44 + 18
                                var x2 = chartArea.dateToX(tTask.startDate || tTask.dueDate)
                                var y2 = 40 + tIdx * 44 + 18

                                ctx.beginPath()
                                ctx.moveTo(x1, y1)
                                ctx.lineTo(x1 + 12, y1)
                                ctx.lineTo(x1 + 12, y2)
                                ctx.lineTo(x2, y2)
                                ctx.stroke()

                                ctx.beginPath()
                                ctx.moveTo(x2, y2)
                                ctx.lineTo(x2 - 6, y2 - 4)
                                ctx.lineTo(x2 - 6, y2 + 4)
                                ctx.closePath()
                                ctx.fill()
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            onClicked: function(mouse) {
                                if (!gantt) return
                                for (var d = 0; d < gantt.dependencies.length; ++d) {
                                    var dep = gantt.dependencies[d]
                                    gantt.highlightDependency(dep.blockerId, dep.blockedId)
                                }
                            }
                        }

                        Connections {
                            target: gantt
                            function onDataChanged() { depCanvas.requestPaint() }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: gantt.loadGantt()
}
