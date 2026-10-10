import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    width: 400
    height: 400
    color: "#181825"

    GridLayout {
        anchors.fill: parent
        anchors.margins: 20
        columns: 3
        rows: 3
        rowSpacing: 12
        columnSpacing: 12

        Repeater {
            model: 9
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: index % 2 === 0 ? "#89b4fa" : "#cba6f7"
                radius: 10

                Text {
                    anchors.centerIn: parent
                    text: "Item " + (index + 1)
                    color: "#11111b"
                    font.pixelSize: 16
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: console.log("Clicked grid item " + (index + 1))
                }
            }
        }
    }
}
