#import "MGLAnnotationView.h"
#import "MGLAnnotationView_Private.h"
#import "MGLMapView_Private.h"
#import "MGLAnnotation.h"

#import "NSBundle+MGLAdditions.h"

#include <mbgl/util/constants.hpp>

@interface MGLAnnotationView () <UIGestureRecognizerDelegate>

@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, readwrite) CATransform3D lastAppliedScaleTransform;
@property (nonatomic, readwrite) CATransform3D lastAppliedRotateTransform;
@property (nonatomic, readwrite) CGFloat lastPitch;
@property (nonatomic, weak) UIPanGestureRecognizer *panGestureRecognizer;
@property (nonatomic, weak) UILongPressGestureRecognizer *longPressRecognizer;
@property (nonatomic, weak) MGLMapView *mapView;

@end

@implementation MGLAnnotationView

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithFrame:CGRectZero];
    if (self) {
        [self commonInitWithAnnotation:nil reuseIdentifier:reuseIdentifier];
    }
    return self;
}

- (instancetype)initWithAnnotation:(nullable id<MGLAnnotation>)annotation reuseIdentifier:(nullable NSString *)reuseIdentifier {
    self = [super initWithFrame:CGRectZero];
    if (self) {
        [self commonInitWithAnnotation:annotation reuseIdentifier:reuseIdentifier];
    }
    return self;
}

- (void)commonInitWithAnnotation:(nullable id<MGLAnnotation>)annotation reuseIdentifier:(nullable NSString *)reuseIdentifier {
    _lastAppliedScaleTransform = CATransform3DIdentity;
    _annotation = annotation;
    _reuseIdentifier = [reuseIdentifier copy];
    _scalesWithViewingDistance = YES;
    _enabled = YES;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        _reuseIdentifier = [decoder decodeObjectOfClass:[NSString class] forKey:@"reuseIdentifier"];
        _annotation = [decoder decodeObjectOfClass:[NSObject class] forKey:@"annotation"];
        _centerOffset = [decoder decodeCGVectorForKey:@"centerOffset"];
        _scalesWithViewingDistance = [decoder decodeBoolForKey:@"scalesWithViewingDistance"];
        _rotatesToMatchCamera = [decoder decodeBoolForKey:@"rotatesToMatchCamera"];
        _selected = [decoder decodeBoolForKey:@"selected"];
        _enabled = [decoder decodeBoolForKey:@"enabled"];
        self.draggable = [decoder decodeBoolForKey:@"draggable"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [super encodeWithCoder:coder];
    [coder encodeObject:_reuseIdentifier forKey:@"reuseIdentifier"];
    [coder encodeObject:_annotation forKey:@"annotation"];
    [coder encodeCGVector:_centerOffset forKey:@"centerOffset"];
    [coder encodeBool:_scalesWithViewingDistance forKey:@"scalesWithViewingDistance"];
    [coder encodeBool:_rotatesToMatchCamera forKey:@"rotatesToMatchCamera"];
    [coder encodeBool:_selected forKey:@"selected"];
    [coder encodeBool:_enabled forKey:@"enabled"];
    [coder encodeBool:_draggable forKey:@"draggable"];
}

- (void)prepareForReuse
{
    // Intentionally left blank. The default implementation of this method does nothing.
}

- (void)setCenterOffset:(CGVector)centerOffset
{
    _centerOffset = centerOffset;
    self.center = self.center;
}

- (void)setSelected:(BOOL)selected
{
    [self setSelected:selected animated:NO];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [self willChangeValueForKey:@"selected"];
    _selected = selected;
    [self didChangeValueForKey:@"selected"];
}

- (CGPoint)center
{
    CGPoint center = super.center;
    center.x -= _centerOffset.dx;
    center.y -= _centerOffset.dy;
    return center;
}

- (void)setCenter:(CGPoint)center
{
    center.x += _centerOffset.dx;
    center.y += _centerOffset.dy;

    super.center = center;
    [self updateScaleTransformForViewingDistance];
    [self updateRotateTransform];
}

- (void)setScalesWithViewingDistance:(BOOL)scalesWithViewingDistance
{
    if (_scalesWithViewingDistance != scalesWithViewingDistance)
    {
        _scalesWithViewingDistance = scalesWithViewingDistance;
        [self updateScaleTransformForViewingDistance];
    }
}

- (void)updateScaleTransformForViewingDistance
{
    if (self.scalesWithViewingDistance == NO || self.dragState == MGLAnnotationViewDragStateDragging) return;

    CGFloat superviewHeight = CGRectGetHeight(self.superview.frame);
    if (superviewHeight > 0.0) {
        // Find the maximum amount of scale reduction to apply as the view's center moves from the top
        // of the superview to the bottom. For example, if this view's center has moved 25% of the way
        // from the top of the superview towards the bottom then the maximum scale reduction is 1 - .25
        // or 75%. The range goes from a maximum of 100% to 0% as the view moves from the top to the bottom
        // along the y axis of its superview.
        CGFloat maxScaleReduction = 1.0 - self.center.y / superviewHeight;

        // Since it is possible for the map view to report a pitch less than 0 due to the nature of
        // how the gesture information is captured, the value is guarded with MAX.
        CGFloat pitch = MAX(self.mapView.camera.pitch, 0);

        // Return early if the map view currently has no pitch and was not previously pitched.
        if (!pitch && !_lastPitch) return;
        _lastPitch = pitch;

        // The pitch intensity represents how much the map view is actually pitched compared to
        // what is possible. The value will range from 0% (not pitched at all) to 100% (pitched as much
        // as the map view will allow). The map view's maximum pitch is defined in `mbgl::util::PITCH_MAX`.
        CGFloat pitchIntensity = pitch / MGLDegreesFromRadians(mbgl::util::PITCH_MAX);

        // The pitch adjusted scale is the inverse proportion of the maximum possible scale reduction
        // multiplied by the pitch intensity. For example, if the maximum scale reduction is 75% and the
        // map view is 50% pitched then the annotation view should be reduced by 37.5% (.75 * .5). The
        // reduction is then normalized for a scale of 1.0.
        CGFloat pitchAdjustedScale = 1.0 - maxScaleReduction * pitchIntensity;

        // We keep track of each viewing distance scale transform that we apply. Each iteration,
        // we can account for it so that we don't get cumulative scaling every time we move.
        // We also avoid clobbering any existing transform passed in by the client, too.
        CATransform3D undoOfLastScaleTransform = CATransform3DInvert(_lastAppliedScaleTransform);
        CATransform3D newScaleTransform = CATransform3DMakeScale(pitchAdjustedScale, pitchAdjustedScale, 1);
        CATransform3D effectiveTransform = CATransform3DConcat(undoOfLastScaleTransform, newScaleTransform);
        self.layer.transform = CATransform3DConcat(self.layer.transform, effectiveTransform);
        _lastAppliedScaleTransform = newScaleTransform;
    }
}

- (void)setRotatesToMatchCamera:(BOOL)rotatesToMatchCamera
{
    if (_rotatesToMatchCamera != rotatesToMatchCamera)
    {
        _rotatesToMatchCamera = rotatesToMatchCamera;
        [self updateRotateTransform];
    }
}

- (void)updateRotateTransform
{
    if (self.rotatesToMatchCamera == NO) return;

    CGFloat directionRad = self.mapView.direction * M_PI / 180.0;
    CATransform3D newRotateTransform = CATransform3DMakeRotation(-directionRad, 0, 0, 1);
    self.layer.transform = CATransform3DConcat(CATransform3DIdentity, newRotateTransform);
    
    _lastAppliedRotateTransform = newRotateTransform;
}

#pragma mark - Draggable

- (void)setDraggable:(BOOL)draggable
{
    [self willChangeValueForKey:@"draggable"];
    _draggable = draggable;
    [self didChangeValueForKey:@"draggable"];

    if (draggable)
    {
        [self enableDrag];
    }
    else
    {
        [self disableDrag];
    }
}

- (void)enableDrag
{
    if (!_longPressRecognizer)
    {
        UILongPressGestureRecognizer *recognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)];
        recognizer.delegate = self;
        [self addGestureRecognizer:recognizer];
        _longPressRecognizer = recognizer;
    }

    if (!_panGestureRecognizer)
    {
        UIPanGestureRecognizer *recognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
        recognizer.delegate = self;
        [self addGestureRecognizer:recognizer];
        _panGestureRecognizer = recognizer;
    }
}

- (void)disableDrag
{
    [self removeGestureRecognizer:_longPressRecognizer];
    [self removeGestureRecognizer:_panGestureRecognizer];
}

- (void)handleLongPress:(UILongPressGestureRecognizer *)sender
{
    switch (sender.state) {
        case UIGestureRecognizerStateBegan:
            self.dragState = MGLAnnotationViewDragStateStarting;
            break;
        case UIGestureRecognizerStateChanged:
            self.dragState = MGLAnnotationViewDragStateDragging;
            break;
        case UIGestureRecognizerStateCancelled:
            self.dragState = MGLAnnotationViewDragStateCanceling;
            break;
        case UIGestureRecognizerStateEnded:
            self.dragState = MGLAnnotationViewDragStateEnding;
            break;
        case UIGestureRecognizerStateFailed:
            self.dragState = MGLAnnotationViewDragStateNone;
            break;
        case UIGestureRecognizerStatePossible:
            break;
    }
}

- (void)handlePan:(UIPanGestureRecognizer *)sender
{
    self.center = [sender locationInView:sender.view.superview];

    if (sender.state == UIGestureRecognizerStateEnded) {
        self.dragState = MGLAnnotationViewDragStateNone;
    }
}

- (void)setDragState:(MGLAnnotationViewDragState)dragState
{
    [self setDragState:dragState animated:YES];
}

- (void)setDragState:(MGLAnnotationViewDragState)dragState animated:(BOOL)animated
{
    [self willChangeValueForKey:@"dragState"];
    _dragState = dragState;
    [self didChangeValueForKey:@"dragState"];

    if (dragState == MGLAnnotationViewDragStateStarting)
    {
        [self.mapView.calloutViewForSelectedAnnotation dismissCalloutAnimated:animated];
        [self.superview bringSubviewToFront:self];
    }
    else if (dragState == MGLAnnotationViewDragStateCanceling)
    {
        if (!self.annotation) {
            [NSException raise:NSInvalidArgumentException
                        format:@"Annotation property should not be nil."];
        }
        self.panGestureRecognizer.enabled = NO;
        self.longPressRecognizer.enabled = NO;
        self.center = [self.mapView convertCoordinate:self.annotation.coordinate toPointToView:self.mapView];
        self.panGestureRecognizer.enabled = YES;
        self.longPressRecognizer.enabled = YES;
        self.dragState = MGLAnnotationViewDragStateNone;
    }
    else if (dragState == MGLAnnotationViewDragStateEnding)
    {
        if ([self.annotation respondsToSelector:@selector(setCoordinate:)])
        {
            CLLocationCoordinate2D coordinate = [self.mapView convertPoint:self.center toCoordinateFromView:self.mapView];
            [(NSObject *)self.annotation setValue:[NSValue valueWithMGLCoordinate:coordinate] forKey:@"coordinate"];
        }
    }
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    BOOL isDragging = self.dragState == MGLAnnotationViewDragStateDragging;

    if (gestureRecognizer == _panGestureRecognizer && !(isDragging))
    {
        return NO;
    }

    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return otherGestureRecognizer == _longPressRecognizer || otherGestureRecognizer == _panGestureRecognizer;
}

#pragma mark UIAccessibility methods

- (BOOL)isAccessibilityElement {
    return !self.hidden;
}

- (UIAccessibilityTraits)accessibilityTraits {
    return UIAccessibilityTraitButton | UIAccessibilityTraitAdjustable;
}

- (NSString *)accessibilityLabel {
    return [self.annotation respondsToSelector:@selector(title)] ? self.annotation.title : super.accessibilityLabel;
}

- (NSString *)accessibilityValue {
    return [self.annotation respondsToSelector:@selector(subtitle)] ? self.annotation.subtitle : super.accessibilityValue;
}

- (NSString *)accessibilityHint {
    return NSLocalizedStringWithDefaultValue(@"ANNOTATION_A11Y_HINT", nil, nil, @"Shows more info", @"Accessibility hint");
}

- (CGRect)accessibilityFrame {
    CGRect accessibilityFrame = self.frame;
    CGRect minimumFrame = CGRectInset({ self.center, CGSizeZero },
                                      -MGLAnnotationAccessibilityElementMinimumSize.width / 2,
                                      -MGLAnnotationAccessibilityElementMinimumSize.height / 2);
    accessibilityFrame = CGRectUnion(accessibilityFrame, minimumFrame);
    return accessibilityFrame;
}

- (void)accessibilityIncrement {
    [self.superview accessibilityIncrement];
}

- (void)accessibilityDecrement {
    [self.superview accessibilityDecrement];
}

@end
