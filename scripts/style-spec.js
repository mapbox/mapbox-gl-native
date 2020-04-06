const referenceSpec = require('../mapbox-gl-js/src/style-spec/reference/v8');

referenceSpec.layer.type.values["location-indicator"] = {};
referenceSpec["layout_location-indicator"] = {
    "top-image": {
        "type": "resolvedImage",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "Name of image in sprite to use as the top of the location indicator."
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
        "doc": "Name of image in sprite to use as the middle of the location indicator."
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
        "doc": "Name of image in sprite to use as the background of the location indicator."
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
        "doc": "The amount of the perspective compensation, between 0 and 1. A value of 1 produces a location indicator of constant width across the screen. A value of 0 makes it scale naturally according to the viewing projection."
    },
    "image-tilt-displacement": {
        "type": "number",
        "property-type": "data-constant",
        "default": "0",
        "units": "pixels",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "doc": "The displacement off the center of the top image and the shadow image when the pitch of the map is greater than 0. This helps producing a three-dimensional appearence."
    }
};

referenceSpec["paint_location-indicator"] = {
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
            "interpolated": true,
            "parameters": []
        },
        "transition": true,
        "doc": "An array of [latitude, longitude, altitude] position of the location indicator."
    },
    "accuracy-radius": {
        "type": "number",
        "units": "meters",
        "default": 0,
        "property-type": "data-constant",
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The accuracy, in meters, of the position source used to retrieve the position of the location indicator."
    },
    "top-image-size": {
        "type": "number",
        "units": "pixels",
        "property-type": "data-constant",
        "default": 0,
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The size of the top image, in pixels."
    },
    "bearing-image-size": {
        "type": "number",
        "units": "pixels",
        "property-type": "data-constant",
        "default": 0,
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The size of the bearing image, in pixels."
    },
    "shadow-image-size": {
        "type": "number",
        "units": "pixels",
        "property-type": "data-constant",
        "default": 0,
        "expression": {
            "interpolated": true,
            "parameters": [
                "zoom"
            ]
        },
        "transition": true,
        "doc": "The size of the shadow image, in pixels."
    },
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
    },
    "bearing": {
        "type": "number",
        "default": "0",
        "default": 0,
        "period": 360,
        "units": "degrees",
        "property-type": "data-constant",
        "expression": {
            "interpolated": false,
            "parameters": [ ]
        },
        "transition": false,
        "doc": "The bearing of the location indicator."
    }
};

var spec = module.exports = referenceSpec
