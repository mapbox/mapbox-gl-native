#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <string>
#include <array>
#include <vector>

#include <android/native_window_jni.h>
#include <sys/system_properties.h>

#include "jni.hpp"
#include "java_types.hpp"
#include "native_map_view.hpp"
#include "connectivity_listener.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/sources.hpp"

#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"
#include "geometry/conversion/feature.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mapbox/geometry.hpp>

#include <jni/jni.hpp>

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace mbgl {
namespace android {

void RegisterNativeHTTPRequest(JNIEnv&);

JavaVM* theJVM;

std::string cachePath;
std::string dataPath;
std::string apkPath;
std::string androidRelease;

jni::jmethodID* onInvalidateId = nullptr;
jni::jmethodID* onMapChangedId = nullptr;
jni::jmethodID* onFpsChangedId = nullptr;
jni::jmethodID* onSnapshotReadyId = nullptr;

jni::jclass* latLngClass = nullptr;
jni::jmethodID* latLngConstructorId = nullptr;
jni::jfieldID* latLngLatitudeId = nullptr;
jni::jfieldID* latLngLongitudeId = nullptr;

jni::jclass* latLngBoundsClass = nullptr;
jni::jmethodID* latLngBoundsConstructorId = nullptr;
jni::jfieldID* latLngBoundsLatNorthId = nullptr;
jni::jfieldID* latLngBoundsLatSouthId = nullptr;
jni::jfieldID* latLngBoundsLonEastId = nullptr;
jni::jfieldID* latLngBoundsLonWestId = nullptr;

jni::jclass* iconClass = nullptr;
jni::jfieldID* iconIdId = nullptr;

jni::jclass* markerClass = nullptr;
jni::jfieldID* markerPositionId = nullptr;
jni::jfieldID* markerIconId = nullptr;
jni::jfieldID* markerIdId = nullptr;

jni::jclass* polylineClass = nullptr;
jni::jfieldID* polylineAlphaId = nullptr;
jni::jfieldID* polylineColorId = nullptr;
jni::jfieldID* polylineWidthId = nullptr;
jni::jfieldID* polylinePointsId = nullptr;

jni::jclass* polygonClass = nullptr;
jni::jfieldID* polygonAlphaId = nullptr;
jni::jfieldID* polygonFillColorId = nullptr;
jni::jfieldID* polygonStrokeColorId = nullptr;
jni::jfieldID* polygonPointsId = nullptr;

jni::jmethodID* listToArrayId = nullptr;

jni::jclass* arrayListClass = nullptr;
jni::jmethodID* arrayListConstructorId = nullptr;
jni::jmethodID* arrayListAddId = nullptr;

jni::jclass* projectedMetersClass = nullptr;
jni::jmethodID* projectedMetersConstructorId = nullptr;
jni::jfieldID* projectedMetersNorthingId = nullptr;
jni::jfieldID* projectedMetersEastingId = nullptr;

jni::jclass* pointFClass = nullptr;
jni::jmethodID* pointFConstructorId = nullptr;
jni::jfieldID* pointFXId = nullptr;
jni::jfieldID* pointFYId = nullptr;

jni::jclass* rectFClass = nullptr;
jni::jmethodID* rectFConstructorId = nullptr;
jni::jfieldID* rectFLeftId = nullptr;
jni::jfieldID* rectFTopId = nullptr;
jni::jfieldID* rectFRightId = nullptr;
jni::jfieldID* rectFBottomId = nullptr;

// Offline declarations start

jni::jfieldID* offlineManagerClassPtrId = nullptr;

jni::jmethodID* listOnListMethodId = nullptr;
jni::jmethodID* listOnErrorMethodId = nullptr;

jni::jclass* offlineRegionClass = nullptr;
jni::jmethodID* offlineRegionConstructorId = nullptr;
jni::jfieldID* offlineRegionOfflineManagerId = nullptr;
jni::jfieldID* offlineRegionIdId = nullptr;
jni::jfieldID* offlineRegionDefinitionId = nullptr;
jni::jfieldID* offlineRegionMetadataId = nullptr;
jni::jfieldID* offlineRegionPtrId = nullptr;

jni::jclass* offlineRegionDefinitionClass = nullptr;
jni::jmethodID* offlineRegionDefinitionConstructorId = nullptr;
jni::jfieldID* offlineRegionDefinitionStyleURLId = nullptr;
jni::jfieldID* offlineRegionDefinitionBoundsId = nullptr;
jni::jfieldID* offlineRegionDefinitionMinZoomId = nullptr;
jni::jfieldID* offlineRegionDefinitionMaxZoomId = nullptr;
jni::jfieldID* offlineRegionDefinitionPixelRatioId = nullptr;

jni::jmethodID* createOnCreateMethodId = nullptr;
jni::jmethodID* createOnErrorMethodId = nullptr;

jni::jmethodID* updateMetadataOnUpdateMethodId = nullptr;
jni::jmethodID* updateMetadataOnErrorMethodId = nullptr;

jni::jmethodID* offlineRegionObserveronStatusChangedId = nullptr;
jni::jmethodID* offlineRegionObserveronErrorId = nullptr;
jni::jmethodID* offlineRegionObserveronLimitId = nullptr;

jni::jclass* offlineRegionStatusClass = nullptr;
jni::jmethodID* offlineRegionStatusConstructorId = nullptr;
jni::jfieldID* offlineRegionStatusDownloadStateId = nullptr;
jni::jfieldID* offlineRegionStatusCompletedResourceCountId = nullptr;
jni::jfieldID* offlineRegionStatusCompletedResourceSizeId = nullptr;
jni::jfieldID* offlineRegionStatusCompletedTileCountId = nullptr;
jni::jfieldID* offlineRegionStatusCompletedTileSizeId = nullptr;
jni::jfieldID* offlineRegionStatusRequiredResourceCountId = nullptr;
jni::jfieldID* offlineRegionStatusRequiredResourceCountIsPreciseId = nullptr;

jni::jclass* offlineRegionErrorClass = nullptr;
jni::jmethodID* offlineRegionErrorConstructorId = nullptr;
jni::jfieldID* offlineRegionErrorReasonId = nullptr;
jni::jfieldID* offlineRegionErrorMessageId = nullptr;

jni::jmethodID* offlineRegionStatusOnStatusId = nullptr;
jni::jmethodID* offlineRegionStatusOnErrorId = nullptr;

jni::jmethodID* offlineRegionDeleteOnDeleteId = nullptr;
jni::jmethodID* offlineRegionDeleteOnErrorId = nullptr;

// Offline declarations end

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
            throw std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw std::runtime_error("AttachCurrentThread() failed");
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
            throw std::runtime_error("DetachCurrentThread() failed");
        }
    }
    *env = nullptr;
}

std::string std_string_from_jstring(JNIEnv *env, jni::jstring* jstr) {
    return jni::Make<std::string>(*env, jni::String(jstr));
}

jni::jstring* std_string_to_jstring(JNIEnv *env, std::string str) {
    return jni::Make<jni::String>(*env, str).Get();
}

std::vector<std::string> std_vector_string_from_jobject(JNIEnv *env, jni::jobject* jlist) {
    std::vector<std::string> vector;

    jni::NullCheck(*env, jlist);
    jni::jarray<jni::jobject>* jarray =
        reinterpret_cast<jni::jarray<jni::jobject>*>(jni::CallMethod<jni::jobject*>(*env, jlist, *listToArrayId));

    jni::NullCheck(*env, jarray);
    std::size_t len = jni::GetArrayLength(*env, *jarray);

    for (std::size_t i = 0; i < len; i++) {
        jni::jstring* jstr = reinterpret_cast<jni::jstring*>(jni::GetObjectArrayElement(*env, *jarray, i));
        vector.push_back(std_string_from_jstring(env, jstr));
    }

    return vector;
}

jni::jobject* std_vector_string_to_jobject(JNIEnv *env, std::vector<std::string> vector) {
    jni::jobject* jlist = &jni::NewObject(*env, *arrayListClass, *arrayListConstructorId);

    for (const auto& str : vector) {
        jni::CallMethod<jboolean>(*env, jlist, *arrayListAddId, std_string_to_jstring(env, str));
    }

    return jlist;
}

jni::jarray<jlong>* std_vector_uint_to_jobject(JNIEnv *env, const std::vector<uint32_t>& vector) {
    jni::jarray<jlong>& jarray = jni::NewArray<jlong>(*env, vector.size());

    std::vector<jlong> v;
    v.reserve(vector.size());
    std::move(vector.begin(), vector.end(), std::back_inserter(v));

    jni::SetArrayRegion(*env, jarray, 0, v);

    return &jarray;
}

static std::vector<uint8_t> metadata_from_java(JNIEnv* env, jni::jarray<jbyte>& j) {
    std::size_t length = jni::GetArrayLength(*env, j);
    std::vector<uint8_t> c;
    c.resize(length);
    jni::GetArrayRegion(*env, j, 0, length, reinterpret_cast<jbyte*>(c.data()));
    return c;
}

static jni::jarray<jbyte>* metadata_from_native(JNIEnv* env, const std::vector<uint8_t>& c) {
    std::size_t length = static_cast<std::size_t>(c.size());
    jni::jarray<jbyte>& j = jni::NewArray<jbyte>(*env, length);
    jni::SetArrayRegion(*env, j, 0, c.size(), reinterpret_cast<const jbyte*>(c.data()));
    return &j;
}

static mbgl::LatLngBounds latlngbounds_from_java(JNIEnv *env, jni::jobject* latLngBounds) {
    jdouble swLat = jni::GetField<jdouble>(*env, latLngBounds, *latLngBoundsLatSouthId);
    jdouble swLon = jni::GetField<jdouble>(*env, latLngBounds, *latLngBoundsLonWestId);
    jdouble neLat = jni::GetField<jdouble>(*env, latLngBounds, *latLngBoundsLatNorthId);
    jdouble neLon = jni::GetField<jdouble>(*env, latLngBounds, *latLngBoundsLonEastId);
    return mbgl::LatLngBounds::hull({ swLat, swLon }, { neLat, neLon });
}

static jni::jobject* latlngbounds_from_native(JNIEnv *env, mbgl::LatLngBounds bounds) {
    double northLatitude = bounds.north();
    double eastLongitude = bounds.east();
    double southLatitude = bounds.south();
    double westLongitude = bounds.west();

    jni::jobject* jbounds = &jni::NewObject(*env, *latLngBoundsClass, *latLngBoundsConstructorId,
        northLatitude, eastLongitude, southLatitude, westLongitude);

    return jbounds;
}

}
}

namespace {

using namespace mbgl::android;
using DebugOptions = mbgl::MapDebugOptions;

jlong nativeCreate(JNIEnv *env, jni::jobject* obj, jni::jstring* cachePath_, jni::jstring* dataPath_, jni::jstring* apkPath_, jfloat pixelRatio, jint availableProcessors, jlong totalMemory) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreate");
    cachePath = std_string_from_jstring(env, cachePath_);
    dataPath = std_string_from_jstring(env, dataPath_);
    apkPath = std_string_from_jstring(env, apkPath_);
    return reinterpret_cast<jlong>(new NativeMapView(env, jni::Unwrap(obj), pixelRatio, availableProcessors, totalMemory));
}

void nativeDestroy(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroy");
    assert(nativeMapViewPtr != 0);
    delete reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
}

void nativeInitializeDisplay(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeDisplay");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->initializeDisplay();
}

void nativeTerminateDisplay(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateDisplay");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->terminateDisplay();
}

void nativeInitializeContext(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeContext");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->initializeContext();
}

void nativeTerminateContext(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateContext");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->terminateContext();
}

void nativeCreateSurface(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jobject* surface) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreateSurface");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->createSurface(ANativeWindow_fromSurface(env, jni::Unwrap(surface)));
}

void nativeDestroySurface(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroySurface");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->destroySurface();
}

void nativeUpdate(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->invalidate();
}

void nativeRender(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->render();
}

void nativeViewResize(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jint width, jint height) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeViewResize");
    assert(nativeMapViewPtr != 0);
    assert(width >= 0);
    assert(height >= 0);
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->resizeView(width, height);
}

void nativeFramebufferResize(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jint fbWidth, jint fbHeight) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeFramebufferResize");
    assert(nativeMapViewPtr != 0);
    assert(fbWidth >= 0);
    assert(fbHeight >= 0);
    assert(fbWidth <= UINT16_MAX);
    assert(fbHeight <= UINT16_MAX);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->resizeFramebuffer(fbWidth, fbHeight);
}

void nativeRemoveClass(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* clazz) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeClass(std_string_from_jstring(env, clazz));
}

jboolean nativeHasClass(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* clazz) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().hasClass(std_string_from_jstring(env, clazz));
}

void nativeAddClass(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* clazz) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().addClass(std_string_from_jstring(env, clazz));
}

void nativeSetClasses(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jobject* classes) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setClasses(std_vector_string_from_jobject(env, classes));
}

jni::jobject* nativeGetClasses(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_vector_string_to_jobject(env, nativeMapView->getMap().getClasses());
}

void nativeSetAPIBaseURL(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* url) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getFileSource().setAPIBaseURL(std_string_from_jstring(env, url));
}

void nativeSetStyleUrl(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* url) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setStyleURL(std_string_from_jstring(env, url));
}

jni::jstring* nativeGetStyleUrl(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr){
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_string_to_jstring(env, nativeMapView->getMap().getStyleURL());
}

void nativeSetStyleJson(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* newStyleJson) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setStyleJSON(std_string_from_jstring(env, newStyleJson));
}

jni::jstring* nativeGetStyleJson(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_string_to_jstring(env, nativeMapView->getMap().getStyleJSON());
}

void nativeSetAccessToken(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* accessToken) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getFileSource().setAccessToken(std_string_from_jstring(env, accessToken));
}

jni::jstring* nativeGetAccessToken(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std_string_to_jstring(env, nativeMapView->getFileSource().getAccessToken());
}

void nativeCancelTransitions(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().cancelTransitions();
}

void nativeSetGestureInProgress(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jboolean inProgress) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setGestureInProgress(inProgress);
}

void nativeMoveBy(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble dx, jdouble dy,
                          jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().moveBy({dx, dy}, mbgl::Milliseconds(duration));
}

void nativeSetLatLng(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble latitude, jdouble longitude, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setLatLng(mbgl::LatLng(latitude, longitude), nativeMapView->getInsets(), mbgl::Duration(duration));
}

jni::jobject* nativeGetLatLng(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng(nativeMapView->getInsets());
    return &jni::NewObject(*env, *latLngClass, *latLngConstructorId, latLng.latitude, latLng.longitude);
}

jdoubleArray nativeGetCameraValues(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng(nativeMapView->getInsets());
    jdoubleArray  output = env->NewDoubleArray(5);
    jsize start = 0;
    jsize leng = 5;
    jdouble buf[5];
    buf[0] = latLng.latitude;
    buf[1] = latLng.longitude;
    buf[2] = -nativeMapView->getMap().getBearing();
    buf[3] = nativeMapView->getMap().getPitch();
    buf[4] = nativeMapView->getMap().getZoom();
    env->SetDoubleArrayRegion(output, start, leng, buf);

    if (output == nullptr) {
       env->ExceptionDescribe();
       return nullptr;
    }

    return output;
}

void nativeResetPosition(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetPosition();
}

jdouble nativeGetPitch(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getPitch();
}

void nativeSetPitch(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble pitch, jlong milliseconds) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::Duration duration((mbgl::Milliseconds(milliseconds)));
    nativeMapView->getMap().setPitch(pitch, duration);
}

void nativeScaleBy(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble ds, jdouble cx,
                           jdouble cy, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().scaleBy(ds, center, mbgl::Milliseconds(duration));
}

void nativeSetScale(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble scale,
                            jdouble cx, jdouble cy, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().setScale(scale, center, mbgl::Milliseconds(duration));
}

jdouble nativeGetScale(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getScale();
}

void nativeSetZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble zoom, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setZoom(zoom, mbgl::Milliseconds(duration));
}

jdouble nativeGetZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getZoom();
}

void nativeResetZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetZoom();
}

void nativeSetMinZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble zoom) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setMinZoom(zoom);
}

jdouble nativeGetMinZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMinZoom();
}

void nativeSetMaxZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble zoom) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setMaxZoom(zoom);
}

jdouble nativeGetMaxZoom(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMaxZoom();
}

void nativeRotateBy(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble sx,
                            jdouble sy, jdouble ex, jdouble ey, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate first(sx, sy);
    mbgl::ScreenCoordinate second(ex, ey);
    nativeMapView->getMap().rotateBy(first, second, mbgl::Milliseconds(duration));
}

void nativeSetBearing(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble degrees,
                              jlong milliseconds) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::Duration duration((mbgl::Milliseconds(milliseconds)));
    nativeMapView->getMap().setBearing(degrees, duration);
}

void nativeSetBearingXY(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble degrees,
                              jdouble cx, jdouble cy) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate center(cx, cy);
    nativeMapView->getMap().setBearing(degrees, center);
}

jdouble nativeGetBearing(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getBearing();
}

void nativeResetNorth(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetNorth();
}

void nativeUpdateMarker(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong markerId, jdouble lat, jdouble lon, jni::jstring* jid) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    if (markerId == -1) {
        return;
    }
    std::string iconId = std_string_from_jstring(env, jid);
    // Because Java only has int, not unsigned int, we need to bump the annotation id up to a long.
    nativeMapView->getMap().updateAnnotation(markerId, mbgl::SymbolAnnotation { mbgl::Point<double>(lon, lat), iconId });
}

jni::jarray<jlong>* nativeAddMarkers(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jarray<jni::jobject>* jarray) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    NullCheck(*env, jarray);
    std::size_t len = jni::GetArrayLength(*env, *jarray);

    mbgl::AnnotationIDs ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::jobject* marker = jni::GetObjectArrayElement(*env, *jarray, i);
        jni::jobject* position = jni::GetField<jni::jobject*>(*env, marker, *markerPositionId);
        jni::jobject* icon = jni::GetField<jni::jobject*>(*env, marker, *markerIconId);
        jni::jstring* jid = reinterpret_cast<jni::jstring*>(jni::GetField<jni::jobject*>(*env, icon, *iconIdId));

        jdouble latitude = jni::GetField<jdouble>(*env, position, *latLngLatitudeId);
        jdouble longitude = jni::GetField<jdouble>(*env, position, *latLngLongitudeId);

        ids.push_back(nativeMapView->getMap().addAnnotation(mbgl::SymbolAnnotation {
            mbgl::Point<double>(longitude, latitude),
            std_string_from_jstring(env, jid)
        }));

        jni::DeleteLocalRef(*env, position);
        jni::DeleteLocalRef(*env, jid);
        jni::DeleteLocalRef(*env, icon);
        jni::DeleteLocalRef(*env, marker);
    }

    return std_vector_uint_to_jobject(env, ids);
}

static mbgl::Color toColor(jint color) {
    float r = (color >> 16) & 0xFF;
    float g = (color >> 8) & 0xFF;
    float b = (color) & 0xFF;
    float a = (color >> 24) & 0xFF;
    return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}

template <class Geometry>
Geometry toGeometry(JNIEnv *env, jni::jobject* jlist) {
    NullCheck(*env, jlist);
    jni::jarray<jni::jobject>* jarray =
        reinterpret_cast<jni::jarray<jni::jobject>*>(jni::CallMethod<jni::jobject*>(*env, jlist, *listToArrayId));
    NullCheck(*env, jarray);

    std::size_t size = jni::GetArrayLength(*env, *jarray);

    Geometry geometry;
    geometry.reserve(size);

    for (std::size_t i = 0; i < size; i++) {
        jni::jobject* latLng = reinterpret_cast<jni::jobject*>(jni::GetObjectArrayElement(*env, *jarray, i));
        NullCheck(*env, latLng);

        geometry.push_back(mbgl::Point<double>(
            jni::GetField<jdouble>(*env, latLng, *latLngLongitudeId),
            jni::GetField<jdouble>(*env, latLng, *latLngLatitudeId)));

        jni::DeleteLocalRef(*env, latLng);
    }

    jni::DeleteLocalRef(*env, jarray);
    jni::DeleteLocalRef(*env, jlist);

    return geometry;
}

jni::jarray<jlong>* nativeAddPolylines(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jarray<jni::jobject>* jarray) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    NullCheck(*env, jarray);
    std::size_t len = jni::GetArrayLength(*env, *jarray);

    mbgl::AnnotationIDs ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::jobject* polyline = jni::GetObjectArrayElement(*env, *jarray, i);
        jni::jobject* points = jni::GetField<jni::jobject*>(*env, polyline, *polylinePointsId);

        mbgl::LineAnnotation annotation { toGeometry<mbgl::LineString<double>>(env, points) };
        annotation.opacity = { jni::GetField<jfloat>(*env, polyline, *polylineAlphaId) };
        annotation.color = { toColor(jni::GetField<jint>(*env, polyline, *polylineColorId)) };
        annotation.width = { jni::GetField<jfloat>(*env, polyline, *polylineWidthId) };
        ids.push_back(nativeMapView->getMap().addAnnotation(annotation));

        jni::DeleteLocalRef(*env, polyline);
    }

    return std_vector_uint_to_jobject(env, ids);
}

jni::jarray<jlong>* nativeAddPolygons(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jarray<jni::jobject>* jarray) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    NullCheck(*env, jarray);
    std::size_t len = jni::GetArrayLength(*env, *jarray);

    mbgl::AnnotationIDs ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::jobject* polygon = jni::GetObjectArrayElement(*env, *jarray, i);
        jni::jobject* points = jni::GetField<jni::jobject*>(*env, polygon, *polygonPointsId);

        mbgl::FillAnnotation annotation { mbgl::Polygon<double> { toGeometry<mbgl::LinearRing<double>>(env, points) } };
        annotation.opacity = { jni::GetField<jfloat>(*env, polygon, *polygonAlphaId) };
        annotation.outlineColor = { toColor(jni::GetField<jint>(*env, polygon, *polygonStrokeColorId)) };
        annotation.color = { toColor(jni::GetField<jint>(*env, polygon, *polygonFillColorId)) };
        ids.push_back(nativeMapView->getMap().addAnnotation(annotation));

        jni::DeleteLocalRef(*env, polygon);
    }

    return std_vector_uint_to_jobject(env, ids);
}

void nativeUpdatePolygon(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong polygonId, jni::jobject* polygon) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    jni::jobject* points = jni::GetField<jni::jobject*>(*env, polygon, *polygonPointsId);

    mbgl::FillAnnotation annotation { mbgl::Polygon<double> { toGeometry<mbgl::LinearRing<double>>(env, points) } };
    annotation.opacity = { jni::GetField<jfloat>(*env, polygon, *polygonAlphaId) };
    annotation.outlineColor = { toColor(jni::GetField<jint>(*env, polygon, *polygonStrokeColorId)) };
    annotation.color = { toColor(jni::GetField<jint>(*env, polygon, *polygonFillColorId)) };
    nativeMapView->getMap().updateAnnotation(polygonId, annotation);
}

void nativeUpdatePolyline(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong polylineId, jni::jobject* polyline) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    jni::jobject* points = jni::GetField<jni::jobject*>(*env, polyline, *polylinePointsId);

    mbgl::LineAnnotation annotation { toGeometry<mbgl::LineString<double>>(env, points) };
    annotation.opacity = { jni::GetField<jfloat>(*env, polyline, *polylineAlphaId) };
    annotation.color = { toColor(jni::GetField<jint>(*env, polyline, *polylineColorId)) };
    annotation.width = { jni::GetField<jfloat>(*env, polyline, *polylineWidthId) };
    nativeMapView->getMap().updateAnnotation(polylineId, annotation);
}

void nativeRemoveAnnotations(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jarray<jlong>* jarray) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    NullCheck(*env, jarray);
    std::size_t len = jni::GetArrayLength(*env, *jarray);
    auto elements = jni::GetArrayElements(*env, *jarray);
    jlong* jids = std::get<0>(elements).get();

    for (std::size_t i = 0; i < len; i++) {
        if(jids[i] == -1L)
            continue;
        nativeMapView->getMap().removeAnnotation(jids[i]);
    }
}

jni::jarray<jlong>* nativeQueryPointAnnotations(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jobject* rect) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    // Conversion
    jfloat left = jni::GetField<jfloat>(*env, rect, *rectFLeftId);
    jfloat right = jni::GetField<jfloat>(*env, rect, *rectFRightId);
    jfloat top = jni::GetField<jfloat>(*env, rect, *rectFTopId);
    jfloat bottom = jni::GetField<jfloat>(*env, rect, *rectFBottomId);
    mbgl::ScreenBox box = {
        { left, top },
        { right, bottom },
    };

    // Assume only points for now
    mbgl::AnnotationIDs ids = nativeMapView->getMap().queryPointAnnotations(box);

    return std_vector_uint_to_jobject(env, ids);
}

void nativeAddAnnotationIcon(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr,
        jni::jstring* symbol, jint width, jint height, jfloat scale, jni::jarray<jbyte>* jpixels) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    const std::string symbolName = std_string_from_jstring(env, symbol);

    NullCheck(*env, jpixels);
    std::size_t size = jni::GetArrayLength(*env, *jpixels);
    mbgl::PremultipliedImage premultipliedImage(
        { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(*env, *jpixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));

    auto iconImage = std::make_shared<mbgl::SpriteImage>(
        std::move(premultipliedImage),
        float(scale));

    nativeMapView->getMap().addAnnotationIcon(symbolName, iconImage);
}

void nativeSetVisibleCoordinateBounds(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr,
        jni::jarray<jni::jobject>* coordinates, jni::jobject* padding, jdouble direction, jlong duration) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jfloat left = jni::GetField<jfloat>(*env, padding, *rectFLeftId);
    jfloat right = jni::GetField<jfloat>(*env, padding, *rectFRightId);
    jfloat top = jni::GetField<jfloat>(*env, padding, *rectFTopId);
    jfloat bottom = jni::GetField<jfloat>(*env, padding, *rectFBottomId);

    NullCheck(*env, coordinates);
    std::size_t count = jni::GetArrayLength(*env, *coordinates);

    mbgl::EdgeInsets mbglInsets = {top, left, bottom, right};
    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(count);

    for (std::size_t i = 0; i < count; i++) {
        jni::jobject* latLng = jni::GetObjectArrayElement(*env, *coordinates, i);
        jdouble latitude = jni::GetField<jdouble>(*env, latLng, *latLngLatitudeId);
        jdouble longitude = jni::GetField<jdouble>(*env, latLng, *latLngLongitudeId);
        latLngs.push_back(mbgl::LatLng(latitude, longitude));
    }

    mbgl::CameraOptions cameraOptions = nativeMapView->getMap().cameraForLatLngs(latLngs, mbglInsets);
    if (direction >= 0) {
        // convert from degrees to radians
        cameraOptions.angle = (-direction * M_PI) / 180;
    }
    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
        animationOptions.duration.emplace(mbgl::Milliseconds(duration));
        // equivalent to kCAMediaTimingFunctionDefault in iOS
        animationOptions.easing.emplace(mbgl::util::UnitBezier { 0.25, 0.1, 0.25, 0.1 });
    }

    nativeMapView->getMap().easeTo(cameraOptions, animationOptions);
}

jni::jarray<jni::jobject>* nativeQueryRenderedFeaturesForPoint(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jfloat x, jni::jfloat y, jni::jarray<jni::jobject>*  layerIds) {
    using namespace mbgl::android::conversion;
    using namespace mapbox::geometry;

    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && jni::GetArrayLength(*env, *layerIds) > 0) {
        layers = toVector(*env, *layerIds);
    }
    point<double> point = {x, y};

    return *convert<jni::jarray<jni::jobject>*, std::vector<mbgl::Feature>>(*env, nativeMapView->getMap().queryRenderedFeatures(point, layers));
}

jni::jarray<jni::jobject>* nativeQueryRenderedFeaturesForBox(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jfloat left, jni::jfloat top, jni::jfloat right, jni::jfloat bottom, jni::jarray<jni::jobject>*  layerIds) {
    using namespace mbgl::android::conversion;
    using namespace mapbox::geometry;

    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && jni::GetArrayLength(*env, *layerIds) > 0) {
        layers = toVector(*env, *layerIds);
    }
    box<double> box = { point<double>{ left, top}, point<double>{ right, bottom } };

    return *convert<jni::jarray<jni::jobject>*, std::vector<mbgl::Feature>>(*env, nativeMapView->getMap().queryRenderedFeatures(box, layers));
}

void nativeOnLowMemory(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().onLowMemory();
}

void nativeSetDebug(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jboolean debug) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    DebugOptions debugOptions = debug ? DebugOptions::TileBorders | DebugOptions::ParseStatus | DebugOptions::Collision
                                      : DebugOptions::NoDebug;
    nativeMapView->getMap().setDebug(debugOptions);
    nativeMapView->enableFps(debug);
}

void nativeToggleDebug(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().cycleDebugOptions();
    nativeMapView->enableFps(nativeMapView->getMap().getDebug() != DebugOptions::NoDebug);
}

jboolean nativeGetDebug(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getDebug() != DebugOptions::NoDebug;
}

jboolean nativeIsFullyLoaded(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().isFullyLoaded();
}

void nativeSetReachability(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jboolean status) {
    assert(nativeMapViewPtr != 0);
    if (status) {
        mbgl::NetworkStatus::Reachable();
    }
}

jdouble nativeGetMetersPerPixelAtLatitude(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble lat, jdouble zoom) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMetersPerPixelAtLatitude(lat, zoom);
}

jni::jobject* nativeProjectedMetersForLatLng(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble latitude, jdouble longitude) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ProjectedMeters projectedMeters = nativeMapView->getMap().projectedMetersForLatLng(mbgl::LatLng(latitude, longitude));
    return &jni::NewObject(*env, *projectedMetersClass, *projectedMetersConstructorId, projectedMeters.northing, projectedMeters.easting);
}

jni::jobject* nativeLatLngForProjectedMeters(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble northing, jdouble easting) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().latLngForProjectedMeters(mbgl::ProjectedMeters(northing, easting));
    return &jni::NewObject(*env, *latLngClass, *latLngConstructorId, latLng.latitude, latLng.longitude);
}

jni::jobject* nativePixelForLatLng(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble latitude, jdouble longitude) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::ScreenCoordinate pixel = nativeMapView->getMap().pixelForLatLng(mbgl::LatLng(latitude, longitude));
    return &jni::NewObject(*env, *pointFClass, *pointFConstructorId, static_cast<jfloat>(pixel.x), static_cast<jfloat>(pixel.y));
}

jni::jobject* nativeLatLngForPixel(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jfloat x, jfloat y) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().latLngForPixel(mbgl::ScreenCoordinate(x, y));
    return &jni::NewObject(*env, *latLngClass, *latLngConstructorId, latLng.latitude, latLng.longitude);
}

jdouble nativeGetTopOffsetPixelsForAnnotationSymbol(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* symbolName) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getTopOffsetPixelsForAnnotationIcon(std_string_from_jstring(env, symbolName));
}

void nativeJumpTo(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble angle, jdouble latitude, jdouble longitude, jdouble pitch, jdouble zoom) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    mbgl::CameraOptions options;
    if (angle != -1) {
        options.angle = (-angle * M_PI) / 180;
    }
    options.center = mbgl::LatLng(latitude, longitude);
    options.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        options.pitch = pitch * M_PI / 180;
    }
    if (zoom != -1) {
        options.zoom = zoom;
    }

    nativeMapView->getMap().jumpTo(options);
}

void nativeEaseTo(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble angle, jdouble latitude, jdouble longitude, jlong duration, jdouble pitch, jdouble zoom, jboolean easing) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = (-angle * M_PI) / 180;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        cameraOptions.pitch = pitch * M_PI / 180;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Duration(duration));

    if (!easing) {
       // add a linear interpolator instead of easing
       animationOptions.easing.emplace(mbgl::util::UnitBezier { 0, 0, 1, 1 });
    }

    nativeMapView->getMap().easeTo(cameraOptions, animationOptions);
}

void nativeSetContentPadding(JNIEnv *env, jni::jobject* obj,long nativeMapViewPtr, double top, double left, double bottom, double right) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->setInsets({top, left, bottom, right});
}

void nativeFlyTo(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jdouble angle, jdouble latitude, jdouble longitude, jlong duration, jdouble pitch, jdouble zoom) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = (-angle * M_PI) / 180 ;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = nativeMapView->getInsets();
    if (pitch != -1) {
        cameraOptions.pitch = pitch * M_PI / 180;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Duration(duration));

    nativeMapView->getMap().flyTo(cameraOptions, animationOptions);
}

jlong nativeGetTransitionDuration(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    const auto transitionOptions = nativeMapView->getMap().getTransitionOptions();
    return transitionOptions.duration.value_or(mbgl::Duration::zero()).count();
}

void nativeSetTransitionDuration(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong duration) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    auto transitionOptions = nativeMapView->getMap().getTransitionOptions();
    transitionOptions.duration = std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<jlong>(duration));
    nativeMapView->getMap().setTransitionOptions(transitionOptions);
}

jlong nativeGetTransitionDelay(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    const auto transitionOptions = nativeMapView->getMap().getTransitionOptions();
    return transitionOptions.delay.value_or(mbgl::Duration::zero()).count();
}

void nativeSetTransitionDelay(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong delay) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    auto transitionOptions = nativeMapView->getMap().getTransitionOptions();
    transitionOptions.delay = std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<jlong>(delay));
    nativeMapView->getMap().setTransitionOptions(transitionOptions);
}

jni::jobject* nativeGetLayer(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* layerId) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    //Get the native map peer
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    //Find the layer
    mbgl::style::Layer* coreLayer = nativeMapView->getMap().getLayer(std_string_from_jstring(env, layerId));
    if (!coreLayer) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No layer found");
       return jni::Object<Layer>();
    }

    //Create and return the layer's native peer
    return createJavaLayerPeer(*env, nativeMapView->getMap(), *coreLayer);
}

void nativeAddLayer(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong nativeLayerPtr, jni::jstring* before) {
    assert(nativeMapViewPtr != 0);
    assert(nativeLayerPtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    Layer *layer = reinterpret_cast<Layer *>(nativeLayerPtr);
    try {
        layer->addToMap(nativeMapView->getMap(), before ? mbgl::optional<std::string>(std_string_from_jstring(env, before)) : mbgl::optional<std::string>());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

/**
 * Remove by layer id. Ownership is not transferred back
 */
void nativeRemoveLayerById(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* id) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    try {
        nativeMapView->getMap().removeLayer(std_string_from_jstring(env, id));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/layers/NoSuchLayerException"), error.what());
    }
}

/**
 * Remove with wrapper object id. Ownership is transferred back to the wrapper
 */
void nativeRemoveLayer(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong layerPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::android::Layer *layer = reinterpret_cast<mbgl::android::Layer *>(layerPtr);
    try {
        std::unique_ptr<mbgl::style::Layer> coreLayer = nativeMapView->getMap().removeLayer(layer->get().getID());
        layer->setLayer(std::move(coreLayer));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/layers/NoSuchLayerException"), error.what());
    }
}


jni::jobject* nativeGetSource(JNIEnv *env, jni::jobject* obj, jni::jlong nativeMapViewPtr, jni::jstring* sourceId) {
    assert(env);
    assert(nativeMapViewPtr != 0);

    //Get the native map peer
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    //Find the source
    mbgl::style::Source* coreSource = nativeMapView->getMap().getSource(std_string_from_jstring(env, sourceId));
    if (!coreSource) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No source found");
       return jni::Object<Source>();
    }

    //Create and return the source's native peer
    return createJavaSourcePeer(*env, nativeMapView->getMap(), *coreSource);
}

void nativeAddSource(JNIEnv *env, jni::jobject* obj, jni::jlong nativeMapViewPtr, jni::jlong nativeSourcePtr) {
    assert(nativeMapViewPtr != 0);
    assert(nativeSourcePtr != 0);

    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    Source *source = reinterpret_cast<Source *>(nativeSourcePtr);
    try {
        source->addToMap(nativeMapView->getMap());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/sources/CannotAddSourceException"), error.what());
    }
}

void nativeRemoveSourceById(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* id) {
     assert(nativeMapViewPtr != 0);
     NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
     try {
         nativeMapView->getMap().removeSource(std_string_from_jstring(env, id));
     } catch (const std::runtime_error& error) {
         jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/sources/NoSuchSourceException"), error.what());
     }
}

void nativeRemoveSource(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jlong sourcePtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::android::Source *source = reinterpret_cast<mbgl::android::Source *>(sourcePtr);
    try {
        std::unique_ptr<mbgl::style::Source> coreSource = nativeMapView->getMap().removeSource(source->get().getID());
        source->setSource(std::move(coreSource));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(*env, jni::FindClass(*env, "com/mapbox/mapboxsdk/style/sources/NoSuchSourceException"), error.what());
    }
}

void nativeAddImage(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* name, jni::jint width, jni::jint height, jni::jfloat pixelRatio, jni::jarray<jbyte>* data) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    // Create Pre-multiplied image from byte[]
    NullCheck(*env, data);
    std::size_t size = jni::GetArrayLength(*env, *data);
    mbgl::PremultipliedImage premultipliedImage(
        { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(*env, *data, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));

    //Wrap in a SpriteImage with the correct pixel ratio
    auto spriteImage = std::make_unique<mbgl::SpriteImage>(std::move(premultipliedImage), float(pixelRatio));

    nativeMapView->getMap().addImage(std_string_from_jstring(env, name), std::move(spriteImage));
}

void nativeRemoveImage(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr, jni::jstring* name) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeImage(std_string_from_jstring(env, name));
}

void nativeScheduleTakeSnapshot(JNIEnv *env, jni::jobject* obj, jlong nativeMapViewPtr) {
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->scheduleTakeSnapshot();
}

// Offline calls begin

jlong createDefaultFileSource(JNIEnv *env, jni::jobject* obj, jni::jstring* cachePath_, jni::jstring* assetRoot_, jlong maximumCacheSize) {
    std::string cachePath = std_string_from_jstring(env, cachePath_);
    std::string assetRoot = std_string_from_jstring(env, assetRoot_);
    mbgl::DefaultFileSource *defaultFileSource = new mbgl::DefaultFileSource(cachePath, assetRoot, maximumCacheSize);
    jlong defaultFileSourcePtr = reinterpret_cast<jlong>(defaultFileSource);
    return defaultFileSourcePtr;
}

void setAccessToken(JNIEnv *env, jni::jobject* obj, jlong defaultFileSourcePtr, jni::jstring* accessToken_) {
    assert(defaultFileSourcePtr != 0);
    std::string accessToken = std_string_from_jstring(env, accessToken_);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->setAccessToken(accessToken);
}

jni::jstring* getAccessToken(JNIEnv *env, jni::jobject* obj, jlong defaultFileSourcePtr) {
    assert(defaultFileSourcePtr != 0);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    std::string accessToken = defaultFileSource->getAccessToken();
    return std_string_to_jstring(env, accessToken);
}

void listOfflineRegions(JNIEnv *env, jni::jobject* obj, jlong defaultFileSourcePtr, jni::jobject* listCallback) {
    // Checks
    assert(defaultFileSourcePtr != 0);
    NullCheck(*env, listCallback);

    // Makes sure the objects don't get GC'ed
    obj = jni::NewGlobalRef(*env, obj).release();
    listCallback = jni::NewGlobalRef(*env, listCallback).release();

    // Launch listCallback
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->listOfflineRegions([obj, defaultFileSourcePtr, listCallback](std::exception_ptr error, mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) mutable {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            jni::CallMethod<void>(*env2, listCallback, *listOnErrorMethodId, std_string_to_jstring(env2, message));
        } else if (regions) {
            // Build jni::jarray<jni::jobject>*
            std::size_t index = 0;
            jni::jarray<jni::jobject>* jregions = &jni::NewObjectArray(*env2, regions->size(), *offlineRegionClass, NULL);
            for (auto& region : *regions) {
                // Create a new local reference frame (capacity 2 for the NewObject allocations below)
                // to avoid a local reference table overflow (#5629)
                jni::UniqueLocalFrame frame = jni::PushLocalFrame(*env2, 2);

                // Build the Region object
                jni::jobject* jregion = &jni::NewObject(*env2, *offlineRegionClass, *offlineRegionConstructorId);
                jni::SetField<jni::jobject*>(*env2, jregion, *offlineRegionOfflineManagerId, obj);
                jni::SetField<jlong>(*env2, jregion, *offlineRegionIdId, region.getID());

                // Definition object
                mbgl::OfflineTilePyramidRegionDefinition definition = region.getDefinition();
                jni::jobject* jdefinition = &jni::NewObject(*env2, *offlineRegionDefinitionClass, *offlineRegionDefinitionConstructorId);
                jni::SetField<jni::jobject*>(*env2, jdefinition, *offlineRegionDefinitionStyleURLId, std_string_to_jstring(env2, definition.styleURL));
                jni::SetField<jni::jobject*>(*env2, jdefinition, *offlineRegionDefinitionBoundsId, latlngbounds_from_native(env2, definition.bounds));
                jni::SetField<jdouble>(*env2, jdefinition, *offlineRegionDefinitionMinZoomId, definition.minZoom);
                jni::SetField<jdouble>(*env2, jdefinition, *offlineRegionDefinitionMaxZoomId, definition.maxZoom);
                jni::SetField<jfloat>(*env2, jdefinition, *offlineRegionDefinitionPixelRatioId, definition.pixelRatio);
                jni::SetField<jni::jobject*>(*env2, jregion, *offlineRegionDefinitionId, jdefinition);

                // Metadata object
                jni::jarray<jbyte>* metadata = metadata_from_native(env2, region.getMetadata());
                jni::SetField<jni::jobject*>(*env2, jregion, *offlineRegionMetadataId, metadata);

                // Moves the region on the stack into a heap-allocated one
                jni::SetField<jlong>(*env2, jregion, *offlineRegionPtrId,
                    reinterpret_cast<jlong>(new mbgl::OfflineRegion(std::move(region))));

                jni::SetObjectArrayElement(*env2, *jregions, index, jregion);
                index++;
            }

            // Trigger callback
            jni::CallMethod<void>(*env2, listCallback, *listOnListMethodId, jregions);
        }

        // Delete global refs and detach when we're done
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(obj));
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(listCallback));
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void createOfflineRegion(JNIEnv *env, jni::jobject* obj, jlong defaultFileSourcePtr, jni::jobject* definition_, jni::jarray<jbyte>* metadata_, jni::jobject* createCallback) {
    // Checks
    assert(defaultFileSourcePtr != 0);
    NullCheck(*env, createCallback);

    // Definition fields
    jni::jstring* jStyleURL = reinterpret_cast<jni::jstring*>(jni::GetField<jni::jobject*>(*env, definition_, *offlineRegionDefinitionStyleURLId));
    std::string styleURL = std_string_from_jstring(env, jStyleURL);
    jni::jobject* jBounds = jni::GetField<jni::jobject*>(*env, definition_, *offlineRegionDefinitionBoundsId);
    jdouble jMinZoom = jni::GetField<jdouble>(*env, definition_, *offlineRegionDefinitionMinZoomId);
    jdouble jMaxZoom = jni::GetField<jdouble>(*env, definition_, *offlineRegionDefinitionMaxZoomId);
    jfloat jPixelRatio = jni::GetField<jfloat>(*env, definition_, *offlineRegionDefinitionPixelRatioId);

    // Convert bounds fields to native
    mbgl::LatLngBounds bounds = latlngbounds_from_java(env, jBounds);

    // Definition
    mbgl::OfflineTilePyramidRegionDefinition definition(styleURL, bounds, jMinZoom, jMaxZoom, jPixelRatio);

    // Metadata
    mbgl::OfflineRegionMetadata metadata;
    if (metadata_ != nullptr) {
        metadata = metadata_from_java(env, *metadata_);
    }

    // Makes sure the objects don't get GC'ed
    obj = jni::NewGlobalRef(*env, obj).release();
    createCallback = jni::NewGlobalRef(*env, createCallback).release();

    // Launch createCallback
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->createOfflineRegion(definition, metadata, [obj, defaultFileSourcePtr, createCallback] (std::exception_ptr error, mbgl::optional<mbgl::OfflineRegion> region) mutable {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            jni::CallMethod<void>(*env2, createCallback, *createOnErrorMethodId, std_string_to_jstring(env2, message));
        } else if (region) {
            // Build the Region object
            jni::jobject* jregion = &jni::NewObject(*env2, *offlineRegionClass, *offlineRegionConstructorId);
            jni::SetField<jni::jobject*>(*env2, jregion, *offlineRegionOfflineManagerId, obj);
            jni::SetField<jlong>(*env2, jregion, *offlineRegionIdId, region->getID());

            // Metadata object
            jni::jarray<jbyte>* jmetadata = metadata_from_native(env2, region->getMetadata());
            jni::SetField<jni::jobject*>(*env2, jregion, *offlineRegionMetadataId, jmetadata);

            // Moves the region on the stack into a heap-allocated one
            jni::SetField<jlong>(*env2, jregion, *offlineRegionPtrId,
                reinterpret_cast<jlong>(new mbgl::OfflineRegion(std::move(*region))));

            // Invoke Java callback
            jni::CallMethod<void>(*env2, createCallback, *createOnCreateMethodId, jregion);
        }

        // Delete global refs and detach when we're done
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(obj));
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(createCallback));
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void setOfflineMapboxTileCountLimit(JNIEnv *env, jni::jobject* obj, jlong defaultFileSourcePtr, jlong limit) {
    // Checks
    assert(defaultFileSourcePtr != 0);
    assert(limit > 0);

    // Set limit
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);
    defaultFileSource->setOfflineMapboxTileCountLimit(limit);
}

mbgl::OfflineRegion* getOfflineRegionPeer(JNIEnv *env, jni::jobject* offlineRegion_) {
    jlong offlineRegionPtr = jni::GetField<jlong>(*env, offlineRegion_, *offlineRegionPtrId);
    if (!offlineRegionPtr) {
        jni::ThrowNew(*env, jni::FindClass(*env, "java/lang/IllegalStateException"),
            "Use of OfflineRegion after OfflineRegion.delete");
    }
    return reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);
}

void destroyOfflineRegion(JNIEnv *env, jni::jobject* offlineRegion_) {
    // Offline region
    jlong offlineRegionPtr = jni::GetField<jlong>(*env, offlineRegion_, *offlineRegionPtrId);
    if (!offlineRegionPtr) {
        return; // Already deleted
    }

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Release the observer and delete the region
    mbgl::OfflineRegion *offlineRegion = reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr);
    defaultFileSource->setOfflineRegionObserver(*offlineRegion, nullptr);
    jni::SetField<jlong>(*env, offlineRegion_, *offlineRegionPtrId, 0);
    delete offlineRegion;
}

void setOfflineRegionObserver(JNIEnv *env, jni::jobject* offlineRegion_, jni::jobject* observerCallback) {
    // Offline region
    mbgl::OfflineRegion* offlineRegion = getOfflineRegionPeer(env, offlineRegion_);

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Define the observer
    class Observer : public mbgl::OfflineRegionObserver {
    public:
        Observer(jni::UniqueGlobalRef<jni::jobject>&& observerCallback_)
            : observerCallback(std::move(observerCallback_)) {
        }

        ~Observer() override {
            mbgl::Log::Debug(mbgl::Event::JNI, "~Observer()");
            // Env
            JNIEnv* env2;
            jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
            jni::DeleteGlobalRef(*env2, std::move(observerCallback));
            detach_jni_thread(theJVM, &env2, renderDetach);
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

            // Create a new local reference frame (capacity 1 for the NewObject allocation below)
            // to avoid a local reference table overflow (#4706)
            jni::UniqueLocalFrame frame = jni::PushLocalFrame(*env2, 1);

            // Stats object
            jni::jobject* jstatus = &jni::NewObject(*env2, *offlineRegionStatusClass, *offlineRegionStatusConstructorId);
            jni::SetField<jint>(*env2, jstatus, *offlineRegionStatusDownloadStateId, downloadState);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedResourceCountId, status.completedResourceCount);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedResourceSizeId, status.completedResourceSize);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedTileCountId, status.completedTileCount);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedTileSizeId, status.completedTileSize);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusRequiredResourceCountId, status.requiredResourceCount);
            jni::SetField<jboolean>(*env2, jstatus, *offlineRegionStatusRequiredResourceCountIsPreciseId, status.requiredResourceCountIsPrecise);
            jni::CallMethod<void>(*env2, observerCallback.get(), *offlineRegionObserveronStatusChangedId, jstatus);

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
                case mbgl::Response::Error::Reason::RateLimit:
                    errorReason = "REASON_RATE_LIMIT";
                    break;
                case mbgl::Response::Error::Reason::Other:
                    errorReason = "REASON_OTHER";
                    break;
            }

            // Error object
            jni::UniqueLocalFrame frame = jni::PushLocalFrame(*env2, 3);
            jni::jobject* jerror = &jni::NewObject(*env2, *offlineRegionErrorClass, *offlineRegionErrorConstructorId);
            jni::SetField<jni::jobject*>(*env2, jerror, *offlineRegionErrorReasonId, std_string_to_jstring(env2, errorReason));
            jni::SetField<jni::jobject*>(*env2, jerror, *offlineRegionErrorMessageId, std_string_to_jstring(env2, error.message));
            jni::CallMethod<void>(*env2, observerCallback.get(), *offlineRegionObserveronErrorId, jerror);

            // Detach when we're done
            detach_jni_thread(theJVM, &env2, renderDetach);
        }

        void mapboxTileCountLimitExceeded(uint64_t limit) override {
            // Env
            JNIEnv* env2;
            jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");

            // Send limit
            jni::CallMethod<void>(*env2, observerCallback.get(), *offlineRegionObserveronLimitId, jlong(limit));

            // Detach when we're done
            detach_jni_thread(theJVM, &env2, renderDetach);
        }

        jni::UniqueGlobalRef<jni::jobject> observerCallback;
    };

    // Set the observer
    defaultFileSource->setOfflineRegionObserver(*offlineRegion,
        std::make_unique<Observer>(jni::NewGlobalRef(*env, observerCallback)));
}

void setOfflineRegionDownloadState(JNIEnv *env, jni::jobject* offlineRegion_, jint offlineRegionDownloadState) {
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
    mbgl::OfflineRegion* offlineRegion = getOfflineRegionPeer(env, offlineRegion_);

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Set new state
    defaultFileSource->setOfflineRegionDownloadState(*offlineRegion, state);
}

void getOfflineRegionStatus(JNIEnv *env, jni::jobject* offlineRegion_, jni::jobject* statusCallback) {
    // Offline region
    mbgl::OfflineRegion* offlineRegion = getOfflineRegionPeer(env, offlineRegion_);

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Makes sure the callback doesn't get GC'ed
    statusCallback = jni::NewGlobalRef(*env, statusCallback).release();

    // Set new state
    defaultFileSource->getOfflineRegionStatus(*offlineRegion, [statusCallback](std::exception_ptr error, mbgl::optional<mbgl::OfflineRegionStatus> status) mutable {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            jni::CallMethod<void>(*env2, statusCallback, *offlineRegionStatusOnErrorId, std_string_to_jstring(env2, message));
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
            jni::jobject* jstatus = &jni::NewObject(*env2, *offlineRegionStatusClass, *offlineRegionStatusConstructorId);
            jni::SetField<jint>(*env2, jstatus, *offlineRegionStatusDownloadStateId, downloadState);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedResourceCountId, status->completedResourceCount);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusCompletedResourceSizeId, status->completedResourceSize);
            jni::SetField<jlong>(*env2, jstatus, *offlineRegionStatusRequiredResourceCountId, status->requiredResourceCount);
            jni::SetField<jboolean>(*env2, jstatus, *offlineRegionStatusRequiredResourceCountIsPreciseId, status->requiredResourceCountIsPrecise);
            jni::CallMethod<void>(*env2, statusCallback, *offlineRegionStatusOnStatusId, jstatus);
        }

        // Delete global refs and detach when we're done
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(statusCallback));
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void deleteOfflineRegion(JNIEnv *env, jni::jobject* offlineRegion_, jni::jobject* deleteCallback) {
    // Offline region
    mbgl::OfflineRegion* offlineRegion = getOfflineRegionPeer(env, offlineRegion_);

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Makes sure the callback doesn't get GC'ed
    deleteCallback = jni::NewGlobalRef(*env, deleteCallback).release();

    // Set new state
    jni::SetField<jlong>(*env, offlineRegion_, *offlineRegionPtrId, 0);
    defaultFileSource->deleteOfflineRegion(std::move(*offlineRegion), [deleteCallback](std::exception_ptr error) mutable {

        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            jni::CallMethod<void>(*env2, deleteCallback, *offlineRegionDeleteOnErrorId, std_string_to_jstring(env2, message));
        } else {
            jni::CallMethod<void>(*env2, deleteCallback, *offlineRegionDeleteOnDeleteId);
        }

        // Delete global refs and detach when we're done
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(deleteCallback));
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

void updateOfflineRegionMetadata(JNIEnv *env, jni::jobject* offlineRegion_, jni::jarray<jbyte>* metadata_, jni::jobject* updateCallback) {
    // Offline region
    mbgl::OfflineRegion* offlineRegion = getOfflineRegionPeer(env, offlineRegion_);

    // File source
    jni::jobject* jmanager = jni::GetField<jni::jobject*>(*env, offlineRegion_, *offlineRegionOfflineManagerId);
    jlong defaultFileSourcePtr = jni::GetField<jlong>(*env, jmanager, *offlineManagerClassPtrId);
    mbgl::DefaultFileSource *defaultFileSource = reinterpret_cast<mbgl::DefaultFileSource *>(defaultFileSourcePtr);

    // Id conversion
    int64_t id = offlineRegion->getID();

    // Metadata
    mbgl::OfflineRegionMetadata metadata;
    if (metadata_ != nullptr) {
        metadata = metadata_from_java(env, *metadata_);
    }

    // Makes sure the objects don't get GC'ed
    updateCallback = jni::NewGlobalRef(*env, updateCallback).release();

    // Launch updateCallback
    defaultFileSource->updateOfflineMetadata(id, metadata, [updateCallback] (std::exception_ptr error, mbgl::optional<mbgl::OfflineRegionMetadata> data) mutable {
        // Reattach, the callback comes from a different thread
        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Offline Thread");
        if (renderDetach) {
            mbgl::Log::Debug(mbgl::Event::JNI, "Attached.");
        }

        if (error) {
            std::string message = mbgl::util::toString(error);
            jni::CallMethod<void>(*env2, updateCallback, *updateMetadataOnErrorMethodId, std_string_to_jstring(env2, message));
        } else if (data) {
            jni::jarray<jbyte>* jmetadata = metadata_from_native(env2, *data);
            jni::CallMethod<void>(*env2, updateCallback, *updateMetadataOnUpdateMethodId, jmetadata);
        }

        // Delete global refs and detach when we're done
        jni::DeleteGlobalRef(*env2, jni::UniqueGlobalRef<jni::jobject>(updateCallback));
        detach_jni_thread(theJVM, &env2, renderDetach);
    });
}

// Offline calls end

} // anonymous

namespace mbgl {
namespace android {

void registerNatives(JavaVM *vm) {
    theJVM = vm;

    jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);

    static mbgl::util::RunLoop mainRunLoop;

    mbgl::android::RegisterNativeHTTPRequest(env);

    java::registerNatives(env);
    registerNativeLayers(env);
    registerNativeSources(env);
    ConnectivityListener::registerNative(env);

    latLngClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/geometry/LatLng");
    latLngClass = jni::NewGlobalRef(env, latLngClass).release();
    latLngConstructorId = &jni::GetMethodID(env, *latLngClass, "<init>", "(DD)V");
    latLngLatitudeId = &jni::GetFieldID(env, *latLngClass, "latitude", "D");
    latLngLongitudeId = &jni::GetFieldID(env, *latLngClass, "longitude", "D");

    latLngBoundsClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/geometry/LatLngBounds");
    latLngBoundsClass = jni::NewGlobalRef(env, latLngBoundsClass).release();
    latLngBoundsConstructorId = &jni::GetMethodID(env, *latLngBoundsClass, "<init>", "(DDDD)V");
    latLngBoundsLatNorthId = &jni::GetFieldID(env, *latLngBoundsClass, "mLatNorth", "D");
    latLngBoundsLatSouthId = &jni::GetFieldID(env, *latLngBoundsClass, "mLatSouth", "D");
    latLngBoundsLonEastId = &jni::GetFieldID(env, *latLngBoundsClass, "mLonEast", "D");
    latLngBoundsLonWestId = &jni::GetFieldID(env, *latLngBoundsClass, "mLonWest", "D");

    iconClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/annotations/Icon");
    iconClass = jni::NewGlobalRef(env, iconClass).release();
    iconIdId = &jni::GetFieldID(env, *iconClass, "mId", "Ljava/lang/String;");

    markerClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/annotations/Marker");
    markerClass = jni::NewGlobalRef(env, markerClass).release();
    markerPositionId = &jni::GetFieldID(env, *markerClass, "position", "Lcom/mapbox/mapboxsdk/geometry/LatLng;");
    markerIconId = &jni::GetFieldID(env, *markerClass, "icon", "Lcom/mapbox/mapboxsdk/annotations/Icon;");
    markerIdId = &jni::GetFieldID(env, *markerClass, "id", "J");

    polylineClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/annotations/Polyline");
    polylineClass = jni::NewGlobalRef(env, polylineClass).release();
    polylineAlphaId = &jni::GetFieldID(env, *polylineClass, "alpha", "F");
    polylineColorId = &jni::GetFieldID(env, *polylineClass, "color", "I");
    polylineWidthId = &jni::GetFieldID(env, *polylineClass, "width", "F");
    polylinePointsId = &jni::GetFieldID(env, *polylineClass, "points", "Ljava/util/List;");

    polygonClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/annotations/Polygon");
    polygonClass = jni::NewGlobalRef(env, polygonClass).release();
    polygonAlphaId = &jni::GetFieldID(env, *polygonClass, "alpha", "F");
    polygonFillColorId = &jni::GetFieldID(env, *polygonClass, "fillColor", "I");
    polygonStrokeColorId = &jni::GetFieldID(env, *polygonClass, "strokeColor", "I");
    polygonPointsId = &jni::GetFieldID(env, *polygonClass, "points", "Ljava/util/List;");

    jni::jclass* listClass = &jni::FindClass(env, "java/util/List");
    listToArrayId = &jni::GetMethodID(env, *listClass, "toArray", "()[Ljava/lang/Object;");

    arrayListClass = &jni::FindClass(env, "java/util/ArrayList");
    arrayListClass = jni::NewGlobalRef(env, arrayListClass).release();
    arrayListConstructorId = &jni::GetMethodID(env, *arrayListClass, "<init>", "()V");
    arrayListAddId = &jni::GetMethodID(env, *arrayListClass, "add", "(Ljava/lang/Object;)Z");

    projectedMetersClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/geometry/ProjectedMeters");
    projectedMetersClass = jni::NewGlobalRef(env, projectedMetersClass).release();
    projectedMetersConstructorId = &jni::GetMethodID(env, *projectedMetersClass, "<init>", "(DD)V");
    projectedMetersNorthingId = &jni::GetFieldID(env, *projectedMetersClass, "northing", "D");
    projectedMetersEastingId = &jni::GetFieldID(env, *projectedMetersClass, "easting", "D");

    pointFClass = &jni::FindClass(env, "android/graphics/PointF");
    pointFClass = jni::NewGlobalRef(env, pointFClass).release();
    pointFConstructorId = &jni::GetMethodID(env, *pointFClass, "<init>", "(FF)V");
    pointFXId = &jni::GetFieldID(env, *pointFClass, "x", "F");
    pointFYId = &jni::GetFieldID(env, *pointFClass, "y", "F");

    rectFClass = &jni::FindClass(env, "android/graphics/RectF");
    rectFClass = jni::NewGlobalRef(env, rectFClass).release();
    rectFConstructorId = &jni::GetMethodID(env, *rectFClass, "<init>", "()V");
    rectFLeftId = &jni::GetFieldID(env, *rectFClass, "left", "F");
    rectFRightId = &jni::GetFieldID(env, *rectFClass, "right", "F");
    rectFTopId = &jni::GetFieldID(env, *rectFClass, "top", "F");
    rectFBottomId = &jni::GetFieldID(env, *rectFClass, "bottom", "F");

    jni::jclass& nativeMapViewClass = jni::FindClass(env, "com/mapbox/mapboxsdk/maps/NativeMapView");

    onInvalidateId = &jni::GetMethodID(env, nativeMapViewClass, "onInvalidate", "()V");
    onMapChangedId = &jni::GetMethodID(env, nativeMapViewClass, "onMapChanged", "(I)V");
    onFpsChangedId = &jni::GetMethodID(env, nativeMapViewClass, "onFpsChanged", "(D)V");
    onSnapshotReadyId = &jni::GetMethodID(env, nativeMapViewClass, "onSnapshotReady","([B)V");

    #define MAKE_NATIVE_METHOD(name, sig) jni::MakeNativeMethod<decltype(name), name>( #name, sig )

    jni::RegisterNatives(env, nativeMapViewClass,
        MAKE_NATIVE_METHOD(nativeCreate, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;FIJ)J"),
        MAKE_NATIVE_METHOD(nativeDestroy, "(J)V"),
        MAKE_NATIVE_METHOD(nativeInitializeDisplay, "(J)V"),
        MAKE_NATIVE_METHOD(nativeTerminateDisplay, "(J)V"),
        MAKE_NATIVE_METHOD(nativeInitializeContext, "(J)V"),
        MAKE_NATIVE_METHOD(nativeTerminateContext, "(J)V"),
        MAKE_NATIVE_METHOD(nativeCreateSurface, "(JLandroid/view/Surface;)V"),
        MAKE_NATIVE_METHOD(nativeDestroySurface, "(J)V"),
        MAKE_NATIVE_METHOD(nativeUpdate, "(J)V"),
        MAKE_NATIVE_METHOD(nativeRender, "(J)V"),
        MAKE_NATIVE_METHOD(nativeViewResize, "(JII)V"),
        MAKE_NATIVE_METHOD(nativeFramebufferResize, "(JII)V"),
        MAKE_NATIVE_METHOD(nativeAddClass, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeRemoveClass, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeHasClass, "(JLjava/lang/String;)Z"),
        MAKE_NATIVE_METHOD(nativeSetClasses, "(JLjava/util/List;)V"),
        MAKE_NATIVE_METHOD(nativeGetClasses, "(J)Ljava/util/List;"),
        MAKE_NATIVE_METHOD(nativeSetStyleUrl, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeGetStyleUrl, "(J)Ljava/lang/String;"),
        MAKE_NATIVE_METHOD(nativeSetStyleJson, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeGetStyleJson, "(J)Ljava/lang/String;"),
        MAKE_NATIVE_METHOD(nativeSetAccessToken, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeGetAccessToken, "(J)Ljava/lang/String;"),
        MAKE_NATIVE_METHOD(nativeCancelTransitions, "(J)V"),
        MAKE_NATIVE_METHOD(nativeSetGestureInProgress, "(JZ)V"),
        MAKE_NATIVE_METHOD(nativeMoveBy, "(JDDJ)V"),
        MAKE_NATIVE_METHOD(nativeSetLatLng, "(JDDJ)V"),
        MAKE_NATIVE_METHOD(nativeGetLatLng, "(J)Lcom/mapbox/mapboxsdk/geometry/LatLng;"),
        MAKE_NATIVE_METHOD(nativeResetPosition, "(J)V"),
        MAKE_NATIVE_METHOD(nativeGetCameraValues, "(J)[D"),
        MAKE_NATIVE_METHOD(nativeGetPitch, "(J)D"),
        MAKE_NATIVE_METHOD(nativeSetPitch, "(JDJ)V"),
        MAKE_NATIVE_METHOD(nativeScaleBy, "(JDDDJ)V"),
        MAKE_NATIVE_METHOD(nativeSetScale, "(JDDDJ)V"),
        MAKE_NATIVE_METHOD(nativeGetScale, "(J)D"),
        MAKE_NATIVE_METHOD(nativeSetZoom, "(JDJ)V"),
        MAKE_NATIVE_METHOD(nativeGetZoom, "(J)D"),
        MAKE_NATIVE_METHOD(nativeResetZoom, "(J)V"),
        MAKE_NATIVE_METHOD(nativeGetMinZoom, "(J)D"),
        MAKE_NATIVE_METHOD(nativeSetMinZoom, "(JD)V"),
        MAKE_NATIVE_METHOD(nativeGetMaxZoom, "(J)D"),
        MAKE_NATIVE_METHOD(nativeSetMaxZoom, "(JD)V"),
        MAKE_NATIVE_METHOD(nativeRotateBy, "(JDDDDJ)V"),
        MAKE_NATIVE_METHOD(nativeSetBearing, "(JDJ)V"),
        MAKE_NATIVE_METHOD(nativeSetBearingXY, "(JDDD)V"),
        MAKE_NATIVE_METHOD(nativeGetBearing, "(J)D"),
        MAKE_NATIVE_METHOD(nativeResetNorth, "(J)V"),
        MAKE_NATIVE_METHOD(nativeAddMarkers, "(J[Lcom/mapbox/mapboxsdk/annotations/Marker;)[J"),
        MAKE_NATIVE_METHOD(nativeAddPolylines, "(J[Lcom/mapbox/mapboxsdk/annotations/Polyline;)[J"),
        MAKE_NATIVE_METHOD(nativeAddPolygons, "(J[Lcom/mapbox/mapboxsdk/annotations/Polygon;)[J"),
        MAKE_NATIVE_METHOD(nativeUpdateMarker, "(JJDDLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeUpdatePolygon, "(JJLcom/mapbox/mapboxsdk/annotations/Polygon;)V"),
        MAKE_NATIVE_METHOD(nativeUpdatePolyline, "(JJLcom/mapbox/mapboxsdk/annotations/Polyline;)V"),
        MAKE_NATIVE_METHOD(nativeRemoveAnnotations, "(J[J)V"),
        MAKE_NATIVE_METHOD(nativeQueryPointAnnotations, "(JLandroid/graphics/RectF;)[J"),
        MAKE_NATIVE_METHOD(nativeAddAnnotationIcon, "(JLjava/lang/String;IIF[B)V"),
        MAKE_NATIVE_METHOD(nativeSetVisibleCoordinateBounds, "(J[Lcom/mapbox/mapboxsdk/geometry/LatLng;Landroid/graphics/RectF;DJ)V"),
        MAKE_NATIVE_METHOD(nativeOnLowMemory, "(J)V"),
        MAKE_NATIVE_METHOD(nativeSetDebug, "(JZ)V"),
        MAKE_NATIVE_METHOD(nativeToggleDebug, "(J)V"),
        MAKE_NATIVE_METHOD(nativeGetDebug, "(J)Z"),
        MAKE_NATIVE_METHOD(nativeIsFullyLoaded, "(J)Z"),
        MAKE_NATIVE_METHOD(nativeSetReachability, "(JZ)V"),
        MAKE_NATIVE_METHOD(nativeGetMetersPerPixelAtLatitude, "(JDD)D"),
        MAKE_NATIVE_METHOD(nativeProjectedMetersForLatLng, "(JDD)Lcom/mapbox/mapboxsdk/geometry/ProjectedMeters;"),
        MAKE_NATIVE_METHOD(nativeLatLngForProjectedMeters, "(JDD)Lcom/mapbox/mapboxsdk/geometry/LatLng;"),
        MAKE_NATIVE_METHOD(nativePixelForLatLng, "(JDD)Landroid/graphics/PointF;"),
        MAKE_NATIVE_METHOD(nativeLatLngForPixel, "(JFF)Lcom/mapbox/mapboxsdk/geometry/LatLng;"),
        MAKE_NATIVE_METHOD(nativeGetTopOffsetPixelsForAnnotationSymbol, "(JLjava/lang/String;)D"),
        MAKE_NATIVE_METHOD(nativeJumpTo, "(JDDDDD)V"),
        MAKE_NATIVE_METHOD(nativeEaseTo, "(JDDDJDDZ)V"),
        MAKE_NATIVE_METHOD(nativeFlyTo, "(JDDDJDD)V"),
        MAKE_NATIVE_METHOD(nativeGetTransitionDuration, "(J)J"),
        MAKE_NATIVE_METHOD(nativeSetTransitionDuration, "(JJ)V"),
        MAKE_NATIVE_METHOD(nativeGetTransitionDelay, "(J)J"),
        MAKE_NATIVE_METHOD(nativeSetTransitionDelay, "(JJ)V"),
        MAKE_NATIVE_METHOD(nativeGetLayer, "(JLjava/lang/String;)Lcom/mapbox/mapboxsdk/style/layers/Layer;"),
        MAKE_NATIVE_METHOD(nativeAddLayer, "(JJLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeRemoveLayerById, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeRemoveLayer, "(JJ)V"),
        MAKE_NATIVE_METHOD(nativeGetSource, "(JLjava/lang/String;)Lcom/mapbox/mapboxsdk/style/sources/Source;"),
        MAKE_NATIVE_METHOD(nativeAddSource, "(JJ)V"),
        MAKE_NATIVE_METHOD(nativeRemoveSourceById, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeRemoveSource, "(JJ)V"),
        MAKE_NATIVE_METHOD(nativeAddImage, "(JLjava/lang/String;IIF[B)V"),
        MAKE_NATIVE_METHOD(nativeRemoveImage, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(nativeSetContentPadding, "(JDDDD)V"),
        MAKE_NATIVE_METHOD(nativeScheduleTakeSnapshot, "(J)V"),
        MAKE_NATIVE_METHOD(nativeQueryRenderedFeaturesForPoint, "(JFF[Ljava/lang/String;)[Lcom/mapbox/services/commons/geojson/Feature;"),
        MAKE_NATIVE_METHOD(nativeQueryRenderedFeaturesForBox, "(JFFFF[Ljava/lang/String;)[Lcom/mapbox/services/commons/geojson/Feature;"),
        MAKE_NATIVE_METHOD(nativeSetAPIBaseURL, "(JLjava/lang/String;)V")
    );

    // Offline begin

    struct OfflineManager {
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager"; }

        struct ListOfflineRegionsCallback {
            static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager$ListOfflineRegionsCallback"; }
        };

        struct CreateOfflineRegionsCallback {
            static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager$CreateOfflineRegionCallback"; }
        };
    };

    struct OfflineRegion {
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion"; }
    };

    jni::Class<OfflineManager> offlineManagerClass = jni::Class<OfflineManager>::Find(env);
    offlineManagerClassPtrId = &jni::GetFieldID(env, offlineManagerClass, "mDefaultFileSourcePtr", "J");

    jni::RegisterNatives(env, offlineManagerClass,
        MAKE_NATIVE_METHOD(createDefaultFileSource, "(Ljava/lang/String;Ljava/lang/String;J)J"),
        MAKE_NATIVE_METHOD(setAccessToken, "(JLjava/lang/String;)V"),
        MAKE_NATIVE_METHOD(getAccessToken, "(J)Ljava/lang/String;"),
        MAKE_NATIVE_METHOD(listOfflineRegions, "(JLcom/mapbox/mapboxsdk/offline/OfflineManager$ListOfflineRegionsCallback;)V"),
        MAKE_NATIVE_METHOD(createOfflineRegion, "(JLcom/mapbox/mapboxsdk/offline/OfflineRegionDefinition;[BLcom/mapbox/mapboxsdk/offline/OfflineManager$CreateOfflineRegionCallback;)V"),
        MAKE_NATIVE_METHOD(setOfflineMapboxTileCountLimit, "(JJ)V")
    );

    jni::Class<OfflineManager::ListOfflineRegionsCallback> listOfflineRegionsCallbackClass = jni::Class<OfflineManager::ListOfflineRegionsCallback>::Find(env);
    listOnListMethodId = &jni::GetMethodID(env, listOfflineRegionsCallbackClass, "onList", "([Lcom/mapbox/mapboxsdk/offline/OfflineRegion;)V");
    listOnErrorMethodId = &jni::GetMethodID(env, listOfflineRegionsCallbackClass, "onError", "(Ljava/lang/String;)V");

    jni::Class<OfflineManager::CreateOfflineRegionsCallback> createOfflineRegionCallbackClass = jni::Class<OfflineManager::CreateOfflineRegionsCallback>::Find(env);
    createOnCreateMethodId = &jni::GetMethodID(env, createOfflineRegionCallbackClass, "onCreate", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion;)V");
    createOnErrorMethodId = &jni::GetMethodID(env, createOfflineRegionCallbackClass, "onError", "(Ljava/lang/String;)V");

    offlineRegionClass = &jni::FindClass(env, OfflineRegion::Name());
    offlineRegionClass = jni::NewGlobalRef(env, offlineRegionClass).release();
    offlineRegionConstructorId = &jni::GetMethodID(env, *offlineRegionClass, "<init>", "()V");
    offlineRegionOfflineManagerId = &jni::GetFieldID(env, *offlineRegionClass, "offlineManager", "Lcom/mapbox/mapboxsdk/offline/OfflineManager;");
    offlineRegionIdId = &jni::GetFieldID(env, *offlineRegionClass, "mId", "J");
    offlineRegionDefinitionId = &jni::GetFieldID(env, *offlineRegionClass, "mDefinition", "Lcom/mapbox/mapboxsdk/offline/OfflineRegionDefinition;");
    offlineRegionMetadataId = &jni::GetFieldID(env, *offlineRegionClass, "mMetadata", "[B");
    offlineRegionPtrId = &jni::GetFieldID(env, *offlineRegionClass, "mOfflineRegionPtr", "J");

    jni::RegisterNatives(env, *offlineRegionClass,
        MAKE_NATIVE_METHOD(destroyOfflineRegion, "()V"),
        MAKE_NATIVE_METHOD(setOfflineRegionObserver, "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionObserver;)V"),
        MAKE_NATIVE_METHOD(setOfflineRegionDownloadState, "(I)V"),
        MAKE_NATIVE_METHOD(getOfflineRegionStatus, "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback;)V"),
        MAKE_NATIVE_METHOD(deleteOfflineRegion, "(Lcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback;)V"),
        MAKE_NATIVE_METHOD(updateOfflineRegionMetadata, "([BLcom/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionUpdateMetadataCallback;)V")
    );

    // This needs to be updated once we support more than one type of region definition
    offlineRegionDefinitionClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineTilePyramidRegionDefinition");
    offlineRegionDefinitionClass = jni::NewGlobalRef(env, offlineRegionDefinitionClass).release();
    offlineRegionDefinitionConstructorId = &jni::GetMethodID(env, *offlineRegionDefinitionClass, "<init>", "()V");
    offlineRegionDefinitionStyleURLId = &jni::GetFieldID(env, *offlineRegionDefinitionClass, "styleURL", "Ljava/lang/String;");
    offlineRegionDefinitionBoundsId = &jni::GetFieldID(env, *offlineRegionDefinitionClass, "bounds", "Lcom/mapbox/mapboxsdk/geometry/LatLngBounds;");
    offlineRegionDefinitionMinZoomId = &jni::GetFieldID(env, *offlineRegionDefinitionClass, "minZoom", "D");
    offlineRegionDefinitionMaxZoomId = &jni::GetFieldID(env, *offlineRegionDefinitionClass, "maxZoom", "D");
    offlineRegionDefinitionPixelRatioId = &jni::GetFieldID(env, *offlineRegionDefinitionClass, "pixelRatio", "F");

    jni::jclass* offlineRegionObserverClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionObserver");
    offlineRegionObserveronStatusChangedId = &jni::GetMethodID(env, *offlineRegionObserverClass, "onStatusChanged", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionStatus;)V");
    offlineRegionObserveronErrorId = &jni::GetMethodID(env, *offlineRegionObserverClass, "onError", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionError;)V");
    offlineRegionObserveronLimitId = &jni::GetMethodID(env, *offlineRegionObserverClass, "mapboxTileCountLimitExceeded", "(J)V");

    offlineRegionStatusClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegionStatus");
    offlineRegionStatusClass = jni::NewGlobalRef(env, offlineRegionStatusClass).release();
    offlineRegionStatusConstructorId = &jni::GetMethodID(env, *offlineRegionStatusClass, "<init>", "()V");
    offlineRegionStatusDownloadStateId = &jni::GetFieldID(env, *offlineRegionStatusClass, "downloadState", "I");
    offlineRegionStatusCompletedResourceCountId = &jni::GetFieldID(env, *offlineRegionStatusClass, "completedResourceCount", "J");
    offlineRegionStatusCompletedResourceSizeId = &jni::GetFieldID(env, *offlineRegionStatusClass, "completedResourceSize", "J");
    offlineRegionStatusCompletedTileCountId = &jni::GetFieldID(env, *offlineRegionStatusClass, "completedTileCount", "J");
    offlineRegionStatusCompletedTileSizeId = &jni::GetFieldID(env, *offlineRegionStatusClass, "completedTileSize", "J");
    offlineRegionStatusRequiredResourceCountId = &jni::GetFieldID(env, *offlineRegionStatusClass, "requiredResourceCount", "J");
    offlineRegionStatusRequiredResourceCountIsPreciseId = &jni::GetFieldID(env, *offlineRegionStatusClass, "requiredResourceCountIsPrecise", "Z");

    offlineRegionErrorClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegionError");
    offlineRegionErrorClass = jni::NewGlobalRef(env, offlineRegionErrorClass).release();
    offlineRegionErrorConstructorId = &jni::GetMethodID(env, *offlineRegionErrorClass, "<init>", "()V");
    offlineRegionErrorReasonId = &jni::GetFieldID(env, *offlineRegionErrorClass, "reason", "Ljava/lang/String;");
    offlineRegionErrorMessageId = &jni::GetFieldID(env, *offlineRegionErrorClass, "message", "Ljava/lang/String;");

    jni::jclass* offlineRegionStatusCallbackClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback");
    offlineRegionStatusOnStatusId = &jni::GetMethodID(env, *offlineRegionStatusCallbackClass, "onStatus", "(Lcom/mapbox/mapboxsdk/offline/OfflineRegionStatus;)V");
    offlineRegionStatusOnErrorId = &jni::GetMethodID(env, *offlineRegionStatusCallbackClass, "onError", "(Ljava/lang/String;)V");

    jni::jclass* offlineRegionDeleteCallbackClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback");
    offlineRegionDeleteOnDeleteId = &jni::GetMethodID(env, *offlineRegionDeleteCallbackClass, "onDelete", "()V");
    offlineRegionDeleteOnErrorId = &jni::GetMethodID(env, *offlineRegionDeleteCallbackClass, "onError", "(Ljava/lang/String;)V");

    jni::jclass* offlineRegionUpdateMetadataCallbackClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionUpdateMetadataCallback");
    updateMetadataOnUpdateMethodId = &jni::GetMethodID(env, *offlineRegionUpdateMetadataCallbackClass, "onUpdate", "([B)V");
    updateMetadataOnErrorMethodId = &jni::GetMethodID(env, *offlineRegionUpdateMetadataCallbackClass, "onError", "(Ljava/lang/String;)V");

    // Offline end

    char release[PROP_VALUE_MAX] = "";
    __system_property_get("ro.build.version.release", release);
    androidRelease = std::string(release);
}
    
} // android
} // mbgl
