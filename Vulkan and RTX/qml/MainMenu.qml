import QtQuick
import QtQuick.Controls

Rectangle {
    id: mainMenu
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    color: "#838383"
    property color buttonColor: "#dbdbdb"

    signal startGameClicked
    signal openSettingsClicked
    signal exitGameClicked

    Text {
        id: label
        y: parent.height * 0.05
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Math Dungeon")
        font.family: Qt.application.font.family
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: Math.max(parent.height * 0.045, 24)
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: button
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.35
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Start Game")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: mainMenu.startGameClicked()
    }

    Button {
        id: button1
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.50
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Settings")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: mainMenu.openSettingsClicked()
    }

    Button {
        id: button2
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.65
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Exit")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: mainMenu.exitGameClicked()
    }
}
