#include <android_native_app_glue.h>
#include <mbgl/benchmark.hpp>
#include <mbgl/util/logging.hpp>

#include <android/asset_manager.h>
#include <android/log.h>

#include "jni.hpp"

#include <unistd.h>
#include <cstdio>
#include <string>
#include <thread>

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

namespace {

template <class T>
class JavaWrapper {
public:
    JavaWrapper(JNIEnv* env_, T obj_) : env(env_), obj(obj_) {}
    ~JavaWrapper() {
        env->DeleteLocalRef(obj);
        env = nullptr;
        obj = NULL;
    }
    T& get() { return obj; }

private:
    JavaWrapper() = delete;
    JNIEnv* env;
    T obj;
};

void changeState(JNIEnv* env, struct android_app* app, bool result) {
    jobject nativeActivity = app->activity->clazz;
    jclass acl = env->GetObjectClass(nativeActivity);
    jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(nativeActivity, getClassLoader);
    JavaWrapper<jclass> classLoader(env, env->FindClass("java/lang/ClassLoader"));
    jmethodID findClass = env->GetMethodID(classLoader.get(), "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    JavaWrapper<jstring> strClassName(env, env->NewStringUTF("android.app.TestState"));
    jclass testStateClass = static_cast<jclass>(env->CallObjectMethod(cls, findClass, strClassName.get()));
    if (testStateClass != NULL) {
        jfieldID id0 = env->GetStaticFieldID(testStateClass, "testResult", "Z");
        env->SetStaticBooleanField(testStateClass, id0, result);
        jfieldID id = env->GetStaticFieldID(testStateClass, "running", "Z");
        env->SetStaticBooleanField(testStateClass, id, false);
    }
}

bool copyFile(JNIEnv* env,
              AAssetManager* assetManager,
              const std::string& filePath,
              const std::string& destinationPath,
              const std::string& fileName) {
    JavaWrapper<jclass> fileClass(env, env->FindClass("java/io/File"));
    jmethodID fileCtor = env->GetMethodID(fileClass.get(), "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jmethodID fileExists = env->GetMethodID(fileClass.get(), "exists", "()Z");

    JavaWrapper<jstring> destination(env, env->NewStringUTF(destinationPath.c_str()));
    JavaWrapper<jstring> file(env, env->NewStringUTF(fileName.c_str()));
    JavaWrapper<jobject> fileToCopy(env, env->NewObject(fileClass.get(), fileCtor, destination.get(), file.get()));

    bool stateOk = true;
    if (env->CallBooleanMethod(fileToCopy.get(), fileExists)) {
        mbgl::Log::Warning(mbgl::Event::General, "File '%s' already exists", filePath.c_str());
    } else {
        std::unique_ptr<AAsset, std::function<void(AAsset*)>> fileAsset(
            AAssetManager_open(assetManager, fileName.c_str(), AASSET_MODE_BUFFER),
            [](AAsset* asset) { AAsset_close(asset); });
        if (fileAsset == nullptr) {
            mbgl::Log::Warning(mbgl::Event::General, "Failed to open asset file %s", fileName.c_str());
            return false;
        }
        const void* fileData = AAsset_getBuffer(fileAsset.get());
        const off_t fileLen = AAsset_getLength(fileAsset.get());

        std::unique_ptr<FILE, std::function<void(FILE*)>> newFile(std::fopen(filePath.c_str(), "w+"),
                                                                  [](FILE* file) { std::fclose(file); });
        stateOk = newFile != nullptr;

        if (!stateOk) {
            mbgl::Log::Warning(mbgl::Event::General, "Failed to create new file entry %s", fileName.c_str());
        } else {
            auto res = static_cast<off_t>(std::fwrite(fileData, sizeof(char), fileLen, newFile.get()));
            if (fileLen != res) {
                mbgl::Log::Warning(
                    mbgl::Event::General, "Failed to generate file entry %s from assets", fileName.c_str());
            }
        }
    }
    return stateOk;
}

std::string jstringToStdString(JNIEnv* env, jstring jStr) {
    if (!jStr) {
        return std::string();
    }

    JavaWrapper<jclass> stringClass(env, env->GetObjectClass(jStr));
    const jmethodID getBytes = env->GetMethodID(stringClass.get(), "getBytes", "(Ljava/lang/String;)[B");
    JavaWrapper<jbyteArray> stringJbytes(
        env,
        static_cast<jbyteArray>(
            env->CallObjectMethod(jStr, getBytes, JavaWrapper<jstring>(env, env->NewStringUTF("UTF-8")).get())));

    size_t length = static_cast<size_t>(env->GetArrayLength(stringJbytes.get()));
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes.get(), NULL);

    std::string ret = std::string(reinterpret_cast<char*>(pBytes), length);
    env->ReleaseByteArrayElements(stringJbytes.get(), pBytes, JNI_ABORT);

    return ret;
}

void unZipFile(JNIEnv* env, const std::string& zipFilePath, const std::string& destinationPath) {
    JavaWrapper<jclass> fileClassWrapper(env, env->FindClass("java/io/File"));
    auto fileClass = fileClassWrapper.get();
    jmethodID fileCtor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jmethodID fileExists = env->GetMethodID(fileClass, "exists", "()Z");
    jmethodID fileIsDirectory = env->GetMethodID(fileClass, "isDirectory", "()Z");
    jmethodID deleteFile = env->GetMethodID(fileClass, "delete", "()Z");
    jmethodID createNewFile = env->GetMethodID(fileClass, "createNewFile", "()Z");
    jmethodID fileGetName = env->GetMethodID(fileClass, "getName", "()Ljava/lang/String;");

    JavaWrapper<jclass> fileInputStreamWrapper(env, env->FindClass("java/io/FileInputStream"));
    auto fileInputStream = fileInputStreamWrapper.get();
    jmethodID finCtor = env->GetMethodID(fileInputStream, "<init>", "(Ljava/lang/String;)V");

    JavaWrapper<jclass> fileOutputStreamWrapper(env, env->FindClass("java/io/FileOutputStream"));
    auto fileOutputStream = fileOutputStreamWrapper.get();
    jmethodID foutCtor = env->GetMethodID(fileOutputStream, "<init>", "(Ljava/io/File;)V");
    jmethodID foutClose = env->GetMethodID(fileOutputStream, "close", "()V");
    jmethodID foutWrite = env->GetMethodID(fileOutputStream, "write", "([BII)V");

    JavaWrapper<jclass> zipInputStreamWrapper(env, env->FindClass("java/util/zip/ZipInputStream"));
    auto zipInputStream = zipInputStreamWrapper.get();
    jmethodID zinCtor = env->GetMethodID(zipInputStream, "<init>", "(Ljava/io/InputStream;)V");
    jmethodID zinGetNextEntry = env->GetMethodID(zipInputStream, "getNextEntry", "()Ljava/util/zip/ZipEntry;");
    jmethodID zinRead = env->GetMethodID(zipInputStream, "read", "([B)I");
    jmethodID zinCloseEntry = env->GetMethodID(zipInputStream, "closeEntry", "()V");

    JavaWrapper<jclass> zipEntryWrapper(env, env->FindClass("java/util/zip/ZipEntry"));
    auto zipEntry = zipEntryWrapper.get();
    jmethodID zipGetName = env->GetMethodID(zipEntry, "getName", "()Ljava/lang/String;");
    jmethodID zipIsDirectory = env->GetMethodID(zipEntry, "isDirectory", "()Z");

    // Unzip the resource folder to destination path
    JavaWrapper<jstring> destination(env, env->NewStringUTF(destinationPath.c_str()));
    JavaWrapper<jstring> zipFile(env, env->NewStringUTF(zipFilePath.c_str()));
    JavaWrapper<jobject> fileIn(env, env->NewObject(fileInputStream, finCtor, zipFile.get()));
    JavaWrapper<jobject> zipIn(env, env->NewObject(zipInputStream, zinCtor, fileIn.get()));

    while (true) {
        JavaWrapper<jobject> obj(env, env->CallObjectMethod(zipIn.get(), zinGetNextEntry));
        jobject zEntry = obj.get();
        if (zEntry == NULL) {
            break;
        }
        jstring dir = static_cast<jstring>(env->CallObjectMethod(zEntry, zipGetName));
        std::string name = jstringToStdString(env, dir);
        bool isDir = env->CallBooleanMethod(zEntry, zipIsDirectory);

        JavaWrapper<jobject> fileHandle(env, env->NewObject(fileClass, fileCtor, destination.get(), dir));
        jobject& f = fileHandle.get();
        std::string fileName = jstringToStdString(env, static_cast<jstring>(env->CallObjectMethod(f, fileGetName)));

        if (isDir) {
            if (!(env->CallBooleanMethod(f, fileIsDirectory))) {
                jmethodID mkdirs = env->GetMethodID(fileClass, "mkdirs", "()Z");
                bool success = (env->CallBooleanMethod(f, mkdirs));
                std::string fileNameStr =
                    jstringToStdString(env, static_cast<jstring>(env->CallObjectMethod(f, fileGetName)));

                if (!success) {
                    mbgl::Log::Warning(
                        mbgl::Event::General, "Failed to create folder entry %s from zip", fileNameStr.c_str());
                }
            }
        } else if (!(env->CallBooleanMethod(f, fileExists))) {
            bool success = env->CallBooleanMethod(f, createNewFile);
            std::string fileNameStr =
                jstringToStdString(env, static_cast<jstring>(env->CallObjectMethod(f, fileGetName)));

            if (!success) {
                mbgl::Log::Warning(
                    mbgl::Event::General, "Failed to create folder entry %s from zip", fileNameStr.c_str());
                continue;
            }

            JavaWrapper<jobject> fout(env, env->NewObject(fileOutputStream, foutCtor, f));
            JavaWrapper<jbyteArray> jBuff(env, env->NewByteArray(2048));

            int count;
            while ((count = env->CallIntMethod(zipIn.get(), zinRead, jBuff.get())) != -1) {
                env->CallVoidMethod(fout.get(), foutWrite, jBuff.get(), 0, count);
            }

            env->CallVoidMethod(zipIn.get(), zinCloseEntry);
            env->CallVoidMethod(fout.get(), foutClose);
        }
    }

    JavaWrapper<jobject> fileToDelete(env, env->NewObject(fileClass, fileCtor, destination.get(), zipFile.get()));
    if (env->CallBooleanMethod(fileToDelete.get(), fileExists)) {
        jboolean success = (env->CallBooleanMethod(fileToDelete.get(), deleteFile));
        if (!success) {
            mbgl::Log::Warning(mbgl::Event::General, "Failed to delete file entry %s", zipFilePath.c_str());
        }
    }
}

} // namespace

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
