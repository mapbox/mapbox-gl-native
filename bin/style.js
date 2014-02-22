"use strict";

module.exports = {
    "buckets": {
        "water": {
            "datasource": "streets",
            "layer": "water",
            "type": "fill"
        },
        "road_large": {
            "datasource": "streets",
            "layer": "road", "field": "class", "value": ["motorway", "main"],
            "type": "line", "cap": "round", "join": "round"
        },
        "road_regular": {
            "datasource": "streets",
            "layer": "road", "field": "class", "value": "street",
            "type": "line", "cap": "round", "join": "round"
        },
        "road_limited": {
            "datasource": "streets",
            "layer": "road", "field": "class", "value": "street_limited",
            "type": "line", "cap": "round", "join": "round"
        },
        "park": {
            "datasource": "streets",
            "layer": "landuse", "field": "class", "value": "park",
            "type": "fill"
        },
        "wood": {
            "datasource": "streets",
            "layer": "landuse", "field": "class", "value": "wood",
            "type": "fill"
        },
        "school": {
            "datasource": "streets",
            "layer": "landuse", "field": "class", "value": "school",
            "type": "fill"
        },
        "cemetery": {
            "datasource": "streets",
            "layer": "landuse", "field": "class", "value": "cemetery",
            "type": "fill"
        },
        "industrial": {
            "datasource": "streets",
            "layer": "landuse", "field": "class", "value": "industrial",
            "type": "fill"
        },
        "building": {
            "datasource": "streets",
            "layer": "building",
            "type": "fill"
        },
        "alcohol_poi": {
            "datasource": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Alcohol"],
            "type": "point"
        },
        "cafe_poi": {
            "datasource": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Cafe"],
            "type": "point"
        },
        "embassy_poi": {
            "datasource": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Embassy"],
            "type": "point"
        },
        "park_poi": {
            "datasource": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Park"],
            "type": "point"
        },
        "restaurant_poi": {
            "datasource": "streets",
            "layer": "poi_label",
            "field": "type",
            "value": ["Restaurant"],
            "type": "point"
        },
    },
    "sprite": "img/maki-sprite",
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
                    "color": "#c8df9f",
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
                        { z: 20, val: 32 },
                        { z: 30, val: 32 }
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
                        { z: 20, val: 64 },
                        { z: 30, val: 64 }
                    ],
                },
                "alcohol_poi": {
                    "type": "marker",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "alcohol-shop",
                    "opacity": 0.75
                },
                "cafe_poi": {
                    "type": "marker",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "cafe",
                    "opacity": 0.75
                },
                "embassy_poi": {
                    "type": "marker",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "embassy",
                    "opacity": 0.75
                },
                "park_poi": {
                    "type": "marker",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "park",
                    "opacity": 0.75
                },
                "restaurant_poi": {
                    "type": "marker",
                    "color": "#cccccc",
                    "size": 18,
                    "image": "restaurant",
                    "opacity": 0.75
                },
            }
        }
    ]
};
