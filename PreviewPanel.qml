import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: main

    width: previewFrame.width + 20
    height: previewFrame.height + 20

    Text {
        id: previewLabel

        anchors.left: previewFrame.left
        anchors.bottom: previewFrame.top
        anchors.bottomMargin: 20

        text: qsTr("preview")
    }

    Text {
        id: showPreviewLabel

        anchors.right: showPreviewCheckBox.left
        anchors.rightMargin: 20
        anchors.bottom: previewFrame.top
        anchors.bottomMargin: 20

        text: qsTr("show")
    }

    CheckBox {
        id: showPreviewCheckBox

        anchors.right: previewFrame.right
        anchors.verticalCenter: showPreviewLabel.verticalCenter

        checked: true
    }

    Rectangle {
        id: previewFrame

        width: 202
        height: 202

        anchors.centerIn: parent

        border.color: "grey"
        radius: 5

        Image {
            id: previewImage

            anchors.centerIn: parent

            sourceSize.width: 200
            sourceSize.height: 200
            fillMode: Image.PreserveAspectFit
            cache: false
            opacity: 0

            Connections {
                target: thumbnailGenerator
                onThumbnailUrlChanged: {
                    if (!showPreviewCheckBox.checked) {
                        previewImage.source = "";
                        return;
                    }
                    if (thumbnailUrl == "") {
                        imageFadeInAnimation.complete();
                        imageFadeOutAnimation.start();
                    }
                    else {
                        imageFadeOutAnimation.complete();
                        imageFadeInAnimation.start();
                    }
                }
            }
        }
    }

    SequentialAnimation {
        id: imageFadeInAnimation

        ScriptAction {
            script: { previewImage.source = thumbnailGenerator.thumbnailUrl }
        }
        NumberAnimation {
            target: previewImage
            properties: "opacity"
            to: 1
            duration: 250
        }
    }

    SequentialAnimation {
        id: imageFadeOutAnimation

        NumberAnimation {
            target: previewImage
            properties: "opacity"
            to: 0
            duration: 250
        }
        ScriptAction {
            script: { previewImage.source = "" }
        }
    }
}
