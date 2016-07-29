#import <Foundation/Foundation.h>

@interface MGLStyleAttributePair : NSObject

+ (instancetype)pairWith:(id)t1 and:(id)t2;

@property (nonatomic) id t1;
@property (nonatomic) id t2;

@end
