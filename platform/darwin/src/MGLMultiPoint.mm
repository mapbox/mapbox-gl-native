#import "MGLMultiPoint_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLTypes.h"

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
    assert(_count > 0);

    return CLLocationCoordinate2DMake(_coordinates[0].latitude, _coordinates[0].longitude);
}

- (NSUInteger)pointCount
{
    return _count;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingPointCount
{
    return [NSSet setWithObjects:@"coordinates", nil];
}

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    if (range.location + range.length > _count)
    {
        [NSException raise:NSRangeException
                    format:@"Invalid coordinate range %@ extends beyond current coordinate count of %zu",
                        NSStringFromRange(range), _count];
    }

    NSUInteger index = 0;

    for (NSUInteger i = range.location; i < range.location + range.length; i++)
    {
        coords[index] = _coordinates[i];
        index++;
    }
}

- (void)replaceCoordinatesInRange:(NSRange)range withCoordinates:(CLLocationCoordinate2D *)coords
{
    if ((coords >= _coordinates && coords < _coordinates + _count) ||
        (coords + range.length >= _coordinates && coords + range.length < _coordinates + _count))
    {
        [NSException raise:NSRangeException format:@"Reuse of existing coordinates array %p not supported", coords];
    }
    else if (range.length == 0)
    {
        [NSException raise:NSRangeException format:@"Empty coordinate range %@", NSStringFromRange(range)];
    }
    else if (range.location > _count)
    {
        [NSException raise:NSRangeException
                    format:@"Invalid range %@ for existing coordinate count %zu",
                        NSStringFromRange(range), _count];
    }

    [self willChangeValueForKey:@"coordinates"];
    if (NSMaxRange(range) <= _count)
    {
        // replacing existing coordinate(s)
        memcpy(_coordinates + range.location, coords, range.length * sizeof(CLLocationCoordinate2D));
    }
    else
    {
        // appending new coordinate(s)
        NSUInteger newCount = NSMaxRange(range);
        CLLocationCoordinate2D *newCoordinates = (CLLocationCoordinate2D *)malloc(newCount * sizeof(CLLocationCoordinate2D));
        memcpy(newCoordinates, _coordinates, fmin(_count, range.location) * sizeof(CLLocationCoordinate2D));
        memcpy(newCoordinates + range.location, coords, range.length * sizeof(CLLocationCoordinate2D));
        [self setupWithCoordinates:newCoordinates count:newCount];
        free(newCoordinates);
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
