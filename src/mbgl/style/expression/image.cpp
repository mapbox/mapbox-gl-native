#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/image.hpp>

namespace mbgl {
namespace style {
namespace expression {

Image::Image(std::string imageID_, bool available_) : imageID(std::move(imageID_)), available(available_) {}

Image::Image(std::string imageID_) : Image(std::move(imageID_), false) {}

Image::Image(const char* imageID_) : Image(std::string(imageID_)) {
    assert(imageID_);
}

bool Image::operator==(const Image& other) const {
    return imageID == other.imageID && available == other.available;
}

mbgl::Value Image::toValue() const {
    return mapbox::base::ValueObject{{"name", imageID}, {"available", available}};
}

const std::string& Image::id() const {
    return imageID;
}

bool Image::isAvailable() const {
    return available;
}

bool Image::empty() const {
    return imageID.empty();
}

} // namespace expression

namespace conversion {
using namespace mbgl::style::expression;
optional<Image> Converter<Image>::operator()(const Convertible& value, Error& error) const {
    if (isArray(value)) {
        Convertible imageParameters = arrayMember(value, 0);
        std::size_t imageParametersLength = arrayLength(imageParameters);
        if (imageParametersLength < 1) {
            error.message = "Image has to contain an ID.";
            return nullopt;
        }

        optional<std::string> imageID = toString(arrayMember(imageParameters, 0));
        if (!imageID) {
            error.message = "Image has to contain an ID.";
            return nullopt;
        }

        return Image(*imageID, false);
    } else if (optional<std::string> result = toString(value)) {
        return Image(*result, false);
    } else {
        error.message = "Image must be plain string or array type.";
        return nullopt;
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
