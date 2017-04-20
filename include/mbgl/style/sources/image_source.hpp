#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
class LatLng;
class AsyncRequest;

namespace style {

class ImageSource : public Source {
public:
    ImageSource(std::string id, const std::vector<LatLng>);
    ~ImageSource() override;

    const std::string& getURL() const;
    void setURL(const std::string& url) ;

    void setImage(mbgl::UnassociatedImage);

    void setCoordinates(const std::vector<LatLng>&);
    std::vector<LatLng> getCoordinates() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;
private:
    optional<std::string> url;
    std::unique_ptr<AsyncRequest> req;
};

template <>
inline bool Source::is<ImageSource>() const {
    return getType() == SourceType::Image;
}

} // namespace style
} // namespace mbgl
