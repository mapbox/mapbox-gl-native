#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/logging.hpp>

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

void RenderImageSource::startRender(Painter& painter) {
    if (!isLoaded()) {
        return;
    }

    matrices.clear();

    for (size_t i = 0; i < tileIds.size(); i++) {
        mat4 matrix;
        matrix::identity(matrix);
        painter.state.matrixFor(matrix, tileIds[i]);
        matrix::multiply(matrix, painter.projMatrix, matrix);
        matrices.push_back(matrix);
    }

    if (bucket->needsUpload()) {
        bucket->upload(painter.context);
    }
}

void RenderImageSource::finishRender(Painter& painter) {
    if (!isLoaded() || !(painter.frame.debugOptions & MapDebugOptions::TileBorders)) {
        return;
    }

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    for (auto matrix : matrices) {
        painter.programs->debug.draw(
            painter.context,
            gl::LineStrip { 4.0f * painter.frame.pixelRatio },
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
             uniforms::u_matrix::Value{ matrix },
             uniforms::u_color::Value{ Color::red() }
            },
            painter.tileVertexBuffer,
            painter.tileBorderIndexBuffer,
            painter.tileBorderSegments,
            paintAttibuteData,
            properties,
            painter.state.getZoom(),
            "debug"
        );
    }
}

std::unordered_map<std::string, std::vector<Feature>>
RenderImageSource::queryRenderedFeatures(const ScreenLineString&,
                                         const TransformState&,
                                         const RenderStyle&,
                                         const RenderedQueryOptions&) const {
    return std::unordered_map<std::string, std::vector<Feature>>();
}

std::vector<Feature> RenderImageSource::querySourceFeatures(const SourceQueryOptions&) const {
    return std::vector<Feature>();
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

    auto size = transformState.getSize();
    const double viewportHeight = size.height;

    // Compute the screen coordinates at wrap=0 for the given LatLng
    ScreenCoordinate nePixel = { -INFINITY, -INFINITY };
    ScreenCoordinate swPixel = { INFINITY, INFINITY };

    for (LatLng latLng : coords) {
        ScreenCoordinate pixel = transformState.latLngToScreenCoordinate(latLng);
        swPixel.x = std::min(swPixel.x, pixel.x);
        nePixel.x = std::max(nePixel.x, pixel.x);
        swPixel.y = std::min(swPixel.y, viewportHeight - pixel.y);
        nePixel.y = std::max(nePixel.y, viewportHeight - pixel.y);
    }
    const double width = nePixel.x - swPixel.x;
    const double height = nePixel.y - swPixel.y;

    // Don't bother drawing the ImageSource unless it occupies >4 screen pixels
    enabled = (width * height > 4);
    if (!enabled) {
        return;
    }

    // Calculate the optimum zoom level to determine the tile ids to use for transforms
    double minScale = INFINITY;
    double scaleX = double(size.width) / width;
    double scaleY = double(size.height) / height;
    minScale = util::min(scaleX, scaleY);
    double zoom = transformState.getZoom() + util::log2(minScale);
    zoom = std::floor(util::clamp(zoom, transformState.getMinZoom(), transformState.getMaxZoom()));
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
    for (auto latLng : coords) {
        auto tc = TileCoordinate::fromLatLng(0, latLng);
        auto gc = TileCoordinate::toGeometryCoordinate(tileIds[0], tc.p);
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
        RasterProgram::layoutVertex({ geomCoords[1].x, geomCoords[1].y }, { 32767, 0 }));
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[3].x, geomCoords[3].y }, { 0, 32767 }));
    bucket->vertices.emplace_back(
        RasterProgram::layoutVertex({ geomCoords[2].x, geomCoords[2].y }, { 32767, 32767 }));

    bucket->indices.emplace_back(0, 1, 2);
    bucket->indices.emplace_back(1, 2, 3);

    bucket->segments.emplace_back(0, 0, 4, 6);
}

void RenderImageSource::dumpDebugLogs() const {
    Log::Info(Event::General, "RenderImageSource::id: %s", impl().id.c_str());
    Log::Info(Event::General, "RenderImageSource::loaded: %s", isLoaded() ? "yes" : "no");
}

} // namespace mbgl
