import QtQuick
import QtQuick.Controls
import planer

Dialog {
    id: root

    title: qsTr("Командная палитра")
    modal: true
    anchors.centerIn: parent
    width: Math.min(parent.width - 48, 400)

    standardButtons: Dialog.Close

    onRejected: app.closeCommandPalette()
    onClosed: app.closeCommandPalette()

    Label {
        width: parent.width
        text: qsTr("В разработке")
        horizontalAlignment: Text.AlignHCenter
    }
}
