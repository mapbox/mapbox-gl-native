#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSData (MGLAdditions)

- (NSData *)mgl_compressedData;

- (NSData *)mgl_decompressedData;

@end

NS_ASSUME_NONNULL_END
