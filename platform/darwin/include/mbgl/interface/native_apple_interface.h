#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLNativeAppleInterfaceDelegate <NSObject>

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR

- (NSString *)nai_skuToken;
- (NSString *)nai_accountTypeKey;

#endif

- (NSURLSessionConfiguration *)nai_sessionConfiguration;

- (void)nai_startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)nai_cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)nai_stopDownloadEventForResponse:(NSURLResponse *)response;

@end

#define MGL_APPLE_EXPORT __attribute__((visibility ("default")))

@interface MGLNativeAppleInterfaceTransmitter: NSObject

+ (MGLNativeAppleInterfaceTransmitter *)shared;

@property (nonatomic, weak) id<MGLNativeAppleInterfaceDelegate> delegate;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
@property (nonatomic, readonly) NSString *accountTypeKey;
@property (nonatomic, readonly) NSString *skuToken;
#endif

@property (nonatomic, readonly) NSURLSessionConfiguration *sessionConfiguration;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;


@end

NS_ASSUME_NONNULL_END
