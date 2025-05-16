import QtQuick
import QtQuick.Controls

Rectangle {
    id: selectEquation
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    //anchors.fill: parent
    color: "#00000000"
    property color buttonColor: "#80aaaaaa"
    property string fontFamily: Qt.application.font.family
    property real fontSize: Math.max(height * 0.021, 11)
    property real buttonWidth: width * 0.14
    property real buttonHeight: height * 0.4

    property real borderThickness: 0.05
    property real buttonRadius: 0.1
    property real animationTime: 250

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
            font.family: selectEquation.fontFamily
            font.pointSize: selectEquation.fontSize
            color: "black"
        }
    }

    Row {
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: parent.width * 0.03

        Card {
            index: 0
            angle: 0
            animationTime: selectEquation.animationTime

            width: selectEquation.buttonWidth
            height: selectEquation.buttonHeight
            defaultFontSize : selectEquation.fontSize
            fontFamily: selectEquation.fontFamily
            buttonRadius : selectEquation.buttonRadius
            buttonColor : selectEquation.buttonColor

            borderThickness: selectEquation.borderThickness
            difficultyTextFunc: selectEquation.difficultyText
            difficultyColorFunc: selectEquation.difficultyColor
            onClicked: function(idx) {
                selectEquation.buttonClicked(idx)
            }
        }

        Card {
            index: 1
            angle: 0
            animationTime: selectEquation.animationTime

            width: selectEquation.buttonWidth
            height: selectEquation.buttonHeight
            defaultFontSize : selectEquation.fontSize
            fontFamily: selectEquation.fontFamily
            buttonRadius : selectEquation.buttonRadius
            buttonColor : selectEquation.buttonColor

            borderThickness: selectEquation.borderThickness
            difficultyTextFunc: selectEquation.difficultyText
            difficultyColorFunc: selectEquation.difficultyColor
            onClicked: function(idx) {
                selectEquation.buttonClicked(idx)
            }
        }

        Card {
            index: 2
            angle: 0
            animationTime: selectEquation.animationTime

            width: selectEquation.buttonWidth
            height: selectEquation.buttonHeight
            defaultFontSize : selectEquation.fontSize
            fontFamily: selectEquation.fontFamily
            buttonRadius : selectEquation.buttonRadius
            buttonColor : selectEquation.buttonColor

            borderThickness: selectEquation.borderThickness
            difficultyTextFunc: selectEquation.difficultyText
            difficultyColorFunc: selectEquation.difficultyColor
            onClicked: function(idx) {
                selectEquation.buttonClicked(idx)
            }
        }
    }
}
