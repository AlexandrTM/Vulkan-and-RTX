import QtQuick
import QtQuick.Controls

Rectangle {
    id: rectangle
    width: 1360
    height: 768

    color: "#EAEAEA"

    signal startGameClicked()
    signal openSettingsClicked()
    signal exitGameClicked()

    Text {
        id: label
        y: 40
        width: 240
        height: 64
        text: qsTr("Aether")
        font.family: Qt.application.font.family
        anchors.topMargin: -376
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 30
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: button
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: 252
        width: 240
        height: 64
        text: qsTr("Start Game")
        font.pointSize: 24
        onClicked: rectangle.startGameClicked()
    }

    Button {
        id: button1
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: 352
        width: 240
        height: 64
        text: qsTr("Settings")
        font.pointSize: 24
        onClicked: rectangle.openSettingsClicked()
    }

    Button {
        id: button2
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        y: 452
        width: 240
        height: 64
        text: qsTr("Exit")
        font.pointSize: 24
        onClicked: rectangle.exitGameClicked()
    }
}
