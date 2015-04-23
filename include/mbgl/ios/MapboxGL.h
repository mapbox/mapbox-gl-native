#import "MGLAnnotation.h"
#import "MGLMapView.h"
#import "MGLTypes.h"
#import "MGLUserLocation.h"

@interface MapboxGL : NSObject

+ (id) sharedInstanceWithAccessToken:(NSString *)token;
+ (NSString *) getAccessToken;

@end