#include <mbgl/util/font_stack.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/hash.hpp>

#include <boost/algorithm/string/join.hpp>

namespace mbgl {

using namespace style;

std::string fontStackToString(const FontStack& fontStack) {
    return boost::algorithm::join(fontStack, ",");
}

FontStackHash FontStackHasher::operator()(const FontStack& fontStack) const {
    std::size_t seed = 0;
    for (const auto& font : fontStack) {
        util::hash_combine(seed, font);
    }
    return seed;
}

std::set<FontStack> fontStacks(const std::vector<Immutable<style::Layer::Impl>>& layers) {
    std::set<FontStack> result;
    for (const auto& layer : layers) {
        layer->populateFontStack(result);
    }

    return result;
}

} // namespace mbgl
