#include <llmr/map/map.hpp>
#include <llmr/map/source.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/transition.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/math.hpp>
#include <llmr/util/clip_ids.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/constants.hpp>

#include <algorithm>
#include <memory>
#include <iostream>

using namespace llmr;

Map::Map(View& view)
    : view(view),
      transform(),
      texturepool(),
      style(),
      glyphAtlas(1024, 1024),
      spriteAtlas(512, 512),
      painter(*this),
      loop(uv_loop_new()) {

    view.initialize(this);

    // Make sure that we're doing an initial drawing in all cases.
    is_clean.clear();
    is_rendered.clear();
    is_swapped.test_and_set();
}

Map::~Map() {
    uv_loop_delete(loop);
    loop = nullptr;
}

void Map::start() {
    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    async = true;

    // Setup async notifications
    async_terminate = new uv_async_t();
    uv_async_init(loop, async_terminate, terminate);
    async_terminate->data = loop;

    async_render = new uv_async_t();
    uv_async_init(loop, async_render, render);
    async_render->data = this;

    async_cleanup = new uv_async_t();
    uv_async_init(loop, async_cleanup, cleanup);
    async_cleanup->data = this;

    uv_thread_create(&thread, [](void *arg) {
        Map *map = static_cast<Map *>(arg);
        map->run();
    }, this);
}

void Map::stop() {
    uv_async_send(async_terminate);
    uv_thread_join(&thread);

    uv_close((uv_handle_t *)async_terminate, delete_async);
    async_terminate = nullptr;
    uv_close((uv_handle_t *)async_render, delete_async);
    async_render = nullptr;
    uv_close((uv_handle_t *)async_cleanup, delete_async);
    async_cleanup = nullptr;

    // Run the event loop once to make sure our async delete handlers are called.
    uv_run(loop, UV_RUN_ONCE);

    async = false;
}

void Map::delete_async(uv_handle_t *handle) {
    delete (uv_async_t *)handle;
}

void Map::run() {
    setup();
    prepare();
    uv_run(loop, UV_RUN_DEFAULT);

    // If the map rendering wasn't started asynchronously, we perform one render
    // *after* all events have been processed.
    if (!async) {
        render();
    }
}

void Map::rerender() {
    // We only send render events if we want to continuously update the map
    // (== async rendering).
    if (async) {
        uv_async_send(async_render);
    }
}

void Map::update() {
    is_clean.clear();
    rerender();
}

bool Map::needsSwap() {
    return is_swapped.test_and_set() == false;
}

void Map::swapped() {
    is_rendered.clear();
    rerender();
}

void Map::cleanup() {
    uv_async_send(async_cleanup);
}

void Map::cleanup(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);

    map->view.make_active();
    map->painter.cleanup();
}

void Map::render(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);


    if (map->state.hasSize()) {
        if (map->is_rendered.test_and_set() == false) {
            map->prepare();
            if (map->is_clean.test_and_set() == false) {
                map->render();
                map->is_swapped.clear();
                map->view.swap();
            } else {
                // We set the rendered flag in the test above, so we have to reset it
                // now that we're not actually rendering because the map is clean.
                map->is_rendered.clear();
            }
        }
    }
}

void Map::terminate(uv_async_t *async) {
    uv_stop(static_cast<uv_loop_t *>(async->data));
}

#pragma mark - Setup

void Map::setup() {
    view.make_active();

    painter.setup();

    sources.emplace("outdoors",
                    std::unique_ptr<Source>(new Source(*this,
                           painter,
                           "http://a.tiles.mapbox.com/v3/mapbox.mapbox-terrain-v1,mapbox.mapbox-streets-v5/%d/%d/%d.vector.pbf",
                           Source::Type::vector,
                           {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 }},
                           512,
                           0,
                           14,
                           true)));

    sources.emplace("satellite",
                    std::unique_ptr<Source>(new Source(*this,
                           painter,
                           "https://a.tiles.mapbox.com/v3/justin.hh0gkdfm/%d/%d/%d%s.png256",
                           Source::Type::raster,
                           {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 }},
                           256,
                           0,
                           21,
                           false)));

    loadStyle(resources::style, resources::style_size);
}

void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
    style.loadJSON(data, bytes);
    update();
}

#pragma mark - Size

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height) {
    if (transform.resize(width, height, ratio, fb_width, fb_height)) {
        update();
    }
}

#pragma mark - Transitions

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::cancelTransitions() {
    transform.cancelTransitions();

    update();
}


#pragma mark - Position

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startPanning() {
    transform.startPanning();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::stopPanning() {
    transform.stopPanning();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getScale() const {
    return transform.getScale();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getZoom() const {
    return transform.getZoom();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetZoom() {
    setZoom(0);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startScaling() {
    transform.startScaling();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::stopScaling() {
    transform.stopScaling();
    update();
}

double Map::getMinZoom() {
    return transform.getMinZoom();
}

double Map::getMaxZoom() {
    return transform.getMaxZoom();
}


#pragma mark - Rotation

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setAngle(double angle, double duration) {
    transform.setAngle(angle, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setAngle(double angle, double cx, double cy) {
    transform.setAngle(angle, cx, cy);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getAngle() const {
    return transform.getAngle();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetNorth() {
    transform.setAngle(0, 500_milliseconds);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startRotating() {
    transform.startRotating();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::stopRotating() {
    transform.stopRotating();
    update();
}

bool Map::canRotate() {
    return transform.canRotate();
}


#pragma mark - Toggles

void Map::setDebug(bool value) {
    debug = value;
    painter.setDebug(debug);
    update();
}

void Map::toggleDebug() {
    setDebug(!debug);
}

bool Map::getDebug() const {
    return debug;
}

void Map::toggleRaster() {
    style.setDefaultTransitionDuration(300);
    style.cancelTransitions();

    auto it = sources.find("satellite");
    if (it != sources.end()) {
        Source &satellite_source = *it->second;
        if (satellite_source.enabled) {
            satellite_source.enabled = false;
            style.appliedClasses.erase("satellite");
        } else {
            satellite_source.enabled = true;
            style.appliedClasses.insert("satellite");
        }
    }

    update();
}

void Map::updateTiles() {
    for (auto &pair : sources) {
        const std::unique_ptr<Source> &source = pair.second;
        if (source->enabled) {
            source->update();
        }
    }
}

void Map::updateRenderState() {
    std::forward_list<Tile::ID> ids;

    for (const auto &pair : sources) {
        Source &source = *pair.second;
        if (source.enabled) {
            ids.splice_after(ids.before_begin(), source.getIDs());
            source.updateMatrices(state);
        }
    }

    const std::map<Tile::ID, ClipID> clipIDs = computeClipIDs(ids);

    for (const auto &pair : sources) {
        Source &source = *pair.second;
        if (source.enabled) {
            source.updateClipIDs(clipIDs);
        }
    }
}

void Map::prepare() {
    view.make_active();

    // Update transform transitions.
    animationTime = util::now();
    if (transform.needsTransition()) {
        transform.updateTransitions(animationTime);
    }

    const TransformState oldState = state;
    state = transform.currentState();

    bool pixelRatioChanged = oldState.getPixelRatio() != state.getPixelRatio();
    bool dimensionsChanged = oldState.getFramebufferWidth() != state.getFramebufferWidth() ||
                             oldState.getFramebufferHeight() != state.getFramebufferHeight();

    if (pixelRatioChanged) {
        style.sprite = std::make_shared<Sprite>(*this, state.getPixelRatio());
        style.sprite->load(kSpriteURL);

        spriteAtlas.resize(state.getPixelRatio());
    }

    if (pixelRatioChanged || dimensionsChanged) {
        painter.clearFramebuffers();
    }

    style.cascade(state.getNormalizedZoom());

    // Update style transitions.
    animationTime = util::now();
    if (style.needsTransition()) {
        style.updateTransitions(animationTime);
        update();
    }

    // Allow the sprite atlas to potentially pull new sprite images if needed.
    if (style.sprite && style.sprite->isLoaded()) {
        spriteAtlas.update(*style.sprite);
    }

    updateTiles();
}

void Map::render() {
#if defined(DEBUG)
    std::vector<std::string> debug;
#endif
    painter.clear();

    painter.resetFramebuffer();

    painter.resize();

    painter.changeMatrix();

    updateRenderState();

    painter.drawClippingMasks(sources);

#if defined(DEBUG)
    // Generate debug information
    size_t source_id = 0;
    for (const auto &pair : sources) {
        const Source &source = *pair.second;
        size_t count = source.getTileCount();
        debug.emplace_back(util::sprintf<100>("source %d: %d\n", source_id, count));
        source_id++;
    }
#endif
    // Actually render the layers
    if (debug::renderTree) { std::cout << "{" << std::endl; indent++; }
    renderLayers(style.layers);
    if (debug::renderTree) { std::cout << "}" << std::endl; indent--; }

    // Finalize the rendering, e.g. by calling debug render calls per tile.
    // This guarantees that we have at least one function per tile called.
    // When only rendering layers via the stylesheet, it's possible that we don't
    // ever visit a tile during rendering.
    for (const auto &pair : sources) {
        Source &source = *pair.second;
        const std::string &name = pair.first;
        gl::group group(std::string("debug ") + name);
        source.finishRender();
    }

    painter.renderMatte();

#if defined(DEBUG)
    painter.renderDebugText(debug);
#endif

    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition()) {
        update();
    }

    glFlush();
}

void Map::renderLayers(const std::vector<LayerDescription>& layers) {
    // TODO: Correctly compute the number of layers recursively beforehand.
    float strata_thickness = 1.0f / (layers.size() + 1);

    // - FIRST PASS ------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque
    // objects first.

    if (debug::renderTree) {
        std::cout << std::string(indent++ * 4, ' ') << "OPAQUE {" << std::endl;
    }
    typedef std::vector<LayerDescription>::const_reverse_iterator riterator;
    int i = 0;
    for (riterator it = layers.rbegin(), end = layers.rend(); it != end; ++it, ++i) {
        painter.setOpaque();
        painter.setStrata(i * strata_thickness);
        renderLayer(*it, Opaque);
    }
    if (debug::renderTree) {
        std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
    }

    // - SECOND PASS -----------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render
    // bottom-to-top.
    if (debug::renderTree) {
        std::cout << std::string(indent++ * 4, ' ') << "TRANSLUCENT {" << std::endl;
    }
    typedef std::vector<LayerDescription>::const_iterator iterator;
    --i;
    for (iterator it = layers.begin(), end = layers.end(); it != end; ++it, --i) {
        painter.setTranslucent();
        painter.setStrata(i * strata_thickness);
        renderLayer(*it, Translucent);
    }
    if (debug::renderTree) {
        std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
    }
}

template <typename Styles>
typename Styles::const_iterator find_style(const Styles &styles, const LayerDescription &layer_desc) {
    auto it = styles.find(layer_desc.name);
#if defined(DEBUG)
    if (it == styles.end()) {
        if (debug::renderWarnings) {
            fprintf(stderr, "[WARNING] can't find class named '%s' in style\n",
                    layer_desc.name.c_str());
        }
    }
#endif
    return it;
}

template <typename Styles>
bool is_invisible(const Styles &styles, const LayerDescription &layer_desc) {
    auto it = find_style(styles, layer_desc);
    if (it == styles.end()) {
        // We don't have a style, so we fall back to the default style which is
        // always visible.
        return false;
    }
    return !it->second.isVisible();
}

void Map::renderLayer(const LayerDescription& layer_desc, RenderPass pass) {
    if (layer_desc.child_layer.size()) {
        // This is a layer group. We render them during our translucent render pass.
        if (pass == Translucent) {
            auto it = find_style(style.computed.composites, layer_desc);
            const CompositeProperties &properties = (it != style.computed.composites.end()) ? it->second : defaultCompositeProperties;
            if (properties.isVisible()) {
                gl::group group(std::string("group: ") + layer_desc.name);

                if (debug::renderTree) {
                    std::cout << std::string(indent++ * 4, ' ') << "+ " << layer_desc.name
                              << " (Composite) {" << std::endl;
                }

                painter.pushFramebuffer();

                renderLayers(layer_desc.child_layer);

                GLuint texture = painter.popFramebuffer();

                // Render the previous texture onto the screen.
                painter.drawComposite(texture, properties);

                if (debug::renderTree) {
                    std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
                }
            }
        }
    } else if (layer_desc.bucket_name == "background") {
        // This layer defines the background color.
    } else {
        // This is a singular layer. Try to find the bucket associated with
        // this layer and render it.

        auto bucket_it = style.buckets.find(layer_desc.bucket_name);
        if (bucket_it != style.buckets.end()) {
            const BucketDescription &bucket_desc = bucket_it->second;

            // Abort early if we can already deduce from the bucket type that
            // we're not going to render anything anyway during this pass.
            switch (bucket_desc.type) {
                case BucketType::Fill:
                    if (is_invisible(style.computed.fills, layer_desc)) return;
                    break;
                case BucketType::Line:
                    if (pass == Opaque) return;
                    if (is_invisible(style.computed.lines, layer_desc)) return;
                    break;
                case BucketType::Icon:
                    if (pass == Opaque) return;
                    if (is_invisible(style.computed.icons, layer_desc)) return;
                    break;
                case BucketType::Text:
                    if (pass == Opaque) return;
                    if (is_invisible(style.computed.texts, layer_desc)) return;
                    break;
                case BucketType::Raster:
                    if (pass == Translucent) return;
                    if (is_invisible(style.computed.rasters, layer_desc)) return;
                    break;
                default:
                    break;
            }

            // Find the source and render all tiles in it.
            auto source_it = sources.find(bucket_desc.source_name);
            if (source_it != sources.end()) {
                if (debug::renderTree) {
                    std::cout << std::string(indent * 4, ' ') << "- " << layer_desc.name << " ("
                              << bucket_desc.type << ")" << std::endl;
                }

                const std::unique_ptr<Source> &source = source_it->second;
                source->render(layer_desc, bucket_desc);
            } else {
                if (debug::renderWarnings) {
                    fprintf(stderr, "[WARNING] can't find source '%s' required for bucket '%s'\n",
                            bucket_desc.source_name.c_str(), layer_desc.bucket_name.c_str());
                }
            }
        } else {
            if (debug::renderWarnings) {
                fprintf(stderr, "[WARNING] can't find bucket '%s' required for layer '%s'\n",
                        layer_desc.bucket_name.c_str(), layer_desc.name.c_str());
            }
        }
    }
}
