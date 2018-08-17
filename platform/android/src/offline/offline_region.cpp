#include "offline_region.hpp"

#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include "offline_region_definition.hpp"
#include "offline_region_error.hpp"
#include "offline_region_status.hpp"
#include "../attach_env.hpp"
#include "../jni/generic_global_ref_deleter.hpp"

namespace mbgl {
namespace android {

// OfflineRegion //

OfflineRegion::OfflineRegion(jni::JNIEnv& env, jni::jlong offlineRegionPtr, jni::Object<FileSource> jFileSource)
    : region(reinterpret_cast<mbgl::OfflineRegion *>(offlineRegionPtr)),
      fileSource(mbgl::android::FileSource::getDefaultFileSource(env, jFileSource)) {}

OfflineRegion::~OfflineRegion() {}

void OfflineRegion::setOfflineRegionObserver(jni::JNIEnv& env_, jni::Object<OfflineRegion::OfflineRegionObserver> callback) {

    // Define the observer
    class Observer : public mbgl::OfflineRegionObserver {
    public:
        Observer(jni::Global<jni::Object<OfflineRegion::OfflineRegionObserver>>&& callback_)
            //TODO add a generic deleter for jni::Object
            : callback(callback_.release().Get()) {
        }

        ~Observer() override {
            android::UniqueEnv env = android::AttachEnv();
            env->DeleteGlobalRef(Unwrap(*callback));
        }

        void statusChanged(mbgl::OfflineRegionStatus status) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            static auto javaClass = jni::Class<OfflineRegion::OfflineRegionObserver>::Singleton(*env);
            static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegionStatus>)>(*env, "onStatusChanged");

            callback.Call(*env, method,
                *jni::SeizeLocal(*env, OfflineRegionStatus::New(*env, status)));
        }

        void responseError(mbgl::Response::Error error) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            static auto javaClass = jni::Class<OfflineRegion::OfflineRegionObserver>::Singleton(*env);
            static auto method = javaClass.GetMethod<void (jni::Object<mbgl::android::OfflineRegionError>)>(*env, "onError");

            callback.Call(*env, method,
                *jni::SeizeLocal(*env, OfflineRegionError::New(*env, error)));
        }

        void mapboxTileCountLimitExceeded(uint64_t limit) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            static auto javaClass = jni::Class<OfflineRegion::OfflineRegionObserver>::Singleton(*env);
            static auto method = javaClass.GetMethod<void (jni::jlong)>(*env, "mapboxTileCountLimitExceeded");

            callback.Call(*env, method, jlong(limit));
        }

        jni::Object<OfflineRegion::OfflineRegionObserver> callback;
    };

    // Set the observer
    fileSource.setOfflineRegionObserver(*region, std::make_unique<Observer>(callback.NewGlobalRef(env_)));
}

void OfflineRegion::setOfflineRegionDownloadState(jni::JNIEnv&, jni::jint jState) {
    // State
    mbgl::OfflineRegionDownloadState state;
    switch (jState) {
        case 0:
          state = mbgl::OfflineRegionDownloadState::Inactive;
          break;
        case 1:
          state = mbgl::OfflineRegionDownloadState::Active;
          break;
        default:
          mbgl::Log::Error(mbgl::Event::JNI, "State can only be 0 (inactive) or 1 (active).");
          return;
    }

    fileSource.setOfflineRegionDownloadState(*region, state);
}

void OfflineRegion::getOfflineRegionStatus(jni::JNIEnv& env_, jni::Object<OfflineRegionStatusCallback> callback_) {

    fileSource.getOfflineRegionStatus(*region, [
        //Ensure the object is not gc'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter())
    ](mbgl::expected<mbgl::OfflineRegionStatus, std::exception_ptr> status) mutable {
        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (status) {
            OfflineRegionStatusCallback::onStatus(*env, jni::Object<OfflineRegionStatusCallback>(*callback), std::move(*status));
        } else {
            OfflineRegionStatusCallback::onError(*env, jni::Object<OfflineRegionStatusCallback>(*callback), status.error());
        }
    });
}

void OfflineRegion::deleteOfflineRegion(jni::JNIEnv& env_, jni::Object<OfflineRegionDeleteCallback> callback_) {
    // Delete
    fileSource.deleteOfflineRegion(std::move(*region), [
        //Ensure the object is not gc'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter())
    ](std::exception_ptr error) mutable {
        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (error) {
            OfflineRegionDeleteCallback::onError(*env, jni::Object<OfflineRegionDeleteCallback>(*callback), error);
        } else {
            OfflineRegionDeleteCallback::onDelete(*env, jni::Object<OfflineRegionDeleteCallback>(*callback));
        }
    });
}

void OfflineRegion::updateOfflineRegionMetadata(jni::JNIEnv& env_, jni::Array<jni::jbyte> jMetadata, jni::Object<OfflineRegionUpdateMetadataCallback> callback_) {

    // Convert
    auto metadata = OfflineRegion::metadata(env_, jMetadata);

    fileSource.updateOfflineMetadata(region->getID(), metadata, [
        //Ensure the object is not gc'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter())
    ](mbgl::expected<mbgl::OfflineRegionMetadata, std::exception_ptr> data) mutable {
        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (data) {
            OfflineRegionUpdateMetadataCallback::onUpdate(*env, jni::Object<OfflineRegionUpdateMetadataCallback>(*callback), std::move(*data));
        } else {
            OfflineRegionUpdateMetadataCallback::onError(*env, jni::Object<OfflineRegionUpdateMetadataCallback>(*callback), data.error());
        }
    });
}

jni::Object<OfflineRegion> OfflineRegion::New(jni::JNIEnv& env, jni::Object<FileSource> jFileSource, mbgl::OfflineRegion region) {

    // Definition
    auto definition = jni::SeizeLocal(env, region.getDefinition().match(
            [&](const mbgl::OfflineTilePyramidRegionDefinition def) {
                return jni::Object<OfflineRegionDefinition>(
                        *OfflineTilePyramidRegionDefinition::New(env, def));
            }, [&](const mbgl::OfflineGeometryRegionDefinition def) {
                return jni::Object<OfflineRegionDefinition>(
                        *OfflineGeometryRegionDefinition::New(env, def));
            }));

    // Create region java object
    static auto javaClass = jni::Class<OfflineRegion>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::jlong, jni::Object<FileSource>, jni::jlong, jni::Object<OfflineRegionDefinition>, jni::Array<jni::jbyte>>(env);

    return javaClass.New(env, constructor,
        reinterpret_cast<jni::jlong>(new mbgl::OfflineRegion(std::move(region))), //Copy a region to the heap
        jFileSource,
        jni::jlong(region.getID()),
        *definition,
        *jni::SeizeLocal(env, OfflineRegion::metadata(env, region.getMetadata())));
}

jni::Array<jni::jbyte> OfflineRegion::metadata(jni::JNIEnv& env, mbgl::OfflineRegionMetadata metadata_) {
    std::vector<jni::jbyte> convertedMetadata(metadata_.begin(), metadata_.end());
    std::size_t length = static_cast<std::size_t>(convertedMetadata.size());
    auto metadata = jni::Array<jni::jbyte>::New(env, length);
    metadata.SetRegion<std::vector<jni::jbyte>>(env, 0, convertedMetadata);
    return metadata;
}

mbgl::OfflineRegionMetadata OfflineRegion::metadata(jni::JNIEnv& env, jni::Array<jni::jbyte> metadata_) {
    std::size_t length = metadata_.Length(env);
    auto metadata_tmp = std::vector<jni::jbyte>();
    metadata_tmp.resize(length);
    metadata_.GetRegion<std::vector<jni::jbyte>>(env, 0, metadata_tmp);
    auto metadata = std::vector<uint8_t>(metadata_tmp.begin(), metadata_tmp.end());
    return metadata;
}

void OfflineRegion::registerNative(jni::JNIEnv& env) {
    jni::Class<OfflineRegionObserver>::Singleton(env);
    jni::Class<OfflineRegionStatusCallback>::Singleton(env);
    jni::Class<OfflineRegionDeleteCallback>::Singleton(env);
    jni::Class<OfflineRegionUpdateMetadataCallback>::Singleton(env);

    static auto javaClass = jni::Class<OfflineRegion>::Singleton(env);

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<OfflineRegion>( env, javaClass, "nativePtr",
        std::make_unique<OfflineRegion, JNIEnv&, jni::jlong, jni::Object<FileSource>>,
        "initialize",
        "finalize",
        METHOD(&OfflineRegion::setOfflineRegionObserver, "setOfflineRegionObserver"),
        METHOD(&OfflineRegion::setOfflineRegionDownloadState, "setOfflineRegionDownloadState"),
        METHOD(&OfflineRegion::getOfflineRegionStatus, "getOfflineRegionStatus"),
        METHOD(&OfflineRegion::deleteOfflineRegion, "deleteOfflineRegion"),
        METHOD(&OfflineRegion::updateOfflineRegionMetadata, "updateOfflineRegionMetadata")
    );
}

// OfflineRegionObserver //

// OfflineRegionStatusCallback //

void OfflineRegion::OfflineRegionStatusCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionStatusCallback> callback,
                                                          std::exception_ptr error) {
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionStatusCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method,
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, mbgl::util::toString(error))));
}

void OfflineRegion::OfflineRegionStatusCallback::onStatus(jni::JNIEnv& env,
                                                        jni::Object<OfflineRegion::OfflineRegionStatusCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegionStatus> status) {
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionStatusCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegionStatus>)>(env, "onStatus");

    callback.Call(env, method,
        *jni::SeizeLocal(env, OfflineRegionStatus::New(env, std::move(*status))));
}

// OfflineRegionDeleteCallback //

void OfflineRegion::OfflineRegionDeleteCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionDeleteCallback> callback,
                                                          std::exception_ptr error) {
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionDeleteCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method,
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, mbgl::util::toString(error))));
}

void OfflineRegion::OfflineRegionDeleteCallback::onDelete(jni::JNIEnv& env, jni::Object<OfflineRegion::OfflineRegionDeleteCallback> callback) {
    // Trigger callback
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionDeleteCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void ()>(env, "onDelete");

    callback.Call(env, method);
}

// OfflineRegionUpdateMetadataCallback //

void OfflineRegion::OfflineRegionUpdateMetadataCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionUpdateMetadataCallback> callback,
                                                          std::exception_ptr error) {
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionUpdateMetadataCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method,
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, mbgl::util::toString(error))));
}

void OfflineRegion::OfflineRegionUpdateMetadataCallback::onUpdate(jni::JNIEnv& env,
                                                        jni::Object<OfflineRegion::OfflineRegionUpdateMetadataCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegionMetadata> metadata) {
    static auto javaClass = jni::Class<OfflineRegion::OfflineRegionUpdateMetadataCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Array<jni::jbyte>)>(env, "onUpdate");

    callback.Call(env, method,
        *jni::SeizeLocal(env, OfflineRegion::metadata(env, std::move(*metadata))));
}

} // namespace android
} // namespace mbgl
