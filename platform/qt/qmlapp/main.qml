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
        color: "#e0ded8"
    }

    ColorDialog {
        id: waterColorDialog
        title: "Water color"
        color: "#63c5ee"
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
                    id: mapFront

                    anchors.fill: parent
                    visible: false

                    parameters: [
                        MapParameter {
                            id: styleStreets
                            property var type: "style"
                            property var url: "mapbox://styles/mapbox/streets-v9"
                        },
                        MapParameter {
                            id: waterPaint
                            property var type: "paint"
                            property var layer: "water"
                            property var fillColor: waterColorDialog.color
                        },
                        MapParameter {
                            property var type: "source"
                            property var name: "routeSource"
                            property var sourceType: "geojson"
                            property var data: ":source.geojson"
                        },
                        MapParameter {
                            property var type: "layer"
                            property var name: "routeCase"
                            property var layerType: "line"
                            property var source: "routeSource"
                        },
                        MapParameter {
                            property var type: "paint"
                            property var layer: "routeCase"
                            property var lineColor: "white"
                            property var lineWidth: 20.0
                        },
                        MapParameter {
                            property var type: "layout"
                            property var layer: "routeCase"
                            property var lineJoin: "round"
                            property var lineCap: lineJoin
                            property var visibility: toggleRoute.checked ? "visible" : "none"
                        },
                        MapParameter {
                            property var type: "layer"
                            property var name: "route"
                            property var layerType: "line"
                            property var source: "routeSource"
                        },
                        MapParameter {
                            property var type: "paint"
                            property var layer: "route"
                            property var lineColor: "blue"
                            property var lineWidth: 8.0
                        },
                        MapParameter {
                            property var type: "layout"
                            property var layer: "route"
                            property var lineJoin: "round"
                            property var lineCap: "round"
                            property var visibility: toggleRoute.checked ? "visible" : "none"
                        },
                        MapParameter {
                            property var type: "image"
                            property var name: "label-arrow"
                            property var sprite: ":label-arrow.svg"
                        },
                        MapParameter {
                            property var type: "image"
                            property var name: "label-background"
                            property var sprite: ":label-background.svg"
                        },
                        MapParameter {
                            property var type: "layer"
                            property var name: "markerArrow"
                            property var layerType: "symbol"
                            property var source: "routeSource"
                        },
                        MapParameter {
                            property var type: "layout"
                            property var layer: "markerArrow"
                            property var iconImage: "label-arrow"
                            property var iconSize: 0.5
                            property var iconIgnorePlacement: true
                            property var iconOffset: [ 0.0, -15.0 ]
                            property var visibility: toggleRoute.checked ? "visible" : "none"
                        },
                        MapParameter {
                            property var type: "layer"
                            property var name: "markerBackground"
                            property var layerType: "symbol"
                            property var source: "routeSource"
                        },
                        MapParameter {
                            property var type: "layout"
                            property var layer: "markerBackground"
                            property var iconImage: "label-background"
                            property var textField: "{name}"
                            property var iconTextFit: "both"
                            property var iconIgnorePlacement: true
                            property var textIgnorePlacement: true
                            property var textAnchor: "left"
                            property var textSize: 16.0
                            property var textPadding: 0.0
                            property var textLineHeight: 1.0
                            property var textMaxWidth: 8.0
                            property var iconTextFitPadding: [ 15.0, 10.0, 15.0, 10.0 ]
                            property var textOffset: [ -0.5, -1.5 ]
                            property var visibility: toggleRoute.checked ? "visible" : "none"
                        },
                        MapParameter {
                            property var type: "paint"
                            property var layer: "markerBackground"
                            property var textColor: "white"
                        },
                        MapParameter {
                            property var type: "filter"
                            property var layer: "markerArrow"
                            property var filter: [ "==", "$type", "Point" ]
                        },
                        MapParameter {
                            property var type: "filter"
                            property var layer: "markerBackground"
                            property var filter: [ "==", "$type", "Point" ]
                        },
                        MapParameter {
                            property var type: "bearing"
                            property var angle: bearingSlider.value
                        },
                        MapParameter {
                            property var type: "pitch"
                            property var angle: pitchSlider.value
                        }
                    ]

                    center: QtPositioning.coordinate(60.170448, 24.942046) // Helsinki
                    zoomLevel: 12.25
                    minimumZoomLevel: 0
                    maximumZoomLevel: 20

                    color: landColorDialog.color
                    copyrightsVisible: true

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

                    source: mapFront
                    maskSource: maskStreets
                }

                MouseArea {
                    anchors.fill: parent

                    property var lastX: 0
                    property var lastY: 0

                    onWheel: mapFront.zoomLevel += 0.2 * wheel.angleDelta.y / 120

                    onPressed: {
                        lastX = mouse.x
                        lastY = mouse.y
                    }

                    onPositionChanged: {
                        mapFront.pan(mouse.x - lastX, mouse.y - lastY)

                        lastX = mouse.x
                        lastY = mouse.y
                    }
                }
            }

            back: Rectangle {
                anchors.fill: parent

                MapboxMap {
                    id: mapBack

                    anchors.fill: parent
                    visible: false

                    parameters: [
                        MapParameter {
                            property var type: "style"
                            property var url: "mapbox://styles/mapbox/satellite-streets-v9"
                        },
                        MapParameter {
                            property var type: "bearing"
                            property var angle: bearingSlider.value
                        },
                        MapParameter {
                            property var type: "pitch"
                            property var angle: pitchSlider.value
                        }
                    ]

                    center: mapFront.center
                    zoomLevel: mapFront.zoomLevel
                    minimumZoomLevel: mapFront.minimumZoomLevel
                    maximumZoomLevel: mapFront.maximumZoomLevel

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

                    source: mapBack
                    maskSource: maskSatellite
                }

                MouseArea {
                    anchors.fill: parent

                    property var lastX: 0
                    property var lastY: 0

                    onWheel: mapFront.zoomLevel += 0.2 * wheel.angleDelta.y / 120

                    onPressed: {
                        lastX = mouse.x
                        lastY = mouse.y
                    }

                    onPositionChanged: {
                        mapFront.pan(mouse.x - lastX, mouse.y - lastY)

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
                text: "Streets style"
                onClicked: {
                    styleStreets.url = "mapbox://styles/mapbox/streets-v9"
                    landColorDialog.color = "#e0ded8"
                    waterColorDialog.color = "#63c5ee"
                }
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Light style"
                onClicked: {
                    styleStreets.url = "mapbox://styles/mapbox/light-v9"
                    landColorDialog.color = "#f6f6f4"
                    waterColorDialog.color = "#cad2d3"
                }
            }

            Button {
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Dark style"
                onClicked: {
                    styleStreets.url = "mapbox://styles/mapbox/dark-v9"
                    landColorDialog.color = "#343332"
                    waterColorDialog.color = "#191a1a"
                }
            }

            CheckBox {
                id: toggleRoute
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Toggle route"
                checked: true
            }
        }
    }
}
