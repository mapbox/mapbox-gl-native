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
    std::vector<CLLocationCoordinate2D> _coordinates;
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        _count = count;
        _coordinates.reserve(_count);
        for (NSUInteger i = 0; i < _count; i++)
        {
            _coordinates.push_back(coords[i]);
        }
        [self computeBounds];
    }

    return self;
}

- (CLLocationCoordinate2D)coordinate
{
    assert(_count > 0);

    CLLocationCoordinate2D coordinate = _coordinates.at(0);
    return CLLocationCoordinate2DMake(coordinate.latitude, coordinate.longitude);
}

- (NSUInteger)pointCount
{
    return _count;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingPointCount
{
    return [NSSet setWithObjects:@"coordinates", nil];
}

- (CLLocationCoordinate2D *)coordinates
{
    return _coordinates.data();
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
        coords[index] = _coordinates.at(i);
        index++;
    }
}

- (void)replaceCoordinatesInRange:(NSRange)range withCoordinates:(CLLocationCoordinate2D *)coords
{
    if (range.length == 0)
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
    NSUInteger newCount = NSMaxRange(range);
    std::vector<CLLocationCoordinate2D> newCoordinates;
    newCoordinates.reserve(range.length);
    for (NSUInteger i = 0; i < range.length; i++)
    {
        newCoordinates.push_back(coords[i]);
    }
    _coordinates.resize(newCount);
    std::copy_backward(newCoordinates.begin(), newCoordinates.end(), _coordinates.begin() + range.location + 1);
    _count = newCount;
    [self computeBounds];
    [self didChangeValueForKey:@"coordinates"];
}

- (void)computeBounds
{
    CLLocationCoordinate2D *coords = self.coordinates;
    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
    for (NSUInteger i = 0; i < _count; i++)
    {
        bounds.extend(mbgl::LatLng(coords[i].latitude, coords[i].longitude));
    }
    _bounds = MGLCoordinateBoundsFromLatLngBounds(bounds);
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
