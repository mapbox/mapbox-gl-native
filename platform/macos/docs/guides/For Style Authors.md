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

## Designing for macOS

When designing your style, consider the context in which your application shows
the style. There are a number of considerations specific to macOS that may
not be obvious when designing your style in Mapbox Studio on the Web. A map view
is essentially a graphical user interface element, so many of same issues in
user interface design also apply when designing a map style.

### Color

Ensure sufficient contrast in your application’s user interface when your map
style is present. Standard user interface elements such as toolbars, sidebars,
and sheets often overlap the map view with a translucent, blurred background, so
make sure the contents of these elements remain legible with the map view
underneath.
If you intend your style to be used in the dark, consider the impact that Night
Shift may have on your style’s colors.

### Typography and graphics


Design sprite images and choose font weights that look crisp on both
standard-resolution displays and Retina displays. This SDK supports the same
resolutions as macOS.
Standard-resolution displays are often found on external monitors. Even with
built-in screens, some of your users may use the Larger Text option in Display
Preferences, which is essentially standard resolution, to make text easier to
read.

Icon and text labels should be legible regardless of the map’s orientation.
By default, this SDK makes it easy for your users to rotate or tilt the map
using multitouch trackpad gestures or keyboard shortcuts.
If you do not intend your design to accommodate rotation and tilting, disable
these gestures using the `MGLMapView.rotateEnabled` and
`MGLMapView.pitchEnabled` properties, respectively, or the corresponding
inspectables in Interface Builder.

### Interactivity

Pay attention to whether elements of your style appear to be interactive.
An icon with a shadow or shading effect may appear to be clickable.
You can make an icon or text label interactive by installing a gesture
recognizer and performing feature querying (e.g.,
`-[MGLMapView visibleFeaturesAtPoint:]`) to get details about the selected
feature.
You can install cursor or tooltip tracking rectangles to indicate interactive
features as an alternative to prominent hover effects.

Make sure your users can easily distinguish any interactive elements from the
surrounding map, such as pins or a route line. If your application supports
printing, consider using the
[runtime styling API](#manipulating-the-style-at-runtime) to optimize your style
for ink economy before printing the map view.

For more information about user interface design, consult Apple’s
[_macOS Human Interface Guidelines_](https://developer.apple.com/library/content/documentation/UserExperience/Conceptual/OSXHIGuidelines/).

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

The names of runtime styling classes and properties on macOS are generally
consistent with the style specification and Mapbox Studio’s Styles editor. Any
exceptions are listed in this document.

To avoid conflicts with Objective-C keywords or Cocoa terminology, this SDK uses
the following terms for concepts defined in the style specification:

In the style specification | In the SDK
---------------------------|---------
bounds                     | coordinate bounds
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
`bounds`      | `bounds`      | `MGLTileSourceOptionCoordinateBounds`
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
`heatmap` | `MGLHeatmapStyleLayer`
`hillshade` | `MGLHillshadeStyleLayer`
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
`NSExpression` object. `NSExpression` objects play the same role as
[expressions in the Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions),
but you create the former using a very different syntax. `NSExpression`’s format
string syntax is reminiscent of a spreadsheet formula or an expression in a
database query. See the
“[Predicates and Expressions](Predicates and Expressions.md)” guide for an
overview of the expression support in this SDK. This SDK no longer supports
style functions; use expressions instead.

### Constant values in expressions

In contrast to the JSON type that the style specification defines for each
layout or paint property, the style value object often contains a more specific
Foundation or Cocoa type. General rules for attribute types are listed below.
Pay close attention to the SDK documentation for the attribute you want to get
or set.

In style JSON | In Objective-C        | In Swift
--------------|-----------------------|---------
Color         | `NSColor` | `NSColor`
Enum          | `NSString`            | `String`
String        | `NSString`            | `String`
Boolean       | `NSNumber.boolValue`  | `NSNumber.boolValue`
Number        | `NSNumber.floatValue` | `NSNumber.floatValue`
Array (`-dasharray`) | `NSArray<NSNumber>` | `[Float]`
Array (`-font`) | `NSArray<NSString>` | `[String]`
Array (`-offset`, `-translate`) | `NSValue` containing `CGVector` | `NSValue` containing `CGVector`
Array (`-padding`) | `NSValue.edgeInsetsValue` | `NSValue.edgeInsetsValue`

For padding attributes, note that the arguments to
`NSEdgeInsetsMake()` in Objective-C and `EdgeInsets(top:left:bottom:right:)` in
Swift
are specified in counterclockwise order, in contrast to the clockwise order
defined by the style specification.

Additionally, on macOS, a screen coordinate of (0, 0) is located at the
lower-left corner of the screen. Therefore, a positive `CGVector.dy` means an
offset or translation upward, while a negative `CGVector.dy` means an offset or
translation downward. This is the reverse of how `CGVector` is interpreted on
iOS.

### Expression operators

In style specification | Method, function, or predicate type | Format string syntax
-----------------------|-------------------------------------|---------------------
`array`                | |
`boolean`              | |
`literal`              | `+[NSExpression expressionForConstantValue:]` | `%@` representing `NSArray` or `NSDictionary`
`number`               | |
`string`               | |
`to-boolean`           | `boolValue` |
`to-color`             | |
`to-number`            | `mgl_numberWithFallbackValues:` |
`to-string`            | `stringValue` |
`typeof`               | |
`geometry-type`        | |
`id`                   | |
`properties`           | |
`at`                   | |
`get`                  | `+[NSExpression expressionForKeyPath:]` | Key path
`has`                  | |
`length`               | `count:` | `count({1, 2, 2, 3, 4, 7, 9})`
`!`                    | `NSNotPredicateType` | `NOT (p0 OR … OR pn)`
`!=`                   | `NSNotEqualToPredicateOperatorType` | `key != value`
`<`                    | `NSLessThanPredicateOperatorType` | `key < value`
`<=`                   | `NSLessThanOrEqualToPredicateOperatorType` | `key <= value`
`==`                   | `NSEqualToPredicateOperatorType` | `key == value`
`>`                    | `NSGreaterThanPredicateOperatorType` | `key > value`
`>=`                   | `NSGreaterThanOrEqualToPredicateOperatorType` | `key >= value`
`all`                  | `NSAndPredicateType` | `p0 AND … AND pn`
`any`                  | `NSOrPredicateType` | `p0 OR … OR pn`
`case`                 | `+[NSExpression expressionForConditional:trueExpression:falseExpression:]` | `TERNARY(condition, trueExpression, falseExpression)`
`coalesce`             | |
`match`                | |
`interpolate`          | `mgl_interpolateWithCurveType:parameters:stops:` |
`step`                 | `mgl_stepWithMinimum:stops:` |
`let`                  | `mgl_expressionWithContext:` |
`var`                  | `+[NSExpression expressionForVariable:]` | `$variable`
`concat`               | `stringByAppendingString:` |
`downcase`             | `lowercase:` | `lowercase('DOWNTOWN')`
`upcase`               | `uppercase:` | `uppercase('Elysian Fields')`
`rgb`                  | `+[NSColor colorWithCalibratedRed:green:blue:alpha:]` |
`rgba`                 | `+[NSColor colorWithCalibratedRed:green:blue:alpha:]` |
`to-rgba`              | |
`-`                    | `from:subtract:` | `2 - 1`
`*`                    | `multiply:by:` | `1 * 2`
`/`                    | `divide:by:` | `1 / 2`
`%`                    | `modulus:by:` |
`^`                    | `raise:toPower:` | `2 ** 2`
`+`                    | `add:to:` | `1 + 2`
`acos`                 | |
`asin`                 | |
`atan`                 | |
`cos`                  | |
`e`                    | | `%@` representing `NSNumber` containing `M_E`
`ln`                   | `ln:` | `ln(2)`
`ln2`                  | | `%@` representing `NSNumber` containing `M_LN2`
`log10`                | `log:` | `log(1)`
`log2`                 | |
`max`                  | `max:` | `max({1, 2, 2, 3, 4, 7, 9})`
`min`                  | `min:` | `min({1, 2, 2, 3, 4, 7, 9})`
`pi`                   | | `%@` representing `NSNumber` containing `M_PI`
`sin`                  | |
`sqrt`                 | `sqrt:` | `sqrt(2)`
`tan`                  | |
`zoom`                 | | `$zoom`
`heatmap-density`      | | `$heatmapDensity`

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

See the “[Predicates and Expressions](Predicates and Expressions.md)” guide for
a full description of the supported operators and operand types.
