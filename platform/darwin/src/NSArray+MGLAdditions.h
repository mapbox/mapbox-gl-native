#import <Foundation/Foundation.h>

#import <mbgl/util/feature.hpp>

@interface NSArray (MGLAdditions)

- (std::vector<mbgl::Value>)mgl_vector;

/** Returns a string resulting from inserting a separator between each attributed string in the array */
- (NSAttributedString *)mgl_attributedComponentsJoinedByString:(NSString *)separator;

@end
