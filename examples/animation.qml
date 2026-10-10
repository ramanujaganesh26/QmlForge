import QtQuick
import QtQuick.Controls

Rectangle {
    width: 400
    height: 400
    color: "#181825"

    Rectangle {
        id: box
        width: 80
        height: 80
        color: "#89b4fa"
        radius: 16
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        RotationAnimation on rotation {
            from: 0
            to: 360
            duration: 2000
            loops: Animation.Infinite
            running: true
        }

        SequentialAnimation on scale {
            loops: Animation.Infinite
            running: true
            NumberAnimation { to: 1.3; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 30
        text: "Infinite Rotation & Pulse Animation"
        color: "#cdd6f4"
        font.pixelSize: 16
    }
}
