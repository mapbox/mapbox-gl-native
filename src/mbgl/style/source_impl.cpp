#include <mbgl/style/source_impl.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

namespace {
void WarnIfOverscaleFactorCapsPrefetchDelta(const optional<uint8_t>& overscale, const optional<uint8_t>& prefetch) {
    const uint8_t prefetchDelta = std::max<uint8_t>(util::DEFAULT_PREFETCH_ZOOM_DELTA, prefetch.value_or(0u));
    if (overscale && *overscale < prefetchDelta) {
        Log::Warning(Event::Style, "Parent tile overscale factor will cap prefetch delta to %d", int(*overscale));
    }
}
} // namespace

Source::Impl::Impl(SourceType type_, std::string id_)
    : type(type_),
      id(std::move(id_)) {
}

void Source::Impl::setPrefetchZoomDelta(optional<uint8_t> delta) noexcept {
    prefetchZoomDelta = std::move(delta);
    WarnIfOverscaleFactorCapsPrefetchDelta(maxOverscaleFactor, prefetchZoomDelta);
}

optional<uint8_t> Source::Impl::getPrefetchZoomDelta() const noexcept {
    return prefetchZoomDelta;
}

void Source::Impl::setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept {
    maxOverscaleFactor = std::move(overscaleFactor);
    WarnIfOverscaleFactorCapsPrefetchDelta(maxOverscaleFactor, prefetchZoomDelta);
}

optional<uint8_t> Source::Impl::getMaxOverscaleFactorForParentTiles() const noexcept {
    return maxOverscaleFactor;
}

} // namespace style
} // namespace mbgl
