# User Interactions in the Mapbox iOS SDK

The Mapbox iOS SDK provides a set of built-in gesture recognizers to developers. These interactions can be customized to fit your use case. You see what gesture recognizers are on your `MGLMapView` by accessing the `gestureRecognizers` property on your map.

## Configuring user interaction

These properties are built in ways to enable or disable a set of gesture recognizers. Boolean values are set to `true` by default.

- `zoomEnabled` - Allows the user to zoom in and out by pinching two fingers, double tapping, or using "quick zoom". Accepts Boolean values.
- `scrollEnabled` - Allows the user to scroll by dragging or swiping one finger. Accepts Boolean values.
- `rotateEnabled` - Allows the user to rotate by moving two fingers in a circular motion. Accepts Boolean values.
- `pitchEnabled` - Allows the user to tilt the map by vertically dragging two fingers. Accepts Boolean values.
- `decelerationRate` - Determines the rate of deceleration after the user lifts their finger. You can set the value using the  `MGLMapViewDecelerationRateNormal`, `MGLMapViewDecelerationRateFast`, or `MGLMapViewDecelerationRateImmediate` constants.

## Individual gestures

|Gesture | name | Description | Related Property |
|:-------:|:---------|----------------| -----------|
|Pinch    | `handlePinchGesture:` |Zooms in or out on the map's anchor point | `zoomEnabled` |
|Rotation | `handleRotateGesture:` | Changes the MGLMapView direction based on the user rotating two fingers in a circular motion | `rotateEnabled` |
|Single Tap |`handleSingleTapGesture:`  | Selects/Deselects the annotation that you tap. | |
|Double Tap |`handleDoubleTapGesture:` | Zooms in on the map's anchor point | `zoomEnabled` |
|Two Finger Tap:| `handleTwoFingerTapGesture:` | Zooms out with the map's anchor point centered | `zoomEnabled` |
|Pan Gesture| `handlePanGesture:`| Scrolls across mapView (_Note: If_ `MGLUserTrackingModeFollow` _is being used, it will be disabled once the user pans_)| `scrollEnabled` |
|Two Finger Drag | `handleTwoFingerDragGesture:` | Adjusts the pitch of the `MGLMapView` | `pitchEnabled` |
|"Quick Zoom" |`handleQuickZoomGesture:` |Tap twice. On second tap, hold your finger on the map and pan up to zoom in, or down to zoom out | `zoomEnabled`|

![quick zoom](img/user-interaction/quickzoom.gif) ![rotation](img/user-interaction/RotateSydney.gif)

## Adding custom gesture recognizers

You can add `UIGestureRecognizers` to your map programmatically or via storyboard. Adding custom responses to gesture recognizers can enhance your user's experience, but try to use standard gestures where possible.

The gesture recognizers that you add will take priority over the built-in gesture recognizer. You can also set up your own gesture recognizer to work simultaneously with built-in gesture recognizers by using `-gestureRecognizer:shouldRecognizeSimultaneouslyWithGestureRecognizer:`, allowing you to enhance already existing gesture recognizers.

You can also add gesture recognizers that are only called when the default gesture recognizer fails, such as when a user taps on a part of the map that is not an annotation. For example:

`let gestureRecognizers = mapView.gestureRecognizers
let singleTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(myCustomFunction))
for recognizer in gestureRecognizers where recognizer is UITapGestureRecognizer {
    singleTapGestureRecognizer.require(toFail: recognizer)
}
mapView.addGestureRecognizer(singleTapGestureRecognizer)`

If you would like to disable a specific set of gesture recognizers, such as zoom, you can set the Boolean value for the appropriate property to `false`. You can then add your own gesture recognizers to perform those actions.

With [runtime styling](runtime-styling.html), you can also use user interactions to style the map!
