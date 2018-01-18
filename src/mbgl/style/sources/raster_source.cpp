#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/mapbox.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize, SourceType sourceType)
    : Source(makeMutable<Impl>(sourceType, std::move(id), tileSize)),
      urlOrTileset(std::move(urlOrTileset_)) {
}

RasterSource::~RasterSource() = default;

const RasterSource::Impl& RasterSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

const variant<std::string, Tileset>& RasterSource::getURLOrTileset() const {
    return urlOrTileset;
}

optional<std::string> RasterSource::getURL() const {
    if (urlOrTileset.is<Tileset>()) {
        return {};
    }

    return urlOrTileset.get<std::string>();
}

uint16_t RasterSource::getTileSize() const {
    return impl().getTileSize();
}

void RasterSource::loadDescription(FileSource& fileSource) {
    if (urlOrTileset.is<Tileset>()) {
        baseImpl = makeMutable<Impl>(impl(), urlOrTileset.get<Tileset>());
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    const std::string& url = urlOrTileset.get<std::string>();
    req = fileSource.request(Resource::source(url), [this, url](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            conversion::Error error;
            optional<Tileset> tileset = conversion::convertJSON<Tileset>(*res.data, error);
            if (!tileset) {
                observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(error.message)));
                return;
            }

            util::mapbox::canonicalizeTileset(*tileset, url, getType(), getTileSize());
            bool changed = impl().getTileset() != *tileset;

            baseImpl = makeMutable<Impl>(impl(), *tileset);
            loaded = true;

            observer->onSourceLoaded(*this);

            if (changed) {
                observer->onSourceChanged(*this);
            }
        }
    });
}

} // namespace style
} // namespace mbgl
