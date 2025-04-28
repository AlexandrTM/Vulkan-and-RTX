import QtQuick
import QtQuick.Controls

Rectangle {
    id: selectEquation
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    //anchors.fill: parent
    color: "#00000000"
    property color buttonColor: "#80bbbbdb"
    property real defaultFontSize: Math.max(height * 0.025, 13)
    property real buttonWidth: width * 0.15
    property real buttonHeight: height * 0.4

    signal buttonClicked(int buttonIndex)

    function difficultyText(value) {
        if (value === 0)
            return qsTr("Easy")
        else if (value === 1)
            return qsTr("Medium")
        else if (value === 2)
            return qsTr("Hard")
        else if (value === 3)
            return qsTr("Insane")
        else if (value === 4)
            return qsTr("Beyond")
        else
            return qsTr("Unknown")
    }

    Rectangle {
        id: titleBackground
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.1
        width: parent.width * 0.2
        height: parent.height * 0.07
        color: "#a8bbbbbb"

        Text {
            id: title
            anchors.centerIn: parent
            text: qsTr("Select an equation")
            font.family: Qt.application.font.family
            font.pointSize: defaultFontSize
            color: "black"
        }
    }

    Button {
        id: button0
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.1
        anchors.verticalCenter: parent.verticalCenter
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight
        onClicked: selectEquation.buttonClicked(0)

        background: Rectangle {
            color: selectEquation.buttonColor
            radius: 10
        }

        contentItem: Item {
            anchors.centerIn: parent
            anchors.fill: parent

            Text {
                id: difficulty0
                objectName: "difficulty0"
                property int value: 99
                text: qsTr("Difficulty: %1").arg(difficultyText(value))
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                wrapMode: Text.WordWrap
                color: "black"
                anchors.top: parent.top
                width: parent.width * 0.8
                anchors.topMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: damage0
                objectName: "damage0"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Button {
        id: button1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight
        onClicked: selectEquation.buttonClicked(1)

        background: Rectangle {
            color: selectEquation.buttonColor
            radius: 10
        }

        contentItem: Item {
            anchors.centerIn: parent
            anchors.fill: parent

            Text {
                id: difficulty1
                objectName: "difficulty1"
                property int value: 99
                text: qsTr("Difficulty: %1").arg(difficultyText(value))
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                wrapMode: Text.WordWrap
                color: "black"
                anchors.top: parent.top
                width: parent.width * 0.8
                anchors.topMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: damage1
                objectName: "damage1"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Button {
        id: button2
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.1
        anchors.verticalCenter: parent.verticalCenter
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight
        onClicked: selectEquation.buttonClicked(2)

        background: Rectangle {
            color: selectEquation.buttonColor
            radius: 10
        }

        contentItem: Item {
            anchors.centerIn: parent
            anchors.fill: parent

            Text {
                id: difficulty2
                objectName: "difficulty2"
                property int value: 99
                text: qsTr("Difficulty: %1").arg(difficultyText(value))
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                wrapMode: Text.WordWrap
                color: "black"
                anchors.top: parent.top
                width: parent.width * 0.8
                anchors.topMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: damage2
                objectName: "damage2"
                property int value: 0
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
