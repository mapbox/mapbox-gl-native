import QtGraphicalEffects 1.0
import QtPositioning 5.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0

import QQuickMapboxGL 1.0

ApplicationWindow {
    title: "Mapbox GL QML example"
    width: 1024
    height: 768
    visible: true

    MapboxMap {
        id: map
        anchors.fill: parent

        parameters: [
            MapParameter {
                id: style
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
                id: source
                property var type: "source"
                property var name: "routeSource"
                property var sourceType: "geojson"
                property var data: ":source1.geojson"
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
                id: linePaint
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

        states: State {
            name: "moved"; when: mouseArea.pressed
            PropertyChanges { target: linePaint; lineColor: "red"; }
        }

        transitions: Transition {
            ColorAnimation { properties: "lineColor"; easing.type: Easing.InOutQuad; duration: 500 }
        }

        Image {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20

            opacity: .75

            sourceSize.width: 80
            sourceSize.height: 80

            source: "icon.png"
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            property var lastX: 0
            property var lastY: 0

            onWheel: map.zoomLevel += 0.2 * wheel.angleDelta.y / 120

            onPressed: {
                lastX = mouse.x
                lastY = mouse.y
            }

            onPositionChanged: {
                map.pan(mouse.x - lastX, mouse.y - lastY)

                lastX = mouse.x
                lastY = mouse.y
            }
        }
    }

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

    Rectangle {
        anchors.fill: menu
        anchors.margins: -20
        radius: 30
        clip: true
    }

    ColumnLayout {
        id: menu

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 30

        Label {
            text: "Bearing:"
        }

        Slider {
            id: bearingSlider

            anchors.left: parent.left
            anchors.right: parent.right
            maximumValue: 180
        }

        Label {
            text: "Pitch:"
        }

        Slider {
            id: pitchSlider

            anchors.left: parent.left
            anchors.right: parent.right
            maximumValue: 60
        }

        GroupBox {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Style:"

            ColumnLayout {
                ExclusiveGroup { id: styleGroup }
                RadioButton {
                    text: "Streets"
                    checked: true
                    exclusiveGroup: styleGroup
                    onClicked: {
                        style.url = "mapbox://styles/mapbox/streets-v9"
                        landColorDialog.color = "#e0ded8"
                        waterColorDialog.color = "#63c5ee"
                    }
                }
                RadioButton {
                    text: "Dark"
                    exclusiveGroup: styleGroup
                    onClicked: {
                        style.url = "mapbox://styles/mapbox/dark-v9"
                        landColorDialog.color = "#343332"
                        waterColorDialog.color = "#191a1a"
                    }
                }
                RadioButton {
                    text: "Satellite"
                    exclusiveGroup: styleGroup
                    onClicked: {
                        style.url = "mapbox://styles/mapbox/satellite-v9"
                    }
                }
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

        GroupBox {
            anchors.left: parent.left
            anchors.right: parent.right
            title: "Route:"

            ColumnLayout {
                ExclusiveGroup { id: sourceGroup }
                RadioButton {
                    text: "Route 1"
                    checked: true
                    exclusiveGroup: sourceGroup
                    onClicked: {
                        source.data = ":source1.geojson"
                    }
                }
                RadioButton {
                    text: "Route 2"
                    exclusiveGroup: sourceGroup
                    onClicked: {
                        source.data = ":source2.geojson"
                    }
                }
                RadioButton {
                    text: "Route 3"
                    exclusiveGroup: sourceGroup
                    onClicked: {
                        source.data = '{ "type": "FeatureCollection", "features": \
                            [{ "type": "Feature", "properties": {}, "geometry": { \
                            "type": "LineString", "coordinates": [[ 24.934938848018646, \
                            60.16830257086771 ], [ 24.943315386772156, 60.16227776476442 ]]}}]}'
                    }
                }
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
