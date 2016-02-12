#include <mbgl/storage/offline.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/source/source_info.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cmath>

namespace mbgl {

OfflineTilePyramidRegionDefinition::OfflineTilePyramidRegionDefinition(
    const std::string& styleURL_, const LatLngBounds& bounds_, double minZoom_, double maxZoom_, float pixelRatio_)
    : styleURL(styleURL_),
      bounds(bounds_),
      minZoom(minZoom_),
      maxZoom(maxZoom_),
      pixelRatio(pixelRatio_) {
    if (minZoom < 0 || maxZoom < 0 || maxZoom < minZoom || pixelRatio < 0 ||
        !std::isfinite(minZoom) || std::isnan(maxZoom) || !std::isfinite(pixelRatio)) {
        throw std::invalid_argument("Invalid offline region definition");
    }
}

std::vector<TileID> OfflineTilePyramidRegionDefinition::tileCover(SourceType type, uint16_t tileSize, const SourceInfo& info) const {
    double minZ = std::max<double>(coveringZoomLevel(minZoom, type, tileSize), info.minZoom);
    double maxZ = std::min<double>(coveringZoomLevel(maxZoom, type, tileSize), info.maxZoom);

    assert(minZ >= 0);
    assert(maxZ >= 0);
    assert(minZ < std::numeric_limits<uint8_t>::max());
    assert(maxZ < std::numeric_limits<uint8_t>::max());

    std::vector<TileID> result;

    for (uint8_t z = minZ; z <= maxZ; z++) {
        for (const auto& tile : mbgl::tileCover(bounds, z, z)) {
            result.push_back(tile.normalized());
        }
    }

    return result;
}

OfflineRegionDefinition decodeOfflineRegionDefinition(const std::string& region) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.Parse<0>(region.c_str());

    if (doc.HasParseError() ||
        !doc.HasMember("style_url") || !doc["style_url"].IsString() ||
        !doc.HasMember("bounds") || !doc["bounds"].IsArray() || doc["bounds"].Size() != 4 ||
          !doc["bounds"][0].IsDouble() || !doc["bounds"][1].IsDouble() ||
          !doc["bounds"][2].IsDouble() || !doc["bounds"][3].IsDouble() ||
        !doc.HasMember("min_zoom") || !doc["min_zoom"].IsDouble() ||
        (doc.HasMember("max_zoom") && !doc["max_zoom"].IsDouble()) ||
        !doc.HasMember("pixel_ratio") || !doc["pixel_ratio"].IsDouble()) {
        throw std::runtime_error("Malformed offline region definition");
    }

    std::string styleURL { doc["style_url"].GetString(), doc["style_url"].GetStringLength() };
    LatLngBounds bounds = LatLngBounds::hull(
        LatLng(doc["bounds"][0].GetDouble(), doc["bounds"][1].GetDouble()),
        LatLng(doc["bounds"][2].GetDouble(), doc["bounds"][3].GetDouble()));
    double minZoom = doc["min_zoom"].GetDouble();
    double maxZoom = doc.HasMember("max_zoom") ? doc["max_zoom"].GetDouble() : INFINITY;
    float pixelRatio = doc["pixel_ratio"].GetDouble();

    return { styleURL, bounds, minZoom, maxZoom, pixelRatio };
}

std::string encodeOfflineRegionDefinition(const OfflineRegionDefinition& region) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.SetObject();

    doc.AddMember("style_url", rapidjson::StringRef(region.styleURL.data(), region.styleURL.length()), doc.GetAllocator());

    rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> bounds(rapidjson::kArrayType);
    bounds.PushBack(region.bounds.south(), doc.GetAllocator());
    bounds.PushBack(region.bounds.west(), doc.GetAllocator());
    bounds.PushBack(region.bounds.north(), doc.GetAllocator());
    bounds.PushBack(region.bounds.east(), doc.GetAllocator());
    doc.AddMember("bounds", bounds, doc.GetAllocator());

    doc.AddMember("min_zoom", region.minZoom, doc.GetAllocator());
    if (std::isfinite(region.maxZoom)) {
        doc.AddMember("max_zoom", region.maxZoom, doc.GetAllocator());
    }

    doc.AddMember("pixel_ratio", region.pixelRatio, doc.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
}

OfflineRegion::OfflineRegion(int64_t id_,
                             const OfflineRegionDefinition& definition_,
                             const OfflineRegionMetadata& metadata_)
    : id(id_),
      definition(definition_),
      metadata(metadata_) {
}

OfflineRegion::OfflineRegion(OfflineRegion&&) = default;
OfflineRegion::~OfflineRegion() = default;

const OfflineRegionDefinition& OfflineRegion::getDefinition() const {
    return definition;
}

const OfflineRegionMetadata& OfflineRegion::getMetadata() const {
    return metadata;
}

int64_t OfflineRegion::getID() const {
    return id;
}

} // namespace mbgl
