#include <mbgl/i18n/collator.hpp>

#include <memory>

namespace mbgl {
namespace platform {

class Collator::Impl {
public:
    Impl(optional<std::string>) {}

    bool operator==(const Impl&) const { return true; }

    int compare(const std::string&, const std::string&) const { return 0; }

    std::string resolvedLocale() const { return ""; }
};

Collator::Collator(bool, bool, optional<std::string> locale_) : impl(std::make_shared<Impl>(std::move(locale_))) {}

int Collator::compare(const std::string& lhs, const std::string& rhs) const {
    return impl->compare(lhs, rhs);
}

bool Collator::operator==(const Collator& other) const {
    return *impl == *(other.impl);
}

std::string Collator::resolvedLocale() const {
    return impl->resolvedLocale();
}

} // namespace platform
} // namespace mbgl
