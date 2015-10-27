#import "MGLCircle.h"

#import <mbgl/util/geo.hpp>
#import <mbgl/util/projection.hpp>

@implementation MGLCircle
{
    CLLocationDistance _radius;
    mbgl::LatLngBounds _bounds;
}

@synthesize coordinate=_coordinate;

- (instancetype)initWithCenterCoordinate:(CLLocationCoordinate2D)coordinate
                                  radius:(CLLocationDistance)radius
                           overlayBounds:(MGLCoordinateBounds)overlayBounds
{
    self = [super init];

    if (self)
    {
        _coordinate = coordinate;
        _radius = radius;

        if (MGLCoordinateBoundsIsEmpty(overlayBounds))
        {
            using Projection = mbgl::Projection;
            using ProjectedMeters = mbgl::ProjectedMeters;

            ProjectedMeters projectedCenter = Projection::projectedMetersForLatLng(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

            ProjectedMeters projectedWest = ProjectedMeters(projectedCenter.northing, projectedCenter.easting - radius);
            ProjectedMeters projectedEast = ProjectedMeters(projectedCenter.northing, projectedCenter.easting + radius);
            ProjectedMeters projectedNorth = ProjectedMeters(projectedCenter.northing + radius, projectedCenter.easting);
            ProjectedMeters projectedSouth = ProjectedMeters(projectedCenter.northing - radius, projectedCenter.easting);

            _bounds.extend(Projection::latLngForProjectedMeters(projectedWest));
            _bounds.extend(Projection::latLngForProjectedMeters(projectedEast));
            _bounds.extend(Projection::latLngForProjectedMeters(projectedNorth));
            _bounds.extend(Projection::latLngForProjectedMeters(projectedSouth));
        }
        else
        {
            _bounds.extend(mbgl::LatLng(overlayBounds.sw.latitude, overlayBounds.sw.longitude));
            _bounds.extend(mbgl::LatLng(overlayBounds.ne.latitude, overlayBounds.ne.longitude));
        }
    }

    return self;
}

+ (instancetype)circleWithCenterCoordinate:(CLLocationCoordinate2D)coordinate
                                    radius:(CLLocationDistance)radius
{
    CLLocationCoordinate2D nullIsland = CLLocationCoordinate2DMake(0, 0);

    return [[self alloc] initWithCenterCoordinate:coordinate
                                           radius:radius
                                    overlayBounds:MGLCoordinateBoundsMake(nullIsland, nullIsland)];
}

+ (instancetype)circleWithOverlayBounds:(MGLCoordinateBounds)overlayBounds
{
    CLLocation *sw = [[CLLocation alloc] initWithLatitude:overlayBounds.sw.latitude
                                                longitude:overlayBounds.sw.longitude];
    CLLocation *ne = [[CLLocation alloc] initWithLatitude:overlayBounds.ne.latitude
                                                longitude:overlayBounds.ne.longitude];

    CLLocationCoordinate2D centerCoordinate = CLLocationCoordinate2DMake(
        (sw.coordinate.latitude  + ne.coordinate.latitude)  / 2,
        (sw.coordinate.longitude + ne.coordinate.longitude) / 2);

    CLLocation *se = [[CLLocation alloc] initWithLatitude:sw.coordinate.latitude
                                                longitude:ne.coordinate.longitude];

    CLLocationDistance radius = [sw distanceFromLocation:se] / 2;

    return [[self alloc] initWithCenterCoordinate:centerCoordinate
                                           radius:radius
                                    overlayBounds:overlayBounds];
}

- (MGLCoordinateBounds)overlayBounds
{
    return {
        CLLocationCoordinate2DMake(_bounds.sw.latitude, _bounds.sw.longitude),
        CLLocationCoordinate2DMake(_bounds.ne.latitude, _bounds.ne.longitude)
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
