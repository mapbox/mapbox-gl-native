#import <Foundation/Foundation.h>
#import <mbgl/interface/native_apple_interface.h>

@implementation MGLNativeNetworkDelegate

static MGLNativeNetworkDelegate *instance = nil;

+ (MGLNativeNetworkDelegate *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[MGLNativeNetworkDelegate alloc] init];
    });
    return instance;
}

- (NSURLSessionConfiguration *)sessionConfiguration {
    if (_delegate && [_delegate respondsToSelector:@selector(sessionConfiguration)]) {
        return [_delegate sessionConfiguration];
    }
    return nil;
}

- (NSString *)skuToken {
    if(_delegate && [_delegate respondsToSelector:@selector(skuToken)]) {
        return [_delegate skuToken];
    }
    return nil;
}

- (NSString *)accountTypeKey {
    if (_delegate && [_delegate respondsToSelector:@selector(accountTypeKey)]) {
        return [_delegate accountTypeKey];
    }
    return nil;
}

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type {
    if (_delegate && [_delegate respondsToSelector:@selector(startDownloadEvent:type:)]) {
        [_delegate startDownloadEvent:event type:type];
    }
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(cancelDownloadEventForResponse:)]) {
        return [_delegate cancelDownloadEventForResponse:response];
    }
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(stopDownloadEventForResponse:)]) {
        return [_delegate stopDownloadEventForResponse:response];
    }
}

@end
