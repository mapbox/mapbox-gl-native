@interface MGLAccountManager : NSObject

+ (instancetype) sharedInstanceWithAccessToken:(NSString *)token;
+ (NSString *) getAccessToken;

@end