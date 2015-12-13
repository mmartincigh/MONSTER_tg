import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MONSTER.ThumbnailGenerator_GUI 1.0

Item {
    id: main

    property bool enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed

    height: sourcePathTextField.height

    Text {
        id: sourcePathLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: sourcePathTextField.verticalCenter

        text: qsTr("source path")
    }

    TextField {
        id: sourcePathTextField

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: sourcePathLabel.right
        anchors.leftMargin: 20
        anchors.right: sourcePathBrowseButton.left
        anchors.rightMargin: 20

        text: sourceManager.sourcePath
        enabled: main.enabled
        style: TextFieldStyle {
            background: Rectangle {
                border.color: sourceManager.isSourcePathUrlValid ? "grey" : "red"
            }
        }

        onTextChanged: { sourceManager.onUpdateSourcePath(text) }

        Text {
            id: sourcePathOverlayText

            anchors.centerIn: parent

            text: qsTr("enter source path")
            font.pointSize: parent.font.pointSize
            color: "grey"
            opacity: parent.activeFocus || parent.length > 0 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 125 }
            }
        }
    }

    Button {
        id: sourcePathBrowseButton

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: sourcePathTextField.verticalCenter

        text: qsTr("browse")
        enabled: main.enabled

        onClicked: { fileDialog.open() }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Select a folder")
        selectFolder: true
        folder: sourceManager.isSourcePathUrlValid ? sourceManager.sourcePathUrl : shortcuts.documents

        onAccepted: { sourceManager.onUpdateSourcePathUrl(folder) }
    }
}
