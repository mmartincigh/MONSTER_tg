import QtQuick 2.5

Item {
    id: main

    Keys.onEscapePressed: { Qt.quit() }

    SourcePanel {
        id: sourcePanel

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
    }

    DestinationPanel {
        id: destinationPanel

        anchors.top: sourcePanel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
    }

    FilesPanel {
        id: filesPanel

        anchors.top: destinationPanel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
    }

    ThumbnailsPanel {
        id: thumbnailsPanel

        anchors.top: filesPanel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: previewPanel.left
        anchors.rightMargin: 20
    }

    PreviewPanel {
        id: previewPanel

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
    }

    Text {
        id: version

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5

        text: qsTr("version ") + application.applicationVersion
    }
}
