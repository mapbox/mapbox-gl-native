#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface MGLSource : NSObject

@property (nonatomic, copy) NSString *sourceIdentifier;
@property (nonatomic, copy) NSString *sourceType;

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier sourceType:(NSString *)sourceType;

@end
