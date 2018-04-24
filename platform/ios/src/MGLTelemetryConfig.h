#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MGLTelemetryConfig : NSObject

@property (nonatomic) CLLocationDistance MGLLocationManagerHibernationRadius;

extern NSString *const MGLMapboxMetricsProfile;

@property (class, nullable, nonatomic, readonly) MGLTelemetryConfig *sharedConfig;

- (void)configurationFromKey:(NSString *)key;

@end

NS_ASSUME_NONNULL_END
