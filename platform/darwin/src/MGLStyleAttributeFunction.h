#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"


NS_ASSUME_NONNULL_BEGIN

@interface MGLStyleAttributeFunction : NSObject <MGLStyleAttributeValue>

@property (nonatomic, copy) NS_DICTIONARY_OF(NSNumber *, id) *stops;

@property (nonatomic, copy, nullable) NSNumber *base;

@end

NS_ASSUME_NONNULL_END
