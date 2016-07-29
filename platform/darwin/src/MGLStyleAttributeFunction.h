#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributePair.h"
#import "MGLStyleAttributeValue.h"

typedef NS_ENUM(NSUInteger, MGLStyleAttributeFunctionType) {
    MGLStyleAttributeFunctionTypeExponential,
    MGLStyleAttributeFunctionTypeInterval,
    MGLStyleAttributeFunctionTypeCategorical
};

NS_ASSUME_NONNULL_BEGIN

@interface MGLStyleAttributeFunction : NSObject <MGLStyleAttributeValue>

@property (nonatomic, copy) NSArray<MGLStyleAttributePair *> *stops;

@property (nonatomic, copy, nullable) NSString *property;

@property (nonatomic, copy, nullable) NSNumber *base;

@property (nonatomic, copy, nullable) NSNumber *functionType;

@end

NS_ASSUME_NONNULL_END
