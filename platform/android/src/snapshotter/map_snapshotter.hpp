#pragma once

#include <mbgl/map/map_snapshotter.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/util.hpp>

#include "../file_source.hpp"
#include "../geometry/lat_lng_bounds.hpp"
#include "../map/camera_position.hpp"

#include <jni/jni.hpp>
#include "../jni/generic_global_ref_deleter.hpp"

#include <memory>

namespace mbgl {
namespace android {

class SnapshotterRendererFrontend;

class MapSnapshotter {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/snapshotter/MapSnapshotter"; };

    static jni::Class<MapSnapshotter> javaClass;

    static void registerNative(jni::JNIEnv&);

    MapSnapshotter(jni::JNIEnv&,
                   jni::Object<MapSnapshotter>,
                   jni::Object<FileSource>,
                   jni::jfloat pixelRatio,
                   jni::jint width,
                   jni::jint height,
                   jni::String styleURL,
                   jni::Object<LatLngBounds> region,
                   jni::Object<CameraPosition> position,
                   jni::jboolean showLogo,
                   jni::String programCacheDir);

    ~MapSnapshotter();

    void setStyleUrl(JNIEnv&, jni::String styleURL);

    void setSize(JNIEnv&, jni::jint width, jni::jint height);

    void setCameraPosition(JNIEnv&, jni::Object<CameraPosition> position);

    void setRegion(JNIEnv&, jni::Object<LatLngBounds> region);

    void start(JNIEnv&);

    void cancel(JNIEnv&);

private:
    MBGL_STORE_THREAD(tid);

    JavaVM *vm = nullptr;
    GenericUniqueWeakObject<MapSnapshotter> javaPeer;

    float pixelRatio;
    bool showLogo;

    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<Actor<mbgl::MapSnapshotter::Callback>> snapshotCallback;
    std::unique_ptr<mbgl::MapSnapshotter> snapshotter;

    FileSource *jFileSource;
    void activateFilesource(JNIEnv&);
    void deactivateFilesource(JNIEnv&);
    bool activatedFilesource = false;
};

} // namespace android
} // namespace mbgl