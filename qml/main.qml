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
            checked: false
            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.stanzaOnly = cbStanzaOnly.checked
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbIncludeReceivedStanza
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("Include Received Stanza")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: {
                LogViewAPI.receivedStanza = cbIncludeReceivedStanza.checked
                if (cbIncludeReceivedStanza.checked == false && cbIncludeSentStanza.checked == false)
                    cbIncludeSentStanza.checked = true
            }
        }

        CheckBox {
            id: cbIncludeSentStanza
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("Include Sent Stanza")

            indicator: CheckBoxIndicator{}
            onCheckedChanged:{
                LogViewAPI.sentStanza = cbIncludeSentStanza.checked
                if (cbIncludeReceivedStanza.checked == false && cbIncludeSentStanza.checked == false)
                                  cbIncludeReceivedStanza.checked = true
            }
        }
    }

    Grid {
        width: parent.width
        rows: 2

        CheckBox {
            id: cbPresence
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("presence")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.cbStanzaOnly = cbPresence.checked
        }

        CheckBox {
            id: cbMessage
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("message")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.messageStanza = cbMessage.checked
        }

        CheckBox {
            id: cbSuccess
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("success")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.successStanza = cbSuccess.checked
        }

        CheckBox {
            id: cbIq
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("iq")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.iqStanza = cbIq.checked
        }

        CheckBox {
            id: cbR
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("r")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.rStanza = cbR.checked
        }

        CheckBox {
            id: cbA
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("a")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.aStanza = cbA.checked
        }

        CheckBox {
            id: cbX
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("x")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.xStanza = cbX.checked
        }

        CheckBox {
            id: cbEnable
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("enable")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.enableStanza = cbEnable.checked
        }

        CheckBox {
            id: cbEnabled
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("enabled")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.enabledStanza = cbEnabled.checked
        }

        CheckBox {
            id: cbStreamStream
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("stream:stream")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.streamStreamStanza = cbStreamStream.checked
        }

        CheckBox {
            id: cbStreamFeatures
            enabled: cbStanzaOnly.checked
            checked: true
            text: qsTr("stream:features")

            indicator: CheckBoxIndicator{}
            onCheckedChanged: LogViewAPI.streamFeaturesStanza = cbStreamFeatures.checked
        }
    }
}
