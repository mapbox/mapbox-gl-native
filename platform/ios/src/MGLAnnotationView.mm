#import "MGLAnnotationView.h"
#import "MGLAnnotationView_Private.h"
#import "MGLAnnotation.h"
#import "MGLMapView_Internal.h"

#import "NSBundle+MGLAdditions.h"

#include <mbgl/util/constants.hpp>

@interface MGLAnnotationView () <UIGestureRecognizerDelegate>

@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, readwrite, nullable) id <MGLAnnotation> annotation;
@property (nonatomic, weak) UIPanGestureRecognizer *panGestureRecognizer;
@property (nonatomic, weak) UILongPressGestureRecognizer *longPressRecognizer;
@property (nonatomic, weak) MGLMapView *mapView;

@end

@implementation MGLAnnotationView

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    self = [self initWithFrame:CGRectZero];
    if (self)
    {
        _reuseIdentifier = [reuseIdentifier copy];
        _scalesWithViewingDistance = YES;
        _enabled = YES;
    }
    return self;
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
    [self updateTransform];
}

- (void)setScalesWithViewingDistance:(BOOL)scalesWithViewingDistance
{
    if (_scalesWithViewingDistance != scalesWithViewingDistance)
    {
        _scalesWithViewingDistance = scalesWithViewingDistance;
        [self updateTransform];
    }
}

- (void)updateTransform
{
    // Omit applying a new transformation while the view is being dragged.
    if (self.dragState == MGLAnnotationViewDragStateDragging)
    {
        return;
    }
    
    self.layer.transform = CATransform3DIdentity;
    if ( ! self.scalesWithViewingDistance)
    {
        return;
    }
    
    CGFloat superviewHeight = CGRectGetHeight(self.superview.frame);
    if (superviewHeight > 0.0) {
        // Find the maximum amount of scale reduction to apply as the view's center moves from the top
        // of the superview to the bottom. For example, if this view's center has moved 25% of the way
        // from the top of the superview towards the bottom then the maximum scale reduction is 1 - .25
        // or 75%. The range goes from a maximum of 100% to 0% as the view moves from the top to the bottom
        // along the y axis of its superview.
        CGFloat maxScaleReduction = 1.0 - self.center.y / superviewHeight;
       
        // The pitch intensity represents how much the map view is actually pitched compared to
        // what is possible. The value will range from 0% (not pitched at all) to 100% (pitched as much
        // as the map view will allow). The map view's maximum pitch is defined in `mbgl::util::PITCH_MAX`.
        // Since it is possible for the map view to report a pitch less than 0 due to the nature of
        // how the gesture information is captured, the value is guarded with MAX.
        CGFloat pitchIntensity = MAX(self.mapView.camera.pitch, 0) / MGLDegreesFromRadians(mbgl::util::PITCH_MAX);
       
        // The pitch adjusted scale is the inverse proportion of the maximum possible scale reduction
        // multiplied by the pitch intensity. For example, if the maximum scale reduction is 75% and the
        // map view is 50% pitched then the annotation view should be reduced by 37.5% (.75 * .5). The
        // reduction is then normalized for a scale of 1.0.
        CGFloat pitchAdjustedScale = 1.0 - maxScaleReduction * pitchIntensity;
        
        CATransform3D transform = CATransform3DIdentity;
        self.layer.transform = CATransform3DScale(transform, pitchAdjustedScale, pitchAdjustedScale, 1);
    }
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
    
    if ([gestureRecognizer isKindOfClass:UIPanGestureRecognizer.class] && !(isDragging))
    {
        return NO;
    }

    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return otherGestureRecognizer == _longPressRecognizer || otherGestureRecognizer == _panGestureRecognizer;
}

- (id<CAAction>)actionForLayer:(CALayer *)layer forKey:(NSString *)event
{
    // Allow mbgl to drive animation of this view’s bounds.
    if ([event isEqualToString:@"bounds"] || [event isEqualToString:@"position"])
    {
        return [NSNull null];
    }
    return [super actionForLayer:layer forKey:event];
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
