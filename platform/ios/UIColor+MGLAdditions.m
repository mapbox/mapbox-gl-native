#import "UIColor+MGLAdditions.h"

/* Portions based on Erica Sadun's uicolor-utilities
   https://github.com/erica/uicolor-utilities */

@interface UIColor (MGLAdditionsPrivate)

+ (UIColor *)colorWithRGBHex:(UInt32)hex;
- (CGColorSpaceModel)colorSpaceModel;
- (BOOL)canProvideRGBComponents;
- (BOOL)red:(CGFloat *)red green:(CGFloat *)green blue:(CGFloat *)blue alpha:(CGFloat *)alpha;
- (UInt32)rgbHex;

@end

@implementation UIColor (MGLAdditions)

+ (UIColor *)colorWithRGBAString:(NSString *)rgbaString
{
    UIColor *color;

    NSString *numberString = [rgbaString   stringByReplacingOccurrencesOfString:@"rgba("
                                                                     withString:@""];
              numberString = [numberString stringByReplacingOccurrencesOfString:@"rgb("
                                                                     withString:@""];
              numberString = [numberString stringByReplacingOccurrencesOfString:@")"
                                                                     withString:@""];

    NSArray *numbers = [numberString componentsSeparatedByString:@","];

    if ([rgbaString hasPrefix:@"rgb("] && [numbers count] == 3)
    {
        color = [UIColor colorWithRed:[numbers[0] floatValue] / 255
                                green:[numbers[1] floatValue] / 255
                                 blue:[numbers[2] floatValue] / 255
                                alpha:1.0];
    }
    else if ([rgbaString hasPrefix:@"rgba("] && [numbers count] == 4)
    {
        color = [UIColor colorWithRed:[numbers[0] floatValue] / 255
                                green:[numbers[1] floatValue] / 255
                                 blue:[numbers[2] floatValue] / 255
                                alpha:[numbers[3] floatValue]];
    }

    return color;
}

- (NSString *)rgbaStringFromColor
{
    CGFloat r,g,b,a;

    [self getRed:&r green:&g blue:&b alpha:&a];

    r *= 255;
    g *= 255;
    b *= 255;
    a *= 255;

    return [NSString stringWithFormat:@"rgba(%lu,%lu,%lu,%lu)", (unsigned long)r, (unsigned long)g, (unsigned long)b, (unsigned long)a];
}

+ (UIColor *)colorWithRGBHex:(UInt32)hex
{
	int r = (hex >> 16) & 0xFF;
	int g = (hex >> 8) & 0xFF;
	int b = (hex) & 0xFF;

	return [UIColor colorWithRed:r / 255.0f
						   green:g / 255.0f
							blue:b / 255.0f
						   alpha:1.0f];
}

- (CGColorSpaceModel)colorSpaceModel
{
	return CGColorSpaceGetModel(CGColorGetColorSpace(self.CGColor));
}

- (BOOL)canProvideRGBComponents
{
	switch (self.colorSpaceModel)
    {
		case kCGColorSpaceModelRGB:
		case kCGColorSpaceModelMonochrome:
        {
			return YES;
        }
		default:
        {
			return NO;
        }
	}
}

- (BOOL)red:(CGFloat *)red green:(CGFloat *)green blue:(CGFloat *)blue alpha:(CGFloat *)alpha
{
	const CGFloat *components = CGColorGetComponents(self.CGColor);

	CGFloat r,g,b,a;

	switch (self.colorSpaceModel)
    {
		case kCGColorSpaceModelMonochrome:
        {
			r = g = b = components[0];
			a = components[1];

            break;
        }
		case kCGColorSpaceModelRGB:
        {
			r = components[0];
			g = components[1];
			b = components[2];
			a = components[3];

            break;
        }
		default:
        {
			return NO;
        }
	}

	if (red)   *red   = r;
	if (green) *green = g;
	if (blue)  *blue  = b;
	if (alpha) *alpha = a;

	return YES;
}

- (UInt32)rgbHex
{
    NSAssert(self.canProvideRGBComponents, @"Must be a RGB color to use rgbHex");

    CGFloat r, g, b, a;

    if ( ! [self red:&r green:&g blue:&b alpha:&a])
        return 0;

    r = fminf(fmaxf(r, 0.0f), 1.0f);
    g = fminf(fmaxf(g, 0.0f), 1.0f);
    b = fminf(fmaxf(b, 0.0f), 1.0f);

    return (((int)roundf(r * 255)) << 16) | (((int)roundf(g * 255)) << 8) | (((int)roundf(b * 255)));
}

- (NSString *)hexStringFromColor
{
	return [NSString stringWithFormat:@"%0.6X", (unsigned int)(self.rgbHex)];
}

+ (UIColor *)colorWithHexString:(NSString *)hexString
{
    NSScanner *scanner = [NSScanner scannerWithString:[hexString stringByReplacingOccurrencesOfString:@"#" withString:@""]];

    unsigned hexNum;

    if ( ! [scanner scanHexInt:&hexNum])
        return nil;

    return [UIColor colorWithRGBHex:hexNum];
}

@end
