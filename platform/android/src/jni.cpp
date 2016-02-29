#include <cstdint>
#include <cinttypes>
#include <cassert>

#include <string>
#include <locale>
#include <codecvt>
#include <array>
#include <vector>

#include <android/native_window_jni.h>
#include <sys/system_properties.h>

#include "jni.hpp"
#include "native_map_view.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/string.hpp>

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace mbgl {
namespace android {

void RegisterNativeHTTPRequest(JNIEnv&);

JavaVM* theJVM;

std::string cachePath;
std::string dataPath;
std::string apkPath;
std::string androidRelease;

jmethodID onInvalidateId = nullptr;
jmethodID onMapChangedId = nullptr;
jmethodID onFpsChangedId = nullptr;

jclass latLngClass = nullptr;
jmethodID latLngConstructorId = nullptr;
jfieldID latLngLatitudeId = nullptr;
jfieldID latLngLongitudeId = nullptr;

jclass latLngBoundsClass = nullptr;
jmethodID latLngBoundsConstructorId = nullptr;
jfieldID latLngBoundsLatNorthId = nullptr;
jfieldID latLngBoundsLatSouthId = nullptr;
jfieldID latLngBoundsLonEastId = nullptr;
jfieldID latLngBoundsLonWestId = nullptr;

jclass iconClass = nullptr;
jfieldID iconIdId = nullptr;

jclass markerClass = nullptr;
jfieldID markerPositionId = nullptr;
jfieldID markerIconId = nullptr;
jfieldID markerIdId = nullptr;

jclass polylineClass = nullptr;
jfieldID polylineAlphaId = nullptr;
jfieldID polylineColorId = nullptr;
jfieldID polylineWidthId = nullptr;
jfieldID polylinePointsId = nullptr;

jclass polygonClass = nullptr;
jfieldID polygonAlphaId = nullptr;
jfieldID polygonFillColorId = nullptr;
jfieldID polygonStrokeColorId = nullptr;
jfieldID polygonPointsId = nullptr;

jclass runtimeExceptionClass = nullptr;
jclass nullPointerExceptionClass = nullptr;

jmethodID listToArrayId = nullptr;

jclass arrayListClass = nullptr;
jmethodID arrayListConstructorId = nullptr;
jmethodID arrayListAddId = nullptr;

jclass projectedMetersClass = nullptr;
jmethodID projectedMetersConstructorId = nullptr;
jfieldID projectedMetersNorthingId = nullptr;
jfieldID projectedMetersEastingId = nullptr;

jclass pointFClass = nullptr;
jmethodID pointFConstructorId = nullptr;
jfieldID pointFXId = nullptr;
jfieldID pointFYId = nullptr;

jclass rectFClass = nullptr;
jmethodID rectFConstructorId = nullptr;
jfieldID rectFLeftId = nullptr;
jfieldID rectFTopId = nullptr;
jfieldID rectFRightId = nullptr;
jfieldID rectFBottomId = nullptr;

jclass customLayerClass = nullptr;
jfieldID customLayerIdId = nullptr;
jfieldID customLayerContextId = nullptr;
jfieldID customLayerInitializeFunctionId = nullptr;
jfieldID customLayerRenderFunctionId = nullptr;
jfieldID customLayerDeinitializeFunctionId = nullptr;

// Offline declarations start

jclass offlineManagerClass  = nullptr;
jfieldID offlineManagerClassPtrId = nullptr;

jclass listOfflineRegionsCallbackClass  = nullptr;
jmethodID listOnListMethodId = nullptr;
jmethodID listOnErrorMethodId = nullptr;

jclass offlineRegionClass = nullptr;
jmethodID offlineRegionConstructorId = nullptr;
jfieldID offlineRegionOfflineManagerId = nullptr;
jfieldID offlineRegionIdId = nullptr;
jfieldID offlineRegionDefinitionId = nullptr;
jfieldID offlineRegionMetadataId = nullptr;
jfieldID offlineRegionPtrId = nullptr;

jclass offlineRegionDefinitionClass = nullptr;
jmethodID offlineRegionDefinitionConstructorId = nullptr;
jfieldID offlineRegionDefinitionStyleURLId = nullptr;
jfieldID offlineRegionDefinitionBoundsId = nullptr;
jfieldID offlineRegionDefinitionMinZoomId = nullptr;
jfieldID offlineRegionDefinitionMaxZoomId = nullptr;
jfieldID offlineRegionDefinitionPixelRatioId = nullptr;

jclass createOfflineRegionCallbackClass = nullptr;
jmethodID createOnCreateMethodId = nullptr;
jmethodID createOnErrorMethodId = nullptr;

jclass offlineRegionObserverClass = nullptr;
jmethodID offlineRegionObserveronStatusChangedId = nullptr;
jmethodID offlineRegionObserveronErrorId = nullptr;
jmethodID offlineRegionObserveronLimitId = nullptr;

jclass offlineRegionStatusClass = nullptr;
jmethodID offlineRegionStatusConstructorId = nullptr;
jfieldID offlineRegionStatusDownloadStateId = nullptr;
jfieldID offlineRegionStatusCompletedResourceCountId = nullptr;
jfieldID offlineRegionStatusCompletedResourceSizeId = nullptr;
jfieldID offlineRegionStatusRequiredResourceCountId = nullptr;
jfieldID offlineRegionStatusRequiredResourceCountIsPreciseId = nullptr;

jclass offlineRegionErrorClass = nullptr;
jmethodID offlineRegionErrorConstructorId = nullptr;
jfieldID offlineRegionErrorReasonId = nullptr;
jfieldID offlineRegionErrorMessageId = nullptr;

jclass offlineRegionStatusCallbackClass = nullptr;
jmethodID offlineRegionStatusOnStatusId = nullptr;
jmethodID offlineRegionStatusOnErrorId = nullptr;

jclass offlineRegionDeleteCallbackClass = nullptr;
jmethodID offlineRegionDeleteOnDeleteId = nullptr;
jmethodID offlineRegionDeleteOnErrorId = nullptr;

// Offline declarations end

bool throw_jni_error(JNIEnv *env, const char *msg) {
    if (env->ThrowNew(runtimeExceptionClass, msg) < 0) {
        env->ExceptionDescribe();
        return false;
    }

    return true;
}

bool attach_jni_thread(JavaVM* vm, JNIEnv** env, std::string threadName) {
    assert(vm != nullptr);
    assert(env != nullptr);

    JavaVMAttachArgs args = {JNI_VERSION_1_2, threadName.c_str(), NULL};

    jint ret;
    *env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void **>(env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            throw new std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw new std::runtime_error("AttachCurrentThread() failed");
            }
            detach = true;
        }
    }

    return detach;
}

void detach_jni_thread(JavaVM* vm, JNIEnv** env, bool detach) {
    if (detach) {
        assert(vm != nullptr);
        assert(env != nullptr);

        jint ret;
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            throw new std::runtime_error("DetachCurrentThread() failed");
        }
    }
    *env = nullptr;
}

std::string std_string_from_jstring(JNIEnv *env, jstring jstr) {
    std::string str;

    if (jstr == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "String cannot be null.") < 0) {
            env->ExceptionDescribe();
            return str;
        }

        return str;
    }

    jsize len = env->GetStringLength(jstr);
    if (len < 0) {
        env->ExceptionDescribe();
        return str;
    }

    const jchar *chars = env->GetStringChars(jstr, nullptr);
    if (chars == nullptr) {
        env->ExceptionDescribe();
        return str;
    }

    std::u16string ustr(reinterpret_cast<const char16_t *>(chars), len);
    env->ReleaseStringChars(jstr, chars);
    chars = nullptr;
    str = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(ustr);
    return str;
}

jstring std_string_to_jstring(JNIEnv *env, std::string str) {
    std::u16string ustr =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(str);

    jstring jstr = env->NewString(reinterpret_cast<const jchar *>(ustr.c_str()), ustr.size());
    if (jstr == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return jstr;
}

std::vector<std::string> std_vector_string_from_jobject(JNIEnv *env, jobject jlist) {
    std::vector<std::string> vector;

    if (jlist == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return vector;
        }

        return vector;
    }

    jobjectArray jarray =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return vector;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return vector;
    }

    for (jsize i = 0; i < len; i++) {
        jstring jstr = reinterpret_cast<jstring>(env->GetObjectArrayElement(jarray, i));
        if (jstr == nullptr) {
            env->ExceptionDescribe();
            return vector;
        }

        vector.push_back(std_string_from_jstring(env, jstr));
    }

    env->DeleteLocalRef(jarray);
    jarray = nullptr;

    return vector;
}

jobject std_vector_string_to_jobject(JNIEnv *env, std::vector<std::string> vector) {
    jobject jlist = env->NewObject(arrayListClass, arrayListConstructorId);
    if (jlist == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    for (const auto& str : vector) {
        env->CallBooleanMethod(jlist, arrayListAddId, std_string_to_jstring(env, str));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }
    }

    return jlist;
}

jlongArray std_vector_uint_to_jobject(JNIEnv *env, std::vector<uint32_t> vector) {
    jlongArray jarray = env->NewLongArray(vector.size());
    if (jarray == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    std::vector<jlong> v;
    for (const uint32_t& id : vector) {
        v.push_back((jlong)id);
    }

    env->SetLongArrayRegion(jarray, 0, v.size(), &(v[0]));
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return jarray;
}

mbgl::AnnotationSegment annotation_segment_from_latlng_jlist(JNIEnv *env, jobject jlist) {
    mbgl::AnnotationSegment segment;

    if (jlist == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return segment;
        }
        return segment;
    }

    jobjectArray jarray =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return segment;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return segment;
    }

    segment.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject latLng = reinterpret_cast<jobject>(env->GetObjectArrayElement(jarray, i));
        if (latLng == nullptr) {
            env->ExceptionDescribe();
            return segment;
        }

        jdouble latitude = env->GetDoubleField(latLng, latLngLatitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return segment;
        }

        jdouble longitude = env->GetDoubleField(latLng, latLngLongitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return segment;
        }

        segment.push_back(mbgl::LatLng(latitude, longitude));
        env->DeleteLocalRef(latLng);
    }

    env->DeleteLocalRef(jarray);
    jarray = nullptr;

    return segment;
}

std::pair<mbgl::AnnotationSegment, mbgl::ShapeAnnotation::Properties> annotation_std_pair_from_polygon_jobject(JNIEnv *env, jobject polygon) {
    jfloat alpha = env->GetFloatField(polygon, polygonAlphaId);
    jint fillColor = env->GetIntField(polygon, polygonFillColorId);
    jint strokeColor = env->GetIntField(polygon, polygonStrokeColorId);

    int rF = (fillColor >> 16) & 0xFF;
    int gF = (fillColor >> 8) & 0xFF;
    int bF = (fillColor) & 0xFF;
    int aF = (fillColor >> 24) & 0xFF;

    int rS = (strokeColor >> 16) & 0xFF;
    int gS = (strokeColor >> 8) & 0xFF;
    int bS = (strokeColor) & 0xFF;
    int aS = (strokeColor >> 24) & 0xFF;

    mbgl::ShapeAnnotation::Properties shapeProperties;
    mbgl::FillAnnotationProperties fillProperties;
    fillProperties.opacity = alpha;
    fillProperties.outlineColor = {{ static_cast<float>(rS) / 255.0f, static_cast<float>(gS) / 255.0f, static_cast<float>(bS) / 255.0f, static_cast<float>(aS) / 255.0f }};
    fillProperties.color = {{ static_cast<float>(rF) / 255.0f, static_cast<float>(gF) / 255.0f, static_cast<float>(bF) / 255.0f, static_cast<float>(aF) / 255.0f }};
    shapeProperties.set<mbgl::FillAnnotationProperties>(fillProperties);

    jobject points = env->GetObjectField(polygon, polygonPointsId);
    mbgl::AnnotationSegment segment = annotation_segment_from_latlng_jlist(env, points);
    env->DeleteLocalRef(points);

    return std::make_pair(segment, shapeProperties);
}

static std::vector<uint8_t> metadata_from_java(JNIEnv* env, jbyteArray j) {
    mbgl::Log::Debug(mbgl::Event::JNI, "metadata_from_java");
    jsize length = env->GetArrayLength(j);
    std::vector<uint8_t> c;
    c.resize(length);
    env->GetByteArrayRegion(j, 0, length, reinterpret_cast<jbyte*>(c.data()));
    return c;
}

static jbyteArray metadata_from_native(JNIEnv* env, const std::vector<uint8_t>& c) {
    mbgl::Log::Debug(mbgl::Event::JNI, "metadata_from_java");
    jsize length = static_cast<jsize>(c.size());
    jbyteArray j = env->NewByteArray(length);
    env->SetByteArrayRegion(j, 0, c.size(), reinterpret_cast<const jbyte*>(c.data()));
    return j;
}

static mbgl::LatLngBounds latlngbounds_from_java(JNIEnv *env, jobject latLngBounds) {
    // Checks
    if (env->ExceptionCheck() || (latLngBounds == nullptr)) {
        env->ExceptionDescribe();
        return mbgl::LatLngBounds::empty();
    }

    jdouble swLat = env->GetDoubleField(latLngBounds, latLngBoundsLatSouthId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return mbgl::LatLngBounds::empty();
    }

    jdouble swLon = env->GetDoubleField(latLngBounds, latLngBoundsLonWestId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return mbgl::LatLngBounds::empty();
    }

    jdouble neLat = env->GetDoubleField(latLngBounds, latLngBoundsLatNorthId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return mbgl::LatLngBounds::empty();
    }

    jdouble neLon = env->GetDoubleField(latLngBounds, latLngBoundsLonEastId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return mbgl::LatLngBounds::empty();
    }

    // Build object
    mbgl::LatLngBounds result = mbgl::LatLngBounds::hull({ swLat, swLon }, { neLat, neLon });
    return result;
}

static jobject latlngbounds_from_native(JNIEnv *env, mbgl::LatLngBounds bounds) {
    double northLatitude = bounds.north();
    double eastLongitude = bounds.east();
    double southLatitude = bounds.south();
    double westLongitude = bounds.west();

    jobject jbounds = env->NewObject(latLngBoundsClass, latLngBoundsConstructorId,
        northLatitude, eastLongitude, southLatitude, westLongitude);

    return jbounds;
}

}
}

namespace {

using namespace mbgl::android;
using DebugOptions = mbgl::MapDebugOptions;

jlong JNICALL nativeCreate(JNIEnv *env, jobject obj, jstring cachePath_, jstring dataPath_, jstring apkPath_, jfloat pixelRatio, jint availableProcessors, jlong totalMemory) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreate");
    cachePath = std_string_from_jstring(env, cachePath_);
    dataPath = std_string_from_jstring(env, dataPath_);
    apkPath = std_string_from_jstring(env, apkPath_);
    NativeMapView *nativeMapView = new NativeMapView(env, obj, pixelRatio, availableProcessors, totalMemory);
    jlong mapViewPtr = reinterpret_cast<jlong>(nativeMapView);
    return mapViewPtr;
}

void JNICALL nativeDestroy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    delete nativeMapView;
    nativeMapView = nullptr;
}

void JNICALL nativeInitializeDisplay(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeDisplay");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    try
    {
        nativeMapView->initializeDisplay();
    } catch(const std::exception& e) {
        throw_jni_error(env, "Unable to initialize GL display.");
    }
}

void JNICALL nativeTerminateDisplay(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateDisplay");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->terminateDisplay();
}

void JNICALL nativeInitializeContext(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeContext");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    try {
        nativeMapView->initializeContext();
    } catch(const std::exception& e) {
        throw_jni_error(env, "Unable to initialize GL context.");
    }
}

void JNICALL nativeTerminateContext(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateContext");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->terminateContext();
}

void JNICALL nativeCreateSurface(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject surface) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreateSurface");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    try {
        nativeMapView->createSurface(ANativeWindow_fromSurface(env, surface));
    } catch(const std::exception& e) {
        throw_jni_error(env, "Unable to create GL surface.");
    }
}

void JNICALL nativeDestroySurface(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroySurface");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->destroySurface();
}

void JNICALL nativePause(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativePause");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->pause();
}

jboolean JNICALL nativeIsPaused(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeIsPaused");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().isPaused();
}

void JNICALL nativeResume(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResume");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->resume();
}

void JNICALL nativeUpdate(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeUpdate");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().update(mbgl::Update::Repaint);
}

void JNICALL nativeRenderSync(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRenderSync");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().renderSync();
}

void JNICALL nativeViewResize(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jint width, jint height) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeViewResize");
    assert(nativeMapViewPtr != 0);
    assert(width >= 0);
    assert(height >= 0);
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->resizeView(width, height);
}

void JNICALL nativeFramebufferResize(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jint fbWidth, jint fbHeight) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeFramebufferResize");
    assert(nativeMapViewPtr != 0);
    assert(fbWidth >= 0);
    assert(fbHeight >= 0);
    assert(fbWidth <= UINT16_MAX);
    assert(fbHeight <= UINT16_MAX);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->resizeFramebuffer(fbWidth, fbHeight);
}

void JNICALL nativeRemoveClass(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring clazz) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveClass");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeClass(std_string_from_jstring(env, clazz));
}

jboolean JNICALL nativeHasClass(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring clazz) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeHasClass");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().hasClass(std_string_from_jstring(env, clazz));
}

void JNICALL nativeAddClass(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring clazz) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddClass");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().addClass(std_string_from_jstring(env, clazz));
}

void JNICALL nativeSetClasses(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject classes) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetClasses");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setClasses(std_vector_string_from_jobject(env, classes));
}

jobject JNICALL nativeGetClasses(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetClasses");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_vector_string_to_jobject(env, nativeMapView->getMap().getClasses());
}

void JNICALL nativeSetDefaultTransitionDuration(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
                                                jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetDefaultTransitionDuration");
    assert(nativeMapViewPtr != 0);
    assert(duration >= 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setDefaultTransitionDuration(mbgl::Milliseconds(duration));
}

jlong JNICALL nativeGetDefaultTransitionDuration(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetDefaultTransitionDuration");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std::chrono::duration_cast<mbgl::Milliseconds>(nativeMapView->getMap().getDefaultTransitionDuration()).count();
}

void JNICALL nativeSetStyleURL(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring url) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetStyleURL");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setStyleURL(std_string_from_jstring(env, url));
}

void JNICALL nativeSetStyleJSON(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
                                jstring newStyleJson, jstring base) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetStyleJSON");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setStyleJSON(std_string_from_jstring(env, newStyleJson),
                                         std_string_from_jstring(env, base));
}

jstring JNICALL nativeGetStyleJSON(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetStyleJSON");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_string_to_jstring(env, nativeMapView->getMap().getStyleJSON());
}

void JNICALL nativeSetAccessToken(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring accessToken) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetAccessToken");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getFileSource().setAccessToken(std_string_from_jstring(env, accessToken));
}

jstring JNICALL nativeGetAccessToken(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetAccessToken");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_string_to_jstring(env, nativeMapView->getFileSource().getAccessToken());
}

void JNICALL nativeCancelTransitions(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCancelTransitions");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().cancelTransitions();
}

void JNICALL nativeSetGestureInProgress(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean inProgress) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetGestureInProgress");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setGestureInProgress(inProgress);
}

void JNICALL nativeMoveBy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble dx, jdouble dy,
                          jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeMoveBy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(dx, dy);
    nativeMapView->getMap().moveBy(center, mbgl::Milliseconds(duration));
}

void JNICALL nativeSetLatLng(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject latLng,
                             jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetLatLng");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(latLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble longitude = env->GetDoubleField(latLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    nativeMapView->getMap().setLatLng(mbgl::LatLng(latitude, longitude), nativeMapView->getInsets(), mbgl::Duration(duration));
}

jobject JNICALL nativeGetLatLng(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetLatLng");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng(nativeMapView->getInsets());

    jobject ret = env->NewObject(latLngClass, latLngConstructorId, latLng.latitude, latLng.longitude);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

jdoubleArray JNICALL nativeGetCameraValues(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetCameraValues");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng(nativeMapView->getInsets());
    jdoubleArray  output = env->NewDoubleArray(5);
    jsize start = 0;
    jsize leng = 5;
    jdouble buf[5];
    buf[0] = latLng.latitude;
    buf[1] = latLng.longitude;
    buf[2] = nativeMapView->getMap().getBearing();
    buf[3] = nativeMapView->getMap().getPitch();
    buf[4] = nativeMapView->getMap().getZoom();
    env->SetDoubleArrayRegion(output, start, leng, buf);

    if (output == nullptr) {
       env->ExceptionDescribe();
       return nullptr;
    }

    return output;
}

void JNICALL nativeResetPosition(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetPosition");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetPosition();
}

jdouble JNICALL nativeGetPitch(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetPitch");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getPitch();
}

void JNICALL nativeSetPitch(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble pitch, jlong milliseconds) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetPitch");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::Duration duration((mbgl::Milliseconds(milliseconds)));
    nativeMapView->getMap().setPitch(pitch, duration);
}

void JNICALL nativeScaleBy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble ds, jdouble cx,
                           jdouble cy, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeScaleBy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().scaleBy(ds, center, mbgl::Milliseconds(duration));
}

void JNICALL nativeSetScale(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble scale,
                            jdouble cx, jdouble cy, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetScale");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().setScale(scale, center, mbgl::Milliseconds(duration));
}

jdouble JNICALL nativeGetScale(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetScale");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getScale();
}

void JNICALL nativeSetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble zoom, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setZoom(zoom, mbgl::Milliseconds(duration));
}

jdouble JNICALL nativeGetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getZoom();
}

void JNICALL nativeResetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetZoom();
}

void JNICALL nativeSetMinZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetMinZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setMinZoom(zoom);
}

jdouble JNICALL nativeGetMinZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMinZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMinZoom();
}

void JNICALL nativeSetMaxZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetMaxZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setMaxZoom(zoom);
}

jdouble JNICALL nativeGetMaxZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMaxZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMaxZoom();
}

void JNICALL nativeRotateBy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble sx,
                            jdouble sy, jdouble ex, jdouble ey, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRotateBy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate first(sx, sy);
    mbgl::ScreenCoordinate second(ex, ey);
    nativeMapView->getMap().rotateBy(first, second, mbgl::Milliseconds(duration));
}

void JNICALL nativeSetBearing(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble degrees,
                              jlong milliseconds) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::Duration duration((mbgl::Milliseconds(milliseconds)));
    nativeMapView->getMap().setBearing(degrees, duration);
}

void JNICALL nativeSetBearing(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble degrees,
                              jdouble cx, jdouble cy) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().setBearing(degrees, center);
}

jdouble JNICALL nativeGetBearing(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetBearing");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getBearing();
}

void JNICALL nativeResetNorth(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetNorth");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetNorth();
}

jlong JNICALL nativeAddMarker(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject marker) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddMarker");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jobject position = env->GetObjectField(marker, markerPositionId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jobject icon = env->GetObjectField(marker, markerIconId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jstring jid = reinterpret_cast<jstring>(env->GetObjectField(icon, iconIdId));
    std::string id = std_string_from_jstring(env, jid);

    jdouble latitude = env->GetDoubleField(position, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jdouble longitude = env->GetDoubleField(position, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    // Because Java only has int, not unsigned int, we need to bump the annotation id up to a long.
    return nativeMapView->getMap().addPointAnnotation(mbgl::PointAnnotation(mbgl::LatLng(latitude, longitude), id));
}

void JNICALL nativeUpdateMarker(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject marker) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeUpdateMarker");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jlong markerId = env->GetLongField(marker, markerIdId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    if (markerId == -1) {
        return;
    }

    jobject position = env->GetObjectField(marker, markerPositionId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    jobject icon = env->GetObjectField(marker, markerIconId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    jstring jid = reinterpret_cast<jstring>(env->GetObjectField(icon, iconIdId));
    std::string iconId = std_string_from_jstring(env, jid);

    jdouble latitude = env->GetDoubleField(position, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    jdouble longitude = env->GetDoubleField(position, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    // Because Java only has int, not unsigned int, we need to bump the annotation id up to a long.
    nativeMapView->getMap().updatePointAnnotation(markerId, mbgl::PointAnnotation(mbgl::LatLng(latitude, longitude), iconId));
}

jlongArray JNICALL nativeAddMarkers(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject jlist) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddMarkers");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<mbgl::PointAnnotation> markers;

    if (jlist == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return nullptr;
        }
        return nullptr;
    }

    jobjectArray jarray =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return nullptr;
    }

    markers.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject marker = reinterpret_cast<jobject>(env->GetObjectArrayElement(jarray, i));

        jobject position = env->GetObjectField(marker, markerPositionId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        jobject icon = env->GetObjectField(marker, markerIconId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }
        env->DeleteLocalRef(marker);

        jstring jid = reinterpret_cast<jstring>(env->GetObjectField(icon, iconIdId));
        env->DeleteLocalRef(icon);

        std::string id = std_string_from_jstring(env, jid);
        env->DeleteLocalRef(jid);

        jdouble latitude = env->GetDoubleField(position, latLngLatitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        jdouble longitude = env->GetDoubleField(position, latLngLongitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }
        env->DeleteLocalRef(position);

        markers.emplace_back(mbgl::PointAnnotation(mbgl::LatLng(latitude, longitude), id));
     }

    env->DeleteLocalRef(jarray);

    std::vector<uint32_t> pointAnnotationIDs = nativeMapView->getMap().addPointAnnotations(markers);
    return std_vector_uint_to_jobject(env, pointAnnotationIDs);
}


jlong JNICALL nativeAddPolyline(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject polyline) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolyline");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jfloat alpha = env->GetFloatField(polyline, polylineAlphaId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jint color = env->GetIntField(polyline, polylineColorId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = (color) & 0xFF;
    int a = (color >> 24) & 0xFF;

    jfloat width = env->GetFloatField(polyline, polylineWidthId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    mbgl::ShapeAnnotation::Properties shapeProperties;
    mbgl::LineAnnotationProperties lineProperties;
    lineProperties.opacity = alpha;
    lineProperties.color = {{ static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f }};
    lineProperties.width = width;
    shapeProperties.set<mbgl::LineAnnotationProperties>(lineProperties);

    jobject points = env->GetObjectField(polyline, polylinePointsId);
    mbgl::AnnotationSegment segment = annotation_segment_from_latlng_jlist(env, points);

    std::vector<mbgl::ShapeAnnotation> shapes;
    shapes.emplace_back(mbgl::AnnotationSegments { segment }, shapeProperties);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    uint32_t id = shapeAnnotationIDs.at(0);

    return id;
}

jlongArray JNICALL nativeAddPolylines(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject jlist) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolylines");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<mbgl::ShapeAnnotation> shapes;

    if (jlist == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return nullptr;
        }
        return nullptr;
    }

    jobjectArray jarray =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return nullptr;
    }

    shapes.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject polyline = reinterpret_cast<jobject>(env->GetObjectArrayElement(jarray, i));

        jfloat alpha = env->GetFloatField(polyline, polylineAlphaId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        jint color = env->GetIntField(polyline, polylineColorId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = (color) & 0xFF;
        int a = (color >> 24) & 0xFF;

        jfloat width = env->GetFloatField(polyline, polylineWidthId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        mbgl::ShapeAnnotation::Properties shapeProperties;
        mbgl::LineAnnotationProperties lineProperties;
        lineProperties.opacity = alpha;
        lineProperties.color = {{ static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f }};
        lineProperties.width = width;
        shapeProperties.set<mbgl::LineAnnotationProperties>(lineProperties);

        jobject points = env->GetObjectField(polyline, polylinePointsId);
        mbgl::AnnotationSegment segment = annotation_segment_from_latlng_jlist(env, points);

        shapes.emplace_back(mbgl::AnnotationSegments { segment }, shapeProperties);

        env->DeleteLocalRef(polyline);
    }

    env->DeleteLocalRef(jarray);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    return std_vector_uint_to_jobject(env, shapeAnnotationIDs);
}

jlong JNICALL nativeAddPolygon(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject polygon) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolygon");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<mbgl::ShapeAnnotation> shapes;
    std::pair<mbgl::AnnotationSegment, mbgl::ShapeAnnotation::Properties> segment = annotation_std_pair_from_polygon_jobject(env, polygon);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    shapes.emplace_back(mbgl::AnnotationSegments { segment.first }, segment.second);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    uint32_t id = shapeAnnotationIDs.at(0);
    return id;
}

jlongArray JNICALL nativeAddPolygons(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject jlist) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolygons");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<mbgl::ShapeAnnotation> shapes;

    if (jlist == nullptr) {
        if (env->ThrowNew(nullPointerExceptionClass, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return nullptr;
        }
        return nullptr;
    }

    jobjectArray jarray =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return nullptr;
    }

    shapes.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject polygon = reinterpret_cast<jobject>(env->GetObjectArrayElement(jarray, i));

        std::pair<mbgl::AnnotationSegment, mbgl::ShapeAnnotation::Properties> segment = annotation_std_pair_from_polygon_jobject(env, polygon);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        shapes.emplace_back(mbgl::AnnotationSegments { segment.first }, segment.second);

        env->DeleteLocalRef(polygon);
    }

    env->DeleteLocalRef(jarray);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    return std_vector_uint_to_jobject(env, shapeAnnotationIDs);
}

void JNICALL nativeRemoveAnnotation(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jlong annotationId) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveAnnotation");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeAnnotation(static_cast<uint32_t>(annotationId));
}

void JNICALL nativeRemoveAnnotations(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jlongArray jarray) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveAnnotations");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<uint32_t> ids;

    if (env->ExceptionCheck() || (jarray == nullptr)) {
        env->ExceptionDescribe();
        return;
    }

    jsize len = env->GetArrayLength(jarray);
    if (len < 0) {
        env->ExceptionDescribe();
        return;
    }

    ids.reserve(len);
    jlong* jids = env->GetLongArrayElements(jarray, nullptr);

    for (jsize i = 0; i < len; i++) {
        if(jids[i] == -1L)
            continue;
        ids.push_back(static_cast<uint32_t>(jids[i]));
    }

    env->ReleaseLongArrayElements(jarray, jids, JNI_ABORT);

    nativeMapView->getMap().removeAnnotations(ids);
}

jlongArray JNICALL nativeGetAnnotationsInBounds(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject latLngBounds_) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetAnnotationsInBounds");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    // Conversion
    mbgl::LatLngBounds latLngBounds = latlngbounds_from_java(env, latLngBounds_);
    if (latLngBounds.isEmpty()) {
        return nullptr;
    }

    // Assume only points for now
    std::vector<uint32_t> annotations = nativeMapView->getMap().getPointAnnotationsInBounds(
        latLngBounds);

    return std_vector_uint_to_jobject(env, annotations);
}

void JNICALL nativeAddAnnotationIcon(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
        jstring symbol, jint width, jint height, jfloat scale, jbyteArray jpixels) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddAnnotationIcon");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    const std::string symbolName = std_string_from_jstring(env, symbol);

    jsize size = env->GetArrayLength(jpixels);
    mbgl::PremultipliedImage premultipliedImage(width, height);

    if (premultipliedImage.size() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    env->GetByteArrayRegion(jpixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));

    auto iconImage = std::make_shared<mbgl::SpriteImage>(
        std::move(premultipliedImage),
        float(scale));

    nativeMapView->getMap().addAnnotationIcon(symbolName, iconImage);
}

void JNICALL nativeSetVisibleCoordinateBounds(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
        jobjectArray coordinates, jobject padding, jdouble direction, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetVisibleCoordinateBounds");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jfloat left = env->GetFloatField(padding, rectFLeftId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jfloat right = env->GetFloatField(padding, rectFRightId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jfloat top = env->GetFloatField(padding, rectFTopId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jfloat bottom = env->GetFloatField(padding, rectFBottomId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jsize count = env->GetArrayLength(coordinates);

    mbgl::EdgeInsets mbglInsets = {top, left, bottom, right};
    mbgl::AnnotationSegment segment;
    segment.reserve(count);

    for (int i = 0; i < count; i++) {
        jobject latLng = reinterpret_cast<jobject>(env->GetObjectArrayElement(coordinates, i));
        jdouble latitude = env->GetDoubleField(latLng, latLngLatitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return;
        }
        jdouble longitude = env->GetDoubleField(latLng, latLngLongitudeId);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return;
        }
        segment.push_back(mbgl::LatLng(latitude, longitude));
    }

    mbgl::CameraOptions cameraOptions = nativeMapView->getMap().cameraForLatLngs(segment, mbglInsets);
    if (direction >= 0) {
        // convert from degrees to radians
        cameraOptions.angle = (-direction * M_PI) / 180;
    }
    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
        animationOptions.duration.emplace(mbgl::Milliseconds(duration));
        // equivalent to kCAMediaTimingFunctionDefault in iOS
        animationOptions.easing = mbgl::util::UnitBezier(0.25, 0.1, 0.25, 0.1);
    }

    nativeMapView->getMap().easeTo(cameraOptions, animationOptions);
}

void JNICALL nativeOnLowMemory(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnLowMemory");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().onLowMemory();
}

void JNICALL nativeSetDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean debug) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    DebugOptions debugOptions = debug ? DebugOptions::TileBorders | DebugOptions::ParseStatus | DebugOptions::Collision
                                      : DebugOptions::NoDebug;
    nativeMapView->getMap().setDebug(debugOptions);
    nativeMapView->enableFps(debug);
}

void JNICALL nativeToggleDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeToggleDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().cycleDebugOptions();
    nativeMapView->enableFps(nativeMapView->getMap().getDebug() != DebugOptions::NoDebug);
}

jboolean JNICALL nativeGetDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getDebug() != DebugOptions::NoDebug;
}

jboolean JNICALL nativeIsFullyLoaded(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeIsFullyLoaded");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().isFullyLoaded();
}

void JNICALL nativeSetReachability(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean status) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetReachability");
    assert(nativeMapViewPtr != 0);
    if (status) {
        mbgl::NetworkStatus::Reachable();
    }
}

jdouble JNICALL nativeGetMetersPerPixelAtLatitude(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble lat, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMetersPerPixelAtLatitude");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMetersPerPixelAtLatitude(lat, zoom);
}

jobject JNICALL nativeProjectedMetersForLatLng(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject latLng) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeProjectedMetersForLatLng");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(latLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jdouble longitude = env->GetDoubleField(latLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    mbgl::ProjectedMeters projectedMeters = nativeMapView->getMap().projectedMetersForLatLng(mbgl::LatLng(latitude, longitude));

    jobject ret = env->NewObject(projectedMetersClass, projectedMetersConstructorId, projectedMeters.northing, projectedMeters.easting);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

jobject JNICALL nativeLatLngForProjectedMeters(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject projectedMeters) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeLatLngForProjectedMeters");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble northing = env->GetDoubleField(projectedMeters, projectedMetersNorthingId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jdouble easting = env->GetDoubleField(projectedMeters, projectedMetersEastingId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    mbgl::LatLng latLng = nativeMapView->getMap().latLngForProjectedMeters(mbgl::ProjectedMeters(northing, easting));

    jobject ret = env->NewObject(latLngClass, latLngConstructorId, latLng.latitude, latLng.longitude);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

jobject JNICALL nativePixelForLatLng(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject latLng) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativePixelForLatLng");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(latLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jdouble longitude = env->GetDoubleField(latLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    mbgl::ScreenCoordinate pixel = nativeMapView->getMap().pixelForLatLng(mbgl::LatLng(latitude, longitude));

    jobject ret = env->NewObject(pointFClass, pointFConstructorId, static_cast<jfloat>(pixel.x), static_cast<jfloat>(pixel.y));
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

jobject JNICALL nativeLatLngForPixel(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject pixel) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeLatLngForPixel");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jfloat x = env->GetFloatField(pixel, pointFXId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jfloat y = env->GetFloatField(pixel, pointFYId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return nullptr;
    }

    mbgl::LatLng latLng = nativeMapView->getMap().latLngForPixel(mbgl::ScreenCoordinate(x, y));

    jobject ret = env->NewObject(latLngClass, latLngConstructorId, latLng.latitude, latLng.longitude);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

jdouble JNICALL nativeGetTopOffsetPixelsForAnnotationSymbol(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring symbolName) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetTopOffsetPixelsForAnnotationSymbol");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getTopOffsetPixelsForAnnotationIcon(std_string_from_jstring(env, symbolName));
}

void JNICALL nativeJumpTo(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble angle, jobject centerLatLng, jdouble pitch, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeJumpTo");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(centerLatLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble longitude = env->GetDoubleField(centerLatLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    mbgl::CameraOptions options;
    if (angle != -1) {
        options.angle = angle;
    }
    options.center = mbgl::LatLng(latitude, longitude);
    options.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        options.pitch = pitch;
    }
    if (zoom != -1) {
        options.zoom = zoom;
    }

    nativeMapView->getMap().jumpTo(options);
}

void JNICALL nativeEaseTo(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble angle, jobject centerLatLng, jlong duration, jdouble pitch, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeEaseTo");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(centerLatLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble longitude = env->GetDoubleField(centerLatLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = angle;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        cameraOptions.pitch = pitch;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Duration(duration));

    nativeMapView->getMap().easeTo(cameraOptions, animationOptions);
}

void JNICALL nativeSetContentPadding(JNIEnv *env, jobject obj,long nativeMapViewPtr, double top, double left, double bottom, double right) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetContentPadding");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->setInsets({top, left, bottom, right});
}


void JNICALL nativeFlyTo(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble angle, jobject centerLatLng, jlong duration, jdouble pitch, jdouble zoom) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeFlyTo");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(centerLatLng, latLngLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble longitude = env->GetDoubleField(centerLatLng, latLngLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = angle;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        cameraOptions.pitch = pitch;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Duration(duration));

    nativeMapView->getMap().flyTo(cameraOptions, animationOptions);
}

void JNICALL nativeAddCustomLayer(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject customLayer, jstring before) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddCustomLayer");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().addCustomLayer(
        std_string_from_jstring(env, reinterpret_cast<jstring>(env->GetObjectField(customLayer, customLayerIdId))),
        reinterpret_cast<mbgl::CustomLayerInitializeFunction>(env->GetLongField(customLayer, customLayerInitializeFunctionId)),
        reinterpret_cast<mbgl::CustomLayerRenderFunction>(env->GetLongField(customLayer, customLayerRenderFunctionId)),
        reinterpret_cast<mbgl::CustomLayerDeinitializeFunction>(env->GetLongField(customLayer, customLayerDeinitializeFunctionId)),
        reinterpret_cast<void*>(env->GetLongField(customLayer, customLayerContextId)),
        before ? std_string_from_jstring(env, before).c_str() : nullptr);
}

void JNICALL nativeRemoveCustomLayer(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring id) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveCustomLayer");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeCustomLayer(std_string_from_jstring(env, id));
}

// Offline calls begin

jlong JNICALL createDefaultFileSource(JNIEnv *env, jobject obj, jstring cachePath_, jstring assetRoot_, jlong maximumCacheSize) {
    mbgl::Log::Debug(mbgl::Event::JNI, "createDefaultFileSource");
    std::string cachePath = std_string_from_jstring(env, cachePath_);
    std::string assetRoot = std_string_from_jstring(env, assetRoot_);
    mbgl::DefaultFileSource *defaultFileSource = new mbgl::DefaultFileSource(cachePath, assetRoot, maximumCacheSize);
    jlong defaultFileSourcePtr = reinterpret_cast<jlong>(defaultFileSource);
    return defaultFileSourcePtr;
}

void JNICALL setAccessToken(JNIEnv *env, jobject obj, jlong defaultFileSourcePtr, jstring accessToken_) {
    mbgl::Log::Debug(mbgl::Event::JNI, "setAccessToken");
    assert(defaultFileSourcePtr != 0);
    std::string accessToken = std_string_from_jstring(env, accessToken_);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->setAccessToken(accessToken);
}

jstring JNICALL getAccessToken(JNIEnv *env, jobject obj, jlong defaultFileSourcePtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "getAccessToken");
    assert(defaultFileSourcePtr != 0);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    std::string accessToken = defaultFileSource->getAccessToken();
    return std_string_to_jstring(env, accessToken);
}

void JNICALL listOfflineRegions(JNIEnv *env, jobject obj, jlong defaultFileSourcePtr, jobject listCallback) {
    mbgl::Log::Debug(mbgl::Event::JNI, "listOfflineRegions");

    // Checks
    assert(defaultFileSourcePtr != 0);
    if (listCallback == nullptr) {
        mbgl::Log::Error(mbgl::Event::JNI, "Callback has to be set.");
        return;
    }

    // Makes sure the objects don't get GC'ed
    obj = reinterpret_cast<jobject>(env->NewGlobalRef(obj));
    listCallback = reinterpret_cast<jobject>(env->NewGlobalRef(listCallback));

    // Launch listCallback
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->listOfflineRegions([obj, defaultFileSourcePtr, listCallback](std::exception_ptr error, mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            env2->CallVoidMethod(listCallback, listOnErrorMethodId, std_string_to_jstring(env2, message));
        } else if (regions) {
            // Build jobjectArray
            jsize index = 0;
            jobjectArray jregions = env2->NewObjectArray(regions->size(), offlineRegionClass, NULL);
            for (auto& region : *regions) {
                // Build the Region object
                jobject jregion = env2->NewObject(offlineRegionClass, offlineRegionConstructorId);
                env2->SetObjectField(jregion, offlineRegionOfflineManagerId, obj);
                env2->SetLongField(jregion, offlineRegionIdId, region.getID());

                // Definition object
                mbgl::OfflineTilePyramidRegionDefinition definition = region.getDefinition();
                jobject jdefinition = env2->NewObject(offlineRegionDefinitionClass, offlineRegionDefinitionConstructorId);
                env2->SetObjectField(jdefinition, offlineRegionDefinitionStyleURLId, std_string_to_jstring(env2, definition.styleURL));
                env2->SetObjectField(jdefinition, offlineRegionDefinitionBoundsId, latlngbounds_from_native(env2, definition.bounds));
                env2->SetDoubleField(jdefinition, offlineRegionDefinitionMinZoomId, definition.minZoom);
                env2->SetDoubleField(jdefinition, offlineRegionDefinitionMaxZoomId, definition.maxZoom);
                env2->SetFloatField(jdefinition, offlineRegionDefinitionPixelRatioId, definition.pixelRatio);
                env2->SetObjectField(jregion, offlineRegionDefinitionId, jdefinition);
                
                // Metadata object
                jbyteArray metadata = metadata_from_native(env2, region.getMetadata());
                env2->SetObjectField(jregion, offlineRegionMetadataId, metadata);

                // Moves the region on the stack into a heap-allocated one
                env2->SetLongField(jregion, offlineRegionPtrId,
                    reinterpret_cast<jlong>(new mbgl::OfflineRegion(std::move(region))));

                env2->SetObjectArrayElement(jregions, index, jregion);
                index++;
            }

            // Trigger callback
            env2->CallVoidMethod(listCallback, listOnListMethodId, jregions);
        }

        // Delete global refs and detach when we're done
        env2->DeleteGlobalRef(obj);
        env2->DeleteGlobalRef(listCallback);
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void JNICALL createOfflineRegion(JNIEnv *env, jobject obj, jlong defaultFileSourcePtr, jobject definition_, jbyteArray metadata_, jobject createCallback) {
    mbgl::Log::Debug(mbgl::Event::JNI, "createOfflineRegion");

    // Checks
    assert(defaultFileSourcePtr != 0);
    if (createCallback == nullptr) {
        mbgl::Log::Error(mbgl::Event::JNI, "Callback has to be set.");
        return;
    }

    // Definition fields
    jstring jStyleURL = reinterpret_cast<jstring>(env->GetObjectField(definition_, offlineRegionDefinitionStyleURLId));
    std::string styleURL = std_string_from_jstring(env, jStyleURL);
    jobject jBounds = env->GetObjectField(definition_, offlineRegionDefinitionBoundsId);
    jdouble jMinZoom = env->GetDoubleField(definition_, offlineRegionDefinitionMinZoomId);
    jdouble jMaxZoom = env->GetDoubleField(definition_, offlineRegionDefinitionMaxZoomId);
    jfloat jPixelRatio = env->GetFloatField(definition_, offlineRegionDefinitionPixelRatioId);

    // Convert bounds fields to native
    mbgl::LatLngBounds bounds = latlngbounds_from_java(env, jBounds);

    // Definition
    mbgl::OfflineTilePyramidRegionDefinition definition(styleURL, bounds, jMinZoom, jMaxZoom, jPixelRatio);

    // Metadata
    mbgl::OfflineRegionMetadata metadata;
    if (metadata_ != nullptr) {
        metadata = metadata_from_java(env, metadata_);
    }

    // Makes sure the objects don't get GC'ed
    obj = reinterpret_cast<jobject>(env->NewGlobalRef(obj));
    createCallback = reinterpret_cast<jobject>(env->NewGlobalRef(createCallback));

    // Launch createCallback
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->createOfflineRegion(definition, metadata, [obj, defaultFileSourcePtr, createCallback] (std::exception_ptr error, mbgl::optional<mbgl::OfflineRegion> region) {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            env2->CallVoidMethod(createCallback, createOnErrorMethodId, std_string_to_jstring(env2, message));
        } else if (region) {
            // Build the Region object
            jobject jregion = env2->NewObject(offlineRegionClass, offlineRegionConstructorId);
            env2->SetObjectField(jregion, offlineRegionOfflineManagerId, obj);
            env2->SetLongField(jregion, offlineRegionIdId, region->getID());

            // Metadata object
            jbyteArray jmetadata = metadata_from_native(env2, region->getMetadata());
            env2->SetObjectField(jregion, offlineRegionMetadataId, jmetadata);

            // Moves the region on the stack into a heap-allocated one
            env2->SetLongField(jregion, offlineRegionPtrId,
                reinterpret_cast<jlong>(new mbgl::OfflineRegion(std::move(*region))));

            // Invoke Java callback
            env2->CallVoidMethod(createCallback, createOnCreateMethodId, jregion);
        }

        // Delete global refs and detach when we're done
        env2->DeleteGlobalRef(obj);
        env2->DeleteGlobalRef(createCallback);
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void JNICALL setOfflineMapboxTileCountLimit(JNIEnv *env, jobject obj, jlong defaultFileSourcePtr, jlong limit) {
    mbgl::Log::Debug(mbgl::Event::JNI, "setOfflineMapboxTileCountLimit");

    // Checks
    assert(defaultFileSourcePtr != 0);
    assert(limit > 0);

    // Set limit
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->setOfflineMapboxTileCountLimit(limit);
}

void JNICALL destroyOfflineRegion(JNIEnv *env, jobject obj, jlong offlineRegionPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "destroyOfflineRegion");
    assert(offlineRegionPtr != 0);
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);
    delete offlineRegion;
    offlineRegion = nullptr;
}

void JNICALL setOfflineRegionObserver(JNIEnv *env, jobject obj, jobject offlineRegion_, jobject observerCallback) {
    mbgl::Log::Debug(mbgl::Event::JNI, "setOfflineRegionObserver");

    // Offline region
    jlong offlineRegionPtr = env->GetLongField(offlineRegion_, offlineRegionPtrId);
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);

    // File source
    jobject jmanager = env->GetObjectField(offlineRegion_, offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = env->GetLongField(jmanager, offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Define the observer
    class Observer : public mbgl::OfflineRegionObserver {
    public:
        Observer(jobject observerCallback_)
            : observerCallback(observerCallback_) {
        }

        void statusChanged(mbgl::OfflineRegionStatus status) override {
            // Env
            JNIEnv* env2;
            jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");

            // Conver to jint
            jint downloadState;
            switch(status.downloadState) {
                case mbgl::OfflineRegionDownloadState::Inactive:
                    downloadState = 0;
                    break;
                case mbgl::OfflineRegionDownloadState::Active:
                    downloadState = 1;
                    break;
            }

            // Stats object
            jobject jstatus = env2->NewObject(offlineRegionStatusClass, offlineRegionStatusConstructorId);
            env2->SetIntField(jstatus, offlineRegionStatusDownloadStateId, downloadState);
            env2->SetLongField(jstatus, offlineRegionStatusCompletedResourceCountId, status.completedResourceCount);
            env2->SetLongField(jstatus, offlineRegionStatusCompletedResourceSizeId, status.completedResourceSize);
            env2->SetLongField(jstatus, offlineRegionStatusRequiredResourceCountId, status.requiredResourceCount);
            env2->SetBooleanField(jstatus, offlineRegionStatusRequiredResourceCountIsPreciseId, status.requiredResourceCountIsPrecise);
            env2->CallVoidMethod(observerCallback, offlineRegionObserveronStatusChangedId, jstatus);

            // Detach when we're done
            detach_jni_thread(theJVM, &env2, renderDetach);
        }

        void responseError(mbgl::Response::Error error) override {            
            // Env
            JNIEnv* env2;
            jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");

            // Handle the value of reason independently of the underlying int value
            std::string errorReason;
            switch(error.reason) {
                case mbgl::Response::Error::Reason::Success:
                    errorReason = "REASON_SUCCESS";
                    break;
                case mbgl::Response::Error::Reason::NotFound:
                    errorReason = "REASON_NOT_FOUND";
                    break;
                case mbgl::Response::Error::Reason::Server:
                    errorReason = "REASON_SERVER";
                    break;
                case mbgl::Response::Error::Reason::Connection:
                    errorReason = "REASON_CONNECTION";
                    break;
                case mbgl::Response::Error::Reason::Other:
                    errorReason = "REASON_OTHER";
                    break;
            }

            // Error object
            jobject jerror = env2->NewObject(offlineRegionErrorClass, offlineRegionErrorConstructorId);
            env2->SetObjectField(jerror, offlineRegionErrorReasonId, std_string_to_jstring(env2, errorReason));
            env2->SetObjectField(jerror, offlineRegionErrorMessageId, std_string_to_jstring(env2, error.message));
            env2->CallVoidMethod(observerCallback, offlineRegionObserveronErrorId, jerror);

            // Detach when we're done
            detach_jni_thread(theJVM, &env2, renderDetach);
        }

        void mapboxTileCountLimitExceeded(uint64_t limit) override {
            // Env
            JNIEnv* env2;
            jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");

            // Send limit
            env2->CallVoidMethod(observerCallback, offlineRegionObserveronLimitId, limit);

            // Detach when we're done
            detach_jni_thread(theJVM, &env2, renderDetach);            
        }

        jobject observerCallback;
    };

    // Makes sure the callback doesn't get GC'ed
    observerCallback = reinterpret_cast<jobject>(env->NewGlobalRef(observerCallback));

    // Set the observer
    defaultFileSource->setOfflineRegionObserver(*offlineRegion, std::make_unique<Observer>(observerCallback));
}

void JNICALL setOfflineRegionDownloadState(JNIEnv *env, jobject obj, jobject offlineRegion_, jint offlineRegionDownloadState) {
    mbgl::Log::Debug(mbgl::Event::JNI, "setOfflineRegionDownloadState");

    // State
    mbgl::OfflineRegionDownloadState state;
    if (offlineRegionDownloadState == 0) {
        state = mbgl::OfflineRegionDownloadState::Inactive;
    } else if (offlineRegionDownloadState == 1) {
        state = mbgl::OfflineRegionDownloadState::Active;
    } else {
        mbgl::Log::Error(mbgl::Event::JNI, "State can only be 0 (inactive) or 1 (active).");
        return;
    }

    // Offline region
    jlong offlineRegionPtr = env->GetLongField(offlineRegion_, offlineRegionPtrId);
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);

    // File source
    jobject jmanager = env->GetObjectField(offlineRegion_, offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = env->GetLongField(jmanager, offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Set new state
    defaultFileSource->setOfflineRegionDownloadState(*offlineRegion, state);
}

void JNICALL getOfflineRegionStatus(JNIEnv *env, jobject obj, jobject offlineRegion_, jobject statusCallback) {
    mbgl::Log::Debug(mbgl::Event::JNI, "getOfflineRegionStatus");

    // Offline region
    jlong offlineRegionPtr = env->GetLongField(offlineRegion_, offlineRegionPtrId);
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);

    // File source
    jobject jmanager = env->GetObjectField(offlineRegion_, offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = env->GetLongField(jmanager, offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Makes sure the callback doesn't get GC'ed
    statusCallback = reinterpret_cast<jobject>(env->NewGlobalRef(statusCallback));

    // Set new state
    defaultFileSource->getOfflineRegionStatus(*offlineRegion, [statusCallback](std::exception_ptr error, mbgl::optional<mbgl::OfflineRegionStatus> status) {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            env2->CallVoidMethod(statusCallback, offlineRegionStatusOnErrorId, std_string_to_jstring(env2, message));
        } else if (status) {
            // Conver to jint
            jint downloadState = -1;
            if (status->downloadState == mbgl::OfflineRegionDownloadState::Inactive) {
                downloadState = 0;
            } else if (status->downloadState == mbgl::OfflineRegionDownloadState::Active) {
                downloadState = 1;
            } else {
                mbgl::Log::Error(mbgl::Event::JNI, "Unsupported OfflineRegionDownloadState value.");
                return;
            }

            // Stats object
            jobject jstatus = env2->NewObject(offlineRegionStatusClass, offlineRegionStatusConstructorId);
            env2->SetIntField(jstatus, offlineRegionStatusDownloadStateId, downloadState);
            env2->SetLongField(jstatus, offlineRegionStatusCompletedResourceCountId, status->completedResourceCount);
            env2->SetLongField(jstatus, offlineRegionStatusCompletedResourceSizeId, status->completedResourceSize);
            env2->SetLongField(jstatus, offlineRegionStatusRequiredResourceCountId, status->requiredResourceCount);
            env2->SetBooleanField(jstatus, offlineRegionStatusRequiredResourceCountIsPreciseId, status->requiredResourceCountIsPrecise);
            env2->CallVoidMethod(statusCallback, offlineRegionStatusOnStatusId, jstatus);
        }

        // Delete global refs and detach when we're done
        env2->DeleteGlobalRef(statusCallback);
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void JNICALL deleteOfflineRegion(JNIEnv *env, jobject obj, jobject offlineRegion_, jobject deleteCallback) {
    mbgl::Log::Debug(mbgl::Event::JNI, "deleteOfflineRegion");

    // Offline region
    jlong offlineRegionPtr = env->GetLongField(offlineRegion_, offlineRegionPtrId);
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);

    // File source
    jobject jmanager = env->GetObjectField(offlineRegion_, offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = env->GetLongField(jmanager, offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Makes sure the callback doesn't get GC'ed
    deleteCallback = reinterpret_cast<jobject>(env->NewGlobalRef(deleteCallback));

    // Set new state
    defaultFileSource->deleteOfflineRegion(std::move(*offlineRegion), [deleteCallback](std::exception_ptr error) {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            env2->CallVoidMethod(deleteCallback, offlineRegionDeleteOnErrorId, std_string_to_jstring(env2, message));
        } else {
            std::string message = mbgl::util::toString(error);
            env2->CallVoidMethod(deleteCallback, offlineRegionDeleteOnDeleteId);
        }

        // Delete global refs and detach when we're done
        env2->DeleteGlobalRef(deleteCallback);
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

// Offline calls end

}

extern "C" {

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    // This reads out values from objects
    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnLoad");

    theJVM = vm;

    JNIEnv *env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        return JNI_ERR;
    }

    mbgl::android::RegisterNativeHTTPRequest(*env);

    latLngClass = env->FindClass("com/mapbox/mapboxsdk/geometry/LatLng");
    if (latLngClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngConstructorId = env->GetMethodID(latLngClass, "<init>", "(DD)V");
    if (latLngConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngLatitudeId = env->GetFieldID(latLngClass, "latitude", "D");
    if (latLngLatitudeId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngLongitudeId = env->GetFieldID(latLngClass, "longitude", "D");
    if (latLngLongitudeId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsClass = env->FindClass("com/mapbox/mapboxsdk/geometry/LatLngBounds");
    if (latLngBoundsClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsConstructorId = env->GetMethodID(latLngBoundsClass, "<init>", "(DDDD)V");
    if (latLngBoundsConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsLatNorthId = env->GetFieldID(latLngBoundsClass, "mLatNorth", "D");
    if (latLngBoundsLatNorthId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsLatSouthId = env->GetFieldID(latLngBoundsClass, "mLatSouth", "D");
    if (latLngBoundsLatSouthId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsLonEastId = env->GetFieldID(latLngBoundsClass, "mLonEast", "D");
    if (latLngBoundsLonEastId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsLonWestId = env->GetFieldID(latLngBoundsClass, "mLonWest", "D");
    if (latLngBoundsLonWestId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    iconClass = env->FindClass("com/mapbox/mapboxsdk/annotations/Icon");
    if (iconClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    iconIdId = env->GetFieldID(iconClass, "mId", "Ljava/lang/String;");
    if (iconIdId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerClass = env->FindClass("com/mapbox/mapboxsdk/annotations/Marker");
    if (markerClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerPositionId = env->GetFieldID(markerClass, "position", "Lcom/mapbox/mapboxsdk/geometry/LatLng;");
    if (markerPositionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerIconId = env->GetFieldID(markerClass, "icon", "Lcom/mapbox/mapboxsdk/annotations/Icon;");
    if (markerIconId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerIdId = env->GetFieldID(markerClass, "id", "J");
    if (markerIdId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineClass = env->FindClass("com/mapbox/mapboxsdk/annotations/Polyline");
    if (polylineClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineAlphaId = env->GetFieldID(polylineClass, "alpha", "F");
    if (polylineAlphaId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineColorId = env->GetFieldID(polylineClass, "color", "I");
    if (polylineColorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineWidthId = env->GetFieldID(polylineClass, "width", "F");
    if (polylineWidthId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylinePointsId = env->GetFieldID(polylineClass, "points", "Ljava/util/List;");
    if (polylineWidthId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonClass = env->FindClass("com/mapbox/mapboxsdk/annotations/Polygon");
    if (polygonClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonAlphaId = env->GetFieldID(polygonClass, "alpha", "F");
    if (polygonAlphaId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonFillColorId = env->GetFieldID(polygonClass, "fillColor", "I");
    if (polygonFillColorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonStrokeColorId = env->GetFieldID(polygonClass, "strokeColor", "I");
    if (polygonStrokeColorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonPointsId = env->GetFieldID(polygonClass, "points", "Ljava/util/List;");
    if (polygonPointsId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass nativeMapViewClass = env->FindClass("com/mapbox/mapboxsdk/maps/NativeMapView");
    if (nativeMapViewClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    onInvalidateId = env->GetMethodID(nativeMapViewClass, "onInvalidate", "()V");
    if (onInvalidateId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    onMapChangedId = env->GetMethodID(nativeMapViewClass, "onMapChanged", "(I)V");
    if (onMapChangedId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    onFpsChangedId = env->GetMethodID(nativeMapViewClass, "onFpsChanged", "(D)V");
    if (onFpsChangedId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    runtimeExceptionClass = env->FindClass("java/lang/RuntimeException");
    if (runtimeExceptionClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    nullPointerExceptionClass = env->FindClass("java/lang/NullPointerException");
    if (nullPointerExceptionClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass listClass = env->FindClass("java/util/List");
    if (listClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    listToArrayId = env->GetMethodID(listClass, "toArray", "()[Ljava/lang/Object;");
    if (listToArrayId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    arrayListClass = env->FindClass("java/util/ArrayList");
    if (arrayListClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    arrayListConstructorId = env->GetMethodID(arrayListClass, "<init>", "()V");
    if (arrayListConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    arrayListAddId = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
    if (arrayListAddId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    projectedMetersClass = env->FindClass("com/mapbox/mapboxsdk/geometry/ProjectedMeters");
    if (projectedMetersClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    projectedMetersConstructorId = env->GetMethodID(projectedMetersClass, "<init>", "(DD)V");
    if (projectedMetersConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    projectedMetersNorthingId = env->GetFieldID(projectedMetersClass, "northing", "D");
    if (projectedMetersNorthingId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    projectedMetersEastingId = env->GetFieldID(projectedMetersClass, "easting", "D");
    if (projectedMetersEastingId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    pointFClass = env->FindClass("android/graphics/PointF");
    if (pointFClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    pointFConstructorId = env->GetMethodID(pointFClass, "<init>", "(FF)V");
    if (pointFConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    pointFXId = env->GetFieldID(pointFClass, "x", "F");
    if (pointFXId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    pointFYId = env->GetFieldID(pointFClass, "y", "F");
    if (pointFYId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFClass = env->FindClass("android/graphics/RectF");
    if (rectFClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFConstructorId = env->GetMethodID(rectFClass, "<init>", "()V");
    if (rectFConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFLeftId = env->GetFieldID(rectFClass, "left", "F");
    if (rectFLeftId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFRightId = env->GetFieldID(rectFClass, "right", "F");
    if (rectFRightId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFTopId = env->GetFieldID(rectFClass, "top", "F");
    if (rectFTopId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    rectFBottomId = env->GetFieldID(rectFClass, "bottom", "F");
    if (rectFBottomId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerClass = env->FindClass("com/mapbox/mapboxsdk/layers/CustomLayer");
    if (customLayerClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerIdId = env->GetFieldID(customLayerClass, "mID", "Ljava/lang/String;");
    if (customLayerIdId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerContextId = env->GetFieldID(customLayerClass, "mContext", "J");
    if (customLayerContextId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerInitializeFunctionId = env->GetFieldID(customLayerClass, "mInitializeFunction", "J");
    if (customLayerInitializeFunctionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerRenderFunctionId = env->GetFieldID(customLayerClass, "mRenderFunction", "J");
    if (customLayerRenderFunctionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    customLayerDeinitializeFunctionId = env->GetFieldID(customLayerClass, "mDeinitializeFunction", "J");
    if (customLayerDeinitializeFunctionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // Offline definitions begin

    offlineManagerClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineManager");
    if (offlineManagerClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineManagerClassPtrId = env->GetFieldID(offlineManagerClass, "mDefaultFileSourcePtr", "J");
    if (offlineManagerClassPtrId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    listOfflineRegionsCallbackClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineManager$ListOfflineRegionsCallback");
    if (listOfflineRegionsCallbackClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    listOnListMethodId = env->GetMethodID(listOfflineRegionsCallbackClass, "onList", "([Lcom/mapbox/mapboxsdk/offline/OfflineRegion;)V");
    if (listOnListMethodId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    listOnErrorMethodId = env->GetMethodID(listOfflineRegionsCallbackClass, "onError", "(Ljava/lang/String;)V");
    if (listOnErrorMethodId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegion");
    if (offlineRegionClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionConstructorId = env->GetMethodID(offlineRegionClass, "<init>", "()V");
    if (offlineRegionConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionOfflineManagerId = env->GetFieldID(offlineRegionClass, "offlineManager", "Lcom/mapbox/mapboxsdk/offline/OfflineManager;");
    if (offlineRegionOfflineManagerId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionIdId = env->GetFieldID(offlineRegionClass, "mId", "J");
    if (offlineRegionIdId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionId = env->GetFieldID(offlineRegionClass, "mDefinition", "Lcom/mapbox/mapboxsdk/offline/OfflineRegionDefinition;");
    if (offlineRegionDefinitionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionMetadataId = env->GetFieldID(offlineRegionClass, "mMetadata", "[B");
    if (offlineRegionMetadataId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionPtrId = env->GetFieldID(offlineRegionClass, "mOfflineRegionPtr", "J");
    if (offlineRegionPtrId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // This needs to be updated once we support more than one type of region definition
    offlineRegionDefinitionClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineTilePyramidRegionDefinition");
    if (offlineRegionDefinitionClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionConstructorId = env->GetMethodID(offlineRegionDefinitionClass, "<init>", "()V");
    if (offlineRegionDefinitionConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionStyleURLId = env->GetFieldID(offlineRegionDefinitionClass, "styleURL", "Ljava/lang/String;");
    if (offlineRegionDefinitionStyleURLId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionBoundsId = env->GetFieldID(offlineRegionDefinitionClass, "bounds", "Lcom/mapbox/mapboxsdk/geometry/LatLngBounds;");
    if (offlineRegionDefinitionBoundsId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionMinZoomId = env->GetFieldID(offlineRegionDefinitionClass, "minZoom", "D");
    if (offlineRegionDefinitionMinZoomId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionMaxZoomId = env->GetFieldID(offlineRegionDefinitionClass, "maxZoom", "D");
    if (offlineRegionDefinitionMaxZoomId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDefinitionPixelRatioId = env->GetFieldID(offlineRegionDefinitionClass, "pixelRatio", "F");
    if (offlineRegionDefinitionPixelRatioId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    createOfflineRegionCallbackClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineManager$CreateOfflineRegionCallback");
    if (createOfflineRegionCallbackClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    createOnCreateMethodId = env->GetMethodID(createOfflineRegionCallbackClass, "onCreate", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;)V");
    if (createOnCreateMethodId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    createOnErrorMethodId = env->GetMethodID(createOfflineRegionCallbackClass, "onError", "(Ljava/lang/String;)V");
    if (createOnErrorMethodId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionObserverClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionObserver");
    if (offlineRegionObserverClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionObserveronStatusChangedId = env->GetMethodID(offlineRegionObserverClass, "onStatusChanged", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionStatus;)V");
    if (offlineRegionObserveronStatusChangedId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionObserveronErrorId = env->GetMethodID(offlineRegionObserverClass, "onError", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionError;)V");
    if (offlineRegionObserveronErrorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionObserveronLimitId = env->GetMethodID(offlineRegionObserverClass, "mapboxTileCountLimitExceeded", "(J)V");
    if (offlineRegionObserveronLimitId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegionStatus");
    if (offlineRegionStatusClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusConstructorId = env->GetMethodID(offlineRegionStatusClass, "<init>", "()V");
    if (offlineRegionStatusConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusDownloadStateId = env->GetFieldID(offlineRegionStatusClass, "downloadState", "I");
    if (offlineRegionStatusDownloadStateId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusCompletedResourceCountId = env->GetFieldID(offlineRegionStatusClass, "completedResourceCount", "J");
    if (offlineRegionStatusCompletedResourceCountId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusCompletedResourceSizeId = env->GetFieldID(offlineRegionStatusClass, "completedResourceSize", "J");
    if (offlineRegionStatusCompletedResourceSizeId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusRequiredResourceCountId = env->GetFieldID(offlineRegionStatusClass, "requiredResourceCount", "J");
    if (offlineRegionStatusRequiredResourceCountId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusRequiredResourceCountIsPreciseId = env->GetFieldID(offlineRegionStatusClass, "requiredResourceCountIsPrecise", "Z");
    if (offlineRegionStatusRequiredResourceCountIsPreciseId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionErrorClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegionError");
    if (offlineRegionErrorClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionErrorConstructorId = env->GetMethodID(offlineRegionErrorClass, "<init>", "()V");
    if (offlineRegionErrorConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionErrorReasonId = env->GetFieldID(offlineRegionErrorClass, "reason", "Ljava/lang/String;");
    if (offlineRegionErrorReasonId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionErrorMessageId = env->GetFieldID(offlineRegionErrorClass, "message", "Ljava/lang/String;");
    if (offlineRegionErrorMessageId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusCallbackClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback");
    if (offlineRegionStatusCallbackClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusOnStatusId = env->GetMethodID(offlineRegionStatusCallbackClass, "onStatus", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionStatus;)V");
    if (offlineRegionStatusOnStatusId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionStatusOnErrorId = env->GetMethodID(offlineRegionStatusCallbackClass, "onError", "(Ljava/lang/String;)V");
    if (offlineRegionStatusOnErrorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDeleteCallbackClass = env->FindClass("com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback");
    if (offlineRegionDeleteCallbackClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDeleteOnDeleteId = env->GetMethodID(offlineRegionDeleteCallbackClass, "onDelete", "()V");
    if (offlineRegionDeleteOnDeleteId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    offlineRegionDeleteOnErrorId = env->GetMethodID(offlineRegionDeleteCallbackClass, "onError", "(Ljava/lang/String;)V");
    if (offlineRegionDeleteOnErrorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // Offline definitions end

    const std::vector<JNINativeMethod> methods = {
        {"nativeCreate", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;FIJ)J",
         reinterpret_cast<void *>(&nativeCreate)},
        {"nativeDestroy", "(J)V", reinterpret_cast<void *>(&nativeDestroy)},
        {"nativeInitializeDisplay", "(J)V", reinterpret_cast<void *>(&nativeInitializeDisplay)},
        {"nativeTerminateDisplay", "(J)V", reinterpret_cast<void *>(&nativeTerminateDisplay)},
        {"nativeInitializeContext", "(J)V", reinterpret_cast<void *>(&nativeInitializeContext)},
        {"nativeTerminateContext", "(J)V", reinterpret_cast<void *>(&nativeTerminateContext)},
        {"nativeCreateSurface", "(JLandroid/view/Surface;)V",
         reinterpret_cast<void *>(&nativeCreateSurface)},
        {"nativeDestroySurface", "(J)V", reinterpret_cast<void *>(&nativeDestroySurface)},
        {"nativePause", "(J)V", reinterpret_cast<void *>(&nativePause)},
        {"nativeIsPaused", "(J)Z", reinterpret_cast<void *>(&nativeIsPaused)},
        {"nativeResume", "(J)V", reinterpret_cast<void *>(&nativeResume)},
        {"nativeUpdate", "(J)V", reinterpret_cast<void *>(&nativeUpdate)},
        {"nativeRenderSync", "(J)V", reinterpret_cast<void *>(&nativeRenderSync)},
        {"nativeViewResize", "(JII)V",
         reinterpret_cast<void *>(static_cast<void JNICALL (
             *)(JNIEnv *, jobject, jlong, jint, jint)>(&nativeViewResize))},
        {"nativeFramebufferResize", "(JII)V",
         reinterpret_cast<void *>(static_cast<void JNICALL (
             *)(JNIEnv *, jobject, jlong, jint, jint)>(&nativeFramebufferResize))},
        {"nativeAddClass", "(JLjava/lang/String;)V",
         reinterpret_cast<void *>(&nativeAddClass)},
        {"nativeRemoveClass", "(JLjava/lang/String;)V",
         reinterpret_cast<void *>(&nativeRemoveClass)},
        {"nativeHasClass", "(JLjava/lang/String;)Z",
         reinterpret_cast<void *>(&nativeHasClass)},
        {"nativeSetClasses", "(JLjava/util/List;)V",
         reinterpret_cast<void *>(&nativeSetClasses)},
        {"nativeGetClasses", "(J)Ljava/util/List;",
         reinterpret_cast<void *>(&nativeGetClasses)},
        {"nativeSetDefaultTransitionDuration", "(JJ)V",
         reinterpret_cast<void *>(&nativeSetDefaultTransitionDuration)},
        {"nativeGetDefaultTransitionDuration", "(J)J",
         reinterpret_cast<void *>(&nativeGetDefaultTransitionDuration)},
        {"nativeSetStyleUrl", "(JLjava/lang/String;)V",
         reinterpret_cast<void *>(&nativeSetStyleURL)},
        {"nativeSetStyleJson", "(JLjava/lang/String;Ljava/lang/String;)V",
         reinterpret_cast<void *>(&nativeSetStyleJSON)},
        {"nativeGetStyleJson", "(J)Ljava/lang/String;",
         reinterpret_cast<void *>(&nativeGetStyleJSON)},
        {"nativeSetAccessToken", "(JLjava/lang/String;)V",
         reinterpret_cast<void *>(&nativeSetAccessToken)},
        {"nativeGetAccessToken", "(J)Ljava/lang/String;",
         reinterpret_cast<void *>(&nativeGetAccessToken)},
        {"nativeCancelTransitions", "(J)V", reinterpret_cast<void *>(&nativeCancelTransitions)},
        {"nativeSetGestureInProgress", "(JZ)V", reinterpret_cast<void *>(&nativeSetGestureInProgress)},
        {"nativeMoveBy", "(JDDJ)V", reinterpret_cast<void *>(&nativeMoveBy)},
        {"nativeSetLatLng", "(JLcom/mapbox/mapboxsdk/geometry/LatLng;J)V",
         reinterpret_cast<void *>(&nativeSetLatLng)},
        {"nativeGetLatLng", "(J)Lcom/mapbox/mapboxsdk/geometry/LatLng;",
         reinterpret_cast<void *>(&nativeGetLatLng)},
        {"nativeResetPosition", "(J)V", reinterpret_cast<void *>(&nativeResetPosition)},
        {"nativeGetCameraValues","(J)[D", reinterpret_cast<void *>(&nativeGetCameraValues)},
        {"nativeGetPitch", "(J)D", reinterpret_cast<void *>(&nativeGetPitch)},
        {"nativeSetPitch", "(JDJ)V", reinterpret_cast<void *>(&nativeSetPitch)},
        {"nativeScaleBy", "(JDDDJ)V", reinterpret_cast<void *>(&nativeScaleBy)},
        {"nativeSetScale", "(JDDDJ)V", reinterpret_cast<void *>(&nativeSetScale)},
        {"nativeGetScale", "(J)D", reinterpret_cast<void *>(&nativeGetScale)},
        {"nativeSetZoom", "(JDJ)V", reinterpret_cast<void *>(&nativeSetZoom)},
        {"nativeGetZoom", "(J)D", reinterpret_cast<void *>(&nativeGetZoom)},
        {"nativeResetZoom", "(J)V", reinterpret_cast<void *>(&nativeResetZoom)},
        {"nativeGetMinZoom", "(J)D", reinterpret_cast<void *>(&nativeGetMinZoom)},
        {"nativeSetMinZoom", "(JD)V", reinterpret_cast<void *>(&nativeSetMinZoom)},
        {"nativeGetMaxZoom", "(J)D", reinterpret_cast<void *>(&nativeGetMaxZoom)},
        {"nativeSetMaxZoom", "(JD)V", reinterpret_cast<void *>(&nativeSetMaxZoom)},
        {"nativeRotateBy", "(JDDDDJ)V", reinterpret_cast<void *>(&nativeRotateBy)},
        {"nativeSetBearing", "(JDJ)V",
         reinterpret_cast<void *>(
             static_cast<void JNICALL (*)(JNIEnv *, jobject, jlong, jdouble, jlong)>(
                 &nativeSetBearing))},
        {"nativeSetBearing", "(JDDD)V",
         reinterpret_cast<void *>(
             static_cast<void JNICALL (*)(JNIEnv *, jobject, jlong, jdouble, jdouble, jdouble)>(
                 &nativeSetBearing))},
        {"nativeGetBearing", "(J)D", reinterpret_cast<void *>(&nativeGetBearing)},
        {"nativeResetNorth", "(J)V", reinterpret_cast<void *>(&nativeResetNorth)},
        {"nativeAddMarker", "(JLcom/mapbox/mapboxsdk/annotations/Marker;)J",
         reinterpret_cast<void *>(&nativeAddMarker)},
         {"nativeAddMarkers", "(JLjava/util/List;)[J",
         reinterpret_cast<void *>(&nativeAddMarkers)},
        {"nativeAddPolyline", "(JLcom/mapbox/mapboxsdk/annotations/Polyline;)J",
         reinterpret_cast<void *>(&nativeAddPolyline)},
         {"nativeAddPolylines", "(JLjava/util/List;)[J",
         reinterpret_cast<void *>(&nativeAddPolylines)},
        {"nativeAddPolygon", "(JLcom/mapbox/mapboxsdk/annotations/Polygon;)J",
         reinterpret_cast<void *>(&nativeAddPolygon)},
        {"nativeAddPolygons", "(JLjava/util/List;)[J",
         reinterpret_cast<void *>(&nativeAddPolygons)},
        {"nativeUpdateMarker", "(JLcom/mapbox/mapboxsdk/annotations/Marker;)V",
        reinterpret_cast<void *>(&nativeUpdateMarker)} ,
        {"nativeRemoveAnnotation", "(JJ)V", reinterpret_cast<void *>(&nativeRemoveAnnotation)},
        {"nativeRemoveAnnotations", "(J[J)V", reinterpret_cast<void *>(&nativeRemoveAnnotations)},
        {"nativeGetAnnotationsInBounds", "(JLcom/mapbox/mapboxsdk/geometry/LatLngBounds;)[J",
         reinterpret_cast<void *>(&nativeGetAnnotationsInBounds)},
        {"nativeAddAnnotationIcon", "(JLjava/lang/String;IIF[B)V", reinterpret_cast<void *>(&nativeAddAnnotationIcon)},
        {"nativeSetVisibleCoordinateBounds", "(J[Lcom/mapbox/mapboxsdk/geometry/LatLng;Landroid/graphics/RectF;DJ)V",
                reinterpret_cast<void *>(&nativeSetVisibleCoordinateBounds)},
        {"nativeOnLowMemory", "(J)V", reinterpret_cast<void *>(&nativeOnLowMemory)},
        {"nativeSetDebug", "(JZ)V", reinterpret_cast<void *>(&nativeSetDebug)},
        {"nativeToggleDebug", "(J)V", reinterpret_cast<void *>(&nativeToggleDebug)},
        {"nativeGetDebug", "(J)Z", reinterpret_cast<void *>(&nativeGetDebug)},
        {"nativeIsFullyLoaded", "(J)Z", reinterpret_cast<void *>(&nativeIsFullyLoaded)},
        {"nativeSetReachability", "(JZ)V", reinterpret_cast<void *>(&nativeSetReachability)},
        {"nativeGetMetersPerPixelAtLatitude", "(JDD)D", reinterpret_cast<void *>(&nativeGetMetersPerPixelAtLatitude)},
        {"nativeProjectedMetersForLatLng",
         "(JLcom/mapbox/mapboxsdk/geometry/LatLng;)Lcom/mapbox/mapboxsdk/geometry/ProjectedMeters;",
         reinterpret_cast<void *>(&nativeProjectedMetersForLatLng)},
        {"nativeLatLngForProjectedMeters",
         "(JLcom/mapbox/mapboxsdk/geometry/ProjectedMeters;)Lcom/mapbox/mapboxsdk/geometry/LatLng;",
         reinterpret_cast<void *>(&nativeLatLngForProjectedMeters)},
        {"nativePixelForLatLng", "(JLcom/mapbox/mapboxsdk/geometry/LatLng;)Landroid/graphics/PointF;",
         reinterpret_cast<void *>(&nativePixelForLatLng)},
        {"nativeLatLngForPixel", "(JLandroid/graphics/PointF;)Lcom/mapbox/mapboxsdk/geometry/LatLng;",
         reinterpret_cast<void *>(&nativeLatLngForPixel)},
        {"nativeGetTopOffsetPixelsForAnnotationSymbol", "(JLjava/lang/String;)D",
         reinterpret_cast<void *>(&nativeGetTopOffsetPixelsForAnnotationSymbol)},
        {"nativeJumpTo", "(JDLcom/mapbox/mapboxsdk/geometry/LatLng;DD)V",
         reinterpret_cast<void *>(&nativeJumpTo)},
        {"nativeEaseTo", "(JDLcom/mapbox/mapboxsdk/geometry/LatLng;JDD)V",
         reinterpret_cast<void *>(&nativeEaseTo)},
        {"nativeFlyTo", "(JDLcom/mapbox/mapboxsdk/geometry/LatLng;JDD)V",
         reinterpret_cast<void *>(&nativeFlyTo)},
        {"nativeAddCustomLayer", "(JLcom/mapbox/mapboxsdk/layers/CustomLayer;Ljava/lang/String;)V",
         reinterpret_cast<void *>(&nativeAddCustomLayer)},
        {"nativeRemoveCustomLayer", "(JLjava/lang/String;)V",
         reinterpret_cast<void *>(&nativeRemoveCustomLayer)},
        {"nativeSetContentPadding", "(JDDDD)V",
         reinterpret_cast<void *>(&nativeSetContentPadding)}
    };

    if (env->RegisterNatives(nativeMapViewClass, methods.data(), methods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // Offline register begin

    const std::vector<JNINativeMethod> offlineManagerMethods = {
        {"createDefaultFileSource", "(Ljava/lang/String;Ljava/lang/String;J)J", reinterpret_cast<void *>(&createDefaultFileSource)},
        {"setAccessToken", "(JLjava/lang/String;)V", reinterpret_cast<void *>(&setAccessToken)},
        {"getAccessToken", "(J)Ljava/lang/String;", reinterpret_cast<void *>(&getAccessToken)},
        {"listOfflineRegions", "(JLcom/mapbox/mapboxsdk/offline/OfflineManager$ListOfflineRegionsCallback;)V", reinterpret_cast<void *>(&listOfflineRegions)},
        {"createOfflineRegion", "(JLcom/mapbox/mapboxsdk/offline/OfflineRegionDefinition;[BLcom/mapbox/mapboxsdk/offline/OfflineManager$CreateOfflineRegionCallback;)V", reinterpret_cast<void *>(&createOfflineRegion)},
        {"setOfflineMapboxTileCountLimit", "(JJ)V", reinterpret_cast<void *>(&setOfflineMapboxTileCountLimit)}
    };

    if (env->RegisterNatives(offlineManagerClass, offlineManagerMethods.data(), offlineManagerMethods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    const std::vector<JNINativeMethod> offlineRegionMethods = {
        {"destroyOfflineRegion", "(J)V", reinterpret_cast<void *>(&destroyOfflineRegion)},
        {"setOfflineRegionObserver", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionObserver;)V", reinterpret_cast<void *>(&setOfflineRegionObserver)},
        {"setOfflineRegionDownloadState", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;I)V", reinterpret_cast<void *>(&setOfflineRegionDownloadState)},
        {"getOfflineRegionStatus", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback;)V", reinterpret_cast<void *>(&getOfflineRegionStatus)},
        {"deleteOfflineRegion", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback;)V", reinterpret_cast<void *>(&deleteOfflineRegion)}
    };

    if (env->RegisterNatives(offlineRegionClass, offlineRegionMethods.data(), offlineRegionMethods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // Offline register end

    latLngClass = reinterpret_cast<jclass>(env->NewGlobalRef(latLngClass));
    if (latLngClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngBoundsClass = reinterpret_cast<jclass>(env->NewGlobalRef(latLngBoundsClass));
    if (latLngBoundsClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        return JNI_ERR;
    }

    iconClass = reinterpret_cast<jclass>(env->NewGlobalRef(iconClass));
    if (iconClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        return JNI_ERR;
    }

    markerClass = reinterpret_cast<jclass>(env->NewGlobalRef(markerClass));
    if (markerClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        return JNI_ERR;
    }

    polylineClass = reinterpret_cast<jclass>(env->NewGlobalRef(polylineClass));
    if (polylineClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        return JNI_ERR;
    }

    polygonClass = reinterpret_cast<jclass>(env->NewGlobalRef(polygonClass));
    if (polygonClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        return JNI_ERR;
    }

    runtimeExceptionClass = reinterpret_cast<jclass>(env->NewGlobalRef(runtimeExceptionClass));
    if (runtimeExceptionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        return JNI_ERR;
    }

    nullPointerExceptionClass =
        reinterpret_cast<jclass>(env->NewGlobalRef(nullPointerExceptionClass));
    if (nullPointerExceptionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        return JNI_ERR;
    }

    arrayListClass = reinterpret_cast<jclass>(env->NewGlobalRef(arrayListClass));
    if (arrayListClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        return JNI_ERR;
    }

    projectedMetersClass = reinterpret_cast<jclass>(env->NewGlobalRef(projectedMetersClass));
    if (projectedMetersClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        return JNI_ERR;
    }

    pointFClass = reinterpret_cast<jclass>(env->NewGlobalRef(pointFClass));
    if (pointFClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        return JNI_ERR;
    }

    rectFClass = reinterpret_cast<jclass>(env->NewGlobalRef(rectFClass));
    if (rectFClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        return JNI_ERR;
    }

    // Offline global definitions begin

    offlineManagerClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineManagerClass));
    if (offlineManagerClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
    }

    listOfflineRegionsCallbackClass = reinterpret_cast<jclass>(env->NewGlobalRef(listOfflineRegionsCallbackClass));
    if (listOfflineRegionsCallbackClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
    }

    offlineRegionClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionClass));
    if (offlineRegionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
    }

    offlineRegionDefinitionClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionDefinitionClass));
    if (offlineRegionDefinitionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
    }

    createOfflineRegionCallbackClass = reinterpret_cast<jclass>(env->NewGlobalRef(createOfflineRegionCallbackClass));
    if (createOfflineRegionCallbackClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
    }

    offlineRegionObserverClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionObserverClass));
    if (offlineRegionObserverClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
        env->DeleteGlobalRef(createOfflineRegionCallbackClass);
    }

    offlineRegionStatusClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionStatusClass));
    if (offlineRegionStatusClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
        env->DeleteGlobalRef(createOfflineRegionCallbackClass);
        env->DeleteGlobalRef(offlineRegionObserverClass);
    }

    offlineRegionErrorClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionErrorClass));
    if (offlineRegionErrorClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
        env->DeleteGlobalRef(createOfflineRegionCallbackClass);
        env->DeleteGlobalRef(offlineRegionObserverClass);
        env->DeleteGlobalRef(offlineRegionStatusClass);
    }

    offlineRegionStatusCallbackClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionStatusCallbackClass));
    if (offlineRegionStatusCallbackClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
        env->DeleteGlobalRef(createOfflineRegionCallbackClass);
        env->DeleteGlobalRef(offlineRegionObserverClass);
        env->DeleteGlobalRef(offlineRegionStatusClass);
        env->DeleteGlobalRef(offlineRegionErrorClass);
    }

    offlineRegionDeleteCallbackClass = reinterpret_cast<jclass>(env->NewGlobalRef(offlineRegionDeleteCallbackClass));
    if (offlineRegionDeleteCallbackClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngBoundsClass);
        env->DeleteGlobalRef(iconClass);
        env->DeleteGlobalRef(markerClass);
        env->DeleteGlobalRef(polylineClass);
        env->DeleteGlobalRef(polygonClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        env->DeleteGlobalRef(pointFClass);
        env->DeleteGlobalRef(rectFClass);
        env->DeleteGlobalRef(offlineManagerClass);
        env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
        env->DeleteGlobalRef(offlineRegionClass);
        env->DeleteGlobalRef(offlineRegionDefinitionClass);
        env->DeleteGlobalRef(createOfflineRegionCallbackClass);
        env->DeleteGlobalRef(offlineRegionObserverClass);
        env->DeleteGlobalRef(offlineRegionStatusClass);
        env->DeleteGlobalRef(offlineRegionErrorClass);
        env->DeleteGlobalRef(offlineRegionStatusCallbackClass);
    }

    // Offline global definitions end

    char release[PROP_VALUE_MAX] = "";
    __system_property_get("ro.build.version.release", release);
    androidRelease = std::string(release);

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnUnload");

    theJVM = vm;

    JNIEnv *env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        throw new std::runtime_error("GetEnv() failed");
    }

    env->DeleteGlobalRef(latLngClass);
    latLngClass = nullptr;
    latLngConstructorId = nullptr;
    latLngLongitudeId = nullptr;
    latLngLatitudeId = nullptr;

    env->DeleteGlobalRef(latLngBoundsClass);
    latLngBoundsClass = nullptr;
    latLngBoundsConstructorId = nullptr;
    latLngBoundsLatNorthId = nullptr;
    latLngBoundsLatSouthId = nullptr;
    latLngBoundsLonEastId = nullptr;
    latLngBoundsLonWestId = nullptr;

    env->DeleteGlobalRef(iconClass);
    iconClass = nullptr;
    iconIdId = nullptr;

    env->DeleteGlobalRef(markerClass);
    markerClass = nullptr;
    markerPositionId = nullptr;
    markerIconId = nullptr;
    markerIdId = nullptr;

    env->DeleteGlobalRef(polylineClass);
    polylineClass = nullptr;
    polylineAlphaId = nullptr;
    polylineColorId = nullptr;
    polylineWidthId = nullptr;
    polylinePointsId = nullptr;

    env->DeleteGlobalRef(polygonClass);
    polygonClass = nullptr;
    polygonAlphaId = nullptr;
    polygonFillColorId = nullptr;
    polygonStrokeColorId = nullptr;
    polygonPointsId = nullptr;

    onInvalidateId = nullptr;
    onMapChangedId = nullptr;
    onFpsChangedId = nullptr;

    env->DeleteGlobalRef(runtimeExceptionClass);
    runtimeExceptionClass = nullptr;

    env->DeleteGlobalRef(nullPointerExceptionClass);
    nullPointerExceptionClass = nullptr;

    listToArrayId = nullptr;

    env->DeleteGlobalRef(arrayListClass);
    arrayListClass = nullptr;
    arrayListConstructorId = nullptr;
    arrayListAddId = nullptr;

    env->DeleteGlobalRef(projectedMetersClass);
    projectedMetersClass = nullptr;
    projectedMetersConstructorId = nullptr;
    projectedMetersNorthingId = nullptr;
    projectedMetersEastingId = nullptr;

    env->DeleteGlobalRef(pointFClass);
    pointFClass = nullptr;
    pointFConstructorId = nullptr;
    pointFXId = nullptr;
    pointFYId = nullptr;

    env->DeleteGlobalRef(rectFClass);
    rectFClass = nullptr;
    rectFConstructorId = nullptr;
    rectFLeftId = nullptr;
    rectFTopId = nullptr;
    rectFRightId = nullptr;
    rectFBottomId = nullptr;

    // Offline delete begins

    env->DeleteGlobalRef(offlineManagerClass);
    offlineManagerClassPtrId = nullptr;
    offlineManagerClassPtrId = nullptr;

    env->DeleteGlobalRef(listOfflineRegionsCallbackClass);
    listOnListMethodId = nullptr;
    listOnErrorMethodId = nullptr;

    env->DeleteGlobalRef(offlineRegionClass);
    offlineRegionConstructorId = nullptr;
    offlineRegionOfflineManagerId = nullptr;
    offlineRegionIdId = nullptr;
    offlineRegionDefinitionId = nullptr;
    offlineRegionMetadataId = nullptr;
    offlineRegionPtrId = nullptr;

    env->DeleteGlobalRef(offlineRegionDefinitionClass);
    offlineRegionDefinitionConstructorId = nullptr;
    offlineRegionDefinitionStyleURLId = nullptr;
    offlineRegionDefinitionBoundsId = nullptr;
    offlineRegionDefinitionMinZoomId = nullptr;
    offlineRegionDefinitionMaxZoomId = nullptr;
    offlineRegionDefinitionPixelRatioId = nullptr;

    env->DeleteGlobalRef(createOfflineRegionCallbackClass);
    createOnCreateMethodId = nullptr;
    createOnErrorMethodId = nullptr;

    env->DeleteGlobalRef(offlineRegionObserverClass);
    offlineRegionObserveronStatusChangedId = nullptr;
    offlineRegionObserveronErrorId = nullptr;
    offlineRegionObserveronLimitId = nullptr;

    env->DeleteGlobalRef(offlineRegionStatusClass);
    offlineRegionStatusConstructorId = nullptr;
    offlineRegionStatusDownloadStateId = nullptr;
    offlineRegionStatusCompletedResourceCountId = nullptr;
    offlineRegionStatusCompletedResourceSizeId = nullptr;
    offlineRegionStatusRequiredResourceCountId = nullptr;
    offlineRegionStatusRequiredResourceCountIsPreciseId = nullptr;

    env->DeleteGlobalRef(offlineRegionErrorClass);
    offlineRegionErrorConstructorId = nullptr;
    offlineRegionErrorReasonId = nullptr;
    offlineRegionErrorMessageId = nullptr;

    env->DeleteGlobalRef(offlineRegionStatusCallbackClass);
    offlineRegionStatusOnStatusId = nullptr;
    offlineRegionStatusOnErrorId = nullptr;

    env->DeleteGlobalRef(offlineRegionDeleteCallbackClass);
    offlineRegionDeleteOnDeleteId = nullptr;
    offlineRegionDeleteOnErrorId = nullptr;

    // Offline delete ends

    theJVM = nullptr;
}
}
