#pragma once

#include <jni/jni.hpp>
#include <mapbox/std/weak.hpp>
#include <mbgl/map/map_snapshotter.hpp>
#include <mbgl/util/util.hpp>

#include <memory>

#include "../file_source.hpp"
#include "../geometry/lat_lng_bounds.hpp"
#include "../map/camera_position.hpp"
#include "../map/image.hpp"
#include "../style/layers/layer.hpp"
#include "../style/sources/source.hpp"

namespace mbgl {
namespace android {

class MapSnapshotter final : public mbgl::MapSnapshotterObserver {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/snapshotter/MapSnapshotter"; };

    static void registerNative(jni::JNIEnv&);

    MapSnapshotter(jni::JNIEnv&,
                   const jni::Object<MapSnapshotter>&,
                   const jni::Object<FileSource>&,
                   jni::jfloat pixelRatio,
                   jni::jint width,
                   jni::jint height,
                   const jni::String& styleURL,
                   const jni::String& styleJSON,
                   const jni::Object<LatLngBounds>& region,
                   const jni::Object<CameraPosition>& position,
                   jni::jboolean showLogo,
                   const jni::String& localIdeographFontFamily);

    virtual ~MapSnapshotter() override;

    void setStyleUrl(JNIEnv&, const jni::String& styleURL);

    void setStyleJson(JNIEnv&, const jni::String& styleJSON);

    void setSize(JNIEnv&, jni::jint width, jni::jint height);

    void setCameraPosition(JNIEnv&, const jni::Object<CameraPosition>& position);

    void setRegion(JNIEnv&, const jni::Object<LatLngBounds>& region);

    void start(JNIEnv&);

    void cancel(JNIEnv&);

    void addLayerAt(JNIEnv&, jlong, jni::jint);
    void addLayerBelow(JNIEnv&, jlong, const jni::String&);
    void addLayerAbove(JNIEnv&, jlong, const jni::String&);
    void addSource(JNIEnv&, const jni::Object<Source>&, jlong nativePtr);
    void addImages(JNIEnv&, const jni::Array<jni::Object<mbgl::android::Image>>&);
    jni::Local<jni::Object<Layer>> getLayer(JNIEnv&, const jni::String&);
    jni::Local<jni::Object<Source>> getSource(JNIEnv&, const jni::String&);

    // MapSnapshotterObserver overrides
    void onDidFailLoadingStyle(const std::string&) override;
    void onDidFinishLoadingStyle() override;
    void onStyleImageMissing(const std::string&) override;

private:
    MBGL_STORE_THREAD(tid);

    JavaVM *vm = nullptr;
    jni::WeakReference<jni::Object<MapSnapshotter>, jni::EnvAttachingDeleter> javaPeer;

    float pixelRatio;
    bool showLogo;

    FileSource *jFileSource;
    void activateFilesource(JNIEnv&);
    void deactivateFilesource(JNIEnv&);
    bool activatedFilesource = false;
    mapbox::base::WeakPtr<mbgl::Scheduler> weakScheduler;
    std::unique_ptr<mbgl::MapSnapshotter> snapshotter;
};

} // namespace android
} // namespace mbgl
