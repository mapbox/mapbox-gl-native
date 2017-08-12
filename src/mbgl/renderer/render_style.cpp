#include <mbgl/renderer/render_style.hpp>
#include <mbgl/renderer/render_style_observer.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

RenderStyleObserver nullObserver;

RenderStyle::RenderStyle(Scheduler& scheduler_, FileSource& fileSource_)
    : scheduler(scheduler_),
      fileSource(fileSource_),
      glyphManager(std::make_unique<GlyphManager>(fileSource)),
      imageManager(std::make_unique<ImageManager>()),
      lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 })),
      imageImpls(makeMutable<std::vector<Immutable<style::Image::Impl>>>()),
      sourceImpls(makeMutable<std::vector<Immutable<style::Source::Impl>>>()),
      layerImpls(makeMutable<std::vector<Immutable<style::Layer::Impl>>>()),
      renderLight(makeMutable<Light::Impl>()),
      observer(&nullObserver) {
    glyphManager->setObserver(this);
}

RenderStyle::~RenderStyle() {
    assert(BackendScope::exists()); // Required for custom layers.
}

void RenderStyle::setObserver(RenderStyleObserver* observer_) {
    observer = observer_;
}

std::vector<const RenderLayer*> RenderStyle::getRenderLayers() const {
    std::vector<const RenderLayer*> result;
    result.reserve(renderLayers.size());
    for (const auto& layer : *layerImpls) {
        result.push_back(getRenderLayer(layer->id));
    }
    return result;
}

RenderLayer* RenderStyle::getRenderLayer(const std::string& id) {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

const RenderLayer* RenderStyle::getRenderLayer(const std::string& id) const {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

const RenderLight& RenderStyle::getRenderLight() const {
    return renderLight;
}

void RenderStyle::update(const UpdateParameters& parameters) {
    assert(BackendScope::exists()); // Required for custom layers.

    const bool zoomChanged = zoomHistory.update(parameters.transformState.getZoom(), parameters.timePoint);

    const TransitionParameters transitionParameters {
        parameters.timePoint,
        parameters.mode == MapMode::Continuous ? parameters.transitionOptions : TransitionOptions()
    };

    const PropertyEvaluationParameters evaluationParameters {
        zoomHistory,
        parameters.timePoint,
        parameters.mode == MapMode::Continuous ? util::DEFAULT_TRANSITION_DURATION : Duration::zero()
    };

    const TileParameters tileParameters {
        parameters.pixelRatio,
        parameters.debugOptions,
        parameters.transformState,
        parameters.scheduler,
        parameters.fileSource,
        parameters.mode,
        parameters.annotationManager,
        *imageManager,
        *glyphManager,
        parameters.prefetchZoomDelta
    };

    glyphManager->setURL(parameters.glyphURL);

    // Update light.
    const bool lightChanged = renderLight.impl != parameters.light;

    if (lightChanged) {
        renderLight.impl = parameters.light;
        renderLight.transition(transitionParameters);
    }

    if (lightChanged || zoomChanged || renderLight.hasTransition()) {
        renderLight.evaluate(evaluationParameters);
    }


    const ImageDifference imageDiff = diffImages(imageImpls, parameters.images);
    imageImpls = parameters.images;

    // Remove removed images from sprite atlas.
    for (const auto& entry : imageDiff.removed) {
        imageManager->removeImage(entry.first);
    }

    // Add added images to sprite atlas.
    for (const auto& entry : imageDiff.added) {
        imageManager->addImage(entry.second);
    }

    // Update changed images.
    for (const auto& entry : imageDiff.changed) {
        imageManager->updateImage(entry.second.after);
    }

    imageManager->setLoaded(parameters.spriteLoaded);


    const LayerDifference layerDiff = diffLayers(layerImpls, parameters.layers);
    layerImpls = parameters.layers;

    // Remove render layers for removed layers.
    for (const auto& entry : layerDiff.removed) {
        renderLayers.erase(entry.first);
    }

    // Create render layers for newly added layers.
    for (const auto& entry : layerDiff.added) {
        renderLayers.emplace(entry.first, RenderLayer::create(entry.second));
    }

    // Update render layers for changed layers.
    for (const auto& entry : layerDiff.changed) {
        renderLayers.at(entry.first)->setImpl(entry.second.after);
    }

    // Update layers for class and zoom changes.
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAdded = layerDiff.added.count(entry.first);
        const bool layerChanged = layerDiff.changed.count(entry.first);

        if (layerAdded || layerChanged) {
            layer.transition(transitionParameters);
        }

        if (layerAdded || layerChanged || zoomChanged || layer.hasTransition()) {
            layer.evaluate(evaluationParameters);
        }
    }


    const SourceDifference sourceDiff = diffSources(sourceImpls, parameters.sources);
    sourceImpls = parameters.sources;

    // Remove render layers for removed sources.
    for (const auto& entry : sourceDiff.removed) {
        renderSources.erase(entry.first);
    }

    // Create render sources for newly added sources.
    for (const auto& entry : sourceDiff.added) {
        std::unique_ptr<RenderSource> renderSource = RenderSource::create(entry.second);
        renderSource->setObserver(this);
        renderSources.emplace(entry.first, std::move(renderSource));
    }

    // Update all sources.
    for (const auto& source : *sourceImpls) {
        std::vector<Immutable<Layer::Impl>> filteredLayers;
        bool needsRendering = false;
        bool needsRelayout = false;

        for (const auto& layer : *layerImpls) {
            if (layer->type == LayerType::Background ||
                layer->type == LayerType::Custom ||
                layer->source != source->id) {
                continue;
            }

            if (!needsRendering && getRenderLayer(layer->id)->needsRendering(zoomHistory.lastZoom)) {
                needsRendering = true;
            }

            if (!needsRelayout && (
                hasLayoutDifference(layerDiff, layer->id) ||
                !imageDiff.added.empty() ||
                !imageDiff.removed.empty() ||
                !imageDiff.changed.empty())) {
                needsRelayout = true;
            }

            filteredLayers.push_back(layer);
        }

        renderSources.at(source->id)->update(source,
                                             filteredLayers,
                                             needsRendering,
                                             needsRelayout,
                                             tileParameters);
    }
}

RenderSource* RenderStyle::getRenderSource(const std::string& id) const {
    auto it = renderSources.find(id);
    return it != renderSources.end() ? it->second.get() : nullptr;
}

bool RenderStyle::hasTransitions() const {
    if (renderLight.hasTransition()) {
        return true;
    }

    for (const auto& entry : renderLayers) {
        if (entry.second->hasTransition()) {
            return true;
        }
    }

    return false;
}

bool RenderStyle::isLoaded() const {
    for (const auto& entry: renderSources) {
        if (!entry.second->isLoaded()) {
            return false;
        }
    }

    if (!imageManager->isLoaded()) {
        return false;
    }

    return true;
}

RenderData RenderStyle::getRenderData(MapDebugOptions debugOptions, float angle) {
    RenderData result;

    for (const auto& entry : renderSources) {
        if (entry.second->isEnabled()) {
            result.sources.insert(entry.second.get());
        }
    }

    for (auto& layerImpl : *layerImpls) {
        RenderLayer* layer = getRenderLayer(layerImpl->id);
        assert(layer);

        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (const RenderBackgroundLayer* background = layer->as<RenderBackgroundLayer>()) {
            if (debugOptions & MapDebugOptions::Overdraw) {
                // We want to skip glClear optimization in overdraw mode.
                result.order.emplace_back(*layer, nullptr);
                continue;
            }
            const BackgroundPaintProperties::PossiblyEvaluated& paint = background->evaluated;
            if (layerImpl.get() == layerImpls->at(0).get() && paint.get<BackgroundPattern>().from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = paint.get<BackgroundColor>() * paint.get<BackgroundOpacity>();
            } else {
                // This is a textured background, or not the bottommost layer. We need to render it with a quad.
                result.order.emplace_back(*layer, nullptr);
            }
            continue;
        }

        if (layer->is<RenderCustomLayer>()) {
            result.order.emplace_back(*layer, nullptr);
            continue;
        }

        RenderSource* source = getRenderSource(layer->baseImpl->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->getID().c_str());
            continue;
        }

        const bool symbolLayer = layer->is<RenderSymbolLayer>();

        auto sortedTiles = source->getRenderTiles();
        if (symbolLayer) {
            // Sort symbol tiles in opposite y position, so tiles with overlapping symbols are drawn
            // on top of each other, with lower symbols being drawn on top of higher symbols.
            std::sort(sortedTiles.begin(), sortedTiles.end(),
                      [angle](const RenderTile& a, const RenderTile& b) {
                Point<float> pa(a.id.canonical.x, a.id.canonical.y);
                Point<float> pb(b.id.canonical.x, b.id.canonical.y);

                auto par = util::rotate(pa, angle);
                auto pbr = util::rotate(pb, angle);

                return std::tie(par.y, par.x) < std::tie(pbr.y, pbr.x);
            });
        } else {
            std::sort(sortedTiles.begin(), sortedTiles.end(),
                      [](const auto& a, const auto& b) { return a.get().id < b.get().id; });
        }

        std::vector<std::reference_wrapper<RenderTile>> sortedTilesForInsertion;
        for (auto& sortedTile : sortedTiles) {
            auto& tile = sortedTile.get();
            if (!tile.tile.isRenderable()) {
                continue;
            }

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (symbolLayer) {
                bool skip = false;
                // Look back through the buckets we decided to render to find out whether there is
                // already a bucket from this layer that is a parent of this tile. Tiles are ordered
                // by zoom level when we obtain them from getTiles().
                for (auto it = sortedTilesForInsertion.rbegin();
                     it != sortedTilesForInsertion.rend(); ++it) {
                    if (tile.tile.id.isChildOf(it->get().tile.id)) {
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            auto bucket = tile.tile.getBucket(*layer->baseImpl);
            if (bucket) {
                sortedTilesForInsertion.emplace_back(tile);
                tile.used = true;
            }
        }
        layer->setRenderTiles(std::move(sortedTilesForInsertion));
        result.order.emplace_back(*layer, source);
    }

    return result;
}

std::vector<Feature> RenderStyle::queryRenderedFeatures(const ScreenLineString& geometry,
                                                  const TransformState& transformState,
                                                  const RenderedQueryOptions& options) const {
    std::vector<const RenderLayer*> layers;
    if (options.layerIDs) {
        for (const auto& layerID : *options.layerIDs) {
            if (const RenderLayer* layer = getRenderLayer(layerID)) {
                layers.emplace_back(layer);
            }
        }
    } else {
        for (const auto& entry : renderLayers) {
            layers.emplace_back(entry.second.get());
        }
    }

    std::unordered_set<std::string> sourceIDs;
    for (const RenderLayer* layer : layers) {
        sourceIDs.emplace(layer->baseImpl->source);
    }

    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;
    for (const auto& sourceID : sourceIDs) {
        if (RenderSource* renderSource = getRenderSource(sourceID)) {
            auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, layers, options);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }

    std::vector<Feature> result;

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer order.
    for (const auto& layerImpl : *layerImpls) {
        const RenderLayer* layer = getRenderLayer(layerImpl->id);
        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }
        auto it = resultsByLayer.find(layer->baseImpl->id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

void RenderStyle::onLowMemory() {
    for (const auto& entry : renderSources) {
        entry.second->onLowMemory();
    }
}

void RenderStyle::onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load glyph range %d-%d for font stack %s: %s",
               glyphRange.first, glyphRange.second, fontStackToString(fontStack).c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void RenderStyle::onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.baseImpl->id.c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void RenderStyle::onTileChanged(RenderSource&, const OverscaledTileID&) {
    observer->onInvalidate();
}

void RenderStyle::dumpDebugLogs() const {
    for (const auto& entry : renderSources) {
        entry.second->dumpDebugLogs();
    }

    imageManager->dumpDebugLogs();
}

} // namespace mbgl
