#import <Foundation/Foundation.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLStyleAttributeFunctionType) {
    MGLStyleAttributeFunctionTypeExponential,
    MGLStyleAttributeFunctionTypeInterval,
    MGLStyleAttributeFunctionTypeCategorical
};

NS_ASSUME_NONNULL_BEGIN

@interface MGLStyleAttributeFunction : NSObject

@property (nonatomic, copy) NSArray *stops;

@property (nonatomic, copy, nullable) NSString *property;

@property (nonatomic, copy, nullable) NSNumber *base;

@property (nonatomic, copy, nullable) NSNumber *functionType;

@end

NS_ASSUME_NONNULL_END
