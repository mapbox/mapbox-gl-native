//
//  FABKitProtocol.h
//
//  Copyright (c) 2015 Twitter. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *  Protocol that a class in a Fabric Kit must conform to to provide information to Fabric at runtime.
 */
@protocol FABKit <NSObject>

@required

/**
 *  Required. The globally unique identifier of the Kit.
 *  We encourage the use of reverse-DNS notation.
 *  Example: @"io.fabric.sdk.ios"
 */
+ (NSString *)bundleIdentifier;

/**
 *  Required. Must return the current version of the Kit that is being used at runtime.
 *  We encourage the use of semantic versioning (http://semver.org/), without prefixing the version with a "v".
 *  This is commonly referred to as the "marketing version".
 *  Example: @"1.2.3"
 */
+ (NSString *)kitDisplayVersion;

@optional

/**
 *  The build version of the kit. Should be monotonically increasing and unique.
 *  Example: 137
 */
+ (NSString *)kitBuildVersion;

/**
 *  Perform any necessary initialization.
 *  This method will be invoked on the Kit when the user calls +[Fabric initializeKits].
 *  @note This method being called does not necessarily imply that the developer has started using the Kit yet.
 */
+ (void)initializeIfNeeded;

@end
