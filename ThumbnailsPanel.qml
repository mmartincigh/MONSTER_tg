import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import MONSTER.ThumbnailGenerator_GUI 1.0

Item {
    id: main

    Text {
        id: thumbnailRowsLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: thumbnailRowsTextField.verticalCenter

        text: qsTr("thumbnail rows")
    }

    TextField {
        id: thumbnailRowsTextField

        width: 50

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: thumbnailRowsLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.thumbnailRows
        enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed
        maximumLength: 5
        validator: IntValidator {
            bottom: 1
            top: 99
        }
        style: TextFieldStyle {
            background: Rectangle {
                border.color: thumbnailRowsTextField.text == "" ? "red" : "grey"
            }
        }

        onTextChanged: { thumbnailGenerator.onUpdateThumbnailRows(text == "" ? 0 : text) }
        onActiveFocusChanged: {
            if (text == "") { text = thumbnailGenerator.thumbnailRows }
        }
    }

    Text {
        id: thumbnailColumnsLabel

        anchors.left: thumbnailRowsTextField.right
        anchors.leftMargin: 20
        anchors.verticalCenter: thumbnailColumnsTextField.verticalCenter

        text: qsTr("thumbnail columns")
    }

    TextField {
        id: thumbnailColumnsTextField

        width: 50

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: thumbnailColumnsLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.thumbnailColumns
        enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed
        maximumLength: 5
        validator: IntValidator {
            bottom: 1
            top: 99
        }
        style: TextFieldStyle {
            background: Rectangle {
                border.color: thumbnailColumnsTextField.text == "" ? "red" : "grey"
            }
        }

        onTextChanged: { thumbnailGenerator.onUpdateThumbnailColumns(text == "" ? 0 : text) }
        onActiveFocusChanged: {
            if (text == "") { text = thumbnailGenerator.thumbnailColumns }
        }
    }

    Text {
        id: thumbnailMaxWidthLabel

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: thumbnailMaxWidthTextField.verticalCenter

        text: qsTr("thumbnail max width")
    }

    TextField {
        id: thumbnailMaxWidthTextField

        width: 50

        anchors.top: thumbnailRowsTextField.bottom
        anchors.topMargin: 20
        anchors.left: thumbnailMaxWidthLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.thumbnailMaxWidth
        enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed
        maximumLength: 5
        validator: IntValidator {
            bottom: 1
            top: 1000
        }
        style: TextFieldStyle {
            background: Rectangle {
                border.color: thumbnailMaxWidthTextField.text == "" ? "red" : "grey"
            }
        }

        onTextChanged: { thumbnailGenerator.onUpdateThumbnailMaxWidth(text == "" ? 0 : text) }
        onActiveFocusChanged: {
            if (text == "") { text = thumbnailGenerator.thumbnailMaxWidth }
        }
    }

    Text {
        id: thumbnailMaxHeightLabel

        anchors.left: thumbnailMaxWidthTextField.right
        anchors.leftMargin: 20
        anchors.verticalCenter: thumbnailMaxHeightTextField.verticalCenter

        text: qsTr("thumbnail max height")
    }

    TextField {
        id: thumbnailMaxHeightTextField

        width: 50

        anchors.top: thumbnailRowsTextField.bottom
        anchors.topMargin: 20
        anchors.left: thumbnailMaxHeightLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.thumbnailMaxHeight
        enabled: thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed
        maximumLength: 5
        validator: IntValidator {
            bottom: 1
            top: 1000
        }
        style: TextFieldStyle {
            background: Rectangle {
                border.color: thumbnailMaxHeightTextField.text == "" ? "red" : "grey"
            }
        }

        onTextChanged: { thumbnailGenerator.onUpdateThumbnailMaxHeight(text == "" ? 0 : text) }
        onActiveFocusChanged: {
            if (text == "") { text = thumbnailGenerator.thumbnailMaxHeight }
        }
    }

    Button {
        id: generateThumbnailsButton

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: progressBar.verticalCenter

        text: qsTr("generate thumbnails")
        enabled: thumbnailGenerator.isEnabled && (thumbnailGenerator.state === Enums.Idle || thumbnailGenerator.state === Enums.Stopped || thumbnailGenerator.state === Enums.Completed)

        onClicked: {
            sourceManager.onGenerateThumbnails();
            destinationManager.onGenerateThumbnails();
            thumbnailGenerator.onGenerateThumbnails();
        }
    }

    ProgressBar {
        id: progressBar

        anchors.top: thumbnailMaxWidthTextField.bottom
        anchors.topMargin: 20
        anchors.left: generateThumbnailsButton.right
        anchors.leftMargin: 20
        anchors.right: generateThumbnailsStatus.left
        anchors.rightMargin: 20

        value: thumbnailGenerator.progress
    }

    Text {
        id: generateThumbnailsStatus

        width: 50

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: progressBar.verticalCenter

        text: qsTr(thumbnailGenerator.stateDescription)
    }

    Button {
        id: pauseButton

        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20

        text: qsTr("pause")
        enabled: thumbnailGenerator.isEnabled && (thumbnailGenerator.state === Enums.Working)

        onClicked: { thumbnailGenerator.onPause() }
    }

    Button {
        id: resumeButton

        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.left: pauseButton.right
        anchors.leftMargin: 20

        text: qsTr("resume")
        enabled: thumbnailGenerator.isEnabled && (thumbnailGenerator.state === Enums.Paused)

        onClicked: { thumbnailGenerator.onResume() }
    }

    Button {
        id: stopButton

        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.left: resumeButton.right
        anchors.leftMargin: 20

        text: qsTr("stop")
        enabled: thumbnailGenerator.isEnabled && (thumbnailGenerator.state === Enums.Paused || thumbnailGenerator.state === Enums.Working)

        onClicked: { thumbnailGenerator.onStop() }
    }

    Text {
        id: progressLabel

        anchors.left: stopButton.right
        anchors.leftMargin: 40
        anchors.verticalCenter: stopButton.verticalCenter

        text: qsTr("progress:")
    }

    Text {
        id: progress

        anchors.left: progressLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: stopButton.verticalCenter

        text: thumbnailGenerator.progressString
    }

    Text {
        id: errorsLabel

        anchors.top: pauseButton.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20

        text: qsTr("errors:")
    }

    Text {
        id: errors

        anchors.top: pauseButton.bottom
        anchors.topMargin: 20
        anchors.left: skipped.left

        text: thumbnailGenerator.errors
        color: thumbnailGenerator.errors > 0 ? "red" : "black"
    }

    Text {
        id: warningsLabel

        anchors.top: pauseButton.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 140

        text: qsTr("warnings:")
    }

    Text {
        id: warnings

        anchors.top: pauseButton.bottom
        anchors.topMargin: 20
        anchors.left: overwritten.left

        text: thumbnailGenerator.warnings
        color: thumbnailGenerator.warnings > 0 ? "orange" : "black"
    }

    Text {
        id: skippedLabel

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20

        text: qsTr("skipped:")
    }

    Text {
        id: skipped

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: skippedLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.skipped
        color: thumbnailGenerator.skipped > 0 ? "blue" : "black"
    }

    Text {
        id: overwrittenLabel

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 140

        text: qsTr("overwritten:")
    }

    Text {
        id: overwritten

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: overwrittenLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.overwritten
        color: thumbnailGenerator.overwritten > 0 ? "blue" : "black"
    }

    Text {
        id: processedLabel

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: overwrittenLabel.right
        anchors.leftMargin: 80

        text: qsTr("processed:")
    }

    Text {
        id: processed

        anchors.top: errorsLabel.bottom
        anchors.topMargin: 20
        anchors.left: processedLabel.right
        anchors.leftMargin: 20

        text: thumbnailGenerator.processed
        color: thumbnailGenerator.processed > 0 ? "green" : "black"
    }

    Text {
        id: currentInputFileLabel

        anchors.top: skippedLabel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20

        text: qsTr("current input file:")
    }

    Text {
        id: currentInputFile

        anchors.top: skippedLabel.bottom
        anchors.topMargin: 20
        anchors.left: currentInputFileLabel.right
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        text: thumbnailGenerator.currentInputFile
        clip: true
    }
}
