#import "MGLOpenGLStyleLayer.h"

#import "MGLMapView_Private.h"
#import "MGLStyle_Private.h"

#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/math/wrap.hpp>

/**
 Runs the preparation handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.
 
 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLPrepareCustomStyleLayer(void *context) {
    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer *)context;
    [layer didMoveToMapView:layer.mapView];
}

/**
 Runs the drawing handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.
 
 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLDrawCustomStyleLayer(void *context, const mbgl::style::CustomLayerRenderParameters &params) {
    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer *)context;
    MGLStyleLayerDrawingContext drawingContext = {
        .size = CGSizeMake(params.width, params.height),
        .centerCoordinate = CLLocationCoordinate2DMake(params.latitude, params.longitude),
        .zoomLevel = params.zoom,
        .direction = mbgl::util::wrap(params.bearing, 0., 360.),
        .pitch = static_cast<CGFloat>(params.pitch),
        .perspectiveSkew = static_cast<CGFloat>(params.altitude),
    };
    [layer drawInMapView:layer.mapView withContext:drawingContext];
}

/**
 Runs the completion handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.
 
 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLFinishCustomStyleLayer(void *context) {
    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer *)context;
    [layer willMoveFromMapView:layer.mapView];
}

/**
 An `MGLOpenGLStyleLayer` is a style layer that is rendered by OpenGL code in
 Objective-C blocks or Swift closures that you specify. You may initialize a new
 OpenGL style layer to add to an `MGLStyle` or obtain one from an `MGLMapView`’s
 current style using the `-[MGLStyle layerWithIdentifier:]` method.
 
 @warning This API is undocumented and therefore unsupported. It may change at
    any time without notice.
 */
@interface MGLOpenGLStyleLayer ()

@property (nonatomic) mbgl::style::CustomLayer *rawLayer;

/**
 The map view whose style currently contains the layer.
 
 If the layer is not currently part of any map view’s style, this property is
 set to `nil`.
 */
@property (nonatomic, weak, readwrite) MGLMapView *mapView;

@end

@implementation MGLOpenGLStyleLayer {
    std::unique_ptr<mbgl::style::CustomLayer> _pendingLayer;
}

/**
 Returns an OpenGL style layer object initialized with the given identifier.
 
 After initializing and configuring the style layer, add it to a map view’s
 style using the `-[MGLStyle addLayer:]` or
 `-[MGLStyle insertLayer:belowLayer:]` method.
 
 @param identifier A string that uniquely identifies the layer in the style to
    which it is added.
 @return An initialized OpenGL style layer.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier {
    if (self = [super initWithIdentifier:identifier]) {
        auto layer = std::make_unique<mbgl::style::CustomLayer>(identifier.UTF8String,
                                                                MGLPrepareCustomStyleLayer,
                                                                MGLDrawCustomStyleLayer,
                                                                MGLFinishCustomStyleLayer,
                                                                (__bridge void *)self);
        _pendingLayer = std::move(layer);
        _rawLayer = _pendingLayer.get();
    }
    return self;
}

#pragma mark - Adding to and removing from a map view

- (void)setMapView:(MGLMapView *)mapView {
    if (_mapView && mapView) {
        [NSException raise:@"MGLLayerReuseException"
                    format:@"%@ cannot be added to more than one MGLStyle at a time.", self];
    }
    _mapView.style.openGLLayers[self.identifier] = nil;
    _mapView = mapView;
    _mapView.style.openGLLayers[self.identifier] = self;
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer {
    self.mapView = mapView;
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{ otherLayer.identifier.UTF8String };
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    self.mapView = nil;
    if (!removedLayer) {
        return;
    }
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::CustomLayer> &>(removedLayer));
    _rawLayer = _pendingLayer.get();
}

/**
 Called immediately after a layer is added to a map view’s style.
 
 This method is intended to be overridden in a subclass. You can use this method
 to perform any setup work before the layer is used to draw a frame. For
 example, you might use this method to compile an OpenGL shader. The default
 implementation of this method does nothing.
 
 Any resource acquired in this method must be released in
 `-willMoveFromMapView:`.
 
 @param mapView The map view to whose style the layer has been added.
 */
- (void)didMoveToMapView:(MGLMapView *)mapView {
    
}

/**
 Called immediately before a layer is removed from a map view’s style.
 
 This method is intended to be overridden in a subclass. You can use this method
 to perform any teardown work once the layer has drawn its last frame and is
 about to be removed from the style. The default implementation of this method
 does nothing.
 
 This method may be called even if `-didMoveToMapView:` has not been called.
 
 @param mapView The map view from whose style the layer is about to be removed.
 */
- (void)willMoveFromMapView:(MGLMapView *)mapView {
    
}

/**
 Called each time the layer needs to draw a new frame in a map view.
 
 This method is intended to be overridden in a subclass. You can use this method
 to draw the layer’s content. The default implementation of this method does
 nothing.
 
 Your implementation should not make any assumptions about the OpenGL state,
 other than that the current OpenGL context is active. It may make changes to
 the OpenGL state. It is not required to reset values such as the depth mask,
 stencil mask, or corresponding test flags to their original values.
 
 Be sure to draw your fragments with a <var>z</var> value of 1 to take advantage
 of the opaque fragment culling, in case the style contains any opaque layers
 above this layer.
 
 @param mapView The map view to which the layer draws.
 @param context A context structure with information defining the frame to draw.
 */
- (void)drawInMapView:(MGLMapView *)mapView withContext:(MGLStyleLayerDrawingContext)context {
    
}

/**
 Forces the map view associated with this style to redraw the receiving layer,
 causing the `-drawInMapView:withContext:` method to be called.
 */
- (void)setNeedsDisplay {
    [self.mapView setNeedsGLDisplay];
}

@end
