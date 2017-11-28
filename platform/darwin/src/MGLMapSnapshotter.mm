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
#import "MGLAttributionInfo_Private.h"

#if TARGET_OS_IPHONE
#import "UIImage+MGLAdditions.h"
#else
#import "NSImage+MGLAdditions.h"
#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>
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
            
            NSMutableArray *infos = [NSMutableArray array];

#if TARGET_OS_IPHONE
            CGFloat fontSize = [UIFont smallSystemFontSize];
            UIColor *attributeFontColor = [UIColor blackColor];
#else
            CGFloat fontSize = [NSFont systemFontSizeForControlSize:NSMiniControlSize];
            NSColor *attributeFontColor = [NSColor blackColor];
#endif
            for (auto attribution = attributions.begin(); attribution != attributions.end(); ++attribution) {
                NSString *attributionHTMLString = @(attribution->c_str());
                NSArray *tileSetInfos = [MGLAttributionInfo attributionInfosFromHTMLString:attributionHTMLString
                                                                                  fontSize:fontSize
                                                                                 linkColor:attributeFontColor];
                [infos growArrayByAddingAttributionInfosFromArray:tileSetInfos];
            }
            
            CGSize attributionBackgroundSize = CGSizeMake(10, 0);
            for (MGLAttributionInfo *info in infos) {
                if (info.isFeedbackLink) {
                    continue;
                }
                attributionBackgroundSize.width += [info.title size].width + 10;
                attributionBackgroundSize.height = MAX([info.title size].height, attributionBackgroundSize.height);
            }
            
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
                    
                    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, mglImage.size.height - (MGLLogoImagePosition.y + logoImage.size.height), logoImage.size.width, logoImage.size.height);
                    CGRect attributionBackgroundFrame = CGRectMake(mglImage.size.width - 10 - attributionBackgroundSize.width,
                                                                   logoImageRect.origin.y + (logoImageRect.size.height / 2) - (attributionBackgroundSize.height / 2) + 1,
                                                                   attributionBackgroundSize.width,
                                                                   attributionBackgroundSize.height);
                    CGPoint attributionTextPosition = CGPointMake(attributionBackgroundFrame.origin.x + 10,
                                                                  attributionBackgroundFrame.origin.y - 1);
                    
                    CGRect cropRect = CGRectMake(attributionBackgroundFrame.origin.x * mglImage.scale,
                                                 attributionBackgroundFrame.origin.y * mglImage.scale,
                                                 attributionBackgroundSize.width * mglImage.scale,
                                                 attributionBackgroundSize.height * mglImage.scale);
                    
                    
                    UIGraphicsBeginImageContextWithOptions(mglImage.size, NO, self.options.scale);
                    
                    [mglImage drawInRect:CGRectMake(0, 0, mglImage.size.width, mglImage.size.height)];
                    
                    [logoImage drawInRect:logoImageRect];
                    
                    UIImage *currentImage = UIGraphicsGetImageFromCurrentImageContext();
                    CGImageRef attributionImageRef = CGImageCreateWithImageInRect([currentImage CGImage], cropRect);
                    UIImage *attributionImage = [UIImage imageWithCGImage:attributionImageRef];
                    CGImageRelease(attributionImageRef);
                    
                    CIImage *ciAttributionImage = [[CIImage alloc] initWithCGImage:attributionImage.CGImage];
                    
                    UIImage *blurredAttributionBackground = [self blurredAttributionBackground:ciAttributionImage];
                    
                    [blurredAttributionBackground drawInRect:attributionBackgroundFrame];
                    
                    [self drawAttributionText:infos origin:attributionTextPosition];
                    
                    UIImage *compositedImage = UIGraphicsGetImageFromCurrentImageContext();
                    
                    UIGraphicsEndImageContext();
#else
                    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
                    NSImage *sourceImage = mglImage;
                    
                    NSSize targetSize = NSMakeSize(self.options.size.width, self.options.size.height);
                    NSRect targetFrame = NSMakeRect(0, 0, targetSize.width, targetSize.height);
                    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, MGLLogoImagePosition.y, logoImage.size.width, logoImage.size.height);
                    CGRect attributionBackgroundFrame = CGRectMake(targetFrame.size.width - 10 - attributionBackgroundSize.width,
                                                                   MGLLogoImagePosition.y + 1,
                                                                   attributionBackgroundSize.width,
                                                                   attributionBackgroundSize.height);
                    CGPoint attributionTextPosition = CGPointMake(attributionBackgroundFrame.origin.x + 10,
                                                                  logoImageRect.origin.y + (logoImageRect.size.height / 2) - (attributionBackgroundSize.height / 2));
                    
                    
                    NSImage *compositedImage = nil;
                    NSImageRep *sourceImageRep = [sourceImage bestRepresentationForRect:targetFrame
                                                                                context:nil
                                                                                  hints:nil];
                    compositedImage = [[NSImage alloc] initWithSize:targetSize];
                    
                    [compositedImage lockFocus];

                    [sourceImageRep drawInRect: targetFrame];
                    
                    [logoImage drawInRect:logoImageRect];
                    
                    NSBitmapImageRep *attributionBackground = [[NSBitmapImageRep alloc] initWithFocusedViewRect:attributionBackgroundFrame];
                    
                    CIImage *attributionBackgroundImage = [[CIImage alloc] initWithCGImage:[attributionBackground CGImage]];
                    
                    NSImage *blurredAttributionBackground = [self blurredAttributionBackground:attributionBackgroundImage];

                    [blurredAttributionBackground drawInRect:attributionBackgroundFrame];
                    
                    [self drawAttributionText:infos origin:attributionTextPosition];
                    
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

- (void)drawAttributionText:(NSArray *)attributionInfo origin:(CGPoint)origin
{
    for (MGLAttributionInfo *info in attributionInfo) {
        if (info.isFeedbackLink) {
            continue;
        }
        [info.title drawAtPoint:origin];
        
        origin.x += [info.title size].width + 10;
    }
}

- (MGLImage *)blurredAttributionBackground:(CIImage *)backgroundImage
{
    CGAffineTransform transform = CGAffineTransformIdentity;
    CIFilter *clamp = [CIFilter filterWithName:@"CIAffineClamp"];
    [clamp setValue:backgroundImage forKey:kCIInputImageKey];
    [clamp setValue:[NSValue valueWithBytes:&transform objCType:@encode(CGAffineTransform)] forKey:@"inputTransform"];
    
    CIFilter *attributionBlurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
    [attributionBlurFilter setValue:[clamp outputImage] forKey:kCIInputImageKey];
    [attributionBlurFilter setValue:@10 forKey:kCIInputRadiusKey];
    
    CIFilter *attributionColorFilter = [CIFilter filterWithName:@"CIColorControls"];
    [attributionColorFilter setValue:[attributionBlurFilter outputImage] forKey:kCIInputImageKey];
    [attributionColorFilter setValue:@(0.1) forKey:kCIInputBrightnessKey];
    
    CIImage *blurredImage = attributionColorFilter.outputImage;
    
    CIContext *ctx = [CIContext contextWithOptions:nil];
    CGImageRef cgimg = [ctx createCGImage:blurredImage fromRect:[backgroundImage extent]];
    MGLImage *image;
    
#if TARGET_OS_IPHONE
    
    image = [UIImage imageWithCGImage:cgimg];
#else
    
    image = [[NSImage alloc] initWithCGImage:cgimg size:[backgroundImage extent].size];
#endif

    CGImageRelease(cgimg);
    return image;
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
