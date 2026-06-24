pragma Singleton
import QtQuick

QtObject {
    readonly property int spacingSmall: 8
    readonly property int spacingMedium: 16
    readonly property int spacingLarge: 24
    readonly property int sidebarWidth: 260
    readonly property int detailPanelWidth: 340
    readonly property int toolbarHeight: 48
    readonly property int radius: 8
    readonly property int radiusSmall: 4

    readonly property int fontTitle: 22
    readonly property int fontHeading: 16
    readonly property int fontBody: 13
    readonly property int fontCaption: 11

    function statusColor(status, palette) {
        switch (status) {
        case "done": return palette.primary
        case "in_progress": return palette.link
        case "review": return palette.link
        case "cancelled": return palette.mid
        case "todo": return palette.button
        default: return palette.button
        }
    }

    function statusLabel(status) {
        switch (status) {
        case "backlog": return qsTr("Backlog")
        case "todo": return qsTr("Todo")
        case "in_progress": return qsTr("In Progress")
        case "review": return qsTr("Review")
        case "done": return qsTr("Done")
        case "cancelled": return qsTr("Cancelled")
        default: return status
        }
    }

    function priorityLabel(priority) {
        switch (priority) {
        case "low": return qsTr("Low")
        case "normal": return qsTr("Normal")
        case "high": return qsTr("High")
        case "critical": return qsTr("Critical")
        default: return priority
        }
    }

    function isOverdue(dueDate, status) {
        if (!dueDate || dueDate.length === 0) return false
        if (status === "done" || status === "cancelled") return false
        var parts = dueDate.split("-")
        if (parts.length !== 3) return false
        var due = new Date(parts[0], parts[1] - 1, parts[2])
        var today = new Date()
        today.setHours(0, 0, 0, 0)
        return due < today
    }

    function str(value) {
        return (value === undefined || value === null) ? "" : String(value)
    }
}
