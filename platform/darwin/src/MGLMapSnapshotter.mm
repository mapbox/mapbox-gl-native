#import "MGLMapSnapshotter.h"

#import <mbgl/actor/actor.hpp>
#import <mbgl/actor/scheduler.hpp>
#import <mbgl/util/geo.hpp>
#import <mbgl/map/map_options.hpp>
#import <mbgl/map/map_snapshotter.hpp>
#import <mbgl/map/camera.hpp>
#import <mbgl/storage/resource_options.hpp>
#import <mbgl/storage/default_file_source.hpp>
#import <mbgl/util/string.hpp>

#import "MGLOfflineStorage_Private.h"
#import "MGLGeometry_Private.h"
#import "NSBundle+MGLAdditions.h"
#import "MGLStyle.h"
#import "MGLAttributionInfo_Private.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLRendererConfiguration.h"
#import "MGLMapSnapshotter_Private.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MGLMapboxEvents.h"
#endif

#if TARGET_OS_IPHONE
#import "UIImage+MGLAdditions.h"
#else
#import "NSImage+MGLAdditions.h"
#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>
#endif

const CGPoint MGLLogoImagePosition = CGPointMake(8, 8);
const CGFloat MGLSnapshotterMinimumPixelSize = 64;


@interface MGLMapSnapshotOverlay() <MGLMapSnapshotProtocol>
@property (nonatomic, assign) CGFloat scale;
- (instancetype)initWithContext:(CGContextRef)context scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn;

@end

@implementation MGLMapSnapshotOverlay {
    mbgl::MapSnapshotter::PointForFn _pointForFn;
    mbgl::MapSnapshotter::LatLngForFn _latLngForFn;
}

- (instancetype) initWithContext:(CGContextRef)context scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn {
    self = [super init];
    if (self) {
        _context = context;
        _scale = scale;
        _pointForFn = pointForFn;
        _latLngForFn = latLngForFn;
    }

    return self;
}

#if TARGET_OS_IPHONE

- (CGPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate
{
    mbgl::ScreenCoordinate sc = _pointForFn(MGLLatLngFromLocationCoordinate2D(coordinate));
    return CGPointMake(sc.x, sc.y);
}

- (CLLocationCoordinate2D)coordinateForPoint:(CGPoint)point
{
    mbgl::LatLng latLng = _latLngForFn(mbgl::ScreenCoordinate(point.x, point.y));
    return MGLLocationCoordinate2DFromLatLng(latLng);
}

#else

- (NSPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate
{
    mbgl::ScreenCoordinate sc = _pointForFn(MGLLatLngFromLocationCoordinate2D(coordinate));
    CGFloat height = ((CGFloat)CGBitmapContextGetHeight(self.context))/self.scale;
    return NSMakePoint(sc.x, height - sc.y);
}

- (CLLocationCoordinate2D)coordinateForPoint:(NSPoint)point
{
    CGFloat height = ((CGFloat)CGBitmapContextGetHeight(self.context))/self.scale;
    auto screenCoord = mbgl::ScreenCoordinate(point.x, height - point.y);
    mbgl::LatLng latLng = _latLngForFn(screenCoord);
    return MGLLocationCoordinate2DFromLatLng(latLng);
}

#endif
@end

@implementation MGLMapSnapshotOptions

- (instancetype _Nonnull)initWithStyleURL:(nullable NSURL *)styleURL camera:(MGLMapCamera *)camera size:(CGSize)size
{
    MGLLogDebug(@"Initializing withStyleURL: %@ camera: %@ size: %@", styleURL, camera, MGLStringFromSize(size));
    self = [super init];
    if (self)
    {
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

@interface MGLMapSnapshot() <MGLMapSnapshotProtocol>
- (instancetype)initWithImage:(nullable MGLImage *)image scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn;

@property (nonatomic) CGFloat scale;
@end

@implementation MGLMapSnapshot {
    mbgl::MapSnapshotter::PointForFn _pointForFn;
    mbgl::MapSnapshotter::LatLngForFn _latLngForFn;
}

- (instancetype)initWithImage:(nullable MGLImage *)image scale:(CGFloat)scale pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn
{
    self = [super init];
    if (self) {
        _pointForFn = pointForFn;
        _latLngForFn = latLngForFn;
        _scale = scale;
        _image = image;
    }
    return self;
}

#if TARGET_OS_IPHONE

- (CGPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate
{
    mbgl::ScreenCoordinate sc = _pointForFn(MGLLatLngFromLocationCoordinate2D(coordinate));
    return CGPointMake(sc.x, sc.y);
}

- (CLLocationCoordinate2D)coordinateForPoint:(CGPoint)point
{
    mbgl::LatLng latLng = _latLngForFn(mbgl::ScreenCoordinate(point.x, point.y));
    return MGLLocationCoordinate2DFromLatLng(latLng);
}

#else

- (NSPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate
{
    mbgl::ScreenCoordinate sc = _pointForFn(MGLLatLngFromLocationCoordinate2D(coordinate));
    return NSMakePoint(sc.x, self.image.size.height - sc.y);
}

- (CLLocationCoordinate2D)coordinateForPoint:(NSPoint)point
{
    auto screenCoord = mbgl::ScreenCoordinate(point.x, self.image.size.height - point.y);
    mbgl::LatLng latLng = _latLngForFn(screenCoord);
    return MGLLocationCoordinate2DFromLatLng(latLng);
}

#endif

@end

@interface MGLMapSnapshotter()
@property (nonatomic) BOOL cancelled;
@property (nonatomic) BOOL terminated;
@property (nonatomic) dispatch_queue_t resultQueue;
@property (nonatomic, copy) MGLMapSnapshotCompletionHandler completion;
+ (void)completeWithErrorCode:(MGLErrorCode)errorCode description:(nonnull NSString*)description onQueue:(dispatch_queue_t)queue completion:(MGLMapSnapshotCompletionHandler)completion;
@end

@implementation MGLMapSnapshotter {
    std::unique_ptr<mbgl::MapSnapshotter> _mbglMapSnapshotter;
    std::unique_ptr<mbgl::Actor<mbgl::MapSnapshotter::Callback>> _snapshotCallback;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    if (_completion) {
        MGLAssert(_snapshotCallback, @"Snapshot in progress - there should be a valid callback");

        [MGLMapSnapshotter completeWithErrorCode:MGLErrorCodeSnapshotFailed
                                     description:@"MGLMapSnapshotter deallocated prior to snapshot completion."
                                         onQueue:_resultQueue
                                      completion:_completion];
    }
}


- (instancetype)init {
    NSAssert(NO, @"Please use -[MGLMapSnapshotter initWithOptions:]");
    [super doesNotRecognizeSelector:_cmd];
    return nil;
}

- (instancetype)initWithOptions:(MGLMapSnapshotOptions *)options
{
    MGLLogDebug(@"Initializing withOptions: %@", options);
    self = [super init];
    if (self) {
        [self setOptions:options];
#if TARGET_OS_IPHONE
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillTerminate:) name:UIApplicationWillTerminateNotification object:nil];
#else
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillTerminate:) name:NSApplicationWillTerminateNotification object:nil];
#endif
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        [MGLMapboxEvents pushTurnstileEvent];
#endif
    }
    return self;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    if (self.completion) {
        [self cancel];
    }

    _mbglMapSnapshotter.reset();
    _snapshotCallback.reset();
    
    self.terminated = YES;
}

- (void)startWithCompletionHandler:(MGLMapSnapshotCompletionHandler)completion
{
    MGLLogDebug(@"Starting withCompletionHandler: %@", completion);
    [self startWithQueue:dispatch_get_main_queue() completionHandler:completion];
}

- (void)startWithQueue:(dispatch_queue_t)queue completionHandler:(MGLMapSnapshotCompletionHandler)completionHandler {
    [self startWithQueue:queue overlayHandler:nil completionHandler:completionHandler];
}

- (void)startWithOverlayHandler:(MGLMapSnapshotOverlayHandler)overlayHandler completionHandler:(MGLMapSnapshotCompletionHandler)completion {
    [self startWithQueue:dispatch_get_main_queue() overlayHandler:overlayHandler completionHandler:completion];
}

- (void)startWithQueue:(dispatch_queue_t)queue overlayHandler:(MGLMapSnapshotOverlayHandler)overlayHandler completionHandler:(MGLMapSnapshotCompletionHandler)completion
{
    if (!mbgl::Scheduler::GetCurrent()) {
        [NSException raise:NSInvalidArgumentException
                    format:@"startWithQueue:completionHandler: must be called from a thread with an active run loop."];
    }

    if (self.completion) {
        // Consider replacing this exception with an error passed to the completion block.
        [NSException raise:NSInternalInconsistencyException
                    format:@"Already started this snapshotter."];
    }

    if (self.terminated) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"Starting a snapshotter after application termination is not supported."];
    }

    self.completion = completion;
    self.resultQueue = queue;
    self.cancelled = NO;

    __weak __typeof__(self) weakSelf = self;
    // mbgl::Scheduler::GetCurrent() scheduler means "run callback on current (ie UI/main) thread"
    // capture weakSelf to avoid retain cycle if callback is never called (ie snapshot cancelled)

    _snapshotCallback = std::make_unique<mbgl::Actor<mbgl::MapSnapshotter::Callback>>(
                                                        *mbgl::Scheduler::GetCurrent(),
                                                        [=](std::exception_ptr mbglError, mbgl::PremultipliedImage image, mbgl::MapSnapshotter::Attributions attributions, mbgl::MapSnapshotter::PointForFn pointForFn, mbgl::MapSnapshotter::LatLngForFn latLngForFn) {

        __typeof__(self) strongSelf = weakSelf;
        // If self had died, _snapshotCallback would have been destroyed and this block would not be executed
        MGLCAssert(strongSelf, @"Snapshot callback executed after being destroyed.");

        if (!strongSelf.completion)
            return;

        if (mbglError) {
            NSString *description = @(mbgl::util::toString(mbglError).c_str());
            NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description};
            NSError *error = [NSError errorWithDomain:MGLErrorDomain code:MGLErrorCodeSnapshotFailed userInfo:userInfo];
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
            [[MMEEventsManager sharedManager] reportError:error];
#endif
            // Dispatch to result queue
            dispatch_async(queue, ^{
                strongSelf.completion(nil, error);
                strongSelf.completion = nil;
            });
          
        } else {
#if TARGET_OS_IPHONE
            MGLImage *mglImage = [[MGLImage alloc] initWithMGLPremultipliedImage:std::move(image) scale:strongSelf.options.scale];
#else
            MGLImage *mglImage = [[MGLImage alloc] initWithMGLPremultipliedImage:std::move(image)];
            mglImage.size = NSMakeSize(mglImage.size.width / strongSelf.options.scale,
                                       mglImage.size.height / strongSelf.options.scale);
#endif

            [strongSelf drawAttributedSnapshot:attributions snapshotImage:mglImage pointForFn:pointForFn latLngForFn:latLngForFn overlayHandler:overlayHandler];
        }
        strongSelf->_snapshotCallback = NULL;

      });

    // Launches snapshot on background Thread owned by mbglMapSnapshotter
    // _snapshotCallback->self() is an ActorRef: if the callback is destroyed, further messages
    // to the callback are just no-ops
    _mbglMapSnapshotter->snapshot(_snapshotCallback->self());
}

+ (MGLImage*)drawAttributedSnapshotWorker:(mbgl::MapSnapshotter::Attributions)attributions snapshotImage:(MGLImage *)mglImage pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn scale:(CGFloat)scale size:(CGSize)size overlayHandler:(MGLMapSnapshotOverlayHandler)overlayHandler {

    NSArray<MGLAttributionInfo *>* attributionInfo = [MGLMapSnapshotter generateAttributionInfos:attributions];

#if TARGET_OS_IPHONE
    MGLAttributionInfoStyle attributionInfoStyle = MGLAttributionInfoStyleLong;
    for (NSUInteger styleValue = MGLAttributionInfoStyleLong; styleValue >= MGLAttributionInfoStyleShort; styleValue--) {
        attributionInfoStyle = (MGLAttributionInfoStyle)styleValue;
        CGSize attributionSize = [MGLMapSnapshotter attributionSizeWithLogoStyle:attributionInfoStyle sourceAttributionStyle:attributionInfoStyle attributionInfo:attributionInfo];
        if (attributionSize.width <= mglImage.size.width) {
            break;
        }
    }
    
    UIImage *logoImage = [MGLMapSnapshotter logoImageWithStyle:attributionInfoStyle];
    CGSize attributionBackgroundSize = [MGLMapSnapshotter attributionTextSizeWithStyle:attributionInfoStyle attributionInfo:attributionInfo];
    
    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, mglImage.size.height - (MGLLogoImagePosition.y + logoImage.size.height), logoImage.size.width, logoImage.size.height);
    CGPoint attributionOrigin = CGPointMake(mglImage.size.width - 10 - attributionBackgroundSize.width,
                                            logoImageRect.origin.y + (logoImageRect.size.height / 2) - (attributionBackgroundSize.height / 2) + 1);
    if (!logoImage) {
        CGSize defaultLogoSize = [MGLMapSnapshotter mapboxLongStyleLogo].size;
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
    
    
    UIGraphicsBeginImageContextWithOptions(mglImage.size, NO, scale);
    
    [mglImage drawInRect:CGRectMake(0, 0, mglImage.size.width, mglImage.size.height)];

    CGContextRef currentContext = UIGraphicsGetCurrentContext();

    if (currentContext && overlayHandler) {
        MGLMapSnapshotOverlay *snapshotOverlay = [[MGLMapSnapshotOverlay alloc] initWithContext:currentContext
                                                                                          scale:scale
                                                                                     pointForFn:pointForFn
                                                                                    latLngForFn:latLngForFn];
        CGContextSaveGState(snapshotOverlay.context);
        overlayHandler(snapshotOverlay);
        CGContextRestoreGState(snapshotOverlay.context);
        currentContext = UIGraphicsGetCurrentContext();
    }

    if (!currentContext && overlayHandler) {
        // If the current context has been corrupted by the user,
        // return nil so we can generate an error later.
        return nil;
    }

    [logoImage drawInRect:logoImageRect];
    
    UIImage *currentImage = UIGraphicsGetImageFromCurrentImageContext();
    CGImageRef attributionImageRef = CGImageCreateWithImageInRect([currentImage CGImage], cropRect);
    UIImage *attributionImage = [UIImage imageWithCGImage:attributionImageRef];
    CGImageRelease(attributionImageRef);
    
    CIImage *ciAttributionImage = [[CIImage alloc] initWithCGImage:attributionImage.CGImage];
    
    UIImage *blurredAttributionBackground = [MGLMapSnapshotter blurredAttributionBackground:ciAttributionImage];
    
    [blurredAttributionBackground drawInRect:attributionBackgroundFrame];
    
    [MGLMapSnapshotter drawAttributionTextWithStyle:attributionInfoStyle origin:attributionTextPosition attributionInfo:attributionInfo];
    
    UIImage *compositedImage = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();

    return compositedImage;

#else

    NSSize targetSize = NSMakeSize(size.width, size.height);
    NSRect targetFrame = NSMakeRect(0, 0, targetSize.width, targetSize.height);
    
    MGLAttributionInfoStyle attributionInfoStyle = MGLAttributionInfoStyleLong;
    for (NSUInteger styleValue = MGLAttributionInfoStyleLong; styleValue >= MGLAttributionInfoStyleShort; styleValue--) {
        attributionInfoStyle = (MGLAttributionInfoStyle)styleValue;
        CGSize attributionSize = [MGLMapSnapshotter attributionSizeWithLogoStyle:attributionInfoStyle sourceAttributionStyle:attributionInfoStyle attributionInfo:attributionInfo];
        if (attributionSize.width <= mglImage.size.width) {
            break;
        }
    }
    
    NSImage *logoImage = [MGLMapSnapshotter logoImageWithStyle:attributionInfoStyle];
    CGSize attributionBackgroundSize = [MGLMapSnapshotter attributionTextSizeWithStyle:attributionInfoStyle attributionInfo:attributionInfo];
    NSImage *sourceImage = mglImage;
    
    CGRect logoImageRect = CGRectMake(MGLLogoImagePosition.x, MGLLogoImagePosition.y, logoImage.size.width, logoImage.size.height);
    CGPoint attributionOrigin = CGPointMake(targetFrame.size.width - 10 - attributionBackgroundSize.width,
                                            MGLLogoImagePosition.y + 1);
    if (!logoImage) {
        CGSize defaultLogoSize = [MGLMapSnapshotter mapboxLongStyleLogo].size;
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
    
    NSGraphicsContext *currentContext = [NSGraphicsContext currentContext];
    if (currentContext && overlayHandler) {
        MGLMapSnapshotOverlay *snapshotOverlay = [[MGLMapSnapshotOverlay alloc] initWithContext:currentContext.CGContext
                                                                                          scale:scale
                                                                                     pointForFn:pointForFn
                                                                                    latLngForFn:latLngForFn];
        [currentContext saveGraphicsState];
        overlayHandler(snapshotOverlay);
        [currentContext restoreGraphicsState];
        currentContext = [NSGraphicsContext currentContext];
    }
    
    if (!currentContext && overlayHandler) {
        // If the current context has been corrupted by the user,
        // return nil so we can generate an error later.
        return nil;
    }
    
    if (logoImage) {
        [logoImage drawInRect:logoImageRect];
    }
    
    NSBitmapImageRep *attributionBackground = [[NSBitmapImageRep alloc] initWithFocusedViewRect:attributionBackgroundFrame];
    
    CIImage *attributionBackgroundImage = [[CIImage alloc] initWithCGImage:[attributionBackground CGImage]];
    
    NSImage *blurredAttributionBackground = [MGLMapSnapshotter blurredAttributionBackground:attributionBackgroundImage];
    
    [blurredAttributionBackground drawInRect:attributionBackgroundFrame];
    
    [MGLMapSnapshotter drawAttributionTextWithStyle:attributionInfoStyle origin:attributionTextPosition attributionInfo:attributionInfo];
    
    [compositedImage unlockFocus];

    return compositedImage;
#endif
}

- (void)drawAttributedSnapshot:(mbgl::MapSnapshotter::Attributions)attributions snapshotImage:(MGLImage *)mglImage pointForFn:(mbgl::MapSnapshotter::PointForFn)pointForFn latLngForFn:(mbgl::MapSnapshotter::LatLngForFn)latLngForFn overlayHandler:(MGLMapSnapshotOverlayHandler)overlayHandler {

    // Process image watermark in a work queue
    dispatch_queue_t workQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_queue_t resultQueue = self.resultQueue;

    // Capture scale and size by value to avoid accessing self from another thread
    CGFloat scale = self.options.scale;
    CGSize size = self.options.size;

    // pointForFn is a copyable std::function that captures state by value: see MapSnapshotter::Impl::snapshot
    __weak __typeof__(self) weakself = self;

    dispatch_async(workQueue, ^{
        // Call a class method to ensure we're not accidentally capturing self
        MGLImage *compositedImage = [MGLMapSnapshotter drawAttributedSnapshotWorker:attributions snapshotImage:mglImage pointForFn:pointForFn latLngForFn:latLngForFn scale:scale size:size overlayHandler:overlayHandler];

        // Dispatch result to origin queue
        dispatch_async(resultQueue, ^{
            __typeof__(self) strongself = weakself;

            if (strongself.completion) {

                if (!compositedImage) {
                    NSDictionary *userInfo = @{NSLocalizedDescriptionKey: @"Failed to generate composited snapshot."};
                    NSError *error = [NSError errorWithDomain:MGLErrorDomain
                                                         code:MGLErrorCodeSnapshotFailed
                                                     userInfo:userInfo];

                    strongself.completion(nil, error);
                    strongself.completion = nil;
                } else {
                    MGLMapSnapshot* snapshot = [[MGLMapSnapshot alloc] initWithImage:compositedImage
                                                                               scale:scale
                                                                          pointForFn:pointForFn
                                                                         latLngForFn:latLngForFn];
                    strongself.completion(snapshot, nil);
                    strongself.completion = nil;
                }
            }
        });
    });
}

+ (NSArray<MGLAttributionInfo *>*) generateAttributionInfos:(mbgl::MapSnapshotter::Attributions)attributions {
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
    return infos;
}

+ (void)drawAttributionTextWithStyle:(MGLAttributionInfoStyle)attributionInfoStyle origin:(CGPoint)origin attributionInfo:(NSArray<MGLAttributionInfo *>*)attributionInfo
{
    for (MGLAttributionInfo *info in attributionInfo) {
        if (info.isFeedbackLink) {
            continue;
        }
        NSAttributedString *attribution = [info titleWithStyle:attributionInfoStyle];
        [attribution drawAtPoint:origin];
        
        origin.x += [attribution size].width + 10;
    }
}

+ (MGLImage *)blurredAttributionBackground:(CIImage *)backgroundImage
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

+ (MGLImage *)logoImageWithStyle:(MGLAttributionInfoStyle)style
{
    MGLImage *logoImage;
    switch (style) {
        case MGLAttributionInfoStyleLong:
            logoImage = [MGLMapSnapshotter mapboxLongStyleLogo];
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

+ (MGLImage *)mapboxLongStyleLogo
{
    MGLImage *logoImage;
#if TARGET_OS_IPHONE
    logoImage =[UIImage imageNamed:@"mapbox" inBundle:[NSBundle mgl_frameworkBundle] compatibleWithTraitCollection:nil];
#else
    logoImage = [[NSImage alloc] initWithContentsOfFile:[[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
#endif
    return logoImage;
}

+ (CGSize)attributionSizeWithLogoStyle:(MGLAttributionInfoStyle)logoStyle sourceAttributionStyle:(MGLAttributionInfoStyle)attributionStyle attributionInfo:(NSArray<MGLAttributionInfo *>*)attributionInfo
{
    MGLImage *logoImage = [self logoImageWithStyle:logoStyle];
    
    CGSize attributionBackgroundSize = [MGLMapSnapshotter attributionTextSizeWithStyle:attributionStyle attributionInfo:attributionInfo];
    
    CGSize attributionSize = CGSizeZero;
    
    if (logoImage) {
        attributionSize.width = MGLLogoImagePosition.x + logoImage.size.width + 10;
    }
    attributionSize.width = attributionSize.width + 10 + attributionBackgroundSize.width + 10;
    attributionSize.height = MAX(logoImage.size.height, attributionBackgroundSize.height);
    
    return attributionSize;
}

+ (CGSize)attributionTextSizeWithStyle:(MGLAttributionInfoStyle)attributionStyle attributionInfo:(NSArray<MGLAttributionInfo *>*)attributionInfo
{
    CGSize attributionBackgroundSize = CGSizeMake(10, 0);
    for (MGLAttributionInfo *info in attributionInfo) {
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
    MGLLogInfo(@"Cancelling snapshotter.");
    self.cancelled = YES;
    
    if (_snapshotCallback) {
        [MGLMapSnapshotter completeWithErrorCode:MGLErrorCodeSnapshotFailed
                                     description:[NSString stringWithFormat:@"MGLMapSnapshotter cancelled from %s", __PRETTY_FUNCTION__]
                                         onQueue:self.resultQueue
                                      completion:self.completion];
        self.completion = nil;
    }

    _snapshotCallback.reset();
    _mbglMapSnapshotter.reset();
}

+ (void)completeWithErrorCode:(MGLErrorCode)errorCode description:(nonnull NSString*)description onQueue:(dispatch_queue_t)queue completion:(MGLMapSnapshotCompletionHandler)completion {
    // The snapshot hasn't completed, so we should alert the caller
    if (completion && queue) {
        dispatch_async(queue, ^{
            NSDictionary *userInfo = @{NSLocalizedDescriptionKey: description};
            NSError *error = [NSError errorWithDomain:MGLErrorDomain
                                                 code:errorCode
                                             userInfo:userInfo];
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
            [[MMEEventsManager sharedManager] reportError:error];
#endif
            completion(NULL, error);
        });
    }
}

- (void)setOptions:(MGLMapSnapshotOptions *)options
{
    if (_terminated) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"Calling MGLMapSnapshotter.options after application termination is not supported."];
    }

    MGLLogDebug(@"Setting options: %@", options);

    if (_completion) {
        [self cancel];
    }
    
    _cancelled = NO;
    _options = options;

    auto mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
    
    std::string styleURL = std::string([options.styleURL.absoluteString UTF8String]);
    std::pair<bool, std::string> style = std::make_pair(false, styleURL);
    
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
    cameraOptions.bearing = MAX(0, options.camera.heading);
    cameraOptions.zoom = MAX(0, options.zoomLevel);
    cameraOptions.pitch = MAX(0, options.camera.pitch);
    
    // Region
    mbgl::optional<mbgl::LatLngBounds> coordinateBounds;
    if (!MGLCoordinateBoundsIsEmpty(options.coordinateBounds)) {
        coordinateBounds = MGLLatLngBoundsFromCoordinateBounds(options.coordinateBounds);
    }
    
    // App-global configuration
    MGLRendererConfiguration* config = [MGLRendererConfiguration currentConfiguration];

    mbgl::ResourceOptions resourceOptions;
    resourceOptions.withCachePath([[MGLOfflineStorage sharedOfflineStorage] mbglCachePath])
                   .withAssetPath([NSBundle mainBundle].resourceURL.path.UTF8String);

    // Create the snapshotter
    _mbglMapSnapshotter = std::make_unique<mbgl::MapSnapshotter>(
        style, size, pixelRatio, cameraOptions, coordinateBounds, config.localFontFamilyName, resourceOptions);
}

@end
