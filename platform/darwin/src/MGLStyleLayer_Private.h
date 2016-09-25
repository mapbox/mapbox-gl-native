#import <Foundation/Foundation.h>

#import "MGLStyleLayer.h"
#import "MGLStyleValue_Private.h"

#include <mbgl/style/layer.hpp>

@interface MGLStyleLayer (Private)

@property (nonatomic, readwrite, copy) NSString *identifier;
@property (nonatomic) mbgl::style::Layer *layer;

@end
