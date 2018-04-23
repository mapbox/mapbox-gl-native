# Adding Markers to a Map

Mapbox offers a few different ways to add markers to a map, each with different tradeoffs. Below is an overview of the variety of approaches that can be used.

## **Annotations API**

Our annotations API includes the `MGLAnnotationImage`, and `MGLAnnotationView` classes. These are most similar to MapKit’s annotation class and provide a familiar interface for working with markers and callouts.

| <img src="img/adding-points-to-a-map/annotation-image.png" alt="MGLAnnotationImage" style="height: 500px;"/>               | <img src="img/adding-points-to-a-map/annotation-view.png" alt="MGLAnnotationView" style="height: 500px;"/>              |
|----------------------|---------------------|
| `MGLAnnotationImage` | `MGLAnnotationView` |

**MGLAnnotationImage** is an annotation class that is easily customizable with any `UIImage`.
It is highly performant, as the images are rendered directly using OpenGL. However, if you need to animate your annotations or control z-layer ordering, consider working with **MGLAnnotationView** which supports any animation that can be applied to a `UIView`. View hierarchy can be manipulated by using instance methods available on `UIView` such as `-[UIView bringSubviewToFront:]`.

**MGLAnnotationView** is an annotation class that is an easily customizable `UIView`. Use this class if you need your markers to be dynamic or animated. `MGLAnnotationView` has a significant advantage over `MGLAnnotationImage` when you need every annotation to be unique. For example, annotation views are ideal for showing user locations on a map using high-resolution profile pictures. However, the map can slow down when many annotation views are visible at the same time, so if you need to add a very large number of markers, consider using our runtime styling APIs instead.

Both `MGLAnnotationImage` and `MGLAnnotationView` can become interactive by adding a [few lines of code](https://www.mapbox.com/ios-sdk/examples/marker/). When the user taps an annotation, the annotation’s name appears in a basic callout. An annotation view can additionally respond to [drag-and-drop gestures](https://www.mapbox.com/ios-sdk/examples/draggable-views/).

## **Runtime styling API**

For full control of how markers are displayed on a map, consider using our [runtime styling](runtime-styling.html) APIs. Like `MGLAnnotationImage`, it is a performant approach to adding markers because they rendered directly using OpenGL. However, the runtime styling APIs also provide support for rendering labels together with icons, finer control of z-ordering, and clustering, so consider using this set of APIs if you need to display a large amount of highly customizable markers.

Our runtime styling API is the most powerful option if you need to create rich data visualizations within in your map, but it is the most complex and has a steeper learning curve than our annotations API.

The runtime styling API includes our `MGLSymbolStyleLayer` and `MGLCircleStyleLayer` classes that can be used to dynamically display on markers on map when used in conjunction with either an `MGLVectorSource` or an `MGLShapeSource`.

If you need to implement callouts with the `MGLSymbolStyleLayer` or `MGLCircleStyleLayer`, you will need to implement your own tap gesture recognizer that calls `-[MGLMapView visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:]` to get the tapped point feature, then show a `UIView` you provide. Additionally, if you need to animate markers when using the runtime styling APIs, consider using a timer to update the source data coordinates accordingly.

| <img src="img/adding-points-to-a-map/circle-layer.png" alt="MGLCircleStyleLayer" style="height: 500px;"/>               | <img src="img/adding-points-to-a-map/symbol-layer.png" alt="MGLSymbolStyleLayer" style="height: 500px;"/>              |
|----------------------|---------------------|
| `MGLCircleStyleLayer` | `MGLSymbolStyleLayer` |

The **MGLCircleStyleLayer** class is the style layer class responsible for displaying the source’s point features as circle-shaped markers. You can specify circle fill and outline colors, as well as size. You can also dynamically change the circle’s styling properties based on any attributes your source data contains.

The **MGLSymbolStyleLayer** class is the style layer class responsible for displaying the source’s point features as icons and labels. You can use custom images as icons and also combine text labels, placing them exactly where you specify. You can also dynamically change the symbol’s styling properties based on any attributes your source data contains.

Still undecided on which approach will work best for your use case? [Reach out to our support team](https://www.mapbox.com/contact/).

See the table below for a summary of APIs that can be used to add markers to a map:

✅ Recommended

⚠️ Supported with caveats

➖ Unavailable or not supported


| Feature                                            | MGLAnnotationView                                                  | MGLAnnotationImage                                       | MGLSymbolStyleLayer                                                                                                     | MGLCircleStyleLayer                                                                                                     |
|----------------------------------------------------|--------------------------------------------------------------------|----------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|
| Customizability                                    | ✅ Text labels, interactive subviews                                | ⚠️ Static images only                                     |  ✅ Full support for text labels and label placement                                                                     |  ✅Customize circle color and outline                                                                                    |
| Borrows familiar concepts from                     | MapKit, Google Maps SDK                                            | ➖                                                        | Mapbox GL JS, Mapbox Studio                                                                                             | Mapbox GL JS, Mapbox Studio                                                                                             |
| Can use images                                     | ✅                                                                  | ✅                                                        | ✅                                                                                                                       | ➖                                                                                                                       |
| Can use text                                       | ✅                                                                  | ➖                                                        | ✅                                                                                                                       | ➖                                                                                                                       |
| Control Z-index                                    | ✅                                                                  | ➖                                                        | ⚠️ Add multiple layers at, above, or below a specified layer index to control ordering                                   | ⚠️ Add multiple layers at, above, or below a specified layer index to control ordering                                   |
| Drag and drop                                      | ✅                                                                  | ➖                                                        | ➖                                                                                                                       | ➖                                                                                                                       |
| Core Animation support                             | ✅                                                                  | ➖                                                        | ➖                                                                                                                       | ➖                                                                                                                       |
| Add/move/replace data                              | ✅                                                                  | ✅                                                        | ⚠️ Partial data updates are less performant than using annotations                                                       | ⚠️ Partial data updates are less performant than using annotations                                                       |
| SceneKit support                                   | ✅                                                                  | ➖                                                        | ➖                                                                                                                       | ➖                                                                                                                       |
| Can be dynamically styled based on data attributes |  ✅ Subclass `MGLPointAnnotation` to add custom attributes          | ✅ Subclass `MGLPointAnnotation` to add custom attributes | ✅                                                                                                                       | ✅                                                                                                                       |
| Supports callouts                                  |  ✅ Built-in callouts included                                      | ✅ Built-in callouts included                             | ⚠️ Implement your own gesture recognizer that uses feature querying, then create custom UIViews to mimic native callouts | ⚠️ Implement your own gesture recognizer that uses feature querying, then create custom UIViews to mimic native callouts |
| Supports clustering                                | ⚠️ Use a [third-party plugin](https://github.com/hulab/ClusterKit/) | ➖                                                        | ✅                                                                                                                       | ✅                                                                                                                       |
