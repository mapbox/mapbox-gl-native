#import <Mapbox/Mapbox.h>

@interface MGLStyle (MBXAdditions)

@property (nonatomic, strong) NSArray<__kindof MGLStyleLayer *> *reversedLayers;

@end
