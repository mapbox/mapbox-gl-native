#pragma once

#include <mbgl/i18n/collator.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace expression {

class Collator {
public:
    Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale = {});

    bool operator==(const Collator& other) const;

    int compare(const std::string& lhs, const std::string& rhs) const;

    std::string resolvedLocale() const;
private:
    platform::Collator collator;
};

} // namespace expression
} // namespace style
} // namespace mbgl
