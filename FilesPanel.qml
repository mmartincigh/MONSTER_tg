import QtQuick 2.5
import QtQuick.Controls 1.4
import MONSTER.ThumbnailGenerator_GUI 1.0

Item {
    id: main

    height: reloadVideoFilesButton.height

    Text {
        id: videoFilesCountLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: reloadVideoFilesButton.verticalCenter

        text: qsTr("video files count:")
    }

    Text {
        id: videoFilesCount

        anchors.left: videoFilesCountLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: reloadVideoFilesButton.verticalCenter

        text: fileManager.videoFilesCount
    }

    Button {
        id: reloadVideoFilesButton

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: videoFilesCountLabel.right
        anchors.leftMargin: 80

        text: qsTr("reload")
        enabled: thumbnailGenerator.isEnabled && (thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed)

        onClicked: { fileManager.onReloadVideoFiles() }
    }

    Text {
        id: overwriteOutputFilesLabel

        anchors.left: reloadVideoFilesButton.right
        anchors.leftMargin: 20
        anchors.verticalCenter: reloadVideoFilesButton.verticalCenter

        text: qsTr("overwrite output files")
    }

    CheckBox {
        id: overwriteOutputFiles

        anchors.left: overwriteOutputFilesLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: reloadVideoFilesButton.verticalCenter

        checked: fileManager.overwriteOutputFiles
        enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed

        Binding {
            target: fileManager
            property: "overwriteOutputFiles"
            value: overwriteOutputFiles.checked
        }
    }
}
