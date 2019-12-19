#include <mbgl/test.hpp>
#include "test_runner_common.hpp"

#include <unistd.h>
#include <thread>

using namespace mbgl;
using namespace mbgl::android;

std::atomic<bool> running{true};
std::once_flag done;
ALooper* looper = NULL;

void runner() {
    std::vector<std::string> arguments = {"mbgl-test-runner", "--gtest_output=xml:/sdcard/test/results/results.xml"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back((char*)arg.data());
    }
    argv.push_back(nullptr);

    mbgl::Log::Info(mbgl::Event::General, "Start TestRunner");
    int status = mbgl::runTests(argv.size(), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "TestRunner finished with status: '%d'", status);
    running = false;
    ALooper_wake(looper);
}

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env = nullptr;
    std::thread runnerThread;
    app->activity->vm->AttachCurrentThread(&env, NULL);
    looper = ALooper_forThread();

    std::string storagePath(app->activity->internalDataPath);
    std::string zipFile = storagePath + "/data.zip";

    if (copyFile(env, app->activity->assetManager, zipFile, storagePath, "data.zip")) {
        if (chdir("/sdcard")) {
            mbgl::Log::Error(mbgl::Event::General, "Failed to change the directory to /sdcard");
            changeState(env, app, false);
        } else {
            unZipFile(env, zipFile, "/sdcard/");
            runnerThread = std::thread(runner);
        }
    } else {
        mbgl::Log::Error(mbgl::Event::General, "Failed to copy zip file '%s' to external storage", zipFile.c_str());
        changeState(env, app, false);
    }

    int outFd, outEvents;
    struct android_poll_source* source = nullptr;

    while (true) {
        ALooper_pollAll(-1, &outFd, &outEvents, reinterpret_cast<void**>(&source));
        if (source != nullptr) {
            source->process(app, source);
        }

        if (!running) {
            std::call_once(done, [&] {
                mbgl::Log::Info(mbgl::Event::General, "TestRunner done");
                runnerThread.join();
                changeState(env, app, true);
            });
        }

        if (app->destroyRequested != 0) {
            app->activity->vm->DetachCurrentThread();
            mbgl::Log::Info(mbgl::Event::General, "Close the App!");
            return;
        }
    }
}
