import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 400
    height: 400
    color: "#1e1e2e"
    radius: 12

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Rectangle & Colors"
            color: "#cdd6f4"
            font.pixelSize: 22
            font.bold: true
        }

        Row {
            spacing: 15
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle { width: 60; height: 60; color: "#f38ba8"; radius: 30 }
            Rectangle { width: 60; height: 60; color: "#a6e3a1"; radius: 30 }
            Rectangle { width: 60; height: 60; color: "#89b4fa"; radius: 30 }
            Rectangle { width: 60; height: 60; color: "#f9e2af"; radius: 30 }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Click anywhere to change background"
            color: "#bac2de"
            font.pixelSize: 14
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var colors = ["#1e1e2e", "#181825", "#313244", "#45475a"];
            var nextColor = colors[Math.floor(Math.random() * colors.length)];
            root.color = nextColor;
            console.log("Background color changed to: " + nextColor);
        }
    }
}
