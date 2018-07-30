#pragma once

#include <mbgl/util/optional.hpp>

#include <map>
#include <string>
#include <algorithm>

namespace mbgl {
namespace util {

const static std::string tokenReservedChars = "{}";

// Replaces {tokens} in a string by calling the lookup function.
template <typename Lookup>
std::string replaceTokens(const std::string &source, const Lookup &lookup) {
    std::string result;
    result.reserve(source.size());

    auto pos = source.begin();
    const auto end = source.end();

    while (pos != end) {
        auto brace = std::find(pos, end, '{');
        result.append(pos, brace);
        pos = brace;
        if (pos != end) {
            for (brace++; brace != end && tokenReservedChars.find(*brace) == std::string::npos; brace++);
            if (brace != end && *brace == '}') {
                std::string key { pos + 1, brace };
                if (optional<std::string> replacement = lookup(key)) {
                    result.append(*replacement);
                } else {
                    result.append("{");
                    result.append(key);
                    result.append("}");
                }
                pos = brace + 1;
            } else {
                result.append(pos, brace);
                pos = brace;
            }
        }
    }

    return result;
}

} // end namespace util
} // end namespace mbgl
