#import <Foundation/Foundation.h>
#import <mbgl/interface/native_apple_interface.h>

@implementation MGLNativeAppleInterfaceTransmitter

static MGLNativeAppleInterfaceTransmitter *instance = nil;

+ (MGLNativeAppleInterfaceTransmitter *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[MGLNativeAppleInterfaceTransmitter alloc] init];
    });
    return instance;
}

- (NSURLSessionConfiguration *)sessionConfiguration {
    if (_delegate && [_delegate respondsToSelector:@selector(nai_sessionConfiguration)]) {
        return [_delegate nai_sessionConfiguration];
    }
    return nil;
}

- (NSString *)skuToken {
    if(_delegate && [_delegate respondsToSelector:@selector(nai_skuToken)]) {
        return [_delegate nai_skuToken];
    }
    return nil;
}

- (NSString *)accountTypeKey {
    if (_delegate && [_delegate respondsToSelector:@selector(nai_accountTypeKey)]) {
        return [_delegate nai_accountTypeKey];
    }
    return nil;
}

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type {
    if (_delegate && [_delegate respondsToSelector:@selector(nai_startDownloadEvent:type:)]) {
        [_delegate nai_startDownloadEvent:event type:type];
    }
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(nai_cancelDownloadEventForResponse:)]) {
        return [_delegate nai_cancelDownloadEventForResponse:response];
    }
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(nai_stopDownloadEventForResponse:)]) {
        return [_delegate nai_stopDownloadEventForResponse:response];
    }
}

@end
