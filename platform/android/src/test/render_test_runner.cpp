#include <android_native_app_glue.h>
#include <mbgl/render_test.hpp>
#include "jni.hpp"
#include "logger.hpp"

#include <string>
#include <vector>

#include <mbgl/util/logging.hpp>

#include <android/log.h>

namespace mbgl {

namespace {

int severityToPriority(EventSeverity severity) {
    switch (severity) {
        case EventSeverity::Debug:
            return ANDROID_LOG_DEBUG;

        case EventSeverity::Info:
            return ANDROID_LOG_INFO;

        case EventSeverity::Warning:
            return ANDROID_LOG_WARN;

        case EventSeverity::Error:
            return ANDROID_LOG_ERROR;

        default:
            return ANDROID_LOG_VERBOSE;
    }
}

} // namespace

void Log::platformRecord(EventSeverity severity, const std::string& msg) {
    __android_log_print(severityToPriority(severity), "mbgl", "%s", msg.c_str());
}

} // namespace mbgl

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread(&env, NULL);

    std::vector<std::string> arguments = {"mbgl-render-test-runner", "-p", "/sdcard/render-test/android-manifest.json"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back((char*)arg.data());
    }
    argv.push_back(nullptr);
    (void)mbgl::runRenderTests(argv.size() - 1, argv.data());
    app->activity->vm->DetachCurrentThread();
}