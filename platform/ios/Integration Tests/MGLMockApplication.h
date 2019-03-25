#import <Foundation/Foundation.h>
#import "../src/MGLApplication_Private.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLMockApplication : NSObject <MGLApplication>

- (void)enterBackground;
- (void)enterForeground;

#pragma mark - MGLApplication

@property(nonatomic, readonly) UIApplicationState applicationState;
@property(nonatomic, nullable, assign) id<UIApplicationDelegate> delegate;
@property(nonatomic, readonly) UIInterfaceOrientation statusBarOrientation __TVOS_PROHIBITED;

// TODO: support openURL:options:completionHandler:
- (BOOL)openURL:(NSURL*)url NS_DEPRECATED_IOS(2_0, 10_0, "Please use openURL:options:completionHandler: instead") NS_EXTENSION_UNAVAILABLE_IOS("");

@end

NS_ASSUME_NONNULL_END
