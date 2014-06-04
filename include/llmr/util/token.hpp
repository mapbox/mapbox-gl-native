#ifndef LLMR_UTIL_TOKEN
#define LLMR_UTIL_TOKEN

#ifdef __linux__
#include <boost/regex.hpp>
namespace regex_impl = boost;
#else
#include <regex>
namespace regex_impl = std;
#endif

namespace llmr {
namespace util {

namespace detail {
const regex_impl::regex tokenRegex("\\{\\{(\\w+)\\}\\}");
const regex_impl::sregex_token_iterator tokensEnd = regex_impl::sregex_token_iterator();
}

template <typename Tokens>
std::string replaceTokens(const std::string &source, const Tokens &tokens) {
    std::string result;
    result.reserve(source.size());

    bool token = false;
    for (auto token_it = regex_impl::sregex_token_iterator(source.begin(), source.end(),
                                                           detail::tokenRegex, {-1, 1});
         token_it != detail::tokensEnd; ++token_it, token = !token) {
        if (!token_it->matched) {
            continue;
        }

        if (token) {
            const auto it_prop = tokens.find(token_it->str());
            if (it_prop != tokens.end()) {
                result += it_prop->second;
            }
        } else {
            result += token_it->str();
        }
    }

    return result;
}

} // end namespace util
} // end namespace llmr

#endif
