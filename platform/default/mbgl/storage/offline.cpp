#include <mapbox/geojson/rapidjson.hpp>

#include <mbgl/storage/offline.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cmath>

namespace mbgl {
    
const char OfflineFixedGeometryTraits<LatLngBounds>::typeName[] = "PYRAMID";
const char OfflineFixedGeometryTraits<Polygon<double>>::typeName[] = "POLYGON";
    
void OfflineFixedGeometryTraits<LatLngBounds>::encode(rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>& doc, LatLngBounds bounds) {
    rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> encodedBounds(rapidjson::kArrayType);
    encodedBounds.PushBack(bounds.south(), doc.GetAllocator());
    encodedBounds.PushBack(bounds.west(), doc.GetAllocator());
    encodedBounds.PushBack(bounds.north(), doc.GetAllocator());
    encodedBounds.PushBack(bounds.east(), doc.GetAllocator());
    doc.AddMember("bounds", encodedBounds, doc.GetAllocator());
}

LatLngBounds OfflineFixedGeometryTraits<LatLngBounds>::decode(const rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>& doc) {
    if (!doc.HasMember("bounds") || !doc["bounds"].IsArray() || doc["bounds"].Size() != 4) {
        throw std::runtime_error("Malformed pyramid offline region definition");
    }
    return LatLngBounds::hull(LatLng(doc["bounds"][0].GetDouble(), doc["bounds"][1].GetDouble()),
                              LatLng(doc["bounds"][2].GetDouble(), doc["bounds"][3].GetDouble()));
}
    
void OfflineFixedGeometryTraits<Polygon<double>>::encode(rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>& doc, const Polygon<double>& polygon) {
    auto encodedGeom = mapbox::geojson::convert<Geometry<double>>({ polygon }, doc.GetAllocator());
    doc.AddMember("polygon", encodedGeom, doc.GetAllocator());
}
    
Polygon<double> OfflineFixedGeometryTraits<Polygon<double>>::decode(const rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>& doc) {
    if (!doc.HasMember("polygon")) {
        throw std::runtime_error("Malformed polygon offline region definition");
    }
    return mapbox::geojson::convert<Geometry<double>>(doc["polygon"]).get<Polygon<double>>();
}
    
float pixelRatio(const OfflineRegionDefinition& region) {
    return OfflineRegionDefinition::visit(region, [](const auto& r) { return r.pixelRatio; });
}

const std::string& styleURL(const OfflineRegionDefinition& region) {
    return OfflineRegionDefinition::visit(region, [](const auto& r) -> const std::string& { return r.styleURL; });
}

std::vector<CanonicalTileID> tileCover(const OfflineRegionDefinition& region, SourceType type, uint16_t tileSize, Range<uint8_t> zoomRange) {
    return OfflineRegionDefinition::visit(region, [type, tileSize, zoomRange](const auto& r) { return r.tileCover(type, tileSize, zoomRange); });
}
    
std::string encodeOfflineRegionDefinition(const OfflineRegionDefinition& region) {
    return OfflineRegionDefinition::visit(region, [](const auto& r) { return r.encode(); });
}

OfflineRegionDefinition decodeOfflineRegionDefinition(const std::string& region) {
    
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.Parse<0>(region.c_str());
    
    if (doc.HasParseError()) {
        throw std::runtime_error("Malformed offline region definition");
    }
    
    std::string type;
    if (!doc.HasMember("type")) {
        // Might be an old database format; assume it's a tile pyramid
        type = OfflineFixedGeometryTraits<LatLngBounds>::typeName;
    }
    else if (!doc["type"].IsString()) {
        throw std::runtime_error("Malformed offline region definition");
    }
    else {
        type = doc["type"].GetString();
    }
    
    if (type == OfflineFixedGeometryTraits<LatLngBounds>::typeName) {
        return OfflineFixedGeometryRegionDefinition<LatLngBounds>::decode(doc);
    }
    else if (type == OfflineFixedGeometryTraits<Polygon<double>>::typeName) {
        return OfflineFixedGeometryRegionDefinition<Polygon<double>>::decode(doc);
    }
    else {
        std::string msg("Unknown offline region type: ");
        throw std::runtime_error(msg + type);
    }
}

OfflineRegion::OfflineRegion(int64_t id_,
                             OfflineRegionDefinition definition_,
                             OfflineRegionMetadata metadata_)
    : id(id_),
      definition(std::move(definition_)),
      metadata(std::move(metadata_)) {
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
