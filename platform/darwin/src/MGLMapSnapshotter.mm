#import "MGLMapSnapshotter.h"

#import <mbgl/actor/actor.hpp>
#import <mbgl/actor/scheduler.hpp>
#import <mbgl/util/geo.hpp>
#import <mbgl/map/map_snapshotter.hpp>
#import <mbgl/map/camera.hpp>
#import <mbgl/storage/default_file_source.hpp>
#import <mbgl/util/default_thread_pool.hpp>
#import <mbgl/util/string.hpp>
#import <mbgl/util/shared_thread_pool.hpp>

#import "MGLOfflineStorage_Private.h"
#import "MGLGeometry_Private.h"
#import "NSBundle+MGLAdditions.h"

#if TARGET_OS_IPHONE
#import "UIImage+MGLAdditions.h"
#else
#import "NSImage+MGLAdditions.h"
#endif

@implementation MGLMapSnapshotOptions

- (instancetype _Nonnull)initWithStyleURL:(NSURL* _Nonnull)styleURL camera:(MGLMapCamera*)camera size:(CGSize) size;
{
    self = [super init];
    if (self) {
        _styleURL = styleURL;
        _size = size;
        _camera = camera;
#if TARGET_OS_IPHONE
        _scale = [UIScreen mainScreen].scale;
#else
        _scale = [NSScreen mainScreen].backingScaleFactor;
#endif
        
    }
    return self;
}

@end

@implementation MGLMapSnapshotter {
    
    std::shared_ptr<mbgl::ThreadPool> mbglThreadPool;
    std::unique_ptr<mbgl::MapSnapshotter> mbglMapSnapshotter;
    std::unique_ptr<mbgl::Actor<mbgl::MapSnapshotter::Callback>> snapshotCallback;
}

- (instancetype)initWithOptions:(MGLMapSnapshotOptions*)options;
{
    self = [super init];
    if (self) {
        _loading = false;
        
        mbgl::DefaultFileSource *mbglFileSource = [MGLOfflineStorage sharedOfflineStorage].mbglFileSource;
        mbglThreadPool = mbgl::sharedThreadPool();
        
        std::string styleURL = std::string([options.styleURL.absoluteString UTF8String]);
        
        // Size; taking into account the minimum texture size for OpenGL ES
        mbgl::Size size = {
            static_cast<uint32_t>(MAX(options.size.width, 64)),
            static_cast<uint32_t>(MAX(options.size.height, 64))
        };
        
        float pixelRatio = MAX(options.scale, 1);
        
        // Camera options
        mbgl::CameraOptions cameraOptions;
        if (CLLocationCoordinate2DIsValid(options.camera.centerCoordinate)) {
            cameraOptions.center = MGLLatLngFromLocationCoordinate2D(options.camera.centerCoordinate);
        }
        cameraOptions.angle = MAX(0, options.camera.heading) * mbgl::util::DEG2RAD;
        cameraOptions.zoom = MAX(0, options.zoom);
        cameraOptions.pitch = MAX(0, options.camera.pitch);
        
        // Region
        mbgl::optional<mbgl::LatLngBounds> region;
        if (!MGLCoordinateBoundsIsEmpty(options.region)) {
            region = MGLLatLngBoundsFromCoordinateBounds(options.region);
        }
        
        // Create the snapshotter
        mbglMapSnapshotter = std::make_unique<mbgl::MapSnapshotter>(*mbglFileSource, *mbglThreadPool, styleURL, size, pixelRatio, cameraOptions, region);
    }
    return self;
}

- (void)startWithCompletionHandler:(MGLMapSnapshotCompletionHandler)completion;
{
    [self startWithQueue:dispatch_get_main_queue() completionHandler:completion];
}

- (void)startWithQueue:(dispatch_queue_t)queue completionHandler:(MGLMapSnapshotCompletionHandler)completion;
{
    if ([self isLoading]) {
        NSDictionary *userInfo = @{NSLocalizedDescriptionKey: @"Already started this snapshotter"};
        NSError *error = [NSError errorWithDomain:MGLErrorDomain code:1 userInfo:userInfo];
        dispatch_async(queue, ^{
           completion(nil, error); 
        });
        return;
    }
    
    _loading = true;
    
    dispatch_async(queue, ^{
        snapshotCallback = std::make_unique<mbgl::Actor<mbgl::MapSnapshotter::Callback>>(*mbgl::Scheduler::GetCurrent(), [=](std::exception_ptr mbglError, mbgl::PremultipliedImage image) {
            _loading = false;
            if (mbglError) {
                NSString *description = @(mbgl::util::toString(mbglError).c_str());
                NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description};
                NSError *error = [NSError errorWithDomain:MGLErrorDomain code:1 userInfo:userInfo];
                
                // Dispatch result to origin queue
                dispatch_async(queue, ^{
                    completion(nil, error);
                });
            } else {
                MGLImage *mglImage = [[MGLImage alloc] initWithMGLPremultipliedImage:std::move(image)];
                
                // Process image watermark in a work queue
                dispatch_queue_t workQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
                dispatch_async(workQueue, ^{
#if TARGET_OS_IPHONE
                    UIImage *logoImage = [UIImage imageNamed:@"mapbox" inBundle:[NSBundle mgl_frameworkBundle] compatibleWithTraitCollection:nil];
                    
                    UIGraphicsBeginImageContext(mglImage.size);
                    
                    [mglImage drawInRect:CGRectMake(0, 0, mglImage.size.width, mglImage.size.height)];
                    [logoImage drawInRect:CGRectMake(8, mglImage.size.height - (8 + logoImage.size.height), logoImage.size.width,logoImage.size.height)];
                    UIImage *compositedImage = UIGraphicsGetImageFromCurrentImageContext();
                    
                    UIGraphicsEndImageContext();
#else
                    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
                    NSImage *compositedImage = mglImage;
                    
                    [compositedImage lockFocus];
                    [logoImage drawInRect:CGRectMake(8, 8, logoImage.size.width,logoImage.size.height)];
                    [compositedImage unlockFocus];
#endif
                    
                    // Dispatch result to origin queue
                    dispatch_async(queue, ^{
                        completion(compositedImage, nil);
                    });
                });
            }
        });
        mbglMapSnapshotter->snapshot(snapshotCallback->self());
    });
}

- (void)cancel;
{
    snapshotCallback.reset();
    mbglMapSnapshotter.reset();
}

@end
