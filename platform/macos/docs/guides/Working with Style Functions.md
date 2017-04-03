
<!--
  This file is generated.
  Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
-->

# Working with Style Functions

Mapbox’s runtime and data-driven styling features allow you to style your maps as a function of the current map zoom level, attributes in the map source data, or both. With data-driven styling, you can style map features automatically based on their individual attributes.

Vary POI icons, transit route line colors, city polygon opacity, and more based on any attribute in your data. Need to visualize hotel data by price? You can have your map’s point radii and colors change automatically with your data.

![available bikes](img/data-driven-styling/citibikes.png) ![subway lines](img/data-driven-styling/polylineExample.png)

This guide uses earthquake data from the [U.S. Geological Survey](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php) and data-driven styling to style a map based on attributes. For more information about how to work with GeoJSON data in our iOS SDK, please see our [working with GeoJSON data](working-with-geojson-data.html) guide.

## Style functions

There are three subclasses of `MGLStyleFunction`:

* `MGLCameraStyleFunction` is a style value that changes with zoom level. For example, you can make the radius of a circle increase according to zoom level.
* `MGLSourceStyleFunction` is a style value that changes with the attributes of a feature. For example, you can adjust the radius of a circle based on the magnitude of an earthquake.
* `MGLCompositeStyleFunction` is a style value that changes with both zoom level and attribute values. For example, you can add a circle layer where each circle has a radius based on both zoom level and the magnitude of an earthquake.

The documentation for each individual style layer property notes which style functions are enabled for that property.

## Stops

Stops are key-value pairs that that determine a style value. With a `MGLCameraSourceFunction` stop, you can use a dictionary with a zoom level for a key and a `MGLStyleValue` for the value. For example, you can use a stops dictionary with zoom levels 0, 10, and 20 as keys, and yellow, orange, and red as the values. A `MGLSourceStyleFunction` uses the relevant attribute value as the key.

```swift
let stops = [0: MGLStyleValue<NSColor>(rawValue: .yellow),
             2.5: MGLStyleValue(rawValue: .orange),
             5: MGLStyleValue(rawValue: .red),
             7.5: MGLStyleValue(rawValue: .blue),
             10: MGLStyleValue(rawValue: .white)]
```

## Interpolation mode

The effect a key has on the style value is determined by the interpolation mode. There are four interpolation modes that can be used with a source style function: exponential, interval, categorical, and identity. You can also use exponential and interval interpolation modes with a camera style function.

### Linear

`MGLInterpolationModeExponential` interpolates linearly or exponentially between style function stop values. By default, the `MGLStyleFunction` options parameter `MGLStyleFunctionOptionInterpolationBase` equals `1`, which represents linear interpolation and doesn’t need to be included in the options dictionary.

The stops dictionary below, for example, shows colors that continuously shift from yellow to orange to red to blue to white based on the attribute value.

``` swift
let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")
let symbolSource = MGLSource(identifier: "source")
let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
style.addSource(source)

let stops = [0: MGLStyleValue<NSColor>(rawValue: .yellow),
             2.5: MGLStyleValue(rawValue: .orange),
             5: MGLStyleValue(rawValue: .red),
             7.5: MGLStyleValue(rawValue: .blue),
             10: MGLStyleValue(rawValue: .white)]

let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
layer.circleColor = MGLStyleValue(interpolationMode: .exponential,
                                  sourceStops: stops,
                                  attributeName: "mag",
                                  options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .green)])
layer.circleRadius = MGLStyleValue(rawValue: 10)
style.insertLayer(layer, below: symbolLayer)
```

![exponential mode](img/data-driven-styling/exponential.png)

### Exponential

By combining `MGLInterpolationModeExponential` with an `MGLStyleFunctionOptionInterpolationBase` greater than `0` (other than `1`), you can interpolate between values exponentially, create an accelerated ramp effect.

Here’s a visualization from Mapbox Studio (see [Working with Mapbox Studio](working-with-mapbox-studio.html)) comparing interpolation base values of `1.5` and `0.5` based on zoom.

<img src="img/data-driven-styling/exponential-function.png" height=344/>
<img src="img/data-driven-styling/exponential-function-1.png" height=344/>

The example below increases a layer’s `circleRadius` exponentially based on a map’s zoom level. The `MGLStyleFunctionOptionInterpolationBase` is `1.5`.

```swift
let stops = [12: MGLStyleValue(rawValue: 0.5),
             14: MGLStyleValue(rawValue: 2),
             18: MGLStyleValue(rawValue: 18)]

layer.circleRadius = MGLStyleValue(interpolationMode: .exponential,
                                  cameraStops: stops,
                                  options: [.interpolationBase: 1.5])
```

### Interval

`MGLInterpolationModeInterval` creates a range using the keys from the stops dictionary. The range is from the given key to just less than the next key. The attribute values that fall into that range are then styled using the style value assigned to that key.

When we use the stops dictionary given above with an interval interpolation mode, we create ranges where earthquakes with a magnitude of 0 to just less than 2.5 would be yellow, 2.5 to just less than 5 would be orange, and so on.

``` swift
let stops = [0: MGLStyleValue<NSColor>(rawValue: .yellow),
             2.5: MGLStyleValue(rawValue: .orange),
             5: MGLStyleValue(rawValue: .red),
             7.5: MGLStyleValue(rawValue: .blue),
             10: MGLStyleValue(rawValue: .white)]

layer.circleColor = MGLStyleValue(interpolationMode: .interval,
                                  sourceStops: stops,
                                  attributeName: "mag",
                                  options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .green)])
```

![interval mode](img/data-driven-styling/interval.png)

### Categorical

At each stop, `MGLInterpolationModeCategorical` produces an output value equal to the function input. We’re going to use a different stops dictionary than we did for the previous two modes.

There are three main types of events in the dataset: earthquakes, explosions, and quarry blasts. In this case, the color of the circle layer will be determined by the type of event, with a default value of green to catch any events that do not fall into any of those categories.

``` swift
let categoricalStops = ["earthquake": MGLStyleValue<NSColor>(rawValue: .orange),
                        "explosion": MGLStyleValue(rawValue: .red),
                        "quarry blast": MGLStyleValue(rawValue: .yellow)]

layer.circleColor = MGLStyleValue(interpolationMode: .categorical,
                                  sourceStops: categoricalStops,
                                  attributeName: "type",
                                  options: [.defaultValue: MGLStyleValue<NSColor>(rawValue: .blue)])

```

![categorical mode](img/data-driven-styling/categorical1.png) ![categorical mode](img/data-driven-styling/categorical2.png)

### Identity

`MGLInterpolationModeIdentity` uses the attribute’s value as the style value. For example, you can set the `circleRadius` to the earthquake’s magnitude. Since the attribute value itself will be used as the style value, `sourceStops` should be set to `nil`.

``` swift
layer.circleRadius = MGLStyleValue(interpolationMode: .identity,
                                   sourceStops: nil,
                                   attributeName: "mag",
                                   options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 0)])

```

![identity mode](img/data-driven-styling/identity.png)

##Resources

* [USGS](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php)
* [For Style Authors](for-style-authors.html)
