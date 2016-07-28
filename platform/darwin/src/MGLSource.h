#import <Foundation/Foundation.h>

@interface MGLSource : NSObject

@property (nonatomic, copy) NSString *sourceID;
@property (nonatomic, copy) NSString *sourceType;

- (instancetype)initWithSourceID:(NSString *)sourceID sourceType:(NSString *)sourceType;

@end
