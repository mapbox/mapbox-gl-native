#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

#import <mbgl/util/geo.hpp>

mbgl::Color MGLColorObjectFromCGColorRef(CGColorRef cgColor) {
    if (!cgColor) {
        return { 0, 0, 0, 0 };
    }
    NSCAssert(CGColorGetNumberOfComponents(cgColor) >= 4, @"Color must have at least 4 components");
    const CGFloat *components = CGColorGetComponents(cgColor);
    return { (float)components[0], (float)components[1], (float)components[2], (float)components[3] };
}

@implementation MGLMultiPoint
{
    size_t _count;
    MGLCoordinateBounds _bounds;
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords
                              count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        _count = count;
        _coordinates = (CLLocationCoordinate2D *)malloc(_count * sizeof(CLLocationCoordinate2D));

        mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();

        for (NSUInteger i = 0; i < _count; i++)
        {
            _coordinates[i] = coords[i];
            bounds.extend(mbgl::LatLng(coords[i].latitude, coords[i].longitude));
        }

        _bounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
    }

    return self;
}

- (void)dealloc
{
    free(_coordinates);
}

- (CLLocationCoordinate2D)coordinate
{
    if ([self isMemberOfClass:[MGLMultiPoint class]])
    {
        [[NSException exceptionWithName:@"MGLAbstractClassException"
                                 reason:@"MGLMultiPoint is an abstract class"
                               userInfo:nil] raise];
    }

    assert(_count > 0);

    return CLLocationCoordinate2DMake(_coordinates[0].latitude, _coordinates[0].longitude);
}

- (NSUInteger)pointCount
{
    if ([self isMemberOfClass:[MGLMultiPoint class]])
    {
        [[NSException exceptionWithName:@"MGLAbstractClassException"
                                 reason:@"MGLMultiPoint is an abstract class"
                               userInfo:nil] raise];
    }

    return _count;
}

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    if ([self isMemberOfClass:[MGLMultiPoint class]])
    {
        [[NSException exceptionWithName:@"MGLAbstractClassException"
                                 reason:@"MGLMultiPoint is an abstract class"
                               userInfo:nil] raise];
    }

    assert(range.location + range.length <= _count);

    NSUInteger index = 0;

    for (NSUInteger i = range.location; i < range.location + range.length; i++)
    {
        coords[index] = _coordinates[i];
        index++;
    }
}

- (MGLCoordinateBounds)overlayBounds
{
    return _bounds;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    return MGLLatLngBoundsFromCoordinateBounds(_bounds).intersects(MGLLatLngBoundsFromCoordinateBounds(overlayBounds));
}

- (BOOL)isWithinDistance:(CGFloat)distance ofPoint:(CGPoint)point transform:(CGPoint (^)(CLLocationCoordinate2D))transform
{
    BOOL isWithinDistance = NO;

    for (NSInteger i = 0; i < self.pointCount; i++) {
        NSInteger j = (i != self.pointCount - 1) ? i + 1 : 0;
        CGPoint endpoint1 = transform(self.coordinates[i]);
        CGPoint endpoint2 = transform(self.coordinates[j]);

        CGFloat length = point.x - endpoint1.x;
        CGFloat height = point.y - endpoint1.y;
        CGFloat xDifference = endpoint2.x - endpoint1.x;
        CGFloat yDifference = endpoint2.y - endpoint1.y;

        CGFloat dotProduct = length * xDifference + height * yDifference;
        CGFloat lineLengthSquared = xDifference * xDifference + yDifference * yDifference;
        CGFloat offset = dotProduct / lineLengthSquared;

        CGFloat xOffset, yOffset;
        if (offset <= 0.0) {
            xOffset = endpoint1.x;
            yOffset = endpoint1.y;
        }
        else if (offset >= 1.0) {
            xOffset = endpoint2.x;
            yOffset = endpoint2.y;
        }
        else {
            xOffset = endpoint1.x + offset * xDifference;
            yOffset = endpoint1.y + offset * yDifference;
        }
        
        CGFloat xDistance = point.x - xOffset;
        CGFloat yDistance = point.y - yOffset;
        
        CGFloat distanceFromPoint = sqrt(xDistance * xDistance + yDistance * yDistance);
        
        if (distanceFromPoint <= distance) {
            isWithinDistance = YES;
            break;
        }
    }

    return isWithinDistance;
}

- (mbgl::Annotation)annotationObjectWithDelegate:(__unused id <MGLMultiPointDelegate>)delegate {
    NSAssert(NO, @"Cannot add an annotation from an instance of %@", NSStringFromClass([self class]));
    return mbgl::SymbolAnnotation({mbgl::Point<double>()});
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; count = %lu; bounds = %@>",
            NSStringFromClass([self class]), (void *)self, (unsigned long)_count, MGLStringFromCoordinateBounds(_bounds)];
}

@end
