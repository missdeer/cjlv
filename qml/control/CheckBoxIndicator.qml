import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Universal 2.0

Rectangle {
    implicitWidth: 14
    implicitHeight: 14
    x: parent.leftPadding
    y: parent.height / 2 - height / 2
    radius: 3
    border.color: parent.down ? "#17a81a" : "#21be2b"

    Rectangle {
        width: 8
        height: 8
        x: 3
        y: 3
        radius: 2
        color: parent.down ? "#17a81a" : "#21be2b"
        visible: parent.parent.checked
    }
}
