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
#include "bitmap.hpp"
#include "bitmap_factory.hpp"
#include "connectivity_listener.hpp"
#include "style/functions/categorical_stops.hpp"
#include "style/functions/exponential_stops.hpp"
#include "style/functions/identity_stops.hpp"
#include "style/functions/interval_stops.hpp"
#include "style/functions/stop.hpp"
#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "graphics/pointF.hpp"
#include "graphics/rectF.hpp"
#include "geometry/feature.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/projected_meters.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/sources.hpp"

#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"

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

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace mbgl {
namespace android {

void RegisterNativeHTTPRequest(JNIEnv&);

JavaVM* theJVM;

std::string cachePath;
std::string dataPath;
std::string apkPath;
std::string androidRelease;

jni::jclass* latLngBoundsClass = nullptr;
jni::jmethodID* latLngBoundsConstructorId = nullptr;
jni::jfieldID* latLngBoundsLatNorthId = nullptr;
jni::jfieldID* latLngBoundsLatSouthId = nullptr;
jni::jfieldID* latLngBoundsLonEastId = nullptr;
jni::jfieldID* latLngBoundsLonWestId = nullptr;

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
            mbgl::Log::Info(mbgl::Event::JNI, "Attached.");
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
            mbgl::Log::Info(mbgl::Event::JNI, "Attached.");
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
            mbgl::Log::Info(mbgl::Event::JNI, "~Observer()");
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
            mbgl::Log::Info(mbgl::Event::JNI, "Attached.");
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
            mbgl::Log::Info(mbgl::Event::JNI, "Attached.");
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
            mbgl::Log::Info(mbgl::Event::JNI, "Attached.");
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

    //For the DefaultFileSource
    static mbgl::util::RunLoop mainRunLoop;

    //Basic types
    java::registerNatives(env);
    java::util::registerNative(env);
    PointF::registerNative(env);
    RectF::registerNative(env);

    //Geometry
    Feature::registerNative(env);
    LatLng::registerNative(env);
    ProjectedMeters::registerNative(env);

    //Annotation
    Marker::registerNative(env);
    Polygon::registerNative(env);
    Polyline::registerNative(env);

    //Map
    NativeMapView::registerNative(env);

    //Http
    RegisterNativeHTTPRequest(env);

    //Bitmap
    Bitmap::registerNative(env);
    BitmapFactory::registerNative(env);

    //Style
    registerNativeLayers(env);
    registerNativeSources(env);
    Stop::registerNative(env);
    CategoricalStops::registerNative(env);
    ExponentialStops::registerNative(env);
    IdentityStops::registerNative(env);
    IntervalStops::registerNative(env);

    //Connectivity
    ConnectivityListener::registerNative(env);

    latLngBoundsClass = &jni::FindClass(env, "com/mapbox/mapboxsdk/geometry/LatLngBounds");
    latLngBoundsClass = jni::NewGlobalRef(env, latLngBoundsClass).release();
    latLngBoundsConstructorId = &jni::GetMethodID(env, *latLngBoundsClass, "<init>", "(DDDD)V");
    latLngBoundsLatNorthId = &jni::GetFieldID(env, *latLngBoundsClass, "mLatNorth", "D");
    latLngBoundsLatSouthId = &jni::GetFieldID(env, *latLngBoundsClass, "mLatSouth", "D");
    latLngBoundsLonEastId = &jni::GetFieldID(env, *latLngBoundsClass, "mLonEast", "D");
    latLngBoundsLonWestId = &jni::GetFieldID(env, *latLngBoundsClass, "mLonWest", "D");

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

    #define MAKE_NATIVE_METHOD(name, sig) jni::MakeNativeMethod<decltype(name), name>( #name, sig )

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

} // namespace android
} // namespace mbgl
