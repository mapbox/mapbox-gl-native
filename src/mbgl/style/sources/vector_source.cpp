#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource_error.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset_)
    : Source(makeMutable<Impl>(std::move(id))),
      urlOrTileset(std::move(urlOrTileset_)) {
}

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
        return;
    }

    if (req) {
        return;
    }

    const std::string& url = urlOrTileset.get<std::string>();
    req = fileSource.request(Resource::source(url), [this, url](Response res) {
        if (!res.error && res.notModified) {
            return;
        } else if (res.error || res.noContent) {
            const auto severity = loaded ? EventSeverity::Warning : EventSeverity::Error;
            const std::string& message =
                res.error ? res.error->message : "unexpectedly empty TileJSON";
            const util::ResourceError err(message, ResourceKind::Source, res.error->status, url);
            observer->onSourceError(*this, std::make_exception_ptr(err), severity);
        } else {
            conversion::Error error;
            optional<Tileset> tileset = conversion::convertJSON<Tileset>(*res.data, error);
            if (!tileset) {
                const auto severity = loaded ? EventSeverity::Warning : EventSeverity::Error;
                observer->onSourceError(
                    *this, std::make_exception_ptr(std::runtime_error(error.message)), severity);
                return;
            }

            util::mapbox::canonicalizeTileset(*tileset, url, getType(), util::tileSize);
            bool changed = impl().getTileset() != *tileset;

            baseImpl = makeMutable<Impl>(impl(), *tileset);
            loaded = true;

            observer->onSourceLoaded(*this);

            if (changed) {
                observer->onSourceChanged(*this);
            }
        }
    });
}

} // namespace style
} // namespace mbgl
