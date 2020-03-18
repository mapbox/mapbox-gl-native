#include "map_snapshotter.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/actor/scheduler.hpp>

#include "../attach_env.hpp"
#include "map_snapshot.hpp"

namespace mbgl {
namespace android {

MapSnapshotter::MapSnapshotter(jni::JNIEnv& _env,
                               const jni::Object<MapSnapshotter>& _obj,
                               const jni::Object<FileSource>& _jFileSource,
                               jni::jfloat _pixelRatio,
                               jni::jint width,
                               jni::jint height,
                               const jni::String& styleURL,
                               const jni::String& styleJSON,
                               const jni::Object<LatLngBounds>& region,
                               const jni::Object<CameraPosition>& position,
                               jni::jboolean _showLogo,
                               const jni::String& _localIdeographFontFamily)
        : javaPeer(_env, _obj)
        , pixelRatio(_pixelRatio) {

    // Get a reference to the JavaVM for callbacks
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    weakScheduler = mbgl::Scheduler::GetCurrent()->makeWeakPtr();

    jFileSource = FileSource::getNativePeer(_env, _jFileSource);
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    showLogo = _showLogo;
    // Create the core snapshotter
    snapshotter = std::make_unique<mbgl::MapSnapshotter>(
        size,
        pixelRatio,
        mbgl::android::FileSource::getSharedResourceOptions(_env, _jFileSource),
        *this,
        _localIdeographFontFamily ? jni::Make<std::string>(_env, _localIdeographFontFamily) : optional<std::string>{});

    if (position) {
        snapshotter->setCameraOptions(CameraPosition::getCameraOptions(_env, position, pixelRatio));
    }

    if (region) {
        snapshotter->setRegion(LatLngBounds::getLatLngBounds(_env, region));
    }

    if (styleJSON) {
        snapshotter->setStyleJSON(jni::Make<std::string>(_env, styleJSON));
    } else {
        snapshotter->setStyleURL(jni::Make<std::string>(_env, styleURL));
    }
    activateFilesource(_env);
}

MapSnapshotter::~MapSnapshotter() {
    auto guard = weakScheduler.lock();
    if (weakScheduler && weakScheduler.get() != mbgl::Scheduler::GetCurrent()) {
        snapshotter->cancel();
        std::shared_ptr<mbgl::MapSnapshotter> shared = std::move(snapshotter);
        weakScheduler->schedule([s = std::move(shared)] {
            (void)s;
        });
    } else {
        snapshotter.reset();
    }
    vm = nullptr;
}

void MapSnapshotter::start(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    activateFilesource(env);
    snapshotter->snapshot([this](std::exception_ptr err,
                                 PremultipliedImage image,
                                 std::vector<std::string> attributions,
                                 mbgl::MapSnapshotter::PointForFn pointForFn,
                                 mbgl::MapSnapshotter::LatLngForFn latLngForFn) {
        MBGL_VERIFY_THREAD(tid);
        android::UniqueEnv _env = android::AttachEnv();
        static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(*_env);

        if (err) {
            // error handler callback
            static auto onSnapshotFailed = javaClass.GetMethod<void (jni::String)>(*_env, "onSnapshotFailed");
            auto weakReference = javaPeer.get(*_env);
            if (weakReference) {
                weakReference.Call(*_env, onSnapshotFailed, jni::Make<jni::String>(*_env, util::toString(err)));
            }
        } else {
            // Create the wrapper
            auto mapSnapshot = android::MapSnapshot::New(*_env, std::move(image), pixelRatio, attributions, showLogo, pointForFn, latLngForFn);

            // invoke callback
            static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<MapSnapshot>)>(*_env, "onSnapshotReady");
            auto weakReference = javaPeer.get(*_env);
            if (weakReference) {
                weakReference.Call(*_env, onSnapshotReady, mapSnapshot);
            }
        }

        deactivateFilesource(*_env);
    });
}

void MapSnapshotter::cancel(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    snapshotter->cancel();
    deactivateFilesource(env);
}

void MapSnapshotter::setStyleUrl(JNIEnv& env, const jni::String& styleURL) {
    snapshotter->setStyleURL(jni::Make<std::string>(env, styleURL));
}

void MapSnapshotter::setStyleJson(JNIEnv& env, const jni::String& styleJSON) {
    snapshotter->setStyleJSON(jni::Make<std::string>(env, styleJSON));
}

void MapSnapshotter::setSize(JNIEnv&, jni::jint width, jni::jint height) {
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    snapshotter->setSize(size);
}

void MapSnapshotter::setCameraPosition(JNIEnv& env, const jni::Object<CameraPosition>& position) {
    auto options = CameraPosition::getCameraOptions(env, position, pixelRatio);
    snapshotter->setCameraOptions(options);
}

void MapSnapshotter::setRegion(JNIEnv& env, const jni::Object<LatLngBounds>& region) {
    snapshotter->setRegion(LatLngBounds::getLatLngBounds(env, region));
}

// Private methods //

void MapSnapshotter::activateFilesource(JNIEnv& env) {
    if (!activatedFilesource) {
        activatedFilesource = true;
        jFileSource->resume(env);
    }
}

void MapSnapshotter::deactivateFilesource(JNIEnv& env) {
    if (activatedFilesource) {
        activatedFilesource = false;
        jFileSource->pause(env);
    }
}

void MapSnapshotter::onDidFailLoadingStyle(const std::string& error) {
    MBGL_VERIFY_THREAD(tid);
    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(*_env);
    static auto onDidFailLoadingStyle = javaClass.GetMethod<void(jni::String)>(*_env, "onDidFailLoadingStyle");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFailLoadingStyle, jni::Make<jni::String>(*_env, error));
    }
}

void MapSnapshotter::onDidFinishLoadingStyle() {
    MBGL_VERIFY_THREAD(tid);
    android::UniqueEnv _env = android::AttachEnv();

    static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(*_env);
    static auto onDidFinishLoadingStyle = javaClass.GetMethod<void()>(*_env, "onDidFinishLoadingStyle");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFinishLoadingStyle);
    }
}

void MapSnapshotter::onStyleImageMissing(const std::string& imageName) {
    MBGL_VERIFY_THREAD(tid);
    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(*_env);
    static auto onStyleImageMissing = javaClass.GetMethod<void(jni::String)>(*_env, "onStyleImageMissing");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onStyleImageMissing, jni::Make<jni::String>(*_env, imageName));
    }
}

void MapSnapshotter::addLayerAt(JNIEnv& env, jlong nativeLayerPtr, jni::jint index) {
    assert(nativeLayerPtr != 0);
    auto layers = snapshotter->getStyle().getLayers();
    Layer* layer = reinterpret_cast<Layer*>(nativeLayerPtr);
    // Check index
    int numLayers = layers.size() - 1;
    if (index > numLayers || index < 0) {
        Log::Error(Event::JNI, "Index out of range: %i", index);
        jni::ThrowNew(env,
                      jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"),
                      std::string("Invalid index").c_str());
    }
    // Insert it below the current at that index
    try {
        layer->addToStyle(snapshotter->getStyle(), layers.at(index)->getID());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(
            env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void MapSnapshotter::addLayerBelow(JNIEnv& env, jlong nativeLayerPtr, const jni::String& below) {
    assert(nativeLayerPtr != 0);

    Layer* layer = reinterpret_cast<Layer*>(nativeLayerPtr);
    try {
        layer->addToStyle(
            snapshotter->getStyle(),
            below ? mbgl::optional<std::string>(jni::Make<std::string>(env, below)) : mbgl::optional<std::string>());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(
            env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void MapSnapshotter::addLayerAbove(JNIEnv& env, jlong nativeLayerPtr, const jni::String& above) {
    assert(nativeLayerPtr != 0);
    Layer* layer = reinterpret_cast<Layer*>(nativeLayerPtr);

    // Find the sibling
    auto layers = snapshotter->getStyle().getLayers();
    auto siblingId = jni::Make<std::string>(env, above);

    size_t index = 0;
    for (auto l : layers) {
        if (l->getID() == siblingId) {
            break;
        }
        index++;
    }

    // Check if we found a sibling to place before
    mbgl::optional<std::string> before;
    if (index + 1 > layers.size()) {
        // Not found
        jni::ThrowNew(env,
                      jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"),
                      std::string("Could not find layer: ").append(siblingId).c_str());
        return;
    } else if (index + 1 < layers.size()) {
        // Place before the sibling
        before = {layers.at(index + 1)->getID()};
    }

    // Add the layer
    try {
        layer->addToStyle(snapshotter->getStyle(), before);
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(
            env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void MapSnapshotter::addSource(JNIEnv& env, const jni::Object<Source>& obj, jlong sourcePtr) {
    assert(sourcePtr != 0);

    Source* source = reinterpret_cast<Source*>(sourcePtr);
    try {
        source->addToStyle(env, obj, snapshotter->getStyle());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(
            env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/CannotAddSourceException"), error.what());
    }
}

void MapSnapshotter::addImages(JNIEnv& env, const jni::Array<jni::Object<mbgl::android::Image>>& jimages) {
    jni::NullCheck(env, &jimages);
    std::size_t len = jimages.Length(env);

    for (std::size_t i = 0; i < len; i++) {
        auto image = mbgl::android::Image::getImage(env, jimages.Get(env, i));
        snapshotter->getStyle().addImage(std::make_unique<mbgl::style::Image>(image));
    }
}

// Static methods //

void MapSnapshotter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(env);

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapSnapshotter>(env,
                                            javaClass,
                                            "nativePtr",
                                            jni::MakePeer<MapSnapshotter,
                                                          const jni::Object<MapSnapshotter>&,
                                                          const jni::Object<FileSource>&,
                                                          jni::jfloat,
                                                          jni::jint,
                                                          jni::jint,
                                                          const jni::String&,
                                                          const jni::String&,
                                                          const jni::Object<LatLngBounds>&,
                                                          const jni::Object<CameraPosition>&,
                                                          jni::jboolean,
                                                          const jni::String&>,
                                            "nativeInitialize",
                                            "finalize",
                                            METHOD(&MapSnapshotter::setStyleUrl, "nativeSetStyleUrl"),
                                            METHOD(&MapSnapshotter::addLayerAt, "nativeAddLayerAt"),
                                            METHOD(&MapSnapshotter::addLayerBelow, "nativeAddLayerBelow"),
                                            METHOD(&MapSnapshotter::addLayerAbove, "nativeAddLayerAbove"),
                                            METHOD(&MapSnapshotter::addSource, "nativeAddSource"),
                                            METHOD(&MapSnapshotter::addImages, "nativeAddImages"),
                                            METHOD(&MapSnapshotter::setStyleJson, "nativeSetStyleJson"),
                                            METHOD(&MapSnapshotter::setSize, "setSize"),
                                            METHOD(&MapSnapshotter::setCameraPosition, "setCameraPosition"),
                                            METHOD(&MapSnapshotter::setRegion, "setRegion"),
                                            METHOD(&MapSnapshotter::start, "nativeStart"),
                                            METHOD(&MapSnapshotter::cancel, "nativeCancel"));
}

} // namespace android
} // namespace mbgl
