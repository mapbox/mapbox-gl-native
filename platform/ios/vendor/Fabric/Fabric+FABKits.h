//
//  Fabric+FABKits.h
//
//  Copyright (c) 2015 Twitter. All rights reserved.
//

#import "Fabric.h"

@protocol FABKit;
// Use this category for methods that kits can call on Fabric.
@interface Fabric (FABKits)

/**
 *  Returns a dictionary containing the kit configuration info for the provided kit.
 *  The configuration information is parsed from the application's Info.plist. This
 *  method is primarily intended to be used by kits to retrieve their configuration.
 *
 *  @param kitClass The class of the kit whose configuration should be returned.
 *  It should conform to the FABKit protocol.
 *
 *  @return A dictionary containing kit specific configuration information or nil if none exists.
 */
+ (fab_nonnull NSDictionary *)configurationDictionaryForKitClass:(fab_nonnull Class)kitClass;

@end
