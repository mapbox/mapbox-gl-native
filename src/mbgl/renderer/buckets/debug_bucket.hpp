#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/fill_program.hpp>

namespace mbgl {

class OverscaledTileID;

namespace gl {
class UploadPass;
} // namespace gl

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(const OverscaledTileID& id,
                bool renderable,
                bool complete,
                optional<Timestamp> modified,
                optional<Timestamp> expires,
                MapDebugOptions);

    void upload(gfx::UploadPass&);

    const bool renderable;
    const bool complete;
    const optional<Timestamp> modified;
    const optional<Timestamp> expires;
    const MapDebugOptions debugMode;


    gfx::VertexVector<FillLayoutVertex> vertices;
    gfx::IndexVector<gfx::Lines> indices;

    SegmentVector<DebugAttributes> segments;
    optional<gfx::VertexBuffer<DebugLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> indexBuffer;
    const std::string drawScopeID;
};

} // namespace mbgl
