#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <string>
#include <jni.h>

namespace mbgl {
namespace android {

class NativeMapView : public mbgl::View, public mbgl::Backend {
public:
    NativeMapView(JNIEnv *env, jobject obj, float pixelRatio, int availableProcessors, size_t totalMemory);
    virtual ~NativeMapView();

    // mbgl::View //

    void bind() override;

    // mbgl::Backend //

    void invalidate() override;
    void notifyMapChange(mbgl::MapChange) override;

    // JNI //

    mbgl::Map &getMap();
    mbgl::DefaultFileSource &getFileSource();

    void render();

    void enableFps(bool enable);

    void onViewportChanged(int width, int height);

    mbgl::EdgeInsets getInsets() { return insets;}
    void setInsets(mbgl::EdgeInsets insets_);

    void scheduleTakeSnapshot();

protected:
    // Unused //

    void activate() override {};
    void deactivate() override {};

private:
    void wake();
    void updateViewBinding();
    mbgl::Size getFramebufferSize() const;

    void resizeView(int width, int height);
    void resizeFramebuffer(int width, int height);

    void updateFps();

private:

    JavaVM *vm = nullptr;
    JNIEnv *env = nullptr;
    jweak obj = nullptr;

    std::string styleUrl;
    std::string apiKey;

    float pixelRatio;
    bool fpsEnabled = false;
    bool snapshot = false;
    double fps = 0.0;

    int width = 0;
    int height = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    bool framebufferSizeChanged = true;

    int availableProcessors = 0;
    size_t totalMemory = 0;

    // Ensure these are initialised last
    std::unique_ptr<mbgl::util::RunLoop> runLoop;
    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
    mbgl::ThreadPool threadPool;
    std::unique_ptr<mbgl::Map> map;
    mbgl::EdgeInsets insets;

};
}
}
