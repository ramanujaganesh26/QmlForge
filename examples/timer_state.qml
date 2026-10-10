import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 400
    height: 400
    color: "#181825"
    state: "STOPPED"

    property int seconds: 0

    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: false
        onTriggered: {
            root.seconds++;
        }
    }

    states: [
        State {
            name: "RUNNING"
            PropertyChanges { target: statusText; text: "Timer Running..." }
            PropertyChanges { target: statusBox; color: "#a6e3a1" }
        },
        State {
            name: "STOPPED"
            PropertyChanges { target: statusText; text: "Timer Paused" }
            PropertyChanges { target: statusBox; color: "#f38ba8" }
        }
    ]

    Column {
        anchors.centerIn: parent
        spacing: 20

        Rectangle {
            id: statusBox
            width: 200
            height: 40
            radius: 8
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                id: statusText
                anchors.centerIn: parent
                color: "#11111b"
                font.bold: true
                font.pixelSize: 15
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.seconds + " s"
            color: "#cdd6f4"
            font.pixelSize: 40
            font.bold: true
        }

        Row {
            spacing: 15
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: timer.running ? "Pause" : "Start"
                onClicked: {
                    timer.running = !timer.running;
                    root.state = timer.running ? "RUNNING" : "STOPPED";
                }
            }

            Button {
                text: "Reset"
                onClicked: {
                    timer.running = false;
                    root.seconds = 0;
                    root.state = "STOPPED";
                }
            }
        }
    }
}
