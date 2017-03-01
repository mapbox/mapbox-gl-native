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
        Observer(jni::UniqueObject<OfflineRegion::OfflineRegionObserver>&& callback_)
            //TODO add a generic deleter for jni::Object
            : callback(callback_.release()->Get()) {
        }

        ~Observer() override {
            android::UniqueEnv env = android::AttachEnv();
            env->DeleteGlobalRef(Unwrap(*callback));
        }

        void statusChanged(mbgl::OfflineRegionStatus status) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            // Status object
            auto jStatus = OfflineRegionStatus::New(*env, status);

            // Call
            static auto method = OfflineRegion::OfflineRegionObserver::javaClass
                .GetMethod<void (jni::Object<OfflineRegionStatus>)>(*env, "onStatusChanged");
            callback.Call(*env, method, jStatus);

            // Delete references
            jni::DeleteLocalRef(*env, jStatus);
        }

        void responseError(mbgl::Response::Error error) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            // Error object
            auto jError = OfflineRegionError::New(*env, error);

            // Call
            static auto method = OfflineRegion::OfflineRegionObserver::javaClass
                .GetMethod<void (jni::Object<mbgl::android::OfflineRegionError>)>(*env, "onError");
            callback.Call(*env, method, jError);

            // Delete references
            jni::DeleteLocalRef(*env, jError);
        }

        void mapboxTileCountLimitExceeded(uint64_t limit) override {
            // Reattach, the callback comes from a different thread
            android::UniqueEnv env = android::AttachEnv();

            // Send limit
            static auto method = OfflineRegion::OfflineRegionObserver::javaClass
                .GetMethod<void (jni::jlong)>(*env, "mapboxTileCountLimitExceeded");
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
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter())
    ](std::exception_ptr error, mbgl::optional<mbgl::OfflineRegionStatus> status) mutable {
        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (error) {
            OfflineRegionStatusCallback::onError(*env, jni::Object<OfflineRegionStatusCallback>(*callback), error);
        } else if (status) {
            OfflineRegionStatusCallback::onStatus(*env, jni::Object<OfflineRegionStatusCallback>(*callback), std::move(status));
        }
    });
}

void OfflineRegion::deleteOfflineRegion(jni::JNIEnv& env_, jni::Object<OfflineRegionDeleteCallback> callback_) {
    // Delete
    fileSource.deleteOfflineRegion(std::move(*region), [
        //Ensure the object is not gc'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter())
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
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter())
    ](std::exception_ptr error, mbgl::optional<mbgl::OfflineRegionMetadata> data) mutable {
        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (error) {
            OfflineRegionUpdateMetadataCallback::onError(*env, jni::Object<OfflineRegionUpdateMetadataCallback>(*callback), error);
        } else if (data) {
            OfflineRegionUpdateMetadataCallback::onUpdate(*env, jni::Object<OfflineRegionUpdateMetadataCallback>(*callback), std::move(data));
        }
    });
}

jni::Object<OfflineRegion> OfflineRegion::New(jni::JNIEnv& env, jni::Object<FileSource> jFileSource, mbgl::OfflineRegion region) {

    // Definition
    auto definition = jni::Object<OfflineRegionDefinition>(*OfflineTilePyramidRegionDefinition::New(env, region.getDefinition()));

    // Metadata
    auto metadata = OfflineRegion::metadata(env, region.getMetadata());

    // Create region java object
    static auto constructor = OfflineRegion::javaClass.GetConstructor<jni::jlong, jni::Object<FileSource>, jni::jlong, jni::Object<OfflineRegionDefinition>, jni::Array<jni::jbyte>>(env);
    auto jregion = OfflineRegion::javaClass.New(env, constructor,
        reinterpret_cast<jni::jlong>(new mbgl::OfflineRegion(std::move(region))), //Copy a region to the heap
        jFileSource, jni::jlong(region.getID()), definition, metadata);

    //Delete references
    jni::DeleteLocalRef(env, definition);
    jni::DeleteLocalRef(env, metadata);

    return jregion;
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

jni::Class<OfflineRegion> OfflineRegion::javaClass;

void OfflineRegion::registerNative(jni::JNIEnv& env) {
    OfflineRegion::OfflineRegionObserver::registerNative(env);
    OfflineRegion::OfflineRegionStatusCallback::registerNative(env);
    OfflineRegion::OfflineRegionDeleteCallback::registerNative(env);
    OfflineRegion::OfflineRegionUpdateMetadataCallback::registerNative(env);

    javaClass = *jni::Class<OfflineRegion>::Find(env).NewGlobalRef(env).release();

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

jni::Class<OfflineRegion::OfflineRegionObserver> OfflineRegion::OfflineRegionObserver::javaClass;

void OfflineRegion::OfflineRegionObserver::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegion::OfflineRegionObserver>::Find(env).NewGlobalRef(env).release();
}

// OfflineRegionStatusCallback //

jni::Class<OfflineRegion::OfflineRegionStatusCallback> OfflineRegion::OfflineRegionStatusCallback::javaClass;

void OfflineRegion::OfflineRegionStatusCallback::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionStatusCallback>::Find(env).NewGlobalRef(env).release();
}

void OfflineRegion::OfflineRegionStatusCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionStatusCallback> callback,
                                                          std::exception_ptr error) {
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    std::string message = mbgl::util::toString(error);
    callback.Call(env, method, jni::Make<jni::String>(env, message));
}

void OfflineRegion::OfflineRegionStatusCallback::onStatus(jni::JNIEnv& env,
                                                        jni::Object<OfflineRegion::OfflineRegionStatusCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegionStatus> status) {
    //Convert to java peer object
    auto jStatus = OfflineRegionStatus::New(env, std::move(*status));

    // Trigger callback
    static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegionStatus>)>(env, "onStatus");
    callback.Call(env, method, jStatus);
    jni::DeleteLocalRef(env, jStatus);
}

// OfflineRegionDeleteCallback //

jni::Class<OfflineRegion::OfflineRegionDeleteCallback> OfflineRegion::OfflineRegionDeleteCallback::javaClass;

void OfflineRegion::OfflineRegionDeleteCallback::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionDeleteCallback>::Find(env).NewGlobalRef(env).release();
}

void OfflineRegion::OfflineRegionDeleteCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionDeleteCallback> callback,
                                                          std::exception_ptr error) {
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    std::string message = mbgl::util::toString(error);
    callback.Call(env, method, jni::Make<jni::String>(env, message));
}

void OfflineRegion::OfflineRegionDeleteCallback::onDelete(jni::JNIEnv& env, jni::Object<OfflineRegion::OfflineRegionDeleteCallback> callback) {
    // Trigger callback
    static auto method = javaClass.GetMethod<void ()>(env, "onDelete");
    callback.Call(env, method);
}

// OfflineRegionUpdateMetadataCallback //

jni::Class<OfflineRegion::OfflineRegionUpdateMetadataCallback> OfflineRegion::OfflineRegionUpdateMetadataCallback::javaClass;

void OfflineRegion::OfflineRegionUpdateMetadataCallback::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionUpdateMetadataCallback>::Find(env).NewGlobalRef(env).release();
}

void OfflineRegion::OfflineRegionUpdateMetadataCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineRegion::OfflineRegionUpdateMetadataCallback> callback,
                                                          std::exception_ptr error) {
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    std::string message = mbgl::util::toString(error);
    callback.Call(env, method, jni::Make<jni::String>(env, message));
}

void OfflineRegion::OfflineRegionUpdateMetadataCallback::onUpdate(jni::JNIEnv& env,
                                                        jni::Object<OfflineRegion::OfflineRegionUpdateMetadataCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegionMetadata> metadata) {
    //Convert to java peer object
    auto jMetadata = OfflineRegion::metadata(env, std::move(*metadata));

    // Trigger callback
    static auto method = javaClass.GetMethod<void (jni::Array<jni::jbyte>)>(env, "onUpdate");
    callback.Call(env, method, jMetadata);
    jni::DeleteLocalRef(env, jMetadata);
}

} // namespace android
} // namespace mbgl
