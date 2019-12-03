#pragma once

#include <mbgl/util/optional.hpp>

#include <memory>
#include <string>

namespace mbgl {
namespace platform {

class Collator {
public:
    explicit Collator(bool caseSensitive, bool diacriticSensitive, optional<std::string> locale = nullopt);
    int compare(const std::string& lhs, const std::string& rhs) const;
    std::string resolvedLocale() const;
    bool operator==(const Collator& other) const;

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace platform
} // namespace mbgl
