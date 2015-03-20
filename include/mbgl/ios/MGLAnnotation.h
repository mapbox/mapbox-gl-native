//
//  MGLAnnotation.h
//  mbgl
//
//  Created by Minh Nguyen on 2015-03-04.
//
//

#import <Foundation/Foundation.h>

@protocol MGLAnnotation <NSObject, NSCopying>

//! The receiver’s center, expressed as a coordinate on the containing map.
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

@optional

//! The receiver’s title string.
@property (nonatomic, readonly, copy) NSString *title;

//! The receiver’s subtitle string.
@property (nonatomic, readonly, copy) NSString *subtitle;

@end
