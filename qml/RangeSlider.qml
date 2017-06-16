import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Universal 2.0

RangeSlider {
    id: control
    height: 20
    width: 500
    from: 1
    to: 100
    first.value: 1
    second.value: 100
    snapMode: RangeSlider.SnapAlways

    first.onValueChanged: {

    }

    second.onValueChanged: {

    }

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: "#bdbebf"

        Rectangle {
            x: control.first.visualPosition * parent.width
            width: control.second.visualPosition * parent.width - x
            height: parent.height
            color: "#21be2b"
            radius: 2
        }
    }

    first.handle: Rectangle {
        x: control.leftPadding + first.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 14
        implicitHeight: 14
        radius: 7
        color: first.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
    }

    second.handle: Rectangle {
        x: control.leftPadding + second.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 14
        implicitHeight: 14
        radius: 7
        color: second.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
    }
}
