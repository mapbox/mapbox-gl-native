# Adding Points to a Map

Mapbox offers a few different ways to add points to a map, each with different tradeoffs.

## MGLPointAnnotation

It’s straightforward to add an annotation to a map. You can use `MGLPointAnnotation` as is, or you can subclass it to add annotations with richer data.

```swift
let annotation = MGLPointAnnotation()
annotation.coordinate = CLLocationCoordinate2D(latitude: 45.5076, longitude: -122.6736)
annotation.title = "Bobby's Coffee"
annotation.subtitle = "Coffeeshop"
mapView.addAnnotation(annotation)
```

See the `MGLMapViewDelegate` method `-mapView:annotationCanShowCallout:` and similar methods for allowing interaction with a callout ([example](https://www.mapbox.com/ios-sdk/examples/callout-delegate/)).

## Displaying annotations

There are two basic ways to display the annotations you’ve added to a map, each with their own tradeoffs.

### Annotation Images (`MGLAnnotationImage`)

Annotation images are the quickest and most performant way to display annotations, but are also the most basic.

By default, annotations added to the map are displayed with a red pin ([example](https://www.mapbox.com/ios-sdk/examples/marker/)). To use custom images, you can implement `MGLMapViewDelegate` `-mapView:imageForAnnotation:` ([example](https://www.mapbox.com/ios-sdk/examples/marker-image/)).

**Pros**

* The easiest way to display a marker on a map
* Easily customizable with any `UIImage`
* High performance, as the images are rendered directly in OpenGL

**Cons**

* Annotation images are purely static and cannot be animated
* No control over z-ordering

### Annotation Views (`MGLAnnotationView`)

If you’re looking to add custom `UIView`s or have annotations that are dynamic or animatable, consider an `MGLAnnotationView` instead of an `MGLAnnotationImage` ([example](https://www.mapbox.com/ios-sdk/examples/annotation-views/)).

Annotation views have significant advantages over annotation images when you need every annotation to be unique. For example, annotation views are ideal for showing user locations on a map using high-resolution profile pictures.

To use annotation views, implement `MGLMapViewDelegate` `-mapView:viewForAnnotation` and provide a custom `MGLAnnotationView` (`UIView`) subclass.

**Pros**

* Custom, native UIViews
* No limit on style or image size
* Full support for animations
* Relative control over z-ordering using the `zPosition` property on `CALayer`
* Familiar API for MapKit users

**Cons**

* Performance implications:
    * `UIView`s are inherently slow to render compared to OpenGL, more apparent if you’re adding many views or moving the map rapidly
    * In some cases, you might consider runtime styling

## Advanced: Runtime Styling

For absolute full control of how points are displayed on a map, consider [runtime styling](runtime-styling.html).

You can use `MGLPointFeature` or any other [style primitives](Style%20Primitives.html) to add points and shapes to an `MGLShapeSource`.

From there, you can create one or many `MGLSymbolStyleLayer` or `MGLCircleStyleLayer` layers to filter and style points for display on the map ([example](https://www.mapbox.com/ios-sdk/examples/runtime-multiple-annotations)).

**Pros**

* Most powerful option
* Highest performance (rendered in GL directly)
* SDK-level support for labels rendered together with icons
* Finer control of z-ordering
    * Rendering respects ordering within the data source
    * Otherwise layers are lightweight so you can create a new layer for each level you need

**Cons**

* Currently you must implement your own tap gesture recognizer together with `MGLMapView.visibleFeaturesAtPoint` to recognize taps and manually show callouts ([example](https://www.mapbox.com/ios-sdk/examples/select-layer/)).
* Currently no SDK support for animations. If you need animations, consider using an NSTimer and updating the layer properties accordingly.
