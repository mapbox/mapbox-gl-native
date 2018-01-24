<!--
  This file is generated.
  Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
-->

# Using Style Functions at Runtime

[Runtime Styling](runtime-styling.html) enables you to modify every aspect of the map’s appearance dynamically as a user interacts with your application. Much of the runtime styling API allows you to specify _style functions_ instead of constant values. A style function allows you to specify in advance how a layout or paint attribute will vary as the zoom level changes or how the appearance of individual features vary based on metadata provided by a content source.

Style functions spare you the inconvenience of manually calculating intermediate values between different zoom levels or creating a multitude of style layers to handle homogeneous features in the map content. For example, if your content source indicates the prices of hotels in an area, you can color-code the hotels by price, relying on a style function to smoothly interpolate among desired colors without having to specify the color for each exact price.

_Data-driven styling_ specifically refers to the use of style functions to vary the map’s appearance based on data in a content source.

You can also specify style functions in a style JSON file, to be applied automatically when the map loads. See the [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#types-function) for details.

![available bikes](img/data-driven-styling/citibikes.png) ![subway lines](img/data-driven-styling/polylineExample.png)

This guide uses earthquake data from the [U.S. Geological Survey](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php) and data-driven styling to style a map based on attributes. For more information about how to work with GeoJSON data in our iOS SDK, please see our [working with GeoJSON data](working-with-geojson-data.html) guide.

A style function is represented at runtime by the `MGLStyleFunction` class. There are three subclasses of `MGLStyleFunction`:

* `MGLCameraStyleFunction` is a style value that changes with zoom level. For example, you can make the radius of a circle increase according to zoom level.
* `MGLSourceStyleFunction` is a style value that changes with the attributes of a feature. For example, you can adjust the radius of a circle based on the magnitude of an earthquake.
* `MGLCompositeStyleFunction` is a style value that changes with both zoom level and attribute values. For example, you can add a circle layer where each circle has a radius based on both zoom level and the magnitude of an earthquake.

The documentation for each individual style layer property notes which style functions are enabled for that property.

## Stops

Stops are dictionary keys that are associated with layer attribute values. With feature attribute values as stops, you can use a dictionary with a zoom level for a key and an expression or constant value for the value. For example, you can use a stop dictionary with the zoom levels 0, 10, and 20 as keys and the colors yellow, orange, and red as the values. Alternatively, attribute values can be the keys.

```swift
let stops: [Float: NSColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]
```

## Interpolation mode

The effect a key has on the style value is determined by the interpolation mode. There are four interpolation modes that can be used with a source style function: exponential, interval, categorical, and identity. You can also use exponential and interval interpolation modes with a camera style function.

### Linear

`MGLInterpolationModeExponential` interpolates linearly or exponentially between style function stop values. By default, the `MGLStyleFunction` options parameter `MGLStyleFunctionOptionInterpolationBase` equals `1`, which represents linear interpolation and doesn’t need to be included in the options dictionary.

The stops dictionary below, for example, shows colors that continuously shift from yellow to orange to red to blue to white based on the attribute value.

```swift
let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
let symbolSource = MGLSource(identifier: "source")
let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
mapView.style?.addSource(source)

let stops: [Float: NSColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]

let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)",
                                 stops)
layer.circleRadius = NSExpression(forConstantValue: 10)
mapView.style?.insertLayer(layer, below: symbolLayer)
```

![exponential mode](img/data-driven-styling/exponential.png)

### Exponential

By combining `MGLInterpolationModeExponential` with an `MGLStyleFunctionOptionInterpolationBase` greater than `0` (other than `1`), you can interpolate between values exponentially, create an accelerated ramp effect.

Here’s a visualization from Mapbox Studio (see [Working with Mapbox Studio](working-with-mapbox-studio.html)) comparing interpolation base values of `1.5` and `0.5` based on zoom.

<img src="img/data-driven-styling/exponential-function.png" height=344/>
<img src="img/data-driven-styling/exponential-function-1.png" height=344/>

The example below increases a layer’s `circleRadius` exponentially based on a map’s zoom level. The `MGLStyleFunctionOptionInterpolationBase` is `1.5`.

```swift
let stops = [
    12: 0.5,
    14: 2,
    18: 18,
]

layer.circleRadius = NSExpression(format: "FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'exponential', 1.5, %@)",
                                  stops)
```

### Interval

`MGLInterpolationModeInterval` creates a range using the keys from the stops dictionary. The range is from the given key to just less than the next key. The attribute values that fall into that range are then styled using the style value assigned to that key.

When we use the stops dictionary given above with an interval interpolation mode, we create ranges where earthquakes with a magnitude of 0 to just less than 2.5 would be yellow, 2.5 to just less than 5 would be orange, and so on.

```swift
let stops: [Float: NSColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]

layer.circleColor = NSExpression(format: "FUNCTION(mag, 'mgl_stepWithMinimum:stops:', %@, %@)",
                                 NSColor.green, stops)
```

![interval mode](img/data-driven-styling/interval.png)

### Categorical

At each stop, `MGLInterpolationModeCategorical` produces an output value equal to the function input. We’re going to use a different stops dictionary than we did for the previous two modes.

There are three main types of events in the dataset: earthquakes, explosions, and quarry blasts. In this case, the color of the circle layer will be determined by the type of event, with a default value of blue to catch any events that do not fall into any of those categories.

```swift
let colors: [String: NSColor] = [
    "earthquake": .orange,
    "explosion": .red,
    "quarry blast": .yellow,
]
let defaultColor = NSColor.blue

layer.circleColor = NSExpression(
    format: "TERNARY(FUNCTION(%@, 'valueForKeyPath:', type) != nil, FUNCTION(%@, 'valueForKeyPath:', type), %@)",
    colors, colors, defaultColor)
```

![categorical mode](img/data-driven-styling/categorical1.png) ![categorical mode](img/data-driven-styling/categorical2.png)

### Identity

`MGLInterpolationModeIdentity` uses the attribute’s value as the style value. For example, you can set the `circleRadius` to the earthquake’s magnitude. Since the attribute value itself will be used as the style value, `sourceStops` should be set to `nil`.

```swift
layer.circleRadius = NSExpression(forKeyPath: "mag")
```

![identity mode](img/data-driven-styling/identity.png)

##Resources

* [USGS](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php)
* [For Style Authors](for-style-authors.html)
