import QtPositioning 5.2
import QtQuick 2.4
import QtQuick.Controls 1.3

import QQuickMapboxGL 1.0

ApplicationWindow {
    id: window
    title: qsTr("Mapbox GL QML example")
    width: 1024
    height: 768
    visible: true

    Rectangle {
        id: rect
        width: parent.width * .8
        height: parent.height * .8
        anchors.centerIn: parent

        QQuickMapboxGL {
            id: map
            anchors.fill: parent

            style: "mapbox://styles/mapbox/streets-v8"

            center: QtPositioning.coordinate(60.170448, 24.942046)
            zoomLevel: 14
            minimumZoomLevel: 8
            maximumZoomLevel: 16

            color: "red"
            copyrightsVisible: true
        }
    }
}
