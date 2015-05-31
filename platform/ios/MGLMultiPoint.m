#import "MGLMultiPoint.h"

@implementation MGLMultiPoint
{
    CLLocationCoordinate2D *_coords;
    size_t _count;
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords
                              count:(NSUInteger)count
{
    assert(sizeof(coords) == count * sizeof(CLLocationCoordinate2D));

    self = [super init];

    if (self)
    {
        _count = count;
        _coords = malloc(_count * sizeof(CLLocationCoordinate2D));

        for (NSUInteger i = 0; i < _count; i++)
        {
            _coords[i] = coords[i];
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

    assert(sizeof(coords) >= sizeof(range.length * sizeof(CLLocationCoordinate2D)));
    assert(range.location + range.length < _count);

    NSUInteger index = 0;

    for (NSUInteger i = range.location; i < range.location + range.length; i++)
    {
        coords[index] = _coords[i];
        index++;
    }
}

@end
