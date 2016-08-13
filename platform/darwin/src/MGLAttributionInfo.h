#import <Foundation/Foundation.h>

#import "MGLTypes.h"

#include <vector>

NS_ASSUME_NONNULL_BEGIN

/**
 Information about an attribution statement, usually a copyright or trademark
 statement, associated with a map source.
 */
@interface MGLAttributionInfo : NSObject

/**
 Parses and returns the attribution infos contained in the given HTML source
 code string.
 */
+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(NSString *)htmlString;

- (instancetype)initWithTitle:(NSString *)title URL:(NSURL *)URL;

@property (nonatomic) NSString *title;
@property (nonatomic) NSURL *URL;

@end

NS_ASSUME_NONNULL_END
