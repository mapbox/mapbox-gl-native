#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLNativeNetworkDelegate <NSObject>

@optional

- (NSString *)skuToken;

@required

- (NSURLSessionConfiguration *)sessionConfiguration;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;

- (void)debugLog:(NSString *)format, ...;

- (void)errorLog:(NSString *)format, ...;

@end

#define MGL_APPLE_EXPORT __attribute__((visibility ("default")))

@interface MGLNativeNetworkManager: NSObject

+ (MGLNativeNetworkManager *)sharedManager;

@property (nonatomic, weak) id<MGLNativeNetworkDelegate> delegate;

@property (nonatomic, readonly) NSString *skuToken;

@property (nonatomic, readonly) NSURLSessionConfiguration *sessionConfiguration;

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type;

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

- (void)stopDownloadEventForResponse:(NSURLResponse *)response;

- (void)debugLog:(NSString *)format, ...;

- (void)errorLog:(NSString *)format, ...;

@end

NS_ASSUME_NONNULL_END
