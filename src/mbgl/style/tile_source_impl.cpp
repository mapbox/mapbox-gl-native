#include <mbgl/style/tile_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {
namespace style {

TileSourceImpl::TileSourceImpl(SourceType type_, std::string id_, Source& base_,
                               variant<std::string, Tileset> urlOrTileset_,
                               uint16_t tileSize_)
    : Impl(type_, std::move(id_), base_),
      urlOrTileset(std::move(urlOrTileset_)),
      tileSize(tileSize_) {
}

TileSourceImpl::~TileSourceImpl() = default;

void TileSourceImpl::loadDescription(FileSource& fileSource) {
    if (urlOrTileset.is<Tileset>()) {
        tileset = urlOrTileset.get<Tileset>();
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    const std::string& url = urlOrTileset.get<std::string>();
    req = fileSource.request(Resource::source(url), [this, url](Response res) {
        if (res.error) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            conversion::Error error;
            optional<Tileset> newTileset = conversion::convertJSON<Tileset>(*res.data, error);
            if (!newTileset) {
                observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(error.message)));
                return;
            }

            util::mapbox::canonicalizeTileset(*newTileset, url, type, tileSize);
            bool attributionChanged = tileset.attribution != (*newTileset).attribution;

            tileset = *newTileset;
            loaded = true;

            observer->onSourceLoaded(base);
            if (attributionChanged) {
                observer->onSourceChanged(base);
            }
        }
    });
}

optional<Tileset> TileSourceImpl::getTileset() const {
    if (loaded) {
        return tileset;
    }
    return {};
}

optional<std::string> TileSourceImpl::getAttribution() const {
    if (loaded && !tileset.attribution.empty()) {
        return tileset.attribution;
    } else {
        return {};
    }
}

} // namespace style
} // namespace mbgl
