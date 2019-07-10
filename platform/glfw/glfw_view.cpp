#include "glfw_view.hpp"
#include "glfw_backend.hpp"
#include "glfw_renderer_frontend.hpp"
#include "ny_route.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/map/camera.hpp>

#include <mapbox/cheap_ruler.hpp>
#include <mapbox/geometry.hpp>
#include <mapbox/geojson.hpp>

#if MBGL_USE_GLES2
#define GLFW_INCLUDE_ES2
#endif // MBGL_USE_GLES2

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdlib>

void glfwError(int error, const char *description) {
    mbgl::Log::Error(mbgl::Event::OpenGL, "GLFW error (%i): %s", error, description);
    assert(false);
}

GLFWView::GLFWView(bool fullscreen_, bool benchmark_)
    : fullscreen(fullscreen_), benchmark(benchmark_) {
    glfwSetErrorCallback(glfwError);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!glfwInit()) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize glfw");
        exit(1);
    }

    GLFWmonitor *monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);
        width = videoMode->width;
        height = videoMode->height;
    }

#if __APPLE__
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);
#endif

#if MBGL_WITH_EGL
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#if MBGL_USE_GLES2
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    if (mbgl::gfx::Backend::GetType() != mbgl::gfx::Backend::Type::OpenGL) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);

    window = glfwCreateWindow(width, height, "Mapbox GL", monitor, nullptr);
    if (!window) {
        glfwTerminate();
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize window");
        exit(1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetMouseButtonCallback(window, onMouseClick);
    glfwSetWindowSizeCallback(window, onWindowResize);
    glfwSetFramebufferSizeCallback(window, onFramebufferResize);
    glfwSetScrollCallback(window, onScroll);
    glfwSetKeyCallback(window, onKey);
    glfwSetWindowFocusCallback(window, onWindowFocus);

    glfwGetWindowSize(window, &width, &height);

    backend = GLFWBackend::Create(window, benchmark);

    pixelRatio = static_cast<float>(backend->getSize().width) / width;

    glfwMakeContextCurrent(nullptr);

    printf("\n");
    printf("================================================================================\n");
    printf("\n");
    printf("- Press `S` to cycle through bundled styles\n");
    printf("- Press `X` to reset the transform\n");
    printf("- Press `N` to reset north\n");
    printf("- Press `R` to enable the route demo\n");
    printf("- Press `E` to insert an example building extrusion layer\n");
    printf("- Press `O` to toggle online connectivity\n");
    printf("- Press `Z` to cycle through north orientations\n");
    printf("- Prezz `X` to cycle through the viewport modes\n");
    printf("- Press `I` to Delete existing database and re-initialize\n");
    printf("- Press `A` to cycle through Mapbox offices in the world + dateline monument\n");
    printf("- Press `B` to cycle through the color, stencil, and depth buffer\n");
    printf("- Press `D` to cycle through camera bounds: inside, crossing IDL at left, crossing IDL at right, and disabled\n");
    printf("- Press `T` to add custom geometry source\n");
    printf("\n");
    printf("- Press `1` through `6` to add increasing numbers of point annotations for testing\n");
    printf("- Press `7` through `0` to add increasing numbers of shape annotations for testing\n");
    printf("\n");
    printf("- Press `Q` to remove annotations\n");
    printf("- Press `K` to add a random custom runtime imagery annotation\n");
    printf("- Press `L` to add a random line annotation\n");
    printf("- Press `W` to pop the last-added annotation off\n");
    printf("\n");
    printf("- Press `P` to pause tile requests\n");
    printf("- `Control` + mouse drag to rotate\n");
    printf("- `Shift` + mouse drag to tilt\n");
    printf("\n");
    printf("- Press `Tab` to cycle through the map debug options\n");
    printf("- Press `Esc` to quit\n");
    printf("\n");
    printf("================================================================================\n");
    printf("\n");
}

GLFWView::~GLFWView() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWView::setMap(mbgl::Map *map_) {
    map = map_;
    map->addAnnotationImage(makeImage("default_marker", 22, 22, 1));
}

void GLFWView::setRenderFrontend(GLFWRendererFrontend* rendererFrontend_) {
    rendererFrontend = rendererFrontend_;
}

mbgl::gfx::RendererBackend& GLFWView::getRendererBackend() {
    return backend->getRendererBackend();
}

void GLFWView::onKey(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (action == GLFW_RELEASE) {
        if (key != GLFW_KEY_R || key != GLFW_KEY_S)
            view->animateRouteCallback = nullptr;

        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_TAB:
            view->map->cycleDebugOptions();
            break;
        case GLFW_KEY_X:
            if (!mods)
                view->map->jumpTo(mbgl::CameraOptions().withCenter(mbgl::LatLng {}).withZoom(0.0).withBearing(0.0).withPitch(0.0));
            break;
        case GLFW_KEY_O:
            view->onlineStatusCallback();
            break;
        case GLFW_KEY_S:
            if (view->changeStyleCallback)
                view->changeStyleCallback();
            break;
#if not MBGL_USE_GLES2
        case GLFW_KEY_B: {
            auto debug = view->map->getDebug();
            if (debug & mbgl::MapDebugOptions::StencilClip) {
                debug &= ~mbgl::MapDebugOptions::StencilClip;
                debug |= mbgl::MapDebugOptions::DepthBuffer;
            } else if (debug & mbgl::MapDebugOptions::DepthBuffer) {
                debug &= ~mbgl::MapDebugOptions::DepthBuffer;
            } else {
                debug |= mbgl::MapDebugOptions::StencilClip;
            }
            view->map->setDebug(debug);
        } break;
#endif // MBGL_USE_GLES2
        case GLFW_KEY_N:
            if (!mods)
                view->map->easeTo(mbgl::CameraOptions().withBearing(0.0), mbgl::AnimationOptions {{mbgl::Milliseconds(500)}});
            break;
        case GLFW_KEY_Z:
            view->nextOrientation();
            break;
        case GLFW_KEY_Q: {
            auto result = view->rendererFrontend->getRenderer()->queryPointAnnotations({ {}, { double(view->getSize().width), double(view->getSize().height) } });
            printf("visible point annotations: %lu\n", result.size());
        } break;
        case GLFW_KEY_P:
            view->pauseResumeCallback();
            break;
        case GLFW_KEY_C:
            view->clearAnnotations();
            break;
        case GLFW_KEY_I:
            view->resetDatabaseCallback();
            break;
        case GLFW_KEY_K:
            view->addRandomCustomPointAnnotations(1);
            break;
        case GLFW_KEY_L:
            view->addRandomLineAnnotations(1);
            break;
        case GLFW_KEY_A: {
            // XXX Fix precision loss in flyTo:
            // https://github.com/mapbox/mapbox-gl-native/issues/4298
            static const std::vector<mbgl::LatLng> places = {
                mbgl::LatLng { -16.796665, -179.999983 },   // Dateline monument
                mbgl::LatLng { 12.9810542, 77.6345551 },    // Mapbox Bengaluru, India
                mbgl::LatLng { -13.15607,-74.21773 },       // Mapbox Peru
                mbgl::LatLng { 37.77572, -122.4158818 },    // Mapbox SF, USA
                mbgl::LatLng { 38.91318,-77.03255 },        // Mapbox DC, USA
            };
            static size_t nextPlace = 0;
            mbgl::CameraOptions cameraOptions;
            cameraOptions.center = places[nextPlace++];
            cameraOptions.zoom = 20;
            cameraOptions.pitch = 30;

            mbgl::AnimationOptions animationOptions(mbgl::Seconds(10));
            view->map->flyTo(cameraOptions, animationOptions);
            nextPlace = nextPlace % places.size();
        } break;
        case GLFW_KEY_R: {
            view->show3DExtrusions = true;
            view->toggle3DExtrusions(view->show3DExtrusions);
            if (view->animateRouteCallback) break;
            view->animateRouteCallback = [](mbgl::Map* routeMap) {
                static mapbox::cheap_ruler::CheapRuler ruler { 40.7 }; // New York
                static mapbox::geojson::geojson route { mapbox::geojson::parse(mbgl::platform::glfw::route) };
                const auto& geometry = route.get<mapbox::geometry::geometry<double>>();
                const auto& lineString = geometry.get<mapbox::geometry::line_string<double>>();

                static double routeDistance = ruler.lineDistance(lineString);
                static double routeProgress = 0;
                routeProgress += 0.0005;
                if (routeProgress > 1.0) {
                    routeProgress = 0.0;
                }

                auto camera = routeMap->getCameraOptions();

                auto point = ruler.along(lineString, routeProgress * routeDistance);
                const mbgl::LatLng center { point.y, point.x };
                auto latLng = *camera.center;
                double bearing = ruler.bearing({ latLng.longitude(), latLng.latitude() }, point);
                double easing = bearing - *camera.bearing;
                easing += easing > 180.0 ? -360.0 : easing < -180 ? 360.0 : 0;
                bearing = *camera.bearing + (easing / 20);
                routeMap->jumpTo(mbgl::CameraOptions().withCenter(center).withZoom(18.0).withBearing(bearing).withPitch(60.0));
            };
            view->animateRouteCallback(view->map);
        } break;
        case GLFW_KEY_E:
            view->toggle3DExtrusions(!view->show3DExtrusions);
            break;
        case GLFW_KEY_D: {
            static const std::vector<mbgl::LatLngBounds> bounds = {
                mbgl::LatLngBounds::hull(mbgl::LatLng { -45.0, -170.0 }, mbgl::LatLng { 45.0, 170.0 }),  // inside
                mbgl::LatLngBounds::hull(mbgl::LatLng { -45.0, -200.0 }, mbgl::LatLng { 45.0, -160.0 }), // left IDL
                mbgl::LatLngBounds::hull(mbgl::LatLng { -45.0, 160.0 }, mbgl::LatLng { 45.0, 200.0 }),   // right IDL
                mbgl::LatLngBounds::unbounded()
            };
            static size_t nextBound = 0u;
            static mbgl::AnnotationID boundAnnotationID = std::numeric_limits<mbgl::AnnotationID>::max();

            mbgl::LatLngBounds bound = bounds[nextBound++];
            nextBound = nextBound % bounds.size();

            view->map->setBounds(mbgl::BoundOptions().withLatLngBounds(bound));

            if (bound == mbgl::LatLngBounds::unbounded()) {
                view->map->removeAnnotation(boundAnnotationID);
                boundAnnotationID = std::numeric_limits<mbgl::AnnotationID>::max();
            } else {
                mbgl::Polygon<double> rect;
                rect.push_back({
                    mbgl::Point<double>{ bound.west(), bound.north() },
                    mbgl::Point<double>{ bound.east(), bound.north() },
                    mbgl::Point<double>{ bound.east(), bound.south() },
                    mbgl::Point<double>{ bound.west(), bound.south() },
                });

                auto boundAnnotation = mbgl::FillAnnotation { rect, 0.5f, { view->makeRandomColor() }, { view->makeRandomColor() } };

                if (boundAnnotationID == std::numeric_limits<mbgl::AnnotationID>::max()) {
                    boundAnnotationID = view->map->addAnnotation(boundAnnotation);
                } else {
                    view->map->updateAnnotation(boundAnnotationID, boundAnnotation);
                }
            }
        } break;
        case GLFW_KEY_T:
            view->toggleCustomSource();
            break;
        }
    }

    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W: view->popAnnotation(); break;
        case GLFW_KEY_1: view->addRandomPointAnnotations(1); break;
        case GLFW_KEY_2: view->addRandomPointAnnotations(10); break;
        case GLFW_KEY_3: view->addRandomPointAnnotations(100); break;
        case GLFW_KEY_4: view->addRandomPointAnnotations(1000); break;
        case GLFW_KEY_5: view->addRandomPointAnnotations(10000); break;
        case GLFW_KEY_6: view->addRandomPointAnnotations(100000); break;
        case GLFW_KEY_7: view->addRandomShapeAnnotations(1); break;
        case GLFW_KEY_8: view->addRandomShapeAnnotations(10); break;
        case GLFW_KEY_9: view->addRandomShapeAnnotations(100); break;
        case GLFW_KEY_0: view->addRandomShapeAnnotations(1000); break;
        case GLFW_KEY_M: view->addAnimatedAnnotation(); break;
        }
    }
}

mbgl::Color GLFWView::makeRandomColor() const {
    const float r = 1.0f * (float(std::rand()) / RAND_MAX);
    const float g = 1.0f * (float(std::rand()) / RAND_MAX);
    const float b = 1.0f * (float(std::rand()) / RAND_MAX);
    return { r, g, b, 1.0f };
}

mbgl::Point<double> GLFWView::makeRandomPoint() const {
    const double x = width * double(std::rand()) / RAND_MAX;
    const double y = height * double(std::rand()) / RAND_MAX;
    mbgl::LatLng latLng = map->latLngForPixel({ x, y });
    return { latLng.longitude(), latLng.latitude() };
}

std::unique_ptr<mbgl::style::Image>
GLFWView::makeImage(const std::string& id, int width, int height, float pixelRatio) {
    const int r = 255 * (double(std::rand()) / RAND_MAX);
    const int g = 255 * (double(std::rand()) / RAND_MAX);
    const int b = 255 * (double(std::rand()) / RAND_MAX);

    const int w = std::ceil(pixelRatio * width);
    const int h = std::ceil(pixelRatio * height);

    mbgl::PremultipliedImage image({ static_cast<uint32_t>(w), static_cast<uint32_t>(h) });
    auto data = reinterpret_cast<uint32_t*>(image.data.get());
    const int dist = (w / 2) * (w / 2);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const int dx = x - w / 2;
            const int dy = y - h / 2;
            const int diff = dist - (dx * dx + dy * dy);
            if (diff > 0) {
                const int a = std::min(0xFF, diff) * 0xFF / dist;
                // Premultiply the rgb values with alpha
                data[w * y + x] =
                    (a << 24) | ((a * r / 0xFF) << 16) | ((a * g / 0xFF) << 8) | (a * b / 0xFF);
            }
        }
    }

    return std::make_unique<mbgl::style::Image>(id, std::move(image), pixelRatio);
}

void GLFWView::nextOrientation() {
    using NO = mbgl::NorthOrientation;
    switch (map->getMapOptions().northOrientation()) {
        case NO::Upwards: map->setNorthOrientation(NO::Rightwards); break;
        case NO::Rightwards: map->setNorthOrientation(NO::Downwards); break;
        case NO::Downwards: map->setNorthOrientation(NO::Leftwards); break;
        default: map->setNorthOrientation(NO::Upwards); break;
    }
}

void GLFWView::addRandomCustomPointAnnotations(int count) {
    for (int i = 0; i < count; i++) {
        static int spriteID = 1;
        const auto name = std::string{ "marker-" } + mbgl::util::toString(spriteID++);
        map->addAnnotationImage(makeImage(name, 22, 22, 1));
        spriteIDs.push_back(name);
        annotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { makeRandomPoint(), name }));
    }
}

void GLFWView::addRandomPointAnnotations(int count) {
    for (int i = 0; i < count; ++i) {
        annotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { makeRandomPoint(), "default_marker" }));
    }
}

void GLFWView::addRandomLineAnnotations(int count) {
    for (int i = 0; i < count; ++i) {
        mbgl::LineString<double> lineString;
        for (int j = 0; j < 3; ++j) {
            lineString.push_back(makeRandomPoint());
        }
        annotationIDs.push_back(map->addAnnotation(mbgl::LineAnnotation { lineString, 1.0f, 2.0f, { makeRandomColor() } }));
    }
}

void GLFWView::addRandomShapeAnnotations(int count) {
    for (int i = 0; i < count; ++i) {
        mbgl::Polygon<double> triangle;
        triangle.push_back({ makeRandomPoint(), makeRandomPoint(), makeRandomPoint() });
        annotationIDs.push_back(map->addAnnotation(mbgl::FillAnnotation { triangle, 0.5f, { makeRandomColor() }, { makeRandomColor() } }));
    }
}

void GLFWView::addAnimatedAnnotation() {
    const double started = glfwGetTime();
    animatedAnnotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { { 0, 0 } , "default_marker" }));
    animatedAnnotationAddedTimes.push_back(started);
}

void GLFWView::updateAnimatedAnnotations() {
    const double time = glfwGetTime();
    for (size_t i = 0; i < animatedAnnotationIDs.size(); i++) {
        auto dt = time - animatedAnnotationAddedTimes[i];

        const double period = 10;
        const double x = dt / period * 360 - 180;
        const double y = std::sin(dt/ period * M_PI * 2.0) * 80;
        map->updateAnnotation(animatedAnnotationIDs[i], mbgl::SymbolAnnotation { {x, y }, "default_marker" });
    }
}

void GLFWView::clearAnnotations() {
    for (const auto& id : annotationIDs) {
        map->removeAnnotation(id);
    }

    annotationIDs.clear();

    for (const auto& id : animatedAnnotationIDs) {
        map->removeAnnotation(id);
    }

    animatedAnnotationIDs.clear();
}

void GLFWView::popAnnotation() {
    if (annotationIDs.empty()) {
        return;
    }

    map->removeAnnotation(annotationIDs.back());
    annotationIDs.pop_back();
}

void GLFWView::onScroll(GLFWwindow *window, double /*xOffset*/, double yOffset) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    double delta = yOffset * 40;

    bool isWheel = delta != 0 && std::fmod(delta, 4.000244140625) == 0;

    double absDelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + std::exp(-absDelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!isWheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }

    view->map->scaleBy(scale, mbgl::ScreenCoordinate { view->lastX, view->lastY });
}

void GLFWView::onWindowResize(GLFWwindow *window, int width, int height) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    view->width = width;
    view->height = height;
    view->map->setSize({ static_cast<uint32_t>(view->width), static_cast<uint32_t>(view->height) });
}

void GLFWView::onFramebufferResize(GLFWwindow *window, int width, int height) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    view->backend->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    // This is only triggered when the framebuffer is resized, but not the window. It can
    // happen when you move the window between screens with a different pixel ratio.
    // We are forcing a repaint my invalidating the view, which triggers a rerender with the
    // new framebuffer dimensions.
    view->invalidate();
}

void GLFWView::onMouseClick(GLFWwindow *window, int button, int action, int modifiers) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT ||
        (button == GLFW_MOUSE_BUTTON_LEFT && modifiers & GLFW_MOD_CONTROL)) {
        view->rotating = action == GLFW_PRESS;
        view->map->setGestureInProgress(view->rotating);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && (modifiers & GLFW_MOD_SHIFT)) {
        view->pitching = action == GLFW_PRESS;
        view->map->setGestureInProgress(view->pitching);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        view->tracking = action == GLFW_PRESS;
        view->map->setGestureInProgress(view->tracking);

        if (action == GLFW_RELEASE) {
            double now = glfwGetTime();
            if (now - view->lastClick < 0.4 /* ms */) {
                if (modifiers & GLFW_MOD_SHIFT) {
                    view->map->scaleBy(0.5, mbgl::ScreenCoordinate { view->lastX, view->lastY }, mbgl::AnimationOptions{{mbgl::Milliseconds(500)}});
                } else {
                    view->map->scaleBy(2.0, mbgl::ScreenCoordinate { view->lastX, view->lastY }, mbgl::AnimationOptions{{mbgl::Milliseconds(500)}});
                }
            }
            view->lastClick = now;
        }
    }
}

void GLFWView::onMouseMove(GLFWwindow *window, double x, double y) {
    auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
    if (view->tracking) {
        const double dx = x - view->lastX;
        const double dy = y - view->lastY;
        if (dx || dy) {
            view->map->moveBy(mbgl::ScreenCoordinate { dx, dy });
        }
    } else if (view->rotating) {
        view->map->rotateBy({ view->lastX, view->lastY }, { x, y });
    } else if (view->pitching) {
        const double dy = y - view->lastY;
        if (dy) {
            view->map->pitchBy(dy / 2);
        }
    }
    view->lastX = x;
    view->lastY = y;
}

void GLFWView::onWindowFocus(GLFWwindow *window, int focused) {
    if (focused == GLFW_FALSE) { // Focus lost.
        auto *view = reinterpret_cast<GLFWView *>(glfwGetWindowUserPointer(window));
        view->rendererFrontend->getRenderer()->reduceMemoryUse();
    }
}

void GLFWView::run() {
    auto callback = [&] {
        if (glfwWindowShouldClose(window)) {
            runLoop.stop();
            return;
        }

        glfwPollEvents();

        if (dirty && rendererFrontend) {
            dirty = false;
            const double started = glfwGetTime();

            if (animateRouteCallback)
                animateRouteCallback(map);

            updateAnimatedAnnotations();

            mbgl::gfx::BackendScope scope { backend->getRendererBackend() };

            rendererFrontend->render();

            report(1000 * (glfwGetTime() - started));
            if (benchmark) {
                invalidate();
            }

        }
    };

    frameTick.start(mbgl::Duration::zero(), mbgl::Milliseconds(1000 / 60), callback);
#if defined(__APPLE__)
    while (!glfwWindowShouldClose(window)) runLoop.run();
#else
    runLoop.run();
#endif
}

float GLFWView::getPixelRatio() const {
    return pixelRatio;
}

mbgl::Size GLFWView::getSize() const {
    return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void GLFWView::invalidate() {
    dirty = true;
    glfwPostEmptyEvent();
}

void GLFWView::report(float duration) {
    frames++;
    frameTime += duration;

    const double currentTime = glfwGetTime();
    if (currentTime - lastReported >= 1) {
        frameTime /= frames;
        mbgl::Log::Info(mbgl::Event::OpenGL, "Frame time: %6.2fms (%6.2f fps)", frameTime,
            1000 / frameTime);
        frames = 0;
        frameTime = 0;
        lastReported = currentTime;
    }
}

void GLFWView::setChangeStyleCallback(std::function<void()> callback) {
    changeStyleCallback = callback;
}

void GLFWView::setShouldClose() {
    glfwSetWindowShouldClose(window, true);
    glfwPostEmptyEvent();
}

void GLFWView::setWindowTitle(const std::string& title) {
    glfwSetWindowTitle(window, (std::string { "Mapbox GL: " } + title).c_str());
}

void GLFWView::onDidFinishLoadingStyle() {
    if (show3DExtrusions) {
        toggle3DExtrusions(show3DExtrusions);
    }
}

void GLFWView::toggle3DExtrusions(bool visible) {
    using namespace mbgl::style;
    using namespace mbgl::style::expression::dsl;

    show3DExtrusions = visible;

    // Satellite-only style does not contain building extrusions data.
    if (!map->getStyle().getSource("composite")) {
        return;
    }

    if (auto layer = map->getStyle().getLayer("3d-buildings")) {
        layer->setVisibility(VisibilityType(!show3DExtrusions));
        return;
    }

    auto extrusionLayer = std::make_unique<FillExtrusionLayer>("3d-buildings", "composite");
    extrusionLayer->setSourceLayer("building");
    extrusionLayer->setMinZoom(15.0f);
    extrusionLayer->setFilter(Filter(eq(get("extrude"), literal("true"))));
    extrusionLayer->setFillExtrusionColor(PropertyExpression<mbgl::Color>(
        interpolate(linear(), number(get("height")),
                    0.f, toColor(literal("#160e23")),
                    50.f, toColor(literal("#00615f")),
                    100.f, toColor(literal("#55e9ff")))));
    extrusionLayer->setFillExtrusionOpacity(0.6f);
    extrusionLayer->setFillExtrusionHeight(PropertyExpression<float>(get("height")));
    extrusionLayer->setFillExtrusionBase(PropertyExpression<float>(get("min_height")));

    map->getStyle().addLayer(std::move(extrusionLayer));
}

void GLFWView::toggleCustomSource() {
    if (!map->getStyle().getSource("custom")) {
        mbgl::style::CustomGeometrySource::Options options;
        options.cancelTileFunction = [](const mbgl::CanonicalTileID&) {};
        options.fetchTileFunction = [&](const mbgl::CanonicalTileID& tileID) {
            double gridSpacing = 0.1;
            mbgl::FeatureCollection features;
            const mbgl::LatLngBounds bounds(tileID);
            for (double y = ceil(bounds.north() / gridSpacing) * gridSpacing; y >= floor(bounds.south() / gridSpacing) * gridSpacing; y -= gridSpacing) {

                mapbox::geojson::line_string gridLine;
                gridLine.emplace_back(bounds.west(), y);
                gridLine.emplace_back(bounds.east(), y);

                features.emplace_back(gridLine);
            }

            for (double x = floor(bounds.west() / gridSpacing) * gridSpacing; x <= ceil(bounds.east() / gridSpacing) * gridSpacing; x += gridSpacing) {
                mapbox::geojson::line_string gridLine;
                gridLine.emplace_back(x, bounds.south());
                gridLine.emplace_back(x, bounds.north());

                features.emplace_back(gridLine);
            }
            auto source = static_cast<mbgl::style::CustomGeometrySource *>(map->getStyle().getSource("custom"));
            if (source) {
                source->setTileData(tileID, features);
                source->invalidateTile(tileID);
            }
        };
        map->getStyle().addSource(std::make_unique<mbgl::style::CustomGeometrySource>("custom", options));
    }

    auto* layer = map->getStyle().getLayer("grid");

    if (!layer) {
        auto lineLayer = std::make_unique<mbgl::style::LineLayer>("grid", "custom");
        lineLayer->setLineColor(mbgl::Color{ 1.0, 0.0, 0.0, 1.0 });
        map->getStyle().addLayer(std::move(lineLayer));
    } else {
        layer->setVisibility(layer->getVisibility() == mbgl::style::VisibilityType::Visible ?
                             mbgl::style::VisibilityType::None : mbgl::style::VisibilityType::Visible);
    }
}
