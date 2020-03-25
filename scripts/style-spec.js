const referenceSpec = require('../mapbox-gl-js/src/style-spec/reference/v8');

referenceSpec.layer.type.values["location-component"] = {};
referenceSpec["layout_location-component"] = {
    "top-image": {
        "type": "resolvedImage",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "Name of image in sprite to use as the top of the location component."
    },
    "bearing-image": {
        "type": "resolvedImage",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "Name of image in sprite to use as the middle of the location component."
    },
    "shadow-image": {
        "type": "resolvedImage",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "Name of image in sprite to use as the background of the location component."
    },
    "location": {
        "type": "array",
        "default": [
            0.0,
            0.0,
            0.0
        ],
        "length": 3,
        "value": "number",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": []
        },
        "doc": "An array of [latitude, longitude, altitude] position of the location component."
    },
    "bearing": {
        "type": "number",
        "default": "0",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [ ]
        },
        "doc": "The bearing of the location component."
    },
    "accuracy-radius": {
        "type": "number",
        "default": "0",
        "property-type": "data-constant",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The accuracy, in meters, of the position source used to retrieve the position of the location component."
    },
    "top-image-size": {
        "type": "number",
        "property-type": "data-constant",
        "default": "1",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The size of the top image, in pixels."
    },
    "bearing-image-size": {
        "type": "number",
        "property-type": "data-constant",
        "default": "1",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The size of the bearing image, in pixels."
    },
    "shadow-image-size": {
        "type": "number",
        "property-type": "data-constant",
        "default": "1",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The size of the shadow image, in pixels."
    },
    "perspective-compensation": {
        "type": "number",
        "default": "0.85",
        "property-type": "data-constant",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The amount of the perspective compensation, between 0 and 1. A value of 1 produces a location component of constant width across the screen. A value of 0 makes it scale naturally according to the viewing projection."
    },
    "image-tilt-displacement": {
        "type": "number",
        "property-type": "data-constant",
        "default": "0",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The displacement off the center of the top image and the shadow image when the pitch of the map is greater than 0. This helps producing a three-dimensional appearence."
    }
};

referenceSpec["paint_location-component"] = {
    "accuracy-radius-color": {
        "type": "color",
        "property-type": "data-constant",
        "default": "#ffffff",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The color for drawing the accuracy radius, as a circle. To adjust transparency, set the alpha component of the color accordingly."

    },
    "accuracy-radius-border-color": {
        "type": "color",
        "property-type": "data-constant",
        "default": "#ffffff",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The color for drawing the accuracy radius border. To adjust transparency, set the alpha component of the color accordingly."
    }
};

var spec = module.exports = referenceSpec
