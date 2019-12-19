#include <mbgl/benchmark.hpp>
#include "test_runner_common.hpp"

#include <unistd.h>
#include <thread>

using namespace mbgl;
using namespace mbgl::android;

bool running = false;
bool done = false;
ALooper* looper = NULL;

void runner() {
    std::vector<std::string> arguments = {"mbgl-benchmark-runner",
                                          "--benchmark_repetitions=3",
                                          "--benchmark_format=json",
                                          "--benchmark_out=/sdcard/benchmark/results/results.json"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back(const_cast<char*>(arg.data()));
    }
    argv.push_back(nullptr);

    mbgl::Log::Info(mbgl::Event::General, "Start BenchmarkRunner");
    int status = mbgl::runBenchmark(argv.size(), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "BenchmarkRunner finished with status: '%d'", status);
    running = false;
    ALooper_wake(looper);
}

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env = nullptr;
    std::thread benchmarkThread;
    app->activity->vm->AttachCurrentThread(&env, NULL);
    looper = ALooper_forThread();

    std::string storagePath(app->activity->internalDataPath);
    std::string zipFile = storagePath + "/data.zip";

    if (copyFile(env, app->activity->assetManager, zipFile, storagePath, "data.zip")) {
        if (chdir("/sdcard")) {
            mbgl::Log::Error(mbgl::Event::General, "Failed to change the directory to /sdcard");
            done = true;
            changeState(env, app, false);
        } else {
            unZipFile(env, zipFile, "/sdcard/");
            running = true;
            benchmarkThread = std::thread(runner);
        }
    } else {
        mbgl::Log::Error(mbgl::Event::General, "Failed to copy zip file '%s' to external storage", zipFile.c_str());
        done = true;
        changeState(env, app, false);
    }

    int outFd, outEvents;
    struct android_poll_source* source = nullptr;

    while (true) {
        ALooper_pollAll(-1, &outFd, &outEvents, reinterpret_cast<void**>(&source));
        if (source != nullptr) {
            source->process(app, source);
        }

        if (!running && !done) {
            mbgl::Log::Info(mbgl::Event::General, "BenchmarkRunner done");
            done = true;
            benchmarkThread.join();
            changeState(env, app, true);
        }

        if (app->destroyRequested != 0) {
            app->activity->vm->DetachCurrentThread();
            mbgl::Log::Info(mbgl::Event::General, "Close the App!");
            return;
        }
    }
}
