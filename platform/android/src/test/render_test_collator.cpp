#include <mbgl/i18n/collator.hpp>

#include <memory>

namespace mbgl {
namespace platform {

class Collator::Impl {
public:
    Impl(bool caseSensitive_, bool diacriticSensitive_, optional<std::string>)
        : caseSensitive(caseSensitive_), diacriticSensitive(diacriticSensitive_) {}

    bool operator==(const Impl& other) const { return true; }

    int compare(const std::string& lhs, const std::string& rhs) const { return 0; }

    std::string resolvedLocale() const { return ""; }

private:
    bool caseSensitive;
    bool diacriticSensitive;
};

Collator::Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale_)
    : impl(std::make_shared<Impl>(caseSensitive, diacriticSensitive, std::move(locale_))) {}

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
