#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLNativeAppleInterfaceDelegate <NSObject>

@optional

- (NSString *)skuToken;
- (NSString *)accountTypeKey;

@required

- (NSURLSessionConfiguration *)sessionConfiguration;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;

@end

#define MGL_APPLE_EXPORT __attribute__((visibility ("default")))

@interface MGLNativeNetworkDelegate: NSObject

+ (MGLNativeNetworkDelegate *)shared;

@property (nonatomic, weak) id<MGLNativeAppleInterfaceDelegate> delegate;

@property (nonatomic, readonly) NSString *accountTypeKey;
@property (nonatomic, readonly) NSString *skuToken;

@property (nonatomic, readonly) NSURLSessionConfiguration *sessionConfiguration;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;


@end

NS_ASSUME_NONNULL_END
