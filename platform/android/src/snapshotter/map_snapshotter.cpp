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

    jFileSource = FileSource::getNativePeer(_env, _jFileSource);
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    optional<mbgl::CameraOptions> cameraOptions;
    if (position) {
        cameraOptions = CameraPosition::getCameraOptions(_env, position, pixelRatio);
    }

    optional<mbgl::LatLngBounds> bounds;
    if (region) {
        bounds = LatLngBounds::getLatLngBounds(_env, region);
    }

    std::pair<bool, std::string> style;
    if (styleJSON) {
        style = std::make_pair(true, jni::Make<std::string>(_env, styleJSON));
    } else {
        style = std::make_pair(false, jni::Make<std::string>(_env, styleURL));
    }

    showLogo = _showLogo;
    // Create the core snapshotter
    snapshotter = std::make_unique<mbgl::MapSnapshotter>(style,
                                                         size,
                                                         pixelRatio,
                                                         cameraOptions,
                                                         bounds,
                                                         _localIdeographFontFamily ?
                                                            jni::Make<std::string>(_env, _localIdeographFontFamily) :
                                                            optional<std::string>{},
                                                         mbgl::android::FileSource::getSharedResourceOptions(_env, _jFileSource));
}

MapSnapshotter::~MapSnapshotter() = default;

void MapSnapshotter::start(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    activateFilesource(env);

    snapshotCallback = std::make_unique<Actor<mbgl::MapSnapshotter::Callback>>(
            *Scheduler::GetCurrent(),
            [this](std::exception_ptr err, PremultipliedImage image, std::vector<std::string> attributions, mbgl::MapSnapshotter::PointForFn pointForFn, mbgl::MapSnapshotter::LatLngForFn latLngForFn) {
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

    snapshotter->snapshot(snapshotCallback->self());
}

void MapSnapshotter::cancel(JNIEnv& env) {
    MBGL_VERIFY_THREAD(tid);
    snapshotCallback.reset();
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

// Static methods //

void MapSnapshotter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    static auto& javaClass = jni::Class<MapSnapshotter>::Singleton(env);

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapSnapshotter>(env, javaClass, "nativePtr",
                                            jni::MakePeer<MapSnapshotter, const jni::Object<MapSnapshotter>&, const jni::Object<FileSource>&, jni::jfloat, jni::jint, jni::jint, const jni::String&, const jni::String&, const jni::Object<LatLngBounds>&, const jni::Object<CameraPosition>&, jni::jboolean, const jni::String&>,
                                           "nativeInitialize",
                                           "finalize",
                                            METHOD(&MapSnapshotter::setStyleUrl, "setStyleUrl"),
                                            METHOD(&MapSnapshotter::setStyleJson, "setStyleJson"),
                                            METHOD(&MapSnapshotter::setSize, "setSize"),
                                            METHOD(&MapSnapshotter::setCameraPosition, "setCameraPosition"),
                                            METHOD(&MapSnapshotter::setRegion, "setRegion"),
                                            METHOD(&MapSnapshotter::start, "nativeStart"),
                                            METHOD(&MapSnapshotter::cancel, "nativeCancel")
    );
}

} // namespace android
} // namespace mbgl
