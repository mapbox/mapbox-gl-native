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
#import "MGLStyle.h"

#if TARGET_OS_IPHONE
#import "UIImage+MGLAdditions.h"
#else
#import "NSImage+MGLAdditions.h"
#endif

const CGPoint MGLLogoImagePosition = CGPointMake(8, 8);
const CGFloat MGLSnapshotterMinimumPixelSize = 64;

@implementation MGLMapSnapshotOptions

- (instancetype _Nonnull)initWithStyleURL:(nullable NSURL *)styleURL camera:(MGLMapCamera *)camera size:(CGSize) size
{
    self = [super init];
    if (self) {
        if ( !styleURL)
        {
            styleURL = [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion];
        }
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

@interface MGLMapSnapshot()
- (instancetype)initWithImage:(nullable MGLImage *)image scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn;

@property (nonatomic) CGFloat scale;
@end

@implementation MGLMapSnapshot {
    mbgl::MapSnapshotter::PointForFn _pointForFn;
}
- (instancetype)initWithImage:(nullable MGLImage *)image scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn
{
    self = [super init];
    if (self) {
        _pointForFn = std::move(pointForFn);
        _scale = scale;
        _image = image;
    }
    return self;
}

- (CGPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate
{
    mbgl::ScreenCoordinate sc = _pointForFn(MGLLatLngFromLocationCoordinate2D(coordinate));
    return CGPointMake(sc.x * self.scale, sc.y * self.scale);
}
@end

@interface MGLMapSnapshotter()
@property (nonatomic) MGLMapSnapshotOptions *options;
@end

@implementation MGLMapSnapshotter {
    
    std::shared_ptr<mbgl::ThreadPool> _mbglThreadPool;
    std::unique_ptr<mbgl::MapSnapshotter> _mbglMapSnapshotter;
    std::unique_ptr<mbgl::Actor<mbgl::MapSnapshotter::Callback>> _snapshotCallback;
}

- (instancetype)initWithOptions:(MGLMapSnapshotOptions *)options
{
    self = [super init];
    if (self) {
        _options = options;
        _loading = false;
        
        mbgl::DefaultFileSource *mbglFileSource = [MGLOfflineStorage sharedOfflineStorage].mbglFileSource;
        _mbglThreadPool = mbgl::sharedThreadPool();
        
        std::string styleURL = std::string([options.styleURL.absoluteString UTF8String]);
        
        // Size; taking into account the minimum texture size for OpenGL ES
        // For non retina screens the ratio is 1:1 MGLSnapshotterMinimumPixelSize
        mbgl::Size size = {
            static_cast<uint32_t>(MAX(options.size.width, MGLSnapshotterMinimumPixelSize)),
            static_cast<uint32_t>(MAX(options.size.height, MGLSnapshotterMinimumPixelSize))
        };
        
        float pixelRatio = MAX(options.scale, 1);
        
        // Camera options
        mbgl::CameraOptions cameraOptions;
        if (CLLocationCoordinate2DIsValid(options.camera.centerCoordinate)) {
            cameraOptions.center = MGLLatLngFromLocationCoordinate2D(options.camera.centerCoordinate);
        }
        cameraOptions.angle = MAX(0, options.camera.heading) * mbgl::util::DEG2RAD;
        cameraOptions.zoom = MAX(0, options.zoomLevel);
        cameraOptions.pitch = MAX(0, options.camera.pitch);
        
        // Region
        mbgl::optional<mbgl::LatLngBounds> coordinateBounds;
        if (!MGLCoordinateBoundsIsEmpty(options.coordinateBounds)) {
            coordinateBounds = MGLLatLngBoundsFromCoordinateBounds(options.coordinateBounds);
        }
        
        // Create the snapshotter
        _mbglMapSnapshotter = std::make_unique<mbgl::MapSnapshotter>(*mbglFileSource, *_mbglThreadPool, styleURL, size, pixelRatio, cameraOptions, coordinateBounds);
    }
    return self;
}

- (void)startWithCompletionHandler:(MGLMapSnapshotCompletionHandler)completion
{
    [self startWithQueue:dispatch_get_main_queue() completionHandler:completion];
}

- (void)startWithQueue:(dispatch_queue_t)queue completionHandler:(MGLMapSnapshotCompletionHandler)completion
{
    if ([self isLoading]) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"Already started this snapshotter."];
    }
    
    _loading = true;
    
    dispatch_async(queue, ^{
        _snapshotCallback = std::make_unique<mbgl::Actor<mbgl::MapSnapshotter::Callback>>(*mbgl::Scheduler::GetCurrent(), [=](std::exception_ptr mbglError, mbgl::PremultipliedImage image, mbgl::MapSnapshotter::Attributions attributions, mbgl::MapSnapshotter::PointForFn pointForFn) {
            _loading = false;
            if (mbglError) {
                NSString *description = @(mbgl::util::toString(mbglError).c_str());
                NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description};
                NSError *error = [NSError errorWithDomain:MGLErrorDomain code:MGLErrorCodeSnapshotFailed userInfo:userInfo];
                
                // Dispatch result to origin queue
                dispatch_async(queue, ^{
                    completion(nil, error);
                });
            } else {
#if TARGET_OS_IPHONE
                MGLImage *mglImage = [[MGLImage alloc] initWithMGLPremultipliedImage:std::move(image) scale:self.options.scale];
#else
                MGLImage *mglImage = [[MGLImage alloc] initWithMGLPremultipliedImage:std::move(image)];
                mglImage.size = NSMakeSize(mglImage.size.width / self.options.scale,
                                           mglImage.size.height / self.options.scale);
#endif
                
                // Process image watermark in a work queue
                dispatch_queue_t workQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
                dispatch_async(workQueue, ^{
#if TARGET_OS_IPHONE
                    UIImage *logoImage = [UIImage imageNamed:@"mapbox" inBundle:[NSBundle mgl_frameworkBundle] compatibleWithTraitCollection:nil];
                    
                    UIGraphicsBeginImageContextWithOptions(mglImage.size, NO, self.options.scale);
                    
                    [mglImage drawInRect:CGRectMake(0, 0, mglImage.size.width, mglImage.size.height)];
                    [logoImage drawInRect:CGRectMake(MGLLogoImagePosition.x, mglImage.size.height - (MGLLogoImagePosition.y + logoImage.size.height), logoImage.size.width,logoImage.size.height)];
                    UIImage *compositedImage = UIGraphicsGetImageFromCurrentImageContext();
                    
                    UIGraphicsEndImageContext();
#else
                    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
                    NSImage *sourceImage = mglImage;
                    
                    NSSize targetSize = NSMakeSize(self.options.size.width, self.options.size.height);
                    NSRect targetFrame = NSMakeRect(0, 0, targetSize.width, targetSize.height);
                    NSImage *compositedImage = nil;
                    NSImageRep *sourceImageRep = [sourceImage bestRepresentationForRect:targetFrame
                                                                                context:nil
                                                                                  hints:nil];
                    compositedImage = [[NSImage alloc] initWithSize:targetSize];
                    
                    [compositedImage lockFocus];
                    [sourceImageRep drawInRect: targetFrame];
                    [logoImage drawInRect:CGRectMake(MGLLogoImagePosition.x, MGLLogoImagePosition.y, logoImage.size.width,logoImage.size.height)];
                    [compositedImage unlockFocus];
                    
#endif
                    
                    // Dispatch result to origin queue
                    dispatch_async(queue, ^{
                        MGLMapSnapshot* snapshot = [[MGLMapSnapshot alloc] initWithImage:compositedImage scale:self.options.scale pointForFn:pointForFn];
                        completion(snapshot, nil);
                    });
                });
            }
        });
        _mbglMapSnapshotter->snapshot(_snapshotCallback->self());
    });
}

- (void)cancel
{
    _snapshotCallback.reset();
    _mbglMapSnapshotter.reset();
}

- (NSURL *)styleURL
{
    NSString *styleURLString = @(_mbglMapSnapshotter->getStyleURL().c_str());
    return styleURLString.length ? [NSURL URLWithString:styleURLString] : nil;
}

- (void)setStyleURL:(NSURL *)url
{
    _mbglMapSnapshotter->setStyleURL(std::string([url.absoluteString UTF8String]));
}

- (CGSize)size
{
    mbgl::Size size = _mbglMapSnapshotter->getSize();
    return CGSizeMake(size.width, size.height);
}

- (void)setSize:(CGSize)size
{
    _mbglMapSnapshotter->setSize({
        static_cast<uint32_t>(MAX(size.width, MGLSnapshotterMinimumPixelSize)),
        static_cast<uint32_t>(MAX(size.height, MGLSnapshotterMinimumPixelSize))
    });
}

- (MGLMapCamera *)camera
{
    mbgl::CameraOptions cameraOptions = _mbglMapSnapshotter->getCameraOptions();
    CGFloat pitch = *cameraOptions.pitch;
    CLLocationDirection heading = mbgl::util::wrap(*cameraOptions.angle, 0., 360.);
    CLLocationDistance distance = MGLAltitudeForZoomLevel(*cameraOptions.zoom, pitch, cameraOptions.center->latitude(), [self size]);
    return [MGLMapCamera cameraLookingAtCenterCoordinate:MGLLocationCoordinate2DFromLatLng(*cameraOptions.center)
                                                   fromDistance:distance
                                                          pitch:pitch
                                                        heading:heading];
}

- (void)setCamera:(MGLMapCamera *)camera
{
    mbgl::CameraOptions cameraOptions;
    CLLocationCoordinate2D center;
    if (CLLocationCoordinate2DIsValid(camera.centerCoordinate)) {
        cameraOptions.center = MGLLatLngFromLocationCoordinate2D(camera.centerCoordinate);
        center = camera.centerCoordinate;
    } else {
        // Center is optional, but always set.
        center = MGLLocationCoordinate2DFromLatLng(*_mbglMapSnapshotter->getCameraOptions().center);
    }
    
    cameraOptions.angle = MAX(0, camera.heading) * mbgl::util::DEG2RAD;
    cameraOptions.zoom = MAX(0, MGLZoomLevelForAltitude(camera.altitude, camera.pitch, center.latitude, [self size]));
    cameraOptions.pitch = MAX(0, camera.pitch);
}

- (double)zoomLevel
{
    mbgl::CameraOptions cameraOptions = _mbglMapSnapshotter->getCameraOptions();
    return MGLAltitudeForZoomLevel(*cameraOptions.zoom, *cameraOptions.pitch, cameraOptions.center->latitude(), [self size]);
}

- (void)setZoomLevel:(double)zoomLevel
{
    mbgl::CameraOptions cameraOptions = _mbglMapSnapshotter->getCameraOptions();
    cameraOptions.zoom = zoomLevel;
    _mbglMapSnapshotter->setCameraOptions(cameraOptions);
}

- (MGLCoordinateBounds)coordinateBounds
{
    return MGLCoordinateBoundsFromLatLngBounds(_mbglMapSnapshotter->getRegion());
}

- (void)setCoordinateBounds:(MGLCoordinateBounds)coordinateBounds
{
    _mbglMapSnapshotter->setRegion(MGLLatLngBoundsFromCoordinateBounds(coordinateBounds));
}

@end
