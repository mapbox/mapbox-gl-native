#import "MGLStyle.h"

#import "MGLMapView_Private.h"
#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import "MGLLineStyleLayer.h"
#import "MGLCircleStyleLayer.h"
#import "MGLSymbolStyleLayer.h"
#import "MGLRasterStyleLayer.h"
#import "MGLBackgroundStyleLayer.h"

#import "MGLStyle_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLSource_Private.h"

#import "NSDate+MGLAdditions.h"

#import "MGLSource.h"
#import "MGLVectorSource.h"
#import "MGLRasterSource.h"
#import "MGLGeoJSONSource.h"

#include <mbgl/util/default_styles.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>

#if TARGET_OS_IPHONE
    #import "UIImage+MGLAdditions.h"
#else
    #import "NSImage+MGLAdditions.h"
#endif

@interface MGLStyle()

@property (nonatomic, readwrite, weak) MGLMapView *mapView;
@property (readonly, copy, nullable) NSURL *URL;

@end

@implementation MGLStyle

#pragma mark Default style URLs

static_assert(mbgl::util::default_styles::currentVersion == MGLStyleDefaultVersion, "mbgl::util::default_styles::currentVersion and MGLStyleDefaultVersion disagree.");

/// @param name The style’s marketing name, written in lower camelCase.
/// @param fileName The last path component in the style’s URL, excluding the version suffix.
#define MGL_DEFINE_STYLE(name, fileName) \
    static NSURL *MGLStyleURL_##name; \
    + (NSURL *)name##StyleURL { \
        static dispatch_once_t onceToken; \
        dispatch_once(&onceToken, ^{ \
            MGLStyleURL_##name = [self name##StyleURLWithVersion:8]; \
        }); \
        return MGLStyleURL_##name; \
    } \
    \
    + (NSURL *)name##StyleURL##WithVersion:(NSInteger)version { \
        if (mbgl::util::default_styles::currentVersion == version) { \
            return [NSURL URLWithString:@(mbgl::util::default_styles::name.url)]; \
        } else { \
            return [NSURL URLWithString:[@"mapbox://styles/mapbox/" #fileName "-v" stringByAppendingFormat:@"%li", (long)version]]; \
        } \
    }

MGL_DEFINE_STYLE(streets, streets)
MGL_DEFINE_STYLE(outdoors, outdoors)
MGL_DEFINE_STYLE(light, light)
MGL_DEFINE_STYLE(dark, dark)
MGL_DEFINE_STYLE(satellite, satellite)
MGL_DEFINE_STYLE(satelliteStreets, satellite-streets)

// Make sure all the styles listed in mbgl::util::default_styles::orderedStyles
// are defined above and also declared in MGLStyle.h.
static_assert(6 == mbgl::util::default_styles::numOrderedStyles,
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

- (NS_MUTABLE_SET_OF(MGLSource *) *)sources {
    auto rawSources = self.mapView.mbglMap->getSources();
    NSMutableSet *sources = [NSMutableSet setWithCapacity:rawSources.size()];
    for (auto rawSource = rawSources.begin(); rawSource != rawSources.end(); ++rawSource) {
        MGLSource *source = [self sourceFromMBGLSource:*rawSource];
        [sources addObject:source];
    }
    return sources;
}

- (void)setSources:(NS_MUTABLE_SET_OF(MGLSource *) *)sources {
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

- (MGLSource *)sourceFromMBGLSource:(mbgl::style::Source *)mbglSource {
    NSString *identifier = @(mbglSource->getID().c_str());
    
    // TODO: Fill in options specific to the respective source classes
    // https://github.com/mapbox/mapbox-gl-native/issues/6584
    MGLSource *source;
    if (mbglSource->is<mbgl::style::VectorSource>()) {
        source = [[MGLVectorSource alloc] initWithIdentifier:identifier];
    } else if (mbglSource->is<mbgl::style::GeoJSONSource>()) {
        source = [[MGLGeoJSONSource alloc] initWithIdentifier:identifier];
    } else if (mbglSource->is<mbgl::style::RasterSource>()) {
        source = [[MGLRasterSource alloc] initWithIdentifier:identifier];
    } else {
        source = [[MGLSource alloc] initWithIdentifier:identifier];
    }
    
    source.rawSource = mbglSource;

    return source;
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
        [NSException raise:@"MGLRedundantSourceIdentiferException" format:@"%s", err.what()];
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

#pragma mark Style layers

- (NS_MUTABLE_ARRAY_OF(MGLStyleLayer *) *)layers
{
    auto layers = self.mapView.mbglMap->getLayers();
    NSMutableArray *styleLayers = [NSMutableArray arrayWithCapacity:layers.size()];
    for (auto layer = layers.rbegin(); layer != layers.rend(); ++layer) {
        MGLStyleLayer *styleLayer = [self layerFromMBGLLayer:*layer];
        [styleLayers addObject:styleLayer];
    }
    return styleLayers;
}

- (void)setLayers:(NS_MUTABLE_ARRAY_OF(MGLStyleLayer *) *)layers {
    for (MGLStyleLayer *layer in self.layers.reverseObjectEnumerator) {
        [self removeLayer:layer];
    }
    for (MGLStyleLayer *layer in layers.reverseObjectEnumerator) {
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
    if (index > layers.size() - 1) {
        [NSException raise:NSRangeException
                    format:@"No style layer at index %lu.", (unsigned long)index];
        return nil;
    }
    auto layer = layers.at(layers.size() - 1 - index);
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
        [styleLayer addToMapView:self.mapView];
    } else {
        MGLStyleLayer *sibling = [self layerFromMBGLLayer:layers.at(layers.size() - index)];
        [styleLayer addToMapView:self.mapView belowLayer:sibling];
    }
}

- (void)removeObjectFromLayersAtIndex:(NSUInteger)index
{
    auto layers = self.mapView.mbglMap->getLayers();
    if (index > layers.size() - 1) {
        [NSException raise:NSRangeException
                    format:@"Cannot remove style layer at out-of-bounds index %lu.", (unsigned long)index];
    }
    auto layer = layers.at(layers.size() - 1 - index);
    self.mapView.mbglMap->removeLayer(layer->getID());
}

- (MGLStyleLayer *)layerFromMBGLLayer:(mbgl::style::Layer *)mbglLayer
{
    NSParameterAssert(mbglLayer);
    
    NSString *identifier = @(mbglLayer->getID().c_str());
    MGLStyleLayer *styleLayer;
    if (auto fillLayer = mbglLayer->as<mbgl::style::FillLayer>()) {
        MGLSource *source = [self sourceWithIdentifier:@(fillLayer->getSourceID().c_str())];
        styleLayer = [[MGLFillStyleLayer alloc] initWithIdentifier:identifier source:source];
    } else if (auto lineLayer = mbglLayer->as<mbgl::style::LineLayer>()) {
        MGLSource *source = [self sourceWithIdentifier:@(lineLayer->getSourceID().c_str())];
        styleLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:identifier source:source];
    } else if (auto symbolLayer = mbglLayer->as<mbgl::style::SymbolLayer>()) {
        MGLSource *source = [self sourceWithIdentifier:@(symbolLayer->getSourceID().c_str())];
        styleLayer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:identifier source:source];
    } else if (auto rasterLayer = mbglLayer->as<mbgl::style::RasterLayer>()) {
        MGLSource *source = [self sourceWithIdentifier:@(rasterLayer->getSourceID().c_str())];
        styleLayer = [[MGLRasterStyleLayer alloc] initWithIdentifier:identifier source:source];
    } else if (auto circleLayer = mbglLayer->as<mbgl::style::CircleLayer>()) {
        MGLSource *source = [self sourceWithIdentifier:@(circleLayer->getSourceID().c_str())];
        styleLayer = [[MGLCircleStyleLayer alloc] initWithIdentifier:identifier source:source];
    } else if (mbglLayer->as<mbgl::style::BackgroundLayer>()) {
        styleLayer = [[MGLBackgroundStyleLayer alloc] initWithIdentifier:identifier];
    } else {
        NSAssert(NO, @"Unrecognized layer type");
        return nil;
    }

    styleLayer.rawLayer = mbglLayer;

    return styleLayer;
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
    [layer addToMapView:self.mapView];
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
    [layer addToMapView:self.mapView belowLayer:sibling];
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
        [layer addToMapView:self.mapView];
    } else {
        MGLStyleLayer *sibling = [self layerFromMBGLLayer:layers.at(index + 1)];
        [layer addToMapView:self.mapView belowLayer:sibling];
    }
    [self didChangeValueForKey:@"layers"];
}

#pragma mark Style classes

- (NS_ARRAY_OF(NSString *) *)styleClasses
{
    const std::vector<std::string> &appliedClasses = self.mapView.mbglMap->getClasses();

    NSMutableArray *returnArray = [NSMutableArray arrayWithCapacity:appliedClasses.size()];

    for (auto appliedClass : appliedClasses) {
       [returnArray addObject:@(appliedClass.c_str())];
    }

    return returnArray;
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses
{
    [self setStyleClasses:appliedClasses transitionDuration:0];
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration
{
    std::vector<std::string> newAppliedClasses;

    for (NSString *appliedClass in appliedClasses)
    {
        newAppliedClasses.push_back([appliedClass UTF8String]);
    }

    mbgl::style::TransitionOptions transition { { MGLDurationInSeconds(transitionDuration) } };
    self.mapView.mbglMap->setTransitionOptions(transition);
    self.mapView.mbglMap->setClasses(newAppliedClasses);
}

- (NSUInteger)countOfStyleClasses {
    const auto &classes = self.mapView.mbglMap->getClasses();
    return classes.size();
}

- (BOOL)hasStyleClass:(NSString *)styleClass
{
    return styleClass && self.mapView.mbglMap->hasClass([styleClass UTF8String]);
}

- (void)addStyleClass:(NSString *)styleClass
{
    if (styleClass)
    {
        self.mapView.mbglMap->addClass([styleClass UTF8String]);
    }
}

- (void)removeStyleClass:(NSString *)styleClass
{
    if (styleClass)
    {
        self.mapView.mbglMap->removeClass([styleClass UTF8String]);
    }
}

#pragma mark Style images

- (void)setImage:(MGLImage *)image forName:(NSString *)name
{
    NSAssert(image, @"image is null");
    NSAssert(name, @"name is null");

    self.mapView.mbglMap->addImage([name UTF8String], image.mgl_spriteImage);
}

- (void)removeImageForName:(NSString *)name
{
    NSAssert(name, @"name is null");

    self.mapView.mbglMap->removeImage([name UTF8String]);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; name = %@, URL = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.name ? [NSString stringWithFormat:@"\"%@\"", self.name] : self.name,
            self.URL ? [NSString stringWithFormat:@"\"%@\"", self.URL] : self.URL];
}

@end
