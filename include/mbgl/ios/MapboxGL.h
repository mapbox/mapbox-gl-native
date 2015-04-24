#import "MGLAnnotation.h"
#import "MGLMapView.h"
#import "MGLTypes.h"
#import "MGLUserLocation.h"

@interface MapboxGL : NSObject

+ (instancetype) sharedInstanceWithAccessToken:(NSString *)token;
+ (NSString *) getAccessToken;

@end