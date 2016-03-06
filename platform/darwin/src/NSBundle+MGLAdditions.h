#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

void mgl_linkBundleCategory();

@interface NSBundle (MGLAdditions)

/// Returns the bundle containing the SDK’s classes and Info.plist file.
+ (instancetype)mgl_frameworkBundle;

+ (nullable NSString *)mgl_frameworkBundleIdentifier;

+ (nullable NS_DICTIONARY_OF(NSString *, id) *)mgl_frameworkInfoDictionary;

/// The relative path to the directory containing the SDK’s resource files, or
/// `nil` if the files are located directly within the bundle’s root directory.
@property (readonly, copy, nullable) NSString *mgl_resourcesDirectory;

@end

NS_ASSUME_NONNULL_END
