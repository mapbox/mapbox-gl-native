#include <cstdint>
#include <cinttypes>
#include <cassert>

#include <string>
#include <locale>
#include <codecvt>
#include <array>
#include <vector>

#include <jni.h>

#include <android/native_window_jni.h>
#include <sys/system_properties.h>

#include <mbgl/android/jni.hpp>
#include <mbgl/android/native_map_view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/annotation/sprite_image.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/network_status.hpp>

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace mbgl {
namespace android {

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

jclass markerClass = nullptr;
jmethodID markerConstructorId = nullptr;
jfieldID markerPositionId = nullptr;
jfieldID markerSpriteId = nullptr;

jclass polylineClass = nullptr;
jmethodID polylineConstructorId = nullptr;
jfieldID polylineAlphaId = nullptr;
jfieldID polylineVisibleId = nullptr;
jfieldID polylineColorId = nullptr;
jfieldID polylineWidthId = nullptr;
jfieldID polylinePointsId = nullptr;

jclass polygonClass = nullptr;
jmethodID polygonConstructorId = nullptr;
jfieldID polygonAlphaId = nullptr;
jfieldID polygonVisibleId = nullptr;
jfieldID polygonFillColorId = nullptr;
jfieldID polygonStrokeColorId = nullptr;
jfieldID polygonStrokeWidthId = nullptr;
jfieldID polygonPointsId = nullptr;
jfieldID polygonHolesId = nullptr;


jclass latLngZoomClass = nullptr;
jmethodID latLngZoomConstructorId = nullptr;
jfieldID latLngZoomLatitudeId = nullptr;
jfieldID latLngZoomLongitudeId = nullptr;
jfieldID latLngZoomZoomId = nullptr;

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

bool throw_error(JNIEnv *env, const char *msg) {
    if (env->ThrowNew(runtimeExceptionClass, msg) < 0) {
        env->ExceptionDescribe();
        return false;
    }

    return true;
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

    jobjectArray array =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return vector;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return vector;
    }

    for (jsize i = 0; i < len; i++) {
        jstring jstr = reinterpret_cast<jstring>(env->GetObjectArrayElement(array, i));
        if (jstr == nullptr) {
            env->ExceptionDescribe();
            return vector;
        }

        vector.push_back(std_string_from_jstring(env, jstr));
    }

    return vector;
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

    jobjectArray array =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return segment;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return segment;
    }

    segment.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject latLng = reinterpret_cast<jobject>(env->GetObjectArrayElement(array, i));
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
    env->DeleteLocalRef(array);
    return segment;
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
}
}

namespace {

using namespace mbgl::android;

jlong JNICALL
nativeCreate(JNIEnv *env, jobject obj, jstring cachePath_, jstring dataPath_, jstring apkPath_, jfloat pixelRatio) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreate");
    cachePath = std_string_from_jstring(env, cachePath_);
    dataPath = std_string_from_jstring(env, dataPath_);
    apkPath = std_string_from_jstring(env, apkPath_);
    NativeMapView *nativeMapView = new NativeMapView(env, obj, pixelRatio);
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
        throw_error(env, "Unable to initialize GL display.");
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
        throw_error(env, "Unable to initialize GL context.");
    }
}

void JNICALL nativeTerminateContext(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateContext");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->terminateContext();
}

void JNICALL
nativeCreateSurface(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject surface) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreateSurface");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    try {
        nativeMapView->createSurface(ANativeWindow_fromSurface(env, surface));
    } catch(const std::exception& e) {
        throw_error(env, "Unable to create GL surface.");
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
    nativeMapView->getMap().update();
}

void JNICALL nativeOnInvalidate(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean inProgress) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeOnInvalidate");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->onInvalidate(inProgress);
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

void JNICALL
nativeSetClasses(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject classes) {
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
    nativeMapView->getMap().setDefaultTransitionDuration(std::chrono::milliseconds(duration));
}

jlong JNICALL nativeGetDefaultTransitionDuration(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetDefaultTransitionDuration");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return std::chrono::duration_cast<std::chrono::milliseconds>(nativeMapView->getMap().getDefaultTransitionDuration()).count();
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

void JNICALL
nativeSetAccessToken(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jstring accessToken) {
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

void JNICALL nativeMoveBy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble dx, jdouble dy,
                          jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeMoveBy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().moveBy(dx, dy, std::chrono::milliseconds(duration));
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

    nativeMapView->getMap().setLatLng(mbgl::LatLng(latitude, longitude), std::chrono::milliseconds(duration));
}

void JNICALL nativeSetSprite(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
        jstring symbol, jint width, jint height, jfloat scale, jbyteArray jpixels) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetSprite");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    const std::string symbolName = std_string_from_jstring(env, symbol);

    jbyte* pixelData = env->GetByteArrayElements(jpixels, nullptr);
    std::string pixels((char*)pixelData, width * height * 4);
    env->ReleaseByteArrayElements(jpixels, pixelData, JNI_ABORT);

    auto spriteImage = std::make_shared<mbgl::SpriteImage>(
        uint16_t(width),
        uint16_t(height),
        float(scale),
        std::move(pixels));

    nativeMapView->getMap().setSprite(symbolName, spriteImage);
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

    jstring jsprite = (jstring)env->GetObjectField(marker, markerSpriteId);
    std::string sprite = std_string_from_jstring(env, jsprite);

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
    return (jlong) nativeMapView->getMap().addPointAnnotation(mbgl::PointAnnotation(mbgl::LatLng(latitude, longitude), sprite));
}

jlong JNICALL nativeAddPolyline(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject polyline) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolyline");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    // ***** Java fields ***** //
    // float alpha;
    // boolean visible;
    // int color
    // float width
    // List<LatLng> points

    jfloat alpha = env->GetFloatField(polyline, polylineAlphaId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jboolean visible = env->GetBooleanField(polyline, polylineVisibleId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }
    visible = JNI_TRUE;

    jint color = env->GetIntField(polyline, polylineColorId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    int r = (color>>16)&0xFF;
    int g = (color>>8)&0xFF;
    int b = (color)&0xFF;
    int a = (color>>24)&0xFF;

    jfloat width = env->GetFloatField(polyline, polylineWidthId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    mbgl::StyleProperties shapeProperties;
    mbgl::LineProperties lineProperties;
    lineProperties.opacity = alpha;
    lineProperties.color = {{ (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f }};
    lineProperties.width = width;
    shapeProperties.set<mbgl::LineProperties>(lineProperties);

    jobject points = env->GetObjectField(polyline, polylinePointsId);
    mbgl::AnnotationSegment segment = annotation_segment_from_latlng_jlist(env, points);

    std::vector<mbgl::ShapeAnnotation> shapes;
    shapes.emplace_back(mbgl::AnnotationSegments {{ segment }}, shapeProperties);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    uint32_t id = shapeAnnotationIDs.at(0);
    return (jlong) id;
}


std::pair<mbgl::AnnotationSegment, mbgl::StyleProperties> readPolygon(JNIEnv *env, jobject polygon) {
    // ***** Java fields ***** //
    // float alpha;
    // boolean visible;
    // int fillColor
    // int strokeColor
    // float strokeWidth
    // List<LatLng> points
    // List<List<LatLng>> holes

    jfloat alpha = env->GetFloatField(polygon, polygonAlphaId);
    jboolean visible = env->GetBooleanField(polygon, polygonVisibleId);
    visible = JNI_TRUE;
    jint fillColor = env->GetIntField(polygon, polygonFillColorId);
    jint strokeColor = env->GetIntField(polygon, polygonStrokeColorId);

    int rF = (fillColor>>16)&0xFF;
    int gF = (fillColor>>8)&0xFF;
    int bF = (fillColor)&0xFF;
    int aF = (fillColor>>24)&0xFF;

    int rS = (strokeColor>>16)&0xFF;
    int gS = (strokeColor>>8)&0xFF;
    int bS = (strokeColor)&0xFF;
    int aS = (strokeColor>>24)&0xFF;

    // jfloat strokeWidth = env->GetFloatField(polygon, polygonStrokeWidthId);
    // if (env->ExceptionCheck()) {
    //     env->ExceptionDescribe();
    //     return -1;
    // }

    mbgl::StyleProperties shapeProperties;
    mbgl::FillProperties fillProperties;
    fillProperties.opacity = alpha;
    fillProperties.stroke_color = {{ (float)rS / 255.0f, (float)gS / 255.0f, (float)bS / 255.0f, (float)aS / 255.0f }};
    fillProperties.fill_color = {{ (float)rF / 255.0f, (float)gF / 255.0f, (float)bF / 255.0f, (float)aF / 255.0f }};
    shapeProperties.set<mbgl::FillProperties>(fillProperties);

    jobject points = env->GetObjectField(polygon, polygonPointsId);
    mbgl::AnnotationSegment segment = annotation_segment_from_latlng_jlist(env, points);
    env->DeleteLocalRef(points);

    return std::make_pair(segment, shapeProperties);
}


jlong JNICALL nativeAddPolygon(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jobject polygon) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeAddPolygon");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<mbgl::ShapeAnnotation> shapes;
    std::pair<mbgl::AnnotationSegment, mbgl::StyleProperties> segment = readPolygon(env, polygon);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    shapes.emplace_back(mbgl::AnnotationSegments {{ segment.first }}, segment.second);

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    uint32_t id = shapeAnnotationIDs.at(0);
    return (jlong) id;
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

    jobjectArray array =
        reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, listToArrayId));
    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return nullptr;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return nullptr;
    }

    shapes.reserve(len);

    for (jsize i = 0; i < len; i++) {
        jobject polygon = reinterpret_cast<jobject>(env->GetObjectArrayElement(array, i));

        std::pair<mbgl::AnnotationSegment, mbgl::StyleProperties> segment = readPolygon(env, polygon);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }

        shapes.emplace_back(mbgl::AnnotationSegments {{ segment.first }}, segment.second);

        env->DeleteLocalRef(polygon);
    }

    std::vector<uint32_t> shapeAnnotationIDs = nativeMapView->getMap().addShapeAnnotations(shapes);
    return std_vector_uint_to_jobject(env, shapeAnnotationIDs);
}


void JNICALL nativeRemoveAnnotation(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jlong annotationId) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveAnnotation");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().removeAnnotation((uint32_t)annotationId);
}

void JNICALL nativeRemoveAnnotations(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jlongArray array) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRemoveAnnotations");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    std::vector<uint32_t> ids;

    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return;
    }

    ids.reserve(len);
    jlong* jids = env->GetLongArrayElements(array, nullptr);

    for (jsize i = 0; i < len; i++) {
        if(jids[i] == -1L)
            continue;
        ids.push_back((uint32_t) jids[i]);
    }

    env->ReleaseLongArrayElements(array, jids, JNI_ABORT);

    nativeMapView->getMap().removeAnnotations(ids);
}

jobject JNICALL nativeGetLatLng(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetLatLng");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng();

    jobject ret = env->NewObject(latLngClass, latLngConstructorId, latLng.latitude, latLng.longitude);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeResetPosition(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetPosition");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetPosition();
}

void JNICALL nativeScaleBy(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble ds, jdouble cx,
                           jdouble cy, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeScaleBy");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().scaleBy(ds, cx, cy, std::chrono::milliseconds(duration));
}

void JNICALL nativeSetScale(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble scale,
                            jdouble cx, jdouble cy, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetScale");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setScale(scale, cx, cy, std::chrono::milliseconds(duration));
}

jdouble JNICALL nativeGetScale(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetScale");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getScale();
}

void JNICALL
nativeSetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble zoom, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setZoom(zoom, std::chrono::milliseconds(duration));
}

jdouble JNICALL nativeGetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getZoom();
}

void JNICALL nativeSetLatLngZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr,
                                 jobject latLngZoom, jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetLatLngZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);

    jdouble latitude = env->GetDoubleField(latLngZoom, latLngZoomLatitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble longitude = env->GetDoubleField(latLngZoom, latLngZoomLongitudeId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    jdouble zoom = env->GetDoubleField(latLngZoom, latLngZoomZoomId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    nativeMapView->getMap().setLatLngZoom(mbgl::LatLng(latitude, longitude), zoom, std::chrono::milliseconds(duration));
}

jobject JNICALL nativeGetLatLngZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetLatLngZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    mbgl::LatLng latLng = nativeMapView->getMap().getLatLng();
    jdouble zoom = nativeMapView->getMap().getZoom();

    jobject ret = env->NewObject(latLngZoomClass, latLngZoomConstructorId, latLng.longitude, latLng.latitude, zoom);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeResetZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().resetZoom();
}

jdouble JNICALL nativeGetMinZoom(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMinZoom");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getMinZoom();
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
    nativeMapView->getMap().rotateBy(sx, sy, ex, ey, std::chrono::milliseconds(duration));
}

void JNICALL nativeSetBearing(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble degrees,
                              jlong duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setBearing(degrees, std::chrono::milliseconds(duration));
}

void JNICALL nativeSetBearing(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jdouble degrees,
                              jdouble cx, jdouble cy) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setBearing(degrees, cx, cy);
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

void JNICALL nativeSetDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean debug) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().setDebug(debug);
    nativeMapView->enableFps(debug);
}

void JNICALL nativeToggleDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeToggleDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    nativeMapView->getMap().toggleDebug();
    nativeMapView->enableFps(nativeMapView->getMap().getDebug());
}

jboolean JNICALL nativeGetDebug(JNIEnv *env, jobject obj, jlong nativeMapViewPtr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetDebug");
    assert(nativeMapViewPtr != 0);
    NativeMapView *nativeMapView = reinterpret_cast<NativeMapView *>(nativeMapViewPtr);
    return nativeMapView->getMap().getDebug();
}

void JNICALL
nativeSetReachability(JNIEnv *env, jobject obj, jlong nativeMapViewPtr, jboolean status) {
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

    mbgl::vec2<double> pixel = nativeMapView->getMap().pixelForLatLng(mbgl::LatLng(latitude, longitude));

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

    mbgl::LatLng latLng = nativeMapView->getMap().latLngForPixel(mbgl::vec2<double>(x, y));

    jobject ret = env->NewObject(latLngClass, latLngConstructorId, latLng.latitude, latLng.longitude);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}
}

extern "C" {

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnLoad");

    JNIEnv *env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        return JNI_ERR;
    }

    latLngClass = env->FindClass("com/mapbox/mapboxgl/geometry/LatLng");
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

    markerClass = env->FindClass("com/mapbox/mapboxgl/annotations/Marker");
    if (markerClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerConstructorId = env->GetMethodID(markerClass, "<init>", "()V");
    if (markerConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerPositionId = env->GetFieldID(markerClass, "position", "Lcom/mapbox/mapboxgl/geometry/LatLng;");
    if (markerPositionId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }
    
    markerSpriteId = env->GetFieldID(markerClass, "sprite", "Ljava/lang/String;");
    if (markerSpriteId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineClass = env->FindClass("com/mapbox/mapboxgl/annotations/Polyline");
    if (polylineClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineConstructorId = env->GetMethodID(polylineClass, "<init>", "()V");
    if (polylineConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineAlphaId = env->GetFieldID(polylineClass, "alpha", "F");
    if (polylineAlphaId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineVisibleId = env->GetFieldID(polylineClass, "visible", "Z");
    if (polylineVisibleId == nullptr) {
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

    polygonClass = env->FindClass("com/mapbox/mapboxgl/annotations/Polygon");
    if (polygonClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonConstructorId = env->GetMethodID(polygonClass, "<init>", "()V");
    if (polygonConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonAlphaId = env->GetFieldID(polygonClass, "alpha", "F");
    if (polygonAlphaId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonVisibleId = env->GetFieldID(polygonClass, "visible", "Z");
    if (polygonVisibleId == nullptr) {
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

    polygonStrokeWidthId = env->GetFieldID(polygonClass, "strokeWidth", "F");
    if (polygonStrokeWidthId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonPointsId = env->GetFieldID(polygonClass, "points", "Ljava/util/List;");
    if (polygonPointsId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonHolesId = env->GetFieldID(polygonClass, "holes", "Ljava/util/List;");
    if (polygonHolesId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomClass = env->FindClass("com/mapbox/mapboxgl/geometry/LatLngZoom");
    if (latLngZoomClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomConstructorId = env->GetMethodID(latLngZoomClass, "<init>", "(DDD)V");
    if (latLngZoomConstructorId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomLatitudeId = env->GetFieldID(latLngZoomClass, "latitude", "D");
    if (latLngZoomLatitudeId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomLongitudeId = env->GetFieldID(latLngZoomClass, "longitude", "D");
    if (latLngZoomLongitudeId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomZoomId = env->GetFieldID(latLngZoomClass, "zoom", "D");
    if (latLngZoomZoomId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass nativeMapViewClass = env->FindClass("com/mapbox/mapboxgl/views/NativeMapView");
    if (nativeMapViewClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    onInvalidateId = env->GetMethodID(nativeMapViewClass, "onInvalidate", "()V");
    if (onInvalidateId == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    onMapChangedId = env->GetMethodID(nativeMapViewClass, "onMapChanged", "()V");
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

    projectedMetersClass = env->FindClass("com/mapbox/mapboxgl/geometry/ProjectedMeters");
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

    const std::vector<JNINativeMethod> methods = {
        {"nativeCreate", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;F)J",
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
        {"nativeResume", "(J)V", reinterpret_cast<void *>(&nativeResume)},
        {"nativeUpdate", "(J)V", reinterpret_cast<void *>(&nativeUpdate)},
        {"nativeOnInvalidate", "(JZ)V", reinterpret_cast<void *>(&nativeOnInvalidate)},
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
        {"nativeMoveBy", "(JDDJ)V", reinterpret_cast<void *>(&nativeMoveBy)},
        {"nativeSetLatLng", "(JLcom/mapbox/mapboxgl/geometry/LatLng;J)V",
         reinterpret_cast<void *>(&nativeSetLatLng)},
        {"nativeSetSprite", "(JLjava/lang/String;IIF[B)V", reinterpret_cast<void *>(&nativeSetSprite)},
        {"nativeAddMarker", "(JLcom/mapbox/mapboxgl/annotations/Marker;)J",
         reinterpret_cast<void *>(&nativeAddMarker)},
        {"nativeAddPolyline", "(JLcom/mapbox/mapboxgl/annotations/Polyline;)J",
         reinterpret_cast<void *>(&nativeAddPolyline)},
        {"nativeAddPolygon", "(JLcom/mapbox/mapboxgl/annotations/Polygon;)J",
         reinterpret_cast<void *>(&nativeAddPolygon)},
        {"nativeAddPolygons", "(JLjava/util/List;)[J",
         reinterpret_cast<void *>(&nativeAddPolygons)},
        {"nativeRemoveAnnotation", "(JJ)V", reinterpret_cast<void *>(&nativeRemoveAnnotation)},
        {"nativeRemoveAnnotations", "(J[J)V", reinterpret_cast<void *>(&nativeRemoveAnnotations)},
        {"nativeGetLatLng", "(J)Lcom/mapbox/mapboxgl/geometry/LatLng;",
         reinterpret_cast<void *>(&nativeGetLatLng)},
        {"nativeResetPosition", "(J)V", reinterpret_cast<void *>(&nativeResetPosition)},
        {"nativeScaleBy", "(JDDDJ)V", reinterpret_cast<void *>(&nativeScaleBy)},
        {"nativeSetScale", "(JDDDJ)V", reinterpret_cast<void *>(&nativeSetScale)},
        {"nativeGetScale", "(J)D", reinterpret_cast<void *>(&nativeGetScale)},
        {"nativeSetZoom", "(JDJ)V", reinterpret_cast<void *>(&nativeSetZoom)},
        {"nativeGetZoom", "(J)D", reinterpret_cast<void *>(&nativeGetZoom)},
        {"nativeSetLatLngZoom", "(JLcom/mapbox/mapboxgl/geometry/LatLngZoom;J)V",
         reinterpret_cast<void *>(&nativeSetLatLngZoom)},
        {"nativeGetLatLngZoom", "(J)Lcom/mapbox/mapboxgl/geometry/LatLngZoom;",
         reinterpret_cast<void *>(&nativeGetLatLngZoom)},
        {"nativeResetZoom", "(J)V", reinterpret_cast<void *>(&nativeResetZoom)},
        {"nativeGetMinZoom", "(J)D", reinterpret_cast<void *>(&nativeGetMinZoom)},
        {"nativeGetMaxZoom", "(J)D", reinterpret_cast<void *>(&nativeGetMaxZoom)},
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
        {"nativeSetDebug", "(JZ)V", reinterpret_cast<void *>(&nativeSetDebug)},
        {"nativeToggleDebug", "(J)V", reinterpret_cast<void *>(&nativeToggleDebug)},
        {"nativeGetDebug", "(J)Z", reinterpret_cast<void *>(&nativeGetDebug)},
        {"nativeSetReachability", "(JZ)V", reinterpret_cast<void *>(&nativeSetReachability)},
        //{"nativeGetWorldBoundsMeters", "(J)V", reinterpret_cast<void *>(&nativeGetWorldBoundsMeters)},
        //{"nativeGetWorldBoundsLatLng", "(J)V", reinterpret_cast<void *>(&nativeGetWorldBoundsLatLng)},
        {"nativeGetMetersPerPixelAtLatitude", "(JDD)D", reinterpret_cast<void *>(&nativeGetMetersPerPixelAtLatitude)},
        {"nativeProjectedMetersForLatLng", "(JLcom/mapbox/mapboxgl/geometry/LatLng;)Lcom/mapbox/mapboxgl/geometry/ProjectedMeters;", reinterpret_cast<void *>(&nativeProjectedMetersForLatLng)},
        {"nativeLatLngForProjectedMeters", "(JLcom/mapbox/mapboxgl/geometry/ProjectedMeters;)Lcom/mapbox/mapboxgl/geometry/LatLng;", reinterpret_cast<void *>(&nativeLatLngForProjectedMeters)},
        {"nativePixelForLatLng", "(JLcom/mapbox/mapboxgl/geometry/LatLng;)Landroid/graphics/PointF;", reinterpret_cast<void *>(&nativePixelForLatLng)},
        {"nativeLatLngForPixel", "(JLandroid/graphics/PointF;)Lcom/mapbox/mapboxgl/geometry/LatLng;", reinterpret_cast<void *>(&nativeLatLngForPixel)},
    };

    if (env->RegisterNatives(nativeMapViewClass, methods.data(), methods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngClass = reinterpret_cast<jclass>(env->NewGlobalRef(latLngClass));
    if (latLngClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    markerClass = reinterpret_cast<jclass>(env->NewGlobalRef(markerClass));
    if (markerClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polylineClass = reinterpret_cast<jclass>(env->NewGlobalRef(polylineClass));
    if (polylineClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    polygonClass = reinterpret_cast<jclass>(env->NewGlobalRef(polygonClass));
    if (polygonClass == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    latLngZoomClass = reinterpret_cast<jclass>(env->NewGlobalRef(latLngZoomClass));
    if (latLngZoomClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        return JNI_ERR;
    }

    runtimeExceptionClass = reinterpret_cast<jclass>(env->NewGlobalRef(runtimeExceptionClass));
    if (runtimeExceptionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngZoomClass);
        return JNI_ERR;
    }

    nullPointerExceptionClass =
        reinterpret_cast<jclass>(env->NewGlobalRef(nullPointerExceptionClass));
    if (nullPointerExceptionClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngZoomClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        return JNI_ERR;
    }

    arrayListClass = reinterpret_cast<jclass>(env->NewGlobalRef(arrayListClass));
    if (arrayListClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngZoomClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        return JNI_ERR;
    }

    projectedMetersClass = reinterpret_cast<jclass>(env->NewGlobalRef(projectedMetersClass));
    if (projectedMetersClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngZoomClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        return JNI_ERR;
    }


    pointFClass = reinterpret_cast<jclass>(env->NewGlobalRef(pointFClass));
    if (pointFClass == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(latLngClass);
        env->DeleteGlobalRef(latLngZoomClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(arrayListClass);
        env->DeleteGlobalRef(projectedMetersClass);
        return JNI_ERR;
    }


    char release[PROP_VALUE_MAX] = "";
    __system_property_get("ro.build.version.release", release);
    androidRelease = std::string(release);

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnUnload");

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

    env->DeleteGlobalRef(markerClass);
    markerClass = nullptr;
    markerConstructorId = nullptr;
    markerPositionId = nullptr;
    markerSpriteId = nullptr;

    env->DeleteGlobalRef(polylineClass);
    polylineClass = nullptr;
    polylineConstructorId = nullptr;
    polylineAlphaId = nullptr;
    polylineVisibleId = nullptr;
    polylineColorId = nullptr;
    polylineWidthId = nullptr;
    polylinePointsId = nullptr;

    env->DeleteGlobalRef(polygonClass);
    polygonClass = nullptr;
    polygonConstructorId = nullptr;
    polygonAlphaId = nullptr;
    polygonVisibleId = nullptr;
    polygonFillColorId = nullptr;
    polygonStrokeColorId = nullptr;
    polygonStrokeWidthId = nullptr;
    polygonPointsId = nullptr;
    polygonHolesId = nullptr;


    env->DeleteGlobalRef(latLngZoomClass);
    latLngZoomClass = nullptr;
    latLngZoomConstructorId = nullptr;
    latLngZoomLongitudeId = nullptr;
    latLngZoomLatitudeId = nullptr;
    latLngZoomZoomId = nullptr;

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
}
}
