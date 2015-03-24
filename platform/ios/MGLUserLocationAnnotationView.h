//
//  MGLUserLocationAnnotationView.h
//  mbgl
//
// Copyright (c) 2008-2013, Route-Me Contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLUserLocation.h"

@class MGLMapView;

/** The MGLUserLocationAnnotationView class defines a specific type of annotation that identifies the user’s current location. You do not create instances of this class directly. Instead, you retrieve an existing MGLUserLocationAnnotationView object from the userLocation property of the map view displayed in your application. */
@interface MGLUserLocationAnnotationView : UIImageView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, strong) MGLUserLocation *annotation;

/** @name Determining the User’s Position */

/** A Boolean value indicating whether the user’s location is currently being updated. (read-only) */
@property (nonatomic, readonly, getter=isUpdating) BOOL updating;

@property (nonatomic, strong, readonly) CALayer *haloLayer;

- (instancetype)initInMapView:(MGLMapView *)mapView;
- (void)setupLayers;

@end
