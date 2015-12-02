#import <Cocoa/Cocoa.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_OPTIONS(NSUInteger, MGLMapDebugMaskOptions) {
    MGLMapDebugTileBoundariesMask = 1 << 1,
    MGLMapDebugTileInfoMask = 1 << 2,
    MGLMapDebugTimestampsMask = 1 << 3,
    MGLMapDebugCollisionBoxesMask = 1 << 4,
};

@class MGLAnnotationImage;

@protocol MGLAnnotation;
@protocol MGLMapViewDelegate;
@protocol MGLOverlay;

IB_DESIGNABLE
@interface MGLMapView : NSView

- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL;

@property (nonatomic, weak, nullable) IBOutlet id <MGLMapViewDelegate> delegate;

@property (nonatomic, null_resettable) NSURL *styleURL;

- (IBAction)reloadStyle:(id)sender;

@property (nonatomic, readonly) NSSegmentedControl *zoomControls;
@property (nonatomic, readonly) NSSlider *compass;
@property (nonatomic, readonly) NSImageView *logoView;
@property (nonatomic, readonly) NSView *attributionView;

@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated;

/**
    The zoom level of the receiver.
    
    In addition to affecting the visual size and detail of features on the map,
    the zoom level affects the size of the vector tiles that are loaded. At zoom
    level 0, each tile covers the entire world map; at zoom level 1, it covers ¼
    of the world; at zoom level 2, <sup>1</sup>⁄<sub>16</sub> of the world, and
    so on.
 */
@property (nonatomic) double zoomLevel;
@property (nonatomic, readonly) double maximumZoomLevel;
@property (nonatomic, readonly) double minimumZoomLevel;

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated;

@property (nonatomic) CLLocationDirection direction;

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated;

@property (nonatomic) MGLCoordinateBounds visibleCoordinateBounds;

@property (nonatomic, getter=isScrollEnabled) BOOL scrollEnabled;
@property (nonatomic, getter=isZoomEnabled) BOOL zoomEnabled;
@property (nonatomic, getter=isRotateEnabled) BOOL rotateEnabled;
@property (nonatomic, getter=isPitchEnabled) BOOL pitchEnabled;

@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLAnnotation>) *annotations;

- (void)addAnnotation:(id <MGLAnnotation>)annotation;
- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;
- (void)removeAnnotation:(id <MGLAnnotation>)annotation;
- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier;

@property (nonatomic, copy) NS_ARRAY_OF(id <MGLAnnotation>) *selectedAnnotations;

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated;
- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated;

- (id <MGLAnnotation>)annotationAtPoint:(NSPoint)point;

- (void)addOverlay:(id <MGLOverlay>)overlay;
- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;
- (void)removeOverlay:(id <MGLOverlay>)overlay;
- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

- (CLLocationCoordinate2D)convertPoint:(NSPoint)point toCoordinateFromView:(nullable NSView *)view;
- (NSPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable NSView *)view;
- (MGLCoordinateBounds)convertRectToCoordinateBounds:(NSRect)rect;
- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude;

@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@end

NS_ASSUME_NONNULL_END
