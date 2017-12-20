#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

namespace mapbox {
namespace util {
template <>
struct nth<0, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) {
        return t.x;
    };
};

template <>
struct nth<1, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) {
        return t.y;
    };
};
} // namespace util
} // namespace mapbox

namespace mbgl {

using namespace style;

struct GeometryTooLongException : std::exception {};

FillExtrusionBucket::FillExtrusionBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(std::piecewise_construct,
                                     std::forward_as_tuple(layer->getID()),
                                     std::forward_as_tuple(
                                                           layer->as<RenderFillExtrusionLayer>()->evaluated,
                                                           parameters.tileID.overscaledZ));
    }
}

void FillExtrusionBucket::addFeature(const GeometryTileFeature& feature,
                                     const GeometryCollection& geometry) {
    for (auto& polygon : classifyRings(geometry)) {
        // Optimize polygons with many interior rings for earcut tesselation.
        limitHoles(polygon, 500);

        std::size_t totalVertices = 0;

        for (const auto& ring : polygon) {
            totalVertices += ring.size();
            if (totalVertices > std::numeric_limits<uint16_t>::max())
                throw GeometryTooLongException();
        }

        if (totalVertices == 0) continue;

        std::vector<uint32_t> flatIndices;
        flatIndices.reserve(totalVertices);

        std::size_t startVertices = vertices.vertexSize();

        if (triangleSegments.empty() ||
            triangleSegments.back().vertexLength + (5 * (totalVertices - 1) + 1) >
                std::numeric_limits<uint16_t>::max()) {
            triangleSegments.emplace_back(startVertices, triangles.indexSize());
        }

        auto& triangleSegment = triangleSegments.back();
        assert(triangleSegment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t triangleIndex = triangleSegment.vertexLength;

        assert(triangleIndex + (5 * (totalVertices - 1) + 1) <=
               std::numeric_limits<uint16_t>::max());

        for (const auto& ring : polygon) {
            std::size_t nVertices = ring.size();

            if (nVertices == 0)
                continue;

            auto edgeDistance = 0;

            for (uint32_t i = 0; i < nVertices; i++) {
                const auto& p1 = ring[i];

                vertices.emplace_back(
                    FillExtrusionProgram::layoutVertex(p1, 0, 0, 1, 1, edgeDistance));
                flatIndices.emplace_back(triangleIndex);
                triangleIndex++;

                if (i != 0) {
                    const auto& p2 = ring[i - 1];

                    const auto d1 = convertPoint<double>(p1);
                    const auto d2 = convertPoint<double>(p2);

                    const Point<double> perp = util::unit(util::perp(d1 - d2));
                    const auto dist = util::dist<int16_t>(d1, d2);
                    if (dist > std::numeric_limits<int16_t>::max()) {
                        edgeDistance = 0;
                    }

                    vertices.emplace_back(
                        FillExtrusionProgram::layoutVertex(p1, perp.x, perp.y, 0, 0, edgeDistance));
                    vertices.emplace_back(
                        FillExtrusionProgram::layoutVertex(p1, perp.x, perp.y, 0, 1, edgeDistance));

                    edgeDistance += dist;

                    vertices.emplace_back(
                        FillExtrusionProgram::layoutVertex(p2, perp.x, perp.y, 0, 0, edgeDistance));
                    vertices.emplace_back(
                        FillExtrusionProgram::layoutVertex(p2, perp.x, perp.y, 0, 1, edgeDistance));

                    triangles.emplace_back(triangleIndex, triangleIndex + 1, triangleIndex + 2);
                    triangles.emplace_back(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);
                    triangleIndex += 4;
                    triangleSegment.vertexLength += 4;
                    triangleSegment.indexLength += 6;
                }
            }
        }

        std::vector<uint32_t> indices = mapbox::earcut(polygon);

        std::size_t nIndices = indices.size();
        assert(nIndices % 3 == 0);

        for (uint32_t i = 0; i < nIndices; i += 3) {
            triangles.emplace_back(flatIndices[indices[i]], flatIndices[indices[i + 1]],
                                   flatIndices[indices[i + 2]]);
        }

        triangleSegment.vertexLength += totalVertices;
        triangleSegment.indexLength += nIndices;
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.populateVertexVectors(feature, vertices.vertexSize());
    }
}

void FillExtrusionBucket::upload(gl::Context& context) {
    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    indexBuffer = context.createIndexBuffer(std::move(triangles));

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(context);
    }

    uploaded = true;
}

bool FillExtrusionBucket::hasData() const {
    return !triangleSegments.empty();
}

float FillExtrusionBucket::getQueryRadius(const RenderLayer& layer) const {
    if (!layer.is<RenderFillExtrusionLayer>()) {
        return 0;
    }

    const std::array<float, 2>& translate = layer.as<RenderFillExtrusionLayer>()->evaluated.get<FillExtrusionTranslate>();
    return util::length(translate[0], translate[1]);
}

} // namespace mbgl
