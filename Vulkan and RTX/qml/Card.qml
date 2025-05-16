import QtQuick
import QtQuick.Controls

Item {
    id: cardWrapper
    objectName: "cardWrapper" + index

    property var difficultyTextFunc
    property var difficultyColorFunc

    property string fontFamily: ""
    property real defaultFontSize: 0

    property color buttonColor
    property real buttonRadius: 0
    property real borderThickness: 0

    property int index: 0
    property real angle: 0
    property real animationTime: 0
    property bool flipped: false

    signal clicked(int index)

    Button {
        anchors.fill: parent
        visible: cardWrapper.angle % 360 >= 90 && cardWrapper.angle % 360 <= 270
        onClicked: cardWrapper.clicked(cardWrapper.index)

        transform: Rotation {
            origin.x: cardWrapper.width / 2
            origin.y: cardWrapper.height / 2
            axis.y: 1
            axis.x: 0
            axis.z: 0
            angle: cardWrapper.angle + 180
        }

        background: Rectangle {
            anchors.fill: parent
            color: difficultyColorFunc(difficulty.value)
            radius: width * cardWrapper.buttonRadius

            Rectangle {
                anchors.fill: parent
                anchors.margins: parent.width * cardWrapper.borderThickness
                color: cardWrapper.buttonColor
                radius: (width - 2 * parent.width * cardWrapper.borderThickness) * cardWrapper.buttonRadius
            }
        }

        contentItem: Item {
            anchors.fill: parent
            anchors.centerIn: parent

            Text {
                id: difficulty
                objectName: "difficulty" + index
                property int value: 99
                text: qsTr("Difficulty: %1").arg(difficultyTextFunc(value))
                font.family: cardWrapper.fontFamily
                font.pointSize: cardWrapper.defaultFontSize
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
                id: damage
                objectName: "damage" + index
                property string value: ""
                text: qsTr("Damage: %1").arg(value)
                font.family: cardWrapper.fontFamily
                font.pointSize: cardWrapper.defaultFontSize
                color: "black"

                width: parent.width * 0.9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.height * 0.1
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.1
            }
            Text {
                id: defence
                objectName: "defence" + index
                property string value: ""
                text: qsTr("Defence: %1").arg(value)
                font.family: cardWrapper.fontFamily
                font.pointSize: cardWrapper.defaultFontSize
                color: "black"

                width: parent.width * 0.9
                anchors.bottom: damage.top
                anchors.bottomMargin: parent.height * 0.04
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.1
            }
        }
    }

    Item {
        id: backFace
        anchors.fill: parent
        visible: cardWrapper.angle % 360 < 90 || cardWrapper.angle % 360 > 270

        transform: Rotation {
            origin.x: cardWrapper.width / 2
            origin.y: cardWrapper.height / 2
            axis.y: 1
            axis.x: 0
            axis.z: 0
            angle: cardWrapper.angle
        }
        Rectangle {
            anchors.fill: parent
            color: difficultyColorFunc(difficulty.value)
            radius: width * cardWrapper.buttonRadius

            Rectangle {
                anchors.fill: parent
                anchors.margins: parent.width * cardWrapper.borderThickness
                color: cardWrapper.buttonColor
                radius: (width - 2 * parent.width * cardWrapper.borderThickness) * cardWrapper.buttonRadius
            }
        }
    }

    // Flip animation
    SequentialAnimation {
        id: flipCard
        objectName: "flipCard" + index
        running: false
        PropertyAnimation {
            target: cardWrapper
            property: "angle"
            to: (cardWrapper.angle + 180) % 360
            duration: cardWrapper.animationTime
            easing.type: Easing.Linear
        }
        ScriptAction {
            script: cardWrapper.flipped = !cardWrapper.flipped
        }
    }
}
