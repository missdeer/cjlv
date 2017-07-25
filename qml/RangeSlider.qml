import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Universal 2.0


Column {
    id: column
    width: 500
    height: 200

    RangeSlider {
        id: rangeSlider
        width: parent.width
        height: 20
        from: LogViewAPI.from
        to: LogViewAPI.to

        onFromChanged: first.value = from
        onToChanged: second.value = to
        first.onValueChanged: LogViewAPI.firstValue = first.value
        second.onValueChanged: LogViewAPI.secondValue = second.value

        background: Rectangle {
            x: rangeSlider.leftPadding
            y: rangeSlider.topPadding + rangeSlider.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: 4
            width: rangeSlider.availableWidth
            height: implicitHeight
            radius: 2
            color: "#bdbebf"

            Rectangle {
                x: rangeSlider.first.visualPosition * parent.width
                width: rangeSlider.second.visualPosition * parent.width - x
                height: parent.height
                color: "#21be2b"
                radius: 2
            }
        }

        first.handle: Rectangle {
            x: rangeSlider.leftPadding + first.visualPosition * (rangeSlider.availableWidth - width)
            y: rangeSlider.topPadding + rangeSlider.availableHeight / 2 - height / 2
            implicitWidth: 14
            implicitHeight: 14
            radius: 7
            color: first.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }

        second.handle: Rectangle {
            x: rangeSlider.leftPadding + second.visualPosition * (rangeSlider.availableWidth - width)
            y: rangeSlider.topPadding + rangeSlider.availableHeight / 2 - height / 2
            implicitWidth: 14
            implicitHeight: 14
            radius: 7
            color: second.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: checkBox
            text: qsTr("Stanza Only")
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: checkBox1
            text: qsTr("Received Stanza Only")
        }

        CheckBox {
            id: checkBox2
            text: qsTr("Sent Stanza Only")
        }
    }

    Grid {
        width: parent.width
        rows: 2

        CheckBox {
            id: checkBox3
            text: qsTr("presence")
        }

        CheckBox {
            id: checkBox4
            text: qsTr("message")
        }

        CheckBox {
            id: checkBox5
            text: qsTr("stream:stream")
        }

        CheckBox {
            id: checkBox6
            text: qsTr("stream:features")
        }

        CheckBox {
            id: checkBox7
            text: qsTr("success")
        }

        CheckBox {
            id: checkBox8
            text: qsTr("iq")
        }

        CheckBox {
            id: checkBox9
            text: qsTr("r")
        }


        CheckBox {
            id: checkBox13
            text: qsTr("a")
        }
        CheckBox {
            id: checkBox10
            text: qsTr("x")
        }


        CheckBox {
            id: checkBox11
            text: qsTr("enable")
        }

        CheckBox {
            id: checkBox12
            text: qsTr("enabled")
        }
    }
}
