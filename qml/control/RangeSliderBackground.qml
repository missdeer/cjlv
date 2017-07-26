import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Universal 2.0

Rectangle {
    x: parent.leftPadding
    y: parent.topPadding + parent.availableHeight / 2 - height / 2
    implicitWidth: 200
    implicitHeight: 4
    width: parent.availableWidth
    height: implicitHeight
    radius: 2
    color: "#bdbebf"

    Rectangle {
        x: parent.parent.first.visualPosition * parent.width
        width: parent.parent.second.visualPosition * parent.width - x
        height: parent.height
        color: "#21be2b"
        radius: 2
    }
}
