import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    id: main

    signal completed()

    width: 800
    height: 450
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height

    Component.onCompleted: {
        completed();
        visible = true;
    }

    Base {
        anchors.fill: parent

        focus: true
    }
}
