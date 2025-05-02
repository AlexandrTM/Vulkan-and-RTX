import QtQuick
import QtQuick.Controls

Rectangle {
    id: selectEquation
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    //anchors.fill: parent
    color: "#00000000"
    property color buttonColor: "#80bbbbdb"
    property real defaultFontSize: Math.max(height * 0.025, 12)
    property real buttonWidth: width * 0.14
    property real buttonHeight: height * 0.4

    signal buttonClicked(int buttonIndex)

    function difficultyText(value) {
        if (value >= 0 && value < 1.5)
            return qsTr("Easy")
        else if (value >= 1.5 && value < 3)
            return qsTr("Medium")
        else if (value >= 3 && value < 4.5)
            return qsTr("Hard")
        else if (value >= 4.5 && value < 6)
            return qsTr("Insane")
        else if (value >= 6)
            return qsTr("Beyond")
        else
            return qsTr("Unknown")
    }

    Rectangle {
        id: titleBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.06
        y: parent.height * 0.1
        width: parent.width * 0.2
        height: parent.height * 0.07
        color: "#a8bbbbbb"
        radius: 10

        Text {
            id: title
            anchors.centerIn: parent
            text: qsTr("Select an expression")
            font.family: Qt.application.font.family
            font.pointSize: defaultFontSize
            color: "black"
        }
    }

    Button {
        id: button0
        anchors.right: button1.left
        anchors.rightMargin: parent.width * 0.03
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
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
                property string value: ""
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: defence0
                objectName: "defence0"
                property string value: ""
                text: qsTr("Defence: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: damage0.top
                anchors.bottomMargin: parent.height * 0.04
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Button {
        id: button1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
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
                property string value: ""
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: defence1
                objectName: "defence1"
                property string value: ""
                text: qsTr("Defence: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: damage1.top
                anchors.bottomMargin: parent.height * 0.04
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Button {
        id: button2
        anchors.left: button1.right
        anchors.leftMargin: parent.width * 0.03
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
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
                property string value: ""
                text: qsTr("Damage: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: defence2
                objectName: "defence2"
                property string value: ""
                text: qsTr("Defence: %1").arg(value)
                font.family: Qt.application.font.family
                font.pointSize: defaultFontSize
                color: "black"
                width: parent.width * 0.9
                anchors.bottom: damage2.top
                anchors.bottomMargin: parent.height * 0.04
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
