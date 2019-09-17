#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class VectorSource final : public Source {
public:
    VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset, optional<float> maxZoom = nullopt,
                 optional<float> minZoom = nullopt);
    ~VectorSource() final;

    const variant<std::string, Tileset>& getURLOrTileset() const;
    optional<std::string> getURL() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }

private:
    const variant<std::string, Tileset> urlOrTileset;
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
    optional<float> maxZoom;
    optional<float> minZoom;
};

template <>
inline bool Source::is<VectorSource>() const {
    return getType() == SourceType::Vector;
}

} // namespace style
} // namespace mbgl
