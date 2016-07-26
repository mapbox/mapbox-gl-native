#import <Foundation/Foundation.h>

#import "MGLTypes.h"

@protocol MGLStyleAttributeValue <NSObject>
@optional
- (NSString *)stringValue;
- (MGLColor *)colorValue;
- (NSNumber *)numberValue;
- (NSValue *)valueValue;
- (BOOL)isFunction;
@end

/*
 - (mbgl::style::PropertyValue<mbgl::Color>)mbgl_propertyValue
 {
 if (!_color)
 {
 // TODO: return undefined
 }
 CGFloat r, g, b, a;
 [_color getRed:&r green:&g blue:&b alpha:&a];
 
 return mbgl::style::PropertyValue<mbgl::Color> {{(float)r, (float)g, (float)b, (float)a}};
 }
*/