#import <Foundation/Foundation.h>

#import "MGLTypes.h"

typedef struct {
    float red;
    float green;
    float blue;
    float alpha;
} MGLAttributeColor;

@protocol MGLStyleAttributeValue <NSObject>
@optional
- (NSString *)stringValue;
- (MGLColor *)colorValue;
- (MGLAttributeColor)attributeColor;
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