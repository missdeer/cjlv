import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Universal 2.0

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
            x: rangeSlider.leftPadding + rangeSlider.first.visualPosition * (rangeSlider.availableWidth - width)
            y: rangeSlider.topPadding + rangeSlider.availableHeight / 2 - height / 2
            implicitWidth: 14
            implicitHeight: 14
            radius: 7
            color: rangeSlider.first.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }

        second.handle: Rectangle {
            x: rangeSlider.leftPadding + rangeSlider.second.visualPosition * (rangeSlider.availableWidth - width)
            y: rangeSlider.topPadding + rangeSlider.availableHeight / 2 - height / 2
            implicitWidth: 14
            implicitHeight: 14
            radius: 7
            color: rangeSlider.second.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbStanzaOnly
            text: qsTr("Stanza Only")
            checked: LogViewAPI.stanzaOnly
            indicator: Rectangle {
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
        }
    }

    Row {
        width: parent.width

        CheckBox {
            id: cbReceivedStanzaOnly
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.receivedStanzaOnly
            text: qsTr("Received Stanza Only")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbSentStanzaOnly
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.sentStanzaOnly
            text: qsTr("Sent Stanza Only")

            indicator: Rectangle {
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

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbMessage
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.messageStanza
            text: qsTr("message")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbSuccess
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.successStanza
            text: qsTr("success")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbIq
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.iqStanza
            text: qsTr("iq")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbR
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.rStanza
            text: qsTr("r")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbA
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.aStanza
            text: qsTr("a")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbX
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.xStanza
            text: qsTr("x")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbEnable
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.enableStanza
            text: qsTr("enable")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbEnabled
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.enabledStanza
            text: qsTr("enabled")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbStreamStream
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.streamStreamStanza
            text: qsTr("stream:stream")

            indicator: Rectangle {
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
        }

        CheckBox {
            id: cbStreamFeatures
            enabled: cbStanzaOnly.checked
            checked: LogViewAPI.streamFeaturesStanza
            text: qsTr("stream:features")

            indicator: Rectangle {
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
        }
    }
}
