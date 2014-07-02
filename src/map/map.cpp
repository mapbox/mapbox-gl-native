#include <llmr/map/map.hpp>
#include <llmr/map/source.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/transition.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/math.hpp>
#include <llmr/util/clip_ids.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/util/uv.hpp>
#include <llmr/style/style_layer_group.hpp>
#include <llmr/style/style_bucket.hpp>
#include <llmr/geometry/sprite_atlas.hpp>

#include <algorithm>
#include <memory>
#include <iostream>

using namespace llmr;

Map::Map(View& view)
    : view(view),
      transform(),
      style(std::make_shared<Style>()),
      glyphAtlas(std::make_shared<GlyphAtlas>(1024, 1024)),
      glyphStore(std::make_shared<GlyphStore>()),
      spriteAtlas(std::make_shared<SpriteAtlas>(512, 512)),
      texturepool(std::make_shared<Texturepool>()),
      painter(*this),
      loop(std::make_shared<uv::loop>()) {

    view.initialize(this);

    // Make sure that we're doing an initial drawing in all cases.
    is_clean.clear();
    is_rendered.clear();
    is_swapped.test_and_set();
}

Map::~Map() {
    // Clear the style first before the rest of the constructor deletes members of this object.
    // This is required because members of the style reference the Map object in their destructors.
    style.reset();

    if (async) {
        stop();
    }
}

void Map::start() {
    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    async = true;

    // Setup async notifications
    async_terminate = new uv_async_t();
    uv_async_init(**loop, async_terminate, terminate);
    async_terminate->data = **loop;

    async_render = new uv_async_t();
    uv_async_init(**loop, async_render, render);
    async_render->data = this;

    async_cleanup = new uv_async_t();
    uv_async_init(**loop, async_cleanup, cleanup);
    async_cleanup->data = this;

    uv_thread_create(&thread, [](void *arg) {
        Map *map = static_cast<Map *>(arg);
        map->run();
    }, this);
}

void Map::stop() {
    if (async_terminate != nullptr) {
        uv_async_send(async_terminate);
    }

    uv_thread_join(&thread);

    // Run the event loop once to make sure our async delete handlers are called.
    uv_run(**loop, UV_RUN_ONCE);

    async = false;
}

void Map::delete_async(uv_handle_t *handle) {
    delete (uv_async_t *)handle;
}

void Map::run() {
    setup();
    prepare();
    uv_run(**loop, UV_RUN_DEFAULT);

    // If the map rendering wasn't started asynchronously, we perform one render
    // *after* all events have been processed.
    if (!async) {
        render();
    }
}

void Map::rerender() {
    // We only send render events if we want to continuously update the map
    // (== async rendering).
    if (async && async_render != nullptr) {
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
    if (async_cleanup != nullptr) {
        uv_async_send(async_cleanup);
    }
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
    // Closes all open handles on the loop. This means that the loop will automatically terminate.
    uv_loop_t *loop = static_cast<uv_loop_t *>(async->data);
    uv_walk(loop, [](uv_handle_t *handle, void *arg) {
        if (!uv_is_closing(handle)) {
            uv_close(handle, NULL);
        }
    }, NULL);
}

#pragma mark - Setup

void Map::setup() {
    view.make_active();

    painter.setup();
}

void Map::setStyleJSON(std::string newStyleJSON) {
    styleJSON.swap(newStyleJSON);
    style->loadJSON((const uint8_t *)styleJSON.c_str());
    update();
}

std::string Map::getStyleJSON() const {
    return styleJSON;
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

double Map::getMinZoom() const {
    return transform.getMinZoom();
}

double Map::getMaxZoom() const {
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

void Map::toggleClass(const std::string &name) {
    style->toggleClass(name);
    if (style->hasTransitions()) {
        update();
    }
}

const std::vector<std::string> &Map::getAppliedClasses() const {
   return style->getAppliedClasses();
}

void Map::setDefaultTransitionDuration(uint64_t duration_milliseconds) {
    style->setDefaultTransitionDuration(duration_milliseconds);
}

void Map::updateTiles() {
    for (const std::shared_ptr<Source> &source : style->getActiveSources()) {
        source->update(*this);
    }
}

void Map::updateRenderState() {
    std::forward_list<Tile::ID> ids;

    for (const std::shared_ptr<Source> &source : style->getActiveSources()) {
        ids.splice_after(ids.before_begin(), source->getIDs());
        source->updateMatrices(painter.projMatrix, state);
    }

    const std::map<Tile::ID, ClipID> clipIDs = computeClipIDs(ids);

    for (const std::shared_ptr<Source> &source : style->getActiveSources()) {
        source->updateClipIDs(clipIDs);
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
        style->sprite = std::make_shared<Sprite>(*this, state.getPixelRatio());
        if (style->sprite_url.size()) {
            style->sprite->load(style->sprite_url);
        }
        spriteAtlas->resize(state.getPixelRatio());
    }

    if (pixelRatioChanged || dimensionsChanged) {
        painter.clearFramebuffers();
    }

    animationTime = util::now();
    style->updateProperties(state.getNormalizedZoom(), animationTime);

    // Allow the sprite atlas to potentially pull new sprite images if needed.
    if (style->sprite && style->sprite->isLoaded()) {
        spriteAtlas->update(*style->sprite);
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

    painter.drawClippingMasks(style->getActiveSources());

#if defined(DEBUG)
    // Generate debug information
    size_t source_id = 0;
    for (const std::shared_ptr<Source> &source : style->getActiveSources()) {
        size_t count = source->getTileCount();
        debug.emplace_back(util::sprintf<100>("source %d: %d\n", source_id, count));
        source_id++;
    }
#endif
    // Actually render the layers
    if (debug::renderTree) { std::cout << "{" << std::endl; indent++; }
    renderLayers(style->layers);
    if (debug::renderTree) { std::cout << "}" << std::endl; indent--; }

    // Finalize the rendering, e.g. by calling debug render calls per tile.
    // This guarantees that we have at least one function per tile called.
    // When only rendering layers via the stylesheet, it's possible that we don't
    // ever visit a tile during rendering.
    for (const std::shared_ptr<Source> &source : style->getActiveSources()) {
        source->finishRender(painter);
    }

    painter.renderMatte();

#if defined(DEBUG)
    painter.renderDebugText(debug);
#endif

    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        update();
    }

    glFlush();
}

void Map::renderLayers(std::shared_ptr<StyleLayerGroup> group) {
    if (!group) {
        // Make sure that we actually do have a layer group.
        return;
    }

    // TODO: Correctly compute the number of layers recursively beforehand.
    float strata_thickness = 1.0f / (group->layers.size() + 1);

    // - FIRST PASS ------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque
    // objects first.

    if (debug::renderTree) {
        std::cout << std::string(indent++ * 4, ' ') << "OPAQUE {" << std::endl;
    }
    int i = 0;
    for (auto it = group->layers.rbegin(), end = group->layers.rend(); it != end; ++it, ++i) {
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
    --i;
    for (auto it = group->layers.begin(), end = group->layers.end(); it != end; ++it, --i) {
        painter.setTranslucent();
        painter.setStrata(i * strata_thickness);
        renderLayer(*it, Translucent);
    }
    if (debug::renderTree) {
        std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
    }
}

void Map::renderLayer(std::shared_ptr<StyleLayer> layer_desc, RenderPass pass) {
    if (layer_desc->layers) {
        // This is a layer group. We render them during our translucent render pass.
        if (pass == Translucent) {
            const CompositeProperties &properties = layer_desc->getProperties<CompositeProperties>();
            if (properties.isVisible()) {
                gl::group group(std::string("group: ") + layer_desc->id);

                if (debug::renderTree) {
                    std::cout << std::string(indent++ * 4, ' ') << "+ " << layer_desc->id
                              << " (Composite) {" << std::endl;
                }

                painter.pushFramebuffer();

                renderLayers(layer_desc->layers);

                GLuint texture = painter.popFramebuffer();

                // Render the previous texture onto the screen.
                painter.drawComposite(texture, properties);

                if (debug::renderTree) {
                    std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
                }
            }
        }
    } else if (layer_desc->id == "background") {
        // This layer defines the background color.
    } else {
        // This is a singular layer.
        if (!layer_desc->bucket) {
            fprintf(stderr, "[WARNING] layer '%s' is missing bucket\n", layer_desc->id.c_str());
            return;
        }

        if (!layer_desc->bucket->source) {
            fprintf(stderr, "[WARNING] can't find source for layer '%s'\n", layer_desc->id.c_str());
            return;
        }

        // Abort early if we can already deduce from the bucket type that
        // we're not going to render anything anyway during this pass.
        switch (layer_desc->bucket->type) {
            case BucketType::Fill:
                if (!layer_desc->getProperties<FillProperties>().isVisible()) return;
                break;
            case BucketType::Line:
                if (pass == Opaque) return;
                if (!layer_desc->getProperties<LineProperties>().isVisible()) return;
                break;
            case BucketType::Icon:
                if (pass == Opaque) return;
                if (!layer_desc->getProperties<IconProperties>().isVisible()) return;
                break;
            case BucketType::Text:
                if (pass == Opaque) return;
                if (!layer_desc->getProperties<TextProperties>().isVisible()) return;
                break;
            case BucketType::Raster:
                if (pass == Translucent) return;
                if (!layer_desc->getProperties<RasterProperties>().isVisible()) return;
                break;
            default:
                break;
        }

        if (debug::renderTree) {
            std::cout << std::string(indent * 4, ' ') << "- " << layer_desc->id << " ("
                      << layer_desc->bucket->type << ")" << std::endl;
        }

        layer_desc->bucket->source->render(painter, layer_desc);
    }
}
