#include <mbgl/style/expression/collator.hpp>

namespace mbgl {
namespace style {
namespace expression {

Collator::Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale)
    : collator(platform::Collator(caseSensitive, diacriticSensitive, std::move(locale))) {}

bool Collator::operator==(const Collator& other) const {
    return collator == other.collator;
}

int Collator::compare(const std::string& lhs, const std::string& rhs) const {
    return collator.compare(lhs, rhs);
}

std::string Collator::resolvedLocale() const {
    return collator.resolvedLocale();
}

} // namespace expression
} // namespace style
} // namespace mbgl
