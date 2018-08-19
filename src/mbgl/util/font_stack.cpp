#include <mbgl/util/font_stack.hpp>

#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/join.hpp>

namespace mbgl {

std::string fontStackToString(const FontStack& fontStack) {
    return boost::algorithm::join(fontStack, ",");
}

std::size_t FontStackHash::operator()(const FontStack& fontStack) const {
    return boost::hash_range(fontStack.begin(), fontStack.end());
}

} // namespace mbgl
