#import <Foundation/Foundation.h>

#import "MGLLight.h"

#import <mbgl/style/light.hpp>
#import <mbgl/style/types.hpp>

@interface MGLLight (Private)


- (instancetype)initWithMBGLLight:(const mbgl::style::Light &)mbglLight;

- (mbgl::style::Light)toMBGLLight;

@end
