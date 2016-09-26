#import <Foundation/Foundation.h>

@interface MGLSource : NSObject

@property (nonatomic, copy) NSString *sourceIdentifier;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier;

@end
