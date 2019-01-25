#import "MGLSource.h"

#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
/**
 `MGLImageSource` is a content source that is used for a georeferenced raster
 image to be shown on the map. The georeferenced image scales and rotates as the
 user zooms and rotates the map. Images may also be used as icons or patterns 
 in a style layer. To register an image for use as an icon or pattern, 
 use the `-[MGLStyle setImage:forName:]` method. To configure a point 
 annotation’s image, use the `MGLAnnotationImage` class.
 
 The geographic location of the raster image content, supplied with 
 `MGLCoordinateQuad`, can be non-axis aligned.
 `MGLImageSource` supports raster content from `NSURL`, `NSImage` (macOS), or 
 `UIImage` (iOS).
 An image source is added to an `MGLStyle` object along with one or more
 `MGLRasterStyleLayer` objects. Use a raster style layer to control the
 appearance of content supplied by the image source.
 
 Each
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-image"><code>image</code></a>
 source defined by the style JSON file is represented at runtime by an
 `MGLImageSource` object that you can use to initialize new style layers. You
 can also add and remove sources dynamically using methods such as
 `-[MGLStyle addSource:]` and `-[MGLStyle sourceWithIdentifier:]`.

 ### Example

 ```swift
 let coordinates = MGLCoordinateQuad(
   topLeft: CLLocationCoordinate2D(latitude: 46.437, longitude: -80.425),
   bottomLeft: CLLocationCoordinate2D(latitude: 37.936, longitude: -80.425),
   bottomRight: CLLocationCoordinate2D(latitude: 37.936, longitude: -71.516),
   topRight: CLLocationCoordinate2D(latitude: 46.437, longitude: -71.516))
 let source = MGLImageSource(identifier: "radar", coordinateQuad: coordinates, url: URL(string: "https://www.mapbox.com/mapbox-gl-js/assets/radar.gif")!)
 mapView.style?.addSource(source)
 ```
 
 #### Related examples
 See the <a href="https://docs.mapbox.com/ios/maps/examples/image-source/">
 Add an image</a> example to learn how to add an image to your map using
 `MGLImageSource`.
 */
MGL_EXPORT
@interface MGLImageSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a georeferenced image source with an identifier, coordinates and a URL.

 @param identifier A string that uniquely identifies the source.
 @param coordinateQuad the top left, top right, bottom right, and bottom left coordinates for the image.
 @param url An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad URL:(NSURL *)url;

/**
 Returns a georeferenced image source with an identifier, coordinates and an image.

 @param identifier A string that uniquely identifies the source.
 @param coordinateQuad The top left, top right, bottom right, and bottom left coordinates for the image.
 @param image The image to display for the source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier coordinateQuad:(MGLCoordinateQuad)coordinateQuad image:(MGLImage *)image;

#pragma mark Accessing a Source’s Content

/**
 The URL to the source image.

 If the receiver was initialized using `-initWithIdentifier:coordinateQuad:image:` or
 the `image` property is set, this property is set to `nil`.
 */
@property (nonatomic, copy, nullable)NSURL *URL;

/**
 The source image.

 If the receiver was initialized using `-initWithIdentifier:coordinateQuad:URL:` or if the `URL` property is set, this property is set to `nil`.
 */
@property (nonatomic, retain, nullable)MGLImage *image;

/**
 The coordinates at which the corners of the source image will be placed.
 */
@property (nonatomic) MGLCoordinateQuad coordinates;
@end

NS_ASSUME_NONNULL_END
