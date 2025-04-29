import QtQuick
import QtQuick.Controls

Rectangle {
    id: pauseMenu
    //width: 1360
    //height: 768
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    color: "#00000000"
    property color statColor: "#90ffd380"
    property real defaultFontSize: Math.max(height * 0.023, 12)
    property real statWidth: width * 0.14
    property real statHeight: height * 0.065

    Rectangle {
        id: playerStatsBackground
        x: parent.width * 0.1
        y: parent.height - (parent.statHeight * 3.75) - parent.height * 0.04
        width: parent.statWidth
        height: parent.statHeight * 3.75
        color: statColor
        visible: playerHealth.value > 0

        Column {
            id: playerStatsColumn
            anchors.fill: parent
            anchors.leftMargin: parent.width * 0.085
            anchors.rightMargin: parent.width * 0.085
            anchors.bottomMargin: parent.height * 0.085
            anchors.topMargin: parent.height * 0.05
            spacing: parent.height * 0.04

            Text {
                id: playerTitle
                text: qsTr("Player")
                font.family: Qt.application.font.family
                font.bold: true
                font.pointSize: defaultFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
            }

            Text {
                id: playerHealth
                objectName: "playerHealth"
                property int value: 0
                text: qsTr("Health: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }

            Text {
                id: playerDamage
                objectName: "playerDamage"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }

            Text {
                id: playerDefense
                objectName: "playerDefense"
                property int value: 0
                text: qsTr("Defense: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }
        }
    }

    Rectangle {
        id: mobStatsBackground
        x: parent.width * 0.9 - parent.statWidth
        y: parent.height - (parent.statHeight * 3.75) - parent.height * 0.04
        width: parent.statWidth
        height: parent.statHeight * 3.75
        color: statColor
        visible: mobHealth.value > 0

        Column {
            id: mobStatsColumn
            anchors.fill: parent
            anchors.leftMargin: parent.width * 0.085
            anchors.rightMargin: parent.width * 0.085
            anchors.bottomMargin: parent.height * 0.085
            anchors.topMargin: parent.height * 0.05
            spacing: parent.height * 0.04

            Text {
                id: mobTitle
                text: qsTr("Mob")
                font.family: Qt.application.font.family
                font.bold: true
                font.pointSize: defaultFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
            }

            Text {
                id: mobHealth
                objectName: "mobHealth"
                property int value: 0
                text: qsTr("Health: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }

            Text {
                id: mobDamage
                objectName: "mobDamage"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }

            Text {
                id: mobDefense
                objectName: "mobDefense"
                property int value: 0
                text: qsTr("Defense: %1").arg(value)
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: defaultFontSize
            }
        }
    }
}
