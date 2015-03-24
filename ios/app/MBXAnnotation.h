#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import <mbgl/ios/MGLAnnotation.h>

@interface MBXAnnotation : NSObject <MGLAnnotation>

+ (instancetype)annotationWithLocation:(CLLocationCoordinate2D)coordinate title:(NSString *)title subtitle:(NSString *)subtitle;

- (instancetype)initWithLocation:(CLLocationCoordinate2D)coordinate title:(NSString *)title subtitle:(NSString *)subtitle;

@end
