import QtQuick
import QtQuick.Controls

Rectangle {
    id: pauseMenu
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    color: "#40000000"
    property color buttonColor: "#dbdbdb"

    signal resumeGameClicked
    signal openSettingsClicked
    signal openMainMenuClicked
    signal exitGameClicked

    Text {
        id: playerHealth
        y: parent.height * 0.45
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Health: ")
        font.family: Qt.application.font.family
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: Math.max(parent.height * 0.045, 20)
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: mobHealth
        y: parent.height * 0.25
        width: parent.width * 0.15
        height: parent.height * 0.1
        text: qsTr("Mob health: ")
        font.family: Qt.application.font.family
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: Math.max(parent.height * 0.045, 20)
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
