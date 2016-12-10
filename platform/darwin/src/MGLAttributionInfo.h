#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Information about an attribution statement, usually a copyright or trademark
 statement, associated with a map content source.
 */
@interface MGLAttributionInfo : NSObject

/**
 Parses and returns the attribution infos contained in the given HTML source
 code string.
 
 @param htmlString The HTML source code to parse.
 @param fontSize The default text size in points.
 @param linkColor The default link color.
 */
+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

- (instancetype)initWithTitle:(NSAttributedString *)title URL:(nullable NSURL *)URL;

@property (nonatomic) NSAttributedString *title;
@property (nonatomic, nullable) NSURL *URL;
@property (nonatomic, getter=isFeedbackLink) BOOL feedbackLink;

- (nullable NSURL *)feedbackURLAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel;

@end

@interface NSMutableArray (MGLAttributionInfoAdditions)

/**
 Adds the given attribution info object to the receiver as long as it isn’t
 redundant to any object already in the receiver. Any existing object that is
 redundant to the given object is replaced by the given object.
 
 @param info The info object to add to the receiver.
 @return True if the given info object was added to the receiver.
 */
- (void)growArrayByAddingAttributionInfo:(MGLAttributionInfo *)info;

/**
 Adds each of the given attribution info objects to the receiver as long as it
 isn’t redundant to any object already in the receiver. Any existing object that
 is redundant to the given object is replaced by the given object.
 
 @param infos An array of info objects to add to the receiver.
 */
- (void)growArrayByAddingAttributionInfosFromArray:(NS_ARRAY_OF(MGLAttributionInfo *) *)infos;

@end

NS_ASSUME_NONNULL_END
