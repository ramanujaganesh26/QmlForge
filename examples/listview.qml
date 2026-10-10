import QtQuick
import QtQuick.Controls

Rectangle {
    width: 400
    height: 400
    color: "#1e1e2e"

    ListModel {
        id: fruitModel
        ListElement { name: "Apple"; price: "$1.99"; iconColor: "#f38ba8" }
        ListElement { name: "Banana"; price: "$0.99"; iconColor: "#f9e2af" }
        ListElement { name: "Blueberry"; price: "$3.49"; iconColor: "#89b4fa" }
        ListElement { name: "Kiwi"; price: "$2.49"; iconColor: "#a6e3a1" }
        ListElement { name: "Orange"; price: "$1.49"; iconColor: "#fab387" }
    }

    ListView {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10
        model: fruitModel

        delegate: Rectangle {
            width: ListView.view.width
            height: 50
            color: "#313244"
            radius: 8

            Row {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                Rectangle {
                    width: 30; height: 30
                    radius: 15
                    color: iconColor
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: name
                    color: "#cdd6f4"
                    font.pixelSize: 16
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item { Layout.fillWidth: true; width: 10 }

                Text {
                    text: price
                    color: "#a6e3a1"
                    font.pixelSize: 15
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
