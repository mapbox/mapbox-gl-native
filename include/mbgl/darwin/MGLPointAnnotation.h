#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The `MGLPointAnnotation` class defines a concrete annotation object located at a specified point. You can use this class, rather than define your own, in situations where all you want to do is associate a point on the map with a title. */
@interface MGLPointAnnotation : MGLShape

/** The coordinate point of the annotation, specified as a latitude and longitude. */
@property (nonatomic, assign) CLLocationCoordinate2D coordinate;

@end

NS_ASSUME_NONNULL_END
