#import <Foundation/Foundation.h>
#import "MMEEventsConfiguration.h"
#import "MMEAPIClient.h"

@protocol MMEConfiguratorDelegate <NSObject>

- (void)configurator:(id)updater didUpdate:(MMEEventsConfiguration *)configuration;

@end

@interface MMEConfigurator : NSObject

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithTimeInterval:(NSTimeInterval)timeInterval NS_DESIGNATED_INITIALIZER;
- (void)updateConfigurationFromAPIClient:(MMEAPIClient *)apiClient;

@property (nonatomic) NSTimeInterval timeInterval;
@property (nonatomic, weak) id <MMEConfiguratorDelegate> delegate;

@end
