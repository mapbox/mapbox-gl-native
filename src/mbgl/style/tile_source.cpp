#include <mbgl/style/tile_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {
namespace style {

TileSource::TileSource(SourceType type_,
                       std::string id_,
                       std::string url_,
                       uint16_t tileSize_,
                       std::unique_ptr<Tileset>&& tileset_)
    : Source(type_, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)) {
}

void TileSource::load(FileSource& fileSource) {
    if (url.empty()) {
        // If the URL is empty, the TileJSON was specified inline in the stylesheet.
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    // URL may either be a TileJSON file, or a GeoJSON file.
    req = fileSource.request(Resource::source(url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            std::unique_ptr<Tileset> newTileset;

            // Create a new copy of the Tileset object that holds the base values we've parsed
            // from the stylesheet. Then merge in the values parsed from the TileJSON we retrieved
            // via the URL.
            try {
                newTileset = style::parseTileJSON(*res.data, url, type, tileSize);
            } catch (...) {
                observer->onSourceError(*this, std::current_exception());
                return;
            }

            // Check whether previous information specifies different tile
            if (tileset && tileset->tiles != newTileset->tiles) {
                // Tile URLs changed: force tiles to be reloaded.
                invalidateTiles();

                // Tile size changed: We need to recalculate the tiles we need to load because we
                // might have to load tiles for a different zoom level
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Min/Max zoom changed: We need to recalculate what tiles to load, if we have tiles
                // loaded that are outside the new zoom range
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Attribution changed: We need to notify the embedding application that this
                // changed. See https://github.com/mapbox/mapbox-gl-native/issues/2723
                // This is not yet implemented.

                // Center/bounds changed: We're not using these values currently
            }

            tileset = std::move(newTileset);
            loaded = true;
            observer->onSourceLoaded(*this);
        }
    });
}

} // namespace style
} // namespace mbgl
