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

@end

@implementation MGLMapSnapshotter {
    
    std::shared_ptr<mbgl::ThreadPool> _mbglThreadPool;
    std::unique_ptr<mbgl::MapSnapshotter> _mbglMapSnapshotter;
    std::unique_ptr<mbgl::Actor<mbgl::MapSnapshotter::Callback>> _snapshotCallback;
    NS_ARRAY_OF(MGLAttributionInfo *) *_attributionInfo;
}

- (instancetype)initWithOptions:(MGLMapSnapshotOptions *)options
{
    self = [super init];
    if (self) {
        [self setOptions:options];
        _loading = false;

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
            
            _attributionInfo = infos;
            
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
                    MGLAttributionInfoStyle attributionInfoStyle = MGLAttributionInfoStyleLong;
                    for (NSUInteger styleValue = MGLAttributionInfoStyleLong; styleValue >= MGLAttributionInfoStyleShort; styleValue--) {
                        attributionInfoStyle = (MGLAttributionInfoStyle)styleValue;
                        CGSize attributionSize = [self attributionSizeWithLogoStyle:attributionInfoStyle sourceAttributionStyle:attributionInfoStyle];
                        if (attributionSize.width <= mglImage.size.width) {
                            break;
                        }
                    }
                    
                    UIImage *logoImage = [self logoImageWithStyle:attributionInfoStyle];
                    CGSize attributionBackgroundSize = [self attributionTextSizeWithStyle:attributionInfoStyle];
                    
                    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, mglImage.size.height - (MGLLogoImagePosition.y + logoImage.size.height), logoImage.size.width, logoImage.size.height);
                    CGPoint attributionOrigin = CGPointMake(mglImage.size.width - 10 - attributionBackgroundSize.width,
                                                            logoImageRect.origin.y + (logoImageRect.size.height / 2) - (attributionBackgroundSize.height / 2) + 1);
                    if (!logoImage) {
                        CGSize defaultLogoSize = [self mapboxLongStyleLogo].size;
                        logoImageRect = CGRectMake(0, mglImage.size.height - (MGLLogoImagePosition.y + defaultLogoSize.height), 0, defaultLogoSize.height);
                        attributionOrigin = CGPointMake(10, logoImageRect.origin.y + (logoImageRect.size.height / 2) - (attributionBackgroundSize.height / 2) + 1);
                    }
                    
                    CGRect attributionBackgroundFrame = CGRectMake(attributionOrigin.x,
                                                                   attributionOrigin.y,
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
                    
                    [self drawAttributionTextWithStyle:attributionInfoStyle origin:attributionTextPosition];
                    
                    UIImage *compositedImage = UIGraphicsGetImageFromCurrentImageContext();
                    
                    UIGraphicsEndImageContext();
#else
                    NSSize targetSize = NSMakeSize(self.options.size.width, self.options.size.height);
                    NSRect targetFrame = NSMakeRect(0, 0, targetSize.width, targetSize.height);
                    
                    MGLAttributionInfoStyle attributionInfoStyle = MGLAttributionInfoStyleLong;
                    for (NSUInteger styleValue = MGLAttributionInfoStyleLong; styleValue >= MGLAttributionInfoStyleShort; styleValue--) {
                        attributionInfoStyle = (MGLAttributionInfoStyle)styleValue;
                        CGSize attributionSize = [self attributionSizeWithLogoStyle:attributionInfoStyle sourceAttributionStyle:attributionInfoStyle];
                        if (attributionSize.width <= mglImage.size.width) {
                            break;
                        }
                    }
                    
                    NSImage *logoImage = [self logoImageWithStyle:attributionInfoStyle];
                    CGSize attributionBackgroundSize = [self attributionTextSizeWithStyle:attributionInfoStyle];
                    NSImage *sourceImage = mglImage;
                    
                    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, MGLLogoImagePosition.y, logoImage.size.width, logoImage.size.height);
                    CGPoint attributionOrigin = CGPointMake(targetFrame.size.width - 10 - attributionBackgroundSize.width,
                                                            MGLLogoImagePosition.y + 1);
                    if (!logoImage) {
                        CGSize defaultLogoSize = [self mapboxLongStyleLogo].size;
                        logoImageRect = CGRectMake(0, MGLLogoImagePosition.y, 0, defaultLogoSize.height);
                        attributionOrigin = CGPointMake(10, attributionOrigin.y);
                    }
                    
                    CGRect attributionBackgroundFrame = CGRectMake(attributionOrigin.x,
                                                                   attributionOrigin.y,
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
                    
                    if (logoImage) {
                        [logoImage drawInRect:logoImageRect];
                    }
                    
                    NSBitmapImageRep *attributionBackground = [[NSBitmapImageRep alloc] initWithFocusedViewRect:attributionBackgroundFrame];
                    
                    CIImage *attributionBackgroundImage = [[CIImage alloc] initWithCGImage:[attributionBackground CGImage]];
                    
                    NSImage *blurredAttributionBackground = [self blurredAttributionBackground:attributionBackgroundImage];

                    [blurredAttributionBackground drawInRect:attributionBackgroundFrame];
                    
                    [self drawAttributionTextWithStyle:attributionInfoStyle origin:attributionTextPosition];
                    
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

- (void)drawAttributionTextWithStyle:(MGLAttributionInfoStyle)attributionInfoStyle origin:(CGPoint)origin
{
    for (MGLAttributionInfo *info in _attributionInfo) {
        if (info.isFeedbackLink) {
            continue;
        }
        NSAttributedString *attribution = [info titleWithStyle:attributionInfoStyle];
        [attribution drawAtPoint:origin];
        
        origin.x += [attribution size].width + 10;
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

- (MGLImage *)logoImageWithStyle:(MGLAttributionInfoStyle)style
{
    MGLImage *logoImage;
    switch (style) {
        case MGLAttributionInfoStyleLong:
            logoImage = [self mapboxLongStyleLogo];
            break;
        case MGLAttributionInfoStyleMedium:
#if TARGET_OS_IPHONE
            logoImage = [UIImage imageNamed:@"mapbox_helmet" inBundle:[NSBundle mgl_frameworkBundle] compatibleWithTraitCollection:nil];
#else
            logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox_helmet" ofType:@"pdf"]];
#endif
            break;
        case MGLAttributionInfoStyleShort:
            logoImage = nil;
            break;
    }
    return logoImage;
}

- (MGLImage *)mapboxLongStyleLogo
{
    MGLImage *logoImage;
#if TARGET_OS_IPHONE
    logoImage =[UIImage imageNamed:@"mapbox" inBundle:[NSBundle mgl_frameworkBundle] compatibleWithTraitCollection:nil];
#else
    logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
#endif
    return logoImage;
}

- (CGSize)attributionSizeWithLogoStyle:(MGLAttributionInfoStyle)logoStyle sourceAttributionStyle:(MGLAttributionInfoStyle)attributionStyle
{
    MGLImage *logoImage = [self logoImageWithStyle:logoStyle];
    
    CGSize attributionBackgroundSize = [self attributionTextSizeWithStyle:attributionStyle];
    
    CGSize attributionSize = CGSizeZero;
    
    if (logoImage) {
        attributionSize.width = MGLLogoImagePosition.x + logoImage.size.width + 10;
    }
    attributionSize.width = attributionSize.width + 10 + attributionBackgroundSize.width + 10;
    attributionSize.height = MAX(logoImage.size.height, attributionBackgroundSize.height);
    
    return attributionSize;
}

- (CGSize)attributionTextSizeWithStyle:(MGLAttributionInfoStyle)attributionStyle
{
    CGSize attributionBackgroundSize = CGSizeMake(10, 0);
    for (MGLAttributionInfo *info in _attributionInfo) {
        if (info.isFeedbackLink) {
            continue;
        }
        CGSize attributionSize = [info titleWithStyle:attributionStyle].size;
        attributionBackgroundSize.width += attributionSize.width + 10;
        attributionBackgroundSize.height = MAX(attributionSize.height, attributionBackgroundSize.height);
    }
    
    return attributionBackgroundSize;
}

- (void)cancel
{
    _snapshotCallback.reset();
    _mbglMapSnapshotter.reset();
}

- (void)setOptions:(MGLMapSnapshotOptions *)options
{
    _options = options;
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

@end
