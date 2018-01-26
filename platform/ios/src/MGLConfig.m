#import "MGLConfig.h"

static const CLLocationDistance MGLConfigHibernationRadiusDefault = 300.0;
static const CLLocationDistance MGLConfigHibernationRadiusWide = 1200.0;

NSString *const MGLMapboxMetricsProfile = @"MGLMapboxMetricsProfile";

static NSString *const MGLConfigDefaultKey = @"Default";
static NSString *const MGLConfigHibernationRadiusWideKey = @"WideGeoFence";

@implementation MGLConfig

- (instancetype) init {
    self = [super init];
    if (self) {
        _MGLLocationManagerHibernationRadius = MGLConfigHibernationRadiusDefault;
    }
    return self;
}

+ (nullable instancetype)sharedConfig {
    static dispatch_once_t onceToken;
    static MGLConfig *_sharedConfig;
    dispatch_once(&onceToken, ^{
        _sharedConfig = [[self alloc] init];
    });
    return _sharedConfig;
}

- (void)configurationFromKey:(NSString *)key {
    if ([key isEqualToString:MGLConfigDefaultKey]) {
        _MGLLocationManagerHibernationRadius = MGLConfigHibernationRadiusDefault;
    } else if ([key isEqualToString:MGLConfigHibernationRadiusWideKey]) {
        _MGLLocationManagerHibernationRadius = MGLConfigHibernationRadiusWide;
    }
}

@end
