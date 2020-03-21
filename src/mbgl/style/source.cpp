#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

static SourceObserver nullObserver;

Source::Source(Immutable<Impl> impl)
    : baseImpl(std::move(impl)),
      observer(&nullObserver) {
}

Source::~Source() = default;

SourceType Source::getType() const {
    return baseImpl->type;
}

std::string Source::getID() const {
    return baseImpl->id;
}

optional<std::string> Source::getAttribution() const {
    return baseImpl->getAttribution();
}

void Source::setObserver(SourceObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

void Source::setPrefetchZoomDelta(optional<uint8_t> delta) noexcept {
    if (getPrefetchZoomDelta() == delta) return;
    auto newImpl = createMutable();
    newImpl->setPrefetchZoomDelta(std::move(delta));
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

optional<uint8_t> Source::getPrefetchZoomDelta() const noexcept {
    return baseImpl->getPrefetchZoomDelta();
}

void Source::setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept {
    if (getMaxOverscaleFactorForParentTiles() == overscaleFactor) return;
    auto newImpl = createMutable();
    newImpl->setMaxOverscaleFactorForParentTiles(std::move(overscaleFactor));
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

optional<uint8_t> Source::getMaxOverscaleFactorForParentTiles() const noexcept {
    return baseImpl->getMaxOverscaleFactorForParentTiles();
}

void Source::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", getID().c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);
}

} // namespace style
} // namespace mbgl
