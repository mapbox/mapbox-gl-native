#include "map_snapshot.hpp"

#include "../bitmap.hpp"
#include "../jni/collection.hpp"

#include <memory>

namespace mbgl {
namespace android {

MapSnapshot::MapSnapshot(float pixelRatio_, MapSnapshot::PointForFn pointForFn_)
    : pixelRatio(pixelRatio_)
    , pointForFn(std::move(pointForFn_)) {
}

MapSnapshot::~MapSnapshot() = default;

jni::Object<PointF> MapSnapshot::pixelForLatLng(jni::JNIEnv& env, jni::Object<LatLng> jLatLng) {
    ScreenCoordinate point = pointForFn(LatLng::getLatLng(env, jLatLng));
    return PointF::New(env, point.x * pixelRatio, point.y * pixelRatio);
}


// Static methods //

jni::Object<MapSnapshot> MapSnapshot::New(JNIEnv& env,
                                          PremultipliedImage&& image,
                                          float pixelRatio,
                                          std::vector<std::string> attributions,
                                          bool showLogo,
                                          mbgl::MapSnapshotter::PointForFn pointForFn) {
    // Create the bitmap
    auto bitmap = Bitmap::CreateBitmap(env, std::move(image));

    // Create the Mapsnapshot peers
    static auto constructor = javaClass.GetConstructor<jni::jlong, jni::Object<Bitmap>, jni::Array<jni::String>, jni::jboolean>(env);
    auto nativePeer = std::make_unique<MapSnapshot>(pixelRatio, pointForFn);
    return javaClass.New(env, constructor, reinterpret_cast<jlong>(nativePeer.release()), bitmap, jni::Make<jni::Array<jni::String>>(env, attributions), (jni::jboolean) showLogo);
}

jni::Class<MapSnapshot> MapSnapshot::javaClass;

void MapSnapshot::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    MapSnapshot::javaClass = *jni::Class<MapSnapshot>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapSnapshot>(env, MapSnapshot::javaClass,
                                            "nativePtr",
                                            std::make_unique<MapSnapshot, JNIEnv&>,
                                            "initialize",
                                            "finalize",
                                            METHOD(&MapSnapshot::pixelForLatLng, "pixelForLatLng")
    );
}

} // namespace android
} // namespace mbgl
