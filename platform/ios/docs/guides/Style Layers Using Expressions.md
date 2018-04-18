<!--
  This file is generated.
  Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
-->

# Styling Layers Using Expressions

[Runtime Styling](runtime-styling.html) enables you to modify every aspect of the map’s appearance dynamically as a user interacts with your application. Much of the runtime styling API allows you to specify *non-constant expressions*. A style expression allows users to specify in advance how a layout or paint attribute will vary as the zoom level changes or how the appearance of individual features vary based on metadata provided by a content source.

Runtime styling removes the inconvenience of manually calculating intermediate values between different zoom levels or creating a multitude of style layers to handle homogeneous features in the map content. For example, if your content source indicates the prices of hotels in an area, you can color-code the hotels by price, relying on a data expression to smoothly interpolate among desired colors without having to specify the color for each exact price.
*Data-driven styling* specifically refers to the use of non-constant expressions to vary the map’s appearance based on data in a content source.
You can also specify expressions in a style JSON file, to be applied automatically when the map loads. See the [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#types-function) for details.

This guide uses earthquake data from the [U.S. Geological Survey](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php) and data-driven styling to style a map based on attributes. For more information about how to work with GeoJSON data in our iOS SDK, please see our [working with GeoJSON data](working-with-geojson-data.html) guide. To learn more about supported expressions functions, see our [Predicates and Expressions](predicates-and-expressions.html) guide. The Predicates and Expressions guide also outlines Mapbox custom functions that can be used to dynamically style a map.

An expression is represented at runtime by the `NSExpression` class. There are several types of expressions that can be used to determined paint and layout values.

- A *constant expression* assigns a set value for that style property. For example, the opacity of a fill style layer can be set to a constant value between 0 and 1.
- A *camera expression* changes a style property with zoom level. For example, you can make the radius of a circle increase according to zoom level. These changes occur either through interpolation or step functions applied to the `$zoom` value.
- A *data expression* changes a style property with the attributes of a feature. For example, you can adjust the radius of a circle based on the magnitude of an earthquake.
- A *composite expression* changes a style property with both zoom level and attribute values. For example, you can add a circle layer where each circle has a radius based on both zoom level and the magnitude of an earthquake.

The documentation for each individual style layer property notes which non-constant expressions are enabled for that property.

## Stops

tops are dictionary keys that are associated with layer attribute values. You can use feature attribute values, zoom levels, and heatmap point density as keys, with an constant value or expression for the value. For example, you can use a stop dictionary with the zoom levels 0, 10, and 20 as keys and the colors yellow, orange, and red as the values.
```swift
let stops: [NSNumber: UIColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]
```

Matched values can be used in the place of stops dictionaries with categorical interpolation. The `MGL_MATCH()` custom function creates feature and layer attribute value pairs.
## Interpolation mode

The effect a key has on the layer style value is determined by the interpolation mode. There are four interpolation modes that can be used with a style expression: exponential/linear, interval, categorical, and identity. You can also use exponential/linear and categorical interpolation modes with a camera expression.
### Linear

`MGLExpressionInterpolationModeLinear` interpolates linearly between stop values.
The stops dictionary below, for example, shows colors that continuously shift from yellow to orange to red to blue to white based on the attribute value.
```swift
let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")!
let symbolSource = MGLSource(identifier: "source")
let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
mapView.style?.addSource(source)

let stops: [Float: UIColor] = [
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

![exponential mode](img/data-driven-styling/linear.png)

### Exponential

By combining `MGLExpressionInterpolationModeExponential` with an interpolation base greater than `0` (other than `1`), you can interpolate between values exponentially, create an accelerated ramp effect.
Here’s a visualization from Mapbox Studio (see [Working with Mapbox Studio](working-with-mapbox-studio.html)) comparing interpolation base values of `1.5` and `0.5` based on zoom.

<img src="img/data-driven-styling/exponential-function.png" height=344/>
<img src="img/data-driven-styling/exponential-function-1.png" height=344/>

The example below increases a layer’s `circleRadius` exponentially based on a map’s zoom level. The interpolation base is `1.5`.
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

Steps, or intervals, create a range using the keys from the stops dictionary. The range is from the given key to just less than the next key. The attribute values that fall into that range are then styled using the layout or paint value assigned to that key.

When we use the stops dictionary given above with an interval interpolation mode, we create ranges where earthquakes with a magnitude of 0 to just less than 2.5 would be yellow, 2.5 to just less than 5 would be orange, and so on.
```swift
let stops: [Float: UIColor] = [
    0: .yellow,
    2.5: .orange,
    5: .red,
    7.5: .blue,
    10: .white,
]

layer.circleColor = NSExpression(format: "mgl_step:from:stops:(mag, %@, %@)",
                                 UIColor.green, stops)
```

![interval mode](img/data-driven-styling/interval.png)

### Categorical

At each stop, a categorical, or match expression, produces an output value equal to the function input. We’re going to match different feature attribute and style layer property values than we did for the previous two modes.

There are three main types of events in the USGS dataset: earthquakes, explosions, and quarry blasts. In this case, the color of the circle layer will be determined by the type of event, with a default value of blue to catch any events that do not fall into any of those categories.

```swift
let defaultColor = UIColor.blue
layer.circleColor = NSExpression(format: "MGL_MATCH(type, 'earthquake', %@, 'explosion', %@, 'quarry blast', %@, %@)",
    UIColor.orange, UIColor.red, UIColor.yellow, defaultColor)
```

`MGL_MATCH` takes the attribute key, followed by possible attribute and style value pairs. The final argument is a default style value.

![categorical mode](img/data-driven-styling/categorical1.png) ![categorical mode](img/data-driven-styling/categorical2.png)

### Identity

Identity interpolation mode uses the attribute’s value as the style layer property value. For example, you can set the `circleRadius` to the earthquake’s magnitude. Since the attribute value itself will be used as the style value, `sourceStops` should be set to `nil`.

```swift
layer.circleRadius = NSExpression(forKeyPath: "mag")
```

![identity mode](img/data-driven-styling/identity.png)

Some built-in expressions functions can be applied to attribute values to style layer property values. To set the circle radius to 3 times the earthquake’s magnitude, create a `multiply:by` function that takes the attribute value and the multiplier as arguments.

```swift
layer.circleRadius = NSExpression(forFunction: "multiply:by:", arguments: [NSExpression(forKeyPath: "mag"), 3])
```

![multiply magnitude](img/data-driven-styling/multiply.png)

Through expressions, you can also cast attribute values in order to use them. One example is to cast an integer as an `NSString` and use it as a text value.

```swift
let magnitudeLayer = MGLSymbolStyleLayer(identifier: "mag-layer", source: source)
magnitudeLayer.text = NSExpression(format: "CAST(mag, 'NSString')")
mapView.style?.addLayer(magnitudeLayer)
```

![cast a value](img/data-driven-styling/cast.png)


  ##Migrating from Style Functions to Expressions

  For constant values that do not necessarily change based on camera or attribute values, use `[NSExpression expressionForConstantValue:]` (previously `[MGLStyleValue valueWithRawValue:]`). Expressions support format string syntax, as shown above, as well as convenience methods that include those listed below.


<table>
<thead>
<tr><th>Interpolation mode</th><th>Syntax prior to v4.0.0</th><th>Current syntax</th></tr>
</thead>
<tbody>
<tr>
  <td>Linear </td>
  <td><code>MGLStyleValue(interpolationMode: .exponential,
                                        sourceStops: stops,
                                        attributeName: "mag",
                                        options: nil) </code></td>
  <td><code>NSExpression(forMGLInterpolating: NSExpression(forKeyPath: "mag"),
                  curveType: MGLExpressionInterpolationMode.linear,
                  parameters: nil,
                  stops: NSExpression(forConstantValue: stops)) </code></td>
</tr>
<tr>
  <td>Exponential</td>
  <td><code>MGLStyleValue(interpolationMode: .exponential,
                                        sourceStops: stops,
                                        attributeName: “mag”,
                                         options:  [.interpolationBase: 1.5])</code></td>
  <td><code>layer.circleRadius = NSExpression(format: "FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'exponential', 1.5, %@)",
                                          stops)</code></td>
</tr>
<tr>
  <td>Interval</td>
  <td><code>MGLStyleValue(interpolationMode: .interval,
                                        sourceStops: [0: UIColor.yellow, 2.5: UIColor.orange],
                                        attributeName: "mag",
                                        options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])</code></td>
  <td><code>NSExpression(forMGLStepping: NSExpression(forKeyPath: "mag"),
                                   from: NSExpression(forConstantValue: UIColor.green),
                                  stops: [0: UIColor.yellow, 2.5: UIColor.orange])</code></td>
</tr>
<tr>
  <td>Categorical</td>
  <td><code>MGLStyleValue(interpolationMode: *.*categorical,
                                    sourceStops: [featureValue : UIColor.green],
                                    attributeName: "type",
                                    options: [*.*defaultValue: MGLStyleValue*<*UIColor*>*(rawValue: *.*blue)])</code></td>
  <td><code>NSExpression(format: “MGL_MATCH(type, ‘earthquake’, %@,  %@)”, UIColor.green, UIColor.blue) </code></td>
</tr>
<tr>
  <td>Identity</td>
  <td><code>MGLStyleValue(interpolationMode: .identity,
                                     sourceStops: nil,
                                     attributeName: "mag",
                                     options: nil)</code></td>
  <td><code>NSExpression(forConstantValue: “mag”)</code></td>
</tbody>
</table>

  In order to convert camera style functions, use `$zoomLevel` or `MGL_FUNCTION('zoom')` as the attribute key.

  ##Resources

* [USGS](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php)
* [For Style Authors](for-style-authors.html)
* [Predicates and Expressions](predicates-and-expressions.html)
