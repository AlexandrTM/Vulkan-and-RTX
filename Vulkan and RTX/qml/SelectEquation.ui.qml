import QtQuick
import QtQuick.Controls

Rectangle {
    id: selectEquation
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    //anchors.fill: parent
    color: "#00000000"
    property color buttonColor: "#80aaaaaa"
    property real defaultFontSize: Math.max(height * 0.022, 12)
    property real buttonWidth: width * 0.14
    property real buttonHeight: height * 0.4

    property real borderThickness: 0.05
    property real buttonRadius: 0.1

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

    function difficultyColor(value) {
        if (value >= 0 && value < 1.5)
            return "#907ab89e" // Easy
        else if (value >= 1.5 && value < 3)
            return "#90bdb942" // Medium
        else if (value >= 3 && value < 4.5)
            return "#a0d48954" // Hard
        else if (value >= 4.5 && value < 6)
            return "#a0b34d4d" // Insane
        else if (value >= 6)
            return "#b06a3d8f" // Beyond
        else
            return "#b0888888" // Unknown
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

    Item {
        id: cardWrapper0
        objectName: "cardWrapper0"
        anchors.right: cardWrapper1.left
        anchors.rightMargin: parent.width * 0.03
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight

        property bool flipped: false
        property real angle: 0

        Button {
            id: button0
            anchors.fill: parent
            onClicked: selectEquation.buttonClicked(0)
            visible: cardWrapper0.angle % 360 >= 90
                     && cardWrapper0.angle % 360 <= 270

            transform: Rotation {
                origin.x: cardWrapper0.width / 2
                origin.y: cardWrapper0.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper0.angle + 180
            }

            background: Rectangle {
                anchors.fill: parent
                color: difficultyColor(
                           difficulty0.value) // Outer colored border
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width
                                     * selectEquation.borderThickness // Controls border thickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
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

                    width: parent.width * 0.8
                    anchors.top: parent.top
                    anchors.topMargin: parent.height * 0.1
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
                    //anchors.horizontalCenter: parent.horizontalCenter
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
                }
            }
        }

        // Back Side
        Item {
            id: backFace0
            anchors.fill: parent
            opacity: 1
            visible: cardWrapper0.angle % 360 <= 90
                     || cardWrapper0.angle % 360 >= 270

            // Mirror the rotation so it appears properly
            transform: Rotation {
                origin.x: cardWrapper0.width / 2
                origin.y: cardWrapper0.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper0.angle
            }

            Rectangle {
                anchors.fill: parent
                color: difficultyColor(difficulty0.value)
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width * selectEquation.borderThickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
            }
        }

        // Flip animation
        SequentialAnimation {
            id: flipCard0
            objectName: "flipCard0"
            running: false
            PropertyAnimation {
                target: cardWrapper0
                property: "angle"
                to: (cardWrapper0.angle + 180) % 360
                duration: 400
                easing.type: Easing.Linear
            }
            ScriptAction {
                script: cardWrapper0.flipped = !cardWrapper0.flipped
            }
        }
    }

    Item {
        id: cardWrapper1
        objectName: "cardWrapper1"
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2

        property bool flipped: false
        property real angle: 0

        Button {
            id: button1
            anchors.fill: parent
            onClicked: selectEquation.buttonClicked(1)
            visible: cardWrapper1.angle % 360 >= 90
                     && cardWrapper1.angle % 360 <= 270

            transform: Rotation {
                origin.x: cardWrapper1.width / 2
                origin.y: cardWrapper1.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper1.angle + 180
            }

            background: Rectangle {
                anchors.fill: parent
                color: difficultyColor(
                           difficulty1.value) // Outer colored border
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width
                                     * selectEquation.borderThickness // Controls border thickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
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

                    width: parent.width * 0.8
                    anchors.top: parent.top
                    anchors.topMargin: parent.height * 0.1
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
                }
            }
        }

        // Back Side
        Item {
            id: backFace1
            anchors.fill: parent
            opacity: 1
            visible: cardWrapper1.angle % 360 <= 90
                     || cardWrapper1.angle % 360 >= 270

            // Mirror the rotation so it appears properly
            transform: Rotation {
                origin.x: cardWrapper1.width / 2
                origin.y: cardWrapper1.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper1.angle
            }

            Rectangle {
                anchors.fill: parent
                color: difficultyColor(difficulty1.value)
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width * selectEquation.borderThickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
            }
        }

        // Flip animation
        SequentialAnimation {
            id: flipCard1
            objectName: "flipCard1"
            running: false
            PropertyAnimation {
                target: cardWrapper1
                property: "angle"
                to: (cardWrapper1.angle + 180) % 360
                duration: 400
                easing.type: Easing.Linear
            }
            ScriptAction {
                script: cardWrapper1.flipped = !cardWrapper1.flipped
            }
        }
    }

    Item {
        id: cardWrapper2
        objectName: "cardWrapper2"
        width: selectEquation.buttonWidth
        height: selectEquation.buttonHeight
        anchors.left: cardWrapper1.right
        anchors.leftMargin: parent.width * 0.03
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2

        property bool flipped: false
        property real angle: 0

        // Front side
        Button {
            id: button2
            anchors.fill: parent
            onClicked: selectEquation.buttonClicked(2)
            visible: cardWrapper2.angle % 360 >= 90
                     && cardWrapper2.angle % 360 <= 270

            // Rotation transform for 3D card flip
            transform: Rotation {
                origin.x: cardWrapper2.width / 2
                origin.y: cardWrapper2.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper2.angle + 180
            }

            background: Rectangle {
                anchors.fill: parent
                color: difficultyColor(difficulty2.value)
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width * selectEquation.borderThickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
            }

            contentItem: Item {
                anchors.fill: parent
                Text {
                    id: difficulty2
                    objectName: "difficulty2"
                    property int value: 99
                    text: qsTr("Difficulty: %1").arg(difficultyText(value))
                    font.family: Qt.application.font.family
                    font.pointSize: defaultFontSize
                    color: "black"
                    wrapMode: Text.WordWrap
                    width: parent.width * 0.8
                    anchors.top: parent.top
                    anchors.topMargin: parent.height * 0.1
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
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
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width * 0.1
                }
            }
        }

        // Back Side
        Item {
            id: backFace2
            anchors.fill: parent
            opacity: 1
            visible: cardWrapper2.angle % 360 <= 90
                     || cardWrapper2.angle % 360 >= 270

            // Mirror the rotation so it appears properly
            transform: Rotation {
                origin.x: cardWrapper2.width / 2
                origin.y: cardWrapper2.height / 2
                axis.y: 1
                axis.x: 0
                axis.z: 0
                angle: cardWrapper2.angle
            }

            Rectangle {
                anchors.fill: parent
                color: difficultyColor(difficulty2.value)
                radius: width * selectEquation.buttonRadius

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: parent.width * selectEquation.borderThickness
                    color: selectEquation.buttonColor
                    radius: (width - 2 * parent.width * selectEquation.borderThickness)
                            * selectEquation.buttonRadius
                }
            }
        }

        // Flip animation
        SequentialAnimation {
            id: flipCard2
            objectName: "flipCard2"
            running: false
            PropertyAnimation {
                target: cardWrapper2
                property: "angle"
                to: (cardWrapper2.angle + 180) % 360
                duration: 400
                easing.type: Easing.Linear
            }
            ScriptAction {
                script: cardWrapper2.flipped = !cardWrapper2.flipped
            }
        }
    }
}
