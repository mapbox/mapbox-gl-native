import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QQuickMapboxGL 1.0

ApplicationWindow {
    id: window
    title: qsTr("Mapbox GL QML example")
    width: 1024
    height: 768
    visible: true

    Rectangle {
        id: rect
        width: 800
        height: 600
        anchors.centerIn: parent

        NumberAnimation on x {
            from: 0;
            to: 100;
            loops: Animation.Infinite;
            duration: 1000;
        }

        QQuickMapboxGL {
            id: map
            anchors.fill: parent
        } // QQuickMapboxGL

    } // Rectangle
}
