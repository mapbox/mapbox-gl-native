#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

using namespace style;

RenderImageSource::RenderImageSource(Immutable<style::ImageSource::Impl> impl_)
    : RenderSource(impl_) {
}

RenderImageSource::~RenderImageSource() = default;

const style::ImageSource::Impl& RenderImageSource::impl() const {
    return static_cast<const style::ImageSource::Impl&>(*baseImpl);
}

bool RenderImageSource::isLoaded() const {
    return !!bucket;
}

void RenderImageSource::startRender(PaintParameters& parameters) {
    if (!isLoaded()) {
        return;
    }

    matrices.clear();

    for (size_t i = 0; i < tileIds.size(); i++) {
        mat4 matrix;
        matrix::identity(matrix);
        parameters.state.matrixFor(matrix, tileIds[i]);
        matrix::multiply(matrix, parameters.alignedProjMatrix, matrix);
        matrices.push_back(matrix);
    }

    if (bucket->needsUpload()) {
        bucket->upload(parameters.context);
    }
}

void RenderImageSource::finishRender(PaintParameters& parameters) {
    if (!isLoaded() || !(parameters.debugOptions & MapDebugOptions::TileBorders)) {
        return;
    }

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttributeData(properties, 0);

    for (auto matrix : matrices) {
        parameters.programs.debug.draw(
            parameters.context,
            gl::LineStrip { 4.0f * parameters.pixelRatio },
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
             uniforms::u_matrix::Value{ matrix },
             uniforms::u_color::Value{ Color::red() }
            },
            parameters.staticData.tileVertexBuffer,
            parameters.staticData.tileBorderIndexBuffer,
            parameters.staticData.tileBorderSegments,
            paintAttributeData,
            properties,
            parameters.state.getZoom(),
            "debug"
        );
    }
}

std::unordered_map<std::string, std::vector<Feature>>
RenderImageSource::queryRenderedFeatures(const ScreenLineString&,
                                         const TransformState&,
                                         const std::vector<const RenderLayer*>&,
                                         const RenderedQueryOptions&,
                                         const CollisionIndex&) const {
    return std::unordered_map<std::string, std::vector<Feature>> {};
}

std::vector<Feature> RenderImageSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderImageSource::update(Immutable<style::Source::Impl> baseImpl_,
                               const std::vector<Immutable<Layer::Impl>>&,
                               const bool needsRendering,
                               const bool,
                               const TileParameters& parameters) {
    enabled = needsRendering;
    if (!needsRendering) {
        return;
    }

    auto transformState = parameters.transformState;
    std::swap(baseImpl, baseImpl_);

    auto coords = impl().getCoordinates();
    std::shared_ptr<PremultipliedImage> image = impl().getImage();

    if (!image || !image->valid()) {
        enabled = false;
        return;
    }

    // Compute the z0 tile coordinates for the given LatLngs
    TileCoordinatePoint nePoint = { -INFINITY, -INFINITY };
    TileCoordinatePoint swPoint = { INFINITY, INFINITY };
    std::vector<TileCoordinatePoint> tileCoordinates;
    for (LatLng latLng : coords) {
        auto point = TileCoordinate::fromLatLng(0, latLng).p;
        tileCoordinates.push_back(point);
        swPoint.x = std::min(swPoint.x, point.x);
        nePoint.x = std::max(nePoint.x, point.x);
        swPoint.y = std::min(swPoint.y, point.y);
        nePoint.y = std::max(nePoint.y, point.y);
   }

    // Calculate the optimum zoom level to determine the tile ids to use for transforms
    auto dx = nePoint.x - swPoint.x;
    auto dy = nePoint.y - swPoint.y;
    auto dMax = std::max(dx, dy);
    double zoom = std::max(0.0, std::floor(-util::log2(dMax)));

    // Only enable if the long side of the image is > 2 pixels. Resulting in a
    // display of at least 2 x 1 px image
    // A tile coordinate unit represents the length of one tile (tileSize) at a given zoom.
    // To convert a tile coordinate to pixels, multiply by tileSize.
    // Here dMax is in z0 tile units, so we also scale by 2^z to match current zoom.
    enabled = dMax * std::pow(2.0, transformState.getZoom()) * util::tileSize > 2.0;
    if (!enabled) {
        return;
    }

    auto imageBounds = LatLngBounds::hull(coords[0], coords[1]);
    imageBounds.extend(coords[2]);
    imageBounds.extend(coords[3]);
    auto tileCover = util::tileCover(imageBounds, zoom);
    tileIds.clear();
    tileIds.push_back(tileCover[0]);

    bool hasVisibleTile = false;
    // Add additional wrapped tile ids if neccessary
    auto idealTiles = util::tileCover(transformState, transformState.getZoom());
    for (auto tile : idealTiles) {
        if (tile.wrap != 0 && tileCover[0].canonical.isChildOf(tile.canonical)) {
            tileIds.push_back({ tile.wrap, tileCover[0].canonical });
            hasVisibleTile = true;
        }
        else if (!hasVisibleTile) {
            for (auto coveringTile: tileCover) {
                if(coveringTile.canonical == tile.canonical ||
                    coveringTile.canonical.isChildOf(tile.canonical) ||
                    tile.canonical.isChildOf(coveringTile.canonical)) {
                    hasVisibleTile = true;
                }
            }
        }
    }

    enabled = hasVisibleTile;
    if (!enabled) {
        return;
    }

    // Calculate Geometry Coordinates based on tile cover at ideal zoom
    GeometryCoordinates geomCoords;
    for (auto tileCoords : tileCoordinates) {
        auto gc = TileCoordinate::toGeometryCoordinate(tileIds[0], tileCoords);
        geomCoords.push_back(gc);
    }
    if (!bucket) {
        bucket = std::make_unique<RasterBucket>(image);
    } else {
        bucket->clear();
        if (image != bucket->image) {
            bucket->setImage(image);
        }
    }

    // Set Bucket Vertices, Indices, and segments
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[0].x, geomCoords[0].y }, { 0, 0 }));
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[1].x, geomCoords[1].y }, { util::EXTENT, 0 }));
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[3].x, geomCoords[3].y }, { 0, util::EXTENT }));
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[2].x, geomCoords[2].y }, { util::EXTENT, util::EXTENT }));

    bucket->indices.emplace_back(0, 1, 2);
    bucket->indices.emplace_back(1, 2, 3);

    bucket->segments.emplace_back(0, 0, 4, 6);
}

void RenderImageSource::dumpDebugLogs() const {
    Log::Info(Event::General, "RenderImageSource::id: %s", impl().id.c_str());
    Log::Info(Event::General, "RenderImageSource::loaded: %s", isLoaded() ? "yes" : "no");
}

} // namespace mbgl
