#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {
namespace style {

Source::Source(SourceType type_, std::unique_ptr<Impl> baseImpl_)
    : baseImpl(std::move(baseImpl_)), type(type_) {
}

Source::~Source() = default;

const std::string& Source::getID() const {
    return baseImpl->id;
}

optional<std::string> Source::getAttribution() const {
    return baseImpl->getAttribution();
}

optional<Range<uint8_t>> Source::getZoomRange() const {
    return baseImpl->getZoomRange();
}

} // namespace style
} // namespace mbgl
