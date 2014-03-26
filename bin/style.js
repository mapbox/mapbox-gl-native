"use strict";

module.exports = {
    "buckets": {
        "water": {
            "source": "streets",
            "layer": "water",
            "type": "fill"
        },
        "road_large": {
            "source": "streets",
            "layer": "road", "field": "class", "value": ["motorway", "main"],
            "type": "line", "cap": "round", "join": "round"
        },
        "road_regular": {
            "source": "streets",
            "layer": "road", "field": "class", "value": "street",
            "type": "line", "cap": "round", "join": "round"
        },
        "road_limited": {
            "source": "streets",
            "layer": "road", "field": "class", "value": "street_limited",
            "type": "line", "cap": "round", "join": "round"
        },
        "park": {
            "source": "streets",
            "layer": "landuse", "field": "class", "value": "park",
            "type": "fill"
        },
        "wood": {
            "source": "streets",
            "layer": "landuse", "field": "class", "value": "wood",
            "type": "fill"
        },
        "school": {
            "source": "streets",
            "layer": "landuse", "field": "class", "value": "school",
            "type": "fill"
        },
        "cemetery": {
            "source": "streets",
            "layer": "landuse", "field": "class", "value": "cemetery",
            "type": "fill"
        },
        "industrial": {
            "source": "streets",
            "layer": "landuse", "field": "class", "value": "industrial",
            "type": "fill"
        },
        "building": {
            "source": "streets",
            "layer": "building",
            "type": "fill"
        },
        "alcohol_poi": {
            "source": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Alcohol"],
            "type": "point"
        },
        "cafe_poi": {
            "source": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Cafe"],
            "type": "point"
        },
        "embassy_poi": {
            "source": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Embassy"],
            "type": "point"
        },
        "park_poi": {
            "source": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Park"],
            "type": "point"
        },
        "restaurant_poi": {
            "source": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Restaurant"],
            "type": "point"
        },
        "country_label": {
            "source": "streets",
            "type": "text",
            "layer": "country_label",
            "feature_type": "point",
            "padding": 10,
            "text_field": "name",
            "path": "horizontal",
            "font": "Open Sans, Jomolhari, Siyam Rupali, Alef, Arial Unicode MS",
            "fontSize": 16
        },
        "place_label": {
            "source": "streets",
            "type": "text",
            "layer": "place_label",
            "feature_type": "point",
            "text_field": "name",
            "path": "horizontal",
            "font": "Open Sans, Jomolhari, Siyam Rupali, Alef, Arial Unicode MS",
            "fontSize": 18,
            "alwaysVisible": true
        },
        "road_label": {
            "source": "streets",
            "type": "text",
            "layer": "road_label",
            "feature_type": "line",
            "text_field": "name",
            "path": "curve",
            "font": "Open Sans, Jomolhari, Siyam Rupali, Alef, Arial Unicode MS",
            "fontSize": 12,
            "textMinDistance": 250,
            "maxAngleDelta": 1.04, // radians
        },
    },
    "sprite": "img/maki-sprite",
    "constants": {
        "park": "#c8df9f",
        "text": "#000000",
    },
    "structure": [
        { "name": "park", "bucket": "park" },
        { "name": "wood", "bucket": "wood" },
        { "name": "water", "bucket": "water" },
        { "name": "building", "bucket": "building" },
        { "name": "road_limited", "bucket": "road_limited" },
        { "name": "road_regular", "bucket": "road_regular" },
        { "name": "road_large", "bucket": "road_large" },
        { "name": "alcohol_poi", "bucket": "alcohol_poi" },
        { "name": "cafe_poi", "bucket": "cafe_poi" },
        { "name": "embassy_poi", "bucket": "embassy_poi" },
        { "name": "park_poi", "bucket": "park_poi" },
        { "name": "restaurant_poi", "bucket": "restaurant_poi" },
        { "name": "country_label", "bucket": "country_label" },
        { "name": "place_label", "bucket": "place_label" },
        { "name": "road_label", "bucket": "road_label" },
    ],
    "classes": [
        {
            "name": "default",
            "layers": {
                "background": {
                    "type": "background",
                    "color": "#FFFFFF"
                },
                "park": {
                    "type": "fill",
                    "color": "park",
                    "antialias": true,
                    "image": "park"
                },
                "wood": {
                    "type": "fill",
                    "color": "#33AA66",
                    "antialias": true,
                    "opacity": 0.1
                },
                "water": {
                    "type": "fill",
                    "color": "#bae0e9",
                    "stroke": "#73b6e6",
                    "antialias": true,
                    "image": "water_etsy"
                },
                "building": {
                    "type": "fill",
                    "color": "#000000",
                    "antialias": true,
                    "opacity": [ "linear", 13, 0, 0.1, 0, 0.1 ]
                },
                "road_limited": {
                    "type": "line",
                    "color": "#BBBBBB",
                    "width": [
                        "stops",
                        { z: 0, val: 1 },
                        { z: 30, val: 1 }
                    ]
                },
                "road_regular": {
                    "type": "line",
                    "color": "#999999",
                    "width": [
                        "stops",
                        { z: 0, val: 0.5 },
                        { z: 13, val: 0.5 },
                        { z: 16, val: 2 },
                        { z: 20, val: 16 },
                        { z: 30, val: 16 }
                    ],
                },
                "road_large": {
                    "type": "line",
                    "color": "#666666",
                    "width": [
                        "stops",
                        { z: 0, val: 0.5 },
                        { z: 11, val: 0.5 },
                        { z: 13, val: 1 },
                        { z: 16, val: 4 },
                        { z: 20, val: 32 },
                        { z: 30, val: 32 }
                    ],
                },
                "alcohol_poi": {
                    "type": "point",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "alcohol-shop",
                    "opacity": [ "linear", 15, 0, 1.0, 0, 0.75 ]
                },
                "cafe_poi": {
                    "type": "point",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "cafe",
                    "opacity": [ "linear", 15, 0, 1.0, 0, 0.75 ]
                },
                "embassy_poi": {
                    "type": "point",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "embassy",
                    "opacity": [ "linear", 15, 0, 1.0, 0, 0.75 ]
                },
                "park_poi": {
                    "type": "point",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "park",
                    "opacity": [ "linear", 15, 0, 1.0, 0, 0.75 ]
                },
                "restaurant_poi": {
                    "enabled": [ "min", 18 ],
                    "type": "point",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "restaurant",
                    "opacity": [ "linear", 15, 0, 1.0, 0, 0.75 ]
                },
                "country_label": {
                    "type": "text",
                    "stroke": [ 1, 1, 1, 0.7 ],
                    "color": "text",
                    "size": 16
                },
                "place_label": {
                    "type": "text",
                    "stroke": [1,1,1,0.7],
                    "color": "text",
                    "size": 18
                },
                "road_label": {
                    "type": "text",
                    "color": "text",
                    "stroke": [1,1,1,0.7],
                    "size": ["exponential", 14, 8, 1, 8, 12]
                },
            }
        }
    ]
};
