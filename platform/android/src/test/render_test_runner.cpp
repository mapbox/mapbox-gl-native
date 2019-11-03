#include <android_native_app_glue.h>
#include <ghc/filesystem.hpp>
#include <mbgl/render_test.hpp>
#include <mbgl/util/logging.hpp>

#include <android/asset_manager.h>
#include <android/log.h>

#include "jni.hpp"

#include <string>
#include <vector>

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

void copyFile(AAssetManager* assetManager, const std::string& filePath, const std::string& fileName) {
    if (ghc::filesystem::exists(filePath)) {
        mbgl::Log::Warning(mbgl::Event::General, "File '%s' already exists", filePath.c_str());
    } else {
        AAsset* fileAsset = AAssetManager_open(assetManager, fileName.c_str(), AASSET_MODE_BUFFER);
        const void* fileData = AAsset_getBuffer(fileAsset);
        const off_t fileLen = AAsset_getLength(fileAsset);

        FILE* newFile = std::fopen(filePath.c_str(), "w+");
        if (NULL == newFile) {
            mbgl::Log::Warning(mbgl::Event::General, "Failed to create new file entry %s", fileName.c_str());
        } else {
            auto res = std::fwrite(fileData, sizeof(char), fileLen, newFile);
            if (fileLen != res) {
                mbgl::Log::Warning(
                    mbgl::Event::General, "Failed to generate file entry %s from assets", fileName.c_str());
            }
        }
        std::fclose(newFile);
        AAsset_close(fileAsset);
    }
}

std::string jstring2string(JNIEnv* env, jstring jStr) {
    if (!jStr) {
        return "";
    }

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray)env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t)env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char*)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, NULL);
    const char* storage_chars = app->activity->internalDataPath;
    std::string storagePath(storage_chars);

    std::string zipFile = storagePath + "/res.zip";

    AAssetManager* assetManager = app->activity->assetManager;
    copyFile(assetManager, zipFile, "res.zip");

    jclass fileInputStream = env->FindClass("java/io/FileInputStream");
    jmethodID finCtor = env->GetMethodID(fileInputStream, "<init>", "(Ljava/lang/String;)V");

    jclass fileOutputStream = env->FindClass("java/io/FileOutputStream");
    jmethodID foutCtor = env->GetMethodID(fileOutputStream, "<init>", "(Ljava/io/File;)V");
    jmethodID foutClose = env->GetMethodID(fileOutputStream, "close", "()V");
    jmethodID foutWrite = env->GetMethodID(fileOutputStream, "write", "([BII)V");

    jclass zipInputStream = env->FindClass("java/util/zip/ZipInputStream");
    jmethodID zinCtor = env->GetMethodID(zipInputStream, "<init>", "(Ljava/io/InputStream;)V");
    jmethodID zinGetNextEntry = env->GetMethodID(zipInputStream, "getNextEntry", "()Ljava/util/zip/ZipEntry;");
    jmethodID zinRead = env->GetMethodID(zipInputStream, "read", "([B)I");
    jmethodID zinCloseEntry = env->GetMethodID(zipInputStream, "closeEntry", "()V");

    jclass zipEntry = env->FindClass("java/util/zip/ZipEntry");
    jmethodID getName = env->GetMethodID(zipEntry, "getName", "()Ljava/lang/String;");
    jmethodID zipIsDirectory = env->GetMethodID(zipEntry, "isDirectory", "()Z");

    jclass fileClass = env->FindClass("java/io/File");
    jmethodID fileCtor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jmethodID fileIsDirectory = env->GetMethodID(fileClass, "isDirectory", "()Z");
    jmethodID exists = env->GetMethodID(fileClass, "exists", "()Z");
    jmethodID createNewFile = env->GetMethodID(fileClass, "createNewFile", "()Z");
    jmethodID fileGetName = env->GetMethodID(fileClass, "getName", "()Ljava/lang/String;");

    // Upzip the resource folder to destination path
    jstring destination = env->NewStringUTF(storage_chars);
    jstring jstr = env->NewStringUTF(zipFile.c_str());
    jobject fin = env->NewObject(fileInputStream, finCtor, jstr);
    jobject zin = env->NewObject(zipInputStream, zinCtor, fin);
    jobject ze = NULL;
    while ((ze = env->CallObjectMethod(zin, zinGetNextEntry)) != NULL) {
        jstring dir = (jstring)env->CallObjectMethod(ze, getName);
        std::string name = jstring2string(env, dir);
        bool ok = env->CallBooleanMethod(ze, zipIsDirectory);

        jobject f = env->NewObject(fileClass, fileCtor, destination, dir);
        if (ok) {
            if (!(env->CallBooleanMethod(f, fileIsDirectory))) {
                jmethodID mkdirs = env->GetMethodID(fileClass, "mkdirs", "()Z");
                jboolean success = (env->CallBooleanMethod(f, mkdirs));
                std::string fileName = jstring2string(env, (jstring)env->CallObjectMethod(f, fileGetName));
                if (!success) {
                    mbgl::Log::Warning(
                        mbgl::Event::General, "Failed to create folder entry %s from zip", fileName.c_str());
                }
            }
        } else {
            if (!(env->CallBooleanMethod(f, exists))) {
                jboolean success = env->CallBooleanMethod(f, createNewFile);
                std::string fileName = jstring2string(env, (jstring)env->CallObjectMethod(f, fileGetName));
                if (!success) {
                    mbgl::Log::Warning(
                        mbgl::Event::General, "Failed to create folder entry %s from zip", fileName.c_str());
                    continue;
                }
                jobject fout = env->NewObject(fileOutputStream, foutCtor, f);
                jbyteArray jBuff = env->NewByteArray(2048);
                int count;
                while ((count = env->CallIntMethod(zin, zinRead, jBuff)) != -1) {
                    env->CallVoidMethod(fout, foutWrite, jBuff, 0, count);
                }
                env->CallVoidMethod(zin, zinCloseEntry);
                env->CallVoidMethod(fout, foutClose);
                env->DeleteLocalRef(jBuff);
            }
        }
    }

    std::string configFile = storagePath + "/android-manifest.json";

    std::vector<std::string> arguments = {"mbgl-render-test-runner", "-p", configFile};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back((char*)arg.data());
    }
    argv.push_back(nullptr);
    (void)mbgl::runRenderTests(argv.size() - 1, argv.data());

    app->activity->vm->DetachCurrentThread();
}