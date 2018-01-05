#import "MGLOpenGLStyleLayer.h"

#import "MGLMapView_Private.h"
#import "MGLStyle_Private.h"
#import "MGLStyleLayer_Private.h"

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
    [layer didMoveToMapView:layer.style.mapView];
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
        .fieldOfView = static_cast<CGFloat>(params.fieldOfView),
    };
    [layer drawInMapView:layer.style.mapView withContext:drawingContext];
}

/**
 Runs the completion handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.

 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLFinishCustomStyleLayer(void *context) {
    MGLOpenGLStyleLayer *layer = (__bridge_transfer MGLOpenGLStyleLayer *)context;
    [layer willMoveFromMapView:layer.style.mapView];
}

/**
 An `MGLOpenGLStyleLayer` is a style layer that is rendered by OpenGL code that
 you provide.

 By default, this class does nothing. You can subclass this class to provide
 custom OpenGL drawing code that is run on each frame of the map. Your subclass
 should override the `-didMoveToMapView:`, `-willMoveFromMapView:`, and
 `-drawInMapView:withContext:` methods.

 You can access an existing OpenGL style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new OpenGL style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 @warning This API is undocumented and therefore unsupported. It may change at
    any time without notice.
 */
@interface MGLOpenGLStyleLayer ()

@property (nonatomic, readonly) mbgl::style::CustomLayer *rawLayer;

/**
 The style currently containing the layer.

 If the layer is not currently part of any style, this property is
 set to `nil`.
 */
@property (nonatomic, weak, readwrite) MGLStyle *style;

@end

@implementation MGLOpenGLStyleLayer

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
    auto layer = std::make_unique<mbgl::style::CustomLayer>(identifier.UTF8String,
                                                            MGLPrepareCustomStyleLayer,
                                                            MGLDrawCustomStyleLayer,
                                                            MGLFinishCustomStyleLayer,
                                                            (__bridge_retained void *)self);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::CustomLayer *)rawLayer {
    return (mbgl::style::CustomLayer *)super.rawLayer;
}

#pragma mark - Adding to and removing from a map view

- (void)setStyle:(MGLStyle *)style {
    if (_style && style) {
        [NSException raise:@"MGLLayerReuseException"
                    format:@"%@ cannot be added to more than one MGLStyle at a time.", self];
    }
    _style = style;
}

- (void)addToStyle:(MGLStyle *)style belowLayer:(MGLStyleLayer *)otherLayer {
    self.style = style;
    [super addToStyle:style belowLayer:otherLayer];
}

- (void)removeFromStyle:(MGLStyle *)style {
    [super removeFromStyle:style];
    self.style = nil;
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
    [self.style.mapView setNeedsGLDisplay];
}

@end
