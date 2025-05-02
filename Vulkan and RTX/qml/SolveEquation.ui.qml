import QtQuick
import QtQuick.Controls

Rectangle {
    id: solveEquation
    width: parent ? parent.width : 1360
    height: parent ? parent.height : 768

    //anchors.fill: parent
    color: "#00000000"
    property color buttonColor: "#80bbbbdb"
    property real defaultFontSize: Math.max(height * 0.025, 13)
    property real buttonWidth: width * 0.15
    property real buttonHeight: height * 0.4

    signal answerSubmitted(string answer)
    signal answerButtonClicked(int buttonIndex)

    Rectangle {
        id: timeRemainingBackground
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.1
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.06
        width: parent.width * 0.12
        height: parent.height * 0.065
        color: "#bb333333"
        radius: 10

        Text {
            id: timeRemaining
            objectName: "timeRemaining"
            anchors.centerIn: parent
            property string value: ""
            property color dynamicColor: "white"
            color: "white"
            text: qsTr("Time: <font color='%1'>%2</font>").arg(
                      dynamicColor).arg(value)
            font.family: Qt.application.font.family
            font.pointSize: solveEquation.defaultFontSize * 0.9
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle {
        id: expressionBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.16
        width: parent.width * 0.55
        height: parent.height * 0.15
        color: "#a8bbbbbb"
        radius: 10

        Text {
            id: expression
            objectName: "expression"
            anchors.centerIn: parent
            property string value: ""
            text: qsTr("Expression: %1").arg(value)
            font.family: Qt.application.font.family
            font.pointSize: solveEquation.defaultFontSize * 1.2
            color: "black"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle {
        id: answerBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: expressionBackground.bottom
        anchors.topMargin: parent.height * 0.1
        width: parent.width * 0.25
        height: parent.height * 0.12
        color: "#80bbbbdb"
        radius: 10

        TextField {
            id: answerInput
            objectName: "answerInput"
            anchors.fill: parent
            anchors.margins: 10
            font.family: Qt.application.font.family
            font.pointSize: solveEquation.defaultFontSize
            color: "black"
            placeholderText: qsTr("Enter your answer...")
            background: null
            text: ""
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            focus: true
            //onTextChanged: solveEquation.answerSubmitted(answerInput.text)
            onAccepted: solveEquation.answerSubmitted(answerInput.text)
        }
        Button {
            width: solveEquation.width * 0.12
            height: parent.height * 0.9
            anchors.left: parent.right
            anchors.leftMargin: solveEquation.width * 0.01
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Submit")
            onClicked: solveEquation.answerSubmitted(answerInput.text)

            background: Rectangle {
                color: solveEquation.buttonColor
                radius: 10
            }

            contentItem: Text {
                text: parent.text
                anchors.centerIn: parent
                font.family: Qt.application.font.family
                font.pointSize: solveEquation.defaultFontSize * 0.8
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
