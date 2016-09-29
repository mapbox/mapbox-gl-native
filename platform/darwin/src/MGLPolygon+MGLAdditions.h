//
//  MGLPolygonFeature+MGLAdditions.h
//  ios
//
//  Created by Mapbox on 9/30/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import <Mapbox/Mapbox.h>

@interface MGLPolygon (MGLAdditions)

- (NS_ARRAY_OF(id) *)mgl_coordinates;

@end
