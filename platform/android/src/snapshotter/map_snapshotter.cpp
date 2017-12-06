#include "map_snapshotter.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/actor/scheduler.hpp>

#include "../attach_env.hpp"
#include "map_snapshot.hpp"

namespace mbgl {
namespace android {

MapSnapshotter::MapSnapshotter(jni::JNIEnv& _env,
                               jni::Object<MapSnapshotter> _obj,
                               jni::Object<FileSource> _jFileSource,
                               jni::jfloat _pixelRatio,
                               jni::jint width,
                               jni::jint height,
                               jni::String styleURL,
                               jni::Object<LatLngBounds> region,
                               jni::Object<CameraPosition> position,
                               jni::jboolean _showLogo,
                               jni::String _programCacheDir)
        : javaPeer(SeizeGenericWeakRef(_env, jni::Object<MapSnapshotter>(jni::NewWeakGlobalRef(_env, _obj.Get()).release())))
        , pixelRatio(_pixelRatio)
        , threadPool(sharedThreadPool()) {

    // Get a reference to the JavaVM for callbacks
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    jFileSource = FileSource::getNativePeer(_env, _jFileSource);
    auto& fileSource = mbgl::android::FileSource::getDefaultFileSource(_env, _jFileSource);
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    auto cameraOptions = position ? CameraPosition::getCameraOptions(_env, position) : CameraOptions();
    optional<mbgl::LatLngBounds> bounds;
    if (region) {
        bounds = LatLngBounds::getLatLngBounds(_env, region);
    }
    
    showLogo = _showLogo;
    // Create the core snapshotter
    snapshotter = std::make_unique<mbgl::MapSnapshotter>(fileSource,
                                                         *threadPool,
                                                         jni::Make<std::string>(_env, styleURL),
                                                         size,
                                                         pixelRatio,
                                                         cameraOptions,
                                                         bounds,
                                                         jni::Make<std::string>(_env, _programCacheDir));

}

MapSnapshotter::~MapSnapshotter() = default;

void MapSnapshotter::start(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    activateFilesource(env);

    snapshotCallback = std::make_unique<Actor<mbgl::MapSnapshotter::Callback>>(
            *Scheduler::GetCurrent(),
            [this](std::exception_ptr err, PremultipliedImage image, std::vector<std::string> attributions, mbgl::MapSnapshotter::PointForFn pointForFn) {
        MBGL_VERIFY_THREAD(tid);
        android::UniqueEnv _env = android::AttachEnv();

        if (err) {
            // error handler callback
            static auto onSnapshotFailed = javaClass.GetMethod<void (jni::String)>(*_env, "onSnapshotFailed");
            javaPeer->Call(*_env, onSnapshotFailed, jni::Make<jni::String>(*_env, util::toString(err)));
        } else {
            // Create the wrapper
            auto mapSnapshot = android::MapSnapshot::New(*_env, std::move(image), pixelRatio, attributions, showLogo, pointForFn);

            // invoke callback
            static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<MapSnapshot>)>(*_env, "onSnapshotReady");
            javaPeer->Call(*_env, onSnapshotReady, mapSnapshot);
        }

        deactivateFilesource(*_env);
    });

    snapshotter->snapshot(snapshotCallback->self());
}

void MapSnapshotter::cancel(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    snapshotCallback.reset();
    deactivateFilesource(env);
}

void MapSnapshotter::setStyleUrl(JNIEnv& env, jni::String styleURL) {
    snapshotter->setStyleURL(jni::Make<std::string>(env, styleURL));
}

void MapSnapshotter::setSize(JNIEnv&, jni::jint width, jni::jint height) {
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    snapshotter->setSize(size);
}

void MapSnapshotter::setCameraPosition(JNIEnv& env, jni::Object<CameraPosition> position) {
    auto options = CameraPosition::getCameraOptions(env, position);
    snapshotter->setCameraOptions(options);
}

void MapSnapshotter::setRegion(JNIEnv& env, jni::Object<LatLngBounds> region) {
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

// Static methods //

jni::Class<MapSnapshotter> MapSnapshotter::javaClass;

void MapSnapshotter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    MapSnapshotter::javaClass = *jni::Class<MapSnapshotter>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapSnapshotter>(env, MapSnapshotter::javaClass, "nativePtr",
                                            std::make_unique<MapSnapshotter, JNIEnv&, jni::Object<MapSnapshotter>, jni::Object<FileSource>, jni::jfloat, jni::jint, jni::jint, jni::String, jni::Object<LatLngBounds>, jni::Object<CameraPosition>, jni::jboolean, jni::String>,
                                           "nativeInitialize",
                                           "finalize",
                                            METHOD(&MapSnapshotter::setStyleUrl, "setStyleUrl"),
                                            METHOD(&MapSnapshotter::setSize, "setSize"),
                                            METHOD(&MapSnapshotter::setCameraPosition, "setCameraPosition"),
                                            METHOD(&MapSnapshotter::setRegion, "setRegion"),
                                            METHOD(&MapSnapshotter::start, "nativeStart"),
                                            METHOD(&MapSnapshotter::cancel, "nativeCancel")
    );
}

} // namespace android
} // namespace mbgl