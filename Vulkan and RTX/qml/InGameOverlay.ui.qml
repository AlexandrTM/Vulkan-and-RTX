import QtQuick
import QtQuick.Controls

Rectangle {
    id: pauseMenu
    //width: 1360
    //height: 768
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    color: "#00000000"
    property color buttonColor: "#dbdbdb"

    Rectangle {
        id: playerHealthBackground
        x: parent.width * 0.1
        y: parent.height * 0.85
        width: parent.width * 0.2
        height: parent.height * 0.08
        color: "#90ffd380"
        visible: playerHealth.value > 0

        Text {
            id: playerHealth
            objectName: "playerHealth"
            anchors.fill: parent
            property int value: 0
            text: qsTr("Health: %1").arg(value)
            font.family: Qt.application.font.family
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: Math.max(pauseMenu.height * 0.03, 16)
        }
    }

    Rectangle {
        id: mobHealthBackgroundz
        x: parent.width * 0.75
        y: parent.height * 0.85
        width: parent.width * 0.2
        height: parent.height * 0.08
        color: "#90ffd380"
        visible: mobHealth.value > 0

        Text {
            id: mobHealth
            objectName: "mobHealth"
            anchors.fill: parent
            property int value: 0
            text: qsTr("Mob health: %1").arg(value)
            font.family: Qt.application.font.family
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: Math.max(parent.height * 0.03, 16)
        }
    }
}
