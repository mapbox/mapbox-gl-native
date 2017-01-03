# Adding Points to a Map

Mapbox offers a few different ways to add points to a map, each with different tradeoffs.

## MGLPointAnnotation

It's straightforward to add an annotation to a map. You can use `MGLPointAnnotation` as is, or you can subclass it to add annotations with richer data.

```swift
let annotation = MGLPointAnnotation()
annotation.coordinate = CLLocationCoordinate2D(latitude: 45.5076, longitude: -122.6736)
annotation.title = "Bobby's Coffee"
annotation.subtitle = "Coffeeshop"
mapView.addAnnotation(annotation)
```

See the `MGLMapViewDelegate` method `-mapView:annotationCanShowCallout:` and similar methods for allowing interaction with a callout.

## Displaying annotations

There are two basic ways to display the annotations you've added to a map, each with their own tradeoffs.

### Annotation Images (`MGLAnnotationImage`)

Annotation images are the quickest and most performant ways to display annotations, and you can provide your own custom annotation images ([example](https://www.mapbox.com/ios-sdk/examples/marker-image/)).

By default, annotations added to the map are displayed with a red pin. If you'd like to customize this annotation, you can implement `MGLMapViewDelegate` `-mapView:imageForAnnotation:`.

**Pros**

* The easiest way to display a marker on a map
* Easily customizable with any `UIImage`
* High performance as the images are rendered directly in OpenGL

**Cons**

* Annotation images are purely static
* No control over z-ordering
* Limits to the size and number of images you can add

### Annotation Views (`MGLAnnotationView`)

If you're looking to add custom UIViews or have annotations that are dynamic or animatable, consider an `MGLAnnotationView` instead of an `MGLAnnotationImage` ([example](https://www.mapbox.com/ios-sdk/examples/annotation-views/)).

Annotation views are a big advantage over annotation images when you need every annotation to be unique. For example, annotation views are ideal if as an example you want to show user locations on a map using their profile pictures.

To use annotation views, you'll need to implement `MGLMapViewDelegate` `-mapView:viewForAnnotation` and provide a custom `MGLAnnotationView` (`UIView`) subclass.

**Pros**

* Custom, native UIViews
* No limit on style or image size
* Full support for animations
* Familiar API to MapKit
* Relative control over z-ordering using the `zPosition` property on `CALayer`

**Cons**

* Performance implications:
    * UIViews are inherently slow to render compared to OpenGL, more apparent if you're adding many views or moving the map rapidly
    * In some cases, you might consider runtime styling

## Advanced: Runtime Styling

If you're looking for absolute full control of how points are displayed on a map, consider [runtime styling](runtime-styling.html).

You can use `MGLPointFeature` or any of the other [style feature subclasses](Style%20Features.html) to add points and shapes to an `MGLShapeSource`.

From there, you can create one or many `MGLSymbolStyleLayer` or `MGLCircleStyleLayer` layers to filter and style points for display on the map ([example](https://www.mapbox.com/ios-sdk/examples/runtime-multiple-annotations)).

**Pros**

* Most powerful option
* Highest performance (rendered in GL directly)
* SDK-level support for labels rendered together with icons
* Finer control of z-ordering
    * Rendering respects ordering within the data source
    * Otherwise layers are lightweight so you can create a new layer for each level you need

**Cons**

* Currently you must implement your own tap gesture recognizer together with `MGLMapView.visibleFeaturesAtPoint` to recognize taps and manually show callouts ([example](https://www.mapbox.com/ios-sdk/examples/select-feature)).
* Currently no SDK support for animations. If you need animations, consider using an NSTimer and updating the layer properties accordingly.

