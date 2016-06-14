#include <mbgl/style/tile_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <sstream>

namespace mbgl {
namespace style {

namespace {

void parseTileJSONMember(const JSValue& value, std::vector<std::string>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsArray()) {
        return;
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        if (!property[i].IsString()) {
            return;
        }
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        target.emplace_back(std::string(property[i].GetString(), property[i].GetStringLength()));
    }
}

void parseTileJSONMember(const JSValue& value, std::string& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsString()) {
        return;
    }

    target = { property.GetString(), property.GetStringLength() };
}

void parseTileJSONMember(const JSValue& value, uint8_t& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsUint()) {
        return;
    }

    unsigned int uint = property.GetUint();
    if (uint > std::numeric_limits<uint8_t>::max()) {
        return;
    }

    target = uint;
}

void parseTileJSONMember(const JSValue& value, std::array<double, 4>& target, const char* name) {
    if (!value.HasMember(name)) {
        return;
    }

    const JSValue& property = value[name];
    if (!property.IsArray() || property.Size() > 4) {
        return;
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        if (!property[i].IsNumber()) {
            return;
        }
    }

    for (rapidjson::SizeType i = 0; i < property.Size(); i++) {
        target[i] = property[i].GetDouble();
    }
}

Tileset parseTileJSON(const JSValue& value) {
    Tileset result;

    parseTileJSONMember(value, result.tiles, "tiles");
    parseTileJSONMember(value, result.zoomRange.min, "minzoom");
    parseTileJSONMember(value, result.zoomRange.max, "maxzoom");
    parseTileJSONMember(value, result.attribution, "attribution");

    std::array<double, 4> array;
    parseTileJSONMember(value, array, "center");
    result.center = { array[0], array[1] };
    result.zoom = array[2];
    parseTileJSONMember(value, array, "bounds");
    result.bounds = LatLngBounds::hull({ array[0], array[1] }, { array[2], array[3] });

    return result;
}

} // end namespace

Tileset TileSource::parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType type, uint16_t tileSize) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError()) {
        std::stringstream message;
        message << document.GetErrorOffset() << " - " << rapidjson::GetParseError_En(document.GetParseError());
        throw std::runtime_error(message.str());
    }

    Tileset result = mbgl::style::parseTileJSON(document);

    // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
    if (util::mapbox::isMapboxURL(sourceURL)) {
        for (auto& url : result.tiles) {
            url = util::mapbox::canonicalizeTileURL(url, type, tileSize);
        }
    }

    return result;
}

optional<variant<std::string, Tileset>> TileSource::parseURLOrTileset(const JSValue& value) {
    if (!value.HasMember("url")) {
        return { mbgl::style::parseTileJSON(value) };
    }

    const JSValue& urlVal = value["url"];
    if (!urlVal.IsString()) {
        Log::Error(Event::ParseStyle, "source url must be a string");
        return {};
    }

    return { std::string(urlVal.GetString(), urlVal.GetStringLength()) };
}

TileSource::TileSource(SourceType type_,
                       std::string id_,
                       variant<std::string, Tileset> urlOrTileset_,
                       uint16_t tileSize_)
    : Source(type_, std::move(id_)),
      urlOrTileset(std::move(urlOrTileset_)),
      tileSize(tileSize_) {
}

TileSource::~TileSource() = default;

void TileSource::load(FileSource& fileSource) {
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
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            Tileset newTileset;

            // Create a new copy of the Tileset object that holds the base values we've parsed
            // from the stylesheet. Then merge in the values parsed from the TileJSON we retrieved
            // via the URL.
            try {
                newTileset = parseTileJSON(*res.data, url, type, tileSize);
            } catch (...) {
                observer->onSourceError(*this, std::current_exception());
                return;
            }

            // Check whether previous information specifies different tile
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
                // changed. See https://github.com/mapbox/mapbox-gl-native/issues/2723
                // This is not yet implemented.

                // Center/bounds changed: We're not using these values currently
            }

            tileset = newTileset;
            loaded = true;

            observer->onSourceLoaded(*this);
        }
    });
}

Range<uint8_t> TileSource::getZoomRange() {
    assert(loaded);
    return tileset.zoomRange;
}

} // namespace style
} // namespace mbgl
