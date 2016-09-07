#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface MGLSource : NSObject

@property (nonatomic, copy) NSString *sourceIdentifier;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier;

@end
