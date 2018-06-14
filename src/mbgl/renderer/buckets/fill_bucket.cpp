#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/util/math.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

namespace mapbox {
namespace util {
template <> struct nth<0, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) { return t.x; };
};

template <> struct nth<1, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) { return t.y; };
};
} // namespace util
} // namespace mapbox

namespace mbgl {

using namespace style;

struct GeometryTooLongException : std::exception {};

FillBucket::FillBucket(const FillBucket::PossiblyEvaluatedLayoutProperties,
                       std::map<std::string, FillBucket::PossiblyEvaluatedPaintProperties> layerPaintProperties,
                       const float zoom,
                       const uint32_t)
    : Bucket(LayerType::Fill) {

    for (const auto& pair : layerPaintProperties) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(
                pair.second,
                zoom));
    }
}

void FillBucket::addFeature(const GeometryTileFeature& feature,
                            const GeometryCollection& geometry,
                            const ImagePositions& patternPositions,
                            const PatternLayerMap& patternDependencies) {
    for (auto& polygon : classifyRings(geometry)) {
        // Optimize polygons with many interior rings for earcut tesselation.
        limitHoles(polygon, 500);

        std::size_t totalVertices = 0;

        for (const auto& ring : polygon) {
            totalVertices += ring.size();
            if (totalVertices > std::numeric_limits<uint16_t>::max())
                throw GeometryTooLongException();
        }

        std::size_t startVertices = vertices.vertexSize();

        for (const auto& ring : polygon) {
            std::size_t nVertices = ring.size();

            if (nVertices == 0)
                continue;

            if (lineSegments.empty() || lineSegments.back().vertexLength + nVertices > std::numeric_limits<uint16_t>::max()) {
                lineSegments.emplace_back(vertices.vertexSize(), lines.indexSize());
            }

            auto& lineSegment = lineSegments.back();
            assert(lineSegment.vertexLength <= std::numeric_limits<uint16_t>::max());
            uint16_t lineIndex = lineSegment.vertexLength;

            vertices.emplace_back(FillProgram::layoutVertex(ring[0]));
            lines.emplace_back(lineIndex + nVertices - 1, lineIndex);

            for (uint32_t i = 1; i < nVertices; i++) {
                vertices.emplace_back(FillProgram::layoutVertex(ring[i]));
                lines.emplace_back(lineIndex + i - 1, lineIndex + i);
            }

            lineSegment.vertexLength += nVertices;
            lineSegment.indexLength += nVertices * 2;
        }

        std::vector<uint32_t> indices = mapbox::earcut(polygon);

        std::size_t nIndicies = indices.size();
        assert(nIndicies % 3 == 0);

        if (triangleSegments.empty() || triangleSegments.back().vertexLength + totalVertices > std::numeric_limits<uint16_t>::max()) {
            triangleSegments.emplace_back(startVertices, triangles.indexSize());
        }

        auto& triangleSegment = triangleSegments.back();
        assert(triangleSegment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t triangleIndex = triangleSegment.vertexLength;

        for (uint32_t i = 0; i < nIndicies; i += 3) {
            triangles.emplace_back(triangleIndex + indices[i],
                                   triangleIndex + indices[i + 1],
                                   triangleIndex + indices[i + 2]);
        }

        triangleSegment.vertexLength += totalVertices;
        triangleSegment.indexLength += nIndicies;
    }

    for (auto& pair : paintPropertyBinders) {
        const auto it = patternDependencies.find(pair.first);
        if (it != patternDependencies.end()){
            pair.second.populateVertexVectors(feature, vertices.vertexSize(), patternPositions, it->second);
        } else {
            pair.second.populateVertexVectors(feature, vertices.vertexSize(), patternPositions, {});
        }
    }
}

void FillBucket::upload(gl::Context& context) {
    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    lineIndexBuffer = context.createIndexBuffer(std::move(lines));
    triangleIndexBuffer = context.createIndexBuffer(std::move(triangles));

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(context);
    }

    uploaded = true;
}

bool FillBucket::hasData() const {
    return !triangleSegments.empty() || !lineSegments.empty();
}

float FillBucket::getQueryRadius(const RenderLayer& layer) const {
    if (!layer.is<RenderFillLayer>()) {
        return 0;
    }

    const std::array<float, 2>& translate = layer.as<RenderFillLayer>()->evaluated.get<FillTranslate>();
    return util::length(translate[0], translate[1]);

}

} // namespace mbgl
