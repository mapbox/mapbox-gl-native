#import "MMEConfigurator.h"
#import "MMEAPIClient.h"

@interface MMEConfigurator ()

@property (nonatomic) NSDate *configurationRotationDate;
@property (nonatomic) MMEEventsConfiguration *configuration;

@end

@implementation MMEConfigurator

- (instancetype)initWithTimeInterval:(NSTimeInterval)timeInterval {
    if (self = [super init]) {
        _timeInterval = timeInterval;
    }
    return self;
}

- (void)updateConfigurationFromAPIClient:(MMEAPIClient *)apiClient {
    if (self.configurationRotationDate && [[NSDate date] timeIntervalSinceDate:self.configurationRotationDate] >= 0) {
        self.configuration = nil;
    }
    if (!self.configuration) {
        [apiClient getConfigurationWithCompletionHandler:^(NSError * _Nullable error, NSData * _Nullable data) {
            if (!error) {
                self.configuration = [MMEEventsConfiguration configurationFromData:data];
                
                [self.delegate configurator:self didUpdate:self.configuration];
                self.configurationRotationDate = [[NSDate date] dateByAddingTimeInterval:self.timeInterval];
            }
        }];
    }
}

@end
