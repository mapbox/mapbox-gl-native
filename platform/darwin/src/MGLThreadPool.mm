#import "MGLThreadPool_Private.h"

#import "NSProcessInfo+MGLAdditions.h"

@interface MGLThreadPool ()

@property (nonatomic) mbgl::ThreadPool *mbglThreadPool;

@end

@implementation MGLThreadPool

#pragma mark - Internal

+ (instancetype)sharedPool {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLThreadPool *sharedThreadPool;
    dispatch_once(&onceToken, ^{
        sharedThreadPool = [[self alloc] init];
    });
    return sharedThreadPool;
}

- (instancetype)init {
    if (self = [super init]) {
        _mbglThreadPool = new mbgl::ThreadPool(4, { "Worker" });
    }
    return self;
}

- (void)dealloc {
    delete _mbglThreadPool;
    _mbglThreadPool = nullptr;
}

@end
