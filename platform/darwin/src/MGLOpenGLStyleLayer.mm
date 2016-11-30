#import "MGLOpenGLStyleLayer.h"

#import "MGLMapView_Private.h"

#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/math/wrap.hpp>

/**
 An `MGLOpenGLStyleLayerHandlers` object owns an OpenGL style layer’s callback
 blocks.
 */
class MGLOpenGLStyleLayerHandlers {
public:
    MGLOpenGLStyleLayerHandlers(MGLOpenGLStyleLayerPreparationHandler p,
                                MGLOpenGLStyleLayerDrawingHandler d,
                                MGLOpenGLStyleLayerCompletionHandler f)
    : prepare(p), draw(d), finish(f) {}

    MGLOpenGLStyleLayerPreparationHandler prepare;
    MGLOpenGLStyleLayerDrawingHandler draw;
    MGLOpenGLStyleLayerCompletionHandler finish;
};

/**
 Runs the preparation handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayerHandlers`.
 
 @param context An `MGLOpenGLStyleLayerHandlers` instance that was provided as
    context when creating an OpenGL style layer.
 */
void MGLPrepareCustomStyleLayer(void *context) {
    MGLOpenGLStyleLayerPreparationHandler prepare = reinterpret_cast<MGLOpenGLStyleLayerHandlers *>(context)->prepare;
    if (prepare) {
        prepare();
    }
}

/**
 Runs the drawing handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayerHandlers`.
 
 @param context An `MGLOpenGLStyleLayerHandlers` instance that was provided as
    context when creating an OpenGL style layer.
 */
void MGLDrawCustomStyleLayer(void *context, const mbgl::style::CustomLayerRenderParameters &params) {
    MGLOpenGLStyleLayerDrawingHandler draw = reinterpret_cast<MGLOpenGLStyleLayerHandlers *>(context)->draw;
    if (draw) {
        draw({
            .size = CGSizeMake(params.width, params.height),
            .centerCoordinate = CLLocationCoordinate2DMake(params.latitude, params.longitude),
            .zoomLevel = params.zoom,
            .direction = mbgl::util::wrap(params.bearing, 0., 360.),
            .pitch = params.pitch,
            .perspectiveSkew = params.altitude,
        });
    }
}

/**
 Runs the completion handler block contained in the given context, which is
 implicitly an instance of `MGLOpenGLStyleLayerHandlers`.
 
 @param context An `MGLOpenGLStyleLayerHandlers` instance that was provided as
    context when creating an OpenGL style layer.
 */
void MGLFinishCustomStyleLayer(void *context) {
    MGLOpenGLStyleLayerHandlers *handlers = reinterpret_cast<MGLOpenGLStyleLayerHandlers *>(context);
    MGLOpenGLStyleLayerCompletionHandler finish = handlers->finish;
    if (finish) {
        finish();
    }
    delete handlers;
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
 @param preparation A block that sets up OpenGL state for the layer.
 @param drawing A block that draws a single frame of the layer.
 @param completion A block that cleans up OpenGL resources used by the layer.
 @return An initialized OpenGL style layer.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier preparationHandler:(MGLOpenGLStyleLayerPreparationHandler)preparation drawingHandler:(MGLOpenGLStyleLayerDrawingHandler)drawing completionHandler:(MGLOpenGLStyleLayerCompletionHandler)completion {
    if (self = [super initWithIdentifier:identifier]) {
        MGLOpenGLStyleLayerHandlers *context = new MGLOpenGLStyleLayerHandlers(preparation, drawing, completion);
        auto layer = std::make_unique<mbgl::style::CustomLayer>(identifier.UTF8String,
                                                                MGLPrepareCustomStyleLayer, MGLDrawCustomStyleLayer, MGLFinishCustomStyleLayer, context);
        _pendingLayer = std::move(layer);
        _rawLayer = _pendingLayer.get();
    }
    return self;
}

#pragma mark - Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView {
    [self addToMapView:mapView belowLayer:nil];
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer {
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{ otherLayer.identifier.UTF8String };
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    if (!removedLayer) {
        return;
    }
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::CustomLayer> &>(removedLayer));
    _rawLayer = _pendingLayer.get();
}

@end

@implementation MGLMapView (MGLOpenGLStyleLayerAdditions)

/**
 Forces the map view associated with this style to redraw its style layers,
 causing any OpenGL sources’ drawing handlers to be run.
 */
- (void)setStyleLayersNeedDisplay {
    [self setNeedsGLDisplay];
}

@end
