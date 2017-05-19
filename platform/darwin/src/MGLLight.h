#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLStyleValue.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLLightAnchorType) {
    MGLLightAnchorTypeMap,
    MGLLightAnchorTypeViewport
};

MGL_EXPORT
@interface MGLLight : NSObject

/**
 `lightAnchorType` property.
 */
@property (nonatomic) MGLStyleValue<NSValue *> *lightAnchorType;

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
@property (nonatomic) MGLStyleValue<UIColor *> *color;

/**
 Values describing animated transitions to `color` property.
 */
@property (nonatomic) MGLTransition colorTransiton;


/**
 Specifies the receiver's intensity.
 */
@property(nonatomic) MGLStyleValue<NSNumber *> *intensity;

/**
 Values describing animated transitions to `intensity` property.
 */
@property (nonatomic) MGLTransition intensityTransition;

@end

@interface NSValue (MGLLightAdditions)

+ (instancetype)valueWithMGLLightAnchorType:(MGLLightAnchorType)lightAnchorType;

@end

NS_ASSUME_NONNULL_END
