#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/util/geojsonvt/geojsonvt_convert.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layer.hpp>

namespace mbgl {

using namespace mapbox::util::geojsonvt;

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_,
                                         const ShapeAnnotation& shape_,
                                         const uint8_t maxZoom)
: id(id_),
  layerID("com.mapbox.annotations.shape." + util::toString(id)),
  shape(shape_),
  shapeTiler(([&] {
    const double baseTolerance = 3;
    const uint16_t extent = 4096;
    const double tolerance = baseTolerance / ((1 << maxZoom) * extent);

    ProjectedGeometryContainer rings;
    std::vector<LonLat> points;

    for (size_t i = 0; i < shape.segments[0].size(); ++i) { // first segment for now (no holes)
        const double constraintedLatitude = ::fmin(::fmax(shape.segments[0][i].latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);
        points.push_back(LonLat(shape.segments[0][i].longitude, constraintedLatitude));
    }

    ProjectedFeatureType featureType;

    if (shape.styleProperties.is<FillProperties>()) {
        featureType = ProjectedFeatureType::Polygon;

        if (points.front().lon != points.back().lon || points.front().lat != points.back().lat) {
            points.push_back(LonLat(points.front().lon, points.front().lat));
        }
    } else {
        featureType = ProjectedFeatureType::LineString;
    }

    ProjectedGeometryContainer ring = Convert::project(points, tolerance);
    rings.members.push_back(ring);

    std::vector<ProjectedFeature> features;
    features.push_back(Convert::create(Tags(), featureType, rings));
    return features;
  })(), maxZoom, 4, 100, 10) {}

void ShapeAnnotationImpl::updateStyle(Style& style) {
    if (style.getLayer(layerID))
        return;

    ClassProperties paintProperties;

    if (shape.styleProperties.is<LineProperties>()) {
        const LineProperties& lineProperties = shape.styleProperties.get<LineProperties>();
        paintProperties.set(PropertyKey::LineOpacity, ConstantFunction<float>(lineProperties.opacity));
        paintProperties.set(PropertyKey::LineWidth, ConstantFunction<float>(lineProperties.width));
        paintProperties.set(PropertyKey::LineColor, ConstantFunction<Color>(lineProperties.color));
    } else if (shape.styleProperties.is<FillProperties>()) {
        const FillProperties& fillProperties = shape.styleProperties.get<FillProperties>();
        paintProperties.set(PropertyKey::FillOpacity, ConstantFunction<float>(fillProperties.opacity));
        paintProperties.set(PropertyKey::FillColor, ConstantFunction<Color>(fillProperties.fill_color));
        paintProperties.set(PropertyKey::FillOutlineColor, ConstantFunction<Color>(fillProperties.stroke_color));
    }

    std::map<ClassID, ClassProperties> shapePaints;
    shapePaints.emplace(ClassID::Default, std::move(paintProperties));
    std::unique_ptr<StyleLayer> shapeLayer = std::make_unique<StyleLayer>(layerID, std::move(shapePaints));
    shapeLayer->type = (shape.styleProperties.is<LineProperties>() ? StyleLayerType::Line : StyleLayerType::Fill);

    util::ptr<StyleBucket> shapeBucket = std::make_shared<StyleBucket>(shapeLayer->type);
    shapeBucket->name = shapeLayer->id;
    shapeBucket->source = AnnotationManager::SourceID;
    shapeBucket->source_layer = shapeLayer->id;
    if (shape.styleProperties.is<LineProperties>()) {
        shapeBucket->layout.set(PropertyKey::LineJoin, ConstantFunction<JoinType>(JoinType::Round));
    }

    shapeLayer->bucket = shapeBucket;
    style.addLayer(std::move(shapeLayer), AnnotationManager::PointLayerID);
}

void ShapeAnnotationImpl::updateTile(const TileID& tileID, AnnotationTile& tile) {
    const auto& shapeTile = shapeTiler.getTile(tileID.z, tileID.x, tileID.y);
    if (!shapeTile)
        return;

    AnnotationTileLayer& layer = *tile.layers.emplace(layerID,
        std::make_unique<AnnotationTileLayer>()).first->second;

    for (auto& shapeFeature : shapeTile.features) {
        FeatureType featureType = FeatureType::Unknown;

        if (shapeFeature.type == TileFeatureType::LineString) {
            featureType = FeatureType::LineString;
        } else if (shapeFeature.type == TileFeatureType::Polygon) {
            featureType = FeatureType::Polygon;
        }

        assert(featureType != FeatureType::Unknown);

        GeometryCollection renderGeometry;
        for (auto& shapeGeometry : shapeFeature.geometry) {
            std::vector<Coordinate> renderLine;
            auto& shapeRing = shapeGeometry.get<TileRing>();

            for (auto& shapePoint : shapeRing.points) {
                renderLine.emplace_back(shapePoint.x, shapePoint.y);
            }

            renderGeometry.push_back(renderLine);
        }

        layer.features.emplace_back(
            std::make_shared<AnnotationTileFeature>(featureType, renderGeometry));
    }
}

LatLngBounds ShapeAnnotationImpl::bounds() const {
    LatLngBounds result;

    for (const auto& segment : shape.segments) {
        for (const auto& point : segment) {
            result.extend(point);
        }
    }

    return result;
}

}
