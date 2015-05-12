@interface MGLAccountManager : NSObject

+ (void) setShowsOptOutInApp:(BOOL)showsOptOut;
+ (void) setAccessToken:(NSString *) accessToken;
+ (NSString *) accessToken;

@end