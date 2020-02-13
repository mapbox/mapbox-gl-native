#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class RasterSource : public Source {
public:
    RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize, SourceType sourceType = SourceType::Raster);
    ~RasterSource() override;

    const variant<std::string, Tileset>& getURLOrTileset() const;
    optional<std::string> getURL() const;

    uint16_t getTileSize() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() final {
        return weakFactory.makeWeakPtr();
    }

protected:
    Mutable<Source::Impl> createMutable() const noexcept final;

private:
    const variant<std::string, Tileset> urlOrTileset;
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

template <>
inline bool Source::is<RasterSource>() const {
    return getType() == SourceType::Raster;
}

} // namespace style
} // namespace mbgl
