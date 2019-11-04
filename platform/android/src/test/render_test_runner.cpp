#include <android_native_app_glue.h>
#include <mbgl/render_test.hpp>
#include <mbgl/util/logging.hpp>

#include <android/asset_manager.h>
#include <android/log.h>

#include "jni.hpp"

#include <cstdio>
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

namespace {
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

bool copyFile(JNIEnv* env,
              AAssetManager* assetManager,
              const std::string& filePath,
              const std::string& destinationPath,
              const std::string& fileName) {
    jclass fileClass = env->FindClass("java/io/File");
    jmethodID fileCtor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jmethodID fileExists = env->GetMethodID(fileClass, "exists", "()Z");

    jstring destination = env->NewStringUTF(destinationPath.c_str());
    jstring file = env->NewStringUTF(fileName.c_str());
    jobject fileToCopy = env->NewObject(fileClass, fileCtor, destination, file);
    bool stateOk = true;
    if (env->CallBooleanMethod(fileToCopy, fileExists)) {
        mbgl::Log::Warning(mbgl::Event::General, "File '%s' already exists", filePath.c_str());
    } else {
        AAsset* fileAsset = AAssetManager_open(assetManager, fileName.c_str(), AASSET_MODE_BUFFER);
        const void* fileData = AAsset_getBuffer(fileAsset);
        const off_t fileLen = AAsset_getLength(fileAsset);

        FILE* newFile = std::fopen(filePath.c_str(), "w+");
        stateOk = newFile != NULL;
        if (!stateOk) {
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
    env->DeleteLocalRef(fileClass);
    return stateOk;
}

void unZipFile(JNIEnv* env, const std::string& zipFilePath, const std::string& destinationPath) {
    jclass fileClass = env->FindClass("java/io/File");
    jmethodID fileCtor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jmethodID fileExists = env->GetMethodID(fileClass, "exists", "()Z");
    jmethodID fileIsDirectory = env->GetMethodID(fileClass, "isDirectory", "()Z");
    jmethodID deleteFile = env->GetMethodID(fileClass, "delete", "()Z");
    jmethodID createNewFile = env->GetMethodID(fileClass, "createNewFile", "()Z");
    jmethodID fileGetName = env->GetMethodID(fileClass, "getName", "()Ljava/lang/String;");

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
    jmethodID zipGetName = env->GetMethodID(zipEntry, "getName", "()Ljava/lang/String;");
    jmethodID zipIsDirectory = env->GetMethodID(zipEntry, "isDirectory", "()Z");

    // Upzip the resource folder to destination path

    jstring destination = env->NewStringUTF(destinationPath.c_str());
    jstring zipFile = env->NewStringUTF(zipFilePath.c_str());
    jobject fileIn = env->NewObject(fileInputStream, finCtor, zipFile);
    jobject zipIn = env->NewObject(zipInputStream, zinCtor, fileIn);
    jobject zEntry = NULL;
    while ((zEntry = env->CallObjectMethod(zipIn, zinGetNextEntry)) != NULL) {
        jstring dir = (jstring)env->CallObjectMethod(zEntry, zipGetName);
        std::string name = jstring2string(env, dir);
        bool isDir = env->CallBooleanMethod(zEntry, zipIsDirectory);

        jobject f = env->NewObject(fileClass, fileCtor, destination, dir);
        if (isDir) {
            if (!(env->CallBooleanMethod(f, fileIsDirectory))) {
                jmethodID mkdirs = env->GetMethodID(fileClass, "mkdirs", "()Z");
                bool success = (env->CallBooleanMethod(f, mkdirs));
                std::string fileName = jstring2string(env, (jstring)env->CallObjectMethod(f, fileGetName));
                if (!success) {
                    mbgl::Log::Warning(
                        mbgl::Event::General, "Failed to create folder entry %s from zip", fileName.c_str());
                }
            }
        } else if (!(env->CallBooleanMethod(f, fileExists))) {
            bool success = env->CallBooleanMethod(f, createNewFile);
            std::string fileName = jstring2string(env, (jstring)env->CallObjectMethod(f, fileGetName));
            if (!success) {
                mbgl::Log::Warning(mbgl::Event::General, "Failed to create folder entry %s from zip", fileName.c_str());
                continue;
            }
            jobject fout = env->NewObject(fileOutputStream, foutCtor, f);
            jbyteArray jBuff = env->NewByteArray(2048);
            int count;
            while ((count = env->CallIntMethod(zipIn, zinRead, jBuff)) != -1) {
                env->CallVoidMethod(fout, foutWrite, jBuff, 0, count);
            }
            env->CallVoidMethod(zipIn, zinCloseEntry);
            env->CallVoidMethod(fout, foutClose);
            env->DeleteLocalRef(jBuff);
        }
    }

    jobject fileToDelete = env->NewObject(fileClass, fileCtor, destination, zipFile);
    if (env->CallBooleanMethod(fileToDelete, fileExists)) {
        jboolean success = (env->CallBooleanMethod(fileToDelete, deleteFile));
        if (!success) {
            mbgl::Log::Warning(mbgl::Event::General, "Failed to delete file entry %s", zipFilePath.c_str());
        }
    }

    env->DeleteLocalRef(fileInputStream);
    env->DeleteLocalRef(fileOutputStream);
    env->DeleteLocalRef(zipInputStream);
    env->DeleteLocalRef(zipEntry);
    env->DeleteLocalRef(fileClass);
}
} // namespace

void android_main(struct android_app* app) {
    mbgl::android::theJVM = app->activity->vm;
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, NULL);
    const char* storage_chars = app->activity->internalDataPath;
    std::string storagePath(storage_chars);
    std::string zipFile = storagePath + "/data.zip";

    if (!copyFile(env, app->activity->assetManager, zipFile, storagePath, "data.zip")) {
        mbgl::Log::Error(
            mbgl::Event::General, "Failed to copy zip File '%s' to external storage for upzipping", zipFile.c_str());
    } else {
        unZipFile(env, zipFile, storagePath);
        std::string configFile = storagePath + "/android-manifest.json";

        std::vector<std::string> arguments = {"mbgl-render-test-runner", "-p", configFile};
        std::vector<char*> argv;
        for (const auto& arg : arguments) {
            argv.push_back((char*)arg.data());
        }
        argv.push_back(nullptr);
        (void)mbgl::runRenderTests(argv.size() - 1, argv.data());
    }

    app->activity->vm->DetachCurrentThread();
}