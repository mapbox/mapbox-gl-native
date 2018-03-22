#import "MGLOpenGLStyleLayer.h"

#import "MGLMapView_Private.h"
#import "MGLStyle_Private.h"
#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/math/wrap.hpp>

@interface MGLOpenGLStyleLayerRetainer ()
@property (nonatomic, weak, readwrite) MGLOpenGLStyleLayer *layer;
@property (nonatomic, assign, readwrite) NSInteger layerRetainCount;
@end

@implementation MGLOpenGLStyleLayerRetainer

- (void)dealloc {
    NSLog(@"LayerRetainer dealloc start %p", self);

    while (self.layerRetainCount) {
        [self releaseLayer];
    }

    NSLog(@"LayerRetainer dealloc end %p", self);
}

- (instancetype)initWithLayer:(MGLOpenGLStyleLayer*)styleLayer {
    if ((self = [super init])) {
        _layer = styleLayer;
    }
    NSLog(@"LayerRetainer %p init with layer %p", self, styleLayer);
    return self;
}

- (void)setOwner:(nullable void *)owner {
    void* oldOwner = _owner;

    _owner = owner;

    if (oldOwner) {
        NSLog(@"LayerRetainer %p released", self);
        CFBridgingRelease((__bridge CFTypeRef)(self));
    }

    if (_owner) {
        NSLog(@"LayerRetainer %p retained", self);
        CFBridgingRetain(self);
    }
}

- (void)retainLayer {

    if (!self.layer)
        return;

    NSAssert(self.layerRetainCount >= 0, @"");

    @autoreleasepool {
        CFBridgingRetain(self.layer);
    }
    self.layerRetainCount++;

    NSLog(@"LayerRetainer %p retained %ld", self,  self.layerRetainCount);
}


- (void)releaseLayer {
    if (!self.layer)
        return;

    NSAssert(self.layerRetainCount > 0, @"");

    self.layerRetainCount--;

    @autoreleasepool {
        CFBridgingRelease((__bridge CFTypeRef)self.layer);
    }

    NSLog(@"LayerRetainer %p released %ld", self, self.layerRetainCount);
}
@end

void MGLCustomLayerContextOwnerChangedFunction(void *context, void* owner) {
    MGLOpenGLStyleLayerRetainer *contextRetainer = (__bridge MGLOpenGLStyleLayerRetainer*)context;
    contextRetainer.owner = owner;
}

void MGLCustomLayerContextAttachFunction(void *context) {
    MGLOpenGLStyleLayerRetainer *contextRetainer = (__bridge MGLOpenGLStyleLayerRetainer*)context;
    [contextRetainer retainLayer];
}

void MGLCustomLayerContextDetachFunction(void *context) {
    MGLOpenGLStyleLayerRetainer *contextRetainer = (__bridge MGLOpenGLStyleLayerRetainer*)context;
    [contextRetainer releaseLayer];
}



/**
 Runs the preparation handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.

 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLPrepareCustomStyleLayer(void *context) {
    // Note, that the layer is retained/released by MGLStyle, ensuring that the layer
    // is alive during rendering
//    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer*)context;

    MGLOpenGLStyleLayerRetainer *retainer = (__bridge MGLOpenGLStyleLayerRetainer*)context;
    MGLOpenGLStyleLayer *layer = retainer.layer;

    [layer didMoveToMapView:layer.style.mapView];
}

/**
 Runs the drawing handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayer`.

 @param context An `MGLOpenGLStyleLayer` instance that was provided as context
    when creating an OpenGL style layer.
 */
void MGLDrawCustomStyleLayer(void *context, const mbgl::style::CustomLayerRenderParameters &params) {
//    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer *)context;
    MGLOpenGLStyleLayer *layer = ((__bridge MGLOpenGLStyleLayerRetainer*)context).layer;

    if (!layer)
    {
        NSLog(@"no layer");
        return;
    }

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
    // Note, that the layer is retained/released by MGLStyle, ensuring that the layer
    // is alive during rendering
//    MGLOpenGLStyleLayer *layer = (__bridge MGLOpenGLStyleLayer*)context;
    MGLOpenGLStyleLayerRetainer *retainer = (__bridge MGLOpenGLStyleLayerRetainer*)context;
    MGLOpenGLStyleLayer *layer = retainer.layer;

    [layer willMoveFromMapView:layer.style.mapView];
}


/**
 Function to be called when the core `CustomLayer` (not the Impl) gets deallocated.
 It's possible taht at this stage the Obj-C style layer is being deallocated (but that case is detected).
 */
//void MGLDeallocateCustomStyleLayer(mbgl::util::unique_any *peer) {
//
//    // We know that the peer object contains a LayerWrapper with a weak pointer to
//    // our custom layer. We can use this to safely access the layer, and clear out the
//    // raw pointer.
//    //
//    // If we don't do this rawLayer can become a dangling pointer (which was previously being
//    // accessed via the description method)
//
//    if (!(peer && peer->has_value()))
//        return;
//
//    LayerWrapper *wrapper = mbgl::util::any_cast<LayerWrapper>(peer);
//
//    if (!wrapper)
//        return;
//
//    // If the MGLStyleLayer is currently being dealloc'd (and trigger the CustomLayer destructor, and
//    // this function) then layer here will be nil (even though wrapper->layer may appear to be non-nil)
//    MGLStyleLayer *layer = wrapper->layer;
//
//    layer.rawLayer = NULL;
//}


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

- (BOOL)isBeingManaged {
    if (self.rawLayer == NULL) {
        return NO;
    }

    auto customLayer = self.rawLayer->template as<mbgl::style::CustomLayer>();

    if (!customLayer) {
        return NO;
    }

    MGLOpenGLStyleLayerRetainer *retainer = (__bridge MGLOpenGLStyleLayerRetainer*)(customLayer->getContext());

    return retainer.layerRetainCount > 0;
}

- (void)dealloc
{
    NSLog(@"MGLOpenGLStyleLayer dealloc %p\n", self);
}

- (instancetype)initWithIdentifier:(NSString *)identifier {

    NSLog(@"MGLOpenGLStyleLayer init %p\n", self);

    MGLOpenGLStyleLayerRetainer *retainer = [[MGLOpenGLStyleLayerRetainer alloc] initWithLayer:self];

    // Note, do not retain self here, otherwise MGLOpenGLStyleLayer will never be dealloc'd
    auto layer = std::make_unique<mbgl::style::CustomLayer>(identifier.UTF8String,
                                                            MGLPrepareCustomStyleLayer,
                                                            MGLDrawCustomStyleLayer,
                                                            MGLFinishCustomStyleLayer,

                                                            MGLCustomLayerContextOwnerChangedFunction,
                                                            MGLCustomLayerContextAttachFunction,
                                                            MGLCustomLayerContextDetachFunction,

                                                            (__bridge void*)retainer);

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

    // We need to ensure that this layer is retained, so that any references from layer impl's
    // e.g. contexts) are still valid
//    [style addToManagedLayers:self];

    [super addToStyle:style belowLayer:otherLayer];
}

- (void)removeFromStyle:(MGLStyle *)style {
    [super removeFromStyle:style];

    // We need to ensure that this layer is now released (however, if this layer is about to be
    // used by the renderer then it will released once rendering is complete)
//    [style removeFromManagedLayers:self];

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
