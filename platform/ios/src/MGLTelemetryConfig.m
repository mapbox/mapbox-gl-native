#import "MGLTelemetryConfig.h"

static const CLLocationDistance MGLConfigHibernationRadiusDefault = 300.0;
static const CLLocationDistance MGLConfigHibernationRadiusWide = 600.0;

NSString *const MGLMapboxMetricsProfile = @"MGLMapboxMetricsProfile";

static NSString *const MGLConfigHibernationRadiusWideKey = @"WideGeoFence";

@implementation MGLTelemetryConfig

- (instancetype) init {
    self = [super init];
    if (self) {
        _MGLLocationManagerHibernationRadius = MGLConfigHibernationRadiusDefault;
    }
    return self;
}

+ (nullable instancetype)sharedConfig {
    static dispatch_once_t onceToken;
    static MGLTelemetryConfig *_sharedConfig;
    dispatch_once(&onceToken, ^{
        _sharedConfig = [[self alloc] init];
    });
    return _sharedConfig;
}

- (void)configurationFromKey:(NSString *)key {
    if ([key isEqualToString:MGLConfigHibernationRadiusWideKey]) {
        _MGLLocationManagerHibernationRadius = MGLConfigHibernationRadiusWide;
    }
}

@end
