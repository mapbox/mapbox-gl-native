#include <mbgl/storage/offline.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/projection.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cmath>
#include <utility>

namespace mbgl {

// OfflineTilePyramidRegionDefinition

OfflineTilePyramidRegionDefinition::OfflineTilePyramidRegionDefinition(std::string styleURL_,
                                                                       LatLngBounds bounds_,
                                                                       double minZoom_,
                                                                       double maxZoom_,
                                                                       float pixelRatio_,
                                                                       bool includeIdeographs_)
    : styleURL(std::move(styleURL_)),
      bounds(bounds_),
      minZoom(minZoom_),
      maxZoom(maxZoom_),
      pixelRatio(pixelRatio_),
      includeIdeographs(includeIdeographs_) {
    if (minZoom < 0 || maxZoom < 0 || maxZoom < minZoom || pixelRatio < 0 ||
        !std::isfinite(minZoom) || std::isnan(maxZoom) || !std::isfinite(pixelRatio)) {
        throw std::invalid_argument("Invalid offline region definition");
    }
}

// OfflineGeometryRegionDefinition

OfflineGeometryRegionDefinition::OfflineGeometryRegionDefinition(std::string styleURL_, Geometry<double> geometry_, double minZoom_, double maxZoom_, float pixelRatio_, bool includeIdeographs_)
    : styleURL(std::move(styleURL_))
    , geometry(std::move(geometry_))
    , minZoom(minZoom_)
    , maxZoom(maxZoom_)
    , pixelRatio(pixelRatio_)
    , includeIdeographs(includeIdeographs_){
    if (minZoom < 0 || maxZoom < 0 || maxZoom < minZoom || pixelRatio < 0 ||
        !std::isfinite(minZoom) || std::isnan(maxZoom) || !std::isfinite(pixelRatio)) {
        throw std::invalid_argument("Invalid offline region definition");
    }
}

OfflineRegionDefinition decodeOfflineRegionDefinition(const std::string& region) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.Parse<0>(region.c_str());

    // validation

    auto hasValidBounds = [&] {
        return doc.HasMember("bounds") && doc["bounds"].IsArray() && doc["bounds"].Size() == 4
               && doc["bounds"][0].IsDouble() && doc["bounds"][1].IsDouble()
               && doc["bounds"][2].IsDouble() && doc["bounds"][3].IsDouble();
    };

    auto hasValidGeometry = [&] {
        return doc.HasMember("geometry") && doc["geometry"].IsObject();
    };

    if (doc.HasParseError()
        || !doc.HasMember("style_url") || !doc["style_url"].IsString()
        || !(hasValidBounds() || hasValidGeometry())
        || !doc.HasMember("min_zoom") || !doc["min_zoom"].IsDouble()
        || (doc.HasMember("max_zoom") && !doc["max_zoom"].IsDouble())
        || !doc.HasMember("pixel_ratio") || !doc["pixel_ratio"].IsDouble()
        || (doc.HasMember("include_ideographs") && !doc["include_ideographs"].IsBool())) {
        throw std::runtime_error("Malformed offline region definition");
    }

    // Common properties

    std::string styleURL { doc["style_url"].GetString(), doc["style_url"].GetStringLength() };
    double minZoom = doc["min_zoom"].GetDouble();
    double maxZoom = doc.HasMember("max_zoom") ? doc["max_zoom"].GetDouble() : INFINITY;
    float pixelRatio = doc["pixel_ratio"].GetDouble();
    bool includeIdeographs = doc.HasMember("include_ideographs") ? doc["include_ideographs"].GetBool() : true;
    
    if (doc.HasMember("bounds")) {
        return OfflineTilePyramidRegionDefinition{
                styleURL,
                LatLngBounds::hull(
                         LatLng(doc["bounds"][0].GetDouble(), doc["bounds"][1].GetDouble()),
                         LatLng(doc["bounds"][2].GetDouble(), doc["bounds"][3].GetDouble())),
                minZoom, maxZoom, pixelRatio, includeIdeographs };
    } else {
        return OfflineGeometryRegionDefinition{
                styleURL,
                mapbox::geojson::convert<Geometry<double>>(doc["geometry"].GetObject()),
                minZoom, maxZoom, pixelRatio, includeIdeographs };
    };

}

std::string encodeOfflineRegionDefinition(const OfflineRegionDefinition& region) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.SetObject();

    // Encode common properties
    region.match([&](auto& _region) {
        doc.AddMember("style_url", rapidjson::StringRef(_region.styleURL.data(), _region.styleURL.length()), doc.GetAllocator());
        doc.AddMember("min_zoom", _region.minZoom, doc.GetAllocator());
        if (std::isfinite(_region.maxZoom)) {
            doc.AddMember("max_zoom", _region.maxZoom, doc.GetAllocator());
        }

        doc.AddMember("pixel_ratio", _region.pixelRatio, doc.GetAllocator());
        doc.AddMember("include_ideographs", _region.includeIdeographs, doc.GetAllocator());
    });

    // Encode specific properties
    region.match(
            [&] (const OfflineTilePyramidRegionDefinition& _region) {
                rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> bounds(rapidjson::kArrayType);
                bounds.PushBack(_region.bounds.south(), doc.GetAllocator());
                bounds.PushBack(_region.bounds.west(), doc.GetAllocator());
                bounds.PushBack(_region.bounds.north(), doc.GetAllocator());
                bounds.PushBack(_region.bounds.east(), doc.GetAllocator());
                doc.AddMember("bounds", bounds, doc.GetAllocator());

            },
            [&] (const OfflineGeometryRegionDefinition& _region) {
                doc.AddMember("geometry", mapbox::geojson::convert(_region.geometry, doc.GetAllocator()), doc.GetAllocator());

            }
    );

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
}


// OfflineRegion

OfflineRegion::OfflineRegion(int64_t id_,
                             OfflineRegionDefinition definition_,
                             OfflineRegionMetadata metadata_)
    : id(id_),
      definition(std::move(definition_)),
      metadata(std::move(metadata_)) {
}

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
