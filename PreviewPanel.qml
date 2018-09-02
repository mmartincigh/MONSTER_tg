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
        font.pointSize: 10
    }

    Text {
        id: showPreviewLabel

        anchors.right: showPreviewCheckBox.left
        anchors.rightMargin: 20
        anchors.bottom: previewFrame.top
        anchors.bottomMargin: 20

        text: qsTr("show")
        font.pointSize: 10
    }

    CheckBox {
        id: showPreviewCheckBox

        anchors.right: previewFrame.right
        anchors.verticalCenter: showPreviewLabel.verticalCenter

        checked: true
    }

    Rectangle {
        id: previewFrame

        width: 402
        height: 402

        anchors.centerIn: parent

        border.color: "grey"
        radius: 5

        Image {
            id: previewImage

            anchors.centerIn: parent

            //source: "image://imageProvider/no_thumbnail"
            source: thumbnailGenerator.thumbnailUrl
            sourceSize.width: 400
            sourceSize.height: 400
            cache: false
            opacity: showPreviewCheckBox.checked ? 1 : 0

            Behavior on opacity { NumberAnimation { duration: 200 } }

            /*Connections {
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
            }*/
        }
    }

    /*SequentialAnimation {
        id: imageFadeInAnimation

        ScriptAction {
            //script: { previewImage.source = thumbnailGenerator.thumbnailUrl }
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
            //script: { previewImage.source = "" }
        }
    }*/
}
