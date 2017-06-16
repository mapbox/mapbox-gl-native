#import "MGLStyle_Private.h"

#import "MGLMapView_Private.h"
#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import "MGLFillExtrusionStyleLayer.h"
#import "MGLLineStyleLayer.h"
#import "MGLCircleStyleLayer.h"
#import "MGLSymbolStyleLayer.h"
#import "MGLRasterStyleLayer.h"
#import "MGLBackgroundStyleLayer.h"
#import "MGLOpenGLStyleLayer.h"

#import "MGLStyle_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLSource_Private.h"
#import "MGLLight_Private.h"

#import "NSDate+MGLAdditions.h"

#import "MGLSource.h"
#import "MGLTileSource_Private.h"
#import "MGLVectorSource.h"
#import "MGLRasterSource.h"
#import "MGLShapeSource.h"
#import "MGLImageSource.h"

#import "MGLAttributionInfo_Private.h"

#include <mbgl/map/map.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/image_source.hpp>

#if TARGET_OS_IPHONE
    #import "UIImage+MGLAdditions.h"
#else
    #import "NSImage+MGLAdditions.h"
#endif

@interface MGLStyle()

@property (nonatomic, readwrite, weak) MGLMapView *mapView;
@property (readonly, copy, nullable) NSURL *URL;
@property (nonatomic, readwrite, strong) NS_MUTABLE_DICTIONARY_OF(NSString *, MGLOpenGLStyleLayer *) *openGLLayers;

@end

@implementation MGLStyle

#pragma mark Default style URLs

/// @param name The style’s marketing name, written in lower camelCase.
/// @param fileName The last path component in the style’s URL, excluding the version suffix.
#define MGL_DEFINE_STYLE(name, fileName) \
    static NSURL *MGLStyleURL_##name; \
    + (NSURL *)name##StyleURL { \
        static dispatch_once_t onceToken; \
        dispatch_once(&onceToken, ^{ \
            MGLStyleURL_##name = [self name##StyleURLWithVersion:mbgl::util::default_styles::name.currentVersion]; \
        }); \
        return MGLStyleURL_##name; \
    } \
    \
    + (NSURL *)name##StyleURL##WithVersion:(NSInteger)version { \
        return [NSURL URLWithString:[@"mapbox://styles/mapbox/" #fileName "-v" stringByAppendingFormat:@"%li", (long)version]]; \
    }

MGL_DEFINE_STYLE(streets, streets)
MGL_DEFINE_STYLE(outdoors, outdoors)
MGL_DEFINE_STYLE(light, light)
MGL_DEFINE_STYLE(dark, dark)
MGL_DEFINE_STYLE(satellite, satellite)
MGL_DEFINE_STYLE(satelliteStreets, satellite-streets)
MGL_DEFINE_STYLE(trafficDay, traffic-day)
MGL_DEFINE_STYLE(trafficNight, traffic-night)

// Make sure all the styles listed in mbgl::util::default_styles::orderedStyles
// are defined above and also declared in MGLStyle.h.
static_assert(8 == mbgl::util::default_styles::numOrderedStyles,
              "mbgl::util::default_styles::orderedStyles and MGLStyle have different numbers of styles.");

// Hybrid has been renamed Satellite Streets, so the last Hybrid version is hard-coded here.
static NSURL *MGLStyleURL_hybrid;
+ (NSURL *)hybridStyleURL {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        MGLStyleURL_hybrid = [NSURL URLWithString:@"mapbox://styles/mapbox/satellite-hybrid-v8"];
    });
    return MGLStyleURL_hybrid;
}

// Emerald is no longer getting new versions as a default style, so the current version is hard-coded here.
static NSURL *MGLStyleURL_emerald;
+ (NSURL *)emeraldStyleURL {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        MGLStyleURL_emerald = [NSURL URLWithString:@"mapbox://styles/mapbox/emerald-v8"];
    });
    return MGLStyleURL_emerald;
}

#pragma mark -

- (instancetype)initWithMapView:(MGLMapView *)mapView {
    if (self = [super init]) {
        _mapView = mapView;
        _openGLLayers = [NSMutableDictionary dictionary];
    }
    return self;
}

- (NSURL *)URL {
    return [NSURL URLWithString:@(self.mapView.mbglMap->getStyleURL().c_str())];
}

- (NSString *)name {
    std::string name = self.mapView.mbglMap->getStyleName();
    return name.empty() ? nil : @(name.c_str());
}

#pragma mark Sources

- (NS_SET_OF(__kindof MGLSource *) *)sources {
    auto rawSources = self.mapView.mbglMap->getSources();
    NS_MUTABLE_SET_OF(__kindof MGLSource *) *sources = [NSMutableSet setWithCapacity:rawSources.size()];
    for (auto rawSource = rawSources.begin(); rawSource != rawSources.end(); ++rawSource) {
        MGLSource *source = [self sourceFromMBGLSource:*rawSource];
        [sources addObject:source];
    }
    return sources;
}

- (void)setSources:(NS_SET_OF(__kindof MGLSource *) *)sources {
    for (MGLSource *source in self.sources) {
        [self removeSource:source];
    }
    for (MGLSource *source in sources) {
        [self addSource:source];
    }
}

- (NSUInteger)countOfSources {
    auto rawSources = self.mapView.mbglMap->getSources();
    return rawSources.size();
}

- (MGLSource *)memberOfSources:(MGLSource *)object {
    return [self sourceWithIdentifier:object.identifier];
}

- (MGLSource *)sourceWithIdentifier:(NSString *)identifier
{
    auto rawSource = self.mapView.mbglMap->getSource(identifier.UTF8String);
    return rawSource ? [self sourceFromMBGLSource:rawSource] : nil;
}

- (MGLSource *)sourceFromMBGLSource:(mbgl::style::Source *)rawSource {
    if (MGLSource *source = rawSource->peer.empty() ? nil : mbgl::any_cast<SourceWrapper>(rawSource->peer).source) {
        return source;
    }

    // TODO: Fill in options specific to the respective source classes
    // https://github.com/mapbox/mapbox-gl-native/issues/6584
    if (auto vectorSource = rawSource->as<mbgl::style::VectorSource>()) {
        return [[MGLVectorSource alloc] initWithRawSource:vectorSource];
    } else if (auto geoJSONSource = rawSource->as<mbgl::style::GeoJSONSource>()) {
        return [[MGLShapeSource alloc] initWithRawSource:geoJSONSource];
    } else if (auto rasterSource = rawSource->as<mbgl::style::RasterSource>()) {
        return [[MGLRasterSource alloc] initWithRawSource:rasterSource];
    } else if (auto imageSource = rawSource->as<mbgl::style::ImageSource>()) {
        return [[MGLImageSource alloc] initWithRawSource:imageSource];
    } else {
        return [[MGLSource alloc] initWithRawSource:rawSource];
    }
}

- (void)addSource:(MGLSource *)source
{
    if (!source.rawSource) {
        [NSException raise:NSInvalidArgumentException format:
         @"The source %@ cannot be added to the style. "
         @"Make sure the source was created as a member of a concrete subclass of MGLSource.",
         source];
    }

    try {
        [source addToMapView:self.mapView];
    } catch (std::runtime_error & err) {
        [NSException raise:@"MGLRedundantSourceIdentifierException" format:@"%s", err.what()];
    }
}

- (void)removeSource:(MGLSource *)source
{
    if (!source.rawSource) {
        [NSException raise:NSInvalidArgumentException format:
         @"The source %@ cannot be removed from the style. "
         @"Make sure the source was created as a member of a concrete subclass of MGLSource.",
         source];
    }
    [source removeFromMapView:self.mapView];
}

- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    // It’d be incredibly convenient to use -sources here, but this operation
    // depends on the sources being sorted in ascending order by creation, as
    // with the std::vector used in mbgl.
    auto rawSources = self.mapView.mbglMap->getSources();
    NSMutableArray *infos = [NSMutableArray arrayWithCapacity:rawSources.size()];
    for (auto rawSource = rawSources.begin(); rawSource != rawSources.end(); ++rawSource) {
        MGLTileSource *source = (MGLTileSource *)[self sourceFromMBGLSource:*rawSource];
        if (![source isKindOfClass:[MGLTileSource class]]) {
            continue;
        }

        NSArray *tileSetInfos = [source attributionInfosWithFontSize:fontSize linkColor:linkColor];
        [infos growArrayByAddingAttributionInfosFromArray:tileSetInfos];
    }
    return infos;
}

#pragma mark Style layers

- (NS_ARRAY_OF(__kindof MGLStyleLayer *) *)layers
{
    auto layers = self.mapView.mbglMap->getLayers();
    NS_MUTABLE_ARRAY_OF(__kindof MGLStyleLayer *) *styleLayers = [NSMutableArray arrayWithCapacity:layers.size()];
    for (auto layer : layers) {
        MGLStyleLayer *styleLayer = [self layerFromMBGLLayer:layer];
        [styleLayers addObject:styleLayer];
    }
    return styleLayers;
}

- (void)setLayers:(NS_ARRAY_OF(__kindof MGLStyleLayer *) *)layers {
    for (MGLStyleLayer *layer in self.layers) {
        [self removeLayer:layer];
    }
    for (MGLStyleLayer *layer in layers) {
        [self addLayer:layer];
    }
}

- (NSUInteger)countOfLayers
{
    return self.mapView.mbglMap->getLayers().size();
}

- (MGLStyleLayer *)objectInLayersAtIndex:(NSUInteger)index
{
    auto layers = self.mapView.mbglMap->getLayers();
    if (index >= layers.size()) {
        [NSException raise:NSRangeException
                    format:@"No style layer at index %lu.", (unsigned long)index];
        return nil;
    }
    auto layer = layers.at(index);
    return [self layerFromMBGLLayer:layer];
}

- (void)getLayers:(MGLStyleLayer **)buffer range:(NSRange)inRange
{
    auto layers = self.mapView.mbglMap->getLayers();
    if (NSMaxRange(inRange) > layers.size()) {
        [NSException raise:NSRangeException
                    format:@"Style layer range %@ is out of bounds.", NSStringFromRange(inRange)];
    }
    NSUInteger i = 0;
    for (auto layer = *(layers.rbegin() + inRange.location); i < inRange.length; ++layer, ++i) {
        MGLStyleLayer *styleLayer = [self layerFromMBGLLayer:layer];
        buffer[i] = styleLayer;
    }
}

- (void)insertObject:(MGLStyleLayer *)styleLayer inLayersAtIndex:(NSUInteger)index
{
    if (!styleLayer.rawLayer) {
        [NSException raise:NSInvalidArgumentException format:
         @"The style layer %@ cannot be inserted into the style. "
         @"Make sure the style layer was created as a member of a concrete subclass of MGLStyleLayer.",
         styleLayer];
    }
    auto layers = self.mapView.mbglMap->getLayers();
    if (index > layers.size()) {
        [NSException raise:NSRangeException
                    format:@"Cannot insert style layer at out-of-bounds index %lu.", (unsigned long)index];
    } else if (index == 0) {
        try {
            MGLStyleLayer *sibling = layers.size() ? [self layerFromMBGLLayer:layers.at(0)] : nil;
            [styleLayer addToMapView:self.mapView belowLayer:sibling];
        } catch (const std::runtime_error & err) {
            [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
        }
    } else {
        try {
            MGLStyleLayer *sibling = [self layerFromMBGLLayer:layers.at(index)];
            [styleLayer addToMapView:self.mapView belowLayer:sibling];
        } catch (std::runtime_error & err) {
            [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
        }
    }
}

- (void)removeObjectFromLayersAtIndex:(NSUInteger)index
{
    auto layers = self.mapView.mbglMap->getLayers();
    if (index >= layers.size()) {
        [NSException raise:NSRangeException
                    format:@"Cannot remove style layer at out-of-bounds index %lu.", (unsigned long)index];
    }
    auto layer = layers.at(index);
    MGLStyleLayer *styleLayer = [self layerFromMBGLLayer:layer];
    [styleLayer removeFromMapView:self.mapView];
}

- (MGLStyleLayer *)layerFromMBGLLayer:(mbgl::style::Layer *)rawLayer
{
    NSParameterAssert(rawLayer);

    if (MGLStyleLayer *layer = rawLayer->peer.empty() ? nil : mbgl::any_cast<LayerWrapper>(rawLayer->peer).layer) {
        return layer;
    }

    if (auto fillLayer = rawLayer->as<mbgl::style::FillLayer>()) {
        return [[MGLFillStyleLayer alloc] initWithRawLayer:fillLayer];
    } else if (auto fillExtrusionLayer = rawLayer->as<mbgl::style::FillExtrusionLayer>()) {
        return [[MGLFillExtrusionStyleLayer alloc] initWithRawLayer:fillExtrusionLayer];
    } else if (auto lineLayer = rawLayer->as<mbgl::style::LineLayer>()) {
        return [[MGLLineStyleLayer alloc] initWithRawLayer:lineLayer];
    } else if (auto symbolLayer = rawLayer->as<mbgl::style::SymbolLayer>()) {
        return [[MGLSymbolStyleLayer alloc] initWithRawLayer:symbolLayer];
    } else if (auto rasterLayer = rawLayer->as<mbgl::style::RasterLayer>()) {
        return [[MGLRasterStyleLayer alloc] initWithRawLayer:rasterLayer];
    } else if (auto circleLayer = rawLayer->as<mbgl::style::CircleLayer>()) {
        return [[MGLCircleStyleLayer alloc] initWithRawLayer:circleLayer];
    } else if (auto backgroundLayer = rawLayer->as<mbgl::style::BackgroundLayer>()) {
        return [[MGLBackgroundStyleLayer alloc] initWithRawLayer:backgroundLayer];
    } else if (auto customLayer = rawLayer->as<mbgl::style::CustomLayer>()) {
        return [[MGLOpenGLStyleLayer alloc] initWithRawLayer:customLayer];
    } else {
        NSAssert(NO, @"Unrecognized layer type");
        return nil;
    }
}

- (MGLStyleLayer *)layerWithIdentifier:(NSString *)identifier
{
    auto mbglLayer = self.mapView.mbglMap->getLayer(identifier.UTF8String);
    return mbglLayer ? [self layerFromMBGLLayer:mbglLayer] : nil;
}

- (void)removeLayer:(MGLStyleLayer *)layer
{
    if (!layer.rawLayer) {
        [NSException raise:NSInvalidArgumentException format:
         @"The style layer %@ cannot be removed from the style. "
         @"Make sure the style layer was created as a member of a concrete subclass of MGLStyleLayer.",
         layer];
    }
    [self willChangeValueForKey:@"layers"];
    [layer removeFromMapView:self.mapView];
    [self didChangeValueForKey:@"layers"];
}

- (void)addLayer:(MGLStyleLayer *)layer
{
    if (!layer.rawLayer) {
        [NSException raise:NSInvalidArgumentException format:
         @"The style layer %@ cannot be added to the style. "
         @"Make sure the style layer was created as a member of a concrete subclass of MGLStyleLayer.",
         layer];
    }
    [self willChangeValueForKey:@"layers"];
    try {
        [layer addToMapView:self.mapView belowLayer:nil];
    } catch (std::runtime_error & err) {
        [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
    }
    [self didChangeValueForKey:@"layers"];
}

- (void)insertLayer:(MGLStyleLayer *)layer atIndex:(NSUInteger)index {
    [self insertObject:layer inLayersAtIndex:index];
}

- (void)insertLayer:(MGLStyleLayer *)layer belowLayer:(MGLStyleLayer *)sibling
{
    if (!layer.rawLayer) {
        [NSException raise:NSInvalidArgumentException
                    format:
         @"The style layer %@ cannot be added to the style. "
         @"Make sure the style layer was created as a member of a concrete subclass of MGLStyleLayer.",
         layer];
    }
    if (!sibling.rawLayer) {
        [NSException raise:NSInvalidArgumentException
                    format:
         @"A style layer cannot be placed below %@ in the style. "
         @"Make sure sibling was obtained using -[MGLStyle layerWithIdentifier:].",
         sibling];
    }
    [self willChangeValueForKey:@"layers"];
    try {
        [layer addToMapView:self.mapView belowLayer:sibling];
    } catch (std::runtime_error & err) {
        [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
    }
    [self didChangeValueForKey:@"layers"];
}

- (void)insertLayer:(MGLStyleLayer *)layer aboveLayer:(MGLStyleLayer *)sibling {
    if (!layer.rawLayer) {
        [NSException raise:NSInvalidArgumentException
                    format:
         @"The style layer %@ cannot be added to the style. "
         @"Make sure the style layer was created as a member of a concrete subclass of MGLStyleLayer.",
         layer];
    }
    if (!sibling.rawLayer) {
        [NSException raise:NSInvalidArgumentException
                    format:
         @"A style layer cannot be placed above %@ in the style. "
         @"Make sure sibling was obtained using -[MGLStyle layerWithIdentifier:].",
         sibling];
    }

    auto layers = self.mapView.mbglMap->getLayers();
    std::string siblingIdentifier = sibling.identifier.UTF8String;
    NSUInteger index = 0;
    for (auto layer : layers) {
        if (layer->getID() == siblingIdentifier) {
            break;
        }
        index++;
    }

    [self willChangeValueForKey:@"layers"];
    if (index + 1 > layers.size()) {
        [NSException raise:NSInvalidArgumentException
                    format:
         @"A style layer cannot be placed above %@ in the style. "
         @"Make sure sibling was obtained using -[MGLStyle layerWithIdentifier:].",
         sibling];
    } else if (index + 1 == layers.size()) {
        try {
            [layer addToMapView:self.mapView belowLayer:nil];
        } catch (std::runtime_error & err) {
            [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
        }
    } else {
        MGLStyleLayer *sibling = [self layerFromMBGLLayer:layers.at(index + 1)];
        try {
            [layer addToMapView:self.mapView belowLayer:sibling];
        } catch (std::runtime_error & err) {
            [NSException raise:@"MGLRedundantLayerIdentifierException" format:@"%s", err.what()];
        }
    }
    [self didChangeValueForKey:@"layers"];
}

#pragma mark Style classes

- (NS_ARRAY_OF(NSString *) *)styleClasses
{
    return @[];
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses
{
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration
{
}

- (NSUInteger)countOfStyleClasses {
    return 0;
}

- (BOOL)hasStyleClass:(NSString *)styleClass
{
    return NO;
}

- (void)addStyleClass:(NSString *)styleClass
{
}

- (void)removeStyleClass:(NSString *)styleClass
{
}

#pragma mark Style images

- (void)setImage:(MGLImage *)image forName:(NSString *)name
{
    if (!image) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Cannot assign nil image to “%@”.", name];
    }
    if (!name) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Cannot assign image %@ to a nil name.", image];
    }

    self.mapView.mbglMap->addImage([image mgl_styleImageWithIdentifier:name]);
}

- (void)removeImageForName:(NSString *)name
{
    if (!name) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Cannot remove image with nil name."];
    }

    self.mapView.mbglMap->removeImage([name UTF8String]);
}

- (MGLImage *)imageForName:(NSString *)name
{
    if (!name) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Cannot get image with nil name."];
    }

    auto styleImage = self.mapView.mbglMap->getImage([name UTF8String]);
    return styleImage ? [[MGLImage alloc] initWithMGLStyleImage:styleImage] : nil;
}

#pragma mark Style transitions

- (void)setTransition:(MGLTransition)transition
{
    auto transitionOptions = self.mapView.mbglMap->getTransitionOptions();
    transitionOptions.duration = MGLDurationFromTimeInterval(transition.duration);
    transitionOptions.delay = MGLDurationFromTimeInterval(transition.delay);
    
    self.mapView.mbglMap->setTransitionOptions(transitionOptions);
}

- (MGLTransition)transition
{
    MGLTransition transition;
    const mbgl::style::TransitionOptions transitionOptions = self.mapView.mbglMap->getTransitionOptions();

    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    
    return transition;
}

#pragma mark Style light

- (void)setLight:(MGLLight *)light
{
    std::unique_ptr<mbgl::style::Light> mbglLight = std::make_unique<mbgl::style::Light>([light mbglLight]);
    self.mapView.mbglMap->setLight(std::move(mbglLight));
}

- (MGLLight *)light
{
    auto mbglLight = self.mapView.mbglMap->getLight();
    MGLLight *light = [[MGLLight alloc] initWithMBGLLight:mbglLight];
    return light;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; name = %@, URL = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.name ? [NSString stringWithFormat:@"\"%@\"", self.name] : self.name,
            self.URL ? [NSString stringWithFormat:@"\"%@\"", self.URL] : self.URL];
}

@end
