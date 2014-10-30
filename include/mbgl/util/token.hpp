#ifndef MBGL_UTIL_TOKEN
#define MBGL_UTIL_TOKEN

#include <map>
#include <string>
#include <algorithm>

namespace mbgl {
namespace util {

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
            for (brace++; brace != end && std::isalnum(*brace); brace++);
            if (brace != end && *brace == '}') {
                result.append(lookup({ pos + 1, brace }));
                pos = brace + 1;
            } else {
                result.append(pos, brace);
                pos = brace;
            }
        }
    }

    return result;
}

template <typename T>
inline std::string replaceTokens(const std::string &source, const std::map<std::string, T> &properties) {
    return replaceTokens(source, [&properties](const std::string &token) -> std::string {
        const auto it_prop = properties.find(token);
        return it_prop != properties.end() ? toString(it_prop->second) : "";
    });
}

} // end namespace util
} // end namespace mbgl

#endif
