import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Universal 2.0
import "qrc:/qml/control"

Column {
    id: column
    width: 500
    height: 140

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

        background: RangeSliderBackground{}
        first.handle: RangeSliderHandle{
            x: parent.leftPadding + parent.first.visualPosition * (parent.availableWidth - width)
            color: parent.first.pressed ? "#f0f0f0" : "#f6f6f6"
        }
        second.handle: RangeSliderHandle{
            x: parent.leftPadding + parent.second.visualPosition * (parent.availableWidth - width)
            color: parent.second.pressed ? "#f0f0f0" : "#f6f6f6"
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbStanzaOnly
            text: qsTr("Stanza Only")
            checked: LogViewAPI.stanzaOnly
            indicator: CheckBoxIndicator{}
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbIncludeReceivedStanza
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.receivedStanza
            text: qsTr("Include Received Stanza")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: if (cbIncludeReceivedStanza.checked == false && cbIncludeSentStanza.checked == false)
                                  cbIncludeSentStanza.checked = true
        }

        CheckBox {
            id: cbIncludeSentStanza
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.sentStanza
            text: qsTr("Include Sent Stanza")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: if (cbIncludeReceivedStanza.checked == false && cbIncludeSentStanza.checked == false)
                                  cbIncludeReceivedStanza.checked = true
        }
    }

    Grid {
        width: parent.width
        rows: 2

        CheckBox {
            id: cbPresence
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.presenceStanza
            text: qsTr("presence")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbMessage
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.messageStanza
            text: qsTr("message")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbSuccess
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.successStanza
            text: qsTr("success")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbIq
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.iqStanza
            text: qsTr("iq")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbR
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.rStanza
            text: qsTr("r")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbA
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.aStanza
            text: qsTr("a")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbX
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.xStanza
            text: qsTr("x")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbEnable
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.enableStanza
            text: qsTr("enable")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbEnabled
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.enabledStanza
            text: qsTr("enabled")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbStreamStream
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.streamStreamStanza
            text: qsTr("stream:stream")

            indicator: CheckBoxIndicator{}
        }

        CheckBox {
            id: cbStreamFeatures
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.streamFeaturesStanza
            text: qsTr("stream:features")

            indicator: CheckBoxIndicator{}
        }
    }
}
