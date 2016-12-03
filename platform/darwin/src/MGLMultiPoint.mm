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
    MGLCoordinateBounds _bounds;
    std::vector<CLLocationCoordinate2D> _coordinates;
}

- (instancetype)initWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        NSAssert(count > 0, @"A multipoint must have coordinates");
        _coordinates = { coords, coords + count };
        [self computeBounds];
    }

    return self;
}

- (CLLocationCoordinate2D)coordinate
{
    NSAssert([self pointCount] > 0, @"A multipoint must have coordinates");
    return _coordinates.at(0);
}

- (NSUInteger)pointCount
{
    return _coordinates.size();
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
    if (range.location + range.length > [self pointCount])
    {
        [NSException raise:NSRangeException
                    format:@"Invalid coordinate range %@ extends beyond current coordinate count of %ld",
                        NSStringFromRange(range), (unsigned long)[self pointCount]];
    }

    std::copy(_coordinates.begin() + range.location, _coordinates.begin() + NSMaxRange(range), coords);
}

- (void)appendCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count
{
    [self willChangeValueForKey:@"coordinates"];
    _coordinates.insert(_coordinates.end(), count, *coords);
    [self computeBounds];
    [self didChangeValueForKey:@"coordinates"];
}

- (void)replaceCoordinatesInRange:(NSRange)range withCoordinates:(const CLLocationCoordinate2D *)coords
{
    if (range.length == 0)
    {
        return;
    }
    
    if (NSMaxRange(range) > _coordinates.size())
    {
        [NSException raise:NSRangeException
                    format:@"Invalid range %@ for existing coordinate count %ld",
         NSStringFromRange(range), (unsigned long)[self pointCount]];
    }

    [self willChangeValueForKey:@"coordinates"];
    std::copy(coords, coords + range.length, _coordinates.begin() + range.location);
    [self computeBounds];
    [self didChangeValueForKey:@"coordinates"];
}

- (void)computeBounds
{
    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
    for (auto coordinate : _coordinates)
    {
        bounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));
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
               NSStringFromClass([self class]), (void *)self, (unsigned long)[self pointCount], MGLStringFromCoordinateBounds(_bounds)];
}

@end
