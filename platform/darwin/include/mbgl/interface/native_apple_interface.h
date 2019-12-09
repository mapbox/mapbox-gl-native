#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLNativeAppleInterfaceDelegate <NSObject>

- (NSString *)nai_skuToken;

- (NSURLSessionConfiguration *)nai_sessionConfiguration;

- (NSString *)nai_accountTypeKey;

- (void)nai_startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)nai_cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)nai_stopDownloadEventForResponse:(NSURLResponse *)response;

@end

#define MGL_APPLE_EXPORT __attribute__((visibility ("default")))

@interface MGLNativeAppleInterfaceTransmitter: NSObject

+ (MGLNativeAppleInterfaceTransmitter *)shared;

@property (nonatomic, weak) id<MGLNativeAppleInterfaceDelegate> delegate;

@property (nonatomic, readonly) NSURLSessionConfiguration *sessionConfiguration;

@property (nonatomic, readonly) NSString *accountTypeKey;

@property (nonatomic, readonly) NSString *skuToken;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;


@end

NS_ASSUME_NONNULL_END
