#ifndef MBGL_UTIL_TOKEN
#define MBGL_UTIL_TOKEN

#ifdef __linux__
#include <boost/regex.hpp>
namespace regex_impl = boost;
#else
#include <regex>
namespace regex_impl = std;
#endif

#include <map>

namespace mbgl {
namespace util {

namespace detail {
const regex_impl::regex tokenRegex("\\{([\\w-]+)\\}");
const regex_impl::sregex_token_iterator tokensEnd = regex_impl::sregex_token_iterator();
}

template <typename Lookup>
std::string replaceTokens(const std::string &source, const Lookup &lookup) {
    std::string result;
    result.reserve(source.size());

    bool token = false;
    for (auto token_it = regex_impl::sregex_token_iterator(source.begin(), source.end(),
                                                           detail::tokenRegex, {-1, 1});
        token_it != detail::tokensEnd; ++token_it, token = !token) {
        if (!token_it->matched) {
            continue;
        }

        result += token ? lookup(token_it->str()) : token_it->str();
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
