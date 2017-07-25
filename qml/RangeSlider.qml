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
            id: cbStanzaOnly
            text: qsTr("Stanza Only")
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbReceivedStanzaOnly
            text: qsTr("Received Stanza Only")
        }

        CheckBox {
            id: cbSentStanzaOnly
            text: qsTr("Sent Stanza Only")
        }
    }

    Grid {
        width: parent.width
        rows: 2

        CheckBox {
            id: cbPresence
            text: qsTr("presence")
        }

        CheckBox {
            id: cbMessage
            text: qsTr("message")
        }

        CheckBox {
            id: cbSuccess
            text: qsTr("success")
        }

        CheckBox {
            id: cbIq
            text: qsTr("iq")
        }

        CheckBox {
            id: cbR
            text: qsTr("r")
        }

        CheckBox {
            id: cbA
            text: qsTr("a")
        }

        CheckBox {
            id: cbX
            text: qsTr("x")
        }

        CheckBox {
            id: cbEnable
            text: qsTr("enable")
        }

        CheckBox {
            id: cbEnabled
            text: qsTr("enabled")
        }

        CheckBox {
            id: cbStreamStream
            text: qsTr("stream:stream")
        }

        CheckBox {
            id: cbStreamFeatures
            text: qsTr("stream:features")
        }
    }
}
