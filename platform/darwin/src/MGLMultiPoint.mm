#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"

#import <mbgl/util/geo.hpp>

mbgl::Color MGLColorObjectFromCGColorRef(CGColorRef cgColor)
{
    if (!cgColor) return { 0, 0, 0, 0 };
    NSCAssert(CGColorGetNumberOfComponents(cgColor) >= 4, @"Color must have at least 4 components");
    const CGFloat *components = CGColorGetComponents(cgColor);
    return { (float)components[0], (float)components[1], (float)components[2], (float)components[3] };
}

@implementation MGLMultiPoint
{
    size_t _count;
    MGLCoordinateBounds _bounds;
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        [self setupWithCoordinates:coords count:count];
    }

    return self;
}

- (void)setupWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    if (_coordinates) free(_coordinates);

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

    if (range.location + range.length > _count)
    {
        [[NSException exceptionWithName:NSRangeException
                                 reason:[NSString stringWithFormat:
                                            @"Invalid coordinate range %@ extends beyond coordinate count of %zu",
                                            NSStringFromRange(range), _count]
                               userInfo:nil] raise];
    }

    NSUInteger index = 0;

    for (NSUInteger i = range.location; i < range.location + range.length; i++)
    {
        coords[index] = _coordinates[i];
        index++;
    }
}

- (void)setCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    if ([self isMemberOfClass:[MGLMultiPoint class]])
    {
        [[NSException exceptionWithName:@"MGLAbstractClassException"
                                 reason:@"MGLMultiPoint is an abstract class"
                               userInfo:nil] raise];
    }

    if (range.length == 0)
    {
        [[NSException exceptionWithName:NSRangeException
                                 reason:[NSString stringWithFormat:@"Empty coordinate range %@", NSStringFromRange(range)]
                               userInfo:nil] raise];
    }

    [self willChangeValueForKey:@"coordinates"];
    if (range.location >= _count)
    {
        // appending new coordinate(s)
        NSUInteger newCount = _count + (_count - range.location) + range.length;
        CLLocationCoordinate2D *newCoords = (CLLocationCoordinate2D *)malloc(newCount * sizeof(CLLocationCoordinate2D));
        for (NSUInteger i = 0; i < range.location; i++)
        {
            newCoords[i] = _coordinates[i];
        }
        for (NSUInteger i = 0; i < range.length; i++)
        {
            newCoords[i + range.location] = coords[i];
        }
        [self setupWithCoordinates:newCoords count:newCount];
        free(newCoords);
    }
    else
    {
        // replacing existing coordinate(s)
        for (NSUInteger i = 0; i < range.length; i++)
        {
            _coordinates[i + range.location] = coords[i];
        }
    }
    [self didChangeValueForKey:@"coordinates"];
}

- (MGLCoordinateBounds)overlayBounds
{
    return _bounds;
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    return MGLLatLngBoundsFromCoordinateBounds(_bounds).intersects(MGLLatLngBoundsFromCoordinateBounds(overlayBounds));
}

- (mbgl::Annotation)annotationObjectWithDelegate:(__unused id <MGLMultiPointDelegate>)delegate
{
    NSAssert(NO, @"Cannot add an annotation from an instance of %@", NSStringFromClass([self class]));
    return mbgl::SymbolAnnotation({mbgl::Point<double>()});
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; count = %lu; bounds = %@>",
            NSStringFromClass([self class]), (void *)self, (unsigned long)_count, MGLStringFromCoordinateBounds(_bounds)];
}

@end
