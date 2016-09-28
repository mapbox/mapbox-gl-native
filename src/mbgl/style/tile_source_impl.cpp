#include <mbgl/style/tile_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/storage/file_source.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <sstream>

namespace mbgl {
namespace style {

Tileset TileSourceImpl::parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType type, uint16_t tileSize) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message << document.GetErrorOffset() << " - " << rapidjson::GetParseError_En(document.GetParseError());
        throw std::runtime_error(message.str());
    }

    conversion::Result<Tileset> result = conversion::convert<Tileset>(document);
    if (!result) {
        throw std::runtime_error(result.error().message);
    }

    // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
    if (util::mapbox::isMapboxURL(sourceURL)) {
        for (auto& url : (*result).tiles) {
            url = util::mapbox::canonicalizeTileURL(url, type, tileSize);
        }
    }

    return *result;
}

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
            Tileset newTileset;

            // Create a new copy of the Tileset object that holds the base values we've parsed
            // from the stylesheet. Then merge in the values parsed from the TileJSON we retrieved
            // via the URL.
            try {
                newTileset = parseTileJSON(*res.data, url, type, tileSize);
            } catch (...) {
                observer->onSourceError(base, std::current_exception());
                return;
            }

            // Check whether previous information specifies different tile
            bool attributionChanged = false;
            if (tileset.tiles != newTileset.tiles) {
                // Tile URLs changed: force tiles to be reloaded.
                invalidateTiles();

                // Tile size changed: We need to recalculate the tiles we need to load because we
                // might have to load tiles for a different zoom level
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Min/Max zoom changed: We need to recalculate what tiles to load, if we have tiles
                // loaded that are outside the new zoom range
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Attribution changed: We need to notify the embedding application that this
                // changed.
                attributionChanged = true;

                // Center/bounds changed: We're not using these values currently
            }

            tileset = newTileset;
            loaded = true;

            observer->onSourceLoaded(base);
            if (attributionChanged) {
                observer->onSourceAttributionChanged(base, newTileset.attribution);
            }
        }
    });
}

Range<uint8_t> TileSourceImpl::getZoomRange() {
    assert(loaded);
    return tileset.zoomRange;
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
