#import <Foundation/Foundation.h>
#import "MGLTypes.h"
#import "MGLGeometry.h"
#import "MGLMapCamera.h"

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
/**
 The options to use when creating images with the `MGLMapsnapshotter`.
 */
@interface MGLMapSnapshotOptions : NSObject

/**
 Creates a set of options with the minimum required information.
 
 @param styleURL URL of the map style to snapshot. The URL may be a full HTTP or HTTPS URL,
 a Mapbox URL indicating the style’s map ID (`mapbox://styles/{user}/{style`}), or a path
 to a local file relative to the application’s resource path. Specify `nil` for the default style.
 @param size The image size.
 */
- (instancetype)initWithStyleURL:(nullable NSURL *)styleURL camera:(MGLMapCamera *)camera size:(CGSize)size;

#pragma mark - Configuring the Map

/**
 URL of the map style to snapshot.
 */
@property (nonatomic, readonly) NSURL *styleURL;

/**
 The zoom level.
 
 The default zoom level is 0. If this property is non-zero and the camera property
 is non-nil, the camera’s altitude is ignored in favor of this property’s value.
 */
@property (nonatomic) double zoomLevel;

/**
 A camera representing the viewport visible in the snapshot.
 
 If this property is non-nil and the `coordinateBounds` property is set to a non-empty
 coordinate bounds, the camera’s center coordinate and altitude are ignored in favor
 of the `coordinateBounds` property.
 */
@property (nonatomic) MGLMapCamera *camera;

/**
 The cooordinate rectangle that encompasses the bounds to capture.
 
 If this property is non-empty and the camera property is non-nil, the camera’s
 center coordinate and altitude are ignored in favor of this property’s value.
 */
@property (nonatomic) MGLCoordinateBounds coordinateBounds;

#pragma mark - Configuring the Image

/**
 The size of the output image, measured in points.
 
 */
@property (nonatomic, readonly) CGSize size;

/**
 The scale of the output image. Defaults to the main screen scale.
 Minimum is 1.
 */
@property (nonatomic) CGFloat scale;

@end

#if TARGET_OS_IPHONE
/**
 A block to processes the result or error of a snapshot request.
 
 @param snapshot The `UIImage` that was generated or `nil` if an error occurred.
 @param error The error that occured or `nil` when successful.
 */
typedef void (^MGLMapSnapshotCompletionHandler)(UIImage* _Nullable snapshot, NSError* _Nullable error);
#else
/**
 A block to processes the result or error of a snapshot request.
 
 @param snapshot The `NSImage` that was generated or `nil` if an error occurred.
 @param error The eror that occured or `nil` when succesful.
 */
typedef void (^MGLMapSnapshotCompletionHandler)(NSImage* _Nullable snapshot, NSError* _Nullable error);
#endif

/**
 An immutable utility object for capturing map-based images.
 
 ### Example
 
 ```swift
 var camera = MGLMapCamera()
 camera.centerCoordinate = CLLocationCoordinate2D(latitude: 37.7184, longitude: -122.4365)
 camera.pitch = 20
 
 var options = MGLMapSnapshotOptions(styleURL: MGLStyle.satelliteStreetsStyleURL(), camera: camera, size: CGSize(width: 320, height: 480))
 options.zoomLevel = 10
 
 var snapshotter = MGLMapSnapshotter(options: options)
 snapshotter.start { (image, error) in
   if error {
     // error handler
   } else {
     // image handler
   }
 }
 ```
 */
MGL_EXPORT
@interface MGLMapSnapshotter : NSObject

- (instancetype)initWithOptions:(MGLMapSnapshotOptions*)options;

/**
 Starts the snapshot creation and executes the specified block with the result.
 
 @param completionHandler The block to handle the result in.
 */
- (void)startWithCompletionHandler:(MGLMapSnapshotCompletionHandler)completionHandler;

/**
 Starts the snapshot creation and executes the specified block with the result on the specified queue.
 
 @param queue The queue to handle the result on.
 @param completionHandler The block to handle the result in.
 */
- (void)startWithQueue:(dispatch_queue_t)queue completionHandler:(MGLMapSnapshotCompletionHandler)completionHandler;

/**
 Cancels the snapshot creation request, if any.
 
 Once you call this method, you cannot resume the snapshot. In order to obtain the
 snapshot, create a new `MGLMapSnapshotter` object.
 */
- (void)cancel;

/**
 Indicates whether as snapshot is currently being generated.
 */
@property (nonatomic, readonly, getter=isLoading) BOOL loading;

@end

NS_ASSUME_NONNULL_END
