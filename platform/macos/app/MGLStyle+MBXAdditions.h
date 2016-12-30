#import <Mapbox/Mapbox.h>

@interface MGLStyle (MBXAdditions)

@property (nonatomic, strong) NS_ARRAY_OF(__kindof MGLStyleLayer *) *reversedLayers;

@end
