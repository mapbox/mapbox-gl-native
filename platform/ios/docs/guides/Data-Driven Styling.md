<!---
To do - JK:
- [ ] Fill it out with more detailed explanations
- [ ] Add examples for camera or composite style functions
- [ ] Cluster earthquakes as an example for camera function?
- [ ] Clean up & optimize screenshots - replace with screenshots showing circles below symbol layer
- [ ] Link API docs
- [ ] Add cocoaprefix so this can be moved to darwin
---->



##Data-Driven Styling

Mapbox’s data-driven styling features allow you to use data properties to style your maps. You can style objects within the same layer differently based on their individual attributes. This enables you to style icons, routes, parks, and more based on attributes.

![available bikes](img/data-driven-styling/citibikes.png)

_Insert cool overview of things you can do! Try to include polylines & polygons_

### How to use Data-Driven Styling
This guide uses earthquake data from the [U.S. Geological Survey](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php) to style a map based on attributes. For more information about how to work with GeoJSON data in our iOS SDK, please see our [working with GeoJSON data](https://www.mapbox.com/ios-sdk/api/3.5.0-beta.1/working-with-geojson-data.html) guide.

`MGLStyleFunction`

There are three subclasses of `MGLStyleFunction`:

* `MGLCameraStyleFunction` - For a style value that changes with zoom level. For example, you can make the radius of a circle to increase based on zoom level. In iOS SDK v3.4, a this was a `MGLStyleFunction`.
* `MGLSourceStyleFunction` - For a style value that changes with the attributes of a feature. You can adjust the radius of a circle based on the magnitude of an earthquake, for example.
* `MGLCompositeStyleFunction` - For a style value that changes with both zoom level and attribute values. For example, you can add a circle layer where each circle has a radius based on both zoom level and the magnitude of an earthquake.

####Stops

Stops are key-value pairs that that determine a style value. With a `MGLCameraSourceFunction` stop, you can use a dictionary with a zoom level for a key and a `MGLStyleValue` for the value. For example, you can use a stops dictionary with zoom levels 0, 10, and 20 as keys, and yellow, orange, and red as the values. A `MGLSourceStyleFunction` uses the relevant attribute value as the key.

####`MGLInterpolationMode`

The effect a key has on the style value is determined by the interpolation mode. There are four interpolation modes that can be used with a source style function: exponential, interval, categorical, and identity. You can also use exponential and interval interpolation modes with a camera style function.

#####Exponential

`MGLInterpolationModelExponential` creates a linear effect based on the values. The key value is the base for interpolation, and the style value is based on where an attribute value falls between two keys.

The stops dictionary below, for example, shows colors that continuously shift from yellow to orange to red to blue to white based on the attribute value.

``` swift
    let url = URL(string: "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_week.geojson")
    let symbolSource = MGLSource(identifier: "source")
    let symbolLayer = MGLSymbolStyleLayer(identifier: "place-city-sm", source: symbolSource)

    let source = MGLShapeSource(identifier: "earthquakes", url: url, options: nil)
    style.addSource(source)

    let stops = [0 : MGLStyleValue(rawValue: UIColor.yellow),
                 2.5 : MGLStyleValue(rawValue: UIColor.orange),
                 5: MGLStyleValue(rawValue: UIColor.red),
                 7.5 : MGLStyleValue(rawValue: UIColor.blue),
                 10 : MGLStyleValue(rawValue: UIColor.white)]

    let layer = MGLCircleStyleLayer(identifier: "circles", source: source)
    layer.circleColor = MGLStyleValue<UIColor>(interpolationMode: .exponential,
                                               sourceStops: stops,
                                               attributeName: "mag",
                                               options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
    layer.circleRadius = MGLStyleValue(rawValue: 10)
    style.insertLayer(layer, below: symbolLayer)
```

![exponential mode](img/data-driven-styling/exponential.png)

#####Interval

`MGLInterpolationModeInterval` creates a range using the keys stops dictionary. The range is from the given key to just less than the next key. The attribute values that fall into that range are then styled using the style value assigned to that key.

When we use the stops dictionary given above with an interval interpolation mode, we create ranges where earthquakes with a magnitude of 0 to just less than 2.5 would be yellow, 2.5 to just less than 5 would be orange, and so on.

``` swift
layer.circleColor = MGLStyleValue<UIColor>(interpolationMode: .interval,
                                                          sourceStops: stops,
                                                          attributeName: "mag",
                                                          options: [.defaultValue: MGLStyleValue<UIColor>(rawValue: .green)])
```
![interval mode](img/data-driven-styling/interval.png)

#####Categorical

Returns the output value that is equal to the stop for the function input. We’re going to use a different stops dictionary than we did for the previous two modes.

There are three main types of events in the dataset: earthquakes, explosions, and quarry blasts. In this case, the color of the circle layer will be determined by the type of event, with a default value of green to catch any events that do not fall into any of those categories.

``` swift
let categoricalStops = ["earthquake" : MGLStyleValue(rawValue: UIColor.orange),
                                    "explosion" : MGLStyleValue(rawValue: UIColor.red),
                                    "quarry blast": MGLStyleValue(rawValue: UIColor.yellow)]

layer.circleColor = MGLStyleValue<UIColor>(interpolationMode: .categorical,
                                                          sourceStops: categoricalStops,
                                                          attributeName: "type",
                                                          options: [.defaultValue: MGLStyleValue(rawValue: UIColor.blue)])

```

<img width="320" alt="screenshot 2017-03-01 13 05 28" src="https://cloud.githubusercontent.com/assets/12474734/23481308/d7c4d644-fe7f-11e6-9e71-11a6ecb89775.png"> <img width="320" alt="screenshot 2017-03-01 13 06 00" src="https://cloud.githubusercontent.com/assets/12474734/23481311/d90ca586-fe7f-11e6-87b8-e63cdeb1db84.png">

#####Identity

`MGLInterpolationModeIdentity` uses the attribute’s value as the style value. For example, you can set the `circleRadius` to the earthquake’s magnitude. Since the attribute value itself will be used as the style value, `sourceStops` can be set to `nil`.

``` swift
layer.circleRadius = MGLStyleFunction<NSNumber>(interpolationMode: .identity,
                                                                 sourceStops: nil,
                                                                 attributeName: "mag",
                                                                 options: [.defaultValue: MGLStyleValue<NSNumber>(rawValue: 0)])

```

<img width="320" alt="screenshot 2017-02-28 09 37 28" src="https://cloud.githubusercontent.com/assets/12474734/23444533/97591ff6-fdea-11e6-9d87-66ae1f205d22.png">

_Insert Conclusion_



Resources:
[USGS](https://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php)
