#pragma once

#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <memory>

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
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace expression
} // namespace style
} // namespace mbgl
