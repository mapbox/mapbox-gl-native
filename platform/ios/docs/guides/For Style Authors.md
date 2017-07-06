<!--
  This file is generated.
  Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.
-->
# Information for Style Authors

A _style_ defines a map view’s content and appearance. If you’ve authored a
style using
[Mapbox Studio’s Styles editor](https://www.mapbox.com/studio/styles/) or as
JSON in a text editor, you can use that style in this SDK and manipulate it
afterwards in code. This document provides information you can use to ensure a
seamless transition from Mapbox Studio to your application.

## Designing for iOS

When designing your style, consider the context in which your application shows
the style. There are a number of considerations specific to iOS that may
not be obvious when designing your style in Mapbox Studio on the Web. A map view
is essentially a graphical user interface element, so many of same issues in
user interface design also apply when designing a map style.

### Color

Ensure sufficient contrast in your application’s user interface when your map
style is present. Standard user interface elements such as toolbars, sidebars,
and sheets often overlap the map view with a translucent, blurred background, so
make sure the contents of these elements remain legible with the map view
underneath.
The user location annotation view, the attribution button, any buttons in
callout views, and any items in the navigation bar are influenced by your
application’s tint color, so choose a tint color that contrasts well with your
map style.
If you intend your style to be used in the dark, consider the impact that Night
Shift may have on your style’s colors.

### Typography and graphics

Choose font and icon sizes appropriate to iOS devices. iPhones and iPads have
smaller screens than the typical browser window in which you would use Mapbox
Studio, especially when multitasking is enabled. Your user’s viewing distance
may be shorter than on a desktop computer. Some of your users may use the Larger
Dynamic Type and Accessibility Text features to increase the size of all text on
the device. You can use the
[runtime styling API](#manipulating-the-style-at-runtime) to adjust your style’s
font and icon sizes accordingly.

Design sprite images and choose font weights that look crisp on both
standard-resolution displays and Retina displays. This SDK supports the same
resolutions as iOS.
Standard-resolution displays are limited to older devices that your application
may or may not support, depending on its minimum deployment target.

Icon and text labels should be legible regardless of the map’s orientation.
By default, this SDK makes it easy for your users to rotate or tilt the map
using multitouch gestures.
If you do not intend your design to accommodate rotation and tilting, disable
these gestures using the `MGLMapView.rotateEnabled` and
`MGLMapView.pitchEnabled` properties, respectively, or the corresponding
inspectables in Interface Builder.

### Interactivity

Pay attention to whether elements of your style appear to be interactive.
A text label may look like a tappable button merely due to matching your
application’s tint color or the default blue tint color.
You can make an icon or text label interactive by installing a gesture
recognizer and performing feature querying (e.g.,
`-[MGLMapView visibleFeaturesAtPoint:]`) to get details about the selected
feature.

Make sure your users can easily distinguish any interactive elements from the
surrounding map, such as pins, the user location annotation view, or a route
line. Avoid relying on hover effects to indicate interactive elements. Leave
enough room between interactive elements to accommodate imprecise tapping
gestures.

For more information about user interface design, consult Apple’s
[_iOS Human Interface Guidelines_](https://developer.apple.com/ios/human-interface-guidelines/).
To learn more about designing maps for mobile devices, see [Nathaniel Slaughter's blog post](https://www.mapbox.com/blog/designing-maps-for-mobile-devices/) on
the subject.

## Applying your style

You set an `MGLMapView` object’s style either in code, by setting the
`MGLMapView.styleURL` property, or in Interface Builder, by setting the “Style
URL” inspectable. The URL must point to a local or remote style JSON file. The
style JSON file format is defined by the
[Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-style-spec/). This
SDK supports the functionality defined by version 8 of the specification unless
otherwise noted in the
[style specification documentation](https://www.mapbox.com/mapbox-gl-style-spec/).

## Manipulating the style at runtime

The _runtime styling API_ enables you to modify every aspect of a style
dynamically as a user interacts with your application. The style itself is
represented at runtime by an `MGLStyle` object, which provides access to various
`MGLSource` and `MGLStyleLayer` objects that represent content sources and style
layers, respectively.
For more information about the capabilities exposed by the runtime styling API,
see “[Runtime Styling](runtime-styling.html)”.

The names of runtime styling classes and properties on iOS are generally
consistent with the style specification and Mapbox Studio’s Styles editor. Any
exceptions are listed in this document.

To avoid conflicts with Objective-C keywords or Cocoa terminology, this SDK uses
the following terms for concepts defined in the style specification:

In the style specification | In the SDK
---------------------------|---------
filter                     | predicate
function type              | interpolation mode
id                         | identifier
image                      | style image
layer                      | style layer
property                   | attribute
SDF icon                   | template image
source                     | content source

## Specifying the map’s content

Each source defined by a style JSON file is represented at runtime by a content
source object that you can use to initialize new style layers. The content
source object is a member of one of the following subclasses of `MGLSource`:

In style JSON | In the SDK
--------------|-----------
`geojson`     | `MGLShapeSource`
`raster`      | `MGLRasterSource`
`vector`      | `MGLVectorSource`
`image`       | `MGLImageSource`

`canvas` and `video` sources are not supported.

### Tile sources

Raster and vector sources may be defined in TileJSON configuration files. This
SDK supports the properties defined in the style specification, which are a
subset of the keys defined in version 2.1.0 of the
[TileJSON](https://github.com/mapbox/tilejson-spec/tree/master/2.1.0)
specification. As an alternative to authoring a custom TileJSON file, you may
supply various tile source options when creating a raster or vector source.
These options are detailed in the `MGLTileSourceOption` documentation:

In style JSON | In TileJSON   | In the SDK
--------------|---------------|-----------
`url`         | —             | `configurationURL` parameter in `-[MGLTileSource initWithIdentifier:configurationURL:]`
`tiles`       | `tiles`       | `tileURLTemplates` parameter in `-[MGLTileSource initWithIdentifier:tileURLTemplates:options:]`
`minzoom`     | `minzoom`     | `MGLTileSourceOptionMinimumZoomLevel`
`maxzoom`     | `maxzoom`     | `MGLTileSourceOptionMaximumZoomLevel`
`tileSize`    | —             | `MGLTileSourceOptionTileSize`
`attribution` | `attribution` | `MGLTileSourceOptionAttributionHTMLString` (but consider specifying `MGLTileSourceOptionAttributionInfos` instead for improved security)
`scheme`      | `scheme`      | `MGLTileSourceOptionTileCoordinateSystem`

### Shape sources

Shape sources also accept various options. These options are detailed in the
`MGLShapeSourceOption` documentation:

In style JSON    | In the SDK
-----------------|-----------
`data`           | `url` parameter in `-[MGLShapeSource initWithIdentifier:URL:options:]`
`maxzoom`        | `MGLShapeSourceOptionMaximumZoomLevel`
`buffer`         | `MGLShapeSourceOptionBuffer`
`tolerance`      | `MGLShapeSourceOptionSimplificationTolerance`
`cluster`        | `MGLShapeSourceOptionClustered`
`clusterRadius`  | `MGLShapeSourceOptionClusterRadius`
`clusterMaxZoom` | `MGLShapeSourceOptionMaximumZoomLevelForClustering`

To create a shape source from local GeoJSON data, first
[convert the GeoJSON data into a shape](working-with-geojson-data.html#converting-geojson-data-into-shape-objects),
then use the `-[MGLShapeSource initWithIdentifier:shape:options:]` method.

### Image sources

Image sources accept a non-axis aligned quadrilateral as their geographic coordinates.
These coordinates, in `MGLCoordinateQuad`, are described in counterclockwise order, 
in contrast to the clockwise order defined in the style specification. 

## Configuring the map content’s appearance

Each layer defined by the style JSON file is represented at runtime by a style
layer object, which you can use to refine the map’s appearance. The style layer
object is a member of one of the following subclasses of `MGLStyleLayer`:

In style JSON | In the SDK
--------------|-----------
`background` | `MGLBackgroundStyleLayer`
`circle` | `MGLCircleStyleLayer`
`fill` | `MGLFillStyleLayer`
`fill-extrusion` | `MGLFillExtrusionStyleLayer`
`line` | `MGLLineStyleLayer`
`raster` | `MGLRasterStyleLayer`
`symbol` | `MGLSymbolStyleLayer`

You configure layout and paint attributes by setting properties on these style
layer objects. The property names generally correspond to the style JSON
properties, except for the use of camelCase instead of kebab-case. Properties
whose names differ from the style specification are listed below:

### Circle style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`circle-pitch-scale` | `MGLCircleStyleLayer.circleScaleAlignment` | `MGLCircleStyleLayer.circleScaleAlignment`
`circle-translate` | `MGLCircleStyleLayer.circleTranslation` | `MGLCircleStyleLayer.circleTranslation`
`circle-translate-anchor` | `MGLCircleStyleLayer.circleTranslationAnchor` | `MGLCircleStyleLayer.circleTranslationAnchor`

### Fill style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`fill-antialias` | `MGLFillStyleLayer.fillAntialiased` | `MGLFillStyleLayer.isFillAntialiased`
`fill-translate` | `MGLFillStyleLayer.fillTranslation` | `MGLFillStyleLayer.fillTranslation`
`fill-translate-anchor` | `MGLFillStyleLayer.fillTranslationAnchor` | `MGLFillStyleLayer.fillTranslationAnchor`

### Fill extrusion style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`fill-extrusion-translate` | `MGLFillExtrusionStyleLayer.fillExtrusionTranslation` | `MGLFillExtrusionStyleLayer.fillExtrusionTranslation`
`fill-extrusion-translate-anchor` | `MGLFillExtrusionStyleLayer.fillExtrusionTranslationAnchor` | `MGLFillExtrusionStyleLayer.fillExtrusionTranslationAnchor`

### Line style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`line-dasharray` | `MGLLineStyleLayer.lineDashPattern` | `MGLLineStyleLayer.lineDashPattern`
`line-translate` | `MGLLineStyleLayer.lineTranslation` | `MGLLineStyleLayer.lineTranslation`
`line-translate-anchor` | `MGLLineStyleLayer.lineTranslationAnchor` | `MGLLineStyleLayer.lineTranslationAnchor`

### Raster style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`raster-brightness-max` | `MGLRasterStyleLayer.maximumRasterBrightness` | `MGLRasterStyleLayer.maximumRasterBrightness`
`raster-brightness-min` | `MGLRasterStyleLayer.minimumRasterBrightness` | `MGLRasterStyleLayer.minimumRasterBrightness`
`raster-hue-rotate` | `MGLRasterStyleLayer.rasterHueRotation` | `MGLRasterStyleLayer.rasterHueRotation`

### Symbol style layers

In style JSON | In Objective-C | In Swift
--------------|----------------|---------
`icon-allow-overlap` | `MGLSymbolStyleLayer.iconAllowsOverlap` | `MGLSymbolStyleLayer.iconAllowsOverlap`
`icon-ignore-placement` | `MGLSymbolStyleLayer.iconIgnoresPlacement` | `MGLSymbolStyleLayer.iconIgnoresPlacement`
`icon-image` | `MGLSymbolStyleLayer.iconImageName` | `MGLSymbolStyleLayer.iconImageName`
`icon-optional` | `MGLSymbolStyleLayer.iconOptional` | `MGLSymbolStyleLayer.isIconOptional`
`icon-rotate` | `MGLSymbolStyleLayer.iconRotation` | `MGLSymbolStyleLayer.iconRotation`
`icon-size` | `MGLSymbolStyleLayer.iconScale` | `MGLSymbolStyleLayer.iconScale`
`icon-keep-upright` | `MGLSymbolStyleLayer.keepsIconUpright` | `MGLSymbolStyleLayer.keepsIconUpright`
`text-keep-upright` | `MGLSymbolStyleLayer.keepsTextUpright` | `MGLSymbolStyleLayer.keepsTextUpright`
`text-max-angle` | `MGLSymbolStyleLayer.maximumTextAngle` | `MGLSymbolStyleLayer.maximumTextAngle`
`text-max-width` | `MGLSymbolStyleLayer.maximumTextWidth` | `MGLSymbolStyleLayer.maximumTextWidth`
`symbol-avoid-edges` | `MGLSymbolStyleLayer.symbolAvoidsEdges` | `MGLSymbolStyleLayer.symbolAvoidsEdges`
`text-field` | `MGLSymbolStyleLayer.text` | `MGLSymbolStyleLayer.text`
`text-allow-overlap` | `MGLSymbolStyleLayer.textAllowsOverlap` | `MGLSymbolStyleLayer.textAllowsOverlap`
`text-font` | `MGLSymbolStyleLayer.textFontNames` | `MGLSymbolStyleLayer.textFontNames`
`text-size` | `MGLSymbolStyleLayer.textFontSize` | `MGLSymbolStyleLayer.textFontSize`
`text-ignore-placement` | `MGLSymbolStyleLayer.textIgnoresPlacement` | `MGLSymbolStyleLayer.textIgnoresPlacement`
`text-justify` | `MGLSymbolStyleLayer.textJustification` | `MGLSymbolStyleLayer.textJustification`
`text-optional` | `MGLSymbolStyleLayer.textOptional` | `MGLSymbolStyleLayer.isTextOptional`
`text-rotate` | `MGLSymbolStyleLayer.textRotation` | `MGLSymbolStyleLayer.textRotation`
`icon-translate` | `MGLSymbolStyleLayer.iconTranslation` | `MGLSymbolStyleLayer.iconTranslation`
`icon-translate-anchor` | `MGLSymbolStyleLayer.iconTranslationAnchor` | `MGLSymbolStyleLayer.iconTranslationAnchor`
`text-translate` | `MGLSymbolStyleLayer.textTranslation` | `MGLSymbolStyleLayer.textTranslation`
`text-translate-anchor` | `MGLSymbolStyleLayer.textTranslationAnchor` | `MGLSymbolStyleLayer.textTranslationAnchor`

## Setting attribute values

Each property representing a layout or paint attribute is set to an
`MGLStyleValue` object, which is either an `MGLConstantStyleValue` object (for
constant values) or an `MGLStyleFunction` object (for style functions). The
style value object is a container for the raw value or function parameters that
you want the attribute to be set to.

### Constant style values

In contrast to the JSON type that the style specification defines for each
layout or paint property, the style value object often contains a more specific
Foundation or Cocoa type. General rules for attribute types are listed below.
Pay close attention to the SDK documentation for the attribute you want to get
or set.

In style JSON | In Objective-C        | In Swift
--------------|-----------------------|---------
Color         | `UIColor` | `UIColor`
Enum          | `NSValue` (see `NSValue(MGLAdditions)`) | `NSValue` (see `NSValue(MGLAdditions)`)
String        | `NSString`            | `String`
Boolean       | `NSNumber.boolValue`  | `Bool`
Number        | `NSNumber.floatValue` | `Float`
Array (`-dasharray`) | `NSArray<NSNumber>` | `[Float]`
Array (`-font`) | `NSArray<NSString>` | `[String]`
Array (`-offset`, `-translate`) | `NSValue.CGVectorValue` | `NSValue.cgVectorValue`
Array (`-padding`) | `NSValue.UIEdgeInsetsValue` | `NSValue.uiEdgeInsetsValue`

For padding attributes, note that the arguments to
`UIEdgeInsetsMake()` in Objective-C and `UIEdgeInsets(top:left:bottom:right:)`
in Swift
are specified in counterclockwise order, in contrast to the clockwise order
defined by the style specification.

### Style functions

A _style function_ allows you to vary the value of a layout or paint attribute
based on the zoom level, data provided by content sources, or both. For more
information about style functions, see “[Using Style Functions at Runtime](using-style-functions-at-runtime.html)”.

Each kind of style function is represented by a distinct class, but you
typically create style functions as you create any other style value, using
class methods on `MGLStyleValue`:

In style specification     | SDK class                   | SDK factory method
---------------------------|-----------------------------|-------------------
zoom function              | `MGLCameraStyleFunction`    | `+[MGLStyleValue valueWithInterpolationMode:cameraStops:options:]`
property function          | `MGLSourceStyleFunction`    | `+[MGLStyleValue valueWithInterpolationMode:sourceStops:attributeName:options:]`
zoom-and-property function | `MGLCompositeStyleFunction` | `+[MGLStyleValue valueWithInterpolationMode:compositeStops:attributeName:options:]`

The documentation for each individual style layer property indicates the kinds
of style functions that are enabled for that property.

When you create a style function, you specify an _interpolation mode_ and a
series of _stops_. Each stop determines the effective value displayed at a
particular zoom level (for camera functions) or the effective value on features
with a particular attribute value in the content source (for source functions).
The interpolation mode tells the SDK how to calculate the effective value
between any two stops:

In style specification       | In the SDK
-----------------------------|-----------
`exponential`                | `MGLInterpolationModeExponential`
`interval`                   | `MGLInterpolationModeInterval`
`categorical`                | `MGLInterpolationModeCategorical`
`identity`                   | `MGLInterpolationModeIdentity`

## Filtering sources

You can filter a shape or vector source by setting the
`MGLVectorStyleLayer.predicate` property to an `NSPredicate` object. Below is a
table of style JSON operators and the corresponding operators used in the
predicate format string:

In style JSON             | In the format string
--------------------------|---------------------
`["has", key]`            | `key != nil`
`["!has", key]`           | `key == nil`
`["==", key, value]`      | `key == value`
`["!=", key, value]`      | `key != value`
`[">", key, value]`       | `key > value`
`[">=", key, value]`      | `key >= value`
`["<", key, value]`       | `key < value`
`["<=", key, value]`      | `key <= value`
`["in", key, v0, …, vn]`  | `key IN {v0, …, vn}`
`["!in", key, v0, …, vn]` | `NOT key IN {v0, …, vn}`
`["all", f0, …, fn]`      | `p0 AND … AND pn`
`["any", f0, …, fn]`      | `p0 OR … OR pn`
`["none", f0, …, fn]`     | `NOT (p0 OR … OR pn)`

See the `MGLVectorStyleLayer.predicate` documentation for a full description of
the supported operators and operand types.
