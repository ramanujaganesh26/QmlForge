import QtQuick
import QtQuick.Controls

Rectangle {
    width: 400
    height: 400
    color: "#1e1e2e"

    property int clickCount: 0

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Button Click Count: " + clickCount
            color: "#cdd6f4"
            font.pixelSize: 20
            font.bold: true
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Click Me!"
            onClicked: {
                clickCount++;
                console.log("Button clicked! Current count: " + clickCount);
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Reset Counter"
            onClicked: {
                clickCount = 0;
                console.log("Counter reset.");
            }
        }
    }
}
