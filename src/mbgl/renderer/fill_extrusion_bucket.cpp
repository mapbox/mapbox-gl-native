#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

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

FillExtrusionBucket::FillExtrusionBucket(const BucketParameters& parameters, const std::vector<const Layer*>& layers) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(layer->getID(),
                                     FillExtrusionProgram::PaintPropertyBinders(
                                                                       layer->as<FillExtrusionLayer>()->impl->paint.evaluated,
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

        // COPIED: do we need
//        std:vector<GLsizei> flatIndices;
//        flatIndices.reserve(totalVertices);

// TODO this was all a lot of copying; make sure segment sizing/creation happens correctly

        std::size_t startVertices = vertices.vertexSize();

        if (triangleSegments.empty() || triangleSegments.back().vertexLength + totalVertices > std::numeric_limits<uint16_t>::max()) {
            triangleSegments.emplace_back(startVertices, triangles.indexSize());
        }

        auto& triangleSegment = triangleSegments.back();
        assert(triangleSegment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t triangleIndex = triangleSegment.vertexLength;

        for (const auto& ring : polygon) {
            std::size_t nVertices = ring.size();

            if (nVertices == 0)
                continue;

            auto edgeDistance = 0;

            for (uint32_t i = 0; i < nVertices; i++) {
                const auto& p1 = ring[i];

                // add vertex + vertex index

                if (i != 0) {
                    const auto& p2 = ring[i - 1];

                    if (!isBoundaryEdge(p1, p2)) {
                        const auto d1 = convertPoint<double>(p1);
                        const auto d2 = convertPoint<double>(p2);

                        const Point<double> perp = util::unit(util::perp(d2 - d1));

                        vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp.x, perp.y, 0, 0, edgeDistance));
                        vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp.x, perp.y, 0, 1, edgeDistance));

                        edgeDistance += util::dist<int16_t>(d1, d2);

                        vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp.x, perp.y, 0, 0, edgeDistance));
                        vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp.x, perp.y, 0, 1, edgeDistance));

                        triangles.emplace_back(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);
                        triangles.emplace_back(triangleIndex + 2, triangleIndex + 3, triangleIndex + 4);
                        triangleIndex += 4;
                        triangleSegment.vertexLength += 4;
                        triangleSegment.indexLength += 2;
                    }
                }
            }
        }

        std::vector<uint32_t> indices = mapbox::earcut(polygon);

        std::size_t nIndicies = indices.size();
        assert(nIndicies % 3 == 0);

        for (uint32_t i = 0; i < nIndicies; i += 3) {
            triangles.emplace_back(triangleIndex + indices[i],
                                   triangleIndex + indices[i + 1],
                                   triangleIndex + indices[i + 2]);
        }

        triangleSegment.vertexLength += totalVertices;
        triangleSegment.indexLength += nIndicies;
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

void FillExtrusionBucket::render(Painter& painter,
                        PaintParameters& parameters,
                        const Layer& layer,
                        const RenderTile& tile) {
    painter.renderFillExtrusion(parameters, *this, *layer.as<FillExtrusionLayer>(), tile);
}

bool FillExtrusionBucket::hasData() const {
    return !triangleSegments.empty();
}

bool isBoundaryEdge(const Point<int16_t>& p1,
                    const Point<int16_t>& p2) {
    // TODO TODO is this even still necessary now that we (a) render to texture and (b) don't have outlines?? investigate -- it's always been kind of hacky anyway
    return (p1.x == p2.x && (p1.x < 0 || p1.x > util::EXTENT)) ||
        (p1.y == p2.y && (p1.y < 0 || p1.y > util::EXTENT));
}

} // namespace mbgl
