#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/mapbox.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset_, optional<float> maxZoom_,
                           optional<float> minZoom_)
    : Source(makeMutable<Impl>(std::move(id))),
      urlOrTileset(std::move(urlOrTileset_)),
      maxZoom(std::move(maxZoom_)),
      minZoom(std::move(minZoom_)) {}

VectorSource::~VectorSource() = default;

const VectorSource::Impl& VectorSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

const variant<std::string, Tileset>& VectorSource::getURLOrTileset() const {
    return urlOrTileset;
}

optional<std::string> VectorSource::getURL() const {
    if (urlOrTileset.is<Tileset>()) {
        return {};
    }

    return urlOrTileset.get<std::string>();
}

void VectorSource::loadDescription(FileSource& fileSource) {
    if (urlOrTileset.is<Tileset>()) {
        baseImpl = makeMutable<Impl>(impl(), urlOrTileset.get<Tileset>());
        loaded = true;
        observer->onSourceLoaded(*this);
        return;
    }

    if (req) {
        return;
    }

    const auto& url = urlOrTileset.get<std::string>();
    req = fileSource.request(Resource::source(url), [this, url](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            conversion::Error error;
            optional<Tileset> tileset = conversion::convertJSON<Tileset>(*res.data, error);
            if (!tileset) {
                observer->onSourceError(*this, std::make_exception_ptr(util::StyleParseException(error.message)));
                return;
            }
            if (maxZoom) {
                tileset->zoomRange.max = *maxZoom;
            }
            if (minZoom) {
                tileset->zoomRange.min = *minZoom;
            }
            util::mapbox::canonicalizeTileset(*tileset, url, getType(), util::tileSize);
            bool changed = impl().tileset != *tileset;

            baseImpl = makeMutable<Impl>(impl(), *tileset);
            loaded = true;

            observer->onSourceLoaded(*this);

            if (changed) {
                observer->onSourceChanged(*this);
            }
        }
    });
}

bool VectorSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return !std::strcmp(info->type, "line") || !std::strcmp(info->type, "symbol") ||
           !std::strcmp(info->type, "circle") || !std::strcmp(info->type, "fill") ||
           !std::strcmp(info->type, "fill-extrusion") || !std::strcmp(info->type, "heatmap");
}

} // namespace style
} // namespace mbgl
