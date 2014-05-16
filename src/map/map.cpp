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

#include <algorithm>
#include <memory>

using namespace llmr;

Map::Map(View& view)
    : view(view),
      transform(),
      texturepool(),
      style(),
      glyphAtlas(1024, 1024),
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

void Map::render(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);

    map->prepare();

    if (map->is_rendered.test_and_set() == false) {
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

void Map::terminate(uv_async_t *async) {
    uv_stop(static_cast<uv_loop_t *>(async->data));
}

#pragma mark - Setup

void Map::setup() {
    view.make_active();

    painter.setup();

    sources.emplace("mapbox streets",
                    std::unique_ptr<Source>(new Source(*this,
                           painter,
                           "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf",
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

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height) {
    if (transform.resize(width, height, ratio, fb_width, fb_height)) {
        update();
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    transform.cancelTransitions();

    update();
}


#pragma mark - Position

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration * 1_second);
    update();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration * 1_second);
    update();
}

void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

void Map::startPanning() {
    transform.startPanning();
    update();
}

void Map::stopPanning() {
    transform.stopPanning();
    update();
}

void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration * 1_second);
    update();
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration * 1_second);
    update();
}

double Map::getScale() const {
    return transform.getScale();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration * 1_second);
    update();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration * 1_second);
    update();
}

void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::startScaling() {
    transform.startScaling();
    update();
}

void Map::stopScaling() {
    transform.stopScaling();
    update();
}


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration * 1_second);
    update();
}

void Map::setAngle(double angle, double duration) {
    transform.setAngle(angle, duration * 1_second);
    update();
}

void Map::setAngle(double angle, double cx, double cy) {
    transform.setAngle(angle, cx, cy);
    update();
}

double Map::getAngle() const {
    return transform.getAngle();
}

void Map::resetNorth() {
    transform.setAngle(0, 500_milliseconds);
    update();
}

void Map::startRotating() {
    transform.startRotating();
    update();
}

void Map::stopRotating() {
    transform.stopRotating();
    update();
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
    auto it = sources.find("satellite");
    if (it != sources.end()) {
        Source &satellite_source = *it->second;

        if (satellite_source.enabled) {
            satellite_source.enabled = false;

            auto style_class = style.appliedClasses.find("satellite");
            if (style_class != style.appliedClasses.end()) {
                style.appliedClasses.erase(style_class);
            }
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



void Map::updateClippingIDs() {
    std::forward_list<Tile::ID> ids;

    for (auto &pair : sources) {
        const std::unique_ptr<Source> &source = pair.second;
        if (source->enabled) {
            ids.splice_after(ids.before_begin(), source->getIDs());
        }
    }

    const std::map<Tile::ID, ClipID> clipIDs = computeClipIDs(ids);

    for (auto &pair : sources) {
        const std::unique_ptr<Source> &source = pair.second;
        if (source->enabled) {
            source->updateClipIDs(clipIDs);
        }
    }
}

void Map::prepare() {
    view.make_active();

    // Update transitions
    animationTime = util::now();
    if (transform.needsTransition()) {
        transform.updateTransitions(animationTime);
    }

    state = transform.currentState();
    style.cascade(state.getNormalizedZoom());
    if (!style.sprite || style.sprite->pixelRatio != state.getPixelRatio()) {
        style.sprite = std::make_shared<Sprite>(*this, state.getPixelRatio());
        style.sprite->load(kSpriteURL);
    }

    updateTiles();

    updateClippingIDs();
}

void Map::render() {
    std::vector<std::string> debug;

    painter.clear();

    painter.resize();

    painter.changeMatrix();

    // First, update the sources' tile matrices with the new
    // transform and render into the stencil buffer, including
    // drawing the background.

    painter.prepareClippingMask();

    size_t source_id = 0;
    for (auto &pair : sources) {
        Source &source = *pair.second;
        size_t count = source.prepareRender(state);
        debug.emplace_back(util::sprintf<100>("source %d: %d\n", source_id, count));
        source_id++;
    }

    painter.finishClippingMask();



    // Render per layer in the stylesheet.
    strata_thickness = 1.0f / (style.layerCount() + 1);
    strata = 0;

    // - FIRST PASS ------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque
    // objects first.
    painter.startOpaquePass();
    renderLayers(style.layers, Opaque);
    painter.endPass();

    // - SECOND PASS -----------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render
    // bottom-to-top.
    --strata;
    painter.startTranslucentPass();
    renderLayers(style.layers, Translucent);
    painter.endPass();


    // Finalize the rendering, e.g. by calling debug render calls per tile.
    // This guarantees that we have at least one function per tile called.
    // When only rendering layers via the stylesheet, it's possible that we don't
    // ever visit a tile during rendering.
    for (auto &pair : sources) {
        Source &source = *pair.second;
        const std::string &name = pair.first;
        gl::group group(std::string("debug ") + name);
        source.finishRender();
    }

    painter.renderMatte();

    painter.renderDebugText(debug);

    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition()) {
        update();
    }

    glFlush();
}

void Map::renderLayers(const std::vector<LayerDescription>& layers, RenderPass pass) {
    if (pass == Opaque) {
        typedef std::vector<LayerDescription>::const_reverse_iterator riterator;
        for (riterator it = layers.rbegin(), end = layers.rend(); it != end; ++it, ++strata) {
            painter.setStrata(strata * strata_thickness);
            renderLayer(*it, pass);
        }
    } else if (pass == Translucent) {
        typedef std::vector<LayerDescription>::const_iterator iterator;
        for (iterator it = layers.begin(), end = layers.end(); it != end; ++it, --strata) {
            painter.setStrata(strata * strata_thickness);
            renderLayer(*it, pass);
        }
    } else {
        throw std::runtime_error("unknown render pass");
    }
}

template <typename Styles>
bool is_invisible(const Styles &styles, const LayerDescription &layer_desc) {
    auto it = styles.find(layer_desc.name);
    if (it == styles.end()) return true;
    const auto &properties = it->second;
    if (!properties.enabled) return true;
    if (properties.opacity <= 0) return true;
    return false;
}

void Map::renderLayer(const LayerDescription& layer_desc, RenderPass pass) {
    if (layer_desc.child_layer.size()) {
        gl::group group(std::string("group: ") + layer_desc.name);
        // This is a layer group.
        // TODO: create framebuffer
        renderLayers(layer_desc.child_layer, pass);
        // TODO: render framebuffer on previous framebuffer
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
                const std::unique_ptr<Source> &source = source_it->second;
                source->render(layer_desc, bucket_desc);
            }
        }
    }
}
