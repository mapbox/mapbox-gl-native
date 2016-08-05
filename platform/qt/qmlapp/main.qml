import QtGraphicalEffects 1.0
import QtPositioning 5.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0

import QQuickMapboxGL 1.0

ApplicationWindow {
    title: "Mapbox GL QML example"
    width: 1024
    height: 768
    visible: true

    ColorDialog {
        id: landColorDialog
        title: "Land color"
        onCurrentColorChanged: { mapStreets.color = currentColor }
    }

    ColorDialog {
        id: waterColorDialog
        title: "Water color"
        onCurrentColorChanged: { waterColor.value = currentColor }
    }

    MapboxLayoutStyleProperty {
        parent: mapStreets
        layer: "road-label-large"
        property: "visibility"
        value: roadLabel.checked ? "visible" : "none"
    }

    MapboxLayoutStyleProperty {
        parent: mapStreets
        layer: "road-label-medium"
        property: "visibility"
        value: roadLabel.checked ? "visible" : "none"
    }

    MapboxLayoutStyleProperty {
        parent: mapStreets
        layer: "road-label-small"
        property: "visibility"
        value: roadLabel.checked ? "visible" : "none"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 50
        spacing: anchors.margins

        Flipable {
            id: flipable

            Layout.fillWidth: true
            Layout.fillHeight: true

            transform: Rotation {
                origin.x: flipable.width / 2
                origin.y: flipable.height / 2

                axis.x: 0; axis.y: 1; axis.z: 0

                angle: flipSlider.value
            }

            front: Rectangle {
                anchors.fill: parent

                MapboxMap {
                    id: mapStreets

                    anchors.fill: parent
                    visible: false

                    style: "mapbox://styles/mapbox/streets-v9"

                    center: QtPositioning.coordinate(60.170448, 24.942046) // Helsinki
                    zoomLevel: 14
                    minimumZoomLevel: 0
                    maximumZoomLevel: 20

                    bearing: bearingSlider.value
                    pitch: pitchSlider.value

                    color: landColorDialog.color
                    copyrightsVisible: true

                    MapboxPaintStyleProperty {
                        id: waterColor
                        layer: "water"
                        property: "fill-color"
                    }

                    Image {
                        id: logo

                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 20

                        opacity: .75

                        sourceSize.width: 80
                        sourceSize.height: 80

                        source: "icon.png"
                    }
                }

                Rectangle {
                    id: maskStreets

                    anchors.fill: parent
                    anchors.margins: 20

                    radius: 30
                    clip: true
                    visible: false
                }

                OpacityMask {
                    anchors.fill: maskStreets

                    source: mapStreets
                    maskSource: maskStreets
                }

                MouseArea {
                    anchors.fill: parent

                    property var lastX: 0
                    property var lastY: 0

                    onWheel: mapStreets.zoomLevel += 0.2 * wheel.angleDelta.y / 120

                    onPressed: {
                        lastX = mouse.x
                        lastY = mouse.y
                    }

                    onPositionChanged: {
                        mapStreets.pan(mouse.x - lastX, mouse.y - lastY)

                        lastX = mouse.x
                        lastY = mouse.y
                    }
                }
            }

            back: Rectangle {
                anchors.fill: parent

                MapboxMap {
                    id: mapSatellite

                    anchors.fill: parent
                    visible: false

                    style: "mapbox://styles/mapbox/satellite-streets-v9"

                    center: mapStreets.center
                    zoomLevel: mapStreets.zoomLevel
                    minimumZoomLevel: mapStreets.minimumZoomLevel
                    maximumZoomLevel: mapStreets.maximumZoomLevel

                    bearing: mapStreets.bearing
                    pitch: mapStreets.pitch

                    Image {
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: logo.anchors.margins

                        opacity: logo.opacity

                        sourceSize.width: logo.sourceSize.width
                        sourceSize.height: logo.sourceSize.height

                        source: logo.source
                    }
                }

                Rectangle {
                    id: maskSatellite

                    anchors.fill: parent
                    anchors.margins: maskStreets.anchors.margins

                    radius: maskStreets.radius
                    clip: true
                    visible: false
                }

                OpacityMask {
                    anchors.fill: maskSatellite

                    source: mapSatellite
                    maskSource: maskSatellite
                }

                MouseArea {
                    anchors.fill: parent

                    property var lastX: 0
                    property var lastY: 0

                    onWheel: mapStreets.zoomLevel += 0.2 * wheel.angleDelta.y / 120

                    onPressed: {
                        lastX = mouse.x
                        lastY = mouse.y
                    }

                    onPositionChanged: {
                        mapStreets.pan(mouse.x - lastX, mouse.y - lastY)

                        lastX = mouse.x
                        lastY = mouse.y
                    }
                }
            }
        }

        ColumnLayout {
            RowLayout {
                anchors.margins: 50
                spacing: anchors.margins

                Slider {
                    id: bearingSlider

                    Layout.fillHeight: true
                    orientation: Qt.Vertical

                    value: 0
                    minimumValue: 0
                    maximumValue: 180
                }

                Slider {
                    id: pitchSlider

                    Layout.fillHeight: true
                    orientation: Qt.Vertical

                    value: 0
                    minimumValue: 0
                    maximumValue: 60
                }

                Slider {
                    id: flipSlider

                    Layout.fillHeight: true
                    orientation: Qt.Vertical

                    value: 0
                    minimumValue: 0
                    maximumValue: 180
                }
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Select land color"
                onClicked: landColorDialog.open()
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Select water color"
                onClicked: waterColorDialog.open()
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Light style"
                onClicked: { mapStreets.style = "mapbox://styles/mapbox/light-v9" }
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Dark style"
                onClicked: { mapStreets.style = "mapbox://styles/mapbox/dark-v9" }
            }

            CheckBox {
                id: roadLabel
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Toggle road label"
                checked: true
            }
        }
    }
}
