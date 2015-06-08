#import "MGLMultiPoint.h"
#import "MGLTypes.h"

#import <mbgl/util/geo.hpp>

@implementation MGLMultiPoint
{
    CLLocationCoordinate2D *_coords;
    size_t _count;
    mbgl::LatLngBounds _bounds;
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords
                              count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        _count = count;
        _coords = (CLLocationCoordinate2D *)malloc(_count * sizeof(CLLocationCoordinate2D));

        for (NSUInteger i = 0; i < _count; i++)
        {
            _coords[i] = coords[i];
            _bounds.extend(mbgl::LatLng(coords[i].latitude, coords[i].longitude));
        }
    }

    return self;
}

- (void)dealloc
{
    free(_coords);
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

    return CLLocationCoordinate2DMake(_coords[0].latitude, _coords[0].longitude);
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
        coords[index] = _coords[i];
        index++;
    }
}

- (MGLCoordinateBounds)overlayBounds
{
    return {
        CLLocationCoordinate2DMake(_bounds.sw.latitude,  _bounds.sw.longitude),
        CLLocationCoordinate2DMake(_bounds.ne.longitude, _bounds.ne.longitude)
    };
}

- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    mbgl::LatLngBounds area(
        mbgl::LatLng(overlayBounds.sw.latitude, overlayBounds.sw.longitude),
        mbgl::LatLng(overlayBounds.ne.latitude, overlayBounds.ne.longitude)
    );

    return _bounds.intersects(area);
}

@end
