#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLStyleValue.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLLightAnchor) {
    MGLLightAnchorTypeMap,
    MGLLightAnchorTypeViewport
};

MGL_EXPORT
@interface MGLLight : NSObject

+ (instancetype)defaultLight;

/**
 `lightAnchorType` property.
 */
@property (nonatomic) MGLLightAnchor lightAnchor;

/**
 Values describing animated transitions to `lightAnchorType` property.
 */
@property (nonatomic) MGLTransition lightAnchorTransition;


/**
 Position.
 */
@property (nonatomic) MGLStyleValue<NSValue *> * position;

/**
 Values describing animated transitions to `position` property.
 */
@property (nonatomic) MGLTransition positionTransiton;


#if TARGET_OS_IPHONE
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *color;
#else

@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *color;
#endif

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

NS_ASSUME_NONNULL_END
