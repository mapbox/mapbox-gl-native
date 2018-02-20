#include <mbgl/util/image.hpp>
#include <mbgl/util/string.hpp>

#include <string>

#include "attach_env.hpp"
#include "bitmap_factory.hpp"

namespace mbgl {

PremultipliedImage decodeImage(Blob blob) {
    auto env{ android::AttachEnv() };

    const auto string = blob.uncompressedData();

    auto array = jni::Array<jni::jbyte>::New(*env, string->size());
    jni::SetArrayRegion(*env, *array, 0, string->size(),
                        reinterpret_cast<const signed char*>(string->data()));

    auto bitmap = android::BitmapFactory::DecodeByteArray(*env, array, 0, string->size());
    return android::Bitmap::GetImage(*env, bitmap);
}

} // namespace mbgl
