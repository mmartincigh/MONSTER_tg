import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MONSTER.ThumbnailGenerator_GUI 1.0

Item {
    id: main

    property bool enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed

    height: destinationPathTextField.height

    Text {
        id: destinationPathLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: destinationPathTextField.verticalCenter

        text: qsTr("destination path")
    }

    TextField {
        id: destinationPathTextField

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: destinationPathLabel.right
        anchors.leftMargin: 20
        anchors.right: destinationPathBrowseButton.left
        anchors.rightMargin: 20

        text: destinationManager.destinationPath
        enabled: main.enabled
        style: TextFieldStyle {
            background: Rectangle {
                border.color: destinationManager.isDestinationPathUrlValid ? "grey" : "red"
            }
        }

        onTextChanged: { destinationManager.onUpdateDestinationPath(text) }

        Text {
            id: destinationPathOverlayText

            anchors.centerIn: parent

            text: qsTr("enter destination path")
            font.pointSize: parent.font.pointSize
            color: "grey"
            opacity: parent.activeFocus || parent.length > 0 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 125 }
            }
        }
    }

    Button {
        id: destinationPathBrowseButton

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: destinationPathTextField.verticalCenter

        text: qsTr("browse")
        enabled: main.enabled

        onClicked: { fileDialog.open() }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Select a folder")
        selectFolder: true
        folder: destinationManager.isDestinationPathUrlValid ? destinationManager.destinationPathUrl : shortcuts.documents

        onAccepted: { destinationManager.onUpdateDestinationPathUrl(folder) }
    }
}
