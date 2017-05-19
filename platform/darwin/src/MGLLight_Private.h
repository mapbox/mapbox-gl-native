#import <Foundation/Foundation.h>

#import "MGLLight.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

@interface MGLLight (Private)

/**
 A raw pointer to the mbgl object.
 */
@property (nonatomic, readonly) mbgl::style::Light *rawLight;

- (instancetype)initWithRawLayer:(mbgl::style::Light *)rawLight;

- (std::unique_ptr<mbgl::style::Light>)mbglLight;

@end
