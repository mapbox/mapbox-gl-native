#ifndef MBGL_ANDROID_NATIVE_MAP_VIEW
#define MBGL_ANDROID_NATIVE_MAP_VIEW

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <string>
#include <jni.h>

namespace mbgl {
namespace android {

class NativeMapView : public mbgl::View, private mbgl::util::noncopyable {
public:
    NativeMapView(JNIEnv *env, jobject obj, float pixelRatio, int availableProcessors, size_t totalMemory);
    virtual ~NativeMapView();

    float getPixelRatio() const override;
    std::array<uint16_t, 2> getSize() const override;
    std::array<uint16_t, 2> getFramebufferSize() const override;
    void activate() override;
    void deactivate() override;
    void notify() override;
    void invalidate() override;
    void swap() override;

    void notifyMapChange(mbgl::MapChange) override;

    mbgl::Map &getMap();
    mbgl::DefaultFileSource &getFileSource();

    void enableFps(bool enable);
    void updateFps();

    void surfaceCreated();
    void surfaceChanged(int width, int height);
    void drawFrame(bool inProgress);

private:
    bool inEmulator();

private:
    JavaVM *vm = nullptr;
    jobject obj = nullptr;

    std::string styleUrl;
    std::string apiKey;

    bool firstTime = false;

    bool fpsEnabled = false;
    double fps = 0.0;

    int width = 0;
    int height = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    const float pixelRatio;

    int availableProcessors = 0;
    size_t totalMemory = 0;

    // Ensure these are initialised last
    mbgl::SQLiteCache fileCache;
    mbgl::DefaultFileSource fileSource;
    mbgl::Map map;

    std::atomic_flag clean = ATOMIC_FLAG_INIT;
};
}
}

#endif
