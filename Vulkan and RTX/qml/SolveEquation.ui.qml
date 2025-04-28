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
    signal buttonClicked(int buttonIndex)

    Rectangle {
        id: equationBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.2
        width: parent.width * 0.6
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
        anchors.top: equationBackground.bottom
        anchors.topMargin: parent.height * 0.1
        width: parent.width * 0.25
        height: parent.height * 0.12
        color: "#80bbbbdb"
        radius: 10

        // Text {
        //     text: activeFocus ? "I have active focus!" : "I do not have active focus"
        // }
        // onVisibleChanged: answerInput.forceActiveFocus()
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
            onTextChanged: solveEquation.answerSubmitted(answerInput.text)
            onAccepted: solveEquation.answerSubmitted(answerInput.text)
        }
    }
    //Component.onCompleted: answerInput.forceActiveFocus()
}
