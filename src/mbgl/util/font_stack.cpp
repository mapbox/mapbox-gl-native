#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/logging.hpp>
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
        if (layer->type != LayerType::Symbol) {
            continue;
        }

        const SymbolLayer::Impl& impl = dynamic_cast<const SymbolLayer::Impl&>(*layer);
        if (impl.layout.get<TextField>().isUndefined()) {
            continue;
        }

        impl.layout.get<TextFont>().match(
            [&] (Undefined) {
                result.insert({"Open Sans Regular", "Arial Unicode MS Regular"});
            },
            [&] (const FontStack& constant) {
                result.insert(constant);
            },
            [&] (const auto& function) {
                for (const auto& value : function.possibleOutputs()) {
                    if (value) {
                        result.insert(*value);
                    } else {
                        Log::Warning(Event::ParseStyle, "Layer '%s' has an invalid value for text-font and will not render text. Output values must be contained as literals within the expression.", impl.id.c_str());
                        break;
                    }
                }
            }
        );
    }

    return result;
}

} // namespace mbgl
