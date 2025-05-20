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
    property real defaultFontSize: Math.max(height * 0.021, 11)
    property real statWidth: width * 0.14
    property real statHeight: height * 0.065

    property string fontFamily: Qt.application.font.family
    property color fontColor: "black"

    function mobNameText(mobIndex) {
        switch (mobIndex) {
        case 0:
            return qsTr("Slime")
        case 1:
            return qsTr("Goblin")
        case 2:
            return qsTr("Orc")
        case 3:
            return qsTr("Skeleton")
        case 4:
            return qsTr("Zombie")
        case 5:
            return qsTr("Bandit")
        case 6:
            return qsTr("Dark Mage")
        case 7:
            return qsTr("Wolf")
        case 8:
            return qsTr("Troll")
        case 9:
            return qsTr("Vampire")
        case 10:
            return qsTr("Elemental")
        case 11:
            return qsTr("Shadow Fiend")
        case 12:
            return qsTr("Lich")
        case 13:
            return qsTr("Demon Brute")
        case 14:
            return qsTr("Ancient Wyrm")
        default:
            return qsTr("Unknown Mob")
        }
    }

    function mobIconPath(mobIndex) {
        if (mobIndex >= 0 && mobIndex <= 14)
            return "../textures/mobs/" + mobIndex + ".png"
        else
            return "../textures/mobs/defaultMobTexture.png"
    }

    Text {
        id: textStyle
        font.family: pauseMenu.fontFamily
        font.pointSize: pauseMenu.defaultFontSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: pauseMenu.fontColor
    }

    Rectangle {
        id: playerStatsBackground
        x: parent.width * 0.015
        y: parent.height - (parent.statHeight * 3.7) - parent.width * 0.015
        width: parent.statWidth
        height: parent.statHeight * 3.7
        color: statColor
        visible: playerHealth.value > 0

        Column {
            id: playerStatsColumn
            anchors.fill: parent
            anchors.leftMargin: parent.width * 0.085
            anchors.rightMargin: parent.width * 0.085
            anchors.bottomMargin: parent.height * 0.085
            anchors.topMargin: parent.height * 0.05
            spacing: parent.height * 0.02

            Text {
                id: playerTitle
                text: qsTr("Player")
                font.bold: true
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize * 0.9
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: playerHealth
                objectName: "playerHealth"
                property int value: 0
                text: qsTr("Health: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: playerDamage
                objectName: "playerDamage"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: playerDefense
                objectName: "playerDefense"
                property int value: 0
                text: qsTr("Defense: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: playerExperience
                objectName: "playerExperience"
                property int value: 0
                text: qsTr("Exp: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
        }
    }

    Rectangle {
        id: mobStatsBackground
        x: parent.width * 0.985 - parent.statWidth
        y: parent.height - (parent.statHeight * 3.7) - parent.width * 0.01
        width: parent.statWidth
        height: parent.statHeight * 3.7
        color: statColor
        visible: mobHealth.value > 0

        Column {
            id: mobStatsColumn
            anchors.fill: parent
            anchors.leftMargin: parent.width * 0.085
            anchors.rightMargin: parent.width * 0.085
            anchors.bottomMargin: parent.height * 0.085
            anchors.topMargin: parent.height * 0.05
            spacing: parent.height * 0.02

            Text {
                id: mobTitle
                objectName: "mobTitle"
                property int value: 99
                text: qsTr("%1").arg(mobNameText(value))
                font.bold: true
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize * 0.9
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: mobHealth
                objectName: "mobHealth"
                property int value: 0
                text: qsTr("Health: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: mobDamage
                objectName: "mobDamage"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: mobDefense
                objectName: "mobDefense"
                property int value: 0
                text: qsTr("Defense: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
            Text {
                id: mobExperience
                objectName: "mobExperience"
                property int value: 0
                text: qsTr("Exp: %1").arg(value)
                font.family: textStyle.font.family
                font.pointSize: textStyle.font.pointSize
                horizontalAlignment: textStyle.horizontalAlignment
                verticalAlignment: textStyle.verticalAlignment
                color: textStyle.color
            }
        }
        Item {
            width: parent.width * 1.5
            height: width
            anchors.bottom: parent.top
            anchors.right: parent.right

            visible: mobStatsColumn.visible

            Rectangle {
                id: mobIconBackground
                anchors.fill: parent
                color: "#ff000000"
            }
            Image {
                id: mobIcon
                source: mobIconPath(mobTitle.value)
                anchors.fill: parent
                anchors.margins: width * 0.1
                fillMode: Image.PreserveAspectFit
                //smooth: true
                //anchors.bottomMargin: height * 0.1
                //anchors.horizontalCenter: mobStatsColumn.horizontalCenter
            }
            Image {
                id: mobFrame
                source: "../textures/mobs/mobFrame.png"
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                //smooth: true
            }
        }
    }

    ListModel {
        id: minimapModel
        objectName: "minimapModel"
    }

    function updateRooms(roomList) {
        minimapModel.clear();
        for (var i = 0; i < roomList.length; ++i) {
            minimapModel.append(roomList[i]);
        }
    }

    Item {
        id: minimapContainer
        objectName: "minimapContainer"
        width: parent.height * 0.3
        height: width
        anchors.left: parent.left
        anchors.top: parent.top

        clip: true

        property int cellSize: minimapContainer.width * 0.12

        // Offset used to center the grid visually
        property int mapOffsetX: 0
        property int mapOffsetY: 0

        Rectangle {
            anchors.fill: parent
            color: "#88777799"
        }

        // Minimap rooms
        Repeater {
            model: minimapModel

            delegate: Rectangle {
                id: minimapCell

                width: minimapContainer.cellSize
                height: width

                // Position rooms on a grid — flip y to draw top-down
                x: (model.x - minimapContainer.mapOffsetX) * width * 1.2 + (minimapContainer.width - width) / 2
                y: (minimapContainer.mapOffsetY - model.y) * height * 1.2 + (minimapContainer.height - height) / 2

                color: model.state === "current" ? "#ff008800"
                     : model.state === "discovered" ? "#779999aa"
                     : model.state === "undiscovered" ? "transparent"
                     : "#ffff0000"

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width * 0.5
                    height: width
                    visible: model.hasMob && (model.state === "current" || model.state === "discovered")
                    color: "#ffbb6666"
                    radius: width / 2
                }
            }
        }
    }
}
