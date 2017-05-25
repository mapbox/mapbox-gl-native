#import "MGLSource.h"

#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
/**
 `MGLImageSource` is a map content source that supplies a georeferenced raster
 image to be shown on the map. The geographic location of the raster image content, 
 supplied with `MGLCoordinateQuad`, can be non-axis aligned.
 `MGLImageSource` supports raster content from `NSURL` or an `MGLImage`.
 Supported image formats are : PNG, TIFF, JPEG, GIF (single frame rendering),
 and BMP.
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
 let coordinates: MGLCoordinateQuad = MGLCoordinateQuad(
   topLeft: CLLocationCoordinate2D(latitude: 46.437, longitude: -80.425),
   topRight: CLLocationCoordinate2D(latitude: 46.437, longitude: -71.516),
   bottomRight: CLLocationCoordinate2D(latitude: 37.936, longitude: -71.516),
   bottomLeft: CLLocationCoordinate2D(latitude: 37.936, longitude: -80.425))
 let source = MGLImageSource(identifier: "images", coordinates: coordinates, imageURL: URL(string: "https://www.mapbox.com/mapbox-gl-js/assets/radar.gif")!)
 mapView.style?.addSource(source)
 ```
 */
@interface MGLImageSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a georeferenced image source with an identifier and coordinates

 @param identifier A string that uniquely identifies the source.
 @param coordinateQuad The top left, top right, bottom right, and bottom left coordinates for the image.
 @return An initialized image source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad NS_DESIGNATED_INITIALIZER;

/**
 Returns a georeferenced image source with an identifier, coordinates and a URL

 @param identifier A string that uniquely identifies the source.
 @param coordinateQuad the top left, top right, bottom right, and bottom left coordinates for the image.
 @param url An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad imageURL:(NSURL *)url;

/**
 Returns a georeferenced image source with an identifier, coordinates and an image

 @param identifier A string that uniquely identifies the source.
 @param coordinateQuad The top left, top right, bottom right, and bottom left coordinates for the image.
 @param image The image to display for the sourcde.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier coordinates:(MGLCoordinateQuad)coordinateQuad image:(MGLImage *)image;

/**
 Returns a georeferenced image source with an identifier and coordinates

 @param identifier A string that uniquely identifies the source.
 @param bounds The aligned coordinate bounds for the image.
 @return An initialized image source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds;

/**
 Returns a georeferenced image source with an identifier, coordinates and a URL

 @param identifier A string that uniquely identifies the source.
 @param bounds The aligned coordinate bounds for the image.@param url An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds imageURL:(NSURL *)url;

/**
 Returns a georeferenced image source with an identifier, coordinates and an image

 @param identifier A string that uniquely identifies the source.
 @param bounds The aligned coordinate bounds for the image.
 @param image The image to display for the sourcde.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier bounds:(MGLCoordinateBounds)bounds image:(MGLImage *)image;

#pragma mark Accessing a Source’s Content

/**
 The URL to the source image.

 If the receiver was initialized using `-initWithIdentifier:coordinates:` or 
 `-initWithIdentifier:coordinates:image:`, or the `image` property is set,
 this property is set to `nil`.
 */
@property (nonatomic, copy, nullable)NSURL *URL;

/**
 The source image.

 If the receiver was initialized using `-initWithIdentifier:coordinates:` or 
 `-initWithIdentifier:coordinates:imageURL:`, or if the `URL` property is set,
 this property is set to `nil`.
 

 */
@property (nonatomic, retain, nullable)MGLImage *image;

/**
 The coordinates at which the source image will be placed.

 If the receiver was initialized using `-initWithIdentifier:bounds:`,
 `-initWithIdentifier:bounds:image:`, or `initWithIdentifier:bounds:imageURL`,
 the bounds will be converted to an `MGLCoordinateQuad`.
 */
@property (nonatomic) MGLCoordinateQuad coordinates;
@end

NS_ASSUME_NONNULL_END
