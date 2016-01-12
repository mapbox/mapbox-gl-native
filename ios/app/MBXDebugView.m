//
//  MBXDebugView.m
//  ios
//
//  Created by Mappy SA on 04/01/2016.
//
//

#import "MBXDebugView.h"

@implementation MBXDebugView

- (instancetype)initWithFrame:(CGRect)frame andWithInsets:(UIEdgeInsets) insets {
    if (self = [super initWithFrame:frame]) {
        self.insets = insets;
        self.backgroundColor = [UIColor clearColor];
        [self setUserInteractionEnabled:NO];
    }
    return self;
}

- (void)drawRect:(__unused CGRect) rect{
    UIEdgeInsets insets = self.insets;
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect insetsRect = CGRectMake(insets.left,
                                   insets.top,
                                   self.frame.size.width - insets.right - insets.left,
                                   self.frame.size.height - insets.bottom - insets.top);
    
    UIColor * redColor = [UIColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:1.0];
    
    CGContextSetStrokeColorWithColor(context, redColor.CGColor);
    CGContextStrokeRect(context, insetsRect);
    CGPoint center = CGPointMake(CGRectGetMidX(insetsRect), CGRectGetMidY(insetsRect));
    CGRect circleRect = CGRectMake(center.x - 10, center.y - 10, 20, 20);
    CGContextStrokeEllipseInRect(context, circleRect);
}

@end
