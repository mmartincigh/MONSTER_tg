import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MONSTER.ThumbnailGenerator_GUI 1.0

Item {
    id: main

    property bool enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed

    height: sourcePathComboBox.height

    Connections {
        target: sourceManager
        onEditTextChanged: { sourcePathComboBox.editText = editText }
    }

    Text {
        id: sourcePathLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: sourcePathComboBox.verticalCenter

        text: qsTr("source path")
    }

    ComboBox {
        id: sourcePathComboBox

        height: 25
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: sourcePathLabel.right
        anchors.leftMargin: 20
        anchors.right: sourcePathBrowseButton.left
        anchors.rightMargin: 20

        model: sourceManager.sourcePathModel
        editable: true
        enabled: main.enabled

        onEditTextChanged: { sourceManager.onUpdateEditText(editText) }

        Text {
            id: sourcePathOverlayText

            anchors.centerIn: parent

            text: qsTr("enter or select the source path")
            color: "grey"
            opacity: parent.activeFocus || parent.editText.length > 0 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 125 }
            }
        }
    }

    Button {
        id: sourcePathBrowseButton

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: sourcePathComboBox.verticalCenter

        text: qsTr("browse")
        enabled: main.enabled

        onClicked: { fileDialog.open() }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Select a folder")
        selectFolder: true
        folder: sourceManager.isSourcePathUrlValid ? sourceManager.sourcePathUrl : shortcuts.documents

        onAccepted: { sourceManager.onUpdateEditText(folder) }
    }
}
