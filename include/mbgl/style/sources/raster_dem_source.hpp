#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class RasterDEMSource : public Source {
public:
    RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize);
    ~RasterDEMSource() final;

    const variant<std::string, Tileset>& getURLOrTileset() const;
    optional<std::string> getURL() const;

    uint16_t getTileSize() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

private:
    const variant<std::string, Tileset> urlOrTileset;
    std::unique_ptr<AsyncRequest> req;
};

template <>
inline bool Source::is<RasterDEMSource>() const {
    return getType() == SourceType::RasterDEM;
}

} // namespace style
} // namespace mbgl
