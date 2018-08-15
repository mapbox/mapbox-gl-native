<!--
  This file is generated.
  Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
-->

# Migrating from Style Functions to Expressions

[Runtime Styling](https://www.mapbox.com/ios-sdk/maps/overview/runtime-styling/) enables you to modify every aspect of the map’s appearance dynamically as a user interacts with your application. Developers can specify in advance how a layout or paint attribute will vary as the zoom level changes or how the appearance of individual features vary based on metadata provided by a content source.

With Mapbox Maps SDK for iOS v4.0.0, style functions have been replaced with expressions. These provide even more tools for developers who want to style their maps dynamically. This guide outlines some tips for migrating from style functions to expressions, and offers an overview of some things that developers can do with expressions.

An expression is represented at runtime by the `NSExpression` class. Expressions can be used to style paint and layout properties based on zoom level, data attributes, or a combination of the two.

A constant expression can also be assigned to a style property. For example, the opacity of a fill style layer can be set to a constant value between 0 and 1.

The documentation for each individual style layer property notes which non-constant expressions are enabled for that property. Style functions supported four interpolation modes: exponential, interval, categorical, and identity.

This guide uses earthquake data from the [U.S. Geological Survey](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php). Under each interpolation mode, the style function implementation will be shown, followed by the current syntax.

For more information about how to work with GeoJSON data in our iOS SDK, please see our [working with GeoJSON data](working-with-geojson-data.html) guide. To learn more about supported expressions, see our ["Predicates and Expressions"](predicates-and-expressions.html) guide. The "Predicates and Expressions" guide also outlines Mapbox custom functions that can be used to dynamically style a map.

## Stops
Stops are dictionary keys that are associated with layer attribute values. Constant values no longer need to be wrapped as style values when they are values in a stops dictionary.


Style function syntax:

```swift
let stops = [
    0: MGLStyleValue<UIColor>(rawValue: .yellow),
    2.5: MGLStyleValue(rawValue: .orange),
    5: MGLStyleValue(rawValue: .red),
    7.5: MGLStyleValue(rawValue: .blue),
    10: MGLStyleValue(rawValue: .white),
]
```

Current syntax:
```swift
let stops: [NSNumber: UIColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]
```


## Interpolation mode

Style functions supported four interpolation modes: exponential/linear, interval, categorical, and identity. For more information about supported custom expressions, please see the "Predicates and Expressions" guide.

### Linear

`+[NSExpression(MGLAdditions) mgl_expressionForInterpolatingExpression:withCurveType:parameters:stops:]` takes the interpolation type as a parameter. If you previously used the default interpolation base, use the curve type `MGLExpressionInterpolationMode.linear`. See the [`mgl_interpolate:withCurveType:parameters:stops:`](predicates-and-expressions.html#code-mgl_interpolate-withcurvetype-parameters-stops-code) documentation for more details.

The stops dictionary below, shows colors that continuously shift from yellow to orange to red to blue to white based on the attribute value.

Style function syntax:

```swift
let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
let symbolSource = MGLSource(identifier: "source")
let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
mapView.style?.addSource(source)

let stops = [
    0: MGLStyleValue<UIColor>(rawValue: .yellow),
    2.5: MGLStyleValue(rawValue: .orange),
    5: MGLStyleValue(rawValue: .red),
    7.5: MGLStyleValue(rawValue: .blue),
    10: MGLStyleValue(rawValue: .white),
]

let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
layer.circleColor = MGLStyleValue(interpolationMode: .exponential,
                                  sourceStops: stops,
                                  attributeName: "mag",
                                  options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
layer.circleRadius = MGLStyleValue(rawValue: 10)
mapView.style?.insertLayer(layer, below: symbolLayer)
```

Current syntax:

```swift
let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
let symbolSource = MGLSource(identifier: "source")
let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
mapView.style?.addSource(source)

let stops: [NSNumber: UIColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]

let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
layer.circleColor = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:(mag, 'linear', nil, %@)",
                                 stops)
layer.circleRadius = NSExpression(forConstantValue: 10)
mapView.style?.insertLayer(layer, below: symbolLayer)
```

### Exponential

If you previously used an interpolation base greater than `0` (other than `1`), you can use `MGLExpressionInterpolationMode.exponential` as the curve type for `+[NSExpression(MGLAdditions) mgl_expressionForInterpolatingExpression:withCurveType:parameters:stops:]` or `'exponential'` as the curve type for [`mgl_interpolate:withCurveType:parameters:stops:`](predicates-and-expressions.html#code-mgl_interpolate-withcurvetype-parameters-stops-code). The `parameters` argument takes that interpolation base. This interpolates between values exponentially, creating an accelerated ramp effect.

Here’s a visualization from Mapbox Studio (see [Mapbox Studio and iOS](https://www.mapbox.com/ios-sdk/maps/overview/mapbox-studio/)) comparing interpolation base values of `1.5` and `0.5` based on zoom. In order to convert camera style functions, use `$zoomLevel` or `MGL_FUNCTION('zoomLevel')` as the attribute key.

<img src="img/data-driven-styling/exponential-function.png" height=344/>
<img src="img/data-driven-styling/exponential-function-1.png" height=344/>

The example below increases a layer’s `circleRadius` exponentially based on a map’s zoom level. The interpolation base is `1.5`.

Style function syntax:

```swift
let stops = [
    12: MGLStyleValue<NSNumber>(rawValue: 0.5),
    14: MGLStyleValue(rawValue: 2),
    18: MGLStyleValue(rawValue: 18),
]

layer.circleRadius = MGLStyleValue(interpolationMode: .exponential,
                                   cameraStops: stops,
                                   options: [.interpolationBase: 1.5])
```

Current syntax:

```swift
let stops = [
    12: 0.5,
    14: 2,
    18: 18,
]

layer.circleRadius = NSExpression(format: "mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'exponential', 1.5, %@)",
                                  stops)
```

### Interval

Steps, or intervals, create a range using the keys from the stops dictionary. The range is from the given key to just less than the next key. The attribute values that fall into that range are then styled using the layout or paint value assigned to that key. You can use the `+[NSExpression(MGLAdditions) mgl_expressionForSteppingExpression:fromExpression:stops:]` method or the custom function [`mgl_step:from:stops:`](predicates-and-expressions.html#code-mgl_step-from-stops-code) for cases where you previously used interval interpolation mode. The first parameter takes the feature attribute name and the second parameter (`from:`) optionally takes the default or fallback value for that function. The final parameter takes a stops dictionary as an argument.

When we use the stops dictionary given above with an `'mgl_step:from:stops:'`, we create ranges where earthquakes with a magnitude of 0 to just less than 2.5 would be yellow, 2.5 to just less than 5 would be orange, and so on.

Style function syntax:

```swift
let stops = [
    0: MGLStyleValue<UIColor>(rawValue: .yellow),
    2.5: MGLStyleValue(rawValue: .orange),
    5: MGLStyleValue(rawValue: .red),
    7.5: MGLStyleValue(rawValue: .blue),
    10: MGLStyleValue(rawValue: .white),
]

layer.circleColor = MGLStyleValue(interpolationMode: .interval,
                                  sourceStops: stops,
                                  attributeName: "mag",
                                  options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
````

Current syntax:

```swift
let stops: [NSNumber: UIColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]

layer.circleColor = NSExpression(format: "mgl_step:from:stops:(mag, %@, %@)",
                                 UIColor.green, stops)
```

### Categorical

Categorical interpolation mode took a stops dictionary. If the value for a specified feature attribute name matched one in that stops dictionary, the style value for that attribute value would be used. Categorical style functions can now be replaced with `MGL_MATCH`.

`MGL_MATCH` takes an initial condition, which in this case is an attribute key. This is followed by possible matches for that key and the value to assign to the layer property if there is a match. The final argument can be a default style value that is to be used if none of the specified values match.

There are three main types of events in the USGS dataset: earthquakes, explosions, and quarry blasts. In this case, the color of the circle layer will be determined by the type of event, with a default value of blue to catch any events that do not fall into any of those categories.

Style function syntax:

```swift
let categoricalStops = [
    "earthquake": MGLStyleValue<UIColor>(rawValue: .orange),
    "explosion": MGLStyleValue(rawValue: .red),
    "quarry blast": MGLStyleValue(rawValue: .yellow),
]

layer.circleColor = MGLStyleValue(interpolationMode: .categorical,
                                  sourceStops: categoricalStops,
                                  attributeName: "type",
                                  options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .blue)])
```

Current syntax:
```swift
let defaultColor = UIColor.blue
layer.circleColor = NSExpression(format: "MGL_MATCH(type, 'earthquake', %@, 'explosion', %@, 'quarry blast', %@, %@)",
    UIColor.orange, UIColor.red, UIColor.yellow, defaultColor)
```

If your use case does not require a default value, you can either apply a predicate to your layer prior to styling it, or use the format string `"valueForKeyPath:"`.

### Identity

Identity interpolation mode used the attribute’s value as the style layer property value. In this example, you might set the `circleRadius` to the earthquake’s magnitude. In order to use a feature attribute value to style a layer property, set the property value to `[NSExpression expressionForKeyPath:]`, which take the feature attribute name as an argument.

Style function syntax:

```swift
layer.circleRadius = MGLStyleValue(interpolationMode: .identity,
                                   sourceStops: nil,
                                   attributeName: "mag",
                                   options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 0)])
```

Current syntax:
```swift
layer.circleRadius = NSExpression(forKeyPath: "mag")
```

![identity mode](img/data-driven-styling/identity.png)

Some built-in functions can be applied to attribute values to style layer property values. To set the circle radius to three times the earthquake’s magnitude, create a `multiply:by:` function that takes the attribute value and the multiplier as arguments, or use a format string.

```swift
layer.circleRadius = NSExpression(forFunction: "multiply:by:", arguments: [NSExpression(forKeyPath: "mag"), 3])
```

![multiply magnitude](img/data-driven-styling/multiply.png)

You can also cast attribute values in order to use them. One example is to cast an integer as an `NSString` and use it as a text value.

```swift
let magnitudeLayer = MGLSymbolStyleLayer(identifier: "mag-layer", source: source)
magnitudeLayer.text = NSExpression(format: "CAST(mag, 'NSString')")
mapView.style?.addLayer(magnitudeLayer)
```

![cast a value](img/data-driven-styling/cast.png)

### Constant Values

For constant values that do not necessarily change based on camera or attribute values, use `[NSExpression expressionForConstantValue:]` (previously `[MGLStyleValue valueWithRawValue:]`).

## Resources

* [USGS Earthquake Feed](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php)
* [For Style Authors](for-style-authors.html)
* [Predicates and Expressions](predicates-and-expressions.html)
