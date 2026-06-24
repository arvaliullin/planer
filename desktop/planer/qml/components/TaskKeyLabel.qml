import QtQuick
import QtQuick.Controls
import planer

Label {
    id: root

    property string taskKey

    text: taskKey
    font.bold: true
    font.pixelSize: PlanerTheme.fontCaption
    font.family: "monospace"
    color: palette.link
}
