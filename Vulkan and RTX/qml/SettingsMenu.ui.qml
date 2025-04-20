import QtQuick
import QtQuick.Controls

Rectangle {
    id: settingsMenu
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768
    border.width: 0

    color: "#838383"
    property color buttonColor: "#dbdbdb"

    signal returnToMainMenuClicked

    Text {
        id: label
        y: parent.height * 0.05
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Settings")
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
        y: parent.height * 0.85
        width: parent.width * 0.18
        height: parent.height * 0.1
        text: qsTr("Main menu")
        font.pointSize: Math.max(parent.height * 0.03, 16)
        onClicked: settingsMenu.returnToMainMenuClicked()
    }
}
