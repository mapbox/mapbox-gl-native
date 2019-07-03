#import "MGLCompassButton.h"
#import "MGLCompassDirectionFormatter.h"

#import <Mapbox/MGLGeometry.h>

#import "MGLMapView_Private.h"
#import "UIImage+MGLAdditions.h"
#import "NSBundle+MGLAdditions.h"

#include <mbgl/math/wrap.hpp>

@interface MGLCompassButton ()

@property (nonatomic) MGLCompassDirectionFormatter *accessibilityCompassFormatter;

@end

@implementation MGLCompassButton

+ (instancetype)compassButton {
    return [[MGLCompassButton alloc] init];
}

- (instancetype)init {
    if (self = [super init]) {
        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    self.image = self.compassImage;

    self.compassVisibility = MGLOrnamentVisibilityAdaptive;

    self.alpha = 0;
    self.userInteractionEnabled = YES;
    self.translatesAutoresizingMaskIntoConstraints = NO;

    self.accessibilityTraits = UIAccessibilityTraitButton;
    self.accessibilityLabel = NSLocalizedStringWithDefaultValue(@"COMPASS_A11Y_LABEL", nil, nil, @"Compass", @"Accessibility label");
    self.accessibilityHint = NSLocalizedStringWithDefaultValue(@"COMPASS_A11Y_HINT", nil, nil, @"Rotates the map to face due north", @"Accessibility hint");

    self.accessibilityCompassFormatter = [[MGLCompassDirectionFormatter alloc] init];
    self.accessibilityCompassFormatter.unitStyle = NSFormattingUnitStyleLong;

    [self sizeToFit];
}

- (UIImage *)compassImage {
    UIImage *scaleImage = [UIImage mgl_resourceImageNamed:@"Compass"];
    UIGraphicsBeginImageContextWithOptions(scaleImage.size, NO, UIScreen.mainScreen.scale);
    [scaleImage drawInRect:{CGPointZero, scaleImage.size}];

    NSAttributedString *north = [[NSAttributedString alloc] initWithString:NSLocalizedStringWithDefaultValue(@"COMPASS_NORTH", nil, nil, @"N", @"Compass abbreviation for north") attributes:@{
        NSFontAttributeName: [UIFont systemFontOfSize:11 weight:UIFontWeightUltraLight],
        NSForegroundColorAttributeName: [UIColor whiteColor],
    }];
    CGRect stringRect = CGRectMake((scaleImage.size.width - north.size.width) / 2,
                                   scaleImage.size.height * 0.435,
                                   north.size.width, north.size.height);
    [north drawInRect:stringRect];

    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (void)updateCompassWithDirection:(CLLocationDirection)direction
{
    CLLocationDirection plateDirection = mbgl::util::wrap(-direction, 0., 360.);
    self.transform = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(plateDirection));

    self.isAccessibilityElement = direction > 0;
    self.accessibilityValue = [self.accessibilityCompassFormatter stringFromDirection:direction];

    switch (self.compassVisibility) {
        case MGLOrnamentVisibilityAdaptive:
            if (direction > 0 && self.alpha < 1) {
                [self showCompass];
            } else if (direction == 0 && self.alpha > 0) {
                [self hideCompass];
            }
            break;
        case MGLOrnamentVisibilityVisible:
            [self showCompass];
            break;
        case MGLOrnamentVisibilityHidden:
            [self hideCompass];
            break;
    }
}

- (void)showCompass {
    [self animateToAlpha:1];
}

- (void)hideCompass {
    [self animateToAlpha:0];
}

- (void)animateToAlpha:(CGFloat)alpha {
    [UIView animateWithDuration:MGLAnimationDuration delay:0 options:UIViewAnimationOptionBeginFromCurrentState animations:^{
         self.alpha = alpha;
     } completion:nil];
}

@end
