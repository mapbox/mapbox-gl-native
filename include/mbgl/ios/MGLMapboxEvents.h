//
//  MapboxEvents.h
//  MapboxEvents
//
//  Created by Brad Leege on 3/5/15.
//  Copyright (c) 2015 Mapbox. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MGLMapboxEvents : NSObject

@property (atomic) NSInteger flushAt;
@property (atomic) NSInteger flushAfter;
@property (atomic) NSString *api;
@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;

+ (id)sharedManager;

- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

@end