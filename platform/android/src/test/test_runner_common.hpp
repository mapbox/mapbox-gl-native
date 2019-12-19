#pragma once

#include <android_native_app_glue.h>

#include <mbgl/util/logging.hpp>

#include "jni.hpp"

namespace mbgl {
namespace android {

int severityToPriority(EventSeverity severity);
void changeState(JNIEnv* env, struct android_app* app, bool result);
bool copyFile(JNIEnv* env,
              AAssetManager* assetManager,
              const std::string& filePath,
              const std::string& destinationPath,
              const std::string& fileName);

void unZipFile(JNIEnv* env, const std::string& zipFilePath, const std::string& destinationPath);

} // namespace android
} // namespace mbgl
