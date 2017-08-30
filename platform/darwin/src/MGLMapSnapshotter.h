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
 Creates a set of options with the minimum required information
 @param styleURL the style url to use
 @param camera the camera settings
 @param size the image size
 */
- (instancetype)initWithStyleURL:(NSURL*)styleURL camera:(MGLMapCamera*)camera size:(CGSize)size;

#pragma mark - Configuring the map

/**
  The style URL for these options.
 */
@property (nonatomic, readonly) NSURL* styleURL;

/**
 The zoom. Default is 0.
 */
@property (nonatomic) double zoom;

/**
 The `MGLMapcamera` options to use.
 */
@property (nonatomic) MGLMapCamera* camera;

/**
 A region to capture. Overrides the center coordinate
 in the mapCamera options if set
 */
@property (nonatomic) MGLCoordinateBounds region;

#pragma mark - Configuring the image

/**
 The size of the output image. Minimum is 64x64
 */
@property (nonatomic, readonly) CGSize size;

/**
 The scale of the output image. Defaults to the main screen scale.
 Minimum is 1.
 */
@property (nonatomic) CGFloat scale;

@end

/**
 A block to processes the result or error of a snapshot request.
 
 The result will be either an `MGLImage` or a `NSError`
 
 @param snapshot The image that was generated or `nil` if an error occurred.
 @param error The eror that occured or `nil` when succesful.
 */
typedef void (^MGLMapSnapshotCompletionHandler)(MGLImage* _Nullable snapshot, NSError* _Nullable error);

/**
 A utility object for capturing map-based images.
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
 */
- (void)cancel;

/**
 Indicates whether as snapshot is currently being generated.
 */
@property (nonatomic, readonly, getter=isLoading) BOOL loading;

@end

NS_ASSUME_NONNULL_END
