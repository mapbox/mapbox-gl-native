#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLStyleValue.h"
#import "MGLTypes.h"

typedef NS_ENUM(NSInteger, MGLLightAnchorType) {
    MGLLightAnchorTypeMap = 0,
    MGLLightAnchorTypeViewport
};

@interface MGLLight : NSObject

/**
 Values describing animated transitions to `intensity` property.
 */
@property (nonatomic) MGLLightAnchorType lightAnchorType;

/**
 Values describing animated transitions to `lightAnchorType` property.
 */
@property (nonatomic) MGLTransition lightAnchorTypeTransition;


/**
 Position.
 */
@property (nonatomic) MGLPosition position;

/**
 Values describing animated transitions to `position` property.
 */
@property (nonatomic) MGLTransition positionTransiton;


/**
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *color;

/**
 Values describing animated transitions to `color` property.
 */
@property (nonatomic) MGLTransition colorTransiton;


/**
 Specifies the receiver's intensity.
 */
@property(nonatomic) CGFloat intensity;

/**
 Values describing animated transitions to `intensity` property.
 */
@property (nonatomic) MGLTransition intensityTransition;

@end
