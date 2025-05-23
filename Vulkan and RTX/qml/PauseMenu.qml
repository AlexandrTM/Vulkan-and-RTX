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

    Button {
        id: resumeButton
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.25
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Resume")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: pauseMenu.resumeGameClicked()
    }

    Button {
        id: settingsButton
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.40
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Settings")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: pauseMenu.openSettingsClicked()
    }

    Button {
        id: mainMenuButton
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.55
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Main Menu")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: pauseMenu.openMainMenuClicked()
    }

    Button {
        id: exitButton
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.70
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Exit")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: pauseMenu.exitGameClicked()
    }
}
