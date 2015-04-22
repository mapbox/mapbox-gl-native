#import "MGLAnnotation.h"
#import "MGLMapView.h"
#import "MGLTypes.h"
#import "MGLUserLocation.h"

@interface MapboxGL : NSObject

+ (void) sharedInstanceWithAccessToken:(NSString *)token;

@end