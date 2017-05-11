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

void Source::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", getID().c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);
}

} // namespace style
} // namespace mbgl
