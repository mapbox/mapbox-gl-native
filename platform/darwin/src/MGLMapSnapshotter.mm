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
#import "MGLStyle_Private.h"
#import "MGLAttributionButton.h"
#import "MGLAttributionInfo.h"

#import <QuartzCore/QuartzCore.h>

#if TARGET_OS_IPHONE
#import "UIImage+MGLAdditions.h"
#else
#import "NSImage+MGLAdditions.h"
#endif

const CGPoint MGLLogoImagePosition = CGPointMake(8, 8);
const CGFloat MGLSnapshotterMinimumPixelSize = 64;

@implementation MGLMapSnapshotOptions

- (instancetype _Nonnull)initWithStyleURL:(nullable NSURL*)styleURL camera:(MGLMapCamera*)camera size:(CGSize) size;
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

@interface MGLMapSnapshotter()
@property (nonatomic) MGLMapSnapshotOptions *options;
@end

@implementation MGLMapSnapshotter {
    
    std::shared_ptr<mbgl::ThreadPool> _mbglThreadPool;
    std::unique_ptr<mbgl::MapSnapshotter> _mbglMapSnapshotter;
    std::unique_ptr<mbgl::Actor<mbgl::MapSnapshotter::Callback>> _snapshotCallback;
}

- (instancetype)initWithOptions:(MGLMapSnapshotOptions*)options;
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

- (void)startWithCompletionHandler:(MGLMapSnapshotCompletionHandler)completion;
{
    [self startWithQueue:dispatch_get_main_queue() completionHandler:completion];
}

- (void)startWithQueue:(dispatch_queue_t)queue completionHandler:(MGLMapSnapshotCompletionHandler)completion;
{
    if ([self isLoading]) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"Already started this snapshotter."];
    }
    
    _loading = true;
    
    dispatch_async(queue, ^{
        _snapshotCallback = std::make_unique<mbgl::Actor<mbgl::MapSnapshotter::Callback>>(*mbgl::Scheduler::GetCurrent(), [=](std::exception_ptr mbglError, mbgl::PremultipliedImage image) {
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
                        completion(compositedImage, nil);
                    });
                });
            }
        });
        _mbglMapSnapshotter->snapshot(_snapshotCallback->self());
    });
}

- (NSView *)attributionView
{
    NSView *attributionView = [[NSView alloc] initWithFrame:NSZeroRect];
    attributionView.wantsLayer = YES;
    
    // Make the background and foreground translucent to be unobtrusive.
    attributionView.layer.opacity = 0.6;
    
    // Blur the background to prevent text underneath the view from running into
    // the text in the view, rendering it illegible.
    CIFilter *attributionBlurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
    [attributionBlurFilter setDefaults];
    
    // Brighten the background. This is similar to applying a translucent white
    // background on the view, but the effect is a bit more subtle and works
    // well with the blur above.
    CIFilter *attributionColorFilter = [CIFilter filterWithName:@"CIColorControls"];
    [attributionColorFilter setDefaults];
    [attributionColorFilter setValue:@(0.1) forKey:kCIInputBrightnessKey];
    
    // Apply the background effects and a standard button corner radius.
    attributionView.backgroundFilters = @[attributionColorFilter, attributionBlurFilter];
    attributionView.layer.cornerRadius = 4;
    
    attributionView.translatesAutoresizingMaskIntoConstraints = NO;
    
    [self updateAttributionView:attributionView];
    
    return attributionView;
}

- (void)updateAttributionView:(NSView *)view
{
    NSView *attributionView = self.attributionView;
    for (NSView *button in attributionView.subviews) {
        [button removeConstraints:button.constraints];
    }
    attributionView.subviews = @[];
    [attributionView removeConstraints:attributionView.constraints];
    
    // Make the whole string mini by default.
    // Force links to be black, because the default blue is distracting.
    CGFloat miniSize = [NSFont systemFontSizeForControlSize:NSMiniControlSize];
    NSArray *attributionInfos = [self.style attributionInfosWithFontSize:miniSize linkColor:[NSColor blackColor]];
    for (MGLAttributionInfo *info in attributionInfos) {
        // Feedback links are added to the Help menu.
        if (info.feedbackLink) {
            continue;
        }
        
        // For each attribution, add a borderless button that responds to clicks
        // and feels like a hyperlink.
        NSButton *button = [[MGLAttributionButton alloc] initWithAttributionInfo:info];
        button.controlSize = NSMiniControlSize;
        button.translatesAutoresizingMaskIntoConstraints = NO;
        
        // Set the new button flush with the buttom of the container and to the
        // right of the previous button, with standard spacing. If there is no
        // previous button, align to the container instead.
        NSView *previousView = attributionView.subviews.lastObject;
        [attributionView addSubview:button];
        [attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:button
                                      attribute:NSLayoutAttributeBottom
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:attributionView
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1
                                       constant:0]];
        [attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:button
                                      attribute:NSLayoutAttributeLeading
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:previousView ? previousView : attributionView
                                      attribute:previousView ? NSLayoutAttributeTrailing : NSLayoutAttributeLeading
                                     multiplier:1
                                       constant:8]];
        [attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:button
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:attributionView
                                      attribute:NSLayoutAttributeTop
                                     multiplier:1
                                       constant:0]];
    }
    
    if (attributionInfos.count) {
        [attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:attributionView
                                      attribute:NSLayoutAttributeTrailing
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:attributionView.subviews.lastObject
                                      attribute:NSLayoutAttributeTrailing
                                     multiplier:1
                                       constant:8]];
    }
}

- (void)cancel;
{
    _snapshotCallback.reset();
    _mbglMapSnapshotter.reset();
}

@end
